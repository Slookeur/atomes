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
* @file cell_cut.c
* @short Functions to create the 'slab cutting' tab of the cell edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cell_cut.c'
*
* Contains:
*

 - The functions to create the 'slab cutting' tab of the cell edition window

*
* List of functions:

  G_MODULE_EXPORT gboolean scroll_set_slab_alpha (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);

  void create_slab_info (project * this_proj);
  void slab_alpha_has_changed (gpointer data, GLfloat v);
  void invert_selection (project * this_proj);

  G_MODULE_EXPORT void setup_passivate (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void set_slab_property (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_slab_property (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_slab_option (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_slab_type (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_slab_alpha (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void select_this_slab (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void cut_this_slab (GtkButton * but, gpointer data);

  GtkWidget * prepare_slab_box (int sid, project * this_proj);
  GtkWidget * create_slab_param_combo (int sid, project * this_proj);
  GtkWidget * cut_in_model (project * this_proj);

*/

#include "cell_edit.h"
#include "atom_edit.h"

extern G_MODULE_EXPORT void set_filter_changed (GtkComboBox * box, gpointer data);

/*!
  \fn G_MODULE_EXPORT void setup_passivate (GtkButton * but, gpointer data)

  \brief passivate button callback

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void setup_passivate (GtkButton * but, gpointer data)
{
  project * this_proj = (project *)data;

  gchar * str = g_strdup_printf ("Cell edition - %s: surface passivation", this_proj -> name);
  GtkWidget * dial = dialogmodal (prepare_for_title(str), GTK_WINDOW(this_proj -> modelgl -> cell_win -> win));
  g_free (str);
  this_proj -> modelgl -> search_widg[8] -> passivating = TRUE;
  GtkWidget * vbox = dialog_get_content_area (dial);
  GtkWidget * scrollsets = create_scroll (NULL, -1, -1, GTK_SHADOW_NONE);
  add_container_child (CONTAINER_SCR, scrollsets, action_tab (6, this_proj));
  widget_set_sensitive (this_proj -> modelgl -> search_widg[8]  -> object_box, 0);
  gtk_widget_set_size_request (scrollsets, 760, 350);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, scrollsets, FALSE, FALSE, 10);
  GtkWidget * info = markup_label ("Select any object(s) <b>A</b> to be replaced when creating the slab, then select the object(s) <b>B</b> to insert in its place.\n"
                                   "When cutting into the model any bond to <b>A</b> with the new surface, will be passivated by <b>B</b>", -1, -1, 0.5, 0.5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, info, FALSE, FALSE, 20);
  show_the_widgets (dial);
  if (! this_proj -> modelgl -> search_widg[8] -> in_selection)
  {
    combo_set_active (this_proj -> modelgl -> search_widg[8] -> atom_box, 0);
    set_filter_changed (GTK_COMBO_BOX(this_proj -> modelgl -> search_widg[8] -> atom_box), this_proj -> modelgl -> search_widg[8]);
  }
  else
  {
    combo_set_active (this_proj -> modelgl -> search_widg[8] -> atom_box, 0);
    set_spec_changed (GTK_COMBO_BOX(this_proj -> modelgl -> search_widg[8] -> atom_box), this_proj -> modelgl -> search_widg[8]);
  }
  g_signal_connect (G_OBJECT(dial), "response", G_CALLBACK(destroy_this_dialog), NULL);
  dialog_id ++;
  Event_loop[dialog_id] = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (Event_loop[dialog_id]);
  this_proj -> modelgl -> search_widg[8] -> passivating = FALSE;
}

/*!
  \fn void create_slab_info (project * this_proj)

  \brief create slab information widget

  \param this_proj the target project
*/
void create_slab_info (project * this_proj)
{
  this_proj -> modelgl -> cell_win -> slab_info = destroy_this_widget (this_proj -> modelgl -> cell_win -> slab_info);
  this_proj -> modelgl -> cell_win -> slab_info = create_vbox (BSEP);
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> modelgl -> cell_win -> slab_info, hbox, FALSE, FALSE, 5);
  gchar * str;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("- Volume: ", 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
  str = g_strdup_printf ("<b>%f</b>", this_proj -> modelgl -> cell_win -> slab_vol);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 90, -1, 0.0, 0.5), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("&#xC5;<sup>3</sup>", 20, -1, 0.5, 0.5), FALSE, FALSE, 0);
  g_free (str);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> modelgl -> cell_win -> slab_info, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("- Atom(s) in slab: ", 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
  str = g_strdup_printf ("<b>%d</b>", this_proj -> modelgl -> cell_win -> slab_atoms);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 20);
  g_free (str);
  int i;
  for (i=0; i<this_proj -> nspec; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> modelgl -> cell_win -> slab_info, hbox, FALSE, FALSE, 0);
    str = g_strdup_printf ("\t - %s atom(s) in slab: ", this_proj -> chemistry -> label[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    str = g_strdup_printf ("<b>%d</b>", this_proj -> modelgl -> cell_win -> slab_lot[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 20);
    g_free (str);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> modelgl -> cell_win -> slab_info_box, this_proj -> modelgl -> cell_win -> slab_info, FALSE, FALSE, 0);
  show_the_widgets (this_proj -> modelgl -> cell_win -> slab_info);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_slab_property (GtkCheckButton * but, gpointer data)

  \brief set slab property toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_slab_property (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_slab_property (GtkToggleButton * but, gpointer data)

  \brief set slab property toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_slab_property (GtkToggleButton * but, gpointer data)
#endif
{
  dint * dat = (dint *)data;
  glwin * view = get_project_by_id(dat -> a) -> modelgl;
  gboolean status = button_get_status ((GtkWidget *)but);
  switch (dat -> b)
  {
    case 0:
      view -> cell_win -> slab_show = status;
      break;
    case 1:
      view -> cell_win -> slab_pbc = status;
      break;
    case 2:
      view -> cell_win -> slab_act = status;
      break;
    case 3:
      view -> cell_win -> slab_out = status;
      break;
    case 4:
      view -> cell_win -> slab_passivate = status;
      widget_set_sensitive (view -> cell_win -> passivate, view -> cell_win -> slab_passivate);
      if (view -> cell_win -> slab_passivate)
      {
        prepare_atom_edition (& view -> colorp[0][0], FALSE);
        view -> search_widg[8] = allocate_atom_search (dat -> a, REPLACE, 8, get_project_by_id(dat -> a) -> nspec);
        view -> search_widg[8] -> status = 2;
        setup_passivate (NULL, get_project_by_id(dat -> a));
      }
      else
      {
        g_free (view -> search_widg[8]);
        view -> search_widg[8] = NULL;
        if (! view -> atom_win -> visible)
        {
          g_free (view -> atom_win);
          view -> atom_win = NULL;
        }
      }
      break;
  }
  if (view -> cell_win -> slab_show)
  {
     view -> create_shaders[SLABS] = TRUE;
  }
  else
  {
    cleaning_shaders (view, SLABS);
  }
  update (view);
}

/*!
  \fn GtkWidget * prepare_slab_box (int sid, project * this_proj)

  \brief create slab parameters widget box

  \param sid the type of slab
  \param this_proj the target project
*/
GtkWidget * prepare_slab_box (int sid, project * this_proj)
{
  gchar * option[6]={"- Position the center of the slab: ", // 6, 7, 8
                     "- Size of the slab: ",                // 9, 10, 11
                     "- Size of the slab: ",                // 12, 13
                     "- Size of the slab: ",                // 14
                     "- Parallelepiped Angles: ",           // 15, 16, 17
                     "- Slab rotation: "};                  // 18, 19, 20
  GtkWidget * vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(option[sid], 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
  int i;
  switch (sid)
  {
    case 0:
      for (i=6; i<9; i++) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_cell_entries (this_proj, i), FALSE, FALSE, 0);
      break;
    case 1:
      for (i=9; i<12; i++) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_cell_entries (this_proj, i), FALSE, FALSE, 0);
      break;
    case 2:
      for (i=12; i<14; i++) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_cell_entries (this_proj, i), FALSE, FALSE, 0);
      break;
    case 3:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_cell_entries (this_proj, 14), FALSE, FALSE, 0);
      break;
    case 4:
      for (i=15; i<18; i++) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_cell_entries (this_proj, i), FALSE, FALSE, 0);
      break;
    case 5:
      for (i=18; i<21; i++) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_cell_entries (this_proj, i), FALSE, FALSE, 0);
      break;
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void set_slab_option (GtkComboBox * box, gpointer data)

  \brief change slab option combo

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_slab_option (GtkComboBox * box, gpointer data)
{
  project * this_proj = (project *)data;
  int i, j;
  i = combo_get_active ((GtkWidget *)box);
  for (j=0; j<6; j++)
  {
    hide_the_widgets (this_proj -> modelgl -> cell_win -> slab_box[j]);
  }
  switch (i)
  {
    case 0:
      show_the_widgets (this_proj -> modelgl -> cell_win -> slab_box[0]);
      break;
    case 1:
      show_the_widgets (this_proj -> modelgl -> cell_win -> slab_box[1+this_proj -> modelgl -> cell_win -> slab_type]);
      break;
    default:
      if (this_proj -> modelgl -> cell_win -> slab_type)
      {
        show_the_widgets (this_proj -> modelgl -> cell_win -> slab_box[5]);
      }
      else
      {
        show_the_widgets (this_proj -> modelgl -> cell_win -> slab_box[i+2]);
      }
      break;
  }
}

/*!
  \fn GtkWidget * create_slab_param_combo (int sid, project * this_proj)

  \brief  create slab parameters combo widget

  \param sid the slab type
  \param this_proj the target project
*/
GtkWidget * create_slab_param_combo (int sid, project * this_proj)
{
  GtkWidget * combo = create_combo ();
  gchar * options[4] = {"Position", "Size", "Angles", "Rotation"};
  int i, j, k;
  i = 4 - sid;
  for (j=0; j<i; j++)
  {
    k = (! sid) ? j : (sid == 1 && j == 2) ? 3 : j;
    combo_text_append (combo, options[k]);
  }
  combo_set_active (combo, 0);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_slab_option), this_proj);
  return combo;
}

/*!
  \fn G_MODULE_EXPORT void set_slab_type (GtkComboBox * box, gpointer data)

  \brief change slab type

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_slab_type (GtkComboBox * box, gpointer data)
{
  project * this_proj = (project *)data;
  int i;
  for (i=0; i<3; i++) hide_the_widgets (this_proj -> modelgl -> cell_win -> slab_hbox[i]);
  this_proj -> modelgl -> cell_win -> slab_type = i = combo_get_active ((GtkWidget *)box);
  show_the_widgets (this_proj -> modelgl -> cell_win -> slab_hbox[i]);
  combo_set_active (this_proj -> modelgl -> cell_win -> slab_param[i], 0);
  set_slab_option (GTK_COMBO_BOX(this_proj -> modelgl -> cell_win -> slab_param[i]), this_proj);
  if (this_proj -> modelgl -> n_shaders[SLABS][0])
  {
    this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
    update (this_proj -> modelgl);
  }
}

/*!
  \fn void slab_alpha_has_changed (gpointer data, GLfloat v)

  \brief change slab opacity

  \param data the associated data pointer
  \param v the new opacity value
*/
void slab_alpha_has_changed (gpointer data, GLfloat v)
{
  glwin * view = (glwin *)data;
  view -> cell_win -> slab_alpha = v;
  if (view -> n_shaders[SLABS][0])
  {
    view -> create_shaders[SLABS] = TRUE;
    update (view);
  }
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_slab_alpha (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief change slab opactiy callback - scoll

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_slab_alpha (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  slab_alpha_has_changed (data, (GLfloat) value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_slab_alpha (GtkRange * range, gpointer data)

  \brief change slab opactiy callback - range

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_slab_alpha (GtkRange * range, gpointer data)
{
  slab_alpha_has_changed (data, (GLfloat) gtk_range_get_value (range));
}

/*!
  \fn void invert_selection (project * this_proj)

  \brief invert atom(s) selection

  \param this_proj the target project
*/
void invert_selection (project * this_proj)
{
  save_all_selections (this_proj -> modelgl, 0);
  int i, j, k;
  if (this_proj -> modelgl -> mode == EDITION)
  {
    // Invert the saved coordinates as well
    double ** saved_c = NULL;
    i = this_proj -> modelgl -> anim -> last -> img -> selected[0] -> selected;
    if (i < this_proj -> natomes)
    {
      saved_c = g_malloc0 ((this_proj -> natomes - i)*sizeof*saved_c);
      k = 0;
      for (j=0; j<this_proj -> natomes; j++)
      {
        if (! this_proj -> atoms[0][j].pick[0])
        {
          saved_c[k] = allocdouble (3);
          saved_c[k][0] = this_proj -> atoms[0][j].x;
          saved_c[k][1] = this_proj -> atoms[0][j].y;
          saved_c[k][2] = this_proj -> atoms[0][j].z;
          k ++;
        }
      }
    }
    g_free (this_proj -> modelgl -> saved_coord[1]);
    this_proj -> modelgl -> saved_coord[1] = NULL;
    if (saved_c)
    {
      this_proj -> modelgl -> saved_coord[1] = g_malloc0 ((this_proj -> natomes - i)*sizeof*this_proj -> modelgl -> saved_coord[1]);
      for (j=0; j<this_proj -> natomes - i; j++)
      {
        this_proj -> modelgl -> saved_coord[1][j] = duplicate_double(3, saved_c[j]);
      }
      g_free (saved_c);
    }
  }
  for (i=0; i<this_proj -> natomes; i++)
  {
    process_selected_atom (this_proj, this_proj -> modelgl, i, 0, 0, 0);
  }
  update_all_selections (this_proj -> modelgl, 0);
  if (this_proj -> modelgl -> mode == EDITION)
  {
    this_proj -> modelgl -> baryc[1] = get_bary (this_proj, 1);
  }
  int shaders[1] = {SELEC};
  re_create_md_shaders (1, shaders, this_proj);
}

/*!
  \fn G_MODULE_EXPORT void select_this_slab (GtkButton * but, gpointer data)

  \brief select / unselect atom(s) in slab callback

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_this_slab (GtkButton * but, gpointer data)
{
  project * this_proj = (project *)data;
  this_proj -> modelgl -> cell_win -> cut_this_slab = TRUE;
  opengl_project_changed (this_proj -> id);
  selected_aspec = -1;
#ifdef GTK4
  select_unselect_atoms (NULL, NULL, & this_proj -> modelgl -> colorp[0][0]);
#else
  select_unselect_atoms (NULL, & this_proj -> modelgl -> colorp[0][0]);
#endif
  save_all_selections (this_proj -> modelgl, 0);
  create_slab_lists (this_proj);
  update_all_selections (this_proj -> modelgl, 0);
  if (this_proj -> modelgl -> cell_win -> slab_act) invert_selection (this_proj);
  this_proj -> modelgl -> cell_win -> cut_this_slab = FALSE;
  this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
  int shaders[1] = {SELEC};
  re_create_md_shaders (1, shaders, this_proj);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void cut_this_slab (GtkButton * but, gpointer data)

  \brief cut this slab callback

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void cut_this_slab (GtkButton * but, gpointer data)
{
  project * this_proj = (project *)data;
  int is_out, i;
  if (this_proj -> modelgl-> cell_win -> slab_atoms)
  {
    if (! this_proj -> modelgl -> cell_win -> slab_passivate || do_we_have_objects_in_selection(this_proj, this_proj -> modelgl -> search_widg[8], FALSE))
    {
      is_out = this_proj -> modelgl -> cell_win -> slab_out;
      if (this_proj -> modelgl -> search_widg[8]) this_proj -> modelgl -> search_widg[8] -> passivating = FALSE;
      gchar * infom[2] = {"Cut and modify model ?\n This is irreversible !", "Cut and create new project ?"};
      if (ask_yes_no("Cut", infom[is_out], GTK_MESSAGE_WARNING, this_proj -> modelgl -> cell_win -> win))
      {
        this_proj -> modelgl -> cell_win -> cut_this_slab = TRUE;
        if (is_out) preserve_ogl_selection (this_proj -> modelgl);
        opengl_project_changed (this_proj -> id);
        selected_aspec = -1;
#ifdef GTK4
        select_unselect_atoms (NULL, NULL, & this_proj -> modelgl -> colorp[0][0]);
#else
        select_unselect_atoms (NULL, & this_proj -> modelgl -> colorp[0][0]);
#endif
        save_all_selections (this_proj -> modelgl, 0);
        create_slab_lists (this_proj);
        update_all_selections (this_proj -> modelgl, 0);
        is_selected = 1;
        if (this_proj -> modelgl -> cell_win -> slab_act) invert_selection (this_proj);
        switch (is_out)
        {
          case 0:
            // Modify project
            if (this_proj -> modelgl -> cell_win -> slab_passivate)
            {
              if (this_proj -> modelgl -> atom_win)
              {
#ifdef GTK4
                remove_the_atoms (NULL, NULL, & cut_sel);
#else
                remove_the_atoms (NULL, & cut_sel);
#endif
                this_proj -> modelgl -> cell_win -> slab_passivate = FALSE;
                to_passivate_using_the_objects (this_proj, this_proj -> modelgl -> search_widg[8]);
                this_proj -> modelgl -> cell_win -> slab_passivate = TRUE;
              }
            }
            else
            {
#ifdef GTK4
              remove_the_atoms (NULL, NULL, & cut_sel);
#else
              remove_the_atoms (NULL, & cut_sel);
#endif
            }
            break;
          case 1:
            // Create new project
            if (this_proj -> modelgl -> cell_win -> slab_passivate)
            {
#ifdef GTK4
              select_unselect_atoms (NULL, NULL, & this_proj -> modelgl -> colorp[0][0]);
#else
              select_unselect_atoms (NULL, & this_proj -> modelgl -> colorp[0][0]);
#endif
            }
#ifdef GTK4
            edit_in_new_project (NULL, NULL, & this_proj -> modelgl -> colorp[0][0]);
#else
            edit_in_new_project (NULL, & this_proj -> modelgl -> colorp[0][0]);
#endif
            if (this_proj -> modelgl -> cell_win -> slab_passivate)
            {
#ifdef GTK4
              select_unselect_atoms (NULL, NULL, & this_proj -> modelgl -> colorp[0][0]);
#else
              select_unselect_atoms (NULL, & this_proj -> modelgl -> colorp[0][0]);
#endif
              this_proj -> modelgl -> cell_win -> cut_this_slab = FALSE;
              this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
              update (this_proj -> modelgl);
              active_project_changed (nprojects-1);
              active_glwin -> cell_win = g_malloc0(sizeof*active_glwin -> cell_win);
              active_glwin -> cell_win -> slab_lot = allocint (active_project -> nspec);
              active_glwin -> cell_win -> slab_type = this_proj -> modelgl -> cell_win ->slab_type;
              active_glwin -> cell_win -> slab_pbc = this_proj -> modelgl -> cell_win ->slab_pbc;
              active_glwin -> cell_win -> slab_act = this_proj -> modelgl -> cell_win -> slab_act;
              active_glwin -> cell_win -> slab_passivate = TRUE;
              for (i=0; i<21; i++) active_glwin -> cell_win -> cparam[i] = this_proj -> modelgl -> cell_win -> cparam[i];
              prepare_atom_edition (& active_glwin -> colorp[0][0], FALSE);
              active_glwin -> search_widg[8] = allocate_atom_search (activep, REPLACE, 8, this_proj -> modelgl -> search_widg[8] -> todo_size);
              active_glwin -> search_widg[8] -> status = 2;
              active_glwin -> search_widg[8] -> object = this_proj -> modelgl -> search_widg[8] -> object;
              active_glwin -> search_widg[8] -> filter = this_proj -> modelgl -> search_widg[8] -> filter;
              active_glwin -> search_widg[8] -> in_selection = this_proj -> modelgl -> search_widg[8] -> in_selection;
              active_glwin -> search_widg[8] -> todo_size = this_proj -> modelgl -> search_widg[8] -> todo_size;
              active_glwin -> search_widg[8] -> todo = duplicate_int (active_glwin -> search_widg[8] -> todo_size, this_proj -> modelgl -> search_widg[8] -> todo);
              active_glwin -> atom_win -> to_be_inserted[3] = duplicate_atomic_object (this_proj -> modelgl -> atom_win -> to_be_inserted[3]);
              atomic_object * tmp_a, * tmp_b;
              tmp_a = this_proj -> modelgl -> atom_win -> to_be_inserted[3];
              tmp_b = active_glwin -> atom_win -> to_be_inserted[3];
              while (tmp_a -> next)
              {
                tmp_b -> next = duplicate_atomic_object (tmp_a -> next);
                tmp_b -> next -> prev = tmp_b;
                tmp_b = tmp_b -> next;
                tmp_a = tmp_a -> next;
              }
              opengl_project_changed (active_project -> id);
#ifdef GTK4
              select_unselect_atoms (NULL, NULL, & active_glwin -> colorp[0][0]);
#else
              select_unselect_atoms (NULL, & active_glwin -> colorp[0][0]);
#endif
              save_all_selections (active_glwin, 0);
              active_glwin -> cell_win -> cut_this_slab = TRUE;
              create_slab_lists (active_project);
              update_all_selections (active_glwin, 0);
              if (active_glwin -> cell_win -> slab_act) invert_selection (active_project);
#ifdef GTK4
              remove_the_atoms (NULL, NULL, & cut_sel);
#else
              remove_the_atoms (NULL, & cut_sel);
#endif
              active_glwin -> cell_win -> slab_passivate = FALSE;
              to_passivate_using_the_objects (active_project, active_glwin -> search_widg[8]);
              active_glwin -> cell_win -> slab_passivate = TRUE;
              g_free (active_glwin -> search_widg[8]);
              active_glwin -> search_widg[8] = NULL;
              if (! active_glwin -> atom_win -> visible)
              {
                g_free (active_glwin -> atom_win);
                active_glwin -> atom_win = NULL;
              }
              g_free (active_glwin -> cell_win);
              active_glwin -> cell_win = NULL;
            }
            break;
        }
#ifdef GTK4
//        select_unselect_atoms (NULL, NULL, & this_proj -> modelgl -> colorp[0][0]);
#else
//        select_unselect_atoms (NULL, & this_proj -> modelgl -> colorp[0][0]);
#endif
        if (is_out) restore_ogl_selection (this_proj -> modelgl);
        this_proj -> modelgl -> cell_win -> cut_this_slab = FALSE;
        this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
        update (this_proj -> modelgl);
      }
    }
    else
    {
      show_info ("Nothing to passivate the bond(s) to be broken:\nDeactivate passivating or adjust option(s) properly !", 0, this_proj -> modelgl -> cell_win -> win);
    }
  }
  else
  {
    show_info ("The slab is empty, nothing to do !", 0, this_proj -> modelgl -> cell_win -> win);
  }
}

/*!
  \fn GtkWidget * cut_in_model (project * this_proj)

  \brief create the cut slab tab

  \param this_proj the target project
*/
GtkWidget * cut_in_model (project * this_proj)
{
  int i;
  GtkWidget * layout;
  i = (this_proj -> nspec > 3) ? 15 * (this_proj -> nspec - 3) : 0;
#ifdef G_OS_WIN32
  layout = create_layout (800, 510+i);
#else
  layout = create_layout (800, 420+i);
#endif
  GtkWidget * box = create_vbox (BSEP);
  layout_add_widget (layout, box, 10, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, markup_label("Cut slab in the simulation box: ", -1, -1, 0.0, 0.5), FALSE, FALSE, 5);

  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button ("<b>Show/Hide slab</b>", 25, -1, this_proj -> modelgl -> cell_win -> slab_show,
                                                   G_CALLBACK(set_slab_property), & this_proj -> modelgl -> cell_win -> slab_pointer[0]), FALSE, FALSE, 20);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button ("Use PBC to select atoms", 25, -1, this_proj -> modelgl -> cell_win -> slab_pbc,
                                                   G_CALLBACK(set_slab_property), & this_proj -> modelgl -> cell_win -> slab_pointer[1]), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button ("Invert cutting (all but the slab)", 25, -1, this_proj -> modelgl -> cell_win -> slab_act,
                                                   G_CALLBACK(set_slab_property), & this_proj -> modelgl -> cell_win -> slab_pointer[2]), FALSE, FALSE, 20);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button ("Export in new project", 25, -1, this_proj -> modelgl -> cell_win -> slab_out,
                                                   G_CALLBACK(set_slab_property), & this_proj -> modelgl -> cell_win -> slab_pointer[3]), FALSE, FALSE, 0);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 10);
  GtkWidget * hox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hox, FALSE, FALSE, 0);
  GtkWidget * combo = create_combo ();
  gchar * type[3] = {"Parallelepiped", "Cylinder", "Sphere"};
  for (i=0; i<3; i++) combo_text_append (combo, type[i]);
  combo_set_active (combo, this_proj -> modelgl -> cell_win -> slab_type);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_slab_type), this_proj);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hox, markup_label("- Shape of the slab: ", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hox, combo, FALSE, FALSE, 5);

  // Passivation
  hox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hox, FALSE, FALSE, 100);
  GtkWidget * but = check_button ("Passivate surface", -1, -1, this_proj -> modelgl -> cell_win -> slab_passivate,
                                 G_CALLBACK(set_slab_property), & this_proj -> modelgl -> cell_win -> slab_pointer[4]);
  // widget_set_sensitive (but, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hox, but, FALSE, FALSE, 20);
  this_proj -> modelgl -> cell_win -> passivate = create_button ("Options", IMG_NONE, NULL, 100, -1, GTK_RELIEF_NORMAL, G_CALLBACK(setup_passivate), this_proj);
  widget_set_sensitive (this_proj -> modelgl -> cell_win -> passivate, this_proj -> modelgl -> cell_win -> slab_passivate);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hox, this_proj -> modelgl -> cell_win -> passivate, FALSE, FALSE, 0);

  GtkWidget * hhbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hhbox, FALSE, FALSE, 5);
  GtkWidget * vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, vbox, FALSE, FALSE, 0);
  this_proj -> modelgl -> cell_win -> slab_opts = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, this_proj -> modelgl -> cell_win -> slab_opts, FALSE, FALSE, 5);
  for (i=0; i<3; i++)
  {
    this_proj -> modelgl -> cell_win -> slab_hbox[i] = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> modelgl -> cell_win -> slab_opts, this_proj -> modelgl -> cell_win -> slab_hbox[i], FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, this_proj -> modelgl -> cell_win -> slab_hbox[i], markup_label("- Select option to tweak: ", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
    this_proj -> modelgl -> cell_win -> slab_param[i] = create_slab_param_combo (i, this_proj);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, this_proj -> modelgl -> cell_win -> slab_hbox[i], this_proj -> modelgl -> cell_win -> slab_param[i], FALSE, FALSE, 5);
  }
  for (i=0; i<6; i++)
  {
    this_proj -> modelgl -> cell_win -> slab_box[i] = prepare_slab_box (i, this_proj);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> modelgl -> cell_win -> slab_opts, this_proj -> modelgl -> cell_win -> slab_box[i], FALSE, FALSE, 5);
  }

  vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, vbox, FALSE, FALSE, 40);
  GtkWidget * vvbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, vvbox, FALSE, FALSE, 20);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, markup_label("<u>Slab information:</u>", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("- Opacity: ", 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, create_hscale (0.0, 1.0, 0.01, this_proj -> modelgl -> cell_win -> slab_alpha, GTK_POS_LEFT, 2, 150,
                                            G_CALLBACK(set_slab_alpha), G_CALLBACK(scroll_set_slab_alpha), this_proj -> modelgl), FALSE, FALSE, 10);
  this_proj -> modelgl -> cell_win -> slab_info_box = create_vbox (BSEP);
  create_slab_info (this_proj);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, this_proj -> modelgl -> cell_win -> slab_info_box, FALSE, FALSE, 5);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, create_button ("Select atom(s) in this slab !", IMG_NONE, NULL, 100, -1, GTK_RELIEF_NORMAL, G_CALLBACK(select_this_slab), this_proj), FALSE, FALSE, 100);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>or</b>", 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, create_button ("Cut this slab now !", IMG_NONE, NULL, 100, -1, GTK_RELIEF_NORMAL, G_CALLBACK(cut_this_slab), this_proj), FALSE, FALSE, 30);

  show_the_widgets (layout);
  return layout;
}
