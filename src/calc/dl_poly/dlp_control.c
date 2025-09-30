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
* @file dlp_control.c
* @short Functions to handle the creation of the DL-POLY CONTROL file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_control.c'
*
* Contains:
*

 - The functions to handle the creation of the DL-POLY CONTROL file

*
* List of functions:

  G_MODULE_EXPORT void set_order (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_thermo_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void check_nvs (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_nvs (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void check_semi (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_semi (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_thermostat (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_ensemble (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_md_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void show_advance_time_step (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void set_md_combo (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void check_impact (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_impact (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void show_impact_dialog (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void set_equi_combo (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_equi_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void check_equi (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_equi (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_out_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void check_out (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_out (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_print_level (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_ana_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void check_ana (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_ana (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_io_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void check_io (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_io (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_io_method (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_elec_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void adjust_precision (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_elec_eval (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void check_elec (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_elec (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_vdw_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_vdw_mix (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void check_vdw (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_vdw (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void check_met (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_met (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_sys_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_sys_restart (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void check_sys (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void check_sys (GtkToggleButton * but, gpointer data);

  GtkWidget * create_thermo_options (int ensemble, int thermo);
  GtkWidget * create_thermo_box (int ensemble);
  GtkWidget * create_ensemble_box ();
  GtkWidget * create_md_box ();
  GtkWidget * create_equi_box ();
  GtkWidget * create_traj_box ();
  GtkWidget * create_dump_box ();
  GtkWidget * create_out_box ();
  GtkWidget * create_overall_box ();
  GtkWidget * create_analyze_box ();
  GtkWidget * create_job_box ();
  GtkWidget * create_io_box ();
  GtkWidget * create_misc_box ();
  GtkWidget * create_elec_param_box ();
  GtkWidget * create_electro_box ();
  GtkWidget * create_vdws_box ();
  GtkWidget * create_metal_box ();
  GtkWidget * create_sys_box ();
  GtkWidget * create_restart_box ();
  GtkWidget * vbox_control (int f);

*/

#include "dlp_field.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"
#include "bind.h"

extern ColRGBA init_color (int id, int numid);
extern gboolean print_ana ();

gchar * celemts[MAXDATC] = {"System information",
                            "Calculation details",
                            "Non-bonded interactions",
                            "Equilibration",
                            "Thermodynamics",
                            "Molecular dynamics",
                            "Output options",
                            "Computational details"};

gchar * celets[MAXDATC] = {"system information",
                           "calculation details",
                           "non-bonded interactions",
                           "equilibration",
                           "thermodynamics",
                           "molecular dynamics",
                           "output options",
                           "computational details"};

#define DLP_ENS 4
#define DLP_ENS_TYPE 10
gchar * md_ensemble[DLP_ENS] = {"NVE", "NVT", "NPT", "NST"};
gchar * md_thermo[DLP_ENS_TYPE] = {"Evans", "Langevin", "Andersen", "Berendsen", "Nosë-Hoover", "Gentle Stochastic", "DPD", "Martyna-Tuckerman-Klein", "Two temperature model", "Inhomogeneous Langevin"};

// In the following, 0 = no thermo, > 0 = thermo and num of opts + 1
int md_ens_opt[DLP_ENS][DLP_ENS_TYPE] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                         {1, 2, 3, 2, 2, 3, 3, 0, 4, 4},
                                         {0, 3, 0, 3, 3, 0, 0, 3, 0, 0},
                                         {0, 3, 0, 3, 3, 0, 0, 3, 0, 0}};

gchar * opts_nvt[9][3]={{NULL, NULL, NULL},
                        {"Relaxation speed constant (friction):", NULL, NULL},
                        {"Relaxation time:", "Softness [0.0 - 1.0]:", NULL},
                        {"Relaxation constant:", NULL, NULL},
                        {"Relaxation constant:", NULL, NULL},
                        {"Relaxation constant:", "Langevin friction:", NULL},
                        {"Shardlow's splitting order:", "Global drag coefficient:", NULL},
                        {"Relaxation constant:", "Enhancement of relaxation constant:", "Cut-off particle velocity for friction enhancement:"},
                        {"Relaxation constant:", "Enhancement of relaxation constant:", "Cut-off particle velocity for friction enhancement:"}};

gchar * unit_nvt[9][3]={{NULL, NULL, NULL},
                        {"<b>ps<sup>-1</sup></b>", NULL, NULL},
                        {"<b>ps</b>", NULL, NULL},
                        {"<b>ps</b>", NULL, NULL},
                        {"<b>ps</b>", NULL, NULL},
                        {"<b>ps</b>", "<b>ps<sup>-1</sup></b>", NULL},
                        {NULL, "<b>Dalton ps<sup>-1</sup></b>", NULL},
                        {"<b>ps<sup>-1</sup></b>", "<b>ps<sup>-1</sup></b>", "<b>&#xC5; ps<sup>-1</sup></b>"},
                        {"<b>ps<sup>-1</sup></b>", "<b>ps<sup>-1</sup></b>", "<b>&#xC5; ps<sup>-1</sup></b>"}};

gchar * opts_npt_nvs[2][3] = {{"Thermostat relaxation speed constant (friction):", "Barostat relaxation speed constant (friction):", NULL},
                              {"Thermostat relaxation time:", "Barostat relaxation time:", "Target surface tension:"}};

gchar * unit_npt_nvs[2][3] = {{"<b>ps<sup>-1</sup></b>", "<b>ps<sup>-1</sup></b>", NULL}, {"<b>ps</b>", "<b>ps</b>", "<b>dyn cm<sup>-1</sup></b>"}};

gchar * extra_nvs[4] = {"Standard", "Area", "Tension", "Orthorhombic"};

GtkWidget * check_nvs_butt[4];
GtkWidget * nvs_label;
GtkWidget * nvs_entry;
GtkWidget * nvs_unit;
GtkWidget * nvs_check[2];
GtkWidget * o_ent_vbox;
GtkWidget * o_vbox;
GtkWidget * e_vbox;
GtkWidget * ens_box;
GtkWidget * bath_box;
GtkWidget * thermo_option_box;

/*!
  \fn G_MODULE_EXPORT void set_order (GtkComboBox * box, gpointer data)

  \brief CONTROL file change thermostat order

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_order (GtkComboBox * box, gpointer data)
{
  tmp_field -> thermo_opts[0] = combo_get_active ((GtkWidget *)box);
}

/*!
  \fn G_MODULE_EXPORT void set_thermo_param (GtkEntry * res, gpointer data)

  \brief CONTROL file thermostat update parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_thermo_param (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i == 9)
  {
    if ((v == 0.0 || v >= 1.0) && v != tmp_field -> thermo_opts[i])
    {
      tmp_field -> thermo_opts[i] = v;
    }
    else if (v < 1.0 && v != 0.0)
    {
      show_warning ("Target temperature for the pseudo bath must be &#8805; 1.0 K,\n"
                    "Alternatively set value to 0.0 K to system temperature.", field_assistant);
    }
  }
  else if (v >= 0.0 && v != tmp_field -> thermo_opts[i])
  {
    tmp_field -> thermo_opts[i] = v;
  }
  update_entry_double(res, tmp_field -> thermo_opts[i]);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_nvs (GtkCheckButton * but, gpointer data)

  \brief CONTROL file thermostat change parameter toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_nvs (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_nvs (GtkToggleButton * but, gpointer data)

  \brief CONTROL file thermostat change parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_nvs (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  tmp_field -> thermo_opts[3] = (double) i;
  for (j=0; j<2; j++)
  {
    widget_set_sensitive (nvs_check[j], FALSE);
  }
  for (j=0; j<4; j++)
  {
    if (j != i)
    {
      if (j == 2)
      {
        update_entry_text (GTK_ENTRY(nvs_entry), "");
        widget_set_sensitive (nvs_label, FALSE);
        widget_set_sensitive (nvs_entry, FALSE);
        widget_set_sensitive (nvs_unit,  FALSE);
      }
    }
    else if (j == i && i == 2)
    {
      widget_set_sensitive (nvs_label, TRUE);
      widget_set_sensitive (nvs_entry, TRUE);
      widget_set_sensitive (nvs_unit,  TRUE);
      update_entry_double (GTK_ENTRY(nvs_entry), 0.0);
      tmp_field -> thermo_opts[4] = 0.0;
      widget_set_sensitive (nvs_check[0], TRUE);
    }
    else if (j == i && i == 3)
    {
      widget_set_sensitive (nvs_check[1], TRUE);
    }
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_semi (GtkCheckButton * but, gpointer data)

  \brief CONTROL file thermostat change parameter toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_semi (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_semi (GtkToggleButton * but, gpointer data)

  \brief CONTROL file thermostat change parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_semi (GtkToggleButton * but, gpointer data)
#endif
{
  int i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  tmp_field -> thermo_opts[i] = j;
  if (i == 6) widget_set_sensitive (bath_box, j);
}

/*!
  \fn GtkWidget * create_thermo_options (int ensemble, int thermo)

  \brief CONTROL file create thermostat option box

  \param ensemble the thermodynamic ensemble
  \param thermo the type of thermostat
*/
GtkWidget * create_thermo_options (int ensemble, int thermo)
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox;
  GtkWidget * entry;
  gboolean val;
  int i, j;
  switch (ensemble)
  {
    case 1:
      for (i=0; i<3; i++)
      {
        if (opts_nvt[thermo][i] != NULL)
        {
          hbox = create_hbox (5);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(opts_nvt[thermo][i], 350, -1, 1.0, 0.5), FALSE, FALSE, 50);
          if (thermo == 6 && i == 0)
          {
            GtkWidget * o_combo = create_combo();
            combo_text_append (o_combo, "s1");
            combo_text_append (o_combo, "s2");
            combo_set_active (o_combo, (int)tmp_field -> thermo_opts[i]);
            g_signal_connect (G_OBJECT (o_combo), "changed", G_CALLBACK(set_order), NULL);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, o_combo, FALSE, FALSE, 5);
          }
          else
          {
            entry = create_entry(G_CALLBACK(set_thermo_param), 100, 10, FALSE, GINT_TO_POINTER(i));
            update_entry_double (GTK_ENTRY(entry), tmp_field -> thermo_opts[i]);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
          }
          if (unit_nvt[thermo][i] != NULL) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(unit_nvt[thermo][i], 20, -1, 0.0, 0.5), FALSE, FALSE, 10);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
        }
      }
      break;
    default:
      for (i=0; i<2; i++)
      {
        hbox = create_hbox (5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(opts_npt_nvs[(thermo) ? 1 : 0][i], 310, -1, 1.0, 0.5), FALSE, FALSE, 50);
        entry = create_entry(G_CALLBACK(set_thermo_param), 100, 10, FALSE, GINT_TO_POINTER(i));
        update_entry_double (GTK_ENTRY(entry), tmp_field -> thermo_opts[i]);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(unit_npt_nvs[(thermo) ? 1 : 0][i], 20, -1, 0.0, 0.5), FALSE, FALSE, 0);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
      }
      if (ensemble == 3)
      {
        GtkWidget * hhbox = create_hbox (0);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hhbox, FALSE, FALSE, 0);
        GtkWidget * vvbox = create_vbox (5);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, hhbox, vvbox, FALSE, FALSE, 50);
        gtk_box_set_homogeneous (GTK_BOX (vvbox), TRUE);
        for (i=0; i<4; i++)
        {
          hbox = create_hbox (5);
          val = ((int)tmp_field -> thermo_opts[3] == i) ? TRUE : FALSE;
#ifdef GTK4
          check_nvs_butt[i] = check_button (extra_nvs[i], 100, -1, val, G_CALLBACK(check_nvs), GINT_TO_POINTER(i));
          if (i)
          {
            gtk_check_button_set_group ((GtkCheckButton *)extra_nvs[i], (GtkCheckButton *)extra_nvs[0]);
          }
#else
          if (! i)
          {
            check_nvs_butt[i] = radio_button (extra_nvs[i], 100, -1, val, G_CALLBACK(check_nvs), GINT_TO_POINTER(i));
          }
          else
          {
            check_nvs_butt[i] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(check_nvs_butt[0]), extra_nvs[i]);
          }
#endif
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_nvs_butt[i], FALSE, FALSE, 20);
          if (i == 2)
          {
            nvs_label = markup_label(opts_npt_nvs[1][2], 150, -1, 0.0, 0.5);
            widget_set_sensitive (nvs_label, val);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, nvs_label, FALSE, FALSE, 0);
            j = 4;
            nvs_entry = create_entry(G_CALLBACK(set_thermo_param), 100, 10, FALSE, GINT_TO_POINTER(j));
            widget_set_sensitive (nvs_entry, val);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, nvs_entry, FALSE, FALSE, 10);
            if (val) update_entry_double (GTK_ENTRY(nvs_entry), tmp_field -> thermo_opts[4]);
            nvs_unit = markup_label(unit_npt_nvs[1][2], 50, -1, 0.0, 0.5);
            widget_set_sensitive (nvs_unit, val);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, nvs_unit, FALSE, FALSE, 0);
          }
          if (i > 1)
          {
            val = (((int)tmp_field -> thermo_opts[3] == i) && tmp_field -> thermo_opts[5] == 1.0) ? TRUE : FALSE;
            nvs_check[i-2] = check_button("Semi-anisotropic constraint", 200, -1, val, G_CALLBACK(check_semi), GINT_TO_POINTER(5));
            widget_set_sensitive (nvs_check[i-2], val);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, nvs_check[i-2], FALSE, FALSE, 10);
          }
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hbox, FALSE, FALSE, 0);
        }
      }
      break;
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void set_thermostat (GtkComboBox * box, gpointer data)

  \brief CONTROL file change thermostat option

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_thermostat (GtkComboBox * box, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  switch (i)
  {
    case 0:
      tmp_field -> thermostat = combo_get_active ((GtkWidget *)box);
      for (j=0; j<6; j++) tmp_field -> thermo_opts[j] = 0.0;
      if (tmp_field -> ensemble)
      {
        o_ent_vbox = destroy_this_widget (o_ent_vbox);
        o_ent_vbox = create_thermo_options (tmp_field -> ensemble, tmp_field -> thermostat);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, o_vbox, o_ent_vbox, FALSE, FALSE, 0);
        show_the_widgets (o_ent_vbox);
      }
      break;
    case 1:
      tmp_field -> thermo_opts[7] = combo_get_active ((GtkWidget *)box);
      break;
  }
}

/*!
  \fn GtkWidget * create_thermo_box (int ensemble)

  \brief CONTROL file create thermostat box parameters

  \param ensemble the target thermodynamic ensemble
*/
GtkWidget * create_thermo_box (int ensemble)
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Thermostat:", 100, -1, 0.0, 0.5), FALSE, FALSE, 30);
  GtkWidget * thermo_box = create_combo();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, thermo_box, FALSE, FALSE, 0);
  int i;
  for (i=0; i<DLP_ENS_TYPE; i++)
  {
    if (md_ens_opt[ensemble][i]) combo_text_append (thermo_box, md_thermo[i]);
  }
  combo_set_active (thermo_box, tmp_field -> thermostat);
  g_signal_connect (G_OBJECT (thermo_box), "changed", G_CALLBACK(set_thermostat), GINT_TO_POINTER(0));
  if (ensemble)
  {
    o_vbox = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, o_vbox, FALSE, FALSE, 0);
    o_ent_vbox = create_thermo_options (ensemble, tmp_field -> thermostat);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, o_vbox, o_ent_vbox, FALSE, FALSE, 0);
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void set_ensemble (GtkComboBox * box, gpointer data)

  \brief CONTROL file change thermodynamic ensemble parameter

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_ensemble (GtkComboBox * box, gpointer data)
{
  int i;
  if (tmp_field -> ensemble)
  {
    for (i=0; i<4; i++)
    {
      check_nvs_butt[i] = destroy_this_widget (check_nvs_butt[i]);
    }
    thermo_option_box = destroy_this_widget (thermo_option_box);
  }
  tmp_field -> ensemble = combo_get_active ((GtkWidget *)box);
  tmp_field -> thermostat = 0;

  for (i=0; i<6; i++) tmp_field -> thermo_opts[i] = 0.0;
  if (tmp_field -> ensemble)
  {
    thermo_option_box = create_thermo_box (tmp_field -> ensemble);
    show_the_widgets (thermo_option_box);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, ens_box, thermo_option_box, FALSE, FALSE, 0);
  }
}

/*!
  \fn GtkWidget * create_ensemble_box ()

  \brief CONTROL file create thermodynamic ensemble parameter widgets
*/
GtkWidget * create_ensemble_box ()
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Ensemble:", 100, -1, 0.0, 0.5), FALSE, FALSE, 30);
  GtkWidget * ensemble = create_combo ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ensemble, FALSE, FALSE, 0);
  int i;
  for (i=0; i<DLP_ENS; i++)
  {
    combo_text_append (ensemble, md_ensemble[i]);
  }
  combo_set_active (ensemble, tmp_field -> ensemble);
  g_signal_connect (G_OBJECT (ensemble), "changed", G_CALLBACK(set_ensemble), NULL);

  ens_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, ens_box, FALSE, FALSE, 0);
  if (tmp_field -> ensemble)
  {
    thermo_option_box = create_thermo_box (tmp_field -> ensemble);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, ens_box, thermo_option_box, FALSE, FALSE, 0);
  }
  gtk_widget_set_size_request (vbox, -1, 390);

  gboolean val = (tmp_field -> thermo_opts[6] == 1.0) ? TRUE : FALSE;
  bath_box =  create_hbox (5);
  add_box_child_end (vbox, bath_box, FALSE, FALSE, 0);
  add_box_child_end (vbox, check_button("Use pseudo thermal bath", 100, -1, val, G_CALLBACK(check_semi), GINT_TO_POINTER(6)), FALSE, FALSE, 5);
  widget_set_sensitive (bath_box, val);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bath_box, markup_label("Thermostat type:", 150, -1, 0.0, 0.5), FALSE, FALSE, 0);
  GtkWidget * combo = create_combo();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bath_box, combo, FALSE, FALSE, 00);
  combo_text_append (combo, "Langevin + Direct");
  combo_text_append (combo, "Langevin");
  combo_text_append (combo, "Gauss");
  combo_text_append (combo, "Direct");
  combo_set_active (combo, (int)tmp_field -> thermo_opts[7]);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_thermostat), GINT_TO_POINTER(1));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bath_box, markup_label("Thickness:", 100, -1, 1.0, 0.5), FALSE, FALSE, 0);
  GtkWidget * entry = create_entry(G_CALLBACK(set_thermo_param), 100, 10, FALSE, GINT_TO_POINTER(8));
  update_entry_double (GTK_ENTRY(entry), tmp_field -> thermo_opts[8]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bath_box, entry, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bath_box, markup_label("<b>&#xC5;</b>", 30, -1, 0.0, 0.5), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bath_box, markup_label("Target T°:", 100, -1, 1.0, 0.5), FALSE, FALSE, 0);
  entry = create_entry (G_CALLBACK(set_thermo_param), 100, 10, FALSE, GINT_TO_POINTER(9));
  update_entry_double (GTK_ENTRY(entry), tmp_field -> thermo_opts[9]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bath_box, entry, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bath_box, markup_label("<b>K</b>", 30, -1, 0.0, 0.5), FALSE, FALSE, 5);
  return vbox;
}

GtkWidget * step_button;
GtkWidget * extra_vbox[2];
GtkWidget * extra_lab[4];
gchar * md_data[6] = {"Target temperature:", "Verlet integrator:", "Number of steps:", "Time step δt:", "Target pressure:", ""};
gchar * md_extra[2][4] = {{"Shake iterations limit:", "Shake tolerance:", "", ""},
                          {"Rattle iterations limit:", "Rattle tolerance:", "FIQA iterations limit:", "FIQA quaternions tolerance:"}};
gchar * md_unit[6] = {"<b>K</b>", "", "", "<b>ps</b>", "<b>katms</b>", ""};
gchar * extra_unit[4] = {"<b>cycle(s)</b>", "", "<b>cycle(s)</b>", ""};
gchar * dt_data[3] = {"Maximum time step:", "Maximum distance allowed:", "Minimum distance allowed:"};
gchar * dt_unit[3] = {"<b>ps</b>", "<b>&#xC5;</b>", "<b>&#xC5;</b>"};
gchar * md_combo[2][2] = {{"Velocity", "Leapfrog"}, {"Fixed", "Variable"}};

/*!
  \fn G_MODULE_EXPORT void set_md_param (GtkEntry * res, gpointer data)

  \brief CONTROL file update MD parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_md_param (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i == 14)
  {
    if (v >= 1.0 && v <= tmp_proj -> natomes)
    {
      tmp_field -> md_opts[i] = v;
    }
  }
  else if (v >= 0.0 && v != tmp_field -> md_opts[i])
  {
    tmp_field -> md_opts[i] = v;
  }
  if (i == 2 || i == 9 || i == 11 || i == 14 || i == 15)
  {
     update_entry_int (res, (int)tmp_field -> md_opts[i]);
  }
  else
  {
    update_entry_double (res, tmp_field -> md_opts[i]);
  }
}

/*!
  \fn G_MODULE_EXPORT void show_advance_time_step (GtkButton * but, gpointer data)

  \brief CONTROL file advanced time step - creating the dialog

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_advance_time_step (GtkButton * but, gpointer data)
{
  GtkWidget * dialog = dialogmodal ("Variable time step information", GTK_WINDOW(field_assistant));
  GtkWidget * vbox = dialog_get_content_area (dialog);
  GtkWidget * hbox;
  GtkWidget * entry;
  int i, j;
  i = 6;
  for (j=0; j<3; j++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 2);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(dt_data[j], 250, -1, 1.0, 0.5), FALSE, FALSE, 5);
    entry = create_entry (G_CALLBACK(set_md_param), 100, 10, FALSE, GINT_TO_POINTER(i+j));
    update_entry_double (GTK_ENTRY(entry), tmp_field -> md_opts[i+j]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(dt_unit[j], 30, -1, 0.0, 0.5), FALSE, FALSE, 5);
  }
  run_this_gtk_dialog (dialog, G_CALLBACK(run_destroy_dialog), NULL);
}

/*!
  \fn G_MODULE_EXPORT void set_md_combo (GtkComboBox * box, gpointer data)

  \brief CONTROL file change MD parameter

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_md_combo (GtkComboBox * box, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  tmp_field -> md_opts[i] =  combo_get_active ((GtkWidget *)box);
  if (i == 3) widget_set_sensitive (step_button, (int)tmp_field -> md_opts[i]);
  if (i == 3 && tmp_field -> md_opts[i] == 1.0) show_advance_time_step (NULL, NULL);
  if (i == 1)
  {
    gtk_widget_set_visible (extra_vbox[1], (int)tmp_field -> md_opts[1]);
    for (j=0 ;j<2; j++)
    {
      gtk_label_set_text (GTK_LABEL(extra_lab[2*j]), md_extra[(int)tmp_field -> md_opts[i]][2*j]);
      gtk_label_set_text (GTK_LABEL(extra_lab[2*j+1]), md_extra[(int)tmp_field -> md_opts[i]][2*j+1]);
    }
  }
}

GtkWidget * impact_but;
gchar * imp_dir[3] = {"on x:", "on y:", "on z:"};

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_impact (GtkCheckButton * but, gpointer data)

  \brief CONTROL file particle impact change parameter toggle callback GTK3

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_impact (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_impact (GtkToggleButton * but, gpointer data)

  \brief CONTROL file particle impact change parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_impact (GtkToggleButton * but, gpointer data)
#endif
{
  gboolean i = button_get_status ((GtkWidget *)but);
  tmp_field -> md_opts[13] = (i) ? 1.0 : 0.0;
  widget_set_sensitive (impact_but, i);
}

/*!
  \fn G_MODULE_EXPORT void show_impact_dialog (GtkButton * but, gpointer data)

  \brief CONTROL file particle impact - creating the dialog

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_impact_dialog (GtkButton * but, gpointer data)
{
  GtkWidget * dialog = dialogmodal ("Initiate impact on particle", GTK_WINDOW(field_assistant));
  GtkWidget * vbox = dialog_get_content_area (dialog);
  GtkWidget * hbox, * hhbox;
  GtkWidget * entry;
  int i, j;
  gchar * str;
  gtk_box_set_homogeneous (GTK_BOX (vbox), TRUE);
  gchar * imp_info[4] = {"Index of the particle to impact: ", "Time step of impact:", "Energy of impact:", "Direction (from center of mass):"};
  gchar * imp_unit[2] = {"<b>n<sup>th</sup> step</b>", "<b>k eV</b>"};
  gchar * imp_dir[3] = {"on x:", "on y:", "on z:"};
  for (i=0; i<4; i++)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 2);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(imp_info[i], 200, -1, 1.0, 0.5), FALSE, FALSE, 5);
    if (i < 3)
    {
      j = (i < 2) ? 5 : 10;
      entry = create_entry (G_CALLBACK(set_md_param), j*10, j + j/6, FALSE, GINT_TO_POINTER(i+14));
      if (i < 2)
      {
        update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> md_opts[i+14]);
      }
      else
      {
        update_entry_double (GTK_ENTRY(entry), tmp_field -> md_opts[i+14]);
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
      if (! i)
      {
        str = g_strdup_printf ("<b>in [1-%d]</b>", tmp_proj -> natomes);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 30, -1, 0.0, 0.5), FALSE, FALSE, 0);
        g_free (str);
      }
      else
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(imp_unit[i-1], 30, -1, 0.0, 0.5), FALSE, FALSE, 0);
      }
    }
    else
    {
      hbox = create_hbox (5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 2);
      hhbox = create_hbox (5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, FALSE, FALSE, 50);
      for (j=0; j<3; j++)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label(imp_dir[j], 50, -1, 1.0, 0.5), FALSE, FALSE, 0);
        entry = create_entry (G_CALLBACK(set_md_param), 100, 11, FALSE, GINT_TO_POINTER(j+17));
        update_entry_double (GTK_ENTRY(entry), tmp_field -> md_opts[j+17]);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, entry, FALSE, FALSE, 5);

      }
    }
  }
  run_this_gtk_dialog (dialog, G_CALLBACK(run_destroy_dialog), NULL);
}

/*!
  \fn GtkWidget * create_md_box ()

  \brief CONTROL file create molecular dynamics parameter widgets
*/
GtkWidget * create_md_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  gtk_widget_set_size_request (vbox, -1, 450);
  GtkWidget * hbox;
  GtkWidget * combo;
  GtkWidget * entry;
  int i, j, k, l;
  l = 0;
  for (i=0; i<3; i++)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
    for (j=0; j<2; j++, l++)
    {
      k = (j) ? 110 : 150;
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(md_data[2*i+j], k, -1, 1.0, 0.5), FALSE, FALSE, 5);
      if (j)
      {
        if (i < 2)
        {
          combo = create_combo ();
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 0);
          for (k=0; k<2; k++)
          {
            combo_text_append (combo, md_combo[i][k]);
          }
          combo_set_active (combo, 0);
          gtk_widget_set_size_request (combo, 100, -1);
          g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_md_combo), GINT_TO_POINTER(l));
          if (i)
          {
            l ++;
            entry = create_entry (G_CALLBACK(set_md_param), 100, 11, FALSE, GINT_TO_POINTER(l));
            update_entry_double (GTK_ENTRY(entry), tmp_field -> md_opts[l]);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(md_unit[l-1], 30, -1, 0.0, 0.5), FALSE, FALSE, 0);
          }
        }
        else
        {
          step_button = create_button ("Time step details", IMG_NONE, NULL, 100, -1, GTK_RELIEF_NORMAL, G_CALLBACK(show_advance_time_step), NULL);
          widget_set_sensitive (step_button, (int)tmp_field -> md_opts[3]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, step_button, FALSE, FALSE, 10);
        }
      }
      else
      {
        entry = create_entry (G_CALLBACK(set_md_param), 100, 11, FALSE, GINT_TO_POINTER(l));
        if (i == 1 && j == 0)
        {
          update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> md_opts[l]);
        }
        else
        {
          update_entry_double (GTK_ENTRY(entry), tmp_field -> md_opts[l]);
        }
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(md_unit[2*i+j], 60, -1, 0.0, 0.5), FALSE, FALSE, 0);
      }
    }
  }

  k = (int)tmp_field -> md_opts[1];
  l = 9;
  gchar * extra_info[2] = {"<b>Bond constraint(s) dynamics:</b>", "<b>Rotational motion of rigid body(ies):</b>"};
  for (i=0; i<2; i++)
  {
    extra_vbox[i]  = create_vbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, extra_vbox[i], FALSE, FALSE, 20);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, extra_vbox[i], markup_label(extra_info[i], 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, extra_vbox[i], hbox, FALSE, FALSE, 0);
    for (j=0; j<2; j++, l++)
    {
      extra_lab[2*i+j] = markup_label(md_extra[k][2*i+j], 150+j*100, -1, 1.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, extra_lab[2*i+j], FALSE, FALSE, 5);
      entry = create_entry (G_CALLBACK(set_md_param), 100, 11, FALSE, GINT_TO_POINTER(l));
      if (j)
      {
        update_entry_double (GTK_ENTRY(entry), tmp_field -> md_opts[l]);
      }
      else
      {
        update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> md_opts[l]);
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
      if (! j) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(extra_unit[2*i+j], 30, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }
  }
  gtk_widget_set_visible (extra_vbox[1], (int)tmp_field -> md_opts[1]);
  for (i=0 ;i<2; i++)
  {
    gtk_label_set_text (GTK_LABEL(extra_lab[2*i]), md_extra[(int)tmp_field -> md_opts[1]][2*i]);
    gtk_label_set_text (GTK_LABEL(extra_lab[2*i+1]), md_extra[(int)tmp_field -> md_opts[1]][2*i+1]);
  }

  hbox = create_hbox (5);
  add_box_child_end (vbox, hbox, FALSE, FALSE, 0);
  gboolean val = (tmp_field -> md_opts[13] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("<b>Initiate impact on particle</b>", 100, -1, val, G_CALLBACK(check_impact), NULL), FALSE, FALSE, 10);
  impact_but = create_button ("Impact details", IMG_NONE, NULL, 100, -1, GTK_RELIEF_NORMAL, G_CALLBACK(show_impact_dialog), NULL);
  widget_set_sensitive (impact_but, val);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, impact_but, FALSE, FALSE, 10);
  return vbox;
}

