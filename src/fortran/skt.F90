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
!! @file skt.F90
!! @short S(k,t) analysis: dynamic structure factor calculation
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
!! @author Noël Jakse <noel.jakse@grenoble-inp.fr>

INTEGER (KIND=c_int) FUNCTION s_of_k_t (NQ_IN, XA_IN, MAX_IN) BIND (C,NAME='s_of_k_t_')

! Total and Partial Dynamic Structure Factor Calculation
!
!    S(q,t) = (1/N) * < \sum_{i} \sum_{j} exp( -i * q * ( r_i(t0+t) - r_j(t0) ) ) >
!
! Partial:
!
!    S_ab(q,t) ~ < \rho_a(q, t0+t) * conjg(\rho_b(q, t0)) >
!

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

! MAX_IN is the maximum correlation time to compute the structure factor
! Should be at least equal to NS/2
INTEGER (KIND=c_int), INTENT(IN) :: NQ_IN, XA_IN, MAX_IN
DOUBLE PRECISION :: factor, xfactor
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: RHO_C, RHO_S
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: NSQT, XSQT
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: LocalCorr
DOUBLE PRECISION, DIMENSION(:,:,:,:), ALLOCATABLE :: SQT

INTERFACE
  DOUBLE PRECISION FUNCTION FQX(TA, Q)
    INTEGER, INTENT(IN) :: TA
    DOUBLE PRECISION, INTENT(IN) :: Q
  END FUNCTION
END INTERFACE

