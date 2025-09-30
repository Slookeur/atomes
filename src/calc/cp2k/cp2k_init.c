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
* @file cp2k_init.c
* @short Initialization of the CP2K input assistant
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cp2k_init.c'
*
* Contains:
*

 - The initialization of the CP2K input assistant

*
* List of functions:

  int find_cp2k_sym ();

  gboolean cp2k_with_motion ();

  gchar * cp_section_name (int p);
  gchar * page_name_cp2k (int p);

  void print_start_buffer ();
  void create_mol_selection_button (GtkWidget * box, int id);
  void cp2k_option_box (GtkWidget * box, int id);
  void add_cp2k_pages ();

  G_MODULE_EXPORT void update_cp2k_option (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void update_cp2k_option_check (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void update_cp2k_option_check (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void changed_cp2k_option_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void mol_selection_button (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void cp2k_option_dialog (GtkWidget *but, gpointer data);
  G_MODULE_EXPORT void update_cp2k_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void update_cp2k_check (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void update_cp2k_check (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void cp2k_file_info (GtkTextBuffer * textbuf, gpointer data);
  G_MODULE_EXPORT void changed_cp2k_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void select_input_type (GtkToggleButton * but, gpointer data);

  GtkWidget * create_vdw_box (int id);
  GtkWidget * create_cp2k_spec_box (int spec);
  GtkWidget * cp2k_section_box (int s);
  GtkWidget * vbox_cp2k (int s);
  GtkWidget * file_info (int id);
  GtkWidget * init_cp2k ();

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"
#include "calc.h"
#include "cp2k.h"
#include "cpmd.h"

extern GtkWidget * thermo_box ();
extern void print_cp2k (int f, GtkTextBuffer * buffer);
extern ColRGBA init_color (int id, int numid);
extern GtkWidget * qm_preview_box (int c, int s, int l);
extern void field_unselect_all ();
extern GtkWidget * cpmd_box (GtkWidget * box, gchar * lab, int v_space, int h_space, int dim);
extern void create_selection_button (GtkWidget * box, int num, int id, gpointer data);
extern void cp2k_fix_molecule ();

cp2k * tmp_cp2k;

double default_cp2k_options[41] = {0.0, 86400.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 300.0,
                                   4.0, 0.0, 0.0, 50.0, 0.000001, 20.0, 0.00001, 2.0, 0.0, 0.0,
                                   0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                   0.0, 7.0, 10000.0, 2.5, 300.0, 1.0, 10000.0, 0.0001, 10.0, 0.0,
                                   0.0};

double default_vdw_cut[2] = {-2.00000000E+000, 1.05835442E+001};
double default_cp2k_extra[3][4] = {{0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {-1.0, -1.0, 1.0, 0.0}};

gchar * cp2k_unit[6]={"s", "Ry", "fs", "K", "a.u.", "steps"};

int cp2k_default_num[12] = {7, 3, 3, 4, 3, 11, 8, 4, 10, 9, 4, 2};

gchar * cp2k_default_keywords[11][11] = {{"ENERGY", "ENERGY_FORCES", "GEOMETRY_OPTIMIZATION", "MOLECULAR_DYNAMICS",
                                          "VIBRATIONAL_ANALYSIS", "LINEAR_RESPONSE", "EHRENFEST_DYN", " ", " ", " ", " "},
                                         {"LOW", "MEDIUM", "HIGH", " ", " ", " ", " ", " ", " ", " ", " "},
                                         {"GPW", "GAPW", "GAPW_XC", " ", " ", " ", " ", " ", " ", " ", " "},
                                         {"ATOMIC", "CORE", "RANDOM", "RESTART", " ", " ", " ", " ", " ", " ", " "},
                                         {"BROYDEN", "CG", "DIIS", " ", " ", " ", " ", " ", " ", " ", " "},
                                         {"BLYP", "B3LYP", "BEEFVDW", "BP", "HCTH120", "LDA", "OLYP", "PADE", "PBE", "PBE0", "TPSS"},
                                         {"XYZ", "XY", "XZ", "YZ", "X", "Y", "Z", "NONE", " ", " ", " "},
                                         {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                         {"CUBIC", "HEXAGONAL", "ORTHORHOMBIC", "RHOMBOHEDRAL",
                                          "TETRAGONAL_AB", "TETRAGONAL_AC", "TETRAGONAL_BC", "MONOCLINIC", "TRICLINIC", "NONE", " "},
                                         {"ISOKIN", "LANGEVIN", "NPE_F", "NPE_I", "NPT_F", "NPT_I", "NVE", "NVT", "NVT_ADIABATIC", " ", " "},
                                         {"BFGS", "CG", " ", " ", " ", " ", " ", " ", " ", " ", " "}};

gchar * cp2k_vdw_keywords[2][3] = {{"DRSLL", "LMKLL", "RVV10"}, {"DFTD2", "DFTD3", "DFTD3(BJ)"}};

gchar * cp2k_default_text[11][11] = {{"Single point energy calculation",
                                      "Single point energy and forces calculation",
                                      "Geometry optimization", "Molecular dynamics",
                                      "Vibrational analysis", "Linear response calculation", "Ehrenfest dynamics", " ", " ", " ", " "},
                                     {"Low", "Medium", "High", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {"Gaussian and plane waves method",
                                      "Gaussian and augmented plane waves method",
                                      "Gaussian and augmented plane waves method only for XC", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {"Generate an atomic density using the atomic code",
                                      "Diagonalize the core Hamiltonian",
                                      "Use random wave-function coefficients",
                                      "Use the RESTART file", " ", " ", " ", " ", " ", " ", " "},
                                     {"Broyden mixing approximating the inverse Hessian",
                                      "Conjugate Gradients", "Direct inversion in the iterative subspace", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {"Becke + Lee-Yang-Parr", "Becke three-parameters hybrid + LYP", "Bayesian error estimation + van der Walls",
                                      "Becke + Perdew", "Hamprecht-Cohen-Tozer-Handy", "Local Density Approximation", "Handy-Cohen + LYP", "Padé",
                                      "Perdew + Burke-Ernzerhof", "Parameter-free PBE", "Tao-Perdew-Staroverov-Scuseria"},
                                     {"3 dimensional (XYZ)", "2 dimensional (XY)", "2 dimensional (XZ)", "2 dimensional (YZ)",
                                      "1 dimensional (X)", "1 dimensional (Y)", "1 dimensional (Z)", "Isolated system", " ", " ", " "},
                                     {"Box parameters (a,b,c and α,β,γ)", "Lattice vectors", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {"Cubic (a = b = c, α = β = γ = 90°)", "Hexagonal (a = b ≠ c, α = β = 90°, γ = 60°)",
                                      "Orthorhombic (a ≠ b ≠ c, α = β = γ = 90°)", "Rhombohedral (a = b = c, α = β = γ ≠ 90°)",
                                      "Tetragonal (a = b ≠ c, α = β = γ = 90°)", "Tetragonal (a = c ≠ b, α = β = γ = 90°)", "Tetragonal (a ≠ b = c, α = β = γ = 90°)",
                                      "Monoclinic (a ≠ b ≠ c ≠ a, α = γ = 90°, β ≠ 90°)", "Triclinic (a ≠ b ≠ c ≠ a, α ≠ β ≠ γ ≠ α ≠ 90°)", "No cell symmetry", " "},
                                     {"Constant kinetic energy", "Langevin dynamics (constant temperature)", "Constant pressure ensemble (no thermostat)",
                                      "Constant pressure ensemble using an isotropic cell (no thermostat)", "Constant temperature and pressure using a flexible cell",
                                      "Constant temperature and pressure using an isotropic cell", "Constant energy (µ-canonical)", "Constant temperature and volume (canonical)",
                                      "Adiabatic dynamics in constant temperature and volume ensemble (CAFES)", " ", " "},
                                     {"Diagonalization of a full Hessian matrix", "Conjugate gradients", " ", " ", " ", " ", " ", " ", " ", " ", " "}};

gchar * cp2k_vdw_text[2][3] = {{"Dion-Rydberg-Schroeder-Langreth-Lundqvist non-local van der Waals density functional",
                                "Lee-Murray-Kong-Lundqvist-Langreth non-local van der Waals density functional",
                                "Revised Vydrov-van Voorhis non-local van der Waals density functional"},
                               {"Grimme D2 method", "Grimme D3 method (zero damping)", "Grimme D3 method (Becke-Johnson damping)"}};

int var_by_cp2sections[4] = {8, 20, 3, 10};
gchar * global_opts[40] = {"Run type:",
                           "Max CPU time:",
                           "Print level:",
                           "Use restart information:",
                           "File that contains the restart information:",
                           "File that contains the basis set(s):",
                           "File that contains the pseudo-potential(s):",
                           "File that contains the wave-function:",
                           "Charge:",
                           "Density cutoff for the finest grid level:",
                           "Number of multi-grids:",
                           "Quickstep method:",
                           "Initial guess for the wave-function:",
                           "Maximum inner-SCF cycle(s):",
                           "Convergence threshold:",
                           "Maximum outer-SCF cycle(s):",
                           "Convergence threshold:",
                           "Orbital transformation minimizer:",
                           "DFT functional:",
                           "Spin unrestricted calculation",
                           "Multiplicity",
                           "Use van der Waals interactions",
                           "Restricted Open Kohn-Sham 'ROKS'",
                           "Forces",
                           "Stress tensor",
                           "Mulliken population",
                           "Löwdin population",
                           "Orbitals",
                           "Periodicity:",
                           "Lattice parameters:",
                           "Symmetry:",
                           "Ensemble:",
                           "Maximum number of MD steps:",
                           "MD time step:",
                           "Initial temperature:",
                           "Geometry optimizer:",
                           "Maximum number of optimization steps:",
                           "Atomic forces convergence criterion:",
                           "Save information every:",
                           "Add constraint(s)"};

gchar * cpelemts[7] = {"The CP2K input structure",
                       "provides the general control parameters for the calculation to be performed",
                       "describes how the interatomic forces will be evaluated",
                       "describes the model: symmetry, periodicity and atomic species, pseudo-potentials and coordinates",
                       "describes the displacement of the atoms for molecular dynamics of geometry optimization",
                       "To describe the thermostat(s) for molecular dynamics",
                       "To describe the parameters required for restart"};

gchar * cpsect[4] = {"GLOBAL",
                     "FORCE_EVAL",
                     "SUBSYS",
                     "MOTION"};

char * k_thermo[CP2NTHERM][4] = {{"TIMECON_LANGEVIN", "TIMECON_NH", "MASS", "CHI"},
                                 {"TIMECON", " ", " ", " "},
                                 {" ", " ", " ", " "},
                                 {"LENGTH", "MTS", "TIMECON", "YOSHIDA"}};

#define NC2OP 20

// 0 = None, 1 = Entry, 2 = Combo (-2 thermostats), 3 = yes/no, 4 = textbuffer
int cp2k_opts_type[4][NC2OP] = {{ 2, 1, 2, 3, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // GLOBAL
                                { 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 3, 1, 3, 3, 3, 3, 3, 3, 3},  // FEVAL
                                { 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // SUBSYS
                                { 2, -2, 1, 1, 1, 2, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}; // MOTION

int idunit;
int checkb;
int optionb;
int motionb;
GtkWidget * print_box;
GtkWidget * vfbox[2];
GtkWidget * filebut[2];
GtkWidget * fileinf[2];
GtkWidget * cp2k_combo;
GtkWidget * cp2k_label;
GtkWidget * checked_box[2];
GtkWidget * option_box[4];
GtkWidget * motion_box[2];
GtkWidget * cp2k_vdw_box[2];
GtkWidget * cp2k_spec_box[2];
GtkWidget * cp2k_spec_combo;
GtkWidget * spec_data_combo[2];
GtkWidget * cp2k_thermo_box;

/*!
  \fn void print_start_buffer ()

  \brief update of the GtkTextBuffer showing the CP2K input
*
*/
void print_start_buffer ()
{
  if (qmbuffer[0])
  {
    GtkTextIter bStart;
    GtkTextIter bEnd;
    gtk_text_buffer_get_start_iter (qmbuffer[0], & bStart);
    gtk_text_buffer_get_end_iter (qmbuffer[0], & bEnd);
    gtk_text_buffer_delete (qmbuffer[0], & bStart, & bEnd);
    print_cp2k (0, qmbuffer[0]);
  }
}

/*!
  \fn G_MODULE_EXPORT void update_cp2k_option (GtkEntry * res, gpointer data)

  \brief update CP2K option entry callback

  \param res the GtkEntry the signal is coming from
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cp2k_option (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  switch (i)
  {
    case CP2VDW:
      if (v > 0.0) tmp_cp2k -> extra_opts[0][2] = v;
      update_entry_double (res, tmp_cp2k -> extra_opts[0][2]);
      break;
    case CP2ROK:
      if (v > 0.0) tmp_cp2k -> extra_opts[1][0] = v;
      update_entry_double (res, tmp_cp2k -> extra_opts[1][0]);
      break;
    default:
      if ((int)v == -1 || (int)v > 0) tmp_cp2k -> extra_opts[2][i-CP2POR] = v;
      update_entry_int (res, tmp_cp2k -> extra_opts[2][i-CP2POR]);
      break;
  }
  print_start_buffer ();
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void update_cp2k_option_check (GtkCheckButton * but, gpointer data)

  \brief CP2K option toggle callback GTK3

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cp2k_option_check (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void update_cp2k_option_check (GtkToggleButton * but, gpointer data)

  \brief CP2K option toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cp2k_option_check (GtkToggleButton * but, gpointer data)
#endif
{
  int i;
  i = GPOINTER_TO_INT(data);
  tmp_cp2k -> extra_opts[i][(i==2)?2:3] = (double) button_get_status ((GtkWidget *)but);
  print_start_buffer ();
}

GtkWidget * create_vdw_box (int id);

/*!
  \fn G_MODULE_EXPORT void changed_cp2k_option_box (GtkComboBox * box, gpointer data)

  \brief change a GtkComboBox while creating a CP2K input file

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_cp2k_option_box (GtkComboBox * box, gpointer data)
{
  int i, j, k;
  i = GPOINTER_TO_INT(data);
  if (i < 2)
  {
    j = k = 1;
  }
  else
  {
    j = 0;
    k = i - CP2VDW;
  }
  tmp_cp2k -> extra_opts[j][k] = (double) combo_get_active ((GtkWidget *)box);
  if (i == CP2VDW)
  {
    cp2k_vdw_box[1] = destroy_this_widget (cp2k_vdw_box[1]);
    cp2k_vdw_box[1] = create_vdw_box ((int)tmp_cp2k -> extra_opts[0][0]);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, cp2k_vdw_box[0], cp2k_vdw_box[1], FALSE, FALSE, 0);
    show_the_widgets (cp2k_vdw_box[1]);
  }
  print_start_buffer ();
}

/*!
  \fn GtkWidget * create_vdw_box (int id)

  \brief create the VdW informaiton and option(s) widgets

  \param id the type of VdW interactions
*/
GtkWidget * create_vdw_box (int id)
{
  GtkWidget * vbox, * hbox;
  GtkWidget * widg;
  gchar * cp2k_vdw_vars[2] = {"Type of potential:", "Cutoff:"};
  int i, j, k;
  vbox = create_vbox(5);
  i = (id == 0) ? 2 : 3;
  for (j=0; j<i; j++)
  {
    hbox = create_hbox(5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    if (j < 2) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(cp2k_vdw_vars[j], 150, -1, 0.0, 0.5), FALSE, FALSE, 30);
    switch (j)
    {
      case 0:
        widg = create_combo ();
        for (k=0; k<3; k++) combo_text_append (widg, cp2k_vdw_text[id][k]);
        combo_set_active (widg, (int)tmp_cp2k -> extra_opts[0][j+1]);
        g_signal_connect (G_OBJECT (widg), "changed", G_CALLBACK(changed_cp2k_option_box), GINT_TO_POINTER(CP2VDW+1));
        break;
      case 1:
        widg = create_entry (G_CALLBACK(update_cp2k_option), 100, 15, FALSE, GINT_TO_POINTER(CP2VDW));
        tmp_cp2k -> extra_opts[0][j+1] = default_vdw_cut[id];
        update_entry_double (GTK_ENTRY(widg), tmp_cp2k -> extra_opts[0][j+1]);
        break;
      case 2:
        widg = check_button ("Apply long range corrections", -1, -1, tmp_cp2k -> extra_opts[0][j+1],
                             G_CALLBACK(update_cp2k_option_check), GINT_TO_POINTER(0));
        break;
     }
     add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 5);
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void mol_selection_button (GtkButton * but, gpointer data)

  \brief NOT USED !

  \param but the GtkButton sending the signal
  \param data the associated data pointer
* /
G_MODULE_EXPORT void mol_selection_button (GtkButton * but, gpointer data)
{
  cp2k_fix_molecule ();
  gchar * stra, * strb;
  if (tmp_cp2k -> fixat[1]  == 0)
  {
    stra = g_strdup_printf ("Not picked yet !");
    strb = g_strdup_printf (DELETEB);
  }
  else
  {
    stra = g_strdup_printf ("%d fragment(s)", tmp_cp2k -> fixat[1]);
    strb = g_strdup_printf (APPLY);
  }
  set_image_from_icon_name (sel_img[2], strb);
  gtk_button_set_label (but, stra);
  g_free (stra);
  g_free (strb);
}
*/

/*!
  \fn void create_mol_selection_button (GtkWidget * box, int id)

  \brief NOT USED !

  \param box the box to insert the new widget in
  \param id the selection status
* /
void create_mol_selection_button (GtkWidget * box, int id)
{
  GtkWidget * hbox = cpmd_box (box, "Molecule(s)<sup>*</sup> selection <sup>**</sup>:", 5, 20, 220);
  gchar * str;
  if (tmp_cp2k -> fixat[1] == 0)
  {
    str = g_strdup_printf ("Not picked yet !");
    sel_img[id] = stock_image (DELETEB);
  }
  else
  {
    str = g_strdup_printf ("%d fragment(s)", tmp_cp2k -> fixat[1]);
    sel_img[id] = stock_image (APPLY);
  }
  sel_but[id] = create_button (str, IMG_NONE, NULL, 150, -1, GTK_RELIEF_NONE, G_CALLBACK(mol_selection_button), NULL);
  g_free (str);
  widget_set_sensitive (sel_but[id], qm_view -> bonding);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sel_but[id], FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sel_img[id], FALSE, FALSE, 30);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, markup_label("* isolated molecular fragment", -1, -1, 0.1, 0.5), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, markup_label("** requires to compute the bonding properties", -1, -1, 0.1, 0.5), FALSE, FALSE, 5);
}
*/

/*!
  \fn G_MODULE_EXPORT void cp2k_option_dialog (GtkWidget * but, gpointer data)

  \brief CP2K option dialog - creating the dialog

  \param but the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void cp2k_option_dialog (GtkWidget *but, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  GtkWidget * vbox, * hbox;
  GtkWidget * widg;
  gchar * cp2k_vdw_options[2] = {"Non-local potential", "Pair potential"};
  gchar * cp2k_roks_options[2] = {"Energy scaling:", "Spin configuration <sup>*</sup>:"};
  gchar * cp2k_orb_options[2] = {"Number of HOMO levels (-1 for all):", "Number of LUMO levels (-1 for all):"};

  gchar * str = g_strdup_printf ("%s - options", global_opts[i]);
  GtkWidget * dial = dialogmodal (str, GTK_WINDOW(qm_assistant));
  g_free (str);
  vbox = dialog_get_content_area (dial);
  switch (i)
  {
    case CP2VDW:
      hbox = create_hbox (5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
      widg = create_combo ();
      for (j=0; j<2; j++)  combo_text_append (widg, cp2k_vdw_options[j]);
      combo_set_active (widg, (int)tmp_cp2k -> extra_opts[0][0]);
      g_signal_connect (G_OBJECT (widg), "changed", G_CALLBACK(changed_cp2k_option_box), GINT_TO_POINTER(CP2VDW));
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Type of functional for the van der Waals interactions", 200, -1, 0.0, 0.5), FALSE, FALSE, 20);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 5);
      cp2k_vdw_box[0] = create_vbox (BSEP);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, cp2k_vdw_box[0], FALSE, FALSE, 10);
      cp2k_vdw_box[1] = create_vdw_box ((int)tmp_cp2k -> extra_opts[0][0]);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, cp2k_vdw_box[0], cp2k_vdw_box[1], FALSE, FALSE, 0);
      break;
    case CP2ROK:
      for (j=0; j<2; j++)
      {
        hbox = create_hbox (5);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(cp2k_roks_options[j], 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
        if (j==0)
        {
          widg = create_entry (G_CALLBACK(update_cp2k_option), 100, 15, FALSE, GINT_TO_POINTER(i+j));
          update_entry_double (GTK_ENTRY(widg), tmp_cp2k -> extra_opts[1][j]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 10);
        }
        else
        {
          widg = create_combo ();
          combo_text_append (widg, "alpha - α");
          combo_text_append (widg, "beta - β");
          combo_set_active (widg, (int)tmp_cp2k -> extra_opts[1][1]);
          g_signal_connect (G_OBJECT (widg), "changed", G_CALLBACK(changed_cp2k_option_box), GINT_TO_POINTER(j));
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 10);
        }
      }
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, gtk_label_new ("* for the singly occupied orbitals"), FALSE, FALSE, 5);
      break;
    case CP2POR:
      for (j=0; j<2; j++)
      {
        hbox = create_hbox (5);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(cp2k_orb_options[j], 250, -1, 0.0, 0.5), FALSE, FALSE, 5);
        widg = create_entry (G_CALLBACK(update_cp2k_option), 100, 15, FALSE, GINT_TO_POINTER(i+j));
        update_entry_int (GTK_ENTRY(widg), tmp_cp2k -> extra_opts[2][j]);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 0);
      }
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, check_button ("Write cube files, if not then only print eigenvalues", -1, -1, tmp_cp2k -> extra_opts[2][2],
                                                        G_CALLBACK(update_cp2k_option_check), GINT_TO_POINTER(2)), FALSE, FALSE, 5);
      break;
    case CP2CON:
      create_selection_button (vbox, tmp_cp2k -> fixat[0], 1, GINT_TO_POINTER(-1));
      //create_mol_selection_button (vbox, 2);
      break;
  }
  run_this_gtk_dialog (dial, G_CALLBACK(run_destroy_dialog), NULL);
}

/*!
  \fn G_MODULE_EXPORT void update_cp2k_parameter (GtkEntry * res, gpointer data)

  \brief update some CP2K parameters

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cp2k_parameter (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (tmp_cp2k -> opts[i] != v)
  {
    tmp_cp2k -> opts[i] = v;
    print_start_buffer ();
  }
  if (i==CP2CUT || i==CP2SCN || i==CP2SCO || i==CP2DLT || i==CP2GEF)
  {
    update_entry_double (res, tmp_cp2k -> opts[i]);
  }
  else
  {
    update_entry_int (res, tmp_cp2k -> opts[i]);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void update_cp2k_check (GtkCheckButton * but, gpointer data)

  \brief update some CP2K parameter toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cp2k_check (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void update_cp2k_check (GtkToggleButton * but, gpointer data)

  \brief update some CP2K parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cp2k_check (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  tmp_cp2k -> opts[i] = (double) button_get_status ((GtkWidget *)but);
  if (i == CP2RES)
  {
    for (j=0; j<2; j++) widget_set_sensitive (checked_box[j], (int)tmp_cp2k -> opts[CP2RES]);
  }
  else if (i==CP2SPI || i==CP2VDW || i==CP2ROK || i==CP2POR || i==CP2CON)
  {
    switch (i)
    {
      case CP2SPI:
        j = 0;
        break;
      case CP2VDW:
        j = 1;
        break;
      case CP2ROK:
        j = 2;
        break;
      case CP2POR:
        j = 3;
        break;
      case CP2CON:
        j = 4;
        break;
    }
    widget_set_sensitive (option_box[j], (int)tmp_cp2k -> opts[i]);
  }
  print_start_buffer ();
}

/*!
  \fn G_MODULE_EXPORT void cp2k_file_info (GtkTextBuffer * textbuf, gpointer data)

  \brief update the C2PK information textview

  \param textbuf the GtkTextBuffer sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void cp2k_file_info (GtkTextBuffer * textbuf, gpointer data)
{
  int i, j, k;
  i = GPOINTER_TO_INT(data) - CP2FRE;
  GtkTextIter bStart;
  GtkTextIter bEnd;
  gtk_text_buffer_get_start_iter (textbuf, & bStart);
  gtk_text_buffer_get_end_iter (textbuf, & bEnd);
  if (i < 5)
  {
    if (tmp_cp2k -> files[i] != NULL) g_free (tmp_cp2k -> files[i]);
    tmp_cp2k -> files[i] = g_strdup_printf ("%s", gtk_text_buffer_get_text (textbuf, & bStart, & bEnd, FALSE));
    if (g_strcmp0 (tmp_cp2k -> files[i], "") == 0) tmp_cp2k -> files[i] = NULL;
  }
  else
  {
    j = combo_get_active (cp2k_spec_combo);
    k = i + CP2FRE - 10;
    if (tmp_cp2k -> spec_files[j][k] != NULL) g_free (tmp_cp2k -> spec_files[j][k]);
    tmp_cp2k -> spec_files[j][k] = g_strdup_printf ("%s", gtk_text_buffer_get_text (textbuf, & bStart, & bEnd, FALSE));
    if (g_strcmp0 (tmp_cp2k -> spec_files[j][k], "") == 0)
    {
      tmp_cp2k -> spec_files[j][k] = NULL;
      combo_set_active (spec_data_combo[k], tmp_cp2k -> spec_data[j][k]);
      widget_set_sensitive (spec_data_combo[k], TRUE);
    }
    else if (spec_data_combo[k] != NULL)
    {
      combo_set_active (spec_data_combo[k], -1);
      widget_set_sensitive (spec_data_combo[k], FALSE);
    }
  }
  print_start_buffer ();
}

/*!
  \fn GtkWidget * create_cp2k_spec_box (int spec)

  \brief create a CP2K chemical species option combo box

  \param spec the chemical species
*/
GtkWidget * create_cp2k_spec_box (int spec)
{
  gchar * info[2] = {"<b>-</b> Select and use set of parameters from the database: ",
                     "<b>-</b> Define and use other set of parameters: "};
  gchar * data_name[2][2] = {{"\t\tAvailable basis set for ", "\t\tAvailable pseudo-potential for "},
                             {"\t\tBasis set for ", "\t\tPseudo-potential for "}};
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  gchar * str;
  int i, j;
  for (i=0; i<2; i++)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(info[i], -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
    for (j=0; j<2; j++)
    {
      str = g_strdup_printf ("%s%s", data_name[i][j], qm_proj -> chemistry -> label[spec]);
      hbox = cpmd_box (vbox, str, 5, 5, 300);
      g_free (str);
      if (i==0)
      {
        spec_data_combo[j] = NULL;
        if (tmp_cp2k -> spec_data[spec][j] > -1)
        {
          spec_data_combo[j] = prepare_basis_combo (spec, j);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, spec_data_combo[j], FALSE, FALSE, 5);
        }
        else
        {
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>None</b>", -1, -1, 0.5, 0.5), FALSE, FALSE, 5);
        }
      }
      else
      {
         add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, create_text_view (350, -1, 1, 0, G_CALLBACK(cp2k_file_info),
                                                               GINT_TO_POINTER(j+10), tmp_cp2k -> spec_files[spec][j]),
                             FALSE, FALSE, 5);
      }
    }
  }
  return vbox;
}

gboolean cp2k_with_motion ();
GtkWidget * vbox_cp2k (int s);
gchar * page_name_cp2k (int p);

/*!
  \fn G_MODULE_EXPORT void changed_cp2k_box (GtkComboBox * box, gpointer data)

  \brief change CP2K option

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_cp2k_box (GtkComboBox * box, gpointer data)
{
  int i, j;
  gboolean motion;
  i = GPOINTER_TO_INT(data);
  j = combo_get_active ((GtkWidget *)box);
  if (j != (int)tmp_cp2k -> opts[i])
  {
    if (i == CP2SYM)
    {
      cp2k_spec_box[1] = destroy_this_widget (cp2k_spec_box[1]);
      cp2k_spec_box[1] = create_cp2k_spec_box (combo_get_active ((GtkWidget *)box));
      show_the_widgets (cp2k_spec_box[1]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cp2k_spec_box[0], cp2k_spec_box[1], FALSE, FALSE, 0);
    }
    else
    {
      if (i == CP2RUN) motion = cp2k_with_motion ();
      tmp_cp2k -> opts[i] = (double) combo_get_active ((GtkWidget *)box);

      if (i == CP2RUN)
      {
        if (motion && ! cp2k_with_motion())
        {
          gtk_assistant_remove_page (GTK_ASSISTANT(qm_assistant), 5);
        }
        else if (! motion && cp2k_with_motion())
        {
          GtkAssistant * assist = GTK_ASSISTANT(qm_assistant);
          idopt = CP2SYM;
          motionb = -1;
          optionb = 3;
          idunit = 1;
          icomb = 8;
          GtkWidget * page = vbox_cp2k (3);
          show_the_widgets (page);
          gtk_assistant_insert_page (assist, page, 5);
          gtk_assistant_set_page_title (assist, page, page_name_cp2k(4));
          gtk_assistant_set_page_type (assist, page, GTK_ASSISTANT_PAGE_CONTENT);
          gtk_assistant_set_page_complete (assist, page, TRUE);
          cp2k_with_motion ();
        }
      }
      else if (i == CP2ENS)
      {
        if (j == 2 || j == 3)
        {
          widget_set_sensitive (cp2k_thermo_box, FALSE);
        }
        else
        {
          widget_set_sensitive (cp2k_thermo_box, TRUE);
        }
      }
      print_start_buffer ();
    }
  }
}

/*!
  \fn void cp2k_option_box (GtkWidget * box, int id)

  \brief create a CP2K option combo box

  \param box the box to insert the new widgets in
  \param id the option id
*/
void cp2k_option_box (GtkWidget * box, int id)
{
  optionb ++;
  option_box[optionb] = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, option_box[optionb], FALSE, FALSE, 5);
  if (id != CP2SPI)
  {
   add_box_child_start (GTK_ORIENTATION_HORIZONTAL, option_box[optionb],
                        create_button ("Options", IMG_STOCK, DPROPERTIES, 200, -1, GTK_RELIEF_NORMAL, G_CALLBACK(cp2k_option_dialog), GINT_TO_POINTER(id)),
                        FALSE, FALSE, 50);
  }
  widget_set_sensitive (option_box[optionb], (int)tmp_cp2k -> opts[id]);
}

/*!
  \fn GtkWidget * cp2k_section_box (int s)

  \brief create the CP2K assistant section box

  \param s the section id
*/
GtkWidget * cp2k_section_box (int s)
{
  int i, j, k;
  GtkWidget * hbox;
  GtkWidget * widg;
  GtkWidget * vbox = create_vbox (BSEP);
  for (i=0; i<NC2OP; i++)
  {
    if (cp2k_opts_type[s][i] > 0)
    {
      idopt ++;
      j = idopt;
      if (j == CP2NST)
      {
        cp2k_thermo_box = thermo_box();
        add_box_child_start (GTK_ORIENTATION_VERTICAL, motion_box[motionb], cp2k_thermo_box, FALSE, FALSE, 5);
        hbox = cpmd_box (motion_box[motionb], global_opts[idopt], 5, 5, 300);
      }
      else if (j == CP2FRE || j == CP2FWV)
      {
        checkb ++;
        checked_box[checkb] = cpmd_box (vbox, global_opts[idopt], 5, 5, 300);
      }
      else if (j == CP2SPM)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, option_box[optionb], markup_label(global_opts[idopt], 100, 30, 0.0, 0.5), FALSE, FALSE, 50);
      }
      else if (j == CP2PFO || j == CP2PLO)
      {
        if (j == CP2PFO) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<u>Print:</u>", -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
        print_box = create_hbox (0);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, print_box, FALSE, FALSE, 0);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, print_box, markup_label(global_opts[idopt], 150, 30, 0.0, 0.5), FALSE, FALSE, 50);
      }
      else if (j > CP2ROK && j < CP2PBC)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, print_box, markup_label(global_opts[idopt], 150, 30, 0.0, 0.5), FALSE, FALSE, 50);
      }
      else if (j == CP2ENS || j == CP2GMI)
      {
        motionb ++;
        motion_box[motionb] = create_vbox (BSEP);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, motion_box[motionb], FALSE, FALSE, 5);
        hbox = cpmd_box (motion_box[motionb], global_opts[idopt], 5, 5, 300);
      }
      else if (j > CP2ENS && j < CP2OUF)
      {
        hbox = cpmd_box (motion_box[motionb], global_opts[idopt], 5, 5, 300);
      }
      else if (j == CP2SCN || j == CP2SCO)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(global_opts[idopt], 150, 30, 0.0, 0.5), FALSE, FALSE, 30);
      }
      else
      {
        hbox = cpmd_box (vbox, global_opts[idopt], 5, 5, 300);
      }
      switch (cp2k_opts_type[s][i])
      {
        case 1:
          ident ++;
          widg = create_entry (G_CALLBACK(update_cp2k_parameter), 100, 15, FALSE, GINT_TO_POINTER(j));
          if (j==CP2CUT || j==CP2SCN || j==CP2SCO || j==CP2DLT || j==CP2GEF)
          {
            update_entry_double (GTK_ENTRY(widg), tmp_cp2k -> opts[j]);
          }
          else
          {
            update_entry_int (GTK_ENTRY(widg), tmp_cp2k -> opts[j]);
          }
          break;
        case 2:
          icomb ++;
          widg = create_combo ();
          for (k=0; k<cp2k_default_num[icomb]; k++) combo_text_append (widg, cp2k_default_text[icomb][k]);
          combo_set_active (widg, (int)tmp_cp2k -> opts[j]);
          g_signal_connect (G_OBJECT (widg), "changed", G_CALLBACK(changed_cp2k_box), GINT_TO_POINTER(j));
          break;
        case 3:
          widg = check_button (NULL, -1, -1, tmp_cp2k -> opts[j], G_CALLBACK(update_cp2k_check), GINT_TO_POINTER(j));
          break;
        case 4:
          widg = create_text_view (200, -1, 1, 0, G_CALLBACK(cp2k_file_info), GINT_TO_POINTER(j), tmp_cp2k -> files[j-CP2FRE]);
          break;
        default:
          break;
      }
      if (j==CP2FRE || j==CP2FWV)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, checked_box[checkb], widg, FALSE, FALSE, 5);
        widget_set_sensitive (checked_box[checkb], (int)tmp_cp2k -> opts[CP2RES]);
      }
      else if (j==CP2SPM)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, option_box[optionb], widg, FALSE, FALSE, 5);
      }
      else if (j > CP2ROK && j < CP2PBC)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, print_box, widg, FALSE, FALSE, 5);
        if (j == CP2POR) cp2k_option_box (print_box, j);
      }
      else
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 5);
        if (j==CP2SPI || j==CP2VDW || j==CP2ROK || j==CP2CON) cp2k_option_box (hbox, j);
      }

      if (j==CP2KTI || j==CP2CUT || j==CP2DLT || j==CP2TMP || j==CP2GEF || j==CP2OUF)
      {
        idunit ++;
        widg = gtk_label_new (cp2k_unit[idunit]);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 5);
      }

      if (idopt == CP2SYM)
      {
        gchar * str = g_strdup_printf (" %s provides official CP2K atomic basis sets and pseudo-potentials, are included:", PACKAGE);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 20);
        g_free (str);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<sup><tt>\t   - BASIS_MOLOPT\t\t(CP2K v9.1, released: <b>2021-12-31</b>)</tt></sup>\n"
                                                         "<sup><tt>\t   - GTH_BASIS_SETS\t\t(CP2K v9.1, released: <b>2021-12-31</b>)</tt></sup>\n"
                                                         "<sup><tt>\t   - BASIS_SET\t\t(CP2K v9.1, released: <b>2021-12-31</b>)</tt></sup>\n"
                                                         "<sup><tt>\t   - POTENTIAL\t\t(CP2K v9.1, released: <b>2021-12-31</b>)</tt></sup>",
                                                         -1, -1, 0.1, 0.5), FALSE, FALSE, 0);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(" Following your selection the files that contains the parameters for your calculation will be generated automatically.\n"
                                                         " Alternatively it is possible to define and use your own set of parameters.",
                                                         -1, -1, 0.0, 0.5), FALSE, FALSE, 20);
        hbox = cpmd_box (vbox, "\t\t\t\tChemical species", 20, 5, 300);
        cp2k_spec_combo = create_combo ();
        for (k=0; k<qm_proj -> nspec; k++) combo_text_append (cp2k_spec_combo, qm_proj -> chemistry -> label[k]);
        combo_set_active (cp2k_spec_combo, 0);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, cp2k_spec_combo, FALSE, FALSE, 5);
        cp2k_spec_box[0] = create_hbox (0);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, cp2k_spec_box[0], FALSE, FALSE, 0);
        cp2k_spec_box[1] = create_cp2k_spec_box (0);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cp2k_spec_box[0], cp2k_spec_box[1], FALSE, FALSE, 0);
        g_signal_connect (G_OBJECT (cp2k_spec_combo), "changed", G_CALLBACK(changed_cp2k_box), GINT_TO_POINTER(j));
      }
    }
  }
  return vbox;
}

