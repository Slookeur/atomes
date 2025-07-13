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
* @file cpmd_nose.c
* @short Functions to prepare the thermostat(s) configuration widgets for QM / QM-MM input files
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cpmd_nose.c'
*
* Contains:
*

 - The functions to prepare the thermostat(s) configuration widgets for QM / QM-MM input files

*
* List of functions:

  int get_num_thermo ();
  int is_not_thermostated (int at, int therm);
  int is_fixed_atom (int at);
  int in_dummy (int at, int id);

  gboolean are_all_atoms_thermostated ();
  gboolean was_it_selected (int id, int at);

  void set_going_forward ();
  void clean_nose_widgets ();
  void thermo_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void thermo_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void remove_nose_thermostat (int num_to_remove);
  void init_thermostats (int type, int elec);
  void clean_thermostat (int new_type);
  void nose_parameters (GtkWidget * vbox, int id, int jd, gchar ** la, gchar ** lb);
  void atom_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void atom_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void fill_thermo_atom_model (int therm);
  void select_atom_from_model (int therm);
  void create_selection_button (GtkWidget * box, int num, int id, gpointer data);
  void create_nose_thermo_param_box (int therm_id);
  void create_selection_combo (int id, int num, int type, GCallback handler);
  void add_thermostat (int extra);
  void prepare_therm_ions ();
  void prepare_therm_elec ();
  void thermo_type_box (GtkWidget * vbox, gchar * str, int id);

  G_MODULE_EXPORT void select_thermo (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void run_remove_nose_thermostat (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void cpmd_select_atom_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void cp2k_select_coord_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void select_atoms_not_thermostated (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void select_atoms_not_thermostated (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void run_select_atom_from_model (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void atom_selection_button (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void changed_nose_thermo_id_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void add_or_remove_thermostat (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void update_thermo_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void changed_thermo_box_nose (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void changed_thermo_box (GtkComboBox * box, gpointer data);

  GtkWidget * create_nose_box (int n);
  GtkWidget * thermo_box ();

  thermostat * get_thermo ();
  thermostat * get_active_thermostat (int id);
  thermostat * init_thermo (int id, int type, int sys);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "calc.h"
#include "cpmd.h"
#include "cp2k.h"

extern void print_the_section (int s, int p, GtkTextBuffer * buffer);
extern ColRGBA init_color (int id, int numid);
extern void proj_unselect_all_atoms ();
extern GtkWidget * cpmd_box (GtkWidget * box, gchar * lab, int v_space, int h_space, int dim);
extern dummy_atom * get_active_dummy (int id);
extern void create_dummy_param_box (int dummy_id);

char * c_thermo[2][CP2NTHERM][4] = {{{"Initial temperature: ", " ", " ", " "},
                                     {"Target temperature: ","Tolerance: ", " ", " "},
                                     {"Target temperature: ", "Target frequency: ", " ", " "},
                                     {" ", " ", " ", " "},
                                     {" ", " ", " ", " "}},
                                    {{" ", " ", " ", " "},
                                     {"Time constant (Langevin): ", "Time constant (Nosë-Hoover): ", "Mass: ", "Chi: "},
                                     {"Time constant: ", " ", " ", " "},
                                     {" ", " ", " ", " "},
                                     {"Length: ", "Multiple time steps: ", "Time constant: ", "Yoshida integrator: "}}};

char * u_thermo[2][CP2NTHERM][4] = {{{" K", " ", " ", " "},
                                     {" K", " K", " ", " "},
                                     {" K", " cm<sup>-1</sup>", " ", " "},
                                     {" ", " ", " ", " "},
                                     {" ", " ", " ", " "}},
                                    {{" ", " ", " ", " "},
                                     {" fs", " fs", " fs<sup>-1</sup>", " fs<sup>-1</sup>"},
                                     {" fs", " ", " ", " "},
                                     {" ", " ", " ", " "},
                                     {" ", " ", " fs", " "}}};

char * ue_thermo[CP2NTHERM][4] = {{" a.u.", " ", " ", " "},
                                  {" a.u.", " ", " ", " "},
                                  {" a.u.", " a.u.", " ", " "},
                                  {" a.u.", " cm<sup>-1</sup>", " ", " "}};

int v_thermo[2][CP2NTHERM] = {{1, 2, 2, 0, 0}, {0, 4, 1, 0, 4}};

double d_thermo[2][CP2NTHERM][4] = {{{ 300.0, 0.0, 0.0, 0.0},
                                     { 300.0, 20.0, 0.0, 0.0},
                                     { 300.0, 200.0, 0.0, 0.0},
                                     { 0.0, 0.0, 0.0, 0.0},
                                     { 0.0, 0.0, 0.0, 0.0}},
                                    {{ 0.0, 0.0, 0.0, 0.0},
                                     { 100.0, 100.0, 1.0, 1.0},
                                     { 100.0, 0.0, 0.0, 0.0},
                                     { 0.0, 0.0, 0.0, 0.0},
                                     { 3.0, 2.0, 1000.0, 3.0}}};
GtkWidget * sel_but[3];
GtkWidget * sel_img[3];
GtkWidget * electron_box;
GtkWidget * therm_ions;
GtkWidget * therm_elec;
GtkWidget * therm_param_ions;
GtkWidget * therm_param_elec;
GtkWidget * nose_box;
GtkWidget * combo_id[2];
GtkWidget * combo_id_box[2];
GtkWidget * nose_id_box[2];
GtkCellRenderer * thermo_renderer[6];
GtkTreeViewColumn * thermo_col[6];
int n_therm;
int at_col;
int * old_thermo;
int ** old_fixed;
int num_cpmd_objects;
int the_therm;
gboolean fixco;
GtkTreeStore * add_model;

/*!
  \fn thermostat * get_thermo ()

  \brief get QM / QM-MM ions thermostat
*/
thermostat * get_thermo ()
{
  if (is_cpmd)
  {
    return tmp_cpmd -> ions_thermostat;
  }
  else
  {
    return tmp_cp2k -> ions_thermostat;
  }
}

/*!
  \fn int get_num_thermo ()

  \brief get the number of thermostat(s)
*/
int get_num_thermo ()
{
  if (is_cpmd)
  {
    return tmp_cpmd -> thermostats;
  }
  else
  {
    return tmp_cp2k -> thermostats;
  }
}

/*!
  \fn gboolean are_all_atoms_thermostated ()

  \brief are all atom(s) in the model thermostated ?
*/
gboolean are_all_atoms_thermostated ()
{
  int i, j;
  thermostat * thermo = get_thermo();
  j = 0;
  if (thermo -> sys < LOCAL) return TRUE;
  for (i=0; i<get_num_thermo(); i++)
  {
    j += thermo -> natoms;
    if (thermo -> next != NULL) thermo = thermo -> next;
  }
  if (j == qm_proj -> natomes)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn void set_going_forward ()

  \brief QM / QM-MM assistant going forward on thermostat page
*/
void set_going_forward ()
{
  GtkAssistant * assist = GTK_ASSISTANT(qm_assistant);
  GtkWidget * page = gtk_assistant_get_nth_page (assist, (is_cpmd) ? 3 : 5);
  gtk_assistant_set_page_complete (assist, page, are_all_atoms_thermostated ());
}

/*!
  \fn void clean_nose_widgets ()

  \brief clean thermostat widgets
*/
void clean_nose_widgets ()
{
  combo_id_box[0] = destroy_this_widget (combo_id_box[0]);
  combo_id[0] = destroy_this_widget (combo_id[0]);
  nose_id_box[1] = destroy_this_widget (nose_id_box[1]);
  nose_id_box[0] = destroy_this_widget (nose_id_box[0]);
  nose_box = destroy_this_widget (nose_box);
}

/*!
  \fn thermostat * get_active_thermostat (int id)

  \brief get thermostat using id

  \param id the thermostat id
*/
thermostat * get_active_thermostat (int id)
{
  if (id < 0)
  {
    return tmp_cpmd -> elec_thermostat;
  }
  else
  {
    thermostat * thermo = get_thermo ();
    while (thermo -> id != id)
    {
      if (thermo -> next != NULL) thermo = thermo -> next;
    }
    return thermo;
  }
}

/*!
  \fn G_MODULE_EXPORT void select_thermo (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select QM / QM-MM thermostat toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*
*/
G_MODULE_EXPORT void select_thermo (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  GtkTreeStore ** model = (GtkTreeStore **)data;
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(* model), & iter, path);
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    get_active_thermostat (old_thermo[n_therm-1]) -> show = FALSE;
    old_thermo[n_therm-1] = -1;
    n_therm --;
    gtk_tree_store_set (* model, & iter, 5, 0, -1);
    //toviz.c = 0;
  }
  else
  {
    n_therm ++;
    gtk_tree_store_set (* model, & iter, 5, 1, -1);
    gtk_tree_model_get (GTK_TREE_MODEL(* model), & iter, 0, & old_thermo[n_therm-1], -1);
    old_thermo[n_therm-1] --;
    get_active_thermostat (old_thermo[n_therm-1]) -> show = TRUE;
    //toviz.c = 1;
  }
  // Viz
}

/*!
  \fn void thermo_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color in the QM / QM-MM thermostat selection tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void thermo_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  i = abs(i);
  gtk_tree_model_get (mod, iter, 5, & j, -1);
  set_renderer_color (j, renderer, init_color (i-1, num_cpmd_objects));
}

/*!
  \fn void thermo_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer in the QM / QM-MM thermostat selection tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void thermo_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  gtk_tree_model_get (mod, iter, 0, & j, -1);
  if (j > 0 && i != 4)
  {
    gtk_cell_renderer_set_visible (renderer, TRUE);
    if (i < 5) thermo_set_color (col, renderer, mod, iter, data);
  }
  else if (j > 0)
  {
    gtk_cell_renderer_set_visible (renderer, FALSE);
  }
  else if (i == 4)
  {
    gtk_cell_renderer_set_visible (renderer, TRUE);
    thermo_set_color (col, renderer, mod, iter, data);
  }
  else
  {
    gtk_cell_renderer_set_visible (renderer, FALSE);
  }
}

GtkWidget * create_nose_box (int n);

/*!
  \fn G_MODULE_EXPORT void run_remove_nose_thermostat (GtkDialog * dialog, gint response_id, gpointer data)

  \brief remove thermostat(s) - running the dialog

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_remove_nose_thermostat (GtkDialog * dialog, gint response_id, gpointer data)
{
  int i, j;
  int num_to_remove = GPOINTER_TO_INT(data);
  gboolean done = FALSE;
  gchar * str;
  thermostat * thermo;
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (n_therm == num_to_remove)
      {
        done = TRUE;
        // Now we remove all selected thermostats:
        for (i=0; i<n_therm; i++)
        {
          thermo = get_thermo();
          for (j=0; j<get_num_thermo(); j++)
          {
            if (thermo -> id == old_thermo[i])
            {
              if (thermo -> next != NULL)
              {
                if (thermo -> prev != NULL)
                {
                  thermo -> next -> prev = thermo -> prev;
                  thermo -> prev -> next = thermo -> next;
                }
                else
                {
                  tmp_cpmd -> ions_thermostat = thermo -> next;
                  tmp_cpmd -> ions_thermostat -> prev = NULL;
                }
              }
              else
              {
                thermo -> prev -> next = NULL;
              }
              g_free (thermo);
              break;
            }
            if (thermo -> next != NULL) thermo = thermo -> next;
          }
        }
        if (is_cpmd)
        {
          tmp_cpmd -> thermostats -= num_to_remove;
        }
        else
        {
          tmp_cp2k -> thermostats -= num_to_remove;
        }
        thermo = get_thermo();
        for (j=0; j<get_num_thermo(); j++)
        {
          thermo -> id = j;
          if (thermo -> next != NULL) thermo = thermo -> next;
        }
        clean_nose_widgets ();
        nose_box = create_nose_box (get_thermo() ->  type);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, therm_param_ions, nose_box, FALSE, FALSE, 0);
        //int j;
        //for (j=1; j<4; j++) print_the_section (j, 0, qmbuffer[j]););
        show_the_widgets (therm_param_ions);
      }
      else
      {
        str = g_strdup_printf ("You must select %d thermostat(s) to be deleted !", num_to_remove);
        show_warning (str, qm_assistant);
        g_free (str);
      }
      break;
    default:
      done = TRUE;
      break;
  }
  if (done) destroy_this_dialog (dialog);
}

/*!
  \fn void remove_nose_thermostat (int num_to_remove)

  \brief remove thermostat(s) - creating the dialog

  \param num_to_remove the number of thermostat(s) to remove
*/
void remove_nose_thermostat (int num_to_remove)
{
  int i, j, k;
  gchar * str;
  // int cpmd_object = 0;
  str = g_strdup_printf ("Select the %d thermostat(s) to be removed", num_to_remove);
  GtkWidget * rthermo = dialogmodal (str, GTK_WINDOW(qm_assistant));
  g_free (str);
  gtk_dialog_add_button (GTK_DIALOG(rthermo), "Apply", GTK_RESPONSE_APPLY);

  gchar * mol_title[6] = {"Id", "Target T°", "Frequency", "Atom(s)", "      ", "Select"};
  gchar * ctype[6] = {"text", "text", "text", "text", "text", "active"};
  GType col_type[6] = {G_TYPE_INT, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN};

  GtkTreeIter thermo_level, atom_level;
  n_therm = 0;
  old_thermo = allocint(get_num_thermo());
  GtkTreeStore * remove_model = gtk_tree_store_newv (6, col_type);
  GtkWidget * remove_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(remove_model));
  for (i=0; i<6; i++)
  {
    if (i < 5)
    {
      thermo_renderer[i] = gtk_cell_renderer_text_new ();
    }
    else
    {
      thermo_renderer[i] = gtk_cell_renderer_toggle_new ();
      gtk_cell_renderer_toggle_set_radio (GTK_CELL_RENDERER_TOGGLE(thermo_renderer[i]), TRUE);
      g_signal_connect (G_OBJECT(thermo_renderer[i]), "toggled", G_CALLBACK(select_thermo), & remove_model);
    }
    thermo_col[i] = gtk_tree_view_column_new_with_attributes (mol_title[i], thermo_renderer[i], ctype[i], i, NULL);
    gtk_tree_view_column_set_alignment (thermo_col[i], 0.5);
    gtk_tree_view_append_column (GTK_TREE_VIEW(remove_tree), thermo_col[i]);
    gtk_tree_view_column_set_cell_data_func (thermo_col[i], thermo_renderer[i], thermo_set_visible, GINT_TO_POINTER(i), NULL);
  }
  // fill model
  thermostat * thermo = get_thermo();
  for (i=0; i<get_num_thermo(); i++)
  {
    gtk_tree_store_append (remove_model, & thermo_level, NULL);
    gtk_tree_store_set (remove_model, & thermo_level, 0, i+1,
                                                      1, thermo -> params[0],
                                                      2, thermo -> params[1],
                                                      3, thermo -> natoms,
                                                      4, " ",
                                                      5, 0, -1);
    for (j=0; j<thermo -> natoms; j++)
    {
      gtk_tree_store_append (remove_model, & atom_level, & thermo_level);
      k = qm_proj -> atoms[0][thermo -> list[j]].sp;
      str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(qm_proj -> chemistry -> label[k]), thermo -> list[j]+1);
      gtk_tree_store_set (remove_model, & atom_level, 0, -(i+1), 1, 0.0, 2, 0.0, 3, 0, 4, str, 5, 0, -1);
      g_free (str);
    }
    if (thermo -> next != NULL) thermo = thermo -> next;
  }
  num_cpmd_objects = get_num_thermo();
  g_object_unref (remove_model);
  i = ((num_cpmd_objects+1)*40 < 500) ? (num_cpmd_objects+1)*40 : 500;
  GtkWidget * scrollsets = create_scroll (dialog_get_content_area (rthermo), 450, i, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrollsets, remove_tree);
  run_this_gtk_dialog (rthermo, G_CALLBACK(run_remove_nose_thermostat), GINT_TO_POINTER(num_to_remove));
}

/*!
  \fn thermostat * init_thermo (int id, int type, int sys)

  \brief initialize new thermostat

  \param id the new thermostat id
  \param type the type of thermostat
  \param sys the thermostat system
*/
thermostat * init_thermo (int id, int type, int sys)
{
  thermostat * thermo = g_malloc0 (sizeof*thermo);
  thermo -> id = id;
  thermo -> type = type;
  thermo -> sys = sys;
  thermo -> show = FALSE;
  if (id < 0)
  {
    thermo -> params[0] = 0.5;
    thermo -> params[1] = (type == 0) ? 0.2: 200.0;
  }
  else
  {
    int i;
    for (i=0; i<v_thermo[!is_cpmd][type]; i++)
    {
      thermo -> params[i] = d_thermo[!is_cpmd][type][i];
    }
  }
  return thermo;
}

/*!
  \fn void init_thermostats (int type, int elec)

  \brief initialize thermostat(s)

  \param type the type of thermostat
  \param elec 1 = fictitious electronic thermostat
*/
void init_thermostats (int type, int elec)
{
  //int i = (type == ULTRA) ? qm_proj -> nspec: 1;
  if (is_cpmd)
  {
    tmp_cpmd -> thermostats = 1;
    tmp_cpmd -> ions_thermostat = init_thermo (0, type, 0);
  }
  else
  {
    tmp_cp2k -> thermostats = 1;
    tmp_cp2k -> ions_thermostat = init_thermo (0, type, 0);
  }
  if (is_cpmd && elec) tmp_cpmd -> elec_thermostat = init_thermo (-1, type, 0);
}

/*!
  \fn void clean_thermostat (int new_type)

  \brief free thermostat data, then initialiaze new type of thermostat

  \param new_type the new type of thermostat
*/
void clean_thermostat (int new_type)
{
  int i;
  thermostat * thermo = get_thermo ();
  if (thermo -> sys > 0)
  {
    for (i=0; i<get_num_thermo(); i++)
    {
      if (thermo -> next != NULL) thermo = thermo -> next;
    }
    for (i=0; i<get_num_thermo(); i++)
    {
      if (thermo -> prev != NULL)
      {
        thermo = thermo -> prev;
        g_free (thermo -> next);
      }
    }
  }
  g_free (thermo);
  init_thermostats (new_type, 0);
}

G_MODULE_EXPORT void update_thermo_parameter (GtkEntry * res, gpointer data);

/*!
  \fn void nose_parameters (GtkWidget * vbox, int id, int jd, gchar ** la, gchar ** lb)

  \brief create thermostat parameters configuration widgets

  \param vbox the GtkWidget sending the signal
  \param id thermostat id, -2 if fictitious electronic
  \param jd number of parameter(s) for this thermostat
  \param la thermostat parameter name(s)
  \param lb thermostat parameter unit(s)
*/
void nose_parameters (GtkWidget * vbox, int id, int jd, gchar ** la, gchar ** lb)
{
  thermostat * thermo = get_active_thermostat (id);
  //gchar * itemp[2]={"Initial temperature:", "Target temperature:"};
  GtkWidget * hbox;
  GtkWidget * widg;
  int i, j;
  i = (id < 0) ? id : 0;
  for (j=0; j<jd; j++)
  {
    hbox = cpmd_box (vbox, la[j], 5, (is_cpmd) ? 20 : 50, 220);
    widg = create_entry (G_CALLBACK(update_thermo_parameter), 100, 15, FALSE, GINT_TO_POINTER(i+j));
    update_entry_double (GTK_ENTRY(widg), thermo -> params[j]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(lb[j], -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
}

/*!
  \fn G_MODULE_EXPORT void cpmd_select_atom_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select CPMD atom id toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*
*/
G_MODULE_EXPORT void cpmd_select_atom_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  GtkTreeStore ** model = (GtkTreeStore **)data;
  GtkTreeIter iter;
  int i, j;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(* model), & iter, path);

  gtk_tree_model_get (GTK_TREE_MODEL(* model), & iter, at_col, & i, -1);
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    j = 0;
    n_therm --;
    old_thermo[i-1] = 0;
    qm_view -> picked --;
  }
  else
  {
    j = 1;
    n_therm ++;
    old_thermo[i-1] = 1;
    qm_view -> picked ++;
  }
  qm_proj -> atoms[0][i-1].pick[0] = j;
  qm_proj -> atoms[0][i-1].label[0] = j;
  init_default_shaders (qm_view);
  gtk_tree_store_set (* model, & iter, 3, j, -1);
}

/*!
  \fn G_MODULE_EXPORT void cp2k_select_coord_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select CP2K fixed id toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*
*/
G_MODULE_EXPORT void cp2k_select_coord_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  int i, j;
  j = GPOINTER_TO_INT(data);
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(add_model), & iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(add_model), & iter, at_col, & i, -1);
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    old_fixed[i-1][j-4] = 0;
  }
  else
  {
    old_fixed[i-1][j-4] = 1;
  }
  gtk_tree_store_set (add_model, & iter, (fixco) ? j : j-2, old_fixed[i-1][j-4], -1);
}

/*!
  \fn void atom_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color in the CPMD atom selection tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void atom_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j, k;
  gtk_tree_model_get (mod, iter, 1, & i, -1);
  gtk_tree_model_get (mod, iter, 3, & j, -1);
  set_renderer_color (j, renderer, init_color (i-1, num_cpmd_objects));
  k = GPOINTER_TO_INT(data);
  if (k == 2)
  {
    gchar * str;
    gtk_tree_model_get (mod, iter, 2, & str, -1);
    g_object_set (renderer, "markup", str, NULL, NULL);
    g_free (str);
  }
}

/*!
  \fn void atom_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer in the CPMD atom selection tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void atom_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j, k;
  i = GPOINTER_TO_INT(data);
  gtk_tree_model_get (mod, iter, at_col, & j, -1);
  if (j == 0 && i != 0)
  {
    gtk_cell_renderer_set_visible (renderer, FALSE);
  }
  else if (j == 0)
  {
    gtk_cell_renderer_set_visible (renderer, TRUE);
  }
  else if (j > 0 && i == 0)
  {
    gtk_cell_renderer_set_visible (renderer, FALSE);
  }
  else if (j > 0)
  {
    if (i < 4)
    {
      gtk_cell_renderer_set_visible (renderer, TRUE);
    }
    else
    {
      gtk_tree_model_get (mod, iter, 3, & k, -1);
      gtk_cell_renderer_set_visible (renderer, k);
    }
    if (i < 3) atom_set_color (col, renderer, mod, iter, data);
  }
}

/*!
  \fn int is_not_thermostated (int at, int therm)

  \brief is atom thermostated ?

  \param at the atom id
  \param therm the thermostat id
*/
int is_not_thermostated (int at, int therm)
{
  int i, j;
  thermostat * thermo = get_thermo ();
  for (i=0; i<get_num_thermo(); i++)
  {
    for (j=0; j<thermo -> natoms; j++)
    {
      if (thermo -> list[j] == at)
      {
        if (i == therm)
        {
          return 2;
        }
        else
        {
          return 0;
        }
      }
    }
    if (thermo -> next != NULL) thermo = thermo -> next;
  }
  return 1;
}

/*!
  \fn int is_fixed_atom (int at)

  \brief is atom fixed ?

  \param at the atom id
*/
int is_fixed_atom (int at)
{
  int i;
  if (is_cpmd)
  {
    for (i=0; i<tmp_cpmd -> fixat; i++)
    {
      if (tmp_cpmd -> fixlist[i] == at)
      {
        return i+2;
      }
    }
  }
  else
  {
    for (i=0; i<tmp_cp2k -> fixat[0]; i++)
    {
      if (tmp_cp2k -> fixlist[0][i] == at)
      {
        return i+2;
      }
    }
  }
  return 1;
}

/*!
  \fn int in_dummy (int at, int id)

  \brief is atom in dummy ?

  \param at the atom id
  \param id the dummy list id
*/
int in_dummy (int at, int id)
{
  dummy_atom * dummy = get_active_dummy (-(id+2));
  int i;
  if (dummy -> natoms == qm_proj -> natomes) return 2;
  for (i=0; i<dummy -> natoms; i++)
  {
    if (dummy -> list[i] == at) return 2;
  }
  return 1;
}

/*!
  \fn gboolean was_it_selected (int id, int at)

  \brief was this atom already selected ?

  \param id the selection type
  \param at the atom id
*/
gboolean was_it_selected (int id, int at)
{
  if (id < -1)
  {
    return in_dummy (at, id);
  }
  else if (id == -1)
  {
    return is_fixed_atom (at);
  }
  else
  {
    return is_not_thermostated (at, id);
  }
}

/*!
  \fn void fill_thermo_atom_model (int therm)

  \brief fill thermostat atom model

  \param therm the thermostat id
*/
void fill_thermo_atom_model (int therm)
{
  int i, j, k, l, m;
  gchar * str;
  GtkTreeIter spec_level;
  GtkTreeIter atom_level;
  k = 0;
  for (i=0; i< qm_proj -> nspec; i++)
  {
    gtk_tree_store_append (add_model, & spec_level, NULL);
    gtk_tree_store_set (add_model, & spec_level, 0, qm_proj -> chemistry -> label[i], 1, 0, 3, 0, 4, 0, -1);
    for (j=0; j< qm_proj -> natomes; j++)
    {
      if (qm_proj -> atoms[0][j].sp == i)
      {
        k ++;
        l = was_it_selected (therm, j);
        if (fixco)
        {
          if (is_cpmd)
          {
            if (tmp_cpmd -> fixat > 0)
            {
              for (m=0; m<tmp_cpmd -> fixat; m++)
              {
                if (qm_proj -> atoms[0][j].coord[2] == tmp_cpmd-> fixlist[m])
                {
                  l = 0;
                  break;
                }
              }
            }
          }
          else
          {
            if (tmp_cp2k -> fixat[1] > 0)
            {
              for (m=0; m<tmp_cp2k -> fixat[1]; m++)
              {
                if (qm_proj -> atoms[0][j].coord[2] == tmp_cp2k -> fixlist[1][m])
                {
                  l = 0;
                  break;
                }
              }
            }
          }
        }
        if (l)
        {
          gtk_tree_store_append (add_model, & atom_level, & spec_level);
          str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(qm_proj -> chemistry -> label[i]), j+1);
          if (fixco)
          {
            gtk_tree_store_set (add_model, & atom_level, 0, 0, 1, k, 2, str, 3, old_thermo[j],
                                4, old_fixed[j][0], 5, old_fixed[j][1], 6, old_fixed[j][2], at_col, j+1, -1);
          }
          else
          {
            gtk_tree_store_set (add_model, & atom_level, 0, 0, 1, k, 2, str, 3, old_thermo[j], at_col, j+1, -1);
          }
          g_free (str);
        }
      }
    }
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void select_atoms_not_thermostated (GtkCheckButton * but, gpointer data)

  \brief select atom to thermostat toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_atoms_not_thermostated (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void select_atoms_not_thermostated (GtkToggleButton * but, gpointer data)

  \brief select atom to thermostat toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_atoms_not_thermostated (GtkToggleButton * but, gpointer data)
#endif
{
  int h, i, j, k, l, m;
  h = GPOINTER_TO_INT (data);
  j = button_get_status ((GtkWidget *)but);
  for (i=0; i<qm_proj -> natomes; i++)
  {
    m = 0;
    if (j)
    {
      k = l = m = 1;
      if (was_it_selected(h, i) != 1)
      {
        m = 0;
      }
    }
    else if (old_thermo[i])
    {
      k = -1;
      l = 0;
      m = 1;
    }
    if (m)
    {
      n_therm += k;
      old_thermo[i] = l;
      if (l) qm_view -> picked ++;
      qm_proj -> atoms[0][i].pick[0] = l;
      qm_proj -> atoms[0][i].label[0] = l;
    }
  }
  gtk_tree_store_clear (add_model);
  fill_thermo_atom_model (h);
  init_default_shaders (qm_view);
}

/*!
  \fn G_MODULE_EXPORT void run_select_atom_from_model (GtkDialog * dialog, gint response_id, gpointer data)

  \brief select atom from model - running the dialgo

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_select_atom_from_model (GtkDialog * dialog, gint response_id, gpointer data)
{
  int i, j;
  gboolean done = FALSE;
  gchar * str;
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (n_therm > 1)
      {
        str = g_strdup_printf ("%d atoms have been selected !", n_therm);
      }
      else if (n_therm)
      {
        str = g_strdup_printf ("A single atom has been selected !");
      }
      else
      {
        str = g_strdup_printf ("Not at single atom has been selected !");
      }
      str = g_strdup_printf ("%s\nis this correct ?", str);
      selection_confirmed = FALSE;
      field_question (str, G_CALLBACK(confirm_selection), NULL);
      g_free (str);
      if (selection_confirmed)
      {
        done = TRUE;
        if (fixco)
        {
          for (i=0; i<qm_proj -> natomes; i++)
          {
            if (old_thermo[i] && ! old_fixed[i][0] && ! old_fixed[i][1] && ! old_fixed[i][2])
            {
              str = g_strdup_printf ("Atom %d has been selected but not coordinates are frozen !\n"
                                     "Unselect atom %d or select coordinate(s) to freeze !", i+1, i+1);
              show_warning (str, qm_assistant);
              done = FALSE;
            }
          }
        }
      }
      thermostat * thermo;
      dummy_atom * dummy;
      if (done)
      {
        if (the_therm < -1)
        {
          dummy = get_active_dummy (-(the_therm+2));
          dummy -> natoms = n_therm;
          if (dummy -> list != NULL)
          {
            g_free (dummy -> list);
            dummy -> list = NULL;
          }
          if (n_therm > 0 && n_therm < qm_proj -> natomes)
          {
            dummy -> list = allocint (n_therm);
            j = -1;
            for (i=0; i< qm_proj -> natomes; i++)
            {
              if (old_thermo[i])
              {
                j ++;
                dummy -> list[j] = i;
              }
            }
          }
        }
        else if (the_therm == -1)
        {
          if (is_cpmd)
          {
            if (tmp_cpmd -> fixlist != NULL)
            {
              g_free (tmp_cpmd -> fixlist);
              tmp_cpmd -> fixlist = NULL;
              if (tmp_cpmd -> fixcoord != NULL)
              {
                g_free (tmp_cpmd -> fixcoord);
                tmp_cpmd -> fixcoord = NULL;
              }
            }
            if (n_therm > 0)
            {
              tmp_cpmd -> fixlist = allocint (n_therm);
              if (fixco) tmp_cpmd -> fixcoord = allocdint (n_therm, 3);
              tmp_cpmd -> fixat = n_therm;
              j = -1;
              for (i=0; i< qm_proj -> natomes; i++)
              {
                if (old_thermo[i])
                {
                  j ++;
                  tmp_cpmd -> fixlist[j] = i;
                  if (fixco)
                  {
                    tmp_cpmd -> fixcoord[j][0] = old_fixed[i][0];
                    tmp_cpmd -> fixcoord[j][1] = old_fixed[i][1];
                    tmp_cpmd -> fixcoord[j][2] = old_fixed[i][2];
                  }
                }
              }
            }
          }
          else
          {
            if (tmp_cp2k -> fixlist[0] != NULL)
            {
              g_free (tmp_cp2k -> fixlist[0]);
              tmp_cp2k -> fixlist[0] = NULL;
              if (tmp_cp2k -> fixcoord[0] != NULL)
              {
                g_free (tmp_cp2k -> fixcoord[0]);
                tmp_cp2k -> fixcoord[0] = NULL;
              }
            }
            if (n_therm > 0)
            {
              tmp_cp2k -> fixlist[0] = allocint (n_therm);
              tmp_cp2k -> fixcoord[0] = allocdint (n_therm, 3);
              tmp_cp2k -> fixat[0] = n_therm;
              j = -1;
              for (i=0; i< qm_proj -> natomes; i++)
              {
                if (old_thermo[i])
                {
                  j ++;
                  tmp_cp2k -> fixlist[0][j] = i;
                  tmp_cp2k -> fixcoord[0][j][0] = old_fixed[i][0];
                  tmp_cp2k -> fixcoord[0][j][1] = old_fixed[i][1];
                  tmp_cp2k -> fixcoord[0][j][2] = old_fixed[i][2];
                }
              }
            }
          }
        }
        else
        {
          thermo = get_active_thermostat (the_therm);
          thermo -> natoms = n_therm;
          if (thermo -> list != NULL)
          {
            g_free (thermo -> list);
            thermo -> list = NULL;
          }
          if (n_therm > 0)
          {
            thermo -> list = allocint (n_therm);
            j = -1;
            for (i=0; i< qm_proj -> natomes; i++)
            {
              if (old_thermo[i])
              {
                j ++;
                thermo -> list[j] = i;
              }
            }
          }
        }
      }
      break;
    case GTK_RESPONSE_CLOSE:
      done = TRUE;
      break;
  }
  if (done) destroy_this_dialog (dialog);
}

/*!
  \fn void select_atom_from_model (int therm)

  \brief select atom from model - creating the dialog

  \param therm the thermostat id
*/
void select_atom_from_model (int therm)
{
  int i, j, k, l, m;
  the_therm = therm;
  GtkTreeViewColumn * ato_col[8];
  GtkCellRenderer * ato_cell[8];
  gchar * ato_title[2][8] = {{"Species", "Id (*)", "Atom", "Viz.3D & Select", " "},
                             {"Species", "Id (*)", "Atom", "Viz.3D & Select", "x", "y", "z", " "}};
  gchar * ctype[2][8] = {{"text", "text", "text", "active", "text", "active", "active", "active"},
                         {"text", "text", "text", "active", "active", "active", "active", "text"}};
  GType col_type[2][8] = {{G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_INT},
                          {G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_INT}};
  gchar * str;
  at_col = 4;

  fixco = FALSE;
  qm_view -> picked = 0;
  if (! is_cpmd && the_therm == -1)
  {
    fixco = TRUE;
  }
  else if (is_cpmd)
  {
    if ((int)tmp_cpmd -> default_opts[DEFFI] == 2) fixco = TRUE;
  }
  if (the_therm < -1)
  {
    str = g_strdup_printf ("Select atom(s) to construct dummy N°%d", -(the_therm+1));
  }
  else if (the_therm == -1)
  {
    str = g_strdup_printf ("Select atom(s) to fix");
    if (fixco)
    {
      g_free (str);
      at_col = 7;
      str = g_strdup_printf ("Select atom(s) and coordinate(s) to fix");
    }
  }
  else
  {
    str = g_strdup_printf ("Add atom(s) to thermostat N°%d", the_therm+1);
  }
  GtkWidget * amol = dialogmodal (str, GTK_WINDOW(qm_assistant));
  g_free (str);
  gtk_dialog_add_button (GTK_DIALOG(amol), "Apply", GTK_RESPONSE_APPLY);
  n_therm = 0;
  i = (fixco) ? 1 : 0;
  j = (fixco) ? 8 : 5;
  add_model = gtk_tree_store_newv (j, col_type[i]);
  GtkWidget * add_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(add_model));
  for (k=0; k<j-1; k++)
  {
    if (k < 3)
    {
      ato_cell[k] = gtk_cell_renderer_text_new ();
    }
    else
    {
      ato_cell[k] = gtk_cell_renderer_toggle_new ();
      if (k==3)
      {
        g_signal_connect (G_OBJECT(ato_cell[k]), "toggled", G_CALLBACK(cpmd_select_atom_id), & add_model);
      }
      else
      {
        g_signal_connect (G_OBJECT(ato_cell[k]), "toggled", G_CALLBACK(cp2k_select_coord_id), GINT_TO_POINTER(k));
      }
    }
    ato_col[k] = gtk_tree_view_column_new_with_attributes (ato_title[i][k], ato_cell[k], ctype[i][k], k, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(add_tree), ato_col[k]);
    gtk_tree_view_column_set_alignment (ato_col[k], 0.5);
    gtk_tree_view_column_set_cell_data_func (ato_col[k], ato_cell[k], atom_set_visible, GINT_TO_POINTER(k), NULL);
  }
  old_thermo = allocint (qm_proj -> natomes);
  if (fixco)
  {
    old_fixed = allocdint (qm_proj -> natomes, 3);
  }
  k = l = 0;
  for (i=0; i < qm_proj -> nspec; i++)
  {
    for (j=0; j < qm_proj -> natomes; j++)
    {
      if (qm_proj -> atoms[0][j].sp == i)
      {
        k ++;
        m = was_it_selected (the_therm, j);
        if (m)
        {
          l ++;
          n_therm += (m > 1) ? 1 : 0;
          old_thermo[j] = (m > 1) ? 1 : 0;
          qm_proj -> atoms[0][j].pick[0] = old_thermo[j];
          qm_view -> picked += old_thermo[j];
          if (fixco && m > 1)
          {
            if (is_cpmd)
            {
              old_fixed[j][0] = tmp_cpmd -> fixcoord[m-2][0];
              old_fixed[j][1] = tmp_cpmd -> fixcoord[m-2][1];
              old_fixed[j][2] = tmp_cpmd -> fixcoord[m-2][2];
            }
            else
            {
              old_fixed[j][0] = tmp_cp2k -> fixcoord[0][m-2][0];
              old_fixed[j][1] = tmp_cp2k -> fixcoord[0][m-2][1];
              old_fixed[j][2] = tmp_cp2k -> fixcoord[0][m-2][2];
            }
          }
        }
      }
    }
  }
  fill_thermo_atom_model (the_therm);
  init_default_shaders (qm_view);
  num_cpmd_objects = l;
  g_object_unref (add_model);
  i = ((qm_proj -> nspec + l)*37 < 500) ? (qm_proj -> nspec + l)*37 : 500;
  //GtkWidget * scrollsets = create_scroll (NULL, -1, -1, GTK_SHADOW_ETCHED_IN, 0);
  GtkWidget * scrollsets = create_scroll (dialog_get_content_area (amol), 320, i, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrollsets, add_tree);
  str = g_strdup_printf (" <b>(*)</b> Order of appearance in the input file");
  GtkWidget * vbox = dialog_get_content_area (amol);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
  g_free (str);
  if (the_therm != -1)
  {
    if (get_num_thermo() > 1 || the_therm < -1)
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
      GtkWidget * hbox = create_hbox (0);
      gchar * lab[2] = {"All non-thermostated atom(s)", "All atom(s)"};
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                          check_button (lab[(the_therm > -1) ? 0 : 1], -1, -1,
                          FALSE, G_CALLBACK(select_atoms_not_thermostated), GINT_TO_POINTER(the_therm)),
                          FALSE, FALSE, 50);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    }
  }
  run_this_gtk_dialog (amol, G_CALLBACK(run_select_atom_from_model), & fixco);
  proj_unselect_all_atoms ();
  if (the_therm > -1) set_going_forward ();
  g_free (old_thermo);
  fixco = FALSE;
}

/*!
  \fn G_MODULE_EXPORT void atom_selection_button (GtkButton * but, gpointer data)

  \brief select atom(s) to be thermostated

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void atom_selection_button (GtkButton * but, gpointer data)
{
  int num, id;
  int i = GPOINTER_TO_INT(data);
  select_atom_from_model (i);
  if (i < -1)
  {
    id = 2;
    num = get_active_dummy (-(i+2)) -> natoms;
  }
  else if (i == -1)
  {
    id = 1;
    num = (is_cpmd) ? tmp_cpmd -> fixat : tmp_cp2k -> fixat[0];
  }
  else
  {
    id = 0;
    thermostat * thermo = get_active_thermostat (i);
    num = thermo -> natoms;
  }
  gchar * stra, * strb;
  if (num  == 0)
  {
    stra = g_strdup_printf ("Not picked yet !");
    strb = g_strdup_printf (DELETEB);
  }
  else
  {
    stra = g_strdup_printf ("%d atom(s)", num);
    strb = g_strdup_printf (APPLY);
  }
  set_image_from_icon_name (sel_img[id], strb);
  gtk_button_set_label (but, stra);
  g_free (stra);
  g_free (strb);
  if (is_cpmd)
  {
    // CPMD
    if (i < -1)
    {
      print_the_section (8, 0, qmbuffer[8]);
    }
    else if (i == -1)
    {
      print_the_section (8, 0, qmbuffer[8]);
    }
    else
    {
      for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
    }
  }
  else
  {
    // CP2K
  }
}

/*!
  \fn void create_selection_button (GtkWidget * box, int num, int id, gpointer data)

  \brief create thermostat atom(s) selection button

  \param box the GtkWidget sending the signal
  \param num the number of atom(s) already selected
  \param id 0 = ionic, 1 = fictitious electronic
  \param data the associated data pointer
*/
void create_selection_button (GtkWidget * box, int num, int id, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  GtkWidget * hbox = cpmd_box (box, "Atom(s) selection: ", 5, 20, (i < -1) ? 120 : 220);
  gchar * str;
  if (num == 0)
  {
    str = g_strdup_printf ("Not picked yet !");
    sel_img[id] = stock_image (DELETEB);
  }
  else
  {
    str = g_strdup_printf ("%d atom(s)", (int)num);
    sel_img[id] = stock_image (APPLY);
  }
  sel_but[id] = gtk_button_new_with_label (str);
  gtk_widget_set_size_request (sel_but[id], 150, -1);
  g_free (str);
  g_signal_connect (G_OBJECT(sel_but[id]), "clicked", G_CALLBACK(atom_selection_button), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sel_but[id], FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sel_img[id], FALSE, FALSE, 30);
}

/*!
  \fn void create_nose_thermo_param_box (int therm_id)

  \brief create thermostat configuration widgets

  \param therm_id the thermostat id
*/
void create_nose_thermo_param_box (int therm_id)
{
  gchar * str;
  nose_id_box[1] = destroy_this_widget (nose_id_box[1]);
  str = g_strdup_printf ("Configuration for thermostat N°<b>%d</b>: ", therm_id+1);
  nose_id_box[1] = create_vbox (BSEP);
  cpmd_box (nose_id_box[1], str, 5, 5, 220);
  g_free (str);
  thermostat * thermo = get_active_thermostat (therm_id);
  create_selection_button (nose_id_box[1], thermo -> natoms, 0, GINT_TO_POINTER(therm_id));
  nose_parameters (nose_id_box[1], therm_id,
                   v_thermo[!is_cpmd][thermo->type],
                   c_thermo[!is_cpmd][thermo->type],
                   u_thermo[!is_cpmd][thermo->type]);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, nose_id_box[0], nose_id_box[1], FALSE, FALSE, 0);
  show_the_widgets (nose_id_box[1]);
}

