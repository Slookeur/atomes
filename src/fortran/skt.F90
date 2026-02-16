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

INTEGER (KIND=c_int) FUNCTION s_of_k_t (NQ_IN, XA_IN, MIN_IN, N_SETS, SETS_T, DELTA_T, Q_NUM, Q_LIST, N_FREQ) BIND (C,NAME='s_of_k_t_')

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

INTEGER (KIND=c_int), INTENT(IN) :: NQ_IN  ! Number of delta q
INTEGER (KIND=c_int), INTENT(IN) :: XA_IN  ! How to compute X rays
INTEGER (KIND=c_int), INTENT(IN) :: MIN_IN ! Minimum value of correlations
INTEGER (KIND=c_int), INTENT(IN) :: N_SETS ! Number of t steps to save, or -1 for all
INTEGER (KIND=c_int), DIMENSION(N_SETS), INTENT(IN) :: SETS_T
INTEGER (KIND=c_int), INTENT(IN) :: Q_NUM  ! Number q compute (q,w) data
INTEGER (KIND=c_int), INTENT(IN) :: N_FREQ ! Number of frequency points
DOUBLE PRECISION (KIND=c_double), DIMENSION(Q_NUM), INTENT(IN) :: Q_LIST

INTEGER, DIMENSION(:), ALLOCATABLE :: QID
DOUBLE PRECISION :: factor, xfactor
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: RHO_C, RHO_S
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: NSQT, XSQT
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: LocalCorr
DOUBLE PRECISION, DIMENSION(:,:,:,:), ALLOCATABLE :: SQT
DOUBLE PRECISION, DIMENSION (:), ALLOCATABLE :: SQTAB

INTERFACE
  DOUBLE PRECISION FUNCTION FQX(TA, Q)
    INTEGER, INTENT(IN) :: TA
    DOUBLE PRECISION, INTENT(IN) :: Q
  END FUNCTION
END INTERFACE

