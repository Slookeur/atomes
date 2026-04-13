/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2026 by CNRS and University of Strasbourg */

/*!
* @file close_p.c
* @short Functions to close an atomes project \n
         Callbacks to close an atomes project
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'close_p.c'
*
* Contains:
*

 - the functions to close an atomes project
 - the callbacks to close an atomes project

*
* List of functions:

  void update_insert_combos ();
  void free_glwin (glwin * to_clow, int steps, int nspec);
  void close_project (project * to_close);

  void to_close_this_project (int to_activate, project * this_proj);
  G_MODULE_EXPORT void on_close_activate (GtkWidget * widg, gpointer cdata);

*/

#include "global.h"
#include "bind.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"
#include "curve.h"
#include "glview.h"

extern GtkTreeStore * tool_model;
extern GtkTreeModel * replace_combo_tree (gboolean insert, int proj);
extern G_MODULE_EXPORT void spin_stop (GtkButton * but, gpointer data);

/*!
  \fn void update_insert_combos ()

  \brief update some GtkComboBox in the workspace if a project is removed
*/
void update_insert_combos ()
{
  GtkTreeModel * model;
  project * this_proj;
  GtkWidget * box;
  int i;
  for (i=0; i<nprojects; i++)
  {
    this_proj = get_project_by_id(i);
    if (this_proj -> modelgl)
    {
      if ((this_proj -> modelgl -> atom_win && this_proj -> modelgl -> atom_win -> visible) || this_proj -> modelgl -> builder_win)
      {
        model = replace_combo_tree (TRUE, i);
        box = (this_proj -> modelgl -> builder_win) ? this_proj -> modelgl -> builder_win -> add_combo : this_proj -> modelgl -> atom_win -> atom_combo[3];
        gtk_combo_box_set_model (GTK_COMBO_BOX(box), model);
        combo_set_active (box, 0);
        combo_set_markup (box);
        g_object_unref (model);
      }
    }
  }
}

/*!
  \fn void free_glwin (glwin * to_clow, int steps, int nspec)

  \brief free all memory related to a gliwn data structure

  \param to_clow the target gliwn to free
  \param steps the number of steps
  \param nspec the number of chemical species
*/
void free_glwin (glwin * to_clow, int steps, int nspec)
{
  int i, j, k;
  if (to_clow -> color_to_pick != NULL)
  {
    g_free (to_clow -> color_to_pick);
    to_clow -> color_to_pick = NULL;
  }

#ifdef GTK3
  for (i=0; i<2; i++)
  {
    g_free (to_clow -> ogl_box_axis[i]);
    for (j=0; j<10; j++)
    {
      if (to_clow -> ogl_geom[i][j]) g_free (to_clow -> ogl_geom[i][j]);
      if (to_clow -> oglmv[i][j]) g_free (to_clow -> oglmv[i][j]);
      if (j < 9)
      {
        if (to_clow -> ogl_poly[i][j]) g_free (to_clow -> ogl_poly[i][j]);
        if (to_clow -> oglmc[i][j]) g_free (to_clow -> oglmc[i][j]);
      }
    }
  }
#endif

  for (i=0; i<steps; i++)
  {
    if (to_clow -> bondid[i])
    {
      for (j=0; j<2; j++)
      {
        if (to_clow -> bondid[i][j])
        {
          for (k=0; k<to_clow -> allbonds[j]; k++)
          {
             if (to_clow -> bondid[i][j][k]) g_free(to_clow -> bondid[i][j][k]);
          }
          g_free(to_clow -> bondid[i][j]);
        }
      }
      g_free(to_clow -> bondid[i]);
    }
    if (to_clow -> clones[i]) g_free(to_clow -> clones[i]);
  }

  if (to_clow -> bondid) g_free(to_clow -> bondid);
  if (to_clow -> clones) g_free(to_clow -> clones);

  for (i=0; i<NUM_COLORS; i++)
  {
    g_free (to_clow -> colorp[i]);
  }
  if (to_clow -> rep_win)
  {
    to_clow -> rep_win -> win = destroy_this_widget (to_clow -> rep_win -> win);
    g_free (to_clow -> rep_win);
  }
  if (to_clow -> gradient_win)
  {
    to_clow -> gradient_win -> win = destroy_this_widget (to_clow -> gradient_win -> win);
    g_free (to_clow -> gradient_win);
  }
  if (to_clow -> box_win)
  {
    to_clow -> box_win -> win = destroy_this_widget (to_clow -> box_win -> win);
    g_free (to_clow -> box_win);
  }
  if (to_clow -> axis_win)
  {
    to_clow -> axis_win -> win = destroy_this_widget (to_clow -> axis_win -> win);
    g_free (to_clow -> axis_win);
  }
  if (to_clow -> measure_win)
  {
    to_clow -> measure_win -> win = destroy_this_widget (to_clow -> measure_win -> win);
    g_free (to_clow -> measure_win);
  }
  if (to_clow -> volume_win)
  {
    to_clow -> volume_win -> win = destroy_this_widget (to_clow -> volume_win -> win);
    g_free (to_clow -> volume_win);
  }
  if (to_clow -> player)
  {
    to_clow -> player -> win = destroy_this_widget (to_clow -> player -> win);
    g_free (to_clow -> player);
  }
  // Stop the spinning if any
  spin_stop (NULL, to_clow);
  if (to_clow -> spiner)
  {
    to_clow -> spiner -> win = destroy_this_widget (to_clow -> spiner -> win);
    g_free (to_clow -> spiner);
  }
  if (to_clow -> rec)
  {
    to_clow -> rec -> win = destroy_this_widget (to_clow -> rec -> win);
    g_free (to_clow -> rec);
  }
  if (to_clow -> atom_win)
  {
    to_clow -> atom_win -> win = destroy_this_widget (to_clow -> atom_win -> win);
    g_free (to_clow -> atom_win);
  }
  if (to_clow -> cell_win)
  {
    to_clow -> cell_win -> win = destroy_this_widget (to_clow -> cell_win -> win);
    g_free (to_clow -> cell_win);
  }
  if (to_clow -> builder_win)
  {
    to_clow -> builder_win -> win = destroy_this_widget (to_clow -> builder_win -> win);
    g_free (to_clow -> builder_win);
  }
  if (to_clow -> coord_win)
  {
    to_clow -> coord_win -> win = destroy_this_widget (to_clow -> coord_win -> win);
    g_free (to_clow -> coord_win);
  }
  for (i=0; i<2; i++)
  {
   if (to_clow -> model_win[i])
    {
      to_clow -> model_win[i] -> win = destroy_this_widget (to_clow -> model_win[i] -> win);
      g_free (to_clow -> model_win[i]);
    }
  }
  if (to_clow -> opengl_win)
  {
    to_clow -> opengl_win -> win = destroy_this_widget (to_clow -> opengl_win -> win);
    g_free (to_clow -> opengl_win);
  }
  to_clow -> win = destroy_this_widget (to_clow -> win);
  for (i=0; i<NGLOBAL_SHADERS; i++)
  {
    cleaning_shaders (to_clow, i);
    g_free (to_clow -> ogl_glsl[i]);
  }
  g_free (to_clow);
}

