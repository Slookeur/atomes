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
* @file sqcall.c
* @short Callbacks for the s(q) / s(k) calculation dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'sqcall.c'
*
* Contains:
*

 - The callbacks for the s(q) / s(k) calculation dialog

*
* List of functions:

  void initsq (int r);
  void update_sq_view (project * this_proj, int sqk);
  void save_xsk_ (int * interv, double datacurve[* interv]);

  G_MODULE_EXPORT void on_calc_sq_released (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void on_calc_sk_released (GtkWidget * widg, gpointer data);

*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"

/*!
  \fn void initsq (int r)

  \brief initialize the curve widgets for the s(q) / s(k) calculation

  \param r s(q) (SQ) or s(k) (SK)
*/
void initsq (int r)
{
  int i, j, k;

  active_project -> analysis[r] -> curves[0] -> name = g_strdup_printf ("S(q) Neutrons");
  active_project -> analysis[r] -> curves[1] -> name = g_strdup_printf ("S(q) Neutrons - smoothed");
  active_project -> analysis[r] -> curves[2] -> name = g_strdup_printf ("Q(q) Neutrons");
  active_project -> analysis[r] -> curves[3] -> name = g_strdup_printf ("Q(q) Neutrons - smoothed");
  active_project -> analysis[r] -> curves[4] -> name = g_strdup_printf ("S(q) X-rays");
  active_project -> analysis[r] -> curves[5] -> name = g_strdup_printf ("S(q) X-rays - smoothed");
  active_project -> analysis[r] -> curves[6] -> name = g_strdup_printf ("Q(q) X-rays");
  active_project -> analysis[r] -> curves[7] -> name = g_strdup_printf ("Q(q) X-rays - smoothed");
  k = 8;
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    for ( j = 0 ; j < active_project -> nspec ; j++ )
    {
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("AL(q)[%s,%s]", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("AL(q)[%s,%s] - smoothed", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
    }
  }
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    for ( j = 0 ; j < active_project -> nspec ; j++ )
    {
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("FZ(q)[%s,%s]", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("FZ(q)[%s,%s] - smoothed", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
    }
  }
  if ( active_project -> nspec == 2 )
  {
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("BT(q)[NN]");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("BT(q)[NN] - smoothed");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("BT(q)[NC]");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("BT(q)[NC] - smoothed");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("BT(q)[CC]");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("BT(q)[CC] - smoothed");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("BT(q)[ZZ]");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("BT(q)[ZZ] - smoothed");
  }
  add_curve_widgets (activep, r, 0);
  active_project -> analysis[r] -> init_ok = TRUE;
}

/*!
  \fn void update_sq_view (project * this_proj, int sqk)

  \brief update the text view for s(q) / s(k) calculation

  \param this_proj the target project
  \param sqk s(q) (SQ) or s(k) (SK)
*/
void update_sq_view (project * this_proj, int sqk)
{
  gchar * str;
  if (this_proj -> analysis[sqk] -> calc_buffer == NULL) this_proj -> analysis[sqk] -> calc_buffer = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("\n\nStructure factor(s)", "heading", this_proj -> analysis[sqk] -> calc_buffer);
  if (sqk == SKD)
  {
    print_info (" - reciprocal space calculation\n\n", "heading", this_proj -> analysis[sqk] -> calc_buffer);
  }
  else
  {
    print_info (" - FFT[g(r)]\n\n", "heading", this_proj -> analysis[sqk] -> calc_buffer);
  }
  print_info ("Calculation details:\n\n", NULL, this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("\tReciprocal space discretization:\n\n", NULL, this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("\t - Number of δq steps: ", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  str = g_strdup_printf ("%d", this_proj -> analysis[sqk] -> num_delta);
  print_info (str, "bold_blue", this_proj -> analysis[sqk] -> calc_buffer);
  g_free (str);
  print_info ("\n\n\t between ", NULL, this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("Q", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("min", "sub_bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info (" and ", NULL, this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("Q", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("max", "sub_bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("\n\t where ", NULL, this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("Q", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("min", "sub_bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info (" is the minimum wave vector, and ", NULL, this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("Q", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("max", "sub_bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info (" is the maximum wave vector:\n\n", NULL, this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("\t\tQ", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("min", "sub_bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info (" = ", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  str = g_strdup_printf ("%f", this_proj -> analysis[sqk] -> min);
  print_info (str, "bold_blue", this_proj -> analysis[sqk] -> calc_buffer);
  g_free (str);
  print_info (" Å", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("-1", "sup_bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("\tand\t", NULL, this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("Q", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("max", "sub_bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info (" = ", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  str = g_strdup_printf ("%f", this_proj -> analysis[sqk] -> max);
  print_info (str, "bold_blue", this_proj -> analysis[sqk] -> calc_buffer);
  g_free (str);
  print_info (" Å", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("-1", "sup_bold", this_proj -> analysis[sqk] -> calc_buffer);

  print_info ("\n\n\t - δq = ", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  str = g_strdup_printf ("%f", this_proj -> analysis[sqk] -> delta);
  print_info (str, "bold_blue", this_proj -> analysis[sqk] -> calc_buffer);
  g_free (str);
  print_info (" Å", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("-1", "sup_bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info ("\n", "bold", this_proj -> analysis[sqk] -> calc_buffer);
  print_info (calculation_time(TRUE, this_proj -> analysis[sqk] -> calc_time), NULL, this_proj -> analysis[sqk] -> calc_buffer);
}

/*!
  \fn G_MODULE_EXPORT void on_calc_sq_released (GtkWidget * widg, gpointer data)

  \brief compute s(q) / s(k)

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_sq_released (GtkWidget * widg, gpointer data)
{
  int i;
  if (! active_project -> analysis[SQD] -> init_ok) initsq (SQD);
  clean_curves_data (SQD, 0, active_project -> analysis[SQD] -> numc);
  active_project -> analysis[SQD] -> delta = (active_project -> analysis[SQD] -> max - active_project -> analysis[SQD] -> min) / active_project -> analysis[SQD] -> num_delta;
  prepostcalc (widg, FALSE, SQD, 0, opac);
  i = s_of_q_ (& active_project -> analysis[SQD] -> max,
               & active_project -> analysis[SQD] -> min,
               & active_project -> analysis[SQD] -> num_delta);
  prepostcalc (widg, TRUE, SQD, i, 1.0);
  if (! i)
  {
    show_error ("The S(q) calculation has failed", 0, widg);
  }
  else
  {
    update_sq_view (active_project, SQD);
    show_the_widgets (curvetoolbox);
  }
  fill_tool_model ();
  for (i=1; i<3; i++) update_after_calc (i);
}

/*!
  \fn void save_xsk_ (int * interv, double datacurve[*interv])

  \brief get s(k) calculation results form Fortran90

  \param interv number of data point (delta r/q)
  \param datacurve calculation result
*/
void save_xsk_ (int * interv, double datacurve[* interv])
{
  if (* interv != 0)
  {
    xsk = duplicate_double (* interv, datacurve);
  }
}

/*!
  \fn G_MODULE_EXPORT void on_calc_sk_released (GtkWidget * widg, gpointer data)

  \brief compute s(q) / s(k)

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_sk_released (GtkWidget * widg, gpointer data)
{
  int i, j;

  if (! active_project -> analysis[SKD] -> init_ok) initsq (SKD);
  clean_curves_data (SKD, 0, active_project -> analysis[SKD] -> numc);
  active_project -> analysis[SKD] -> delta = (active_project -> analysis[SKD] -> max - active_project -> analysis[SKD] -> min) / active_project -> analysis[SKD] -> num_delta;
  prepostcalc (widg, FALSE, SKD, 0, opac);
  i = cqvf_ (& active_project -> analysis[SKD] -> max,
             & active_project -> analysis[SKD] -> min,
             & active_project -> analysis[SKD] -> num_delta,
             & active_project -> sk_advanced[0],
             & active_project -> sk_advanced[1]);
  if (i == 1)
  {
    for (i=0; i<active_project -> analysis[SKD] -> numc; i++)
    {
      active_project -> analysis[SKD] -> curves[i] -> ndata = 0;
    }
    j = s_of_k_ (& active_project -> analysis[SKD] -> num_delta, & active_project -> xcor);
    g_free (xsk);
    xsk = NULL;
    prepostcalc (widg, TRUE, SKD, j, 1.0);
    active_project -> analysis[GDK] -> avail_ok = j;

    if (! j)
    {
      remove_action ("analyze.3");
      show_error ("The S(q) calculation has failed", 0, widg);
    }
    else
    {
      add_analysis_action (GDK);
      update_sq_view (active_project, SKD);
      show_the_widgets (curvetoolbox);
    }
  }
  else
  {
    prepostcalc (widg, TRUE, SKD, i, 1.0);
    show_error ("Problem during the selection of the k-points\nused to sample the recipocal lattice", 0, widg);
  }
  fill_tool_model ();
  for (i=1; i<3; i++) update_after_calc (i);
}
