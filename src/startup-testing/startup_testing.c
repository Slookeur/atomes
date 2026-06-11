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
* @file startup_testing.c
* @short Startup testing of OpenGL configuration for the atomes program
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#ifdef GTK3
#  include <gtk/gtkx.h>
#endif

#ifdef G_OS_WIN32
#  include <windef.h>
#  include <windows.h>
#  include <epoxy/gl.h>
#  include <GL/glu.h>
#else
#  include <epoxy/gl.h>
#  include <GL/glu.h>
#  ifdef OSX
#    include <GL/glx.h>
#  else
#    include <epoxy/glx.h>
#  endif
#endif

#ifdef G_OS_WIN32
#define APP_EXTENSION ".exe"
#else
#include <pwd.h>
#define APP_EXTENSION
#endif

int opengl_visual = 1; // OpenGL visual: 1 = GTK default, 0 = X11 default (GTK3 + GtkGLArea only), -1 = no OpenGL

/*!
  \fn void printhelp()

  \brief print basic help
*/
void printhelp()
{
  char * help    = "\nUsage: ATOMES_OPENGL_TESTING\n\n"
                   " atomes OpenGL setup utility\n\n"
                   "options:\n"
                   "  -v, --version             version information\n"
                   "  -h, --help                display this help message\n\n";
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
  char scanid[80]="\natomes OpenGL setup utility\n";
  char bug[20] = "\nReport a bug to <";
  char eh[4] = ">\n";

  printf ("%s", scanid);
  printf ("\n%s version         : %s\n", PACKAGE, VERSION);
  printf ("\nGTK+ version           : %1d.%1d.%1d\n",
          GTK_MAJOR_VERSION,
          GTK_MINOR_VERSION,
          GTK_MICRO_VERSION);
  printf ("%s", bug);
  printf ("%s", PACKAGE_BUGREPORT);
  printf ("%s\n", eh);
}

/*!
  \fn void show_the_widgets (GtkWidget * widg)

  \brief show GtkWidget

  \param widg the GtkWidget to show
*/
void show_the_widgets (GtkWidget * widg)
{
  if (widg)
  {
    if (GTK_IS_WIDGET(widg))
    {
#ifdef GTK4
      gtk_widget_set_visible (widg, TRUE);
#else
      gtk_widget_show_all (widg);
#endif
    }
  }
}

/*!
  \fn void hide_the_widgets (GtkWidget * widg)

  \brief hide GtkWidget

  \param widg the GtkWidget to show
*/
void hide_the_widgets (GtkWidget * widg)
{
  if (widg)
  {
    if (GTK_IS_WIDGET(widg))
    {
#ifdef GTK4
      gtk_widget_set_visible (widg, FALSE);
#else
      gtk_widget_hide (widg);
#endif
    }
  }
}

/*!
  \fn GtkWidget * destroy_this_widget (GtkWidget * widg)

  \brief destroy a GtkWidget

  \param widg the GtkWidget to destroy
*/
GtkWidget * destroy_this_widget (GtkWidget * widg)
{
  hide_the_widgets (widg);
#ifdef GTK3
  if (widg != NULL)
  {
    if (GTK_IS_WIDGET(widg))
    {
      gtk_widget_destroy (widg);
    }
  }
#endif // GTK3
  return NULL;
}

/*!
  \fn gboolean is_GLExtension_Supported (const char * extension)

  \brief test if this GLExtension is support

  \param extension the target GLExtension
*/
gboolean is_GLExtension_Supported (const char * extension)
{
  int i, j;
  i = j = 0;
  glGetIntegerv (GL_NUM_EXTENSIONS, & i);
  for (j=0; j<i; j++)
  {
    if (g_strcmp0 (extension, (const char*)glGetStringi(GL_EXTENSIONS, j)) == 0) return TRUE;
  }
  return FALSE;
}

