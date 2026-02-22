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
!! @file rings-king.F90
!! @short King ring statistics
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

SUBROUTINE SETUP_CPAT_VPAT_RING (NAT, STR, CONT, VOIS, CPT, VPT)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: NAT, STR
INTEGER, DIMENSION(NAT,NS), INTENT(IN):: CONT
INTEGER, DIMENSION(MAXN,NAT,NS), INTENT(IN) :: VOIS
INTEGER, DIMENSION(NAT), INTENT(INOUT):: CPT
INTEGER, DIMENSION(NAT,MAXN), INTENT(INOUT) :: VPT
INTEGER :: RAB, RAC, RAD

!do RAB=1, NAT
!  write (6, '("At= ",i4," Neigh= ",i2)') RAB, CONTJ(RAB,STR)
!  do RAC=1, CONTJ(RAB,STR)
!    write (6, '("  i= ",i2," Vois= ",i4)') RAC, VOISJ(RAC,RAB,STR)
!  enddo
!enddo

CPT(:) = 0
VPT(:,:) = 0

do RAB=1, NAT
  if (CONT(RAB,STR) .gt. 1) then
    RAC = 0
    do RAD=1, CONT(RAB,STR)
      if (CONT(VOIS(RAD,RAB,STR),STR) .gt. 1) then
        RAC=RAC+1
        VPT(RAB,RAC) = VOIS(RAD,RAB,STR)
      endif
    enddo
    if (RAC .ge. 2) then
      CPT(RAB) = RAC
    endif
  endif
enddo

END SUBROUTINE

SUBROUTINE SETUP_CPAT_VPAT (STR, NAT)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: STR, NAT
INTEGER :: RAB, RAC, RAD

!do RAB=1, NAT
!  write (6, '("At= ",i4," Neigh= ",i2)') RAB, CONTJ(RAB,STR)
!  do RAC=1, CONTJ(RAB,STR)
!    write (6, '("  i= ",i2," Vois= ",i4)') RAC, VOISJ(RAC,RAB,STR)
!  enddo
!enddo


do RAB=1, NAT
  CPAT(RAB) = 0
  if (CONTJ(RAB,STR) .gt. 1) then
    RAC = 0
    do RAD=1, CONTJ(RAB,STR)
      if (CONTJ(VOISJ(RAD,RAB,STR),STR) .gt. 1) then
        RAC=RAC+1
        VPAT(RAB,RAC) = VOISJ(RAD,RAB,STR)
      endif
    enddo
    if (RAC .ge. 2) then
      CPAT(RAB) = RAC
    endif
  endif
enddo

END SUBROUTINE

INTEGER FUNCTION KING_RINGS()

USE PARAMETERS

#ifdef OPENMP
!$ USE OMP_LIB
#endif
IMPLICIT NONE

INTEGER :: ar
#ifdef OPENMP
INTEGER :: NUMTH
LOGICAL :: DOATOMS
#endif

INTERFACE
  INTEGER FUNCTION RECRINGS(VID)
    INTEGER, INTENT(IN) :: VID
  END FUNCTION
END INTERFACE

KING_RINGS = 0
ar = 0
if (.not.ALLRINGS) ar=1

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
  call KING_RING_SEARCH_ATOMS (ar, NUMTH)
else
#ifdef DEBUG
  write (6, *) "OpenMP on MD steps, NUMTH= ",NUMTH
#endif
  call KING_RING_SEARCH_STEPS (ar, NUMTH)
endif
#else
call KING_RING_SEARCH_STEPS (ar)
#endif

KING_RINGS = RECRINGS(ar)

END FUNCTION

#ifdef OPENMP
SUBROUTINE KING_RING_SEARCH_ATOMS (ARI, NUMTH)

USE PARAMETERS

!$ USE OMP_LIB
IMPLICIT NONE