/*!
  \fn G_MODULE_EXPORT void changed_nose_thermo_id_box (GtkComboBox * box, gpointer data)

  \brief change thermostat id

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_nose_thermo_id_box (GtkComboBox * box, gpointer data)
{
  int i;
  i = combo_get_active ((GtkWidget *)box);
  create_nose_thermo_param_box (i);
}

/*!
  \fn void create_selection_combo (int id, int num, int type, GCallback handler)

  \brief create thermostat selection combo box

  \param id 0 = ionic, 1 = fictitious electronic
  \param num the number of thermostat
  \param type the type of thermostat
  \param handler the associated callback
*/
void create_selection_combo (int id, int num, int type, GCallback handler)
{
  int i, j;
  gchar * str;
  gchar * lab[2]={"Thermostat", "Dummy atom"};
  combo_id_box[id] = destroy_this_widget (combo_id_box[id]);
  combo_id_box[id] = create_combo ();
  for (i=0; i<num; i++)
  {
    str = g_strdup_printf ("%s N°%d", lab[id], i+1);
    combo_text_append (combo_id_box[id], str);
    g_free (str);
  }
  j = type;
  if (num > 0)
  {
    combo_set_active (combo_id_box[id], 0);
    if (id == 0)
    {
      create_nose_thermo_param_box (0);
    }
    else
    {
      create_dummy_param_box (0);
    }
  }
  else
  {
    combo_set_active (combo_id_box[id], -1);
    widget_set_sensitive (combo_id_box[id], FALSE);
  }
  g_signal_connect (G_OBJECT (combo_id_box[id]), "changed", handler, GINT_TO_POINTER(j));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, combo_id[id], combo_id_box[id], FALSE, FALSE, 0);
  show_the_widgets (combo_id[id]);
}

