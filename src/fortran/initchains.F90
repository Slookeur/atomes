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
!! @file initchains.F90
!! @short Initialize chain statistics
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION initchains (VTLT, VAAA, VACA, VHOMO, V121, VTAILLC, VNUMA) BIND (C,NAME='initchains_')

!
! Initialization of the chain statistics
! The key variable is NUMA see in the following lines
!

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: VTLT, VAAA, VACA, VHOMO, V121, VTAILLC, VNUMA

INTERFACE
  INTEGER FUNCTION CHAINS()
  END FUNCTION
END INTERFACE

  if (VTLT .eq. 0) then
    TLT=NSP+1
  else
    TLT=VTLT
  endif
  NUMA=VNUMA
  NTLT=NBSPBS(TLT)
  TAILLC=VTAILLC

  AAAA=.false.
  ACAC=.false.
  NO_HOMO=.false.
  if (NSP.gt. 1) then
    if (VAAA == 1) AAAA=.true.
    if (VACA == 1) ACAC=.true.
    if (VHOMO == 1) NO_HOMO=.true.
  endif
  ISOLATED=.false.
  if (V121 == 1) ISOLATED=.true.
#ifdef DEBUG
  write (6, '("CHAINS:: AAAA= ",l1,", ACAC= ",l1,", NO_HOMO= ",l1,", ISOLATED= ",l1)') AAAA, ACAC, NO_HOMO, ISOLATED
#endif
  TBR=.false.
  ALC=.false.

  initchains = CHAINS()

END FUNCTION

INTEGER FUNCTION RECHAINS()

USE PARAMETERS

IMPLICIT NONE

DOUBLE PRECISION :: TAMP, ETAMP
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: CTAB

allocate(CTAB(TAILLC), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECHAINS"//CHAR(0), "Table: CTAB"//CHAR(0))
  RECHAINS=0
  goto 001
endif
if (allocated(RED)) deallocate(RED)
allocate(RED(NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECHAINS"//CHAR(0), "Table: RED"//CHAR(0))
  RECHAINS=0
  goto 001
endif
if (allocated(MOYRED)) deallocate(MOYRED)
allocate(MOYRED(TAILLC), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECHAINS"//CHAR(0), "Table: MOYRED"//CHAR(0))
  RECHAINS=0
  goto 001
endif
if (allocated(ECTYPE)) deallocate(ECTYPE)
allocate(ECTYPE(TAILLC), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECHAINS"//CHAR(0), "Table: ECTYPE"//CHAR(0))
  RECHAINS=0
  goto 001
endif

RED(:)=0.0d0

do l=1, TAILLC
  do i=1, NS
    RED(i)=dble(NRING(l,i))/NTLT
  enddo
  if (NS .gt. 1) then
    MOYRED(l)=0.0d0
    ECTYPE(l)=0.0d0
    call MOYENNE(RED, NS, MOYRED(l))
    call ECT_TYPE(MOYRED(l), RED, NS, ECTYPE(l))
  else
    MOYRED(l)=RED(1)
  endif
enddo

TAMP=0.0
do i=1, TAILLC
  CTAB(i)=MOYRED(i)
  TAMP = TAMP + CTAB(i)
enddo

if (TAMP .eq. 0.0) then
  RECHAINS=1
  goto 001
endif

l=0
if (TLT .ne. NSP+1) l = l + TLT

call save_curve (TAILLC, CTAB, l, IDCH)

if (allocated(TOTPSTEP)) deallocate(TOTPSTEP)
allocate(TOTPSTEP(NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECHAINS"//CHAR(0), "Table: TOTPSTEP"//CHAR(0))
  RECHAINS=0
  goto 001
endif

do i=1, NS
  TOTPSTEP(i)=0
  do j=1, TAILLC
    TOTPSTEP(i)=TOTPSTEP(i)+dble(NRING(j,i))
  enddo
enddo

TAMP=0.0d0
ETAMP=0.0d0
call MOYENNE(TOTPSTEP, NS, TAMP)
call ECT_TYPE(TAMP, TOTPSTEP, NS, ETAMP)
call save_chains_data (TAILLC, ECTYPE, TAMP, ETAMP)

RECHAINS=1

001 continue

if (allocated(TOTPSTEP)) deallocate(TOTPSTEP)
if (allocated(MOYRED)) deallocate(MOYRED)
if (allocated(RED)) deallocate(RED)
if (allocated(CTAB)) deallocate(CTAB)

END FUNCTION
