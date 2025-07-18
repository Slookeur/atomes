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
* @file m_box.c
* @short Functions to create the 'Model -> Box' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_box.c'
*
* Contains:
*

 - The functions to create the 'Model -> Box' submenu

*
* List of functions:

  void update_show_hide_box_axis (glwin * view, int box_or_axis, int status);

  G_MODULE_EXPORT void set_box_axis_style (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void show_hide_box_axis (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void change_box_axis_radio (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * create_box_axis_menu (char * name, int val, int box, GtkWidget * menu, tint * data);
  GtkWidget * create_color_widget (GtkWidget * widg, glwin * view, int va);
  GtkWidget * create_layout_widget (gchar * str, GtkWidget * menu, int vab, gpointer data);
  GtkWidget * menu_box_axis (glwin * view, int id, int ab);

  GMenuItem * menu_box_axis (glwin * view, int popm, int ab);

  GMenu * axis_box_style (glwin * view, int popm, int ab, int abs);
  GMenu * axis_box_param (glwin * view, int popm, int ab, int style);
  GMenuItem * menu_box_axis (glwin * view, int popm, int ab);

*/

#include "global.h"
#include "glview.h"
#include "glwindow.h"
#include "color_box.h"
#include "submenus.h"


G_MODULE_EXPORT void box_advanced (GtkWidget * widg, gpointer data);
#ifdef GTK4
extern G_MODULE_EXPORT void window_bonds (GSimpleAction * action, GVariant * parameter, gpointer data);
#else
extern G_MODULE_EXPORT void window_bonds (GtkWidget * widg, gpointer data);
#endif

extern gboolean from_box_or_axis;

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void to_box_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief open box configuration window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_box_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  box_advanced (NULL, data);
}
#endif

/*!
  \fn void update_show_hide_box_axis (glwin * view, int box_or_axis, int status)

  \brief update edition buttons with menu show / hide status
*/
void update_show_hide_box_axis (glwin * view, int box_or_axis, int status)
{
  switch (box_or_axis)
  {
    case 0:
      if (view -> box_win)
      {
        if (view -> box_win -> show_hide && GTK_IS_WIDGET(view -> box_win -> show_hide))
        {
          button_set_status (view -> box_win -> show_hide, ! status);
        }
      }
      break;
    case 1:
      if (view -> axis_win)
      {
        if (view -> axis_win -> show_hide && GTK_IS_WIDGET(view -> axis_win -> show_hide))
        {
          button_set_status (view -> axis_win -> show_hide, ! status);
        }
      }
      break;
  }
}

#ifdef GTK3
// GTK3 Menu Action To Check
/*!
  \fn G_MODULE_EXPORT void set_box_axis_style (GtkWidget * widg, gpointer data)

  \brief set box/axis style

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_box_axis_style (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  int i, j, k, l, m, n;
  j = the_data -> b;
  n = the_data -> c;
  glwin * view = get_project_by_id(the_data -> a) -> modelgl;
  k = (n) ? view -> anim -> last -> img -> xyz -> axis : view -> anim -> last -> img -> abc -> box;

  int dim[2]={OGL_BOX, OGL_AXIS};
  l = (k == NONE) ? 2 : (j == WIREFRAME) ? 2 : 1;
  m = (k == NONE) ? 0 : (j == WIREFRAME) ? 1 : 2;
  if (k != j && gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    if (n)
    {
      view -> anim -> last -> img -> xyz -> axis = NONE - 1;
    }
    else
    {
      view -> anim -> last -> img -> abc -> box = NONE - 1;
    }
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[n][l], FALSE);
    show_the_widgets (view -> ogl_box_axis[n][3+2*(m-1)]);
    hide_the_widgets (view -> ogl_box_axis[n][3+2*(l-1)]);
    if (widg != view -> ogl_box_axis[n][(k == NONE) ? 1 : m])
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[n][(k == NONE) ? 1 : m], TRUE);
    }
    for (i=1; i<dim[n]; i++) widget_set_sensitive (view -> ogl_box_axis[n][i], 1);
    if (n)
    {
      view -> anim -> last -> img -> xyz -> axis = (k == NONE) ? WIREFRAME : j;
    }
    else
    {
     view -> anim -> last -> img -> abc -> box = (k == NONE) ? WIREFRAME : j;
    }

  }
  else if (k == j && ! gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[n][(k == NONE) ? 1 : m], TRUE);
  }
  else if (j == 0)
  {
    for (i=1; i<3; i++)
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[n][i], FALSE);
      widget_set_sensitive (view -> ogl_box_axis[n][i], 0);
      widget_set_sensitive (view -> ogl_box_axis[n][i], 0);
    }
    widget_set_sensitive (view -> ogl_box_axis[n][4], 0);
    for (i=6; i<dim[n]; i++)
    {
      widget_set_sensitive (view -> ogl_box_axis[n][i], 0);
    }
    if (widg != view -> ogl_box_axis[n][0]) gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[n][0], FALSE);
    if (n)
    {
      view -> anim -> last -> img -> xyz -> axis = NONE;
    }
    else
    {
      view -> anim -> last -> img -> abc -> box = NONE;
    }
  }
  if (n)
  {
    i = (view -> anim -> last -> img -> xyz -> axis == NONE) ? NONE : (view -> anim -> last -> img -> xyz -> axis == WIREFRAME) ? 0 : 1;
  }
  else
  {
    i = (view -> anim -> last -> img -> abc -> box == NONE) ? NONE : (view -> anim -> last -> img -> abc -> box == WIREFRAME) ? 0 : 1;
  }
  switch (n)
  {
    case 0:
      if (view -> box_win)
      {
        if (view -> box_win -> styles && GTK_IS_WIDGET(view -> box_win -> styles))
        {
          combo_set_active (view -> box_win -> styles, i);
        }
      }
      break;
    case 1:
      if (view -> axis_win)
      {
        if (view -> axis_win -> styles && GTK_IS_WIDGET(view -> axis_win -> styles))
        {
          combo_set_active (view -> axis_win -> styles, i);
        }
      }
      break;
  }
  if (! from_box_or_axis) update_show_hide_box_axis (view, n, (i == NONE) ? 1 : 0);
  view -> create_shaders[n+MDBOX] = TRUE;
  update (view);
}

/*!
  \fn GtkWidget * create_box_axis_menu (char * name, int val, int box, GtkWidget * menu, tint * data)

  \brief create a menu item for box/axis style

  \param name the new menu item label
  \param val box/axis active style
  \param box menu item style type
  \param menu the GtkMenu to attach the new item to
  \param data the pointer associated with the callback
*/
GtkWidget * create_box_axis_menu (char * name, int val, int box, GtkWidget * menu, tint * data)
{
  GtkWidget * box_widget = gtk3_menu_item (menu, name, IMG_NONE, NULL, G_CALLBACK(set_box_axis_style), data,
                                           FALSE, 0, 0, TRUE, (data -> b != 0) ? TRUE : FALSE, (box == val && val != NONE) ? TRUE : FALSE);
  if (data -> b != 0)
  {
    if (val == NONE)
    {
      widget_set_sensitive (box_widget, 0);
    }
  }
  return box_widget;
}

/*!
  \fn GtkWidget * create_color_widget (GtkWidget * widg, glwin * view, int va)

  \brief create box color selection menu item

  \param widg the GtkMenuItem to attach the color palette to
  \param view the target glwin
  \param va box style
*/
GtkWidget * create_color_widget (GtkWidget * widg, glwin * view, int va)
{
  GtkWidget * color_widget = color_box(view, -1, 0, 0);
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, color_widget);
  if (va == NONE)
  {
    widget_set_sensitive (color_widget, 0);
  }
  return color_widget;
}

/*!
  \fn GtkWidget * create_layout_widget (gchar * str, GtkWidget * menu, int vab, gpointer data)

  \brief create box/axis style menu widget

  \param str menu item label
  \param menu the GtkMenu to attach the new item to
  \param vab box/axis style
  \param data the associated data pointer
*/
GtkWidget * create_layout_widget (gchar * str, GtkWidget * menu, int vab, gpointer data)
{
  GtkWidget * layout = create_menu_item (TRUE, str);
  gtk_menu_shell_append ((GtkMenuShell *)menu, layout);
  g_signal_connect (G_OBJECT (layout), "activate", G_CALLBACK(window_bonds), data);
  if (vab == NONE) widget_set_sensitive (layout, 0);
  return layout;
}

/*!
  \fn GtkWidget * menu_box_axis (glwin * view, int id, int ab)

  \brief create the 'Model -> Box' and 'View -> Axis' submenus GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
  \param ab box (0) or axis (1)
*/
GtkWidget * menu_box_axis (glwin * view, int id, int ab)
{
  GtkWidget * widg;
  gchar * menu_title[2]={"Box", "Axis"};
  GtkWidget * ab_menu;
  GtkWidget * menu_ab = gtk_menu_new ();
  if (id == 0 && ab == 0)
  {
    view -> ogl_box[0] = create_menu_item (TRUE, menu_title[ab]);
    gtk_menu_item_set_submenu ((GtkMenuItem *)view -> ogl_box[0], menu_ab);
    widget_set_sensitive (view -> ogl_box[0], get_project_by_id(view -> proj) -> cell.ltype);
  }
  else
  {
    ab_menu = create_menu_item (TRUE, menu_title[ab]);
    gtk_menu_item_set_submenu ((GtkMenuItem *)ab_menu, menu_ab);
    if (ab == 0) widget_set_sensitive (ab_menu, get_project_by_id(view -> proj) -> cell.ltype);
  }

  GtkWidget * menul;
  int i = (ab) ? view -> anim -> last -> img -> xyz -> axis : view -> anim -> last -> img -> abc -> box;
  if (id == 0)
  {
    view -> ogl_box_axis[ab][0] = create_box_axis_menu ("Show/_Hide", i, i, menu_ab, & view -> colorp[0][ab]);
  }
  else
  {
    widg = create_box_axis_menu ("Show/Hide", i, i, menu_ab, & view -> colorp[0][ab]);
  }
  widg = create_menu_item (FALSE, "Style");
  gtk_menu_shell_append ((GtkMenuShell *)menu_ab, widg);
  GtkWidget * menus = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menus);
  gchar * str;
  str = g_strdup_printf ("Width [ %f pts ]", (ab) ? view -> anim -> last -> img -> xyz -> line : view -> anim -> last -> img -> abc -> line);
  int box_axis = (ab) ? view -> anim -> last -> img -> xyz -> axis : view -> anim -> last -> img -> abc -> box;
  if (id == 0)
  {
    view -> ogl_box_axis[ab][1] = create_box_axis_menu (text_styles[1], i, 1, menus, & view -> colorp[1][ab]);
    view -> ogl_box_axis[ab][2] = create_box_axis_menu (text_styles[4], i, 4, menus, & view -> colorp[4][ab]);
    view -> ogl_box_axis[ab][3] = create_menu_item (TRUE, "_Lines");
    gtk_menu_shell_append ((GtkMenuShell *)menu_ab, view -> ogl_box_axis[ab][3]);
    menul = gtk_menu_new ();
    gtk_menu_item_set_submenu ((GtkMenuItem *)view -> ogl_box_axis[ab][3], menul);
    view -> ogl_box_axis[ab][4] = create_layout_widget (str, menul, box_axis, & view -> colorp[WIREFRAME][ab]);
  }
  else
  {
    create_box_axis_menu (text_styles[1], i, 1, menus, & view -> colorp[1][ab]);
    create_box_axis_menu (text_styles[4], i, 4, menus, & view -> colorp[4][ab]);
    if (i == WIREFRAME)
    {
      widg = create_menu_item (FALSE, "Lines");
      gtk_menu_shell_append ((GtkMenuShell *)menu_ab, widg);
      menul = gtk_menu_new ();
      gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menul);
      widg = create_layout_widget (str, menul, box_axis, & view -> colorp[WIREFRAME][ab]);
    }
  }
  g_free (str);

  str = g_strdup_printf ("Radius [ %f Å ]", (ab) ? view -> anim -> last -> img -> xyz -> rad : view -> anim -> last -> img -> abc -> rad);
  if (id == 0)
  {
    view -> ogl_box_axis[ab][5] = create_menu_item (FALSE, "Cylinders");
    gtk_menu_shell_append ((GtkMenuShell *)menu_ab, view -> ogl_box_axis[ab][5]);
    menul = gtk_menu_new ();
    gtk_menu_item_set_submenu ((GtkMenuItem *)view -> ogl_box_axis[ab][5], menul);
    view -> ogl_box_axis[ab][6] = create_layout_widget (str, menul, box_axis, & view -> colorp[CYLINDERS][ab]);
  }
  else if (i == CYLINDERS)
  {
    widg = create_menu_item (FALSE, "Cylinders");
    gtk_menu_shell_append ((GtkMenuShell *)menu_ab, widg);
    menul = gtk_menu_new ();
    gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menul);
    widg = create_layout_widget (str, menul, box_axis, & view -> colorp[CYLINDERS][ab]);
  }
  g_free (str);

  if (ab == 0)
  {
    widg = create_menu_item (FALSE, "Color");
    gtk_menu_shell_append ((GtkMenuShell *)menu_ab, widg);
    if (id == 0)
    {
      view -> ogl_box_axis[ab][7] = create_color_widget (widg, view, i);
    }
    else
    {
      widg = create_color_widget (widg, view, i);
    }
    add_advanced_item (menu_ab, G_CALLBACK(box_advanced), (gpointer)view, FALSE, 0, 0);
  }
  else
  {
    menu_axis (menu_ab, view, id);
  }

  if (id == 0 && ab == 0)
  {
    return view -> ogl_box[0];
  }
  else
  {
    return ab_menu;
  }
}
#else
/*!
  \fn G_MODULE_EXPORT void show_hide_box_axis (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief handle the 'box/axis' show/hide signal GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_box_axis (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  tint * the_data = (tint *)data;
  glwin * view = get_project_by_id(the_data -> a) -> modelgl;
  const gchar * name = g_action_get_name ((GAction *)action);
  int lgt = strlen (name);
  gchar * str = g_strdup_printf ("%c%c", name[lgt-2], name[lgt-1]);
  if (g_strcmp0(str, ".1") == 0)
  {
    g_free (str);
    str = g_strdup_printf ("%.*s.0.0", lgt-4, name);
    g_action_group_activate_action ((GActionGroup *)view -> action_group, (const gchar *)str, NULL);
    g_free (str);
  }
  else
  {
    GVariant * state;
    gboolean show;
    state = g_action_get_state (G_ACTION (action));
    show = ! g_variant_get_boolean (state);
    if (show)
    {
      switch (the_data -> c)
      {
        case 0:
          view -> anim -> last -> img -> abc -> box = WIREFRAME;
          break;
        case 1:
          view -> anim -> last -> img -> xyz -> axis = WIREFRAME;
          break;
      }
    }
    else
    {
      switch (the_data -> c)
      {
        case 0:
          view -> anim -> last -> img -> abc -> box = NONE;
          break;
        case 1:
          view -> anim -> last -> img -> xyz -> axis = NONE;
          break;
      }
    }
    view -> create_shaders[the_data -> c+MDBOX] = TRUE;
    update_menu_bar (view);
    update (view);
    g_action_change_state (G_ACTION (action), g_variant_new_boolean (show));
    g_variant_unref (state);
    update_show_hide_box_axis (view, the_data -> c, ! show);
  }
}

/*!
  \fn G_MODULE_EXPORT void change_box_axis_radio (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief handle a box/axis radio menu item signal GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void change_box_axis_radio (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  tint * the_data = (tint *)data;
  int i, j;
  i = the_data -> c;
  glwin * view = get_project_by_id (the_data -> a) -> modelgl;
  const gchar * style = g_variant_get_string (parameter, NULL);
  int lgt = strlen (style);
  gchar * name = g_strdup_printf ("%c%c", style[lgt-2], style[lgt-1]);
  if (g_strcmp0(name, ".1") == 0)
  {
    name = g_strdup_printf ("%.*s.0", lgt-2, style);
    gchar * str = g_strdup_printf ("set-%s-style", (i) ? "axis" : "box");
    g_action_group_activate_action ((GActionGroup *)view -> action_group, (const gchar *)str, g_variant_new_string((const gchar *)name));
    g_free (str);
    g_free (name);
  }
  else
  {
    gchar * style_name = NULL;
    gchar * str = g_strdup_printf ("set-%s-style", (i) ? "axis" : "box");
    for (j=0; j<2; j++)
    {
      style_name = g_strdup_printf ("%s.%d.0", str, j);
      if (g_strcmp0(style, (const gchar *)style_name) == 0)
      {
        if (i)
        {
          view -> anim -> last -> img -> xyz -> axis = (j == 0) ? WIREFRAME : CYLINDERS;
        }
        else
        {
          view -> anim -> last -> img -> abc -> box = (j == 0) ? WIREFRAME : CYLINDERS;
        }
        view -> create_shaders[i+MDBOX] = TRUE;
        update (view);
        update_menu_bar (view);
        g_free (style_name);
        style_name = NULL;
        break;
      }
      g_free (style_name);
      style_name = NULL;
    }
    g_free (str);
    g_action_change_state (G_ACTION (action), parameter);
  }
}

/*!
  \fn GMenu * axis_box_style (glwin * view, int popm, int ab, int abs)

  \brief create the box/axis '-> Style' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param ab box (0) or axis (1)
  \param abs the active box/axis style
*/
GMenu * axis_box_style (glwin * view, int popm, int ab, int abs)
{
  GMenu * menu = g_menu_new ();
  gchar * str = g_strdup_printf ("%s-style", (ab) ? "axis" : "box");
  append_opengl_item (view, menu, text_styles[1], str, popm, 0, NULL, IMG_NONE, NULL, FALSE,
                      G_CALLBACK(change_box_axis_radio), & view -> colorp[1][ab], FALSE, (abs == WIREFRAME) ? TRUE : FALSE,
                      TRUE, (abs != NONE) ? TRUE : FALSE);
  g_free (str);
  str = g_strdup_printf ("%s-style", (ab) ? "axis" : "box");
  append_opengl_item (view, menu, text_styles[4], str, popm, 1, NULL, IMG_NONE, NULL, FALSE,
                      G_CALLBACK(change_box_axis_radio), & view -> colorp[4][ab], FALSE, (abs == CYLINDERS) ? TRUE : FALSE,
                      TRUE, (abs != NONE) ? TRUE : FALSE);
  g_free (str);
  return menu;
}

