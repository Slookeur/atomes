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
* @file d_measures.c
* @short Functions to prepare the OpenGL rendering of the measure(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'd_measures.c'
*
* Contains:
*

 - The functions to prepare the OpenGL rendering of the measure(s)

*
* List of functions:

  int prepare_measure_shaders (int type, int shaders);

  void draw_angle_label (atom * at, atom * bt, atom * ct, int pi);
  void set_measure_color (int selected, int id, int num);
  void setup_this_measured_angle (int s, int sa, int sb, int sc, int pi);
  void angles_loop (glwin * view, int id, int pi, GtkTreeStore * store);
  void dihedrals_loop (glwin * view, int id, int pi, GtkTreeStore * store);
  void draw_bond_label (atom * at, atom * bt, int pi);
  void setup_this_measured_bond (int s, int sa, int sb, int pi);
  void bonds_loop (glwin * view, int id, int pi, GtkTreeStore * store);
  void create_measures_lists ();

*/

#include "global.h"
#include "glview.h"

extern void setup_line_vertice (float * vertices, vec3_t pos, ColRGBA col, float alpha);
extern void unrotate_camera ();
extern double arc_cos (double val);
extern ColRGBA init_color (int id, int numid);
extern void fill_bond_model_row (int p, int a, int b, GtkTreeStore * store);
extern void fill_angle_model_row (int p, int a, int b, int c, GtkTreeStore * store);
extern void fill_dihedral_model_row (int p, int a, int b, int c, int d, GtkTreeStore * store);
extern int num_bonds (int i);
extern int num_angles (int i);
extern int num_dihedrals (int i);
extern void clean_labels (int id);

extern int objects[3];
extern int * object_was_selected[3];
extern int ** tmp_object_id[3];

int type_of_measure;
int measures_drawing;

atom_in_selection * tmp_a, * tmp_b, * tmp_c, * tmp_d;
atom * ta, * tb, * tc, * td, * te, * tf;
distance dist_ba, dist_bc;
int * shift;
double dist;

object_3d * measure;

ColRGBA col;
ColRGBA col_gdk;

/*!
  \fn void draw_angle_label (atom * at, atom * bt, atom * ct, int pi)

  \brief prepare an measured angle label OpenGL rendering

  \param at 1st atom
  \param bt 2nd atom
  \param ct 3rd atom
  \param pi 0 = mouse analysis mode, 1 = mouse edition mode
*/
void draw_angle_label (atom * at, atom * bt, atom * ct, int pi)
{
  angle real_theta = angle_3d (cell_gl, (cell_gl -> npt) ? step : 0, at, bt, ct);
  gchar * str;
  if (real_theta.pbc)
  {
    str = g_strdup_printf("%.1lf° (PBC)", real_theta.angle);
  }
  else
  {
    str = g_strdup_printf("%.1lf°", real_theta.angle);
  }
  float ls[3] = {15.0, 15.0, 0.0};
  vec3_t pos = vec3(bt -> x, bt -> y, bt -> z);
  prepare_string (str, 3+pi, plot -> labels[3+pi].color[0], pos, ls, at, bt, ct);
  g_free (str);
}

/*!
  \fn void set_measure_color (int selected, int id, int num)

  \brief set measure color

  \param selected total number of selected atom(s)
  \param id measured element id number
  \param num total number of measured element(s) in the list
*/
void set_measure_color (int selected, int id, int num)
{
  if (selected)
  {
    col = init_color (id, num);
  }
  else
  {
    col.red   = 1.0 - plot -> back -> color.red;
    col.green = 1.0 - plot -> back -> color.green;
    col.blue  = 1.0 - plot -> back -> color.blue;
    col.alpha = 1.0;
  }
}

/*!
  \fn void setup_this_measured_angle (int s, int sa, int sb, int sc, int pi)

  \brief prepare measured angle to render

  \param s measured line (0) or label (1)
  \param sa 1st atom
  \param sb 2nd atom
  \param sc 3rd atom
  \param pi 0 = mouse analysis mode, 1 = mouse edition mode
*/
void setup_this_measured_angle (int s, int sa, int sb, int sc, int pi)
{
  float alpha = 1.0;
  float shift[3];
  int p, q, r;
  vec3_t pos_a, pos_b, pos_c;
  atom * at, * bt, * ct;
  at = & proj_gl -> atoms[step][sa];
  bt = & proj_gl -> atoms[step][sb];
  ct = & proj_gl -> atoms[step][sc];

  for (p=0; p<plot -> abc -> extra_cell[0]+1;p++)
  {
    for (q=0; q<plot -> abc -> extra_cell[1]+1; q++)
    {
      for (r=0; r<plot -> abc -> extra_cell[2]+1; r++)
      {
        shift[0]=p*box_gl -> vect[0][0]+q*box_gl -> vect[1][0]+r*box_gl -> vect[2][0];
        shift[1]=p*box_gl -> vect[0][1]+q*box_gl -> vect[1][1]+r*box_gl -> vect[2][1];
        shift[2]=p*box_gl -> vect[0][2]+q*box_gl -> vect[1][2]+r*box_gl -> vect[2][2];
        at_shift (at, shift);
        at_shift (bt, shift);
        at_shift (ct, shift);
        pos_a = vec3(at -> x, at -> y, at -> z);
        pos_b = vec3(bt -> x, bt -> y, bt -> z);
        pos_c = vec3(ct -> x, ct -> y, ct -> z);
        if (s == 0)
        {
          setup_line_vertice (measure -> vertices, pos_a, col, alpha);
          setup_line_vertice (measure -> vertices, pos_b, col, alpha);
          setup_line_vertice (measure -> vertices, pos_c, col, alpha);
        }
        else
        {
          // Text location for the instances !
          draw_angle_label (at, bt, ct, pi);
        }
        at_unshift (at, shift);
        at_unshift (bt, shift);
        at_unshift (ct, shift);
        alpha = 0.5;
      }
    }
  }
}

/*!
  \fn void angles_loop (glwin * view, int id, int pi, GtkTreeStore * store)

  \brief do things for the measured angles selection

  \param view the target glwin
  \param id action to perform: \n
                           -2 = save angles selection, \n
                           -1 = clean angles selection, \n
                            0 = prepare OpenGL buffer data analysis mode selection measure, \n
                            1 = prepare OpenGL buffer data edition mode selection measure, \n
                            2 = fill angles tree store measures window
  \param pi 0 = mouse analysis mode, 1 = mouse edition mode
  \param store the tree store to fill, if any
*/
void angles_loop (glwin * view, int id, int pi, GtkTreeStore * store)
{
  int i, j, k, l, m;
  int ** did_it;
  int aid;
  gboolean do_it;
  //gboolean colored;
  image * img = view -> anim -> last -> img;
  // Total number of angles:
  int n_angl = num_angles (img -> selected[pi] -> selected);

  did_it = allocdint (n_angl, 3);
  aid = 0;
  tmp_a = img -> selected[pi] -> first;
  for (i=0; i < img -> selected[pi] -> selected; i++)
  {
    tmp_b = img -> selected[pi] -> first;
    for (j=0; j < img -> selected[pi] -> selected; j++)
    {
      if (tmp_b -> id != tmp_a -> id)
      {
        tmp_c = img -> selected[pi] -> first;
        for (k=0; k < img -> selected[pi] -> selected; k++)
        {
          if (tmp_c -> id != tmp_a -> id && tmp_c -> id != tmp_b -> id)
          {
            do_it = TRUE;
            for (l=0; l<aid; l++)
            {
              if (did_it[l][1] == tmp_b -> id &&
                  ((did_it[l][0] == tmp_a -> id && did_it[l][2] == tmp_c -> id)
                || (did_it[l][2] == tmp_a -> id && did_it[l][0] == tmp_c -> id)))
              {
                do_it = FALSE;
                break;
              }
            }
            if (do_it)
            {
              did_it[aid][0] = tmp_a -> id;
              did_it[aid][1] = tmp_b -> id;
              did_it[aid][2] = tmp_c -> id;
              switch (id)
              {
                case -2:
                  for (m=0; m<3; m++) tmp_object_id[1][aid][m] = did_it[aid][m];
                  break;
                case -1:
                  img -> selected[pi] -> selected_angles[aid] = 0;
                  break;
                default:
                  if (img -> selected[pi] -> selected_angles[aid] || img -> m_is_pressed == 2 || id == 2)
                  {
                    //colored = FALSE;
                    if (id < 2)
                    {
                      set_measure_color (img -> selected[pi] -> selected_angles[aid], aid, n_angl);
                      //colored = TRUE;
                    }
                    switch (id)
                    {
                      case 2:
                        fill_angle_model_row (view -> proj, tmp_a -> id, tmp_b -> id, tmp_c -> id, store);
                        break;
                      default:
                        setup_this_measured_angle (id, tmp_a -> id, tmp_b -> id, tmp_c -> id, pi);
                        break;
                    }
                  }
                  break;
              }
              aid ++;
            }
          }
          if (tmp_c -> next != NULL) tmp_c = tmp_c -> next;
        }
      }
      if (tmp_b -> next != NULL) tmp_b = tmp_b -> next;
    }
    if (tmp_a -> next != NULL) tmp_a = tmp_a -> next;
  }

  if (id == -1)
  {
    for (i=0; i<n_angl; i++)
    {
      for (j=0; j<objects[1]; j++)
      {
        if (did_it[i][1] == tmp_object_id[1][j][1] &&
            ((did_it[i][0] == tmp_object_id[1][j][0] && did_it[i][2] == tmp_object_id[1][j][2])
            || (did_it[i][0] == tmp_object_id[1][j][2] && did_it[i][2] == tmp_object_id[1][j][0])))
        {
          img -> selected[pi] -> selected_angles[i] = object_was_selected[1][j];
        }
      }
    }
  }
  g_free (did_it);
}

/*!
  \fn void dihedrals_loop (glwin * view, int id, int pi, GtkTreeStore * store)

  \brief do things for the measured dihedrals selection

  \param view the target glwin
  \param id action to perform: \n
                           -2 = save dihedrals selection \n
                           -1 = clean dihedrals selection \n
                            0 = prepare OpenGL buffer data analysis mode selection measure \n
                            1 = prepare OpenGL buffer data edition mode selection measure \n
                            2 = fill dihedrals tree store measures window
  \param pi 0 = mouse analysis mode, 1 = mouse edition mode
  \param store the tree store to fill, if any
*/
void dihedrals_loop (glwin * view, int id, int pi, GtkTreeStore * store)
{
  int i, j, k, l, m;
  int ** did_it;
  int did;
  gboolean do_it;
  image * img = view -> anim -> last -> img;
  // gboolean colored;
  // Total number of dihedral angles:
  int n_dihedral = num_dihedrals (img -> selected[pi] -> selected);

  did_it = allocdint (n_dihedral, 4);
  did = 0;
  tmp_a = img -> selected[pi] -> first;
  for (i=0; i < img -> selected[pi] -> selected; i++)
  {
    tmp_b = img -> selected[pi] -> first;
    for (j=0; j < img -> selected[pi] -> selected; j++)
    {
      if (tmp_b -> id != tmp_a -> id)
      {
        tmp_c = img -> selected[pi] -> first;
        for (k=0; k < img -> selected[pi] -> selected; k++)
        {
          if (tmp_c -> id != tmp_a -> id && tmp_c -> id != tmp_b -> id)
          {
            tmp_d = img -> selected[pi] -> first;
            for (l=0; l < img -> selected[pi] -> selected; l++)
            {
              if (tmp_d -> id != tmp_a -> id && tmp_d -> id != tmp_b -> id && tmp_d -> id != tmp_c -> id)
              {
                do_it = TRUE;
                for (m=0; m<did; m++)
                {
                  if ((did_it[m][0] == tmp_a -> id && did_it[m][1] == tmp_b -> id && did_it[m][2] == tmp_c -> id && did_it[m][3] == tmp_d -> id)
                    ||(did_it[m][0] == tmp_d -> id && did_it[m][1] == tmp_c -> id && did_it[m][2] == tmp_b -> id && did_it[m][3] == tmp_a -> id))
                  {
                    do_it = FALSE;
                    break;
                  }
                }
                if (do_it)
                {
                  did_it[did][0] = tmp_a -> id;
                  did_it[did][1] = tmp_b -> id;
                  did_it[did][2] = tmp_c -> id;
                  did_it[did][3] = tmp_d -> id;
                  switch (id)
                  {
                    case -2:
                      for (m=0; m<4; m++) tmp_object_id[2][did][m] = did_it[did][m];
                      break;
                    case -1:
                      img -> selected[pi] -> selected_dihedrals[did] = 0;
                      break;
                    default:
                      if (img -> selected[pi] -> selected_dihedrals[did] || img -> m_is_pressed == 3 || id == 2)
                      {
                        //colored = FALSE;
                        if (id < 2)
                        {
                          set_measure_color (img -> selected[pi] -> selected_dihedrals[did], did, n_dihedral);
                          //colored = TRUE;
                        }
                        switch (id)
                        {
                          case 0:
                            //draw_angle_arc (colored);
                            break;
                          case 1:
                            //draw_angle_label ();
                            break;
                          case 2:
                            fill_dihedral_model_row (view -> proj, tmp_a -> id, tmp_b -> id, tmp_c -> id, tmp_d -> id, store);
                            break;
                        }
                      }
                      break;
                  }
                  did ++;
                }
              }
              if (tmp_d -> next != NULL) tmp_d = tmp_d -> next;
            }
          }
          if (tmp_c -> next != NULL) tmp_c = tmp_c -> next;
        }
      }
      if (tmp_b -> next != NULL) tmp_b = tmp_b -> next;
    }
    if (tmp_a -> next != NULL) tmp_a = tmp_a -> next;
  }

  if (id == -1)
  {
    for (i=0; i<n_dihedral; i++)
    {
      for (j=0; j<objects[2]; j++)
      {
        if ((did_it[i][0] == tmp_object_id[2][j][0] && did_it[i][1] == tmp_object_id[2][j][1] && did_it[i][2] == tmp_object_id[2][j][2] && did_it[i][3] == tmp_object_id[2][j][3])
          ||(did_it[i][0] == tmp_object_id[2][j][3] && did_it[i][1] == tmp_object_id[2][j][2] && did_it[i][2] == tmp_object_id[2][j][1] && did_it[i][3] == tmp_object_id[2][j][0]))
        {
          img -> selected[pi] -> selected_dihedrals[i] = object_was_selected[2][j];
        }
      }
    }
  }
  g_free (did_it);
}

/*!
  \fn void draw_bond_label (atom * at, atom * bt, int pi)

  \brief prepare a measured distance OpenGL rendering

  \param at 1st atom
  \param bt 2nd atom
  \param pi 0 = mouse analysis mode, 1 = mouse edition mode
*/
void draw_bond_label (atom * at, atom * bt, int pi)
{
  distance dist = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, at, bt);
  vec3_t pos;
  if (dist.pbc)
  {
    pos = vec3 (at -> x - (at -> x - bt -> x)/2.0, at -> y - (at -> y - bt -> y)/2.0, at -> z - (at -> z - bt -> z)/2.0);
  }
  else
  {
    pos = vec3 (at -> x-dist.x/2.0, at -> y-dist.y/2.0, at -> z-dist.z/2.0);
  }
  gchar * str;
  if (dist.pbc)
  {
    str = g_strdup_printf("%.3lf Å (PBC)", dist.length);
  }
  else
  {
    str = g_strdup_printf("%.3lf Å", dist.length);
  }
  float ls[3] = {-15.0, 15.0, 0.0};
  prepare_string (str, 3+pi, plot -> labels[3+pi].color[0], pos, ls, at, bt, NULL);
  g_free (str);
}

