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
* @file ogl_utils.c
* @short 2D and 3D calculations utilities for distances and angles
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'ogl_utils.c'
*
* Contains:
*

 - 2D and 3D calculations utilities for distances and angles

*
* List of functions:

  double arc_cos (double val);

  distance distance_2d (atom * at, atom * bt);
  distance distance_3d (cell_info * cell, int mdstep, atom * at, atom * bt);
  angle angle_2d (atom * at, atom * bt, atom * ct);
  angle angle_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct);
  angle dihedral_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct, atom * dt);
  angle inversion_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct, atom * dt);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "dlp_field.h"

extern gboolean field_color;
extern ColRGBA init_color (int id, int numid);

/*!
  \fn distance distance_2d (atom * at, atom * bt)

  \brief distance between atom a and b in 2D

  \param at atom a
  \param bt atom b
*/
distance distance_2d (atom * at, atom * bt)
{
  distance dist;
  dist.pbc = FALSE;
  dist.x = at -> x - bt -> x;
  dist.y = at -> y - bt -> y;
  dist.z = 0.0;
  dist.length = sqrt(dist.x*dist.x + dist.y*dist.y);
  return dist;
}

/*!
  \fn distance distance_3d (cell_info * cell, int mdstep, atom * at, atom * bt)

  \brief distance between atom a and b in 3D

  \param cell unit cell
  \param mdstep the MD step
  \param at atom a
  \param bt atom b
*/
distance distance_3d (cell_info * cell, int mdstep, atom * at, atom * bt)
{
  distance dist;
  double tmp;
  vec3_t dij;
  dist.pbc = FALSE;
  dist.x = at -> x - bt -> x;
  dist.y = at -> y - bt -> y;
  dist.z = at -> z - bt -> z;
  dist.length = sqrt(dist.x*dist.x + dist.y*dist.y + dist.z*dist.z);
  if (cell -> pbc)
  {
    if (cell -> box[mdstep].param[1][0] == 90.0 && cell -> box[mdstep].param[1][1] == 90.0 && cell -> box[mdstep].param[1][2] == 90.0)
    {
      dij.x = dist.x - round((at -> x-bt -> x)/cell -> box[mdstep].param[0][0]) * cell -> box[mdstep].param[0][0];
      dij.y = dist.y - round((at -> y-bt -> y)/cell -> box[mdstep].param[0][1]) * cell -> box[mdstep].param[0][1];
      dij.z = dist.z - round((at -> z-bt -> z)/cell -> box[mdstep].param[0][2]) * cell -> box[mdstep].param[0][2];
    }
    else
    {
      vec3_t a = vec3(at -> x, at -> y, at -> z);
      vec3_t b = vec3(bt -> x, bt -> y, bt -> z);
      vec3_t af = m4_mul_coord (cell -> box[mdstep].cart_to_frac, a);
      vec3_t bf = m4_mul_coord (cell -> box[mdstep].cart_to_frac, b);
      vec3_t nij = v3_sub(af, bf);
      nij.x -= round(nij.x);
      nij.y -= round(nij.y);
      nij.z -= round(nij.z);
      dij = m4_mul_coord (cell -> box[mdstep].frac_to_cart, nij);
    }
    tmp = v3_length(dij);
    if (dist.length - tmp > 0.001)
    {
      dist.pbc = TRUE;
      dist.x = dij.x;
      dist.y = dij.y;
      dist.z = dij.z;
      dist.length = tmp;
    }
  }
  return dist;
}

/*!
  \fn double arc_cos (double val)

  \brief compute arc cosinus

  \param val the angle
*/
double arc_cos (double val)
{
  if (val < -1.0)
  {
    return acos(-2.0 - val) * 180.0 / pi;
  }
  else if (val > 1.0)
  {
    return acos(2.0 - val) * 180.0 / pi;
  }
  else
  {
    return acos(val) * 180.0 / pi;
  }
}