/*!
  \fn GMenu * axis_box_param (glwin * view, int popm, int ab, int style)

  \brief create the box/axis '-> Length/Width/Radius' submenus GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param ab box (0) or axis (1)
  \param style the active box/axis style
*/
GMenu * axis_box_param (glwin * view, int popm, int ab, int style)
{
  gchar * str, * key;
  if (style == WIREFRAME)
  {
    str = g_strdup_printf ("Width [ %f pts ]", (ab) ? view -> anim -> last -> img -> xyz -> line : view -> anim -> last -> img -> abc -> line);
    key = g_strdup_printf ("%s-width", (ab) ? "axis" : "box");
  }
  else if (style == CYLINDERS)
  {
    str = g_strdup_printf ("Radius [ %f Å ]", (ab) ? view -> anim -> last -> img -> xyz -> rad : view -> anim -> last -> img -> abc -> rad);
    key = g_strdup_printf ("%s-radius", (ab) ? "axis" : "box");
  }
  else
  {
    str = g_strdup_printf (" length [ %f Å ]", view -> anim -> last -> img -> xyz -> length);
    key = g_strdup_printf ("axis-legnth");
  }
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, str, key, popm, ab, NULL, IMG_NONE, NULL, FALSE,
                      G_CALLBACK(window_bonds), & view -> colorp[style][ab], FALSE, FALSE, FALSE, TRUE);
  g_free (str);
  g_free (key);
  return menu;
}

