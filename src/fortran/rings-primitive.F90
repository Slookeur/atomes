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
!! @file rings-primitive.F90
!! @short Primitive and strong ring statistics
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER FUNCTION PRIMITIVE_RINGS ()

!
! Primitive and strong ring statistics
!
#ifdef OPENMP
!$ USE OMP_LIB
#endif
USE PARAMETERS

IMPLICIT NONE

INTEGER :: RID
#ifdef OPENMP
INTEGER :: NUMTH
LOGICAL :: DOATOMS=.false.
#endif

INTERFACE
  INTEGER FUNCTION RECRINGS(VID)
    INTEGER, INTENT(IN) :: VID
  END FUNCTION
END INTERFACE

PRIMITIVE_RINGS=0

! Dynamic allocation of pointers and tables used in the "RINGS" subroutine

RID = 3
if (CALC_STRINGS) RID = 4

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
  call PRIMITIVE_RING_SEARCH_ATOMS (RID, NUMTH)
else
#ifdef DEBUG
  write (6, *) "OpenMP on MD steps, NUMTH= ",NUMTH
#endif
  call PRIMITIVE_RING_SEARCH_STEPS (RID, NUMTH)
endif
#else
call PRIMITIVE_RING_SEARCH_STEPS (RID)
#endif

PRIMITIVE_RINGS = RECRINGS(RID)

END FUNCTION

#ifdef OPENMP
SUBROUTINE PRIMITIVE_RING_SEARCH_ATOMS (RID, NUMTH)

