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
* @file cedit.h
* @short Variable declarations for the curve layout edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'cedit.h'
*
* Contains:

 - Variable declarations for the curve layout edition window

*/

#ifndef CEDIT_H_
#define CEDIT_H_

typedef struct curve_edition curve_edition;
struct curve_edition
{
  GtkWidget * win;
  tint prc;
  GtkTreePath ** ppath;
  GtkTreePath ** cpath;
  GtkWidget * projtree;
  GtkWidget * xyp[2];
  // tab 1
  GtkWidget * title_box;
  GtkWidget * frame[11];
  GtkWidget * frame_box;
  GtkWidget * frame_style_area;
  GtkWidget * frame_pix_box;
  qint dataxe[2];
  qint framxe[4];
  GtkWidget * custom_title;
  // tab 2
  GtkWidget * setcolorbox;
  GtkWidget * thesetbox;
  GtkWidget * data_color;
  GtkWidget * data_aspect;
  GtkWidget * data_thickness;
  GtkWidget * data_glyph;
  GtkWidget * data_glyph_size;
  GtkWidget * data_glyph_freq;
  GtkWidget * data_hist_width;
  GtkWidget * data_hist_opac;
  GtkWidget * data_hist_pos;
  GtkWidget * data_dash;
  GtkWidget * stylearea;
  GtkWidget * pixarea;
  GtkWidget * Glyph_box;
  GtkWidget * Hist_box;
  GtkWidget * xyl[2];
  GtkWidget * orgtree;
  GtkWidget * datascroll;
  // tab 3
  GtkWidget * legend_box;
  GtkWidget * legend_box_style;
  GtkWidget * legend_style_area;
  // tab 4
  GtkWidget * axischoice;
  GtkWidget * scale_box;
  GtkWidget * ticks_inout_box;
  GtkWidget * ticks_pos_box;
  GtkWidget * labels_pos_box;
  GtkWidget * ticks_labels_angle;
  GtkWidget * ticks_labels_font;
  GtkWidget * show_axis;
  GtkWidget * show_grid;
  GtkWidget * auto_scale;
  GtkWidget * axis_default_title;
  GtkWidget * axis_title;
  GtkWidget * axis_title_font;
  GtkWidget * majt;
  GtkWidget * vmin;
  GtkWidget * vmax;
  GtkWidget * nmi[2], * ndi[2];
  GtkWidget * mats[2];
  GtkWidget * mits[2];
  GtkWidget * nptx[2], * npty[2];
  GtkWidget * tptx[2], * tpty[2];
  int handler_id;
};

extern int activeg;
extern int activec;
extern int activer;

extern char * lapos[2];
extern char * ctext[2];

void prepbox (gpointer data);
void edit_curve (gpointer cdata);
#endif
