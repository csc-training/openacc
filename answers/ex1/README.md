## Compiling and running simple OpenACC program
### Solution 

Load pgi compiler / cuda modules

```
module purge
module load pgi/14.4 cuda
```

Compile (with PGI compiler)

C:
```
pgcc -g -acc -ta=tesla,kepler+ ex1_hello.c -o ex1_hello
```
Fortran:
```
pgf90 -g -acc -ta=tesla,kepler+ ex1_hello.F90 -o ex1_hello
```

Running:
```
srun -N1 -n1 -c1 -p gpu --gres=gpu:1 -t 00:05:00 ./ex1_hello
```
