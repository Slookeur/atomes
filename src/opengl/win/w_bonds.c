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
* @file w_bonds.c
* @short Functions to create the bond (legnth/radius) edition window(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_bonds.c'
*
* Contains:
*

 - The functions to create the bond (legnth/radius) edition window(s)

*
* List of functions:

  void bonds_input_win (GtkWidget * win, project * this_proj, int nspec, int aoc, double ** val)

  G_MODULE_EXPORT void update_bond_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_bond_parameter (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void set_bond_parameter (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void window_bonds (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void window_bonds (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"
#include "color_box.h"

extern gchar * label_cutrab (project * this_proj, glwin * view, int id);

/*!
  \fn G_MODULE_EXPORT void update_bond_parameter (GtkEntry * res, gpointer data)

  \brief update bond parameter callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_bond_parameter (GtkEntry * res, gpointer data)
{
  int id = GPOINTER_TO_INT(data);
  const gchar * n = entry_get_text (res);
  double v = string_to_double ((gpointer)n);
  int j, k, l, m;
  int a, b, s;
  s =  opengl_project -> nspec;
#ifdef GTK3
  // GTK3 Menu Action To Check
  int c;
  gchar * str;
#endif
  switch (id)
  {
    case -5:
      if (v > 0.0) opengl_project -> modelgl -> anim -> last -> img -> xyz -> length = v;
      v = opengl_project -> modelgl -> anim -> last -> img -> xyz -> length;
#ifdef GTK3
      // GTK3 Menu Action To Check
      str = g_strdup_printf ("_Axis length [ %f Å ]", v);
      gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_box_axis[1][7]), str);
      g_free (str);
#endif
      if (opengl_project -> modelgl -> axis_win -> length)
      {
        update_entry_double (GTK_ENTRY(opengl_project -> modelgl -> axis_win -> length), v);
      }
      opengl_project -> modelgl -> create_shaders[MAXIS] = TRUE;
      break;
    case -4:
      if (opengl_project -> modelgl -> anim -> last -> img -> xyz -> axis == CYLINDERS)
      {
        if (v > 0.0) opengl_project -> modelgl -> anim -> last -> img -> xyz -> rad = v;
        v = opengl_project -> modelgl -> anim -> last -> img -> xyz -> rad;
#ifdef GTK3
        // GTK3 Menu Action To Check
        str = g_strdup_printf ("_Radius [ %f Å ]", v);
        gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_box_axis[1][6]), str);
        g_free (str);
#endif
        if (opengl_project -> modelgl -> axis_win)
        {
          if (opengl_project -> modelgl -> axis_win -> radius && GTK_IS_WIDGET(opengl_project -> modelgl -> axis_win -> radius))
          {
            update_entry_double (GTK_ENTRY(opengl_project -> modelgl -> axis_win -> radius), v);
          }
        }
      }
      else
      {
        if (v > 0.0) opengl_project -> modelgl -> anim -> last -> img -> xyz -> line = v;
        v = opengl_project -> modelgl -> anim -> last -> img -> xyz -> line;
#ifdef GTK3
        str = g_strdup_printf ("_Width [ %f pts ]", v);
        gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_box_axis[1][4]), str);
        g_free (str);
#endif
      }
      if (opengl_project -> modelgl -> axis_win)
      {
        if (opengl_project -> modelgl -> axis_win -> width && GTK_IS_WIDGET(opengl_project -> modelgl -> axis_win -> width))
        {
          update_entry_double (GTK_ENTRY(opengl_project -> modelgl -> axis_win -> width), v);
        }
      }
      opengl_project -> modelgl -> create_shaders[MAXIS] = TRUE;
      break;
    case -3:
      if (opengl_project -> modelgl -> anim -> last -> img -> abc -> box == CYLINDERS)
      {
        if (v > 0.0) opengl_project -> modelgl -> anim -> last -> img -> abc -> rad = v;
        v = opengl_project -> modelgl -> anim -> last -> img -> abc -> rad;
#ifdef GTK3
        // GTK3 Menu Action To Check
        str = g_strdup_printf ("_Radius [ %f Å ]", v);
        gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_box_axis[0][6]), str);
        g_free (str);
#endif
        if (opengl_project -> modelgl -> box_win)
        {
          if (opengl_project -> modelgl -> box_win -> radius && GTK_IS_WIDGET(opengl_project -> modelgl -> box_win -> radius))
          {
            update_entry_double (GTK_ENTRY(opengl_project -> modelgl -> box_win -> radius), v);
          }
        }
      }
      else
      {
        if (v > 0.0) opengl_project -> modelgl -> anim -> last -> img -> abc -> line = v;
        v = opengl_project -> modelgl -> anim -> last -> img -> abc -> line;
#ifdef GTK3
        // GTK3 Menu Action To Check
        str = g_strdup_printf ("_Width [ %f pts ]", v);
        gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_box_axis[0][4]), str);
        g_free (str);
#endif
        if (opengl_project -> modelgl -> box_win)
        {
          if (opengl_project -> modelgl -> box_win -> width && GTK_IS_WIDGET(opengl_project -> modelgl -> box_win -> width))
          {
            update_entry_double (GTK_ENTRY(opengl_project -> modelgl -> box_win -> width), v);
          }
        }
      }
      opengl_project -> modelgl -> create_shaders[MDBOX] = TRUE;
      break;
    case -2:
      if (v > 0.0)
      {
        opengl_project -> modelgl -> anim -> last -> img -> radall[1] = v;
      }
      v = opengl_project -> modelgl -> anim -> last -> img -> radall[1];
#ifdef GTK3
      // GTK3 Menu Action To Check
      str = g_strdup_printf ("Cylinder(s) [ %f Å ]", v);
      gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_bonds[8]), str);
      g_free (str);
#endif
      init_default_shaders (opengl_project -> modelgl);
      break;
    case -1:
      if (v > 0.0)
      {
        opengl_project -> modelgl -> anim -> last -> img -> radall[0] = v;
      }
      v = opengl_project -> modelgl -> anim -> last -> img -> radall[0];
#ifdef GTK3
      // GTK3 Menu Action To Check
      str = g_strdup_printf ("Cylinder(s) [ %f Å ]", v);
      gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_bonds[1]), str);
      g_free (str);
#endif
      init_default_shaders (opengl_project -> modelgl);
      break;
    default:
      k = s*(s + 1)/2;
      if (id < k)
      {
        j = id;
        m = 0;
#ifdef GTK3
        // GTK3 Menu Action To Check
        c = 0;
#endif
      }
      else
      {
        j = id - k;
        m = s;
#ifdef GTK3
        // GTK3 Menu Action To Check
        c = 1;
#endif
      }
      if (j < s)
      {
        a = j + m;
        b = j + m;
      }
      else
      {
        j -= s;
        for (k=0; k<s-1; k++)
        {
          for (l=k+1; l<s; l++)
          {
            if (j == 0)
            {
              a = k + m;
              b = l + m;
            }
            j -= 1;
          }
        }
      }
#ifdef DEBUG
      g_debug ("UPDATE_BONDS:: id= %d, a= %d, b=%d, a+2s= %d, b+2s= %d, val= %f", id, a, b, a+2*s, b+2*s, v);
#endif
      if (opengl_project -> modelgl -> anim -> last -> img -> style == WIREFRAME)
      {
        if (v > 0.0)
        {
          opengl_project -> modelgl -> anim -> last -> img -> linerad[a][b]
           = opengl_project -> modelgl -> anim -> last -> img -> linerad[b][a] = v;
        }
        v = opengl_project -> modelgl -> anim -> last -> img -> linerad[a][b];
#ifdef GTK3
        // GTK3 Menu Action To Check
        gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_bonds[5+8*c]),
                                 label_cutrab (opengl_project, opengl_project -> modelgl, 3+c));
#endif
      }
      else
      {
        if (v > 0.0)
        {
          opengl_project -> modelgl -> anim -> last -> img -> bondrad[a][b]
            = opengl_project -> modelgl -> anim -> last -> img -> bondrad[b][a] = v;
        }
        v = opengl_project -> modelgl -> anim -> last -> img -> bondrad[a][b];
#ifdef GTK3
      // GTK3 Menu Action To Check
        gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_bonds[3+8*c]),
                                 label_cutrab (opengl_project, opengl_project -> modelgl, 1+c));
#endif
      }
      init_default_shaders (opengl_project -> modelgl);
      break;
  }
  update_entry_double (res, v);
  update (opengl_project -> modelgl);
}

/*!
  \fn void bonds_input_win (GtkWidget * win, project * this_proj, int nspec, int aoc, double ** val)

  \brief prepare bond property entry list

  \param win the GtkWidget sending the signal
  \param this_proj the target project
  \param nspec total number of chemical species
  \param aoc atom(s) (0) or clone(s) (1)
  \param val the list of value(s) to display
*/
void bonds_input_win (GtkWidget * win, project * this_proj, int nspec, int aoc, double ** val)
{
  int i, j, k, l, m, n;
  gchar * dim[2]={"  pts"," Å "};
  GtkWidget * vbox = dialog_get_content_area (win);
  GtkWidget * hbo;
  GtkWidget * rad;
  gchar * str;
  if (this_proj -> modelgl -> anim -> last -> img -> style == WIREFRAME)
  {
    n = 0;
  }
  else
  {
    n = 1;
  }
  for (i=0, k=aoc*nspec; i<nspec; i++, k++)
  {
    j = i + aoc * nspec * (nspec + 1) / 2;
    hbo = create_hbox (0);
    gtk_widget_set_size_request (hbo, 250, -1);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbo, TRUE, TRUE, 0);
    str = g_strdup_printf ("  %s - %s", this_proj -> chemistry -> label[i], this_proj -> chemistry -> label[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, markup_label(str, 100, -1, 0.2, 0.5), FALSE, FALSE, 0);
    g_free (str);
    rad = create_entry (G_CALLBACK(update_bond_parameter), 120, 15, FALSE, GINT_TO_POINTER(j));
    update_entry_double (GTK_ENTRY(rad), val[k][k]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, rad, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, markup_label(dim[n], 40, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
  k = j+1;
  for (i=0; i<nspec-1; i++)
  {
    for (j=i+1; j<nspec; j++, k++)
    {
      l = i + aoc * nspec;
      m = j + aoc * nspec;
      hbo = create_hbox (0);
      gtk_widget_set_size_request (hbo, 250, -1);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbo, TRUE, TRUE, 0);
      str = g_strdup_printf ("  %s - %s", this_proj -> chemistry -> label[i], this_proj -> chemistry -> label[j]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, markup_label(str, 100, -1, 0.2, 0.5), FALSE, FALSE, 0);
      g_free (str);
      rad = create_entry (G_CALLBACK(update_bond_parameter), 120, 15, FALSE, GINT_TO_POINTER(k));
      update_entry_double (GTK_ENTRY(rad), val[l][m]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, rad, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, markup_label(dim[n], 40, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }
  }
  show_the_widgets (vbox);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_bond_parameter (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief update bond parameter callback - creating the dialog GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_bond_parameter (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_bond_parameter (GtkWidget * widg, gpointer data)

  \brief update bond parameter callback - creating the dialog GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_bond_parameter (GtkWidget * widg, gpointer data)
#endif
{
  tint * the_data = (tint *)data;
  gchar * title[2][2] = {{"Adjust atomic bond radius(ii)", "Adjust clone bond radius(ii)"},
                         {"Adjust atomic line width(s)", "Adjust clone line width(s)"}};

  int p = the_data -> a;
  opengl_project_changed (p);
  int s = opengl_project -> nspec;
  double ** val;
  int id = the_data -> b;
  int jd = the_data -> c;
  if (jd)
  {
    val = opengl_project -> modelgl -> anim -> last -> img -> linerad;
  }
  else
  {
    val = opengl_project -> modelgl -> anim -> last -> img -> bondrad;
  }
  GtkWidget * win = dialogmodal (title[jd][id], GTK_WINDOW(opengl_project -> modelgl -> win));
  bonds_input_win (win, opengl_project, s, id, val);
  run_this_gtk_dialog (win, G_CALLBACK(run_destroy_dialog), NULL);
#ifdef GTK4
  update_menu_bar (opengl_project -> modelgl);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void window_bonds (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief create bond(s) configuration window(s) callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void window_bonds (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void window_bonds (GtkWidget * widg, gpointer data)

  \brief create bond(s) configuration window(s) callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void window_bonds (GtkWidget * widg, gpointer data)
#endif
{
  int id;
  tint * bid = (tint *)data;
  gchar * obj[4] = {"box", "axis", "bond", "clone bond"};
  gchar * Nobj[4] = {"Box", "Axis", "Bond", "Clone bond"};
  gchar * str;
  switch (bid -> b)
  {
    case 0:
      id = -5;
      str = g_strdup_printf ("Adjust axis length");
      break;
    case 1:
      id = -3 - bid -> c;
      str = g_strdup_printf ("Adjust %s line width", obj[bid -> c]);
      break;
    case 4:
      id = -3 - bid -> c;
      str = g_strdup_printf ("Adjust %s cylinders radius", obj[bid -> c]);
      break;
    default:
      id = - bid -> b + 1;
      str = g_strdup_printf ("Adjust %s cylinders radius", obj[bid -> b]);
      break;
  }
  opengl_project_changed (bid -> a);
  GtkWidget * win = dialogmodal (str, GTK_WINDOW(opengl_project -> modelgl -> win));
  g_free (str);
  GtkWidget * vbox = dialog_get_content_area (win);
  GtkWidget * hbox = create_hbox (0);
  gtk_widget_set_size_request (hbox, 300, -1);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  switch (bid -> b)
  {
    case 0:
      str = g_strdup_printf ("Axis length");
      break;
    case 1:
      str = g_strdup_printf ("%s line width", Nobj[bid -> c]);
      break;
    case 4:
      str = g_strdup_printf ("%s cylinders radius", Nobj[bid -> c]);
      break;
    default:
      str = g_strdup_printf ("%s cylinders radius", Nobj[bid -> b]);
      break;
  }
  GtkWidget * label = gtk_label_new (str);
  g_free (str);
  gtk_widget_set_size_request (label, 150, -1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, label, FALSE, FALSE, 0);
  GtkWidget * rad = create_entry (G_CALLBACK(update_bond_parameter), 120, -1, FALSE, GINT_TO_POINTER(id));
  switch (bid -> b)
  {
    case 0:
      update_entry_double (GTK_ENTRY(rad), opengl_project -> modelgl -> anim -> last -> img -> xyz -> length);
      break;
    case 1:
      update_entry_double (GTK_ENTRY(rad), (bid -> c) ? opengl_project -> modelgl -> anim -> last -> img -> xyz -> line : opengl_project -> modelgl -> anim -> last -> img -> abc -> line);
      break;
    case 4:
      update_entry_double (GTK_ENTRY(rad), (bid -> c) ? opengl_project -> modelgl -> anim -> last -> img -> xyz -> rad : opengl_project -> modelgl -> anim -> last -> img -> abc -> rad);
      break;
    default:
      update_entry_double (GTK_ENTRY(rad), opengl_project -> modelgl -> anim -> last -> img -> radall[bid -> b-2]);
      break;
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, rad, FALSE, FALSE, 0);
  switch (bid -> b)
  {
    case 1:
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(" [pts]", 40, -1, 0.0, 0.5), FALSE, FALSE, 0);
      break;
    default:
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(" [&#xC5;]", 30, -1, 0.0, 0.5), FALSE, FALSE, 0);
      break;
  }
  gtk_widget_set_size_request (win, 350, 100);
  show_the_widgets (vbox);
  run_this_gtk_dialog (win, G_CALLBACK(run_destroy_dialog), NULL);
#ifdef GTK4
  update_menu_bar (opengl_project -> modelgl);
#endif
}
