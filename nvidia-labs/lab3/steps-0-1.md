NVIDIA OpenACC Course - Lab 3
=============================


In this lab you will build upon the work from lab 2 to add explicit data
management directives, eliminating the need to use CUDA Unified Memory, and
optimize the `matvec` kernel using the OpenACC `loop` directive. 


Step 0 - Building the code
--------------------------

Makefiles have been provided for building both the C and Fortran versions of
the code. Change directory to your language of choice and run the `make`
command to build the code.

### C/C++

```
$ cd ~/c99
$ make
```
    
### Fortran

```
$ cd ~/f90
$ make
```
    
This will build an executable named `cg` that you can run with the `./cg`
command. You may change the options passed to the compiler by modifying the
`CFLAGS` variable in `c99/Makefile` or `FCFLAGS` in `f90/Makefile`. You should
not need to modify anything in the Makefile except these compiler flags.

Step 1 - Step 1 - Express Data Movement
---------------------------------------

In the previous lab we used CUDA Unified Memory, which we enabled with the
`ta=tesla:managed` compiler option, to eliminate the need for data management
directives. Replace this compiler flag in the Makefile with `-ta=tesla` and try
to rebuild the code.

### C/C++
With the managed memory option removed the C/C++ version will fail to build
because the compiler will not be able to determine the sizes of some of the
arrays used in compute regions. You will see an error like the one below.

    PGCC-S-0155-Compiler failed to translate accelerator region (see -Minfo
messages): Could not find allocated-variable index for symbol (main.cpp: 15)

### Fortran
The Fortran version of the code will build successfully and run, however the
tolerance value will be incorrect with the managed memory option removed.

    $ ./cg
     Rows:      8120601 nnz:    218535025
     Iteration:  0 Tolerance: 4.006700E+08
     Iteration: 10 Tolerance: 4.006700E+08
     Iteration: 20 Tolerance: 4.006700E+08
     Iteration: 30 Tolerance: 4.006700E+08
     Iteration: 40 Tolerance: 4.006700E+08
     Iteration: 50 Tolerance: 4.006700E+08
     Iteration: 60 Tolerance: 4.006700E+08
     Iteration: 70 Tolerance: 4.006700E+08
     Iteration: 80 Tolerance: 4.006700E+08
     Iteration: 90 Tolerance: 4.006700E+08
      Total Iterations:          100

---

We can correct both of these problems by explicitly declaring the data movement
for the arrays that we need on the GPU. In the associated lecture we discussed
the OpenACC structured `data` directive and the unstructured `enter data` and
`exit data` directives. Either approaced can be used to express the data
locality in this code, but the unstructured directives are probably cleaner to
use.

### C/C++
In the `allocate_3d_poisson_matrix` function in matrix.h, add the following two
directives to the end of the function.


    #pragma acc enter data copyin(A)
    #pragma acc enter data copyin(A.row_offsets[:num_rows+1],A.cols[:nnz],A.coefs[:nnz])

The first directive copies the A structure to the GPU, which includes the
`num_rows` member and the pointers for the three member arrays. The second
directive then copies the three arrays to the device. Now that we've created
space on the GPU for these arrays, it's necessary to clean up the space when
we're done. In the `free_matrix` function, add the following directives
immediately before the calls to `free`.


    #pragma acc exit data delete(A.row_offsets,A.cols,A.coefs)
    #pragma acc exit data delete(A)
      free(row_offsets);
      free(cols);
      free(coefs);

Notice that we are performing the operations in the reverse order. First we are
deleting the 3 member arrays from the device, then we are deleting the
structure containing those arrays. It's also critical that we place our pragmas
*before* the arrays are freed on the host, otherwise the `exit data` directives
will fail.

Now go into `vector.h` and do the same thing in `allocate_vector` and
`free_vector` with the structure `v` and its member array `v.coefs`. Because 
we are copying the arrays to the device before they have been populated with 
data, use the `create` data clause, rather than `copyin`.

If you try
to build again at this point, the code will still fail to build because we
haven't told our compute regions that the data is already present on the
device, so the compiler is still trying to determine the array sizes itself.
Now go to the compute regions (`kernels` or `parallel loop`) in
`matrix_functions.h` and `vector_functions.h` and use the `present` clause to
inform the compiler that the arrays are already on the device. Below is an
example for `matvec`.

    #pragma acc kernels present(row_offsets,cols,Acoefs,xcoefs,ycoefs)

Once you have added the `present` clause to all three compute regions, the 
application should now build and run on the GPU, but is no longer getting
correct results. This is because we've put the arrays on the device, but we've
failed to copy the input data into these arrays. Add the following directive to
the end of `initialize_vector` function.

    #pragma acc update device(v.coefs[:v.n])

This will copy the data now in the host array to the GPU
copy of the array. With this data now correctly copied to the GPU, the code
should run to completion and give the same results as before.

### Fortran
To make the application return correct answers again, it will be necessary to
add explicit data management directives. This could be done using either the
structured `data` directives or unstructured `enter data` and `exit data`
directives, as discussed in the lecture. Since this program has clear routines
for allocating and initializing the data structures and also deallocating,
we'll use the unstructured directives to make the code easy to understand.

The `allocate_3d_poisson_matrix` in matrix.F90 handles allocating and
initializing the primary array. At the end of this routine, add the following
directive for copying the three arrays in the matrix type to the device.

    !$acc enter data copyin(arow_offsets,acols,acoefs)

These three arrays can be copied in seperate `enter data` directives as well.
Notice that because Fortran arrays are self-describing, it's unnecessary to
provide the array bounds, although it would be safe to do so as well. Since
we've allocated these arrays on the device, they should be removed from the
device when we are done with them as well. In the `free_matrix` subroutine of
matrix.F90 add the following directive.

    !$acc exit data delete(arow_offsets,acols,acoefs)
    deallocate(arow_offsets)
    deallocate(acols)
    deallocate(acoefs)

Notice that the `exit data` directive appears before the `deallocate`
statement. Because the OpenACC programming model assumes we always begin and
end execution on the host, it's necessary to remove arrays from the device
before freeing them on the host to avoid an error or crash. Now go add 
`enter data` and `exit data` directives to vector.F90 as well. Notice that the
`allocate_vector` routine only allocates the array, but does not initialize it,
so `copyin` may be replaced with `create` on the `enter data` directive.

If we build and run the application at this point we should see our tolerance
changing once again, but the answers will still be incorrect. Next let go to
each compute directive (`kernels` or `parallel loop`) in matrix.F90 and
vector.F90 and inform the compiler that the arrays used in those regions are
already present on the device. Below is an example from matrix.F90.

    !$acc kernels present(arow_offsets,acols,acoefs,x,y)

At this point the compiler knows that it does not need to be concerned with
data movement in our compute regions, but we're still getting the wrong answer.
The last change we need to make is to make sure that we're copying the input
data to the device before execution. In vector.F90 add the following directive
to the end of `initialize_vector`.

    vector(:) = value
    !$acc update device(vector)

Now that we have the correct input data on the device the code should run
correctly once again.

---

*(NOTE for C/C++ and Fortran: One could also parallelize the loop in
`initialize_vector` on the GPU, but we choose to use the `update` directive
here to illustrate how this directive is used.)*
