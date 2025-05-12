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
* @file cbuild_action.c
* @short Functions to build a crystal using space group, crystallographic position(s) and object(s) to insert
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cbuild_action.c'
*
* Contains:
*

 - The functions to build a crystal using space group, crystallographic position(s) and object(s) to insert

*
* List of functions:

  int test_lattice (builder_edition * cbuilder, cell_info * cif_cell);
  int pos_not_saved (vec3_t * all_pos, int num_pos, vec3_t pos);
  int build_crystal (gboolean visible, project * this_proj, gboolean to_wrap, gboolean show_clones, cell_info * cell, GtkWidget * widg);

  double get_val_from_setting (gchar * pos, gchar * sval);
  double get_value_from_pos (gchar * pos);
  double get_val_from_wyckoff (gchar * pos, gchar * wval);

  gboolean same_coords (float a, float b);
  gboolean are_equal_vectors (vec3_t u, vec3_t v);
  gboolean pos_not_taken (int pos, int dim, int * tab);
  gboolean adjust_object_occupancy (crystal_data * cryst, int occupying, int tot_cell);

  void get_origin (space_group * spg);
  void compute_lattice_properties (cell_info * cell);
  void clean_this_proj (project * this_proj, gboolean newp);

  space_group * duplicate_space_group (space_group * spg);

  crystal_data * allocate_crystal_data (int objects, int species);
  crystal_data * free_crystal_data (crystal_data * cryst);

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "glview.h"
#include "bind.h"
#include "project.h"
#include "workspace.h"
#include "atom_edit.h"
#include "cbuild_edit.h"
#include "readers.h"
#include <ctype.h>

extern int get_crystal_id (int spg);
extern atomic_object * cif_object;

gchar * tmp_pos = NULL;

/*!
  \fn double get_val_from_setting (gchar * pos, gchar * sval)

  \brief get value from space group setting

  \param pos the position "a", "b" or "c"
  \param sval the space group setting
*/
double get_val_from_setting (gchar * pos, gchar * sval)
{
  if (! strstr(sval, pos))
  {
    return 0.0;
  }
  else
  {
    if (! g_strcmp0(sval, pos))
    {
      g_free (tmp_pos);
      tmp_pos = NULL;
      return 1.0;
    }
    gchar * sym[8]={"-1/3", "+1/3", "1/3", "-2/3", "+2/3", "2/3", "-", "+"};
    double vals[8]={-1.0/3.0, 1.0/3.0, 1.0/3.0, -2.0/3.0, 2.0/3.0, 2.0/3.0, -1.0, 1.0};
    gchar * ksym = NULL;
    int i;
    for (i=0; i<8; i++)
    {
      ksym = g_strdup_printf ("%s%s", sym[i], pos);
      if (strstr(sval, ksym))
      {
        tmp_pos = g_strdup_printf ("%s", replace_markup (tmp_pos, ksym, NULL));
        g_free (ksym);
        ksym = NULL;
        if (! strlen(tmp_pos))
        {
          g_free (tmp_pos);
          tmp_pos = NULL;
        }
        return vals[i];
      }
      g_free (ksym);
      ksym = NULL;
    }
    tmp_pos = g_strdup_printf ("%s", replace_markup (tmp_pos, pos, NULL));
    if (! strlen(tmp_pos))
    {
      g_free (tmp_pos);
      tmp_pos = NULL;
    }
    return 1.0;
  }
}

/*!
  \fn double get_value_from_pos (gchar * pos)

  \brief get position double value from string description

  \param pos the string description
*/
double get_value_from_pos (gchar * pos)
{
  if (strstr(pos, "/"))
  {
    char * p = NULL;
    double u, v;
    p = strtok(pos, "/");
    u  = string_to_double ((gpointer)p);
    p = strtok(NULL, "/");
    v = string_to_double ((gpointer)p);
    return u/v;
  }
  else
  {
    return string_to_double ((gpointer)pos);
  }
}

/*!
  \fn void get_origin (space_group * spg)

  \brief get space group origin matrices

  \param spg the target space group
*/
void get_origin (space_group * spg)
{
  char * vinit[3]={"a", "b", "c"};
  double spinit[3][4];
  int i, j, k;
  i = spg -> sid;
  for (j=0; j<3; j++)
  {
    tmp_pos = g_strdup_printf ("%s", spg -> settings[i].pos[j]);
    for (k=0; k<3; k++)
    {
      spinit[j][k] = get_val_from_setting (vinit[k], spg -> settings[i].pos[j]);
    }
    if (tmp_pos)
    {
      spinit[j][3] = get_value_from_pos (tmp_pos);
      g_free (tmp_pos);
      tmp_pos = NULL;
    }
    else
    {
      spinit[j][3] = 0.0;
    }
  }
  spg -> coord_origin = mat4 (spinit[0][0], spinit[1][0], spinit[2][0], 0.0,
                              spinit[0][1], spinit[1][1], spinit[2][1], 0.0,
                              spinit[0][2], spinit[1][2], spinit[2][2], 0.0,
                              0.0, 0.0, 0.0, 1.0);
  spg -> wyck_origin = m4_invert_affine (spg -> coord_origin);
  spg -> coord_origin.m30 = spg -> wyck_origin.m30 = spinit[0][3];
  spg -> coord_origin.m31 = spg -> wyck_origin.m31 = spinit[1][3];
  spg -> coord_origin.m32 = spg -> wyck_origin.m32 = spinit[2][3];
#ifdef DEBUG
  g_debug ("Coord origin:");
  m4_print (spg -> coord_origin);
  g_debug ("Wyck origin:");
  m4_print (spg -> wyck_origin);
#endif
}

