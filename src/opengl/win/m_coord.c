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
* @file m_coord.c
* @short Functions to create the 'Chemistry -> Coordination' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_coord.c'
*
* Contains:
*

 - The functions to create the 'Chemistry -> Coordination' submenu

*
* List of functions:

  gboolean is_coord_in_menu (int id, project * this_proj);

  void detach_frag_mol_menu (glwin * view, int id, int jd);

  G_MODULE_EXPORT void show_hide_coord (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void show_hide_coord (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void to_coord_properties (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * add_menu_coord (glwin * view, int id, int jd);
  GtkWidget * menu_coord (glwin * view, int id);
  GtkWidget * menu_rings (glwin * view, int id);

  GMenu * color_item (glwin * view, gchar * act, int popm, int id, GCallback handler, gpointer data);
  GMenu * menu_show_coord (glwin * view, int popm, int id, int mid);
  GMenu * menu_show_frag_mol (glwin * view, int popm, int id, int mid);
  GMenu * menu_show_rings (glwin * view, int popm, int id, int mid);
  GMenu * add_menu_coord (glwin * view, int popm, int id);
  GMenu * menu_coord (glwin * view, int popm);
  GMenu * menu_rings (glwin * view, int popm);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"

extern G_MODULE_EXPORT void coord_properties (GtkWidget * widg, gpointer data);
#ifdef GTK4
extern G_MODULE_EXPORT void window_color_coord (GSimpleAction * action, GVariant * parameter, gpointer data);
extern GtkWidget * color_palette (glwin * view, int ideo, int spec, int geo);
#endif

/*!
  \fn gboolean is_coord_in_menu (int id, project * this_proj)

  \brief is this coordination in a menu ?

  \param id the coordination type
  \param this_proj the target project
*/
gboolean is_coord_in_menu (int id, project * this_proj)
{
  if (((id == 2 || id == 3) && this_proj -> coord -> totcoord[id] <= COORD_MAX_MENU) || id < 2 || id > 3)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void show_hide_coord (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief show/hide coordination callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_coord (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void show_hide_coord (GtkWidget * widg, gpointer data)

  \brief show/hide coordination callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_coord (GtkWidget * widg, gpointer data)
#endif
{
  qint * cid = (qint *)data;
  int i, j, k;
  int c, g, s;
  gboolean doit = TRUE;
  gboolean show;
  project * this_proj = get_project_by_id(cid -> a);
  s = cid -> b;
  c = cid -> c;
  g = cid -> d;
  j = 0;
  if (g < 2)
  {
    for (i=0; i<s; i++)
    {
      j += this_proj -> coord -> ntg[g][i];
    }
  }
  j += c;
#ifdef GTK3
  show = (widg) ? gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg) : this_proj -> modelgl -> anim -> last -> img -> show_coord[g][j];
#else
  GVariant * state;
  if (action)
  {
    state = g_action_get_state (G_ACTION (action));
    show = ! g_variant_get_boolean (state);
    const gchar * coord = g_action_get_name ((GAction *)action);
    int lgt = strlen (coord);
    gchar * name = g_strdup_printf ("%c%c", coord[lgt-2], coord[lgt-1]);
    if (g_strcmp0(name, ".1") == 0)
    {
      name = g_strdup_printf ("%.*s.0", lgt-2, coord);
      g_action_group_activate_action ((GActionGroup *)this_proj -> modelgl -> action_group, (const gchar *)name, NULL);
      g_free (name);
      doit = FALSE;
    }
  }
  else
  {
    show = this_proj -> modelgl -> anim -> last -> img -> show_coord[g][j];
  }
#endif
#ifdef DEBUG
  g_debug ("SHOW_HIDE_COORD:: p= %d, s= %d, c= %d, g= %d, show= %d", this_proj -> id, s, c, g, show);
#endif

#ifdef GTK3
  // GTK3 Menu Action To Check
  if (is_coord_in_menu(g, this_proj))
  {
    for (i=0; i<2; i++)
    {
      if (widg != this_proj -> modelgl -> ogl_geom[i][g][j])
      {
        if (GTK_IS_WIDGET(this_proj -> modelgl -> ogl_geom[i][g][j]))
        {
          if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[i][g][j]) != show)
          {
            gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[i][g][j], show);
          }
        }
      }
    }
  }
#endif
  if (doit)
  {
    this_proj -> modelgl -> anim -> last -> img -> show_coord[g][j] = show;
    for (i=0; i<this_proj -> steps; i++)
    {
      for (j=0; j<this_proj -> natomes; j++)
      {
        if (g < 2)
        {
          if (this_proj -> atoms[i][j].sp == s && this_proj -> atoms[i][j].coord[g] == c)
          {
            this_proj -> atoms[i][j].show[0] = this_proj -> atoms[i][j].show[1] = show;
          }
        }
        else if (g < 4)
        {
          if (this_proj -> atoms[i][j].coord[g] == c)
          {
            this_proj -> atoms[i][j].show[0] = this_proj -> atoms[i][j].show[1] = show;
          }
        }
        else if (g < 9)
        {
          k = this_proj -> coord -> geolist[g][0][c] - 1;
          if (this_proj -> atoms[i][j].rings[s][k] != NULL)
          {
            if (this_proj -> atoms[i][j].rings[s][k][0])
            {
              this_proj -> atoms[i][j].show[0] = this_proj -> atoms[i][j].show[1] = show;
            }
          }
        }
        else
        {
          k = this_proj -> coord -> geolist[g][0][c] - 1;
          if (this_proj -> atoms[i][j].chain[k] != NULL)
          {
            if (this_proj -> atoms[i][j].chain[k][0])
            {
              this_proj -> atoms[i][j].show[0] = this_proj -> atoms[i][j].show[1] = show;
            }
          }
        }
      }
    }
    init_default_shaders (this_proj -> modelgl);
#ifdef GTK4
    if (action)
    {
      g_action_change_state (G_ACTION (action), g_variant_new_boolean (show));
      g_variant_unref (state);
    }
#endif
  }
}

#ifdef GTK3
/*!
  \fn void detach_frag_mol_menu (glwin * view, int id, int jd)

  \brief create a fragment/molecule type submenu

  \param view the target glwin
  \param id main app (0) or popup (1)
  \param jd coordination type
*/
void detach_frag_mol_menu (glwin * view, int id, int jd)
{
  GtkWidget * widg;
  if (view -> oglmc[id][jd] != NULL)
  {
    if (view -> oglmc[id][jd][0] != NULL)
    {
      if (GTK_IS_WIDGET(view -> oglmc[id][jd][0]))
      {
        widg =  gtk_menu_get_attach_widget (GTK_MENU(view -> oglmc[id][jd][0]));
        if (GTK_IS_WIDGET(widg))
        {
          g_object_ref (view -> oglmc[id][jd][0]);
          gtk_menu_detach (GTK_MENU(view -> oglmc[id][jd][0]));
        }
      }
    }
  }
  if (view -> oglmv[id][jd] != NULL)
  {
    if (view -> oglmv[id][jd][0] != NULL)
    {
      if (GTK_IS_WIDGET(view -> oglmv[id][jd][0]))
      {
        widg =  gtk_menu_get_attach_widget (GTK_MENU(view -> oglmv[id][jd][0]));
        if (GTK_IS_WIDGET(widg))
        {
          g_object_ref (view -> oglmv[id][jd][0]);
          gtk_menu_detach (GTK_MENU(view -> oglmv[id][jd][0]));
        }
      }
    }
  }
}

/*!
  \fn GtkWidget * add_menu_coord (glwin * view, int id, int jd)

  \brief create a coordination type submenu

  \param view the target glwin
  \param id main app (0) or popup (1)
  \param jd coordination type
*/
GtkWidget * add_menu_coord (glwin * view, int id, int jd)
{
  int i, j;
  GtkWidget * widg;
  GtkWidget * menucts;
  project * this_proj = get_project_by_id (view -> proj);
  GtkWidget * menuct = gtk_menu_new ();
  if (is_coord_in_menu(jd, this_proj))
  {
    if (jd == 2 || jd == 3) detach_frag_mol_menu(view, id, jd);
    GtkWidget * colt = create_menu_item (TRUE, "Show/Hide");
    gtk_menu_shell_append ((GtkMenuShell *)menuct, colt);
    if (jd < 2 || jd > 3)
    {
      menucts = gtk_menu_new ();
      gtk_menu_item_set_submenu ((GtkMenuItem *)colt, menucts);
      if (jd > 3)
      {
        if (jd == 9)
        {
          colt = create_menu_item (FALSE, "Atoms in chain(s) of size");
        }
        else
        {
          colt = create_menu_item (FALSE, "Atoms in ring(s) of size");
        }
        gtk_menu_shell_append ((GtkMenuShell *)menucts, colt);
        menucts = gtk_menu_new ();
        gtk_menu_item_set_submenu ((GtkMenuItem *)colt, menucts);
      }
      j = (jd < 2) ? this_proj -> nspec : this_proj -> coord -> totcoord[jd];
      for (i=0; i<j; i++)
      {
        if (jd < 2)
        {
          if (view -> oglmv[id][jd])
          {
            if (GTK_IS_WIDGET(view -> oglmv[id][jd][i]))
            {
              widg = gtk_widget_get_parent (view -> oglmv[id][jd][i]);
              if (GTK_IS_WIDGET(widg))
              {
                g_object_ref (view -> oglmv[id][jd][i]);
                gtk_container_remove (GTK_CONTAINER(widg), view -> oglmv[id][jd][i]);
              }
              gtk_menu_shell_append ((GtkMenuShell *)menucts, view -> oglmv[id][jd][i]);
            }
          }
        }
        else
        {
          if (view -> ogl_geom[id][jd])
          {
            if (GTK_IS_WIDGET(view -> ogl_geom[id][jd][i]))
            {
              widg =  gtk_widget_get_parent (view -> ogl_geom[id][jd][i]);
              if (GTK_IS_WIDGET(widg))
              {
                g_object_ref (view -> ogl_geom[id][jd][i]);
                gtk_container_remove (GTK_CONTAINER(widg), view -> ogl_geom[id][jd][i]);
              }
              gtk_menu_shell_append ((GtkMenuShell *)menucts, view -> ogl_geom[id][jd][i]);
            }
          }
        }
      }
    }
    else if ((jd == 2 && view -> adv_bonding[0]) || (jd == 3 && view -> adv_bonding[1]))
    {
      if (view -> oglmv[id][jd])
      {
        gtk_menu_item_set_submenu ((GtkMenuItem *)colt, view -> oglmv[id][jd][0]);
      }
    }
    if (jd != 9)
    {
      colt = create_menu_item (FALSE, "Color(s)");
      gtk_menu_shell_append ((GtkMenuShell *)menuct, colt);
    }
    if (jd < 2)
    {
      menucts = gtk_menu_new ();
      gtk_menu_item_set_submenu ((GtkMenuItem *)colt, menucts);
      if (view -> oglmc[id][jd])
      {
        for (i=0; i<this_proj -> nspec; i++)
        {
          if (GTK_IS_WIDGET(view -> oglmc[id][jd][i]))
          {
            widg = gtk_widget_get_parent (view -> oglmc[id][jd][i]);
            if (GTK_IS_WIDGET(widg))
            {
              g_object_ref (view -> oglmc[id][jd][i]);
              gtk_container_remove (GTK_CONTAINER(widg), view -> oglmc[id][jd][i]);
            }
            gtk_menu_shell_append ((GtkMenuShell *)menucts, view -> oglmc[id][jd][i]);
          }
        }
      }
    }
    else if ((jd == 2 && view -> adv_bonding[0]) || (jd == 3 && view -> adv_bonding[1]))
    {
      if (view -> oglmc[id][jd])
      {
        if (GTK_IS_WIDGET(view -> oglmc[id][jd][0]))
        {
          gtk_menu_item_set_submenu ((GtkMenuItem *)colt, view -> oglmc[id][jd][0]);
        }
      }
    }
    else if ((jd > 3 && jd < 9) && view -> rings)
    {
      if (view -> oglmc[id][jd])
      {
        if (GTK_IS_WIDGET(view -> oglmc[id][jd][0]))
        {
          widg =  gtk_menu_get_attach_widget (GTK_MENU(view -> oglmc[id][jd][0]));
          if (GTK_IS_WIDGET(widg))
          {
            g_object_ref (view -> oglmc[id][jd][0]);
            gtk_menu_detach (GTK_MENU(view -> oglmc[id][jd][0]));
          }
          gtk_menu_item_set_submenu ((GtkMenuItem *)colt, view -> oglmc[id][jd][0]);
        }
      }
    }
  }
  add_advanced_item (menuct, G_CALLBACK(coord_properties), (gpointer)& view -> colorp[jd][1], FALSE, 0, 0);
  return menuct;
}

/*!
  \fn GtkWidget * menu_coord (glwin * view, int id)

  \brief create the 'Coordination' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * menu_coord (glwin * view, int id)
{
  GtkWidget * menuco = create_menu_item (FALSE, "Coordination");
  GtkWidget * menuc = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)menuco, menuc);

  gtk_menu_shell_append ((GtkMenuShell *)menuc, menu_item_new_with_submenu ("Total(s)", TRUE, add_menu_coord(view, id, 0)));
  gtk_menu_shell_append ((GtkMenuShell *)menuc, menu_item_new_with_submenu ("Partial(s)", TRUE, add_menu_coord(view, id, 1)));
  return menuco;
}

/*!
  \fn GtkWidget * menu_rings (glwin * view, int id)

  \brief create the 'Ring(s)' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * menu_rings (glwin * view, int id)
{
  GtkWidget * menuco = gtk_menu_new ();
  if (view -> rings)
  {
    int i;
    for (i=0; i<5; i++)
    {
      if (view -> ring_max[i])
      {
        if (id == 0)
        {
          view -> ogl_rings[i+1] = menu_item_new_with_submenu (rings_type[i], TRUE, add_menu_coord (view, id, 4+i));
          gtk_menu_shell_append ((GtkMenuShell *)menuco, view -> ogl_rings[i+1]);
        }
        else
        {
          gtk_menu_shell_append ((GtkMenuShell *)menuco, menu_item_new_with_submenu (rings_type[i], TRUE, add_menu_coord (view, id, 4+i)));
        }
      }
    }
  }
  return menuco;
}
#else
/*!
  \fn GMenu * color_item (glwin * view, gchar * act, int popm, int id, GCallback handler, gpointer data)

  \brief create a submenu to attach a color palette

  \param view the target glwin
  \param act the custom attribute to attach a widget
  \param popm main app (0) or popup (1)
  \param id id number
  \param handler the associated callback
  \param data the associated data pointer
*/
GMenu * color_item (glwin * view, gchar * act, int popm, int id, GCallback handler, gpointer data)
{
  GMenu *  menu = g_menu_new ();
  append_opengl_item (view, menu, act, act, popm, id, NULL, IMG_NONE, NULL, TRUE, NULL, NULL, FALSE, FALSE, FALSE, FALSE);
  append_opengl_item (view, menu, "More colors ...", act, popm, id, NULL, IMG_NONE, NULL, FALSE, handler, data, FALSE, FALSE, FALSE, TRUE);
  return menu;
}

/*!
  \fn GMenu * menu_show_coord (glwin * view, int popm, int id, int mid)

  \brief create the coordination 'Show' or 'Color' submenus

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param id coordination type
  \param mid 0 = show, 1 = color
*/
GMenu * menu_show_coord (glwin * view, int popm, int id, int mid)
{
  GMenu * menu = g_menu_new ();
  GMenu * menus;
  project * this_proj = get_project_by_id (view -> proj);
  gchar * stra,  * strb;
  int i, j, k;
  for (i=0; i<this_proj -> nspec; i++)
  {
    menus = g_menu_new ();
    if (this_proj -> coord)
    {
      if (this_proj -> coord -> ntg[id])
      {
        j = 0;
        for (k=0; k<i; k++)
        {
          j += this_proj -> coord -> ntg[id][k];
        }
        for (k=0; k < this_proj -> coord -> ntg[id][i]; k++)
        {
          if (id)
          {
            stra = exact_name (env_name (this_proj, k, i, 1, NULL));
          }
          else
          {
            stra = g_strdup_printf ("%d", this_proj -> coord -> geolist[id][i][k]);
          }
          if (! mid)
          {
            strb = g_strdup_printf ("%s-s", stra);
            append_opengl_item (view, menus, stra, strb, popm, k+j, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(show_hide_coord), & view -> gcid[id][k+j][id],
                                TRUE, view -> anim -> last -> img -> show_coord[id][k+j], FALSE, TRUE);
          }
          else
          {
            strb = g_strdup_printf ("%s-c", stra);
            append_submenu (menus, stra, color_item(view, strb, popm, k+j, G_CALLBACK(window_color_coord), & view -> gcid[id][k+j][id]));
          }
          g_free (stra);
          g_free (strb);
        }
      }
    }
    append_submenu (menu, this_proj -> chemistry -> label[i], menus);
    g_object_unref (menus);
  }
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void to_coord_properties (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief open advancedd coordination properties GTK4 callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_coord_properties (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  coord_properties (NULL, data);
}

/*!
  \fn GMenu * menu_show_frag_mol (glwin * view, int popm, int id, int mid)

  \brief create a fragment/molecule coordination submenu

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param id coordination type
  \param mid 0 = show, 1 = color
*/
GMenu * menu_show_frag_mol (glwin * view, int popm, int id, int mid)
{
  GMenu * menu = g_menu_new ();
  project * this_proj = get_project_by_id (view -> proj);
  if (active_coord -> totcoord[id] <= COORD_MAX_MENU)
  {
    gchar * stra, * strb;
    int i;
    for (i=0; i < this_proj -> coord -> totcoord[id]; i++)
    {
      stra = g_strdup_printf ("%s N°%d", (id == 2) ? "Fragment" : "Molecule", i+1);
      strb = g_strdup_printf ("%s-%d", (id == 2) ? "frag" : "mol", i+1);
      if (! mid)
      {
        append_opengl_item (view, menu, stra, strb, popm, i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(show_hide_coord), & view -> gcid[id][i][id],
                            TRUE, view -> anim -> last -> img -> show_coord[id][i], FALSE, TRUE);
      }
      else
      {
        append_submenu (menu, stra, color_item(view, (id == 2) ? "fcol": "mcol", popm, i, G_CALLBACK(window_color_coord), & view -> gcid[id][i][id]));
      }
      g_free (stra);
      g_free (strb);
    }
  }
  // append_opengl_item (view, menu, "All", "all-fm", popm, id, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_coord_properties), & view -> colorp[id][1], FALSE, FALSE, FALSE, TRUE);
  return menu;
}

/*!
  \fn GMenu * menu_show_rings (glwin * view, int popm, int id, int mid)

  \brief create a ring coordination submenu

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param id coordination type
  \param mid 0 = show, 1 = color
*/
GMenu * menu_show_rings (glwin * view, int popm, int id, int mid)
{
  GMenu * menu = g_menu_new ();
  GMenu * menus;
  if (! mid)
  {
    menus = g_menu_new ();
    append_submenu (menu, (id < 9) ? "Atoms In Ring(s) of Size " : "Atoms In Chain(s) of Size ", menus);
  }
  project * this_proj = get_project_by_id (view -> proj);
  gchar * stra, * strb;
  int i;
  for (i=0; i < this_proj -> coord -> totcoord[id]; i++)
  {
    stra = g_strdup_printf ("%d", this_proj -> coord -> geolist[id][0][i]);
    strb = g_strdup_printf ("%d-%s-%s", id, stra, (mid) ? "color" : "coord");
    if (! mid)
    {
      append_opengl_item (view, menus, stra, strb, popm, i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(show_hide_coord), & view -> gcid[id][i][id],
                          TRUE, view -> anim -> last -> img -> show_coord[id][i], FALSE, TRUE);
    }
    else
    {
      append_submenu (menu, stra, color_item(view, "color", popm, i, G_CALLBACK(window_color_coord), & view -> gcid[id][i][id]));
    }
    g_free (stra);
    g_free (strb);
  }
  if (! mid) g_object_unref (menus);
  return menu;
}

/*!
  \fn GMenu * add_menu_coord (glwin * view, int popm, int id)

  \brief create a coordination type submenu

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param id coordination type
*/
GMenu * add_menu_coord (glwin * view, int popm, int id)
{
  GMenu * menu = g_menu_new ();
  project * this_proj = get_project_by_id (view -> proj);
  gchar * menu_name[2] = {"Show/Hide", "Color"};
  if (is_coord_in_menu(id, this_proj))
  {
    int i;
    for (i=0; i<2; i++)
    {
      if (id < 2)
      {
        append_submenu (menu, menu_name[i], menu_show_coord(view, popm, id, i));
      }
      else if (id < 4)
      {
        append_submenu (menu, menu_name[i], menu_show_frag_mol(view, popm, id, i));
      }
      else if (id < 10)
      {
        append_submenu (menu, menu_name[i], menu_show_rings(view, popm, id, i));
      }
    }
    append_opengl_item (view, menu, "Advanced", "adv-c", popm, id, NULL, IMG_STOCK, (gpointer)DPROPERTIES, FALSE,
                        G_CALLBACK(to_coord_properties), & view -> colorp[id][1], FALSE, FALSE, FALSE, TRUE);
  }
  return menu;
}

/*!
  \fn GMenu * menu_coord (glwin * view, int popm)

  \brief create the 'Coordination' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_coord (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_submenu (menu, "Total(s)", add_menu_coord(view, popm, 0));
  append_submenu (menu, "Partial(s)", add_menu_coord(view, popm, 1));
  return menu;
}

/*!
  \fn GMenu * menu_rings (glwin * view, int popm)

  \brief create the 'Ring(s)' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_rings (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  if (view -> rings)
  {
    int i;
    for (i=0; i<5; i++)
    {
      if (view -> ring_max[i])
      {
        append_submenu (menu, rings_type[i], add_menu_coord (view, popm, 4+i));
      }
    }
  }
  return menu;
}
#endif
