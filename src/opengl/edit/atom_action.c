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
* @file atom_action.c
* @short Functions to apply the edition actions to the model
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'atom_action.c'
*
* Contains:
*

 - The functions to apply the edition actions to the model

*
* List of functions:

  int action_atoms_from_project (project * this_proj, atom_search * asearch, gboolean visible);

  gboolean do_we_have_objects_in_selection (project * this_proj, atom_search * asearch, gboolean editing);

  void free_dummies (dummy_atom * tmp_pick);
  void clean_this_project (project * this_proj);
  void clean_motion_search (project * this_proj, atom_search * asearch, int sid);
  void clean_all_trees (atom_search * asearch, project * this_proj);
  void apply_action (project * this_proj, atom_search * asearch);
  void prepare_random_action (project * this_proj, atom_search * asearch);

  G_MODULE_EXPORT void take_action (GtkButton * but, gpointer data);

  atom_search * duplicate_atom_search (atom_search * asearch);

*/

#include "atom_edit.h"
#include "curve.h"
#include "preferences.h"
#include "bind.h"

extern GLdouble get_max_depth (GLdouble depth);
extern void setup_default_lights (project * this_proj, image * img);

/*!
  \fn void free_dummies (dummy_atom * tmp_pick)

  \brief free atom dummy list

  \param tmp_pick the atom dummy list to free
*/
void free_dummies (dummy_atom * tmp_pick)
{
  while (tmp_pick)
  {
    if (tmp_pick -> next)
    {
      tmp_pick = tmp_pick -> next;
      g_free (tmp_pick -> prev);
      tmp_pick -> prev = NULL;
    }
    else
    {
      g_free (tmp_pick);
      tmp_pick = NULL;
    }
  }
}

/*!
  \fn void clean_this_project (project * this_proj)

  \brief clean project data for the edition process

  \param this_proj the target project
*/
void clean_this_project (project * this_proj)
{
  int i, j;
  opengl_project_changed (this_proj -> id);
  selected_aspec = -1;
  is_selected = -1;
  for (i=0; i<opengl_project -> natomes; i++)
  {
    opengl_project -> atoms[0][i].show[0] = opengl_project -> atoms[0][i].show[1] = FALSE;
  }
  gboolean vis_stat = opengl_project -> modelgl -> atom_win -> visible;
  opengl_project -> modelgl -> atom_win -> visible = FALSE;
  i = opengl_project -> modelgl -> selection_mode;
#ifdef GTK4
  select_unselect_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][0]);
  label_unlabel_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][0]);
#else
  select_unselect_atoms (NULL, & opengl_project -> modelgl -> colorp[0][0]);
  label_unlabel_atoms (NULL, & opengl_project -> modelgl -> colorp[0][0]);
#endif
  opengl_project -> modelgl -> selection_mode = NSELECTION-1;
#ifdef GTK4
  select_unselect_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][0]);
  label_unlabel_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][0]);
#else
  select_unselect_atoms (NULL, & opengl_project -> modelgl -> colorp[0][0]);
  label_unlabel_atoms (NULL, & opengl_project -> modelgl -> colorp[0][0]);
#endif
  opengl_project -> modelgl -> selection_mode = i;
  opengl_project -> modelgl -> atom_win -> visible = vis_stat;
#ifdef GTK4
  label_unlabel_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][0]);
#else
  label_unlabel_atoms (NULL, & opengl_project -> modelgl -> colorp[0][0]);
#endif
  opengl_project -> modelgl -> create_shaders[RINGS] = TRUE;
  opengl_project -> modelgl -> n_shaders[RINGS][0] = -1;
  for (i=0; i<2; i++)
  {
    for (j=0; j<opengl_project -> coord -> totcoord[i]; j++)
    {
      opengl_project -> modelgl  -> anim -> last -> img -> show_poly[i][j] = FALSE;
    }
  }
  opengl_project -> modelgl -> create_shaders[POLYS] = TRUE;
  opengl_project -> modelgl -> n_shaders[POLYS][0] = -1;
  opengl_project -> natomes = 0;
  g_free (this_proj -> atoms[0]);
  for (i=0; i<2; i++)
  {
    if (opengl_project -> modelgl -> bonds[0][i])
    {
      if (opengl_project -> modelgl -> bondid[0][i])
      {
        g_free (opengl_project -> modelgl -> bondid[0][i]);
        opengl_project -> modelgl -> bondid[0][i] = NULL;
      }
    }
    opengl_project -> modelgl -> bonds[0][i] = 0;
    opengl_project -> modelgl -> allbonds[i] = 0;
  }
  g_free (opengl_project -> chemistry);
  opengl_project -> chemistry = NULL;
  init_curves_and_calc (opengl_project);
  opengl_project -> numwid = -1;
  frag_update = mol_update = 0;
  prep_calc_actions ();
  free_glwin_spec_data (opengl_project, opengl_project -> nspec);
  glwin_init_spec_data (opengl_project, 1);
  opengl_project -> nspec = 0;
  for (i=0; i<10; i++) opengl_project -> coord -> totcoord[i] = 0;
#ifdef GTK3
  // GTK3 Menu Action To Check
  for (i=1; i<OGL_COORDS; i++) opengl_project -> modelgl -> ogl_coord[i] = NULL;
  for (i=0; i<OGL_RINGS; i++) opengl_project -> modelgl -> ogl_rings[i] = NULL;
  opengl_project -> modelgl -> ogl_chains[0] = NULL;
#endif
  prepare_opengl_menu_bar (opengl_project -> modelgl);
}

/*!
  \fn void clean_motion_search (project * this_proj, atom_search * asearch, int sid)

  \brief clean atom search motion data

  \param this_proj the target project
  \param asearch the target atom search
  \param sid motion id
*/
void clean_motion_search (project * this_proj, atom_search * asearch, int sid)
{
  int i;
  if (asearch -> in_selection)
  {
    for (i=0; i<asearch -> todo_size; i++) asearch -> todo[i] = 0;
    asearch -> in_selection = 0;
    if (this_proj -> modelgl -> atom_win -> to_be_moved[sid])
    {
      atomic_object * object = this_proj -> modelgl -> atom_win -> to_be_moved[sid];
      while (object -> next)
      {
        object = object -> next;
        g_free (object -> prev);
      }
      g_free (object);
      this_proj -> modelgl -> atom_win -> to_be_moved[sid] = NULL;
    }
  }
  this_proj -> modelgl -> atom_win -> rebuilt[! sid] = TRUE;
  this_proj -> modelgl -> atom_win -> rebuilt[sid] = FALSE;
}