allocate(SQT(NQ_IN, NS-MIN_IN, NSP, NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: SQT"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif
SQT(:,:,:,:) = 0.0d0

allocate(NSQT(NQ_IN, NS-MIN_IN), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: NSQT"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif
NSQT(:,:) = 0.0d0

allocate(XSQT(NQ_IN, NS-MIN_IN), STAT=ERR)
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
ALLOCATE(LocalCorr(NS-MIN_IN, NSP, NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_k_t"//CHAR(0), "Table: LocalCorr"//CHAR(0))
  s_of_k_t = 0
  goto 001
endif

#ifdef OPENMP
  !t0 = OMP_GET_WTIME ()
  call FOURIER_TRANS_QVECT_SKT (MIN_IN) ! Default Q-vector parallelization
  !t1 = OMP_GET_WTIME ()
  !write (*,*) "temps d’excecution QVT 2:", t1-t0
#else
  call FOURIER_TRANS_QVECT_SKT (MIN_IN)
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

do t=1, NS-MIN_IN

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

if (Q_DIN .gt. 0) then

  if (allocated(SQTAB)) deallocate(SQTAB)
  allocate(SQTAB(NQ_IN), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: s_of_k_t"//CHAR(0), "Table: SQTAB"//CHAR(0))
    s_of_k_t = 0
    goto 001
  endif
  if (allocated(QID)) deallocate(QID)
  allocate(QID(NQ_IN), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: s_of_k_t"//CHAR(0), "Table: QID"//CHAR(0))
    s_of_k_t = 0
    goto 001
  endif
  SQTAB(:)=0.0d0
  NSQ = 0;
  do i=1, NQ_IN
    if (degeneracy(i) .gt. 0) then
      NSQ=NSQ+1
      SQTAB(NSQ)= K_POINT(i)
      QID(NSQ) = i
    endif
  enddo
  call save_xsk (NSQ, SQTAB)

  ALLOCATE(SQW_TAB(N_FREQ), STAT=ERR)
  if (ERR .ne. 0) then
     call show_error ("Impossible to allocate memory"//CHAR(0), &
                      "Function: s_of_k_t"//CHAR(0), "Table: SQW_TAB"//CHAR(0))
     s_of_k_t = 0
     goto 001
  endif

  COMPUTE_SQW (NSQ, QID, NSQT, DELTA_T, Q_NUM, Q_LIST, N_FREQ)

  COMPUTE_SQW (NSQ, QID, XSQT, DELTA_T, Q_NUM, Q_LIST, N_FREQ)

  ALLOCATE(SKT_TAB(NQ_IN, NS-MIN_IN), STAT=ERR)
  if (ERR .ne. 0) then
     call show_error ("Impossible to allocate memory"//CHAR(0), &
                      "Function: s_of_k_t"//CHAR(0), "Table: SKT_TAB"//CHAR(0))
     s_of_k_t = 0
     goto 001
  endif

  do j=1, NSP
    do k=1, NSP
      do l=1, NS-MIN_IN
        do m=1, NQ_IN
          SKT_TAB(l,m) = SQT(l,m,j,k)
        enddo
      enddo
      COMPUTE_SQW (NSQ, QID, SKT_TAB, DELTA_T, Q_NUM, Q_LIST, N_FREQ)
    enddo
  enddo

  if (allocated(SKT_TAB)) deallocate(SKT_TAB)
  if (allocated(SQW_TAB)) deallocate(SQW_TAB)

endif

s_of_k_t = SKT_SAVE ()

001 continue

if (allocated(SQTAB)) deallocate(SQTAB)
if (allocated(SKT_TAB)) deallocate(SKT_TAB)
if (allocated(SQW_TAB)) deallocate(SQW_TAB)
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
SUBROUTINE FOURIER_TRANS_QVECT_SKT (MIN_IN)

  USE PARAMETERS

  IMPLICIT NONE

  INTEGER, INTENT(IN) :: MIN_IN

  INTEGER :: q, NumCorr, t_n
  DOUBLE PRECISION :: qx, qy, qz, qtr
  DOUBLE PRECISION :: Corr

#ifdef OPENMP
  INTEGER :: NUMTH
  NUMTH = OMP_GET_MAX_THREADS ()
  if (NUMBER_OF_QVECT.lt.NUMTH) NUMTH=NUMBER_OF_QVECT
  ! OpemMP on Qvect
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(qx, qy, qz, qtr, i, j, k, l, m, n, q, t) &
  !$OMP& PRIVATE (t_n, NumCorr, RHO_C, RHO_S, LocalCorr, Corr) &
  !$OMP& SHARED(NUMTH, NUMBER_OF_QVECT, SQT, NQ_IN, modq, qvmin, DELTA_Q) &
  !$OMP& SHARED(qvectx, qvecty, qvectz, FULLPOS, NS, NSP, NA, LOT, MIN_IN)
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

      do t=1, NS
        ! Compute density history for this Q vector
        do i=1, NA
          j = LOT(i)
          qtr = qx*FULLPOS(i,1,t) + qy*FULLPOS(i,2,t) + qz*FULLPOS(i,3,t)
          RHO_C(t, j) = RHO_C(t, j) + cos(qtr)
          RHO_S(t, j) = RHO_S(t, j) + sin(qtr)
        enddo
      enddo

      ! If 'MIN_IN = 0' and 't = 0', then S(t=0) is the static structure factor
      do t=0, NS-1-MIN_IN
        NumCorr = NS-t-MIN_IN
        do t_n=1, NumCorr
          do m=1, NSP
            do n=1, NSP
              Corr = RHO_C(t+t_n, m) * RHO_C(t_n, n) + RHO_S(t+t_n, m) * RHO_S(t_n, n)
              LocalCorr(t+1, m, n) = LocalCorr(t+1, m, n) + Corr
            enddo
          enddo
        enddo
        ! Normalize by NumCorr here
        LocalCorr(t+1, :, :) = LocalCorr(t+1, :, :) / DBLE(NumCorr)
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

SUBROUTINE COMPUTE_SQW (N_Q_IN, NSQ, SQID, SKT_TAB, DELTA_T, Q_NUM, Q_LIST, N_FREQ)

  USE PARAMETERS

  IMPLICIT NONE

  INTEGER, INTENT(IN) :: N_Q_IN
  INTEGER, INTENT(IN) :: NSQ
  INTEGER, INTENT(IN) :: Q_NUM
  INTEGER, DIMENSION(NSQ), INTENT(IN) :: SQID
  INTEGER, DIMENSION(Q_NUM), INTENT(IN) :: Q_LIST
  INTEGER, INTENT(IN) :: N_FREQ
  DOUBLE PRECISION, DIMENSION (NQ_IN,NS-MIN_IN), INTENT(IN) :: SKT_TAB
  DOUBLE PRECISION, INTENT(IN) :: DELTA_T

  INTEGER :: q_num, id_q_num
  DOUBLE PRECISION :: max_omega, delta_omega

  max_omega = PI / DELTA_T
  delta_omega = max_omega / DBLE(N_FREQ)

  do q_num = 1, NSQ

    id_q_num = SQID(q_num)

    do freq = 1, N_FREQ

      omega = DBLE(freq-1) * delta_omega
      sqw_val = 0.5d0 * SKT_TAB(id_q_num, 1)

      do t = 2, NS-MIN_IN
        time_val = DBLE(t-1) * DELTA_T
        sqw_val = sqw_val + SKT_TAB(id_q_num, t) * cos(omega * time_val
      enddo

      sqw_val = 2.0d0 * sqw_val * DELTA_T ! Factor 2 for symmetry -inf to +inf
      SQW_TAB(freq) = sqw_val

    enddo

    ! Save SQW_TAB here !
    call save_curve (N_FREQ, SQW_TAB, , IDSKT)

  enddo

END SUBROUTINE

INTEGER FUNCTION SKT_SAVE (NSQ)

USE PARAMETERS
USE MENDELEIEV

INTEGER, INTENT(IN) :: NSQ

INTEGER :: NDT, tps, cid
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

if (NSQ .gt. 0) then  ! If wave vectors exist

  SQTAB(:)=0.0d0

  if (N_SETS .eq. 1 .and. SETS_T(1) .eq. -1) then
    NDT = NS-MIN_IN
  else
    NDT = N_SETS
  endif

  do t=1, NDT

    cid = (t-1)*SKNUM
    if (NDT .eq. NS-MIN_IN) then
      tps = t
    else
      tps = SETS_T(t)
    endif

    i=0
    do k=1, NQ_IN
      if (degeneracy(k) .gt. 0) then
        i=i+1
        SQTAB(i)= NSQT(k,tps)
      endif
    enddo
    call save_curve (NSQ, SQTAB, cid, IDSKT)

    i=0
    do k=1, NQ_IN
      if (degeneracy(k) .gt. 0) then
        i=i+1
        SQTAB(i)= (NSQT(k,tps)-1.0)*K_POINT(k)
      endif
    enddo
    call save_curve (NSQ, SQTAB, cid + 2, IDSKT)

    i=0
    do k=1, NQ_IN
      if (degeneracy(k) .gt. 0) then
        i=i+1
        SQTAB(i)= XSQT(k,tps)
      endif
    enddo
    call save_curve (NSQ, SQTAB, cid + 4, IDSKT)

    i=0
    do k=1, NQ_IN
      if (degeneracy(k) .gt. 0) then
        i=i+1
        SQTAB(i)= (XSQT(k,tps)-1.0)*K_POINT(k)
      endif
    enddo
    call save_curve (NSQ, SQTAB, cid + 6, IDSKT)

    SQTAB(:)=0.0d0
    Sij(:,:,:)=0.0d0
    l = 8
    do i=1, NSP
      do j=1, NSP
        m=0

        do k=1, NQ_IN
          Sij(k,i,j) = SQT(k,tps,i,j)
          if (degeneracy(k) .gt. 0) then
            m=m+1
            SQTAB(m)=Sij(k,i,j)
          endif
        enddo
        call save_curve (NSQ, SQTAB, cid + l, IDSKT)
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
          if (degeneracy(k) .gt. 0) then
            m=m+1
            SQTAB(m)= FZSij(k,i,j)
          endif
        enddo
        call save_curve (NSQ, SQTAB, cid + l, IDSKT)
        l=l+2
      enddo
    enddo
    if (NSP .eq. 2) then
      do i=1, 4
        k=0
        do j=1, NQ_IN
          if (degeneracy(j) .gt. 0) then
            k=k+1
            SQTAB(k)= BTij(j,i)
          endif
        enddo
        call save_curve (NSQ, SQTAB, cid + l, IDSKT)
        l=l+2
      enddo
    endif

  enddo

endif ! If wave vectors exist

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
