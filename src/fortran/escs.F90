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
!! @file escs.F90
!! @short Edge and Corner sharing tetrahedra analysis
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

LOGICAL FUNCTION EESCS ()

USE PARAMETERS

IMPLICIT NONE

LOGICAL :: DMTXOK

INTERFACE
  LOGICAL FUNCTION ALLOCEDCO (alloc)
    LOGICAL, INTENT(IN) :: alloc
  END FUNCTION
  LOGICAL FUNCTION DISTMTX(NAN, LAN, LOOKNGB, UPNGB, MOLVOL)
    INTEGER, INTENT(IN) :: NAN
    INTEGER, DIMENSION(NAN), INTENT(IN) :: LAN
    LOGICAL, INTENT(IN) :: LOOKNGB, UPNGB, MOLVOL
  END FUNCTION
END INTERFACE

if (.not. ALLOCEDCO (.true.)) then
  EESCS=.false.
  goto 001
endif

CALC_PRINGS=.false.
NOHP=.false.
! Modify DISTMX to evaluate ES/CS
DMTXOK = DISTMTX(NA, LOT, .false., .false., .false.)

if (.not. DMTXOK) then
  EESCS=.false.
  goto 001
endif

do i=1, NS
  do j=1, NA
    do o=1, NSP
      if (CONTJ(j,i).eq.4 .and. (LA_COUNT(j,o,i) .eq. 4)) then
        if(NS .gt. 1) TDSA(LOT(j),o,i)=TDSA(LOT(j),o,i)+1
        TDA(LOT(j),o) = TDA(LOT(j),o)+1
      endif
    enddo
  enddo
enddo

do j=1, NSP
  do o=1, NSP
    do i=1, NS
      MTABL(i)=EDGEA(j,o,i)
    enddo
    z=0.0d0
    call MOYENNE(MTABL, NS, z)
    call ECT_TYPE(z, MTABL, NS, ETYPEA(j,o))
    do i=1, NS
      MTABL(i)=CORNERA(j,o,i)
    enddo
    z=0.0d0
    call MOYENNE(MTABL, NS, z)
    call ECT_TYPE(z, MTABL, NS, CTYPEA(j,o))
    do i=1, NS
      MTABL(i)=DEFA(j,o,i)
    enddo
    z=0.0d0
    call MOYENNE(MTABL, NS, z)
    call ECT_TYPE(z, MTABL, NS, DETYPEA(j,o))
    do i=1, NS
      MTABL(i)=TDSA(j,o,i)
    enddo
    call MOYENNE(MTABL, NS, z)
    call ECT_TYPE(z, MTABL, NS, ETDA(j,o))
  enddo
enddo

001 continue

EESCS=.true.

END FUNCTION
