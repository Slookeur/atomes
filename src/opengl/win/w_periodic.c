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
* @file w_periodic.c
* @short Functions to create the periodic table of the elements \n
         Functions to select element from the periodic table
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_periodic.c'
*
* Contains:
*

 - The functions to create the periodic table of the elements
 - The functions to select element from the periodic table

*
* List of functions:

  int is_in_table (int i, int j);
  int get_atom_id_from_periodic_table (atom_search * asearch);

  G_MODULE_EXPORT gboolean on_element_focus (GtkWidget * widget, GdkEvent * event, gpointer data);

  gchar * get_electronic_structure (int r, int c);

  G_MODULE_EXPORT void on_element_focus (GtkEventControllerFocus * focus, gpointer data);
  G_MODULE_EXPORT void on_element_motion_enter (GtkEventControllerMotion * motion, gdouble x, gdouble y, gpointer data);
  G_MODULE_EXPORT void run_periodic_table (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void get_element (GtkButton * but, gpointer data);

  GtkWidget * create_el_preview (int p, int a, int r, int c);
  GtkWidget * el_preview (int p, int a);
  GtkWidget * create_css_button (int p, int id, int r, int c);
  GtkWidget * create_css_group (int p, int i);
  GtkWidget * css_element (int p, int i, int j);
  GtkWidget * periodic_table (int p, int a);

*/

#include "global.h"

extern insertion_menu mol[];

atom_search * periodic_search;
int element;

dint table_p[118];

ColRGBA rtcolo[11] = {{142, 255, 142, 1.0}, // #8EFF8E
                      {245, 255, 83, 1.0},  // #F5FF53
                      {255, 184, 83, 1.0},  // #FFB853
                      {255, 108, 125, 1.0}, // #FF6C7D
                      {244, 167, 255, 1.0}, // #F4A7FF
                      {64, 133, 56},        // #408538
                      {177, 94, 255},       // #B15EFF
                      {142, 227, 255, 1.0}, // #8EE3FF
                      {80, 93, 253, 1.0},   // #505DFD
                      {126, 255, 235, 1.0}, // #7EFFEB
                      {95, 192, 176, 1.0}}; // #5FC0B0
gchar * ptcolo[11] = {"#8EFF8E", "#F5FF53", "#FFB853", "#FF6C7D", "#F4A7FF", "#408538", "#B15EFF", "#8EE3FF", "#505DFD", "#7EFFEB", "#5FC0B0"};

/*!
  \fn gchar * get_electronic_structure (int r, int c)

  \brief get electronic structure for this (row,column) combination

  \param r row
  \param c column
*/
gchar * get_electronic_structure (int r, int c)
{
  gchar * str = NULL;
  gchar * prev[10]={" ", "He", "Ne", "Ar", "Kr", "Xe", "Rn", " ", "Xe", "Rn"};
  if (r == -1 && c == -1) return g_strdup_printf ("Electronic structure: [None]");
  if (r > 0)
  {
    str = g_strdup_printf ("Electronic structure:\t [%s]", prev[r]);
  }
  else
  {
    str = g_strdup_printf ("Electronic structure:\t ");
  }

  if (c < 2)
  {
    str = g_strdup_printf ("%s %ds<sup>%d</sup>", str, r+1, c+1);
  }
  else
  {
    if (r < 3)
    {
      str = g_strdup_printf ("%s %ds<sup>%d</sup> %dp<sup>%d</sup>", str, r+1, 2, r+1, c-11);
    }
    else if (r < 5)
    {
      if (c == 5 || c == 10)
      {
        str = g_strdup_printf ("%s %dd<sup>%d</sup> %ds<sup>%d</sup>", str, r, c, r+1, 1);
      }
      else if (r == 4 && (c == 4 || c == 7 ||c == 8))
      {
        str = g_strdup_printf ("%s %dd<sup>%d</sup> %ds<sup>%d</sup>", str, r, c, r+1, 1);
      }
      else if (r == 4 && c == 9)
      {
        str = g_strdup_printf ("%s %dd<sup>%d</sup>", str, r, 10);
      }
      else if (c < 12)
      {
        str = g_strdup_printf ("%s %dd<sup>%d</sup> %ds<sup>%d</sup>", str, r, c-1, r+1, 2);
      }
      else
      {
        str = g_strdup_printf ("%s %dd<sup>%d</sup> %ds<sup>%d</sup> %dp<sup>%d</sup>", str, r, 10, r+1, 2, r+1, c-11);
      }
    }
    else if (r < 7)
    {
      if (c == 9 || c == 10)
      {
        str = g_strdup_printf ("%s  %df<sup>%d</sup> %dd<sup>%d</sup> %ds<sup>%d</sup>", str, r-1, 14, r, c, r+1, 1);
      }
      else if (c < 12)
      {
        str = g_strdup_printf ("%s  %df<sup>%d</sup> %dd<sup>%d</sup> %ds<sup>%d</sup>", str, r-1, 14, r, c-1, r+1, 2);
      }
      else
      {
        str = g_strdup_printf ("%s %df<sup>%d</sup> %dd<sup>%d</sup> %ds<sup>%d</sup> %dp<sup>%d</sup>", str, r-1, 14, r, 10, r+1, 2, r+1, c-11);
      }
    }
    else if (c == 3)
    {
      str = g_strdup_printf ("%s  %dd<sup>%d</sup> %ds<sup>%d</sup>", str, r-3, c-2, r-2, 2);
    }
    else if ((c > 7 && c < 10) || (c > 10 && c < 17))
    {
      str = g_strdup_printf ("%s  %df<sup>%d</sup> %ds<sup>%d</sup>", str, r-4, c-2, r-2, 2);
    }
    else if (r == 8)
    {
      if (c > 4 && c < 8)
      {
        str = g_strdup_printf ("%s  %df<sup>%d</sup> %ds<sup>%d</sup>", str, r-4, c-2, r-2, 2);
      }
      else
      {
        str = g_strdup_printf ("%s  4f<sup>%d</sup> 5d<sup>1</sup> 6s<sup>2</sup>", str, c-3);
      }
    }
    else
    {
      if (c == 4)
      {
        str = g_strdup_printf ("%s  %dd<sup>%d</sup> %ds<sup>%d</sup>", str, r-3, c-2, r-2, 2);
      }
      else if (c == 17)
      {
        str = g_strdup_printf ("%s  %df<sup>%d</sup> %ds<sup>%d</sup> %dp<sup>%d</sup>", str, r-4, c-3, r-2, 2, r-2, 1);
      }
      else
      {
        str = g_strdup_printf ("%s  %df<sup>%d</sup> %dd<sup>%d</sup> %ds<sup>%d</sup>", str, r-4, c-3, r-3, 1, r-2, 2);
      }
    }
  }
  return str;
}

/*!
  \fn int is_in_table (int i, int j)

  \brief is this (row,column) combination a chemical element ?

  \param i row
  \param j column
*/
int is_in_table (int i, int j)
{
  if (i == 7) return 0;
  if (i == 0 && (j > 0 && j < 17)) return 0;
  if ((i == 1 || i == 2) && (j > 1 && j < 12)) return 0;
  if ((i == 5 || i == 6) && j ==2) return 0;
  if (i > 7 && j< 3) return 0;
  if (i > 2 && i < 5) return 19 + (i-3)*18 + j;
  if (i == 0)
  {
    if (j==0)
    {
      return 1;
    }
    else
    {
      return 2;
    }
  }
  if (i > 0 && i < 3) return 2 + j+1 - 10 * (j/11) + 8 * (i/2);
  if (i == 5 && j < 2) return 55 + j;
  if (i == 6 && j < 2) return 87 + j;
  if (i == 5 && j > 2) return 72 + j-3;
  if (i == 6 && j > 2) return 104 + j-3;
  if (i == 8) return 57 + j-3;
  if (i == 9) return 89 + j-3;
  return -1;
}

/*!
  \fn GtkWidget * create_el_preview (int p, int a, int r, int c)

  \brief create element preview

  \param p the target project id
  \param a target atom search id
  \param r row
  \param c column
*/
GtkWidget * create_el_preview (int p, int a, int r, int c)
{
  GtkWidget * preview = create_vbox (5);
  GtkWidget * hbox;
  GtkWidget * lab[5];
  int i, id;
  int size[5] = {125, 30, 15, 15, 15};
  gchar * backcol;
  gchar * name;
  gchar * str;
  if (r < 0 && c < 0)
  {
    id = 0;
  }
  else
  {
    id = is_in_table (r, c);
  }
  for (i=0; i<5; i++)
  {
    backcol = g_strdup_printf ("label#prev-%d-%d-%d {\n"
                               "  color: black;\n"
                               "  font-weight: bold;\n"
                               "  font-size: %dpx;\n"
                               "}\n", i, p, a, size[i]);
    provide_gtk_css (backcol);
    g_free (backcol);
    if (i == 2 || i == 4) hbox = create_hbox (0);
    name = g_strdup_printf ("prev-%d-%d-%d", i, p, a);
    switch (i)
    {
      case 0:
        lab[i] = gtk_label_new (periodic_table_info[id].lab);
        break;
      case 1:
        lab[i] = gtk_label_new (periodic_table_info[id].name);
        break;
      case 2:
        str = g_strdup_printf ("Z= %d", periodic_table_info[id].Z);
        lab[i] = gtk_label_new (str);
        g_free (str);
        break;
      case 3:
        str = g_strdup_printf ("M= %.3f g/mol", periodic_table_info[id].M);
        lab[i] = gtk_label_new (str);
        g_free (str);
        break;
      case 4:
        lab[i] = markup_label(get_electronic_structure (r, c), -1, -1, 0.0, 0.5);
        break;

    }
    gtk_widget_set_name (lab[i], name);
    g_free (name);
    if (i < 2)
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, preview, lab[i], FALSE, FALSE, 0);
    }
    else
    {
      if (i == 2 || i == 4) add_box_child_start (GTK_ORIENTATION_VERTICAL, preview, hbox, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab[i], FALSE, FALSE, 30);
    }
  }
  show_the_widgets (preview);
  return preview;
}

