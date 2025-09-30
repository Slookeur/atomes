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

/*! \typedef bond_cutoff

  \brief bond cutoff data structure
*/
typedef struct bond_cutoff bond_cutoff;
struct bond_cutoff
{
  int Z[2];         /*!< Atomic numbers */
  double cutoff;    /*!< Associated radius */
  gboolean use;     /*!< Use or not */
  bond_cutoff * next;
  bond_cutoff * prev;
};

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

/*! \typedef rep_data

  \brief representation data structure
*/
typedef struct rep_data rep_data;
struct rep_data
{
  int rep;
  int proj;
  GLdouble c_angle[2];   /*!< Camera angle: pitch and heading */
  GLdouble c_shift[2];   /*!< Camera position: x and y */
  GLdouble gnear;        /*!< Camera depth position */
  GLdouble zoom;         /*!< Zoom factor */
};

extern float get_radius (int object, int col, int z, element_radius * rad_list);
extern ColRGBA get_spec_color (int z, element_color * clist);
extern GtkWidget * adv_box (GtkWidget * box, char * lab, int vspace, int size, float xalign);
extern void duplicate_background_data (background * new_back, background * old_back);
extern void duplicate_box_data (box * new_box, box * old_box);
extern void duplicate_axis_data (axis * new_axis, axis * old_axis);

// Analysis parameters

extern gboolean preferences;

extern double default_totcut;
extern double tmp_totcut;
extern bond_cutoff * default_bond_cutoff;
extern bond_cutoff * tmp_bond_cutoff;

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

extern gboolean default_clones;
extern gboolean * default_o_at_rs;
extern double * default_at_rs;
extern gboolean * default_o_bd_rw;
extern double * default_bd_rw;

extern screen_label default_label[5];
extern screen_label * tmp_label[5];
extern int default_acl_format[2];
extern int tmp_acl_format[2];
extern gboolean default_mtilt[2];
extern gboolean tmp_mtilt[2];
extern int default_mpattern[2];
extern int tmp_mpattern[2];
extern int default_mfactor[2];
extern int tmp_mfactor[2];
extern double default_mwidth[2];
extern double tmp_mwidth[2];

extern rep_data default_rep;
extern rep_data * tmp_rep;
extern rep_edition * pref_rep_win;
extern background default_background;
extern background * tmp_background;
extern box default_box;
extern box * tmp_box;
extern box_edition * pref_box_win;
extern axis default_axis;
extern axis * tmp_axis;
extern axis_edition * pref_axis_win;
extern ColRGBA default_sel_color[2];

extern gradient_edition * pref_gradient_win;

extern element_color * default_label_color[2];
extern element_color * default_atom_color[2];

extern tint * pref_pointer;

extern opengl_edition * pref_ogl_edit;

extern void set_atomes_preferences ();

#endif // PREFERENCES_H_
