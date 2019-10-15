# GPU Programming with OpenACC

14-15 October, 2019, at CSC - IT Center for Science.

Course [homepage](https://www.csc.fi/web/training/-/gpu-openacc-2019). 


## Table of content

1. [Schedule](#schedule)
1. [Material](#lecture-material)
1. [Instructions for running OpenACC exercises](#instructions-for-running-openacc-exercises)
1. [Exercises](https://github.com/csc-training/openacc/tree/master/exercises)

## Schedule

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

## Lecture material

The lecture material of the course is available in this repository at [docs](/docs). There are also pre-compiled pdf versions available:
* [Introduction to accelerators 1/2](https://a3s.fi/openacc-10-2019/01-a-GPU-intro.pdf)
* [Introduction to accelerators 2/2](https://a3s.fi/openacc-10-2019/01-b-parallel-concepts.pdf)
* [Introduction to OpenACC](https://a3s.fi/openacc-10-2019/02-OpenACC-intro.pdf)
* [OpenACC data management](https://a3s.fi/openacc-10-2019/03-OpenACC-data.pdf)
* [Profiling and performance optimization](https://a3s.fi/openacc-10-2019/04-Profiling-GPUs.pdf)
* [Advanced topics 1/2](https://a3s.fi/openacc-10-2019/05-async-routine.pdf)
* [Advanced topics 2/2](https://a3s.fi/06-interoperability.pdf)
* The full set of [slides](https://a3s.fi/openacc-10-2019/openacc-2019.pdf)
    
    
Other external material
- [OpenACC 2.6 quick reference](https://www.openacc.org/sites/default/files/inline-files/OpenACC%20API%202.6%20Reference%20Guide.pdf)
- [openacc.org](http://www.openacc.org)
- [openacc 2.7 specifications](https://www.openacc.org/sites/default/files/inline-files/OpenACC.2.7.pdf)
- [NVIDIA Accelerated Commputing](https://developer.nvidia.com/accelerated-computing)
- [NVIDIA OpenACC resources](https://developer.nvidia.com/openacc)
- [PGI compiler suite](http://www.pgroup.com/) Free of charge OpenACC compiler (community edition)

## Instructions for running OpenACC exercises

All exercises can be done using the local classroom workstations. You may also use the [Puhti-AI](https://docs.csc.fi) partition of Puhti cluster.

### Downloading the exercises

To get the exercises you can clone the repository

```shell
git clone https://github.com/csc-training/openacc.git
```

This command will create a folder called `openacc` where all the materials are located. If the repository is updated during the course you can update your local copy of it with a `git pull` command.

You can find the exercises in two different folders within this repository. Folder [openacc/exercises](/exercises/) has a set of exercises prepared by CSC,while the exercises in [openacc/nvidia-labs](/nvidia-labs/) are courtesy of Nvidia.

### Compiling with PGI compiler

PGI compiler commands  are

- `pgcc` for C
- `pgfortran` for fortran
- `pgc++` for C++

OpenACC compilation can be enabled with option `-acc`. Note that without this flag, a non-accelerated version will be compiled. In addition to the acc flag, you have to specify the type of the accelerator with `-ta=nvidia,kepler` flag. If you run these exercises on some other system, you have to modify the type of the accelerator accordingly. You can check the type of the acclerator and recommended flags with command `pgaccelinfo`. If you want to get detailed information on OpenACC code generation, you can use option `-Minfo=accel`.

### Running on Puhti

You can log into [Puhti](https://docs.csc.fi/#computing/overview/) front-end node using ssh command `ssh -Y trainingXXX@puhti.csc.fi` where `XXX` is the number of your training account. You can also use your own CSC account if you have one.

Before compiling the exercises you have to load the correct environment module `pgi` as well as reload the git module to access this repository

```shell
module purge
module load pgi
module load cuda
module load git
```

Serial jobs can be run interactively with srun command, for example

```shell
srun -n1 -p gpu -t 00:05:00 --gres=gpu:v100:1 --account=YYY ./my_program
```

If the normal GPU nodes are all occupied, it can also make sense to run in the GPU test partition 

```shell
srun -n1 -p gputest -t 00:05:00 --gres=gpu:v100:1 --account=YYY ./my_program
```

If you are using CSC training accounts you should use the following project as your account:
`--account=project_2000745`.



#### Reservation

This course has a resource reservation that can be used for the exercises. You can run your job within the reservation with `--reservation` flag, such as

```shell
srun --reservation=openACC_course_mon -n1 -p gpu --gres=gpu:v100:1 --account=YYY ./my_program
```

Names of the reservations for Monday and Tuesday are `openACC_course_mon` and `openACC_course_tue` respectively.

#### Running the visual profiler

The visual profiler is a GUI program which means we need to get the X session out of the compute nodes. The way this is done now is to first allocate a GPU for your use, then start an ssh session into the node you allocated. Once logged into the node you need to reload the modules and then you can run the visual profiler `nvvp`

```shell
salloc -N1 -n 1 --account=YYY -p gpu --gres=gpu:v100:1  --reservation=openACC_course_tue
ssh -X $(srun hostname) 
module load pgi cuda
nvvp
```

