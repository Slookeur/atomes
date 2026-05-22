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

  gboolean test_gr (int rdf);
  gboolean test_sq (int fdq);
  gboolean test_bonds ();
  gboolean test_rings ();
  gboolean test_sph ();
  gboolean test_msd ();
  gboolean test_skt ();

  void update_omega_max ();
  void calc_sph (GtkWidget * vbox);
  void calc_msd (GtkWidget * vbox, int cid);
  void calc_rings (GtkWidget * vbox);
  void calc_bonds (GtkWidget * vbox);
  void add_advanced_options (int skq, dint skadv[2], GtkWidget * vbox);
  void add_smoothing_options (int grsqk, GtkWidget * vbox);
  void calc_gr_sq (GtkWidget * box, int id);
  void add_remove_t_steps_q_vectors (int val, int calc);
  void add_correlations_options (int cid);
  void calc_sk_t (GtkWidget * box);

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
  G_MODULE_EXPORT void set_skt_step_id (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_sqw_q_id (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_t_q_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_correlations (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void toggle_skt_all (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void toggle_skt_all (GtkToggleButton * but, gpointer data);
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
extern G_MODULE_EXPORT void on_calc_skt_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_rings_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_chains_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_msd_released (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_sph_released (GtkWidget * widg, gpointer data);
extern void dyna_parameters (GtkWidget * vbox, int cid);

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
  return calc_dialog;
}

void add_remove_t_steps_q_vectors (int val, int calc);

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
  if (v > 0.0 && v > active_project -> analysis[c] -> min)
  {
    if (active_project -> analysis[c] -> max != v)
    {
      active_project -> analysis[c] -> max = v;
      if (c == SKT)
      {
        add_remove_t_steps_q_vectors (active_project -> sqw_n_data_sets, 1);
      }
      // Max has changed do something !?
    }
  }
  update_entry_double (entry, active_project -> analysis[c] -> max);
}

GtkWidget * rings_box[2];
GtkWidget * omega_max_hbox = NULL;
GtkWidget * omega_max_info = NULL;

/*!
  \fn void update_omega_max ()

  \brief update omega max information string
*/
void update_omega_max ()
{
  gchar * freq_unit[5]={" THz", " THz", " MHz", " KHz", " Hz"};
  gchar * str;
  omega_max_info = destroy_this_widget (omega_max_info);
  double delt = (preferences) ? tmp_delta_t[0]: active_project -> analysis[MSD] -> delta;
  int num_delt = (preferences) ? tmp_num_delta[MSD-2] : active_project -> analysis[MSD] -> num_delta;
  int t_unit = (preferences) ? (int)tmp_delta_t[1] : active_project -> tunit;
  if ( delt > 0.0 && num_delt && t_unit > -1)
  {
    omega_max_info = create_hbox(0);
    double omega_max = pi/(delt * num_delt);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, omega_max_info, markup_label (_("&#969;<sub>max</sub> ="), 50, -1, 0.0, 0.5), FALSE, FALSE, 2);
    str = g_strdup_printf ("%f", (t_unit == 1) ? omega_max : omega_max * 1000.0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, omega_max_info, markup_label (str, 100, -1, 1.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, omega_max_info, markup_label (freq_unit[t_unit], 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
  else
  {
    str = g_strdup_printf (_("Update step(s) between conf. and &#x3b4;t to evaluate &#969;<sub>max</sub>"));
    omega_max_info = markup_label (str, -1, -1, 0.5, 0.5);
    g_free (str);
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, omega_max_hbox, omega_max_info, FALSE, FALSE, 5);
  show_the_widgets (omega_max_hbox);
}

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
  if (c == RIN)
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
  else if (c == CHA)
  {
    k = (preferences) ? tmp_csparam[1] : active_project -> csparam[5];
  }
  else if (c > -1 && ! preferences)
  {
    k = active_project -> analysis[c] -> num_delta;
  }
  if (c < 0 && ! preferences)
  {
    if (v > 0.0)
    {
      if (active_project -> analysis[-c] -> delta != v)
      {
        active_project -> analysis[-c] -> delta = v;
      }
    }
  }
  else if (i > 0 && ! preferences)
  {
    if (c == RIN)
    {
      if (active_project -> rsparam[j][1] != i)
      {
        active_project -> rsparam[j][1] = i;
      }
      k = active_project -> rsparam[j][1] = i;
    }
    else if (c == CHA)
    {
      if (active_project -> csparam[5] != i)
      {
        active_project -> csparam[5] = i;
      }
      k = active_project -> csparam[5] = i;
    }
    else
    {
      if (active_project -> analysis[c] -> num_delta != i)
      {
        active_project -> analysis[c] -> num_delta = i;
      }
      k = active_project -> analysis[c] -> num_delta;
    }
  }
  if (c < 0)
  {
    update_entry_double (entry, active_project -> analysis[-c] -> delta);
  }
  else
  {
    update_entry_int (entry, k);
  }
  if (omega_max_hbox) update_omega_max ();
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
  if (omega_max_hbox) update_omega_max ();
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
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,  markup_label ( _("Maximum <b><i>l</i></b>, <i>l<sub>max</sub></i> &#x2208; [2-40]"), 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
  entry = create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, (gpointer)GINT_TO_POINTER(SPH));
  update_entry_int (GTK_ENTRY(entry), active_project -> analysis[SPH] -> num_delta);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
}

/*!
  \fn void calc_msd (GtkWidget * vbox, int cid)

  \brief creation of the MSD calculation widgets

  \param vbox GtkWidget that will receive the data
  \param cid calculation id, MSD or SKT
*/
void calc_msd (GtkWidget * vbox, int cid)
{
  int i, j, k, l, m;
  gchar * val_b[3]={i18n("Number of configurations:"),
                    i18n("\tTime step &#x3b4;t used during the dynamics:"),
                    i18n("\tNumber of step(s) between each configuration:")};
  gchar * val_c[3]={i18n("Number of configurations:"),
                    i18n("Time step &#x3b4;t"),
                    i18n("Step(s) between conf.")};
  GtkWidget * hbox;
  GtkWidget * entry;
  k = (cid == MSD) ? 15 : 0;
  l = (cid == MSD) ? 350 : 150;
  m = (cid == MSD) ? 0 : 5;
  hbox = create_hbox (k);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new (_(val_b[0])), FALSE, FALSE, m);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (g_strdup_printf ("<b>%d</b>",active_project -> steps), -1, 50, 0.5, 0.5), FALSE, FALSE, 10);
  for (i=1; i<3; i++)
  {
    hbox = create_hbox (k);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ((cid == MSD) ? _(val_b[i]) : _(val_c[i]), l, -1, 0.0, 0.5), FALSE, FALSE, m);
    if (i == 1)
    {
      entry = create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, (gpointer)GINT_TO_POINTER(-MSD));
      update_entry_double (GTK_ENTRY(entry), active_project -> analysis[MSD] -> delta);
    }
    else
    {
      entry = create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, (gpointer)GINT_TO_POINTER(MSD));
      update_entry_int (GTK_ENTRY(entry), active_project -> analysis[MSD] -> num_delta);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 2*m);
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
  for (i=0; i<num; i++) combo_text_append (rings_box[id], _(list_item[i]));
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
        for (i=1; i<4; i++)
        {
          button_set_status(rings_check[i], ! status);
          active_project -> csparam[i+1] = ! status;
        }
      }
      else if (oid == 1 && status)
      {
        button_set_status(rings_check[0], ! status);
        active_project -> csparam[1] = ! status;
      }
      else if (oid == 2 && status)
      {
        button_set_status(rings_check[1], ! status);
        active_project -> csparam[2] = ! status;
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
  gchar * defs[6]={i18n("All Rings (No Rule)"),
                   i18n("King's [1, 3]"),
                   i18n("Guttman's [2]"),
                   i18n("Primitives [4, 5, 6]"),
                   i18n("Strongs [4, 5]")};
  gchar * val_a[2]={i18n("Definition of ring to be used: "), i18n("Atom(s) to initiate the search from: ")};
  gchar * val_b[2][2]={{i18n("<i><b>n</b><sub>max</sub></i> = maximum size for a ring <sup>*</sup>"),
                        i18n("Maximum number of rings of size <i><b>n</b></i> per MD step <sup>**</sup>")},
                       {i18n("<i><b>n</b><sub>max</sub></i> = maximum size for a chain <sup>*</sup>"),
                        i18n("Maximum number of chains of size <i><b>n</b></i> per MD step <sup>**</sup>")}};
  gchar * val_c[2][4]={{i18n("Only search for ABAB rings"), i18n("No homopolar bonds in the rings (A-A, B-B ...) <sup>***</sup>"), i18n("No homopolar bonds in the connectivity matrix"), " "},
                       {i18n("Only search for AAAA chains"), i18n("Only search for ABAB chains"),
                        i18n("No homopolar bonds in the chains (A-A, B-B ...) <sup>***</sup>"), i18n("Only search for 1-(2)<sub>n</sub>-1 coordinated atom chains, ie. isolated chains.")}};
  gchar * start[3]={"<sup>*</sup>", "<sup>**</sup>", "<sup>***</sup>"};
  gchar * val_d[3]={i18n("in total number of nodes (or atoms)"),
                    i18n("value used for memory allocation = f(<i><b>n</b><sub>max</sub></i>, system studied)"),
                    i18n("but homopolar bonds can shorten the rings")};
  gchar * val_e={i18n("<sub>[1] S. V. King. <i>Nature</i>, <b>213</b>:1112 (1967).</sub>\n"
                      "<sub>[2] L. Guttman. <i>J. Non-Cryst. Solids.</i>, <b>116</b>:145-147 (1990).</sub>\n"
                      "<sub>[3] D. S. Franzblau. <i>Phys. Rev. B</i>, <b>44</b>(10):4925-4930 (1991).</sub>\n"
                      "<sub>[4] K. Goetzke and H. J. Klein. <i>J. Non-Cryst. Solids.</i>, <b>127</b>:215-220 (1991).</sub>\n"
                      "<sub>[5] X. Yuan and A. N. Cormack. <i>Comp. Mat. Sci.</i>, <b>24</b>:343-360 (2002).</sub>\n"
                      "<sub>[6] F. Wooten. <i>Acta Cryst. A</i>, <b>58</b>(4):346-351 (2002).</sub>")};
  gchar * list_node[(preferences) ? 2 : active_project -> nspec+1];
  int i, j, k;

  if (! search_type)  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, combox_rings (_(val_a[0]), 5, defs, 0), FALSE, FALSE, 5);

  list_node[0] = g_strdup_printf (_("All"));
  if (preferences)
  {
    list_node[1] = g_strdup_printf (_("Chemical Species"));
  }
  else
  {
    for (i=0; i<active_project -> nspec; i++) list_node[i+1] = g_strdup_printf ("%s", active_chem -> label[i]);
  }

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, combox_rings (_(val_a[1]), (preferences) ? 2 : active_project -> nspec+1, list_node, 1), FALSE, FALSE, 5);

  j = (preferences) ? tmp_rsparam[0] : active_project -> rsearch[0];
  k = RIN + search_type;
  GtkWidget * hbox;
  for (i=0; i<2; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (_(val_b[search_type][i]), 400, -1, 0.0, 0.5), FALSE, FALSE, 0);
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
      status = (search_type) ? active_project -> csparam[i+1] : active_project -> rsparam[j][i+2];
    }
    rings_check[i] = check_button (_(val_c[search_type][i]), -1, 40, status, G_CALLBACK(toggle_rings), GINT_TO_POINTER(i));
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, rings_check[i], FALSE, FALSE, 0);
    if (! preferences)
    {
      if (active_project -> nspec == 1) widget_set_sensitive (rings_check[i], 0);
      if (! search_type && j < 0) widget_set_sensitive (rings_check[i], 0);
    }
  }
  for (i=0; i<3; i++)
  {
    append_comments (vbox, start[i], _(val_d[i]));
  }
  if (! search_type)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label (_(val_e), -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
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
      show_info (_("The result of the nearest neighbors analysis\n"
                   "will be saved in a file that you may use afterwards.\n"), 0, MainWindow);
#ifdef GTK4
     GtkFileChooserNative * info;
#else
      GtkWidget * info;
#endif
      info = create_file_chooser (_("Save neighbors analysis in file"),
                                  GTK_WINDOW(MainWindow),
                                  GTK_FILE_CHOOSER_ACTION_SAVE,
                                  _("Save"));
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

  \param rdf type of g(r): real space (GR) or FFT (GK)
*/
gboolean test_gr (int rdf)
{
  if (active_project -> analysis[rdf] -> num_delta < 2)
  {
    show_warning (_("You must specify a number of &#x3b4;r intevals >= 2\n"
                    "to discretize the real space between 0.0 and D<sub>max</sub>\n"), calc_win);
    return FALSE;
  }
  else if (rdf == GDK && (active_project -> analysis[rdf] -> max > active_project -> analysis[SKD] -> max || active_project -> analysis[rdf] -> max <= active_project -> analysis[SKD] -> min))
  {
    show_warning (_("You must specify a maximum wave vector Q<sub>max</sub>[FFT]\n"
                    "for the FFT, with Q<sub>min</sub> < Q<sub>max</sub>[FFT] <= Q<sub>max</sub>"), calc_win);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn gboolean test_sq (int fdq)

  \brief is it safe to compute s(q) ?

  \param fdq type of s(q): 0 = FFT g(r), 1 = Debye, 2 = Dynamic
*/
gboolean test_sq (int fdq)
{
  if (active_project -> analysis[fdq] -> max <= active_project -> analysis[fdq] -> min)
  {
    show_warning (_("You must specify a maximum wave vector Q<sub>max</sub>\n"
                    "note that Q<sub>max</sub> must be > Q<sub>min</sub>"), calc_win);
    return FALSE;
  }
  else if (active_project -> analysis[fdq] -> num_delta < 2)
  {
    show_warning (_("You must specify a number of &#x3b4;q intervals >= 2\n"
                    "to discretize the reciprocal space between 0.0 and Q<sub>max</sub>\n"), calc_win);
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
  if (active_project -> runc[0] && active_project -> analysis[BND] -> num_delta < 2)
  {
    show_warning (_("You must specify a number of &#x3b4;r intervals >= 2\n"
                    "to discretize the real space between\n"
                    "the shortest and the highest inter-atomic distances"), calc_win);
    return FALSE;
  }
  if (active_project -> runc[1] && active_project -> analysis[ANG] -> num_delta < 2)
  {
    show_warning (_("You must specify a number of &#x3b4;&#x3b8; intervals >= 2\n"
                    "to discretize the angular space between 0 and 180°"), calc_win);
    return FALSE;
  }
  return TRUE;
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
  gchar * sobj[2]={i18n("ring"), i18n("chain")};
  gchar * str;
  if (i == 0)
  {
    str = g_strdup_printf (_("You must specify a maximum %s size > 1 for the search"), _(sobj[search_type]));
    show_warning (str, calc_win);
    g_free (str);
    return FALSE;
  }
  if (j == 0)
  {
    str = g_strdup_printf (_("You must specify a number of %ss per size 'n' and per node\n"
                             "this value is used when allocating the memory to store the results\n"
                             "and depends on both the maximum %s size used in the search\n"
                             "and the system studied.\n"
                             "We recommend a value at least equal to 100.\n"
                             "If it appears that the value given is not big enough\n"
                             "(ex: more than 100 different %ss of size 'n' for a single node)\n"
                             "then the search will failed but the program will propose\n"
                             "you to initiate a new search using a higher value"),
                             sobj[search_type], sobj[search_type], sobj[search_type]);
    show_warning (str, calc_win);
    g_free (str);
    return FALSE;
  }
  return TRUE;
}

/*!
  \fn gboolean test_sph ()

  \brief is it safe to compute spherical harmonics ?
*/
gboolean test_sph ()
{
  if (active_project -> analysis[SPH] -> num_delta < 2 || active_project -> analysis[SPH] -> num_delta > 40)
  {
    show_warning (_("You must specify a number <i>l<sub>max</sub></i> &#x2208; [2-40]"), calc_win);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn gboolean test_msd ()

  \brief is it safe to compute MSD ?
*/
gboolean test_msd ()
{
  if(active_project -> steps > 1)
  {
    if (active_project -> analysis[MSD] -> delta < 1)
    {
      show_warning (_("You must specify the time step &#x3b4;t\n"
                      "used to integrate the Newton's equations\n"
                      "of motion during the molecular dynamics\n"), calc_win);
      return FALSE;
    }
    if (active_project -> tunit < 0)
    {
      show_warning (_("You must specify the time unit\n"
                      "used to integrate the Newton's equations\n"
                      "of motion during the molecular dynamics\n"), calc_win);
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    if (active_project -> analysis[MSD] -> num_delta < 1)
    {
      show_warning (_("You must specify the number of steps\n"
                    "between each of the %d configurations\n"
                    "found for the molecular dynamics\n"), calc_win);
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
}

/*!
  \fn gboolean test_skt ()

  \brief is it safe to compute dynamic structure factors ?
*/
gboolean test_skt ()
{
  if (! test_sq(SKT)) return FALSE;
  if (! test_msd()) return FALSE;
  if (active_project -> sqw_freq > 0)
  {
    return TRUE;
  }
  else
  {
    show_warning (_("You must specify the number of frequency points\n"), calc_win);
    return FALSE;
  }
}

/*!
  \fn void calc_bonds (GtkWidget * vbox)

  \brief creation of the bond calculation widgets

  \param vbox GtkWidget that will receive the data
*/
void calc_bonds (GtkWidget * vbox)
{
  gchar * val_a[2]={i18n("Number of &#x3b4;r [D<sub>ij</sub>min-D<sub>ij</sub>max]"),
                    i18n("Number of &#x3b4;&#x3b8; [0-180°]")};
  gchar * val_b[4]={i18n("First coordination sphere properties"),
                    i18n("Bond and dihedral angles distribution"),
                    i18n("Search for molecules and isolated fragments"),
                    i18n("Output detailed results in text file")};

  GtkWidget * hbox;
  int i;
  for (i=0; i<3; i++)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                         check_button (_(val_b[i]), -1, 40, active_project -> runc[i], G_CALLBACK(toggle_bond), (gpointer)GINT_TO_POINTER(i)),
                         FALSE, FALSE, 0);
    if (i < 2)
    {
      hbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (_(val_a[i]), 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
      ba_entry[i] = create_entry (G_CALLBACK(set_delta), 150, 15, FALSE, (gpointer)GINT_TO_POINTER(BND+i));
      update_entry_int (GTK_ENTRY(ba_entry[i]), active_project -> analysis[BND+i] -> num_delta);
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
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (note[i], 50, -1, 0.0, 0.5), FALSE, FALSE, 5);
  str = g_strdup_printf ("%f", val);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 100, -1, 1.0, 0.5), FALSE, FALSE, 0);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (unit[i], 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
  return hbox;
}

GtkWidget * avbox = NULL;
GtkWidget * smbox = NULL;
dint skadv[2];
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
  dint * idc = (dint *)data;
  const gchar * m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  double qmin = active_project -> analysis[idc -> a] -> min;
  double qmax = active_project -> analysis[idc -> a] -> max;
  int idk = (idc -> a == SKD) ? 0 : 1;
  if (v != active_project -> sk_advanced[idk][idc -> b])
  {
    if (idc -> b == 0 && (v < 0.0 || v > 1.0))
    {
      show_warning (_("You must specify a probability between 0.0 and 1.0"), calc_win);
    }
    else if (idc -> b == 1 && (v < qmin || v > qmax))
    {
      show_warning (_("Q<sub>lim</sub> must be &#8805; Q<sub>min</sub> and &#8804; Q<sub>max</sub>"), calc_win);
    }
    else
    {
      active_project -> sk_advanced[idk][idc -> b] = v;
    }
  }
  update_entry_double (entry, active_project -> sk_advanced[idk][idc -> b]);
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
    show_warning (_("The smoothing factor must be between 0.0 and 1.0"), calc_win);
  }
  else
  {
    active_project -> analysis[i] -> fact = v;
  }
  update_entry_double (entry, active_project -> analysis[i] -> fact);
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
  int g, h, i, j, k, l, m;

  l = GPOINTER_TO_INT(data);
  if (active_project -> analysis[l] -> calc_ok)
  {
    if (l == SKD || l == SKT)
    {
      xsk = duplicate_double(active_project -> analysis[l] -> curves[0] -> ndata, active_project -> analysis[l] -> curves[0] -> data[0]);
    }
    g = (l == SKT) ? (active_project -> skt_all_sets) ? active_project -> steps - active_project -> skt_corr_threshold : active_project -> skt_n_data_sets : 1;
    h = (l == SKT) ? active_project -> analysis[SKD] -> numc : 1;
    for (j=0; j<g; j++)
    {
      i = 1 + j*h;
      smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                        active_project -> analysis[l] -> curves[i-1] -> data[1],
                        & active_project -> analysis[l] -> fact,
                        & i,
                        & active_project -> analysis[l] -> curves[i-1] -> ndata,
                        & l);
      i = i+2;
      smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                        active_project -> analysis[l] -> curves[i-1] -> data[1],
                        & active_project -> analysis[l] -> fact,
                        & i,
                        & active_project -> analysis[l] -> curves[i-1] -> ndata,
                        & l);
      i = i+2;
      smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                        active_project -> analysis[l] -> curves[i-1] -> data[1],
                        & active_project -> analysis[l] -> fact,
                        & i,
                        & active_project -> analysis[l] -> curves[i-1] -> ndata,
                        & l);
      i = i+2;
      smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                        active_project -> analysis[l] -> curves[i-1] -> data[1],
                        & active_project -> analysis[l] -> fact,
                        & i,
                        & active_project -> analysis[l] -> curves[i-1] -> ndata,
                        & l);
      if (l ==GDR || l == GDK)
      {
        i = i+2;
        smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                          active_project -> analysis[l] -> curves[i-1] -> data[1],
                          & active_project -> analysis[l] -> fact,
                          & i,
                          & active_project -> analysis[l] -> curves[i-1] -> ndata,
                          & l);
        i = i+2;
        smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                          active_project -> analysis[l] -> curves[i-1] -> data[1],
                          & active_project -> analysis[l] -> fact,
                          & i,
                          & active_project -> analysis[l] -> curves[i-1] -> ndata,
                          & l);
        i = i+2;
        smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                          active_project -> analysis[l] -> curves[i-1] -> data[1],
                          & active_project -> analysis[l] -> fact,
                          & i,
                          & active_project -> analysis[l] -> curves[i-1] -> ndata,
                          & l);
        i = i+2;
        smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                          active_project -> analysis[l] -> curves[i-1] -> data[1],
                          & active_project -> analysis[l] -> fact,
                          & i,
                          & active_project -> analysis[l] -> curves[i-1] -> ndata,
                          & l);
      }
      for (k=0 ; k<active_project -> nspec ; k++)
      {
        for (m=0 ; m<active_project -> nspec ; m++)
        {
          i = i+2;
          smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                            active_project -> analysis[l] -> curves[i-1] -> data[1],
                            & active_project -> analysis[l] -> fact,
                            & i,
                            & active_project -> analysis[l] -> curves[i-1] -> ndata,
                            & l);
          if (l == GDR || l == GDK)
          {
            i = i+2;
            smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                              active_project -> analysis[l] -> curves[i-1] -> data[1],
                              & active_project -> analysis[l] -> fact,
                              & i,
                              & active_project -> analysis[l] -> curves[i-1] -> ndata,
                              & l);
            i = i+1;
          }
        }
      }
      if (l == SQD || l == SKD || l == SKT)
      {
        for (k=0 ; k<active_project -> nspec ; k++)
        {
          for (m=0 ; m<active_project -> nspec ; m++)
          {
            i = i+2;
            smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                              active_project -> analysis[l] -> curves[i-1] -> data[1],
                              & active_project -> analysis[l] -> fact,
                              & i,
                              & active_project -> analysis[l] -> curves[i-1] -> ndata,
                              & l);
          }
        }
      }
      if (active_project -> nspec == 2)
      {
        m = 3;
        if (l == SQD || l == SKD || l == SKT) m = m+1;
        for (k=0 ; k<m; k++)
        {
          i = i+2;
          smooth_and_save_ (active_project -> analysis[l] -> curves[i-1] -> data[0],
                            active_project -> analysis[l] -> curves[i-1] -> data[1],
                            & active_project -> analysis[l] -> fact,
                            & i,
                            & active_project -> analysis[l] -> curves[i-1] -> ndata,
                           & l);
        }
      }
    }
    if (l == SKD || l == SKT)
    {
      g_free (xsk);
      xsk = NULL;
    }
    fill_tool_model ();
    show_the_widgets (curvetoolbox);
    if (l == GDR || l == GDK)
    {
      for (i=0; i<4; i=i+3) update_after_calc (i);
    }
    else if (l == SQD || l == SKD)
    {
      for (i=1; i<3; i++) update_after_calc (i);
    }
    else
    {
      update_after_calc (l);
    }
  }
  else
  {
    show_error (_("No data set(s) to be smoothed\n"), 0, calc_win);
  }
}

