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
* @file edit_menu.c
* @short Creation of the edition dialogs
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'edit_menu.c'
*
* Contains:
*

 - The creation of the edition dialogs

*
* List of functions:

  gboolean test_vol (double box[2][3], double vect[3][3]);
  gboolean test_pbc (int pbc, int frac, double box[2][3], double vect[3][3]);
  gboolean has_box_changed ();
  gboolean have_vectors_changed ();
  gboolean test_cutoffs ();

  void edit_box (GtkWidget * vbox);
  void edit_chem (GtkWidget * vbox);
  void init_box_calc ();
  void prep_box (int id);
  void test_chem ();
  void edit_bonds (GtkWidget * vbox);

  G_MODULE_EXPORT void update_box (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void toggle_pbc (GtkCheckButton * Button, gpointer data);
  G_MODULE_EXPORT void toggle_pbc (GtkToggleButton * Button, gpointer data);
  G_MODULE_EXPORT void toggle_frac (GtkCheckButton * Button, gpointer data);
  G_MODULE_EXPORT void toggle_frac (GtkToggleButton * Button, gpointer data);
  G_MODULE_EXPORT void update_vect (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void run_vectors (GtkDialog * win, gint response_id, gpointer data);
  G_MODULE_EXPORT void on_vectors_clicked (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void update_chemistry (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void on_spec_changed (GtkComboBox * combo, gpointer data);
  G_MODULE_EXPORT void on_rad_changed (GtkComboBox * combo, gpointer data);
  G_MODULE_EXPORT void toggle_xcor (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void toggle_xcor (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void run_on_edit_activate (GtkDialog * win, gint response_id, gpointer data);
  G_MODULE_EXPORT void on_edit_activate (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "bind.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"
#include "glwindow.h"
#include "preferences.h"

char * box_p[2]={"<b>Edges [&#xC5;]</b>", "<b>Angles [&#xB0;]</b>"};
char * box_prop[2][3]={{"<b><i>a</i></b>", "<b><i>b</i></b>", "<b><i>c</i></b>"},
                       {"alpha - &#x3B1;", "beta - &#x3B2;", "gamma - &#x263;"}};
char * chem_lab[CHEM_PARAMS+2]={"Atom:", "Element:",
                                "Atomic number", "Atomic mass:", "Radius",
                                "Neutrons scattering length:", "X-rays scattering length:"};
char * chem_rad[4]={"Covalent", "Ionic", "van Der Waals", "In crystal"};
char * chem_unit[4]={" g/mol", " &#xC5;", " fm", " e.u"};
char * edit_prop[4]={"Chemistry and physics", "Box and periodicity", "Bond cutoffs", "Lattice vectors"};
char * vect_name[3]={"<i>a</i>", "<i>b</i>", "<i>c</i>"};
char * vect_comp[3]={"x", "y", "z"};
GtkWidget * frac_box;
GtkWidget * spec_box;
GtkWidget * rad_box;
GtkWidget * chem_spec[2];
GtkWidget * chem_entry[CHEM_PARAMS-1];
GtkWidget * vect_entry[9];
double * tmp_chem[CHEM_PARAMS];
double tmp_lattice[2][3];
double tmp_vect[3][3];
int tmp_pbc, tmp_frac;
int tmp_xcor, tmp_lat;
dint t_box[9];
extern double * tmpcut;
extern void update_cutoffs (project * this_proj);
extern void cut_box (project * this_proj, GtkWidget * vbox);

/*!
  \fn G_MODULE_EXPORT void update_box (GtkEntry * entry, gpointer data)

  \brief update lattice parameters

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_box (GtkEntry * entry, gpointer data)
{
  dint * id = (dint *)data;
  const gchar * m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  if (v >= 0.0)
  {
    tmp_lattice[id -> a][id -> b] = v;
  }
  update_entry_double (entry, v);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggle_pbc (GtkCheckButton * Button, gpointer data)

  \brief use PBC ?

  \param Button the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_pbc (GtkCheckButton * Button, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggle_pbc (GtkToggleButton * Button, gpointer data)

  \brief use PBC ?

  \param Button the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_pbc (GtkToggleButton * Button, gpointer data)
#endif
{
  tmp_pbc = button_get_status ((GtkWidget *)Button);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggle_frac (GtkCheckButton * Button, gpointer data)

  \brief use fractional coordinates

  \param Button the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_frac (GtkCheckButton * Button, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggle_frac (GtkToggleButton * Button, gpointer data)

  \brief use fractional coordinates

  \param Button the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_frac (GtkToggleButton * Button, gpointer data)
#endif
{
  tmp_frac = button_get_status ((GtkWidget *)Button);
  /*if (gtk_toggle_button_get_active (Button))
  {
    widget_set_sensitive (frac_box, 1);
    combo_set_active (frac_box, 0);
  }
  else
  {
    combo_set_active (frac_box, -1);
    widget_set_sensitive (frac_box, 0);
  }*/
}

/*!
  \fn G_MODULE_EXPORT void update_vect (GtkEntry * entry, gpointer data)

  \brief update lattice vector component

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_vect (GtkEntry * entry, gpointer data)
{
  const gchar * m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  update_entry_double (entry, v);
}

/*!
  \fn G_MODULE_EXPORT void run_vectors (GtkDialog * win, gint response_id, gpointer data)

  \brief lattice vectors: run the dialog

  \param win the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_vectors (GtkDialog * win, gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_APPLY)
  {
    int i, j, k;
    const gchar * m;
    double v;
    i = 0;
    for (j=0; j<3; j++)
    {
      for (k=0; k<3; k++)
      {
        m = entry_get_text (GTK_ENTRY(vect_entry[i]));
        v = string_to_double ((gpointer)m);
        tmp_vect[j][k] = v;
        i ++;
      }
    }
  }
  destroy_this_dialog (win);
}

/*!
  \fn G_MODULE_EXPORT void on_vectors_clicked (GtkButton * but, gpointer data)

  \brief lattice vectors - prepare the dialog

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_vectors_clicked (GtkButton * but, gpointer data)
{
  int i, j, k;
  GtkWidget * win = dialog_cancel_apply (edit_prop[3], MainWindow, FALSE);
  GtkWidget * table = gtk_grid_new ();
  add_box_child_start (GTK_ORIENTATION_VERTICAL, dialog_get_content_area (win), table, FALSE, FALSE, 5);
  k = 0;
  for (i=0; i<4; i++)
  {
    if (i > 0)
    {
      gtk_grid_attach (GTK_GRID (table), markup_label(vect_name[i-1], 30, -1, 0.5, 0.5), 0, i, 1, 1);
    }
    for (j=0; j<4; j++)
    {
      if (j > 0)
      {
        if (i == 0)
        {
          gtk_grid_attach (GTK_GRID (table), markup_label(vect_comp[j-1], -1, 30, 0.5, 0.5), j, 0, 1, 1);
        }
        else
        {
          vect_entry[k] = create_entry (G_CALLBACK(update_vect), 100, 15, FALSE, NULL);
          update_entry_double (GTK_ENTRY(vect_entry[k]), tmp_vect[i-1][j-1]);
          gtk_grid_attach (GTK_GRID (table), vect_entry[k], j, i, 1, 1);
          k ++;
        }
      }
    }
  }
  run_this_gtk_dialog (win, G_CALLBACK(run_vectors), NULL);
}

/*!
  \fn void edit_box (GtkWidget * vbox)

  \brief creation of the edit cell widgets

  \param vbox GtkWidget that will receive the data
*/
void edit_box (GtkWidget * vbox)
{
  int i, j, k;
  GtkWidget * entry;
  GtkWidget * hbox;
  tmp_pbc = active_cell -> pbc;
  tmp_frac = active_cell -> frac;

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                       check_button ("Apply Periodic Boundary Conditions",
                                     -1, 40, tmp_pbc, G_CALLBACK(toggle_pbc), NULL), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                       check_button ("Fractional coordinates",
                                     -1, 40, tmp_frac, G_CALLBACK(toggle_frac), NULL), FALSE, FALSE, 0);
  GtkWidget * table = gtk_grid_new ();
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, table, FALSE, FALSE, 0);
  k = 0;
  for (i=0; i<3; i++)
  {
    for (j=0; j<3; j++, k++)
    {
      if (i < 2) tmp_lattice[i][j] = active_box -> param[i][j];
      tmp_vect[i][j] = active_box -> vect[i][j];
      t_box[k].a = i;
      t_box[k].b = j;
    }
  }
  tmp_lat = active_cell -> ltype;
  k = 0;
  for (i=0; i<2; i++)
  {
    gtk_grid_attach (GTK_GRID (table), markup_label(box_p[i], -1, 50, 0.5, 0.5), 1, i+2*i, 1, 1);
    for (j=0; j<3; j++, k++)
    {
      gtk_grid_attach (GTK_GRID (table), markup_label(box_prop[i][j], -1, -1, 0.5, 0.5), j, i+2*i+1, 1, 1);
      entry = create_entry (G_CALLBACK(update_box), 100, 15, FALSE, (gpointer)& t_box[k]);
      update_entry_double (GTK_ENTRY(entry), tmp_lattice[i][j]);
      gtk_grid_attach (GTK_GRID (table), entry, j, i+2*i+2, 1, 1);
    }
  }
  GtkWidget * but = create_button ("Lattice Vectors", IMG_NONE, NULL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(on_vectors_clicked), NULL);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 145);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 15);
}

/*!
  \fn G_MODULE_EXPORT void update_chemistry (GtkEntry * entry, gpointer data)

  \brief update chemical property

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_chemistry (GtkEntry * entry, gpointer data)
{
  int i = combo_get_active (spec_box);
  int j = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (entry);
  double v = string_to_double ((gpointer)m);
  tmp_chem[j][i] = v;
  update_entry_double (entry, v);
}

/*!
  \fn G_MODULE_EXPORT void on_spec_changed (GtkComboBox * combo, gpointer data)

  \brief change the chemical species

  \param combo the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_spec_changed (GtkComboBox * combo, gpointer data)
{
  int i, j;
  i = combo_get_active ((GtkWidget *)combo);
  gtk_label_set_text (GTK_LABEL(chem_spec[0]), active_chem -> element[i]);
  gtk_label_set_text (GTK_LABEL(chem_spec[1]), g_strdup_printf("%d", (int)active_chem -> chem_prop[CHEM_Z][i]));
  for (j=0; j<CHEM_PARAMS-1; j++)
  {
    update_entry_double (GTK_ENTRY(chem_entry[j]), tmp_chem[j][i]);
  }
  combo_set_active (rad_box, -1);
}

/*!
  \fn G_MODULE_EXPORT void on_rad_changed (GtkComboBox * combo, gpointer data)

  \brief change the type of atomic radius

  \param combo the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_rad_changed (GtkComboBox * combo, gpointer data)
{
  int i, j, k;
  i = combo_get_active ((GtkWidget *)combo);
  if (i != -1)
  {
    j = combo_get_active (spec_box);
    k = (int)active_chem -> chem_prop[CHEM_Z][j];
    tmp_chem[1][j] = set_radius_ (& k, & i);
    update_entry_double (GTK_ENTRY(chem_entry[1]), tmp_chem[1][j]);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggle_xcor (GtkCheckButton * but, gpointer data)

  \brief use X ray diffraction q coorection

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_xcor (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggle_xcor (GtkToggleButton * but, gpointer data)

  \brief use X ray diffraction q coorection

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_xcor (GtkToggleButton * but, gpointer data)
#endif
{
  tmp_xcor = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (chem_entry[CHEM_PARAMS-2], ! tmp_xcor);
}

/*!
  \fn void edit_chem (GtkWidget * vbox)

  \brief creation of the edit chemical properties widgets

  \param vbox GtkWidget that will receive the data
*/
void edit_chem (GtkWidget * vbox)
{
  int i, j;
  spec_box = create_combo();
  widget_set_sensitive(spec_box, 1);
  g_signal_connect(G_OBJECT(spec_box), "changed", G_CALLBACK(on_spec_changed), NULL);
  GtkWidget * table = gtk_grid_new ();
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, table, FALSE, FALSE, 5);
  GtkWidget * chem_fixed = gtk_fixed_new ();
  gtk_grid_attach (GTK_GRID (table), chem_fixed, 1, 0, 1, 1);
  for (i=0; i<CHEM_PARAMS+2; i++)
  {
    j = (i == 0) ? i : i + 2;
    gtk_grid_attach (GTK_GRID (table), markup_label(chem_lab[i], 180, 30, 0.0, 0.5), 0, j, 1, 1);
    if (i == 0)
    {
      for (j=0; j<active_project -> nspec; j++)
      {
        combo_text_append (spec_box, active_chem -> label[j]);
      }
    }
    else
    {
      if (i > 0 && i < 3)
      {
        chem_spec[i-1] = markup_label ("", -1, -1, 0.5, 0.5);
        gtk_grid_attach (GTK_GRID (table), chem_spec[i-1], 1, i+2, 1, 1);
      }
      else
      {
        if (i==5)
        {
          rad_box = create_combo ();
          gtk_widget_set_size_request (rad_box, -1, 30);
          for (j=0; j<4; j++)
          {
            combo_text_append (rad_box, chem_rad[j]);
          }
          combo_set_active (rad_box, -1);
          g_signal_connect(G_OBJECT(rad_box), "changed", G_CALLBACK(on_rad_changed), NULL);
          gtk_grid_attach (GTK_GRID (table), rad_box, 1, i+1, 1, 1);
        }
        tmp_chem[i-3] = g_malloc (active_project -> nspec*sizeof*tmp_chem[i-3]);
        for (j=0; j<active_project -> nspec; j++)
        {
          tmp_chem[i-3][j] =  active_chem -> chem_prop[i-2][j];
        }
        chem_entry[i-3] = create_entry (G_CALLBACK(update_chemistry), 120, 15, FALSE, GINT_TO_POINTER(i-3));
        update_entry_double (GTK_ENTRY(chem_entry[i-3]), tmp_chem[i-3][0]);
        gtk_grid_attach (GTK_GRID (table), chem_entry[i-3], 2, i+2, 1, 1);
        if (i==CHEM_PARAMS+1)
        {
          tmp_xcor = active_project -> xcor;
          GtkWidget * chem_xcor = check_button ("= f(Q)", -1, 30, tmp_xcor, G_CALLBACK(toggle_xcor), NULL);
          widget_set_sensitive (chem_entry[i-3], ! tmp_xcor);
          gtk_grid_attach (GTK_GRID (table), chem_xcor, 1, i+2, 1, 1);
        }
        gtk_grid_attach (GTK_GRID (table), markup_label(chem_unit[i-3], -1, -1, 0.0, 0.5), 3, i+2, 1, 1);
      }
    }
  }
  combo_set_active (spec_box, 0);
  gtk_fixed_put (GTK_FIXED (chem_fixed), spec_box, -1, -1);
}

/*!
  \fn gboolean test_vol (double box[2][3], double vect[3][3])

  \brief is the cell properly described to use PBC ?

  \param box lattice parameters
  \param vect lattice vectors
*/
gboolean test_vol (double box[2][3], double vect[3][3])
{
  int i, j;
  gboolean val = TRUE;
  tmp_lat = 0;
  for (i=0; i<2; i++)
  {
    for (j=0; j<3; j++)
    {
      if (box[i][j] == 0.0) val = FALSE;
    }
  }
  if (val) tmp_lat = 1;
  val = TRUE;
  for (i=0; i<3; i++)
  {
    if (vect[i][0] == 0.0 && vect[i][1] == 0.0 && vect[i][2] == 0.0)
    {
      val = FALSE;
    }
  }
  if (val) tmp_lat = 2;
  return (tmp_lat) ? TRUE : FALSE;
}

/*!
  \fn gboolean test_pbc (int pbc, int frac, double box[2][3], double vect[3][3])

  \brief is the cell properly described ?

  \param pbc
  \param frac fractional coordinates
  \param box lattice parameters
  \param vect lattice vectors
*/
gboolean test_pbc (int pbc, int frac, double box[2][3], double vect[3][3])
{
  if (! pbc && ! frac)
  {
    if (! test_vol (box, vect)) tmp_lat = active_cell -> ltype;
    return TRUE;
  }
  else
  {
    return test_vol (box, vect);
  }
}

/*!
  \fn void init_box_calc ()

  \brief initialize calculation possibilities based the periodicity
*/
void init_box_calc ()
{
  int i;
  active_cell -> has_a_box = test_vol (active_box -> param, active_box -> vect);
  if (! active_cell -> has_a_box)
  {
    for (i=0; i<4; i++) active_project -> runok[i] = FALSE;
  }
  else
  {
    for (i=0; i<3; i=i+2)
    {
      active_project -> runok[i] = TRUE;
      active_project -> runok[i+1] = active_project -> visok[i];
    }
  }
  prep_calc_actions ();
}

gboolean has_box_changed ()
{
  int i, j;
  gboolean changed = FALSE;
  for (i=0; i<2; i++)
  {
    for (j=0; j<3; j++)
    {
      if (tmp_lattice[i][j] != active_box -> param[i][j])
      {
        active_box -> param[i][j] = tmp_lattice[i][j];
        changed = TRUE;
      }
    }
  }
  return changed;
}

gboolean have_vectors_changed ()
{
  int i, j;
  gboolean changed = FALSE;
  for (i=0; i<3; i++)
  {
    for (j=0; j<3; j++)
    {
      if (tmp_vect[i][j] != active_box -> vect[i][j])
      {
        active_box -> vect[i][j] = tmp_vect[i][j];
        changed = TRUE;
      }
    }
  }
  return changed;
}

/*!
  \fn void prep_box (int id)

  \brief prepare the project depending on the changes to the MD box

  \param id
*/
void prep_box (int id)
{
  int i;
  // active_project -> run = 1;
  if (tmp_pbc != active_cell -> pbc) active_project -> run = 0;
  active_cell -> pbc = tmp_pbc;
  if (tmp_lat < 2 && tmp_lat != active_cell -> ltype) active_project -> run = 0;
  if (tmp_lat > 0)
  {
    if (tmp_lat == 1 && has_box_changed())
    {
      active_project -> run = 0;
    }
    else if (tmp_lat == 2 && have_vectors_changed())
    {
      active_project -> run = 0;
    }
  }
  active_cell -> ltype = tmp_lat;
  init_box_calc ();
  if (tmp_frac != active_cell -> frac) active_project -> run = 0;
  active_cell -> frac = tmp_frac;
  if (id > 1) active_project -> run = 0;
  if (! active_project -> run)
  {
    if (active_project -> modelgl)
    {
      for (i=0; i<3; i++) active_project -> modelgl -> cshift[i] = 0.0;
    }
  }
}

/*!
  \fn void test_chem ()

  \brief were chemical properties modified ?
*/
void test_chem ()
{
  int i, j;
  gboolean upchem = FALSE;
  for (i=1; i<CHEM_PARAMS; i++)
  {
    for (j=0; j<active_project -> nspec; j++)
    {
      if (tmp_chem[i-1][j] != active_chem -> chem_prop[i][j])
      {
        active_chem -> chem_prop[i][j] = tmp_chem[i-1][j];
        upchem = TRUE;
      }
    }
    g_free (tmp_chem[i-1]);
    tmp_chem[i-1] = NULL;
  }
  if (tmp_xcor != active_project -> xcor)
  {
    active_project -> xcor = tmp_xcor;
    upchem = TRUE;
  }
  if (upchem)
  {
    read_chem_ (active_chem -> chem_prop[CHEM_M], active_chem -> chem_prop[CHEM_R],
                active_chem -> chem_prop[CHEM_N], active_chem -> chem_prop[CHEM_X]);
  }
}

/*!
  \fn gboolean test_cutoffs ()

  \brief are all cutoffs described ?
*/
gboolean test_cutoffs ()
{
  int i, j, k;
  k = 0;
  for (i=0; i<active_project -> nspec; i++, k++)
  {
    if (tmpcut[k] == 0.0 || (active_project -> run && tmpcut[k] > active_project -> max[GR])) return FALSE;
  }
  for (i=0; i<active_project -> nspec-1; i++)
  {
    for (j=i+1; j<active_project -> nspec; j++, k++)
    {
      if (tmpcut[k] == 0.0 || (active_project -> run && tmpcut[k] > active_project -> max[GR])) return FALSE;
    }
  }
  if (tmpcut[k] == 0.0 || (active_project -> run && tmpcut[k] > active_project -> max[GR])) return FALSE;
  return TRUE;
}

/*!
  \fn void edit_bonds (GtkWidget * vbox)

  \brief creation of the edit bond cutoff widgets

  \param vbox the GtkWidget to store the data
*/
void edit_bonds (GtkWidget * vbox)
{
  gchar * mess[2] = {"To define the existence of a bond between two atoms i (&#x3B1;) and j (&#x3B2;), ",
                     "a bond exits if the two following conditions are verified:"};
  gchar * cond[2] = {"1) D<sub>ij</sub> &#x3c; first minimum of the total g(r) - r<sub>cut</sub> (Tot.)",
                     "2) D<sub>ij</sub> &#x3c; first minimum of the partial gr<sub>&#x3B1;,&#x3B2;</sub>(r) - r<sub>cut</sub> (&#x3B1;,&#x3B2;)"};
  gchar * m_end= "0.0 &#x3c; r<sub>cut</sub> &#x2264; D<sub>max</sub>";
  gchar * fin = "D<sub>max</sub> is the maximum inter-atomic distance in the model\n";
  gchar * str;
  if (! preferences)
  {
    if (active_project -> max[0] != 0.0)
    {
      str = g_strdup_printf ("%s\twith\tD<sub>max</sub> = %f  &#xC5;",
                             m_end, active_project -> max[0]);
    }
    else
    {
      str = g_strdup_printf ("With %s", m_end);
    }
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(mess[0], -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(mess[1], -1, -1, 0.5, 0.5), FALSE, FALSE, 5);
  GtkWidget * vvbox = create_vbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, vvbox, FALSE, FALSE, 20);
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(cond[0], -1, -1, 0.0, 0.5), FALSE, FALSE, 50);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(cond[1], -1, -1, 0.0, 0.5), FALSE, FALSE, 50);
  if (! preferences)
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, vbox, markup_label(str, -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, vbox, markup_label(fin, -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
    g_free (str);
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    GtkWidget * boxv = create_vbox (BSEP);
    cut_box (active_project, boxv);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, boxv, FALSE, FALSE, 50);
  }
}

