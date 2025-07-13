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
* @file cbuild_info.c
* @short Functions to create the space group information dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cbuild_info.c'
*
* Contains:
*

 - The functions to create the space group information dialog

*
* List of functions:

  gchar * get_bravais (int spg);
  gchar * get_frac (float val);

  void get_wyck_char (float val, int ax, int bx);
  void get_extra_val (float val, int ax);
  void get_wyck_names (space_group * spg, int i, int j);
  void set_wisible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);

  static void fill_wyckoff_model (GtkTreeStore * store, space_group * spg);

  G_MODULE_EXPORT void set_so_info (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void show_sg_info (GtkWidget * but, gpointer data);

  GtkWidget * create_wyckoff_tree (space_group * spg);
  GtkWidget * create_setting_info (space_group * spg, int sid);
  GtkWidget * create_wyck_pts_info (space_group * spg, int sid);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "cbuild_edit.h"

extern int get_crystal_id (int spg);
extern int get_bravais_img_id (int spg);
extern gchar * frac_string (gchar * init);
extern gchar * get_num_string (gchar * str);
extern gchar * get_so_string (space_group * spg, int id);
extern GtkTreeModel * so_combo_tree (space_group * spg);

extern gchar * tmp_pos;
extern gchar * latt_info[7];

GtkWidget * info_hsbox;
GtkWidget * info_vs;
GtkWidget * wypts_vbox;
GtkWidget * info_wyck_pts;
GtkWidget * info_wyck_scroll;
GtkWidget * info_wyck_tree;

gchar * wnpos[3] = {NULL, NULL, NULL};

/*!
  \fn gchar * get_bravais (int spg)

  \brief retrieve the space group bravais lattice

  \param spg the target space group id
*/
gchar * get_bravais (int spg)
{
  if (groups[spg-1][0] == 'A' || groups[spg-1][0] == 'C')
  {
    return "Base-centered";
  }
  else if (groups[spg-1][0] == 'F')
  {
    return "Face-centered";
  }
  else if (groups[spg-1][0] == 'I')
  {
    return "Body-centered";
  }
  else if (groups[spg-1][0] == 'P')
  {
    return "Primitive";
  }
  else if (groups[spg-1][0] == 'R')
  {
    return "Rhombohedral";
  }
  else
  {
    return NULL;
  }
}

/*!
  \fn gchar * get_frac (float val)

  \brief get string for value

  \param val the target value
*/
gchar * get_frac (float val)
{
  float vallist[12] = {1.0, 1.0/2.0, 1.0/4.0, 3.0/4.0, 1.0/8.0, 3.0/8.0, 5.0/8.0, 7.0/8.0, 1.0/3.0, 2.0/3.0, 1.0/6.0, 5.0/6.0};
  gchar * charlist[12] = {"1", "1/2", "1/4", "3/4", "1/8", "3/8", "5/8", "7/8", "1/3", "2/3", "1/6", "5/6"};
  int i;
  for (i=0; i<12; i++)
  {
    if (fabs(val) - vallist[i] == 0.0) return g_strdup_printf ("%s", charlist[i]);
  }
  return NULL;
}

/*!
  \fn void get_wyck_char (float val, int ax, int bx)

  \brief convert wyckoff value to string

  \param val the target value
  \param ax axis id, x = 0, y = 1, z = 2
  \param bx axis label 0 = "x", 1 = "y", 2 = "z"
*/
void get_wyck_char (float val, int ax, int bx)
{
  if (wnpos[ax])
  {
    if (fabs(val) == 1.0)
    {
      if (val > 0.0)
      {
        wnpos[ax] = g_strdup_printf ("%s+%s", wnpos[ax], vect_comp[bx]);
      }
      else
      {
        wnpos[ax] = g_strdup_printf ("%s-%s", wnpos[ax], vect_comp[bx]);
      }
    }
    else if (val != 0.0)
    {
      if (val > 0.0)
      {
        wnpos[ax] = g_strdup_printf ("%s+%s%s", wnpos[ax], get_frac(val), vect_comp[bx]);
      }
      else
      {
        wnpos[ax] = g_strdup_printf ("%s-%s%s", wnpos[ax], get_frac(val), vect_comp[bx]);
      }
    }
  }
  else
  {
    if (fabs(val) == 1.0)
    {
      if (val > 0.0)
      {
        wnpos[ax] = g_strdup_printf ("%s", vect_comp[bx]);
      }
      else
      {
        wnpos[ax] = g_strdup_printf ("-%s", vect_comp[bx]);
      }
    }
    else if (val != 0.0)
    {
      if (val > 0.0)
      {
        wnpos[ax] = g_strdup_printf ("%s%s", get_frac(val), vect_comp[bx]);
      }
      else
      {
        wnpos[ax] = g_strdup_printf ("-%s%s", get_frac(val), vect_comp[bx]);
      }
    }
  }
}

/*!
  \fn void get_extra_val (float val, int ax)

  \brief convert wyckoff extra value to string

  \param val the target value
  \param ax axis id, x = 0, y = 1, z = 2
*/
void get_extra_val (float val, int ax)
{
  if (val)
  {
    if (wnpos[ax])
    {
      if (val > 0.0)
      {
        wnpos[ax] = g_strdup_printf ("%s+%s", wnpos[ax], get_frac(val));
      }
      else
      {
        wnpos[ax] = g_strdup_printf ("%s-%s", wnpos[ax], get_frac(val));
      }
    }
    else
    {
      float v = (val > 0.0) ? val : 1.0 + val;
      v = (v == 1.0) ? 0.0 : v;
      if (v != 0.0) wnpos[ax] = g_strdup_printf ("%s", get_frac(v));
    }
  }
}

/*!
  \fn void get_wyck_names (space_group * spg, int i, int j)

  \brief get the name of this wyckoff position

  \param spg the target space group
  \param i the wyckoff position id
  \param j the multiplicity for this wyckoff position
*/
void get_wyck_names (space_group * spg, int i, int j)
{
  int k, l;
  double spgpos[3][4];
  mat4_t wpos;
  for (k=0; k<3; k++)
  {
    tmp_pos = g_strdup_printf ("%s", spg -> wyckoff[i].pos[j][k]);
    for (l=0; l<3; l++)
    {
      spgpos[k][l] = get_val_from_wyckoff (vect_comp[l], spg -> wyckoff[i].pos[j][k]);
    }
    if (tmp_pos)
    {
      spgpos[k][3] = get_value_from_pos (tmp_pos);
      g_free (tmp_pos);
      tmp_pos = NULL;
    }
    else
    {
      spgpos[k][3] = 0.0;
    }
  }
  wpos = mat4 (spgpos[0][0], spgpos[0][1], spgpos[0][2], spgpos[0][3],
               spgpos[1][0], spgpos[1][1], spgpos[1][2], spgpos[1][3],
               spgpos[2][0], spgpos[2][1], spgpos[2][2], spgpos[2][3],
               0.0, 0.0, 0.0, 1.0);
  if (i == spg -> numw - 1) m4_print (wpos);
  wpos = m43_mul(spg -> wyck_origin, wpos);
  for (k=0; k<3; k++)
  {
    if (wnpos[k])
    {
      g_free (wnpos[k]);
      wnpos[k] = NULL;
    }
  }

  get_wyck_char (wpos.m00, 0, 0);
  get_wyck_char (wpos.m10, 0, 1);
  get_wyck_char (wpos.m20, 0, 2);
  get_extra_val (wpos.m30, 0);

  get_wyck_char (wpos.m01, 1, 0);
  get_wyck_char (wpos.m11, 1, 1);
  get_wyck_char (wpos.m21, 1, 2);
  get_extra_val (wpos.m31, 1);

  get_wyck_char (wpos.m02, 2, 0);
  get_wyck_char (wpos.m12, 2, 1);
  get_wyck_char (wpos.m22, 2, 2);
  get_extra_val (wpos.m32, 2);

  for (k=0; k<3; k++) if (! wnpos[k]) wnpos[k] = g_strdup_printf ("0");
}

/*!
  \fn static void fill_wyckoff_model (GtkTreeStore * store, space_group * spg)

  \brief fill wyckoff position tree store

  \param store the tree store to fill
  \param spg the target space group
*/
static void fill_wyckoff_model (GtkTreeStore * store, space_group * spg)
{
  GtkTreeIter wlevel;
  GtkTreeIter mlevel;
  int i, j, k;
  gchar * str;
  gchar * strw[3];
  if (spg)
  {
    for (i=0; i<spg -> numw; i++)
    {
      gtk_tree_store_append (store, & wlevel, NULL);
      str = g_strdup_printf ("%d", spg -> wyckoff[i].multi);
      gtk_tree_store_set (store, & wlevel, 0, 1,
                                           1, i+1,
                                           2, str,
                                           3, spg -> wyckoff[i].let,
                                           4, spg -> wyckoff[i].site, -1);
      g_free (str);
      for (j=0; j<spg -> wyckoff[i].multi; j++)
      {
        get_wyck_names (spg, i, j);
        gtk_tree_store_append (store, & mlevel, & wlevel);
        for (k= 0; k<3; k++) strw[k] = frac_string (wnpos[k]);
        str = g_strdup_printf ("(%s,%s,%s)", strw[0], strw[1], strw[2]);
        for (k= 0; k<3; k++) g_free (strw[k]);
        gtk_tree_store_set (store, & mlevel, 0, 0, 5, str, -1);
        g_free (str);
      }
    }
  }
}

/*!
  \fn void set_wisible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer, if visible then add or not pango markup

  \param col the tree view column
  \param renderer the cell renderer
  \param mod the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
void set_wisible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  gtk_tree_model_get (mod, iter, 0, & j, -1);
  if ((j && (i > 1 && i < 5)) || (!j && i==5))
  {
    gchar * str = NULL;
    gtk_tree_model_get (mod, iter, i, & str, -1);
    g_object_set (renderer, "markup", str, NULL, NULL);
    g_free (str);
  }
  gtk_cell_renderer_set_visible (renderer, (i<5) ? j : ! j);
}

/*!
  \fn GtkWidget * create_wyckoff_tree (space_group * spg)

  \brief create wyckoff position tree

  \param spg the target space group
*/
GtkWidget * create_wyckoff_tree (space_group * spg)
{
  int i;
  GtkTreeViewColumn * wcol[6];
  GtkCellRenderer * wcell[6];
  gchar * witle[6] = {" ", "Id.", "Multiplicity", "Letter", "Symmetry", "Coordinates"};
  gchar * wtype[6]={"text", "text", "text", "text", "text", "text"};
  GType w_type[6] = {G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING};
  GtkTreeStore * wmodel = gtk_tree_store_newv (6, w_type);
  GtkWidget * wtree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(wmodel));
  for (i=0; i<6; i++)
  {
    wcell[i] = gtk_cell_renderer_text_new();
    wcol[i] =  gtk_tree_view_column_new_with_attributes(witle[i], wcell[i], wtype[i], i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(wtree), wcol[i]);
    gtk_tree_view_column_set_cell_data_func (wcol[i], wcell[i], set_wisible, GINT_TO_POINTER(i), NULL);
    if (!i) gtk_tree_view_column_set_visible (wcol[i], FALSE);
  }
  fill_wyckoff_model (wmodel, spg);
  g_object_unref (wmodel);
  return wtree;
}

