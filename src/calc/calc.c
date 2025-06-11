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
* @file calc.c
* @short Callbacks used in by the molecular dynamics calculation assistants \n
         Atom selection structure manipulation tools \n
         The initialization of the assistants
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'calc.c'
*
* Contains:
*

 - Callbacks used in by the molecular dynamics calculation assistant
 - Atom selection structure manipulation tools
 - The initialization of the assistants

*
* List of functions:

  void field_question (gchar * question, GCallback handler, gpointer data);
  void unselect_all_atoms (glwin * view);
  void restore_ogl_selection (glwin * view);
  void preserve_ogl_selection (glwin * view);

  G_MODULE_EXPORT void confirm_selection (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void create_field (GtkWidget * widg, gpointer data);

  atom_selection * duplicate_ogl_selection (atom_selection * old_sel);

*/

#include "global.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"

#define MAXDATAQM 7

extern void create_classical_force_field (int p, int f);
extern void create_qm_input_file (int c, int p, int s);
extern void create_new_project_using_data (atom_selection * selection);
extern int selected_aspec;
extern int num_bonds (int i);
extern int num_angles (int i);
extern int num_dihedrals (int i);
extern atom_in_selection * new_atom_in_selection (int id, int sp);

GtkWidget * qm_assistant;
project * qm_proj;
glwin * qm_view;
coord_info * qm_coord;
GtkTextBuffer * qmbuffer[MAXDATAQM+2];
gboolean force_mol = FALSE;
int idopt;
int icalc;
int ident;
int icomb;

gboolean selection_confirmed;

/*!
  \fn G_MODULE_EXPORT void confirm_selection (GtkDialog * dialog, gint response_id, gpointer data)

  \brief confirm that the selection is good

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the pointer if needed, not in this case
*/
G_MODULE_EXPORT void confirm_selection (GtkDialog * dialog, gint response_id, gpointer data)
{
  selection_confirmed = (response_id == GTK_RESPONSE_YES) ? TRUE : FALSE;
  destroy_this_dialog (dialog);
}

/*!
  \fn void field_question (gchar * question, GCallback handler, gpointer data)

  \brief ask the use to confirm something

  \param question the text to display
  \param handler the callback to use
  \param data the data to transmit to the callback
*/
void field_question (gchar * question, GCallback handler, gpointer data)
{
  GtkWidget * dialog;
  dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Are you sure ?");
  gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s", question);
  gtk_window_set_title(GTK_WINDOW(dialog), "Please confirm");
  run_this_gtk_dialog (dialog, handler, data);
}

/*!
  \fn atom_selection * duplicate_ogl_selection (atom_selection * old_sel)

  \brief copy an atom selection

  \param old_sel the atom selection to copy
*/
atom_selection * duplicate_ogl_selection (atom_selection * old_sel)
{
  int i, j;
  atom_in_selection * at, * bt;
  atom_selection * new_sel = g_malloc0 (sizeof*new_sel);
  if (! old_sel -> selected) return new_sel;
  new_sel -> selected = old_sel -> selected;
  at = old_sel -> first;
  for (i=0; i< old_sel -> selected; i++)
  {
    if (! i)
    {
      new_sel -> first = new_atom_in_selection (at -> id, at -> sp);
      bt = new_sel -> first;
      new_sel -> last = NULL;
    }
    else
    {
      bt -> next = new_atom_in_selection (at -> id, at -> sp);
      bt -> next -> prev = bt;
      bt = bt -> next;
    }
    if (at -> next != NULL) at = at -> next;
  }
  bt -> next = NULL;
  new_sel -> last = bt;
  if (old_sel -> selected > 1 && old_sel -> selected < MAX_IN_SELECTION)
  {
    i = num_bonds (old_sel -> selected);
    new_sel -> selected_bonds = allocint (i);
    for (j=0; j<i; j++)
    {
      new_sel -> selected_bonds[j] = old_sel -> selected_bonds[j];
    }
    if (new_sel -> selected > 2)
    {
      i = num_angles (old_sel -> selected);
      new_sel -> selected_angles = allocint (i);
      for (j=0; j<i; j++)
      {
        new_sel -> selected_angles[j] = old_sel -> selected_angles[j];
      }
      if (new_sel -> selected > 3 && new_sel -> selected < MAX_IN_SELECTION-10)
      {
        i = num_dihedrals (old_sel -> selected);
        new_sel -> selected_dihedrals = allocint (i);
        for (j=0; j<i; j++)
        {
          new_sel -> selected_dihedrals[j] = old_sel -> selected_dihedrals[j];
        }
      }
    }
  }
  return new_sel;
}

/*!
  \fn void unselect_all_atoms (glwin * view)

  \brief remove all atom(s) from selection

  \param view the glwin the selection comes from
*/
void unselect_all_atoms (glwin * view)
{
  int i, j, k;
  project * this_proj = get_project_by_id (view -> proj);
  for (i=0; i<2; i++)
  {
    save_all_selections (view, i);
    for (j=0; j<this_proj -> steps; j++)
    {
      for (k=0; k<this_proj -> natomes; k++)
      {
        if (j == view -> anim -> last -> img -> step)
        {
          if (this_proj -> atoms[j][k].pick[i])
          {
            process_selected_atom (this_proj, view, k, 0, 0, i);
          }
        }
        else
        {
          this_proj -> atoms[j][k].pick[i] = FALSE;
        }
      }
    }
    update_all_selections (view, i);
  }
  init_default_shaders (view);
}

/*!
  \fn void restore_ogl_selection (glwin * view)

  \brief restore a saved atom selection

  \param view the glwin to restore the selection to
*/
void restore_ogl_selection (glwin * view)
{
  int i, j, k, l;
  project * this_proj = get_project_by_id (view -> proj);
  unselect_all_atoms (view);
  for (i=0; i<2; i++)
  {
    view -> anim -> last -> img -> selected[i] = duplicate_ogl_selection (view -> tmp_sel[i]);
    if (view -> anim -> last -> img -> selected[i] -> selected)
    {
      atom_in_selection * at = view -> anim -> last -> img -> selected[i] -> first;
      while (at)
      {
        for (j=0; j<this_proj -> steps; j++)
        {
          this_proj -> atoms[j][at -> id].pick[i] = TRUE;
        }
        at = at -> next;
      }
    }
    if (view -> stored_labels[i])
    {
      for (j=1; j<view -> stored_labels[i][0]; j++)
      {
        k = view -> stored_labels[i][j];
        for (l=0; l<this_proj -> steps; l++) this_proj -> atoms[l][k].label[i] = TRUE;
      }
      g_free (view -> stored_labels[i]);
      view -> stored_labels[i] = NULL;
    }
  }
  init_default_shaders (view);
  if (check_label)
  {
    view -> labelled = check_label_numbers (this_proj, 2);
    update (view);
  }
}

/*!
  \fn void preserve_ogl_selection (glwin * view)

  \brief copy the atom selection, so that it can be re-used once the input assistant is closed.

  \param view the glwin the selection comes from
*/
void preserve_ogl_selection (glwin * view)
{
  int h, i, j, k, l;
  project * this_proj = get_project_by_id (view -> proj);
  h = view -> anim -> last -> img -> step;
  k = 0;
  for (i=0; i<2; i++)
  {
    view -> tmp_sel[i] = duplicate_ogl_selection (view -> anim -> last -> img -> selected[i]);
    for (j=0; j<2; j++)
    {
      if (j && k)
      {
        view -> stored_labels[i] = allocint(k+1);
        view -> stored_labels[i][0] = k+1;
      }
      k = j;
      for (l=0; l<this_proj -> natomes; l++)
      {
        if (this_proj -> atoms[h][l].label[i])
        {
          if (j)
          {
            view -> stored_labels[i][k] = l;
            this_proj -> atoms[h][l].label[i] = FALSE;
          }
          k ++;
        }
      }
    }
  }
  unselect_all_atoms (view);
}

extern char * input_types[NINPUTS];

/*!
  \fn G_MODULE_EXPORT void create_field (GtkWidget * widg, gpointer data)

  \brief start an input creation assistant

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void create_field (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  int p = the_data -> a;
  int t = the_data -> b;
  // if a selection was made preserve it
  opengl_project_changed (p);
  preserve_ogl_selection (opengl_project -> modelgl);
  update (opengl_project -> modelgl);

  if (opengl_project -> steps > 1)
  {
    gchar * info = g_strdup_printf (" %s contains %d different sets of atomic coordinates. \n"
                                    " Please select the one to use to create the input file(s) \n"
                                    " Note that a new project will be generated for that purpose. \n",
                                    opengl_project -> name,
                                    opengl_project -> steps);
    // preserve parameters
    int tmp_s = opengl_project -> modelgl -> anim -> last -> img -> step;
    int tmp_a = activep;
    int tmp_v = activev;
    tint t_data;
    t_data.a = p;
    t_data.b = iask (info, "Enter the selected MD step: ", 5+opengl_project -> steps, opengl_project -> modelgl -> win);
    t_data.c = 1;
    g_free (info);

    // then change the MD step and select all atoms
    opengl_project -> modelgl -> anim -> last -> img -> step = t_data.b;
    selected_aspec = -1;
#ifdef GTK4
   select_unselect_atoms (NULL, NULL, & t_data);
#else
    select_unselect_atoms (NULL, & t_data);
#endif
    // create new project with selection
    force_mol = TRUE;
    create_new_project_using_data (opengl_project -> modelgl -> anim -> last -> img -> selected[0]);
    force_mol = FALSE;
    opengl_project -> modelgl -> anim -> last -> img -> step = tmp_s;
    restore_ogl_selection (opengl_project -> modelgl);
    // Set the new project to be use for input creation
    p = activep;
    preserve_ogl_selection (active_glwin);
    // restore old active project
    active_project_changed (tmp_a);
    opengl_project_changed (tmp_v);
  }

  switch (t)
  {
    case 0:
      // DLPOLY
      create_classical_force_field (p, 0);
      break;
    case 1:
      // LAMMPS
      create_classical_force_field (p, 1);
      break;
    case 2:
      create_qm_input_file (0, p, 0);
      break;
    case 3:
      create_qm_input_file (1, p, 0);
      break;
    case 4:
      create_qm_input_file (0, p, 1);
      break;
    case 5:
      create_qm_input_file (1, p, 1);
      break;
  }
}

