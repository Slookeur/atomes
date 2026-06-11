! This file is part of the 'atomes' software.
!
! 'atomes' is free software: you can redistribute it and/or modify it under the terms
! of the GNU Affero General Public License as published by the Free Software Foundation,
! either version 3 of the License, or (at your option) any later version.
!
! 'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
! without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
! See the GNU General Public License for more details.
!
! You should have received a copy of the GNU Affero General Public License along with 'atomes'.
! If not, see <https://www.gnu.org/licenses/>
!
! Copyright (C) 2022-2026 by CNRS and University of Strasbourg
!
!>
!! @file cqvf.F90
!! @short q vectors selection for the reciprocal calculation of the S(k)
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION cqvf (QMAX, QMIN, NQ, PROBA, LIMQ) BIND (C,NAME='cqvf_')

!
! Determines the q-vectors for which the q-dependent
! correlation functions are computed - general cell
!

  USE PARAMETERS

  IMPLICIT NONE

  INTEGER (KIND=c_int), INTENT(IN) :: NQ
  REAL (KIND=c_double), INTENT(IN) :: PROBA, LIMQ
  REAL (KIND=c_double), INTENT(IN) :: QMAX, QMIN

  LOGICAL :: KPTS, KEEPKPTS
  INTEGER :: hkl, q_index, seed
  INTEGER :: klow, llow
  INTEGER, DIMENSION(3) :: NKPTS
  DOUBLE PRECISION, DIMENSION(3) :: qmrecip
  DOUBLE PRECISION :: QMAX2, QMIN2, LIMQ2
  DOUBLE PRECISION :: kpx, kpy, kpz, keep

  INTERFACE
    DOUBLE PRECISION FUNCTION RAN3 (idnum)
      INTEGER, INTENT(IN) :: idnum
    END FUNCTION
  END INTERFACE

  qmrecip(:) = 0.0d0
  do i=1, NCELLS
    do j=1, 3
      qmrecip(j) = qmrecip(j)+THE_BOX(i)%modr(j)
    enddo
  enddo
  qmrecip(:) = qmrecip(:)/NCELLS
  do i=1, 3
    NKPTS(i) = AnINT(QMAX/qmrecip(i))+1
  enddo
  hkl=0
! To simplify the case of the calculation in the case of a simple cubic box
  if (OVERALL_CUBIC) then
    QMAX2=(QMAX/QMIN)**2
    QMIN2=1.0d0
    LIMQ2=(LIMQ/QMIN)**2
  else
    QMAX2=QMAX**2                    ! mod(q_max)**2
    LIMQ2=LIMQ**2
    QMIN2=QMIN**2                    ! min module of the reciprocal lattice vectors - lattice.f90
  endif
  q_index=0
  qvmax=0.0d0
  qvmin=50.0d0

do i=1, 2