/*!
  \fn GtkWidget * el_preview (int p, int a)

  \brief create element preview CSS widget

  \param p the target project id
  \param a target atom search id
*/
GtkWidget * el_preview (int p, int a)
{
  GtkWidget * preview = create_vbox (BSEP);
  gchar * backcol = g_strdup_printf ("box#preview-%d-%d {\n"
                                     "border: 3px solid black;\n"
                                     "background-color: white;\n"
                                     "border-top-left-radius: 60px 90px;\n"
                                     "border-bottom-right-radius: 60px 90px;\n"
#ifdef GTK3
                                     "box-shadow: 5px 5px 5px 5px rgba(0,0,0,0.4), 2px 2px 0px 1px rgba(0,0,0,0.4) inset;\n"
#endif // GTK3
                                     "}\n", p, a);
  provide_gtk_css (backcol);
  g_free (backcol);
  gchar * str = g_strdup_printf ("preview-%d-%d", p, a);
  gtk_widget_set_name (preview, str);
  g_free (str);
  periodic_search -> preview[1] = create_el_preview (p, a, -1, -1);
  gtk_widget_set_size_request (preview, 400, 275);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, preview, periodic_search -> preview[1], FALSE, FALSE, 0);
  return preview;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void on_element_focus (GtkEventControllerFocus * focus, gpointer data)

  \brief periodic table focus event callback GTK4

  \param focus the GtkEventControllerFocus sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_element_focus (GtkEventControllerFocus * focus, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_element_focus (GtkWidget * widget, GdkEvent * event, gpointer data)

  \brief periodic table focus event callback GTK3

  \param widget the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_element_focus (GtkWidget * widget, GdkEvent * event, gpointer data)
#endif
{
  dint * id = (dint *)data;
  periodic_search -> preview[1] = destroy_this_widget (periodic_search -> preview[1]);
  periodic_search -> preview[1] = create_el_preview (periodic_search -> proj, periodic_search -> action, id -> a, id -> b);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, periodic_search -> preview[0], periodic_search -> preview[1], FALSE, FALSE, 0);
#ifdef GTK3
  return FALSE;
#endif
}
#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void on_element_motion_enter (GtkEventControllerMotion * motion, gdouble x, gdouble y, gpointer data)

  \brief periodic table motion event callback GTK4

  \param motion the GtkEventControllerMotion sending the signal
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_element_motion_enter (GtkEventControllerMotion * motion, gdouble x, gdouble y, gpointer data)
{
  dint * id = (dint *)data;
  periodic_search -> preview[1] = destroy_this_widget (periodic_search -> preview[1]);
  periodic_search -> preview[1] = create_el_preview (periodic_search -> proj, periodic_search -> action, id -> a, id -> b);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, periodic_search -> preview[0], periodic_search -> preview[1], FALSE, FALSE, 0);
}
#endif

