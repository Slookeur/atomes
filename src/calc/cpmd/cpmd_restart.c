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
* @file cpmd_restart.c
* @short Functions to configure the restart section for the CPMD input file creation assistant
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cpmd_restart.c'
*
* Contains:
*

 - The functions to configure the restart section for the CPMD input file creation assistant

*
* List of functions:

  G_MODULE_EXPORT void update_restart_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void update_restart_check (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void update_restart_check (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void changed_restart_box (GtkComboBox * box, gpointer data);

  GtkWidget * restart_box ();

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "calc.h"
#include "cpmd.h"

extern void print_the_section (int s, int p, GtkTextBuffer * buffer);
extern GtkWidget * cpmd_box (GtkWidget * box, gchar * lab, int v_space, int h_space, int dim);

/*!
  \fn G_MODULE_EXPORT void update_restart_parameter (GtkEntry * res, gpointer data)

  \brief CPMD input file update restart parameter value entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_restart_parameter (GtkEntry * res, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  j = (int)v;
  if (j != tmp_cpmd -> restart[i])
  {
    tmp_cpmd -> restart[i] = j;
  }
  update_entry_int (res, tmp_cpmd -> restart[i]);
  for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

GtkWidget * sace[2];
GtkWidget * trap_box[2];

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void update_restart_check (GtkCheckButton * but, gpointer data)

  \brief CPMD input file restart option toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_restart_check (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void update_restart_check (GtkToggleButton * but, gpointer data)

  \brief CPMD input file restart option toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_restart_check (GtkToggleButton * but, gpointer data)
#endif
{
  int i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  tmp_cpmd -> restart[i] = (j) ? 1 : 0;
  if (i == 3 || i == 6)
  {
    if (tmp_cpmd -> restart[i]) tmp_cpmd -> restart[i] = 10;
    update_entry_int (GTK_ENTRY(sace[i/6]), (tmp_cpmd -> restart[i]) ? TRUE : FALSE);
    widget_set_sensitive (trap_box[i/6], j);
  }
  for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

/*!
  \fn G_MODULE_EXPORT void changed_restart_box (GtkComboBox * box, gpointer data)

  \brief CPMD input file change restart type

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_restart_box (GtkComboBox * box, gpointer data)
{
  tmp_cpmd -> restart[0] = combo_get_active ((GtkWidget *)box);
  int i;
  for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

/*!
  \fn GtkWidget * restart_box ()

  \brief prepare the CPMD input preparation assistant restart widgets
*/
GtkWidget * restart_box ()
{
  int i;
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  // Thermostat type ions
  gtk_widget_set_size_request (vbox, 525, 350);

  // Initialization: Random / Atom / Restart
  hbox = cpmd_box (vbox, "<u>Initialization for the Wavefunction:</u>", 20, 5, 230);

  GtkWidget * box = create_combo ();
  for (i=0; i<3; i++)
  {
    combo_text_append (box, rest_opts[i]);
  }
  combo_set_active (box, tmp_cpmd -> restart[0]);
  g_signal_connect (G_OBJECT (box), "changed", G_CALLBACK(changed_restart_box), NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, box, FALSE, FALSE, 5);

  // Save restart every
  hbox = cpmd_box (vbox, "Save the information required for a restart every:", 5, 5, 340);
  GtkWidget * widg = create_entry (G_CALLBACK(update_restart_parameter), 100, 15, FALSE, GINT_TO_POINTER(1));
  update_entry_int (GTK_ENTRY(widg), tmp_cpmd -> restart[1]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new("steps"), FALSE, FALSE, 10);

  // Num of restart:
  hbox = cpmd_box (vbox, "Number of restart file(s): ", 0, 5, 340);
  widg = create_entry (G_CALLBACK(update_restart_parameter), 100, 15, FALSE, GINT_TO_POINTER(2));
  update_entry_int (GTK_ENTRY(widg), tmp_cpmd -> restart[2]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 0);

  // Trajectory:
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                      check_button("Create a trajectory", -1, -1, tmp_cpmd -> restart[3], G_CALLBACK(update_restart_check), GINT_TO_POINTER(3)),
                      FALSE, FALSE, 20);
  trap_box[0] = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,trap_box[0], FALSE, FALSE, 0);

  hbox = cpmd_box (trap_box[0], "Save the atomic coordinates every: ", 0, 25, 300);
  sace[0] = create_entry (G_CALLBACK(update_restart_parameter), 100, 15, FALSE, GINT_TO_POINTER(3));
  update_entry_int (GTK_ENTRY(sace[0]), tmp_cpmd -> restart[3]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sace[0], FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new("steps"), FALSE, FALSE, 10);
  hbox = cpmd_box (trap_box[0], "Write XYZ format", 0, 25, 300);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                      check_button(NULL, -1, -1, tmp_cpmd -> restart[4], G_CALLBACK(update_restart_check), GINT_TO_POINTER(4)),
                      FALSE, FALSE, 0);
  hbox = cpmd_box (trap_box[0], "Write atomic forces", 0, 25, 300);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                      check_button(NULL, -1, -1, tmp_cpmd -> restart[5], G_CALLBACK(update_restart_check), GINT_TO_POINTER(5)),
                      FALSE, FALSE, 0);
  widget_set_sensitive (trap_box[0], tmp_cpmd -> restart[3]);

  // Print information
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                      check_button("Print extra data in the global output file", -1, -1, tmp_cpmd -> restart[6], G_CALLBACK(update_restart_check), GINT_TO_POINTER(6)),
                      FALSE, FALSE, 20);
  trap_box[1] = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, trap_box[1], FALSE, FALSE, 0);
  hbox = cpmd_box (trap_box[1], "Save the extra information every: ", 0, 25, 300);
  sace[1] = create_entry (G_CALLBACK(update_restart_parameter), 100, 15, FALSE, GINT_TO_POINTER(6));
  update_entry_int (GTK_ENTRY(sace[1]), tmp_cpmd -> restart[6]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sace[1], FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new("steps"), FALSE, FALSE, 10);
  hbox = cpmd_box (trap_box[1], "Select the information to print:", 5, 25, 300);
  widget_set_sensitive (trap_box[1], tmp_cpmd -> restart[6]);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, trap_box[1], hbox, FALSE, FALSE, 0);
  gchar * print_str[3]={"Information", "Coordinates", "Forces"};
  for (i=0; i<3; i++)
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                        check_button(print_str[i], -1, -1, tmp_cpmd -> restart[i+7], G_CALLBACK(update_restart_check), GINT_TO_POINTER(i+7)),
                        FALSE, FALSE, 5);
  }
  return vbox;
}