/*!
  \fn void add_advanced_options (int skq, dint skadv[2], GtkWidget * vbox)

  \brief add k-point selection advanced options

  \param skq calculation ID (SKD or SKT)
  \param skadv associated pointers
  \param vbox target box widget
*/
void add_advanced_options (int skq, dint skadv[2], GtkWidget * vbox)
{
  gchar * adv_name[2]={i18n("Probability to keep wave\nvector <i>q</i> > Q<sub>lim</sub> &#x2208; [0.0-1.0]"),
                       i18n("Q<sub>lim</sub> [&#xC5;<sup>-1</sup>] &#x2208; [Q<sub>min</sub>-Q<sub>max</sub>]")};
  GtkWidget * advanced_options = create_expander (_("  Advanced options"), NULL);
  gtk_widget_set_size_request (advanced_options, -1, 20);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, advanced_options, FALSE, TRUE, 10);
  avbox = create_vbox (5);
  GtkWidget * ahbox;
  GtkWidget * fixed;
  GtkWidget * aentry;
  int i, j;
  i = (skq == SKT) ? 1 : 0;
  for (j=0; j<2; j++)
  {
    ahbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, avbox, ahbox, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label (_(adv_name[j]), 175, -1, 0.0, 0.5), FALSE, FALSE, 5);
    aentry = create_entry (G_CALLBACK(set_advanced_sq), 100, 15, FALSE, & skadv[j]);
    update_entry_double (GTK_ENTRY(aentry), active_project -> sk_advanced[i][j]);
    fixed = gtk_fixed_new ();
    gtk_fixed_put (GTK_FIXED(fixed), aentry, 0.0, 0.0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, fixed, FALSE, FALSE, 10);
  }
  g_signal_connect (G_OBJECT(advanced_options), "activate", G_CALLBACK(expand_opt), GINT_TO_POINTER(i));
  add_container_child (CONTAINER_EXP, advanced_options, avbox);
  show_the_widgets (advanced_options);
  widget_set_sensitive (advanced_options, 1);
}