/*!
  \fn void setup_this_measured_bond (int s, int sa, int sb, int pi)

  \brief prepare measured distance to render

  \param s measured line (0) or label (1)
  \param sa 1st atom
  \param sb 2nd atom
  \param pi 0 = mouse analysis mode, 1 = mouse edition mode
*/
void setup_this_measured_bond (int s, int sa, int sb, int pi)
{
  float alpha = 1.0;
  float shift[3];
  int p, q, r;
  vec3_t pos_a, pos_b;
  atom * at, * bt;
  at = & proj_gl -> atoms[step][sa];
  bt = & proj_gl -> atoms[step][sb];

  for (p=0; p<plot -> abc -> extra_cell[0]+1;p++)
  {
    for (q=0; q<plot -> abc -> extra_cell[1]+1; q++)
    {
      for (r=0; r<plot -> abc -> extra_cell[2]+1; r++)
      {
        shift[0]=p*box_gl -> vect[0][0]+q*box_gl -> vect[1][0]+r*box_gl -> vect[2][0];
        shift[1]=p*box_gl -> vect[0][1]+q*box_gl -> vect[1][1]+r*box_gl -> vect[2][1];
        shift[2]=p*box_gl -> vect[0][2]+q*box_gl -> vect[1][2]+r*box_gl -> vect[2][2];
        at_shift (at, shift);
        at_shift (bt, shift);
        pos_a = vec3(at -> x, at -> y, at -> z);
        pos_b = vec3(bt -> x, bt -> y, bt -> z);
        if (s == 0)
        {
          setup_line_vertice (measure -> vertices, pos_a, col, alpha);
          setup_line_vertice (measure -> vertices, pos_b, col, alpha);
        }
        else
        {
          // Text location for the instances !
          draw_bond_label (at, bt, pi);
        }
        at_unshift (at, shift);
        at_unshift (bt, shift);
        alpha = 0.5;
      }
    }
  }
}

