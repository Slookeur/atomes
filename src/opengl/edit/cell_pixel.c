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
* @file cell_pixel.c
* @short Functions to create the pixel debugging tab for the cell edition window \n
         This is only used when DEBUG option is activated at build time
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cell_pixel.c'
*
* Contains:
*

 - The functions to create the pixel debugging tab for the cell edition window
   This is only used when DEBUG option is activated at build time

*
* List of functions:

  void pix_info_ (int * na, int * nb, int * nc);
  void send_pix_info_ (int * p, int listp[27], int * ngb);
  void update_pix_table (project * this_proj);

  G_MODULE_EXPORT void set_pix (GtkEntry * res, gpointer data);

  GtkWidget * create_css_label (gchar * str, int id);
  GtkWidget * attach_grid (project * this_proj, int init);
  GtkWidget * pixels_tab (project * this_proj);

*/

#ifdef DEBUG

#include "cell_edit.h"

/*!
  \fn void pix_info_ (int * na, int * nb, int * nc)

  \brief allocate pixel data pointer from Fortran90

  \param na x size
  \param nb y size
  \param nc z size
*/
void pix_info_ (int * na, int * nb, int * nc)
{
  active_project -> pix[0] = *na;
  active_project -> pix[1] = *nb;
  active_project -> pix[2] = *nc;
  active_project -> pixels = allocdint (active_project -> pix[0]*active_project -> pix[1]*active_project -> pix[2],27);
}

/*!
  \fn void send_pix_info_ (int * p, int listp[27], int * ngb)

  \brief retrieve pixel information from Fortran90

  \param p the pixel id
  \param listp the neighbor list for pixel id
  \param ngb the number of neighbor for pixel id
*/
void send_pix_info_ (int * p, int listp[27], int * ngb)
{
  int i;
  for (i=0; i<* ngb; i++)
  {
    active_project -> pixels[* p][i] = listp[i];
  }
}

/*!
  \fn GtkWidget * create_css_label (gchar * str, int id)

  \brief create CSS label

  \param str the string
  \param id 0 = white, 1 = yellow
*/
GtkWidget * create_css_label (gchar * str, int id)
{
  gchar * colo[2] = {"white", "yellow"};
  gchar * backcol;
  if (id)
  {
    backcol = g_strdup_printf ("label#icolor {\n"
                               "  background-color: red;\n"
                               "  color: %s;\n}", colo[id]);
  }
  else
  {
    backcol = g_strdup_printf ("label#color {\n"
                               "  background-color: blue;\n"
                               "  color: %s;\n}", colo[id]);

  }
  provide_gtk_css (backcol);
  g_free (backcol);
  GtkWidget * lab = markup_label(str, 50, 50, 0.5, 0.5);
  gtk_widget_set_name (lab, (id) ? "icolor" : "color");
  show_the_widgets (lab);
  return lab;
}

/*!
  \fn GtkWidget * attach_grid (project * this_proj, int init)

  \brief create pixel grid

  \param this_proj the target project
  \param init the target pixel
*/
GtkWidget * attach_grid (project * this_proj, int init)
{
  GtkWidget * table = gtk_grid_new ();
  gchar * str;
  gboolean actif;
  int i, j, k, l, m;
  l = init+1;
  for (i=0; i<this_proj -> pix[1]; i++)
  {
    for (j=0; j<this_proj -> pix[0]; j++)
    {
      actif = FALSE;
      for (k=0; k<27; k++)
      {
        if (l == this_proj -> pixels[this_proj -> actif_pix-1][k])
        {
          actif = TRUE;
        }
      }
      if (l == this_proj -> actif_pix)
      {
        str = g_strdup_printf ("<b>%d</b>", l);
        m = 1;
      }
      else
      {
        str = g_strdup_printf ("%d", l);
        m = 0;
      }
      l ++;
      if (actif)
      {
        gtk_grid_attach (GTK_GRID (table), create_css_label(str, m), j, i, 1, 1);
      }
      else
      {
        gtk_grid_attach (GTK_GRID (table), markup_label(str, 50, 50, 0.5, 0.5), j, i, 1, 1);
      }
      g_free (str);
    }
  }
  gtk_widget_set_size_request (table, this_proj -> pix[0]*50, this_proj -> pix[1]*50);
  return table;
}

