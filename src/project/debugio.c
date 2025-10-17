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
* @file debugio.c
* @short Some debugging and printing functions
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'debugio.c'
*
* Contains:
*

 - Some debugging and printing functions

*
* List of functions:

  void debugiocurve (project * this_proj, gboolean win, int rid, int cid, gchar * iost);
  void debug_lattice_info (project * this_proj, gchar * iost);
  void debugioproj (project * this_proj, gchar * iost);
  void debug_chemical_information (project * this_proj);

*/

#include "global.h"
#include "project.h"

/*!
  \fn void debugiocurve (project * this_proj, gboolean win, int rid, int cid, gchar * iost)

  \brief debug and print some info about the curves in a project

  \param this_proj the target project
  \param win curve window exists (1/0)
  \param rid analysis id
  \param cid curve id
  \param iost input or output information
*/
void debugiocurve (project * this_proj, gboolean win, int rid, int cid, gchar * iost)
{
  Curve * this_curve = this_proj -> analysis[rid] -> curves[cid];
  g_debug (" ");
  g_debug ("IODEBUG: %s: pid= %d, rid= %d, cid= %d", iost, this_proj -> id, rid, cid);
  g_debug ("IODEBUG: %s: ndata= %d", iost, this_curve -> ndata);
  if (this_curve -> data[0] == NULL)
  {
    g_debug ("IODEBUG: %s: data[0] == NULL", iost);
  }
  g_debug ("IODEBUG: %s: x[0]= %f", iost, this_curve -> data[0][0]);
  g_debug ("IODEBUG: %s: y[0]= %f", iost, this_curve -> data[1][0]);
  g_debug ("IODEBUG: %s: x[ndata-1]= %f", iost, this_curve -> data[0][this_curve -> ndata - 1]);
  g_debug ("IODEBUG: %s: y[ndata-1]= %f", iost, this_curve -> data[1][this_curve -> ndata - 1]);
  if (win)
  {
    g_debug ("IODEBUG: %s: cmax[0]= %f, cmax[1]= %f", iost,
                          this_curve -> cmax[0], this_curve -> cmax[1]);
    g_debug ("IODEBUG: %s: name= %s", iost, this_curve -> name);
    g_debug ("IODEBUG: %s: show_title= %d", iost, this_curve -> show_title);
    g_debug ("IODEBUG: %s: default_title= %d", iost, this_curve -> default_title);
    g_debug ("IODEBUG: %s: title= %s", iost, this_curve -> title);
    g_debug ("IODEBUG: %s: title_font= %s", iost, this_curve -> title_font);
    g_debug ("IODEBUG: %s: title_pos[0]= %f, title_pos[1]= %f", iost,
             this_curve -> title_pos[0], this_curve -> title_pos[1]);
    g_debug ("IODEBUG: %s: title.red= %f, title.green= %f, title.blue= %f", iost,
             this_curve -> title_color.red,
             this_curve -> title_color.green,
             this_curve -> title_color.blue);
    // Axis
    g_debug ("IODEBUG: %s: axmin[0]= %f, axmin[1]= %f", iost,
             this_curve -> axmin[0], this_curve -> axmin[1]);
    g_debug ("IODEBUG: %s: axmax[0]= %f, axmax[1]= %f", iost,
             this_curve -> axmax[0], this_curve -> axmax[1]);
    g_debug ("IODEBUG: %s: x_axis_title= %s", iost, this_curve -> axis_title[0]);
    g_debug ("IODEBUG: %s: x_axis_title_font= %s", iost, this_curve -> axis_title_font[0]);
    g_debug ("IODEBUG: %s: x_axis_title_pos[0]= %d, x_axis_title_pos[1]= %d", iost,
             this_curve -> axis_title_x[0], this_curve -> axis_title_y[0]);
    g_debug ("IODEBUG: %s: y_axis_title= %s", iost, this_curve -> axis_title[1]);
    g_debug ("IODEBUG: %s: y_axis_title_font= %s", iost, this_curve -> axis_title_font[1]);
    g_debug ("IODEBUG: %s: y_axis_title_pos[0]= %d, y_axis_title_pos[1]= %d", iost,
             this_curve -> axis_title_x[1], this_curve -> axis_title_y[1]);
    g_debug ("IODEBUG: %s: scale[0]= %d, scale[1]= %d", iost,
             this_curve -> scale[0], this_curve -> scale[1]);
    g_debug ("IODEBUG: %s: x_axis_defaut_title= %d", iost, this_curve -> axis_defaut_title[0]);
    g_debug ("IODEBUG: %s: y_axis_defaut_title= %d", iost, this_curve -> axis_defaut_title[1]);
    g_debug ("IODEBUG: %s: grid[0]= %d, grid[1]= %d", iost,
             this_curve -> show_grid[0], this_curve -> show_grid[1]);
    g_debug ("IODEBUG: %s: show_axis[0]= %d, show_axis[1]= %d", iost,
             this_curve -> show_axis[0], this_curve -> show_axis[1]);
    g_debug ("IODEBUG: %s: autoscale[0]= %d, autoscale[1]= %d", iost,
             this_curve -> autoscale[0], this_curve -> autoscale[1]);
    g_debug ("IODEBUG: %s: majt[0]= %f, majt[1]= %f", iost,
             this_curve -> majt[0], this_curve -> majt[1]);
    g_debug ("IODEBUG: %s: mint[0]= %d, mint[1]= %d", iost,
             this_curve -> mint[0], this_curve -> mint[1]);
    g_debug ("IODEBUG: %s: ticks_io[0]= %d, ticks_io[1]= %d", iost,
             this_curve -> ticks_io[0], this_curve -> ticks_io[1]);
    g_debug ("IODEBUG: %s: ticks_pos[0]= %d, ticks_pos[1]= %d", iost,
             this_curve -> ticks_pos[0], this_curve -> ticks_pos[1]);
    g_debug ("IODEBUG: %s: majt_size[0]= %d, majt_size[1]= %d", iost,
             this_curve -> majt_size[0], this_curve -> majt_size[1]);
    g_debug ("IODEBUG: %s: mint_size[0]= %d, mint_size[1]= %d", iost,
             this_curve -> mint_size[0], this_curve -> mint_size[1]);
    g_debug ("IODEBUG: %s: labels_pos[0]= %d, labels_pos[1]= %d", iost,
             this_curve -> labels_pos[0], this_curve -> labels_pos[1]);
    g_debug ("IODEBUG: %s: labels_digit[0]= %d, labels_digit[1]= %d", iost,
             this_curve -> labels_digit[0], this_curve -> labels_digit[1]);
    g_debug ("IODEBUG: %s: labels_font[0]= %s, labels_font[1]= %s", iost,
             this_curve -> labels_font[0], this_curve -> labels_font[1]);
    g_debug ("IODEBUG: %s: labels_angle[0]= %f, labels_angle[1]= %f", iost,
             this_curve -> labels_angle[0], this_curve -> labels_angle[1]);
    g_debug ("IODEBUG: %s: labels_shift_x[0]= %d, labels_shift_y[0]= %d", iost,
             this_curve -> labels_shift_x[0], this_curve -> labels_shift_y[0]);
    g_debug ("IODEBUG: %s: labels_shift_x[1]= %d, labels_shift_y[1]= %d", iost,
             this_curve -> labels_shift_x[1], this_curve -> labels_shift_y[1]);
    // Legend
    g_debug ("IODEBUG: %s: show_legend= %d", iost, this_curve -> show_legend);
    g_debug ("IODEBUG: %s: legend_pos[0]= %f, legend_pos[1]= %f", iost,
             this_curve -> legend_pos[0], this_curve -> legend_pos[1]);
    g_debug ("IODEBUG: %s: legend_font= %s", iost, this_curve -> legend_font);
    g_debug ("IODEBUG: %s: legend.red= %f, legend.green= %f, legend.blue= %f", iost,
             this_curve -> legend_color.red,
             this_curve -> legend_color.green,
             this_curve -> legend_color.blue);
    g_debug ("IODEBUG: %s: show_legend_box= %d", iost, this_curve -> show_legend_box);
    g_debug ("IODEBUG: %s: legend_box_dash= %d", iost, this_curve -> legend_box_dash);
    g_debug ("IODEBUG: %s: legend_box_thickness= %f", iost, this_curve -> legend_box_thickness);
    g_debug ("IODEBUG: %s: legend_box.red= %f, legend_box.green= %f, legend_box.blue= %f", iost,
             this_curve -> legend_box_color.red,
             this_curve -> legend_box_color.green,
             this_curve -> legend_box_color.blue);
    // Frame
    g_debug ("IODEBUG: %s: show_frame= %d", iost, this_curve -> show_frame);
    g_debug ("IODEBUG: %s: frame_type= %d", iost, this_curve -> frame_type);
    g_debug ("IODEBUG: %s: frame_dash= %d", iost, this_curve -> frame_dash);
    g_debug ("IODEBUG: %s: frame_thickness= %f", iost, this_curve -> frame_thickness);
    g_debug ("IODEBUG: %s: frame.red= %f, frame.green= %f, frame.blue= %f", iost,
             this_curve -> frame_color.red,
             this_curve -> frame_color.green,
             this_curve -> frame_color.blue);
    g_debug ("IODEBUG: %s: frame.xmin= %f, frame.xmax= %f", iost,
             this_curve -> frame_pos[0][0], this_curve -> frame_pos[0][1]);
    g_debug ("IODEBUG: %s: frame.ymin= %f, frame.ymax= %f", iost,
             this_curve -> frame_pos[1][0], this_curve -> frame_pos[1][1]);
    // Data
    g_debug ("IODEBUG: %s: backc.red= %f, backc.green= %f, backc.blue= %f", iost,
             this_curve -> backcolor.red,
             this_curve -> backcolor.green,
             this_curve -> backcolor.blue);
    g_debug ("IODEBUG: %s: legend_font= %s", iost, this_curve -> legend_font);
    if (this_curve -> cfile != NULL)
    {
      g_debug ("IODEBUG: %s: cfile= %s", iost, this_curve -> cfile);
    }
  }
  else
  {
    g_debug ("IODEBUG: %s: No curve window", iost);
  }
}

