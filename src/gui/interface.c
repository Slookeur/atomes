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
* @file interface.c
* @short General messaging functions
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'interface.c'
*
* Contains:
*

 - General messaging functions

*
* List of functions:

  int dummy_ask_ (char * question);
  int iask (char * question, char * lab, int id, GtkWidget * win);

  gboolean ask_yes_no (gchar * title, gchar * text, int type, GtkWidget * widg);

  G_MODULE_EXPORT gboolean leaving_question (GtkWindow * widget, gpointer data);
  G_MODULE_EXPORT gboolean leaving_question (GtkWidget * widget, GdkEvent * event, gpointer data);

  gchar * exact_name (gchar * name);
  gchar * cask (char * question,  char * lab, int id, char * old, GtkWidget * win);
  gchar * textcolor (int i);
  gchar * env_name (project * this_proj, int g, int s, int f, GtkTextBuffer * buffer);

  void show_web (GtkWidget * dialog, int id);
  void show_info (char * information, int val, GtkWidget * win);
  void show_warning (char * warning, GtkWidget * win);
  void show_warning_ (char * warning, char * sub, char * tab);
  void show_error (char * error, int val, GtkWidget * win);
  void show_error_ (char * error, char * sub, char * tab);
  void init_data_ (int * nats, int * nspc, int * stps, int * cid);
  void print_info  (gchar * str, gchar * stag, GtkTextBuffer * buffer);
  void lattice_info_ (int * bid, double * volume, double * density,
                       double dvects[3][3], double rvects[3][3], double mod[3], double ang[3],
                       double f_to_c[3][3], double c_to_f[3][3]);
  void send_chem_info_ (int prop[active_project -> nspec]);
  void update_after_calc (int calc);

  G_MODULE_EXPORT void create_about_dialog (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void run_yes_no (GtkDialog * dial, gint response_id, gpointer data);
  G_MODULE_EXPORT void on_answer_changed (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void run_iask (GtkDialog * iask, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_cask (GtkDialog * cask, gint response_id, gpointer data);

  GtkWidget * addweb (int id);
  GtkWidget * show_pop (char * pop, GtkWidget * pwin);

*/

#include "global.h"
#include "bind.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"
#include "affero.h"

extern gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif);

GtkWidget * answer;

/* Old version to make url/email in about dialog clickable */
/* void about_dialog_handle_url (GtkAboutDialog * dialog, const gchar * link, gpointer data)
{
  GtkWidget * error_dialog;
  gchar * url, * escaped;
  g_return_if_fail (GTK_IS_ABOUT_DIALOG (dialog));
  g_return_if_fail (link != NULL);

/ * simple check if this is an email address * /
  if (! g_str_has_prefix (link, "mailto:") && strchr (link, '@') != NULL)
  {
    escaped = g_uri_escape_string (link, NULL, FALSE);
    url = g_strdup_printf ("mailto:%s", escaped);
    g_free (escaped);
  }
  else
  {
    url = g_strdup_printf ("%s", link);
  }

#ifdef G_OS_WIN32
 HINSTANCE h;
 h = ShellExecute (NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
 if ((int)h <= 32)
#else
#ifdef GTK4
  // gtk_show_uri (GTK_WINDOW (dialog), url, gtk_event_controller_get_current_event_time ( , ));
#else
  GError * error = NULL;
  gtk_show_uri_on_window (GTK_WINDOW (dialog), url, gtk_get_current_event_time (), & error);
  if (error != NULL)
#endif
#endif
  {
    gchar * uri = g_strdup_printf ("Failed to open link:\n%s", url);
    error_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog),
                                           GTK_DIALOG_DESTROY_WITH_PARENT,
                                           GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                           "Impossible to open link: ");
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s", uri);
    run_this_gtk_dialog (error_dialog, G_CALLBACK(run_destroy_dialog), NULL);
    g_free (uri);
  }
  g_free (url);
} */

/*!
  \fn GtkWidget * addweb (int id)

  \brief create a widget to present

  \param id Add contact info (1) or not (0)
*/
GtkWidget * addweb (int id)
{
  GtkWidget * web;
  const gchar * contact = " Please contact me at:\n";
  const gchar * seb     = " Dr. Sébastien Le Roux ";
  GtkWidget * eseb;
  GtkWidget * vbox;
  GtkWidget * hbox;
  const gchar * or = "\n Or check the website for information\n";
  if (id)
  {
    web = gtk_link_button_new_with_label (ATOMES_URL, "Visit the project's website");
  }
  gchar * mailto = g_strdup_printf ("mailo:%s", PACKAGE_BUGREPORT);
  gchar * mailsh = g_strdup_printf ("<%s>",PACKAGE_BUGREPORT);
  eseb = gtk_link_button_new_with_label (mailto, mailsh);
  g_free (mailto);
  g_free (mailsh);
  vbox = create_vbox (BSEP);
  if (id) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, gtk_label_new (contact), FALSE, FALSE, 0);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new (seb), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, eseb, FALSE, FALSE, 0);
  if (id)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, gtk_label_new (or), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, web, FALSE, FALSE, 0);
  }
  return (vbox);
}