/*!
  \fn void add_thermostat (int extra)

  \brief add thermostat(s)

  \param extra the number of thermostat(s) to add
*/
void add_thermostat (int extra)
{
  int i, j;
  thermostat * thermo = get_thermo ();
  j = get_num_thermo ();
  while (thermo -> next != NULL) thermo = thermo -> next;
  for (i=0; i<extra; i++)
  {
    thermo -> next = init_thermo (i+j, thermo -> type, thermo -> sys);
    thermo -> next -> prev = thermo;
    if (i < extra-1) thermo = thermo -> next;
  }

  if (is_cpmd)
  {
    tmp_cpmd -> thermostats += extra;
  }
  else
  {
    tmp_cp2k -> thermostats += extra;
  }
  // finally update the combobox
  create_selection_combo (0, get_num_thermo (), get_thermo() -> type, G_CALLBACK(changed_nose_thermo_id_box));
}

/*!
  \fn G_MODULE_EXPORT void add_or_remove_thermostat (GtkSpinButton * res, gpointer data)

  \brief add / remove thermostat

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void add_or_remove_thermostat (GtkSpinButton * res, gpointer data)
{
  int id = gtk_spin_button_get_value_as_int(res);
  int i, j, k;
  gchar * str;
  gboolean add_thermo = TRUE;
  thermostat * thermo = get_thermo();
  if (id != get_num_thermo ())
  {
    if (id > get_num_thermo ())
    {
      // adding thermostats
      // ok if: 1) all atoms do not already have a thermostat assigned
      //        2) number of thermostat(s) to create < number of atoms that do have a thermostat assigned
      j = 0;
      for (i=0; i<get_num_thermo (); i++)
      {
        j += thermo -> natoms;
        if (thermo -> next != NULL) thermo = thermo -> next;
      }
      if (j < qm_proj -> natomes)
      {
        // Number of thermostat to create
        k = id - get_num_thermo ();
        if (k < (qm_proj -> natomes - j))
        {
          if (k > 1)
          {
            str = g_strdup_printf ("Do you really want to add %d thermostat(s) ?", k);
            add_thermo = ask_yes_no ("Adding thermostat(s) ?", str, GTK_MESSAGE_QUESTION, qm_assistant);
            g_free (str);
          }
          if (add_thermo)
          {
            add_thermostat (k);
          }
          else
          {
            // Set value back to the number of ionic thermostats
            gtk_spin_button_set_value (GTK_SPIN_BUTTON(res), (double)get_num_thermo ());
          }
        }
        else
        {
          show_warning ("It is not possible to create so many thermostats", qm_assistant);
          gtk_spin_button_set_value (GTK_SPIN_BUTTON(res), (double)get_num_thermo ());
        }
      }
      else
      {
        show_warning ("All atoms arleady have a thermostat assigned", qm_assistant);
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(res), (double)get_num_thermo ());
      }
    }
    else
    {
      // removing thermostats
      remove_nose_thermostat (get_num_thermo () - id);
    }
  }
  gtk_spin_button_set_value (GTK_SPIN_BUTTON(res), (double)(get_num_thermo ()));
  if (is_cpmd) for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

/*!
  \fn G_MODULE_EXPORT void update_thermo_parameter (GtkEntry * res, gpointer data)

  \brief update thermostat parameter value entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_thermo_parameter (GtkEntry * res, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i < 0)
  {
    if (tmp_cpmd -> elec_thermostat -> params[i+2] != v)
    {
      tmp_cpmd -> elec_thermostat -> params[i+2] = v;
      update_entry_double (res, v);
    }
  }
  else
  {
    if (get_num_thermo () > 1)
    {
      j = combo_get_active (combo_id_box[0]);
    }
    else
    {
      j = 0;
    }
    thermostat * thermo = get_active_thermostat (j);
    if (i < 2)
    {
      if (thermo -> params[i] != v)
      {
        thermo -> params[i] = v;
        update_entry_double (res, v);
      }
    }
    else
    {
      if ((int)thermo -> params[i] != (int)v)
      {
        thermo -> params[i] = v;
        update_entry_int (res, (int)v);
      }
    }
  }
  if (is_cpmd) for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

/*!
  \fn GtkWidget * create_nose_box (int n)

  \brief create thermostat configuration widgets

  \param n the thermostat system
*/
GtkWidget * create_nose_box (int n)
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * widg;
  if (n > GLOBAL)
  {
    hbox = cpmd_box (vbox, "Number of thermostat(s): ", 5, 5, 220);
    widg = spin_button (G_CALLBACK(add_or_remove_thermostat),
                        (double)(get_num_thermo ()), 1.0, (double)qm_proj -> natomes, 1.0, 0, 100, NULL);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 0);
    combo_id[0] = cpmd_box (vbox, "Thermostat to configure: ", 5, 5, 220);
    nose_id_box[0] = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, nose_id_box[0], FALSE, FALSE, 10);
    create_selection_combo (0, get_num_thermo (), get_thermo () -> type, G_CALLBACK(changed_nose_thermo_id_box));
    create_nose_thermo_param_box (0);
  }
  else
  {
    nose_parameters (vbox, 0,
                     v_thermo[!is_cpmd][get_thermo()->type],
                     c_thermo[!is_cpmd][get_thermo()->type],
                     u_thermo[!is_cpmd][get_thermo()->type]);
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void changed_thermo_box_nose (GtkComboBox * box, gpointer data)

  \brief change the type of the ionic thermostat

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_thermo_box_nose (GtkComboBox * box, gpointer data)
{
  int i;
  i = combo_get_active ((GtkWidget *)box);
  if (i != get_thermo () -> sys)
  {
    clean_thermostat (get_thermo () -> type);
    clean_nose_widgets ();
    get_thermo () -> sys = i;
    nose_box = create_nose_box (get_thermo () -> sys);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, therm_param_ions, nose_box, FALSE, FALSE, 0);
    int j;
    if (is_cpmd) for (j=1; j<4; j++) print_the_section (j, 0, qmbuffer[j]);
    show_the_widgets (therm_param_ions);
  }
  set_going_forward ();
}

