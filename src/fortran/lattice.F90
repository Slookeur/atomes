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
!! @file lattice.F90
!! @short Lattice properties
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER (KIND=c_int) FUNCTION add_cells (NP, NPS, sizec) BIND (C,NAME='add_cells_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: NP, NPS
INTEGER (KIND=c_int), INTENT(IN), DIMENSION(3) :: sizec
INTEGER :: PIA, PIB, PIC, PID, PIE, PIF
DOUBLE PRECISION, DIMENSION(3) :: lshift
INTEGER, DIMENSION(:), ALLOCATABLE :: NEWLOT
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: NEWPOS

INTERFACE
  INTEGER FUNCTION SEND_POS(NPA, NPS, NLOT, POSTAB)
    INTEGER, INTENT(IN) :: NPA, NPS
    INTEGER, DIMENSION(:), INTENT(IN) :: NLOT
    DOUBLE PRECISION, DIMENSION(:,:,:), INTENT(IN) :: POSTAB
  END FUNCTION
END INTERFACE

PIA = (sizec(1)+1)*(sizec(2)+1)*(sizec(3)+1)
PIB = NP * PIA

if (allocated(NEWPOS)) deallocate(NEWPOS)
allocate(NEWPOS(PIB,3,NPS), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: add_cells"//CHAR(0), "Table: NEWPOS"//CHAR(0))
  add_cells=0
  goto 001
endif
if (allocated(NEWLOT)) deallocate(NEWLOT)
allocate(NEWLOT(PIB), STAT=ERR)
if (ERR .ne. 0) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Function: add_cells"//CHAR(0), "Table: NEWLOT"//CHAR(0))
  add_cells=0
  goto 001
endif

do PIA=1, NPS
  PIB=0
  do PID=1, sizec(1)+1
    do PIE=1, sizec(2)+1
      do PIF=1, sizec(3)+1
        lshift(1)=(PID-1)*THE_BOX(1)%lvect(1,1) + (PIE-1)*THE_BOX(1)%lvect(2,1) + (PIF-1)*THE_BOX(1)%lvect(3,1)
        lshift(2)=(PID-1)*THE_BOX(1)%lvect(1,2) + (PIE-1)*THE_BOX(1)%lvect(2,2) + (PIF-1)*THE_BOX(1)%lvect(3,2)
        lshift(3)=(PID-1)*THE_BOX(1)%lvect(1,3) + (PIE-1)*THE_BOX(1)%lvect(2,3) + (PIF-1)*THE_BOX(1)%lvect(3,3)
        do PIC=1, NP
          PIB=PIB+1
          NEWPOS(PIB,:,PIA) = FULLPOS(PIC,:,PIA) + lshift(:)
          NEWLOT(PIB) = LOT(PIC)
        enddo
      enddo
    enddo
  enddo
enddo

add_cells=0
call init_data (PIB, NSP, NPS, 0)
if (SEND_POS(PIB, NPS, NEWLOT, NEWPOS) .eq. 1) add_cells=1

001 continue
if (allocated(NEWPOS)) deallocate(NEWPOS)
if (allocated(NEWLOT)) deallocate(NEWLOT)

END FUNCTION

INTEGER (KIND=c_int) FUNCTION shift_box_center (NP, NPS, cshift, REF) BIND (C,NAME='shift_box_center_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: NP, NPS, REF
REAL (KIND=c_double), INTENT(IN), DIMENSION(3) :: cshift
INTEGER :: PIB, PIC, PID
DOUBLE PRECISION, DIMENSION(3,3) :: h_mat
DOUBLE PRECISION, DIMENSION(3) :: TPO

INTERFACE
  INTEGER FUNCTION SEND_POS(NPA, NPS, NLOT, POSTAB)
    INTEGER, INTENT(IN) :: NPA, NPS
    INTEGER, DIMENSION(:), INTENT(IN) :: NLOT
    DOUBLE PRECISION, DIMENSION(:,:,:), INTENT(IN) :: POSTAB
  END FUNCTION
END INTERFACE

h_mat(:,1) = THE_BOX(1)%lvect(1,:)
h_mat(:,2) = THE_BOX(1)%lvect(2,:)
h_mat(:,3) = THE_BOX(1)%lvect(3,:)

do PIC=1, NPS
  do PIB=1, NP
    do PID=1, 3
      FULLPOS(PIB,PID,PIC) = FULLPOS(PIB,PID,PIC) + cshift(PID)
    enddo
    TPO=MATMUL(THE_BOX(1)%lrecp, FULLPOS(PIB,:,PIC))
    TPO=TPO-NINT(TPO/0.5)
    FULLPOS(PIB,:,PIC) = MATMUL(h_mat,TPO)

    !FULLPOS(PIB,:,PIC) = FULLPOS(PIB,:,PIC) + cshift(2)
    !FULLPOS(PIB,:,PIC) = FULLPOS(PIB,:,PIC) + cshift(3)
  enddo
enddo

if (REF .eq. 1) then
  shift_box_center = SEND_POS (NP, NPS, LOT, FULLPOS)
else
  shift_box_center = 1
endif

END FUNCTION

DOUBLE PRECISION FUNCTION f_dot_product (a, b)

DOUBLE PRECISION, DIMENSION(3), INTENT(IN) :: a, b
INTEGER :: DIM
f_dot_product = 0.0d0

do  DIM=1, 3
  f_dot_product = f_dot_product + a(DIM)*b(DIM)
enddo

END FUNCTION

SUBROUTINE f_cross_product (a, b, c)

DOUBLE PRECISION, DIMENSION(3), INTENT(IN) :: a, b
DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: c

c(:)=0.0d0

c(1) = a(2)*b(3) - a(3)*b(2)
c(2) = a(3)*b(1) - a(1)*b(3)
c(3) = a(1)*b(2) - a(2)*b(1)

END SUBROUTINE

INTEGER (KIND=c_int) FUNCTION lattice (totl, lid, vectors, vmod, angles, lat, cfrac, apbc) BIND (C,NAME='lattice_')

!
! Compute lattice angles from lattice vectors
! Lattice vector modules
! Lattice volume
! Reciprocal lattice parameters
!

USE PARAMETERS

IMPLICIT NONE

REAL (KIND=c_double), INTENT(IN), DIMENSION(3,3) :: vectors
REAL (KIND=c_double), INTENT(IN), DIMENSION(3) :: vmod
REAL (KIND=c_double), INTENT(INOUT), DIMENSION(3) :: angles
INTEGER (KIND=c_int), INTENT(IN) :: totl, lid, lat, cfrac, apbc

DOUBLE PRECISION :: ALPHA, BETA, GAMA                            ! Lattice Angles
DOUBLE PRECISION :: CALPHA, SALPHA, CBETA, SBETA, CGAMA, SGAMA   ! Cosinus and Sinus
DOUBLE PRECISION, DIMENSION(3) :: TMPLA

INTERFACE
  DOUBLE PRECISION FUNCTION f_dot_product (a, b)
    DOUBLE PRECISION, DIMENSION(3), INTENT(IN) :: a, b
  END FUNCTION
  SUBROUTINE f_cross_product (a, b, c)
    DOUBLE PRECISION, DIMENSION(3), INTENT(IN) :: a, b
    DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: c
  END SUBROUTINE
END INTERFACE

! Transition from C/Gtk to Fortran90 !
lattice = 0

if (lid .eq. 0) then
  if (allocated(THE_BOX)) deallocate(THE_BOX)
  allocate(THE_BOX(totl), STAT=ERR)
  if (ERR .ne. 0) then
    call show_error ("Impossible to allocate memory"//CHAR(0), &
                     "Function: lattice_"//CHAR(0), "Type: THE_BOX"//CHAR(0))
    goto 001
  endif
  NCELLS = totl
endif

NBOX => THE_BOX(lid+1)

NBOX%GLASS=.false.
NBOX%CUBIC=.false.

if (lat .gt. 0) then

  do i=1, 3
    do j=1, 3
      NBOX%lvect(j,i) = vectors(i,j)
    enddo
  enddo

  NBOX%modv(1) = vmod(1)
  NBOX%modv(2) = vmod(2)
  NBOX%modv(3) = vmod(3)
  ALPHA = angles(1)
  BETA = angles(2)
  GAMA = angles(3)

  if (lat .eq. 1) then

    if (ALPHA.eq.90.0 .and. BETA.eq.90.0 .and. GAMA.eq.90.0) then
       NBOX%GLASS=.true.
       if (NBOX%modv(1).eq.NBOX%modv(2) .and. NBOX%modv(2).eq.NBOX%modv(3)) NBOX%CUBIC=.true.
    endif

    if (ALPHA.eq.90.0) then
      ALPHA = PI/2.0d0
      SALPHA = 1.0d0
      CALPHA = 0.0d0
    else
      ALPHA = ALPHA*PI/180.0d0
      SALPHA = sin(ALPHA)
      CALPHA = cos(ALPHA)
    endif
    if (BETA.eq.90.0) then
      BETA = PI/2.0d0
      SBETA = 1.0d0
      CBETA = 0.0d0
    else
      BETA = BETA*PI/180.0d0
      SBETA = sin(BETA)
      CBETA = cos(BETA)
    endif
    if (GAMA.eq.90.0) then
      GAMA = PI/2.0d0
      SGAMA = 1.0d0
      CGAMA = 0.0d0
    else
      GAMA = GAMA*PI/180.0d0
      SGAMA = sin(GAMA)
      CGAMA = cos(GAMA)
    endif

    NBOX%lvect(1,1) = NBOX%modv(1)
    NBOX%lvect(1,2) = 0.0d0
    NBOX%lvect(1,3) = 0.0d0
    NBOX%lvect(2,1) = NBOX%modv(2)*CGAMA
    NBOX%lvect(2,2) = NBOX%modv(2)*SGAMA
    NBOX%lvect(2,3) = 0.d0
    NBOX%lvect(3,1) = NBOX%modv(3)*CBETA
    LTEMP = (CALPHA - CBETA*CGAMA)/SGAMA
    NBOX%lvect(3,2) = NBOX%modv(3)*LTEMP
    NBOX%lvect(3,3) = NBOX%modv(3)*sqrt(SBETA*SBETA - LTEMP*LTEMP)

  else

    NBOX%modv(1)=sqrt(NBOX%lvect(1,1)**2+NBOX%lvect(1,2)**2+NBOX%lvect(1,3)**2)
    NBOX%modv(2)=sqrt(NBOX%lvect(2,1)**2+NBOX%lvect(2,2)**2+NBOX%lvect(2,3)**2)
    NBOX%modv(3)=sqrt(NBOX%lvect(3,1)**2+NBOX%lvect(3,2)**2+NBOX%lvect(3,3)**2)

    ALPHA= (NBOX%lvect(3,1)*NBOX%lvect(2,1)+ &
                 NBOX%lvect(3,2)*NBOX%lvect(2,2)+ &
                 NBOX%lvect(3,3)*NBOX%lvect(2,3))/(NBOX%modv(2)*NBOX%modv(3))
    BETA = (NBOX%lvect(1,1)*NBOX%lvect(3,1)+ &
                 NBOX%lvect(1,2)*NBOX%lvect(3,2)+ &
                 NBOX%lvect(1,3)*NBOX%lvect(3,3))/(NBOX%modv(1)*NBOX%modv(3))
    GAMA = (NBOX%lvect(1,1)*NBOX%lvect(2,1)+ &
                 NBOX%lvect(1,2)*NBOX%lvect(2,2)+ &
                 NBOX%lvect(1,3)*NBOX%lvect(2,3))/(NBOX%modv(1)*NBOX%modv(2))

    if (ALPHA.eq.0.0d0 .and. BETA.eq.0.0d0 .and. GAMA.eq.0.0d0) then
      NBOX%GLASS=.true.
      if (NBOX%modv(1).eq.NBOX%modv(2) .and. NBOX%modv(2).eq.NBOX%modv(3)) NBOX%CUBIC=.true.
    endif

    if (ALPHA.eq.0.0d0) then
      angles(1) = 90.0d0
      ALPHA = PI/2.0d0
      SALPHA = 1.0d0
      CALPHA = 0.0d0
    else
      ALPHA = acos(ALPHA)
      angles(1) = ALPHA*180.0d0/PI
      SALPHA = sin(ALPHA)
      CALPHA = cos(ALPHA)
    endif
    if (BETA.eq.0.0d0) then
      angles(2) = 90.0d0
      BETA = PI/2.0d0
      SBETA = 1.0d0
      CBETA = 0.0d0
    else
      BETA = acos(BETA)
      angles(2) = BETA*180.0d0/PI
      SBETA = sin(BETA)
      CBETA = cos(BETA)
    endif
    if (GAMA.eq.0.0d0) then
      angles(3) = 90.0d0
      GAMA = PI/2.0d0
      SGAMA = 1.0d0
      CGAMA = 0.0d0
    else
      GAMA = acos(GAMA)
      angles(3) = GAMA*180.0d0/PI
      SGAMA = sin(GAMA)
      CGAMA = cos(GAMA)
    endif

  endif

  if (ALPHA.eq.0.0d0 .and. BETA.eq.0.0d0 .and. GAMA.eq.0.0d0) then
  ! If some problems display a GTK error message.
    call show_error ("Problem with the simulation box parameters"//CHAR(0), &
                     "Computed angles are equal to 0.0d0"//CHAR(0), "Function: lattice"//CHAR(0))
    goto 001
  endif

!  write (*,*) NBOX%lvect(1,1), NBOX%lvect(1,2), NBOX%lvect(1,3)
!  write (*,*) NBOX%lvect(2,1), NBOX%lvect(2,2), NBOX%lvect(2,3)
!  write (*,*) NBOX%lvect(3,1), NBOX%lvect(3,2), NBOX%lvect(3,3)
!  write (*,*) NBOX%modv(1), NBOX%modv(2), NBOX%modv(3)
!  write (*,*) ALPHA*180/PI, BETA*180/PI, GAMA*180/PI

  NBOX%minv=min(NBOX%modv(1),NBOX%modv(2))
  NBOX%minv=min(NBOX%minv,NBOX%modv(3))
  NBOX%maxv=max(NBOX%modv(1),NBOX%modv(2))
  NBOX%maxv=max(NBOX%maxv,NBOX%modv(3))

  NBOX%VOLUME=(NBOX%lvect(1,2)*NBOX%lvect(2,3)-NBOX%lvect(1,3)*NBOX%lvect(2,2))*NBOX%lvect(3,1) &
                     +(NBOX%lvect(1,3)*NBOX%lvect(2,1)-NBOX%lvect(1,1)*NBOX%lvect(2,3))*NBOX%lvect(3,2) &
                     +(NBOX%lvect(1,1)*NBOX%lvect(2,2)-NBOX%lvect(1,2)*NBOX%lvect(2,1))*NBOX%lvect(3,3)
  NBOX%VOLUME = abs(NBOX%VOLUME)

! Reciprocal lattice parameters !

  call f_cross_product (NBOX%lvect(2,:), NBOX%lvect(3,:), TMPLA)
  NBOX%lrecp(1,:) = TMPLA / f_dot_product(NBOX%lvect(1,:), TMPLA)
  call f_cross_product (NBOX%lvect(3,:), NBOX%lvect(1,:), TMPLA)
  NBOX%lrecp(2,:) = TMPLA / f_dot_product(NBOX%lvect(2,:), TMPLA)
  call f_cross_product (NBOX%lvect(1,:), NBOX%lvect(2,:), TMPLA)
  NBOX%lrecp(3,:) = TMPLA / f_dot_product(NBOX%lvect(3,:), TMPLA)

! modules of the reciprocal lattice vectors

  NBOX%modr(1)=sqrt(NBOX%lrecp(1,1)**2+NBOX%lrecp(1,2)**2+NBOX%lrecp(1,3)**2)
  NBOX%modr(2)=sqrt(NBOX%lrecp(2,1)**2+NBOX%lrecp(2,2)**2+NBOX%lrecp(2,3)**2)
  NBOX%modr(3)=sqrt(NBOX%lrecp(3,1)**2+NBOX%lrecp(3,2)**2+NBOX%lrecp(3,3)**2)

  NBOX%modr(:) = 2.0d0*PI*NBOX%modr(:)

  NBOX%minr=min(NBOX%modr(1),NBOX%modr(2))
  NBOX%minr=min(NBOX%minr,NBOX%modr(3))
  NBOX%maxr=max(NBOX%modr(1),NBOX%modr(2))
  NBOX%maxr=max(NBOX%maxr,NBOX%modr(3))

! Creation of the matrix to convert fractional to cartesian coordinates

  z=sqrt(abs(1 - CALPHA*CALPHA &
               - CBETA*CBETA &
               - CGAMA*CGAMA &
               + 2*CALPHA*CBETA*CGAMA))
  z=z/SGAMA
  i = 0
  if (cfrac > 0) i = cfrac - 1

  NBOX%fractocart(1,1)=NBOX%modv(1)/(2.0**(i))
  NBOX%fractocart(1,2)=0.0d0
  NBOX%fractocart(1,3)=0.0d0
  NBOX%fractocart(2,1)=NBOX%modv(2)*CGAMA/(2.0**(i))
  NBOX%fractocart(2,2)=NBOX%modv(2)*SGAMA/(2.0**(i))
  NBOX%fractocart(2,3)=0.0d0
  NBOX%fractocart(3,1)=NBOX%modv(3)*CBETA/(2.0**(i))
  NBOX%fractocart(3,2)=NBOX%modv(3)*((CALPHA-CBETA*CGAMA)/SGAMA)/(2.0**(i))
  NBOX%fractocart(3,3)=NBOX%modv(3)*z/(2.0**(i))
  !write (6, *)
  !write (6, '("Frac to cart matrix in lattice:")')
  !write (6, '(f15.10,4x,f15.10,4x,f15.10)') NBOX%fractocart(1,1), NBOX%fractocart(1,2), NBOX%fractocart(1,3)
  !write (6, '(f15.10,4x,f15.10,4x,f15.10)') NBOX%fractocart(2,1), NBOX%fractocart(2,2), NBOX%fractocart(2,3)
  !write (6, '(f15.10,4x,f15.10,4x,f15.10)') NBOX%fractocart(3,1), NBOX%fractocart(3,2), NBOX%fractocart(3,3)
  !write (6, *)

! Creation of the matrix to convert cartesian to fractional coordinates

  NBOX%carttofrac(1,1)=1.0d0/NBOX%fractocart(1,1)
  NBOX%carttofrac(1,2)=0.0d0
  NBOX%carttofrac(1,3)=0.0d0
  NBOX%carttofrac(2,1)=-CGAMA/(SGAMA*(NBOX%modv(1)/(2.0**(i))))
  NBOX%carttofrac(2,2)=1.0d0/NBOX%fractocart(2,2)
  NBOX%carttofrac(2,3)=0.0d0
  NBOX%carttofrac(3,1)=((NBOX%modv(2)/(2.0**(i)))*(NBOX%modv(3)/(2.0**(i))))/NBOX%VOLUME
  NBOX%carttofrac(3,1)=NBOX%carttofrac(3,1) * (CALPHA*CGAMA - CBETA)/SALPHA
  NBOX%carttofrac(3,2)=((NBOX%modv(1)/(2.0**(i)))*(NBOX%modv(3)/(2.0**(i))))/NBOX%VOLUME
  NBOX%carttofrac(3,2)=NBOX%carttofrac(3,2) * (CBETA*CGAMA - CALPHA)/SGAMA
  NBOX%carttofrac(3,3)=1.0d0/NBOX%fractocart(3,3)

  if (apbc .eq. 1) then
    PBC=.true.
  else
    PBC=.false.
    NBOX%GLASS=.false.
    NBOX%CUBIC=.false.
  endif

else

  NBOX%VOLUME=0.0d0
  NBOX%minr=0.0d0
  NBOX%modv(:)=0.0d0
  NBOX%modr(:)=0.0d0
  NBOX%lvect(:,:)=0.0d0
  NBOX%lrecp(:,:)=0.0d0
  NBOX%fractocart(:,:)=0.0d0
  NBOX%GLASS=.false.
  NBOX%CUBIC=.false.
  PBC=.false.

endif

REAL_DENSITY=0.0d0
TOTAL_DENSITY=0.0d0
if (NBOX%VOLUME.ne.0.0) then
  do i=1, NSP
    REAL_DENSITY=REAL_DENSITY+NBSPBS(i)*MASS(i)
  enddo
  TOTAL_DENSITY = dble(NA)/NBOX%VOLUME
  REAL_DENSITY=REAL_DENSITY/AVOGADRO
  REAL_DENSITY=REAL_DENSITY/NBOX%VOLUME
  REAL_DENSITY=REAL_DENSITY*10.0d0
endif

! To lattice_info_
call lattice_info (lid, NBOX%VOLUME, REAL_DENSITY, &
                   NBOX%lvect, NBOX%lrecp, NBOX%modv, angles, &
                   NBOX%fractocart, NBOX%carttofrac)

if (lid .eq. totl-1) then
  MBOX = 0.0d0
  MEANVOL = 0.0d0
  OVERALL_CUBIC = .true.
  do i=1, NCELLS
    MBOX = MBOX + THE_BOX(i)%minv
    MEANVOL = MEANVOL + THE_BOX(i)%VOLUME
    if (OVERALL_CUBIC .and. THE_BOX(i)%CUBIC) then
      OVERALL_CUBIC = .true.
    else
      OVERALL_CUBIC = .false.
    endif
  enddo
  MEANVOL = MEANVOL / NCELLS
  MBOX = MBOX / (2.0d0*NCELLS)
endif

lattice = 1

001 continue

END FUNCTION