/*!
  \fn void add_smoothing_options (int grsqk, GtkWidget * vbox)

  \brief add gaussian smoothing options

  \param calculation ID (GDR, SQD, SKD, GDK or SKT)
  \param vbox target box widget
*/
void add_smoothing_options (int grsqk, GtkWidget * vbox)
{
  GtkWidget * smooth_options = create_expander (_("  Gaussian data smoothing"), NULL);
  gtk_widget_set_size_request (smooth_options, -1, 20);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, smooth_options, FALSE, TRUE, 10);
  GtkWidget * avbox = create_vbox (5);
  GtkWidget * smbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, smbox, gtk_label_new(_("Factor [0.0-1.0]")), FALSE, FALSE, 0);
  GtkWidget * sentry = create_entry (G_CALLBACK(set_sfact), 100, 15, FALSE, GINT_TO_POINTER(grsqk));
  update_entry_double (GTK_ENTRY(sentry), active_project -> analysis[grsqk] -> fact);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, smbox, sentry, FALSE, TRUE, 10);
  GtkWidget * smooth = create_button (_("Smooth"), IMG_NONE, NULL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(on_smoother_released), GINT_TO_POINTER(grsqk));
  g_signal_connect (G_OBJECT(smooth_options), "activate", G_CALLBACK(expand_opt), GINT_TO_POINTER(0));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, smbox, smooth, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, avbox, smbox, FALSE, FALSE, 5);
  add_container_child (CONTAINER_EXP, smooth_options, avbox);
  show_the_widgets (smooth_options);
  widget_set_sensitive (smooth_options, 1);
}

