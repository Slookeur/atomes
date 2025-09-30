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
* @file global.h
* @short Global variable declarations \n
         Global convenience function declarations \n
         Global data structure definitions
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'global.h'
*
* Contains:

 - Global variable declarations
 - Global convenience function declarations
 - Global data structure definitions

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define GDK_SYNCHRONIZE

#ifndef GLOBAL_H_

#define GLOBAL_H_

#include <glib.h>
#include <glib/gi18n.h>
#include <locale.h>
#ifdef OSX
#include <xlocale.h>
#endif
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include <gtk/gtk.h>
#ifndef GTK4
#  include <gtk/gtkx.h>
#endif
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <pango/pangoft2.h>

#ifdef MAC_INTEGRATION
#  include <gtkosxapplication.h>
#endif

#ifdef G_OS_WIN32
#  include <windef.h>
#  include <windows.h>
#  define WIN32_LEAN_AND_MEAN 1
#  include <epoxy/gl.h>
#  include <GL/glu.h>
#else
#  define max(a,b) (a>=b?a:b)
#  define min(a,b) (a<=b?a:b)
#  include <epoxy/gl.h>
#  include <GL/glu.h>
#  ifdef OSX
#    include <GL/glx.h>
#  else
#    include <epoxy/glx.h>
#  endif
#endif

#define MY_ENCODING "UTF-8"

#include "math_3d.h"

// dint, tint and qint structures are used for pointer purposes.
// dint, tint, qint, ColRGBA and coord_info structures must be defined before including 'glwin.h'
typedef struct dint dint;
struct dint
{
  int a;
  int b;
};

typedef struct tint tint;
struct tint
{
  int a;
  int b;
  int c;
};

typedef struct qint qint;
struct qint
{
  int a;
  int b;
  int c;
  int d;
};

typedef struct ColRGBA  ColRGBA;
struct ColRGBA
{
  float red;
  float green;
  float blue;
  float alpha;
};

/*! \typedef shortcuts

  \brief keyboard shortcuts information
*/
typedef struct shortcuts shortcuts;
struct shortcuts
{
  gchar * description;                                       /*!< Shortcut description */
  gchar * subtitle;                                          /*!< Shortcut subtitle */
  gint key;                                                  /*!< Shortcut key */
  gchar * accelerator;                                       /*!< Shortcut accelerator */
};

/*! \typedef atomes_action

  \brief action information data structure
*/
typedef struct atomes_action atomes_action;
struct atomes_action
{
  gchar * action_name;                                       /*!< The name of the action */
  gpointer action_data;                                      /*!< The associated data pointer */
};

/*! \typedef coord_info

  \brief coordination information data structure
*/
typedef struct coord_info coord_info;
struct coord_info
{
  int species;            /*!< The number of chemical species */
  // 0 = tot
  // 1 = partial
  // 2 = fragments
  // 3 = molecules
  // > 3 = rings
  // 9 = chains
  int totcoord[10];       /*!< The total number of objects: \n
                               0 = total coordination(s), \n
                               1 = partial coordination(s), \n
                               2 = fragment(s), \n
                               3 = molecule(s), \n
                               4 to 8 = ring statistics, \n
                               9 = chain statistics */
  int * ntg[10];          /*!< The number of objects by chemical species */
  int ** geolist[10];     /*!< The corresponding list of geometries */
  int *** partial_geo;    /*!< The list of partial geometries */
  int cmin;               /*!< The minimum coordination number */
  int cmax;               /*!< The Maximum coordination number */
};

#include "glwin.h"

#define MEDIA_NEXT "media-skip-forward"
#define MEDIA_PREV "media-skip-backward"
#define MEDIA_LAST "go-last"
#define MEDIA_FIRST "go-first"
#define MEDIA_GOTO "go-jump"
#define MEDIA_PLAY "media-playback-start"
#define MEDIA_STOP "media-playback-stop"
#define MEDIA_LOOP "view-refresh"
#define MEDIA_ULOOP "edit-cut"
#define MEDIA_SLOW "view-sort-ascending"
#define MEDIA_FAST "view-sort-descending"
#define GO_RIGHT "go-next"
#define GO_LEFT "go-previous"
#define GO_UP "go-up"
#define GO_DOWN "go-down"
#define PAGE_SETUP "document-page-setup"
#define FULLSCREEN "view-fullscreen"
#define FITBEST "zoom-fit-best"
#define RECORD "media-record"
#define YES "gtk-yes"
#define EDITA "edit-find-replace"
#define EDITC "edit-copy"
#define EDITF "edit-find"
#define ECUT "edit-cut"
#define DPROPERTIES "document-properties"
#define FNEW "document-new"
#define FOPEN "document-open"
#define FSAVE "document-save"
#define FSAVEAS "document-save-as"
#define FCLOSE "window-close"
#define FEXIT "application-exit"
#define LIST_ADD "list-add"
#define LIST_REM "list-remove"
#define CANCEL "process-stop"
#define EXECUTE "system-run"
#ifdef GTK4
#define APPLY "emblem-default"
#else
#define APPLY "gtk-apply"
#endif
#define DELETEB "edit-delete"
#define DIAL_ERROR "dialog-error"
#define AFILE "text-x-generic"
#define ABOUT "help-about"

#ifdef GTK4
/*! \enum ReliefStyle */
enum ReliefStyle {
  GTK_RELIEF_NORMAL = 0, /*!< 0 */
  GTK_RELIEF_HALF   = 1, /*!< 1 */
  GTK_RELIEF_NONE   = 2  /*!< 2 */
};

/*! \enum ShadowStyle */
enum ShadowStyle {
  GTK_SHADOW_NONE       = 0, /*!< 0 */
  GTK_SHADOW_IN         = 1, /*!< 1 */
  GTK_SHADOW_OUT        = 2, /*!< 2 */
  GTK_SHADOW_ETCHED_IN  = 3, /*!< 3 */
  GTK_SHADOW_ETCHED_OUT = 4  /*!< 4 */
};
#define BSEP 3
#else
#define BSEP 0
#endif

/*! \enum ContainerType */
enum ContainerType {
  CONTAINER_WIN = 0, /*!< 0 */
  CONTAINER_SCR = 1, /*!< 1 */
  CONTAINER_VIE = 2, /*!< 2 */
  CONTAINER_BUT = 3, /*!< 3 */
  CONTAINER_FRA = 4, /*!< 4 */
  CONTAINER_EXP = 5  /*!< 5 */
};

