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
!! @file pdb.F90
!! @short Unused: read atomic coordinates in PDB format - old version
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION read_pdb (pdb_f, lpdb) BIND (C,NAME='read_pdb_')

!
! Lecture of atom types and coordinates - pdb files
!

USE PARAMETERS
USE MENDELEIEV

IMPLICIT NONE

LOGICAL :: isok
INTEGER (KIND=c_int), INTENT(IN) :: lpdb
INTEGER, DIMENSION(:), ALLOCATABLE :: LINEA
LOGICAL :: with_cell = .false.
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: pdb_f
CHARACTER (LEN=lpdb) :: pdb_file
CHARACTER (LEN=4) :: ANAME
CHARACTER (LEN=6) :: RID
INTEGER :: ASI
CHARACTER (LEN=1) :: ALB
CHARACTER (LEN=3) :: ACC
CHARACTER (LEN=1) :: CIC
INTEGER :: RSN
CHARACTER (LEN=1) :: IC
DOUBLE PRECISION :: OCA
DOUBLE PRECISION THF
CHARACTER (LEN=2) :: SYM
CHARACTER (LEN=2) :: CHA
REAL, DIMENSION(6) :: CELL
CHARACTER (LEN=11) :: SPG
INTEGER :: ZVAL

do i=1, lpdb
  pdb_file(i:i) = pdb_f(i)
enddo
CELL(:) = 0.0

