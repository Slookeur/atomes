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
!! @file rings-guttman.F90
!! @short Guttman ring statistics
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER FUNCTION GUTTMAN_RINGS()

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

#ifdef OPENMP
INTEGER :: NUMTH
LOGICAL :: DOATOMS
#endif

INTERFACE
  INTEGER FUNCTION RECRINGS(VID)
    INTEGER, INTENT(IN) :: VID
  END FUNCTION
END INTERFACE

GUTTMAN_RINGS = 0

#ifdef OPENMP
NUMTH = OMP_GET_MAX_THREADS ()
DOATOMS=.false.
! Dynamic allocation of pointers and tables used in the "RINGS" subroutine
if (NS.ge.1 .and. NS.lt.NUMTH) then
  if (NUMTH .ge. 2*(NS-1)) then
    DOATOMS=.true.
  else
    NUMTH=NS
  endif
endif

if (ALL_ATOMS) DOATOMS=.true.

if (DOATOMS) then
  if (NA.lt.NUMTH) NUMTH=NA
#ifdef DEBUG
  write (6, *) "OpenMP on atoms, NUMTH= ",NUMTH
#endif
  call GUTTMAN_RING_SEARCH_ATOMS (NUMTH)
else
#ifdef DEBUG
  write (6, *) "OpenMP on MD steps, NUMTH= ",NUMTH
#endif
  call GUTTMAN_RING_SEARCH_STEPS (NUMTH)
endif
#else
call GUTTMAN_RING_SEARCH_STEPS ()
#endif

GUTTMAN_RINGS = RECRINGS(2)

END FUNCTION

#ifdef OPENMP
SUBROUTINE GUTTMAN_RING_SEARCH_ATOMS (NUMTH)

USE PARAMETERS

!$ USE OMP_LIB
IMPLICIT NONE

INTEGER, INTENT(IN) :: NUMTH
TYPE (RING), DIMENSION(:), ALLOCATABLE :: THE_RING
INTEGER, DIMENSION(:), ALLOCATABLE :: TRING, INDTE, INDTH
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVRING, ORDRING
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVR, ORDR
INTEGER :: LORA, LORB, LORC, ri

INTERFACE
  RECURSIVE SUBROUTINE INSIDE_RING (THE_RING, FND, S_IR, AI_IR, RID, TAE, TAH, LRA, LRB, &
                                    NRPAT, RSAVED, OSAVED, TRING, INDE, INDH, RESL, CPT, VPT)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLD), INTENT(INOUT) :: THE_RING
    LOGICAL, INTENT(INOUT) :: FND
    INTEGER, INTENT(IN) :: S_IR, AI_IR, RID
    INTEGER, INTENT(INOUT) :: TAE, TAH
    INTEGER, INTENT(IN) :: LRA, LRB
    INTEGER, DIMENSION(NA), INTENT(INOUT) :: NRPAT
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: TRING
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE, INDH
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL
    INTEGER, DIMENSION(NA), INTENT(IN):: CPT
    INTEGER, DIMENSION(NA,MAXN), INTENT(IN) :: VPT
  END SUBROUTINE
  INTEGER FUNCTION RINGS_TO_OGL_MENU (IDSEARCH, NRI)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: IDSEARCH
    INTEGER, DIMENSION(TAILLR, NS), INTENT(IN) :: NRI
  END FUNCTION
  INTEGER FUNCTION RINGS_TO_OGL (STEP, IDSEARCH, NRI, RSAVED, OSAVED)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: STEP, IDSEARCH
    INTEGER, DIMENSION(TAILLR,NS), INTENT(IN) :: NRI
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(IN) :: RSAVED, OSAVED
  END FUNCTION
  SUBROUTINE DEL_THIS_RING (TLED, RSAVED, OSAVED, TRING, RESL, INDT)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: TLED
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL, TRING
  END SUBROUTINE
END INTERFACE