INTEGER, INTENT(IN) :: ARI, NUMTH
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
    INTEGER, INTENT(IN) :: S_IR, AI_IR, RID, LRA, LRB
    INTEGER, INTENT(INOUT) :: TAE, TAH
    INTEGER, DIMENSION(NA), INTENT(INOUT) :: NRPAT
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE, INDH
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL, TRING
    INTEGER, DIMENSION(NA,MAXN), INTENT(IN) :: VPT
    INTEGER, DIMENSION(NA), INTENT(IN):: CPT
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
  !$OMP& NUMA, FACTATRING, ATRING, MAXPNA, MINPNA, DOAMPAT, AMPAT, ABAB, NO_HOMO, ALLRINGS, &
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

        do l=1, CPAT(j)-1

          do m=l+1, CPAT(j)

            LORA=LOT(j)
            LORB=LOT(VPAT(j,l))
            LORC=LOT(VPAT(j,m))
            if (ABAB) then
              if ((LORB .ne. LORA) .and. (LORC .ne. LORA) .and. (LORB .eq. LORC)) then
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
              do k=1, CPAT(j)
                RPAT(VPAT(j,k)) = 1
              enddo
              RPAT(VPAT(j,l)) = 0
              THE_RING(1)%ATOM=VPAT(j,l)
              THE_RING(1)%SPEC=LORB
              THE_RING(1)%NEIGHBOR=1
              THE_RING(2)%ATOM=j
              THE_RING(2)%SPEC=LORA
              THE_RING(2)%NEIGHBOR=1
              RPAT(j)=1
              TAILLE=TAILLR
              TAILLH=TAILLR
              RES_LIST(:) = 0
              INDTE(:) = 0
              INDTH(:) = 0
              THE_RING(3)%ATOM=VPAT(j,m)
              THE_RING(3)%SPEC=LORC
              THE_RING(3)%NEIGHBOR=CPAT(VPAT(j,m))

              call INSIDE_RING (THE_RING, FOUND, i, VPAT(j,m), 3, TAILLE, TAILLH, LORA, LORB, &
                                RPAT, SAVR, ORDR, TRING, INDTE, INDTH, RES_LIST, CPAT, VPAT)
              if (ALC) ALC_TAB="INSIDE_RING"
              if (TBR .or. ALC) goto 003

              if (ALLRINGS) then
                do k=3, TAILLR
                  do n=1, TRING(k)
                    if (INDTE(n).ne.0 .and. APNA(k).eq.0) then
                      APNA(k)=1
                      MINAT=min(MINAT,k)
                      MAXAT=max(MAXAT,k)
                      ! if (FACTATPNA) ATPNA(k,o,i)=1
                      SAUT=.false.
                    endif
                  enddo
                enddo
              else if (FOUND) then
                if (APNA(TAILLE) .eq. 0) then
                  APNA(TAILLE)=1
                  MINAT=min(MINAT,TAILLE)
                  MAXAT=max(MAXAT,TAILLE)
                  ! if (FACTATPNA) ATPNA(TAILLE,o,i)=1
                  SAUT=.false.
                endif
              else if (NO_HOMO .and. RES_LIST(TAILLH) .ne. 0) then
                ! Some shortest ring with HP bonds and < TAILLR were found
                ! delete these rings now
                call DEL_THIS_RING (TAILLH, SAVR, ORDR, TRING, RES_LIST, INDTH)
              else if (DOAMPAT) then
                if (CONTJ(VPAT(j,l),i) .ge. 2 .and. CONTJ(VPAT(j,m),i) .ge. 2) then
                  !$OMP ATOMIC
                  AMPAT(o,i)=AMPAT(o,i)+1
                endif
              endif
            endif
          enddo
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

    003 continue
  enddo
  !$OMP END DO NOWAIT
  if (TBR .or. ALC) goto 002
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
  !  write (6, '("s= ",i4,", j= ",i2,", nr(",i2,",",i4,")= ",i2)') i,j,j,i, NRING(j,i)
  !  if (NRING(j,i) .gt. 0) then
  !    do k=1, NRING(j,i)
  !      write (6, *) "   k= ",k,", R(o)= ",ORDRING(j,k,1:j)
  !    enddo
  !  endif
  !enddo
  ri = ri + RINGS_TO_OGL (i, ARI, NRING, SAVRING, ORDRING)