/*!
  \fn GtkWidget * create_setting_info (space_group * spg, int sid)

  \brief create setting information label

  \param spg the target space group
  \param sid the setting id
*/
GtkWidget * create_setting_info (space_group * spg, int sid)
{
  gchar * sxyz[3];
  int i;
  for (i=0; i<3; i++)
  {
    sxyz[i] = g_strdup_printf ("%s", spg -> settings[sid].pos[i]);
    sxyz[i] = frac_string (sxyz[i]);
  }
  gchar * str = g_strdup_printf ("<b>(%s,%s,%s)</b>", sxyz[0], sxyz[1], sxyz[2]);
  for (i=0; i<3; i++) g_free (sxyz[i]);

  GtkWidget * label = markup_label(str, 90, -1, 0.0, 0.5);
  g_free (str);
  return label;
}

/*!
  \fn GtkWidget * create_wyck_pts_info (space_group * spg, int sid)

  \brief create wyckoff label

  \param spg the target space group
  \param sid the setting id
*/
GtkWidget * create_wyck_pts_info (space_group * spg, int sid)
{
  int i, j;
  gchar * str;
  gchar * sxyz[3];
  if (spg -> settings[sid].nump > 1)
  {
    for (i=0; i<spg -> settings[sid].nump; i++)
    {
      for (j=0; j<3; j++)
      {
        sxyz[j] = g_strdup_printf ("%s", spg -> settings[sid].points[i][j]);
        sxyz[j] = frac_string (sxyz[j]);
      }
      if (! i)
      {
        str = g_strdup_printf ("<b>+(%s,%s,%s)</b>", sxyz[0], sxyz[1], sxyz[2]);
      }
      else
      {
        str = g_strdup_printf ("%s\t<b>+(%s,%s,%s)</b>", str, sxyz[0], sxyz[1], sxyz[2]);
      }
      for (j=0; j<3; j++) g_free (sxyz[j]);
    }

    return markup_label(str, -1, -1, 0.5, 0.5);
  }
  else
  {
    return NULL;
  }
}

