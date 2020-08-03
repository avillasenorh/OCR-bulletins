      program datphases

      parameter (maxph=200)
      parameter (maxnm=100)
      character*256 datfile
      character*8 phasej
      character*8 kphnm(maxph,maxnm)
      character*8 knoid(maxnm)
      integer nnm(maxph)
      integer nph(maxph)

      character*512 line

      logical lfound

c initialize station variables
c
c nph(i) = number of phases in file for phase i-1
c nnm(i) = number of phase names for phase i-1
c          (ideal situation would be only one name per phase)
c noid   = number of un-identified phases (iphj=999)
c nmnoid = number of phase names (phasej) with iphj=999

      noid=0
      nmnoid=0
      do i=1,maxph
         nph(i)=0
         nnm(i)=0
      enddo

      write(*,1002) 'Enter input ISC 96-byte format file (DAT): '
      read(*,1000) datfile

      open(1,file=datfile,status='old',
     & form='formatted',access='sequential',iostat=ios)

      i=0
   10 read(1,1000,iostat=ios,end=20) line
         i=i+1

c        read here iphj,phasej,iphi
         phasej='        '
         read(line,4000) icode
         if (icode.eq.5) then
            read(line,4005,iostat=ios) iphj,phasej,iphi
         elseif (icode.eq.6) then
            read(line,4006,iostat=ios) iphj,phasej,iphi
         else
            go to 10
         endif

         if (iphj.eq.999) then
             if (phasej.ne.'        ') then
                write(1010) 'WARNING: non-blank 999 phase:',phasej
                write(0,1000) line(1:lnblnk(line))
             endif
             noid=noid+1
             if (nmnoid.eq.0) then
                nmnoid=1
                knoid(1)=phasej
             else
                lfound=.FALSE.
                do j=1,nmnoid
                   if (phasej.eq.knoid(j)) then
                      lfound=.TRUE.
                   endif
                enddo
                if (.not.lfound) then
                   nmnoid=nmnoid+1
                   knoid(nmnoid)=phasej
                endif
             endif
         else if (iphj.lt.0) then
             write(0,1001) 'WARNING: invalid phase code:',iphj,phasej
             write(0,1000) line(1:lnblnk(line))
         else if (iphj.ge.maxph) then
             write(0,1001) 'WARNING: phase code greater than maxph:',
     &       iphj,phasej
             write(0,1000) line(1:lnblnk(line))
         else
             nph(iphj+1)=nph(iphj+1)+1
             if (nnm(iphj+1).eq.0) then
                nnm(iphj+1)=1
                kphnm(iphj+1,1)=phasej
             else
                lfound=.FALSE.
                do j=1,nnm(iphj+1)
                   if (phasej.eq.kphnm(iphj+1,j)) then
                      lfound=.TRUE.
                   endif
                enddo
                if (.not.lfound) then
                   nnm(iphj+1)=nnm(iphj+1)+1
                   kphnm(iphj+1,nnm(iphj+1))=phasej
                endif
             endif
         endif

      go to 10

   20 write(0,1001) 'Number of lines read in:',i

      close(1)


      open(2,file='isc.phases',status='unknown')
      do i=1,maxph
         if (nph(i).gt.0) then
         write(2,2000) i-1,nph(i),nnm(i),(kphnm(i,j),j=1,nnm(i))
         endif
      enddo
      write(2,2000) 999,noid,nmnoid,(knoid(j),j=1,nmnoid)
      close(2)

 1000 format(a)
 1001 format(a,i12,4x,a8)
 1002 format(a,$)
 1010 format(a,2x,'**',a8,'**')
 2000 format(3i10,4x,20(1x,a8))
 4000 format(i2)
 4005 format(45x,i3,a8,4x,i3)
 4006 format(24x,i3,a8,4x,i3)

      stop
      end