/*! \enum ImageFormats */
enum ImageFormats {
  IMG_NONE    = 0, /*!< 0 */
  IMG_PIXBUF  = 1, /*!< 1 */
  IMG_SURFACE = 2, /*!< 2 */
  IMG_FILE    = 3, /*!< 3 */
  IMG_STOCK   = 4  /*!< 4 */
};

#define IODEBUG FALSE

/*! \def ATOM_LIMIT
  \brief atom number limit to compute fragment(s) and molecule(s) analysis automatically
*/
#define ATOM_LIMIT 100000

/*!< \def STEP_LIMIT
  \brief MD step number limit to compute fragment(s) and molecule(s) analysis automatically
*/
#define STEP_LIMIT 10000

#define OK            0
#define ERROR_RW      1
#define ERROR_PROJECT 2
#define ERROR_CURVE   3
#define ERROR_IMAGE   4
#define ERROR_ATOM_A  5
#define ERROR_ATOM_B  6
#define ERROR_UPDATE  7
#define ERROR_NO_WAY  8
#define ERROR_COORD   9
#define ERROR_RINGS  10
#define ERROR_CHAINS 11
#define ERROR_MOL    12

/*!< \def CHEM_PARAMS
  \brief number of chemical parameters
*/
#define CHEM_PARAMS 5
#define CHEM_Z 0
#define CHEM_M 1
#define CHEM_R 2
#define CHEM_N 3
#define CHEM_X 4

#define NDOTS 8

/*!< \def NCALCS
  \brief number of analysis
*/
#define NCALCS 12

/*!< \def NGRAPHS
  \brief number of analysis with results in curve window(s)
*/
#define NGRAPHS 10

/*!< \def NCFORMATS
  \brief number atomic coordinates file formats
*/
#define NCFORMATS 13

#define NITEMS 16
#define OT 4
#define GR 0
#define SQ 1
#define SK 2
#define GK 3
#define BD 4
#define AN 5
#define RI 6
#define CH 7
#define SP 8
#define MS 9
#define BV 10
#define FF 12

#define DEFAULT_FONT_SIZE 8
#define DEFAULT_ALPHA 0.75
#define MINCUT 1.85

#ifdef G_OS_WIN32
extern gchar * PACKAGE_PREFIX;
extern gchar * PACKAGE_LIBEXEC;
#endif
extern gchar * PACKAGE_LIB_DIR;
extern gchar * PACKAGE_IMP;
extern gchar * PACKAGE_CON;
extern gchar * PACKAGE_IMG;
extern gchar * PACKAGE_PDF;
extern gchar * PACKAGE_SVG;
extern gchar * PACKAGE_EPS;
extern gchar * PACKAGE_PNG;
extern gchar * PACKAGE_JPG;
extern gchar * PACKAGE_BMP;
extern gchar * PACKAGE_TIFF;
extern gchar * PACKAGE_VOID;
extern gchar * PACKAGE_GR;
extern gchar * PACKAGE_SQ;
extern gchar * PACKAGE_BD;
extern gchar * PACKAGE_AN;
extern gchar * PACKAGE_RI;
extern gchar * PACKAGE_CH;
extern gchar * PACKAGE_SP;
extern gchar * PACKAGE_MS;
extern gchar * PACKAGE_TD;
extern gchar * PACKAGE_MOL;
extern gchar * PACKAGE_OGL;
extern gchar * PACKAGE_OGLM;
extern gchar * PACKAGE_OGLC;
extern gchar * PACKAGE_PRO;
extern gchar * PACKAGE_SET;
extern gchar * PACKAGE_LOGO;
extern gchar * PACKAGE_LAGPL;
extern gchar * PACKAGE_LABOUT;
extern gchar * PACKAGE_DOTA;
extern gchar * PACKAGE_DOTB;
extern gchar * PACKAGE_DOTC;
extern gchar * PACKAGE_DOTD;
extern gchar * PACKAGE_DOTE;
extern gchar * PACKAGE_DOTF;
extern gchar * PACKAGE_DOTG;
extern gchar * PACKAGE_DOTH;
extern gchar * PACKAGE_DFBD;
extern gchar * PACKAGE_DFAN;
extern gchar * PACKAGE_DFDI;
extern gchar * PACKAGE_DFTD;
extern gchar * PACKAGE_DFIN;
extern gchar * PACKAGE_SGCP;
extern gchar * PACKAGE_SGCI;
extern gchar * PACKAGE_SGCF;
extern gchar * PACKAGE_SGHP;
extern gchar * PACKAGE_SGTR;
extern gchar * PACKAGE_SGTI;
extern gchar * PACKAGE_SGTP;
extern gchar * PACKAGE_SGOP;
extern gchar * PACKAGE_SGOI;
extern gchar * PACKAGE_SGOC;
extern gchar * PACKAGE_SGOF;
extern gchar * PACKAGE_SGMP;
extern gchar * PACKAGE_SGMI;
extern gchar * PACKAGE_SGTC;
extern gchar * ATOMES_CONFIG_DIR;
extern gchar * ATOMES_CONFIG;
extern gchar * ATOMES_URL;

extern gchar * mode_name[2];
extern gchar * graph_img[NGRAPHS];
extern gchar * dots[NDOTS];
extern gchar * bravais_img[14];
extern gchar * ifield[8];
extern gchar * projfile;
extern char * ifbug;
extern char * coord_files[NCFORMATS+1];
extern char * coord_files_ext[NCFORMATS+1];
extern char * calc_name[NCALCS-2];
extern char * graph_name[NGRAPHS] ;
extern char * rings_type[5];
extern char * untime[5];
extern gchar * workspacefile;

extern int nprojects;
extern int activep;
extern int activev;
extern int activef;
extern int inactep;
extern int activew;
extern int statusval;
extern int atomes_visual;
extern int dialog_id;

extern int bonds_update;
extern int frag_update;
extern int mol_update;

extern int tmp_pixels[2];
extern int * pasted_todo;
extern atomic_object * copied_object;

extern GMainLoop * Event_loop[5];

