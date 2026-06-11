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
* @file submenus.h
* @short Function declarations for the creation of the OpenGL window menus
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'submenus.h'
*
* Contains:

 - Function declarations for the creation of the OpenGL window menus

*/

#ifndef SUBMENUS_H_
#define SUBMENUS_H_

#ifdef GTK3
extern void menu_items_opengl (GtkWidget * menu, glwin * view, int popm);
extern GtkWidget * menu_style (glwin * view, int id);
extern GtkWidget * menu_map (glwin * view, int id);
extern GtkWidget * menu_atoms ( glwin * view, int id, int at);
extern GtkWidget * menu_bonds (glwin * view, int id, int at);
extern GtkWidget * menu_clones (glwin * view, int id);
extern GtkWidget * menu_box_axis (glwin * view, int id, int ab);
extern void detach_frag_mol_menu (glwin * view, int id, int jd);
extern GtkWidget * menu_coord (glwin * view, int id);
extern GtkWidget * add_menu_coord (glwin * view, int id, int jd);
extern GtkWidget * menu_poly (glwin * view, int id);
extern GtkWidget * menu_rings (glwin * view, int id);
extern GtkWidget * menu_edit (glwin * view, int id);
extern GtkWidget * menu_tools (glwin * view, int id);
extern GtkWidget * menu_rep (glwin * view, int id);
extern GtkWidget * menu_proj (glwin * view);
extern GtkWidget * menu_back (glwin * view);
extern void menu_axis (GtkWidget * menu_ab, glwin * view, int id);
extern GtkWidget * menu_anim (glwin * view, int id);
extern void menu_items_view (GtkWidget * menu, glwin * view, int popm);
#else
extern GMenu * menu_style (glwin * view, int popm);
extern GMenu * menu_map (glwin * view, int popm);
extern GMenu * menu_atoms ( glwin * view, int popm, int at);
extern GMenu * menu_bonds (glwin * view, int popm, int at);
extern GMenu * menu_clones (glwin * view, int popm);
extern GMenu * axis_box_param (glwin * view, int popm, int ab, int style);
extern GMenuItem * menu_box_axis (glwin * view, int popm, int ab);
extern GMenu * color_item (glwin * view, gchar * act, int id, GCallback handler, gpointer data);
extern GMenu * menu_coord (glwin * view, int popm);
extern GMenu * add_menu_coord (glwin * view, int popm, int id);
extern GMenu * menu_poly (glwin * view, int popm);
extern GMenu * menu_rings (glwin * view, int popm);
extern GMenu * extract_section (glwin * view, int popm);
extern GMenu * menu_edit (glwin * view, int popm);
extern GMenu * menu_tools (glwin * view, int popm);
extern GMenu * menu_reset (glwin * view, int popm);
extern GMenu * menu_fullscreen (glwin * view, int popm);
extern GMenu * menu_rep (glwin * view, int popm);
extern GMenu * menu_proj (glwin * view, int popm);
extern GMenu * menu_back (glwin * view, int popm);
extern void menu_axis (GMenu * menu, glwin * view, int popm);
extern void menu_items_view (GMenu * menu, glwin * view, int popm);
extern GMenu * prepare_opengl_menu (glwin * view, int popm);
extern GMenu * prepare_model_menu (glwin * view, int popm);
extern GMenu * prepare_coord_menu (glwin * view, int popm);
extern GMenu * menu_view (glwin * view, int id);
extern GMenu * menu_anim (glwin * view, int popm);
extern GMenu * menu_shortcuts (glwin * view, int popm);
#endif

#endif
