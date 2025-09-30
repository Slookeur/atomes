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
* @file cell_density.c
* @short Functions to create the 'density adjustment' tab in the cell edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cell_density.c'
*
* Contains:
*

 - The functions to create the 'density adjustment' tab in the cell edition window

*
* List of functions:

  void display_density (cell_edition * cell, double vol, double dens, double adens);

  G_MODULE_EXPORT void set_rescaling (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_rescaling (GtkToggleButton * but, gpointer data);

  GtkWidget * adjust_density_tab (project * this_proj);

*/

#include "cell_edit.h"

/*!
  \fn void display_density (cell_edition * cell, double vol, double dens, double adens)

  \brief create density information widgets

  \param cell the target cell edition
  \param vol the volume
  \param dens the density
  \param adens the atomic density
*/
void display_density (cell_edition * cell, double vol, double dens, double adens)
{
  cell -> density = destroy_this_widget (cell -> density);
  cell -> density = create_vbox (BSEP);
  GtkWidget * hbox;
  gchar * pname[3]= {"Volume:", "Mass density:", "Number density:"};
  gchar * punit[3]= {"&#xC5;<sup>3</sup>", "g/cm<sup>3</sup>", "atom/&#xC5;<sup>3</sup>"};
  double val[3];
  val[0] = vol;
  val[1] = dens;
  val[2] = adens;
  int i;
  gchar * str;
  for (i=0; i<3; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, cell -> density, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(pname[i], 120, -1, 0.0, 0.5), FALSE, FALSE, 100);
    str = g_strdup_printf ("<b>%f</b>", val[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 120, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(punit[i], 20, -1, 0.5, 0.5), FALSE, FALSE, 10);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, cell -> density_box, cell -> density, FALSE, FALSE, 10);
  show_the_widgets (cell -> density);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_rescaling (GtkCheckButton * but, gpointer data)

  \brief homogeneous rescaling toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_rescaling (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_rescaling (GtkToggleButton * but, gpointer data)

  \brief homogeneous rescaling toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_rescaling (GtkToggleButton * but, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  view -> cell_win -> homo_density = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (view -> cell_win -> shift_box[1], view -> cell_win -> homo_density);
}

/*!
  \fn GtkWidget * adjust_density_tab (project * this_proj)

  \brief create the density tab

  \param this_proj the target project
*/
GtkWidget * adjust_density_tab (project * this_proj)
{
  GtkWidget * layout = create_layout (350, 400);
  GtkWidget * vbox = create_vbox (BSEP);
  layout_add_widget (layout, vbox, 100, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("Modify the density of the simulation box <sup>*</sup>: ", -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, check_button ("Homogeneous rescaling <sup>**</sup>", -1, -1, this_proj -> modelgl -> cell_win -> homo_density,
                                           G_CALLBACK(set_rescaling), this_proj -> modelgl), FALSE, FALSE, 5);
  int i;
  for (i=3; i<6; i++) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_cell_entries (this_proj, i), FALSE, FALSE, 0);
  this_proj -> modelgl -> cell_win -> density_box = create_vbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, this_proj -> modelgl -> cell_win -> density_box, FALSE, FALSE, 5);
  display_density (this_proj -> modelgl -> cell_win, this_proj -> cell.volume,
                   this_proj -> cell.density, this_proj -> natomes/this_proj -> cell.volume);
  gchar * info[2] = {" * Visual information will be preserved, real bonding is likely to be modified.\n"
                     "  Bond properties should be re-calculated, if required bond cutoff should be updated.",
                     "** Keep the ratios between the lattice vectors A, B and C like in the initial cell."};
  for (i=0; i<2; i++) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(info[i], -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
  show_the_widgets (layout);
  return layout;
}
