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
!! @file parameters.F90
!! @short Global variable declarations
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

!################################################################
!   This is the atomes code main include file which contains:
!           -  Variables
!           -  Structures
!       used all along the code and classified as follow:
!
!           - LOGICAL type variables
!           - LOGICAL (:) type variables
!           - LOGICAL (:,:) type variables
!
!           - INTEGER type variables
!           - INTEGER(:)  type variables
!           - INTEGER(:,:) type variables
!           - INTEGER(:,:,:) type variables
!           - INTEGER(:,:,:,:) type variables
!           - INTEGER(:,:,:,:,:) type variables
!
!           - CHARACTER type variables
!           - CHARACTER(:) type variables
!           - CHARACTER(:,:) type variables
!           - CHARACTER(:,:,:) type variables
!
!           - DOUBLE PRECISION type variables
!
!           - DOUBLE PRECISION type variables
!           - DOUBLE PRECISION(:) type variables
!           - DOUBLE PRECISION(:,:) type variables
!           - DOUBLE PRECISION(:,:,:) type variables
!           - DOUBLE PRECISION(:,:,:,:) type variables
!
!           - STRUCTURES definitions
!
!           - MPI // Only variables
!
!    furthermore inside this first selection variables
!    have been sorted by routines when necessary/possible
!################################################################

MODULE PARAMETERS

USE, INTRINSIC :: iso_c_binding

IMPLICIT NONE

!#################################### LOGICAL VARIABLES ####################################!

LOGICAL :: PBC=.false.           ! 1/0 Enable or disable periodic boundary conditions
LOGICAL :: FRAC=.false.          ! 1/0 Lattice relatives or DOUBLE PRECISIONs positions for atoms
LOGICAL :: NOHP=.false.          ! 1/0 Enable/disable homopolar bonds in connectivty
LOGICAL :: ABAB=.false.          ! 1/0 Enable or disable only ABAB rings if more than 3 chemical species
LOGICAL :: AAAA=.false.          ! 1/0 Enable or disable only AAAA chains
LOGICAL :: ACAC=.false.          ! 1/0 Enable or disable only ABAB chains
LOGICAL :: ISOLATED=.false.      ! 1/0 1-2-2-1 chains or N-2-2-N N=1,3 ... chains
LOGICAL :: TOMO=.false.          ! 1/0 Homopolar or no homopolar bonds in the rings
LOGICAL :: CALC_RINGS=.false.    ! 1/0 Compute ring statistics
LOGICAL :: CALC_R0=.false.       ! 1/0 Find all existing rings in the box
LOGICAL :: CALC_R1=.false.       ! 1/0 Shortest Path analysis
LOGICAL :: CALC_R2=.false.       ! 1/0 King's analysis
LOGICAL :: CALC_R3=.false.       ! 1/0 SP + Homo
LOGICAL :: CALC_R4=.false.       ! 1/0 King's + Homo
LOGICAL :: CALC_PRINGS=.false.   ! 1/0 Compute primitive ring statistics
LOGICAL :: CALC_STRINGS=.false.  ! 1/0 Compute strongs ring statistics
LOGICAL :: RING_P1=.false.       ! 1/0 Compute first part of detailed ring properties
LOGICAL :: RING_P2=.false.       ! 1/0 Compute second part of detailed ring properties
LOGICAL :: RING_P3=.false.       ! 1/0 Compute third part of detailed ring properties
LOGICAL :: RING_P4=.false.       ! 1/0 Compute fourth part of detailed ring properties
LOGICAL :: RING_P5=.false.       ! 1/0 Compute fifth part of detailed ring properties
LOGICAL :: OVERALL_CUBIC=.false. ! 1/0 Cubic a=b=c, 90.0, 90.0, 90.0
#ifdef OPENMP
LOGICAL :: ALL_ATOMS=.false.     ! 1/0 Force OpenMP on ATOMS
#endif

