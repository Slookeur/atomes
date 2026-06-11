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
!! @file gr.F90
!! @short g(r) analysis: direct real space calculation
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION g_of_r (NDR, DTR, FCR) BIND (C,NAME='g_of_r_')

! Radial Pair Distribution function

!
!             -2                             V
!    g(r)= rho    < Sum Sum d(ri)d(rj-r) > = -- < Sum Sum d(r-rij) >
!                    i  j!i                  N²    i  j!i
!
!
!
!
!                1                    rho(b)                 rho(b)
!    g  (r)= ------------  Sum Sum  ----------  = Sum Sum ------------------
!     ab     4*PI*Delat(r)  a  b!a  d(ab)²*N(a)    a  b!a  Vshell(d(ab))*N(a)
!

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: NDR, FCR
REAL (KIND=c_double), INTENT(IN) :: DTR
DOUBLE PRECISION :: Hcap1, Hcap2, Vcap, Dgr
DOUBLE PRECISION :: NORM_FACT, GRLIM
DOUBLE PRECISION :: SUML, XSUML
LOGICAL :: IS_CRYSTAL=.false.
#ifdef OPENMP
INTEGER :: NUMTH
LOGICAL :: DOATOMS
#endif

INTERFACE
  LOGICAL FUNCTION ALLOCGR(NDR)
    INTEGER, INTENT(IN) :: NDR
  END FUNCTION
  LOGICAL FUNCTION GRBT(GrToBT, NDTR)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: NDTR
    DOUBLE PRECISION, DIMENSION(NDTR,NSP,NSP), INTENT(IN) :: GrToBT
  END FUNCTION
  DOUBLE PRECISION FUNCTION CALCDIJ (R12, AT1, AT2, STEP_1, STEP_2, SID)
    DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: R12
    INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
  END FUNCTION
END INTERFACE

if (.not. allocgr(NDR)) then
  g_of_r = 0
  goto 001
endif

do i=1, NDR+1
  SHELL_VOL(i) = 0.0d0
  SHELL_VOL(i) = 4.0d0*PI*(((i-1)*DTR+DTR)**3 - ((i-1)*DTR)**3)/3
! To take into account the atoms between a/2 and a srqt(3)/2
! We need the small volume they can be found in.
  if (OVERALL_CUBIC .and. i*DTR.gt.MBOX) then
    Hcap1=i*DTR-MBOX
    if ((i-1)*DTR .le. MBOX) then
      Hcap2=0.0d0
    else
      Hcap2=(i-1)*DTR-MBOX
    endif
    Vcap=Hcap1**2*(3*i*DTR - Hcap1) - Hcap2**2*(3*(i-1)*DTR - Hcap2)
    Vcap=Vcap*PI*2
    SHELL_VOL(i)=SHELL_VOL(i)-Vcap
  endif
enddo

GRLIM = NDR*DTR
GRLIM = GRLIM*GRLIM

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
#endif

if (IS_CRYSTAL) then
  ! To write the case of highly distoreded crystal
else
#ifdef OPENMP
  if (DOATOMS) then
    if (NA.lt.NUMTH) NUMTH=NA
    ! OpemMP on atoms only
    !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
    !$OMP& PRIVATE(Dgr, Dij, Rij, GR_INDEX, NORM_FACT, i, j, k, l, m, n) &
    !$OMP& SHARED(NUMTH, NS, NA, NCELLS, LOT, NBSPBS, SHELL_VOL, DTR, MEANVOL, GRLIM, Gij, Dn, NSP, NDR)
    !$OMP DO SCHEDULE(STATIC,NA-1/NUMTH)
    do i=1, NA-1
      do k=1, NS
        do j=i+1, NA
          if (NCELLS .gt. 1) then
            Dij = CALCDIJ (Rij,i,j,k,k,k)
          else
            Dij = CALCDIJ (Rij,i,j,k,k,1)
          endif
          if (Dij <= GRLIM) then
            l=LOT(i)
            m=LOT(j)
            Dgr = sqrt(Dij)
            if (l .eq. m) then
              n = NBSPBS(l)-1
            else
              n = NBSPBS(l)
            endif
            GR_INDEX = int(Dgr/DTR)+1
            NORM_FACT = 1.0d0/(SHELL_VOL(GR_INDEX)*dble(n))
            !$OMP ATOMIC
            Gij(GR_INDEX,l,m,k) = Gij(GR_INDEX,l,m,k) + NORM_FACT/(dble(NBSPBS(m))/MEANVOL)
            !$OMP ATOMIC
            Dn(GR_INDEX,l,m,k) = Dn(GR_INDEX,l,m,k) + 1.0d0
          endif
        enddo
      enddo
    enddo
    !$OMP END DO NOWAIT
    !$OMP END PARALLEL
  else
    ! OpemMP on MD steps
    !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
    !$OMP& PRIVATE(Dgr, Dij, Rij, GR_INDEX, NORM_FACT, i, j, k, l, m, n) &
    !$OMP& SHARED(NUMTH, NS, NA, NCELLS, LOT, NBSPBS, SHELL_VOL, DTR, MEANVOL, GRLIM, Gij, Dn, NSP, NDR)
    !$OMP DO SCHEDULE(STATIC,NS/NUMTH)
