program ex3_helloacc
  use mpi
#ifdef _OPENACC
  use openacc
#endif

  implicit none

  integer, parameter :: vecsize_def = 1024
  integer, parameter :: dp = selected_real_kind(12)

  character(len=MPI_MAX_PROCESSOR_NAME) :: nodename
  integer :: rank, nprocs, noderank, nodenprocs, devcount

  character(len=80) :: buf
  integer :: p, vecsize, rc, allocstat
  real(kind=dp), allocatable :: vec(:)
  real(kind=dp) :: csum

  call MPI_Init(rc)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, rc)
  call MPI_Comm_size(MPI_COMM_WORLD, nprocs, rc)

  select case (command_argument_count())
  case(0)
     vecsize = vecsize_def
  case(1)
     call get_command_argument(1,buf)
     read (buf, *) vecsize
  case default
     call get_command_argument(0, buf)
     if (rank == 0) call usage(trim(buf))
     call MPI_Barrier(MPI_COMM_WORLD, rc)
     call MPI_Finalize(rc)
     stop
  end select

  call getnodeinfo(nodename, noderank, nodenprocs, devcount)
#ifdef _OPENACC
  if (nodenprocs > devcount) then
     write (*,*) 'Not enough GPUs for all processes in the node.'
     call MPI_Abort(MPI_COMM_WORLD, -1, rc)
     call MPI_Finalize(rc)
     stop
  end if
#endif


  allocate(vec(vecsize), stat=allocstat)
  if (allocstat /= 0) then
     write (*,*) 'Memory allocation failed!'
     call MPI_Abort(MPI_COMM_WORLD, -1, rc)
     call MPI_Finalize(rc)
     stop
  end if
  
  !$acc data create(vec(1:vecsize))

  ! Initialize the data
  call initialize(vecsize, vec)

  ! Broadcast data vector from rank 0 to all other processes
  !$acc host_data use_device(vec)
  call MPI_Bcast(vec, vecsize, MPI_DOUBLE_PRECISION, 0, MPI_COMM_WORLD, rc)
  !$acc end host_data

  ! Compute with OpenACC
  call compute(vecsize, vec, rank)
  
  if (rank == 0) then
     ! Receive results from other ranks
     do p=1,nprocs-1
        !$acc host_data use_device(vec)
        call MPI_Recv(vec, vecsize, MPI_DOUBLE_PRECISION, p, &
             MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE, rc)
        !$acc end host_data

        ! Compute sum of vector
        csum = checksum(vecsize, vec)

        write (*,'("Rank: ", I0, ", element average=", ES12.3)') p, csum/vecsize
     end do
  else 
     ! Send result back to rank 0

     !$acc host_data use_device(vec)
     call MPI_Send(vec, vecsize, MPI_DOUBLE_PRECISION, 0, &
          0, MPI_COMM_WORLD, rc)
     !$acc end host_data
  end if 

  !$acc end data

  call MPI_Finalize(rc)
  deallocate(vec)

  contains 
    
    subroutine initialize(vecsize, vec)
      implicit none
      integer, intent(in) :: vecsize
      real(kind=dp) :: vec(vecsize)
      
      integer :: i

      !$acc parallel loop present(vec(1:vecsize))
      do i=1,vecsize
         vec(i) = real(0, dp)
      end do
    end subroutine initialize

    subroutine compute(vecsize, vec, rank)
      implicit none
      integer, intent(in) :: vecsize
      real(kind=dp) :: vec(vecsize)
      integer, intent(in) :: rank
      
      integer :: i

      !$acc parallel loop present(vec(1:vecsize))
      do i=1,vecsize
         vec(i) = vec(i) + rank
      end do
    end subroutine compute

    function checksum(vecsize, vec) result(csum)
      implicit none
      
      integer, intent(in) :: vecsize
      real(kind=dp), intent(in) :: vec(:)
      real(kind=dp) :: csum
      
      integer :: i

      csum = 0
      !$acc parallel loop present(vec(1:vecsize)) reduction(+:csum)
      do i=1,vecsize
         csum = csum+vec(i)
      end do
      !$acc end parallel loop

    end function checksum

    subroutine getnodeinfo(nodename, noderank, nodeprocs, devcount)
      implicit none

      character(len=MPI_MAX_PROCESSOR_NAME) :: nodename
      integer :: noderank
      integer :: nodeprocs
      integer :: devcount

      integer :: hostid
      integer :: namelen
      integer :: icomm
      integer :: mask, rc
#ifdef _OPENACC
      integer(kind=acc_device_kind) :: accdevtype
#endif

      ! Interface to gethostid() -function from unistd.h
      interface 
         function gethostid() result(id) bind(C)
           implicit none
           integer :: id
         end function gethostid
      end interface

      hostid = gethostid()
      call MPI_Get_processor_name(nodename, namelen, rc)
      mask = IAND(INT(z'7FFFFFFF'), hostid)
      call MPI_Comm_split(MPI_COMM_WORLD, mask, 0, icomm, rc)

      call MPI_Comm_rank(icomm, noderank, rc)
      call MPI_Comm_size(icomm, nodeProcs, rc)
      call MPI_Comm_free(icomm, rc)

      devcount = 0
#ifdef _OPENACC
      accdevtype = acc_get_device_type()
      devcount = acc_get_num_devices(accdevtype)
#endif
    end subroutine getnodeinfo

    subroutine usage(pname)
      implicit none
      character(len=*), intent(in) :: pname

      write (*,'("Usage: ",A," [n], where n is the vector length")'), pname

    end subroutine usage

end program ex3_helloacc
