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
!! @file sk.F90
!! @short S(k) analysis: direct reciprocal space calculation
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION s_of_k (NQ, XA) BIND (C,NAME='s_of_k_')

! Total structure factor
! Partial structure factors from k-points

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: NQ, XA
DOUBLE PRECISION :: factor, xfactor

INTERFACE
  DOUBLE PRECISION FUNCTION FQX(TA, Q)
    INTEGER, INTENT(IN) :: TA
    DOUBLE PRECISION, INTENT(IN) :: Q
  END FUNCTION
END INTERFACE

if(allocated(Sij)) deallocate(Sij)
allocate(Sij(NQ,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k"//CHAR(0), "Table: Sij"//CHAR(0))
  s_of_k = 0
  goto 001
endif
Sij(:,:,:)=0.0d0

if(allocated(cij)) deallocate(cij)
allocate(cij(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k"//CHAR(0), "Table: cij"//CHAR(0))
  s_of_k = 0
  goto 001
endif
if(allocated(sik)) deallocate(sik)
allocate(sik(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k"//CHAR(0), "Table: sik"//CHAR(0))
  s_of_k = 0
  goto 001
endif

#ifdef OPENMP
  !t0 = OMP_GET_WTIME ()
  call FOURIER_TRANS_QVECT () ! Default Q-vector parallelization
  !t1 = OMP_GET_WTIME ()
  !write (*,*) "temps d’excecution QVT 2:", t1-t0
#else
  call FOURIER_TRANS_STEPS ()
#endif

if (allocated(cij)) deallocate(cij)
if (allocated(sik)) deallocate(sik)
if (allocated(qvectx)) deallocate(qvectx)
if (allocated(qvecty))deallocate(qvecty)
if (allocated(qvectz)) deallocate(qvectz)
if (allocated(modq)) deallocate(modq)

if(allocated(S)) deallocate(S)
allocate(S(NQ), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k"//CHAR(0), "Table: S"//CHAR(0))
  s_of_k = 0
  goto 001
endif
if(allocated(XS)) deallocate(XS)
allocate(XS(NQ), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k"//CHAR(0), "Table: XS"//CHAR(0))
  s_of_k = 0
  goto 001
endif

factor=0.0d0
do i=1, NSP
  factor=factor + NBSPBS(i)*NSCATTL(i)**2
enddo
factor=factor*NS

if (XA .eq. 1) then
  xfactor=0.0d0
  do i=1, NSP
    xfactor=xfactor + NBSPBS(i)*XSCATTL(i)**2
  enddo
  xfactor=xfactor*NS
endif

S(:)=0.0d0
XS(:)=0.0d0

do i=1,  NQ
  if (degeneracy(i) .gt. 0) then
    do k=1, NSP
      do j=1, NSP
        S(i)=S(i)+Sij(i,k,j)*NSCATTL(k)*NSCATTL(j)
        if (XA .eq. 1) then
          XS(i)=XS(i)+Sij(i,k,j)*XSCATTL(k)*XSCATTL(j)
        else
          XS(i)=XS(i)+Sij(i,k,j)*FQX(INT(XSCATTL(k)),K_POINT(i))*FQX(INT(XSCATTL(j)),K_POINT(i))
        endif
      enddo
    enddo
    S(i)=S(i)/(factor*degeneracy(i))
    if (XA .eq. 1) then
      XS(i)=XS(i)/(xfactor*degeneracy(i))
    else
      xfactor=0.0d0
      do k=1, NSP
        xfactor=xfactor + NBSPBS(k)*FQX(INT(XSCATTL(k)),K_POINT(i))**2
      enddo
      XS(i)=XS(i)/(xfactor*degeneracy(i)*NS)
    endif
    do j=1, NSP
      do k=1, NSP
        Sij(i,j,k)=Sij(i,j,k)/(degeneracy(i)*sqrt(dble(NBSPBS(k)*NBSPBS(j)))*NS)
      enddo
    enddo
  endif
enddo

s_of_k = SK_SAVE ()

001 continue

if (allocated(degeneracy)) deallocate(degeneracy)
if (allocated(K_POINT)) deallocate(K_POINT)
if (allocated(Sij)) deallocate(Sij)
if (allocated(S)) deallocate(S)
if (allocated(XS)) deallocate(XS)

CONTAINS

!************************************************************
!
! this subroutine computes the sine and cosine sums from the
! configuration for all q-vectors needed
!
SUBROUTINE FOURIER_TRANS_STEPS ()

  USE PARAMETERS

  IMPLICIT NONE
  DOUBLE PRECISION :: qx, qy, qz, qtr, sini, cosi
  INTEGER :: sp

  do k=1, NS
    do j=1, NUMBER_OF_QVECT

      l=AnINT((modq(j)-qvmin)/DELTA_Q)+1

      if (l .le. NQ) then

        qx=qvectx(j)
        qy=qvecty(j)
        qz=qvectz(j)

        do i=1, NSP
          cij(i)=0.d0
          sik(i)=0.d0
        enddo

        do i=1, NA
          qtr = qx*FULLPOS(i,1,k)+qy*FULLPOS(i,2,k)+qz*FULLPOS(i,3,k)
          sini = sin(qtr)
          cosi = cos(qtr)
          sp = LOT(i)
          sik(sp) = sik(sp) + sini
          cij(sp) = cij(sp) + cosi
        enddo

        do i=1, NSP
        do m=1, NSP
          Sij(l,i,m) = Sij(l,i,m) + cij(i)*cij(m) + sik(i)*sik(m)
        enddo
        enddo

     endif
    enddo
  enddo

END SUBROUTINE

#ifdef OPENMP
!************************************************************
!
! This subroutine computes the sine and cosine sums from the
! configuration for all q-vectors needed
! OpenMP // on Qvect
!
SUBROUTINE FOURIER_TRANS_QVECT ()

  USE PARAMETERS

  !$ USE OMP_LIB
  IMPLICIT NONE

  INTEGER :: NUMTH, sp
  DOUBLE PRECISION :: qx, qy, qz, qtr, sini, cosi

  NUMTH = OMP_GET_MAX_THREADS ()
  if (NUMBER_OF_QVECT.lt.NUMTH) NUMTH=NUMBER_OF_QVECT
  ! OpemMP on Qvect
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(qx, qy, qz, cij, sik, qtr, sini, cosi, i, j, k, l, m, sp) &
  !$OMP& SHARED(NUMTH, qvectx, qvecty, qvectz, &
  !$OMP& FULLPOS, NS, NSP, NA, LOT, DELTA_Q, NUMBER_OF_QVECT, NQ, modq, qvmin, Sij)
  !$OMP DO SCHEDULE(STATIC,NUMBER_OF_QVECT/NUMTH)
  do j=1, NUMBER_OF_QVECT

    l=AnINT((modq(j)-qvmin)/DELTA_Q)+1
    if (l .le. NQ) then

      qx=qvectx(j)
      qy=qvecty(j)
      qz=qvectz(j)

      do k=1, NS

        do i=1, NSP
          cij(i)=0.d0
          sik(i)=0.d0
        enddo

        do i=1, NA
          qtr = qx*FULLPOS(i,1,k)+qy*FULLPOS(i,2,k)+qz*FULLPOS(i,3,k)
          sini = sin(qtr)
          cosi = cos(qtr)
          sp = LOT(i)
          sik(sp) = sik(sp) + sini
          cij(sp) = cij(sp) + cosi
        enddo

        do i=1, NSP
        do m=1, NSP
          !$OMP ATOMIC
          Sij(l,i,m) = Sij(l,i,m) + cij(i)*cij(m) + sik(i)*sik(m)
        enddo
        enddo

      enddo

    endif

  enddo

  !$OMP END DO NOWAIT
  !$OMP END PARALLEL

END SUBROUTINE
#endif

INTEGER FUNCTION SK_SAVE()

USE PARAMETERS

INTEGER :: NSQ
DOUBLE PRECISION, DIMENSION (:), ALLOCATABLE :: SQTAB

INTERFACE
  LOGICAL FUNCTION FZBT (NDQ, SQIJ)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: NDQ
    DOUBLE PRECISION, DIMENSION(NDQ,NSP,NSP), INTENT(IN) :: SQIJ
  END FUNCTION
END INTERFACE

i=0
do j=1, NQ
  if (degeneracy(j) .gt. 0) i=i+1
enddo
NSQ=i

if (NSQ .gt. 0) then  ! If wave vectors exist

  if (allocated(SQTAB)) deallocate(SQTAB)
  allocate(SQTAB(NSQ), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: SK_SAVE"//CHAR(0), "Table: SQTAB"//CHAR(0))
    SK_SAVE = 0
    goto 001
  endif

  i = 0;
  do k=1, NQ
    if (degeneracy(k).gt.0) then
      i=i+1
      SQTAB(i)= K_POINT(k)
    endif
  enddo
  call save_xsk (NSQ, SQTAB)

  i=0
  do k=1, NQ
    if (degeneracy(k).gt.0) then
      i=i+1
      SQTAB(i)= S(k)
    endif
  enddo
  call save_curve (NSQ, SQTAB, 0, IDSK)

  i=0
  do k=1, NQ
    if (degeneracy(k).gt.0) then
      i=i+1
      SQTAB(i)= (S(k)-1.0)*K_POINT(k)
    endif
  enddo
  call save_curve (NSQ, SQTAB, 2, IDSK)

  i=0
  do k=1, NQ
    if (degeneracy(k).gt.0) then
      i=i+1
      SQTAB(i)= XS(k)
    endif
  enddo
  call save_curve (NSQ, SQTAB, 4, IDSK)

  i=0
  do k=1, NQ
    if (degeneracy(k).gt.0) then
      i=i+1
      SQTAB(i)= (XS(k)-1.0)*K_POINT(k)
    endif
  enddo
  call save_curve (NSQ, SQTAB, 6, IDSK)

  l = 8
  do i=1, NSP
    do j=1, NSP
      m=0
      do k=1, NQ
        if (degeneracy(k).gt.0) then
          m=m+1
          SQTAB(m)=Sij(k,i,j)
        endif
      enddo
      call save_curve (NSQ, SQTAB, l, IDSK)
      l=l+2
    enddo
  enddo

  !  To compute FZ and BT partials
  if (.not.FZBT (NQ, Sij)) then
    SK_SAVE = 0
    goto 001
  endif

  do i=1, NSP
    do j=1, NSP
      m=0
      do k=1, NQ
        if (degeneracy(k).gt.0) then
          m=m+1
          SQTAB(m)= FZSij(k,i,j)
        endif
      enddo
      call save_curve (NSQ, SQTAB, l, IDSK)
      l=l+2
    enddo
  enddo
  if (NSP .eq. 2) then
    do i=1, 4
      k=0
      do j=1, NQ
        if (degeneracy(j).gt.0) then
          k=k+1
          SQTAB(k)= BTij(j,i)
        endif
      enddo
      call save_curve (NSQ, SQTAB, l, IDSK)
      l=l+2
    enddo
  endif

  SK_SAVE=1

endif ! If wave vectors exist

001 continue

if (allocated(FZSij)) deallocate(FZSij)
if (NSP.eq.2 .and. allocated(BTij)) deallocate(BTij)
if (allocated(SQTAB)) deallocate(SQTAB)

END FUNCTION

END FUNCTION s_of_k

INTEGER (KIND=c_int) FUNCTION smooth_and_save (DPOINT, CTS, SFC, IDC, NQPTS, DATS) BIND (C,NAME='smooth_and_save_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: IDC, NQPTS, DATS
REAL (KIND=c_double), DIMENSION(NQPTS), INTENT(IN) :: DPOINT, CTS
REAL (KIND=c_double), INTENT(IN) :: SFC
DOUBLE PRECISION, DIMENSION (:), ALLOCATABLE :: SQTAB

INTERFACE
  LOGICAL FUNCTION SMOOTH (TABTOLISS, GTOLISS, DIMTOLISS, SIGMALISS)
    INTEGER, INTENT(IN) :: DIMTOLISS
    DOUBLE PRECISION, INTENT(IN) :: SIGMALISS
    DOUBLE PRECISION, INTENT(IN), DIMENSION(DIMTOLISS) :: GTOLISS
    DOUBLE PRECISION, INTENT(INOUT), DIMENSION(DIMTOLISS) :: TABTOLISS
  END FUNCTION
END INTERFACE

if (allocated(SQTAB)) deallocate(SQTAB)

allocate(SQTAB(NQPTS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: smooth_and_save"//CHAR(0), "Table: SQTAB"//CHAR(0))
  smooth_and_save=0
  goto 001
endif

do k=1, NQPTS
  SQTAB(k)=CTS(k)
enddo

if (.not.SMOOTH (SQTAB, DPOINT, NQPTS, SFC)) then
  smooth_and_save=0
  goto 001
endif

call save_curve (NQPTS, SQTAB, IDC, DATS)

smooth_and_save=1

001 continue

if (allocated(SQTAB)) deallocate(SQTAB)

END FUNCTION

DOUBLE PRECISION FUNCTION FQX(TA, Q)

USE MENDELEIEV

INTEGER, INTENT(IN) :: TA
DOUBLE PRECISION, INTENT(IN) :: Q
DOUBLE PRECISION :: SINLA
DOUBLE PRECISION, PARAMETER :: PI=acos(-1.0)
!
! Acta Cryst. (1968). A24, 321
!
! SINLA = ((sin(THETA)/LAMBDA)**2
! and 2 d Sin(THETA) = LAMBDA
! so 2 d = LAMBDA / Sin(THETA)
! with d = 2 PI / Q we get:
! 4.0 PI / Q =  LAMBDA / Sin(THETA)
SINLA=(Q/(4.0*PI))**2

FQX = a1(TA)*exp(-b1(TA)*SINLA) &
    + a2(TA)*exp(-b2(TA)*SINLA) &
    + a3(TA)*exp(-b3(TA)*SINLA) &
    + a4(TA)*exp(-b4(TA)*SINLA) + c(TA)

END FUNCTION
