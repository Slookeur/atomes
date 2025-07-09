/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2025 by CNRS and University of Strasbourg */

/*!
* @file preferences.h
* @short Preference variable declarations
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'preferences.h'
*
* Contains:

 - Preference variable declarations

*/

#ifndef PREFERENCES_H_

#define PREFERENCES_H_

/*! \typedef element_radius

  \brief element radius data structure
*/
typedef struct element_radius element_radius;
struct element_radius
{
  int Z;            /*!< Atomic number */
  double rad;       /*!< Associated radius */
  element_radius * next;
  element_radius * prev;
};

/*! \typedef element_color

  \brief element color data structure
*/
typedef struct element_color element_color;
struct element_color
{
  int Z;            /*!< Atomic number */
  ColRGBA col;      /*!< Associated color */
  element_color * next;
  element_color * prev;
};

extern float get_radius (int object, int col, int z, element_radius * rad_list);

// Analysis parameters

extern gboolean preferences;

extern gboolean default_clones;
extern gchar * default_delta_num_leg[8];
extern int * default_num_delta;
extern int * tmp_num_delta;
extern double * default_delta_t;
extern gchar * default_ring_param[7] ;
extern int * default_rsparam;
extern int * tmp_rsparam;
extern gchar * default_chain_param[7];
extern int * default_csparam;
extern int * tmp_csparam;

// OpenGL
extern int * default_opengl;
extern int * tmp_opengl;
extern Material default_material;
extern Material tmp_material;
extern Lightning default_lightning;
extern Lightning tmp_lightning;
extern Fog default_fog;
extern Fog tmp_fog;

// Model
extern element_radius * default_atomic_rad[16];
extern element_radius * tmp_atomic_rad[16];
// 3 styles + 3 cloned styles
extern element_radius * default_bond_rad[6];
extern element_radius * tmp_bond_rad[6];

extern gboolean * default_o_at_rs;
extern double * default_at_rs;
extern gboolean * default_o_bd_rw;
extern double * default_bd_rw;

extern screen_label default_label[5];
extern screen_label * tmp_label[5];
extern int default_acl_format[2];
extern int tmp_acl_format[2];
extern gboolean default_mtilt;
extern gboolean tmp_mtilt;
extern int defaut_mpattern;
extern int tmp_mpattern;
extern int default_mfactor;
extern int tmp_mfactor;
extern double default_mwidth;
extern double tmp_mwidth;

extern tint * pref_pointer;

extern opengl_edition * pref_ogl_edit;

extern void set_atomes_preferences ();

#endif // PREFERENCES_H_
