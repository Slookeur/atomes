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
* @file calc_menu.c
* @short Creation of the calculation dialogs
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'calc_menu.c'
*
* Contains:
*

 - The creation of the calculation dialogs

*
* List of functions:

  gboolean test_gr (int gr);
  gboolean test_sq (int sq);
  gboolean test_bonds ();
  gboolean test_rings ();
  gboolean test_msd ();
  gboolean test_sph ();

  void calc_sph (GtkWidget * vbox);
  void calc_msd (GtkWidget * vbox);
  void calc_rings (GtkWidget * vbox);
  void calc_bonds (GtkWidget * vbox);
  void calc_gr_sq (GtkWidget * box, int id);

  G_MODULE_EXPORT void set_max (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void set_delta (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void combox_tunit_changed (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_numa (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void combox_rings_changed (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void toggle_rings (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void toggle_rings (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void run_toggle_bond (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_toggle_bond (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void toggle_bond (GtkCheckButton * Button, gpointer data);
  G_MODULE_EXPORT void toggle_bond (GtkToggleButton * Button, gpointer data);
  G_MODULE_EXPORT void expand_opt (GtkWidget * exp, gpointer data);
  G_MODULE_EXPORT void set_advanced_sq (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void set_sfact (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void on_show_curve_toolbox (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void on_smoother_released (GtkButton * button, gpointer data);
  G_MODULE_EXPORT void run_on_calc_activate (GtkDialog * dial, gint response_id, gpointer data);
  G_MODULE_EXPORT void on_calc_activate (GtkWidget * widg, gpointer data);

  GtkWidget * calc_window (int i);
  GtkWidget * combox_rings (gchar * str, int num, gchar * list_item[num], int id);
  GtkWidget * hbox_note (int i, double val);

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "preferences.h"
#include "bind.h"
#include "project.h"
#include "workspace.h"

extern G_MODULE_EXPORT void on_calc_gr_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_gq_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_sq_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_sk_released (GtkWidget * widg, gpointer data);
extern gboolean toggled_rings;
extern G_MODULE_EXPORT void on_calc_rings_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_chains_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_msd_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_sph_released (GtkWidget * widg, gpointer data);
extern gchar * calc_img[NCALCS-2];

GtkWidget * calc_win = NULL;
GtkWidget * ba_entry[2];
int search_type;

/*!
  \fn GtkWidget * calc_window (int i)

  \brief create a calculation window

  \param i the calculation id
*/
GtkWidget * calc_window (int i)
{
  calc_dialog = dialog_cancel_apply (calc_name[i], MainWindow, FALSE);
#ifndef GTK4
  /* GtkWidget * apply_button =
  button_set_image (GTK_BUTTON(apply_button), calc_name[i], IMG_FILE, (gpointer)calc_img[i]);
  show_the_widgets (apply_button); */
#endif
  return calc_dialog;
}

/*!
  \fn G_MODULE_EXPORT void set_max (GtkEntry * entry, gpointer data)

  \brief set a maximum value

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_max (GtkEntry * entry, gpointer data)
{
  int c = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  if (v > 0)
  {
    if (active_project -> max[c] != v)
    {
      active_project -> max[c] = v;
      // Max has changed do something !?
    }
  }
  update_entry_double (entry, active_project -> max[c]);
}

GtkWidget * rings_box[2];

/*!
  \fn G_MODULE_EXPORT void set_delta (GtkEntry * entry, gpointer data)

  \brief set the number of delta between data points

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_delta (GtkEntry * entry, gpointer data)
{
  int c = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  int i, j, k;
  i = (int)v;
  if (c == RI)
  {
    if (preferences)
    {
      k = tmp_rsparam[2];
    }
    else
    {
      j = combo_get_active (rings_box[0]);
      k = active_project -> rsparam[j][1];
    }
  }
  else if (c == CH)
  {
    k = (preferences) ? tmp_csparam[1] : active_project -> csparam[5];
  }
  else if (c > -1 && ! preferences)
  {
    k = active_project -> num_delta[c];
  }
  if (c < 0 && ! preferences)
  {
    if (v > 0.0)
    {
      if (active_project -> delta[-c] != v)
      {
        active_project -> delta[-c] = v;
      }
    }
  }
  else if (i > 0 && ! preferences)
  {
    if (c == RI)
    {
      if (active_project -> rsparam[j][1] != i)
      {
        active_project -> rsparam[j][1] = i;
      }
      k = active_project -> rsparam[j][1] = i;
    }
    else if (c == CH)
    {
      if (active_project -> csparam[5] != i)
      {
        active_project -> csparam[5] = i;
      }
      k = active_project -> csparam[5] = i;
    }
    else
    {
      if (active_project -> num_delta[c] != i)
      {
        active_project -> num_delta[c] = i;
      }
      k = active_project -> num_delta[c];
    }
  }
  if (c < 0)
  {
    update_entry_double (entry, active_project -> delta[-c]);
  }
  else
  {
    update_entry_int (entry, k);
  }
}

/*!
  \fn G_MODULE_EXPORT void combox_tunit_changed (GtkComboBox * box, gpointer data)

  \brief change time units

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void combox_tunit_changed (GtkComboBox * box, gpointer data)
{
  active_project -> tunit = combo_get_active ((GtkWidget *)box);
}

/*!
  \fn void calc_sph (GtkWidget * vbox)

  \brief creation of the spherical harmonics calculation widgets

  \param vbox GtkWidget that will receive the data
*/
void calc_sph (GtkWidget * vbox)
{
  GtkWidget * hbox;
  GtkWidget * entry;
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  gchar * str = "Maximum <b><i>l</i></b>, <i>l<sub>max</sub></i> in [2-40]";
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,  markup_label (str, 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
  entry = create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, (gpointer)GINT_TO_POINTER(SP));
  update_entry_int (GTK_ENTRY(entry), active_project -> num_delta[SP]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
}

/*!
  \fn void calc_msd (GtkWidget * vbox)

  \brief creation of the MSD calculation widgets

  \param vbox GtkWidget that will receive the data
*/
void calc_msd (GtkWidget * vbox)
{
  int i, j;
  gchar * val_b[3]={"Number of configurations:",
                    "\tTime step &#x3b4;t used during the dynamics:",
                    "\tNumber of step(s) between each configuration:"};
  GtkWidget * hbox;
  GtkWidget * entry;
  hbox = create_hbox (15);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new (val_b[0]), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (g_strdup_printf ("<b>%d</b>",active_project -> steps), -1, 50, 0.5, 0.5), FALSE, FALSE, 0);
  for (i=1; i<3; i++)
  {
    hbox = create_hbox (15);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (val_b[i], 350, -1, 0.0, 0.5), FALSE, FALSE, 0);
    if (i == 1)
    {
      entry = create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, (gpointer)GINT_TO_POINTER(-MS));
      update_entry_double (GTK_ENTRY(entry), active_project -> delta[MS]);
    }
    else
    {
      entry = create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, (gpointer)GINT_TO_POINTER(MS));
      update_entry_int (GTK_ENTRY(entry), active_project -> num_delta[MS]);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
    if (i == 1)
    {
      GtkWidget * tcombo = create_combo ();
      for (j=0; j<5 ; j++) combo_text_append (tcombo, untime[j]);
      combo_set_active (tcombo, active_project -> tunit);
      g_signal_connect(G_OBJECT(tcombo), "changed", G_CALLBACK(combox_tunit_changed), NULL);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, tcombo, FALSE, FALSE, 0);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_numa (GtkEntry * entry, gpointer data)

  \brief set the rings/chains statistics parameter NUMA

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_numa (GtkEntry * entry, gpointer data)
{
  const gchar * m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  int i, j;
  i = (int)v;
  if (i > 0)
  {
    if (! search_type)
    {
      if (preferences)
      {
        tmp_rsparam[3] = i;
      }
      else
      {
        if (active_project -> rsearch[1] != i) active_project -> rsearch[1] = i;
        j = active_project -> rsearch[1];
      }
    }
    else
    {
      if (preferences)
      {
        tmp_csparam[2] = i;
      }
      else
      {
        if (active_project -> csearch != i) active_project -> csearch = i;
        j = active_project -> csearch;
      }
    }
  }
  else
  {
    if (preferences)
    {
      j = (search_type) ? tmp_csparam[2] : tmp_rsparam[3];
    }
    else
    {
      j = (search_type) ? active_project -> csearch : active_project -> rsearch[1];
    }
  }
  update_entry_int (entry, j);
}

/*!
  \fn GtkWidget * combox_rings (gchar * str, int num, gchar * list_item[num], int id)

  \brief create a combo box for the ring statistics calculation

  \param str label of the combo box
  \param num number of values to insert in the combo box
  \param list_item text data to insert in the combo boc
  \param id id of the box to create
*/
GtkWidget * combox_rings (gchar * str, int num, gchar * list_item[num], int id)
{
  int i;
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 250, -1, 0.0, 0.5), FALSE, FALSE, 0);
  // GtkWidget * fixed = gtk_fixed_new ();
  rings_box[id] = create_combo ();
  gtk_widget_set_size_request (rings_box[id], 180, -1);
  for (i=0; i<num; i++) combo_text_append (rings_box[id], list_item[i]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, rings_box[id], FALSE, FALSE, 10);

  //gtk_fixed_put (GTK_FIXED(fixed), rings_box[id], -1, 5);

  return hbox;
}

 GtkWidget * rings_entry[2];
 GtkWidget * rings_check[4];

/*!
  \fn G_MODULE_EXPORT void combox_rings_changed (GtkComboBox * box, gpointer data)

  \brief change ring statistics calculation parameter

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void combox_rings_changed (GtkComboBox * box, gpointer data)
{
  int i, j;
  int id = GPOINTER_TO_INT(data);
  if (id == 0)
  {
    if (preferences)
    {
      tmp_rsparam[0] = combo_get_active ((GtkWidget *)box);
    }
    else
    {
      active_project -> rsearch[0] = combo_get_active ((GtkWidget *)box);
      widget_set_sensitive (rings_box[1], (active_project -> rsearch[0]<0) ? 0 : 1);
      combo_set_active (rings_box[1], active_project -> rsparam[active_project -> rsearch[0]][0]);
      for (i=0; i<2; i++) widget_set_sensitive (rings_entry[i], (active_project -> rsearch[0]<0) ? 0 : 1);
      for (i=0; i<3; i++) widget_set_sensitive (rings_check[i], (active_project -> rsearch[0]<0) ? 0 : 1);
      update_entry_int (GTK_ENTRY(rings_entry[0]), active_project -> rsparam[active_project -> rsearch[0]][1]);
      i = combo_get_active (rings_box[0]);
      for (j=0; j<3; j++)
      {
        button_set_status (rings_check[j], active_project -> rsparam[i][j+2]);
      }
    }
  }
  else
  {
    if (search_type)
    {
      if (preferences)
      {
        tmp_csparam[0] = combo_get_active ((GtkWidget *)box);
      }
      else
      {
        active_project -> csparam[0] = combo_get_active ((GtkWidget *)box);
      }
    }
    else
    {
      if (preferences)
      {
        tmp_rsparam[1] = combo_get_active ((GtkWidget *)box);
      }
      else
      {
        i = combo_get_active (rings_box[0]);
        active_project -> rsparam[i][0] = combo_get_active ((GtkWidget *)box);
      }
    }
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggle_rings (GtkCheckButton * but, gpointer data)

  \brief toggle a rings statistics calculation parameter

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_rings (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggle_rings (GtkToggleButton * but, gpointer data)

  \brief toggle a rings statistics calculation parameter

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_rings (GtkToggleButton * but, gpointer data)
#endif
{
  int oid = GPOINTER_TO_INT(data);
  int i;
  gboolean status;
  status = button_get_status ((GtkWidget *)but);
  toggled_rings = TRUE;
  switch (search_type)
  {
    case 0:
      i = combo_get_active (rings_box[0]);
      if (preferences)
      {
        tmp_rsparam[oid+4] = status;
      }
      else
      {
        active_project -> rsparam[i][oid+2] = status;
      }
      break;
    case 1:
      if (preferences)
      {
        tmp_csparam[oid+3] = status;
      }
      else
      {
        active_project -> csparam[oid+1] = status;
      }
      if (oid == 0 && status)
      {
        for (i=1; i<4; i++) button_set_status(rings_check[i], ! status);
      }
      else if (oid == 1 && status)
      {
        button_set_status(rings_check[0], ! status);
      }
      else if (oid == 2 && status)
      {
        button_set_status(rings_check[1], ! status);
      }
      break;
  }
}

/*!
  \fn void calc_rings (GtkWidget * vbox)

  \brief creation of the rings statistics calculation widgets

  \param vbox GtkWidget that will receive the data
*/
void calc_rings (GtkWidget * vbox)
{
  gchar * defs[6]={"All rings (No rules)",
                   "King's [1, 3]",
                   "Guttman's [2]",
                   "Primitives [4, 5, 6]",
                   "Strongs [4, 5]"};
  gchar * val_a[2]={"Definition of ring to be used: ", "Atom(s) to initiate the search from: "};
  gchar * val_b[2][2]={{"<i><b>n</b><sub>max</sub></i> = maximum size for a ring <sup>*</sup>",
                        "Maximum number of rings of size <i><b>n</b></i> per MD step <sup>**</sup>"},
                       {"<i><b>n</b><sub>max</sub></i> = maximum size for a chain <sup>*</sup>",
                        "Maximum number of chains of size <i><b>n</b></i> per MD step <sup>**</sup>"}};
  gchar * val_c[2][4]={{"Only search for ABAB rings", "No homopolar bonds in the rings (A-A, B-B ...) <sup>***</sup>", "No homopolar bonds in the connectivity matrix", " "},
                       {"Only search for AAAA chains", "Only search for ABAB chains",
                        "No homopolar bonds in the chains (A-A, B-B ...) <sup>***</sup>", "Only search for 1-(2)<sub>n</sub>-1 coordinated atom chains, ie. isolated chains."}};
  gchar * start[3]={"<sup>*</sup>", "<sup>**</sup>", "<sup>***</sup>"};
  gchar * val_d[3]={"<i><b>n</b><sub>max</sub></i> in total number of nodes (or atoms)",
                    "value used for memory allocation = f(<i><b>n</b><sub>max</sub></i>, system studied)",
                    "but homopolar bonds can shorten the rings"};
  gchar * val_e={"\n<sub>[1] S. V. King. <i>Nature</i>, <b>213</b>:1112 (1967).</sub>\n"
                 "<sub>[2] L. Guttman. <i>J. Non-Cryst. Solids.</i>, <b>116</b>:145-147 (1990).</sub>\n"
                 "<sub>[3] D. S. Franzblau. <i>Phys. Rev. B</i>, <b>44</b>(10):4925-4930 (1991).</sub>\n"
                 "<sub>[4] K. Goetzke and H. J. Klein. <i>J. Non-Cryst. Solids.</i>, <b>127</b>:215-220 (1991).</sub>\n"
                 "<sub>[5] X. Yuan and A. N. Cormack. <i>Comp. Mat. Sci.</i>, <b>24</b>:343-360 (2002).</sub>\n"
                 "<sub>[6] F. Wooten. <i>Acta Cryst. A</i>, <b>58</b>(4):346-351 (2002).</sub>"};
  gchar * list_node[(preferences) ? 2 : active_project -> nspec+1];
  int i, j, k;
  toggled_rings = FALSE;
  if (! search_type)  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, combox_rings (val_a[0], 5, defs, 0), FALSE, FALSE, 5);

  list_node[0] = g_strdup_printf ("All");
  if (preferences)
  {
    list_node[1] = g_strdup_printf ("Chemical species");
  }
  else
  {
    for (i=0; i<active_project -> nspec; i++) list_node[i+1] = g_strdup_printf ("%s", active_chem -> label[i]);
  }

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, combox_rings (val_a[1], (preferences) ? 2 : active_project -> nspec+1, list_node, 1), FALSE, FALSE, 5);

  j = (preferences) ? tmp_rsparam[0] : active_project -> rsearch[0];
  k = RI + search_type;
  GtkWidget * hbox;
  for (i=0; i<2; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (val_b[search_type][i], 400, -1, 0.0, 0.5), FALSE, FALSE, 0);
    if (i == 0)
    {
      rings_entry[i] = create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, GINT_TO_POINTER(k));
      if (preferences)
      {
        update_entry_int (GTK_ENTRY(rings_entry[i]), (search_type) ? tmp_csparam[1] : tmp_rsparam[2]);
      }
      else
      {
        update_entry_int (GTK_ENTRY(rings_entry[i]), (search_type) ? active_project -> csparam[5] : active_project -> rsparam[j][1]);
      }
    }
    else
    {
      rings_entry[i] = create_entry (G_CALLBACK(set_numa), 100, 15, FALSE, NULL);
      if (preferences)
      {
        update_entry_int (GTK_ENTRY(rings_entry[i]), (search_type) ? tmp_csparam[2] : tmp_rsparam[3]);
      }
      else
      {
        update_entry_int (GTK_ENTRY(rings_entry[i]), (search_type) ? active_project -> csearch : active_project -> rsearch[1]);
      }
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, rings_entry[i], FALSE, FALSE, 0);
    if (! search_type && j < 0 && ! preferences) widget_set_sensitive (rings_entry[i], 0);
  }
  gboolean status;
  for (i=0; i<3+search_type; i++)
  {
    if (preferences)
    {
      status = (search_type) ? tmp_csparam[i+3] : tmp_rsparam[i+4];
    }
    else
    {
      status =  (j < 0) ? 0 : (search_type) ? active_project -> csparam[i+1] : active_project -> rsparam[j][i+2];
    }
    rings_check[i] = check_button (val_c[search_type][i], -1, 40, status, G_CALLBACK(toggle_rings), GINT_TO_POINTER(i));
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, rings_check[i], FALSE, FALSE, 0);
    if (! preferences)
    {
      if (active_project -> nspec == 1) widget_set_sensitive (rings_check[i], 0);
      if (! search_type && j < 0) widget_set_sensitive (rings_check[i], 0);
    }
  }
  for (i=0; i<3; i++)
  {
    append_comments (vbox, start[i], val_d[i]);
  }
  if (! search_type)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label (val_e, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    i = 0;
    g_signal_connect(G_OBJECT(rings_box[0]), "changed", G_CALLBACK(combox_rings_changed), GINT_TO_POINTER(0));
    combo_set_active (rings_box[0], (preferences) ? tmp_rsparam[0] : active_project -> rsearch[0]);
    widget_set_sensitive (rings_box[1], (preferences) ? 1 : (active_project -> rsearch[0]<0) ? 0 : 1);
  }
  i = 1;
  g_signal_connect(G_OBJECT(rings_box[1]), "changed", G_CALLBACK(combox_rings_changed), GINT_TO_POINTER(i));
  if (preferences)
  {
    combo_set_active (rings_box[1], (search_type) ? tmp_csparam[0] : tmp_rsparam[1]);
  }
  else
  {
    combo_set_active (rings_box[1], (search_type) ? active_project -> csparam[0] : active_project -> rsparam[j][0]);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_toggle_bond (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief bond properties detailed saving: run the dialog

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_toggle_bond (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_toggle_bond (GtkDialog * info, gint response_id, gpointer data)

  \brief bond properties detailed saving: run the dialog

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_toggle_bond (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    active_project -> bondfile = file_chooser_get_file_name (chooser);
  }
  else
  {
    button_set_status (data, FALSE);
  }
#ifdef GTK4
  destroy_this_native_dialog (info);
#else
  destroy_this_dialog (info);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggle_bond (GtkCheckButton * Button, gpointer data)

  \brief bond properties detailed saving and prepare the dialog

  \param Button the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_bond (GtkCheckButton * Button, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggle_bond (GtkToggleButton * Button, gpointer data)

  \brief activate bond properties detailed saving and prepare the dialog

  \param Button the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_bond (GtkToggleButton * Button, gpointer data)
#endif
{
  int i = GPOINTER_TO_INT (data);
  gboolean status = button_get_status ((GtkWidget *)Button);
  if (i < 3)
  {
    active_project -> runc[i] = status;
    if (i < 2) widget_set_sensitive (ba_entry[i], status);
    if (i == 2) frag_update = mol_update = status;
  }
  else
  {
    if (status)
    {
      // To add = do not annoy me with that again !
      show_info ("Activate this option and the result of the nearest neighbors\n"
                 "analysis will be saved during the search in a file that you may\n"
                 "use afterwards.\n", 0, MainWindow);
#ifdef GTK4
      GtkFileChooserNative * info;
#else
      GtkWidget * info;
#endif
      info = create_file_chooser ("Save neighbors analysis in file",
                                  GTK_WINDOW(MainWindow),
                                  GTK_FILE_CHOOSER_ACTION_SAVE,
                                  "Save");
      GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
#ifdef GTK3
      gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
      file_chooser_set_current_folder (chooser);
      gtk_file_chooser_set_current_name (chooser, "neighbors.dat");
#ifdef GTK4
      run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_toggle_bond), Button);
#else
      run_this_gtk_dialog (info, G_CALLBACK(run_toggle_bond), Button);
#endif
    }
    else
    {
      active_project -> bondfile = NULL;
    }
  }
}

/*!
  \fn gboolean test_gr (int gr)

  \brief is it safe to compute g(r) ?

  \param gr type of g(r): real space (GR) or FFT (GK)
*/
gboolean test_gr (int gr)
{
  if (active_project -> num_delta[gr] < 2)
  {
    show_warning ("You must specify a number of &#x3b4;r >= 2\n"
                  "to discretize the real space between 0.0 and D<sub>max</sub>\n", calc_win);
    return FALSE;
  }
  else if (gr == GK && (active_project -> max[gr] > active_project -> max[SK] || active_project -> max[gr] <= active_project -> min[SK]))
  {
    show_warning ("You must specify a maximum wave vector Q<sub>max</sub>[FFT]\n"
                  "for the FFT, with Q<sub>min</sub> < Q<sub>max</sub>[FFT] <= Q<sub>max</sub>", calc_win);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn gboolean test_sq (int sq)

  \brief is it safe to compute s(q) ?

  \param sq type of s(q): FFT (1) or direct (SK)
*/
gboolean test_sq (int sq)
{
  if (active_project -> max[sq] <= active_project -> min[sq])
  {
    show_warning ("You must specify a maximum wave vector Q<sub>max</sub>\n"
                  "note that Q<sub>max</sub> must be > Q<sub>min</sub>", calc_win);
    return FALSE;
  }
  else if (active_project -> num_delta[sq] < 2)
  {
    show_warning ("You must specify a number of &#x3b4;q >= 2\n"
                  "to discretize the reciprocal space between 0.0 and Q<sub>max</sub>\n", calc_win);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn gboolean test_bonds ()

  \brief is it safe to compute bond properties ?
*/
gboolean test_bonds ()
{
  if (active_project -> runc[0] && active_project -> num_delta[BD] < 2)
  {
    show_warning ("You must specify a number of &#x3b4;r >= 2\n"
                  "to discretize the real space between\n"
                  "the shortest and the highest inter-atomic distances", calc_win);
    return FALSE;
  }
  else if (active_project -> runc[1] && active_project -> num_delta[AN] < 2)
  {
    show_warning ("You must specify a number of &#x3b4;&#x3b8; >= 2\n"
                  "to discretize the angular space between 0 and 180°", calc_win);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn gboolean test_rings ()

  \brief is it safe to compute ring statistics ?
*/
gboolean test_rings ()
{
  int i, j;
  i = (search_type) ? active_project -> csparam[5] : active_project -> rsparam[active_project -> rsearch[0]][1];
  j = (search_type) ? active_project -> csearch : active_project -> rsearch[1];
  gchar * sobj[2]={"ring", "chain"};
  gchar * str;
  if (i == 0)
  {
    str = g_strdup_printf ("You must specify a maximum %s size > 1 for the search", sobj[search_type]);
    show_warning (str, calc_win);
    g_free (str);
    return FALSE;
  }
  if (j == 0)
  {
    str = g_strdup_printf ("You must specify a number of %ss per size 'n' and per node\n"
                           "this value is used when allocating the memory to store the results\n"
                           "and depends on both the maximum %s size used in the search\n"
                           "and the system studied.\n"
                           "We recommend a value at least equal to 100.\n"
                           "If it appears that the value given is not big enough\n"
                           "(ex: more than 100 different %ss of size 'n' for a single node)\n"
                           "then the search will failed but the program will propose\n"
                           "you to initiate a new search using a higher value",
                           sobj[search_type], sobj[search_type], sobj[search_type]);
    show_warning (str, calc_win);
    g_free (str);
    return FALSE;
  }
  return TRUE;
}

/*!
  \fn gboolean test_msd ()

  \brief is it safe to compute MSD ?
*/
gboolean test_msd ()
{
  if(active_project -> steps > 1)
  {
    if (active_project -> delta[MS] == 0.0)
    {
      show_warning ("You must specify the time step &#x3b4;t\n"
                    "used to integrate the Newton's equations\n"
                    "of motion during the molecular dynamics\n", calc_win);
      return FALSE;
    }
    if (active_project -> tunit < 0)
    {
      show_warning ("You must specify the time unit\n"
                    "used to integrate the Newton's equations\n"
                    "of motion during the molecular dynamics\n", calc_win);
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    if (active_project -> num_delta[MS] < 1)
    {
      show_warning ("You must specify the number of steps\n"
                    "between each of the %d configurations\n"
                    "found for the molecular dynamics\n", calc_win);
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
}

/*!
  \fn gboolean test_sph ()

  \brief is it safe to compute spherical harmonics ?
*/
gboolean test_sph ()
{
  if (active_project -> num_delta[SP] < 2 || active_project -> num_delta[SP] > 40)
  {
    show_warning ("You must specify a number <i>l<sub>max</sub></i> in [2-40]", calc_win);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn void calc_bonds (GtkWidget * vbox)

  \brief creation of the bond calculation widgets

  \param vbox GtkWidget that will receive the data
*/
void calc_bonds (GtkWidget * vbox)
{
  gchar * val_a[2]={"Number of &#x3b4;r [D<sub>ij</sub>min-D<sub>ij</sub>max]",
                    "Number of &#x3b4;&#x3b8; [0-180°]"};
  gchar * val_b[4]={"First coordination sphere properties",
                    "Bond and dihedral angles distribution",
                    "Search for molecules and isolated fragments",
                    "Output detailed results in text file"};

  GtkWidget * hbox;
  int i;
  for (i=0; i<3; i++)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                         check_button (val_b[i], -1, 40, active_project -> runc[i], G_CALLBACK(toggle_bond), (gpointer)GINT_TO_POINTER(i)),
                         FALSE, FALSE, 0);
    if (i < 2)
    {
      hbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (val_a[i], 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
      ba_entry[i] = create_entry (G_CALLBACK(set_delta), 150, 15, FALSE, (gpointer)GINT_TO_POINTER(BD+i));
      update_entry_int (GTK_ENTRY(ba_entry[i]), active_project -> num_delta[BD+i]);
      widget_set_sensitive (ba_entry[i], active_project -> runc[i]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ba_entry[i], FALSE, FALSE, 0);
    }
  }
  GtkWidget * checkbd = check_button (val_b[i], -1, 40, FALSE, G_CALLBACK(toggle_bond), (gpointer)GINT_TO_POINTER(i));
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, checkbd, FALSE, FALSE, 0);
  widget_set_sensitive (checkbd, 1);
}

/*!
  \fn GtkWidget * hbox_note (int i, double val)

  \brief foot note message box with some parameters

  \param i message id
  \param val value to display
*/
GtkWidget * hbox_note (int i, double val)
{
  gchar * note[3] = {"D<sub>max</sub> = ", "Q<sub>min</sub> = ", "Q<sub>max</sub> = "};
  gchar * unit[3] = {" &#xC5;", " &#xC5;<sup>-1</sup>", " &#xC5;<sup>-1</sup>"};
  gchar * str;
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (note[i], 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
  str = g_strdup_printf ("%f", val);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 100, -1, 1.0, 0.5), FALSE, FALSE, 0);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (unit[i], 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
  return hbox;
}

GtkWidget * avbox;
GtkWidget * smbox;
int avsize;

/*!
  \fn G_MODULE_EXPORT void expand_opt (GtkWidget * exp, gpointer data)

  \brief open expander actions

  \param exp the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void expand_opt (GtkWidget * exp, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  GtkExpander * expander = GTK_EXPANDER (exp);
  GtkWidget * wind = get_top_level (exp);
  gtk_window_set_resizable (GTK_WINDOW (wind), TRUE);
  if (gtk_expander_get_expanded (expander))
  {
    if (i == 1)
    {
      hide_the_widgets (avbox);
    }
    else
    {
      hide_the_widgets (smbox);
    }
  }
  else
  {
    if (i == 1)
    {
      show_the_widgets (avbox);

    }
    else
    {
      show_the_widgets (smbox);
    }
  }
  gtk_widget_set_size_request (exp, -1, -1);
  gtk_window_set_resizable (GTK_WINDOW (wind), FALSE);
}

/*!
  \fn G_MODULE_EXPORT void set_advanced_sq (GtkEntry * entry, gpointer data)

  \brief set a s(k) calculation parameter

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_advanced_sq (GtkEntry * entry, gpointer data)
{
  int c = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  if (v != active_project -> sk_advanced[c])
  {
    if (c == 0 && (v < 0.0 || v > 1.0))
    {
      show_warning ("You must specify a probability between 0.0 and 1.0", calc_win);
    }
    else if (c == 1 && (v < active_project -> min[SK] || v > active_project -> max[SK]))
    {
      show_warning ("Q<sub>lim</sub> must be &#8805; Q<sub>min</sub> and &#8804; Q<sub>max</sub>", calc_win);
    }
    else
    {
      active_project -> sk_advanced[c] = v;
    }
  }
  update_entry_double (entry, active_project -> sk_advanced[c]);
}

/*!
  \fn G_MODULE_EXPORT void set_sfact (GtkEntry * entry, gpointer data)

  \brief set the Gaussian smoothing factor

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_sfact (GtkEntry * entry, gpointer data)
{
  const gchar * m;
  int i = GPOINTER_TO_INT(data);
  m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  if (v <= 0.0 || v >= 1.0)
  {
    show_warning ("The smoothing factor must be between 0.0 and 1.0", calc_win);
  }
  else
  {
    active_project -> fact[i] = v;
  }
  update_entry_double (entry, active_project -> fact[i]);
}

/*!
  \fn G_MODULE_EXPORT void on_show_curve_toolbox (GtkWidget * widg, gpointer data)

  \brief show the curve toolboxes

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_show_curve_toolbox (GtkWidget * widg, gpointer data)
{
  if (! is_the_widget_visible (curvetoolbox))
  {
    show_the_widgets (curvetoolbox);
  }
  else
  {
    hide_the_widgets (curvetoolbox);
  }
}

/*!
  \fn G_MODULE_EXPORT void on_smoother_released (GtkButton * button, gpointer data)

  \brief smooth g(r), s(q), s(k) or g(k)

  \param button the button sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_smoother_released (GtkButton * button, gpointer data)
{
  int i, k, l, m;

  l = GPOINTER_TO_INT(data);
  if (active_project -> visok[l])
  {
    if (l == 2)
    {
      xsk = duplicate_double(active_project -> curves[l][0] -> ndata, active_project -> curves[l][0] -> data[0]);
    }
    i = 1;
    smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                      active_project -> curves[l][i-1] -> data[1],
                      & active_project -> fact[l],
                      & i,
                      & active_project -> curves[l][i-1] -> ndata,
                      & l);
    i = i+2;
    smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                      active_project -> curves[l][i-1] -> data[1],
                      & active_project -> fact[l],
                      & i,
                      & active_project -> curves[l][i-1] -> ndata,
                      & l);
    i = i+2;
    smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                      active_project -> curves[l][i-1] -> data[1],
                      & active_project -> fact[l],
                      & i,
                      & active_project -> curves[l][i-1] -> ndata,
                      & l);
    i = i+2;
    smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                      active_project -> curves[l][i-1] -> data[1],
                      & active_project -> fact[l],
                      & i,
                      & active_project -> curves[l][i-1] -> ndata,
                      & l);
    if (l == 0 || l == 3)
    {
      i = i+2;
      smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                        active_project -> curves[l][i-1] -> data[1],
                        & active_project -> fact[l],
                        & i,
                        & active_project -> curves[l][i-1] -> ndata,
                        & l);
      i = i+2;
      smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                        active_project -> curves[l][i-1] -> data[1],
                        & active_project -> fact[l],
                        & i,
                        & active_project -> curves[l][i-1] -> ndata,
                        & l);
      i = i+2;
      smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                        active_project -> curves[l][i-1] -> data[1],
                        & active_project -> fact[l],
                        & i,
                        & active_project -> curves[l][i-1] -> ndata,
                        & l);
      i = i+2;
      smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                        active_project -> curves[l][i-1] -> data[1],
                        & active_project -> fact[l],
                        & i,
                        & active_project -> curves[l][i-1] -> ndata,
                        & l);
    }
    for (k=0 ; k<active_project -> nspec ; k++)
    {
      for (m=0 ; m<active_project -> nspec ; m++)
      {
        i = i+2;
        smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                          active_project -> curves[l][i-1] -> data[1],
                          & active_project -> fact[l],
                          & i,
                          & active_project -> curves[l][i-1] -> ndata,
                          & l);
        if (l == 0 || l == 3)
        {
          i = i+2;
          smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                            active_project -> curves[l][i-1] -> data[1],
                            & active_project -> fact[l],
                            & i,
                            & active_project -> curves[l][i-1] -> ndata,
                            & l);
          i = i+1;
        }
      }
    }
    if (l == 1 || l == 2)
    {
      for (k=0 ; k<active_project -> nspec ; k++)
      {
        for (m=0 ; m<active_project -> nspec ; m++)
        {
          i = i+2;
          smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                            active_project -> curves[l][i-1] -> data[1],
                            & active_project -> fact[l],
                            & i,
                            & active_project -> curves[l][i-1] -> ndata,
                            & l);
        }
      }
    }
    if (active_project -> nspec == 2)
    {
      m = 3;
      if (l == 1 || l == 2) m = m+1;
      for (k=0 ; k<m; k++)
      {
        i = i+2;
        smooth_and_save_ (active_project -> curves[l][i-1] -> data[0],
                          active_project -> curves[l][i-1] -> data[1],
                          & active_project -> fact[l],
                          & i,
                          & active_project -> curves[l][i-1] -> ndata,
                          & l);
      }
    }
    if (l == 2)
    {
      g_free (xsk);
      xsk = NULL;
    }
    fill_tool_model ();
    show_the_widgets (curvetoolbox);
    if (l == 0 || l == 3)
    {
      for (i=0; i<4; i=i+3) update_after_calc (i);
    }
    else
    {
      for (i=1; i<3; i++) update_after_calc (i);
    }
  }
  else
  {
    show_error ("No data set(s) to be smoothed\n", 0, calc_win);
  }
}

/*!
  \fn void calc_gr_sq (GtkWidget * box, int id)

  \brief creation of the g(r) / s(q) / s(k) / g(k) calculation widgets

  \param box GtkWidget that will receive the data
  \param id the calculation id
*/
void calc_gr_sq (GtkWidget * box, int id)
{
  gchar * val_a[4]={"Number of &#x3b4;r steps",
                    "Number of &#x3b4;q steps",
                    "Number of &#x3b4;q steps",
                    "Number of &#x3b4;r steps"};
  gchar * val_b[3]={"Q<sub>max</sub> [&#xC5;<sup>-1</sup>]",
                    "Q<sub>max</sub> [&#xC5;<sup>-1</sup>]",
                    "Q<sub>max</sub> for the FFT [&#xC5;<sup>-1</sup>]"};

  GtkWidget * vbox = create_vbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, vbox, FALSE, FALSE, 0);
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (val_a[id], 150, -1, 0.0, 0.5), FALSE, FALSE, 10);
  GtkWidget * entry= create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, GINT_TO_POINTER(id));
  update_entry_int (GTK_ENTRY(entry), active_project -> num_delta[id]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
  if (id > GR)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (val_b[id-1], 150, -1, 0.0, 0.5), FALSE, FALSE, 10);
    GtkWidget * entry= create_entry (G_CALLBACK(set_max), 100, 15, FALSE, GINT_TO_POINTER(id));
    update_entry_double (GTK_ENTRY(entry), active_project -> max[id]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
  }

  if (id == GR || id == GK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_note (0, active_project -> max[GR]), FALSE, FALSE, 0);
  }
  if (id == SQ)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_note (1, active_project -> min[SQ]), FALSE, FALSE, 0);
  }
  if (id == SK|| id == GK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_note (1, active_project -> min[SK]), FALSE, FALSE, 0);
  }
  if (id == GK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_note (2, active_project -> max[SK]), FALSE, FALSE, 0);
  }
  if (id == GR || id == GK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                         markup_label ("D<sub>max</sub> is the maximum inter-atomic distance in the model", -1, -1, 0.0, 0.5),
                         FALSE, FALSE, 0);
  }
  if (id > 0)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                         markup_label ("Q<sub>min</sub> is the minimum wave vector for the model", -1, -1, 0.0, 0.5),
                         FALSE, FALSE, 0);
  }
  if (id == GK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                         markup_label ("Q<sub>max</sub> is the maximum wave vector to compute S(q)", -1, -1, 0.0, 0.5),
                         FALSE, FALSE, 0);
  }

  vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, vbox, FALSE, FALSE, 0);
  GtkWidget * aentry;
  if (id == SK)
  {
    gchar * adv_name[2]={"Probability to keep wave\nvector <i>q</i> > Q<sub>lim</sub> [0.0-1.0]",
                         "Q<sub>lim</sub> [&#xC5;<sup>-1</sup>] in [Q<sub>min</sub>-Q<sub>max</sub>]"};
    GtkWidget * advanced_options = create_expander ("  Advanced options", NULL);
    gtk_widget_set_size_request (advanced_options, -1, 20);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, advanced_options, FALSE, TRUE, 10);
    avbox = create_vbox (5);
    GtkWidget * ahbox;
    GtkWidget * fixed;
    int i;
    for (i=0; i<2; i++)
    {
      ahbox = create_hbox (5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, avbox, ahbox, FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label (adv_name[i], 175, -1, 0.0, 0.5), FALSE, FALSE, 5);
      aentry = create_entry (G_CALLBACK(set_advanced_sq), 100, 15, FALSE, GINT_TO_POINTER(i));
      update_entry_double (GTK_ENTRY(aentry), active_project -> sk_advanced[i]);
      fixed = gtk_fixed_new ();
      gtk_fixed_put (GTK_FIXED(fixed), aentry, 0.0, 0.0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, fixed, FALSE, FALSE, 10);
    }
    g_signal_connect (G_OBJECT(advanced_options), "activate", G_CALLBACK(expand_opt), GINT_TO_POINTER(1));
    add_container_child (CONTAINER_EXP, advanced_options, avbox);
    show_the_widgets (advanced_options);
    widget_set_sensitive (advanced_options, 1);
  }

  GtkWidget * smooth_options = create_expander ("  Gaussian data smoothing", NULL);
  gtk_widget_set_size_request (smooth_options, -1, 20);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, smooth_options, FALSE, TRUE, 10);
  avbox = create_vbox (5);
  smbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, smbox, gtk_label_new("Factor [0.0-1.0]"), FALSE, FALSE, 0);
  aentry = create_entry (G_CALLBACK(set_sfact), 100, 15, FALSE, GINT_TO_POINTER(id));
  update_entry_double (GTK_ENTRY(aentry), active_project -> fact[id]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, smbox, aentry, FALSE, TRUE, 10);
  GtkWidget * smooth = create_button ("Smooth", IMG_NONE, NULL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(on_smoother_released), GINT_TO_POINTER(id));
  g_signal_connect (G_OBJECT(smooth_options), "activate", G_CALLBACK(expand_opt), GINT_TO_POINTER(0));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, smbox, smooth, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, avbox, smbox, FALSE, FALSE, 5);
  add_container_child (CONTAINER_EXP, smooth_options, avbox);
  show_the_widgets (smooth_options);
  widget_set_sensitive (smooth_options, 1);
}