/*!
  \fn int action_atoms_from_project (project * this_proj, atom_search * asearch, gboolean visible)

  \brief apply atom edition action to project (motion, remove, replace, insert, random move)

  \param this_proj the target project
  \param asearch the target atom search
  \param visible is the 'model edition' window visible (1/0)
*/
int action_atoms_from_project (project * this_proj, atom_search * asearch, gboolean visible)
{
  int i, j, k, l, m, n, o, p;
  dummy_atom * to_rem, * to_add;
  dummy_atom * tmp_rem, * tmp_add;
  atomic_object * object_list = NULL;
  atomic_object * tmp_list;
  atomic_object * object, * tmp_obj;
  int remove, extra, nmols;
  int act = (asearch -> pointer[0].c == 3) ? 0 : (asearch -> pointer[0].c == 5) ? 1 : 3;

  to_rem = tmp_rem = NULL;
  to_add = tmp_add = NULL;
  remove = extra = nmols = 0;
  atom_edition * edit = this_proj -> modelgl -> atom_win;
  edit -> add_spec = 0;
  if (this_proj -> nspec)
  {
    edit -> new_z = duplicate_double (this_proj -> nspec, this_proj -> chemistry -> chem_prop[CHEM_Z]);
    edit -> coord = duplicate_coord_info (this_proj -> coord);
  }
  else if (! this_proj -> natomes)
  {
    edit -> coord = NULL;
    this_proj -> modelgl -> anim -> last -> img -> xyz -> axis = default_axis.axis;
  }
  gboolean passivate = FALSE;
  if (asearch -> action == REMOVE)
  {
    if (this_proj -> modelgl -> cell_win)
    {
      if (this_proj -> modelgl -> cell_win -> slab_passivate) passivate = TRUE;
    }
  }

  if (asearch -> action == DISPL || asearch -> action == REMOVE || asearch -> action == RANMOVE)
  {
    for (i=0; i<this_proj -> natomes; i++)
    {
      if (asearch -> todo[i])
      {
        if (to_rem != NULL)
        {
          tmp_rem -> next = g_malloc0 (sizeof*tmp_rem);
          tmp_rem -> next -> prev = tmp_rem;
          tmp_rem = tmp_rem -> next;
        }
        else
        {
          to_rem = g_malloc0 (sizeof*to_rem);
          tmp_rem = to_rem;
        }
        tmp_rem -> id = i;
        remove ++;
      }
    }
  }
  else
  {
    object = edit -> to_be_inserted[act];
    i = o = p = 0;
    while (object)
    {
      j = object -> id;
/* #ifdef DEBUG
      g_debug ("object -> id= %d, name= %s, todo[%d]= %d", j+1, object -> name, j+1, asearch -> todo[j]);
#endif */
      if (asearch -> todo[j])
      {
        if (edit -> coord)
        {
          o += search_for_new_spec (edit, object);
        }
        else
        {
          edit -> coord = duplicate_coord_info (object -> coord);
          edit -> new_z = allocdouble (edit -> coord -> species);
          for (k=0; k<edit -> coord -> species; k++) edit -> new_z[k] = (double)object -> old_z[k];
          o += object -> species;
        }
        asearch -> in_selection --;
        if (! object_list)
        {
          object_list = duplicate_atomic_object (object);
          tmp_list = object_list;
        }
        else
        {
          tmp_list -> next = duplicate_atomic_object (object);
          tmp_list -> next -> prev = tmp_list;
          tmp_list = tmp_list -> next;
        }
        for (k=0; k<object -> atoms; k++)
        {
          if (to_add)
          {
            tmp_add -> next = g_malloc0 (sizeof*tmp_add);
            tmp_add -> next -> prev = tmp_add;
            tmp_add = tmp_add -> next;
          }
          else
          {
            to_add = g_malloc0 (sizeof*to_add);
            tmp_add = to_add;
          }
          tmp_add -> id = this_proj -> natomes + extra - remove;
          l = object -> at_list[k].sp;
          tmp_add -> type = find_spec_id (edit -> coord -> species, object -> old_z[l], edit -> new_z);
          tmp_add -> xyz[0] = object -> at_list[k].x + object -> baryc[0];
          tmp_add -> xyz[1] = object -> at_list[k].y + object -> baryc[1];
          tmp_add -> xyz[2] = object -> at_list[k].z + object -> baryc[2];
          for (m=0; m<2; m++)
          {
            tmp_add -> coord[m] = find_this_geo_id (m, object -> coord, object -> old_z, object -> at_list[k].coord[m],
                                                    l, tmp_add -> type, edit -> coord, edit -> new_z);

          }
          for (m=2; m<4; m++) tmp_add -> coord[m] = object -> at_list[k].coord[2] + i;
          if (this_proj -> coord)
          {
            for (m=2; m<4; m++) tmp_add -> coord[m] += this_proj -> coord -> totcoord[m];
          }
          tmp_add -> numv = object -> at_list[k].numv;
          if (tmp_add -> numv)
          {
            tmp_add -> vois = duplicate_int (object -> at_list[k].numv, object -> at_list[k].vois);
            for (m=0; m<tmp_add -> numv; m++) tmp_add -> vois[m] += p + this_proj -> natomes;
            sort (tmp_add ->numv, tmp_add -> vois);
          }
          extra ++;
        }
        i += object -> coord -> totcoord[2];
        p += object -> atoms;
        if (object -> prev)
        {
          if (object -> next)
          {
            object -> prev -> next = object -> next;
            object -> next -> prev = object -> prev;
            tmp_obj = object;
            object = object -> next;
            g_free (tmp_obj);
          }
          else
          {
            object -> prev -> next = NULL;
            g_free (object);
            object = NULL;
          }
        }
        else
        {
          if (object -> next)
          {
            edit -> to_be_inserted[act] = object -> next;
            object = object -> next;
            g_free (object -> prev);
            object -> prev = NULL;
          }
          else
          {
            g_free (edit -> to_be_inserted[act]);
            edit -> to_be_inserted[act] = NULL;
            object = NULL;
          }
        }
      }
      else
      {
        object = object -> next;
      }
    }
    nmols += i;
    edit -> coord -> totcoord[2] += i;
    edit -> add_spec = o;
  }

#ifdef DEBUG
  if (asearch -> action == INSERT && extra)
  {
    g_debug ("Project coord before insert:");
    if (this_proj -> natomes)
    {
      print_coord_info (this_proj, this_proj -> coord);
    }
    else
    {
      g_debug (" *** None *** ");
    }
    g_debug ("Coord info after object insert:");
    print_coord_info (NULL, edit -> coord);
  }
#endif

  if (asearch -> action == DISPL && remove == 0) return -1;
  if (asearch -> action == RANMOVE && remove == 0) return -1;
  if (asearch -> action == REMOVE && remove == 0) return -1;
  if ((asearch -> action == INSERT || asearch -> action == REPLACE) && extra == 0) return extra;
  if (asearch -> action == REMOVE && remove > this_proj -> natomes) return -1;

  if ((asearch -> action != DISPL && asearch -> action != RANMOVE)
  || (asearch -> passivating && asearch -> filter < 3)
  || (asearch -> action == RANMOVE && asearch -> passivating && asearch -> object < 2)
  || (asearch -> action == RANMOVE && ! asearch -> passivating && ! asearch -> object))
  {
    this_proj -> modelgl -> bonding = FALSE;
    this_proj -> coord -> totcoord[3] = 0;
    if (this_proj -> modelgl -> adv_bonding[1])
    {
      for (i=0; i<2; i++)
      {
        if (this_proj -> modelgl -> anim -> last -> img -> color_map[i] == 4 || this_proj -> modelgl -> anim -> last -> img -> color_map[i] == 5)
        {
#ifdef GTK3
          // GTK3 Menu Action To Check
          if (this_proj -> modelgl -> color_styles[i*ATOM_MAPS])
          {
            gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> color_styles[i*ATOM_MAPS], TRUE);
            set_color_map (this_proj -> modelgl -> color_styles[i*ATOM_MAPS], & this_proj -> modelgl  -> colorp[i*ATOM_MAPS][0]);
          }
#endif
        }
      }
    }
    this_proj -> modelgl -> adv_bonding[1] = FALSE;
    if (this_proj -> force_field[0])
    {
      g_free (this_proj -> force_field[0]);
      this_proj -> force_field[0] = NULL;
    }
    if (asearch -> action != DISPL && asearch -> action != RANMOVE)
    {
      if (this_proj -> modelgl -> custom_map)
      {
        g_free (this_proj -> modelgl -> custom_map);
        this_proj -> modelgl -> custom_map = NULL;
        for (i=0; i<2; i++)
        {
          j = i*ATOM_MAPS;
#ifdef GTK3
          gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> color_styles[j], TRUE);
          set_color_map (this_proj -> modelgl -> color_styles[j], & this_proj -> modelgl -> colorp[j][0]);
          if (i) widget_set_sensitive (this_proj -> modelgl -> color_styles[j+6], 0);
#endif
        }
      }
    }
  }

  // Clean curves data
  for (i=0 ; i<NGRAPHS ; i++)
  {
    this_proj -> visok[i]=FALSE;
    hide_curves (this_proj, i);
    erase_curves (this_proj, i);
  }
  if (this_proj -> modelgl -> rings)
  {
    this_proj -> modelgl -> rings = FALSE;
    for (i=0; i<5; i++)
    {
      clean_rings_data (i, this_proj -> modelgl);
    }
#ifdef GTK3
    update_rings_menus (this_proj -> modelgl);
#endif
  }
  if (this_proj -> modelgl -> chains)
  {
    clean_chains_data (this_proj -> modelgl);
#ifdef GTK3
    update_chains_menus (this_proj -> modelgl);
#endif
  }
  clean_volumes_data (this_proj -> modelgl);

  if (asearch -> action == REMOVE && remove == this_proj -> natomes)
  {
    clean_this_project (this_proj);
    return remove;
  }

  atom * new_list = NULL;
  atom * tmp_new = NULL;
  gboolean * showfrag;
  int ** tmpgeo[2];
  int new_atoms = 0;
  int * old_id = NULL;
  if (this_proj -> natomes)
  {
    old_id = allocint (this_proj -> natomes);
    tmp_rem = to_rem;
    for (i=0; i<this_proj -> natomes; i++)
    {
      if (tmp_rem && tmp_rem -> id == i)
      {
        old_id[i] = -(i+1);
        if (asearch -> action == DISPL || asearch -> action == RANMOVE || passivate)
        {
          if (new_list)
          {
            tmp_new -> next = duplicate_atom (& this_proj -> atoms[0][i]);
            tmp_new -> next -> prev = tmp_new;
            tmp_new = tmp_new -> next;
          }
          else
          {
            new_list = duplicate_atom (& this_proj -> atoms[0][i]);
            tmp_new = new_list;
          }
          new_atoms ++;
        }
        if (tmp_rem -> next != NULL) tmp_rem = tmp_rem -> next;
      }
      else
      {
        old_id[i] = i+1;
        if (new_list)
        {
          tmp_new -> next = duplicate_atom (& this_proj -> atoms[0][i]);
          tmp_new -> next -> prev = tmp_new;
          tmp_new = tmp_new -> next;
        }
        else
        {
          new_list = duplicate_atom (& this_proj -> atoms[0][i]);
          tmp_new = new_list;
        }
        new_atoms ++;
      }
    }
    if (asearch -> action == DISPL || asearch -> action == REMOVE || asearch -> action == RANMOVE)
    {
      check_coord_modification (this_proj, old_id, new_list, NULL, TRUE, passivate);
      // old_id for atoms to be passivated (removed then replaced) have been corrected to be > 0,
      // accordingly the total number of atoms to save must be updated
      if (passivate)
      {
        i = 0;
        tmp_new = new_list;
        while (tmp_new)
        {
          if (old_id[tmp_new -> id] > 0) i++;
          tmp_new = tmp_new -> next;
        }
        new_atoms = i;
      }
    }
    // Preserving coordination information
    for (i=0; i<2; i++)
    {
      j= this_proj -> nspec + edit -> add_spec;
      tmpgeo[i] = g_malloc (j*sizeof*tmpgeo[i]);
      for (k=0; k<j; k++)
      {
        tmpgeo[i][k] = allocint (edit -> coord -> ntg[i][k]);
      }
    }
    tmp_new = new_list;
    while (tmp_new)
    {
      j = tmp_new -> id;
      k = tmp_new -> sp;
      // g_debug ("       id= %d, c[0]= %d, c[1]= %d", j, tmp_new -> coord[0], tmp_new -> coord[1]);
      if (! passivate || old_id[j] > 0)
      {
        for (l=0; l<2; l++)
        {
          m = tmp_new -> coord[l];
          if (m >= edit -> coord -> ntg[l][k])
          {
            g_warning ("Error: at= %d, sp= %d, l= %d, geo_id= %d, edit -> coord -> ntg[%d][%d]= %d", j+1, k, l, m, l, k, edit -> coord -> ntg[l][k]);
          }
          tmpgeo[l][k][m] ++;
        }
      }
      tmp_new = tmp_new -> next;
    }

    if (asearch -> action != INSERT && asearch -> action != REPLACE)
    {
      showfrag = remove_bonds_from_project (this_proj, NULL, old_id, new_list, (asearch -> action == DISPL || asearch -> action == RANMOVE) ? FALSE : TRUE, passivate);
    }
    else
    {
      i = edit -> coord -> totcoord[2];
      j = this_proj -> coord -> totcoord[2];
      showfrag = allocbool (i);
      for (k=0; k<j; k++)
      {
        showfrag[k] = this_proj -> modelgl -> anim -> last -> img -> show_coord[2][k];
      }
      for (k=j; k<i; k++) showfrag[k] = TRUE;
    }
  }
  else
  {
    for (i=0; i<2; i++)
    {
      tmpgeo[i] = g_malloc (edit -> coord -> species*sizeof*tmpgeo[i]);
      for (j=0; j<edit -> coord -> species; j++)
      {
        tmpgeo[i][j] = allocint (edit -> coord -> ntg[i][j]);
      }
    }
    showfrag = allocbool(edit -> coord -> totcoord[2]);
    for (j=0; j<edit -> coord -> totcoord[2]; j++)
    {
      showfrag[j] = TRUE;
    }
  }

  if (asearch -> action == DISPL || asearch -> action == REMOVE || asearch -> action == RANMOVE)
  {
    free_dummies (to_rem);
    to_rem = NULL;
    to_add = NULL;
    tmp_new = new_list;
    i = k = 0;
    while (tmp_new)
    {
      if (old_id[tmp_new -> id] > 0 || asearch -> action != REMOVE)
      {
        if (tmp_new -> pick[0] || tmp_new -> pick[1])
        {
          if (to_add != NULL)
          {
            tmp_add -> next = g_malloc0 (sizeof*tmp_add);
            tmp_add -> next -> prev = tmp_add;
            tmp_add = tmp_add -> next;
          }
          else
          {
            to_add = g_malloc0 (sizeof*to_add);
            tmp_add = to_add;
          }
          for (j=0; j<2; j++) tmp_add -> pick[j] = tmp_new -> pick[j];
          tmp_add -> id = i;
          k ++;
       }
       i ++;
      }
      tmp_new = tmp_new -> next;
    }
  }
  else
  {
    tmp_add = to_add;
    if (new_list)
    {
      tmp_new = new_list;
      while (tmp_new -> next) tmp_new = tmp_new -> next;
    }
    while (tmp_add)
    {
      if (new_list)
      {
        tmp_new -> next = g_malloc0 (sizeof*tmp_new -> next);
        tmp_new -> next -> prev = tmp_new;
        tmp_new = tmp_new -> next;
      }
      else
      {
        new_list = g_malloc0 (sizeof*new_list);
        tmp_new = new_list;
      }
      tmp_new -> sp = tmp_add -> type;
      tmp_new -> show[0] = tmp_new -> show[1] = TRUE;
      tmp_new -> x = tmp_add -> xyz[0];
      tmp_new -> y = tmp_add -> xyz[1];
      tmp_new -> z = tmp_add -> xyz[2];
      for (i=0; i<4; i++) tmp_new -> coord[i] = tmp_add -> coord[i];
      i = tmp_new -> sp;
      for (j=0; j<2; j++)
      {
        k = tmp_add -> coord[j];
        tmpgeo[j][i][k] ++;
      }
      // Neighbors
      tmp_new -> numv = tmp_add -> numv;
      if (tmp_new -> numv) tmp_new -> vois = duplicate_int (tmp_new -> numv, tmp_add -> vois);

      new_atoms ++;
      tmp_add -> pick[0] = TRUE;
      tmp_add = tmp_add -> next;
    }

    atomic_object * object = object_list;
    i = 0;
    while (object)
    {
      i += object -> bonds;
       object = object -> next;
    }
    int ** new_bond_list = allocdint (i, 2);
    i = j = 0;
    object = object_list;
    while (object)
    {
      add_bonds_to_list (new_bond_list, i, j, object);
      i += object -> atoms;
      j += object -> bonds;
      if (object -> next)
      {
        object = object -> next;
        g_free (object -> prev);
      }
      else
      {
        g_free (object);
        object = NULL;
      }
    }
    add_bonds_to_project (this_proj, remove, j, new_bond_list);
  }

  if (this_proj -> natomes)
  {
    opengl_project_changed (this_proj -> id);
    is_selected = selected_aspec = -1;
    gboolean vis_stat = opengl_project -> modelgl -> atom_win -> visible;
    opengl_project -> modelgl -> atom_win -> visible = FALSE;
    i = opengl_project -> modelgl -> selection_mode;
    opengl_project -> modelgl -> selection_mode = ATOMS;
#ifdef GTK4
    select_unselect_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][0]);
