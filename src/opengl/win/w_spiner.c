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
* @file w_spiner.c
* @short Functions to create the 'Spin' window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_spiner.c'
*
* Contains:
*

 - The functions to create the 'Spin' window

*
* List of functions:

  gboolean spin (gpointer data);

  static gboolean spin_wait_for_stop (gpointer data);

  G_MODULE_EXPORT void spin_go (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void spin_stop (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void window_spinner (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "glview.h"

extern void save_rotation_quaternion (glwin * view);
extern void rotate_x_y (glwin * view, double angle_x, double angle_y);

/*!
  \fn gboolean spin (gpointer data)

  \brief spin

  \param data the associated data pointer
*/
gboolean spin (gpointer data)
{
  tint * val = (tint *) data;
  project * this_proj = get_project_by_id(val -> a);
#ifdef DEBUG
//  g_debug (":: SPIN:: a= %d, c= %d", val -> a, val -> c);
//  g_debug (":: SPIN:: speed[c]= %d", this_proj -> modelgl -> spin_speed[val -> c]);
#endif
  // The next 2 tests are required if a spinning project is closed
  // For some reason even after ending the spin the signal is not
  // terminated before deleting the project's glwin.
  if (this_proj)
  {
    if (this_proj -> modelgl)
    {
      if (this_proj -> modelgl -> spin[val -> c])
      {
        save_rotation_quaternion (this_proj -> modelgl);
        double cameraAngle[2] = {0.0, 0.0};
        cameraAngle[val -> c] = 0.1 * this_proj -> modelgl -> spin_speed[val -> c];
        rotate_x_y (this_proj -> modelgl, cameraAngle[0], cameraAngle[1]);
        update (this_proj -> modelgl);
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn static gboolean spin_wait_for_stop (gpointer data)

  \brief spin and wait for stop

  \param data the associated data pointer
*/
static gboolean spin_wait_for_stop (gpointer data)
{
  tint * val = (tint *) data;
  if (get_project_by_id(val -> a) -> modelgl -> spin[val -> c])
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn G_MODULE_EXPORT void spin_go (GtkButton * but, gpointer data)

  \brief start spin

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void spin_go (GtkButton * but, gpointer data)
{
  tint * val = (tint *) data;
  glwin * view = get_project_by_id(val -> a) -> modelgl;

  int s = val -> b - 2;
#ifdef DEBUG
//  g_debug (":: SPIN_GO:: a= %d, b= %d, c= %d, d=%d", val -> a, val -> b, val -> c, s);
//  g_debug (":: SPIN_GO:: speed[b]= %d", view -> spin_speed[val -> c]);
#endif
  view -> spin_speed[val -> c] += s;
  if (view -> spin_speed[val -> c] == 0)
  {
    view -> spin[val -> c] = FALSE;
    g_timeout_add (REFRESH, (GSourceFunc) spin_wait_for_stop, data);
  }
  else if (! view -> spin[val -> c])
  {
    view -> spin[val -> c] = TRUE;
    g_timeout_add (REFRESH, (GSourceFunc) spin, data);
  }
}

/*!
  \fn G_MODULE_EXPORT void spin_stop (GtkButton * but, gpointer data)

  \brief stop spin

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void spin_stop (GtkButton * but, gpointer data)
{
  glwin * view = (glwin *) data;
  int i;
  for (i=0; i<2; i++)
  {
    view -> spin[i] = FALSE;
    view -> spin_speed[i] = 0;
  }
  //set_sensitive_coord_menu (view, TRUE);
}

/*!
  \fn G_MODULE_EXPORT void window_spinner (GtkWidget * widg, gpointer data)

  \brief create the spin window callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void window_spinner (GtkWidget * widg, gpointer data)
{
  glwin * view = (glwin *) data;
  if (view -> spiner == NULL)
  {
    view -> spiner = g_malloc0 (sizeof*view -> spiner);
    gchar * str = g_strdup_printf ("%s - spin", prepare_for_title(get_project_by_id(view -> proj) -> name));
    view -> spiner -> win = create_win (str, view -> win, FALSE, FALSE);
    g_free (str);
    GtkWidget * table = gtk_grid_new ();
    add_container_child (CONTAINER_WIN, view -> spiner -> win, table);
    view -> spiner -> right = create_button ("Right", IMG_STOCK, GO_RIGHT, -1, -1, GTK_RELIEF_NONE, G_CALLBACK(spin_go), & view -> colorp[1][1]);
    view -> spiner -> left = create_button ("Left", IMG_STOCK, GO_LEFT, -1, -1, GTK_RELIEF_NONE, G_CALLBACK(spin_go), & view -> colorp[3][1]);
    view -> spiner -> stop = create_button ("Stop", IMG_STOCK, MEDIA_STOP, -1, -1, GTK_RELIEF_NONE, G_CALLBACK(spin_stop), view);
    view -> spiner -> up = create_button ("Up", IMG_STOCK, GO_UP, -1, -1, GTK_RELIEF_NONE, G_CALLBACK(spin_go), & view -> colorp[3][0]);
    view -> spiner -> down = create_button ("Down", IMG_STOCK, GO_DOWN, -1, -1, GTK_RELIEF_NONE, G_CALLBACK(spin_go), & view -> colorp[1][0]);
    gtk_grid_attach (GTK_GRID (table), view -> spiner -> right, 2,1,1,1);
    gtk_grid_attach (GTK_GRID (table), view -> spiner -> left, 0,1,1,1);
    gtk_grid_attach (GTK_GRID (table), view -> spiner -> stop, 1,1,1,1);
    gtk_grid_attach (GTK_GRID (table), view -> spiner -> up, 1,0,1,1);
    gtk_grid_attach (GTK_GRID (table), view -> spiner -> down, 1,2,1,1);
    add_gtk_close_event (view -> spiner -> win, G_CALLBACK(hide_this_window), NULL);
    show_the_widgets (view -> spiner -> win);
    gtk_window_set_resizable (GTK_WINDOW(view -> spiner -> win), FALSE);
  }
  else
  {
    show_the_widgets (view -> spiner -> win);
  }
}
