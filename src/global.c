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
* @file global.c
* @short Global variables declaration \n
         Global convenience function implementations
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'global.c'
*
* Contains:
*

 - global variables declarations
 - global convenience functions implementation

*
* List of functions:

  int * allocint (int  val);
  int ** allocdint (int xal, int yal);
  int *** alloctint (int xal, int yal, int zal);
  int **** allocqint (int wal, int xal, int yal, int zal);
  int * duplicate_int (int num, int * old_val);

  float * allocfloat (int  val);
  float ** allocdfloat (int xal, int yal);
  float *** alloctfloat (int xal, int yal, int zal);
  float * duplicate_float (int num, float * old_val);

  double * allocdouble (int val);
  double ** allocddouble (int xal, int yal);
  double *** alloctdouble (int xal, int yal, int zal);
  double **** allocqdouble (int wal, int xal, int yal, int zal);
  double * duplicate_double (int num, double * old_val);
  double string_to_double (gpointer string);
  double get_calc_time (struct timespec start, struct timespec stop);

  gboolean * allocbool (int  val);
  gboolean ** allocdbool (int xal, int yal);
  gboolean *** alloctbool (int xal, int yal, int zal);
  gboolean * duplicate_bool (int num, gboolean * old_val);

  gchar ** duplicate_strings (int num, gchar ** old_val);
  gchar * calculation_time (gboolean modelv, double ctime);

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define BILLION  1000000000L;

#ifdef G_OS_WIN32
gchar * PACKAGE_PREFIX = NULL;
gchar * PACKAGE_LIBEXEC = NULL;
#endif
gchar * PACKAGE_LIB_DIR = NULL;
gchar * PACKAGE_IMP = NULL;
gchar * PACKAGE_CON = NULL;
gchar * PACKAGE_IMG = NULL;
gchar * PACKAGE_PDF = NULL;
gchar * PACKAGE_SVG = NULL;
gchar * PACKAGE_EPS = NULL;
gchar * PACKAGE_PNG = NULL;
gchar * PACKAGE_JPG = NULL;
gchar * PACKAGE_BMP = NULL;
gchar * PACKAGE_TIFF = NULL;
gchar * PACKAGE_VOID = NULL;
gchar * PACKAGE_GR = NULL;
gchar * PACKAGE_SQ = NULL;
gchar * PACKAGE_BD = NULL;
gchar * PACKAGE_AN = NULL;
gchar * PACKAGE_RI = NULL;
gchar * PACKAGE_CH = NULL;
gchar * PACKAGE_SP = NULL;
gchar * PACKAGE_MS = NULL;
gchar * PACKAGE_TD = NULL;
gchar * PACKAGE_MOL = NULL;
gchar * PACKAGE_OGL = NULL;
gchar * PACKAGE_OGLM = NULL;
gchar * PACKAGE_OGLC = NULL;
gchar * PACKAGE_PRO = NULL;
gchar * PACKAGE_SET = NULL;
gchar * PACKAGE_LOGO = NULL;
gchar * PACKAGE_LAGPL = NULL;
gchar * PACKAGE_LABOUT = NULL;
gchar * PACKAGE_DOTA = NULL;
gchar * PACKAGE_DOTB = NULL;
gchar * PACKAGE_DOTC = NULL;
gchar * PACKAGE_DOTD = NULL;
gchar * PACKAGE_DOTE = NULL;
gchar * PACKAGE_DOTF = NULL;
gchar * PACKAGE_DOTG = NULL;
gchar * PACKAGE_DOTH = NULL;
gchar * PACKAGE_DFBD = NULL;
gchar * PACKAGE_DFAN = NULL;
gchar * PACKAGE_DFDI = NULL;
gchar * PACKAGE_DFTD = NULL;
gchar * PACKAGE_DFIN = NULL;
gchar * PACKAGE_SGCP = NULL;
gchar * PACKAGE_SGCI = NULL;
gchar * PACKAGE_SGCF = NULL;
gchar * PACKAGE_SGHP = NULL;
gchar * PACKAGE_SGTR = NULL;
gchar * PACKAGE_SGTI = NULL;
gchar * PACKAGE_SGTP = NULL;
gchar * PACKAGE_SGOP = NULL;
gchar * PACKAGE_SGOI = NULL;
gchar * PACKAGE_SGOC = NULL;
gchar * PACKAGE_SGOF = NULL;
gchar * PACKAGE_SGMP = NULL;
gchar * PACKAGE_SGMI = NULL;
gchar * PACKAGE_SGTC = NULL;
gchar * ATOMES_CONFIG_DIR = NULL;
gchar * ATOMES_CONFIG = NULL;
gchar * ATOMES_URL = "https://atomes.ipcms.fr";
gchar * mode_name[2]={"Analysis", "Edition"};
gchar * bravais_img[14];
gchar * ifield[8];
gchar * projfile = NULL;
char * ifbug="\nIf this is a bug please report it to:";

