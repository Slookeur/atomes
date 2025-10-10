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
* @file spcall.c
* @short Callbacks for the spherical harmonics calculation dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'spcall.c'
*
* Contains:
*

 - The callbacks for the spherical harmonics calculation dialog

*
* List of functions:

  void initsh (int str);
  void update_spherical_view (project * this_proj);

  G_MODULE_EXPORT void on_calc_sph_released (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"

extern void alloc_curves (int c);
extern gboolean run_distance_matrix (GtkWidget * widg, int calc, int up_ngb);

/*!
  \fn void initsh (int str)

  \brief initialize the curve widgets for the spherical harmonics

  \param str initialize or not (1/0)
*/
void initsh (int str)
{
  int i, j, k;
  if (str)
  {
#ifdef NEW_ANA
    active_project -> numwid -= active_project -> analysis[SP].numc;
    active_project -> analysis[SP].numc = active_project -> nspec;
    for (i=0; i<active_project -> nspec; i++)
    {
      active_project -> analysis[SP].numc += active_coord -> ntg[1][i];
    }
    alloc_curves (SP);
    active_project -> numwid += active_project -> analysis[SP].numc;
    j = 0;
    for (i = 0 ; i < active_project -> nspec ; i++)
    {
      active_project -> analysis[SP].curves[i+j] -> name = g_strdup_printf("Q(l) [%s] (l=0 -> %d)",
                                                                  active_chem -> label[i],
                                                                  active_project -> analysis[SP].num_delta);
      j += active_coord -> ntg[1][i];
    }
    k = 1;
    for (i=0 ; i < active_project -> nspec; i++)
    {
      for (j=0 ; j < active_coord -> ntg[1][i]; j++)
      {
        active_project -> analysis[SP].curves[j+k] -> name = g_strdup_printf("Q(l) %s (l=0 -> %d)",
                                                                    exact_name(env_name (active_project, j, i, 0, NULL)),
                                                                               active_project -> analysis[SP].num_delta);
      }
      k += active_coord -> ntg[1][i]+1;
    }
    addcurwidgets (activep, SP, 0);
    active_project -> analysis[SP].init_ok = TRUE;
#else
    active_project -> numwid -= active_project -> numc[SP];
    active_project -> numc[SP] = active_project -> nspec;
    for (i=0; i<active_project -> nspec; i++)
    {
      active_project -> numc[SP] += active_coord -> ntg[1][i];
    }
    alloc_curves (SP);
    active_project -> numwid += active_project -> numc[SP];
    j = 0;
    for (i = 0 ; i < active_project -> nspec ; i++)
    {
      active_project -> curves[SP][i+j] -> name = g_strdup_printf("Q(l) [%s] (l=0 -> %d)",
                                                                  active_chem -> label[i],
                                                                  active_project -> num_delta[SP]);
      j += active_coord -> ntg[1][i];
    }
    k = 1;
    for (i=0 ; i < active_project -> nspec; i++)
    {
      for (j=0 ; j < active_coord -> ntg[1][i]; j++)
      {
        active_project -> curves[SP][j+k] -> name = g_strdup_printf("Q(l) %s (l=0 -> %d)",
                                                                    exact_name(env_name (active_project, j, i, 0, NULL)),
                                                                               active_project -> num_delta[SP]);
      }
      k += active_coord -> ntg[1][i]+1;
    }
    addcurwidgets (activep, SP, 0);
    active_project -> initok[SP] = TRUE;
#endif
  }
}

/*!
  \fn void update_spherical_view (project * this_proj)

  \brief update the text view for spherical harmonics

  \param this_proj the target project
*/
void update_spherical_view (project * this_proj)
{
  int i, j, k, l, m;
  gchar * str;
  gchar * tab;
  gchar * cid;
#ifdef NEW_ANA
  if (this_proj -> analysis[SP].calc_buffer == NULL) this_proj -> analysis[SP].calc_buffer = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> analysis[SP].calc_buffer);

  print_info ("\n\nSpherical harmonics\n\n", "heading", this_proj -> analysis[SP].calc_buffer);
  m = 0;
  for (i=0; i<this_proj -> nspec; i++)
  {
    print_info ("\nResults for the ", NULL, this_proj -> analysis[SP].calc_buffer);
    print_info (exact_name(active_chem -> label[i]), textcolor(i), this_proj -> analysis[SP].calc_buffer);
    print_info (" atoms: \n\n", NULL, this_proj -> analysis[SP].calc_buffer);
    // Here print average spec info

    print_info ("\tl\t", "bold_italic", this_proj -> analysis[SP].calc_buffer);
    print_info ("Q(","bold", this_proj -> analysis[SP].calc_buffer);
    print_info ("l", "bold_italic", this_proj -> analysis[SP].calc_buffer);
    print_info (")","bold", this_proj -> analysis[SP].calc_buffer);
    print_info (active_chem -> label[i], textcolor(i), this_proj -> analysis[SP].calc_buffer);
    print_info ("[All]", "bold", this_proj -> analysis[SP].calc_buffer);
    for (j=0 ; j < active_coord -> ntg[1][i]; j++)
    {
      print_info ("\tQ(","bold", this_proj -> analysis[SP].calc_buffer);
      print_info ("l", "bold_italic", this_proj -> analysis[SP].calc_buffer);
      print_info (")","bold", this_proj -> analysis[SP].calc_buffer);
      env_name (this_proj, j, i, 1, this_proj -> analysis[SP].calc_buffer);
    }
    print_info ("\n", NULL, this_proj -> analysis[SP].calc_buffer);
    k = 1;
    tab = NULL;
    cid = NULL;
    for (j=0; j<this_proj -> num_delta[SP]/2+1 ; j++)
    {
      k ++;
      if (k - 2*(k/2) == 0)
      {
        tab = g_strdup_printf ("grey_back");
        cid = g_strdup_printf ("bold_grey_back");
      }
      else
      {
        tab = NULL;
        cid = g_strdup_printf ("bold");
      }
      print_info ("\t", NULL, this_proj -> analysis[SP].calc_buffer);
      if (j < 5)
      {
        print_info (" ",cid, this_proj -> analysis[SP].calc_buffer);
      }
      str = g_strdup_printf("%d", 2*j);
      print_info (str, cid, this_proj -> analysis[SP].calc_buffer);
      g_free (str);
      for (l=0; l<active_coord -> ntg[1][i]+1; l++)
      {
        str = g_strdup_printf("\t%f", this_proj -> analysis[SP].curves[l+m] -> data[1][j]);
        print_info (str, tab, this_proj -> analysis[SP].calc_buffer);
        g_free (str);
      }
      print_info ("\n", NULL, this_proj -> analysis[SP].calc_buffer);
      if (tab != NULL)
      {
        g_free (tab);
      }
      if (cid != NULL)
      {
        g_free (cid);
      }
    }
    m += active_coord -> ntg[1][i]+1;
  }

  print_info (calculation_time(TRUE, this_proj -> calc_time[SP]), NULL, this_proj -> analysis[SP].calc_buffer);
#else
  if (this_proj -> text_buffer[SP+OT] == NULL) this_proj -> text_buffer[SP+OT] = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> text_buffer[SP+OT]);

  print_info ("\n\nSpherical harmonics\n\n", "heading", this_proj -> text_buffer[SP+OT]);
  m = 0;
  for (i=0; i<this_proj -> nspec; i++)
  {
    print_info ("\nResults for the ", NULL, this_proj -> text_buffer[SP+OT]);
    print_info (exact_name(active_chem -> label[i]), textcolor(i), this_proj -> text_buffer[SP+OT]);
    print_info (" atoms: \n\n", NULL, this_proj -> text_buffer[SP+OT]);
    // Here print average spec info

    print_info ("\tl\t", "bold_italic", this_proj -> text_buffer[SP+OT]);
    print_info ("Q(","bold", this_proj -> text_buffer[SP+OT]);
    print_info ("l", "bold_italic", this_proj -> text_buffer[SP+OT]);
    print_info (")","bold", this_proj -> text_buffer[SP+OT]);
    print_info (active_chem -> label[i], textcolor(i), this_proj -> text_buffer[SP+OT]);
    print_info ("[All]", "bold", this_proj -> text_buffer[SP+OT]);
    for (j=0 ; j < active_coord -> ntg[1][i]; j++)
    {
      print_info ("\tQ(","bold", this_proj -> text_buffer[SP+OT]);
      print_info ("l", "bold_italic", this_proj -> text_buffer[SP+OT]);
      print_info (")","bold", this_proj -> text_buffer[SP+OT]);
      env_name (this_proj, j, i, 1, this_proj -> text_buffer[SP+OT]);
    }
    print_info ("\n", NULL, this_proj -> text_buffer[SP+OT]);
    k = 1;
    tab = NULL;
    cid = NULL;
    for (j=0; j<this_proj -> num_delta[SP]/2+1 ; j++)
    {
      k ++;
      if (k - 2*(k/2) == 0)
      {
        tab = g_strdup_printf ("grey_back");
        cid = g_strdup_printf ("bold_grey_back");
      }
      else
      {
        tab = NULL;
        cid = g_strdup_printf ("bold");
      }
      print_info ("\t", NULL, this_proj -> text_buffer[SP+OT]);
      if (j < 5)
      {
        print_info (" ",cid, this_proj -> text_buffer[SP+OT]);
      }
      str = g_strdup_printf("%d", 2*j);
      print_info (str, cid, this_proj -> text_buffer[SP+OT]);
      g_free (str);
      for (l=0; l<active_coord -> ntg[1][i]+1; l++)
      {
        str = g_strdup_printf("\t%f", this_proj -> curves[SP][l+m] -> data[1][j]);
        print_info (str, tab, this_proj -> text_buffer[SP+OT]);
        g_free (str);
      }
      print_info ("\n", NULL, this_proj -> text_buffer[SP+OT]);
      if (tab != NULL)
      {
        g_free (tab);
      }
      if (cid != NULL)
      {
        g_free (cid);
      }
    }
    m += active_coord -> ntg[1][i]+1;
  }

  print_info (calculation_time(TRUE, this_proj -> calc_time[SP]), NULL, this_proj -> text_buffer[SP+OT]);
