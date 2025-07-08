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
* @file m_style.c
* @short Functions to create the 'OpenGL -> Style' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_style.c'
*
* Contains:
*

 - The functions to create the 'OpenGL -> Style' submenu

*
* List of functions:

  void clean_atom_style (project * this_proj);
  void update_menus (glwin * view);

  G_MODULE_EXPORT void set_style (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void change_style_radio (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * create_style_menu (char * name, int val, int style, int vbl, int filled, guint accel, GtkWidget * menu, tint * data);
  GtkWidget * menu_style (glwin * view, int id);

  GMenu * menu_style (glwin * view, int popm);

*/

#include "global.h"
#include "bind.h"
#include "project.h"
#include "glview.h"
#include "glwindow.h"
#include "preferences.h"

extern gchar * label_atpts (project * this_proj, glwin * view, int id);

char * text_styles[OGL_STYLES] = {"Ball and stick",
                                  "Wireframe",
                                  "Spacefilled",
                                  "Spheres",
                                  "Cylinders",
                                  "Dots"};

char * text_filled[FILLED_STYLES] = {"Covalent radius",
                                     "Ionic radius",
                                     "van Der Waals radius",
                                     "In crystal radius"};

/*!
  \fn void clean_atom_style (project * this_proj)

  \brief clean all atom(s) possible alternative rendering styles

  \param this_proj the target project
*/
void clean_atom_style (project * this_proj)
{
  int i, j;
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      this_proj -> atoms[i][j].style = NONE;
    }
  }
}

#ifdef GTK3
// GTK3 Menu Action To Check
/*!
  \fn void update_menus (glwin * view)

  \brief update GTK3 menus of style has changed

  \param view the target glwin
*/
void update_menus (glwin * view)
{
  int i, j;
  int s = view -> anim -> last -> img -> style;
  gchar * str;
  for (i=0; i<2; i++)
  {
    for (j=0; j<6; j+=2)
    {
      hide_the_widgets (view -> ogl_bonds[j+8*i]);
      if (j<4) hide_the_widgets (view -> ogl_atoms[4*i+j]);
    }
  }
  switch (s)
  {
    case CYLINDERS:
      for (i=0; i<2; i++) show_the_widgets (view -> ogl_bonds[8*i]);
      break;
    case WIREFRAME:
      for (i=0; i<2; i++)
      {
        show_the_widgets (view -> ogl_bonds[4+8*i]);
        str = label_atpts (get_project_by_id(view -> proj), view, 1+2*i);
        gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_atoms[4*i+1]), str);
        g_free (str);
        show_the_widgets (view -> ogl_atoms[4*i+2]);
      }
      break;
    case SPACEFILL:
      for (i=0; i<2; i++)
      {
        str = label_atpts (get_project_by_id(view -> proj), view, 4);
        gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_atoms[4*i+1]), str);
        g_free (str);
        show_the_widgets (view -> ogl_atoms[4*i]);
      }
      break;
    case PUNT:
      for (i=0; i<2; i++)
      {
        str = label_atpts (get_project_by_id(view -> proj), view, 2*i+1);
        gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_atoms[4*i+1]), str);
        g_free (str);
        show_the_widgets (view -> ogl_atoms[4*i+2]);
      }
      break;
    case SPHERES:
      for (i=0; i<2; i++)
      {
        str = label_atpts (get_project_by_id(view -> proj), view, 2*i);
        gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_atoms[4*i+1]), str);
        g_free (str);
        show_the_widgets (view -> ogl_atoms[4*i]);
      }
      break;
    default:
      for (i=0; i<2; i++)
      {
        show_the_widgets (view -> ogl_bonds[2+8*i]);
        str = label_atpts (get_project_by_id(view -> proj), view, 2*i);
        gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_atoms[4*i+1]), str);
        g_free (str);
        show_the_widgets (view -> ogl_atoms[4*i]);
      }
      break;
  }
}
#endif

