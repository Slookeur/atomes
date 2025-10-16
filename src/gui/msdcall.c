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
* @file msdcall.c
* @short Callbacks for the MSD calculation dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'msdcall.c'
*
* Contains:
*

 - The callbacks for the MSD calculation dialog

*
* List of functions:

  void initmsd ();
  void update_msd_view (project * this_proj);

  G_MODULE_EXPORT void on_calc_msd_released (GtkWidget * widg, gpointer data);

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

/*!
  \fn void initmsd ()

  \brief initialize the curve widgets for the MSD
*/
void initmsd ()
{
  int i, j;
  j = 0;
#ifdef NEW_ANA

  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(nac)[%s]", active_chem -> label[i]);
    j=j+1;
  }
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(x)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(y)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(z)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(xy)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(xz)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(yz)[%s]", active_chem -> label[i]);
    j=j+1;
  }
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(x/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(y/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(z/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(xy/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(xz/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("MSD(yz/nac)[%s]", active_chem -> label[i]);
    j=j+1;
  }
  active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("Correction[x]");
  j=j+1;
  active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("Correction[y]");
  j=j+1;
  active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("Correction[z]");
  j=j+1;
  active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("Drift[x]");
  j=j+1;
  active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("Drift[y]");
  j=j+1;
  active_project -> analysis[MSD] -> curves[j] -> name = g_strdup_printf ("Drift[z]");

  addcurwidgets (activep, MSD, 0);
  active_project -> analysis[MSD] -> init_ok=TRUE;
#else
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(nac)[%s]", active_chem -> label[i]);
    j=j+1;
  }
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(x)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(y)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(z)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(xy)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(xz)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(yz)[%s]", active_chem -> label[i]);
    j=j+1;
  }
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(x/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(y/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(z/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(xy/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(xz/nac)[%s]", active_chem -> label[i]);
    j=j+1;
    active_project -> curves[MSD][j] -> name = g_strdup_printf ("MSD(yz/nac)[%s]", active_chem -> label[i]);
    j=j+1;
  }
  active_project -> curves[MSD][j] -> name = g_strdup_printf ("Correction[x]");
  j=j+1;
  active_project -> curves[MSD][j] -> name = g_strdup_printf ("Correction[y]");
  j=j+1;
  active_project -> curves[MSD][j] -> name = g_strdup_printf ("Correction[z]");
  j=j+1;
  active_project -> curves[MSD][j] -> name = g_strdup_printf ("Drift[x]");
  j=j+1;
  active_project -> curves[MSD][j] -> name = g_strdup_printf ("Drift[y]");
  j=j+1;
  active_project -> curves[MSD][j] -> name = g_strdup_printf ("Drift[z]");

  addcurwidgets (activep, MSD, 0);
  active_project -> initok[MSD]=TRUE;
#endif
}

/*!
  \fn void update_msd_view (project * this_proj)

  \brief update the project text view for the MSD calculation

  \param this_proj the target project
*/
void update_msd_view (project * this_proj)
{
  gchar * str;
#ifdef NEW_ANA
  if (this_proj -> analysis[MSD] -> calc_buffer == NULL) this_proj -> analysis[MSD] -> calc_buffer = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> analysis[MSD] -> calc_buffer);
  print_info ("\n\nMean Square Displacement\n\n", "heading", this_proj -> analysis[MSD] -> calc_buffer);
  print_info ("Calculation details:\n\n", NULL, this_proj -> analysis[MSD] -> calc_buffer);
  print_info ("\t - Number of configurations: ", "bold", this_proj -> analysis[MSD] -> calc_buffer);
  str = g_strdup_printf ("%d", this_proj -> steps);
  print_info (str, "bold_blue", this_proj -> analysis[MSD] -> calc_buffer);
  g_free (str);
  print_info ("\n\n\t - Number of time steps between each configuration: ", "bold", this_proj -> analysis[MSD] -> calc_buffer);
  str = g_strdup_printf ("%d", this_proj -> analysis[MSD] -> num_delta);
  print_info (str, "bold_blue", this_proj -> analysis[MSD] -> calc_buffer);
  g_free (str);
  print_info ("\n\n\t - Time step δt used to integrate Newton's equations of motion: ", "bold", this_proj -> analysis[MSD] -> calc_buffer);
  str = g_strdup_printf ("%f", this_proj -> analysis[MSD] -> delta);
  print_info (str, "bold_blue", this_proj -> analysis[MSD] -> calc_buffer);
  g_free (str);
  print_info (" ", "bold", this_proj -> analysis[MSD] -> calc_buffer);
  print_info (untime[this_proj -> tunit], "bold_red", this_proj -> analysis[MSD] -> calc_buffer);
  print_info (calculation_time(TRUE, this_proj -> analysis[MSD] -> calc_time), NULL, this_proj -> analysis[MSD] -> calc_buffer);
#else
  if (this_proj -> text_buffer[MS+OT] == NULL) this_proj -> text_buffer[MS+OT] = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> text_buffer[MS+OT]);
  print_info ("\n\nMean Square Displacement\n\n", "heading", this_proj -> text_buffer[MS+OT]);
  print_info ("Calculation details:\n\n", NULL, this_proj -> text_buffer[MS+OT]);
  print_info ("\t - Number of configurations: ", "bold", this_proj -> text_buffer[MS+OT]);
  str = g_strdup_printf ("%d", this_proj -> steps);
  print_info (str, "bold_blue", this_proj -> text_buffer[MS+OT]);
  g_free (str);
  print_info ("\n\n\t - Number of time steps between each configuration: ", "bold", this_proj -> text_buffer[MS+OT]);
  str = g_strdup_printf ("%d", this_proj -> num_delta[MSD]);
  print_info (str, "bold_blue", this_proj -> text_buffer[MS+OT]);
  g_free (str);
  print_info ("\n\n\t - Time step δt used to integrate Newton's equations of motion: ", "bold", this_proj -> text_buffer[MS+OT]);
  str = g_strdup_printf ("%f", this_proj -> delta[MSD]);
  print_info (str, "bold_blue", this_proj -> text_buffer[MS+OT]);
  g_free (str);
  print_info (" ", "bold", this_proj -> text_buffer[MS+OT]);
  print_info (untime[this_proj -> tunit], "bold_red", this_proj -> text_buffer[MS+OT]);
  print_info (calculation_time(TRUE, this_proj -> calc_time[MSD]), NULL, this_proj -> text_buffer[MS+OT]);
#endif
}

/*!
  \fn G_MODULE_EXPORT void on_calc_msd_released (GtkWidget * widg, gpointer data)

  \brief compute MSD

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_msd_released (GtkWidget * widg, gpointer data)
{
  int i;
#ifdef NEW_ANA
  if (! active_project -> analysis[MSD] -> init_ok)  initmsd ();
  clean_curves_data (MSD, 0, active_project -> analysis[MSD] -> numc);
  prepostcalc (widg, FALSE, MSD, 0, opac);
  active_project -> analysis[MSD] -> min = active_project -> analysis[MSD] -> delta*active_project -> analysis[MSD] -> num_delta;
  active_project -> analysis[MSD] -> max = (active_project -> steps -1)*active_project -> analysis[MSD] -> delta*active_project -> analysis[MSD] -> num_delta;
  clock_gettime (CLOCK_MONOTONIC, & start_time);
  i = msd_ (& active_project -> analysis[MSD] -> delta, & active_project -> analysis[MSD] -> num_delta);
  clock_gettime (CLOCK_MONOTONIC, & stop_time);
  active_project -> analysis[MSD] -> calc_time = get_calc_time (start_time, stop_time);
#else
  if (! active_project -> initok[MSD])  initmsd ();
  clean_curves_data (MSD, 0, active_project -> numc[MSD]);
  prepostcalc (widg, FALSE, MSD, 0, opac);
  active_project -> min[MSD] = active_project -> delta[MSD]*active_project -> num_delta[MSD];
  active_project -> max[MSD] = (active_project -> steps -1)*active_project -> delta[MSD]*active_project -> num_delta[MSD];
  clock_gettime (CLOCK_MONOTONIC, & start_time);
  i = msd_ (& active_project -> delta[MSD], & active_project -> num_delta[MSD]);
  clock_gettime (CLOCK_MONOTONIC, & stop_time);
  active_project -> calc_time[MSD] = get_calc_time (start_time, stop_time);
#endif
  prepostcalc (widg, TRUE, MSD, i, 1.0);
  if (! i)
  {
    show_error ("The MSD calculation has failed", 0, widg);
  }
  else
  {
    update_msd_view (active_project);
    show_the_widgets (curvetoolbox);
  }
  fill_tool_model ();
}
