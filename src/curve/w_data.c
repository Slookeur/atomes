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
* @file w_data.c
* @short Functions to export curve data
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_data.c'
*
* Contains:
*

 - The functions to export curve data

*
* List of functions:

  void write_curve (gpointer idata);

  G_MODULE_EXPORT void run_write_curve (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_write_curve (GtkDialog * info, gint response_id, gpointer data);

*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "interface.h"
#include "curve.h"

void save_to_file_ (int *, char *, int *, double *, double *, int *, int *, int *, double *, int *, int *, int *, int *, char *);
void prep_file_ (int *, char *, int *, int *, int *, double *, int *, int *);
void append_to_file_ (int *, double *, double *, double *, int *, int *, int *, int *, int *, int *, int *, char *);

GtkFileFilter * filter1, * filter2;

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_write_curve (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief save curve data - running the dialog GTK4

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_write_curve (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_write_curve (GtkDialog * info, gint response_id, gpointer data)

  \brief save curve data - running the dialog GTK3

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_write_curve (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  int a, b, c, i, j, k, l, m, n, o, p, q;
  tint * cd = (tint *)data;
  Curve * this_curve = get_curve_from_pointer (data);
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  double delta = get_project_by_id(a) -> analysis[b] -> delta;
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    project * this_proj = get_project_by_id(a);
    this_curve -> cfile = file_chooser_get_file_name (chooser);
    GtkFileFilter * tmp = gtk_file_chooser_get_filter (chooser);
    if (tmp == filter1)
    {
      l = 0;
    }
    else if (tmp == filter2)
    {
      l = 1;
    }
    k = strlen (this_curve -> cfile);
    j = strlen (this_curve -> title);
    if (this_curve -> extrac -> extras == 0)
    {
      save_to_file_ (& k, this_curve -> cfile,
                     & this_curve -> ndata,
                     this_curve -> data[0],
                     this_curve -> data[1],
                     & this_curve -> scale[0],
                     & this_curve -> scale[1],
                     & this_curve -> layout -> aspect,
                     & delta, & b, & c, & l,
                     & j, this_curve -> title);

    }
    else
    {
      p = this_curve -> extrac -> extras + 1;
      m = 0;
      prep_file_ (& k, this_curve -> cfile, & l,
                  & this_curve -> scale[0],
                  & this_curve -> scale[1],
                  & delta, & b, & c);
      append_to_file_ (& this_curve -> ndata,
                       this_curve -> data[0],
                       this_curve -> data[1],
                       & delta,
                       & this_curve -> layout -> aspect,
                       & b, & c, & l, & m, & p,
                       & j, this_curve -> title);
      j = this_curve -> extrac -> extras;
      CurveExtra * ctmp = this_curve -> extrac -> first;
      for (i=0 ; i < j ; i++ )
      {
        m = m + 1;
        q = ctmp -> id.a;
        o = ctmp -> id.b;
        n = ctmp -> id.c;
        this_proj = get_project_by_id(q);
        j = strlen (this_proj -> analysis[o] -> curves[n] -> title);
        append_to_file_ (& this_proj -> analysis[o] -> curves[n] -> ndata,
                         this_proj -> analysis[o] -> curves[n] -> data[0],
                         this_proj -> analysis[o] -> curves[n] -> data[1],
                         & this_proj -> analysis[o] -> delta,
                         & ctmp -> layout -> aspect,
                         & o, & n, & l, & m, & p,
                         & j, this_proj -> analysis[o] -> curves[n] -> title);
        if (ctmp -> next) ctmp = ctmp -> next;
      }
    }
  }
#ifdef GTK4
  destroy_this_native_dialog (info);
#else
  destroy_this_dialog (info);
#endif
}

/*!
  \fn void write_curve (gpointer idata)

  \brief save curve data - creating the dialog

  \param idata the associated data pointer
*/
void write_curve (gpointer idata)
{
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  Curve * this_curve = get_curve_from_pointer (idata);
  info = create_file_chooser ("Save Data",
                              GTK_WINDOW(this_curve -> window),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              "Save");
  GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
#ifdef GTK3
  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
  filter1 = gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter1), "Xmgrace file (*.agr)");
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter1), "*.agr");
  gtk_file_chooser_add_filter (chooser, filter1);
  filter2 = gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter2), "x/y ASCII file (*.dat)");
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter2), "*.dat");
  gtk_file_chooser_add_filter (chooser, filter2);
  if (this_curve -> cfile != NULL)
  {
    if (! file_chooser_set_file_name (chooser, this_curve -> cfile)) goto end;
  }
  else
  {
    file_chooser_set_current_folder (chooser);
    gtk_file_chooser_set_current_name (chooser, this_curve -> name);
  }
#ifdef GTK4
  run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_write_curve), idata);
#else
  run_this_gtk_dialog (info, G_CALLBACK(run_write_curve), idata);
#endif
  end:;
}
