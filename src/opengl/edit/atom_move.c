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
* @file atom_move.c
* @short Functions to move atom(s) and group of atom(s) \n
         Functions to move randomly atom(s) and group of atom(s) \n
         Functions to create the motion widgets of the model edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'atom_move.c'
*
* Contains:
*

 - The functions to move atom(s) and group of atom(s)
 - The functions to move randomly atom(s) and group of atom(s)
 - The functions to create the motion widgets of the model edition window

*
* List of functions:

  float get_limit (int mot, glwin * view);

  double ** save_coordinates (project * this_proj, int status);

  gboolean rebuild_selection (project * this_proj, atom_search * asearch, int filter);
  gboolean random_move_objects (project * this_proj, atom_search * asearch, int numo, int filter, int obj);
  gboolean move_objects (project * this_proj, atom_search * asearch, int action, int axis, vec3_t trans, float ang);

  G_MODULE_EXPORT gboolean scroll_range_move (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);

  void reset_coordinates (project * this_proj, int status);
  void init_coordinates (project * this_proj, int status, gboolean win, gboolean init);
  void translate (project * this_proj, int status, int axis, vec3_t trans);
  void rotate_quat (project * this_proj, vec4_t q, int status, int axis);
  void rotate (project * this_proj, int status, int axis, int raxis, float param);
  void random_move_this_atom (project * this_proj, int aid);
  void random_rotate_this_object (project * this_proj, atomic_object * object, double ratio, double msd);
  void random_translate_this_object (project * this_proj, atomic_object * object, double ratio, double msd);
  void random_move_this_object (project * this_proj, atomic_object * object, int move, double msd);
  void trigger_refresh (project * this_proj, atom_search * asearch);
  void random_move (project * this_proj, atom_search * asearch);
  void translate_this_atom (project * this_proj, int aid, int axis, vec3_t trans);
  void translate_this_object (project * this_proj, atomic_object * object, int axis, vec3_t trans);
  void rotate_this_object (project * this_proj, atomic_object * object, int axis, int rax, float ang);
  void move_selection (project * this_proj, int action, int axis, vec3_t trans, float ang);
  void update_coordinates (project * this_proj, int status, int axis, int action);
  void update_range_and_entry (project * this_proj, int i, int j, int k);
  void range_has_changed (gpointer data, double v);
  void check_motion_interactors (project * this_proj, atom_search * asearch);

  G_MODULE_EXPORT void repeat_move (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_move (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void range_move (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void set_axis_for_motion (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void  set_show_motion_axis (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_show_motion_axis (GtkToggleButton * but, gpointer data);

  GtkWidget * create_axis_entries (atom_search * asearch, project * this_proj, int mot, int axd);
  GtkWidget * add_motion_interaction (atom_search * asearch, int axd, project * this_proj);

*/

#include "atom_edit.h"

gboolean * was_moved_atom;

/*!
  \fn double ** save_coordinates (project * this_proj, int status)

  \brief save atomic coordinates

  \param this_proj the target project
  \param status selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
*/
double ** save_coordinates (project * this_proj, int status)
{
  int i, j;
  i = 0;
  if (status > 1)
  {
    i = this_proj -> natomes;
  }
  else
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      if (this_proj -> atoms[0][j].pick[0] == status) i++;
    }
  }
  if (i == 0) return NULL;

  double ** coords = allocddouble (i, 3);
  i = 0;
  if (status > 1)
  {
    for (i=0; i<this_proj -> natomes; i++)
    {
      coords[i][0] = this_proj -> atoms[0][i].x;
      coords[i][1] = this_proj -> atoms[0][i].y;
      coords[i][2] = this_proj -> atoms[0][i].z;
    }
  }
  else
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      if (this_proj -> atoms[0][j].pick[0] == status)
      {
        coords[i][0] = this_proj -> atoms[0][j].x;
        coords[i][1] = this_proj -> atoms[0][j].y;
        coords[i][2] = this_proj -> atoms[0][j].z;
        i ++;
      }
    }
  }
  return coords;
}

/*!
  \fn void reset_coordinates (project * this_proj, int status)

  \brief reset transformation and restore saved atomic coordinates

  \param this_proj the target project
  \param status selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
*/
void reset_coordinates (project * this_proj, int status)
{
  int i, j;
  i = 0;
  if (this_proj -> modelgl -> saved_coord[status] != NULL)
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      if (this_proj -> atoms[0][j].pick[0] == status || status > 1)
      {
        this_proj -> atoms[0][j].x = this_proj -> modelgl -> saved_coord[status][i][0];
        this_proj -> atoms[0][j].y = this_proj -> modelgl -> saved_coord[status][i][1];
        this_proj -> atoms[0][j].z = this_proj -> modelgl -> saved_coord[status][i][2];
        i ++;
      }
    }
  }
}

/*!
  \fn vec3_t get_bary (project * this_proj, int status)

  \brief get barycenter of atomic coordinates

  \param this_proj the target project
  \param status selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
*/
vec3_t get_bary (project * this_proj, int status)
{
  vec3_t bar = vec3(0.0,0.0,0.0);
  int j;
  float i = 0.0;
  for (j=0; j<this_proj -> natomes; j++)
  {
    if (this_proj -> atoms[0][j].pick[0] == status || status == 2)
    {
      i += 1.0;
      bar = v3_add (bar, vec3(this_proj -> atoms[0][j].x, this_proj -> atoms[0][j].y, this_proj -> atoms[0][j].z));
    }
  }
  if (i > 0.0) bar = v3_divs (bar, i);
  return bar;
}

