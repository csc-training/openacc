# Hello world with OpenACC

Compile and run a simple OpenACC test program, provided as `hello(.c|.F90)`.

In order to compile the program on Puhti, you'll need to first load the
following modules:
```bash
module load pgi/19.7 cuda/10.1.168
```

After this, you can compile the program using the PGI compiler (`pgcc` or
`pgf90`). Just remember to turn on support for OpenACC (`-acc` flag with PGI).

An example batch job script (`job.sh`) is provided and can be used to run the
program after fixing it with a valid account (project number).
