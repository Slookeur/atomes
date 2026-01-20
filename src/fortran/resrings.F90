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
!! @file resrings.F90
!! @short Export results of ring and chain statistics to C
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER FUNCTION RECRINGS(VID)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: VID

DOUBLE PRECISION :: MAMP, EAMP
DOUBLE PRECISION :: TAMP, ETAMP
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: AMP
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: RINGSPNA, ECTRPNA
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: EMIN, EMAX
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: MPNA
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: RTAB

if (TBR) then

  RECRINGS=2
  goto 001

else if (ALC) then

  RECRINGS=0
  goto 001

endif

if (allocated(RTAB)) deallocate(RTAB)
allocate(RTAB(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: RTAB"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(MOYRED)) deallocate(MOYRED)
allocate(MOYRED(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: MOYRED"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(RED)) deallocate(RED)
allocate(RED(NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: RED"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(ECTYPE)) deallocate(ECTYPE)
allocate(ECTYPE(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: ECTYPE"//CHAR(0))
  RECRINGS=0
  goto 001
endif
!if (allocated(MOYPUR)) deallocate(MOYPUR)
!allocate(MOYPUR(TAILLR), STAT=ERR)
!if (ERR .ne. 0) then
!  call show_error ("Impossible to allocate memory"//CHAR(0), &
!                   "Function: RECRINGS"//CHAR(0), "Table: MOYPUR"//CHAR(0))
!  RECRINGS=0
!  goto 001
!endif
!if (allocated(IRRED)) deallocate(IRRED)
!allocate(IRRED(NS), STAT=ERR)
!if (ERR .ne. 0) then
!  call show_error ("Impossible to allocate memory"//CHAR(0), &
!                   "Function: RECRINGS"//CHAR(0), "Table: IRRED"//CHAR(0))
!  RECRINGS=0
!  goto 001
!endif
!if (allocated(ECTYP)) deallocate(ECTYP)
!allocate(ECTYP(TAILLR), STAT=ERR)
!if (ERR .ne. 0) then
!  call show_error ("Impossible to allocate memory"//CHAR(0), &
!                   "Function: RECRINGS"//CHAR(0), "Table: ECTYP"//CHAR(0))
!  RECRINGS=0
!  goto 001
!endif

ECTYPE(:)=0.0d0
!ECTYP(:)=0.0d0

do l=3, TAILLR
  do i=1, NS
    RED(i)=dble(NRING(l,i))/NTLT
!    IRRED(i)=dble(NIRR(l,i))/NTLT
  enddo
  if (NS .gt. 1) then
    MOYRED(l)=0.0d0
!    MOYPUR(l)=0.0d0
    ECTYPE(l)=0.0d0
!    ECTYP(l)=0.0d0
    call MOYENNE(RED, NS, MOYRED(l))
!    call MOYENNE(IRRED, NS, MOYPUR(l))
    call ECT_TYPE(MOYRED(l), RED, NS, ECTYPE(l))
!    call ECT_TYPE(MOYPUR(l), IRRED, NS, ECTYP(l))
  else
    MOYRED(l)=RED(1)
!    MOYPUR(l)=IRRED(1)
  endif
enddo

TAMP=0.0
do i=3, TAILLR
  RTAB(i)=MOYRED(i)
  TAMP = TAMP + RTAB(i)
enddo

if (TAMP .eq. 0.0) then
  RECRINGS=1
  goto 001
endif

l=(NSP+1)*4*VID
if (TLT .ne. NSP+1) l = l + 4*TLT
call save_curve (TAILLR, RTAB, l, IDRI)

if (allocated(MPNA)) deallocate(MPNA)
allocate(MPNA(TAILLR,TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: MPNA"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(EPNA)) deallocate(EPNA)
allocate(EPNA(TAILLR,TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: EPNA"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(RINGSPNA)) deallocate(RINGSPNA)
allocate(RINGSPNA(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: RINGSPNA"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(ECTRPNA)) deallocate(ECTRPNA)
allocate(ECTRPNA(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: ECTPNA"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(RNAMAX)) deallocate(RNAMAX)
allocate(RNAMAX(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: RNAMAX"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(RNAMIN)) deallocate(RNAMIN)
allocate(RNAMIN(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: RNAMIN"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(EMAX)) deallocate(EMAX)
allocate(EMAX(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: EMAX"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(EMIN)) deallocate(EMIN)
allocate(EMIN(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: EMIN"//CHAR(0))
  RECRINGS=0
  goto 001
endif
if (allocated(MTABL)) deallocate(MTABL)
allocate(MTABL(NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: MTABL"//CHAR(0))
  RECRINGS=0
  goto 001
endif

RINGSPNA(:)=0.0d0
ECTRPNA(:)=0.0d0
RNAMAX(:)=0.0d0
RNAMIN(:)=0.0d0
EMAX(:)=0.0d0
EMIN(:)=0.0d0
MPNA(:,:)=0.0d0
EPNA(:,:)=0.0d0

do i=3, TAILLR
  do j=3, TAILLR
    if (NS .gt. 1) then
      do k=1, NS
        MTABL(k)=PNA(i,j,k)
      enddo
      call MOYENNE(MTABL, NS, MPNA(i,j))
      call ECT_TYPE(MPNA(i,j), MTABL, NS, EPNA(i,j))
      if (i.eq.j) then
        RINGSPNA(i)=MPNA(i,j)/NTLT
        ECTRPNA(i)=EPNA(i,j)/NTLT
      endif
    else
      MPNA(i,j)=dble(PNA(i,j,1))/NTLT
      if (i.eq.j) RINGSPNA(i)=MPNA(i,j)
    endif
  enddo
enddo

do i=1, TAILLR
  RTAB(i)=RINGSPNA(i)
enddo
l=l+1
call save_curve (TAILLR, RTAB, l, IDRI)

do i=1, TAILLR
  if (NS .gt. 1) then
    m=0
    do j=1, NS
      if (PNA(i,i,j) .eq. 0) then
        MTABL(j)=0.0d0
      else
        m=m+1
        MTABL(j)=dble(MAXPNA(i,j))/dble(PNA(i,i,j))
      endif
    enddo
    if (m.gt.1) then
      call MOYENNE(MTABL, NS, RNAMAX(i))
      RNAMAX(i)=RNAMAX(i)*NS/m
      call ECT_TYPE_RINGS(RNAMAX(i), MTABL, NS, m, EMAX(i))
    elseif (m.eq.1) then
      RNAMAX(i)=RNAMAX(i)*NS
      EMAX(i)=0.0d0
    else
      RNAMAX(i)=0.0d0
      EMAX(i)=0.0d0
    endif
    m=0
    do j=1, NS
      if (PNA(i,i,j) .eq. 0) then
        MTABL(j)=0.0d0
      else
        m=m+1
        MTABL(j)=dble(MINPNA(i,j))/dble(PNA(i,i,j))
      endif
    enddo
    if (m.gt.1) then
      call MOYENNE(MTABL, NS, RNAMIN(i))
      RNAMIN(i)=RNAMIN(i)*NS/m
      call ECT_TYPE_RINGS(RNAMIN(i), MTABL, NS, m, EMIN(i))
    elseif (m.eq.1) then
      RNAMIN(i)=RNAMIN(i)*NS
      EMIN(i)=0.0d0
    else
      RNAMIN(i)=0.0d0
      EMIN(i)=0.0d0
    endif
  else
    if (PNA(i,i,1).eq.0.0) then
      RNAMAX(i)=0.0d0
      EMAX(i)=0.0d0
      RNAMIN(i)=0.0d0
      EMIN(i)=0.0d0
    else
      RNAMAX(i)=dble(MAXPNA(i,1))/PNA(i,i,1)
      EMAX(i)=0.0d0
      RNAMIN(i)=dble(MINPNA(i,1))/PNA(i,i,1)
      EMIN(i)=0.0d0
    endif
  endif
enddo

do i=1, TAILLR
  RTAB(i)=RNAMAX(i)
enddo
l=l+1
call save_curve (TAILLR, RTAB, l, IDRI)
do i=1, TAILLR
  RTAB(i)=RNAMIN(i)
enddo
l=l+1
call save_curve (TAILLR, RTAB, l, IDRI)

if (allocated(AMP)) deallocate(AMP)
allocate(AMP(NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: AMP"//CHAR(0))
  RECRINGS=0
  goto 001
endif

do i=1, NS
  AMP(i)=0.0d0
  if (DOAMPAT) then
    do j=1, NTLT
      AMP(i)=AMP(i)+dble(AMPAT(j,i))
    enddo
  endif
enddo
MAMP=0.0d0
EAMP=0.0d0
if (DOAMPAT) then
  if (NS .gt. 1) then
    call MOYENNE(AMP, NS, MAMP)
    call ECT_TYPE(MAMP, AMP, NS, EAMP)
  else
    MAMP=AMP(1)
  endif
endif

if (allocated(TOTPSTEP)) deallocate(TOTPSTEP)
allocate(TOTPSTEP(NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: RECRINGS"//CHAR(0), "Table: TOTPSTEP"//CHAR(0))
  RECRINGS=0
  goto 001
endif

do i=1, NS
  TOTPSTEP(i)=0
  do j=3, TAILLR
    TOTPSTEP(i)=TOTPSTEP(i)+dble(NRING(j,i))
  enddo
enddo

TAMP=0.0d0
ETAMP=0.0d0
call MOYENNE(TOTPSTEP, NS, TAMP)
call ECT_TYPE(TAMP, TOTPSTEP, NS, ETAMP)
call save_rings_data (TAILLR, ECTYPE, ECTRPNA, EMAX, EMIN, TAMP, ETAMP, MAMP, EAMP)

RECRINGS=1

001 continue

if (allocated(RTAB)) deallocate(RTAB)
if (allocated(AMPAT)) deallocate(AMPAT)
if (allocated(AMP)) deallocate(AMP)
if (allocated(TOTPSTEP)) deallocate(TOTPSTEP)
if (allocated(MOYRED)) deallocate(MOYRED)
if (allocated(RED)) deallocate(RED)
if (allocated(ECTYPE)) deallocate(ECTYPE)
!if (allocated(MOYPUR)) deallocate(MOYPUR)
!if (allocated(IRRED)) deallocate(IRRED)
!if (allocated(ECTYP)) deallocate(ECTYP)
if (allocated(MPNA)) deallocate(MPNA)
if (allocated(EPNA)) deallocate(EPNA)
if (allocated(RINGSPNA)) deallocate(RINGSPNA)
if (allocated(ECTRPNA)) deallocate(ECTRPNA)
if (allocated(RNAMAX)) deallocate(RNAMAX)
if (allocated(RNAMIN)) deallocate(RNAMIN)
if (allocated(EMAX)) deallocate(EMAX)
if (allocated(EMIN)) deallocate(EMIN)
if (allocated(MTABL)) deallocate(MTABL)
if (allocated(NRING)) deallocate(NRING)

END FUNCTION