/*!
  \fn void init_coordinates (project * this_proj, int status, gboolean win, gboolean init)

  \brief preserve atomic coordinates

  \param this_proj the target project
  \param status selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
  \param win is the model edition window opened ?
  \param init preserve atomic coordinates
*/
void init_coordinates (project * this_proj, int status, gboolean win, gboolean init)
{
  if (win)
  {
    int i, j;
    for (i=0; i<2; i++)
    {
      for (j=0; j<6; j++)
      {
        this_proj -> modelgl -> atom_win -> new_param[status][i][j] = this_proj -> modelgl -> edition_param[status][i][j];
        this_proj -> modelgl -> atom_win -> old_param[status][i][j] = this_proj -> modelgl -> edition_param[status][i][j];
      }
    }
    this_proj -> modelgl -> atom_win -> axis[status] = 1;
    this_proj -> modelgl -> atom_win -> show_axis[status] = FALSE;
  }
  this_proj -> modelgl -> baryc[status] = get_bary (this_proj, status);
  if (init)
  {
    this_proj -> modelgl -> saved_coord[status] = save_coordinates (this_proj, status);
  }
}

/*!
  \fn void translate (project * this_proj, int status, int axis, vec3_t trans)

  \brief translate

  \param this_proj the target project
  \param status selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
  \param axis 0 = model, 1 = eye
  \param trans translation vector
*/
void translate (project * this_proj, int status, int axis, vec3_t trans)
{
  int i, j;
  vec3_t c_old, c_new;
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      if (this_proj -> atoms[i][j].pick[0] == status || status < 0)
      {
        c_old = vec3(this_proj -> atoms[i][j].x, this_proj -> atoms[i][j].y, this_proj -> atoms[i][j].z);
        if (axis)
        {
          c_new = m4_mul_pos (this_proj -> modelgl -> view_matrix, c_old);
          c_old = v3_add (c_new, trans);
          c_new = m4_mul_pos (this_proj -> modelgl -> un_view_matrix, c_old);
        }
        else
        {
          c_new = v3_add (c_old, trans);
        }
        this_proj -> atoms[i][j].x = c_new.x;
        this_proj -> atoms[i][j].y = c_new.y;
        this_proj -> atoms[i][j].z = c_new.z;
      }
    }
  }
}

/*!
  \fn void rotate_quat (project * this_proj, vec4_t q, int status, int axis)

  \brief rotate using quaternion

  \param this_proj the target project
  \param q rotation quaternion
  \param status selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
  \param axis 0 = model, 1 = eye
*/
void rotate_quat (project * this_proj, vec4_t q, int status, int axis)
{
  int j;
  mat4_t rot = m4_quat_rotation (q);
  vec3_t c_old, c_new;
  for (j=0; j<this_proj -> natomes; j++)
  {
    if (this_proj -> atoms[0][j].pick[0] == status)
    {
      c_old = vec3(this_proj -> atoms[0][j].x, this_proj -> atoms[0][j].y, this_proj -> atoms[0][j].z);
      c_new = v3_sub(c_old, this_proj -> modelgl -> baryc[status]);
      if (axis)
      {
        c_old = m4_mul_pos (this_proj -> modelgl -> view_matrix, c_new);
        c_new = m4_mul_pos (rot, c_old);
        c_old = m4_mul_pos (this_proj -> modelgl -> un_view_matrix, c_new);
      }
      else
      {
        c_old = m4_mul_pos (rot, c_new);
      }
      c_new = v3_add (c_old, this_proj -> modelgl -> baryc[status]);
      this_proj -> atoms[0][j].x = c_new.x;
      this_proj -> atoms[0][j].y = c_new.y;
      this_proj -> atoms[0][j].z = c_new.z;
    }
  }
}

/*!
  \fn void rotate (project * this_proj, int status, int axis, int raxis, float param)

  \brief rotate

  \param this_proj the target project
  \param status selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
  \param axis 0 = model, 1 = eye
  \param raxis rotation axis 0 = x, 1 = y, 2 = z
  \param param rotation angle
*/
void rotate (project * this_proj, int status, int axis, int raxis, float param)
{
  vec4_t qr;
  vec3_t ax[3];
  ax[0] = vec3(1.0,0.0,0.0);
  ax[1] = vec3(0.0,1.0,0.0);
  ax[2] = vec3(0.0,0.0,1.0);
  qr = axis_to_quat(ax[raxis-3], param*pi/90.0);
  rotate_quat (this_proj, qr, status, axis);
}

