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
* @file dlp_ff_match.c
* @short Functions the list the available database parameters for the atom database window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_ff_match.c'
*
* Contains:
*

 - The functions the list the available database parameters for the atom database window

*
* List of functions:

  int this_body_has_atom (field_nth_body * body, char * name);

  gboolean is_this_object_a_match (int fsid, int nat, int * ffc, int * fpar);

  static gboolean update_rend (GtkTreeModel * model, GtkTreeIter * iter, gpointer  data);

  G_MODULE_EXPORT gboolean on_ff_button_event (GtkWidget * widget, GdkEvent * event, gpointer data);

  gchar * get_this_prop_param (int sid, int key, int calc, int newp, float * val);
  gchar * get_this_prop_string (int sid, int oid, int type, int calc);

  void update_result_list (int sid, field_object_match * new_match);
  void fill_update_model (GtkTreeStore * store);
  void get_update_tree_data (GtkWidget * tree, gpointer data, GtkTreePath * path);
  void ff_button_event (double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data);
  void ff_button_event (GdkEvent * event, double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data);
  void win_update_tree (GtkWidget * vbx);
  void look_up_this_field_object (int fsid, int fpid, int ssid, int nat, int * fsp, int * fat);
  void check_this_fprop (int fsid, int fpid, int ssid, int * fat, int * fsp);
  void check_atom_for_updates ();

  G_MODULE_EXPORT void changed_update_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data);
  G_MODULE_EXPORT void on_ff_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
  G_MODULE_EXPORT void on_ff_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
  G_MODULE_EXPORT void on_toggle_update (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void run_check_atom_for_updates (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void changed_field_prop_combo (GtkComboBox * box, gpointer data);

  GtkWidget * create_update_tree ();
  GtkWidget * create_field_prop_combo (int f, int is_moy);

  GtkTreeModel * global_render_tree ();

  field_data * get_ff_data (int i, int j);
  field_object_match * duplicate_match (field_object_match * old_m);

  dint get_visible (gboolean result, gchar * the_name);

*/

#include "global.h"
#include "interface.h"
#include "glwindow.h"
#include "glview.h"
#include "dlp_field.h"

extern int * atoms_id;
extern int ** atoms_id_list;
extern int * field_objects;
extern char *** ff_atoms;
extern char * fvars_bond[2][FBONDS][FBONDS_P];
extern char * fvars_angle[2][FANGLES][FANGLES_P];
extern char * fvars_dihedral[2][FDIHEDRAL][FDIHEDRAL_P];
extern char * fvars_inversion[2][FINVERS][FINVERS_P];
extern char * fvars_vdw[2][FVDW][FVDW_P];
extern gchar * felemts[MAXDATA+1];
extern GtkWidget * afftype;
extern GtkWidget * field_key_combo;
extern GtkWidget * p_box;
extern GtkWidget * param_box;
field_object_match * up_match[9];
field_object_match * tmp_match, * other_match;
field_data * ff_data;

extern GtkWidget * param_prop_param_box (int pid);
extern gchar * get_this_vdw_string ();
extern G_MODULE_EXPORT void markup_action_renderer (GtkCellRendererCombo * cell, GtkCellEditable * editable, gchar * path_string, gpointer data);
extern int get_num_vdw_max ();
extern gchar * get_body_element_name (field_nth_body * body, int aid, int nbd);

#define NUCOL 8

GtkTreeViewColumn * update_col[NUCOL];
GtkCellRenderer * update_renderer[NUCOL];
GtkWidget * update_tree;
GtkWidget * ff_p_combo[2];
GtkTreeStore * update_model;
dint up;
GtkTreeModel * prop_to_up;
field_object_match * tmp_res[9];
dint pup[2];
gboolean vdw_same_atom;

/*!
  \fn gboolean is_this_object_a_match (int fsid, int nat, int * ffc, int * fpar)

  \brief check if object matches parameters from the database

  \param fsid the type of field property
  \param nat the number of atoms for this field property
  \param ffc object data
  \param fpar field database data
*/
gboolean is_this_object_a_match (int fsid, int nat, int * ffc, int * fpar)
{
  int i, j;
  int has_x = 0;
  gboolean a_match, b_match;
  for (i=0; i<nat; i++)
  {
    if (fpar[i] == -1 || ffc[i] == -1) has_x ++;
  }
  if (has_x == nat) return FALSE;
  switch (nat)
  {
    case 2:
      for (i=0; i<2; i++)
      {
        j = (i == 0) ? 1 : 0;
        if (ffc[0] == fpar[i] && ffc[1] == fpar[j]) return TRUE;
        if (ffc[0] == fpar[i] && has_x) return TRUE;
        if (ffc[1] == fpar[i] && has_x) return TRUE;
      }
      return FALSE;
      break;
    case 3:
      if (ffc[1] != fpar[1] && fpar[1] != -1 && ffc[1] != -1) return FALSE;
      for (i=0; i<3; i=i+2)
      {
        j = (i==0) ? 2 : 0;
        if (ffc[0] == fpar[i] && ffc[2] == fpar[j]) return TRUE;
        if (ffc[i] == fpar[i] && (ffc[j] == -1 || fpar[j] == -1)) return TRUE;
        if (ffc[j] == fpar[i] && (ffc[i] == -1 || fpar[j] == -1)) return TRUE;
      }
      return FALSE;
      break;
    case 4:
      a_match = b_match = TRUE;
      for (i=0; i<4; i++)
      {
        if (ffc[i] != fpar[i] && ffc[i] != -1 && fpar[i] != -1)
        {
          a_match = FALSE;
          break;
        }
      }
      if (fsid < 6)
      {
        for (i=0; i<4; i++)
        {
          j = 3-i;
          if (ffc[i] != fpar[j] && ffc[i] != -1 && fpar[j] != -1)
          {
            b_match = FALSE;
            break;
          }
        }
      }
      else if (fsid == 6)
      {
        if (ffc[0] != fpar[0] && ffc[0] != -1 && fpar[0] != -1)
        {
          b_match = FALSE;
        }
        else if (ffc[3] != fpar[3] && ffc[3] != -1 && fpar[3] != -1)
        {
          b_match = FALSE;
        }
        else
        {
          if ((ffc[1] == fpar[1] || ffc[1] == -1 || fpar[1] == -1) && (ffc[2] == fpar[2] || ffc[2] == -1 || fpar[2] == -1))
          {
            return TRUE;
          }
          else if ((ffc[1] == fpar[2] || ffc[1] == -1 || fpar[2] == -1) && (ffc[2] == fpar[1] || ffc[2] == -1 || fpar[1] == -1))
          {
            return TRUE;
          }
        }
      }
      if (! a_match && ! b_match) return FALSE;
      return TRUE;
      break;
  }
  return TRUE;
}

/*!
  \fn field_data * get_ff_data (int i, int j)

  \brief retrieve field property from database

  \param i the type of field property
  \param j the id of the force field database, if any
*/
field_data * get_ff_data (int i, int j)
{
  switch (i/2)
  {
    case 0:
      return ff_bonds[j];
    break;
    case 1:
      return ff_angles[j];
      break;
    case 2:
      return ff_dih[j];
      break;
    default:
      if (i == 6)
      {
        return ff_imp;
      }
      else if (i == 7)
      {
        return ff_inv;
      }
      else
      {
        return ff_vdw;
      }
      break;
  }
}

/*!
  \fn gchar * get_this_prop_param (int sid, int key, int calc, int newp, float * val)

  \brief prepare field property parameters description string

  \param sid the type of field property
  \param key the key, or formalism to use, for the field property
  \param calc from the tree model (1) or from the selection combo box (0)
  \param newp 1 = from 'get_this_prop_string', 0 = else
  \param val the list of parameter(s)
*/
gchar * get_this_prop_param (int sid, int key, int calc, int newp, float * val)
{
  char ** vars;
  char * vbdq[4] = {"k/2", "r<sub>0</sub>", "k'/3", "k''/4"};
  char * vanq[4] = {"k/2", "θ<sub>0</sub>", "k'/3", "k''/4"};
  char * vdwij[2] = {"Ɛ<sub>i</sub>", "r0<sub>i</sub>/2.0"};
  switch (sid/2)
  {
    case 0:
      if (newp && (key == 0 || key == 5))
      {
        vars = (char **)vbdq;
      }
      else
      {
        vars = (char **)fvars_bond[key];
      }
      break;
    case 1:
      if (newp && key < 2)
      {
        vars = (char **)vanq;
      }
      else
      {
        vars = (char **)fvars_angle[key];
      }
      break;
    default:
      if (sid > 7)
      {
        if (newp && (tmp_field -> type <= CHARMMSI || tmp_field -> type > COMPASS) && ! calc)
        {
          vars = (char **)vdwij;
        }
        else
        {
          vars = (char **)fvars_vdw[key];
        }
      }
      else if (sid < 7)
      {
        vars = (char **)fvars_dihedral[key];
      }
      else
      {
         vars = (char **)fvars_inversion[key];
      }
      break;
  }

  return g_strdup_printf ("%s pot. (%s)", fnames[activef][(sid > 7) ? 10 : sid+2][key],
                          parameters_info ((sid > 7) ? 9 : sid+1, key, vars, val));
}

/*!
  \fn gchar * get_this_prop_string (int sid, int oid, int type, int calc)

  \brief get property name

  \param sid the type of field property
  \param oid the number of atoms for this field property
  \param type the id of the force field database
  \param calc from the tree model (1) or from the selection combo box (0)
*/
gchar * get_this_prop_string (int sid, int oid, int type, int calc)
{
  gchar * str;
  int i, j, k, l;
  i = (sid > 7) ? 1 : struct_id (sid+7);
  ff_data = get_ff_data (sid, type);
  float * val;
  for (j=0; j<i; j++)
  {
    k = ff_data -> atoms_id[oid][j];
    if (j > 0)
    {
      str = g_strdup_printf ("%s-%s", str, (k < 0) ? "X" : ff_atoms[k][2]);
    }
    else
    {
      str = g_strdup_printf ("%s", (k < 0) ? "X" : ff_atoms[k][2]);
    }
  }
  j = abs(ff_data -> key)-1;
  k = fvalues[activef][(sid < 8) ? sid+1 : 9][j];
  val = allocfloat (k);
  for (l=0; l<k; l++) val[l] = ff_data -> param[oid][l];
   // Here
  if (sid == 8 && calc)
  {
    if (tmp_field -> type <= CHARMMSI || tmp_field -> type > COMPASS)
    {
      float v = sqrt(ff_data -> param[oid][0]*ff_data -> param[oid][0]);
      val[0] = v*pow((double)(ff_data -> param[oid][1] + ff_data -> param[oid][1]), 12.0);
      val[1] = 2.0 * v * pow((double)(ff_data -> param[oid][1] + ff_data -> param[oid][1]), 6.0);
    }
    else if (tmp_field -> type <= CVFF_AUG)
    {
      for (l=0; l<2; l++) val[l] = sqrt(ff_data -> param[oid][l]*ff_data -> param[oid][l]);
    }
  }

  str = g_strdup_printf ("%s: %s", str, get_this_prop_param(sid, j, calc, 1, val));
  g_free (val);
  return str;
}

/*!
  \fn field_object_match * duplicate_match (field_object_match * old_m)

  \brief create a copy of a field match

  \param old_m the field match to duplicate
*/
field_object_match * duplicate_match (field_object_match * old_m)
{
  field_object_match * new_m = g_malloc0 (sizeof*new_m);
  new_m -> id = old_m -> id;
  new_m -> obj = old_m -> obj;
  new_m -> oid = old_m -> oid;
  new_m -> type = old_m -> type;
  new_m -> use = old_m -> use;
  return new_m;
}

/*!
  \fn void update_result_list (int sid, field_object_match * new_match)

  \brief update avialable parameter list

  \param sid the type of field property
  \param new_match the field match
*/
void update_result_list (int sid, field_object_match * new_match)
{
  if (tmp_res[sid])
  {
    field_object_match * map, * mbp;
    map = duplicate_match (new_match);
    mbp = tmp_res[sid];
    while (mbp)
    {
      if (mbp -> id == map -> id && mbp -> obj == map -> obj)
      {
        // Changing a value already saved
        if (mbp -> next && mbp -> prev)
        {
          mbp -> prev -> next = map;
          map -> prev = mbp -> prev;
          map -> next = mbp -> next;
          mbp -> next -> prev = map;
        }
        else if (mbp -> next)
        {
          mbp -> next -> prev = map;
          map -> next = mbp -> next;
        }
        else if (mbp -> prev)
        {
          mbp -> prev -> next = map;
          map -> prev = mbp -> prev;
        }
      }
      else if (! mbp -> next)
      {
        mbp -> next = map;
        map -> prev = mbp -> next;
        mbp = mbp -> next;
      }
      mbp = mbp -> next;
    }
  }
  else
  {
    tmp_res[sid] = duplicate_match (new_match);
  }
}

/*!
  \fn void fill_update_model (GtkTreeStore * store)

  \brief fill the parameters database tree store

  \param store the GtkTreeStore to fill
*/
void fill_update_model (GtkTreeStore * store)
{
  GtkTreeIter prop_level, struct_level;
  field_atom* upat;
  int g, h, i, j;
  gchar * str;
  gchar * stra, * strb, * strc, * strd, * stre;
  for (i=0; i<9; i++)
  {
    g = (i==8) ? i+1 : i;
    h = (i==8) ? 2 : struct_id (i+7);
    if (up_match[i])
    {
      if (g%2 && i < 6)
      {
        if (i == 3 && tmp_field -> type > AMBER99 && tmp_field -> type < CVFF_AUG)
        {
          str = g_strdup_printf ("<b>%s(s)</b> <sup>(2,3)</sup>", felemts[8+g]);
        }
        else
        {
          str = g_strdup_printf ("<b>%s(s)</b> <sup>(2)</sup>", felemts[8+g]);
        }
      }
      else if (i < 8)
      {
        if (i == 2 && tmp_field -> type > AMBER99 && tmp_field -> type < CVFF_AUG)
        {
          str = g_strdup_printf ("<b>%s(s)</b> <sup>(3)</sup>", felemts[8+g]);
        }
        else
        {
          str = g_strdup_printf ("<b>%s(s)</b>", felemts[8+g]);
        }
      }
      else if (tmp_field -> type <= AMBER99 || tmp_field -> type > COMPASS)
      {
        str = g_strdup_printf ("<b>%s(s)</b> <sup>(3)</sup>", felemts[8+i]);
      }
      else if (tmp_field -> type <= CHARMMSI)
      {
        str = g_strdup_printf ("<b>%s(s)</b> <sup>(4)</sup>", felemts[8+i]);
      }
      else
      {
        str = g_strdup_printf ("<b>%s(s)</b>", felemts[8+i]);
      }
      gtk_tree_store_append (store, & prop_level, NULL);
      gtk_tree_store_set (store, & prop_level, 0, -(i+1),
                                               1, 0,
                                               2, str,
                                               3, NULL,
                                               4, NULL,
                                               5, NULL,
                                               6, NULL,
                                               NUCOL-1, FALSE, -1);
      g_free (str);
      tmp_match = up_match[i];
      while (tmp_match)
      {
        if (i<8)
        {
          tmp_fstr = get_active_struct (i, tmp_fmol -> id, tmp_match -> id);
          if (tmp_match -> obj > 0)
          {
            tmp_fprop = get_active_prop (tmp_fstr -> other, tmp_match -> obj - 1);
            tmp_match -> use = TRUE;
          }
          else
          {
            tmp_fprop = tmp_fstr -> def;
            tmp_match -> use = TRUE;
          }
        }
        else
        {
          tmp_fbody = get_active_body (tmp_match -> obj, 0);
          tmp_match -> use = TRUE;
        }
        for (j=0; j<h; j++)
        {
          if (i < 8)
          {
            upat = get_active_atom (tmp_fmol -> id, tmp_fstr -> aid[j]);
          }
          else
          {
            upat = get_active_atom (tmp_fbody -> ma[j][0], tmp_fbody -> a[j][0]);
          }
          if (j > 0)
          {
            stra = g_strdup_printf ("%s - ""%s""", stra, upat -> name);
            if (tmp_match -> obj > 0 && i < 8) strb = g_strdup_printf ("%s-%d", strb, tmp_fprop -> aid[j]+1);
            strc = g_strdup_printf ("%s - %s", strc, tmp_proj -> chemistry -> label[upat -> sp]);
          }
          else
          {
            stra = g_strdup_printf ("""%s""", upat -> name);
            if (tmp_match -> obj > 0 && i < 8) strb = g_strdup_printf ("%d", tmp_fprop -> aid[j]+1);
            strc = g_strdup_printf ("%s", tmp_proj -> chemistry -> label[upat -> sp]);
          }
        }
        other_match = tmp_match;
        if (tmp_match -> use)
        {
          update_result_list (i, tmp_match);
          strd = g_strdup_printf ("%s", get_this_prop_string (i, other_match -> oid, other_match -> type, 1));
        }
        else
        {
          strd = g_strdup_printf ("Select ...");
        }

        if (i < 8)
        {
          stre = g_strdup_printf ("%s", get_this_prop_param(i, tmp_fprop -> key, 1, 0, tmp_fprop -> val));
        }
        else
        {
          stre = g_strdup_printf ("%s", get_this_prop_param(i, tmp_fbody -> key, 1, 0, tmp_fbody -> val));
        }
        if (tmp_match -> obj > 0  && i < 8)
        {
          str = g_strdup_printf ("%s", strb);
          g_free (strb);
        }
        else
        {
          str = g_strdup_printf ("%s <sup>(*)</sup>", stra);
        }
        gtk_tree_store_append (store, & struct_level, & prop_level);
        while (other_match && other_match -> id == tmp_match -> id && other_match -> obj == tmp_match -> obj)
        {
          other_match = other_match -> next;
        }
        gtk_tree_store_set (store, & struct_level, 0, i,
                                                   1, (i == 8) ? tmp_match -> obj : tmp_match -> id,
                                                   2, NULL,
                                                   3, str,
                                                   4, strc,
                                                   5, strd,
                                                   6, stre,
                                                   NUCOL-1, tmp_match -> use, -1);
        g_free (str);
        g_free (stra);
        g_free (strc);
        g_free (strd);
        g_free (stre);
        tmp_match = other_match;
      }
    }
  }
}

/*!
  \fn GtkTreeModel * global_render_tree ()

  \brief create the field property combo box model that contains the list of available parameters
*/
GtkTreeModel * global_render_tree ()
{
  int i;
  GtkTreeIter iter;
  GtkTreeStore * store;
  gchar * str;
  store = gtk_tree_store_new (1, G_TYPE_STRING);
  gtk_tree_store_append (store, & iter, NULL);
  gtk_tree_store_set (store, & iter, 0, "Select ...", -1);
  for (i=0; i<9; i++)
  {
    if (up_match[i])
    {
      other_match = up_match[i];
      while (other_match)
      {
        str = g_strdup_printf ("%s", get_this_prop_string (i, other_match -> oid, other_match -> type, 1));
        gtk_tree_store_append (store, & iter, NULL);
        gtk_tree_store_set (store, & iter, 0, str, -1);
        g_free (str);
        other_match = other_match -> next;
      }
    }
  }
  return gtk_tree_model_filter_new(GTK_TREE_MODEL (store), NULL);
}

/*!
  \fn dint get_visible (gboolean result, gchar * the_name)

  \brief get cell renderer visibility poitner

  \param result is renderer visible or not ?
  \param the_name the target name
*/
dint get_visible (gboolean result, gchar * the_name)
{
  dint vis;
  gchar * str;
  gboolean done = FALSE;
  // With the "Select ..." path starts at 1 and not 0
  vis.a = vis.b = 0;
  int i;
  for (i=0; i<9; i++)
  {
    if (up_match[i])
    {
      other_match = up_match[i];
      while (other_match)
      {
        if (i == up.a && ((i < 8 && other_match -> id == up.b) || (i == 8 && other_match -> obj == up.b)))
        {
          if (result)
          {
            str = g_strdup_printf ("%s", get_this_prop_string (i, other_match -> oid, other_match -> type, 1));
            if (g_strcmp0 (str, the_name) == 0)
            {
              vis.a = -1;
              return vis;
            }
          }
          vis.b ++;
          done = TRUE;
        }
        else if (! done)
        {
          vis.a ++;
        }
        other_match = other_match -> next;
      }
      if (done) break;
    }
  }
  return vis;
}
/*!
  \fn G_MODULE_EXPORT void changed_update_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data)

  \brief change combo box in tree model callback

  \param combo the cell renderer combo box
  \param path_string the path in the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_update_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data)
{
  GValue val = {0, };
  GObject * cmodel;
  dint res;
  g_object_get (combo, "model", & cmodel, NULL);
  gtk_tree_model_get_value ((GtkTreeModel *)cmodel, iter, 0, & val);
  if (gtk_tree_model_get_iter_from_string ((GtkTreeModel *)update_model, iter, path_string))
  {
    gchar * str = g_strdup_printf ("%s", (char *)g_value_get_string (& val));
    gtk_tree_store_set (update_model, iter, 5, str, -1);
    res = get_visible (TRUE, str);
    if (res.a < 0)
    {
      update_result_list (up.a, other_match);
    }
  }
}

/*!
  \fn void get_update_tree_data (GtkWidget * tree, gpointer data, GtkTreePath * path)

  \brief update force field parameters database tree model data

  \param tree the GtkWidget sending the signal
  \param data the associated data pointer
  \param path the path in the tree model
*/
void get_update_tree_data (GtkWidget * tree, gpointer data, GtkTreePath * path)
{
  GtkTreeModel * tmodel = gtk_tree_view_get_model (GTK_TREE_VIEW(tree));
  GtkTreeIter update_iter;
  up.a = up.b = -1;
  if (gtk_tree_model_get_iter (tmodel, & update_iter, path))
  {
    gtk_tree_model_get (tmodel, & update_iter, 0, & up.a, 1, & up.b, -1);
    gtk_tree_model_filter_refilter ((GtkTreeModelFilter *)prop_to_up);
  }
}

#ifdef GTK4
/*!
  \fn void ff_button_event (double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data)

  \brief handle mouse button event on the DL-POLY force field parameters database window GTK4

  \param event_x x position
  \param event_y y position
  \param event_button event button
  \param event_type event type
  \param event_time event time
  \param data the associated data pointer
*/
void ff_button_event (double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data)
#else
/*!
  \fn void ff_button_event (GdkEvent * event, double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data)

  \brief handle mouse button event on the DL-POLY force field parameters database window GTK3

  \param event the GdkEvent triggering the signal
  \param event_x x position
  \param event_y y position
  \param event_button event button
  \param event_type event type
  \param event_time event time
  \param data the associated data pointer
*/
void ff_button_event (GdkEvent * event, double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data)
#endif
{
  if (event_type == GDK_BUTTON_PRESS)
  {
    GtkTreePath * path;
    GtkTreeViewColumn * column;
    int i, j;
#ifdef GTK4
    int e_x, e_y;
    gtk_tree_view_convert_widget_to_bin_window_coords (GTK_TREE_VIEW(update_tree), event_x, event_y, & e_x, & e_y);
    gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(update_tree), e_x, e_y, & path, & column, & i, & j);
#else
    gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(update_tree), event_x, event_y, & path, & column, & i, & j);
#endif
    if (path != NULL)
    {
      get_update_tree_data (update_tree, data, path);
    }
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void on_ff_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief mouse button pressed signal on the DL-POLY force field parameters database window

  \param gesture the GtkGesture sending the signal
  \param n_press number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_ff_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  ff_button_event (x, y, gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture), GDK_BUTTON_PRESS, gtk_event_controller_get_current_event_time((GtkEventController *)gesture), data);
}

