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
!! @file prepdata.F90
!! @short First level analysis of atomic coordinates
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif

SUBROUTINE free_contj_voisj () BIND (C,NAME='free_contj_voisj_')

USE PARAMETERS

IMPLICIT NONE

if (allocated(VOISJ)) deallocate(VOISJ)
if (allocated(CONTJ)) deallocate(CONTJ)

END SUBROUTINE

SUBROUTINE read_contj (ATO, STP, CON) BIND (C,NAME='read_contj_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: ATO, STP, CON

CONTJ(ATO+1,STP+1) = CON

END SUBROUTINE

SUBROUTINE read_voisj (ATO, STP, CID, VID) BIND (C,NAME='read_voisj_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: ATO, STP, CID, VID

VOISJ(CID+1,ATO+1,STP+1) = VID+1

END SUBROUTINE

INTEGER (KIND=c_int) FUNCTION alloc_contj_voisj (N1, N2) BIND (C,NAME='alloc_contj_voisj_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: N1, N2

! N1 = atomes, N2 = steps
if (allocated(VOISJ)) deallocate(VOISJ)
allocate(VOISJ(MAXN,N1,N2), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: alloc_cont_vois"//CHAR(0), "Table: VOISJ"//CHAR(0))
  alloc_contj_voisj = 0
  goto 001
endif
if (allocated(CONTJ)) deallocate(CONTJ)
allocate(CONTJ(N1,N2), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: alloc_cont_vois"//CHAR(0), "Table: CONTJ"//CHAR(0))
  alloc_contj_voisj = 0
endif

alloc_contj_voisj = 1

001 continue

END FUNCTION

INTEGER (KIND=c_int) FUNCTION alloc_data (N1, N2, N3) BIND (C,NAME='alloc_data_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: N1, N2, N3

INTERFACE
  INTEGER FUNCTION ALLOCHEM()
  END FUNCTION
END INTERFACE

NA=N1
NSP=N2
NS=N3

if (allocated(FULLPOS)) deallocate(FULLPOS)
allocate (FULLPOS(NA,3,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: alloc_data"//CHAR(0), "Table: FULLPOS"//CHAR(0))
  alloc_data = 0
  goto 001
endif
if (allocated(LOT)) deallocate(LOT)
allocate(LOT(NA), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: alloc_data"//CHAR(0), "Table: LOT"//CHAR(0))
  alloc_data = 0
  goto 001
endif
alloc_data = ALLOCHEM ()

001 continue

END FUNCTION

SUBROUTINE prep_spec (idatoms, nsps, open_apf) BIND (C,NAME='prep_spec_')

USE PARAMETERS
USE MENDELEIEV

IMPLICIT NONE

INTEGER (KIND=c_int), DIMENSION(NSP), INTENT(IN) :: nsps
REAL (KIND=c_double), DIMENSION(NSP), INTENT(IN) :: idatoms
INTEGER (KIND=c_int), INTENT(IN) :: open_apf

CHARACTER (LEN=14) :: ELEM
! Now we are calling the GTK+ routines

do i=1, NSP
  NBSPBS(i) = nsps(i)
  j = int(idatoms(i))
  ATOMID(i) = j
  if (open_apf .eq. 1) then
    if (j .gt. 0) then
      TL(i) = ATSYM(j)
      ELEM = ELEMENT(j)
    else
      TL(i) = "X "
      ELEM = "Unknown"
    endif
    ! In C all string must be terminated by a CHAR(0)
    ! To spec_data_
    call spec_data (0, i-1, ATOMID(i), NBSPBS(i), &
                    TL(i)//CHAR(0), ELEM//CHAR(0), &
                    0.0d0, 0.0d0, 0.0d0, 0.0d0)
  endif
enddo
NBSPBS(NSP+1) = NA

END SUBROUTINE

SUBROUTINE read_chem (PMASS, PRAD, PNSCATT, PXSCATT) BIND (C,NAME='read_chem_')

USE PARAMETERS

IMPLICIT NONE

REAL (KIND=c_double), DIMENSION(NSP) :: PMASS, PRAD, PNSCATT, PXSCATT

do i=1, NSP
  MASS(i) = PMASS(i)
  RVDW(i) = PRAD(i)
  NSCATTL(i) = PNSCATT(i)
  XSCATTL(i) = PXSCATT(i)
enddo

END SUBROUTINE

SUBROUTINE read_data (PLOT, PBS) BIND (C,NAME='read_data_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), DIMENSION(NA), INTENT(IN) :: PLOT
INTEGER (KIND=c_int), DIMENSION(NSP), INTENT(IN) :: PBS

do i=1, NA
  LOT(i)=PLOT(i)+1
enddo
do i=1, NSP
  NBSPBS(i) = PBS(i)
  Xi(i) = dble(NBSPBS(i)) / dble(NA)
enddo
NBSPBS(NSP+1) = NA

END SUBROUTINE

SUBROUTINE read_pos (PCX, PCY, PCZ) BIND (C,NAME='read_pos_')

USE PARAMETERS

REAL (KIND=c_double), DIMENSION(NA*NS), INTENT(IN) :: PCX, PCY, PCZ

k=0
do i=1, NS
  do j=1, NA
    k=k+1
    FULLPOS(j,1,i)=PCX(k)
    FULLPOS(j,2,i)=PCY(k)
    FULLPOS(j,3,i)=PCZ(k)
  enddo
enddo

END SUBROUTINE

INTEGER FUNCTION SEND_POS (NPA, NPS, NLOT, POSTAB)

INTEGER :: i, j, k, ERR
INTEGER, INTENT(IN) :: NPA, NPS
INTEGER, DIMENSION(:), INTENT(IN) :: NLOT
DOUBLE PRECISION, DIMENSION(:,:,:), INTENT(IN) :: POSTAB
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: XPOS, YPOS, ZPOS

if (allocated(XPOS)) deallocate(XPOS)
allocate(XPOS(NPA*NPS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: SEND_POS"//CHAR(0), "Table: XPOS"//CHAR(0))
  SEND_POS = 0
  goto 001
endif
if (allocated(YPOS)) deallocate(YPOS)
allocate(YPOS(NPA*NPS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: SEND_POS"//CHAR(0), "Table: YPOS"//CHAR(0))
  SEND_POS = 0
  goto 001
endif
if (allocated(ZPOS)) deallocate(ZPOS)
allocate(ZPOS(NPA*NPS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: SEND_POS"//CHAR(0), "Table: ZPOS"//CHAR(0))
  SEND_POS = 0
  goto 001
endif

k=0
do i=1, NPS
do j=1, NPA
  k=k+1
  XPOS(k)=POSTAB(j,1,i)
  YPOS(k)=POSTAB(j,2,i)
  ZPOS(k)=POSTAB(j,3,i)
enddo
enddo

! To 'save_pos_'
call save_pos (NPA, NLOT, k, XPOS, YPOS, ZPOS)

SEND_POS = 1

001 continue

if (allocated(XPOS)) deallocate (XPOS)
if (allocated(YPOS)) deallocate (YPOS)
if (allocated(ZPOS)) deallocate (ZPOS)

END FUNCTION

INTEGER (KIND=c_int) FUNCTION prep_data () BIND (C,NAME='prep_data_')

!
! Data initialization
!

USE PARAMETERS
USE MENDELEIEV

IMPLICIT NONE

CHARACTER (LEN=2), DIMENSION(MAXE) :: TTYPE

INTERFACE
  INTEGER FUNCTION SEND_POS(NPA, NPS, NLOT, POSTAB)
    INTEGER, INTENT(IN) :: NPA, NPS
    INTEGER, DIMENSION(:), INTENT(IN) :: NLOT
    DOUBLE PRECISION, DIMENSION(:,:,:), INTENT(IN) :: POSTAB
  END FUNCTION
  INTEGER FUNCTION ALLOCHEM()
  END FUNCTION
  INTEGER FUNCTION FINDID(NAMEAT)
    CHARACTER (LEN=2), INTENT(IN) :: NAMEAT
  END FUNCTION
  INTEGER FUNCTION chemistry ()
  END FUNCTION
END INTERFACE

prep_data = 0

if (allocated(LOT)) deallocate(LOT)
allocate(LOT(NA), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: prep_data"//CHAR(0), "Table: LOT"//CHAR(0))
  goto 001
endif
LOT(:)=0        ! Initialisation du tableau

NOC=1
do NOA=1, NA
  if (NOA .eq. 1) then
    LOT(NOA)=NOC
    TTYPE(NOC)=TAB_OF_TYPE(NOA)
  else if (LOT(NOA) .eq. 0) then
    do NOB=1, NOC
      if (TAB_OF_TYPE(NOA) .eq. TTYPE(NOB)) then
        LOT(NOA)= NOB
      endif
    enddo
    if (LOT(NOA) .eq. 0) then
      NOC=NOC+1
      LOT(NOA)=NOC
      TTYPE(NOC)=TAB_OF_TYPE(NOA)
    endif
  endif
enddo
NSP=NOC

if (ALLOCHEM () == 0) then
  prep_data = 0
  goto 001
endif

if (allocated(ELEMID)) deallocate(ELEMID)
allocate(ELEMID(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: prep_data"//CHAR(0), "Table: ELEMID"//CHAR(0))
  goto 001
endif

NBSPBS(:) = 0
do NOA=1, NA
  NOB=LOT(NOA)
  NBSPBS(NOB)=NBSPBS(NOB)+1
enddo

NBSPBS(NSP+1)=NA

!
! Link between index and label
! Correspondance entre indice de type et label
!

do NOB=1, NSP
  TL(NOB)=TTYPE(NOB)
enddo

do NOA=1, NSP
  ATOMID(NOA) = FINDID(TL(NOA))
  select case (ATOMID(NOA))
    case (-1)
      prep_data = 0
      goto 001
    case (0)
      ELEMID(NOA) = "Dummy "//TL(NOA)
      MASS(NOA) = 1.0
      RVDW(NOA) = 0.5
      NSCATTL(NOA) = 0.0
      XSCATTL(NOA) = 0.0
    case default
      ELEMID(NOA) = ELEMENT(ATOMID(NOA))
      MASS(NOA) = AMASS(ATOMID(NOA))
      XSCATTL(NOA) = ATOMID(NOA)
      if(ATOMID(NOA) < 105) then
        NSCATTL(NOA) = COHEB(ATOMID(NOA))
        ! Covalent radius are the defaut values
        RVDW(NOA) = ARCOV(ATOMID(NOA))
        ! To use ionic radius uncomment the following line
        ! RVDW(NOA) = ARION(ATOMID(NOA))
        ! To use Wander Waals radius uncomment the following line
        ! RVDW(NOA) = ARVDW(ATOMID(NOA))
      else
        NSCATTL(NOA) = 0.0d0
        RVDW(NOA) = 0.0d0
      endif
  end select
  if (NSCATTL(NOA) .eq. 0.00) then
      call show_warning ("Element "//TL(NOA)//" does not have neutron scattering length "//CHAR(0), &
                         "If this is a bug please report it to"//CHAR(0), PACKAGE_BUGREPORT//CHAR(0))
  endif
enddo

! Now we are calling the GTK+ routines
! To init_data_
call init_data (NA, NSP, NS, 1)

do i=1, NSP
! In C all string must be terminated by a CHAR(0)
! To spec_data_
  call spec_data (1, i-1, ATOMID(i), NBSPBS(i), &
                  TL(i)//CHAR(0), ELEMID(i)//CHAR(0), &
                  MASS(i), RVDW(i), NSCATTL(i), XSCATTL(i))
enddo

if (chemistry () .eq. 1) then
  prep_data = SEND_POS (NA, NS, LOT, FULLPOS)
endif

001 continue

if (allocated(ELEMID)) deallocate(ELEMID)

END FUNCTION

INTEGER FUNCTION FINDID(NAMEAT)

USE PARAMETERS
USE MENDELEIEV

INTEGER (KIND=c_int), EXTERNAL :: dummy_ask
INTEGER :: ELEMT
CHARACTER (LEN=2), INTENT(IN) :: NAMEAT

FINDID=0
do ELEMT=1, MAXE

 if (ATSYM(ELEMT) .eq. NAMEAT) then
   FINDID=ELEMT
   exit
 endif

enddo

if (FINDID .eq. 0) then
  call show_warning ("Problem with the atomic coordinates"//CHAR(0), &
                     "Element "//NAMEAT//" does not exist "//CHAR(0), " "//CHAR(0))
  FINDID = dummy_ask ("Do you want to use dummy atom(s) for unknown species "//NAMEAT//" ?"//CHAR(0));
endif

END FUNCTION

REAL (KIND=c_double) FUNCTION set_mass (SW) BIND (C,NAME='set_mass_')

USE MENDELEIEV
USE PARAMETERS

INTEGER (KIND=c_int), INTENT(IN) :: SW

set_mass = AMASS(SW)

END FUNCTION

REAL (KIND=c_double) FUNCTION set_radius (SW, RD) BIND (C,NAME='set_radius_')

USE MENDELEIEV
USE PARAMETERS

INTEGER (KIND=c_int), INTENT(IN) :: SW, RD
DOUBLE PRECISION :: RAD

if (SW .lt. 108) then
  if (RD .eq. 0) then
    RAD = ARCOV(SW)
  else if (RD .eq. 1) then
    RAD = ARION(SW)
  else if (RD .eq. 2) then
    RAD = ARVDW(SW)
  else if (RD .eq. 3) then
    RAD = ARCRY(SW)
  endif
else
  RAD = 0.0
endif
set_radius = RAD

END FUNCTION

REAL (KIND=c_double) FUNCTION set_neutron (SW) BIND (C,NAME='set_neutron_')

USE MENDELEIEV
USE PARAMETERS

INTEGER (KIND=c_int), INTENT(IN) :: SW

if (SW .lt. 105) then
  set_neutron = COHEB(SW)
else
  set_neutron = 0.0
endif

END FUNCTION

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!                Reconstruction des trajectoires réelles
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
SUBROUTINE prep_pos (PINFO, FINFO) BIND (C,NAME='prep_pos_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: PINFO, FINFO

INTERFACE
  INTEGER FUNCTION SEND_POS(NPA, NPS, NLOT, POSTAB)
    INTEGER, INTENT(IN) :: NPA, NPS
    INTEGER, DIMENSION(:), INTENT(IN) :: NLOT
    DOUBLE PRECISION, DIMENSION(:,:,:), INTENT(IN) :: POSTAB
  END FUNCTION
END INTERFACE

PBC=.false.
if (PINFO .eq. 1) then
  PBC=.true.
endif

FRAC=.false.
if (FINFO > 0) FRAC=.true.

if (FRAC) then
  do NOC=1, NS
    do NOA=1, NA
      if (NCELLS .gt. 1) then
        FULLPOS(NOA,:,NOC) = MATMUL(FULLPOS(NOA,:,NOC),THE_BOX(NOC)%fractocart)
      else
        FULLPOS(NOA,:,NOC) = MATMUL(FULLPOS(NOA,:,NOC),THE_BOX(1)%fractocart)
      endif
    enddo
  enddo
endif

NOA = SEND_POS (NA, NS, LOT, FULLPOS)

END SUBROUTINE