/*!
  \fn void calc_gr_sq (GtkWidget * box, int id)

  \brief creation of the g(r) / s(q) / s(k) / g(k) calculation widgets

  \param box GtkWidget that will receive the data
  \param id the calculation id
*/
void calc_gr_sq (GtkWidget * box, int id)
{
  gchar * val_a[4]={i18n("Number of &#x3b4;r steps"),
                    i18n("Number of &#x3b4;q steps"),
                    i18n("Number of &#x3b4;q steps"),
                    i18n("Number of &#x3b4;r steps")};
  gchar * val_b[3]={i18n("Q<sub>max</sub> [&#xC5;<sup>-1</sup>]"),
                    i18n("Q<sub>max</sub> [&#xC5;<sup>-1</sup>]"),
                    i18n("Q<sub>max</sub> for the FFT [&#xC5;<sup>-1</sup>]")};

  if (id == SKD)
  {
    skadv[0].a = skadv[1].a = SKD;
    skadv[0].b = 0;
    skadv[1].b = 1;
  }
  GtkWidget * vbox = create_vbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, vbox, FALSE, FALSE, 0);
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (_(val_a[id]), 150, -1, 0.0, 0.5), FALSE, FALSE, 10);
  GtkWidget * entry= create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, GINT_TO_POINTER(id));
  update_entry_int (GTK_ENTRY(entry), active_project -> analysis[id] -> num_delta);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
  if (id > GDR)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (_(val_b[id-1]), 150, -1, 0.0, 0.5), FALSE, FALSE, 10);
    GtkWidget * entry= create_entry (G_CALLBACK(set_max), 100, 15, FALSE, GINT_TO_POINTER(id));
    update_entry_double (GTK_ENTRY(entry), active_project -> analysis[id] -> max);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
  }

  if (id == GDR || id == GDK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_note (0, active_project -> analysis[GDR] -> max), FALSE, FALSE, 0);
  }
  if (id == SQD)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_note (1, active_project -> analysis[SQD] -> min), FALSE, FALSE, 0);
  }
  if (id == SKD|| id == GDK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_note (1, active_project -> analysis[SKD] -> min), FALSE, FALSE, 0);
  }
  if (id == GDK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_note (2, active_project -> analysis[SKD] -> max), FALSE, FALSE, 0);
  }
  if (id == GDR || id == GDK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                         markup_label (_("D<sub>max</sub> is the maximum inter-atomic distance in the model"), -1, -1, 0.0, 0.5),
                         FALSE, FALSE, 0);
  }
  if (id > 0)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                         markup_label (_("Q<sub>min</sub> is the minimum wave vector for the model"), -1, -1, 0.0, 0.5),
                         FALSE, FALSE, 0);
  }
  if (id == GDK)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                         markup_label (_("Q<sub>max</sub> is the maximum wave vector to compute S(q)"), -1, -1, 0.0, 0.5),
                         FALSE, FALSE, 0);
  }

  vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, vbox, FALSE, FALSE, 0);

  if (id == SKD) add_advanced_options (SKD, skadv, vbox);

  add_smoothing_options (id, vbox);
}

