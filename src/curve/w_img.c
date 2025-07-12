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
* @file w_img.c
* @short Functions to save screenshot from graph / curve(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_img.c'
*
* Contains:
*

 - The functions to save screenshot from graph / curve(s)

*
* List of functions:

  void write_image (gpointer curetow);
  void save_image (gpointer cdata);

  G_MODULE_EXPORT void set_size (GtkEntry * val, gpointer data);
  G_MODULE_EXPORT void set_background (GtkCheckButton * backb);
  G_MODULE_EXPORT void set_background (GtkToggleButton * backb);
  G_MODULE_EXPORT void choose_format (GtkComboBox * box, gpointer cid);
  G_MODULE_EXPORT void run_write_image (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_write_image (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_save_image (GtkDialog * save_img, gint response_id, gpointer data);

*/

#include <gtk/gtk.h>
#include <stdlib.h>

#include "global.h"
#include "interface.h"
#include "curve.h"

typedef struct {
  GtkWidget * a;
  GtkWidget * b;
} dwidget;

double back_alpha;
dwidget thedata;
int forme;

/*!
  \fn G_MODULE_EXPORT void set_size (GtkEntry * val, gpointer data)

  \brief set image size entry callback

  \param val the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_size (GtkEntry * val, gpointer data)
{
  const gchar * m;
  char * text[2];
  int i, j;
  j = GPOINTER_TO_INT(data);
  text[0] = "X size must be > 0";
  text[1] = "Y size must be > 0";
  m = entry_get_text (val);
  i = string_to_double ((gpointer)m);
  if (i > 0)
  {
    resol[j] = i;
  }
  else
  {
    show_warning (text[j], NULL);
  }
  update_entry_int (val, resol[j]);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_background (GtkCheckButton * backb, gpointer data)

  \brief show / hide image background toggle callback GTK4

  \param backb the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_background (GtkCheckButton * backb, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_background (GtkToggleButton * backb, gpointer data)

  \brief show / hide image background toggle callback GTK3

  \param backb the GtkTogglekButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_background (GtkToggleButton * backb, gpointer data)
#endif
{
  if (button_get_status ((GtkWidget *)backb))
  {
    back_alpha = 0.0;
  }
  else
  {
    back_alpha = 1.0;
  }
}

/*!
  \fn G_MODULE_EXPORT void choose_format (GtkComboBox * box, gpointer cid)

  \brief change image format

  \param box the GtkComboBox sending the signal
  \param cid the associated data pointer
*/
G_MODULE_EXPORT void choose_format (GtkComboBox * box, gpointer cid)
{
  forme = combo_get_active ((GtkWidget *)box);
  if (forme != -1)
  {
    dwidget * imf  = (dwidget *) cid;
    button_set_status (imf -> a, 0);
    if (forme == 1 || forme == 3)
    {
      widget_set_sensitive(imf -> a, 0);
    }
    else
    {
      widget_set_sensitive(imf -> a, 1);
    }
    switch (forme)
    {
      case 0:
        gtk_image_set_from_file (GTK_IMAGE(imf -> b), PACKAGE_PNG);
        break;
      case 1:
        gtk_image_set_from_file (GTK_IMAGE(imf -> b), PACKAGE_PDF);
        break;
      case 2:
        gtk_image_set_from_file (GTK_IMAGE(imf -> b), PACKAGE_SVG);
        break;
      case 3:
        gtk_image_set_from_file (GTK_IMAGE(imf -> b), PACKAGE_EPS);
        break;
    }
  }
}

gchar * i_title[4]={"Portable Network Graphics",
                    "Portable Document File",
                    "Scalable Vector Graphics",
                    "Encapsulated Post-Script Image"};
gchar * i_pattern[4]={"*.png",
                      "*.pdf",
                      "*.svg",
                      "*.eps"};

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_write_image (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief write image - running the dialog GTK4

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_write_image (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_write_image (GtkDialog * info, gint response_id, gpointer data)

  \brief write image - running the dialog GTK3

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_write_image (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  tint * cd = (tint *)data;
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    curve_image_file = file_chooser_get_file_name (chooser);
    project * this_proj = get_project_by_id (cd -> a);
    this_proj -> curves[cd -> b][cd -> c] -> format = forme + 1;
#ifdef GTK3
    show_curve (NULL, NULL, data);
#else
    GdkSurface * surf = gtk_native_get_surface ((GtkNative *)this_proj -> curves[cd -> b][cd -> c] -> plot);
    cairo_t * rec = (cairo_t *)gdk_cairo_context_cairo_create (gdk_surface_create_cairo_context (surf));
    show_curve (NULL, rec, resol[0], resol[1], data);
#endif
  }
#ifdef GTK4
  destroy_this_native_dialog (info);
#else
  destroy_this_dialog (info);
#endif
}

/*!
  \fn void write_image (gpointer curetow)

  \brief write image - creating the file chooser dialog

  \param curetow the associated data pointer
*/
void write_image (gpointer curetow)
{
  int a, b, c;
  tint * cd = (tint *)curetow;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  gchar * i_filter[4]={"PNG file (*.png)",
                       "PDF file (*.pdf)",
                       "SVG file (*.svg)",
                       "EPS file (*.eps)"};
  GtkFileFilter * filter;
  project * this_proj = get_project_by_id(a);
  if (forme == -1)
  {
    show_warning ("To save an image please enter a file format", this_proj -> curves[b][c] -> window);
  }
  else if (resol[0] == 0 || resol[1] == 0)
  {
    show_warning ("You need to specify the size of the image", this_proj -> curves[b][c] -> window);
  }
  else
  {
#ifdef GTK4
    GtkFileChooserNative * info;
#else
    GtkWidget * info;
#endif
    info = create_file_chooser (i_title[forme],
                                GTK_WINDOW(this_proj -> curves[b][c] -> window),
     				            GTK_FILE_CHOOSER_ACTION_SAVE,
                                "Save");
    GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
#ifdef GTK3
    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(GTK_FILE_FILTER(filter), i_filter[forme]);
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), i_pattern[forme]);
    gtk_file_chooser_add_filter (chooser, filter);
#ifdef GTK4
    run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_write_image), curetow);
#else
    run_this_gtk_dialog (info, G_CALLBACK(run_write_image), curetow);
#endif
  }
}