/*!
  \fn G_MODULE_EXPORT void create_about_dialog (GtkWidget * widg, gpointer data)

  \brief create the about dialog

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void create_about_dialog (GtkWidget * widg, gpointer data)
{
  GtkWidget * aboutdialog;
#ifdef GTK4
  GdkPaintable * atomes_logo = gtk_image_get_paintable (GTK_IMAGE(gtk_image_new_from_file(PACKAGE_LAGPL)));
#else
  GdkPixbuf * atomes_logo = gdk_pixbuf_new_from_file (PACKAGE_LAGPL, NULL);
#endif
  const gchar *authors[] = {"Dr. Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>", "", NULL};
  const gchar * weblabel = "https://atomes.ipcms.fr";
  const gchar * comments = "Visualization, analyzis, creation/edition and post-processing of atomistic models !";
  const gchar * copyrights = "Copyright © 2025 \nDr. Sébastien Le Roux";

  // gtk_about_dialog_set_url_hook (about_dialog_handle_url, NULL, NULL);
  // gtk_about_dialog_set_email_hook (about_dialog_handle_url, NULL, NULL);

  aboutdialog = gtk_about_dialog_new ();
  gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG(aboutdialog), atomes_logo);
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(aboutdialog), VERSION);
  gchar * str = g_strdup_printf ("%s", PACKAGE);
  gtk_about_dialog_set_program_name (GTK_ABOUT_DIALOG(aboutdialog), str);
  g_free (str);
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG(aboutdialog), comments);
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG(aboutdialog), g_strdup_printf("%s", ATOMES_URL));
  gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG(aboutdialog), weblabel);
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG(aboutdialog), authors);
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG(aboutdialog), copyrights);
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG(aboutdialog), affero_license);
  gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG(aboutdialog), GTK_LICENSE_CUSTOM);
  gtk_about_dialog_set_wrap_license (GTK_ABOUT_DIALOG(aboutdialog), FALSE);
  run_this_gtk_dialog (aboutdialog,  G_CALLBACK(destroy_this_dialog), NULL);
}

/*!
  \fn void show_web (GtkWidget * dialog, int id)

  \brief add / show web information to widget

  \param dialog the GtkWidget to modify
  \param id Add contact info (1) or not (0)
*/
void show_web (GtkWidget * dialog, int id)
{
  GtkWidget * box  = dialog_get_content_area (dialog);
  GtkWidget * theweb = addweb (id);
  show_the_widgets (theweb);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, theweb, FALSE, FALSE, 0);
  show_the_widgets (box);
}

/*!
  \fn void show_info (char * information, int val, GtkWidget * win)

  \brief add / show information message to widget

  \param information Message
  \param val Add contact info (1) or not (0)
  \param win the GtkWidget to modify
*/
void show_info (char * information, int val, GtkWidget * win)
{
  gchar * info=NULL;
  if (val < 0)
  {
    info = g_strdup_printf ("%s\n%s", information, ifbug);
  }
  else
  {
    info = g_strdup_printf ("%s", information);
  }

  GtkWidget * dialog = message_dialogmodal (info, "Information", GTK_MESSAGE_INFO, GTK_BUTTONS_OK, win);
  if (val != 0) show_web (dialog, (val < 0) ? 0 : val);
  run_this_gtk_dialog (dialog, G_CALLBACK(run_destroy_dialog), NULL);
  g_free (info);
}

