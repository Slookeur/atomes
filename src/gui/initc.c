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
* @file initc.c
* @short Curve data buffer initialization
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'initc.c'
*
* Contains:
*

 - Curve data buffer initialization

*
* List of functions:

  void clean_curves_data (int calc, int start, int end);
  void alloc_curves (int rid);
  void initcwidgets ();
  void prepostcalc (GtkWidget * widg, gboolean status, int run, int adv, double opc);
  void alloc_analysis_curves (atomes_analysis * this_analysis);
  void init_atomes_analysis (gboolean apply_defaults);
  void initialize_this_analysis (project * this_proj, int ana);

  atomes_analysis * setup_analysis (gchar * name, int analysis, gboolean req_md, gboolean graph, int num_curves, int n_compat, int * compat, gchar * x_title);

*/

#include "global.h"
#include "callbacks.h"
#include "project.h"

extern void clean_this_curve_window (int cid, int rid);
extern void apply_analysis_default_parameters_to_project (project * this_proj);

/*!
  \fn void clean_curves_data (int calc, int start, int end)

  \brief clean curve data on a range of curve id

  \param calc the calculation
  \param start the starting value
  \param end the ending value
*/
void clean_curves_data (int calc, int start, int end)
{
  int i;
  if (active_project -> analysis[calc])
  {
    if (active_project -> analysis[calc] -> curves)
    {
      for (i=start; i<end; i++)
      {
        clean_this_curve_window (i, calc);
      }
    }
  }
}

/*!
  \fn void prepostcalc (GtkWidget * widg, gboolean status, int run, int adv, double opc)

  \brief to just before and just after running a calculation

  \param widg the GtkWidget sending the signal
  \param status calculation completed (1/0)
  \param run calculation id
  \param adv calculation result
  \param opc opacity
*/
void prepostcalc (GtkWidget * widg, gboolean status, int run, int adv, double opc)
{
  if (run < NGRAPHS && run > -1) active_project -> analysis[run] -> calc_ok = adv;
  if (! status)
  {
    clock_gettime (CLOCK_MONOTONIC, & start_time);
#ifdef GTK3
    if (widg != NULL) gdk_window_set_opacity (gtk_widget_get_window(widg), opc);
#endif
/*    if (adv)
    {
      // bar[run]
      mess = g_strdup_printf ("Please wait calculation in progress");
      pop = show_popup (mess, widg);
      g_free (mess);
      mess = g_strdup_printf ("Computing");
      //statusval = gtk_statusbar_push (statusbar, run, mess);
      g_free (mess);
      show_the_widgets (pop);
    }*/
  }
  else
  {
    if (adv && run > -1)
    {
      //gtk_statusbar_remove (statusbar, run, statusval);
      //destroy_this_widget(pop);
    }
    if (run > -1)
    {
      clock_gettime (CLOCK_MONOTONIC, & stop_time);
      active_project -> analysis[run] -> calc_time = get_calc_time (start_time, stop_time);
    }
#ifdef GTK3
    if (widg != NULL) gdk_window_set_opacity (gtk_widget_get_window(widg), opc);
#endif
  }
}

/*!
  \fn void alloc_analysis_curves (atomes_analysis * this_analysis)

  \brief allocating analysis curve data

  \param this_analysis the target atomes_analysis pointer
*/
void alloc_analysis_curves (atomes_analysis * this_analysis)
{
  int i;
  if (this_analysis -> idcc != NULL)
  {
    g_free (this_analysis -> idcc);
    this_analysis -> idcc = NULL;
  }
  this_analysis -> idcc = g_malloc0 (this_analysis -> numc*sizeof*this_analysis -> idcc);
  if (this_analysis -> curves != NULL)
  {
    g_free (this_analysis -> curves);
    this_analysis -> curves = NULL;
  }
  this_analysis -> curves = g_malloc (this_analysis -> numc*sizeof*this_analysis -> curves);
  for (i = 0; i < this_analysis -> numc; i++)
  {
    this_analysis -> curves[i] = g_malloc0 (sizeof*this_analysis -> curves[i]);
  }
}

