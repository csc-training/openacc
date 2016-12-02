## Lab 4 - Asynchronous operation

Perform the following 5 steps
 0. Use OpenACC routine and parallel loop or kernels directive to make
generate the image on the GPU.
 1. Break the image creation into blocks by adding a blocking loop around the
existing loops and changing the “y” loop to operate on blocks.
 2. Change the data region to create the image array and use the update directive
to copy each block back upon completion.
 3. Use the block numbers to place blocks in a single async queue and wait for the 
queue to complete. 
 4. Use the block numbers to place blocks in multiple async queues and wait for all
queues to complete. Experiment with the number of blocks and queues.