! *rings*.f90 !

LOGICAL :: FACTATRING=.false.    ! 1/0 Calculate atomic ring factor using the RN variable (thesis S. LE ROUX)
LOGICAL :: FACTATPNA=.false.     ! 1/0 Calculate atomic ring factor using the PN  variable (thesis S. LE ROUX)
LOGICAL :: FACTATRMAX=.false.    ! 1/0 Calculate atomic ring factor using the Pmax var (thesis S. LE ROUX)
LOGICAL :: FACTATRMIN=.false.    ! 1/0 Calculate atomic ring factor using the Pmin var (thesis S. LE ROUX)
LOGICAL :: RUNSEARCH             ! 1/0 Run ring statistics in the case of 3 or more species
LOGICAL :: ADDSPEC               ! 1/0 Add to chain in the ring case of 3 or more species
LOGICAL :: DOAMPAT=.false.       ! 1/0 if CALC_R1,R2,R3 or R4 to known if there are paths > TAILLD
LOGICAL :: SAUT, SSAUT
LOGICAL :: FOUND
LOGICAL :: PATHOUT
LOGICAL :: FIRR, PIRR
LOGICAL :: HOMO
LOGICAL :: DOSEARCH
LOGICAL :: TBR=.false.
LOGICAL :: ALC=.false.
LOGICAL :: NO_HOMO=.false.
LOGICAL :: ALLRINGS=.false.

!##########################################################################################!


!###################################### INTEGER VARIABLES #####################################!

!***************************************** Misc *******************************************!

INTEGER :: h=0, i=0, j=0, k=0, l=0, m=0, n=0, o=0, p=0, r=0, t=0, u=0, v=0
INTEGER :: NOA, NOB, NOC, NOD, NOE
INTEGER :: COUNTER, RATE_COUNT, MAX_COUNT
INTEGER :: NSUB, NTSUB, TSUB, ERR
INTEGER :: NL, NC, NSI, NSJ
INTEGER :: NUMBER_OF_I
INTEGER :: DTV
INTEGER :: NBONDS, ANBONDS
INTEGER :: NDX, NBX, IDR
INTEGER :: GR_INDEX
INTEGER :: SUM_INDEX
INTEGER :: ANG_I
INTEGER :: FINISH, INDEXMSF
INTEGER :: Id1, Id2, Id, GtBsize
INTEGER :: SPIRR
INTEGER :: L_TOT, LA_TOT
INTEGER :: NUMA, SC
INTEGER :: PATH, NNP, NNA
INTEGER :: LOA, LOB, LOC
INTEGER :: MAXAT, MINAT
INTEGER :: MAXST, MINST
INTEGER :: GRNUM, SQNUM, SKNUM, GQNUM, BDNUM, ANNUM, RINUM, CHNUM, SHNUM, MSNUM
INTEGER :: ab, abc

!*************************************** Defined ******************************************!

INTEGER :: NA, NUMBER_OF_A              ! Number of atom in the box
INTEGER :: NSP, NSP_BY_STEP             ! Number of species
INTEGER :: NS, NOS, NSTP                ! Number of DM steps = Number of configurations
INTEGER :: NSBG                         ! Number of step between each geomerty
INTEGER :: TAILLR, TAILLD           ! Depth for rings hunt
INTEGER :: TAILLE, TAILLH, TAILLT       ! Depth for rings hunt
INTEGER :: TAILLC                       ! Depth for chains hunt
INTEGER :: TLT, NTLT
INTEGER :: NUMBER_OF_QMOD               ! Number of Qvect modulus
INTEGER :: NUMBER_OF_QVECT              ! Number of Qvectors
INTEGER :: LTLT                         ! Ring's hunt species
INTEGER :: NCELLS                       ! Number of lattice 1 or MD steps if NPT calculation