/*!
  \fn void debug_lattice_info (project * this_proj, gchar * iost)

  \brief debug lattice data

  \param this_proj the target project
  \param iost input or output information
*/
void debug_lattice_info (project * this_proj, gchar * iost)
{
  g_debug ("IODEBUG::%s:: lattice debug", iost);
  cell_info * debug_cell = & this_proj -> cell;
  g_debug ("IODEBUG::%s:: pbc= %d, frac= %d, ltype= %d", iost, debug_cell -> pbc, debug_cell -> frac, debug_cell -> ltype);
  g_debug ("IODEBUG::%s:: npt= %d, has_a_box= %d, crystal= %d", iost, debug_cell -> npt, debug_cell -> has_a_box, debug_cell -> crystal);
  g_debug ("IODEBUG::%s:: volumec= %f, density= %f", iost, debug_cell -> volume, debug_cell -> density);
  g_debug ("IODEBUG::%s:: cextra[0]= %d, cextra[1]= %d, cextra[2]= %d", iost, debug_cell -> cextra[0], debug_cell -> cextra[1], debug_cell -> cextra[2]);
  if (debug_cell -> box)
  {
    box_info * debug_box = debug_cell -> box;
    int i;
    for (i=0; i<2; i++) g_debug ("IODEBUG::%s:: i= %d, param[%d][0]= %f, param[%d][1]= %f, param[%d][2]= %f", iost, i, i, debug_box -> param[i][0], i, debug_box -> param[i][1], i, debug_box -> param[i][2]);
    for (i=0; i<3; i++) g_debug ("IODEBUG::%s:: i= %d, vect[%d][0]= %f, vect[%d][1]= %f, vect[%d][2]= %f", iost, i, i, debug_box -> vect[i][0], i, debug_box -> vect[i][1], i, debug_box -> vect[i][2]);
    for (i=0; i<3; i++) g_debug ("IODEBUG::%s:: i= %d, rvect[%d][0]= %f, rvect[%d][1]= %f, rvect[%d][2]= %f", iost, i, i, debug_box -> rvect[i][0], i, debug_box -> rvect[i][1], i, debug_box -> rvect[i][2]);
  }
}