/*!
  \fn gchar * cp_section_name (int p)

  \brief get CP2K section name

  \param p the section id
*/
gchar * cp_section_name (int p)
{
  if (p < 4)
  {
    return g_strdup_printf ("Details of the <b>%s</b> section that %s", cpsect[p], cpelemts[p+1]);
  }
  else
  {
    return g_strdup_printf ("%s", cpelemts[p+1]);
  }
}

/*!
  \fn GtkWidget * vbox_cp2k (int s)

  \brief create the CP2K assistant page vertical box

  \param s the section id
*/
GtkWidget * vbox_cp2k (int s)
{
  GtkWidget * vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(cp_section_name(s), -1, 20, 0.0, 0.5), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, cp2k_section_box(s), FALSE, FALSE, 0);
  if (s== 0) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, qm_preview_box (1, s, 0), FALSE, FALSE, 5);
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void select_input_type (GtkToggleButton * but, gpointer data)

  \brief select the input file type

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_input_type (GtkToggleButton * but, gpointer data)
{
  tmp_cp2k -> input_type = GPOINTER_TO_INT (data);
  widget_set_sensitive (fileinf[! tmp_cp2k -> input_type], FALSE);
  widget_set_sensitive (fileinf[tmp_cp2k -> input_type], TRUE);
  print_start_buffer ();
}

