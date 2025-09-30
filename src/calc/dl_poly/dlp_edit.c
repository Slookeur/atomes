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
* @file dlp_edit.c
* @short Functions to edit DL-POLY force field parameters
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_edit.c'
*
* Contains:
*

 - The functions to edit DL-POLY force field parameters

*
* List of functions:

  int get_num_vdw_max ();

  gboolean are_identical_prop (int ti, int ai, field_prop * pro_a, field_prop * pro_b);
  gboolean tersoff_question ();
  gboolean body_identicals (field_nth_body * body, int nbd, int * na, int ** ma, int ** ba);

  gchar * get_this_vdw_string ();
  gchar * field_str (int a);
  gchar * body_str (int a);
  gchar * get_body_element_name (field_nth_body * body, int aid, int nbd);

  void adjust_field_prop (int fil, int sti, field_prop * tmp, int * ids, int key);
  void select_atom_set_color (GtkCellRenderer * renderer, int i);
  void select_atom_set_cmv (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void update_field_dist (float v);
  void adjust_vdw_interactions (gboolean add_shell);
  void edit_parameters (int f, int id);
  void update_tersoffs (int id, int key);
  void check_tersoffs (int id, int key);

  G_MODULE_EXPORT void update_atom_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void update_field_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void update_cross_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void changed_cross_combo (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void changed_field_key_combo (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void visualize_it (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void visualize_it (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void select_it (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void select_it (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void field_molecule_select_atom_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void edit_unit_weight (GtkCellRendererText * cell, gchar * path_string, gchar * new_text, gpointer data);
  G_MODULE_EXPORT void select_atom_id_from_fied_molecule (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void selection_button (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void changed_atom_combo (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void shell_in_vdw (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void shell_in_vdw (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void run_edit_parameters (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void edit_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void add_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void remove_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * combo_cross (field_nth_body * body);
  GtkWidget * parameters_box (int obj, int key,  gchar ** words, float * data);
  GtkWidget * param_prop_param_box (int pid);

*/

#include "global.h"
#include "interface.h"
#include "glwindow.h"
#include "glview.h"
#include "dlp_field.h"
#include "calc.h"

extern int * atoms_id;
extern int ** atoms_id_list;
extern char *** ff_atoms;

extern gchar * felemts[MAXDATA+1];
extern gchar * elemts[MAXDATA];
extern gchar * mo_title[8];

extern int fetypes[2][16];

extern void clean_up_molecules_info (gboolean usel);
extern void select_object (int id, int jd, int kd);
extern G_MODULE_EXPORT void run_add_atom_dialog (GtkDialog * add_dialog, gint response_id, gpointer data);
extern void field_selection (int i, int viz, int lab, int aid);
extern void field_unselect_all ();
extern void compare_non_bonded (gchar * fatom);
extern void visualize_single_struct (int id, int jd, int kd, int * ids);
extern void visualize_body (int viz, int bd, field_nth_body * body);
extern void init_default_shaders (glwin * view);
extern GtkWidget * create_field_prop_combo (int f, int is_moy);
extern void check_atom_for_updates ();
extern void check_to_visualize_properties_for_this_field_mol (int pid, int mol);
extern GtkWidget * ff_p_combo[2];

GtkWidget * field_key_combo;
GtkWidget * p_box;
GtkWidget * cross_vbox, * cross_hbox;
GtkWidget * param_box;
GtkWidget * img_but[4];
GtkWidget * shell_but;

GtkWidget * shell_hbox[3];
GtkWidget * shell_cbox[2];
GtkWidget * body_lab;
GtkWidget * afftype;
int is_moy;
int * edit_atids;

/*!
  \fn gboolean are_identical_prop (int ti, int ai, field_prop * pro_a, field_prop * pro_b)

  \brief are the 2 field property identicals ?

  \param ti the type of field property
  \param ai the number of field atoms for this field property
  \param pro_a 1st field property
  \param pro_b 2nd field property
*/
gboolean are_identical_prop (int ti, int ai, field_prop * pro_a, field_prop * pro_b)
{
  if (pro_a -> key != pro_b -> key) return FALSE;
  if (pro_a -> use != pro_b -> use) return FALSE;
  if (pro_a -> show != pro_b -> show) return FALSE;
  int i;
  /*for (i=0; i<ai; i++)
  {
    if (pro_a -> aid[i] != pro_b -> aid[i]) return FALSE;
  }*/
  for (i=0; i<fvalues[activef][ti][pro_a -> key]; i++)
  {
    if (pro_a -> val[i] != pro_b -> val[i]) return FALSE;
  }
  return TRUE;
}

/*!
  \fn void adjust_field_prop (int fil, int sti, field_prop * tmp, int * ids, int key)

  \brief adjust field property

  \param fil the type of field property
  \param sti the number of field atoms for this type of field property
  \param tmp the pointer of the field property list
  \param ids the list of field atoms
  \param key the key value to adjust
*/
void adjust_field_prop (int fil, int sti, field_prop * tmp, int * ids, int key)
{
  int i, j, k, l;
  gboolean add;
  add = FALSE;
  field_prop * pro, * ptmp;
  if (ids[0] < 0)
  {
    // Default prop
    if (tmp == NULL)
    {
      // New val for default field prop
      tmp_fstr -> def = init_field_prop (fil, key, tmp_fstr -> def -> show, tmp_fstr -> def -> use);
    }
    else
    {
      tmp_fstr -> def = duplicate_field_prop (tmp, fil);
    }
  }
  else
  {
    // Not default, between specified atoms
    if (tmp != NULL)
    {
      pro = get_active_prop_using_atoms (tmp_fstr -> other, sti, ids);
      if (pro == NULL)
      {
        // No 'other' yet, but is it identical to default ?
        if (! are_identical_prop (fil+1, sti, tmp, tmp_fstr -> def))
        {
          add = TRUE;
        }
      }
      else if (! are_identical_prop (fil+1, sti, tmp, pro))
      {
        add = TRUE;
      }
    }
    else
    {
      if (tmp_fstr -> other == NULL)
      {
        add = TRUE;
        j = key;
        k = tmp_fstr -> def -> show;
        l = tmp_fstr -> def -> use;
      }
      else
      {
        pro = get_active_prop_using_atoms (tmp_fstr -> other, sti, ids);
        if (pro == NULL)
        {
          if (key != tmp_fstr -> def -> key)
          {
            add = TRUE;
            j = key;
            k = tmp_fstr -> def -> show;
            l = tmp_fstr -> def -> use;
          }
        }
        else if (key != pro -> key)
        {
          add = TRUE;
          j = key;
        }
      }
    }

    if (add)
    {
      if (tmp_fstr -> other == NULL)
      {
        if (tmp != NULL)
        {
          tmp_fstr -> other = duplicate_field_prop (tmp, fil);
        }
        else
        {
          tmp_fstr -> other = init_field_prop (fil, j, k, l);
          for (i=0; i<sti; i++) tmp_fstr -> other -> aid[i] = ids[i];
        }
      }
      else if (pro == NULL)
      {
        pro = tmp_fstr -> other;
        while (pro -> next != NULL) pro = pro -> next;
        if (tmp != NULL)
        {
          pro -> next = duplicate_field_prop (tmp, fil);
        }
        else
        {
          pro -> next = init_field_prop (fil, j, k, l);
          pro = pro -> next;
          for (i=0; i<sti; i++) pro -> aid[i] = ids[i];
        }
      }
      else
      {
        if (tmp != NULL)
        {
          // Modifying an existing 'other' prop
          pro -> key = tmp -> key;
          pro -> val = NULL;
          if (fvalues[activef][fil+1][tmp -> key] > 0)
          {
            pro -> val = duplicate_float (fvalues[activef][fil+1][tmp -> key], tmp -> val);
          }
          pro -> show = tmp -> show;
          pro -> use = tmp -> use;
        }
        else
        {
          pro -> key = j;
          pro -> val = NULL;
          pro -> val = allocfloat (fvalues[activef][fil+1][j]);
        }
      }
    }
  }

  if (tmp_fstr -> other != NULL)
  {
    ptmp = tmp_fstr -> other;
    while (ptmp)
    {
      if (are_identical_prop (fil+1, sti, ptmp, tmp_fstr -> def))
      {
        if (ptmp -> next != NULL)
        {
          if (ptmp -> prev != NULL)
          {
            ptmp -> prev -> next = ptmp -> next;
            ptmp -> next -> prev = ptmp -> prev;
          }
          else
          {
            ptmp -> next -> prev = NULL;
          }
        }
        else if (ptmp -> prev != NULL)
        {
          ptmp -> prev -> next = NULL;
        }
        else
        {
          g_free (tmp_fstr -> other);
          tmp_fstr -> other = NULL;
          ptmp = NULL;
        }
      }
      if (ptmp) ptmp = ptmp -> next;
    }
  }
  if (tmp_fstr -> other != NULL)
  {
    ptmp = tmp_fstr -> other;
    ptmp -> pid = 0;
    while (ptmp -> next != NULL)
    {
      ptmp -> next -> pid = ptmp -> pid + 1;
      ptmp = ptmp -> next;
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void update_atom_parameter (GtkEntry * res, gpointer data)

  \brief update field atom parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_atom_parameter (GtkEntry * res, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  double v;
  const gchar * m = entry_get_text (res);
  if (i > -1)
  {
    v = string_to_double ((gpointer)m);
    update_entry_double (res, v);
  }
  switch (i)
  {
    case -1:
      tmp_fat -> name = g_strdup_printf ("%s", m);
      break;
    case 0:
      tmp_fat -> mass = v;
      break;
    case 1:
      tmp_fat -> charge = v;
      break;
    case 2:
      tmp_fshell -> m = v;
      break;
    case 3:
      tmp_fshell -> z = v;
      break;
    case 4:
      tmp_fshell -> k2 = v;
      break;
    case 5:
      tmp_fshell -> k4 = v;
      break;
    case 6:
      tmp_fcons -> length = v;
      break;
    case 7:
      tmp_fpmf -> length = v;
      break;
  }
}

int object_is;

/*!
  \fn G_MODULE_EXPORT void update_field_parameter (GtkEntry * res, gpointer data)

  \brief update field parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_field_parameter (GtkEntry * res, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  update_entry_double (res, v);
  if (object_is > 0 && object_is < 9)
  {
    tmp_fprop -> val[i] = v;
  }
  else
  {
    switch (object_is)
    {
      case 0:
        tmp_ftet -> val[i] = v;
        break;
      case 14:
        tmp_fext -> val[i] = v;
        break;
      default:
        tmp_fbody -> val[i] = v;
        break;
    }
  }
}

GtkWidget * centry[3];
GtkWidget * cross_box;
double *** cross = NULL;
int num_body_d;
gboolean change_tersoff;

/*!
  \fn G_MODULE_EXPORT void update_cross_parameter (GtkEntry * res, gpointer data)

  \brief update field cross parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cross_parameter (GtkEntry * res, gpointer data)
{
  int j, k;
  k = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  update_entry_double (res, v);
  j = combo_get_active (cross_box);
  cross[tmp_fbody -> id][j][k] = cross[j][tmp_fbody -> id][k] = v;
}

/*!
  \fn G_MODULE_EXPORT void changed_cross_combo (GtkComboBox * box, gpointer data)

  \brief change field cross combo

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_cross_combo (GtkComboBox * box, gpointer data)
{
  int i, j;
  i = combo_get_active ((GtkWidget *)box);
  for (j=0; j<3; j++)
  {
    // g_debug ("Updating entry:: id= %d, i= %d, j= %d", tmp_fbody -> id, i, j);
    update_entry_double (GTK_ENTRY(centry[j]), cross[tmp_fbody -> id][i][j]);
  }
}

/*!
  \fn GtkWidget * combo_cross (field_nth_body * body)

  \brief create field cross configuration widgets

  \param body the pointer on the first non bonded field structure
*/
GtkWidget * combo_cross (field_nth_body * body)
{
 GtkWidget * combo;
 combo = create_combo ();
 g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(changed_cross_combo), NULL);
 if (tmp_field -> first_body[2] && tmp_fbody -> na[0] > -1)
 {
   field_nth_body * obody;
   obody = tmp_field -> first_body[2];
   while (obody)
   {
     combo_text_append (combo, get_active_atom(obody -> ma[0][0], obody -> a[0][0]) -> name);
     obody = obody -> next;
   }
 }
 return combo;
}

/*!
  \fn gchar * get_this_vdw_string ()

  \brief get VdW formalism description string
*/
gchar * get_this_vdw_string ()
{
  gchar * str = NULL;
  if (tmp_field -> type <= CHARMMSI || tmp_field -> type > COMPASS)
  {
    str = g_strdup_printf ("<i>U</i>(r<sub>ij</sub>) = <b>Ɛ<sub>ij</sub></b> x [ (<b>r0<sub>ij</sub></b>/r<sub>ij</sub>)<sup>12</sup> - 2.0 (<b>r0<sub>ij</sub></b>/r<sub>ij</sub>)<sup>6</sup> ]\n"
                           "\t\twith <b>Ɛ<sub>ij</sub></b> = sqrt (Ɛ<sub>i</sub> x Ɛ<sub>j</sub>)\n"
                           "\t\tand <b>r0<sub>ij</sub></b> =  r0<sub>i</sub>/2.0 + r0<sub>j</sub>/2.0");
  }
  else if (tmp_field -> type == CVFF || tmp_field -> type == CVFF_AUG)
  {
    str = g_strdup_printf ("<i>U</i>(r<sub>ij</sub>) = (<b>A<sub>ij</sub></b>/r<sub>ij</sub>)<sup>12</sup> - 2.0 (<b>B<sub>ij</sub></b>/r<sub>ij</sub>)<sup>6</sup>\n"
                           "\t\twith <b>A<sub>ij</sub></b> = sqrt (A<sub>i</sub> x A<sub>j</sub>)\n"
                           "\t\tand <b>B<sub>ij</sub></b>  = sqrt (B<sub>i</sub> x B<sub>j</sub>)");
  }
  else if (tmp_field -> type >= CFF91 && tmp_field -> type <= COMPASS)
  {
    str = g_strdup_printf ("<i>U</i>(r<sub>ij</sub>) = <b>Ɛ<sub>ij</sub></b> x [ 2.0 (<b>r0<sub>ij</sub></b>/r<sub>ij</sub>)<sup>9</sup> - 3.0 (<b>r0<sub>ij</sub></b>/r<sub>ij</sub>)<sup>6</sup> ]\n"
                           "\t\twith <b>Ɛ<sub>ij</sub></b> = 2.0 sqrt (Ɛ<sub>i</sub> x Ɛ<sub>j</sub>) x (r0<sub>i</sub><sup>3</sup> r0<sub>j</sub><sup>3</sup>) / (r0<sub>i</sub><sup>6</sup> + r0<sub>j</sub><sup>6</sup>)\n"
                           "\t\tand <b>r0<sub>ij</sub></b> = [(r0<sub>i</sub><sup>6</sup> + r0<sub>j</sub><sup>6</sup>)/2.0] <sup>1/6</sup>");
  }
  return str;
}

/*!
  \fn GtkWidget * parameters_box (int obj, int key,  gchar ** words, float * data)

  \brief pepare field property edition parameters

  \param obj the type of field property
  \param key the key type for this field property, if any
  \param words the parameter(s) labels
  \param data the actual value(s) for the parameter(s)
*/
GtkWidget * parameters_box (int obj, int key,  gchar ** words, float * data)
{
  int i;
  gchar * str = NULL;
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * lab;
  GtkWidget * entry;
  object_is = obj;

  if (obj > -1 && key > -1)
  {
    if (fvalues[activef][obj][key] > 0)
    {
      for (i=0; i< fvalues[activef][obj][key]; i++)
      {
        if (obj == 11 && key == 0 && i == 0)
        {
          hbox = create_hbox (0);
          lab = markup_label ("<u><i>Single terms:</i></u>", 100, 50, 0.0, 0.5);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 30);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
        }
        hbox = create_hbox (0);
        str = g_strdup_printf (" <b><i>%s</i></b>", words[i]);
        lab = markup_label (str, 100, -1, 0.0, 0.5);
        g_free (str);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 50);
        entry = create_entry (G_CALLBACK(update_field_parameter), 100, 15, FALSE, GINT_TO_POINTER(i));
        update_entry_double (GTK_ENTRY(entry), data[i]);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
      }
      if (obj == 11 && key == 0)
      {
        cross_vbox = create_vbox (BSEP);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, cross_vbox, FALSE, FALSE, 0);
        cross_hbox = create_hbox (0);
        lab = markup_label ("<u><i>Cross terms with atom:</i></u>", 160, -1, 0.0, 0.5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cross_hbox, lab, FALSE, FALSE, 30);
        cross_box = combo_cross (tmp_fbody);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cross_hbox, cross_box, FALSE, FALSE, 0);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, cross_vbox, cross_hbox, FALSE, FALSE, 10);
        for (i=11; i< 14; i++)
        {
          hbox = create_hbox (0);
          str = g_strdup_printf (" <b><i>%s</i></b>", words[i]);
          lab = markup_label (str, 120, -1, 0.0, 0.5);
          g_free (str);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 50);
          centry[i-11] = create_entry (G_CALLBACK(update_cross_parameter), 100, 15, FALSE, GINT_TO_POINTER(i-11));
          update_entry_double (GTK_ENTRY(centry[i-11]), cross[tmp_fbody -> id][0][i-11]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, centry[i-11], FALSE, FALSE, 0);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, cross_vbox, hbox, FALSE, FALSE, 0);
        }
        if (tmp_fbody -> na[0] < 0)
        {
          widget_set_sensitive (cross_vbox, FALSE);
        }
        else
        {
          combo_set_active (cross_box, 0);
        }
      }
    }
    else
    {
      hbox = create_hbox (0);
      str = g_strdup_printf ("<i>Tabulated</i>");
      lab = markup_label (str, 100, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 30);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    }
    if (obj == 9 && tmp_field -> type >= CFF91 && tmp_field -> type <= COMPASS)
    {
      str = g_strdup_printf ("In %s, non-bonded interactions are evaluated using: \n%s\nTherefore the parameters provided by the force field are incompatible with the DL-POLY options.", field_acro[tmp_field -> type], get_this_vdw_string());
      hbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, -1, -1, 0.5, 0.5), FALSE, FALSE, 50);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
      g_free (str);
    }
    else if (obj == 9 && key == 0 && (tmp_field -> type <= CVFF_AUG || tmp_field -> type > COMPASS))
    {
      if (tmp_field -> type < CVFF)
      {
        str = g_strdup_printf ("In %s, 12-6 non-bonded interactions are evaluated using: \n%s\n<i><b>A</b></i> and <i><b>B</b></i>"
                               " are calculated using Ɛ<sub>i/j</sub> and r0<sub>i/j</sub> provided by the force field parameters.",
                               field_acro[tmp_field -> type], get_this_vdw_string());
        if (tmp_field -> type > AMBER99)
        {
          str = g_strdup_printf ("%s\nScaled 1-4 exclusion parameters, provided by the %s force field, are ignored.", str, field_acro[tmp_field -> type]);
        }
      }
      else
      {
        str = g_strdup_printf ("In %s, 12-6 non-bonded interactions are evaluated using: \n%s\n<i><b>A</b></i> and <i><b>B</b></i>"
                               " are calculated using A<sub>i/j</sub> and B<sub>i/j</sub> provided by the force field parameters.",
                               field_acro[tmp_field -> type], get_this_vdw_string());
      }
      hbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, -1, -1, 0.5, 0.5), FALSE, FALSE, 50);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
      g_free (str);
    }
    else if ((obj == 3 || obj == 4) && tmp_field -> type > AMBER99 && tmp_field -> type < CVFF)
    {
      hbox = create_hbox (0);
      str = g_strdup_printf ("Urey-Bradley terms provided by the %s force field are ignored.\n", field_acro[tmp_field -> type]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, -1, -1, 0.5, 0.5), FALSE, FALSE, 50);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
      g_free (str);
    }
  }
  return vbox;
}

/*!
  \fn gchar * field_str (int a)

  \brief get field external name

  \param a the field external id
*/
gchar * field_str (int a)
{
  gchar * str = g_strdup_printf ("External field");
  if (a > -1)
  {
    str = g_strdup_printf ("%s: <b>%s</b>", str, fnames[activef][15][a]);
  }
  return str;
}

/*!
  \fn gboolean tersoff_question ()

  \brief change Tersoff potential ?
*/
gboolean tersoff_question ()
{
  gchar * text = "Are you sure to want to change the type of Tersoff potential ?\n"
                 "<i>Tersoff (ter)</i> and <i>KIHS (kihs)</i> forms cannot be mixed\n"
                 "Therefore this choice will affect <b>all</b> Tersoff potential(s)"
                 "Any previous parameters will be erased !";
  if (num_body_d > 1)
  {
    return ask_yes_no ("Change Tersoff potential ?!", text, GTK_MESSAGE_QUESTION, field_assistant);
  }
  else
  {
    change_tersoff = TRUE;
    return TRUE;
  }
}

/*!
  \fn GtkWidget * param_prop_param_box (int pid)

  \brief prepare field parameter edition widgets

  \param pid the type of field parameter
*/
GtkWidget * param_prop_param_box (int pid)
{
  switch (pid)
  {
    case MOLIMIT-9:
      return parameters_box (0, tmp_ftet -> key, fvars_teth[activef][tmp_ftet -> key], tmp_ftet -> val);
      break;
    case MOLIMIT-1:
      return parameters_box (pid-6, tmp_fprop -> key, fvars_inversion[activef][tmp_fprop -> key], tmp_fprop -> val);
      break;
    case MOLIMIT:
      return parameters_box (pid-6, tmp_fbody -> key, fvars_vdw[activef][tmp_fbody -> key], tmp_fbody -> val);
      break;
    case MOLIMIT+1:
      return parameters_box (pid-6, tmp_fbody -> key, fvars_met[activef][tmp_fbody -> key], tmp_fbody -> val);
      break;
    case MOLIMIT+2:
      return parameters_box (pid-6, tmp_fbody -> key, fvars_ters[activef][tmp_fbody -> key], tmp_fbody -> val);
      break;
    case MOLIMIT+3:
      return parameters_box (pid-6, tmp_fbody -> key, fvars_tbd[activef][tmp_fbody -> key], tmp_fbody -> val);
      break;
    case MOLIMIT+4:
      return parameters_box (pid-6, tmp_fbody -> key, fvars_fbd[activef][tmp_fbody -> key], tmp_fbody -> val);
      break;
    case SEXTERN:
       return parameters_box (14, tmp_fext -> key, fvars_fext[activef][tmp_fext -> key], tmp_fext -> val);
       break;
    default:
      switch ((pid-7)/2)
      {
        case 0:
          return parameters_box (pid-6, tmp_fprop -> key, fvars_bond[activef][tmp_fprop -> key], tmp_fprop -> val);
          break;
        case 1:
          return parameters_box (pid-6, tmp_fprop -> key, fvars_angle[activef][tmp_fprop -> key], tmp_fprop -> val);
          break;
        default:
          return parameters_box (pid-6, tmp_fprop -> key, fvars_dihedral[activef][tmp_fprop -> key], tmp_fprop -> val);
          break;
      }
      break;
  }
}

/*!
  \fn G_MODULE_EXPORT void changed_field_key_combo (GtkComboBox * box, gpointer data)

  \brief change field key

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_field_key_combo (GtkComboBox * box, gpointer data)
{
  int i, j;
  gboolean changeit = FALSE;
  i = GPOINTER_TO_INT(data);
  j = combo_get_active ((GtkWidget *)box);
  if (i > 6 && i < MOLIMIT)
  {
    if (j != tmp_fprop -> key)
    {
      tmp_fprop -> key = j;
      tmp_fprop -> val = NULL;
      tmp_fprop -> val = allocfloat (fvalues[activef][i-6][j]);
      changeit = TRUE;
    }
  }
  else
  {
    switch (i)
    {
      case 6:
        if (j != tmp_ftet -> key)
        {
          tmp_ftet -> key = j;
          tmp_ftet -> val = NULL;
          tmp_ftet -> val = allocfloat (fvalues[activef][0][j]);
          changeit = TRUE;
        }
        break;
      case SEXTERN:
        if (j != tmp_fext -> key)
        {
          tmp_fext -> key = j;
          gtk_label_set_text (GTK_LABEL(body_lab), field_str(tmp_fext -> key));
          gtk_label_set_use_markup (GTK_LABEL(body_lab), TRUE);
          tmp_fext -> val = NULL;
          tmp_fext -> val = allocfloat (fvalues[activef][SEXTERN-6][j]);
          changeit = TRUE;
        }
        break;
      default:
        if (j != tmp_fbody -> key)
        {
          changeit = TRUE;
          if (i == MOLIMIT+2) changeit = tersoff_question ();
          if (changeit)
          {
            tmp_fbody -> key = j;
            tmp_fbody -> val = NULL;
            tmp_fbody -> val = allocfloat (fvalues[activef][i-6][j]);
            if (i == MOLIMIT+2 && j == 0)
            {
              cross = alloctdouble (num_body_d, num_body_d, 3);
            }
            else if (j == 1 && cross != NULL)
            {
              g_free (cross);
              cross = NULL;
            }
          }
          else
          {
            combo_set_active ((GtkWidget *)box, tmp_fbody -> key);
          }
        }
        break;
    }
  }
  if (changeit)
  {
    if (ff_p_combo[0]) combo_set_active (ff_p_combo[0], 0);
    if (ff_p_combo[1]) combo_set_active (ff_p_combo[1], 0);
    p_box = destroy_this_widget (p_box);
    p_box = param_prop_param_box (i);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, param_box, p_box, FALSE, FALSE, 0);
    show_the_widgets (param_box);
  }

}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void visualize_it (GtkCheckButton * but, gpointer data)

  \brief  visualize object toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void visualize_it (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void visualize_it (GtkToggleButton * but, gpointer data)

  \brief visualize object toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void visualize_it (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j;
  i = GPOINTER_TO_INT (data);
  j = 0;
  toviz.c = button_get_status ((GtkWidget *)but);
  if (i < MOLIMIT)
  {
    j = combo_get_active (combo_mol[i-1]);
    tmp_fmol = get_active_field_molecule (j);
  }
  if (is_moy)
  {
    visualize_object (i, row_id, j);
    if (i < MOLIMIT) check_to_visualize_properties_for_this_field_mol (i, j);
  }
  else
  {
    visualize_single_struct (i, row_id, j, edit_atids);
    if (i < MOLIMIT)
    {
      check_to_visualize_properties_for_this_field_mol (i, j);
      tmp_fprop -> show = toviz.c;
    }
  }
  if (i < MOLIMIT) tmp_fmol = get_active_field_molecule (j);
  init_default_shaders (tmp_view);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void select_it (GtkCheckButton * but, gpointer data)

  \brief select object toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_it (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void select_it (GtkToggleButton * but, gpointer data)

  \brief select object toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_it (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j;
  i = GPOINTER_TO_INT (data);
  toviz.c = button_get_status ((GtkWidget *)but);
  if (i < MOLIMIT) j = combo_get_active (combo_mol[i-1]);
  if (is_moy)
  {
    select_object (i, row_id, j);
  }
  else
  {
    tmp_fprop -> use = toviz.c;
  }
}

extern ColRGBA init_color (int id, int numid);
extern int active_sel;
extern int a_ato;
extern int ** sel_at;
extern void run_select_atom_dialog (GtkDialog * select_dialog, gint response_id, gpointer data);
GtkWidget * add_tree;
int vdw_id;

/*!
  \fn G_MODULE_EXPORT void field_molecule_select_atom_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select atom in field molecule toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*/
G_MODULE_EXPORT void field_molecule_select_atom_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  GtkTreeStore ** model = (GtkTreeStore **)data;
  GtkTreeIter iter;
  int h, i, j, k, l, m, n;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(* model), & iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(* model), & iter, 0, & i, -1);
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    toviz.c = 0;
    a_ato --;
  }
  else
  {
    toviz.c = 1;
    a_ato ++;
  }
  if (active_sel < 11)
  {
    sel_at[0][i-1] = toviz.c;
  }
  switch (active_sel)
  {
    case 0:
      j = i-1;
      break;
    case 1:
      j = i-1;
      break;
    case 2:
      j = tmp_fshell -> id;
      break;
    case 3:
      j = tmp_fshell -> id;
      break;
    case 4:
      j = tmp_fcons -> id;
      break;
    case 5:
      j = tmp_fcons -> id;
      break;
    case 6:
      j = tmp_fpmf -> id;
      break;
    case 7:
      j = tmp_fpmf -> id;
      break;
    case 8:
      j = tmp_frig -> id;
      break;
    case 9:
      j = tmp_ftet -> id;
      break;
    default:
      j = tmp_fbody -> id;
      break;
  }
  if (active_sel < 10)
  {
    for (k=0; k<tmp_fmol -> multi; k++)
    {
      l = tmp_fmol -> atoms_id[i-1][k].a;
      m = tmp_fmol -> atoms_id[i-1][k].b;
      n = get_active_atom (tmp_fmol -> id, l) -> list[m];
      field_selection (n, toviz.c, toviz.c, i-1);
    }
  }
  else
  {
    h = vdw_id = sel_at[0][i-1];
    field_nth_body * tmp_fbo = tmp_field -> first_body[0];
    for (k=0; k<tmp_field -> nbody[0]; k++)
    {
      if (tmp_fbo -> id == h)
      {
        visualize_body (toviz.c, h, tmp_fbo);
        break;
      }
      else if (tmp_fbo -> next != NULL)
      {
        tmp_fbo = tmp_fbo -> next;
      }
    }
  }
  init_default_shaders (tmp_view);
  j = (active_sel > 5 && active_sel < 8) ? 1 : 0;
  gtk_tree_store_set (* model, & iter, 3+j, toviz.c, -1);
  hide_the_widgets (add_tree);
  show_the_widgets (add_tree);
}