/*!
  \fn G_MODULE_EXPORT void run_periodic_table (GtkDialog * info, gint response_id, gpointer data)

  \brief periodic table - running the dialog

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_periodic_table (GtkDialog * info, gint response_id, gpointer data)
{
  destroy_this_dialog (info);
}

/*!
  \fn G_MODULE_EXPORT void get_element (GtkButton * but, gpointer data)

  \brief get element from periodic table button

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void get_element (GtkButton * but, gpointer data)
{
  element = GPOINTER_TO_INT(data);
  run_periodic_table (GTK_DIALOG(get_top_level(GTK_WIDGET(but))), 0, NULL);
  // g_signal_emit_by_name (G_OBJECT(get_top_level(but)), "activate", data);
}

/*!
  \fn GtkWidget * create_css_button (int p, int id, int r, int c)

  \brief create CSS periodic table element button

  \param p the target project id
  \param id is element table (1 / 0)
  \param r row
  \param c column
*/
GtkWidget * create_css_button (int p, int id, int r, int c)
{
  int i;

  switch (r)
  {
    case 8:
      i = 9;
      break;
    case 9:
      i = 10;
      break;
    default:
      switch (c)
      {
        case 1:
          i = 2;
          break;
        case 2:
          i = 3;
          break;
        case 11:
          i = 4;
          break;
        case 12:
          i = 4;
          if (r == 1) i = 7;
          if (r == 6) i = 6;
          break;
        case 17:
          i = 6;
          if (r < 6) i = 8;
          break;

        default:
          if ((r == 0 && c == 0) || (r==1 && (c > 13 && c < 17)) || ((r>1 && r <5) && c == 16))
          {
            i = 0;
          }
          else if (c == 0)
          {
            i = 1;
          }
          else if (c > 2 && c < 8)
          {
            i = 3;
          }
          else if ((c > 7 && c <11) && r < 6)
          {
            i = 3;
          }
          else if (r==6 && ((c > 7 && c < 11) || c > 11))
          {
            i = 6;
          }
          else if (c==13 && (r == 4 || r == 5))
          {
            i = 4;
          }
          else if ((c == 14 || c == 15) && r == 5)
          {
            i = 4;
          }
          else if ((r==1 &&  c==13) || (r==2 &&  (c==14||c==15)) || (r==3 && c==15))
          {
            i = 5;
          }
          else
          {
            i = 7;
          }
          break;
      }
  }
  if (c == 2 && r == 5) i = 9;
  if (c == 2 && r ==6) i = 10;

  GtkWidget * but;
  if (p != -1)
  {
    but = create_button (NULL, IMG_NONE, NULL, -1, -1, GTK_RELIEF_NONE,  G_CALLBACK(get_element), GINT_TO_POINTER(id));
  }
  else
  {
    but = create_button (NULL, IMG_NONE, NULL, -1, -1, GTK_RELIEF_NONE,  NULL, NULL);
  }
  gchar * butcol =  g_strdup_printf ("button#element-%d-%d {\n"
                                     //"  border-radius: 10px;\n"
                                     "  background-color: %s;\n"
                                     "  border-color: black;\n"
                                     "  border-width: 1px;\n"
                                     "  color: black;\n"
                                     "  font-weight: bold;\n"
                                     "}\n"
                                     "button#element-%d-%d:hover {\n"
                                     "  background-color: rgba(255,255,255,0.3);\n"
                                     "  color: %s;\n"
                                     "  font-weight: bold;\n"
                                     "}\n", p, id, ptcolo[i], p, id, ptcolo[i]);
  provide_gtk_css (butcol);
  g_free (butcol);
  gchar * str = g_strdup_printf ("element-%d-%d", p, id);
  gtk_widget_set_name (but, str);
  g_free (str);
  GtkWidget * lab;
  if (id > 0)
  {
    GtkWidget * vbox = create_vbox (BSEP);
    lab = gtk_label_new (periodic_table_info[id].lab);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, lab, FALSE, FALSE, 0);
    gchar * labcol = g_strdup_printf ("label#mass-%d-%d {\n"
                                      "  font-size: 10px;"
                                      "}\n", p, id);
    str = g_strdup_printf ("%d", periodic_table_info[id].Z);
    lab = gtk_label_new (str);
    provide_gtk_css (labcol);
    g_free (labcol);
    str = g_strdup_printf ("mass-%d-%d", p, id);
    gtk_widget_set_name (lab, str);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, lab, FALSE, FALSE, 0);
    add_container_child (CONTAINER_BUT, but, vbox);
    table_p[id-1].a = r;
    table_p[id-1].b = c;