GtkWidget * equi_lab[6];
GtkWidget * equi_entry[3];
GtkWidget * equi_box[9];

gchar * equi_info[8]= {"Equilibrate for the first:", "Scale temperature:", "Cap forces:",
                       "Resample the momenta distribution:", "Minimize system configuration:",
                       "Optimize system configuration from start:", "Perform zero temperature (10 K) optimization", "Include equilibration data in statistics"};
gchar * equi_data[5]= {"Every:", "f<sub>max</sub>=", "During:", "Minimize:", "Optimize"};
gchar * equi_unit[3]= {"<b>step(s)</b>", "<b>k<sub><i>B</i></sub>T / &#xC5;</b>", "<b>step(s)</b>"};
gchar * equi_min[3]= {"Force", "Energy", "Distance"};
gchar * equi_minu[3]= {"<b>k<sub><i>B</i></sub>T / &#xC5;</b>", "", "<b>&#xC5;</b>"};

double equi_lim[2][3] = {{1.0, 0.0, 0.000001}, {1000.0, 0.01, 0.1}};
float init_minop[3]={50.0, 0.005, 0.005};

/*!
  \fn G_MODULE_EXPORT void set_equi_combo (GtkComboBox * box, gpointer data)

  \brief CONTROL file change equilibration parameter

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_equi_combo (GtkComboBox * box, gpointer data)
{
  int i, j, k;
  i = GPOINTER_TO_INT(data);
  tmp_field -> equi_opts[i] =  combo_get_active ((GtkWidget *)box);
  j = (i == 9) ? 11 : 14;
  k = (i == 9) ? 0 : 1;
  tmp_field -> equi_opts[j] = init_minop[(int)tmp_field -> equi_opts[i]];
  update_entry_double (GTK_ENTRY(equi_entry[2*k]), tmp_field -> equi_opts[j]);
  gtk_label_set_text (GTK_LABEL(equi_lab[4*k]), g_strdup_printf ("%s:", equi_min[(int)tmp_field -> equi_opts[i]]));
  gtk_label_set_text (GTK_LABEL(equi_lab[4*k+1]), g_strdup_printf ("<b>%s</b>", equi_minu[(int)tmp_field -> equi_opts[i]]));
  gtk_label_set_use_markup (GTK_LABEL(equi_lab[4*k+1]), TRUE);
}

/*!
  \fn G_MODULE_EXPORT void set_equi_param (GtkEntry * res, gpointer data)

  \brief CONTROL file update equilibration parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_equi_param (GtkEntry * res, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i == 10 || i == 14)
  {
    j = (i == 10) ? 9 : 13;
    if (v >= equi_lim[0][(int)tmp_field -> equi_opts[j]] && v <= equi_lim[1][(int)tmp_field -> equi_opts[j]])
    {
     tmp_field -> equi_opts[i] = v;
    }
    else
    {
      gchar * str = g_strdup_printf ("Minimization parameter must be in [ %f - %f ]",
                                     equi_lim[0][(int)tmp_field -> equi_opts[j]], equi_lim[1][(int)tmp_field -> equi_opts[j]]);
      show_warning (str, field_assistant);
      g_free (str);
    }
  }
  else if (v >= 0.0 && v != tmp_field -> equi_opts[i])
  {
    tmp_field -> equi_opts[i] = v;
  }
  if (i == 1 || i == 3 || i == 7 || i == 11)
  {
     update_entry_int (res, (int)tmp_field -> equi_opts[i]);
  }
  else
  {
    update_entry_double (res, tmp_field -> equi_opts[i]);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_equi (GtkCheckButton * but, gpointer data)

  \brief CONTROL file change equilibration parameter toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_equi (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_equi (GtkToggleButton * but, gpointer data)

  \brief CONTROL file change equilibration parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_equi (GtkToggleButton * but, gpointer data)
#endif
{
  int i, k;
  i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  k = 2*i + 2*(i/5) + i/6 - i/7;
  tmp_field -> equi_opts[k] = (j) ? 1.0 : 0.0;
  if (i < 6) widget_set_sensitive (equi_box[i], j);
}

/*!
  \fn GtkWidget * create_equi_box ()

  \brief CONTROL file create equilibration parameter widgets
*/
GtkWidget * create_equi_box ()
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox, * hhbox;
  GtkWidget * entry;
  GtkWidget * combo;
  gboolean val;
  int i, j, k, l, m;
  hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  val = (tmp_field -> equi_opts[0] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Equilibrate", 100, -1, val, G_CALLBACK(check_equi), GINT_TO_POINTER(0)), FALSE, FALSE, 5);
  equi_box[0] =  create_vbox (5);
  widget_set_sensitive (equi_box[0], val);
  gtk_box_set_homogeneous (GTK_BOX (equi_box[0]), TRUE);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, equi_box[0], FALSE, FALSE, 0);
  for (i=0; i<8; i++)
  {
    hbox = create_hbox (5);
    j = 2*i + 2*(i/5) - i/7;
    k = (i != 2) ? 5 : 10;
    l = j + 1;

    add_box_child_start (GTK_ORIENTATION_VERTICAL, equi_box[0], hbox, FALSE, FALSE, 0);

    if (i < 4)
    {
      entry = create_entry (G_CALLBACK(set_equi_param), k*10, k + k/6, FALSE, GINT_TO_POINTER(l));
      if (i == 2)
      {
        update_entry_double (GTK_ENTRY(entry), tmp_field -> equi_opts[l]);
      }
      else
      {
        update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> equi_opts[l]);
      }
    }
    if (i)
    {
      val = (tmp_field -> equi_opts[j] == 1.0) ? TRUE : FALSE;
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(equi_info[i], 300, -1, val, G_CALLBACK(check_equi), GINT_TO_POINTER(i)), FALSE, FALSE, 5);
      if (i == 4 || i == 5)
      {
        hhbox = create_hbox (5);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, equi_box[0], hhbox, FALSE, FALSE, 0);
        hbox = create_hbox (5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, hbox, FALSE, FALSE, 50);
      }
      equi_box[i] = create_hbox (5);
      widget_set_sensitive (equi_box[i], val);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, equi_box[i], FALSE, FALSE, 5);
      if (i < 6) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, equi_box[i], markup_label(equi_data[i-1], 100, -1, 0.8, 0.5), FALSE, FALSE, 0);
      if (i < 4)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, equi_box[i], entry, FALSE, FALSE, 5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, equi_box[i], markup_label(equi_unit[i-1], 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
      }
      else if (i < 6)
      {
        combo = create_combo();
        for (m=0; m<3; m++) combo_text_append (combo, equi_min[m]);
        combo_set_active (combo, (int)tmp_field -> equi_opts[l]);
        gtk_widget_set_size_request (combo, 100, -1);
        g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_equi_combo), GINT_TO_POINTER(l));
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, equi_box[i], combo, FALSE, FALSE, 5);
        for (m=i-4; m<2; m++, l++)
        {
          equi_lab[(i-4)*2+2*m] =  markup_label((i == 4 && m) ? equi_data[0] : g_strdup_printf ("%s:", equi_min[(int)tmp_field -> equi_opts[l]]), 75, -1, 0.8, 0.5);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, equi_box[i], equi_lab[(i-4)*2+2*m], FALSE, FALSE, 0);
          k = (i == 4 && m) ? 5 : 10;
          equi_entry[i-4+m] = create_entry (G_CALLBACK(set_equi_param), k*10, k + k/6, FALSE, GINT_TO_POINTER(l+1+m/2));
          if (i == 4 && m)
          {
            update_entry_int (GTK_ENTRY(equi_entry[i-4+m]), (int)tmp_field -> equi_opts[l+1+m/2]);
          }
          else
          {
            update_entry_double (GTK_ENTRY(equi_entry[i-4+m]), tmp_field -> equi_opts[l+1+m/2]);
          }
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, equi_box[i], equi_entry[i-4+m], FALSE, FALSE, 5);
          equi_lab[(i-4)*2+2*m+1] = markup_label((i == 4 && m) ? equi_unit[0] : equi_minu[(int)tmp_field -> equi_opts[l]], 50, -1, 0.0, 0.5);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, equi_box[i], equi_lab[(i-4)*2+2*m+1], FALSE, FALSE, 0);
        }
      }
    }
    else
    {
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(equi_info[i], 250, -1, 0.8, 0.5), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(equi_unit[i], 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }
  }
  return vbox;
}

