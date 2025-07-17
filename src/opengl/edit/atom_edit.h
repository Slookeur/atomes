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
* @file atom_edit.h
* @short Function declarations for the mode edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'atom_edit.h'
*
* Contains:

 - Function declarations for the mode edition window

*/

#include "global.h"
#include "bind.h"
#include "callbacks.h"
#include "interface.h"
#include "initcoord.h"
#include "glview.h"
#include "glwindow.h"
#include "project.h"
#include "workspace.h"

#ifndef ATOM_EDIT_H_
#define ATOM_EDIT_H_

#define IDCOL 0
#define TOLAB 3
#define TOPIC 4

extern GtkWidget * selection_tab (atom_search * asearch, int nats);
extern G_MODULE_EXPORT void set_show_axis_toggle (GtkToggleButton * but, gpointer data);
extern void save_rotation_quaternion (glwin * view);
extern void center_molecule (project * this_proj);
extern vec3_t get_arc_ball_vector (glwin * view, int x, int y);
extern void update_search_tree (atom_search * asearch);
extern int selected_aspec;
extern int is_selected;
extern chemical_data * alloc_chem_data (int spec);
extern void setup_image_spec_data (project * this_proj, image * img);
extern void free_glwin_spec_data (project * this_proj, int spec);
extern void glwin_init_spec_data (project * this_proj, int nspec);
extern void prepare_opengl_menu_bar (glwin * view);
extern gboolean test_vol (double box[2][3], double vect[3][3]);
extern GtkTreeModel * replace_combo_tree (gboolean insert, int proj);
extern int get_selected_object_id (gboolean visible, int p, gchar * str, atom_search * asearch);
extern int get_atom_id_from_periodic_table (atom_search * asearch);
extern G_MODULE_EXPORT void set_mode (GtkWidget * widg, gpointer data);
extern insertion_menu mol[];
extern atom_search * remove_search;
extern GtkWidget * coord_menu (glwin * view);
extern GtkWidget * advanced_coord_properties (glwin * view, int page);
extern GtkWidget * create_cell_edition_window (project * this_proj, gpointer data);
extern G_MODULE_EXPORT void window_spinner (GtkWidget * widg, gpointer data);
extern gchar * mot[2][2];

extern gboolean in_bond (int at, int bd[2]);
extern int being_copied;
extern atomic_object * lib_object;
extern float limit[2];
extern gchar * action_name[5];
extern gchar * action_atoms[3];
extern void check_all_trees (project * this_proj);
extern atomic_object * get_atomic_object_by_origin (atomic_object * first, int oid, int aid);
extern void adjust_object_to_move (project * this_proj, atom_search * asearch, int mv, int id);
extern void motion_to_zero (atom_search * asearch);
extern int get_asearch_num_objects (atom_search * asearch);
extern int get_asearch_object (atom_search * asearch);
extern int get_asearch_filter (atom_search * asearch);
extern int get_todo_size (atom_search * asearch);
extern void allocate_todo (atom_search * asearch, int tsize);
extern void clean_todo (atom_search * asearch);
extern void clean_picked_and_labelled (atom_search * asearch, gboolean clean_msd);
extern void adjust_search_param (atom_search * asearch, project * this_proj, int a, int s, int c, gboolean status);
extern void re_populate_tree_search (atom_search * asearch);
extern G_MODULE_EXPORT void set_search_mode (GtkComboBox * box, gpointer data);
extern G_MODULE_EXPORT void set_object_changed (GtkComboBox * box, gpointer data);
extern G_MODULE_EXPORT void set_filter_changed (GtkComboBox * box, gpointer data);
extern G_MODULE_EXPORT void set_spec_changed (GtkComboBox * box, gpointer data);

extern void add_bonds_to_list (int ** new_bond_list, int nat, int nbd, atomic_object * object);
extern void add_bonds_to_project (project * this_proj, int removed, int nbd, int ** new_bond_list);
extern gboolean * remove_bonds_from_project (project * this_proj, atomic_object * this_object, int * old_id, atom * new_list, gboolean remove, gboolean passivate);
#ifdef GTK4
extern G_MODULE_EXPORT void set_reset_transformation (GtkCheckButton * but, gpointer data);
#else
extern G_MODULE_EXPORT void set_reset_transformation (GtkToggleButton * but, gpointer data);
#endif
extern void to_remove_this_list_of_objects (project * this_proj, atom_search * asearch);
extern void to_passivate_using_the_objects (project * this_proj, atom_search * asearch);

