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

  int test_this_arg (gchar * arg);
  int main (int argc, char *argv[]);

  gboolean destroy_func (gpointer user_data);

  G_MODULE_EXPORT gboolean splashdraw (GtkWidget * widget, cairo_t * cr, gpointer data);

  void printhelp();
  void printversion ();
  void read_this_file (int file_type, gchar * this_file);
  void open_this_data_file (int file_type, gchar * file_name);

  G_MODULE_EXPORT void run_program (GApplication * app, gpointer data);

  GtkWidget * create_splash_window ();

*/

#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "version.h"
#include "global.h"
#include "bind.h"
#include "callbacks.h"
#include "preferences.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"

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
  for (j=0; j<15; j++) if (g_strcmp0 (str, fext[j]) == 0) return j+1;
  gchar * aext = g_strdup_printf ("%c%c%c%c", str[i-4], str[i-3], str[i-2], str[i-1]);
  char * eext[15]={".awf", ".apf", ".xyz", "NULL", ".c3d", ".trj", "NULL", "tcar", "NULL", ".pdb", ".ent", ".cif", "NULL", "hist", ".ipf"};
  for (j=0; j<15; j++) if (g_strcmp0 (aext, eext[j]) == 0) return -(j+1);
  g_free (str);
  g_free (aext);
  return 0;
}

/*!
  \fn void printhelp()

  \brief print basic help
*/
void printhelp()
{
  char * help    = "\nUsage: ATOMES [OPTION]\n"
                   "       ATOMES [FILE]\n"
                   "       ATOMES [OPTION] [FILE]\n"
                   "       ATOMES [FILE1] [FILE2] ...\n"
                   "       ATOMES [OPTION1] [FILE1] [OPTION2] [FILE2] ...\n\n"
                   "3D atomistic model analysis, creation/edition and post-processing tool\n\n"
                   "options:\n"
                   "  -v, --version             version information\n"
                   "  -h, --help                display this help message\n\n"
                   "files, any number, in any order, in the following formats:\n\n"
                   "  Atomes workspace file: .awf\n"
                   "  Atomes prject file: .apf\n"
                   "  XYZ coordinates file: .xyz\n"
                   "  Chem3D coordinates file: .c3d\n"
                   "  CPMD trajectory: .trj\n"
                   "  VASP trajectory: .xdatcar\n"
                   "  PDB coordinates: .pdb, .ent\n"
                   "  Crystallographic Information File: .cif\n"
                   "  DL-POLY history file: .hist\n"
                   "  ISAACS project file: .ipf\n\n"
                   " alternatively specify the file format using:\n\n"
                   " -awf FILE\n"
                   " -apf FILE\n"
                   " -xyz FILE\n"
                   " -c3d FILE\n"
                   " -trj FILE\n"
                   " -xdatcar FILE\n"
                   " -pdb FILE, or, -ent FILE\n"
                   " -cif FILE\n"
                   " -hist FILE\n"
                   " -ipf FILE\n\n"
                   "ex:\n\n"
                   " atomes -pdb this.f file.awf -cif that.f *.xyz\n";
  char bug[20] = "\nReport a bug to <";
  char eh[4] = ">\n";

  printf("%s", help);
  printf("%s", bug);
  printf("%s", PACKAGE_BUGREPORT);
  printf("%s\n", eh);
}

/*!
  \fn void printversion ()

  \brief print version information
*/
void printversion ()
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
    open_save (fp, 0, activep, activep, 0, this_file);
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
  printversion ();
#endif // DEBUG
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
  PACKAGE_GR = g_build_filename (PACKAGE_PREFIX, "pixmaps/gr.png", NULL);
  PACKAGE_SQ = g_build_filename (PACKAGE_PREFIX, "pixmaps/sq.png", NULL);
  PACKAGE_BD = g_build_filename (PACKAGE_PREFIX, "pixmaps/bd.png", NULL);
  PACKAGE_AN = g_build_filename (PACKAGE_PREFIX, "pixmaps/an.png", NULL);
  PACKAGE_RI = g_build_filename (PACKAGE_PREFIX, "pixmaps/ri.png", NULL);
  PACKAGE_CH = g_build_filename (PACKAGE_PREFIX, "pixmaps/ch.png", NULL);
  PACKAGE_SP = g_build_filename (PACKAGE_PREFIX, "pixmaps/sp.png", NULL);
  PACKAGE_MS = g_build_filename (PACKAGE_PREFIX, "pixmaps/ms.png", NULL);
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

  int i, j, k;
  switch (argc)
  {
    case 1:
      RUNC=TRUE;
      break;
    case 2:
      if (g_strcmp0 (argv[1], "-h") == 0 || g_strcmp0 (argv[1], "--help") == 0)
      {
        printhelp();
        RUNC=FALSE;
      }
      else if (g_strcmp0 (argv[1], "-v") == 0 || g_strcmp0 (argv[1], "--version") == 0)
      {
        printversion();
        RUNC=FALSE;
      }
      else
      {
        RUNC=TRUE;
        i = test_this_arg (argv[1]);
        if (i !=0)
        {
          flist = g_malloc0(sizeof*flist);
          flist -> file_name = g_strdup_printf ("%s", argv[1]);
          flist -> file_type = -i;
          if (flist -> file_type == 1) with_workspace = TRUE;
        }
      }
      break;
    default:
      RUNC=TRUE;
      i=0;
      for (j=1; j<argc; j++)
      {
        k = test_this_arg (argv[j]);
        if (! (abs(k) == 1 && with_workspace))
        {
          if (k > 0 && j < argc-1)
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
            j ++;
          }
          else if (k < 0)
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
            ftmp -> file_name = g_strdup_printf ("%s", argv[j]);
            ftmp -> file_type = -k;
          }
          if (abs(k) == 1) with_workspace = TRUE;
        }
        else if (k == 1)
        {
          j ++;
        }
      }
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