/*!
  \fn void random_move_this_atom (project * this_proj, int aid)

  \brief random move atom

  \param this_proj the target project
  \param aid the atom id
*/
void random_move_this_atom (project * this_proj, int aid)
{
  int i, j, k, l;
  // Using CPU time to randomize
  clock_t begin = clock();
  double prob;
  i = (int)begin;
  for (j=0; j<3; j++)
  {
    k = (j+1)*i*aid*(3*this_proj -> numwid);
    prob= random3_(& k);
    k *= k;
    l = (prob <= 0.5) ? 1 : -1;
    prob = random3_(& k);
    switch (j)
    {
      case 0:
        this_proj -> atoms[0][aid].x += l*prob*sqrt(this_proj -> modelgl -> atom_win -> msd[aid]/3.0);
        break;
      case 1:
        this_proj -> atoms[0][aid].y += l*prob*sqrt(this_proj -> modelgl -> atom_win -> msd[aid]/3.0);
        break;
      case 2:
        this_proj -> atoms[0][aid].z += l*prob*sqrt(this_proj -> modelgl -> atom_win -> msd[aid]/3.0);
        break;
    }
  }
}

/*!
  \fn void random_rotate_this_object (project * this_proj, atomic_object * object, double ratio, double msd)

  \brief random rotate an object

  \param this_proj the target project
  \param object the object to rotate
  \param ratio ratio translation / rotation
  \param msd the MSD
*/
void random_rotate_this_object (project * this_proj, atomic_object * object, double ratio, double msd)
{
  int i, j, k, l, m, n;
  vec3_t c_new, c_old;
  vec3_t ax[3];
  ax[0] = vec3(1.0,0.0,0.0);
  ax[1] = vec3(0.0,1.0,0.0);
  ax[2] = vec3(0.0,0.0,1.0);
  vec3_t baryc = vec3(object -> baryc[0], object -> baryc[1], object -> baryc[2]);
  vec4_t qr;
  mat4_t rot;
  // Using CPU time to randomize
  clock_t begin = clock();
  double prob;
  i = (int)begin;
  for (j=0; j<3; j++)
  {
    k = (j+1)*i*(3*this_proj -> numwid);
    prob=random3_(& k);
    k *= k;
    l = (prob <= 0.5) ? 1 : -1;
    prob = random3_(& k);
    qr = axis_to_quat(ax[j], l*prob*sqrt(ratio*msd/3.0)*pi/90.0);
    rot = m4_quat_rotation (qr);
    for (m=0; m<object -> atoms; m++)
    {
      n = object -> at_list[m].id;
      if (! was_moved_atom[n])
      {
        c_old = vec3(object -> at_list[m].x, object -> at_list[m].y, object -> at_list[m].z);
        c_new = m4_mul_pos (rot, c_old);
        object -> at_list[m].x = c_new.x;
        object -> at_list[m].y = c_new.y;
        object -> at_list[m].z = c_new.z;
        c_old = v3_add (c_new, baryc);
        this_proj -> atoms[0][n].x = c_old.x;
        this_proj -> atoms[0][n].y = c_old.y;
        this_proj -> atoms[0][n].z = c_old.z;
      }
    }
  }
}

/*!
  \fn void random_translate_this_object (project * this_proj, atomic_object * object, double ratio, double msd)

  \brief random translate an object

  \param this_proj the target project
  \param object the object to translate
  \param ratio ratio translation / rotation
  \param msd the MSD
*/
void random_translate_this_object (project * this_proj, atomic_object * object, double ratio, double msd)
{
  int i, j, k, l, m, n;
  // Using CPU time to randomize
  clock_t begin = clock();
  double prob;
  i = (int)begin;
  for (j=0; j<3; j++)
  {
    k = (j+1)*i*(3*this_proj -> numwid);
    prob=random3_(& k);
    k *= k;
    l = (prob <= 0.5) ? 1 : -1;
    prob = l*random3_(& k)*sqrt(ratio*msd/3.0);
    switch (j)
    {
      case 0:
        for (m=0; m<object -> atoms; m++)
        {
          n = object -> at_list[m].id;
          if (! was_moved_atom[n]) this_proj -> atoms[0][n].x += prob;
        }
        break;
      case 1:
        for (m=0; m<object -> atoms; m++)
        {
          n = object -> at_list[m].id;
          if (! was_moved_atom[n]) this_proj -> atoms[0][n].y += prob;
        }
        break;
      case 2:
        for (m=0; m<object -> atoms; m++)
        {
          n = object -> at_list[m].id;
          if (! was_moved_atom[n]) this_proj -> atoms[0][n].z += prob;
        }
        break;
    }
    object -> baryc[j] += prob;
  }
}

/*!
  \fn void random_move_this_object (project * this_proj, atomic_object * object, int move, double msd)

  \brief random move object

  \param this_proj the target project
  \param object the object to move
  \param move the number of times to repeat the motion
  \param msd the MSD
*/
void random_move_this_object (project * this_proj, atomic_object * object, int move, double msd)
{
  int i, j;
  // Using CPU time to randomize
  clock_t begin = clock();
  double prob;
  i = (int)begin;
  switch (move)
  {
    case 1:
      random_translate_this_object (this_proj, object, 1.0, msd);
      break;
    case 2:
      random_rotate_this_object (this_proj, object, 1.0, msd);
      break;
    case 3:
      j = object -> species*i*object -> atoms*(3*this_proj -> numwid);
      prob = random3_(& j);
      random_translate_this_object (this_proj, object, prob, msd);
      random_rotate_this_object (this_proj, object, 1.0-prob, msd);
      break;
  }
}

