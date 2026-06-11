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
* @file callbacks.c
* @short General callbacks \n
         Functions to open and close files
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'callbacks.c'
*
* Contains:
*

 - General callbacks
 - The functions to open and close files

*
* List of functions:

  int signal_error (gchar * file, gchar * func, int error_line, int error_id);
  int open_save (FILE * fp, int act, int wid, int pid, int aid, gchar * pfile);
  int open_save_workspace (FILE * fp, int act);
  int prep_chem_data ();
  int to_read_trj_or_vas (int ff);
  int read_npt_data ();
  int open_coordinate_file (int id);

  void quit_gtk ();
  void update_error_trace (gchar * file, gchar * func, int trace_line);
  void open_this_proj (gpointer data, gpointer user_data);
  void run_project ();
  void apply_project (gboolean showtools);
  void open_this_isaacs_xml_file (gchar * profile, int ptoc, gboolean visible);
  void to_read_pos ();
  void check_read_sa ();
  void update_sa_info (int sid);
  void prepare_sp_box ();
  void cell_data_from_pdb_ (float * a, float * b, float * c, float * alp, float * bet, float * gam);
  void open_this_coordinate_file (int format, gchar * proj_name);

  G_MODULE_EXPORT void on_close_workspace (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void run_on_open_save_active (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_on_open_save_active (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void on_open_save_activate (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void on_save_as_activate (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void run_on_isaacs_port (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_on_isaacs_port (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void on_isaacs_port (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void update_sa (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void changed_spec_combo (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void update_at_sp (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void run_to_read_trj_or_vas (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_read_npt_data (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_read_npt_data (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_on_coord_port (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_on_coord_port (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void on_coord_port (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "callbacks.h"
#include "bind.h"
#include "project.h"
#include "workspace.h"
#include "glwindow.h"
#include "glview.h"
#include "atom_edit.h"
#include "cell_edit.h"
#include "readers.h"

char * coord_files[NCFORMATS+1] = {i18n("XYZ file"),
                                   i18n("XYZ file - NPT"),
                                   i18n("Chem3D file"),
                                   i18n("CPMD trajectory"),
                                   i18n("CPMD trajectory - NPT"),
                                   i18n("VASP trajectory"),
                                   i18n("VASP trajectory - NPT"),
                                   i18n("PDB file"),
                                   i18n("PDB file"),
                                   i18n("Cryst. information (crystal build) - single configuration"),
                                   i18n("Cryst. information (crystal build) - multiple configurations"),
                                   i18n("Cryst. information (symmetry positions) - single configuration"),
                                   i18n("DL-POLY HISTORY file"),
                                   i18n("ISAACS Project File")};

char * coord_files_ext[NCFORMATS+1]={"xyz", "xyz", "c3d", "trj", "trj", "xdatcar", "xdatcar",
                                    "pdb", "ent", "cif", "cif", "cif", "hist", "ipf"};

char ** las;

extern void simple_image_render();
extern G_MODULE_EXPORT void on_edit_activate (GtkWidget * widg, gpointer data);
extern gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif);
extern const gchar * dfi[2];
extern int open_cif_file (gchar * filename);
extern int open_coord_file (gchar * filename, int fti);
extern int open_history_file (gchar * filename);
extern int open_cell_file (int format, gchar * filename);
extern double get_z_from_periodic_table (gchar * lab);

/*!
  \fn G_MODULE_EXPORT void on_close_workspace (GtkWidget * widg, gpointer data)

  \brief close the active workspace

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_close_workspace (GtkWidget * widg, gpointer data)
{
  int i, j;
  gboolean close = FALSE;
  j = GPOINTER_TO_INT (data);
  if (j == 1)
  {
    close = ask_yes_no (_("Close workspace ?"), _("Are you sure ?"), GTK_MESSAGE_QUESTION, MainWindow);
  }
  else
  {
    close = TRUE;
  }

  if (close)
  {
    j = nprojects-1;
    for (i=j; i>-1; i--)
    {
#ifdef DEBUG
      g_debug ("CLOSING:: %d", i);
#endif
      on_close_activate (NULL, GINT_TO_POINTER(i));
    }
  }
}

gboolean save = TRUE;

atomes_error * project_error = NULL;
atomes_error_signal errors_messages[] = {{ ERROR_RW,      i18n("I/O error")},
                                         { ERROR_PROJECT, i18n("project information")},
                                         { ERROR_CURVE,   i18n("curve(s) information")},
                                         { ERROR_IMAGE,   i18n("OpenGL information")},
                                         { ERROR_ATOM_A,  i18n("atom's data - A")},
                                         { ERROR_ATOM_B,  i18n("atom's data - B")},
                                         { ERROR_UPDATE,  i18n("updating project")},
                                         { ERROR_NO_WAY,  i18n("this should not happen")},
                                         { ERROR_COORD,   i18n("coordination data")},
                                         { ERROR_RINGS,   i18n("ring(s) data")},
                                         { ERROR_CHAINS,  i18n("chain(s) data")},
                                         { ERROR_MOL,     i18n("molecule(s) data")},
                                         { ERROR_ANA,     i18n("analysis data")},
                                         { ERROR_QM,      i18n("ab-initio data")},
                                         { ERROR_FIELD,   i18n("classical data")}};

/*!
  \fn int signal_error (gchar * file, gchar * func, int error_line, int error_id)

  \brief store atomes files I/O error information

  \param file the file where the error occured
  \param func the function where the error occured
  \param error_line the line at witch the error occured
  \param error_id the error ID
*/
int signal_error (const char * file, const char * func, int error_line, int error_id)
{
  project_error -> error_file = g_strdup_printf("%s", file);
  project_error -> error_func = g_strdup_printf("%s", func);
  project_error -> error_line = error_line;
  project_error -> error_signal = errors_messages[error_id-1];
  return error_line;
}

/*!
  \fn void update_error_trace (gchar * file, gchar * func, int trace_line)

  \brief update atomes files I/O error traceback

  \param file the file where the call was made
  \param func the function the call was made to
  \param error_line the line at witch the call was made
*/
void update_error_trace (const char * file, const char * func, int trace_line)
{
  gchar * intro = "<span font_desc=\"monospace 10\">\n";
  gchar * file_part;
  gchar * func_part;
  gchar * line_part;
  gchar * tab_part = NULL;
  gchar * tmp_trace;
  int i;
  if (project_error -> trace_id)
  {
    tmp_trace = g_strdup_printf ("%s\n%s", project_error -> error_trace, intro);
    g_free (project_error -> error_trace);
  }
  else
  {
    tmp_trace = g_strdup_printf ("%s", intro);
  }
  for (i=0; i<project_error -> trace_id+1; i++)
  {
    tab_part = (tab_part) ? g_strdup_printf ("%s\t\t", tab_part) : g_strdup_printf ("\t\t");
  }
  file_part = g_strdup_printf (_("File    : %s\n"), file);
  func_part = g_strdup_printf (_("Function: %s()\n"), func);
  line_part = g_strdup_printf (_("Line    : %d</span>"), trace_line);
  project_error -> error_trace = g_strdup_printf ("%s%s%s%s%s%s%s", tmp_trace, tab_part, file_part, tab_part, func_part, tab_part, line_part);
  g_free (tmp_trace);
  g_free (tab_part);
  g_free (file_part);
  g_free (func_part);
  g_free (line_part);
  project_error -> trace_id ++;
}

/*!
  \fn int open_save (FILE * fp, int act, int wid, int pid, int aid, gchar * pfile)

  \brief open or save project file

  \param fp the file pointer
  \param act 0 = read, 1 = write
  \param wid read or save workspace (1/0)
  \param pid the project id
  \param aid the active project id
  \param pfile the file name
*/
int open_save (FILE * fp, int act, int wid, int pid, int aid, gchar * pfile)
{
  int j;
  project_error = g_malloc0(sizeof*project_error);
  gchar * tmp_err;
  if (act == 0)
  {
    reading_input = TRUE;
    j = open_project (fp, wid);
    reading_input = FALSE;
    if (j != OK)
    {
      tmp_err = g_strdup_printf ("%s", get_project_by_id (pid) -> name);
      to_close_this_project (aid, active_project);
    }
    else
    {
      get_project_by_id (pid) -> projfile = g_strdup_printf ("%s", pfile);
      if (! atomes_render_image)
      {
        add_project_to_workspace ();
        prep_calc_actions ();
      }
      else
      {
        simple_image_render ();
      }
    }
  }
  else
  {
    j = save_project (fp, get_project_by_id(pid), wid);
    if (j == OK)
    {
      if (pfile != NULL) get_project_by_id(pid) -> projfile = g_strdup_printf ("%s", pfile);
    }
    else
    {
      tmp_err = g_strdup_printf ("%s", get_project_by_id(pid) -> name);
    }
  }
  if (j != OK)
  {
    gchar * err;
    if (pfile != NULL && ! wid)
    {
      err = g_strdup_printf (_("Impossible to %s project file: \n\n"
                               "\t\t%s\n\n"
                               "\tError %s %s\n"
                               "\tProject file version: <b>%1.1f</b>\n"),
                               (! act) ? _("open") : _("save"),
                               pfile,
                               (! act) ? _("reading") : _("saving"),
                               _(project_error -> error_signal.message),
                               project_file_version);

    }
    else if (wid)
    {
      err = g_strdup_printf (_("Impossible to %s workspace file.\n"
                               "Error with project: \n\n"
                               "\t\t%s\n\n"
                               "\tError %s %s\n"
                               "\tProject file version: <b>%1.1f</b>\n"),
                               (! act) ? _("open") : _("save"),
                               tmp_err,
                               (! act) ? _("reading") : _("saving"),
                               _(project_error -> error_signal.message),
                               project_file_version);
    }
    show_error_with_trace (err, project_error, act, 0, MainWindow);
    g_free (tmp_err);
    g_free (err);
  }
  g_free (project_error);
  project_error = NULL;
  return j;
}

/*!
  \fn void quit_gtk ()

  \brief Leave the application
*/
void quit_gtk ()
{
  if (atomes_from_libreoffice)
  {
    // Update image for LibreOffice document
    atomes_render_image = TRUE;
    simple_image_render ();
    atomes_render_image = FALSE;
    // Mandatory saving of the project file
    FILE * fp = fopen (projfile, dfi[1]);
    open_save (fp, 1, 1, 0, 0, NULL);
    fclose (fp);
    to_close_this_project (0, active_project);
  }
  else
  {
    int i;
    for (i=nprojects-1; i>=0; i--) to_close_this_project (i, get_project_by_id(i));
  }
  profree_ ();
  g_application_quit (G_APPLICATION(AtomesApp));
}

/*!
  \fn int open_save_workspace (FILE * fp, int act)

  \brief open or save the active workspace

  \param fp the file to read or write
  \param act the action (0 = read, 1 = write)
*/
int open_save_workspace (FILE * fp, int act)
{
  int i, j, k, l;
  gchar * ver;
  /*PangoFontDescription * font_desc;
  GtkTextBuffer * buffer;
  GtkTextIter bStart;
  GtkTextIter bEnd;*/

  // First 2 lines for compatibility issues
  if (act == 0)
  {
    if (fread (& i, sizeof(int), 1, fp) != 1) return 1;
    ver = g_malloc0(i*sizeof*ver);
    if (fread (ver, sizeof(char), i, fp) != i) return 1;
    // test on ver for version ?
    g_free (ver);
    if (fread (& i, sizeof(int), 1, fp) != 1) return 1;
  }
  else
  {
    i = 1;
    ver = g_strdup_printf ("%%\n%% Workspace file v-%1d.0\n%%\n", i);
    i = strlen (ver);
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return 1;
    if (fwrite (ver, sizeof(char), i, fp) != i) return 1;
    g_free (ver);
    i = 0;
    for (j=0; j<nprojects; j++) if (get_project_by_id(j) -> natomes) i++;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return 1;
  }

  if (i > 0)
  {
    for (j=0; j<i; j++)
    {
      k = activep;
      if (act == 0)
      {
        init_project (FALSE);
        l = open_save (fp, act, 1, j, k, NULL);
        if (l != 0) return l;
      }
      else if (get_project_by_id(j) -> natomes)
      {
        l = open_save (fp, act, 1, j, k, NULL);
        if (l != 0) return l;
      }
    }
    return 0;
  }
  else
  {
    return -1;
  }
}

/*!
  \fn void open_this_proj (gpointer data, gpointer user_data)

  \brief Open many projects, one at a time

  \param data
  \param user_data
*/
void open_this_proj (gpointer data, gpointer user_data)
{
  FILE * fp = fopen (data, dfi[0]);
  int pactive = activep;
  init_project (FALSE);
  reading_project = TRUE;
  open_save (fp, 0, 0, activew, pactive, data);
  reading_project = FALSE;
  fclose (fp);
  activew = activep;
}

tint osp;
gboolean run_os;

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_on_open_save_active (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief open or save an atomes file - running the dialog

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_open_save_active (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GListModel * projlist;
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_on_open_save_active (GtkDialog * info, gint response_id, gpointer data)

  \brief open or save an atomes file - running the dialog

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_open_save_active (GtkDialog * info, gint response_id, gpointer data)
{
  GSList * projlist = NULL;
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  FILE * fp = NULL;
  gchar * err;
  gboolean io = FALSE;
  const gchar * mess[2]={i18n("reading"),i18n("saving")};
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    if (osp.a == 0)
    {
      projlist = file_chooser_get_file_names (chooser);
    }
    else
    {
      projfile = file_chooser_get_file_name (chooser);
    }
    if (osp.a > 1)
    {
      workspacefile = g_strdup_printf ("%s", projfile);
    }
    io = TRUE;
  }
  if (run_os)
  {
#ifdef GTK4
    destroy_this_native_dialog (info);
#else
    destroy_this_dialog (info);
#endif
  }
  if (io)
  {
    if (osp.a > 0)
    {
      fp = fopen (projfile, dfi[osp.b]);
    }
    if (osp.a == 0)
    {
#ifdef GTK3
      g_slist_foreach (projlist, open_this_proj, NULL);
      g_slist_free (projlist);
#else
      int i;
      for (i=0; i<g_list_model_get_n_items (projlist); i++)
      {
        GObject * obj = g_list_model_get_item (projlist, i);
        open_this_proj (g_file_get_parse_name((GFile *)obj), NULL);
      }
      g_object_unref (projlist);
#endif
    }
    else if (osp.a == 1)
    {
      open_save (fp, osp.a, 0, activew, osp.c, projfile);
    }
    else
    {
      int k = open_save_workspace (fp, osp.b);
      if (k != 0)
      {
        err = g_strdup_printf (_("Error %s workspace file\n%s\n"), _(mess[osp.b]), projfile);
        show_error (err, 0, MainWindow);
        g_free (err);
      }
    }
    if (osp.a > 0)
    {
      fclose (fp);
      g_free (projfile);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void on_open_save_activate (GtkWidget * widg, gpointer data)

  \brief open or save an atomes file - prepare the dialog

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_open_save_activate (GtkWidget * widg, gpointer data)
{
  int i, j, k;
  gint action;
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  GtkFileChooser * chooser;
  GtkFileFilter * filter1, * filter2;
  const gchar * str[4]={i18n("Open Project File(s)"), i18n("Save Project File"), i18n("Open Workspace"), i18n("Save Workspace")};
  const gchar * res[2]={i18n("Open"), i18n("Save")};
  const gchar * file_name[2]={i18n("Project file (*.apf)"), i18n("Workspace file (*.awf)")};
  const gchar * file_ext[2]={"*.apf", "*.awf"};
  GtkFileChooserAction act[2]={GTK_FILE_CHOOSER_ACTION_OPEN, GTK_FILE_CHOOSER_ACTION_SAVE};
  project * this_proj = get_project_by_id (activew);
  int pactive = activep;
  i = GPOINTER_TO_INT (data);
  run_os = FALSE;
  if (i == 1 || i == 3)
  {
    j = 1;
  }
  else
  {
    j = 0;
  }
  action = 0;
  if (i == 2 && ! newspace)
  {
    show_info (_("A workspace is already open !"), 0, MainWindow);
  }
  else if (i == 3 && newspace)
  {
    show_warning (_("Empty workspace ... nothing to be saved\n"), MainWindow);
  }
  else if (i == 3)
  {
    for (k=0; k<nprojects; k++) if (get_project_by_id(k) -> natomes) action = 1;
    if (! action)
    {
      show_warning (_("Workspace contains only empty projects ... nothing to be saved\n"), MainWindow);
    }
  }
  else if (i == 1 && nprojects == 0)
  {
    show_warning (_("No project open ... nothing to be saved\n"), MainWindow);
  }
  else if (i == 1 && ! this_proj -> natomes)
  {
    show_warning (_("Empty project ... nothing to be saved\n"), MainWindow);
  }
  else
  {
    action = 1;
  }

  if (action)
  {
    if (nprojects == 0)
    {
      run_os = TRUE;
    }
    else
    {
      if (i == 1)
      {
        if (g_strcmp0(this_proj -> projfile, "(null)") == 0) this_proj -> projfile = NULL;
        if (save && this_proj -> projfile != NULL)
        {
          run_os = FALSE;
          projfile = g_strdup_printf ("%s", this_proj -> projfile);
        }
        else
        {
          run_os = TRUE;
          projfile = NULL;
        }
      }
      else if (i == 3)
      {
        if (g_strcmp0(workspacefile, "(null)") == 0) workspacefile = NULL;
        if (save && workspacefile != NULL)
        {
          run_os = FALSE;
          projfile = g_strdup_printf ("%s", workspacefile);
        }
        else
        {
          run_os = TRUE;
        }
      }
      else
      {
        run_os = TRUE;
      }
    }

    gchar * tmp_str;
    if (i == 0)
    {
      tmp_str = g_strdup_printf (_("%s - New project"), _(str[i]));
    }
    else if (i == 1)
    {
      tmp_str = g_strdup_printf ("%s - %s", _(str[i]), prepare_for_title(this_proj -> name));
    }
    else
    {
      tmp_str = g_strdup_printf ("%s", _(str[i]));
    }
    info = create_file_chooser (tmp_str,
                                GTK_WINDOW(MainWindow),
                                act[j],
                                _(res[j]));
    chooser = GTK_FILE_CHOOSER (info);
    g_free (tmp_str);
#ifdef GTK3
    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
    if (i == 1 || i == 3) gtk_file_chooser_set_create_folders (chooser, TRUE);
    if (nprojects == 0 || i == 0)
    {
      file_chooser_set_current_folder (chooser);
      if (i == 0)
      {
        gtk_file_chooser_set_select_multiple (chooser, TRUE);
      }
    }
    else
    {
      if (i == 1)
      {
        if (projfile != NULL)
        {
          file_chooser_set_current_folder (chooser);
          gtk_file_chooser_set_current_name (chooser, projfile);
        }
        else
        {
          if (! file_chooser_set_file_name (chooser, g_strdup_printf ("%s.apf", prepare_for_title(this_proj -> name)))) goto end;
        }
      }
      else if (i == 3)
      {
        if (workspacefile != NULL)
        {
          file_chooser_set_current_folder (chooser);
          gtk_file_chooser_set_current_name (chooser, g_strdup_printf ("%s", workspacefile));
        }
        else
        {
          if (! file_chooser_set_file_name (chooser, "New-Workspace.awf")) goto end;
        }
      }
    }
    filter1 = gtk_file_filter_new();
    gtk_file_filter_set_name (GTK_FILE_FILTER(filter1), _(file_name[i/2]));
    gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter1), file_ext[i/2]);
    gtk_file_chooser_add_filter (chooser, filter1);
    filter2 = gtk_file_filter_new();
    gtk_file_filter_set_name (GTK_FILE_FILTER(filter2), _("All files (*)"));
    gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter2), "*");
    gtk_file_chooser_add_filter (chooser, filter2);
    osp.a = i;
    osp.b = j;
    osp.c = pactive;
    if (run_os)
    {
#ifdef GTK4
     run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_on_open_save_active), this_proj);
#else
     run_this_gtk_dialog (info, G_CALLBACK(run_on_open_save_active), this_proj);
#endif
    }
    else
    {
#ifdef GTK4
      run_on_open_save_active ((GtkNativeDialog *)info, GTK_RESPONSE_ACCEPT, this_proj);
#else
      run_on_open_save_active ((GtkDialog *)info, GTK_RESPONSE_ACCEPT, this_proj);
#endif
    }
  }
  activew = activep;
  update_insert_combos ();
  end:;
}

/*!
  \fn G_MODULE_EXPORT void on_save_as_activate (GtkWidget * widg, gpointer data)

  \brief open or save, choosing a file name

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_save_as_activate (GtkWidget * widg, gpointer data)
{
  save = FALSE;
  on_open_save_activate (widg, data);
  save = TRUE;
}

/*!
  \fn void run_project ()

  \brief send project data to Fortran90
*/
void run_project ()
{
  if (! active_project -> run)
  {
    if (! active_project -> analysis)
    {
      init_atomes_analysis (active_project, TRUE);
    }
    int i, j;
    j = (active_cell -> npt) ? active_project -> steps : 1;
    for (i=0; i<j; i++)
    {
      lattice_ (& j, & i,
                active_cell -> box[i].vect,
                active_cell -> box[i].param[0],
                active_cell -> box[i].param[1],
                & active_cell -> ltype,
                & active_cell -> frac,
                & active_cell -> pbc);
    }
    to_read_pos ();
    prep_pos_ (& active_cell -> pbc, & active_cell -> frac);
    active_project -> dmtx = FALSE;
    active_project -> run = 1;
  }
  if (active_cell -> frac) active_cell -> frac = 0;
}

/*!
  \fn void apply_project (gboolean showtools)

  \brief get project ready for calculation and initialize the OpenGL window

  \param showtools
*/
void apply_project (gboolean showtools)
{
  if (active_project -> natomes)
  {
    run_project ();
    initcutoffs (active_chem, active_project -> nspec);
  }
  prep_model (active_project -> id);
  if (showtools && ! atomes_render_image) show_the_widgets (curvetoolbox);
}

/*!
  \fn void open_this_isaacs_xml_file (gchar * profile, int ptoc, gboolean visible)

  \brief open an ISAACS XML file

  \param profile the XML file name
  \param ptoc the previous active project
  \param visible read from the command line (FALSE), from the gui (TRUE)
*/
void open_this_isaacs_xml_file (gchar * profile, int ptoc, gboolean visible)
{
  if (! open_xml (profile))
  {
    active_project -> name = substitute_string (g_path_get_basename (profile), ".ipf", NULL);
    on_edit_activate (NULL, GINT_TO_POINTER(3));
    on_edit_activate (NULL, GINT_TO_POINTER(5));
    active_project_changed (activep);
    frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
    mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
    apply_project (TRUE);
    active_project_changed (activep);
    add_project_to_workspace ();
    if (visible) show_info (_("ISAACS project file (XML) successfully opened"), 0, MainWindow);
  }
  else
  {
    to_close_this_project (ptoc, active_project);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_on_isaacs_port (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief open or write ISAACS XML file - running the dialog

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_isaacs_port (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_on_isaacs_port (GtkDialog * info, gint response_id, gpointer data)

  \brief open or write ISAACS XML file - running the dialog

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_isaacs_port (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    if (osp.a == 0 || nprojects == 0) init_project (TRUE);
    projfile = file_chooser_get_file_name (chooser);
#ifdef GTK4
    destroy_this_native_dialog (info);
#else
    destroy_this_dialog (info);
#endif
    if (osp.a == 0)
    {
      open_this_isaacs_xml_file (projfile, osp.b, TRUE);
    }
    else if (osp.a == 1)
    {
      active_project_changed (activew);
      if (write_xml (projfile) == 0)
      {
        show_error (_("Impossible to write the IPF file\n"), 0, MainWindow);
      }
      active_project_changed (osp.b);
    }
    g_free (projfile);
  }
  else
  {
#ifdef GTK4
    destroy_this_native_dialog (info);
#else
    destroy_this_dialog (info);
#endif
  }
}

/*!
  \fn G_MODULE_EXPORT void on_isaacs_port (GtkWidget * widg, gpointer data)

  \brief open or write ISAACS XML file - prepare the dialog

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer (int *) open = 0, save = 1
*/
G_MODULE_EXPORT void on_isaacs_port (GtkWidget * widg, gpointer data)
{
  int i, j;
  gboolean action;
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  GtkFileChooser * chooser;
  GtkFileFilter * filter[2];
  const gchar * file_ext[2]={"*.ipf", "*"};
  const gchar * file_type[2]={i18n("IPF file (*.ipf)"), i18n("All files (*)")};
  const gchar * str[2]={i18n("Import ISAACS Project File"), i18n("Export ISAACS Project File")};
  const gchar * res[2]={i18n("Open"), i18n("Save")};
  GtkFileChooserAction act[2]={GTK_FILE_CHOOSER_ACTION_OPEN, GTK_FILE_CHOOSER_ACTION_SAVE};
  int pactive = activep;
  i = GPOINTER_TO_INT (data);

  action = (i && ! nprojects) ? ask_yes_no (_("Save an empty project ?"), _("Do you want to save an empty project ?"), GTK_MESSAGE_QUESTION, MainWindow) : TRUE;
  if (action)
  {
    info = create_file_chooser (_(str[i]),
                                GTK_WINDOW(MainWindow),
                                act[i],
                                _(res[i]));
    chooser = GTK_FILE_CHOOSER (info);
#ifdef GTK3
    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
    if (i) gtk_file_chooser_set_create_folders (chooser, TRUE);
    for (j=0; j<2; j++)
    {
      filter[j] = gtk_file_filter_new();
      gtk_file_filter_set_name (GTK_FILE_FILTER(filter[j]), _(file_type[j]));
      gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter[j]), file_ext[j]);
      gtk_file_chooser_add_filter (chooser, filter[j]);
    }
    file_chooser_set_current_folder (chooser);
    osp.a = i;
    osp.b = pactive;
#ifdef GTK4
    run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_on_isaacs_port), NULL);
#else
    run_this_gtk_dialog (info, G_CALLBACK(run_on_isaacs_port), NULL);
#endif
  }
  activew = activep;
  update_insert_combos ();
}

/*!
  \fn void to_read_pos ()

  \brief send atomic coordinates to Fortran90
*/
void to_read_pos ()
{
  int i, j, k;
  double * x, * y, * z;
  double lat[3];

  x = allocdouble (active_project -> steps*active_project -> natomes);
  y = allocdouble (active_project -> steps*active_project -> natomes);
  z = allocdouble (active_project -> steps*active_project -> natomes);
  k = 0;
  lat[0] = lat[1] = lat[2] = 0.0;
  if (active_cell -> crystal)
  {
    for (i=0; i<3; i++)
    {
      for (j=0; j<3; j++) lat[i] -= active_box -> vect[j][i]/2.0;
    }
  }
  for (i=0; i<active_project -> steps; i++)
  {
    for (j=0; j<active_project -> natomes; j++)
    {
      x[k] = active_project -> atoms[i][j].x + lat[0];
      y[k] = active_project -> atoms[i][j].y + lat[1];
      z[k] = active_project -> atoms[i][j].z + lat[2];
      k ++;
    }
  }
  read_pos_ (x, y, z);
  g_free (x);
  x = NULL;
  g_free (y);
  y = NULL;
  g_free (z);
  z = NULL;
}

GtkWidget * read_box;
GtkWidget * all_sp_box = NULL;
GtkWidget * sa_lab[2];
GtkWidget * sa_entry[2];
GtkWidget * read_this;
int read_spec;

/*!
  \fn void check_read_sa ()

  \brief reading CPMD/VASP trajectory, testing parameters to active the read capabilty
*/
void check_read_sa ()
{
  int i, j, k;
  i = j = 0;
  for (k=0; k<this_reader -> nspec; k++)
  {
    i += this_reader -> nsps[k];
    j += (this_reader -> label[k]) ? 1: 0;
  }
  if (i == this_reader -> natomes && j == this_reader -> nspec)
  {
    widget_set_sensitive (read_this, 1);
  }
  else
  {
    widget_set_sensitive (read_this, 0);
  }
}

/*!
  \fn void update_sa_info (int sid)

  \brief reading CPMD/VASP trajectory, update chemical species info

  \param sid the species id
*/
void update_sa_info (int sid)
{
  gchar * str = g_strdup_printf (_("Label of atomic spec. N° %d:"), sid+1);
  gtk_label_set_text (GTK_LABEL(sa_lab[0]), str);
  g_free (str);
  if (this_reader -> label[sid])
  {
    update_entry_text (GTK_ENTRY(sa_entry[0]), this_reader -> label[sid]);
    str = g_strdup_printf (_("Number of %s atom(s):"), this_reader -> label[sid]);
  }
  else
  {
    update_entry_text (GTK_ENTRY(sa_entry[0]), "");
    str = g_strdup_printf (_("Number of atom(s) for spec. N° %d:"), sid+1);
  }
  gtk_label_set_text (GTK_LABEL(sa_lab[1]), str);
  g_free (str);
  if (this_reader -> nsps[sid])
  {
    update_entry_int (GTK_ENTRY(sa_entry[1]), this_reader -> nsps[sid]);
  }
  else
  {
    update_entry_text (GTK_ENTRY(sa_entry[1]), "");
  }
  read_spec = sid;
}

/*!
  \fn G_MODULE_EXPORT void update_sa (GtkEntry * res, gpointer data)

  \brief reading CPMD/VASP trajectory, set the number of chemical species

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_sa (GtkEntry * res, gpointer data)
{
  int i, v;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  if (i == 0)
  {
    if (this_reader -> label[read_spec]) g_free (this_reader -> label[read_spec]);
    this_reader -> label[read_spec] = NULL;
    this_reader -> label[read_spec] = g_strdup_printf ("%s", m);
    update_sa_info (read_spec);
  }
  else
  {
    v= (int)string_to_double ((gpointer)m);
    if (v > 0)
    {
      this_reader -> nsps[read_spec] = v;
    }
    update_entry_int (res, this_reader -> nsps[read_spec]);
  }
  check_read_sa ();
}

/*!
  \fn G_MODULE_EXPORT void changed_spec_combo (GtkComboBox * box, gpointer data)

  \brief reading CPMD/VASP trajectory, change the active species

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_spec_combo (GtkComboBox * box, gpointer data)
{
  update_sa_info (combo_get_active ((GtkWidget *)box));
}

/*!
  \fn void prepare_sp_box ()

  \brief eading CPMD/VASP trajectory, prepare the species combo box
*/
void prepare_sp_box ()
{
  int i;
  if (all_sp_box)
  {
    for (i=0; i<2; i++)
    {
      sa_lab[i] = destroy_this_widget(sa_lab[i]);
      sa_entry[i] = destroy_this_widget(sa_entry[i]);
    }
    all_sp_box = destroy_this_widget(all_sp_box);
  }
  widget_set_sensitive (read_this, 0);
  all_sp_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, read_box, all_sp_box, FALSE, FALSE, 20);
  GtkWidget * hbox;
  add_box_child_start (GTK_ORIENTATION_VERTICAL, all_sp_box, markup_label(_("Chemical species info:"), 200, -1, 0.5, 0.5), FALSE, FALSE, 0);
  hbox = create_hbox(0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, all_sp_box, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(_("Species: "), 100, -1, 0.0, 0.5), FALSE, FALSE, 5);
  GtkWidget * combo;
  combo = create_combo ();
  gchar * str;

  for (i=0; i<this_reader -> nspec; i++)
  {
    str = g_strdup_printf ("N°%d", i+1);
    gtk_combo_box_text_append_text ((GtkComboBoxText *)combo, str);
    g_free (str);
  }
  combo_set_active (combo, 0);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(changed_spec_combo), NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 5);
  for (i=0; i<2; i++)
  {
    hbox = create_hbox(0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, all_sp_box, hbox, FALSE, FALSE, 5);
    sa_lab[i] = markup_label("", 250, -1, 0.0, 0.5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sa_lab[i], FALSE, FALSE, 5);
    sa_entry[i] = create_entry (G_CALLBACK(update_sa), 100, 15, FALSE, GINT_TO_POINTER(i));
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sa_entry[i], FALSE, FALSE, 0);
  }
  show_the_widgets (read_box);
  update_sa_info (0);
}

/*!
  \fn G_MODULE_EXPORT void update_at_sp (GtkEntry * res, gpointer data)

  \brief reading CPMD/VASP trajectory, changing number of atomes or species

  \param res the GtkEntry sending the signal
  \param data the associated data pointer (int *) 0 = atomes, 1 = species
*/
G_MODULE_EXPORT void update_at_sp (GtkEntry * res, gpointer data)
{
  int i, v;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  gboolean up = FALSE;
  v = (int)string_to_double ((gpointer)m);
  if (i == 0)
  {
    this_reader -> natomes = (v > 0) ? v : 0;
    update_entry_int (res, this_reader -> natomes);
  }
  else
  {
    if (v != this_reader -> nspec)
    {
      this_reader -> nspec = (v > 0) ? v : 0;
      if (this_reader -> nspec)
      {
        if (this_reader -> nsps) g_free (this_reader -> nsps);
        this_reader -> nsps = allocint (v);
        if (this_reader -> z) g_free (this_reader -> z);
        this_reader -> z = allocdouble (v);
        if (this_reader -> label) g_free (this_reader -> label);
        this_reader -> label = g_malloc0(v*sizeof*this_reader -> label);
      }
      up = TRUE;
    }
    update_entry_int (res, this_reader -> nspec);
  }
  if (up) prepare_sp_box();
}

int reading_vas_trj;

/*!
  \fn int prep_chem_data ()

  \brief prepare chemical data to read CPMD/VASP file
*/
int prep_chem_data ()
{
  int i;
  double z;
  for (i=0; i<this_reader -> nspec; i++)
  {
    z = get_z_from_periodic_table (this_reader -> label[i]);
    if (! z) return 0;
    this_reader -> z[i] = z;
  }
  return 1;
}

/*!
  \fn G_MODULE_EXPORT void run_to_read_trj_or_vas (GtkDialog * dialog, gint response_id, gpointer data)

  \brief reading CPMD/VASP trajectory: run the dialog

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_to_read_trj_or_vas (GtkDialog * dialog, gint response_id, gpointer data)
{
  int id = GPOINTER_TO_INT(data);
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (prep_chem_data())
      {
        reading_vas_trj = open_coord_file (active_project -> coordfile, id);
      }
      else
      {
        reading_vas_trj = 3;
      }
      break;
    default:
        reading_vas_trj = 3;
      break;
  }
  if (all_sp_box)
  {
    int i;
    for (i=0; i<2; i++)
    {
      sa_lab[i] = destroy_this_widget(sa_lab[i]);
      sa_entry[i] = destroy_this_widget(sa_entry[i]);
    }
    all_sp_box = destroy_this_widget(all_sp_box);
  }
  destroy_this_dialog (dialog);
}

/*!
  \fn int to_read_trj_or_vas (int ff)

  \brief reading CPMD/VASP trajectory - prepare the dialog

  \param ff file type
*/
int to_read_trj_or_vas (int ff)
{
  int i;
  gchar * rlabel[2]={i18n("Total number of atom(s):"), i18n("Number of chemical species:")};
  GtkWidget * dialog = dialogmodal (_("Reading CPMD / VASP trajectory"), GTK_WINDOW(MainWindow));
  read_this = gtk_dialog_add_button (GTK_DIALOG (dialog), _("Apply"), GTK_RESPONSE_APPLY);
  GtkWidget * vbox = dialog_get_content_area (dialog);
  widget_set_sensitive (read_this, 0);
  GtkWidget * rentry;
  GtkWidget * hbox;
  for (i=0; i<2; i++)
  {
    hbox = create_hbox(0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(_(rlabel[i]), 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
    rentry = create_entry (G_CALLBACK(update_at_sp), 100, 15, FALSE, GINT_TO_POINTER(i));
    update_entry_text (GTK_ENTRY(rentry), "");
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, rentry, FALSE, FALSE, 5);
  }
  read_box = create_hbox(0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, read_box, FALSE, FALSE, 5);
  run_this_gtk_dialog (dialog, G_CALLBACK(run_to_read_trj_or_vas), GINT_TO_POINTER(ff));
  return reading_vas_trj;
}

/*!
  \fn void cell_data_from_pdb_ (float * a, float * b, float * c, float * alp, float * bet, float * gam)

  \brief update cell parameters from the data in the PDB file

  \param a a
  \param b b
  \param c c
  \param alp alpha
  \param bet beta
  \param gam gamma
*/
void cell_data_from_pdb_ (float * a, float * b, float * c, float * alp, float * bet, float * gam)
{
  active_box -> param[0][0] = * a;
  active_box -> param[0][1] = * b;
  active_box -> param[0][2] = * c;
  active_box -> param[1][0] = * alp;
  active_box -> param[1][1] = * bet;
  active_box -> param[1][2] = * gam;
  // In a PDB file it is required to turn off PBC
  // The box usually barely encompass the molecule
  active_cell -> pbc = 0;
  active_cell -> ltype = 1;
}

int npt_selection;
gchar * npt_file;

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_read_npt_data (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief read NPT data associated with atomic coordinates: run the dialog GTK4 callback

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_read_npt_data (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_read_npt_data (GtkDialog * info, gint response_id, gpointer data)

  \brief read NPT data associated with atomic coordinates: run the dialog GTK3 callback

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_read_npt_data (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    npt_file = file_chooser_get_file_name (chooser);
    npt_selection = iask (_("Please select the file format of the NPT cell data"), _("Select format:"), 6, MainWindow);
  }
  else
  {
    npt_selection = -1;
  }
#ifdef GTK4
  destroy_this_native_dialog (info);
#else
  destroy_this_dialog (info);
#endif
}

/*!
  \fn int read_npt_data ()

  \brief read NPT data associated with atomic coordinates: setup the dialog
*/
int read_npt_data ()
{
  GtkFileFilter * filter[2];
#ifdef GTK4
   GtkFileChooserNative * info;
#else
   GtkWidget * info;
#endif
  info = create_file_chooser (_("Read cell data for NPT molecular dynamics"),
                              GTK_WINDOW(MainWindow),
                              GTK_FILE_CHOOSER_ACTION_OPEN,
                              _("Open"));
  GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
  filter[0] = gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter[0]), _("DAT files (*.dat)"));
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter[0]), "*.dat");
  gtk_file_chooser_add_filter (chooser, filter[0]);
  filter[1] = gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter[1]), _("All files (*)"));
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter[1]), "*");
  gtk_file_chooser_add_filter (chooser, filter[1]);
  npt_file = NULL;
#ifdef GTK4
  run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_read_npt_data), NULL);
#else
  run_this_gtk_dialog (info, G_CALLBACK(run_read_npt_data), NULL);
#endif
  return (npt_selection < 0) ? 0 : open_cell_file (npt_selection, npt_file);
}

/*!
  \fn int open_coordinate_file (int id)

  \brief try to open coordinate file, type is based of id

  \param id file type to open
*/
int open_coordinate_file (int id)
{
  struct timespec sta_time;
  struct timespec sto_time;
  int result;
  int length = strlen(active_project -> coordfile);
  clock_gettime (CLOCK_MONOTONIC, & sta_time);
  this_reader = g_malloc0(sizeof*this_reader);
  // Set default message type to warning
  this_reader -> mid = 1;
  switch (id)
  {
    case 0:
      // XYZ file
      result = open_coord_file (active_project -> coordfile, id);
      // result = read_xyz_ (active_project -> coordfile, & length, & npt);
      break;
    case 1:
      // XYZ file NPT
      result = open_coord_file (active_project -> coordfile, id);
      // result = read_xyz_ (active_project -> coordfile, & length, & npt);
      if (! result) result = read_npt_data ();
      break;
    case 2:
      // Chem3D file
      result = open_coord_file (active_project -> coordfile, id);
      // result = read_c3d_ (active_project -> coordfile, & length);
      break;
    case 3:
      // CPMD TRJ file
      result = to_read_trj_or_vas (id);
      break;
    case 4:
      // CPMD TRJ file NPT
      result = to_read_trj_or_vas (id);
      if (! result) result = read_npt_data ();
      break;
    case 5:
      // VASP XDATCAR file
      result = to_read_trj_or_vas (id);
      break;
    case 6:
      // VASP XDATCAR file NPT
      result = to_read_trj_or_vas (id);
      if (! result) result = read_npt_data ();
      break;
    case 7:
      // PDB file
      // result = open_coord_file (active_project -> coordfile, id);
      result = read_pdb_ (active_project -> coordfile, & length);
      break;
    case 8:
      // PDB file
      // result = open_coord_file (active_project -> coordfile, id);
      result = read_pdb_ (active_project -> coordfile, & length);
      break;
    case 9:
      // CIF file building the crystal
      result = open_coord_file (active_project -> coordfile, 9);
      break;
    case 10:
      // CIF file using symmetry positions
      result = open_coord_file (active_project -> coordfile, 10);
      break;
    case 11:
      // CIF file using symmetry positions
      result = open_coord_file (active_project -> coordfile, 11);
      break;
    case 12:
      // DL-POLY file
      result = open_coord_file (active_project -> coordfile, 12);
      break;
    default:
      result = 2;
      break;
  }
  clock_gettime (CLOCK_MONOTONIC, & sto_time);
  g_print ("Time to read atomic coordinates: %s\n", calculation_time(FALSE, get_calc_time (sta_time, sto_time)));
  if (this_reader)
  {
    if (this_reader -> msg && (! silent_input || cif_use_symmetry_positions))
    {
      gchar * info = g_strdup_printf ("%s", this_reader -> info[0]);
      int i;
      for (i=1; i<this_reader -> msg; i++)
      {
        info = g_strdup_printf ("%s\n%s", info, this_reader -> info[i]);
      }
      switch (this_reader -> mid)
      {
        case 0:
          show_error (info, 0, MainWindow);
          break;
        case 1:
          show_warning (info, MainWindow);
          break;
      }
      g_free (info);
    }
    if (this_reader)
    {
      if (this_reader -> info) g_free (this_reader -> info);
      if (this_reader -> z) g_free (this_reader -> z);
      if (this_reader -> nsps) g_free (this_reader -> nsps);
      if (this_reader -> dummy) g_free (this_reader -> dummy);
      if (this_reader -> label) g_free (this_reader -> label);
      if (this_reader -> object_list) g_free (this_reader -> object_list);
      if (this_reader -> u_atom_list) g_free (this_reader -> u_atom_list);
      if (this_reader -> coord) g_free (this_reader -> coord);
      if (this_reader -> lot) g_free (this_reader -> lot);
      if (this_reader -> sym_pos) g_free (this_reader -> sym_pos);
      if (this_reader -> wyckoff) g_free (this_reader -> wyckoff);
      if (this_reader -> occupancy) g_free (this_reader -> occupancy);
      if (this_reader -> disorder) g_free (this_reader -> disorder);
      if (this_reader -> multi) g_free (this_reader -> multi);
      if (this_reader -> lattice.sp_group) g_free (this_reader -> lattice.sp_group);
      if (this_reader -> lattice.box) g_free (this_reader -> lattice.box);
      g_free (this_reader);
      this_reader = NULL;
    }
  }
  switch (result)
  {
    case 1:
      show_error (_("Error loading atomic coordinates:\nfile does not exist"), 0, MainWindow);
      break;
    case 2:
      show_error (_("Error loading coordinates file: format not supported"), 0, MainWindow);
      break;
    case 3:
      show_error (_("Error at input: impossible to process input file data"), 0, MainWindow);
      break;
    default:
      if (id > 6 && id < 9)
      {
        clock_gettime (CLOCK_MONOTONIC, & sta_time);
        if (! prep_data_ ())
        {
          show_error (_("Error while parsing the chemical information\n"
                        "please check carefully the coordinates file"), 0, MainWindow);
          result = 4;
        }
        clock_gettime (CLOCK_MONOTONIC, & sto_time);
        g_print ("Time to prepare data: %s\n", calculation_time(FALSE, get_calc_time (sta_time, sto_time)));
      }
      break;
  }
  return result;
}

GtkFileFilter * filter[NCFORMATS+1];
int pactive;

/*!
  \fn void open_this_coordinate_file (int format, gchar * proj_name)

  \brief open coordinate file format, if successful add to workspace

  \param format the format of the file that contains the atomic coordinates
  \param proj_name the project name to use, if any
*/
void open_this_coordinate_file (int format, gchar * proj_name)
{
  active_project -> newproj = FALSE;
  clock_gettime (CLOCK_MONOTONIC, & start_time);
  if (open_coordinate_file (format) == 0)
  {
    clock_gettime (CLOCK_MONOTONIC, & stop_time);
    g_print ("Time to open coordinate file: %s\n", calculation_time(FALSE, get_calc_time (start_time, stop_time)));
    active_project -> tfile = format;
    if (proj_name)
    {
      active_project -> name = g_strdup_printf ("%s", proj_name);
    }
    else
    {
      gchar * str = g_path_get_basename (active_project -> coordfile);
      active_project -> name = g_strdup_printf ("%s", substitute_string (str, g_strdup_printf (".%s", coord_files_ext[format]), NULL));
      g_free (str);
    }
    on_edit_activate (NULL, GINT_TO_POINTER(0));
    if (format != 1 && format != 4 && format != 6 && format != 9 && format != 10 && format != 11 && format != 12) on_edit_activate (NULL, GINT_TO_POINTER(4));
    initcutoffs (active_chem, active_project -> nspec);
    on_edit_activate (NULL, GINT_TO_POINTER(2));
    active_project_changed (activep);
    frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
    mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
    chemistry_ ();
    apply_project (TRUE);
    active_project_changed (activep);
    if (atomes_render_image)
    {
      simple_image_render();
    }
    else
    {
      if ((format == 9 || format == 10 || format == 11) && active_cell -> has_a_box)
      {
#ifdef GTK3
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)active_glwin -> ogl_rep[0], TRUE);
        set_rep (active_glwin -> ogl_rep[0], & active_glwin -> colorp[0][0]);
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)active_glwin -> ogl_clones[0], TRUE);
        widget_set_sensitive (active_glwin -> ogl_clones[0], active_glwin -> allbonds[1]);
        show_hide_clones (active_glwin -> ogl_clones[0], active_glwin);
#endif
        shift_it (vec3 (0.0, 0.0, 0.0), 1, activep);
        active_glwin -> wrapped = TRUE;
      }
      add_project_to_workspace ();
      if ((format == 9 || format == 10) && cif_use_symmetry_positions)
      {
        gchar * file_name = g_strdup_printf ("%s", active_project -> coordfile);
        gchar * proj_name = g_strdup_printf (_("%s - symmetry position(s)"), active_project -> name);
        init_project (TRUE);
        active_project -> coordfile = g_strdup_printf ("%s", file_name);
        g_free (file_name);
        open_this_coordinate_file (11, proj_name);
        g_free (proj_name);
      }
    }
  }
  else
  {
    to_close_this_project (pactive, active_project);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_on_coord_port (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief export or import atomic coordinates: run dialog

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_coord_port (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_on_coord_port (GtkDialog * info, gint response_id, gpointer data)

  \brief export or import atomic coordinates: run dialog

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_coord_port (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  int i, j, k, l, m;
  GtkFileFilter * tmp;
  int format;
  int car_to_au;
  i = GPOINTER_TO_INT(data);
  gchar * tmp_str;
  switch (response_id)
  {
    case GTK_RESPONSE_ACCEPT:
      tmp = gtk_file_chooser_get_filter (chooser);
      active_project -> coordfile = file_chooser_get_file_name (chooser);
#ifdef GTK4
      destroy_this_native_dialog (info);
#else
      destroy_this_dialog (info);
#endif
      j = 0;
      while (tmp != filter[j]) j++;
      if (i == 0)
      {
#ifdef OSX
        j = NCFORMATS;
#endif
        if (j == NCFORMATS)
        {
          j = iask (_("Please select the file format of the atomic coordinates"), _("Select format:"), 2, MainWindow);
        }
        open_this_coordinate_file (j, NULL);
      }
      else
      {
        if (j < 2)
        {
          format = iask (_("Please select the format of the atomic coordinates"), _("Select format:"), 1, MainWindow);
        }
        else
        {
          format = 0;
        }
        active_cell -> frac = (format < 2) ? 0 : format - 1;
        car_to_au = (format == 1) ? 1 : 0;
        if (j < 2)
        {
          m = (active_cell -> npt) ? active_project -> steps : 1;
          for (l=0; l<m; l++)
          {
            lattice_ (& m, & l,
                      active_cell -> box[l].vect,
                      active_cell -> box[l].param[0],
                      active_cell -> box[l].param[1],
                      & active_cell -> ltype,
                      & active_cell -> frac,
                      & active_cell -> pbc);
          }
          to_read_pos ();
        }
        int length = strlen (active_project -> coordfile);
        for (l=1; l<active_project -> nspec+1; l++)
        {
          m = strlen(active_chem -> label[l-1]);
          send_label_ (& l, & m, active_chem -> label[l-1]);
        }
        switch (j)
        {
          case 0:
            k = write_xyz_ (active_project -> coordfile, & length, & active_cell -> frac, & car_to_au);
            break;
          case 1:
            k = write_c3d_ (active_project -> coordfile, & length, & active_cell -> frac, & car_to_au);
            break;
        }
        if (k)
        {
          tmp_str = g_strdup_printf (_("Impossible to export the atomic coordinates\nError code: %d"), k);
          show_error (tmp_str, 0, MainWindow);
          g_free (tmp_str);
        }
        active_project_changed (pactive);
      }
      break;
    default:
      if (i == 0)
      {
        to_close_this_project (pactive, active_project);
      }
#ifdef GTK4
      destroy_this_native_dialog (info);
#else
      destroy_this_dialog (info);
#endif
      break;
  }
}

/*!
  \fn G_MODULE_EXPORT void on_coord_port (GtkWidget * widg, gpointer data)

  \brief export or import atomic coordinates: prepare dialog

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_coord_port (GtkWidget * widg, gpointer data)
{
  int i, j;
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  GtkFileChooser * chooser;
  gchar * tmp_str;
  int num_files[2]={NCFORMATS, 2};
  const gchar * str[2]= {i18n("Import atomic coordinates"), i18n("Export atomic coordinates")};
  const gchar * res[2]= {i18n("Open"), i18n("Save")};
  char * out_files[2] = {i18n("XYZ file"), i18n("Chem3D file")};
  char * out_ext[2]={"xyz", "c3d"};
  GtkFileChooserAction act[2]={GTK_FILE_CHOOSER_ACTION_OPEN, GTK_FILE_CHOOSER_ACTION_SAVE};
  pactive = activep;
  i = GPOINTER_TO_INT (data);

  if ((nprojects > 0 && get_project_by_id(activew) -> natomes) || i == 0)
  {
    if (i == 0)
    {
      init_project (TRUE);
    }
    else
    {
      active_project_changed (activew);
    }
    tmp_str = g_strdup_printf ("%s - %s", prepare_for_title(active_project -> name), _(str[i]));
    info = create_file_chooser (tmp_str,
                                GTK_WINDOW(MainWindow),
                                act[i],
                                _(res[i]));
    g_free (tmp_str);
    chooser = GTK_FILE_CHOOSER(info);
    if (i) gtk_file_chooser_set_create_folders (chooser, TRUE);
#ifdef GTK3
    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
    for (j=0; j<num_files[i]; j++)
    {
      filter[j] = gtk_file_filter_new();
      if (i == 0)
      {
        tmp_str = g_strdup_printf ("%s (*.%s)", _(coord_files[j]), coord_files_ext[j]);
      }
      else
      {
        tmp_str = g_strdup_printf ("%s (*.%s)", _(out_files[j]), out_ext[j]);
      }
      gtk_file_filter_set_name (GTK_FILE_FILTER(filter[j]), tmp_str);
      g_free (tmp_str);
      if (i == 0)
      {
        tmp_str = g_strdup_printf ("*.%s", coord_files_ext[j]);
      }
      else
      {
        tmp_str = g_strdup_printf ("*.%s", out_ext[j]);
      }
      gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter[j]), tmp_str);
      gtk_file_chooser_add_filter (chooser, filter[j]);
      g_free (tmp_str);
    }
    if (i==0)
    {
      filter[j] = gtk_file_filter_new();
      gtk_file_filter_set_name (GTK_FILE_FILTER(filter[j]), _("All files (*)"));
      gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter[j]), "*");
      gtk_file_chooser_add_filter (chooser, filter[j]);
    }
    else
    {
      file_chooser_set_current_folder (chooser);
      gtk_file_chooser_set_current_name (chooser, "coord.xyz");
    }
#ifdef GTK4
    run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_on_coord_port), GINT_TO_POINTER(i));
#else
    run_this_gtk_dialog (info, G_CALLBACK(run_on_coord_port), GINT_TO_POINTER(i));
#endif
  }
  else
  {
    if (nprojects == 0)
    {
      show_warning (_("No project loaded ... nothing to be saved\n"), MainWindow);
    }
    else
    {
      tmp_str = g_strdup_printf (_("Project <b>%s</b> is empty ... nothing to be saved\n"),
                                 get_project_by_id(activew) -> name);
      show_warning (tmp_str, MainWindow);
      g_free (tmp_str);
    }
  }
  activew = activep;
  update_insert_combos ();
}