enddo

001 continue

if (ALC) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: KING_RING_SEARCH_ATOMS"//CHAR(0), "Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif

if (allocated(CPAT)) deallocate (CPAT)
if (allocated(VPAT)) deallocate (VPAT)
if (allocated(SAVRING)) deallocate (SAVRING)
if (allocated(ORDRING)) deallocate (ORDRING)

if (ri .eq. NS) ri = RINGS_TO_OGL_MENU (ARI, NRING)

END SUBROUTINE
#endif

#ifdef OPENMP
SUBROUTINE KING_RING_SEARCH_STEPS (ARI, NUMTH)

USE PARAMETERS
!$ USE OMP_LIB
IMPLICIT NONE
INTEGER, INTENT(IN) :: ARI, NUMTH
#else
SUBROUTINE KING_RING_SEARCH_STEPS (ARI)

USE PARAMETERS
IMPLICIT NONE
INTEGER, INTENT(IN) :: ARI
#endif
TYPE (RING), DIMENSION(:), ALLOCATABLE :: THE_RING
INTEGER, DIMENSION(:), ALLOCATABLE :: TRING, INDTE, INDTH
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVRING, ORDRING
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
#ifdef OPENMP
! OpenMP on steps only
!$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
!$OMP& PRIVATE(TAILLE, TAILLH, MAXAT, MINAT, SAUT, RUNSEARCH, &
!$OMP& i, j, k, l, m, n, o, p, LORA, LORB, LORC, THE_RING, RES_LIST, INDTE, INDTH, APNA, &
!$OMP& FOUND, ERR, TRING, SAVRING, ORDRING, RPAT, CPAT, VPAT) &
!$OMP& SHARED(ARI, NUMTH, NS, NA, TLT, NSP, LOT, TAILLR, TAILLD, CONTJ, VOISJ, &
!$OMP& NUMA, FACTATRING, ATRING, MAXPNA, MINPNA, DOAMPAT, AMPAT, ABAB, NO_HOMO, ALLRINGS, &
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

        do l=1, CPAT(j)-1

          do m=l+1, CPAT(j)

            LORA=LOT(j)
            LORB=LOT(VPAT(j,l))
            LORC=LOT(VPAT(j,m))
            if (ABAB) then
              if ((LORB .ne. LORA) .and. (LORC .ne. LORA) .and. (LORB .eq. LORC)) then
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
              do k=1, CPAT(j)
                RPAT(VPAT(j,k)) = 1
              enddo
              p = VPAT(j,l)
              RPAT(p) = 0
              RPAT(j)=1
              THE_RING(1)%ATOM=p
              THE_RING(1)%SPEC=LORB
              THE_RING(1)%NEIGHBOR=1
              THE_RING(2)%ATOM=j
              THE_RING(2)%SPEC=LORA
              THE_RING(2)%NEIGHBOR=1
              THE_RING(3)%ATOM=VPAT(j,m)
              THE_RING(3)%SPEC=LORC
              THE_RING(3)%NEIGHBOR=CPAT(VPAT(j,m))
              TAILLE=TAILLR
              TAILLH=TAILLR
              RES_LIST(:) = 0
              INDTE(:) = 0
              INDTH(:) = 0
              call INSIDE_RING (THE_RING, FOUND, i, VPAT(j,m), 3, TAILLE, TAILLH, LORA, LORB, &
                                RPAT, SAVRING, ORDRING, TRING, INDTE, INDTH, RES_LIST, CPAT, VPAT)
              if (ALC) ALC_TAB="INSIDE_RING"
              if (TBR .or. ALC) goto 002

              if (ALLRINGS) then
                do k=3, TAILLR
                  do n=1, TRING(k)
                    if (INDTE(n).ne.0 .and. APNA(k).eq.0) then
                      APNA(k)=1
                      MINAT=min(MINAT,k)
                      MAXAT=max(MAXAT,k)
                      ! if (FACTATPNA) ATPNA(k,o,i)=1
                      SAUT=.false.
                    endif
                  enddo
                enddo
              else if (FOUND) then
                if (APNA(TAILLE) .eq. 0) then
                  APNA(TAILLE)=1
                  MINAT=min(MINAT,TAILLE)
                  MAXAT=max(MAXAT,TAILLE)
                  ! if (FACTATPNA) ATPNA(TAILLE,o,i)=1
                  SAUT=.false.
                endif
              else if (NO_HOMO .and. RES_LIST(TAILLH) .ne. 0) then
                ! Some shortest ring with HP bonds and < TAILLR were found
                ! delete these rings now
                call DEL_THIS_RING (TAILLH, SAVRING, ORDRING, TRING, RES_LIST, INDTH)
              else if (DOAMPAT) then
                if (CONTJ(VPAT(j,l),i) .ge. 2 .and. CONTJ(VPAT(j,m),i) .ge. 2) AMPAT(o,i)=AMPAT(o,i)+1
              endif
            endif
          enddo
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
  !  write (6, '("s= ",i4,", j= ",i2,", nr(",i2,",",i4,")= ",i3)') i,j,j,i, NRING(j,i)
  !  if (NRING(j,i) .gt. 0) then
  !    do k=1, NRING(j,i)
  !      write (6, *) "   k= ",k,", R(o)= ",ORDRING(j,k,1:j)
  !    enddo
  !  endif
  !enddo
  j = RINGS_TO_OGL (i, ARI, NRING, SAVRING, ORDRING)

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
                   "Subroutine: KING_RING_SEARCH_STEPS"//CHAR(0), "Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
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

if (ri .eq. NS) ri = RINGS_TO_OGL_MENU (ARI, NRING)

END SUBROUTINE

INTEGER FUNCTION CHECK_RING (THE_RING, FND, S_CR, RID, TAE, TAH, RSAVED, OSAVED, TRING, INDE, INDH, RESL)

USE PARAMETERS

IMPLICIT NONE

TYPE (RING), DIMENSION(TAILLD), INTENT(IN) :: THE_RING
LOGICAL, INTENT(INOUT) :: FND
INTEGER, INTENT(IN) :: S_CR, RID
INTEGER, INTENT(INOUT) :: TAE, TAH
INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: TRING
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE, INDH
INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL
INTEGER :: ix, iy, CHAINE
LOGICAL :: IS_RING
LOGICAL :: HOMOP
DOUBLE PRECISION :: DUV
DOUBLE PRECISION, DIMENSION(3) :: RAB, VAB

INTERFACE
  DOUBLE PRECISION FUNCTION  CALCDIJ(R12, AT1, AT2, STEP_1, STEP_2, SID)
    DOUBLE PRECISION, DIMENSION(3), INTENT(INOUT) :: R12
    INTEGER, INTENT(IN) :: AT1, AT2, STEP_1, STEP_2, SID
  END FUNCTION
  SUBROUTINE SAVE_THIS_RING (THE_RING, TLES, RSAVED, OSAVED, TRING, INDT, RESL)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLR), INTENT(IN) :: THE_RING
    INTEGER, INTENT(IN) :: TLES
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL, TRING
  END SUBROUTINE
  SUBROUTINE DEL_THIS_RING (TLED, RSAVED, OSAVED, TRING, RESL, INDT)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: TLED
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL, TRING
  END SUBROUTINE
