program vectorsum
#ifdef _OPENACC
  use openacc
#endif
  implicit none
  integer, parameter :: rk = selected_real_kind(12)
  integer, parameter :: ik = selected_int_kind(9)
  integer, parameter :: nx = 102400

  real(kind=rk), dimension(nx) :: vecA, vecB, vecC
  real(kind=rk)    :: sum
  integer(kind=ik) :: i

  ! Initialization of vectors
  do i = 1, nx
     vecA(i) = 1.0_rk/(real(nx - i + 1, kind=rk))
     vecB(i) = vecA(i)**2
  end do

  !$acc parallel loop copyin(vecA(1:nx), vecB(1:nx)) copyout(vecC(1:nx))
  do i = 1, nx
     vecC(i) = vecA(i) * vecB(i)
  end do
  !$acc end parallel loop

  ! Compute the check value
  write(*,*) 'Reduction sum: ', sum(vecC)
  
end program vectorsum
