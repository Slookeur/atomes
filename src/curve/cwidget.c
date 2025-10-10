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
* @file cwidget.c
* @short Initialization of the curve widget
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cwidget.c'
*
* Contains:
*

 - The initialization of the curve widget

*
* List of functions:

  void curve_default_scale (int rid, int cid);
  void initcurve (project * pid, int rid, int cid);
  void addcurwidgets (int pid, int rid, int str);

  DataLayout * curve_default_layout (project * pid, int rid, int cid);

*/

#include <gtk/gtk.h>
#include <stdlib.h>

#include "global.h"
#include "interface.h"
#include "curve.h"

/*!
  \fn DataLayout * curve_default_layout (project * pid, int rid, int cid)

  \brief prepare the default layout for a curve

  \param pid the project id
  \param rid the analysis id
  \param cid the curve id
*/
DataLayout * curve_default_layout (project * pid, int rid, int cid)
{
  DataLayout * layout = g_malloc0 (sizeof*layout);
  layout -> datacolor.red = RED;
  layout -> datacolor.green = GREEN;
  layout -> datacolor.blue = BLUE;
  layout -> datacolor.alpha = 1.0;
  layout -> thickness = DTHICK;
  layout -> hwidth = (rid == SP) ? 1.0 : pid -> analysis[rid].delta;
  layout -> hopac = 0.25;
  layout -> hpos = 1;
  layout -> dash = 1;
  layout -> gfreq = 1;
  if (rid < RI)
  {
    layout -> aspect = 0;
    layout -> glyph = 0;
    layout -> gsize = 10;
  }
  else if (rid == RI)
  {
    if ( cid%4 == 0 || cid%4 == 1 )
    {
      layout -> aspect = 1;
      layout -> glyph = 0;
      layout -> gsize = 10;
    }
    else
    {
      layout -> aspect = 0;
      layout -> glyph = 13;
      layout -> gsize = 5.0;
    }
  }
  else if (rid < MS)
  {
    layout -> aspect = 1;
    layout -> glyph = 0;
    layout -> gsize = 10;
  }
  else
  {
    layout -> aspect = 0;
    layout -> glyph = 0;
    layout -> gsize = 10;
  }
  return layout;
}

/*!
  \fn void curve_default_scale (int rid, int cid)

  \brief pick appropriate scale based on the type of analysis

  \param rid analysis id
  \param cid curve id
*/
void curve_default_scale (int rid, int cid)
{
#ifdef NEW_ANA
  if (rid < RI || rid == MS)
  {
    active_project -> analysis[rid].curves[cid] -> cmin[0] = active_project -> min[rid];
    active_project -> analysis[rid].curves[cid] -> cmax[0] = active_project -> max[rid];
  }
  else
  {
    active_project -> analysis[rid].curves[cid] -> cmin[0] = 1.0;
    active_project -> analysis[rid].curves[cid] -> cmax[0] = active_project -> analysis[rid].curves[cid] -> ndata;
  }

  if (rid < MS)
  {
    active_project -> analysis[rid].curves[cid] -> scale[0] = 0;
    active_project -> analysis[rid].curves[cid] -> scale[1] = 0;
  }
  else
  {
    if (cid < active_project -> numc[MS] - 6)
    {
      active_project -> analysis[rid].curves[cid] -> scale[0] = 1;
      active_project -> analysis[rid].curves[cid] -> scale[1] = 1;
    }
    else
    {
      active_project -> analysis[rid].curves[cid] -> scale[0] = 0;
      active_project -> analysis[rid].curves[cid] -> scale[1] = 0;
    }
  }
#else
  if (rid < RI || rid == MS)
  {
    active_project -> curves[rid][cid] -> cmin[0] = active_project -> min[rid];
    active_project -> curves[rid][cid] -> cmax[0] = active_project -> max[rid];
  }
  else
  {
    active_project -> curves[rid][cid] -> cmin[0] = 1.0;
    active_project -> curves[rid][cid] -> cmax[0] = active_project -> curves[rid][cid] -> ndata;
  }

  if (rid < MS)
  {
    active_project -> curves[rid][cid] -> scale[0] = 0;
    active_project -> curves[rid][cid] -> scale[1] = 0;
  }
  else
  {
    if (cid < active_project -> numc[MS] - 6)
    {
      active_project -> curves[rid][cid] -> scale[0] = 1;
      active_project -> curves[rid][cid] -> scale[1] = 1;
    }
    else
    {
      active_project -> curves[rid][cid] -> scale[0] = 0;
      active_project -> curves[rid][cid] -> scale[1] = 0;
    }
  }
#endif
}

