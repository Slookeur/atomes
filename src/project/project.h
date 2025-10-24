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
* @file project.h
* @short Function declarations for reading atomes project file \n
         Function declarations for saving atomes project file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'project.h'
*
* Contains:

 - Function declarations for reading atomes project file
 - Function declarations for saving atomes project file

*/

#ifndef PROJECT_H_
#define PROJECT_H_

#define IODEBUG FALSE

extern int num_bonds (int i);
extern int num_angles (int i);
extern int num_dihedrals (int i);

// Read
extern int read_atom_a (FILE * fp, project * this_proj, int s, int a);
extern int read_atom_b (FILE * fp, project * this_proj, int s, int a);
extern int read_opengl_image (FILE * fp, project * this_proj, image * img, int sid);
extern int read_project_curve (FILE * fp, int wid, int pid);
extern int read_mol (FILE * fp);
extern int read_bonding (FILE * fp);
extern int read_dlp_field_data (FILE * fp, project * this_proj);
extern int read_lmp_field_data (FILE * fp, project * this_proj);
extern int read_cpmd_data (FILE * fp, int cid, project * this_proj);
extern int read_cp2k_data (FILE * fp, int cid, project * this_proj);
extern gchar * read_this_string (FILE * fp);
extern void alloc_proj_data (project * this_proj,  int cid);
extern int open_project (FILE * fp, int npw);

// Save
extern int save_atom_a (FILE * fp, project * this_proj, int s, int a);
extern int save_opengl_image (FILE * fp, project * this_proj, image * img, int sid);
extern int save_project_curve (FILE * fp, int wid, project * this_proj, int rid, int cid);
extern int save_dlp_field_data (FILE * fp, project * this_proj);
extern int save_lmp_field_data (FILE * fp, project * this_proj);
extern int save_cpmd_data (FILE * fp, int cid, project * this_proj);
extern int save_cp2k_data (FILE * fp, int cid, project * this_proj);
extern int save_this_string (FILE * fp, gchar * string);
extern int save_mol (FILE * fp, project * this_proj);
extern int save_bonding (FILE * fp, project * this_proj);
extern int save_project (FILE * fp, project * this_proj, int wid);

extern G_MODULE_EXPORT void set_color_map (GtkWidget * widg, gpointer data);
extern int * save_color_map (glwin * view);
extern void restore_color_map (glwin * view, int * colm);
extern colormap * allocate_color_map (int pts, project * this_proj);
extern gboolean setup_custom_color_map (float * data, project * this_proj, gboolean init);
#ifdef GTK4
extern G_MODULE_EXPORT gboolean edit_tab (GtkWidget * widget, GdkEvent * event, gpointer fdata);
#else
extern G_MODULE_EXPORT gboolean edit_tab (GtkWidget * widget, GdkEventButton * event, gpointer fdata);
#endif
extern void init_curves_and_calc (project * this_proj);
extern void init_project (gboolean alloc_box);
extern void update_analysis_availability (project * this_proj);
extern int update_project ();
extern void clean_view ();
extern void view_buffer (GtkTextBuffer * buffer);
extern void update_insert_combos ();
extern void close_project (project * to_close);
extern void to_close_this_project (int to_activate, project * this_proj);
extern void hide_current_project (project * to_hide);
extern void active_project_changed (int id);
extern void opengl_project_changed (int id);
extern G_MODULE_EXPORT void on_close_activate (GtkWidget * widg, gpointer cdata);
extern void add_project (GtkTreeStore * store, int i);

extern void debugiocurve (project * this_proj, gboolean win, int rid, int cid, gchar * iost);
extern void debugioproj (project * this_proj, gchar * iost);
#endif
