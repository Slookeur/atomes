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
* @file w_record.c
* @short Functions to create the recording window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_record.c'
*
* Contains:
*

 - The functions to create the recording window

*
* List of functions:

  void prepare_edition_windows (glwin * view, int status);

  G_MODULE_EXPORT void rec_start (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void rec_stop (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void window_recorder (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "glview.h"

extern void window_encode (glwin * view, gboolean video);

/*!
  \fn void prepare_edition_windows (glwin * view, int status)

  \brief update the sensitivity of the edition window(s) if any are opened

  \param view the target glwin
  \param status sensitivity
*/
void prepare_edition_windows (glwin * view, int status)
{
  GtkWidget * widg;
  int i;
  if (view -> atom_win != NULL)
  {
    if (view -> atom_win -> notebook != NULL)
    {
      if (GTK_IS_WIDGET(view -> atom_win -> notebook))
      {
        for (i=0; i<5; i++)
        {
          widg = gtk_notebook_get_nth_page (GTK_NOTEBOOK (view -> atom_win -> notebook), i);
          widget_set_sensitive (widg, status);
        }
      }
    }
  }
  if (view -> cell_win != NULL)
  {
    if (view -> cell_win -> notebook != NULL)
    {
      if (GTK_IS_WIDGET(view -> cell_win -> notebook))
      {
        widg = gtk_notebook_get_nth_page (GTK_NOTEBOOK (view -> cell_win -> notebook), 0);
        widget_set_sensitive (widg, status);
        widg = gtk_notebook_get_nth_page (GTK_NOTEBOOK (view -> cell_win -> notebook), 1);
        widget_set_sensitive (widg, status);
        widg = gtk_notebook_get_nth_page (GTK_NOTEBOOK (view -> cell_win -> notebook), 3);
        widget_set_sensitive (widg, status);
        widg = gtk_notebook_get_nth_page (GTK_NOTEBOOK (view -> cell_win -> notebook), 4);
        widget_set_sensitive (widg, status);
        widg = gtk_notebook_get_nth_page (GTK_NOTEBOOK (view -> cell_win -> notebook), 5);
        widget_set_sensitive (widg, status);
      }
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void rec_start (GtkButton * but, gpointer data)

  \brief start recording

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void rec_start (GtkButton * but, gpointer data)
{
  glwin * view = (glwin *) data;
  if (! view -> record)
  {
    button_set_image (GTK_BUTTON(view -> rec -> rec), NULL, IMG_STOCK, RECORD);
    show_the_widgets (view -> rec -> rec);
    widget_set_sensitive (view -> rec -> stop, 1);
#ifdef GTK3
    // GTK3 Menu Action To Check
    widget_set_sensitive (view -> ogl_mode[1], 0);
#endif
    //gtk_widget_set_size_request (view -> plot, view -> pixels[0], view -> pixels[1]);
    //gtk_window_set_resizable (GTK_WINDOW (view -> win), FALSE);
    prepare_edition_windows (view, 0);
    view -> record = TRUE;
  }
}

/*!
  \fn G_MODULE_EXPORT void rec_stop (GtkButton * but, gpointer data)

  \brief stop recording

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void rec_stop (GtkButton * but, gpointer data)
{
  glwin * view = (glwin *) data;
  view -> record = FALSE;
  button_set_image (GTK_BUTTON(view -> rec -> rec), NULL, IMG_STOCK, YES);
  show_the_widgets (view -> rec -> rec);
  window_encode (view, TRUE);
  widget_set_sensitive (view -> rec -> stop, 0);
#ifdef GTK3
  // GTK3 Menu Action To Check
  widget_set_sensitive (view -> ogl_mode[1], 1);
#endif
  gtk_window_set_resizable (GTK_WINDOW (view -> win), TRUE);
  prepare_edition_windows (view, 1);
}

/*!
  \fn G_MODULE_EXPORT void window_recorder (GtkWidget * widg, gpointer data)

  \brief create video recorder window

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void window_recorder (GtkWidget * widg, gpointer data)
{
  glwin * view = (glwin *) data;
  if (view -> rec == NULL)
  {
    view -> rec = g_malloc0 (sizeof*view -> rec);
    gchar * str = g_strdup_printf ("%s - record", get_project_by_id(view -> proj) -> name);
    view -> rec -> win = create_win (str, view -> win, FALSE, FALSE);
    g_free (str);
    GtkWidget * hbox = create_hbox (0);
    add_container_child (CONTAINER_WIN, view -> rec -> win, hbox);
    // Record
    view -> rec -> rec = create_button ("Record", IMG_STOCK, YES, -1, -1, GTK_RELIEF_NONE, G_CALLBACK(rec_start), data);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, view -> rec -> rec, TRUE, TRUE, 0);
    // Stop
    view -> rec -> stop = create_button ("Stop", IMG_STOCK, MEDIA_STOP, -1, -1, GTK_RELIEF_NONE, G_CALLBACK(rec_stop), data);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, view -> rec -> stop, TRUE, TRUE, 0);
    widget_set_sensitive (view -> rec -> stop, view -> record);
    add_gtk_close_event (view -> rec -> win, G_CALLBACK(hide_this_window), NULL);
    show_the_widgets (view -> rec -> win);
  }
  else
  {
    show_the_widgets (view -> rec -> win);
  }
}