/*!
  \fn void select_atom_set_color (GtkCellRenderer * renderer, int i)

  \brief set cell renderer color

  \param renderer the target GtkCellRen
  \param i the color id
*/
void select_atom_set_color (GtkCellRenderer * renderer, int i)
{
  set_renderer_color (i, renderer, init_color (i-1, num_field_objects));
}

/*!
  \fn void select_atom_set_cmv (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief field atom set renderer color, markup and visibility in the property edition atom(s) selection tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void select_atom_set_cmv (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int h, i, j, k;
  gchar * str = NULL;
  gtk_tree_model_get (mod, iter, 0, & h, -1);
  if (active_sel < 11)
  {
    gtk_tree_model_get (mod, iter, 1, & i, -1);
  }
  else
  {
    i = ! h;
  }
  j = GPOINTER_TO_INT(data);
  switch (j)
  {
    case 0:
      gtk_cell_renderer_set_visible (renderer, ! i);
      break;
    case 1:
      if (active_sel > 10)
      {
        gtk_tree_model_get (mod, iter, 1, & str, -1);
        g_object_set (renderer, "markup", str, NULL, NULL);
        g_free (str);
        i = h;
      }
      gtk_cell_renderer_set_visible (renderer, i);
      break;
    case 2:
      gtk_tree_model_get (mod, iter, 2, & str, -1);
      g_object_set (renderer, "markup", str, NULL, NULL);
      g_free (str);
      gtk_cell_renderer_set_visible (renderer, i);
      break;
    case 3:
      gtk_cell_renderer_set_visible (renderer, ! i);
      break;
    case 4:
      if ((active_sel > 1 && active_sel < 6) || active_sel > 8)
      {
        if (active_sel > 10)
        {
          gtk_tree_model_get (mod, iter, 3, & k, -1);
        }
        else
        {
          k = sel_at[0][h-1];
        }
        if (k || ! a_ato)
        {
          gtk_cell_renderer_set_visible (renderer, TRUE);
        }
        else
        {
          gtk_cell_renderer_set_visible (renderer, ! a_ato);
        }
      }
      else
      {
        gtk_cell_renderer_set_visible (renderer, ! i);
      }
      break;
  }
  if (active_sel < 2 && j < 4)
  {
    for (i=0; i<tmp_fat -> num/tmp_fmol -> multi; i++)
    {
      if (sel_at[0][i]+1 == abs(h)) break;
    }
    if (sel_at[1][i])
    {
      select_atom_set_color (renderer, sel_at[0][i]+1);
    }
    else
    {
      select_atom_set_color (renderer, sel_at[1][i]);
    }
  }
  else if (j < 4)
  {
    if (h < 0) h = -h;
    if (active_sel < 11)
    {
      k = (sel_at[0][h-1]) ? h : 0;
    }
    else
    {
      gtk_tree_model_get (mod, iter, 3, & k, -1);
    }
    select_atom_set_color (renderer, k);
  }
}

float * val_at;

/*
* G_MODULE_EXPORT void edit_unit_weight (GtkCellRendererText * cell, gchar * path_string, gchar * new_text, gpointer data)

  \brief on edit unit weight callback

  \param cell the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
*  gchar
  \param data the associated data pointer
*/
G_MODULE_EXPORT void edit_unit_weight (GtkCellRendererText * cell, gchar * path_string, gchar * new_text, gpointer data)
{
  GtkTreeStore ** model = (GtkTreeStore **)data;
  GtkTreeIter iter;
  int i;
  GtkTreePath * path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(* model), & iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(* model), & iter, 0, & i, -1);
  val_at[i] = string_to_double ((gpointer)new_text);
}

