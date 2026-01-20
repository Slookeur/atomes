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
!! @file allocmsd.F90
!! @short Memory allocation for MSD analysis
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

LOGICAL FUNCTION ALLOCMSD()

USE PARAMETERS

if (allocated(D2i)) deallocate(D2i)
allocate(D2i(NSP,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCMSD"//CHAR(0), "Table: D2i"//CHAR(0))
  ALLOCMSD = .false.
  goto 001
endif

if (allocated(D2iNAC)) deallocate(D2iNAC)
allocate(D2iNAC(NSP,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCMSD"//CHAR(0), "Table: D2iNAC"//CHAR(0))
  ALLOCMSD = .false.
  goto 001
endif

if (allocated(D2dir)) deallocate(D2dir)
allocate(D2dir(NSP,6,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCMSD"//CHAR(0), "Table: D2dir"//CHAR(0))
  ALLOCMSD = .false.
  goto 001
endif

if (allocated(D2dirNAC)) deallocate(D2dirNAC)
allocate(D2dirNAC(NSP,6,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCMSD"//CHAR(0), "Table: D2dir"//CHAR(0))
  ALLOCMSD = .false.
  goto 001
endif

if (allocated(Dcte)) deallocate(Dcte)
allocate(Dcte(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCMSD"//CHAR(0), "Table: Dcte"//CHAR(0))
  ALLOCMSD = .false.
  goto 001
endif

if (allocated(COR)) deallocate(COR)
allocate(COR(3,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCMSD"//CHAR(0), "Table: COR"//CHAR(0))
  ALLOCMSD = .false.
  goto 001
endif

if (allocated(DRIFT)) deallocate(DRIFT)
allocate(DRIFT(3,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCMSD"//CHAR(0), "Table: DRIFT"//CHAR(0))
  ALLOCMSD = .false.
  goto 001
endif

D2i(:,:)=0.0d0
D2iNAC(:,:)=0.0d0
D2dir(:,:,:) =0.0d0
D2dirNAC(:,:,:) =0.0d0
Dcte(:)=0.0d0
COR(:,:)=0.0d0
DRIFT(:,:)=0.0d0

ALLOCMSD=.true.

001 continue

END FUNCTION

SUBROUTINE DEALLOCMSD

!
! Memory allocation for bond properties
!

USE PARAMETERS

if (allocated(D2i)) deallocate(D2i)
if (allocated(D2iNAC)) deallocate(D2iNAC)
if (allocated(D2dir)) deallocate(D2dir)
if (allocated(D2dirNAC)) deallocate(D2dirNAC)
if (allocated(Dcte)) deallocate(Dcte)
if (allocated(COR)) deallocate(COR)
if (allocated(DRIFT)) deallocate(DRIFT)

END SUBROUTINE