/*!
  \fn void bonds_loop (glwin * view, int id, int pi, GtkTreeStore * store)

  \brief do things for the measured distances selection

  \param view the target glwin
  \param id action to perform: \n
                           -2 = save bonds selection \n
                           -1 = clean bonds selection \n
                            0 = prepare OpenGL buffer data analysis mode selection measure \n
                            1 = prepare OpenGL buffer data edition mode selection measure \n
                            2 = fill bonds distances tree store measures window
  \param pi 0 = mouse analysis mode, 1 = mouse edition mode
  \param store the tree store to fill, if any
*/
void bonds_loop (glwin * view, int id, int pi, GtkTreeStore * store)
{
  int i, j;
  int ** did_it;
  int bid;
  image * img = view -> anim -> last -> img;
  int n_dist = num_bonds (img -> selected[pi] -> selected);

  if (id == 1) shift = allocint(2);
  did_it = allocdint (n_dist, 2);
  bid = -1;
  tmp_a = img -> selected[pi] -> first;
  for (i=0; i < img -> selected[pi] -> selected-1; i++)
  {
    tmp_b = tmp_a -> next;
    for (j=i+1; j < img -> selected[pi] -> selected; j++)
    {
      bid ++;
      did_it[bid][0] = tmp_a -> id;
      did_it[bid][1] = tmp_b -> id;
      switch (id)
      {
         case -2:
           tmp_object_id[0][bid][0] = tmp_a -> id;
           tmp_object_id[0][bid][1] = tmp_b -> id;
           break;
         case -1:
           img -> selected[pi] -> selected_bonds[bid] = 0;
           break;
         default:
           if (img -> selected[pi] -> selected_bonds[bid] || img -> m_is_pressed == 1 || id == 2)
           {
             if (id < 2)
             {
               set_measure_color (img -> selected[pi] -> selected_bonds[bid], bid, n_dist);
             }
             switch (id)
             {
               case 2:
                 fill_bond_model_row (view -> proj, tmp_a -> id, tmp_b -> id, store);
                 break;
               default:
                 setup_this_measured_bond (id, tmp_a -> id, tmp_b -> id, pi);
                 break;
             }
           }
           break;
      }
      if (tmp_b -> next != NULL) tmp_b = tmp_b -> next;
    }
    if (tmp_a -> next != NULL) tmp_a = tmp_a -> next;
  }
  if (id == 1) g_free (shift);
  if (id == -1)
  {
    for (i=0; i<n_dist; i++)
    {
      for (j=0; j<objects[0]; j++)
      {
        if ((did_it[i][0] == tmp_object_id[0][j][0] && did_it[i][1] == tmp_object_id[0][j][1]) ||
            (did_it[i][0] == tmp_object_id[0][j][1] && did_it[i][1] == tmp_object_id[0][j][0]))
        {
          img -> selected[pi] -> selected_bonds[i] = object_was_selected[0][j];
        }
      }
    }
  }
  g_free (did_it);
}