/*!
  \fn void show_warning (char * warning, GtkWidget * win)

  \brief show warning

  \param warning Message
  \param win Parent GtkWidget, if any
*/
void show_warning (char * warning, GtkWidget * win)
{
  GtkWidget * dialog = message_dialogmodal (warning,  "Warning", GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, win);
  run_this_gtk_dialog (dialog, G_CALLBACK(run_destroy_dialog), NULL);
}

/*!
  \fn void show_warning_ (char * warning, char * sub, char * tab)

  \brief show warning from Fortran90

  \param warning Message
  \param sub Fortan90 subroutine
  \param tab Fortran90 pointer
*/
void show_warning_ (char * warning, char * sub, char * tab)
{
  /* This function is called from fortran 90 */
  gchar * wtot=NULL;
  wtot = g_strdup_printf ("%s\n%s\n%s", warning, sub, tab);
  show_warning (wtot, MainWindow);
  g_free (wtot);
}

/*!
  \fn void show_error (char * error, int val, GtkWidget * win)

  \brief show error message

  \param error Message
  \param val Add contact info (1) or not (0)
  \param win Parent GtkWidget, if any
*/
void show_error (char * error, int val, GtkWidget * win)
{
  gchar * etot=NULL;
  if (val)
  {
    etot = g_strdup_printf ("%s", error);
  }
  else
  {
    etot = g_strdup_printf ("%s\n%s", error, ifbug);
  }
  GtkWidget * dialog = message_dialogmodal (etot, "Error", GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, win);
  show_web (dialog, val);
  g_warning ("%s", etot);
  run_this_gtk_dialog (dialog, G_CALLBACK(run_destroy_dialog), NULL);
  g_free (etot);
}

/*!
  \fn void show_error_ (char * error, char * sub, char * tab)

  \brief show error from Fortran90

  \param error Message
  \param sub Fortan90 subroutine
  \param tab Fortran90 pointer
*/
void show_error_ (char * error, char * sub, char * tab)
{
  /* This function is called from fortran 90 */
  gchar * etot=NULL;
  etot = g_strdup_printf ("%s\n\t%s\n\t%s", error, sub, tab);
  show_error (etot, 0, MainWindow);
  g_free (etot);
}

gboolean res_yes_no;

/*!
  \fn G_MODULE_EXPORT void run_yes_no (GtkDialog * dial, gint response_id, gpointer data)

  \brief ask yes or no for something: running dialog

  \param dial the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_yes_no (GtkDialog * dial, gint response_id, gpointer data)
{
  res_yes_no = (response_id == GTK_RESPONSE_YES) ? TRUE : FALSE;
  destroy_this_dialog (dial);
}

/*!
  \fn gboolean ask_yes_no (gchar * title, gchar * text, int type, GtkWidget * widg)

  \brief ask yes or no for something: prepare dialog

  \param title Title
  \param text Message
  \param type the type of message window
  \param widg the parent GtkWidget, if any
*/
gboolean ask_yes_no (gchar * title, gchar * text, int type, GtkWidget * widg)
{
  GtkWidget * dialog = message_dialogmodal (text, title, type, GTK_BUTTONS_YES_NO, widg);
  run_this_gtk_dialog (dialog, G_CALLBACK(run_yes_no), NULL);
  return res_yes_no;
}

/*!
  \fn gchar * exact_name (gchar * name)

  \brief short cut to print string without spaces

  \param name the initial string
*/
gchar * exact_name (gchar * name)
{
  return substitute_string (name, " ", NULL);
}

