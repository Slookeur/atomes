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
!! @file chains.F90
!! @short Chain statistics
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

SUBROUTINE SETUP_CPAT_VPAT_CHAIN (CONT, VOIS, STR, CPT, VPT)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: STR
INTEGER, DIMENSION(NA,NS), INTENT(IN):: CONT
INTEGER, DIMENSION(MAXN,NA,NS), INTENT(IN) :: VOIS
INTEGER, DIMENSION(NA), INTENT(INOUT):: CPT
INTEGER, DIMENSION(NA,MAXN), INTENT(INOUT) :: VPT
INTEGER :: RAB, RAC

!do RAB=1, NAT
!  write (6, '("At= ",i4," Neigh= ",i2)') RAB, CONTJ(RAB,STR)
!  do RAC=1, CONTJ(RAB,STR)
!    write (6, '("  i= ",i2," Vois= ",i4)') RAC, VOISJ(RAC,RAB,STR)
!  enddo
!enddo

CPT(:) = 0
VPT(:,:) = 0

do RAB=1, NA
  CPT(RAB) = CONT(RAB,STR)
  do RAC=1, CONT(RAB,STR)
    VPT(RAB,RAC) = VOIS(RAC,RAB,STR)
  enddo
enddo

END SUBROUTINE

INTEGER FUNCTION CHAINS()

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
  INTEGER FUNCTION RECHAINS ()
  END FUNCTION
END INTERFACE

CHAINS = 0

#ifdef OPENMP
NUMTH = OMP_GET_MAX_THREADS ()
DOATOMS=.false.
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
  call CHAINS_SEARCH_ATOMS (NUMTH)
else
#ifdef DEBUG
  write (6, *) "OpenMP on MD steps, NUMTH= ",NUMTH
#endif
  call CHAINS_SEARCH_STEPS (NUMTH)
endif
#else
call CHAINS_SEARCH_STEPS ()
#endif

CHAINS = RECHAINS()

END FUNCTION

#ifdef OPENMP
SUBROUTINE CHAINS_SEARCH_ATOMS (NUMTH)

USE PARAMETERS

!$ USE OMP_LIB
IMPLICIT NONE

INTEGER, INTENT(IN) :: NUMTH
TYPE (RING), DIMENSION(:), ALLOCATABLE :: THE_CHAIN
INTEGER, DIMENSION(:), ALLOCATABLE :: TRING, INDTE
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVRING
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVR
INTEGER :: LORA, LORB, RES, ch

INTERFACE
  INTEGER FUNCTION CHECK_CHAIN (THE_CHAIN, CHAINE, TAE, RSAVED, TRING, INDE, RESL)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLC), INTENT(IN) :: THE_CHAIN
    INTEGER, INTENT(IN) :: CHAINE
    INTEGER, INTENT(INOUT) :: TAE
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: TRING
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL
  END FUNCTION
  RECURSIVE SUBROUTINE INSIDE_CHAIN (THE_CHAIN, CID, TAE, LRA, LRB, NRPAT, RSAVED, TRING, INDE, RESL, CPT, VPT)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLC), INTENT(INOUT) :: THE_CHAIN
    INTEGER, INTENT(IN) :: CID, LRA, LRB
    INTEGER, INTENT(INOUT) :: TAE
    INTEGER, DIMENSION(NA), INTENT(INOUT) :: NRPAT
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: TRING
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL
    INTEGER, DIMENSION(NA), INTENT(IN):: CPT
    INTEGER, DIMENSION(NA,MAXN), INTENT(IN) :: VPT
  END SUBROUTINE
  INTEGER FUNCTION CHAINS_TO_OGL (STEP, NRI, RSAVED)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: STEP
    INTEGER, DIMENSION(TAILLC, NS), INTENT(IN) :: NRI
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(IN) :: RSAVED
  END FUNCTION
  INTEGER FUNCTION CHAINS_TO_OGL_MENU (NRI)
    USE PARAMETERS
    INTEGER, DIMENSION(TAILLC, NS), INTENT(IN) :: NRI
  END FUNCTION