char * rings_type[5] = {"All (No rules)",
                        "King's",
                        "Guttman's",
                        "Primitive",
                        "Strong"};

char * untime[5] = {"fs",
                    "ps",
                    "ns",
                    "µs",
                    "ms"};

gchar * workspacefile = NULL;

int nprojects = 0;
int activep = 0;
int activev = 0;
int activef = 0;
int inactep = 0;
int activew = 0;
int statusval;
int atomes_visual = 1; // OpenGL visual: 1 = GTK default, 0 = X11 default (GTK3 + GtkGLArea only), Else = no OpenGL
// Loop Id for the dialogs
int dialog_id = -1;

int bonds_update = 0;
int frag_update = 0;
int mol_update = 0;

int tmp_pixels[2];
int * pasted_todo = NULL;

double * xsk = NULL;

GMainLoop * Event_loop[5];

gboolean in_movie_encoding = FALSE;
gboolean newspace = TRUE;
gboolean reading_input;
gboolean tmp_adv_bonding[2];
gboolean column_label = FALSE;
gboolean check_label = TRUE;
gboolean object_motion = FALSE;
gboolean selected_status = FALSE;
gboolean silent_input = FALSE;
gboolean cif_use_symmetry_positions = FALSE;

struct timespec start_time;
struct timespec stop_time;

double opac = 0.75;
double pi = 3.141592653589793238462643383279502884197;

GSimpleAction * edition_actions[3];
GSimpleAction * analyze_actions[9];

GtkApplication * AtomesApp = NULL;
GtkWidget * MainWindow = NULL;
GtkWidget * MainView = NULL;
GtkWidget * MainFrame[2];
GtkWidget * pop = NULL;
GtkWidget * curvetoolbox = NULL;
GtkWidget * progressbar = NULL;
GtkWidget * MainScrol[2];
GtkWidget * atomes_logo = NULL;
GtkWidget * atomes_shortcuts = NULL;
GtkWidget * calc_dialog = NULL;
GtkWidget * register_button = NULL;

GtkTextTag * tag = NULL;
GtkStatusbar * statusbar;

GdkPixbuf * THETD = NULL;
GdkPixbuf * THEMO = NULL;
GdkPixbuf * THEBD = NULL;
GdkPixbuf * SETTING = NULL;
GdkPixbuf * SETTINGS = NULL;
GdkPixbuf * OGL = NULL;
GdkPixbuf * OGLM = NULL;
GdkPixbuf * OGLC = NULL;
GdkPixbuf * RUN = NULL;

// Basic utility functions

/*!
  \fn int StringLength (char * str)

  \brief return the length of a string

  \param str string to measure
* /
int StringLength (char * str)
{
  int na, nb;
  char c;

  for (na = 0; (c = * str) != '\0'; ++ str, na ++);
  -- str;
  for (nb = na; (c = * str) == ' '; -- str, nb --);
  return nb;
}
*/

/*!
  \fn gboolean * allocbool (int  val)

  \brief allocate a gboolean * pointer

  \param val size of the pointer to allocate
*/
gboolean * allocbool (int  val)
{
  gboolean * var = NULL;

  var = g_malloc0 (val*sizeof*var);
  return var;
}

/*!
  \fn gboolean ** allocdbool (int xal, int yal)

  \brief allocate a gboolean ** pointer

  \param xal 1st dimension size of the pointer to allocate
  \param yal 2nd dimension size of the pointer to allocate
*/
gboolean ** allocdbool (int xal, int yal)
{
  gboolean ** var = NULL;
  int i;

  var = g_malloc (xal*sizeof*var);
  for ( i = 0 ; i < xal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = allocbool(yal);
  }
  return var;
}

/*!
  \fn gboolean *** alloctbool (int xal, int yal, int zal)

  \brief allocate a gboolean *** pointer

  \param xal 1st dimension size of the pointer to allocate
  \param yal 2nd dimension size of the pointer to allocate
  \param zal 3rd dimension size of the pointer to allocate
*/
gboolean *** alloctbool (int xal, int yal, int zal)
{
  gboolean *** var = NULL;
  int i;

  var = g_malloc (xal*sizeof*var);
  for ( i = 0 ; i < xal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = allocdbool(yal, zal);
  }
  return var;
}