/*!
  \fn G_MODULE_EXPORT void run_on_edit_activate (GtkDialog * win, gint response_id, gpointer data)

  \brief create an edition dialog: run the dialog

  \param win the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_on_edit_activate (GtkDialog * win, gint response_id, gpointer data)
{
  int id = GPOINTER_TO_INT(data);
  int i = (id < 2) ? id : (id > 2 && id < 5) ? 1 : 2;

  gboolean done = FALSE;
  char * errpbc ="To apply PBC or use fractional coordinates\n"
                 "describe properly the simulation box parameters";
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (i== 0)
      {
        test_chem ();
        done = TRUE;
      }
      else if (i == 1)
      {
        done = test_pbc (tmp_pbc, tmp_frac, tmp_lattice, tmp_vect);
        if (done)
        {
          prep_box (i);
          if (id == 1)
          {
            if (! active_project -> run)
            {
              run_project ();
              if (active_glwin) active_glwin -> create_shaders[MDBOX] = TRUE;
              bonds_update = 1;
              frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
              mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
              active_project -> runc[0] = FALSE;
              on_calc_bonds_released (NULL, NULL);
            }
          }
        }
        else
        {
          show_warning (errpbc, GTK_WIDGET(win));
        }
      }
      else
      {
        gboolean upc;
        if (active_project -> modelgl)
        {
          if (active_project -> modelgl -> rings || active_project -> modelgl -> chains)
          {
            upc = ask_yes_no ("Data can be lost !", "You will lose\n rings statistics and/or chains statistics data\nProceed anyway ?", GTK_MESSAGE_WARNING, GTK_WIDGET(win));
          }
          else
          {
            upc = TRUE;
          }
        }
        else
        {
          upc = TRUE;
        }
        if (upc) update_cutoffs (active_project);
        done = TRUE;
      }
      break;
    default:
      if (i == 0)
      {
        test_chem ();
        done = TRUE;
      }
      else if (i == 1)
      {
        done = test_pbc (active_cell -> pbc,
                         active_cell -> frac,
                         active_box -> param,
                         active_box -> vect);
      }
      else
      {
        done = TRUE;
      }
      break;
  }
  if (done) destroy_this_dialog (win);
}

/*!
  \fn G_MODULE_EXPORT void on_edit_activate (GtkWidget * widg, gpointer data)

  \brief create an edition dialog - prepare the dialog

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_edit_activate (GtkWidget * widg, gpointer data)
{
  int i;
  gboolean skip = FALSE;
  int id = GPOINTER_TO_INT(data);
  i = (id < 2) ? id : (id > 2 && id < 5) ? 1 : 2;
  GtkWidget * win = dialog_cancel_apply (edit_prop[i], MainWindow, FALSE);
  GtkWidget * box = dialog_get_content_area (win);
  gtk_box_set_homogeneous (GTK_BOX(box), FALSE);
  if (i == 0)
  {
    edit_chem(box);
  }
  else if (i == 1)
  {
    edit_box (box);
    if (id == 3)
    {
      skip = test_pbc (tmp_pbc, tmp_frac, tmp_lattice, tmp_vect);
      if (skip)
      {
        active_cell -> ltype = tmp_lat;
        init_box_calc ();
      }
      active_project -> run = 0;
    }
  }
  else
  {
    opengl_project_changed (activep);
    edit_bonds (box);
    if (id == 5) skip = test_cutoffs ();
  }
  if (! skip && ! silent_input)
  {
    run_this_gtk_dialog (win, G_CALLBACK(run_on_edit_activate), data);
  }
#ifdef GTK3
  // GTK3 Menu Action To Check
  int j, k;
  if (active_glwin != NULL)
  {
    if (active_glwin -> init)
    {
      for (i=0; i<2; i++)
      {
        if (active_glwin -> ogl_box[i] != NULL)
        {
          widget_set_sensitive (active_glwin -> ogl_box[i], active_cell -> ltype);
          if (i)
          {
            j = (active_cell -> ltype && active_project -> steps == 1) ? 1 : 0;
            for (k=5; k<8; k++) widget_set_sensitive (active_glwin -> ogl_box[k], j);
          }
        }
      }
    }
  }
#endif
  if (tmpcut)
  {
    g_free (tmpcut);
    tmpcut = NULL;
  }
}