#endif
    do k=1, NS
      do i=1, NA-1
        do j=i+1, NA
          if (NCELLS .gt. 1) then
            Dij = CALCDIJ (Rij,i,j,k,k,k)
          else
            Dij = CALCDIJ (Rij,i,j,k,k,1)
          endif
          if (Dij <= GRLIM) then
            l=LOT(i)
            m=LOT(j)
            Dgr = sqrt(Dij)
            if (l .eq. m) then
              n = NBSPBS(l)-1
            else
              n = NBSPBS(l)
            endif
            GR_INDEX = int(Dgr/DTR)+1
            NORM_FACT = 1.0d0/(SHELL_VOL(GR_INDEX)*dble(n))
#ifdef OPENMP
            !$OMP ATOMIC
#endif
            Gij(GR_INDEX,l,m,k) = Gij(GR_INDEX,l,m,k) + NORM_FACT/(dble(NBSPBS(m))/MEANVOL)
#ifdef OPENMP
            !$OMP ATOMIC
#endif
            Dn(GR_INDEX,l,m,k) = Dn(GR_INDEX,l,m,k) + 1.0d0
          endif
        enddo
      enddo
    enddo
#ifdef OPENMP
    !$OMP END DO NOWAIT
    !$OMP END PARALLEL
  endif
#endif
endif

do i=1, NDR
  do l=1, NS
    do j=1, NSP
      Dn(i,j,j,l) = 2.0d0*Dn(i,j,j,l)/dble(NBSPBS(j)-1)
    enddo
    do j=1, NSP-1
      do k=j+1, NSP
        Dn(i,j,k,l) = Dn(i,j,k,l)+Dn(i,k,j,l)
        Dn(i,k,j,l) = Dn(i,j,k,l)
        Dn(i,j,k,l) = Dn(i,j,k,l)/dble(NBSPBS(j))
        Dn(i,k,j,l) = Dn(i,k,j,l)/dble(NBSPBS(k))
      enddo
    enddo
  enddo
enddo

do i=2, NDR
  do l=1, NS
    do j=1, NSP
      do k=1, NSP
        Dn(i,j,k,l) = Dn(i-1,j,k,l) + Dn(i,j,k,l)
      enddo
    enddo
  enddo
enddo

do i=1, NSP
  do j=1, NSP
    do k=1, NDR
      do l=1, NS
          Dn_ij(k,i,j) = Dn_ij(k,i,j) + Dn(k,i,j,l)
          Gr_ij(k,i,j) = Gr_ij(k,i,j) + Gij(k,i,j,l)
      enddo
      Gr_ij(k,i,j) = Gr_ij(k,i,j)/dble(NS)
      Dn_ij(k,i,j) = Dn_ij(k,i,j)/dble(NS)
    enddo
  enddo
enddo

do k=1, NDR
  do l=1, NSP
    Gr_ij(k,l,l) = 2.0d0*Gr_ij(k,l,l)
  enddo
  do j=1, NSP-1
    do i=j+1, NSP
      Gr_ij(k,i,j) = Gr_ij(k,i,j) + Gr_ij(k,j,i)
      Gr_ij(k,j,i) = Gr_ij(k,i,j)
    enddo
  enddo
enddo

if (allocated(Dn)) deallocate(Dn)
if (allocated(Gij)) deallocate(Gij)

l = 16
do i=1, NSP
  do j=1, NSP
    do k=1, NDR
      GRTAB(k)=Gr_ij(k,i,j)
    enddo
    call save_curve (NDR, GRTAB, l, IDGR)
    l = l+2
    do k=1, NDR
      Ggr_ij(k,i,j) = (Gr_ij(k,i,j)-1.0)*4.0*PI*(NA/MEANVOL)*(k-0.5)*DTR
      GRTAB(k) = Ggr_ij(k,i,j)
    enddo
    call save_curve (NDR, GRTAB, l, IDGR)
    l = l+2
    do k=1, NDR
      GRTAB(k)=Dn_ij(k,i,j)
    enddo
    call save_curve (NDR, GRTAB, l, IDGR)
    l = l+1
  enddo
