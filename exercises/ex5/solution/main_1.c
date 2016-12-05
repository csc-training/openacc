/* 2D heat equation

   Copyright (C) 2014  CSC - IT Center for Science Ltd.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heat_1.h"


int main(int argc, char **argv)
{
    double a = 0.5;             //!< Diffusion constant
    field current, previous;    //!< Current and previous temperature fields

    double dt;                  //!< Time step
    int nsteps = 500;           //!< Number of time steps

    int rows = 200;             //!< Field dimensions with default values
    int cols = 200;

    char input_file[64];        //!< Name of the optional input file

    int image_interval = 10;    //!< Image output interval

    int iter;

    /* Following combinations of command line arguments are possible:
       No arguments:    use default field dimensions and number of time steps
       One argument:    read initial field from a given file
       Two arguments:   initial field from file and number of time steps
       Three arguments: field dimensions (rows,cols) and number of time steps
     */

    switch (argc) {
    case 1:
        // use defaults
        initialize_field_metadata(&current, rows, cols);
        initialize_field_metadata(&previous, rows, cols);
        initialize(&current, &previous);
        break;
    case 2:
        // Initial field from a file
        strncpy(input_file, argv[1], 64);
        read_input(&current, &previous, input_file);
        break;
    case 3:
        // Initial field from a file
        strncpy(input_file, argv[1], 64);
        read_input(&current, &previous, input_file);
        // Number of time steps
        nsteps = atoi(argv[2]);
        break;
    case 4:
        // Field dimensions
        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
        initialize_field_metadata(&current, rows, cols);
        initialize_field_metadata(&previous, rows, cols);
        initialize(&current, &previous);
        // Number of time steps
        nsteps = atoi(argv[3]);
        break;
    default:
        printf("Unsupported number of command line arguments\n");
        return -1;
    }

    // Output the initial field
    output(&current, 0);

    // Largest stable time step
    dt = current.dx2 * current.dy2 /
        (2.0 * a * (current.dx2 + current.dy2));

    // Time evolve
    for (iter = 1; iter < nsteps; iter++) {
        evolve(&current, &previous, a, dt);
        // output every 10 iteration
        if (iter % image_interval == 0)
            output(&current, iter);
        // make current field to be previous for next iteration step
        swap_fields(&current, &previous);
    }

    finalize(&current, &previous);
    return 0;
}