/*!
  \fn G_MODULE_EXPORT void select_atom_id_from_fied_molecule (GtkButton * but, gpointer data)

  \brief select atom id from field molecule - creating the dialog

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_atom_id_from_fied_molecule (GtkButton * but, gpointer data)
{
  int i, j, k, l, m, n, o, p, q;
  GtkTreeIter id_level;
  GtkTreeIter atom_level;
  GtkTreeViewColumn * ato_col[5];
  GtkCellRenderer * ato_cell[5];
  gchar * ato_title[5] = {"Atom Id", "Fragment", "Atom", "Weight (1)", "Viz.3D & Select"};
  gchar * nbd_title[5] = {"Atom Id", "Field atom", "Field molecule(s)", " ", "Viz.3D & Select"};
  gchar * ctype[5]={"text", "text", "text", "text", "active"};
  GType col_type[2][5] = {{G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_FLOAT},
                          {G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_BOOLEAN}};
  GType cbl_type[4] = {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN};
  field_object = 1;
  active_sel = GPOINTER_TO_INT (data);
  gboolean add;
  gchar * str;
  q = (active_sel > 5 && active_sel < 8) ? 1 : 0;
  switch (active_sel)
  {
    case 0:
      str = g_strdup_printf ("Please select the new atom(s)");
      o = -1;
      p = -1;
      break;
    case 1:
      str = g_strdup_printf ("Please select the atom(s) to freeze / unfreeze");
      o = -1;
      p = -1;
      break;
    case 2:
      str = g_strdup_printf ("Please select the core atom");
      o = tmp_fshell -> ia[0]-1;
      p = tmp_fshell -> ia[1]-1;
      break;
    case 3:
      str = g_strdup_printf ("Please select the atom(s) to shell");
      o = tmp_fshell -> ia[1]-1;
      p = tmp_fshell -> ia[0]-1;
      break;
    case 4:
      str = g_strdup_printf ("Please select the first atom");
      o = tmp_fcons -> ia[0]-1;
      p = tmp_fcons -> ia[1]-1;
      break;
    case 5:
      str = g_strdup_printf ("Please select the second atom");
      o = tmp_fcons -> ia[1]-1;
      p = tmp_fcons -> ia[0]-1;
      break;
    case 6:
      str = g_strdup_printf ("Please select the atom(s) in the first unit");
      break;
    case 7:
      str = g_strdup_printf ("Please select the atom(s) in the second unit");
      break;
    case 8:
      str = g_strdup_printf ("Please select the atom(s) in the rigid unit");
      p = -1;
      break;
    case 9:
      str = g_strdup_printf ("Please select the tethered atom");
      o = tmp_ftet -> num-1;
      p = -1;
      break;
    case 11:
      if (tmp_fbody -> bd == 2)
      {
        str = g_strdup_printf ("Please select the type of field atom");
      }
      else
      {
        str = g_strdup_printf ("Please select the first type of field atom");
      }
      break;
    case 12:
      str = g_strdup_printf ("Please select the second type of field atom");
      break;
    case 13:
      str = g_strdup_printf ("Please select the third type of field atom");
      break;
    case 14:
      str = g_strdup_printf ("Please select the fourth type of field atom");
      break;
  }
  GtkWidget * amol = dialogmodal (str, GTK_WINDOW(field_assistant));
  g_free (str);
  gtk_dialog_add_button (GTK_DIALOG(amol), "Apply", GTK_RESPONSE_APPLY);
  a_ato = 0;
  GtkTreeStore * add_model;
  if (active_sel < 11)
  {
    add_model = gtk_tree_store_newv (4+q, col_type[q]);
  }
  else
  {
    add_model = gtk_tree_store_newv (4, cbl_type);
  }
  add_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(add_model));
  if (active_sel < 11)
  {
    num_field_objects = tmp_fmol -> mol -> natoms;
  }

  for (i=0; i<4+q; i++)
  {
    k = i;
    if (i < 3+q)
    {
      ato_cell[i] = gtk_cell_renderer_text_new ();
    }
    else
    {
      if (! q) k ++;
      ato_cell[i] = gtk_cell_renderer_toggle_new ();
      if (active_sel > 1) gtk_cell_renderer_toggle_set_radio (GTK_CELL_RENDERER_TOGGLE(ato_cell[i]), TRUE);
      g_signal_connect (G_OBJECT(ato_cell[i]), "toggled", G_CALLBACK(field_molecule_select_atom_id), & add_model);
    }
    if (active_sel < 11)
    {
      ato_col[i] = gtk_tree_view_column_new_with_attributes (ato_title[k], ato_cell[i], ctype[k], i, NULL);
    }
    else
    {
      ato_col[i] = gtk_tree_view_column_new_with_attributes (nbd_title[k], ato_cell[i], ctype[k], i, NULL);
    }
    gtk_tree_view_append_column (GTK_TREE_VIEW(add_tree), ato_col[i]);
    gtk_tree_view_column_set_alignment (ato_col[i], 0.5);
    gtk_tree_view_column_set_cell_data_func (ato_col[i], ato_cell[i], select_atom_set_cmv, GINT_TO_POINTER(k), NULL);
    if (q && i == 3)
    {
      g_object_set (ato_cell[i], "editable", TRUE, NULL);
      g_signal_connect (G_OBJECT(ato_cell[i]), "edited", G_CALLBACK(edit_unit_weight), & add_model);
    }
  }
  // Clean 3D viz
  field_unselect_all ();
  // fill model
  if (active_sel < 2)
  {
    sel_at = allocdint(2, tmp_fat -> num/tmp_fmol -> multi);
  }
  else if (active_sel < 11)
  {
    sel_at = allocdint(2, tmp_fmol -> mol -> natoms);
  }
  else
  {
    field_nth_body * tmp_fbo = tmp_field -> first_body[0];
    k = 0;
    for (i=0; i<tmp_field -> nbody[0]; i++)
    {
      if (g_strcmp0 (get_active_atom (tmp_fbo -> ma[0][0], tmp_fbo -> a[0][0]) -> name,
                     get_active_atom (tmp_fbo -> ma[1][0], tmp_fbo -> a[1][0]) -> name) == 0) k ++;
      if (tmp_fbo -> next != NULL) tmp_fbo = tmp_fbo -> next;
    }
    sel_at = allocdint(1, k);
    num_field_objects = k;
  }

  if (active_sel < 2)
  {
    l = 0;
    for (i=0; i<tmp_fmol -> mol -> natoms; i++)
    {
      if (tmp_fmol -> atoms_id[i][0].a == tmp_fat -> id)
      {
        j = tmp_fmol -> atoms_id[i][0].b;
        k = tmp_fat -> list_id[j];
        sel_at[0][l] = k;
        if (active_sel)
        {
          sel_at[1][l] = tmp_fat -> frozen_id[j];
        }
        else
        {
          sel_at[1][l] = 0;
        }
        l ++;
      }
    }
    for (i=0; i<tmp_fat -> num/tmp_fmol -> multi; i++)
    {
      gtk_tree_store_append (add_model, & id_level, NULL);
      gtk_tree_store_set (add_model, & id_level, 0, sel_at[0][i]+1, 1, 0, 3, 0, -1);
      if (sel_at[1][i])
      {
        field_molecule_select_atom_id (GTK_CELL_RENDERER_TOGGLE(ato_cell[3]),
                                       gtk_tree_path_to_string(gtk_tree_model_get_path(GTK_TREE_MODEL(add_model), & id_level)),
                                       & add_model);
      }
      for (j=0; j< tmp_fmol -> multi; j++)
      {
        gtk_tree_store_append (add_model, & atom_level, & id_level);
        k = sel_at[0][i];
        l = tmp_fmol -> atoms_id[k][j].b;
        m = tmp_fat -> list[l];
        str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(tmp_proj -> chemistry -> label[tmp_fat -> sp]), m+1);
        gtk_tree_store_set (add_model, & atom_level, 0, -(sel_at[0][i]+1), 1, tmp_fmol -> fragments[j]+1, 2, str, -1);
        g_free (str);
      }
    }
  }
  else if (active_sel < 4)
  {
    for (i=0; i<tmp_fmol -> mol -> natoms; i++)
    {
      if (i != p)
      {
        gtk_tree_store_append (add_model, & id_level, NULL);
        gtk_tree_store_set (add_model, & id_level, 0, i+1, 1, 0, 3, 0, -1);
        tmp_fat =  get_active_atom (tmp_fmol -> id, tmp_fmol -> atoms_id[i][0].a);
        for (j=0; j< tmp_fmol -> multi; j++)
        {
          gtk_tree_store_append (add_model, & atom_level, & id_level);
          k = tmp_fat -> list[tmp_fmol -> atoms_id[i][j].b];
          str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(tmp_proj -> chemistry -> label[tmp_fat -> sp]), k+1);
          gtk_tree_store_set (add_model, & atom_level, 0, -(i+1), 1, tmp_fmol -> fragments[j]+1, 2, str, -1);
          g_free (str);
        }
      }
    }
  }
  else if (active_sel < 6 || (active_sel > 7 && active_sel < 11))
  {
    for (i=0; i<tmp_fmol -> mol -> natoms; i++)
    {
      if (i != p)
      {
        tmp_fat = get_active_atom (tmp_fmol -> id, tmp_fmol -> atoms_id[i][0].a);
        gtk_tree_store_append (add_model, & id_level, NULL);
        if (active_sel < 6 || active_sel == 9)
        {
          if (i == o)
          {
            k = 1;
          }
          else
          {
            k = 0;
          }
        }
        else
        {
          k = 0;
          if (tmp_frig -> num > 0)
          {
            for (j=0; j<tmp_frig -> num; j++)
            {
              if (i == tmp_frig -> list[j])
              {
                k = 1;
                break;
              }
            }
          }
        }
        sel_at[0][i] = k;
        gtk_tree_store_set (add_model, & id_level, 0, i+1, 1, 0, 3, 0, -1);
        if (sel_at[0][i])
        {
          field_molecule_select_atom_id (GTK_CELL_RENDERER_TOGGLE(ato_cell[3]),
                                         gtk_tree_path_to_string(gtk_tree_model_get_path(GTK_TREE_MODEL(add_model), & id_level)),
                                         & add_model);
        }
        for (j=0; j< tmp_fmol -> multi; j++)
        {
          gtk_tree_store_append (add_model, & atom_level, & id_level);
          l = tmp_fat -> list[tmp_fmol -> atoms_id[i][j].b];
          str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(tmp_proj -> chemistry -> label[tmp_fat -> sp]), l+1);
          gtk_tree_store_set (add_model, & atom_level, 0, -(i+1), 1, tmp_fmol -> fragments[j]+1, 2, str, -1);
          g_free (str);
        }
      }
    }
  }
  else if (active_sel > 10)
  {
    m = active_sel-11;
    field_nth_body * tmp_fbo = tmp_field -> first_body[0];
    k = 0;
    for (i=0; i<tmp_field -> nbody[0]; i++)
    {
      // need to find the atoms name
      if (g_strcmp0 (get_active_atom (tmp_fbo -> ma[0][0], tmp_fbo -> a[0][0]) -> name,
                 get_active_atom (tmp_fbo -> ma[1][0], tmp_fbo -> a[1][0]) -> name) == 0)
      {
        add = TRUE;
        for (j=0; j < body_at(tmp_fbody -> bd); j++)
        {
          if (j != m && tmp_fbody -> na[j] > -1)
          {
            if (g_strcmp0 (get_active_atom (tmp_fbo -> ma[0][0], tmp_fbo -> a[0][0]) -> name,
                       get_active_atom (tmp_fbody -> ma[j][0], tmp_fbody -> a[j][0]) -> name) == 0) add = FALSE;
          }
        }
        if (add)
        {
          gtk_tree_store_append (add_model, & id_level, NULL);
          l = 0;
          if (tmp_fbody -> na[m] > -1)
          {
            if (g_strcmp0 (get_active_atom (tmp_fbo -> ma[0][0], tmp_fbo -> a[0][0]) -> name,
                       get_active_atom (tmp_fbody -> ma[m][0], tmp_fbody -> a[m][0]) -> name) == 0)
            {
              l = 1;
              vdw_id = tmp_fbo -> id;
              a_ato = 1;
            }
          }
          gtk_tree_store_set (add_model, & id_level, 0, k+1, 1,
                              exact_name(get_active_atom (tmp_fbo -> ma[0][0], tmp_fbo -> a[0][0]) -> name), 3, l, -1);
          sel_at[0][k] = tmp_fbo -> id;
          for (j=0; j<tmp_fbo -> na[0]; j++)
          {
            gtk_tree_store_append (add_model, & atom_level, & id_level);
            gtk_tree_store_set (add_model, & atom_level, 0, 0, 2, get_active_field_molecule(tmp_fbo -> ma[0][j]) -> name, 3, 0, -1);
          }
        }
        k ++;
      }
      if (tmp_fbo -> next != NULL) tmp_fbo = tmp_fbo -> next;
    }
  }
  else
  {
    val_at = allocfloat (tmp_fmol -> mol -> natoms);
    if (active_sel == 5)
    {
      k = 1;
      m = 0;
    }
    else
    {
      k = 0;
      m = 1;
    }
    for (i=0; i<tmp_fmol -> mol -> natoms; i++)
    {
      add = TRUE;
      if (tmp_fpmf -> num[k] > 0)
      {
        for (l=0; l<tmp_fpmf -> num[k]; l++)
        {
          if (i == tmp_fpmf -> list[k][l])
          {
            add = FALSE;
            break;
          }
        }
      }
      if (add)
      {
        tmp_fat = get_active_atom (tmp_fmol -> id, tmp_fmol -> atoms_id[i][0].a);
        gtk_tree_store_append (add_model, & id_level, NULL);
        n = 0;
        if (tmp_fpmf -> num[m] > 0)
        {
          for (l=0; l<tmp_fpmf -> num[m]; l++)
          {
            if (i == tmp_fpmf -> list[m][l])
            {
              n = 1;
              break;
            }
          }
        }
        val_at[i] = tmp_fat -> mass;
        sel_at[0][i] = n;
        gtk_tree_store_set (add_model, & id_level, 0, i+1, 1, 0, 3, val_at[i], 4, 0, -1);
        if (sel_at[0][i])
        {
          field_molecule_select_atom_id (GTK_CELL_RENDERER_TOGGLE(ato_cell[4]),
                                         gtk_tree_path_to_string(gtk_tree_model_get_path(GTK_TREE_MODEL(add_model), & id_level)),
                                         & add_model);
        }
        for (l=0; l< tmp_fmol -> multi; l++)
        {
          gtk_tree_store_append (add_model, & atom_level, & id_level);
          n = tmp_fat -> list[tmp_fmol -> atoms_id[i][l].b];
          str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(tmp_proj -> chemistry -> label[tmp_fat -> sp]), n+1);
          gtk_tree_store_set (add_model, & atom_level, 0, -(i+1), 1, tmp_fmol -> fragments[l]+1, 2, str, -1);
          g_free (str);
        }
      }
    }
  }
  g_object_unref (add_model);

  if (active_sel < 2)
  {
    j = tmp_fat -> num/tmp_fmol -> multi;
  }
  else
  {
    j = num_field_objects;
  }
  i = ((j+1)*37 < 500) ? (j+1)*37 : 500;
  if (active_sel > 10) q = 1;
  GtkWidget * vbox = dialog_get_content_area (amol);
  GtkWidget * scrollsets = create_scroll (vbox, 320+q*100, i, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrollsets, add_tree);

  if (q && active_sel < 11)
  {
    str = g_strdup_printf (" <b>(1)</b> if all 0.0 then atomic weight(s) will be used by DL-POLY");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
  }
  if (active_sel < 2)
  {
    str = g_strdup_printf ("The atom(s) above will be described\n"
                           "in the force field using the parameters\n"
                           "of the <b>%s</b> field atom.\n", tmp_fat -> name);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
    g_free (str);
  }
  show_the_widgets (amol);
  switch (active_sel)
  {
    case 0:
      run_this_gtk_dialog (amol, G_CALLBACK(run_add_atom_dialog), NULL);
      break;
    default:
      run_this_gtk_dialog (amol, G_CALLBACK(run_select_atom_dialog), GINT_TO_POINTER(active_sel));
      if (active_sel == 1)
      {
        str = g_strdup_printf ("%d atom(s)", tmp_fat -> frozen);
        if (but) gtk_button_set_label (but, str);
      }
      break;
  }
  g_free (sel_at);
  if (q && active_sel < 11) g_free (val_at);
}

GtkWidget * av_lgt;

/*!
  \fn void update_field_dist (float v)

  \brief update field distance widget

  \param v the new value
*/
void update_field_dist (float v)
{
  gchar * str;
  if (v < 0.0)
  {
    str = g_strdup_printf (" ");
  }
  else
  {
    str = g_strdup_printf ("<b>%8.3f</b>", v);
  }
  gtk_label_set_text (GTK_LABEL(av_lgt), str);
  gtk_label_set_use_markup (GTK_LABEL(av_lgt), TRUE);
}

