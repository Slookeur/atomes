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
!! @file vas.F90
!! @short Unused: read atomic coordinates in VASP trajectory format - old version
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION read_vas(vas_f, lvas, navas, nbsvas, is_npt) BIND (C,NAME='read_vas_')

!
! Lecture of atom types and coordinates - VASP trajectory
!

USE PARAMETERS

IMPLICIT NONE

LOGICAL :: isok
INTEGER :: NVAS
INTEGER (KIND=c_int), INTENT(IN) :: lvas, navas, is_npt
INTEGER (KIND=c_int), DIMENSION(NSP), INTENT(IN) :: nbsvas
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: vas_f
CHARACTER (LEN=lvas) :: vas_file

do i=1, lvas
  vas_file(i:i) = vas_f(i)
enddo

read_vas=1

inquire (file=vas_file, exist=isok)
if (isok) then

  open (unit=20, file=vas_file, action='read', status='old', err=001)
  NS=0
  NA=navas
  NVAS=TEST_VAS(20, NA)
  if (NVAS.ge.1) then
    rewind(20)
    if (allocated(FULLPOS)) deallocate(FULLPOS)
    allocate (FULLPOS(NA,3,NS), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_vas"//CHAR(0), "Table: FULLPOS"//CHAR(0))
    endif
    if (allocated(TAB_OF_TYPE)) deallocate(TAB_OF_TYPE)
    allocate (TAB_OF_TYPE(NA), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_vas"//CHAR(0), "Table: TAB_OF_TYPE"//CHAR(0))
    endif
    do i=1, NVAS
      read (20, *, err=002)
    enddo
    do i=1, NS
     ! Read the 'Konfig' line
      read (20, *, err=002)
      l=1
      do j=1, NSP
        do k=1, nbsvas(j)
           read (20, *, err=002) FULLPOS(l,1,i), FULLPOS(l,2,i), FULLPOS(l,3,i)
           TAB_OF_TYPE(l)= LABEL(j)
           l=l+1
         enddo
      enddo
    enddo
    if (is_npt .eq. 1) call send_steps (NS)
    read_vas=0
  else
    read_vas=2
  endif

  002 continue

  close (20)

  001 continue

endif

CONTAINS

INTEGER FUNCTION TEST_VAS(UNITFILE, NATL)

USE PARAMETERS

IMPLICIT NONE
INTEGER, INTENT(IN) :: UNITFILE, NATL
INTEGER :: LENF
INTEGER :: EOF

rewind(UNITFILE)
EOF=0
LENF=0
do while (EOF .eq. 0)
  read (UNITFILE, *, iostat=EOF)
  LENF=LENF+1
enddo
LENF=LENF-1

 if (mod(LENF-7,NATL+1) .eq. 0) then
  NS=(LENF-7)/(NATL+1)
  TEST_VAS=7
else if (mod(LENF-6,NATL+1) .eq. 0) then
  NS=(LENF-6)/(NATL+1)
  TEST_VAS=6
else if (mod(LENF-5,NATL+1) .eq. 0) then
  NS=(LENF-5)/(NATL+1)
  TEST_VAS=5
else
  TEST_VAS=0
endif

END FUNCTION

END FUNCTION