END INTERFACE

  IS_RING=.true.
  HOMOP=.false.
  if (THE_RING(RID)%ATOM.eq.THE_RING(1)%ATOM .and. RID.ge.4) then

    IS_RING=.true.
    HOMOP=.false.
    if (NSP .gt. 1) then
      do ix=1, RID-1
        if (THE_RING(ix)%SPEC .eq. THE_RING(ix+1)%SPEC) then
          HOMOP=.true.
          exit
        endif
      enddo
    endif

    if (PBC) then

      VAB(:)=0.0d0
      do ix=1, RID-1
        if (NCELLS .gt. 1) then
          DUV = CALCDIJ (RAB,THE_RING(ix)%ATOM,THE_RING(ix+1)%ATOM,S_CR,S_CR,S_CR)
        else
          DUV = CALCDIJ (RAB,THE_RING(ix)%ATOM,THE_RING(ix+1)%ATOM,S_CR,S_CR,1)
        endif
        do iy=1,3
          VAB(iy)=VAB(iy)+RAB(iy)
        enddo
      enddo
      if (abs(VAB(1)) .ge. 0.01 .or. abs(VAB(2)) .ge. 0.01 .or. abs(VAB(3)) .ge. 0.01) then
        IS_RING=.false.
      endif

    endif

    if (IS_RING) then

      CHAINE = RID-1
      if (ALLRINGS) then
        if (.not.NO_HOMO .or. .not.HOMOP) then
          FND=.true.
          call SAVE_THIS_RING (THE_RING, CHAINE, RSAVED, OSAVED, TRING, INDE, RESL)
        endif
      else
        if (NO_HOMO) then
          if (CHAINE .le. TAH) then
            if (CHAINE.lt.TAH .or. .not.HOMOP) call DEL_THIS_RING (TAH, RSAVED, OSAVED, TRING, RESL, INDH)
            if (CHAINE.lt.TAE)  call DEL_THIS_RING (TAE, RSAVED, OSAVED, TRING, RESL, INDE)
            if (.not.HOMOP .and. CHAINE.le.TAE) then
              FND=.true.
              TAE=CHAINE
              call SAVE_THIS_RING (THE_RING, TAE, RSAVED, OSAVED, TRING, INDE, RESL)
            else if (HOMOP) then
              if (CHAINE .lt. TAE) then
                FND=.false.
                TAH=CHAINE
                call SAVE_THIS_RING (THE_RING, TAH, RSAVED, OSAVED, TRING, INDH, RESL)
              endif
            endif
          endif
        else
          if (CHAINE .le. TAE) then
            if (CHAINE.lt.TAE)  call DEL_THIS_RING (TAE, RSAVED, OSAVED, TRING, RESL, INDE)
            FND=.true.
            TAE=CHAINE
            call SAVE_THIS_RING (THE_RING, TAE, RSAVED, OSAVED, TRING, INDE, RESL)
          endif
        endif
      endif
      CHECK_RING = 1

    else

      CHECK_RING = 2

    endif

  else

    CHECK_RING = 0

  endif

