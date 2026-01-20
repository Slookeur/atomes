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
!! @file fzbt.F90
!! @short Partial structure factors: Faber-Ziman and Bathia-Thornton analysis
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

LOGICAL FUNCTION FZBT (NDQ)

!
! Compute Faber-Ziman and Bathia-Thornton S(q) from Ashcroft S(q)
!

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: NDQ
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: Xr


if (NSP .eq. 2) then
  if (allocated(BTij)) deallocate(BTij)
  allocate(BTij(NDQ,4), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: FZBT"//CHAR(0), "Table: BTij"//CHAR(0))
    FZBT=.false.
    goto 001
  endif
  BTij(:,:)=0.0d0
endif

if (allocated(FZSij)) deallocate(FZSij)
allocate(FZSij(NDQ,NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: FZBT"//CHAR(0), "Table: FZSij"//CHAR(0))
  FZBT=.false.
  goto 001
endif

FZSij(:,:,:)=0.0d0

if (allocated(Xr)) deallocate(Xr)
allocate(Xr(NSP), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: FZBT"//CHAR(0), "Table: Xr"//CHAR(0))
  FZBT=.false.
  goto 001
endif

do o=1, NSP
  Xr(o)=Xi(o)
enddo

do m=1, NDQ
  do n=1, NSP
    do o=1, NSP
      if (o .eq. n) then
        FZSij(m,n,o) = 1 + (Sij(m,n,o)-1)/Xr(n)
        if (NSP .eq. 2) then
          BTij(m,1) = BTij(m,1) + Xr(o)*Xr(o)*FZSij(m,n,o)
          BTij(m,3) = BTij(m,3) + FZSij(m,n,o)
        endif
      else
        FZSij(m,n,o) = Sij(m,n,o)/sqrt(Xr(n)*Xr(o)) + 1
        if (NSP .eq. 2) then
          BTij(m,1) = BTij(m,1) + Xr(o)*Xr(n)*FZSij(m,n,o)
          BTij(m,3) = BTij(m,3) - FZSij(m,n,o)
        endif
      endif
    enddo
  enddo
  if (NSP .eq. 2) then
    if (NBSPBS(1) .le. NBSPBS(2)) then
      o=1
      n=2
    else
      o=2
      n=1
    endif
    BTij(m,2) = Xr(o)*FZSij(m,o,o) - Xr(n)*FZSij(m,n,n)
    BTij(m,2) = BTij(m,2)+FZSij(m,o,n)*(Xr(n)-Xr(o))
    BTij(m,2) = BTij(m,2)*Xr(n)*Xr(o)
    BTij(m,3) = (BTij(m,3)*Xr(1)*Xr(2) +1)*Xr(1)*Xr(2)
    BTij(m,4) = BTij(m,3)/(Xr(1)*Xr(2))
  endif
enddo

if (allocated(Xr)) deallocate(Xr)

FZBT=.true.

001 continue

END FUNCTION

LOGICAL FUNCTION GRBT(GrToBT, NDTR)

!
! Compute Bathia-Thornton g(r)
!

USE PARAMETERS

INTEGER, INTENT(IN) :: NDTR
DOUBLE PRECISION, DIMENSION(NDTR,NSP,NSP), INTENT(IN) :: GrToBT

if (allocated(BTij)) deallocate(BTij)
allocate(BTij(NDTR,3), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: GRBT"//CHAR(0), "Table: BTij"//CHAR(0))
  GRBT=.false.
  goto 001
endif
BTij(:,:)=0.0d0

do m=1, NDTR
  do n=1, NSP
    do o=1, NSP
      if (o .eq. n) then
        BTij(m,1) = BTij(m,1) + Xi(o)*Xi(o)*GrToBT(m,n,o)
        BTij(m,3) = BTij(m,3) + GrToBT(m,n,o)
      else
        BTij(m,1) = BTij(m,1) + Xi(o)*Xi(n)*GrToBT(m,n,o)
        BTij(m,3) = BTij(m,3) - GrToBT(m,n,o)
      endif
    enddo
  enddo
  if (NBSPBS(1) .le. NBSPBS(2)) then
    o=1
    n=2
  else
    o=2
    n=1
  endif
  BTij(m,2) = Xi(o)*GrToBT(m,o,o) - Xi(n)*GrToBT(m,n,n)
  BTij(m,2) = BTij(m,2)+GrToBT(m,o,n)*(Xi(n)-Xi(o))
  BTij(m,3) = BTij(m,3)*Xi(1)*Xi(2)
!  BTij(m,4) = BTij(m,3)/(Xi(1)*Xi(2))
!  BTij(m,5) = BTij(m,2)/(Xi(n)/ )
enddo

GRBT=.true.

001 continue

END FUNCTION