/*!
  \fn void trigger_refresh (project * this_proj, atom_search * asearch)

  \brief refresh search tree model

  \param this_proj the target project
  \param asearch the target atom search
*/
void trigger_refresh (project * this_proj, atom_search * asearch)
{
  clean_all_trees (asearch, this_proj);
  this_proj -> modelgl -> atom_win -> rebuilt[(asearch -> action == DISPL) ? 0 : 1] = TRUE;
  update_search_tree (this_proj -> modelgl -> search_widg[(asearch -> action == DISPL) ? 6 : 2]);
  check_all_trees (this_proj);
}

/*!
  \fn gboolean rebuild_selection (project * this_proj, atom_search * asearch, int filter)

  \brief rebuild selection (split fragments linked thru PBC)

  \param this_proj the target project
  \param asearch the target atom search
  \param filter the filter (0 = species, 1 = partial coord, 2 = total coord, 3 = fragment, 4 = molecule)
*/
gboolean rebuild_selection (project * this_proj, atom_search * asearch, int filter)
{
  int i, j, k, l;
  gboolean was_frag = this_proj -> modelgl -> adv_bonding[0];
  gboolean was_mol = this_proj -> modelgl -> adv_bonding[1];
  gboolean recons = FALSE;
  int old_frag = this_proj -> coord -> totcoord[2];
  i = (asearch -> action == DISPL) ? 0 : 1;
  if (asearch -> object && this_proj -> modelgl -> rebuild[0][i])
  {
    int * saved_todo = duplicate_int (asearch -> todo_size, asearch -> todo);
    int old_tds = asearch -> todo_size;
    g_free (asearch -> todo);
    allocate_todo (asearch, this_proj -> natomes);
    j = 0;
    int * oifcl = NULL;
    atomic_object * object = this_proj -> modelgl -> atom_win -> to_be_moved[i];
    while (object)
    {
      j += object -> ifcl;
      object = object -> next;
    }
    if (j)
    {
      object = this_proj -> modelgl -> atom_win -> to_be_moved[i];
      oifcl = allocint (j);
      l = 0;
      while (object)
      {
        for (k=0; k<object -> ifcl; k++)
        {
          oifcl[l] = object -> bcid[k];
          l ++;
        }
        object = object -> next;
      }
    }
    if (j)
    {
      reconstruct_bonds (this_proj, j, oifcl);
      g_free (oifcl);
      oifcl = NULL;
    }
    object = this_proj -> modelgl -> atom_win -> to_be_moved[i];
    while (object)
    {
      if (object -> ifcl) reconstruct_coordinates_for_object (this_proj, object, TRUE);
      if (filter < 3)
      {
        recons = TRUE;
        for (j=0; j<object -> atoms; j++)
        {
          k = object -> at_list[j].id;
          asearch -> todo[k] = 1;
        }
      }
      object = object -> next;
    }
    if (recons)
    {
      apply_action (this_proj, asearch);
      trigger_refresh (this_proj, asearch);
    }
    g_free (asearch -> todo);
    asearch -> todo = duplicate_int (old_tds, saved_todo);
    asearch -> todo_size = old_tds;
    g_free (saved_todo);
  }
  if ((was_frag && ! this_proj -> modelgl -> adv_bonding[0]) || (was_mol && ! this_proj -> modelgl -> adv_bonding[1]))
  {
    return TRUE;
  }
  else if (old_frag != this_proj -> coord -> totcoord[2])
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn gboolean random_move_objects (project * this_proj, atom_search * asearch, int numo, int filter, int obj)

  \brief random move object(s)

  \param this_proj the target project
  \param asearch the target atom search
  \param numo the number of object(s) to move
  \param filter the filter (0 = species, 1 = partial coord, 2 = total coord, 3 = fragment, 4 = molecule)
  \param obj the object (0 = atom(s), 1 = group of atoms)
*/
gboolean random_move_objects (project * this_proj, atom_search * asearch, int numo, int filter, int obj)
{
  atomic_object * object = this_proj -> modelgl -> atom_win -> to_be_moved[1];
  float v;
  int i, j, k;
  gboolean recons = FALSE;
  if (! this_proj -> modelgl -> atom_win -> rebuilt[1])
  {
    recons = rebuild_selection (this_proj, asearch, filter);
  }
  was_moved_atom = allocbool (this_proj -> natomes);
  for (i=0; i<numo; i++)
  {
    if (asearch -> todo[i] && object)
    {
      v  =  (obj && filter > 2) ? this_proj -> modelgl -> atom_win -> msd_all[i] : this_proj -> modelgl -> atom_win -> msd[i];
      if (v > 0.0)
      {
        for (j=0; j<this_proj -> modelgl -> atom_win -> repeat_move; j++)
        {
          random_move_this_object (this_proj, object, asearch -> todo[i], v);
        }
        for (j=0; j<object -> atoms; j++)
        {
          k = object -> at_list[j].id;
          was_moved_atom[k] = TRUE;
        }
      }
      object = object -> next;
    }
  }
  g_free (was_moved_atom);
  return recons;
}

/*!
  \fn void random_move (project * this_proj, atom_search * asearch)

  \brief random move

  \param this_proj the target project
  \param asearch the target atom search
*/
void random_move (project * this_proj, atom_search * asearch)
{
  int obj = get_asearch_object (asearch);
  int filter = get_asearch_filter (asearch);
  int i, j;
  gboolean recons = FALSE;
  if (this_proj -> modelgl -> atom_win -> to_be_moved[1])
  {
    recons = random_move_objects (this_proj, asearch, asearch -> todo_size, filter, obj);
  }
  else
  {
    if (this_proj -> modelgl -> rebuild[0][1] && ! this_proj -> modelgl -> atom_win -> rebuilt[1])
    {
      apply_action (this_proj, asearch);
      recons = TRUE;
    }
    for (i=0; i<asearch -> todo_size; i++)
    {
      if (asearch -> todo[i] && this_proj -> modelgl -> atom_win -> msd[i] > 0.0)
      {
        for (j=0; j<this_proj -> modelgl -> atom_win -> repeat_move; j++) random_move_this_atom (this_proj, i);
      }
    }
  }
  if (asearch -> recompute_bonding)
  {
    i = activep;
    active_project_changed (activep);
    bonds_update = 1;
    frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
    mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
    active_project -> runc[0] = FALSE;
    on_calc_bonds_released (NULL, NULL);
    active_project_changed (i);
    recons = TRUE;
  }
  this_proj -> modelgl -> was_moved = TRUE;
  init_default_shaders (this_proj -> modelgl);
#ifdef GTK3
  // GTK3 Menu Action To Check
  set_advanced_bonding_menus (this_proj -> modelgl);
#endif
  if (recons) trigger_refresh (this_proj, asearch);
}

/*!
  \fn void translate_this_atom (project * this_proj, int aid, int axis, vec3_t trans)

  \brief translate atom

  \param this_proj the target project
  \param aid atom id
  \param axis 0 = model, 1 = eye
  \param trans translation vector
*/
void translate_this_atom (project * this_proj, int aid, int axis, vec3_t trans)
{
  vec3_t c_old, c_new;
  c_old = vec3(this_proj -> atoms[0][aid].x, this_proj -> atoms[0][aid].y, this_proj -> atoms[0][aid].z);
  if (axis)
  {
    c_new = m4_mul_pos (this_proj -> modelgl -> view_matrix, c_old);
    c_old = v3_add (c_new, trans);
    c_new = m4_mul_pos (this_proj -> modelgl -> un_view_matrix, c_old);
  }
  else
  {
    c_new = v3_add (c_old, trans);
  }
  this_proj -> atoms[0][aid].x = c_new.x;
  this_proj -> atoms[0][aid].y = c_new.y;
  this_proj -> atoms[0][aid].z = c_new.z;
}

/*!
  \fn void translate_this_object (project * this_proj, atomic_object * object, int axis, vec3_t trans)

  \brief translate object

  \param this_proj the target project
  \param object the object to translate
  \param axis 0 = model, 1 = eye
  \param trans the translation vector
*/
void translate_this_object (project * this_proj, atomic_object * object, int axis, vec3_t trans)
{
  int i, j;
  vec3_t c_old, c_new;
  c_old = vec3(object -> baryc[0], object -> baryc[1], object -> baryc[2]);
  if (axis)
  {
    c_new = m4_mul_pos (this_proj -> modelgl -> view_matrix, c_old);
    c_old = v3_add (c_new, trans);
    c_new = m4_mul_pos (this_proj -> modelgl -> un_view_matrix, c_old);
  }
  else
  {
    c_new = v3_add (c_old, trans);
  }
  object -> baryc[0] = c_new.x;
  object -> baryc[1] = c_new.y;
  object -> baryc[2] = c_new.z;
  for (i=0; i<object -> atoms; i++)
  {
    j = object -> at_list[i].id;
    if (! was_moved_atom[j])
    {
      was_moved_atom[j] = TRUE;
      this_proj -> atoms[0][j].x = object -> baryc[0] + object -> at_list[i].x;
      this_proj -> atoms[0][j].y = object -> baryc[1] + object -> at_list[i].y;
      this_proj -> atoms[0][j].z = object -> baryc[2] + object -> at_list[i].z;
    }
  }
}

/*!
  \fn void rotate_this_object (project * this_proj, atomic_object * object, int axis, int rax, float ang)

  \brief rotate object

  \param this_proj the target project
  \param object the object to rotate
  \param axis 0 = model, 1 = eye
  \param rax the rotation axis
  \param ang the rotation angle
*/
void rotate_this_object (project * this_proj, atomic_object * object, int axis, int rax, float ang)
{
  int i, j;
  vec3_t c_new, c_old;
  vec3_t baryc = vec3(object -> baryc[0], object -> baryc[1], object -> baryc[2]);
  vec3_t ax[3];
  ax[0] = vec3(1.0,0.0,0.0);
  ax[1] = vec3(0.0,1.0,0.0);
  ax[2] = vec3(0.0,0.0,1.0);
  vec4_t qr;
  mat4_t rot;
  qr = axis_to_quat(ax[rax], ang*pi/180.0);
  rot = m4_quat_rotation (qr);
  for (i=0; i<object -> atoms; i++)
  {
    j = object -> at_list[i].id;
    if (! was_moved_atom[j])
    {
      c_old = vec3(object -> at_list[i].x, object -> at_list[i].y, object -> at_list[i].z);
      if (axis)
      {
        c_new = m4_mul_pos (this_proj -> modelgl -> view_matrix, c_old);
        c_old = m4_mul_pos (rot, c_new);
        c_new = m4_mul_pos (this_proj -> modelgl -> un_view_matrix, c_old);
      }
      else
      {
        c_new = m4_mul_pos (rot, c_old);
      }
      object -> at_list[i].x = c_new.x;
      object -> at_list[i].y = c_new.y;
      object -> at_list[i].z = c_new.z;
      c_old = v3_add (c_new, baryc);
      this_proj -> atoms[0][j].x = c_old.x;
      this_proj -> atoms[0][j].y = c_old.y;
      this_proj -> atoms[0][j].z = c_old.z;
      was_moved_atom[j] = TRUE;
    }
  }
}

/*!
  \fn gboolean move_objects (project * this_proj, atom_search * asearch, int action, int axis, vec3_t trans, float ang)

  \brief move objects, return reconstruction status

  \param this_proj the target project
  \param asearch the target atom search
  \param action 0 = translation, 1 = rotation
  \param axis 0 = model, 1 = eye
  \param trans the translation vector, if any
  \param ang the rotation angle, if any
*/
gboolean move_objects (project * this_proj, atom_search * asearch, int action, int axis, vec3_t trans, float ang)
{
  gboolean recons = FALSE;
  if (this_proj -> modelgl -> rebuild[0][0] && ! this_proj -> modelgl -> atom_win -> rebuilt[0])
  {
    recons = rebuild_selection (this_proj, asearch, get_asearch_filter(asearch));
  }
  atomic_object * object = this_proj -> modelgl -> atom_win -> to_be_moved[0];
  was_moved_atom = allocbool (this_proj -> natomes);
  int i, j;
  while (object)
  {
    if (action < 3)
    {
      translate_this_object (this_proj, object, axis, trans);
    }
    else
    {
      rotate_this_object (this_proj, object, axis, action-3, ang);
    }
    for (i=0; i<object -> atoms; i++)
    {
      j = object -> at_list[i].id;
      was_moved_atom[j] = TRUE;
    }
    object = object -> next;
  }
  g_free (was_moved_atom);
  return recons;
}

extern atom_search * duplicate_atom_search (atom_search * asearch);

/*!
  \fn void move_selection (project * this_proj, int action, int axis, vec3_t trans, float ang)

  \brief move atom selection

  \param this_proj the target project
  \param action 0 = translation, 1 = rotation
  \param axis 0 = model, 1 = eye
  \param trans the translation vector, if any
  \param ang the rotation angle, if any
*/
void move_selection (project * this_proj, int action, int axis, vec3_t trans, float ang)
{
  atom_search * asearch = duplicate_atom_search (this_proj -> modelgl -> search_widg[2]);
  gboolean recons = FALSE;
  gboolean move_it = do_we_have_objects_in_selection (this_proj, asearch, TRUE);
  if (move_it)
  {
    int i;
    if (this_proj -> modelgl -> atom_win -> to_be_moved[0])
    {
      recons = move_objects (this_proj, asearch, action, axis, trans, ang);
    }
    else
    {
      if (this_proj -> modelgl -> rebuild[0][0] && ! this_proj -> modelgl -> atom_win -> rebuilt[0])
      {
        apply_action (this_proj, asearch);
        recons = TRUE;
      }
      for (i=0; i<asearch -> todo_size; i++)
      {
        if (asearch -> todo[i])
        {
          translate_this_atom (this_proj, i, axis, trans);
        }
      }
    }
    if (asearch -> recompute_bonding)
    {
      i = activep;
      active_project_changed (activep);
      bonds_update = 1;
      frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
      mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
      active_project -> runc[0] = FALSE;
      on_calc_bonds_released (NULL, NULL);
      active_project_changed (i);
      recons = TRUE;
    }
    init_default_shaders (this_proj -> modelgl);
#ifdef GTK3
    // GTK3 Menu Action To Check
    set_advanced_bonding_menus (this_proj -> modelgl);
#endif
    if (recons) trigger_refresh (this_proj, asearch);
  }
}

/*!
  \fn void update_coordinates (project * this_proj, int status, int axis, int action)

  \brief update atomic coordinates on motion

  \param this_proj the target project
  \param status selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
  \param axis axis, 0 = model, 1 = eye
  \param action 0 = translation, 1 = rotation
*/
void update_coordinates (project * this_proj, int status, int axis, int action)
{
  vec3_t trans = vec3(this_proj -> modelgl -> atom_win -> new_param[status][axis][0]-this_proj -> modelgl -> atom_win -> old_param[status][axis][0],
                      this_proj -> modelgl -> atom_win -> new_param[status][axis][1]-this_proj -> modelgl -> atom_win -> old_param[status][axis][1],
                      this_proj -> modelgl -> atom_win -> new_param[status][axis][2]-this_proj -> modelgl -> atom_win -> old_param[status][axis][2]);
  float r = this_proj -> modelgl -> atom_win -> new_param[status][axis][action] - this_proj -> modelgl -> atom_win -> old_param[status][axis][action];
  if (v3_length(trans) != 0.0 || r != 0.0)
  {
    move_selection (this_proj, action, axis, trans, r);
    this_proj -> modelgl -> was_moved = TRUE;
    this_proj -> modelgl -> atom_win -> old_param[status][axis][action] = this_proj -> modelgl -> atom_win -> new_param[status][axis][action];
    init_default_shaders (this_proj -> modelgl);
    update (this_proj -> modelgl);
  }
}

/*!
  \fn G_MODULE_EXPORT void repeat_move (GtkSpinButton * res, gpointer data)

  \brief repeat motion callback

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void repeat_move (GtkSpinButton * res, gpointer data)
{
  project * this_proj = (project *)data;
  this_proj -> modelgl -> atom_win -> repeat_move = gtk_spin_button_get_value_as_int(res);
}

/*!
  \fn void update_range_and_entry (project * this_proj, int i, int j, int k)

  \brief update motion range

  \param this_proj the target project
  \param i selection status, 0 = non selected atom(s), 1 = selected atom(s), 2 = all atom(s)
  \param j axis, 0 = model, 1 = eye
  \param k parameter to update: x, y, z or alpha, beta, gamma
*/
void update_range_and_entry (project * this_proj, int i, int j, int k)
{
  update_entry_double (GTK_ENTRY(this_proj -> modelgl -> atom_win -> edit_entry[k]),
                                   this_proj -> modelgl -> atom_win -> new_param[i][j][k]);
  gtk_range_set_value (GTK_RANGE(this_proj -> modelgl -> atom_win -> edit_scale[k]),
                                   this_proj -> modelgl -> atom_win -> new_param[i][j][k]);
}

/*!
  \fn float get_limit (int mot, glwin * view)

  \brief get motion limit

  \param mot translation (0) or rotation (1)
  \param view the target glwin
*/
float get_limit (int mot, glwin * view)
{
  if (! mot)
  {
    int plimit = (int)(view -> anim -> last -> img -> p_depth/limit[mot]);
    return limit[mot] + 2.0*plimit*limit[mot];
  }
  else
  {
    return limit[mot];
  }
}

/*!
  \fn void range_has_changed (gpointer data, double v)

  \brief motion

  \param data the associated data pointer
  \param v the value for motion
*/
void range_has_changed (gpointer data, double v)
{
  tint * id = (tint *)data;
  project * this_proj = get_project_by_id(id -> a);
  int h, i, j, k;
  h = id -> b - TOLAB;
  i = (h < 3) ? 0 : 1;
  j = this_proj -> modelgl -> atom_win -> axis[i];
  k = this_proj -> modelgl -> search_widg[id -> c] -> status;
  if (v != this_proj -> modelgl -> atom_win -> new_param[k][j][h])
  {
    float plim = get_limit (i, this_proj -> modelgl);
    if (v >= -plim && v <= plim)
    {
      this_proj -> modelgl -> atom_win -> new_param[k][j][h] = v;
      this_proj -> modelgl -> atom_win -> active = i;
      update_coordinates (this_proj, k, j, h);
    }
    update_range_and_entry (this_proj, k, j, h);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_move (GtkEntry * res, gpointer data)

  \brief motion callback - entry

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_move (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  range_has_changed (data, v);
}

/*!
  \fn G_MODULE_EXPORT void range_move (GtkRange * range, gpointer data)

  \brief motion callback - range

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void range_move (GtkRange * range, gpointer data)
{
  range_has_changed (data, gtk_range_get_value (range));
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_range_move (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief motion callback - scroll

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_range_move (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  range_has_changed (data, value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_axis_for_motion (GtkComboBox * box, gpointer data)

  \brief set motion axis (eye or model)

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_for_motion (GtkComboBox * box, gpointer data)
{
  tint * id = (tint *)data;
  project * this_proj = get_project_by_id (id -> a);
  int i, j;
  j = id -> b - TOLAB;
  this_proj -> modelgl -> atom_win -> axis[j] = combo_get_active ((GtkWidget *)box);
  for (i=3*j; i<(j + 1)*3; i++)
  {
    update_range_and_entry (this_proj, this_proj -> modelgl -> search_widg[id -> c] -> status, this_proj -> modelgl -> atom_win -> axis[j], i);
  }
  this_proj -> modelgl -> atom_win -> active = j;
  update (this_proj -> modelgl);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void  set_show_motion_axis (GtkCheckButton * but, gpointer data)

  \brief set show / hide motion axis toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void  set_show_motion_axis (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_show_motion_axis (GtkToggleButton * but, gpointer data)

  \brief set show / hide motion axis toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_show_motion_axis (GtkToggleButton * but, gpointer data)
#endif
{
  tint * id = (tint *)data;
  project * this_proj = get_project_by_id (id -> a);
  int i, j;
  j = id -> b - TOLAB;
  i = button_get_status ((GtkWidget *)but);
  button_set_status (this_proj -> modelgl -> atom_win -> axis_but[! j], i);
  this_proj -> modelgl -> atom_win -> active = j;
  this_proj -> modelgl -> atom_win -> show_axis[j] = i;
  if (i)
  {
    this_proj -> modelgl -> anim -> last -> img -> xyz -> axis = CYLINDERS;
  }
  else
  {
    this_proj -> modelgl -> anim -> last -> img -> xyz -> axis = this_proj -> modelgl -> atom_win -> old_axis;
  }
  this_proj -> modelgl -> create_shaders[MAXIS] = TRUE;
  update (this_proj -> modelgl);
}

/*!
  \fn void check_motion_interactors (project * this_proj, atom_search * asearch)

  \brief add motion check button

  \param this_proj the target project
  \param asearch the target atom search
*/
void check_motion_interactors (project * this_proj, atom_search * asearch)
{
  gboolean activate = do_we_have_objects_in_selection (this_proj, asearch, FALSE);
  int i;
  if (this_proj -> modelgl -> atom_win)
  {
    for (i=0; i<6; i++)
    {
      if (this_proj -> modelgl -> atom_win -> edit_entry[i])
      {
        widget_set_sensitive (this_proj -> modelgl -> atom_win -> edit_entry[i], (((asearch -> object  && ! asearch -> passivating ) || (asearch -> object > 1 && asearch -> passivating)) || i < 3) ? activate : 0);
      }
      if (this_proj -> modelgl -> atom_win -> edit_scale[i])
      {
        widget_set_sensitive (this_proj -> modelgl -> atom_win -> edit_scale[i], (((asearch -> object  && ! asearch -> passivating ) || (asearch -> object > 1 && asearch -> passivating)) || i < 3) ? activate : 0);
      }
    }
  }
}

/*!
  \fn GtkWidget * create_axis_entries (atom_search * asearch, project * this_proj, int mot, int axd)

  \brief create axis entries

  \param asearch the target atom search
  \param this_proj the target project
  \param mot translation (0) or rotation (1)
  \param axd axis (0 = x, 1 = y, 2 = z)
*/
GtkWidget * create_axis_entries (atom_search * asearch, project * this_proj, int mot, int axd)
{
  gchar * str;
  GtkWidget * lab;
  GtkWidget * hbox;
  gchar * axis[3]={"x", "y", "z"};
  gchar * unit[2]={"<b>&#xC5;</b>", "<b>&#xB0;</b>"};
  GtkWidget * vbox = create_vbox (5);
  int j = mot*3 + axd;
  float plim = get_limit (mot, this_proj -> modelgl);
  this_proj -> modelgl -> atom_win -> edit_entry[j] = create_entry (G_CALLBACK(set_move), 100, 15, FALSE, & asearch -> pointer[j]);
  this_proj -> modelgl -> atom_win -> edit_scale[j] = create_hscale (-plim, plim, 0.001,
                                                                     this_proj -> modelgl -> atom_win -> new_param[asearch -> status][1][j], GTK_POS_TOP, 4, 250,
                                                                     G_CALLBACK(range_move), G_CALLBACK(scroll_range_move), & asearch -> pointer[j]);
  widget_set_sensitive (this_proj -> modelgl -> atom_win -> edit_entry[j], 0);
  widget_set_sensitive (this_proj -> modelgl -> atom_win -> edit_scale[j], 0);
  str = g_strdup_printf ("On <b>%s</b> axis:", axis[axd]);
  lab = markup_label(unit[mot], 20, -1, 0.0, 0.5);
  update_entry_double (GTK_ENTRY(this_proj -> modelgl -> atom_win -> edit_entry[j]), this_proj -> modelgl -> atom_win -> new_param[asearch -> status][1][j]);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 70, -1, 0.0, 0.5), FALSE, FALSE, 50);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, this_proj -> modelgl -> atom_win -> edit_scale[j], FALSE, FALSE, 0);
  GtkWidget * fixed = gtk_fixed_new ();
  gtk_fixed_put (GTK_FIXED(fixed), this_proj -> modelgl -> atom_win -> edit_entry[j], 0, 15);
  gtk_fixed_put (GTK_FIXED(fixed), lab, 120, 25);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, FALSE, FALSE, 50);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  return vbox;
}

