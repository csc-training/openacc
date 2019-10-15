# Calling cuRAND from OpenACC code

In this exercise we use routines from the cuRAND CUDA library to calculate a simple estimate for π. The skeleton code `curand_openacc(.c|.F90)` implements a version that computes the estimate using cpu code. Fill in the missing parts of a function `gpu_pi` that does the similar computation using OpenACC.

Initializing and closing the random number generator is done in Fortran with a call
```Fortran
type(curandGenerator) :: g
logical :: istat
istat = curandCreateGenerator(g, CURAND_RNG_PSEUDO_DEFAULT)
...
istat = curandDestroyGenerator(g)
```

and in C

```C
curandGenerator g;
int istat;
istat = curandCreateGenerator(&g, CURAND_RNG_PSEUDO_DEFAULT);
...
istat = curandDestroyGenerator(g);
```

Vector `x` of `n` Uniform random numbers between 0 and 1 can be generated in Fortran using call

```Fortran
istat = curandGenerateUniform(g, x, n)
```

and C

```C
istat = curandGenerateUniform(g, x, n)
```

Here the vector `x` has to be a *device pointer*, so use correct `host_data` specification.