! The first iteration to evaluate the number of k-points
! to be saved for the analysis, and the second to store them.

  if (i .eq. 2) then

    NUMBER_OF_QMOD=q_index
    if (allocated(qvectx)) deallocate(qvectx)
    allocate(qvectx(NUMBER_OF_QMOD), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: COMP_Q_VAL_FULL"//CHAR(0), "Table: qvectx"//CHAR(0))
      CQVF=0
      goto 001
    endif
    if (allocated(qvecty)) deallocate(qvecty)
    allocate(qvecty(NUMBER_OF_QMOD), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: COMP_Q_VAL_FULL"//CHAR(0), "Table: qvecty"//CHAR(0))
      CQVF=0
      goto 001
    endif
    if (allocated(qvectz)) deallocate(qvectz)
    allocate(qvectz(NUMBER_OF_QMOD), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: COMP_Q_VAL_FULL"//CHAR(0), "Table: qvectz"//CHAR(0))
      CQVF=0
      goto 001
    endif
    if (allocated(modq)) deallocate(modq)
    allocate(modq(NUMBER_OF_QMOD), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: COMP_Q_VAL_FULL"//CHAR(0), "Table: modq"//CHAR(0))
      CQVF=0
      goto 001
    endif
    q_index=0

  endif

  do h=0, NKPTS(1)

    if (h .eq. 0) then
      klow=0
    else
      klow=-NKPTS(2)
    endif

    do k=klow, NKPTS(2)

      if (k.eq.0 .and. h.eq.0) then
        llow=0
      else
        llow=-NKPTS(3)
      endif

      do l=llow, NKPTS(3)

        if (OVERALL_CUBIC) then
          kpx= h
          kpy= k
          kpz= l
        else
          ! Note : because we only consider the first step this is an approximation in the case of NPT trajectory
          kpx= 2.0d0*PI*(h*THE_BOX(1)%lrecp(1,1) + k*THE_BOX(1)%lrecp(2,1) + l*THE_BOX(1)%lrecp(3,1))
          kpy= 2.0d0*PI*(h*THE_BOX(1)%lrecp(1,2) + k*THE_BOX(1)%lrecp(2,2) + l*THE_BOX(1)%lrecp(3,2))
          kpz= 2.0d0*PI*(h*THE_BOX(1)%lrecp(1,3) + k*THE_BOX(1)%lrecp(2,3) + l*THE_BOX(1)%lrecp(3,3))
        endif
        qvmod= kpx**2 + kpy**2 + kpz**2

!       we want the maximum number of k-points in the FSDP part of the spectra
!       therefore we choose to keep all qvectors with qmod in:
!                   minr < qmod < limq
!       where 'minr' is minimum q modulus accessible considering the analyzed
!       lattice ie. the minimum modulus of the reciprocal cell vectors,
!       'limq' is the limit to be fixed for the FSDP part of the spectra
!       Thereafter: QMIN=minr**2 and LIMQ2=limq**2.
!       The qvectors with a 'qmod > limq' are accepted with a probability of 'PROBA'

        if (qvmod .le. QMAX2 .and. qvmod .ge. QMIN2) then
          KPTS=.true.
        else
          KPTS=.false.
        endif
        if (i .eq. 1 .and. KPTS) then
          q_index=q_index+2
        elseif (i .eq. 2 .and. KPTS) then
          if (qvmod .le. LIMQ2) then
            KEEPKPTS=.true.
          else
            seed=173932
            keep = RAN3(seed+h**3+k**2+l**5)
            if (keep .le. PROBA) then
              KEEPKPTS=.true.
            else
              KEEPKPTS=.false.
            endif
          endif
          if (KEEPKPTS) then
            q_index=q_index+1
            qvectx(q_index)=kpx
            qvecty(q_index)=kpy
            qvectz(q_index)=kpz
            modq(q_index)=sqrt(qvmod)
            qvmax=max(qvmax,modq(q_index))
            qvmin=min(qvmin,modq(q_index))
            if (h.ne.0 .or. k.ne.0 .or. l.ne.0) then
              q_index=q_index+1
              qvectx(q_index)=-kpx
              qvecty(q_index)=-kpy
              qvectz(q_index)=-kpz
              modq(q_index)=sqrt(qvmod)
            endif
          endif
        endif

      enddo ! l

    enddo ! k

  enddo ! h

enddo

NUMBER_OF_QVECT=q_index

! NQ is given in input
! the value of each K_POINT is found in the
! interval |qvmax - qvmin| and then we compute
! the degeneracy of each K_POINT in q modulus.

if (allocated(degeneracy)) deallocate(degeneracy)
allocate(degeneracy(NQ+1), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: COMP_Q_VAL_FULL"//CHAR(0), "Table: degeneracy"//CHAR(0))
  CQVF=0
  goto 001
endif
degeneracy(:)=0
DELTA_Q=(qvmax-qvmin)/NQ

do i=1, NUMBER_OF_QVECT
  hkl=AnINT((modq(i)-qvmin)/DELTA_Q)+1
  degeneracy(hkl)=degeneracy(hkl)+1
enddo

if (allocated(K_POINT)) deallocate(K_POINT)
allocate(K_POINT(NQ), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: COMP_Q_VAL_FULL"//CHAR(0), "Table: K_POINT"//CHAR(0))
  CQVF=0
  goto 001
endif

do i=1, NQ
  K_POINT(i)=(i-1.0)*DELTA_Q+qvmin
enddo

if (OVERALL_CUBIC) then
  do i=1, NUMBER_OF_QVECT
    qvectx(i)=qvectx(i)*QMIN
    qvecty(i)=qvecty(i)*QMIN
    qvectz(i)=qvectz(i)*QMIN
  enddo
  do i=1, NQ
    K_POINT(i)=K_POINT(i)*QMIN
  enddo
endif

!do i=1, NQ
!  write (6, *) "i= ",i,", K_POINT(i)= ",K_POINT(i)
!enddo

CQVF=1

001 continue

END FUNCTION