/*!
  \fn void init_opengl ()

  \brief initialize OpenGL rendering parameters
*/
void init_opengl ()
{
  glEnable (GL_DEPTH_TEST);
  glDepthMask (GL_TRUE);
  glDepthFunc (GL_LEQUAL);
  glDepthRange (0.0f, 1.0f);
  glClearDepth (1.0f);
  glEnable (GL_NORMALIZE);

  glShadeModel(GL_SMOOTH);
  glCullFace(GL_BACK);
  glEnable (GL_CULL_FACE);                         // Incompatible with polyhedra viz

  glEnable (GL_COLOR_SUM_EXT);

  glEnable (GL_PROGRAM_POINT_SIZE);
  glEnable (GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable (GL_POINT_SPRITE);

  glEnable (GL_POINT_SMOOTH);
  glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
  glEnable (GL_LINE_SMOOTH);                       // Lines antialiasing
  glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);

  glDisable (GL_POLYGON_SMOOTH);                   // To disable ploygon antialiasing
  glEnable (GL_POLYGON_STIPPLE);
  glEnable (GL_POLYGON_OFFSET_FILL);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonOffset (1.0, 1.0);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

  glHint (GL_FOG_HINT, GL_NICEST);
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations
}

/*!
  \fn GError * init_gtk_gl_area (GtkGLArea * area)

  \brief initialize a GtkGLArea, return error if any

  \param area the GtkGLArea point to initialize
*/
GError * init_gtk_gl_area (GtkGLArea * area)
{
  if (area == NULL)
  {
    area = (GtkGLArea *)gtk_gl_area_new ();
  }
  else
  {
    gtk_gl_area_make_current (area);
  }
  gtk_gl_area_set_has_depth_buffer (area, TRUE);
  gtk_gl_area_set_has_stencil_buffer (area, TRUE);
  return gtk_gl_area_get_error (area);
}

#ifdef GTKGLAREA
/*!
  \fn G_MODULE_EXPORT void on_realize (GtkGLArea * area, gpointer data)

  \brief realize event for a GtkGLArea

  \param area the GtkGLArea sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_realize (GtkGLArea * area, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void on_realize (GtkWidget * widg, gpointer data)

  \brief realize event for the OpenGL widget

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_realize (GtkWidget * widg, gpointer data)
#endif
{
  GError * err;
#ifdef GTKGLAREA
  err = init_gtk_gl_area (area);
  if (err == NULL)
  {
#else
  GdkWindow * xwin = gtk_widget_get_window (widg);
  GLint attr_list[] = {GLX_DOUBLEBUFFER,
                       GLX_RGBA,
                       GLX_DEPTH_SIZE, 1,
                       GLX_RED_SIZE,   1,
                       GLX_GREEN_SIZE, 1,
                       GLX_BLUE_SIZE,  1,
                       None};
  XVisualInfo * visualinfo = glXChooseVisual (GDK_WINDOW_XDISPLAY (xwin),
                                              gdk_screen_get_number (gdk_window_get_screen (xwin)), attr_list);
  GLXcontext * glcontext = glXCreateContext (GDK_WINDOW_XDISPLAY (xwin), visualinfo, NULL, TRUE);
  g_free (visualinfo);
  if (glXMakeCurrent (GDK_WINDOW_XDISPLAY (xwin), GDK_WINDOW_XID (xwin), glcontext))
  {
#endif
    init_opengl ();
  }
  else
  {
#ifdef GTK3
#ifdef GTKGLAREA
#ifndef G_OS_WIN32
    if (opengl_visual)
    {
      opengl_visual = 0;
      goto end;
    }
#endif
#endif
#endif
    opengl_visual = -1;
  }
#ifdef GTK3
#ifdef GTKGLAREA
#ifndef G_OS_WIN32
  end:;
#endif
#endif
#endif
}

#ifdef GTK3
#ifdef LINUX
/*!
  \fn void gtk_window_change_gdk_visual (GtkWidget * win)

  \brief change the Gdk visual

  \param win the GtkWidget sending the signal
*/
void gtk_window_change_gdk_visual (GtkWidget * win)
{
  // GTK+ > 3.15.1 uses an X11 visual optimized for GTK+'s OpenGL stuff
  // since revid dae447728d: https://github.com/GNOME/gtk/commit/dae447728d
  // However, in some cases it simply cannot start an OpenGL context.
  // This changes to the default X11 visual instead the GTK's default.
  GdkScreen * screen = gdk_screen_get_default ();
  GList * visuals = gdk_screen_list_visuals (screen);
  // printf("n visuals: %u\n", g_list_length(visuals));
  GdkX11Screen* x11_screen = GDK_X11_SCREEN (screen);
  g_assert (x11_screen != NULL);
  Visual * default_xvisual = DefaultVisual (GDK_SCREEN_XDISPLAY(x11_screen), GDK_SCREEN_XNUMBER(x11_screen));
  GdkVisual * default_visual = NULL;
  // int i = 0;
  while (visuals != NULL)
  {
    GdkVisual * visual = GDK_X11_VISUAL (visuals -> data);
    if (default_xvisual -> visualid == gdk_x11_visual_get_xvisual(GDK_X11_VISUAL (visuals -> data)) -> visualid)
    {
      // printf("Default visual %d\n", i);
      default_visual = visual;
    }
    // i++;
    visuals = visuals -> next;
  }
  gtk_widget_set_visual (win, default_visual);
}
#endif
#endif

