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
* @file atom_search.c
* @short Functions to prepare the model edition 'search' tabs and the associated widgets
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'atom_search.c'
*
* Contains:
*

 - The functions to prepare the model edition 'search' tabs and the associated widgets

*
* List of functions:

  G_MODULE_EXPORT void turn_rebuild_on (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void turn_rebuild_on (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void turn_bonding_on (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void turn_bonding_on (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_atoms_for_action (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void expanding_atoms (GtkWidget * exp, gpointer data);

  GtkWidget * create_search_box (int aid, project * this_proj);
  GtkWidget * create_action_combo (int id, project * this_proj);
  GtkWidget * action_tab (int aid, project * this_proj);

*/

#include "atom_edit.h"

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void turn_rebuild_on (GtkCheckButton * but, gpointer data)

  \brief rebuild toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void turn_rebuild_on (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void turn_rebuild_on (GtkToggleButton * but, gpointer data)

  \brief rebuild toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void turn_rebuild_on (GtkToggleButton * but, gpointer data)
#endif
{
  tint * dat = (tint *) data;
  int i = button_get_status ((GtkWidget *)but);
  get_project_by_id(dat -> a) -> modelgl -> rebuild[0][dat -> c] = i;
#ifdef GTK3
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)get_project_by_id(dat -> a) -> modelgl -> rbuild[i], i);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void turn_bonding_on (GtkCheckButton * but, gpointer data)

  \brief update bonding toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void turn_bonding_on (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void turn_bonding_on (GtkToggleButton * but, gpointer data)

  \brief update bonding toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void turn_bonding_on (GtkToggleButton * but, gpointer data)
#endif
{
  atom_search * asearch = (atom_search *) data;
  asearch -> recompute_bonding = button_get_status ((GtkWidget *)but);
}

/*!
  \fn GtkWidget * create_search_box (int aid, project * this_proj)

  \brief create the action search widget

  \param aid the action id
  \param this_proj the target project
*/
GtkWidget * create_search_box (int aid, project * this_proj)
{
  gchar * appl[5] = {" Move atom(s)", " Replace atom(s)", " Remove atom(s)", " Insert atom(s)", " Move atom(s)"};
  gchar * img[4]= {EDITA, LIST_REM, LIST_ADD, MEDIA_PLAY};
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * widg;
  int i;
  if (aid == 4)
  {
    i = (this_proj -> modelgl -> search_widg[aid+1] != NULL) ? this_proj -> modelgl -> search_widg[aid+1] -> in_selection : 0;
  }
  else if (aid == 8)
  {
    i = this_proj -> nspec;
  }
  else
  {
    i = this_proj -> modelgl -> search_widg[aid+1] -> todo_size;
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, selection_tab (this_proj -> modelgl -> search_widg[aid+1], i), FALSE, FALSE, 0);
  if (aid == 1 || aid == 5)
  {
    int j = (aid == 1) ? 0 : 1;
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    this_proj -> modelgl -> atom_win -> edition_but[j] = check_button("Extract/rebuild the object(s) to be moved, ie. cut/clean bonds with nearest neighbor(s)",
                                                                       -1, 25, this_proj -> modelgl -> rebuild[0][j], G_CALLBACK(turn_rebuild_on), & this_proj -> modelgl -> colorp[0][j]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, this_proj -> modelgl -> atom_win -> edition_but[j], FALSE, FALSE, 50);
  }
  if (aid == 1 || aid == 3 || aid == 5)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    widg = check_button("Recompute bonding information using bond cutoff(s)",
                        -1, 25, this_proj -> modelgl -> search_widg[aid+1] -> recompute_bonding, G_CALLBACK(turn_bonding_on),  this_proj -> modelgl -> search_widg[aid+1]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 50);
  }

  if (aid > 1 && aid < 6)
  {
    GtkWidget * lay = create_layout (-1, 100);
    GtkWidget * but;
    if (aid == 5)
    {
      layout_add_widget (lay, markup_label("Repeat <i>n</i> times, <i>n</i>= ", 100, -1, 0.0, 0.5), 125, 25);
      layout_add_widget (lay, spin_button (G_CALLBACK(repeat_move), 1, 1, 1000, 1, 0, 100, this_proj), 275, 20);
    }
    but = create_button (appl[aid-1], IMG_STOCK, img[aid-2], 100, 35, GTK_RELIEF_NORMAL, G_CALLBACK(take_action), & this_proj -> modelgl -> search_widg[aid+1] -> pointer[0]);
    layout_add_widget (lay, but, 500, 20);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, lay, FALSE, FALSE, 0);
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void set_atoms_for_action (GtkComboBox * box, gpointer data)

  \brief change selection status

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_atoms_for_action (GtkComboBox * box, gpointer data)
{
  tint * id = (tint *)data;
  project * this_proj = get_project_by_id (id -> a);
  int i, j, k;
  i = combo_get_active ((GtkWidget *)box);
  this_proj -> modelgl -> search_widg[id -> c] -> status = i;
  if (id -> c == 2)
  {
    for (j=0; j<2; j++)
    {
      combo_set_active (this_proj -> modelgl -> atom_win -> axis_combo[j], this_proj -> modelgl -> atom_win -> axis[j]);
      for (k=0; k<6; k++)
      {
        gtk_range_set_value (GTK_RANGE(this_proj -> modelgl -> atom_win -> edit_scale[k]), this_proj -> modelgl -> atom_win -> new_param[i][j][k]);
        update_range_and_entry (this_proj, i, this_proj -> modelgl -> atom_win -> axis[j], k);
      }
    }
  }
  clean_picked_and_labelled (this_proj -> modelgl -> search_widg[id -> c], TRUE);
  update_search_tree (this_proj -> modelgl -> search_widg[id -> c]);
}

/*!
  \fn GtkWidget * create_action_combo (int id, project * this_proj)

  \brief create action combo box

  \param id the action id
  \param this_proj the target project
*/
GtkWidget * create_action_combo (int id, project * this_proj)
{
  GtkWidget * combo;
  if (id == 3 || id == 5)
  {
    GtkTreeModel * model = replace_combo_tree (TRUE, this_proj -> id);
    combo = gtk_combo_box_new_with_model (model);
    g_object_unref (model);
    GtkCellRenderer * renderer = gtk_cell_renderer_combo_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);
    g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(set_atoms_to_insert), & this_proj -> modelgl -> search_widg[id+2] -> pointer[0]);
    combo_set_active (combo, 0);
    combo_set_markup (combo);
    set_atoms_to_insert (GTK_COMBO_BOX(combo), & this_proj -> modelgl -> search_widg[id+2] -> pointer[0]);
  }
  else
  {
    combo = create_combo ();
    int i;
    for (i=0; i<3; i++) combo_text_append (combo, action_atoms[i]);
    combo_set_active (combo, this_proj -> modelgl -> search_widg[id+2] -> status);
    g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_atoms_for_action), & this_proj -> modelgl -> search_widg[id+2] -> pointer[0]);
  }

  return combo;
}

/*!
  \fn G_MODULE_EXPORT void expanding_atoms (GtkWidget * exp, gpointer data)

  \brief expanding motion callback

  \param exp the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void expanding_atoms (GtkWidget * exp, gpointer data)
{
  tint * dat = (tint *)data;
  project * this_proj = get_project_by_id (dat -> a);
  int i;
  for (i=0; i<3; i++)
  {
    if (i != dat -> c - 1)
    {
      if (gtk_expander_get_expanded (GTK_EXPANDER (this_proj -> modelgl -> atom_win -> at_expand[i])))
      {
        gtk_expander_set_expanded (GTK_EXPANDER (this_proj -> modelgl -> atom_win -> at_expand[i]), FALSE);
      }
    }
  }
}

/*!
  \fn GtkWidget * action_tab (int aid, project * this_proj)

  \brief create model edtion action tab

  \param aid the action id
  \param this_proj the target project
*/
GtkWidget * action_tab (int aid, project * this_proj)
{
  gchar * action[7] = {"moved", "replaced", "removed", "inserted", "moved randomly", " ", "passivated"};
  GtkWidget * vbox = create_vbox (BSEP);
  atom_search * asearch = this_proj -> modelgl -> search_widg[aid+2];
  GtkWidget * hbox;
  if (aid < 5)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
    gchar * str = g_strdup_printf ("<u>Select the object(s) to be %s in:</u> ", action[aid]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 200, -1, 0.0, 0.5), FALSE, FALSE, 20);
    g_free (str);
    this_proj -> modelgl -> atom_win -> atom_combo[aid] = create_action_combo (aid, this_proj);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, this_proj -> modelgl -> atom_win -> atom_combo[aid], FALSE, FALSE, 20);
  }
  GtkWidget * sbox = create_vbox (BSEP);
  GtkWidget * tbox = create_search_box (aid+1, this_proj);
  if (! aid)
  {
    gchar * exp_name[3] = {"<b>Atom selection:</b>", "<b>Translate:</b>", "<b>Rotate:</b>"};
    int i;
    for (i=0; i<3; i++)
    {
      this_proj -> modelgl -> atom_win -> at_expand[i] = create_expander (exp_name[i], NULL);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, sbox, this_proj -> modelgl -> atom_win -> at_expand[i], TRUE, TRUE, 10);
      if (! i)
      {
        add_container_child (CONTAINER_EXP, this_proj -> modelgl -> atom_win -> at_expand[i], tbox);
        gtk_expander_set_expanded (GTK_EXPANDER(this_proj -> modelgl -> atom_win -> at_expand[i]), TRUE);
      }
      else
      {
        add_container_child (CONTAINER_EXP, this_proj -> modelgl -> atom_win -> at_expand[i], add_motion_interaction (asearch, i-1, this_proj));
        gtk_expander_set_expanded (GTK_EXPANDER(this_proj -> modelgl -> atom_win -> at_expand[i]), FALSE);
        if (i == 2) widget_set_sensitive (this_proj -> modelgl -> atom_win -> at_expand[2], asearch -> object);

      }
      g_signal_connect (G_OBJECT(this_proj -> modelgl -> atom_win -> at_expand[i]), "activate", G_CALLBACK(expanding_atoms), & asearch -> pointer[0]);
    }
  }
  else
  {
    if (aid < 3)
    {
      asearch -> mode_box = create_combo ();
      combo_text_append (asearch -> mode_box, "Normally");
      combo_text_append (asearch -> mode_box, "Randomly");
      combo_set_active (asearch -> mode_box, 0);
      g_signal_connect (G_OBJECT (asearch -> mode_box), "changed", G_CALLBACK(set_search_mode), asearch);
      GtkWidget * box = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, sbox, box, FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, markup_label("<b>.</b>", 5, -1, 0.0, 0.5), FALSE, FALSE, 10);
      gchar * type_of[2]={"Replace: ", "Remove: "};
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, markup_label(type_of[aid-1], 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, asearch -> mode_box, FALSE, FALSE, 5);
    }
    add_box_child_start (GTK_ORIENTATION_VERTICAL, sbox, tbox, FALSE, FALSE, 0);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, sbox, FALSE, FALSE, 5);
  if (! aid || aid == 4)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 20);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                         check_button ("Reset transformation(s)", -1, 35, FALSE, G_CALLBACK(set_reset_transformation), & asearch -> pointer[0]),
                         FALSE, FALSE, 10);
  }
  show_the_widgets (vbox);
  if (aid != 3) widget_set_sensitive (vbox, this_proj -> nspec);
  return vbox;
}
