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
* @file d_background.c
* @short Functions to prepare the OpenGL rendering for the gradient background

* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'd_background.c'
*
* Contains:
*

 - The functions to prepare the OpenGL gradient background

*
* List of functions:

  void create_background_lists ();

*/

#include "global.h"
#include "glview.h"

/*!
  \fn void create_background_lists ()

  \brief prepare background OpenGL rendering
*/
void create_background_lists ()
{
  cleaning_shaders (wingl, BACKG);
  wingl -> create_shaders[BACKG] = FALSE;

  wingl -> n_shaders[BACKG][0] = 1;
  object_3d * back = g_malloc0 (sizeof*back);
  back -> vert_buffer_size = 2;
  back -> num_vertices = 4;
  back -> vertices = allocfloat (back -> vert_buffer_size*back -> num_vertices);
  back -> vertices[0] = back -> vertices[1] = back -> vertices[3] = back -> vertices[4] = -1.0;
  back -> vertices[2] = back -> vertices[5] = back -> vertices[6] = back -> vertices[7] = 1.0;
  wingl -> ogl_glsl[BACKG][0] = g_malloc0 (sizeof*wingl -> ogl_glsl[BACKG][0]);
  switch (plot -> back -> gradient)
  {
    case 1:
      wingl -> ogl_glsl[BACKG][0][0] = init_shader_program (BACKG, GLSL_BACK, background_vertex, NULL, background_linear, GL_TRIANGLE_STRIP, 1, 5, FALSE, back);
      break;
    case 2:
      wingl -> ogl_glsl[BACKG][0][0] = init_shader_program (BACKG, GLSL_BACK, background_vertex, NULL, background_circular, GL_TRIANGLE_STRIP, 1, 5, FALSE, back);
      break;
  }
  g_free (back);
}
