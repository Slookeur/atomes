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
* @file grcall.c
* @short Callbacks for the g(r)/g(k) calculation dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'grcall.c'
*
* Contains:
*

 - The callbacks for the g(r)/g(k) calculation dialog

*
* List of functions:

  int recup_data_ (int * cd, int * rd);

  void initgr (int r);
  void update_rdf_view (project * this_proj, int rdf);
  void sendcutoffs_ (int * nc, double * totc, double partc[* nc][* nc]);

  G_MODULE_EXPORT void on_calc_gr_released (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void on_cutcheck_toggled (GtkToggleButton * Button);
  G_MODULE_EXPORT void on_calc_gq_released (GtkWidget * widg, gpointer data);

*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "curve.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"

int fitc = 0;

/*!
  \fn void initgr (int r)

  \brief initialize the curve widgets for the g(r)/g(k)

  \param r GR = real space, GK = FFT
*/
void initgr (int r)
{
  int i, j, k;
  active_project -> analysis[r] -> curves[0] -> name = g_strdup_printf ("g(r) neutrons");
  active_project -> analysis[r] -> curves[1] -> name = g_strdup_printf ("g(r) neutrons - smoothed");
  active_project -> analysis[r] -> curves[2] -> name = g_strdup_printf ("G(r) neutrons");
  active_project -> analysis[r] -> curves[3] -> name = g_strdup_printf ("G(r) neutrons - smoothed");
  active_project -> analysis[r] -> curves[4] -> name = g_strdup_printf ("D(r) neutrons");
  active_project -> analysis[r] -> curves[5] -> name = g_strdup_printf ("D(r) neutrons - smoothed");
  active_project -> analysis[r] -> curves[6] -> name = g_strdup_printf ("T(r) neutrons");
  active_project -> analysis[r] -> curves[7] -> name = g_strdup_printf ("T(r) neutrons - smoothed");
  active_project -> analysis[r] -> curves[8] -> name = g_strdup_printf ("g(r) X-rays");
  active_project -> analysis[r] -> curves[9] -> name = g_strdup_printf ("g(r) X-rays - smoothed");
  active_project -> analysis[r] -> curves[10] -> name = g_strdup_printf ("G(r) X-rays");
  active_project -> analysis[r] -> curves[11] -> name = g_strdup_printf ("G(r) X-rays - smoothed");
  active_project -> analysis[r] -> curves[12] -> name = g_strdup_printf ("D(r) X-rays");
  active_project -> analysis[r] -> curves[13] -> name = g_strdup_printf ("D(r) X-rays - smoothed");
  active_project -> analysis[r] -> curves[14] -> name = g_strdup_printf ("T(r) X-rays");
  active_project -> analysis[r] -> curves[15] -> name = g_strdup_printf ("T(r) X-rays - smoothed");
  k = 16;
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    for ( j = 0 ; j < active_project -> nspec ; j++ )
    {
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("g(r)[%s,%s]", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("g(r)[%s,%s] - smoothed", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("G(r)[%s,%s]", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("G(r)[%s,%s] - smoothed", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
      active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf ("dn(r)[%s,%s]", active_chem -> label[i], active_chem -> label[j]);
      k=k+1;
    }
  }
  if ( active_project -> nspec == 2 )
  {
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf("BT(r)[NN]");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf("BT(r)[NN] - smoothed");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf("BT(r)[NC]");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf("BT(r)[NC] - smoothed");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf("BT(r)[CC]");
    k=k+1;
    active_project -> analysis[r] -> curves[k] -> name = g_strdup_printf("BT(r)[CC] - smoothed");
  }
  add_curve_widgets (activep, r, 0);
  active_project -> analysis[r] -> init_ok = TRUE;
}

/*!
  \fn void update_rdf_view (project * this_proj, int rdf)

  \brief update the project text view for the g(r)/g(k) calculation

  \param this_proj the target project
  \param rdf the calculation GR / GK
*/
void update_rdf_view (project * this_proj, int rdf)
{
  gchar * str;
  if (this_proj -> analysis[rdf] -> calc_buffer == NULL) this_proj -> analysis[rdf] -> calc_buffer = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("\n\nRadial distribution function(s)", "heading", this_proj -> analysis[rdf] -> calc_buffer);
  if (rdf == GDR)
  {
    print_info (" - real space calculation\n\n", "heading", this_proj -> analysis[rdf] -> calc_buffer);
  }
  else
  {
    print_info (" - FFT[S(q)]\n\n", "heading", this_proj -> analysis[rdf] -> calc_buffer);
  }
  print_info ("Calculation details:\n\n", NULL, this_proj -> analysis[rdf] -> calc_buffer);

  if (rdf == GDK)
  {
    print_info ("\tReciprocal space data:\n\n", NULL, this_proj -> analysis[rdf] -> calc_buffer);
    print_info ("\t - Minimum vector Q", "bold", this_proj -> analysis[rdf] -> calc_buffer);
    print_info ("min", "sub_bold", this_proj -> analysis[rdf] -> calc_buffer);
    print_info (": ", "bold", this_proj -> analysis[rdf] -> calc_buffer);
    str = g_strdup_printf ("%f", this_proj -> analysis[SKD] -> min);
    print_info (str, "bold_red", this_proj -> analysis[rdf] -> calc_buffer);
    g_free (str);
    print_info (" Å", "bold", this_proj -> analysis[rdf] -> calc_buffer);
    print_info ("-1", "sup_bold", this_proj -> analysis[rdf] -> calc_buffer);
    print_info ("\n\t - Maximum vector Q", "bold", this_proj -> analysis[rdf] -> calc_buffer);
    print_info ("max", "sub_bold", this_proj -> analysis[rdf] -> calc_buffer);
    print_info (" for the FFT: ", "bold", this_proj -> analysis[rdf] -> calc_buffer);
    str = g_strdup_printf ("%f", this_proj -> analysis[rdf] -> max);
    print_info (str, "bold_red", this_proj -> analysis[rdf] -> calc_buffer);
    g_free (str);
    print_info (" Å", "bold", this_proj -> analysis[rdf] -> calc_buffer);
    print_info ("-1", "sup_bold", this_proj -> analysis[rdf] -> calc_buffer);
    print_info ("\n\n", NULL, this_proj -> analysis[rdf] -> calc_buffer);
  }
  print_info ("\tReal space discretization:\n\n", NULL, this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("\t - Number of δr steps: ", "bold", this_proj -> analysis[rdf] -> calc_buffer);
  str = g_strdup_printf ("%d", this_proj -> analysis[rdf] -> num_delta);
  print_info (str, "bold_blue", this_proj -> analysis[rdf] -> calc_buffer);
  g_free (str);
  print_info ("\n\n\t between 0.0 and ", NULL, this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("D", "bold", this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("max", "sub_bold", this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("\n\t where ", NULL, this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("D", "bold", this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("max", "sub_bold", this_proj -> analysis[rdf] -> calc_buffer);
  print_info (" is the maximum distance in real space, ", NULL, this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("D", "bold", this_proj -> analysis[rdf] -> calc_buffer);
  print_info ("max", "sub_bold", this_proj -> analysis[rdf] -> calc_buffer);
  print_info (" = ", NULL, this_proj -> analysis[rdf] -> calc_buffer);
  str = g_strdup_printf ("%f", this_proj -> analysis[rdf] -> max);
  print_info (str, "bold_blue", this_proj -> analysis[rdf] -> calc_buffer);
  g_free (str);
  print_info (" Å\n\n\t - δr = ", "bold", this_proj -> analysis[rdf] -> calc_buffer);
  str = g_strdup_printf ("%f", this_proj -> analysis[rdf] -> delta);
  print_info (str, "bold_blue", this_proj -> analysis[rdf] -> calc_buffer);
  g_free (str);
  print_info (" Å\n", "bold", this_proj -> analysis[rdf] -> calc_buffer);
  print_info (calculation_time(TRUE, this_proj -> analysis[rdf] -> calc_time), NULL, this_proj -> analysis[rdf] -> calc_buffer);
}

/*!
  \fn G_MODULE_EXPORT void on_calc_gr_released (GtkWidget * widg, gpointer data)

  \brief compute g(r)

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_gr_released (GtkWidget * widg, gpointer data)
{
  int i;
  if (! active_project -> analysis[GDR] -> init_ok) initgr (GDR);
  clean_curves_data (GDR, 0, active_project -> analysis[GDR] -> numc);
  active_project -> analysis[GDR] -> delta = active_project -> analysis[GDR] -> max / active_project -> analysis[GDR] -> num_delta;
  prepostcalc (widg, FALSE, GDR, 0, opac);
  i = g_of_r_ (& active_project -> analysis[GDR] -> num_delta, & active_project -> analysis[GDR] -> delta, & fitc);
  prepostcalc (widg, TRUE, GDR, i, 1.0);
  if (! i)
  {
    remove_action ("analyze.1");
    show_error ("The RDF's calculation has failed", 0, widg);
  }
  else
  {
    add_analysis_action (1);
    update_rdf_view (active_project, GDR);
    show_the_widgets (curvetoolbox);
  }
  fill_tool_model ();
  for (i=0; i<4; i=i+3) update_after_calc (i);
}

/*!
  \fn void sendcutoffs_ (int * nc, double * totc, double partc[*nc][*nc])

  \brief bond cutoff from Fortran90

  \param nc number of species
  \param totc total cutoff
  \param partc partials cutoff
*/
void sendcutoffs_ (int * nc, double * totc, double partc[* nc][* nc])
{
  int i, j;

  active_chem -> grtotcutoff = * totc;
  for ( i=0; i< active_project -> nspec; i++)
  {
    for (j=0; j<active_project -> nspec; j++)
    {
      active_chem -> cutoffs[i][j]=partc[j][i];
    }
  }
  active_project -> dmtx = FALSE;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void on_cutcheck_toggled (GtkCheckButton * but, gpointer data)

  \brief Fitting bond cutoff or data ?

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_cutcheck_toggled (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void on_cutcheck_toggled (GtkToggleButton * Button)

  \brief Fitting bond cutoff or data ?

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_cutcheck_toggled (GtkToggleButton * but, gpointer data)
#endif
{
  gboolean status = button_get_status ((GtkWidget *)but);
  if (status)
  {
    fitc = 1;
  }
  else
  {
    fitc = 0;
  }
}

/*!
  \fn int recup_data_ (int * cd, int * rd)

  \brief Sending data back to Fortran90

  \param cd the curve id
  \param rd the analysis id
*/
int recup_data_ (int * cd, int * rd)
{
  if (* rd == 0)
  {
    return send_gr_ (cd,
                     & active_project -> analysis[GDR] -> curves[* cd] -> ndata,
                     & active_project -> analysis[GDR] -> delta,
                     active_project -> analysis[GDR] -> curves[* cd] -> data[0],
                     active_project -> analysis[GDR] -> curves[* cd] -> data[1]);
  }
  else
  {
    return send_sq_ (cd,
                     & active_project -> analysis[SKD] -> curves[* cd] -> ndata,
                     & active_project -> analysis[GDK] -> delta,
                     active_project -> analysis[SKD] -> curves[* cd] -> data[0],
                     active_project -> analysis[SKD] -> curves[* cd] -> data[1]);
  }
}

/*!
  \fn G_MODULE_EXPORT void on_calc_gq_released (GtkWidget * widg, gpointer data)

  \brief compute g(k)

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_gq_released (GtkWidget * widg, gpointer data)
{
  int i;

  if (! active_project -> analysis[GDK] -> init_ok) initgr (GDK);
  clean_curves_data (GDK, 0, active_project -> analysis[GDK] -> numc);
  active_project -> analysis[GDK] -> delta = active_project -> analysis[GDK] -> max / active_project -> analysis[GDK] -> num_delta;
  prepostcalc (widg, FALSE, GDK, 0, opac);
  i = g_of_r_fft_ (& active_project -> analysis[GDK] -> num_delta,
                   & active_project -> analysis[GDK] -> delta,
                   & active_project -> analysis[GDK] -> max);
  prepostcalc (widg, TRUE, GDK, i, 1.0);
  if (! i)
  {
    show_error ("The RDF's from FFT[S(k)] calculation has failed", 0, widg);
  }
  else
  {
    update_rdf_view (active_project, GDK);
    show_the_widgets (curvetoolbox);
  }
  fill_tool_model ();
  for (i=0; i<4; i=i+3) update_after_calc (i);
}
