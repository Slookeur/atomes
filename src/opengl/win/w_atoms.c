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
* @file w_atoms.c
* @short Functions to create the atom(s) / clone(s) advanced configuration window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_atoms.c'
*
* Contains:
*

 - The functions to create the atom(s) / clone(s) advanced configuration window

*
* List of functions:

  G_MODULE_EXPORT gboolean close_event_model (GtkWindow * widg, gpointer data);
  G_MODULE_EXPORT gboolean close_event_model (GtkWidget * widg, GdkEvent * event, gpointer data);

  void atoms_input_win (GtkWidget * win, project * this_proj, int nspec, int aoc, double * val);

  G_MODULE_EXPORT void update_atom_size (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_atom_parameter (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void set_atom_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void toggled_show_hide_atom (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void toggled_show_hide_atom (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void toggled_show_hide_label (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void toggled_show_hide_label (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void close_model (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void atom_properties (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void atom_properties (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * prop_tab (glwin * view, int aoc);
  GtkWidget * advance_atom_notebook (glwin * view, int atom_or_clone);
  GtkWidget * advanced_atom_properties (int atom_or_clone, glwin * view);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"
#include "color_box.h"

extern atom_search * allocate_atom_search (int proj, int action, int searchid, int tsize);
extern void check_hidden_visible (project * this_proj);
extern gchar * label_atpts (project * this_proj, glwin * view, int id);
extern GtkWidget * labels_tab (glwin * view, int id);
extern GtkWidget * selection_tab (atom_search * asearch, int nats);
extern GtkTreeStore * atom_model;

/*!
  \fn G_MODULE_EXPORT void update_atom_size (GtkEntry * res, gpointer data)

  \brief update atom radius callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_atom_size (GtkEntry * res, gpointer data)
{
  int j;
  gchar * str;
  tint * the_data = (tint *)data;
  int a, t;
  a = the_data -> c;
  project * this_proj = get_project_by_id (the_data -> a);
  j = this_proj -> modelgl -> anim -> last -> img -> style;
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  int s = this_proj -> nspec;
  if (j == WIREFRAME || j == PUNT)
  {
    if (v > 0.0)
    {
      this_proj -> modelgl -> anim -> last -> img -> pointrad[a] = v;
      int shaders[2] = {ATOMS, SELEC};
      re_create_md_shaders (2, shaders, this_proj);
      this_proj -> modelgl -> create_shaders[PICKS] = TRUE;
      this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
    }
    v = this_proj -> modelgl -> anim -> last -> img -> pointrad[a];
    t = PUNT;
  }
  else
  {
    if (v > 0.0)
    {
      this_proj -> modelgl -> anim -> last -> img -> sphererad[a] = v;
      int shaders[2] = {ATOMS, SELEC};
      re_create_md_shaders (2, shaders, this_proj);
      this_proj -> modelgl -> create_shaders[PICKS] = TRUE;
      this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
    }
    v = this_proj -> modelgl -> anim -> last -> img -> sphererad[a];
    t = SPHERES;
  }
  update_entry_double (res, v);
  if (a < s)
  {
    str = label_atpts (this_proj, this_proj -> modelgl, t/PUNT);
#ifdef GTK3
    gtk_menu_item_set_label (GTK_MENU_ITEM(this_proj -> modelgl -> ogl_atoms[t-2]), str);
#endif
  }
  else
  {
    str = label_atpts (opengl_project, opengl_project -> modelgl, 2+t/PUNT);
#ifdef GTK3
    gtk_menu_item_set_label (GTK_MENU_ITEM(opengl_project -> modelgl -> ogl_atoms[t+2]), str);
#endif
  }
  g_free (str);
  update (this_proj -> modelgl);
}

/*!
  \fn void atoms_input_win (GtkWidget * win, project * this_proj, int nspec, int aoc, double * val)

  \brief prepare atom radii entry list

  \param win the GtkWidget sending the signal
  \param this_proj the target project
  \param nspec total number of chemical species
  \param aoc atom(s) (0) or clone(s) (1)
  \param val radii list
*/
void atoms_input_win (GtkWidget * win, project * this_proj, int nspec, int aoc, double * val)
{
  int i, j, k;
  GtkWidget * vbox = dialog_get_content_area (win);
  GtkWidget * hbo;
  GtkWidget * rad;
  gchar * dim[2]={"  pts"," Å "};
  gchar * str;
  if (this_proj -> modelgl -> anim -> last -> img -> style == WIREFRAME || this_proj -> modelgl -> anim -> last -> img -> style == PUNT)
  {
    k = 0;
  }
  else
  {
    k = 1;
  }
  for (i=0; i<nspec; i++)
  {
    hbo = create_hbox (0);
    gtk_widget_set_size_request (hbo, 250, -1);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbo, TRUE, TRUE, 0);
    if (aoc == 0)
    {
      str = g_strdup_printf ("  %s", this_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("  %s*", this_proj -> chemistry -> label[i]);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, markup_label(str, 100, -1, 0.2, 0.5), FALSE, FALSE, 0);
    g_free (str);
    j = i + aoc*nspec;
    rad = create_entry (G_CALLBACK(update_atom_size), 120, 15, FALSE, (gpointer)& this_proj -> modelgl -> colorp[0][j]);
    update_entry_double (GTK_ENTRY(rad), val[j]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, rad, FALSE, FALSE, 0);
    if (this_proj -> modelgl -> anim -> last -> img -> style == SPACEFILL) widget_set_sensitive (rad, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbo, markup_label(dim[k], 30, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
  show_the_widgets (vbox);
}

/*!
  \fn G_MODULE_EXPORT void set_atom_parameter (GtkWidget * widg, gpointer data)

  \brief set atom parameter - creating the dialog

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_atom_parameter (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  gchar * title[2][2] = {{"Adjust atom sphere radius", "Adjust clone sphere radius"},
                         {"Adjust atom point size", "Adjust clone point size"}};

  opengl_project_changed (the_data -> a);
  int s = opengl_project -> nspec;
  double * val;
  int id = the_data -> b;
  int jd = the_data -> c;
  if (jd)
  {
    val = opengl_project -> modelgl -> anim -> last -> img -> pointrad;
  }
  else if (opengl_project -> modelgl -> anim -> last -> img -> style == SPACEFILL)
  {
    val = opengl_project -> modelgl -> anim -> last -> img -> atomicrad;
  }
  else
  {
    val = opengl_project -> modelgl -> anim -> last -> img -> sphererad;
  }
  GtkWidget * win = dialogmodal (title[jd][id], GTK_WINDOW(opengl_project -> modelgl -> win));
  atoms_input_win (win, opengl_project, s, id, val);
  run_this_gtk_dialog (win, G_CALLBACK(run_destroy_dialog), NULL);
}

/*!
  \fn G_MODULE_EXPORT void set_atom_color (GtkColorChooser * colob, gpointer data)

  \brief set atom color callback

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_atom_color (GtkColorChooser * colob, gpointer data)
{
  tint * the_data = (tint *)data;
  int a;
  project * this_proj = get_project_by_id(the_data -> a);
  a = the_data -> c;
  this_proj -> modelgl -> anim -> last -> img -> at_color[a] = get_button_color (colob);
  int shaders[2] = {ATOMS, BONDS};
  re_create_md_shaders (2, shaders, this_proj);
  update (this_proj -> modelgl);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggled_show_hide_atom (GtkCheckButton * but, gpointer data)

  \brief toggle show/hide atom(s) callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_show_hide_atom (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggled_show_hide_atom (GtkToggleButton * but, gpointer data)

  \brief toggle show/hide atom(s) callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_show_hide_atom (GtkToggleButton * but, gpointer data)
#endif
{
  tint * id = (tint *)data;
  int i, j, k;
  i = id -> a;
  j = id -> b;
  k = id -> c;
  gboolean show = button_get_status ((GtkWidget *)but);
#ifdef GTK4
  project * this_proj = get_project_by_id (i);
  int l, m;
  for (l=0; l<this_proj -> steps; l++)
  {
    for (m=0; m<this_proj -> natomes; m++)
    {
      if (this_proj -> atoms[l][m].sp == k)
      {
        this_proj -> atoms[l][m].show[j] = show;
      }
    }
  }
  this_proj -> modelgl -> anim -> last -> img -> show_atom[j][k] = show;
  check_hidden_visible (this_proj);
  init_default_shaders (this_proj -> modelgl);
  update_menu_bar (this_proj -> modelgl);
#else
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)get_project_by_id(i) -> modelgl -> ogl_spec[j][k], show);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggled_show_hide_label (GtkCheckButton * but, gpointer data)

  \brief toggle show/hide label(s) callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_show_hide_label (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggled_show_hide_label (GtkToggleButton * but, gpointer data)

  \brief toggle show/hide label(s) callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_show_hide_label (GtkToggleButton * but, gpointer data)
#endif
{
  tint * id = (tint *)data;
  int i, j, k;
  i = id -> a;
  j = id -> b;
  k = id -> c;
  gboolean show = button_get_status ((GtkWidget *)but);
  project * this_proj = get_project_by_id (i);
#ifdef GTK4
  int l, m;
  for (l=0; l<this_proj -> steps; l++)
  {
    for (m=0; m<this_proj -> natomes; m++)
    {
      if (this_proj -> atoms[l][m].sp == k)
      {
        this_proj -> atoms[l][m].label[j] = show;
      }
    }
  }
  this_proj -> modelgl -> labelled = check_label_numbers (this_proj, j);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  update (this_proj -> modelgl);
  update_menu_bar (this_proj -> modelgl);
#else
  // GTK3 Menu Action To Check
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_lab[j][k], show);
#endif
}

/*!
  \fn GtkWidget * prop_tab (glwin * view, int aoc)

  \brief create atom(s)/clone(s) properties tab

  \param view the target glwin
  \param aoc atom(s) (0) or clone(s) (1)
*/
GtkWidget * prop_tab (glwin * view, int aoc)
{
  int i, j, k;
  gchar * str;
  GtkWidget * lab;
  GtkWidget * prop_box;
  GtkWidget * hbox;
  GtkWidget * but;
  GtkWidget * entry;
  project * this_proj = get_project_by_id(view -> proj);
  gchar * col[5] = {"<b>Color</b>",
                    "<b>Radius [Å]</b>",
                    "<b>Show</b>",
                    "<b>Label</b>",
                    "<b>Size [pts]</b>"};
  int csize[5] = {60, 0, 25, 0, 0};

  GtkWidget * prop = create_layout (-1, -1);
  GtkWidget * vbox = add_vbox_to_layout (prop, -1, -1);

  k = view -> anim -> last -> img -> style;
  prop_box = create_hbox (0);
  gtk_widget_set_size_request (prop_box, -1, -1);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, prop_box, FALSE, TRUE, 0);
  for (i=0; i<4; i++)
  {
    if (i == 1 && (k == WIREFRAME || k == PUNT))
    {
      lab = markup_label(col[4], -1, -1, 0.0, 0.0);
    }
    else
    {
      lab = markup_label (col[i], -1, -1, 0.0, 0.0);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, prop_box, lab, FALSE, FALSE, csize[i]);
  }
  for (i=0; i< this_proj -> nspec; i++)
  {
    hbox = create_hbox (0);
    //gtk_widget_set_size_request (hbox, -1, -1);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, TRUE, 0);
    j = i + aoc * this_proj -> nspec;
    if (aoc == 0)
    {
      str = g_strdup_printf (" <b>%s</b>", this_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf (" <b>%s<sup>*</sup></b>", this_proj -> chemistry -> label[i]);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 30, -1, 0.0, 0.5), FALSE, FALSE, 10);
    g_free (str);

    but = color_button (view -> anim -> last -> img -> at_color[j], TRUE, 80, -1, G_CALLBACK(set_atom_color), & view -> colorp[0][j]);
    if (aoc == 0)
    {
      str = g_strdup_printf ("%s atom(s) color", this_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("%s clone(s) color", this_proj -> chemistry -> label[i]);
    }
    gtk_color_button_set_title (GTK_COLOR_BUTTON(but), str);
    g_free (str);
    if (this_proj -> modelgl -> anim -> last -> img -> color_map[0] != 0)
    {
      widget_set_sensitive (but, 0);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 5);

    // Raduis entry
    entry = create_entry (G_CALLBACK(update_atom_size), 80, 10, FALSE, (gpointer)& this_proj -> modelgl -> colorp[0][j]);
    if (k == WIREFRAME || k == PUNT)
    {
      update_entry_double (GTK_ENTRY(entry), view -> anim -> last -> img -> pointrad[j]);
    }
    else if (k == BALL_AND_STICK || k == SPHERES)
    {
      update_entry_double (GTK_ENTRY(entry), view -> anim -> last -> img -> sphererad[j]);
    }
    else if (k == SPACEFILL)
    {
      update_entry_double (GTK_ENTRY(entry), view -> anim -> last -> img -> atomicrad[j]);
      widget_set_sensitive (entry, 0);
    }
    else
    {
      if (k == CYLINDERS)
      {
        update_entry_double (GTK_ENTRY(entry), view -> anim -> last -> img -> radall[aoc]);
      }
      widget_set_sensitive (entry, 0);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);

    // Show/Hide check button
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button (NULL, -1, -1, view -> anim -> last -> img -> show_atom[aoc][i], G_CALLBACK(toggled_show_hide_atom), & view -> colorp[aoc][i]), FALSE, FALSE, 30);
    // Show/Hide label check button
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button (NULL, -1, -1, view -> anim -> last -> img -> show_label[aoc][i], G_CALLBACK(toggled_show_hide_label), & view -> colorp[aoc][i]), FALSE, FALSE, 5);
  }
  return prop;
}

/*!
  \fn G_MODULE_EXPORT void close_model (GtkButton * but, gpointer data)

  \brief free 'model_win' data

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void close_model (GtkButton * but, gpointer data)
{
  tint * dat = (tint *)data;
  project * this_proj = get_project_by_id(dat -> a);
  this_proj -> modelgl -> model_win[dat -> b] -> win = destroy_this_widget (this_proj -> modelgl -> model_win[dat -> b] -> win);
  g_free (this_proj -> modelgl -> model_win[dat -> b]);
  this_proj -> modelgl -> model_win[dat -> b] = NULL;
  g_free (this_proj -> modelgl -> search_widg[dat -> b]);
  this_proj -> modelgl -> search_widg[dat -> b] = NULL;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean close_event_model (GtkWindow * widg, gpointer data)

  \brief close event callback GTK4

  \param widg the GtkWindow sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean close_event_model (GtkWindow * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean close_event_model (GtkWidget * widg, GdkEvent * event, gpointer data)

  \brief close event callback GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean close_event_model (GtkWidget * widg, GdkEvent * event, gpointer data)
#endif
{
  close_model (NULL, data);
  return FALSE;
}

/*!
  \fn GtkWidget * advance_atom_notebook (glwin * view, int atom_or_clone)

  \brief create atom(s) advanced properties notebook

  \param view the target glwin
  \param atom_or_clone atom(s) or clone(s) (0/1)
*/
GtkWidget * advance_atom_notebook (glwin * view, int atom_or_clone)
{
  GtkWidget * notebook = gtk_notebook_new ();
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), prop_tab (view, atom_or_clone), gtk_label_new ("Display properties"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), labels_tab (view, atom_or_clone), gtk_label_new ("Label properties"));

  if (view -> search_widg[atom_or_clone] == NULL)
  {
    view -> search_widg[atom_or_clone] = allocate_atom_search (view -> proj, atom_or_clone, atom_or_clone, get_project_by_id(view -> proj) -> natomes);
  }
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook),
                            selection_tab (view -> search_widg[atom_or_clone], get_project_by_id(view -> proj) -> natomes),
                            gtk_label_new ((atom_or_clone) ? "Clone(s) selection" : "Atom(s) selection"));
  return notebook;
}

/*!
  \fn GtkWidget * advanced_atom_properties (int atom_or_clone, glwin * view)

  \brief create atom advanced properties window 'model_win'

  \param atom_or_clone atom(s) (0) or clone(s) (1)
  \param view the target glwin
*/
GtkWidget * advanced_atom_properties (int atom_or_clone, glwin * view)
{
  GtkWidget * aprop;
  gchar * win_title[2]={"Atom(s) configuration - ", "Clone(s) configuration - "};
  gchar * str = g_strdup_printf ("%s%s", win_title[atom_or_clone], prepare_for_title(get_project_by_id(view -> proj)->name));
  aprop = create_win (str, view -> win, FALSE, FALSE);
  int i = (get_project_by_id(view -> proj)-> natomes > 10000) ? 170 : 0;
  gtk_widget_set_size_request (aprop, -1, 580+i);
  GtkWidget * vbox = create_vbox (5);
  add_container_child (CONTAINER_WIN, aprop, vbox);
  view -> model_win[atom_or_clone] -> notebook = advance_atom_notebook (view, atom_or_clone);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,  view -> model_win[atom_or_clone] -> notebook, TRUE, TRUE, 0);
  GtkWidget * hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  GtkWidget * but = create_button ("Close", IMG_STOCK, FCLOSE, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(close_model), & view -> colorp[atom_or_clone][0]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 5);
  add_gtk_close_event (aprop, G_CALLBACK(close_event_model), & view -> colorp[atom_or_clone][0]);
  return aprop;
}

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT void atom_properties (GtkWidget * widg, gpointer data)

  \brief atom(s) property callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void atom_properties (GtkWidget * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void atom_properties (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief atom(s) property callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void atom_properties (GSimpleAction * action, GVariant * parameter, gpointer data)
#endif
{
  tint * the_data = (tint *) data;
  int atom_or_clone = the_data -> b;
  project * this_proj = get_project_by_id (the_data -> a);
  if (this_proj -> modelgl -> model_win[atom_or_clone] == NULL)
  {
    this_proj -> modelgl -> model_win[atom_or_clone] = g_malloc0 (sizeof*this_proj -> modelgl -> model_win[atom_or_clone]);
    this_proj -> modelgl -> model_win[atom_or_clone] -> win = advanced_atom_properties (atom_or_clone, this_proj -> modelgl);
    show_the_widgets (this_proj -> modelgl -> model_win[atom_or_clone] -> win);
    if (this_proj -> natomes < 10000)
    {
      hide_the_widgets (this_proj -> modelgl -> search_widg[atom_or_clone] -> info[1]);
    }
    else
    {
      hide_the_widgets (this_proj -> modelgl -> search_widg[atom_or_clone] -> id_box);
    }
  }
  if (GTK_IS_WIDGET(this_proj -> modelgl -> model_win[atom_or_clone] -> win))
  {
     gtk_notebook_set_current_page (GTK_NOTEBOOK (this_proj -> modelgl -> model_win[atom_or_clone] -> notebook), the_data -> c);
  }
  else
  {
    show_warning (g_strdup_printf ("Error impossible to display the model window for %s !", (atom_or_clone) ? "clones" : "atoms"), this_proj -> modelgl -> win);
  }
}
