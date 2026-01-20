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
!! @file utils.F90
!! @short Global function declarations
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

!
! This file contains various tools
!

!********************************************************************
!
! Transformer un entier en chaine de charactères
! Transform an integer into character
!

SUBROUTINE CHARINT(Word, Num)

IMPLICIT NONE

INTEGER :: lc

INTEGER, INTENT(IN) :: Num

CHARACTER (LEN=16) :: Pseudo

CHARACTER (LEN=15), INTENT(INOUT) :: Word


Word=' '
write (Word, *) Num
lc=1
do while (Word(1:lc) .eq. ' ')

  lc= lc+1

enddo

Pseudo=Word
Word=' '

write (Word, *) Pseudo(lc:LEN_TRIM(Pseudo))

END SUBROUTINE

SUBROUTINE CREAT_RING (RING_INIT, ELEM_CR)

USE PARAMETERS

IMPLICIT NONE

TYPE (RING), INTENT(INOUT) :: RING_INIT
INTEGER, INTENT(IN) :: ELEM_CR

RING_INIT%SPEC = 0
RING_INIT%ATOM = ELEM_CR
nullify(RING_INIT%PAST)
nullify(RING_INIT%NEXT)

END SUBROUTINE

SUBROUTINE DO_RING (THE_RING, ELEM_DO)

USE PARAMETERS

IMPLICIT NONE