/*!
  \fn GtkWidget * show_pop (char * pop, GtkWidget * pwin)

  \brief display pop information window

  \param pop Message
  \param pwin Parent widget, if any
*/
GtkWidget * show_pop (char * pop, GtkWidget * pwin)
{
  GtkWidget * wpop = create_win ("Information", pwin, TRUE, FALSE);
  gtk_widget_set_size_request (wpop, 600, 80);
#ifdef GTK3
  gtk_window_set_position (GTK_WINDOW (wpop), GTK_WIN_POS_CENTER);
#endif
  GtkWidget * hpop = create_hbox (0);
  add_container_child (CONTAINER_WIN, wpop, hpop);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hpop, stock_image(EXECUTE), TRUE, TRUE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hpop, gtk_label_new (pop), TRUE, TRUE, 0);
  show_the_widgets (wpop);
  return (wpop);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean leaving_question (GtkWindow * widget, gpointer data)

  \brief Leaving atomes ?

  \param widget the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean leaving_question (GtkWindow * widget, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean leaving_question (GtkWidget * widget, GdkEvent * event, gpointer data)

  \brief Leaving atomes ?

  \param widget the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean leaving_question (GtkWidget * widget, GdkEvent * event, gpointer data)
#endif
{
  if (ask_yes_no ("Leaving ?!", "Are you sure you want to quit ?", GTK_MESSAGE_QUESTION, MainWindow))
  {
    quit_gtk ();
  }
  else
  {
    show_the_widgets (MainWindow);
  }
  return TRUE;
}

/*!
  \fn int dummy_ask_ (char * question)

  \brief Ask to use dummy atoms or not from Fortran90

  \param question Message
*/
int dummy_ask_ (char * question)
{
  GtkWidget * dask = message_dialogmodal (question, "Parameter required", GTK_MESSAGE_INFO, GTK_BUTTONS_YES_NO, MainWindow);
  run_this_gtk_dialog (dask, G_CALLBACK(run_yes_no), NULL);
  return (res_yes_no) ? 0 : -1;
}

extern  gchar * field_init[3];
gchar * coord_type[3]={"Cartesian",
                       "Atomic units",
                       "Fractional"};
gchar * npt_type[4]={"A\tB\tC\t&#x3B1;\t&#x3B2;\t&#x263;",
                     "A\tB\tC\n&#x3B1;\t&#x3B2;\t&#x263;",
                     "a<sub>x</sub>\ta<sub>y</sub>\ta<sub>z</sub>\tb<sub>x</sub>\tb<sub>y</sub>\tb<sub>z</sub>\tc<sub>x</sub>\tc<sub>y</sub>\tc<sub>z</sub>",
                     "a<sub>x</sub>\ta<sub>y</sub>\ta<sub>z</sub>\nb<sub>x</sub>\tb<sub>y</sub>\tb<sub>z</sub>\nc<sub>x</sub>\tc<sub>y</sub>\tc<sub>z</sub>"};
gchar * npt_info[3]={"1 line by step, as many lines as MD steps",
                     "2 lines by step, twice as many lines as MD steps",
                     "3 lines by step, three times as many lines as MD steps"};
GtkWidget * answer_info;

/*!
  \fn G_MODULE_EXPORT void on_answer_changed (GtkWidget * widg, gpointer data)

  \brief Handling the GtkComboBox in 'int iask'

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_answer_changed (GtkWidget * widg, gpointer data)
{
  int i, j;
  i = gtk_combo_box_get_active (GTK_COMBO_BOX(widg));
  j = (! i || i == 2) ? 0 : (i == 1) ? 1 : 2;
  gtk_label_set_text (GTK_LABEL(answer_info), npt_info[j]);
}

int res_int;

/*!
  \fn G_MODULE_EXPORT void run_iask (GtkDialog * iask, gint response_id, gpointer data)

  \brief enter an integer value - running the dialog

  \param iask the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_iask (GtkDialog * iask, gint response_id, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  gboolean done = FALSE;
  const gchar * riask;
  if (response_id == GTK_RESPONSE_OK)
  {
    if (i == 0 || i > 3)
    {
      riask = entry_get_text (GTK_ENTRY(answer));
      res_int = string_to_double ((gpointer)riask);
      if (i > 4)
      {
        if (res_int > 0 && res_int < i+1)
        {
          done = TRUE;
          res_int --;
        }
      }
      else if (res_int > 0)
      {
        done = TRUE;
      }
    }
    else
    {
      done = TRUE;
      res_int = gtk_combo_box_get_active (GTK_COMBO_BOX(answer));
    }
  }
  if (done) destroy_this_dialog (iask);
}

/*!
  \fn int iask (char * question, char * lab, int id, GtkWidget * win)

  \brief enter an integer value - prepare the dialog

  \param question Message
  \param lab Text to use for label
  \param id the required parameter id
  \param win the parent GtkWidget, if any
*/
int iask (char * question, char * lab, int id, GtkWidget * win)
{
  GtkWidget * iask;
  GtkWidget * vbox;
  GtkWidget * hboxa;
  GtkWidget * quest;
  int i;
  iask = message_dialogmodal (question, "Parameter Required", GTK_MESSAGE_INFO, GTK_BUTTONS_OK, win);
  vbox = dialog_get_content_area (iask);
  gtk_box_set_spacing (GTK_BOX(vbox), 15);
  hboxa = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hboxa, TRUE, TRUE, 0);
  quest = gtk_label_new (lab);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hboxa, quest, TRUE, TRUE, 0);

  if (id == 0 || id > 3)
  {
    answer = gtk_entry_new ();
    gtk_widget_set_size_request (answer, 100, -1);
    gtk_entry_set_alignment (GTK_ENTRY(answer), 1.0);
  }
  else
  {
    if (id < 3)
    {
      answer = create_combo ();
      gtk_widget_set_size_request (answer, -1, 40);
      if (id < 0) for (i=0; i<3; i++) combo_text_append (answer, field_init[i]);
      if (id == 1) for (i=0; i<3; i++) combo_text_append (answer, coord_type[i]);
      if (id == 2) for (i=0; i<NCFORMATS; i++) combo_text_append (answer, coord_files[i]);
    }
    else
    {
      GtkTreeIter iter;
      GtkTreeStore * store = store = gtk_tree_store_new (1, G_TYPE_STRING);
      for (i=0; i<4; i++)
      {
        gtk_tree_store_append (store, & iter, NULL);
        gtk_tree_store_set (store, & iter, 0, npt_type[i], -1);
      }
      answer = gtk_combo_box_new_with_model (GTK_TREE_MODEL(store));
      GtkCellRenderer * renderer = gtk_cell_renderer_combo_new ();
      gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (answer), renderer, TRUE);
      gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (answer), renderer, "text", 0, NULL);
      GList * cell_list = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT(answer));
      if(cell_list && cell_list -> data)
      {
        gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(answer), cell_list -> data, "markup", 0, NULL);
      }
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX(answer), 0);
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hboxa, answer, FALSE, FALSE, 10);
  if (id == 3)
  {
    answer_info = markup_label(npt_info[0], -1, -1, 0.5, 0.5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, answer_info, FALSE, FALSE, 5);
    g_signal_connect(G_OBJECT(answer), "changed", G_CALLBACK(on_answer_changed), NULL);
  }

  run_this_gtk_dialog (iask, G_CALLBACK(run_iask), GINT_TO_POINTER(id));
  return res_int;
}

