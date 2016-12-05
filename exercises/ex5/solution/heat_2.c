/* 2D heat equation

   Copyright (C) 2014  CSC - IT Center for Science Ltd.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef _OPENACC
#include <openacc.h>
#endif

#include "heat_2.h"
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
void initialize_field_metadata(field * temperature, int nx, int ny)
{
    temperature->dx = DX;
    temperature->dy = DY;
    temperature->dx2 = DX * DX;
    temperature->dy2 = DY * DY;
    temperature->nx = nx;
    temperature->ny = ny;
}

void fields_to_device(field * prev, field * curr) 
{
  int nx, ny;
  double ** cdata, **pdata;
  nx = curr->nx;
  ny = curr->ny;
  cdata = curr->data;
  pdata = prev->data;
  
#pragma acc enter data copyin(cdata[0:nx+2][0:ny+2], pdata[0:nx+2][0:ny+2])
}

void fields_from_device(field * prev, field * curr)
{
  int nx, ny;
  double ** cdata, **pdata;
  nx = curr->nx;
  ny = curr->ny;
  cdata = curr->data;
  pdata = prev->data;
  
#pragma acc exit data copyout(cdata[0:nx+2][0:ny+2], pdata[0:nx+2][0:ny+2])
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

// Swap the data of fields temperature1 and temperature2
void swap_fields(field * temperature1, field * temperature2)
{
    double **tmp;
    tmp = temperature1->data;
    temperature1->data = temperature2->data;
    temperature2->data = tmp;
}

// Initialize the temperature field.  Pattern is disc with a radius 
// of nx_full / 6 in the center of the grid.
// Boundary conditions are (different) constant temperatures outside the grid
void initialize(field * temperature1, field * temperature2)
{
    int i, j;
    double radius2;
    int ds2;

    // Allocate also ghost layers
    temperature1->data =
        malloc_2d(temperature1->nx + 2, temperature1->ny + 2);
    temperature2->data =
        malloc_2d(temperature2->nx + 2, temperature2->ny + 2);


    // Square of the disc radius
    radius2 = (temperature1->nx / 6.0) * (temperature1->nx / 6.0);
    for (i = 0; i < temperature1->nx + 2; i++)
        for (j = 0; j < temperature1->ny + 2; j++) {
           // Distance of point i, j from the origin
           ds2 = (i - temperature1->nx / 2 + 1) *
              (i - temperature1->nx / 2 + 1) + 
              (j - temperature1->ny / 2 + 1) * 
              (j - temperature1->ny / 2+ 1);
        if (ds2 < radius2)
          temperature1->data[i][j] = 5.0;
        else
          temperature1->data[i][j] = 65.0;
      }
          

    // Boundary conditions
    for (i = 0; i < temperature1->nx + 2; i++) {
        temperature1->data[i][0] = 20.0;
        temperature1->data[i][temperature1->ny + 1] = 70.0;
    }

    for (j = 0; j < temperature1->ny + 2; j++) {
        temperature1->data[0][j] = 85.0;
        temperature1->data[temperature1->nx + 1][j] = 5.0;
    }
    
    copy_field(temperature1, temperature2);

}

void evolve(field * curr, field * prev, double a, double dt)
{
  int i, j, nx, ny;
  double ** cdata, ** pdata, dx2, dy2;

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

void finalize(field * temperature1, field * temperature2)
{
    free_2d(temperature1->data);
    free_2d(temperature2->data);
}

void output(field * temperature, int iter)
{
    char filename[64];

    // The actual write routine takes only the actual data
    // (without ghost layers) so we need array for that
    int height, width;
    double **full_data, **cdata;

    int i, nx, ny;

    height = temperature->nx;
    width = temperature->ny;

    cdata = temperature->data;
    nx = temperature->nx;
    ny = temperature->ny;

#pragma acc update host(cdata[0:nx+2][0:ny+2])

    // Copy the inner data
    full_data = malloc_2d(height, width);
    for (i = 0; i < temperature->nx; i++)
        memcpy(full_data[i], &temperature->data[i + 1][1],
               temperature->ny * sizeof(double));

    sprintf(filename, "%s_%04d.png", "heat", iter);
    save_png(full_data[0], height, width, filename, 'c');

    free(full_data);
}

void read_input(field * temperature1, field * temperature2, char *filename)
{
    FILE *fp;
    int nx, ny, i, j;

    fp = fopen(filename, "r");
    // Read the header
    fscanf(fp, "# %d %d \n", &nx, &ny);

    initialize_field_metadata(temperature1, nx, ny);
    initialize_field_metadata(temperature2, nx, ny);

    // Allocate arrays (including ghost layers
    temperature1->data = malloc_2d(nx + 2, ny + 2);
    temperature2->data = malloc_2d(nx + 2, ny + 2);

    // Read the actual data
    for (i = 1; i < nx + 1; i++) {
        for (j = 1; j < ny + 1; j++) {
            fscanf(fp, "%lf", &temperature1->data[i][j]);
        }
    }

    // Set the boundary values
    for (i = 1; i < nx + 1; i++) {
        temperature1->data[i][0] = temperature1->data[i][1];
        temperature1->data[i][ny + 1] = temperature1->data[i][ny];
    }
    for (j = 0; j < ny + 2; j++) {
        temperature1->data[0][j] = temperature1->data[1][j];
        temperature1->data[nx + 1][j] = temperature1->data[nx][j];
    }

    copy_field(temperature1, temperature2);

    fclose(fp);
}
