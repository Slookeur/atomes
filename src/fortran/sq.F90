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
!! @file sq.F90
!! @short S(q) analysis: Fourier transform calculation
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION s_of_q (QMAX, QMIN, NQ) BIND (C,NAME='s_of_q_')

! Total structure factor
! Partials structure factors

! Total structure factor:

!
!                         00        sin (q*r)
!    S(q)= 1 + 4*PI*rho* /   dr r² ------------ (g(r) - 1)
!                       0              (q*r)
!

! or:

!             1          Rmax                 sin (q*r)    sin (PI*r/Rmax)
!    S(q)= 1 + 4*PI*rho* /   dr r² (g(r) -1) ----------- * ---------------
!                       0                       (q*r)         PI*r/Rmax
!

! Partials structure factors:

!
!                                                       Rmax
!    S  (q)=     delta(a,b) + sqrt(x * x ) *4*PI*rho* /   dr r² (g  (r) - 1)
!     ab                            a   b            0            ab
!
!

! or:

!                                                    Rmax                      sin (q*r)    sin (PI*r/Rmax)
!    S  (q)=  delta(a,b) + sqrt(x * x ) *4*PI*rho*  /   dr r² (g  (r) - go  ) ----------- * ---------------
!     ab                         a   b             0             ab       ab     (q*r)         PI*r/Rmax
!

! with x = Nbre(a)/Nbre(tot)
!       a


USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: NQ
REAL (KIND=c_double), INTENT(IN) :: QMAX , QMIN
DOUBLE PRECISION :: DQ
DOUBLE PRECISION, DIMENSION (:), ALLOCATABLE :: SQTAB

INTERFACE
  INTEGER FUNCTION recup_data (i, j)
    INTEGER, INTENT(IN) :: i, j
  END FUNCTION
  LOGICAL FUNCTION FZBT (NDQ, SQIJ)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: NDQ
    DOUBLE PRECISION, DIMENSION(NDQ,NSP,NSP), INTENT(IN) :: SQIJ
  END FUNCTION
END INTERFACE

NUMBER_OF_QMOD = NQ
TOTAL_DENSITY = dble(NA)/MEANVOL