TYPE (RING), POINTER, INTENT(INOUT) :: THE_RING
!INTEGER, DIMENSION(NA), INTENT(INOUT) :: RPT
INTEGER, INTENT(IN) :: ELEM_DO
TYPE (RING), POINTER :: NEW
allocate(NEW, STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: DO_RING"//CHAR(0), "Pointer: NEW"//CHAR(0))
  ALC=.true.
  goto 001
endif
nullify(NEW%NEXT)
nullify(NEW%PAST)
THE_RING%NEXT => NEW
NEW%PAST => THE_RING
NEW%ATOM = ELEM_DO
NEW%SPEC = THE_RING%SPEC+1
THE_RING => NEW
!if (RPT(THE_RING%ATOM) .eq. 0) then
!  RPT(THE_RING%ATOM) = 1
!endif

001 continue

END SUBROUTINE

!********************************************************************
!
! Calculer une distance
! Compute a distance including PBC
!

SUBROUTINE CALCRIJ(AT1, AT2, STEP_1, STEP_2, SID)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
INTEGER :: R1
DOUBLE PRECISION, DIMENSION(3) :: COORDA, COORDB
DOUBLE PRECISION, DIMENSION(3) :: Aij, Bij, Nij

if (STEP_1 .eq. -2) then

  do R1=1, 3
    COORDA(R1) = NFULLPOS(AT1,R1,STEP_2)
    COORDB(R1) = NFULLPOS(AT2,R1,STEP_2)
  enddo

elseif (STEP_1 .eq. -1) then

! Prepos for MSD
  do R1=1, 3
    COORDA(R1) = POB(AT2,R1)
    COORDB(R1) = POA(AT1,R1)
  enddo

else
! Standard inter-atomic distance
  do R1=1, 3
    COORDA(R1) = FULLPOS(AT1,R1,STEP_1)
    COORDB(R1) = FULLPOS(AT2,R1,STEP_2)
  enddo

endif

do R1=1, 3
  Rij(R1) = COORDA(R1) - COORDB(R1)
enddo

if (.not.PBC) goto 001

! Test if the simulation box is cubic-like symmetric
! For that we check the lattice angles
if (THE_BOX(SID)%GLASS) then

  do R1=1, 3
    Rij(R1)=Rij(R1) - ANint(Rij(R1)/(THE_BOX(SID)%modv(R1)))*(THE_BOX(SID)%modv(R1))
  enddo

else

  Aij = MATMUL(COORDA,THE_BOX(SID)%carttofrac)
  Bij = MATMUL(COORDB,THE_BOX(SID)%carttofrac)
  do R1=1,3
    Nij(R1) = Aij(R1) - Bij(R1)
    Nij(R1) = Nij(R1) - AnINT(Nij(R1))
  enddo
  Rij = MATMUL(Nij,THE_BOX(SID)%fractocart)

endif

001 continue

Dij=0.0d0
do R1=1, 3
  Dij=Dij+Rij(R1)**2
enddo

END SUBROUTINE

DOUBLE PRECISION FUNCTION  CALCDIJ(R12, AT1, AT2, STEP_1, STEP_2, SID)

USE PARAMETERS

IMPLICIT NONE

DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: R12
INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
INTEGER :: R1
DOUBLE PRECISION, DIMENSION(3) :: COORDA, COORDB
DOUBLE PRECISION, DIMENSION(3) :: Aij, Bij, Nij

if (STEP_1 .eq. -2) then

  do R1=1, 3
    COORDA(R1) = NFULLPOS(AT1,R1,STEP_2)
    COORDB(R1) = NFULLPOS(AT2,R1,STEP_2)
  enddo

elseif (STEP_1 .eq. -1) then

! Prepos for MSD
  do R1=1, 3
    COORDA(R1) = POB(AT2,R1)
    COORDB(R1) = POA(AT1,R1)
  enddo

else
! Standard inter-atomic distance
  do R1=1, 3
    COORDA(R1) = FULLPOS(AT1,R1,STEP_1)
    COORDB(R1) = FULLPOS(AT2,R1,STEP_2)
  enddo

endif

do R1=1, 3
  R12(R1) = COORDA(R1) - COORDB(R1)
enddo

if (.not.PBC) goto 001

! Test if the simulation box is cubic-like symetric
! For that we check the lattice angles
if (THE_BOX(SID)%GLASS) then

  do R1=1, 3
    R12(R1)=R12(R1) - ANint(R12(R1)/(THE_BOX(SID)%modv(R1)))*(THE_BOX(SID)%modv(R1))
  enddo

else

  Aij = MATMUL(COORDA,THE_BOX(SID)%carttofrac)
  Bij = MATMUL(COORDB,THE_BOX(SID)%carttofrac)
  do R1=1,3
    Nij(R1) = Aij(R1) - Bij(R1)
    Nij(R1) = Nij(R1) - AnINT(Nij(R1))
  enddo
  R12 = MATMUL(Nij,THE_BOX(SID)%fractocart)

endif

001 continue

CALCDIJ = 0.0d0
do R1=1, 3
  CALCDIJ = CALCDIJ + R12(R1)**2
enddo

END FUNCTION

!********************************************************************
!
! Home made arcosine calculation
!

DOUBLE PRECISION FUNCTION SACOS (ANG)

USE PARAMETERS

DOUBLE PRECISION, INTENT(IN) :: ANG
DOUBLE PRECISION :: SCOS

if (ANG .lt. -1.0d0) then
  SCOS = -2.0d0 - ANG
else if (ANG .gt. 1.0d0) then
  SCOS = 2.0d0 - ANG
else
  SCOS = ANG
endif
SACOS = DACOS(SCOS)*180.d0/PI

END FUNCTION

!********************************************************************
!
! Calculer un angle direct
! Compute a direct angle
!

DOUBLE PRECISION FUNCTION ANGIJK(ATG1, ATG2, ATG3, ASTEP)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: ATG1, ATG2, ATG3, ASTEP
INTEGER :: AG1, AG2
DOUBLE PRECISION :: DAA, DBB, VANG
DOUBLE PRECISION, DIMENSION(3) :: RAA, RBB

INTERFACE
  DOUBLE PRECISION FUNCTION  CALCDIJ(R12, AT1, AT2, STEP_1, STEP_2, SID)
    DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: R12
    INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
  END FUNCTION
  DOUBLE PRECISION FUNCTION SACOS (ANG)
    DOUBLE PRECISION, INTENT(IN) :: ANG
  END FUNCTION
END INTERFACE

if (NCELLS .gt. 1) then
  AG2 = ASTEP
else
  AG2 = 1
endif

DAA = CALCDIJ(RAA, ATG2,ATG1,ASTEP,ASTEP,AG2)
DAA=sqrt(DAA)
DBB = CALCDIJ(RBB, ATG2,ATG3,ASTEP,ASTEP,AG2)
VANG=0.0d0
do AG1=1, 3
  VANG=VANG+RAA(AG1)*RBB(AG1)
enddo
DBB=sqrt(DBB)

ANGIJK = SACOS(VANG/(DAA*DBB))

END FUNCTION

!********************************************************************
!
! Calculer un angle dièdre
! Compute a dihedral angle
!

DOUBLE PRECISION FUNCTION DIEDRE(DG1, DG2, DG3, DG4, DSTEP)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: DG1, DG2, DG3, DG4, DSTEP
INTEGER :: AG, AH
DOUBLE PRECISION :: DH1, DH2, DH3, VDI
DOUBLE PRECISION, DIMENSION(3) :: D12, D23, D34
DOUBLE PRECISION, DIMENSION(3) :: VH1, VH2

INTERFACE
  DOUBLE PRECISION FUNCTION  CALCDIJ(R12, AT1, AT2, STEP_1, STEP_2, SID)
    DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: R12
    INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
  END FUNCTION
  DOUBLE PRECISION FUNCTION SACOS (ANG)
    DOUBLE PRECISION, INTENT(IN) :: ANG
  END FUNCTION
END INTERFACE

if (NCELLS .gt. 1) then
  AH = DSTEP
else
  AH = 1
endif

DH1 = CALCDIJ(D12,DG1,DG2,DSTEP,DSTEP,AH)
DH2 = CALCDIJ(D23,DG2,DG3,DSTEP,DSTEP,AH)
DH3 = CALCDIJ(D34,DG3,DG4,DSTEP,DSTEP,AH)

VH1(1) = D12(2)*D23(3) - D12(3)*D23(2)
VH1(2) = D12(3)*D23(1) - D12(1)*D23(3)
VH1(3) = D12(1)*D23(2) - D12(2)*D23(1)

VH2(1) = D23(2)*D34(3) - D23(3)*D34(2)
VH2(2) = D23(3)*D34(1) - D23(1)*D34(3)
VH2(3) = D23(1)*D34(2) - D23(2)*D34(1)

DH1=0.0d0
DH2=0.0d0
do AG=1, 3
  DH1 = DH1 + VH1(AG)*VH1(AG)
  DH2 = DH2 + VH2(AG)*VH2(AG)
enddo
DH1=sqrt(DH1)
DH2=sqrt(DH2)
if (DH1.eq.0.0d0 .or. DH2.eq.0.0d0) then
  DIEDRE = 0.0d0
else
  VDI = 0.0d0
  do AG=1, 3
    VDI=VDI+VH1(AG)*VH2(AG)
  enddo
  VDI = VDI/(DH1*DH2)
  DIEDRE = SACOS (VDI)
endif

END FUNCTION

! Cas particulier lors des statistiques d'anneaux

SUBROUTINE ECT_TYPE_RINGS(MOYENNE, TABLEAU, LONGTE, LREPRES, EC_TYPE)

INTEGER :: pose
INTEGER, INTENT(IN) :: LONGTE, LREPRES

DOUBLE PRECISION, INTENT(IN) :: MOYENNE
DOUBLE PRECISION, INTENT(IN), DIMENSION(LONGTE) :: TABLEAU
DOUBLE PRECISION, INTENT(INOUT) :: EC_TYPE

EC_TYPE=0.0d0

do pose=1, LONGTE

  if (TABLEAU(pose) .ne. 0.0) EC_TYPE= EC_TYPE + (TABLEAU(pose) - MOYENNE)**2

enddo

EC_TYPE=sqrt(EC_TYPE/dble(LREPRES-1))

END SUBROUTINE

!********************************************************************
!
! Générateur de nombre aléatoire
! Numerical recipes random number generator 3
!

DOUBLE PRECISION FUNCTION RAN3 (idnum)

INTEGER, INTENT(IN) :: idnum
INTEGER :: idum
INTEGER :: MBIG,MSEED,MZ
INTEGER ::  iii,iff,ii,inext,inextp,k
INTEGER :: mj,mk,ma(55)

DOUBLE PRECISION :: FAC

PARAMETER (MBIG=1000000000,MSEED=161803398,MZ=0,FAC=1./MBIG)
SAVE  iff,inext,inextp,ma
DATA  iff /0/

idum = idnum
if(idum.lt.0 .or. iff.eq.0)then

  iff=1
  mj=MSEED-iabs(idum)
  mj=mod(mj,MBIG)
  ma(55)=mj
  mk=1
  do 11 iii=1,54

    ii=mod(21*iii,55)
    ma(ii)=mk
    mk=mj-mk
    if(mk.lt.MZ)mk=mk+MBIG
    mj=ma(ii)

11  continue

  do 13 k=1,4

    do 12 iii=1,55

      ma(iii)=ma(iii)-ma(1+mod(iii+30,55))
      if(ma(iii).lt.MZ)ma(iii)=ma(iii)+MBIG

12    continue

13  continue
  inext=0
  inextp=31
  idum=1

endif

inext=inext+1
if(inext.eq.56)inext=1
inextp=inextp+1
if(inextp.eq.56)inextp=1
mj=ma(inext)-ma(inextp)
if(mj.lt.MZ)mj=mj+MBIG
ma(inext)=mj
RAN3=mj*FAC

END FUNCTION

REAL (KIND=c_double) FUNCTION random3(seed) BIND (C,NAME='random3_')

USE, INTRINSIC :: iso_c_binding

INTEGER (KIND=c_int), INTENT(IN) :: seed

INTERFACE
  DOUBLE PRECISION FUNCTION RAN3 (idnum)
    INTEGER, INTENT(IN) :: idnum
  END FUNCTION
END INTERFACE

random3 = RAN3(seed)

END FUNCTION

!********************************************************************
!
! Calculer la somme d'une série de valeurs
! Compute sum of a list of values
!

SUBROUTINE SOMME(TABS, LONGTS, SOMTAB)

INTEGER :: posm
INTEGER, INTENT(IN) :: LONGTS

DOUBLE PRECISION, INTENT(INOUT) :: SOMTAB
DOUBLE PRECISION, INTENT(IN), DIMENSION(LONGTS) :: TABS

SOMTAB=0.0d0

do posm=1, LONGTS

SOMTAB=SOMTAB+TABS(posm)

enddo

END SUBROUTINE

!********************************************************************
!
! Calculer la moyenne d'une série de valeurs
! Compute the average of list of values
!

SUBROUTINE MOYENNE(TABLEAU, LONGTM, MOYTAB)

INTEGER :: posm
INTEGER, INTENT(IN) :: LONGTM

DOUBLE PRECISION, INTENT(INOUT) :: MOYTAB
DOUBLE PRECISION, INTENT(IN), DIMENSION(LONGTM) :: TABLEAU

MOYTAB=0.0d0

do posm=1, LONGTM

MOYTAB=MOYTAB+TABLEAU(posm)

enddo

MOYTAB=MOYTAB/dble(LONGTM)

END SUBROUTINE

!********************************************************************
!
! Calculer l'écart type sur une série de valeurs
! Compute the standard error on a list of values
!

SUBROUTINE ECT_TYPE(MOYENNE, TABLEAU, LONGTE, EC_TYPE)

INTEGER :: pose
INTEGER, INTENT(IN) :: LONGTE

DOUBLE PRECISION, INTENT(IN) :: MOYENNE
DOUBLE PRECISION, INTENT(IN), DIMENSION(LONGTE) :: TABLEAU
DOUBLE PRECISION, INTENT(INOUT) :: EC_TYPE

EC_TYPE=0.0d0

do pose=1, LONGTE

EC_TYPE= EC_TYPE + (TABLEAU(pose) - MOYENNE)**2

enddo

EC_TYPE=sqrt(EC_TYPE/dble(LONGTE-1))

END SUBROUTINE

!********************************************************************
!
! Trier une liste d'entiers
! Sort a list of integers
! This is the most simple sort
! For an improved routine (huge data to sort) see SORT3 in this file
!

SUBROUTINE TRI(TAB, DIMTAB)

IMPLICIT NONE

INTEGER, INTENT(IN) :: DIMTAB
INTEGER, DIMENSION(DIMTAB), INTENT(INOUT) :: TAB
INTEGER :: SORTX, SORTY, VALUE

do SORTX=2, DIMTAB

  VALUE=TAB(SORTX)

  do SORTY=SORTX-1, 1, -1

    if (TAB(SORTY) .le. VALUE) exit

    TAB(SORTY+1)=TAB(SORTY)

  enddo

  TAB(SORTY+1)=VALUE

enddo

END SUBROUTINE

!********************************************************************
!
! Lisser une courbe / To smooth a curve
!

LOGICAL FUNCTION SMOOTH (TABTOLISS, GTOLISS, DIMTOLISS, SIGMALISS)

! Lissage selon une gaussienne
! Gaussian smoothing - zero based curve

IMPLICIT NONE

INTEGER, INTENT(IN) :: DIMTOLISS
DOUBLE PRECISION, INTENT(IN) :: SIGMALISS
DOUBLE PRECISION, INTENT(IN), DIMENSION(DIMTOLISS) :: GTOLISS
DOUBLE PRECISION, INTENT(INOUT), DIMENSION(DIMTOLISS) :: TABTOLISS

INTEGER :: INDA, INDB, ERR
DOUBLE PRECISION, PARAMETER :: PI=acos(-1.0)
DOUBLE PRECISION :: DQ, DQ2
DOUBLE PRECISION :: FACTLISS

DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: NEWTAB

allocate(NEWTAB(DIMTOLISS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: SMOOTH"//CHAR(0), "Table: NEWTAB"//CHAR(0))
  SMOOTH=.false.
  goto 001
endif

do INDA=1, DIMTOLISS
  NEWTAB(INDA)=0.0d0
enddo

FACTLISS=1.0d0/(SIGMALISS*sqrt(2.0d0*PI))

do INDA=1, DIMTOLISS
  do INDB=1, DIMTOLISS

    if(INDB .eq. 1)then
      DQ= GTOLISS(2)-GTOLISS(1)
    elseif (INDB .eq. DIMTOLISS) then
      DQ = GTOLISS(DIMTOLISS)- GTOLISS(DIMTOLISS-1)
    else
      DQ = (GTOLISS(INDB+1)- GTOLISS(INDB-1))*0.5
    endif

    DQ2=(GTOLISS(INDA)-GTOLISS(INDB))*(GTOLISS(INDA)-GTOLISS(INDB))
    NEWTAB(INDA) = NEWTAB(INDA) + exp(-DQ2/(2.0d0*SIGMALISS*SIGMALISS))*TABTOLISS(INDB)*DQ

  enddo
  NEWTAB(INDA)=FACTLISS*NEWTAB(INDA)
enddo

do INDA=1, DIMTOLISS
  TABTOLISS(INDA)=NEWTAB(INDA)
enddo

SMOOTH=.true.

001 continue

if (allocated(NEWTAB)) deallocate(NEWTAB)

END FUNCTION

!********************************************************************
!
! Sort routine adapted from:
! Numerical sort array 3 - Quicksort INTEGER
!

SUBROUTINE SORT3(arr, ndum)

INTEGER, INTENT(IN) :: ndum
INTEGER, DIMENSION(ndum) :: index

INTEGER, DIMENSION(ndum), INTENT(INOUT) :: arr

call indexx_i(arr, index, ndum)

arr=arr(index)

END SUBROUTINE

SUBROUTINE indexx_i(arr, index, n)

  IMPLICIT NONE
  INTEGER, PARAMETER :: NPAR_ARTH=16,NPAR2_ARTH=8
  INTEGER, DIMENSION(n), INTENT(INOUT) :: index
  INTEGER, PARAMETER :: NN=15, NSTACK=50
  INTEGER, INTENT(IN) :: n
  INTEGER :: k,i,j,indext,jstack,l,r
  INTEGER, DIMENSION(NSTACK) :: istack
  INTEGER :: a
  INTEGER, DIMENSION(n), INTENT(IN) :: arr

  call arth(1,1,n,index)
  jstack=0
  l=1
  r=n
  do
    if (r-l < NN) then
      do j=l+1,r
        indext=index(j)
        a=arr(indext)
        do i=j-1,l,-1
          if (arr(index(i)) <= a) exit
          index(i+1)=index(i)
        end do
        index(i+1)=indext
      end do
      if (jstack .eq. 0) RETURN
      r=istack(jstack)
      l=istack(jstack-1)
      jstack=jstack-2
    else
      k=(l+r)/2
      call swap(index(k),index(l+1))
      call icomp_xchg(index(l),index(r))
      call icomp_xchg(index(l+1),index(r))
      call icomp_xchg(index(l),index(l+1))
      i=l+1
      j=r
      indext=index(l+1)
      a=arr(indext)
      do
        do
          i=i+1
          if (arr(index(i)) >= a) exit
        end do
        do
          j=j-1
          if (arr(index(j)) <= a) exit
        end do
        if (j < i) exit
        call swap(index(i),index(j))
      end do
      index(l+1)=index(j)
      index(j)=indext
      jstack=jstack+2
      if (jstack > NSTACK) write (6, *) 'Sort3 error: indexx: NSTACK too small'
      if (r-i+1 >= j-l) then
        istack(jstack)=r
        istack(jstack-1)=i
        r=j-1
      else
        istack(jstack)=j-1
        istack(jstack-1)=l
        l=i
      end if
    end if
  end do
  CONTAINS
!BL
  SUBROUTINE icomp_xchg(i, j)
  INTEGER, INTENT(INOUT) :: i,j
  INTEGER :: swp
  if (arr(j) < arr(i)) then
    swp=i
    i=j
    j=swp
  end if
  END SUBROUTINE icomp_xchg

  SUBROUTINE arth(first, increment, m, arth_i)
  INTEGER, INTENT(IN) :: first,increment,m
  INTEGER, DIMENSION(m) :: arth_i
  INTEGER :: k,k2,temp

  if (m > 0) arth_i(1)=first
  if (m <= NPAR_ARTH) then
    do k=2,m
      arth_i(k)=arth_i(k-1)+increment
    end do
  else
    do k=2,NPAR2_ARTH
      arth_i(k)=arth_i(k-1)+increment
    end do
    temp=increment*NPAR2_ARTH
    k=NPAR2_ARTH
    do
      if (k >= m) exit
      k2=k+k
      arth_i(k+1:min(k2,m))=temp+arth_i(1:min(k,m-k))
      temp=temp+temp
      k=k2
    end do
  end if
  END SUBROUTINE

  SUBROUTINE swap(a,b)
  INTEGER, INTENT(INOUT) :: a,b
  INTEGER :: dum
  dum=a
  a=b
  b=dum
  END SUBROUTINE swap

END SUBROUTINE indexx_i
!********************************************************************

!********************************************************************
!
! Sort routine adapted from:
! Numerical sort array 3 - Quicksort DOUBLE PRECISION
!

SUBROUTINE SORT3_DP (arr, ndum)

INTEGER, INTENT(IN) :: ndum
INTEGER, DIMENSION(ndum) :: index

DOUBLE PRECISION, DIMENSION(ndum), INTENT(INOUT) :: arr

call indexx_dp (arr, index, ndum)

arr=arr(index)

END SUBROUTINE

SUBROUTINE indexx_dp (arr, index, n)

  IMPLICIT NONE
  INTEGER, PARAMETER :: NPAR_ARTH=16,NPAR2_ARTH=8
  INTEGER, DIMENSION(n), INTENT(INOUT) :: index
  INTEGER, PARAMETER :: NN=15, NSTACK=50
  INTEGER, INTENT(IN) :: n
  INTEGER :: k,i,j,indext,jstack,l,r
  INTEGER, DIMENSION(NSTACK) :: istack
  DOUBLE PRECISION :: a
  DOUBLE PRECISION, DIMENSION(n), INTENT(IN) :: arr

  call arth(1,1,n,index)
  jstack=0
  l=1
  r=n
  do
    if (r-l < NN) then
      do j=l+1,r
        indext=index(j)
        a=arr(indext)
        do i=j-1,l,-1
          if (arr(index(i)) <= a) exit
          index(i+1)=index(i)
        end do
        index(i+1)=indext
      end do
      if (jstack .eq. 0) RETURN
      r=istack(jstack)
      l=istack(jstack-1)
      jstack=jstack-2
    else
      k=(l+r)/2
      call swap(index(k),index(l+1))
      call icomp_xchg(index(l),index(r))
      call icomp_xchg(index(l+1),index(r))
      call icomp_xchg(index(l),index(l+1))
      i=l+1
      j=r
      indext=index(l+1)
      a=arr(indext)
      do
        do
          i=i+1
          if (arr(index(i)) >= a) exit
        end do
        do
          j=j-1
          if (arr(index(j)) <= a) exit
        end do
        if (j < i) exit
        call swap(index(i),index(j))
      end do
      index(l+1)=index(j)
      index(j)=indext
      jstack=jstack+2
      if (jstack > NSTACK) write (6, *) 'Sort3 error: indexx: NSTACK too small'
      if (r-i+1 >= j-l) then
        istack(jstack)=r
        istack(jstack-1)=i
        r=j-1
      else
        istack(jstack)=j-1
        istack(jstack-1)=l
        l=i
      end if
    end if
  end do
  CONTAINS
!BL
  SUBROUTINE icomp_xchg(i, j)
  INTEGER, INTENT(INOUT) :: i,j
  INTEGER :: swp
  if (arr(j) < arr(i)) then
    swp=i
    i=j
    j=swp
  end if
  END SUBROUTINE icomp_xchg

  SUBROUTINE arth(first, increment, m, arth_i)
  INTEGER, INTENT(IN) :: first,increment,m
  INTEGER, DIMENSION(m) :: arth_i
  INTEGER :: k,k2,temp

  if (m > 0) arth_i(1)=first
  if (m <= NPAR_ARTH) then
    do k=2,m
      arth_i(k)=arth_i(k-1)+increment
    end do
  else
    do k=2,NPAR2_ARTH
      arth_i(k)=arth_i(k-1)+increment
    end do
    temp=increment*NPAR2_ARTH
    k=NPAR2_ARTH
    do
      if (k >= m) exit
      k2=k+k
      arth_i(k+1:min(k2,m))=temp+arth_i(1:min(k,m-k))
      temp=temp+temp
      k=k2
    end do
  end if
  END SUBROUTINE

  SUBROUTINE swap(a,b)
  INTEGER, INTENT(INOUT) :: a,b
  INTEGER :: dum
  dum=a
  a=b
  b=dum
  END SUBROUTINE swap

END SUBROUTINE indexx_dp
!********************************************************************
