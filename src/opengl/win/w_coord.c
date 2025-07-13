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
* @file w_coord.c
* @short Functions to create the 'Environments configuration' window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_coord.c'
*
* Contains:
*

 - The functions to create the 'Environments configuration' window

*
* List of functions:

  int get_page_from_geo_coord (glwin * view, int geo, int coord);

  gboolean add_geo (int poly, project * this_proj, int g, int i, int j);

  G_MODULE_EXPORT gboolean scroll_set_poly_alpha (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
  G_MODULE_EXPORT gboolean close_event_coord (GtkWindow * widg, gpointer data);
  G_MODULE_EXPORT gboolean close_event_coord (GtkWidget * widg, GdkEvent * event, gpointer data);

  void poly_alpha_has_changed (gpointer data, GLfloat v);
  void set_frag_mol_cell_background (GtkListStore * store, GtkTreeIter iter, ColRGBA col);
  void add_this_frag_mol_to_search_tree (project * this_proj, int geo, int gid);
  void set_this_frag_mol_color (gpointer data, GtkTreePath * path);

  G_MODULE_EXPORT void toggled_show_hide_coord (GtkCheckButton * widg, gpointer data);
  G_MODULE_EXPORT void toggled_show_hide_coord (GtkToggleButton * widg, gpointer data);
  G_MODULE_EXPORT void toggled_label_unlabel_coord (GtkCheckButton * widg, gpointer data);
  G_MODULE_EXPORT void toggled_label_unlabel_coord (GtkToggleButton * widg, gpointer data);
  G_MODULE_EXPORT void toggled_select_unselect_coord (GtkCheckButton * widg, gpointer data);
  G_MODULE_EXPORT void toggled_select_unselect_coord (GtkToggleButton * widg, gpointer data);
  G_MODULE_EXPORT void toggled_show_hide_poly (GtkCheckButton * widg, gpointer data);
  G_MODULE_EXPORT void toggled_show_hide_poly (GtkToggleButton * widg, gpointer data);
  G_MODULE_EXPORT void set_color_frag_mol (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_color_coord (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_poly_alpha (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void on_select_frag_mol (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void run_set_this_frag_mol_color (GtkDialog * win, gint response_id, gpointer data);
  G_MODULE_EXPORT void to_set_this_frag_mol_color (GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer data);
  G_MODULE_EXPORT void update_frag_mol_search (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_atom_color_map_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_poly_color_map_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void on_cloned_poly_toggled (GtkCheckButton * Button, gpointer data);
  G_MODULE_EXPORT void on_cloned_poly_toggled (GtkToggleButton * Button, gpointer data);
  G_MODULE_EXPORT void close_coord (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void coord_properties (GtkWidget * widg, gpointer data);

  GtkWidget * coord_tab (glwin * view, int geo, int poly);
  GtkWidget * create_frag_mol_tree (project * this_proj, int geo);
  GtkWidget * create_frag_mol_search (project * this_proj, int geo);
  GtkWidget * fragmol_tab (glwin * view, int geo);
  GtkWidget * param_tab (glwin * view);
  GtkWidget * advanced_coord_properties (glwin * view, int page);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"
#include "bind.h"

extern cairo_surface_t * col_surface (double r, double g, double b, int x, int y);
extern G_MODULE_EXPORT void set_color_map (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void cloned_poly (GtkWidget * widg, gpointer data);
extern int selected_aspec;
extern GtkWidget * rings_tab (glwin * view, int g);
extern GtkWidget * chains_tab (glwin * view);

char * text_maps[ATOM_MAPS] = {"Atomic species",
                               "Total",
                               "Partial",
                               "Fragment(s)",
                               "Molecule(s)"};

int frag_mol_status;

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggled_show_hide_coord (GtkCheckButton * widg, gpointer data)

  \brief toggle show / hide coordination callback GTK4

  \param widg the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_show_hide_coord (GtkCheckButton * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggled_show_hide_coord (GtkToggleButton * widg, gpointer data)

  \brief toggle show / hide coordination callback GTK3

  \param widg the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_show_hide_coord (GtkToggleButton * widg, gpointer data)
#endif
{
  qint * the_data = (qint *)data;
  int i, j, k;
  int s, g, c;
  project * this_proj = get_project_by_id (the_data -> a);
  s = the_data -> b;
  g = the_data -> d;
  c = the_data -> c;
#ifdef DEBUG
  g_debug ("Toggle show/hide coord:: s= %d, g= %d, c= %d", s, g, c);
#endif
  j = 0;
  if (g < 2)
  {
    for (i=0; i<s; i++)
    {
      j += this_proj -> coord -> ntg[g][i];
    }
  }
  j += c;

  k = (widg) ? button_get_status ((GtkWidget *)widg) : frag_mol_status;
  if (k != this_proj -> modelgl -> anim -> last -> img -> show_coord[g][j])
  {
    if (is_coord_in_menu(g, this_proj))
    {
#ifdef GTK4
      gchar * name;
      gchar * str;
      if (g < 2)
      {
        if (g)
        {
          str = exact_name (env_name (this_proj, c, s, 1, NULL));
        }
        else
        {
          str = g_strdup_printf ("%d", this_proj -> coord -> geolist[g][s][c]);
        }
        name = g_strdup_printf ("set-%s-s.%d.0", str, j);
        g_free (str);
      }
      else if (g > 1 && g < 4)
      {
        name = g_strdup_printf ("set-%s-%d.%d.0", (g == 2) ? "frag" : "mol", c+1, c);
      }
      else if (g > 3 && g < 9)
      {
        name = g_strdup_printf ("set-%d-%d-coord.%d.0", g, this_proj -> coord -> geolist[g][0][j], j);
      }
      g_action_group_activate_action ((GActionGroup *)this_proj -> modelgl -> action_group, (const gchar *)name, NULL);
      g_free (name);
#else
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[0][g][j], k);
#endif
    }
    else
    {
      tint pointer;
      pointer.a = g;
      pointer.b = c;
      pointer.c = k;
      opengl_project_changed (the_data -> a);
#ifdef GTK4
      show_hide_the_coord (NULL, NULL, & pointer);
#else
      show_hide_the_coord (NULL, & pointer);
#endif // GTK4
    }
  }
  init_default_shaders (this_proj -> modelgl);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggled_label_unlabel_coord (GtkCheckButton * widg, gpointer data)

  \brief toggle label / unlabel coordination callback GTK4

  \param widg the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_label_unlabel_coord (GtkCheckButton * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggled_label_unlabel_coord (GtkToggleButton * widg, gpointer data)

  \brief toggle label / unlabel coordination callback GTK3

  \param widg the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_label_unlabel_coord (GtkToggleButton * widg, gpointer data)
#endif
{
  qint * the_data = (qint *)data;
  tint pointer;
  pointer.a = the_data -> d;
  pointer.b = the_data -> c;
  pointer.c = (widg) ? button_get_status ((GtkWidget *)widg) : frag_mol_status;
  selected_aspec = the_data -> b;
#ifdef DEBUG
  g_debug ("Toggle label/unlabel coord:: s= %d, g= %d, c= %d, selec_sp= %d", pointer.a, pointer.b, pointer.c, selected_aspec);
#endif
  opengl_project_changed (the_data -> a);
#ifdef GTK4
  label_unlabel_coord (NULL, NULL, & pointer);
#else
  label_unlabel_coord (NULL, & pointer);
#endif // GTK4
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggled_select_unselect_coord (GtkCheckButton * widg, gpointer data)

  \brief toggle select / unselect coordination callback GTK4

  \param widg the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_select_unselect_coord (GtkCheckButton * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggled_select_unselect_coord (GtkToggleButton * widg, gpointer data)

  \brief toggle select / unselect coordination callback GTK3

  \param widg the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_select_unselect_coord (GtkToggleButton * widg, gpointer data)
#endif
{
  qint * the_data = (qint *)data;
  tint pointer;
  pointer.a = the_data -> d;
  pointer.b = the_data -> c;
  pointer.c = (widg) ? button_get_status ((GtkWidget *)widg) : frag_mol_status;
  selected_aspec = the_data -> b;
#ifdef DEBUG
  g_debug ("Toggle select/unselect coord:: s= %d, g= %d, c= %d, selec_sp= %d", pointer.a, pointer.b, pointer.c, selected_aspec);
#endif
  opengl_project_changed (the_data -> a);
#ifdef GTK4
  select_unselect_coord (NULL, NULL, & pointer);
#else
  select_unselect_coord (NULL, & pointer);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggled_show_hide_poly (GtkCheckButton * widg, gpointer data)

  \brief toggle show / hide polyhedra callback GTK4

  \param widg the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_show_hide_poly (GtkCheckButton * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggled_show_hide_poly (GtkToggleButton * widg, gpointer data)

  \brief toggle show / hide polyhedra callback GTK3

  \param widg the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_show_hide_poly (GtkToggleButton * widg, gpointer data)
#endif
{
  qint * the_data = (qint *)data;
  int i, j, k;
  int s, g, c;
  project * this_proj = get_project_by_id(the_data -> a);
  s = the_data -> b;
  g = the_data -> d;
  c = the_data -> c;
  j = c;
  if (g < 2)
  {
    for (i=0; i<s; i++)
    {
      j += this_proj -> coord -> ntg[g][i];
    }
  }
  k = button_get_status ((GtkWidget *)widg);
#ifdef GTK4
  if (k != this_proj -> modelgl -> anim -> last -> img -> show_poly[g][j])
  {
    gchar * str;
    gchar * name;
    if (g < 2)
    {
      if (g)
      {
        str = exact_name (env_name (this_proj, c, s, 1, NULL));
      }
      else
      {
        str = g_strdup_printf ("%d", this_proj -> coord -> geolist[g][s][c]);
      }
      name = g_strdup_printf ("set-%s-%d-p.%d.0", str, g, j);
      g_free (str);
    }
    else if (g > 3 && g < 9)
    {
      name = g_strdup_printf ("set-%d-%d-p.%d.0", g, this_proj -> coord -> geolist[g][0][j], j);
    }
    g_action_group_activate_action ((GActionGroup *)this_proj -> modelgl -> action_group, (const gchar *)name, NULL);
    g_free (name);
  }
#else
  // GTK3 Menu Action To Check
  if (is_coord_in_menu(g, this_proj))
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_poly[0][g][j], k);
  }
  this_proj -> modelgl -> anim -> last -> img -> show_poly[g][j] = k;
  int shaders[2] = {POLYS, RINGS};
  re_create_md_shaders (2, shaders, this_proj);
  update (this_proj -> modelgl);
#endif
}

/*!
  \fn G_MODULE_EXPORT void set_color_frag_mol (GtkColorChooser * colob, gpointer data)

  \brief set fragment or molecule color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_color_frag_mol (GtkColorChooser * colob, gpointer data)
{
  qint * cid = (qint *)data;
  int c, g;
  project * this_proj = get_project_by_id(cid -> a);
  g = cid -> b;
  c = cid -> c;
  this_proj -> modelgl -> anim -> last -> img -> spcolor[g][0][c] = get_button_color (colob);
  int shaders[4] = {ATOMS, BONDS, POLYS, RINGS};
  re_create_md_shaders (4, shaders, this_proj);
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void set_color_coord (GtkColorChooser * colob, gpointer data)

  \brief set coordination color

  \param colob the GtkColorChosser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_color_coord (GtkColorChooser * colob, gpointer data)
{
  qint * cid = (qint *)data;
  int c, g, s;
  project * this_proj = get_project_by_id(cid -> a);
  s = cid -> b;
  c = cid -> c;
  g = cid -> d;
  if (g > 3) s = 0;
  this_proj -> modelgl -> anim -> last -> img -> spcolor[g][s][c] = get_button_color (colob);
  int shaders[4] = {ATOMS, BONDS, POLYS, RINGS};
  re_create_md_shaders (4, shaders, this_proj);
  update (this_proj -> modelgl);
}

/*!
  \fn void poly_alpha_has_changed (gpointer data, GLfloat v)

  \brief change polyhedra opacity

  \param data the associated data pointer
  \param v the new opacity value
*/
void poly_alpha_has_changed (gpointer data, GLfloat v)
{
  qint * cid = (qint *)data;
  int c, g, s;
  project * this_proj = get_project_by_id(cid -> a);
  s = cid -> b;
  c = cid -> c;
  g = cid -> d;
  if (g > 1) s = 0;
  this_proj -> modelgl -> anim -> last -> img -> spcolor[g][s][c].alpha = v;
  int shaders[2] = {POLYS, RINGS};
  re_create_md_shaders (2, shaders, this_proj);
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_poly_alpha (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief set polyhedra opacity - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_poly_alpha (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  poly_alpha_has_changed (data, (GLfloat) value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_poly_alpha (GtkRange * range, gpointer data)

  \brief set polyhedra opacity - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_poly_alpha (GtkRange * range, gpointer data)
{
  poly_alpha_has_changed (data, (GLfloat) gtk_range_get_value (range));
}

/*!
  \fn gboolean add_geo (int poly, project * this_proj, int g, int i, int j)

  \brief test add this geometry data to the tree store or not ?

  \param poly polyhedra (1 / 0)
  \param this_proj the target project
  \param g the target geometry, 0 = total coord, 1 = partial coord, 4-8 = ring(s), 9 = chain(s)
  \param i target chemical species or 0
  \param j geometry id number
*/
gboolean add_geo (int poly, project * this_proj, int g, int i, int j)
{
  if (! poly)
  {
    if (g == 0)
    {
      if (this_proj -> coord -> geolist[g][i][j] != -1)
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else if (g == 1)
    {
      return TRUE;
    }
    else
    {
      if (this_proj -> coord -> geolist[g][i][j])
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
  }
  else
  {
    if (g == 0 || g > 3)
    {
      if (this_proj -> coord -> geolist[g][i][j] > 2)
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      int l, m;
      m = 0;
      for (l=0; l<this_proj -> nspec; l++)
      {
        m += this_proj -> coord -> partial_geo[i][j][l];
      }
      if (m > 2)
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
  }
}

/*!
  \fn GtkWidget * coord_tab (glwin * view, int geo, int poly)

  \brief create coordination(s) and polyhedra tab

  \param view the target glwin
  \param geo the target geometry, 0 = total coord, 1 = partial coord, 4-8 = ring(s), 9 = chain(s)
  \param poly polyhedra (1 / 0)
*/
GtkWidget * coord_tab (glwin * view, int geo, int poly)
{
  int i, j, k, l, m, n;
  int p = view -> proj;
  gchar * str;
  GtkWidget * wb;
  GtkWidget * but;
  gchar * col[15] = {" ", "<b>Color</b>", "<b>Show</b>", "<b>Label</b>", "<b>Pick</b>", "<b>Alpha</b>",
                          "<b>Color <sup>*</sup></b>", "<b>Show <sup>**</sup></b>", "<b>Label <sup>**</sup></b>", "<b>Pick <sup>**</sup></b>",
                          "<b>Ring(s) size</b>",
                          "<b>Chain(s) size</b>", "<b>Show <sup>*</sup></b>", "<b>Label <sup>*</sup></b>", "<b>Pick <sup>*</sup></b>"};
  int scol[15] = {100, 70, 65, 40, 40, 150, 70, 75, 55, 55, 100, 100, 60, 50, 55};
  GtkWidget * box = create_vbox (BSEP);
  GtkWidget * coord = create_scroll (box, -1, -1, GTK_SHADOW_NONE);
  gtk_widget_set_hexpand (coord, TRUE);
  gtk_widget_set_vexpand (coord, TRUE);
  n = 0;
  if (geo > 3 && ! poly)
  {
    n = (geo == 9) ? 11 : 10;
    gtk_widget_set_size_request (coord, -1, 475);
    if (geo < 9)
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, markup_label("  * for the polyhedra only", -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, markup_label("** affect all atoms that belong to ring(s) of this size", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }
    else
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, markup_label("* affect all atoms that belong to chain(s) of this size", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }
  }
  GtkWidget * vbox = create_vbox (BSEP);
  add_container_child (CONTAINER_SCR, coord, vbox);
  l = 0;
  if (poly)
  {
    l = 1;
  }
  wb = create_hbox (0);
  gtk_widget_set_size_request (wb, 350, -1);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, wb, FALSE, FALSE, 0);
  for (i=0; i<5-2*l-geo/9; i++)
  {
    if (geo == 9)
    {
      j = 11 + i;
    }
    else if (i == 0 && geo > 3)
    {
      j = 10;
    }
    else if (i > 0 && geo > 3 && ! poly)
    {
      j = i+5;
    }
    else if (i == 1 && poly)
    {
      j = 5;
    }
    else
    {
      j = i;
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, markup_label(col[j], scol[j], -1, 0.5, 0.5), FALSE, FALSE, 5);
  }
  k = 0;
  project * this_proj = get_project_by_id(p);
  m = (geo < 2) ? this_proj -> nspec : 1;
  if ((geo < 2 || (geo == 9 && view -> chain_max) || (geo > 3 && geo < 9 && view -> ring_max[geo-4])) && this_proj -> coord -> totcoord[geo] > 0)
  {
    for (i=0; i < m; i++)
    {
      if (geo == 0)
      {
        for (j=0 ; j < this_proj -> coord -> ntg[geo][i]; j++)
        {
          if (add_geo(poly, this_proj, geo, i, j))
          {
            str = g_strdup_printf ("  <b>%s</b>", this_proj -> chemistry -> label[i]);
            add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
            g_free (str);
            break;
          }
        }
      }
      for (j=0 ; j < this_proj -> coord -> ntg[geo][i]; j++, k++)
      {
        if (add_geo(poly, this_proj, geo, i, j))
        {
          wb = create_hbox (0);
          gtk_widget_set_size_request (wb, 350, -1);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, wb, FALSE, FALSE, 0);
          if (geo == 0)
          {
            str =  g_strdup_printf ("%d", this_proj -> coord -> geolist[geo][i][j]);
          }
          else if (geo == 1)
          {
            str = exact_name(env_name (this_proj, j, i, 1, NULL));
          }
          else
          {
            str = g_strdup_printf ("%d atoms", this_proj -> coord -> geolist[geo][i][j]);
          }
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, markup_label(str, scol[0], -1, 0.5, 0.5), FALSE, FALSE, 5);
          g_free (str);
          if (poly)
          {
            but = create_hscale (0.0, 1.0, 0.01, view -> anim -> last -> img -> spcolor[geo][i][j].alpha, GTK_POS_LEFT, 3, 160,
                                 G_CALLBACK(set_poly_alpha), G_CALLBACK(scroll_set_poly_alpha), & view -> gcid[geo][k][geo]);
          }
          else if (geo != 9)
          {
            ColRGBA col = view -> anim -> last -> img -> spcolor[geo][i][j];
            col.alpha = 1.0;
            but = color_button (col, TRUE, 80, -1, G_CALLBACK(set_color_coord), & view -> gcid[geo][k][geo]);
          }
          if (poly || geo != 9) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, but, FALSE, FALSE, 5);

          if (poly)
          {
            but = check_button (NULL, -1, -1, view -> anim -> last -> img -> show_poly[geo][k], G_CALLBACK(toggled_show_hide_poly), & view -> gcid[geo][k][geo]);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, but, FALSE, FALSE, 20);
          }
          else
          {
            but = check_button (NULL, -1, -1, view -> anim -> last -> img -> show_coord[geo][k], G_CALLBACK(toggled_show_hide_coord), & view -> gcid[geo][k][geo]);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, but, FALSE, FALSE, 20);
            but = check_button (NULL, -1, -1, FALSE, G_CALLBACK(toggled_label_unlabel_coord), & view -> gcid[geo][k][geo]);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, but, FALSE, FALSE, 15+n);
            but = check_button (NULL, -1, -1, FALSE, G_CALLBACK(toggled_select_unselect_coord), & view -> gcid[geo][k][geo]);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, but, FALSE, FALSE, 15+n/2);
          }
        }
      }
    }
  }
  return box;
}

/*!
  \fn G_MODULE_EXPORT void on_select_frag_mol (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief tree store action on cell selection

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree view
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_select_frag_mol (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  int i, g, v, act;
  tint * dat = (tint * )data;
  GtkTreeIter iter;
  opengl_project_changed(dat -> a);
  coord_edition * coord = opengl_project -> modelgl -> coord_win;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  g = (dat -> b < 30) ? 2 : 3;
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    v = 0;
  }
  else
  {
    v = 1;
  }
  gtk_tree_model_get_iter (GTK_TREE_MODEL(coord -> frag_mol_model[g-2]), & iter, path);
  act = dat -> b - 10*g;
  gtk_list_store_set (coord -> frag_mol_model[g-2], & iter, act, v, -1);
  gtk_tree_model_get (GTK_TREE_MODEL(coord -> frag_mol_model[g-2]), & iter, 0, & i, -1);
  frag_mol_status = v;
  switch (act)
  {
    case 1:
      // Viz
      toggled_show_hide_coord (NULL, & opengl_project -> modelgl -> gcid[g][i-1][g]);
      break;
    case 3:
      // Label
      toggled_label_unlabel_coord (NULL, & opengl_project -> modelgl -> gcid[g][i-1][g]);
      break;
    case 4:
      // Pick
      toggled_select_unselect_coord (NULL, & opengl_project -> modelgl -> gcid[g][i-1][g]);
      break;
  }
}

/*!
  \fn void set_frag_mol_cell_background (GtkListStore * store, GtkTreeIter iter, ColRGBA col)

  \brief set tree store cell background color

  \param store the GtkListStore
  \param iter the GtkTreeIter target to update
  \param col the background color to set
*/
void set_frag_mol_cell_background (GtkListStore * store, GtkTreeIter iter, ColRGBA col)
{
  cairo_surface_t * surface;
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 70, 50);
  cairo_t * cr = cairo_create (surface);
  cairo_set_source_rgba (cr, col.red, col.green, col.blue, 1.0);
  cairo_paint (cr);
  GdkPixbuf * pix = convert_to_pixbuf (surface);
  cairo_surface_destroy (surface);
  cairo_destroy(cr);
  gtk_list_store_set (store, & iter, 2, pix, -1);
}

/*!
  \fn void add_this_frag_mol_to_search_tree (project * this_proj, int geo, int gid)

  \brief add fragment or molecule in the search tree

  \param this_proj the target project
  \param geo 2 = fragment(s) or 3 = molecule(s)
  \param gid fragment or molecule id number to add
*/
void add_this_frag_mol_to_search_tree (project * this_proj, int geo, int gid)
{
  GtkTreeIter id_level;
  GtkTreeIter new_level;
  gboolean valid;
  int i;
  int prepend = 0;
  coord_edition * coord = this_proj -> modelgl -> coord_win;
  GtkListStore * store = (GtkListStore *) coord -> frag_mol_model[geo-2];
  GtkTreeModel * mod = GTK_TREE_MODEL(coord -> frag_mol_model[geo-2]);
  valid = gtk_tree_model_get_iter_first(mod, & id_level);
  while (valid)
  {
    gtk_tree_model_get (mod, & id_level, 0, & i, -1);
    if (i > gid)
    {
      prepend = 1;
      valid = FALSE;
    }
    else if (i == gid)
    {
      prepend = 2;
      valid = FALSE;
    }
    else
    {
      valid = gtk_tree_model_iter_next(mod, & id_level);
    }
  }
  if (prepend < 2)
  {
    switch (prepend)
    {
      case 0:
        gtk_list_store_append (store, & id_level);
        gtk_list_store_set (store, & id_level, 0, gid,
                                               1, this_proj -> modelgl -> anim -> last -> img -> show_coord[geo][gid-1],
                                               3, FALSE,
                                               4, FALSE, -1);
        set_frag_mol_cell_background (store, id_level, this_proj -> modelgl -> anim -> last -> img -> spcolor[geo][0][gid-1]);
        break;
      case 1:
        gtk_list_store_insert_before (store, & new_level, & id_level);
        gtk_list_store_set (store, & new_level, 0, gid,
                                                1, this_proj -> modelgl -> anim -> last -> img -> show_coord[geo][gid-1],
                                                3, FALSE,
                                                4, FALSE, -1);
        set_frag_mol_cell_background (store, new_level, this_proj -> modelgl -> anim -> last -> img -> spcolor[geo][0][gid-1]);
        break;
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void run_set_this_frag_mol_color (GtkDialog * win, gint response_id, gpointer data)

  \brief set fragment / molecule color - running the dialog

  \param win the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_set_this_frag_mol_color (GtkDialog * win, gint response_id, gpointer data)
{
  tint * dat = (tint * )data;
  int i, g;
  if (response_id == GTK_RESPONSE_OK)
  {
    g = (dat -> b < 30) ? 2 : 3;
    GtkTreeIter fm_iter;
    gtk_tree_model_get (GTK_TREE_MODEL(opengl_project -> modelgl -> coord_win -> frag_mol_model[g-2]), & fm_iter, 0, & i, -1);
    opengl_project -> modelgl -> anim -> last -> img -> spcolor[g][0][i-1] = get_window_color (GTK_WIDGET(win));
    int shaders[3] = {ATOMS, BONDS, LABEL};
    re_create_md_shaders (3, shaders, opengl_project);
    update (opengl_project -> modelgl);
  }
  destroy_this_dialog (win);
}

/*!
  \fn void set_this_frag_mol_color (gpointer data, GtkTreePath * path)

  \brief set fragment / molecule color - creating the dialog

  \param data the associated data pointer
  \param path the path in the tree view
*/
void set_this_frag_mol_color (gpointer data, GtkTreePath * path)
{
  int i, g;
  tint * dat = (tint * )data;
  opengl_project_changed(dat -> a);
  g = (dat -> b < 30) ? 2 : 3;
  GtkTreeIter iter;
  gtk_tree_model_get_iter (GTK_TREE_MODEL(opengl_project -> modelgl -> coord_win -> frag_mol_model[g-2]), & iter, path);
  coord_edition * coord = opengl_project -> modelgl -> coord_win;
  gtk_tree_model_get (GTK_TREE_MODEL(coord -> frag_mol_model[g-2]), & iter, 0, & i, -1);
  gchar * ctitle[2] = {"Fragment", "Molecule"};
  gchar * str = g_strdup_printf ("%s N°%d color", ctitle[g-2], i);
  GtkWidget * widg = gtk_color_chooser_dialog_new (str, GTK_WINDOW(coord -> win));
  g_free (str);
  set_color_chooser_color (widg, opengl_project -> modelgl -> anim -> last -> img -> spcolor[g][0][i-1]);
  run_this_gtk_dialog (widg, G_CALLBACK(run_set_this_frag_mol_color), data);
  set_frag_mol_cell_background ((GtkListStore *) coord -> frag_mol_model[g-2], iter, opengl_project -> modelgl -> anim -> last -> img -> spcolor[g][0][i-1]);
}

/*!
  \fn G_MODULE_EXPORT void to_set_this_frag_mol_color (GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer data)

  \brief set fragment/molecule color tree view callback

  \param tree_view the GtkTreeView sending the signal
  \param path the path in the tree view
  \param column the tree view column
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_set_this_frag_mol_color (GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer data)
{
  gchar * title;
  g_object_get (column, "title", & title, NULL, NULL);
  if (g_strcmp0(title, "Color") == 0)
  {
    set_this_frag_mol_color (data, path);
  }
}

/*!
  \fn GtkWidget * create_frag_mol_tree (project * this_proj, int geo)

  \brief create the fragment(s) / molecule(s) search tree store

  \param this_proj the target project
  \param geo 2 = fragment(s) or 3 = molecule(s)
*/
GtkWidget * create_frag_mol_tree (project * this_proj, int geo)
{
  int i;
  GtkTreeViewColumn * frag_mol_col[7];
  GtkCellRenderer * frag_mol_cell[7];
  gchar * ctitle[5]={"Id.", "Show", "Color", "Label", "Pick"};
  gchar * ctype[5]={"text", "active", "pixbuf", "active", "active"};
  GType col_type[5]={G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_OBJECT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN};
  coord_edition * coord = this_proj -> modelgl -> coord_win;
  coord -> frag_mol_model[geo-2] = gtk_list_store_newv (5, col_type);
  GtkWidget * frag_mol_tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL(coord -> frag_mol_model[geo-2]));
  for (i=0; i<5; i++)
  {
    if (! i)
    {
      frag_mol_cell[i] = gtk_cell_renderer_text_new ();
    }
    else if (i == 2)
    {
      frag_mol_cell[i] = gtk_cell_renderer_pixbuf_new ();
    }
    else
    {
      frag_mol_cell[i] = gtk_cell_renderer_toggle_new ();
      g_signal_connect (G_OBJECT(frag_mol_cell[i]), "toggled", G_CALLBACK(on_select_frag_mol), & this_proj -> modelgl -> colorp[geo*10+i][0]);
    }
    gtk_cell_renderer_set_fixed_size (frag_mol_cell[i], 70, 25);
    frag_mol_col[i] = gtk_tree_view_column_new_with_attributes (ctitle[i], frag_mol_cell[i], ctype[i], i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(frag_mol_tree), frag_mol_col[i]);
    gtk_tree_view_column_set_alignment (frag_mol_col[i], 0.5);
  }
  g_object_set (frag_mol_tree, "activate-on-single-click", TRUE, NULL, NULL);
  g_signal_connect (G_OBJECT(frag_mol_tree), "row-activated",  G_CALLBACK(to_set_this_frag_mol_color), & this_proj -> modelgl -> colorp[geo*10+i][0]);
  return frag_mol_tree;
}

/*!
  \fn G_MODULE_EXPORT void update_frag_mol_search (GtkEntry * res, gpointer data)

  \brief update the fragment(s) / molecule(s) search widget

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_frag_mol_search (GtkEntry * res, gpointer data)
{
  tint * dat = (tint * )data;
  const gchar * m = entry_get_text (res);
  int v = (int)string_to_double ((gpointer)m);
  project * this_proj = get_project_by_id(dat -> a);
  int g = dat -> b;
  if (v > 0 && v <= this_proj -> coord -> totcoord[g])
  {
    add_this_frag_mol_to_search_tree (this_proj, g, v);
  }
  else
  {
    update_entry_text (res, "");
  }
}

/*!
  \fn GtkWidget * create_frag_mol_search (project * this_proj, int geo)

  \brief create the frgament(s)/molecule(s) search widget

  \param this_proj the target project
  \param geo 2 = fragment(s) or 3 = molecule(s)
*/
GtkWidget * create_frag_mol_search (project * this_proj, int geo)
{
  GtkWidget * frag_mol_search = create_vbox (BSEP);
  gchar * obj[2] = {"fragment", "molecule"};
  gchar * str = g_strdup_printf ("Too many <b>%ss</b> in your model !\n"
                                 "  It is impossible to display the entire list ...\n"
                                 "... instead you can look for %s(s) 'manually':\n", obj[geo-2], obj[geo-2]);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, frag_mol_search, markup_label(str, 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  g_free (str);
  gchar * search_item[2]={"Fragment ID:", "Molecule ID:"};
  GtkWidget * hbox;
  GtkWidget * entry;
  GtkWidget * label;
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, frag_mol_search, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(search_item[geo-2], 100, -1, 0.0, 0.5), FALSE, FALSE, 20);
  entry = create_entry (G_CALLBACK(update_frag_mol_search), 100, 15, FALSE, & this_proj -> modelgl -> colorp[geo][0]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,entry, FALSE, FALSE, 0);
  str = g_strdup_printf ("in [%d - %d]", 1, this_proj ->  coord -> totcoord[geo]);
  label = markup_label (str, 50, -1, 0.0, 0.5);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, label, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, frag_mol_search, markup_label("<b>Search result(s)</b>", 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, frag_mol_search, create_frag_mol_tree (this_proj, geo), FALSE, FALSE, 0);
  return frag_mol_search;
}

/*!
  \fn GtkWidget * fragmol_tab (glwin * view, int geo)

  \brief create fragment(s) or molecule(s) tab

  \param view the target glwin
  \param geo 2 = fragment(s) or 3 = molecule(s)
*/
GtkWidget * fragmol_tab (glwin * view, int geo)
{
  int i, j;
  GtkWidget * wb;
  gchar * str;
  int scol[5] = {70, 70, 65, 40, 40};
  gchar * col[5] = {"<b>Id</b>", "<b>Color</b>", "<b>Show</b>", "<b>Label</b>", "<b>Pick</b>"};

  GtkWidget * box = create_vbox (BSEP);
  GtkWidget * fragmol = create_scroll (box, -1, -1, GTK_SHADOW_NONE);
  gtk_widget_set_hexpand (fragmol, TRUE);
  gtk_widget_set_vexpand (fragmol, TRUE);
  project * this_proj = get_project_by_id(view -> proj);
  i =  this_proj -> coord -> totcoord[geo];
  if (i >  10000)
  {
    add_container_child (CONTAINER_SCR, fragmol, create_frag_mol_search(this_proj, geo));
  }
  else
  {
    GtkWidget * vbox = create_vbox (BSEP);
    add_container_child (CONTAINER_SCR, fragmol, vbox);
    wb = create_hbox (0);
    gtk_widget_set_size_request (wb, 350, -1);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, wb, FALSE, TRUE, 0);
    for (j=0; j<5; j++)
    {
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, markup_label(col[j], scol[j], -1, 0.5, 0.5), FALSE, FALSE, 5);
    }
    for (j=0; j<i; j++)
    {
      wb = create_hbox (0);
      gtk_widget_set_size_request (wb, 300, -1);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, wb, FALSE, TRUE, 0);

      str = g_strdup_printf ("N°%d", j+1);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb, markup_label(str, scol[0], -1, 0.25, 0.5), FALSE, FALSE, 5);
      g_free (str);
      ColRGBA col = view -> anim -> last -> img -> spcolor[geo][0][j];
      col.alpha = 1.0;
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb,
                           color_button (col, TRUE, 80, -1, G_CALLBACK(set_color_frag_mol), & view -> gcid[geo][j][0]),
                           FALSE, FALSE, 5);

      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb,
                           check_button(NULL, -1, -1, view -> anim -> last -> img -> show_coord[geo][j], G_CALLBACK(toggled_show_hide_coord), & view -> gcid[geo][j][geo]),
                           FALSE, FALSE, 20);

      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb,
                           check_button(NULL, -1, -1, FALSE, G_CALLBACK(toggled_label_unlabel_coord), & view -> gcid[geo][j][geo]),
                           FALSE, FALSE, 15);

      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, wb,
                           check_button(NULL, -1, -1, FALSE, G_CALLBACK(toggled_select_unselect_coord), & view -> gcid[geo][j][geo]),
                           FALSE, FALSE, 15);
    }
  }
  return box;
}

/*!
  \fn G_MODULE_EXPORT void set_atom_color_map_box (GtkComboBox * box, gpointer data)

  \brief change atom color map

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_atom_color_map_box (GtkComboBox * box, gpointer data)
{
  glwin * view = (glwin *)data;
  int i = combo_get_active ((GtkWidget *)box);
  int j = view -> cmap[i];
#ifdef GTK4
  gchar * variant = g_strdup_printf ("set-amap.%d.0", j);
  g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-amap", g_variant_new_string((const gchar *)variant));
  g_free (variant);
#else
  // GTK3 Menu Action To Check
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> color_styles[j], TRUE);
#endif
}

/*!
  \fn G_MODULE_EXPORT void set_poly_color_map_box (GtkComboBox * box, gpointer data)

  \brief change polyhedra color map

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_poly_color_map_box (GtkComboBox * box, gpointer data)
{
  glwin * view = (glwin *)data;
  int i = combo_get_active ((GtkWidget *)box);
  int j = view -> cmap[i] + ATOM_MAPS;
#ifdef GTK4
  gchar * variant = g_strdup_printf ("set-pmap.%d.0", j);
  g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-pmap", g_variant_new_string((const gchar *)variant));
  g_free (variant);
#else
  // GTK3 Menu Action To Check
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> color_styles[j], TRUE);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void on_cloned_poly_toggled (GtkCheckButton * Button, gpointer data)

  \brief toggle show / hide cloned polyhedra callback GTK4

  \param Button the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_cloned_poly_toggled (GtkCheckButton * Button, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void on_cloned_poly_toggled (GtkToggleButton * Button, gpointer data)

  \brief toggle show / hide cloned polyhedra callback GTK3

  \param Button the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_cloned_poly_toggled (GtkToggleButton * Button, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
#ifdef GTK4
   view -> anim -> last -> img -> cloned_poly = button_get_status ((GtkWidget *)Button);
   g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-cloned-poly.0.0", NULL);
   /* int shaders[2] = {POLYS, RINGS};
   re_create_md_shaders (2, shaders, get_project_by_id(view -> proj));
   update (view); */
#else
  // GTK3 Menu Action To Check
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_clones[5], gtk_toggle_button_get_active (Button));
#endif
}

/*!
  \fn GtkWidget * param_tab (glwin * view)

  \brief environments configuration window parameters tab

  \param view the target glwin
*/
GtkWidget * param_tab (glwin * view)
{
  int i, j, k, l;
  GtkWidget * vbox = create_vbox (BSEP);
  /*gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW(fragmol), vbox); */
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, TRUE, 25);
  GtkWidget * lab = markup_label ("<b>Atom(s) and bond(s) color map:</b>", -1, -1, 0.0, 0.5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 20);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  GtkWidget * color_box = create_combo ();
  j = 0;
  for (i=0; i<ATOM_MAPS; i++)
  {
    if (i == 1 || i == 2)
    {
      view -> cmap[j] = i;
      combo_text_append (color_box, g_strdup_printf ("%s coordination(s)", text_maps[i]));
      j ++;
    }
    else if (i == 5 && get_project_by_id(view -> proj) -> force_field[0])
    {
      view -> cmap[j] = i;
      combo_text_append (color_box, "Force field (DL_POLY)");
      j ++;
    }
    else if (i == 6 && view -> custom_map != NULL)
    {
      view -> cmap[j] = i;
      combo_text_append (color_box, "Custom");
      j ++;
    }
    else if (i == 0 || (i == 3 && view -> adv_bonding[0]) || (i == 4 && view -> adv_bonding[1]))
    {
      view -> cmap[j] = i;
      combo_text_append (color_box, text_maps[i]);
      j ++;
    }
  }
  l = 0;
  for (k=0; k<j; k++)
  {
    if (view -> anim -> last -> img -> color_map[0] == view -> cmap[k])
    {
      l = k;
      break;
    }
  }
  combo_set_active (color_box, l);
  g_signal_connect (G_OBJECT (color_box), "changed", G_CALLBACK(set_atom_color_map_box), view);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, color_box, FALSE, FALSE, 100);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, TRUE, 25);
  lab = markup_label ("<b>Polyhedra color map:</b>", -1, -1, 0.0, 0.5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 20);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  color_box = create_combo ();
  for (i=0; i<POLY_MAPS; i++)
  {
    if (i == 1 || i == 2)
    {
      combo_text_append (color_box, g_strdup_printf ("%s coordination(s)", text_maps[i]));
    }
    else if (i == 0 || (i == 3 && view -> adv_bonding[0]) || (i == 4 && view -> adv_bonding[1]))
    {
      combo_text_append (color_box, text_maps[i]);
    }
    else if (i == 5 && get_project_by_id(view -> proj) -> force_field[0])
    {
     combo_text_append (color_box, "Force field (DL_POLY)");
    }
    else if (i == 6 && view -> custom_map != NULL)
    {
      combo_text_append (color_box, "Use atom(s) custom map");
    }
  }
  l = 0;
  for (k=0; k<j; k++)
  {
    if (view -> anim -> last -> img -> color_map[1] == view -> cmap[k])
    {
      l = k; // + ATOM_MAPS;
      break;
    }
  }
  combo_set_active (color_box, l);
  g_signal_connect (G_OBJECT (color_box), "changed", G_CALLBACK(set_poly_color_map_box), view);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, color_box, FALSE, FALSE, 100);
  widget_set_sensitive (color_box, view -> bonding);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("\n<b>Display polyhedra at the edges of the model box\n"
                                                   "using the PBC even if the clones are not shown:</b>", -1, -1, 0.5, 0.5),
                                      FALSE, FALSE, 15);
  GtkWidget * cloned_p = check_button ("Cloned polyhedra", -1, 40, view -> anim -> last -> img -> cloned_poly,
                                       G_CALLBACK(on_cloned_poly_toggled), view);
  widget_set_sensitive ((cloned_p), get_project_by_id(view -> proj) -> cell.pbc);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, TRUE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, cloned_p, FALSE, FALSE, 50);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("This is automatic if the clones are shown.\n"
                                                   "The clones are the replica of the atoms linked by PBC.", -1, -1, 0.5, 0.5),
                                      FALSE, FALSE, 5);
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void close_coord (GtkButton * but, gpointer data)

  \brief environments configuration window close button

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void close_coord (GtkButton * but, gpointer data)
{
  glwin * view = (glwin *)data;
  view -> coord_win -> win = destroy_this_widget (view -> coord_win -> win);
  g_free (view -> coord_win);
  view -> coord_win = NULL;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean close_event_coord (GtkWindow * widg, gpointer data)

  \brief environments configuration window close event callback GTK4

  \param widg the GtkWindow sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean close_event_coord (GtkWindow * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean close_event_coord (GtkWidget * widg, GdkEvent * event, gpointer data)

  \brief environments configuration window close event callback GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean close_event_coord (GtkWidget * widg, GdkEvent * event, gpointer data)
#endif
{
  close_coord (NULL, data);
  return FALSE;
}

/*!
  \fn GtkWidget * advanced_coord_properties (glwin * view, int page)

  \brief create the environments configuration window

  \param view the target glwin
  \param page the page of the notebook to display
*/
GtkWidget * advanced_coord_properties (glwin * view, int page)
{
  gchar * str = g_strdup_printf ("Environments configuration - %s", get_project_by_id(view -> proj)->name);
  GtkWidget * win = create_win (str, view -> win, FALSE, FALSE);
  g_free (str);
  char * rings_short[5] = {"AR", "KR", "GR", "PR", "SR"};
  GtkWidget * vbox = create_vbox (5);
  add_container_child (CONTAINER_WIN, win, vbox);
  gtk_widget_set_size_request (win, 625, 600);
  view -> coord_win -> notebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(view -> coord_win -> notebook), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(view -> coord_win -> notebook), GTK_POS_LEFT);
  show_the_widgets (view -> coord_win -> notebook);
  gtk_widget_set_size_request (view -> coord_win -> notebook, 600, 550);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, view -> coord_win -> notebook, FALSE, FALSE, 0);
  gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), param_tab (view), gtk_label_new ("Parameters"));
  gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), coord_tab (view, 0, 0), markup_label("Total coordination(s) <b>[TC]</b>", -1, -1, 0.0, 0.5));
  gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), coord_tab (view, 1, 0), markup_label("Partial coordination(s) <b>[PC]</b>", -1, -1, 0.0, 0.5));

  int i;
  if (view -> rings)
  {
    for (i=0; i<5; i++)
    {
      if (view -> ring_max[i])
      {
        str = g_strdup_printf ("%s ring(s) <b>[%s]</b>", rings_type[i], rings_short[i]);
        gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), coord_tab (view, i+4, 0), markup_label(str, -1, -1, 0.0, 0.5));
        g_free (str);
      }
    }
  }
  if (view -> chains && view -> chain_max)
  {
    gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), coord_tab (view, 9, 0), markup_label("Chain(s)", -1, -1, 0.0, 0.5));
  }
  gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), coord_tab (view, 0, 1), markup_label("Polyhedra from <b>TC</b>", -1, -1, 0.0, 0.5));
  gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), coord_tab (view, 1, 1), markup_label("Polyhedra from <b>PC</b>", -1, -1, 0.0, 0.5));
  if (view -> rings)
  {
    for (i=0; i<5; i++)
    {
      if (view -> ring_max[i])
      {
        str = g_strdup_printf ("Polyhedra from <b>%s</b>", rings_short[i]);
        gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), coord_tab (view, i+4, 1), markup_label(str, -1, -1, 0.0, 0.5));
        g_free (str);
      }
    }
  }

  if (view -> adv_bonding[0])
  {
    gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), fragmol_tab (view, 2), markup_label ("Fragment(s)", -1, -1, 0.0, 0.5));
  }
  if (view -> adv_bonding[1])
  {
    gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), fragmol_tab (view, 3), markup_label ("Molecule(s)", -1, -1, 0.0, 0.5));
  }

  if (view -> rings)
  {
    for (i=0; i<5; i++)
    {
      if (view -> ring_max[i])
      {
        str = g_strdup_printf ("Isolated ring(s) from <b>%s</b>", rings_short[i]);
        gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), rings_tab (view, i), markup_label(str, -1, -1, 0.0, 0.5));
        g_free (str);
      }
    }
  }

  if (view -> chains && view -> chain_max)
  {
     gtk_notebook_append_page (GTK_NOTEBOOK(view -> coord_win -> notebook), chains_tab (view), markup_label("Isolated chain(s)", -1, -1, 0.0, 0.5));
  }

  GtkWidget * hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, TRUE, FALSE, 0);
  GtkWidget * but = create_button ("Close", IMG_STOCK, FCLOSE, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(close_coord), view);
  add_box_child_end (hbox, but, FALSE, FALSE, 0);
  add_gtk_close_event (win, G_CALLBACK(close_event_coord), view);
  show_the_widgets (win);
  gtk_notebook_set_current_page (GTK_NOTEBOOK (view -> coord_win -> notebook), page);
  return win;
}