/*!
  \fn G_MODULE_EXPORT void on_ff_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief mouse button released signal on the DL-POLY force field parameters database window

  \param gesture the GtkGesture sending the signal
  \param n_press number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_ff_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  ff_button_event (x, y, gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture), GDK_BUTTON_RELEASE, gtk_event_controller_get_current_event_time((GtkEventController *)gesture), data);
}
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_ff_button_event (GtkWidget * widget, GdkEvent * event, gpointer data)

  \brief mouse button event on the DL-POLY force field parameters database window

  \param widget the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_ff_button_event (GtkWidget * widget, GdkEvent * event, gpointer data)
{
  GdkEventButton * bevent = (GdkEventButton *)event;
  ff_button_event (event, bevent -> x, bevent -> y, bevent -> button, bevent -> type, bevent -> time, data);
  return FALSE;
}
#endif

/*!
  \fn G_MODULE_EXPORT void on_toggle_update (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief  on select field parameter toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_toggle_update (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  GtkTreeIter iter;
  int i;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(update_model), & iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(update_model), & iter, 7, & i, -1);
  gchar * str;
  gtk_tree_model_get (GTK_TREE_MODEL(update_model), & iter, 5, & str, -1);
  dint res = get_visible(TRUE, str);
  g_free (str);
  if (res.a < 0)
  {
    tmp_match = tmp_res[up.a];
    while (tmp_match)
    {
      if (tmp_match -> id == other_match -> id && tmp_match -> obj == other_match -> obj)
      {
        tmp_match -> use = ! i;
        break;
      }
      tmp_match = tmp_match -> next;
    }
    gtk_tree_store_set (update_model, & iter, 7, ! i, -1);
  }
}

/*!
  \fn static gboolean update_rend (GtkTreeModel * model, GtkTreeIter * iter, gpointer  data)

  \brief udpate cell renderer visibility

  \param model the target GtkTreeModel
  \param iter the target tree iter
  \param data the associated data pointer
*/
static gboolean update_rend (GtkTreeModel * model, GtkTreeIter * iter, gpointer  data)
{
  GtkTreeIter upiter;
  dint vis;
  gchar * str;
  if (gtk_tree_model_get_iter_first(model, & upiter))
  {
    GtkTreePath * path = gtk_tree_model_get_path(model, iter);
    str = gtk_tree_path_to_string(path);
    gtk_tree_path_free(path);
    if (g_strcmp0 (str, "0") == 0) return TRUE;
    vis = get_visible (FALSE, NULL);
    int p = (int)string_to_double ((gpointer)str);
    g_free (str);
    if (p > vis.a && p < vis.a+vis.b+1) return TRUE;
  }
  return FALSE;
}

