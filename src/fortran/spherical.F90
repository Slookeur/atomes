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
!! @file spherical.F90
!! @short Spherical harmonics analysis
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

! Bond order parameter from spherical harmonics

INTEGER (KIND=c_int) FUNCTION sphericals (MAXL, SPC, GEO, IDC, COOSPH)

  USE PARAMETERS
#ifdef OPENMP
!$ USE OMP_LIB
#endif
  IMPLICIT NONE

  INTEGER (KIND=c_int), INTENT(IN) :: MAXL, SPC, GEO, IDC
  INTEGER (KIND=c_int), DIMENSION(NSP), INTENT(IN) :: COOSPH
  INTEGER :: NSPSH
  INTEGER, DIMENSION(:), ALLOCATABLE :: NEIGH
  LOGICAL :: SPHRUN
  DOUBLE PRECISION :: XC, YC, ZC
  DOUBLE PRECISION :: SR, ST, SP
  DOUBLE PRECISION, DIMENSION(0:MAXL,-MAXL:MAXL) :: HSP
! DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: THSP
  DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: ATHSP
  DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: SPTSHP
  DOUBLE PRECISION, DIMENSION(0:MAXL) :: SPHA
#ifdef OPENMP
  INTEGER :: NUMTH
  LOGICAL :: DOATOMS