/*!
  \fn void prepare_therm_ions ()

  \brief  prepare ions thermostat widgets
*/
void prepare_therm_ions ()
{
  int i, j, k;
  GtkWidget * hbox;
  GtkWidget * tbox;
  gchar * str;
  clean_nose_widgets ();
  therm_param_ions = destroy_this_widget (therm_param_ions);
  therm_param_ions = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, therm_ions, therm_param_ions, FALSE, FALSE, 0);
  i = (is_cpmd) ? 1 : 0;
  //if (is_cpmd)
  {
    if (get_thermo () -> type > i)
    {
      hbox = cpmd_box (therm_param_ions, "Thermostat type: ", 5, 5, 220);
      tbox = create_combo ();
      j = 2; // For QM-MM: qm_view -> bonding + 2;
      for (k=0; k<j; k++)
      {
        str = g_strdup_printf ("%s", nosetype[k]);
        combo_text_append (tbox, str);
        g_free (str);
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, tbox, FALSE, FALSE, 0);
      combo_set_active (tbox, get_thermo () -> sys);
      g_signal_connect (G_OBJECT (tbox), "changed", G_CALLBACK(changed_thermo_box_nose), NULL);
    }
  }
  nose_box = create_nose_box (get_thermo () -> sys);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, therm_param_ions, nose_box, FALSE, FALSE, 0);
  show_the_widgets (therm_param_ions);
  if (is_cpmd) for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

