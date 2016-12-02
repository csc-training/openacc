# GPU Programming with OpenACC

7-9 december, 2016

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



# Instructions for running OpenACC exercises

All exercises except the MPI labs can be done using the local classroom workstations. You may also use the GPU partition of Taito cluster.

## Local desktop computers

Classroom workstations have Quadro K600 GPUs and both CUDA SDK and PGI compiler installations. The PGI compiler is not in the PATH by default, but you can initialize the environment using command ```source ~/pgi/pgi-16.10```.

## Taito-GPU

You can log into Taito GPU front-end node using ssh command ```ssh -Y trngXX@taito-gpu.csc.fi``` where ```XX``` is the number of your training account. You can also use your own CSC account if you have one.

Before compiling the exercises you have to load the correct environment module using command ```module load openacc-env```. Serial jobs can be run interactively with srun command, for example
```
srun -n1 -pgpu --gres=gpu:1 -Ck80 ./my_program
```

Multi-GPU jobs require a slightly different set of options, for example a MPI job that uses eight GPUs on two nodes and launch a single MPI task per GPU can be run with command
```
srun -n8 --ntasks-per-socket=2 -N2 --gres=gpu:4 -Ck80 ./my_program
```

### Reservation

This course has a resource reservation that can be used for the exercises. You can run your job within the reservation with ```--reservation``` flag, such as ```srun --reservation=acc_course_wed -n1 -pgpu --gres=gpu:1 ./my_program```.

Names of the reservations for Wednesday, Thursday and Friday are ```acc_course_wed```, ```acc_course_thu``` and ```acc_course_fri``` respectively.

#Exercises

## Introduction to accelerators  & OpenACC

In this session we do the following exercises
 * [Exercise 1 ](/exercises/ex1/)
 * [Exercise 2 ](/exercises/ex2/)
 * [Exercise 3 ](/exercises/ex3/)

<!--INCLUDE-exercises/ex1/README.md-->
<!--INCLUDE-exercises/ex2/README.md-->
<!--INCLUDE-exercises/ex3/README.md-->


## OpenACC data management

In this session we do the following exercises
 * [Exercise 4 ](/exercises/ex4/)
 * [NVIDIA Lab 3](/nvidia-labs/lab3/) [steps 0 - 1](/nvidia-labs/lab3/steps-0-1.md)
 * [Exercise 5 ](/exercises/ex5/)



<!--INCLUDE-exercises/ex4/README.md-->


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