/*!
  \fn void compute_lattice_properties (cell_info * cell)

  \brief compute lattice parameters following cell description

  \param cell the target cell description
*/
void compute_lattice_properties (cell_info * cell)
{
  int i;
  box_info * box = & cell -> box[0];
  double ltemp;
  double angle[3];
  double sangle[3], cangle[3];

  if (! cell -> ltype)
  {
    for (i=0; i<3; i++)
    {
      if (box -> param[1][i] == 90.0)
      {
        angle[i] = pi/2.0;
        sangle[i] = 1.0;
        cangle[i] = 0.0;
      }
      else
      {
        angle[i] = box -> param[1][i]*pi/180.0;
        sangle[i] = sin(angle[i]);
        cangle[i] = cos(angle[i]);
      }
    }
    box -> vect[0][0] = box -> param[0][0];
    box -> vect[0][1] = box -> vect[0][2] = 0.0;
    box -> vect[1][0] = box -> param[0][1] * cangle[2];
    box -> vect[1][1] = box -> param[0][1] * sangle[2];
    box -> vect[1][2] = 0.0;
    box -> vect[2][0] = box -> param[0][2] * cangle[1];
    ltemp = (cangle[0] - cangle[1]*cangle[2]) / sangle[2];
    box -> vect[2][1] = box -> param[0][2] * ltemp;
    box -> vect[2][2] = box -> param[0][2] * sqrt(sangle[1]*sangle[1] - ltemp*ltemp);
  }
  else
  {
    for (i=0; i<3; i++) box -> param[0][i] = v3_length(vec3(box -> vect[i][0], box -> vect[i][1], box -> vect[i][2]));
    box -> param[1][0] = (box -> vect[2][0]*box -> vect[1][0] + box -> vect[2][1]*box -> vect[1][1] + box -> vect[2][2]*box -> vect[1][2]);
    box -> param[1][0] /= (box -> param[0][1] * box -> param[0][2]);
    box -> param[1][1] = (box -> vect[0][0]*box -> vect[2][0] + box -> vect[0][1]*box -> vect[2][1] + box -> vect[0][2]*box -> vect[2][2]);
    box -> param[1][1] /= (box -> param[0][0] * box -> param[0][2]);
    box -> param[1][2] = (box -> vect[0][0]*box -> vect[1][0] + box -> vect[0][1]*box -> vect[1][1] + box -> vect[0][2]*box -> vect[1][2]);
    box -> param[1][2] /= (box -> param[0][0] * box -> param[0][1]);

    for (i=0; i<3; i++)
    {
      if (box -> param[1][i] == 0.0)
      {
        box -> param[1][i] = 90.0;
        sangle[i] = 1.0;
        cangle[i] = 0.0;
      }
      else
      {
        angle[i] = acos(box -> param[1][i]);
        sangle[i] = sin(angle[i]);
        cangle[i] = cos(angle[i]);
        box -> param[1][i] = angle[i]*180.0/pi;
      }
    }
  }

#ifdef DEBUG
  g_debug ("     a= %f,     b= %f,     c= %f", box -> param[0][0], box -> param[0][1], box -> param[0][2]);
  g_debug (" alpha= %f,  beta= %f, gamma= %f", box -> param[1][0], box -> param[1][1], box -> param[1][2]);
  g_debug ("   a.x= %f,   a.y= %f,   a.z= %f", box -> vect[0][0], box -> vect[0][1], box -> vect[0][2]);
  g_debug ("   b.x= %f,   b.y= %f,   b.z= %f", box -> vect[1][0], box -> vect[1][1], box -> vect[1][2]);
  g_debug ("   c.x= %f,   c.y= %f,   c.z= %f", box -> vect[2][0], box -> vect[2][1], box -> vect[2][2]);
#endif
  box -> vol = (box -> vect[0][1]*box -> vect[1][2] - box -> vect[0][2]*box -> vect[1][1])*box -> vect[2][0];
  box -> vol += (box -> vect[0][2]*box -> vect[1][0] - box -> vect[0][0]*box -> vect[1][2])*box -> vect[2][1];
  box -> vol += (box -> vect[0][0]*box -> vect[1][1] - box -> vect[0][1]*box -> vect[1][0])*box -> vect[2][2];
  box -> vol = fabs(box -> vol);
  cell -> volume = box -> vol;
#ifdef DEBUG
  g_debug ("Lattice volume= %f", box -> vol);
#endif
  double z = sqrt(fabs(1 - cangle[0]*cangle[0]-cangle[1]*cangle[1]-cangle[2]*cangle[2] + 2*cangle[0]*cangle[1]*cangle[2]));

  float ** ftc;
  ftc = allocdfloat (4,4);
  ftc[0][0]=box -> param[0][0];
  ftc[0][1]=box -> param[0][1]*cangle[2];
  ftc[0][2]=box -> param[0][2]*cangle[1];
  ftc[1][1]=box -> param[0][1]*sangle[2];
  ftc[1][2]=box -> param[0][2]*((cangle[0]-cangle[1]*cangle[2])/sangle[2]);
  ftc[2][2]=box -> param[0][2]*z/sangle[2];
  box -> frac_to_cart = mat4(ftc[0][0], ftc[0][1], ftc[0][2], ftc[0][3],
                             ftc[1][0], ftc[1][1], ftc[1][2], ftc[1][3],
                             ftc[2][0], ftc[2][1], ftc[2][2], ftc[2][3],
                             ftc[3][0], ftc[3][1], ftc[3][2], ftc[3][3]);
  ftc[0][0]=1.0/box -> frac_to_cart.m00;
  ftc[0][1]=-cangle[2]/(sangle[2]*box -> param[0][0]);
  ftc[0][2]=box -> param[0][1]*box -> param[0][2]/box -> vol;
  ftc[0][2]=ftc[0][2]*(cangle[0]*cangle[2] - cangle[1])/sangle[0];
  ftc[1][1]=1.0/box -> frac_to_cart.m11;
  ftc[1][2]=(box -> param[0][0]*box -> param[0][2])/box -> vol;
  ftc[1][2]=ftc[1][2]*(cangle[1]*cangle[2] - cangle[0])/sangle[2];
  ftc[2][2]=1.0/box -> frac_to_cart.m22;
  box -> cart_to_frac = mat4 (ftc[0][0], ftc[0][1], ftc[0][2], ftc[0][3],
                              ftc[1][0], ftc[1][1], ftc[1][2], ftc[1][3],
                              ftc[2][0], ftc[2][1], ftc[2][2], ftc[2][3],
                              ftc[3][0], ftc[3][1], ftc[3][2], ftc[3][3]);
  g_free (ftc);
#ifdef DEBUG
  m4_print (box -> frac_to_cart);
  m4_print (box -> cart_to_frac);
#endif
}

/*!
  \fn int test_lattice (builder_edition * cbuilder, cell_info * cif_cell)

  \brief test lattice parameters

  \param cbuilder the builder edition with the lattice parameters
  \param cif_cell the cell information when testing CIF file
*/
int test_lattice (builder_edition * cbuilder, cell_info * cif_cell)
{
  int i, j;
  cell_info * cell = (cbuilder) ? & cbuilder -> cell : cif_cell;
  box_info * box = & cell -> box[0];
  i = cell -> sp_group -> id;
  j = get_crystal_id (i);

  if (cbuilder)
  {
    if (! cell -> ltype)
    {
      // Adjust a,b,c,alpha,beta,gamma and compute vectors
      if (j == 3 || j == 4 || j == 6)
      {
        box -> param[1][1] = box -> param[1][2] = box -> param[1][0];
      }
      if (j == 3 || (j == 4 && cell -> sp_group -> name[0] == 'P') || j == 5)
      {
        box -> param[0][1] = box -> param[0][0];
      }
      else if ((j == 4 && cell -> sp_group -> name[0] == 'R') || j == 6)
      {
        box -> param[0][1] = box -> param[0][2] = box -> param[0][0];
      }
    }
    if (! test_vol(box -> param, box -> vect))
    {
      show_warning ("Please describe properly the lattice parameters", cbuilder -> win);
      return 0;
    }
    compute_lattice_properties (cell);
  }

  // Strictly different or possibly different ?
  /*if (j < 3)
  {
    if (box -> param[0][0] == box -> param[0][1] || box -> param[0][0] == box -> param[0][2] || box -> param[0][1] == box -> param[0][2])
    {
      // Box error: a, b, c not all different
      return 0;
    }
  }*/

  if (j == 3 || j == 4 || j == 5)
  {
    if (box -> param[0][0] != box -> param[0][1])
    {
      // Box error: a and b must be equal
      return 0;
    }
  }
  if (j == 2 || j == 3 || j == 6)
  {
    if (box -> param[1][0] != 90.0 || box -> param[1][1] != 90.0 || box -> param[1][2] != 90.0)
    {
      // Angle error: alpha, beta and gamma must be = 90
      return 0;
    }
  }

  switch (j)
  {
    // Strictly different or possibly different ?
    /*
    case 0:
      if (! (box -> param[1][0] != box -> param[1][1] && box -> param[1][0] != box -> param[1][2] && box -> param[1][1] != box -> param[1][2]))
      {
        // Angle error: alpha, beta, gamma not all different
       return 0;
      }
      break;
    */
    case 1:
      if (box -> param[1][0] != 90.0)
      {
        // Angle error: alpha must be = 90
        return 0;
      }
      else if (box -> param[1][1] != 90.0 && box -> param[1][2] != 90.0)
      {
        // Angle error: beta or gamma must be = 90
        return 0;
      }
      break;
    case 4:
      if (cell -> sp_group -> name[0] != 'R')
      {
        if (box -> param[1][0] != 90.0 || box -> param[1][1] != 90.0 || box -> param[1][2] != 120.0)
        {
          // Angle error: alpha and beta must be equal= 90, gamma must be = 120
          return 0;
        }
      }
      else
      {
        if (box -> param[1][0] != box -> param[1][1] || box -> param[1][0] != box -> param[1][2] || box -> param[1][1] != box -> param[1][2])
        {
          // Angle error: alpha, beta, gamma must all be equal
          return 0;
        }
        else if (box -> param[0][0] != box -> param[0][2])
        {
          // Box error: a, b and c must all be equal
          return 0;
        }
      }
      break;
    case 5:
      if (box -> param[1][0] != 90.0 || box -> param[1][1] != 90.0 || box -> param[1][2] != 120.0)
      {
        // Angle error: alpha and beta must be = 90, gamma must be = 120
        return 0;
      }
      break;
    case 6:
      if (box -> param[0][0] != box -> param[0][1] || box -> param[0][0] != box -> param[0][2] || box -> param[0][1] != box -> param[0][2])
      {
        // Box error: a, b, c not all equal
        return 0;
      }
      break;
  }
  return 1;
}