/*!
  \fn void prepare_therm_elec ()

  \brief prepare electrons thermostat widgets
*/
void prepare_therm_elec ()
{
  therm_param_elec = destroy_this_widget (therm_param_elec);
  therm_param_elec = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, therm_elec, therm_param_elec, FALSE, FALSE, 0);
  int i;
  nose_parameters (therm_param_elec, -2,
                   v_thermo[0][tmp_cpmd -> elec_thermostat -> type],
                   c_thermo[0][tmp_cpmd -> elec_thermostat -> type],
                   ue_thermo[tmp_cpmd -> elec_thermostat -> type]);
  show_the_widgets (therm_param_elec);
  for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

/*!
  \fn G_MODULE_EXPORT void changed_thermo_box (GtkComboBox * box, gpointer data)

  \brief change the thermostat family (ionic / fictitious electronic)

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_thermo_box (GtkComboBox * box, gpointer data)
{
  int i, j;
  i = combo_get_active ((GtkWidget *)box);
  j = GPOINTER_TO_INT (data);
  if (j == 0 && i != get_thermo () -> type)
  {
    clean_thermostat (i);
    thermostat * thermo = get_thermo();
    thermo -> sys = GLOBAL;
    prepare_therm_ions ();
  }
  else if (j == -1)
  {
    if (i != tmp_cpmd -> elec_thermostat -> type)
    {
      g_free (tmp_cpmd -> elec_thermostat);
      tmp_cpmd -> elec_thermostat = init_thermo (-1, i, 0);
      tmp_cpmd -> elec_thermostat -> sys = 0;
      prepare_therm_elec ();
    }
  }
  set_going_forward ();
}

/*!
  \fn void thermo_type_box (GtkWidget * vbox, gchar * str, int id)

  \brief prepare the thermostat option widgets

  \param vbox the GtkWidget sending the signal
  \param str thermostat information
  \param id the type of thermostat
*/
void thermo_type_box (GtkWidget * vbox, gchar * str, int id)
{
  int i;
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
  GtkWidget * tbox = create_combo ();
  for (i=0; i<num_thermo[!is_cpmd]; i++)
  {
    str = g_strdup_printf ("%s", thermo_name[!is_cpmd][i]);
    combo_text_append (tbox, str);
    g_free (str);
  }
  if (id == -1)
  {
    combo_set_active (tbox, tmp_cpmd -> elec_thermostat -> type);
  }
  else
  {
    thermostat * thermo = get_thermo();
    combo_set_active (tbox, thermo -> type);
  }
  g_signal_connect (G_OBJECT (tbox), "changed", G_CALLBACK(changed_thermo_box), GINT_TO_POINTER(id));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, tbox, FALSE, FALSE, 0);
}

