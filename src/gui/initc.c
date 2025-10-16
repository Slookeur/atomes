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

*/

#include "global.h"
#include "callbacks.h"
#include "project.h"

extern void clean_this_curve_window (int cid, int rid);

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
#ifdef NEW_ANA
  for (i=start; i<end; i++)
  {
    if (active_project -> analysis[calc] -> curves)
    {
      clean_this_curve_window (i, calc);
    }
  }
#else
  for (i=start; i<end; i++)
  {
    if (active_project -> curves[calc])
    {
      clean_this_curve_window (i, calc);
    }
  }
#endif // NEW_ANA
}

#ifndef NEW_ANA
/*!
  \fn void alloc_curves (int rid)

  \brief allocating curve data

  \param rid analysis id
*/
void alloc_curves (int rid)
{
  int i;
  if (active_project -> idcc[rid] != NULL)
  {
    g_free (active_project -> idcc[rid]);
    active_project -> idcc[rid] = NULL;
  }
  active_project -> idcc[rid] = g_malloc0 (active_project -> numc[rid]*sizeof*active_project -> idcc[rid]);
  if (active_project -> curves[rid] != NULL)
  {
    g_free (active_project -> curves[rid]);
    active_project -> curves[rid] = NULL;
  }
  active_project -> curves[rid] = g_malloc (active_project -> numc[rid]*sizeof*active_project -> curves);
  for (i = 0; i < active_project -> numc[rid]; i++)
  {
    active_project -> curves[rid][i] = g_malloc0 (sizeof*active_project -> curves[rid][i]);
    active_project -> curves[rid][i] -> cfile = NULL;
    active_project -> curves[rid][i] -> name = NULL;
    active_project -> curves[rid][i] -> axis_title[0] = NULL;
    active_project -> curves[rid][i] -> axis_title[1] = NULL;
  }
}

/*!
  \fn void initcwidgets ()

  \brief initializing curve values
*/
void initcwidgets ()
{
  int i, j;

  j=active_project -> nspec;
  active_project -> numc[GDR] = 16+5*j*j;
  active_project -> numc[SQD] = 8+4*j*j;
  active_project -> numc[SKD] = 8+4*j*j;
  active_project -> numc[GDK] = active_project -> numc[GDR];
  active_project -> numc[BND] = j*j;
  active_project -> numc[ANG] = j*j*j + j*j*j*j;
  active_project -> numc[RIN] = 20*(j+1);
  active_project -> numc[CHA] = j+1;
  active_project -> numc[SPH] = 0;
  active_project -> numc[MSD] = 0;
  if (active_project -> steps > 1) active_project -> numc[MSD] = 14*j+6;

  if (j == 2)
  {
    active_project -> numc[GDR] = active_project -> numc[GDR] + 6;
    active_project -> numc[SQD] = active_project -> numc[SQD] + 8;
    active_project -> numc[SKD] = active_project -> numc[SKD] + 8;
    active_project -> numc[GDK] = active_project -> numc[GDK] + 6;
  }
  active_project -> numwid = active_project -> numc[GDR]
                       + active_project -> numc[SQD]
                       + active_project -> numc[SKD]
                       + active_project -> numc[GDK]
                       + active_project -> numc[BND]
                       + active_project -> numc[ANG]
                       + active_project -> numc[RIN]
                       + active_project -> numc[CHA]
                       + active_project -> numc[MSD];
  for (i=0; i<NGRAPHS; i++)
  {
    if (i != SP) alloc_curves (i);
  }
}
#endif

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
  //int i;
//  char * bar[2] = {"bond properties", "nearest neigbhors table"};
//  char * mess;
#ifdef NEW_ANA
  if (run < NGRAPHS && run > -1) active_project -> analysis[run] -> calc_ok = adv;
#else
  if (run < NGRAPHS && run > -1) active_project -> visok[run] = adv;
#endif // NEW_ANA
  if (! status)
  {
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
#ifdef GTK3
    if (widg != NULL) gdk_window_set_opacity (gtk_widget_get_window(widg), opc);
#endif
  }
}

#ifdef NEW_ANA
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
  \fn void init_atomes_analyses ()

  \brief initialize analysis data structures for atomes
