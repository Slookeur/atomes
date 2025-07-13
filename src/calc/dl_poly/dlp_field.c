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
* @file dlp_field.c
* @short  Functions to prepare the DL-POLY / LAMMPS input preparation assistant \n
          Functions to design a classical force field
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_field.c'
*
* Contains:
*

 - The functions to prepare the DL-POLY / LAMMPS input preparation assistant
 - The functions to design a classical force field

*
* List of functions:

  int struct_id (int f);
  int body_at (int b);
  int get_field_tree_data (GtkWidget * tree, int treeid, GtkTreePath * path);
  int get_field_data_id (int k, gchar * data);

  G_MODULE_EXPORT gint on_assistant_go_forward (gint current_page, gpointer data);

  gboolean set_nbd_but_sensitive (int nbid);
  gboolean field_file_has_energy_parameters (gboolean scale, int sca, int scb);

  G_MODULE_EXPORT gboolean on_pop_up_field (GtkWidget * widget, gpointer data);
  G_MODULE_EXPORT gboolean on_field_button_event (GtkWidget * widget, GdkEvent * event, gpointer data);
  G_MODULE_EXPORT gboolean on_assistant_cancel_event (GtkWindow * assistant, gpointer data);
  G_MODULE_EXPORT gboolean on_assistant_cancel_event (GtkWidget * assistant, GdkEvent * event, gpointer data);

  gchar * parameters_info (int obj, int key,  gchar ** words, float * data);
  gchar * set_field_label (int f, int m);
  gchar * pop_info (int i, int id);
  gchar * pop_edit (int i);
  gchar * pop_add (int i);
  gchar * pop_remove (int i);

  void set_mol_num_label ();
  void setup_cs_labels (int i);
  void fill_field_struct (GtkTreeStore * store, int id, int mo);
  void fill_field_body (GtkTreeStore * store, int id);
  void fill_field_model (GtkTreeStore * store, int f, int m);
  void update_field_trees ();
  void get_is_energy (int i, int l);
  void append_field_item (GMenu * menu, const gchar * name, const gchar * key, int item_id,
                          gchar * accel, int image_format, gpointer icon,
                          gboolean custom, GCallback handler, gpointer data,
                          gboolean check, gboolean status, gboolean radio, gboolean sensitive);
  void pop_up_field_context_menu (int row_id, GtkWidget * widget, double event_x, double event_y, gpointer data);
  void pop_up_field_context_menu (int row_id, GtkWidget * widget, GdkEvent * event, gpointer data);
  void field_button_event (double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data);
  void field_button_event (GdkEvent * event, double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data);
  void field_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void field_set_markup_box (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void field_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void field_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void field_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void field_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void prop_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void pmf_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void rig_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void field_set_color_markup_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void field_set_color_markup_and_visible_box (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void get_field_iter_and_edit (gchar * path_string, gpointer data);
  void create_field_list (GtkWidget * vbx, int f);
  void close_the_assistant (GtkAssistant * assistant);
  void hide_show_this_pages (int start, int end, int status, int delta);
  void remove_classical_assistant_pages ();
  void add_classical_assistant_pages (int p);
  void create_ff_structure (int ai, int type);
  void on_assistant_apply (GtkAssistant * assistant, gpointer data);
  void create_classical_force_field (int p, int f);

  G_MODULE_EXPORT void toggle_field_params (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void toggle_field_params (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void changed_mol_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void run_changed_energy_unit (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void changed_energy_unit (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void changed_field_key_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data);
  G_MODULE_EXPORT void to_select_atom_id_from_fied_molecule (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void on_field_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
  G_MODULE_EXPORT void on_field_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
  G_MODULE_EXPORT void edit_field_cell (GtkCellRendererText * cell, gchar * path_string,  gchar * new_text, gpointer data);
  G_MODULE_EXPORT void to_edit_field_prop (GtkCellRenderer * cell, GtkCellEditable * editable, gchar * path_string, gpointer data);
  G_MODULE_EXPORT void on_field_row_activated (GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * col, gpointer data);
  G_MODULE_EXPORT void on_assistant_cancel (GtkAssistant * assistant, gpointer data);
  G_MODULE_EXPORT void on_assistant_close (GtkAssistant * assistant, gpointer data);
  G_MODULE_EXPORT void on_assistant_prepare (GtkAssistant * assistant, GtkWidget * page, gpointer data);
  G_MODULE_EXPORT void run_clean_field (GtkDialog * dial, gint response_id, gpointer data);
  G_MODULE_EXPORT void clean_field (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void clean_field (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void select_field_action (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void select_field_action (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void changed_init_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void show_force_field_preview (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void run_on_assistant_apply (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_on_assistant_apply (GtkDialog * info, gint response_id, gpointer data);

  GtkWidget * vbox_init (int p);
  GtkWidget * create_combo_mol (int f);
  GtkWidget * create_field_tree (int f);
  GtkWidget * create_mol_box (int f);
  GtkWidget * vbox_field (int f);

*/

#include "dlp_field.h"
#include "calc.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"
#include "bind.h"

gboolean append_pages;
extern GtkWidget * extra_vbox[2];
extern ColRGBA init_color (int id, int numid);
extern void restore_ogl_selection (glwin * view);

gchar * felemts[MAXDATA+1] = {"Energy unit",
                              "Molecule",
                              "Atom",
                              "Core-shell unit",
                              "Constraint bond",
                              "Mean force potential",
                              "Rigid unit",
                              "Tethering potential",
                              "Flexible chemical bond",
                              "Bond restraint",
                              "Bond angle",
                              "Angular restraint",
                              "Dihedral angle",
                              "Torsional restraint",
                              "Improper angle",
                              "Inversion angle",
                              "van der Waals potential",
                              "Metal potential",
                              "Tersoff Potential",
                              "Three-body potential",
                              "Four-body potential",
                              "External field"};

gchar * elemts[MAXDATA] = {"molecule",
                           "atom",
                           "core-shell unit",
                           "constraint bond",
                           "mean force potential",
                           "rigid unit",
                           "tethering potential",
                           "flexible chemical bond",
                           "bond restraint",
                           "bond angle",
                           "angular restraint",
                           "dihedral angle",
                           "torsional restraint",
                           "improper angle",
                           "inversion angle",
                           "van der Waals potential",
                           "metal potential",
                           "Tersoff Potential",
                           "three-body potential",
                           "four-body potential",
                           "external field"};

gchar * fkeysw[2][16][21] = {{{"eV", "kcal", "kJ", "K", "internal", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                                       //  0 - Energy unit(s)
                              {"harm", "rhrm", "quar", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                                          //  1 - Tethering pot.
                              {"harm", "mors", "12-6", "lj  ", "rhrm", "quar", "buck", "coul", "fene", "mmst", "tab ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                  //  2 - Bond pot.
                              {"-hrm", "-mrs", "-126", "-lj ", "-rhm", "-qur", "-bck", "-cul", "-fne", "-mst", "-tab", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                  //  3 - Bond rest.
                              {"harm", "quar", "thrm", "shrm", "bvs1", "bvs2", "hcos", "cos ", "mmsb", "stst", "stbe", "cmps", "mmbd", "kky ", "tab ", " ", " ", " ", " ", " ", " "},                      //  4 - Angles pot.
                              {"-hrm", "-qur", "-thm", "-shm", "-bv1", "-bv2", "-hcs", "-cos", "-msb", "-sts", "-stb", "-cmp", "-mbd", "-kky", "-tab", " ", " ", " ", " ", " ", " "},                      //  5 - Angles rest.
                              {"cos ", "harm", "hcos", "cos3", "ryck", "rbf ", "opls", "tab ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                           //  6 - Dihedral pot.
                              {"-cos", "-hrm", "-hcs", "-cs3", "-rck", "-rbf", "-opl", "-tab", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                           //  8 - Torsion pot.
                              {"cos ", "harm", "hcos", "cos3", "ryck", "rbf ", "opls", "tab ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                           //  7 - Improper pot.
                              {"harm", "hcos", "plan", "xpln", "calc", "tab ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                                 //  9 - Inversion pot.
                              {"12-6", "lj  ", "ljc", "nm ", "buck", "bhm ", "hbnd", "snm ", "mors", "wca ", "dpd", "14-7", "mstw", "ryb", "zbl", "zbls", "zblb", "mlj", "mbuc", "m126", "tab "},          // 10 - Vdw pot.
                              {"eam ", "eeam", "2bea", "2bee", "fnsc", "exfs", "stch", "gupt", "mbpc", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                        // 11 - Metal pot.
                              {"ters", "kihs", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                                             // 12 - Tersoff pot.
                              {"harm", "thrm", "shrm", "bvs1", "bvs2", "hbnd", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                                 // 13 - Three-body pot.
                              {"harm", "hcos", "plan", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},                                                          // 14 - Four-body pot.
                              {"elec", "oshr", "shrx", "grav", "magn", "sphr", "zbnd", "xpis", "zres", "zrs-", "zrs+", "osel", "ushr", " ", " ", " ", " ", " ", " ", " ", " "}},                           // 15 - External fields.
                             {{"lj", "real", "metal", "si", "cgs", "electron", "micro", "nano", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"harmonic", "class2", "fene", "fene/expand", "gromos", "harmonic/shift", "harmonic/shift/cut", "mm3", "morse", "nonlinear", "oxdna/fene", "oxdna2/fene", "oxrna2/fene", "quartic", "special", "table", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"harmonic", "charmm", "class2", "class2/p6", "cosine", "cosine/buck6d", "cosine/delta", "cosine/periodic", "cosine/shift", "cosine/shift/exp", "cosine/squared", "cross", "dipole", "fourier", "fourier/simple", "mm3", "quartic", "sdk", "table", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"cosine/shift/exp", "charmm", "charmmfsw", "class2", "fourier", "harmonic", "helix", "multi/harmonic", "nharmonic", "opls", "quadratic","spherical", "table", "table/cut", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"harmonic", "class2", "cossq", "cvff", "distance", "distharm", "fourier", "ring", "sqdistharm", "umbrella", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"inversion/harmonic", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}}};

gchar * fnames[2][16][21] = {{{"Electron-Volts", "k-calories per mol", "k-Joules per mol", "Kelvin per Boltzmann", "DL_POLY internal units", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "Restraint", "Quartic", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "Morse", "12-6", "Lennard-Jones", "Restraint", "Quartic", "Buckingham", "Coulomb", "Shifted FENE", "MM3 bond stretch", "Tabulated", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "Morse", "12-6", "Lennard-Jones", "Restraint", "Quartic", "Buckingham", "Coulomb", "Shifted FENE", "MM3 bond stretch", "Tabulated", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "Quartic", "Truncated harmonic", "Screened harmonic", "Screened Vessal", "Truncated Vessal", "Harmonic cosine", "Cosine", "MM3 stretch-bend", "Compass stretch-stretch", "Compass stretch-bend", "Compass all terms", "MM3 angle bend", "KKY", "Tabulated", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "Quartic", "Truncated harmonic", "Screened harmonic", "Screened Vessal", "Truncated Vessal", "Harmonic cosine", "Cosine", "MM3 stretch-bend", "Compass stretch-stretch", "Compass stretch-bend", "Compass all terms", "MM3 angle bend", "KKY", "Tabulated", " ", " ", " ", " ", " ", " "},
                              {"Cosine", "Harmonic", "Harmonic cosine", "Triple cosine", "Ryckaert-Bellemans", "Fluorinated Ryckaert-Bellemans", "OPLS torsion", "Tabulated", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Cosine", "Harmonic", "Harmonic cosine", "Triple cosine", "Ryckaert-Bellemans", "Fluorinated Ryckaert-Bellemans", "OPLS torsion", "Tabulated", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Cosine", "Harmonic", "Harmonic cosine", "Triple cosine", "Ryckaert-Bellemans", "Fluorinated Ryckaert-Bellemans", "OPLS torsion", "Tabulated", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "Harmonic cosine", "Planar", "Extended Planar", "Calcite", "Tabulated", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"12-6", "Lennard-Jones", "LJ cohesive", "n-m", "Buckingham", "Born-Huggins-Meyer", "12-10 H-bond", "Shifted force n-m", "Morse", "Shifted Weeks-Chandler-Anderson", "Standard DPD", "14-7 buffered AMOEBA FF",
                               "Morse modified", "Rydberg", "ZBL", "ZBL mixed with Morse", "ZBL mixed with Buckingham", "Lennard-Jones tapered with MDF", "Buckingham tapered with MDF", "12-6 Lennard-Jones tapered with MDF", "Tabulated"},
                              {"EAM", "EEAM", "2BEAM", "2BEEAM", "Finis-Sinclair", "Extended Finis-Sinclair", "Sutton-Chen", "Gupta", "MBPC", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Tersoff", "KIHS", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "Truncated harmonic", "Screened harmonic", "Screened Vessal", "Truncated Vessal", "H-bond", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "Harmonic cosine", "Planar", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Electric field", "Oscillating Shear", "Continuous Shear", "Gravitational field", "Magnetic field", "Containing Sphere", "Repulsive Wall", "X-Piston", "Molecule in HR Zone", "HR Zone (pull out)", "HR Zone (pull in)", "Osc. Electric Field", "Umbrella sampling, harm. constant", " ", " ", " ", " ", " ", " ", " ", " "}},
                             {{"lj", "k-calories per mol", "Electron-Volts", "k-Joules per mol", "cgs", "electron", "micro", "nano", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic","COMPASS class2 force field", "Finite extensible nonlinear elastic (FENE)", "FENE with variable size particles", "GROMOS force field", "Shifted harmonic", "Shifted truncated harmonic", "MM3 anharmonic", "Morse", "Nonlinear", "Finite extensible nonlinear elastic DNA",
                               "Finite extensible nonlinear elastic DNA (2)", "Finite extensible nonlinear elastic RNA", "Breakable quartic", "Special bond exclusions for 1-5 pairs and beyond", "Tabulated", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "CHARMM force field", "COMPASS class2 force field", "COMPASS class2 force field - 6th order", "Cosine", "Cosine with Buckingham term between 1-3", "Difference of cosines", "DREIDING force field", "Cosine with a shift", "Cosine with a shift and exponential term in spring",
                               "Cosine with squared term", "Cross term coupling angle and bond length", "Dipole orientation", "Fourier (multiple cosines)", "Fourier (single cosine)", "MM3", "Quartic", "Combination of the harmonic (SDK)", "Tabulated", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Cosine with a shift and exponential term in spring", "CHARMM force field", "CHARMM force field with force switching", "COMPASS class2 force field", "Fourier (multiple cosines)", "Harmonic", "Helix", "Harmonic with 5 terms",
                               "Harmonic with N terms", "OPLS force field", "Quadratic", "Spherical", "Tabulated", "Tabulated with analytic cutoff", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic", "COMPASS class2 force field", "Cosine squared", "CVFF force field", "Distance between atom planes", "Out-of-the plane distance", "Fourier (multiple cosines)", "Planar conformation", "Squared distance harmonic", "DREIDING force field", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {"Harmonic with Wilson Decius out-of-plane", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                              {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}}};

int fvalues[2][15][21] = {{{ 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  0 - Tethered
                           { 2, 3, 2, 2, 3, 4, 3, 1, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  1 - Bond(s)
                           { 2, 3, 2, 2, 3, 4, 3, 1, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  2 - Bond restraint(s)
                           { 2, 4, 3, 4, 4, 4, 2, 3, 4, 3, 3, 6, 2, 4, 0, 0, 0, 0, 0, 0, 0},  //  3 - Angle(s)
                           { 2, 4, 3, 4, 4, 4, 2, 3, 4, 3, 3, 6, 2, 4, 0, 0, 0, 0, 0, 0, 0},  //  4 - Angle restraint(s)
                           { 3, 2, 2, 3, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  5 - Dihedral(s)
                           { 3, 2, 2, 3, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  6 - Torsional restraint(s)
                           { 3, 2, 2, 3, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  7 - Improper(s)
                           { 2, 2, 1, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  8 - Inversion(s)
                           { 2, 2, 3, 4, 3, 5, 2, 5, 3, 3, 2, 2, 4, 3, 2, 7, 7, 3, 4, 3, 0},  //  9 - vdW
                           { 0, 0, 0, 0, 7, 9, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 10 - Metal(s)
                           {11,16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 11 - Tersoff(s)
                           { 2, 3, 4, 4, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 12 - 3-body
                           { 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 13 - 4-body
                           { 3, 2, 2, 3, 3, 4, 3, 3, 5, 5, 5, 4, 6, 0, 0, 0, 0, 0, 0, 0, 0}}, // 14 - External(s)
                          {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  0 - Tethered
                           { 2, 4, 4, 5, 2, 3, 3, 2, 3, 3, 3, 3, 3, 5, 2, 2, 0, 0, 0, 0, 0},  //  1 - Bond(s)
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  2 - Bond restraint(s)
                           { 2, 4, 4, 4, 1, 3, 2, 3, 2, 3, 2, 6, 2, 4, 3, 2, 4, 2, 2, 0, 0},  //  3 - Angle(s)
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  4 - Angle restraint(s)
                           { 3, 4, 4, 6, 4, 3, 3, 5, 2, 4, 2,11, 2, 2, 0, 0, 0, 0, 0, 0, 0},  //  5 - Dihedral(s)
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  6 - Torsional restraint(s)
                           { 2, 2, 2, 3, 2, 2, 5, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  7 - Improper(s)
                           { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  8 - Inversion(s)
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  //  9 - vdW
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 10 - Metal(s)
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 11 - Tersoff(s)
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 12 - 3-body
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 13 - 4-body
                           { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};// 14 - External(s)

int fetypes[2][16] = {{DL_ENERGY, DL_TETH, DL_BONDS, DL_BONDS, DL_ANGLES, DL_ANGLES, DL_DIHEDRAL, DL_DIHEDRAL, DL_DIHEDRAL, DL_INVERS, DL_VDW, DL_METALS, DL_TERSOFFS, DL_THREEBODY, DL_FOURBODY, DL_EXTERNAL},
                      {LA_ENERGY, LA_TETH, LA_BONDS, 1, LA_ANGLES, 1, LA_DIHEDRAL, 1, LA_DIHEDRAL, LA_INVERS, LA_VDW, LA_METALS, LA_TERSOFFS, LA_THREEBODY, LA_FOURBODY, LA_EXTERNAL}};

int fntypes[2][15] = {{DL_TETH_P, DL_BONDS_P, DL_BONDS_P, DL_ANGLES_P, DL_ANGLES_P, DL_DIHEDRAL_P, DL_DIHEDRAL_P, DL_DIHEDRAL_P, DL_INVERS_P, DL_VDW_P, DL_METALS_P, DL_TERSOFFS_P, DL_THREEBODY_P, DL_FOURBODY_P, DL_EXTERNAL_P},
                      {LA_TETH_P, LA_BONDS_P, LA_BONDS_P, LA_ANGLES_P, LA_ANGLES_P, LA_DIHEDRAL_P, LA_DIHEDRAL_P, LA_DIHEDRAL_P, LA_INVERS_P, LA_VDW_P, LA_METALS_P, LA_TERSOFFS_P, LA_THREEBODY_P, LA_FOURBODY_P, LA_EXTERNAL_P}};

char * fvars_teth[2][FTETH][FTETH_P] = {{{"k", " ", " "},
                                         {"k", "r<sub>c</sub>", " "},
                                         {"k", "k'", "k''"}},
                                        {{" ", " ", " "},
                                         {" ", " ", " "},
                                         {" ", " ", " "}}};

int feunit_teth[2][FTETH][FTETH_P] = {{{1, 0, 0},
                                       {1, 0, 0},
                                       {1, 0, 0}},
                                      {{0, 0, 0},
                                       {0, 0, 0},
                                       {0, 0, 0}}};

char * fvars_bond[2][FBONDS][FBONDS_P] = {{{"k", "r<sub>0</sub>", " ", " ", " "},
                                           {"E<sub>0</sub>", "r<sub>0</sub>", "k", " ", " "},
                                           {"A", "B", " ", " ", " "},
                                           {"ϵ", "σ", " ", " ", " "},
                                           {"k", "r<sub>0</sub>", "r<sub>c</sub>", " ", " "},
                                           {"k", "r<sub>0</sub>", "k'", "k''", " "},
                                           {"A", "ρ", "C", " ", " "},
                                           {"k", " ", " ", " ", " "},
                                           {"k", "R<sub>0</sub>", "Δ", " ", " "},
                                           {"k", "r<sub>o</sub>", " ", " ", " "},
                                           {" ", " ", " ", " ", " "},
                                           {" ", " ", " ", " ", " "},
                                           {" ", " ", " ", " ", " "},
                                           {" ", " ", " ", " ", " "},
                                           {" ", " ", " ", " ", " "},
                                           {" ", " ", " ", " ", " "}},
                                          {{"K", "r<sub>0</sub>", " ", " ", " "},
                                           {"r<sub>0</sub>", "K<sub>2</sub>", "K<sub>3</sub>", "K<sub>4</sub>", " "},
                                           {"K", "R<sub>0</sub>", "ϵ", "σ", " "},
                                           {"K", "R<sub>0</sub>", "ϵ", "σ", "Δ"},
                                           {"K", "r<sub>0</sub>", " ", " ", " "},
                                           {"K", "r<sub>0</sub>", "r<sub>c</sub>", " ", " "},
                                           {"U<sub>min</sub>", "r<sub>0</sub>", "r<sub>c</sub>", " ", " "},
                                           {"K", "r<sub>0</sub>", " ", " ", " "},
                                           {"D", "α", "r<sub>0</sub>", " ", " "},
                                           {"ϵ", "r<sub>0</sub>", "λ", " ", " "},
                                           {"ϵ", "Δ", "r<sub>0</sub>", " ", " "},
                                           {"ϵ", "Δ", "r<sub>0</sub>", " ", " "},
                                           {"ϵ", "Δ", "r<sub>0</sub>", " ", " "},
                                           {"K", "B<sub>1</sub>", "B<sub>2</sub>", "R<sub>c</sub>", "U<sub>0</sub>"},
                                           {"w<sub>LJ</sub>", "w<sub>Coul</sub>", " ", " ", " "},
                                           {"Interpolation style", "Distances", " ", " ", " "}}};

int feunit_bond[2][FBONDS][FBONDS_P] = {{{1, 0, 0, 0, 0},
                                         {1, 0, 1, 0, 0},
                                         {1, 1, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 1, 1, 0},
                                         {1, 0, 1, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0}},
                                        {{1, 0, 0, 0, 0},
                                         {0, 1, 1, 1, 0},
                                         {1, 0, 1, 0, 0},
                                         {1, 0, 1, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 0},
                                         {1, 0, 0, 0, 1},
                                         {0, 0, 0, 0, 0},
                                         {0, 0, 0, 0, 0}}};

char * fvars_angle[2][FANGLES][FANGLES_P] = {{{"k", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"k", "θ<sub>0</sub>", "k'", "k''", " ", " "},
                                              {"k", "θ<sub>0</sub>", "ρ", " ", " ", " "},
                                              {"k", "θ<sub>0</sub>",  "ρ<sub>1</sub>",  "ρ<sub>2</sub>", " ", " "},
                                              {"k", "θ<sub>0</sub>",  "ρ<sub>1</sub>",  "ρ<sub>2</sub>", " ", " "},
                                              {"k", "θ<sub>0</sub>", "a", "ρ", " ", " "},
                                              {"k", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"A", "δ", "m", " ", " ", " "},
                                              {"A", "θ<sub>0</sub>", "r<sup>o</sup><sub>ij</sub>", "r<sup>o</sup><sub>jk</sub>", " ", " "},
                                              {"A", "r<sup>o</sup><sub>ij</sub>", "r<sup>o</sup><sub>jk</sub>", " ", " ", " "},
                                              {"A", "θ<sub>0</sub>", "r<sup>o</sup><sub>ij</sub>", " ", " ", " "},
                                              {"A", "B", "C", "θ<sub>0</sub>", "r<sup>o</sup><sub>ij</sub>", "r<sup>o</sup><sub>jk</sub>"},
                                              {"k", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"f<sub>k</sub>", "θ<sub>0</sub>", "g<sub>r</sub>", "r<sub>o</sub>", " ", " "},
                                              {" ", " ", " ", " ", " ", " "},
                                              {" ", " ", " ", " ", " ", " "},
                                              {" ", " ", " ", " ", " ", " "},
                                              {" ", " ", " ", " ", " ", " "},
                                              {" ", " ", " ", " ", " ", " "}},
                                             {{"K", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"K", "θ<sub>0</sub>", "K<sub>ub</sub>", "r<sub>ub</sub>", " ", " "},
                                              {"θ<sub>0</sub>", "K<sub>2</sub>", "K<sub>3</sub>", "K<sub>4</sub>", " ", " "},
                                              {"θ<sub>0</sub>", "K<sub>2</sub>", "K<sub>3</sub>", "K<sub>4</sub>", " ", " "},
                                              {"K", " ", " ", " ", " ", " "},
                                              {"K", "n", "θ<sub>0</sub>", " ", " ", " "},
                                              {"K", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"C", "B", "n", " ", " ", " "},
                                              {"U<sub>min</sub>", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"U<sub>min</sub>", "θ<sub>0</sub>", "A", " ", " ", " "},
                                              {"K", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"K<sub>SS</sub>", "K<sub>BS0</sub>", "K<sub>BS1</sub>", "r<sub>12,0</sub>", "r<sub>32,0</sub>", "θ<sub>0</sub>"},
                                              {"K", "λ<sub>0</sub>", " ", " ", " ", " "},
                                              {"K", "C<sub>0</sub>", "C<sub>1</sub>", "C<sub>2</sub>", " ", " "},
                                              {"K", "c", "n", "", " ", " "},
                                              {"K", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"θ<sub>0</sub>", "K<sub>2</sub>", "K<sub>3</sub>", "K<sub>4</sub>", " ", " "},
                                              {"K", "θ<sub>0</sub>", " ", " ", " ", " "},
                                              {"Interpolation style", "Angles", " ", " ", " ", " "}}};

int feunit_angle[2][FANGLES][FANGLES_P] = {{{1, 0, 0, 0, 0, 0},
                                            {1, 0, 1, 1, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 1, 1, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0}},
                                           {{1, 0, 0, 0, 0, 0},
                                            {1, 0, 1, 0, 0, 0},
                                            {0, 1, 1, 1, 0, 0},
                                            {0, 1, 1, 1, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 1, 1, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {0, 1, 1, 1, 0, 0},
                                            {1, 0, 0, 0, 0, 0},
                                            {0, 0, 0, 0, 0, 0}}};

char * fvars_dihedral[2][FDIHEDRAL][FDIHEDRAL_P] = {{{"A", "δ", "m", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"k", "ϕ<sub>0</sub>", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"k", "ϕ<sub>0</sub>", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"A<sub>1</sub>", "A<sub>2</sub>", "A<sub>3</sub>", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"A", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"A", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"A<sub>0</sub>", "A<sub>1</sub>", "A<sub>2</sub>", "A<sub>3</sub>", "ϕ<sub>0</sub>", " ", " ", " ", " ", " ", " "},
                                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}},
                                                    {{"U<sub>min</sub>", "θ<sub>0</sub>", "a", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"K", "n", "d", "weighting factor", " ", " ", " ", " ", " ", " ", " "},
                                                     {"K", "n", "d", "weighting factor", " ", " ", " ", " ", " ", " ", " "},
                                                     {"E<sub>d</sub>", "E<sub>mbt</sub>", "E<sub>ebt</sub>", "E<sub>at</sub>", "E<sub>aat</sub>", "E<sub>bb13</sub>", " ", " ", " ", " ", " "},
                                                     {"m", "K", "n", "d", " ", " ", " ", " ", " ", " ", " "},
                                                     {"K", "d", "n", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"A", "B", "C", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"A<sub>1</sub>", "A<sub>2</sub>", "A<sub>3</sub>", "A<sub>4</sub>", "A<sub>5</sub>", " ", " ", " ", " ", " ", " "},
                                                     {"n", "A", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"K<sub>1</sub>", "K<sub>2</sub>", "K<sub>3</sub>", "K<sub>4</sub>", " ", " ", " ", " ", " ", " ", " "},
                                                     {"K", "ϕ<sub>0</sub>", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"n", "C", "K", "a", "u", "L", "b", "v", "M", "c", "w"},
                                                     {"Interpolation style", "Dihedrals", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                                     {"Interpolation style", "Dihedrals", " ", " ", " ", " ", " ", " ", " ", " ", " "}}};

int feunit_dihedral[2][FDIHEDRAL][FDIHEDRAL_P] = {{{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                                  {{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
                                                   {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
                                                   {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};

/*gchar la_dih_class2[6][9] = {{"K<sub>1</sub>", "ϕ<sub>1</sub>", "K<sub>2</sub>", "ϕ<sub>2</sub>", "K<sub>3</sub>", "ϕ<sub>3</sub>", " ", " ", " "},
                             {"mbt", "A<sub>1</sub>", "A<sub>2</sub>", "A<sub>3</sub>", "r<sub>2</sub>", " ", " ", " ", " "},
                             {"ebt", "B<sub>1</sub>", "B<sub>2</sub>", "B<sub>3</sub>", "C<sub>1</sub>", "C<sub>2</sub>", "C<sub>3</sub>", "r<sub>1</sub>", "r<sub>3</sub>"},
                             {"at", "D<sub>1</sub>", "D<sub>2</sub>", "D<sub>3</sub>", "E<sub>1</sub>", "E<sub>2</sub>", "E<sub>3</sub>", "θ<sub>1</sub>", "θ<sub>2</sub>"},
                             {"aat", "M", "θ<sub>1</sub>", "θ<sub>2</sub>", " ", " ", " ", " ", " "},
                             {"bb13", "N", "r<sub>1</sub>", "r<sub>3</sub>", " ", " ", " ", " ", " "}};

int feu_la_dih_class2[6][9] = {{1, 0, 1, 0, 1, 0, 0, 0, 0},
                               {0, 1, 1, 1, 0, 0, 0, 0, 0},
                               {0, 1, 1, 1, 1, 1, 1, 0, 0},
                               {0, 1, 1, 1, 1, 1, 1, 0, 0},
                               {0, 1, 0, 0, 0, 0, 0, 0, 0},
                               {0, 1, 0, 0, 0, 0, 0, 0, 0}};*/

char * fvars_inversion[2][FINVERS][FINVERS_P] = {{{"k", "ϕ<sub>0</sub>", " "},
                                                  {"k", "ϕ<sub>0</sub>", " "},
                                                  {"A", " ", " "},
                                                  {"k", "m", "ϕ<sub>0</sub>"},
                                                  {"A", "B", " "},
                                                  {" ", " ", " "}},
                                                 {{" ", " ", " "},
                                                  {" ", " ", " "},
                                                  {" ", " ", " "},
                                                  {" ", " ", " "},
                                                  {" ", " ", " "},
                                                  {" ", " ", " "}}};

int feunit_inversion[2][FINVERS][FINVERS_P] = {{{1, 0, 0},
                                                {1, 0, 0},
                                                {1, 0, 0},
                                                {1, 0, 0},
                                                {1, 1, 0},
                                                {0, 0, 0}},
                                               {{1, 0, 0},
                                                {1, 0, 0},
                                                {1, 0, 0},
                                                {1, 0, 0},
                                                {1, 1, 0},
                                                {0, 0, 0}}};

// LAMMPS Impropers
gchar * la_improper_style_keyw[11] = {"class2",
                                      "cossq",
                                      "cvff",
                                      "distance",
                                      "distharm",
                                      "fourier",
                                      "harmonic",
                                      "inversion/harmonic",
                                      "ring",
                                      "sqdistharm",
                                      "umbrella"};

/*gchar * la_improper_style_info[11] = {"COMPASS class2 force field",
                                      "Cosine squared",
                                      "CVFF force field",
                                      "Distance between atom planes",
                                      "Out-of-the plane distance",
                                      "Fourier (multiple cosines)",
                                      "Harmonic",
                                      "Harmonic with Wilson Decius out-of-plane",
                                      "Planar conformation",
                                      "Squared distance harmonic",
                                      "DREIDING force field"};

gchar la_improper_var[11][LIMPROPERS] = {{"E<sub>i</sub>", "E<sub>aa</sub>", " ", " ", " "},
                                         {"K", "χ<sub>0</sub>", " ", " ", " "},
                                         {"K", "d", "n", " ", " "},
                                         {"K<sub>2</sub>", "K<sub>4</sub>", " ", " ", " "}
                                         {"K", "d<sub>0</sub>", " ", " ", " "},
                                         {"K", "C<sub>0</sub>", "C<sub>1</sub>", "C<sub>2</sub>", "all"},
                                         {"K", "χ<sub>0</sub>", " ", " ", " "},
                                         {"K", "ω<sub>0</sub>", " ", " ", " "},
                                         {"K", "θ<sub>0</sub>", " ", " ", " "},
                                         {"K", "d<sub>0</sub><sup>2</sup>", " ", " ", " "},
                                         {"K", "ω<sub>0</sub>", " ", " ", " "}};

gchar la_imp_class2[2][7] = {{"K", "χ<sub>0</sub>", " ", " ", " ", " ", " "},
                             {"aa", "M<sub>1</sub>", "M<sub>2</sub>", "M<sub>3</sub>", "θ<sub>1</sub>", "θ<sub>2</sub>", "θ<sub>3</sub>"}};

int feu_la_imp_class2[2][7] = {{1, 0, 0, 0, 0, 0, 0},
                               {0, 1, 1, 1, 0, 0, 0}};

int feu_la_improper[15][LIMPROPERS] = {{0, 0, 0, 0, 0},
                                       {1, 0, 0, 0, 0},
                                       {1, 0, 0, 0, 0},
                                       {1, 1, 0, 0, 0},
                                       {1, 0, 0, 0, 0},
                                       {1, 0, 0, 0, 0},
                                       {1, 0, 0, 0, 0},
                                       {1, 0, 0, 0, 0},
                                       {1, 0, 0, 0, 0},
                                       {1, 0, 0, 0, 0}};*/

char * fvars_vdw[2][FVDW][FVDW_P] = {{{"A", "B", " ", " ", " ", " ", " "},
                                      {"ϵ", "ρ", " ", " ", " ", " ", " "},
                                      {"ϵ", "ρ", "c", " ", " ", " ", " "},
                                      {"E<sub>0</sub>", "n", "m", "r<sub>0</sub>", " ", " ", " "},
                                      {"A", "ρ", "C", " ", " ", " ", " "},
                                      {"A", "B", "σ", "C", "D", " ", " "},
                                      {"A", "B", " ", " ", " ", " ", " "},
                                      {"E<sub>0</sub>", "n", "m", "r<sub>0</sub>", "r<sub>c</sub>", " ", " "},
                                      {"E<sub>0</sub>", "r<sub>0</sub>", "k", " ", " ", " ", " "},
                                      {"ϵ", "σ", "Δ", " ", " ", " ", " "},
                                      {"A", "r<sub>c</sub>", " ", " ", " ", " ", " "},
                                      {"ϵ", "r<sub>o</sub>", " ", " ", " ", " ", " "},
                                      {"E<sub>0</sub>", "r<sub>0</sub>", "k", "c", " ", " ", " "},
                                      {"a", "b", "σ", " ", " ", " ", " "},
                                      {"Z<sub>1</sub>", "Z<sub>2</sub>", " ", " ", " ", " ", " "},
                                      {"Z<sub>1</sub>", "Z<sub>2</sub>", "r<sub>m</sub>", "ξ", "E<sub>0</sub>", "r<sub>0</sub>", "k"},
                                      {"Z<sub>1</sub>", "Z<sub>2</sub>", "r<sub>m</sub>", "ξ", "A", "ρ", "C"},
                                      {"ϵ", "ρ", "r<sub>i</sub>", " ", " ", " ", " "},
                                      {"A", "ρ", "C", "r<sub>i</sub>", " ", " ", " "},
                                      {"A", "B", "r<sub>i</sub>", " ", " ", " ", " "},
                                      {" ", " ", " ", " ", " ", " ", " "}},
                                     {{"A", "B", " ", " ", " ", " ", " "},
                                      {"A", "B", " ", " ", " ", " ", " "},
                                      {"ϵ", "ρ", " ", " ", " ", " ", " "},
                                      {"ϵ", "ρ", "c", " ", " ", " ", " "},
                                      {"E<sub>0</sub>", "n", "m", "r<sub>0</sub>", " ", " ", " "},
                                      {"A", "ρ", "C", " ", " ", " ", " "},
                                      {"A", "B", "σ", "C", "D", " ", " "},
                                      {"E<sub>0</sub>", "n", "m", "r<sub>0</sub>", "r<sub>c</sub>", " ", " "},
                                      {"E<sub>0</sub>", "r<sub>0</sub>", "k", " ", " ", " ", " "},
                                      {"ϵ", "σ", "Δ", " ", " ", " ", " "},
                                      {"A", "r<sub>c</sub>", " ", " ", " ", " ", " "},
                                      {"ϵ", "r<sub>o</sub>", " ", " ", " ", " ", " "},
                                      {"E<sub>0</sub>", "r<sub>0</sub>", "k", "c", " ", " ", " "},
                                      {"a", "b", "σ", " ", " ", " ", " "},
                                      {"Z<sub>1</sub>", "Z<sub>2</sub>", " ", " ", " ", " ", " "},
                                      {"Z<sub>1</sub>", "Z<sub>2</sub>", "r<sub>m</sub>", "ξ", "E<sub>0</sub>", "r<sub>0</sub>", "k"},
                                      {"Z<sub>1</sub>", "Z<sub>2</sub>", "r<sub>m</sub>", "ξ", "A", "ρ", "C"},
                                      {"ϵ", "ρ", "r<sub>i</sub>", " ", " ", " ", " "},
                                      {"A", "ρ", "C", "r<sub>i</sub>", " ", " ", " "},
                                      {"A", "B", "r<sub>i</sub>", " ", " ", " ", " "},
                                      {" ", " ", " ", " ", " ", " ", " "}}};

int feunit_vdw[2][FVDW][FVDW_P] = {{{1, 1, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 1, 0, 0, 0, 0},
                                    {1, 0, 0, 1, 1, 0, 0},
                                    {1, 1, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 1, 0, 0, 0, 0, 0},
                                    {1, 1, 0, 0, 0, 0, 0},
                                    {1, 1, 0, 0, 1, 0, 0},
                                    {1, 1, 0, 0, 1, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 1, 0, 0, 0, 0, 0},
                                    {0, 0, 0, 0, 0, 0, 0}},
                                   {{1, 1, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 1, 0, 0, 0, 0},
                                    {1, 0, 0, 1, 1, 0, 0},
                                    {1, 1, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 1, 0, 0, 0, 0, 0},
                                    {1, 1, 0, 0, 0, 0, 0},
                                    {1, 1, 0, 0, 1, 0, 0},
                                    {1, 1, 0, 0, 1, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 0, 0, 0, 0, 0, 0},
                                    {1, 1, 0, 0, 0, 0, 0},
                                    {0, 0, 0, 0, 0, 0, 0}}};

char * fvars_met[2][FMETALS][FMETALS_P] = {{{" ", " ", " ", " ", " ", " ", " ", " ", " "},
                                            {" ", " ", " ", " ", " ", " ", " ", " ", " "},
                                            {" ", " ", " ", " ", " ", " ", " ", " ", " "},
                                            {" ", " ", " ", " ", " ", " ", " ", " ", " "},
                                            {"c<sub>0</sub>", "c<sub>1</sub>", "c<sub>2</sub>", "c", "A", "d", "β", " ", " "},
                                            {"c<sub>0</sub>", "c<sub>1</sub>", "c<sub>2</sub>", "c<sub>3</sub>", "c<sub>4</sub>", "c", "A", "d", "B"},
                                            {"ϵ", "a", "n", "m", "c", " ", " ", " ", " "},
                                            {"A", "r<sub>0</sub>", "p", "B", "q<sub>ij</sub>", " ", " ", " ", " "},
                                            {"ϵ", "a", "m", "α", "r<sub>o</sub>", " ", " ", " ", " "}},
                                           {{" ", " ", " ", " ", " ", " ", " ", " ", " "},
                                            {" ", " ", " ", " ", " ", " ", " ", " ", " "},
                                            {" ", " ", " ", " ", " ", " ", " ", " ", " "},
                                            {" ", " ", " ", " ", " ", " ", " ", " ", " "},
                                            {"c<sub>0</sub>", "c<sub>1</sub>", "c<sub>2</sub>", "c", "A", "d", "β", " ", " "},
                                            {"c<sub>0</sub>", "c<sub>1</sub>", "c<sub>2</sub>", "c<sub>3</sub>", "c<sub>4</sub>", "c", "A", "d", "B"},
                                            {"ϵ", "a", "n", "m", "c", " ", " ", " ", " "},
                                            {"A", "r<sub>0</sub>", "p", "B", "q<sub>ij</sub>", " ", " ", " ", " "},
                                            {"ϵ", "a", "m", "α", "r<sub>o</sub>", " ", " ", " ", " "}}};

int feunit_met[2][FMETALS][FMETALS_P] = {{{0, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0, 1},
                                          {1, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 1, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0, 0}},
                                         {{0, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0, 1},
                                          {1, 0, 0, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 1, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0, 0}}};

char * fvars_ters[2][FTERSOFFS][FTERSOFFS_P] = {{{"A", "a", "B", "b", "R", "S", "β", "η", "c", "d", "h", "χ", "ω", "δ", " ", " "},
                                                 {"A", "a", "B", "b", "R", "S", "η", "δ", "c<sub>1</sub>", "c<sub>2</sub>", "c<sub>3</sub>", "c<sub>4</sub>", "c<sub>5</sub>", "h", "α", "β"}},
                                                {{"A", "a", "B", "b", "R", "S", "β", "η", "c", "d", "h", "χ", "ω", "δ", " ", " "},
                                                 {"A", "a", "B", "b", "R", "S", "η", "δ", "c<sub>1</sub>", "c<sub>2</sub>", "c<sub>3</sub>", "c<sub>4</sub>", "c<sub>5</sub>", "h", "α", "β"}}};

int feunit_ters[2][FTERSOFFS][FTERSOFFS_P] = {{{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                               {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                              {{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                               {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};

char * fvars_tbd[2][FTHREEBODY][FTHREEBODY_P] = {{{"k", "θ<sub>0</sub>", " ", " ", " "},
                                                 {"k", "θ<sub>0</sub>", "ρ", " ", " "},
                                                 {"k", "θ<sub>0</sub>", "ρ<sub>1</sub>", "ρ<sub>2</sub>", " "},
                                                 {"k", "θ<sub>0</sub>", "ρ<sub>1</sub>", "ρ<sub>2</sub>", " "},
                                                 {"k", "θ<sub>0</sub>", "a", "ρ", " "},
                                                 {"D<sub>hb</sub>", "R<sub>hb</sub>", " ", " ", " "}},
                                                {{"k", "θ<sub>0</sub>", " ", " ", " "},
                                                 {"k", "θ<sub>0</sub>", "ρ", " ", " "},
                                                 {"k", "θ<sub>0</sub>", "ρ<sub>1</sub>", "ρ<sub>2</sub>", " "},
                                                 {"k", "θ<sub>0</sub>", "ρ<sub>1</sub>", "ρ<sub>2</sub>", " "},
                                                 {"k", "θ<sub>0</sub>", "a", "ρ", " "},
                                                 {"D<sub>hb</sub>", "R<sub>hb</sub>", " ", " ", " "}}};

int feunit_tbd[2][FTHREEBODY][FTHREEBODY_P] = {{{1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0}},
                                               {{1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {1, 0, 0, 0, 0}}};

char * fvars_fbd[2][FFOURBODY][FFOURBODY_P] = {{{"k", "ϕ<sub>0</sub>", " "},
                                                {"k", "ϕ<sub>0</sub>", " "},
                                                {"A", " ", " "}},
                                               {{"k", "ϕ<sub>0</sub>", " "},
                                                {"k", "ϕ<sub>0</sub>", " "},
                                                {"A", " ", " "}}};

int feunit_fbd[2][FFOURBODY][FFOURBODY_P] = {{{1, 0, 0},
                                              {1, 0, 0},
                                              {1, 0, 0}},
                                             {{1, 0, 0},
                                              {1, 0, 0},
                                              {1, 0, 0}}};

char * fvars_fext[2][FEXTERNAL][FEXTERNAL_P] = {{{"E<sub>x</sub>", "E<sub>y</sub>", "E<sub>z</sub>", " ", " ", " "},
                                                 {"A", "n", " ", " ", " ", " "},
                                                 {"A", "z<sub>0</sub>", " ", " ", " ", " "},
                                                 {"G<sub>x</sub>", "G<sub>y</sub>", "G<sub>z</sub>", " ", " ", " "},
                                                 {"H<sub>x</sub>", "H<sub>y</sub>", "H<sub>z</sub>", " ", " ", " "},
                                                 {"A", "R<sub>0</sub>", "n", "R<sub>cut</sub>", " ", " "},
                                                 {"A", "z<sub>0</sub>", "p", " ", " ", " "},
                                                 {"i<sub>ind</sub><sup>glob</sup>", "j<sub>ind</sub><sup>glob</sup>", "P<sup>in</sup><sub>k-atom</sub>", " ", " ", " "},
                                                 {"i<sub>ind</sub><sup>glob</sup>", "j<sub>ind</sub><sup>glob</sup>", "k", "z<sub>mn</sub>", "z<sub>mx</sub>", " "},
                                                 {"i<sub>ind</sub><sup>glob</sup>", "j<sub>ind</sub><sup>glob</sup>", "k", "z<sub>mn</sub>", "z<sub>mx</sub>", " "},
                                                 {"i<sub>ind</sub><sup>glob</sup>", "j<sub>ind</sub><sup>glob</sup>", "k", "z<sub>mn</sub>", "z<sub>mx</sub>", " "},
                                                 {"E<sub>x</sub>", "E<sub>y</sub>", "E<sub>z</sub>", "ω<sup>in</sup><sub>ps<sup>-1</sup></sub>", " ", " "},
                                                 {"i<sub>gid</sub><sup>A</sup>", "j<sub>gid</sub><sup>A</sup>", "k", "i<sub>gid</sub><sup>B</sup>", "j<sub>gid</sub><sup>B</sup>", "R<sub>0</sub>"}},
                                                {{"E<sub>x</sub>", "E<sub>y</sub>", "E<sub>z</sub>", " ", " ", " "},
                                                 {"A", "n", " ", " ", " ", " "},
                                                 {"A", "z<sub>0</sub>", " ", " ", " ", " "},
                                                 {"G<sub>x</sub>", "G<sub>y</sub>", "G<sub>z</sub>", " ", " ", " "},
                                                 {"H<sub>x</sub>", "H<sub>y</sub>", "H<sub>z</sub>", " ", " ", " "},
                                                 {"A", "R<sub>0</sub>", "n", "R<sub>cut</sub>", " ", " "},
                                                 {"A", "z<sub>0</sub>", "p", " ", " ", " "},
                                                 {"i<sub>ind</sub><sup>glob</sup>", "j<sub>ind</sub><sup>glob</sup>", "P<sup>in</sup><sub>k-atom</sub>", " ", " ", " "},
                                                 {"i<sub>ind</sub><sup>glob</sup>", "j<sub>ind</sub><sup>glob</sup>", "k", "z<sub>mn</sub>", "z<sub>mx</sub>", " "},
                                                 {"i<sub>ind</sub><sup>glob</sup>", "j<sub>ind</sub><sup>glob</sup>", "k", "z<sub>mn</sub>", "z<sub>mx</sub>", " "},
                                                 {"i<sub>ind</sub><sup>glob</sup>", "j<sub>ind</sub><sup>glob</sup>", "k", "z<sub>mn</sub>", "z<sub>mx</sub>", " "},
                                                 {"E<sub>x</sub>", "E<sub>y</sub>", "E<sub>z</sub>", "ω<sup>in</sup><sub>ps<sup>-1</sup></sub>", " ", " "},
                                                 {"i<sub>gid</sub><sup>A</sup>", "j<sub>gid</sub><sup>A</sup>", "k", "i<sub>gid</sub><sup>B</sup>", "j<sub>gid</sub><sup>B</sup>", "R<sub>0</sub>"}}};

int feunit_fext[2][FEXTERNAL][FEXTERNAL_P] = {{{1, 1, 1, 0, 0, 0},
                                               {1, 0, 0, 0, 0, 0},
                                               {1, 0, 0, 0, 0, 0},
                                               {1, 1, 1, 0, 0, 0},
                                               {1, 1, 1, 0, 0, 0},
                                               {1, 0, 0, 0, 0, 0},
                                               {1, 0, 0, 0, 0, 0},
                                               {0, 0, 1, 0, 0, 0},
                                               {0, 0, 1, 0, 0, 0},
                                               {0, 0, 1, 0, 0, 0},
                                               {0, 0, 1, 0, 0, 0},
                                               {1, 1, 1, 0, 0, 0},
                                               {0, 0, 0, 0, 1, 0}},
                                              {{1, 1, 1, 0, 0, 0},
                                               {1, 0, 0, 0, 0, 0},
                                               {1, 0, 0, 0, 0, 0},
                                               {1, 1, 1, 0, 0, 0},
                                               {1, 1, 1, 0, 0, 0},
                                               {1, 0, 0, 0, 0, 0},
                                               {1, 0, 0, 0, 0, 0},
                                               {0, 0, 1, 0, 0, 0},
                                               {0, 0, 1, 0, 0, 0},
                                               {0, 0, 1, 0, 0, 0},
                                               {0, 0, 1, 0, 0, 0},
                                               {1, 1, 1, 0, 0, 0},
                                               {0, 0, 0, 0, 1, 0}}};

int field_v[MAXDATA] = {  8,   8,   9,   7,   8,   5,   5,   8,   8,   9,   9,  10,  10,  10,  10,   6,   6,   5,   7,   8,   3};
int field_s[MAXDATA] = {650, 520, 520, 500, 600, 450, 600, 720, 720, 720, 720, 720, 720, 720, 720, 720, 720, 720, 720, 720, 720};
int field_a[MAXDATA] = {  0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0};

gchar * ctitle[MAXDATA][12] ={{"Id", "Name", "Multiplicity", "Chemistry", "Atom(s)", "Species", "Viz. 3D", "Viz. Atom num."},                           // Molecule(s)
                              {"Id", "Name", "Element", "Mass", "Charge", "Frozen", "Atom(s)", "Viz. 3D"},                                              // Atom(s)
                              {"Id", "Core Id", "Shell", "Masse", "Charge", "k2 (1)", "k4 (2)", "Viz. 3D", "Use (3)"},                                  // Core-shell unit(s)
                              {"Id", "At.1 Id", "At.2 Id", "Length [Å]", "av. d [Å] (1)", "Viz. 3D", "Use (2)"},                                        // Constraint(s)
                              {"Id", "Length [Å]", "av. d12 [Å] (1)", "Unit Id", "At. Id", "Weight (2)", "Viz. 3D", "Use (3)"},                         // Mean force potential(s)
                              {"Id", "Atom(s) in unit", "List of atom(s)", "Viz. 3D", "Use(1)"},                                                        // Rigid unit(s)
                              {"Id", "Atom Id", "Viz. 3D", "Use (1)", "Potential"},                                                                     // Tethering potential(s)
                              {"Id", "At.1", "At.2", "Bond(s)", "av. d [Å] (1)", "Viz. 3D", "Use (2)", "Potential (3)"},                                // Bond(s)
                              {"Id", "At.1", "At.2", "Bond restraint(s)", "av. d [Å] (1)", "Viz. 3D", "Use (2)", "Potential (3)"},                      // Bond restraint(s)
                              {"Id", "At.1", "At.2", "At.3", "Angle(s)", "av. θ [°] (1)", "Viz. 3D", "Use (2)", "Potential (3)"},                       // Angle(s)
                              {"Id", "At.1", "At.2", "At.3", "Angle restraint(s)", "av. θ [°] (1)", "Viz. 3D", "Use (2)", "Potential (3)"},             // Angle restraint(s)
                              {"Id", "At.1", "At.2", "At.3", "At.4", "Dihedral(s)", "av. ϕ [°] (1)", "Viz. 3D", "Use (2)", "Potential (3)"},            // Dihedrals
                              {"Id", "At.1", "At.2", "At.3", "At.4", "Torsional restraint(s)", "av. ϕ [°] (1)", "Viz. 3D", "Use (2)", "Potential (3)"}, // Torsions
                              {"Id", "At.1", "At.2", "At.3", "At.4", "Improper(s)", "av. ϕ [°] (1)", "Viz. 3D", "Use (2)", "Potential (3)"},            // Impropers
                              {"Id", "At.1", "At.2", "At.3", "At.4", "Inversion(s)", "av. ϕ [°] (1)", "Viz. 3D", "Use (2)", "Potential (3)"},           // Inversion(s)
                              {"Id", "Spec.1", "Spec.2", "Viz. 3D", "Use (1)", "VdW potential"},                                                        // VdW(s)
                              {"Id", "At.1", "At.2", "Viz. 3D", "Use (1)", "Metal potential"},                                                          // Metal(s)
                              {"Id", "At.", "Viz. 3D", "Use (1)", "Tersoff Pot."},                                                                      // Tersoff potential(s)
                              {"Id", "At.1", "At.2*", "At.3", "Viz. 3D", "Use (1)", "Three-body Pot."},                                                 // Three-body potential(s)
                              {"Id", "At.1*", "At.2", "At.3", "At.4", "Viz. 3D", "Use (1)", "Four-body Pot."},                                          // Four-body potential(s)
                              {"Id", "Use (1)", "Field type"}};                                                                                         // External field(s)

GType col_type[MAXDATA][12] = {{G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},                //  0 - Molecule(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},              //  1 - Atom(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_FLOAT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},      //  2 - Core-shell unit(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_FLOAT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},            //  3 - Constraint(s)
                               {G_TYPE_INT, G_TYPE_FLOAT, G_TYPE_FLOAT, G_TYPE_INT, G_TYPE_INT, G_TYPE_FLOAT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},                //  4 - Mean force potential(s)
                               {G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},                   //  5 - Rigid unit(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},             //  6 - Tethering potential(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},       //  7 - Bond(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},       //  8 - Bond restraint(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT},    //  9 - Angle(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT},    // 10 - Angle restraint(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT}, // 11 - Dihedral(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT}, // 12 - Improper(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT}, // 13 - Torsion(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT}, // 14 - Inversion(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},          // 15 - vdW(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},          // 16 - Metal(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},             // 17 - Tersoff potential(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},       // 18 - Three-body potential(s)
                               {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT},    // 19 - Four-body potential(s)
                               {G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT}};                   // 20 - External field(s)

// 0 - nothing
// 1 - combo box
// 2 - viz3D bool
// 3 - pick bool
// 4 - editable
int is_special[MAXDATA][11] ={{0, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0},  // Molecule(s)
                              {0, 0, 0, 4, 4, 0, 0, 2, 0, 0, 0},  // Atom(s)
                              {0, 0, 0, 4, 4, 4, 4, 2, 3, 0, 0},  // Core-shell unit(s)
                              {0, 0, 0, 4, 0, 2, 3, 0, 0, 0, 0},  // Constraint(s)
                              {0, 4, 0, 0, 0, 4, 2, 3, 0, 0, 0},  // Mean force potential(s)
                              {0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0},  // Rigid unit(s)
                              {0, 0, 2, 3, 1, 0, 0, 0, 0, 0, 0},  // Tethering potential(s)
                              {0, 0, 0, 0, 0, 2, 3, 1, 0, 0, 0},  // Bond(s)
                              {0, 0, 0, 0, 0, 2, 3, 1, 0, 0, 0},  // Bond restrain(s)
                              {0, 0, 0, 0, 0, 0, 2, 3, 1, 0, 0},  // Angle(s)
                              {0, 0, 0, 0, 0, 0, 2, 3, 1, 0, 0},  // Angle restraint(s)
                              {0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0},  // Dihedral(s)
                              {0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0},  // Torsional restraint(s)
                              {0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0},  // Improper(s)
                              {0, 0, 0, 0, 0, 0, 0, 2, 3, 1, 0},  // Inversion(s)
                              {0, 0, 0, 2, 3, 1, 0, 0, 0, 0, 0},  // vdW(s)
                              {0, 0, 0, 2, 3, 1, 0, 0, 0, 0, 0},  // Metal(s)
                              {0, 0, 2, 3, 1, 0, 0, 0, 0, 0, 0},  // Tersoff potential(s)
                              {0, 0, 0, 0, 2, 3, 1, 0, 0, 0, 0},  // Three-body potential(s)
                              {0, 0, 0, 0, 0, 2, 3, 1, 0, 0, 0},  // Four-body potential(s)
                              {0, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0}}; // External field(s)

gboolean afp_init[MAXDATC+MAXDATA] = {TRUE,  //  0 - System
                                      TRUE,  //  1 - Analysis
                                      TRUE,  //  2 - Electrostatics
                                      TRUE,  //  3 - Equilibration
                                      TRUE,  //  4 - Thermodynamics
                                      TRUE,  //  5 - Molecular dynamics
                                      TRUE,  //  6 - Output
                                      TRUE,  //  7 - Computational
                                      TRUE,  //  8 - Molecule(s)
                                      TRUE,  //  9 - Atom(s)
                                      FALSE, // 10 - Shell(s)
                                      FALSE, // 11 - Constraint(s)
                                      FALSE, // 12 - Mean force potential(s)
                                      FALSE, // 13 - Rigid unit(s)
                                      FALSE, // 14 - Tethering potential(s)
                                      TRUE,  // 15 - Bond(s)
                                      FALSE, // 16 - Bond restraint(s)
                                      TRUE,  // 17 - Angle(s)
                                      FALSE, // 18 _ Angle restraint(s)
                                      TRUE,  // 19 - Dihedral(s)
                                      FALSE, // 20 - Torsional restraint(s)
                                      FALSE, // 21 - Improper(s)
                                      FALSE, // 22 - Inversion(s)
                                      TRUE,  // 23 - Vdw(s)
                                      FALSE, // 24 - Metal(s)
                                      FALSE, // 25 - Tersoff potential(s)
                                      FALSE, // 26 - Three-body potential(s)
                                      FALSE, // 27 - Four-body potential(s)
                                      FALSE};// 28 - External field(s)

gchar * mo_title[8]={"Bond", "Bond restraint", "Angle", "Angle restraint", "Dihedral", "Torsional restraint", "Improper", "Inversion"};

// Energy converter, values taken from DL_POLY 'read_file.f90' and 'setup_module.f90'
float internal_to_other[5] = {9648.530821, 418.4, 100.0, 0.831451115, 1.0};

GtkWidget * field_assistant;
GtkTreeViewColumn * field_col[MAXDATA][11];
GtkCellRenderer * field_renderer[MAXDATA][11];
GtkWidget * mol_box[MOLIMIT-1];
GtkWidget * combo_mol[MOLIMIT-1];
GtkWidget * field_tree[MAXDATA];
GtkWidget * field_label[MAXDATA];
GtkTreeStore * field_model[MAXDATA];
GtkTreeIter field_iter;

int atom_init = PARTIAL_COORD_AND_SPEC; //SPEC_ONLY;
classical_field * tmp_field;
glwin * tmp_view = NULL;
project * tmp_proj;
coord_info * tmp_coord;
field_molecule * tmp_fmol;
molecule * tmp_mol;
field_atom* tmp_fat, * tmp_fbt, * tmp_fct, * tmp_fdt;
field_shell * tmp_fshell;
field_constraint * tmp_fcons;
field_pmf * tmp_fpmf;
field_rigid * tmp_frig;
field_tethered * tmp_ftet;
field_prop * tmp_fprop;
field_struct * tmp_fstr;
field_nth_body * tmp_fbody;
field_nth_body * new_body;
field_external * tmp_fext;

//GtkWidget * view[13];
GtkWidget * aview;
GtkWidget * cs_label[2];
GtkWidget * fwin = NULL;
GtkWidget * cwin = NULL;
GtkWidget * mol_num_label = NULL;
GtkWidget * fibox[MAXDATC+MAXDATA+2];
GtkWidget * enbox;
GtkWidget * ff_but[19];

int field_object = FALSE;
gboolean field_color = FALSE;
int num_field_objects = 0;
int row_id;

extern void field_unselect_all ();
extern gboolean tersoff_question ();
extern void check_tersoffs (int id, int key);
extern void check_to_visualize_properties (int id);
extern int get_num_vdw_max ();
extern gchar * get_body_element_name (field_nth_body * body, int aid, int nbd);
extern void print_lammps_atom_file (GtkTextBuffer * buf);

/*!
  \fn int struct_id (int f)

  \brief number of atoms in a structural element

  \param f the type of structural element
*/
int struct_id (int f)
{
  if (f < 9)
  {
    return 2;
  }
  else if (f < 11)
  {
    return 3;
  }
  else
  {
    return 4;
  }
}

/*!
  \fn int body_at (int b)

  \brief find the number of atom(s) in a non bonded interaction

  \param b
*/
int body_at (int b)
{
  if (b < 2)
  {
    return 2;
  }
  else if (b == 2)
  {
    return 1;
  }
  else
  {
    return b;
  }
}

model example;

/*!
  \fn void set_mol_num_label ()

  \brief classical force field prepare the molecule information widget
*/
void set_mol_num_label ()
{
  gchar * str;
  str = g_strdup_printf ("<b>%d</b>", tmp_field -> molecules);
  if (mol_num_label == NULL)
  {
    mol_num_label =  markup_label(str, -1, -1, 0.0, 0.5);
  }
  else
  {
    gtk_label_set_text (GTK_LABEL(mol_num_label), str);
    gtk_label_set_use_markup (GTK_LABEL(mol_num_label), TRUE);
  }
}

/*!
  \fn void setup_cs_labels (int i)

  \brief classical force field prepare the core-shell description strings

  \param i the energy unit id
*/
void setup_cs_labels (int i)
{
  gchar * str;
  str = g_strdup_printf ("\t <b>(1)</b> Force constant of the core-shell spring in [<b>%s &#xC5;<sup>-2</sup></b>]", exact_name(fkeysw[activef][0][i]));
  gtk_label_set_text (GTK_LABEL(cs_label[0]), str);
  gtk_label_set_use_markup (GTK_LABEL(cs_label[0]), TRUE);
  g_free (str);

  str = g_strdup_printf ("\t <b>(2)</b> Quartic (anharmonic) force constant of the core-shell spring in [<b>%s &#xC5;<sup>-4</sup></b>]"
                         " usually <i>k<sub>2</sub></i> >> <i>k<sub>4</sub></i>", exact_name(fkeysw[activef][0][i]));
  gtk_label_set_text (GTK_LABEL(cs_label[1]), str);
  gtk_label_set_use_markup (GTK_LABEL(cs_label[1]), TRUE);
  g_free (str);
}

int ** is_param;
int * has_energy;
int ** is_energy;
gchar *** is_var;

/*!
  \fn gchar * parameters_info (int obj, int key,  gchar ** words, float * data)

  \brief prepare classical force field parameter description string

  \param obj the type of field object
  \param key the formalism key for this type of object
  \param words the string description lists
  \param data the value(s) to print
*/
gchar * parameters_info (int obj, int key,  gchar ** words, float * data)
{
  int i;
  gchar * str = NULL;
  if (fvalues[activef][obj][key] > 0)
  {
    // float v = internal_to_other[tmp_field -> energy_unit] / internal_to_other[ff_unit];
    str = g_strdup_printf (" <b><i>%s</i>=</b> %.3f", words[0], data[0]);
    for (i=1; i< fvalues[activef][obj][key]; i++) str = g_strdup_printf ("%s, <b><i>%s</i>=</b> %.3f", str,  words[i], data[i]);
  }
  else
  {
    str = g_strdup_printf ("<i>Tabulated</i>");
  }
  return str;
}

extern void print_all_field_struct (field_molecule * mol, int str);

/*!
  \fn void fill_field_struct (GtkTreeStore * store, int id, int mo)

  \brief classical force field fill the tree store with structural element parameter(s)

  \param store the tree store to fill
  \param id the type of structural element
  \param mo the id of the target field molecule
*/
void fill_field_struct (GtkTreeStore * store, int id, int mo)
{
  int j, k, l;
  char ** vars;
  gchar * stra, * strb, * strc;
  GtkTreeIter field_level;
  if (tmp_field -> afp[MAXDATC])
  {
    for (j=0; j<mo; j++) tmp_fmol = tmp_fmol -> next;
  }
#ifdef DEBUG
  //g_debug ("id= %d, mo= %d", id, mo);
  //if (id > 5) print_all_field_struct (tmp_fmol, id);
#endif
  tmp_fstr  = tmp_fmol -> first_struct[id];
  l = id + 2;
  while (tmp_fstr)
  {
    gtk_tree_store_append (store, & field_level, NULL);
    tmp_fprop = tmp_fstr -> def;
    stra = g_strdup_printf ("%.3f", tmp_fstr -> av);
    strb = g_strdup_printf ("<b>Default</b>: %s (%s)", fnames[activef][l][tmp_fprop -> key], exact_name(fkeysw[activef][l][tmp_fprop -> key]));
    if (id < 2)
    {
      vars = (char **)fvars_bond[activef][tmp_fprop -> key];
    }
    else if (id < 4)
    {
      vars = (char **)fvars_angle[activef][tmp_fprop -> key];
    }
    else if (id < 7)
    {
      vars = (char **)fvars_dihedral[activef][tmp_fprop -> key];
    }
    else
    {
      vars = (char **)fvars_inversion[activef][tmp_fprop -> key];
    }

    strc = parameters_info (l-1, tmp_fprop -> key, vars, tmp_fprop -> val);
    for (k=0; k<struct_id(id+7); k++) gtk_tree_store_set (store, & field_level, k+1, get_active_atom (mo, tmp_fstr -> aid[k]) -> name, -1);
    gtk_tree_store_set (store, & field_level, 0, tmp_fstr -> id+1,
                                              k+1, tmp_fstr -> num,
                                              k+2, stra,
                                              k+3, tmp_fstr -> def -> show,
                                              k+4, tmp_fstr -> def -> use,
                                              k+5, strb,
                                              k+6, strc,
                                              k+7, tmp_fstr -> id, -1);
    g_free (stra);
    g_free (strb);
    g_free (strc);
    tmp_fat = get_active_atom (mo, tmp_fstr -> aid[0]);
    tmp_fbt = get_active_atom (mo, tmp_fstr -> aid[1]);
    if (id > 1) tmp_fct = get_active_atom (mo, tmp_fstr -> aid[2]);
    if (id > 3) tmp_fdt = get_active_atom (mo, tmp_fstr -> aid[3]);
    if (id < 2)
    {
      print_dlp_bond (id, NULL, tmp_fstr, tmp_fmol -> fragments[0], store, & field_level);
    }
    else if (id < 4)
    {
      print_dlp_angle (id, NULL, tmp_fstr, tmp_fmol -> fragments[0], store, & field_level);
    }
    else if (id < 6)
    {
      print_dlp_dihedral (id, NULL, tmp_fstr, tmp_fmol -> fragments[0], store, & field_level);
    }
    else if (id < 8)
    {
      print_dlp_improper_inversion (id, NULL, tmp_fstr, tmp_fmol -> fragments[0], store, & field_level);
    }
    tmp_fstr = tmp_fstr -> next;
  }
}

/*!
  \fn void fill_field_body (GtkTreeStore * store, int id)

  \brief classical force field fill the tree store with non bonded parameter(s)

  \param store the tree store to fill
  \param id the type of non bonded interaction
*/
void fill_field_body (GtkTreeStore * store, int id)
{
  int i, j, k, l;
  gchar * stra, * strb;
  GtkTreeIter field_level;
  if (! id)
  {
    i = get_num_vdw_max ();
  }
  tmp_fbody = tmp_field -> first_body[id];
  k = body_at(id);
  for (i=0; i < tmp_field -> nbody[id]; i++)
  {
    gtk_tree_store_append (store, & field_level, NULL);
    for (j=0; j<k ; j++)
    {
      if (tmp_fbody -> na[j] > 0)
      {
        if (! id)
        {
          gtk_tree_store_set (store, & field_level, j+1, get_body_element_name (tmp_fbody, j, 0), -1);
        }
        else
        {
          l = tmp_fbody -> ma[j][0];
          gtk_tree_store_set (store, & field_level, j+1, get_active_atom (l, tmp_fbody -> a[j][0]) -> name, -1);
        }
      }
    }
    stra = g_strdup_printf ("%s (%s)", fnames[activef][10+id][tmp_fbody -> key], exact_name(fkeysw[activef][10+id][tmp_fbody -> key]));
    if (id == 0) strb = parameters_info (9+id, tmp_fbody -> key, fvars_vdw[activef][tmp_fbody -> key], tmp_fbody -> val);
    if (id == 1) strb = parameters_info (9+id, tmp_fbody -> key, fvars_met[activef][tmp_fbody -> key], tmp_fbody -> val);
    if (id == 2) strb = parameters_info (9+id, tmp_fbody -> key, fvars_ters[activef][tmp_fbody -> key], tmp_fbody -> val);
    if (id == 3) strb = parameters_info (9+id, tmp_fbody -> key, fvars_tbd[activef][tmp_fbody -> key], tmp_fbody -> val);
    if (id == 4) strb = parameters_info (9+id, tmp_fbody -> key, fvars_fbd[activef][tmp_fbody -> key], tmp_fbody -> val);
    gtk_tree_store_set (store, & field_level, 0, tmp_fbody -> id+1,
                                              k+1, tmp_fbody -> show,
                                              k+2, tmp_fbody -> use,
                                              k+3, stra,
                                              k+4, strb, -1);
    g_free (stra);
    g_free (strb);
    if (tmp_fbody -> next != NULL) tmp_fbody = tmp_fbody -> next;
  }
}

/*!
  \fn void fill_field_model (GtkTreeStore * store, int f, int m)

  \brief classical force field fill the tree store

  \param store the tree store to fill
  \param f the type of field object
  \param m the target field molecule, if any
*/
void fill_field_model (GtkTreeStore * store, int f, int m)
{
  GtkTreeIter field_level, unit_level, site_level;
  int i, j, k, l;
  gchar * stra, * strb, * strc, * strd;

  tmp_fmol = tmp_field -> first_molecule;

  //if (get_active_field_elements(f) > 10000 &&  ! tmp_field -> show_all[f])
  {

  }
  //else
  {
    switch (f)
    {
      case 0:
        stra = NULL;
        for (i=0; i< tmp_field -> molecules; i++)
        {
          tmp_mol = tmp_fmol -> mol;
          for (j=0; j < tmp_proj -> nspec; j++)
          {
            if (tmp_mol -> species[j] > 0)
            {
              if (tmp_mol -> species[j] == 1)
              {
                if (stra == NULL)
                {
                  stra = g_strdup_printf ("%s ", exact_name(tmp_proj -> chemistry -> label[j]));
                }
                else
                {
                  stra = g_strdup_printf ("%s%s ", stra, exact_name(tmp_proj -> chemistry -> label[j]));
                }
              }
              else
              {
                if (stra == NULL)
                {
                  stra = g_strdup_printf ("%s<sub>%d</sub> ", exact_name(tmp_proj -> chemistry -> label[j]), tmp_mol -> species[j]);
                }
                else
                {
                  stra = g_strdup_printf ("%s%s<sub>%d</sub> ", stra, exact_name(tmp_proj -> chemistry -> label[j]), tmp_mol -> species[j]);
                }
              }
          }
        }
        gtk_tree_store_append (store, & field_level, NULL);
        gtk_tree_store_set (store, & field_level, 0, i+1,
                                                  1, tmp_fmol -> name,
                                                  2, tmp_fmol -> multi,
                                                  3, stra,
                                                  4, tmp_mol -> natoms,
                                                  5, tmp_mol -> nspec,
                                                  6, tmp_fmol -> show,
                                                  7, tmp_fmol -> show_id, -1);
        g_free (stra);
        stra = NULL;
        if (tmp_fmol -> next != NULL) tmp_fmol = tmp_fmol -> next;
      }
      break;
    case 1:
      if (tmp_field -> afp[MAXDATC])
      {
        for (j=0; j<m; j++) tmp_fmol = tmp_fmol -> next;
      }
      tmp_fat = tmp_fmol -> first_atom;
      for (i=0; i < tmp_fmol -> atoms; i++)
      {
        gtk_tree_store_append (store, & field_level, NULL);
        stra = g_strdup_printf ("%.3f", tmp_fat -> mass);
        strb = g_strdup_printf ("%.2f", tmp_fat -> charge);
        gtk_tree_store_set (store, & field_level, 0, i+1,
                                                  1, tmp_fat -> name,
                                                  2, tmp_proj -> chemistry -> label[tmp_fat -> sp],
                                                  3, stra,
                                                  4, strb,
                                                  5, tmp_fat -> frozen,
                                                  6, tmp_fat -> num / tmp_fmol -> multi,
                                                  7, tmp_fat -> show, -1);
        g_free (stra);
        g_free (strb);
        if (tmp_fat -> next != NULL) tmp_fat = tmp_fat -> next;
      }
      break;
    case 2:
      if (tmp_field -> afp[MAXDATC])
      {
        for (j=0; j<m; j++) tmp_fmol = tmp_fmol -> next;
      }
      tmp_fshell = tmp_fmol -> first_shell;
      for (i=0; i < tmp_fmol -> shells; i++)
      {
        gtk_tree_store_append (store, & field_level, NULL);
        if (tmp_fshell -> ia[0])
        {
          l = tmp_fmol -> atoms_id[tmp_fshell -> ia[0]-1][0].a;
          tmp_fat = get_active_atom (tmp_fmol -> id, l);
          stra = g_strdup_printf ("%d (%s)", tmp_fshell -> ia[0], exact_name(tmp_proj -> chemistry -> label[tmp_fat -> sp]));
        }
        else
        {
          stra = g_strdup_printf ("<i>None</i>");
        }
        if (tmp_fshell -> ia[1])
        {
          l = tmp_fmol -> atoms_id[tmp_fshell -> ia[1]-1][0].a;
          tmp_fat = get_active_atom (tmp_fmol -> id, l);
          strb = g_strdup_printf ("%d (%s)", tmp_fshell -> ia[1], exact_name(tmp_proj -> chemistry -> label[tmp_fat -> sp]));
        }
        else
        {
          strb = g_strdup_printf ("<i>None</i>");
        }
        strc = g_strdup_printf ("%.3f", tmp_fshell -> m);
        strd = g_strdup_printf ("%.2f", tmp_fshell -> z);
        gtk_tree_store_set (store, & field_level, 0, i+1,
                                                  1, stra,
                                                  2, strb,
                                                  3, strc,
                                                  4, strd,
                                                  5, tmp_fshell -> k2,
                                                  6, tmp_fshell -> k4,
                                                  7, tmp_fshell -> show,
                                                  8, tmp_fshell -> use, -1);
        g_free (stra);
        g_free (strb);
        g_free (strc);
        g_free (strd);
        if (tmp_fshell -> next != NULL) tmp_fshell = tmp_fshell -> next;
      }
      break;
    case 3:
      if (tmp_field -> afp[MAXDATC])
      {
        for (j=0; j<m; j++) tmp_fmol = tmp_fmol -> next;
      }
      tmp_fcons = tmp_fmol -> first_constraint;
      for (i=0; i < tmp_fmol -> constraints; i++)
      {
        gtk_tree_store_append (store, & field_level, NULL);
        if (tmp_fcons -> ia[0])
        {
          k = tmp_fmol -> atoms_id[tmp_fcons -> ia[0]-1][0].a;
          l = get_active_atom (m, k) -> sp;
          stra = g_strdup_printf ("%d (%s)", tmp_fcons -> ia[0], exact_name(tmp_proj -> chemistry -> label[l]));
        }
        else
        {
          stra = g_strdup_printf ("<i>None</i>");
        }
        if (tmp_fcons -> ia[1])
        {
          k = tmp_fmol -> atoms_id[tmp_fcons -> ia[1]-1][0].a;
          l = get_active_atom (m, k) -> sp;
          strb = g_strdup_printf ("%d (%s)", tmp_fcons -> ia[1], exact_name(tmp_proj -> chemistry -> label[l]));
        }
        else
        {
          strb = g_strdup_printf ("<i>None</i>");
        }
        gtk_tree_store_set (store, & field_level, 0, i+1,
                                                  1, stra,
                                                  2, strb,
                                                  3, tmp_fcons -> length,
                                                  4, tmp_fcons -> av,
                                                  5, tmp_fcons -> show,
                                                  6, tmp_fcons -> use, -1);
        g_free (stra);
        g_free (strb);
        if (tmp_fcons -> next != NULL) tmp_fcons = tmp_fcons -> next;
      }
      break;
    case 4:
      if (tmp_field -> afp[MAXDATC])
      {
        for (j=0; j<m; j++) tmp_fmol = tmp_fmol -> next;
      }
      tmp_fpmf = tmp_fmol -> first_pmf;
      for (i=0; i < tmp_fmol -> pmfs; i++)
      {
        gtk_tree_store_append (store, & field_level, NULL);
        gtk_tree_store_set (store, & field_level, 0, i+1,
                                                  1, tmp_fpmf -> length,
                                                  2, tmp_fpmf -> av,
                                                  3, 0, 4, 0, 5, 0.0,
                                                  6, tmp_fpmf -> show,
                                                  7, tmp_fpmf -> use, -1);
        for (j=0; j<2; j++)
        {
          gtk_tree_store_append (store, & unit_level, & field_level);
          gtk_tree_store_set (store, & unit_level, 0, -(i+1), 1, 0.0, 2, 0.0, 3, j+1, 4, 0, 5, 0.0, 6, tmp_fpmf -> show, 7, FALSE, -1);
          if (tmp_fpmf -> num[j] > 0)
          {
              for (k=0; k<tmp_fpmf -> num[j]; k++)
              {
                gtk_tree_store_append (store, & site_level, & unit_level);
                gtk_tree_store_set (store, & site_level, 0, -(i+1), 1, 0.0, 2, 0.0, 3, -(j+1),
                                    4, tmp_fpmf -> list[j][k]+1,
                                    5, tmp_fpmf -> weight[j][k], 6, tmp_fpmf -> show, 7, FALSE,  -1);
              }
          }
        }
        if (tmp_fpmf -> next != NULL) tmp_fpmf = tmp_fpmf -> next;
      }
      break;
    case 5:
      if (tmp_field -> afp[MAXDATC])
      {
        for (j=0; j<m; j++) tmp_fmol = tmp_fmol -> next;
      }
      tmp_frig = tmp_fmol -> first_rigid;
      for (i=0; i < tmp_fmol -> rigids; i++)
      {
        gtk_tree_store_append (store, & field_level, NULL);
        gtk_tree_store_set (store, & field_level, 0, i+1,
                                                  1, tmp_frig -> num,
                                                  2, NULL,
                                                  3, tmp_frig -> show,
                                                  4, tmp_frig -> use, -1);
        if (tmp_frig -> num > 0)
        {
          for (j=0; j<tmp_frig -> num; j++)
          {
             l = tmp_fmol -> atoms_id[tmp_frig -> list[j]][0].a;
             k = get_active_atom (m, l) -> sp;
             stra = g_strdup_printf ("%d (%s)",  tmp_frig -> list[j]+1, exact_name(tmp_proj -> chemistry -> label[k]));
             gtk_tree_store_append (store, & unit_level, & field_level);
             gtk_tree_store_set (store, & unit_level, 0, -(i+1), 1, 0, 2, stra, 3, FALSE, 4, FALSE, -1);
             g_free (stra);
          }
        }
        if (tmp_frig -> next != NULL) tmp_frig = tmp_frig -> next;
      }
      break;
    case 6:
      if (tmp_field -> afp[MAXDATC])
      {
        for (j=0; j<m; j++) tmp_fmol = tmp_fmol -> next;
      }
      tmp_ftet = tmp_fmol -> first_tethered;
      for (i=0; i < tmp_fmol -> tethered; i++)
      {
        j = tmp_fmol -> atoms_id[tmp_ftet -> num-1][0].a;
        k = get_active_atom (m, j) -> sp;
        stra = g_strdup_printf ("%d (%s)",  tmp_ftet -> num, exact_name(tmp_proj -> chemistry -> label[k]));
        strb = g_strdup_printf ("%s (%s)", fnames[activef][1][tmp_ftet -> key], exact_name(fkeysw[activef][1][tmp_ftet -> key]));
        strc = parameters_info (0, tmp_ftet -> key, fvars_teth[activef][tmp_ftet -> key], tmp_ftet -> val);
        gtk_tree_store_append (store, & field_level, NULL);
        gtk_tree_store_set (store, & field_level, 0, i+1,
                                                  1, stra,
                                                  2, tmp_ftet -> show,
                                                  3, tmp_ftet -> use,
                                                  4, strb,
                                                  5, strc, -1);
        g_free (stra);
        g_free (strb);
        g_free (strc);
        if (tmp_ftet -> next != NULL) tmp_ftet = tmp_ftet -> next;
      }
      break;
    case SEXTERN:
      // External field
      tmp_fext = tmp_field -> first_external;
      for (i=0; i < tmp_field -> extern_fields; i++)
      {
        gtk_tree_store_append (store, & field_level, NULL);
        stra = g_strdup_printf ("%s (%s)", fnames[activef][15][tmp_fext -> key], exact_name(fkeysw[activef][15][tmp_fext -> key]));
        gtk_tree_store_set (store, & field_level, 0, tmp_fext -> id+1,
                                                  1, tmp_fext -> use,
                                                  2, stra,
                                                  3, parameters_info (14, tmp_fext -> key, fvars_fext[activef][tmp_fext -> key], tmp_fext -> val), -1);
        g_free (stra);
        if (tmp_fext -> next != NULL) tmp_fext = tmp_fext -> next;
      }
      break;
    default:
      if (f < MOLIMIT)
      {
        fill_field_struct (store, f-7, m);
      }
      else
      {
        fill_field_body (store, f - MOLIMIT);
      }
      break;
  }
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggle_field_params (GtkCheckButton * but, gpointer data)

  \brief change classical force field parameter toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_field_params (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggle_field_params (GtkToggleButton * but, gpointer data)

  \brief change classical force field parameter toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_field_params (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j;
  i = GPOINTER_TO_INT (data);
  tmp_field -> afp[i] = button_get_status ((GtkWidget *)but);
  if (i == MAXDATC)
  {
    for (j=0; j<MOLIMIT-1; j++)
    {
      widget_set_sensitive (mol_box[j], tmp_field -> afp[MAXDATC]);
      combo_set_active (combo_mol[j], 0);
    }
  }
  else if (i > MAXDATC)
  {
    gtk_widget_set_visible (fibox[i+1], tmp_field -> afp[i]);
  }
}

/*!
  \fn gboolean set_nbd_but_sensitive (int nbid)

  \brief adjust non bonded interaction button sensitivity

  \param nbid the type of non bonded interaction
*/
gboolean set_nbd_but_sensitive (int nbid)
{
  if (nbid == 0 || nbid == 2 || nbid == 5)
  {
    return TRUE;
  }
  else
  {
    int i, j;
    i = (nbid == 1) ? 2 : nbid;
    j = 0;
    field_molecule * ftmp = tmp_field -> first_molecule;
    while (ftmp)
    {
      j += ftmp -> atoms;
      ftmp = ftmp -> next;
    }
    gboolean res = (j >= i) ? TRUE : FALSE;
    return res;
  }
}

/*!
  \fn gchar * set_field_label (int f, int m)

  \brief prepare classical force field description string

  \param f the type of field object(s)
  \param m the field molecule id, if any
*/
gchar * set_field_label (int f, int m)
{
  gchar * lab;
  int k;
  if (f < MOLIMIT)
  {
    if (tmp_field -> afp[MAXDATC])
    {
      tmp_fmol = get_active_field_molecule (m);
      lab = g_strdup_printf ("the <b>%d</b> different fragment(s) in molecule <b>%s</b>\n",
                             tmp_fmol -> multi, tmp_fmol -> name);
    }
    else
    {
      lab = g_strdup_printf ("the system\n");
    }
  }
  switch (f)
  {
    case 1:
      lab = g_strdup_printf ("%seach contains the following <b>%d</b> type(s) of atom", lab, tmp_fmol -> atoms);
      break;
    case 2:
      if (tmp_fmol ->  shells == 0)
      {
        lab = g_strdup_printf ("%sdo not contains any core-shell unit", lab);
      }
      else
      {
        lab = g_strdup_printf ("%seach contains the following <b>%d</b> core-shell unit", lab, tmp_fmol -> shells);
      }
      break;
    case 3:
      if (tmp_fmol ->  constraints == 0)
      {
        lab = g_strdup_printf ("%sdo not contains any constraint", lab);
      }
      else
      {
        lab = g_strdup_printf ("%seach contains the following <b>%d</b> constraint(s)", lab, tmp_fmol -> constraints);
      }
      break;
    case 4:
      if (tmp_fmol ->  pmfs == 0)
      {
        lab = g_strdup_printf ("%sdo not contains any mean force potential", lab);
      }
      else
      {
        lab = g_strdup_printf ("%seach contains the following <b>%d</b> type(s) of mean force potential(s)", lab, tmp_fmol -> pmfs);
      }
      break;
    case 5:
      if (tmp_fmol ->  rigids == 0)
      {
        lab = g_strdup_printf ("%sdo not contains any rigid unit", lab);
      }
      else
      {
        lab = g_strdup_printf ("%seach contains the following <b>%d</b> rigid unit(s)", lab, tmp_fmol -> rigids);
      }
      break;
    case 6:
      if (tmp_fmol ->  tethered == 0)
      {
        lab = g_strdup_printf ("%sdo not contains any tethering potential(s)", lab);
      }
      else
      {
        lab = g_strdup_printf ("%seach contains the following <b>%d</b> tethering potential(s)", lab, tmp_fmol -> tethered);
      }
      break;
    case SEXTERN:
      if (tmp_field -> extern_fields == 0)
      {
        lab = g_strdup_printf ("the force field do not contains any external field(s)");
      }
      else
      {
        lab = g_strdup_printf ("the force field contains the following <b>%d</b> external field(s)", tmp_field -> extern_fields);
      }
      break;
    default:
      if (f < MOLIMIT)
      {
        if (f == 14)
        {
          if (tmp_fmol -> nstruct[f-7] == 0)
          {
            lab = g_strdup_printf ("%sdo not contains any %s <sup>*</sup>", lab, mo_title[f-7]);
          }
          else
          {
            lab = g_strdup_printf ("%s contains the following <b>%d</b> type(s) of %s <sup>*</sup>", lab, tmp_fmol -> nstruct[f-7], elemts[f]);
          }
        }
        else
        {
          if (tmp_fmol -> nstruct[f-7] == 0)
          {
            lab = g_strdup_printf ("%sdo not contains any %s", lab, mo_title[f-7]);
          }
          else
          {
            lab = g_strdup_printf ("%s contains the following <b>%d</b> type(s) of %s", lab, tmp_fmol -> nstruct[f-7], elemts[f]);
          }
        }
        break;
      }
      else
      {
        k = f - MOLIMIT;
        if (tmp_field -> nbody[k] == 0)
        {
          lab = g_strdup_printf ("the force field do not contains any %s(s)", elemts[f]);
        }
        else
        {
          lab = g_strdup_printf ("the force field contains the following <b>%d</b> %s(s)", tmp_field -> nbody[k], elemts[f]);
        }
        break;
      }
  }
  return lab;
}

/*!
  \fn G_MODULE_EXPORT void changed_mol_box (GtkComboBox * box, gpointer data)

  \brief classical force field assistant change the target molecule for the page

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_mol_box (GtkComboBox * box, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  j = combo_get_active ((GtkWidget *)box);
  gtk_label_set_text (GTK_LABEL(field_label[i]), set_field_label(i, j));
  gtk_label_set_use_markup (GTK_LABEL(field_label[i]), TRUE);
  gtk_tree_store_clear (field_model[i]);
  fill_field_model (field_model[i], i, j);
}

/*!
  \fn void update_field_trees ()

  \brief classical force field assistant update all tree models
*/
void update_field_trees ()
{
  int i;
  for (i=0; i<MAXDATA; i++)
  {
    if (i > 0 && i < MOLIMIT)
    {
      changed_mol_box (GTK_COMBO_BOX (combo_mol[i-1]), GINT_TO_POINTER(i));
    }
    else
    {
      if (i > 0)
      {
        gtk_label_set_text (GTK_LABEL(field_label[i]), set_field_label(i, 0));
        gtk_label_set_use_markup(GTK_LABEL(field_label[i]), TRUE);
      }
      gtk_tree_store_clear (field_model[i]);
      fill_field_model (field_model[i], i, -1);
    }
  }
}

/*!
  \fn void get_is_energy (int i, int l)

  \brief get the energy unit linked parameter list

  \param i the type of force field object
  \param l 1 = prepare the associated string description(s), 0 = no description(s)
*/
void get_is_energy (int i, int l)
{
  int j, k;
  j = (i > 1 && i < 8) ? (i-1)/2 + 1 - i/7 : i;
  is_energy = g_malloc (fetypes[activef][i+1]*sizeof*is_energy);
  if (l)
  {
    is_var = g_malloc (fetypes[activef][i+1]*sizeof*is_var);
  }
  switch (j)
  {
    case 0:
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FTETH_P, feunit_teth[activef][k]);
        if (l) is_var[k] = duplicate_strings (FTETH_P, fvars_teth[activef][k]);
      }
      break;
    case 1:
      // Bonds
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FBONDS_P, feunit_bond[activef][k]);
        if (l) is_var[k] = duplicate_strings (FBONDS_P, fvars_bond[activef][k]);
      }
      break;
    case 2:
      // Angles
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FANGLES_P, feunit_angle[activef][k]);
        if (l) is_var[k] = duplicate_strings (FANGLES_P, fvars_angle[activef][k]);
      }
      break;
    case 3:
      // Dihedrals / Impropers
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FDIHEDRAL_P, feunit_dihedral[activef][k]);
        if (l) is_var[k] = duplicate_strings (FDIHEDRAL_P, fvars_dihedral[activef][k]);
      }
      break;
    case 8:
      // Inversions
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FINVERS_P, feunit_inversion[activef][k]);
        if (l) is_var[k] = duplicate_strings (FINVERS_P, fvars_inversion[activef][k]);
      }
      break;
    case 9:
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FVDW_P, feunit_vdw[activef][k]);
        if (l) is_var[k] = duplicate_strings (FVDW_P, fvars_vdw[activef][k]);
      }
      break;
    case 10:
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FMETALS_P, feunit_met[activef][k]);
        if (l) is_var[k] = duplicate_strings (FMETALS_P, fvars_met[activef][k]);
      }
      break;
    case 11:
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FTERSOFFS_P, feunit_ters[activef][k]);
        if (l) is_var[k] = duplicate_strings (FTERSOFFS_P, fvars_ters[activef][k]);
      }
      break;
    case 12:
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FTHREEBODY_P, feunit_tbd[activef][k]);
        if (l) is_var[k] = duplicate_strings (FTHREEBODY_P, fvars_tbd[activef][k]);
      }
      break;
    case 13:
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FFOURBODY_P, feunit_fbd[activef][k]);
        if (l) is_var[k] = duplicate_strings (FFOURBODY_P, fvars_fbd[activef][k]);
      }
      break;
    case 14:
      for (k=0; k<fetypes[activef][i+1]; k++)
      {
        is_energy[k] = duplicate_int (FEXTERNAL_P, feunit_fext[activef][k]);
        if (l) is_var[k] = duplicate_strings (FEXTERNAL_P, fvars_fext[activef][k]);
      }
      break;
    }
}

/*!
  \fn gboolean field_file_has_energy_parameters (gboolean scale, int sca, int scb)

  \brief scale all field parameter(s) related to the energy unit

  \param scale scale or not the parameter
  \param sca initial scale
  \param scb target scale
*/
gboolean field_file_has_energy_parameters (gboolean scale, int sca, int scb)
{
  int i, j, k, l;
  is_param = allocdint(15, 21);
  has_energy = allocint (15);
  i = 0;
  tmp_fmol = tmp_field -> first_molecule;
  while (tmp_fmol)
  {
    tmp_fshell = get_active_shell(tmp_fmol -> id, 0);
    while (tmp_fshell)
    {
      if (! scale)
      {
        i =1;
        break;
      }
      else
      {
        tmp_fshell -> k2 = (tmp_fshell -> k2 * internal_to_other[sca]) / internal_to_other[scb];
        tmp_fshell -> k4 = (tmp_fshell -> k4 * internal_to_other[sca]) / internal_to_other[scb];
        tmp_fshell = tmp_fshell -> next;
      }
    }
    tmp_fmol = tmp_fmol -> next;
  }
  for (j=0; j<15; j++)
  {
    get_is_energy (j, 0);
    l = 0;
    if (j == 0)
    {
      tmp_fmol = tmp_field -> first_molecule;
      while (tmp_fmol)
      {
        tmp_ftet = get_active_tethered (tmp_fmol -> id, 0);
        while (tmp_ftet)
        {
          for (k=0; k<fvalues[activef][j][tmp_ftet -> key]; k++)
          {
            if (is_energy[tmp_ftet -> key][k])
            {
              is_param[j][tmp_ftet -> key] ++;
              if (scale) tmp_ftet -> val[k]= (tmp_ftet -> val[k] * internal_to_other[sca]) / internal_to_other[scb];
              l ++;
            }
          }
          tmp_ftet = tmp_ftet -> next;
        }
        tmp_fmol = tmp_fmol -> next;
      }
    }
    else if (j>0 && j<9)
    {
      tmp_fmol = tmp_field -> first_molecule;
      while (tmp_fmol)
      {
         tmp_fstr = tmp_fmol -> first_struct[j-1];
         //get_active_struct (j-1, tmp_fmol -> id, 0);
         while (tmp_fstr)
         {
           for (k=0; k<fvalues[activef][j][tmp_fstr -> def -> key]; k++)
           {
             if (is_energy[tmp_fstr -> def -> key][k])
             {
               is_param[j][tmp_fstr -> def -> key] ++;
               if (scale) tmp_fstr -> def -> val[k] = (tmp_fstr -> def -> val[k] * internal_to_other[sca]) / internal_to_other[scb];
               l ++;
             }
           }
           tmp_fprop = tmp_fstr -> other;
           while (tmp_fprop)
           {
             for (k=0; k<fvalues[activef][j][tmp_fprop -> key]; k++)
             {
               if (is_energy[tmp_fprop -> key][k])
               {
                 is_param[j][tmp_fprop -> key] ++;
                 if (scale) tmp_fprop -> val[k] = (tmp_fprop -> val[k] * internal_to_other[sca]) / internal_to_other[scb];
                 l ++;
               }
             }
             tmp_fprop = tmp_fprop -> next;
           }
           tmp_fstr = tmp_fstr -> next;
         }
         tmp_fmol = tmp_fmol -> next;
      }
    }
    else
    {
      tmp_fbody = get_active_body (0, j-9);
      while (tmp_fbody)
      {
        for (k=0; k<fvalues[activef][j][tmp_fbody -> key]; k++)
        {
          if (is_energy[tmp_fbody -> key][k])
          {
            is_param[j][tmp_fbody -> key] ++;
            if (scale) tmp_fbody -> val[k] = (tmp_fbody -> val[k] * internal_to_other[sca]) / internal_to_other[scb];
            l ++;
          }
        }
        tmp_fbody = tmp_fbody -> next;
      }
    }
    has_energy[j] = l;
    i += l;
    for (k=0; k<fetypes[activef][j+1]; k++) g_free (is_energy[k]);
    g_free (is_energy);
  }
  if (i)
  {
    return TRUE;
  }
  else
  {
    g_free (is_param);
    return FALSE;
  }
}

/*!
  \fn G_MODULE_EXPORT void run_changed_energy_unit (GtkDialog * dialog, gint response_id, gpointer data)

  \brief change the classical force field energy unit - running the dialog

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_changed_energy_unit (GtkDialog * dialog, gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_YES)
  {
    int i = GPOINTER_TO_INT(data);
    setup_cs_labels (i);
    // Correct all parameters
    field_file_has_energy_parameters (TRUE, tmp_field -> energy_unit, i);
    tmp_field -> energy_unit = i;
  }
  g_free (is_param);
  destroy_this_dialog (dialog);
}

/*!
  \fn G_MODULE_EXPORT void changed_energy_unit (GtkComboBox * box, gpointer data)

  \brief change the classical force field energy unit - creating the dialog

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_energy_unit (GtkComboBox * box, gpointer data)
{
  int i, j, k, l;
  i = combo_get_active ((GtkWidget *)box);
  if (i != tmp_field -> energy_unit)
  {
    if (field_file_has_energy_parameters(FALSE, 0, 0))
    {
      GtkWidget * dialog =  gtk_dialog_new_with_buttons ("Change energy unit ?", GTK_WINDOW(field_assistant),
                                                         GTK_DIALOG_MODAL, "Yes", GTK_RESPONSE_YES, "No", GTK_RESPONSE_NO, NULL);
      gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
      GtkWidget * vbox = dialog_get_content_area (dialog);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("Change the energy unit ?\nThe value of all field parameters that are energy related (listed below) will be scaled accordingly.", -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("List of energy related parameters in your FIELD file:\n", -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
      GtkWidget * hbox;
      GtkWidget * vbax, * vbbx;
      GtkWidget * hax, * hbx;
      gchar * str;
      // Core-Shell
      tmp_fmol = tmp_field -> first_molecule;
      while (tmp_fmol)
      {
        tmp_fshell = get_active_shell(tmp_fmol -> id, 0);
        if (tmp_fshell)
        {
          hbox = create_hbox (0);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
          vbax = create_vbox (BSEP);
          vbbx = create_vbox (BSEP);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbax, FALSE, FALSE, 0);
          hax = create_hbox (0);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbax, hax, FALSE, FALSE, 0);
          str = g_strdup_printf ("\t<b>%s(s):</b>", felemts[3]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hax, markup_label(str, 250, -1, 0.0, 0.5), FALSE, FALSE, 0);
          g_free (str);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbbx, FALSE, FALSE, 0);
          hbx = create_hbox (0);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbbx, hbx, FALSE, FALSE, 0);
          str = g_strdup_printf ("k<sub>2</sub> and k<sub>4</sub>");
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, markup_label(str, 250, -1, 0.0, 0.5), FALSE, FALSE, 0);
          g_free (str);
          break;
        }
        tmp_fmol = tmp_fmol -> next;
      }
      for (j=0; j<15; j++)
      {
        if (has_energy[j])
        {
          hbox = create_hbox (0);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
          vbax = create_vbox (BSEP);
          vbbx = create_vbox (BSEP);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbax, FALSE, FALSE, 0);
          hax = create_hbox (0);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbax, hax, FALSE, FALSE, 0);
          str = g_strdup_printf ("\t<b>%s:</b>", felemts[j+7]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hax, markup_label(str, 250, -1, 0.0, 0.5), FALSE, FALSE, 0);
          g_free (str);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbbx, FALSE, FALSE, 0);
          get_is_energy (j, 1);
          for (k=0; k<fetypes[activef][j+1]; k++)
          {
            if (is_param[j][k])
            {
              hbx = create_hbox (0);
              add_box_child_start (GTK_ORIENTATION_VERTICAL, vbbx, hbx, FALSE, FALSE, 0);
              str = g_strdup_printf ("%s %s:", fnames[activef][j+1][k], elemts[j+6]);
              add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, markup_label(str, 250, -1, 0.0, 0.5), FALSE, FALSE, 0);
              g_free (str);
              str = NULL;
              for (l=0; l<fntypes[activef][j]; l++)
              {
                if (is_energy[k][l])
                {
                  if (str)
                  {
                    str = g_strdup_printf ("%s, %s", str, is_var[k][l]);
                  }
                  else
                  {
                    str = g_strdup_printf ("%s", is_var[k][l]);
                  }
                }
              }
              add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, markup_label(str, 250, -1, 0.0, 0.5), FALSE, FALSE, 0);
              g_free (str);
            }
          }
          for (k=0; k<fetypes[activef][j+1]; k++)
          {
            g_free (is_energy[k]);
            g_free (is_var[k]);
          }
          g_free (is_energy);
          g_free (is_var);
        }
      }
      run_this_gtk_dialog (dialog, G_CALLBACK(run_changed_energy_unit), GINT_TO_POINTER(i));
    }
    else
    {
      setup_cs_labels (i);
      tmp_field -> energy_unit = i;
    }
  }
  combo_set_active ((GtkWidget *)box, tmp_field -> energy_unit);
  update_field_trees ();
}

/*!
  \fn GtkWidget * vbox_init (int p)

  \brief classical force field assistant prepare the field object configuration widgets

  \param p the type of field object
*/
GtkWidget * vbox_init (int p)
{
  int i, j, k, l;
  int col[3]={5, 4, 4};
  gchar * str;
  GtkWidget * vbox;
  GtkWidget * hbox;
  GtkWidget * ebox;

  vbox = create_vbox (BSEP);
  hbox = create_hbox (0);
  str = g_strdup_printf ("<b>Please select the %s:</b>", felemts[0]);
  ebox = fbox (hbox, str);
  g_free (str);
  enbox = create_combo ();
  for (j=0; j<fetypes[activef][0]; j++)
  {
    str = g_strdup_printf ("%s (%s)", fnames[activef][0][j], fkeysw[activef][0][j]);
    combo_text_append (enbox, str);
    g_free (str);
  }
  combo_set_active (enbox, tmp_field -> energy_unit);
  g_signal_connect (G_OBJECT (enbox), "changed", G_CALLBACK(changed_energy_unit), GINT_TO_POINTER(0));
  gtk_widget_set_size_request (enbox, 250, 30);
  widget_set_sensitive (enbox, ! activef);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ebox, enbox, FALSE, FALSE, 60);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);

  hbox = create_hbox (0);
  ebox = fbox (hbox, "<b>Please select the component(s) of the force field:</b>");
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<u> Intra-molecular interaction(s):</u>", 200, 20, 0.0, 0.5), FALSE, FALSE, 10);

  //str = g_strdup_printf ("Use multiple molecule(s)");
  //but = check_button (str, 225, 40, tmp_field -> afp[MAXDATC], G_CALLBACK(toggle_field_params), GINT_TO_POINTER(MAXDATC));
  //g_free (str);
  //add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 30);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
  hbox = create_hbox (0);
  k = 2+MAXDATC;
  l = 0;
  for (i=0; i<3; i++)
  {
    ebox = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ebox, FALSE, FALSE, 20);
    for (j=0; j<col[i]; j++)
    {
      str = g_strdup_printf ("Use %s(s)", elemts[k-MAXDATC]);
      ff_but[l] = check_button (str, 225, 40, tmp_field -> afp[k], G_CALLBACK(toggle_field_params), GINT_TO_POINTER(k));
      g_free (str);
      widget_set_sensitive (ff_but[l], ! activef);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, ebox, ff_but[l], FALSE, FALSE, 0);
      k ++;
      l ++;
    }
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<u>Non-bonded interaction(s)</u>", 200, 20, 0.0, 0.5), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
  hbox = create_hbox (0);
  for (i=0; i<3; i++)
  {
    ebox = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ebox, FALSE, FALSE, 20);
    for (j=0; j<2; j++)
    {
      str = g_strdup_printf ("Use %s(s)", elemts[k-MAXDATC]);
      if (activef) tmp_field -> afp[k] = FALSE;
      ff_but[l] = check_button (str, 225, 40, tmp_field -> afp[k], G_CALLBACK(toggle_field_params), GINT_TO_POINTER(k));
      k ++;
      g_free (str);
      widget_set_sensitive (ff_but[l], set_nbd_but_sensitive (l-13));
      widget_set_sensitive (ff_but[l], ! activef);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, ebox, ff_but[l], FALSE, FALSE, 0);
      l ++;
    }
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  return vbox;
}

/*!
  \fn int get_field_tree_data (GtkWidget * tree, int treeid, GtkTreePath * path)

  \brief retrieve classical force field data from tree model

  \param tree the GtkWidget sending the signal
  \param treeid the type of field object
  \param path the path in the tree model
*/
int get_field_tree_data (GtkWidget * tree, int treeid, GtkTreePath * path)
{
  int res = -1;
  GtkTreeModel * tmodel = gtk_tree_view_get_model (GTK_TREE_VIEW(tree));
  if (gtk_tree_model_get_iter (tmodel, & field_iter, path))
  {
    gtk_tree_model_get (tmodel, & field_iter, 0, & res, -1);
    if (res == 0)
    {
      gtk_tree_model_get (tmodel, & field_iter, field_v[treeid]+1, & res, -1);
    }
    else
    {
      res --;
    }
  }
  return res;
}

/*!
  \fn int get_field_data_id (int k, gchar * data)

  \brief get classical force field parameter from description string

  \param k the type of field object
  \param data the target description string
*/
int get_field_data_id (int k, gchar * data)
{
  int i, j;
  gchar * str;
  i = -1;
  for (j=0; j<fetypes[activef][k]; j++)
  {
    str = g_strdup_printf ("%s (%s)", fnames[activef][k][j], exact_name(fkeysw[activef][k][j]));
    if (g_strcmp0 (data, str) == 0)
    {
      i = j;
      g_free (str);
      break;
    }
    g_free (str);
  }
  return i;
}

/*!
  \fn G_MODULE_EXPORT void changed_field_key_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data)

  \brief change combo box in classical force field tree model

  \param combo the cell renderer combo box
  \param path_string the path in the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_field_key_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data)
{
  GValue val = {0, };
  int i, j, k, l, m, n, o;
  gboolean changeit = TRUE;
  char ** vars;
  int * ids;
  i = GPOINTER_TO_INT(data);
  GObject * cmodel;
  g_object_get (combo, "model", & cmodel, NULL);
  gtk_tree_model_get_value ((GtkTreeModel *)cmodel, iter, 0, & val);
  if (gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL(field_model[i]), iter, path_string))
  {
    GValue vbl = {0, };
    gtk_tree_model_get_value (GTK_TREE_MODEL(field_model[i]), iter, field_v[i]+1, & vbl);
    j = (int) g_value_get_int (& vbl);
    gchar * str = g_strdup_printf ("%s", (char *)g_value_get_string (& val));
    l = get_field_data_id (i-5, str);
    if (l > -1)
    {
      if (i < MOLIMIT) k = combo_get_active (combo_mol[i-1]);
      if (i > 6 && i < MOLIMIT)
      {
        tmp_fstr = get_active_struct (i-7, k, j);
        o = struct_id (i);
        ids = allocint (o);
        if (i < 9)
        {
          vars = (char **)fvars_bond[activef][l];
        }
        else if (i < 11)
        {
          vars = (char **)fvars_angle[activef][l];
        }
        else if (i < 14)
        {
          vars = (char **)fvars_dihedral[activef][l];
        }
        else
        {
          vars = (char **)fvars_inversion[activef][l];
        }
        gtk_tree_model_get (GTK_TREE_MODEL(field_model[i]), iter, 0, & m, -1);
        if (m)
        {
          // Default prop
          for (n=0; n<o; n++) ids[n] = -1;
          if (l != tmp_fstr -> def -> key)
          {
            adjust_field_prop (i-7, o, NULL, ids, l);
            // print status of field prop
            str = g_strdup_printf ("<b>Default</b>: %s", str);
            gtk_tree_store_set (field_model[i], iter, field_v[i]-1, str, -1);
            gtk_tree_store_set (field_model[i], iter, field_v[i], parameters_info (i-6, l, vars, tmp_fstr -> def  -> val), -1);
            update_field_trees ();
          }
        }
        else
        {
          // Other prop
          gtk_tree_store_set_value (field_model[i], iter, field_v[i]-1, & val);
          for (n=1; n<o+1; n++)
          {
            gtk_tree_model_get (GTK_TREE_MODEL(field_model[i]), iter, n, & str, -1);
            ids[n-1] = (int) string_to_double ((gpointer)str) - 1;
          }
          adjust_field_prop (i-7, o, NULL, ids, l);
          gtk_tree_store_set (field_model[i], iter, field_v[i], parameters_info (i-6, l, vars, get_active_prop_using_atoms(tmp_fstr -> other, o, ids) -> val), -1);
        }
        g_free (ids);
      }
      else
      {
        switch (i)
        {
          case 6:
            tmp_ftet = get_active_tethered (k, j-1);
            tmp_ftet -> key = l;
            tmp_ftet -> val = NULL;
            tmp_ftet -> val = allocfloat (fvalues[activef][i-6][l]);
            gtk_tree_store_set_value (field_model[i], iter, field_v[i]-1, & val);
            gtk_tree_store_set (field_model[i], iter, field_v[i], parameters_info (0, l, fvars_teth[activef][l], tmp_ftet-> val), -1);
            break;
          case SEXTERN:
            tmp_fext = get_active_external(j-1);
            tmp_fext -> key = l;
            tmp_fext -> val = NULL;
            tmp_fext -> val = allocfloat (fvalues[activef][i-6][l]);
            gtk_tree_store_set_value (field_model[i], iter, 2, & val);
            gtk_tree_store_set (field_model[i], iter, 3, parameters_info (i-6, l, fvars_fext[activef][l], tmp_fext -> val), -1);
            break;
          default:
            k = i - MOLIMIT;
            tmp_fbody = get_active_body (j-1, k);
            if (k == 2)
            {
              changeit = tersoff_question ();
            }
            if (changeit)
            {
              if (k == 2)
              {
                check_tersoffs (-1, l);
                gtk_tree_store_clear (field_model[i]);
                fill_field_model (field_model[i], i, -1);
              }
              else
              {
                tmp_fbody -> key = l;
                tmp_fbody -> val = NULL;
                tmp_fbody -> val = allocfloat (fvalues[activef][i-6][l]);
                gtk_tree_store_set_value (field_model[i], iter, field_v[i]-1, & val);
              }
            }
            else if (i == 17)
            {
              gtk_tree_store_set (field_model[i], iter, field_v[i], parameters_info (i-6, l, fvars_ters[activef][l], tmp_fbody -> val), -1);
            }

            if (i == 15) gtk_tree_store_set (field_model[i], iter, field_v[i], parameters_info (i-6, l, fvars_vdw[activef][l], tmp_fbody -> val), -1);
            if (i == 16) gtk_tree_store_set (field_model[i], iter, field_v[i], parameters_info (i-6, l, fvars_met[activef][l], tmp_fbody -> val), -1);
            if (i == 18) gtk_tree_store_set (field_model[i], iter, field_v[i], parameters_info (i-6, l, fvars_tbd[activef][l], tmp_fbody -> val), -1);
            if (i == 19) gtk_tree_store_set (field_model[i], iter, field_v[i], parameters_info (i-6, l, fvars_fbd[activef][l], tmp_fbody -> val), -1);
            break;
        }
      }
    }
    g_free (str);
    g_value_unset(& vbl);
  }
  g_value_unset(& val);
}

/*!
  \fn GtkWidget * create_combo_mol (int f)

  \brief classical force field create molecule selection combo box

  \param f the type of force field object
*/
GtkWidget * create_combo_mol (int f)
{
  int i;
  gchar * str;

  GtkWidget * combo =  create_combo ();
  tmp_fmol = tmp_field -> first_molecule;
  for (i=0; i<tmp_field -> molecules; i++)
  {
    str = g_strdup_printf ("%d: %s", i+1, tmp_fmol -> name);
    combo_text_append (combo, str);
    g_free (str);
    if (tmp_fmol -> next != NULL) tmp_fmol = tmp_fmol -> next;
  }
  widget_set_sensitive (combo, (tmp_field -> molecules > 1) ? TRUE : FALSE);
  combo_set_active (combo, 0);
  g_signal_connect (G_OBJECT (combo), "changed", G_CALLBACK(changed_mol_box), GINT_TO_POINTER(f+1));
  return combo;
}

int actel;

/*!
  \fn gchar * pop_info (int i, int id)

  \brief get popup information message

  \param i the type of element
  \param id the id number of the element
*/
gchar * pop_info (int i, int id)
{
  int j, k, l;
  gchar * str = NULL;

  actel = 0;
  switch (i)
  {
    case 0:
      tmp_fmol = get_active_field_molecule (row_id);
      str = g_strdup_printf ("Molecule N°%d", id+1);
      actel = 1;
      break;
    default:
      if (i < MOLIMIT)
      {
        j = combo_get_active (combo_mol[i-1]);
        tmp_fmol = get_active_field_molecule (j);
      }
      switch (i)
      {
        case 1:
          tmp_fat = get_active_atom (j, id);
          str = g_strdup_printf ("Atom N°%d: \"%s\"", tmp_fat -> id+1, tmp_fat -> name);
          actel = tmp_fat -> num;
          break;
        case 2:
          if (tmp_fmol -> shells > 0)
          {
            tmp_fshell = get_active_shell (j, id);
            actel = 1;
            if (tmp_fshell -> ia[0])
            {
              k = tmp_fmol -> atoms_id[tmp_fshell -> ia[0]-1][0].a;
              str = g_strdup_printf ("Core-Shell N°%d: Atom %d (%s) - ", tmp_fshell -> id+1, tmp_fshell -> ia[0],
                                     exact_name(tmp_proj -> chemistry -> label[get_active_atom (j, k) -> sp]));
            }
            else
            {
              str = g_strdup_printf ("Core-Shell N°%d: <i>None</i> - ", tmp_fshell -> id+1);
            }
            if (tmp_fshell -> ia[1])
            {
              k = tmp_fmol -> atoms_id[tmp_fshell -> ia[1]-1][1].a;
              str = g_strdup_printf ("%s%d (%s)", str, tmp_fshell -> ia[1], exact_name(tmp_proj -> chemistry -> label[get_active_atom (j, k) -> sp]));
            }
            else
            {
              str = g_strdup_printf ("%s<i>None</i>", str);
            }
          }
          break;
        case 3:
          if (tmp_fmol -> constraints > 0)
          {
            tmp_fcons = get_active_constraint (j, id);
            actel = 1;
            if (tmp_fcons -> ia[0])
            {
              k = tmp_fmol -> atoms_id[tmp_fcons -> ia[0]-1][0].a;
              str = g_strdup_printf ("Constrained Bond N°%d: Atom %d (%s) - ", tmp_fcons -> id+1, tmp_fcons -> ia[0],
                                     exact_name(tmp_proj -> chemistry -> label[get_active_atom (j, k) -> sp]));
            }
            else
            {
              str = g_strdup_printf ("Constrained Bond N°%d: <i>None</i> - ", tmp_fcons -> id+1);
            }
            if (tmp_fcons -> ia[1])
            {
              k = tmp_fmol -> atoms_id[tmp_fcons -> ia[1]-1][1].a;
              str = g_strdup_printf ("%s%d (%s)", str, tmp_fcons -> ia[1], exact_name(tmp_proj -> chemistry -> label[get_active_atom (j, k) -> sp]));
            }
            else
            {
              str = g_strdup_printf ("%s<i>None</i>", str);
            }
          }
          break;
        case 4:
          if (tmp_fmol -> pmfs > 0)
          {
            tmp_fpmf = get_active_pmf (j, id);
            actel = 1;
            str = g_strdup_printf ("Mean Force Potential N°%d", tmp_fpmf -> id+1);
          }
          break;
        case 5:
          if (tmp_fmol -> rigids > 0)
          {
            tmp_frig = get_active_rigid (j, id);
            actel = 1;
            str = g_strdup_printf ("Rigid Unit N°%d: %d Atom(s)", tmp_frig -> id+1, tmp_frig -> num);
          }
          break;
        case 6:
          if (tmp_fmol -> tethered > 0)
          {
            tmp_ftet = get_active_tethered (j, id);
            actel = 1;
            str = g_strdup_printf ("Tethering Potential N°%d", tmp_ftet -> id+1);
          }
          break;
        case SEXTERN:
          if (tmp_field -> extern_fields > 0)
          {
            tmp_fext = get_active_external (row_id);
            str = g_strdup_printf ("%s N°: %d", felemts[i+1], tmp_fext -> id+1);
            actel = 1;
          }
          break;
        default:
          if (i < MOLIMIT)
          {
            if (tmp_fmol -> nstruct[i-7] > 0)
            {
              actel = 1;
              tmp_fstr = get_active_struct (i-7, j, row_id);
              str = g_strdup_printf ("%s N°%d: \"", mo_title[i-7], tmp_fstr -> id+1);
              for (k=0; k<struct_id(i); k++)
              {
                str = g_strdup_printf ("%s%s", str, get_active_atom (j, tmp_fstr -> aid[k]) -> name);
                if (k<struct_id(i)-1) str = g_strdup_printf ("%s-", str);
              }
              str = g_strdup_printf ("%s\"", str);
            }
          }
          else
          {
            k = i - MOLIMIT;
            if (tmp_field -> nbody[k] > 0)
            {
              tmp_fbody = get_active_body (row_id, k);
              str = g_strdup_printf ("%s N°: %d \"%s", felemts[i+1], tmp_fbody -> id+1, get_active_atom (tmp_fbody -> ma[0][0], tmp_fbody -> a[0][0]) -> name);
              for (l=1; l<body_at(k); l++)
              {
                if (tmp_fbody -> na[l] < 0)
                {
                  str = g_strdup_printf ("%s - NONE", str);
                }
                else
                {
                  str = g_strdup_printf ("%s - %s", str, get_active_atom (tmp_fbody -> ma[l][0], tmp_fbody -> a[l][0]) -> name);
                }
              }
              str = g_strdup_printf ("%s\"", str);
              actel = 1;
            }
          }
          break;
      }
      break;
  }
  return str;
}

/*!
  \fn gchar * pop_edit (int i)

  \brief get edit string label

  \param i the type of element to edit
*/
gchar * pop_edit (int i)
{
  gchar * str = NULL;
  switch (i)
  {
    case 0:
      str = g_strdup_printf ("Edit Name: \"%s\"", tmp_fmol -> name);
      break;
    default:
      str = g_strdup_printf ("Edit %s Properties", elemts[i]);
      break;
  }
  return str;
}

/*!
  \fn gchar * pop_add (int i)

  \brief get add string label

  \param i the type of element to add
*/
gchar * pop_add (int i)
{
  gchar * str = NULL;
  if ((i>1 &&  i<7) || (i>MOLIMIT && i<MAXDATA)) str = g_strdup_printf ("Add a New %s", elemts[i]);
  return str;
}

/*!
  \fn gchar * pop_remove (int i)

  \brief get remove string label

  \param i the type of element to remove
*/
gchar * pop_remove (int i)
{
  gchar * str = NULL;
  if ((i>1 &&  i<7) || (i>MOLIMIT && i<MAXDATA)) str = g_strdup_printf ("Remove %s", elemts[i]);
  return str;
}

/*!
  \fn G_MODULE_EXPORT void to_select_atom_id_from_fied_molecule (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief create new field atom from field atom popup menu callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_select_atom_id_from_fied_molecule (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  select_atom_id_from_fied_molecule (NULL, data);
}

GSimpleActionGroup * field_pop_actions = NULL;

/*!
  \fn void append_field_item (GMenu * menu, const gchar * name, const gchar * key, int item_id,
                        gchar * accel, int image_format, gpointer icon,
                        gboolean custom, GCallback handler, gpointer data,
                        gboolean check, gboolean status, gboolean radio, gboolean sensitive)

  \brief append menu items to the popup menu

  \param menu the menu to attach the new menu item to
  \param name the new menu item label, if any
  \param key the new menu item action key
  \param item_id the new menu item action id
  \param accel keyboard accelerator for the new menu item, if any (NULL otherwise)
  \param image_format the image format (in enum ImageFormats)
  \param icon the image data if any (or NULL)
  \param custom custom menu item (1= yes, 0 = no), to insert a widget later on
  \param handler the new menu item callback (or NULL)
  \param data the associated data pointer (or NULL)
  \param check is the new menu item a check menu item ?
  \param status is 'check' then what is the status of the new check menu item ?
  \param radio is the new menu item a radio menu item ?
  \param sensitive new menu item sensitivity
*/
void append_field_item (GMenu * menu, const gchar * name, const gchar * key, int item_id,
                        gchar * accel, int image_format, gpointer icon,
                        gboolean custom, GCallback handler, gpointer data,
                        gboolean check, gboolean status, gboolean radio, gboolean sensitive)
{
  gchar * str_a, * str_b, * str_c;
  str_a = g_strdup_printf ("set-%s", key);
  str_b = g_strdup_printf ("%s.%d", str_a, item_id);
  str_c = (sensitive) ? g_strdup_printf ("ff-%d.%s", item_id, (radio) ? str_a : str_b) : g_strdup_printf ("None");
  append_menu_item (menu, name, (const gchar *) str_c, accel, (custom) ? str_b : NULL, image_format, icon, check, status, radio, (radio) ? (const gchar *)str_b : NULL);
  if (handler)
  {
    if (! radio || (radio && status))
    {
      widget_add_action (field_pop_actions, (radio) ? (const gchar *)str_a : (const gchar *)str_b, handler, data,
                         check, status, radio, (const gchar *)str_b);
    }
  }
  g_free (str_a);
  g_free (str_b);
  g_free (str_c);
}

#ifdef GTK4
/*!
  \fn void pop_up_field_context_menu (int row_id, GtkWidget * widget, double event_x, double event_y, gpointer data)

  \brief classical force field tree model popup menu GTK4

  \param row_id the row id in the tree model
  \param widget the target tree model
  \param event_x x position
  \param event_y y position
  \param data the associated data pointer
*/
void pop_up_field_context_menu (int id, GtkWidget * widget, double event_x, double event_y, gpointer data)
#else
/*!
  \fn void pop_up_field_context_menu (int row_id, GtkWidget * widget, GdkEvent * event, gpointer data)

  \brief classical force field tree model popup menu GTK3

  \param row_id the row id in the tree model
  \param widget the target tree model
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
void pop_up_field_context_menu (int row_id, GtkWidget * widget, GdkEvent * event, gpointer data)
#endif
{
  GtkWidget * menu;
  gchar * str;
  int i, j, k, l;
  i = GPOINTER_TO_INT (data);
  str = pop_info (i, row_id);
  if (field_pop_actions) g_object_unref (field_pop_actions);
  field_pop_actions = g_simple_action_group_new ();
  GMenu * fmenu = g_menu_new ();
  GMenu * fmenus;
  if (str)
  {
    fmenus = g_menu_new ();
    append_field_item (fmenus, str, "pop", i, NULL, IMG_NONE, NULL, FALSE, NULL, NULL, FALSE, FALSE, FALSE, TRUE);
    g_free (str);
    g_menu_append_section (fmenu, NULL, (GMenuModel *)fmenus);
    g_object_unref (fmenus);
  }
  if (actel > 0)
  {
    str = pop_edit (i);
    if (str)
    {
      fmenus = g_menu_new ();
      append_field_item (fmenus, str, "edit-fp", i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(edit_field_prop), data, FALSE, FALSE, FALSE, TRUE);
      g_free (str);
      g_menu_append_section (fmenu, NULL, (GMenuModel *)fmenus);
      g_object_unref (fmenus);
    }
    switch (i)
    {
      case 0:
        if (tmp_field -> molecules < tmp_coord -> totcoord[2] && tmp_fmol -> multi > 1)
        {
          fmenus = g_menu_new ();
          append_field_item (fmenus, "Add New Molecule", "add-mol", i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(add_molecule_to_field), data, FALSE, FALSE, FALSE, TRUE);
          g_menu_append_section (fmenu, NULL, (GMenuModel *)fmenus);
          g_object_unref (fmenus);
        }
        if (tmp_field -> molecules > tmp_coord -> totcoord[3]
            && tmp_fmol -> mol -> multiplicity > 1
            && tmp_fmol -> multi < tmp_fmol -> mol -> multiplicity)
        {
          fmenus = g_menu_new ();
          str = g_strdup_printf ("Remove Molecule %s From Field", tmp_fmol -> name);
          append_field_item (fmenus, str, "rem-mol", i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(remove_molecule_from_field), data, FALSE, FALSE, FALSE, TRUE);
          g_free (str);
          g_menu_append_section (fmenu, NULL, (GMenuModel *)fmenus);
          g_object_unref (fmenus);
        }
        break;
      default:
        if (i == 1)
        {
          j = combo_get_active (combo_mol[i-1]);
          tmp_fmol = get_active_field_molecule (j);
          if (actel > 1)
          {

            fmenus = g_menu_new ();
            str = g_strdup_printf ("Created New Field Atom From %s Atom(s)", tmp_fat -> name);
            append_field_item (fmenus, str, "add-fat", i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_select_atom_id_from_fied_molecule), GINT_TO_POINTER(0), FALSE, FALSE, FALSE, TRUE);
            g_menu_append_section (fmenu, NULL, (GMenuModel *)fmenus);
            g_object_unref (fmenus);
          }
          l = 0;
          tmp_fbt = tmp_fmol -> first_atom;
          for (k=0; k<tmp_fmol -> atoms; k++)
          {
            if (tmp_fbt -> sp == tmp_fat -> sp) l ++;
            if (tmp_fbt -> next != NULL) tmp_fbt = tmp_fbt -> next;
          }
          if (l > 1)
          {
            fmenus = g_menu_new ();
            str = g_strdup_printf ("Remove Atom %s From Field Molecule", tmp_fat -> name);
            append_field_item (fmenus, str, "rem-fat", i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(remove_atom_from_field_molecule), (gpointer *)tmp_fat, FALSE, FALSE, FALSE, TRUE);
            g_free (str);
            g_menu_append_section (fmenu, NULL, (GMenuModel *)fmenus);
            g_object_unref (fmenus);
          }
        }
        str = pop_remove (i);
        if (str) append_field_item (fmenu, str, "rem-fp", i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(remove_field_prop), data, FALSE, FALSE, FALSE, TRUE);
        g_free (str);
        break;
    }
  }
  str = pop_add (i);
  if (str != NULL)
  {
    fmenus = g_menu_new ();
    append_field_item (fmenus, str, "add-prop", i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(add_field_prop), data, FALSE, FALSE, FALSE, TRUE);
    g_free (str);
    g_menu_append_section (fmenu, NULL, (GMenuModel *)fmenus);
    g_object_unref (fmenus);
  }
#ifdef GTK4
  menu = gtk_popover_menu_new_from_model_full ((GMenuModel *)fmenu, GTK_POPOVER_MENU_NESTED);
  gtk_widget_set_parent (menu, widget);
  // gtk_widget_set_size_request (menu, -1, i);
#else
  menu = gtk_menu_new_from_model ((GMenuModel *)fmenu);
#endif
  str = g_strdup_printf ("ff-%d", i);
  gtk_widget_insert_action_group (menu, str, G_ACTION_GROUP(field_pop_actions));
  g_free (str);
#ifdef GTK4
  pop_menu_at_pointer (menu, event_x, event_y);
#else
  pop_menu_at_pointer (menu, event);
#endif
  /* GtkWidget * prop;
  menu = gtk_menu_new ();
  if (str != NULL)
  {
    prop = create_menu_item (FALSE, str);
    g_free (str);
    gtk_menu_shell_append ((GtkMenuShell *)menu, prop);
    if (actel > 0) add_menu_separator (menu);
  }
  if (actel > 0)
  {
    str = pop_edit (i);
    if (str != NULL)
    {
      prop = create_menu_item (FALSE, str);
      g_free (str);
      gtk_menu_shell_append ((GtkMenuShell *)menu, prop);
      g_signal_connect (G_OBJECT(prop), "activate", G_CALLBACK(edit_field_prop), data);
    }
    switch (i)
    {
      case 0:
        if (tmp_field -> molecules < tmp_coord -> totcoord[2] && tmp_fmol -> multi > 1)
        {
          add_menu_separator (menu);
          prop = create_menu_item (FALSE, "Add New Molecule");
          gtk_menu_shell_append ((GtkMenuShell *)menu, prop);
          g_signal_connect (G_OBJECT(prop), "activate", G_CALLBACK(add_molecule_to_field), data);
        }
        if (tmp_field -> molecules > tmp_coord -> totcoord[3]
            && tmp_fmol -> mol -> multiplicity > 1
            && tmp_fmol -> multi < tmp_fmol -> mol -> multiplicity)
        {
          add_menu_separator (menu);
          str = g_strdup_printf ("Remove Molecule %s From Field", tmp_fmol -> name);
          prop = create_menu_item (FALSE, str);
          g_free (str);
          gtk_menu_shell_append ((GtkMenuShell *)menu, prop);
          g_signal_connect (G_OBJECT(prop), "activate", G_CALLBACK(remove_molecule_from_field), (gpointer *)tmp_fmol);
        }
        break;
      default:
        if (i == 1)
        {
          j = combo_get_active (combo_mol[i-1]);
          tmp_fmol = get_active_field_molecule (j);
          if (actel > 1)
          {
            add_menu_separator (menu);
            str = g_strdup_printf ("Created New Field Atom From %s Atom(s)", tmp_fat -> name);
            prop = create_menu_item (FALSE, str);
            g_free (str);
            gtk_menu_shell_append ((GtkMenuShell *)menu, prop);
            g_signal_connect (G_OBJECT(prop), "activate", G_CALLBACK(select_atom_id_from_fied_molecule), GINT_TO_POINTER(0));
          }
          l = 0;
          tmp_fbt = tmp_fmol -> first_atom;
          for (k=0; k<tmp_fmol -> atoms; k++)
          {
            if (tmp_fbt -> sp == tmp_fat -> sp) l ++;
            if (tmp_fbt -> next != NULL) tmp_fbt = tmp_fbt -> next;
          }
          if (l > 1)
          {
            add_menu_separator (menu);
            str = g_strdup_printf ("Remove Atom %s From Field Molecule", tmp_fat -> name);
            prop = create_menu_item (FALSE, str);
            g_free (str);
            gtk_menu_shell_append ((GtkMenuShell *)menu, prop);
            g_signal_connect (G_OBJECT(prop), "activate", G_CALLBACK(remove_atom_from_field_molecule), (gpointer *)tmp_fat);
          }
        }
        str = pop_remove (i);
        if (str != NULL)
        {
          prop = create_menu_item (FALSE, str);
          g_free (str);
          gtk_menu_shell_append ((GtkMenuShell *)menu, prop);
          g_signal_connect (G_OBJECT(prop), "activate", G_CALLBACK(remove_field_prop), data);
        }
        break;
    }
  }
  str = pop_add (i);
  if (str != NULL)
  {
    if (actel > 0) add_menu_separator (menu);
    prop = create_menu_item (FALSE, str);
    g_free (str);
    gtk_menu_shell_append ((GtkMenuShell *)menu, prop);
    g_signal_connect (G_OBJECT(prop), "activate", G_CALLBACK(add_field_prop), data);
  }
  pop_menu_at_pointer (menu, (GdkEvent *)event);
#endif*/
}

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT gboolean on_pop_up_field (GtkWidget * widget, gpointer data)

  \brief classical force field tree model popup contextual menu callback GTK3

  \param widget the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_pop_up_field (GtkWidget * widget, gpointer data)
{
  pop_up_field_context_menu (row_id, widget, NULL, data);
  return TRUE;
}
#endif

#ifdef GTK4
/*!
  \fn void field_button_event (double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data)

  \brief classical force field tree model button event GTK4

  \param event_x x position
  \param event_y y position
  \param event_button event button
  \param event_type event type
  \param event_time event time
  \param data the associated data pointer
*/
void field_button_event (double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data)
#else
/*!
  \fn void field_button_event (GdkEvent * event, double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data)

  \brief classical force field tree model button event GTK3

  \param event the GdkEvent triggering the signal
  \param event_x x position
  \param event_y y position
  \param event_button event button
  \param event_type event type
  \param event_time event time
  \param data the associated data pointer
*/
void field_button_event (GdkEvent * event, double event_x, double event_y, guint event_button, guint event_type, guint32 event_time, gpointer data)
#endif
{
  if (event_type == GDK_BUTTON_PRESS)
  {
    GtkTreePath * path;
    GtkTreeViewColumn * column;
    int i, j, k;
    i = GPOINTER_TO_INT(data);
#ifdef GTK4
    int e_x, e_y;
    gtk_tree_view_convert_widget_to_bin_window_coords (GTK_TREE_VIEW(field_tree[i]), event_x, event_y, & e_x, & e_y);
    gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(field_tree[i]), e_x, e_y, & path, & column, & j, & k);
#else
    gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(field_tree[i]), event_x, event_y, & path, & column, & j, & k);
#endif
    if (path != NULL)
    {
      row_id = get_field_tree_data (field_tree[i], i, path);
    }
    else
    {
      row_id = 0;
    }
    if (event_button == 3)
    {
#ifdef GTK4
      pop_up_field_context_menu (row_id, field_tree[i], event_x, event_y, data);
#else
      pop_up_field_context_menu (row_id, field_tree[i], event, data);
#endif
    }
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void on_field_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief classical force field tree model button pressed callback GTK4

  \param gesture the GtkGesture sending the signal
  \param n_press number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_field_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  field_button_event (x, y, gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture), GDK_BUTTON_PRESS, gtk_event_controller_get_current_event_time((GtkEventController *)gesture), data);
}

/*!
  \fn G_MODULE_EXPORT void on_field_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief  classical force field tree model button released callback GTK4

  \param gesture the GtkGesture sending the signal
  \param n_press number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_field_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  field_button_event (x, y, gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture), GDK_BUTTON_RELEASE, gtk_event_controller_get_current_event_time((GtkEventController *)gesture), data);
}
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_field_button_event (GtkWidget * widget, GdkEvent * event, gpointer data)

  \brief classical force field tree model button event callback GTK3

  \param widget the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_field_button_event (GtkWidget * widget, GdkEvent * event, gpointer data)
{
  GdkEventButton * bevent = (GdkEventButton *)event;
  field_button_event (event, bevent -> x, bevent -> y, bevent -> button, bevent -> type, bevent -> time, data);
  return FALSE;
}
#endif

/*!
  \fn void field_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color in the classical force field tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j, k, l;
  int tree = GPOINTER_TO_INT(data);
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  if (tree  < SEXTERN)
  {
    if (i == 0)
    {
      gtk_tree_model_get (mod, iter, field_v[tree]+1, & i, -1);
      i ++;
    }
    else if  (i < 0)
    {
      i = - i;
    }
    for (k=0; k<field_v[tree]; k++)
    {
      if (is_special[tree][k] == 2) break;
    }
    gtk_tree_model_get (mod, iter, k, & j, -1);
    l = (tree && tree < MOLIMIT) ? combo_get_active (combo_mol[tree-1]) : 0;
    k = get_field_objects (tree, l);
    set_renderer_color (j, renderer, init_color (i-1, k));
  }
}

/*!
  \fn void field_set_markup_box (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer pango markup in the classical force field tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_markup_box (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int tree = GPOINTER_TO_INT(data);
  gchar * str = NULL;
  gtk_tree_model_get (mod, iter, field_v[tree]-1, & str, -1);
  g_object_set (renderer, "markup", str, NULL, NULL);
  g_free (str);
}

/*!
  \fn void field_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer pango markup in the classical force field tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i;
  int tree = GPOINTER_TO_INT(data);
  i = (tree > 5) ? 1 : 0;
  if (i)
  {
    gchar * str = NULL;
    gtk_tree_model_get (mod, iter, field_v[tree], & str, -1);
    g_object_set (renderer, "markup", str, NULL, NULL);
    g_free (str);
  }
  else
  {
    gchar * str = NULL;
    GtkTreeViewColumn * cal;
    switch (tree)
    {
      case 0:
        gtk_tree_model_get (mod, iter, 3, & str, -1);
        break;
      default:
        // CHECK THIS
        cal = gtk_tree_view_get_column (GTK_TREE_VIEW(field_tree[tree]), 1);
        if (cal == col)
        {
          gtk_tree_model_get (mod, iter, 1, & str, -1);
        }
        else
        {
          gtk_tree_model_get (mod, iter, 2, & str, -1);
        }
        break;
    }
    g_object_set (renderer, "markup", str, NULL, NULL);
    g_free (str);
  }
}

/*!
  \fn void field_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer visibility in the classical force field tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  int tree = GPOINTER_TO_INT(data);
  if (tree != SEXTERN)
  {
    if (tree == 4 || tree == 5)
    {
      gtk_tree_model_get (mod, iter, 0, & j, -1);
      if (j < 0) j = 0;
    }
    else
    {
      for (i=0; i<field_v[tree]; i++)
      {
        if (is_special[tree][i] == 3) break;
      }
      gtk_tree_model_get (mod, iter, i, & j, -1);
    }
    gtk_cell_renderer_set_visible (renderer, j);
  }
}

/*!
  \fn void field_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color and markup in the classical force field tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  field_set_color (col, renderer, mod, iter, data);
  field_set_markup (col, renderer, mod, iter, data);
}

/*!
  \fn void field_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color and visibility in the classical force field tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  field_set_color (col, renderer, mod, iter, data);
  field_set_visible (col, renderer, mod, iter, data);
}

/*!
  \fn void prop_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color and visibility in the classical force field structural property tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void prop_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i;
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  gtk_cell_renderer_set_visible (renderer, i);
  field_set_color (col, renderer, mod, iter, data);
}

/*!
  \fn void pmf_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color and visibility in the classical force field mean force potential tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void pmf_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  field_set_color (col, renderer, mod, iter, GINT_TO_POINTER(4));
  int i, j, k;
  i = GPOINTER_TO_INT(data);
  if (i == 3)
  {
    gtk_tree_model_get (mod, iter, i, & j, -1);
    k = (! j || j < 0) ? 0 : 1;
  }
  else
  {
    gtk_tree_model_get (mod, iter, 4, & k, -1);
  }
  gtk_cell_renderer_set_visible (renderer, k);
}

/*!
  \fn void rig_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color and visibility in the classical force field rigid constraint(s) tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void rig_set_color_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  field_set_color (col, renderer, mod, iter, data);
  int i;
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  if (i < 0) i = 0;
  gtk_cell_renderer_set_visible (renderer, ! i);
}

/*!
  \fn void field_set_color_markup_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color, markup and visibility in the classical force field tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_color_markup_and_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  field_set_color (col, renderer, mod, iter, data);
  field_set_markup (col, renderer, mod, iter, data);
  field_set_visible (col, renderer, mod, iter, data);
}

/*!
  \fn void field_set_color_markup_and_visible_box (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color, markup and visibility in the classical force field tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void field_set_color_markup_and_visible_box (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  field_set_color (col, renderer, mod, iter, data);
  field_set_markup_box (col, renderer, mod, iter, data);
  field_set_visible (col, renderer, mod, iter, data);
}

/*!
  \fn G_MODULE_EXPORT void edit_field_cell (GtkCellRendererText * cell, gchar * path_string,  gchar * new_text, gpointer data)

  \brief  edit cell in the classical force field tree model

  \param cell the GtkCellRendererText sending the signal
  \param path_string the path in the tree model
  \param new_text the new string to insert
  \param data the associated data pointer
*/
G_MODULE_EXPORT void edit_field_cell (GtkCellRendererText * cell, gchar * path_string,  gchar * new_text, gpointer data)
{
  int i, j, k, l;
  i = GPOINTER_TO_INT(data);
  if (i < 2)
  {
    j = 1;
  }
  else if (i < 6)
  {
    j = 2;
  }
  else if (i == 6)
  {
    j = 3;
  }
  else if (i < 9)
  {
    j = 4;
  }
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(field_model[j]), & iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(field_model[j]), & iter, 0, & k, -1);
  l = combo_get_active (combo_mol[j-1]);
  float val = string_to_double ((gpointer)new_text);
  k --;
  if (i == 0) get_active_atom (l, k) -> mass = val;
  if (i == 1) get_active_atom (l, k) -> charge = val;
  if (i == 2) get_active_shell (l, k) -> m = val;
  if (i == 3) get_active_shell (l, k) -> z = val;
  if (i == 4) get_active_shell (l, k) -> k2 = val;
  if (i == 5) get_active_shell (l, k) -> k4 = val;
  if (i == 6) get_active_constraint (l, k) -> length = val;
  if (i == 7) get_active_pmf (l, k) -> length = val;
  if (i == 8)
  {
    int n, m;
    gtk_tree_model_get (GTK_TREE_MODEL(field_model[j]), & iter, 3, & n, 4, & m, -1);
    if (n < 0 && m > 0)
    {
      get_active_pmf (l, k) -> weight[-n-1][m-1] = val;
    }
  }
  if (j < MOLIMIT) changed_mol_box (GTK_COMBO_BOX(combo_mol[j-1]), GINT_TO_POINTER(j));
}

/*!
  \fn void get_field_iter_and_edit (gchar * path_string, gpointer data)

  \brief edit field property in the classical force field tree model

  \param path_string the path in the tree model
  \param data the associated data pointer
*/
void get_field_iter_and_edit (gchar * path_string, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(field_model[i]),
                           & field_iter,
                           gtk_tree_path_new_from_string (path_string));
  if (i == 4 || i == 5 || (i > 6 && i < MOLIMIT))
  {
    GValue vbl = {0, };
    if (i < 6)
    {
      gtk_tree_model_get_value (GTK_TREE_MODEL(field_model[i]), & field_iter, 0, & vbl);
    }
    else
    {
      gtk_tree_model_get_value (GTK_TREE_MODEL(field_model[i]), & field_iter, field_v[i]+1, & vbl);
    }
    row_id = (int) g_value_get_int (& vbl);
    if (i < 6) row_id = abs(row_id) - 1;
  }
  edit_field_prop (g_simple_action_new ("Dummy", NULL), NULL, data);
}

/*!
  \fn G_MODULE_EXPORT void to_edit_field_prop (GtkCellRenderer * cell, GtkCellEditable * editable, gchar * path_string, gpointer data)

  \brief to edit data in the classical force field tree model

  \param cell the GtkTreeView sending the signal
  \param editable the editable
  \param path_string the path in the tree model
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_edit_field_prop (GtkCellRenderer * cell, GtkCellEditable * editable, gchar * path_string, gpointer data)
{
  destroy_this_widget (GTK_WIDGET(editable));
  get_field_iter_and_edit (path_string, data);
}

/*!
  \fn G_MODULE_EXPORT void on_field_row_activated (GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * col, gpointer data)

  \brief activating row in the classical force field tree model

  \param treeview the GtkTreeView sending the signal
  \param path the path in the tree view
  \param col the column in the tree view
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_field_row_activated (GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * col, gpointer data)
{
  get_field_iter_and_edit (gtk_tree_path_to_string (path), data);
}

/*!
  \fn GtkWidget * create_field_tree (int f)

  \brief classical force field create the list store selection widgets

  \param f the page number / type of field object(s)
*/
GtkWidget * create_field_tree (int f)
{
  int i, j, k, l, m;
  gchar * str;
  GtkListStore * list_store_combo;
  GtkTreeIter iter;
  gboolean combox = FALSE;

  l = (f > 5) ? 1 : 0;
  m = 0;
  field_model[f] = gtk_tree_store_newv (field_v[f]+l+field_a[f], col_type[f]);
  field_tree[f] = gtk_tree_view_new_with_model(GTK_TREE_MODEL(field_model[f]));
  for (i=0; i<field_v[f]+l; i++)
  {
    if (is_special[f][i] == 1)
    {
      k = f-5;
      field_renderer[f][i] = gtk_cell_renderer_combo_new ();
      list_store_combo = gtk_list_store_new (1, G_TYPE_STRING);
      for (j=0; j<fetypes[activef][k]; j++)
      {
        str = g_strdup_printf ("%s (%s)", fnames[activef][k][j], exact_name(fkeysw[activef][k][j]));
        gtk_list_store_append (list_store_combo, & iter);
        gtk_list_store_set (list_store_combo, & iter, 0, str, -1);
        g_free (str);
      }
      g_object_set (field_renderer[f][i], "model", list_store_combo,
                                          "text-column", 0,
                                          "has-entry", FALSE,
                                          "editable", TRUE, NULL);
      g_signal_connect (G_OBJECT(field_renderer[f][i]), "changed", G_CALLBACK(changed_field_key_renderer), GINT_TO_POINTER(f));
      field_col[f][i] = gtk_tree_view_column_new_with_attributes (ctitle[f][i], field_renderer[f][i], "text", i, NULL);
      g_object_unref (list_store_combo);
      combox = TRUE;
    }
    else if (combox)
    {
      field_renderer[f][i] = gtk_cell_renderer_text_new ();
      field_col[f][i] = gtk_tree_view_column_new_with_attributes ("Parameter(s)", field_renderer[f][i], "text", i, NULL);
      m = 1;
      combox = FALSE;
    }
    else if (is_special[f][i] > 1 && is_special[f][i] < 4)
    {
      field_renderer[f][i] = gtk_cell_renderer_toggle_new ();
      field_col[f][i] = gtk_tree_view_column_new_with_attributes (ctitle[f][i-m], field_renderer[f][i], "active", i, NULL);
      g_signal_connect (G_OBJECT(field_renderer[f][i]), "toggled",
                        G_CALLBACK(on_toggle_visualize_or_select_object),
                        & tmp_view -> colorp[f][is_special[f][i]-2]);
      if ((f == 4 && i == field_v[f]+l-1) || f == SEXTERN) gtk_cell_renderer_toggle_set_radio (GTK_CELL_RENDERER_TOGGLE(field_renderer[f][i]), TRUE);
      gtk_tree_view_column_set_clickable (field_col[f][i], TRUE);
      if (f != SEXTERN)
      {
        g_signal_connect (G_OBJECT(field_col[f][i]), "clicked", G_CALLBACK(visualize_or_select_all_elements),  & tmp_view -> colorp[f][is_special[f][i]-2]);
      }
    }
    else
    {
      field_renderer[f][i] = gtk_cell_renderer_text_new();
      field_col[f][i] = gtk_tree_view_column_new_with_attributes (ctitle[f][i-m], field_renderer[f][i], "text", i, NULL);
    }

    if (is_special[f][i] == 4)
    {
      g_object_set (field_renderer[f][i], "editable", TRUE, NULL);
      if (f < 3)
      {
        j = 2*(f-1) + i - 3;
      }
      else if (f == 3)
      {
        j = 6;
      }
      else if (f == 4)
      {
        j = (i == 1) ? 7 : 8;
      }
      g_signal_connect (G_OBJECT(field_renderer[f][i]), "edited", G_CALLBACK(edit_field_cell), GINT_TO_POINTER(j));
    }
    if ((f == 0 && i == 3) || (f == 2 && (i == 1 || i == 2)) || (f == 3 && (i == 1 || i == 2)))
    {
      gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], field_set_color_and_markup, GINT_TO_POINTER(f), NULL);
    }
    else if (f == 4 || f == 5)
    {
      if ((f == 4 && i < 3) || (f == 5 && i < 2))
      {
        gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], field_set_color_and_visible, GINT_TO_POINTER(f), NULL);
      }
      else if (f == 4 && (i == 3 || i == 4 || i == 5))
      {
        gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], pmf_set_color_and_visible, GINT_TO_POINTER(i), NULL);
      }
      else if (f == 5 && i == 2)
      {
        gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], rig_set_color_and_visible, GINT_TO_POINTER(f), NULL);
      }
      else
      {
        gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], field_set_visible, GINT_TO_POINTER(f), NULL);
      }
    }
    else if (f > 6 && f < MOLIMIT && (i == 0 || i == struct_id(f)+1))
    {
      gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], prop_set_color_and_visible, GINT_TO_POINTER(f), NULL);
    }
    else if (l == 1 && i > field_v[f]-2)
    {
      if (i == field_v[f]-1)
      {
        gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], field_set_color_markup_and_visible_box, GINT_TO_POINTER(f), NULL);
      }
      else if (i == field_v[f])
      {
        gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], field_set_color_markup_and_visible, GINT_TO_POINTER(f), NULL);
        g_object_set (field_renderer[f][i], "editable", TRUE, NULL);
        g_signal_connect (G_OBJECT(field_renderer[f][i]), "editing-started", G_CALLBACK(to_edit_field_prop), GINT_TO_POINTER(f));
      }
    }
    else if (is_special[f][i] < 2 || is_special[f][i] > 3)
    {
      gtk_tree_view_column_set_cell_data_func (field_col[f][i], field_renderer[f][i], field_set_color, GINT_TO_POINTER(f), NULL);
    }
    gtk_tree_view_column_set_alignment (field_col[f][i], 0.5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(field_tree[f]), field_col[f][i]);
  }

  fill_field_model (field_model[f], f, 0);
  g_object_unref (field_model[f]);
  GtkTreeSelection * fieldselect = gtk_tree_view_get_selection (GTK_TREE_VIEW(field_tree[f]));
  gtk_tree_selection_set_mode (fieldselect, GTK_SELECTION_SINGLE);
#ifdef GTK4
  add_widget_gesture_and_key_action (field_tree[f], "field-button-pressed", G_CALLBACK(on_field_button_pressed), GINT_TO_POINTER(f),
                                                    "field-button-released", G_CALLBACK(on_field_button_released), GINT_TO_POINTER(f),
                                                    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#else
  g_signal_connect (G_OBJECT(field_tree[f]), "button_press_event", G_CALLBACK(on_field_button_event), GINT_TO_POINTER(f));
  g_signal_connect (G_OBJECT(field_tree[f]), "popup-menu", G_CALLBACK(on_pop_up_field), GINT_TO_POINTER(f));
  g_signal_connect (G_OBJECT(field_tree[f]), "row-activated", G_CALLBACK(on_field_row_activated), GINT_TO_POINTER(f));
#endif
  return field_tree[f];
}

/*!
  \fn void create_field_list (GtkWidget * vbx, int f)

  \brief classical force field creation prepare list store selection widgets

  \param vbx the GtkWidget sending the signal
  \param f the page number
*/
void create_field_list (GtkWidget * vbx, int f)
{
  int i;
  field_tree[f] = NULL;
  field_model[f] = NULL;
  for (i=0; i<11; i++)
  {
    field_renderer[f][i] = NULL;
    field_col[f][i] = NULL;
  }
  GtkWidget * scrollsets = create_scroll (NULL, -1, -1, GTK_SHADOW_ETCHED_IN);
  if (f == 0)
  {
    gtk_widget_set_size_request (scrollsets, field_s[f], 175);
  }
  else if (f < MOLIMIT)
  {
    if (f == 1)
    {
      gtk_widget_set_size_request (scrollsets, field_s[f], 320);
    }
    else if (f == 8 || f == 10 || f == 12 || f == 13)
    {
      gtk_widget_set_size_request (scrollsets, field_s[f], 180);
    }
    else
    {
      gtk_widget_set_size_request (scrollsets, field_s[f], 220);
    }
  }
  else
  {
    gtk_widget_set_size_request (scrollsets, field_s[f], 300);
  }
  add_container_child (CONTAINER_SCR, scrollsets, create_field_tree(f));
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, scrollsets, FALSE, FALSE, (800-field_s[f])/2);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbx, hbox, FALSE, FALSE, 20);
}

/*!
  \fn GtkWidget * create_mol_box (int f)

  \brief classical force field create molecule selection widgets

  \param f the type of force field object
*/
GtkWidget * create_mol_box (int f)
{
  GtkWidget * hbox;
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Please select molecule: ", -1, -1, 0.0, 0.5), FALSE, FALSE, 100);
  combo_mol[f] =  create_combo_mol (f);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo_mol[f], FALSE, FALSE, 10);
  return hbox;
}

/*!
  \fn GtkWidget * vbox_field (int f)

  \brief classical force field create page option widgets

  \param f the page number
*/
GtkWidget * vbox_field (int f)
{
  gchar * str;
  GtkWidget * vbx;
  GtkWidget * hbx;
  GtkWidget * vbox;
  GtkWidget * hbox;
  vbox = create_vbox (5);
  if (f > 6 && f < MOLIMIT)
  {
    hbx = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbx, FALSE, FALSE, 0);
    vbx = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, vbx, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbx, gtk_image_new_from_file(ifield[f-7]), FALSE, FALSE, 50);
  }
  if (f == 0)
  {
    hbox = fbox (vbox, "Number of isolated molecular fragments: ");
    str = g_strdup_printf ("<b>%d</b>", tmp_coord -> totcoord[2]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 75);
    g_free (str);
    str = g_strdup_printf ("\tEach of these fragments can be described separately in the force field.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
    g_free (str);
    hbox = fbox (vbox, "Number of distinct molecules: ");
    str = g_strdup_printf ("<b>%d</b>", tmp_coord -> totcoord[3]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 75);
    g_free (str);
    hbox = fbox (vbox, "Number of molecules in the force field: ");
    set_mol_num_label ();
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, mol_num_label, FALSE, FALSE, 75);
  }
  else if (f < MOLIMIT)
  {
    // if f > 0, ie. atoms tab at least
    // need a combobox to pick the 'active' molecule and its corresponding force field
    // If using molecules the combo box is active, and visible
    // Otherwise deactivate and hide the combo box
    // Anyway fill the treestore according to the selection

    mol_box[f-1] = create_mol_box (f-1);
    if (f > 6 && f < MOLIMIT)
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbx, mol_box[f-1], FALSE, FALSE, 0);
    }
    else
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, mol_box[f-1], FALSE, FALSE, 0);
    }
    widget_set_sensitive (mol_box[f-1], tmp_field -> afp[MAXDATC]);
  }

  if (f > 6 && f < MOLIMIT)
  {
    hbox = fbox (vbx, g_strdup_printf ("%s(s) properties: ", felemts[f+1]));
  }
  else
  {
    hbox = fbox (vbox, g_strdup_printf ("%s(s) properties: ", felemts[f+1]));
  }
  if (f > 0)
  {
    field_label[f] = markup_label(set_field_label(f, 0), 120, 40, 0.35, 0.5);
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, field_label[f], TRUE, TRUE, 0);
    if (f > 6 && f < MOLIMIT)
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbx, hbox, FALSE, FALSE, 10);
    }
    else
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
    }
  }
  create_field_list (vbox, f);
  if (f > 6 && f < MOLIMIT)
  {
    if (f == 8 || f == 10 || f == 12 || f == 13)
    {
      int g = (f < 12) ? f - 1 : 11;
      str = g_strdup_printf ("\t <b>*</b> in the FIELD file %s(s) appear in the %s section.\n"
                             "\t They are presented separately in this assistant for clarity purposes only.",
                             elemts[f], elemts[g]);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
      g_free (str);
    }
    else if (f == 14)
    {
      str = g_strdup_printf ("\t <b>*</b> the potential will be calculated by the sum of the 3 possible inversion terms between atoms 1 (center), 2, 3 and 4.");
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
      g_free (str);
    }
    str = g_strdup_printf ("\t <b>(1)</b> average value for the force field element as measured in the model.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    str = g_strdup_printf ("\t <b>(2)</b> only the selected element(s) will be used to create the force field.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    str = g_strdup_printf ("\t <b>(3)</b> each force field element can be tuned separately, if not the <b>Default</b> parameters will be used.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);

  }
  else if (f == 2)
  {
    cs_label[0] = markup_label(" ", -1, -1, 0.0, 0.5);
    cs_label[1] = markup_label(" ", -1, -1, 0.0, 0.5);
    setup_cs_labels (tmp_field -> energy_unit);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, cs_label[0], FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, cs_label[1], FALSE, FALSE, 0);
    str = g_strdup_printf ("\t <b>(3)</b> only the selected element(s) will be used to create the force field.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
  }
  else if (f == 3)
  {
    str = g_strdup_printf ("\t <b>(1)</b> average value for the force field element as measured in the model.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    str = g_strdup_printf ("\t <b>(2)</b> only the selected element(s) will be used to create the force field.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
  }
  else if (f == 4)
  {
    str = g_strdup_printf ("\t <b>(1)</b> average distance between the barycenters of units 1 and 2 as measured in the model.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    str = g_strdup_printf ("\t <b>(2)</b> if all 0.0 then atomic weight(s) will be used by DL-POLY.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    str = g_strdup_printf ("\t <b>(3)</b> only the selected element(s) will be used to create the force field.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
  }
  else if ((f > 2 && f < 7) || f > 11)
  {
    str = g_strdup_printf ("\t <b>(1)</b> only the selected element(s) will be used to create the force field.");
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
  }
  return vbox;
}

GtkWidget * field_i_prep[2];
GtkWidget * field_i_box[2];
GtkWidget * field_i_lab[2];
GtkWidget * field_i_fixed[2];
GtkWidget * field_i_combo[2];
GtkWidget * field_i_obj[2];
GtkWidget * preview_but;
gboolean assist_init;
int saved_label_format[2];
int saved_label_position[2];

/*!
  \fn void close_the_assistant (GtkAssistant * assistant)

  \brief classical force field creation close the assistant

  \param assistant the target assistant
*/
void close_the_assistant (GtkAssistant * assistant)
{
  field_assistant = destroy_this_widget (field_assistant);
  int j;
#ifdef GTK3
  // GTK3 Menu Action To Check
  gboolean i = (tmp_proj -> force_field[activef]) ? TRUE : FALSE;
  widget_set_sensitive (tmp_view -> color_styles[5], i);
  widget_set_sensitive (tmp_view -> color_styles[5+ATOM_MAPS], i);
#endif
  field_color = FALSE;
  for (j=0; j<2; j++)
  {
    tmp_view -> anim -> last -> img -> acl_format[j] = saved_label_format[j];
    tmp_view -> anim -> last -> img -> labels[j].position = saved_label_position[j];
  }
  field_unselect_all ();
/*
  tmp_fmol = tmp_field -> first_molecule;
  for (j=0; j<tmp_field -> molecules; j++)
  {
    g_debug ("FIELD_CANCEL:: Mol= %d, show= %d, show_id= %d", j, tmp_fmol -> show, tmp_fmol -> show_id);
    if (tmp_fmol -> next != NULL) tmp_fmol = tmp_fmol -> next;
  }
*/
  // restore selection if any from calc.c
  restore_ogl_selection (tmp_view);
  mol_num_label = NULL;
  tmp_field = NULL;
  tmp_view = NULL;
  if (! assist_init)
  {
    if (tmp_proj -> force_field[activef])
    {
      g_free (tmp_proj -> force_field[activef]);
      tmp_proj -> force_field[activef] = NULL;
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void on_assistant_cancel (GtkAssistant * assistant, gpointer data)

  \brief classical force field creation cancel assistant

  \param assistant the GtkAssistant sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_assistant_cancel (GtkAssistant * assistant, gpointer data)
{
  close_the_assistant (assistant);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean on_assistant_cancel_event (GtkWindow * assistant, gpointer data)

  \brief classical force field creation cancel event callback GTK4

  \param assistant the GtkWindow sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_assistant_cancel_event (GtkWindow * assistant, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_assistant_cancel_event (GtkWidget * assistant, GdkEvent * event, gpointer data)

  \brief classical force field creation cancel event callback GTK3

  \param assistant the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_assistant_cancel_event (GtkWidget * assistant, GdkEvent * event, gpointer data)
#endif
{
  on_assistant_cancel (((GtkAssistant *)assistant), data);
  return TRUE;
}

/*!
  \fn G_MODULE_EXPORT void on_assistant_close (GtkAssistant * assistant, gpointer data)

  \brief classical force field creation close assistant

  \param assistant the GtkAssistant sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_assistant_close (GtkAssistant * assistant, gpointer data)
{
  close_the_assistant (assistant);
}

/*!
  \fn G_MODULE_EXPORT gint on_assistant_go_forward (gint current_page, gpointer data)

  \brief classical force field assistant find the next page to go to

  \param current_page the current page id
  \param data the associated data pointer
*/
G_MODULE_EXPORT gint on_assistant_go_forward (gint current_page, gpointer data)
{
  int i, j;
  j = -1;
  switch (current_page)
  {
    case 0:
      j = 1;
      if (! tmp_field -> prepare_file[0])
      {
        j = MAXDATC+1;
        if (! tmp_field -> prepare_file[1]) j += MAXDATA+2;
      }
      return j;
      break;
    case MAXDATC+MAXDATA+2:
      return -1;
      break;
    default:
      if (current_page > MAXDATC-1 && tmp_field -> prepare_file[1])
      {
        for (i=current_page-1; i<MAXDATC+MAXDATA; i++)
        {
          if (tmp_field -> afp[i])
          {
            j = i+2;
            break;
          }
        }
      }
      else if (tmp_field -> prepare_file[0])
      {
        for (i=current_page; i<MAXDATC; i++)
        {
          if (tmp_field -> afp[i])
          {
            j = i+1;
            break;
          }
        }
      }
      if (j == -1) j = MAXDATC+MAXDATA+2;
      return j;
    break;
  }
}

/*!
  \fn G_MODULE_EXPORT void on_assistant_prepare (GtkAssistant * assistant, GtkWidget * page, gpointer data)

  \brief prepare classical force field assistant pages before display

  \param assistant the GtkAssistant sending the signal
  \param page the current page
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_assistant_prepare (GtkAssistant * assistant, GtkWidget * page, gpointer data)
{
  int i, j, k;
  i = gtk_assistant_get_current_page (assistant);
  field_unselect_all ();
  switch (i)
  {
    case 0:
      // if (is_the_widget_visible(preview_but)) hide_the_widgets (preview_but);
      break;
    case 1:
      // if (! is_the_widget_visible(preview_but)) show_the_widgets (preview_but);
      break;
    case MAXDATC+MAXDATA+2:
      break;
    default:
      j = field_object = i-2-MAXDATC;
      toviz.c = 1;
      switch (j)
      {
        case 0:
          tmp_fmol = tmp_field -> first_molecule;
          for (k=0; k<tmp_field -> molecules; k++)
          {
            if (tmp_fmol -> show) visualize_object (j, k, -1);
            if (tmp_fmol -> show_id) visualize_object (-1, k, -1);
            if (tmp_fmol -> next != NULL) tmp_fmol = tmp_fmol -> next;
          }
          break;
        default:
          if (j > -1 && j < SEXTERN) check_to_visualize_properties (j);
          break;
      }
      break;
  }
  init_default_shaders (tmp_view);
}

/*!
  \fn void hide_show_this_pages (int start, int end, int status, int delta)

  \brief classical force field assistant show / hide pages

  \param start starting page id
  \param end ending page id
  \param status 0 = hide, 1 = show
  \param delta delta from starting page
*/
void hide_show_this_pages (int start, int end, int status, int delta)
{
  int i;
  for (i=start; i<end; i++)
  {
    if (fibox[i+delta] != NULL)
    {
      if (GTK_IS_WIDGET(fibox[i+delta]))
      {
        gtk_widget_set_visible (fibox[i+delta], (status) ? tmp_field -> afp[i] : status);
      }
    }
  }
}

/*!
  \fn void remove_classical_assistant_pages ()

  \brief classical force field creation remove assistant pages
*/
void remove_classical_assistant_pages ()
{
  int i;
  GtkAssistant * assist = GTK_ASSISTANT(field_assistant);
  mol_num_label = destroy_this_widget (mol_num_label);
  for (i=MAXDATC+MAXDATA+1; i>-1; i--)
  {
    gtk_assistant_remove_page (assist, i+1);
    fibox[i] = destroy_this_widget(fibox[i]);
  }
}

/*!
  \fn G_MODULE_EXPORT void run_clean_field (GtkDialog * dial, gint response_id, gpointer data)

  \brief clean force field data - running the dialog

  \param dial the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_clean_field (GtkDialog * dial, gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_APPLY)
  {
    gtk_assistant_set_page_complete (GTK_ASSISTANT (field_assistant),
                                     gtk_assistant_get_nth_page(GTK_ASSISTANT (field_assistant), 0), FALSE);
    gtk_label_set_text (GTK_LABEL(field_i_lab[1]), "Initialize force field using: ");
    int i;
    for (i=0; i<2; i++)
    {
      combo_set_active (field_i_combo[i], -1);
      //tmp_field -> prepare_file[i] = TRUE;
      widget_set_sensitive (field_i_combo[i], TRUE);
      widget_set_sensitive (field_i_lab[i], TRUE);
      widget_set_sensitive (field_i_prep[i], FALSE);
      field_i_obj[i] = destroy_this_widget (field_i_obj[i]);
      field_i_obj[i] = stock_image (CANCEL);
      show_the_widgets (field_i_obj[i]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, field_i_box[i], field_i_obj[i], TRUE, TRUE, 25);
    }
    for (i=0; i<MAXDATC+MAXDATA+2; i++)
    {
      if (fibox[i] != NULL)
      {
        if (GTK_IS_WIDGET(fibox[i]))
        {
          gtk_widget_set_visible (fibox[i], FALSE);
        }
      }
    }
    hide_the_widgets (preview_but);
    for (i=0; i<19; i++)
    {
      button_set_status (ff_but[i], afp_init[i+MAXDATC+2]);
    }
    if (append_pages) remove_classical_assistant_pages ();
    button_set_status (data, FALSE);
    assist_init = FALSE;
  }
  destroy_this_dialog (dial);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void clean_field (GtkCheckButton * but, gpointer data)

  \brief clean force field data toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void clean_field (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void clean_field (GtkToggleButton * but, gpointer data)

  \brief clean force field data toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void clean_field (GtkToggleButton * but, gpointer data)
#endif
{
  if (button_get_status ((GtkWidget *)but))
  {
    GtkWidget * dial = dialog_cancel_apply ("Clean all force field parameter(s) ?", field_assistant, FALSE);
    GtkWidget * box = dialog_get_content_area (dial);
    gtk_box_set_homogeneous (GTK_BOX(box), FALSE);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, box, gtk_label_new ("Providing that the force field information was already saved once\n"
                                                                       "the previous data could still be retrieved by canceling the assistant."), FALSE, FALSE, 0);
    run_this_gtk_dialog (dial, G_CALLBACK(run_clean_field), but);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void select_field_action (GtkCheckButton * but, gpointer data)

  \brief select force field creation option toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_field_action (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void select_field_action (GtkToggleButton * but, gpointer data)

  \brief select force field creation option toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_field_action (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  tmp_field -> prepare_file[i] = button_get_status ((GtkWidget *)but);
  if (i == 1)
  {
    for (j=0; j<2; j++)
    {
      widget_set_sensitive (field_i_lab[j], (tmp_field -> prepare_file[i] && ! assist_init) ? TRUE : FALSE);
      widget_set_sensitive (field_i_combo[j], (tmp_field -> prepare_file[i] && ! assist_init) ? TRUE : FALSE);
    }
    hide_show_this_pages (MAXDATC-1, MAXDATC+MAXDATA, tmp_field -> prepare_file[1], 1);
  }
  else
  {
    hide_show_this_pages (0, MAXDATC, tmp_field -> prepare_file[0], 0);
  }
}

/*!
  \fn void add_classical_assistant_pages (int p)

  \brief classical force field assistant add pages

  \param p the target project id
*/
void add_classical_assistant_pages (int p)
{
  int i;
  gchar * info;
  GtkAssistant * assist = GTK_ASSISTANT(field_assistant);
  // Control file
  for (i=0; i<MAXDATC; i++)
  {
    fibox[i] = vbox_control (i);
    gtk_assistant_append_page (assist, fibox[i]);
    gtk_assistant_set_page_title (assist, fibox[i], g_strdup_printf ("%s", celemts[i]));
    gtk_assistant_set_page_type (assist, fibox[i], GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (assist, fibox[i], TRUE);
  }

  // Field file
  i = MAXDATC;
  fibox[i] = vbox_init (p);
  gtk_assistant_append_page (assist, fibox[i]);
  gtk_assistant_set_page_title (assist, fibox[i], "Select the component(s) of the force field");
  gtk_assistant_set_page_type (assist, fibox[i], GTK_ASSISTANT_PAGE_CONTENT);
  gtk_assistant_set_page_complete (assist, fibox[i], TRUE);
  for (i=MAXDATC+1; i<MAXDATC+MAXDATA+1; i++)
  {
    fibox[i] = vbox_field (i-MAXDATC-1);
    gtk_assistant_append_page (assist, fibox[i]);
    gtk_assistant_set_page_title (assist, fibox[i], g_strdup_printf ("%s(s)", felemts[i-MAXDATC]));
    gtk_assistant_set_page_type (assist, fibox[i], GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_complete (assist, fibox[i], TRUE);
  }
  i = MAXDATC+MAXDATA+1;
  fibox[i] = create_vbox (BSEP);

  info = g_strdup_printf ("<b>   Finalize the creation of the %s input file(s) now !</b>", (activef) ? "LAMMPS" : "DL-POLY");
  add_box_child_start (GTK_ORIENTATION_VERTICAL, fibox[i], markup_label(info, -1, -1, 0.5, 0.5), TRUE, TRUE, 100);
  g_free (info);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, fibox[i], markup_label("\n \t<b>Note: </b>You can re-open this assistant later if required to adjust your choices\n", -1, -1, 0.0, 0.5),
                       FALSE, FALSE, 0);
  gtk_assistant_append_page (assist, fibox[i]);
  gtk_assistant_set_page_title (assist, fibox[i], (activef) ? "Create the LAMMPS input file(s) now !" : "Create the DL-POLY input file(s) now !");
  gtk_assistant_set_page_type (assist, fibox[i], GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (assist, fibox[i], TRUE);
  gtk_assistant_update_buttons_state (assist);
  append_pages = TRUE;
}

gchar * field_init[3]={"Atomic species as field atom(s)",
                       "Atomic species and total coordination(s) as field atom(s)",
                       "Atomic species and partial coordination(s) as field atom(s)"};

// gboolean create_ff_structure (gpointer data)
/*!
  \fn void create_ff_structure (int ai, int type)

  \brief create the classical force field data structure and update widgets

  \param ai how to initialize the force field (0 = atomic species, 1 = total coordination, 2 = partial coordination)
  \param type the type of force field to create (0 = DL-POLY, 1 = LAMMPS)
*/
void create_ff_structure (int ai, int type)
{
  int i;
  //if (append_pages) remove_classical_assistant_pages ();
  tmp_proj -> force_field[activef] = create_force_field_data_structure (ai);
  tmp_proj -> force_field[activef] -> type = type;
  tmp_field = tmp_proj -> force_field[activef];
  for (i=0; i<MAXDATC+MAXDATA; i++) tmp_field -> afp[i] = afp_init[i];
  for (i=0; i<2; i++)
  {
    widget_set_sensitive (field_i_prep[i], TRUE);
    if (activef && ! i)
    {
      widget_set_sensitive (field_i_prep[i], FALSE);
      tmp_field -> prepare_file[i] = FALSE;
    }
    else
    {
      tmp_field -> prepare_file[i] = button_get_status (field_i_prep[i]);
    }
    widget_set_sensitive (field_i_lab[i], FALSE);
  }
  // for (i=13; i<19; i++) widget_set_sensitive (ff_but[i], set_nbd_but_sensitive (i));
  add_classical_assistant_pages (tmp_view -> proj);
  set_mol_num_label ();
  update_field_trees ();
  field_i_obj[1] = destroy_this_widget (field_i_obj[1]);
  field_i_obj[1] = stock_image (APPLY);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, field_i_box[1], field_i_obj[1], TRUE, TRUE, 25);
  gtk_label_set_text (GTK_LABEL(field_i_lab[1]), "Force field was initialized using: ");
  gtk_assistant_set_page_complete (GTK_ASSISTANT (field_assistant),
                                   gtk_assistant_get_nth_page(GTK_ASSISTANT (field_assistant), 0), TRUE);
  show_the_widgets (field_assistant);
  if (tmp_field -> md_opts[1] == 0.0) hide_the_widgets (extra_vbox[1]);
  hide_show_this_pages (0, MAXDATC, tmp_field -> prepare_file[0], 0);
  hide_show_this_pages (MAXDATC-1, MAXDATC+MAXDATA, tmp_field -> prepare_file[1], 1);
  // return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void changed_init_box (GtkComboBox * box, gpointer data)

  \brief classical force field change initialization parameter

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_init_box (GtkComboBox * box, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  j = combo_get_active ((GtkWidget *)box);
  if (j > -1)
  {
    switch (i)
    {
      case 0:
        tmp_field -> type = j;
        setup_this_force_field (j);
        if (tmp_field -> type == j)
        {
          field_i_obj[0] = destroy_this_widget (field_i_obj[0]);
          field_i_obj[0] = stock_image (APPLY);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, field_i_box[0], field_i_obj[0], TRUE, TRUE, 25);
          show_the_widgets (field_i_obj[0]);
        }
        else
        {
          combo_set_active ((GtkWidget *)box, -1);
        }
        break;
      case 1:
        if (tmp_field -> type > -1)
        {
          widget_set_sensitive (field_i_combo[0], FALSE);
          widget_set_sensitive (field_i_combo[1], FALSE);
          field_i_obj[1] = destroy_this_widget (field_i_obj[1]);
          field_i_obj[1] = gtk_spinner_new ();
          show_the_widgets (field_i_obj[1]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, field_i_box[1], field_i_obj[1], TRUE, TRUE, 25);
          show_the_widgets (field_i_obj[1]);
          gtk_spinner_start (GTK_SPINNER(field_i_obj[1]));
          // gdk_threads_add_idle (create_ff_structure, GINT_TO_POINTER(j));
          create_ff_structure (j, tmp_field -> type);
          assist_init = TRUE;
        }
        else
        {
          show_warning ("Please select the force field first !", field_assistant);
        }
        break;
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void show_force_field_preview (GtkButton * but, gpointer data)

  \brief show classical force field input files preview

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_force_field_preview (GtkButton * but, gpointer data)
{
  GtkWidget * preview = (activef) ? dialogmodal ("LAMMPS files preview", GTK_WINDOW(field_assistant)) : dialogmodal ("DL-POLY files preview", GTK_WINDOW(field_assistant));
  GtkWidget * notebook = gtk_notebook_new ();
  GtkWidget * scrollsets;
  GtkWidget * aview;
  gchar * ff_files[2][3] = {{"CONTROL", "FIELD" , "CONFIG"}, {"LAMMPS.IN", "LAMMPS.DATA", ""}};
  int num_files[2] = {3, 2};
  int i;
  for (i=0; i<num_files[activef]; i++)
  {
    if ((i==0 && tmp_field -> prepare_file[0]) || (i > 0 && tmp_field -> prepare_file[1]))
    {
      scrollsets = create_scroll (NULL, 700, 350, GTK_SHADOW_ETCHED_IN);
      aview = create_text_view (-1, -1, 0, 1, NULL, NULL, NULL);
      add_container_child (CONTAINER_SCR, scrollsets, aview);
      if (! activef)
      {
        switch (i)
        {
          case 0:
            print_dlp_control (gtk_text_view_get_buffer(GTK_TEXT_VIEW(aview)));
            break;
          case 1:
            print_dlp_field (gtk_text_view_get_buffer(GTK_TEXT_VIEW(aview)));
            break;
          case 2:
            print_dlp_config (gtk_text_view_get_buffer(GTK_TEXT_VIEW(aview)));
            break;
        }
      }
      else
      {
        switch (i)
        {
          case 0:

            break;
          case 1:
            print_lammps_atom_file (gtk_text_view_get_buffer(GTK_TEXT_VIEW(aview)));
            break;
        }
      }
      gtk_notebook_append_page (GTK_NOTEBOOK(notebook), scrollsets, gtk_label_new (ff_files[activef][i]));
    }
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, dialog_get_content_area (preview), notebook, FALSE, FALSE, 0);
  if (gtk_assistant_get_current_page (GTK_ASSISTANT (field_assistant)) > MAXDATC && tmp_field -> prepare_file[0])  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 1);
  run_this_gtk_dialog (preview, G_CALLBACK(run_destroy_dialog), NULL);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_on_assistant_apply (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief on classical force field assistant apply - running the dialog GTK4

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_assistant_apply (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_on_assistant_apply (GtkDialog * info, gint response_id, gpointer data)

  \brief on classical force field assistant apply - running the dialog GTK3

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_assistant_apply (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  int i;
  GtkTextBuffer * buffer;
  GtkTextIter bStart;
  GtkTextIter bEnd;
  GError * err = NULL;
  gboolean result = FALSE;
  gchar * ff_files[2][3] = {{"CONTROL", "FIELD" , "CONFIG"}, {"LAMMPS.IN", "LAMMPS.DATA", ""}};
  int num_files[2] = {3, 2};
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    gchar * direname = file_chooser_get_current_folder (chooser);
    if (direname != NULL)
    {
      gchar * filename;
      gchar * str;
      gboolean doit[3];
      for (i=0; i<num_files[activef]; i++)
      {
        if ((i==0 && tmp_field -> prepare_file[0]) || (i > 0 && tmp_field -> prepare_file[1]))
        {
          filename = g_strdup_printf ("%s/%s", direname, ff_files[activef][i]);
          if (g_file_test(filename, G_FILE_TEST_EXISTS))
          {
            str = g_strdup_printf ("%s file found in '%s'\nreplace existing %s file ?", ff_files[activef][i], direname, ff_files[activef][i]);
            doit[i] = ask_yes_no ("Replace file ?", str, GTK_MESSAGE_QUESTION, field_assistant);
            g_free (str);
          }
          else
          {
            doit[i] = TRUE;
          }
        }
        else
        {
          doit[i] = FALSE;
        }
      }
      for (i=0; i<num_files[activef]; i++)
      {
        buffer = add_buffer (NULL, NULL, NULL);
        if (doit[i])
        {
          filename = g_strdup_printf ("%s/%s", direname, ff_files[activef][i]);
          switch (i)
          {
            case 0:
              print_dlp_control (buffer);
              break;
            case 1:
              print_dlp_field (buffer);
              break;
            case 2:
              print_dlp_config (buffer);
              break;
          }
          gtk_text_buffer_get_start_iter (buffer, & bStart);
          gtk_text_buffer_get_end_iter (buffer, & bEnd);
          gchar * text = gtk_text_buffer_get_text (buffer, & bStart, & bEnd, FALSE);
          gtk_text_buffer_set_modified (buffer, FALSE);
          result = g_file_set_contents (filename, text, -1, & err);
          g_free (text);
          g_object_unref (buffer);
          if (! result && err)
          {
            show_error (g_strdup_printf ("Error while saving input file: %s\n Error: %s", filename, err -> message), 0, field_assistant);
            g_error_free (err);
          }
          g_free (filename);
        }
      }
      g_free (direname);
    }
  }
#ifdef GTK4
  destroy_this_native_dialog (info);
#else
  destroy_this_dialog (info);
#endif
}

/*!
  \fn void on_assistant_apply (GtkAssistant * assistant, gpointer data)

  \brief on classical force field assistant apply - creating the dialog

  \param assistant the GtkAssistant sending the signal
  \param data the associated data pointer
*/
void on_assistant_apply (GtkAssistant * assistant, gpointer data)
{
  gchar * text;
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  gchar * ff_type[2] = {"DL-POLY", "LAMMPS"};

  text = g_strdup_printf ("Saving %s input file(s)", ff_type[activef]);
  info = create_file_chooser (text,
                              GTK_WINDOW(assistant),
                              GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                              "Save");
  g_free (text);
#ifdef GTK4
  run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_on_assistant_apply), NULL);
#else
  run_this_gtk_dialog (info, G_CALLBACK(run_on_assistant_apply), NULL);
  // GTK3 Menu Action To Check
  gboolean i = (tmp_proj -> force_field[activef]) ? TRUE : FALSE;
  widget_set_sensitive (tmp_view -> color_styles[5], i);
  widget_set_sensitive (tmp_view -> color_styles[5+ATOM_MAPS], i);
#endif
}

/*!
  \fn void create_classical_force_field (int p, int f)

  \brief create classical force field prepare the assistant

  \param p the target project id
  \param f 0 = DL-POLY, 1 = LAMMPS
*/
void create_classical_force_field (int p, int f)
{
  field_assistant = gtk_assistant_new ();
  gtk_widget_set_size_request (field_assistant, 800, 600);
  int i, j, k;
  activef = f;
  field_color = TRUE;
  field_object = -1;
  selected_aspec = -1;
  opengl_project_changed (p);
  tmp_proj = get_project_by_id(p);
  tmp_coord = tmp_proj -> coord;
  tmp_view = tmp_proj -> modelgl;
  tmp_fmol = NULL;
  tmp_fmol = g_malloc0 (sizeof*tmp_fmol);
  gchar * field_type[2] = {"DL-POLY 4", "LAMMPS"};

  // Preparing data structure and pointers
  append_pages = FALSE;
  if (tmp_proj -> force_field[activef] == NULL)
  {
    assist_init = FALSE;
    tmp_proj -> force_field[activef] = g_malloc0 (sizeof*tmp_proj -> force_field[activef]);
    tmp_proj -> force_field[activef] -> type = -1;
    tmp_proj -> force_field[activef] -> atom_init = -1;
    for (i=0; i<2; i++) tmp_proj -> force_field[activef] -> prepare_file[i] = TRUE;
    if (activef) tmp_proj -> force_field[activef] -> prepare_file[0] = FALSE;
  }
  else
  {
    assist_init = TRUE;
    if (tmp_proj -> force_field[activef] -> type > -1) setup_this_force_field (tmp_proj -> force_field[activef] -> type);
    //tmp_field = duplicate_dlpoly_field (tmp_proj -> force_field[activef]);
  }
  tmp_field = tmp_proj -> force_field[activef];

  gtk_window_set_resizable (GTK_WINDOW (field_assistant), FALSE);
  gtk_window_set_modal (GTK_WINDOW (field_assistant), TRUE);
  gchar * info = g_strdup_printf ("%s calculation assistant", field_type[f]);
  gtk_window_set_title (GTK_WINDOW(field_assistant), info);
  g_free (info);

  GtkWidget * intro = create_vbox (BSEP);
  info = g_strdup_printf ("<b>   This assistant will help you to setup a %s \n"
                          "calculation using <i>%s</i> 3D model as starting point</b>", field_type[f], tmp_proj -> name);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, intro, markup_label(info, -1, -1, 0.5, 0.5), FALSE, FALSE, 50);
  g_free (info);

  gchar * i_titles[2][2] = {{"Force field: ", "Force field: "}, {"Initialize force field using: ","Force field was initialized using: "}};
  for (i=0; i<2; i++)
  {
    field_i_box[i] = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, intro, field_i_box[i], FALSE, FALSE, 0);
    j = (i == 0) ? tmp_field -> type + 1 : tmp_field -> atom_init + 1;
    k = (j) ? 1 : 0;
    field_i_lab[i] = markup_label(i_titles[i][k], 210, -1, 0.0, 0.5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, field_i_box[i], field_i_lab[i], FALSE, FALSE, 20);
    field_i_combo[i] = create_combo ();
    combo_set_active (field_i_combo[i], -1);
    switch (i)
    {
      case 0:
        for (j=0; j<N_FIELDS; j++) combo_text_append (field_i_combo[i], field_acro[j]);
        combo_set_active (field_i_combo[i], tmp_field -> type);
        break;
      case 1:
        for (j=0; j<3; j++) combo_text_append (field_i_combo[i], field_init[j]);
        combo_set_active (field_i_combo[i], tmp_field -> atom_init);
        break;
    }
    widget_set_sensitive (field_i_combo[i], ! k);
    g_signal_connect (G_OBJECT (field_i_combo[i]), "changed", G_CALLBACK(changed_init_box), GINT_TO_POINTER(i));
    field_i_fixed[i] = gtk_fixed_new ();
    gtk_widget_set_size_request (field_i_fixed[i], 410, -1);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, field_i_box[i], field_i_fixed[i], FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED(field_i_fixed[i]), field_i_combo[i], 0, 0);
    field_i_obj[i] = (k) ? stock_image (APPLY) : stock_image (CANCEL);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, field_i_box[i], field_i_obj[i], FALSE, FALSE, 25);
  }

  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, intro, hbox, FALSE, FALSE, 10);
  GtkWidget * but = check_button ("\tDelete the existing force field data and reset the parameters to the default values",
                                  -1, 40, FALSE, G_CALLBACK(clean_field), GINT_TO_POINTER(p));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 85);

  if (! f)
  {
    field_i_prep[0] = check_button ("\tPrepare the CONTROL file to describe the calculation",
                                    -1, 40, tmp_field -> prepare_file[0], G_CALLBACK(select_field_action), GINT_TO_POINTER(0));
    field_i_prep[1] = check_button ("\tPrepare the FIELD and CONFIG files to describe the force field and the atomistic model",
                                    -1, 40, tmp_field -> prepare_file[1], G_CALLBACK(select_field_action), GINT_TO_POINTER(1));
  }
  else
  {
    field_i_prep[0] = check_button ("\tPrepare the LAMMPS Input to describe the calculation",
                                    -1, 40, tmp_field -> prepare_file[0], G_CALLBACK(select_field_action), GINT_TO_POINTER(0));
    field_i_prep[1] = check_button ("\tPrepare the LAMMPS Atom file to describe the force field and the atomistic model",
                                    -1, 40, tmp_field -> prepare_file[1], G_CALLBACK(select_field_action), GINT_TO_POINTER(1));
  }
  for (i=0; i<2; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, intro, hbox, FALSE, FALSE, 0);
    widget_set_sensitive (field_i_prep[i], assist_init);
    if (activef) widget_set_sensitive (field_i_prep[0], 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, field_i_prep[i], FALSE, FALSE, 50);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, intro,
                       markup_label("\n \t<b>Note: </b>You can re-open this assistant later if required to adjust your choices\n",
                       -1, -1, 0.0, 0.5), FALSE, FALSE, 50);

  gtk_assistant_append_page (GTK_ASSISTANT (field_assistant), intro);
  info = g_strdup_printf ("%s calculation set-up", field_type[f]);
  gtk_assistant_set_page_title (GTK_ASSISTANT (field_assistant), intro, info);
  g_free (info);
  gtk_assistant_set_page_type (GTK_ASSISTANT (field_assistant), intro, GTK_ASSISTANT_PAGE_INTRO);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (field_assistant), intro, assist_init);
  preview_but = create_button ("Preview", IMG_STOCK, EDITF, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(show_force_field_preview), NULL);
  gtk_assistant_add_action_widget (GTK_ASSISTANT (field_assistant), preview_but);

  if (assist_init)
  {
    add_classical_assistant_pages (tmp_view -> proj);
    show_the_widgets (field_assistant);
    hide_show_this_pages (0, MAXDATC, tmp_field -> prepare_file[0], 0);
    hide_show_this_pages (MAXDATC-1, MAXDATC+MAXDATA, tmp_field -> prepare_file[1], 1);
  }
  else
  {
    show_the_widgets (field_assistant);
    hide_the_widgets (preview_but);
  }

  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (field_assistant), on_assistant_go_forward, NULL, NULL);
  g_signal_connect (G_OBJECT (field_assistant), "prepare", G_CALLBACK(on_assistant_prepare), NULL);
  g_signal_connect (G_OBJECT (field_assistant), "cancel", G_CALLBACK(on_assistant_cancel), NULL);
  g_signal_connect (G_OBJECT (field_assistant), "close", G_CALLBACK(on_assistant_close), NULL);
  g_signal_connect (G_OBJECT (field_assistant), "apply", G_CALLBACK(on_assistant_apply), NULL);
  add_gtk_close_event (field_assistant, G_CALLBACK(on_assistant_cancel_event), NULL);

  for (i=0; i<2; i++)
  {
    saved_label_format[i] = tmp_view -> anim -> last -> img -> acl_format[i];
    saved_label_position[i] = tmp_view -> anim -> last -> img -> labels[i].position;
    tmp_view -> anim -> last -> img -> labels[i].position = 0;
    tmp_view -> anim -> last -> img -> acl_format[i] = ID_IN_MOLECULE;
  }

  //g_debug ("Number of pages in the assitant: %d", gtk_assistant_get_n_pages (GTK_ASSISTANT (field_assistant)));
  //g_debug ("Active page is: %d", gtk_assistant_get_current_page (GTK_ASSISTANT (fwin)));
}