END FUNCTION

RECURSIVE SUBROUTINE INSIDE_RING (THE_RING, FND, S_IR, AI_IR, RID, TAE, TAH, LRA, LRB, &
                                  NRPAT, RSAVED, OSAVED, TRING, INDE, INDH, RESL, CPT, VPT)
USE PARAMETERS

IMPLICIT NONE

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
INTEGER :: IND, RES
LOGICAL :: ADDSP

INTERFACE
  INTEGER FUNCTION CHECK_RING (THE_RING, FND, S_CR, RID, TAE, TAH, RSAVED, OSAVED, TRING, INDE, INDH, RESL)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLR), INTENT(IN) :: THE_RING
    LOGICAL, INTENT(INOUT) :: FND
    INTEGER, INTENT(IN) :: S_CR, RID
    INTEGER, INTENT(INOUT) :: TAE, TAH
    INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: TRING
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE, INDH
    INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL
  END FUNCTION
END INTERFACE

if (RID-1 .lt. TAE) then

  do while (THE_RING(RID)%NEIGHBOR .ge. 1)

    IND = VPT(AI_IR, THE_RING(RID)%NEIGHBOR)
    if (NRPAT(IND).eq.0) then

      if (ABAB) then
        if (mod(RID,2).eq.0 .and. LOT(IND).eq.LRB) then
          ADDSP=.true.
        elseif (mod(RID,2).ne.0 .and. LOT(IND).eq.LRA) then
          ADDSP=.true.
        else
          ADDSP=.false.
        endif
      else
        ADDSP=.true.
      endif
      if (ADDSP) then

        THE_RING(RID+1)%ATOM = IND
        THE_RING(RID+1)%SPEC = LOT(IND)
        THE_RING(RID+1)%NEIGHBOR = CPT(IND)
        NRPAT(IND)=1
        RES = CHECK_RING (THE_RING, FND, S_IR, RID+1, TAE, TAH, RSAVED, OSAVED, TRING, INDE, INDH, RESL)
        if (TBR .or. ALC) goto 001
        if (RES .eq. 0) then
          call INSIDE_RING (THE_RING, FND, S_IR, IND, RID+1, TAE, TAH, LRA, LRB, &
                            NRPAT, RSAVED, OSAVED, TRING, INDE, INDH, RESL, CPT, VPT)
          if (TBR .or. ALC) goto 001
        endif
        NRPAT(IND) = 0

      endif

    endif

    THE_RING(RID)%NEIGHBOR = THE_RING(RID)%NEIGHBOR - 1

  enddo

