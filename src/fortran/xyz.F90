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
! Copyright (C) 2022-2025 by CNRS and University of Strasbourg
!
!>
!! @file xyz -> F90
!! @short Write XYZ atomic coordinates
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION read_xyz (xyz_f, lxyz, is_npt) BIND (C,NAME='read_xyz_')

!INTEGER FUNCTION read_xyz (xyz_file, lxyz)

!
! Lecture of atom types and coordinates - XYZ files
!
USE PARAMETERS

IMPLICIT NONE

LOGICAL :: isok
INTEGER (KIND=c_int), INTENT(IN) :: lxyz, is_npt
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: xyz_f
CHARACTER (LEN=lxyz) :: xyz_file
INTERFACE
INTEGER FUNCTION TEST_LENGTH(UNITFILE, NATL)
  INTEGER, INTENT(IN) :: UNITFILE, NATL
END FUNCTION
END INTERFACE

do i=1, lxyz
  xyz_file(i:i) = xyz_f(i)
enddo

read_xyz=1

inquire (file=xyz_file, exist=isok)
if (isok) then

  open (unit=20, file=xyz_file, action='read', status='old', err=001)
  NS=0
  read (20, *, err=002) NA
  NS = TEST_LENGTH(20, NA+2)
  if (NS .gt. 0) then

    rewind(20)
    if (allocated(FULLPOS)) deallocate(FULLPOS)
    allocate (FULLPOS(NA,3,NS), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_xyz"//CHAR(0), "Table: FULLPOS"//CHAR(0))
    endif
    if (allocated(TAB_OF_TYPE)) deallocate(TAB_OF_TYPE)
    allocate (TAB_OF_TYPE(NA), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: read_xyz"//CHAR(0), "Table: TAB_OF_TYPE"//CHAR(0))
    endif

    do i=1, NS
      read (20, *, err=002)
      read (20, *, err=002)
      do j=1, NA
        read (20, *, err=002) TAB_OF_TYPE(j), FULLPOS(j,1,i), FULLPOS(j,2,i), FULLPOS(j,3,i)
      enddo
    enddo
    if (is_npt .eq. 1) call send_steps (NS)
    read_xyz=0
  else
    read_xyz=2
  endif

  002 continue

  close (20)

  001 continue

endif

END FUNCTION

INTEGER FUNCTION TEST_LENGTH (UNITFILE, NATL)

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

TEST_LENGTH=mod(LENF,NATL)
if (TEST_LENGTH .eq. 0) then
 TEST_LENGTH=LENF/NATL
else
 TEST_LENGTH=0
endif

END FUNCTION

SUBROUTINE send_label (sp_id, sp_ln, spec_label) BIND (C,NAME='send_label_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: sp_id, sp_ln
CHARACTER (KIND=c_char), DIMENSION(sp_ln), INTENT(IN) :: spec_label

TL(sp_id)="  "
do i=1, sp_ln
  TL(sp_id)(i:i) = spec_label(i)
enddo

END SUBROUTINE

INTEGER (KIND=c_int) FUNCTION write_xyz (xyz_f, lxyz, fxyz, txyz) BIND (C,NAME='write_xyz_')

!
! output of atom types and coordinates - Multiple XYZ files
!

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: lxyz, fxyz, txyz
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: xyz_f
DOUBLE PRECISION, DIMENSION(3) :: savep
CHARACTER (LEN=lxyz) :: xyz_file

INTERFACE
INTEGER FUNCTION TEST_LENGTH(UNITFILE, NATL)
  INTEGER, INTENT(IN) :: UNITFILE, NATL
END FUNCTION
END INTERFACE

do i=1, lxyz
  xyz_file(i:i) = xyz_f(i)
enddo

write_xyz=1
open (unit=20, file=xyz_file, action='write', status='unknown', err=001)
do i=1, NS
  write (20, '(i10)') NA
  write (20, *)
  do j=1, NA
    if (fxyz .eq. 1) then
      if (NCELLS .gt. 1) then
        NBOX => THE_BOX(i)
      else
        NBOX => THE_BOX(1)
      endif
      savep = MATMUL(FULLPOS(j,:,i),NBOX%carttofrac)
    else
      savep = FULLPOS(j,:,i)
      if (txyz .eq. 1) then
        savep(:) = savep(:)/ANGTOBOHR
      endif
    endif
    write (20, '(a2,3(3x,f15.10))', err=002) TL(LOT(j)), savep
  enddo
enddo
write_xyz=0

002 continue

close (20)

001 continue

END FUNCTION