gchar * traj_data[4] = {"Defects:",
                        "Displacements:",
                        "History:",
                        "Atom's MSD:"};

gchar * traj_info[4][3] = {{"From step:", "Every:", "Site interstitial cutoff:"},
                           {"From step:", "Every:", "Qualifying cutoff:"},
                           {"From step:", "Every:", "Data level:"},
                           {"From step:", "Every:", NULL}};

gchar * traj_level[3]={"0", "1", "2"};

gchar * out_print[3]={"Radial distribution functions (RDFs):",
                      "Velocity autocorrelation functions (VAFs):",
                      "Z density profile:"};

GtkWidget * out_hbox[11];
GtkWidget * out_entry[3];

/*!
  \fn G_MODULE_EXPORT void set_out_param (GtkEntry * res, gpointer data)

  \brief CONTROL file update output information parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_out_param (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i == 3 || i == 7 || i == 23)
  {
    if (v >= 0.0 && v != tmp_field -> out_opts[i])
    {
      tmp_field -> out_opts[i] = v;
    }
    update_entry_double (res, tmp_field -> out_opts[i]);
    if (i == 23)
    {
      update_entry_double (GTK_ENTRY(out_entry[0]), tmp_field -> out_opts[i]);
      update_entry_double (GTK_ENTRY(out_entry[2]), tmp_field -> out_opts[i]);
    }
  }
  else
  {
    if ((int)v != (int)tmp_field -> out_opts[i])
    {
      tmp_field -> out_opts[i] = (int)v;
    }
    update_entry_int (res, (int)tmp_field -> out_opts[i]);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_out (GtkCheckButton * but, gpointer data)

  \brief CONTROL file change output information parameter toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_out (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_out (GtkToggleButton * but, gpointer data)

  \brief CONTROL file change output information parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_out (GtkToggleButton * but, gpointer data)
#endif
{
  int i, k;
  i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  if (i < 7)
  {
    k = 4*i - i/4 - 2*(i/5) - 2*(i/6);
    tmp_field -> out_opts[k] = (j) ? 1.0 : 0.0;
    widget_set_sensitive (out_hbox[i], j);
  }
  else
  {
    tmp_field -> out_opts[i] = (j) ? 1.0 : 0.0;
    if (i < 29)
    {
      k = (i == 21) ? 7 : (i == 24) ? 8 : 9;
      widget_set_sensitive (out_hbox[k], j);
      if (i == 24) widget_set_sensitive (out_hbox[10], j);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_print_level (GtkComboBox * box, gpointer data)

  \brief CONTROL file change print level

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_print_level (GtkComboBox * box, gpointer data)
{
  tmp_field -> out_opts[12] =  combo_get_active ((GtkWidget *)box);
}

/*!
  \fn GtkWidget * create_traj_box ()

  \brief CONTROL file create MD trajectory parameter widgets
*/
GtkWidget * create_traj_box ()
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox;
  GtkWidget * combo;
  GtkWidget * entry;
  gboolean val;
  int i, j, k, l;
  for (i=0; i<4; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    k = (i < 4) ? 4*i : 15;
    val = (tmp_field -> out_opts[k] == 1.0) ? TRUE : FALSE;
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(traj_data[i], 150, -1, val, G_CALLBACK(check_out), GINT_TO_POINTER(i)), FALSE, FALSE, 5);
    out_hbox[i] = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, out_hbox[i], FALSE, FALSE, 0);
    widget_set_sensitive (out_hbox[i], val);

    for (j=0; j<3-i/3; j++)
    {
      l = (j == 2) ? 150 : 50;
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i], markup_label(traj_info[i][j], l, -1, 1.0, 0.5), FALSE, FALSE, 5);
      if (i != 2 || j != 2)
      {
        l = (j<2) ? 50 : 100;
        entry = create_entry (G_CALLBACK(set_out_param), l, l/10, FALSE, GINT_TO_POINTER(k+j+1));
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i], entry, FALSE, FALSE, 0);
        if (j == 2)
        {
          update_entry_double (GTK_ENTRY(entry), tmp_field -> out_opts[k+j+1]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i], markup_label("<b>&#xC5;</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
        }
        else
        {
          update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> out_opts[k+j+1]);
          if (j) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i], markup_label("<b>step(s)</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
        }
      }
      else
      {
        combo = create_combo();
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i], combo, FALSE, FALSE, 0);
        for (l=0; l<3; l++) combo_text_append (combo, traj_level[l]);
        combo_set_active (combo, (int)tmp_field -> out_opts[k+j+1]);
        g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_print_level), NULL);
      }
    }
  }
  return vbox;
}

