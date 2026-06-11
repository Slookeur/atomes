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
!! @file msd.F90
!! @short MSD analysis
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION MSD (DLT, NDTS) BIND (C,NAME='msd_')

!
! Mean Square Displacement
!

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: NDTS
REAL (KIND=c_double), INTENT(IN) :: DLT
DOUBLE PRECISION :: MASSTOT
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: MSDTAB
#ifdef OPENMP
INTEGER :: NUMTH
#endif

INTERFACE
  LOGICAL FUNCTION ALLOCMSD()
  END FUNCTION
END INTERFACE

! Calcul du déplacement carré moyen
if(.not.TRANSPO()) then
  MSD=0
  goto 001
endif

if (.not. ALLOCMSD()) then
  MSD=0
  goto 001
endif

MASSTOT=0.0d0
do j=1, NSP
  MASSTOT=MASSTOT+NBSPBS(j)*MASS(j)
enddo

#ifdef OPENMP
NUMTH = OMP_GET_MAX_THREADS ()
if (NS.lt.NUMTH) NUMTH=NS

! OpemMP on MD steps
!$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
!$OMP& PRIVATE(RCm, RCm2, R2Cor, Dij, Rij, i, j, k, l, m, n, o, p, Vij) &
!$OMP& SHARED(NUMTH, NS, NA, D2i, D2dir, DRIFT, LOT, MASS, MASSTOT, NDTS, DLT, NFULLPOS)
!$OMP DO SCHEDULE(STATIC,NS-1/NUMTH)
#endif
do j=1, NS-1

  do m=1, 3
    RCm(m)=0.0d0
  enddo

  do i=1, NA

    k=LOT(i)
    do m=1, 3
      RCm(m)=RCm(m)+MASS(k)*NFULLPOS(i,m,j)
    enddo
  enddo

  do m=1, 3
    RCm(m)=RCm(m)/MASSTOT
  enddo

  do k=j+1, NS

    do m=1, 3
      RCm2(m)=0.0d0
    enddo

    do i=1, NA
      n=LOT(i)
      do m=1, 3
        RCm2(m)=RCm2(m)+MASS(n)*NFULLPOS(i,m,k)
      enddo
    enddo

    do m=1, 3
      RCm2(m)=RCm2(m)/MASSTOT
    enddo

    do i=1, NA

      o=LOT(i)
      Dij=0.0d0
      do m=1,3
        Rij(m)=NFULLPOS(i,m,k)-NFULLPOS(i,m,j)
        R2Cor(m) = RCm2(m) - RCm(m)
        Vij = (Rij(m)-R2Cor(m))**2
#ifdef OPENMP
        !$OMP ATOMIC
#endif
        D2dir(o,m,k-j)=D2dir(o,m,k-j)+Vij
        Dij=Dij+Vij
      enddo
#ifdef OPENMP
      !$OMP ATOMIC
#endif
      D2i(o,k-j)=D2i(o,k-j)+Dij

#ifdef OPENMP
      !$OMP CRITICAL
#endif
      p=4
      do m=1, 2
      do n=m+1, 3
        D2dir(o,p,k-j)=D2dir(o,m,k-j)+D2dir(o,n,k-j)
        p=p+1
      enddo
      enddo
#ifdef OPENMP
      !$OMP END CRITICAL
#endif
    enddo

    if (k .eq. j+1) then

      do i=1, NA
        o=LOT(i)
        do m=1,3
          DRIFT(m,k)=DRIFT(m,k)+1e5*(NFULLPOS(i,m,k)-NFULLPOS(i,m,j))*MASS(o)/(NDTS*DLT)
        enddo
      enddo

      do m=1,3
        DRIFT(m,k)=DRIFT(m,k)/MASSTOT
      enddo

    endif

  enddo
enddo
#ifdef OPENMP
!$OMP END DO NOWAIT
!$OMP END PARALLEL
#endif

do k=1, NS-1

  l=k+1
  do m=1,3
    RCm(m)=0.0d0
    RCm2(m)=0.0d0
  enddo

  do i=1, NA
    o=LOT(i)
    do m=1, 3
        RCm(m)=RCm(m)+ MASS(o)*NFULLPOS(i,m,k)
        RCm2(m)=RCm2(m)+ MASS(o)*NFULLPOS(i,m,l)
    enddo
  enddo

  do m=1, 3
    RCm(m)=RCm(m)/MASSTOT
    RCm2(m)=RCm2(m)/MASSTOT
  enddo

  do i=1, NA

    o=LOT(i)
    Dij=0.0d0
    do m=1,3
      R2Cor(m) = RCm2(m) - RCm(m)
      Rij(m)=NFULLPOS(i,m,l)-NFULLPOS(i,m,k)
      D2dirNAC(o,m,k)=(Rij(m)-R2Cor(m))**2
      Dij=Dij+(Rij(m)-R2Cor(m))**2
      COR(m,k)=COR(m,k)+R2Cor(m)
    enddo

    p=4
    do m=1, 2
    do n=m+1, 3
      D2dirNAC(o,p,k)=D2dirNAC(o,m,k)+D2dirNAC(o,n,k)
      p=p+1
    enddo
    enddo
    D2iNAC(o,k)=D2iNAC(o,k)+Dij

  enddo
