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
* @file update_p.c
* @short Functions to update a project \n
         Functions to activate a project
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'update_p.c'
*
* Contains:
*

 - The functions to update a project
 - The functions to activate a project

*
* List of functions:

  int update_project ();

  void prep_calc_actions ();
  void active_project_changed (int id);
  void opengl_project_changed (int id);
  void update_analysis_availability (project * this_proj);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "workspace.h"

extern GtkWidget * work_menu (int p, int c);
extern GtkTreeStore * tool_model;

/*!
  \fn void prep_calc_actions ()

  \brief prepare analysis widgets
*/
void prep_calc_actions ()
{
  int i;
  // Depends on the number of calculations available
  for (i=0; i<G_N_ELEMENTS(analyze_actions); i++)
  {
#ifdef NEW_ANA
    if (! active_project -> analysis)
    {
      remove_action (analyze_acts[i].action_name);
    }
    else
    {
#endif
    if (i < ANG)
    {
#ifdef NEW_ANA
      if (active_project -> analysis[i])
      {
        if (active_project -> analysis[i] -> avail_ok)
        {
          add_action (analyze_actions[i]);
        }
        else
        {
          remove_action (analyze_acts[i].action_name);
        }
      }
      else
      {
        remove_action (analyze_acts[i].action_name);
      }
#else
      if (active_project -> runok[i])
      {
        add_action (analyze_actions[i]);
      }
      else
      {
        remove_action (analyze_acts[i].action_name);
      }
#endif
    }
    else
    {
#ifdef NEW_ANA
      if (active_project -> analysis[i+1])
      {
        if (active_project -> analysis[i+1] -> avail_ok)
        {
          add_action (analyze_actions[i]);
        }
        else
        {
          remove_action (analyze_acts[i].action_name);
        }
      }
      else
      {
        remove_action (analyze_acts[i].action_name);
      }
#else
      if (active_project -> runok[i+1])
      {
        add_action (analyze_actions[i]);
      }
      else
      {
        remove_action (analyze_acts[i].action_name);
      }
#endif
    }
#ifdef NEW_ANA
  }
#endif // NEW_ANA
  }
}

/*!
  \fn void update_analysis_availability (project * this_proj)

  \brief update analysis availability for a target projet

  \param this_proj the target project
*/
void update_analysis_availability (project * this_proj)
{
  if (this_proj -> natomes && this_proj -> numwid)
  {
    if (this_proj -> cell.has_a_box)
    {
#ifdef NEW_ANA
      this_proj -> analysis[GDR] -> avail_ok = TRUE;
      this_proj -> analysis[SKD] -> avail_ok = TRUE;
#else
      this_proj -> runok[GDR] = TRUE;
      this_proj -> runok[SKD] = TRUE;
#endif
    }
    else
    {
#ifdef NEW_ANA
      this_proj -> analysis[GDR] -> avail_ok = FALSE;
      this_proj -> analysis[SQD] -> avail_ok = FALSE;
      this_proj -> analysis[SKD] -> avail_ok = FALSE;
      this_proj -> analysis[SKD] -> avail_ok = FALSE;
#else
      this_proj -> runok[GDR] = FALSE;
      this_proj -> runok[SQD] = FALSE;
      this_proj -> runok[SKD] = FALSE;
      this_proj -> runok[GDK] = FALSE;
#endif
    }
#ifdef NEW_ANA
    this_proj -> analysis[BND] -> avail_ok = TRUE;
    this_proj -> analysis[ANG] -> avail_ok = TRUE;
    this_proj -> analysis[RIN] -> avail_ok = TRUE;
    this_proj -> analysis[CHA] -> avail_ok = TRUE;
    this_proj -> analysis[SPH] -> avail_ok = TRUE;
    if (this_proj -> steps > 1) this_proj -> analysis[MSD] -> avail_ok = TRUE;
#else
    this_proj -> runok[BND] = TRUE;
    this_proj -> runok[ANG] = TRUE;
    this_proj -> runok[RIN] = TRUE;
    this_proj -> runok[CHA] = TRUE;
    this_proj -> runok[SPH] = TRUE;
    if (this_proj -> steps > 1) this_proj -> runok[MSD] = TRUE;
#endif
  }
  else
  {
    int i;
    for (i=0; i<NGRAPHS; i++)
    {
      this_proj -> analysis[i] -> avail_ok = FALSE;
    }
  }
}

