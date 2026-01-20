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
!! @file molecules.F90
!! @short Fragment(s) and molecule(s) analysis
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

LOGICAL FUNCTION ADD_MOL(this_mol, MOL_ID, STEP_ID)

USE PARAMETERS

IMPLICIT NONE

TYPE (MOL), POINTER, INTENT(INOUT) :: this_mol
INTEGER, INTENT(IN) :: MOL_ID, STEP_ID
INTEGER :: AAA

TYPE (MOL), POINTER :: NEW

allocate(NEW, STAT=ERR)
if (ERR .ne. 0) then
  !call show_error ("Impossible to allocate memory"//CHAR(0), &
  !                 "Function: ADD_MOL"//CHAR(0), "Pointer: NEW"//CHAR(0))
  ADD_MOL = .false.
  goto 001
endif
NEW%MID = MOL_ID
NEW%STEP = STEP_ID
NEW%ATOMES = 0
allocate(NEW%BSP(NSP), STAT=ERR)
if (ERR .ne. 0) then
  !call show_error ("Impossible to allocate memory"//CHAR(0), &
  !                 "Function: ADD_MOL"//CHAR(0), "TABLE: NEW%BSP"//CHAR(0))
  ADD_MOL = .false.
  goto 001
endif
nullify(NEW%FIRST_AT)
do AAA=1, NSP
  NEW%BSP(AAA) = 0
enddo
nullify(NEW%NEXT)
NEW%PREV => this_mol
this_mol%NEXT => NEW
this_mol => NEW
ADD_MOL = .true.

001 continue

END FUNCTION

LOGICAL FUNCTION ADD_ATO(this_mol, ATOM_ID)

USE PARAMETERS

IMPLICIT NONE

TYPE (MOL), POINTER, INTENT(INOUT) :: this_mol
INTEGER, INTENT(IN) :: ATOM_ID

TYPE (AT), POINTER :: NEW

allocate(NEW, STAT=ERR)
if (ERR .ne. 0) then
  !call show_error ("Impossible to allocate memory"//CHAR(0), &
  !                 "Function: ADD_ATO"//CHAR(0), "POINTER: NEW"//CHAR(0))
  ADD_ATO = .false.
  goto 001
endif
NEW%IND = ATOM_ID
NEW%PREV => this_mol%ATOM
this_mol%ATOM%NEXT => NEW
this_mol%ATOM => NEW
ADD_ATO=.true.

001 continue

END FUNCTION

RECURSIVE SUBROUTINE SETMOL (this_mol, toglin, stmb, molcount, the_step, the_atom, the_id)

USE PARAMETERS

IMPLICIT NONE

TYPE (MOL), POINTER, INTENT(INOUT) :: this_mol
INTEGER, INTENT(INOUT) :: molcount, stmb
INTEGER, INTENT(IN) :: the_step, the_atom, the_id
INTEGER, DIMENSION(NA), INTENT(INOUT) :: toglin
INTEGER :: MC, MD
INTERFACE
  LOGICAL FUNCTION ADD_ATO(this_mol, ATOM_ID)
    USE PARAMETERS
    TYPE (MOL), POINTER, INTENT(INOUT) :: this_mol
    INTEGER, INTENT(IN) :: ATOM_ID
  END FUNCTION
END INTERFACE

molcount=molcount+1
toglin(the_atom) = the_id
this_mol%ATOMES = this_mol%ATOMES + 1
this_mol%BSP(LOT(the_atom)) = this_mol%BSP(LOT(the_atom)) + 1
stmb = stmb + 1
if (this_mol%ATOMES > 1) then
  if (.not. ADD_ATO (this_mol, the_atom)) then
    molcount=-1
    goto 001
  endif
else
  allocate(this_mol%FIRST_AT, STAT=ERR)
  if (ERR .ne. 0) then
    !call show_error ("Impossible to allocate memory"//CHAR(0), &
    !                 "Function: SETMOL"//CHAR(0), "POINTER: this_mol%FIRST_AT"//CHAR(0))
    molcount=-1
    goto 001
  endif
  !write (6, '("Adding first atom:: Mol= ",i4,", At= ",i4)') this_mol%MID, the_atom
  this_mol%FIRST_AT%IND = the_atom
  this_mol%ATOM => this_mol%FIRST_AT
endif
if  (stmb .eq. NA) goto 002
if (molcount.eq.10000) goto 001
if (molcount.eq.-1) goto 001
do MC=1, CONTJ(the_atom, the_step)
  MD = VOISJ(MC,the_atom,the_step)
  if (toglin(MD).eq.0 .and. molcount.lt.10000) then
    call SETMOL (this_mol, toglin, stmb, molcount, the_step, VOISJ(MC,the_atom,the_step), the_id)
  endif
  if (stmb .eq. NA) goto 002
  if (molcount.eq.10000) goto 001
  if (molcount.eq.-1) goto 001
enddo

002 continue

molcount=molcount-1

001 continue

END SUBROUTINE SETMOL

INTEGER (KIND=c_int) FUNCTION molecules (frag_and_mol, allbonds) BIND (C,NAME='molecules_')

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: frag_and_mol, allbonds
INTEGER :: TOTMOL, MOLPS, MAXMOL
INTEGER, DIMENSION(:), ALLOCATABLE :: ATMOL
INTEGER, DIMENSION(:), ALLOCATABLE :: ATVS, MTMBS
#ifdef OPENMP
INTEGER :: NUMTH
#endif
INTERFACE
  LOGICAL FUNCTION ADD_MOL(this_mol, MOL_ID, STEP_ID)
    USE PARAMETERS
    TYPE (MOL), POINTER, INTENT(INOUT) :: this_mol
    INTEGER, INTENT(IN) :: MOL_ID, STEP_ID
  END FUNCTION
  RECURSIVE SUBROUTINE SETMOL (this_mol, toglin, stmb, molcount, the_step, the_atom, the_id)
    USE PARAMETERS
    TYPE (MOL), POINTER, INTENT(INOUT) :: this_mol
    INTEGER, INTENT(INOUT) :: molcount, stmb
    INTEGER, INTENT(IN) :: the_step, the_atom, the_id
    INTEGER, DIMENSION(NA), INTENT(INOUT) :: toglin
  END SUBROUTINE
END INTERFACE

TYPE (AT), POINTER :: TMPAT
TYPE (MOL), ALLOCATABLE, TARGET :: THEMOL
TYPE (MOL), POINTER :: TMPMOL

if (allocated(FULLPOS)) deallocate(FULLPOS)

k = 0
do i=1, NS
 do j=1, NA
   if (CONTJ(j,i) .eq. 0) k = k + 1
 enddo
enddo
MAXMOL = k + allbonds/2;

if (allocated(THEMOL)) deallocate(THEMOL)
allocate(THEMOL, STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="THEMOL"
  ALC=.true.
  molecules = 0
  goto 001
endif
if (allocated(MTMBS)) deallocate(MTMBS)
allocate(MTMBS(NS), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="MTMBS"
  ALC=.true.
  molecules = 0
  goto 001
endif

if (frag_and_mol .eq. 1) then
  call allocate_mol_data ()
endif

molecules = 1
#ifdef OPENMP
NUMTH = OMP_GET_MAX_THREADS ()
if (NS.lt.NUMTH) NUMTH=NS
!$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
!$OMP& PRIVATE(i, j, k, l, m, n, o, ERR, TOGL, THEMOL, TMPMOL, &
!$OMP& TOTMOL, MOLCOUNTER, TMBS, ATMOL, TMPAT, ATVS) &
!$OMP& SHARED(NUMTH, frag_and_mol, NS, NA, NSP, LOT, MTMBS, CONTJ, VOISJ, ALC, ALC_TAB)
#endif
if (allocated(TOGL)) deallocate(TOGL)
allocate(TOGL(NA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="TOGL"
  ALC=.true.
  molecules = 0
#ifdef OPENMP
  goto 005
#else
  goto 001
#endif
endif
nullify(TMPAT)
#ifdef OPENMP
  !$OMP DO SCHEDULE(STATIC,NS/NUMTH)
#endif
do i=1, NS

#ifdef OPENMP
  if (molecules .eq.0) goto 004
#endif
  TOGL(:)=0
  THEMOL%MID = 1
  THEMOL%STEP = i
  THEMOL%ATOMES = 0
  nullify(THEMOL%FIRST_AT)
  allocate(THEMOL%BSP(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="THEMOL%BSP"
    ALC=.true.
    molecules = 0
#ifdef OPENMP
    goto 004
#else
    goto 001
#endif
  endif
  do j=1, NSP
    THEMOL%BSP(j) = 0
  enddo
  nullify(THEMOL%NEXT)
  nullify(THEMOL%PREV)
  TMBS=0
  TOTMOL = 0
  MTMBS(i)=0
  MOLCOUNTER=0
  TMPMOL => THEMOL
  n = 1
  002 continue
  do j=n, NA
    if (MOLCOUNTER.eq.0 .and. TOGL(j).eq.0) then
      TOTMOL = TOTMOL + 1
      if (TOTMOL > 1) then
        if (.not.ADD_MOL(TMPMOL, MTMBS(i)+1, i)) then
          ALC_TAB="ADD_MOL"
          ALC=.true.
          molecules = 0
#ifdef OPENMP
          goto 004
#else
          goto 001
#endif
        endif
      endif
      MTMBS(i) = MTMBS(i) + 1
      call SETMOL (TMPMOL, TOGL, TMBS, MOLCOUNTER, i, j, TOTMOL)
      if (MOLCOUNTER .lt. 0) then
        ALC_TAB="SETMOL"
        ALC=.true.
#ifdef OPENMP
          goto 004
#else
          goto 001
#endif
      endif
      if (TMBS.eq.NA) goto 003
      ! The 10000 iterations break required because of F90 limitations
      if (MOLCOUNTER.eq.10000) goto 002
      MOLCOUNTER = 0
      n = j
      goto 002
    else if (MOLCOUNTER.gt.0 .and.TOGL(j).eq.TOTMOL) then
      do l=1, CONTJ(j,i)
        m = VOISJ(l,j,i)
        if (TOGL(m).eq.0) then
          MOLCOUNTER = 0
          call SETMOL (TMPMOL, TOGL, TMBS, MOLCOUNTER, i, VOISJ(l,j,i), TOTMOL)
          if (MOLCOUNTER .lt. 0) then
            ALC_TAB="SETMOL"
            ALC=.true.
#ifdef OPENMP
              goto 004
#else
              goto 001
#endif
          endif
          if (TMBS.eq.NA) goto 003
          if (MOLCOUNTER.eq.10000) goto 002
          MOLCOUNTER = 1
        endif
      enddo
    endif
  enddo

  if (TMBS .lt. NA) then
    MOLCOUNTER = 0
    goto 002
  endif

  003 continue

  if (frag_and_mol .eq. 1) then

    TMPMOL => THEMOL
    call allocate_mol_for_step (i, MTMBS(i))
    do j=1, MTMBS(i)
      if (allocated(ATMOL)) deallocate(ATMOL)
      allocate(ATMOL(TMPMOL%ATOMES), STAT=ERR)
      if (ERR .ne. 0) then
        ALC_TAB="ATMOL"
        ALC=.true.
        molecules = 0
#ifdef OPENMP
        goto 004
#else
        goto 001
#endif
      endif
      if (associated(TMPAT)) deallocate (TMPAT)
      allocate(TMPAT, STAT=ERR)
      if (ERR .ne. 0) then
        ALC_TAB="TMPAT"
        ALC=.true.
        molecules = 0
#ifdef OPENMP
        goto 004
#else
        goto 001
#endif
      endif
      TMPAT = TMPMOL%FIRST_AT
      do k=1, TMPMOL%ATOMES
        ATMOL(k) = TMPAT%IND
        if (k < TMPMOL%ATOMES) then
          TMPAT => TMPAT%NEXT
          deallocate (TMPAT%PREV)
        else
          deallocate (TMPAT)
        endif
      enddo

      call send_mol_details (TMPMOL%STEP, TMPMOL%MID, TMPMOL%ATOMES, NSP, TMPMOL%BSP, ATMOL)
      if (TMPMOL%ATOMES .gt. 1) then
        do l=1, TMPMOL%ATOMES
          m = ATMOL(l)
          n = CONTJ(m,TMPMOL%STEP)
          if (allocated(ATVS)) deallocate(ATVS)
          allocate(ATVS(n), STAT=ERR)
          if (ERR .ne. 0) then
            ALC_TAB="ATVS"
            ALC=.true.
            molecules = 0
#ifdef OPENMP
            goto 004
#else
            goto 001
#endif
          endif
          do o=1, n
            ATVS(o)= VOISJ(o,m,TMPMOL%STEP)
          enddo
          call send_mol_neighbors (TMPMOL%STEP, TMPMOL%MID, m, n, ATVS)
        enddo
      endif
      if (allocated(ATVS)) deallocate(ATVS)
      if (allocated(ATMOL)) deallocate(ATMOL)
      if (allocated(TMPMOL%BSP)) deallocate (TMPMOL%BSP)

      if (j .lt. MTMBS(i)) TMPMOL => TMPMOL%NEXT

    enddo
    call setup_molecules (i)
  endif
  call setup_fragments (i, TOGL)
  do while (TMPMOL%MID .gt. 1)
    TMPMOL => TMPMOL%PREV
    deallocate (TMPMOL%NEXT)
  enddo
#ifdef OPENMP
  004 continue
#endif
enddo
#ifdef OPENMP
!$OMP END DO NOWAIT
005 continue
if (allocated(TOGL)) deallocate (TOGL)
if (allocated(THEMOL)) deallocate(THEMOL)
if (allocated(ATMOL)) deallocate(ATMOL)
if (allocated(ATVS)) deallocate(ATVS)
!$OMP END PARALLEL
#else
if (allocated(TOGL)) deallocate (TOGL)
if (allocated(THEMOL)) deallocate(THEMOL)
if (allocated(ATVS)) deallocate(ATVS)
if (allocated(ATMOL)) deallocate(ATMOL)
#endif

MOLPS = 0
j = 0
do i=1, NS
  MOLPS = MOLPS + MTMBS(i)
 j = max(MTMBS(i), j)
enddo
call send_coord_opengl (2, 1, 0, 0, j, 1)
call init_menu_fragmol (2)

if (frag_and_mol .eq. 1) then
  call setup_menu_molecules ()
endif

molecules=1

001 continue

if (ALC) then
  call show_error ("Impossible to allocate memory !"//CHAR(0), &
                   "Function: molecules"//CHAR(0), CHAR(9)//"Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif

if (allocated(MTMBS)) deallocate(MTMBS)

END FUNCTION MOLECULES