/*!
  \fn void field_set_markup_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief field parameter set renderer markup and visibility in the parameter database selection tree model

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_markup_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j, k;
  i = GPOINTER_TO_INT(data);
  gtk_tree_model_get (mod, iter, 0, & j, -1);
  k = (j < 0) ? 0 : 1;
  gtk_cell_renderer_set_visible (renderer,  (i == 2) ? ! k : k);
  if ((j < 0 && i == 2) || (j>-1 && i != 7))
  {
    gchar * str = NULL;
    gtk_tree_model_get (mod, iter, i, & str, -1);
    g_object_set (renderer, "markup", str, NULL, NULL);
    g_free (str);
  }
}

/*!
  \fn GtkWidget * create_update_tree ()

  \brief create the parameters database tree model
*/
GtkWidget * create_update_tree ()
{
  int i;
  gchar * str;
  gchar * up_title[NUCOL]={" ", " ", "Property", "Atoms", "Species",  "parameters (1)", "Actual parameters (1)", "Update ?"};
  GType up_type[NUCOL] = {G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN};

  update_model = gtk_tree_store_newv (NUCOL, up_type);
  update_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(update_model));
  for (i=0; i<NUCOL; i++)
  {
    if (i == 5)
    {
      str =  g_strdup_printf ("%s %s", field_acro[tmp_field -> type], up_title[i]);
    }
    else
    {
      str = g_strdup_printf ("%s", up_title[i]);
    }
    if (i == 5)
    {
      update_renderer[i] = gtk_cell_renderer_combo_new();
      prop_to_up = global_render_tree ();
      g_object_set (update_renderer[i], "model", prop_to_up, "text-column", 0, "has-entry", FALSE, "editable", TRUE, NULL);
      gtk_tree_model_filter_set_visible_func ((GtkTreeModelFilter *)prop_to_up, (GtkTreeModelFilterVisibleFunc)G_CALLBACK(update_rend), GTK_TREE_VIEW(update_tree), NULL);
      g_object_unref (prop_to_up);
      g_signal_connect (G_OBJECT(update_renderer[i]), "editing-started", G_CALLBACK(markup_action_renderer), NULL);
      g_signal_connect (G_OBJECT(update_renderer[i]), "changed", G_CALLBACK(changed_update_renderer), NULL);
      update_col[i] = gtk_tree_view_column_new_with_attributes (str, update_renderer[i], "text", i, NULL);
    }
    else if (i == NUCOL-1)
    {
      update_renderer[i] = gtk_cell_renderer_toggle_new ();
      update_col[i] = gtk_tree_view_column_new_with_attributes (str, update_renderer[i], "active", i, NULL);
      g_signal_connect (G_OBJECT(update_renderer[i]), "toggled", G_CALLBACK(on_toggle_update), NULL);
    }
    else
    {
      update_renderer[i] = gtk_cell_renderer_text_new();
      update_col[i] = gtk_tree_view_column_new_with_attributes (str, update_renderer[i], "text", i, NULL);
    }
    g_free (str);
    if (i > 1)
    {
      gtk_tree_view_column_set_alignment (update_col[i], 0.5);
      gtk_tree_view_append_column(GTK_TREE_VIEW(update_tree), update_col[i]);
      gtk_tree_view_column_set_cell_data_func (update_col[i], update_renderer[i], field_set_markup_and_visible, GINT_TO_POINTER(i), NULL);
    }
  }
  for (i=0; i<9; i++) tmp_res[i] = NULL;
  fill_update_model (update_model);
  g_object_unref (update_model);
  GtkTreeSelection * update_select = gtk_tree_view_get_selection (GTK_TREE_VIEW(update_tree));
  gtk_tree_selection_set_mode (update_select, GTK_SELECTION_SINGLE);