ri = 0
if(allocated(SAVRING)) deallocate(SAVRING)
allocate(SAVRING(TAILLR,NUMA,TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="SAVRING"
  ALC=.true.
  goto 001
endif
if(allocated(ORDRING)) deallocate(ORDRING)
allocate(ORDRING(TAILLR,NUMA,TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="ORDRING"
  ALC=.true.
  goto 001
endif
if(allocated(CPAT)) deallocate(CPAT)
allocate(CPAT(NA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="CPAT"
  ALC=.true.
  goto 001
endif
 if(allocated(VPAT)) deallocate(VPAT)
allocate(VPAT(NA,MAXN), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="VPAT"
  ALC=.true.
  goto 001
endif

do i=1, NS

  p=0

  SAVRING(:,:,:)=0
  ORDRING(:,:,:)=0
  call SETUP_CPAT_VPAT_RING (NA, i, CONTJ, VOISJ, CPAT, VPAT)

  ! OpenMP on atoms only
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(MAXAT, MINAT, RPAT, APNA, SAUT, RUNSEARCH, &
  !$OMP& j, k, l, m, n, o, LORA, LORB, LORC, TAILLE, TAILLH, THE_RING, RES_LIST, &
  !$OMP& FOUND, ERR, SAVR, ORDR, TRING, INDTE, INDTH) &
  !$OMP& SHARED(i, p, NUMTH, NS, NA, TLT, NSP, LOT, TAILLR, TAILLD, CONTJ, VOISJ, CPAT, VPAT, &
  !$OMP& NUMA, FACTATRING, ATRING, MAXPNA, MINPNA, AMPAT, ABAB, NO_HOMO, ALLRINGS, &
  !$OMP& TBR, ALC, ALC_TAB, NCELLS, PBC, MAXN, SAVRING, ORDRING, NRING, INDRING, PNA, ri)

  if (allocated(RPAT)) deallocate(RPAT)
  allocate(RPAT(NA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="RPAT"
    ALC=.true.
    goto 002
  endif
  if(allocated(RES_LIST)) deallocate(RES_LIST)
  allocate(RES_LIST(TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="RES_LIST"
    ALC=.true.
    goto 002
  endif
  if(allocated(INDTE)) deallocate(INDTE)
  allocate(INDTE(NUMA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="INDTE"
    goto 002
  endif
  if(allocated(INDTH)) deallocate(INDTH)
  allocate(INDTH(NUMA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="INDTH"
    goto 002
  endif
  if(allocated(APNA)) deallocate(APNA)
  allocate(APNA(TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="APNA"
    ALC=.true.
    goto 002
  endif
  if(allocated(SAVR)) deallocate(SAVR)
  allocate(SAVR(TAILLR,NUMA,TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="SAVR"
    ALC=.true.
    goto 002
  endif
  if(allocated(ORDR)) deallocate(ORDR)
  allocate(ORDR(TAILLR,NUMA,TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="ORDR"
    ALC=.true.
    goto 002
  endif
  if(allocated(TRING)) deallocate(TRING)
  allocate(TRING(TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="TRING"
    ALC=.true.
    goto 002
  endif
  if(allocated(THE_RING)) deallocate(THE_RING)
  allocate(THE_RING(TAILLD), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="THE_RING"
    ALC=.true.
    goto 002
  endif

  TRING(:)=0
  SAVR(:,:,:)=0
  ORDR(:,:,:)=0
  !$OMP DO SCHEDULE(STATIC,NA/NUMTH)
  do j=1, NA

    if (TBR .or. ALC) goto 003
    if (TLT .eq. NSP+1 .or. LOT(j) .eq. TLT) then

      !$OMP CRITICAL
      p = p+1
      o = p
      !$OMP END CRITICAL
      APNA(:) = 0
      MINAT=TAILLD
      MAXAT=1
      SAUT=.true.

      if (CPAT(j).ge.2) then

        do l=1, CPAT(j)

          LORA=LOT(j)
          LORB=LOT(VPAT(j,l))
          if (ABAB) then
            if (LORA .ne. LORB) then
              RUNSEARCH=.true.
            else
              RUNSEARCH=.false.
            endif
          else
            RUNSEARCH=.true.
          endif

          if (RUNSEARCH) then

            FOUND=.false.
            RPAT(:) = 0
            THE_RING(1)%ATOM=j
            THE_RING(1)%SPEC=LORA
            THE_RING(1)%NEIGHBOR=1
            m = VPAT(j,l)
            RPAT(m)=1
            THE_RING(2)%ATOM=m
            THE_RING(2)%SPEC=LORB
            THE_RING(2)%NEIGHBOR=CPAT(m)
            TAILLE=TAILLR
            TAILLH=TAILLR
            RES_LIST(:) = 0
            INDTE(:) = 0
            INDTH(:) = 0
            call INSIDE_RING (THE_RING, FOUND, i, m, 2, TAILLE, TAILLH, LORB, LORA, &
                              RPAT, SAVR, ORDR, TRING, INDTE, INDTH, RES_LIST, CPAT, VPAT)
            if (ALC) ALC_TAB="INSIDE_RING"
            if (TBR .or. ALC) goto 003
            if (FOUND) then
              if (APNA(TAILLE) .eq. 0) then
                APNA(TAILLE)=1
                MINAT=min(MINAT,TAILLE)
                MAXAT=max(MAXAT,TAILLE)
                ! if (FACTATPNA) ATPNA(TAILLE,o,i)=1
                SAUT=.false.
              endif
            elseif (NO_HOMO .and. RES_LIST(TAILLH) .ne. 0) then
              ! Some shortest ring with HP bonds and < TAILLR were found
              ! delete these rings now
              call DEL_THIS_RING (TAILLH, SAVR, ORDR, TRING, RES_LIST, INDTH)
            else
              if (CONTJ(j,i) .ge. 2 .and. CONTJ(m,i) .ge. 2) then
                !$OMP ATOMIC
                AMPAT(o,i)=AMPAT(o,i)+1
              endif
            endif
          endif
        enddo

      endif

      if (.not. SAUT) then
        do k=3, TAILLR
          do l=3, TAILLR
            if (APNA(k).eq.1 .and. APNA(l).eq.1) then
              !$OMP ATOMIC
              PNA(k,l,i)=PNA(k,l,i)+1
            endif
          enddo
        enddo
        !$OMP ATOMIC
        MAXPNA(MAXAT,i)=MAXPNA(MAXAT,i)+1
        !$OMP ATOMIC
        MINPNA(MINAT,i)=MINPNA(MINAT,i)+1
      endif
    endif

    !$OMP CRITICAL
    do k=3, TAILLR
      if (TRING(k).gt.0) then
        if (NRING(k,i).gt.0) then
          o = 0
          do l=1, TRING(k)
            do m=1, NRING(k,i)
              SAUT=.true.
              do n=1, k
                if (SAVRING(k,m,n) .ne. SAVR(k,l,n)) then
                  SAUT=.false.
                  exit
                endif
              enddo
              if (SAUT) exit
            enddo
            if (.not.SAUT) then
              o = o + 1
              if (NRING(k,i)+o .gt. NUMA) then
                TBR=.true.
                goto 004
              endif
              do m=1, k
                SAVRING(k,NRING(k,i)+o,m) = SAVR(k,l,m)
                ORDRING(k,NRING(k,i)+o,m) = ORDR(k,l,m)
              enddo
            endif
          enddo
          NRING(k,i)=NRING(k,i)+o
        else
          do l=1, TRING(k)
            do m=1, k
              SAVRING(k,l,m) = SAVR(k,l,m)
              ORDRING(k,l,m) = ORDR(k,l,m)
            enddo
          enddo
          NRING(k,i) = TRING(k)
        endif
      endif
    enddo

    004 continue
    !$OMP END CRITICAL

    003 continue
  enddo
  !$OMP END DO NOWAIT

  002 continue
  if (allocated(RPAT)) deallocate (RPAT)
  if (allocated(RES_LIST)) deallocate (RES_LIST)
  if (allocated(INDTE)) deallocate (INDTE)
  if (allocated(INDTH)) deallocate (INDTH)
  if (allocated(APNA)) deallocate (APNA)
  if (allocated(TRING)) deallocate (TRING)
  if (allocated(SAVR)) deallocate (SAVR)
  if (allocated(ORDR)) deallocate (ORDR)
  if (allocated(THE_RING)) deallocate (THE_RING)
  !$OMP END PARALLEL
  if (ALC .or. TBR) goto 001

  !do j=3, TAILLR
  !    write (6, '("s= ",i4,", j= ",i2,", nr(",i2,",",i4,")= ",i2)') i,j,j,i, NRING(j,i)
  !  if (NRING(j,i) .gt. 0) then
  !    do k=1, NRING(j,i)
  !      write (6, *) "   k= ",k,", R(o)= ",ORDRING(j,k,1:j)
  !    enddo
  !  endif
  !enddo
  ri = ri + RINGS_TO_OGL (i, 2, NRING, SAVRING, ORDRING)

enddo

001 continue

if (ALC) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: GUTTMAN_RING_SEARCH_ATOMS"//CHAR(0), "Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif

if (allocated(CPAT)) deallocate (CPAT)
if (allocated(VPAT)) deallocate (VPAT)
if (allocated(SAVRING)) deallocate (SAVRING)
if (allocated(ORDRING)) deallocate (ORDRING)

if (ri .eq. NS) ri = RINGS_TO_OGL_MENU (2, NRING)

END SUBROUTINE
#endif

#ifdef OPENMP
SUBROUTINE GUTTMAN_RING_SEARCH_STEPS (NUMTH)

USE PARAMETERS
!$ USE OMP_LIB
IMPLICIT NONE
INTEGER, INTENT(IN) :: NUMTH
#else
SUBROUTINE GUTTMAN_RING_SEARCH_STEPS ()

USE PARAMETERS
IMPLICIT NONE
#endif
TYPE (RING), DIMENSION(:), ALLOCATABLE :: THE_RING
INTEGER, DIMENSION(:), ALLOCATABLE :: TRING, INDTE, INDTH
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVRING, ORDRING
INTEGER :: LORA, LORB, ri

INTERFACE
  RECURSIVE SUBROUTINE INSIDE_RING (THE_RING, FND, S_IR, AI_IR, RID, TAE, TAH, LRA, LRB, &
                                    NRPAT, RSAVED, OSAVED, TRING, INDE, INDH, RESL, CPT, VPT)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLD), INTENT(INOUT) :: THE_RING
    LOGICAL, INTENT(INOUT) :: FND
    INTEGER, INTENT(IN) :: S_IR, AI_IR, RID
    INTEGER, INTENT(INOUT) :: TAE, TAH
    INTEGER, INTENT(IN) :: LRA, LRB
    INTEGER, DIMENSION(NA), INTENT(INOUT) :: NRPAT
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: TRING
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE, INDH
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL
    INTEGER, DIMENSION(NA), INTENT(IN):: CPT
    INTEGER, DIMENSION(NA,MAXN), INTENT(IN) :: VPT
  END SUBROUTINE
  INTEGER FUNCTION RINGS_TO_OGL_MENU (IDSEARCH, NRI)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: IDSEARCH
    INTEGER, DIMENSION(TAILLR, NS), INTENT(IN) :: NRI
  END FUNCTION
  INTEGER FUNCTION RINGS_TO_OGL (STEP, IDSEARCH, NRI, RSAVED, OSAVED)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: STEP, IDSEARCH
    INTEGER, DIMENSION(TAILLR,NS), INTENT(IN) :: NRI
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(IN) :: RSAVED, OSAVED
  END FUNCTION
  SUBROUTINE DEL_THIS_RING (TLED, RSAVED, OSAVED, TRING, RESL, INDT)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: TLED
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL, TRING
  END SUBROUTINE
END INTERFACE

ri = 0

#ifdef OPENMP
! OpenMP on steps only
!$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
!$OMP& PRIVATE(TAILLE, TAILLH, MAXAT, MINAT, SAUT, RUNSEARCH, &
!$OMP& i, j, k, l, m, n, o, LORA, LORB, THE_RING, RES_LIST, INDTE, INDTH, APNA, &
!$OMP& FOUND, ERR, TRING, SAVRING, ORDRING, RPAT, CPAT, VPAT) &
!$OMP& SHARED(p, NUMTH, NS, NA, TLT, NSP, LOT, TAILLR, TAILLD, CONTJ, VOISJ, &
!$OMP& NUMA, FACTATRING, ATRING, MAXPNA, MINPNA, AMPAT, ABAB, NO_HOMO, ALLRINGS, &
!$OMP& TBR, ALC, ALC_TAB, NCELLS, THE_BOX, FULLPOS, PBC, MAXN, NRING, INDRING, PNA, ri)
#endif

if(allocated(SAVRING)) deallocate(SAVRING)
allocate(SAVRING(TAILLR,NUMA,TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="SAVRING"
  ALC=.true.
  goto 001
endif
if(allocated(ORDRING)) deallocate(ORDRING)
allocate(ORDRING(TAILLR,NUMA,TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="ORDRING"
  ALC=.true.
  goto 001
endif
if(allocated(CPAT)) deallocate(CPAT)
allocate(CPAT(NA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="CPAT"
  ALC=.true.
  goto 001
endif
if(allocated(VPAT)) deallocate(VPAT)
allocate(VPAT(NA,MAXN), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="VPAT"
  ALC=.true.
  goto 001
endif
if (allocated(RPAT)) deallocate(RPAT)
allocate(RPAT(NA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="RPAT"
  ALC=.true.
  goto 001
endif
if(allocated(RES_LIST)) deallocate(RES_LIST)
allocate(RES_LIST(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="RES_LIST"
  ALC=.true.
  goto 001
endif
if(allocated(INDTE)) deallocate(INDTE)
allocate(INDTE(NUMA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="INDTE"
  goto 001
endif
if(allocated(INDTH)) deallocate(INDTH)
allocate(INDTH(NUMA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="INDTH"
  goto 001
endif
if(allocated(APNA)) deallocate(APNA)
allocate(APNA(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="APNA"
  ALC=.true.
  goto 001
endif
if(allocated(TRING)) deallocate(TRING)
allocate(TRING(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="TRING"
  ALC=.true.
  goto 001
endif
if(allocated(THE_RING)) deallocate(THE_RING)
allocate(THE_RING(TAILLD), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="THE_RING"
  ALC=.true.
  goto 001
endif

#ifdef OPENMP
!$OMP DO SCHEDULE(STATIC,NS/NUMTH)
#endif
do i=1, NS

  if (TBR .or. ALC) goto 002
  SAVRING(:,:,:)=0
  ORDRING(:,:,:)=0
  TRING(:)=0
  call SETUP_CPAT_VPAT_RING (NA, i, CONTJ, VOISJ, CPAT, VPAT)

  o=0
  do j=1, NA

    if (TLT .eq. NSP+1 .or. LOT(j) .eq. TLT) then

      o=o+1
      APNA(:) = 0
      MINAT=TAILLR
      MAXAT=1
      SAUT=.true.

      if (CPAT(j).ge.2) then

        do l=1, CPAT(j)

          LORA=LOT(j)
          LORB=LOT(VPAT(j,l))
          if (ABAB) then
            if (LORA .ne. LORB) then
              RUNSEARCH=.true.
            else
              RUNSEARCH=.false.
            endif
          else
            RUNSEARCH=.true.
          endif

          if (RUNSEARCH) then
            FOUND=.false.
            RPAT(:) = 0
            THE_RING(1)%ATOM=j
            THE_RING(1)%SPEC=LORA
            THE_RING(1)%NEIGHBOR=1
            m = VPAT(j,l)
            RPAT(m)=1
            THE_RING(2)%ATOM=m
            THE_RING(2)%SPEC=LORB
            THE_RING(2)%NEIGHBOR=CPAT(m)
            TAILLE=TAILLR
            TAILLH=TAILLR
            RES_LIST(:) = 0
            INDTE(:) = 0
            INDTH(:) = 0
            call INSIDE_RING (THE_RING, FOUND, i, m, 2, TAILLE, TAILLH, LORB, LORA, &
                              RPAT, SAVRING, ORDRING, TRING, INDTE, INDTH, RES_LIST, CPAT, VPAT)
            if (ALC) ALC_TAB="INSIDE_RING"
            if (TBR .or. ALC) goto 002
            if (FOUND) then
              if (APNA(TAILLE) .eq. 0) then
                APNA(TAILLE)=1
                MINAT=min(MINAT,TAILLE)
                MAXAT=max(MAXAT,TAILLE)
                ! if (FACTATPNA) ATPNA(TAILLE,o,i)=1
                SAUT=.false.
              endif
            elseif (NO_HOMO .and. RES_LIST(TAILLH) .ne. 0) then
              ! Some shortest ring with HP bonds and < TAILLR were found
              ! delete these rings now
              call DEL_THIS_RING (TAILLH, SAVRING, ORDRING, TRING, RES_LIST, INDTH)
            else
              if (CONTJ(j,i) .ge. 2 .and. CONTJ(m,i) .ge. 2) AMPAT(o,i)=AMPAT(o,i)+1
            endif
          endif
        enddo

      endif

      if (.not. SAUT) then
        do k=3, TAILLR
          do l=3, TAILLR
            if (APNA(k).eq.1 .and. APNA(l).eq.1) then
              PNA(k,l,i)=PNA(k,l,i)+1
            endif
          enddo
        enddo
        MAXPNA(MAXAT,i)=MAXPNA(MAXAT,i)+1
        MINPNA(MINAT,i)=MINPNA(MINAT,i)+1
      endif
    endif

  enddo

  do j=3, TAILLR
    NRING(j,i) = TRING(j)
  enddo

  !do j=3, TAILLR
  !  write (6, '("s= ",i4,", j= ",i2,", nr(",i2,",",i4,")= ",i2)') i,j,j,i, NRING(j,i)
  !  if (NRING(j,i) .gt. 0) then
  !    do k=1, NRING(j,i)
  !      write (6, *) "   k= ",k,", R(o)= ",ORDRING(j,k,1:j)
  !    enddo
  !  endif
  !enddo
  j = RINGS_TO_OGL (i, 2, NRING, SAVRING, ORDRING)
#ifdef OPENMP
  !$OMP ATOMIC
#endif
  ri = ri + j
  002 continue
enddo
#ifdef OPENMP
!$OMP END DO NOWAIT
#endif

001 continue

if (ALC) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: GUTTMAN_RING_SEARCH_STEPS"//CHAR(0), "Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif

if (allocated(TRING)) deallocate (TRING)
if (allocated(THE_RING)) deallocate (THE_RING)
if (allocated(SAVRING)) deallocate (SAVRING)
if (allocated(ORDRING)) deallocate (ORDRING)
if (allocated(CPAT)) deallocate (CPAT)
if (allocated(VPAT)) deallocate (VPAT)
if (allocated(RPAT)) deallocate (RPAT)
if (allocated(RES_LIST)) deallocate (RES_LIST)
if (allocated(INDTE)) deallocate (INDTE)
if (allocated(INDTH)) deallocate (INDTH)
if (allocated(APNA)) deallocate (APNA)

#ifdef OPENMP
!$OMP END PARALLEL
#endif

if (ri .eq. NS) ri = RINGS_TO_OGL_MENU (2, NRING)

END SUBROUTINE
