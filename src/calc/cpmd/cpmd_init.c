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
* @file cpmd_init.c
* @short Functions to initialize the QM / QM-MM input file creation assistant
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cpmd_init.c'
*
* Contains:
*

 - The functions to initialize the QM / QM-MM input file creation assistant

*
* List of functions:

  G_MODULE_EXPORT gint on_qm_assistant_go_forward (gint current_page, gpointer data);

  gboolean go_for_it (int i, int j, gboolean print[2]);

  G_MODULE_EXPORT gboolean on_qm_assistant_cancel_event (GtkWindow * assistant, gpointer data);
  G_MODULE_EXPORT gboolean on_qm_assistant_cancel_event (GtkWidget * assistant, GdkEvent  * event, gpointer data);

  gchar * section_name (int p);
  gchar * page_name (int p);

  void print_all_sections (GtkTextBuffer * buf);
  void add_cpmd_pages ();
  void proj_unselect_all_atoms ();
  void create_qm_input_file (int c, int p, int s);

  G_MODULE_EXPORT void update_cpmd_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void update_cpmd_check (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void update_cpmd_check (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void changed_opt_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void update_calc_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void changed_calc_opt_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void update_calc_check (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void update_calc_check (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void changed_calc_box (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void changed_info (GtkTextBuffer * textbuf, gpointer data);
  G_MODULE_EXPORT void on_qm_assistant_cancel (GtkAssistant * assistant, gpointer data);
  G_MODULE_EXPORT void on_qm_assistant_close (GtkAssistant * assistant, gpointer data);
  G_MODULE_EXPORT void on_qm_assistant_prepare (GtkAssistant * assistant, GtkWidget * page, gpointer data);
  G_MODULE_EXPORT void show_qm_file_preview (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void run_saving_qm (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_saving_qm (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void on_qm_assistant_apply (GtkAssistant * assistant, gpointer data);

  GtkWidget * cpmd_box (GtkWidget * box, gchar * lab, int v_space, int h_space, int dim);
  GtkWidget * prepare_qm_option_box (int s);
  GtkWidget * calc_qm_option_box (int c);
  GtkWidget * info_box ();
  GtkWidget * section_box (int s);
  GtkWidget * qm_preview_box (int c, int s, int l);
  GtkWidget * vbox_cpmd (int s);

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"
#include "calc.h"
#include "cpmd.h"
#include "cp2k.h"

extern void print_cp2k (int f, GtkTextBuffer * buffer);
extern void add_cp2k_pages ();

extern ColRGBA init_color (int id, int numid);
extern GtkWidget * electron_box;
extern GtkWidget * thermo_box ();
extern void field_unselect_all ();
extern void print_the_section (int s, int p, GtkTextBuffer * buffer);
extern void init_thermos ();
extern gboolean are_all_atoms_thermostated ();
extern void create_selection_combo (int id, int num, int type, GCallback handler);
extern G_MODULE_EXPORT void atom_button (GtkButton * but, gpointer data);
extern GtkWidget * restart_box ();
extern void restore_ogl_selection (glwin * view);

gchar * co_type[2] = {"CPMD", "CP2K"};

cpmd * tmp_cpmd;

gchar * cpmd_elements[MAXDATAQM] = {"INFO",
                                    "CPMD",
                                    "DFT",
                                    "VDW",
                                    "PROP",
                                    "SYSTEM",
                                    "ATOMS"};

gchar * cdescr[MAXDATAQM] = {"provides an informal description of the system and the calculation to be performed",
                             "provides the general control parameters for the calculation to be performed",
                             "provides the exchange and correlation functional (DFT) parameters",
                             "describes the implementation of the van der Waals interactions",
                             "provides details about the physical properties to be calculated",
                             "describes the symmetry and periodicity of the system",
                             "describes the atomic species, pseudo-potentials and coordinates"};

double default_cpmd_options[17] = {400.0, 0.0, 0.0, 4.0, 0.000001, 0.0, 0.0, 0.0, 0.0, 0.0, 70.0, 0.0, -1.0, 0.0, 0.0, 1.0, 1.0};

gchar * default_opts[MAXDATAQM-1][NSECOP]= {{"Fictitious electronic mass:", "Local Spin Density", "Van der Walls interactions", " ", " ", " "},
                                            {"DFT functional:", "Density cutoff<sup>*</sup>:", " ", " ", " ", " "},
                                            {" ", " ", " ", " ", " ", " "},
                                            {" ", " ", " ", " ", " ", " "},
                                            {"Use Angströms (default a.u.)", "Lattice:", "Symmetry:", "Parameters:", "Angles:", "Cutoff for the plane wave basis:"},
                                            {"Use constraints", "Fix:", "Use dummy atoms:", "Atom type:", "Maximum angular momentum <i>l</i>:", "Local angular momentum <i>l</i>:"}};

// 0 = None, 1 = Entry, 2 = Combo, 3 = yes/no
int default_opts_type[MAXDATAQM-1][NSECOP] = {{1, 3, 3, 0, 0, 0},
                                              {2, 1, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0},
                                              {3, 2, 2, 2 ,2, 1},
                                              {3, 0, 3, 2, 2, 2}};
// NDF is the max by far
int defaut_num[9]={NDFT, 2, NSYM, 2, 2, 3, -1, 3, 3};

gchar * default_keywords[9][NDFT] = {{"SONLY", "LDA", "BONLY", "BP", "BLYP", "XLYP", "PW91", "PBE", "PBES", "REVPBE", "HCTH", "OPTX", "OLYP", "TPSS", "PBE0", "B1LYP", "B3LYP", "X3LYP", "HSE06"},
                                     {" ", " VECTORS", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {" ", " ABSOLUTE", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {" ", " DEGREE", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {"ALL", "ATOMS", "COORDINATES", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {"S", "P", "D", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                     {"S", "P", "D", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}};

gchar * default_text[9][NDFT] = {{"Slater exchange only", "Local Density Approximation","Becke 88", "Becke + Perdew", "Becke + Lee-Yang-Parr", "Extended B88+PW91+LYP88", "Perdew + Wang 91",
                                  "Perdew + Burke-Ernzerhof", "PBE revised for solids", "Revised - PBE", "Hamprecht-Cohen-Tozer-Handy", "Optimized Becke 88", "Handy-Cohen + LYP",
                                  "Tao-Perdew-Staroverov-Scuseria", "Parameter-free PBE", "Becke one-parameter hybrid + LYP", "Becke three-parameters hybrid + LYP", "Extend hybrid + LYP",  "Heyd-Scuseria-Ernzerhof 06"},
                                 {"Box parameters (a,b,c and α,β,γ)", "Lattice vectors", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                 {"Isolated", "Cubic", "Face centered cubic (FCC)", "Body centered cubic (BCC)", "Hexagonal", "Trigonal",
                                  "Tetragonal", "Body centered tetragonal (BCT)", "Orthorombic", "Monoclinic", "Triclinic",  " ", " ", " ", " ", " ", " ", " ", " "},
                                 {"Default (a, b/a, c/a)", "Absolute (a, b, c)", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                 {"Default (cos α, cos β, cos γ)", "Degrees (α, β, γ)", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                 {"All atoms", "Some atoms", "Some coordinates", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                 {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                 {"s", "p", "d", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
                                 {"s", "p", "d", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}};

double default_calc_options[24] = {0.00001, 0.0, 10000.0, 3.0,
                                   0.0005, 0.0, 10000.0, 3.0,
                                   10000.0, 5.0, 0.0, 0.0, 0.9, 0.0, 0.9,
                                   10000.0, 5.0, 0.0, 0.0, 0.9,
                                   50.0, 0.0, 0.0, 0.0};

gchar * calc_opts[NCPMDCALC][NOPTPC]={{"Convergence criteria<sup>*</sup>:", "Optimizer:", "Max steps:", "Integration step:", " ", " ", " "},
                                      {"Convergence criteria<sup>*</sup>:", "Optimizer:", "Max steps:", "Integration step:", " ", " ", " "},
                                      {"Max MD steps: ", "Time step:", "Barostat:", "Ions", "Factor:", "Fictitious electrons", "Factor:"},
                                      {"Max MD steps: ", "Time step:", "Barostat:", "Ions", "Factor:", " ", " "},
                                      {"Number of unoccupied states:", "3D visualization<sup>*</sup>", "Number of objects<sup>**</sup>:", " ", " ", " ", " "},
                                      {"Property to compute:", " ", " ", " ", " ", " ", " "}};

int default_type[NCPMDCALC][NOPTPC]={{1, 2, 1, 1, 0, 0, 0},
                                     {1, 2, 1, 1, 0, 0, 0},
                                     {1, 1, 2, 3, 1, 3, 1},
                                     {1, 1, 2, 3, 1, 0, 0},
                                     {1, 3, 1, 0, 0, 0, 0},
                                     {2, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0}};

gchar * calc_kw[NCPMDCALC]= {"OPTIMIZE WAVEFUNCTION",
                             "OPTIMIZE GEOMETRY",
                             "MOLECULAR DYNAMICS",
                             "MOLECULAR DYNAMICS BO",
                             "KOHN-SHAM ENERGIES",
                             "VIBRATIONAL ANALYSIS",
                             "PROPERTIES"};

gchar * calc_ds[NCPMDCALC]= {"Wavefunction optimization",
                             "Geometry optimization",
                             "CPMD Molecular Dynamics",
                             "Born-Oppenheimer Molecular Dynamics",
                             "Kohn-Sham eigen values",
                             "Vibrational analysis",
                             "Calculation of physical properties"};

int calc_box_num[NCACOMBO]={2, 2, 3, 3, 3};

gchar * calc_box_name[NCACOMBO][3] = {{"Preconditioned Gradient", "Direct Inversion of Iterative Subspace", " "},
                                      {"Quasi-Newton method", "Direct Inversion of Iterative Subspace", " "},
                                      {"None", "Parrinello-Rahman", "Parrinello-Rahman + NPT"},
                                      {"None", "Parrinello-Rahman", "Parrinello-Rahman + NPT"},
                                      {"Finite differences of first derivatives", "Linear response of ionic displacement", "Precalculated Hessian"}};

gchar * calc_box_keys[NCACOMBO][3] = {{"PCG", "ODIIS", " "},
                                      {"BFGS", "GDIIS", " "},
                                      {" ", "PARRINELLO-RAHMAN", "PARRINELLO-RAHMAN NPT"},
                                      {" ", "PARRINELLO-RAHMAN", "PARRINELLO-RAHMAN NPT"},
                                      {"FD", "LR", "IN"}};

gchar * rest_kw[2] = {"RANDOM", "ATOMS"};
gchar * rest_opts[3] = {"Random", "Atomic pseudo wavefunctions", "Use a RESTART.* file"};
gchar * nosetype[3] = {"Gobal", "Local", "Molecule"};
gchar * nosekey[2] = {" ", "LOCAL"};
gchar * thermo_name[2][5] = {{"None", "Controlled", "Nosé-Hoover chains", " ", " "},
                             {"None", "Adaptive Langevin", "Canonical sampling through velocity rescaling", "GLE", "Nosé-Hoover chains"}};
int num_thermo[2] = {3, 5};
int type_thermo[2] = {2, 2};
gchar * termoke[2] = {"TEMPCONTROL", "NOSE"};

GtkWidget * qm_preview_but;
GtkWidget * calc_combo;
gboolean qm_assist_init;
int qm_saved_label_format[2];
GtkWidget * calc_label;
GtkWidget * latbox;
GtkWidget * but_at[2];
GtkWidget * spatbox;
GtkWidget * ppbox[2];
GtkWidget * calc_box[NCPMDCALC];
GtkWidget * qm_option_box[MAXDATAQM-1];
gboolean is_cpmd;

/*!
  \fn GtkWidget * cpmd_box (GtkWidget * box, gchar * lab, int v_space, int h_space, int dim)

  \brief prepare a labelled box widget for the CPMD input creation assistant

  \param box the GtkWidget sending the signal
  \param lab the label
  \param v_space vertical spacing
  \param h_space horizontal spacing
  \param dim label width
*/
GtkWidget * cpmd_box (GtkWidget * box, gchar * lab, int v_space, int h_space, int dim)
{
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, v_space);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(lab, dim, 30, 0.0, 0.5), FALSE, FALSE, h_space);
  return hbox;
}

/*!
  \fn void print_all_sections (GtkTextBuffer * buf)

  \brief print all sections of the CPMD input file

  \param buf the GtkTextBuffer to print into
*/
void print_all_sections (GtkTextBuffer * buf)
{
  int i;
  for (i=0; i<MAXDATAQM+2; i++)
  {
    if (i != 5 && i != 6)
    {
      if (buf == NULL)
      {
        print_the_section (i, 0, qmbuffer[i]);
      }
      else if (i == 0 || i > 2)
      {
        print_the_section (i, 1, buf);
      }
    }
    else if (i == 5 && (int)tmp_cpmd -> default_opts[2])
    {
      if (buf == NULL)
      {
        print_the_section (i, 0, qmbuffer[i]);
      }
      else
      {
        print_the_section (i, 1, buf);
      }
    }
    else if (i == 6 && tmp_cpmd -> calc_type == 4)
    {
      if (buf == NULL)
      {
        print_the_section (i, 0, qmbuffer[i]);
      }
      else
      {
        print_the_section (i, 1, buf);
      }
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void update_cpmd_parameter (GtkEntry * res, gpointer data)

  \brief update CPMD QM option value entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cpmd_parameter (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  tmp_cpmd -> default_opts[i] = v;
  if (i == DEFGC)
  {
    update_entry_long_double (res, tmp_cpmd -> default_opts[i]);
  }
  else
  {
    update_entry_double (res, tmp_cpmd -> default_opts[i]);
  }
  print_all_sections (NULL);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void update_cpmd_check (GtkCheckButton * but, gpointer data)

  \brief update CPMD QM option toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cpmd_check (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void update_cpmd_check (GtkToggleButton * but, gpointer data)

  \brief update CPMD QM option toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_cpmd_check (GtkToggleButton * but, gpointer data)
#endif
{
  int i = GPOINTER_TO_INT(data);
  gboolean j = button_get_status ((GtkWidget *)but);
  if (i == DEFCO || i == DEFDU)
  {
    widget_set_sensitive (but_at[(i == DEFCO) ? 0 : 1], j);
  }
  tmp_cpmd -> default_opts[i] = (double) j;
  print_all_sections (NULL);
}

/*!
  \fn G_MODULE_EXPORT void changed_opt_box (GtkComboBox * box, gpointer data)

  \brief change CPMD QM option

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_opt_box (GtkComboBox * box, gpointer data)
{
  int i, j, k;
  j = GPOINTER_TO_INT(data);
  i = combo_get_active ((GtkWidget *)box);
  if (j != DEFLM && j != DEFLO)
  {
    if (i != (int)tmp_cpmd -> default_opts[j])
    {
      tmp_cpmd -> default_opts[j] = (double)i;
      if (j == DEFSP)
      {
        k = (int)tmp_cpmd -> default_opts[j];
        combo_set_active (ppbox[0], tmp_cpmd -> pp[k][0]);
        combo_set_active (ppbox[1], tmp_cpmd -> pp[k][1]);
      }
      else if (j == DEFFI)
      {
        tmp_cpmd -> default_opts[DEFCO] = tmp_cpmd -> default_opts[j] + 1;
        if (tmp_cpmd -> fixlist != NULL)
        {
          g_free (tmp_cpmd -> fixlist);
          tmp_cpmd -> fixlist = NULL;
          if (tmp_cpmd -> fixcoord != NULL)
          {
            g_free (tmp_cpmd -> fixcoord);
            tmp_cpmd -> fixcoord = NULL;
          }
        }
        tmp_cpmd -> fixat = 0;
        widget_set_sensitive (sel_but[1], i);
      }
    }
  }
  else
  {
    if (i != tmp_cpmd -> pp[(int)tmp_cpmd -> default_opts[DEFSP]][j-DEFLM])
    {
      tmp_cpmd -> pp[(int)tmp_cpmd -> default_opts[DEFSP]][j-DEFLM] = i;
    }
  }
  if (j == DEFVE)
  {
    widget_set_sensitive (latbox, ! (int)tmp_cpmd -> default_opts[j]);
  }
  print_all_sections (NULL);
}

/*!
  \fn GtkWidget * prepare_qm_option_box (int s)

  \brief CPM input file creation prepare section general options widgets

  \param s the section id
*/
GtkWidget * prepare_qm_option_box (int s)
{
  int i, j, k, l;
  GtkWidget * hbox;
  GtkWidget * widg;
  gchar * str;
  gchar * atom_but[2] = {"Configure constraints", "Configure dummy atoms"};
  GtkWidget * vbox = create_vbox (BSEP);
  for (i=0; i<NSECOP; i++)
  {
    if (default_opts_type[s][i] > 0)
    {
      ident ++;
      if (ident == DEFAB || ident == DEFDG || ident == DEFSY)
      {
        hbox = cpmd_box (latbox, default_opts[s][i], 5, 20, (s == 1) ? 150 : 220);
      }
      else if (ident == DEFCO || ident == DEFDU)
      {
        hbox = cpmd_box (vbox, default_opts[s][i], 20, 20, 180);
      }
      else if (ident == DEFLM || ident == DEFLO)
      {
        hbox = cpmd_box (spatbox, default_opts[s][i], 5, 20, 220);
      }
      else
      {
        hbox = cpmd_box (vbox, default_opts[s][i], (ident == DEFSP) ? 20 : 5, 5, (s == 1) ? 150 : 220);
      }
      j = ident;
      switch (default_opts_type[s][i])
      {
        case 1:
          widg = create_entry (G_CALLBACK(update_cpmd_parameter), 100, 15, FALSE, GINT_TO_POINTER(j));
          if (j == DEFGC)
          {
            update_entry_long_double (GTK_ENTRY(widg), tmp_cpmd -> default_opts[j]);
          }
          else
          {
            update_entry_double (GTK_ENTRY(widg), tmp_cpmd -> default_opts[j]);
          }
          break;
        case 2:
          icomb ++;
          if (ident == DEFLM || ident == DEFLO)
          {
            ppbox[ident - DEFLM] = create_combo ();
          }
          else
          {
            widg = create_combo ();
          }
          l = (ident == DEFSP) ? qm_proj -> nspec : defaut_num[icomb];
          for (k=0; k<l; k++)
          {
            if (icomb == 0)
            {
              str = g_strdup_printf ("%s (%s)", default_text[0][k], default_keywords[0][k]);
            }
            else if (ident == DEFSP)
            {
              str = g_strdup_printf ("%s atom(s)", qm_proj -> chemistry -> label[k]);
            }
            else
            {
              str = g_strdup_printf ("%s", default_text[icomb][k]);
            }
            if (ident == DEFLM || ident == DEFLO)
            {
              combo_text_append (ppbox[ident - DEFLM], str);
            }
            else
            {
              combo_text_append (widg, str);
            }
            g_free (str);
          }
          if (ident == DEFLM || ident == DEFLO)
          {
            combo_set_active (ppbox[ident - DEFLM], tmp_cpmd -> pp[(int)tmp_cpmd -> default_opts[DEFSP]][ident - DEFLM]);
            g_signal_connect (G_OBJECT (ppbox[ident - DEFLM]), "changed", G_CALLBACK(changed_opt_box), GINT_TO_POINTER(j));
          }
          else
          {
            combo_set_active (widg, (int)tmp_cpmd -> default_opts[j]);
            g_signal_connect (G_OBJECT (widg), "changed", G_CALLBACK(changed_opt_box), GINT_TO_POINTER(j));
          }
          break;
        case 3:
          widg = check_button (NULL, -1, -1, tmp_cpmd -> default_opts[j], G_CALLBACK(update_cpmd_check), GINT_TO_POINTER(j));
          break;
      }
      if (ident == DEFLM || ident == DEFLO)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ppbox[ident - DEFLM], FALSE, FALSE, 0);
      }
      else
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 0);
      }
      if (j==DEFEM || j==DEFGC || j==DEFCU)
      {
        if (j==DEFEM || j==DEFGC)
        {
          widg = gtk_label_new ("a.u.");
        }
        else
        {
          widg = gtk_label_new ("Ry");
        }
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, 5);
      }
      else if (j == DEFVE)
      {
        latbox = create_vbox (BSEP);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, latbox, FALSE, FALSE, 15);
        widget_set_sensitive (latbox, ! (int)tmp_cpmd -> default_opts[DEFVE]);
      }
      else if (j == DEFCO || j == DEFDU)
      {
        k = (j==DEFCO) ? 0 : 1;
        but_at[k] = create_button (atom_but[k], IMG_NONE, NULL, 175, -1, GTK_RELIEF_NORMAL, G_CALLBACK(atom_button), GINT_TO_POINTER(j));
        widget_set_sensitive (but_at[k], (int)tmp_cpmd -> default_opts[j]);
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but_at[k], FALSE, FALSE, 20);
        if (j == DEFCO)
        {
          ident ++;
          icomb ++;
        }
      }
      else if (j == DEFSP)
      {
        spatbox = create_vbox (BSEP);
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, spatbox, FALSE, FALSE, 15);
      }
    }
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void update_calc_parameter (GtkEntry * res, gpointer data)

  \brief update CPMD calculation option value entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_calc_parameter (GtkEntry * res, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (i==STEPO || i==STEPG || i==STEPC || i==STEPB || i==KSUNO || i==NBAND)
  {
    j = (int) v;
    tmp_cpmd -> calc_opts[i] = (double) j;
    update_entry_int (res, j);
  }
  else
  {
    tmp_cpmd -> calc_opts[i] = v;
    if (i == CONVO || i == CONVG)
    {
      update_entry_long_double (res, tmp_cpmd -> calc_opts[i]);
    }
    else
    {
      update_entry_double (res, tmp_cpmd -> calc_opts[i]);
    }
  }
  for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

/*!
  \fn G_MODULE_EXPORT void changed_calc_opt_box (GtkComboBox * box, gpointer data)

  \brief change CPMD calculation option

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_calc_opt_box (GtkComboBox * box, gpointer data)
{
  int i, j;
  j = GPOINTER_TO_INT(data);
  i = combo_get_active ((GtkWidget *)box);
  if (i != tmp_cpmd -> calc_opts[j])
  {
    tmp_cpmd -> calc_opts[j] = i;
    for (j=1; j<4; j++) print_the_section (j, 0, qmbuffer[j]);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void update_calc_check (GtkCheckButton * but, gpointer data)

  \brief update CPMD calculation option toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_calc_check (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void update_calc_check (GtkToggleButton * but, gpointer data)

  \brief update CPMD calculation option toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_calc_check (GtkToggleButton * but, gpointer data)
#endif
{
  int i = GPOINTER_TO_INT(data);
  tmp_cpmd -> calc_opts[i] = (double) button_get_status ((GtkWidget *)but);
  for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
}

/*!
  \fn GtkWidget * calc_qm_option_box (int c)

  \brief CPMD input assistant prepare the calculation option widgets

  \param c the calculation id
*/
GtkWidget * calc_qm_option_box (int c)
{
  int i, j, k, l;
  GtkWidget * hbox;
  GtkWidget * widg;
  gchar * str;
  GtkWidget * vbox = create_vbox (BSEP);
  for (i=0; i<NOPTPC; i++)
  {
    if (default_type[c][i] > 0)
    {
      idopt ++;
      l = 0;
      if (idopt == ANNIC || idopt == ANNIB)
      {
        hbox = cpmd_box (vbox, "Annealing:", 0, 5, 220);
      }
      if (idopt == ANNIC || idopt == ANNEC || idopt == ANNIB)
      {
        hbox = cpmd_box (vbox, calc_opts[c][i], 0, 25, 120);
        l = 30;
      }
      else if (idopt == AFAIC || idopt == AFAEC || idopt == AFAIB)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new(calc_opts[c][i]), FALSE, FALSE, 10);
        l = 30;
      }
      else
      {
        hbox = cpmd_box (vbox, calc_opts[c][i], 0, 5, 220);
      }
      j = idopt;
      switch (default_type[c][i])
      {
        case 1:
          widg = create_entry (G_CALLBACK(update_calc_parameter), 100, 15, FALSE, GINT_TO_POINTER(j));
          if (j==STEPO || j==STEPG || j==STEPC || j==STEPB || j==KSUNO || j==NBAND)
          {
            update_entry_int (GTK_ENTRY(widg), (int) tmp_cpmd -> calc_opts[j]);
          }
          else
          {
            update_entry_double (GTK_ENTRY(widg), tmp_cpmd -> calc_opts[j]);
          }
          break;
        case 2:
          icalc ++;
          widg = create_combo ();
          for (k=0; k<calc_box_num[icalc]; k++)
          {
            str = g_strdup_printf ("%s", calc_box_name[icalc][k]);
            combo_text_append (widg, str);
            g_free (str);
          }
          combo_set_active (widg, (int)tmp_cpmd -> calc_opts[j]);
          g_signal_connect (G_OBJECT (widg), "changed", G_CALLBACK(changed_calc_opt_box), GINT_TO_POINTER(j));
          break;
        case 3:
          widg = check_button (NULL, -1, -1, tmp_cpmd -> calc_opts[j], G_CALLBACK(update_calc_check), GINT_TO_POINTER(j));
          break;
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, widg, FALSE, FALSE, l);
      if (j==TSTPO || j==TSTPG || j==TSTPC || j==TSTPB)
      {
        add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new ("a.u."), FALSE, FALSE, 5);
      }
      else if (j==NBAND)
      {
        gchar * ksout = "\t * electronic density(ies) or/and wavefunction(s)\n"
                        "\t ** as many index(es) to be supplied on the next line:\n"
                        "\t\t &gt; 0 for electronic density\n\t\t &lt; 0 for wavefunction";
        add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(ksout, -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
      }
    }
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void changed_calc_box (GtkComboBox * box, gpointer data)

  \brief change CPMD calculation type

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_calc_box (GtkComboBox * box, gpointer data)
{
  int i;
  i = combo_get_active ((GtkWidget *)box);
  if (i != tmp_cpmd -> calc_type)
  {
    gtk_label_set_text (GTK_LABEL(calc_label), g_strdup_printf ("<u>%s option(s)</u>", calc_ds[i]));
    gtk_label_set_use_markup (GTK_LABEL(calc_label), TRUE);
    hide_the_widgets (calc_box[tmp_cpmd -> calc_type]);
    show_the_widgets (calc_box[i]);
    tmp_cpmd -> calc_type = i;
    if (tmp_cpmd -> calc_type != 2)
    {
      hide_the_widgets (electron_box);
    }
    else
    {
      show_the_widgets (electron_box);
    }
    for (i=1; i<4; i++) print_the_section (i, 0, qmbuffer[i]);
  }
}

/*!
  \fn G_MODULE_EXPORT void changed_info (GtkTextBuffer * textbuf, gpointer data)

  \brief update CPMD input file preview

  \param textbuf the GtkTextBuffer to print into
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_info (GtkTextBuffer * textbuf, gpointer data)
{
  GtkTextIter bStart;
  GtkTextIter bEnd;
  gtk_text_buffer_get_start_iter (textbuf, & bStart);
  gtk_text_buffer_get_end_iter (textbuf, & bEnd);
  if (tmp_cpmd -> info != NULL) g_free (tmp_cpmd -> info);
  tmp_cpmd -> info = g_strdup_printf ("%s", gtk_text_buffer_get_text (textbuf, & bStart, & bEnd, FALSE));
  print_the_section (0, 0, qmbuffer[0]);
}

/*!
  \fn GtkWidget * info_box ()

  \brief create CPMD input file information widgets
*/
GtkWidget * info_box ()
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox = create_hbox (0);
  GtkWidget * scrollsets = create_scroll (NULL, 355, 250, GTK_SHADOW_ETCHED_IN);
  GtkWidget * aview = create_text_view (-1, -1, 1, 0, G_CALLBACK(changed_info), NULL, NULL);
  if (tmp_cpmd -> info != NULL) print_info (tmp_cpmd -> info, NULL, gtk_text_view_get_buffer(GTK_TEXT_VIEW(aview)));
  add_container_child (CONTAINER_SCR, scrollsets, aview);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, scrollsets, FALSE, FALSE, 85);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);

  return vbox;
}

/*!
  \fn GtkWidget * section_box (int s)

  \brief create CPMD section box

  \param s the section id
*/
GtkWidget * section_box (int s)
{
  int i;
  gchar * str;
  GtkWidget * hbox;
  GtkWidget * vbox = create_vbox (BSEP);
  if (s == 0)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, info_box(), FALSE, FALSE, 50);
  }
  else if (s == 1)
  {
    // CPMD Calculation
    /* Calc type combo_box: */
    idopt = -1;
    icalc = -1;
    ident = -1;
    icomb = -1;
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>Calculation to be performed:</b>", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
    calc_combo = create_combo ();
    for (i=0; i<NCPMDCALC; i++) combo_text_append (calc_combo, calc_ds[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, calc_combo, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);

    /* Calc related options: */
    str = g_strdup_printf ("<u>%s option(s)</u>", calc_ds[tmp_cpmd -> calc_type]);
    calc_label = markup_label(str, -1, -1, 0.0, 0.5);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, calc_label, FALSE, FALSE, 10);
    for (i=0; i<NCPMDCALC; i++)
    {
      calc_box[i] = calc_qm_option_box (i);
      gtk_widget_set_size_request (calc_box[i], -1, 200);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, calc_box[i], FALSE, FALSE, 0);
    }
    g_signal_connect (G_OBJECT (calc_combo), "changed", G_CALLBACK(changed_calc_box), NULL);
    combo_set_active (calc_combo, tmp_cpmd -> calc_type);
  }
  else if (s == 2)
  {
    // CPMD - thermostat(s)
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, thermo_box(), FALSE, FALSE, 5);
  }
  else if (s == 3)
  {
    // CPMD - restart
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, restart_box(), FALSE, FALSE, 0);
  }

  /* General CPMD section options: */
  if (s > 0 && s != 2 && s != 3)
  {
    i = (s == 1) ? s : s - 2;
    str = g_strdup_printf ("<u>General %s section option(s)</u>",cpmd_elements[i]);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, 525, -1, 0.0, 0.5), FALSE, FALSE, 10);
    g_free (str);
    qm_option_box[i-1] = prepare_qm_option_box (i-1);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, qm_option_box[i-1], FALSE, FALSE, 0);
  }
  return vbox;
}

/*!
  \fn GtkWidget * qm_preview_box (int c, int s, int l)

  \brief prepare preview section widgets

  \param c 0 = CPMD, 1 = CP2K
  \param s section id
  \param l spacing
*/
GtkWidget * qm_preview_box (int c, int s, int l)
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * scrollsets = create_scroll (NULL, 250, (c) ? 280 : 410, GTK_SHADOW_ETCHED_IN);
  GtkWidget * aview = create_text_view (-1, -1, 0, 1, NULL, NULL, NULL);
  qmbuffer[s] = gtk_text_view_get_buffer (GTK_TEXT_VIEW(aview));
  add_container_child (CONTAINER_SCR, scrollsets, aview);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, scrollsets, FALSE, FALSE, l);
  if (c == 0)
  {
    print_the_section (s, 0, qmbuffer[s]);
  }
  else
  {
    print_cp2k (s, qmbuffer[s]);
  }
  return vbox;
}

/*!
  \fn gchar * section_name (int p)

  \brief get CPMD input creation section name

  \param p the section id
*/
gchar * section_name (int p)
{
  if (p > 0 && p < 4)
  {
    return g_strdup_printf ("Details of the <b>%s</b> section that %s", cpmd_elements[1], cdescr[1]);
  }
  else if (p == 0)
  {
    return g_strdup_printf ("Details of the <b>%s</b> section that %s", cpmd_elements[p], cdescr[p]);
  }
  else
  {
    return g_strdup_printf ("Details of the <b>%s</b> section that %s", cpmd_elements[p-2], cdescr[p-2]);
  }
}

/*!
  \fn GtkWidget * vbox_cpmd (int s)

  \brief create CPMD input creation section widgets

  \param s the section id
*/
GtkWidget * vbox_cpmd (int s)
{
  GtkWidget * vbox;
  GtkWidget * hbox;
  vbox = create_vbox (BSEP);
  hbox = create_hbox (0);
  if (s < 2 || s > 3)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(section_name(s), -1, 20, 0.0, 0.5), FALSE, FALSE, 20);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, (s < 2 ||s > 3) ? 5 : 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, section_box(s), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, qm_preview_box (0, s, (s < 2 ||s > 3) ? 0 : 65), FALSE, FALSE, 0);
  return vbox;
}

/*!
  \fn gchar * page_name (int p)

  \brief get CPMD input creation assistant page name

  \param p the page id
*/
gchar * page_name (int p)
{
  if (p > 0 && p <4)
  {
    if (p == 1)
    {
      return g_strdup_printf ("The %s section - Calculation options", cpmd_elements[1]);
    }
    else if (p == 2)
    {
      return g_strdup_printf ("The %s section - Thermostat options", cpmd_elements[1]);
    }
    else
    {
      return g_strdup_printf ("The %s section - RESTART options", cpmd_elements[1]);
    }
  }
  else if (p == 0)
  {
    return g_strdup_printf ("The %s section", cpmd_elements[p]);
  }
  else
  {
    return g_strdup_printf ("The %s section", cpmd_elements[p-2]);
  }
}

/*!
  \fn void add_cpmd_pages ()

  \brief add pages to the CPMD input file creation assistant
*/
void add_cpmd_pages ()
{
  int i;
  gchar * info;
  GtkAssistant * assist = GTK_ASSISTANT(qm_assistant);
  GtkWidget * page;
  for (i=0; i<MAXDATAQM+2; i++)
  {
    page = vbox_cpmd (i);
    gtk_assistant_append_page (assist, page);
    gtk_assistant_set_page_title (assist, page, page_name(i));
    gtk_assistant_set_page_type (assist, page, GTK_ASSISTANT_PAGE_CONTENT);
    if (i != 3)
    {
      gtk_assistant_set_page_complete (assist, page, TRUE);
    }
    else
    {
      gtk_assistant_set_page_complete (assist, page, are_all_atoms_thermostated ());
    }
  }

  GtkWidget * conclu = create_vbox (BSEP);
  info = g_strdup_printf ("<b>   Finalize the creation of the CPMD input file now !</b>");
  add_box_child_start (GTK_ORIENTATION_VERTICAL, conclu, markup_label(info, -1, -1, 0.5, 0.5), TRUE, TRUE, 100);
  g_free (info);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, conclu, markup_label("\n \t<b>Note: </b>You can re-open this assitant later if required to adjust your choices\n", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
  gtk_assistant_append_page (assist, conclu);
  gtk_assistant_set_page_title (assist, conclu, "Create the input file now !");
  gtk_assistant_set_page_type (assist, conclu, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (assist, conclu, TRUE);
  gtk_assistant_update_buttons_state (assist);
}

/*!
  \fn void proj_unselect_all_atoms ()

  \brief unselect all atom(s) in the target project of the assistant
*/
void proj_unselect_all_atoms ()
{
  int i, j;
  for (i=0; i<qm_proj -> natomes; i++)
  {
    for (j=0; j<2; j++)
    {
      qm_proj -> atoms[0][i].pick[j] = FALSE;
      qm_proj -> atoms[0][i].label[j] = FALSE;
    }
  }
  qm_view -> picked = 0;
  init_default_shaders (qm_view);
}

/*!
  \fn G_MODULE_EXPORT void on_qm_assistant_cancel (GtkAssistant * assistant, gpointer data)

  \brief  cancel QM / QM-MM input file creation

  \param assistant the GtkAssistant sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_qm_assistant_cancel (GtkAssistant * assistant, gpointer data)
{
  destroy_this_widget (GTK_WIDGET(assistant));
  proj_unselect_all_atoms ();
   // restore selection if any from calc.c
  restore_ogl_selection (qm_view);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean on_qm_assistant_cancel_event (GtkWindow * assistant, gpointer data)

  \brief QM / QM-MM input file creation cancel event GTK4

  \param assistant the GtkWindow sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_qm_assistant_cancel_event (GtkWindow * assistant, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_qm_assistant_cancel_event (GtkWidget * assistant, GdkEvent  * event, gpointer data)

  \brief QM / QM-MM input file creation cancel event GTK3

  \param assistant the GtkWidget sending the signal
  \param event
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_qm_assistant_cancel_event (GtkWidget * assistant, GdkEvent  * event, gpointer data)
#endif
{
  on_qm_assistant_cancel (((GtkAssistant *)assistant), data);
  return TRUE;
}

/*!
  \fn G_MODULE_EXPORT void on_qm_assistant_close (GtkAssistant * assistant, gpointer data)

  \brief close QM / QM-MM input creation assistant

  \param assistant the GtkAssistant sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_qm_assistant_close (GtkAssistant * assistant, gpointer data)
{
  // Apply changes ... then close window
  destroy_this_widget (GTK_WIDGET(assistant));
  // Saving

  // Cleaning
  proj_unselect_all_atoms ();
   // restore selection if any from calc.c
  restore_ogl_selection (qm_view);
}

/*!
  \fn G_MODULE_EXPORT gint on_qm_assistant_go_forward (gint current_page, gpointer data)

  \brief QM / QM-MM assistant go to the next page

  \param current_page the current assistant page
  \param data the associated data pointer
*/
G_MODULE_EXPORT gint on_qm_assistant_go_forward (gint current_page, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  if (i == 0)
  {
    switch (current_page)
    {
      case MAXDATAQM+4:
        return -1;
        break;
      case 2:
        if (tmp_cpmd -> calc_type == 2 || tmp_cpmd -> calc_type == 3)
        {
          return 3;
        }
        else
        {
          return 4;
        }
        break;
      case 5:
        if ((int)tmp_cpmd -> default_opts[2])
        {
          return current_page+1;
        }
        else if (tmp_cpmd -> calc_type == 6)
        {
          return current_page+2;
        }
        else
        {
          return current_page+3;
        }
        break;
      case 6:
        if (tmp_cpmd -> calc_type == 6)
        {
          return current_page+1;
        }
        else
        {
          return current_page+2;
        }
       default:
        return current_page+1;
        break;
    }
  }
  else
  {
    switch (current_page)
    {
      case MAXDATAQM+4:
        return -1;
        break;
      default:
        return current_page+1;
        break;
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void on_qm_assistant_prepare (GtkAssistant * assistant, GtkWidget * page, gpointer data)

  \brief prepare QM / QM-MM assistant pages before display

  \param assistant the GtkAssistant sending the signal
  \param page the current page
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_qm_assistant_prepare (GtkAssistant * assistant, GtkWidget * page, gpointer data)
{
  int i = gtk_assistant_get_current_page (assistant);
  // field_unselect_all ();
  switch (i)
  {
    case 0:
      if (is_the_widget_visible(qm_preview_but)) hide_the_widgets (qm_preview_but);
      break;
    default:
      if (! is_the_widget_visible(qm_preview_but)) show_the_widgets (qm_preview_but);
      break;
  }
}

/*!
  \fn gboolean go_for_it (int i, int j, gboolean print[2])

  \brief add tab to the QM / QM-MM file preview window notebook

  \param i tab id
  \param j last tab id
  \param print basis and pseudopotential printing status
*/
gboolean go_for_it (int i, int j, gboolean print[2])
{
  if (tmp_cp2k -> input_type && i<=j-3) return TRUE;
  if (i==0) return TRUE;
  if (i==j-2 && print[0]) return TRUE;
  if (i==j-1 && print[1]) return TRUE;
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void show_qm_file_preview (GtkButton * but, gpointer data)

  \brief show QM / QM-MM input file preview

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_qm_file_preview (GtkButton * but, gpointer data)
{
  int i, j, k, c;
  c = GPOINTER_TO_INT(data);
  gchar * ptitle[5] = {"CP2K input file", "forces.inc", "system.inc", "motion.inc", "coord.inc"};
  gchar * wtite[2] = {" input file preview", " input files preview"};
  gchar * str;
  GtkWidget * scrollsets;
  GtkWidget * aview;
  GtkWidget * notebook;
  k = 0;
  gboolean print[2];
  if (c)
  {
    j = 6;
    notebook = gtk_notebook_new ();
    if (tmp_cp2k -> opts[CP2RUN] > 1.0 && tmp_cp2k -> opts[CP2RUN] < 4.0) k = 1;
    if (tmp_cp2k -> input_type)
    {
      str = g_strdup_printf ("%s %s", co_type[c], wtite[1]);
    }
    else
    {
      str = g_strdup_printf ("%s %s", co_type[c], wtite[0]);
    }
    print[0] = print[1] = FALSE;
    for (i=0; i<qm_proj -> nspec; i++)
    {
      if (tmp_cp2k -> spec_data[i][0] != -1)
      {
        print[0] = TRUE;
        break;
      }
    }
    for (i=0; i<qm_proj -> nspec; i++)
    {
      if (tmp_cp2k -> spec_data[i][1] != -1)
      {
        print[1] = TRUE;
        break;
      }
    }
  }
  else
  {
    j = 1;
    str = g_strdup_printf ("%s %s", co_type[c], wtite[0]);
  }
  GtkWidget * preview = dialogmodal (str, GTK_WINDOW (qm_assistant));
  g_free (str);
  if (c)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, dialog_get_content_area(preview), notebook, FALSE, FALSE, 0);
  }
  else
  {
    scrollsets = create_scroll (dialog_get_content_area(preview), 700, 350, GTK_SHADOW_ETCHED_IN);
  }
  for (i=0; i<j+k; i++)
  {
    if (! c || (c && go_for_it(i, j+k, print)))
    {
      aview = create_text_view (-1, -1, 0, 1, NULL, NULL, NULL);
      if (c)
      {
        scrollsets = create_scroll (NULL, 700, 350, GTK_SHADOW_ETCHED_IN);
        add_container_child (CONTAINER_SCR, scrollsets, aview);
        if (i > j+k-3)
        {
          str = g_strdup_printf ("%s", tmp_cp2k -> files[i-(j+k-3)]);
        }
        else
        {
          str = g_strdup_printf ("%s", ptitle[(i == j-3+k) ? 4 : i]);
        }
        gtk_notebook_append_page (GTK_NOTEBOOK(notebook), scrollsets, gtk_label_new (str));
        g_free (str);
      }
      else
      {
        add_container_child (CONTAINER_SCR, scrollsets, aview);
      }
    }
    if (c)
    {
      if (tmp_cp2k -> input_type || i != j+k-3)
      {
        print_cp2k ((i >= j+k-3) ? i+1-k: i, gtk_text_view_get_buffer(GTK_TEXT_VIEW(aview)));
      }
    }
    else
    {
      print_all_sections (gtk_text_view_get_buffer(GTK_TEXT_VIEW(aview)));
    }
  }
  run_this_gtk_dialog (preview, G_CALLBACK(run_destroy_dialog), NULL);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_saving_qm (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief apply QM / QM-MM assistant and create input file(s) - running the dialog GTK3

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_saving_qm (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_saving_qm (GtkDialog * info, gint response_id, gpointer data)

  \brief apply QM / QM-MM assistant and create input file(s) - running the dialog GTK3

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_saving_qm (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  int i, j, k, l;
  int c = GPOINTER_TO_INT(data);
  gboolean result = FALSE;
  gboolean done = FALSE;
  GtkTextBuffer * buffer = NULL;
  GtkTextIter bStart;
  GtkTextIter bEnd;
  GError * err = NULL;
  gchar * text;
  gchar * filename;
  gchar * direname;
  gchar * cp2sp[2] = {"basis.inc", "pseudo.inc"};
  gchar * cp2file[4] = {"forces.inc", "system.inc", "motion.inc", "coord.inc"};
  switch (response_id)
  {
    case GTK_RESPONSE_ACCEPT:
      filename = file_chooser_get_file_name (chooser);
      direname = file_chooser_get_current_folder (chooser);
      if (filename != NULL)
      {
        done = TRUE;
        if (! c || (c && ! tmp_cp2k -> input_type)) buffer = add_buffer (NULL, NULL, NULL);
        switch (c)
        {
          case 0:
            print_all_sections (buffer);
            break;
          case 1:
            j = 6;
            k = 0;
            if (tmp_cp2k -> opts[CP2RUN] > 1.0 && tmp_cp2k -> opts[CP2RUN] < 4.0) k = 1;
            for (i=0; i<j+k; i++)
            {
              if (i != j+k-3 || tmp_cp2k -> input_type)
              {
                l = (i >= j+k-3) ? i+1-k: i;
                if (tmp_cp2k -> input_type)
                {
                  buffer = add_buffer (NULL, NULL, NULL);
                  if (i > 0) filename = g_strdup_printf ("%s/%s", direname, cp2file[l-1]);
                  print_cp2k (l, buffer);
                  gtk_text_buffer_get_start_iter (buffer, & bStart);
                  gtk_text_buffer_get_end_iter (buffer, & bEnd);
                  text = gtk_text_buffer_get_text (buffer, & bStart, & bEnd, FALSE);
                  gtk_text_buffer_set_modified (buffer, FALSE);
                  result = g_file_set_contents (filename, text, -1, & err);
                  g_free (text);
                  g_object_unref (buffer);
                  if (! result && err)
                  {
                    show_error (g_strdup_printf ("Error while saving input file: %s\n Error: %s", filename, err -> message), 0, qm_assistant);
                    g_error_free (err);
                  }
                  g_free (filename);
                }
                else if (l != 5 && l != 6)
                {
                  print_cp2k (l, buffer);
                }
              }
            }
            break;
        }
        if (! c || (c && ! tmp_cp2k -> input_type))
        {
          gtk_text_buffer_get_start_iter (buffer, & bStart);
          gtk_text_buffer_get_end_iter (buffer, & bEnd);
          text = gtk_text_buffer_get_text (buffer, & bStart, & bEnd, FALSE);
          gtk_text_buffer_set_modified (buffer, FALSE);
          result = g_file_set_contents (filename, text, -1, & err);
          g_free (text);
          g_object_unref (buffer);
          if (! result && err)
          {
            /* error saving file, show message to user */
            show_error (g_strdup_printf ("Error while saving input file: %s\n Error: %s", filename, err -> message), 0, qm_assistant);
            g_error_free (err);
          }
          g_free (filename);
        }
        if (c)
        {
          // save pseudo and basis auto
          for (i=0; i<2; i++)
          {
            buffer = add_buffer (NULL, NULL, NULL);
            filename = g_strdup_printf ("%s/%s", direname, cp2sp[i]);
            print_cp2k (i+5, buffer);
            gtk_text_buffer_get_start_iter (buffer, & bStart);
            gtk_text_buffer_get_end_iter (buffer, & bEnd);
            text = gtk_text_buffer_get_text (buffer, & bStart, & bEnd, FALSE);
            result = g_file_set_contents (filename, text, -1, & err);
            g_free (text);
            if (! result && err)
            {
              /* error saving file, show message to user */
              show_error (g_strdup_printf ("Error while saving file %s: %s", cp2sp[i], err -> message), 0, qm_assistant);
              g_error_free (err);
              break;
            }
            g_free (filename);
            g_object_unref (buffer);
          }
        }
      }
      break;
    default:
      done = TRUE;
      break;
  }
  if (done)
  {
#ifdef GTK4
    destroy_this_native_dialog (info);
#else
    destroy_this_dialog (info);
#endif
  }
  if (c && result)
  {
    if (tmp_cp2k -> input_type)
    {
      if (tmp_cp2k -> opts[CP2RUN] > 1.0 && tmp_cp2k -> opts[CP2RUN] < 4.0)
      {
        show_info ("The input files 'forces.inc', 'system.inc', 'motion.inc' and 'coord.inc'\n"
                   "as well as the files 'basis.inc' and 'pseudo.inc'\n"
                   "for the atomic basis set and peudo-potentials\n"
                   "were saved in the same directory as the main input file.", 0, qm_assistant);
      }
      else
      {
        show_info ("The input files 'forces.inc', 'system.inc', and 'coord.inc'\n"
                   "as well as the files 'basis.inc' and 'pseudo.inc'\n"
                   "for the atomic basis set and peudo-potentials\n"
                   "were saved in the same directory as the main input file.", 0, qm_assistant);
      }
    }
    else
    {
      show_info ("The files 'basis.inc' and 'pseudo.inc'\n"
                 "for the atomic basis set and peudo-potentials\n"
                 "were saved in the same directory as the main input file.", 0, qm_assistant);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void on_qm_assistant_apply (GtkAssistant * assistant, gpointer data)

  \brief apply QM / QM-MM assistant and create input file(s) - creating the dialog

  \param assistant the GtkAssistant sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_qm_assistant_apply (GtkAssistant * assistant, gpointer data)
{
  int c = GPOINTER_TO_INT(data);
  gchar * text;
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  GtkFileFilter * filter1, * filter2;
  gchar * qm_type[2] = {"CPMD", "CP2K"};
  const gchar * qm_name[2] = {"CPMD input file (*.in)", "CP2K input file (*.inp)"};
  const gchar * qm_ext[2] = {".in", ".inp"};

  text = g_strdup_printf ("Saving %s input file(s)", qm_type[c]);
  info = create_file_chooser (text,
                              GTK_WINDOW(assistant),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              "Save");
  GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
  g_free (text);
#ifdef GTK3
  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
  file_chooser_set_current_folder (chooser);
  filter1 = gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter1), qm_name[c]);
  text = g_strdup_printf ("*%s", qm_ext[c]);
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter1), text);
  g_free (text);
  gtk_file_chooser_add_filter (chooser, filter1);
  filter2 = gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter2), "All files (*)");
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter2), "*");
  gtk_file_chooser_add_filter (chooser, filter2);
  text = g_strdup_printf ("%s%s", prepare_for_title(qm_proj -> name), qm_ext[c]);
  file_chooser_set_current_folder (chooser);
  gtk_file_chooser_set_current_name (chooser, text);
  g_free (text);
#ifdef GTK4
  run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_saving_qm), NULL);
#else
  run_this_gtk_dialog (info, G_CALLBACK(run_saving_qm), data);
#endif
}

/*!
  \fn void create_qm_input_file (int c, int p, int s)

  \brief initialize an ab-initio MD input file creation assistant

  \param c 0 = CPMD, 1 = CP2K
  \param p the target project
  \param s 0 = ab-initio, 1 = QM-MM
*/
void create_qm_input_file (int c, int p, int s)
{
  gchar * qm_type[2] = {"first-principles", "QM-MM"};
  qm_assistant = gtk_assistant_new ();
  gtk_widget_set_size_request (qm_assistant, 800, 600);
  int i, j;
  //field_color = TRUE;
  field_object = -1;
  //selected_aspec = -1;
  opengl_project_changed (p);
  qm_proj = get_project_by_id(p);
  for (i=0; i<MAXDATAQM+2; i++) qmbuffer[i] = NULL;
  if (c == 0)
  {
    is_cpmd = TRUE;
    if (qm_proj -> cpmd_input[s] == NULL)
    {
      qm_proj -> cpmd_input[s] = g_malloc0 (sizeof*qm_proj -> cpmd_input[s]);
      qm_proj -> cpmd_input[s] -> calc_type = 0;
      qm_proj -> cpmd_input[s] -> thermostats = 0;
      qm_proj -> cpmd_input[s] -> ions_thermostat = NULL;
      qm_proj -> cpmd_input[s] -> elec_thermostat = NULL;
      qm_proj -> cpmd_input[s] -> dummies = 0;
      qm_proj -> cpmd_input[s] -> dummy = NULL;
      qm_proj -> cpmd_input[s] -> fixat = 0;
      qm_proj -> cpmd_input[s] -> fixlist = NULL;
      qm_proj -> cpmd_input[s] -> fixcoord = NULL;
      qm_proj -> cpmd_input[s] -> restart[0] = 0;
      qm_proj -> cpmd_input[s] -> restart[1] = 100;
      qm_proj -> cpmd_input[s] -> restart[2] = 2;
      qm_proj -> cpmd_input[s] -> restart[3] = 0;
      qm_proj -> cpmd_input[s] -> restart[4] = 1;
      for (i=5; i<9; i++) qm_proj -> cpmd_input[s] -> restart[i] = 0;
      qm_proj -> cpmd_input[s] -> restart[9] = 0;
      for (i=0; i<17; i++) qm_proj -> cpmd_input[s] -> default_opts[i] = default_cpmd_options[i];
      for (i=0; i<24; i++) qm_proj -> cpmd_input[s] -> calc_opts[i] = default_calc_options[i];
      qm_proj -> cpmd_input[s] -> pp = allocdint (qm_proj -> nspec, 2);
      for (i=0; i<qm_proj -> nspec; i++) qm_proj -> cpmd_input[s] -> pp[i][0] = qm_proj -> cpmd_input[s] -> pp[i][1] = 1;
      qm_proj -> cpmd_input[s] -> info = g_strdup_printf ("  Project name: %s\n"
                                                          "  Total number of atoms:      %d\n"
                                                          "  Number of chemical species: %d",
                                                          prepare_for_title(qm_proj -> name), qm_proj -> natomes, qm_proj -> nspec);
    }
    tmp_cpmd = qm_proj -> cpmd_input[s];
  }
  else
  {
    is_cpmd = FALSE;
    if (qm_proj -> cp2k_input[s] == NULL)
    {
      qm_proj -> cp2k_input[s] = g_malloc0 (sizeof*qm_proj -> cp2k_input[s]);
      qm_proj -> cp2k_input[s] -> input_type = 0;
      for (i=0; i<2; i++)
      {
        qm_proj -> cp2k_input[s] -> fixat[i] = 0;
        qm_proj -> cp2k_input[s] -> fixlist[i] = NULL;
        qm_proj -> cp2k_input[s] -> fixcoord[i] = NULL;
      }
      qm_proj -> cp2k_input[s] -> thermostats = 0;
      qm_proj -> cp2k_input[s] -> ions_thermostat = NULL;
      for (i=0; i<5; i++) qm_proj -> cp2k_input[s] -> files[i] = NULL;
      for (i=0; i<41; i++) qm_proj -> cp2k_input[s] -> opts[i] = default_cp2k_options[i];
      qm_proj -> cp2k_input[s] -> opts[CP2SYM] = find_cp2k_sym ();
      for (i=0; i<3; i++)
      {
        for (j=0;j<4;j++) qm_proj -> cp2k_input[s] -> extra_opts[i][j] = default_cp2k_extra[i][j];
        if (i == 0) qm_proj -> cp2k_input[s] -> extra_opts[i][2] = default_vdw_cut[0];
      }
      qm_proj -> cp2k_input[s] -> spec_data = allocdint(qm_proj -> nspec, 2);
      qm_proj -> cp2k_input[s] -> spec_files = g_malloc (qm_proj -> nspec*sizeof*qm_proj -> cp2k_input[s] -> spec_files);
      for (i=0; i<qm_proj -> nspec; i++)
      {
        qm_proj -> cp2k_input[s] -> spec_data[i][0] = cp2k_is_basis_in_database (i);
        qm_proj -> cp2k_input[s] -> spec_data[i][1] = cp2k_is_pseudo_in_database (i);
        qm_proj -> cp2k_input[s] -> spec_files[i] = g_malloc0 (2*sizeof*qm_proj -> cp2k_input[s] -> spec_files[i]);
        qm_proj -> cp2k_input[s] -> spec_files[i][0] = NULL;
        qm_proj -> cp2k_input[s] -> spec_files[i][1] = NULL;
      }
      gchar * defname[2]={"basis.inc", "pseudo.inc"};
      for (j=0;j<2;j++)
      {
        for (i=0; i<qm_proj -> nspec; i++)
        {
          if (qm_proj -> cp2k_input[s] -> spec_data[i][j] > -1)
          {
            qm_proj -> cp2k_input[s] -> files[1+j] = g_strdup_printf ("%s", defname[j]);
            break;
          }
        }
      }
      qm_proj -> cp2k_input[s] -> info = g_strdup_printf ("  Project name: %s\n"
                                                          "  Total number of atoms:      %d\n"
                                                          "  Number of chemical species: %d",
                                                          prepare_for_title(qm_proj -> name), qm_proj -> natomes, qm_proj -> nspec);
    }
    tmp_cp2k = qm_proj -> cp2k_input[s];
  }
  qm_coord = qm_proj -> coord;
  qm_view = qm_proj -> modelgl;

  gtk_window_set_resizable (GTK_WINDOW (qm_assistant), FALSE);
  gtk_window_set_modal (GTK_WINDOW (qm_assistant), TRUE);
  gchar * str = g_strdup_printf ("Basic %s - %s - calculation assistant", co_type[c], qm_type[s]);
  gtk_window_set_title (GTK_WINDOW(qm_assistant), str);
  g_free (str);

  GtkWidget * intro = create_vbox (BSEP);
  gchar * info = g_strdup_printf ("\t<b>This assistant will help you to setup a %s %s\n"
                                  "\tcalculation using <i>%s</i> 3D model as starting point</b>\n"
                                  "\nPlease note that the %s code offers so many calculation options that it is not possible\n"
                                  "to provide a description and offer a comprehensive usage guide for each of them. \n"
                                  "Therefore this assistant only provides help towards basics and / or frequently used %s instructions. \n\n"
                                  "<b>In any case if you intent to use the %s code please refer to the user manual.</b>",
                                  qm_type[s], co_type[c], qm_proj -> name, co_type[c], co_type[c], co_type[c]);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, intro, markup_label(info, -1, -1, 0.5, 0.5), TRUE, TRUE, 50);
  g_free (info);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, intro,
                       markup_label("\n \t<b>Note: </b>You can re-open this assistant later if required to adjust your choices\n", -1, -1, 0.0, 0.5),
                       FALSE, FALSE, 0);
  gtk_assistant_append_page (GTK_ASSISTANT (qm_assistant), intro);
  str = g_strdup_printf ("%s calculation set-up", co_type[c]);
  gtk_assistant_set_page_title (GTK_ASSISTANT (qm_assistant), intro, str);
  g_free (str);
  gtk_assistant_set_page_type (GTK_ASSISTANT (qm_assistant), intro, GTK_ASSISTANT_PAGE_INTRO);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (qm_assistant), intro, qm_assist_init);
  if (c == 0)
  {
    add_cpmd_pages ();
  }
  else
  {
    add_cp2k_pages ();
  }
  gtk_assistant_set_forward_page_func (GTK_ASSISTANT (qm_assistant), on_qm_assistant_go_forward, GINT_TO_POINTER(c), NULL);
  qm_preview_but = create_button ("Preview", IMG_STOCK, EDITF, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(show_qm_file_preview), GINT_TO_POINTER(c));
  gtk_assistant_add_action_widget (GTK_ASSISTANT (qm_assistant), qm_preview_but);
  g_signal_connect (G_OBJECT (qm_assistant), "prepare", G_CALLBACK(on_qm_assistant_prepare), GINT_TO_POINTER(c));
  g_signal_connect (G_OBJECT (qm_assistant), "cancel", G_CALLBACK(on_qm_assistant_cancel), GINT_TO_POINTER(c));
  g_signal_connect (G_OBJECT (qm_assistant), "close", G_CALLBACK(on_qm_assistant_close), GINT_TO_POINTER(c));
  g_signal_connect (G_OBJECT (qm_assistant), "apply", G_CALLBACK(on_qm_assistant_apply), GINT_TO_POINTER(c));
  add_gtk_close_event (qm_assistant, G_CALLBACK(on_qm_assistant_cancel_event), GINT_TO_POINTER(c));

  gtk_assistant_set_page_complete (GTK_ASSISTANT (qm_assistant),
                                   gtk_assistant_get_nth_page(GTK_ASSISTANT (qm_assistant), 0), TRUE);
  show_the_widgets (qm_preview_but);
  show_the_widgets (qm_assistant);
  if (c == 0)
  {
    for (i=0; i<NCPMDCALC; i++) hide_the_widgets (calc_box[i]);
    show_the_widgets (calc_box[tmp_cpmd -> calc_type]);
    if (tmp_cpmd -> calc_type != 2) hide_the_widgets (electron_box);
  }
  hide_the_widgets (qm_preview_but);
}