#ifdef GTK3
    g_signal_connect (G_OBJECT (but), "enter-notify-event", G_CALLBACK(on_element_focus), & table_p[id-1]);
#endif
  }
  else
  {
    lab = gtk_label_new (" ");
    gtk_label_align (lab, 0.5, 0.5);
    add_container_child (CONTAINER_BUT, but, lab);
  }
#ifdef GTK4
  GtkEventController * focus = gtk_event_controller_focus_new ();
  g_signal_connect (G_OBJECT (focus), "enter", G_CALLBACK(on_element_focus), & table_p[id-1]);
  gtk_widget_add_controller (but, GTK_EVENT_CONTROLLER (focus));
  GtkEventController * motion = gtk_event_controller_motion_new ();
  g_signal_connect (motion, "enter", G_CALLBACK(on_element_motion_enter), & table_p[id-1]);
  gtk_widget_add_controller (but, GTK_EVENT_CONTROLLER (motion));
#endif
  show_the_widgets (but);
  return but;
}

/*!
  \fn GtkWidget * create_css_group (int p, int i)

  \brief create group CSS

  \param p the target project id
  \param i the group
*/
GtkWidget * create_css_group (int p, int i)
{
  gchar * backcol = g_strdup_printf ("label#group-%d-%d {\n"
                                     "  border-radius: 25px;\n"
                                     "  border-color: black;\n"
                                     "  border-width: 1px;\n"
                                     "  background-color: %s;\n"
                                     "  color: black;\n"
                                     "  font-weight: bold;\n"
                                     "}\n", p, i, ptcolo[i]);
  provide_gtk_css (backcol);
  g_free (backcol);
  GtkWidget * lab = gtk_label_new (" ");
  gtk_widget_set_size_request (lab, 50, 40);
  gtk_label_align (lab, 0.5, 0.5);
  gchar * str = g_strdup_printf ("group-%d-%d", p, i);
  gtk_widget_set_name (lab, str);
  g_free (str);
  show_the_widgets (lab);
  return lab;
}

