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
!! @file bonds.F90
!! @short Bonding properties
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION bonding (scf, sbf, adv, bdist, bmin, delt_ij, sfil) BIND (C,NAME='bonding_')

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: scf, sbf, adv, bdist
REAL (KIND=c_double), INTENT(IN) :: bmin, delt_ij
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: sfil
INTEGER, DIMENSION(:), ALLOCATABLE :: GESP
CHARACTER (LEN=scf) :: sfile
DOUBLE PRECISION :: DBD
DOUBLE PRECISION, DIMENSION(3) :: RBD
#ifdef OPENMP
INTEGER :: NUMTH
LOGICAL :: DOATOMS
#endif
TYPE GEOMETRY
  INTEGER :: INDICE
  INTEGER :: COORD
  INTEGER :: NCOORD
  INTEGER, DIMENSION(:), ALLOCATABLE :: GEO
  TYPE (GEOMETRY), POINTER :: FIRST
  TYPE (GEOMETRY), POINTER :: LAST
  TYPE (GEOMETRY), POINTER :: NEXT
  TYPE (GEOMETRY), POINTER :: PREV
END TYPE GEOMETRY

TYPE(GEOMETRY), DIMENSION(:), POINTER :: GEOT, GEOP
TYPE(GEOMETRY), POINTER :: GA

INTERFACE
  LOGICAL FUNCTION ALLOCBONDS (alloc)
    !INTEGER, INTENT(IN) :: adv
    LOGICAL, INTENT(IN) :: alloc
  END FUNCTION
    LOGICAL FUNCTION ALLOCEDCO (alloc)
    LOGICAL, INTENT(IN) :: alloc
  END FUNCTION
  DOUBLE PRECISION FUNCTION  CALCDIJ(R12, AT1, AT2, STEP_1, STEP_2, SID)
    DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: R12
    INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
  END FUNCTION
  LOGICAL FUNCTION EESCS ()
  END FUNCTION
  INTEGER FUNCTION MOLECULES()
  END FUNCTION
END INTERFACE

