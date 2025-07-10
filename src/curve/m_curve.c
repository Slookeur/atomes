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
* @file m_curve.c
* @short Curve context menu (mouse right click)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_curve.c'
*
* Contains:
*

 - The curve context menu (mouse right click)

*
* List of functions:

  gboolean was_not_added (ExtraSets * sets, int a, int b, int c);

  void autoscale (gpointer data);
  void action_to_plot (pointer data);
  void add_extra (ExtraSets * sets, tint * id);
  void remove_extra (ExtraSets * sets, CurveExtra * ctmp);
  void prep_extra_rid (tint * data);
  void curve_window_add_menu_bar (tint * data);

  G_MODULE_EXPORT void curve_edit_menu_action (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void curve_add_remove_menu_action (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void curve_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * curve_popup_menu (gpointer data);

  GMenu * curve_section (GSimpleActionGroup * action_group, gchar * act, ExtraSets * sets, gboolean add, int edit, int a, int b, tint * data);
  GMenu * create_curve_submenu (GSimpleActionGroup * action_group, gchar * act, tint * data, gboolean add, int edit);
  GMenu * create_curve_menu (gchar * str);
  GMenu * edit_data_section (GSimpleActionGroup * action_group, gchar * str, tint * data);
  GMenu * curve_close_section (gchar * str);
  GMenu * create_data_menu (GSimpleActionGroup * action_group, int pop, gchar * str, tint * data);
  GMenu * curve_menu_bar (project * this_proj, GSimpleActionGroup * action_group, gchar * str, tint * data);
  GMenu * create_add_remove_section (GSimpleActionGroup * action_group, gchar * act, int num, tint * data);
  GMenu * autoscale_section (gchar * str);

  CurveExtra * init_extra (tint * id);

*/

#include <gtk/gtk.h>
#include <stdlib.h>

#include "global.h"
#include "callbacks.h"
#include "curve.h"
#include "cedit.h"
#include "datab.h"

extern DataLayout * curve_default_layout (project * pid, int rid, int cid);
extern GtkWidget * shortcuts_window (int sections, int group_by_section[sections], int groups, int shortcut_by_group[groups],
                                     gchar * section_names[sections], gchar * group_names[groups], shortcuts shortcs[]);
int ** extrarid;
// gchar * curve_act[3]={"edit", "add", "rem"};

gchar * curve_section_names[]={ "Curve window"};
int curve_group_by_section[] = { 2 };
gchar * curve_group_names[]={"Edition", "Export"};
int curve_shortcut_by_group[] = { 3, 2 };

shortcuts curve_shortcuts[] = {
//
// Global
//
  { "Autoscale", "Autoscale", GDK_KEY_a, "<Ctrl>a" },
  { "Edit curve", "Edit curve", GDK_KEY_e, "<Ctrl>e" },
  { "Close curve", "Close curve", GDK_KEY_c, "<Ctrl>c" },
  { "Export data", "Export data", GDK_KEY_s, "<Ctrl>s" },
  { "Save image", "Save image", GDK_KEY_i, "<Ctrl>i" }
};

/*!
  \fn void autoscale (gpointer data)

  \brief autoscale callback

  \param data the associated data pointer
*/
void autoscale (gpointer data)
{
  tint * id = (tint *)data;
  get_project_by_id(id -> a) -> curves[id -> b][id -> c] -> autoscale[0] = TRUE;
  get_project_by_id(id -> a) -> curves[id -> b][id -> c] -> autoscale[1] = TRUE;
  update_curve (data);
}

/*!
  \fn CurveExtra * init_extra (tint * id)

  \brief create extra data set

  \param id the associated data pointer
*/
CurveExtra * init_extra (tint * id)
{
  CurveExtra * ctmp = g_malloc0 (sizeof*ctmp);
  ctmp -> id.a = id -> a;
  ctmp -> id.b = id -> b;
  ctmp -> id.c = id -> c;
  ctmp -> layout = curve_default_layout (get_project_by_id(id -> a), id -> b, id -> c);
  ctmp -> layout -> datacolor.red = 0.0;
  ctmp -> layout -> datacolor.green = 0.0;
  ctmp -> layout -> datacolor.blue = 0.0;
  return ctmp;
}

/*!
  \fn void add_extra (ExtraSets * sets, tint * id)

  \brief add set to the extra set(s)

  \param sets the extra set(s)
  \param id the associated data pointer
*/
void add_extra (ExtraSets * sets, tint * id)
{
  if (sets -> extras == 0)
  {
    sets -> first = init_extra (id);
    sets -> last = sets -> first;
  }
  else
  {
    sets -> last -> next = init_extra (id);
    sets -> last -> next -> prev = sets -> last;
    sets -> last = sets -> last -> next;
  }
  sets -> extras ++;
}

/*!
  \fn void remove_extra (ExtraSets * sets, CurveExtra * ctmp)

  \brief remove data from extra set(s)

  \param sets the extra set(s)
  \param ctmp the data set to remove from the extra set(s)
*/
void remove_extra (ExtraSets * sets, CurveExtra * ctmp)
{
  if (sets -> extras == 1)
  {
    g_free (ctmp);
    sets -> first = NULL;
    sets -> last = NULL;
  }
  else
  {
    if (ctmp -> prev == NULL)
    {
      sets -> first = ctmp -> next;
      g_free (ctmp);
      sets -> first -> prev = NULL;
    }
    else if (ctmp -> next == NULL)
    {
      ctmp = ctmp -> prev;
      g_free (ctmp -> next);
      ctmp -> next = NULL;
      sets -> last = ctmp;
    }
    else
    {
      ctmp -> prev -> next = ctmp -> next;
      ctmp -> next -> prev = ctmp -> prev;
      g_free (ctmp);
    }
  }
  sets -> extras --;
}

void curve_window_add_menu_bar (tint * data);

/*!
  \fn void prep_extra_rid (tint * data)

  \brief  look up extra data set(s) for curve

  \param data the associated data pointer
*/
void prep_extra_rid (tint * data)
{
  int i;
  extrarid = allocdint (nprojects, NGRAPHS);
  project * this_proj = get_project_by_id (data -> a);
  if (this_proj -> curves[data -> b][data -> c] -> extrac -> extras > 0)
  {
    CurveExtra * ctmp = this_proj -> curves[data -> b][data -> c] -> extrac -> first;
    for (i=0; i<this_proj -> curves[data -> b][data -> c] -> extrac -> extras; i++)
    {
      extrarid[ctmp -> id.a][ctmp -> id.b] ++;
      if (ctmp -> next != NULL) ctmp = ctmp -> next;
    }
  }
}

/*!
  \fn void action_to_plot (gpointer data)

  \brief add to plot, or, remove curve from plot

  \param data the associated data pointer
*/
void action_to_plot (gpointer data)
{
  int i;
  tint * id = (tint *)data;
  gboolean remove = FALSE;
  project * this_proj = get_project_by_id (activeg);
  if (this_proj -> curves[activer][activec] -> extrac > 0)
  {
    CurveExtra * ctmp = this_proj -> curves[activer][activec] -> extrac -> first;
    for (i=0; i<this_proj -> curves[activer][activec] -> extrac -> extras; i++)
    {
      if (ctmp -> id.a == id -> a && ctmp -> id.b == id -> b && ctmp -> id.c == id -> c)
      {
        remove = TRUE;
        break;
      }
      if (ctmp -> next != NULL) ctmp = ctmp -> next;
    }
    if (! remove)
    {
      add_extra (this_proj -> curves[activer][activec] -> extrac, id);
    }
    else
    {
      remove_extra (this_proj -> curves[activer][activec] -> extrac, ctmp);
    }
  }
  else
  {
    add_extra (this_proj -> curves[activer][activec] -> extrac, id);
  }
  curve_window_add_menu_bar (& this_proj -> idcc[activer][activec]);
  update_curve ((gpointer)& this_proj -> idcc[activer][activec]);
}

/*!
  \fn G_MODULE_EXPORT void curve_edit_menu_action (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief curve menu edit action callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void curve_edit_menu_action (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  edit_data (data);
}

/*!
  \fn G_MODULE_EXPORT void curve_add_remove_menu_action (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief curve menu add / remove action callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void curve_add_remove_menu_action (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  action_to_plot (data);
}

/*!
  \fn G_MODULE_EXPORT void curve_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief curve menu action callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void curve_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  gchar * name = g_strdup_printf ("%s", g_action_get_name(G_ACTION(action)));
  if (g_strcmp0 (name, "save.data") == 0)
  {
    write_curve (data);
  }
  else if (g_strcmp0 (name, "close.curve") == 0)
  {
    hide_curve (data);
  }
  else if (g_strcmp0 (name, "edit.curve") == 0)
  {
    edit_curve (data);
  }
  else if (g_strcmp0 (name, "save.image") == 0)
  {
    save_image (data);
  }
  else if (g_strcmp0 (name, "autoscale.curve") == 0)
  {
    autoscale (data);
  }
  else if (g_strcmp0 (name, "shortcuts.curve") == 0)
  {
    tint * id = (tint *)data;
    get_project_by_id(id -> a) -> curves[id -> b][id -> c] -> shortcuts = destroy_this_widget (get_project_by_id(id -> a) -> curves[id -> b][id -> c] -> shortcuts);
    get_project_by_id(id -> a) -> curves[id -> b][id -> c] -> shortcuts = shortcuts_window (G_N_ELEMENTS(curve_group_by_section), curve_group_by_section, G_N_ELEMENTS(curve_shortcut_by_group),
                                                                                            curve_shortcut_by_group, curve_section_names, curve_group_names, curve_shortcuts);
  }
}

/*!
  \fn gboolean was_not_added (ExtraSets * sets, int a, int b, int c)

  \brief test if already in the menu or not

  \param sets the target extra sets
  \param a the target project
  \param b the target calculation
  \param c the target curve
*/
gboolean was_not_added (ExtraSets * sets, int a, int b, int c)
{
  int i, j;
  CurveExtra * ctmp = sets -> first;
  for (i=0; i<sets -> extras; i++)
  {
    if (ctmp -> id.a == a && ctmp -> id.b == b)
    {
      for (j=0; j<get_project_by_id(a) -> numc[b]; j++)
      {
        if (ctmp -> id.c == c) return FALSE;
      }
    }
    if (ctmp -> next != NULL) ctmp = ctmp -> next;
  }
  return TRUE;
}

/*!
  \fn GMenu * curve_section (GSimpleActionGroup * action_group, gchar * act, ExtraSets * sets, gboolean add, int edit, int a, int b, tint * data)

  \brief create curve submenu

  \param action_group the menu action group
  \param act the action string
  \param sets the target extra sets
  \param add remove (0) or add (1) menu
  \param edit data edition (1 / 0)
  \param a target project
  \param b target calculation
  \param data the associated data pointer
*/
GMenu * curve_section (GSimpleActionGroup * action_group, gchar * act, ExtraSets * sets, gboolean add, int edit, int a, int b, tint * data)
{
  GMenu * menu = g_menu_new ();
  gchar * str_a, * str_b, * str_c;
  gchar * text[2] = {"curve.action", "edit.data"};
  project * this_proj = get_project_by_id(a);
  int i;
  for (i=0; i<this_proj -> numc[b]; i++)
  {
    if (this_proj -> curves[b][i] -> ndata > 0)
    {
      if (((a != data -> a || b != data -> b || i != data -> c) && add == was_not_added(sets, a, b, i)) || (a == data -> a && b == data -> b && i == data -> c && edit))
      {
        str_a = g_strdup_printf ("%s", this_proj -> curves[b][i] -> name);
        str_b = g_strdup_printf ("%s.%d-%d-%d", text[edit], a, b, i);
        str_c = g_strdup_printf ("%s.%s", act, str_b);
        append_menu_item (menu, (const gchar *)str_a, (const gchar *)str_c, NULL, NULL, IMG_NONE, NULL, FALSE, FALSE, FALSE, NULL);
        g_free (str_a);
        g_free (str_c);
        if (edit)
        {
          widget_add_action (action_group, (const gchar *)str_b, G_CALLBACK(curve_edit_menu_action), & this_proj -> idcc[b][i], FALSE, FALSE, FALSE, NULL);
        }
        else
        {
          widget_add_action (action_group, (const gchar *)str_b, G_CALLBACK(curve_add_remove_menu_action), & this_proj -> idcc[b][i], FALSE, FALSE, FALSE, NULL);
        }
        g_free (str_b);
      }
    }
  }
  return menu;
}

/*!
  \fn GMenu * create_curve_submenu (GSimpleActionGroup * action_group, gchar * act, tint * data, gboolean add, int edit)

  \brief create curve add / remove submenus

  \param action_group the menu action group
  \param act the action string
  \param data the associated data pointer
  \param add remove (0) or add (1) menu
  \param edit data edition (1 / 0)
*/
GMenu * create_curve_submenu (GSimpleActionGroup * action_group, gchar * act, tint * data, gboolean add, int edit)
{
  project * this_proj;
  GMenu * menu = g_menu_new ();
  int i, j, k;
  gboolean * create_proj = allocbool (nprojects);
  gboolean ** create_menu = allocdbool (nprojects, NCALCS);
  for (i=0; i<nprojects; i++)
  {
    this_proj = get_project_by_id(i);
    create_menu[i][data -> b] = FALSE;
    create_proj[i] = FALSE;
    j = 0;
    if (data -> b == GR || data -> b == GK)
    {
      j = 1;
      k = (data -> b == GR) ? GK : GR;
    }
    else if (data -> b == SQ || data -> b == SK)
    {
      j = 1;
      k = (data -> b == SQ) ? SK : SQ;
    }

    if (((add && extrarid[i][data -> b] < this_proj -> numc[data -> b])
    || (! add && extrarid[i][data -> b] > 0)) && this_proj -> visok[data -> b])
    {
      create_menu[i][data -> b] = TRUE;
      create_proj[i] = TRUE;
    }

    if (j && this_proj -> visok[k])
    {
      create_menu[i][k] = FALSE;
      if (this_proj -> curves[k][0] -> ndata > 0)
      {
        if ((add && extrarid[i][k] < this_proj -> numc[k])
        || (! add && extrarid[i][k] > 0))
        {
          create_menu[i][k] = TRUE;
          create_proj[i] = TRUE;
        }
      }
    }
  }
  if (edit)
  {
    create_menu[data -> a][data -> b] = TRUE;
    create_proj[data -> a] = TRUE;
  }
  this_proj = get_project_by_id(data -> a);
  GMenu * smenu;
  for (i=0; i<nprojects; i++)
  {
    if (create_proj[i])
    {
      smenu = g_menu_new ();
      if (create_menu[i][data -> b]) append_submenu (smenu, graph_name[data -> b], curve_section(action_group, act, this_proj -> curves[data -> b][data -> c] -> extrac, add, edit, i, data -> b, data));
      if (j && create_menu[i][k]) append_submenu (smenu, graph_name[k], curve_section(action_group, act, this_proj -> curves[data -> b][data -> c] -> extrac, add, edit, i, k, data));
      // This way GTK3 will not be able to apply markup
      append_submenu (menu, get_project_by_id(i) -> name, smenu);
      g_object_unref (smenu);
    }
  }
  g_free (create_proj);
  g_free (create_menu);
  return menu;
}

extern GIcon * get_gicon_from_data (int format, const gchar * icon);

/*!
  \fn GMenu * create_curve_menu (gchar * str)

  \brief create the curve submenu

  \param str the action string
*/
GMenu * create_curve_menu (gchar * str)
{
  GMenu * menu = g_menu_new ();
  gchar * act = g_strdup_printf ("%s.edit.curve", str);
  append_menu_item (menu, "Edit Curve", (const gchar *)act, "<CTRL>E", NULL, IMG_STOCK, PAGE_SETUP, FALSE, FALSE, FALSE, NULL);
  g_free (act);
  act = g_strdup_printf ("%s.save.image", str);
  append_menu_item (menu, "Export Image", (const gchar *)act, "<CTRL>I", NULL, IMG_FILE, PACKAGE_IMG, FALSE, FALSE, FALSE, NULL);
  g_free (act);
  return menu;
}

/*!
  \fn GMenu * edit_data_section (GSimpleActionGroup * action_group, gchar * str, tint * data)

  \brief create the edit data submenu

  \param action_group the menu action group
  \param str the action string
  \param data the associated data pointer
*/
GMenu * edit_data_section (GSimpleActionGroup * action_group, gchar * str, tint * data)
{
  GMenu * menu = g_menu_new ();
  GMenuItem * item = g_menu_item_new ("Edit Data", NULL);
  gchar * str_edit = g_strdup_printf ("%s-win-edit", str);
  g_menu_item_set_attribute (item, "custom", "s", str_edit, NULL);
  g_free (str_edit);
#ifdef MENU_ICONS
  GIcon * gicon = get_gicon_from_data (IMG_STOCK, EDITA);
  g_menu_item_set_icon (item, gicon);
  g_object_unref (gicon);
#endif
  g_menu_item_set_submenu (item, (GMenuModel *)create_curve_submenu (action_group, str, data, FALSE, 1));
  g_menu_append_item (menu, item);
  g_object_unref (item);
  return menu;
}

/*!
  \fn GMenu * curve_close_section (gchar * str)

  \brief create the close menu item

  \param str the action string
*/
GMenu * curve_close_section (gchar * str)
{
  GMenu * menu = g_menu_new ();
  gchar * act = g_strdup_printf ("%s.close.curve", str);
  append_menu_item (menu, "Close", (const gchar *)act, "<CTRL>C", NULL, IMG_STOCK, FCLOSE, FALSE, FALSE, FALSE, NULL);
  g_free (act);
  return menu;
}

/*!
  \fn GMenu * curve_help_menu (gchar * str)

  \brief create the help menu item

  \param str the action string
*/
GMenu * curve_help_menu (gchar * str)
{
  GMenu * menu = g_menu_new ();
  gchar * act = g_strdup_printf ("%s.shortcuts.curve", str);
  append_menu_item (menu, "Shortcuts", (const gchar *)act, NULL, NULL, IMG_NONE, NULL, FALSE, FALSE, FALSE, NULL);
  g_free (act);
  return menu;
}

/*!
  \fn GMenu * create_data_menu (GSimpleActionGroup * action_group, int pop, gchar * str, tint * data)

  \brief create the save data submenu

  \param action_group the menu action group
  \param pop 0 = menu bar, 1 = popup menu
  \param str the action string
  \param data the associated data pointer
*/
GMenu * create_data_menu (GSimpleActionGroup * action_group, int pop, gchar * str, tint * data)
{
  GMenu * menu = g_menu_new ();
  g_menu_append_section (menu, NULL, (GMenuModel*)edit_data_section(action_group, str, data));
  gchar * act = g_strdup_printf ("%s.save.data", str);
  append_menu_item (menu, "Save Data", (const gchar *)act, "<CTRL>S", NULL, IMG_STOCK, FSAVEAS, FALSE, FALSE, FALSE, NULL);
  g_free (act);
  if (! pop) g_menu_append_section (menu, NULL, (GMenuModel *)curve_close_section(str));
  return menu;
}

/*!
  \fn GMenu * curve_menu_bar (project * this_proj, GSimpleActionGroup * action_group, gchar * str, tint * data)

  \brief create the curve window menu bar

  \param this_proj the target project
  \param action_group the menu action group
  \param str the action string
  \param data the associated data pointer
*/
GMenu * curve_menu_bar (project * this_proj, GSimpleActionGroup * action_group, gchar * str, tint * data)
{
  GMenu * menu = g_menu_new ();
  prep_extra_rid (data);
  append_submenu (menu, "Data", create_data_menu(action_group, 0, str, data));
  g_free (extrarid);
  append_submenu (menu, "Curve", create_curve_menu(str));
  append_submenu (menu, "Help", curve_help_menu(str));
  return menu;
}

/*!
  \fn void curve_window_add_menu_bar (tint * data)

  \brief add menu bar to the curve window

  \param data the associated data pointer
*/
void curve_window_add_menu_bar (tint * data)
{
  project * this_proj = get_project_by_id (data -> a);
  this_proj -> curves[data -> b][data -> c] -> pos = destroy_this_widget (this_proj -> curves[data -> b][data -> c] -> pos);
  this_proj -> curves[data -> b][data -> c] -> pos = gtk_label_new (" ");
  this_proj -> curves[data -> b][data -> c] -> curve_hbox = destroy_this_widget (this_proj -> curves[data -> b][data -> c] -> curve_hbox);
  this_proj -> curves[data -> b][data -> c] -> curve_hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> curves[data -> b][data -> c] -> curve_vbox, this_proj -> curves[data -> b][data -> c] -> curve_hbox, FALSE, FALSE, 0);
  gchar * str = g_strdup_printf ("c-%d", this_proj -> curves[data -> b][data -> c] -> action_id);
#ifdef GTK3
  GtkWidget * menu = gtk_menu_bar_new_from_model ((GMenuModel *)curve_menu_bar(this_proj, this_proj -> curves[data -> b][data -> c] -> action_group, str, data));
#else
  GtkWidget * menu = gtk_popover_menu_bar_new_from_model ((GMenuModel *)curve_menu_bar(this_proj, this_proj -> curves[data -> b][data -> c] -> action_group, str, data));
#endif
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, this_proj -> curves[data -> b][data -> c] -> curve_hbox, menu, TRUE, TRUE, 0);
  gtk_label_align (this_proj -> curves[data -> b][data -> c] -> pos, 1.0, 0.5);
  add_box_child_end (this_proj -> curves[data -> b][data -> c] -> curve_hbox, this_proj -> curves[data -> b][data -> c] -> pos, FALSE, FALSE, 0);
  show_the_widgets (this_proj -> curves[data -> b][data -> c] -> curve_hbox);
}