/*!
  \fn GtkWidget * css_element (int p, int i, int j)

  \brief create CSS for the periodic table elements

  \param p target project id
  \param i row
  \param j column
*/
GtkWidget * css_element (int p, int i, int j)
{
  int k = is_in_table (i, j);
  if (k)
  {
    return  create_css_button (p, k, i, j);
  }
  else if (j == 2 && (i == 5 || i == 6))
  {
    return  create_css_button (p, i-7, i, j);
  }
  else
  {
    return gtk_label_new (" ");
  }
}

/*!
  \fn GtkWidget * periodic_table (int p, int a)

  \brief create periodic table

  \param p target project id
  \param a target atom search id
*/
GtkWidget * periodic_table (int p, int a)
{
  GtkWidget * ptable = gtk_grid_new ();
  gtk_grid_set_row_homogeneous (GTK_GRID (ptable), TRUE);
  gtk_grid_set_column_homogeneous (GTK_GRID (ptable), TRUE);
  gtk_grid_set_row_spacing (GTK_GRID (ptable), 2);
  gtk_grid_set_column_spacing (GTK_GRID (ptable), 2);
  int i, j;

  periodic_search -> preview[0] = el_preview (p, a);
  gtk_grid_attach (GTK_GRID (ptable), periodic_search -> preview[0], 3, 0, 8, 5);
  for (i=0; i<10; i++)
  {
    for (j=0; j<18; j++)
    {
      gtk_grid_attach (GTK_GRID (ptable), css_element(p, i, j), j, i+3, 1, 1);
    }
  }

  gchar * groups[11] = {" <b>Alkali metal</b>", " <b>Alkaline earth metal</b>", " <b>Lanthanide</b>", " <b>Actinide</b>",
                        " <b>Transition metal</b>", " <b>Post-transition metal</b>", " <b>Metalloid</b>", " <b>Polyatomic non-metal</b>",
                        " <b>Diatomic non-metal</b>", " <b>Noble gas</b>", " <b>Unknown</b>"};
  gtk_grid_attach (GTK_GRID (ptable), gtk_label_new (" "), 18, 0, 1, 1);
  int colid[11] = {1, 2, 9, 10, 3, 4, 7, 5, 0, 8, 6};
  for (i=0; i<11; i++)
  {
    gtk_grid_attach (GTK_GRID (ptable), create_css_group (p, colid[i]), 19, 1+i, 1, 1);
    gtk_grid_attach (GTK_GRID (ptable), markup_label(groups[i], 120, -1, 0.0, 0.5), 20, 1+i, 3, 1);
  }

  return ptable;
}

