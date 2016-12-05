## Lab 4 - Asynchronous operation

In this lab we will write an OpenACC version of a Mandelbrot solver,
optimizing its performance in 5 steps. 


 1. The base version of the mandelbrot solver runs on the CPU. Use OpenACC
 routine and parallel loop or kernels directive to run the solver on the GPU.

 2.  Break the image creation into blocks by adding a blocking loop around the
 existing loops and changing the “y” loop to operate on blocks.

 3. Change the data region to create the image array and use the update directive
to copy each block back upon completion.

 4. Use the block numbers to place blocks in a single async queue and wait for
the queue to complete.

 5. Use the block numbers to place blocks in multiple async queues and wait for all
queues to complete. Experiment with the number of blocks and queues.

