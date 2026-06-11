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
* @file arcball.c
* @short Mouse trackball rotation functions
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'arcball.c'
*
* Contains:
*

 - The mouse trackball rotation functions

*
* List of functions:

  void arc_ball_rotation (glwin * view, int x, int y);

*/

#include "global.h"
#include "glview.h"

extern void rotate_quat (project * this_proj, vec4_t q, int status, int axis);

vec3_t arc_ball_init;
vec3_t arc_ball_new;
vec4_t old_rotation_quaternion;

vec3_t get_arc_ball_vector (glwin * view, int x, int y)
{
  vec3_t vect;
  vect.x = 2.0*x/view -> pixels[0] - 1.0;
  y = view -> pixels[1] - y;
  vect.y = 2.0*y/view -> pixels[1] - 1.0;
  float norm_arc = vect.x*vect.x + vect.y*vect.y;
  if (norm_arc > 1.0)
  {
    vect.z = 0.0;
  }
  else
  {
    vect.z = sqrt(1.0 - norm_arc);
  }
  return v3_norm (vect);
}

/*!
  \fn void arc_ball_rotation (glwin * view, int x, int y)

  \brief Perform arcball rotation

  \param view the target glwin
  \param x x position
  \param y y position
*/
void arc_ball_rotation (glwin * view, int x, int y)
{
  arc_ball_new = get_arc_ball_vector (view, x, y);
  vec3_t rot_axis = v3_cross (arc_ball_init, arc_ball_new);
  if (v3_length(rot_axis) > 0.0)
  {
    double rot_angle = - acos(v3_dot (arc_ball_init, arc_ball_new));
    vec4_t quat = axis_to_quat (rot_axis, rot_angle);
    if (view -> mode != EDITION)
    {
      view -> anim -> last -> img -> rotation_quaternion = q4_mul (old_rotation_quaternion, quat);
    }
    else
    {
      rotate_quat (get_project_by_id(view -> proj), quat, 1, 1);
    }
  }
}