#ifdef GTK4
  add_widget_gesture_and_key_action (update_tree, "ff-button-pressed", G_CALLBACK(on_ff_button_pressed), NULL,
                                                  "ff-button-released", G_CALLBACK(on_ff_button_released), NULL,
                                                  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

#else
  g_signal_connect (G_OBJECT(update_tree), "button_press_event", G_CALLBACK(on_ff_button_event), NULL);
#endif
  return update_tree;
}

/*!
  \fn void win_update_tree (GtkWidget * vbx)

  \brief update the parameters database tree model

  \param vbx the GtkWidget sending the signal
*/
void win_update_tree (GtkWidget * vbx)
{
  int i;
  update_tree = NULL;
  update_model = NULL;
  for (i=0; i<NUCOL; i++)
  {
    update_renderer[i] = NULL;
    update_col[i] = NULL;
  }
  GtkWidget * scrollsets = create_scroll (NULL, -1, -1, GTK_SHADOW_ETCHED_IN);
  gtk_widget_set_size_request (scrollsets, 800, 400);

  add_container_child (CONTAINER_SCR, scrollsets, create_update_tree());
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, scrollsets, FALSE, FALSE, 20);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbx, hbox, FALSE, FALSE, 20);
  gchar * funits[5] ={"Ev", "kcal mol<sup>-1</sup>", "kJ mol<sup>-1</sup>", "K B<sup>-1</sup>", "DL_POLY internal units"};
  gchar * str = g_strdup_printf ("\t<b>(*)</b> Each force field element can be tuned separately, if not this <b>default</b> parameters will be used.\n"
                                 "\t<b>(1)</b> %s energy unit: %s, if required conversion to FIELD file energy unit will be performed upon selection.\n"
                                 "\t<b>(2)</b> Restraint parameters are duplicates of the non-restraint parameters.\n",
                                 field_acro[tmp_field -> type], funits[ff_unit]);
  i = 3;
  if (tmp_field -> type > AMBER99 && tmp_field -> type < CVFF)
  {
    str = g_strdup_printf ("%s\t<b>(%d)</b> Urey-Bradley terms provided by the %s force field are ignored.\n", str, i, field_acro[tmp_field -> type]);
    i ++;
  }
  if (tmp_field -> type <= CVFF_AUG || tmp_field -> type > COMPASS)
  {
    str = g_strdup_printf ("%s\t<b>(%d)</b> In %s, 12-6 non-bonded interactions are evaluated using:", str, i, field_acro[tmp_field -> type]);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbx, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
  g_free (str);
  if (tmp_field -> type <= CVFF_AUG || tmp_field -> type > COMPASS)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbx, hbox, FALSE, FALSE, 0);
    if (tmp_field -> type < CVFF || tmp_field -> type > COMPASS)
    {
      str = g_strdup_printf ("%s\n<i><b>A</b></i> and <i><b>B</b></i> are calculated using Ɛ<sub>i/j</sub> and r0<sub>i/j</sub> provided by the force field parameters.", get_this_vdw_string());
      if (tmp_field -> type > AMBER99)
      {
        str = g_strdup_printf ("%s\nScaled 1-4 exclusion parameters, provided by the %s force field, are ignored.", str, field_acro[tmp_field -> type]);
      }
    }
    else
    {
      str = g_strdup_printf ("%s\n<i><b>A</b></i> and <i><b>B</b></i> are calculated using A<sub>i/j</sub> and B<sub>i/j</sub> provided by the force field parameters.", get_this_vdw_string());
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 200);
    g_free (str);
  }
}

