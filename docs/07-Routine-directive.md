---
title:  "OpenACC: routine directive"
author: CSC - IT Center for Science
date:   2019-10
lang:   en
---


# Function calls in compute regions

- Often it can be useful to call functions within loops to improve
  readability and modularisation
- By default OpenACC does not create accelerated regions for loops
  calling functions
- One has to instruct the compiler to compile a device version of the
  function


# Routine directive

- Define a function to be compiled for an accelerator as well as the host
    - C/C++: `#pragma acc routine (name) [clauses]`
    - Fortran: `!$acc routine (name) [clauses]`
- The directive should be placed at the function declaration
    - Visible both to function definition (actual code) and call site
- Optional name enables the directive to be declared separately


# Routine directive

- Clauses defining level of parallelism in function
    - `gang` Function contains gang level parallelism
    - `worker` Function contains worker level parallelism
    - `vector` Function contains vector level parallelism
    - `seq` Function is not OpenACC parallel
- Other clauses
    - `nohost` Do not compile host version
    - `bind(string)` Define name to use when calling function in
      accelerated region


# Routine directive example

<div class="column">
## C/C++
```c
#pragma acc routine vector
void foo(float* v, int i, int n) {
    #pragma acc loop vector
    for ( int j=0; j<n; ++j) {
        v[i*n+j] = 1.0f/(i*j);
    }
}

#pragma acc parallel loop
for (int i=0; i<n; ++i) {
    foo(v,i);
    // call on the device
}
```
<small>
Example from Nvida devblog: <https://bit.ly/2Vl4weF/>
</small>
</div>

<div class="column">
## Fortran
```fortran
subroutine foo(v, i, n)
  !$acc routine vector
  real :: v(:,:)
  integer :: i, n
  !$acc loop vector
  do j=1,n
     v(i,j) = 1.0/(i*j)
  enddo
end subroutine

!$acc parallel loop
do i=1,n
  call foo(v,i,n)
enddo
!$acc end parallel loop
```
</div>


# Summary

- Routine directive
    - Enables one to write device functions that can be called within
      parallel loops