/*!
  \fn void prepare_measure_shaders (int mode)

  \brief prepare measure OpenGL rendering

  \param mode (0 = analysis mode, 1 = edition mode)
*/
void prepare_measure_shaders (int mode)
{
  int nshaders = 0;
  if (plot -> selected[mode] -> selected > 1 && plot -> selected[mode] -> selected < MAX_IN_SELECTION)
  {
    if (plot -> mpattern[mode] > -1)
    {
      // First the bond distances
      measure = g_malloc0 (sizeof*measure);
      measure -> vert_buffer_size = LINE_BUFF_SIZE;
      measure -> num_vertices = 2 * num_bonds (plot -> selected[mode] -> selected) * (plot -> abc -> extra_cell[0]+1)*(plot -> abc -> extra_cell[1]+1)*(plot -> abc -> extra_cell[2]+1);
      measure -> vertices = allocfloat (measure -> vert_buffer_size*measure -> num_vertices);
      nbs = 0;
      bonds_loop (wingl, 0, mode, NULL);

      if (plot -> mpattern[mode] != 2)
      {
        wingl -> ogl_glsl[MEASU][mode][0] = init_shader_program (MEASU, GLSL_LINES, line_vertex, line_stipple, line_stipple_color, GL_LINES, 2, 7, FALSE, measure);
      }
      else
      {
        wingl -> ogl_glsl[MEASU][mode][0] = init_shader_program (MEASU, GLSL_LINES, line_vertex, NULL, line_color, GL_LINES, 2, 7, FALSE, measure);
      }
      wingl -> ogl_glsl[MEASU][mode][0] -> line_width = plot -> mwidth[mode];
      nshaders ++;
      g_free (measure);

      // The angles
      if (plot -> selected[mode] -> selected > 2)
      {
        measure = g_malloc0 (sizeof*measure);
        measure -> vert_buffer_size = LINE_BUFF_SIZE;
        measure -> num_vertices = 3 * num_angles (plot -> selected[mode] -> selected) * (plot -> abc -> extra_cell[0]+1)*(plot -> abc -> extra_cell[1]+1)*(plot -> abc -> extra_cell[2]+1);
        measure -> vertices = allocfloat (measure -> vert_buffer_size*measure -> num_vertices);
        nbs = 0;
        angles_loop (wingl, 0, mode, NULL);
        if (plot -> mpattern[mode] != 2)
        {
          wingl -> ogl_glsl[MEASU][mode][1] = init_shader_program (MEASU, GLSL_LINES, angle_vertex, angle_stipple, line_stipple_color, GL_TRIANGLES, 2, 7, FALSE, measure);
        }
        else
        {
          wingl -> ogl_glsl[MEASU][mode][1] = init_shader_program (MEASU, GLSL_LINES, angle_vertex, angle_stipple, angle_color, GL_TRIANGLES, 2, 7, FALSE, measure);
        }
        wingl -> ogl_glsl[MEASU][mode][1] -> line_width = plot -> mwidth[mode];
        nshaders ++;
        g_free (measure);
      }
    }
     // When all labels are found we render the text if any
    if (plot -> labels[3+mode].list != NULL)
    {
      measures_drawing = nshaders;
      render_all_strings (MEASU, 3+mode);
    }
  }
}