/*!
  \fn G_MODULE_EXPORT void run_save_image (GtkDialog * save_img, gint response_id, gpointer data)

  \brief export curve window plot to image - running the dialog

  \param save_img the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_save_image (GtkDialog * save_img, gint response_id, gpointer data)
{
  gboolean done = FALSE;
  while (! done)
  {
    switch (response_id)
    {
      case GTK_RESPONSE_APPLY:
        if (forme > -1)
        {
          write_image (data);
          done = TRUE;
        }
        else
        {
          done = FALSE;
        }
        break;
      default:
        done = TRUE;
        break;
    }
  }
  if (done) destroy_this_dialog (save_img);
}

/*!
  \fn void save_image (gpointer cdata)

  \brief export curve window plot to image - creating the dialog

  \param cdata the associated data pointer
*/
void save_image (gpointer cdata)
{
  GtkWidget * save_img;
  GtkWidget * hbox;
  GtkWidget * comboformat;
  GtkWidget * x;
  GtkWidget * y;
  GtkWidget * background;
  GtkWidget * log;
  int xlgt, ylgt;
  int i;
  gchar * str;
  int a, b, c;
  tint * cd = (tint *)cdata;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  project * this_proj = get_project_by_id(a);
  save_img = dialog_cancel_apply ("Export image", this_proj -> curves[b][c] -> window, FALSE);
  gtk_window_set_resizable (GTK_WINDOW (save_img), FALSE);
#ifndef GTK4
  gtk_window_set_icon (GTK_WINDOW (save_img), THETD);
#endif
  GtkWidget * vbox = dialog_get_content_area (save_img);
  gtk_box_set_spacing (GTK_BOX(vbox), 15);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, TRUE, TRUE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("Format: ", -1, 50, 0.0, 0.5), TRUE, TRUE, 0);
  GtkWidget * fixed = gtk_fixed_new ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, TRUE, TRUE, 10);
  comboformat = create_combo ();
  for (i=0; i<4; i++)
  {
    str = g_strdup_printf ("%s - (%s)", i_title[i], i_pattern[i]);
    combo_text_append (comboformat, str);
    g_free (str);
  }
  gtk_fixed_put (GTK_FIXED(fixed), comboformat, -1, 10);
  gtk_widget_set_size_request (comboformat, -1, 30);

  log = gtk_image_new_from_file (PACKAGE_VOID);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, log, TRUE, TRUE, 0);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, TRUE, TRUE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Size: ", 100, -1, 0.0, 0.5), FALSE, TRUE, 0);

  xlgt = get_widget_width (this_proj -> curves[b][c] -> plot);
  ylgt = get_widget_height (this_proj -> curves[b][c] -> plot);
  x = create_entry (G_CALLBACK(set_size), 50, 15, FALSE, GINT_TO_POINTER(0));
  update_entry_int (GTK_ENTRY(x), xlgt);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, x, TRUE, TRUE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new ("x"), TRUE, TRUE, 5);
  y = create_entry (G_CALLBACK(set_size), 50, 15, FALSE, GINT_TO_POINTER(1));
  update_entry_int (GTK_ENTRY(y), ylgt);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, y, TRUE, TRUE, 0);

  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new ("pixels"), TRUE, TRUE, 0);

  background = check_button ("Make background transparent", -1, -1, FALSE, G_CALLBACK(set_background), NULL);
  widget_set_sensitive (background, FALSE);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, background, TRUE, TRUE, 0);

  forme = -1;
  back_alpha = 1.0;
  thedata.a = background;
  thedata.b = log;
  g_signal_connect (G_OBJECT(comboformat), "changed", G_CALLBACK(choose_format), & thedata);

  run_this_gtk_dialog (save_img, G_CALLBACK(run_save_image), cdata);
}