/*!
  \fn int * allocint (int  val)

  \brief allocate an int * pointer

  \param val size of the pointer to allocate
*/
int * allocint (int  val)
{
  int * var = NULL;

  var = g_malloc0 (val*sizeof*var);
  return var;
}

/*!
  \fn int ** allocdint (int xal, int yal)

  \brief allocate an int ** pointer

  \param xal 1st dimension size of the pointer to allocate
  \param yal 2nd dimension size of the pointer to allocate
*/
int ** allocdint (int xal, int yal)
{
  int ** var = NULL;
  int i;

  var = g_malloc (xal*sizeof*var);
  for ( i = 0 ; i < xal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = allocint(yal);
  }
  return var;
}

/*!
  \fn int *** alloctint (int xal, int yal, int zal)

  \brief allocate an int *** pointer

  \param xal 1st dimension size of the pointer to allocate
  \param yal 2nd dimension size of the pointer to allocate
  \param zal 3rd dimension size of the pointer to allocate
*/
int *** alloctint (int xal, int yal, int zal)
{
  int *** var = NULL;
  int i;

  var = g_malloc (xal*sizeof*var);
  for ( i = 0 ; i < xal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = allocdint(yal, zal);
  }
  return var;
}

/*!
  \fn int **** allocqint (int wal, int xal, int yal, int zal)

  \brief allocate an int **** pointer

  \param wal 1st dimension size of the pointer to allocate
  \param xal 2nd dimension size of the pointer to allocate
  \param yal 3rd dimension size of the pointer to allocate
  \param zal 4th dimension size of the pointer to allocate
*/
int **** allocqint (int wal, int xal, int yal, int zal)
{
  int **** var = NULL;
  int i;

  var = g_malloc (wal*sizeof*var);
  for ( i = 0 ; i < wal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = alloctint(xal, yal, zal);
  }
  return var;
}

/*!
  \fn float * allocfloat (int  val)

  \brief allocate a float * pointer

  \param val size of the pointer to allocate
*/
float * allocfloat (int  val)
{
  float * var = NULL;

  var = g_malloc0 (val*sizeof*var);
  return var;
}

/*!
  \fn float ** allocdfloat (int xal, int yal)

  \brief allocate a float ** pointer

  \param xal 1st dimension size of the pointer to allocate
  \param yal 2nd dimension size of the pointer to allocate
*/
float ** allocdfloat (int xal, int yal)
{
  float ** var = NULL;
  int i;

  var = g_malloc (xal*sizeof*var);
  for ( i = 0 ; i < xal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = allocfloat(yal);
  }
  return var;
}

/*!
  \fn float *** alloctfloat (int xal, int yal, int zal)

  \brief allocate a float *** pointer

  \param xal 1st dimension size of the pointer to allocate
  \param yal 2nd dimension size of the pointer to allocate
  \param zal 3rd dimension size of the pointer to allocate
*/
float *** alloctfloat (int xal, int yal, int zal)
{
  float *** var = NULL;
  int i;

  var = g_malloc (xal*sizeof*var);
  for ( i = 0 ; i < xal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = allocdfloat (yal, zal);
  }
  return var;
}


/*!
  \fn double * allocdouble (int val)

  \brief allocate a double * pointer

  \param val size of the pointer to allocate
*/
double * allocdouble (int val)
{
  double * var = NULL;

  var = g_malloc0 (val*sizeof*var);
  return var;
}

/*!
  \fn double ** allocddouble (int xal, int yal)

  \brief allocate a double ** pointer

  \param xal 1st dimension size of the pointer to allocate
  \param yal 2nd dimension size of the pointer to allocate
*/
double ** allocddouble (int xal, int yal)
{
  double ** var = NULL;
  int i;

  var = g_malloc (xal*sizeof*var);
  for ( i = 0 ; i < xal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = allocdouble (yal);
  }
  return var;
}

/*!
  \fn double *** alloctdouble (int xal, int yal, int zal)

  \brief allocate a double *** pointer

  \param xal 1st dimension size of the pointer to allocate
  \param yal 2nd dimension size of the pointer to allocate
  \param zal 3rd dimension size of the pointer to allocate
*/
double *** alloctdouble (int xal, int yal, int zal)
{
  double *** var = NULL;
  int i;

  var = g_malloc (xal*sizeof*var);
  for ( i = 0 ; i < xal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = allocddouble (yal, zal);
  }
  return var;
}