/*!
  \fn int get_atom_id_from_periodic_table (atom_search * asearch)

  \brief get atom Z from selection in the periodic table

  \param asearch target atom search, if any
*/
int get_atom_id_from_periodic_table (atom_search * asearch)
{
  GtkWidget * win = gtk_dialog_new ();
  gchar * str;
  if (asearch != NULL)
  {
    str = g_strdup_printf ("Element selection from the periodic table");
    periodic_search = asearch;
#ifdef GTK3
    if (get_project_by_id (asearch -> proj) ->  modelgl)
    {

      gtk_window_set_attached_to (GTK_WINDOW (win), get_project_by_id (asearch -> proj) ->  modelgl -> win);
    }
    else
    {
      gtk_window_set_attached_to (GTK_WINDOW (win), MainWindow);
    }
#endif
  }
  else
  {
    str = g_strdup_printf ("Periodic table of the elements");
    periodic_search = g_malloc0(sizeof*periodic_search);
    periodic_search -> proj = -1;
    periodic_search -> action = -1;
#ifdef GTK3
    gtk_window_set_attached_to (GTK_WINDOW (win), MainWindow);
#endif
  }
  gtk_window_set_resizable (GTK_WINDOW (win), FALSE);
  gtk_window_set_title (GTK_WINDOW(win), str);
  g_free (str);
  gtk_window_set_modal (GTK_WINDOW (win), TRUE);
  GtkWidget * vbox = dialog_get_content_area (win);
  element = 0;
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, periodic_table (periodic_search -> proj, periodic_search -> action), FALSE, FALSE, 0);

  run_this_gtk_dialog (win, G_CALLBACK(run_periodic_table), NULL);
  if (asearch == NULL) g_free (periodic_search);
  periodic_search = NULL;
  return element;
}