/*!
  \fn int get_page_from_geo_coord (glwin * view, int geo, int coord)

  \brief find the proper page to display in the notebook

  \param view the target glwin
  \param geo the type of environment
  \param coord coordination (1) or polyhedra (0)
*/
int get_page_from_geo_coord (glwin * view, int geo, int coord)
{
  int i, j;
  if (geo == 30) return 0;
  if (! coord)
  {
    i = (geo < 2) ? 3 : 0;
    if (view -> rings)
    {
      for (j=0; j<5; j++)
      {
        if (view -> ring_max[j]) i ++;
      }
    }
    i = (geo < 2) ? i + geo : 4 + 2*i;
  }
  else
  {
    if (geo > 1)
    {
      i = 0;
      if (view -> rings)
      {
        for (j=0; j<5; j++)
        {
          if (view -> ring_max[j]) i ++;
        }
      }
      if (geo < 4)
      {
        i = 2*(i+2) + geo - 1;
      }
      else
      {
        i = 0;
        for (j=0; j<5; j++)
        {
          if (view -> ring_max[j])
          {
            if (geo == 4 + j) break;
            i ++;
          }
        }
        i = 3 + i;
      }
    }
    else
    {
      i = geo + 1;
    }
  }
  return i;
}

/*!
  \fn G_MODULE_EXPORT void coord_properties (GtkWidget * widg, gpointer data)

  \brief create the environments configuration window

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void coord_properties (GtkWidget * widg, gpointer data)
{
  tint * id = (tint *) data;
  glwin * view = get_project_by_id(id -> a) -> modelgl;
  int page = get_page_from_geo_coord(view, id -> b, id -> c);
  if (view -> coord_win == NULL)
  {
    view -> coord_win = g_malloc0 (sizeof*view -> coord_win);
    view -> coord_win -> win = advanced_coord_properties (view, page);
  }
  else if (GTK_IS_WIDGET(view -> coord_win -> win))
  {
    show_the_widgets (view -> coord_win -> win);
    gtk_notebook_set_current_page (GTK_NOTEBOOK (view -> coord_win -> notebook), page);
  }
  else
  {
    show_warning ("Error impossible to display the coordination window !", view -> win);
  }
}
