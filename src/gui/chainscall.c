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
* @file chainscall.c
* @short Callbacks for the chains statistics calculation dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'chainscall.c'
*
* Contains:
*

 - The callbacks for the chains statistics calculation dialog

*
* List of functions:

  void initchn ();
  void update_chains_menus (glwin * view);
  void update_chains_view (project * this_proj);
  void clean_chains_data (glwin * view);

  G_MODULE_EXPORT void on_calc_chains_released (GtkWidget * widg, gpointer data);

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"
#include "initcoord.h"
#include "submenus.h"

extern gboolean run_distance_matrix (GtkWidget * widg, int calc, int up_ngb);
extern void clean_coord_window (project * this_proj);

/*!
  \fn void initchn ()

  \brief initialize the curve widgets for the chains statistics calculation
*/
void initchn ()
{
  int i;
  active_project -> analysis[CHA] -> curves[0] -> name = g_strdup_printf ("Chains - Cc(n)[All]");
  for (i=0 ; i<active_project -> nspec ; i++)
  {
    active_project -> analysis[CHA] -> curves[i+1] -> name = g_strdup_printf ("Chains - Cc(n)[%s]", active_chem -> label[i]);
  }
  addcurwidgets (activep, CHA, 0);
  active_project -> analysis[CHA] -> init_ok = TRUE;
}

#ifdef GTK3
/*!
  \fn void update_chains_menus (glwin * view)

  \brief update the chains statistics menus

  \param view the gliwn to update the menu from
*/
void update_chains_menus (glwin * view)
{
  GtkWidget * menu;
  view -> ogl_chains[0] = destroy_this_widget (view -> ogl_chains[0]);
  view -> ogl_chains[0] = menu_item_new_with_submenu ("Chain(s)", view -> chains, add_menu_coord (view, 0, 9));
  menu = gtk_menu_item_get_submenu (GTK_MENU_ITEM (view -> ogl_coord[0]));
  if (GTK_IS_WIDGET(menu))
  {
    gtk_menu_shell_insert (GTK_MENU_SHELL(menu), view -> ogl_chains[0], 3);
    show_the_widgets (view -> ogl_chains[0]);
  }
}
#endif