/*!
  \fn GtkWidget * file_info (int id)

  \brief create the assistant page that present the input file(s) properties

  \param id the input file(s) type (0 = single, 1 = multiple)
*/
GtkWidget * file_info (int id)
{
  GtkWidget * main_box;
  gchar * filename[4] = {"CP2K main input file '*.inp'", "forces.inc", "system.inc", "motion.inc"};
  gchar * filetext[4] = {"<b>&amp;GLOBAL</b>\n...\n<b>&amp;END GLOBAL</b>\n\n<b>@INCLUDE</b> 'forces.inc'\n\n<b>@INCLUDE</b> 'motion.inc'",
                         "<b>&amp;FORCE_EVAL</b>\n...\n<b>@INCLUDE</b> 'system.inc'\n<b>&amp;END FORCE_EVAL</b>",
                         "<b>&amp;SUBSYS</b>\n...\n<b>&amp;END SUBSYS</b>",
                         "<b>&amp;MOTION</b>\n...\n<b>&amp;END MOTION</b>"};
  GtkWidget * vbox, * vbx, * hbx;
  GtkWidget * vframe;
  if (id == 0)
  {
    main_box = create_vbox (BSEP);
    hbx = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, stock_image(AFILE), FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, gtk_label_new ("CP2K input file '*.inp'"), FALSE, FALSE, 5);
    vframe = gtk_frame_new (NULL);
    gtk_frame_set_label_widget (GTK_FRAME(vframe), hbx);
#ifdef GTK3
    gtk_frame_set_shadow_type (GTK_FRAME(vframe), GTK_SHADOW_ETCHED_IN);
#endif
    add_box_child_start (GTK_ORIENTATION_VERTICAL, main_box, vframe, FALSE, FALSE, 10);
    vbox = create_vbox (BSEP);
    add_container_child (CONTAINER_FRA, vframe, vbox);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("! Information", 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>&amp;GLOBAL</b>\n...\n<b>&amp;END GLOBAL</b>", 100, -1, 0.0, 0.5), FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>&amp;FORCE_EVAL</b>\n...", 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
    hbx  = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbx, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, markup_label("<b>&amp;SUBSYS</b>\n...\n<b>&amp;END SUBSYS</b>", 150, -1, 0.0, 0.5), FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>&amp;END FORCE_EVAL</b>", 200, -1, 0.0, 0.5), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b>&amp;MOTION</b>\n...\n<b>&amp;END MOTION</b>", 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
  }
  else
  {
    main_box = create_hbox (0);
    vbox = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, main_box, vbox, FALSE, FALSE, 0);
    int i;
    for (i=0; i<4; i++)
    {
      hbx = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, stock_image(AFILE), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, gtk_label_new (filename[i]), FALSE, FALSE, 5);
      vframe = gtk_frame_new (NULL);
      gtk_frame_set_label_widget (GTK_FRAME(vframe), hbx);
#ifdef GTK3
      gtk_frame_set_shadow_type (GTK_FRAME(vframe), GTK_SHADOW_ETCHED_IN);
#endif
      vbx = create_vbox (BSEP);
      add_container_child (CONTAINER_FRA, vframe, vbx);
      if (i == 0) add_box_child_start (GTK_ORIENTATION_VERTICAL, vbx, markup_label("! Information", 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbx, markup_label(filetext[i], 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, vframe, FALSE, FALSE, 5);
      if (i == 0)
      {
        vbox = create_vbox (BSEP);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, main_box, vbox, FALSE, FALSE, 20);
      }
    }
  }
  return main_box;
}

