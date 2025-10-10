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
  for (i=start; i<end; i++)
  {
    if (active_project -> curves[calc])
    {
      clean_this_curve_window (i, calc);
    }
  }
}

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
  active_project -> numc[GR] = 16+5*j*j;
  active_project -> numc[SQ] = 8+4*j*j;
  active_project -> numc[SK] = 8+4*j*j;
  active_project -> numc[GK] = active_project -> numc[GR];
  active_project -> numc[BD] = j*j;
  active_project -> numc[AN] = j*j*j + j*j*j*j;
  active_project -> numc[RI] = 20*(j+1);
  active_project -> numc[CH] = j+1;
  active_project -> numc[SP] = 0;
  active_project -> numc[MS] = 0;
  if (active_project -> steps > 1) active_project -> numc[MS] = 14*j+6;

  if (j == 2)
  {
    active_project -> numc[GR] = active_project -> numc[GR] + 6;
    active_project -> numc[SQ] = active_project -> numc[SQ] + 8;
    active_project -> numc[SK] = active_project -> numc[SK] + 8;
    active_project -> numc[GK] = active_project -> numc[GK] + 6;
  }
  active_project -> numwid = active_project -> numc[GR]
                       + active_project -> numc[SQ]
                       + active_project -> numc[SK]
                       + active_project -> numc[GK]
                       + active_project -> numc[BD]
                       + active_project -> numc[AN]
                       + active_project -> numc[RI]
                       + active_project -> numc[CH]
                       + active_project -> numc[MS];
  for (i=0; i<NGRAPHS; i++)
  {
    if (i != SP) alloc_curves (i);
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
  //int i;
//  char * bar[2] = {"bond properties", "nearest neigbhors table"};
//  char * mess;
  if (run < NGRAPHS && run > -1) active_project -> visok[run] = adv;
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
  this_analysis -> idcc = g_malloc0 (this_analysis -> *sizeof*this_analysis -> idcc);
  if (this_analysis -> curves != NULL)
  {
    g_free (this_analysis -> curves);
    this_analysis -> curves = NULL;
  }
  this_analysis -> curves = g_malloc (this_analysis -> numc*sizeof*this_analysis -> curves);
  for (i = 0; i < this_analysis -> numc; i++)
  {
    this_analysis -> curves[i] = g_malloc0 (sizeof*this_analysis -> curves[i]);
    this_analysis -> curves[i] -> cfile = NULL;
    this_analysis -> curves[i] -> name = NULL;
    this_analysis -> curves[i] -> axis_title[0] = NULL;
    this_analysis -> curves[i] -> axis_title[1] = NULL;
  }
}

/*!
  \fn atomes_analysis * setup_analysis (int analysis, gboolean graph, int num_curves, int n_compat, int * compat)

  \brief allocate atomes_analysis data structure

  \param analysis
  \param graph
  \param num_curves
  \param n_compat
  \param compat
*/
atomes_analysis * setup_analysis (int analysis, gboolean graph, int num_curves, int n_compat, int * compat)
{
  atomes_analysis * new_analysis = g_malloc0(sizeof*atomes_analysis);
  new_analysis -> aid = analysis;
  new_analysis -> graph_res = graph;
  if (graph)
  {
    new_analysis -> compat_id = duplicate_int (n_compat, compat);
    new_analysis -> numc = num_curves;
    alloc_analysis_curves (new_analysis);
  }
}

/*
  From global.h:

  #define NCALCS 10
  #define NGRAPHS 10

  #define GR 0
  #define SQ 1
  #define SK 2
  #define GK 3
  #define BD 4
  #define AN 5
  #define RI 6
  #define CH 7
  #define SP 8
  #define MS 9
*/

