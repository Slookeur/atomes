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
* @file cell_extra.c
* @short Functions to create the 'add cell(s)' tab in the cell edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cell_extra.c'
*
* Contains:
*

 - The functions to create the 'add cell(s)' tab in the cell edition window

*
* List of functions:

  G_MODULE_EXPORT void add_cell (GtkSpinButton * res, gpointer data);

  GtkWidget * add_extra_cell_tab (glwin * view);

*/

#include "cell_edit.h"

/*!
  \fn G_MODULE_EXPORT void add_cell (GtkSpinButton * res, gpointer data)

  \brief add cell callback

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void add_cell (GtkSpinButton * res, gpointer data)
{
  tint * dat = (tint *)data;
  int i;
  project * this_proj = get_project_by_id (dat -> a);
  image * last = this_proj -> modelgl -> anim -> last -> img;
  i = max(last -> abc -> extra_cell[0], last -> abc -> extra_cell[1]);
  i = max(i, last -> abc -> extra_cell[2]);
  last -> p_depth /= (i+1);
  last -> abc -> extra_cell[dat -> b] = gtk_spin_button_get_value_as_int(res) - 1;
  i = max(last -> abc -> extra_cell[0], last -> abc -> extra_cell[1]);
  i = max(i, last -> abc -> extra_cell[2]);
  last -> p_depth *= (i+1);
  double fact = last -> p_depth*2.0 / last -> gfar;
  last -> gfar = last -> p_depth*2.0;
  last -> gnear *= fact;
  last -> zoom *= fact;
  init_default_shaders (this_proj -> modelgl);
  this_proj -> modelgl -> create_shaders[MDBOX] = TRUE;
  this_proj -> modelgl -> create_shaders[MAXIS] = TRUE;
  if (this_proj -> modelgl -> n_shaders[SLABS][0]) this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
  update (this_proj -> modelgl);
  sens_superbut (this_proj);
}

/*!
  \fn GtkWidget * add_extra_cell_tab (glwin * view)

  \brief create the add cell(s) tab

  \param view the target glwin
*/
GtkWidget * add_extra_cell_tab (glwin * view)
{
  GtkWidget * layout = create_layout (350, 150);
  GtkWidget * hbox = create_hbox (50);
  GtkWidget * vbox = create_vbox (BSEP);
  layout_add_widget (layout, hbox, 50, 50);
  int i;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("\tAdd extra cell(s):", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
  GtkWidget * hhbox;
  gchar * str;
  for (i=0; i<3; i++)
  {
    hhbox = create_hbox (0);
    view -> cell_win -> ax_cell[i] = spin_button (G_CALLBACK(add_cell), view -> anim -> last -> img -> abc -> extra_cell[i]+1, 1, 1000, 1, 0, 100, & view -> colorp[i][0]);
    str = g_strdup_printf ("%s x ", box_prop[0][i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 40);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, view -> cell_win -> ax_cell[i], FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hhbox, FALSE, FALSE, 5);
  }
  vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbox, FALSE, FALSE, 50);
  view -> cell_win -> superbut = create_button ("Create super-cell", IMG_NONE, NULL, 100, -1, GTK_RELIEF_NORMAL, G_CALLBACK(super_cell_but), view);
  sens_superbut (get_project_by_id(view -> proj));
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, view -> cell_win -> superbut, FALSE, FALSE, 90);
  show_the_widgets (layout);
  return layout;
}
