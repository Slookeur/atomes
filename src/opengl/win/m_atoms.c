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
* @file m_atoms.c
* @short Functions to create the 'Model -> Atom(s)' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_atoms.c'
*
* Contains:
*

 - The functions to create the 'Model -> Atom(s)' submenu

*
* List of functions:

  gchar * label_atpts (project * this_proj, glwin * view, int id);

  G_MODULE_EXPORT void show_hide_atoms (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void show_hide_labels (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void show_hide_all_atom_labels (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void show_hide_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void show_hide_labels (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void show_hide_all_atom_labels (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * create_spec_menu (char * name, gboolean va, gboolean vb, GtkWidget * menu, GCallback handler, tint * data);
  GtkWidget * create_atom_layout_widget (gchar * str, GtkWidget * widg, tint * data);
  GtkWidget * show_atoms_submenu (glwin * view, int id, int at);
  GtkWidget * color_atoms_submenu (glwin * view, int id, int at);
  GtkWidget * label_atoms_submenu (glwin * view, int id, int at);
  GtkWidget * menu_atoms (glwin * view, int id, int at);

  GMenu * label_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive);
  GMenu * color_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive);
  GMenu * show_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive);
  GMenu * menu_atoms (glwin * view, int popm, int at);

*/

#include "global.h"
#include "glview.h"
#include "glwindow.h"
#include "color_box.h"

extern void check_hidden_visible (project * this_proj);
extern G_MODULE_EXPORT void set_atom_parameter (GtkWidget * widg, gpointer data);

/*!
  \fn gchar * label_atpts (project * this_proj, glwin * view, int id)

  \brief prepare the text of a menu item in the 'Model -> Atom(s)' submenu

  \param this_proj the target project
  \param view the target glwin
  \param id the type of label to prepare
*/
gchar * label_atpts (project * this_proj, glwin * view, int id)
{
  int i;
  gchar * mot;
  gchar * tmp;
  gchar * str;

  for (i=0; i < this_proj -> nspec; i++)
  {
    switch (id)
    {
      case 0:
        str = g_strdup_printf ("%s [ %f Å ]", this_proj -> chemistry -> label[i], view -> anim -> last -> img -> sphererad[i]);
        break;
      case 1:
        str = g_strdup_printf ("%s [ %f pts ]", this_proj -> chemistry -> label[i], view -> anim -> last -> img -> pointrad[i]);
        break;
      case 2:
        str = g_strdup_printf ("%s [ %f pts ]", this_proj -> chemistry -> label[i], view -> anim -> last -> img -> sphererad[i+this_proj -> nspec]);
        break;
      case 3:
        str = g_strdup_printf ("%s [ %f pts ]", this_proj -> chemistry -> label[i], view -> anim -> last -> img -> pointrad[i+this_proj -> nspec]);
        break;
      default:
        str = g_strdup_printf ("%s [ %f Å ]", this_proj -> chemistry -> label[i], view -> anim -> last -> img ->  atomicrad[i]);
        break;
    }
    if (i == 0)
    {
      mot = g_strdup_printf ("%s", str);
    }
    else
    {
      tmp = g_strdup_printf ("%s", mot);
      g_free (mot);
      mot = g_strdup_printf ("%s\n%s", tmp, str);
      g_free (tmp);
    }
    g_free (str);
  }
  return mot;
}

#ifdef GTK3
extern G_MODULE_EXPORT void set_show_hide_all_atom_labels (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void atom_properties (GtkWidget * widg, gpointer data);

/*!
  \fn G_MODULE_EXPORT void show_hide_atoms (GtkWidget * widg, gpointer data)

  \brief handle the show / hide atomic species signal

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_atoms (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *) data;
  int j = the_data -> b;
  int k = the_data -> c;
  int l, m;
  project * this_proj = get_project_by_id(the_data -> a);
  gboolean v = gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg);
  if (widg != this_proj -> modelgl -> ogl_spec[j][k])
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_spec[j][k], v);
  }
  for (l=0; l<this_proj -> steps; l++)
  {
    for (m=0; m<this_proj -> natomes; m++)
    {
      if (this_proj -> atoms[l][m].sp == k) this_proj -> atoms[l][m].show[j] = v;
    }
  }
  this_proj -> modelgl -> anim -> last -> img -> show_atom[j][k] = v;
  check_hidden_visible (this_proj);
  init_default_shaders (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void show_hide_labels (GtkWidget * widg, gpointer data)

  \brief handle the show / hide atomic species label signal

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_labels (GtkWidget * widg, gpointer data)
{
  int l, m;
  tint * id = (tint *) data;
  int j = id -> b;
  int k = id -> c;
  project * this_proj = get_project_by_id(id -> a);
  gboolean v = gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg);
  if (widg != this_proj -> modelgl -> ogl_lab[j][k])
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_lab[j][k], v);
  }
  for (l=0; l<this_proj -> steps; l++)
  {
    for (m=0; m<this_proj -> natomes; m++)
    {
      if (this_proj -> atoms[l][m].sp == k)
      {
        if (this_proj -> atoms[l][m].label[j] != v)
        {
          this_proj -> atoms[l][m].label[j] = v;
        }
      }
    }
  }
  this_proj -> modelgl -> labelled = check_label_numbers (this_proj, j);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void show_hide_all_atom_labels (GtkWidget * widg, gpointer data)

  \brief handle the show / hide all atomic labels signal

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_all_atom_labels (GtkWidget * widg, gpointer data)
{
  tint * id = (tint *) data;
  int j = id -> b;
  int k;
  gboolean show = TRUE;
  project * this_proj = get_project_by_id(id -> a);
  for (k=0; k<this_proj -> nspec; k++)
  {
    if (plot -> show_label[j][k]) show = FALSE;
  }
  for (k=0; k<this_proj -> nspec; k++)
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_lab[j][k], show);
  }
}

/*!
  \fn GtkWidget * create_spec_menu (char * name, gboolean va, gboolean vb, GtkWidget * menu, GCallback handler, tint * data)

  \brief create a chemical species related check menu item GTK3

  \param name menu item name
  \param va check menu item status
  \param vb menu item sensitivity
  \param menu the GtkWidget sending the signal
  \param handler the associated callback
  \param data the associated data pointer
*/
GtkWidget * create_spec_menu (char * name, gboolean va, gboolean vb, GtkWidget * menu, GCallback handler, tint * data)
{
  GtkWidget * spec_widget = gtk3_menu_item (menu, name, IMG_NONE, NULL, handler, data, FALSE, 0, 0, TRUE, FALSE, va);
  widget_set_sensitive (spec_widget, vb);
  return spec_widget;
}

/*!
  \fn GtkWidget * create_atom_layout_widget (gchar * str, GtkWidget * widg, tint * data)

  \brief create a 'Model -> Atom(s)' menu item GTK3

  \param str the label for the menu item
  \param widg the menu GtkWidget to attach the menu item to
  \param data the associated data pointer
*/
GtkWidget * create_atom_layout_widget (gchar * str, GtkWidget * widg, tint * data)
{
  GtkWidget * menu = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menu);
  GtkWidget * layout = create_menu_item (TRUE, str);
  gtk_menu_shell_append ((GtkMenuShell *)menu, layout);
  g_signal_connect (G_OBJECT (layout), "activate", G_CALLBACK(set_atom_parameter), data);
  return layout;
}

/*!
  \fn GtkWidget * show_atoms_submenu (glwin * view, int id, int at)

  \brief create the 'Atom(s) -> show' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
  \param at atoms (0) or clones (1)
*/
GtkWidget * show_atoms_submenu (glwin * view, int id, int at)
{
  GtkWidget * mshow = gtk_menu_new ();
  gchar * str;
  gboolean sensitive = (! at) ? TRUE : view -> anim -> last -> img -> draw_clones;
  project * this_proj = get_project_by_id (view -> proj);
  if (id == 0)
  {
    view -> ogl_spec[at] = g_malloc (this_proj -> nspec*sizeof*view -> ogl_spec[at]);
  }
  int i;
  for (i=0; i< this_proj -> nspec; i++)
  {
    if (at == 0)
    {
      str = g_strdup_printf ("%s", this_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("%s*", this_proj -> chemistry -> label[i]);
    }
    if (id == 0)
    {
      view -> ogl_spec[at][i] = create_spec_menu (str,
                                                  view -> anim -> last -> img -> show_atom[at][i],
                                                  sensitive,
                                                  mshow,
                                                  G_CALLBACK(show_hide_atoms),
                                                  & view -> colorp[at][i]);

    }
    else
    {
      create_spec_menu (str, view -> anim -> last -> img -> show_atom[at][i],
                        sensitive,
                        mshow,
                        G_CALLBACK(show_hide_atoms),
                        & view -> colorp[at][i]);
    }
    g_free (str);
  }
  return mshow;
}

/*!
  \fn GtkWidget * color_atoms_submenu (glwin * view, int id, int at)

  \brief create the 'Atom(s) -> Color(s)' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
  \param at atoms (0) or clones (1)
*/
GtkWidget * color_atoms_submenu (glwin * view, int id, int at)
{
  GtkWidget * menuc = gtk_menu_new ();
  project * this_proj = get_project_by_id (view -> proj);
  gchar * str;
  GtkWidget * sp;
  int i, j;
  if (at == 0)
  {
    j = 0;
  }
  else
  {
    j = this_proj -> nspec;
  }
  for (i=0; i< this_proj -> nspec; i++)
  {
    if (at == 0)
    {
      str = g_strdup_printf ("%s", this_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("%s*", this_proj -> chemistry -> label[i]);
    }
    sp = create_menu_item (FALSE, str);
    g_free (str);
    gtk_menu_shell_append ((GtkMenuShell *)menuc, sp);
    gtk_menu_item_set_submenu ((GtkMenuItem *)sp, color_box(view, i+j, 0, 0));
  }
  if (at == 1) widget_set_sensitive (menuc, view -> anim -> last -> img -> draw_clones);
  return menuc;
}

/*!
  \fn GtkWidget * label_atoms_submenu (glwin * view, int id, int at)

  \brief create the 'Atom(s) -> Label(s)' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
  \param at atoms (0) or clones (1)
*/
GtkWidget * label_atoms_submenu (glwin * view, int id, int at)
{
  GtkWidget * menul = gtk_menu_new ();
  GtkWidget * all = create_menu_item (FALSE, "Show/Hide all");
  g_signal_connect (G_OBJECT (all), "activate", G_CALLBACK(show_hide_all_atom_labels), & view -> colorp[at][0]);
  gtk_menu_shell_append ((GtkMenuShell *)menul, all);
  GtkWidget * l_show = create_menu_item (FALSE, "Show");
  gtk_menu_shell_append ((GtkMenuShell *)menul, l_show);
  GtkWidget * mshow = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)l_show, mshow);
  project * this_proj = get_project_by_id (view -> proj);
  gchar * str;
  gboolean sensitive = (! at) ? TRUE : view -> anim -> last -> img -> draw_clones;
  if (id == 0)
  {
    view -> ogl_lab[at] = g_malloc (this_proj -> nspec*sizeof*view -> ogl_lab[at]);
  }
  int i;
  for (i=0; i< this_proj -> nspec; i++)
  {
    if (at == 0)
    {
      str = g_strdup_printf ("%s", this_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("%s*", this_proj -> chemistry -> label[i]);
    }
    if (id == 0)
    {
      view -> ogl_lab[at][i] = create_spec_menu (str,
                                                 view -> anim -> last -> img -> show_label[at][i],
                                                 sensitive,
                                                 mshow,
                                                 G_CALLBACK(show_hide_labels),
                                                 & view -> colorp[at][i]);
    }
    else
    {
      create_spec_menu (str,
                        view -> anim -> last -> img -> show_label[at][i],
                        sensitive,
                        mshow,
                        G_CALLBACK(show_hide_labels),
                        & view -> colorp[at][i]);
    }
  }
  if (at == 0 || id == 1)
  {
    GtkWidget * l_sel = create_menu_item (FALSE, "Select atom(s)");
    gtk_menu_shell_append ((GtkMenuShell *)menul, l_sel);
    g_signal_connect (G_OBJECT (l_sel), "activate",G_CALLBACK(atom_properties), & view -> colorp[at][2]);
    if (id == 1) widget_set_sensitive (l_sel, sensitive);
    GtkWidget * l_adv = add_advanced_item (menul, G_CALLBACK(atom_properties), (gpointer)& view -> colorp[at][1], FALSE, 0, 0);
    if (id == 1) widget_set_sensitive (l_adv, sensitive);
  }
  else
  {
    view -> ogl_clones[2] = create_menu_item (FALSE, "Select atom(s)");
    gtk_menu_shell_append ((GtkMenuShell *)menul, view -> ogl_clones[2]);
    g_signal_connect (G_OBJECT (view -> ogl_clones[2]), "activate",G_CALLBACK(atom_properties), & view -> colorp[at][2]);
    widget_set_sensitive (view -> ogl_clones[2], sensitive);
    view -> ogl_clones[3] = add_advanced_item (menul, G_CALLBACK(atom_properties), (gpointer)& view -> colorp[at][1], FALSE, 0, 0);
    widget_set_sensitive (view -> ogl_clones[3], sensitive);
  }
  return menul;
}

/*!
  \fn GtkWidget * menu_atoms (glwin * view, int id, int at)

  \brief create the 'Atom(s)' submenu elements GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
  \param at atoms (0) or clones (1)
*/
GtkWidget * menu_atoms (glwin * view, int id, int at)
{
  int i;
  GtkWidget * widg;
  gchar * str;
  project * this_proj = get_project_by_id(view -> proj);
  GtkWidget * menua = gtk_menu_new ();
  gtk_menu_shell_append ((GtkMenuShell *)menua, menu_item_new_with_submenu ("Show", TRUE, show_atoms_submenu (view, id, at)));
  if (at == 1 && id == 0)
  {
    view -> ogl_clones[1] = color_atoms_submenu (view, id, at);
    gtk_menu_shell_append ((GtkMenuShell *)menua, menu_item_new_with_submenu ("Color(s)", ! (view -> anim -> last -> img -> color_map[0] != 0), view -> ogl_clones[1]));
  }
  else
  {
    gtk_menu_shell_append ((GtkMenuShell *)menua, menu_item_new_with_submenu ("Color(s)", ! (view -> anim -> last -> img -> color_map[0] != 0), color_atoms_submenu (view, id, at)));
  }
  gtk_menu_shell_append ((GtkMenuShell *)menua, menu_item_new_with_submenu ("Label(s)", TRUE, label_atoms_submenu (view, id, at)));

  i = view -> anim -> last -> img -> style;
  str = label_atpts (this_proj, view, 2*at);
  if (id == 0)
  {
    view -> ogl_atoms[4*at] = create_menu_item(FALSE, "Radius(ii)");
    gtk_menu_shell_append ((GtkMenuShell *)menua, view -> ogl_atoms[4*at]);
    view -> ogl_atoms[4*at+1] = create_atom_layout_widget (str, view -> ogl_atoms[4*at], & view -> colorp[at][0]);
    if (at == 1 && ! view -> anim -> last -> img -> draw_clones)
    {
      widget_set_sensitive (view -> ogl_atoms[4*at+1], 0);
    }
  }
  else if (i == SPHERES || i == BALL_AND_STICK)
  {
    widg = create_menu_item (FALSE, "Radius(ii)");
    gtk_menu_shell_append ((GtkMenuShell *)menua, widg);
    widg = create_atom_layout_widget (str, widg, & view -> colorp[at][0]);
    if (at == 1 && ! view -> anim -> last -> img -> draw_clones)
    {
      widget_set_sensitive (widg, 0);
    }
  }
  g_free (str);

  str = label_atpts (this_proj, view, 1+2*at);
  if (id == 0)
  {
    view -> ogl_atoms[4*at+2] = create_menu_item(FALSE, "Dot size(s)");
    gtk_menu_shell_append ((GtkMenuShell *)menua, view -> ogl_atoms[4*at+2]);
    view -> ogl_atoms[4*at+3] = create_atom_layout_widget (str, view -> ogl_atoms[4*at+2], & view -> colorp[at][1]);
    if (at == 1 && ! view -> anim -> last -> img -> draw_clones)
    {
      widget_set_sensitive (view -> ogl_atoms[4*at+3], 0);
    }
  }
  else if (i == WIREFRAME || i == PUNT)
  {
    widg = create_menu_item (FALSE, "Dot size(s)");
    gtk_menu_shell_append ((GtkMenuShell *)menua, widg);
    widg = create_atom_layout_widget (str, widg, & view -> colorp[at][1]);
    if (at == 1 && ! view -> anim -> last -> img -> draw_clones)
    {
      widget_set_sensitive (widg, 0);
    }
  }
  g_free (str);
  if (id == 0 && at == 1)
  {
    view -> ogl_clones[4] = add_advanced_item (menua, G_CALLBACK(atom_properties), (gpointer)& view -> colorp[at][0], FALSE, 0, 0);
    if (at == 1)
    {
      widget_set_sensitive (view -> ogl_clones[4], view -> anim -> last -> img -> draw_clones);
    }
  }
  else
  {
    GtkWidget * advanced = add_advanced_item (menua, G_CALLBACK(atom_properties), (gpointer)& view -> colorp[at][0], FALSE, 0, 0);
    if (at == 1)
    {
      widget_set_sensitive (advanced, view -> anim -> last -> img -> draw_clones);
    }
  }
  return menua;
}
#else

extern G_MODULE_EXPORT void atom_properties (GSimpleAction * action, GVariant * state, gpointer data);

/*!
  \fn G_MODULE_EXPORT void show_hide_atoms (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief handle the show/hide signal GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_atoms (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  tint * the_data = (tint *) data;
  int j = the_data -> b;
  int k = the_data -> c;
  int l, m;
  gboolean doit = TRUE;
  gboolean show;
  project * this_proj = get_project_by_id (the_data -> a);
  GVariant * state;
  if (action)
  {
    state = g_action_get_state (G_ACTION (action));
    show = ! g_variant_get_boolean (state);
    const gchar * atom = g_action_get_name ((GAction *)action);
    int lgt = strlen (atom);
    gchar * name = g_strdup_printf ("%c%c", atom[lgt-2], atom[lgt-1]);
    if (g_strcmp0(name, ".1") == 0)
    {
      g_free (name);
      name = g_strdup_printf ("%.*s.0", lgt-2, atom);
      g_action_group_activate_action ((GActionGroup *)this_proj -> modelgl -> action_group, (const gchar *)name, NULL);
      g_free (name);
      doit = FALSE;
    }
  }
  else
  {
    show = this_proj -> modelgl -> anim -> last -> img -> show_atom[j][k];
  }
  if (doit)
  {
    for (l=0; l<this_proj -> steps; l++)
    {
      for (m=0; m<this_proj -> natomes; m++)
      {
        if (this_proj -> atoms[l][m].sp == k) this_proj -> atoms[l][m].show[j] = show;
      }
    }
    this_proj -> modelgl -> anim -> last -> img -> show_atom[j][k] = show;
    init_default_shaders (this_proj -> modelgl);
    if (action)
    {
      g_action_change_state (G_ACTION (action), g_variant_new_boolean (show));
      g_variant_unref (state);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void show_hide_labels (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief handle the show/hide label signal GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_labels (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  tint * the_data = (tint *) data;
  int j = the_data -> b;
  int k = the_data -> c;
  gboolean doit = TRUE;
  gboolean show;
  project * this_proj = get_project_by_id (the_data -> a);
  GVariant * state;
  if (action)
  {
    state = g_action_get_state (G_ACTION (action));
    show = ! g_variant_get_boolean (state);
    const gchar * label = g_action_get_name ((GAction *)action);
    int lgt = strlen (label);
    gchar * name = g_strdup_printf ("%c%c", label[lgt-2], label[lgt-1]);
    if (g_strcmp0(name, ".1") == 0)
    {
      g_free (name);
      name = g_strdup_printf ("%.*s.0", lgt-2, label);
      g_action_group_activate_action ((GActionGroup *)this_proj -> modelgl -> action_group, (const gchar *)name, NULL);
      g_free (name);
      doit = FALSE;
    }
  }
  else
  {
    show = ! this_proj -> modelgl -> anim -> last -> img -> show_label[j][k];
  }
  if (doit)
  {
    int l, m;
    for (l=0; l<this_proj -> steps; l++)
    {
      for (m=0; m<this_proj -> natomes; m++)
      {
        if (this_proj -> atoms[l][m].sp == k)
        {
          if (this_proj -> atoms[l][m].label[j] != show)
          {
            this_proj -> atoms[l][m].label[j] = show;
          }
        }
      }
    }
    this_proj -> modelgl -> labelled = check_label_numbers (this_proj, j);
    this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
    update (this_proj -> modelgl);
    if (action)
    {
      g_action_change_state (G_ACTION (action), g_variant_new_boolean (show));
      g_variant_unref (state);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void show_hide_all_atom_labels (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief handle the show/hide all labels GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_all_atom_labels (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  // Neither check, nor radio
  tint * id = (tint *) data;
  int i, j, k;
  project * this_proj = get_project_by_id(id -> a);
  i = id -> b;
  gboolean show = ! this_proj -> atoms[0][0].label[i];
  for (j=0; j<this_proj -> steps; j++)
  {
    for (k=0; k<this_proj -> natomes; k++)
    {
      this_proj -> atoms[j][k].label[i] = show;
    }
  }
  this_proj -> modelgl -> labelled = check_label_numbers (this_proj, i);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  update (this_proj -> modelgl);
  update_menu_bar (this_proj -> modelgl);
}

/*!
  \fn GMenu * label_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive)

  \brief create the 'Atom(s) -> Label(s)' submenu elements

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param at atoms (0) or clones (1)
  \param sensitive menu item sensitivity
*/
GMenu * label_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive)
{
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, "Show/Hide All", (at) ? "clones-labels-all" : "atoms-labels-all", popm, popm, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(show_hide_all_atom_labels), & view -> colorp[at][0], FALSE, FALSE, FALSE, sensitive);
  GMenu * smenu = g_menu_new ();
  project * this_proj = get_project_by_id (view -> proj);
  gchar * str;
  int i;
  for (i=0; i< this_proj -> nspec; i++)
  {
    if (at == 0)
    {
      str = g_strdup_printf ("%s", this_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("%s*", this_proj -> chemistry -> label[i]);
    }
    append_opengl_item (view, smenu, str, (! at) ? "atom-label" : "clone-label", popm, i, NULL, IMG_NONE, NULL,
                        FALSE, G_CALLBACK(show_hide_labels), & view -> colorp[at][i],
                        TRUE, view -> anim -> last -> img -> show_label[at][i], FALSE, sensitive);
  }
  append_submenu (menu, "Show", smenu);
  append_opengl_item (view, menu, "Select atom(s)", (! at) ? "atom-select" : "clone-select", popm, i, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(atom_properties), & view -> colorp[at][2], FALSE, FALSE, FALSE, sensitive);
  append_opengl_item (view, menu, "Advanced", (! at) ? "atom-lab-adv" : "clone-lab-adv", popm, i, NULL, IMG_STOCK, DPROPERTIES,
                      FALSE, G_CALLBACK(atom_properties), & view -> colorp[at][1], FALSE, FALSE, FALSE, sensitive);
  return menu;
}

/*!
  \fn GMenu * color_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive)

  \brief create the 'Atom(s) -> Color(s)' submenu elements

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param at atoms (0) or clones (1)
  \param sensitive menu item sensitivity
*/
GMenu * color_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive)
{
  GMenu * menu = g_menu_new ();
  GMenu * menuc;
  project * this_proj = get_project_by_id (view -> proj);
  gchar * stra, * strb;
  int i;
  for (i=0; i< this_proj -> nspec; i++)
  {
    if (at == 0)
    {
      stra = g_strdup_printf ("%s", this_proj -> chemistry -> label[i]);
    }
    else
    {
      stra = g_strdup_printf ("%s*", this_proj -> chemistry -> label[i]);
    }
    strb = g_strdup_printf ("%s", (! at) ? "atom-color" : "clone-color");
    menuc = g_menu_new ();
    append_opengl_item (view, menuc, strb, strb, popm, i, NULL, IMG_NONE, NULL, TRUE, NULL, NULL, FALSE, FALSE, FALSE, FALSE);
    append_opengl_item (view, menuc, "More colors ...", strb, popm, i, NULL, IMG_NONE, NULL,
                        FALSE, G_CALLBACK(to_run_atom_color_window), & view -> colorp[0][i+at*this_proj -> nspec], FALSE, FALSE, FALSE, sensitive);
    append_submenu (menu, stra, menuc);
    g_free (stra);
    g_free (strb);
    g_object_unref (menuc);
  }
  return menu;
}

/*!
  \fn GMenu * show_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive)

  \brief create the 'Atom(s) -> Show' submenu elements

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param at atoms (0) or clones (1)
  \param sensitive menu item sensitivity
*/
GMenu * show_atoms_submenu (glwin * view, int popm, int at, gboolean sensitive)
{
  GMenu * menu = g_menu_new ();
  gchar * str;
  project * this_proj = get_project_by_id (view -> proj);
  int i;
  for (i=0; i<this_proj -> nspec; i++)
  {
    if (at == 0)
    {
      str = g_strdup_printf ("%s", this_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("%s*", this_proj -> chemistry -> label[i]);
    }
    append_opengl_item (view, menu, str, (! at) ? "show-atom" : "show-clone", popm, i, NULL, IMG_NONE, NULL,
                        FALSE, G_CALLBACK(show_hide_atoms), & view -> colorp[at][i],
                        TRUE, view -> anim -> last -> img -> show_atom[at][i], FALSE, sensitive);
    g_free (str);
  }
  return menu;
}

/*!
  \fn GMenu * menu_atoms (glwin * view, int popm, int at)

  \brief create the 'Atom(s)' submenu elements

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param at atoms (0) or clones (1)
*/
GMenu * menu_atoms (glwin * view, int popm, int at)
{
  int i = view -> anim -> last -> img -> style;
  gboolean sensitive = (at) ? view -> anim -> last -> img -> draw_clones : TRUE;

  GMenu * menu = g_menu_new ();
  append_submenu (menu, "Show", show_atoms_submenu(view, popm, at, sensitive));
  append_submenu (menu, "Color(s)", color_atoms_submenu (view, popm, at, sensitive));
  append_submenu (menu, "Label(s)", label_atoms_submenu (view, popm, at, sensitive));
  GMenuItem * item;
  if (i == SPHERES || i == BALL_AND_STICK)
  {
    item = g_menu_item_new ("Radius(ii)", (sensitive) ? NULL : "None");
    g_menu_item_set_attribute (item, "custom", "s", (at) ? "clone-radii" : "atom-radii", NULL);
    g_menu_append_item (menu, item);
  }
  if (i == WIREFRAME || i == PUNT)
  {
    item = g_menu_item_new ("Dot size(s)", (sensitive) ? NULL : "None");
    g_menu_item_set_attribute (item, "custom", "s", (at) ? "clone-pts" : "atom-pts", NULL);
    g_menu_append_item (menu, item);
  }
  append_opengl_item (view, menu, "Advanced", (! at) ? "atom-advanced" : "clone-advanced", popm, popm, NULL, IMG_STOCK, DPROPERTIES,
                      FALSE, G_CALLBACK(atom_properties), & view -> colorp[at][0],
                      FALSE, FALSE, FALSE, sensitive);
  return menu;
}
#endif