/*!
  \fn void look_up_this_field_object (int fsid, int fpid, int ssid, int nat, int * fsp, int * fat)

  \brief look up in database for this field property

  \param fsid the type of field property
  \param fpid global propert (0) or atom specific (> 0)
  \param ssid the id of the field property to check
  \param nat the number of atoms for this field property
  \param fat the list of field atom(s)
  \param fsp the list of the chemical species for the field atom(s)
*/
void look_up_this_field_object (int fsid, int fpid, int ssid, int nat, int * fsp, int * fat)
{
  int i, j;
  tmp_obj_id = field_objects_id[(fsid < 6) ? fsid / 2 : fsid - 3];
  while (tmp_obj_id)
  {
    i = tmp_obj_id -> oid;
    j = tmp_obj_id -> type;
    ff_data = get_ff_data (fsid, j);
    if (is_this_object_a_match (fsid, nat, fsp, ff_data -> atoms_z[i]))
    {
      if (is_this_object_a_match (fsid, nat, fat, ff_data -> atoms_id[i]))
      {
        if (up_match[fsid] == NULL)
        {
          up_match[fsid] = g_malloc0 (sizeof*up_match[fsid]);
          tmp_match = up_match[fsid];
        }
        else
        {
          tmp_match -> next = g_malloc0 (sizeof*tmp_match);
          tmp_match = tmp_match -> next;
        }
        tmp_match -> id = ssid;
        tmp_match -> obj = fpid;
        tmp_match -> oid = i;
        tmp_match -> type = j;
        // g_debug ("Match :: fsid= %d, -> id = %d, -> obj = %d, -> oid = %d, -> type = %d", fsid, ssid, fpid, i, j);
      }
    }
    tmp_obj_id = tmp_obj_id -> next;
  }
}

