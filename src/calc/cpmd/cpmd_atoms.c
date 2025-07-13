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
* @file cpmd_atoms.c
* @short Functions to handle dummy atom(s) for the CPMD input file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cpmd_atoms.c'
*
* Contains:
*

 - The functions to handle dummy atom(s) for the CPMD input file

*
* List of functions:

  void create_dummy_param_box (int dummy_id);
  void dummy_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void dummy_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void clean_dummy_widgets ();
  void remove_dummy (int num_to_remove);
  void add_dummy (int extra);

  G_MODULE_EXPORT void update_dummy_coord (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void dummy_type_changed (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void changed_dummy_id_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void select_dummy (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void run_remove_dummy (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void add_or_remove_dummy (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void atom_button (GtkWidget * but, gpointer data);

  GtkWidget * create_dummy_box ();

  dummy_atom * get_active_dummy (int id);
  dummy_atom * init_dummy (int type, int id);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "calc.h"
#include "cpmd.h"

extern void create_selection_combo (int id, int num, int type, GCallback handler);
extern void print_the_section (int s, int p, GtkTextBuffer * buffer);
extern void create_selection_button (GtkWidget * box, int num, int id, gpointer data);
extern G_MODULE_EXPORT void changed_opt_box (GtkComboBox * box, gpointer data);
extern GtkWidget * combo_id[2];
extern GtkWidget * combo_id_box[2];
extern ColRGBA init_color (int id, int numid);
extern int num_cpmd_objects;

GtkWidget * dummy_box[2];
GtkWidget * the_dummy_box;
GtkWidget * dummy_param_box;
dummy_atom * dummy;
int n_dummy;
int * old_dummy;
GtkCellRenderer * dummy_renderer[5];
GtkTreeViewColumn * dummy_col[5];

/*!
  \fn dummy_atom * get_active_dummy (int id)

  \brief get dummy atom by id

  \param id the target dummy atom id
*/
dummy_atom * get_active_dummy (int id)
{
  dummy_atom * dumm = tmp_cpmd -> dummy;
  while (dumm -> id != id)
  {
    if (dumm -> next != NULL) dumm = dumm -> next;
  }
  return dumm;
}

/*!
  \fn G_MODULE_EXPORT void update_dummy_coord (GtkEntry * res, gpointer data)

  \brief udate dummy atom coordinate entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_dummy_coord (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  dummy -> xyz[i] = v;
  update_entry_double (res, dummy -> xyz[i]);
  print_the_section (8, 0, qmbuffer[8]);
}

void create_dummy_param_box (int dummy_id);

/*!
  \fn G_MODULE_EXPORT void dummy_type_changed (GtkComboBox * box, gpointer data)

  \brief change the dummy atom type

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void dummy_type_changed (GtkComboBox * box, gpointer data)
{
  int i;
  i = combo_get_active ((GtkWidget *)box);
  if (i != dummy -> type)
  {
    dummy -> type = i;
    create_dummy_param_box (dummy -> id);
  }
  print_the_section (8, 0, qmbuffer[8]);
}

/*!
  \fn void create_dummy_param_box (int dummy_id)

  \brief crreate dummy atom parameter widgets

  \param dummy_id dummy atom id
*/
void create_dummy_param_box (int dummy_id)
{
  gchar * str;
  dummy_box[1] = destroy_this_widget (dummy_box[1]);
  if (tmp_cpmd -> dummies > 0)
  {
    GtkWidget * hbox;
    GtkWidget * widg;
    str = g_strdup_printf ("Configuration for dummy atom N°<b>%d</b>: ", dummy_id+1);
    dummy_box[1] = create_vbox (BSEP);
    cpmd_box (dummy_box[1], str, 0, 5, 280);
    g_free (str);
    int i;
    hbox = cpmd_box (dummy_box[1], "Type of dummy atom:", 0, 25, -1);
    GtkWidget * box = create_combo ();
    gchar * dtypes[3] = {"Type 1", "Type 2", "Type 3"};
    gchar * dtext[3] = {"fixed in space:",
                        "calculated by\nthe arithmetic mean of the coordinates of the selected atom(s)",
                        "calculated by\nthe center of mass of the coordinates of the selected atom(s)"};
    for (i=0; i<3; i++)
    {
      combo_text_append (box, dtypes[i]);
    }
    g_signal_connect (G_OBJECT (box), "changed", G_CALLBACK(dummy_type_changed), NULL);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, box, FALSE, FALSE, 10);
    dummy = get_active_dummy (dummy_id);
    str = g_strdup_printf ("The coordinates of the dummy atom are %s", dtext[dummy -> type]);

    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, dummy_box[1], hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, -1, (dummy -> type == 0) ? 40 : 60, 0.0, 0.5), FALSE, FALSE, 25);
    g_free (str);
    if (dummy -> type > 0)
    {
      create_selection_button (dummy_box[1], dummy -> natoms, 2, GINT_TO_POINTER(-(dummy -> id + 2)));
    }
    else
    {
      gchar * lcoo[3]={"<b>x</b>", "<b>y</b>", "<b>z</b>"};

      for (i=0; i<3; i++)
      {
        str = g_strdup_printf ("Dummy %s coordinate: ", lcoo[i]);
        hbox = cpmd_box (dummy_box[1], str, 0, 25, -1);
        widg = create_entry (G_CALLBACK(update_dummy_coord), 100, 15, FALSE, GINT_TO_POINTER(i));
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 10);
        update_entry_double (GTK_ENTRY(widg), dummy -> xyz[i]);
      }
    }
    combo_set_active (box, dummy -> type);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, dummy_box[0], dummy_box[1], FALSE, FALSE, 0);
    show_the_widgets (dummy_box[1]);
  }
}