#else
    select_unselect_atoms (NULL, & opengl_project -> modelgl -> colorp[0][0]);
#endif
    opengl_project -> modelgl -> selection_mode = NSELECTION-1;
#ifdef GTK4
    select_unselect_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][0]);
#else
    select_unselect_atoms (NULL, & opengl_project -> modelgl -> colorp[0][0]);
#endif
    opengl_project -> modelgl -> selection_mode = i;
    opengl_project -> modelgl -> atom_win -> visible = vis_stat;
  }

  if (this_proj -> nspec)
  {
    g_free (this_proj -> atoms[0]);
  }
  else
  {
    this_proj -> atoms = g_malloc0 (sizeof*this_proj -> atoms);
  }

  int rem_spec;
  int * spid, * spdel;
  spid = allocint (this_proj -> nspec + edit -> add_spec);
  spdel = allocint (this_proj -> nspec + edit -> add_spec);
  int * atid = allocint (new_atoms);
  this_proj -> atoms[0] = g_malloc0 (new_atoms*sizeof*this_proj -> atoms[0]);
  tmp_new = new_list;
  i = 0;
  while (tmp_new)
  {
    if (asearch -> action != REMOVE || old_id[tmp_new -> id] > 0)
    {
      this_proj -> atoms[0][i] = * duplicate_atom (tmp_new);
      this_proj -> atoms[0][i].id = i;
      spid[this_proj -> atoms[0][i].sp] ++;
      atid[i] = this_proj -> atoms[0][i].sp;
      this_proj -> atoms[0][i].pick[0] = this_proj -> atoms[0][i].pick[1] = FALSE;
      i ++;
    }
    if (tmp_new -> next)
    {
      tmp_new = tmp_new -> next;
      g_free (tmp_new -> prev);
    }
    else
    {
      g_free (tmp_new);
      tmp_new = NULL;
    }
  }
  if (old_id) g_free (old_id);
  old_id = NULL;
  rem_spec = 0;
  for (i=0; i<this_proj -> nspec + edit -> add_spec; i++)
  {
    if (spid[i] == 0)
    {
      // A chem spec is being removed ... shit that's more work
      rem_spec ++;
    }
    else
    {
      spdel[i] = rem_spec;
    }
  }

  if (rem_spec || edit -> add_spec)
  {
    int new_spec = this_proj -> nspec - rem_spec + edit -> add_spec;
    chemical_data * newchem = alloc_chem_data (new_spec);
    i = 0;
    for (j=0; j<this_proj -> nspec; j++)
    {
      if (spid[j] != 0)
      {
        newchem -> nsps[i] = this_proj -> chemistry -> nsps[j];
        newchem -> formula[i] = this_proj -> chemistry -> formula[j];
        newchem -> label[i] = g_strdup_printf ("%s", this_proj -> chemistry -> label[j]);
        newchem -> element[i] = g_strdup_printf ("%s", this_proj -> chemistry -> element[j]);
        for (k=0; k<CHEM_PARAMS; k++)
        {
          newchem -> chem_prop[k][i] = this_proj -> chemistry -> chem_prop[k][j];
        }

        if (j < this_proj -> nspec-1)
        {
          k = i;
          for (l=j; l<this_proj -> nspec; l++)
          {
            if (spid[l] != 0)
            {
              newchem -> cutoffs[i][k] = newchem -> cutoffs[k][i] = this_proj -> chemistry -> cutoffs[j][l];
              k ++;
            }
          }
        }
        newchem -> nsps[i] = spid[j];
        i ++;
      }
    }
    j = (this_proj -> nspec) ? 1 : 0;
    for (k=0; k<edit -> add_spec - j*rem_spec; k++)
    {
      l = this_proj -> nspec + k;
      if ( spid[l])
      {
        newchem -> nsps[i] = spid[l];
        newchem -> formula[i] = 0;
        newchem -> chem_prop[CHEM_Z][i] = newchem -> chem_prop[CHEM_X][i] = edit -> new_z[l];
        m = (int)newchem -> chem_prop[CHEM_Z][i];
        newchem -> label[i] = g_strdup_printf ("%s", periodic_table_info[m].lab);
        newchem -> element[i] = g_strdup_printf ("%s", periodic_table_info[m].name);
        n = 0;
        newchem -> chem_prop[CHEM_M][i] = set_mass_ (& m);
        newchem -> chem_prop[CHEM_R][i] = set_radius_ (& m, & n);
        newchem -> chem_prop[CHEM_N][i] = set_neutron_ (& m);
        newchem -> chem_prop[CHEM_X][i] = newchem -> chem_prop[CHEM_Z][i];
        i ++;
      }
    }
    initcutoffs (newchem, new_spec);
    for (i=0; i<new_atoms; i++)
    {
      atid[i] -= spdel[atid[i]];
      this_proj -> atoms[0][i].sp = atid[i];
    }

    if (this_proj -> chemistry) g_free (this_proj -> chemistry);
    this_proj -> chemistry = NULL;
    this_proj -> chemistry = duplicate_chemical_data (new_spec, newchem);
    g_free (newchem);
  }
  else
  {
    g_free (this_proj -> chemistry -> nsps);
    this_proj -> chemistry -> nsps = duplicate_int (this_proj -> nspec, spid);
  }
  g_free (atid);

  i = activep;
  if (! this_proj -> natomes)
  {
    this_proj -> natomes = new_atoms;
    if (! this_proj -> cell.crystal) center_molecule (this_proj);
  }
  else
  {
    this_proj -> natomes = new_atoms;
  }

  // Active project changes in the next call
  recover_opengl_data (this_proj, nmols, edit -> add_spec, rem_spec, spid, spdel, tmpgeo, showfrag);

  if (showfrag)
  {
    g_free (showfrag);
    showfrag = NULL;
  }
