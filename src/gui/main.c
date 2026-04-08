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
* @file main.c
* @short Initialization of the atomes program \n
         Functions required to read data from the command line
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'main.c'
*
* Contains:
*

 - The initialization of the atomes program
 - The functions required to read data from the command line

*
* List of functions:

  int test_this_ext (int len, gchar * arg);
  int test_this_arg (gchar * arg);
  int get_style_from_string (gchar * style_string);
  int get_rep_from_string (gchar * rep_string);
  int get_box_axis_from_string (gchar * box_axis_string);
  int get_color_map_from_string (gchar * col_string);
  int get_gradient_from_string (gchar * grad_string);
  int check_for_atomes_file_options (int start, int end, char *argv[]);
  int parse_command_line (int argc, char *argv[])
  int main (int argc, char *argv[]);

  ColRGBA * get_color_from_hexa_string (gchar * color_string);

  gboolean destroy_func (gpointer user_data);

  G_MODULE_EXPORT gboolean splashdraw (GtkWidget * widget, cairo_t * cr, gpointer data);

  void printhelp();
  void printversion ();
  void read_this_file (int file_type, gchar * this_file);
  void open_this_data_file (int file_type, gchar * file_name);

  G_MODULE_EXPORT void run_program (GApplication * app, gpointer data);

  GtkWidget * create_splash_window ();

*
* Notes:
*

  LLM tools (Le Chat) were used at few occasions to prepare some sections of this file, including:
    - To write of the function to convert hexadecimal chain to color

*/

#include <getopt.h>
#include <ctype.h>
#include "version.h"
#include "global.h"
#include "bind.h"
#include "callbacks.h"
#include "preferences.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"
#include "glview.h"
#include "movie.h"

#ifdef G_OS_WIN32
#define APP_EXTENSION ".exe"
#include <shlobj.h>
#else
#include <pwd.h>
#define APP_EXTENSION
#endif

extern GtkWidget * create_main_window (GApplication * app);

const gchar * dfi[2];
struct file_list {
  gchar * file_name;
  int file_type;
  struct file_list * next;
};

struct file_list * flist = NULL;
struct file_list * ftmp = NULL;
gboolean with_workspace = FALSE;

gchar * bs_styles[] = {"ball&stick", "balls&sticks", "balls&stick", "ball&sticks",
                       "ball_&_stick", "balls_&_sticks", "balls_&_stick", "ball_&_sticks",
                       "ball_and_sticks", "balls_and_sticks", "balls_and_stick", "ball_and_stick",
                       "ballsandsticks", "ballsandstick", "ballanddsticks", "ballandstick",
                       "b&s", "b_and_s", "bs", "b", NULL};
gchar * wi_styles[] = {"wireframe", "wireframes", "wires", "wire", "wi", "w", NULL};
gchar * co_styles[] = {"covalent_radius", "covalent", "cov_rad", "cov", "co", "cr", NULL};
gchar * io_styles[] = {"ionic_radius", "ionic_rad", "ionic", "ion", "ir", NULL};
gchar * vw_styles[] = {"vdw_radius", "van_der_waals_radius", "van_der_waals", "vdw_radius", "vdw_rad", "vdw", "vr", NULL};
gchar * cr_styles[] = {"crystal", "crystal_radius", "in_crystal", "in_crystal_radius", "cryst", "crystal_rad", "cr", NULL};
gchar * sp_styles[] = {"sphere", "spheres", "sph", "sp", "S", NULL};
gchar * cy_styles[] = {"cylinder", "cylinders", "cyl", "cy", "c", NULL};
gchar * do_styles[] = {"dot", "dots", "d", NULL};

/*!
  \fn void print_help()

  \brief print basic help
*/
void print_help()
{
  char * help    = "\nUsage: atomes [OPTION]\n"
                   "       atomes [FILE]\n"
                   "       atomes [OPTION] [FILE]\n"
                   "       atomes [FILE1] [FILE2] ...\n"
                   "       atomes [OPTION1] [FILE1] [OPTION2] [FILE2] ...\n\n"
                   "3D atomic-scale models analysis, creation/edition and post-processing tool\n\n"
                   "options:\n"
                   "  -v, --version             version information\n"
                   "  -h, --help                display this help message\n\n"
                   "files, any number, in any order, in the following formats:\n\n"
                   "  atomes workspace file             : .awf\n"
                   "  atomes project file               : .apf\n"
                   "  XYZ coordinates file              : .xyz\n"
                   "  Chem3D coordinates file           : .c3d\n"
                   "  CPMD trajectory                   : .trj\n"
                   "  VASP trajectory                   : .xdatcar\n"
                   "  PDB coordinates                   : .pdb, .ent\n"
                   "  Crystallographic Information File : .cif\n"
                   "  DL-POLY history file              : .hist\n"
                   "  ISAACS project file               : .ipf\n\n"
                   " alternatively specify the file format using:\n\n"
                   " -awf [FILE]\n"
                   " -apf [FILE]\n"
                   " -xyz [FILE]\n"
                   " -c3d [FILE]\n"
                   " -trj [FILE]\n"
                   " -xdatcar [FILE]\n"
                   " -pdb [FILE], or, -ent [FILE]\n"
                   " -cif [FILE]\n"
                   " -hist [FILE]\n"
                   " -ipf [FILE]\n\n"
                   "ex:\n\n"
                   " atomes -pdb this.f file.awf -cif that.f *.xyz\n\n"
                   "Image rendering from the command line:\n\n"
                   "Usage: atomes [IMAGE_OPTIONS]"
                   "  -p, --png, --render-png    render image in PNG format\n"
                   "  -j, --jpg, --render-jpg    render image in JPEG format\n"
                   "  -o, --output=[FILE]        image file name\n"
                   "  -W, --width=[XSIZE]        image width\n"
                   "  -H, --height=[YSIZE]       image height\n"
                   "  -s, --style=[STYLE]        rendering style\n"
                   "  -r, --rep=[REP]            representation type\n"
                   "  -b, --box=[STYLE]          box style\n"
                   "  -a, --axis=[STYLE]         axis style\n"
                   "  -e, --acolor=[MAP]         atoms and bonds color map\n"
                   "  -t, --pcolor=[MAP]         ployhedra color map\n"
                   "  -C, --box_color=[COLOR]    box color\n"
                   "  -G, --back_grad=[GRAD]     background gradient type\n"
                   "  -B, --back_color=[COLOR]   background color\n"
                   "  -D, --back_dir=[DIR]       background direction\n"
                   "  -P, --back_pos=[POS]       colors mixed position\n"
                   "  -U, --grad_col_a=[COL]     gradient initial color\n"
                   "  -V, --grad_col_b=[COL]     gradient final color\n\n"

                   "ex:\n\n"
                   " atomes --render-png --width=1920 -H 1024 --output=image.png project.apf -s ball_and_stick\n"
                   " atomes --jpg --style=vdw -r ortho -e pc -t pc\n\n";
  char bug[20] = "\nReport a bug to <";
  char eh[4] = ">\n";

  printf("%s", help);
  printf("%s", bug);
  printf("%s", PACKAGE_BUGREPORT);
  printf("%s\n", eh);
}