/*!
  \fn int update_project ()

  \brief update project: send data to Fortran90, and update calculation interactors
*/
int update_project ()
{
#ifdef DEBUG
  g_debug ("UPDATE_PROJECT: to update");
#endif
  int i, j;
  if (! active_project -> newproj && active_project -> natomes)
  {
    i = alloc_data_ (& active_project -> natomes,
                     & active_project -> nspec,
                     & active_project -> steps);
    if (i == 1)
    {
      to_read_pos ();
      int * lot = allocint (active_project -> natomes);
      for (j=0; j<active_project -> natomes; j++) lot[j] = active_project -> atoms[0][j].sp;
      read_data_ (lot, active_chem -> nsps);
      g_free (lot);
      read_chem_ (active_chem -> chem_prop[CHEM_M], active_chem -> chem_prop[CHEM_R],
                  active_chem -> chem_prop[CHEM_N], active_chem -> chem_prop[CHEM_X]);
      j = 0;
      prep_spec_ (active_chem -> chem_prop[CHEM_Z], active_chem -> nsps, & j);
    }
    else
    {
      return 0;
    }
    if (active_project -> run)
    {
      active_project -> dmtx = FALSE;
      j = (active_cell -> npt) ? active_project -> steps : 1;
      for (i=0; i<j; i++)
      {
        lattice_ (& j, & i,
                  active_cell -> box[i].vect,
                  active_cell -> box[i].param[0],
                  active_cell -> box[i].param[1],
                  & active_cell -> ltype,
                  & active_cell -> frac,
                  & active_cell -> pbc);

      }
      cutoffsend ();
    }
  }
  update_analysis_availability (active_project);
#ifdef DEBUG
  g_debug ("UPDATE_PROJECT: updated");
#endif
  return 1;
}

/*!
  \fn void active_project_changed (int id)

  \brief change the active project

  \param id the id of the new active project
*/
void active_project_changed (int id)
{
  char * errp = NULL;
  if (id != inactep && inactep < nprojects && ! atomes_logo) clean_view ();
  gtk_tree_store_clear (tool_model);
  activep = id;
  active_project = get_project_by_id (id);
  active_chem = active_project -> chemistry;
  active_coord = active_project -> coord;
  active_cell = & active_project -> cell;
  active_box = NULL;
  active_glwin = NULL;
  active_image = NULL;
  if (active_project -> modelgl != NULL)
  {
    active_glwin = active_project -> modelgl;
    if (active_glwin -> anim != NULL)
    {
      active_image = active_glwin -> anim -> last -> img;
    }
  }

  if (active_cell -> box)
  {
    if (active_project -> steps == 1 || ! active_cell -> npt || ! active_image)
    {
      active_box = & active_cell -> box[0];
    }
    else if (active_cell -> npt)
    {
      active_box = & active_cell -> box[active_image -> step];
    }
  }
  if (update_project() == 0)
  {
    errp = g_strdup_printf ("Impossible to update project: %s\n", active_project -> name);
    show_error (errp, 0, MainWindow);
    g_free (errp);
  }
  else
  {
    if (active_project -> numwid > 0)
    {
      prep_calc_actions ();
      add_action (edition_actions[0]);
      if (active_cell -> npt)
      {
        remove_action (edition_acts[1].action_name);
      }
      else
      {
        add_action (edition_actions[1]);
      }
      add_action (edition_actions[2]);
      fill_tool_model ();
      correct_this_window_title (curvetoolbox, g_strdup_printf ("Toolboxes - %s", prepare_for_title(active_project -> name)));
      correct_this_window_title (MainWindow, g_strdup_printf ("%s - %s", PACKAGE, prepare_for_title (active_project -> name)));
    }
    inactep = activep;
  }
  activew = activep;
}

/*!
  \fn void opengl_project_changed (int id)

  \brief change the OpenGL project

  \param id the id of the new OpenGL project
*/
void opengl_project_changed (int id)
{
  activev = id;
  opengl_project = get_project_by_id(id);
}