/*!
  \fn void close_project (project * to_close)

  \brief close a project

  \param to_close the project to close
*/
void close_project (project * to_close)
{
  int i, j, k, l;

#ifdef DEBUG
  g_debug ("CLOSE_PROJECT: proj to close= %d", to_close -> id);
  g_debug ("CLOSE_PROJECT: nprojects    = %d", nprojects);
  g_debug ("CLOSE_PROJECT: activep      = %d", activep);
  if (to_close -> pixels)
  {
    for (i=0; i<active_project -> pix[0]*active_project -> pix[1]*active_project -> pix[2]; i++)
    {
      g_free (to_close -> pixels[i]);
    }
    g_free (to_close -> pixels);
  }
#endif

  if (to_close -> modelgl)
  {
    free_glwin (to_close -> modelgl, to_close -> steps, to_close -> nspec);
    if (to_close -> modelfc)
    {
      if (to_close -> modelfc -> mols)
      {
        for (i=0; i< to_close -> steps; i++)
        {
          if (to_close -> modelfc -> mols[i])
          {
            g_free (to_close -> modelfc -> mols[i]);
          }
        }
        g_free (to_close -> modelfc -> mols);
      }
    }
  }
  if (to_close -> atoms)
  {
    for (i=0; i<to_close -> steps; i++)
    {
      for (j=0; j<to_close -> natomes; j++)
      {
        g_free (to_close -> atoms[i][j].vois);
      }
      if (to_close -> atoms[i]) g_free (to_close -> atoms[i]);
    }
    g_free (to_close -> atoms);
  }
  if (to_close -> cell.box) g_free (to_close -> cell.box);
  if (to_close -> cell.sp_group) g_free (to_close -> cell.sp_group);

  if (to_close -> run)
  {
    if (to_close -> analysis)
    {
      for (i=0 ; i<NCALCS ; i++)
      {
        if (to_close -> analysis[i])
        {
          to_close -> analysis[i] -> calc_ok = FALSE;
          if (to_close -> analysis[i] -> name) g_free (to_close -> analysis[i] -> name);
          if (to_close -> analysis[i] -> x_title) g_free (to_close -> analysis[i] -> x_title);
          if (to_close -> analysis[i] -> compat_id) g_free (to_close -> analysis[i] -> compat_id);
          if (to_close -> analysis[i] -> curves)
          {
            hide_curves (to_close, i);
            erase_curves (to_close, i);
            g_free (to_close -> analysis[i] -> idcc);
            g_free (to_close -> analysis[i] -> curves);
          }
          g_free (to_close -> analysis[i]);
        }
      }
      g_free (to_close -> analysis);
    }
  }

  if (! atomes_render_image) clean_view ();
  g_free (to_close -> projfile);

  if (nprojects > 1)
  {
    if (to_close == workzone.first)
    {
      workzone.first = workzone.first -> next;
      workzone.first -> prev = NULL;
    }
    else if (to_close == workzone.last)
    {
      workzone.last = workzone.last -> prev;
      workzone.last -> next = NULL;
    }
    else
    {
      to_close -> prev -> next = to_close -> next;
      to_close -> next -> prev = to_close -> prev;
    }
  }
  else if (! atomes_render_image)
  {
    // if (workzone.first) g_free (workzone.first);
    workzone.first = NULL;
    // if (workzone.last) g_free (workzone.last);
    workzone.last = NULL;
    activep = -1;
    correct_this_window_title (MainWindow, g_strdup_printf ("%s", PACKAGE));
    correct_this_window_title (curvetoolbox, g_strdup_printf ("Toolboxes"));
    if (workspacefile != NULL)
    {
      g_free (workspacefile);
      workspacefile = NULL;
    }
    newspace = TRUE;
  }
  g_free (to_close);
  nprojects --;

  if (nprojects)
  {
    project * this_proj = workzone.first;
    for (i=0 ; i<nprojects ; i++)
    {
      this_proj -> id = i;
      if (this_proj -> initgl && this_proj -> modelgl)
      {
        this_proj -> modelgl -> proj = i;
        for (j=0; j<NUM_COLORS; j++)
        {
          for (k=0; k<this_proj -> nspec*2; k++)
          {
            this_proj -> modelgl -> colorp[j][k].a = i;
          }
        }
        for (j=0; j<10; j++)
        {
          if (this_proj -> modelgl -> gcid[j] != NULL)
          {
            for (k=0; k<this_proj -> coord -> totcoord[j]; k++)
            {
              for (l=0; l<NUM_COLORS; l++)
              {
                this_proj -> modelgl -> gcid[j][k][l].a = i;
              }
            }
          }
        }
        if (this_proj -> modelgl -> atom_win)
        {
          for (j=0; j<8; j++)
          {
            if (this_proj -> modelgl -> search_widg[j])
            {
              for (k=0; k<3; k++) this_proj -> modelgl -> search_widg[j] -> pointer[k].a = i;
            }
          }
        }
      }
      if (this_proj -> analysis)
      {
        for (j=0; j<NCALCS; j++)
        {
          if (this_proj -> analysis[j])
          {
            if (this_proj -> analysis[j] -> idcc != NULL)
            {
              for (k=0; k<this_proj -> analysis[j] -> numc; k++)
              {
                this_proj -> analysis[j] -> idcc[k].a = i;
              }
            }
          }
        }
      }
      if (this_proj -> next != NULL) this_proj = this_proj -> next;
    }
    this_proj = workzone.first;
    for (i=0 ; i<nprojects ; i++)
    {
      this_proj -> id = i;
      if (this_proj -> analysis)
      {
        for (j=0; j<NCALCS; j++)
        {
          if (this_proj -> analysis[j])
          {
            for (k=0; k<this_proj -> analysis[j] -> numc; k++)
            {
              if (this_proj -> analysis[j] -> curves[k] -> window)
              {
                curve_window_add_menu_bar (&  this_proj -> analysis[j] -> idcc[k]);
                if (is_the_widget_visible(this_proj -> analysis[j] -> curves[k] -> plot))
                {
                  gtk_widget_queue_draw (this_proj -> analysis[j] -> curves[k] -> plot);
                }
              }
            }
          }
        }
      }
      if (this_proj -> next != NULL) this_proj = this_proj -> next;
    }
  }
  g_debug ("so clos 5");
  if (! atomes_render_image) update_insert_combos ();
}