/*!
  \fn G_MODULE_EXPORT void set_style (GtkWidget * widg, gpointer data)

  \brief set style callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_style (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  project * this_proj = get_project_by_id(the_data -> a);
  int s = the_data -> b;
  int st = (s >= OGL_STYLES) ? SPACEFILL : s;
  int ft = (s >= OGL_STYLES) ? s - OGL_STYLES * (s/OGL_STYLES) : NONE;
  int old_style = this_proj -> modelgl -> anim -> last -> img -> style;
  int old_filled = this_proj -> modelgl -> anim -> last -> img -> filled_type;
  int i, j, k, l, m;
#ifdef GTK3
  if ((old_style != st || old_filled != ft) && gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
#else
  if (old_style != st || old_filled != ft)
#endif
  {
    i = old_style;
    j = old_filled;
    this_proj -> modelgl -> anim -> last -> img -> style = NONE;
    this_proj -> modelgl -> anim -> last -> img -> filled_type = NONE;
#ifdef GTK3
    if (old_style == SPACEFILL)
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> filled_styles[j], FALSE);
    }
    else
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_styles[i], FALSE);
    }
#endif
    if (s >= OGL_STYLES)
    {
      j = this_proj -> nspec;
      k = (ft) ? 9 + ft : 2;
      l = (ft) ? 12 + ft : 7;
      for (i=0; i<j; i++)
      {
        m = (int)this_proj -> chemistry -> chem_prop[CHEM_Z][i];
        this_proj -> modelgl -> anim -> last -> img -> atomicrad[i] = (default_o_at_rs[2]) ? default_at_rs[2] : get_radius (2, ft, m, default_atomic_rad[k]);
        this_proj -> modelgl -> anim -> last -> img -> atomicrad[i+j] = (default_o_at_rs[7]) ? default_at_rs[7] : get_radius (7, ft, m, default_atomic_rad[l]);
      }
#ifdef GTK3
      if (widg != this_proj -> modelgl -> filled_styles[ft])
      {
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> filled_styles[ft], TRUE);
      }
#endif
      this_proj -> modelgl -> anim -> last -> img -> filled_type = ft;
      this_proj -> modelgl -> anim -> last -> img -> style = SPACEFILL;
    }
    else
    {
#ifdef GTK3
      if (widg != this_proj -> modelgl -> ogl_styles[st])
      {
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_styles[st], TRUE);
      }
#endif
      this_proj -> modelgl -> anim -> last -> img -> style = st;
    }
    clean_atom_style (this_proj);
    if (st == WIREFRAME || st == PUNT)
    {
      j = 0;
    }
    else
    {
      j = 1;
    }
#ifdef GTK3
    for (i=0; i<OGL_RENDERS; i++)
    {
      widget_set_sensitive (this_proj -> modelgl -> ogl_render[i], j);
    }
    update_menus (this_proj -> modelgl);
#else

#endif
    init_default_shaders (this_proj -> modelgl);
  }
  /*else if (st != SPACEFILL && old_style != NONE && ! gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_styles[st], TRUE);
  }
  else if (old_style != NONE && old_filled != NONE && ! gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> filled_styles[ft], TRUE);
  }*/
}

#ifdef GTK3
/*!
  \fn GtkWidget * create_style_menu (char * name, int val, int style, int vbl, int filled, guint accel, GtkWidget * menu, tint * data)

  \brief create style menu item GTK3

  \param name the menu item label
  \param val active style
  \param style style value for this menu item
  \param vbl active filled style if any
  \param filled filled value for this menu item
  \param accel keyboard accelerator
  \param menu the menu to attach the new menu item to
  \param data the associated data pointer
*/
GtkWidget * create_style_menu (char * name, int val, int style, int vbl, int filled, guint accel, GtkWidget * menu, tint * data)
{
  GtkWidget * style_widget = gtk3_menu_item (menu, name, IMG_NONE, NULL, G_CALLBACK(set_style), data, (accel != -1) ? TRUE : FALSE, accel,
                                             0, TRUE, TRUE, (style == val && filled == vbl) ? TRUE : FALSE);
  return style_widget;
}

