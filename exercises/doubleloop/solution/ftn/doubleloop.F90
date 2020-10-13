program doubleloop
  use, intrinsic :: iso_fortran_env, only : real64
#ifdef _OPENACC
  use openacc
#endif
  implicit none

  integer, parameter :: dp = real64

  real(kind=dp) :: eps
  real(kind=dp), allocatable, dimension(:,:) :: u, unew
  real(kind=dp) :: norm, mlups, checksum
  integer :: nx,ny,iter,maxth,nargs,ndef,i,j
  integer, parameter :: niter = 2
  character(len=12) arg
  real(kind=dp), parameter :: factor = 0.25_dp
  real(kind=dp) :: t, dt

  eps = 0.5e-3_dp
  ndef = 1022
  nargs = command_argument_count()

  nx = ndef
  ny = ndef

  allocate(u(0:nx+1,0:ny+1))
  allocate(unew(0:nx+1,0:ny+1))

  call init(u)
  call init(unew)

  t = ftimer()

  do iter = 1, niter
     !$acc parallel
     !$acc loop
     do j = 1, ny
        !$acc loop
        do i = 1, nx
           unew(i,j) = factor * (u(i-1,j) + u(i+1,j) + u(i,j-1) + u(i,j+1))
        enddo
     enddo
     !$acc end parallel
     !$acc parallel
     !$acc loop
     do j = 1, ny
        !$acc loop
        do i = 1, nx
           u(i,j) = factor * (unew(i-1,j) + unew(i+1,j) + unew(i,j-1) + unew(i,j+1))
        enddo
     enddo
     !$acc end parallel
  enddo

  ! Check sum, do not parallelize this loop!
  checksum = 0.0_dp
  do j = 1, ny
     do i = 1, nx
        checksum = checksum + unew(i,j)
     end do
  end do

  deallocate(u,unew)

  mlups = real(iter, kind=dp) * real(nx, kind=dp) * real(ny, kind=dp) * real(1.0e-6, kind=dp)
  dt = ftimer() - t
  write(*,'(3(a,f12.6))')   'Stencil: Time =',dt,' sec, MLups/s=',mlups/dt,' check sum: ',checksum

contains

  subroutine init(new)
    implicit none
    real(kind=dp), intent(out) :: new(0:nx+1,0:ny+1)
    integer i,j

    do j = 0, ny + 1
       do i = 0, nx + 1
          new(i,j) = 0.0_dp
       enddo
    enddo

    new(:,ny+1) = 1.0_dp
    new(nx+1,:) = 1.0_dp

  end subroutine init

  function ftimer()
    implicit none
    real(kind=dp) :: ftimer
    integer :: t, rate

    call system_clock(t,count_rate=rate)
    ftimer = real(t,kind(ftimer))/real(rate,kind(ftimer))
  end function ftimer

end program doubleloop