/*!
  \fn void update_pix_table (project * this_proj)

  \brief prepare pixel table widget

  \param this_proj the target project
*/
void update_pix_table (project * this_proj)
{
  int i;
  int pix[3];

  for (i=0; i<3; i++)
  {
    this_proj -> pix_tab[i] = destroy_this_widget (this_proj -> pix_tab[i]);
  }

  // Seed for bottom grid
  if (this_proj -> actif_pix <= this_proj -> pix[0]*this_proj -> pix[1])
  {
    pix[0] = this_proj -> pix[0]*this_proj -> pix[1]*(this_proj -> pix[2]-1);
  }
  else
  {
    pix[0] = (this_proj -> actif_pix-1)/(this_proj -> pix[0]*this_proj -> pix[1]);
    pix[0] -= 1;
    pix[0] *= (this_proj -> pix[0]*this_proj -> pix[1]);
  }

  // Seed for pixel grid
  pix[1] = (this_proj -> actif_pix-1)/(this_proj -> pix[0]*this_proj -> pix[1]);
  pix[1] *= (this_proj -> pix[0]*this_proj -> pix[1]);

  // Seed for top grid
  if (this_proj -> actif_pix > this_proj -> pix[0]*this_proj -> pix[1]*(this_proj -> pix[2]-1))
  {
    pix[2] = 0;
  }
  else
  {
    pix[2] = (this_proj -> actif_pix-1)/(this_proj -> pix[0]*this_proj -> pix[1]);
    pix[2] += 1;
    pix[2] *= (this_proj -> pix[0]*this_proj -> pix[1]);
  }
  for (i=0; i<3; i++)
  {
    this_proj -> pix_tab[i] = attach_grid (this_proj, pix[i]);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> pix_box, this_proj -> pix_tab[i], FALSE, FALSE, 20);
  }
  show_the_widgets (this_proj -> pix_box);
}

/*!
  \fn G_MODULE_EXPORT void set_pix (GtkEntry * res, gpointer data)

  \brief set pixel id callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_pix (GtkEntry * res, gpointer data)
{
  int id = GPOINTER_TO_INT (data);
  project * this_proj = get_project_by_id (id);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  int p = (int)v;
  int n = this_proj-> pix[0] * this_proj -> pix[1] * this_proj -> pix[2];
  if (p > 0 && p <= n)
  {
    this_proj -> actif_pix = p;
  }
  update_entry_int (res, this_proj -> actif_pix);
  update_pix_table (this_proj);
}

/*!
  \fn GtkWidget * pixels_tab (project * this_proj)

  \brief create the PBC pixels checking tab

  \param this_proj the target project
*/
GtkWidget * pixels_tab (project * this_proj)
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox = create_hbox (0);
  gchar * str = g_strdup_printf (_("\tSelect pixel to check, from 1 to %d:"), this_proj -> pix[0]*this_proj -> pix[1]*this_proj->pix[2]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, create_entry(G_CALLBACK(set_pix), 100, 15, FALSE, GINT_TO_POINTER(this_proj -> id)), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  GtkWidget * scroll = create_scroll (vbox, 700, 350, GTK_SHADOW_ETCHED_IN);
  this_proj -> actif_pix = 1;
  this_proj -> pix_box = create_vbox (0);
  this_proj -> pix_tab[0] = this_proj -> pix_tab[1] = this_proj -> pix_tab[2] = NULL;
  update_pix_table (this_proj);
  add_container_child (CONTAINER_SCR, scroll, this_proj -> pix_box);
  return vbox;
}

#endif // DEBUG