extern gboolean in_movie_encoding;
extern gboolean newspace;
extern gboolean reading_input;
extern gboolean tmp_adv_bonding[2];
extern gboolean column_label;
extern gboolean check_label;
extern gboolean object_motion;
extern gboolean selected_status;
extern gboolean silent_input;
extern gboolean cif_use_symmetry_positions;

extern struct timespec start_time;
extern struct timespec stop_time;

extern double opac;
extern double pi;

extern GtkWidget * MainWindow;
extern GtkWidget * MainView;
extern GtkWidget * MainFrame[2];
extern GtkWidget * pop;
extern GtkWidget * curvetoolbox;
extern GtkWidget * progressbar;
extern GtkWidget * MainScrol[2];
extern GtkWidget * atomes_logo;
extern GtkWidget * atomes_shortcuts;
extern GtkWidget * calc_dialog;
extern GtkWidget * register_button;

extern GtkTextTag * tag;
extern GtkStatusbar * statusbar;

extern ColRGBA std[6];

extern GdkPixbuf * THETD;
extern GdkPixbuf * THEMO;
extern GdkPixbuf * THEBD;
extern GdkPixbuf * SETTING;
extern GdkPixbuf * SETTINGS;
extern GdkPixbuf * OGL;
extern GdkPixbuf * OGLM;
extern GdkPixbuf * OGLC;
extern GdkPixbuf * RUN;
extern tint cut_sel;

// Data structures
#define LINE_SIZE 160

/*! \typedef line_node */
typedef struct line_node line_node;
struct line_node
{
  gchar * line;              /*!< Size max = LINE_SIZE */
  struct line_node * next;
  struct line_node * prev;
};

/*! \typedef coord_file

  \brief atomic coordinates file, data container
*/
typedef struct coord_file coord_file;
struct coord_file
{
  int natomes;                             /*!< Number of atom(s) */
  int steps;                               /*!< Number of MD step(s) */
  int nspec;                               /*!< Number of chemical species */
  double * z;                              /*!< List of atomic numbers */
  int * nsps;                              /*!< Number of atoms by species */
  double ** coord;                         /*!< Atomic coordinates */
  gboolean cartesian;                      /*!< Cartesian (1) or Fractional coordinates (0) */
  gboolean chemical;                       /*!< CIF file describing chemical reaction */
  int * lot;                               /*!< Chemical species by atom */
  int ndummy;                              /*!< Number of dummy atom(s), if any */
  gchar ** dummy;                          /*!< List of dummy atom(s), if any */
  cell_info lattice;                       /*!< Description of the periodicity */
  int mid;                                 /*!< Message type (0 = error, 1 = warning), if any */
  int msg;                                 /*!< Number of message(s) */
  gchar ** info;                           /*!< Information message(s), if required */
  gchar ** label;                          /*!< VAS or TRJ: list of chemical labels, \n CIF: Label list of mis-labelled object(s) */
  // The following line is only used for DL_POLY history files:
  int traj;                                /*!< */
  // The following lines are only used for CIF files:
  int num_sym_pos;                         /*!< Number of symmetry positions, if any */
  gchar *** sym_pos;                       /*!< The symmetry positions, if any */
  int setting;                             /*!< Space group setting */
  int * wyckoff;                           /*!< Wyckoff positions */
  int rounding;                            /*!< Occupancy rounding */
  double * occupancy;                      /*!< Site(s) occupancy */
  int * multi;                             /*!< Multiplicity */
  int dis;                                 /*!< Number of disorder group */
  int * disorder;                          /*!< Site disorder group */
  int atom_unlabelled;                     /*!< Number of atom(s) unlabelled */
  int * u_atom_list;                       /*!< List of unlabelled atom(s) */
  int object_to_insert;                    /*!< Number of object(s) to label */
  int * object_list;                       /*!< List of object to insert by atom(s) */
};

/*! \typedef MouseState

  \brief data structure to store mouse information on curve widget
*/
typedef struct MouseState MouseState;
struct MouseState
{
  gint start_x;                 /*!< Initial x position */
  gint start_y;                 /*!< Initial y position */
  gint time;                    /*!< Time */
  gboolean MouseIsDown;         /*!< Is the mouse button up (0) or down (1) */
};

/*! \typedef CurveState

  \brief data structure used for zoom in / out on curve widget
*/
typedef struct CurveState CurveState;
struct CurveState
{
  MouseState mouseState;        /*!< Mouse status information */
  tint * id;                    /*!< Curve data pointer */
};

/*! \typedef DataLayout

  \brief curve layout information
*/
typedef struct DataLayout DataLayout;
struct DataLayout
{
  ColRGBA datacolor;             /*!< Data color */
  double thickness;              /*!< Data line thickness */
  int dash;                      /*!< Data line style */
  int glyph;                     /*!< Data glyph type */
  double gsize;                  /*!< Data glyph size */
  int gfreq;                     /*!< Data glyph frequency */
  int aspect;                    /*!< X/Y or histogram */
  double hwidth;                 /*!< Histogram width */
  double hopac;                  /*!< Histogram color opacity */
  int hpos;                      /*!< Histogram is transparent ? */
};

/*! \typedef CurveExtra

  \brief extra curve(s) data information
*/
typedef struct CurveExtra CurveExtra;
struct CurveExtra
{
  tint id;                       /*!< Curve data pointer */
  DataLayout * layout;           /*!< Curve layout information */
  CurveExtra * prev;
  CurveExtra * next;
};

/*! \typedef ExtraSets

  \brief list of extra data sets for a curve
*/
typedef struct ExtraSets ExtraSets;
struct ExtraSets
{
  int extras;                 /*!< Number of extra data sets, if any */
  CurveExtra * first;         /*!< First data set of the list, if any */
  CurveExtra * last;          /*!< Last data set of the list, if any */
};

