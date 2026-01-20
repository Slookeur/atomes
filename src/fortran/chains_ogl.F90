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
!! @file chains_ogl.F90
!! @short Send chain statistics data to C for OpenGL rendering
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER FUNCTION CHAINS_TO_OGL (STEP, NRI, RSAVED)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: STEP
INTEGER, DIMENSION(TAILLC, NS), INTENT(IN) :: NRI
INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(IN) :: RSAVED
INTEGER, DIMENSION(NUMA) :: CHAIN_LIST
INTEGER, DIMENSION(:), ALLOCATABLE :: CHAIN_ID
INTEGER :: RAA, RAB, RAC, RAD, RAE

RAB=0
do RAA=2, TAILLC
  if (NRI(RAA,STEP) > 0) then
    RAB=RAB+1
    call allocate_all_chains (STEP-1, RAA, NRI(RAA,STEP))
  endif
enddo
if (RAB.eq.0) write (6, *) "Chains RAB=0"

do RAA=1, NA
  do RAB=2, TAILLC
    RAC = 0
    do RAD=1, NUMA
      CHAIN_LIST(RAD) = 0
    enddo
    do RAD=1, NRI(RAB,STEP)
      do RAE=1, RAB
        if (RSAVED(RAB,RAD,RAE) .eq. RAA) then
          RAC=RAC+1
          CHAIN_LIST(RAC) = RAD
          goto 001
        endif
      enddo
      001 continue
    enddo
    if (allocated(CHAIN_ID)) deallocate(CHAIN_ID)
    allocate(CHAIN_ID(RAC), STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: CHAINS_TO_OGL"//CHAR(0), "Table: CHAIN_ID (1)"//CHAR(0))
      CHAINS_TO_OGL = 0
      goto 002
    endif
    do RAD=1, RAC
      CHAIN_ID(RAD) = CHAIN_LIST(RAD)
    enddo
    if (RAC > 0) call send_atom_chains_id_opengl (STEP-1, RAA-1, RAB, RAC, CHAIN_ID)
  enddo
enddo

do RAA=2, TAILLC
  if (allocated(CHAIN_ID)) deallocate(CHAIN_ID)
  allocate(CHAIN_ID(RAA), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: CHAINS_TO_OGL"//CHAR(0), "Table: CHAIN_ID (2)"//CHAR(0))
    CHAINS_TO_OGL = 0
    goto 002
  endif
  if (NRI(RAA,STEP) > 0) then
    do RAB=1, NRI(RAA,STEP)
      do RAC=1, RAA
        CHAIN_ID(RAC) = RSAVED(RAA,RAB,RAC)
      enddo
      call send_chains_opengl (STEP-1, RAA, RAB-1, CHAIN_ID)
    enddo
  endif
enddo

CHAINS_TO_OGL = 1

if (allocated(CHAIN_ID)) deallocate(CHAIN_ID)

002 continue

END FUNCTION

INTEGER FUNCTION CHAINS_TO_OGL_MENU (NRI)

USE PARAMETERS

IMPLICIT NONE

INTEGER, DIMENSION(TAILLC, NS), INTENT(IN) :: NRI
INTEGER, DIMENSION(:), ALLOCATABLE :: CHAIN_ID, CHAIN_JD, CHAIN_KD

if (allocated(CHAIN_ID)) deallocate(CHAIN_ID)
allocate(CHAIN_ID(TAILLC-1), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: CHAINS_TO_OGL_MENU"//CHAR(0), "Table: CHAIN_ID "//CHAR(0))
  CHAINS_TO_OGL_MENU = 0
  goto 001
endif

k = 0
do i=2, TAILLC
  CHAIN_ID(i-1) = 0
  do j=1, NS
    CHAIN_ID(i-1) = CHAIN_ID(i-1) + NRI(i,j)
  enddo
  if (CHAIN_ID(i-1) > 0) then
    k = k + 1
  endif
enddo

if (allocated(CHAIN_JD)) deallocate(CHAIN_JD)
allocate(CHAIN_JD(k), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: CHAINS_TO_OGL_MENU"//CHAR(0), "Table: CHAIN_JD "//CHAR(0))
  CHAINS_TO_OGL_MENU = 0
  goto 001
endif
if (allocated(CHAIN_KD)) deallocate(CHAIN_KD)
allocate(CHAIN_KD(k), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: CHAINS_TO_OGL_MENU"//CHAR(0), "Table: CHAIN_KD "//CHAR(0))
  CHAINS_TO_OGL_MENU = 0
  goto 001
endif

k = 0
do i=2, TAILLC
  if (CHAIN_ID(i-1) > 0) then
    k = k + 1
    CHAIN_JD(k) = i
    CHAIN_KD(k) = CHAIN_ID(i-1)
  endif
enddo

if (k > 0) then
  call send_coord_opengl (9, 1, 0, 0, k, k)
  call init_menurings (9, 5, k, CHAIN_JD, CHAIN_KD)
endif

if (allocated(CHAIN_ID)) deallocate(CHAIN_ID)
if (allocated(CHAIN_JD)) deallocate(CHAIN_JD)
if (allocated(CHAIN_KD)) deallocate(CHAIN_KD)

CHAINS_TO_OGL_MENU = 1

001 continue

END FUNCTION