END INTERFACE

ch = 0
if (allocated(NRING)) deallocate(NRING)
allocate(NRING(TAILLC,NS), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="NRING"
  ALC=.true.
  goto 001
endif
NRING(:,:) = 0
if(allocated(SAVRING)) deallocate(SAVRING)
allocate(SAVRING(TAILLC,NUMA,TAILLC), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="SAVRING"
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

  SAVRING(:,:,:)=0
  call SETUP_CPAT_VPAT_CHAIN (CONTJ, VOISJ, i, CPAT, VPAT)

  ! OpenMP on atoms only
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(THE_CHAIN, RPAT, RUNSEARCH, ERR, SAVR, TRING, INDTE, &
  !$OMP& j, k, l, m, n, o, LORA, LORB, RES, RES_LIST, TAILLE, SAUT) &
  !$OMP& SHARED(i, p, NUMTH, NS, NA, TLT, NSP, LOT, ISOLATED, CONTJ, VOISJ, CPAT, VPAT, &
  !$OMP& NUMA, MAXN, ACAC, AAAA, NOHP, TAILLC, TBR, ALC, ALC_TAB, NCELLS, PBC, SAVRING, NRING, ch)

  if (allocated(RPAT)) deallocate(RPAT)
  allocate(RPAT(NA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="RAPT"
    ALC=.true.
    goto 003
  endif
  if(allocated(RES_LIST)) deallocate(RES_LIST)
  allocate(RES_LIST(TAILLC), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="RES_LIST"
    ALC=.true.
    goto 003
  endif
  if(allocated(INDTE)) deallocate(INDTE)
  allocate(INDTE(NUMA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="INDTE"
    goto 003
  endif
  if(allocated(SAVR)) deallocate(SAVR)
  allocate(SAVR(TAILLC,NUMA,TAILLC), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="SAVR"
    ALC=.true.
    goto 003
  endif
  if(allocated(TRING)) deallocate(TRING)
  allocate(TRING(TAILLC), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="TRING"
    ALC=.true.
    goto 003
  endif
  if(allocated(THE_CHAIN)) deallocate(THE_CHAIN)
  allocate(THE_CHAIN(TAILLC), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="THE_CHAIN"
    ALC=.true.
    goto 003
  endif

  TRING(:)=0
  SAVR(:,:,:)=0
  !$OMP DO SCHEDULE(STATIC,NA/NUMTH)
  do j=1, NA

    if (TBR .or. ALC) goto 002
    if (TLT .eq. NSP+1 .or. LOT(j) .eq. TLT) then

      if ((ISOLATED .and. CPAT(j).eq.1) .or. (.not.ISOLATED .and. (CPAT(j).gt.0 .and. CPAT(j).ne.2))) then

        do l=1, CPAT(j)

          LORA=LOT(j)
          LORB=LOT(VPAT(j,l))
          if (AAAA) then
            if (LORA .ne. LORB) then
              RUNSEARCH=.false.
            else
              RUNSEARCH=.true.
           endif
          else if (ACAC) then
            if (LORA .ne. LORB) then
              RUNSEARCH=.true.
            else
              RUNSEARCH=.false.
            endif
          else if (NOHP .and. LORA.eq.LORB) then
            RUNSEARCH=.false.
          else
            RUNSEARCH=.true.
          endif

          if (RUNSEARCH) then

            RPAT(:) = 0
            do k=1, CPAT(j)
              RPAT(VPAT(j,k)) = 1
            enddo
            RPAT(j)=1
            THE_CHAIN(1)%ATOM=j
            THE_CHAIN(1)%SPEC=LORA
            THE_CHAIN(1)%NEIGHBOR=1
            THE_CHAIN(2)%ATOM=VPAT(j,l)
            THE_CHAIN(2)%SPEC=LORB
            THE_CHAIN(2)%NEIGHBOR=CPAT(VPAT(j,l))
            RES_LIST(:) = 0
            INDTE(:) = 0
            TAILLE=0
            RES = CHECK_CHAIN (THE_CHAIN, 2, TAILLE, SAVR, TRING, INDTE, RES_LIST)
            if (ALC) ALC_TAB="CHECK_CHAIN"
            if (TBR .or. ALC) goto 002
            if (CPAT(VPAT(j,l)) .eq. 2) then
              call INSIDE_CHAIN (THE_CHAIN, 2, TAILLE, LORA, LORB, RPAT, SAVR, TRING, INDTE, RES_LIST, CPAT, VPAT)
              if (ALC) ALC_TAB="INSIDE_CHAIN"
            endif
            if (TBR .or. ALC) goto 002

          endif
        enddo
      endif
    endif

    002 continue
  enddo
  !$OMP END DO NOWAIT
  if (TBR .or. ALC) goto 003
  !$OMP CRITICAL
  do k=2, TAILLC
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
            if (.not.SAUT) then
              SAUT=.true.
              do n=1, k
                if (SAVRING(k,m,k-n+1) .ne. SAVR(k,l,n)) then
                  SAUT=.false.
                  exit
                endif
              enddo
            endif
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
            enddo
          endif
        enddo
        NRING(k,i)=NRING(k,i)+o
      else
        do l=1, TRING(k)
          do m=1, k
            SAVRING(k,l,m) = SAVR(k,l,m)
          enddo
        enddo
        NRING(k,i) = TRING(k)
      endif
    endif
  enddo

  004 continue
  !$OMP END CRITICAL

  003 continue

  if (allocated(RPAT)) deallocate (RPAT)
  if (allocated(RES_LIST)) deallocate (RES_LIST)
  if (allocated(INDTE)) deallocate (INDTE)
  if (allocated(TRING)) deallocate (TRING)
  if (allocated(SAVR)) deallocate (SAVR)
  if (allocated(THE_CHAIN)) deallocate (THE_CHAIN)
  !$OMP END PARALLEL
  if (ALC .or. TBR) goto 001

  !do j=2, TAILLC
  !  write (6, '("s= ",i4,", j= ",i2,", nr(",i2,",",i4,")= ",i2)') i,j,j,i, NRING(j,i)
  !  if (NRING(j,i) .gt. 0) then
  !    do k=1, NRING(j,i)
  !      write (6, *) "   k= ",k,", R(o)= ",SAVRING(j,k,1:j)
  !    enddo
  !  endif
  !enddo
  ch = ch + CHAINS_TO_OGL (i, NRING, SAVRING)

enddo

001 continue

if (ALC) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: CHAINS_SEARCH_ATOMS"//CHAR(0), "Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif

if (allocated(CPAT)) deallocate (CPAT)
if (allocated(VPAT)) deallocate (VPAT)
if (allocated(SAVRING)) deallocate (SAVRING)

if (ch .eq. NS) ch = CHAINS_TO_OGL_MENU (NRING)

END SUBROUTINE
#endif

#ifdef OPENMP
SUBROUTINE CHAINS_SEARCH_STEPS (NUMTH)

USE PARAMETERS
!$ USE OMP_LIB
IMPLICIT NONE
INTEGER, INTENT(IN) :: NUMTH
#else
SUBROUTINE CHAINS_SEARCH_STEPS ()

USE PARAMETERS
IMPLICIT NONE
#endif
TYPE (RING), DIMENSION(:), ALLOCATABLE :: THE_CHAIN
INTEGER, DIMENSION(:), ALLOCATABLE :: TRING, INDTE
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVRING
INTEGER :: RES, LORA, LORB, ch

INTERFACE
  INTEGER FUNCTION CHECK_CHAIN (THE_CHAIN, CHAINE, TAE, RSAVED, TRING, INDE, RESL)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLC), INTENT(IN) :: THE_CHAIN
    INTEGER, INTENT(IN) :: CHAINE
    INTEGER, INTENT(INOUT) :: TAE
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: TRING
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL
  END FUNCTION
  RECURSIVE SUBROUTINE INSIDE_CHAIN (THE_CHAIN, CID, TAE, LRA, LRB, NRPAT, RSAVED, TRING, INDE, RESL, CPT, VPT)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLC), INTENT(INOUT) :: THE_CHAIN
    INTEGER, INTENT(IN) :: CID, LRA, LRB
    INTEGER, INTENT(INOUT) :: TAE
    INTEGER, DIMENSION(NA), INTENT(INOUT) :: NRPAT
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: TRING
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL
    INTEGER, DIMENSION(NA), INTENT(IN):: CPT
    INTEGER, DIMENSION(NA,MAXN), INTENT(IN) :: VPT
  END SUBROUTINE
  INTEGER FUNCTION CHAINS_TO_OGL (STEP, NRI, RSAVED)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: STEP
    INTEGER, DIMENSION(TAILLC, NS), INTENT(IN) :: NRI
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(IN) :: RSAVED
  END FUNCTION
  INTEGER FUNCTION CHAINS_TO_OGL_MENU (NRI)
    USE PARAMETERS
    INTEGER, DIMENSION(TAILLC, NS), INTENT(IN) :: NRI
  END FUNCTION
END INTERFACE

ch = 0

if (allocated(NRING)) deallocate(NRING)
allocate(NRING(TAILLC,NS), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="NRING"
  ALC=.true.
  goto 001
endif
NRING(:,:) = 0

#ifdef OPENMP
! OpenMP on steps only
!$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
!$OMP& PRIVATE(THE_CHAIN, RPAT, RUNSEARCH, ERR, SAVRING, TRING, INDTE, &
!$OMP& j, k, l, m, n, o, LORA, LORB, RES, RES_LIST, CPAT, VPAT, TAILLE) &
!$OMP& SHARED(i, p, NUMTH, NS, NA, TLT, NSP, LOT, ISOLATED, CONTJ, VOISJ, &
!$OMP& NUMA, MAXN, ACAC, AAAA, NOHP, TAILLC, TBR, ALC, ALC_TAB, NCELLS, PBC, NRING, ch)
#endif

if(allocated(SAVRING)) deallocate(SAVRING)
allocate(SAVRING(TAILLC,NUMA,TAILLC), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="SAVRING"
  ALC=.true.
  goto 002
endif
if(allocated(CPAT)) deallocate(CPAT)
allocate(CPAT(NA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="CPAT"
  ALC=.true.
  goto 002
endif
 if(allocated(VPAT)) deallocate(VPAT)
allocate(VPAT(NA,MAXN), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="VPAT"
  ALC=.true.
  goto 002
endif
if (allocated(RPAT)) deallocate(RPAT)
allocate(RPAT(NA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="RPAT"
  ALC=.true.
  goto 002
endif
if(allocated(RES_LIST)) deallocate(RES_LIST)
allocate(RES_LIST(TAILLC), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="REST_LIST"
  ALC=.true.
  goto 002
endif
if(allocated(INDTE)) deallocate(INDTE)
allocate(INDTE(NUMA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="INDTE"
  goto 002
endif
if(allocated(TRING)) deallocate(TRING)
allocate(TRING(TAILLC), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="TRING"
  ALC=.true.
  goto 002
endif
if(allocated(THE_CHAIN)) deallocate(THE_CHAIN)
allocate(THE_CHAIN(TAILLC), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="THE_CHAIN"
  ALC=.true.
  goto 002
endif

#ifdef OPENMP
!$OMP DO SCHEDULE(STATIC,NS/NUMTH)
#endif
do i=1, NS

  if (TBR .or. ALC) goto 003
  SAVRING(:,:,:)=0
  TRING(:)=0
  call SETUP_CPAT_VPAT_CHAIN (CONTJ, VOISJ, i, CPAT, VPAT)

  do j=1, NA

    if (TLT .eq. NSP+1 .or. LOT(j) .eq. TLT) then

      if ((ISOLATED .and. CPAT(j).eq.1) .or. (.not.ISOLATED .and. (CPAT(j).gt.0 .and. CPAT(j).ne.2))) then

        do l=1, CPAT(j)

          LORA=LOT(j)
          LORB=LOT(VPAT(j,l))
          if (AAAA) then
            if (LORA .ne. LORB) then
              RUNSEARCH=.false.
            else
              RUNSEARCH=.true.
           endif
          else if (ACAC) then
            if (LORA .ne. LORB) then
              RUNSEARCH=.true.
            else
              RUNSEARCH=.false.
            endif
          else if (NOHP .and. LORA.eq.LORB) then
            RUNSEARCH=.false.
          else
            RUNSEARCH=.true.
          endif

          if (RUNSEARCH) then

            RPAT(:) = 0
            do k=1, CPAT(j)
              RPAT(VPAT(j,k)) = 1
            enddo
            RPAT(j)=1
            THE_CHAIN(1)%ATOM=j
            THE_CHAIN(1)%SPEC=LORA
            THE_CHAIN(1)%NEIGHBOR=1
            THE_CHAIN(2)%ATOM=VPAT(j,l)
            THE_CHAIN(2)%SPEC=LORB
            THE_CHAIN(2)%NEIGHBOR=CPAT(VPAT(j,l))
            RES_LIST(:) = 0
            INDTE(:) = 0
            TAILLE = 0
            RES = CHECK_CHAIN (THE_CHAIN, 2, TAILLE, SAVRING, TRING, INDTE, RES_LIST)
            if (ALC) ALC_TAB="CHECK_CHAIN"
            if (TBR .or. ALC) goto 003
            if (CPAT(VPAT(j,l)) .eq. 2) then
              call INSIDE_CHAIN (THE_CHAIN, 2, TAILLE, LORA, LORB, RPAT, SAVRING, TRING, INDTE, RES_LIST, CPAT, VPAT)
              if (ALC) ALC_TAB="INSIDE_CHAIN"
            endif
            if (TBR .or. ALC) goto 003

          endif
        enddo
      endif
    endif

  enddo

  do j=1, TAILLC
    NRING(j,i) = TRING(j)
  enddo

  !do j=2, TAILLC
  !  write (6, '("s= ",i4,", j= ",i2,", nr(",i2,",",i4,")= ",i2)') i,j,j,i, NRING(j,i)
  !  if (NRING(j,i) .gt. 0) then
  !    do k=1, NRING(j,i)
  !      write (6, *) "   k= ",k,", R(o)= ",SAVRING(j,k,1:j)
  !    enddo
  !  endif
  !enddo

  j = CHAINS_TO_OGL (i, NRING, SAVRING)
#ifdef OPENMP
  !$OMP ATOMIC
#endif
  ch = ch + j

  003 continue

enddo
#ifdef OPENMP
!$OMP END DO NOWAIT
#endif

002 continue

if (ALC) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: CHAINS_SEARCH_STEPS"//CHAR(0), "Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif

if (allocated(TRING)) deallocate (TRING)
if (allocated(THE_CHAIN)) deallocate (THE_CHAIN)
if (allocated(SAVRING)) deallocate (SAVRING)
if (allocated(CPAT)) deallocate (CPAT)
if (allocated(VPAT)) deallocate (VPAT)
if (allocated(RPAT)) deallocate (RPAT)
if (allocated(RES_LIST)) deallocate (RES_LIST)
if (allocated(INDTE)) deallocate (INDTE)

#ifdef OPENMP
!$OMP END PARALLEL
#endif

if (ch .eq. NS) ch = CHAINS_TO_OGL_MENU (NRING)

001 continue

END SUBROUTINE

INTEGER FUNCTION CHECK_CHAIN (THE_CHAIN, CHAINE, TAE, RSAVED, TRING, INDE, RESL)

USE PARAMETERS

IMPLICIT NONE

TYPE (RING), DIMENSION(TAILLC), INTENT(IN) :: THE_CHAIN
INTEGER, INTENT(IN) :: CHAINE
INTEGER, INTENT(INOUT) :: TAE
INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: TRING
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE
INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL
INTERFACE
  SUBROUTINE SAVE_THIS_CHAIN (THE_CHAIN, TLES, RSAVED, TRING, INDT, RESL)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLC), INTENT(IN) :: THE_CHAIN
    INTEGER, INTENT(IN) :: TLES
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL, TRING
  END SUBROUTINE
  SUBROUTINE DEL_THIS_CHAIN (TLED, RSAVED, TRING, RESL, INDT)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: TLED
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL, TRING
  END
END INTERFACE

  CHECK_CHAIN = 0
  if (CHAINE .ge. TAE .and. CHAINE .le. TAILLC) then
    if (TAE .gt. 0) call DEL_THIS_CHAIN (TAE, RSAVED, TRING, RESL, INDE)
    TAE=CHAINE
    call SAVE_THIS_CHAIN (THE_CHAIN, TAE, RSAVED, TRING, INDE, RESL)
    if (TBR .or. ALC) CHECK_CHAIN = 1
  endif

END FUNCTION

RECURSIVE SUBROUTINE INSIDE_CHAIN (THE_CHAIN, CID, TAE, LRA, LRB, &
                                   NRPAT, RSAVED, TRING, INDE, RESL, CPT, VPT)
USE PARAMETERS

IMPLICIT NONE

TYPE (RING), DIMENSION(TAILLC), INTENT(INOUT) :: THE_CHAIN
INTEGER, INTENT(IN) :: CID, LRA, LRB
INTEGER, INTENT(INOUT) :: TAE
INTEGER, DIMENSION(NA), INTENT(INOUT) :: NRPAT
INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: TRING
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE
INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL
INTEGER, DIMENSION(NA), INTENT(IN):: CPT
INTEGER, DIMENSION(NA,MAXN), INTENT(IN) :: VPT
INTEGER :: IND, RES
LOGICAL :: ADDSP

INTERFACE
  INTEGER FUNCTION CHECK_CHAIN (THE_CHAIN, CHAINE, TAE, RSAVED, TRING, INDE, RESL)
    USE PARAMETERS
    TYPE (RING), DIMENSION(TAILLC), INTENT(IN) :: THE_CHAIN
    INTEGER, INTENT(IN) :: CHAINE
    INTEGER, INTENT(INOUT) :: TAE
    INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: TRING
    INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDE
    INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL
  END FUNCTION
END INTERFACE

if (CID .lt. TAILLC) then

  do while (THE_CHAIN(CID)%NEIGHBOR .ge. 1)

    IND = VPT(THE_CHAIN(CID)%ATOM, THE_CHAIN(CID)%NEIGHBOR)
    if (NRPAT(IND).eq.0 .and. CPT(IND).ge.1) then

      if (AAAA) then
        if (THE_CHAIN(CID)%SPEC .eq. LOT(IND)) then
          ADDSP=.true.
        else
          ADDSP=.false.
        endif
      else if (ACAC) then
        if (mod(CID,2).ne.0 .and. LOT(IND).eq.LRB) then
          ADDSP=.true.
        else if (mod(CID,2).eq.0 .and. LOT(IND).eq.LRA) then
          ADDSP=.true.
        else
          ADDSP=.false.
        endif
      else if (NOHP .and. THE_CHAIN(CID)%SPEC.eq.LOT(IND)) then
        ADDSP=.false.
      else
        ADDSP=.true.
      endif

      if (ADDSP .and. ((ISOLATED .and. CPT(IND).le.2) .or. .not.ISOLATED)) then
        THE_CHAIN(CID+1)%ATOM = IND
        THE_CHAIN(CID+1)%SPEC = LOT(IND)
        THE_CHAIN(CID+1)%NEIGHBOR = CPT(IND)
        NRPAT(IND)=1
        RES = CHECK_CHAIN (THE_CHAIN, CID+1, TAE, RSAVED, TRING, INDE, RESL)
        if (TBR .or. ALC) goto 001
        if (RES.eq.0 .and. CPT(IND).eq.2) then
          call INSIDE_CHAIN(THE_CHAIN, CID+1, TAE, LRA, LRB, NRPAT, RSAVED, TRING, INDE, RESL, CPT, VPT)
          if (TBR .or. ALC) goto 001
        endif
        NRPAT(IND) = 0
      endif

    endif
    THE_CHAIN(CID)%NEIGHBOR = THE_CHAIN(CID)%NEIGHBOR - 1

  enddo

endif

001 continue

END SUBROUTINE INSIDE_CHAIN

SUBROUTINE SAVE_THIS_CHAIN (THE_CHAIN, TLES, RSAVED, TRING, INDT, RESL)

USE PARAMETERS

IMPLICIT NONE

TYPE (RING), DIMENSION(TAILLC), INTENT(IN) :: THE_CHAIN
INTEGER, INTENT(IN) :: TLES
INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL, TRING
INTEGER :: idx, idy
LOGICAL :: NEWCHAIN
INTEGER, DIMENSION(TLES) :: TOSAV

! A ring has been found, we need to check if it has already been found or not

do idx=1, TLES

! Creation of two tab first for the numerical sorting of the list
! The other without sorting for output
  TOSAV(idx)=THE_CHAIN(idx)%ATOM

enddo

if (TRING(TLES) .ne. 0) then

  do idx=1, TRING(TLES)
!   do-loop on existing rings to check if the new on has already been found

    NEWCHAIN=.false.

    do idy=1, TLES
      if (TOSAV(idy) .ne. RSAVED(TLES,idx,idy)) then
        NEWCHAIN=.true.
        exit
      endif
    enddo

    if (NEWCHAIN) then
      NEWCHAIN=.false.
      do idy=1, TLES
        if (TOSAV(TLES-idy+1) .ne. RSAVED(TLES,idx,idy)) then
          NEWCHAIN=.true.
          exit
        endif
      enddo
    endif

    if (.not.NEWCHAIN) then

      ! Already been found n-times, increment of the counter
      INDT(idx)=INDT(idx)+1
      exit

    endif

  enddo

else

  NEWCHAIN=.true.

endif

if (NEWCHAIN) then
  RESL(TLES)=RESL(TLES)+1
  TRING(TLES)=TRING(TLES)+1
  if (TRING(TLES) .gt. NUMA) then
    TBR=.true.
    goto 001
  endif
  INDT(TRING(TLES))=INDT(TRING(TLES))+1
  do idx=1, TLES
    RSAVED(TLES,TRING(TLES),idx)=TOSAV(idx)
  enddo
endif

001 continue

END SUBROUTINE

SUBROUTINE DEL_THIS_CHAIN (TLED, RSAVED, TRING, RESL, INDT)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: TLED
INTEGER, DIMENSION(TAILLC,NUMA,TAILLC), INTENT(INOUT) :: RSAVED
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
INTEGER, DIMENSION(TAILLC), INTENT(INOUT) :: RESL, TRING
INTEGER :: xdel, did

! The max size of the ring possible for the triplet N1-At-N2 has down
! We have to delete the bigger rings already found for this triplet
if (RESL(TLED) .ge. 1) then

  do xdel=0, RESL(TLED)-1

    do did=1, TAILLC
      RSAVED(TLED,TRING(TLED)-xdel,did)=0
    enddo
    INDT(:) = 0

  enddo

  TRING(TLED)=TRING(TLED)-RESL(TLED)
  RESL(TLED)=0

endif

END SUBROUTINE