/*!
  \fn GtkWidget * menu_style (glwin * view, int id)

  \brief create the 'OpenGL -> Style' submenu - GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * menu_style (glwin * view, int id)
{
  int i, j;
  GtkWidget * widg;
  GtkWidget * menus = gtk_menu_new ();
  guint accels[OGL_STYLES-1]={GDK_KEY_b, GDK_KEY_w, GDK_KEY_s, GDK_KEY_c, GDK_KEY_d};
  guint accelf[FILLED_STYLES]={GDK_KEY_o, GDK_KEY_i, GDK_KEY_v, GDK_KEY_r};
  if (id == 0)
  {
    for (i=0; i<OGL_STYLES; i++)
    {
      if (i != SPACEFILL)
      {
        view -> ogl_styles[i] = create_style_menu (text_styles[i],
                                                   i,
                                                   view -> anim -> last -> img -> style,
                                                   i,
                                                   i,
                                                   accels[(i < SPACEFILL) ? i : i-1],
                                                   menus,
                                                   & view -> colorp[i][0]);
      }
      else
      {
        widg = create_menu_item (FALSE, "Spacefilled");
        gtk_menu_shell_append ((GtkMenuShell *)menus, widg);
        GtkWidget * menuf = gtk_menu_new ();
        gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menuf);
        for (j=0; j < FILLED_STYLES; j++)
        {
           view -> filled_styles[j] = create_style_menu (text_filled[j],
                                                         SPACEFILL,
                                                         view -> anim -> last -> img -> style,
                                                         j,
                                                         view -> anim -> last -> img -> filled_type,
                                                         accelf[j],
                                                         menuf,
                                                         & view -> colorp[OGL_STYLES + j][0]);
        }
      }
    }
  }
  else
  {
    for (i=0; i<OGL_STYLES; i++)
    {
      if (i != SPACEFILL)
      {
        widg = create_style_menu (text_styles[i],
               i,
               view -> anim -> last -> img -> style,
               i,
               i,
               accels[(i < SPACEFILL) ? i : i-1],
               menus,
               & view -> colorp[i][0]);
      }
      else
      {
        widg = create_menu_item (FALSE, "Spacefilled");
        gtk_menu_shell_append ((GtkMenuShell *)menus, widg);
        GtkWidget * menuf = gtk_menu_new ();
        gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menuf);
        for (j=0; j < FILLED_STYLES; j++)
        {
           widg = create_style_menu (text_filled[j],
                                     SPACEFILL,
                                     view -> anim -> last -> img -> style,
                                     j,
                                     view -> anim -> last -> img -> filled_type,
                                     accelf[j],
                                     menuf,
                                     & view -> colorp[OGL_STYLES + j][0]);
        }

      }
    }
  }
  return menus;
}
#else
/*!
  \fn G_MODULE_EXPORT void change_style_radio (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief change style callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void change_style_radio (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  glwin * view = (glwin *)data;
  const gchar * style = g_variant_get_string (parameter, NULL);
  int lgt = strlen (style);
  gchar * name = g_strdup_printf ("%c%c", style[lgt-2], style[lgt-1]);
  if (g_strcmp0(name, ".1") == 0)
  {
    g_free (name);
    name = g_strdup_printf ("%.*s.0", lgt-2, style);
    g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-style", g_variant_new_string((const gchar *)name));
    g_free (name);
  }
  else
  {
    gchar * style_name = NULL;
    int i;
    for (i=0; i<OGL_STYLES+FILLED_STYLES; i++)
    {
      style_name = g_strdup_printf ("set-style.%d.0", i);
      if (g_strcmp0(style, (const gchar *)style_name) == 0)
      {
        if (i < SPACEFILL)
        {
          set_style (NULL, & view -> colorp[i][0]);
        }
        else if (i < SPACEFILL + FILLED_STYLES)
        {
          i -= SPACEFILL;
          set_style (NULL, & view -> colorp[OGL_STYLES+i][0]);
        }
        else
        {
          i -= FILLED_STYLES;
          set_style (NULL, & view -> colorp[i][0]);
        }
        g_free (style_name);
        style_name = NULL;
        break;
      }
      g_free (style_name);
      style_name = NULL;
    }
    g_action_change_state (G_ACTION (action), parameter);
  }
}

/*!
  \fn GMenu * menu_style (glwin * view, int popm)

  \brief create the 'OpenGL -> Style' submenu - GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_style (glwin * view, int popm)
{
  int i, j, k;
  GMenu * menu = g_menu_new ();
  gchar * accels[OGL_STYLES-1]={"b", "w", "s", "c", "d"};
  gchar * accelf[FILLED_STYLES]={"o", "i", "v", "r"};

  k = 0;
  for (i=0; i<OGL_STYLES; i++, k++)
  {
    if (i != SPACEFILL)
    {
      append_opengl_item (view, menu, text_styles[i], "style", popm, k,
                          accels[(i < SPACEFILL) ? i : i-1], IMG_NONE, NULL, FALSE,
                          G_CALLBACK(change_style_radio), (gpointer)view,
                          FALSE, (view -> anim -> last -> img -> style == i) ? TRUE : FALSE, TRUE, TRUE);
    }
    else
    {
      GMenu * menus = g_menu_new ();
      for (j=0; j < FILLED_STYLES; j++, k++)
      {
        append_opengl_item (view, menus, text_filled[j], "style", popm, k,
                            accelf[j], IMG_NONE, NULL, FALSE,
                            G_CALLBACK(change_style_radio), (gpointer)view,
                            FALSE, (view -> anim -> last -> img -> style == SPACEFILL && view -> anim -> last -> img -> filled_type == j) ? TRUE : FALSE, TRUE, TRUE);
      }
      append_submenu (menu, "Spacefilled", menus);
    }
  }
  return menu;
}
#endif