/*!
  \fn double get_val_from_wyckoff (gchar * pos, gchar * wval)

  \brief get point value from wyckoff position

  \param pos "x", "y" or "z"
  \param wval wyckoff position vector
*/
double get_val_from_wyckoff (gchar * pos, gchar * wval)
{
  if (! strstr(wval, pos))
  {
    return 0.0;
  }
  else
  {
    if (! g_strcmp0(wval, pos))
    {
      g_free (tmp_pos);
      tmp_pos = NULL;
      return 1.0;
    }
    gchar * sym[4]={"-2", "2", "-", "+"};
    double sval[4]={-2.0, 2.0, -1.0, 1.0};
    gchar * ksym = NULL;
    int i;
    for (i=0; i<4; i++)
    {
      ksym = g_strdup_printf ("%s%s", sym[i], pos);
      if (strstr(wval, ksym))
      {
        tmp_pos = g_strdup_printf ("%s", replace_markup (tmp_pos, ksym, NULL));
        g_free (ksym);
        ksym = NULL;
        if (! strlen(tmp_pos))
        {
          g_free (tmp_pos);
          tmp_pos = NULL;
        }
        return sval[i];
      }
      g_free (ksym);
      ksym = NULL;
    }
    tmp_pos = g_strdup_printf ("%s", replace_markup (tmp_pos, pos, NULL));
    if (! strlen(tmp_pos))
    {
      g_free (tmp_pos);
      tmp_pos = NULL;
    }
    return 1.0;
  }
}

/*!
  \fn void clean_this_proj (project * this_proj, gboolean newp)

  \brief clean project and/or associated cell parameters

  \param this_proj the target project
  \param newp is this a new project ?
*/
void clean_this_proj (project * this_proj, gboolean newp)
{
  int i, j;
  if (newp)
  {
    close_project(this_proj);
  }
  else
  {
    for (i=0; i<3; i++)
    {
      for (j=0; j<3; j++)
      {
        if (i < 2) this_proj -> cell.box[0].param[i][j] = 0.0;
       this_proj -> cell.box[0].vect[i][j] = 0.0;
      }
    }
    this_proj -> cell.ltype = 0;
    this_proj -> cell.pbc = FALSE;
  }
}

/*!
  \fn gboolean same_coords (float a, float b)

  \brief test if values are similar, allowing a 0.0001 difference

  \param a 1st value
  \param b 2nd value
*/
gboolean same_coords (float a, float b)
{
  int i;
  for (i=0; i<10; i++) if (fabs(fabs(a-b) - i) < 0.0001) return TRUE;
  return FALSE;
}

/*!
  \fn gboolean are_equal_vectors (vec3_t u, vec3_t v)

  \brief comparing atomic coordinates vectors

  \param u 1st vector
  \param v 2nd vector
*/
gboolean are_equal_vectors (vec3_t u, vec3_t v)
{
  if (same_coords(u.x, v.x) && same_coords(u.y, v.y) && same_coords(u.z, v.z))
  {
    return TRUE;
  }
  return FALSE;
}

/*!
  \fn int pos_not_saved (vec3_t * all_pos, int num_pos, vec3_t pos)

  \brief was this position already saved ?

  \param all_pos the list of saved atomic coordinates
  \param num_pos the number of saved atomic coordinates
  \param pos the vector to test
*/
int pos_not_saved (vec3_t * all_pos, int num_pos, vec3_t pos)
{
  int i;
  for (i=0; i<num_pos; i++)
  {
    if (are_equal_vectors(all_pos[i], pos)) return -(i+1);
  }
  return 1;
}

/*!
  \fn space_group * duplicate_space_group (space_group * spg)

  \brief duplicate space ground information

  \param spg the space group to duplicate
*/
space_group * duplicate_space_group (space_group * spg)
{
  space_group * new_spg = g_malloc0(sizeof*new_spg);
  new_spg -> id = spg -> id;
  new_spg -> hms = g_strdup_printf ("%s", spg -> hms);
  new_spg -> bravais = g_strdup_printf ("%s", spg -> bravais);
  if (spg -> settings[spg -> sid].origin)
  {
    new_spg -> setting = g_strdup_printf ("%s:%d", spg -> settings[spg -> sid].name, spg -> settings[spg -> sid].origin);
  }
  else
  {
    new_spg -> setting = g_strdup_printf ("%s", spg -> settings[spg -> sid].name);
  }
  return new_spg;
}

/*!
  \fn crystal_data * allocate_crystal_data (int objects, int species)

  \brief allocate crystal data pointer

  \param objects the number of object(s)
  \param species the number of chemical species
*/
crystal_data * allocate_crystal_data (int objects, int species)
{
  crystal_data * cryst = g_malloc0(sizeof*cryst);
  cryst -> objects = objects;
  cryst -> spec = species;
  cryst -> at_by_object = allocint(cryst -> objects);
  cryst -> pos_by_object = allocint(cryst -> objects);
  cryst -> occupancy = allocdouble(cryst -> objects);
  cryst -> sites = g_malloc0(cryst -> objects*sizeof*cryst -> sites);
  cryst -> insert = g_malloc0(cryst -> objects*sizeof*cryst -> insert);
  cryst -> position = g_malloc0(cryst -> objects*sizeof*cryst -> position);
  cryst -> coord = g_malloc0(cryst -> objects*sizeof*cryst -> coord);
  cryst -> lot = g_malloc0(cryst -> objects*sizeof*cryst -> lot);
  cryst -> at_type = g_malloc0(cryst -> objects*sizeof*cryst -> at_type);
  cryst -> z = allocdouble (species);
  cryst -> nsps = allocint (species);
  cryst -> holes = allocbool (cryst -> objects);
  return cryst;
}

/*!
  \fn crystal_data * free_crystal_data (crystal_data * cryst)

  \brief free crystal data pointer

  \param cryst the data pointer to free
*/
crystal_data * free_crystal_data (crystal_data * cryst)
{
  if (cryst -> at_by_object) g_free (cryst -> at_by_object);
  if (cryst -> pos_by_object) g_free (cryst -> pos_by_object);
  if (cryst -> occupancy) g_free (cryst -> occupancy);
  if (cryst -> sites) g_free (cryst -> sites);
  if (cryst -> insert) g_free (cryst -> insert);
  if (cryst -> position) g_free (cryst -> position);
  if (cryst -> coord) g_free (cryst -> coord);
  if (cryst -> lot) g_free (cryst -> lot);
  if (cryst -> at_type) g_free (cryst -> at_type);
  if (cryst -> z) g_free (cryst -> z);
  if (cryst -> nsps) g_free (cryst -> nsps);
  if (cryst -> holes) g_free (cryst -> holes);
  g_free (cryst);
  return NULL;
}

