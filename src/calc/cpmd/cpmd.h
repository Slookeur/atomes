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
* @file cpmd.h
* @short Variable declarations for the creation of the CPMD input file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'cpmd.h'
*
* Contains:

  Variable declarations for the creation of the CPMD input file

*/

#define NCPMDCALC 7
#define NCACOMBO 5
#define NDFT 19
#define NSYM 11
#define NOPTPC 7

#define NSECOP 6
#define DEFEM 0
#define DEFLS 1
#define DEFVD 2
#define DEFDF 3
#define DEFGC 4
#define DEFAN 5
#define DEFVE 6
#define DEFSY 7
#define DEFAB 8
#define DEFDG 9
#define DEFCU 10
#define DEFCO 11
#define DEFFI 12
#define DEFDU 13
#define DEFSP 14
#define DEFLM 15
#define DEFLO 16

#define CONVO 0
#define OPTIO 1
#define STEPO 2
#define TSTPO 3
#define CONVG 4
#define OPTIG 5
#define STEPG 6
#ifndef CPMD_H_
#define CPMD_H_

#define TSTPG 7
#define STEPC 8
#define TSTPC 9
#define BAROC 10
#define ANNIC 11
#define AFAIC 12
#define ANNEC 13
#define AFAEC 14
#define STEPB 15
#define TSTPB 16
#define BAROB 17
#define ANNIB 18
#define AFAIB 19
#define KSUNO 20
#define RHOUT 21
#define NBAND 22
#define VIBRA 23

extern cpmd * tmp_cpmd;
extern gboolean is_cpmd;

extern gchar * cpmd_elements[MAXDATAQM];
extern gchar * cdescr[MAXDATAQM];
extern double default_cpmd_options[17];
extern gchar * default_opts[MAXDATAQM-1][NSECOP];
// 0 = None, 1 = Entry, 2 = Combo, 3 = yes/no
extern int default_opts_type[MAXDATAQM-1][NSECOP];
// NDF is the max by far
extern int defaut_num[9];
extern gchar * default_keywords[9][NDFT];
extern gchar * default_text[9][NDFT];
extern double default_calc_options[24];
extern gchar * calc_opts[NCPMDCALC][NOPTPC];
extern int default_type[NCPMDCALC][NOPTPC];
extern gchar * calc_kw[NCPMDCALC];
extern gchar * calc_ds[NCPMDCALC];
extern int calc_box_num[NCACOMBO];
extern gchar * calc_box_name[NCACOMBO][3];
extern gchar * calc_box_keys[NCACOMBO][3];

extern GtkWidget * sel_but[3];
extern GtkWidget * sel_img[3];

extern GtkWidget * cpmd_box (GtkWidget * box, gchar * lab, int v_space, int h_space, int dim);

/*! \enum therm_types */
enum therm_types {
  CONTROL = -1, /*!< -1 */
  GLOBAL = 0, /*!< 0 */
  LOCAL = 1, /*!< 1 */
  MOLECULE = 2 /*!< 2 */
};

extern gchar * nosetype[3];
extern gchar * nosekey[2];
extern gchar * thermo_name[2][5];
extern int num_thermo[2];
extern int type_thermo[2];

extern gchar * param[2];
extern gchar * iunit[2];
extern gchar * eunit[2];
#endif