#endif
}

/*!
  \fn G_MODULE_EXPORT void on_calc_sph_released (GtkWidget * widg, gpointer data)

  \brief compute spherical harmonics

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_sph_released (GtkWidget * widg, gpointer data)
{
  int i, j, k, l, m;
#ifdef NEW_ANA
  if (! active_project -> analysis[SP].init_ok) initsh(1);
  if (! active_project -> dmtx) active_project -> dmtx = run_distance_matrix (widg, 0, 0);

  if (active_project -> dmtx)
  {
    clean_curves_data (SP, 0, active_project -> analysis[SP].numc);
    prepostcalc (widg, FALSE, SP, 0, opac);
    k = 0;
    l = active_project -> nspec;
    m = active_project -> analysis[SP].num_delta;
    clock_gettime (CLOCK_MONOTONIC, & start_time);
    for (i=0; i<active_project -> nspec; i++)
    {
      for (j=0; j< active_coord -> ntg[1][i]; j++)
      {
        if (j==0) k ++;
        l += sphericals_ (& m, & i, & j, & k, active_coord -> partial_geo[i][j]);
        k ++;
      }
    }
    clock_gettime (CLOCK_MONOTONIC, & stop_time);
    active_project -> analysis[SP].calc_time = get_calc_time (start_time, stop_time);
    if (l != active_project -> analysis[SP].numc)
#else
  if (! active_project -> initok[SP]) initsh(1);
  if (! active_project -> dmtx) active_project -> dmtx = run_distance_matrix (widg, 0, 0);

  if (active_project -> dmtx)
  {
    clean_curves_data (SP, 0, active_project -> numc[SP]);
    prepostcalc (widg, FALSE, SP, 0, opac);
    k = 0;
    l = active_project -> nspec;
    m = active_project -> num_delta[SP];
    clock_gettime (CLOCK_MONOTONIC, & start_time);
    for (i=0; i<active_project -> nspec; i++)
    {
      for (j=0; j< active_coord -> ntg[1][i]; j++)
      {
        if (j==0) k ++;
        l += sphericals_ (& m, & i, & j, & k, active_coord -> partial_geo[i][j]);
        k ++;
      }
    }
    clock_gettime (CLOCK_MONOTONIC, & stop_time);
    active_project -> calc_time[SP] = get_calc_time (start_time, stop_time);
    if (l != active_project -> numc[SP])
#endif
    {
      i = 0;
    }
    else
    {
      i = 1;
    }
    prepostcalc (widg, TRUE, SP, i, 1.0);
    if (! i)
    {
      show_error ("Unexpected error when analyzing the spherical harmonics", 0, widg);
    }
    else
    {
      update_spherical_view (active_project);
      show_the_widgets (curvetoolbox);
    }
  }
  else
  {
    show_error ("The nearest neighbors table calculation has failed", 0, widg);
  }
  fill_tool_model ();
}