#ifdef DEBUG
  if (this_proj -> natomes)
  {
    print_coord_info (this_proj, this_proj -> coord);
  }
  else
  {
    g_debug (" *** None *** ");
  }
#endif
  g_free (spdel);
  g_free (edit -> new_z);
  g_free (spid);
  for (j=0; j<2; j++)
  {
    g_free (tmpgeo[j]);
  }

  init_curves_and_calc (active_project);
  if (active_box)
  {
    if (test_vol(active_box -> param, active_box -> vect))
    {
      for (j=0; j<3; j=j+2) active_project -> runok[j] = TRUE;
    }
  }
  initcwidgets ();
  active_project -> dmtx = FALSE;
  active_project -> run = (active_project -> natomes) ? TRUE : FALSE;
  chemistry_ () ;

  if (asearch -> recompute_bonding)
  {
    active_project_changed (activep);
    bonds_update = 1;
    frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
    mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
    active_project -> runc[0] = FALSE;
    on_calc_bonds_released (NULL, NULL);
  }
  // Then back to the previous active project
  active_project_changed (i);

  int shaders[5] = {ATOMS, BONDS, POLYS, RINGS, SELEC};
  re_create_md_shaders (5, shaders, this_proj);
  this_proj -> modelgl -> create_shaders[PICKS] = TRUE;
  this_proj -> modelgl -> create_shaders[MDBOX] = TRUE;
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  this_proj -> modelgl -> create_shaders[MEASU] = TRUE;

  tmp_add = to_add;
  for (i=0; i<2; i++) save_all_selections (this_proj -> modelgl, i);
  while (tmp_add != NULL)
  {
    for (i=0; i<2; i++)
    {
      if (tmp_add -> pick[i])
      {
        process_selected_atom (this_proj, this_proj -> modelgl, tmp_add -> id, 0, 0, i);
      }
    }
    tmp_add = tmp_add -> next;
  }
  for (i=0; i<2; i++) update_all_selections (this_proj -> modelgl, i);
  if (to_add != NULL) free_dummies (to_add);

  if (visible && (asearch -> action != DISPL && asearch -> action != RANMOVE))
  {
    for (i=0; i<3; i++)
    {
      g_free (this_proj -> modelgl -> saved_coord[i]);
      this_proj -> modelgl -> saved_coord[i] = NULL;
      init_coordinates (this_proj, i, FALSE, TRUE);
    }
    this_proj -> modelgl -> was_moved = FALSE;
    this_proj -> modelgl -> atom_win -> rebuilt[0] = FALSE;
    this_proj -> modelgl -> atom_win -> rebuilt[1] = FALSE;
    if (this_proj -> modelgl -> atom_win -> msd) g_free (this_proj -> modelgl -> atom_win -> msd);
    this_proj -> modelgl -> atom_win -> msd = allocfloat (this_proj -> natomes);
    if (this_proj -> modelgl -> atom_win -> msd_all) g_free (this_proj -> modelgl -> atom_win -> msd_all);
    this_proj -> modelgl -> atom_win -> msd_all = allocfloat (this_proj -> nspec);
    for (i=0; i<2; i++) clean_motion_search (this_proj, this_proj -> modelgl -> search_widg[2+4*i], i);
    motion_to_zero (this_proj -> modelgl -> search_widg[2]);
  }
  else if (visible && (asearch -> action == DISPL || asearch -> action == RANMOVE))
  {
    this_proj -> modelgl -> was_moved = TRUE;
    clean_motion_search (this_proj, this_proj -> modelgl -> search_widg[(asearch -> action == DISPL) ? 6 : 2], (asearch -> action == DISPL) ? 1 : 0);
    if (asearch -> action == RANMOVE) motion_to_zero (this_proj -> modelgl -> search_widg[2]);
  }
  else if (this_proj -> modelgl -> anim -> last -> img -> selected[0] -> selected)
  {
    g_free (this_proj -> modelgl -> saved_coord[1]);
    this_proj -> modelgl -> saved_coord[1] = save_coordinates (this_proj, 1);
    this_proj -> modelgl -> baryc[1] = get_bary (this_proj, 1);
  }
  update (this_proj -> modelgl);
