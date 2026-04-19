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

  void init_sqw (project * this_proj);
  void recup_sqw_list_ (int nq, double qval[nq]);
  void init_skt (project * this_proj, int opening);
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
extern void update_dynamic_view (project * this_proj, GtkTextBuffer * calc_buffer);
extern gboolean skt_all_sets;

/*!
  \fn void init_sqw (project * this_proj)

  \brief initialize the curve widgets for the s(k,t) and s(q,w) calculations

  \param this_proj the target project
*/
void init_sqw (project * this_proj)
{
  int g, h, i, k, l, m;
  g = 8+4*this_proj -> nspec*this_proj -> nspec;
  if (this_proj -> nspec == 2) g += 8;
  for (h=0; h < this_proj -> sqw_n_data_sets; h++)
  {
    i = this_proj -> skt_sets + h*g;
    this_proj -> analysis[SKT] -> curves[0+i] -> name = g_strdup_printf ("S(q,ω) %s - q= %f", "Neutrons", this_proj -> sqw_q_id[h]);
    this_proj -> analysis[SKT] -> curves[1+i] -> name = g_strdup_printf ("S(q,ω) %s - q= %f - %s", "Neutrons",  this_proj -> sqw_q_id[h], "smoothed");
    this_proj -> analysis[SKT] -> curves[2+i] -> name = g_strdup_printf ("Q(q,ω) %s - q= %f", "Neutrons", this_proj -> sqw_q_id[h]);
    this_proj -> analysis[SKT] -> curves[3+i] -> name = g_strdup_printf ("Q(q,ω) %s - q= %f - %s", "Neutrons", this_proj -> sqw_q_id[h], "smoothed");
    this_proj -> analysis[SKT] -> curves[4+i] -> name = g_strdup_printf ("S(q,ω) %s - q= %f", "X-rays", this_proj -> sqw_q_id[h]);
    this_proj -> analysis[SKT] -> curves[5+i] -> name = g_strdup_printf ("S(q,ω) %s - q= %f - %s", "X-rays", this_proj -> sqw_q_id[h], "smoothed");
    this_proj -> analysis[SKT] -> curves[6+i] -> name = g_strdup_printf ("Q(q,ω) %s - q= %f", "X-rays", this_proj -> sqw_q_id[h]);
    this_proj -> analysis[SKT] -> curves[7+i] -> name = g_strdup_printf ("Q(q,ω) %s - q= %f - %s", "X-rays", this_proj -> sqw_q_id[h], "smoothed");
    k = i + 8;
    for ( l = 0 ; l < this_proj -> nspec ; l++ )
    {
      for ( m = 0 ; m < this_proj -> nspec ; m++ )
      {
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("AL(q,ω)[%s,%s] - q= %f", active_chem -> label[l], active_chem -> label[m], this_proj -> sqw_q_id[h]);
        k ++;
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("AL(q,ω)[%s,%s] - q= %f - %s", active_chem -> label[l], active_chem -> label[m], this_proj -> sqw_q_id[h], "smoothed");
        k ++;
      }
    }
    for ( l = 0 ; l < this_proj -> nspec ; l++ )
    {
      for ( m = 0 ; m < this_proj -> nspec ; m++ )
      {
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("FZ(q,ω)[%s,%s] - q= %f", active_chem -> label[l], active_chem -> label[m], this_proj -> sqw_q_id[h]);
        k ++;
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("FZ(q,ω)[%s,%s] - q= %f - %s", active_chem -> label[l], active_chem -> label[m], this_proj -> sqw_q_id[h], "smoothed");
        k ++;
      }
    }
    if ( this_proj -> nspec == 2 )
    {
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ω)[NN] - q= %f", this_proj -> sqw_q_id[h]);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ω)[NN] - q= %f - %s", this_proj -> sqw_q_id[h], "smoothed");
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ω)[NC] - q= %f", this_proj -> sqw_q_id[h]);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ω)[NC] - q= %f - %s", this_proj -> sqw_q_id[h], "smoothed");
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ω)[CC] - q= %f", this_proj -> sqw_q_id[h]);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ω)[CC] - q= %f - %s", this_proj -> sqw_q_id[h], "smoothed");
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ω)[ZZ] - q= %f", this_proj -> sqw_q_id[h]);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ω)[ZZ] - q= %f - %s", this_proj -> sqw_q_id[h], "smoothed");
    }
  }
  for (h=this_proj -> skt_sets; h < this_proj -> analysis[SKT] -> numc; h++)
  {
    init_curve_title (this_proj, SKT, h);
  }
}

