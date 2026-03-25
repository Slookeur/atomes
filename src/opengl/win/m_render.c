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
* @file m_render.c
* @short Functions to create the 'OpenGL -> Render' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_render.c'
*
* Contains:
*

 - The functions to create the 'OpenGL -> Render' submenu

*
* List of functions:

  void set_render (gpointer data);

*/

#include "global.h"
#include "glview.h"
#include "glwindow.h"

gchar * text_renders[OGL_RENDERS] = {"Filled", "Lines", "Points"};

/*!
  \fn void set_render (gpointer data)

  \brief set OpenGL rendering mode callback

  \param data the associated data pointer
*/
void set_render (gpointer data)
{
  tint * the_data = (tint *)data;
  project * this_proj = get_project_by_id(the_data -> a);
  int i = this_proj -> modelgl -> anim -> last -> img -> render;
  int j = the_data -> b;
  if (i != j)
  {
    this_proj -> modelgl -> anim -> last -> img -> render = NONE;
    this_proj -> modelgl -> anim -> last -> img -> render = j;
    this_proj -> modelgl -> create_shaders[MAXIS] = TRUE;
    update (this_proj -> modelgl);
  }
}