/*!
  \fn GtkWidget * create_dump_box ()

  \brief CONTROL file create dump parameter widgets
*/
GtkWidget * create_dump_box ()
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox;
  GtkWidget * entry;

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Dump restart information every:", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
  entry = create_entry (G_CALLBACK(set_out_param), 50, 6, FALSE, GINT_TO_POINTER(30));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
  update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> out_opts[30]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>step(s)</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);

  return vbox;
}

gchar * out_data[3] = {"Print system data:",
                       "Accumulate statistics data:",
                       "Rolling average stack:"};

gchar * out_info[3] = {"Every:", "Every:", " "};

/*!
  \fn GtkWidget * create_out_box ()

  \brief CONTROL file create output information widgets
*/
GtkWidget * create_out_box ()
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox;
  GtkWidget * entry;
  gboolean val;
  int i, j;
  for (i=4; i<7; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    j = 2*(i-4)+15;
    val = (tmp_field -> out_opts[j] == 1.0) ? TRUE : FALSE;
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(out_data[i-4], 200, -1, val, G_CALLBACK(check_out), GINT_TO_POINTER(i)), FALSE, FALSE, 5);
    out_hbox[i] = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, out_hbox[i], FALSE, FALSE, 0);
    widget_set_sensitive (out_hbox[i], val);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i], markup_label(out_info[i-4], 50, -1, 1.0, 0.5), FALSE, FALSE, 5);
    entry = create_entry (G_CALLBACK(set_out_param), 50, 5, FALSE, GINT_TO_POINTER(j+1));
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i], entry, FALSE, FALSE, 0);
    update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> out_opts[j+1]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i], markup_label("<b>step(s)</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
  return vbox;
}

