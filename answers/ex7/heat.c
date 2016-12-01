/* 2D heat equation

   Copyright (C) 2014  CSC - IT Center for Science Ltd.

   Licensed under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Code is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   Copy of the GNU General Public License can be onbtained from
   see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#include "heat.h"
#include "pngwriter.h"

// Utility routine for allocating a two dimensional array
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


// Utility routine for deallocating a two dimensional array
void free_2d(double **array)
{
    free(array[0]);
    free(array);
}

// Initialize the metadata. Note that the nx is the size of the first
// dimension and ny the second.
void initialize_field_metadata(field * temperature, int nx, int ny,
                               parallel_data * parallel)
{
    int nx_local;

    nx_local = nx / parallel->size;

    temperature->dx = DX;
    temperature->dy = DY;
    temperature->dx2 = DX * DX;
    temperature->dy2 = DY * DY;
    temperature->nx = nx_local;
    temperature->ny = ny;
    temperature->nx_full = nx;
    temperature->ny_full = ny;
}

// Copy data on temperature1 into temperature2
void copy_field(field * temperature1, field * temperature2)
{
    assert(temperature1->nx == temperature2->nx);
    assert(temperature1->ny == temperature2->ny);
    memcpy(temperature2->data[0], temperature1->data[0],
           (temperature1->nx + 2) * (temperature1->ny +
                                     2) * sizeof(double));
}

void fields_to_device(field * prev, field * curr)
{
    int nx, ny;
    double **cdata, **pdata;
    nx = curr->nx;
    ny = curr->ny;
    cdata = curr->data;
    pdata = prev->data;

#pragma acc enter data copyin(cdata[0:nx+2][0:ny+2], pdata[0:nx+2][0:ny+2])
}

void fields_from_device(field * prev, field * curr)
{
    int nx, ny;
    double **cdata, **pdata;
    nx = curr->nx;
    ny = curr->ny;
    cdata = curr->data;
    pdata = prev->data;

#pragma acc exit data copyout(cdata[0:nx+2][0:ny+2], pdata[0:nx+2][0:ny+2])
}

// Swap the data of fields temperature1 and temperature2
void swap_fields(field * temperature1, field * temperature2)
{
    double **tmp;
    tmp = temperature1->data;
    temperature1->data = temperature2->data;
    temperature2->data = tmp;

}

void parallel_initialize(parallel_data * parallel, int nx, int ny)
{
    int nx_local;

    parallel->comm = MPI_COMM_WORLD;

    MPI_Comm_size(parallel->comm, &parallel->size);

    nx_local = nx / parallel->size;
    if (nx_local * parallel->size != nx) {
        printf("Cannot divide grid evenly to processors\n");
        MPI_Abort(parallel->comm, -2);
    }


    MPI_Comm_rank(parallel->comm, &parallel->rank);

    parallel->nup = parallel->rank - 1;
    parallel->ndown = parallel->rank + 1;

    if (parallel->nup < 0)
        parallel->nup = MPI_PROC_NULL;
    if (parallel->ndown > parallel->size - 1)
        parallel->ndown = MPI_PROC_NULL;

}

void initialize(field * temperature1, field * temperature2,
                parallel_data * parallel)
{
    int i, j;

    // Allocate also ghost layers
    temperature1->data =
        malloc_2d(temperature1->nx + 2, temperature1->ny + 2);
    temperature2->data =
        malloc_2d(temperature2->nx + 2, temperature2->ny + 2);

    // Initialize to zero
    memset(temperature1->data[0], 0.0,
           (temperature1->nx + 2) * (temperature1->ny + 2)
           * sizeof(double));


    for (i = 0; i < temperature1->nx + 2; i++) {
        temperature1->data[i][0] = 30.0;
        temperature1->data[i][temperature1->ny + 1] = -10.0;
    }

    if (parallel->rank == 0) {
        for (j = 0; j < temperature1->ny + 2; j++)
            temperature1->data[0][j] = 15.0;
    } else if (parallel->rank == parallel->size - 1) {
        for (j = 0; j < temperature1->ny + 2; j++)
            temperature1->data[temperature1->nx + 1][j] = -25.0;
    }

    copy_field(temperature1, temperature2);

}

void evolve(field * curr, field * prev, double a, double dt)
{
    int i, j, nx, ny;
    double **cdata, **pdata, dx2, dy2;

    // Determine the temperature field at next time step
    // As we have fixed boundary conditions, the outermost gridpoints
    // are not updated
  // *INDENT-OFF*
  cdata = curr->data;
  pdata = prev->data;
  nx = curr->nx;
  ny = curr->ny;
  dx2 = prev->dx2;
  dy2 = prev->dy2;

#pragma acc parallel loop private(i,j) \
  present(pdata[0:nx+2][0:ny+2], cdata[0:nx+2][0:ny+2]) collapse(2)
    for (i = 1; i < nx + 1; i++)
        for (j = 1; j < ny + 1; j++) {
            cdata[i][j] = pdata[i][j] + a * dt *
                ((      pdata[i + 1][j] -
                  2.0 * pdata[  i  ][j] +
                        pdata[i - 1][j]) / dx2 +
                 (      pdata[i][j + 1] -
                  2.0 * pdata[i][  j  ] +
                        pdata[i][j - 1]) / dy2);
        }
    // *INDENT-ON*

}

void exchange(field * temperature, parallel_data * parallel)
{

    double **pdata, *data0, *data1, *datanx, *datanx1;
    int nx, ny;

    nx = temperature->nx;
    ny = temperature->ny;
    pdata = temperature->data;
    data0 = temperature->data[0];
    data1 = temperature->data[1];
    datanx = temperature->data[nx];
    datanx1 = temperature->data[nx + 1];

#pragma acc host_data use_device(data1, datanx1)
    // Send to the up, receive from down
    MPI_Sendrecv(data1, temperature->ny + 2, MPI_DOUBLE,
                 parallel->nup, 11,
                 datanx1,
                 temperature->ny + 2, MPI_DOUBLE, parallel->ndown, 11,
                 parallel->comm, MPI_STATUS_IGNORE);

    // Send to the down, receive from up
#pragma acc host_data use_device(data0, datanx)
    MPI_Sendrecv(datanx, temperature->ny + 2,
                 MPI_DOUBLE, parallel->ndown, 12,
                 data0, temperature->ny + 2, MPI_DOUBLE,
                 parallel->nup, 12, parallel->comm, MPI_STATUS_IGNORE);

}

void finalize(field * temperature1, field * temperature2,
              parallel_data * parallel)
{
    free_2d(temperature1->data);
    free_2d(temperature2->data);
}

void output(field * temperature, int iter, parallel_data * parallel)
{
    char filename[64];

    // The actual write routine takes only the actual data
    // (without ghost layers) so we need array for that
    int height, width;
    double **full_data;
    double **tmp_data;          // array for MPI sends and receives
    double **cdata;

    int i, p, nx, ny;

    height = temperature->nx * parallel->size;
    width = temperature->ny;
    cdata = temperature->data;
    nx = temperature->nx;
    ny = temperature->ny;

#pragma acc update host(cdata[0:nx+2][0:ny+2])

    tmp_data = malloc_2d(temperature->nx, temperature->ny);

    if (parallel->rank == 0) {
        // Copy the inner data
        full_data = malloc_2d(height, width);
        for (i = 0; i < temperature->nx; i++)
            memcpy(full_data[i], &temperature->data[i + 1][1],
                   temperature->ny * sizeof(double));

        // Receive data
        for (p = 1; p < parallel->size; p++) {
            MPI_Recv(&tmp_data[0][0], temperature->nx * temperature->ny,
                     MPI_DOUBLE, p, 22, parallel->comm, MPI_STATUS_IGNORE);
            // Copy data to full array
            memcpy(&full_data[p * temperature->nx][0], tmp_data[0],
                   temperature->nx * temperature->ny * sizeof(double));
        }
    } else {
        // Send data
        for (i = 0; i < temperature->nx; i++)
            memcpy(tmp_data[i], &temperature->data[i + 1][1],
                   temperature->ny * sizeof(double));
        MPI_Send(&tmp_data[0][0], temperature->nx * temperature->ny,
                 MPI_DOUBLE, 0, 22, parallel->comm);
    }

    if (parallel->rank == 0) {
        sprintf(filename, "%s_%04d.png", "heat", iter);
        save_png(full_data[0], height, width, filename, 'c');

        free_2d(full_data);
    }
    free_2d(tmp_data);
}

void read_input(field * temperature1, field * temperature2, char *filename,
                parallel_data * parallel)
{
    FILE *fp;
    int nx, ny, i, j;

    double **full_data;
    double **inner_data;

    int nx_local;

    fp = fopen(filename, "r");
    // Read the header
    fscanf(fp, "# %d %d \n", &nx, &ny);

    parallel_initialize(parallel, nx, ny);
    initialize_field_metadata(temperature1, nx, ny, parallel);
    initialize_field_metadata(temperature2, nx, ny, parallel);

    // Allocate arrays (including ghost layers)
    temperature1->data =
        malloc_2d(temperature1->nx + 2, temperature1->ny + 2);
    temperature2->data =
        malloc_2d(temperature2->nx + 2, temperature2->ny + 2);

    inner_data = malloc_2d(temperature1->nx, temperature1->ny);

    if (parallel->rank == 0) {
        // Full array
        full_data = malloc_2d(nx, ny);

        // Read the actual data
        for (i = 0; i < nx; i++) {
            for (j = 0; j < ny; j++) {
                fscanf(fp, "%lf", &full_data[i][j]);
            }
        }
    } else
        // dummy array for full data
        full_data = malloc_2d(1, 1);

    nx_local = temperature1->nx;

    MPI_Scatter(full_data[0], nx_local * ny, MPI_DOUBLE, inner_data[0],
                nx_local * ny, MPI_DOUBLE, 0, parallel->comm);

    // Copy to the array containing also boundaries
    for (i = 0; i < nx_local; i++)
        memcpy(&temperature1->data[i + 1][1], &inner_data[i][0],
               ny * sizeof(double));

    // Set the boundary values
    for (i = 0; i < nx_local + 1; i++) {
        temperature1->data[i][0] = temperature1->data[i][1];
        temperature1->data[i][ny + 1] = temperature1->data[i][ny];
    }
    for (j = 0; j < ny + 2; j++) {
        temperature1->data[0][j] = temperature1->data[1][j];
        temperature1->data[nx_local + 1][j] =
            temperature1->data[nx_local][j];
    }

    copy_field(temperature1, temperature2);

    free_2d(full_data);
    free_2d(inner_data);
    fclose(fp);
}