/*!
  \fn void update_chains_view (project * this_proj)

  \brief update the chains statistics text view after the calculation

  \param this_proj the target project
*/
void update_chains_view (project * this_proj)
{
  int i, j, k, l;
  gchar * nelt;
  gchar * col;
  gchar * tab;
  gchar * cid;
  gchar * str;

  if (this_proj -> analysis[CHA] -> calc_buffer == NULL) this_proj -> analysis[CHA] -> calc_buffer = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> analysis[CHA] -> calc_buffer);
  j = this_proj -> csparam[0];
  if (! j)
  {
    nelt = g_strdup_printf ("All");
    col = NULL;
  }
  else
  {
    nelt = g_strdup_printf ("%s", this_proj -> chemistry -> label[j-1]);
    col = textcolor(j-1);
  }
  print_info ("\n\nChain statistics\n\n", "heading", this_proj -> analysis[CHA] -> calc_buffer);

  if (this_proj -> csparam[1])
  {
    print_info (" * only AAAA chains have been considered\n", "italic", this_proj -> analysis[CHA] -> calc_buffer);
  }
  if (this_proj -> csparam[2])
  {
    print_info (" * only ABAB chains have been considered\n", "italic", this_proj -> analysis[CHA] -> calc_buffer);
  }
  if (this_proj -> csparam[3])
  {
    print_info (" * homopolar bonds can not shorten the chains\n", "italic", this_proj -> analysis[CHA] -> calc_buffer);
  }
  if (this_proj -> csparam[4])
  {
    print_info (" * only 1-(2)", "italic", this_proj -> analysis[CHA] -> calc_buffer);
    print_info ("n", "sub_italic", this_proj -> analysis[CHA] -> calc_buffer);
    print_info ("-1 chains have been considered, ie. isolated chains\n", "italic", this_proj -> analysis[CHA] -> calc_buffer);
  }
  print_info ("\n Atom(s) used to start the search: ", NULL, this_proj -> analysis[CHA] -> calc_buffer);
  print_info (nelt, col, this_proj -> analysis[CHA] -> calc_buffer);
  if (j != 0) print_info (" atom(s) only", NULL, this_proj -> analysis[CHA] -> calc_buffer);

  if (this_proj -> steps > 1)
  {
    print_info ("\n Average number of chains per configuration: ", NULL, this_proj -> analysis[CHA] -> calc_buffer);
    str = g_strdup_printf ("%f", this_proj -> csdata[0]);
    print_info (str, "bold", this_proj -> analysis[CHA] -> calc_buffer);
    g_free (str);
    str = g_strdup_printf (" +/- %f\n", this_proj -> csdata[1]);
    print_info (str, "bold", this_proj -> analysis[CHA] -> calc_buffer);
    g_free (str);
  }
  else
  {
    print_info ("\n Total number of chains: ", NULL, this_proj -> analysis[CHA] -> calc_buffer);
    str = g_strdup_printf ("%f\n", this_proj -> csdata[0]);
    print_info (str, "bold", this_proj -> analysis[CHA] -> calc_buffer);
    g_free (str);
  }

  if (this_proj -> steps > 1)
  {
    print_info ("\n\t n     Av. by step \tCc(n)[", "bold", this_proj -> analysis[CHA] -> calc_buffer);
    print_info (nelt, col, this_proj -> analysis[CHA] -> calc_buffer);
    if (j == this_proj -> nspec)
    {
      print_info ("]\t  +/-\n", "bold", this_proj -> analysis[CHA] -> calc_buffer);
    }
    else
    {
      print_info ("]\t   +/-\n", "bold", this_proj -> analysis[CHA] -> calc_buffer);
    }
  }
  else
  {
    print_info ("\n\t n\tNumber\t\tCc(n)[", "bold", this_proj -> analysis[CHA] -> calc_buffer);
    print_info (nelt, col, this_proj -> analysis[CHA] -> calc_buffer);
    print_info ("]\n", NULL, this_proj -> analysis[CHA] -> calc_buffer);
  }
  tab = NULL;
  cid = NULL;
  k = this_proj -> csparam[0];
  l = (! k) ? this_proj -> natomes : this_proj -> chemistry -> nsps[k-1];
  j = 1;
  for ( i=1 ; i < this_proj -> csparam[5] ; i++ )
  {
    if (this_proj -> analysis[CHA] -> curves[k] -> data[1][i] != 0.0)
    {
      j ++;
      if (j - 2*(j/2) == 0)
      {
        tab = g_strdup_printf ("grey_back");
        cid = g_strdup_printf ("bold_grey_back");
      }
      else
      {
        tab = NULL;
        cid = g_strdup_printf ("bold");
      }
      print_info ("\t", NULL, this_proj -> analysis[CHA] -> calc_buffer);
      if (i < 9)
      {
        print_info (" ",cid, this_proj -> analysis[CHA] -> calc_buffer);
      }
      str = g_strdup_printf("%d", i+1);
      print_info (str, cid, this_proj -> analysis[CHA] -> calc_buffer);
      g_free (str);
      str = g_strdup_printf("\t%f\t", l*this_proj -> analysis[CHA] -> curves[k] -> data[1][i]);
      print_info (str, tab, this_proj -> analysis[CHA] -> calc_buffer);
      g_free (str);
      str = g_strdup_printf("%f\t", this_proj -> analysis[CHA] -> curves[k] -> data[1][i]);
      print_info (str, tab, this_proj -> analysis[CHA] -> calc_buffer);
      g_free (str);
      if (this_proj -> steps > 1)
      {
        str = g_strdup_printf("%f\t", this_proj -> analysis[CHA] -> curves[k] -> err[i]);
        print_info (str, tab, this_proj -> analysis[CHA] -> calc_buffer);
        g_free (str);
      }
       print_info ("\n", NULL, this_proj -> analysis[CHA] -> calc_buffer);
      if (tab != NULL)
      {
        g_free (tab);
      }
      if (cid != NULL)
      {
        g_free (cid);
      }
    }
  }
  print_info (calculation_time(TRUE, this_proj -> analysis[CHA] -> calc_time), NULL, this_proj -> analysis[CHA] -> calc_buffer);
  g_free (nelt);
  if (col != NULL)
  {
    g_free (col);
  }
}