/*!
  \fn void recup_sqw_list_ (int nq, double qval[nq])

  \brief retrieve exact q vector values from Fortran

  \param nq the number of q vectors
  \param qval the list of q vectors
*/
void recup_sqw_list_ (int nq, double qval[nq])
{
  if (nq != active_project -> sqw_n_data_sets)
  {
    // This should happen, error ?
  }
  int i;
  for (i=0; i<* qval; i++)
  {
    active_project -> sqw_q_id[i] = qval[i];
  }
  init_sqw (active_project);
  // Update calculation dialog ?
}

/*!
  \fn void init_skt (project * this_proj, int opening)

  \brief initialize the curve widgets for the s(k,t) and s(q,w) calculations

  \param this_proj the target project
  \param opening opening project file 1, otherwise 0
*/
void init_skt (project * this_proj, int opening)
{
  int g, h, i, j, k, l, m;
  if (this_proj -> analysis[SKT] -> curves)
  {
    for (i=0; i<this_proj -> analysis[SKT] -> numc; i++)
    {
      hide_the_widgets(this_proj -> analysis[SKT] -> curves[i] -> window);
    }
  }
  this_proj -> skt_sets = 8+4*this_proj -> nspec*this_proj -> nspec;
  if (this_proj -> nspec == 2) this_proj -> skt_sets += 8;
  this_proj -> sqw_sets = this_proj -> skt_sets;
  g = this_proj -> skt_sets;
  if (this_proj -> skt_all_sets)
  {
    this_proj -> skt_sets *= (this_proj -> steps - this_proj -> skt_corr_threshold);
  }
  else
  {
    this_proj -> skt_sets *= this_proj -> skt_n_data_sets;
  }
  this_proj -> sqw_sets *= this_proj -> sqw_n_data_sets;
  this_proj -> analysis[SKT] -> numc = this_proj -> skt_sets + this_proj -> sqw_sets;
  alloc_analysis_curves (this_proj -> id, this_proj -> analysis[SKT]);
  for (h=0; h < ((this_proj -> skt_all_sets) ? this_proj -> steps - this_proj -> skt_corr_threshold : this_proj -> skt_n_data_sets); h++)
  {
    i = h*g;
    j = (this_proj -> skt_all_sets) ? h+1 : active_project -> skt_step_id[h];
    this_proj -> analysis[SKT] -> curves[0+i] -> name = g_strdup_printf ("S(q,ẟt) %s - ẟt= %d", "Neutrons", j);
    this_proj -> analysis[SKT] -> curves[1+i] -> name = g_strdup_printf ("S(q,ẟt) %s - ẟt= %d - %s", "Neutrons", j, "smoothed");
    this_proj -> analysis[SKT] -> curves[2+i] -> name = g_strdup_printf ("Q(q,ẟt) %s - ẟt= %d", "Neutrons", j);
    this_proj -> analysis[SKT] -> curves[3+i] -> name = g_strdup_printf ("Q(q,ẟt) %s - ẟt= %d - %s", "Neutrons", j, "smoothed");
    this_proj -> analysis[SKT] -> curves[4+i] -> name = g_strdup_printf ("S(q,ẟt) %s - ẟt= %d", "X-rays", j);
    this_proj -> analysis[SKT] -> curves[5+i] -> name = g_strdup_printf ("S(q,ẟt) %s - ẟt= %d - %s", "X-rays", j, "smoothed");
    this_proj -> analysis[SKT] -> curves[6+i] -> name = g_strdup_printf ("Q(q,ẟt) %s - ẟt= %d", "X-rays", j);
    this_proj -> analysis[SKT] -> curves[7+i] -> name = g_strdup_printf ("Q(q,ẟt) %s - ẟt= %d - %s", "X-rays", j, "smoothed");

    k = 8+i;
    for ( l = 0 ; l < this_proj -> nspec ; l++ )
    {
      for ( m = 0 ; m < this_proj -> nspec ; m++ )
      {
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("AL(q,ẟt)[%s,%s] - ẟt= %d", active_chem -> label[l], active_chem -> label[m], j);
        k ++;
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("AL(q,ẟt)[%s,%s] - ẟt= %d - %s", active_chem -> label[l], active_chem -> label[m], j, "smoothed");
        k ++;
      }
    }
    for ( l = 0 ; l < this_proj -> nspec ; l++ )
    {
      for ( m = 0 ; m < this_proj -> nspec ; m++ )
      {
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("FZ(q,ẟt)[%s,%s] - ẟt= %d", active_chem -> label[l], active_chem -> label[m], j);
        k ++;
        this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("FZ(q,ẟt)[%s,%s] - ẟt= %d - %s", active_chem -> label[l], active_chem -> label[m], j, "smoothed");
        k ++;
      }
    }
    if ( this_proj -> nspec == 2 )
    {
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ẟt)[NN] - ẟt= %d", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ẟt)[NN] - ẟt= %d - %s", j, "smoothed");
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ẟt)[NC] - ẟt= %d", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ẟt)[NC] - ẟt= %d - %s", j,"smoothed");
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ẟt)[CC] - ẟt= %d", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ẟt)[CC] - ẟt= %d - %s", j, "smoothed");
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ẟt)[ZZ] - ẟt= %d", j);
      k ++;
      this_proj -> analysis[SKT] -> curves[k] -> name = g_strdup_printf ("BT(q,ẟt)[ZZ] - t= %d - %s", j, "smoothed");
    }
  }
  if (opening) init_sqw (this_proj);
  add_curve_widgets (this_proj, SKT);
  this_proj -> analysis[SKT] -> init_ok = TRUE;
}

