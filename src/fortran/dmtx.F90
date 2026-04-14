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
!! @file dmtx.F90
!! @short Distance matrix calculation
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

SUBROUTINE PRINT_THIS_PIXEL (TPS, TPIX, PIXL, pix, atoms)

USE PARAMETERS

IMPLICIT NONE

TYPE (PIXEL), DIMENSION(TPS), INTENT(IN) :: TPIX
TYPE (PIXEL), INTENT(IN) :: PIXL
LOGICAL, INTENT(IN) :: atoms
INTEGER, INTENT(IN) :: TPS, pix
INTEGER :: PA, PB

write (6, '("Pixel= ",i4)') pix
do PA=1, PIXL%NEIGHBOR
  PB = PIXL%IDNEIGH(PA)
  if (atoms) then
    write (6, '("        N(",i4,")= ",i10," contains: ",i4," atom(s)")') PA, PIXL%IDNEIGH(PA), TPIX(PB)%ATOMS
  else
    write (6, '("        N(",i4,")= ",i10)') PA, PIXL%IDNEIGH(PA)
  endif
enddo
write (6, *)
if (atoms) then
  do PA=1, PIXL%ATOMS
    write (6, '("        At(",i4,")= ",i10)') PA, PIXL%ATOM_ID(PA)
  enddo
  write (6, *)
endif

END SUBROUTINE

SUBROUTINE SET_SHIFT (shift, ai, bi, ci, npa, npb, npc, nab, abc)

  IMPLICIT NONE

  INTEGER, DIMENSION(3,3,3), INTENT(INOUT) :: shift
  INTEGER, INTENT(IN) :: ai, bi, ci
  INTEGER, INTENT(IN) :: npa, npb, npc
  INTEGER, INTENT(IN) :: nab, abc

  ! Standard pixel positions:

  ! Lower level
  !shift(1,1,1) = nab+npa+1
  !shift(1,1,2) = nab+npa
  !shift(1,1,3) = nab+npa-1
  !shift(1,2,1) = nab+1
  !shift(1,2,2) = nab
  !shift(1,2,3) = nab-1
  !shift(1,3,1) = nab-npa+1
  !shift(1,3,2) = nab-npa
  !shift(1,3,3) = nab-npa-1

  ! Same level
  !shift(2,1,1) = npa+1
  !shift(2,1,2) = npa
  !shift(2,1,3) = npa-1
  !shift(2,2,1) = 1
  !shift(2,2,2) = 0
  !shift(2,2,3) = -1
  !shift(2,3,1) = -npa+1
  !shift(2,3,2) = -npa
  !shift(2,3,3) = -npa-1

  ! Higher level
  !shift(3,1,1) = npa+1-nab
  !shift(3,1,2) = npa-nab
  !shift(3,1,3) = npa-1-nab
  !shift(3,2,1) = 1-nab
  !shift(3,2,2) = -nab
  !shift(3,2,3) = -1-nab
  !shift(3,3,1) = -npa+1-nab
  !shift(3,3,2) = -npa-nab
  !shift(3,3,3) = -npa-1-nab

  ! The shift is to compensate PBC

  if (ai .eq. 1) then
    shift (1,:,:) = npa
  else if (ai .eq. npa) then
    shift (3,:,:) = - npa
  endif

  if (bi .eq. 1) then
    shift (:,1,:) = shift (:,1,:) + nab
  else if (bi .eq. npb) then
    shift (:,3,:) = shift (:,3,:) - nab
  endif

  if (ci .eq. 1) then
    shift (:,:,1) = shift (:,:,1) + abc
  else if (ci .eq. npc) then
    shift (:,:,3) = shift (:,:,3) - abc
  endif

END SUBROUTINE

INTEGER FUNCTION GETNBX(NP, NPS)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: NP, NPS
INTEGER :: PIA, PIB, PIC, PID
INTEGER :: TPIXD, ADAPT_CUT
DOUBLE PRECISION :: ICUT, INID
DOUBLE PRECISION :: MPSIZE
DOUBLE PRECISION :: NEW_MPSIZE
DOUBLE PRECISION :: TARGETDP
DOUBLE PRECISION :: RHONUM
INTEGER :: MEMOID

if (.not.PBC) then
  do PIA=1, 3
    pmin(PIA)=FULLPOS(1,PIA,1)
    pmax(PIA)=pmin(PIA)
  enddo
  do PIA=1, NPS
    if (PIA.eq.1) then
      PID=2
    else
      PID=1
    endif
    do PIB=PID, NP
      do PIC=1, 3
        pmin(PIC) = min(FULLPOS(PIB,PIC,PIA),pmin(PIC))
        pmax(PIC) = max(FULLPOS(PIB,PIC,PIA),pmax(PIC))
      enddo
    enddo
  enddo
  do PIA=1, 3
    pmax(PIA) = pmax(PIA) - pmin(PIA)
  enddo
else
  pmax(:) = 0.0d0
  do PIB=1, NCELLS
    do PIA=1, 3
      pmax(PIA) = max(pmax(PIA), THE_BOX(PIB)%modv(PIA))
   enddo
  enddo
  do PIA=1, 3
    pmin(PIA) = - pmax(PIA)/2.0
  enddo
endif

#ifdef DEBUG
  write (6, '("pmin:: x= ",f15.10,", y= ",f15.10,", z= ",f15.10)') pmin(1), pmin(2), pmin(3)
  write (6, '("pmax:: x= ",f15.10,", y= ",f15.10,", z= ",f15.10)') pmax(1), pmax(2), pmax(3)
#endif

ICUT=0.0
do PIA=1, NSP
  do PIB=1, NSP
    ICUT=max(ICUT,Gr_TMP(PIA,PIB))
  enddo
enddo
ICUT = sqrt(ICUT)
ICUT = ICUT + 1.0d0
! The pixel size must be larger than the cutoff
! If the system is large with a low density the pixel box can be too big in number of pixels,
! and it is not possible to allocate the required memory.
! Therefore it is required to increase the cutoff slightly to reduce the number of pixels.
! We need to look into the density of atoms per pixel, good target values range between 1.5 and 2.0.
! With PBC this works providing that you do not have a large box that contains an isolated molecule,
! That is an extremely low number density, thus we need to check that as well.
TARGETDP=1.85d0
MPSIZE=1.0d0
! Always adjust cutoff if periodicity is not applied
ADAPT_CUT=2

! Number density
RHONUM=NP
do PIA=1, 3
  RHONUM=RHONUM/pmax(PIA)
enddo
if (RHONUM.gt.0.01) ADAPT_CUT=1

do TPIXD=1, ADAPT_CUT

! MPSIZE is the modifier
  CUTF=ICUT*MPSIZE
  CUTF=1.0d0/CUTF
  do PIA=1, 3
    isize(PIA) = INT(abs(pmax(PIA))*CUTF)
    if (isize(PIA) .lt. 3) isize(PIA) = 1
  enddo

  GETNBX = 1
  if (CALC_PRINGS .and. PBC) then
    GETNBX = 5
    do PIA=1, 3
      pmin(PIA) = GETNBX*pmin(PIA)
      pmax(PIA) = GETNBX*pmax(PIA)
    enddo
    do PIA=1, 3
      isize(PIA) = INT(abs(pmax(PIA))*CUTF)
      if (isize(PIA) .lt. 3) isize(PIA) = 1
    enddo
  endif

  if (TPIXD .eq. 1) then
    PIA = isize(1)*isize(2)*isize(3)
    INID = NP
    INID = INID/PIA
    NEW_MPSIZE = (TARGETDP/INID)**(1.0/3.0)
    if (NEW_MPSIZE .gt. MPSIZE) then
      ! Adaptation only if the cutoff increases
      ! Otherwise we would increase the number of pixels
      MPSIZE = NEW_MPSIZE
    endif
  endif
