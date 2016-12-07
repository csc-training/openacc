# GPU Programming with OpenACC

7-9 december, 2016, at CSC - IT Center for Science.

## Table of content
 1. [Schedule](#schedule)
 2. [Slides](#lecture-material)
 3. [Instructions for running OpenACC exercises](#instructions-for-running-openacc-exercises)
 4. [Exercises](#exercises)




# Schedule


| Day 1         |                              |
| ------------- | ----------------------------
| 9:00  - 10:00 | Introduction to accelerators
| 10:00 - 10:15 | Break                        
| 10:15 - 11:15 | Introduction to OpenACC  
| 11:15 - 12:00 | Exercises
| 12:00 - 12:45 | Lunch
| 12:45 - 13:15 | Exercises
| 13:15 - 14:15 | OpenACC data management
| 14:15 - 14:30 | Break
| 14:30 - 16:00 | Exercises & wrap up of day 1


| Day 2         |                              |
| ------------- | ----------------------------
| 9:00  - 10:15 | Profiling and performance optimization
| 10:15 - 10:30 | Break
| 10:30 - 12:00 | Exercises
| 12:00 - 12:45 | Lunch
| 12:45 - 14:00 | Advanced topics
| 14:00 - 14:15 | Break
| 14:15 - 16:00 | Exercises & wrap up of day 2

| Day 3         |                              |
| ------------- | ----------------------------
| 9:00  - 10:15 | Multi-GPU programming with OpenACC
| 10:15 - 10:30 | Break
| 10:30 - 12:00 | Exercises
| 12:00 - 12:45 | Lunch
| 12:45 - 16:00 | BYOC (break at 14:00)


# Lecture material

The [slides](https://events.prace-ri.eu/event/562/material/slides/) can be downloaded from the course [homepage](https://events.prace-ri.eu/event/562/) on the PRACE website.

# Instructions for running OpenACC exercises

All exercises except the MPI labs can be done using the local
classroom workstations. You may also use the [Taito-GPU](https://research.csc.fi/taito-gpu) partition of Taito cluster.


## Downloading the exercises

The exercises are in this repository in two different folders. In
[openacc/exercises](/exercises/) are a set of exercises prepared by
CSC, while in [openacc/nvidia-labs](/nvidia-labs/) exercises courtesy
of Nvidia are located.

To get the exercises you can clone the repository

```
git clone https://github.com/csc-training/openacc.git
```

This command will create a folder called ```openacc``` where all the
materials are located. If the repository is updated during the course
you can update your local copy of it with a ```git pull``` command.

## Compiling with PGI compiler

PGI compiler commands for C and Fortran are ```pgcc``` and
```pgfortran```.  OpenACC compilation can be enabled with option
```-acc```. Note that without this flag, a non-accelerated version
will be compiled. In addition to the acc flag, you have to specify the
type of the accelerator with ```-ta=nvidia,kepler``` flag.  If you run
these exercises on some other system, you have to modify the type of
the accelerator accordingly. You can check the type of the acclerator
and recommended flags with command ```pgaccelinfo```. If you want to
get detailed information on OpenACC code generation, you can use
option ```-Minfo=accel```.

## Running on local desktop computers

Classroom workstations have Quadro K600 GPUs and both CUDA SDK and PGI
compiler installations. The PGI compiler is not in the PATH by default, but
you can initialize the environment using command ```source ~/pgi/pgi-16.10```.


## Running on Taito-GPU

You can log into [Taito-GPU](https://research.csc.fi/taito-gpu)
front-end node using ssh command ```ssh -Y trngXXX@taito-gpu.csc.fi```
where ```XXX``` is the number of your training account. You can also
use your own CSC account if you have one.

Before compiling the exercises you have to load the correct environment module
using command ```module load openacc-env/16.7```. Serial jobs can be run
interactively with srun command, for example
```
srun -n1 -pgpu --gres=gpu:1 -Ck80 ./my_program
```

Multi-GPU jobs require a slightly different set of options, for example a MPI
job that uses eight GPUs on two nodes and launch a single MPI task per GPU can
be run with command
```
srun -n8 --ntasks-per-socket=2 -N2 --gres=gpu:4 -Ck80 ./my_program
```

### Reservation

This course has a resource reservation that can be used for the exercises. You
can run your job within the reservation with ```--reservation``` flag, such as
```srun --reservation=acc_course_wed -n1 -pgpu --gres=gpu:1 ./my_program```.

Names of the reservations for Wednesday, Thursday and Friday are
```acc_course_wed```, ```acc_course_thu``` and ```acc_course_fri```
respectively.

#Exercises


## Introduction to accelerators  & OpenACC

In this session we do the following exercises
 * [Exercise 1 ](/exercises/ex1/)
 * [Exercise 2 ](/exercises/ex2/)
 * [Exercise 3 ](/exercises/ex3/)

## OpenACC data management

In this session we do the following exercises
 * [Exercise 4 ](/exercises/ex4/)
 * [NVIDIA Lab 3](/nvidia-labs/lab3/) [steps 0 - 1](/nvidia-labs/lab3/steps-0-1.md)
 * [Exercise 5 ](/exercises/ex5/)


## Profiling and performance optimization


In this session we do the following exercises
 * Analyze performance of exercise 5. Use [solution](/exercises/ex5/solution) if you did not finish it on your own in the last session.
 * [NVIDIA Lab 3](/nvidia-labs/lab3/) [steps 2 - 5](/nvidia-labs/lab3/steps-2-5.md)



## Advanced topics
In this session we do the following exercises
 * [NVIDIA Lab 4](/nvidia-labs/lab4.pipelining/)


## Multi GPU programming with OpenACC
In this session we do the following exercises
 * [Exercise 6 ](/exercises/ex6/)
 * [Exercise 7 ](/exercises/ex7/)