/*!
  \fn double **** allocqdouble (int wal, int xal, int yal, int zal)

  \brief allocate a double **** pointer

  \param wal 1st dimension size of the pointer to allocate
  \param xal 2nd dimension size of the pointer to allocate
  \param yal 3rd dimension size of the pointer to allocate
  \param zal 4th dimension size of the pointer to allocate
*/
double **** allocqdouble (int wal, int xal, int yal, int zal)
{
  double **** var = NULL;
  int i;

  var = g_malloc (wal*sizeof*var);
  for ( i = 0 ; i < wal ; i ++ )
  {
    /* allocation d'un tableau de tableau */
    var[i] = alloctdouble (xal, yal, zal);
  }
  return var;
}

/*!
  \fn gchar ** duplicate_strings (int num, gchar ** old_val)

  \brief copy a list of strings

  \param num number of elements in the list
  \param old_val the list to copy
*/
gchar ** duplicate_strings (int num, gchar ** old_val)
{
  gchar ** new_val = g_malloc0 (num*sizeof*new_val);
  int i;
  for (i=0; i<num; i++) new_val[i] = g_strdup_printf ("%s", old_val[i]);
  return new_val;
}

/*!
  \fn int * duplicate_int (int num, int * old_val)

  \brief copy a list of int

  \param num number of elements in the list
  \param old_val the list to copy
*/
int * duplicate_int (int num, int * old_val)
{
  int * new_val = allocint (num);
  int i;
  for (i=0; i<num; i++) new_val[i] = old_val[i];
  return new_val;
}

/*!
  \fn gboolean * duplicate_bool (int num, gboolean * old_val)

  \brief copy a list of gboolean

  \param num number of elements in the list
  \param old_val the list to copy
*/
gboolean * duplicate_bool (int num, gboolean * old_val)
{
  gboolean * new_val = allocbool (num);
  int i;
  for (i=0; i<num; i++) new_val[i] = old_val[i];
  return new_val;
}

/*!
  \fn float * duplicate_float (int num, float * old_val)

  \brief copy a list of float

  \param num number of elements in the list
  \param old_val the list to copy
*/
float * duplicate_float (int num, float * old_val)
{
  float * new_val = allocfloat (num);
  int i;
  for (i=0; i<num; i++) new_val[i] = old_val[i];
  return new_val;
}

/*!
  \fn double * duplicate_double (int num, double * old_val)

  \brief copy a list of double

  \param num number of elements in the list
  \param old_val the list to copy
*/
double * duplicate_double (int num, double * old_val)
{
  double * new_val = allocdouble (num);
  int i;
  for (i=0; i<num; i++) new_val[i] = old_val[i];
  return new_val;
}


/*!
  \fn double string_to_double (gpointer string)

  \brief convert string to double

  \param string the string to convert
*/
double string_to_double (gpointer string)
{
  char * endPtr = NULL;
  double value = strtod ((char *)string, & endPtr);
  if (endPtr == (char *)string)
  {
     g_print ("Error in string format: string = \"%s\" - value == %lf\n", endPtr, value);
  }
  return value;
}

/*!
  \fn double get_calc_time (struct timespec start, struct timespec stop)

  \brief get calculation time in s

  \param start the initial time
  \param stop the final time
*/
double get_calc_time (struct timespec start, struct timespec stop)
{
  return (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec)/BILLION;
}

/*!
  \fn gchar * calculation_time (gboolean modelv, double ctime)

  \brief get calculation time, human readable

  \param modelv was an analysis performed ?
  \param ctime the calculation time
*/
gchar * calculation_time (gboolean modelv, double ctime)
{
  int i, j, k;
  gchar * t_string = (modelv) ? "\n \tAnalysis was performed in: ": "";
  if (ctime < 60.0)
  {
    return g_strdup_printf ("%s%f s", t_string, ctime);
  }
  else if (ctime < 3600.0)
  {
    i = ctime/60.0;
    return g_strdup_printf ("%s%d m %f s", t_string, i, ctime-i*60.0);
  }
  else if (ctime < 86400.0)
  {
    // Really ?!
    i = ctime/3600.0;
    j = (ctime - i*3600.0)/60.0;
    return g_strdup_printf ("%s%d h %d m %f s", t_string, i, j, ctime-i*3600.0-j*60.0);
  }
  else
  {
    // Seriously ?!
    i = ctime/86400.0;
    j = (ctime - i*86400.0)/3600.0;
    k = (ctime - i*86400.0 - j*3600.0)/60.0;
    return g_strdup_printf ("%s%d d %d h %d m %f s", t_string, i, j, k, ctime-i*86400.0-j*3600.0-k*60.0);
  }
}
