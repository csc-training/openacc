program hello
#ifdef _OPENACC
  use openacc
#endif
  implicit none
#ifdef _OPENACC
  integer(acc_device_kind) :: devkind
#endif

  write (*,*) 'Hello world from OpenACC'
#ifdef _OPENACC
  devkind = acc_get_device_type()
  write (*,'(A,X,I0)') 'Number of available OpenACC devices:', acc_get_num_devices(devkind)
  write (*,'(A,X,I0)') 'Type of available OpenACC devices:', devkind
#else
  write (*,*) 'Code compiled without OpenACC'
#endif

end program hello
