## Data transfers

The file `ex4_data(.c|.F90)` implements a Jacobi iteration with OpenACC on the
device. Modify the code in such a way that the computation is done entirely on
the device, i.e., no additional data transfers take place between the host and
the device.