/*!
  \fn void check_this_fprop (int fsid, int fpid, int ssid, int * fat, int * fsp)

  \brief check if the database offers parameters for a field property

  \param fsid the type of field property
  \param fpid global propert (0) or atom specific (> 0)
  \param ssid the id of the field property to check
  \param fat the list of field atom(s)
  \param fsp the list of the chemical species for the field atom(s)
*/
void check_this_fprop (int fsid, int fpid, int ssid, int * fat, int * fsp)
{
  int i, j, k;
  int * ffat, * ffsp;
  gboolean update = TRUE;
  field_atom* tmpat;
  i = struct_id(fsid+7);
  for (j=0; j<i; j++)
  {
    k = fsp[j];
    tmpat = get_active_atom (tmp_fmol -> id, fat[j]);
    if (tmpat -> id != tmp_fat -> id)
    {
      if (tmpat -> afid < 0)
      {
        if (atoms_id[k] == 0 && extraz_id[(fsid < 6)? fsid/2: fsid-3][k] == 0)
        {
          update = FALSE;
        }
      }
    }
  }
  if (update)
  {
    // Is there any field data available ?
    // Check the field data for available parameters
    ffat = allocint(i);
    ffsp = allocint(i);
    for (j=0; j<i; j++)
    {
      ffat[j] = get_active_atom (tmp_fmol -> id, fat[j]) -> afid;
      ffsp[j] = tmp_proj -> chemistry -> chem_prop[CHEM_Z][get_active_atom (tmp_fmol -> id, fat[j]) -> sp];
    }
    look_up_this_field_object (fsid, fpid, ssid, i, ffsp, ffat);
    g_free (ffat);
    g_free (ffsp);
  }
}

/*!
  \fn int this_body_has_atom (field_nth_body * body, char * name)

  \brief check if non bonded potential has this field atom

  \param body the non bonded potential to check
  \param name the name of the target field atom
*/
int this_body_has_atom (field_nth_body * body, char * name)
{
  int i, j, k, l , m;
  m = 0;
  for (i=0; i<2; i++)
  {
    for (j=0; j<body -> na[i]; j++)
    {
      k = body -> ma[i][j];
      l = body -> a[i][j];
      if (g_strcmp0 (get_active_atom(k, l) -> name, name) == 0) m += (i+1);
    }
  }
  return m;
}

/*!
  \fn G_MODULE_EXPORT void run_check_atom_for_updates (GtkDialog * dialog, gint response_id, gpointer data)

  \brief check for parameters in the database to be used in the force field - running the dialog

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_check_atom_for_updates (GtkDialog * dialog, gint response_id, gpointer data)
{
  int i, j, k, l, m;
  float v;
  gboolean update_vdw = FALSE;
  if (response_id == GTK_RESPONSE_APPLY)
  {
    for (i=0; i<9; i++)
    {
      if (tmp_res[i])
      {
        tmp_match = other_match = tmp_res[i];
        while (other_match)
        {
          if (tmp_match -> use)
          {
            if (i < 8)
            {
              tmp_fstr = get_active_struct (i, tmp_fmol -> id, tmp_match -> id);
              tmp_fprop = (tmp_match -> obj > 0) ? get_active_prop (tmp_fstr -> other, tmp_match -> obj - 1) : tmp_fstr -> def;
              g_free (tmp_fprop -> val);
              ff_data = get_ff_data (i, tmp_match -> type);
              tmp_fprop -> val = duplicate_float(ff_data -> npar, ff_data -> param[tmp_match -> oid]);
              // UNITS !
              tmp_fprop -> fpid = tmp_match -> oid;
              tmp_fprop -> key = abs(ff_data -> key)-1;
              if ((i < 2 && (tmp_fprop -> key == 1 || tmp_fprop -> key == 5)) || ((i == 2 || i == 3) && tmp_fprop -> key < 2))
              {
                for (l=0; l<ff_data -> npar; l++)
                {
                  if (l != 1) tmp_fprop -> val[l] *= (l+2 - l/3);
                }
              }
              if (ff_unit != tmp_field -> energy_unit)
              {
                v =  internal_to_other[tmp_field -> energy_unit] / internal_to_other[ff_unit];
                tmp_fprop -> val[0] = tmp_fprop -> val[0]*v;
                if ((i/2 == 1 && tmp_fprop -> key == 1) || (i/2 == 1 && tmp_fprop -> key == 3))
                {
                  tmp_fprop -> val[2] = tmp_fprop -> val[2]*v;
                  tmp_fprop -> val[3] = tmp_fprop -> val[3]*v;
                }
                else if (i/2 == 0 && tmp_fprop -> key == 1)
                {
                  tmp_fprop -> val[2] = tmp_fprop -> val[2]*v;
                }
              }
              tmp_fprop -> use = TRUE;
            }
            else
            {
              // Pot must be defined for the 2 atoms
              tmp_fbody = get_active_body (0, 0);
              ff_data = get_ff_data (i, tmp_match -> type);
              while (tmp_fbody)
              {
                update_vdw = FALSE;
                j = this_body_has_atom (tmp_fbody, tmp_fat -> name);
                k = tmp_match -> oid;
                if (j == 3)
                {
                  tmp_fbody -> fpid[0] = tmp_fbody -> fpid[1] = k;
                  j = l = k;
                  update_vdw = TRUE;
                }
                else if (j)
                {
                  j --;
                  tmp_fbody -> fpid[j] = k;
                  // Need to do something here !
                  l = (j) ? 0 : 1;
                  update_vdw = FALSE;
                  if (tmp_fbody -> fpid[l] > -1)
                  {
                    j = k;
                    l = tmp_fbody -> fpid[l];
                    update_vdw = TRUE;
                  }
                }
                if (update_vdw)
                {
                  if (tmp_field -> type <= CHARMMSI || tmp_field -> type > COMPASS)
                  {
                    v = sqrt(ff_data -> param[j][0]*ff_data -> param[l][0]);
                    tmp_fbody -> val[0] = v*pow((double)(ff_data -> param[j][1] + ff_data -> param[l][1]), 12.0);
                    tmp_fbody -> val[1] = 2.0 * v * pow((double)(ff_data -> param[j][1] + ff_data -> param[l][1]), 6.0);
                  }
                  else if (tmp_field -> type <= CVFF_AUG)
                  {
                    for (m=0; m<2; m++) tmp_fbody -> val[m] = sqrt(ff_data -> param[j][m]*ff_data -> param[l][m]);
                  }
                }
                tmp_fbody = tmp_fbody -> next;
              }
            }
          }
          other_match = tmp_match -> next;
          g_free (tmp_match);
          tmp_match = other_match;
        }
      }
    }
    for (i=0; i<9; i++)
    {
      if (up_match[i])
      {
        tmp_match = other_match = up_match[i];
        while (other_match)
        {
          other_match = tmp_match -> next;
          g_free (tmp_match);
          tmp_match = other_match;
        }
        up_match[i] = NULL;
      }
    }
  }
  destroy_this_dialog (dialog);
}

/*!
  \fn void check_atom_for_updates ()

  \brief check for parameters in the database to be used in the force field - creating the dialog
*/
void check_atom_for_updates ()
{
  int i, j, k, l;
  int * fsp;
  gboolean update_this = FALSE;
  field_prop * oth;

  // Lookup for this atoms in bonds, etc ... and if parameters, then look for update is all atoms are field auto
  // Field struct (bonds, bond rest., angles, angles rest., dih, tors. rest., imp., vdw.)
  // g_debug ("Checking for field parameters of atoms: %s - id = %d", tmp_fat -> name, tmp_fat -> id + 1);
  for (i=0; i<8; i++)
  {
    up_match[i] = NULL;
    if ((i != 4 && i != 5) || tmp_field -> type < CFF91 || tmp_field -> type > COMPASS)
    {
      if (get_ff_data(i, 0) && tmp_field -> afp[i+15])
      {
        k = struct_id(i+7);
        fsp = allocint(k);
        tmp_fstr = tmp_fmol -> first_struct[i];
        while (tmp_fstr != NULL)
        {
          for (j=0; j<k; j++)
          {
            if (tmp_fstr -> aid[j] == tmp_fat -> id)
            {
              // Update this field struct parameters
              for (l=0; l<k; l++)
              {
                fsp[l]= get_active_atom (tmp_fmol -> id, tmp_fstr -> aid[l]) -> sp;
              }
              check_this_fprop (i, 0, tmp_fstr -> id, tmp_fstr -> aid, fsp);
              oth = tmp_fstr -> other;
              while (oth != NULL)
             {
                check_this_fprop (i, oth -> pid+1, tmp_fstr -> id, tmp_fstr -> aid, fsp);
                oth = oth -> next;
              }
              break;
            }
          }
          tmp_fstr = tmp_fstr -> next;
        }
        g_free (fsp);
      }
    }
  }
  up_match[8] = NULL;
  if (tmp_field -> type < CFF91 || tmp_field -> type > COMPASS)
  {
    tmp_fbody = get_active_body (0, 0);
    while (tmp_fbody && tmp_field -> afp[23])
    {
      if (this_body_has_atom (tmp_fbody, tmp_fat -> name) == 3)
      {
        tmp_obj_id = field_objects_id[5];
        while (tmp_obj_id)
        {
          i = tmp_proj -> chemistry -> chem_prop[CHEM_Z][tmp_fat -> sp];
          j = tmp_obj_id -> oid;
          if (ff_vdw -> atoms_z[j][0] == i && (ff_vdw -> atoms_id[j][0] == tmp_fat -> afid || ff_vdw -> atoms_id[j][0] == -1))
          {
            if (up_match[8] == NULL)
            {
              up_match[8] = g_malloc0 (sizeof*up_match[8]);
              tmp_match = up_match[8];
            }
            else
            {
              tmp_match -> next = g_malloc0 (sizeof*tmp_match);
              tmp_match = tmp_match -> next;
            }
            tmp_match -> id = 8;
            tmp_match -> obj = tmp_fbody -> id;
            tmp_match -> oid = tmp_obj_id -> oid;
            tmp_match -> type = 0;
            tmp_match -> use = FALSE;
          }
          tmp_obj_id = tmp_obj_id -> next;
        }
      }
      tmp_fbody = tmp_fbody -> next;
    }
  }
  for (i=0; i<9; i++)
  {
    if (up_match[i] != NULL)
    {
      update_this = TRUE;
      break;
    }
  }
  if (update_this)
  {
    gchar * str = g_strdup_printf ("Update FIELD file with %s parameters using \"%s\" atom(s)", field_acro[tmp_field -> type], exact_name(ff_atoms[tmp_fat -> afid][2]));
    GtkWidget * dial = dialog_cancel_apply (str, field_assistant, FALSE);
    g_free (str);
    GtkWidget * vbox = dialog_get_content_area (dial);
    win_update_tree (vbox);
    run_this_gtk_dialog (dial, G_CALLBACK(run_check_atom_for_updates), NULL);
  }
}

