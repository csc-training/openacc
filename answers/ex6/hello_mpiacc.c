#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#ifdef _OPENACC
#include <openacc.h>
#endif

void getNodeInfo(char *nodeName, int *nodeRank, int *nodeProcs,
                 int *devCount)
{
    unsigned hostid;
    int nodenamelen;
    MPI_Comm intranodecomm;
#ifdef _OPENACC
    acc_device_t accdevtype;
#endif

    hostid = gethostid();
    MPI_Get_processor_name(nodeName, &nodenamelen);
    MPI_Comm_split(MPI_COMM_WORLD, 0x7FFFFFFF & hostid, 0, &intranodecomm);

    MPI_Comm_rank(intranodecomm, nodeRank);
    MPI_Comm_size(intranodecomm, nodeProcs);

    MPI_Comm_free(&intranodecomm);
    *devCount = 0;
#ifdef _OPENACC
    accdevtype = acc_get_device_type();
    *devCount = acc_get_num_devices(accdevtype);
#endif
}

void initialize(int vecsize, double *vec)
{
    int i;
#pragma acc parallel loop present(vec[0:vecsize])
    for (i = 0; i < vecsize; i++)
        vec[i] = 0.0;
}

void compute(int vecsize, double *vec, int rank)
{
    int i;
#pragma acc parallel loop present(vec[0:vecsize])
    for (i = 0; i < vecsize; i++)
        vec[i] = vec[i] + (double) rank;
}

double checksum(int vecsize, double *vec)
{
    int i;
    double csum = 0;
#pragma acc parallel loop present(vec[0:vecsize]) reduction(+:csum)
    for (i = 0; i < vecsize; i++)
        csum = csum + vec[i];

    return csum;
}

void usage(char *pname)
{
    printf("Usage %s [n], where n is the vector length\n", pname);
}

int main(int argc, char *argv[])
{

    char nodename[MPI_MAX_PROCESSOR_NAME];
    const int vecsize_def = 1024;
    int rank, nprocs, noderank, nodenprocs, devcount;

    int vecsize, p;
    double *vec, csum;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    switch (argc) {
    case 1:
        vecsize = vecsize_def;
        break;
    case 2:
        vecsize = atoi(argv[1]);
        break;
    default:
        usage(argv[0]);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    getNodeInfo(nodename, &noderank, &nodenprocs, &devcount);
#ifdef _OPENACC
    if (nodenprocs > devcount) {
        printf("Not enough GPUs for all processes in the node.\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
#endif

    MPI_Barrier(MPI_COMM_WORLD);

    vec = (double *) malloc(sizeof(double) * vecsize);
    if (vec == NULL) {
        printf("Memory allocation failed!\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
#pragma acc data create(vec[0:vecsize])
    {
        /* Initialize the data */
        initialize(vecsize, vec);

        /* Broadcast data vector from rank 0 to all other processes */
#pragma acc host_data use_device(vec)
        {
            MPI_Bcast(vec, vecsize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }

        compute(vecsize, vec, rank);

        if (rank == 0) {
            for (p = 1; p < nprocs; p++) {
#pragma acc host_data use_device(vec)
                MPI_Recv(vec, vecsize, MPI_DOUBLE, p,
                         MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                csum = checksum(vecsize, vec);

                printf("Rank: %d, element average= %18.16f\n", p,
                       (double) csum / vecsize);
            }
        } else {
            /* Send result back to rank 0 */
#pragma acc host_data use_device(vec)
            MPI_Send(vec, vecsize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }

    }
    /* Free memory */
    free(vec);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