/*!
  \fn void clean_chains_data (glwin * view)

  \brief cleaning the OpenGL data related to chain statistics

  \param view the gliwn to clean the data from
*/
void clean_chains_data (glwin * view)
{
  project * this_proj = get_project_by_id(view -> proj);
  if (this_proj -> coord -> totcoord[9])
  {
    if (this_proj -> coord -> ntg[9]) g_free (this_proj -> coord -> ntg[9]);
    this_proj -> coord -> ntg[9] = NULL;
    if (this_proj -> coord -> geolist[9][0]) g_free (this_proj -> coord -> geolist[9][0]);
    this_proj -> coord -> geolist[9][0] = NULL;
    this_proj -> coord -> totcoord[9] = 0;
  }
  if (view -> all_chains) g_free (view -> all_chains);
  view -> all_chains = NULL;
  if (view -> num_chains) g_free (view -> num_chains);
  view -> num_chains = NULL;
  if (view -> gcid[9]) g_free (view -> gcid[9]);
  view -> gcid[9] = NULL;
  view -> chains = FALSE;
  view -> chain_max = 0;
  int i, j;
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      if (this_proj -> atoms[i][j].chain)
      {
        g_free (this_proj -> atoms[i][j].chain);
        this_proj -> atoms[i][j].chain = NULL;
      }
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void on_calc_chains_released (GtkWidget * widg, gpointer data)

  \brief compute chains statistics

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_chains_released (GtkWidget * widg, gpointer data)
{
  int j, k;

  cutoffsend ();
  //if (active_project -> steps > 1) statusb = 1;

  if (! active_project -> analysis[CHA] -> init_ok) initchn ();
  active_project -> csparam[6] = 0;
  if (! active_project -> dmtx) active_project -> dmtx = run_distance_matrix (widg, 6, 0);

  if (active_project -> dmtx)
  {
    clean_curves_data (CHA, 0, active_project -> analysis[CHA] -> numc);
    clean_chains_data (active_glwin);
    active_glwin -> all_chains = g_malloc0 (active_project -> steps*sizeof*active_glwin -> all_chains);
    active_glwin -> num_chains = g_malloc0 (active_project -> steps*sizeof*active_glwin -> num_chains);
    for (j=0; j<active_project -> steps; j++)
    {
      active_glwin -> all_chains[j] = g_malloc0 (active_project -> csparam[5]*sizeof*active_glwin -> all_chains[j]);
      active_glwin -> num_chains[j] = allocint (active_project -> csparam[5]);
      for (k=0; k < active_project -> natomes; k++)
      {
        if (active_project -> atoms[j][k].chain) g_free (active_project -> atoms[j][k].chain);
        active_project -> atoms[j][k].chain = NULL;
        active_project -> atoms[j][k].chain = g_malloc0 (active_project -> csparam[5]*sizeof*active_project -> atoms[j][k].chain);
      }
    }
    k = 1;
    prepostcalc (widg, FALSE, CHA, 0, opac);
    clock_gettime (CLOCK_MONOTONIC, & start_time);
    j = initchains_ (& active_project -> csparam[0],
                     & active_project -> csparam[1],
                     & active_project -> csparam[2],
                     & active_project -> csparam[3],
                     & active_project -> csparam[4],
                     & active_project -> csparam[5],
                     & active_project -> csearch);
    clock_gettime (CLOCK_MONOTONIC, & stop_time);
    active_project -> analysis[CHA] -> calc_time = get_calc_time (start_time, stop_time);
    if (j == 0)
    {
      show_error ("The chain statistics calculation has failed", 0, widg);
    }
    else if (j == 2)
    {
      gchar * str = g_strdup_printf ("\t<b>The chain statistics have failed !</b>\n\n"
                                     "The number of chain per MD step appears\n"
                                     "to be bigger than the initial value of <b>%d</b>\n"
                                     "used to allocate memory to store the results.\n\n"
                                     "Increase the value and start again !",
                                     active_project -> csearch);
      show_error (str, 0, widg);
      g_free (str);
      j = 0;
    }
    prepostcalc (widg, TRUE, CHA, j, 1.0);
  }
  else
  {
    show_error ("The nearest neighbors table calculation has failed", 0, widg);
  }
  if (active_coord -> totcoord[9])
  {
    active_project -> csparam[6] = 1;
    active_glwin -> chains = TRUE;
    active_glwin -> chain_max = active_project -> csparam[5];
    update_chains_view (active_project);
  }
  show_the_widgets (curvetoolbox);
  clean_coord_window (active_project);
  fill_tool_model ();
#ifdef GTK3
  update_chains_menus (active_glwin);
#endif
}

/*!
  \fn void save_chains_data_ (int * taille, double ectrc[*taille], double * rpstep, double * ectrpst)

  \brief get chains statistics results form Fortran90

  \param taille number of data points
  \param ectrc standard deviation per MD step
  \param rpstep chains per MD step
  \param ectrpst standard deviation
*/
void save_chains_data_ (int * taille, double ectrc[* taille], double * rpstep, double * ectrpst)
{
  int i;
  active_project -> csdata[0] = * rpstep;
  active_project -> csdata[1] = * ectrpst;
  i = active_project -> csparam[0];
  active_project -> analysis[CHA] -> curves[i] -> err = duplicate_double (* taille, ectrc);
}