if (allocated(Q_POINT)) deallocate(Q_POINT)
allocate(Q_POINT(NQ), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_q"//CHAR(0), "Table: Q_POINT"//CHAR(0))
  s_of_q = 0
  goto 001
endif
if (allocated(S)) deallocate(S)
allocate(S(NQ), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_q"//CHAR(0), "Table: S"//CHAR(0))
  s_of_q = 0
  goto 001
endif
if (allocated(XS)) deallocate(XS)
allocate(XS(NQ), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_q"//CHAR(0), "Table: XS"//CHAR(0))
  s_of_q = 0
  goto 001
endif
if (allocated(Sij)) deallocate(Sij)
allocate(Sij(NQ,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_q"//CHAR(0), "Table: Sij"//CHAR(0))
  s_of_q = 0
  goto 001
endif

! 'QMIN' is minimum q modulus accessible considering the analysed
! lattice ie. the minimum modulus of the reciprocal cell vectors,
DQ=((QMAX-QMIN)/dble(NQ))

S(:)=0.0d0
XS(:)=0.0d0
Q_POINT(:)=0.0d0
do i=1, NQ
  Q_POINT(i)= dble(i-1)*DQ+QMIN
enddo

Sij(:,:,:)=0.0d0

j=0
if (recup_data (j, IDGR) .ne. 1) then
  s_of_q = 0
  goto 001
endif

j=j+8
if (recup_data (j, IDGR) .ne. 1) then
  s_of_q = 0
  goto 001
endif

j=j+8
do o=1, NSP
do p=1, NSP
  if (recup_data (j, IDGR) .ne. 1) then
    s_of_q = 0
    goto 001
  endif
  j=j+5
enddo
enddo

if (.not. FZBT (NQ, Sij)) then
  s_of_q = 0
  goto 001
endif

if (allocated(SQTAB)) deallocate(SQTAB)
allocate(SQTAB(NQ), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: s_of_q"//CHAR(0), "Table: SQTAB"//CHAR(0))
  s_of_q = 0
  goto 001
endif

l=0
do k=1, NQ
  SQTAB(k)= S(k)
enddo
call save_curve (NQ, SQTAB, l, IDSQ)
l=l+2

do k=1, NQ
  SQTAB(k)= (S(k)-1.0)*Q_POINT(k)
enddo
call save_curve (NQ, SQTAB, l, IDSQ)
l=l+2

do k=1, NQ
  SQTAB(k)= XS(k)
enddo
call save_curve (NQ, SQTAB, l, IDSQ)
l=l+2

do k=1, NQ
  SQTAB(k)= (XS(k)-1.0)*Q_POINT(k)
enddo
call save_curve (NQ, SQTAB, l, IDSQ)
l=l+2

do i=1, NSP
do j=1, NSP
  do k=1, NQ
    SQTAB(k)= Sij(k,i,j)
  enddo
  call save_curve (NQ, SQTAB, l, IDSQ)
  l=l+2
enddo
enddo
do i=1, NSP
do j=1, NSP
  do k=1, NQ
    SQTAB(k)= FZSij(k,i,j)
  enddo
  call save_curve (NQ, SQTAB, l, IDSQ)
  l=l+2
enddo
enddo
if (NSP .eq. 2) then
  do i=1, 4
    do j=1, NQ
      SQTAB(j)= BTij(j,i)
    enddo
    call save_curve (NQ, SQTAB, l, IDSQ)
    l=l+2
  enddo
endif

s_of_q = 1

001 continue

if (allocated(SQTAB)) deallocate(SQTAB)
if (allocated(Q_POINT)) deallocate(Q_POINT)
if (allocated(Sij)) deallocate(Sij)
if (allocated(S)) deallocate(S)
if (allocated(XS)) deallocate(XS)
if (allocated(FZSij)) deallocate(FZSij)
if (allocated(BTij)) deallocate(BTij)

END FUNCTION

INTEGER (KIND=c_int) FUNCTION send_gr (IC, VAL, DR, RDATA, GDATA) BIND (C,NAME='send_gr_')

USE PARAMETERS

INTEGER (KIND=c_int), INTENT(IN) :: IC, VAL
REAL (KIND=c_double), INTENT(IN) :: DR
REAL (KIND=c_double), DIMENSION(VAL), INTENT(IN) ::  RDATA, GDATA
DOUBLE PRECISION :: Hcap1, Hcap2, Vcap
INTEGER :: rinit

if (allocated(SHELL_VOL)) deallocate(SHELL_VOL)
allocate(SHELL_VOL(VAL+1), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: send_gr"//CHAR(0), "Table: SHELL_VOL"//CHAR(0))
  send_gr = 0
  goto 001
endif

do i=1, VAL
  SHELL_VOL(i) = 0.0d0
  SHELL_VOL(i) = 4.0d0*PI*((RDATA(i)+DR)**3 - (RDATA(i)**3))/3
! To take into account the atoms between a/2 and a srqt(3)/2
! We need the small volume they can be found in.
  if (OVERALL_CUBIC .and. RDATA(i)+DR.gt.MBOX) then
    Hcap1=RDATA(i)+DR-MBOX
    if (RDATA(i) .le. MBOX) then
      Hcap2=0.0d0
    else
      Hcap2=RDATA(i)-MBOX
    endif
    Vcap=Hcap1**2*(3*RDATA(i) - Hcap1) - Hcap2**2*(3*RDATA(i) - Hcap2)
    Vcap=Vcap*PI*2
    SHELL_VOL(i)=SHELL_VOL(i)-Vcap
  endif
enddo

j=IC
if (j > 8) then
  m=j-16
  m=m/5
  k=m/NSP+1
  l=m-(k-1)*NSP+1
endif

rinit = 1
if (RDATA(1) .eq. 0.0) rinit = 2

Rmax = RDATA(VAL)
do i=1, NUMBER_OF_QMOD
  do n=rinit, VAL
    Phi = Q_POINT(i)*RDATA(n)
    Fact_Rmax = PI*RDATA(n)/Rmax
!    Sinus_Fact_Rmax = sin(Fact_Rmax)/Fact_Rmax
    Sinus_phi = sin(Phi)/Phi
    if (j .eq. 0) then
      S(i) = S(i) + SHELL_VOL(n)*(GDATA(n) - 1)*Sinus_phi!*Sinus_Fact_Rmax
    else if (j .eq. 8) then
      XS(i) = XS(i) + SHELL_VOL(n)*(GDATA(n) - 1)*Sinus_phi
    else
      Sij(i,k,l) = Sij(i,k,l) + SHELL_VOL(n)*(GDATA(n) - 1)*Sinus_phi!*Sinus_Fact_Rmax
    endif
  enddo
  if (j .eq. 0) then
    S(i) = 1.0d0 + S(i)*TOTAL_DENSITY
  else if (j .eq. 8) then
    XS(i) = 1.0d0 + XS(i)*TOTAL_DENSITY
  else
    if (l .eq. k) then
      Sij(i,k,l) = 1.0d0 + Xi(l)*Sij(i,k,l)*TOTAL_DENSITY
    else
      Sij(i,k,l) = sqrt(Xi(k)*Xi(l))*Sij(i,k,l)*TOTAL_DENSITY
    endif
  endif
enddo

send_gr = 1

001 continue

if (allocated(SHELL_VOL)) deallocate(SHELL_VOL)

END FUNCTION