/*! \typedef Curve

  \brief curve data structure
*/
typedef struct Curve Curve;
struct Curve
{
  int cid;                       /*!< Curve id */
  int ndata;                     /*!< number of data points */
  double * data[2];              /*!< X and Y data */
  double * err;                  /*!< Error bar on Y if any. */
  double cmin[2];                /*!< Min of the data on X and Y */
  double cmax[2];                /*!< Max of the data on X and Y */
  GtkWidget * plot;              /*!< Drawing area */
  GtkWidget * button;            /*!< Interaction button for 'Toolboxes' */
  GtkWidget * curve_vbox;        /*!< Curve top boxes for the menu bar */
  GtkWidget * curve_hbox;
  GtkWidget * window;            /*!< Widget for the window */
  GtkWidget * pos;               /*!< Mouse cursor position in graph */
  GtkWidget * shortcuts;         /*!< Shortcuts window */
  int wsize[2];                  /*!< Curve window size */
  GtkWidget * datatree;          /*!< Widget for the selection tree */
  qint idcol[2];                 /*!< For navigation in the list view */
  ColRGBA backcolor;             /*!< Background color */
  int format;                    /*!< Format of output (screen, png, pdf, ...) */
  char * name;                   /*!< Name of the curve */
// Axis
  double axmin[2];               /*!< Min for the axis X and Y */
  double axmax[2];               /*!< Max for the axis X and Y */
  gboolean axis_defaut_title[2]; /*!< Use axis default title */
  char * axis_title[2];          /*!< Title of axis */
  int axis_title_x[2];           /*!< Position of the x axis title */
  int axis_title_y[2];           /*!< Position of the y axis title */
  gchar * axis_title_font[2];    /*!< Axis title font */
  int scale[2];                  /*!< Axis scale (linear or log) */
  gboolean autoscale[2];         /*!< Autoscale */
  gboolean show_grid[2];         /*!< Show/Hide axis grid */
  gboolean show_axis[2];         /*!< Show/Hide axis bar */
  double majt[2];                /*!< Value for major ticks */
  int mint[2];                   /*!< Number of minor ticks */
  int ticks_io[2];               /*!< Ticks in or out / axis bar */
  int ticks_pos[2];              /*!< Ticks position: normal, opposite, both */
  int majt_size[2];              /*!< Majors ticks size (pixels) */
  int mint_size[2];              /*!< Minors ticks size (pixels) */
  int labels_pos[2];             /*!< Ticks label position: normal, opposite, both */
  int labels_digit[2];           /*!< Significant digits for tick labels */
  gchar * labels_font[2];        /*!< Ticks label font */
  double labels_angle[2];        /*!< Ticks label angle */
  int labels_shift_x[2];         /*!< Ticks position shift from x axis */
  int labels_shift_y[2];         /*!< Ticks position shift from y axis */
  // Legend
  gboolean show_legend;          /*!< Show (1) /Hide (0) legend */
  double legend_pos[2];          /*!< Position of the legend */
  gchar * legend_font;           /*!< Legend font */
  ColRGBA legend_color;          /*!< Legend color */
  gboolean show_legend_box;      /*!< Display legend box */
  int legend_box_dash;           /*!< Legend box line style */
  ColRGBA legend_box_color;      /*!< Legend box color */
  double legend_box_thickness;   /*!< Legend box line thickness */
// Title
  char * title;                  /*!< Title to display */
  gboolean show_title;           /*!< Show/Hide title */
  gboolean default_title;        /*!< Use default title */
  double title_pos[2];           /*!< Position of the title */
  gchar * title_font;            /*!< Title font */
  ColRGBA title_color;           /*!< Title color */
// Frame
  gboolean show_frame;           /*!< Show/Hide frame */
  int frame_type;                /*!< Frame type */
  int frame_dash;                /*!< Frame line style */
  double frame_thickness;        /*!< Frame line thickness */
  ColRGBA frame_color;           /*!< Frame color */
  double frame_pos[2][2];        /*!< Frame  0 = x, 1 = y, 0 = min and 1 = max */

  DataLayout * layout;           /*!< The curve layout */
  ExtraSets * extrac;            /*!< Extra data set(s) added to graph */
  cairo_surface_t * surface;     /*!< The rendering surface */
  int draw_id;                   /*!< Curve drawing order */
  int bshift;                    /*!< Curve x shift for bar diagram */

  gboolean displayed;
  char * cfile;
  gchar * path;                  /*!< Path for the toolbox tree */
  int action_id;                 /*!< Unique Id to identify actions */
  CurveState state;              /*!< Curve state */
  GSimpleActionGroup * action_group;
};

/*! \def MAXDATC
  \brief Number of tabs for the description of the classical calculation
*/
#define MAXDATC 8

/*! \def MAXDATA
  \brief Number of tabs for the description of the classical force field
*/
#define MAXDATA 21

typedef struct classical_field classical_field;
struct classical_field
{
  gboolean prepare_file[2];
  // Field and Config files
  gboolean afp[MAXDATC+MAXDATA];
  int type;
  int energy_unit;
  int atom_init;
  int molecules;
  struct field_molecule * first_molecule;
  int nbody[5];
  struct field_nth_body * first_body[5];
  // Tersoff potential cross terms
  double *** cross; /*!< Tersoff potential cross termms */
  int extern_fields;
  struct field_external * first_external;

  // Control file
  double * sys_opts;
  double * io_opts;
  double * ana_opts;
  double * elec_opts;
  double * vdw_opts;
  double * met_opts;
  double * equi_opts;
  int ensemble;
  int thermostat;
  double * thermo_opts;
  double * md_opts;
  double * out_opts;
};

typedef struct thermostat thermostat;
struct thermostat
{
  int id;
  // For CPMD: 0 = none, 2 = controlled, 3 = nose
  // For CP2K: 0 = none, 2 = langevin, 3 = csvr, 4 = gle, 5 = nose
  int type;
  // For CPMD: 0 = global, 1 = local
  // For CP2K: 0 = global, 1 = local, 2 = molecule
  int sys;
  gboolean show;
  double params[4];
  int natoms;
  int * list;
  thermostat * next;
  thermostat * prev;
};

typedef struct dummy_atom dummy_atom;
struct dummy_atom
{
  // 0 = type1, 1 = type2, ...
  int id;
  int type;
  int pick[2];
  gboolean show;
  double xyz[3];
  int coord[4];
  int natoms;
  int * list;
  int numv;
  int * vois;
  dummy_atom * next;
  dummy_atom * prev;
};

typedef struct cpmd cpmd;
struct cpmd
{
  int calc_type;
  int restart[10];
  int thermostats;
  thermostat * ions_thermostat;
  thermostat * elec_thermostat;
  int fixat;
  int * fixlist;
  int ** fixcoord;
  int dummies;
  dummy_atom * dummy;
  double default_opts[17];
  double calc_opts[24];
  int ** pp;
  gchar * info;
};

