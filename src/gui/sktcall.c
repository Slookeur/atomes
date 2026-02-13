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

extern void init_sq (project * this_proj, int sqk);
extern void update_sq_view (project * this_proj, int sqk);

/*!
  \fn G_MODULE_EXPORT void on_calc_skt_released (GtkWidget * widg, gpointer data)

  \brief callback to compute the dynamic structure factor analysis

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_skt_released (GtkWidget * widg, gpointer data)
{
  // Initializing the graph for this calculation, if not done already
  if (! active_project -> analysis[SKT] -> init_ok)  init_sq (active_project, SKT);

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
    int res_skt = s_of_k_t_ (& active_project -> analysis[SKT] -> num_delta, & active_project -> xcor, & active_project -> skt_correlations);
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