#ifdef GTK4
  update_menu_bar (this_proj -> modelgl);
#endif
  clean_coord_window (this_proj);

  switch (asearch -> action)
  {
    case REPLACE:
      return extra;
      break;
    case INSERT:
      return extra;
      break;
    case REMOVE:
      return remove;
      break;
    default:
      return 0;
      break;
  }
}

/*!
  \fn void clean_all_trees (atom_search * asearch, project * this_proj)

  \brief clean all tree models in the 'model edition' window

  \param asearch the target atom search
  \param this_proj the target project
*/
void clean_all_trees (atom_search * asearch, project * this_proj)
{
  int i, j;
  for (i=0; i<5; i++)
  {
    if (i == 3)
    {
      clean_picked_and_labelled (this_proj -> modelgl -> search_widg[i+2], TRUE);
      if (this_proj -> modelgl -> search_widg[INSERT] -> in_selection)
      {
        allocate_todo (this_proj -> modelgl -> search_widg[i+2], this_proj -> modelgl -> search_widg[INSERT] -> in_selection);
        atomic_object * object = this_proj -> modelgl -> atom_win -> to_be_inserted[1];
        j = 0;
        while (object)
        {
          object -> id = j;
          j ++;
          object = object -> next;
        }
        update_search_tree (this_proj -> modelgl -> search_widg[i+2]);
      }
    }
    else if (this_proj -> modelgl -> search_widg[i+2] -> atom_tree)
    {
      j = get_asearch_filter (this_proj -> modelgl -> search_widg[i+2]);
      if ((j == 3 && ! this_proj -> modelgl -> adv_bonding[0]) || (j == 4 && ! this_proj -> modelgl -> adv_bonding[1]))
      {
        combo_set_active (this_proj -> modelgl -> search_widg[i+2] -> filter_box, 0);
        set_filter_changed (GTK_COMBO_BOX(this_proj -> modelgl -> search_widg[i+2] -> filter_box), this_proj -> modelgl -> search_widg[i+2]);
      }
      else if (asearch -> action != DISPL && asearch -> action  != RANMOVE)
      {
        set_filter_changed (GTK_COMBO_BOX(this_proj -> modelgl -> search_widg[i+2] -> filter_box), this_proj -> modelgl -> search_widg[i+2]);
      }
      update_search_tree (this_proj -> modelgl -> search_widg[i+2]);
    }
    if ((i == 0 || i == 4) && this_proj -> modelgl -> search_widg[i+2] -> todo_size >= 10000)
    {
      re_populate_tree_search (this_proj -> modelgl -> search_widg[i+2]);
    }
  }
  if ((this_proj -> modelgl -> atom_win -> adv_bonding[0] && ! this_proj -> modelgl -> adv_bonding[0])
   || (this_proj -> modelgl -> atom_win -> adv_bonding[1] && ! this_proj -> modelgl -> adv_bonding[1]))
  {
    int j;
    for (i=0; i<5; i++)
    {
      if (i != 3)
      {
        if (this_proj -> modelgl -> search_widg[i+2] -> passivating)
        {
          j = (this_proj -> modelgl -> search_widg[i+2] -> object < 2) ? 3 : 2;
        }
        else
        {
          j = (! this_proj -> modelgl -> search_widg[i+2] -> object) ? 3 : 2;
        }
        if (GTK_IS_WIDGET(this_proj -> modelgl -> search_widg[i+2] -> filter_box))
        {
          if (this_proj -> modelgl -> atom_win -> adv_bonding[1] && ! this_proj -> modelgl -> adv_bonding[1])
          {
            gtk_combo_box_text_remove ((GtkComboBoxText *) this_proj -> modelgl -> search_widg[i+2] -> filter_box, j+1);
          }
          if (this_proj -> modelgl -> atom_win -> adv_bonding[0] && ! this_proj -> modelgl -> adv_bonding[0])
          {
            gtk_combo_box_text_remove ((GtkComboBoxText *) this_proj -> modelgl -> search_widg[i+2] -> filter_box, j);
          }
        }
      }
    }
  }
  for (i=0 ;i<2; i++) this_proj -> modelgl -> atom_win -> adv_bonding[i] = this_proj -> modelgl -> adv_bonding[i];
}

/*!
  \fn void apply_action (project * this_proj, atom_search * asearch)

  \brief apply edition action

  \param this_proj the target project
  \param asearch the target atom search
*/
void apply_action (project * this_proj, atom_search * asearch)
{
  gchar * str;
  gchar * appl[3] = {"replaced", "removed", "inserted"};
  int k, l;
  l = 0;
  gboolean visible = (this_proj -> modelgl -> atom_win) ? this_proj -> modelgl -> atom_win -> visible : FALSE;
  if (asearch -> action == REPLACE || asearch -> action == REMOVE)
  {
    to_remove_this_list_of_objects (this_proj, asearch);
    l = action_atoms_from_project (this_proj, remove_search, visible);
    g_free (remove_search);
    remove_search = NULL;
  }
  k = (asearch -> action == REMOVE) ? l : action_atoms_from_project (this_proj, asearch, visible);
  if (asearch -> action != DISPL && asearch -> action != RANMOVE)
  {
    switch (k)
    {
      case 0:
        str = g_strdup_printf ("No atoms to be %s !", appl[asearch -> action-3]);
        break;
      default:
        if (asearch -> action == REPLACE && l)
        {
          if (asearch -> pointer[0].c == 8) l += asearch -> int_b - k;
          str = g_strdup_printf ("%d atom(s) removed !\n%d atom(s) inserted !", l, k);
        }
        else
        {
          str = g_strdup_printf ("%d atom(s) %s !", k, appl[asearch -> action-3]);
        }
      break;
    }
    if (this_proj -> modelgl -> atom_win -> win)
    {
      show_info (str, 0, this_proj -> modelgl -> atom_win -> win);
    }
    else
    {
      show_info (str, 0, this_proj -> modelgl -> win);
    }
    g_free (str);
  }
  if (visible)
  {
    if (k || l)
    {
      if (this_proj -> modelgl -> atom_win)
      {
        if (this_proj -> modelgl -> atom_win -> visible) clean_all_trees (asearch, this_proj);
        if (this_proj -> modelgl -> atom_win -> msd)
        {
          g_free (this_proj -> modelgl -> atom_win -> msd);
          this_proj -> modelgl -> atom_win -> msd = NULL;
        }
        this_proj -> modelgl -> atom_win -> msd = allocfloat (this_proj -> natomes);
      }
    }
  }
  clean_other_window_after_edit (this_proj);
}