typedef struct cp2k cp2k;
struct cp2k
{
  int input_type;
  double opts[42];
  double extra_opts[3][4];
  int thermostats;
  thermostat * ions_thermostat;
  int fixat[2];
  int * fixlist[2];
  int ** fixcoord[2];
  gchar * files[5];
  gchar *** spec_files;
  int ** spec_data;
  gchar * info;
};

/*! \typedef molecule

  \brief the molecule data structure
*/
typedef struct molecule molecule;
struct molecule
{
  int id;                         /*!< Molecule id number */
  int md;                         /*!< MD step */
  int multiplicity;               /*!< Multiplicity */
  int * fragments;                /*!< Fragments list */
  int natoms;                     /*!< Number of atoms */
  int nspec;                      /*!< Number of chemical species */
  int * species;                  /*!< Number of atom by species */
};

/*! \typedef model

  \brief data structure to describe the topology
*/
typedef struct model model;
struct model
{
  int * mol_by_step;       /*!< Num of mol by steps */
  molecule ** mols;        /*!< List of molecules by steps */
};

/*! \typedef element_data

  \brief element description used for the periodic table defined in 'w_periodic.c'
*/
typedef struct element_data element_data;
struct element_data
{
  gchar * lab;      /*!< Label */
  gchar * name;     /*!< Name */
  int Z;            /*!< Atomic number */
  float M;          /*!< Atomic mass */
};

/*! \typedef chemical_data

  \brief a structure to store some chemical information
*/
typedef struct chemical_data chemical_data;
struct chemical_data
{
  // 0 = Z, 1 = Mass, 2 = Radius, 3 = Neutrons, 4 = X-rays
  double ** chem_prop; /*!< chemical properties: \n
                            0 = Z, \n
                            1 = Mass, \n
                            2 = Radius, \n
                            3 = Neutrons, \n
                            4 = X-rays */
  int * nsps;          /*!< Number of atom(s) by chemical species */
  int * formula;       /*!< Chemical formula */
  char ** label;       /*!< Element label(s) */
  char ** element;     /*!< Element name(s) */
  double ** cutoffs;   /*!< partial cutoff(s) */
  double grtotcutoff;  /*!< Total cutoff */
};

/*! \typedef insertion_menu

  \brief data structure for the insertion pop-up menu
*/
typedef struct insertion_menu insertion_menu;
struct insertion_menu
{
  gchar * type;
  gchar * object;
  double Z;
  int ats;
};

/*! \typedef atom

  \brief atom data structure
*/
typedef struct atom atom;
struct atom
{
  int id;                        /*!< The atom's id in the model */ // The id in the model
  int sp;                        /*!< The chemical species */ // The chemical species
  // The coordinates
  double x;                      /*!< x coordinate */
  double y;                      /*!< y coordinate */
  double z;                      /*!< z coordinate */
  int numv;                      /*!< The number of neighbors */ // The number of neighbors
  int * vois;                    /*!< The list of neighbors */ // The list of neighbors
  // 0 = Total coordination
  // 1 = Partial coordination
  // 2 = Fragment
  // 3 = Molecule
  // 4 = Field object id
  int coord[5];                   /*!< Coordination information: \n
    0 = Total coordination, \n
    1 = Partial coordination, \n
    2 = Fragment, \n
    3 = Molecule, \n
    4 = Field object id
   */ // Coordination information
  // 0 = All
  // 1 = King's
  // 2 = Guttman's
  // 3 = Primitive
  // 4 = Strong
  int ** rings[5];                /*!< Ring statistics information: \n
    0 = All, \n
    1 = King's, \n
    2 = Guttman, \n
    3 = Primitive, \n
    4 = Strong
   */ // Rings statistics information
  int ** chain;                   /*!< Chain statistics information */
  int fid;                        /*!< Force field id */
  int faid;                       /*!< Force field id in fragment */
  gboolean show[2];               /*!< Show / Hide (0 = atom, 1 = clone) */
  gboolean label[2];              /*!< Label / Unlabel (0 = atom, 1 = clone) */
  gboolean pick[2];               /*!< Selected / Unselected (0 = atom, 1 = clone) */
  gboolean cloned;                /*!< Clone(s) ? (0 = no, 1 = yes) */
  int style;                      /*!< Rendering style if not global */
  atom * prev;
  atom * next;
};

