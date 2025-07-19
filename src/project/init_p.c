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
* @file init_p.c
* @short Functions to initialize an atomes project
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'init_p.c'
*
* Contains:
*

  - The functions to initialize an atomes project

*
* List of functions:

  void init_curves_and_calc (project * this_proj);
  void apply_default_parameters_to_project (project * this_proj);
  void init_project (gboolean alloc_box);

*/

#include "global.h"
#include "bind.h"
#include "callbacks.h"
#include "interface.h"
#include "preferences.h"
#include "project.h"
#include "curve.h"
#include "glview.h"
#include "glwindow.h"

extern void init_camera (project * this_proj, int get_depth);
extern void setup_default_image (project * this_proj, image * img);
extern void clean_atom_style (project * this_proj);
extern void setup_default_species_parameters_for_image (project * this_proj, image * img);
extern void init_shaders (glwin * view);
extern G_MODULE_EXPORT void box_advanced (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void axis_advanced (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void representation_advanced (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void opengl_advanced (GtkWidget * widg, gpointer data);
extern void update_gradient_widgets (gradient_edition * gradient_win, background * back);
extern G_MODULE_EXPORT void gradient_advanced (GtkWidget * widg, gpointer data);

/*!
  \fn void init_curves_and_calc (project * this_proj)

  \brief for a project reset analysis, curves, data to not performed

  \param this_proj the target project
*/
void init_curves_and_calc (project * this_proj)
{
  int i;
  for (i=0; i<NGRAPHS; i++)
  {
    this_proj -> runok[i] = FALSE;
    this_proj -> initok[i] = FALSE;
    this_proj -> visok[i] = FALSE;
  }
}

/*!
  \fn void apply_default_parameters_to_project (project * this_proj)

  \brief apply new default parameters to project

  \param this_proj the target project
*/
void apply_default_parameters_to_project (project * this_proj)
{
  int i, j;
  // Calc parameters
  if (this_proj -> chemistry)
  {
    this_proj -> chemistry -> grtotcutoff = default_totcut;
  }
  for (i=0; i<6; i++) this_proj -> num_delta[i] = default_num_delta[i];
  this_proj -> num_delta[SP] = default_num_delta[6];
  this_proj -> num_delta[MS] = default_num_delta[7];
  this_proj -> delta[MS] = default_delta_t[0];
  for (i=0; i<5; i++)
  {
    this_proj -> rsparam[i][0] = default_rsparam[1];
    this_proj -> rsparam[i][1] = default_rsparam[2];
    for (j=2; j<5; j++) this_proj -> rsparam[i][j] = default_rsparam[j+2];
  }
  this_proj -> rsearch[0] = default_rsparam[0];
  this_proj -> rsearch[1] = default_rsparam[3];
  this_proj -> csparam[0] = default_csparam[0];
  for (i=1; i<4; i++) this_proj -> csparam[i]= default_csparam[i+2];
  this_proj -> csparam[5] = default_csparam[1];
  this_proj -> csearch = default_csparam[2];
  if (this_proj -> modelgl)
  {
    image * img = this_proj -> modelgl -> anim -> last -> img;
    if (img)
    {
      preferences = FALSE;
      setup_default_image (this_proj, img);
      preferences = TRUE;
      init_camera (this_proj, FALSE);
      setup_default_species_parameters_for_image (this_proj, img);
      clean_atom_style (this_proj);
      init_shaders (this_proj -> modelgl);
      update (this_proj -> modelgl);
      if (this_proj -> modelgl -> box_win)
      {
        if (this_proj -> modelgl -> box_win -> win) this_proj -> modelgl -> box_win -> win = destroy_this_widget (this_proj -> modelgl -> box_win -> win);
        g_free (this_proj -> modelgl -> box_win);
        this_proj -> modelgl -> box_win = NULL;
        box_advanced (NULL, this_proj -> modelgl);
      }
      if (this_proj -> modelgl -> axis_win)
      {
        if (this_proj -> modelgl -> axis_win -> win) this_proj -> modelgl -> axis_win -> win = destroy_this_widget (this_proj -> modelgl -> axis_win -> win);
        g_free (this_proj -> modelgl -> axis_win);
        this_proj -> modelgl -> axis_win = NULL;
        axis_advanced (NULL, this_proj -> modelgl);
      }
      if (this_proj -> modelgl -> rep_win)
      {
        if (this_proj -> modelgl -> rep_win -> win) this_proj -> modelgl -> rep_win -> win = destroy_this_widget (this_proj -> modelgl -> rep_win -> win);
        g_free (this_proj -> modelgl -> rep_win);
        this_proj -> modelgl -> rep_win = NULL;
        representation_advanced (NULL, this_proj -> modelgl);
      }
      if (this_proj -> modelgl -> opengl_win)
      {
        if (this_proj -> modelgl -> opengl_win -> win) this_proj -> modelgl -> opengl_win -> win = destroy_this_widget (this_proj -> modelgl -> opengl_win -> win);
        g_free (this_proj -> modelgl -> opengl_win);
        this_proj -> modelgl -> opengl_win = NULL;
        opengl_advanced (NULL, this_proj -> modelgl);
      }
      if (this_proj -> modelgl -> gradient_win) update_gradient_widgets (this_proj -> modelgl -> gradient_win, img -> back);
    }
  }
}

/*!
  \fn void init_project (gboolean alloc_box)

  \brief initialize a new project

  \param alloc_box allocate data for the MD box (1/0)
*/
void init_project (gboolean alloc_box)
{
  project * new_proj = g_malloc0 (sizeof*proj);
  nprojects ++;
  activep = nprojects - 1;
  new_proj -> id = activep;
  new_proj -> name = g_strdup_printf("%s%2d", "Project N°", activep);
  new_proj -> delta[RI] = new_proj -> delta[CH] = 1.0;
  new_proj -> min[RI] = new_proj -> min[CH] = 1;
  new_proj -> delta[SP] = 2.0;
  apply_default_parameters_to_project (new_proj);
  new_proj -> tfile = -1;
  new_proj -> newproj = TRUE;
  new_proj -> steps = 1;
  new_proj -> xcor = 1;
  new_proj -> tunit = (int)default_delta_t[1];

  new_proj -> sk_advanced[0] = 1.0;
  new_proj -> sk_advanced[1] = 15.0;

  new_proj -> coord = g_malloc0 (sizeof*new_proj -> coord);
  if (alloc_box) new_proj -> cell.box = g_malloc0(sizeof*new_proj -> cell.box);

  remove_edition_actions ();
  init_curves_and_calc (new_proj);
  new_proj -> numwid = -1;
  if (nprojects == 1)
  {
    workzone.first = g_malloc0 (sizeof*workzone.first);
    workzone.first = new_proj;
    workzone.last = g_malloc0 (sizeof*workzone.last);
  }
  else
  {
    new_proj -> prev = workzone.last;
    workzone.last -> next = new_proj;
  }
  workzone.last = new_proj;
  active_project_changed (new_proj -> id);
  prep_calc_actions ();
  new_proj -> newproj = FALSE;
}