/*!
  \fn gboolean pos_not_taken (int pos, int dim, int * tab)

  \brief is this position already taken ?

  \param pos position id
  \param dim number of position(s)
  \param tab the list of position(s)
*/
gboolean pos_not_taken (int pos, int dim, int * tab)
{
  int i;
  for (i=0; i<dim; i++) if (tab[i] == pos) return FALSE;
  return TRUE;
}

/*!
  \fn gboolean adjust_object_occupancy (crystal_data * cryst, int occupying, int tot_cell)

  \brief adjust the crystallograpĥic sites occupancy

  \param cryst the crystal
  \param occupying how to adjust occupancy:
      0 = Random for the initial cell only,
      1 = Random cell by cell,
      2 = Completely random
      3 = Successively
      4 = Alternatively
  \param tot_cell the total number of cell to build
*/
gboolean adjust_object_occupancy (crystal_data * cryst, int occupying, int tot_cell)
{
  int h, i, j, k, l, m, n, o, p, q, r, s, t;
  double v;
  double prob;
  gboolean * taken_pos = NULL;
  gboolean ** holes_pos = NULL;
  int * pos_order = NULL;
  int * site_pos = NULL;
  int num_to_save;
  int pos_bs, num_bs, site_bs;
  gboolean pick_it;
  clock_t CPU_time;
  gboolean low_occ = FALSE;
  gboolean with_holes = FALSE;
  gboolean occ_sym = (occupying == 0 || occupying > 2) ? TRUE : FALSE;

  // position ordering must be used to ensure proper overlapping with holes
  pos_order = allocint (cryst -> objects);
  i = j = 0;
  for (k=0; k<cryst -> objects; k++)
  {
    if (cryst -> holes[k])
    {
      pos_order[i] = k;
      j = max (j, cryst -> pos_by_object[i]);
      i ++;
    }
  }
  if (i)
  {
    with_holes = TRUE;
    holes_pos = allocdbool (cryst -> objects, j);
  }
  for (j=0; j<cryst -> objects; j++)
  {
    if (! cryst -> holes[j])
    {
      pos_order[i] = j;
      i ++;
    }
  }

  for (h=0; h<cryst -> objects; h++)
  {
    i = pos_order[h];
#ifdef DEBUG
    g_debug ("i= %d, occ[%d]= %f, pbo[%d]= %d", i+1, i+1, cryst -> occupancy[i], i+1, cryst -> pos_by_object[i]);
#endif
    if (cryst -> sites[i][0] > 0 && cryst -> occupancy[i]*cryst -> pos_by_object[i] < 1.0 && ! cryst -> holes[i]) low_occ = TRUE;

    if (cryst -> sites[i][0] > 0 && cryst -> occupancy[i] < 1.0 && cryst -> pos_by_object[i] > 1)
    {
      v = 0.0;
      pos_bs = cryst -> pos_by_object[i];
      site_bs = (cryst -> holes[i]) ? 1 : cryst -> sites[i][0];
      num_bs = cryst -> sites[i][0];
      taken_pos = allocbool (pos_bs);
#ifdef DEBUG
      g_debug ("i= %d, site[%d][0]= %d, pos_bs= %d, num_bs= %d", i+1, i+1, cryst -> sites[i][0], pos_bs, num_bs);
#endif
      for (j=0; j<cryst -> sites[i][0]; j++)
      {
        k = cryst -> sites[i][j+1];
        v += cryst -> occupancy[k];
      }
      l = 0;
      if (with_holes)
      {
        for (j=0; j<site_bs; j++)
        {
          if (holes_pos[i][j]) l ++;
        }
      }
      for (j=0; j<site_bs; j++)
      {
        k = cryst -> sites[i][j+1];
#ifdef DEBUG
        g_debug ("\tj= %d, k= %d, sites[%d][0]= %d, occ[%d]= %f, pos_by_objects[%d]= %d",
                 j+1, k+1, k+1, cryst -> sites[k][0], k+1, cryst -> occupancy[k], k+1, cryst -> pos_by_object[k]);
#endif
        if (cryst -> sites[k][0] > -1)
        {
          num_to_save = round (pos_bs*cryst -> occupancy[k]);
          l += num_to_save;
#ifdef DEBUG
          g_debug ("\tnum_to_save= %d, l= %d", num_to_save, l);
#endif
          if (j == num_bs-1)
          {
            if ((v == 1.0 && l != pos_bs) || l > pos_bs) num_to_save += (pos_bs-l);
          }
          site_pos = allocint (num_to_save);
          m = 0;
          while (m < num_to_save)
          {
            pick_it = TRUE;
            if (occupying < 3)
            {
              CPU_time = clock();
              n = (CPU_time - (k+17)*pos_bs) + (3*cryst -> at_by_object[i]);
              prob = random3_(& n);
              p = round (prob * (pos_bs-1));
              if (with_holes)
              {
                pick_it = (taken_pos[p] || holes_pos[i][p]) ? FALSE : TRUE;
              }
              else
              {
                pick_it = ! taken_pos[p];
              }
            }
            else if (occupying == 3)
            {
              p = 0;
              if (with_holes)
              {
                while (taken_pos[p] || holes_pos[i][p]) p ++;
              }
              else
              {
                while (taken_pos[p]) p ++;
              }
            }
            else
            {
#ifdef DEBUG
              g_debug ("\t\tp= %d, m= %d, sites[i][0]= %d", p, m, cryst -> sites[k][0]);
#endif
              p = j + m*cryst -> sites[k][0];
              if (p > cryst -> pos_by_object[k]-1)
              {
                p -= (cryst -> pos_by_object[k]-1);
              }
#ifdef DEBUG
              g_debug ("\t\tp= %d", p);
#endif
              for (n=0; n<m; n++)
              {
                if (site_pos[n] == p)
                {
                  n = p ++;
                  if (n == cryst -> pos_by_object[k]) n = 0;
                  pick_it = FALSE;
                  while (! pick_it)
                  {
                    pick_it = TRUE;
                    for (o=0; o<m; o++)
                    {
                      if (site_pos[o] == n)
                      {
                        pick_it = FALSE;
                        n ++;
                      }
                    }
                  }
                  p = n;
                  break;
                }
              }
            }
            if (pick_it)
            {
              site_pos[m] = p;
              taken_pos[p] = TRUE;
              m ++;
            }
          }
          sort (num_to_save, site_pos);
#ifdef DEBUG
          for (m=0; m<num_to_save; m++)
          {
            g_debug ("\t\tm= %d, site_pos[%d]= %d", m+1, m+1, site_pos[m]);
          }

#endif
          for (m=0; m<num_to_save; m++)
          {
            n = site_pos[m];
            cryst -> coord[k][m] = cryst -> coord[k][n];
            if (occ_sym && tot_cell > 1)
            {
              for (o=1; o<tot_cell; o++)
              {
                p = k+o*(cryst -> objects/tot_cell);
                cryst -> coord[p][m] = cryst -> coord[p][n];
              }
            }
          }

          cryst -> pos_by_object[k] = num_to_save;
          cryst -> sites[k][0] = -1;
          if (occ_sym && tot_cell > 1)
          {
            for (o=1; o<tot_cell; o++)
            {
              p = k+o*(cryst -> objects/tot_cell);
              cryst -> pos_by_object[p] = num_to_save;
              cryst -> sites[p][0] = -1;
            }
          }
          if (cryst -> holes[i])
          {
            for (o=0; o<num_bs; o++)
            {
              p = cryst -> sites[i][o+1];
              for (q=0; q<num_to_save; q++)
              {
                r = site_pos[q];
                holes_pos[p][r] = TRUE;
                if (occ_sym && tot_cell > 1)
                {
                  for (s=1; s<tot_cell; s++)
                  {
                    t = p+s*(cryst -> objects/tot_cell);
                    holes_pos[t][r] = TRUE;
                  }
                }
              }
            }
          }
          g_free (site_pos);
        }
      }
      g_free (taken_pos);
    }
  }
  if (holes_pos) g_free (holes_pos);
  return low_occ;
}

