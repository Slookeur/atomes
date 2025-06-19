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
* @file preferences.c
* @short Functions to create the 'User preferences' window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'configuration.c'
*
* Contains:
*

 - The GUI for the general configuration window
 - The associated controlers

*
* List of functions:



*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"


/*!
  \fn void create_configuration_dialog ()

  \brief create the configuration window
*/
void create_user_preferences_dialog ()
{
  // General prefs
  /*
  - default radius to be used
  - default X-ray scattering method

  - calculations:
    - default dr for g(r)
    - dq for s(q)
    - dk for s(k)
    - dr for g(r)

    - default dr for distances
    - default d° for angles
    - default search for molecules and frag
    - default output in file + file name

    - default ring definition to be used
    - default atom to start the search (all or
    - default max ring size
    - default NUMA ring
    - default search ABAB, no homo in rings, no homo in matrix

    - default max chain size
    - default NUMA chain
    - default only AAAA, ABAB, no homo in chains,

    - delta t for dynamics + unit
    - Steps between configurations
  */
  // OpenGL prefs
  /*

  - Default style
  - Default color scheme: atoms, poly
  - Default quality
  - Default lightning model
  - Default material (or template) : with all parameters
  - Default number of light sources, light types
  - Default fog mode and other options
  */

  // Model prefs

  /*
  - Atoms / Bonds, etc
  -
  - Default show clones
  - Default show box and box option
  */

  // Representation prefs

  /* Ortho / persp

  */

  gchar * str;
  GtkWidget * win = create_win ("User preferences", view -> win, FALSE, FALSE);
  GtkWidget * vbox = create_vbox (5);
  add_container_child (CONTAINER_WIN, win, vbox);
  gtk_widget_set_size_request (win, 625, 600);
  GtkWidget * notebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
  show_the_widgets (notebook);
  gtk_widget_set_size_request (notebook, 600, 550);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, notebook, FALSE, FALSE, 0);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), calc_preferences(), gtk_label_new ("Analysis"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), opengl_preferences(), gtk_label_new ("OpenGL"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), model_preferences(), gtk_label_new ("Model"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), rep_preferences(), gtk_label_new ("Representation"));

}
