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
* @file w_colors.c
* @short Functions to create the color selection dialogs
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_colors.c'
*
* Contains:
*

 - The functions to create the color selection dialogs

*
* List of functions:

  void window_color (project * this_proj, glwin * view);

  G_MODULE_EXPORT void run_window_color (GtkDialog * win, gint response_id, gpointer data);
  G_MODULE_EXPORT void to_run_back_color_window (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_run_back_color_window (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void to_run_box_color_window (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_run_box_color_window (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void to_run_atom_color_window (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_run_atom_color_window (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void run_window_color_coord (GtkDialog * win, gint response_id, gpointer data);
  G_MODULE_EXPORT void window_color_coord (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void window_color_coord (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"
#include "color_box.h"
#include "preferences.h"

/*
  The object type for color change
    -2    : background
    -1    : box
     > -1 : coordination
*/
int wc_cid;

/*!
  \fn G_MODULE_EXPORT void run_window_color (GtkDialog * win, gint response_id, gpointer data)

  \brief window color chooser - running the dialog

  \param win the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_window_color (GtkDialog * win, gint response_id, gpointer data)
{
  project * this_proj = (project *)data;

  if (response_id == GTK_RESPONSE_OK)
  {
    ColRGBA colo = get_window_color (GTK_WIDGET(win));
    if (wc_cid == -2)
    {
      this_proj -> modelgl -> anim -> last -> img -> back -> color = colo;
      this_proj -> modelgl -> create_shaders[MEASU] = TRUE;
      cleaning_shaders (this_proj -> modelgl, BACKG);
    }
    else if (wc_cid == -1)
    {
      this_proj -> modelgl -> anim -> last -> img -> box_color = colo;
      this_proj -> modelgl -> create_shaders[MDBOX] = TRUE;
    }
    else
    {
      this_proj -> modelgl -> anim -> last -> img -> at_color[wc_cid] = colo;
      int shaders[2] = {ATOMS, BONDS};
      re_create_md_shaders (2, shaders, this_proj);
      int shader[1] = {POLYS};
      if (this_proj -> modelgl ->  anim -> last -> img -> color_map[1] == 0) re_create_md_shaders (1, shader, this_proj);
    }
    update (this_proj -> modelgl);
  }
  destroy_this_dialog (win);
}

/*!
  \fn void window_color (project * this_proj, glwin * view)

  \brief window color chooser - creating the dialog

  \param this_proj the target project
  \param view the target glwin
*/
void window_color (project * this_proj, glwin * view)
{
  gchar * str;
  GdkRGBA col;
  if (wc_cid == -2)
  {
    str = g_strdup_printf ("Background color");
    col = colrgba_togtkrgba (view -> anim -> last -> img -> back -> color);
  }
  else if (wc_cid == -1)
  {
    str = g_strdup_printf ("Model box color");
    col = colrgba_togtkrgba (view -> anim -> last -> img -> box_color);
  }
  else
  {
    if (wc_cid < this_proj -> nspec)
    {
      str = g_strdup_printf ("%s - atom(s) color", this_proj -> chemistry -> label[wc_cid]);
    }
    else
    {
      str = g_strdup_printf ("%s* - clone(s) color", this_proj -> chemistry -> label[wc_cid-this_proj -> nspec]);
    }
    col = colrgba_togtkrgba (view -> anim -> last -> img -> at_color[wc_cid]);
  }
  GtkWidget * win = gtk_color_chooser_dialog_new (str, GTK_WINDOW(view -> win));
  gtk_window_set_modal (GTK_WINDOW(win), TRUE);
  gtk_color_chooser_set_use_alpha (GTK_COLOR_CHOOSER(win), TRUE);
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER(win), & col);
  g_free (str);
  run_this_gtk_dialog (win, G_CALLBACK(run_window_color), this_proj);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void to_run_back_color_window (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief to run background color selection window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_run_back_color_window (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void to_run_back_color_window (GtkWidget * widg, gpointer data)

  \brief to run background color selection window callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_run_back_color_window (GtkWidget * widg, gpointer data)
#endif
{
  glwin * view = (glwin *) data;
  wc_cid = -2;
  window_color (get_project_by_id(view -> proj), view);
  update (view);
}

/*!
  \fn G_MODULE_EXPORT void set_gradient_color (GtkColorChooser * colob, gpointer data)

  \brief change background color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_gradient_color (GtkColorChooser * colob, gpointer data)
{
  tint * bid = (tint *)data;
  glwin * view;
  if (preferences)
  {
    switch (bid -> b)
    {
      case 0:
        tmp_background -> color = get_button_color (colob);
        break;
      default:
        tmp_background -> gradient_color[bid -> b -1] = get_button_color (colob);
        break;
    }
  }
  else
  {
    view = get_project_by_id(bid -> a) -> modelgl;
    switch (bid -> b)
    {
      case 0:
        view -> anim -> last -> img -> back -> color = get_button_color (colob);
        break;
      default:
        view -> anim -> last -> img -> back -> gradient_color[bid -> b -1] = get_button_color (colob);
        break;
    }
  }
  if (! preferences)
  {
    if (bid -> b)
    {
      view -> create_shaders[BACKG] = TRUE;
    }
    view -> create_shaders[MEASU] = TRUE;
    update (view);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_gradient_parameter (GtkWidget * widg, gpointer data)

  \brief set gradient parameter callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_gradient_parameter (GtkWidget * widg, gpointer data)
{
  tint * bid = (tint *)data;
  glwin * view;
  gradient_edition * the_gradient;
  int i = combo_get_active (widg);
  if (preferences)
  {
    the_gradient = pref_gradient_win;
    switch (bid -> b)
    {
      case 0:
        tmp_background -> gradient = i;
        break;
      case 1:
        tmp_background -> direction = i;
        break;
    }
  }
  else
  {
    view = get_project_by_id(bid -> a) -> modelgl;
    the_gradient = view -> gradient_win;
    switch (bid -> b)
    {
      case 0:
        view -> anim -> last -> img -> back -> gradient = i;
        break;
      default:
        view -> anim -> last -> img -> back -> direction = i;
        break;
    }
  }
  if (! bid -> b && ! i)
  {
    hide_the_widgets (the_gradient -> d_box[0]);
    hide_the_widgets (the_gradient -> d_box[1]);
    show_the_widgets (the_gradient -> color_box[0]);
    hide_the_widgets (the_gradient -> color_box[1]);
    if (! preferences) cleaning_shaders (view, BACKG);
  }
  else
  {
    if (! preferences)
    {
      view -> create_shaders[BACKG] = TRUE;
      // view -> create_shaders[MEASU] = TRUE;
    }
    show_the_widgets (the_gradient -> color_box[1]);
    hide_the_widgets (the_gradient -> color_box[0]);
    if (!  bid -> b)
    {
      if (i == 1)
      {
        show_the_widgets (the_gradient -> d_box[0]);
        hide_the_widgets (the_gradient -> d_box[1]);
        combo_set_active (the_gradient -> d_box[0], 0);
      }
      else
      {
        hide_the_widgets (the_gradient -> d_box[0]);
        show_the_widgets (the_gradient -> d_box[1]);
        combo_set_active (the_gradient -> d_box[1], 0);
      }
    }
  }
  if (! preferences)
  {
    update (view);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean on_gradient_delete (GtkWindow * widg, gpointer data)

  \brief gradient window delete event - GTK4

  \param widg
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_gradient_delete (GtkWindow * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_gradient_delete (GtkWidget * widg, GdkEvent * event, gpointer data)

  \brief gradient window delete event - GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_gradient_delete (GtkWidget * widg, GdkEvent * event, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  view -> gradient_win -> win = destroy_this_widget (view -> gradient_win -> win);
  g_free (view -> gradient_win);
  view -> gradient_win = NULL;
  return TRUE;
}

G_MODULE_EXPORT void gradient_advanced (GtkWidget * widg, gpointer data)
{
  GtkWidget * hbox, * vbox;
  GtkWidget * hhbox, * vvbox;
  gradient_edition * the_gradient;
  int back_gradient;
  int back_direction;
  ColRGBA back_color;
  ColRGBA * gradient_color;
  glwin * view;
  vbox = create_vbox (BSEP);
  if (preferences)
  {
    the_gradient = pref_gradient_win;
    back_gradient = tmp_background -> gradient;
    back_direction = tmp_background -> direction;
    back_color = tmp_background -> color;
    gradient_color = tmp_background -> gradient_color;
  }
  else
  {
    view = (glwin *)data;
    view -> gradient_win = g_malloc0(sizeof*view -> gradient_win);
    the_gradient = view -> gradient_win;
    back_gradient = view -> anim -> last -> img -> back -> gradient;
    back_direction = view -> anim -> last -> img -> back -> direction;
    back_color = view -> anim -> last -> img -> back -> color;
    gradient_color = view -> anim -> last -> img -> back -> gradient_color;
  }
  if (preferences)
  {
    the_gradient -> win = create_vbox (BSEP);
    adv_box (the_gradient -> win, "<b>Background settings</b>", 5, 120, 0.0);
    hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, the_gradient -> win, hbox, FALSE, FALSE, 20);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbox, FALSE, FALSE, 60);
  }
  else
  {
    gchar * str = g_strdup_printf ("Background settings - %s", get_project_by_id(view -> proj)->name);
    the_gradient -> win = create_win (str, view -> win, FALSE, FALSE);
    g_free (str);
    add_container_child (CONTAINER_WIN, the_gradient -> win, vbox);
  }

  gchar * g_name[2] = {"Gradient type", "Gradient direction"};
  gchar * g_type[3] = {"No gradient", "Linear", "Circular"};
  gchar * g_direction[2][9] = {{"Top to bottom", "Right to left", "Bottom right to top left", "Top right to bottom left", "", "", "", "", ""},
                               {"Right to left", "Left to right", "Top to bottom", "Bottom to top",
                                "Bottom right to top left", "Bottom left to top right", "Top right to bottom left", "Top left to bottom right", "Center"}};
  int n_val[2] = {4, 9};
  int i, j;
  hbox = abox (vbox, g_name[0], 5);
  the_gradient -> g_box = create_combo ();
  for (i=0; i<3; i++)
  {
    combo_text_append (the_gradient -> g_box, g_type[i]);
  }
  combo_set_active (the_gradient -> g_box, back_gradient);
  gtk_widget_set_size_request (the_gradient -> g_box, 200, -1);
  g_signal_connect (G_OBJECT (the_gradient -> g_box), "changed", G_CALLBACK(set_gradient_parameter), (preferences) ? & pref_pointer[0] : & view -> colorp[0][0]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, the_gradient -> g_box, FALSE, FALSE, 20);

  hbox = abox (vbox, g_name[1], 5);
  for (i=0; i<2; i++)
  {
    the_gradient -> d_box[i] = create_combo ();
    for (j=0; j<n_val[i]; j++)
    {
      combo_text_append (the_gradient -> d_box[i], g_direction[i][j]);
    }
    gtk_widget_set_size_request (the_gradient -> d_box[i], 200, -1);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, the_gradient -> d_box[i], FALSE, FALSE, 20);
  }
  if (back_gradient)
  {
    combo_set_active (the_gradient -> d_box[back_gradient-1], back_direction);
  }
  for (i=0; i<2; i++)
  {
    g_signal_connect (G_OBJECT (the_gradient -> d_box[i]), "changed", G_CALLBACK(set_gradient_parameter),  (preferences) ? & pref_pointer[1] : & view -> colorp[1][0]);
  }
  the_gradient -> color_box[0] = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, the_gradient -> color_box[0], FALSE, FALSE, 20);
  abox (the_gradient -> color_box[0], "Background color", 5);
  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, the_gradient -> color_box[0], hbox, FALSE, FALSE, 5);
  vvbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vvbox, FALSE, FALSE, 60);
  hhbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hhbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label ("Single color", 150, -1, 0.0, 0.5), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, color_button (back_color, FALSE, 100, -1, G_CALLBACK(set_gradient_color),  (preferences) ? & pref_pointer[0] : & view -> colorp[0][0]), FALSE, FALSE, 0);

  the_gradient -> color_box[1] = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, the_gradient -> color_box[1], FALSE, FALSE, 20);
  abox (the_gradient -> color_box[1], "Gradient colors", 5);
  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, the_gradient -> color_box[1], hbox, FALSE, FALSE, 5);
  vvbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vvbox, FALSE, FALSE, 60);
  gchar * c_name[2] = {"First color", "Second color"};
  for (i=0; i<2; i++)
  {
    hhbox = create_hbox (BSEP);
    // g_print ("col.r= %f, col.g= %f, col.b= %f\n", gradient_color[i].red, gradient_color[i].green, gradient_color[i].blue);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hhbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label (c_name[i], 150, -1, 0.0, 0.5), FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox,
                         color_button (gradient_color[i], FALSE, 100, -1, G_CALLBACK(set_gradient_color), (preferences) ? & pref_pointer[i+1] : & view -> colorp[i+1][0]), FALSE, FALSE, 0);
  }

  if (! preferences)
  {
    add_gtk_close_event (the_gradient -> win, G_CALLBACK(on_gradient_delete), view);
    show_the_widgets (the_gradient -> win);
    if (! back_gradient)
    {
      hide_the_widgets (the_gradient -> color_box[1]);
      for (i=0; i<2; i++) hide_the_widgets (the_gradient -> d_box[i]);
    }
    else
    {
      hide_the_widgets (the_gradient -> color_box[0]);
      if (back_gradient == 1) hide_the_widgets (the_gradient -> d_box[1]);
      if (back_gradient == 2) hide_the_widgets (the_gradient -> d_box[0]);
    }
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void to_run_box_color_window (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief to run box color selection window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_run_box_color_window (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void to_run_box_color_window (GtkWidget * widg, gpointer data)

  \brief  to run box color selection window callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_run_box_color_window (GtkWidget * widg, gpointer data)
#endif
{
  glwin * view = (glwin *) data;
  wc_cid = -1;
  window_color (get_project_by_id(view -> proj), view);
  view -> create_shaders[MDBOX] = TRUE;
  update (view);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void to_run_atom_color_window (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief to run atom color selection window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_run_atom_color_window (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void to_run_atom_color_window (GtkWidget * widg, gpointer data)

  \brief to run atom color selection window callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_run_atom_color_window (GtkWidget * widg, gpointer data)
#endif
{
  tint * id = (tint *) data;
  // g_debug ("Atom color:: proj= %d, id -> b= %d, id -> c= %d", id -> a,  id -> b, id -> c);
  project * this_proj = get_project_by_id(id -> a);
  wc_cid = id -> c;
  window_color (this_proj, this_proj -> modelgl);
  int shaders[3] = {ATOMS, BONDS, SELEC};
  re_create_md_shaders (3, shaders, this_proj);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void run_window_color_coord (GtkDialog * win, gint response_id, gpointer data)

  \brief window to select a color - running the dialog

  \param win the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_window_color_coord (GtkDialog * win, gint response_id, gpointer data)
{
  qint * cid = (qint *)data;
  int c, g, s;
  project * this_proj = get_project_by_id(cid -> a);
  s = cid -> b;
  c = cid -> c;
  g = cid -> d;
  if (response_id == GTK_RESPONSE_OK)
  {
    if (g > 1) s = 0;
    this_proj -> modelgl -> anim -> last -> img -> spcolor[g][s][c] = get_window_color (GTK_WIDGET(win));
    int shaders[4] = {ATOMS, BONDS, POLYS, RINGS};
    re_create_md_shaders (4, shaders, this_proj);
    update (this_proj -> modelgl);
  }
  destroy_this_dialog (win);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void window_color_coord (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief create a window to select a color callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void window_color_coord (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void window_color_coord (GtkWidget * widg, gpointer data)

  \brief create a window to select a color callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void window_color_coord (GtkWidget * widg, gpointer data)
#endif
{
  qint * cid = (qint *)data;
  gchar * str;
  int c, g, s;
  project * this_proj = get_project_by_id(cid -> a);
  s = cid -> b;
  c = cid -> c;
  g = cid -> d;
  g_debug ("s= %d, c= %d, g= %d", s, c, g);
  switch (g)
  {
    case 0:
      str = g_strdup_printf ("%s atom(s) %d fold coordination sphere color", this_proj -> chemistry -> label[s],
      this_proj -> coord -> geolist[0][s][c]);
      break;
    case 1:
      str = g_strdup_printf ("%s - %s coordination sphere color", this_proj -> chemistry -> label[s],
                             prepare_for_title(exact_name(env_name (this_proj, c, s, 1, NULL))));
      break;
    case 2:
      str = g_strdup_printf ("Fragment N°%d color", c);
      g = s;
      s = 0;
      break;
    case 3:
      str = g_strdup_printf ("Molecule N°%d color", c);
      g = s;
      s = 0;
      break;
    case 9:
      str = g_strdup_printf ("%d atom chain(s) color", this_proj -> coord -> geolist[g][0][c]);
      s = 0;
      break;
    default:
      str = g_strdup_printf ("%s - %d atom ring(s) color", rings_type[s], this_proj -> coord -> geolist[g][0][c]);
      s = 0;
      break;
  }
  GtkWidget * win = gtk_color_chooser_dialog_new (str, GTK_WINDOW(this_proj -> modelgl -> win));
  g_free (str);
  set_color_chooser_color (win, this_proj -> modelgl -> anim -> last -> img -> spcolor[g][s][c]);
  gtk_color_chooser_set_use_alpha (GTK_COLOR_CHOOSER(win), TRUE);
  gtk_window_set_modal ((GtkWindow *)win, TRUE);
  run_this_gtk_dialog (win, G_CALLBACK(run_window_color_coord), data);
}