INTEGER :: IDGR=0
INTEGER :: IDSQ=1
INTEGER :: IDSK=2
INTEGER :: IDGRFFT=3
INTEGER :: IDBD=4
INTEGER :: IDAN=5
INTEGER :: IDRI=6
INTEGER :: IDCH=7
INTEGER :: IDSP=8
INTEGER :: IDMSD=9
INTEGER :: IDSKT=10

INTEGER :: MAXN=20                      ! The maximun number of neighbors an atom can have

! *rings*.f90 !

INTEGER :: MOLATS
INTEGER :: MOLSTEP
INTEGER :: MOLCOUNTER
INTEGER :: TMBS

!##########################################################################################!


!#################################### INTEGER(:) VARIABLES #####################################!

!************************************* defined ********************************************!

INTEGER, DIMENSION(:), ALLOCATABLE :: LOT
INTEGER, DIMENSION(:), ALLOCATABLE :: NBSPBS               ! Number of atom of species=index_1

! prepdata.F90 !

INTEGER, DIMENSION(:), ALLOCATABLE :: ATOMID

! dmtx.f90 !

INTEGER, DIMENSION(3) :: isize

! sk.f90 !

INTEGER, DIMENSION(:), ALLOCATABLE :: degeneracy

! molecuels.f90 !

INTEGER, DIMENSION(:), ALLOCATABLE :: MIBS

! bonds.F90 !

INTEGER, DIMENSION(:), ALLOCATABLE :: TOT_GSA
INTEGER, DIMENSION(:), ALLOCATABLE :: NTETRA
INTEGER, DIMENSION(:), ALLOCATABLE :: LP_GEOM
INTEGER, DIMENSION(:), ALLOCATABLE :: TOGL, TIGL
INTEGER, DIMENSION(:), ALLOCATABLE :: LGSA, NGSA

! *rings*.f90 !

INTEGER, DIMENSION(:), ALLOCATABLE :: RPAT, CPAT
INTEGER, DIMENSION(:), ALLOCATABLE :: MOL_ATOMS

! alloc*rings.F90 - rings.F90 !

INTEGER, DIMENSION(:), ALLOCATABLE :: RES_LIST
INTEGER, DIMENSION(:), ALLOCATABLE :: APNA, SPNA
INTEGER, DIMENSION(:), ALLOCATABLE :: VALID

! primrings.F90 !

INTEGER, DIMENSION(:), ALLOCATABLE :: QUEUE, RINGSTAT
INTEGER, DIMENSION(:), ALLOCATABLE :: NPRING, MATDIST

!#################################### INTEGER (:,:) VARIABLES ###################################!

! bonds.F90 !

INTEGER, DIMENSION(:,:), ALLOCATABLE :: LT_GEOM
INTEGER, DIMENSION(:,:), ALLOCATABLE :: GEOM_LA
INTEGER, DIMENSION(:,:), ALLOCATABLE :: TOT_GEOMSA
INTEGER, DIMENSION(:,:), ALLOCATABLE :: TETRA

! molecules.f90 !

INTEGER, DIMENSION(:,:), ALLOCATABLE :: TIIDA
INTEGER, DIMENSION(:,:), ALLOCATABLE :: TISP

! dmtx.f90 !

INTEGER, DIMENSION(:,:), ALLOCATABLE :: CONTJ

! escs.F90 !

INTEGER, DIMENSION(:,:), ALLOCATABLE :: CORT
INTEGER, DIMENSION(:,:), ALLOCATABLE :: EDGET
INTEGER, DIMENSION(:,:), ALLOCATABLE :: DEFT
INTEGER, DIMENSION(:,:), ALLOCATABLE :: CORTA
INTEGER, DIMENSION(:,:), ALLOCATABLE :: EDGETA
INTEGER, DIMENSION(:,:), ALLOCATABLE :: DEFTA
INTEGER, DIMENSION(:,:), ALLOCATABLE :: TDA

! alloc*rings.F90 - rings.F90 !