allocate(SQT(NQ_IN, MAX_IN+1, NSP, NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: SQT"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif
SQT(:,:,:,:) = 0.0d0

allocate(NSQT(NQ_IN, MAX_IN+1), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: NSQT"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif
NSQT(:,:) = 0.0d0

allocate(XSQT(NQ_IN, MAX_IN+1), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: XSQT"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif
XSQT(:,:) = 0.0d0

! Allocate density arrays RHO_C and RHO_S
ALLOCATE(RHO_C(NS, NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: RHO_C"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif
ALLOCATE(RHO_S(NS, NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: RHO_S"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif
ALLOCATE(LocalCorr(MAX_IN+1, NSP, NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: LocalCorr"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif

#ifdef OPENMP
  !t0 = OMP_GET_WTIME ()
  call FOURIER_TRANS_QVECT_SKT (MAX_IN) ! Default Q-vector parallelization
  !t1 = OMP_GET_WTIME ()
  !write (*,*) "temps d’excecution QVT 2:", t1-t0
#else
  call FOURIER_TRANS_QVECT_SKT (MAX_IN)
#endif

if (allocated(qvectx)) deallocate(qvectx)
if (allocated(qvecty))deallocate(qvecty)
if (allocated(qvectz)) deallocate(qvectz)
if (allocated(modq)) deallocate(modq)

! Normalization and weighting (Neutron/X-ray)

factor=0.0d0
do i=1, NSP
  factor=factor + NBSPBS(i)*NSCATTL(i)**2
enddo

if (XA_IN .eq. 1) then
  xfactor=0.0d0
  do i=1, NSP
    xfactor=xfactor + NBSPBS(i)*XSCATTL(i)**2
  enddo
endif

do t=1, MAX_IN+1

  do i=1, NQ_IN

    if (degeneracy(i) .gt. 0) then
      do j=1, NSP
        do k=1, NSP
          ! Neutrons
          NSQT(i,t) = NSQT(i,t) + SQT(i,t,j,k) * NSCATTL(j) * NSCATTL(k)
          ! X-rays
          if (XA_IN .eq. 1) then
            XSQT(i,t) = XSQT(i,t) + SQT(i,t,j,k) * XSCATTL(j) * XSCATTL(k)
          else
            ! Use form factors FQX
            XSQT(i,t) = XSQT(i,t) + SQT(i,t,j,k) * FQX(INT(XSCATTL(j)), K_POINT(i)) * FQX(INT(XSCATTL(k)), K_POINT(i))
          endif
        enddo
      enddo

      ! Normalization
      NSQT(i,t) = NSQT(i,t) / (factor * degeneracy(i))

      if (XA_IN .eq. 1) then
        XSQT(i,t) = XSQT(i,t) / (xfactor * degeneracy(i))
      else
        ! Function FQX appears in sk.F90
        xfactor = 0.0d0
        do k=1, NSP
          xfactor = xfactor + NBSPBS(k) * FQX(INT(XSCATTL(k)), K_POINT(i))**2
        enddo
        XSQT(i,t) = XSQT(i,t) / (xfactor * degeneracy(i))
      endif

      ! Normalize Partials
      do j=1, NSP
        do k=1, NSP
          SQT(i,t,j,k) = SQT(i,t,j,k) / (degeneracy(i) * SQRT(DBLE(NBSPBS(j)*NBSPBS(k))))
        enddo
      enddo

    endif

  enddo

enddo

write (6, *) "Before saving"
s_of_k_t = SKT_SAVE ()
write (6, *) "After saving :: s_of_k_t = ",s_of_k_t

001 continue

if (allocated(SQT)) deallocate(SQT)
if (allocated(NSQT)) deallocate(NSQT)
if (allocated(XSQT)) deallocate(XSQT)
if (allocated(RHO_C)) deallocate(RHO_C)
if (allocated(RHO_S)) deallocate(RHO_S)
if (allocated(LocalCorr)) deallocate(LocalCorr)

CONTAINS

!************************************************************
!
! Compute S(q,t) loops over Q-vectors
! OpenMP // on Qvect
!
SUBROUTINE FOURIER_TRANS_QVECT_SKT (MAX_IN)

  USE PARAMETERS

  IMPLICIT NONE

  INTEGER, INTENT(IN) :: MAX_IN

  INTEGER :: q, n_origins, t0
  DOUBLE PRECISION :: qx, qy, qz, qtr
  DOUBLE PRECISION :: Corr

#ifdef OPENMP
  INTEGER :: NUMTH
  NUMTH = OMP_GET_MAX_THREADS ()
  if (NUMBER_OF_QVECT.lt.NUMTH) NUMTH=NUMBER_OF_QVECT

  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(qx, qy, qz, qtr, i, j, k, l, m, n, q, t0, t, n_origins, RHO_C, RHO_S, LocalCorr, Corr) &
  !$OMP& SHARED(NUMTH, NUMBER_OF_QVECT, SQT, NQ_IN, modq, qvmin, DELTA_Q) &
  !$OMP& SHARED(qvectx, qvecty, qvectz, FULLPOS, NS, NSP, NA, LOT, MAX_IN)
  !$OMP DO SCHEDULE(STATIC,NUMBER_OF_QVECT/NUMTH)
#endif
  do q=1, NUMBER_OF_QVECT

    l=AnINT((modq(q)-qvmin)/DELTA_Q)+1
    if (l .le. NQ_IN) then

      RHO_C(:,:) = 0.0d0
      RHO_S(:,:) = 0.0d0
      LocalCorr(:,:,:) = 0.0d0

      qx=qvectx(q)
      qy=qvecty(q)
      qz=qvectz(q)

      ! Compute density history for this Q vector
      do k=1, NS
        do i=1, NA
          j = LOT(i)
          qtr = qx*FULLPOS(i,1,k) + qy*FULLPOS(i,2,k) + qz*FULLPOS(i,3,k)
          RHO_C(k, j) = RHO_C(k, j) + cos(qtr)
          RHO_S(k, j) = RHO_S(k, j) + sin(qtr)
        enddo
      enddo

      do t=0, MAX_IN
        n_origins = NS - t
        do t0=1, n_origins
          do m=1, NSP
            do n=1, NSP
              ! Correlation Real Part: Rc(t)*Rc(0) + Rs(t)*Rs(0)
              Corr = RHO_C(t0+t, m) * RHO_C(t0, n) + RHO_S(t0+t, m) * RHO_S(t0, n)
              LocalCorr(t+1, m, n) = LocalCorr(t+1, m, n) + Corr
            enddo
          enddo
        enddo
        ! Normalize by n_origins here
        LocalCorr(t+1, :, :) = LocalCorr(t+1, :, :) / DBLE(n_origins)
      enddo

#ifdef OPENMP
     !$OMP CRITICAL
#endif
      SQT(l, :, :, :) = SQT(l, :, :, :) + LocalCorr(:, :, :)
#ifdef OPENMP
     !$OMP END CRITICAL
#endif
    endif

  enddo
#ifdef OPENMP
  !$OMP END DO
  !$OMP END PARALLEL
#endif

END SUBROUTINE

INTEGER FUNCTION SKT_SAVE ()

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

h = 8+4*NSP*NSP
if (NSP .eq. 2) h=h+8

if (allocated(SQTAB)) deallocate(SQTAB)
allocate(SQTAB(NQ_IN), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: SKT_SAVE"//CHAR(0), "Table: SQTAB"//CHAR(0))
  SKT_SAVE = 0
  goto 001
endif
if(allocated(Sij)) deallocate(Sij)
allocate(Sij(NQ_IN,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: SKT_SAVE"//CHAR(0), "Table: Sij"//CHAR(0))
  SKT_SAVE = 0
  goto 001
endif

do t=1, MAX_IN+1

  write (6 , *)
  write (6, '("t = ",i4)') t
  i=0
  do j=1, NQ_IN
    if (NSQT(j,t) .ne. 0.0) i=i+1
  enddo
  NSQ=i

  if (NSQ .gt. 0) then  ! If wave vectors exist

    SQTAB(:)=0.0d0
    i = 0;
    do k=1, NQ_IN
      if (k.eq.1 .or. NSQT(k,t).ne.0.0) then
        i=i+1
        SQTAB(i)= K_POINT(k)
      endif
    enddo
    ! To do for SKT
    ! call save_xsk (NSQ, SQTAB)

    i=0
    do k=1, NQ_IN
      if (k.eq.1 .or. NSQT(k,t).ne.0.0) then
        i=i+1
        SQTAB(i)= NSQT(k,t)
      endif
    enddo

    ! call save_curve (NSQ, SQTAB, (t-1)*h, IDSKT)

    i=0
    do k=1, NQ_IN
      if (k.eq.1 .or. NSQT(k,t).ne.0.0) then
        i=i+1
        SQTAB(i)= (NSQT(k,t)-1.0)*K_POINT(k)
      endif
    enddo
    ! call save_curve (NSQ, SQTAB, 2 + (t-1)*h, IDSKT)

    i=0
    do k=1, NQ_IN
      if (k.eq.1 .or. NSQT(k,t).ne.0.0) then
        i=i+1
        SQTAB(i)= XSQT(k,t)
      endif
    enddo
    ! call save_curve (NSQ, SQTAB, 4 + (t-1)*h, IDSKT)

    i=0
    do k=1, NQ_IN
      if (k.eq.1 .or. NSQT(k,t).ne.0.0) then
        i=i+1
        SQTAB(i)= (XSQT(k,t)-1.0)*K_POINT(k)
      endif
    enddo
    ! call save_curve (NSQ, SQTAB, 6 + (t-1)*h, IDSKT)

    SQTAB(:)=0.0d0
    Sij(:,:,:)=0.0d0
    l = 8
    do i=1, NSP
      do j=1, NSP
        m=0
        do k=1, NQ_IN
          Sij(k,i,j) = SQT(k,t,i,j)
          if (k.eq.1 .or. NSQT(k,t).ne.0.0) then
            m=m+1
            SQTAB(m)=Sij(k,i,j)
            if (i.eq.1 .and. j.eq.1) write (6, '(i4,3x,f15.10,4x,f15.10)') t-1, K_POINT(m), SQTAB(m)
          endif
        enddo
        ! call save_curve (NSQ, SQTAB, l + (t-1)*h, IDSKT)
        l=l+2
      enddo
    enddo

  !  To compute FZ and BT partials
    if (.not.FZBT (NQ_IN, Sij)) then
      SKT_SAVE = 0
      goto 001
    endif

    do i=1, NSP
      do j=1, NSP
        m=0
        do k=1, NQ_IN
          if (k.eq.1 .or. NSQT(k,t).ne.0.0) then
            m=m+1
            SQTAB(m)= FZSij(k,i,j)
            if (i.eq.1 .and. j.eq.1) write (6, '(i4,3x,f15.10,4x,f15.10)') t-1, K_POINT(m), SQTAB(m)
          endif
        enddo
        ! call save_curve (NSQ, SQTAB, l + (t-1)*h, IDSKT)
        l=l+2
      enddo
    enddo
    if (NSP .eq. 2) then
      do i=1, 4
        k=0
        do j=1, NQ_IN
          if (j.eq.1 .or. NSQT(j,t).ne.0.0) then
            k=k+1
            SQTAB(k)= BTij(j,i)
          endif
        enddo
        ! call save_curve (NSQ, SQTAB, l + (t-1)*h, IDSKT)
        l=l+2
      enddo
    endif

  endif ! If wave vectors exist

enddo

SKT_SAVE=1

001 continue

if (allocated(FZSij)) deallocate(FZSij)
if (NSP.eq.2 .and. allocated(BTij)) deallocate(BTij)
if (allocated(SQTAB)) deallocate(SQTAB)
if(allocated(Sij)) deallocate(Sij)
if(allocated(FZSij)) deallocate(FZSij)
if(allocated(BTij)) deallocate(BTij)

END FUNCTION

END FUNCTION s_of_k_t