/*!
  \fn void print_version ()

  \brief print version information
*/
void print_version ()
{
  char scanid[80]="\n3D atomistic model analysis, creation/edition and post-processing tool\n";
  char bug[20] = "\nReport a bug to <";
  char eh[4] = ">\n";

  printf ("%s", scanid);
  printf ("\n%s version         : %s\n", PACKAGE, VERSION);
  printf ("\nGTK+ version           : %1d.%1d.%1d\n",
          GTK_MAJOR_VERSION,
          GTK_MINOR_VERSION,
          GTK_MICRO_VERSION);
  printf ("Libavutil version      : %2d.%2d.%3d\n",
          LIBAVUTIL_VERSION_MAJOR,
          LIBAVUTIL_VERSION_MINOR,
          LIBAVUTIL_VERSION_MICRO);
  printf ("Libavformat version    : %2d.%2d.%3d\n",
          LIBAVFORMAT_VERSION_MAJOR,
          LIBAVFORMAT_VERSION_MINOR,
          LIBAVFORMAT_VERSION_MICRO);
  printf ("Libavcodec version     : %2d.%2d.%3d\n",
          LIBAVCODEC_VERSION_MAJOR,
          LIBAVCODEC_VERSION_MINOR,
          LIBAVCODEC_VERSION_MICRO);
  printf ("Libswscale version     : %2d.%2d.%3d\n",
          LIBSWSCALE_VERSION_MAJOR,
          LIBSWSCALE_VERSION_MINOR,
          LIBSWSCALE_VERSION_MICRO);
#ifdef OPENMP
  float v;
  char * v_string;
  switch (_OPENMP)
  {
    case 200505:
      v = 2.5;
      v_string = "2005-05";
      break;
    case 200805:
      v = 3.0;
      v_string = "2008-05";
      break;
    case 201107:
      v = 3.1;
      v_string = "2011-07";
      break;
    case 201307:
      v = 4.0;
      v_string = "2013-07";
      break;
    case 201511:
      v = 4.5;
      v_string = "2015-11";
      break;
    case 201811:
      v = 5.0;
      v_string = "2018-11";
      break;
    case 202011:
      v = 5.1;
      v_string = "2020-11";
      break;
  }
  printf ("OpenMP version         : %1.1f (%s)\n", v, v_string);
#endif
  printf ("FC    Compiler         : %s\n", FC);
  printf ("FC    Compiler flags   : %s\n", FCFLAGS);
  printf ("C     Compiler         : %s\n", CC);
  printf ("C     Compiler flags   : %s\n", CFLAGS);

  printf ("%s", bug);
  printf ("%s", PACKAGE_BUGREPORT);
  printf ("%s\n", eh);
}

/*!
  \fn int test_this_ext (int len, gchar * arg)

  \brief test extension of an argument from the command line

  \param len argument string length
  \param arg the argument to test
*/
int test_this_ext (int len, gchar * arg)
{
  int i;
  gchar * aext = g_strdup_printf ("%c%c%c%c", arg[len-4], arg[len-3], arg[len-2], arg[len-1]);
  char * eext[15]={".awf", ".apf", ".xyz", "NULL", ".c3d", ".trj", "NULL", "tcar", "NULL", ".pdb", ".ent", ".cif", "NULL", "hist", ".ipf"};
  for (i=0; i<15; i++) if (g_strcmp0 (aext, eext[i]) == 0)
  {
    g_free (aext);
    return -(i+1);
  }
  g_free (aext);
  return 0;
}

/*!
  \fn int test_this_arg (gchar * arg)

  \brief test an argument from the command line

  \param arg the argument to test
*/
int test_this_arg (gchar * arg)
{
  char * fext[15]={"-awf", "-apf", " -xyz", "NULL", "-c3d", "-trj", "NULL", "-xdatcar", "NULL", "-pdb", "-ent", "-cif", "NULL", "-hist", "-ipf"};
  int i, j;
  i = strlen(arg);
  gchar * str = g_ascii_strdown (arg, i);
  for (j=0; j<15; j++)
  {
    if (g_strcmp0 (str, fext[j]) == 0)
    {
      g_free (str);
      return j+1;
    }
  }
  j = test_this_ext (i, str);
  g_free (str);
  return (j) ? j : 0;
}

/*!
  \fn gboolean is_string_in_string_list (gchar * string, gchar ** list)

  \brief check if a string is in a list of strings

  \param string the string to search for
  \param list the string list
*/
gboolean is_string_in_string_list (gchar * string, gchar ** list)
{
  if (string != NULL)
  {
    while (* list != NULL)
    {
      if (g_strcmp0(* list, string) == 0)
      {
        return TRUE;
      }
      list ++;
    }
  }
  return FALSE;
}

