# Doubleloop

Parallelise a simple stencil update kernel with OpenACC parallel or kernels
pragmas.

The file `doubleloop(.c|.F90)` implements a simple stencil update kernel.
Search for a TODO tag and try to parallelize the given loop nest with OpenACC
parallel or kernels pragmas. Pay attention to the compiler diagnostics
output.

Compile a reference version without OpenACC and compare the results. Are you
able to get the same results between different runs?