/*!
  \fn G_MODULE_EXPORT void changed_field_prop_combo (GtkComboBox * box, gpointer data)

  \brief change the field parameter selection

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_field_prop_combo (GtkComboBox * box, gpointer data)
{
  int i, j;
  dint * prop = (dint *)data;
  float * val, * vbl;
  i = combo_get_active ((GtkWidget *)box);
  if (i)
  {
    tmp_match = up_match[prop -> b];
    for (j=0; j<i-1; j++) tmp_match = tmp_match -> next;
    ff_data = get_ff_data (prop -> a - 7, tmp_match -> type);
    if (prop -> a < 15)
    {
      tmp_fprop -> key = abs(ff_data -> key) - 1;
      g_free (tmp_fprop -> val);
      tmp_fprop -> fpid = tmp_match -> oid;
      tmp_fprop -> val = duplicate_float (ff_data -> npar, ff_data -> param[tmp_match -> oid]);
      if ((prop -> a < 9 && (tmp_fprop -> key == 0 || tmp_fprop -> key == 5)) || ((prop -> a == 9 || prop -> a == 10) && tmp_fprop -> key < 2))
      {
        for (j=0; j<ff_data -> npar; j++)
        {
          if (j != 1) tmp_fprop -> val[j] *= (j+2 - j/3);
        }
      }
      if (ff_unit != tmp_field -> energy_unit)
      {
        float v =  internal_to_other[tmp_field -> energy_unit] / internal_to_other[ff_unit];
        tmp_fprop -> val[0] = tmp_fprop -> val[0]*v;
        if (((prop -> a - 7)/2 == 1 && tmp_fprop -> key == 1) || ((prop -> a - 7)/2 == 1 && tmp_fprop -> key == 3))
        {
          tmp_fprop -> val[2] = tmp_fprop -> val[2]*v;
          tmp_fprop -> val[3] = tmp_fprop -> val[3]*v;
        }
        else if ((prop -> a - 7)/2 == 0 && tmp_fprop -> key == 1)
        {
          tmp_fprop -> val[2] = tmp_fprop -> val[2]*v;
        }
      }
      combo_set_active (field_key_combo, tmp_fprop -> key);
    }
    else
    {
      // Check both atoms pot, and use same pot.
      tmp_fbody -> key = abs(ff_data -> key) - 1;
      tmp_fbody -> fpid[prop -> b] = tmp_match -> oid;
      val = duplicate_float (ff_data -> npar, ff_data -> param[tmp_match -> oid]);
      combo_set_active (field_key_combo, tmp_fbody -> key);
    }
  }
  else
  {
    if (prop -> a < 15)
    {
      tmp_fprop -> fpid = -1;
    }
    else
    {
      tmp_fbody -> fpid[prop -> b] = -1;
    }
  }
  if (prop -> a == 15)
  {
    float * vcl;
    float eij;
    float rij;
    i = (prop -> b) ? 0 : 1;
    if (vdw_same_atom)
    {
      tmp_fbody -> fpid[i] = tmp_fbody -> fpid[!i];
    }
    if (tmp_fbody -> fpid[0] > -1 && tmp_fbody -> fpid[1] > -1)
    {
      j = tmp_fbody -> fpid[i];
      vbl = duplicate_float (ff_data -> npar, ff_data -> param[j]);
      if ((tmp_field -> type <= CHARMMSI || tmp_field -> type > COMPASS) && tmp_fbody -> key == 0)
      {
        // 12-6, recalculate A and B
        vcl = allocfloat (ff_data -> npar);
        eij = sqrt(val[0]*vbl[0]);
        rij = (val[1] + vbl[1]);
        vcl[0] = eij * pow((double) rij, 12.0);
        vcl[1] = 2.0 * eij * pow((double) rij, 6.0);
        g_free (tmp_fbody -> val);
        tmp_fbody -> val = duplicate_float(ff_data -> npar, vcl);
        g_free (vcl);
      }
      else if (tmp_field -> type == CVFF || tmp_field -> type == CVFF_AUG)
      {
        // 12-6, A-B
        vcl = allocfloat (ff_data -> npar);
        vcl[0] = sqrt(val[0]*vbl[0]);
        vcl[1] = sqrt(val[1]*vbl[1]);
        g_free (tmp_fbody -> val);
        tmp_fbody -> val = duplicate_float(ff_data -> npar, vcl);
        g_free (vcl);
      }
      g_free (vbl);
    }
    g_free (val);
  }
  p_box = destroy_this_widget (p_box);
  p_box = param_prop_param_box (prop -> a);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, param_box, p_box, FALSE, FALSE, 0);
  show_the_widgets (param_box);
}

/*!
  \fn GtkWidget * create_field_prop_combo (int f, int is_moy)

  \brief create field parameter selection combo box

  \param f the type of force field property
  \param is_moy global parameter (1) or atom(s) specific parameter (0)
*/
GtkWidget * create_field_prop_combo (int f, int is_moy)
{
  int h, i, j, k, l, m, n;
  GtkWidget * vbox, * hbox;
  gboolean save_it;
  gchar * str_vdw[2];
  gchar * str;
  h = (f == 15) ? 2 : struct_id(f);
  i = (f == 15) ? 2 : 1;
  for (j=0; j<i; j++) up_match[j] = g_malloc0 (sizeof*up_match[j]);
  int * spec_z = allocint (h);
  vbox = create_vbox (BSEP);
  if (f == 15)
  {
    vdw_same_atom = FALSE;
    j = get_num_vdw_max ();
    for (j=0; j<2; j++)
    {
      str_vdw[j] = g_strdup_printf ("%s", get_body_element_name (tmp_fbody, j, 0));
      l = tmp_fbody -> ma[j][0];
      m = tmp_fbody -> a[j][0];
      n = get_active_atom (l, m) -> sp;
      spec_z[j] = tmp_proj -> chemistry -> chem_prop[CHEM_Z][n];
    }
    if (g_strcmp0 (str_vdw[0], str_vdw[1]) == 0)
    {
      vdw_same_atom = TRUE;
      i = 1;
    }
    for (j=0; j<i; j++)
    {
      hbox = create_hbox(0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
      ff_p_combo[j] = create_combo ();
      combo_text_append (ff_p_combo[j], "Manual");
      l = tmp_fbody -> ma[j][0];
      m = tmp_fbody -> a[j][0];
      if (i == 1)
      {
        str = g_strdup_printf ("Atom (1 and 2) <b>%s</b>", str_vdw[j]);
      }
      else
      {
        str = g_strdup_printf ("Atom (%d) <b>%s</b>", j+1, str_vdw[j]);
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 150, -1, 0.0, 0.5), FALSE, FALSE, 10);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ff_p_combo[j], FALSE, FALSE, 10);
    }
    for (j=0; j<2; j++) g_free (str_vdw[j]);
  }
  else
  {
    hbox = create_hbox(0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    ff_p_combo[0] = create_combo ();
    combo_text_append (ff_p_combo[0], "Manual");
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ff_p_combo[0], FALSE, FALSE, 10);
    for (j=0; j<h; j++)
    {
      if (is_moy)
      {
        k = tmp_fstr -> aid[j];
        l = get_active_atom (tmp_fmol -> id, k) -> sp;
      }
      else
      {
        k = tmp_fprop -> aid[j];
        l = get_active_atom (tmp_fmol -> id, tmp_fmol -> atoms_id[k][0].a) -> sp;
      }
      spec_z[j] = tmp_proj -> chemistry -> chem_prop[CHEM_Z][l];
    }
  }
  if (f != 15 || tmp_field -> type < CFF91 || tmp_field -> type > COMPASS)
  {
    for (j=0; j<i; j++)
    {
      tmp_obj_id = field_objects_id[(f-7) < 6 ? (f-7) / 2 : f-10];
      m = n = 0;
      up_match[j] = NULL;
      while (tmp_obj_id)
      {
        k = tmp_obj_id -> oid;
        l = tmp_obj_id -> type;
        ff_data = get_ff_data (f-7, l);
        save_it = FALSE;
        if (f < 15 && is_this_object_a_match (f-7, h, spec_z, ff_data -> atoms_z[k]))
        {
          m ++;
          if (tmp_fprop -> fpid == k && tmp_fprop -> key == abs(ff_data -> key)-1) n = m;
          save_it = TRUE;
        }
        else if (f == 15 && ff_data -> atoms_z[k][0] == spec_z[j])
        {
          m ++;
          if (tmp_fbody -> fpid[j] == k) n = m;
          save_it = TRUE;
        }
        if (save_it)
        {
          str = g_strdup_printf ("%s", get_this_prop_string (f-7, k, l, 0));
          if (ff_data -> info)
          {
            if (g_strcmp0 (ff_data -> info[k], " ") != 0) str = g_strdup_printf ("%s : %s", str, ff_data -> info[k]);
          }
          combo_text_append (ff_p_combo[j], str);
          g_free (str);
          if (up_match[j] == NULL)
          {
            up_match[j] = g_malloc0 (sizeof*up_match[j]);
            tmp_match = up_match[j];
          }
          else
          {
            tmp_match -> next = g_malloc0 (sizeof*tmp_match -> next);
            tmp_match -> next -> id = tmp_match -> id + 1;
            tmp_match = tmp_match -> next;
          }
          tmp_match -> obj = f - 7;
          tmp_match -> oid = k;
          tmp_match -> type = l;
        }
        tmp_obj_id = tmp_obj_id -> next;
      }
      combo_set_markup (ff_p_combo[j]);
      pup[j].a = f;
      pup[j].b = j;
      combo_set_active (ff_p_combo[j], n);
      if (m == 0) widget_set_sensitive (ff_p_combo[j], FALSE);
      g_signal_connect (G_OBJECT(ff_p_combo[j]), "changed", G_CALLBACK(changed_field_prop_combo), & pup[j]);
    }
  }
  return vbox;
}