/*!
  \fn gchar * body_str (int a)

  \brief get body potential string name

  \param a the type of potential
*/
gchar * body_str (int a)
{
  int i, j;
  j = body_at (a);
  gchar * str = g_strdup_printf ("%s <b>", felemts[a+MOLIMIT+1]);
  for (i=0; i<j; i++)
  {
    if (tmp_fbody -> na[i] > -1)
    {
      str = g_strdup_printf ("%s%s", str, get_active_atom (tmp_fbody -> ma[i][0], tmp_fbody -> a[i][0]) -> name);
    }
    if (i < j-1 && j > 1) str = g_strdup_printf ("%s - ", str);
  }
  str = g_strdup_printf ("%s</b>", str);
  return str;
}

/*!
  \fn G_MODULE_EXPORT void selection_button (GtkButton * but, gpointer data)

  \brief select field object callback

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void selection_button (GtkButton * but, gpointer data)
{
  select_atom_id_from_fied_molecule (NULL, data);
  int i, j, k, l, m, n, o, p, q, r;
  i = GPOINTER_TO_INT(data);
  gchar * stra = NULL;
  gchar * strb = NULL;
  if (i == 2 || i == 3)
  {
    j=i-2;
    if (! tmp_fshell -> ia[j])
    {
      stra = g_strdup_printf ("Not picked yet !");
      strb = g_strdup_printf (DELETEB);
    }
    else
    {
      k = tmp_fshell -> ia[j]-1;
      l = tmp_fmol -> atoms_id[k][0].a;
      stra = g_strdup_printf ("%s - %d", exact_name(tmp_proj -> chemistry -> label[get_active_atom (tmp_fmol -> id, l) -> sp]), tmp_fshell -> ia[j]);
      strb = g_strdup_printf (APPLY);
    }
  }
  else if (i == 4 || i == 5)
  {
    j=i-4;
    if (! tmp_fcons -> ia[j])
    {
      stra = g_strdup_printf ("Not picked yet !");
      strb = g_strdup_printf (DELETEB);
    }
    else
    {
      k = tmp_fcons -> ia[j] - 1;
      l = tmp_fmol -> atoms_id[k][0].a;
      stra = g_strdup_printf ("%s - %d", exact_name(tmp_proj -> chemistry -> label[get_active_atom (tmp_fmol -> id, l) -> sp]), tmp_fcons -> ia[j]);
      strb = g_strdup_printf (APPLY);
    }
  }
  else if (i == 6 || i == 7)
  {
    j=i-6;
    if (! tmp_fpmf -> num[j])
    {
      stra = g_strdup_printf ("Not picked yet !");
      strb = g_strdup_printf (DELETEB);
    }
    else
    {
      stra = g_strdup_printf ("%d atom(s)", tmp_fpmf -> num[j]);
      strb = g_strdup_printf (APPLY);
    }
  }
  else if (i == 8)
  {
    j=0;
    if (! tmp_frig -> num)
    {
      stra = g_strdup_printf ("Not picked yet !");
      strb = g_strdup_printf (DELETEB);
    }
    else
    {
      stra = g_strdup_printf ("%d atom(s)", tmp_frig -> num);
      strb = g_strdup_printf (APPLY);
    }
  }
  else if (i == 9)
  {
    j=0;
    if (! tmp_ftet -> num)
    {
      stra = g_strdup_printf ("Not picked yet !");
      strb = g_strdup_printf (DELETEB);
    }
    else
    {
      k = tmp_fmol -> atoms_id[tmp_ftet -> num - 1][0].a;
      stra = g_strdup_printf ("%s - %d", exact_name(tmp_proj -> chemistry -> label[get_active_atom (tmp_fmol -> id, k) -> sp]), tmp_ftet -> num);
      strb = g_strdup_printf (APPLY);
    }
  }
  else if (i == 11 || i == 12 || i == 13 || i == 14)
  {
    j=i-11;
    if (tmp_fbody -> na[j] < 0)
    {
      stra = g_strdup_printf ("Not picked yet !");
      strb = g_strdup_printf (DELETEB);
    }
    else
    {
      stra = g_strdup_printf ("%s", exact_name(get_active_atom (tmp_fbody -> ma[j][0]-1, tmp_fbody -> a[j][0]) -> name));
      strb = g_strdup_printf (APPLY);
    }
    if (tmp_fbody -> bd == 2)
    {
      cross_box = destroy_this_widget (cross_box);
      cross_box = combo_cross (tmp_fbody);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cross_hbox, cross_box, FALSE, FALSE, 0);
      show_the_widgets (cross_box);
    }
    if (tmp_fbody -> na[j] < 0)
    {
      widget_set_sensitive (cross_vbox, FALSE);
    }
    else
    {
      widget_set_sensitive (cross_vbox, TRUE);
      combo_set_active (cross_box, 0);
    }
    gtk_label_set_text (GTK_LABEL(body_lab), body_str (tmp_fbody -> bd));
    gtk_label_set_use_markup (GTK_LABEL(body_lab), TRUE);
  }
  set_image_from_icon_name (img_but[j], strb);
  g_free (strb);
  if (i == 4 || i == 5)
  {
    if (tmp_fcons -> ia[0] && tmp_fcons -> ia[1])
    {
      // update_distance
      tmp_fcons -> av = 0.0;
      for (k=0; k<tmp_fmol -> multi; k++)
      {
        l = tmp_fmol -> atoms_id[tmp_fcons -> ia[0]-1][k].a;
        m = tmp_fmol -> atoms_id[tmp_fcons -> ia[0]-1][k].b;
        n = get_active_atom (tmp_fmol -> id, l) -> list[m];
        l = tmp_fmol -> atoms_id[tmp_fcons -> ia[1]-1][k].a;
        m = tmp_fmol -> atoms_id[tmp_fcons -> ia[1]-1][k].b;
        o = get_active_atom (tmp_fmol -> id, l) -> list[m];
        tmp_fcons -> av += distance_3d (& tmp_proj -> cell, 0, & tmp_proj -> atoms[0][n], & tmp_proj -> atoms[0][o]).length;
      }
      tmp_fcons -> av /= tmp_fmol -> multi;
    }
    update_field_dist (tmp_fcons -> av);
  }
  else if (i == 6 || i == 7)
  {
    if (tmp_fpmf -> num[0] > 0 && tmp_fpmf -> num[1] > 0)
    {
      atom at[2][tmp_fmol -> multi];
      float ma[2][tmp_fmol -> multi];
      gboolean all_zero = TRUE;
      float v;
      for (k=0; k<2; k++)
      {
        for (l=0; l<tmp_fpmf -> num[k]; l++)
        {
          if (tmp_fpmf -> weight[k][l] != 0.0)
          {
            all_zero = FALSE;
            break;
          }
        }
        if (! all_zero) break;
      }
      for (k=0; k<2; k++)
      {
        for (l=0; l<tmp_fmol -> multi; l++)
        {
          ma[k][l] = 0.0;
          at[k][l].x = 0.0;
          at[k][l].y = 0.0;
          at[k][l].z = 0.0;
        }
      }
      for (k=0; k<2; k++)
      {
        for (l=0; l<tmp_fpmf -> num[k]; l++)
        {
          m = tmp_fpmf -> list[k][l];
          for (n=0; n<tmp_fmol -> multi; n++)
          {
            o = tmp_fmol -> atoms_id[m][n].a;
            p = tmp_fmol -> atoms_id[m][n].b;
            q = get_active_atom (tmp_fmol -> id, o) -> list[p];
            if (all_zero)
            {
              r = tmp_proj -> atoms[0][q].sp;
              v = tmp_proj -> chemistry -> chem_prop[CHEM_M][r];
            }
            else
            {
              v = tmp_fpmf -> weight[k][l];
            }
            at[k][n].x += v * tmp_proj -> atoms[0][q].x;
            at[k][n].y += v * tmp_proj -> atoms[0][q].y;
            at[k][n].z += v * tmp_proj -> atoms[0][q].z;
            ma[k][n] += v;
          }
        }
      }
      v = 0.0;
      for (n=0; n<tmp_fmol -> multi; n++)
      {
        for (k=0; k<2; k++)
        {
          at[k][n].x /= ma[k][n];
          at[k][n].y /= ma[k][n];
          at[k][n].z /= ma[k][n];
        }
        v += distance_3d (& tmp_proj -> cell, 0, & at[0][n], & at[1][n]).length;
      }
      tmp_fpmf -> av = v / tmp_fmol -> multi;
    }
    update_field_dist (tmp_fpmf -> av);
  }
  gtk_button_set_label (but, stra);
  gtk_widget_set_size_request ((GtkWidget *)but, 150, -1);
  g_free (stra);
}

/*!
  \fn G_MODULE_EXPORT void changed_atom_combo (GtkComboBox * box, gpointer data)

  \brief change atom

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_atom_combo (GtkComboBox * box, gpointer data)
{
  int i, j, k;
  i = GPOINTER_TO_INT(data);
  j = combo_get_active ((GtkWidget *)box);
  switch (i)
  {
    case 0:
      widget_set_sensitive (afftype, (j) ? TRUE : FALSE);
      k = -1;
      if (tmp_fat -> afid > -1)
      {
        for (k=0; k<atoms_id[tmp_fat -> sp]; k++)
        {
          if (atoms_id_list[tmp_fat -> sp][k] == tmp_fat -> afid) break;
        }
      }
      combo_set_active (afftype, k+1);
      if (j)
      {
        tmp_fat -> mass =(tmp_fat -> afid < 0) ? tmp_proj -> chemistry -> chem_prop[CHEM_M][tmp_fat -> sp] : get_force_field_atom_mass (tmp_fat -> sp, tmp_fat -> afid);
      }
      else
      {
        tmp_fat -> mass = tmp_proj -> chemistry -> chem_prop[CHEM_M][tmp_fat -> sp];
      }
      break;
    case 1:
      if (j > 0)
      {
        tmp_fat -> afid = atoms_id_list[tmp_fat -> sp][j - 1];
      }
      else
      {
        tmp_fat -> afid = - 1;
      }
      tmp_fat -> mass = get_force_field_atom_mass (tmp_fat -> sp, j-1);
      check_atom_for_updates ();
      break;
  }
}

/*!
  \fn gchar * get_body_element_name (field_nth_body * body, int aid, int nbd)

  \brief get field body potential element name

  \param body the field body potential
  \param aid the atom id, if any
  \param nbd the body potential id
*/
gchar * get_body_element_name (field_nth_body * body, int aid, int nbd)
{
  int i, j;
  i = body -> ma[aid][0];
  if (! nbd)
  {
    return g_strdup_printf ("%s",  get_active_atom (i, body -> a[aid][0]) -> name);
  }
  else
  {
    j = get_active_shell (i, body -> a[aid][0]-nbd+1) -> ia[0];
    return g_strdup_printf ("%s_sh",  get_active_atom (i, j) -> name);
  }
}