/*!
  \fn G_MODULE_EXPORT void changed_dummy_id_box (GtkComboBox * box, gpointer data)

  \brief change the dummy atom id

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_dummy_id_box (GtkComboBox * box, gpointer data)
{
  int i;
  i = combo_get_active ((GtkWidget *)box);
  create_dummy_param_box (i);
}

/*!
  \fn dummy_atom * init_dummy (int type, int id)

  \brief create dummy atom

  \param type the type of dummy atom to create
  \param id the id of the new dummy atom
*/
dummy_atom * init_dummy (int type, int id)
{
  dummy_atom * dumm = g_malloc0 (sizeof*dumm);
  dumm -> id = id;
  dumm -> type = type;
  dumm -> show = FALSE;
  dumm -> natoms = 0;
  dumm -> list = NULL;
  dumm -> xyz[0] = 0.0;
  dumm -> xyz[1] = 0.0;
  dumm -> xyz[2] = 0.0;
  return dumm;
}

/*!
  \fn G_MODULE_EXPORT void select_dummy (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select dummy atom toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_dummy (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  GtkTreeStore ** model = (GtkTreeStore **)data;
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(* model), & iter, path);
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    get_active_dummy (old_dummy[n_dummy-1]) -> show = FALSE;
    old_dummy[n_dummy-1] = -1;
    n_dummy --;
    gtk_tree_store_set (* model, & iter, 4, 0, -1);
    //toviz.c = 0;
  }
  else
  {
    n_dummy ++;
    gtk_tree_store_set (* model, & iter, 4, 1, -1);
    gtk_tree_model_get (GTK_TREE_MODEL(* model), & iter, 0, & old_dummy[n_dummy-1], -1);
    old_dummy[n_dummy-1] --;
    get_active_dummy (old_dummy[n_dummy-1]) -> show = TRUE;
    //toviz.c = 1;
  }
  // Viz
}

/*!
  \fn void dummy_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color in the CPMD dummy atom(s) tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void dummy_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i;
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  i = abs(i);
  set_renderer_color (get_active_dummy (i-1) -> show, renderer, init_color (i-1, num_cpmd_objects));
}

/*!
  \fn void dummy_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer in the CPMD dummy atom(s) tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void dummy_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  gtk_tree_model_get (mod, iter, 0, & j, -1);
  if (j > 0 && i != 3)
  {
    gtk_cell_renderer_set_visible (renderer, TRUE);
    if (i < 3) dummy_set_color (col, renderer, mod, iter, data);
  }
  else if (j > 0)
  {
    gtk_cell_renderer_set_visible (renderer, FALSE);
  }
  else if (i == 3)
  {
    gtk_cell_renderer_set_visible (renderer, TRUE);
    dummy_set_color (col, renderer, mod, iter, data);
  }
  else
  {
    gtk_cell_renderer_set_visible (renderer, FALSE);
  }
}

GtkWidget * create_dummy_box ();

/*!
  \fn void clean_dummy_widgets ()

  \brief destroy dummy atom(s) widgets
*/
void clean_dummy_widgets ()
{
  combo_id_box[1] = destroy_this_widget (combo_id_box[1]);
  combo_id[1] = destroy_this_widget (combo_id[1]);
  dummy_box[1] = destroy_this_widget (dummy_box[1]);
  dummy_box[0] = destroy_this_widget (dummy_box[0]);
  the_dummy_box = destroy_this_widget (the_dummy_box);
}

