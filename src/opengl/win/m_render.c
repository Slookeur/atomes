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
* @file m_render.c
* @short Functions to create the 'OpenGL -> Render' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_render.c'
*
* Contains:
*

 - The functions to create the 'OpenGL -> Render' submenu

*
* List of functions:

  G_MODULE_EXPORT void set_render (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void change_render_radio (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * menu_render (glwin * view, int id);

  GMenu * menu_render (glwin * view, int popm);

*/

#include "global.h"
#include "glview.h"
#include "glwindow.h"

gchar * text_renders[OGL_RENDERS] = {"Filled", "Lines", "Points"};

/*!
  \fn G_MODULE_EXPORT void set_render (GtkWidget * widg, gpointer data)

  \brief set OpenGL rendering mode callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_render (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  project * this_proj = get_project_by_id(the_data -> a);
  int i = this_proj -> modelgl -> anim -> last -> img -> render;
  int j = the_data -> b;
#ifdef GTK3
  if (i != j && gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
#else
  if (i != j)
#endif
  {
    this_proj -> modelgl -> anim -> last -> img -> render = NONE;
#ifdef GTK3
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_render[i], FALSE);
    if (widg != this_proj -> modelgl -> ogl_render[j])
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_render[j], TRUE);
    }
#endif
    this_proj -> modelgl -> anim -> last -> img -> render = j;
    this_proj -> modelgl -> create_shaders[MAXIS] = TRUE;
    update (this_proj -> modelgl);
  }
#ifdef GTK3
  else if (i == j && ! gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_render[j], TRUE);
  }
#endif
}

#ifdef GTK3
/*!
  \fn GtkWidget * menu_render (glwin * view, int id)

  \brief create the 'OpenGL -> Render' submenu - GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * menu_render (glwin * view, int id)
{
  int i, j;
  GtkWidget * menur = gtk_menu_new ();
  j = view -> anim -> last -> img -> render;
  if (id == 0)
  {
    for (i=0; i<OGL_RENDERS; i++)
    {
      view -> ogl_render[i] = gtk3_menu_item (menur, text_renders[i], IMG_NONE, NULL, G_CALLBACK(set_render), & view -> colorp[i][0], FALSE, 0, 0, TRUE, TRUE, (i == j) ? TRUE : FALSE);
      widget_set_sensitive (view -> ogl_render[i], 0);
      if (view -> anim -> last -> img -> style != WIREFRAME)
      {
        widget_set_sensitive (view -> ogl_render[i], 1);
      }
    }
  }
  else
  {
    GtkWidget * widg;
    for (i=0; i<OGL_RENDERS; i++)
    {
      widg = gtk3_menu_item (menur, text_renders[i], IMG_NONE, NULL, G_CALLBACK(set_render), & view -> colorp[i][0], FALSE, 0, 0, TRUE, TRUE, (i == j) ? TRUE : FALSE);
      widget_set_sensitive (widg, 0);
      if (view -> anim -> last -> img -> style != WIREFRAME)
      {
        widget_set_sensitive (widg, 1);
      }
    }
  }
  return menur;
}
#else

/*!
  \fn G_MODULE_EXPORT void change_render_radio (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief set OpenGL rendering mode callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void change_render_radio (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  glwin * view = (glwin *)data;
  const gchar * render = g_variant_get_string (parameter, NULL);
  int lgt = strlen (render);
  gchar * name = g_strdup_printf ("%c%c", render[lgt-2], render[lgt-1]);
  if (g_strcmp0(name, ".1") == 0)
  {
    g_free (name);
    name = g_strdup_printf ("%.*s.0", lgt-2, render);
    g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-render", g_variant_new_string((const gchar *)name));
    g_free (name);
  }
  else
  {
    gchar * render_name = NULL;
    int i;
    for (i=0; i<OGL_RENDERS*2; i++)
    {
      render_name = g_strdup_printf ("set-render.%d.0", i);
      if (g_strcmp0(render, (const gchar *)render_name) == 0)
      {
        set_render (NULL, & view -> colorp[i][0]);
        g_free (render_name);
        render_name = NULL;
        break;
      }
      g_free (render_name);
      render_name = NULL;
    }
    g_action_change_state (G_ACTION (action), parameter);
  }
}

/*!
  \fn GMenu * menu_render (glwin * view, int popm)

  \brief create the 'OpenGL -> Render' submenu - GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_render (glwin * view, int popm)
{
  int i;
  GMenu * menu = g_menu_new ();
  // gboolean sensitive = (view -> anim -> last -> img -> style == WIREFRAME || view -> anim -> last -> img -> style == PUNT) ? FALSE : TRUE;
  for (i=0; i<OGL_RENDERS; i++)
  {
    append_opengl_item (view, menu, text_renders[i], "render", popm, i,
                        NULL, IMG_NONE, NULL,
                        FALSE, G_CALLBACK(change_render_radio), (gpointer)view,
                        FALSE, (view -> anim -> last -> img -> render == i) ? TRUE : FALSE, TRUE, TRUE);

  }
  return menu;
}
#endif