USE PARAMETERS
!$ USE OMP_LIB
IMPLICIT NONE
INTEGER, INTENT(IN) :: NUMTH
INTEGER, INTENT(IN) :: RID
INTEGER, DIMENSION(:), ALLOCATABLE :: TRING, INDTE
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVRING, ORDRING
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVR, ORDR
INTEGER :: ri
LOGICAL, DIMENSION(2) :: FNDTAB
INTERFACE
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
allocate(CPAT(NNA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="CPAT"
  ALC=.true.
  goto 001
endif
 if(allocated(VPAT)) deallocate(VPAT)
allocate(VPAT(NNA,MAXN), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="VPAT"
  ALC=.true.
  goto 001
endif

do i=1, NS

  SAVRING(:,:,:)=0
  ORDRING(:,:,:)=0
  call SETUP_CPAT_VPAT_RING (NNA, i, CONTJ, VOISJ, CPAT, VPAT)
  ! OpenMP on atoms only
  !$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
  !$OMP& PRIVATE(FNDTAB, MAXAT, MINAT, SAUT, PATH, PATHOUT, &
  !$OMP& h, j, k, l, m, n, o, p, INDTE, APNA, RES_LIST, &
  !$OMP& ERR, TRING, SAVR, ORDR, PRINGORD, NPRING, MATDIST, QUEUE, QUERNG) &
  !$OMP& SHARED(NUMTH, i, RID, CALC_STRINGS, NS, NA, NNA, NNP, TLT, NSP, LOT, TAILLR, CONTJ, VOISJ, &
  !$OMP& NUMA, MAXPNA, MINPNA, ABAB, NO_HOMO, TBR, ALC, ALC_TAB, SAVRING, ORDRING, CPAT, VPAT, &
  !$OMP& NCELLS, THE_BOX, FULLPOS, PBC, MAXN, NRING, INDRING, PNA, ri)
  if(allocated(MATDIST)) deallocate(MATDIST)
  allocate(MATDIST(NNA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="MATDIST"
    ALC=.true.
    goto 002
  endif
  if(allocated(QUEUE)) deallocate(QUEUE)
  allocate(QUEUE(NNA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="QUEUE"
    ALC=.true.
    goto 002
  endif
  if (allocated(PRINGORD)) deallocate(PRINGORD)
  allocate(PRINGORD(NUMA*10,TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="PRINGORD"
    ALC=.false.
    goto 002
  endif
  if (allocated(NPRING)) deallocate(NPRING)
  allocate(NPRING(NNA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="NPRING"
    ALC=.false.
    goto 002
  endif
  if(allocated(RES_LIST)) deallocate(RES_LIST)
  allocate(RES_LIST(TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="RES_LIST"
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
  if(allocated(INDTE)) deallocate(INDTE)
  allocate(INDTE(NUMA), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="INDTE"
    goto 002
  endif
  if (allocated(TRING)) deallocate(TRING)
  allocate(TRING(TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="TRING"
    ALC=.true.
    goto 002
  endif
  if(allocated(APNA)) deallocate(APNA)
  allocate(APNA(TAILLR), STAT=ERR)
  if (ERR .ne. 0) then
    ALC_TAB="APNA"
    ALC=.true.
    goto 002
  endif
  SAVR(:,:,:)=0
  ORDR(:,:,:)=0
  TRING(:)=0
  !$OMP DO SCHEDULE(STATIC,NA/NUMTH)
  do j=NNP+1, NNP+NA ! atoms-loop

    if (TBR .or. ALC) goto 003
    if (TLT .eq. NSP+1 .or. LOT(j-NNP) .eq. TLT) then

      APNA(:)=0
      MINAT=TAILLR
      MAXAT=1
      SAUT=.true.

      call DIJKSTRA (j, CPAT, VPAT, QUEUE, MATDIST)
      if (TBR .or. ALC) goto 003

      do k=1, TAILLR/2 + mod(TAILLR,2) ! ring-sizes-loop

        INDTE(:)=0
        RES_LIST(:)=0
        PATH=0
        do l=1, NNA
          if (MATDIST(l) .eq. k) then
            call SPATH_REC (PATH,l,k,k,MATDIST,CPAT,VPAT,NPRING,PRINGORD)
          endif
        enddo
        h = PATH*(PATH-1)/2
        if (allocated(QUERNG)) deallocate(QUERNG)
        allocate(QUERNG(h,2), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="QUERNG"
          ALC=.true.
          goto 003
        endif

        l=0
        do m=1, PATH-1
          do n=m+1, PATH
            l=l+1
            QUERNG(l,1)=m
            QUERNG(l,2)=n
!           Paths which share atoms ... so with overlap ... are deleted
            PATHOUT=.false.
            do o=1, k-1
              do p=1, k-1
                if (PRINGORD(m,o) .eq. PRINGORD(n,p)) then
                  PATHOUT=.true.
                  exit
                endif
              enddo
              if (PATHOUT) exit
            enddo
            if (PATHOUT) then
              QUERNG(l,1)=0
              QUERNG(l,2)=0
              l=l-1
            endif
          enddo
        enddo
        FNDTAB(:)=.false.
        call PRIM_RING (FNDTAB, j, l, k, h, CPAT, VPAT, QUERNG, PRINGORD, MATDIST, &
                        SAVR, ORDR, TRING, INDTE, RES_LIST)
        if (TBR .or. ALC) goto 003
        m = 2*k
        if (FNDTAB(1)) then
          if (APNA(m).eq.0) then
            APNA(m)=1
            MINAT=min(MINAT,m)
            MAXAT=max(MAXAT,m)
            SAUT=.false.
          endif
        endif
        if (FNDTAB(2)) then
          m = m + 1
          if (APNA(m).eq.0) then
            APNA(m)=1
            MINAT=min(MINAT,m)
            MAXAT=max(MAXAT,m)
            SAUT=.false.
          endif
        endif
        if (allocated(QUERNG)) deallocate(QUERNG)

      enddo ! end ring-sizes-loop

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

  enddo ! end atoms-loop
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

  if (allocated(INDTE)) deallocate (INDTE)
  if (allocated(APNA)) deallocate (APNA)
  if (allocated(TRING)) deallocate (TRING)
  if (allocated(SAVR)) deallocate (SAVR)
  if (allocated(ORDR)) deallocate (ORDR)
  if (allocated(MATDIST)) deallocate(MATDIST)
  if (allocated(QUEUE)) deallocate(QUEUE)
  if (allocated(PRINGORD)) deallocate(PRINGORD)

  !$OMP END PARALLEL

  if (ALC .or. TBR) goto 001
  ri = ri + RINGS_TO_OGL (i, RID, NRING, SAVRING, ORDRING)

  do k=3, TAILLR
    if (NRING(k,i) .ne. 0) then
!      do j=1, NRING(k,i)
!!  The algorithm implies that you may have a problem for
!!  the biggest size of ring therefore lets put this size out of the proof checking.
!!  Furthermore we check results only if all atoms are used to initiate the search.
!        if (mod(INDRING(k,j,i), k).ne.0 .and. k.lt.TAILLR .and. LTLT.eq.0) then
!          write (6, 003) i, k, j, INDRING(k,j,i)
!          write (6, 007)
!          do l=1, k
!            write (6, '(i4,2x)', advance='no') RINGORD(k,j,l,i)
!          enddo
!          write (6, *)
!          write (6, 004)
!          if (ABAB) then
!            write (6, 008)
!          else
!            write (6, 005)
!          endif
!          write (6, *)
!        endif
!      enddo
    endif
  enddo

enddo

001 continue

if (ALC) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: PRIMITIVE_RING_SEARCH_ATOMS"//CHAR(0), "Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif

if (allocated(CPAT)) deallocate (CPAT)
if (allocated(VPAT)) deallocate (VPAT)
if (allocated(SAVRING)) deallocate (SAVRING)
if (allocated(ORDRING)) deallocate (ORDRING)

if (ri .eq. NS) ri = RINGS_TO_OGL_MENU (RID, NRING)

END SUBROUTINE
#endif

#ifdef OPENMP
SUBROUTINE PRIMITIVE_RING_SEARCH_STEPS (RID, NUMTH)

USE PARAMETERS
!$ USE OMP_LIB
IMPLICIT NONE
INTEGER, INTENT(IN) :: NUMTH
#else
SUBROUTINE PRIMITIVE_RING_SEARCH_STEPS (RID)

USE PARAMETERS
IMPLICIT NONE
#endif
INTEGER, INTENT(IN) :: RID
INTEGER, DIMENSION(:), ALLOCATABLE :: TRING, INDTE
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: SAVRING, ORDRING
INTEGER :: ri
LOGICAL, DIMENSION(2) :: FNDTAB
INTERFACE
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
END INTERFACE

ri = 0
#ifdef OPENMP
! OpenMP on steps only
!$OMP PARALLEL NUM_THREADS(NUMTH) DEFAULT (NONE) &
!$OMP& PRIVATE(FNDTAB, MAXAT, MINAT, SAUT, PATH, PATHOUT, &
!$OMP& h, i, j, k, l, m, n, o, p, INDTE, APNA, RES_LIST, &
!$OMP& ERR, TRING, SAVRING, ORDRING, CPAT, VPAT, &
!$OMP& PRINGORD, NPRING, MATDIST, QUEUE, QUERNG) &
!$OMP& SHARED(NUMTH, RID, CALC_STRINGS, NS, NA, NNA, NNP, TLT, NSP, LOT, TAILLR, CONTJ, VOISJ, &
!$OMP& NUMA, MAXPNA, MINPNA, ABAB, NO_HOMO, TBR, ALC, ALC_TAB, &
!$OMP& NCELLS, THE_BOX, FULLPOS, PBC, MAXN, NRING, INDRING, PNA, ri)
#endif
if(allocated(MATDIST)) deallocate(MATDIST)
allocate(MATDIST(NNA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="MATDIST"
  ALC=.true.
  goto 001
endif
if(allocated(QUEUE)) deallocate(QUEUE)
allocate(QUEUE(NNA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="QUEUE"
  ALC=.true.
  goto 001
endif
if (allocated(PRINGORD)) deallocate(PRINGORD)
allocate(PRINGORD(NUMA*10,TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="PRINGORD"
  ALC=.false.
  goto 001
endif
if (allocated(NPRING)) deallocate(NPRING)
allocate(NPRING(NNA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="NPRING"
  ALC=.false.
  goto 001
endif
if(allocated(CPAT)) deallocate(CPAT)
allocate(CPAT(NNA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="CPAT"
  ALC=.true.
  goto 001
endif
if(allocated(VPAT)) deallocate(VPAT)
allocate(VPAT(NNA,MAXN), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="VPAT"
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
if(allocated(INDTE)) deallocate(INDTE)
allocate(INDTE(NUMA), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="INDTE"
  goto 001
endif
if (allocated(TRING)) deallocate(TRING)
allocate(TRING(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="TRING"
  ALC=.true.
  goto 001
endif
if(allocated(APNA)) deallocate(APNA)
allocate(APNA(TAILLR), STAT=ERR)
if (ERR .ne. 0) then
  ALC_TAB="APNA"
  ALC=.true.
  goto 001
endif

!$OMP DO SCHEDULE(STATIC,NS/NUMTH)
do i=1, NS

  if (TBR .or. ALC) goto 002
  SAVRING(:,:,:)=0
  ORDRING(:,:,:)=0
  TRING(:)=0
  call SETUP_CPAT_VPAT_RING (NNA, i, CONTJ, VOISJ, CPAT, VPAT)

  do j=NNP+1, NNP+NA ! atoms-loop

    if (TLT .eq. NSP+1 .or. LOT(j-NNP) .eq. TLT) then

      APNA(:)=0
      MINAT=TAILLR
      MAXAT=1
      SAUT=.true.

      call DIJKSTRA(j, CPAT, VPAT, QUEUE, MATDIST)
      if (TBR .or. ALC) goto 002

      do k=1, TAILLR/2 + mod(TAILLR,2) ! ring-sizes-loop

        INDTE(:)=0
        RES_LIST(:)=0
        PATH=0
        do l=1, NNA
          if (MATDIST(l) .eq. k) then
            call SPATH_REC (PATH,l,k,k,MATDIST,CPAT,VPAT,NPRING,PRINGORD)
          endif
        enddo
        h = PATH*(PATH-1)/2
        if (allocated(QUERNG)) deallocate(QUERNG)
        allocate(QUERNG(h,2), STAT=ERR)
        if (ERR .ne. 0) then
          ALC_TAB="QUERNG"
          ALC=.true.
          goto 002
        endif

        l=0
        do m=1, PATH-1
          do n=m+1, PATH
            l=l+1
            QUERNG(l,1)=m
            QUERNG(l,2)=n
!           Paths which share atoms ... so with overlap ... are deleted
            PATHOUT=.false.
            do o=1, k-1
              do p=1, k-1
                if (PRINGORD(m,o) .eq. PRINGORD(n,p)) then
                  PATHOUT=.true.
                  exit
                endif
              enddo
              if (PATHOUT) exit
            enddo
            if (PATHOUT) then
              QUERNG(l,1)=0
              QUERNG(l,2)=0
              l=l-1
            endif
          enddo
        enddo
        FNDTAB(:)=.false.
        call PRIM_RING (FNDTAB, j, l, k, h, CPAT, VPAT, QUERNG, PRINGORD, MATDIST, &
                        SAVRING, ORDRING, TRING, INDTE, RES_LIST)
        if (TBR .or. ALC) goto 002
        m = 2*k
        if (FNDTAB(1)) then
          if (APNA(m).eq.0) then
            APNA(m)=1
            MINAT=min(MINAT,m)
            MAXAT=max(MAXAT,m)
            SAUT=.false.
          endif
        endif
        if (FNDTAB(2)) then
          m = m + 1
          if (APNA(m).eq.0) then
            APNA(m)=1
            MINAT=min(MINAT,m)
            MAXAT=max(MAXAT,m)
            SAUT=.false.
          endif
        endif
        if (allocated(QUERNG)) deallocate(QUERNG)

      enddo ! end ring-sizes-loop

      if (.not. SAUT) then
        do k=3, TAILLR
          do m=3, TAILLR
            if (APNA(k).eq.1 .and. APNA(m).eq.1) then
              PNA(k,m,i)=PNA(k,m,i)+1
            endif
          enddo
        enddo
        MAXPNA(MAXAT,i)=MAXPNA(MAXAT,i)+1
        MINPNA(MINAT,i)=MINPNA(MINAT,i)+1
      endif

    endif

  enddo ! end atoms-loop

  do j=3, TAILLR
    NRING(j,i) = TRING(j)
  enddo

  k = RINGS_TO_OGL (i, RID, NRING, SAVRING, ORDRING)
  !$OMP ATOMIC
  ri = ri + k

  do k=3, TAILLR
    if (NRING(k,i) .ne. 0) then
!      do j=1, NRING(k,i)
!!  The algorithm implies that you may have a problem for
!!  the biggest size of ring therefore lets put this size out of the proof checking.
!!  Furthermore we check results only if all atoms are used to initiate the search.
!        if (mod(INDRING(k,j,i), k).ne.0 .and. k.lt.TAILLR .and. LTLT.eq.0) then
!          write (6, 003) i, k, j, INDRING(k,j,i)
!          write (6, 007)
!          do l=1, k
!            write (6, '(i4,2x)', advance='no') RINGORD(k,j,l,i)
!          enddo
!          write (6, *)
!          write (6, 004)
!          if (ABAB) then
!            write (6, 008)
!          else
!            write (6, 005)
!          endif
!          write (6, *)
!        endif
!      enddo
    endif
  enddo

  002 continue

enddo
#ifdef OPENMP
!$OMP END DO NOWAIT
#endif

001 continue

if (ALC) then
  call show_error ("Impossible to allocate memory"//CHAR(0), &
                   "Subroutine: PRIMITIVE_RING_SEARCH_STEPS"//CHAR(0), "Table: "//ALC_TAB(1:LEN_TRIM(ALC_TAB))//CHAR(0))
endif

if (allocated(TRING)) deallocate (TRING)
if (allocated(SAVRING)) deallocate (SAVRING)
if (allocated(ORDRING)) deallocate (ORDRING)
if (allocated(CPAT)) deallocate (CPAT)
if (allocated(VPAT)) deallocate (VPAT)
if (allocated(INDTE)) deallocate (INDTE)
if (allocated(APNA)) deallocate (APNA)
if(allocated(MATDIST)) deallocate(MATDIST)
if(allocated(QUEUE)) deallocate(QUEUE)
if (allocated(PRINGORD)) deallocate(PRINGORD)

#ifdef OPENMP
!$OMP END PARALLEL
#endif

if (ri .eq. NS) ri = RINGS_TO_OGL_MENU (RID, NRING)

END SUBROUTINE

SUBROUTINE DIJKSTRA(NODE, CPT, VPT, QUE, MATDIS)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: NODE
INTEGER, DIMENSION(NNA), INTENT(INOUT) :: QUE, MATDIS
INTEGER, DIMENSION(NNA), INTENT(IN) :: CPT
INTEGER, DIMENSION(NNA,MAXN), INTENT(IN) :: VPT
INTEGER :: QBEGIN, QEND, QID, AT1, AT2, DAT1

QUE(:)=0
MATDIS(:)=NNA+2
QUE(1)=NODE
MATDIS(NODE)=0
QBEGIN=0
QEND=1

! Trouver les plus cours chemin reliant un atome aux autres
! Find the shortest paths between atoms

do while (QBEGIN < QEND)

  QBEGIN=QBEGIN+1
  AT1=QUE(QBEGIN)
  DAT1=MATDIS(AT1)+1

  do QID=1, CPT(AT1)

    AT2=VPT(AT1,QID)
    if (MATDIS(AT2) .gt. DAT1) then

      MATDIS(AT2) = DAT1
      if (DAT1 < NNA) then

        QEND=QEND+1
        QUE(QEND)= AT2

      endif

    endif

  enddo

enddo

END SUBROUTINE

RECURSIVE SUBROUTINE SPATH_REC (PTH, NODE, LENGTH, LNGTH, MATDIS, CPT, VPT, NPRI, PORDR)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(INOUT) :: PTH
INTEGER, INTENT(IN) :: NODE, LENGTH, LNGTH
INTEGER, DIMENSION(NNA), INTENT(IN) :: CPT, MATDIS
INTEGER, DIMENSION(NNA,MAXN), INTENT(IN) :: VPT
INTEGER, DIMENSION(NNA), INTENT(INOUT) :: NPRI
INTEGER, DIMENSION(NUMA*10,TAILLR), INTENT(INOUT) :: PORDR
INTEGER :: DISTNN, IDV, VDI, IDT

NPRI(LENGTH) = NODE
do IDV=1, CPT(NODE)

! Boucle sur tous les voisins du noeud "NODE"
! Loop on all the neighbors of node "NODE"

  VDI=VPT(NODE,IDV)                  ! Selection du voisin - neighbor selection
  DISTNN=MATDIS(VDI)                 ! Distance to starting atom = VDI in the node distance table

  if (DISTNN .eq. 0) then

    PTH=PTH+1
    do IDT=1, LNGTH
      PORDR(PTH,IDT)=NPRI(IDT)
    enddo

  elseif (DISTNN .eq. LENGTH-1) then

    call SPATH_REC (PTH, VDI, DISTNN, LNGTH, MATDIS, CPT, VPT, NPRI, PORDR)

  endif

enddo

END SUBROUTINE

INTEGER FUNCTION REAL_ATOM_ID (IND, NATS)

IMPLICIT NONE
INTEGER, INTENT(IN) :: IND, NATS

REAL_ATOM_ID=  IND - (IND/NATS)*NATS
if (REAL_ATOM_ID .eq. 0) REAL_ATOM_ID=NATS

END FUNCTION

SUBROUTINE PRIM_RING (FNDTAB, NODE, PTH, LGTH, NPT, CPT, VPT, QRNG, PORD, MATDIS, &
                      RSAVED, OSAVED, TRIN, INDP, RESLP)

USE PARAMETERS

IMPLICIT NONE

LOGICAL, DIMENSION(2), INTENT(INOUT) :: FNDTAB
INTEGER, INTENT(IN) :: NODE, PTH, LGTH, NPT
INTEGER, DIMENSION(NNA), INTENT(IN) :: CPT, MATDIS
INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: TRIN
INTEGER, DIMENSION(NNA,MAXN), INTENT(IN) :: VPT
INTEGER, DIMENSION(NPT,2), INTENT(IN) :: QRNG
INTEGER, DIMENSION(NUMA*10,TAILLR), INTENT(INOUT) :: PORD
INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: RSAVED, OSAVED
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDP
INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: RESLP
INTEGER :: RM, RN, PR, PTH1, PTH2, IRS, IRX
INTEGER :: ATB, ATA, ATC, ATD, ATE, MAXD, MIND
INTEGER :: PROBE
INTEGER, DIMENSION(TAILLR) :: TOPRIM, PRIMTO
LOGICAL:: GOAL, TOSAVE
LOGICAL, DIMENSION(NNA) :: CHK
INTERFACE
  INTEGER FUNCTION REAL_ATOM_ID (IND, NATS)
    INTEGER, INTENT(IN) :: IND, NATS
  END FUNCTION
END INTERFACE

GOAL=.false.

do PR=1, PTH

! Premier chemin i->j - First path i->j
  PTH1= QRNG(PR,1)
! Second chemin i->k - second path i->k
  PTH2= QRNG(PR,2)

  PROBE=0
  ATA= PORD(PTH1,LGTH)
  ATB= PORD(PTH2,LGTH)
  do RM=1, CPT(ATA)
    ATE=VPT(ATA,RM)
    if (ATE .eq. ATB) PROBE=1
  enddo

  if (ATA .eq. ATB .or. PROBE .eq. 1) then

    do RM=1, LGTH-1
    do RN=RM, LGTH-1+PROBE

      ATC= PORD(PTH1,RM)
      ATD= PORD(PTH2,RN)
      MAXD=MATDIS(ATC)+MATDIS(ATD)
      MIND=2*LGTH+PROBE-MAXD
      CHK(:)=.false.

      call PAIR_SEARCH (GOAL, ATC, ATD, 1, MAXD, MIND, CPT, VPT, CHK)
      if (GOAL) then
        GOAL= .false.
        goto 001
      endif

    enddo
    enddo

    do RN=1, LGTH-1
    do RM=RN, LGTH-1+PROBE

      ATC= PORD(PTH1,RM)
      ATD= PORD(PTH2,RN)
      MAXD=MATDIS(ATC)+MATDIS(ATD)
      MIND=2*LGTH+PROBE-MAXD
      CHK(:)=.false.

      call PAIR_SEARCH (GOAL, ATC, ATD, 1, MAXD, MIND, CPT, VPT, CHK)
      if (GOAL) then
        GOAL= .false.
        goto 001
      endif

    enddo
    enddo

    if ((PROBE.eq.0 .and. 2*LGTH.le.TAILLR) .or. (PROBE.eq.1 .and. 2*LGTH.lt.TAILLR)) then

      TOPRIM(:)=0
      PRIMTO(:)=0
      do IRX=1, LGTH
        TOPRIM(IRX)=PORD(PTH1,IRX)
      enddo
      if (PROBE .eq. 1) TOPRIM(LGTH+1)=PORD(PTH2,LGTH)
      IRS=LGTH+PROBE
      do IRX=LGTH-1, 1, -1
        IRS=IRS+1
        TOPRIM(IRS)=PORD(PTH2,IRX)
      enddo
      TOPRIM(IRS+1)=NODE

!      To find atom id in the primitive unit cell
      do IRX=1, 2*LGTH+PROBE
        PRIMTO(IRX)=REAL_ATOM_ID(TOPRIM(IRX),NA)
      enddo
      TOSAVE=.true.
      if (ABAB) then
        if (PROBE .eq. 0) then
          call TESTABAB (TOSAVE,LGTH,PRIMTO)
        else
          TOSAVE=.false.
        endif
      endif
      if (NO_HOMO) then
        call TESTHOMO(TOSAVE,LGTH,PRIMTO)
      endif
      if (TOSAVE .and. LGTH*2+PROBE.le.TAILLR) then

        if (CALC_STRINGS) then
           call STRONG_RINGS (FNDTAB, LGTH, PROBE, TOPRIM, PRIMTO, RSAVED, OSAVED, TRIN, INDP, RESLP, CPT, VPT)
        else
          call SAVE_DIJKSTRA_RING (PRIMTO, LGTH*2+PROBE, RSAVED, OSAVED, TRIN, INDP, RESLP)
          FNDTAB(1+PROBE)=.true.
        endif
        if (TBR .or. ALC) goto 002

      endif

    endif

  endif

001 continue

enddo

002 continue

END

RECURSIVE SUBROUTINE PAIR_SEARCH (GOAL, AT1, AT2, LG, MAXM, MINM, CPT, VPT, CHK)

USE PARAMETERS

IMPLICIT NONE

LOGICAL, INTENT(INOUT) :: GOAL
INTEGER, INTENT(IN) :: AT1, AT2, LG, MAXM, MINM
INTEGER, DIMENSION(NNA), INTENT(IN) :: CPT
INTEGER, DIMENSION(NNA,MAXN), INTENT(IN) :: VPT
LOGICAL, DIMENSION(NNA), INTENT(INOUT) :: CHK
INTEGER :: PSC, AT3

CHK(AT1)=.true.

if (AT1 .eq. AT2) then

  GOAL=.true.

else

  do PSC=1, CPT(AT1)

    AT3=VPT(AT1,PSC)

    if (.not.CHK(AT3)) then

      if (AT3.eq.AT2) then

        GOAL=.true.
        goto 001

      elseif (LG.lt.MAXM-1 .and. LG.lt.MINM-1) then

        call PAIR_SEARCH (GOAL, AT3, AT2, LG+1, MAXM, MINM, CPT, VPT, CHK)

        if (GOAL) then
           goto 001
        endif

      endif

    endif

  enddo

endif

001 continue

CHK(AT1)=.false.

END SUBROUTINE

SUBROUTINE STRONG_RINGS (FNDTAB, RLGTH, RPROBE, TOPRIM, PRIMTO, ASRING, OSRING, TRNG, INDT, RESL, CPT, VPT)

USE PARAMETERS

IMPLICIT NONE

LOGICAL, DIMENSION(2), INTENT(INOUT) :: FNDTAB
INTEGER, INTENT(IN) :: RLGTH, RPROBE
INTEGER, DIMENSION(TAILLR), INTENT(IN) :: TOPRIM, PRIMTO
INTEGER, DIMENSION(TAILLR,NUMA,TAILLR), INTENT(INOUT) :: ASRING, OSRING
INTEGER, DIMENSION(NUMA), INTENT(INOUT) :: INDT
INTEGER, DIMENSION(TAILLR), INTENT(INOUT) :: TRNG, RESL
INTEGER, DIMENSION(NNA), INTENT(IN) :: CPT
INTEGER, DIMENSION(NNA,MAXN), INTENT(IN) :: VPT
INTEGER :: RM, RN, STLGT
LOGICAL, DIMENSION(NNA) :: CHKS

LOGICAL :: DVSTR
LOGICAL :: SGOAL=.false.
LOGICAL :: FGOAL=.false.

STLGT=2*RLGTH+RPROBE

CHKS(:)=.false.

do RM=1, STLGT-1

  do RN=RM+1, STLGT

    DVSTR=.false.
    call SEARCH_STRONG_RINGS (DVSTR, RM, RN, STLGT, STLGT, SGOAL, FGOAL, TOPRIM, CPT, VPT, CHKS)
    if (TBR .or. ALC) goto 002
    if (FGOAL) goto 001
    if (SGOAL) goto 002

  enddo

enddo

001 continue

call SAVE_DIJKSTRA_RING (PRIMTO, RLGTH*2+RPROBE, ASRING, OSRING, TRNG, INDT, RESL)
FNDTAB(1+RPROBE)=.true.

if (TBR .or. ALC) goto 002

002 continue

END SUBROUTINE

RECURSIVE SUBROUTINE SEARCH_STRONG_RINGS (DLOW, IDX, IDY, DLGTR, LGTR, SRGOAL, FRGOAL, TOTER, CPT, VPT, CHKS)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: DLGTR, LGTR, IDX, IDY
LOGICAL, INTENT(INOUT) :: DLOW, SRGOAL, FRGOAL
INTEGER, DIMENSION(DLGTR), INTENT(IN) :: TOTER
INTEGER, DIMENSION(NNA), INTENT(IN) :: CPT
INTEGER, DIMENSION(NNA,MAXN), INTENT(IN) :: VPT
LOGICAL, DIMENSION(NNA), INTENT(INOUT) :: CHKS
INTEGER :: DXY, DYX, DTEST, DMIN
INTEGER :: RG, RF, RH, RJ, RL, RO, RP, RQ, RR
INTEGER :: NBPATH,  IDZ
INTEGER, DIMENSION(:), ALLOCATABLE :: STRST, NEWTER
INTEGER, DIMENSION(:,:), ALLOCATABLE :: TOTPATH
LOGICAL, DIMENSION(:), ALLOCATABLE :: CUTRING
LOGICAL, DIMENSION(NNA) :: CHK
TYPE(RING), TARGET :: ST_RING
TYPE(RING), POINTER :: SRING, TMP_RI

INTERFACE
  RECURSIVE SUBROUTINE PATH_SEARCH (AT0,AT1,AT2,IDT1,IDT2,DMAX,DMED,DARING,NPATH,DLPATH, &
                                    STPATH,ACRING,TPATH,CUTPATH,CPT,VPT,CHK,CHKS,SRING)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: AT0, AT1, AT2, IDT1, IDT2, DMAX, DMED, DARING
    INTEGER, INTENT(INOUT) :: NPATH
    LOGICAL, INTENT(INOUT) :: DLPATH
    INTEGER, DIMENSION(DMAX*DMAX), INTENT(INOUT) :: STPATH
    INTEGER, DIMENSION(DMAX*DMAX,DMAX), INTENT(INOUT) :: TPATH
    INTEGER, DIMENSION(DMED), INTENT(IN) :: ACRING
    INTEGER, DIMENSION(NNA), INTENT(IN) :: CPT
    INTEGER, DIMENSION(NNA,MAXN), INTENT(IN) :: VPT
    LOGICAL, DIMENSION(DMAX*DMAX), INTENT(INOUT) :: CUTPATH
    LOGICAL, DIMENSION(NNA), INTENT(INOUT) :: CHK, CHKS
    TYPE (RING), POINTER, INTENT(INOUT) :: SRING
  END
  SUBROUTINE SHORTCUT_RING (IDA, IDB, IDC, DLT, TABAB, SRING)
    USE PARAMETERS
    INTEGER, INTENT(IN) :: IDA, IDB, IDC, DLT
    INTEGER, DIMENSION(DLT), INTENT(IN) :: TABAB
    TYPE(RING), POINTER, INTENT(INOUT) :: SRING
  END
  SUBROUTINE CREAT_RING (RING_INIT, ELEM_CR)
    USE PARAMETERS
    TYPE (RING), INTENT(INOUT) :: RING_INIT
    INTEGER, INTENT(IN) :: ELEM_CR
  END SUBROUTINE
  SUBROUTINE DO_RING (THE_RING, ELEM_DO)
    USE PARAMETERS
    TYPE (RING), POINTER, INTENT(INOUT) :: THE_RING
    INTEGER, INTENT(IN) :: ELEM_DO
  END SUBROUTINE
END INTERFACE

do RG=1, DLGTR
  CHKS(TOTER(RG))=.true.
enddo

DXY=abs(IDY-IDX)
DYX=DLGTR-DXY
DMIN=min(DXY,DYX)
DTEST=LGTR-1-DMIN

!   First we find find a ring starting from an atom i and enclosing
!   all tested atoms, then this ring will be tested and so on
!   if the size of the enclosing ring became smaller than
!   DLGTR then the initial ring is not strong

if (DTEST.ge.2) then

  do RJ=2, DTEST

    if (allocated(TOTPATH)) deallocate(TOTPATH)
    allocate(TOTPATH(RJ*RJ,RJ), STAT=ERR)
    if (ERR .ne. 0) then
      ALC_TAB="TOTPATH"
      ALC=.true.
      goto 001
    endif
    if (allocated(CUTRING)) deallocate(CUTRING)
    allocate(CUTRING(RJ*RJ), STAT=ERR)
    if (ERR .ne. 0) then
      ALC_TAB="CUTRING"
      ALC=.true.
      goto 001
    endif
    if (allocated(STRST)) deallocate(STRST)
    allocate(STRST(RJ*RJ), STAT=ERR)
    if (ERR .ne. 0) then
      ALC_TAB="STRST"
      ALC=.true.
      goto 001
    endif

    NBPATH=0
    STRST(:)=0
    CUTRING(:)=.false.
    TOTPATH(:,:)=0
    CHK(:)=.false.

    RR=1
    call CREAT_RING (ST_RING, TOTER(IDX))
    SRING => ST_RING
! On recherche tous les chemins de taille RJ entre les atomes TOTER(IDX) et TOTER(IDY)
! On stock ces NBPATH chemins dans le tableau TOTPATH

! We are looking for all path of size RJ between atoms TOTER(IDX) and TOTER(IDY)
! Then these NBPATH are saved in the tab TOTPATH

    call PATH_SEARCH (TOTER(IDX),TOTER(IDX),TOTER(IDY),IDX,IDY,RJ,DLGTR,LGTR,NBPATH,DLOW, &
                      STRST,TOTER,TOTPATH,CUTRING,CPT,VPT,CHK,CHKS,SRING)
    if (ALC) goto 001
    if (NBPATH .ne. 0) then

      do RH=1, NBPATH

        IDZ=0
        if (CUTRING(RH)) then
          do RG=1, DLGTR
            if (TOTER(RG) .eq. STRST(RH)) then
              IDZ=RG
              exit
            endif
          enddo
        endif

        if(IDZ.eq.0)then

          if (.not.CUTRING(RH)) then
            call CREAT_RING (ST_RING, TOTER(IDX))
            SRING => ST_RING
            do RG=1, RJ
              RR=RR+1
              call DO_RING (SRING, TOTPATH(RH,RG))
              if (ALC) goto 001
            enddo
            if (DXY .le. DYX) then
              do RG=IDY+1, DLGTR
                call DO_RING (SRING, TOTER(RG))
                if (ALC) goto 001
              enddo
              do RG=1, IDX
                call DO_RING (SRING, TOTER(RG))
                if (ALC) goto 001
              enddo
            else
              do RG=IDY-1, IDX, -1
                call DO_RING (SRING, TOTER(RG))
                if (ALC) goto 001
              enddo
            endif
          else
            call CREAT_RING (ST_RING, TOTER(1))
            SRING => ST_RING
            do RG=2, DLGTR
              call DO_RING (SRING, TOTER(RG))
              if (ALC) goto 001
            enddo
            CHKS(TOTPATH(RH,1))=.true.
          endif

        else

          call CREAT_RING (ST_RING, TOTPATH(RH,1))
          SRING => ST_RING
          call SHORTCUT_RING (IDX, IDY, IDZ, DLGTR, TOTER, SRING)
          if (ALC) goto 001

        endif

!    A new path as been found, so a new rings as been built according
!    to the new list of atoms.
!    The size of this new ring has to be tested

        if (SRING%SPEC+1 .lt. LGTR) then

          TMP_RI => SRING
          do RF=1, SRING%SPEC
            TMP_RI => TMP_RI%PAST
            deallocate (TMP_RI%NEXT)
          enddo
          SRGOAL=.true.
          goto 001

        else

! If all the atoms are CHK and ring is bigger than the initial ring
! the initial ring is strong .. we have to check this
          RF=0
          do RG=1, NNA
            if (CHKS(RG)) RF=RF+1
          enddo
          if (RF .eq. NNA) then

            TMP_RI => SRING
            do RG=1, SRING%SPEC
              TMP_RI => TMP_RI%PAST
              deallocate (TMP_RI%NEXT)
            enddo
            FRGOAL=.true.
            goto 001

          else

!       Else the new ring has to be tested
            RR = SRING%SPEC+1
            if (allocated(NEWTER)) deallocate(NEWTER)
            allocate(NEWTER(RR), STAT=ERR)
            if (ERR .ne. 0) then
              ALC_TAB="NEWTER"
              ALC=.true.
              goto 001
            endif
            SRGOAL=.false.
            TMP_RI => SRING
            do RG=1, RR
              NEWTER(RR-RG+1)=TMP_RI%ATOM
              if (RG .lt. RR) then
                TMP_RI => TMP_RI%PAST
                deallocate (TMP_RI%NEXT)
              endif
            enddo
            DLOW=.false.
            SRING => ST_RING
            if (IDZ.eq.0) then
              if (DMIN .eq. 1) then
                RO=2
                RP=RJ
                DLOW=.true.
              else
                RO=1
                RP=RR-1
              endif
            else
              RO=1
              RP=RR-1
            endif

            do RG=RO, RP

              if (IDZ.eq.0) then
                if (DMIN .eq. 1) then
                  DLOW=.true.
                  RQ=RJ+2
                else
                  RQ=RG+1
                endif
              else
                 RQ=RG+1
              endif

              do RF=RQ, RR
                call SEARCH_STRONG_RINGS (DLOW, RG, RF, RR, LGTR, SRGOAL, FRGOAL, NEWTER, CPT, VPT, CHKS)
                if (TBR .or. ALC) goto 001
                if (SRGOAL .or. FRGOAL) goto 001
                do RL=1, DLGTR
                  CHKS(TOTER(RL))=.true.
                enddo
                if (IDZ.eq.0 .and. CUTRING(RH)) CHKS(TOTPATH(RH,1))=.true.
              enddo

            enddo

            if (allocated(NEWTER)) deallocate(NEWTER)

          endif

        endif

        if (IDZ.eq.0 .and. CUTRING(RH)) CHKS(TOTPATH(RH,1))=.false.

      enddo

    else

      SRGOAL=.false.

    endif

    if (allocated(TOTPATH)) deallocate(TOTPATH)
    if (allocated(CUTRING)) deallocate(CUTRING)
    if (allocated(STRST)) deallocate(STRST)

  enddo

endif

001 continue

if (allocated(TOTPATH)) deallocate(TOTPATH)
if (allocated(CUTRING)) deallocate(CUTRING)
if (allocated(STRST)) deallocate(STRST)
if (allocated(NEWTER)) deallocate(NEWTER)

do RG=1, DLGTR
  CHKS(TOTER(RG))=.false.
enddo

END SUBROUTINE

RECURSIVE SUBROUTINE PATH_SEARCH (AT0,AT1,AT2,IDT1,IDT2,DMAX,DMED,DARING,NPATH,DLPATH, &
                                  STPATH,ACRING,TPATH,CUTPATH,CPT,VPT,CHK,CHKS,SRING)
USE PARAMETERS
IMPLICIT NONE

INTEGER, INTENT(IN) :: AT0, AT1, AT2, IDT1, IDT2, DMAX, DMED, DARING
INTEGER, INTENT(INOUT) :: NPATH
LOGICAL, INTENT(INOUT) :: DLPATH
INTEGER, DIMENSION(DMAX*DMAX), INTENT(INOUT) :: STPATH
INTEGER, DIMENSION(DMAX*DMAX,DMAX), INTENT(INOUT) :: TPATH
INTEGER, DIMENSION(DMED), INTENT(IN) :: ACRING
INTEGER, DIMENSION(NNA), INTENT(IN) :: CPT
INTEGER, DIMENSION(NNA,MAXN), INTENT(IN) :: VPT
LOGICAL, DIMENSION(DMAX*DMAX), INTENT(INOUT) :: CUTPATH
LOGICAL, DIMENSION(NNA), INTENT(INOUT) :: CHK, CHKS
TYPE (RING), POINTER, INTENT(INOUT) :: SRING

INTEGER :: AT3, AT4, AT5, AT6, AT7
INTEGER :: DUV, DUW, DVW
LOGICAL :: VAL1, VAL2
LOGICAL :: TOUCH
TYPE (RING), POINTER :: TMPR

INTERFACE
  SUBROUTINE DO_RING (THE_RING, ELEM_DO)
    USE PARAMETERS
    TYPE (RING), POINTER, INTENT(INOUT) :: THE_RING
    INTEGER, INTENT(IN) :: ELEM_DO
  END SUBROUTINE
END INTERFACE

CHK(AT1)=.true.

if (AT1.eq.AT2 .and. SRING%SPEC.eq.DMAX) then


  NPATH=NPATH+1
  TMPR => SRING
  do AT3=1, DMAX
    TPATH(NPATH,DMAX-AT3+1)=TMPR%ATOM
    if (AT3 .lt. DMAX) TMPR => TMPR%PAST
  enddo

else

  do AT4=1, CPT(AT1)

    AT3=VPT(AT1,AT4)
    if (.not.CHK(AT3)) then

      call DO_RING (SRING, AT3)
      if (ALC) goto 001
      if (AT3 .eq. AT2 .and. SRING%SPEC.eq.DMAX) then

        NPATH=NPATH+1
        TMPR => SRING
        do AT5=1, DMAX
          TPATH(NPATH,DMAX-AT5+1)=TMPR%ATOM
          if (AT5 .lt. DMAX) TMPR => TMPR%PAST
        enddo

      elseif (SRING%SPEC.lt.DMAX .and. .not.CHKS(AT3)) then

        TOUCH=.false.
        AT7=0
        VAL1=.false.
        VAL2=.false.
        do AT5=1, CPT(AT3)
          AT6=VPT(AT3,AT5)
          if (AT6 .eq. AT0) VAL1=.true.
          if (AT6 .eq. AT2) VAL2=.true.
          if (CHKS(AT6)) then
            AT7=AT7+1
            if (AT6.ne.AT0 .and. AT6.ne.AT2) STPATH(NPATH+1)=AT6
          endif
        enddo
        AT6=0
        do AT5=1, DMED
          if (ACRING(AT5) .eq. STPATH(NPATH+1)) then
            AT6=AT5
            exit
          endif
        enddo
        if (AT7 .eq. 0) then
          TOUCH=.false.
        elseif (AT7.eq.1 .and. VAL1 .and. .not.VAL2) then
          TOUCH=.false.
        elseif (AT7.eq.1 .and. VAL2 .and. .not.VAL1) then
          TOUCH=.false.
        elseif (AT7.eq.1 .and. .not.VAL1 .and. .not.VAL2) then
          TOUCH=.true.
        elseif (AT7.eq.2 .and. VAL1 .and. VAL2 .and. DMAX.eq.2) then
          TOUCH=.false.
        elseif (AT7.gt.2 .and. VAL1 .and. VAL2 .and. DMAX.eq.2) then
          if (DLPATH) then
            TOUCH=.true.
          else
            if (AT6 .ne. 0) then
              if (AT6 .lt. IDT1) then
                DUV=IDT1-AT6
                DVW=IDT2-IDT1
                DUW=DMED-(IDT2-AT6)
              elseif(AT6 .lt. IDT2) then
                DUV=AT6-IDT1
                DVW=DMED-(AT2-AT1)
                DUW=IDT2-AT6
              elseif(AT6 .gt. IDT2) then
                DUV=DMED-(AT6-IDT1)
                DVW=IDT2-IDT1
                DUW=AT6-IDT2
              endif
              if (DUV.lt.DARING-1 .and. DUW.lt.DARING-1) then
                CUTPATH(NPATH+1)=.true.
                TOUCH=.false.
              elseif (DVW.lt.DARING-1 .and. DUW.lt.DARING-1) then
                CUTPATH(NPATH+1)=.true.
                TOUCH=.false.
              elseif (DUV.lt.DARING-1 .and. DVW.lt.DARING-1) then
                CUTPATH(NPATH+1)=.true.
                TOUCH=.false.
              else
                TOUCH=.true.
              endif
            else
               CUTPATH(NPATH+1)=.true.
               TOUCH=.false.
            endif
          endif
        else
          TOUCH=.true.
        endif
        if (.not.TOUCH) then
          call PATH_SEARCH (AT0,AT3,AT2,IDT1,IDT2,DMAX,DMED,DARING,NPATH,DLPATH, &
                            STPATH,ACRING,TPATH,CUTPATH,CPT,VPT,CHK,CHKS,SRING)
          if (ALC) goto 001
        endif
      endif
      SRING => SRING%PAST
      deallocate (SRING%NEXT)

    endif

  enddo

endif

CHK(AT1)=.false.
001 continue

END SUBROUTINE

SUBROUTINE SHORTCUT_RING (IDA, IDB, IDC, DLT, TABAB, SRING)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: IDA, IDB, IDC, DLT
INTEGER, DIMENSION(DLT), INTENT(IN) :: TABAB
TYPE(RING), POINTER, INTENT(INOUT) :: SRING
INTEGER :: IDD
INTEGER :: LXY, LXZ, LYZ

INTERFACE
  SUBROUTINE DO_RING (THE_RING, ELEM_DO)
    USE PARAMETERS
    TYPE (RING), POINTER, INTENT(INOUT) :: THE_RING
    INTEGER, INTENT(IN) :: ELEM_DO
  END SUBROUTINE
END INTERFACE

if (IDA .gt. IDC) then
! IDB > IDA > IDC
  LXZ=IDA-IDC
  LXY=IDB-IDA
  LYZ=DLT-(IDB-IDC)
  if (LXY .gt. LYZ) then
    if (LXY .gt. LXZ) then
      do IDD=IDA, IDB
        call DO_RING (SRING, TABAB(IDD))
        if (ALC) goto 001
      enddo
    else
      do IDD=IDC, IDA
        call DO_RING (SRING, TABAB(IDD))
        if (ALC) goto 001
      enddo
    endif
  else
    if (LXZ .gt. LYZ) then
      do IDD=IDC, IDA
        call DO_RING (SRING, TABAB(IDD))
        if (ALC) goto 001
      enddo
    else
      do IDD=IDB, DLT
        call DO_RING (SRING, TABAB(IDD))
        if (ALC) goto 001
      enddo
      do IDD=1, IDC
        call DO_RING (SRING, TABAB(IDD))
        if (ALC) goto 001
      enddo
    endif
  endif
else
! IDB > IDA .and. IDC .gt. IDA
  if (IDC .gt. IDB) then
! IDB > IDA .and. IDC .gt. IDA .and. IDC .gt. IDB
! =>  IDC > IDB > IDA
    LXY=IDB-IDA
    LYZ=IDC-IDB
    LXZ=DLT-(IDC-IDA)
    if (LXY .gt. LYZ) then
      if (LXY .gt. LXZ) then
        do IDD=IDA, IDB
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
      else
        do IDD=IDC, DLT
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
        do IDD=1, IDA
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
      endif
    else
      if (LYZ .ge. LXZ) then
        do IDD=IDB, IDC
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
      else
        do IDD=IDA, IDB
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
      endif
    endif
  else
! IDB > IDA .and. IDC .gt. IDA .and. IDB .gt. IDC
! =>  IDB > IDC > IDA
    LXY=DLT-(IDB-IDA)
    LYZ=IDB-IDC
    LXZ=IDC-IDA
    if (LXY .gt. LYZ) then
      if (LXY .gt. LXZ) then
        do IDD=IDB, DLT
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
        do IDD=1, IDA
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
      else
        do IDD=IDA, IDC
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
      endif
    else
      if (LYZ .gt. LXZ) then
        do IDD=IDC, IDB
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
      else
        do IDD=IDA, IDC
          call DO_RING (SRING, TABAB(IDD))
          if (ALC) goto 001
        enddo
      endif
    endif
  endif
endif

001 continue

END SUBROUTINE

SUBROUTINE TESTABAB(VALTEST, LGTEST, PRIMT)

USE PARAMETERS

IMPLICIT NONE

LOGICAL, INTENT(INOUT) :: VALTEST
INTEGER, INTENT(IN) :: LGTEST
INTEGER, DIMENSION(TAILLR), INTENT(IN) :: PRIMT
INTEGER :: LTA, LTB, LTC, LTD

LTA=LOT(PRIMT(1))
LTB=LOT(PRIMT(2))

VALTEST=.true.
if (LTA .ne. LTB) then
  do LTD=3, 2*LGTEST
    LTC=LOT(PRIMT(LTD))
    if (mod(LTD,2).eq.0 .and. LTC.ne.LTB) then
      VALTEST=.false.
      exit
    elseif (mod(LTD,2).ne.0 .and. LTC.ne.LTA) then
      VALTEST=.false.
      exit
    endif
  enddo
else
  VALTEST=.false.
endif

END SUBROUTINE

SUBROUTINE TESTHOMO(VALTEST, LGTEST, PRIMT)

USE PARAMETERS

IMPLICIT NONE

LOGICAL, INTENT(INOUT) :: VALTEST
INTEGER, INTENT(IN) :: LGTEST
INTEGER, DIMENSION(TAILLR), INTENT(IN) :: PRIMT
INTEGER :: LTA, LTB

VALTEST=.true.
do LTA=1, 2*LGTEST
  LTB=LTA+1
  if (LTA .eq. 2*LGTEST) LTB=1
  if (LOT(PRIMT(LTA)) .eq. LOT(PRIMT(LTB))) then
    VALTEST=.false.
    exit
  endif
enddo

END SUBROUTINE

SUBROUTINE SAVE_DIJKSTRA_RING (TAB, TLES, RSAVED, OSAVED, TRING, INDT, RESL)

USE PARAMETERS

IMPLICIT NONE

INTEGER, DIMENSION(TAILLR), INTENT(IN) :: TAB
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
  TOTRI(idx)= TAB(idx)
  TOSAV(idx)= TAB(idx)

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