enddo

#ifdef DEBUG

  write (6, '("NBX= ",i10)') GETNBX
  write (6, '("isize:: x= ",I4,", y= ",I4,", z= ",I4)') isize(1), isize(2), isize(3)
  MEMOID = GUESS_GRID_MEMORY_SIZE(isize(1)*isize(2)*isize(3))
  write (6, '("Estimated memory required to store the entire pixel grid in Mb : ",f15.10)') MEMOID/1000000.0d0
  write (6, '("Estimated memory required to store the entire pixel grid in Mo : ",f15.10)') MEMOID/8000000.0d0

  CONTAINS

     INTEGER FUNCTION GUESS_GRID_MEMORY_SIZE (abc)

       INTEGER, INTENT(IN) :: abc

       allocate(THEPIX(1), STAT=ERR)
       allocate(THEPIX(1)%ATOM_ID(50), STAT=ERR)

       GUESS_GRID_MEMORY_SIZE = storage_size(THEPIX)*abc

       deallocate(THEPIX(1)%ATOM_ID)
       deallocate(THEPIX)

     END FUNCTION

#endif

END FUNCTION

#ifdef DEBUG
SUBROUTINE PRINT_PIXEL_GRID ()

USE PARAMETERS

IMPLICIT NONE

INTEGER :: pix, pid, cid, did, eid, ai, bi, ci
INTEGER :: init_a, end_a
INTEGER :: init_b, end_b
INTEGER :: init_c, end_c
INTEGER, DIMENSION(3) :: dim
INTEGER, DIMENSION(3,3,3) :: shift
LOGICAL :: boundary, keep_it

dim(1) = -1
dim(2) = 0
dim(3) = 1
call pix_info (isize(1), isize(2), isize(3))

allocate(THEPIX(abc))

pix=1

do ci=1, isize(3)
  do bi=1, isize(2)
    do ai=1, isize(1)
      shift(:,:,:) = 0
      if (PBC .and. .not.CALC_PRINGS) then
        call SET_SHIFT (shift, ai, bi, ci, isize(1), isize(2), isize(3), ab, abc)
        boundary=.false.
      else
        if (ai.eq.1 .or. ai.eq.isize(1)) boundary=.true.
        if (bi.eq.1 .or. bi.eq.isize(2)) boundary=.true.
        if (ci.eq.1 .or. ci.eq.isize(3)) boundary=.true.
      endif
      init_a = 1
      end_a = 3
      if (isize(1) .eq. 1) then
        init_a = 2
        end_a = 2
      endif
      init_b = 1
      end_b = 3
      if (isize(2) .eq. 1) then
        init_b = 2
        end_b = 2
      endif
      init_c = 1
      end_c = 3
      if (isize(3) .eq. 1) then
        init_c = 2
         end_c = 2
       endif
      pid = 0
      do cid=init_a, end_a
        do did=init_b, end_b
          do eid=init_c, end_c
            keep_it=.true.
            if (boundary) then
              if (ai.eq.1 .and. cid.eq.1) then
                keep_it=.false.
              else if (ai.eq.isize(1) .and. cid.eq.3) then
                keep_it=.false.
              else if (bi.eq.1 .and. did.eq.1) then
                keep_it=.false.
              else if (bi.eq.isize(2) .and. did.eq.3) then
                keep_it=.false.
              else if (ci.eq.1 .and. eid.eq.1) then
                keep_it=.false.
              else if (ci.eq.isize(3) .and. eid.eq.3) then
                keep_it=.false.
              endif
            endif
            if (keep_it) then
              pid = pid + 1
              THEPIX(pix)%IDNEIGH(pid) = pix + dim(cid) + dim(did) * isize(1) + dim(eid) * ab
              THEPIX(pix)%IDNEIGH(pid) = THEPIX(pix)%IDNEIGH(pid) + shift (cid,did,eid)
            endif
          enddo
        enddo
      enddo
      THEPIX(pix)%NEIGHBOR = pid
      call send_pix_info (pix-1, THEPIX(pix)%IDNEIGH, pid)
      ! call PRINT_THIS_PIXEL (abc, THEPIX, THEPIX(pix), pix, .false.)
      pix = pix + 1
    enddo
  enddo
enddo

END SUBROUTINE
#endif

LOGICAL FUNCTION DISTMTX(NAN, LAN, LOOKNGB, UPNGB)

!
! Compute the distance matrix - neighbors table
!

USE PARAMETERS
USE MENDELEIEV

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER, INTENT(IN) :: NAN
INTEGER, DIMENSION(NAN), INTENT(IN) :: LAN
LOGICAL, INTENT(IN) :: LOOKNGB, UPNGB
INTEGER :: SAT
INTEGER :: RA, RB, RC, RD, RF, RG, RH, RI, RJ, RK, RL, RM
INTEGER :: RN, RO, RP, RQ, RS, RT, RU, RV, RW, RX, RY, RZ
INTEGER :: A_START, A_END
INTEGER :: pix
DOUBLE PRECISION :: Dik
DOUBLE PRECISION :: MAXBD, MINBD
INTEGER, DIMENSION(:), ALLOCATABLE :: BA, BB
INTEGER, DIMENSION(:), ALLOCATABLE :: CA, CB
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: XC, YC, ZC
LOGICAL :: CALCMAT=.false.
! Error message info !
LOGICAL :: TOOM=.false.
INTEGER :: TOOI
LOGICAL :: PIXR=.false.
INTEGER :: POUT
!
LOGICAL :: IS_CLONE, boundary, keep_it
INTEGER, DIMENSION(3) :: pixpos
DOUBLE PRECISION, DIMENSION(3) :: XYZ, UVW
INTEGER :: pid, cid, did, eid, fid, ai, bi, ci
INTEGER :: init_a, end_a
INTEGER :: init_b, end_b
INTEGER :: init_c, end_c
INTEGER, DIMENSION(3) :: dim = (/-1, 0, 1/)
INTEGER, DIMENSION(3,3,3) :: shift
#ifdef OPENMP
INTEGER :: NUMTH
INTEGER :: THREAD_NUM
INTEGER :: ATOM_START, ATOM_END
INTEGER, DIMENSION(:), ALLOCATABLE :: ATPIX
LOGICAL :: DOATOMS
#endif

INTERFACE
  INTEGER FUNCTION GETNBX (NP, NPS)
    INTEGER, INTENT(IN) :: NP, NPS
  END FUNCTION
  LOGICAL FUNCTION dvtbox (ST, NAS, NAT, NPOS)
    INTEGER, INTENT(IN) :: ST, NAS, NAT
    DOUBLE PRECISION, DIMENSION(NAT,3), INTENT(INOUT) :: NPOS
  END FUNCTION
#ifdef OPENMP
  INTEGER FUNCTION GET_THREAD_START (NOBJ, NTHREADS, THREAD_ID)
    INTEGER, INTENT(IN) :: NOBJ, NTHREADS, THREAD_ID
  END FUNCTION
  INTEGER FUNCTION GET_THREAD_END (NOBJ, NTHREADS, THREAD_ID)
    INTEGER, INTENT(IN) :: NOBJ, NTHREADS, THREAD_ID
  END FUNCTION