*/
void init_atomes_analyses ()
{
  int i, j;

  j = active_project -> nspec;
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
  active_project -> analysis[GDR] = setup_analysis ("g(r)/G(r)", GDR, FALSE, TRUE, 16+5*j*j + ((j ==2) ? 6 : 0), 2, comp_list, "r [Å]");
  // g(r) FFT  - same compatibility list
  active_project -> analysis[GDK] = setup_analysis ("g(r)/G(r) from FFT[S(q)]", GDK, FALSE, TRUE, 16+5*j*j + ((j ==2) ? 6 : 0), 2, comp_list, "r [Å]");

  // s(q)
  comp_list[0] = SQD;
  comp_list[1] = SKD;
  active_project -> analysis[SQD] = setup_analysis ("S(q) from FFT[g(r)]", SQD, FALSE, TRUE, 8+4*j*j + ((j ==2) ? 8 : 0), 2, comp_list, "q [Å-1]");
  // s(k) - same compatibility list
  active_project -> analysis[SKD] = setup_analysis ("S(q) from Debye equation", SKD, FALSE, TRUE, 8+4*j*j + ((j ==2) ? 8 : 0), 2, comp_list, "q [Å-1]");

  g_free (comp_list);

  comp_list = allocint (1);
  // Bond length  distribution(s)
  comp_list[0] = BND;
  active_project -> analysis[BND] = setup_analysis ("Bonds properties", BND, FALSE, TRUE, j*j, 1, comp_list, "Dij [Å]");

  // Angle distribution(s)
  comp_list[0] = ANG;
  active_project -> analysis[ANG] = setup_analysis ("Angle distributions", ANG, FALSE, TRUE, j*j*j + j*j*j*j, 1, comp_list, "θ [°]");

  // Ring statistic(s)
  comp_list[0] = RIN;
  active_project -> analysis[RIN] = setup_analysis ("Ring statistics", RIN, FALSE, TRUE, 20*(j+1), 1, comp_list, "Size n of the ring [total number of nodes]");

  // Chain statistic(s)
  comp_list[0] = CHA;
  active_project -> analysis[CHA] = setup_analysis ("Chain statistics", CHA, FALSE, TRUE, j+1, 1, comp_list, "Size n of the chain [total number of nodes]");

  // Spherical harmonic(s)
  comp_list[0] = SPH;
  active_project -> analysis[SPH] = setup_analysis ("Spherical harmonics", SPH, FALSE, TRUE, 0, 1, comp_list, "Ql");

  // Mean square displacement
  comp_list[0] = MSD;
  if (active_project -> steps > 1) active_project -> analysis[MSD] = setup_analysis ("Mean Squared Displacement", MSD, TRUE, TRUE, 14*j+6, 1, comp_list, NULL);

  g_free (comp_list);
  /*
    How to add a new analysis:

       - edit the file 'global.c'

         - create a 'PACKAGE_IDC variable': gchar * PACKAGE_IDC = NULL;

       - edit the file 'global.h' to make the information available in other parts of the code:
         - define 'IDC' a new, and unique, 3 characters variable, ex: #define IDC 10
         - extern gchar * PACKAGE_IDC;
         - increment the total number of calculations available : NCALCS
         - increment increment the total number calculation using graphs : NGRAPHS (if needed)

       - to use new icon for this calculation edit the file 'main.c'
         - to read the icon file: PACKAGE_IDC = g_build_filename (PACKAGE_PREFIX, "pixmaps/idc.png", NULL);

       - edit the file 'gui.c'
         - At the top of the file modify the following variables to describe the new calculation, and create the corresponding menu elements:
           - atomes_action analyze_acts[] : {"analyze.idc",    GINT_TO_POINTER(IDC-1)}
           - char * calc_name[] : add the new calculation name for the menu items
           - char * graph_name[] : add the new calculation name for the graph windows

           - in the function 'G_MODULE_EXPORT void atomes_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data)' add the calculation menu callback:

             else if (g_strcmp0 (name, "analyze.idc") == 0)  // Update this line using the value in analyze_acts[]
             {
               on_calc_activate (NULL, data); // This does not change
             }

           - in the function 'GtkWidget * create_main_window (GApplication * atomes).' declare the icon for the new calculation:

             graph_img[IDC] = g_strdup_printf ("%s", PACKAGE_IDC);

       - edit the file 'initc.c'

         - declare the new analysis:

           active_project -> analysis[IDC] = setup_analysis (IDC, TRUE, num_graphs, num_compat, list_of_compat_calc);

       - if periodicity is required for this calculation, edit 'edit_menuc.c' edit the 'init_box_calc()' function
         to add the proper flags for :  'active_project -> analysis[IDC] -> avail_ok'

       - edit the file 'cbuild_action.c' line 1680 to add the default availability for this calculation
       - edit the file 'popup.c' line 2155 to add the default availability for this calculation

       - Curves setup if any:
         - adjust autoscale information, edit the file 'yaxis.c' line 107

       - Finally '*.apf' and '*.awf' files version should evolve to save and read the new calculation data

       - Ultimately: modify the 'preferences.c' file to offer the options to save user preferences for this calculation

  */
  active_project -> numwid = 0;

  for (i=0; i<NCALCS; i++)
  {
    if (active_project -> analysis[i]) active_project -> numwid += active_project -> analysis[i] -> numc;
  }
}
#endif