/*!
  \fn void prepare_random_action (project * this_proj, atom_search * asearch)

  \brief prepare random action

  \param this_proj the target project
  \param asearch the target atom search
*/
void prepare_random_action (project * this_proj, atom_search * asearch)
{
  int i, j, k, l, m, n, o, p, q;
  double test, prob;
  atomic_object * object = NULL;
  atomic_object * tmp_oba, * tmp_obb;
  molecule * molfc;
  int max_num, total_num;
  gboolean lets_do_this = (asearch -> action == REMOVE || (asearch -> action == REPLACE && this_proj -> modelgl -> atom_win -> to_be_inserted[0])) ? TRUE : FALSE;
  if (lets_do_this)
  {
    int filter = get_asearch_filter (asearch);
    int obj = get_asearch_object (asearch);
    int num_elem = asearch -> todo_size;
    int * random_todo = duplicate_int (num_elem, asearch -> todo);
    total_num = (asearch -> mode && filter > 2 && obj) ? this_proj -> coord -> totcoord[2] : this_proj -> natomes;
    g_free (asearch -> todo);
    allocate_todo (asearch, total_num);
    if (asearch -> action == REPLACE && this_proj -> modelgl -> atom_win -> to_be_inserted[0])
    {
      tmp_oba = this_proj -> modelgl -> atom_win -> to_be_inserted[0];
      object = duplicate_atomic_object (tmp_oba);
      tmp_obb = object;
      while (tmp_oba -> next)
      {
        tmp_oba = tmp_oba -> next;
        tmp_obb -> next = duplicate_atomic_object (tmp_oba);
        tmp_obb = tmp_obb -> next;
      }
      g_free (this_proj -> modelgl -> atom_win -> to_be_inserted[0]);
      this_proj -> modelgl -> atom_win -> to_be_inserted[0] = NULL;
    }
    gboolean doit;
    asearch -> in_selection = 0;
    tmp_oba = object;
    for (i=0; i<num_elem; i++)
    {
      if (random_todo[i])
      {
        max_num = 0;
        switch (filter)
        {
          case 0:
            max_num = this_proj -> chemistry -> nsps[i];
            break;
          case 1:
            for (j=0; j<this_proj -> natomes; j++)
            {
              if (this_proj -> atoms[0][j].numv == i) max_num ++;
            }
            break;
          case 2:
            j = 0;
            for (k=0; k<this_proj -> nspec; k++)
            {
              j += this_proj -> coord -> ntg[1][k];
              if (j > i) break;
            }
            l = 0;
            for (m=0; m<k; m++) l += this_proj -> coord -> ntg[1][m];
            p = i - l;
            for (l=0; l<this_proj -> natomes; l++)
            {
              if (this_proj -> atoms[0][l].sp == k && this_proj -> atoms[0][l].coord[1] == p) max_num ++;
            }
            break;
          case 3:
            if (obj)
            {
              max_num = this_proj -> coord -> totcoord[2];
            }
            else
            {
              for (l=0; l<this_proj -> natomes; l++)
              {
                if (this_proj -> atoms[0][l].coord[2] == i) max_num ++;
              }
            }
            break;
          case 4:
            molfc = & this_proj -> modelfc -> mols[0][i];
            if (obj)
            {
              max_num = molfc -> multiplicity;
            }
            else
            {
              max_num = molfc -> natoms*molfc -> multiplicity;
            }
            break;
        }
        test = 1.0 / max_num;
        j = 0;
        // Using CPU time to randomize
        clock_t begin = clock();
        k = (int)begin;
        while (j < random_todo[i])
        {
          l = 0;
          for (m=0; m<total_num; m++)
          {
            if (! asearch -> todo[m])
            {
              doit = FALSE;
              if (filter < 3 || ! obj)
              {
                n = this_proj -> atoms[0][m].sp;
                if (this_proj -> atoms[0][m].pick[0] == asearch -> status || asearch -> status == 2)
                {
                  switch (filter)
                  {
                    case 0:
                      if (n == i) doit = TRUE;
                      break;
                    case 1:
                      if (this_proj -> atoms[0][m].numv == i) doit = TRUE;
                      break;
                    case 2:
                      o = this_proj -> atoms[0][m].coord[1];
                      for (q=0;q<n;q++) o += this_proj -> coord -> ntg[1][q];
                      if (o == i) doit = TRUE;
                      break;
                    default:
                      if (this_proj -> atoms[0][m].coord[filter-1] == i) doit = TRUE;
                      break;
                  }
                }
              }
              else if (filter == 3)
              {
                doit = TRUE;
              }
              else
              {
                for (n=0; n<molfc -> multiplicity; n++)
                {
                  if (molfc -> fragments[n] == m)
                  {
                    doit = TRUE;
                    break;
                  }
                }
              }
              if (doit)
              {
                l ++;
                o = (o+1)*max_num*m;
                prob = random3_(& n);
                if (prob >= (l-1)*test && prob < l*test)
                {
                  j ++;
                  asearch -> todo[m] = 1;
                  if (asearch -> action == REPLACE)
                  {
                    if (this_proj -> modelgl -> atom_win -> to_be_inserted[0] == NULL)
                    {
                      this_proj -> modelgl -> atom_win -> to_be_inserted[0] = duplicate_atomic_object (tmp_oba);
                      tmp_obb = this_proj -> modelgl -> atom_win -> to_be_inserted[0];
                    }
                    else
                    {
                      tmp_obb -> next = duplicate_atomic_object (tmp_oba);
                      tmp_obb -> next -> prev = tmp_obb;
                      tmp_obb = tmp_obb -> next;
                    }
                    tmp_obb -> id = m;
                  }
                  asearch -> in_selection ++;
                  if (j == random_todo[i]) break;
                }
              }
            }
          }
        }
        if (asearch -> action == REPLACE)
        {
          if (tmp_oba -> next != NULL) tmp_oba = tmp_oba -> next;
        }
      }
    }
    if (asearch -> action == REPLACE) g_free (object);
    if (random_todo) g_free (random_todo);
    int old_filter = asearch -> filter;
    // if (asearch -> mode && obj && asearch -> filter == 3) asearch -> filter = 2;
    apply_action (this_proj, asearch);
    asearch -> filter = old_filter;
  }
}

