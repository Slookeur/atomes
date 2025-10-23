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
* @file curve.h
* @short Variable declarations for the curve widget \n
         Functions for interactions with the curve widget
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'curve.h'
*
* Contains:

 - Variable declarations for the curve widget
 - Functions for interactions with the curve widget

*/

#ifndef CURVE_H_
#define CURVE_H_

#include "global.h"

// Grid lines width in pixels
#define GRIDSIZE 0.1
// Default position of the legend on the curve
#define LEGX 0.65
#define LEGY 0.15
// Default color of the data from 0 to 65535
#define RED 1.0
#define GREEN 0.0
#define BLUE 0.0
// Default linewidth of the data line
#define DTHICK 1.0
// Number of dash types
#define NDASHS 11
// Number of glyph types
#define NGLYPHS 37
// The marges between the windows border and the axes
#define MARGX 100
#define MARGY 70

extern GtkWidget * axischoice;
extern GtkWidget * vmin, * vmax;
extern GtkWidget * majt;
extern GtkWidget * nmi[2];
extern GtkWidget * xyp[2];
extern int resol[2];
extern int originp;
extern int ndash;
extern int nglyph;
extern double back_alpha;

typedef struct curve_dash curve_dash;
struct curve_dash
{
  const double * a;
  int b;
};

extern gint32 etime;
extern double XDRAW, YDRAW;
extern double DXDRAW, DYDRAW;
extern double xmax, ymax;
extern double x_min, x_max;
extern double y_max, y_min;
extern double ax, ay;
extern double cxy[2];
extern double mticks;
extern int nticks;
extern int xlog, ylog;
extern int dxlog, dylog;
extern char * sca;

extern PangoLayout * layout;
extern gboolean dogrid;
extern int x_shift, y_shift;
extern int amint, amajt;
extern int tickpos, labpos;

// Marge sur X et Y dans les graphs
extern int xmarge;
extern int ymarge;
extern char * curve_image_file;

extern int get_curve_shift (Curve * this_curve);

// Number of dash formats
extern int ndash;
extern int nglyph;
extern const double dashed1[];
extern int len1;
extern const double pdashed[];
extern int lenp;

extern curve_dash * selectdash (int iddash);
extern double scale (double axe);
extern Curve * get_curve_from_pointer (gpointer data);
extern void prep_plot (Curve * this_curve);
extern void hide_curves (project * this_proj, int c);
extern void erase_curves (project * this_proj, int c);
extern void update_curves ();
extern void update_curve (gpointer curve);

#ifdef GTK3
extern gboolean show_curve (GtkWidget * grwdget, cairo_t * cr, gpointer curve);
#else
extern void show_curve (GtkDrawingArea * area, cairo_t * cr, int width, int height, gpointer curve);
#endif
extern void hide_curve (gpointer data);
#ifdef GTK3
extern G_MODULE_EXPORT gboolean to_hide_curve (GtkWidget * thecurve, GdkEvent * event, gpointer data);
#else
extern G_MODULE_EXPORT gboolean to_hide_curve (GtkWindow * thecurve, gpointer data);
#endif

extern void clean_curves_data (int calc, int start, int end);
extern void initcurve (project * pid, int rid, int cid);
extern void add_curve_widgets (int pid, int rid, int st);
extern void allocextra (int a, int b, int c);

extern void label_curve (cairo_t * cr, double val, int axe, int p, Curve * this_curve);

extern void show_frame (cairo_t * cd,
                        int tf, int da, int res[2],
                        double ti, double x[2], double y[2],
                        ColRGBA dcol);
extern void prep_frame (cairo_t * fr, int da, double ti, ColRGBA dcol);
extern void draw_frame (cairo_t * cr, Curve * this_curve, int rid, int cid);

extern void draw_glyph (cairo_t * in, int theglyph, double x, double y, ColRGBA gcolor, double size);

extern const gchar * default_title (int ax, gpointer data);
extern void show_title (cairo_t * cr, Curve * this_curve);

extern void autoscale_axis (project * this_proj, Curve * this_curve, int rid, int cid, int aid);
extern void setup_xaxis_linear (cairo_t * cr, Curve * this_curve);
extern void setup_xaxis_log (cairo_t * cr, Curve * this_curve, int rid, int cid, gboolean draw_it);
extern void setup_yaxis_linear (cairo_t * cr, Curve * this_curve);
extern void setup_yaxis_log (cairo_t * cr, Curve * this_curve, gboolean draw_it);

extern void write_curve (gpointer idata);
extern void save_image (gpointer cdata);
extern void remove_extra (ExtraSets * sets, CurveExtra * ctmp);
extern void curve_window_add_menu_bar (tint * data);
extern GtkWidget * curve_popup_menu (gpointer data);
extern void show_curve_popup_menu (GdkEvent * event, gpointer data);

void draw_curve (cairo_t * cr,
                 int cid,
                 int rid,
                 project * this_proj,
                 int points,
                 ColRGBA withcolor,
                 int xscale,
                 int yscale,
                 int asp,
                 int vdash,
                 double thick,
                 int glyp,
                 double gize,
                 int freq,
                 double hwidth,
                 double hopac,
                 int hpos,
                 int extra,
                 int pid);

extern void show_legend (cairo_t * cr, project * this_proj, int rid, int cid);
#endif
