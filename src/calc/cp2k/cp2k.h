/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2026 by CNRS and University of Strasbourg */

/*!
* @file cp2k.h
* @short Variable declarations for the creation of the CP2K input file(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'cp2k.h'
*
* Contains:

  Variable declarations for the creation of the CP2K input file(s)

*  Called by:

  calc/cp2k/cp2k_files.c
  calc/cp2k/cp2k_init.c
  calc/cp2k/cp2k_mol.c
  calc/cp2k/cp2k_print.c
  calc/cpmd/cpmd_init.c
  calc/cpmd/cpmd_nose.c

*/

#ifndef CP2K_H_
#define CP2K_H_

#define CP2RUN 0    // Def: 0 = ENERGY run
#define CP2KTI 1    // Def: 86400 = 24h
#define CP2PLE 2    // Def: 1 = MEDIUM
#define CP2RES 3    // Def: 0 = Not a restart
#define CP2FRE 4    // Def: 0 = Null
#define CP2FBA 5    // Def: 0 = Null
#define CP2FPS 6    // Def: 0 = Null
#define CP2FWV 7    // Def: 0 = Null
#define CP2CHA 8    // Def: 0 = neutral
#define CP2CUT 9    // Def: 300
#define CP2GRI 10   // Def: 4
#define CP2QSM 11   // Def: 0 = GPW
#define CP2SCG 12   // Def: Guess 0 = ATOMS
#define CP2SNN 13   // Def: 50
#define CP2SCN 14   // Def: 0.000001
#define CP2SNO 15   // Def: 20
#define CP2SCO 16   // Def: 0.00001
#define CP2SMI 17   // Def: 2 = DIIS
#define CP2FCT 18   // Def: 0 = BLYP
#define CP2SPI 19   // Def: 0 = spin restricted
#define CP2SPM 20   // Def: 0 spin multiplicity
#define CP2VDW 21   // Def: 0 = non vdw
#define CP2ROK 22   // Def: 0 = non ROKS
#define CP2PFO 23   // Def: 0 = do not print forces
#define CP2PST 24   // Def: 0 = do not print stress tensor
#define CP2PMU 25   // Def: 0 = do not print Mulliken
#define CP2PLO 26   // Def: 0 = do not print Löwdin
#define CP2POR 27   // Def: 0 = do not print orbitals
#define CP2PBC 28   // Def: f(system)
#define CP2LAT 29   // 0 = a,b,c,alpha,beta,gamma  :: 1 = ax,ay,az,bx,by,bz,cx,cy,cz
#define CP2SYM 30   // Def: f(system)
#define CP2ENS 31   // Def: 7 = NVT ensemble
#define CP2NST 32   // Def: 10000 max step md
#define CP2DLT 33   // Def: 2.5
#define CP2TMP 34   // Def: 300
#define CP2GMI 35   // Def: 1 = CG geometry optimizer
#define CP2MAG 36   // Def: 10000 max step geo
#define CP2GEF 37   // Def: 0.0001
#define CP2OUF 38   // Def: 10
#define CP2CON 39   // Def: 0
#define CP2OUU 40   // Def: 0

#define NCP2KCALC 7
#define CP2NTHERM 5

extern cp2k * tmp_cp2k;

extern double default_cp2k_options[41];
extern double default_cp2k_extra[3][4];
extern double default_vdw_cut[2];

extern int cp2k_default_num[12];

extern gchar * cp2k_default_keywords[11][11];
extern gchar * cp2k_default_text[11][11];
extern gchar * cp2k_vdw_keywords[2][3];

extern int cp2k_is_basis_in_database (int sp);
extern int cp2k_is_pseudo_in_database (int sp);
extern GtkWidget * prepare_basis_combo (int sp, int obj);
extern gchar * get_nth_elem (int sp, int id, int obj);
extern gchar * get_nth_key (int sp, int id, int obj);

extern int find_cp2k_sym ();

// For CP2K v9.1
#define N_MOLOPT 191
extern char * molopt_elem[N_MOLOPT];
extern char * molopt_keys[N_MOLOPT];
extern char * cp2k_molopt[N_MOLOPT];
#define N_GTH 156
extern char * gth_elem[N_GTH];
extern char * gth_keys[N_GTH];
extern char * cp2k_gth[N_GTH];
#define N_BASIS 250
extern char * basis_elem[N_BASIS];
extern char * basis_keys[N_BASIS];
extern char * cp2k_basis[N_BASIS];
#define N_POTS 421
extern char * pot_elem[N_POTS];
extern char * pot_keys[N_POTS];
extern char * cp2k_pot[N_POTS];
#endif