/*! \typedef project

  \brief data structure for the 'atomes' project
*/
typedef struct project project;
struct project
{
  /*
     General parameters
  */
  int id;                              /*!< Project ID */
  char * name;                         /*!< Project name */
  char * projfile;                     /*!< Name of the project file, if any */
  char * coordfile;                    /*!< Name of atomic coordinates file, if any */
  char * bondfile;                     /*!< Name of the file to ouput bonding information, if any */
  gboolean newproj;                    /*!< New project ? yes / no */
  gboolean run;                        /*!< Run this project ? yes / no */
  gboolean dmtx;                       /*!< Trigger the calculation of the distance matrix ? yes / no */
  int tfile;                           /*!< File format for the atomic coordinates, when imported */
  int nspec;                           /*!< Number of chemical species */
  int natomes;                         /*!< Number of atoms */
  int dummies;                         /*!< Number of atoms including extra cells */
  int steps;                           /*!< Number of MD steps */
  int tunit;                           /*!< Time unit between steps, if MD */
  chemical_data * chemistry;           /*!< Chemical data */
  coord_info * coord;                  /*!< Coordination(s) data */
  cell_info cell;                      /*!< Periodicity data */
  atom ** atoms;                /*!< Atom list: atoms[steps][natomes] */
  /*
     Analysis related parameters
  */
  gboolean runok[NGRAPHS];             /*!< Analysis calculation availability */
  gboolean initok[NGRAPHS];            /*!< Curves initizalization  */
  gboolean visok[NGRAPHS];             /*!< Analysis calculation confirmation */
  int xcor;                            /*!< S(q) X-rays type of calculation: f(q) (1) or approximated (0) */
  gboolean runc[3];                    /*!< Trigger to run bonds, angles and molecules analysis */
  // gr, sq, sk, gftt, bd, an, frag-mol, ch, sp, msd
  int numc[NGRAPHS];                   /*!< Number of curves: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = frag-mol, \n 7 = ch, \n 8 = sp, \n 9 = msd */
  int num_delta[NGRAPHS];              /*!< Number of x points: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = frag-mol, \n 7 = ch, \n 8 = sp, \n 9 = msd */
  double calc_time[NGRAPHS];           /*!< Calculation time: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = frag-mol, \n 7 = ch, \n 8 = sp, \n 9 = msd */
  double delta[NGRAPHS];               /*!< Discretization: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = frag-mol, \n 7 = ch, \n 8 = sp, \n 9 = msd */
  double min[NGRAPHS];                 /*!< Minimum x value: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = frag-mol, \n 7 = ch, \n 8 = sp, \n 9 = msd */
  double max[NGRAPHS];                 /*!< Maximum x value: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = frag-mol, \n 7 = ch, \n 8 = sp, \n 9 = msd */
  // 0 = Search type, 1 = NUMA
  int rsearch[2];                      /*!< Ring statistics parameters: 0 = Search type, 1 = Ring's allocation parameter NUMA */
  // First col : search type (up to chains stat). Second col: search info
  // 0 = Initnode, 1 = RMAX, 2 = ABAB, 3 = Homo, 4 = Homo in DMTX, 5 = Done + Rings
  int rsparam[5][6];                   /*!< Ring statistics parameters (first column the type of rings): \n
                                            0 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                            1 = Maximum size of ring for the search Rmax, \n
                                            2 = Search only for ABAB rings or not, \n
                                            3 = Include Homopolar bond(s) in the analysis or not, \n
                                            4 = Include homopolar bond(s) when calculating the distance matrix, \n
                                            5 = Analysis completed and rings were found (yes / no) */
  double rsdata[5][5];                 /*!< Results for the ring statistics (first column the type of rings): \n
                                            0 = Total number of ring(s) per MD step: RpS, \n
                                            1 = Standard deviation for RpS, \n
                                            2 = Number of ring(s) with size > Rmax that potentially exist per MD step: RpE, \n
                                            3 = Standard deviation for RpE, \n
                                            4 = calculation time for the analysis */
  int csearch;                         /*!< Chain statistics allocation parameter: CNUMA */
  // 0 = Initnode, 1 = AAAA, 2 = ABAB, 3 = Homo, 4 = 1221, 5 = RMAX, 6 = Done + Chains
  int csparam[7];                      /*!< Chain statistics parameters: \n
                                            0 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                            1 = Search only for AAAA chains or not, \n
                                            2 = Search only for ABAB chains or not, \n
                                            3 = Include Homopolar bond(s) in the analysis or not, \n
                                            4 = Search only for 1-(2)n-1 chains, \n
                                            5 = Maximum size of chain for the search Rmax
                                            6 = Analysis completed and chains were found (yes / no) */
  double csdata[2];                    /*!< Results for the chain statistics: \n
                                            0 = Total number of chains) per MD step: CpS, \n
                                            1 = Standard deviation for CpS */
  double fact[4];                      /*!< Gaussian smoothing factors: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt */
  double sk_advanced[2];               /*!< */
  GtkTextBuffer * text_buffer[NITEMS]; /*!< The text buffer for the results of the calculations */
  tint * idcc[NGRAPHS];                /*!< Pointers for the curves */
  int numwid;                          /*!< total number of curves for this project */
  Curve ** curves[NGRAPHS];            /*!< The curves, graph for the results of the calculations */
  /*
     OpenGL related parameters
  */
  gboolean initgl;                     /*!< Was OpenGL rendering initialized ? yes / no */
  int tmp_pixels[2];                   /*!< Saved size of the OpenGL window */
  glwin * modelgl;                     /*!< The OpenGL widget */
  /*
     Molecular dynamics related parameters
  */
  model * modelfc;                     /*!< Description of the topology */
  // MD input files
  classical_field * force_field[2];    /*!< Classical MD input files: \n 0 = DL_POLY, \n 1 = LAMMPS */
  cpmd * cpmd_input[2];                /*!< CPMD input files: \n 0 = Ab-initio, \n 1= QM-MM */
  cp2k * cp2k_input[2];                /*!< CP2K input files: \n 0 = Ab-initio, \n 1= QM-MM */

#ifdef DEBUG
  GtkWidget * pix_tab[3];              /*!< Not used anymore see 'cell_pixel.c' */
  GtkWidget * pix_box;                 /*!< Not used anymore see 'cell_pixel.c' */
  int pix[3];                          /*!< Not used anymore see 'cell_pixel.c' */
  int ** pixels;                       /*!< Not used anymore see 'cell_pixel.c' */
  int actif_pix;                       /*!< Not used anymore see 'cell_pixel.c' */
#endif

  project * next;
  project * prev;
};

/*! \typedef workspace

  \brief data structure for the 'atomes' workspace
*/
typedef struct workspace workspace;
struct workspace
{
  project * first;
  project * last;
};

extern atomes_action edition_acts[];
extern atomes_action analyze_acts[];
extern GSimpleAction * edition_actions[3];
extern GSimpleAction * analyze_actions[9];
extern void add_action (GSimpleAction * action);
extern void remove_action (gchar * action_name);
extern void remove_edition_actions ();
extern void remove_edition_and_analyze_actions ();

extern GtkApplication * AtomesApp;
extern workspace workzone;
extern project * proj;
extern chemical_data * active_chem;
extern coord_info * active_coord;
extern cell_info * active_cell;
extern box_info * active_box;
extern image * active_image;
extern glwin * active_glwin;
extern project * active_project;
extern project * opengl_project;
extern element_data periodic_table_info[];

extern project * get_project_by_id (int p);
extern void opengl_project_changed (int id);
extern gboolean in_md_shaders (project * this_proj, int id);
extern void recreate_all_shaders (glwin * view);
extern gboolean is_atom_win_active (glwin * view);

// extern gboolean run_distance_matrix (GtkWidget * widg, int calc, int up_ngb);
extern G_MODULE_EXPORT void on_calc_bonds_released (GtkWidget * widg, gpointer data);
extern void update_rings_menus (glwin * view);
extern void clean_rings_data (int rid, glwin * view);
extern void clean_chains_data (glwin * view);
extern void clean_volumes_data (glwin * view);

extern void initcutoffs (chemical_data * chem, int species);
extern void cutoffsend ();
extern void update_entry_int (GtkEntry * entry, int intval);
extern void update_entry_double (GtkEntry * entry, double doubleval);
extern void update_entry_long_double (GtkEntry * entry, double doubleval);
extern void update_entry_text (GtkEntry * entry, gchar * text);