/*!
  \fn G_MODULE_EXPORT void run_remove_dummy (GtkDialog * dialog, gint response_id, gpointer data)

  \brief remove dummy atom(s) - running the dialog

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_remove_dummy (GtkDialog * dialog, gint response_id, gpointer data)
{
  int i, j;
  gboolean done = FALSE;
  int num_to_remove = GPOINTER_TO_INT(data);
  dummy_atom * dumm;
  gchar * str;
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (n_dummy == num_to_remove)
      {
        done = TRUE;
        // Now we remove all selected dummy atoms
        for (i=0; i<n_dummy; i++)
        {
          dumm = tmp_cpmd -> dummy;
          for (j=0; j<tmp_cpmd -> dummies; j++)
          {
            if (dumm -> id == old_dummy[i])
            {
              if (dumm -> next != NULL)
              {
                if (dumm -> prev != NULL)
                {
                  dumm -> next -> prev = dumm -> prev;
                  dumm -> prev -> next = dumm -> next;
                }
                else
                {
                  tmp_cpmd -> dummy = dumm -> next;
                  tmp_cpmd -> dummy -> prev = NULL;
                }
                g_free (dumm);
              }
              else if (dumm -> prev != NULL)
              {
                dumm -> prev -> next = NULL;
                g_free (dumm);
              }
              else
              {
                g_free (tmp_cpmd -> dummy);
                tmp_cpmd -> dummy = NULL;
              }
              break;
            }
            if (dumm -> next != NULL) dumm = dumm -> next;
          }
        }
        tmp_cpmd -> dummies -= num_to_remove;
        if (tmp_cpmd -> dummies > 0)
        {
          dumm = tmp_cpmd -> dummy;
          for (j=0; j<tmp_cpmd -> dummies; j++)
          {
            dumm -> id = j;
            if (dumm -> next != NULL) dumm = dumm -> next;
          }
        }
        clean_dummy_widgets ();
        the_dummy_box = create_dummy_box ();
        add_box_child_start (GTK_ORIENTATION_VERTICAL, dummy_param_box, the_dummy_box, FALSE, FALSE, 0);
        show_the_widgets (dummy_param_box);
      }
      else
      {
        str = g_strdup_printf ("You must select %d dummy atom(s) to be deleted !", num_to_remove);
        show_warning (str, qm_assistant);
        g_free (str);
      }
      break;
    default:
      // field_unselect_all ();
      done = TRUE;
      break;
  }
  if (done) destroy_this_dialog (dialog);
}

/*!
  \fn void remove_dummy (int num_to_remove)

  \brief remove dummy atom(s) - creating the dialog

  \param num_to_remove the number of dummy atom(s) to remove
*/
void remove_dummy (int num_to_remove)
{
  int i, j, k;
  gchar * str;
  str = g_strdup_printf ("Select the %d dummy atom(s) to be removed", num_to_remove);
  GtkWidget * rdummy = dialogmodal (str, GTK_WINDOW(qm_assistant));
  g_free (str);
  gtk_dialog_add_button (GTK_DIALOG(rdummy), "Apply", GTK_RESPONSE_APPLY);

  gchar * mol_title[5] = {"Id", "Type", "Atom(s)", "      ", "Select"};
  gchar * ctype[5] = {"text", "text", "text", "text", "active"};
  GType col_type[5] = {G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN};

  GtkTreeIter dummy_level, atom_level;
  n_dummy = 0;
  old_dummy = allocint(tmp_cpmd -> dummies);
  GtkTreeStore * remove_model = gtk_tree_store_newv (5, col_type);
  GtkWidget * remove_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(remove_model));
  for (i=0; i<5; i++)
  {
    if (i < 4)
    {
      dummy_renderer[i] = gtk_cell_renderer_text_new ();
    }
    else
    {
      dummy_renderer[i] = gtk_cell_renderer_toggle_new ();
      gtk_cell_renderer_toggle_set_radio (GTK_CELL_RENDERER_TOGGLE(dummy_renderer[i]), TRUE);
      g_signal_connect (G_OBJECT(dummy_renderer[i]), "toggled", G_CALLBACK(select_dummy), & remove_model);
    }
    dummy_col[i] = gtk_tree_view_column_new_with_attributes (mol_title[i], dummy_renderer[i], ctype[i], i, NULL);
    gtk_tree_view_column_set_alignment (dummy_col[i], 0.5);
    gtk_tree_view_append_column (GTK_TREE_VIEW(remove_tree), dummy_col[i]);
    gtk_tree_view_column_set_cell_data_func (dummy_col[i], dummy_renderer[i], dummy_set_visible, GINT_TO_POINTER(i), NULL);
  }
  // fill model
  dummy_atom * dumm = tmp_cpmd -> dummy;
  for (i=0; i<tmp_cpmd -> dummies; i++)
  {
    gtk_tree_store_append (remove_model, & dummy_level, NULL);
    gtk_tree_store_set (remove_model, & dummy_level, 0, i+1,
                                                     1, g_strdup_printf ("Type %d", dumm -> type + 1),
                                                     2, dumm -> natoms,
                                                     3, NULL,
                                                     4, 0, -1);
    for (j=0; j<dumm -> natoms; j++)
    {
      gtk_tree_store_append (remove_model, & atom_level, & dummy_level);
      k = qm_proj -> atoms[0][dumm -> list[j]].sp;
      str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(qm_proj -> chemistry -> label[k]), dumm -> list[j]+1);
      gtk_tree_store_set (remove_model, & atom_level, 0, -(i+1), 1, 0.0, 2, 0.0, 3, str, 4, 0, -1);
      g_free (str);
    }
    if (dumm -> next != NULL) dumm = dumm -> next;
  }
  num_cpmd_objects = tmp_cpmd -> dummies;
  g_object_unref (remove_model);

  i = ((tmp_cpmd -> dummies+1)*40 < 500) ? (tmp_cpmd -> dummies+1)*40 : 500;
  GtkWidget * scrollsets = create_scroll (dialog_get_content_area (rdummy), 450, i, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrollsets, remove_tree);
  run_this_gtk_dialog (rdummy, G_CALLBACK(run_remove_dummy), GINT_TO_POINTER(num_to_remove));
}


