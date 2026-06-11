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
!! @file angles.F90
!! @short Distribution of bond angles and dihedrals
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION bond_angles(nda) BIND (C,NAME='bond_angles_')

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: nda
INTEGER, DIMENSION(:,:,:,:), ALLOCATABLE :: ANGLEA
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SUM_ANGA
DOUBLE PRECISION :: ANG
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: ANGTAB
#ifdef OPENMP
INTEGER :: NUMTH
LOGICAL :: DOATOMS
#endif
INTERFACE
  DOUBLE PRECISION FUNCTION ANGIJK (ATG1, ATG2, ATG3, ASTEP)
    INTEGER, INTENT(IN) :: ATG1, ATG2, ATG3, ASTEP
  END FUNCTION
END INTERFACE

if (allocated(ANGLEA)) deallocate(ANGLEA)
allocate(ANGLEA(NSP,NSP,NSP,nda), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bond_angles"//CHAR(0), "Table: ANGLEA"//CHAR(0))
  bond_angles=0
  goto 001
endif

ANGLEA(:,:,:,:)=0

DELTA_ANG=180.0/dble(nda)

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
  do i=1, NS
    ! OpemMP on Atoms
    !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
    !$OMP& PRIVATE(j, k, l, m, n, ANG, ANG_I) &
    !$OMP& SHARED(NUMTH, NS, i, NA, NCELLS, LOT, CONTJ, VOISJ, ANGLEA, DELTA_ANG, nda)
    !$OMP DO SCHEDULE(STATIC,NA/NUMTH)
    do j=1, NA

      if (CONTJ(j,i) .gt. 1) then

        do k=1, CONTJ(j,i)-1
          do l=k+1, CONTJ(j,i)
            m=VOISJ(k,j,i)
            n=VOISJ(l,j,i)
            ANG = ANGIJK (m, j, n, i)

            ANG_I=AnINT (ANG/DELTA_ANG)
            if (ANG_I.le.0) ANG_I=1
            if (ANG_I.gt.nda) ANG_I=nda
            !$OMP ATOMIC
            ANGLEA(LOT(m),LOT(j),LOT(n),ANG_I)=ANGLEA(LOT(m),LOT(j),LOT(n),ANG_I)+1
            if (LOT(m) .ne. LOT(n)) then
              !$OMP ATOMIC
              ANGLEA(LOT(n),LOT(j),LOT(m),ANG_I)=ANGLEA(LOT(n),LOT(j),LOT(m),ANG_I)+1
            endif
          enddo
        enddo

      endif

    enddo
    !$OMP END DO NOWAIT
    !$OMP END PARALLEL
  enddo

else
  ! OpemMP on MD steps
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(i, j, k, l, m, n, ANG, ANG_I) &
  !$OMP& SHARED(NUMTH, NS, NA, NCELLS, LOT, CONTJ, VOISJ, ANGLEA, DELTA_ANG, nda)
  !$OMP DO SCHEDULE(STATIC,NS/NUMTH)
#endif
  do i=1, NS

    do j=1, NA

      if (CONTJ(j,i) .gt. 1) then

        do k=1, CONTJ(j,i)-1
          do l=k+1, CONTJ(j,i)
            m=VOISJ(k,j,i)
            n=VOISJ(l,j,i)
            ANG = ANGIJK (m, j, n, i)

            ANG_I=AnINT (ANG/DELTA_ANG)
            if (ANG_I.le.0) ANG_I=1
            if (ANG_I.gt.nda) ANG_I=nda
#ifdef OPENMP
            !$OMP ATOMIC
#endif
            ANGLEA(LOT(m),LOT(j),LOT(n),ANG_I)=ANGLEA(LOT(m),LOT(j),LOT(n),ANG_I)+1
            if (LOT(m) .ne. LOT(n)) then
#ifdef OPENMP
              !$OMP ATOMIC
#endif
              ANGLEA(LOT(n),LOT(j),LOT(m),ANG_I)=ANGLEA(LOT(n),LOT(j),LOT(m),ANG_I)+1
            endif
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

if (allocated(SUM_ANGA)) deallocate(SUM_ANGA)
allocate (SUM_ANGA(NSP,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bond_angles"//CHAR(0), "Table: SUM_ANGA"//CHAR(0))
  bond_angles=0
  goto 001
endif

do i=1, NSP
  do j=1, NSP
    do k=1, NSP
      SUM_ANGA(i,j,k) = 0
      do l=1, nda
        SUM_ANGA(i,j,k) = SUM_ANGA(i,j,k) + ANGLEA(i,j,k,l)
      enddo
    enddo
  enddo
enddo

if (allocated(ANGTAB)) deallocate(ANGTAB)
allocate(ANGTAB(nda), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bond_angles"//CHAR(0), "Table: ANGTAB"//CHAR(0))
  bond_angles=0
  goto 001
endif
ANGTAB(:)=0.0

m=0
do i=1, NSP
  do j=1, NSP
    do k=1, NSP
      if (SUM_ANGA(i,j,k) .ne. 0) then
        do l=1, nda
          ANGTAB(l)=100.0*dble(ANGLEA(i,j,k,l))/dble(SUM_ANGA(i,j,k))
        enddo
        call save_curve (nda, ANGTAB, m, IDAN)
      else
        call save_curve (0, ANGTAB, m, IDAN)
      endif
      m=m+1
    enddo
  enddo
enddo

bond_angles=1

001 continue

if (allocated(ANGLEA)) deallocate(ANGLEA)
if (allocated(SUM_ANGA)) deallocate(SUM_ANGA)
if (allocated(ANGTAB)) deallocate(ANGTAB)

END FUNCTION bond_angles

INTEGER (KIND=c_int) FUNCTION bond_diedrals(nda) BIND (C,NAME='bond_diedrals_')

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: nda
INTEGER, DIMENSION(:,:,:,:,:), ALLOCATABLE :: ANGLED
INTEGER, DIMENSION(:,:,:,:), ALLOCATABLE :: SUM_ANGD
DOUBLE PRECISION :: ANG
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: ANGTAB
#ifdef OPENMP
INTEGER :: NUMTH
LOGICAL :: DOATOMS
#endif
INTERFACE
  DOUBLE PRECISION FUNCTION DIEDRE (DG1, DG2, DG3, DG4, DSTEP)
    INTEGER, INTENT(IN) :: DG1, DG2, DG3, DG4, DSTEP
  END FUNCTION
END INTERFACE

if (allocated(ANGLED)) deallocate(ANGLED)
allocate(ANGLED(NSP,NSP,NSP,NSP,nda), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bond_angles"//CHAR(0), "Table: ANGLED"//CHAR(0))
  bond_diedrals=0
  goto 001
endif

ANGLED(:,:,:,:,:)=0

DELTA_ANG=180.0/dble(nda)

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
  do i=1, NS
    ! OpemMP on Atoms
    !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
    !$OMP& PRIVATE(j, k, l, m, n, o, p, ANG, ANG_I) &
    !$OMP& SHARED(NUMTH, NS, i, NA, NCELLS, LOT, CONTJ, VOISJ, ANGLED, DELTA_ANG, nda)
    !$OMP DO SCHEDULE(STATIC,NA/NUMTH)
    do j=1, NA
      do k=1, CONTJ(j,i)
        m=VOISJ(k,j,i)
        if (CONTJ(m,i) .ge. 2) then
          do l=1, CONTJ(m,i)
            n=VOISJ(l,m,i)
            if (n .ne. j) then
              if (CONTJ(n,i) .ge. 2) then
                do o=1, CONTJ(n,i)
                  p = VOISJ(o,n,i)
                  if (p.ne.j .and. p.ne.m) then
                    ANG=DIEDRE (j, m, n, p, i)
                    ANG_I=AnINT (ANG/DELTA_ANG)+1
                    if (ANG_I.le.0) ANG_I=1
                    if (ANG_I.gt.nda) ANG_I=nda
                    !$OMP ATOMIC
                    ANGLED(LOT(j),LOT(m),LOT(n),LOT(p),ANG_I)=ANGLED(LOT(j),LOT(m),LOT(n),LOT(p),ANG_I)+1
                    if (LOT(j).ne.LOT(m) .or. LOT(j).ne.LOT(n) .or. LOT(j).ne.LOT(p)) then
                     !$OMP ATOMIC
                      ANGLED(LOT(p),LOT(n),LOT(m),LOT(j),ANG_I)=ANGLED(LOT(p),LOT(n),LOT(m),LOT(j),ANG_I)+1
                    endif
                  endif
                enddo
              endif
            endif
          enddo
        endif
      enddo
    enddo
    !$OMP END DO NOWAIT
    !$OMP END PARALLEL
  enddo

else
  ! OpemMP on MD steps
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(i, j, k, l, m, n, o, p, ANG, ANG_I) &
  !$OMP& SHARED(NUMTH, NS, NA, NCELLS, LOT, CONTJ, VOISJ, ANGLED, DELTA_ANG, nda)
  !$OMP DO SCHEDULE(STATIC,NS/NUMTH)
#endif
  do i=1, NS
    do j=1, NA
      do k=1, CONTJ(j,i)
        m=VOISJ(k,j,i)
        if (CONTJ(m,i) .ge. 2) then
          do l=1, CONTJ(m,i)
            n=VOISJ(l,m,i)
            if (n .ne. j) then
              if (CONTJ(n,i) .ge. 2) then
                do o=1, CONTJ(n,i)
                  p = VOISJ(o,n,i)
                  if (p.ne.j .and. p.ne.m) then
                    ANG=DIEDRE (j, m, n, p, i)
                    ANG_I=AnINT (ANG/DELTA_ANG)+1
                    if (ANG_I.le.0) ANG_I=1
                    if (ANG_I.gt.nda) ANG_I=nda
#ifdef OPENMP
                    !$OMP ATOMIC
#endif
                    ANGLED(LOT(j),LOT(m),LOT(n),LOT(p),ANG_I)=ANGLED(LOT(j),LOT(m),LOT(n),LOT(p),ANG_I)+1
                    if (LOT(j).ne.LOT(m) .or. LOT(j).ne.LOT(n) .or. LOT(j).ne.LOT(p)) then
#ifdef OPENMP
                     !$OMP ATOMIC
#endif
                      ANGLED(LOT(p),LOT(n),LOT(m),LOT(j),ANG_I)=ANGLED(LOT(p),LOT(n),LOT(m),LOT(j),ANG_I)+1
                    endif
                  endif
                enddo
              endif
            endif
          enddo
        endif
      enddo
    enddo
  enddo
#ifdef OPENMP
  !$OMP END DO NOWAIT
  !$OMP END PARALLEL
endif
#endif
if (allocated(SUM_ANGD)) deallocate(SUM_ANGD)
allocate (SUM_ANGD(NSP,NSP,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bond_angles"//CHAR(0), "Table: SUM_ANGD"//CHAR(0))
  bond_diedrals=0
  goto 001
endif
SUM_ANGD(:,:,:,:) = 0
do i=1, NSP
  do j=1, NSP
    do k=1, NSP
      do l=1, NSP
        do m=1, nda
          SUM_ANGD(i,j,k,l) = SUM_ANGD(i,j,k,l) + ANGLED(i,j,k,l,m)
        enddo
      enddo
    enddo
  enddo
enddo

if (allocated(ANGTAB)) deallocate(ANGTAB)
allocate(ANGTAB(nda), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bond_angles"//CHAR(0), "Table: ANGTAB"//CHAR(0))
  bond_diedrals=0
  goto 001
endif
ANGTAB(:)=0.0

n=NSP*NSP*NSP
do i=1, NSP
  do j=1, NSP
    do k=1, NSP
      do l=1, NSP
        if (SUM_ANGD(i,j,k,l) .ne. 0) then
          o=1
          do m=1, nda
            ANGTAB(m)=100.0*dble(ANGLED(i,j,k,l,m))/dble(SUM_ANGD(i,j,k,l))
          enddo
          call save_curve (nda, ANGTAB, n, IDAN)
        else
          call save_curve (0, ANGTAB, n, IDAN)
        endif
        n=n+1
      enddo
    enddo
  enddo
enddo

bond_diedrals=1

001 continue

if (allocated(ANGLED)) deallocate(ANGLED)
if (allocated(SUM_ANGD)) deallocate(SUM_ANGD)
if (allocated(ANGTAB)) deallocate(ANGTAB)

END FUNCTION bond_diedrals