if (adv .eq. 1) then
  if (allocated(STATBD)) deallocate(STATBD)
  allocate(STATBD(NSP,NSP,0:bdist), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: bonding"//CHAR(0), "Table: STATBD"//CHAR(0))
    bonding=0
    goto 001
  endif
  STATBD(:,:,:)=0
endif

if (.not. ALLOCBONDS(.true.)) then
  bonding=0
  goto 001
endif

if (allocated(GESP)) deallocate(GESP)
allocate(GESP(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bonding"//CHAR(0), "Table: GESP"//CHAR(0))
  bonding=0
  goto 001
endif
if (allocated(LT_GEOM)) deallocate(LT_GEOM)
allocate(LT_GEOM(NSP,NA), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bonding"//CHAR(0), "Table: LT_GEOM"//CHAR(0))
  bonding=0
  goto 001
endif
LT_GEOM(:,:) = 0
allocate(GEOT(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bonding"//CHAR(0), "Pointer: GEOT"//CHAR(0))
  bonding=0
  goto 001
endif
allocate(GEOP(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bonding"//CHAR(0), "Pointer: GEOP"//CHAR(0))
  bonding=0
  goto 001
endif
allocate(GA, STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: bonding"//CHAR(0), "Pointer: GA"//CHAR(0))
  bonding=0
  goto 001
endif
do i=1, NSP
  GEOT(i)%INDICE = 0
  nullify(GEOT(i)%NEXT)
  nullify(GEOT(i)%PREV)
  nullify(GEOT(i)%FIRST)
  nullify(GEOT(i)%LAST)
  GEOP(i)%INDICE = 0
  nullify(GEOP(i)%NEXT)
  nullify(GEOP(i)%PREV)
  nullify(GEOP(i)%FIRST)
  nullify(GEOP(i)%LAST)
enddo

! Décompte des nombres de coordination
! Evaluation of the coordination numbers
#ifdef OPENMP
NUMTH = OMP_GET_MAX_THREADS ()
DOATOMS=.false.
if (NS.lt.NUMTH) then
  if (NUMTH .ge. 2*(NS-1)) then
    DOATOMS=.true.
  else
    NUMTH=NS
  endif
endif

if (ALL_ATOMS) DOATOMS=.true.

if (DOATOMS) then

  if (NA.lt.NUMTH) NUMTH=NA
  do i=1, NS
    ! OpemMP on Atoms
    !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
    !$OMP& PRIVATE(j, l, k, m, n, o, p, DBD, RBD, GESP, GA) &
    !$OMP& SHARED(NUMTH, NS, i, NA, NCELLS, LOT, CONTJ, VOISJ, LA_COUNT, STATBD, adv, bmin, delt_ij)
    !$OMP DO SCHEDULE(STATIC,NA/NUMTH)
    do j=1, NA
      k = LOT(j)
      l = CONTJ(j,i)
      do m=1, l
        n = VOISJ(m,j,i)
        o = LOT(n)
        LA_COUNT(j,o,i)=LA_COUNT(j,o,i)+1
        if (adv .eq. 1) then
          if (NCELLS .gt. 1) then
            DBD = CALCDIJ (RBD, j, n, i, i, i)
          else
            DBD = CALCDIJ (RBD, j, n, i, i, 1)
          endif
          DBD = sqrt(DBD)
          p =INT((DBD-bmin)/delt_ij)
          !$OMP ATOMIC
          STATBD(k,o,p)=STATBD(k,o,p) + 1
        endif
      enddo
    enddo
    !$OMP END DO NOWAIT
    !$OMP END PARALLEL
  enddo
else
  ! OpemMP on MD steps
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(i, j, k, l, m, n, o, p, DBD, RBD, GESP, GA) &
  !$OMP& SHARED(NUMTH, NS, NA, NCELLS, LOT, CONTJ, VOISJ, LA_COUNT, STATBD, adv, bmin, delt_ij)
  !$OMP DO SCHEDULE(STATIC,NS/NUMTH)
#endif
 do i=1, NS
    do j=1, NA
      k = LOT(j)
      l = CONTJ(j,i)
      do m=1, l
        n = VOISJ(m,j,i)
        o = LOT(n)
        LA_COUNT(j,o,i)=LA_COUNT(j,o,i)+1
        if (adv .eq. 1) then
          if (NCELLS .gt. 1) then
            DBD = CALCDIJ (RBD, j, n, i, i, i)
          else
            DBD = CALCDIJ (RBD, j, n, i, i, 1)
          endif
          DBD = sqrt(DBD)
          p =INT((DBD-bmin)/delt_ij)
          !$OMP ATOMIC
          STATBD(k,o,p)=STATBD(k,o,p) + 1
        endif
      enddo
    enddo
  enddo
#ifdef OPENMP
  !$OMP END DO NOWAIT
  !$OMP END PARALLEL
endif
#endif

if (sbf .eq. 1 .and. scf.gt.0) then
  do i=1, scf
    sfile(i:i) = sfil(i)
  enddo
  open(unit=100, file=sfile, action="write", status='unknown')
  do i=1, NS
    write (100, '("Configuration N°",i6)') i
    write (100, *)
    do j=1, NA
      write (100, '(A2,i6)') TL(LOT(j)), j
      write (100, '(4x,"Nc[tot]= ",i2)') CONTJ(j,i)
      do k=1, NSP
        write (100, '(5x,"Nc[",A2,"]= ",i2)') TL(k), LA_COUNT(j,k,i)
        do l=1, CONTJ(j,i)
          if (LOT(VOISJ(l,j,i)) .eq. l) then
            if (NCELLS .gt. 1) then
              DBD = CALCDIJ (RBD, j, VOISJ(l,j,i), i, i, i)
            else
              DBD = CALCDIJ (RBD, j, VOISJ(l,j,i), i, i, 1)
            endif
            write (100, '(10x,A2,1x,i6,1x,"at",1x,f7.5,1x,"Å")') TL(k), VOISJ(l,j,i), sqrt(DBD)
          endif
        enddo
      enddo
    enddo
  enddo
  close(100)
endif

do i=1, NS
  do j=1, NA
    k = LOT(j)
    l = 0
    do m=1, NSP
      n = LA_COUNT(j,m,i)
      l = l + n
      GESP(m) = n
      MA_COUNT(k,m)=MA_COUNT(k,m)+n
      SA_COUNT(k)=SA_COUNT(k)+n
    enddo
    GA => GEOP(k)
    m = NEWCOORD(1, l, GA, NSP, GESP)
    GA => GEOT(k)
    n = NEWCOORD(0, l, GA, NSP, GESP)
    if (m > 0 .and. n > 0) then
      LT_GEOM(k,n) = 1
      TOGL((i-1)*NA+j) = m
      TIGL((i-1)*NA+j) = n
    else
      bonding=0
      goto 001
    endif
  enddo
enddo

n=0
m=0

do i=1, NSP
  j = GEOP(i)%LAST%INDICE
  n = n + j
  GA => GEOP(i)%FIRST
  do k=1, j
    m = max(m, GA%COORD)
    if (k < j) GA => GA%NEXT
  enddo
enddo

call send_coord_opengl (1, NS*NA, 0, 0, n, TOGL)
if (allocated(TOGL)) deallocate(TOGL)

o = 0
n = 0
m = 20
do i=1, NSP
  j = GEOT(i)%LAST%INDICE
  o = o + j
  GA => GEOT(i)%FIRST
  do k=1, j
    n = max(n, GA%COORD)
    m = min(m, GA%COORD)
    if (k < j) GA => GA%NEXT
  enddo
enddo

call send_coord_opengl (0, NS*NA, m, n, o, TIGL)

do i=1, NSP
  j = GEOT(i)%LAST%INDICE
  if (allocated(LGSA)) deallocate(LGSA)
  allocate(LGSA(j), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: bonding"//CHAR(0), "Table: LGSA"//CHAR(0))
    bonding=0
    goto 001
  endif
  GA => GEOT(i)%FIRST
  do k=1, j
    LGSA(k) = GA%COORD
    if (k < j) GA => GA%NEXT
  enddo
  call init_menu_coordinations (0, i-1, j, LGSA)
enddo

if (allocated(LGSA)) deallocate(LGSA)
if (allocated(TIGL)) deallocate(TIGL)
do i=1, NSP
  SA_COUNT(i)=SA_COUNT(i)/NS/NBSPBS(i)
  do j=1, NSP
    MA_COUNT(i,j)=MA_COUNT(i,j)/NS/NBSPBS(i)
  enddo
enddo

do i=1, NSP
  do j=1, NSP
    MAC(j)=MA_COUNT(i,j)
  enddo
  call coordout (i-1, SA_COUNT(i), MAC, GEOP(i)%LAST%INDICE)
enddo

do i=1, NSP
  j = GEOP(i)%LAST%INDICE
  if (allocated(NGSA)) deallocate(NGSA)
  allocate(NGSA(j), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: bonding"//CHAR(0), "Table: NGSA"//CHAR(0))
    bonding=0
    goto 001
  endif
  if (allocated(LGSA)) deallocate(LGSA)
  allocate(LGSA(j), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: bonding"//CHAR(0), "Table: LGSA"//CHAR(0))
    bonding=0
    goto 001
  endif
  GA => GEOP(i)%FIRST
  call allocate_partial_geo (i-1, j)
  do k=1, j
    LGSA(k)=GA%COORD
    NGSA(k)=GA%NCOORD
    do l=1, NSP
      GESP(l) = GA%GEO(l)
    enddo
    call partial_geo_out (i-1, k-1, NSP, GESP)
    if (k .lt. j) GA => GA%NEXT
  enddo
  call envout (i-1, j, NGSA)
  call init_menu_coordinations (1, i-1, j, LGSA)
  if (allocated(NGSA)) deallocate(NGSA)
  if (allocated(LGSA)) deallocate(LGSA)
enddo
call CLEAN_GEOM ()
if (allocated(GESP)) deallocate(GESP)

if (adv .eq. 1) then
  if (.not.EESCS()) then
    bonding=0
    goto 001
  endif
  do i=1, NSP
    do j=1, NSP
      EABL(j)=EDGETA(i,j)
      CABL(j)=CORTA(i,j)
      DABL(j)=DEFTA(i,j)
      ETABL(j)=ETYPEA(i,j)
      CTABL(j)=CTYPEA(i,j)
      DTABL(j)=DETYPEA(i,j)
      TDTABL(j)=TDA(i,j)
      ECTABL(j)=ETDA(i,j)
    enddo
    call tetraout (i-1, EABL, CABL, DABL, ETABL, CTABL, DTABL, TDTABL, ECTABL)
  enddo
endif
if (.not.ALLOCEDCO(.false.)) then
  bonding=0
  goto 001
endif

if (adv .eq. 1) then

  if (allocated(EABL)) deallocate(EABL)
  allocate(EABL(0:bdist), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: bonding"//CHAR(0), "Table: EABL"//CHAR(0))
    bonding=0
    goto 001
  endif
  do k=0, bdist
    EABL(k) = 0.0
  enddo
  l=0
  do i=1, NSP
  do j=1, NSP
    m=0
    do k=0, bdist
      m = m + STATBD(i,j,k)
    enddo
    if (m .ne. 0) then
      do k=0, bdist
        EABL(k) = 100.0*STATBD(i,j,k)/m
      enddo
      call save_curve (bdist+1, EABL, l, IDBD)
    else
      call save_curve (0, EABL, l, IDBD)
    endif
    l=l+1
  enddo
  enddo
  if (allocated(STATBD)) deallocate(STATBD)
endif

if (.not.ALLOCBONDS(.false.)) then
  bonding=0
  goto 001
endif

bonding=1

001 continue

CONTAINS

INTEGER FUNCTION NEWCOORD (ID, GEO, GP, NP, GSP)

INTEGER, INTENT(IN) :: ID, GEO, NP
INTEGER, DIMENSION(NP), INTENT(IN) :: GSP
TYPE(GEOMETRY), INTENT(INOUT), POINTER :: GP
INTEGER :: AB, AC, AD
LOGICAL :: NEWGEO=.false.
TYPE(GEOMETRY), POINTER :: GM

if (GP%INDICE .eq. 0) then
  GP%INDICE = 1
  GP%COORD = GEO
  GP%NCOORD = 1
  GP%LAST => GP
  GP%FIRST => GP
  NEWCOORD=GP%INDICE
  if (ID .eq. 1) then
    allocate(GP%GEO(NP))
    do AB=1, NP
      GP%GEO(AB) = GSP(AB)
    enddo
  endif
else

  GP => GP%FIRST
  AC = GP%LAST%INDICE
  NEWGEO=.true.
  if (ID .eq. 0) then
    do AB=1, AC
      if (GP%COORD .eq. GEO) then
        NEWGEO=.false.
        NEWCOORD = GP%INDICE
        exit
      endif
      if (AB .lt. AC) GP => GP%NEXT
    enddo
  else
    do AB=1, AC
      if (GP%COORD .eq. GEO) then
        NEWGEO=.false.
        do AD=1, NP
          if (GP%GEO(AD) .ne. GSP(AD)) then
            NEWGEO=.true.
            exit
          endif
        enddo
        if (.not.NEWGEO) exit
      endif
      if (AB .lt. AC) GP => GP%NEXT
    enddo
  endif
  if (NEWGEO) then
    allocate(GM, STAT=ERR)
    if (ERR .ne. 0) then
      call show_error ("Impossible to allocate memory"//CHAR(0), &
                       "Function: NEWCOORD"//CHAR(0), "Pointer: GM"//CHAR(0))
      NEWCOORD=0
      goto 001
    endif
    nullify(GM%FIRST)
    nullify(GM%NEXT)
    nullify(GM%LAST)
    nullify(GM%PREV)
    GM%FIRST => GP%FIRST
    GP%NEXT => GM
    GP%FIRST%LAST => GM
    GP%LAST => GM
    GM%LAST => GM
    GM%PREV => GP
    GM%INDICE = GP%INDICE+1
    GM%COORD = GEO
    GM%NCOORD = 1
    NEWCOORD = GM%INDICE
    if (ID .eq. 1) then
      allocate(GM%GEO(NP))
      do AB=1, NP
        GM%GEO(AB) = GSP(AB)
      enddo
    endif
    GP => GM
  else
    NEWCOORD = GP%INDICE
    GP%NCOORD = GP%NCOORD+1
  endif

endif

001 continue

END FUNCTION

SUBROUTINE CLEAN_GEOM ()

INTEGER :: AB, AC, AD

do AB=1, NSP
  GA => GEOT(AB)%LAST
  AC=GA%INDICE
  do AD=1, AC-1
    GA => GA%PREV
    deallocate (GA%NEXT)
  enddo
enddo
deallocate(GEOT)

do AB=1, NSP
  GA => GEOP(AB)%LAST
  AC=GA%INDICE
  do AD=1, AC-1
    GA => GA%PREV
    deallocate (GA%NEXT)
  enddo
enddo
deallocate(GEOP)

END SUBROUTINE

END FUNCTION

SUBROUTINE sendcuts (cspa, cspb, cutab) BIND (C,NAME='sendcuts_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: cspa, cspb
REAL (KIND=c_double), INTENT(IN) :: cutab

if (cspa .eq. NSP) then
  Gr_cutoff=cutab**2
else
  Gr_CUT(cspa+1,cspb+1) = cutab**2
endif

END SUBROUTINE

REAL (KIND=c_double) FUNCTION fdmax(use_pbc) BIND (C,NAME='fdmax_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: use_pbc

if (use_pbc .eq. 0) then
  do l=1,3
    pmin(l) = FULLPOS(1,l,1)
    pmax(l) = pmin(l)
  enddo
  do i=1, NS
    if (i .eq. 1) then
      k = 2
    else
      k = 1
    endif
    do j=k, NA
      do l=1,3
        pmin(l) = min(pmin(l), FULLPOS(j,l,i))
        pmax(l) = max(pmax(l), FULLPOS(j,l,i))
      enddo
    enddo
  enddo
  fdmax = 0.0d0
  do l=1,3
    fdmax = fdmax + (pmax(l)-pmin(l))**2
  enddo
  fdmax = sqrt(fdmax)
else
  if (THE_BOX(1)%CUBIC) then
    fdmax = sqrt(2.0)*THE_BOX(1)%minv/2.0
  else
    fdmax = THE_BOX(1)%minv/2.0
  endif
  if (NCELLS .gt. 1) then
    do l=2, NCELLS
      if (THE_BOX(l)%CUBIC) then
        fdmax = min(fdmax,sqrt(2.0)*THE_BOX(l)%minv/2.0)
      else
        fdmax = min(fdmax, THE_BOX(l)%minv/2.0)
      endif
    enddo
  endif

endif

END FUNCTION

REAL (KIND=c_double) FUNCTION fkmin(use_pbc) BIND (C,NAME='fkmin_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: use_pbc

if (use_pbc .eq. 0) then
  fkmin = 0.0
else
  fkmin = 0.0
  do l=1, NCELLS
    fkmin = fkmin + THE_BOX(l)%minr
  enddo
  fkmin = fkmin / NCELLS
endif

END FUNCTION

REAL (KIND=c_double) FUNCTION oglmax() BIND (C,NAME='oglmax_')

USE PARAMETERS

IMPLICIT NONE

DOUBLE PRECISION :: maxd
DOUBLE PRECISION, DIMENSION(3) :: dmax

do l=1,3
  pmin(l) = FULLPOS(1,l,1)
  pmax(l) = pmin(l)
enddo
do i=1, NS
  if (i .eq. 1) then
    k = 2
  else
    k = 1
  endif
  do j=k, NA
    do l=1,3
      pmin(l) = min(pmin(l), FULLPOS(j,l,i))
      pmax(l) = max(pmax(l), FULLPOS(j,l,i))
    enddo
  enddo
enddo

allocate (NFULLPOS(2,3,1))
do l=1, 3
  NFULLPOS(1,l,1) = pmin(l)
  NFULLPOS(2,l,1) = pmax(l)
enddo
call CALCRIJ (1, 2, -2, 1, 1)
deallocate (NFULLPOS)

if (PBC) then
  oglmax = sqrt(Dij)
  dmax(:) = 0.0d0
  maxd = 0.0d0
  do l=1, NCELLS
    do m=1, 3
      dmax(m) = max(dmax(m), THE_BOX(l)%modv(m)*THE_BOX(l)%modv(m))
    enddo
  enddo
  maxd = sqrt(dmax(1)+dmax(2)+dmax(3))
  if (oglmax < maxd) oglmax = maxd;
else
  oglmax=sqrt(Dij)
endif
oglmax = 2.0*oglmax
if (oglmax .lt. 10.0) oglmax = 10.0

END FUNCTION