/*!
  \fn GMenu * create_add_remove_section (GSimpleActionGroup * action_group, gchar * act, int num, tint * data)

  \brief create the add / remove curve submenus

  \param action_group the menu action group
  \param act the action string
  \param num the number of curve(s) to add
  \param data the associatec data pointer
*/
GMenu * create_add_remove_section (GSimpleActionGroup * action_group, gchar * act, int num, tint * data)
{
  GMenu * menu = g_menu_new ();
  GMenuItem * item = g_menu_item_new ("Add Data Set", NULL);
#ifdef MENU_ICONS
  GIcon * gicon = get_gicon_from_data (IMG_STOCK, LIST_ADD);
  g_menu_item_set_icon (item, gicon);
  g_object_unref (gicon);
#endif
  project * this_proj = get_project_by_id (data -> a);
  if (this_proj -> curves[data -> b][data -> c] -> extrac -> extras < num)
  {
    g_menu_item_set_submenu (item, (GMenuModel *)create_curve_submenu (action_group, act, data, TRUE, 0));
  }
  else
  {
    g_menu_item_set_attribute (item, "action", "s", "None", NULL);
  }
  g_menu_append_item (menu, item);
  g_object_unref (item);

  item = g_menu_item_new ("Remove Data Set", NULL);
#ifdef MENU_ICONS
  gicon = get_gicon_from_data (IMG_STOCK, LIST_REM);
  g_menu_item_set_icon (item, gicon);
  g_object_unref (gicon);
#endif
  if (this_proj -> curves[data -> b][data -> c] -> extrac -> extras > 0)
  {
    g_menu_item_set_submenu (item, (GMenuModel *)create_curve_submenu (action_group, act, data, FALSE, 0));
  }
  else
  {
    g_menu_item_set_attribute (item, "action", "s", "None", NULL);
  }
  g_menu_append_item (menu, item);
  g_object_unref (item);
  return menu;
}