/*!
  \fn GMenuItem * menu_box_axis (glwin * view, int popm, int ab)

  \brief create the 'Model -> Box' and 'View -> Axis' submenus GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param ab box (0) or axis (1)
*/
GMenuItem * menu_box_axis (glwin * view, int popm, int ab)
{
  GMenuItem * ab_item = g_menu_item_new ((ab) ? "Axis" : "Box", (ab) ? NULL : (get_project_by_id(view -> proj) -> cell.ltype) ? NULL : "None");
  int i = (ab) ? view -> anim -> last -> img -> xyz -> axis : view -> anim -> last -> img -> abc -> box;
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, "Show/Hide", (ab) ? "show-axis" : "show-box", popm, popm, NULL, IMG_NONE, NULL, FALSE,
                      G_CALLBACK(show_hide_box_axis), & view -> colorp[0][ab], TRUE, (i != NONE) ? TRUE : FALSE, FALSE, (ab) ? TRUE : get_project_by_id(view -> proj) -> cell.ltype);

  append_submenu (menu, "Style", axis_box_style (view, popm, ab, i));
  GMenuItem * item ;
  if (i == WIREFRAME)
  {
    item = g_menu_item_new ("Lines", NULL);
    g_menu_item_set_attribute (item, "custom", "s", (ab) ? "axis-lines" : "box-lines", NULL);
    g_menu_item_set_submenu (item, (GMenuModel *)axis_box_param (view, popm, ab, WIREFRAME));
    g_menu_append_item (menu, item);
  }
  if (i == CYLINDERS)
  {
    item = g_menu_item_new ("Cylinders", NULL);
    g_menu_item_set_attribute (item, "custom", "s", (ab) ? "axis-cylinders" : "box-cylinders", NULL);
    g_menu_item_set_submenu (item, (GMenuModel *)axis_box_param (view, popm, ab, CYLINDERS));
    g_menu_append_item (menu, item);
  }

  if (ab == 0)
  {
    GMenu * menuc = g_menu_new ();
    append_opengl_item (view, menuc, "box-color", "box-color", popm, popm, NULL, IMG_NONE, NULL, TRUE, NULL, NULL, FALSE, FALSE, FALSE, FALSE);
    append_opengl_item (view, menuc, "More colors ...", "box-color", popm, popm, NULL, IMG_NONE, NULL, FALSE,
                        G_CALLBACK(to_run_box_color_window), view, FALSE, FALSE, FALSE, get_project_by_id(view -> proj) -> cell.ltype);
    append_submenu (menu, "Color", menuc);
    g_object_unref (menuc);
    append_opengl_item (view, menu, "Advanced", "box-advanced", popm, popm, NULL, IMG_STOCK, DPROPERTIES, FALSE,
                      G_CALLBACK(to_box_advanced), (gpointer)view, FALSE, FALSE, FALSE, TRUE);
  }
  else
  {
    menu_axis (menu, view, popm);
  }
  g_menu_item_set_submenu (ab_item, (GMenuModel *)menu);
  return ab_item;
}
#endif