/*!
  \fn G_MODULE_EXPORT void run_on_calc_activate (GtkDialog * dial, gint response_id, gpointer data)

  \brief create a calculation dialog: run the dialog

  \param dial the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_calc_activate (GtkDialog * dial, gint response_id, gpointer data)
{
  int i;
  int id = GPOINTER_TO_INT(data);
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      switch (id)
      {
        case GR:
          if (test_gr (GR)) on_calc_gr_released (calc_win, NULL);
          break;
        case SQ:
          if (test_sq (SQ)) on_calc_sq_released (calc_win, NULL);
          break;
        case SK:
          if (test_sq (SK)) on_calc_sk_released (calc_win, NULL);
          break;
        case GK:
          if (test_gr (GK)) on_calc_gq_released (calc_win, NULL);
          break;
        case BD:
          if (test_bonds ()) on_calc_bonds_released (calc_win, NULL);
          break;
        case RI-1:
          if (test_rings ())
          {
            //show_the_widgets (spinner);
            //gtk_spinner_start (GTK_SPINNER(spinner));
            on_calc_rings_released (calc_win, NULL);
            toggled_rings = FALSE;
            //gtk_spinner_stop (GTK_SPINNER(spinner));
            //hide_the_widgets (spinner);
          }
          break;
        case CH-1:
          if (test_rings ())
          {
            //show_the_widgets (spinner);
            //gtk_spinner_start (GTK_SPINNER(spinner));
            on_calc_chains_released (calc_win, NULL);
            //gtk_spinner_stop (GTK_SPINNER(spinner));
            //hide_the_widgets (spinner);
          }
          break;
        case SP-1:
          if (test_sph ()) on_calc_sph_released (calc_win, NULL);
          break;
        case MS-1:
          if (test_msd ()) on_calc_msd_released (calc_win, NULL);
          break;
        default:
          break;
      }
      break;
    default:
      frag_update = mol_update = 0;
      for (i=0; i<3; i++) active_project -> runc[i] = FALSE;
      destroy_this_dialog (dial);
      calc_win = destroy_this_widget (calc_win);
      avbox = NULL;
  }
}

/*!
  \fn G_MODULE_EXPORT void on_calc_activate (GtkWidget * widg, gpointer data)

  \brief create a calculation dialog - prepare the dialog

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_activate (GtkWidget * widg, gpointer data)
{
  int id = GPOINTER_TO_INT(data);
  calc_win = calc_window(id);
  GtkWidget * box = dialog_get_content_area (calc_win);
  gtk_box_set_homogeneous (GTK_BOX(box), FALSE);
  switch (id)
  {
    case BD:
      calc_bonds (box);
      break;
    case RI-1:
      search_type = 0;
      calc_rings (box);
      break;
    case CH-1:
      search_type = 1;
      calc_rings (box);
      break;
    case SP-1:
      calc_sph (box);
      break;
    case MS-1:
      calc_msd (box);
      break;
    case 9:
      // calc_valence (box);
      break;
    default:
      calc_gr_sq (box, id);
      break;
  }

#ifndef GTK4
  gtk_window_set_icon (GTK_WINDOW (calc_win), gdk_pixbuf_new_from_file(calc_img[id], NULL));
#endif
  GtkWidget * vbox = create_vbox (BSEP);
  gtk_widget_set_size_request (vbox, -1, 30);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, vbox, FALSE, FALSE, 0);
  /*GtkWidget * spinner = gtk_spinner_new ();
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, spinner, FALSE, FALSE, 0);
  gtk_widget_set_size_request (spinner, -1, 30);
  hide_the_widgets (spinner);*/
  run_this_gtk_dialog (calc_win, G_CALLBACK(run_on_calc_activate), data);
}