/*!
  \fn gboolean do_we_have_objects_in_selection (project * this_proj, atom_search * asearch, gboolean editing)

  \brief check for object(s) in selection to apply action

  \param this_proj the target project
  \param asearch the target atom search
  \param editing actually going for action (1) or simply counting elements (0)
*/
gboolean do_we_have_objects_in_selection (project * this_proj, atom_search * asearch, gboolean editing)
{
  int i, j, k, l, m, n, o, p, q;
  gboolean taking_action = FALSE;
  if (asearch -> action == INSERT)
  {
    taking_action = TRUE;
  }
  else
  {
    i = j = 0;
    if (asearch -> mode)
    {
      for (l=0; l<asearch -> todo_size; l++)
      {
        if (asearch -> todo[l]) i++;
        if (asearch -> action == REPLACE)
        {
          if (get_atomic_object_by_origin (this_proj -> modelgl -> atom_win -> to_be_inserted[0], -(l+3), 0))
          {
            if (asearch -> todo[l]) j ++;
          }
        }
        else
        {
          if (asearch -> pick[l] && asearch -> todo[l]) j ++;
        }
      }
      if (asearch -> action == REMOVE && i == j)
      {
        taking_action = TRUE;
      }
      else if (asearch -> action == REPLACE && i == j && i == asearch -> in_selection)
      {
        taking_action = TRUE;
      }
    }
    else
    {
      m = (asearch -> pointer[0].c == 3) ? 0 : (asearch -> pointer[0].c == 5) ? 1 : 3;
      int filter = get_asearch_filter (asearch);
      int object = get_asearch_object (asearch);
      int pass_size;
      int * pass_todo = NULL;
      atomic_object * passivating_object = NULL;
      atomic_object * pao = NULL;
      atomic_object * pio = NULL;
      float * tmp_msd = NULL;
      if (asearch -> passivating)
      {
        if (object > 1 && filter > 2)
        {
          pass_size = this_proj -> coord -> totcoord[filter - 1];
        }
        else
        {
          pass_size = this_proj -> natomes;
        }
        pass_todo = allocint (pass_size);
        if (asearch -> action == RANMOVE) tmp_msd = allocfloat (this_proj -> natomes);
      }
      switch (asearch -> action)
      {
        case DISPL:
          for (l=0; l<asearch -> todo_size; l++)
          {
            if (asearch -> todo[l]) i ++;
            if (editing)
            {
              if (asearch -> todo[l])
              {
                if (asearch -> passivating)
                {
                  if (object < 2)
                  {
                    switch (filter)
                    {
                      case 0:
                        for (n=0; n<this_proj -> natomes; n++)
                        {
                          if (this_proj -> atoms[0][n].sp == l)
                          {
                            pass_todo[n] = asearch -> todo[l];
                            k ++;
                          }
                        }
                        break;
                      case 1:
                        for (n=0; n<this_proj -> natomes; n++)
                        {
                          if (this_proj -> atoms[0][n].numv == l)
                          {
                            pass_todo[n] = asearch -> todo[l];
                            k ++;
                          }
                        }
                        break;
                      case 2:
                        for (n=0; n<this_proj -> natomes; n++)
                        {
                          o = this_proj -> atoms[0][n].coord[filter - 1];
                          p = this_proj -> atoms[0][n].sp;
                          for (q=0; q<p; q++) o += this_proj -> coord -> ntg[1][q];
                          if (o == l)
                          {
                            pass_todo[n] = asearch -> todo[l];
                            k ++;
                          }
                        }
                        break;
                      default:
                        for (n=0; n<this_proj -> natomes; n++)
                        {
                          o = this_proj -> atoms[0][n].coord[filter - 1];
                          if (o == l)
                          {
                            pass_todo[n] = asearch -> todo[l];
                            k ++;
                          }
                        }
                        break;
                    }
                  }
                  else
                  {
                    switch (filter)
                    {
                      case 1:
                        for (n=0; n<this_proj -> natomes; n++)
                        {
                          if (this_proj -> atoms[0][n].numv == l)
                          {
                            pass_todo[n] = asearch -> todo[l];
                            k ++;
                            if (! passivating_object)
                            {
                              passivating_object = create_object_from_atom_coordination (this_proj, filter-1, n, NULL);
                              pao = passivating_object;
                            }
                            else
                            {
                              pao -> next = create_object_from_atom_coordination (this_proj, filter-1, n, NULL);
                              pao -> next -> prev = pao;
                              pao = pao -> next;
                            }
                          }
                        }
                        break;
                      case 2:
                        for (n=0; n<this_proj -> natomes; n++)
                        {
                          o = this_proj -> atoms[0][n].coord[filter - 1];
                          p = this_proj -> atoms[0][n].sp;
                          for (q=0; q<p; q++) o += this_proj -> coord -> ntg[1][q];
                          if (o == l)
                          {
                            pass_todo[n] = asearch -> todo[l];
                            k ++;
                            if (! passivating_object)
                            {
                              passivating_object = create_object_from_atom_coordination (this_proj, filter-1, n, NULL);
                              pao = passivating_object;
                            }
                            else
                            {
                              pao -> next = create_object_from_atom_coordination (this_proj, filter-1, n, NULL);
                              pao -> next -> prev = pao;
                              pao = pao -> next;
                            }
                          }
                        }
                        break;
                      default:
                        for (n=0; n<this_proj -> coord -> totcoord[filter -1]; n++)
                        {
                          if (n == l)
                          {
                            pass_todo[n] = asearch -> todo[l];
                            k ++;
                            if (! passivating_object)
                            {
                              passivating_object = create_object_from_frag_mol (this_proj, filter-1, n, NULL);
                              pao = passivating_object;
                            }
                            else
                            {
                              pao -> next = create_object_from_frag_mol (this_proj, filter-1, n, NULL);
                              pao -> next -> prev = pao;
                              pao = pao -> next;
                            }
                          }
                        }
                        break;
                    }
                  }
                }
              }
            }
          }
          break;
        case REPLACE:
          for (l=0; l<asearch -> todo_size; l++)
          {
            if (asearch -> todo[l]) i ++;
            pio = get_atomic_object_by_origin (this_proj -> modelgl -> atom_win -> to_be_inserted[m], l, 0);
            if (pio)
            {
              if (asearch -> todo[l])
              {
                j ++;
                if (asearch -> passivating)
                {
                  switch (filter)
                  {
                    case 0:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        if (this_proj -> atoms[0][n].sp == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          if (! passivating_object)
                          {
                            passivating_object = duplicate_atomic_object (pio);
                            pao = passivating_object;
                            pao -> id = n;
                          }
                          else
                          {
                            pao -> next = duplicate_atomic_object (pio);
                            pao -> id = n;
                            pao -> next -> prev = pao;
                            pao = pao -> next;
                          }
                          k ++;
                        }
                      }
                      break;
                    case 1:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        if (this_proj -> atoms[0][n].numv == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          if (! passivating_object)
                          {
                            passivating_object = duplicate_atomic_object (pio);
                            pao = passivating_object;
                            pao -> id = n;
                          }
                          else
                          {
                            pao -> next = duplicate_atomic_object (pio);
                            pao -> id = n;
                            pao -> next -> prev = pao;
                            pao = pao -> next;
                          }
                          k ++;
                        }
                      }
                      break;
                    case 2:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        o = this_proj -> atoms[0][n].coord[filter - 1];
                        p = this_proj -> atoms[0][n].sp;
                        for (q=0; q<p; q++) o += this_proj -> coord -> ntg[1][q];
                        if (o == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          if (! passivating_object)
                          {
                            passivating_object = duplicate_atomic_object (pio);
                            pao = passivating_object;
                            pao -> id = n;
                          }
                          else
                          {
                            pao -> next = duplicate_atomic_object (pio);
                            pao -> id = n;
                            pao -> next -> prev = pao;
                            pao = pao -> next;
                          }
                          k ++;
                        }
                      }
                    default:
                      if (object < 2)
                      {
                        for (n=0; n<this_proj -> natomes; n++)
                        {
                          o = this_proj -> atoms[0][n].coord[filter - 1];
                          if (o == l)
                          {
                            pass_todo[n] =  asearch -> todo[l];
                            if (! passivating_object)
                            {
                              passivating_object = duplicate_atomic_object (pio);
                              pao = passivating_object;
                              pao -> id = n;
                            }
                            else
                            {
                              pao -> next = duplicate_atomic_object (pio);
                              pao -> id = n;
                              pao -> next -> prev = pao;
                              pao = pao -> next;
                            }
                            k ++;
                          }
                        }
                      }
                      else
                      {
                        pass_todo[l] = asearch -> todo[l];
                        if (! passivating_object)
                        {
                          passivating_object = duplicate_atomic_object (pio);
                          pao = passivating_object;
                          pao -> id = l;
                        }
                        else
                        {
                          pao -> next = duplicate_atomic_object (pio);
                          pao -> id = l;
                          pao -> next -> prev = pao;
                          pao = pao -> next;
                        }
                        k ++;
                      }
                      break;
                  }
                }
              }
            }
          }
          break;
        case REMOVE:
          for (l=0; l<asearch -> todo_size; l++)
          {
            if (asearch -> todo[l])
            {
              i ++;
              if (asearch -> passivating)
              {
                switch (filter)
                {
                  case 0:
                    for (n=0; n<this_proj -> natomes; n++)
                    {
                      if (this_proj -> atoms[0][n].sp == l)
                      {
                        pass_todo[n] = 1;
                        k ++;
                      }
                    }
                    break;
                  case 1:
                    for (n=0; n<this_proj -> natomes; n++)
                    {
                      if (this_proj -> atoms[0][n].numv == l)
                      {
                        pass_todo[n] = 1;
                        k ++;
                      }
                    }
                    break;
                  case 2:
                    for (n=0; n<this_proj -> natomes; n++)
                    {
                      o = this_proj -> atoms[0][n].coord[filter - 1];
                      p = this_proj -> atoms[0][n].sp;
                      for (q=0; q<p; q++) o += this_proj -> coord -> ntg[1][q];
                      if (o == l)
                      {
                        pass_todo[n] = 1;
                        k ++;
                      }
                    }
                  default:
                    if (object < 2)
                    {
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        o = this_proj -> atoms[0][n].coord[filter - 1];
                        if (o == l)
                        {
                          pass_todo[n] = 1;
                          k ++;
                        }
                      }
                    }
                    else
                    {
                      pass_todo[l] = 1;
                      k ++;
                    }
                    break;
                }
             }
            }
          }
          break;
        case RANMOVE:
          for (l=0; l<asearch -> todo_size; l++)
          {
            if (asearch -> passivating)
            {
              if (asearch -> todo[l] && this_proj -> modelgl -> atom_win -> msd_all[l] > 0.0)
              {
                i ++;
                if (object < 2)
                {
                  switch (filter)
                  {
                    case 0:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        if (this_proj -> atoms[0][n].sp == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          tmp_msd[n] = this_proj -> modelgl -> atom_win -> msd_all[l];
                          k ++;
                        }
                      }
                      break;
                    case 1:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        if (this_proj -> atoms[0][n].numv == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          tmp_msd[n] = this_proj -> modelgl -> atom_win -> msd_all[l];
                          k ++;
                        }
                      }
                      break;
                    case 2:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        o = this_proj -> atoms[0][n].coord[filter - 1];
                        p = this_proj -> atoms[0][n].sp;
                        for (q=0; q<p; q++) o += this_proj -> coord -> ntg[1][q];
                        if (o == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          tmp_msd[n] = this_proj -> modelgl -> atom_win -> msd_all[l];
                          k ++;
                        }
                      }
                      break;
                    default:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        o = this_proj -> atoms[0][n].coord[filter - 1];
                        if (o == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          tmp_msd[n] = this_proj -> modelgl -> atom_win -> msd_all[l];
                          k ++;
                        }
                      }
                      break;
                  }
                }
                else
                {
                  switch (filter)
                  {
                    case 1:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        if (this_proj -> atoms[0][n].numv == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          tmp_msd[n] = this_proj -> modelgl -> atom_win -> msd_all[l];
                          if (! passivating_object)
                          {
                            passivating_object = create_object_from_atom_coordination (this_proj, filter-1, n, NULL);
                            pao = passivating_object;
                          }
                          else
                          {
                            pao -> next = create_object_from_atom_coordination (this_proj, filter-1, n, NULL);
                            pao -> next -> prev = pao;
                            pao = pao -> next;
                          }
                          k ++;
                        }
                      }
                      break;
                    case 2:
                      for (n=0; n<this_proj -> natomes; n++)
                      {
                        o = this_proj -> atoms[0][n].coord[filter - 1];
                        p = this_proj -> atoms[0][n].sp;
                        for (q=0; q<p; q++) o += this_proj -> coord -> ntg[1][q];
                        if (o == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          tmp_msd[n] = this_proj -> modelgl -> atom_win -> msd_all[l];
                          if (! passivating_object)
                          {
                            passivating_object = create_object_from_atom_coordination (this_proj, filter-1, n, NULL);
                            pao = passivating_object;
                          }
                          else
                          {
                            pao -> next = create_object_from_atom_coordination (this_proj, filter-1, n, NULL);
                            pao -> next -> prev = pao;
                            pao = pao -> next;
                          }
                          k ++;
                        }
                      }
                      break;
                    default:
                      for (n=0; n<this_proj -> coord -> totcoord[filter -1]; n++)
                      {
                        if (n == l)
                        {
                          pass_todo[n] = asearch -> todo[l];
                          k ++;
                          tmp_msd[n] = this_proj -> modelgl -> atom_win -> msd_all[l];
                          if (! passivating_object)
                          {
                            passivating_object = create_object_from_frag_mol (this_proj, filter-1, n, NULL);
                            pao = passivating_object;
                          }
                          else
                          {
                            pao -> next = create_object_from_frag_mol (this_proj, filter-1, n, NULL);
                            pao -> next -> prev = pao;
                            pao = pao -> next;
                          }
                        }
                      }
                      break;
                  }
                }
              }
            }
            else if (object && filter > 2)
            {
              if (asearch -> todo[l] && this_proj -> modelgl -> atom_win -> msd_all[l] > 0.0) i ++;
            }
            else
            {
              if (asearch -> todo[l] && this_proj -> modelgl -> atom_win -> msd[l] > 0.0) i ++;
            }
          }
          break;
      }
      if (asearch -> action == REMOVE && i)
      {
        taking_action = TRUE;
      }
      else if (asearch -> action == REPLACE)
      {
        if (asearch -> in_selection == i && i == j) taking_action = TRUE;
      }
      else
      {
        asearch -> in_selection = i;
        if (i) taking_action = TRUE;
      }
      if (asearch -> passivating && taking_action && editing)
      {
        g_free (asearch -> todo);
        asearch -> todo = duplicate_int (pass_size, pass_todo);
        g_free (pass_todo);
        asearch -> todo_size = pass_size;
        asearch -> in_selection = k;
        switch (asearch -> action)
        {
          case DISPL:
            this_proj -> modelgl -> atom_win -> to_be_moved[0] = passivating_object;
            break;
          case REPLACE:
            this_proj -> modelgl -> atom_win -> to_be_inserted[m] = passivating_object;
            break;
          case RANMOVE:
            if (this_proj -> modelgl -> atom_win -> msd) g_free (this_proj -> modelgl -> atom_win -> msd);
            this_proj -> modelgl -> atom_win -> msd = duplicate_float (this_proj -> natomes, tmp_msd);
            g_free (tmp_msd);
            this_proj -> modelgl -> atom_win -> to_be_moved[1] = passivating_object;
            break;

        }
      }
    }
  }
  return taking_action;
}