/*!
  \fn gboolean body_identicals (field_nth_body * body, int nbd, int * na, int ** ma, int ** ba)

  \brief are these non bonded potentials identicals ?

  \param body the field non bonded property
  \param nbd number of distinct interactions
  \param na number of atoms (0 = 1st pot, 1 = 2nd pot)
  \param ma 1st potential data
  \param ba 2nd potential data
*/
gboolean body_identicals (field_nth_body * body, int nbd, int * na, int ** ma, int ** ba)
{
  gchar * stra, * strb, * strc, * strd;
  int i, j;
  stra = get_body_element_name (body, 0, 0);
  strb = get_body_element_name (body, 1, 0);
  i = ma[0][0];
  if (ba[0][0] < nbd)
  {
    strc = g_strdup_printf ("%s",  get_active_atom (i, ba[0][0]) -> name);
  }
  else
  {
    j = get_active_shell (i, ba[0][0]-nbd) -> ia[0];
    strc = g_strdup_printf ("%s_sh",  get_active_atom (i, j) -> name);
  }
  i = ma[1][0];
  if (ba[1][0] < nbd)
  {
    strd = g_strdup_printf ("%s",  get_active_atom (i, ba[1][0]) -> name);
  }
  else
  {
    j = get_active_shell (i, ba[1][0]-nbd) -> ia[0];
    strd = g_strdup_printf ("%s_sh",  get_active_atom (i, j) -> name);
  }
  gboolean res = FALSE;
  if (g_strcmp0(stra, strc) == 0 && g_strcmp0(strb, strd) == 0) res = TRUE;
  if (g_strcmp0(stra, strd) == 0 && g_strcmp0(strb, strc) == 0) res = TRUE;
  // g_debug ("stra= %s, strb= %s, strc= %s, strd= %s:: res= %d", stra, strb, strc, strd, res);
  g_free (stra);
  g_free (strb);
  g_free (strc);
  g_free (strd);
  return res;
}

/*!
  \fn int get_num_vdw_max ()

  \brief Get the number of field shell interactions
*/
int get_num_vdw_max ()
{
  int i;
  field_molecule * molff;
  field_shell * shellff;
  molff = tmp_field -> first_molecule;
  i = 0;
  while (molff)
  {
    i += molff -> atoms;
    shellff = molff -> first_shell;
    while (shellff)
    {
      if (shellff -> use && shellff -> vdw && shellff -> ia[0] > -1) i ++;
      shellff = shellff -> next;
    }
    molff = molff -> next;
  }
  return i;
}