/*!
  \fn void add_dummy (int extra)

  \brief add dummy atom(s)

  \param extra the number of dummy atom(s) to add
*/
void add_dummy (int extra)
{
  int i;
  if (tmp_cpmd -> dummy == NULL)
  {
    tmp_cpmd -> dummy = init_dummy (0, 0);
    tmp_cpmd -> dummies = 1;
    extra --;
  }
  dummy_atom * dumm = tmp_cpmd -> dummy;
  while (dumm -> next != NULL) dumm = dumm -> next;
  for (i=0; i<extra; i++)
  {
    dumm -> next = init_dummy (0, i+tmp_cpmd -> dummies);
    dumm -> next -> prev = dumm;
    dumm = dumm -> next;
  }

  tmp_cpmd -> dummies += extra;
  // finaly update the combobox
  create_selection_combo (1, tmp_cpmd -> dummies, 0, G_CALLBACK(changed_dummy_id_box));
}

/*!
  \fn G_MODULE_EXPORT void add_or_remove_dummy (GtkSpinButton * res, gpointer data)

  \brief add or remove dummy atom spin callback

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void add_or_remove_dummy (GtkSpinButton * res, gpointer data)
{
  int id = gtk_spin_button_get_value_as_int(res);
  int k;
  gchar * str;
  gboolean to_add_dummy = TRUE;
  if (id != tmp_cpmd -> dummies)
  {
    if (id > tmp_cpmd -> dummies)
    {
      // adding dummy
      k = id - tmp_cpmd -> dummies;
      if (k > 1)
      {
        str = g_strdup_printf ("Do you really want to add %d dummy atom(s) ?", k);
        to_add_dummy = ask_yes_no ("Adding dummy atom(s) ?", str, GTK_MESSAGE_QUESTION, qm_assistant);
        g_free (str);
      }
      if (to_add_dummy)
      {
        add_dummy (k);
      }
    }
    else if (id < tmp_cpmd -> dummies)
    {
      remove_dummy (tmp_cpmd -> dummies - id);
    }
  }
  gtk_spin_button_set_value (GTK_SPIN_BUTTON(res), tmp_cpmd -> dummies);
  print_the_section (8, 0, qmbuffer[8]);
}

/*!
  \fn GtkWidget * create_dummy_box ()

  \brief create the dummy atom(s) widgets
*/
GtkWidget * create_dummy_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * widg;
  hbox = cpmd_box (vbox, "Number of dummy atom(s):", 5, 20, 200);
  widg = spin_button (G_CALLBACK(add_or_remove_dummy),
                      tmp_cpmd -> dummies, 0.0, (double)qm_proj -> natomes, 1.0, 0, 100, NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 0);
  combo_id[1] = cpmd_box (vbox, "Dummy atom to configure: ", 5, 5, 200);
  dummy_box[0] = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, dummy_box[0], FALSE, FALSE, 10);
  create_selection_combo (1, tmp_cpmd -> dummies, 0, G_CALLBACK(changed_dummy_id_box));
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void atom_button (GtkWidget * but, gpointer data)

  \brief CPMD input file, add constraint(s) or dummy atom(s) - creating the dialog

  \param but the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void atom_button (GtkWidget * but, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  gchar * atom_cons[2] = {"Add and configure constraint(s)", "Add and configure dummy atoms"};
  GtkWidget * amol = dialogmodal (atom_cons[(i == DEFCO) ? 0 : 1], GTK_WINDOW(qm_assistant));
  GtkWidget * vbox = dialog_get_content_area (amol);
  GtkWidget * hbox;
  GtkWidget * widg;
  gchar * str;
  if (i == DEFCO)
  {
    hbox = cpmd_box (vbox, default_opts[5][1], 5, 20, 120);
    widg = create_combo ();
    for (j=0; j<defaut_num[5]; j++)
    {
      str = g_strdup_printf ("%s", default_text[5][j]);
      combo_text_append (widg, str);
      g_free (str);
    }
    combo_set_active (widg, (int)tmp_cpmd -> default_opts[i+1]);
    g_signal_connect (G_OBJECT (widg), "changed", G_CALLBACK(changed_opt_box), GINT_TO_POINTER(i+1));
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 0);
    create_selection_button (vbox, tmp_cpmd -> fixat, 1, GINT_TO_POINTER(-1));
    j = (tmp_cpmd -> default_opts[i+1] < 0.0) ? 0 : (int) tmp_cpmd -> default_opts[i+1];
    widget_set_sensitive (sel_but[1], j);
  }
  else if (i == DEFDU)
  {
    dummy_box[0] = NULL;
    dummy_box[1] = NULL;
    combo_id[1] = NULL;
    dummy_param_box = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, dummy_param_box, FALSE, FALSE, 0);
    the_dummy_box = create_dummy_box();
    add_box_child_start (GTK_ORIENTATION_VERTICAL, dummy_param_box, the_dummy_box, FALSE, FALSE, 0);
  }
  run_this_gtk_dialog (amol, G_CALLBACK(run_destroy_dialog), NULL);
}