/*!
  \fn void update_skt_view (project * this_proj)

  \brief update the text view for s(k,t) and s(q,w) calculation

  \param this_proj the target project
*/
void update_skt_view (project * this_proj)
{
  int i;
  gchar * str;
  update_sq_view (this_proj, SKT);
  print_info ("\n", NULL, this_proj -> analysis[SKT] -> calc_buffer);
  print_info ("\tMolecular dynamics information:\n\n", NULL, this_proj -> analysis[SKT] -> calc_buffer);
  update_dynamic_view (this_proj, this_proj -> analysis[SKT] -> calc_buffer);

  print_info ("\n\n\tIntermediate scattering - F(q,δt)\n\n", NULL, this_proj -> analysis[SKT] -> calc_buffer);

  print_info ("\t - Correlation threshold δt", "bold", this_proj -> analysis[SKT] -> calc_buffer);
  print_info ("min", "sup_bold", this_proj -> analysis[SKT] -> calc_buffer);
  print_info (" = ", "bold", this_proj -> analysis[SKT] -> calc_buffer);
  str = g_strdup_printf ("%d", this_proj -> skt_corr_threshold);
  print_info (str, "bold_blue", this_proj -> analysis[SKT] -> calc_buffer);
  g_free (str);
  print_info ("\n", NULL, this_proj -> analysis[SKT] -> calc_buffer);
  if (this_proj -> skt_all_sets)
  {
    print_info ("\t - All ", "bold", this_proj -> analysis[SKT] -> calc_buffer);
    str = g_strdup_printf ("%d", this_proj -> skt_sets);
    print_info (str, "bold_blue", this_proj -> analysis[SKT] -> calc_buffer);
    g_free (str);
    print_info (" correlated calculations saved", "bold", this_proj -> analysis[SKT] -> calc_buffer);
    print_info ("\n", NULL, this_proj -> analysis[SKT] -> calc_buffer);
  }
  else
  {
    print_info ("\t - Results saved for ", "bold", this_proj -> analysis[SKT] -> calc_buffer);
    str = g_strdup_printf ("%d", this_proj -> skt_n_data_sets);
    print_info (str, "bold_blue", this_proj -> analysis[SKT] -> calc_buffer);
    g_free (str);
    print_info (" correlated calculations:\n\n", "bold", this_proj -> analysis[SKT] -> calc_buffer);
    for (i=0; i<this_proj -> skt_n_data_sets; i++)
    {
      print_info (" \t\t ", NULL, this_proj -> analysis[SKT] -> calc_buffer);
      str = g_strdup_printf ("%d", i);
      print_info (str, NULL, this_proj -> analysis[SKT] -> calc_buffer);
      g_free (str);
      print_info (") δt\t=\t", NULL, this_proj -> analysis[SKT] -> calc_buffer);
      str = g_strdup_printf ("%d\n", this_proj -> skt_step_id[i]);
      print_info (str, "bold_green", this_proj -> analysis[SKT] -> calc_buffer);
      g_free (str);
    }
  }
  print_info ("\n", NULL, this_proj -> analysis[SKT] -> calc_buffer);
  print_info ("\tDynamic structure factor - S(q,ω)\n\n", NULL, this_proj -> analysis[SKT] -> calc_buffer);
  print_info ("\t - ", "bold", this_proj -> analysis[SKT] -> calc_buffer);
  str = g_strdup_printf ("%d", this_proj -> sqw_n_data_sets);
  print_info (str, "bold_blue", this_proj -> analysis[SKT] -> calc_buffer);
  g_free (str);
  print_info (" q vectors were analyzed:\n\n", "bold", this_proj -> analysis[SKT] -> calc_buffer);
  for (i=0; i<this_proj -> sqw_n_data_sets; i++)
  {
    print_info (" \t\t ", NULL, this_proj -> analysis[SKT] -> calc_buffer);
    str = g_strdup_printf ("%d", i);
    print_info (str, NULL, this_proj -> analysis[SKT] -> calc_buffer);
    g_free (str);
    print_info (") q\t=\t", NULL, this_proj -> analysis[SKT] -> calc_buffer);
    str = g_strdup_printf ("%f", this_proj -> sqw_q_id[i]);
    print_info (str, "bold_red", this_proj -> analysis[SKT] -> calc_buffer);
    g_free (str);
    print_info (" Å", "bold", this_proj -> analysis[SKT] -> calc_buffer);
    print_info ("-1", "sup_bold", this_proj -> analysis[SKT] -> calc_buffer);
    print_info ("\n", "bold", this_proj -> analysis[SKT] -> calc_buffer);
  }
  print_info (calculation_time(TRUE, this_proj -> analysis[SKT] -> calc_time), NULL, this_proj -> analysis[SKT] -> calc_buffer);
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
  init_skt (active_project, 0);

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
    double detla_t = active_project -> analysis[MSD] -> delta*active_project -> analysis[MSD] -> num_delta;
    int res_skt = s_of_k_t_ (& active_project -> analysis[SKT] -> num_delta,
                             & active_project -> xcor,
                             & active_project -> skt_corr_threshold,
                             & active_project -> skt_n_data_sets,
                             active_project -> skt_step_id,
                             & detla_t,
                             & active_project -> sqw_n_data_sets,
                             active_project -> sqw_q_id,
                             & active_project -> sqw_freq);
    g_free (xsk);
    xsk = NULL;
    prepostcalc (widg, TRUE, SKT, res_skt, 1.0);
    if (! res_skt)
    {
      show_error ("The dynamic structure factor calculation has failed", 0, widg);
    }
    else
    {
      update_skt_view (active_project);
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