#endif
  INTERFACE
    DOUBLE PRECISION FUNCTION PLEGENDRE (l, m, x)
      INTEGER, INTENT(IN) :: l, m
      DOUBLE PRECISION, INTENT(IN) :: x
    END FUNCTION
    DOUBLE PRECISION FUNCTION CALCDIJ (R12, AT1, AT2, STEP_1, STEP_2, SID)
      DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: R12
      INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
    END FUNCTION
  END INTERFACE

  if (allocated(NEIGH)) deallocate(NEIGH)
  allocate(NEIGH(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: sphericals"//CHAR(0), "Table: NEIGH"//CHAR(0))
    sphericals=0
    goto 001
  endif

!  if (allocated(THSP)) deallocate(THSP)
!  allocate(THSP(NBONDS,0:MAXL,-MAXL:MAXL), STAT=ERR)
!  if (ERR .ne. 0) then
!    call show_error ("Impossible to allocate memory"//CHAR(0), &
!                     "Function: sphericals"//CHAR(0), "Table: THSP"//CHAR(0))
!    sphericals=0
!    goto 001
!  endif
  if (allocated(ATHSP)) deallocate(ATHSP)
  allocate(ATHSP(0:MAXL,-MAXL:MAXL), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: sphericals"//CHAR(0), "Table: ATHSP"//CHAR(0))
    sphericals=0
    goto 001
  endif
  if (allocated(SPTSHP)) deallocate(SPTSHP)
  allocate(SPTSHP(0:MAXL,-MAXL:MAXL), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: sphericals"//CHAR(0), "Table: SPTSHP"//CHAR(0))
    sphericals=0
    goto 001
  endif

  ATHSP(:,:)=0.0d0
  SPTSHP(:,:)=0.0d0
  ANBONDS=0
  NSPSH=0
#ifdef OPENMP
  NUMTH = OMP_GET_MAX_THREADS ()
  DOATOMS=.false.
  if (NS.lt.NUMTH) then
    if (NUMTH .ge. 2*(NS-1)) then
      DOATOMS=.true.
    else
      NUMTH=NS
    endif
  endif

  if (ALL_ATOMS) DOATOMS=.true.

  if (DOATOMS) then
    if (NA.lt.NUMTH) NUMTH=NA
#ifdef DEBUG
    write (6, *) "OpenMP on atoms, NUMTH= ",NUMTH
#endif
    ! OpemMP on atoms
    do i=1, NS

      !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
      !$OMP& PRIVATE(SPHRUN, NEIGH, Dij, Rij, j, k, l, m, XC, YC, ZC, SR, ST, SP, HSP) &
      !$OMP& SHARED(NUMTH, i, NA, NSP, NCELLS, LOT, SPC, CONTJ, VOISJ, NSPSH, ATHSP, SPTSHP, ANBONDS, COOSPH, MAXL)
      !$OMP DO SCHEDULE(STATIC,NA/NUMTH)
      do j=1, NA

        if (LOT(j) .eq. SPC+1) then
          SPHRUN=.true.
          NEIGH(:)=0
          do k=1, CONTJ(j,i)
            NEIGH(LOT(VOISJ(k,j,i)))=NEIGH(LOT(VOISJ(k,j,i)))+1
          enddo

          !$OMP ATOMIC
          NSPSH=NSPSH+CONTJ(j,i)
          do k=1, NSP
            if (NEIGH(k) .ne. COOSPH(k)) then
              SPHRUN=.false.
              exit
            endif
          enddo

          if (SPHRUN) then
            !$OMP ATOMIC
            ANBONDS=ANBONDS+CONTJ(j,i)
          endif

          do k=1, CONTJ(j,i)
            if (NCELLS .gt. 1) then
              Dij = CALCDIJ (Rij, j, VOISJ(k,j,i), i, i, i)
            else
              Dij = CALCDIJ (Rij, j, VOISJ(k,j,i), i, i, 1)
            endif
            XC=Rij(1)
            YC=Rij(2)
            ZC=Rij(3)
!           NBONDS=NBONDS+1
            call CART2SPHER(XC, YC, ZC, SR, ST, SP)
            do l=0, MAXL
              do m=0, l
                HSP(l,m) = 0.0d0
                HSP(l,m) = PLEGENDRE (l, m, ST) * cos(SP * m)
                if (m .ne. 0) HSP(l,-m) = (-1)**m*HSP(l,m)
              enddo
              do m=-l, l
!                THSP(NBONDS,l,m)=HSP(l,m)
                !$OMP ATOMIC
                SPTSHP(l,m)=SPTSHP(l,m)+HSP(l,m)
                if (SPHRUN) then
                  !$OMP ATOMIC
                  ATHSP(l,m)=ATHSP(l,m)+HSP(l,m)
                endif
              enddo
            enddo
          enddo
        endif
      enddo
      !$OMP END DO NOWAIT
      !$OMP END PARALLEL
    enddo

  else
#ifdef DEBUG
    write (6, *) "OpenMP on MD steps, NUMTH= ",NUMTH
#endif
  ! OpemMP on MD steps
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(SPHRUN, NEIGH, Dij, Rij, i, j, k, l, m, XC, YC, ZC, SR, ST, SP, HSP) &
  !$OMP& SHARED(NUMTH, NS, NA, NSP, NCELLS, LOT, SPC, CONTJ, VOISJ, NSPSH, ATHSP, SPTSHP, ANBONDS, COOSPH, MAXL)
  !$OMP DO SCHEDULE(STATIC,NS/NUMTH)
#endif
    do i=1, NS
      do j=1, NA

        if (LOT(j) .eq. SPC+1) then
          SPHRUN=.true.
          NEIGH(:)=0
          do k=1, CONTJ(j,i)
            NEIGH(LOT(VOISJ(k,j,i)))=NEIGH(LOT(VOISJ(k,j,i)))+1
          enddo
#ifdef OPENMP
          !$OMP ATOMIC
#endif
          NSPSH=NSPSH+CONTJ(j,i)
          do k=1, NSP
            if (NEIGH(k) .ne. COOSPH(k)) then
              SPHRUN=.false.
              exit
            endif
          enddo

          if (SPHRUN) then
#ifdef OPENMP
            !$OMP ATOMIC
#endif
            ANBONDS=ANBONDS+CONTJ(j,i)
          endif

          do k=1, CONTJ(j,i)
            if (NCELLS .gt. 1) then
              Dij = CALCDIJ (Rij, j, VOISJ(k,j,i), i, i, i)
            else
              Dij = CALCDIJ (Rij, j, VOISJ(k,j,i), i, i, 1)
            endif
            XC=Rij(1)
            YC=Rij(2)
            ZC=Rij(3)
!           NBONDS=NBONDS+1
            call CART2SPHER(XC, YC, ZC, SR, ST, SP)
            do l=0, MAXL
              do m=0, l
                HSP(l,m) = 0.0d0
                HSP(l,m) = PLEGENDRE (l, m, ST) * cos(SP * m)
                if (m .ne. 0) HSP(l,-m) = (-1)**m*HSP(l,m)
              enddo
              do m=-l, l
!                THSP(NBONDS,l,m)=HSP(l,m)
#ifdef OPENMP
                !$OMP ATOMIC
#endif
                SPTSHP(l,m)=SPTSHP(l,m)+HSP(l,m)
                if (SPHRUN) then
#ifdef OPENMP
                  !$OMP ATOMIC
#endif
                  ATHSP(l,m)=ATHSP(l,m)+HSP(l,m)
                endif
              enddo
            enddo
          enddo
        endif
      enddo
    enddo
#ifdef OPENMP
    !$OMP END DO NOWAIT
    !$OMP END PARALLEL
  endif
#endif

  if (GEO .eq. 0) then
    SPHA(:)=0.0d0
    if (NSPSH .gt. 0) then
      do l=0, MAXL
        do m=-l, l
          SPHA(l)=SPHA(l)+(SPTSHP(l,m)/NSPSH)**2
        enddo
        SPHA(l)=sqrt(4*PI*SPHA(l)/(2*l+1))
      enddo
    endif
    call save_curve (MAXL+1, SPHA, IDC-1, IDSP)
  endif
  SPHA(:)=0.0d0
  if (ANBONDS .gt. 0) then
    do l=0, MAXL
      do m=-l, l
        SPHA(l)=SPHA(l)+(ATHSP(l,m)/ANBONDS)**2
      enddo
      SPHA(l)=sqrt(4*PI*SPHA(l)/(2*l+1))
    enddo
  endif
  call save_curve (MAXL+1, SPHA, IDC, IDSP)

  sphericals = 1

  001 continue

!  if (allocated(THSP)) deallocate(THSP)
  if (allocated(ATHSP)) deallocate(ATHSP)
  if (allocated(SPTSHP)) deallocate(SPTSHP)
  if (allocated(NEIGH)) deallocate(NEIGH)

END FUNCTION

SUBROUTINE CART2SPHER(XC, YC, ZC, RS, TS, PS)

DOUBLE PRECISION, INTENT(IN) :: XC, YC, ZC
DOUBLE PRECISION, INTENT(OUT) :: RS, TS, PS

RS=0.0d0
TS=0.0d0
PS=0.0d0

RS= sqrt(XC**2 + YC**2 + ZC**2)
TS= acos(ZC/RS)
PS= atan2(XC, YC)

END SUBROUTINE

! Adapted from Numercial Recipies !

DOUBLE PRECISION FUNCTION PLEGENDRE (l, m, x)

  DOUBLE PRECISION, PARAMETER :: PI=acos(-1.0)
  INTEGER, INTENT(IN) :: l, m
  DOUBLE PRECISION, INTENT(IN) :: x
  DOUBLE PRECISION :: y
  INTEGER :: i, ll
  DOUBLE PRECISION :: fact, oldfact, pll, pmm, pmmp1, omx2

  y=cos(x)
  pmm=1.0
  if (m > 0) then
    omx2=(1.0-y)*(1.0+y)
    fact=1.0
    i = 1
    do while (i.le.m)
      pmm = pmm * (omx2*fact/(fact+1.0))
      fact = fact + 2.0
      i = i+1
    enddo
  endif
  pmm=sqrt((2*m+1)*pmm/(4.0*PI))
  if (mod(m,2) .eq. 1) pmm=-pmm

  if (l .eq. m) then
    PLEGENDRE=pmm
  else
    pmmp1=y*sqrt(2.0*m+3.0)*pmm
    if (l .eq. (m+1)) then
      PLEGENDRE=pmmp1
    else
      oldfact=sqrt(2.0*m+3.0)
      ll=m+2
      do while (ll.le.l)
        fact=sqrt((4.0*ll*ll-1.0)/(ll*ll-m*m))
        pll=(y*pmmp1-pmm/oldfact)*fact
        oldfact=fact
        pmm=pmmp1
        pmmp1=pll
        ll=ll+1
      enddo
      PLEGENDRE=pll
    endif
  endif

END FUNCTION

DOUBLE PRECISION FUNCTION PLGNDR(l, m, x)

  DOUBLE PRECISION, PARAMETER :: PI=acos(-1.0)
  INTEGER :: j
  INTEGER, INTENT(IN) :: l, m
  DOUBLE PRECISION, INTENT(IN) :: x
  DOUBLE PRECISION :: prod

  INTERFACE
    DOUBLE PRECISION FUNCTION PLEGENDRE(l, m, x)
      INTEGER, INTENT(IN) :: l, m
      DOUBLE PRECISION, INTENT(IN) :: x
    END FUNCTION
  END INTERFACE

  prod=1.0d0
  j=l-m+1
  do while (j.le.l+m)
    prod = prod*j
    j=j+1
  enddo

  PLGNDR=sqrt(4.0*PI*prod/(2*l+1))*PLEGENDRE(l,m,x)

END FUNCTION