INTEGER, DIMENSION(:,:), ALLOCATABLE, TARGET :: INDR
INTEGER, DIMENSION(:,:), ALLOCATABLE, TARGET :: NRING
INTEGER, DIMENSION(:,:), ALLOCATABLE :: VPAT
INTEGER, DIMENSION(:,:), ALLOCATABLE :: INDRAT, NIRRAT, NIRR
INTEGER, DIMENSION(:,:), ALLOCATABLE :: ATRMSX, ATRMSN
INTEGER, DIMENSION(:,:), ALLOCATABLE :: AMPAT, ATRMAX, ATRMIN
INTEGER, DIMENSION(:,:), ALLOCATABLE :: MAXPNA, MINPNA

! primrings.F90 !

INTEGER, DIMENSION(:,:), ALLOCATABLE :: PRING
INTEGER, DIMENSION(:,:), ALLOCATABLE :: QUERNG, VREF
INTEGER, DIMENSION(:,:), ALLOCATABLE :: PRINGORD

!##########################################################################################!


!#################################### INTEGER VARIABLES #################################!

! bonds.F90 !

INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: LA_COUNT

! escs.F90 !

INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: CORNER
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: EDGE
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: DEF
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: CORNERA
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: EDGEA
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: DEFA
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: TDSA

! dmtx.f90 !

INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: VOISJ

! bonds.f90 !

INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: STATBD

! allocrings.F90 - rings.F90 !

INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: INDRING, INDSRING, LIRR
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: ATRING, ATPNA, MATPNA
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: STPNA
INTEGER, DIMENSION(:,:,:), ALLOCATABLE :: PNA, SNA
INTEGER, DIMENSION(:,:,:), ALLOCATABLE, TARGET :: RINGSAVED

! primrings.F90 !

INTEGER, DIMENSION(:,:,:), ALLOCATABLE, TARGET :: SRINGSAVED

!##########################################################################################!


!################################## INTEGER(:,:,:,:) VARIABLES #################################!

! angles.f90 !

! allocrings.F90 - rings.F90 !

INTEGER, DIMENSION(:,:,:,:), ALLOCATABLE, TARGET :: RINGORD

! allocprims.f90 - primrings.F90 !

INTEGER, DIMENSION(:,:,:,:), ALLOCATABLE, TARGET :: SRINGORD

!##########################################################################################!


!###################################### CHARACTER VARIABLES ####################################!

! Misc !

CHARACTER (LEN=15) :: sdate, stime, szone                ! Starting date and time
CHARACTER (LEN=15) :: edate, etime, ezone                ! Ending date and time
CHARACTER (LEN=9) :: PROGNAME="ATOMES"

! misc !

CHARACTER (LEN=2) :: CHOOSE
CHARACTER (LEN=2) :: A1, A2
CHARACTER (LEN=2) :: ATOT
CHARACTER (LEN=20) :: NOM_TMP
CHARACTER (LEN=15) :: NTMP, NTMP2, NTMP3, Nstep, Nomx, Nomy
CHARACTER (LEN=15) :: Nom, Nom1, Nom2, Nom3, Nom4
CHARACTER (LEN=40) :: NomSys, NomS
CHARACTER (LEN=16) :: SPIND
CHARACTER (LEN=20) :: ALC_TAB

!##########################################################################################!


!##################################### CHARACTER (:) VARIABLES ###################################!

! Misc !

CHARACTER (LEN=2), DIMENSION(:), ALLOCATABLE :: TL, FTAB          ! Label of species=index_1
CHARACTER (LEN=2), DIMENSION(:), ALLOCATABLE :: TAB_OF_TYPE       ! List of atom by name(species)=indx_1, by DM step=indx_2
CHARACTER (LEN=2), DIMENSION(:), ALLOCATABLE :: LABEL             ! Label of species=index in input

! prepdata.F90 !

