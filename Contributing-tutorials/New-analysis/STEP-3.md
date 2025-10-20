# Adding the new analysis using the **atomes** software

## 1. Create a new file to implement the calculation

  - The new file must be located in the [`src/gui`][gui] directory
  - The new file must be named **analysis**call.c, in the following `idccall.c`
  - The new file must contain the callback for the analysis, what happens when the user click on `Apply` in the calculation dialog. 
  - The new file can contain the analysis it-self

## 2. Writting the calculation callbacks in the file `analysiscall.c`

```C
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
* @short Callbacks for the IDC calculation dialog
* @author Your name <your@email.com>
*/

/*
* This file: 'idccall.c'
*
* Contains:
*

 - The callbacks for the IDC calculation dialog

*
* List of functions:

  void init_idc ();
  void update_idc_view (project * this_proj);

  G_MODULE_EXPORT void on_calc_idc_released (GtkWidget * widg, gpointer data);

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
  \fn void init_idc ()

  \brief initialize the curve widgets for the IDC analysis
*/
void init_idc ()
{
  int i;
  // In this example a result is described for each chemical species.
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    active_project -> analysis[IDC] -> curves[i] -> name = g_strdup_printf ("IDC[%s]", active_chem -> label[i]);
  }
  // The total number of curves declare in this function
  // should be equal to active_project -> analysis[IDC] -> numc
  addcurwidgets (activep, IDC, 0);
  active_project -> analysis[IDC] -> init_ok = TRUE;
}

/*!
  \fn void update_idc_view (project * this_proj)

  \brief update the project text view for the IDC calculation

  \param this_proj the target project
*/
void update_idc_view (project * this_proj)
{
  gchar * str;
  if (this_proj -> analysis[IDC] -> calc_buffer == NULL)
  {
    // If this text buffer does not exist, then initialize it
    this_proj -> analysis[IDC] -> calc_buffer = add_buffer (NULL, NULL, NULL);
  } 
  // Display the text buffer for the IDC calculation
  view_buffer (this_proj -> analysis[IDC] -> calc_buffer);
  print_info ("\n\nThis is the new IDC analysis\n\n", "heading", this_proj -> analysis[IDC] -> calc_buffer);
  print_info ("Calculation details:\n\n", NULL, this_proj -> analysis[IDC] -> calc_buffer);
  
  print_info ("\n\n\t - Number of steps in IDC: ", "bold", this_proj -> analysis[IDC] -> calc_buffer);
  str = g_strdup_printf ("%d", this_proj -> analysis[IDC] -> num_delta);
  print_info (str, "bold_blue", this_proj -> analysis[IDC] -> calc_buffer);
  g_free (str);
  print_info ("\n\n\t - Step δ used in IDC: ", "bold", this_proj -> analysis[IDC] -> calc_buffer);
  str = g_strdup_printf ("%f", this_proj -> analysis[IDC] -> delta);
  print_info (str, "bold_blue", this_proj -> analysis[IDC] -> calc_buffer);
  g_free (str);

  str = calculation_time (TRUE, this_proj -> analysis[IDC] -> calc_time);
  print_info (str, NULL, this_proj -> analysis[IDC] -> calc_buffer);
  g_free (str);
}

/*!
  \fn G_MODULE_EXPORT void on_calc_idc_released (GtkWidget * widg, gpointer data)

  \brief compute the new IDC analysis

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_idc_released (GtkWidget * widg, gpointer data)
{
  // Initializing the graph for this calculation, if this was done already
  if (! active_project -> analysis[IDC] -> init_ok)  init_idc ();
  // Cleaning previous results, if any
  clean_curves_data (IDC, 0, active_project -> analysis[IDC] -> numc);
  prepostcalc (widg, FALSE, IDC, 0, opac);
  clock_gettime (CLOCK_MONOTONIC, & start_time);
  int i = calc_idc ( ); // The calculation is perfomed here, getting back and integer value as status (1 = ok, 0 = bad)
  clock_gettime (CLOCK_MONOTONIC, & stop_time);  
  active_project -> analysis[IDC] -> calc_time = get_calc_time (start_time, stop_time);
  prepostcalc (widg, TRUE, IDC, i, 1.0);
  if (! i)
  {
    show_error ("The IDC calculation has failed", 0, widg);
  }
  else
  {
    update_idc_view (active_project);
    show_the_widgets (curvetoolbox);
  }
  fill_tool_model ();
}

```

## 3. Implementing the new analysis

I would recommend to put the analysis, and therefore the implementation of the `calc_idc` function in a separate file to simply its reading. 
But that is ultimately up to you to decide. 

Please give a look to the **atomes** [project data structure][atomes_project] to know how to retrieve the elements you might need for your calculation, 
at this point you only need to call the element of the active project `active_project`. 

 >[!IMPORTANT]
 > To ensure for the implementation in **atomes** to be as fast as possible,  
 > I am asking you, if not done yet, to parallelize the analysis in OpenMP. 
 > Do not hesitate to ask for my help in the process.

[gui]:https://slookeur.github.io/atomes-doxygen/dir_11bc0974ce736ce9a6fadebbeb7a8314.html
[atomes_project]:https://slookeur.github.io/atomes-doxygen/dd/dbe/structproject.html