/*!
  \fn void adjust_vdw_interactions (gboolean add_shell)

  \brief adjust VdW interactions

  \param add_shell update field shells
*/
void adjust_vdw_interactions (gboolean add_shell)
{
  int i, j, k, l, m, n;
  field_molecule * molff;
  field_atom* atff;
  field_shell * shellff;
  field_nth_body * bodyff;

  i = get_num_vdw_max ();
  m = i * (i+1) / 2;
  n = (add_shell) ? i * (i-1) / 2 : (i+1) * (i+2) / 2;
  bodyff = get_active_body (0, 0);
  while (bodyff)
  {
    for (j=0; j<2; j++)
    {
      k = bodyff -> ma[j][0];
      if (bodyff -> a[j][0] >= n)
      {
        bodyff -> a[j][0] -= n;
        bodyff -> a[j][0] += m;
      }
    }
    bodyff = bodyff -> next;
  }
  if (add_shell)
  {
    gboolean add_vdw;
    gchar * str;
    gchar ** to_be_vdw = g_malloc (i*sizeof*to_be_vdw);
    int * vdw_mlist = allocint (i);
    int ** vdw_aids = allocdint (i,i);
    int ** vdw_mids = allocdint (i,i);
    int vdw_na[2];
    int * vdw_a[2], * vdw_ma[2];
    int nbd = m;
    molff = tmp_field -> first_molecule;
    l = 0;
    while (molff)
    {
      atff = molff -> first_atom;
      while (atff)
      {
        add_vdw = TRUE;
        for (k=0; k<l; k++)
        {
          if (g_strcmp0 (to_be_vdw[k], atff -> name) == 0)
          {
            add_vdw = FALSE;
            vdw_mlist[k] ++;
            vdw_mids[k][vdw_mlist[k]] = molff -> id;
            vdw_aids[k][vdw_mlist[k]] = atff -> id;
            break;
          }
        }
        if (add_vdw)
        {
          to_be_vdw[k] = g_strdup_printf ("%s", atff -> name);
          vdw_mids[k][0] = molff -> id;
          vdw_aids[k][0] = atff -> id;
          l ++;
        }
        atff = atff -> next;
      }
      shellff = molff -> first_shell;
      while (shellff)
      {
        if (shellff -> use && shellff -> vdw && shellff -> ia[0] > -1)
        {
          add_vdw = TRUE;
          atff = get_active_atom (molff -> id, shellff -> ia[0]);
          str = g_strdup_printf ("%s_sh", atff -> name);
          for (k=0; k<l; k++)
          {
            if (g_strcmp0 (to_be_vdw[k], str) == 0)
            {
              add_vdw = FALSE;
              vdw_mlist[k] ++;
              vdw_mids[k][vdw_mlist[k]] = molff -> id;
              vdw_aids[k][vdw_mlist[k]] = shellff -> id + nbd;
              break;
            }
          }
          if (add_vdw)
          {
            to_be_vdw[k] = g_strdup_printf ("%s", str);
            vdw_mids[k][0] = molff -> id;
            vdw_aids[k][0] = shellff -> id + nbd;
            l ++;
          }
          g_free (str);
        }
        shellff = shellff -> next;
      }
      molff = molff -> next;
    }
    i = tmp_field -> nbody[0];
    j = 0;
    for (k=0; k<l; k++)
    {
      bodyff = get_active_body (0, 0);
      while (bodyff)
      {
        vdw_na[0] = vdw_na[1] = vdw_mlist[k]+1;
        vdw_a[0] = vdw_a[1] = duplicate_int (vdw_na[0], vdw_aids[k]);
        vdw_ma[0] = vdw_ma[1] = duplicate_int (vdw_na[0], vdw_mids[k]);
        if (body_identicals(bodyff, nbd, vdw_na, vdw_ma, vdw_a)) break;
        bodyff = bodyff -> next;
      }
      if (! bodyff)
      {
        bodyff = get_active_body (i+j, 0);
        bodyff -> next = init_field_nth_body (i+j, 0, vdw_na, vdw_ma, vdw_a);
        bodyff -> next -> prev = bodyff;
        j ++;
      }
    }
    for (m=0; m<l-1; m++)
    {
      for (n=m+1; n<l; n++)
      {
        bodyff = get_active_body (0, 0);
        while (bodyff)
        {
          vdw_na[0] = vdw_mlist[m]+1;
          vdw_na[1] = vdw_mlist[n]+1;
          vdw_a[0] = duplicate_int (vdw_na[0], vdw_aids[m]);
          vdw_a[1] = duplicate_int (vdw_na[1], vdw_aids[n]);
          vdw_ma[0] = duplicate_int (vdw_na[0], vdw_mids[m]);
          vdw_ma[1] = duplicate_int (vdw_na[1], vdw_mids[n]);
          if (body_identicals(bodyff, nbd, vdw_na, vdw_ma, vdw_a)) break;
          bodyff = bodyff -> next;
        }
        if (! bodyff)
        {
          bodyff = get_active_body (i+j, 0);
          bodyff -> next = init_field_nth_body (i+j, 0, vdw_na, vdw_ma, vdw_a);
          bodyff -> next -> prev = bodyff;
          j ++;
        }
      }
    }
    tmp_field -> nbody[0] += j;
  }
  else
  {
    atff = get_active_atom (tmp_fmol -> id, tmp_fshell -> ia[0]);
    gchar * str = g_strdup_printf ("%s_sh", atff -> name);
    gchar * stra, * strb;
    bodyff = get_active_body (0, 0);
    i = 0;
    while (bodyff)
    {
      stra = get_body_element_name (bodyff, 0, m+1);
      strb = get_body_element_name (bodyff, 1, m+1);
      if (g_strcmp0(str,stra) == 0 || g_strcmp0(str,strb) == 0)
      {
        if (bodyff -> next)
        {
          bodyff -> prev -> next = bodyff -> next;
          bodyff -> next -> prev = bodyff -> prev;
        }
        else
        {
          bodyff = bodyff -> prev;
          g_free (bodyff -> next);
          bodyff -> next = NULL;
        }
        i ++;
      }
      g_free (stra);
      g_free (strb);
      bodyff = bodyff -> next;
    }
    g_free (str);
    tmp_field -> nbody[0] -= i;
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void shell_in_vdw (GtkCheckButton * but, gpointer data)

  \brief VdW in shell toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void shell_in_vdw (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void shell_in_vdw (GtkToggleButton * but, gpointer data)

  \brief VdW in shell toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void shell_in_vdw (GtkToggleButton * but, gpointer data)
#endif
{
  tmp_fshell -> vdw = button_get_status ((GtkWidget *)but);
  adjust_vdw_interactions (tmp_fshell -> vdw);
}

dint rep;
gchar * rep_atom_name;

/*!
  \fn G_MODULE_EXPORT void run_edit_parameters (GtkDialog * dialog, gint response_id, gpointer data)

  \brief edit field parameter - running the dialog

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_edit_parameters (GtkDialog * dialog, gint response_id, gpointer data)
{
  if (rep.a > 6 && rep.a < MOLIMIT)
  {
    adjust_field_prop (rep.a-7, rep.b, tmp_fprop, edit_atids, tmp_fprop -> key);
  }
  else if (rep.a == 1)
  {
    if (g_strcmp0 (rep_atom_name, tmp_fat -> name) != 0)
    {
      // Atom name's change, need to update non bonded interactions
      compare_non_bonded (tmp_fat -> name);
    }
    g_free (rep_atom_name);
    rep_atom_name = NULL;
  }
  destroy_this_dialog (dialog);
}

/*!
  \fn void edit_parameters (int f, int id)

  \brief edit field parameter - creating the dialog

  \param f the type of parameter to edit
  \param id the field molecule id, if any
*/
void edit_parameters (int f, int id)
{
  gchar * str = g_strdup_printf ("%s parameter(s)", felemts[f+1]);
  GtkWidget * dialog = dialogmodal(str, GTK_WINDOW(field_assistant));
  GtkWidget * lab;
  GtkWidget * entry;
  GtkWidget * but;
  GtkWidget * hbox;
  GtkWidget * combo;
  GtkWidget * box = dialog_get_content_area (dialog);
  gboolean show_it;
  gboolean use_it;
  gchar * cs_name[3]={"Core atom: ", "Shell atom: ", "Selection: "};
  gchar * cs_param[4]={"Mass:", "Charge:", "<i>k<sub>2</sub></i> ", "<i>k<sub>4</sub></i> "};
  gchar * cs_unit[4]={"g mol<sup>-1</sup>", " ", "&#xC5;<sup>-2</sup>", "&#xC5;<sup>-4</sup>"};
  gchar * co_name[4]={"First atom: ", "Second atom: ", "Third atom: ", "Fourth atom: "};
  gchar * pm_name[2]={"First unit: ", "Second unit: "};
  int i, j, k, l, m;
  float v;
  gchar * ba;
  is_moy = 1;
  show_it = use_it = FALSE;
  for (i=0; i<2; i++) ff_p_combo[i] = NULL;
  if (f < MOLIMIT)
  {
   i = combo_get_active (combo_mol[f-1]);
   lab = markup_label (g_strdup_printf ("\tMolecule: \t<b>%s</b>", get_active_field_molecule(i) -> name), -1, 30, 0.0, 0.5);
   add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);
   tmp_fmol = get_active_field_molecule (i);
  }
  switch (f)
  {
    case 1:
      // Atom edit
      tmp_fat = get_active_atom(i, id);
      rep_atom_name = g_strdup_printf("%s", tmp_fat -> name);
      lab = markup_label (g_strdup_printf ("\tAtom: \t\t<b>%s</b>", rep_atom_name), -1, 50, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);
      hbox = create_hbox (0);
      str = g_strdup_printf ("Element:");
      lab = markup_label (str, 120, 30, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      str = g_strdup_printf ("%s", exact_name(tmp_proj -> chemistry -> element[tmp_fat -> sp]));
      lab = markup_label (str, -1, -1, 0.5, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      hbox = create_hbox (0);
      str = g_strdup_printf ("Field parameters:");
      lab = markup_label (str, 120, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      combo = create_combo ();
      combo_text_append (combo, "Manual");
      combo_text_append (combo, "Automatic");
      combo_set_active (combo, (tmp_fat -> afid > -1) ? 1 : 0);
      widget_set_sensitive (combo, atoms_id[tmp_fat -> sp]);
      g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(changed_atom_combo), GINT_TO_POINTER(0));
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      hbox = create_hbox (0);
      str = g_strdup_printf ("Force field type <sup>*</sup>:");
      lab = markup_label (str, 120, 30, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      afftype = create_combo ();
      combo_text_append (afftype, "None");
      k = 0;
      for (i=0; i<atoms_id[tmp_fat -> sp]; i++)
      {
        j = atoms_id_list[tmp_fat -> sp][i];
        str = g_strdup_printf ("%s", exact_name(ff_atoms[j][2]));
        if (g_strcmp0 (ff_atoms[j][3], " ") != 0) str = g_strdup_printf ("%s : %s", str, ff_atoms[j][3]);
        combo_text_append (afftype, str);
        g_free (str);
        if (atoms_id_list[tmp_fat -> sp][i] == tmp_fat -> afid) k = i+1;
      }
      combo_set_active (afftype, k);
      g_signal_connect (G_OBJECT(afftype), "changed", G_CALLBACK(changed_atom_combo), GINT_TO_POINTER(1));
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, afftype, FALSE, FALSE, 0);
      widget_set_sensitive (afftype, (tmp_fat -> afid > -1) ? TRUE : FALSE);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      hbox = create_hbox (0);
      str = g_strdup_printf ("Name: ");
      lab = markup_label (str, 120, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      entry = create_entry (G_CALLBACK(update_atom_parameter), 100, 15, FALSE, GINT_TO_POINTER(-1));
      update_entry_text (GTK_ENTRY(entry), tmp_fat -> name);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      hbox = create_hbox (0);
      str = g_strdup_printf ("Mass: ");
      lab = markup_label (str, 120, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      entry = create_entry (G_CALLBACK(update_atom_parameter), 100, 15, FALSE, GINT_TO_POINTER(0));
      update_entry_double (GTK_ENTRY(entry), tmp_fat -> mass);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      hbox = create_hbox (0);
      str = g_strdup_printf ("Charge: ");
      lab = markup_label (str, 120, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      entry = create_entry (G_CALLBACK(update_atom_parameter), 100, 15, FALSE, GINT_TO_POINTER(1));
      update_entry_double (GTK_ENTRY(entry), tmp_fat -> charge);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      hbox = create_hbox (0);
      str = g_strdup_printf ("Frozen: ");
      lab = markup_label (str, 120, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      str = g_strdup_printf ("%d atom(s)", tmp_fat -> frozen);
      but = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(select_atom_id_from_fied_molecule), GINT_TO_POINTER(1));
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      break;
    case 2:
      // Shell edit
      tmp_fshell = get_active_shell (i, id);
      str = g_strdup_printf ("\tCore-Shell N°<b>%d</b>", tmp_fshell -> id+1);
      lab = markup_label (str, 300, 50, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);
      for (k=0; k<2; k++)
      {
        hbox = create_hbox (0);
        lab = markup_label (cs_name[k], 100, -1, 0.0, 0.5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
        if (! tmp_fshell -> ia[k])
        {
          str = g_strdup_printf ("Not picked yet !");
          img_but[k] = stock_image (DELETEB);
        }
        else
        {
          j = tmp_fshell -> ia[k] - 1;
          l = tmp_fmol -> atoms_id[j][0].a;
          str = g_strdup_printf ("%s - %d", exact_name(tmp_proj -> chemistry -> label[get_active_atom (i, l) -> sp]), tmp_fshell -> ia[k]);
          img_but[k] = stock_image (APPLY);
        }
        but = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(selection_button), GINT_TO_POINTER(k+2));
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 0);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img_but[k], FALSE, FALSE, 30);
      }

      /*for (k=0; k<3; k++)
      {
        shell_hbox[k] = create_hbox (0);
        lab = markup_label (cs_name[k], 100, -1, 0.0, 0.5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, shell_hbox[k], lab, FALSE, FALSE, 10);
        if ((k < 2 && tmp_fshell -> ia[k] == -1) || (k == 2 && ! tmp_fshell -> ib))
        {
          str = g_strdup_printf ("Not picked yet !");
          img_but[k] = stock_image (DELETEB);
        }
        else
        {
          if (k < 2)
          {
            str = g_strdup_printf ("Selected !");
          }
          else
          {
            str = g_strdup_printf ("%d atom(s) selected !", tmp_fshell -> ia[1]-1);
          }
          img_but[k] = stock_image (APPLY);
        }
        if (k < 2)
        {
          shell_cbox[k] = create_combo ();
          combo_text_append (shell_cbox[k], "Not picked yet !");
          switch (k)
          {
            case 0:
              tmp_fat = tmp_fmol -> first_atom;
              while (tmp_fat)
              {
                combo_text_append (shell_cbox[k], tmp_fat -> name);
                tmp_fat = tmp_fat -> next;
              }
              break;
            case 1:
              combo_text_append (shell_cbox[k], "All atoms");
              combo_text_append (shell_cbox[k], "Selected atoms");
              break;
          }
          l = (! k) ? tmp_fshell -> ia[k]+1 : (tmp_fshell -> ia[k] < 1) ? tmp_fshell -> ia[k]+1 : 2;
          combo_set_active (shell_cbox[k], l);
          g_signal_connect (G_OBJECT(shell_cbox[k]), "changed", G_CALLBACK(changed_shell_combo), GINT_TO_POINTER(k));
          gtk_widget_set_size_request (shell_cbox[k], 150, -1);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, shell_hbox[k], shell_cbox[k], FALSE, FALSE, 0);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, shell_hbox[k], img_but[k], FALSE, FALSE, 30);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, box, shell_hbox[k], FALSE, FALSE, 0);
        }
        else
        {
           shell_but = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(selection_button), GINT_TO_POINTER(2));
           add_box_child_start (GTK_ORIENTATION_HORIZONTAL, shell_hbox[k], shell_but, FALSE, FALSE, 0);
           add_box_child_start (GTK_ORIENTATION_HORIZONTAL, shell_hbox[k], img_but[k], FALSE, FALSE, 30);
           add_box_child_start (GTK_ORIENTATION_VERTICAL, box, shell_hbox[k], FALSE, FALSE, 0);
        }
        g_free (str);
      }
      */
      for (k=0; k<4; k++)
      {
        hbox = create_hbox (0);
        lab = markup_label (cs_param[k], 100, -1, 0.0, 0.5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
        entry = create_entry (G_CALLBACK(update_atom_parameter), 150, 15, FALSE, GINT_TO_POINTER(2+k));
        v = (! k) ? tmp_fshell -> m : (k == 1) ? tmp_fshell -> z : (k == 2) ? tmp_fshell -> k2 : tmp_fshell -> k4;
        update_entry_double (GTK_ENTRY(entry), v);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
        if (k < 2)
        {
          str = g_strdup_printf ("%s", cs_unit[k]);
        }
        else
        {
          str = g_strdup_printf ("%s %s", fkeysw[activef][0][tmp_field -> energy_unit], cs_unit[k]);
        }
        lab = markup_label (str, -1, -1, 0.0, 0.5);
        g_free (str);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 30);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      }
      hbox = create_hbox (0);
      lab = markup_label("Use non-bonded: ", 185, 40, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button (NULL, -1, -1, tmp_fshell -> vdw, G_CALLBACK(shell_in_vdw), NULL), FALSE, FALSE, 30);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      show_it = tmp_fshell -> show;
      use_it = tmp_fshell -> use;
      break;
    case 3:
      // Constraint edit
      tmp_fcons = get_active_constraint (i, id);
      str = g_strdup_printf ("\tBond constraint N°<b>%d</b>", tmp_fcons -> id+1);
      lab = markup_label (str, 300, 50, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);
      for (k=0; k<2; k++)
      {
        hbox = create_hbox (0);
        lab = markup_label (co_name[k], 100, -1, 0.0, 0.5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
        if (! tmp_fcons -> ia[k])
        {
          str = g_strdup_printf ("Not picked yet !");
          img_but[k] = stock_image (DELETEB);
        }
        else
        {
          j = tmp_fcons -> ia[k]-1;
          l = tmp_fmol -> atoms_id[j][0].a;
          str = g_strdup_printf ("%s - %d", exact_name(tmp_proj -> chemistry -> label[get_active_atom (i, l) -> sp]), tmp_fcons -> ia[k]);
          img_but[k] = stock_image (APPLY);
        }
        but = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(selection_button), GINT_TO_POINTER(k+4));
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 0);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img_but[k], FALSE, FALSE, 30);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      }
      hbox = create_hbox (0);
      str = g_strdup_printf ("Av. distance (1): ");
      lab = markup_label (str, 100, 50, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      av_lgt = markup_label ("", 150, 50, 0.5, 0.5);
      update_field_dist (tmp_fcons -> av);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, av_lgt, FALSE, FALSE, 0);
      str = g_strdup_printf ("[&#xC5;]");
      lab = markup_label (str, -1, -1, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 30);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      hbox = create_hbox (0);
      str = g_strdup_printf ("Length: ");
      lab = markup_label (str, 100, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      entry = create_entry (G_CALLBACK(update_atom_parameter), 150, 15, FALSE, GINT_TO_POINTER(6));
      update_entry_double (GTK_ENTRY(entry), tmp_fcons -> length);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
      str = g_strdup_printf ("[&#xC5;]");
      lab = markup_label (str, -1, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 30);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      show_it = tmp_fcons -> show;
      use_it = tmp_fcons -> use;
      break;
    case 4:
      // Pmf edit
      tmp_fpmf = get_active_pmf (i, id);
      str = g_strdup_printf ("\tMean force potential N°<b>%d</b>", tmp_fpmf -> id+1);
      lab = markup_label (str, 300, 50, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);
      for (k=0; k<2; k++)
      {
        hbox = create_hbox (0);
        lab = markup_label (pm_name[k], 100, -1, 0.0, 0.5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
        if (tmp_fpmf -> num[k] == 0)
        {
          str = g_strdup_printf ("Not picked yet !");
          img_but[k] = stock_image (DELETEB);
        }
        else
        {
          str = g_strdup_printf ("%d atom(s)", tmp_fpmf -> num[k]);
          img_but[k] = stock_image (APPLY);
        }
        but = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(selection_button), GINT_TO_POINTER(k+6));
        g_free (str);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 0);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img_but[k], FALSE, FALSE, 30);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      }

      hbox = create_hbox (0);
      str = g_strdup_printf ("Av. d<sub>1-2</sub> (1): ");
      lab = markup_label (str, 100, 50, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      av_lgt = markup_label ("", 150, 50, 0.5, 0.5);
      update_field_dist (tmp_fpmf -> av);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, av_lgt, FALSE, FALSE, 0);
      str = g_strdup_printf ("[&#xC5;]");
      lab = markup_label (str, -1, 50, 0.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 30);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);

      hbox = create_hbox (0);
      str = g_strdup_printf ("Length: ");
      lab = markup_label (str, 100, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      entry = create_entry (G_CALLBACK(update_atom_parameter), 150, 15, FALSE, GINT_TO_POINTER(7));
      update_entry_double (GTK_ENTRY(entry), tmp_fpmf -> length);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
      str = g_strdup_printf ("[&#xC5;]");
      lab = markup_label (str, -1, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 30);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      show_it = tmp_fpmf -> show;
      use_it = tmp_fpmf -> use;
      break;
    case 5:
      // Rigid edit
      tmp_frig = get_active_rigid (i, id);
      str = g_strdup_printf ("\tRigid unit N°<b>%d</b>", id+1);
      lab = markup_label (str, 300, 50, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);

      hbox = create_hbox (0);
      str = g_strdup_printf ("Atom(s): ");
      lab = markup_label (str, 100, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      if (tmp_frig -> num == 0)
      {
        str = g_strdup_printf ("Not picked yet !");
        img_but[0] = stock_image (DELETEB);
      }
      else
      {
        str = g_strdup_printf ("%d atom(s)", tmp_frig -> num);
        img_but[0] = stock_image (APPLY);
      }
      but = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(selection_button), GINT_TO_POINTER(8));
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img_but[0], FALSE, FALSE, 30);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      show_it = tmp_frig -> show;
      use_it = tmp_frig -> use;
      break;
    case 6:
      // Tethered
      tmp_ftet = get_active_tethered (i, id);
      str = g_strdup_printf ("\tTethered atom N°<b>%d</b>", id+1);
      lab = markup_label (str, 300, 50, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);

      hbox = create_hbox (0);
      str = g_strdup_printf ("Atom: ");
      lab = markup_label (str, 100, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      if (! tmp_ftet -> num)
      {
        str = g_strdup_printf ("Not picked yet !");
        img_but[0] = stock_image (DELETEB);
      }
      else
      {
        k = tmp_fmol -> atoms_id[tmp_ftet -> num-1][0].a;
        str = g_strdup_printf ("%s - %d", exact_name(tmp_proj -> chemistry -> label[get_active_atom (i, k) -> sp]), tmp_ftet -> num);
        img_but[0] = stock_image (APPLY);
      }
      but = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(selection_button), GINT_TO_POINTER(9));
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img_but[0], FALSE, FALSE, 30);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      show_it = tmp_ftet -> show;
      use_it = tmp_ftet -> use;
      break;
    case SEXTERN:
      tmp_fext = get_active_external (id);
      body_lab = markup_label (field_str(tmp_fext -> key), 250, 50, 0.5, 0.5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, body_lab, FALSE, FALSE, 0);
      use_it = tmp_fext -> use;
      break;
    default:
      if (f < MOLIMIT)
      {
        // Bond, angle, dihedral, improper, inversion edit
        tmp_fstr = get_active_struct (f-7, i, id);
        m = struct_id (f);
        str = g_strdup_printf ("\t%s type N°<b>%d</b>\n\tField atoms: <b>", mo_title[f-7], id+1);
        for (k=0; k<m; k++)
        {
          if (k == m-1) str = g_strdup_printf ("%s</b> and <b>", str);
          str = g_strdup_printf ("%s%s", str, get_active_atom (i, tmp_fstr -> aid[k]) -> name);
          if (m > 2 && k < m-1) str = g_strdup_printf ("%s</b>, <b>", str);
        }
        str = g_strdup_printf ("%s</b>", str);
        edit_atids = allocint (m);
        gtk_tree_model_get (GTK_TREE_MODEL(field_model[f]), & field_iter, 0, & is_moy, -1);
        if (! is_moy)
        {
          str = g_strdup_printf ("%s\n\tBetween atoms: <b>", str);
          for (k=0; k<m; k++)
          {
            gtk_tree_model_get (GTK_TREE_MODEL(field_model[f]), & field_iter, k+1, & ba, -1);
            edit_atids[k] =  (int) string_to_double ((gpointer)ba) - 1;
            if (k == m-1) str = g_strdup_printf ("%s</b> and <b>", str);
            str = g_strdup_printf ("%s%s", str, ba);
            if (m > 2 && k < m-1) str = g_strdup_printf ("%s</b>, <b>", str);
          }
          str = g_strdup_printf ("%s</b>", str);
          if (tmp_fstr -> other)
          {
            tmp_fprop = get_active_prop_using_atoms (tmp_fstr -> other, m, edit_atids);
            if (! tmp_fprop)
            {
              tmp_fprop = tmp_fstr -> other;
              while (tmp_fprop -> next) tmp_fprop = tmp_fprop -> next;
              tmp_fprop -> next = duplicate_field_prop (tmp_fstr -> def, f-7);
              tmp_fprop = tmp_fprop -> next;
              for (k=0; k<m; k++) tmp_fprop -> aid[k] = edit_atids[k];
            }
          }
          else
          {
            tmp_fstr -> other = duplicate_field_prop (tmp_fstr -> def, f-7);
            for (k=0; k<m; k++) tmp_fstr -> other -> aid[k] = edit_atids[k];
            tmp_fprop = tmp_fstr -> other;
          }
        }
        else
        {
          for (l=0; l<m; l++) edit_atids[l] = -1;
          tmp_fprop = tmp_fstr -> def;
        }
        add_box_child_start (GTK_ORIENTATION_VERTICAL, box, markup_label (str, -1, (is_moy) ? 70 : 100, 0.0, 0.5), FALSE, FALSE, 0);
        g_free (str);
        show_it = tmp_fprop -> show;
        use_it = tmp_fprop -> use;
      }
      else
      {
        l = f - MOLIMIT;
        tmp_fbody = get_active_body (id, l);
        body_lab = markup_label (body_str (tmp_fbody -> bd), 250, 50, 0.5, 0.5);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, box, body_lab, FALSE, FALSE, 0);
        if (l > 0)
        {
          for (k=0; k< body_at(l); k++)
          {
            hbox = create_hbox (0);
            if (l == 2)
            {
              lab = markup_label ("Tersoff atom: ", 100, -1, 0.0, 0.5);
            }
            else
            {
              lab = markup_label (co_name[k], 100, -1, 0.0, 0.5);
            }
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
            if (tmp_fbody -> na[k] < 0)
            {
              str = g_strdup_printf ("Not picked yet !");
              img_but[k] = stock_image (DELETEB);
            }
            else
            {
              str = g_strdup_printf ("%s", get_active_atom (tmp_fbody -> ma[k][0], tmp_fbody -> a[k][0]) -> name);
              img_but[k] = stock_image (APPLY);
            }
            but = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(selection_button), GINT_TO_POINTER(11+k));
            g_free (str);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 0);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img_but[k], FALSE, FALSE, 30);
            add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
          }
        }
        show_it = tmp_fbody -> show;
        use_it = tmp_fbody -> use;
      }
      break;
  }
  if (f < SEXTERN)
  {
    hbox = create_hbox (0);
    lab = markup_label("Visualize in the model: ", (f == 1) ? 150 : 185, 40, 0.0, 0.5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button (NULL, -1, -1, show_it, G_CALLBACK(visualize_it), GINT_TO_POINTER(f)), FALSE, FALSE, 30);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
  }
  if (f == 1)
  {
    str = g_strdup_printf ("<sup>*</sup> this will be used to adjust bonding, angles, etc ... accordingly,\n"
                           "  providing that some parameters can be found in the force field data.\n"
                           "  Please note that comments are directly imported from the force field file.");
    lab = markup_label (str, -1, 75, 0.5, 0.5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);
  }
  if (f > 1)
  {
    hbox = create_hbox (0);
    lab = markup_label("Use to create force field: ", 185, 40, 0.0, 0.5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button (NULL, -1, -1, use_it, G_CALLBACK(select_it), GINT_TO_POINTER(f)), FALSE, FALSE, 30);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
    if (f > 5)
    {
      if (f > 6 && f < 16)
      {
        if (f != 15 || tmp_field -> type < CFF91 || tmp_field -> type > COMPASS)
        {
          hbox = create_hbox (0);
          gchar * funits[5] ={"Ev", "kcal mol<sup>-1</sup>", "kJ mol<sup>-1</sup>", "K B<sup>-1</sup>", "DL_POLY internal units"};
          str = g_strdup_printf ("Field parameters <sup>*</sup>:");
          lab = markup_label (str, 120, -1, 0.0, 0.5);
          g_free (str);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, create_field_prop_combo (f, is_moy), FALSE, FALSE, 0);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
          str = g_strdup_printf ("<sup>*</sup> Available parameters with matching chemical species,\n"
                                 "  %s force field energy related parameters in: %s \n"
                                 "  if required conversion to FIELD file energy unit will be performed upon selection.\n"
                                 "  Please note that comments are directly imported from the force field file.\n",
                                 field_acro[tmp_field -> type], funits[ff_unit]);
          lab = markup_label (str, -1, 75, 0.5, 0.5);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, box, lab, FALSE, FALSE, 0);
        }
      }
      hbox = create_hbox (0);
      str = g_strdup_printf ("Potential: ");
      lab = markup_label (str, 100, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      field_key_combo = create_combo ();
      for (j=0; j<fetypes[activef][f-5]; j++)
      {
        str = g_strdup_printf ("%s (%s)", fnames[activef][f-5][j], exact_name(fkeysw[activef][f-5][j]));
        combo_text_append (field_key_combo, str);
        g_free (str);
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, field_key_combo, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
      param_box = create_hbox (0);
      p_box = param_prop_param_box (f);
      switch (f)
      {
        case 6:
          // Tethered
          combo_set_active (field_key_combo, tmp_ftet -> key);
          break;
        case SEXTERN:
          combo_set_active (field_key_combo, tmp_fext -> key);
          break;
        default:
          if (f < MOLIMIT)
          {
            combo_set_active (field_key_combo, tmp_fprop -> key);
          }
          else
          {
            // non nonded
            combo_set_active (field_key_combo, tmp_fbody -> key);
          }
          break;
      }
      g_signal_connect (G_OBJECT(field_key_combo), "changed", G_CALLBACK(changed_field_key_combo), GINT_TO_POINTER(f));
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, param_box, p_box, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, param_box, FALSE, FALSE, 5);
    }
    else if (f == 3)
    {
      hbox = create_hbox (0);
      str = g_strdup_printf ("  <b>(1)</b> average distance between the 2 atoms\n"
                             "\tas measured in the 3D model");
      lab = markup_label (str, 320, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
    }
    else if (f == 4)
    {
      hbox = create_hbox (0);
      str = g_strdup_printf ("  <b>(1)</b> average distance between the barycenters\n"
                             "\tof units 1 and 2 as measured in the model");
      lab = markup_label (str, 320, -1, 0.0, 0.5);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
    }
  }
  rep.a = f;
  rep.b = m;
  g_signal_connect (G_OBJECT(dialog), "response", G_CALLBACK(run_edit_parameters), NULL);
  show_the_widgets (dialog);
  if (f == 2)
  {
    if (tmp_fshell -> ia[0] < 0 || tmp_fshell -> ia[1] < 1) hide_the_widgets (shell_hbox[2]);
    if (tmp_fshell -> ia[0] < 0) widget_set_sensitive (shell_hbox[1], FALSE);
  }

  dialog_id ++;
  Event_loop[dialog_id] = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (Event_loop[dialog_id]);
}

