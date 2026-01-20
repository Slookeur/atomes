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
!! @file allochem.F90
!! @short Memory allocation for chemistry buffers
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER FUNCTION ALLOCHEM ()

USE PARAMETERS

IMPLICIT NONE

GRNUM=16+5*NSP*NSP
SQNUM=8+4*NSP*NSP
SKNUM=8+4*NSP*NSP
GQNUM=GRNUM
BDNUM=NSP*NSP
ANNUM=NSP*NSP*NSP+NSP*NSP*NSP*NSP
if (NSP .eq. 2) then
  GRNUM=GRNUM+6
  SQNUM=SQNUM+8
  SKNUM=SKNUM+8
  GQNUM=GQNUM+6
endif
RINUM=20*(NSP+1)
CHNUM=NSP+1
SHNUM=NSP+1
MSNUM=0
if (NS .gt. 1) MSNUM=14*NSP+6

if (allocated(NBSPBS)) deallocate(NBSPBS)
allocate(NBSPBS(NSP+1), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: NBSPBS"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif
if (allocated(TL)) deallocate(TL)
allocate(TL(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: TL"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif
if (allocated(MASS)) deallocate(MASS)
allocate(MASS(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: MASS"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif
if (allocated(RVDW)) deallocate(RVDW)
allocate(RVDW(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: RVDW"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif
if (allocated(ATOMID)) deallocate(ATOMID)
allocate(ATOMID(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: ATOMID"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif
if (allocated(Gr_CUT)) deallocate(Gr_CUT)
allocate(Gr_CUT(NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: Gr_CUT"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif
if (allocated(Xi)) deallocate(Xi)
allocate(Xi(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: Xi"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif
if (allocated(NSCATTL)) deallocate(NSCATTL)
allocate(NSCATTL(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: NSCATTL"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif
if (allocated(XSCATTL)) deallocate(XSCATTL)
allocate(XSCATTL(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCHEM"//CHAR(0), "Table: XSCATTL"//CHAR(0))
  ALLOCHEM = 0
  goto 001
endif

ALLOCHEM = 1

001 continue

END FUNCTION