extern double * xsk;
extern gboolean * allocbool (int val);
extern gboolean ** allocdbool (int xal, int yal);
extern gboolean *** alloctbool (int xal, int yal, int zal);
extern int * allocint (int val);
extern int ** allocdint (int xal, int yal);
extern int *** alloctint (int xal, int yal, int zal);
extern int **** allocqint (int wal, int xal, int yal, int zal);
extern float * allocfloat (int  val);
extern float ** allocdfloat (int xal, int yal);
extern float *** alloctfloat (int xal, int yal, int zal);
extern double * allocdouble (int val);
extern double ** allocddouble (int xal, int yal);
extern double *** alloctdouble (int xal, int yal, int zal);
extern double **** allocqdouble (int wal, int xal, int yal, int zal);

extern GLuint * allocgluint (int val);
extern GLuint ** allocdgluint (int xal, int yal);
extern GLfloat ** allocdGLfloat (int xal, int yal);

extern gchar ** duplicate_strings (int num, gchar ** old_val);
extern int * duplicate_int (int num, int * old_val);
extern gboolean * duplicate_bool (int num, gboolean * old_val);
extern float * duplicate_float (int num, float * old_val);
extern double * duplicate_double (int num, double * old_val);
extern ColRGBA * duplicate_color (int num, ColRGBA * col);

extern void combo_set_markup (GtkWidget * combo);
extern void combo_set_active (GtkWidget * combo, int pos);
extern int combo_get_active (GtkWidget * combo);
extern void combo_text_append (GtkWidget * combo, gchar * text);
extern void combo_text_prepend (GtkWidget * combo, gchar * text);
extern GtkWidget * create_combo ();
extern GtkTextBuffer * add_buffer (GCallback handler, gpointer data, gchar * text);
extern GtkWidget * create_text_view (int dimx, int dimy, int edit, int mono,
                                     GCallback handler, gpointer data, gchar * text);

extern GdkPixbuf * convert_to_pixbuf (cairo_surface_t * surf);

extern void text_view_set_monospace (GtkWidget * view);
extern void gtk_label_align (GtkWidget * lab, float ax, float ay);

extern gchar * prepare_for_title (gchar * init);

extern void show_the_widgets (GtkWidget * widg);
extern void hide_the_widgets (GtkWidget * widg);
extern gboolean is_the_widget_visible (GtkWidget * widg);
extern void widget_set_sensitive (GtkWidget * widg, gboolean sensitive);

extern GtkWidget * new_gtk_window ();
extern GtkWidget * dialogmodal (gchar * str, GtkWindow * parent);
extern GtkWidget * message_dialogmodal (gchar * message, gchar * title, GtkMessageType mtype, GtkButtonsType buttons, GtkWidget * parent);
extern GtkWidget * dialog_cancel_apply (gchar * title, GtkWidget * parent, gboolean resiz);
extern void run_this_gtk_dialog (GtkWidget * dial, GCallback handler, gpointer data);
extern void resize_this_window (GtkWidget * window, int x, int y);

extern GtkWidget * create_hscale (float min, float max, float delta,
                                  float val, int pos, int round, int size,
                                  GCallback handler, GCallback scroll_handler, gpointer data);
extern GtkWidget * create_vscale (float min, float max, float delta,
                                  float val, int pos, int round, int size,
                                  GCallback handler, GCallback scroll_handler, gpointer data);

extern void append_comments (GtkWidget * vbox, gchar * symbol, gchar * legend);
extern void add_container_child (int type, GtkWidget * widg, GtkWidget * child);
extern void add_box_child_end (GtkWidget * widg, GtkWidget * child, gboolean expand, gboolean fill, int padding);
extern void add_box_child_start (int orientation, GtkWidget * widg, GtkWidget * child, gboolean expand, gboolean fill, int padding);
extern GtkWidget * create_vbox (int spacing);
extern GtkWidget * create_hbox (int spacing);
extern GtkWidget * dialog_get_content_area (GtkWidget * widg);
extern void layout_add_widget (GtkWidget * layout, GtkWidget * child, int x_pos, int y_pos);
extern GtkWidget * add_vbox_to_layout (GtkWidget * layout, int size_x, int size_y);
extern GtkWidget * create_layout (int x, int y);
extern void add_global_option (GtkWidget * vbox, tint * oid);

