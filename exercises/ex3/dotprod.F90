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
  
  ! TODO: 
  ! Implement dot product with OpenACC on device
  ! sum = (vecA, vecB) = vecB^T * vecA
     
  write(*,'(A30,X,ES18.5)') 'Sum using OpenACC reduction:', sum

end program dotprod
