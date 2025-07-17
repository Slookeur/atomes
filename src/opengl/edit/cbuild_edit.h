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
* @file cbuild_edit.h
* @short Function declarations for the crystal builder
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'cbuild_edit.h'
*
* Contains:

 - Function declarations for the crystal builder

*/

#ifndef CBUILD_EDIT_H_
#define CBUILD_EDIT_H_

#include "global.h"
#include <libxml/xmlreader.h>

typedef struct crystal_data crystal_data;
struct crystal_data
{
  int objects;
  int spec;
  double * z;
  int ** lot;
  int ** at_type;
  int * nsps;
  int * at_by_object;
  int * pos_by_object;
  double * occupancy;
  int rounding;
  gboolean overlapping;
  int ** sites;
  gboolean shared_sites;
  gboolean with_holes;
  gboolean * holes;
  vec3_t * insert;
  vec3_t ** coord;
  vec3_t ** position;
};

extern int clean_xml_data (xmlDoc * doc, xmlTextReaderPtr reader);
extern xmlNodePtr findnode (xmlNodePtr startnode, char * nname);
extern gchar * groups[230];
extern gchar * hmsymbols[230];
extern dint t_box[9];
extern double tmp_box[2][3];
extern double tmp_vect[3][3];
extern char * vect_name[3];
extern char * vect_comp[3];
extern char * box_p[2];
extern char * box_prop[2][3];
extern gchar * cif_occupancies[3];
extern gchar * cif_occ[3];
extern gchar * cif_sites[2];
extern atom_search * insert_search;
extern gboolean crystal_low_warning;
extern gboolean crystal_crowded;
extern gboolean crystal_dist_chk;
extern gboolean test_vol (double box[2][3], double vect[3][3]);
extern G_MODULE_EXPORT void update_vect (GtkEntry * entry, gpointer data);
extern G_MODULE_EXPORT void update_box (GtkEntry * entry, gpointer data);
extern double get_value_from_pos (gchar * pos);
extern double get_val_from_wyckoff (gchar * pos, gchar * wval);
extern crystal_data * allocate_crystal_data (int objects, int species);
extern crystal_data * free_crystal_data (crystal_data * cryst);
#ifdef GTK4
extern G_MODULE_EXPORT void to_update_box (GtkEditable * widg, gpointer data);
extern G_MODULE_EXPORT void to_update_vect (GtkEditable * widg, gpointer data);
#else
extern G_MODULE_EXPORT gboolean to_update_box (GtkWidget * widg, GdkEventFocus * event, gpointer data);
extern G_MODULE_EXPORT gboolean to_update_vect (GtkWidget * widg, GdkEventFocus * event, gpointer data);
#endif
extern int occupancy (double occ, int cif_occ);
extern gchar * replace_markup (char * init, char * key, char * rep);
extern gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif);
extern space_group * read_sg_xml_file (const char * filetoread);
extern GtkWidget * create_atoms_tree (atom_search * asearch, project * this_proj, int na);
extern atom_search * allocate_atom_search (int proj, int action, int searchid, int tsize);
extern GtkWidget * create_action_combo (int id, project * this_proj);
extern void prepare_atom_edition (gpointer data, gboolean visible);
extern void clean_all_trees (atom_search * asearch, project * this_proj);
extern void prepare_opengl_menu_bar (glwin * view);
extern void allocatoms (project * this_proj);
extern chemical_data * alloc_chem_data (int spec);
extern void setup_default_lights (project * this_proj, image * img);
extern void setup_image_spec_data (project * this_proj, image * img);
extern void glwin_init_spec_data (project * this_proj, int nspec);
extern G_MODULE_EXPORT void set_mode (GtkWidget * widg, gpointer data);
extern void shift_it (vec3_t shift, int refresh, int proj);
extern G_MODULE_EXPORT void close_edit (GtkWidget * widg, gpointer data);
extern int insert_this_project_from_lib (int id, gboolean visible, project * this_proj, atom_search * asearch);
extern void to_insert_in_project (int stat, int orig, project * this_proj, atom_search * asearch, gboolean visible);
#endif