/*!
  \fn GtkWidget * thermo_box ()

  \brief create the thermostat configuration widgets
*/
GtkWidget * thermo_box ()
{
  gchar * thermo_info[2] = {"<u>Ionic subsystem thermostat:</u> ", "Thermostat: "};
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * vvbox;
  nose_id_box[0] = nose_id_box[1] = NULL;
  therm_param_ions = NULL;
  therm_param_elec = NULL;
  nose_box = NULL;
  combo_id_box[0] = NULL;
  combo_id[0] = NULL;
  if (get_num_thermo() == 0) init_thermostats (0, 1);
  // Thermostat type ions
  vvbox = create_vbox (BSEP);
  gtk_widget_set_size_request (vvbox, (is_cpmd) ? 525 : -1, (is_cpmd) ? 350 : 260);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, vvbox, FALSE, FALSE, 0);
  thermo_type_box (vvbox, thermo_info[!is_cpmd], 0);
  therm_ions = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, therm_ions, FALSE, FALSE, (is_cpmd) ? 0 : 20);
  prepare_therm_ions ();
  // Thermostat type electrons
  if (is_cpmd)
  {
    electron_box = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, electron_box, FALSE, FALSE, 5);
    thermo_type_box (electron_box, "<u>Fictitious electronic subsystem:</u> ", -1);
    therm_elec = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, electron_box, therm_elec, FALSE, FALSE, 0);
    prepare_therm_elec ();
  }
  return vbox;
}