CHARACTER (LEN=15), DIMENSION(:), ALLOCATABLE :: ELEMID


!##########################################################################################!

!#################################### CHARACTER (:,:) VARIABLES ##################################!

CHARACTER (LEN=12), DIMENSION(:,:), ALLOCATABLE :: LISTE_GEOMS

!##########################################################################################!

!################################### CHARACTER (:,:,:) VARIABLES #################################!

! CHARACTER (LEN=12), DIMENSION(:,:,:), ALLOCATABLE :: LGEO
CHARACTER (LEN=12), DIMENSION(:,:,:), ALLOCATABLE :: LGEO

!##########################################################################################!



!###################################### DOUBLE PRECISION VARIABLES ####################################!

DOUBLE PRECISION :: LTEMP

!##########################################################################################!



!###################################### DOUBLE PRECISION VARIABLES ##################################!

! Misc !

DOUBLE PRECISION :: w=0.0d0, x=0.0d0, y=0.0d0, z=0.0d0
DOUBLE PRECISION, PARAMETER :: PI=acos(-1.0)
DOUBLE PRECISION, PARAMETER :: AVOGADRO=6.02214179d0
DOUBLE PRECISION, PARAMETER :: ANGTOBOHR=0.52917721
DOUBLE PRECISION :: p1, p2, p3
DOUBLE PRECISION :: TIME_TOT, tstat
DOUBLE PRECISION :: MBOX
DOUBLE PRECISION :: BOXS2
DOUBLE PRECISION :: BASE
DOUBLE PRECISION :: TETA
DOUBLE PRECISION :: DC
DOUBLE PRECISION :: DELTA_ANG
DOUBLE PRECISION :: MEANVOL
DOUBLE PRECISION :: TOTAL_DENSITY, REAL_DENSITY
DOUBLE PRECISION :: FREEVOL, ECFREEV
DOUBLE PRECISION :: Dij, Dil, Vij
DOUBLE PRECISION :: Gr_cutoff
DOUBLE PRECISION :: CUTF
!DOUBLE PRECISION :: A2A, B2B, C2C, AAA, BBB, CCC

! sq.f90 !

DOUBLE PRECISION :: Phi, Sinus_phi

! sk.f90 !

DOUBLE PRECISION :: NORM_TOT, DELTA_Q
DOUBLE PRECISION :: Rmax, Sinus_Fact_Rmax, Fact_Rmax
DOUBLE PRECISION :: SIGMA_LISS
DOUBLE PRECISION :: qvmod, qvmax, qvmin

!##########################################################################################!


!#################################### DOUBLE PRECISION (:) VARIABLES ##################################!

! Misc !

DOUBLE PRECISION, DIMENSION(3) :: Rij, Ril, Rim, Dab, VAR                       ! Position vector
DOUBLE PRECISION, DIMENSION(3) :: R2ij, R2Cor, RCm, RCm2
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: S_LENGTH
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: MASS, M_SS
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: RVDW, R_DW
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: Dcte                             ! Diffusion Cte
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: DIST_IJ, DIST_JI
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: CMOY, Xi
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: MTABL

! dmtx.f90 !

DOUBLE PRECISION, DIMENSION(3) :: CUTFV
DOUBLE PRECISION, DIMENSION(3) :: pmin, pmax

! bonds.F90 !

DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: MAC
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: SA_COUNT

! escs.f90 !

DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: EABL, CABL, DABL
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: ETABL, CTABL, DTABL
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: ECTABL, TDTABL

! gr.f90 !

DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: R_POINT
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: GRTAB
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: GrTOT, GgrTOT    ! Total RDF
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: Drn, Trn         ! Total RDF
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: XGrTOT, XGgrTOT  ! Total RDF
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: Drx, Trx         ! Total RDF
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: SHELL_VOL        ! Shell volume as a function of d=(index-0.5)DELTA

! sq.f90 !