gchar * res_char;

/*!
  \fn G_MODULE_EXPORT void run_cask (GtkDialog * cask, gint response_id, gpointer data)

  \brief enter a string - running the dialog

  \param cask the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_cask (GtkDialog * cask, gint response_id, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  gboolean done = FALSE;
  if (i < 0)
  {
    while (! done)
    {
      if (response_id == GTK_RESPONSE_OK)
      {
        res_char = g_strdup_printf ("%s", entry_get_text (GTK_ENTRY(answer)));
        done = TRUE;
      }
    }
  }
  else
  {
    if (response_id == GTK_RESPONSE_OK)
    {
      res_char = g_strdup_printf ("%s", entry_get_text (GTK_ENTRY(answer)));
    }
    else
    {
      res_char = NULL;
    }
    done = TRUE;
  }
  if (done) destroy_this_dialog (cask);
}

/*!
  \fn gchar * cask (char * question,  char * lab, int id, char * old, GtkWidget * win)

  \brief enter a string - prepare the dialog

  \param question Message
  \param lab Text to use for label
  \param id the required parameter id
  \param old the initial value for the string
  \param win the parent GtkWidget, if any
*/
gchar * cask (char * question,  char * lab, int id, char * old, GtkWidget * win)
{
  GtkWidget * cask;
  GtkWidget * dialog_ask;
  GtkWidget * hboxa;
  GtkWidget * quest;
  res_char = NULL;
  cask = message_dialogmodal (question, "Parameter required", GTK_MESSAGE_INFO, GTK_BUTTONS_OK, win);
  dialog_ask = dialog_get_content_area (cask);
  gtk_box_set_spacing (GTK_BOX(dialog_ask), 15);

  hboxa = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, dialog_ask, hboxa, TRUE, TRUE, 0);
  show_the_widgets (hboxa);
  quest = gtk_label_new (lab);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hboxa, quest, TRUE, TRUE, 0);
  show_the_widgets (quest);
  answer = gtk_entry_new ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hboxa, answer, TRUE, TRUE, 0);
  gtk_widget_set_size_request (answer, 50, -1);
  gtk_entry_set_alignment (GTK_ENTRY(answer), 1.0);
  update_entry_text (GTK_ENTRY(answer), old);
  run_this_gtk_dialog (cask, G_CALLBACK(run_cask), GINT_TO_POINTER(id));

  return (res_char);
}

