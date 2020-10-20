# GPU Programming with OpenACC

22-23 October, 2019

Course [homepage](https://events.prace-ri.eu/event/1073/).


## Schedule

| Day 1         |                              |
| ------------- | ----------------------------
|  9:00 -  9:30 | Introduction to accelerators
|  9:30 -  9:35 | (break)
|  9:35 - 10:30 | Introduction to OpenACC
| 10:30 - 16:00 | (self-study) exercises: hello world, vector sum, double loop
| 16:00 - 17:00 | Q&A session

| Day 2         |                              |
| ------------- | ----------------------------
|  9:00 -  9:45 | Profiling and performance optimisation
|  9:45 - 10:00 | (break)
| 10:00 - 11:00 | Data management
| 11:00 - 16:00 | (self-study) exercises: jacobi, heat equation (profile)
| 15:30 - 16:00 | Advanced topic: Multiple GPUs with MPI
| 16:00 - 17:00 | Q&A session


## Lecture materials

The lecture materials of the course are available in this repository in the
[docs/](/docs) directory. Pre-compiled PDF versions of the slides are also
available at: https://kannu.csc.fi/s/kYPBWXDd94PasdQ

Other useful material:
- [OpenACC 2.7 quick reference](https://www.openacc.org/sites/default/files/inline-files/API%20Guide%202.7.pdf)
- [OpenACC specification](https://www.openacc.org/specification)
- [openacc.org](http://www.openacc.org)
- [NVIDIA OpenACC resources](https://developer.nvidia.com/openacc)
- [NVIDIA HPC SDK](https://developer.nvidia.com/hpc-sdk)


## Exercises

Exercise materials are located in two different directories within this
repository. In the [exercises/](/exercises/) directory is a set of exercises
prepared by CSC, while the exercises in the [nvidia-labs/](/nvidia-labs/)
directory are courtesy of NVIDIA.

Each directory contains a `README.md` file describing the exercise as well as
any skeleton codes or other files needed for the exercise. Exercises for each
topic are outlined in [exercises/README.md](/exercises/README.md).

For the exercises, you may use [CSC's Puhti](https://docs.csc.fi) system
with its GPU partition of NVIDIA V100 GPUs, or your own local system if you
have suitable GPUs and an OpenACC compiler installed.


### Downloading the exercises

To get the exercises you can clone the repository

```shell
git clone https://github.com/csc-training/openacc.git
```

This command will create a folder called `openacc` where all the materials are
located. If the repository is updated during the course you can update your
local copy of it with a `git pull` command.


### Compiling with PGI compiler

PGI compiler commands are

- `pgcc` for C
- `pgfortran` for fortran
- `pgc++` for C++

OpenACC compilation can be enabled with option `-acc`. Note that without this
flag, a non-accelerated version will be compiled.

In addition, on some systems you may have to specify the type of the
accelerator e.g. with `-ta=tesla` flag. On Puhti, the default is `tesla`.
If you run these exercises on some other system, you have to modify the type
of the accelerator accordingly. You can check the type of the accelerator and
recommended flags with command `pgaccelinfo`. If you want to get detailed
information on OpenACC code generation, you can use option `-Minfo=accel`.


### Running on Puhti

You can log into [Puhti](https://docs.csc.fi/computing/overview/) using the
ssh command `ssh -Y trainingXXX@puhti.csc.fi` where `XXX` is the
number of your training account. You can also use your own CSC account if you
have one.

Before compiling the exercises you have to load the correct environment module
`pgi` as well as reload the git module to access this repository

```shell
module purge
module load pgi/19.7
module load cuda/10.1.168
module load git
```

Serial jobs can be run interactively with the srun command, for example

```shell
srun -n1 -p gputest -t 00:05:00 --gres=gpu:v100:1 --account=YYY ./my_program
```

If you want to run a longer job, you may also use the normal GPU partition,
instead of the GPU test partition:

```shell
srun -n1 -p gpu -t 00:30:00 --gres=gpu:v100:1 --account=YYY ./my_program
```

If you are using CSC training accounts you should use the following project as
your account: `--account=project_2000745`. To see what scratch disk space you
have available, please run `csc-workspaces`.


#### Reservation

This course has also a resource reservation that can be used for the
exercises. To use these dedicated resources, you may run your job with the
`--reservation=openacc2020` flag, such as

```shell
srun --reservation=openacc2020 -n1 -p gpu --gres=gpu:v100:1 --account=YYY ./my_program
```

Please note that the normal GPU partition (`-p gpu`) needs to be used with
the reservation.


#### Running the visual profiler

The visual profiler is a GUI program which means we need to get the X session
out of the compute nodes. The way this is done now is to first allocate a GPU
for your use, then start an ssh session into the node you allocated. Once
logged into the node you need to reload the modules and then you can run the
visual profiler `nvvp`

```shell
salloc -N1 -n 1 --account=YYY -p gpu --gres=gpu:v100:1
ssh -X $(srun hostname)
module load pgi cuda
nvvp
```
