## Compiling and running simple OpenACC program
### Solution 

The cuda environment is already loaded by default on Taito. To take in use the
latest versions of the PGI compiler and Cuda you can reset the environment as follows:
```
module purge
module load pgi/16.7 cuda/8.0 git
```

Compile (with PGI compiler)

C:
```
pgcc -g -acc -ta=tesla,kepler+ hello.c -o hello
```
Fortran:
```
pgf90 -g -acc -ta=tesla,kepler+ hello.F90 -o hello
```

Running:
```
srun -N1 -n1 -c1 -p gpu --gres=gpu:1 -t 00:05:00 ./hello
```