/*!
  \fn void create_measures_lists ()

  \brief prepare measure(s) OpenGL rendering
*/
void create_measures_lists ()
{
  // First we draw all lines/angles displayed on screen
#ifdef DEBUG
  g_debug ("Measure LIST");
#endif
  cleaning_shaders (wingl, MEASU);
  clean_labels (3);
  clean_labels (4);
  wingl -> create_shaders[MEASU] = FALSE;
  wingl -> n_shaders[MEASU][0] = wingl -> n_shaders[MEASU][1] = 0;

  int i, j, k;
  i = (is_atom_win_active(wingl) || (wingl -> mode == EDITION && wingl -> selection_mode == NSELECTION-1)) ? 1 : 0;
  for (j=i; j<2; j++)
  {
    if (plot -> selected[j] -> selected > 1 && plot -> selected[j] -> selected < MAX_IN_SELECTION)
    {
      if (plot -> mpattern[j] > -1)
      {
        wingl -> n_shaders[MEASU][j] ++;
        if (plot -> selected[j] -> selected > 2) wingl -> n_shaders[MEASU][j] ++;
      }
      // First we need to prepare the labels
      type_of_measure = 6;
      bonds_loop (wingl, 1, j, NULL);

      if (plot -> selected[j] -> selected >= 3)
      {
        type_of_measure = 9;
        angles_loop (wingl, 1, j, NULL);
      }

      if (plot -> labels[3+j].list != NULL)
      {
        // shaders for the labels if any
        wingl -> n_shaders[MEASU][j] += (plot -> labels[3+j].render+1) * (plot -> labels[3+j].list -> last -> id + 1);
      }
    }
  }

  if (wingl -> n_shaders[MEASU][0] || wingl -> n_shaders[MEASU][1])
  {
    measures_drawing = 0;
    for (k=i; k<2; k++)
    {
      wingl -> ogl_glsl[MEASU][k] = g_malloc0 (wingl -> n_shaders[MEASU][k]*sizeof*wingl -> ogl_glsl[MEASU][k]);
      prepare_measure_shaders (k);
    }
  }
}

