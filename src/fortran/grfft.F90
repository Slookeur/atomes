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
!! @file grfft.F90
!! @short g(r) analysis: Fourier transform calculation
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION g_of_r_fft (NDR, DTR, MMX) BIND (c,name='g_of_r_fft_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: NDR
REAL (KIND=c_double), INTENT(IN) :: DTR, MMX
INTERFACE
  INTEGER FUNCTION recup_data (i, j)
    INTEGER, INTENT(IN) :: i, j
  END FUNCTION
  LOGICAL FUNCTION GRBT(GrToBT, NDTR)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: NDTR
    DOUBLE PRECISION, DIMENSION(NDTR,NSP,NSP), INTENT(IN) :: GrToBT
  END FUNCTION
END INTERFACE

qvmax = MMX
NUMBER_OF_I = NDR

if (allocated(GFFT)) deallocate(GFFT)
allocate(GFFT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: g_of_r_fft"//CHAR(0), "Table: GFFT"//CHAR(0))
  g_of_r_fft=0
  goto 001
endif
if (allocated(DFFT)) deallocate(DFFT)
allocate(DFFT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: g_of_r_fft"//CHAR(0), "Table: DFFT"//CHAR(0))
  g_of_r_fft=0
  goto 001
endif
if (allocated(TDFFT)) deallocate(TDFFT)
allocate(TDFFT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: g_of_r_fft"//CHAR(0), "Table: TDFFT"//CHAR(0))
  g_of_r_fft=0
  goto 001
endif

if (allocated(R_PFFT)) deallocate(R_PFFT)
allocate(R_PFFT(NDR), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: g_of_r_fft"//CHAR(0), "Table: R_PFFT"//CHAR(0))
  g_of_r_fft=0
  goto 001
endif

do i=1, NDR
  R_PFFT(i)= 0.0d0
  R_PFFT(i)= 1.0 / (2.0*PI*PI*NA/MEANVOL*(i-0.5)*DTR)
enddo

j=0
l=0
if (recup_data (j, IDSK) .ne. 1) then
  g_of_r_fft=0
  goto 001
endif

j=4
if (recup_data (j, IDSK) .ne. 1) then
  g_of_r_fft=0
  goto 001
endif

if (NSP .eq. 2) then
  if (allocated(GQBT)) deallocate(GQBT)
  allocate(GQBT(NDR,NSP,NSP), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: g_of_r_fft"//CHAR(0), "Table: GQBT"//CHAR(0))
    g_of_r_fft=0
    goto 001
  endif
endif

j=j+NSP*NSP*2+2
do n=1, NSP
do m=1, NSP
  j=j+2
  if (recup_data (j, IDSK) .ne. 1) then
    g_of_r_fft=0
    goto 001
  endif
enddo
enddo

if (NSP .eq. 2) then
  if (GRBT (GQBT, NDR)) then
    do k=1, NDR
      GFFT(k)=BTij(k,1)
    enddo
    call save_curve (NDR, GFFT, l, IDGRFFT)
    l = l+2
    do k=1, NDR
      GFFT(k)=BTij(k,2)
    enddo
    call save_curve (NDR, GFFT, l, IDGRFFT)
    l = l+2
    do k=1, NDR
      GFFT(k)=BTij(k,3)
    enddo
    call save_curve (NDR, GFFT, l, IDGRFFT)
  endif
  if (allocated(BTij)) deallocate(BTij)
endif

g_of_r_fft=1

001 continue

if (allocated(GFFT)) deallocate(GFFT)
if (allocated(DFFT)) deallocate(DFFT)
if (allocated(TDFFT)) deallocate(TDFFT)
if (allocated(R_PFFT)) deallocate(R_PFFT)
if (allocated(GQBT)) deallocate(GQBT)

END FUNCTION

INTEGER FUNCTION send_sq (IC, VAL, DTR, KDATA, SDATA)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: IC, VAL
DOUBLE PRECISION, INTENT(IN) :: DTR
DOUBLE PRECISION, DIMENSION(VAL), INTENT(IN) :: KDATA, SDATA
DOUBLE PRECISION :: SUML

j = IC

call FFT_TO_GR (VAL, KDATA, SDATA, GFFT)

call save_curve (NUMBER_OF_I, GFFT, l, IDGRFFT)
l=l+2

if (j.gt.4) then

  if (nsp .eq. 2) then
    do i=1, NUMBER_OF_I
      GQBT(i,n,m) = GFFT(i)
    enddo
  endif
  do i=1, NUMBER_OF_I
    DFFT(i)= 4.0*PI*(((i-0.5)*DTR)**2)*DTR*GFFT(i)/MEANVOL
    if (i .gt. 1) DFFT(i)= DFFT(i) + DFFT(i-1)
  enddo
  if (n .eq. m) then
    k = (NBSPBS(n)-1)
  else
    k = NBSPBS(m)
  endif
  do i=1, NUMBER_OF_I
    DFFT(i)= DFFT(i)*k
  enddo
  do i=1, NUMBER_OF_I
    GFFT(i)=(GFFT(i)-1.0)*4.0*PI*(NA/MEANVOL)*(i-0.5)*DTR
  enddo
  call save_curve (NUMBER_OF_I, GFFT, l, IDGRFFT)
  l=l+2
  call save_curve (NUMBER_OF_I, DFFT, l, IDGRFFT)
  l=l+1

else

  SUML=0.0d0
  do k=1, NSP
    if (j .eq. 0) then
      SUML=SUML+NSCATTL(k)*Xi(k)
    else
      SUML=SUML+XSCATTL(k)*Xi(k)
    endif
  enddo
  SUML=SUML*SUML
  do i=1, NUMBER_OF_I
    GFFT(i)=(GFFT(i)-1.0)*4.0*PI*(NA/MEANVOL)*(i-0.5)*DTR
  enddo
  call save_curve (NUMBER_OF_I, GFFT, l, IDGRFFT)
  l=l+2
  do i=1, NUMBER_OF_I
    TDFFT(i) = GFFT(i)*SUML
  enddo
  call save_curve (NUMBER_OF_I, TDFFT, l, IDGRFFT)
  l=l+2
  do i=1, NUMBER_OF_I
    TDFFT(i) = TDFFT(i) + 4.0*PI*(NA/MEANVOL)*(i-0.5)*DTR*SUML
  enddo
  call save_curve (NUMBER_OF_I, TDFFT, l, IDGRFFT)
  l=l+2

endif

send_sq = 1

CONTAINS

SUBROUTINE FFT_TO_GR (LTAB, KTAB, TAB, RTAB)

USE PARAMETERS

INTEGER, INTENT(IN) :: LTAB
DOUBLE PRECISION, DIMENSION(LTAB), INTENT(IN) :: KTAB, TAB
DOUBLE PRECISION, DIMENSION(NUMBER_OF_I), INTENT(INOUT) :: RTAB

do i=1, NUMBER_OF_I
  RTAB(i)= 0.0d0
  do k=1, LTAB-1
    Phi = KTAB(k)*(i-0.5)*DTR
    RTAB(i) = RTAB(i) + KTAB(k)*(TAB(k) - 1.0)*sin(Phi)*(KTAB(k+1) - KTAB(k))
  enddo
  RTAB(i) = 1.0 + R_PFFT(i) * RTAB(i)
enddo

END SUBROUTINE

END FUNCTION
