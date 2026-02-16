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
* @file sktcall.c
* @short Callbacks for the dynamic structure factor S(k,t) calculation dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'sktcall.c'
*
* Contains:
*

 - The callbacks for the dynamic structure factor calculation dialog

*
* List of functions:

  void init_skt ();
  void update_skt_view (project * this_proj);

  G_MODULE_EXPORT void on_calc_skt_released (GtkWidget * widg, gpointer data);

*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "curve.h"
#include "project.h"

extern void alloc_analysis_curves (int pid, atomes_analysis * this_analysis);
extern void update_sq_view (project * this_proj, int sqk);
extern gboolean skt_all_sets;

/*!
  \fn void init_skt (project * this_proj)

  \brief initialize the curve widgets for the s(k,t) and s(q,w) calculations

  \param this_proj the target project
*/
void init_skt (project * this_proj)
{
  int g, h, i, j, k, l, m;
  g = this_proj -> analysis[SKD] -> numc;
  this_proj -> analysis[SKT] -> numc = ((this_proj -> skt_all_sets) ? this_proj -> steps - this_proj -> skt_correlations : this_proj -> skt_n_data_sets) * g;
  alloc_analysis_curves (this_proj -> id, this_proj -> analysis[SKT]);
  for (h=0; h<  ((this_proj -> skt_all_sets) ? this_proj -> steps - this_proj -> skt_correlations : this_proj -> skt_n_data_sets); h++)
  {
    i = h*g;
    j = (this_proj -> skt_all_sets) ? h+1 : active_project -> skt_step_id[h];
    this_proj -> analysis[SKT] -> curves[0+i] -> name = g_strdup_printf ("S(q,t) Neutrons - t= %d", j);
    this_proj -> analysis[SKT] -> curves[1+i] -> name = g_strdup_printf ("S(q,t) Neutrons - t= %d - smoothed", j);
    this_proj -> analysis[SKT] -> curves[2+i] -> name = g_strdup_printf ("Q(q,t) Neutrons - t= %d", j);
    this_proj -> analysis[SKT] -> curves[3+i] -> name = g_strdup_printf ("Q(q,t) Neutrons - t= %d - smoothed", j);
    this_proj -> analysis[SKT] -> curves[4+i] -> name = g_strdup_printf ("S(q,t) X-rays - t= %d", j);
    this_proj -> analysis[SKT] -> curves[5+i] -> name = g_strdup_printf ("S(q,t) X-rays - t= %d - smoothed", j);
    this_proj -> analysis[SKT] -> curves[6+i] -> name = g_strdup_printf ("Q(q,t) X-rays - t= %d", j);
    this_proj -> analysis[SKT] -> curves[7+i] -> name = g_strdup_printf ("Q(q,t) X-rays - t= %d - smoothed", j);

    k = 8+i;
    for ( l = 0 ; l < this_proj -> nspec ; l++ )
    {
      for ( m = 0 ; m < this_proj -> nspec ; m++ )
      {
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("AL(q,t)[%s,%s] - t= %d", active_chem -> label[l], active_chem -> label[m], j);
        k ++;
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("AL(q,t)[%s,%s] - t= %d - smoothed", active_chem -> label[l], active_chem -> label[m], j);
        k ++;
      }
    }
    for ( l = 0 ; l < this_proj -> nspec ; l++ )
    {
      for ( m = 0 ; m < this_proj -> nspec ; m++ )
      {
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("FZ(q,t)[%s,%s] - t= %d", active_chem -> label[l], active_chem -> label[m], j);
        k ++;
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("FZ(q,t)[%s,%s] - t= %d - smoothed", active_chem -> label[l], active_chem -> label[m], j);
        k ++;
      }
    }
    if ( this_proj -> nspec == 2 )
    {
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,t)[NN] - t= %d", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,t)[NN] - t= %d - smoothed", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,t)[NC] - t= %d", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,t)[NC] - t= %d - smoothed", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,t)[CC] - t= %d", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,t)[CC] - t= %d - smoothed", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,t)[ZZ] - t= %d", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,t)[ZZ] - t= %d - smoothed", j);
    }
  }
  add_curve_widgets (this_proj, SKT);
  this_proj -> analysis[SKT] -> init_ok = TRUE;
}

/*!
  \fn G_MODULE_EXPORT void on_calc_skt_released (GtkWidget * widg, gpointer data)

  \brief callback to compute the dynamic structure factor analysis

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_skt_released (GtkWidget * widg, gpointer data)
{
  // Initializing the graph for this calculation, if not done already
  init_skt (active_project);

  // Cleaning previous results, if any
  clean_curves_data (SKT, 0, active_project -> analysis[SKT] -> numc);
  active_project -> analysis[SKT] -> delta = (active_project -> analysis[SKT] -> max - active_project -> analysis[SKT] -> min) / active_project -> analysis[SKT] -> num_delta;

  // Calculation time for dynamic structure factor analysis starts here !
  prepostcalc (widg, FALSE, SKT, 0, opac);
  int i;
  i = cqvf_ (& active_project -> analysis[SKT] -> max,
             & active_project -> analysis[SKT] -> min,
             & active_project -> analysis[SKT] -> num_delta,
             & active_project -> sk_advanced[1][0],
             & active_project -> sk_advanced[1][1]);
  if (i == 1)
  {
    for (i=0; i<active_project -> analysis[SKT] -> numc; i++)
    {
      active_project -> analysis[SKT] -> curves[i] -> ndata = 0;
    }
    int res_skt = s_of_k_t_ (& active_project -> analysis[SKT] -> num_delta,
                             & active_project -> xcor,
                             & active_project -> skt_correlations,
                             & active_project -> skt_n_data_sets,
                             active_project -> skt_step_id);
    g_free (xsk);
    xsk = NULL;
    prepostcalc (widg, TRUE, SKT, res_skt, 1.0);
    if (! res_skt)
    {
      show_error ("The dynamic structure factor calculation has failed", 0, widg);
    }
    else
    {
      update_sq_view (active_project, SKT);
      show_the_widgets (curvetoolbox);
    }
  }
  else
  {
    prepostcalc (widg, TRUE, SKT, i, 1.0);
    show_error ("Problem during the selection of the k-points\nused to sample the reciprocal lattice", 0, widg);
  }
  fill_tool_model ();
}