/*!
  \fn int get_style_from_string (gchar * style_string)

  \brief retrieve style from command line string

  \param style_string the style keyword from command line
*/
int get_style_from_string (gchar * style_string)
{
  if (is_string_in_string_list(style_string, bs_styles)) return BALL_AND_STICK;
  if (is_string_in_string_list(style_string, wi_styles)) return WIREFRAME;
  if (is_string_in_string_list(style_string, co_styles)) return OGL_STYLES;
  if (is_string_in_string_list(style_string, io_styles)) return OGL_STYLES+1;
  if (is_string_in_string_list(style_string, vw_styles)) return OGL_STYLES+2;
  if (is_string_in_string_list(style_string, cr_styles)) return OGL_STYLES+3;
  if (is_string_in_string_list(style_string, sp_styles)) return SPHERES;
  if (is_string_in_string_list(style_string, cy_styles)) return CYLINDERS;
  if (is_string_in_string_list(style_string, do_styles)) return PUNT;
  return NONE;
}

/*!
  \fn int get_rep_from_string (gchar * rep_string)

  \brief retrieve representation style from command line string

  \param rep_string the representation keyword from command line
*/
int get_rep_from_string (gchar * rep_string)
{
  gchar * ortho_keys[] = {"orthographic", "ortho", "o", "0", NULL};
  gchar * persp_keys[] = {"perspective", "persp", "p", "1", NULL};
  if (is_string_in_string_list(rep_string, ortho_keys)) return ORTHOGRAPHIC;
  if (is_string_in_string_list(rep_string, persp_keys)) return PERSPECTIVE;
  return NONE;
}

/*!
  \fn int get_box_axis_from_string (gchar * box_axis_string)

  \brief retrieve box or axis style from command line string

  \param box_axis_string the axis keyword from command line
*/
int get_box_axis_from_string (gchar * box_axis_string)
{
  if (is_string_in_string_list(box_axis_string, wi_styles)) return WIREFRAME;
  if (is_string_in_string_list(box_axis_string, cy_styles)) return CYLINDERS;
  return NONE;
}

/*!
  \fn int get_color_map_from_string (gchar * col_string)

  \brief retrieve color map from command line string

  \param col_string the color map keyword from command line
*/
int get_color_map_from_string (gchar * col_string)
{
  gchar * sp_keys[] = {"species", "spec", "sp", "0", NULL};
  gchar * pt_keys[] = {"total_coordinations", "total_coordination", "total_coord", "total", "tc", "t", "1", NULL};
  gchar * pc_keys[] = {"partial_coordinations", "partial_coordination", "partial_coord", "partial", "pc", "p", "2", NULL};
  gchar * fg_keys[] = {"fragments", "fragment", "frag", "fg", "f", "3", NULL};
  gchar * mo_keys[] = {"molecules", "molecule", "mol", "mo", "m", "4", NULL};
  if (is_string_in_string_list(col_string, sp_keys)) return 0;
  if (is_string_in_string_list(col_string, pt_keys)) return 1;
  if (is_string_in_string_list(col_string, pc_keys)) return 2;
  if (is_string_in_string_list(col_string, fg_keys)) return 3;
  if (is_string_in_string_list(col_string, mo_keys)) return 4;
  return NONE;
}

/*!
  \fn ColRGBA * get_color_from_hexa_string (gchar * color_string)

  \brief convert Hexadecimal string to ColRGBA

  \param color_string the color keyword from command line
*/
ColRGBA * get_color_from_hexa_string (gchar * color_string)
{
   ColRGBA * col = NULL;
  const char * color = color_string;
  // Ignorer le '#' si présent au début
  if (color_string[0] == '#') color = color_string + 1;
  size_t len = strlen(color);
  if (len == 3 || len == 6)
  {
    // Valid chain
    col = g_malloc0(sizeof*col);
    col -> red   = strtol(color + 0, NULL, 16) / 255.0;
    col -> green = strtol(color + len/3, NULL, 16) / 255.0;
    col -> blue  = strtol(color + 2*len/3, NULL, 16) / 255.0;
    col -> alpha = 1.0;
  }
  return col;
}

/*!
  \fn int get_gradient_from_string (gchar * grad_string)

  \brief retrieve background gradient from command line string

  \param grad_string the color map keyword from command line
*/
int get_gradient_from_string (gchar * grad_string)
{
  gchar * no_keys[] = {"none", "no", "n", "0", NULL};
  gchar * li_keys[] = {"linear", "lin", "li", "l", "1", NULL};
  gchar * ci_keys[] = {"circular", "circ", "ci", "c", "2", NULL};
  if (is_string_in_string_list(grad_string, no_keys)) return 0;
  if (is_string_in_string_list(grad_string, li_keys)) return 1;
  if (is_string_in_string_list(grad_string, ci_keys)) return 2;
  return NONE;
}

/*!
  \fn int check_for_atomes_file_options (int start, int end, char *argv[])

  \brief retrieve the list of files to read from the command line
         leave out all other options

  \param start starting position on the command line
  \param end ending position on the command line
  \param *argv[] list of argument(s) on the command line
*/
int check_for_atomes_file_options (int start, int end, char *argv[])
{
  int i, j, k;
  i=0;
  for (j=start; j<end; j++)
  {
    k = test_this_arg (argv[j]);
    if (! (abs(k) == 1 && with_workspace))
    {
      if (k > 0 && j < end-1)
      {
        if (! flist)
        {
          flist = g_malloc0(sizeof*flist);
          ftmp = flist;
        }
        else
        {
          ftmp -> next = g_malloc0(sizeof*ftmp -> next);
          ftmp = ftmp -> next;
        }
        ftmp -> file_name = g_strdup_printf ("%s", argv[j+1]);
        ftmp -> file_type = k;
        argv[j] = argv[j+1] = g_strdup_printf (" ");
        j ++;
        i ++;
      }
      if (abs(k) == 1) with_workspace = TRUE;
    }
    else if (k == 1)
    {
      // Other workspaces are ignored
      argv[j] = argv[j+1] = g_strdup_printf (" ");
      j ++;
    }
  }
  return i;
}