enddo

do k=1, NS-1
  do i=1, 6
    do l=1, NSP
      D2dir(l,i,k)=D2dir(l,i,k)/(NS-k)/NBSPBS(l)
      D2dirNAC(l,i,k)=D2dirNAC(l,i,k)/NBSPBS(l)
    enddo
  enddo
enddo

do k=1, NS-1
  do i=1, NSP
    D2i(i,k)=D2i(i,k)/(NS-k)/NBSPBS(i)
    D2iNAC(i,k)=D2iNAC(i,k)/NBSPBS(i)
  enddo
enddo

! Evaluation of the diffusion constant D
! A minimum of dynamic time is needed for a 'physical' meaning of D
! so if the simulation time is big enough we compute D
! Here arbitrary we choose 30ps as time limit (30000 fs)
! We also decide to evaluate only if there is more than 1000 MD steps
if (NS*NDTS*DLT .gt. 30000 .and. NS.gt.1000) then
! then take car to cut the first 1 ps of the simulation
! not to be anymore in the ballistic regime
! afterwards take care to cut the last 1 ps of the calculation
! usually the correlations are unclear in this zone of the MSD
  z=NS*NDTS*DLT
  z=z-1.0
  k=INT(z/(NDTS*DLT))
  l=INT(1.0/(NDTS*DLT))
  do i=1, NSP
    Dcte(i)=(D2i(i,k)-D2i(i,l))/((k-l)*NDTS*DLT)
  enddo

else

  do i=1, NSP
    Dcte(i)=0.d0
  enddo

endif

allocate(MSDTAB(NS-1), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: MSD"//CHAR(0), "Table: MSDTAB"//CHAR(0))
  MSD=0
  goto 001
endif

k=0
do i=1, NSP
  do j=1, NS-1
    MSDTAB(j)=D2i(i,j)
  enddo
  call save_curve (NS-1, MSDTAB, k, IDMSD)
  k=k+1
  do j=1, NS-1
    MSDTAB(j)=D2iNAC(i,j)
  enddo
  call save_curve (NS-1, MSDTAB, k, IDMSD)
  k=k+1
enddo

do i=1, NSP
  do l=1, 6
    do j=1, NS-1
      MSDTAB(j)=D2dir(i,l,j)
    enddo
    call save_curve (NS-1, MSDTAB, k, IDMSD)
    k=k+1
  enddo
enddo

do i=1, NSP
  do l=1, 6
    do j=1, NS-1
      MSDTAB(j)=D2dirNAC(i,l,j)
    enddo
    call save_curve (NS-1, MSDTAB, k, IDMSD)
    k=k+1
  enddo
enddo

do i=1, 3
  do j=1, NS-1
    MSDTAB(j)=COR(i,j)
  enddo
  call save_curve (NS-1, MSDTAB, k, IDMSD)
  k=k+1
enddo
do i=1, 3
  do j=1, NS-1
    MSDTAB(j)=DRIFT(i,j)
  enddo
  call save_curve (NS-1, MSDTAB, k, IDMSD)
  k=k+1
enddo

MSD=1

001 continue

if (allocated(NFULLPOS)) deallocate(NFULLPOS)

call DEALLOCMSD

CONTAINS

LOGICAL FUNCTION TRANSPO()

USE PARAMETERS

if (allocated(NFULLPOS)) deallocate(NFULLPOS)
allocate(NFULLPOS(NA,3,NS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: TRANSPO"//CHAR(0), "Table: NFULLPOS"//CHAR(0))
  TRANSPO=.false.
  goto 001
endif

if (allocated(POA)) deallocate(POA)
allocate(POA(NA,3), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: TRANSPO"//CHAR(0), "Table: POA"//CHAR(0))
  TRANSPO=.false.
  goto 001
endif

if (allocated(POB)) deallocate(POB)
allocate(POB(NA,3), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: TRANSPO"//CHAR(0), "Table: POB"//CHAR(0))
  TRANSPO=.false.
  goto 001
endif

do NOA=1, NS
do NOB=1, NA
do NOC=1, 3
  NFULLPOS(NOB,NOC,NOA) = FULLPOS(NOB,NOC,NOA)
enddo
enddo
enddo

do NOA=1, NA
  do NOB=1, 3
    POA(NOA,NOB)=NFULLPOS(NOA,NOB,1)
  enddo
enddo

do NOC=2, NS
  do NOA=1, NA
    do NOB=1, 3
      POB(NOA,NOB)=NFULLPOS(NOA,NOB,NOC)
    enddo
    if (NCELLS .gt. 1) then
      call CALCRIJ(NOA, NOA, -1, -1, NOC-1)
    else
      call CALCRIJ(NOA, NOA, -1, -1, 1)
    endif
    do NOB=1, 3
      POA(NOA,NOB)=POA(NOA,NOB)+Rij(NOB)
      NFULLPOS(NOA,NOB,NOC)=POA(NOA,NOB)
    enddo
  enddo
enddo

TRANSPO = .true.

001 continue

if (allocated(POA)) deallocate(POA)
if (allocated(POB)) deallocate(POB)

END FUNCTION

END FUNCTION