/*!
  \fn GtkWidget * create_overall_box ()

  \brief CONTROL file create overall parameter widgets
*/
GtkWidget * create_overall_box ()
{
  GtkWidget * vbox = create_vbox (5);
  gtk_box_set_homogeneous (GTK_BOX (vbox), TRUE);
  GtkWidget * hbox, * hhbox;
  GtkWidget * entry;
  gboolean val;
  int i, k, l, m, n;
  i = 6;
  for (l=0; l<3; l++)
  {
    k = 21 + 3*l;
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    val = (tmp_field -> out_opts[k] == 1.0) ? TRUE : FALSE;
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(out_print[l], 300, -1, val, G_CALLBACK(check_out), GINT_TO_POINTER(k)), FALSE, FALSE, 5);
    out_hbox[i+l+1] = create_hbox (5);
    widget_set_sensitive (out_hbox[i+l+1], val);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, out_hbox[i+l+1], FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i+l+1], markup_label("Every:", 50, -1, 1.0, 0.5), FALSE, FALSE, 5);
    entry = create_entry (G_CALLBACK(set_out_param), 50, 5, FALSE, GINT_TO_POINTER(k+1));
    update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> out_opts[k+1]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i+l+1], entry, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i+l+1], markup_label("<b>step(s)</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i+l+1], markup_label("Bin size:", 100, -1, 1.0, 0.5), FALSE, FALSE, 5);
    m = (l==1) ? k + 2 : 23;
    n = (l==1) ? 1 : 2;
    out_entry[l] = create_entry (G_CALLBACK(set_out_param), n*50, n*5, FALSE, GINT_TO_POINTER(m));
    if (l != 1)
    {
      update_entry_double (GTK_ENTRY(out_entry[l]), tmp_field -> out_opts[m]);
    }
    else
    {
      update_entry_int (GTK_ENTRY(out_entry[l]), (int)tmp_field -> out_opts[m]);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i+l+1], out_entry[l], FALSE, FALSE, 0);
    if (l != 1)
    {
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[i+l+1], markup_label("<b>&#xC5;</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }
    if (l == 1)
    {
      hbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
      out_hbox[10] = create_hbox (0);
      hhbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, FALSE, FALSE, 0);
      gtk_widget_set_size_request (hhbox, 330, -1);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, out_hbox[10], FALSE, FALSE, 0);
      val = (tmp_field -> out_opts[29] == 1.0) ? TRUE : FALSE;
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, out_hbox[10], check_button("Ignore time-averaging for the VAFs", 100, -1, val, G_CALLBACK(check_out), GINT_TO_POINTER(29)), FALSE, FALSE, 5);
      widget_set_sensitive (out_hbox[10], val);
    }
  }
  return vbox;
}

GtkWidget * ana_box[5];
gchar * ana_info[5]={"All:", "Bonds:", "Angles:", "Dihedrals:", "Inversions:"};
gchar * ana_param[3]={"Every:", "Num. δ", "Cutoff = "};

/*!
  \fn G_MODULE_EXPORT void set_ana_param (GtkEntry * res, gpointer data)

  \brief  CONTROL file update analysis parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_ana_param (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (v >= 0.0 && v != tmp_field -> ana_opts[i])
  {
    tmp_field -> ana_opts[i] = v;
  }
  if (i == 3 || i == 7)
  {
    update_entry_double (GTK_ENTRY(res), tmp_field -> ana_opts[i]);
  }
  else
  {
    update_entry_int (GTK_ENTRY(res), (int)tmp_field -> ana_opts[i]);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_ana (GtkCheckButton * but, gpointer data)

  \brief CONTROL file change analysis parameter toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_ana (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_ana (GtkToggleButton * but, gpointer data)

  \brief CONTROL file change analysis parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_ana (GtkToggleButton * but, gpointer data)
#endif
{
  int i, k;
  i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  tmp_field -> ana_opts[i] = (j) ? 1.0 : 0.0;
  switch (i)
  {
    case 14:
      k = 4;
      break;
    case 11:
      k = 3;
      break;
    default:
      k = i / 4;
      break;
  }
  widget_set_sensitive (ana_box[k], j);
}

/*!
  \fn GtkWidget * create_analyze_box ()

  \brief CONTROL file create analysis parameter widgets
*/
GtkWidget * create_analyze_box ()
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox;
  GtkWidget * entry;
  gboolean val;
  gchar * str;

  int i, j;
  for (i=0; i<5; i++)
  {
    hbox = create_hbox (5);
    j = (i < 3) ? 4*i: 11 + 3*(i-3);
    val = (tmp_field -> ana_opts[j] == 1.0) ? TRUE : FALSE;
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(ana_info[i], 100, -1, val, G_CALLBACK(check_ana), GINT_TO_POINTER(j)), FALSE, FALSE, 5);
    ana_box[i] = create_hbox (5);

    widget_set_sensitive (ana_box[i], val);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ana_box[i], FALSE, FALSE, 0);

    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], markup_label(ana_param[0], 50, -1, 1.0, 0.5), FALSE, FALSE, 5);
    entry = create_entry (G_CALLBACK(set_ana_param), 50, 5, FALSE, GINT_TO_POINTER(j+1));

    update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> ana_opts[j+1]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], entry, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], markup_label("<b>step(s)</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    if (i < 2)
    {
      str = g_strdup_printf ("%sr in [0 : r]", ana_param[1]);
    }
    else if (i == 3)
    {
      str = g_strdup_printf ("%s° in [-180 : 180]", ana_param[1]);
    }
    else
    {
      str = g_strdup_printf ("%s° in [0 : 180]", ana_param[1]);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], markup_label(str, 160, -1, 1.0, 0.5), FALSE, FALSE, 5);
    g_free (str);
    entry = create_entry (G_CALLBACK(set_ana_param), 50, 5, FALSE, GINT_TO_POINTER(j+2));
    update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> ana_opts[j+2]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], entry, FALSE, FALSE, 0);
    if (i < 2)
    {
      str = g_strdup_printf ("<b>δr</b>");
    }
    else
    {
      str = g_strdup_printf ("<b>δ°</b>");
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], markup_label(str, 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    if (i < 2)
    {
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], markup_label(ana_param[2], 50, -1, 1.0, 0.5), FALSE, FALSE, 5);
      entry = create_entry (G_CALLBACK(set_ana_param), 100, 11, FALSE, GINT_TO_POINTER(j+3));
      update_entry_double (GTK_ENTRY(entry), tmp_field -> ana_opts[j+3]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], entry, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ana_box[i], markup_label("<b>&#xC5;</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }

  }
  return vbox;
}

GtkWidget * time_box[2];
GtkWidget * io_box[2];
GtkWidget * io_pre;
GtkWidget * misc_box;
gchar * time_info[2]={"Set job time:", "Set job closure time:"};
gchar * tps_info[2]={"run time:", "closure time:"};
gchar * io_rw_m[4] = {"MPI I/O", "FORTRAN I/O", "Traditional master I/O", "netCDF I/O"};
gchar * io_info[2] = {"<b>General Input/Output (I/O) read interface:</b>", "<b>General Input/Output (I/O) write interface:</b>"};
gchar * io_para[4] = {"Reader count:", "Batch size:", "Buffer size:", "// error check"};
gchar * io_pres[2] = {"Float - 32 bit", "Double - 64 bit"};
gchar * io_type[2] = {"Sorted", "Unsorted"};
GtkWidget * io_hp[2][4];
GtkWidget * check_e[2];