/*!
  \fn void to_close_this_project (int to_activate, project * this_proj)

  \brief to close this project

  \param to_activate If the workspace is not empty, activate first another project
  \param this_proj the target project
*/
void to_close_this_project (int to_activate, project * this_proj)
{
  if (nprojects > 0) close_project (this_proj);
  if (nprojects > 0)
  {
    activate_project (NULL, GINT_TO_POINTER(to_activate));
  }
  else if (! atomes_render_image || atomes_from_libreoffice)
  {
    remove_edition_and_analyze_actions ();
    active_project = NULL;
    fill_tool_model ();
  }
}

/*!
  \fn G_MODULE_EXPORT void on_close_activate (GtkWidget * widg, gpointer cdata)

  \brief signal to close a project

  \param widg the GtkWidget sending the signal
  \param cdata the associated data pointer
*/
G_MODULE_EXPORT void on_close_activate (GtkWidget * widg, gpointer cdata)
{
  if (nprojects > 0)
  {
    int i = GPOINTER_TO_INT(cdata);
    int j = activep;
    if (j >= i && j > 0) j--;
    gtk_tree_store_clear (tool_model);
    remove_project_from_workspace (i);
    close_project (get_project_by_id(i));
    if (nprojects > 0)
    {
      activate_project (NULL, GINT_TO_POINTER(j));
    }
    else
    {
      remove_edition_and_analyze_actions ();
      active_project = NULL;
      fill_tool_model ();
    }
  }
  else
  {
    show_warning ("No project to be closed", MainWindow);
  }
}