/*!
  \fn void initcurve (project * pid, int rid, int cid)

  \brief initialize curve widget

  \param pid the project id
  \param rid the analysis id
  \param cid the curve id
*/
void initcurve (project * pid, int rid, int cid)
{
  int k;
#ifdef NEW_ANA
  Curve * this_curve = pid -> analysis[rid].curve[cid];
  this_curve -> window = NULL;
  this_curve -> plot = NULL;
  this_curve -> wsize[0] = 800;
  this_curve -> wsize[1] = 600;
  this_curve -> show_title = FALSE;
  this_curve -> default_title = TRUE;
  this_curve -> title_font = g_strdup_printf ("Sans Bold 12");
  this_curve -> title_pos[0] = 0.4;
  this_curve -> title_pos[1] = 0.05;
  this_curve -> title_color.red = 0.0;
  this_curve -> title_color.blue = 0.0;
  this_curve -> title_color.green = 0.0;
  this_curve -> title_color.alpha = 1.0;
  this_curve -> format = 0;
  for (k=0 ; k<2; k++)
  {
    if (this_curve -> data[k] != NULL)
    {
      g_free (this_curve -> data[k]);
      this_curve -> data[k] = NULL;
    }
    this_curve -> autoscale[k] = TRUE;
    this_curve -> show_grid[k] = FALSE;
    this_curve -> show_axis[k] = TRUE;
    this_curve -> labels_digit[k] = 1;
    this_curve -> ticks_io[k] = 0;
    this_curve -> labels_angle[k] = 0.0;
    this_curve -> labels_font[k] = g_strdup_printf ("Sans 12");
    this_curve -> mint_size[k] = 5;
    this_curve -> majt_size[k] = 10;
    this_curve -> axis_defaut_title[k] = TRUE;
    this_curve -> axis_title_font[k] = g_strdup_printf ("Sans 12");
  }
  if (this_curve -> err != NULL)
  {
    g_free (this_curve -> err);
    this_curve -> err = NULL;
  }
  this_curve -> labels_shift_x[0] = 10;
  this_curve -> labels_shift_y[0] = 20;
  this_curve -> labels_shift_x[1] = 50;
  this_curve -> labels_shift_y[1] = 10;
  this_curve -> axis_title_x[0] = -20;
  this_curve -> axis_title_y[0] = 45;
  this_curve -> axis_title_x[1] = MARGX - 10;
  this_curve -> axis_title_y[1] = -50;
  this_curve -> frame_type = 2;
  this_curve -> frame_dash = 1;
  this_curve -> frame_thickness = 1.0;
  this_curve -> frame_color.red = 0.0;
  this_curve -> frame_color.green = 0.0;
  this_curve -> frame_color.blue = 0.0;
  this_curve -> frame_color.alpha = 1.0;
  this_curve -> frame_pos[0][0] = 100.0/840.0;
  this_curve -> frame_pos[0][1] = 1.0;
  this_curve -> frame_pos[1][0] = 530.0/600.0;
  this_curve -> frame_pos[1][1] = 0.0;
  this_curve -> legend_font = g_strdup_printf ("Sans 10");
  this_curve -> show_legend = FALSE;
  this_curve -> show_frame = TRUE;
  this_curve -> legend_color.red = 0.0;
  this_curve -> legend_color.blue = 0.0;
  this_curve -> legend_color.green = 0.0;
  this_curve -> legend_color.alpha = 1.0;
  this_curve -> legend_pos[0] = LEGX;
  this_curve -> legend_pos[1] = LEGY;
  this_curve -> show_legend_box = FALSE;
  this_curve -> legend_box_dash = 1;
  this_curve -> legend_box_color.red = 0.0;
  this_curve -> legend_box_color.green = 0.0;
  this_curve -> legend_box_color.blue = 0.0;
  this_curve -> legend_box_color.alpha = 1.0;
  this_curve -> legend_box_thickness = 1.0;
  this_curve -> backcolor.red =  1.0;
  this_curve -> backcolor.green = 1.0;
  this_curve -> backcolor.blue = 1.0;
  this_curve -> backcolor.alpha = 1.0;
  this_curve -> layout = curve_default_layout (pid, rid, cid);
  this_curve -> extrac = NULL;
  this_curve -> extrac = g_malloc0 (sizeof*this_curve -> extrac);
  this_curve -> extrac -> extras = 0;
  if (this_curve -> cfile != NULL)
  {
    g_free (this_curve -> cfile);
  }
#else
  pid -> curves[rid][cid] -> window = NULL;
  pid -> curves[rid][cid] -> plot = NULL;
  pid -> curves[rid][cid] -> wsize[0] = 800;
  pid -> curves[rid][cid] -> wsize[1] = 600;
  pid -> curves[rid][cid] -> show_title = FALSE;
  pid -> curves[rid][cid] -> default_title = TRUE;
  pid -> curves[rid][cid] -> title_font = g_strdup_printf ("Sans Bold 12");
  pid -> curves[rid][cid] -> title_pos[0] = 0.4;
  pid -> curves[rid][cid] -> title_pos[1] = 0.05;
  pid -> curves[rid][cid] -> title_color.red = 0.0;
  pid -> curves[rid][cid] -> title_color.blue = 0.0;
  pid -> curves[rid][cid] -> title_color.green = 0.0;
  pid -> curves[rid][cid] -> title_color.alpha = 1.0;
  pid -> curves[rid][cid] -> format = 0;
  for (k=0 ; k<2; k++)
  {
    if (pid -> curves[rid][cid] -> data[k] != NULL)
    {
      g_free (pid -> curves[rid][cid] -> data[k]);
      pid -> curves[rid][cid] -> data[k] = NULL;
    }
    pid -> curves[rid][cid] -> autoscale[k] = TRUE;
    pid -> curves[rid][cid] -> show_grid[k] = FALSE;
    pid -> curves[rid][cid] -> show_axis[k] = TRUE;
    pid -> curves[rid][cid] -> labels_digit[k] = 1;
    pid -> curves[rid][cid] -> ticks_io[k] = 0;
    pid -> curves[rid][cid] -> labels_angle[k] = 0.0;
    pid -> curves[rid][cid] -> labels_font[k] = g_strdup_printf ("Sans 12");
    pid -> curves[rid][cid] -> mint_size[k] = 5;
    pid -> curves[rid][cid] -> majt_size[k] = 10;
    pid -> curves[rid][cid] -> axis_defaut_title[k] = TRUE;
    pid -> curves[rid][cid] -> axis_title_font[k] = g_strdup_printf ("Sans 12");
  }
  if (pid -> curves[rid][cid] -> err != NULL)
  {
    g_free (pid -> curves[rid][cid] -> err);
    pid -> curves[rid][cid] -> err = NULL;
  }
  pid -> curves[rid][cid] -> labels_shift_x[0] = 10;
  pid -> curves[rid][cid] -> labels_shift_y[0] = 20;
  pid -> curves[rid][cid] -> labels_shift_x[1] = 50;
  pid -> curves[rid][cid] -> labels_shift_y[1] = 10;
  pid -> curves[rid][cid] -> axis_title_x[0] = -20;
  pid -> curves[rid][cid] -> axis_title_y[0] = 45;
  pid -> curves[rid][cid] -> axis_title_x[1] = MARGX - 10;
  pid -> curves[rid][cid] -> axis_title_y[1] = -50;
  pid -> curves[rid][cid] -> frame_type = 2;
  pid -> curves[rid][cid] -> frame_dash = 1;
  pid -> curves[rid][cid] -> frame_thickness = 1.0;
  pid -> curves[rid][cid] -> frame_color.red = 0.0;
  pid -> curves[rid][cid] -> frame_color.green = 0.0;
  pid -> curves[rid][cid] -> frame_color.blue = 0.0;
  pid -> curves[rid][cid] -> frame_color.alpha = 1.0;
  pid -> curves[rid][cid] -> frame_pos[0][0] = 100.0/840.0;
  pid -> curves[rid][cid] -> frame_pos[0][1] = 1.0;
  pid -> curves[rid][cid] -> frame_pos[1][0] = 530.0/600.0;
  pid -> curves[rid][cid] -> frame_pos[1][1] = 0.0;
  pid -> curves[rid][cid] -> legend_font = g_strdup_printf ("Sans 10");
  pid -> curves[rid][cid] -> show_legend = FALSE;
  pid -> curves[rid][cid] -> show_frame = TRUE;
  pid -> curves[rid][cid] -> legend_color.red = 0.0;
  pid -> curves[rid][cid] -> legend_color.blue = 0.0;
  pid -> curves[rid][cid] -> legend_color.green = 0.0;
  pid -> curves[rid][cid] -> legend_color.alpha = 1.0;
  pid -> curves[rid][cid] -> legend_pos[0] = LEGX;
  pid -> curves[rid][cid] -> legend_pos[1] = LEGY;
  pid -> curves[rid][cid] -> show_legend_box = FALSE;
  pid -> curves[rid][cid] -> legend_box_dash = 1;
  pid -> curves[rid][cid] -> legend_box_color.red = 0.0;
  pid -> curves[rid][cid] -> legend_box_color.green = 0.0;
  pid -> curves[rid][cid] -> legend_box_color.blue = 0.0;
  pid -> curves[rid][cid] -> legend_box_color.alpha = 1.0;
  pid -> curves[rid][cid] -> legend_box_thickness = 1.0;
  pid -> curves[rid][cid] -> backcolor.red =  1.0;
  pid -> curves[rid][cid] -> backcolor.green = 1.0;
  pid -> curves[rid][cid] -> backcolor.blue = 1.0;
  pid -> curves[rid][cid] -> backcolor.alpha = 1.0;
  pid -> curves[rid][cid] -> layout = curve_default_layout (pid, rid, cid);
  pid -> curves[rid][cid] -> extrac = NULL;
  pid -> curves[rid][cid] -> extrac = g_malloc0 (sizeof*pid -> curves[rid][cid] -> extrac);
  pid -> curves[rid][cid] -> extrac -> extras = 0;
  if (pid -> curves[rid][cid] -> cfile != NULL)
  {
    g_free (pid -> curves[rid][cid] -> cfile);
  }
#endif
  activer = -1;
  curve_default_scale (rid, cid);
  activer = rid;
}