/*!
  \fn atom_search * duplicate_atom_search (atom_search * asearch)

  \brief duplicate atom search data structure

  \param asearch
*/
atom_search * duplicate_atom_search (atom_search * asearch)
{
  atom_search * bsearch = g_malloc0 (sizeof*bsearch);
  bsearch -> search_digit = asearch -> search_digit;
  bsearch -> proj = asearch -> proj;
  bsearch -> action = asearch -> action;
  bsearch -> status = asearch -> status;
  bsearch -> mode = asearch -> mode;
  bsearch -> object = asearch -> object;
  bsearch -> filter = asearch -> filter;
  bsearch -> search_digit = asearch -> search_digit;
  bsearch -> spec_to_add = asearch -> spec_to_add;
  bsearch -> num_to_add = asearch -> num_to_add;
  bsearch -> in_selection = asearch -> in_selection;
  bsearch -> passivating = asearch -> passivating;
  bsearch -> was_selected = asearch -> was_selected;
  bsearch -> recompute_bonding = asearch -> recompute_bonding;
  bsearch -> set_for_all = asearch -> set_for_all;
  bsearch -> int_b = asearch -> int_b;
  int i =  get_asearch_num_objects (asearch);
  bsearch -> lab = duplicate_int (i, asearch -> lab);
  bsearch -> pick = duplicate_int (i, asearch -> lab);
  bsearch -> todo_size = asearch -> todo_size;
  bsearch -> todo = duplicate_int (bsearch -> todo_size, asearch -> todo);
  for (i=0; i<6; i++)
  {
    bsearch -> pointer[i].a = asearch -> pointer[i].a;
    bsearch -> pointer[i].b = asearch -> pointer[i].b;
    bsearch -> pointer[i].c = asearch -> pointer[i].c;
  }
  return bsearch;
}

/*!
  \fn G_MODULE_EXPORT void take_action (GtkButton * but, gpointer data)

  \brief take edition action

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void take_action (GtkButton * but, gpointer data)
{
  tint * id = (tint *)data;
  project * this_proj = get_project_by_id (id -> a);
  int i;
  i = id -> c;
  atom_search * this_search = duplicate_atom_search (this_proj -> modelgl -> search_widg[i]);
  if (do_we_have_objects_in_selection(this_proj, this_search, TRUE))
  {
    if (i != RANMOVE)
    {
      if (this_search -> mode)
      {
        prepare_random_action (this_proj, this_search);
      }
      else
      {
        apply_action (this_proj, this_search);
      }
    }
    else
    {
      random_move (this_proj, this_search);
      clean_all_trees (this_search, this_proj);
    }
    if (this_proj -> modelgl -> atom_win -> visible)
    {
      for (i=0; i<5; i++) widget_set_sensitive (gtk_notebook_get_nth_page(GTK_NOTEBOOK (this_proj -> modelgl -> atom_win -> notebook), i), this_proj -> natomes);
      widget_set_sensitive (gtk_notebook_get_nth_page(GTK_NOTEBOOK (this_proj -> modelgl -> atom_win -> notebook), 3), 1);
    }
  }
  else
  {
    if (i < RANMOVE)
    {
      show_info ("Nothing to be done, check selection !", 0, this_proj -> modelgl -> atom_win -> win);
    }
    else
    {
      show_info ("Nothing to be done, check selection and/or MSD !", 0, this_proj -> modelgl -> atom_win -> win);
    }
  }
}