/*!
  \fn atomes_analysis * setup_analysis (gchar * name, int analysis, gboolean req_md, gboolean graph, int num_curves, int n_compat, int * compat, gchar * x_title)

  \brief allocate atomes_analysis data structure

  \param name analysis name
  \param analysis analysis ID
  \param req_md requires MD trajectory (1/0)
  \param graph curves as output or not (1/0)
  \param num_curves number of curves to be produced for this analysis
  \param n_compat number of compatible analysis
  \param compat list of compatible analysis
  \param x_title default title for x axis for graphs
*/
atomes_analysis * setup_analysis (gchar * name, int analysis, gboolean req_md, gboolean graph, int num_curves, int n_compat, int * compat, gchar * x_title)
{
  atomes_analysis * new_analysis = g_malloc0(sizeof*new_analysis);
  new_analysis -> name = g_strdup_printf ("%s", name);
  new_analysis -> aid = analysis;
  new_analysis -> requires_md = req_md;
  new_analysis -> graph_res = graph;
  if (graph)
  {
    new_analysis -> c_sets = n_compat;
    new_analysis -> compat_id = duplicate_int (n_compat, compat);
    if (num_curves)
    {
      new_analysis -> numc = num_curves;
      alloc_analysis_curves (new_analysis);
    }
    if (x_title) new_analysis -> x_title = g_strdup_printf ("%s", x_title);
  }
  return new_analysis;
}

/*
  From global.h:

  #define NCALCS 10
  #define NGRAPHS 10

  #define GDR 0
  #define SQD 1
  #define SKD 2
  #define GDK 3
  #define BND 4
  #define ANG 5
  #define RIN 6
  #define CHA 7
  #define SPH 8
  #define MSD 9
*/

/*!
  \fn void init_atomes_analysis (gboolean apply_defaults)

  \brief initialize analysis data structures for atomes

  \param apply_defaults apply default parameters (1/0)
*/
void init_atomes_analysis (gboolean apply_defaults)
{
  int i = active_project -> nspec;
  /* Compatible analysis:
    - always include self, and others if required
    - x axis must be similar or allow comparison (ex: distance)
  */
  int * comp_list;
  active_project -> analysis = g_malloc0(NCALCS*sizeof*active_project -> analysis);
  // g(r)
  comp_list = allocint (2);
  comp_list[0] = GDR;
  comp_list[1] = GDK;
  active_project -> analysis[GDR] = setup_analysis ("g(r)/G(r)", GDR, FALSE, TRUE, 16+5*i*i + ((i ==2) ? 6 : 0), 2, comp_list, "r [Å]");
  // g(r) FFT  - same compatibility list
  active_project -> analysis[GDK] = setup_analysis ("g(r)/G(r) from FFT[S(q)]", GDK, FALSE, TRUE, 16+5*i*i + ((i ==2) ? 6 : 0), 2, comp_list, "r [Å]");

  // s(q)
  comp_list[0] = SQD;
  comp_list[1] = SKD;
  active_project -> analysis[SQD] = setup_analysis ("S(q) from FFT[g(r)]", SQD, FALSE, TRUE, 8+4*i*i + ((i ==2) ? 8 : 0), 2, comp_list, "q [Å-1]");
  // s(k) - same compatibility list
  active_project -> analysis[SKD] = setup_analysis ("S(q) from Debye equation", SKD, FALSE, TRUE, 8+4*i*i + ((i ==2) ? 8 : 0), 2, comp_list, "q [Å-1]");

  g_free (comp_list);

  comp_list = allocint (1);
  // Bond length  distribution(s)
  comp_list[0] = BND;
  active_project -> analysis[BND] = setup_analysis ("Bonds properties", BND, FALSE, TRUE, i*i, 1, comp_list, "Dij [Å]");

  // Angle distribution(s)
  comp_list[0] = ANG;
  active_project -> analysis[ANG] = setup_analysis ("Angle distributions", ANG, FALSE, TRUE, i*i*i + i*i*i*i, 1, comp_list, "θ [°]");

  // Ring statistic(s)
  comp_list[0] = RIN;
  active_project -> analysis[RIN] = setup_analysis ("Ring statistics", RIN, FALSE, TRUE, 20*(i+1), 1, comp_list, "Size n of the ring [total number of nodes]");

  // Chain statistic(s)
  comp_list[0] = CHA;
  active_project -> analysis[CHA] = setup_analysis ("Chain statistics", CHA, FALSE, TRUE, i+1, 1, comp_list, "Size n of the chain [total number of nodes]");

  // Spherical harmonic(s)
  comp_list[0] = SPH;
  active_project -> analysis[SPH] = setup_analysis ("Spherical harmonics", SPH, FALSE, TRUE, 0, 1, comp_list, "Ql");

  // Mean square displacement
  comp_list[0] = MSD;
  if (active_project -> steps > 1) active_project -> analysis[MSD] = setup_analysis ("Mean Squared Displacement", MSD, TRUE, TRUE, 14*i+6, 1, comp_list, NULL);

  g_free (comp_list);

  if (apply_defaults) apply_analysis_default_parameters_to_project (active_project);
}