extern tint ulam_coord (glwin * view);
extern void correct_pos_and_get_dim (atomic_object * object, gboolean adjust);
extern atomic_object * duplicate_atomic_object (atomic_object * old_obj);
extern atomic_object * create_object_from_species (project * this_proj, int sid, atom_search * remove);
extern void reconstruct_bonds (project * this_proj, int ifcl, int * bcid);
extern void reconstruct_coordinates_for_object (project * this_proj, atomic_object * this_object, gboolean upcoord);
extern atomic_object * create_object_from_selection (project * this_proj);
extern atomic_object * create_object_from_atom_coordination (project * this_proj, int coord, int aid, atom_search * remove);
extern atomic_object * create_object_from_overall_coordination (project * this_proj, int coord, int aid, atom_search * remove);
extern atomic_object * create_object_from_frag_mol (project * this_proj, int coord, int geo, atom_search * remove);
extern void create_object_from_library (int p);
extern int create_object_from_open_project (project * this_proj, int p);
extern void clean_this_object (int orig, int act, project * this_proj, atom_search * asearch);
extern void to_insert_in_project (int stat, int orig, project * this_proj, atom_search * asearch, gboolean visible);
extern int insert_this_project_from_lib (int id, gboolean visible, project * this_proj, atom_search * asearch);
extern G_MODULE_EXPORT void set_atoms_to_insert (GtkComboBox * box, gpointer data);

extern void clean_other_window_after_edit (project * this_proj);
extern void clean_coord_window (project * this_proj);
extern GtkWidget * create_atom_notebook (project * this_proj, GtkWidget * vbox);
extern double ** save_coordinates (project * this_proj, int status);
extern void reset_coordinates (project * this_proj, int status);
extern vec3_t get_bary (project * this_proj, int status);
extern void init_coordinates (project * this_proj, int status, gboolean win, gboolean init);
extern void translate (project * this_proj, int status, int axis, vec3_t trans);
extern void rotate_quat (project * this_proj, vec4_t q, int status, int axis);
extern void rotate (project * this_proj, int i, int j, int k, float param);
extern void random_move (project * this_proj, atom_search * asearch);
extern void update_coordinates (project * this_proj, int status, int axis, int action);
extern G_MODULE_EXPORT void repeat_move (GtkSpinButton * res, gpointer data);

#ifdef GTK4
extern G_MODULE_EXPORT void to_set_move (GtkEditable * widg, gpointer data);
#else
extern G_MODULE_EXPORT gboolean to_set_move (GtkWidget * widg, GdkEventFocus * event, gpointer data);
#endif
extern G_MODULE_EXPORT void set_move (GtkEntry * res, gpointer data);
extern void range_has_changed (gpointer data, double v);
extern void update_range_and_entry (project * this_proj, int i, int j, int k);
extern G_MODULE_EXPORT gboolean scroll_range_move (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
extern G_MODULE_EXPORT void range_move (GtkRange * range, gpointer data);
extern void check_motion_interactors (project * this_proj, atom_search * asearch);
extern GtkWidget * add_motion_interaction (atom_search * asearch, int axd, project * this_proj);

extern void apply_action (project * this_proj, atom_search * asearch);
extern void clean_all_trees (atom_search * asearch, project * this_proj);
extern gboolean do_we_have_objects_in_selection (project * this_proj, atom_search * asearch, gboolean editing);
extern G_MODULE_EXPORT void take_action (GtkButton * but, gpointer data);
extern GtkWidget * action_tab (int aid, project * this_proj);

extern int find_this_geo_id (int id, coord_info * obj, int * old_z, int old_geo, int old_sp, int new_sp, coord_info * coord, double * new_z);
extern void check_coord_modification (project * this_proj, int old_id[], atom * new_list,
                                      atomic_object * this_object, gboolean movtion, gboolean passivating);
extern void print_coord_info (project * this_proj, coord_info * coord);
extern coord_info * duplicate_coord_info (coord_info * old_coord);
extern void recover_opengl_data (project * this_proj, int nmols, int add, int rem, int * num, int * rec, int *** tmpgeo, gboolean * showfrag);

extern chemical_data * duplicate_chemical_data (int spec, chemical_data * chem);
extern int find_spec_id (int s, int z, double * list_z);
extern int search_for_new_spec (atom_edition * edit, atomic_object * object);

extern void prepare_atom_edition (gpointer data, gboolean visible);

#ifdef GTK4
G_MODULE_EXPORT void action_window (GSimpleAction * action, GVariant * parameter, gpointer data);
#else
G_MODULE_EXPORT void action_window (GtkWidget * widg, gpointer data);
#endif
G_MODULE_EXPORT void close_build (GtkWidget * widg, gpointer data);
#endif