/*!
  \fn angle angle_2d (atom * at, atom * bt, atom * ct)

  \brief angle between atom a, b and c in 2D

  \param at atom a
  \param bt atom b
  \param ct atom c
*/
angle angle_2d (atom * at, atom * bt, atom * ct)
{
  angle theta;
  distance dist_a = distance_2d (bt, at);
  distance dist_b = distance_2d (bt, ct);
  theta.pbc = FALSE;
  double v = 0.0;
  v = dist_a.x*dist_b.x + dist_a.y*dist_b.y;
  theta.angle = arc_cos(v/(dist_a.length*dist_b.length));
  return theta;
}

/*!
  \fn angle angle_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct)

  \brief angle between atom a, b and c in 3D

  \param cell unit cell
  \param mdstep the MD step
  \param at atom a
  \param bt atom b
  \param ct atom c
*/
angle angle_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct)
{
  angle theta;
  distance dist_a = distance_3d (cell, mdstep, bt, at);
  distance dist_b = distance_3d (cell, mdstep, bt, ct);
  theta.pbc = FALSE;
  if (dist_a.pbc || dist_b.pbc) theta.pbc = TRUE;
  double v = 0.0;
  v = dist_a.x*dist_b.x + dist_a.y*dist_b.y + dist_a.z*dist_b.z;
  theta.angle = arc_cos(v/(dist_a.length*dist_b.length));
  return theta;
}

/*!
  \fn angle dihedral_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct, atom * dt)

  \brief dihedral between atom a, b, c and d in 3D

  \param cell unit cell
  \param mdstep the MD step
  \param at atom a
  \param bt atom b
  \param ct atom c
  \param dt atom d
*/
angle dihedral_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct, atom * dt)
{
  angle phi;
  distance dist_a = distance_3d (cell, mdstep, at, bt);
  distance dist_b = distance_3d (cell, mdstep, bt, ct);
  distance dist_c = distance_3d (cell, mdstep, ct, dt);
  vec3_t u, v;

  if (dist_a.pbc || dist_b.pbc || dist_c.pbc) phi.pbc = TRUE;

  u = vec3(dist_a.y*dist_b.z  - dist_a.z*dist_b.y, dist_a.z*dist_b.x  - dist_a.x*dist_b.z, dist_a.x*dist_b.y  - dist_a.y*dist_b.x);
  v = vec3(dist_b.y*dist_c.z  - dist_b.z*dist_c.y, dist_b.z*dist_c.x  - dist_b.x*dist_c.z, dist_b.x*dist_c.y  - dist_b.y*dist_c.x);

  if (v3_length(u) == 0.0 || v3_length(v) == 0.0)
  {
    phi.angle = 0.0;
  }
  else
  {
    phi.angle = arc_cos(v3_dot(u, v)/(v3_length(u)*v3_length(v)));
  }
  return phi;
}

/*!
  \fn angle inversion_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct, atom * dt)

  \brief inversion angle between atom a, b, c and d in 3D

  \param cell unit cell
  \param mdstep the MD step
  \param at atom a
  \param bt atom b
  \param ct atom c
  \param dt atom d
*/
angle inversion_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct, atom * dt)
{
  angle inv;
  distance dist_a = distance_3d (cell, mdstep, bt, at);
  distance dist_b = distance_3d (cell, mdstep, ct, at);
  distance dist_c = distance_3d (cell, mdstep, at, dt);
  vec3_t u, v, w, x;

  if (dist_a.pbc || dist_b.pbc || dist_c.pbc) inv.pbc = TRUE;

  u = vec3(dist_b.x, dist_b.y, dist_b.z);
  v = vec3(dist_c.x, dist_c.y, dist_c.z);
  w = v3_cross (u, v);
  x = vec3(dist_a.x, dist_a.y, dist_a.z);
  if (v3_length(w) == 0.0 || v3_length(x) == 0.0)
  {
    inv.angle = 0.0;
  }
  else
  {
    inv.angle = fabs(90.0 - arc_cos(v3_dot(w, x)/(v3_length(w)*v3_length(x))));
  }
  return inv;
}