/*!
  \fn GtkWidget * init_cp2k ()

  \brief initialize the CP2K input creation assistant
*
*/
GtkWidget * init_cp2k ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  gchar * text = "The CP2K input system offers to have the information gathered in a single, rather long, file\n"
                 "or separated in several files each of them dedicated to a particular section of the input: ";
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, gtk_label_new (text), FALSE, FALSE, 20);
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  GtkWidget * hbx;
  gchar * ftext[2] = {"Single input file", "Multiple files"};
  int i, j;
  j = tmp_cp2k -> input_type;
  for (i=0; i<2; i++)
  {
    vfbox[i] = create_vbox (BSEP);
    gtk_widget_set_size_request (vfbox[i], (i==0) ? 200 : 400, -1);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vfbox[i], FALSE, FALSE, (i==0) ? 50 : 10);
    fileinf[i] = file_info (i);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vfbox[i], fileinf[i], FALSE, FALSE, (i==0) ? 0 : 15);
    hbx = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vfbox[i], hbx, FALSE, FALSE, 0);

#ifdef GTK4
    filebut[i] = check_button (ftext[i], -1, -1, (i != j) ? FALSE : TRUE, G_CALLBACK(select_input_type), GINT_TO_POINTER(i));
    if (i) gtk_check_button_set_group ((GtkCheckButton *)filebut[i], (GtkCheckButton *)filebut[0]);