/*!
  \fn G_MODULE_EXPORT void set_io_param (GtkEntry * res, gpointer data)

  \brief CONTROL file update I/O parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_io_param (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i < 2)
  {
    if (v >= 0.0 && v != tmp_field -> io_opts[2*i+1])
    {
      tmp_field -> io_opts[2*i+1] = v;
    }
    update_entry_double (GTK_ENTRY(res), tmp_field -> io_opts[2*i+1]);
  }
  else
  {
    if (i == 7 || i == 15)
    {
      if (v >= 1.0 && v <= 100000000.0)
      {
        if (v != tmp_field -> io_opts[i])
        {
         tmp_field -> io_opts[i] = v;
        }
      }
      else
      {
        show_warning ("The batch size or max. number of particles by batch\n"
                      "must be in [1 - 10 000 000]", field_assistant);
      }
    }
    else if (i == 8 || i == 16)
    {
      if (v >= 100.0 && v <= 100000.0)
      {
        if (v != tmp_field -> io_opts[i])
        {
         tmp_field -> io_opts[i] = v;
        }
      }
      else
      {
        show_warning ("The buffer size or max. number of ASCII line records by batch\n"
                      "must be in [100 - 100 000]", field_assistant);
      }
    }
    else if (i == 19 || i == 20 || i == 21)
    {
      if (v != tmp_field -> io_opts[i])
      {
        tmp_field -> io_opts[i] = v;
      }
    }
    else if (v > 0.0 && v != tmp_field -> io_opts[i])
    {
      tmp_field -> io_opts[i] = v;
    }
    update_entry_int (GTK_ENTRY(res), (int)tmp_field -> io_opts[i]);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_io (GtkCheckButton * but, gpointer data)

  \brief CONTROL file change I/O parameter toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_io (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_io (GtkToggleButton * but, gpointer data)

  \brief CONTROL file change I/O parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_io (GtkToggleButton * but, gpointer data)
#endif
{
  int i, k;
  i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  if (i < 2)
  {
    tmp_field -> io_opts[2*i] = (j) ? 1.0 : 0.0;
    widget_set_sensitive (time_box[i], j);
  }
  else
  {
    tmp_field -> io_opts[i] = (j) ? 1.0 : 0.0;
    if (i < 18)
    {
      k = (i == 4) ? 0 : 1;
      widget_set_sensitive (io_box[k], j);
    }
    else if (i == 18)
    {
      widget_set_sensitive (misc_box, j);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_io_method (GtkComboBox * box, gpointer data)

  \brief CONTROL file change I/O method

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_io_method (GtkComboBox * box, gpointer data)
{
  int i, j, k;
  gboolean l;
  i = GPOINTER_TO_INT(data);
  j = combo_get_active ((GtkWidget *)box);
  tmp_field -> io_opts[i] = (double)j;
  if (i < 12)
  {
    k = (i == 5) ? 0 : 1;
    l = (j == 2) ? FALSE : TRUE;
    widget_set_sensitive (io_hp[k][0], l);
    widget_set_sensitive (io_hp[k][2], l);
    widget_set_sensitive (check_e[k], l);
    if (i == 11)
    {
      l = (j == 3) ? TRUE : FALSE;
      widget_set_sensitive (io_pre, l);
    }
  }
}

/*!
  \fn GtkWidget * create_job_box ()

  \brief CONTROL file create job parameter vidgets
*/
GtkWidget * create_job_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * entry;
  gboolean val;
  int i;
  for (i=0; i<2; i++)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
    val = (tmp_field -> io_opts[2*i] == 1.0) ? TRUE : FALSE;
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(time_info[i], 200, -1, val, G_CALLBACK(check_io), GINT_TO_POINTER(i)), FALSE, FALSE, 5);
    time_box[i] =  create_hbox (5);
    widget_set_sensitive (time_box[i], val);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, time_box[i], FALSE, FALSE, 0);
    entry = create_entry (G_CALLBACK(set_io_param), 100, 11, FALSE, GINT_TO_POINTER(i));
    update_entry_double (GTK_ENTRY(entry), tmp_field -> io_opts[i*2+1]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, time_box[i], entry, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, time_box[i], markup_label("<b>s</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
  return vbox;
}

/*!
  \fn GtkWidget * create_io_box ()

  \brief CONTROL file create I/O parameter vidgets
*/
GtkWidget * create_io_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * combo;
  GtkWidget * entry;
  gboolean val;
  int i, j, k;
  gboolean l, m;
  k = 4;
  for (i=0; i<2; i++)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
    val = (tmp_field -> io_opts[k] == 1.0) ? TRUE : FALSE;
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(io_info[i], 200, -1, val, G_CALLBACK(check_io), GINT_TO_POINTER(k)), FALSE, FALSE, 5);
    io_box[i] = create_vbox (5);
    widget_set_sensitive (io_box[i], val);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, io_box[i], FALSE, FALSE, 0);
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, io_box[i], hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Method:", 100, -1, 1.0, 0.5), FALSE, FALSE, 5);
    combo = create_combo();
    for (j=0; j<4; j++)
    {
      combo_text_append(combo, io_rw_m[j]);
    }
    k ++;
    combo_set_active (combo, (int)tmp_field -> io_opts[k]);
    g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_io_method), GINT_TO_POINTER(k));
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 5);
    l = tmp_field -> io_opts[k] == 2.0 ? FALSE : TRUE;
    m = tmp_field -> io_opts[k] == 3.0 ? TRUE : FALSE;
    if (i)
    {
      io_pre = create_hbox(5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, io_pre, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, io_pre, markup_label("Real precision:", 150, -1, 1.0, 0.5), FALSE, FALSE, 5);
      combo = create_combo();
      for (j=0; j<2; j++) combo_text_append(combo, io_pres[j]);
      k ++;
      combo_set_active (combo, (int)tmp_field -> io_opts[k]);
      g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_io_method), GINT_TO_POINTER(k));
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, io_pre, combo, FALSE, FALSE, 5);
      widget_set_sensitive (io_pre, m);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Type:", 50, -1, 1.0, 0.5), FALSE, FALSE, 5);
      combo = create_combo();
      for (j=0; j<2; j++) combo_text_append(combo, io_type[j]);
      k ++;
      combo_set_active (combo, (int)tmp_field -> io_opts[k]);
      g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_io_method), GINT_TO_POINTER(k));
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 5);
    }
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, io_box[i], hbox, FALSE, FALSE, 0);
    for (j=0; j<3; j++)
    {
      io_hp[i][j] = create_hbox (5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, io_hp[i][j], FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, io_hp[i][j], markup_label(io_para[j], 100, -1, 1.0, 0.5), FALSE, FALSE, 5);
      k++;
      entry = create_entry (G_CALLBACK(set_io_param), 70, 8, FALSE, GINT_TO_POINTER(k));
      update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> io_opts[k]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, io_hp[i][j], entry, FALSE, FALSE, 5);
      if (j == 1) widget_set_sensitive (io_hp[i][j], l);
    }
    k++;
    check_e[i] = check_button(io_para[j], 50, -1, val, G_CALLBACK(check_io), GINT_TO_POINTER(k));
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_e[i], FALSE, FALSE, 20);
    widget_set_sensitive (check_e[i], l);
    k ++;
  }
  return vbox;
}

/*!
  \fn GtkWidget * create_misc_box ()

  \brief CONTROL file create miscalleanous parameter vidgets
*/
GtkWidget * create_misc_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * entry;
  gchar * str;
  gboolean val;
  int i;
  hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  val = (tmp_field -> io_opts[18] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Random number generator seeds:", 200, -1, val, G_CALLBACK(check_io), GINT_TO_POINTER(18)), FALSE, FALSE, 5);
  misc_box =  create_hbox (5);
  widget_set_sensitive (misc_box, val);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, misc_box, FALSE, FALSE, 0);
  for (i=19; i<22; i++)
  {
    str = g_strdup_printf ("<i>n</i><sub>%d</sub>", i-18);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, misc_box, markup_label(str, 50, -1, 1.0, 0.5), FALSE, FALSE, 5);
    g_free (str);
    entry = create_entry (G_CALLBACK(set_io_param), 70, 10, FALSE, GINT_TO_POINTER(i));
    update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> io_opts[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, misc_box, entry, FALSE, FALSE, 5);
  }
  hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  val = (tmp_field -> io_opts[22] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Limits to 2 the number of processors in <i>z</i> direction for slab simulations", 200, -1, val, G_CALLBACK(check_io), GINT_TO_POINTER(22)), FALSE, FALSE, 5);
  return vbox;
}


GtkWidget * elec_box[4];
GtkWidget * pres_spin;
gchar * eval_m[10] = {"Direct Coulomb sum", "Distance dependent dielectric Coulomb sum",
                      "Ewald sum (auto)", "Ewald sum",
                      "Reaction field electrostatics", "Reaction field with Fennel damping",
                      "Reaction field with Fennel damping (auto)",
                      "Force-shifted Coulomb sum", "Force-shifted Coulomb sum with Fennel damping",
                      "Force-shifted Coulomb sum with Fennel damping (auto)"};

/*!
  \fn G_MODULE_EXPORT void set_elec_param (GtkEntry * res, gpointer data)

  \brief  CONTROL file update electrostatic interactions parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_elec_param (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i == 1)
  {
    if (v > 0.0 && v != tmp_field -> elec_opts[i])
    {
      tmp_field -> elec_opts[i] = v;
    }
    else if (v < 0.0)
    {
      show_warning ("Cutoff must be > 0.0 &#xC5;", field_assistant);
    }
  }
  else if (i == 3)
  {
    if (v > 0.0 && v != tmp_field -> elec_opts[i])
    {
      if (v >= min(0.05, 0.5*tmp_field -> elec_opts[1]/100.0))
      {
        tmp_field -> elec_opts[i] = v;
      }
      else
      {
        show_warning ("Padding must be &#8805; min(0.05, 0.5%r<sub>cut</sub>) &#xC5;", field_assistant);
      }
    }
    else if (v < 0.0)
    {
      show_warning ("Padding must be > 0.0 &#xC5;", field_assistant);
    }
  }
  else
  {
    if (i != 6 && (v >= 0.0 && v != tmp_field -> elec_opts[i]))
    {
      tmp_field -> elec_opts[i] = v;
    }
    else if (i == 6)
    {
      if (tmp_field -> elec_opts[5] == 2.0 || tmp_field -> elec_opts[5] == 6.0 || tmp_field -> elec_opts[5] == 9.0)
      {
        int j = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(pres_spin));
        double w = v * pow (10, j);
        if ((w >= 1e-20 && w <= 0.5) && w != tmp_field -> elec_opts[i])
        {
          tmp_field -> elec_opts[i] = w;
          w = v;
        }
        else if (w < 1e-20 || w > 0.5)
        {
          show_warning ("Precision must be in [10<sup>-20</sup> - 0.5]", field_assistant);
          w = tmp_field -> elec_opts[i] / pow (10, j);
        }
      }
      else if (v >= 0.0 && v != tmp_field -> elec_opts[i])
      {
        tmp_field -> elec_opts[i] = v;
      }
    }
  }
  if (i == 10)
  {
    if (tmp_field -> elec_opts[i] == 0.0) tmp_field -> elec_opts[i] = 1.0;
    if (tmp_field -> elec_opts[i] > 10.0) tmp_field -> elec_opts[i] = 4.0;
    update_entry_int (GTK_ENTRY(res), (int)tmp_field -> elec_opts[i]);
  }
  else if (i == 7 || i == 8 || i == 9)
  {
      update_entry_int (GTK_ENTRY(res), (int)tmp_field -> elec_opts[i]);
  }
  else
  {
    if (i == 6)
    {
      update_entry_double (GTK_ENTRY(res), v);
    }
    else
    {
      update_entry_double (GTK_ENTRY(res), tmp_field -> elec_opts[i]);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void adjust_precision (GtkSpinButton * res, gpointer data)

  \brief CONTROL file update electrostatic interactions precision spin callback

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void adjust_precision (GtkSpinButton * res, gpointer data)
{
  int powa = gtk_spin_button_get_value_as_int(res);
  double v = tmp_field -> elec_opts[6];
  while (v < 0.1)
  {
    v = v*10;
  }
  v = v*10;
  tmp_field -> elec_opts[6] = v * pow(10, powa);
}

/*!
  \fn GtkWidget * create_elec_param_box ()

  \brief CONTROL file create electrostatic interactions parameter box
*/
GtkWidget * create_elec_param_box ()
{
  GtkWidget * vbox, * vvbox;
  GtkWidget * hbox, * hhbox;
  GtkWidget * entry;
  vvbox = create_hbox (5);
  vbox = create_vbox (5);
  gtk_box_set_homogeneous (GTK_BOX (vbox), TRUE);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, vvbox, vbox, FALSE, FALSE, 50);
  gchar * dir[3] = {"x:", "y:", "z:"};
  if (tmp_field -> elec_opts[5] == 2.0 || tmp_field -> elec_opts[5] == 6.0 || tmp_field -> elec_opts[5] == 9.0)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Precision:", 180, -1, 0.0, 0.5), FALSE, FALSE, 0);
    // tmp_field -> elec_opts[6] = 1e-20;
    entry = create_entry (G_CALLBACK(set_elec_param), 100, 11, FALSE, GINT_TO_POINTER(6));
    double v = tmp_field -> elec_opts[6];
    int i = -1;
    while (v < 0.1)
    {
      v = v*10;
      i --;
    }
    update_entry_double (GTK_ENTRY(entry), tmp_field -> elec_opts[6] / pow(10,i));
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(" x 10<sup>-</sup>", 5, -1, 0.0, 0.5), FALSE, FALSE, 0);
    pres_spin = spin_button (G_CALLBACK(adjust_precision), i, -20, -1, 1, 0, 15, NULL);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, pres_spin, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("in [10<sup>-20</sup> - 0.5]", 50, -1, 0.0, 0.5), FALSE, FALSE, 5);
  }
  else if (tmp_field -> elec_opts[5] == 3.0)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Ewald convergence parameter:", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
    // tmp_field -> elec_opts[6] = 1.0;
    entry = create_entry (G_CALLBACK(set_elec_param), 100, 11, FALSE, GINT_TO_POINTER(6));
    update_entry_double (GTK_ENTRY(entry), tmp_field -> elec_opts[6]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>&#xC5;<sup>-1</sup></b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 5);
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Maximum k vector index in directions:", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    int i;
    for (i=0; i<3; i++)
    {
      hhbox = create_hbox (5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, FALSE, FALSE, 40);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(dir[i], 10, -1, 0.0, 0.5), FALSE, FALSE, 0);
      // tmp_field -> elec_opts[7+i] = 1.0;
      entry = create_entry (G_CALLBACK(set_elec_param), 50, 5, FALSE, GINT_TO_POINTER(7+i));
      update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> elec_opts[7+i]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 5);
    }
  }
  else if (tmp_field -> elec_opts[5] == 5.0 || tmp_field -> elec_opts[5] == 8.0)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Fennell damping parameter (&#x3B1;):", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    entry = create_entry (G_CALLBACK(set_elec_param), 100, 11, FALSE, GINT_TO_POINTER(6));
    update_entry_double (GTK_ENTRY(entry), tmp_field -> elec_opts[6]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>&#xC5;<sup>-1</sup></b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 5);
  }
  if (tmp_field -> elec_opts[5] == 2.0 || tmp_field -> elec_opts[5] == 3.0)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Evaluate <i>k</i>-space contribution to the Ewald sum every:", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    entry = create_entry (G_CALLBACK(set_elec_param), 50, 5, FALSE, GINT_TO_POINTER(10));
    update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> elec_opts[10]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>step(s)</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 5);
  }
  return vvbox;
}

