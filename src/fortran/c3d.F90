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
!! @file c3d.F90
!! @short Write Chem3D atomic coordinates
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION read_c3d (c3d_f, lc3d) BIND (C,NAME='read_c3d_')

!
! Lecture of atom types and coordinates - C3D files
!

USE PARAMETERS

IMPLICIT NONE

LOGICAL :: isok
INTEGER (KIND=c_int), INTENT(IN) :: lc3d
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: c3d_f
CHARACTER (LEN=lc3d) :: c3d_file
INTERFACE
INTEGER FUNCTION TEST_LENGTH(UNITFILE, NATL)
  INTEGER, INTENT(IN) :: UNITFILE, NATL
END FUNCTION
END INTERFACE

do i=1, lc3d
  c3d_file(i:i) = c3d_f(i)
enddo

read_c3d=1

inquire (file=c3d_file, exist=isok)
if (isok) then

  open (unit=20, file=c3d_file, action='read', status='old', err=001)
  NS=0
  read (20, *, err=002) NA
  NS = TEST_LENGTH(20, NA+1)
  if (NS .eq. 1) then
    rewind(20)
    if (allocated(FULLPOS)) deallocate(FULLPOS)
    allocate (FULLPOS(NA,3,NS), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_c3d"//CHAR(0), "Table: FULLPOS"//CHAR(0))
    endif
    if (allocated(TAB_OF_TYPE)) deallocate(TAB_OF_TYPE)
    allocate (TAB_OF_TYPE(NA), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_c3d"//CHAR(0), "Table: TAB_OF_TYPE"//CHAR(0))
    endif
    do i=1, NS
      read (20, *, err=002)
      do j=1, NA
        read (20, *, err=002) TAB_OF_TYPE(j), k, FULLPOS(j,1,i), FULLPOS(j,2,i), FULLPOS(j,3,i)
      enddo
    enddo
    read_c3d=0
  else
    read_c3d=2
  endif

  002 continue

  close (20)

  001 continue

endif

END FUNCTION

INTEGER (KIND=c_int) FUNCTION write_c3d (c3d_f, lc3d, fc3d, tc3d) BIND (C,NAME='write_c3d_')

!
! output of atom types and coordinates - Chem3D files
!

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: lc3d, fc3d, tc3d
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: c3d_f
DOUBLE PRECISION, DIMENSION(3) :: savep
CHARACTER (LEN=lc3d) :: c3d_file
INTERFACE
INTEGER FUNCTION TEST_LENGTH(UNITFILE, NATL)
  INTEGER, INTENT(IN) :: UNITFILE, NATL
END FUNCTION
END INTERFACE

do i=1, lc3d
  c3d_file(i:i) = c3d_f(i)
enddo

write_c3d=1
open (unit=20, file=c3d_file, action='write', status='unknown', err=001)

do i=1, NS
  write (20, '(i10)') NA
  write (20, *)
  do j=1, NA
    if (fc3d .eq. 1) then
      if (NCELLS .gt. 1) then
        NBOX => THE_BOX(i)
      else
        NBOX => THE_BOX(1)
      endif
      savep = MATMUL(FULLPOS(j,:,i),NBOX%carttofrac)
    else
      savep = FULLPOS(j,:,i)
      if (tc3d .eq. 1) then
        savep(:) = savep(:)/ANGTOBOHR
      endif
    endif
    write (20, '(a2,3x,i2,3(3x,f15.10))', err=002) TL(LOT(j)), 0, savep
  enddo
enddo
write_c3d=0

002 continue

close (20)

001 continue

END FUNCTION