/*!
  \fn GtkWidget * create_opengl_window (GApplication * app)

  \brief create the test OpenGL window

  \param app the target GApplication
*/
GtkWidget * create_opengl_window (GApplication * app)
{
  GtkWidget * win = gtk_application_window_new (GTK_APPLICATION(app));
#ifdef GTK3
#ifdef GTKGLAREA
#ifdef LINUX
  if (! opengl_visual) gtk_window_change_gdk_visual (win);
#endif
#endif
#endif
  GtkWidget * area;
#ifdef GTKGLAREA
    area = gtk_gl_area_new ();
#else
    area = gtk_drawing_area_new ();
#endif
#ifdef GTK3
  gtk_container_add (GTK_CONTAINER(win), area);
#else
  gtk_window_set_child (GTK_WINDOW(win), area);
#endif
  g_signal_connect (G_OBJECT (area), "realize", G_CALLBACK(on_realize), NULL);
  show_the_widgets (win);
  return win;
}

/*!
  \fn void test_opengl_window (GApplication * app)

  \brief test possibility to create an OpenGL context

  \param app the application
*/
void test_opengl_window (GApplication * app)
{
  GtkWidget * win = create_opengl_window (app);
#ifdef GTK3
#ifdef GTKGLAREA
#ifndef G_OS_WIN32
  if (! opengl_visual)
  {
    win = destroy_this_widget (win);
    win = create_opengl_window (app);
  }
#endif // G_OS_WIN32
#endif // GTKGLAREA
#endif // GTK3
  win = destroy_this_widget (win);
}

/*!
  \fn G_MODULE_EXPORT void run_opengl_test (GApplication * app, gpointer data)

  \brief run the OpenGL testing

  \param app the application to run
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_opengl_test (GApplication * app, gpointer data)
{
  test_opengl_window (app);
  g_application_quit (app);
}

int main (int argc, char * argv[])
{
  gboolean RUNC = TRUE;
  switch (argc)
  {
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
      break;
    default:
      break;
  }
#ifdef G_OS_WIN32
#ifndef DEBUG
  FreeConsole ();
#endif
#endif
  if (RUNC)
  {
    GtkApplication * TestOpenGLApp;
#if GLIB_MINOR_VERSION < 74
    TestOpenGLApp = gtk_application_new (g_strdup_printf ("fr.ipcms.atomes.ogl-%d", (int)clock()), G_APPLICATION_FLAGS_NONE);
#else
    TestOpenGLApp = gtk_application_new (g_strdup_printf ("fr.ipcms.atomes.ogl-%d", (int)clock()), G_APPLICATION_DEFAULT_FLAGS);
#endif
    GError * error = NULL;
    g_application_register (G_APPLICATION(TestOpenGLApp), NULL, & error);
    g_signal_connect (G_OBJECT(TestOpenGLApp), "activate", G_CALLBACK(run_opengl_test), NULL);
    int status = g_application_run (G_APPLICATION (TestOpenGLApp), 0, NULL);
    g_object_unref (TestOpenGLApp);
#ifdef DEBUG
    g_debug ("open_visual = %d, status= %d", opengl_visual, status);
#endif
    switch (opengl_visual)
    {
      case 0:
        // X11 default
        return -1;
        break;
      case -1:
        // Impossible
        return -2;
        break;
      default:
        // Gtk default (0) or impossible with fatal error (1)
        return status;
        break;
    }
  }
  return 0;
}
