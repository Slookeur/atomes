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
!! @file initrings.F90
!! @short Initialize ring statistics
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

LOGICAL FUNCTION ALLOCRINGS()

!
! Memory allocation for ring statistics
!

USE PARAMETERS

IMPLICIT NONE

if(allocated(NRING)) deallocate(NRING)
allocate(NRING(TAILLR,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCRINGS"//CHAR(0), "Table: NRING"//CHAR(0))
  ALLOCRINGS=.false.
  goto 001
endif
if (allocated(PNA)) deallocate(PNA)
allocate(PNA(TAILLR,TAILLR,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCRINGS"//CHAR(0), "Table: PNA"//CHAR(0))
  ALLOCRINGS=.false.
  goto 001
endif
if (allocated(MAXPNA)) deallocate(MAXPNA)
allocate(MAXPNA(TAILLR,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCRINGS"//CHAR(0), "Table: MAXPNA"//CHAR(0))
  ALLOCRINGS=.false.
  goto 001
endif
if (allocated(MINPNA)) deallocate(MINPNA)
allocate(MINPNA(TAILLR,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCRINGS"//CHAR(0), "Table: MINPNA"//CHAR(0))
  ALLOCRINGS=.false.
  goto 001
endif

!if (FACTATRING) then
!  if (allocated(ATRING)) deallocate(ATRING)
!  allocate(ATRING(TAILLD,NTLT,NS), STAT=ERR)
!  if (ERR .ne. 0) then
!
!  endif
!  ATRING(:,:,:)=0
!endif

if (DOAMPAT) then
  if (allocated(AMPAT)) deallocate(AMPAT)
  allocate(AMPAT(NTLT,NS), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCRINGS"//CHAR(0), "Table: AMPAT"//CHAR(0))
    ALLOCRINGS=.false.
    goto 001
  endif
  AMPAT(:,:)=0
endif

!if (FACTATPNA) then
! if (allocated(ATPNA)) deallocate(ATPNA)
!  allocate(ATPNA(TAILLD,NTLT,NS), STAT=ERR)
!  if (ERR .ne. 0) then
!    call show_error ("Impossible to allocate memory"//CHAR(0), &
!                     "Function: ALLOCRINGS"//CHAR(0), "Table: ATPNA"//CHAR(0))
!    ALLOCRINGS=.false.
!    goto 001
!  endif
!  ATPNA(:,:,:)=0
!endif

! End of allocation / Initialisation

MAXPNA(:,:)=0
MINPNA(:,:)=0
PNA(:,:,:)=0
NRING(:,:)=0

ALLOCRINGS=.true.

001 continue

END FUNCTION

INTEGER (KIND=c_int) FUNCTION initrings (VRINGS, VTAILLD, VTLT, VNUMA, VABAB, VHOMO) BIND (C,NAME='initrings_')

!
! Initialization of the ring statistics
! The key variable is NUMA see in the following lines
!

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: VRINGS, VTAILLD, VTLT, VNUMA, VABAB, VHOMO
INTERFACE
  LOGICAL FUNCTION ALLOCRINGS()
  END FUNCTION
  INTEGER FUNCTION KING_RINGS()
  END FUNCTION
  INTEGER FUNCTION GUTTMAN_RINGS()
  END FUNCTION
  INTEGER FUNCTION PRIMITIVE_RINGS()
  END FUNCTION
END INTERFACE

if (VTLT .eq. 0) then
  TLT=NSP+1
else
  TLT=VTLT
endif
NUMA=VNUMA
NTLT=NBSPBS(TLT)
TAILLR=VTAILLD
TAILLD=VTAILLD+1

ABAB=.false.
NO_HOMO=.false.
if (NSP .gt. 1) then
  if (VABAB .eq. 1) ABAB=.true.
  if (VHOMO .eq. 1) NO_HOMO=.true.
endif

#ifdef DEBUG
  write (6, '("RINGS:: ABAB= ",l1,", NO_HOMO= ",l1,", NUMA= ",i5,", TAILLR= ",i2)') ABAB, NO_HOMO, NUMA, TAILLR
#endif

TBR=.false.
ALC=.false.
ALLRINGS=.false.
CALC_STRINGS=.false.

! In the following lines the NUMA variable
! gives the 'average' number of rings per size and per step
! this variable depends on the type of rings, of the system studied as well
! as of the search depth.
! NUMA is used as a reference to allocate almost all tabs during rings
! search and analysis. NUMA is a key variable - tweak with caution !

if (VRINGS.eq.1 .or. VRINGS.eq.2) then
  DOAMPAT=.true.
else
  DOAMPAT=.false.
endif

if (.not.ALLOCRINGS()) then
  initrings=0
  goto 001
endif

if (VRINGS .eq. 0) ALLRINGS=.true.

if (VRINGS .le. 1) then
  initrings=KING_RINGS()
endif

if (VRINGS .eq. 2) then
  initrings=GUTTMAN_RINGS()
endif

if (VRINGS >  2) then
  CALC_PRINGS=.true.
  if (.not.PBC) then
    NNA=NA
    NNP=0
  else
    NNP=NA*(NBX**3 - 1)/2
  endif
  if (VRINGS .eq. 4) CALC_STRINGS=.true.
  initrings=PRIMITIVE_RINGS()
endif

001 continue

END FUNCTION