inquire (file=pdb_file, exist=isok)
if (isok) then

  open (unit=20, file=pdb_file, action='read', status='old')
  NS=1
  NA=0
  l = 0
  do while (.true.)
    read (20, '(A6)', end=999) RID
    l = l+1
    if (RID.eq.'HETATM' .or. RID.eq.'ATOM  ') NA=NA+1
    if (RID.eq. 'CRYST1') with_cell = .true.
  enddo
  999 continue
  if (NA .ne. 0) then
    rewind(20)
    if (allocated(LINEA)) deallocate(LINEA)
    allocate (LINEA(l), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_pdb"//CHAR(0), "Table: LINEA"//CHAR(0))
    endif
    l=0
    do while (.true.)
      read (20, '(A6)', end=888) RID
      l = l+1
      LINEA(l)=0
      if (RID.eq.'HETATM' .or. RID.eq.'ATOM  ') LINEA(l) = 1
      if (RID.eq.'CRYST1') LINEA(l) = 2
    enddo
    888 continue
    rewind(20)
    if (allocated(FULLPOS)) deallocate(FULLPOS)
    allocate (FULLPOS(NA,3,NS), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_pdb"//CHAR(0), "Table: FULLPOS"//CHAR(0))
    endif
    if (allocated(TAB_OF_TYPE)) deallocate(TAB_OF_TYPE)
    allocate (TAB_OF_TYPE(NA), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_pdb"//CHAR(0), "Table: TAB_OF_TYPE"//CHAR(0))
    endif
    do i=1, NS
      m = 0
      do j=1, l-1
        if (LINEA(j) .eq. 1) then
          m = m+1
          read (20, 001) RID, &
                         ASI, &
                         ANAME, &
                         ALB, &
                         ACC, &
                         CIC, &
                         RSN, &
                         IC, &
                         FULLPOS(m,1,i), &
                         FULLPOS(m,2,i), &
                         FULLPOS(m,3,i), &
                         OCA, &
                         THF, &
                         SYM, &
                         CHA
          ! write (6, '("m= ",i4," x= ",f15.10,", y= ",f15.10,", z= ",f15.10)') m, FULLPOS(m,1,i),FULLPOS(m,2,i),FULLPOS(m,3,i)
          TAB_OF_TYPE(m)=ATOMPDB(j, SYM)
        else if (LINEA(j) .eq. 2) then
          read (20, 002) RID, &
                         CELL(1), &
                         CELL(2), &
                         CELL(3), &
                         CELL(4), &
                         CELL(5), &
                         CELL(6), &
                         SPG, &
                         ZVAL
        else
          read (20, *)
        endif
      enddo
    enddo
    if (allocated(LINEA)) deallocate(LINEA)
    if (with_cell) then
      call cell_data_from_pdb (CELL(1), CELL(2), CELL(3), CELL(4), CELL(5), CELL(6))
    endif
    read_pdb=0
  else
    read_pdb=2
  endif
  close(20)
else
  read_pdb=1
endif

001 FORMAT (A6,I5,1X,A4,A1,A3,1X,A1,I4,A1,3X,F8.3,F8.3,F8.3,F6.2,F6.2,10X,A2,A2)
002 FORMAT (A6,F9.3,F9.3,F9.3,F7.2,F7.2,F7.2,1X,A11,I4)

!
! PDB format v3.30
!
! HETATM / ATOM keyword
!
!---------------------------------------------------------------------------
!Field |    Column    | FORTRAN |
!  No. |     range    | format  | Description
!---------------------------------------------------------------------------
!   1. |    1 -  6    |   A6    | "ATOM  " or "HETATM"
!   2. |    7 - 11    |   I5    | Atom serial number
!   -  |   12 - 12    |   1X    | Blank
!   3. |   13 - 16    |   A4    | Atom name
!   4. |   17 - 17    |   A1    | Alternative location code (if any)
!   5. |   18 - 20    |   A3    | Standard 3-letter amino acid code for residue
!   -  |   21 - 21    |   1X    | Blank
!   6. |   22 - 22    |   A1    | Chain identifier code
!   7. |   23 - 26    |   I4    | Residue sequence number
!   8. |   27 - 27    |   A1    | Insertion code (if any)
!   -  |   28 - 30    |   3X    | Blank
!   9. |   31 - 38    |  F8.3   | Atom's x-coordinate
!  10. |   39 - 46    |  F8.3   | Atom's y-coordinate
!  11. |   47 - 54    |  F8.3   | Atom's z-coordinate
!  12. |   55 - 60    |  F6.2   | Occupancy value for atom
!  13. |   61 - 66    |  F6.2   | B-value (thermal factor)
!   -  |   67 - 76    |  10X    | Blank
!  14. |   77 - 78    |   A2    | Element symbol
!  15. |   79 - 80    |   A2    | Charge
!---------------------------------------------------------------------------

! CRYST1 keyword
!
!---------------------------------------------------------------------------
!Field |    Column    | FORTRAN |
!  No. |     range    | format  | Description
!---------------------------------------------------------------------------
!   1. |    1 -  6    |   A6    | "CRYST1"
!   2. |    7 - 15    |  F9.3   | a
!   3. |   16 - 24    |  F9.3   | b
!   4. |   25 - 33    |  F9.3   | c
!   5. |   34 - 40    |  F7.2   | alpha
!   6. |   41 - 47    |  F7.2   | beta
!   7. |   48 - 54    |  F7.2   | gamma
!   -  |   55 - 55    |   1X    | Blank
!   8. |   56 - 66    |  A11    | Space group
!   9. |   67 - 70    |   I4    | z
!---------------------------------------------------------------------------

CONTAINS

CHARACTER (LEN=2) FUNCTION ATOMPDB(LIN, PDBSTR)

INTEGER :: PA, PB
CHARACTER (LEN=2), INTENT(IN) :: PDBSTR
INTEGER, INTENT(IN) :: LIN

ATOMPDB='  '
PB=0
do PA=1, 2
  if (PDBSTR(PA:PA) .ne. ' ') then
    PB= PB+1
    ATOMPDB(PB:PB)=PDBSTR(PA:PA)
  endif
enddo

if (PB .eq. 2) then
  if (ATOMPDB(2:2).eq.'A') ATOMPDB(2:2)='a'
  if (ATOMPDB(2:2).eq.'B') ATOMPDB(2:2)='b'
  if (ATOMPDB(2:2).eq.'C') ATOMPDB(2:2)='c'
  if (ATOMPDB(2:2).eq.'D') ATOMPDB(2:2)='d'
  if (ATOMPDB(2:2).eq.'E') ATOMPDB(2:2)='e'
  if (ATOMPDB(2:2).eq.'F') ATOMPDB(2:2)='f'
  if (ATOMPDB(2:2).eq.'G') ATOMPDB(2:2)='g'
  if (ATOMPDB(2:2).eq.'H') ATOMPDB(2:2)='h'
  if (ATOMPDB(2:2).eq.'I') ATOMPDB(2:2)='i'
  if (ATOMPDB(2:2).eq.'J') ATOMPDB(2:2)='j'
  if (ATOMPDB(2:2).eq.'K') ATOMPDB(2:2)='k'
  if (ATOMPDB(2:2).eq.'L') ATOMPDB(2:2)='l'
  if (ATOMPDB(2:2).eq.'M') ATOMPDB(2:2)='m'
  if (ATOMPDB(2:2).eq.'N') ATOMPDB(2:2)='n'
  if (ATOMPDB(2:2).eq.'O') ATOMPDB(2:2)='o'
  if (ATOMPDB(2:2).eq.'P') ATOMPDB(2:2)='p'
  if (ATOMPDB(2:2).eq.'Q') ATOMPDB(2:2)='q'
  if (ATOMPDB(2:2).eq.'R') ATOMPDB(2:2)='r'
  if (ATOMPDB(2:2).eq.'S') ATOMPDB(2:2)='s'
  if (ATOMPDB(2:2).eq.'T') ATOMPDB(2:2)='t'
  if (ATOMPDB(2:2).eq.'U') ATOMPDB(2:2)='u'
  if (ATOMPDB(2:2).eq.'V') ATOMPDB(2:2)='v'
  if (ATOMPDB(2:2).eq.'W') ATOMPDB(2:2)='w'
  if (ATOMPDB(2:2).eq.'X') ATOMPDB(2:2)='x'
  if (ATOMPDB(2:2).eq.'Y') ATOMPDB(2:2)='y'
  if (ATOMPDB(2:2).eq.'Z') ATOMPDB(2:2)='z'
endif

PB=0
do PA=1, MAXE
  if (ATSYM(PA) .eq. ATOMPDB) then
    PB=1
  endif
enddo
if (PB .eq. 0) then
  write (6, '("Warning reading PDB file: atom symbol not found line: ",i10)') LIN
endif

END FUNCTION

END FUNCTION