#ifdef GTK4
extern void run_this_gtk_native_dialog (GtkNativeDialog * dial, GCallback handler, gpointer data);
extern G_MODULE_EXPORT void select_unselect_this_atom (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void select_unselect_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void select_unselect_coord (GSimpleAction * action, GVariant * parameter, gpointer data);

extern G_MODULE_EXPORT void label_unlabel_this_atom (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void label_unlabel_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void label_unlabel_coord (GSimpleAction * action, GVariant * parameter, gpointer data);

extern G_MODULE_EXPORT void show_hide_this_atom (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void show_hide_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void show_hide_the_coord (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void show_hide_coord (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void show_hide_poly (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void show_hide_labels (GSimpleAction * action, GVariant * parameter, gpointer data);

extern G_MODULE_EXPORT void remove_the_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void replace_the_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void edit_in_new_project (GSimpleAction * action, GVariant * parameter, gpointer data);
extern void add_widget_gesture_and_key_action (GtkWidget * widget,
                                               gchar * cp_name, GCallback cp_handler, gpointer cp_data,
                                               gchar * cr_name, GCallback cr_handler, gpointer cr_data,
                                               gchar * kp_name, GCallback kp_handler, gpointer kp_data,
                                               gchar * mo_name, GCallback mo_handler, gpointer mo_data,
                                               gchar * sc_name, GCallback sc_handler, gpointer sc_data);
extern G_MODULE_EXPORT gboolean destroy_this_window (GtkWindow * win, gpointer data);
extern G_MODULE_EXPORT gboolean hide_this_window (GtkWindow * win, gpointer data);
extern GtkFileChooserNative * create_file_chooser (const gchar * title, GtkWindow * parent, GtkFileChooserAction act, const gchar * act_name);
extern void pop_menu_at_pointer (GtkWidget * pop, double x, double y);
GListModel * file_chooser_get_file_names (GtkFileChooser * chooser);
extern void update_menu_bar (glwin * view);

#else

extern void update_chains_menus (glwin * view);

extern G_MODULE_EXPORT void select_unselect_this_atom (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void select_unselect_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void select_unselect_coord (GtkWidget * widg, gpointer data);

extern G_MODULE_EXPORT void label_unlabel_this_atom (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void label_unlabel_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void label_unlabel_coord (GtkWidget * widg, gpointer data);

extern G_MODULE_EXPORT void show_hide_this_atom (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void show_hide_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void show_hide_the_coord (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void show_hide_coord (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void show_hide_poly (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void show_hide_labels (GtkWidget * widg, gpointer data);

extern G_MODULE_EXPORT void remove_the_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void replace_the_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void edit_in_new_project (GtkWidget * widg, gpointer data);
extern GtkWidget * create_menu_item (gboolean add_mnemo, gchar * action);
extern GtkWidget * create_menu_item_from_widget (GtkWidget * widg, gboolean check, gboolean radio, gboolean status);
extern GtkWidget * menu_item_new_with_submenu (gchar * name, gboolean active, GtkWidget * sub_menu);
extern void add_menu_separator (GtkWidget * menu);
extern G_MODULE_EXPORT gboolean destroy_this_window (GtkWidget * win, GdkEvent * event, gpointer data);
extern G_MODULE_EXPORT gboolean hide_this_window (GtkWidget * win, GdkEvent * event, gpointer data);
extern GtkWidget * create_file_chooser (const gchar * title, GtkWindow * parent, GtkFileChooserAction act, const gchar * act_name);
extern void pop_menu_at_pointer (GtkWidget * widg, GdkEvent * event);
GSList * file_chooser_get_file_names (GtkFileChooser * chooser);
#endif

extern const gchar * entry_get_text (GtkEntry * entry);
extern GtkWidget * create_entry (GCallback handler, int dim, int cdim, gboolean key_release, gpointer data);


extern GtkWidget * stock_image (const gchar * stock_id);
extern void set_image_from_icon_name (GtkWidget * widg, gchar * icon);
extern GtkWidget * create_image_from_data (int format, gpointer item_image);

extern GtkWidget * gtk3_menu_item (GtkWidget * menu, gchar * name,
                                   int icon_format, gpointer item_icon,
                                   GCallback handler, gpointer data,
                                   gboolean accel, guint key, GdkModifierType mod,
                                   gboolean check, gboolean radio, gboolean status);

extern GtkWidget * add_advanced_item (GtkWidget * menu, GCallback handler, gpointer data, gboolean accel, guint key, GdkModifierType mod);

extern GtkWidget * markup_label (gchar * text, int dimx, int dimy, float ax, float ay);
extern GtkWidget * color_button (ColRGBA col, gboolean alpha, int dimx, int dimy, GCallback handler, gpointer data);
extern GtkWidget * font_button (gchar * font, int dimx, int dimy, GCallback handler, gpointer data);
extern GtkWidget * spin_button (GCallback handler, double value, double start, double end, double step, int digits, int dim,  gpointer data);
extern int button_get_status (GtkWidget * button);
extern void button_set_status (GtkWidget * button, int status);
extern GtkWidget * check_button (gchar * text, int dimx, int dimy, gboolean state, GCallback handler, gpointer data);
extern GtkWidget * radio_button (gchar * text, int dimx, int dimy, gboolean state, GCallback handler, gpointer data);
extern GtkWidget * create_button (gchar * text, int image_format, gchar * image, int dimx, int dimy, int relief, GCallback handler, gpointer data);

extern ColRGBA gdkrgba_to_rgba (GdkRGBA colgdk);
extern GdkRGBA colrgba_togtkrgba (ColRGBA col);
extern ColRGBA get_button_color (GtkColorChooser * colob);
extern ColRGBA get_window_color (GtkWidget * color_win);
extern void set_color_chooser_color (GtkWidget * color_win, ColRGBA col);
extern void set_renderer_color (int tocol, GtkCellRenderer * renderer, ColRGBA col);

extern void button_set_image (GtkButton * but, gchar * text, int format, gpointer image);

extern GtkWidget * abox (GtkWidget * box, char * lab, int vspace);
extern GtkWidget * bbox (GtkWidget * box, char * lab);
extern GtkWidget * cbox (GtkWidget * box, char * lab);
extern GtkWidget * fbox (GtkWidget * box, char * lab);
extern GtkWidget * create_scroll (GtkWidget * box, int dimx, int dimy, int shadow);
extern GtkWidget * create_expander (gchar * name, gchar * file_img);

extern GtkWidget * create_win (gchar * str, GtkWidget * parent, gboolean modal, gboolean resiz);

extern void widget_add_action (GSimpleActionGroup * action_group, const gchar * act, GCallback handler, gpointer data,
                               gboolean check, gboolean status, gboolean radio, const gchar * stat);
extern void append_menu_item (GMenu * menu, const gchar * label, const gchar * action, const gchar * accel,
                              const gchar * custom, int format, const gchar * icon,
                              gboolean check, gboolean status, gboolean radio, const gchar * rstatus);
extern void append_submenu (GMenu * menu, const gchar * label, GMenu * submenu);

extern GtkWidget * destroy_this_widget (GtkWidget * widg);
extern void destroy_this_dialog (GtkDialog * dialog);
extern void destroy_this_native_dialog (GtkNativeDialog * dialog);
extern G_MODULE_EXPORT void run_destroy_dialog (GtkDialog * dialog, gint response_id, gpointer data);

extern void add_gtk_close_event (GtkWidget * widg, GCallback handler, gpointer data);

extern gchar * file_chooser_get_file_name (GtkFileChooser * chooser);
extern gchar * file_chooser_get_current_folder (GtkFileChooser * chooser);
extern gboolean file_chooser_set_file_name (GtkFileChooser * chooser, gchar * filename);
extern void file_chooser_set_current_folder (GtkFileChooser * chooser);

extern GtkWidget * get_top_level (GtkWidget * widg);

extern void provide_gtk_css (gchar * css);
extern double string_to_double (gpointer string);
extern double get_calc_time (struct timespec start, struct timespec stop);
extern gchar * calculation_time (gboolean modelv, double ctime);

extern int get_widget_width (GtkWidget * widg);
extern int get_widget_height (GtkWidget * widg);

typedef struct {
  GCallback handler;
  gpointer data;
} focus_data;

#endif  // GLOBAL_H_