/*!
  \fn void init_data_ (int * nats, int * nspc, int * stps, int * cid)

  \brief update project data using information from Fortran90

  \param nats number of atoms
  \param nspc number of species
  \param stps number of steps
  \param cid allocate chemistry data (1) or not (0)
*/
void init_data_ (int * nats, int * nspc, int * stps, int * cid)
{
  active_project -> natomes = * nats;
  active_project -> nspec = * nspc;
  active_project -> steps = * stps;
  alloc_proj_data (active_project, * cid);
  if (* cid) active_chem = active_project -> chemistry;
}

/*!
  \fn void spec_data_ (int * status, int * ind, int * atd, int * nsp,
*                   char * lbel, char * el_nme,
*                   double * amss, double * rdus,
*                   double * nscatt, double * xscatt)

  \brief update project data using information from Fortran90

  \param status Update data (1) or not (0)
  \param ind the chemical species
  \param atd Z
  \param nsp Number of atoms of this species
  \param lbel Symbol
  \param el_nme Element
  \param amss M
  \param rdus Radius
  \param nscatt Neutron scattering length
  \param xscatt X scattering length
*/
void spec_data_ (int * status, int * ind, int * atd, int * nsp,
                 char * lbel, char * el_nme,
                 double * amss, double * rdus,
                 double * nscatt, double * xscatt)
{
  int id = * ind;
  active_chem -> label[id]= g_strdup_printf("%s", lbel);
  active_chem -> element[id]= g_strdup_printf("%s", el_nme);
  if (* status)
  {
    active_chem -> nsps[id] = * nsp;
    active_chem -> chem_prop[CHEM_Z][id] = (double) * atd;
    active_chem -> chem_prop[CHEM_M][id] = * amss;
    active_chem -> chem_prop[CHEM_R][id] = * rdus;
    active_chem -> chem_prop[CHEM_N][id] = * nscatt;
    active_chem -> chem_prop[CHEM_X][id] = * xscatt;
  }
}

/*!
  \fn void print_info  (gchar * str, gchar * stag, GtkTextBuffer * buffer)

  \brief print information in GtkTextBuffer

  \param str the text
  \param stag the tags
  \param buffer the GtkTextBuffer to print to
*/
void print_info  (gchar * str, gchar * stag, GtkTextBuffer * buffer)
{
  GtkTextIter bEnd;
  GtkTextTag * tag;

  gtk_text_buffer_get_end_iter (buffer, &bEnd);
  if (stag != NULL)
  {
    tag = gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table(buffer), stag);
    gtk_text_buffer_insert_with_tags (buffer, &bEnd, str, -1, tag, NULL);
  }
  else
  {
    tag = gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table(buffer), "default-size");
    gtk_text_buffer_insert_with_tags (buffer, &bEnd, str, -1, tag, NULL);
  }
}

