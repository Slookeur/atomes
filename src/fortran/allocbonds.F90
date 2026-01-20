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
!! @file allocbonds.F90
!! @short Memory allocation for bonding properties analysis
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

LOGICAL FUNCTION ALLOCEDCO (alloc)

!
! Memory allocation for Edge and Corner sharing analysis
!

USE PARAMETERS

IMPLICIT NONE

LOGICAL, INTENT(IN) :: alloc

if (allocated(EDGETA)) deallocate(EDGETA)
if (allocated(CORTA)) deallocate(CORTA)
if (allocated(DEFTA)) deallocate(DEFTA)
if (allocated(TDA)) deallocate(TDA)
if (allocated(CORNERA)) deallocate(CORNERA)
if (allocated(EDGEA)) deallocate(EDGEA)
if (allocated(DEFA)) deallocate(DEFA)
if (allocated(TDSA)) deallocate(TDSA)
if (allocated(EABL)) deallocate(EABL)
if (allocated(CABL)) deallocate(CABL)
if (allocated(DABL)) deallocate(DABL)
if (allocated(ETABL)) deallocate(ETABL)
if (allocated(CTABL)) deallocate(CTABL)
if (allocated(DTABL)) deallocate(DTABL)
if (allocated(ECTABL)) deallocate(ECTABL)
if (allocated(TDTABL)) deallocate(TDTABL)
if (allocated(ETYPEA)) deallocate(ETYPEA)
if (allocated(CTYPEA)) deallocate(CTYPEA)
if (allocated(DETYPEA)) deallocate(DETYPEA)
if (allocated(ETDA)) deallocate(ETDA)
if (allocated(MTABL)) deallocate(MTABL)

if (alloc) then
  allocate(EDGETA(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: EDGETA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(CORTA(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: CORTA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(DEFTA(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: DEFTA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(TDA(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: TDA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  do m=1, NSP
  do n=1, NSP
    EDGETA(n,m)=0
    CORTA(n,m)=0
    DEFTA(n,m)=0
    TDA(n,m)=0
  enddo
  enddo
  allocate(CORNERA(NSP,NSP,NS), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: CORNERA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(EDGEA(NSP,NSP,NS), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: EDGEA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(DEFA(NSP,NSP,NS), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: DEFA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(TDSA(NSP,NSP,NS), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: TDSA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  do m=1, NSP
    do n=1, NSP
      do o=1, NS
        CORNERA(m,n,o)=0
        EDGEA(m,n,o)=0
        DEFA(m,n,o)=0
        TDSA(m,n,o)=0
      enddo
    enddo
  enddo
  allocate(EABL(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: EABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(CABL(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: CABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(DABL(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: DABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(ETABL(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: ETABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(CTABL(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: CTABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(DTABL(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: DTABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(ECTABL(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: ECTABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(TDTABL(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: TDTABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate (ETYPEA(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: ETYPEA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate (CTYPEA(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: CTYPEA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate (DETYPEA(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: DETYPEA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate (ETDA(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: EDTA"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  allocate(MTABL(NS), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCEDCO"//CHAR(0), "Table: MTABL"//CHAR(0))
    ALLOCEDCO=.false.
    goto 001
  endif
  do i=1, NSP
    do j=1,NSP
      ETYPEA(j,i)=0.0d0
      CTYPEA(j,i)=0.0d0
      DETYPEA(j,i)=0.0d0
      ETDA(j,i)=0.0d0
    enddo
  enddo
endif

ALLOCEDCO=.true.

001 continue

END FUNCTION

LOGICAL FUNCTION ALLOCBONDS (alloc)

!
! Memory allocation for bond properties
!

USE PARAMETERS

!INTEGER, INTENT(IN) :: adv
LOGICAL, INTENT(IN) :: alloc

! For neighbors and environments

if (allocated(LA_COUNT)) deallocate(LA_COUNT)
if (allocated(SA_COUNT)) deallocate(SA_COUNT)
if (allocated(MA_COUNT)) deallocate(MA_COUNT)
if (allocated(TOGL)) deallocate(TOGL)
if (allocated(TIGL)) deallocate(TIGL)
if (allocated(NUM_GSA)) deallocate(NUM_GSA)
if (allocated(TOT_GSA)) deallocate(TOT_GSA)
if (allocated(CABL)) deallocate(CABL)
if (allocated(DABL)) deallocate(DABL)
if (allocated(EABL)) deallocate(EABL)
if (allocated(LGSA)) deallocate(LGSA)
if (allocated(NGSA)) deallocate(NGSA)
if (allocated(LP_GEOM)) deallocate(LP_GEOM)
if (allocated(LT_GEOM)) deallocate(LT_GEOM)
if (allocated(CMOY)) deallocate(CMOY)
if (allocated(MAC)) deallocate(MAC)

if (alloc) then
  allocate(LA_COUNT(NA,NSP,NS), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCBONDS"//CHAR(0), "Table: LA_COUNT"//CHAR(0))
    ALLOCBONDS=.false.
    goto 001
  endif
  allocate(SA_COUNT(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCBONDS"//CHAR(0), "Table: SA_COUNT"//CHAR(0))
    ALLOCBONDS=.false.
    goto 001
  endif
  allocate(MA_COUNT(NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCBONDS"//CHAR(0), "Table: MA_COUNT"//CHAR(0))
    ALLOCBONDS=.false.
    goto 001
  endif
  SA_COUNT(:)=0
  MA_COUNT(:,:)=0
  LA_COUNT(:,:,:)=0
  allocate(TOGL(NS*NA), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCBONDS"//CHAR(0), "Table: TOGL"//CHAR(0))
    ALLOCBONDS=.false.
    goto 001
  endif
  allocate(TIGL(NS*NA), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCBONDS"//CHAR(0), "Table: TIGL"//CHAR(0))
    ALLOCBONDS=.false.
    goto 001
  endif
  allocate(NUM_GSA(NSP,NA*10), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCBONDS"//CHAR(0), "Table: NUM_GSA"//CHAR(0))
    ALLOCBONDS=.false.
    goto 001
  endif
  allocate(TOT_GSA(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCBONDS"//CHAR(0), "Table: TOT_GSA"//CHAR(0))
    ALLOCBONDS=.false.
    goto 001
  endif
  allocate(MAC(NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: ALLOCBONDS"//CHAR(0), "Table: MAC"//CHAR(0))
    ALLOCBONDS=.false.
    goto 001
  endif
endif

!if (adv .eq. 1) then
!  ALLOCBONDS = ALLOCEDCO ()
!else
  ALLOCBONDS=.true.
!endif

001 continue

END FUNCTION