GtkWidget * skt_all_info = NULL;
GtkWidget * sktqw_vbox[2];
GtkWidget * sktqw_delta[2];
GtkWidget * t_q_vbox[2];
GtkWidget * skt_res_info;

/*!
  \fn G_MODULE_EXPORT void set_skt_step_id (GtkEntry * res, gpointer data)

  \brief set "to be saved" step id

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_skt_step_id (GtkEntry * res, gpointer data)
{
  int sid = GPOINTER_TO_INT (data);
  const gchar * m = entry_get_text (res);
  int v = (int) string_to_double ((gpointer)m);
  if (sid && sid < active_project -> skt_n_data_sets - 1)
  {
    if (v > active_project -> skt_step_id[sid-1] && v < active_project -> skt_step_id[sid+1]) active_project -> skt_step_id[sid] = v;
  }
  else if (sid)
  {
    if (v > active_project -> skt_step_id[sid-1]) active_project -> skt_step_id[sid] = v;
  }
  else if (! sid && active_project -> skt_n_data_sets > 1)
  {
    if (v < active_project -> skt_step_id[sid+1]) active_project -> skt_step_id[sid] = v;
  }
  update_entry_int (res, active_project -> skt_step_id[sid]);
}

/*!
  \fn G_MODULE_EXPORT void set_sqw_q_id (GtkEntry * res, gpointer data)

  \brief set "to be saved" q vector

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_sqw_q_id (GtkEntry * res, gpointer data)
{
  int sid = GPOINTER_TO_INT (data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (sid && sid < active_project -> sqw_n_data_sets - 1)
  {
    if (v > active_project -> sqw_q_id[sid-1] && v < active_project -> sqw_q_id[sid+1]) active_project -> sqw_q_id[sid] = v;
  }
  else if (sid)
  {
    if (v > active_project -> sqw_q_id[sid-1] && v <= active_project -> analysis[SKT] -> max) active_project -> sqw_q_id[sid] = v;
  }
  else if (! sid && active_project -> sqw_n_data_sets > 1)
  {
    if (v < active_project -> sqw_q_id[sid+1] && v >= active_project -> analysis[SKT] -> min) active_project -> sqw_q_id[sid] = v;
  }
  update_entry_double (res, active_project -> sqw_q_id[sid]);
}

/*!
  \fn void add_remove_t_steps_q_vectors (int val, int calc)

  \brief add or remove t steps results to save

  \param val total number of t steps
  \param calc 0 = S(k,t), 1 = S(q,w)
*/
void add_remove_t_steps_q_vectors (int val, int calc)
{
  t_q_vbox[calc] = destroy_this_widget (t_q_vbox[calc]);
  switch (calc)
  {
    case 0:
      if (active_project -> skt_step_id) g_free (active_project -> skt_step_id);
      active_project -> skt_step_id = NULL;
      break;
    case 1:
      if (active_project -> sqw_q_id) g_free (active_project -> sqw_q_id);
      active_project -> sqw_q_id = NULL;
      break;
  }
  if (val)
  {
    t_q_vbox[calc] = create_vbox(0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, sktqw_delta[calc], t_q_vbox[calc], FALSE, FALSE, 0);
    int delta_t;
    double delta_q;
    if (! calc)
    {
      active_project -> skt_n_data_sets = min (val, active_project -> steps - active_project -> skt_corr_threshold);
      delta_t = (active_project -> steps - active_project -> skt_corr_threshold) / active_project -> skt_n_data_sets;
      active_project -> skt_step_id = allocint (active_project -> skt_n_data_sets);
    }
    else
    {
      active_project -> sqw_n_data_sets = val;
      active_project -> sqw_q_id = allocdouble (active_project -> sqw_n_data_sets);
      delta_q = (active_project -> analysis[SKT] -> max - active_project -> analysis[SKT] -> min)/active_project -> sqw_n_data_sets;

    }
    int i, j;
    GtkWidget * hbox;
    GtkWidget * entry;
    j = (! calc) ? active_project -> skt_n_data_sets : active_project -> sqw_n_data_sets;
    for (i=0; i<j; i++)
    {
      if (! calc)
      {
        active_project -> skt_step_id[i] = 1 + i*delta_t;
      }
      else
      {
        active_project -> sqw_q_id[i] = active_project -> analysis[SKT] -> min + i*delta_q;
      }
      hbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, t_q_vbox[calc], hbox, FALSE, FALSE, 0);
      if (! calc)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (g_strdup_printf("%d)\t&#x3b4;t= ", i+1), 150, -1, 0.75, 0.5), FALSE, FALSE, 10);
        entry = create_entry (G_CALLBACK(set_skt_step_id), 100, 15, FALSE, GINT_TO_POINTER(i));
        update_entry_int (GTK_ENTRY(entry), active_project -> skt_step_id[i]);
      }
      else
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (g_strdup_printf("%d)\tq= ", i+1), 150, -1, 0.75, 0.5), FALSE, FALSE, 10);
        entry = create_entry (G_CALLBACK(set_sqw_q_id), 100, 15, FALSE, GINT_TO_POINTER(i));
        update_entry_double (GTK_ENTRY(entry), active_project -> sqw_q_id[i]);
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
    }
    show_the_widgets (sktqw_delta[calc]);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_t_q_spin (GtkSpinButton * res, gpointer data)

  \brief set the number of t steps results to save - spin button

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_t_q_spin (GtkSpinButton * res, gpointer data)
{
  if (! preferences) add_remove_t_steps_q_vectors (gtk_spin_button_get_value_as_int(res), GPOINTER_TO_INT(data));
}

