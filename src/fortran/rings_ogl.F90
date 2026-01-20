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
!! @file rings_ogl.F90
!! @short Send ring statistics data to C for OpenGL rendering
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER FUNCTION RINGS_TO_OGL (STEP, IDSEARCH, NRI, RSAVED, OSAVED)

USE PARAMETERS

INTEGER, INTENT(IN) :: STEP, IDSEARCH
INTEGER, DIMENSION(TAILLR, NS), INTENT(IN) :: NRI
INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(IN) :: RSAVED, OSAVED
INTEGER, DIMENSION(NUMA) :: RING_LIST
INTEGER, DIMENSION(:), ALLOCATABLE :: RING_ID
INTEGER :: RAA, RAB, RAC, RAD, RAE

RAB=0
do RAA=3, TAILLR
  if (NRI(RAA,STEP) > 0) then
    RAB=RAB+1
    call allocate_all_rings (IDSEARCH, STEP-1, RAA, NRI(RAA,STEP))
  endif
enddo

do RAA=1, NA
  do RAB=3, TAILLR
    RAC = 0
    do RAD=1, NUMA
      RING_LIST(RAD) = 0
    enddo
    do RAD=1, NRI(RAB,STEP)
      do RAE=1, RAB
        if (RSAVED(RAB,RAD,RAE) .eq. RAA) then
          RAC=RAC+1
          RING_LIST(RAC) = RAD
          goto 001
        endif
      enddo
      001 continue
    enddo
    if (allocated(RING_ID)) deallocate(RING_ID)
    allocate(RING_ID(RAC), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: RINGS_TO_OGL"//CHAR(0), "Table: RING_ID (1)"//CHAR(0))
      RINGS_TO_OGL = 0
      goto 002
    endif
    do RAD=1, RAC
      RING_ID(RAD) = RING_LIST(RAD)
    enddo
    !write (6, '("Sending atom/rings data:: step= ",i1,", at= ",i4," ring= ",i2,", size= ",i2,", num= ",i4)') STEP-1, RAA-1, IDSEARCH, RAB, RAC
    if (RAC > 0) call send_atom_rings_id_opengl (STEP-1, RAA-1, IDSEARCH, RAB-1, RAC, RING_ID)
  enddo
enddo

do RAA=3, TAILLR
  if (allocated(RING_ID)) deallocate(RING_ID)
  allocate(RING_ID(RAA), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: RINGS_TO_OGL"//CHAR(0), "Table: RING_ID (2)"//CHAR(0))
    RINGS_TO_OGL = 0
    goto 002
  endif
  do RAB=1, NRI(RAA,STEP)
    do RAC=1, RAA
      RING_ID(RAC) = OSAVED(RAA,RAB,RAC)
    enddo
    call send_rings_opengl (IDSEARCH, STEP-1, RAA-1, RAB-1, RING_ID)
  enddo
enddo

RINGS_TO_OGL = 1

if (allocated(RING_ID)) deallocate(RING_ID)

002 continue

END FUNCTION

INTEGER FUNCTION RINGS_TO_OGL_BIS (STEP, IDSEARCH, NRI, RSAVED, OSAVED)

USE PARAMETERS

INTEGER, INTENT(IN) :: STEP, IDSEARCH
INTEGER, DIMENSION(TAILLR), INTENT(IN) :: NRI
INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(IN) :: RSAVED, OSAVED
INTEGER, DIMENSION(NUMA) :: RING_LIST
INTEGER, DIMENSION(:), ALLOCATABLE :: RING_ID
INTEGER :: RAA, RAB, RAC, RAD, RAE

do RAA=3, TAILLR
  if (NRI(RAA) > 0) call allocate_all_rings (IDSEARCH, STEP-1, RAA, NRI(RAA))
enddo

do RAA=1, NA
  do RAB=3, TAILLR
    RAC = 0
    do RAD=1, NUMA
      RING_LIST(RAD) = 0
    enddo
    do RAD=1, NRI(RAB)
      do RAE=1, RAB
        if (RSAVED(RAB,RAD,RAE) .eq. RAA) then
          RAC=RAC+1
          RING_LIST(RAC) = RAD
          goto 001
        endif
      enddo
      001 continue
    enddo
    if (allocated(RING_ID)) deallocate(RING_ID)
    allocate(RING_ID(RAC), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: RINGS_TO_OGL_BIS"//CHAR(0), "Table: RING_ID (1)"//CHAR(0))
      RINGS_TO_OGL_BIS = 0
      goto 002
    endif
    do RAD=1, RAC
      RING_ID(RAD) = RING_LIST(RAD)
    enddo
    !write (6, '("Sending atom/rings data:: step= ",i1,", at= ",i4," ring= ",i2,", size= ",i2,", num= ",i4)') STEP-1, RAA-1, IDSEARCH, RAB, RAC
    if (RAC > 0) call send_atom_rings_id_opengl (STEP-1, RAA-1, IDSEARCH, RAB, RAC, RING_ID)
  enddo
enddo

do RAA=3, TAILLR
  if (allocated(RING_ID)) deallocate(RING_ID)
  allocate(RING_ID(RAA), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: RINGS_TO_OGL_BIS"//CHAR(0), "Table: RING_ID (2)"//CHAR(0))
    RINGS_TO_OGL_BIS = 0
    goto 002
  endif
  do RAB=1, NRI(RAA)
    do RAC=1, RAA
      RING_ID(RAC) = OSAVED(RAA,RAB,RAC)
    enddo
    call send_rings_opengl (IDSEARCH, STEP-1, RAA, RAB-1, RING_ID)
  enddo
enddo

RINGS_TO_OGL_BIS = 1

if (allocated(RING_ID)) deallocate(RING_ID)

002 continue

END FUNCTION

INTEGER FUNCTION RINGS_TO_OGL_MENU (IDSEARCH, NRI)

USE PARAMETERS

INTEGER, INTENT(IN) :: IDSEARCH
INTEGER, DIMENSION(TAILLR, NS), INTENT(IN) :: NRI
INTEGER, DIMENSION(:), ALLOCATABLE :: RING_ID, RING_JD

if (allocated(RING_ID)) deallocate(RING_ID)
allocate(RING_ID(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RINGS_TO_OGL_MENU"//CHAR(0), "Table: RING_ID "//CHAR(0))
  RINGS_TO_OGL_MENU = 0
  goto 001
endif

k = 0
do i=1, TAILLR
  RING_ID(i) = 0
  do j=1, NS
    RING_ID(i) = RING_ID(i) + NRI(i,j)
  enddo
  if (RING_ID(i) > 0) then
    k = k + 1
  endif
enddo

if (allocated(RING_JD)) deallocate(RING_JD)
allocate(RING_JD(k), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RINGS_TO_OGL_MENU"//CHAR(0), "Table: RING_JD "//CHAR(0))
  RINGS_TO_OGL_MENU = 0
  goto 001
endif

k = 0
do i=1, TAILLR
  if (RING_ID(i) > 0) then
    k = k + 1
    RING_JD(k) = i
  endif
enddo

call send_coord_opengl (4+IDSEARCH, 1, 0, 0, k, k)
call init_menurings (4+IDSEARCH, IDSEARCH, k, RING_JD, 1)

if (allocated(RING_ID)) deallocate(RING_ID)
if (allocated(RING_JD)) deallocate(RING_JD)

RINGS_TO_OGL_MENU = 1

001 continue

END FUNCTION

INTEGER FUNCTION RINGS_TO_OGL_M (IDSEARCH)

USE PARAMETERS

INTEGER, INTENT(IN) :: IDSEARCH
INTEGER, DIMENSION(:), ALLOCATABLE :: RING_ID, RING_JD

if (allocated(RING_ID)) deallocate(RING_ID)
allocate(RING_ID(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RINGS_TO_OGL_MENU"//CHAR(0), "Table: RING_ID "//CHAR(0))
  RINGS_TO_OGL_M = 0
  goto 001
endif

k = 0
do i=1, TAILLR
  RING_ID(i) = 0
  do j=1, NS
    RING_ID(i) = RING_ID(i) + NRING(i,j)
  enddo
  if (RING_ID(i) > 0) then
    k = k + 1
  endif
enddo

write (6, *) "k= ",k

if (allocated(RING_JD)) deallocate(RING_JD)
allocate(RING_JD(k), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RINGS_TO_OGL_MENU"//CHAR(0), "Table: RING_JD "//CHAR(0))
  RINGS_TO_OGL_M = 0
  goto 001
endif

k = 0
do i=1, TAILLR
  if (RING_ID(i) > 0) then
    k = k + 1
    RING_JD(k) = i
  endif
enddo

call send_coord_opengl (4+IDSEARCH, 1, 0, 0, k, k)
call init_menurings (4+IDSEARCH, IDSEARCH, k, RING_JD, 1)

if (allocated(RING_ID)) deallocate(RING_ID)
if (allocated(RING_JD)) deallocate(RING_JD)

RINGS_TO_OGL_M = 1

001 continue

END FUNCTION