#else
    if (! i)
    {
      filebut[i] = radio_button (ftext[i], -1, -1, (i != j) ? FALSE : TRUE, G_CALLBACK(select_input_type), GINT_TO_POINTER(i));
    }
    else
    {
      filebut[i] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(filebut[0]), ftext[i]);
    }
#endif
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, filebut[i], FALSE, FALSE, (i==0) ? 50 : 120);
  }
  tmp_cp2k -> input_type = j;
  widget_set_sensitive (fileinf[tmp_cp2k -> input_type], TRUE);
  widget_set_sensitive (fileinf[! tmp_cp2k -> input_type], FALSE);
  show_the_widgets (vbox);
  return vbox;
}

/*!
  \fn gchar * page_name_cp2k (int p)

  \brief get CP2K assistant page name

  \param p the page id
*/
gchar * page_name_cp2k (int p)
{
  if (p == 0)
  {
    return g_strdup_printf ("The CP2K input structure");
  }
  else if (p < 5)
  {
    return g_strdup_printf ("The %s section", cpsect[p-1]);
  }
  else if (p == 5)
  {
    return g_strdup_printf ("The MOTION section - thermostat(s)");
  }
  else
  {
    return g_strdup_printf ("The MOTION section - restart(s)");
  }
}

/*!
  \fn gboolean cp2k_with_motion ()

  \brief CP2K MD calculation ?
*/
gboolean cp2k_with_motion ()
{
  if (GTK_IS_WIDGET(motion_box[0])) hide_the_widgets (motion_box[0]);
  if (GTK_IS_WIDGET(motion_box[1])) hide_the_widgets (motion_box[1]);
  if (tmp_cp2k -> opts[CP2RUN] == 2.0 || tmp_cp2k -> opts[CP2RUN] == 3.0 || tmp_cp2k -> opts[CP2RUN] == 6.0)
  {
    int i = (tmp_cp2k -> opts[CP2RUN] == 2.0) ? 1 : 0;
    if (GTK_IS_WIDGET(motion_box[i])) show_the_widgets (motion_box[i]);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn void add_cp2k_pages ()

  \brief add pages to the CP2 assistant
*/
void add_cp2k_pages ()
{
  int i;
  gchar * info;
  GtkAssistant * assist = GTK_ASSISTANT(qm_assistant);
  GtkWidget * page;
  idopt = -1;
  ident = -1;
  icomb = -1;
  idunit = -1;
  checkb = -1;
  optionb = -1;
  motionb = -1;
  for (i=0; i<(cp2k_with_motion() ? 5: 4); i++)
  {
    if (i > 0)
    {
      page = vbox_cp2k (i-1);
    }
    else
    {
      page = init_cp2k ();
    }
    gtk_assistant_append_page (assist, page);
    gtk_assistant_set_page_title (assist, page, page_name_cp2k(i));
    gtk_assistant_set_page_type (assist, page, GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (assist, page, TRUE);
  }
  GtkWidget * conclu = create_vbox (BSEP);
  info = g_strdup_printf ("<b>   Finalize the creation of the CP2K input files now !</b>");
  add_box_child_start (GTK_ORIENTATION_VERTICAL, conclu, markup_label(info, -1, -1, 0.5, 0.5), TRUE, TRUE, 100);
  g_free (info);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, conclu, markup_label("\n \t<b>Note: </b>You can re-open this assistant later if required to adjust your choices\n", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
  gtk_assistant_append_page (assist, conclu);
  gtk_assistant_set_page_title (assist, conclu, "Create the input files now !");
  gtk_assistant_set_page_type (assist, conclu, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (assist, conclu, TRUE);
  gtk_assistant_update_buttons_state (assist);
}

/*!
  \fn int find_cp2k_sym ()

  \brief find the CP2K symmetry parameter based on the cell properties
*
*/
int find_cp2k_sym ()
{
  box_info * box = & qm_proj -> cell.box[0];
  if (box -> param[0][0] == box -> param[0][1] &&
      box -> param[0][0] == box -> param[0][2] &&
      box -> param[0][1] == box -> param[0][2])
  {
    if (box -> param[1][0] == box -> param[1][1] &&
        box -> param[1][0] == box -> param[1][2] &&
        box -> param[1][1] == box -> param[1][2] &&
        box -> param[1][0] == 90.0) return 0;
    if (box -> param[1][0] == box -> param[1][1] &&
        box -> param[1][0] == box -> param[1][2] &&
        box -> param[1][1] == box -> param[1][2] &&
        box -> param[1][0] != 90.0) return 3;
  }
  if (box -> param[1][0] == box -> param[1][1] &&
      box -> param[1][0] == box -> param[1][2] &&
      box -> param[1][1] == box -> param[1][2] &&
      box -> param[1][0] == 90.0)
  {
    if (box -> param[0][0] == box -> param[0][1] && box -> param[0][0] != box -> param[0][2]) return 4;
    if (box -> param[0][0] == box -> param[0][2] && box -> param[0][0] != box -> param[0][1]) return 5;
    if (box -> param[0][1] == box -> param[0][2] && box -> param[0][1] != box -> param[0][0]) return 6;
  }
  if (box -> param[1][0] == box -> param[1][1] &&
      box -> param[1][0] == box -> param[1][2] &&
      box -> param[1][1] == box -> param[1][2])
  {
    if (box -> param[0][0] != box -> param[0][1] &&
        box -> param[0][0] != box -> param[0][2] &&
        box -> param[0][1] != box -> param[0][2]) return 2;
  }
  if (box -> param[1][0] == box -> param[1][1] && box -> param[1][0] == 90.0)
  {
    if (box -> param[1][2] == 60.0)
    {
      if (box -> param[0][0] == box -> param[0][1] && box -> param[0][0] != box -> param[0][2]) return 1;
    }
  }
  if (box -> param[1][0] == box -> param[1][2] && box -> param[1][0] == 90.0)
  {
    if (box -> param[1][1] != 90.0)
    {
      if (box -> param[0][0] != box -> param[0][1] &&
          box -> param[0][0] != box -> param[0][2] &&
          box -> param[0][1] != box -> param[0][2]) return 7;
    }
  }
  if (box -> param[0][0] != box -> param[0][1] &&
      box -> param[0][0] != box -> param[0][2] &&
      box -> param[0][1] != box -> param[0][2])
  {
    if (box -> param[1][0] != box -> param[1][1] &&
        box -> param[1][0] != box -> param[1][2] &&
        box -> param[1][1] != box -> param[1][2]) return 8;
  }
  return 9;
}