/*!
  \fn void init_atomes_analyses ()

  \brief initialize analysis data structures for atomes
*/
void init_atomes_analyses ()
{
  int i, j;
  active_project -> total_analysis = NCALCS;
  active_project -> analysis = g_malloc0(NCALCS*sizeof*active_project -> analysis);
  j = active_project -> nspec;
  // g(r)
  active_project -> analysis[GR] = setup_analysis (GR, TRUE, 16+5*j*j + (j ==2) ? 6 : 0, 2, {GR, GK});
  // s(q)
  active_project -> analysis[SQ] = setup_analysis (SQ, TRUE, 8+4*j*j + (j ==2) ? 8 : 0, 2, {SQ, SK});
  // s(k)
  active_project -> analysis[SK] = setup_analysis (SK, TRUE, 8+4*j*j + (j ==2) ? 8 : 0, 2, {SQ, SK});
  // g(r) FFT
  active_project -> analysis[GK] = setup_analysis (GK, TRUE, 16+5*j*j + (j ==2) ? 6 : 0, 2, {GR, GK});
  // Bond length  distribution(s)
  active_project -> analysis[BD] = setup_analysis (BD, TRUE, j*j, 1, BD);
  // Angle distribution(s)
  active_project -> analysis[AN] = setup_analysis (AN, TRUE, j*j*j + j*j*j*j, 1, AN);
  // Ring statistic(s)
  active_project -> analysis[RI] = setup_analysis (RI, TRUE, 20*(j+1), 1, RI);
  // Chain statistic(s)
  active_project -> analysis[CH] = setup_analysis (CH, TRUE, j+1, 1, CH);
  // Mean square displacement
  if (active_project -> steps > 1) active_project -> analisys[MS] = setup_analysis (MS, TRUE, 14*j+6, 1, MS);

  /*
    How to add a new analysis:

       - edit the file 'global.c'
         - create a PACKAGE_ID variable: gchar * PACKAGE_ID = NULL;
         - increment NCALCS
         - increment NGRAPHS if needed
         - define ID a new, and unique, 2 character variable, ex: GR

       - edit the file 'global.h' to make the information available in other parts of the code: extern gchar * PACKAGE_ID;

       - edit the file 'main.c'
         - to read the icon file: PACKAGE_ID = g_build_filename (PACKAGE_PREFIX, "pixmaps/id.png", NULL);

       - edit the file 'gui.c'
         - modifiy the following variables to describe the new calculation, and create the corresponding menu elements:
           - atomes_action analyze_acts[]
           - char * calc_name[]
           - char * graph_name[]
           - in the function 'G_MODULE_EXPORT void atomes_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data)' add the calculation menu callback:

             else if (g_strcmp0 (name, "analyze.id") == 0)
             {
               on_calc_activate (NULL, data); // This does not change
             }

           - in the function 'GMenu * create_analyze_menu ()' create the new menu element :

             append_menu_item (menu, "MNew calculation", "app.analyze.id", NULL, NULL, IMG_FILE, PACKAGE_ID, FALSE, FALSE, FALSE, NULL);

           - in the function 'GtkWidget * create_main_window (GApplication * atomes).' declare the icon for the new calculation:

             calc_img[ID] = g_strdup_printf ("%s", PACKAGE_ID);

       - edit the file 'initc.c'

         - define the maximum number of graph that the analysis is producing
         - define the list of compatible analysis, and list the analyis ID.

           active_project -> analysis[ID] = setup_analysis (ID, TRUE, num_graphs, num_compat, {compat_1, compat_2, ...});

       - if periodicity is required for this calculation, edit 'edit_menuc.c' edit the 'init_box_calc()' function
         to add the proper flags for :  'active_project -> analysis[ID].run_ok'

       - edit the file 'cbuild_action.c' line 1680 to add the default availability for this calculation
       - edit the file 'popup.c' line 2155 to add the default availability for this calculation

       - Modify the 'preferences.c' file to offer the options to save user preferences for this calculation

       - Finally apf and awf files version should evolve to save and read the new calculation data

  */
  for (i=0; i<NCALCS; i++) active_project -> numwid = active_project -> numwid + active_project -> analysis[i].numc;
}
#endif