/*!
  \fn gchar * textcolor (int i)

  \brief setup text color keyword

  \param i color id
*/
gchar * textcolor (int i)
{
  gchar * col = NULL;
  switch (i - i * (i / 9))
  {
    case 0:
      col = g_strdup_printf ("red");
      break;
    case 1:
      col = g_strdup_printf ("blue");
      break;
    case 2:
      col = g_strdup_printf ("cyan");
      break;
    case 3:
      col = g_strdup_printf ("green");
      break;
    case 4:
      col = g_strdup_printf ("light_green");
      break;
    case 5:
      col = g_strdup_printf ("yellow");
      break;
    case 6:
      col = g_strdup_printf ("orange");
      break;
    case 7:
      col = g_strdup_printf ("violet");
      break;
    case 8:
      col = g_strdup_printf ("pink");
      break;
  }
  return col;
}

/*!
  \fn void lattice_info_ (int * bid, double * volume, double * density,
                       double dvects[3][3], double rvects[3][3], double mod[3], double ang[3],
                       double f_to_c[3][3], double c_to_f[3][3])

  \brief lattice data from Fortran90

  \param bid 0 or MD step if NPT
  \param volume volume
  \param density density
  \param dvects direct space lattice vectors
  \param rvects reciprocal lattice vectors
  \param mod modulus of lattice vectors (a,b,c)
  \param ang lattice angles (alpha, beta, gamma)
  \param f_to_c fractional to cartesian matrix
  \param c_to_f cartesian to fractional matrix
*/
void lattice_info_ (int * bid, double * volume, double * density,
                    double dvects[3][3], double rvects[3][3], double mod[3], double ang[3],
                    double f_to_c[3][3], double c_to_f[3][3])
{
  int i, j;
  for ( i=0; i<3; i++)
  {
    for (j=0; j<3; j++)
    {
      active_cell -> box[* bid].vect[i][j] = dvects[j][i];
      active_cell -> box[* bid].rvect[i][j] = rvects[j][i];
    }
    active_cell -> box[* bid].param[0][i] = mod[i];
    active_cell -> box[* bid].param[1][i] = ang[i];
  }
  active_cell -> box[* bid].frac_to_cart = mat4(f_to_c[0][0], f_to_c[0][1], f_to_c[0][2], 0.0,
                                                f_to_c[1][0], f_to_c[1][1], f_to_c[1][2], 0.0,
                                                f_to_c[2][0], f_to_c[2][1], f_to_c[2][2], 0.0,
                                                0.0, 0.0, 0.0, 0.0);
  active_cell -> box[* bid].cart_to_frac = mat4(c_to_f[0][0], c_to_f[0][1], c_to_f[0][2], 0.0,
                                                c_to_f[1][0], c_to_f[1][1], c_to_f[1][2], 0.0,
                                                c_to_f[2][0], c_to_f[2][1], c_to_f[2][2], 0.0,
                                                0.0, 0.0, 0.0, 0.0);
#ifdef DEBUG
  // m4_print (active_cell -> box[* bid].frac_to_cart);
  // m4_print (active_cell -> box[* bid].cart_to_frac);
#endif
  active_cell -> box[* bid].vol = * volume;
  active_cell -> box[* bid].dens = * density;
  if ((active_cell -> npt && * bid == active_project -> steps-1) || ! active_cell -> npt)
  {
    active_project -> max[GR] = fdmax_ (& active_cell -> pbc);
    active_project -> min[SQ] = active_project -> min[SK] = fkmin_ (& active_cell -> pbc);
    int i, j;
    active_cell -> volume = active_cell -> density = 0.0;
    i = (active_cell -> npt) ? active_project -> steps : 1;
    for (j=0; j<i; j++)
    {
      active_cell -> volume += active_cell -> box[j].vol;
      active_cell -> density += active_cell -> box[j].dens;
    }
    active_cell -> volume /= i;
    active_cell -> density /= i;
  }
}