/*!
  \fn GMenu * autoscale_section (gchar * str)

  \brief create autoscale menu item

  \param str the action string id
*/
GMenu * autoscale_section (gchar * str)
{
  GMenu * menu = g_menu_new ();
  gchar * act = g_strdup_printf ("%s.autoscale.curve", str);
  append_menu_item (menu, "Autoscale", (const gchar *)act, "<CTRL>A", NULL, IMG_STOCK, FITBEST, FALSE, FALSE, FALSE, NULL);
  g_free (act);
  return menu;
}

/*!
  \fn GtkWidget * curve_popup_menu (gpointer data)

  \brief create curve popup menu

  \param data the associated data pointer
*/
GtkWidget * curve_popup_menu (gpointer data)
{
  GtkWidget * curve_pop_menu;
  int i, j;
  CurveState * cstate = (CurveState *)data;
  GSimpleActionGroup * curve_popup_actions = g_simple_action_group_new ();
  GSimpleAction * curve_popup_action[6];
  curve_popup_action[0] = g_simple_action_new ("save.data", NULL);
  curve_popup_action[1] = g_simple_action_new ("close.curve", NULL);
  curve_popup_action[2] = g_simple_action_new ("edit.curve", NULL);
  curve_popup_action[3] = g_simple_action_new ("save.image", NULL);
  curve_popup_action[4] = g_simple_action_new ("autoscale.curve", NULL);
  curve_popup_action[5] = g_simple_action_new ("shortcuts.curve", NULL);
  for (i=0; i<6; i++)
  {
    g_action_map_add_action (G_ACTION_MAP(curve_popup_actions), G_ACTION(curve_popup_action[i]));
    g_signal_connect (curve_popup_action[i], "activate", G_CALLBACK(curve_menu_bar_action), cstate -> id);
  }
  prep_extra_rid (cstate -> id);
  gchar * str = g_strdup_printf ("mc-%d", get_project_by_id(cstate -> id -> a) -> curves[cstate -> id -> b][cstate -> id -> c] -> action_id);
  GMenu * menu = g_menu_new ();
  g_menu_append_section (menu, NULL, (GMenuModel *)create_data_menu(curve_popup_actions, 1, str, cstate -> id));
  g_menu_append_section (menu, NULL, (GMenuModel *)create_curve_menu(str));
  i = -1;
  for ( j=0 ; j < nprojects; j++ )
  {
    i += get_project_by_id(j) -> numc[cstate -> id -> b];
    if (cstate -> id -> b == GR || cstate -> id -> b == GK)
    {
      i += get_project_by_id(j) -> numc[(cstate -> id -> b == GR) ? GK : GR];
    }
    else if (cstate -> id -> b == SQ || cstate -> id -> b == SK)
    {
      i += get_project_by_id(j) -> numc[(cstate -> id -> b == SQ) ? SK : SQ];
    }
  }
  g_menu_append_section (menu, NULL, (GMenuModel *)create_add_remove_section(curve_popup_actions, str, i, cstate -> id));
  g_menu_append_section (menu, NULL, (GMenuModel *)autoscale_section(str));
  g_menu_append_section (menu, NULL, (GMenuModel *)curve_close_section(str));
  g_menu_append_section (menu, NULL, (GMenuModel *)curve_help_menu(str));
  g_free (extrarid);
#ifdef GTK4
  curve_pop_menu = gtk_popover_menu_new_from_model_full ((GMenuModel *)menu, GTK_POPOVER_MENU_NESTED);
  gtk_widget_set_parent (curve_pop_menu,  get_project_by_id(cstate -> id -> a) -> curves[cstate -> id -> b][cstate -> id -> c] -> window);
  gtk_widget_set_size_request (curve_pop_menu, -1, 305);
#else
  curve_pop_menu = gtk_menu_new_from_model ((GMenuModel *)menu);
#endif
  // Finally adding actions to the menu
  gtk_widget_insert_action_group (curve_pop_menu, str, G_ACTION_GROUP(curve_popup_actions));
  g_free (str);
  return curve_pop_menu;
}
