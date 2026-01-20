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
* @file cell_shift.c
* @short Functions to create the 'shift cell center' tab for the cell edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cell_shift.c'
*
* Contains:
*

 - The functions to create the 'shift cell center' tab for the cell edition window

*
* List of functions:

  G_MODULE_EXPORT gboolean scroll_shift_coord (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);

  void modify_coordinates_in_lattice (project * this_proj, mat4_t * dlat, mat4_t * drec, int refresh, int density);
  void shift_it (vec3_t shift, int refresh, int proj);
  void adjust_it (int refresh, int proj);
  void shift_has_changed (gpointer data, double val);
  void wrapping (glwin * view);

  G_MODULE_EXPORT void set_shift (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void shift_coord (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void wrap_coord (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void wrap_coord (GtkToggleButton * but, gpointer data);

  GtkWidget * create_cell_entries (project * this_proj, int i);
  GtkWidget * create_shift_box (project * this_proj);
  GtkWidget * shift_center_tab (project * this_proj);

*/

#include "cell_edit.h"

/*!
  \fn void modify_coordinates_in_lattice (project * this_proj, mat4_t * dlat, mat4_t * drec, int refresh, int density)

  \brief modify atomic coordinates in lattice

  \param this_proj the target project
  \param dlat lattice vectors matrix
  \param drec reciprocal vectors matrix
  \param refresh refresh rendering data
  \param density 0= shift, 1 = density modification
*/
void modify_coordinates_in_lattice (project * this_proj, mat4_t * dlat, mat4_t * drec, int refresh, int density)
{
  vec3_t pos;
  vec3_t res;
  int i, j;
  box_info * box;
  mat4_t lat, rec;
  if (! density)
  {
    box = & this_proj -> cell.box[0];
    lat = mat4 (box -> vect[0][0], box -> vect[1][0], box -> vect[2][0], 0.0,
                box -> vect[0][1], box -> vect[1][1], box -> vect[2][1], 0.0,
                box -> vect[0][2], box -> vect[1][2], box -> vect[2][2], 0.0,
                0.0, 0.0, 0.0, 1.0);
    rec = mat4 (box -> rvect[0][0], box -> rvect[0][1], box -> rvect[0][2], 0.0,
                box -> rvect[1][0], box -> rvect[1][1], box -> rvect[1][2], 0.0,
                box -> rvect[2][0], box -> rvect[2][1], box -> rvect[2][2], 0.0,
                0.0, 0.0, 0.0, 1.0);
  }
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      pos = vec3(this_proj -> atoms[i][j].x, this_proj -> atoms[i][j].y, this_proj -> atoms[i][j].z);
      if (density)
      {
        res = m4_mul_pos(*drec, pos);
        pos = m4_mul_pos(*dlat, res);
      }
      else
      {
        if (this_proj -> cell.npt && i)
        {
          box = & this_proj -> cell.box[i];
          lat = mat4 (box -> vect[0][0], box -> vect[1][0], box -> vect[2][0], 0.0,
                      box -> vect[0][1], box -> vect[1][1], box -> vect[2][1], 0.0,
                      box -> vect[0][2], box -> vect[1][2], box -> vect[2][2], 0.0,
                      0.0, 0.0, 0.0, 1.0);
          rec = mat4 (box -> rvect[0][0], box -> rvect[0][1], box -> rvect[0][2], 0.0,
                      box -> rvect[1][0], box -> rvect[1][1], box -> rvect[1][2], 0.0,
                      box -> rvect[2][0], box -> rvect[2][1], box -> rvect[2][2], 0.0,
                      0.0, 0.0, 0.0, 1.0);
        }
        res = m4_mul_pos(rec, pos);
        res.x -= (int)res.x;
        res.x -= (int)(res.x/0.5);
        res.y -= (int)res.y;
        res.y -= (int)(res.y/0.5);
        res.z -= (int)res.z;
        res.z -= (int)(res.z/0.5);
        pos = m4_mul_pos(lat, res);
      }
      this_proj -> atoms[i][j].x = pos.x;
      this_proj -> atoms[i][j].y = pos.y;
      this_proj -> atoms[i][j].z = pos.z;
    }
    if (density)
    {
      for (j=0; j<this_proj -> modelgl -> bonds[i][1]; j++)
      {
        pos = vec3(this_proj -> modelgl -> clones[i][j].x, this_proj -> modelgl -> clones[i][j].y, this_proj -> modelgl -> clones[i][j].z);
        res = m4_mul_pos(*drec, pos);
        pos = m4_mul_pos(*dlat, res);
        this_proj -> modelgl -> clones[i][j].x = pos.x;
        this_proj -> modelgl -> clones[i][j].y = pos.y;
        this_proj -> modelgl -> clones[i][j].z = pos.z;
      }
    }
  }
  if (refresh)
  {
    i = activep;
    active_project_changed (this_proj -> id);
    if (! density)
    {
      this_proj -> dmtx = run_distance_matrix (NULL, 0, 1);
#ifdef GTK3
      // GTK3 Menu Action To Check
      widget_set_sensitive (active_glwin -> ogl_clones[0], active_glwin -> allbonds[1]);
#endif
    }
    else
    {
      /*if (this_proj -> modelgl -> adv_bonding[1])
      {
        for (j=0; j<2; j++)
        {
          if (this_proj -> modelgl -> anim -> last -> img -> color_map[j] == 4 || this_proj -> modelgl -> anim -> last -> img -> color_map[j] == 5)
          {
            if (this_proj -> modelgl -> color_styles[j*ATOM_MAPS])
            {
              gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> color_styles[j*ATOM_MAPS], TRUE);
              set_color_map (this_proj -> modelgl -> color_styles[j*ATOM_MAPS], & this_proj -> modelgl  -> colorp[j*ATOM_MAPS][0]);
            }
          }
        }
      }*/
      /*this_proj -> modelgl -> bonding = FALSE;
      this_proj -> modelgl -> adv_bonding[0] = this_proj -> modelgl -> adv_bonding[1] = FALSE;
      if (this_proj -> force_field[0])
      {
        g_free (this_proj -> force_field[0]);
        this_proj -> force_field[0] = NULL;
      }*/
      /*int j;
      if (this_proj -> modelgl -> rings)
      {
        this_proj -> modelgl -> rings = FALSE;
        for (j=0; j<5; j++)
        {
          clean_rings_data (j, this_proj -> modelgl);
#ifdef GTK3
          update_rings_menus (this_proj -> modelgl);
#endif
        }
      }
      if (this_proj -> modelgl -> chains)
      {
        clean_chains_data (this_proj -> modelgl);
#ifdef GTK3
        update_chains_menus (this_proj -> modelgl);
#endif
      }*/
    }
    init_default_shaders (this_proj -> modelgl);
    if (density) this_proj -> modelgl -> create_shaders[MDBOX] = TRUE;
    if (this_proj -> modelgl -> n_shaders[SLABS][0]) this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
    update (this_proj -> modelgl);
    if (i != this_proj -> id) active_project_changed (i);
  }
}

/*!
  \fn void shift_it (vec3_t shift, int refresh, int proj)

  \brief shift atomic coordinates

  \param shift the shift vector
  \param refresh refresh rendering data
  \param proj the target project id
*/
void shift_it (vec3_t shift, int refresh, int proj)
{
  project * this_proj = get_project_by_id (proj);
  translate (this_proj, -1, 0, shift);
  modify_coordinates_in_lattice (this_proj, NULL, NULL, refresh, 0);
}


/*!
  \fn void adjust_it (int refresh, int proj)

  \brief adjust atomic coordinates

  \param refresh refresh rendering data
  \param proj the project id
*/
void adjust_it (int refresh, int proj)
{
  int i, j;
  project * this_proj = get_project_by_id (proj);
  box_info * box = & this_proj -> cell.box[0];
  mat4_t rec = mat4 (box -> rvect[0][0], box -> rvect[0][1], box -> rvect[0][2], 0.0,
                     box -> rvect[1][0], box -> rvect[1][1], box -> rvect[1][2], 0.0,
                     box -> rvect[2][0], box -> rvect[2][1], box -> rvect[2][2], 0.0,
                     0.0, 0.0, 0.0, 1.0);
  for (i=0; i<3; i++) box -> param[0][i] = this_proj -> modelgl -> cell_win -> cparam[i+3];
  i = this_proj -> cell.ltype;
  this_proj -> cell.ltype = 1;
  j = activep;
  active_project_changed (this_proj -> id);
  this_proj -> cell.ltype = i;
  active_project_changed (j);
  mat4_t lat = mat4 (box -> vect[0][0], box -> vect[1][0], box -> vect[2][0], 0.0,
                     box -> vect[0][1], box -> vect[1][1], box -> vect[2][1], 0.0,
                     box -> vect[0][2], box -> vect[1][2], box -> vect[2][2], 0.0,
                     0.0, 0.0, 0.0, 1.0);
  modify_coordinates_in_lattice (this_proj, & lat, & rec, refresh, 1);
  double m = 0.0;
  for (i=0; i<this_proj -> nspec; i++) m += this_proj -> chemistry -> nsps[i] * this_proj -> chemistry -> chem_prop[CHEM_M][i];
  this_proj -> cell.density = 10.0*m/(this_proj -> cell.volume*6.02214179);
  display_density (this_proj -> modelgl -> cell_win,
                   this_proj -> cell.volume,
                   this_proj -> cell.density, this_proj -> natomes/this_proj -> cell.volume);
}

/*!
  \fn void shift_has_changed (gpointer data, double val)

  \brief shift atomic coordinates

  \param data the associated data pointer
  \param val the shift value
*/
void shift_has_changed (gpointer data, double val)
{
  tint * dat = (tint *)data;
  int i, j, k, l;
  project * this_proj = get_project_by_id (dat -> a);
  cell_edition * cedit = this_proj -> modelgl -> cell_win;
  l = (this_proj -> cell.npt) ? this_proj -> modelgl -> anim -> last -> img -> step : 0;
  if (dat -> b < 3)
  {
    if (val >= - this_proj -> cell.box[l].param[0][dat -> b]/2.0 && val <= this_proj -> cell.box[l].param[0][dat -> b]/2.0)
    {
      if (val != cedit -> cparam[dat -> b])
      {
        double cparam[3];
        for (i=0; i<3; i++) cparam[i] = - cedit -> cparam[i];
        shift_it (vec3(cparam[0], cparam[1], cparam[2]), 0, dat -> a);
        cedit -> cparam[dat -> b] = val;
        for (i=0; i<3; i++) cparam[i] = cedit -> cparam[i];
        shift_it (vec3(cparam[0], cparam[1], cparam[2]), 1, dat -> a);
      }
    }
  }
  else if (dat -> b < 6)
  {
    if (val > 0.0 && val <= cedit -> initbox[dat -> b-3]*100.0)
    {
      if (val != cedit -> cparam[dat -> b])
      {
        adjust_it (0, dat -> a);
        if (cedit -> homo_density)
        {
          double v = val / cedit -> initbox[dat -> b - 3];
          for (i=0; i<3; i++) cedit -> cparam[i+3] = v * cedit -> initbox[i];
        }
        else
        {
          cedit -> cparam[dat -> b] = val;
        }
        adjust_it (1, dat -> a);
      }
    }
  }
  else if (dat -> b > 5 && dat -> b < 15)
  {
    i = (dat -> b < 9) ? 6 : (dat -> b < 12) ? 9 : dat -> b;
    j = (dat -> b < 9) ? 1 : (dat -> b < 12) ? 2 : 10;
    k = (dat -> b < 9) ? 1 : 0;
    if (val > -k * cedit -> initbox[dat -> b - i] && val <= j * cedit -> initbox[dat -> b - i])
    {
      if (val != cedit -> cparam[dat -> b])
      {
        cedit -> cparam[dat -> b] = val;
        if (this_proj -> modelgl -> n_shaders[SLABS][0])
        {
          this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
          update (this_proj -> modelgl);
        }
      }
    }
  }
  else if (dat -> b > 14)
  {
    i = (dat -> b < 18) ? 0 : 1;
    if (val >= -i * 180.0 && val <= 180.0)
    {
      if (val != cedit -> cparam[dat -> b])
      {
        cedit -> cparam[dat -> b] = val;
        if (this_proj -> modelgl -> n_shaders[SLABS][0])
        {
          this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
          update (this_proj -> modelgl);
        }
      }
    }
  }
  if ((dat -> b > 2 && dat -> b < 6) && cedit -> homo_density)
  {
    for (i=3; i<6; i++)
    {
      update_entry_double (GTK_ENTRY(cedit -> edit_entry[i]), cedit -> cparam[i]);
      gtk_range_set_value (GTK_RANGE(cedit -> edit_scale[i]), cedit -> cparam[i]);
    }
  }
  else
  {
    update_entry_double (GTK_ENTRY(cedit -> edit_entry[dat -> b]), cedit -> cparam[dat -> b]);
    gtk_range_set_value (GTK_RANGE(cedit -> edit_scale[dat -> b]), cedit -> cparam[dat -> b]);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_shift (GtkEntry * res, gpointer data)

  \brief set atomic coordinates shift

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_shift (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  shift_has_changed (data, v);
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_shift_coord (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief shift coordinates callback - scroll

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_shift_coord (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  shift_has_changed (data, value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void shift_coord (GtkRange * range, gpointer data)

  \brief shift coordinates callback - range

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void shift_coord (GtkRange * range, gpointer data)
{
  shift_has_changed (data, gtk_range_get_value (range));
}

/*!
  \fn GtkWidget * create_cell_entries (project * this_proj, int i)

  \brief create the cell entry widgets

  \param this_proj the target project
  \param i target parameter/action id (shift, cut, density)
*/
GtkWidget * create_cell_entries (project * this_proj, int i)
{
  int j, k, l, m;
  gchar * str;
  GtkWidget * lab;
  GtkWidget * hbox;
  gchar * axis[3]={"x", "y", "z"};
  gchar * angl[3]={"&#x3B1;", "&#x3B2;", "&#x263;"};
  gchar * unit[2]={"<b>&#xC5;</b>", "<b>°</b>"};
  GtkWidget * vbox = create_vbox (5);
  cell_edition * cedit = this_proj -> modelgl -> cell_win;
  cedit -> edit_entry[i] = create_entry (G_CALLBACK(set_shift), 100, 15, FALSE, & this_proj -> modelgl -> colorp[i][0]);
  if (i < 3 || (i > 5 && i < 12))
  {
    j = (i < 3) ? i : (i < 9) ? i - 6 : i - 9;
    k = (i < 3) ? 2 : 1;
    l = (i < 9) ? 1 : 0;
    m = (i < 9) ? 1 : 2;
    cedit -> edit_scale[i] = create_hscale (-l*this_proj -> cell.box[0].param[0][j]/k, m*this_proj -> cell.box[0].param[0][j]/k, 0.01, cedit -> cparam[i],
                                            GTK_POS_TOP, 3, 200, G_CALLBACK(shift_coord), G_CALLBACK(scroll_shift_coord), & this_proj -> modelgl -> colorp[i][0]);
    str = g_strdup_printf ("on <b>%s</b> axis [+/- &#xC5;]", axis[j]);
  }
  else if (i < 6)
  {
    cedit -> initbox[i-3] = cedit -> cparam[i] = this_proj -> cell.box[0].param[0][i-3];
    cedit -> edit_scale[i] = create_hscale (0.0, this_proj -> cell.box[0].param[0][i-3]*10.0, 0.01, cedit -> cparam[i],
                                            GTK_POS_TOP, 3, 200, G_CALLBACK(shift_coord), G_CALLBACK(scroll_shift_coord), & this_proj -> modelgl -> colorp[i][0]);
    str = g_strdup_printf ("Lattice <b>%s</b> [+/- &#xC5;]", box_prop[0][i-3]);
  }
  else if (i == 12)
  {
    cedit -> edit_scale[i] = create_hscale (0.0, this_proj -> cell.box[0].param[0][0]*10.0, 0.01, cedit -> cparam[i],
                                              GTK_POS_TOP, 3, 200, G_CALLBACK(shift_coord), G_CALLBACK(scroll_shift_coord), & this_proj -> modelgl -> colorp[i][0]);
    str = g_strdup_printf ("Length [+/- &#xC5;]");
  }
  else if (i > 14 && i < 18)
  {
    cedit -> edit_scale[i] = create_hscale (0.0, 180.0, 0.01, cedit -> cparam[i],
                                            GTK_POS_TOP, 3, 200, G_CALLBACK(shift_coord), G_CALLBACK(scroll_shift_coord), & this_proj -> modelgl -> colorp[i][0]);
    str = g_strdup_printf ("Angle <b>%s</b> [+/- °]", angl[i-15]);
  }
  else if (i > 17)
  {
    cedit -> edit_scale[i] = create_hscale (-180, 180.0, 0.01, cedit -> cparam[i],
                                            GTK_POS_TOP, 3, 200, G_CALLBACK(shift_coord), G_CALLBACK(scroll_shift_coord), & this_proj -> modelgl -> colorp[i][0]);
    str = g_strdup_printf ("on <b>%s</b> axis [+/- °]", axis[i-18]);
  }
  else
  {
    cedit -> edit_scale[i] = create_hscale (0.0, this_proj -> cell.box[0].param[0][0]*10.0, 0.01, cedit -> cparam[i],
                                            GTK_POS_TOP, 3, 200, G_CALLBACK(shift_coord), G_CALLBACK(scroll_shift_coord), & this_proj -> modelgl -> colorp[i][0]);
    str = g_strdup_printf ("Radius [+/- &#xC5;]");
  }

  update_entry_double (GTK_ENTRY(cedit -> edit_entry[i]), cedit -> cparam[i]);
  lab = markup_label(unit[(i > 14 && i < 18) ? 1 : 0], 20, -1, 0.0, 0.5);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 110, -1, 0.0, 0.5), FALSE, FALSE, 0);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, cedit -> edit_scale[i], FALSE, FALSE, 20);
  GtkWidget * fixed = gtk_fixed_new ();
  gtk_fixed_put (GTK_FIXED(fixed), cedit -> edit_entry[i], 0, 15);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, FALSE, FALSE, 0);
  fixed = gtk_fixed_new ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, FALSE, FALSE, 5);
  gtk_fixed_put (GTK_FIXED(fixed), lab, (i>2 && i<6) ? -50 : 0, 25);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  return vbox;
}

/*!
  \fn GtkWidget * create_shift_box (project * this_proj)

  \brief create shift box widgets

  \param this_proj the target project
*/
GtkWidget * create_shift_box (project * this_proj)
{
  GtkWidget * vbox = create_vbox (BSEP);
  int i;
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("Shift the position of the cell center <sup>*</sup>: ", -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
  for (i=0; i<3; i++) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_cell_entries (this_proj, i), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("* requires all atoms to be wrapped in the unit cell", -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
  widget_set_sensitive (vbox, this_proj -> modelgl -> wrapped);
  show_the_widgets (vbox);
  return vbox;
}

/*!
  \fn void wrapping (glwin * view)

  \brief wrapping atomic coordinates

  \param view the target glwin
*/
void wrapping (glwin * view)
{
  gchar * text = "You are about to put all the atoms back inside the model box\n"
                 "This action is irreversible, proceed anyway ?";
  if (ask_yes_no ("Wrap atomic coordinates in unit cell ?", text, GTK_MESSAGE_WARNING, view -> win))
  {
    shift_it (vec3 (0.0, 0.0, 0.0), 1, view -> proj);
    view -> wrapped = TRUE;
  }
  if (view -> cell_win)
  {
    if (view -> cell_win -> shift_box[0])
    {
      if (GTK_IS_WIDGET(view -> cell_win -> shift_box[0]))
      {
        widget_set_sensitive (view -> cell_win -> shift_box[0], view -> wrapped);
      }
    }
  }
#ifdef GTK3
  // GTK3 Menu Action To Check
  if (GTK_IS_WIDGET(view -> ogl_box[2]))
  {
    widget_set_sensitive (view -> ogl_box[2], ! view -> wrapped);
  }
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void wrap_coord (GtkCheckButton * but, gpointer data)

  \brief wrap atomic coordinates callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void wrap_coord (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void wrap_coord (GtkToggleButton * but, gpointer data)

  \brief wrap atomic coordinates callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void wrap_coord (GtkToggleButton * but, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  wrapping (view);
  widget_set_sensitive (GTK_WIDGET(but), ! view -> wrapped);
}

/*!
  \fn GtkWidget * shift_center_tab (project * this_proj)

  \brief create the shift cell center tab

  \param this_proj the target project
*/
GtkWidget * shift_center_tab (project * this_proj)
{
  GtkWidget * layout = create_layout (350, 250);
  glwin * view = this_proj -> modelgl;
  view -> cell_win -> shift_box[0] = create_shift_box (this_proj);
  view -> cell_win -> put_in_box = check_button ("Wrap atomic coordinates in unit cell", -1, -1, FALSE, G_CALLBACK(wrap_coord), view);
  widget_set_sensitive (view -> cell_win -> put_in_box, ! view -> wrapped);
  if (view -> record) widget_set_sensitive (view -> cell_win -> put_in_box, FALSE);
  layout_add_widget (layout, view -> cell_win -> put_in_box, 20, 20);
  layout_add_widget (layout, view -> cell_win -> shift_box[0], 100, 100);
  show_the_widgets (layout);
  return layout;
}