DOUBLE PRECISION, DIMENSION(2) :: Gd, Gn, Go, C_Box
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: NSCATTL          ! Neutron Scattering Length of species=index
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: XSCATTL          ! X-ray Scattering Length of species=index
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: S, XS            ! Total structure factor \\ Go=f(G(r))
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: Gr_T
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: Q_POINT, K_POINT

! sk.f90 - utils.f90 !

DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: FNBSPBS
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: modq
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: qvectx, qvecty, qvectz
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: cij, sik

! grfft.f90 !

DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: GFFT, DFFT, R_PFFT, TDFFT

! resrings.f90 !

DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: MOYPUR, MOYRED
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: MOYPAT, MOYRAT
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: ECTYPE, ECTYP, ECTYPAT, ECTAT
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: IRRED, RED
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: REDAT, IRRAT
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: TOTPSTEP
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: RNAMAX, RNAMIN
DOUBLE PRECISION, DIMENSION(:), ALLOCATABLE :: EVMAX, EVMIN

!##########################################################################################!


!################################## DOUBLE PRECISION (:,:) VARIABLES ##################################!

! Misc !

DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: EPNA
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: POA, POB                      ! Atm=indx_1, coord=indx_2(1=x, 2=y, 3=z)

! bonds.F90 !

DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: MA_COUNT
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: NUM_GSA
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: Gr_CUT, Gr_TMP

! fzbt.f90 !

DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: BTij                         ! Bathia-Thornton Partial g(r)/s(q)

! escs.F90 !

DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: ETYPE
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: CTYPE
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: DETYPE
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: ETYPEA
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: CTYPEA
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: DETYPEA
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: ETDA

! sk.f90 !

DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: qvect

! msd.f90 !

DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: DRIFT, COR
DOUBLE PRECISION, DIMENSION(:,:), ALLOCATABLE :: D2i, D2iNAC

!##########################################################################################!


!################################# DOUBLE PRECISION (:,:,:) VARIABLES #################################!

! Misc !

DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: FULLPOS
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: FULLVEL
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: NFULLPOS, NFPOS
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: ECART_TYPE

! gr.f90 !

DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: Gr_ij        ! Average of the RDF on all the MD run
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: Ggr_ij       ! Average of the RDF on all the MD run
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: Dn_ij        ! Average of the Dn on all the MD run

! grfft.f90 !

DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: GQBT

! sq.f90 !

DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: Sij, Spij            ! Partial structure factor
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: Ss_ij                ! Partial structure factor

! fzbt.f90 !

DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: FZSij

! msd.f90 !

DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: D2ij, D2ijNAC, D2dir, D2dirNAC
DOUBLE PRECISION, DIMENSION(:,:,:), ALLOCATABLE :: CORNAC

!##########################################################################################!


!################################ DOUBLE PRECISION (:,:,:,:) VARIABLES ################################!

! gr.F90 !

DOUBLE PRECISION, DIMENSION(:,:,:,:), ALLOCATABLE :: Gij ! RDF distance=index_1, atom_a=index_2, atom_b=index_3, step=index_4
DOUBLE PRECISION, DIMENSION(:,:,:,:), ALLOCATABLE :: Dn  ! Dn distance=index_1, atom_a=index_2, atom_b=index_3, step=index_4

!##########################################################################################!

!################################ STRUCTURES VARIABLES Definition ###############################!

!TYPE ATOM                                                          !
!  INTEGER :: INDICE                                                !
!  INTEGER :: ELEMENT                                               !
!  INTEGER :: NEIGHBOR                                              !   Atom structure definition
!  DOUBLE PRECISION :: X, Y, Z                                      !
!  TYPE (ATOM), POINTER :: PAST                                     !
!  TYPE (ATOM), POINTER :: NEXT                                     !
!END TYPE ATOM                                                      !