/*!
  \fn void update_tersoffs (int id, int key)

  \brief update non bonded potential

  \param id potential id
  \param key potenntial type
*/
void update_tersoffs (int id, int key)
{
  int i;
  field_nth_body * bod = tmp_field -> first_body[2];
  for (i=0; i<tmp_field -> nbody[2]; i++)
  {
    if (i != id)
    {
      if (bod -> key != key)
      {
        bod -> key = key;
        bod -> val = NULL;
        bod -> val = allocfloat (fvalues[activef][MOLIMIT-4][bod -> key]);
      }
    }
    if (bod -> next != NULL) bod = bod -> next;
  }
  if (key && tmp_field -> cross != NULL)
  {
    g_free (tmp_field -> cross);
    tmp_field -> cross = NULL;
  }
}

/*!
  \fn void check_tersoffs (int id, int key)

  \brief check non bonded potential

  \param id potential id
  \param key potential type
*/
void check_tersoffs (int id, int key)
{
  int i, j;
  if (change_tersoff && id != -1) update_tersoffs (id, key);
  if (! key)
  {
    if (tmp_field -> cross)
    {
      g_free (tmp_field -> cross);
      tmp_field -> cross = NULL;
    }
    if (cross)
    {
      tmp_field -> cross = g_malloc (tmp_field -> nbody[2]*sizeof*tmp_field -> cross);
      for (i=0; i<tmp_field -> nbody[2]; i++)
      {
        tmp_field -> cross[i] = g_malloc (tmp_field -> nbody[2]*sizeof*tmp_field -> cross[i]);
        for (j=0; j<tmp_field -> nbody[2]; j++) tmp_field -> cross[i][j] = duplicate_double (3, cross[i][j]);
      }
    }
    else
    {
      tmp_field -> cross = alloctdouble (tmp_field -> nbody[2], tmp_field -> nbody[2], 3);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void edit_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief edit field property callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void edit_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  int i, j, k;
  i = GPOINTER_TO_INT (data);
  gchar * tmp;
  switch (i)
  {
    case 0:
      if (action == NULL)
      {
        tmp_fmol = get_active_field_molecule (row_id);
        tmp = g_strdup_printf ("Please enter name for new molecule N°%d", tmp_field -> molecules);
        tmp_fmol -> name = g_strdup_printf("MOL-%d", i);
        tmp = cask (tmp, "Molecule name:", 0, tmp_fmol -> name, field_assistant);
        if (tmp != NULL)
        {
          tmp_fmol -> name = g_strdup_printf ("%s", tmp);
          g_free (tmp);
        }
      }
      else
      {
        tmp_fmol = get_active_field_molecule (row_id);
        tmp = g_strdup_printf ("Please enter name for molecule N°%d", row_id+1);
        tmp = cask (tmp, "Molecule name:", 0, tmp_fmol -> name, field_assistant);
        if (tmp != NULL)
        {
          tmp_fmol -> name = g_strdup_printf ("%s", tmp);
          g_free (tmp);
          clean_up_molecules_info (FALSE);
        }
      }
      break;
    default:
      if (i == MOLIMIT+2)
      {
        num_body_d = tmp_field -> nbody[2];
        cross = NULL;
        if (tmp_field -> cross != NULL)
        {
          cross = g_malloc (tmp_field -> nbody[2]*sizeof*cross);
          for (j=0; j<tmp_field -> nbody[2]; j++)
          {
            cross[j] = g_malloc (tmp_field -> nbody[2]*sizeof*cross[j]);
            for (k=0; k<tmp_field -> nbody[2]; k++) cross[j][k] = duplicate_double (3, tmp_field -> cross[j][k]);
          }
        }
        change_tersoff = FALSE;
      }
      edit_parameters (i, row_id);
      if (i == MOLIMIT+2) check_tersoffs (tmp_fbody -> id, tmp_fbody -> key);
      update_field_trees ();
      break;
  }
}

/*!
  \fn G_MODULE_EXPORT void add_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief add field property callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void add_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  int i, j, k, l, m;
  gboolean save_it;
  i = GPOINTER_TO_INT (data);
  field_molecule * fmol;
  if (i < MOLIMIT)
  {
    j = combo_get_active (combo_mol[i-1]);
    tmp_fmol = fmol = get_active_field_molecule (j);
  }
  if (i == 2)
  {
    if (fmol -> first_shell == NULL)
    {
      fmol -> first_shell = init_field_shell (fmol -> shells, 0, 0);
    }
    else
    {
      tmp_fshell = get_active_shell (j, fmol -> shells);
      tmp_fshell -> next = init_field_shell (fmol -> shells, 0, 0);
      tmp_fshell -> next -> prev = g_malloc (sizeof*tmp_fshell -> next -> prev);
      tmp_fshell -> next -> prev = tmp_fshell;
    }
    row_id = fmol -> shells;
    edit_parameters (i, fmol -> shells);
    if (tmp_fshell -> ia[0] || tmp_fshell -> ia[1])
    {
      fmol -> shells ++;
      if (! tmp_fshell -> ia[0] || ! tmp_fshell -> ia[1]) tmp_fshell -> use = FALSE;
    }
    else
    {
      tmp_fshell = get_active_shell (j, fmol -> shells);
      tmp_fshell -> next = NULL;
      if (fmol -> shells == 0) fmol -> first_shell = NULL;
    }
  }
  else if (i == 3)
  {
    if (fmol -> first_constraint == NULL)
    {
      fmol -> first_constraint = init_field_constraint (fmol -> constraints, 0, 0);
    }
    else
    {
      tmp_fcons = get_active_constraint (j, fmol -> constraints);
      tmp_fcons -> next = init_field_constraint (fmol -> constraints, 0, 0);
      tmp_fcons -> next -> prev = g_malloc0 (sizeof*tmp_fcons -> next -> prev);
      tmp_fcons -> next -> prev = tmp_fcons;
    }
    row_id = fmol -> constraints;
    edit_parameters (i, fmol -> constraints);
    if (tmp_fcons -> ia[0] || tmp_fcons -> ia[1])
    {
      fmol -> constraints ++;
      if (! tmp_fcons -> ia[0] || ! tmp_fcons -> ia[1]) tmp_fcons -> use = FALSE;
    }
    else
    {
      tmp_fcons = get_active_constraint (j, fmol -> constraints);
      tmp_fcons -> next = NULL;
      if (fmol -> constraints == 0) fmol -> first_constraint = NULL;
    }
  }
  else if (i == 4)
  {
    if (fmol -> first_pmf == NULL)
    {
      fmol -> first_pmf = init_field_pmf (fmol -> pmfs, NULL, NULL, NULL);
    }
    else
    {
      tmp_fpmf = get_active_pmf (j, fmol -> pmfs);
      tmp_fpmf -> next = init_field_pmf (fmol -> pmfs, NULL, NULL, NULL);
      tmp_fpmf -> next -> prev = g_malloc0 (sizeof*tmp_fpmf -> next -> prev);
      tmp_fpmf -> next -> prev = tmp_fpmf;
    }
    row_id = fmol -> pmfs;
    edit_parameters (i, fmol -> pmfs);
    if (tmp_fpmf -> num[0] > 0 || tmp_fpmf -> num[1] > 0)
    {
      fmol -> pmfs ++;
      if (tmp_fpmf -> length == 0.0 || tmp_fpmf -> num[0] == 0 || tmp_fpmf -> num[1] == 0) tmp_fpmf -> use = FALSE;
      if (tmp_fpmf -> use)
      {
        toviz.c = TRUE;
        select_object (i, fmol -> pmfs, j);
      }
    }
    else
    {
      tmp_fpmf = get_active_pmf (j, fmol -> pmfs);
      tmp_fpmf -> next = NULL;
      if (fmol -> pmfs == 0) fmol -> first_pmf = NULL;
    }
  }
  else if (i == 5)
  {
    if (fmol -> first_rigid == NULL)
    {
      fmol -> first_rigid = init_field_rigid (fmol -> rigids, 0, NULL);
    }
    else
    {
      tmp_frig = get_active_rigid (j, fmol -> rigids);
      tmp_frig -> next = init_field_rigid (fmol -> rigids, 0, NULL);
      tmp_frig -> next -> prev = g_malloc0 (sizeof*tmp_frig -> next -> prev);
      tmp_frig -> next -> prev = tmp_frig;
    }
    row_id = fmol -> rigids;
    edit_parameters (i, fmol -> rigids);
    if (tmp_frig -> num > 0)
    {
      fmol -> rigids ++;
      tmp_frig -> use = TRUE;
    }
    else
    {
      tmp_frig = get_active_rigid (j, fmol -> rigids);
      tmp_frig -> next = NULL;
      if (fmol -> rigids == 0) fmol -> first_rigid = NULL;
    }
  }
  else if (i == 6)
  {
    if (fmol -> first_tethered == NULL)
    {
      fmol -> first_tethered = init_field_tethered (fmol -> tethered, 0);
    }
    else
    {
      tmp_ftet = get_active_tethered (j, fmol -> tethered);
      tmp_ftet -> next = init_field_tethered (fmol -> tethered, 0);
      tmp_ftet -> next -> prev = g_malloc0 (sizeof*tmp_ftet -> next -> prev);
      tmp_ftet -> next -> prev = tmp_ftet;
    }
    row_id = fmol -> tethered;
    edit_parameters (i, fmol -> tethered);
    if (tmp_ftet -> num)
    {
      fmol -> tethered ++;
      tmp_ftet -> use = TRUE;
    }
    else
    {
      tmp_ftet = get_active_tethered (j, fmol -> tethered);
      tmp_ftet -> next = NULL;
      if (fmol -> tethered == 0) fmol -> first_tethered = NULL;
    }
  }
  else if (i == SEXTERN)
  {
    if (tmp_field -> first_external == NULL)
    {
      tmp_field -> first_external = init_field_external (tmp_field -> extern_fields);
    }
    else
    {
      tmp_fext = get_active_external (tmp_field -> extern_fields-1);
      tmp_fext -> next = init_field_external (tmp_field -> extern_fields);
      tmp_fext -> next -> prev = g_malloc0 (sizeof*tmp_fext -> next -> prev);
      tmp_fext -> next -> prev = tmp_fext;
    }
    row_id = tmp_field -> extern_fields;
    edit_parameters (i, tmp_field -> extern_fields);
    if (tmp_fext -> use && tmp_fext -> key > -1)
    {
      tmp_field -> extern_fields ++;
    }
    else
    {
      tmp_fext = get_active_external (tmp_field -> extern_fields);
      tmp_fext -> next = NULL;
      if (tmp_field -> extern_fields == 0) tmp_field -> first_external = NULL;
    }
  }
  else
  {
    j = i - MOLIMIT;
    row_id = tmp_field -> nbody[j];
    if (tmp_field -> first_body[j] == NULL)
    {
      tmp_field -> first_body[j] = init_field_nth_body (tmp_field -> nbody[j], j, NULL, NULL, NULL);
    }
    else
    {
      tmp_fbody = get_active_body (tmp_field -> nbody[j], j);
      tmp_fbody -> next = init_field_nth_body (tmp_field -> nbody[j], j, NULL, NULL, NULL);
      tmp_fbody -> next -> prev = g_malloc0 (sizeof*tmp_fbody -> next -> prev);
      tmp_fbody -> next -> prev = tmp_fbody;
    }
    if (j == 2)
    {
      cross = NULL;
      cross = alloctdouble (tmp_field -> nbody[j]+1, tmp_field -> nbody[j]+1, 3);
      if (tmp_field -> cross != NULL)
      {
        for (k=0; k<tmp_field -> nbody[j]; k++)
        {
          for (l=0; l<tmp_field -> nbody[j]; l++)
          {
            for (m=0; m<3; m++) cross[k][l][m] = cross[l][k][m] = tmp_field -> cross[k][l][m];
          }
        }
      }
      else
      {
        tmp_field -> cross = alloctdouble (1, 1, 3);
      }
    }
    edit_parameters (i, tmp_field -> nbody[j]);
    save_it = FALSE;
    k = body_at (j);
    for (l=0; l<k; l++) if (tmp_fbody -> na[l] > -1) save_it = TRUE;
    if (save_it)
    {
      tmp_field -> nbody[j] ++;
      for (l=0; l<k; l++) if (tmp_fbody -> na[l] < 0) tmp_fbody -> use = FALSE;
      if (j == 2) check_tersoffs (tmp_fbody -> id, tmp_fbody -> key);
    }
    else
    {
      tmp_fbody = get_active_body (tmp_field -> nbody[j], j);
      tmp_fbody -> next = NULL;
      if (tmp_field -> nbody[j] == 0) tmp_field -> first_body[j] = NULL;
    }
  }
  update_field_trees ();
}

/*!
  \fn G_MODULE_EXPORT void remove_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief remove field property callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void remove_field_prop (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  int i, j, k, l, m, n, o;
  i = GPOINTER_TO_INT (data);
  gchar * str;
  if (i < MOLIMIT)
  {
    j = combo_get_active (combo_mol[i-1]);
    tmp_fmol = get_active_field_molecule (j);
    str = g_strdup_printf ("Delete %s N°%d from molecule %s, is this correct ?", elemts[i], row_id+1, tmp_fmol -> name);
  }
  else
  {
    str = g_strdup_printf ("Delete %s, is this correct ?", elemts[i]);
  }
  selection_confirmed = FALSE;
  field_question (str, G_CALLBACK(confirm_selection), NULL);
  g_free (str);
  if (selection_confirmed)
  {
    if (i == 2)
    {
      tmp_fshell = get_active_shell (j, row_id);
      if (tmp_fshell == tmp_fmol -> first_shell)
      {
        if (tmp_fshell -> next != NULL)
        {
          tmp_fmol -> first_shell = tmp_fmol -> first_shell -> next;
          tmp_fmol -> first_shell -> prev = NULL;
        }
        else
        {
          tmp_fmol -> first_shell = NULL;
        }
      }
      else if (tmp_fshell -> next == NULL)
      {
        tmp_fshell = tmp_fshell -> prev;
        tmp_fshell -> next = NULL;
      }
      else
      {
        tmp_fshell -> prev -> next = tmp_fshell -> next;
        tmp_fshell -> next -> prev = tmp_fshell -> prev;
      }
      tmp_fmol -> shells --;
      tmp_fshell = tmp_fmol -> first_shell;
      for (j=0; j<tmp_fmol -> shells; j++)
      {
        tmp_fshell -> id = j;
        if (tmp_fshell -> next != NULL) tmp_fshell = tmp_fshell -> next;
      }
    }
    else if (i == 3)
    {
      tmp_fcons = get_active_constraint (j, row_id);
      if (tmp_fcons == tmp_fmol -> first_constraint)
      {
        if (tmp_fcons -> next != NULL)
        {
          tmp_fmol -> first_constraint = tmp_fmol -> first_constraint -> next;
          tmp_fmol -> first_constraint -> prev = NULL;
        }
        else
        {
          tmp_fmol -> first_constraint = NULL;
        }
      }
      else if (tmp_fcons -> next == NULL)
      {
        tmp_fcons = tmp_fcons -> prev;
        tmp_fcons -> next = NULL;
      }
      else
      {
        tmp_fcons -> prev -> next = tmp_fcons -> next;
        tmp_fcons -> next -> prev = tmp_fcons -> prev;
      }
      tmp_fmol -> constraints --;
      tmp_fcons = tmp_fmol -> first_constraint;
      for (j=0; j<tmp_fmol -> constraints; j++)
      {
        tmp_fcons -> id = j;
        if (tmp_fcons -> next != NULL) tmp_fcons = tmp_fcons -> next;
      }
    }
    else if (i == 4)
    {
      tmp_fpmf = get_active_pmf (j, row_id);
      if (tmp_fpmf == tmp_fmol -> first_pmf)
      {
        if (tmp_fpmf -> next != NULL)
        {
          tmp_fmol -> first_pmf = tmp_fmol -> first_pmf -> next;
          tmp_fmol -> first_pmf -> prev = NULL;
        }
        else
        {
          tmp_fmol -> first_pmf = NULL;
        }
      }
      else if (tmp_fpmf -> next == NULL)
      {
        tmp_fpmf = tmp_fpmf -> prev;
        tmp_fpmf -> next = NULL;
      }
      else
      {
        tmp_fpmf -> prev -> next = tmp_fpmf -> next;
        tmp_fpmf -> next -> prev = tmp_fpmf -> prev;
      }
      tmp_fmol -> pmfs --;
      tmp_fpmf = tmp_fmol -> first_pmf;
      for (j=0; j<tmp_fmol -> pmfs; j++)
      {
        tmp_fpmf -> id = j;
        if (tmp_fpmf -> next != NULL) tmp_fpmf = tmp_fpmf -> next;
      }
    }
    else if (i == 5)
    {
      tmp_frig = get_active_rigid (j, row_id);
      if (tmp_frig == tmp_fmol -> first_rigid)
      {
        if (tmp_frig -> next != NULL)
        {
          tmp_fmol -> first_rigid = tmp_fmol -> first_rigid -> next;
          tmp_fmol -> first_rigid -> prev = NULL;
        }
        else
        {
          tmp_fmol -> first_rigid = NULL;
        }
      }
      else if (tmp_frig -> next == NULL)
      {
        tmp_frig = tmp_frig -> prev;
        tmp_frig -> next = NULL;
      }
      else
      {
        tmp_frig -> prev -> next = tmp_frig -> next;
        tmp_frig -> next -> prev = tmp_frig -> prev;
      }
      tmp_fmol -> rigids --;
      tmp_frig = tmp_fmol -> first_rigid;
      for (j=0; j<tmp_fmol -> rigids; j++)
      {
        tmp_frig -> id = j;
        if (tmp_frig -> next != NULL) tmp_frig = tmp_frig -> next;
      }
    }
    else if (i == 6)
    {
      tmp_ftet = get_active_tethered (j, row_id);
      if (tmp_ftet == tmp_fmol -> first_tethered)
      {
        if (tmp_ftet -> next != NULL)
        {
          tmp_fmol -> first_tethered = tmp_fmol -> first_tethered -> next;
          tmp_fmol -> first_tethered -> prev = NULL;
        }
        else
        {
          tmp_fmol -> first_tethered = NULL;
        }
      }
      else if (tmp_ftet -> next == NULL)
      {
        tmp_ftet = tmp_ftet -> prev;
        tmp_ftet -> next = NULL;
      }
      else
      {
        tmp_ftet -> prev -> next = tmp_ftet -> next;
        tmp_ftet -> next -> prev = tmp_ftet -> prev;
      }
      tmp_fmol -> tethered --;
      tmp_ftet = tmp_fmol -> first_tethered;
      for (j=0; j<tmp_fmol -> tethered; j++)
      {
        tmp_ftet -> id = j;
        if (tmp_ftet -> next != NULL) tmp_ftet = tmp_ftet -> next;
      }
    }
    else if (i == SEXTERN)
    {
      tmp_fext = get_active_external (row_id);
      if (tmp_fext == tmp_field -> first_external)
      {
        if (tmp_fext -> next != NULL)
        {
          tmp_field -> first_external = tmp_field -> first_external -> next;
          tmp_field -> first_external -> prev = NULL;
        }
        else
        {
          tmp_field -> first_external = NULL;
        }
      }
      else if (tmp_fext -> next == NULL)
      {
        tmp_fext = tmp_fext -> prev;
        tmp_fext -> next = NULL;
      }
      else
      {
        tmp_fext -> prev -> next = tmp_fext -> next;
        tmp_fext -> next -> prev = tmp_fext -> prev;
      }
      tmp_field -> extern_fields --;
      tmp_fext = tmp_field -> first_external;
      for (k=0; k<tmp_field -> extern_fields; k++)
      {
        tmp_fext -> id = k;
        if (tmp_fext -> next != NULL) tmp_fext = tmp_fext -> next;
      }
    }
    else
    {
      j = i - MOLIMIT;
      tmp_fbody = get_active_body (row_id, j);
      if (j == 2)
      {
        cross = NULL;
        if (tmp_field -> nbody[j] > 1)
        {
          cross = alloctdouble (tmp_field -> nbody[j]-1, tmp_field -> nbody[j]-1, 3);
          m = -1;
          for (k=0; k<tmp_field -> nbody[j]; k++)
          {
            if (k != tmp_fbody -> id)
            {
              m ++;
              n = -1;
              for (l=0; l<tmp_field -> nbody[j]; l++)
              {
                if (l != tmp_fbody -> id)
                {
                  n ++;
                  for (o=0; o<3; o++) cross[m][n][o] = cross[n][m][o] = tmp_field -> cross[k][l][o];
                }
              }
            }
          }
        }
      }
      if (tmp_fbody == tmp_field -> first_body[j])
      {
        if (tmp_fbody -> next != NULL)
        {
          tmp_field -> first_body[j] = tmp_field -> first_body[j] -> next;
          tmp_field -> first_body[j] -> prev = NULL;
        }
        else
        {
          tmp_field -> first_body[j] = NULL;
        }
      }
      else if (tmp_fbody -> next == NULL)
      {
        tmp_fbody = tmp_fbody -> prev;
        tmp_fbody -> next = NULL;
      }
      else
      {
        tmp_fbody -> prev -> next = tmp_fbody -> next;
        tmp_fbody -> next -> prev = tmp_fbody -> prev;
      }
      tmp_field -> nbody[j] --;
      g_free (tmp_fbody);
      tmp_fbody = tmp_field -> first_body[j];
      k = 0;
      while (tmp_fbody)
      {
        tmp_fbody -> id = k;
        k ++;
        tmp_fbody = tmp_fbody -> next;
      }

      if (j == 2) check_tersoffs (-1, tmp_field -> first_body[j] -> key);
    }
    update_field_trees ();
  }
}
