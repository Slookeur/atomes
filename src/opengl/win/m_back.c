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
* @file m_back.c
* @short Function to create the background color menu item
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_back.c'
*
* Contains:
*

 - The function to create the background color menu item

*
* List of functions:

  GtkWidget * menu_back (glwin * view);

  GMenu * menu_back (glwin * view, int popm);

*/

#include "global.h"
#include "color_box.h"
#include "glwindow.h"

extern G_MODULE_EXPORT void gradient_advanced (GtkWidget * widg, gpointer data);

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void to_gradient_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief to run background gradient color selection window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_gradient_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  gradient_advanced (NULL, data);
}
#endif

#ifdef GTK3
/*!
  \fn GtkWidget * menu_back (glwin * view)

  \brief create the background color menu item GTK3

  \param view the target glwin
*/
GtkWidget * menu_back (glwin * view)
{
  GtkWidget * menub = gtk_menu_new ();
  GtkWidget * bc = create_menu_item (FALSE, "Color");
  gtk_menu_shell_append ((GtkMenuShell *)menub, bc);
  gtk_menu_item_set_submenu ((GtkMenuItem *)bc, color_box(view, -2, 0, 0));
  GtkWidget * gc = create_menu_item (FALSE, "Gradient color");
  g_signal_connect (G_OBJECT (gc), "activate", G_CALLBACK(gradient_advanced), view);
  gtk_menu_shell_append ((GtkMenuShell *)menub, gc);
  return menub;
}
#else
/*!
  \fn GMenu * menu_back (glwin * view, int popm)

  \brief create the background color menu item GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_back (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, "back-color", "back-color", popm, popm, NULL, IMG_NONE, NULL, TRUE, NULL, NULL, FALSE, FALSE, FALSE, FALSE);
  append_opengl_item (view, menu, "More colors ...", "back-color", popm, popm, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_run_back_color_window), view, FALSE, FALSE, FALSE, TRUE);
  append_opengl_item (view, menu, "Gradient colors", "back-gradient", popm, popm, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_gradient_advanced), view, FALSE, FALSE, FALSE, TRUE);
  return menu;
}
#endif
