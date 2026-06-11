/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2026 by CNRS and University of Strasbourg */

/*!
* @file m_quality.c
* @short Functions to create the 'OengGL -> Quality' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_quality.c'
*
* Contains:
*

 - The functions to create the 'OengGL -> Quality' submenu

*
* List of functions:

  void set_quality (int q, glwin * view);

  G_MODULE_EXPORT void set_quality_spin (GtkSpinButton * res, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "preferences.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"

/*!
  \fn void set_quality (int q, glwin * view)

  \brief set OpenGL quality

  \param q new OpenGL quality
  \param view the target glwin
*/
void set_quality (int q, glwin * view)
{
  if (! preferences)
  {
    view -> anim -> last -> img -> quality = q;
    init_default_shaders (view);
    view -> create_shaders[MDBOX] = TRUE;
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
  }
  else
  {
    tmp_opengl[3] = q;
  }
}

/*!
  \fn G_MODULE_EXPORT void set_quality_spin (GtkSpinButton * res, gpointer data)

  \brief set quality spin button callback

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_quality_spin (GtkSpinButton * res, gpointer data)
{
  glwin * view = (glwin *)data;
  set_quality (gtk_spin_button_get_value_as_int(res), view);
  update_entry_int (GTK_ENTRY(res), view -> anim -> last -> img -> quality);
}