!TYPE MOLECULE                                                      !
!  INTEGER :: INDICE                                                !
!  INTEGER :: ATOMS                                                 !
!  TYPE (ATOM), POINTER :: FIRST_ATOM                               !
!  TYPE (ATOM), POINTER :: LAST_ATOM                                !   Molecule structure definition
!  TYPE (MOLECULE), POINTER :: NEXT_MOL                             !
!  TYPE (MOLECULE), POINTER :: PAST_MOL                             !
!END TYPE MOLECULE

!TYPE MODEL                                                         !
!  INTEGER :: ATOMS                                                 !
!  INTEGER :: MOLECULES                                             !   Model structure definition
!  TYPE (MOLECULE), POINTER :: FIRST_MOL                            !
!  TYPE (MOLECULE), POINTER :: LAST_MOL                             !
!END TYPE MODEL
                                                                    !
!TYPE (ATOM), POINTER :: AT                                         !
!TYPE (MOLECULE), POINTER :: MOL                                    !
!TYPE (MODEL), POINTER :: MODL                                      !

TYPE AT
  INTEGER :: IND
  TYPE (AT), POINTER :: NEXT
  TYPE (AT), POINTER :: PREV
END TYPE AT

TYPE MOL
  INTEGER :: MID
  INTEGER :: STEP
  INTEGER :: ATOMES
  INTEGER, DIMENSION(:), ALLOCATABLE :: BSP
  TYPE (AT), POINTER :: FIRST_AT
  TYPE (AT), POINTER :: ATOM
  TYPE (MOL), POINTER :: NEXT
  TYPE (MOL), POINTER :: PREV
END TYPE MOL

TYPE RING                                                          !
  INTEGER :: ATOM                                                  !
  INTEGER :: NEIGHBOR                                              !      Ring structure definition
  INTEGER :: SPEC                                                  !
  TYPE (RING), POINTER :: PAST                                     !   used in the ring search subroutines
  TYPE (RING), POINTER :: NEXT                                     !
END TYPE RING                                                      !

TYPE PIXEL                                                         !
  INTEGER :: NEIGHBOR                                              !   Pixel structure definition
  INTEGER :: ATOMS                                                 !
  INTEGER, DIMENSION(:), ALLOCATABLE :: ATOM_ID                    ! Note: MAXN*10 vs. ALL for Angles
  LOGICAL :: TOCHECK                                               !
  LOGICAL :: CHECKED                                               !
  INTEGER, DIMENSION(27) :: IDNEIGH                                !
END TYPE PIXEL

TYPE (PIXEL), DIMENSION(:), ALLOCATABLE :: THEPIX, TESTPIX

TYPE LATTICE
  LOGICAL :: GLASS=.false.                                         ! 1/0 if the structure is 'cubic like' (90/90/90)
  LOGICAL :: CUBIC=.false.                                         ! 1/0 if the structure is 'cubic' (90/90/90, a=b=c)
  DOUBLE PRECISION :: VOLUME
  DOUBLE PRECISION, DIMENSION(3) :: modv                           ! Lattice parameters
  DOUBLE PRECISION :: minv, maxv                                   ! Min, max a,b, c
  DOUBLE PRECISION, DIMENSION(3) :: modr                           ! Reciprocal lattice parameters
  DOUBLE PRECISION :: minr, maxr                                   ! Min, max, ra, rb, rc
  DOUBLE PRECISION, DIMENSION(3,3) :: lvect                        ! Lattice vectors
  DOUBLE PRECISION, DIMENSION(3,3) :: lrecp                        ! Reciprocal lattice vectors
  DOUBLE PRECISION, DIMENSION(3,3) :: fractocart, carttofrac       ! Conversion matrix
END TYPE LATTICE

TYPE (LATTICE), DIMENSION(:), ALLOCATABLE, TARGET :: THE_BOX
TYPE (LATTICE), POINTER :: NBOX

!##########################################################################################!

!##########################################################################################!

END MODULE PARAMETERS

! ########################################  EOF ###########################################!