endif

001 continue

END SUBROUTINE INSIDE_RING

SUBROUTINE SAVE_THIS_RING (THE_RING, TLES, RSAVED, OSAVED, TRING, INDT, RESL)

USE PARAMETERS

IMPLICIT NONE

TYPE (RING), DIMENSION(TAILLD), INTENT(IN) :: THE_RING
INTEGER, INTENT(IN) :: TLES
INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL, TRING
INTEGER :: idx, idy
LOGICAL :: NEWRING
INTEGER, DIMENSION(TLES) :: TOTRI, TOSAV

! A ring has been found, we need to check if it has already been found or not

do idx=1, TLES

! Creation of two tab first for the numerical sorting of the list
! The other without sorting for output
  TOTRI(idx)=THE_RING(idx)%ATOM
  TOSAV(idx)=THE_RING(idx)%ATOM

enddo

call TRI(TOTRI, TLES)

if (TRING(TLES) .ne. 0) then

  do idx=1, TRING(TLES)
!   do-loop on existing rings to check if the new on has already been found

    NEWRING=.false.

    do idy=1, TLES
      if (TOTRI(idy) .ne. RSAVED(TLES,idx,idy)) then
        NEWRING=.true.
        exit
      endif
    enddo

    if (.not.NEWRING) then

      ! Already been found n-times, increment of the counter
      INDT(idx)=INDT(idx)+1
      exit

    endif

  enddo

else

  NEWRING=.true.

endif

if (NEWRING) then

! A new ring has been found
! We save the data
  RESL(TLES)=RESL(TLES)+1
  TRING(TLES)=TRING(TLES)+1
  if (TRING(TLES) .gt. NUMA) then
    TBR=.true.
    goto 001
  endif
  INDT(TRING(TLES))=INDT(TRING(TLES))+1
  do idx=1, TLES
    RSAVED(TLES,TRING(TLES),idx)=TOTRI(idx)
    OSAVED(TLES,TRING(TLES),idx)=TOSAV(idx)
  enddo

endif

001 continue

END SUBROUTINE

SUBROUTINE DEL_THIS_RING (TLED, RSAVED, OSAVED, TRING, RESL, INDT)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: TLED
INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESL, TRING
INTEGER :: xdel, did

! The max size of the ring possible for the triplet N1-At-N2 has down
! We have to delete the bigger rings already found for this triplet
if (RESL(TLED) .ge. 1) then

  do xdel=0, RESL(TLED)-1

    do did=1, TAILLR

      RSAVED(TLED,TRING(TLED)-xdel,did)=0
      OSAVED(TLED,TRING(TLED)-xdel,did)=0

    enddo

    do did=1, NUMA

      INDT(did)=0

    enddo

  enddo

  TRING(TLED)=TRING(TLED)-RESL(TLED)
  RESL(TLED)=0

endif

END SUBROUTINE