#endif
  DOUBLE PRECISION FUNCTION CALCDIJ (R12, AT1, AT2, STEP_1, STEP_2, SID)
    USE PARAMETERS
    DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: R12
    INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
  END FUNCTION
  DOUBLE PRECISION FUNCTION SPHERES_CAPS_VOLUMES (DAB, RAP, RBP)
    DOUBLE PRECISION, INTENT(IN) :: DAB, RAP, RBP
  END FUNCTION
END INTERFACE

#ifdef DEBUG
  write (6, '("In DMTX:: LOOKNGB= ",L1, ", NOHOM= ",L1," PBC= ",L1,", CUBIC= ",L1)') LOOKNGB, NOHP, PBC, OVERALL_CUBIC
#endif

if (allocated(Gr_TMP)) deallocate(Gr_TMP)
allocate(Gr_TMP(NSP,NSP), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="VOISJ"
  ALC=.true.
  DISTMTX=.false.
  goto 001
endif

do i=1, NSP
  do j=1, NSP
    if (Gr_CUT(i,j) .gt. Gr_cutoff) then
      Gr_TMP(i,j)=Gr_cutoff
    else
      Gr_TMP(i,j)=Gr_CUT(i,j)
    endif
    if (.not.LOOKNGB) Gr_TMP(i,j) = Gr_TMP(i,j) + 50.0
  enddo
enddo

NBX = GETNBX (NAN, NS)

if (PBC) then
  NNA = NBX**3*NAN
else
  NNA = NAN
endif

if (LOOKNGB) then
  if (allocated(VOISJ)) deallocate(VOISJ)
  allocate(VOISJ(MAXN,NNA,NS), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="VOISJ"
    ALC=.true.
    DISTMTX=.false.
    goto 001
  endif
  if (allocated(CONTJ)) deallocate(CONTJ)
  allocate(CONTJ(NNA,NS), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="CONTJ"
    ALC=.true.
    DISTMTX=.false.
    goto 001
  endif
  CONTJ(:,:)=0
  VOISJ(:,:,:)=0
endif

if (LOOKNGB) then
  MAXBD=0.0d0
  MINBD=10.0d0
endif

if (CALC_PRINGS) then
  A_START = 1
  A_END = NNA
else
  A_START = NAN *(NBX**3 - 1)/2 + 1
  A_END = A_START + NAN - 1
endif

ab = isize(1)*isize(2)
abc = ab*isize(3)

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

TOOM=.false.

#ifdef DEBUG
  call PRINT_PIXEL_GRID ()
#endif

if (DOATOMS) then
! OpemMP on Atoms
  if (NAN.lt.NUMTH) NUMTH=NAN
  DISTMTX=.true.
  if (NBX.gt.1) then
    allocate(POA(NNA,3), STAT=ERR)
    if (ERR .ne. 0) then
      ALC_TAB="POA"
      ALC=.true.
      DISTMTX=.false.
      goto 001
    endif
  endif

  do SAT=1, NS

    if (NBX.gt.1) then
      if (.not.dvtbox(SAT, NA, NNA, POA)) then
        DISTMTX=.false.
        goto 001
      endif
    endif

    if (allocated(THEPIX)) deallocate(THEPIX)
    allocate(THEPIX(abc), STAT=ERR)
    if (ERR .ne. 0) then
      ALC_TAB="THEPIX"
      ALC=.true.
      DISTMTX = .false.
      goto 001
    endif

    if (allocated(ATPIX)) deallocate(ATPIX)
    allocate(ATPIX(NNA), STAT=ERR)
    if (ERR .ne. 0) then
      ALC_TAB="ATPIX"
      ALC=.true.
      DISTMTX = .false.
      goto 001
    endif

    RA = INT(MAXN*2.5)
    ! 1) LOOKNGB: "RA" is the number of atom(s) in one pixel
    ! with MAXN = 20, the '50' value seems high enough
    ! to ensure to catch every atom in the pixel
    ! this is ok providing that no funny cutoff is used
    ! 2) .not.LOOKNGB: looking for tetrahedra, requires larger value:
    if (.not.LOOKNGB) RA = MAXN*10
    if (.not.LOOKNGB .or. abc .le. 5) RA = min(RA*5, NNA)
    do RB=1, abc
      THEPIX(RB)%ATOMS=0
      THEPIX(RB)%TOCHECK=.false.
      THEPIX(RB)%CHECKED=.false.
      allocate(THEPIX(RB)%ATOM_ID(RA), STAT=ERR)
      if (ERR .ne. 0) then
        ALC_TAB="THEPIX%ATOM_ID"
        ALC=.true.
        DISTMTX = .false.
        goto 001
      endif
      THEPIX(RB)%IDNEIGH(:) = 0
      THEPIX(RB)%ATOM_ID(:) = 0
    enddo

    do RA=1, NNA
      if (.not. PBC) then
        do RB=1, 3
          if (isize(RB) .eq. 1) then
            pixpos(RB) = 0
          else
            pixpos(RB) = INT((FULLPOS(RA,RB,SAT) - pmin(RB))*CUTF)
            if (pixpos(RB) .eq. isize(RB)) pixpos(RB) = pixpos(RB)-1
          endif
        enddo
      else
        if (NBX .gt. 1) then
          if (NCELLS.gt.1) then
            XYZ = MATMUL(POA(RA,:), THE_BOX(SAT)%carttofrac/NBX)
          else
            XYZ = MATMUL(POA(RA,:), THE_BOX(1)%carttofrac/NBX)
          endif
        else
          if (NCELLS.gt.1) then
            XYZ = MATMUL(FULLPOS(RA,:,SAT), THE_BOX(SAT)%carttofrac)
          else
            XYZ = MATMUL(FULLPOS(RA,:,SAT), THE_BOX(1)%carttofrac)
          endif
        endif
        do RB=1, 3
          UVW(RB) = XYZ(RB) - floor(XYZ(RB))
          pixpos(RB) = INT(UVW(RB)*isize(RB))
          ! if (pixpos(RD) .eq. isize(RD)) pixpos(RD) = pixpos(RD)-1
        enddo
      endif
      pix = pixpos(1) + pixpos(2)*isize(1) + pixpos(3)*ab + 1
      if (pix.gt.abc .or. pix.le.0) then
        write (6, '("Thread(id)= ",i4,", MD step= ",i8)') THREAD_NUM, SAT
        if (NBX .gt. 1) then
          write (6, '("at= ",i7,", pos(x)= ",f15.10,", pos(y)= ",f15.10,", pos(z)= ",f15.10)') RA, POA(RA,:)
        else
          write (6, '("at= ",i7,", pos(x)= ",f15.10,", pos(y)= ",f15.10,", pos(z)= ",f15.10)') RA, FULLPOS(RA,:,SAT)
        endif
        if (PBC) then
          write (6, '("at= ",i7,", fra(x)= ",f15.10,", fra(y)= ",f15.10,", fra(z)= ",f15.10)') RA, XYZ
          write (6, '("at= ",i7,", cor(x)= ",f15.10,", cor(y)= ",f15.10,", cor(z)= ",f15.10)') RA, UVW
        endif
        write (6, '("at= ",i7,", pixpos(x)= ",i4,", pixpos(y)= ",i4,", pixpos(z)= ",i4)') RA, pixpos
        write (6, '("at= ",i7,", pixpos= ",i10)') RA, pix
        PIXR=.true.
        POUT=pix
        DISTMTX=.false.
        goto 001
      else
        if ((PBC .and. RA.ge.A_START .and. RA.le.A_END) .or. .not.PBC) then
          if (.not.THEPIX(pix)%TOCHECK) then
            ai = pixpos(1) + 1
            bi = pixpos(2) + 1
            ci = pixpos(3) + 1
            pid = 0
            shift(:,:,:) = 0
            boundary=.false.
            if (PBC .and. .not.CALC_PRINGS) then
              call SET_SHIFT (shift, ai, bi, ci, isize(1), isize(2), isize(3), ab, abc)
            else
              if (ai.eq.1 .or. ai.eq.isize(1)) boundary=.true.
              if (bi.eq.1 .or. bi.eq.isize(2)) boundary=.true.
              if (ci.eq.1 .or. ci.eq.isize(3)) boundary=.true.
            endif
            init_a = 1
            end_a = 3
            if (isize(1) .eq. 1) then
              init_a = 2
              end_a = 2
            endif

            init_b = 1
            end_b = 3
            if (isize(2) .eq. 1) then
              init_b = 2
              end_b = 2
            endif

            init_c = 1
            end_c = 3
            if (isize(3) .eq. 1) then
              init_c = 2
              end_c = 2
            endif

            do cid=init_a, end_a
              do did=init_b, end_b
                 do eid=init_c, end_c
                   keep_it=.true.
                   if (boundary) then
                     if (ai.eq.1 .and. cid.eq.1) then
                       keep_it=.false.
                     else if (ai.eq.isize(1) .and. cid.eq.3) then
                       keep_it=.false.
                     else if (bi.eq.1 .and. did.eq.1) then
                       keep_it=.false.
                     else if (bi.eq.isize(2) .and. did.eq.3) then
                       keep_it=.false.
                     else if (ci.eq.1 .and. eid.eq.1) then
                       keep_it=.false.
                     else if (ci.eq.isize(3) .and. eid.eq.3) then
                       keep_it=.false.
                     endif
                   endif
                   if (keep_it) then
                     pid = pid+1
                     THEPIX(pix)%IDNEIGH(pid) = pix + dim(cid) + dim(did) * isize(1) + dim(eid) * ab + shift (cid,did,eid)
                   endif
                enddo
              enddo
            enddo
            THEPIX(pix)%NEIGHBOR = pid
          endif
          THEPIX(pix)%TOCHECK=.true.
        endif
        THEPIX(pix)%ATOMS = THEPIX(pix)%ATOMS + 1
        THEPIX(pix)%ATOM_ID(THEPIX(pix)%ATOMS) = RA
      endif
      ATPIX(RA) = pix
    enddo

    RA = 0
    RB = 0
    !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
    !$OMP& PRIVATE(THREAD_NUM, ATOM_START, ATOM_END, &
    !$OMP& RC, RD, RF, RG, RH, RI, RJ, RK, RL, RM, &
    !$OMP& RN, RO, RP, RQ, RS, RT, RU, RV, RW, RX, RY, RZ, ERR, ai, bi, ci, &
    !$OMP& IS_CLONE, CALCMAT, Dij, Rij, Dik, BA, BB, CA, CB, XC, YC, ZC) &
    !$OMP& SHARED(NUMTH, SAT, NS, NA, NNA, NAN, LAN, NSP, LOOKNGB, UPNGB, DISTMTX, &
    !$OMP& NBX, PBC, THE_BOX, NCELLS, A_START, A_END, NOHP, MAXN, CUTF, &
    !$OMP& POA, FULLPOS, Gr_TMP, CALC_PRINGS, MAXBD, MINBD, CONTJ, VOISJ, RA, RB, &
    !$OMP& LA_COUNT, CORTA, CORNERA, EDGETA, EDGEA, DEFTA, DEFA, &
    !$OMP& ALC, ALC_TAB, TOOM, TOOI, THEPIX, ATPIX)
    THREAD_NUM = OMP_GET_THREAD_NUM ()
    ATOM_START = GET_THREAD_START (NNA, NUMTH, THREAD_NUM)
    ATOM_END = GET_THREAD_END (NNA, NUMTH, THREAD_NUM)

    do RC=ATOM_START, ATOM_END
      RD = ATPIX(RC)
      RF = RC - (RC/NAN)*NAN
      if (RF .eq. 0) RF=NAN
      RG = LAN(RF)
      do RH=1, THEPIX(RD)%NEIGHBOR
        RI = THEPIX(RD)%IDNEIGH(RH)
        RJ = THEPIX(RI)%ATOMS
        do RK=1, RJ
          RL = THEPIX(RI)%ATOM_ID(RK)
          if (RL .ne. RC) then
            if ((RC.ge.A_START .and. RC.le.A_END) .or. (RL.ge.A_START .and. RL.le.A_END)) then
              RM = RL - (RL/NAN)*NAN
              if (RM .eq. 0) RM=NAN
              RN = LAN(RM)
              if (LOOKNGB) then
                if (.not.PBC .or. NBX.gt.1) then
                  if (((RC.ge.A_START .and. RC.le.A_END) .and. (RL.ge.A_START .and. RL.le.A_END)) .or. .not.PBC) then
                    IS_CLONE=.false.
                  else
                    IS_CLONE=.true.
                  endif
                else
                  IS_CLONE=.false.
                endif
                if (RG.ne.RN .or. .not.NOHP) then
                  CALCMAT=.true.
                else
                  CALCMAT=.false.
                endif
                if (CALCMAT) then
                  Dij=0.0d0
                  if (NBX.gt.1) then
                    do RP=1,3
                      Rij(RP) = POA(RC,RP) - POA(RL,RP)
                      Dij=Dij+Rij(RP)**2
                    enddo
                  else
                    Dik=0.0d0
                    do RP=1,3
                      Rij(RP) = FULLPOS(RF,RP,SAT) - FULLPOS(RM,RP,SAT)
                      Dik=Dik+Rij(RP)**2
                    enddo
                    if (NCELLS .gt. 1) then
                      Dij = CALCDIJ (Rij,RF,RM,SAT,SAT,SAT)
                    else
                      Dij = CALCDIJ (Rij,RF,RM,SAT,SAT,1)
                    endif
                    if (Dik-Dij .gt.0.01d0) then
                      IS_CLONE=.true.
                    endif
                  endif
                  if (Dij .le. Gr_TMP(RG,RN)) then
                    !$OMP ATOMIC
                    MAXBD=max(Dij,MAXBD)
                    !$OMP ATOMIC
                    MINBD=min(Dij,MINBD)
                    if (CALC_PRINGS) then
                      RP = RC
                      RQ = RL
                    else
                      RP = RF
                      RQ = RM
                    endif
                    CONTJ(RP,SAT)=CONTJ(RP,SAT)+1
                    if (CONTJ(RP,SAT) .gt. MAXN) then
                      TOOM=.true.
                      TOOI=RP
                      DISTMTX=.false.
                      goto 002
                    endif
                    VOISJ(CONTJ(RP,SAT),RP,SAT)=RQ
                    if (.not.CALC_PRINGS .and. UPNGB) then
                      if (IS_CLONE) then
                        VOISJ(CONTJ(RP,SAT),RP,SAT)=-RQ
                        !$OMP ATOMIC
                        RB = RB + 1
                      else
                        !$OMP ATOMIC
                        RA = RA + 1
                      endif
                    endif
                  endif
                endif
              else
                if (RG .eq. RN) then
                  do RO=1, NSP
                    if (LA_COUNT(RF,RO,SAT).eq.4 .and. CONTJ(RF,SAT).eq.4) then
                      if (LA_COUNT(RM,RO,SAT).eq.4 .and. CONTJ(RM,SAT).eq.4) then
                        RP=0
                        do RQ=1, 4
                          RS = VOISJ(RQ,RF,SAT)
                          do RT=1, CONTJ(RS,SAT)
                            if (VOISJ(RT,RS,SAT) .eq. RM) RP=RP+1
                          enddo
                        enddo
                        if (RP.eq.1) then
                          !$OMP ATOMIC
                          CORTA(RG,RO)=CORTA(RG,RO)+1
                          if (NS .gt. 1) then
                            !$OMP ATOMIC
                            CORNERA(RG,RO,SAT)=CORNERA(RG,RO,SAT)+1
                          endif
                        elseif (RP.eq.2) then
                          !$OMP ATOMIC
                          EDGETA(RG,RO)=EDGETA(RG,RO)+1
                          if (NS .gt. 1) then
                            !$OMP ATOMIC
                            EDGEA(RG,RO,SAT)=EDGEA(RG,RO,SAT)+1
                          endif
                        elseif (RP.ge.3) then
                          !$OMP ATOMIC
                          CORTA(RG,RO)=CORTA(RG,RO)+1
                          !$OMP ATOMIC
                          EDGETA(RG,RO)=EDGETA(RG,RO)+1
                          !$OMP ATOMIC
                          DEFTA(RG,RO)=DEFTA(RG,RO)+1
                          if (NS .gt. 1) then
                            !$OMP ATOMIC
                            CORNERA(RG,RO,SAT)=CORNERA(RG,RO,SAT)+1
                            !$OMP ATOMIC
                            EDGEA(RG,RO,SAT)=EDGEA(RG,RO,SAT)+1
                            !$OMP ATOMIC
                            DEFA(RG,RO,SAT)=DEFA(RG,RO,SAT)+1
                          endif
                        endif
                      endif
                    endif
                  enddo
                endif
              endif
            endif
          endif
        enddo
      enddo
    enddo

    002 continue
    !$OMP END PARALLEL

    do RB=1, abc
      if (allocated(THEPIX(RB)%ATOM_ID)) deallocate(THEPIX(RB)%ATOM_ID)
    enddo
    if (allocated(THEPIX)) deallocate(THEPIX)
    if (allocated(ATPIX)) deallocate(ATPIX)
    if (.not.DISTMTX) then
      goto 001
    endif
    if (.not.LOOKNGB) then
      do RC=1, NSP
        do RD=1, NSP
          CORTA(RC,RD)=CORTA(RC,RD)/2
          EDGETA(RC,RD)=EDGETA(RC,RD)/2
          DEFTA(RC,RD)=DEFTA(RC,RD)/2
          if (NS .gt. 1) then
            CORNERA(RC,RD,SAT)=CORNERA(RC,RD,SAT)/2
            EDGEA(RC,RD,SAT)=EDGEA(RC,RD,SAT)/2
            DEFA(RC,RD,SAT)=DEFA(RC,RD,SAT)/2
           endif
         enddo
      enddo
    endif
    if (.not.CALC_PRINGS .and.UPNGB) then
      if (RA .gt. 0) then
        if (allocated(BA)) deallocate(BA)
        allocate(BA(RA), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="BA"
          ALC=.true.
          DISTMTX=.false.
          goto 001
        endif
        if (allocated(BB)) deallocate(BB)
        allocate(BB(RA), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="BB"
          ALC=.true.
          DISTMTX=.false.
          goto 001
        endif
      endif
      if (RB .gt. 0) then
        if (allocated(CA)) deallocate(CA)
        allocate(CA(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="CA"
          ALC=.true.
          DISTMTX=.false.
          goto 001
        endif
        if (allocated(CB)) deallocate(CB)
        allocate(CB(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="CB"
          ALC=.true.
          DISTMTX=.false.
          goto 001
        endif
        if (allocated(XC)) deallocate(XC)
        allocate(XC(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="XC"
          ALC=.true.
          DISTMTX=.false.
          goto 001
        endif
        if (allocated(YC)) deallocate(YC)
        allocate(YC(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="YC"
          ALC=.true.
          DISTMTX=.false.
          goto 001
        endif
        if (allocated(ZC)) deallocate(ZC)
        allocate(ZC(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="ZC"
          ALC=.true.
          DISTMTX=.false.
          goto 001
        endif
      endif
      if (RA.gt.0 .or. RB.gt.0) then
        RA=0
        RB=0
        do RC=1, NNA
          do RD=1, CONTJ(RC,SAT)
            RF = VOISJ(RD,RC,SAT)
            if (RF .gt. 0) then
              if (RF.gt.RC) then
                RA=RA+1
                BA(RA) = RC
                BB(RA) = RF
              endif
            else
              VOISJ(RD,RC,SAT) = -RF
              RF=-RF
              if (RF.gt.RC) then
                RB=RB+1
                CA(RB) = RC
                CB(RB) = RF
                if (NCELLS .gt. 1) then
                  call CALCRIJ (RC,RF,SAT,SAT,SAT)
                else
                  call CALCRIJ (RC,RF,SAT,SAT,1)
                endif
                XC(RB) = Rij(1)
                YC(RB) = Rij(2)
                ZC(RB) = Rij(3)
              endif
            endif
          enddo
        enddo
      endif
      call update_bonds (0, SAT-1, RA, BA, BB, XC, YC, ZC)
      call update_bonds (1, SAT-1, RB, CA, CB, XC, YC, ZC)
      do RC=1, NNA
        call update_atom_neighbors (SAT-1, RC-1, CONTJ(RC,SAT))
        do RD=1, CONTJ(RC,SAT)
          call update_this_neighbor (SAT-1, RC-1, RD-1, VOISJ(RD,RC,SAT))
        enddo
      enddo
    endif

  enddo ! En MD steps loop

else

  ! OpemMP on MD steps
  DISTMTX=.true.
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(THEPIX, SAT, pix, pixpos, XYZ, UVW, shift, ai, bi, ci, &
  !$OMP& RA, RB, RC, RD, RF, RG, RH, RI, RJ, RK, RL, RM, &
  !$OMP& RN, RO, RP, RQ, RS, RT, RU, RV, RW, RX, RY, RZ, ERR, &
  !$OMP& IS_CLONE, CALCMAT, Dij, Rij, Dik, BA, BB, CA, CB, XC, YC, ZC, POA, &
  !$OMP& pid, cid, did, eid, fid, init_a, end_a, init_b, end_b, init_c, end_c, boundary, keep_it) &
  !$OMP& SHARED(NUMTH, NS, NA, NNA, NAN, LAN, NSP, LOOKNGB, UPNGB, DISTMTX, &
  !$OMP& NBX, PBC, THE_BOX, NCELLS, isize, pmin, pmax, abc, ab, A_START, A_END, NOHP, MAXN, CUTF, &
  !$OMP& FULLPOS, CONTJ, VOISJ, Gr_TMP, CALC_PRINGS, MAXBD, MINBD, &
  !$OMP& LA_COUNT, CORTA, CORNERA, EDGETA, EDGEA, DEFTA, DEFA, &
  !$OMP& ALC, ALC_TAB, TOOM, TOOI, PIXR, POUT, dim)
#endif

  if (allocated(THEPIX)) deallocate(THEPIX)
  allocate(THEPIX(abc), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="THEPIX"
    ALC=.true.
    DISTMTX = .false.
#ifdef OPENMP
    goto 006
#else
    goto 001
#endif
  endif

  RA = INT(MAXN*2.5)
  ! 1) LOOKNGB: "RA" is the number of atom(s) in one pixel
  ! with MAXN = 20, the '50' value seems high enough
  ! to ensure to catch every atom in the pixel
  ! this is ok providing that no funny cutoff is used
  ! 2) .not.LOOKNGB: looking for tetrahedra, requires larger value:
  if (.not.LOOKNGB) RA = MAXN*10
  if (.not.LOOKNGB .or. abc .le. 5) RA = min(RA*5, NNA)
  do RB=1, abc
    THEPIX(RB)%ATOMS=0
    THEPIX(RB)%TOCHECK=.false.
    THEPIX(RB)%CHECKED=.false.
    allocate(THEPIX(RB)%ATOM_ID(RA), STAT=ERR)
    if (ERR .ne. 0) then
      ALC_TAB="THEPIX%ATOM_ID"
      ALC=.true.
      DISTMTX = .false.
#ifdef OPENMP
      goto 006
#else
      goto 001
#endif
    endif
    THEPIX(RB)%IDNEIGH(:) = 0
    THEPIX(RB)%ATOM_ID(:) = 0
  enddo
#ifdef OPENMP
  !$OMP DO SCHEDULE(STATIC,NS/NUMTH)
  do SAT=1, NS

    if (.not.DISTMTX) goto 007
#else
  if (allocated(POA)) deallocate (POA)
  allocate(POA(NNA,3), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="POA"
    ALC=.true.
    DISTMTX=.false.
    goto 001
  endif
  do SAT=1, NS
#endif
    if (NBX.gt.1) then
#ifdef OPENMP
      if (allocated(POA)) deallocate (POA)
      allocate(POA(NNA,3), STAT=ERR)
      if (ERR .ne. 0) then
        ALC_TAB="POA"
        ALC=.true.
        DISTMTX=.false.
        goto 007
      endif
#endif
      if (.not.dvtbox(SAT, NA, NNA, POA)) then
        DISTMTX=.false.
#ifdef OPENMP
        goto 007
#else
        goto 001
#endif
      endif
    endif

    ! Clean THEPIX for each and every MD step
    do RA=1, abc
      THEPIX(RA)%ATOMS=0
      THEPIX(RA)%TOCHECK=.false.
      THEPIX(RA)%CHECKED=.false.
      THEPIX(RA)%IDNEIGH(:) = 0
      THEPIX(RA)%ATOM_ID(:) = 0
    enddo
    do RA=1, NNA
      if (.not. PBC) then
        do RB=1, 3
          if (isize(RB) .eq. 1) then
            pixpos(RB) = 0
          else
            pixpos(RB) = INT((FULLPOS(RA,RB,SAT) - pmin(RB))*CUTF)
            if (pixpos(RB) .eq. isize(RB)) pixpos(RB) = pixpos(RB)-1
          endif
        enddo
      else
        if (NBX .gt. 1) then
          if (NCELLS.gt.1) then
            XYZ = MATMUL(POA(RA,:), THE_BOX(SAT)%carttofrac/NBX)
          else
            XYZ = MATMUL(POA(RA,:), THE_BOX(1)%carttofrac/NBX)
          endif
        else
          if (NCELLS.gt.1) then
            XYZ = MATMUL(FULLPOS(RA,:,SAT), THE_BOX(SAT)%carttofrac)
          else
            XYZ = MATMUL(FULLPOS(RA,:,SAT), THE_BOX(1)%carttofrac)
          endif
        endif
        do RB=1, 3
          UVW(RB) = XYZ(RB) - floor(XYZ(RB))
          pixpos(RB) = INT(UVW(RB)*isize(RB))
          ! if (pixpos(RB) .eq. isize(RB)) pixpos(RB) = pixpos(RB)-1
        enddo
      endif
      pix = pixpos(1) + pixpos(2)*isize(1) + pixpos(3)*ab + 1
      if (pix.gt.abc .or. pix.le.0) then
#ifdef OPENMP
        write (6, '("Thread(id)= ",i4,", MD step= ",i8)') OMP_GET_THREAD_NUM(), SAT
#else
        write (6, '("MD step= ",i8)') SAT
#endif
        if (NBX.gt.1) then
          write (6, '("at= ",i7,", pos(x)= ",f15.10,", pos(y)= ",f15.10,", pos(z)= ",f15.10)') RA, POA(RA,:)
        else
          write (6, '("at= ",i7,", pos(x)= ",f15.10,", pos(y)= ",f15.10,", pos(z)= ",f15.10)') RA, FULLPOS(RA,:,SAT)
        endif
        if (PBC) then
          write (6, '("at= ",i7,", fra(x)= ",f15.10,", fra(y)= ",f15.10,", fra(z)= ",f15.10)') RA, XYZ
          write (6, '("at= ",i7,", cor(x)= ",f15.10,", cor(y)= ",f15.10,", cor(z)= ",f15.10)') RA, UVW
        endif
        write (6, '("at= ",i7,", pixpos(x)= ",i4,", pixpos(y)= ",i4,", pixpos(z)= ",i4)') RA, pixpos
        write (6, '("at= ",i7,", pixpos= ",i10)') RA, pix
        PIXR=.true.
        POUT=pix
        DISTMTX=.false.
#ifdef OPENMP
        goto 007
#else
        goto 001
#endif
      else
        if ((PBC .and. RA.ge.A_START .and. RA.le.A_END) .or. .not.PBC) then
          if (.not.THEPIX(pix)%TOCHECK) then
            dim(1) = -1
            dim(2) = 0
            dim(3) = 1
            ai = pixpos(1) + 1
            bi = pixpos(2) + 1
            ci = pixpos(3) + 1
            pid = 0
            shift(:,:,:) = 0
            boundary=.false.
            if (PBC .and. .not.CALC_PRINGS) then
              call SET_SHIFT (shift, ai, bi, ci, isize(1), isize(2), isize(3), ab, abc)
            else
              if (ai.eq.1 .or. ai.eq.isize(1)) boundary=.true.
              if (bi.eq.1 .or. bi.eq.isize(2)) boundary=.true.
              if (ci.eq.1 .or. ci.eq.isize(3)) boundary=.true.
            endif
            init_a = 1
            end_a = 3
            if (isize(1) .eq. 1) then
              init_a = 2
              end_a = 2
            endif

            init_b = 1
            end_b = 3
            if (isize(2) .eq. 1) then
              init_b = 2
              end_b = 2
            endif

            init_c = 1
            end_c = 3
            if (isize(3) .eq. 1) then
              init_c = 2
              end_c = 2
            endif

            do cid=init_a, end_a
              do did=init_b, end_b
                 do eid=init_c, end_c
                   keep_it=.true.
                   if (boundary) then
                     if (ai.eq.1 .and. cid.eq.1) then
                       keep_it=.false.
                     else if (ai.eq.isize(1) .and. cid.eq.3) then
                       keep_it=.false.
                     else if (bi.eq.1 .and. did.eq.1) then
                       keep_it=.false.
                     else if (bi.eq.isize(2) .and. did.eq.3) then
                       keep_it=.false.
                     else if (ci.eq.1 .and. eid.eq.1) then
                       keep_it=.false.
                     else if (ci.eq.isize(3) .and. eid.eq.3) then
                       keep_it=.false.
                     endif
                   endif
                   if (keep_it) then
                     pid = pid+1
                     THEPIX(pix)%IDNEIGH(pid) = pix + dim(cid) + dim(did) * isize(1) + dim(eid) * ab + shift (cid,did,eid)
                   endif
                enddo
              enddo
            enddo
            THEPIX(pix)%NEIGHBOR = pid
          endif
          THEPIX(pix)%TOCHECK=.true.
        endif
        THEPIX(pix)%ATOMS = THEPIX(pix)%ATOMS + 1
        THEPIX(pix)%ATOM_ID(THEPIX(pix)%ATOMS) = RA
      endif
    enddo

    RA = 0
    RB = 0
    do RC=1, abc
      if (THEPIX(RC)%TOCHECK) then
        RD = THEPIX(RC)%ATOMS
        if (RD .gt. 0) then
          do RF=1, THEPIX(RC)%NEIGHBOR
            RG = THEPIX(RC)%IDNEIGH(RF)
            if (.not.THEPIX(RG)%CHECKED) then
              RH = THEPIX(RG)%ATOMS
              if (RH .gt. 0) then
                if (RC .eq. RG) then
                  RI=1
                else
                  RI=0
                endif
                do RJ=1, RD-RI
                  RK = THEPIX(RC)%ATOM_ID(RJ)
                  do RM=RI*RJ+1, RH
                    RN = THEPIX(RG)%ATOM_ID(RM)
                    if ((RK.ge.A_START .and. RK.le.A_END) .or. (RN.ge.A_START .and. RN.le.A_END)) then
                      RP=RK - (RK/NAN)*NAN
                      RQ=RN - (RN/NAN)*NAN
                      if (RP .eq. 0) RP=NAN
                      if (RQ .eq. 0) RQ=NAN
                      if (RP .ne. RQ) then
                        RL=LAN(RP)
                        RO=LAN(RQ)
                        if (LOOKNGB) then
                          if (.not.PBC .or. NBX.gt.1) then
                            if (((RK.ge.A_START .and. RK.le.A_END) .and. (RN.ge.A_START .and. RN.le.A_END)) .or. .not.PBC) then
                              IS_CLONE=.false.
                            else
                              IS_CLONE=.true.
                            endif
                          else
                            IS_CLONE=.false.
                          endif
                          if (RL.ne.RO .or. .not.NOHP) then
                            CALCMAT=.true.
                          else
                            CALCMAT=.false.
                          endif
                          if (CALCMAT) then
                            Dij=0.0d0
                            if (NBX.gt.1) then
                              do RS=1,3
                                Rij(RS) = POA(RK,RS) - POA(RN,RS)
                                Dij=Dij+Rij(RS)**2
                              enddo
                            else
                              Dik=0.0d0
                              do RS=1,3
                                Rij(RS) = FULLPOS(RP,RS,SAT) - FULLPOS(RQ,RS,SAT)
                                Dik=Dik+Rij(RS)**2
                              enddo
                              if (NCELLS .gt. 1) then
                                Dij = CALCDIJ (Rij,RP,RQ,SAT,SAT,SAT)
                              else
                                Dij = CALCDIJ (Rij,RP,RQ,SAT,SAT,1)
                              endif
                              if (Dik-Dij .gt.0.01d0) then
                                IS_CLONE=.true.
                              endif
                            endif
                            if (Dij .le. Gr_TMP(RL,RO)) then
#ifdef OPENMP
                              !$OMP ATOMIC
#endif
                              MAXBD=max(Dij,MAXBD)
#ifdef OPENMP
                              !$OMP ATOMIC
#endif
                              MINBD=min(Dij,MINBD)
                              if (CALC_PRINGS) then
                                RT = RK
                                RU = RN
                              else
                                RT = RP
                                RU = RQ
                              endif

                              CONTJ(RT,SAT)=CONTJ(RT,SAT)+1
                              if (CONTJ(RT,SAT) .gt. MAXN) then
                                TOOM=.true.
                                TOOI=RT
                                DISTMTX=.false.
#ifdef OPENMP
                                goto 007
#else
                                goto 001
#endif
                              endif
                              VOISJ(CONTJ(RT,SAT),RT,SAT)=RU
                              CONTJ(RU,SAT)=CONTJ(RU,SAT)+1
                              if (CONTJ(RU,SAT) .gt. MAXN) then
                                TOOM=.true.
                                TOOI=RU
                                DISTMTX=.false.
#ifdef OPENMP
                                goto 007
#else
                                goto 001
#endif
                              endif
                              VOISJ(CONTJ(RU,SAT),RU,SAT)=RT
                              if (.not.CALC_PRINGS .and. UPNGB) then
                                if (IS_CLONE) then
                                  RB = RB + 1
                                  VOISJ(CONTJ(RT,SAT),RT,SAT)=-RU
                                  VOISJ(CONTJ(RU,SAT),RU,SAT)=-RT
                                else
                                  RA = RA + 1
                                endif
                              endif
                            endif
                          endif
                        else
                          if (RL .eq. RO) then
                            do RV=1, NSP
                              if (LA_COUNT(RP,RV,SAT).eq.4 .and. CONTJ(RP,SAT).eq.4) then
                                if (LA_COUNT(RQ,RV,SAT).eq.4 .and. CONTJ(RQ,SAT).eq.4) then
                                  RW=0
                                  do RX=1, 4
                                    RY = VOISJ(RX,RP,SAT)
                                    do RZ=1, CONTJ(RY,SAT)
                                      if (VOISJ(RZ,RY,SAT) .eq. RQ) RW=RW+1
                                    enddo
                                  enddo
                                  if (RW.eq.1) then
#ifdef OPENMP
                                    !$OMP ATOMIC
#endif
                                    CORTA(RL,RV)=CORTA(RL,RV)+1
                                    if (NS .gt. 1) CORNERA(RL,RV,SAT)=CORNERA(RL,RV,SAT)+1
                                  elseif (RW.eq.2) then
#ifdef OPENMP
                                    !$OMP ATOMIC
#endif
                                    EDGETA(RL,RV)=EDGETA(RL,RV)+1
                                    if (NS .gt. 1) EDGEA(RL,RV,SAT)=EDGEA(RL,RV,SAT)+1
                                  elseif (RW.ge.3) then
#ifdef OPENMP
                                    !$OMP ATOMIC
#endif
                                    CORTA(RL,RV)=CORTA(RL,RV)+1
#ifdef OPENMP
                                    !$OMP ATOMIC
#endif
                                    EDGETA(RL,RV)=EDGETA(RL,RV)+1
#ifdef OPENMP
                                    !$OMP ATOMIC
#endif
                                    DEFTA(RL,RV)=DEFTA(RL,RV)+1
                                    if (NS .gt. 1) then
                                      CORNERA(RL,RV,SAT)=CORNERA(RL,RV,SAT)+1
                                      EDGEA(RL,RV,SAT)=EDGEA(RL,RV,SAT)+1
                                      DEFA(RL,RV,SAT)=DEFA(RL,RV,SAT)+1
                                    endif
                                  endif
                                endif
                              endif
                            enddo
                          endif
                        endif
                      endif
                    endif
                  enddo
                enddo
              endif
            endif
          enddo
          THEPIX(RC)%CHECKED=.true.
        endif
      endif
    enddo
    if (.not.CALC_PRINGS .and.UPNGB) then
      if (RA .gt. 0) then
        if (allocated(BA)) deallocate(BA)
        allocate(BA(RA), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="BA"
          ALC=.true.
          DISTMTX=.false.
#ifdef OPENMP
          goto 007
#else
          goto 001
#endif
        endif
        if (allocated(BB)) deallocate(BB)
        allocate(BB(RA), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="BB"
          ALC=.true.
          DISTMTX=.false.
#ifdef OPENMP
          goto 007
#else
          goto 001
#endif
        endif
      endif
      if (RB .gt. 0) then
        if (allocated(CA)) deallocate(CA)
        allocate(CA(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="CA"
          ALC=.true.
          DISTMTX=.false.
#ifdef OPENMP
          goto 007
#else
          goto 001
#endif
        endif
        if (allocated(CB)) deallocate(CB)
        allocate(CB(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="CB"
          ALC=.true.
          DISTMTX=.false.
#ifdef OPENMP
          goto 007
#else
          goto 001
#endif
        endif
        if (allocated(XC)) deallocate(XC)
        allocate(XC(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="XC"
          ALC=.true.
          DISTMTX=.false.
#ifdef OPENMP
          goto 007
#else
          goto 001
#endif
        endif
        if (allocated(YC)) deallocate(YC)
        allocate(YC(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="YC"
          ALC=.true.
          DISTMTX=.false.
#ifdef OPENMP
          goto 007
#else
          goto 001
#endif
        endif
        if (allocated(ZC)) deallocate(ZC)
        allocate(ZC(RB), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="ZC"
          ALC=.true.
          DISTMTX=.false.
#ifdef OPENMP
          goto 007
#else
          goto 001
#endif
        endif
      endif
      if (RA.gt.0 .or. RB.gt.0) then
        RA=0
        RB=0
        do RC=1, NNA
          do RD=1, CONTJ(RC,SAT)
            RF = VOISJ(RD,RC,SAT)
            if (RF .gt. 0) then
              if (RF.gt.RC) then
                RA=RA+1
                BA(RA) = RC
                BB(RA) = RF
              endif
            else
              VOISJ(RD,RC,SAT) = -RF
              RF=-RF
              if (RF.gt.RC) then
                RB=RB+1
                CA(RB) = RC
                CB(RB) = RF
                if (NCELLS .gt. 1) then
                  Dij = CALCDIJ (Rij,RC,RF,SAT,SAT,SAT)
                else
                  Dij = CALCDIJ (Rij,RC,RF,SAT,SAT,1)
                endif
                XC(RB) = Rij(1)
                YC(RB) = Rij(2)
                ZC(RB) = Rij(3)
              endif
            endif
          enddo
        enddo
      endif
      call update_bonds (0, SAT-1, RA, BA, BB, XC, YC, ZC)
      call update_bonds (1, SAT-1, RB, CA, CB, XC, YC, ZC)
      do RC=1, NNA
        call update_atom_neighbors (SAT-1, RC-1, CONTJ(RC,SAT))
        do RD=1, CONTJ(RC,SAT)
          call update_this_neighbor (SAT-1, RC-1, RD-1, VOISJ(RD,RC,SAT))
        enddo
      enddo
    endif

#ifdef OPENMP
    007 continue
#endif

  enddo ! En MD steps loop
#ifdef OPENMP
  !$OMP END DO NOWAIT

  006 continue
  do RB=1, abc
    if (allocated(THEPIX(RB)%ATOM_ID)) deallocate(THEPIX(RB)%ATOM_ID)
  enddo
  if (allocated(THEPIX)) deallocate(THEPIX)

  !$OMP END PARALLEL

  if (.not.DISTMTX) goto 001

endif

#endif

if (UPNGB) then
  MAXBD = sqrt(MAXBD)
  MINBD = sqrt(MINBD)
  if (MAXBD-MINBD .lt. 0.1) then
    MINBD = MINBD - 0.5;
    MAXBD = MAXBD + 0.5;
  endif
  call recup_dmin_dmax (MINBD, MAXBD)
endif
DISTMTX=.true.

001 continue

if (TOOM) call TOOMUCH(TOOI)
if (ALC) then
  call show_error ("Impossible to allocate memory !"//CHAR(0), &
                   "Function: DMTX"//CHAR(0), CHAR(9)//"Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif
if (PIXR) call PIXOUT (POUT)

if (allocated(THEPIX)) deallocate(THEPIX)
if (allocated(POA)) deallocate(POA)
if (allocated(BA)) deallocate(BA)
if (allocated(BB)) deallocate(BB)
if (allocated(CA)) deallocate(CA)
if (allocated(CB)) deallocate(CB)
if (allocated(XC)) deallocate(XC)
if (allocated(YC)) deallocate(YC)
if (allocated(ZC)) deallocate(ZC)

CONTAINS

END FUNCTION

SUBROUTINE PIXOUT (PIX)

  IMPLICIT NONE

  INTEGER, INTENT(IN) :: PIX
  CHARACTER (LEN=15) :: IDPIX
  call CHARINT(IDPIX, PIX)
  call show_error ("Pixel size error !"//CHAR(0), &
                  CHAR(9)//"Pixel out of bound(s): "//IDPIX(2:LEN_TRIM(IDPIX))//CHAR(0), &
                  "Function: DMTX"//CHAR(0))

END SUBROUTINE

SUBROUTINE TOOMUCH(ATI)

IMPLICIT NONE

INTEGER, INTENT(IN) :: ATI
CHARACTER (LEN=15) :: IDATI

call CHARINT(IDATI, ATI)

call show_error ("Too much neighbors for atom "//IDATI(2:LEN_TRIM(IDATI))//" (>20)"//CHAR(0), &
                 "Please check:"//ACHAR(10)//CHAR(9)//"  - The lattice parameters"//ACHAR(10) &
                 //CHAR(9)//"  - The bond cutoff(s)"//ACHAR(10)//CHAR(9)//"  - The atomic coordinates"//CHAR(0), &
                 "Function: DMTX"//CHAR(0))

END SUBROUTINE

INTEGER (KIND=c_int) FUNCTION rundmtx (PRINGS, VNOHP, VUP) BIND (C,NAME='rundmtx_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: PRINGS, VNOHP, VUP
LOGICAL :: DMTXOK=.false.
LOGICAL :: UPNG

INTERFACE
  LOGICAL FUNCTION DISTMTX(NAN, LAN, LOOKNGB, UPNGB)
    INTEGER, INTENT(IN) :: NAN
    INTEGER, DIMENSION(NAN), INTENT(IN) :: LAN
    LOGICAL, INTENT(IN) :: LOOKNGB, UPNGB
  END FUNCTION
END INTERFACE

CALC_PRINGS=.false.
if (PRINGS .eq. 1) CALC_PRINGS=.true.
NOHP=.false.
if (VNOHP .eq. 1) NOHP=.true.
UPNG=.false.
if (VUP .eq. 1) UPNG=.true.

DMTXOK = DISTMTX(NA, LOT, .true., UPNG)
CALC_PRINGS=.false.

if (.not. DMTXOK) then
  if (allocated(VOISJ)) deallocate(VOISJ)
  if (allocated(CONTJ)) deallocate(CONTJ)
  rundmtx=0
  goto 001
endif

rundmtx=1

001 continue

END FUNCTION