/*!
  \fn void initialize_this_analysis (project * this_proj, int ana)

  \brief initialize an analysis data structure for atomes

  \param this_proj the target project data structure
  \param ana the target analysis
*/
void initialize_this_analysis (project * this_proj, int ana)
{
  int i = this_proj -> nspec;
  int * comp_list;
  if (! this_proj -> analysis)
  {
    this_proj -> analysis = g_malloc0(NCALCS*sizeof*this_proj -> analysis);
  }
  switch (ana)
  {
    case GDR:
      // g(r)
      comp_list = allocint (2);
      comp_list[0] = GDR;
      comp_list[1] = GDK;
      this_proj -> analysis[GDR] = setup_analysis ("g(r)/G(r)", GDR, FALSE, TRUE, 16+5*i*i + ((i ==2) ? 6 : 0), 2, comp_list, "r [Å]");
      break;
    case SQD:
      // S(q)
      comp_list = allocint (2);
      comp_list[0] = SQD;
      comp_list[1] = SKD;
      this_proj -> analysis[SQD] = setup_analysis ("S(q) from FFT[g(r)]", SQD, FALSE, TRUE, 8+4*i*i + ((i ==2) ? 8 : 0), 2, comp_list, "q [Å-1]");
      break;
    case SKD:
      // S(k)
      comp_list = allocint (2);
      comp_list[0] = SQD;
      comp_list[1] = SKD;
      this_proj -> analysis[SKD] = setup_analysis ("S(q) from Debye equation", SKD, FALSE, TRUE, 8+4*i*i + ((i ==2) ? 8 : 0), 2, comp_list, "q [Å-1]");
      break;
    case GDK:
      // g(r) FFT S(q)
      comp_list = allocint (2);
      comp_list[0] = GDR;
      comp_list[1] = GDK;
      this_proj -> analysis[GDK] = setup_analysis ("g(r)/G(r) from FFT[S(q)]", GDK, FALSE, TRUE, 16+5*i*i + ((i ==2) ? 6 : 0), 2, comp_list, "r [Å]");
      break;
    case BND:
      // Bond length  distribution(s)
      comp_list = allocint (1);
      comp_list[0] = BND;
      this_proj -> analysis[BND] = setup_analysis ("Bonds properties", BND, FALSE, TRUE, i*i, 1, comp_list, "Dij [Å]");
      break;
    case ANG:
      // Angle distribution(s)
      comp_list = allocint (1);
      comp_list[0] = ANG;
      this_proj -> analysis[ANG] = setup_analysis ("Angle distributions", ANG, FALSE, TRUE, i*i*i + i*i*i*i, 1, comp_list, "θ [°]");
      break;
    case RIN:
      // Ring statistics
      comp_list = allocint (1);
      comp_list[0] = RIN;
      this_proj -> analysis[RIN] = setup_analysis ("Ring statistics", RIN, FALSE, TRUE, 20*(i+1), 1, comp_list, "Size n of the ring [total number of nodes]");
      break;
    case CHA:
      // Chain statistics
      comp_list = allocint (1);
      comp_list[0] = CHA;
      this_proj -> analysis[CHA] = setup_analysis ("Chain statistics", CHA, FALSE, TRUE, i+1, 1, comp_list, "Size n of the chain [total number of nodes]");
      break;
    case SPH:
      // Spherical harmonics as order parameters
      comp_list = allocint (1);
      comp_list[0] = SPH;
      this_proj -> analysis[SPH] = setup_analysis ("Spherical harmonics", SPH, FALSE, TRUE, 0, 1, comp_list, "Ql");
      break;
    case MSD:
      // Mean square displacement
      comp_list = allocint (1);
      comp_list[0] = MSD;
      if (this_proj -> steps > 1) this_proj -> analysis[MSD] = setup_analysis ("Mean Squared Displacement", MSD, TRUE, TRUE, 14*i+6, 1, comp_list, NULL);
      break;
  }
  g_free (comp_list);
}