/*!
  \fn void send_chem_info_ (int prop[active_project->nspec])

  \brief getting chemistry formula information from Fortran90

  \param prop the formula
*/
void send_chem_info_ (int prop[active_project -> nspec])
{
  int i;
  for (i=0; i<active_project -> nspec; i++)
  {
    active_chem -> formula[i] = prop[i];
  }
}

/*!
  \fn gchar * env_name (project * this_proj, int g, int s, int f, GtkTextBuffer * buffer)

  \brief ouput the name of a coordination sphere

  \param this_proj the target project
  \param g the coordination (0 = total, 1 = partial)
  \param s the chemical species
  \param f With markup or not
  \param buffer Output in a GtkTextBuffer, or not if NULL
*/
gchar * env_name (project * this_proj, int g, int s, int f, GtkTextBuffer * buffer)
{
  int l, m;
  gchar * spec = exact_name(this_proj -> chemistry -> label[s]);
  gchar * stra;
  gchar * strb;

  m = 0;
  for (l=0; l<this_proj -> nspec; l++)
  {
    m += this_proj -> coord -> partial_geo[s][g][l];
  }
  if (m > 0)
  {
    stra = g_strdup_printf ("%s [", spec);
    if (buffer != NULL)
    {
      print_info (spec, textcolor(s), buffer);
      print_info ("[", "bold", buffer);
    }
    for (l=0; l<this_proj -> nspec; l++)
    {
      m = this_proj -> coord -> partial_geo[s][g][l];
      if (m > 1)
      {
        if (f == 1)
        {
          strb = g_strdup_printf ("%s%s<sub>%d</sub>", stra, exact_name(this_proj -> chemistry -> label[l]), m);
        }
        else
        {
          strb = g_strdup_printf ("%s%s%d", stra, exact_name(this_proj -> chemistry -> label[l]), m);
        }
        if (buffer != NULL)
        {
          print_info (exact_name(this_proj -> chemistry -> label[l]), "bold", buffer);
          g_free (strb);
          strb = g_strdup_printf ("%d", m);
          print_info (strb, "sub", buffer);
        }
      }
      else if (m > 0)
      {
        strb = g_strdup_printf ("%s%s", stra, exact_name(this_proj -> chemistry -> label[l]));
        if (buffer != NULL)
        {
          print_info (exact_name(this_proj -> chemistry -> label[l]), "bold", buffer);
        }
      }
      else
      {
        strb = g_strdup_printf ("%s", stra);
      }
      g_free (stra);
      stra = g_strdup_printf ("%s", strb);
      g_free (strb);
    }
    strb = g_strdup_printf ("%s]", stra);
    if (buffer != NULL)
    {
      print_info ("]", "bold", buffer);
    }
    g_free (stra);
  }
  else
  {
    strb = g_strdup_printf ("%s - isolated", spec);
    if (buffer != NULL)
    {
      print_info (spec, textcolor(s), buffer);
      print_info (" - isolated", "bold", buffer);
    }
  }
  if (buffer != NULL)
  {
    strb = NULL;
  }
  g_free (spec);
  return strb;
}

/*!
  \fn void update_after_calc (int calc)

  \brief To update all curve plots in the workspace after a calculation

  \param calc Analysis id
*/
void update_after_calc (int calc)
{
  int i, j;
  tint cd;
  project * this_proj;
  for (i=0; i<nprojects; i++)
  {
    this_proj = get_project_by_id(i);
    if (this_proj -> initok[calc])
    {
      for (j= 0; j < this_proj -> numc[calc]; j++)
      {
        if (this_proj -> curves[calc][j] -> plot != NULL)
        {
          cd.a = i;
          cd.b = calc;
          cd.c = j;
          update_curve ((gpointer)& cd);
        }
      }
    }
  }
}