enddo

if (NSP .eq. 2) then
  if (GRBT(Gr_ij, NDR)) then
    do k=1, NDR
      GRTAB(k)=BTij(k,1)
    enddo
    call save_curve (NDR, GRTAB, l, IDGR)
    l = l+2
    do k=1, NDR
      GRTAB(k)=BTij(k,2)
    enddo
    call save_curve (NDR, GRTAB, l, IDGR)
    l = l+2
    do k=1, NDR
      GRTAB(k)=BTij(k,3)
    enddo
    call save_curve (NDR, GRTAB, l, IDGR)
  endif
  if (allocated(BTij)) deallocate(BTij)
endif

SUML=0.0d0
XSUML=0.0d0
do k=1, NSP
  SUML=SUML+NSCATTL(k)*Xi(k)
  XSUML=XSUML+XSCATTL(k)*Xi(k)
enddo
SUML=SUML*SUML
XSUML=XSUML*XSUML

! Attention bi(NSCATTL(i) dans le code) en fm et bi² en barn = fm*fm*1e-2 = 1e-24cm²
do i=1, NDR
  do j=1, NSP
    do k=1, NSP
      GrTOT(i) = GrTOT(i) + Xi(j)*Xi(k)*NSCATTL(j)*NSCATTL(k)*Gr_ij(i,j,k)
      GgrTOT(i) = GgrTOT(i) + Xi(j)*Xi(k)*NSCATTL(j)*NSCATTL(k)*Ggr_ij(i,j,k)
      XGrTOT(i) = XGrTOT(i) + Xi(j)*Xi(k)*XSCATTL(j)*XSCATTL(k)*Gr_ij(i,j,k)
      XGgrTOT(i) = XGgrTOT(i) + Xi(j)*Xi(k)*XSCATTL(j)*XSCATTL(k)*Ggr_ij(i,j,k)
    enddo
  enddo
  GrTOT(i) = GrTOT(i)/SUML
  Drn(i) = GgrTOT(i)
  Trn(i) =  Drn(i) + 4.0*PI*(NA/MEANVOL)*(i-0.5)*DTR*SUML
  GgrTOT(i) = GgrTOT(i)/SUML


  XGrTOT(i) = XGrTOT(i)/XSUML
  Drx(i) = XGgrTOT(i)
  Trx(i) =  Drx(i) + 4.0*PI*(NA/MEANVOL)*(i-0.5)*DTR*XSUML
  XGgrTOT(i) = XGgrTOT(i)/XSUML
enddo

do i=1, NDR
  GRTAB(i) = GrTOT(i)
enddo
call save_curve (NDR, GRTAB, 0, IDGR)

do i=1, NDR
  GRTAB(i) = GgrTOT(i)
enddo
call save_curve (NDR, GRTAB, 2, IDGR)

do i=1, NDR
  GRTAB(i) = Drn(i)
enddo
call save_curve (NDR, GRTAB, 4, IDGR)

do i=1, NDR
  GRTAB(i) = Trn(i)
enddo
call save_curve (NDR, GRTAB, 6, IDGR)

do i=1, NDR
  GRTAB(i) = XGrTOT(i)
enddo
call save_curve (NDR, GRTAB, 8, IDGR)

do i=1, NDR
  GRTAB(i) = XGgrTOT(i)
enddo
call save_curve (NDR, GRTAB, 10, IDGR)

do i=1, NDR
  GRTAB(i) = Drx(i)
enddo
call save_curve (NDR, GRTAB, 12, IDGR)

do i=1, NDR
  GRTAB(i) = Trx(i)
enddo
call save_curve (NDR, GRTAB, 14, IDGR)

if (FCR .eq. 1) call FITCUTOFFS

g_of_r=1

001 continue

if (allocated(Dn)) deallocate (Dn)
if (allocated(Gij)) deallocate (Gij)
if (allocated(Dn_ij)) deallocate (Dn_ij)
if (allocated(GRTAB)) deallocate (GRTAB)
if (allocated(GgrTOT)) deallocate (GgrTOT)
if (allocated(XGgrTOT)) deallocate (XGgrTOT)
if (allocated(Trn)) deallocate (Trn)
if (allocated(Trx)) deallocate (Trx)
if (allocated(Drn)) deallocate (Drn)
if (allocated(Drx)) deallocate (Drx)
if (allocated(Ggr_ij)) deallocate (Ggr_ij)
if (allocated(Gr_ij)) deallocate (Gr_ij)
if (allocated(SHELL_VOL)) deallocate(SHELL_VOL)

CONTAINS

SUBROUTINE FITCUTOFFS

