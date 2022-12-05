! �������1: �������н��� obj �в�ͬ����ĸ���
! ע���ó�����Ҫ���� numbering-countf-utils.c �еĺ���
      include 'mpif.h'
      parameter(n_max = 1000000, np_max = 1024)
      integer i, n, myrank, nprocs, obj(n_max), ierr
      double precision sum
*
      integer slen, scnts(np_max), sdsps(np_max), sbuffer(n_max)
      integer rlen, rcnts(np_max), rdsps(np_max), rbuffer(n_max)
      integer a, p
      integer rand0
*
      call MPI_Init(ierr)
      call MPI_Comm_size(MPI_COMM_WORLD, nprocs, ierr)
      call MPI_Comm_rank(MPI_COMM_WORLD, myrank, ierr)
*
      if (myrank .eq. 0) then
         print *, 'Enter n: '
         read(*,*) n
      endif
      call MPI_Bcast(n, 1, MPI_INTEGER, 0, MPI_COMM_WORLD, ierr)
      if (n .gt. n_max .or. nprocs .gt. np_max) then
         if (myrank .eq. 0) print *, 'n or nprocs too large!.'
         call MPI_Finalize(ierr)
         stop
      endif
*
*     ������� obj �е�����
*     ע��srand0, rand0 �Լ� qsortf ������ numbering-countf-utils.c �С�
      call srand0()
      do i = 1, n
         obj(i) = mod(rand0(), 2 * n * nprocs)
      enddo
*     �� obj ���� */
      call qsortf(obj, n)

* TODO: remove duplicate entries in obj

*
*     Now obj contains sorted integers
      write(*, 1000) myrank, (obj(i), i = 1, n)
1000  format(' proc ', i2, ':', 30(' ', i2))


*
*     �����跢�͵������̵����ݸ�����cnts(p) �����跢�͵�����p�����ݸ���
      do p = 1, nprocs
         scnts(p) = 0                   ! ��ʼ�� cnts
      enddo
      do i = 1, n
         p = mod(obj(i), nprocs)
         scnts(p + 1) = scnts(p + 1) + 1
      enddo

*     �����̽��� scnts()
      call MPI_Alltoall(scnts, 1, MPI_INTEGER, rcnts, 1, MPI_INTEGER,
     +                  MPI_COMM_WORLD, ierr)

*     ���� sdsps() �� rdsps()
      slen = 0
      rlen = 0
      do p = 1, nprocs
         sdsps(p) = slen
         rdsps(p) = rlen
         slen = slen + scnts(p)
         rlen = rlen + rcnts(p)
      enddo

      if (rlen .gt. n_max) then
         print *, 'proc ', myrank, ', rlen is too large: ', rlen
         call MPI_Abort(MPI_COMM_WORLD, 1, ierr)
      endif

*     ׼�����ͻ�����
      do i = 1, n
         p = mod(obj(i), nprocs)
         sbuffer(sdsps(p + 1) + 1) = obj(i)
         sdsps(p + 1) = sdsps(p + 1) + 1
      enddo

*     �ָ� sdsps ����
      do p = 1, nprocs
         sdsps(p) = sdsps(p) - scnts(p)
      enddo

*     �������ݵ�Ŀ�Ľ��� */
      call MPI_Alltoallv(sbuffer, scnts, sdsps, MPI_INTEGER,
     +                   rbuffer, rcnts, rdsps, MPI_INTEGER,
     +                   MPI_COMM_WORLD, ierr)

      call qsortf(rbuffer, rlen)

*     ���㱾�غ� */
      N = 0
      if (rlen .eq. 0) goto 300
      i = 1
100   a = rbuffer(i)
      N = N + 1
200   i = i + 1
      if (i .gt. rlen) goto 300
      if (rbuffer(i) .eq. a) goto 200
      goto 100
300   continue

*     ����ȫ�ֺ�
      i = N
      call MPI_Reduce(i, N, 1, MPI_INTEGER, MPI_SUM, 0, MPI_COMM_WORLD,
     +                ierr)

      if (myrank .eq. 0) print *, '���в�ͬ��������Ϊ: ', N

      call MPI_Finalize(ierr)
      stop
      end
