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
* @file glwindow.h
* @short Function declarations for the creation of the OpenGL window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'glwindow.h'
*
* Contains:

 - Function declarations for the creation of the OpenGL window

*/

extern void set_this_style (glwin * view, int style);
extern void update_menus (glwin * view);
extern void prep_model (int p);

extern void append_opengl_item (glwin * view, GMenu * menu, const gchar * name, gchar * key, int mpop, int item_id,
                                gchar * accel, int image_format, gpointer icon,
                                gboolean custom, GCallback handler, gpointer data,
                                gboolean check, gboolean status, gboolean radio, gboolean sensitive);

#ifdef GTK4
extern G_MODULE_EXPORT void to_reset_view (GSimpleAction * action, GVariant * parameter, gpointer data);
extern void activate_glwin_action (gchar * action_string, gchar * action_name, glwin * view);
#else
extern G_MODULE_EXPORT void to_reset_view (GtkWidget * widg, gpointer data);
#endif
