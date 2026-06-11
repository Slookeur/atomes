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
* @file color_box.h
* @short Structure definitions for color management \n
         Function declarations for color management
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'color_box.h'
*
* Contains:

 - Structure definitions for color management
 - Function declarations for color management

*/

typedef struct {
  double red;
  double green;
  double blue;
} color;

extern color colorp[64];

#ifdef GTK4
extern void color_box (glwin * view, int ideo, int spec, int geo);
extern G_MODULE_EXPORT void to_run_back_color_window (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void to_run_box_color_window (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void to_run_atom_color_window (GSimpleAction * action, GVariant * parameter, gpointer data);
#else
extern GtkWidget * color_box (glwin * view, int ideo, int spec, int geo);
extern G_MODULE_EXPORT void window_color_coord (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void to_run_back_color_window (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void to_run_box_color_window (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void to_run_atom_color_window (GtkWidget * widg, gpointer data);
#endif
extern G_MODULE_EXPORT void window_color_total_poly (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void window_color_partial_poly (GtkWidget * widg, gpointer data);