/*!
  \fn int build_crystal (gboolean visible, project * this_proj, gboolean to_wrap, gboolean show_clones, cell_info * cell, GtkWidget * widg)

  \brief build crystal

  \param visible is the crystal builder window visible ?
  \param this_proj the target project
  \param to_wrap wrap or not atomic coordinates in the unit cell
  \param show_clones show / hide clone(s)
  \param cell the cell info that contains the crystal description
  \param widg the GtkWidget sending the signal
*/
int build_crystal (gboolean visible, project * this_proj, gboolean to_wrap, gboolean show_clones, cell_info * cell, GtkWidget * widg)
{
  int h, i, j, k, l, m, n, o, p, q;
  int build_res = 1;
  space_group * sp_group = cell -> sp_group;
  box_info * box = & cell -> box[0];
  gchar * str;
  mat4_t ** wyckpos = g_malloc0 (sp_group -> numw*sizeof*wyckpos);
  double spgpos[3][4];
  for (i=0; i<1; i++)//sp_group -> numw; i++)
  {
    wyckpos[i] = g_malloc0 (sp_group -> wyckoff[i].multi*sizeof*wyckpos[i]);
    for (j=0; j<sp_group -> wyckoff[i].multi; j++)
    {
      for (k=0; k<3; k++)
      {
        tmp_pos = g_strdup_printf ("%s", sp_group -> wyckoff[i].pos[j][k]);
        for (l=0; l<3; l++)
        {
          spgpos[k][l] = get_val_from_wyckoff (vect_comp[l], sp_group -> wyckoff[i].pos[j][k]);
        }
        if (tmp_pos)
        {
          spgpos[k][3] = get_value_from_pos (tmp_pos);
          g_free (tmp_pos);
          tmp_pos = NULL;
        }
        else
        {
          spgpos[k][3] = 0.0;
        }
      }

      wyckpos[i][j] = mat4 (spgpos[0][0], spgpos[0][1], spgpos[0][2], spgpos[0][3],
                            spgpos[1][0], spgpos[1][1], spgpos[1][2], spgpos[1][3],
                            spgpos[2][0], spgpos[2][1], spgpos[2][2], spgpos[2][3],
                            0.0, 0.0, 0.0, 1.0);
      wyckpos[i][j] = m43_mul(sp_group -> wyck_origin, wyckpos[i][j]);
#ifdef DEBUG
//      g_debug ("w_id= %d, w_mul= %d", i+1, j+1);
//      m4_print (wyckpos[i][j]);
#endif
    }
  }
  double copos[3];
  int npoints;
  vec3_t * points;
  h = sp_group -> sid;
  if (sp_group -> settings[h].nump)
  {
    points = g_malloc0(sp_group -> settings[h].nump*sizeof*points);
    for (i=0; i<sp_group -> settings[h].nump; i++)
    {
      for (j=0; j<3; j++)
      {
        tmp_pos = g_strdup_printf ("%s", sp_group -> settings[h].points[i][j]);
        copos[j] = get_value_from_pos (tmp_pos);
        g_free (tmp_pos);
        tmp_pos = NULL;
      }
      points[i] = vec3(copos[0], copos[1], copos[2]);
      //m4_mul_coord (sp_group -> coord_origin, vec3(copos[0], copos[1], copos[2]));
#ifdef DEBUG
//      g_debug ("point=%d, p.x= %f, p.y= %f, p.z= %f", i+1, points[i].x, points[i].y ,points[i].z);
#endif
    }
    npoints = sp_group -> settings[h].nump;
  }
  else
  {
    points = g_malloc0(1*sizeof*points);
    points[0] = vec3(0.0, 0.0, 0.0);
    npoints = 1;
  }

  vec3_t pos;
  atomic_object * object = NULL;
  gboolean done;
  crystal_data * cdata = NULL;
  int occupying;
  double amin = box -> param[0][0];
  for (i=1; i<3; i++) amin = min(amin, box -> param[0][i]);;
  if (! visible)
  {
    tint point;
    point.a = this_proj -> id;
    point.b = point.c = 0;
    this_proj -> modelgl = g_malloc0(sizeof*this_proj -> modelgl);
    prepare_atom_edition (& point, FALSE);
    this_proj -> modelgl -> search_widg[7] = allocate_atom_search (this_proj -> id, INSERT, 7, this_reader -> natomes);
    gboolean do_obj;
    for (i=0; i<this_reader -> natomes; i++)
    {
      do_obj = FALSE;
      for (j=0; j<this_reader -> atom_unlabelled; j++)
      {
        if (this_reader -> u_atom_list[j] == i)
        {
          do_obj = TRUE;
          break;
        }
      }
      if (do_obj)
      {
        if (! object)
        {
          this_proj -> modelgl -> atom_win -> to_be_inserted[2] = duplicate_atomic_object (get_atomic_object_by_origin (cif_object, this_reader -> object_list[j], 0));
          object = this_proj -> modelgl -> atom_win -> to_be_inserted[2];
        }
        else
        {
          object -> next = duplicate_atomic_object (get_atomic_object_by_origin (cif_object, this_reader -> object_list[j], 0));
        }
      }
      else
      {
        j = this_reader -> lot[i];
        to_insert_in_project ((int)this_reader -> z[j], -1, this_proj, this_proj -> modelgl -> search_widg[7], FALSE);
      }
      this_proj -> modelgl -> search_widg[7] -> todo[i] = 1;
      if (! object)
      {
        object = this_proj -> modelgl -> atom_win -> to_be_inserted[2];
      }
      else
      {
        object = object -> next;
      }
      object -> occ = this_reader -> occupancy[i];
      for (j=0; j<3; j++) object -> baryc[j] = this_reader -> coord[i][j];
    }
    occupying = 0;
  }
  else
  {
    occupying = this_proj -> modelgl -> builder_win -> occupancy;
  }
  for (h=0; h<2; h++)
  {
    object = this_proj -> modelgl -> atom_win -> to_be_inserted[2];
    i = j = k = 0;
    while (object)
    {
      l = object -> id;
      if (this_proj -> modelgl -> search_widg[7] -> todo[l])
      {
        if (h)
        {
          for (m=0; m<object -> species; m++)
          {
            done = FALSE;
            for (l=0; l<k; l++)
            {
              if (object -> old_z[m] == cdata -> z[l])
              {
                done = TRUE;
                break;
              }
            }
            if (! done && object -> old_z[m] > 0.0)
            {
              cdata -> z[k] = object -> old_z[m];
              k ++;
            }
            if (object -> old_z[m] == 0.0)
            {
              cdata -> holes[i] = TRUE;
              cdata -> with_holes = TRUE;
            }
          }
          n = sp_group -> wyckoff[0].multi*npoints;
          cdata -> at_type[i] = allocint(n);
          cdata -> coord[i] = g_malloc0(n*sizeof*cdata -> coord[i]);
          cdata -> insert[i] = m4_mul_coord (sp_group -> coord_origin, vec3(object -> baryc[0], object -> baryc[1], object -> baryc[2]));
#ifdef DEBUG
          // g_debug ("at_orig= %d, pos.x= %f, pos.y= %f, pos.z= %f", i+1, object -> baryc[0], object -> baryc[1], object -> baryc[2]);
          // g_debug ("at_calc= %d, pos.x= %f, pos.y= %f, pos.z= %f", i+1, cdata -> insert[i].x, cdata -> insert[i].y, cdata -> insert[i].z);
#endif
          n = 0;
          for (o=0; o<npoints; o++)
          {
            for (p=0; p<sp_group -> wyckoff[0].multi; p++)
            {
              pos = v3_add (m4_mul_coord (wyckpos[0][p], cdata -> insert[i]), points[o]);
              q = pos_not_saved (cdata -> coord[i], n, pos);
              if (q > 0)
              {
                cdata -> coord[i][n].x = pos.x;
                cdata -> coord[i][n].y = pos.y;
                cdata -> coord[i][n].z = pos.z;
#ifdef DEBUG
                // g_debug ("      c.x= %f, c.y= %f, c.z= %f", cdata -> coord[i][n].x, cdata -> coord[i][n].y, cdata -> coord[i][n].z);
#endif
                cdata -> at_type[i][n] = 1;
                n ++;
              }
              else if (q < 0)
              {
                cdata -> at_type[i][-(q+1)] ++;
              }
            }
          }
          cdata -> pos_by_object[i] = n;
          cdata -> occupancy[i] = object -> occ;
          if (! cdata -> holes[i]) cdata -> lot[i] = allocint(object -> atoms);
          cdata -> position[i] = g_malloc0(object -> atoms*sizeof*cdata -> position[i]);
          for (l=0; l<object -> atoms; l++)
          {
            n = object -> at_list[l].sp;
            cdata -> position[i][l].x = object -> at_list[l].x;
            cdata -> position[i][l].y = object -> at_list[l].y;
            cdata -> position[i][l].z = object -> at_list[l].z;
            if (! cdata -> holes[i])
            {
              for (o=0; o<k; o++)
              {
                if (cdata -> z[o] == object -> old_z[n])
                {
                  cdata -> lot[i][l] = o;
                  break;
                }
              }
            }
          }
          cdata -> at_by_object[i] = object -> atoms;
          i ++;
        }
        else
        {
          if (object -> dim > amin)
          {
            str = g_strdup_printf ("%s size (%f Ang.) is bigger than the min(<b><i>a,b,c</i></b>)\n"
                                   "If you build the crystal the final structure is likely to be crowded !\n"
                                   "Continue anyway ?", object -> name, object -> dim);
            build_res = 2;
            if (! ask_yes_no("This object might be too big !" , str, GTK_MESSAGE_WARNING, widg))
            {
              g_free (str);
              if (points) g_free (points);
              if (wyckpos) g_free (wyckpos);
              if (cdata) cdata = free_crystal_data (cdata);
              if (! visible)
              {
                this_proj -> modelgl -> search_widg[7] = free_this_search_data (this_proj -> modelgl -> search_widg[7]);
                g_free (this_proj -> modelgl);
                this_proj -> modelgl = NULL;
                active_glwin = NULL;
              }
              return 0;
            }
            g_free (str);
          }
          i ++;
          j += object -> species;
          k += object -> atoms;
        }
      }
      object = object -> next;
    }
    if (! h)
    {
      if (k)
      {
        cdata = allocate_crystal_data (i, j);
        cdata -> overlapping = (visible) ? this_proj -> modelgl -> builder_win -> overlapping : FALSE;
      }
      else
      {
        if (points) g_free (points);
        if (wyckpos) g_free (wyckpos);
        if (cdata) cdata = free_crystal_data (cdata);
        if (! visible)
        {
          this_proj -> modelgl -> search_widg[7] = free_this_search_data (this_proj -> modelgl -> search_widg[7]);
          g_free (this_proj -> modelgl);
          this_proj -> modelgl = NULL;
          active_glwin = NULL;
        }
        return 0;
      }
    }
  }

  if (points) g_free (points);
  if (wyckpos) g_free (wyckpos);

  cdata -> spec = k;

  double u, v;
  m = 0;
  for (i=0; i<cdata -> objects; i++)
  {
    m += (cdata -> holes[i]) ? 1 : 0;
    u = v = 0.0;
    for (j=0; j<2; j++)
    {
      k = 1;
      if (! cdata -> overlapping || cdata -> holes[i])
      {
        u = cdata -> occupancy[i];
      }
      else
      {
        v = cdata -> occupancy[i];
      }
      if (u+v > 1.0 || u+v < 0.0)
      {
        if (cdata) cdata = free_crystal_data (cdata);
        show_warning ("Impossible to build crystal: check occupancy !", widg);
        return 0;
      }
      else if (cdata -> overlapping && ! cdata -> with_holes)
      {
        if (j) cdata -> sites[i][k] = i;
      }
      else
      {
        for (l=0; l<cdata -> objects; l++)
        {
          if (l != i)
          {
            if (cdata -> insert[i].x == cdata -> insert[l].x
             && cdata -> insert[i].y == cdata -> insert[l].y
             && cdata -> insert[i].z == cdata -> insert[l].z)
            {
              if (j)
              {
                if (! cdata -> overlapping || cdata -> holes[i])
                {
                  k ++;
                  cdata -> sites[i][k] = l;
                }
              }
              else
              {
                if (! cdata -> overlapping || cdata -> holes[i]) k ++;
                if (! cdata -> overlapping || cdata -> holes[l])
                {
                  u += cdata -> occupancy[l];
                }
                else
                {
                  v = max(v, cdata -> occupancy[l]);
                }
                if (u+v > 1.00001)
                {
                  show_warning ("Impossible to build crystal: site total occupancy > 1.0", widg);
                  if (cdata) cdata = free_crystal_data (cdata);
                  return 0;
                }
              }
            }
          }
        }
      }
      if (! j)
      {
        cdata -> sites[i] = allocint (k+1);
        cdata -> sites[i][0] = k;
        cdata -> sites[i][1] = i;
        if (k > 1) cdata -> shared_sites = TRUE;
      }
    }
  }
#ifdef DEBUG
  /*for (i=0; i<cdata -> objects; i++)
  {
    g_debug ("i= %d, site[%d]= %d", i+1, i+1, cdata -> sites[i][0]);
    for (j=0; j<cdata -> sites[i][0]; j++)
    {
      g_debug ("\t j= %d, co-site[%d][%d]= %d", j+1, i+1, j+1, cdata -> sites[i][j+1]+1);
    }
  }*/
#endif // DEBUG

  if (m == cdata -> objects)
  {
    if (cdata) cdata = free_crystal_data (cdata);
    show_warning ("Impossible to build crystal: empty site(s) only !", widg);
    return 0;
  }
  gboolean new_proj = (this_proj -> natomes && visible) ? TRUE : FALSE;

  if (new_proj)
  {
    init_project(TRUE);
  }
  else if (visible)
  {
    active_project_changed (this_proj -> id);
  }
  for (i=0; i<3; i++)
  {
    for (j=0; j<3; j++)
    {
      if (i < 2)
      {
        active_box -> param[i][j] = box -> param[i][j];
        if (! i) active_box -> param[i][j] *= cell -> cextra[j];
      }
      active_box -> vect[i][j] *= cell -> cextra[i];
    }
  }
  compute_lattice_properties (active_cell);
  active_cell -> ltype = 1;
  active_cell -> pbc = TRUE;
  int tot_cell = 1;
  for (i=0; i<3; i++)
  {
    tot_cell *= cell -> cextra[i];
  }
  vec3_t vx, vy, vz, shift;
  h = 0;
  i = (occupying == 2) ? 0 : 1;
  j = (occupying == 2) ? 1 : tot_cell;
  crystal_data * cryst = allocate_crystal_data (j*cdata -> objects, cdata -> spec);
  g_free (cryst -> z);
  cryst -> z = duplicate_double (cdata -> spec, cdata -> z);
  cryst -> shared_sites = cdata -> shared_sites;
  cryst -> overlapping = cdata -> overlapping;
  cryst -> with_holes = cdata -> with_holes;
  if (occupying == 2)
  {
    for (k=0; k<cdata -> objects; k++)
    {
      cryst -> pos_by_object[k] = tot_cell*cdata -> pos_by_object[k];
      cryst -> at_by_object[k] = cdata -> at_by_object[k];
      cryst -> at_type[k] = duplicate_int (sp_group -> wyckoff[0].multi*npoints, cdata -> at_type[k]);
      cryst -> holes = duplicate_bool (cdata -> objects, cdata -> holes);
      if (! cdata -> holes[k]) cryst -> lot[k] = duplicate_int (cdata -> at_by_object[k], cdata -> lot[k]);
      cryst -> occupancy[k] = cdata -> occupancy[k];
      cryst -> sites[k] = duplicate_int (cdata -> sites[k][0]+1, cdata -> sites[k]);
      cryst -> position[k] = g_malloc0 (cdata -> at_by_object[k]*sizeof*cryst -> position[k]);
      for (l=0; l<cdata -> at_by_object[k]; l++) cryst -> position[k][l] = cdata -> position[k][l];
      cryst -> coord[k] = g_malloc0(cryst -> pos_by_object[k]*sizeof*cryst -> coord[k]);
    }
  }

  for (k=0; k<cell -> cextra[0]; k++)
  {
    vx = v3_muls(vec3(box -> vect[0][0], box -> vect[0][1], box -> vect[0][2]), k);
    for (l=0; l<cell -> cextra[1]; l++)
    {
      vy = v3_muls(vec3(box -> vect[1][0], box -> vect[1][1], box -> vect[1][2]), l);
      for (m=0; m<cell -> cextra[2]; m++)
      {
        vz = v3_muls(vec3(box -> vect[2][0], box -> vect[2][1], box -> vect[2][2]), m);
        shift = v3_add (vx, v3_add(vy, vz));
        for (n=0; n<cdata -> objects; n++)
        {
          if (occupying != 2)
          {
            cryst -> coord[n+h] = g_malloc0(cdata -> pos_by_object[n]*sizeof*cryst -> coord[n+h]);
            cryst -> pos_by_object[n+h] = cdata -> pos_by_object[n];
            cryst -> at_by_object[n+h] = cdata -> at_by_object[n];
            cryst -> holes[n+h] = cdata -> holes[n];
            cryst -> at_type[n+h] = duplicate_int (sp_group -> wyckoff[0].multi*npoints, cdata -> at_type[n]);
            if (! cdata -> holes[n]) cryst -> lot[n+h] = duplicate_int (cdata -> at_by_object[n], cdata -> lot[n]);
            cryst -> occupancy[n+h] = cdata -> occupancy[n];
            cryst -> sites[n+h] = duplicate_int (cdata -> sites[n][0]+1, cdata -> sites[n]);
            for (o=0; o<cryst -> sites[n+h][0]; o++) cryst -> sites[n+h][o+1] += h;
            cryst -> position[n+h] = g_malloc0 (cdata -> at_by_object[n]*sizeof*cryst -> position[n+h]);
            for (o=0; o<cdata -> at_by_object[n]; o++) cryst -> position[n+h][o] = cdata -> position[n][o];
          }
          o = cdata -> pos_by_object[n];
          for (p=0; p<o; p++)
          {
            cryst -> coord[n+i*h][p+(!i)*h*o] = v3_add(m4_mul_coord (box -> frac_to_cart, cdata -> coord[n][p]), shift);
          }
        }
        h += (occupying != 2) ? cdata -> objects : 1;
      }
    }
  }
  cdata = free_crystal_data (cdata);
  gboolean low_occ = adjust_object_occupancy (cryst, occupying, tot_cell);
  atom at, bt;
  distance dist;
  gboolean dist_chk = TRUE;

  if (! cryst -> overlapping)
  {
    for (i=0; i<cryst -> objects; i++)
    {
      if (! cryst -> holes[i])
      {
        for (j=0; j<cryst -> pos_by_object[i]; j++)
        {
          at.x = cryst -> coord[i][j].x;
          at.y = cryst -> coord[i][j].y;
          at.z = cryst -> coord[i][j].z;
          for (k=i; k<cryst -> objects; k++)
          {
            if (! cryst -> holes[k])
            {
              if (k != i || j < cryst -> pos_by_object[i]-1)
              {
                l = (k == i) ? j+1 : 0;
                for (m=l; m<cryst -> pos_by_object[k]; m++)
                {
                  bt.x = cryst -> coord[k][m].x;
                  bt.y = cryst -> coord[k][m].y;
                  bt.z = cryst -> coord[k][m].z;
                  dist = distance_3d (active_cell, 0, & at, & bt);
                  if (dist.length < 0.5)
                  {
                    // g_print ("i= %d, j= %d, k= %d, m= %d, d= %f\n", i, j, k, m, dist.length);
                    if (dist_chk)
                    {
                      build_res = 3;
                      if (ask_yes_no ("Inter-object distance(s) < 0.5 Ang. !",
                                      "Inter-object distance(s) &lt; 0.5 Ang. !\n\n\t\tContinue and leave a single object at each position ?", GTK_MESSAGE_WARNING, widg))
                      {
                        dist_chk = FALSE;
                      }
                      else
                      {
                        clean_this_proj (active_project, new_proj);
                        cryst = free_crystal_data (cryst);
                        return 0;
                      }
                    }
                    if (! dist_chk)
                    {
                      if (dist.length < 0.1)
                      {
                        cryst -> at_type[i][j] += cryst -> at_type[k][m];
                        for (n=m; n<cryst -> pos_by_object[k]-1; n++)
                        {
                          cryst -> coord[k][n].x = cryst -> coord[k][n+1].x;
                          cryst -> coord[k][n].y = cryst -> coord[k][n+1].y;
                          cryst -> coord[k][n].z = cryst -> coord[k][n+1].z;
                          cryst -> at_type[k][n] += cryst -> at_type[k][n+1];
                        }
                        cryst -> pos_by_object[k] --;
                        m --;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  int tot_new_at = 0;
  for (i=0; i<cryst -> objects; i++)
  {
    if (! cryst -> holes[i]) tot_new_at += cryst -> pos_by_object[i]*cryst -> at_by_object[i];
  }
  int * tot_new_lot = allocint(tot_new_at);
  vec3_t * ncc = g_malloc0(tot_new_at*sizeof*ncc);
  i = 0;
  for (j=0; j<cryst -> objects; j++)
  {
    if (! cryst -> holes[j])
    {
      for (k=0; k<cryst -> at_by_object[j]; k++)
      {
        for (l=0; l<cryst -> pos_by_object[j]; l++)
        {
          ncc[i] = v3_add (cryst -> coord[j][l], cryst -> position[j][k]);
          m = tot_new_lot[i] = cryst -> lot[j][k];
          cryst -> nsps[m] ++;
          i ++;
        }
      }
    }
  }
  active_project -> steps = 1;
  active_project -> natomes = tot_new_at;
  if (low_occ)
  {
    i = 0;
    for (j=0; j<cryst -> spec; j++)
    {
       if (cryst -> nsps[j]) i ++;
    }
    active_project -> nspec = i;
  }
  else
  {
    active_project -> nspec = cryst -> spec;
  }
#ifdef DEBUG
  g_debug ("CRYSTAL:: atoms= %d, species= %d", active_project -> natomes, active_project -> nspec);
#endif
  alloc_proj_data (active_project, 1);
  active_project_changed (activep);
  k = l = 0;
  for (i=0; i<cryst -> spec; i++)
  {
    if (! cryst -> nsps[i])
    {
      if (! low_occ)
      {
        if (ncc) g_free (ncc);
        if (tot_new_lot) g_free (tot_new_lot);
#ifdef DEBUG
        g_debug ("CRYSTAL:: spec= %d, label= %s, nsps= %d", i+1, periodic_table_info[j].lab, 0);
#endif
        return 0;
      }
    }
    else
    {
      j = (int)cryst -> z[i];
      active_chem -> label[k] = g_strdup_printf ("%s", periodic_table_info[j].lab);
      active_chem -> element[k] = g_strdup_printf ("%s", periodic_table_info[j].name);
      active_chem -> nsps[k] = cryst -> nsps[i];
      active_chem -> chem_prop[CHEM_Z][k] = cryst -> z[i];
      active_chem -> chem_prop[CHEM_M][k] = set_mass_ (& j);
      active_chem -> chem_prop[CHEM_R][k] = set_radius_ (& j, & l);
      active_chem -> chem_prop[CHEM_N][k] = set_neutron_ (& j);
      active_chem -> chem_prop[CHEM_X][k] = active_chem -> chem_prop[CHEM_Z][k];
#ifdef DEBUG
      g_debug ("CRYSTAL:: spec= %d, label= %s, nsps= %d", k+1, active_chem -> label[k], active_chem -> nsps[k]);
#endif
      for (m=0; m<tot_new_at;m++)
      {
        if (tot_new_lot[m] == i) tot_new_lot[m] = k;
      }
      k ++;
    }
  }
  cryst = free_crystal_data (cryst);
  copos[0] = copos[1] = copos[2] = 0.0;
  if (visible && ! new_proj)
  {
    for (i=0; i<3; i++)
    {
      for (j=0; j<3; j++) copos[i] -= active_box -> vect[j][i]/2.0;
    }
  }
  for (i=0; i<tot_new_at; i++)
  {
    active_project -> atoms[0][i].id = i;
    j = tot_new_lot[i];
    active_project -> atoms[0][i].sp =  j;
    active_project -> atoms[0][i].x = ncc[i].x + copos[0];
    active_project -> atoms[0][i].y = ncc[i].y + copos[1];
    active_project -> atoms[0][i].z = ncc[i].z + copos[2];
    active_project -> atoms[0][i].show[0] = TRUE;
    active_project -> atoms[0][i].show[1] = TRUE;
    active_project -> atoms[0][i].label[0] = FALSE;
    active_project -> atoms[0][i].label[1] = FALSE;
    active_project -> atoms[0][i].pick[0] = FALSE;
    active_project -> atoms[0][i].cloned = FALSE;
#ifdef DEBUG
    // g_debug ("sp= %d, %s %f %f %f", j+1, active_chem -> label[j], ncc[i].x, ncc[i].y, ncc[i].z);
#endif
  }
  if (ncc) g_free (ncc);
  if (tot_new_lot) g_free (tot_new_lot);

  active_cell -> has_a_box = TRUE;
  active_cell -> crystal = TRUE;

  if (visible)
  {
    for (i=0; i<3; i=i+2) active_project -> runok[i] = TRUE;
    active_project -> runok[BD] = TRUE;
    active_project -> runok[RI] = TRUE;
    active_project -> runok[CH] = TRUE;
    active_project -> runok[SP] = TRUE;
    active_project_changed (activep);
    if (new_proj)
    {
      add_project_to_workspace ();
      apply_project (TRUE);
    }
    else
    {
      active_project -> run = TRUE;
      active_image -> style = (active_project -> natomes <= 1000) ? BALL_AND_STICK : DEFAULT_STYLE;
      initcutoffs (active_chem, active_project -> nspec);
      init_curves_and_calc (active_project);
      initcwidgets ();
      active_project_changed (activep);
      init_camera (active_project, TRUE);
      set_img_lights (active_project, active_image);
      image_init_spec_data (active_image, active_project, active_project -> nspec);
      glwin_init_spec_data (active_project, active_project -> nspec);
#ifdef GTK3
      // GTK3 Menu Action To Check
      for (j=1; j<OGL_COORDS; j++) active_glwin -> ogl_coord[j] = NULL;
      for (j=0; j<OGL_RINGS; j++) active_glwin -> ogl_rings[j] = NULL;
      for (j=0; j<2; j++) active_glwin -> ogl_mode[2+j+NINPUTS] = NULL;
      active_glwin -> ogl_chains[0] = NULL;
#endif
      prepare_opengl_menu_bar (active_glwin);
      active_glwin -> labelled = check_label_numbers (active_project, 0);
      frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
      mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
      bonds_update = 1;
      active_project -> runc[0] = FALSE;
      on_calc_bonds_released (NULL, NULL);
      if (active_glwin -> mode == EDITION)
      {
#ifdef GTK4
        set_mode (NULL, & active_glwin -> colorp[0][0]);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)active_glwin -> ogl_mode[0], TRUE);
        set_mode (active_glwin -> ogl_mode[0], & active_glwin -> colorp[0][0]);
#endif
      }
      show_the_widgets (active_glwin -> win);
      gtk_button_set_label (GTK_BUTTON(active_glwin -> builder_win -> pbut), "Build (new project)");
      if (active_glwin -> atom_win)
      {
        if (active_glwin -> atom_win -> win)
        {
          clean_all_trees (active_glwin -> search_widg[7], active_project);
        }
      }
    }
    init_camera (active_project, TRUE);
    active_image -> box_axis[0] = 1;
    if (to_wrap)
    {
      shift_it (vec3(0.0,0.0,0.0), 1, activep);
      active_glwin -> wrapped = TRUE;
    }
    active_image -> draw_clones = (active_glwin -> allbonds[1]) ? show_clones : FALSE;
    update_all_menus (active_glwin, active_project -> natomes);
#ifdef GTK3
    // GTK3 Menu Action To Check
    for (i=0; i<2; i++)
    {
      if (active_glwin -> ogl_box[i] != NULL)
      {
        widget_set_sensitive (active_glwin -> ogl_box[i], active_cell -> ltype);
      }
    }
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)active_glwin -> ogl_rep[0], TRUE);
    set_rep (active_glwin -> ogl_rep[0], & active_glwin -> colorp[0][0]);
#endif
    init_shaders (active_glwin);
    update (active_glwin);
    chemistry_ ();
  }
  else
  {
    active_project -> modelgl -> search_widg[7] = free_this_search_data (active_project -> modelgl -> search_widg[7]);
    g_free (active_project -> modelgl);
    active_project -> modelgl = NULL;
    active_glwin = NULL;
  }
  update_insert_combos ();
  active_cell -> sp_group = duplicate_space_group (sp_group);
  if (low_occ)
  {
    gchar * low_warning = "The crystal will be created however some objects might be missing,\n"
                           "Occupancy is too low compared to the number of site(s) per cell.\n\n"
                          "<b>To build a crystal matching the defined occupancy</b>:\n"
                          "\t <b>1)</b> If you are trying to read a CIF file, use the crystal builder instead.\n"
                          "\t <b>2)</b> Modify the occupancy set-up to 'Completely random'.\n"
                          "\t <b>3)</b> Increase the number of unit cells up to get rid of this message.";
    show_warning (low_warning, widg);
  }
  return build_res;
}