/*!
  \fn void add_correlations_options (int cid)

  \brief add correlation(s) options to the S(k,t) calculation dialog

  \param cid calculation ID: 0 = S(k,t), 1 = S(q,w)
*/
void add_correlations_options (int cid)
{
  sktqw_delta[cid] = create_vbox(0);
  t_q_vbox[cid] = NULL;
  add_box_child_start (GTK_ORIENTATION_VERTICAL, sktqw_vbox[cid], sktqw_delta[cid], FALSE, FALSE, 0);
  gchar * aco_info = g_strdup_printf ("%s", (! cid) ? _("Number of <b>&#x3b4;t</b> results to save") : _("Number of <b>q</b> vectors to analyze"));
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, sktqw_delta[cid], hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (aco_info, 150, -1, 0.0, 0.5), FALSE, FALSE, 10);
  g_free (aco_info);
  int val, vbl;
  if (! cid)
  {
    val = (preferences) ? tmp_skt_n_sets : active_project -> skt_n_data_sets;
    vbl = (preferences) ? 100 : active_project -> steps-active_project -> skt_corr_threshold;
  }
  else
  {
    val = (preferences) ? tmp_sqw_n_sets : active_project -> sqw_n_data_sets;
    vbl = 100;
  }
  GtkWidget * spin = spin_button (G_CALLBACK(set_t_q_spin), val, 1.0, vbl, 1.0, 0, 100, GINT_TO_POINTER(cid));
  gtk_widget_set_size_request (spin, 25, -1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, spin, FALSE, FALSE, 10);
  show_the_widgets (sktqw_vbox[cid]);
}

