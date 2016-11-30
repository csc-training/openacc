program dotprod
  implicit none
  integer, parameter :: rk = selected_real_kind(12)
  integer, parameter :: ik = selected_int_kind(9)
  integer, parameter :: nx = 102400

  real(kind=rk), dimension(nx) :: vecA, vecB
  real(kind=rk)    :: sum, psum
  integer(kind=ik) :: i

  ! Initialization of vectors
  do i = 1, nx
     vecA(i) = 1.0_rk/(real(nx - i + 1, kind=rk))
     vecB(i) = vecA(i)**2
  end do

  sum = dot_product(vecA,vecB)
  write(*,'(A30,X,ES18.5)') 'Sum on host:', sum

  sum = real(0,rk)
  ! Dot product on device using reduction
  !$acc parallel loop copyin(vecA(1:nx), vecB(1:nx)) reduction(+:sum)
  do i = 1, nx
     sum = sum + vecA(i) * vecB(i)
  end do
  !$acc end parallel loop
  write(*,'(A30,X,ES18.5)') 'Sum using OpenACC reduction:', sum

end program dotprod
