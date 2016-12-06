NVIDIA OpenACC Course - Lab 3
=============================

In this lab you will build upon the work on a Conjugate Gradient
solver from [lab
2](https://github.com/NVIDIA-OpenACC-Course/nvidia-openacc-course-sources/tree/master/october-2015/labs/lab2)
to add explicit data management directives, eliminating the need to
use CUDA Unified Memory, and optimize the `matvec` kernel using the
OpenACC `loop` directive.


Steps
---------

* [Steps 0 and 1](steps-0-1.md)
* [Steps 2 to 5](steps-2-5.md)


Conclusion
----------

In this lab we started with a code that relied on CUDA Unified Memory to handle
data movement and added explicit OpenACC data locality directives. This makes
the code portable to any OpenACC compiler and accelerators that may not have
Unified Memory. We used both the unstructured data directives and the `update`
directive to achieve this.

Next we profiled the code to determine how it could run more efficiently on the
GPU we're using. We used our knowledge of both the application and the hardware
to find a loop mapping that ran well on the GPU, achieving a 2-4X speed-up over
our starting code.

The table below shows runtime for each step of this lab on an NVIDIA Tesla K40 
and on the Qwiklabs GPUs.

| Step             | K40       | Qwiklab GPU | 
| ---------------- | --------- | ----------- |
| Unified Memory   | 8.458172  | 32.084347   |
| Explicit Memory  | 8.459754  | 33.251878   | 
| Vector Length 32 | 11.656281 | 23.83046    |
| Final Code       | 4.802727  | 8.834735    | 