/*!
  \fn int parse_command_line (int argc, char *argv[])

  \brief test command line arguments

  \param argc number of argument(s) on the command line
  \param *argv[] list of argument(s) on the command line
*/
int parse_command_line (int argc, char *argv[])
{
  struct option atomes_options[] = {{"help", no_argument, 0, 'h'},
                                    {"version", no_argument, 0, 'v'},
                                    {"libreoffice", no_argument, 0, 'l'},
                                    {"render-png", no_argument, 0, 'p'},
                                    {"png", no_argument, 0, 'p'},
                                    {"render-jpg", no_argument, 0, 'j'},
                                    {"jpg", no_argument, 0, 'j'},
                                    {"width", required_argument, 0, 'W'},
                                    {"height", required_argument, 0, 'H'},
                                    {"output", required_argument, 0, 'o'},
                                    {"style", required_argument, 0, 's'},
                                    {"atoms", required_argument, 0, 'e'},
                                    {"poly", required_argument, 0, 't'},
                                    {"axis", required_argument, 0, 'a'},
                                    {"box", required_argument, 0, 'b'},
                                    {"box_color", required_argument, 0, 'C'},
                                    {"back_color", required_argument, 0, 'B'},
                                    {"back_grad", required_argument, 0, 'G'},
                                    {"back_dir", required_argument, 0, 'D'},
                                    {"back_pos", required_argument, 0, 'P'},
                                    {"grad_col_a", required_argument, 0, 'U'},
                                    {"grad_col_b", required_argument, 0, 'V'},
                                    {"rep", required_argument, 0, 'r'},
                                    // {"debug", no_argument, 0, 'd'},
                                    {0, 0, 0, 0}};
  int opt;
  int index = -1;
  int i, j;
  gchar * image_x = NULL;
  gchar * image_y = NULL;
  int img_opt = 0;

  for (i=0; i<2; i++) render_image_grad_color[i] = NULL;
  /* We want to parse the command line using GNU getopt, but then
     some options that are atomes specific and not GNU getopt compatible
     might disappear, to prevent that it is mandatory to prepare
     the information to be given to GNU getopt */
  int files_to_read = check_for_atomes_file_options (1, argc, argv);

  /* Letter follow by : means that the command requires an argument
     No letter if the option is only in long format, ex : --width
     If the long name is empty the command is only in short format */
  while ((opt = getopt_long(argc, argv, "hvlpjdW:H:o:s:a:b:r:e:t:B:C:G:D:P:U:V:", atomes_options, & index)) != -1)
  {
    switch (opt)
    {
      case 'h':
        print_help();
        return FALSE;
        break;
      case 'v':
        print_version();
        return FALSE;
        break;
      case 'e':
        render_image_acolor = get_color_map_from_string (g_ascii_strdown(optarg,strlen(optarg)));
        trigger_fragment_analysis = (render_image_acolor > 2) ? TRUE : FALSE;
        trigger_molecule_analysis = (render_image_acolor > 3) ? TRUE : FALSE;
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 't':
        render_image_pcolor = get_color_map_from_string (g_ascii_strdown(optarg,strlen(optarg)));
        trigger_fragment_analysis = (render_image_pcolor > 2) ? TRUE : FALSE;
        trigger_molecule_analysis = (render_image_pcolor > 3) ? TRUE : FALSE;
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'l':
        atomes_from_libreoffice = TRUE;
        render_image_format = 0;
        break;
      case 'p':
        atomes_render_image = TRUE;
        render_image_format = 0;
        img_opt ++;
        break;
      case 'j':
        atomes_render_image = TRUE;
        render_image_format = 1;
        img_opt ++;
        break;
      case 'o':
        render_image_output = g_strdup_printf ("%s", optarg);
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'W':
        image_x = g_strdup_printf ("%s", optarg);
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'H':
        image_y = g_strdup_printf ("%s", optarg);
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 's':
        render_image_style = get_style_from_string (g_ascii_strdown(optarg,strlen(optarg)));
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'a':
        render_image_axis = get_box_axis_from_string (g_ascii_strdown(optarg,strlen(optarg)));
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'b':
        render_image_box = get_box_axis_from_string (g_ascii_strdown(optarg,strlen(optarg)));
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'r':
        render_image_rep = get_rep_from_string (g_ascii_strdown(optarg,strlen(optarg)));
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'C':
        render_image_box_color = get_color_from_hexa_string (optarg);
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'B':
        render_image_back_color = get_color_from_hexa_string (optarg);
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'G':
        render_image_back_grad = get_gradient_from_string (g_ascii_strdown(optarg,strlen(optarg)));
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'D':
        if (isdigit(optarg[0]))
        {
          render_image_back_dir = ((optarg[0] - '0') < 9) ? optarg[0] - '0' : NONE;
        }
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'P':
        double v = string_to_double(optarg);
        render_image_back_pos = (v >= 0.0 && v <= 1.0) ? v : NONE;
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'U':
        render_image_grad_color[0] = get_color_from_hexa_string (optarg);
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
      case 'V':
        render_image_grad_color[1] = get_color_from_hexa_string (optarg);
        img_opt ++;
        img_opt += (index == -1) ? 1 : 0;
        break;
    }
    index = -1;
  }
  if (atomes_from_libreoffice) atomes_render_image = FALSE;
  if (atomes_render_image)
  {
    if (! render_image_output) render_image_output = g_strdup_printf ("%s", (render_image_format) ? "image.jpg" : "image.png");
    if (argc == img_opt + 2)
    {
      if (image_x || image_y)
      {
        render_image_pixels = allocint(2);
        int pix;
        if (image_x)
        {
          pix = (int) string_to_double(image_x);
          if (pix > 0) render_image_pixels[0] = pix;
        }
        if (image_y)
        {
          pix = (int) string_to_double(image_y);
          if (pix > 0) render_image_pixels[1] = pix;
        }
      }
    }
    else
    {
      return FALSE;
    }
  }

  for (i=optind; i<argc; i++)
  {
    j = test_this_arg (argv[i]);
    if (! (abs(j) == 1 && with_workspace))
    {
      if (j < 0)
      {
        if (! flist)
        {
          flist = g_malloc0(sizeof*flist);
          ftmp = flist;
        }
        else
        {
          ftmp -> next = g_malloc0(sizeof*ftmp -> next);
          ftmp = ftmp -> next;
        }
        ftmp -> file_name = g_strdup_printf ("%s", argv[i]);
        ftmp -> file_type = -j;
        files_to_read ++;
        if (atomes_from_libreoffice) projfile = g_strdup_printf ("%s", argv[i]);
      }
    }
    else if (j == -1)
    {
      // Other workspace are ignored
      i ++;
    }
  }

  return (atomes_render_image && files_to_read == 1) ? TRUE : (atomes_render_image) ? FALSE : TRUE;
}