INTERFACE
  INTEGER FUNCTION CUTFIT (TABTOFIT, NPOINTS)

    INTEGER, INTENT(IN) :: NPOINTS
    DOUBLE PRECISION, DIMENSION(NPOINTS), INTENT(IN) :: TABTOFIT
  END FUNCTION
END INTERFACE

if (allocated(GFFT)) deallocate(GFFT)
allocate(GFFT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: FITCUTOFFS"//CHAR(0), "Table: GFFT"//CHAR(0))
endif

do l=1, NSP
  do j=1, NSP
    do k=1, NDR
      GFFT(k)=Gr_ij(k,l,j)
    enddo
    Gr_CUT(l,j) = (CUTFIT(GFFT, NDR) - 0.5) * DTR
  enddo
enddo
Gr_cutoff = (CUTFIT(GrTOT, NDR) - 0.5) * DTR

call sendcutoffs(NSP, Gr_cutoff, Gr_CUT)

if (allocated(GFFT)) deallocate(GFFT)

END SUBROUTINE

END FUNCTION

INTEGER FUNCTION CUTFIT (TABTOFIT, NPOINTS)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: NPOINTS
DOUBLE PRECISION, DIMENSION(NPOINTS), INTENT(IN) :: TABTOFIT

o = 1
p = 0
do while (p .eq. 0)
  r=INT(o+1+ANint(5.0*dble(NPOINTS)/dble(100*NS)))
  if (o.eq.NPOINTS .or. r.gt.NPOINTS) exit
  if (TABTOFIT(o) .le. TABTOFIT(o+1)) then
    o=o+1
  else if (TABTOFIT(o) .le. TABTOFIT(r)) then
    o=o+1
  else if (TABTOFIT(o) .le. 0.05) then
    o=o+1
  else
    p=1
  endif
enddo
p = 0
do while (p .eq. 0)
  r=INT(o+1+ANint(5.0*dble(NPOINTS)/dble(100*NS)))
  if (o.eq.NPOINTS .or. r.gt.NPOINTS) exit
  if (TABTOFIT(o) .le. TABTOFIT(o+1)) then
    o=o+1
  else if (TABTOFIT(o) .le. TABTOFIT(r)) then
    o=o+1
  else
    p=1
  endif
enddo

CUTFIT = o

END FUNCTION

LOGICAL FUNCTION ALLOCGR (NDR)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: NDR

ALLOCGR=.true.

if (allocated(SHELL_VOL)) deallocate(SHELL_VOL)
allocate(SHELL_VOL(NDR+1), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: SHELL_VOL"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Gij)) deallocate(Gij)
allocate(Gij(NDR+1,NSP,NSP,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Gij"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Dn)) deallocate(Dn)
allocate(Dn(NDR+1,NSP,NSP,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Dn"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif

Gij(:,:,:,:)=0.0d0
Dn(:,:,:,:)=0.0d0

if (allocated(XGrTOT)) deallocate(XGrTOT)
allocate(XGrTOT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: XGrTOT"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(GrTOT)) deallocate(GrTOT)
allocate(GrTOT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: GrTOT"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Gr_ij)) deallocate(Gr_ij)
allocate(Gr_ij(NDR,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Gr_ij"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Dn_ij)) deallocate(Dn_ij)
allocate(Dn_ij(NDR,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Dn_ij"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(GRTAB)) deallocate(GRTAB)
allocate(GRTAB(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: GRTAB"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Ggr_ij)) deallocate(Ggr_ij)
allocate(Ggr_ij(NDR,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Ggr_ij"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(GgrTOT)) deallocate(GrTOT)
allocate(GgrTOT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: GgrTOT"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(XGgrTOT)) deallocate(XGgrTOT)
allocate(XGgrTOT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: XGgrTOT"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Trn)) deallocate(Trn)
allocate(Trn(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Trn"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Drn)) deallocate(Drn)
allocate(Drn(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Drn"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Trx)) deallocate(Trx)
allocate(Trx(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Trx"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif
if (allocated(Drx)) deallocate(Drx)
allocate(Drx(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: ALLOCGR"//CHAR(0), "Table: Drx"//CHAR(0))
  ALLOCGR=.false.
  goto 001
endif

GrTOT(:)=0.0d0
XGrTOT(:)=0.0d0
GRTAB(:)=0.0d0
GgrTOT(:)=0.0d0
XGgrTOT(:)=0.0d0
Trn(:)=0.0d0
Trx(:)=0.0d0
Drn(:)=0.0d0
Drx(:)=0.0d0
Gr_ij(:,:,:)=0.0d0
Dn_ij(:,:,:)=0.0d0
Ggr_ij(:,:,:)=0.0d0

001 continue

END FUNCTION
