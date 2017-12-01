program curand_demo
  use curand
  use openacc
  use, intrinsic :: ISO_FORTRAN_ENV, only : real32, real64
  implicit none
  integer, parameter :: sp = real32
  real(sp), allocatable :: x(:), y(:)
  character(len=256) :: argument
  integer :: n, inside, i
  type(curandGenerator) :: g
  real :: pi
  logical :: istat
  
  select case(command_argument_count())
  case(0)
     write(*,*) 'Usage: ./curand_openacc N'
     stop
  case(1)
     call get_command_argument(1, argument)
     read(argument,*) n
  end select
  
  print *, 'Value of pi is ', cpu_pi(n)  
  print *, 'or ', gpu_pi(n)
  
contains

  function gpu_pi(n) result(pi)
    implicit none
    integer, intent(in) :: n
    real(kind=sp) :: pi
    real(kind=sp), dimension(:), allocatable :: x, y
    integer :: inside, i
    logical :: istat
    
    allocate(x(n), y(n))

    ! TODO: Add here the initialization call as instructed in the README.md
    !       Create appropriate data region and generate the random numbers using
    !      curand call. Add correct pragmas for the computation loop.
    istat = curandCreateGenerator(g, CURAND_RNG_PSEUDO_DEFAULT)

    !$acc data create(x(1:n), y(1:n)) copy(inside) copyin(n)
    !$acc host_data use_device(x, y)
    istat = curandGenerateUniform(g, x, n)
    istat = curandGenerateUniform(g, y, n)
    !$acc end host_data
    
    !$acc parallel loop reduction(+:inside)
    do i = 1, n
       if (x(i)**2 + y(i)**2 < 1.0_sp) then
          inside = inside + 1
       end if
    end do
    !$acc end data

    ! TODO: Free the random number generator
    istat = curandDestroyGenerator(g)

    deallocate(x, y)
    
    pi = 4.0_sp * real(inside, kind=sp)/real(n, kind=sp)
    
  end function gpu_pi
  
  function cpu_pi(n) result(pi)
    implicit none
    integer, intent(in) :: n
    real(kind=sp) :: pi
    real(kind=sp), dimension(:), allocatable :: x, y
    integer :: inside, i
    
    allocate(x(n), y(n))

    call random_number(x)
    call random_number(y)
    
    inside = 0
    do i = 1, n
       if (x(i)**2 + y(i)**2 < 1.0_sp) then
          inside = inside + 1
       end if
    end do

    deallocate(x)
    deallocate(y)
    
    pi = 4.0_sp * real(inside, kind=sp)/real(n, kind=sp)
  end function cpu_pi
  
end program curand_demo