/*!
  \fn GtkWidget * add_motion_interaction (atom_search * asearch, int axd, project * this_proj)

  \brief add motion interaction widgets

  \param asearch the target atom search
  \param axd translation (0) or rotation (1)
  \param this_proj the target project
*/
GtkWidget * add_motion_interaction (atom_search * asearch, int axd, project * this_proj)
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<u>Select the axis to be used:</u> ", 200, -1, 0.0, 0.5), FALSE, FALSE, 20);
  this_proj -> modelgl -> atom_win -> axis_combo[axd] = create_combo ();
  combo_text_append (this_proj -> modelgl -> atom_win -> axis_combo[axd], "Model axis");
  combo_text_append (this_proj -> modelgl -> atom_win -> axis_combo[axd], "Eye (viewer) axis");
  combo_set_active (this_proj -> modelgl -> atom_win -> axis_combo[axd], this_proj -> modelgl -> atom_win -> axis[axd]);
  g_signal_connect (G_OBJECT (this_proj -> modelgl -> atom_win -> axis_combo[axd]), "changed", G_CALLBACK(set_axis_for_motion), & asearch -> pointer[axd]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, this_proj -> modelgl -> atom_win -> axis_combo[axd], FALSE, FALSE, 20);
  this_proj -> modelgl -> atom_win -> axis_but[axd] = check_button ("Show", 100, 35, FALSE, G_CALLBACK(set_show_motion_axis), & asearch -> pointer[axd]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, this_proj -> modelgl -> atom_win -> axis_but[axd], FALSE, FALSE, 20);
  int i;
  for (i=0; i<3; i++)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_axis_entries (asearch, this_proj, axd, i), FALSE, FALSE, 0);
  }
  hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 20);
  //add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
  //                    check_button ("Reset transformation(s)", -1, 35, FALSE, G_CALLBACK(set_reset_transformation), & asearch -> pointer[0]),
  //                    FALSE, FALSE, 10);
  return vbox;
}
