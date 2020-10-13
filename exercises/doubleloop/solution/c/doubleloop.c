#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#ifdef _OPENACC
#include <openacc.h>
#endif

/* Utility routine for allocating a two dimensional array */
double **malloc_2d(int nx, int ny)
{
    double **array;
    int i;

    array = (double **) malloc(nx * sizeof(double *));
    array[0] = (double *) malloc(nx * ny * sizeof(double));

    for (i = 1; i < nx; i++) {
        array[i] = array[0] + i * ny;
    }

    return array;
}


/* Utility routine for deallocating a two dimensional array */
void free_2d(double **array)
{
    free(array[0]);
    free(array);
}


void init(double ** arr, int nx, int ny)
{
    int i, j;
    for (i = 0; i < nx + 2; i++)
        for (j = 0; j < ny + 2; j++)
            arr[i][j] = 0e0;

    for (i = 0; i < nx + 2; i++)
        arr[i][ny+1] = 1e0;

    for (i = 0; i < ny+2; i++)
        arr[nx+1][i] = 1e0;
}


int main(int argc, char **argv)
{
    double eps;

    double ** u, ** unew;
    const double factor = 0.25;
    const int niter = 2;
    double mlups = 0e0;
    int nx = 1022, ny = 1022;
    clock_t t_start, t_end;
    double dt;
    double sum;
    int i, j;
    int iter;

    u = malloc_2d(nx+2, ny+2);
    unew = malloc_2d(nx+2, ny+2);

    init(u, nx, ny);
    init(unew, nx, ny);

    t_start = clock();

    for (iter = 0; iter < niter; iter++) {
#pragma acc parallel
        {
#pragma acc loop
            for (i = 1; i < nx + 1; i++)
#pragma acc loop
                for (j = 1; j < ny + 1; j++) {
                    unew[i][j] = factor * (u[i-1][j] + u[i+1][j] +
                            u[i][j-1] + u[i][j+1]);
                }
        }
#pragma acc parallel
        {
#pragma acc loop
            for (i = 1; i < nx + 1; i++)
#pragma acc loop
                for (j = 1; j < ny + 1; j++) {
                    u[i][j] = factor * (unew[i-1][j] + unew[i+1][j] +
                            unew[i][j-1] + unew[i][j+1]);
                }
        }
    }

    /* Compute a reference sum, do not parallelize this! */
    sum = 0.0;
    for (i = 1; i < nx + 1; i++)
        for (j = 1; j < ny + 1; j++)
            sum += u[i][j];

    free_2d(u);
    free_2d(unew);

    mlups = niter * nx * ny * 1.0e-6;
    t_end = clock();
    dt = ((double)(t_end-t_start)) / CLOCKS_PER_SEC;
    printf("Stencil: Time =%18.16f sec, MLups/s=%18.16f, sum=%18.16f\n",dt, (double) mlups/dt, sum);

    return 0;
}