/*!
  \fn gboolean destroy_func (gpointer user_data)

  \brief destroy splash screen

  \param user_data the splash screen to destroy
*/
gboolean destroy_func (gpointer user_data)
{
  GtkWidget * splashi = (GtkWidget*) user_data;
  destroy_this_widget (splashi);
  return FALSE;
}

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT gboolean splashdraw (GtkWidget * widget, cairo_t * cr, gpointer data)

  \brief draw splash screen

  \param widget the GtkWidget sending the signal
  \param cr the cairo drawing context
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean splashdraw (GtkWidget * widget, cairo_t * cr, gpointer data)
{
  cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0); /* transparent */
  cairo_paint (cr);
  return FALSE;
}
#endif

/*!
  \fn GtkWidget * create_splash_window ()

  \brief create splash screen window
*/
GtkWidget * create_splash_window ()
{
  GtkWidget * splash_window = new_gtk_window ();
  gtk_window_set_decorated (GTK_WINDOW (splash_window), FALSE);
  GtkWidget * image;
#ifdef GTK4
  image = gtk_picture_new_for_filename (PACKAGE_LOGO);
  gchar * backcol = g_strdup_printf ("window#splash {\n"
                                     "  background: none;"
                                     "  background-color: rgba(255, 255, 255, 0);}");
  provide_gtk_css (backcol);
  gtk_widget_set_name (splash_window, "splash");
  g_free (backcol);
#else
  gtk_window_set_type_hint (GTK_WINDOW (splash_window), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
  gtk_window_set_position (GTK_WINDOW (splash_window), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_widget_set_app_paintable (splash_window, TRUE);
  GdkScreen * screen = gtk_widget_get_screen (splash_window);
  GdkVisual * visual = gdk_screen_get_rgba_visual (screen);
  gtk_widget_set_visual (splash_window, visual);
  // Next line might be optional for total transparency
  g_signal_connect(G_OBJECT(splash_window), "draw", G_CALLBACK(splashdraw), NULL);
  image = gtk_image_new_from_file (PACKAGE_LOGO);
#endif
  add_container_child (CONTAINER_WIN, splash_window, image);
  gtk_window_set_transient_for ((GtkWindow *)splash_window, (GtkWindow *)MainWindow);
  show_the_widgets (splash_window);
  return splash_window;
}

/*!
  \fn void read_this_file (int file_type, gchar * this_file)

  \brief read file from the command line

  \param file_type File type
  \param this_file File name
*/
void read_this_file (int file_type, gchar * this_file)
{
  FILE * fp = fopen (this_file, dfi[0]);
  if (file_type == 1)
  {
    int i = open_save_workspace (fp, 0);
    if (i != 0)
    {
      gchar * err = g_strdup_printf ("Error while reading workspace file\n%s\nError code: %d\n", this_file, i);
      show_error (err, 0, MainWindow);
      g_free (err);
    }
  }
  else
  {
    init_project (FALSE);
    reading_project = TRUE;
    open_save (fp, 0, 0, activep, activep, this_file);
    reading_project = FALSE;
  }
  fclose (fp);
}

/*!
  \fn void open_this_data_file (int file_type, gchar * file_name)

  \brief open data file from the command line

  \param file_type File type
  \param file_name File name
*/
void open_this_data_file (int file_type, gchar * file_name)
{
  gchar * end;
  gchar * str;
  gchar * filedir;
  int i;
#ifdef G_OS_WIN32
  WIN32_FIND_DATA ffd;
  HANDLE hFind;
#else
  DIR * d;
  struct dirent * dir;
#endif
  switch (file_type)
  {
    case 1:
      read_this_file (1, file_name);
      break;
    case 2:
      end = g_strdup_printf ("%c", file_name[strlen(file_name)-1]);
      if (g_strcmp0 (file_name, "*") == 0 || g_strcmp0 (end, "*") == 0)
      {
        if (g_strcmp0 (file_name, "*") == 0)
        {
          filedir = g_strdup_printf ("./");
        }
        else
        {
          filedir = g_strdup_printf ("%c", file_name[0]);
          for (i=1; i<strlen(file_name)-1; i++) filedir = g_strdup_printf ("%s%c", filedir, file_name[i]);
        }
#ifdef G_OS_WIN32
        hFind = FindFirstFile (filedir, & ffd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
          if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
          {
            str = g_strdup_printf ("%s\\%s", filedir, (gchar *)ffd.cFileName);
            read_this_file (2, str);
            g_free (str);
          }
          while (FindNextFile(hFind, &ffd) != 0)
          {
            if (ffd.dwFileAttributes & ! FILE_ATTRIBUTE_DIRECTORY)
            {
              str = g_strdup_printf ("%s\\%s", filedir, (gchar *)ffd.cFileName);
              read_this_file (2, str);
              g_free (str);
            }
          }
        }
        FindClose(hFind);
#else
        d = opendir (filedir);
        if (d)
        {
          while ((dir = readdir(d)) != NULL)
          {
            if (dir -> d_type == DT_REG)
            {
              str = g_strdup_printf ("%s/%s", filedir, dir -> d_name);
              read_this_file (2, str);
              g_free (str);
            }
          }
          closedir(d);
        }
#endif
        g_free (filedir);
      }
      else
      {
        read_this_file (2, file_name);
      }
      break;
    case 15:
      init_project (TRUE);
      open_this_isaacs_xml_file (g_strdup_printf ("%s", file_name), activep, FALSE);
      break;
    default:
      end = g_strdup_printf ("%c", file_name[strlen(file_name)-1]);
      if (g_strcmp0 (file_name, "*") == 0 || g_strcmp0 (end, "*") == 0)
      {
        if (g_strcmp0 (file_name, "*") == 0)
        {
          filedir = g_strdup_printf ("./");
        }
        else
        {
          filedir = g_strdup_printf ("%c", file_name[0]);
          for (i=1; i<strlen(file_name)-1; i++) filedir = g_strdup_printf ("%s%c", filedir, file_name[i]);
        }
#ifdef G_OS_WIN32
        hFind = FindFirstFile (filedir, & ffd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
          if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
          {
            init_project (TRUE);
            active_project -> coordfile =  g_strdup_printf ("%s\\%s", filedir, (gchar *)ffd.cFileName);
            open_this_coordinate_file (file_type-3, NULL);
          }
          while (FindNextFile(hFind, &ffd) != 0)
          {
            if (ffd.dwFileAttributes & ! FILE_ATTRIBUTE_DIRECTORY)
            {
              init_project (TRUE);
              active_project -> coordfile = g_strdup_printf ("%s\\%s", filedir, (gchar *)ffd.cFileName);
              open_this_coordinate_file (file_type-3, NULL);
            }
          }
        }
        FindClose(hFind);
#else
        d = opendir (filedir);
        if (d)
        {
          while ((dir = readdir(d)) != NULL)
          {
            if (dir -> d_type == DT_REG)
            {
              init_project (TRUE);
              active_project -> coordfile = g_strdup_printf ("%s/%s", filedir, dir -> d_name);
              open_this_coordinate_file (file_type-3, NULL);
            }
          }
          closedir(d);
        }
#endif
        g_free (filedir);
      }
      else
      {
        init_project (TRUE);
        active_project -> coordfile = g_strdup_printf ("%s", file_name);
        open_this_coordinate_file (file_type-3, NULL);
      }
      break;
  }
}

/*!
  \fn G_MODULE_EXPORT void run_program (GApplication * app, gpointer data)

  \brief run the program

  \param app the application to run
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_program (GApplication * app, gpointer data)
{
  GtkSettings * default_settings = gtk_settings_get_default ();
/*
#ifndef G_OS_WIN32
  g_object_set (default_settings, "gtk-application-prefer-dark-theme", TRUE, NULL);
#endif
*/
#ifdef GTK3
  g_object_set (default_settings, "gtk-button-images", TRUE, NULL);
#endif
#ifdef G_OS_WIN32
#ifdef GTK3
  g_object_set (default_settings, "gtk-key-theme-name", "win32", NULL);
#endif
  dfi[0]="rb";
  dfi[1]="wb";
#else
  dfi[0]="r";
  dfi[1]="w";
#endif

#ifdef MAC_INTEGRATION
  GtkosxApplication * ProgOSX;
  ProgOSX = g_object_new (GTKOSX_TYPE_APPLICATION, NULL);
  gtkosx_application_set_use_quartz_accelerators (ProgOSX, FALSE);
  gtkosx_application_ready (ProgOSX);
#endif
#ifdef DEBUG
  print_version ();
#endif // DEBUG

  if (! atomes_render_image)
  {
    MainWindow = create_main_window (app);
    GtkWidget * isplash = create_splash_window ();
    if (isplash == NULL)
    {
      g_print ("Impossible to load the splash screen\n");
    }
    else
    {
      g_timeout_add_seconds (1, destroy_func, isplash);
    }
  }
  if (flist)
  {
    ftmp = flist;
    silent_input = TRUE;
    if (with_workspace)
    {
      while (ftmp)
      {
        if (ftmp -> file_type == 1)
        {
          // Open the workspace
          open_this_data_file (ftmp -> file_type, ftmp -> file_name);
        }
        ftmp = ftmp -> next;
      }
    }
    ftmp = flist;
    while (ftmp)
    {
      if (ftmp -> file_type != 1)
      {
        // Add project(s) to workspace
        open_this_data_file (ftmp -> file_type, ftmp -> file_name);
      }
      ftmp = ftmp -> next;
    }
    g_free (flist);
    flist = NULL;
    silent_input = FALSE;
  }
#ifdef MAC_INTEGRATION
  g_object_unref (ProgOSX);
#endif
}

/*!
  \fn int check_opengl_rendering ()

  \brief check the initialization parameters for an OpenGL context
*/
int check_opengl_rendering ()
{
  GError * error = NULL;
  gchar * proc_dir = NULL;
  gchar * proc_path = NULL;
  const char * proc_name;
#ifdef G_OS_WIN32
  proc_dir = g_build_filename (PACKAGE_PREFIX, "bin", NULL);
  proc_name = "atomes_startup_testing.exe";
  proc_path = g_build_filename (proc_dir, proc_name, NULL);
#else
  proc_dir = g_build_filename (PACKAGE_LIBEXEC, NULL);
  proc_name = "atomes_startup_testing";
  proc_path = g_build_filename (PACKAGE_LIBEXEC, proc_name, NULL);
#endif
#ifdef DEBUG
  g_print ("proc_dir= %s\n", proc_dir);
  g_print ("proc_name= %s\n", proc_name);
  g_print ("proc_path= %s\n", proc_path);
#endif

#ifdef CODEBLOCKS
  GSubprocess * proc = g_subprocess_new (G_SUBPROCESS_FLAGS_NONE, & error, proc_path, NULL);
#else
#ifndef OSX
  GSubprocessLauncher * proc_launch = g_subprocess_launcher_new (G_SUBPROCESS_FLAGS_NONE);
  g_subprocess_launcher_set_cwd (proc_launch, proc_dir);
  GSubprocess * proc = g_subprocess_launcher_spawn (proc_launch, & error, proc_path, NULL);
#else
  GSubprocess * proc = g_subprocess_new (G_SUBPROCESS_FLAGS_NONE, & error, proc_path, NULL);
#endif
#endif

  if (error)
  {
    g_print ("error: %s\n", error -> message);
    g_clear_error (& error);
  }
  g_subprocess_wait (proc, NULL, & error);
  int res = g_subprocess_get_exit_status (proc);
#ifdef DEBUG
  g_debug ("Exit status of atomes_startup_testing = %d", res);
#endif
  g_clear_object (& proc);
#ifndef CODEBLOCKS
#ifndef OSX
  g_clear_object (& proc_launch);
#endif
#endif
  g_free (proc_path);
  g_free (proc_dir);
  gchar * ogl_info = NULL;
  switch (res)
  {
    case 1:
      ogl_info = g_strdup_printf ("Fatal error on OpenGL initialization: trying to adjust environment !");
      break;
    case -2:
      ogl_info = g_strdup_printf ("Impossible to initialize the OpenGL 3D rendering !");
      break;
    case -1:
      ogl_info = g_strdup_printf ("GDK visual must be modified to initialize the OpenGL context !");
      break;
    default:
      break;
  }
  if (ogl_info)
  {
    g_print ("%s\n", ogl_info);
    g_free (ogl_info);
    ogl_info = NULL;
  }
  return res;
}

/*!
  \fn int main (int argc, char *argv[])

  \brief initialization of the atomes program

  \param argc number of argument(s) on the command line
  \param *argv[] list of argument(s) on the command line
*/
int main (int argc, char *argv[])
{
  gboolean RUNC = FALSE;

#ifdef G_OS_WIN32
  PACKAGE_PREFIX = g_win32_get_package_installation_directory_of_module (NULL);
  // g_win32_get_package_installation_directory (NULL, NULL);
#endif
  PACKAGE_LIB_DIR = g_build_filename (PACKAGE_PREFIX, "library", NULL);
  PACKAGE_IMP = g_build_filename (PACKAGE_PREFIX, "pixmaps/import.png", NULL);
  PACKAGE_IMP = g_build_filename (PACKAGE_PREFIX, "pixmaps/import.png", NULL);
  PACKAGE_CON = g_build_filename (PACKAGE_PREFIX, "pixmaps/convert.png", NULL);
  PACKAGE_IMG = g_build_filename (PACKAGE_PREFIX, "pixmaps/image.png", NULL);
  PACKAGE_PDF = g_build_filename (PACKAGE_PREFIX, "pixmaps/pdf.png", NULL);
  PACKAGE_SVG = g_build_filename (PACKAGE_PREFIX, "pixmaps/svg.png", NULL);
  PACKAGE_EPS = g_build_filename (PACKAGE_PREFIX, "pixmaps/eps.png", NULL);
  PACKAGE_PNG = g_build_filename (PACKAGE_PREFIX, "pixmaps/png.png", NULL);
  PACKAGE_JPG = g_build_filename (PACKAGE_PREFIX, "pixmaps/jpg.png", NULL);
  PACKAGE_BMP = g_build_filename (PACKAGE_PREFIX, "pixmaps/bmp.png", NULL);
  PACKAGE_TIFF = g_build_filename (PACKAGE_PREFIX, "pixmaps/tiff.png", NULL);
  PACKAGE_VOID = g_build_filename (PACKAGE_PREFIX, "pixmaps/void.png", NULL);
  PACKAGE_TD = g_build_filename (PACKAGE_PREFIX, "pixmaps/td.png", NULL);
  PACKAGE_MOL = g_build_filename (PACKAGE_PREFIX, "pixmaps/molecule.png", NULL);
  PACKAGE_OGL = g_build_filename (PACKAGE_PREFIX, "pixmaps/opengl.png", NULL);
  PACKAGE_OGLM = g_build_filename (PACKAGE_PREFIX, "pixmaps/mol.png", NULL);
  PACKAGE_OGLC = g_build_filename (PACKAGE_PREFIX, "pixmaps/mol.png", NULL);
  PACKAGE_PRO = g_build_filename (PACKAGE_PREFIX, "pixmaps/prop.png", NULL);
  PACKAGE_SET = g_build_filename (PACKAGE_PREFIX, "pixmaps/settings.png", NULL);
  PACKAGE_LOGO = g_build_filename (PACKAGE_PREFIX, "pixmaps/logo.png", NULL);
  PACKAGE_LAGPL = g_build_filename (PACKAGE_PREFIX, "pixmaps/logo-agpl.png", NULL);
  PACKAGE_LABOUT = g_build_filename (PACKAGE_PREFIX, "pixmaps/logo-about.png", NULL);
  PACKAGE_DOTA = g_build_filename (PACKAGE_PREFIX, "pixmaps/dots/dots-a.png", NULL);
  PACKAGE_DOTB = g_build_filename (PACKAGE_PREFIX, "pixmaps/dots/dots-b.png", NULL);
  PACKAGE_DOTC = g_build_filename (PACKAGE_PREFIX, "pixmaps/dots/dots-c.png", NULL);
  PACKAGE_DOTD = g_build_filename (PACKAGE_PREFIX, "pixmaps/dots/dots-d.png", NULL);
  PACKAGE_DOTE = g_build_filename (PACKAGE_PREFIX, "pixmaps/dots/dots-e.png", NULL);
  PACKAGE_DOTF = g_build_filename (PACKAGE_PREFIX, "pixmaps/dots/dots-f.png", NULL);
  PACKAGE_DOTG = g_build_filename (PACKAGE_PREFIX, "pixmaps/dots/dots-g.png", NULL);
  PACKAGE_DOTH = g_build_filename (PACKAGE_PREFIX, "pixmaps/dots/dots-h.png", NULL);
  PACKAGE_DFBD = g_build_filename (PACKAGE_PREFIX, "pixmaps/field/bd.png", NULL);
  PACKAGE_DFAN = g_build_filename (PACKAGE_PREFIX, "pixmaps/field/an.png", NULL);
  PACKAGE_DFDI = g_build_filename (PACKAGE_PREFIX, "pixmaps/field/di.png", NULL);
  PACKAGE_DFTD = g_build_filename (PACKAGE_PREFIX, "pixmaps/field/td.png", NULL);
  PACKAGE_DFIN = g_build_filename (PACKAGE_PREFIX, "pixmaps/field/in.png", NULL);
  PACKAGE_SGCP = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Cubic-P.png", NULL);
  PACKAGE_SGCI = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Cubic-I.png", NULL);
  PACKAGE_SGCF = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Cubic-F.png", NULL);
  PACKAGE_SGHP = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Hexagonal.png", NULL);
  PACKAGE_SGTR = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Trigonal-R.png", NULL);
  PACKAGE_SGTI = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Tetragonal-I.png", NULL);
  PACKAGE_SGTP = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Tetragonal-P.png", NULL);
  PACKAGE_SGOP = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Orthorhombic-P.png", NULL);
  PACKAGE_SGOI = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Orthorhombic-I.png", NULL);
  PACKAGE_SGOC = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Orthorhombic-C.png", NULL);
  PACKAGE_SGOF = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Orthorhombic-F.png", NULL);
  PACKAGE_SGMP = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Monoclinic-P.png", NULL);
  PACKAGE_SGMI = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Monoclinic-I.png", NULL);
  PACKAGE_SGTC = g_build_filename (PACKAGE_PREFIX, "pixmaps/bravais/Triclinic.png", NULL);

  switch (argc)
  {
    case 1:
      RUNC = TRUE;
      break;
    default:
      RUNC = parse_command_line (argc, argv);
      break;
  }

  if (RUNC)
  {
#ifdef G_OS_WIN32
#ifndef DEBUG
    FreeConsole ();
#endif
#endif
    atomes_visual = check_opengl_rendering ();
    if (atomes_visual == 1)
    {
      // OpenGL initialization error, try adapting environment
      g_setenv ("GSK_RENDERER", "gl", TRUE);
      g_setenv ("GDK_DEBUG", "gl-prefer-gl", TRUE);
      atomes_visual = check_opengl_rendering ();
      if (atomes_visual == 1)
      {
        // OpenGL initialization error, again try adapting environment
        g_setenv ("GDK_RENDERER", "ngl", TRUE);
        atomes_visual = check_opengl_rendering ();
      }
    }
    if (atomes_visual > 0 || atomes_visual == -2)
    {
      // No way to initialize an OpenGL context: must quit
      return 1;
    }
#ifdef OSX
    g_setenv ("GSK_RENDERER", "gl", TRUE);
#endif
    atomes_visual = ! (abs(atomes_visual));

#ifdef G_OS_WIN32
    PWSTR localPath = NULL;
    HRESULT hr = SHGetKnownFolderPath (& FOLDERID_LocalAppData, 0, NULL, & localPath);
    if (FAILED(hr))
    {
      fprintf (stderr, "Error impossible to obtain the AppData\\Roaming (code 0x%08lx)\n%s", hr);
      ATOMES_CONFIG_DIR = NULL;
      ATOMES_CONFIG = NULL;
    }
    else
    {
      char appdata[MAX_PATH];
      wcstombs (appdata, localPath, MAX_PATH);
      CoTaskMemFree (localPath);  // libérer mémoire retournée par SHGetKnownFolderPath
      // Build the folder path for atomes
      ATOMES_CONFIG_DIR = g_strdup_printf ("%s\\atomes", appdata);
      ATOMES_CONFIG = g_strdup_printf ("%s\\atomes.pml", ATOMES_CONFIG_DIR);
    }
#else
    struct passwd * pw = getpwuid(getuid());
    ATOMES_CONFIG_DIR = g_strdup_printf ("%s/.config/atomes", pw -> pw_dir);
    ATOMES_CONFIG = g_strdup_printf ("%s/atomes.pml", ATOMES_CONFIG_DIR);
#endif
    set_atomes_preferences ();
    // setlocale(LC_ALL,"en_US");
    gtk_disable_setlocale ();
#if GLIB_MINOR_VERSION < 74
    AtomesApp = gtk_application_new (g_strdup_printf ("fr.ipcms.atomes.prog-%d", (int)clock()), G_APPLICATION_FLAGS_NONE);
#else
    AtomesApp = gtk_application_new (g_strdup_printf ("fr.ipcms.atomes.prog-%d", (int)clock()), G_APPLICATION_DEFAULT_FLAGS);
#endif
    GError * error = NULL;
    g_application_register (G_APPLICATION(AtomesApp), NULL, & error);
    g_signal_connect (G_OBJECT(AtomesApp), "activate", G_CALLBACK(run_program), NULL);
    int status = g_application_run (G_APPLICATION (AtomesApp), 0, NULL);
    g_object_unref (AtomesApp);
    return status;
  }
  return 0;
}