/*!
  \fn G_MODULE_EXPORT void set_so_info (GtkComboBox * box, gpointer data)

  \brief change space group origin

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_so_info (GtkComboBox * box, gpointer data)
{
  builder_edition * cbuilder = (builder_edition *)data;
  int i = combo_get_active ((GtkWidget *)box);
  combo_set_active (cbuilder -> so_combo, i);
  if (i > -1)
  {
    info_vs = destroy_this_widget (info_vs);
    info_vs = create_setting_info (cbuilder -> cell.sp_group, i);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, info_hsbox, info_vs, FALSE, FALSE, 5);
    show_the_widgets (info_vs);
    info_wyck_pts = destroy_this_widget(info_wyck_pts);
    info_wyck_pts = create_wyck_pts_info (cbuilder -> cell.sp_group, i);
    if (info_wyck_pts)
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, wypts_vbox, info_wyck_pts, FALSE, FALSE, 0);
      show_the_widgets (wypts_vbox);
    }
    if (info_wyck_scroll)
    {
      info_wyck_tree = destroy_this_widget (info_wyck_tree);
      info_wyck_tree = create_wyckoff_tree (cbuilder -> cell.sp_group);
      add_container_child (CONTAINER_SCR, info_wyck_scroll, info_wyck_tree);
      show_the_widgets (info_wyck_scroll);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void show_sg_info (GtkWidget * but, gpointer data)

  \brief show space group information dialog callback

  \param but the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_sg_info (GtkWidget * but, gpointer data)
{
  tint * id = (tint *) data;
  space_group * spg = get_project_by_id(id -> a) -> modelgl -> builder_win -> cell.sp_group;
  gchar * str = g_strdup_printf ("%s info", groups[spg -> id-1]);
  str = substitute_string (str, "<sub>", NULL);
  str = substitute_string (str, "</sub>", NULL);
  GtkWidget * info = dialogmodal (str, GTK_WINDOW(get_project_by_id(id -> a) -> modelgl -> builder_win -> win));
  GtkWidget * vbox, * hbox;
  vbox = dialog_get_content_area (info);
  GtkWidget * ivbox[2];
  GtkWidget * ihbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, ihbox, FALSE, FALSE, 0);
  int xsize = 150;
  int i;
  for (i=0; i<2; i++)
  {
    ivbox[i] = create_vbox(i);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ihbox, ivbox[i], FALSE, FALSE, 0);
  }

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Space group: ", xsize, -1, 0.0, 0.5), FALSE, FALSE, 5);
  str = g_strdup_printf ("<b>%s</b>", groups[spg -> id-1]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 5);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ivbox[0], hbox, FALSE, FALSE, 5);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Number: ", xsize, -1, 0.0, 0.5), FALSE, FALSE, 5);
  str = g_strdup_printf ("<b>%d</b>", spg -> id);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 5);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ivbox[0], hbox, FALSE, FALSE, 5);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Hermman-Mauguin: ", xsize, -1, 0.0, 0.5), FALSE, FALSE, 5);
  str = g_strdup_printf ("%s", spg -> hms);
  str = g_strdup_printf ("<b>%s</b>", get_num_string(str));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 5);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ivbox[0], hbox, FALSE, FALSE, 5);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Crystal system: ", xsize, -1, 0.0, 0.5), FALSE, FALSE, 5);
  str = g_strdup_printf ("<b>%s</b>", spg -> bravais);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 5);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ivbox[0], hbox, FALSE, FALSE, 5);

  str = get_bravais(spg -> id);
  if (str)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Bravais lattice: ", xsize, -1, 0.0, 0.5), FALSE, FALSE, 5);
    str = g_strdup_printf ("<b>%s</b>", str);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 5);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, ivbox[0], hbox, FALSE, FALSE, 5);
  }

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Lattice constraints: ", xsize, -1, 0.0, 0.5), FALSE, FALSE, 5);
  str = g_strdup_printf ("<b>%s</b>", latt_info[get_crystal_id (spg -> id)]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 150, -1, 0.0, 0.5), FALSE, FALSE, 5);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ivbox[0], hbox, FALSE, FALSE, 5);

  GtkWidget * bvimg = gtk_image_new_from_file (bravais_img[get_bravais_img_id(spg -> id)]);
#ifdef GTK4
  gtk_widget_set_size_request (bvimg, 200, 200);
  gtk_widget_set_hexpand (bvimg, TRUE);
  gtk_widget_set_vexpand (bvimg, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ivbox[1], bvimg, FALSE, FALSE, 5);

  info_hsbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, info_hsbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, info_hsbox, markup_label("Setting(s): ", xsize, -1, 0.0, 0.5), FALSE, FALSE, 5);
  if (spg -> nums > 1)
  {
    GtkTreeModel * model = so_combo_tree (spg);
    GtkWidget *  sbox = gtk_combo_box_new_with_model (model);
    g_object_unref (model);
    GtkCellRenderer * renderer = gtk_cell_renderer_combo_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (sbox), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (sbox), renderer, "text", 0, NULL);
    gtk_widget_set_size_request (sbox, 150, 25);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, info_hsbox, sbox, FALSE, FALSE, 5);
    combo_set_active (sbox, 0);
    g_signal_connect (G_OBJECT(sbox), "changed", G_CALLBACK(set_so_info), get_project_by_id(id -> a) -> modelgl -> builder_win);
    combo_set_markup (sbox);
  }
  else
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, info_hsbox, markup_label(get_so_string (spg, 0), 50, -1, 0.0, 0.5), FALSE, FALSE, 5);
  }

  info_vs = create_setting_info (spg, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, info_hsbox, info_vs, FALSE, FALSE, 0);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Wyckoff position(s): ", xsize, -1, 0.0, 0.5), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  wypts_vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, wypts_vbox, FALSE, FALSE, 5);
  info_wyck_pts = create_wyck_pts_info (spg, 0);
  if (info_wyck_pts) add_box_child_start (GTK_ORIENTATION_VERTICAL, wypts_vbox, info_wyck_pts, FALSE, FALSE, 0);
  info_wyck_scroll = create_scroll (NULL, 400, 200, GTK_SHADOW_NONE);
  info_wyck_tree =  create_wyckoff_tree (spg);
  add_container_child (CONTAINER_SCR, info_wyck_scroll, info_wyck_tree);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, info_wyck_scroll, FALSE, FALSE, 5);

  show_the_widgets (info);
  run_this_gtk_dialog (info, G_CALLBACK(run_destroy_dialog), NULL);
}