/*!
  \fn G_MODULE_EXPORT void set_correlations (GtkEntry * entry, gpointer data)

  \brief set the minimum number of correlated configuration to compute S(k,t)

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_correlations (GtkEntry * entry, gpointer data)
{
  const gchar * m;
  m = entry_get_text (entry);
  int v = (int) string_to_double ((gpointer)m);
  int pid = GPOINTER_TO_INT(data);
  switch (pid)
  {
    case 0:
      if (v <= 0 || v >= active_project -> steps-1)
      {
        show_warning (_("This value must be &#62; 0 and &#60; number of steps"), calc_win);
      }
      else
      {
        active_project -> skt_corr_threshold = v;
        gchar * str = g_strdup_printf (_("%d calculation results in total !"), active_project -> analysis[SKD] -> numc * (active_project -> steps - active_project -> skt_corr_threshold));
        skt_all_info = destroy_this_widget (skt_all_info);
        skt_all_info = markup_label (str, -1, -1, 0.5, 0.5);
        g_free (str);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, skt_res_info, skt_all_info, FALSE, FALSE, 5);
        widget_set_sensitive (skt_all_info, active_project -> skt_all_sets);
        add_remove_t_steps_q_vectors (active_project -> skt_n_data_sets, 0);
        show_the_widgets (skt_res_info);
      }
      update_entry_int (entry, active_project -> skt_corr_threshold);
      break;
    case 1:
      if (v <= 0)
      {
        show_warning (_("This value must be > 0"), calc_win);
      }
      else
      {
        if (preferences)
        {
          tmp_sqw_freq = v;
        }
        else
        {
          active_project -> sqw_freq = v;
        }
      }
      update_entry_int (entry, (preferences) ? tmp_sqw_freq : active_project -> sqw_freq);
      break;
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggle_skt_all (GtkCheckButton * but, gpointer data)

  \brief toggle the output of all s(k,t) data sets

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_skt_all (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggle_skt_all (GtkToggleButton * but, gpointer data)

  \brief toggle the output of all s(k,t) data sets

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_skt_all (GtkToggleButton * but, gpointer data)
#endif
{
  gboolean status = button_get_status ((GtkWidget *)but);
  if (! preferences)
  {
    active_project -> skt_all_sets = status;
  }
  else
  {
    tmp_skt_sets = status;
  }
  widget_set_sensitive (sktqw_delta[0], ! status);
  if (! preferences)
  {
    widget_set_sensitive (skt_all_info, status);
    if (status)
    {
      hide_the_widgets (sktqw_delta[0]);
      if (active_project -> skt_step_id) g_free (active_project -> skt_step_id);
      active_project -> skt_step_id = NULL;
      active_project -> skt_n_data_sets = 1;
      active_project -> skt_step_id = allocint (1);
      active_project -> skt_step_id[0] = -1;
    }
    else
    {
      active_project -> skt_corr_threshold = (active_project -> steps < 20) ? 1 : 10;
      active_project -> skt_n_data_sets = min (5, active_project -> steps);
      add_remove_t_steps_q_vectors (active_project -> skt_n_data_sets, 0);
      show_the_widgets (sktqw_delta[0]);
    }
  }

}

/*!
  \fn void calc_sk_t (GtkWidget * box)

  \brief creation of the s(k,t) calculation widgets

  \param box GtkWidget that will receive the data
*/
void calc_sk_t (GtkWidget * box)
{
  skadv[0].a = skadv[1].a = SKT;
  skadv[0].b = 0;
  skadv[1].b = 1;

  GtkWidget * vbox = create_vbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, vbox, FALSE, FALSE, 0);
  GtkWidget * hbox_skt = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox_skt, FALSE, FALSE, 5);
  GtkWidget * vbox_skt[2];

  int i, j;
  j = (preferences) ? 1 : 2;
  for (i=0; i<j; i++)
  {
    vbox_skt[i] = create_vbox(BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox_skt, vbox_skt[i], FALSE, FALSE, 10);
  }

  if (! preferences)
  {
    if (! active_project -> skt_step_id)
    {
      active_project -> skt_n_data_sets = default_skt_n_sets;
    }
    if (! active_project -> sqw_q_id)
    {
      active_project -> sqw_n_data_sets = default_sqw_n_sets;
    }
  }

  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[0], hbox, FALSE, FALSE, 0);

  gchar * str = g_strdup_printf ("%s", _("Number of &#x3b4;q steps"));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 150, -1, 0.0, 0.5), FALSE, FALSE, 5);
  g_free (str);
  GtkWidget * entry = create_entry (G_CALLBACK(set_delta), 100, 15, FALSE, GINT_TO_POINTER(SKT));
  update_entry_int (GTK_ENTRY(entry), (preferences) ? tmp_num_delta[SKT-2] : active_project -> analysis[SKT] -> num_delta);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);

  if (! preferences)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[0], hbox, FALSE, FALSE, 5);
    str = g_strdup_printf ("%s", _("Q<sub>max</sub> [&#xC5;<sup>-1</sup>]"));
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 150, -1, 0.0, 0.5), FALSE, FALSE, 5);
    g_free (str);
    entry= create_entry (G_CALLBACK(set_max), 100, 15, FALSE, GINT_TO_POINTER(SKT));
    update_entry_double (GTK_ENTRY(entry), active_project -> analysis[SKT] -> max);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);

    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[0], hbox_note (1, active_project -> analysis[SKT] -> min), FALSE, FALSE, 5);
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[0], hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                         markup_label (_("Q<sub>min</sub> is the minimum wave vector for the model"), -1, -1, 0.0, 0.5),
                         FALSE, FALSE, 5);

    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[0], hbox, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("&#x3b4;t<sub>min</sub>", 150, -1, 0.0, 0.5), FALSE, FALSE, 5);
    entry = create_entry (G_CALLBACK(set_correlations), 100, 15, FALSE, GINT_TO_POINTER(0));
    update_entry_int (GTK_ENTRY(entry), active_project -> skt_corr_threshold);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[0], hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                         markup_label (_("&#x3b4;t<sub>min</sub> is the correlation threshold"), -1, -1, 0.0, 0.5),
                         FALSE, FALSE, 5);
  }

  if (! preferences)
  {
    calc_msd (vbox_skt[0], SKT);
  }
  else
  {
    dyna_parameters (vbox_skt[0], SKT-2);
  }
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[0], hbox, FALSE, FALSE, 5);
  str = g_strdup_printf ("%s", _("&#969; points in [0.0, &#969;<sub>max</sub>]"));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 150, -1, 0.0, 0.5), FALSE, FALSE, 5);
  g_free (str);
  entry = create_entry (G_CALLBACK(set_correlations), 100, 15, FALSE, GINT_TO_POINTER(1));
  update_entry_int (GTK_ENTRY(entry), (preferences) ? tmp_sqw_freq : active_project -> sqw_freq);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
  omega_max_hbox = destroy_this_widget(omega_max_hbox);
  omega_max_hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[0], omega_max_hbox, FALSE, FALSE, 5);
  update_omega_max ();
  if (! preferences)
  {
    add_advanced_options (SKT, skadv, vbox_skt[0]);
    add_smoothing_options (SKT, vbox_skt[0]);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[(preferences) ? 0 : 1], markup_label (_("<b>Results</b>"), -1, -1, 0.0, 0.5), FALSE, FALSE, (preferences) ? 10 : 0);
  GtkWidget * tbox = create_vbox(0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[(preferences) ? 0 : 1], tbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, tbox, markup_label (_("<b>atomes</b> cannot yet display 3D results,"), -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, tbox, markup_label (_("but we are working to add this feature !"), -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, tbox, markup_label (_("For the time being you need to select"), -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, tbox, markup_label (_("what to display using the 2D graph system:"), -1, -1, 0.5, 0.5), FALSE, FALSE, 0);

  GtkWidget * notebook = gtk_notebook_new ();
#ifdef GTK4
  gtk_widget_set_vexpand (notebook, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox_skt[(preferences) ? 0 : 1], notebook, TRUE, TRUE, (preferences) ? 10 : 5);

  GtkWidget * nbox;
  GtkWidget * scroll;
  for (i=0; i<2; i++)
  {
    scroll = create_scroll (NULL, 200, (preferences) ? 100 : 350, GTK_SHADOW_NONE);
    nbox = create_vbox(BSEP);
    add_container_child (CONTAINER_SCR, scroll, nbox);
    if (! i)
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, nbox, check_button(_("All <b>&#x3b4;t</b> correlated calculations"), -1, -1, FALSE, G_CALLBACK(toggle_skt_all), NULL), FALSE, FALSE, 5);
      if (! preferences)
      {
        str = g_strdup_printf (_("%d calculation results in total !"), active_project -> analysis[SKD] -> numc * (active_project -> steps - active_project -> skt_corr_threshold));
        skt_all_info = markup_label (str, -1, -1, 0.5, 0.5);
        g_free (str);
        skt_res_info = create_hbox (0);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, nbox, skt_res_info, FALSE, FALSE, 5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, skt_res_info, skt_all_info, FALSE, FALSE, 5);
      }
    }
    sktqw_vbox[i] = create_vbox(0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, nbox, sktqw_vbox[i], FALSE, FALSE, 0);
    if (! i) widget_set_sensitive (sktqw_vbox[i], (preferences) ? ! tmp_skt_sets : ! active_project -> skt_all_sets);
    add_correlations_options (i);
    if (! preferences)
    {
      if (! i && ! active_project -> skt_all_sets)
      {
        add_remove_t_steps_q_vectors (active_project -> skt_n_data_sets, 0);
      }
      else if (i)
      {
        add_remove_t_steps_q_vectors (active_project -> sqw_n_data_sets, 1);
      }
    }
    if (! i) widget_set_sensitive(skt_all_info, (preferences) ? tmp_skt_sets : active_project -> skt_all_sets);
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), scroll,
                              markup_label((! i) ? _("<b>Intermediate scattering</b>: F(q,&#x3b4;t)") : _("<b>Dynamic calculations</b>: S(q,&#969;)"), -1, -1, 0.0, 0.5));
  }
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
        case GDR:
          if (test_gr (GDR)) on_calc_gr_released (calc_win, NULL);
          break;
        case SQD:
          if (test_sq (SQD)) on_calc_sq_released (calc_win, NULL);
          break;
        case SKD:
          if (test_sq (SKD)) on_calc_sk_released (calc_win, NULL);
          break;
        case GDK:
          if (test_gr (GDK)) on_calc_gq_released (calc_win, NULL);
          break;
        case BND:
          if (test_bonds ()) on_calc_bonds_released (calc_win, NULL);
          break;
        case RIN-1:
          if (test_rings ())
          {
            //show_the_widgets (spinner);
            //gtk_spinner_start (GTK_SPINNER(spinner));
            on_calc_rings_released (calc_win, NULL);
            //gtk_spinner_stop (GTK_SPINNER(spinner));
            //hide_the_widgets (spinner);
          }
          break;
        case CHA-1:
          if (test_rings ())
          {
            //show_the_widgets (spinner);
            //gtk_spinner_start (GTK_SPINNER(spinner));
            on_calc_chains_released (calc_win, NULL);
            //gtk_spinner_stop (GTK_SPINNER(spinner));
            //hide_the_widgets (spinner);
          }
          break;
        case SPH-1:
          if (test_sph ()) on_calc_sph_released (calc_win, NULL);
          break;
        case MSD-1:
          if (test_msd ()) on_calc_msd_released (calc_win, NULL);
          break;
        case SKT-1:
          if (test_skt ()) on_calc_skt_released (calc_win, NULL);
        default:
          break;
      }
      break;
    default:
      frag_update = mol_update = 0;
      for (i=0; i<3; i++) active_project -> runc[i] = FALSE;
      if (id == SKT-1)
      {
        omega_max_info = destroy_this_widget (omega_max_info);
        omega_max_hbox = destroy_this_widget (omega_max_hbox);
        skt_all_info = destroy_this_widget (skt_all_info);
      }
      avbox = destroy_this_widget (avbox);
      destroy_this_dialog (dial);
      calc_win = destroy_this_widget (calc_win);
      break;
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
    case BND:
      calc_bonds (box);
      break;
    case RIN-1:
      search_type = 0;
      calc_rings (box);
      break;
    case CHA-1:
      search_type = 1;
      calc_rings (box);
      break;
    case SPH-1:
      calc_sph (box);
      break;
    case MSD-1:
      calc_msd (box, MSD);
      break;
    case SKT-1:
      calc_sk_t (box);
      break;
    default:
      calc_gr_sq (box, id);
      break;
  }

#ifndef GTK4
  gtk_window_set_icon (GTK_WINDOW (calc_win), gdk_pixbuf_new_from_file(graph_img[(id < ANG) ? id : id+1], NULL));
#endif
  GtkWidget * vbox = create_vbox (BSEP);
  gtk_widget_set_size_request (vbox, -1, 30);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, vbox, FALSE, FALSE, 0);
  run_this_gtk_dialog (calc_win, G_CALLBACK(run_on_calc_activate), data);
}
