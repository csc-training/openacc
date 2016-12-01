## Hybrid Hello with MPI + OpenACC

Write a simple hybrid program where each OpenACC compute task
communicates using MPI. Implement the following steps.

1. The first MPI task sends a vector of some length initialized
to its rank id to all other tasks.
2. The other tasks then add their MPI rank to the vector by using
OpenACC.
3. Finally, the other tasks send the modified vector back to the
first MPI task which outputs its contents to stdout.
