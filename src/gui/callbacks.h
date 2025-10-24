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
* @file callbacks.h
* @short Callback declarations for main window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'callbacks.h'
*
* Contains:

 - Callback declarations for main window

*/

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

extern void fill_tool_model ();
void free_data ();
void quit_gtk ();
void show_Help ();

G_MODULE_EXPORT int open_save_workspace (FILE * fp, int act);
G_MODULE_EXPORT int open_save (FILE * fp, int i, int pid, int aid, gchar * pfile);
G_MODULE_EXPORT void on_close_workspace (GtkWidget * widg, gpointer data);
G_MODULE_EXPORT void on_open_save_activate (GtkWidget * widg, gpointer data);
G_MODULE_EXPORT void on_save_as_activate (GtkWidget * widg, gpointer data);
extern void open_this_isaacs_xml_file (gchar * profile, int ptoc, gboolean visible);
G_MODULE_EXPORT void on_isaacs_port (GtkWidget * widg, gpointer data);
extern void open_this_coordinate_file (int format, gchar * proj_name);
G_MODULE_EXPORT void on_coord_port (GtkWidget * widg, gpointer data);
void to_read_pos ();
void display_distances ();
void run_project ();
void apply_project (gboolean showtools);
G_MODULE_EXPORT void on_Bonds_activate ();
G_MODULE_EXPORT void on_Help_released (GtkButton * but, gpointer data);
G_MODULE_EXPORT void on_check_toggled (GtkToggleButton * Button, gpointer data);

G_MODULE_EXPORT void expanding (GtkExpander * expander, gpointer data);
G_MODULE_EXPORT void on_show_curve_toolbox (GtkWidget * widg, gpointer data);
#endif
