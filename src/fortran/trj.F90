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
!! @file trj.F90
!! @short Unused: read atomic coordinates in CPMD trajectory format - old version
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>


INTEGER (KIND=c_int) FUNCTION read_trj (trj_f, ltrj, natrj, nbstrj, is_npt) BIND (C,NAME='read_trj_')

!
! Lecture of atom types and coordinates - CPMD trajectory
!

USE PARAMETERS

IMPLICIT NONE

LOGICAL :: isok
INTEGER (KIND=c_int), INTENT(IN) :: ltrj, natrj, is_npt
INTEGER (KIND=c_int), DIMENSION(NSP), INTENT(IN) :: nbstrj
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: trj_f
CHARACTER (LEN=ltrj) :: trj_file
INTERFACE
INTEGER FUNCTION TEST_LENGTH(UNITFILE, NATL)
  INTEGER, INTENT(IN) :: UNITFILE, NATL
END FUNCTION
END INTERFACE

do i=1, ltrj
  trj_file(i:i) = trj_f(i)
enddo

read_trj = 1
inquire (file=trj_file, exist=isok)
if (isok) then

  open (unit=20, file=trj_file, action='read', status='old', err=001)
  NS=0
  NA=natrj
  NS=TEST_LENGTH(20, NA)
  if (NS .ge. 1) then
    rewind(20)
    if (allocated(FULLPOS)) deallocate(FULLPOS)
    allocate (FULLPOS(NA,3,NS), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_trj"//CHAR(0), "Table: FULLPOS"//CHAR(0))
      read_trj=3
      goto 001
    endif
    if (allocated(TAB_OF_TYPE)) deallocate(TAB_OF_TYPE)
    allocate (TAB_OF_TYPE(NA), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_trj"//CHAR(0), "Table: TAB_OF_TYPE"//CHAR(0))
      read_trj=3
      goto 001
    endif
    do i=1, NS
      l=1
      do j=1, NSP
        do k=1, nbstrj(j)
           read (20, *, err=002) z, FULLPOS(l,1,i), FULLPOS(l,2,i), FULLPOS(l,3,i)
           ! CPMD trajectory is in atomic units
           do m=1, 3
             FULLPOS(l,m,i)=ANGTOBOHR*FULLPOS(l,m,i)
           enddo
           TAB_OF_TYPE(l)= LABEL(j)
           l=l+1
         enddo
      enddo
    enddo
    if (is_npt .eq. 1) call send_steps (NS)
    read_trj=0
  else
    read_trj=2
  endif

  002 continue

  close (20)

  001 continue

endif

END FUNCTION