/*!
  \fn void debugioproj (project * this_proj, gchar * iost)

  \brief debug project info

  \param this_proj the target project
  \param iost input or output information
*/
void debugioproj (project * this_proj, gchar * iost)
{
  int i, j;

  g_debug ("IODEBUG::%s: name= %s", iost, this_proj -> name);
  if (this_proj -> coordfile != NULL)
  {
    g_debug ("IODEBUG::%s: coordfile= %s", iost, this_proj -> coordfile);
    g_debug ("IODEBUG::%s: tfile= %d", iost, this_proj -> tfile);
  }
  if (this_proj -> bondfile != NULL)
  {
    g_debug ("IODEBUG::%s: bondfile= %s", iost, this_proj -> bondfile);
  }
  g_debug ("IODEBUG::%s: newproj= %d", iost, this_proj -> newproj);
  g_debug ("IODEBUG::%s: pbc= %d", iost, this_proj -> cell.pbc);
  g_debug ("IODEBUG::%s: frac= %d", iost, this_proj -> cell.frac);
  g_debug ("IODEBUG::%s: run= %d", iost, this_proj -> run);
  g_debug ("IODEBUG::%s: dmtx= %d", iost, this_proj -> dmtx);
  g_debug ("IODEBUG::%s: nspec= %d", iost, this_proj -> nspec);
  g_debug ("IODEBUG::%s: natomes= %d", iost, this_proj -> natomes);
  g_debug ("IODEBUG::%s: steps= %d", iost, this_proj -> steps);
  g_debug ("IODEBUG::%s: grtotcutoff= %f", iost, this_proj -> chemistry -> grtotcutoff);
  g_debug ("IODEBUG::%s: cvect[0][0]= %f, cvect[0][1]= %f, cvect[0][2]= %f",
           iost, this_proj -> cell.box[0].vect[0][0], this_proj -> cell.box[0].vect[0][1], this_proj -> cell.box[0].vect[0][2]);
  g_debug ("IODEBUG::%s: cvect[1][0]= %f, cvect[1][1]= %f, cvect[1][2]= %f",
           iost, this_proj -> cell.box[0].vect[1][0], this_proj -> cell.box[0].vect[1][1], this_proj -> cell.box[0].vect[1][2]);
  g_debug ("IODEBUG::%s: cvect[2][0]= %f, cvect[2][1]= %f, cvect[2][2]= %f",
           iost, this_proj -> cell.box[0].vect[2][0], this_proj -> cell.box[0].vect[2][1], this_proj -> cell.box[0].vect[2][2]);
  g_debug ("IODEBUG::%s: langles[0]= %f, langles[1]= %f, langles[2]= %f", iost,
                        this_proj -> cell.box[0].param[1][0], this_proj -> cell.box[0].param[1][1], this_proj -> cell.box[0].param[1][2]);
  g_debug ("IODEBUG::%s: vmod[0]= %f, vmod[1]= %f, vmod[2]= %f", iost,
                        this_proj -> cell.box[0].param[0][0], this_proj -> cell.box[0].param[0][1], this_proj -> cell.box[0].param[0][2]);
  for (i=0; i<NGRAPHS; i++)
  {
    g_debug ("IODEBUG::%s: i= %d, visok[i]= %d", iost, i, this_proj -> analysis[i] -> calc_ok);
    g_debug ("IODEBUG::%s: i= %d, initok[i]= %d", iost, i, this_proj -> analysis[i] -> init_ok);
    g_debug ("IODEBUG::%s: i= %d, num_delta[i]= %d", iost, i, this_proj -> analysis[i] -> num_delta);
    g_debug ("IODEBUG::%s: i= %d, delta[i]= %f", iost, i, this_proj -> analysis[i] -> delta);
    g_debug ("IODEBUG::%s: i= %d, min[i]= %f", iost, i, this_proj -> analysis[i] -> min);
    g_debug ("IODEBUG::%s: i= %d, max[i]= %f", iost, i, this_proj -> analysis[i] -> max);
  }
  if (this_proj -> natomes != 0 && this_proj -> nspec != 0)
  {
    for (i=0; i<this_proj -> steps; i++)
    {
      g_debug ("IODEBUG::%s: proj.atom[%d][%d].x= %f, proj.atom[%d][%d].x= %f",
               iost, i, 0, this_proj -> atoms[i][0].x, i, this_proj -> natomes-1, this_proj -> atoms[i][this_proj -> natomes - 1].x);
      g_debug ("IODEBUG::%s: proj.atom[%d][%d].y= %f, proj.atom[%d][%d].y= %f",
               iost, i, 0, this_proj -> atoms[i][0].y, i, this_proj -> natomes - 1, this_proj -> atoms[i][this_proj -> natomes - 1].y);
      g_debug ("IODEBUG::%s: proj.atom[%d][%d].z= %f, proj.atom[%d][%d].z= %f",
               iost, i, 0, this_proj -> atoms[i][0].z, i, this_proj -> natomes - 1, this_proj -> atoms[i][this_proj -> natomes - 1].z);
    }
    for (i=0; i<this_proj -> nspec; i++)
    {
      g_debug ("IODEBUG::%s: i= %d, nsps[i]= %d", iost, i, this_proj -> chemistry -> nsps[i]);
      g_debug ("IODEBUG::%s: i= %d, atid[i]= %d", iost, i, (int)this_proj -> chemistry -> chem_prop[CHEM_Z][i]);
      g_debug ("IODEBUG::%s: i= %d, mass[i]= %f", iost, i, this_proj -> chemistry -> chem_prop[CHEM_M][i]);
      g_debug ("IODEBUG::%s: i= %d, rad[i]= %f", iost, i, this_proj -> chemistry -> chem_prop[CHEM_R][i]);
    }
    for (i=0; i<this_proj -> nspec; i++)
    {
      for (j=0; j<this_proj -> nspec; j++)
      {
        g_debug ("IODEBUG::%s: i= %d, j= %d, cutoffs[i][j]= %f", iost, i, j, this_proj -> chemistry -> cutoffs[i][j]);
      }
    }
  }
  debug_lattice_info (this_proj, iost);
}

/*!
  \fn void debug_chemical_information (project * this_proj)

  \brief debug chemical data info

  \param this_proj the target project
*/
void debug_chemical_information (project * this_proj)
{
  g_debug ("Number of species: %d", this_proj -> nspec);
  int i;
  for (i=0; i<this_proj -> nspec; i++)
  {
    g_debug ("Spec= %d, label= %s, element= %s ", i+1,
             this_proj -> chemistry -> label[i],
             this_proj -> chemistry -> element[i]);
    g_debug ("   nsps[%d]= %d, formula[%d]= %d",
             i+1, this_proj -> chemistry -> nsps[i], i+1, this_proj -> chemistry -> formula[i]);
  }
  g_debug ("Number of atoms: %d", this_proj -> natomes);
  for (i=0; i<this_proj -> natomes; i++)
  {
    g_debug (" at= %d, lot[%d]= %d", i+1, i+1, this_proj -> atoms[0][i].sp);
  }
}