/*!
  \fn G_MODULE_EXPORT void set_elec_eval (GtkComboBox * box, gpointer data)

  \brief CONTROL file change electrostatics evaluation method

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_elec_eval (GtkComboBox * box, gpointer data)
{
  tmp_field -> elec_opts[5] =  combo_get_active ((GtkWidget *)box);
  elec_box[3] = destroy_this_widget (elec_box[3]);
  elec_box[3] = create_elec_param_box ();
  show_the_widgets (elec_box[3]);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, elec_box[2], elec_box[3], FALSE, FALSE, 10);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_elec (GtkCheckButton * but, gpointer data)

  \brief change CONTROL file electrostatics option toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_elec (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_elec (GtkToggleButton * but, gpointer data)

  \brief change CONTROL file electrostatics option toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_elec (GtkToggleButton * but, gpointer data)
#endif
{
  int i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  tmp_field -> elec_opts[i] = (j) ? 1.0 : 0.0;
  if (! i) widget_set_sensitive (elec_box[j], j);
  if (i == 2) widget_set_sensitive (elec_box[1], j);
}

/*!
  \fn GtkWidget * create_electro_box ()

  \brief CONTROL file create electrostatic interactions parameter vidgets
*/
GtkWidget * create_electro_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  gboolean val = (tmp_field -> elec_opts[0] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, check_button("Evaluate electrostatics interactions", -1, -1, val, G_CALLBACK(check_elec), GINT_TO_POINTER(0)), FALSE, FALSE, 5);
  hbox = create_hbox(0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  elec_box[0] = create_vbox(5);
  widget_set_sensitive (elec_box[0], val);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, elec_box[0], FALSE, FALSE, 50);
  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, elec_box[0], hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Long range interaction cutoff [r<sub>cut</sub>]:", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
  GtkWidget * entry = create_entry (G_CALLBACK(set_elec_param), 100, 10, FALSE, GINT_TO_POINTER(1));
  update_entry_double (GTK_ENTRY(entry), tmp_field -> elec_opts[1]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>&#xC5;</b>", 30, -1, 0.0, 0.5), FALSE, FALSE, 5);
  val = (tmp_field -> elec_opts[2] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Add padding to r<sub>cut</sub>:", 150, -1, val, G_CALLBACK(check_elec), GINT_TO_POINTER(2)), FALSE, FALSE, 0);
  elec_box[1] = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, elec_box[1], FALSE, FALSE, 0);
  entry = create_entry (G_CALLBACK(set_elec_param), 100, 10, FALSE, GINT_TO_POINTER(3));
  update_entry_double (GTK_ENTRY(entry), tmp_field -> elec_opts[3]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, elec_box[1], entry, FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, elec_box[1], markup_label("<b>&#xC5;</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 5);
  widget_set_sensitive (elec_box[1], val);
  val = (tmp_field -> elec_opts[4] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_VERTICAL, elec_box[0], check_button("Use extended coulombic exclusion", -1, -1, val, G_CALLBACK(check_elec), GINT_TO_POINTER(4)), FALSE, FALSE, 5);
  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, elec_box[0], hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Evaluation method:", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
  GtkWidget * combo = create_combo();
  int i;
  for (i=0; i<10; i++)
  {
    combo_text_append(combo, eval_m[i]);
  }
  combo_set_active (combo, (int)tmp_field -> elec_opts[5]);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_elec_eval), NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 10);
  elec_box[2] = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, elec_box[0], elec_box[2], FALSE, FALSE, 0);
  elec_box[3] = create_elec_param_box ();
  add_box_child_start (GTK_ORIENTATION_VERTICAL, elec_box[2], elec_box[3], FALSE, FALSE, 10);
  return vbox;
}

GtkWidget * vdw_box[2];
gchar * eval_vdw[6] = {"Lorentz-Berthelot", "Fender-Halsey", "Hogervorst",
                       "Halgren HHG", "Tang-Toennies", "Functional"};

/*!
  \fn G_MODULE_EXPORT void set_vdw_param (GtkEntry * res, gpointer data)

  \brief CONTROL file udpate VdW parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_vdw_param (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i == 1)
  {
    if (v > 0.0 && v != tmp_field -> vdw_opts[i])
    {
      tmp_field -> vdw_opts[i] = v;
    }
    else if (v < 0.0)
    {
      show_warning ("Cutoff must be > 0.0 &#xC5;", field_assistant);
    }
  }
  update_entry_double (GTK_ENTRY(res), tmp_field -> vdw_opts[i]);
}

/*!
  \fn G_MODULE_EXPORT void set_vdw_mix (GtkComboBox * box, gpointer data)

  \brief CONTROL file change VdW mixing rule

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_vdw_mix (GtkComboBox * box, gpointer data)
{
  tmp_field -> vdw_opts[5] =  combo_get_active ((GtkWidget *)box);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_vdw (GtkCheckButton * but, gpointer data)

  \brief change CONTROL file VdW option toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_vdw (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_vdw (GtkToggleButton * but, gpointer data)

  \brief change CONTROL file VdW option toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_vdw (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j;
  j = GPOINTER_TO_INT(data);
  i = button_get_status ((GtkWidget *)but);
  tmp_field -> vdw_opts[j] = (double)i;
  if (j == 0) widget_set_sensitive (vdw_box[0], i);
  if (j == 4) widget_set_sensitive (vdw_box[1], i);
}

/*!
  \fn GtkWidget * create_vdws_box ()

  \brief CONTROL file create VdW parameter vidgets
*/
GtkWidget * create_vdws_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  gboolean val = (tmp_field -> vdw_opts[0] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, check_button("Evaluate Van der Waals interactions", -1, -1, val, G_CALLBACK(check_vdw), GINT_TO_POINTER(0)), FALSE, FALSE, 5);
  hbox = create_hbox(0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  vdw_box[0] = create_vbox(5);
  gtk_box_set_homogeneous (GTK_BOX (vdw_box[0]), TRUE);
  widget_set_sensitive (vdw_box[0], val);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vdw_box[0], FALSE, FALSE, 50);
  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vdw_box[0], hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Short range interaction cutoff:", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
  GtkWidget * entry = create_entry (G_CALLBACK(set_vdw_param), 100, 10, FALSE, GINT_TO_POINTER(1));
  update_entry_double (GTK_ENTRY(entry), tmp_field -> vdw_opts[1]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>&#xC5;</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 5);

  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vdw_box[0], hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Enforce direct calculation of van der Waals interactions", -1, -1, val, G_CALLBACK(check_vdw), GINT_TO_POINTER(2)), FALSE, FALSE, 5);

  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vdw_box[0], hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Apply force-shifting (contributions smoothly fall to zero near r<sub>cut</sub>)", -1, -1, val, G_CALLBACK(check_vdw), GINT_TO_POINTER(3)), FALSE, FALSE, 5);

  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vdw_box[0], hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Apply mixing rule (when needed and if possible generate cross species interactions)", -1, -1, val, G_CALLBACK(check_vdw), GINT_TO_POINTER(4)), FALSE, FALSE, 5);
  vdw_box[1] = create_vbox(5);
  val = (tmp_field -> vdw_opts[4] == 1.0) ? TRUE : FALSE;
  widget_set_sensitive (vdw_box[1], val);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vdw_box[0], vdw_box[1], FALSE, FALSE, 0);
  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vdw_box[1], hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Mixing rule:", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
  GtkWidget * combo = create_combo();
  int i;
  for (i=0; i<6; i++)
  {
    combo_text_append(combo, eval_vdw[i]);
  }
  combo_set_active (combo, (int)tmp_field -> vdw_opts[5]);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_vdw_mix), NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 10);

  return vbox;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_met (GtkCheckButton * but, gpointer data)

  \brief change CONTROL file metal interactions toggle callback GTK3

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_met (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_met (GtkToggleButton * but, gpointer data)

  \brief change CONTROL file metal interactions toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_met (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j;
  j = GPOINTER_TO_INT(data);
  i = button_get_status ((GtkWidget *)but);
  tmp_field -> met_opts[j] = (double)i;
}

/*!
  \fn GtkWidget * create_metal_box ()

  \brief create CONTROL file metal interaction widgets
*/
GtkWidget * create_metal_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  gtk_box_set_homogeneous (GTK_BOX (vbox), TRUE);
  GtkWidget * hbox;
  gboolean val = (tmp_field -> met_opts[0] == 1.0) ? TRUE : FALSE;
  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Enforce direct calculation of metal interactions by explicit potentials, does not work with *EAM* potentials",
                                                  -1, 25, val, G_CALLBACK(check_met), GINT_TO_POINTER(0)), FALSE, FALSE, 5);
  val = (tmp_field -> met_opts[1] == 1.0) ? TRUE : FALSE;
  hbox = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button("Switch the default embedding functions, <i>F</i>, from <i>F(&#x3c1;)</i> to <i>F(√&#x3c1;)</i>",
                                                  -1, 25, val, G_CALLBACK(check_met), GINT_TO_POINTER(1)), FALSE, FALSE, 5);
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void set_sys_param (GtkEntry * res, gpointer data)

  \brief update CONTROL file system option parameter entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_sys_param (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i == 9)
  {
    if (v >= 1.0 && v != tmp_field -> sys_opts[i])
    {
      tmp_field -> sys_opts[i] = v;
    }
    else if (v < 1.0)
    {
      show_warning ("Subcelling threshold density must be &#8805; 1.0", field_assistant);
    }
  }
  else if (v >= 0.0 && v != tmp_field -> sys_opts[i])
  {
    tmp_field -> sys_opts[i] = v;
  }
  if (i > 10)
  {
    update_entry_int (GTK_ENTRY(res), (int)tmp_field -> sys_opts[i]);
  }
  else
  {
    update_entry_double (GTK_ENTRY(res), tmp_field -> sys_opts[i]);
  }
}

