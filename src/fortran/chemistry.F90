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
!! @file chemistry.F90
!! @short Basic chemistry analysis
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION chemistry () BIND (C,NAME='chemistry_')

!
! Density, number density, concentrations, empirical formula
!

USE PARAMETERS

IMPLICIT NONE

INTEGER :: REF1, REF2, REF3
INTEGER, DIMENSION(:), ALLOCATABLE :: INDEX_FORM           ! Formula index
INTEGER, DIMENSION(:), ALLOCATABLE :: REFP

LOGICAL :: UNCLEAR, UNFRAC

!
! Determination of the empirical formula
! Détermination de la formule brute
!
if (allocated(INDEX_FORM)) deallocate(INDEX_FORM)
allocate(INDEX_FORM(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: chemistry"//CHAR(0), "Table: INDEX_FORM"//CHAR(0))
  chemistry = 0
  goto 001
endif

if (allocated(REFP)) deallocate(REFP)
allocate(REFP(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: chemistry"//CHAR(0), "Table: REFP"//CHAR(0))
  chemistry = 0
  goto 001
endif

do i=1, NSP
  REFP(i)= NBSPBS(i)
enddo

do i=1, NSP

  if (i .eq. 1)then

    REF1=NBSPBS(i)
    REF2=i

  else

    REF3=REF1
    REF1=min(NBSPBS(i),REF1)
    if (REF3 .ne. REF1) REF2=i

  endif

enddo

UNCLEAR=.true.
UNFRAC=.false.
l=1
do while (UNCLEAR)

  UNCLEAR=.false.
  do i=1, NSP
    if (i .ne. REF2) then
      if (.not.UNFRAC) then
        m=1
        do while (mod(m*REFP(i),REF1).ne.0 .and. m.lt.REF1)
          m=m+1
        enddo
        if (mod(m*REFP(i),REF1) .eq. 0) then
          INDEX_FORM(i)=m*REFP(i)/REF1
          UNCLEAR=.true.
        else
          INDEX_FORM(i)=NBSPBS(i)
          UNCLEAR=.true.
          UNFRAC=.true.
        endif
      else
        INDEX_FORM(i)=NBSPBS(i)
      endif
    endif
  enddo
  if (UNFRAC) then
    INDEX_FORM(REF2)=NBSPBS(REF2)
  else
    INDEX_FORM(REF2)=REFP(REF2)/NBSPBS(REF2)
  endif
  if (UNCLEAR) then
    UNCLEAR=.false.
    do i=1, NSP-1
      do j=i+1, NSP
        y=dble(INDEX_FORM(i))/dble(INDEX_FORM(j))
        z=dble(NBSPBS(i))/dble(NBSPBS(j))
        if (y .ne. z) UNCLEAR=.true.
      enddo
    enddo
    if (UNCLEAR) then
      REFP=REFP/l
      l=l+1
      REFP=REFP*l
    endif
  endif
enddo

! To send_chem_info_
call send_chem_info (INDEX_FORM)

chemistry = 1

001 continue

if (allocated(INDEX_FORM)) deallocate(INDEX_FORM)
if (allocated(REFP)) deallocate(REFP)

END FUNCTION