/*!
  \fn void addcurwidgets (int pid, int rid, int str)

  \brief add curve widgets to the project

  \param pid the project id
  \param rid the analysis id
  \param str at the project creation stage (1) or latter on (0)
*/
void addcurwidgets (int pid, int rid, int str)
{
  int j, k;
  activer = rid;
  project * tmp_proj = get_project_by_id(pid);
#ifdef NEW_ANA
  for (j=0; j<tmp_proj -> analysis[rid].numc; j++)
  {
    tmp_proj -> analysis[rid].curves[j] -> cid = j;
    tmp_proj -> analysis[rid].idcc[j].a = pid;
    tmp_proj -> analysis[rid].idcc[j].b = rid;
    tmp_proj -> analysis[rid].idcc[j].c = j;
    if (str == 0 || tmp_proj -> analysis[rid].curves[j] -> ndata == 0)
    {
      initcurve (tmp_proj, rid, j);
    }
    if (tmp_proj -> analysis[rid].curves[j] -> default_title)
    {
      tmp_proj -> analysis[rid].curves[j] -> title = g_strdup_printf ("%s - %s", prepare_for_title(tmp_proj -> name), tmp_proj -> analysis[rid].curves[j] -> name);
    }
    for (k=0; k<2; k++)
    {
      if (tmp_proj -> analysis[rid].curves[j] -> axis_defaut_title[k])
      {
        tmp_proj -> analysis[rid].curves[j] -> axis_title[k] = g_strdup_printf ("%s", default_title(k, j));
      }
    }
  }
#else
  int l = 0;
  for (j=0; j<rid; j++)
  {
    l += tmp_proj -> numc[j];
  }
  for (j=0; j<tmp_proj -> numc[rid]; j++)
  {
    tmp_proj -> curves[rid][j] -> cid = l + j;
    tmp_proj -> idcc[rid][j].a = pid;
    tmp_proj -> idcc[rid][j].b = rid;
    tmp_proj -> idcc[rid][j].c = j;
    if (str == 0 || tmp_proj -> curves[rid][j] -> ndata == 0)
    {
      initcurve (tmp_proj, rid, j);
    }
    if (tmp_proj -> curves[rid][j] -> default_title)
    {
      tmp_proj -> curves[rid][j] -> title = g_strdup_printf ("%s - %s", prepare_for_title(tmp_proj -> name), tmp_proj -> curves[rid][j] -> name);
    }
    for (k=0; k<2; k++)
    {
      if (tmp_proj -> curves[rid][j] -> axis_defaut_title[k])
      {
        tmp_proj -> curves[rid][j] -> axis_title[k] = g_strdup_printf ("%s", default_title(k, j));
      }
    }
  }
#endif
}