GtkWidget * sys_box[4];
gchar * sys_opts[10] = {"Relative dielectric constant &#949;<sub>r</sub>",
                        "Allowed local variation of system density:",
                        "Ignore the particle indices in CONFIG file",
                        "Ignore strict checks, hide warnings and assume safe simulation parameters",
                        "Skip detailed topology reporting during read of FIELD file in output",
                        "Ignore center of mass removal during the calculation",
                        "Set tolerance for relaxed shell model:",
                        "Set the subcelling threshold density of particles per link cell:",
                        "Create an expanded version of the current model:",
                        "Restart calculation:"};

/*!
  \fn G_MODULE_EXPORT void set_sys_restart (GtkComboBox * box, gpointer data)

  \brief change CONTROL file system option

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_sys_restart (GtkComboBox * box, gpointer data)
{
  tmp_field -> sys_opts[15] =  combo_get_active ((GtkWidget *)box);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void check_sys (GtkCheckButton * but, gpointer data)

  \brief change CONTROL file system option toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_sys (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void check_sys (GtkToggleButton * but, gpointer data)

  \brief change CONTROL file system option toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void check_sys (GtkToggleButton * but, gpointer data)
#endif
{
  int i, k;
  i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  tmp_field -> sys_opts[i] = (j) ? 1.0 : 0.0;
  if (i > 5)
  {
    k = i - 6;
    if (i == 14) k = 3;
    widget_set_sensitive (sys_box[k], j);
  }
}

/*!
  \fn GtkWidget * create_sys_box ()

  \brief prepare the DL-POLY CONTROL file system options widgets
*/
GtkWidget * create_sys_box ()
{
  GtkWidget * vbox;
  GtkWidget * hbox, * hhbox;
  GtkWidget * entry;
  gboolean val;
  hhbox = create_hbox (5);
  vbox = create_vbox (5);
  gtk_box_set_homogeneous (GTK_BOX (vbox), TRUE);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, vbox, FALSE, FALSE, 10);
  int i, j, k;
  for (i=0; i<9; i++)
  {
    hbox = create_hbox(5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    if (i < 2)
    {
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(sys_opts[i], 300, -1, 0.0, 0.5), FALSE, FALSE, 5);
      entry = create_entry (G_CALLBACK(set_sys_param), 100, 10, FALSE, GINT_TO_POINTER(i));
      update_entry_double (GTK_ENTRY(entry), tmp_field -> sys_opts[i]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 10);
      if (i) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>%</b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }
    else
    {
      j = (i < 7) ? i : (i == 7) ? 8 : 10;
      val = (tmp_field -> sys_opts[j] == 1.0) ? TRUE : FALSE;
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(sys_opts[i], -1, 25, val, G_CALLBACK(check_sys), GINT_TO_POINTER(i)), FALSE, FALSE, 5);
      if (i > 5)
      {
        sys_box[i-6] = create_hbox(5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sys_box[i-6], FALSE, FALSE, 0);
        widget_set_sensitive (sys_box[i-6], val);
        if (i < 8)
        {
          entry = create_entry (G_CALLBACK(set_sys_param), 100, 10, FALSE, GINT_TO_POINTER(j+1));
          update_entry_double (GTK_ENTRY(entry), tmp_field -> sys_opts[j+1]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, sys_box[i-6], entry, FALSE, FALSE, 10);
          if (i == 6) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, sys_box[i-6], markup_label("<b>D &#xC5; ps<sup>-2</sup></b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
        }
        else
        {
          for (k=0; k<3; k++)
          {
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, sys_box[i-6], markup_label(imp_dir[k], 50, -1, 1.0, 0.5), FALSE, FALSE, 0);
            entry = create_entry (G_CALLBACK(set_sys_param), 50, 5, FALSE, GINT_TO_POINTER(j+k+1));
            update_entry_int (GTK_ENTRY(entry), (int)tmp_field -> sys_opts[j+k+1]);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, sys_box[i-6], entry, FALSE, FALSE, 5);
          }
        }
      }
    }
  }

  return hhbox;
}

/*!
  \fn GtkWidget * create_restart_box ()

  \brief prepare the DL-POLY CONTROL file restart widgets
*/
GtkWidget * create_restart_box ()
{
  GtkWidget * vbox = create_vbox (5);
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  gboolean val = (tmp_field -> sys_opts[14] == 1.0) ? TRUE : FALSE;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button(sys_opts[9], -1, 25, val, G_CALLBACK(check_sys), GINT_TO_POINTER(14)), FALSE, FALSE, 5);
  sys_box[3] = create_hbox(5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sys_box[3], FALSE, FALSE, 0);
  widget_set_sensitive (sys_box[3], val);
  gchar * rtype[3]={"Continue current simulation",
                    "Start new simulation from older run without temperature reset",
                    "Start new simulation from older run with temperature reset"};
  GtkWidget * combo = create_combo();
  int i;
  for (i=0; i<3; i++) combo_text_append(combo, rtype[i]);
  combo_set_active (combo, (int)tmp_field -> sys_opts[15]);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(set_sys_restart), NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, sys_box[3], combo, FALSE, FALSE, 20);
  return vbox;
}


/*!
  \fn GtkWidget * vbox_control (int f)

  \brief crerate DL-POLY option widgets

  \param f the page number in the CONTROL file section of the DL-POLY input file(s) creation assistant
*/
GtkWidget * vbox_control (int f)
{
  GtkWidget * vbox;
  vbox = create_vbox (5);
  switch (f)
  {
    case 0:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Global options:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_sys_box(), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Restart options:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_restart_box(), FALSE, FALSE, 0);
      break;
    case 1:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Intra-molecular probability denisty function (PDF) analysis:</b>", 350, -1, 0.0, 0.5), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_analyze_box(), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Overall analysis:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_overall_box(), FALSE, FALSE, 0);
      break;
    case 2:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Van der Waals (Non-bonded short range):</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_vdws_box(), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Electrostatics (Non-bonded long range):</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_electro_box(), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Metallic interactions:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_metal_box(), FALSE, FALSE, 0);
      break;
    case 3:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Equilibration:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_equi_box(), FALSE, FALSE, 0);
      break;
    case 4:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Thermodynamics:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_ensemble_box(), FALSE, FALSE, 0);
      break;
    case 5:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Molecular dynamics:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_md_box(), FALSE, FALSE, 0);
      break;
    case 6:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Output information:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_out_box(), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Trajectory file(s):</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_traj_box(), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Restart file:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_dump_box(), FALSE, FALSE, 10);
      break;
    case 7:
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Job options:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_job_box(), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>I/O options:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_io_box(), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>Other options:</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_misc_box(), FALSE, FALSE, 0);
      break;
    default:
      break;
  }
  return vbox;
}

/*
  0: System options:

    0 = Epsilon
    1 = Density variation
    2 = no index
    3 = no strict
    4 = no topology
    5 = no vafar
    6 = no vom
    7 = Set tolerance for rsm ...
    8 = ... rsm tolerance
    9 = Set subscelling threshold ...
   10 = ... subscelling threshold
   11 = Create expanded system ...
   12 = ... x ...
   13 = ... y ...
   14 = ... z
   15 = Restart calc ...
   16 = ... restart type


  1: Calc. options:

    0 = All PDFs
    1 = All: every
    2 = All: ndelta
    3 = All: cutoff
    4 = Bonds PDFs
    5 = Bonds: every
    6 = Bonds: ndelta
    7 = Bonds: cutoff
    8 = Angles PDFs
    9 = Angles: every
   10 = Angles: ndelta
   11 = Dihedrals PDFs
   12 = Dihedrals: every
   13 = Dihedrals: ndelta
   14 = Inversions PDFs
   15 = Inversions: every
   16 = Inversions: ndelta
   17 = print pdfs
   18 = rdf
   19 = every n steps
   20 = bin size
   21 = print velocity autocorrelation functions
   22 = every n steps
   23 = print z-dens
   24 = every n steps
   25 = Job time
   26 = Job closure

  2: Electro. stat options:
    0 = Evaluate (or not) electrostatics
    1 = Electrostatics evaluation method
    2 = Param 1
    3 = Param 2
    4 = Param 3
    5 = Param 4

  3: Equil. options:

    0 = Equil
    1 = Equi for n steps
    2 = Scale temp
    3 = Scale temp: every
    4 = cap forces
    5 = cap: force max
    6 = Resample
    7 = Resample: during
    8 = Minimize
    9 = Minimize: string
   10 = Minimize: target
   11 = Minimize: every
   12 = Optimize
   13 = Optimize: string
   14 = Optimize: target
   15 = zero temp MD
   16 = collect

  4: Thermo options:

    0 = Ensemble
    1 = Thermostat
    2 to 7 = Thermostat options

  5: MD options
    0 = target temp
    1 = integrator
    2 = MD steps
    3 = Time step type
    4 = Time step
    5 = Max time step
    6 = Max dist. per time step
    7 = Min dist. per time step
    8 = Target pressure
    9 = shake limit
   10 = shake tol
   11 = FIQA lim
   12 = FIQA tol

   17 = Initiate impact
   18 = particle index
   19 = MD step
   20 = impact energy
   21 = impact x direct
   22 = impact y direct
   23 = impact z direct

  6: Output options:

    0 = Write defects trajectory ...
    1 = ... start time to dump defects
    2 = ... time step to dump defects
    3 = ... site interstitial cutoff
    4 = write displacements trajectory ...
    5 = ... start time to dump displacements
    6 = ... time step to dump displacements
    7 = ... displacement qualifying cutoff
    8 = Write history/trajectory file ...
    9 = ... start time to dump trajectory
   10 = ... time step to dump trajectory
   11 = ... data level [0=coord, 1=0+velo, 2=1+forces]
   12 = Write MSDTMP file ...
   13 = ... start time to dump trajectory
   14 = ... time step to dump trajectory
   15 = print data ...
   16 = ... every n steps
   17 = accumulate stat ...
   18 = ... every n step(s)
   19 = set rolling avertage stack ...
   20 = ... to n time step(s)
   21 = Write RDFs ...
   22 = ... every n step(s)
   23 = ... bin size *
   24 = Write VAFs ...
   25 = ... every n step(s)
   26 = ... bin size
   27 = Write z-dens ...
   28 = ... every n step(s)
   29 = Ignore time average for VAFs
   30 = Dump restart every n step(s)
*/

