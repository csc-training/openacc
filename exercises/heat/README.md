# Heat equation solver parallelized with OpenACC

In this exercise we will port a serial heat equation solver (see below
for details) in two steps.

1. Parallelize the serial heat equation solver with OpenACC by parallelizing the loop for time evolution.
1. Improve the OpenACC parallelization such that the data transfers between the host and the device are minimized.

## Heat equation solver

The heat equation is a partial differential equation that describes the variation of temperature in a given region over time

<img src="images/laplacian.png" alt="du/dt = alpha * nabla^2 u" height="58" >

where u(x, y, z, t) represents temperature variation over space at a given time, and α is a thermal diffusivity constant.

We limit ourselves to two dimensions (plane) and discretize the equation onto a grid.  Then the Laplacian can be expressed as finite differences as

<img src="images/fidi.png" alt="" height="113" >

Where ∆x and ∆y are the grid spacing of the temperature grid u(i,j). We can study the development of the temperature grid with explicit time evolution over time steps ∆t:

<img src="images/timeevo.png" alt="" height="45" >

There are a solver for the 2D equation implemented with Fortran (including some C for printing out the images). You can compile the program by adjusting the Makefile as needed and typing “make”. The solver carries out the time development of the 2D heat equation over the number of time steps provided by the user. The default geometry is a flat rectangle (with grid size provided by the user), but other shapes may be used via input files. Examples on how to run the binary:

- `./heat` No arguments - the program will run with the default arguments: 200 x 200 grid and 500 time steps
- `./heat bottle.dat` One argument - start from a temperature grid provided in the file bottle.dat for the default number of time steps.
- `./heat bottle.dat 1000` Two arguments - will run the program starting from a temperature grid provided in the file bottle.dat for 1000 time steps
- `./heat 1024 2048 1000` Three arguments - will run the program in a 1024x2048 grid for 1000 time steps.

The program will produce a `.png` image of the temperature field after every 100 iterations. You can change that from the parameter `image_interval`. You can visualise the images using the command animate: `animate heat_*.png`, or by using `eog heat_000.png` and using the arrow-keys to loop backward or forward through the files.