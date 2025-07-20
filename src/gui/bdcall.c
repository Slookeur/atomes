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
* @file bdcall.c
* @short Callbacks for the bond properties calculation dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'bdcall.c'
*
* Contains:
*

 - The callbacks for the bond properties calculation dialog

*
* List of functions:

  int * save_color_map (glwin * view);

  gboolean run_distance_matrix (GtkWidget * widg, int calc, int up_ngb);

  double get_cutoff (double s_a, double s_b);

  void restore_color_map (glwin * view, int * colm);
  void recup_dmin_dmax_ (double * min, double * max);
  void initbd ();
  void initang ();
  void initcutoffs (chemical_data * chem, int species);
  void cutoffsend ();
  void prep_ogl_bonds ();
  void update_ang_view (project * this_proj);
  void update_glwin_after_bonds (int bonding, int * colm);
  void coordination_info (int sp, double sac, double ssac[active_project -> nspec]);
  void coordout_ (int * sid, double * sac, double ssac[active_project -> nspec], int * totgsa);
  void env_info (int sp, int totgsa, int numgsa[totgsa]);
  void update_angle_view (project * this_proj);
  void envout_ (int * sid, int * totgsa, int numgsa[* totgsa]);

  G_MODULE_EXPORT void on_calc_bonds_released (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"
#include "dlp_field.h"
#include "glview.h"
#include "initcoord.h"
#include "preferences.h"

extern G_MODULE_EXPORT void set_color_map (GtkWidget * widg, gpointer data);
extern void clean_coord_window (project * this_proj);
extern G_MODULE_EXPORT void set_filter_changed (GtkComboBox * box, gpointer data);

/*!
  \fn int * save_color_map (glwin * view)

  \brief save atoms and polyhedra color maps

  \param view the target glwin
*/
int * save_color_map (glwin * view)
{
  int i;
  int * colm = allocint(2);
  for (i=0; i<2; i++) colm[i] = view -> anim -> last -> img -> color_map[i];
  return colm;
}

/*!
  \fn void restore_color_map (glwin * view, int * colm)

  \brief restore saved color maps

  \param view the target glwin
  \param colm the saved color map values
*/
void restore_color_map (glwin * view, int * colm)
{
#ifdef GTK3
  // GTK3 Menu Action To Check
  int i, j;
  gboolean was_input = reading_input;
  reading_input = TRUE;
  for (i=0; i<2; i++)
  {
    if ((i == 3 || i == 4) && ! view -> adv_bonding[i-3])
    {
      j = i*ATOM_MAPS;
    }
    else
    {
      j = i*ATOM_MAPS + colm[i];
    }
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> color_styles[j], TRUE);
    set_color_map (view -> color_styles[j], & view -> colorp[j][0]);
  }
  reading_input = was_input;
#endif
}

/*!
  \fn void recup_dmin_dmax_ (double * min, double * max)

  \brief retrieve min and max inter-atomic distances from Fortran

  \param min the smallest inter-atomic distance
  \param max the highest inter-atomic distance
*/
void recup_dmin_dmax_ (double * min, double * max)
{
  active_project -> min[BD] = * min;
  active_project -> max[BD] = * max;
}

/*!
  \fn void initbd ()

  \brief initialize the curve widgets for the bond distribution
*/
void initbd ()
{
  int i, j, k;

  k = 0;
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    for ( j = 0 ; j < active_project -> nspec ; j++ )
    {
      active_project -> curves[BD][k] -> name = g_strdup_printf("Dij [%s-%s]",
                                                active_chem -> label[i],
                                                active_chem -> label[j]);
      k=k+1;
    }
  }
  addcurwidgets (activep, BD, 0);
  active_project -> initok[BD] = TRUE;
}

/*!
  \fn void initang ()

  \brief initialize the curve widgets for the angle distribution
*/
void initang ()
{
  int h, i, j, k, l;

  h=0;
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    for ( j = 0 ; j < active_project -> nspec ; j++ )
    {
      for ( k = 0 ; k < active_project -> nspec ; k++ )
      {
        active_project -> curves[AN][h] -> name = g_strdup_printf("Angles [%s-%s-%s]",
                                                  active_chem -> label[i],
                                                  active_chem -> label[j],
                                                  active_chem -> label[k]);
        h=h+1;
      }
    }
  }
  for ( i = 0 ; i < active_project -> nspec ; i++ )
  {
    for ( j = 0 ; j < active_project -> nspec ; j++ )
    {
      for ( k = 0 ; k < active_project -> nspec ; k++ )
      {
        for ( l = 0 ; l < active_project -> nspec ; l++ )
        {
          active_project -> curves[AN][h] -> name = g_strdup_printf("Dihedral [%s-%s-%s-%s]",
                                                    active_chem -> label[i], active_chem -> label[j],
                                                    active_chem -> label[k], active_chem -> label[l]);
          h=h+1;
        }
      }
    }
  }
  addcurwidgets (activep, AN, 0);
  active_project -> initok[AN] = TRUE;
}

/*!
  \fn double get_cutoff (double s_a, double s_b)

  \param retrieve cutoff radius between spec a and spec b

  \param s_a species a
  \param s_b species b
*/
double get_cutoff (double s_a, double s_b)
{
  if (default_bond_cutoff)
  {
    bond_cutoff * cut = default_bond_cutoff;
    int z_a = min (s_a, s_b);
    int z_b = max (s_a, s_b);
    while (cut)
    {
      if (cut -> Z[0] == z_a && cut -> Z[1] == z_b) return cut -> cutoff;
      cut = cut -> next;
    }
    return 0.0;
  }
  else
  {
    return 0.0;
  }
}

/*!
  \fn void initcutoffs (chemical_data * chem, int species)

  \brief initialize bond cutoffs

  \param chem the target chemical data
  \param species the number of chemical species
*/
void initcutoffs (chemical_data * chem, int species)
{
  int i, j;
  for (i = 0; i < species; i++)
  {
    chem -> cutoffs[i][i] = (chem -> cutoffs[i][i] != 0.0) ? chem -> cutoffs[i][i] : get_cutoff (chem -> chem_prop[CHEM_Z][i], chem -> chem_prop[CHEM_Z][i]);
    if (chem -> cutoffs[i][i] == 0.0)
    {
      chem -> cutoffs[i][i] = 2.0*chem -> chem_prop[CHEM_R][i];
      chem -> cutoffs[i][i] = max(MINCUT, chem -> cutoffs[i][i]);
      if (chem -> chem_prop[CHEM_Z][i] == 1.0) chem -> cutoffs[i][i] = 0.5;
    }
  }
  for (i = 0; i < species-1; i++)
  {
    for (j = i+1; j < species; j++)
    {
      chem -> cutoffs[i][j] = (chem -> cutoffs[i][j] != 0.0) ? chem -> cutoffs[i][j] : get_cutoff (chem -> chem_prop[CHEM_Z][i], chem -> chem_prop[CHEM_Z][j]);
      if (chem -> cutoffs[i][j] == 0.0)
      {
        chem -> cutoffs[i][j] = chem -> chem_prop[CHEM_R][i] + chem -> chem_prop[CHEM_R][j];
        chem -> cutoffs[i][j] = max(MINCUT, chem -> cutoffs[i][j]);
        if (chem -> chem_prop[CHEM_Z][i] == 1.0 && chem -> chem_prop[CHEM_Z][j] == 6.0) chem -> cutoffs[i][j] = 1.2;
        if (chem -> chem_prop[CHEM_Z][j] == 1.0 && chem -> chem_prop[CHEM_Z][i] == 6.0) chem -> cutoffs[j][i] = 1.2;
        if (chem -> chem_prop[CHEM_Z][i] == 1.0 && chem -> chem_prop[CHEM_Z][j] == 8.0) chem -> cutoffs[i][j] = 1.2;
        if (chem -> chem_prop[CHEM_Z][j] == 1.0 && chem -> chem_prop[CHEM_Z][i] == 8.0) chem -> cutoffs[j][i] = 1.2;
      }
      chem -> cutoffs[j][i] = chem -> cutoffs[i][j];
    }
  }
  chem -> grtotcutoff = (chem -> grtotcutoff != 0.0) ? chem -> grtotcutoff : default_totcut;
  if (chem -> grtotcutoff == 0.0)
  {
    for (i = 0; i < species; i++)
    {
      chem -> grtotcutoff += chem -> chem_prop[CHEM_R][i];
    }
    chem -> grtotcutoff *= 2.0;
    chem -> grtotcutoff /= species;
    chem -> grtotcutoff = max(MINCUT, chem -> grtotcutoff);
  }
}

/*!
  \fn void cutoffsend ()

  \brief send cutoffs to Fortran90
*/
void cutoffsend ()
{
  int i, j;

  if (active_chem -> cutoffs == NULL)
  {
    // g_debug ("alloc cuttofs !!!");
    active_chem -> cutoffs = allocddouble (active_project -> nspec, active_project -> nspec);
  }
  for ( i=0; i < active_project -> nspec; i++)
  {
    for (j=0; j < active_project -> nspec; j++)
    {
      // g_debug ("cut[%d,%d]= %f", i+1, j+1, active_chem -> cutoffs[i][j]);
      sendcuts_ (& i, & j, & active_chem -> cutoffs[i][j]);
    }
  }
  i = active_project -> nspec;
  // g_debug ("totcut= %f", active_chem -> grtotcutoff);
  sendcuts_ (& i, & i, & active_chem -> grtotcutoff);
}

/*!
  \fn void prep_ogl_bonds ()

  \brief initialize bond pointers
*/
void prep_ogl_bonds ()
{
  int i;
  active_glwin -> bonding = FALSE;
#ifdef GTK3
  // GTK3 Menu Action To Check
  for (i=0; i<ATOM_MAPS+POLY_MAPS; i++)
  {
    widget_set_sensitive (active_glwin -> color_styles[i], 0);
  }
  for (i=1; i<OGL_COORDS; i++)
  {
    widget_set_sensitive (active_glwin -> ogl_coord[i], 0);
  }
#endif
  active_image -> color_map[0] = 0;
  active_image -> color_map[1] = 0;
  for (i=0; i<4; i++)
  {
    if (active_glwin -> gcid[i]) g_free (active_glwin -> gcid[i]);
    active_glwin -> gcid[i] = NULL;
    active_coord -> totcoord[i] = 0;
  }
  if (! active_project -> dmtx)
  {
    active_glwin -> allbonds[0] = 0;
    active_glwin -> allbonds[1] = 0;
  }
  for (i=0; i<2; i++) active_glwin -> adv_bonding[i] = FALSE;
}

/*!
  \fn gboolean run_distance_matrix (GtkWidget * widg, int calc, int up_ngb)

  \brief compute distance matrix

  \param widg the GtkWidget sending the signal, if any
  \param calc the calculation that requires the analysis
  \param up_ngb update neighbors information (0 = no, 1 = yes)
*/
gboolean run_distance_matrix (GtkWidget * widg, int calc, int up_ngb)
{
  int i, j, k;
  gboolean res;
  if (up_ngb)
  {
    for (i=0; i < active_project -> steps; i++)
    {
      for (j=0; j < active_project -> natomes; j++)
      {
        active_project -> atoms[i][j].cloned = FALSE;
        if (active_project -> atoms[i][j].numv)
        {
          active_project -> atoms[i][j].numv = 0;
          if (active_project -> atoms[i][j].vois)
          {
            g_free (active_project -> atoms[i][j].vois);
            active_project -> atoms[i][j].vois = NULL;
          }
        }
      }
    }
  }
  prepostcalc (widg, FALSE, -1, (active_project -> steps > 1) ? 1 : 0, opac);
  i = j = 0;
  k = up_ngb;;
  if (calc > 3 && calc < 6) i = 1;
  if (calc > 0 && calc < 6)
  {
    j = active_project -> rsparam[calc-1][4];
  }
  else if (calc == 6)
  {
    j = active_project -> csparam[3];
  }
#ifdef DEBUG
  g_debug ("Run dmtx Prim= %d, NOHP= %d, UPDATE= %d", i, j, k);
#endif
  clock_gettime (CLOCK_MONOTONIC, & start_time);
  res = rundmtx_ (& i, & j, & k);
  prepostcalc (widg, TRUE, -1, 0, 1.0);
  clock_gettime (CLOCK_MONOTONIC, & stop_time);
  g_print ("Time to calculate distance matrix: %s\n", calculation_time(FALSE, get_calc_time (start_time, stop_time)));
  return res;
}

/*!
  \fn void update_ang_view (project * this_proj)

  \brief update angle calculation text buffer

  \param this_proj the target project
*/
void update_ang_view (project * this_proj)
{
  gchar * str;
  if (this_proj -> text_buffer[AN+OT] == NULL) this_proj -> text_buffer[AN+OT] = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> text_buffer[AN+OT]);
  print_info ("\n\nAngles and diherdrals distribution(s)\n\n", "heading", this_proj -> text_buffer[AN+OT]);
  print_info ("Calculation details:\n\n", NULL, this_proj -> text_buffer[AN+OT]);
  print_info ("\tAngular space discretization:\n\n", NULL, this_proj -> text_buffer[AN+OT]);
  print_info ("\t - Number of δ° steps: ", "bold", this_proj -> text_buffer[AN+OT]);
  str = g_strdup_printf ("%d", this_proj -> num_delta[AN]);
  print_info (str, "bold_blue", this_proj -> text_buffer[AN+OT]);
  g_free (str);
  print_info ("\n\n\t between 0.0 and 180.0", NULL, this_proj -> text_buffer[AN+OT]);
  print_info (" °\n\n\t - δ° = ", "bold", this_proj -> text_buffer[AN+OT]);
  str = g_strdup_printf ("%f", this_proj -> delta[AN]);
  print_info (str, "bold_blue", this_proj -> text_buffer[AN+OT]);
  g_free (str);
  print_info (" °\n", "bold", this_proj -> text_buffer[AN+OT]);
  print_info (calculation_time(TRUE, this_proj -> calc_time[AN]), NULL, this_proj -> text_buffer[AN+OT]);
}

/*!
  \fn void update_glwin_after_bonds (int bonding, int * colm)

  \brief update glwin menus after bond calculation

  \param bonding calculation result (0 = failure, 1 = success)
  \param colm saved color map to restore
*/
void update_glwin_after_bonds (int bonding, int * colm)
{
  active_glwin -> bonding = bonding;
#ifdef GTK3
  // GTK3 Menu Action To Check
  if (active_glwin -> init)
  {
    prep_all_coord_menus (active_glwin);
    set_advanced_bonding_menus (active_glwin);
    widget_set_sensitive (active_glwin -> ogl_clones[0], active_glwin -> allbonds[1]);
  }
#endif
  int shaders[5] = {ATOMS, BONDS, POLYS, RINGS, SELEC};
  re_create_md_shaders (5, shaders, active_project);
  active_glwin -> create_shaders[MEASU] = TRUE;
  active_glwin -> create_shaders[PICKS] = TRUE;
  clean_coord_window (active_project);
  if (active_glwin -> init)
  {
    restore_color_map (active_glwin, colm);
    g_free (colm);
  }

  int i, j;
  for (i=2; i<7; i++)
  {
    if (i != 5)
    {
      if (active_glwin -> search_widg[i])
      {
        if (active_glwin -> search_widg[i] -> filter_box)
        {
          if (GTK_IS_WIDGET(active_glwin -> search_widg[i] -> filter_box))
          {
            if (active_glwin -> atom_win -> adv_bonding[1] && ! active_glwin -> adv_bonding[1])
            {
              gtk_combo_box_text_remove ((GtkComboBoxText *) active_glwin -> search_widg[i] -> filter_box, 4);
            }
            else if (! active_glwin -> atom_win -> adv_bonding[0] && active_glwin -> adv_bonding[0])
            {
              combo_text_append (active_glwin -> search_widg[i] -> filter_box, "Fragment");
            }
            if (active_glwin -> atom_win -> adv_bonding[0] && ! active_glwin -> adv_bonding[0])
            {
              gtk_combo_box_text_remove ((GtkComboBoxText *) active_glwin -> search_widg[i] -> filter_box, 3);
            }
            else if (! active_glwin -> atom_win -> adv_bonding[1] && active_glwin -> adv_bonding[1])
            {
              combo_text_append (active_glwin -> search_widg[i] -> filter_box, "Molecule");
            }
            j = active_glwin -> search_widg[i] -> object + active_glwin -> search_widg[i] -> filter;
            if (j == 4)
            {
              set_filter_changed (GTK_COMBO_BOX(active_glwin -> search_widg[i] -> filter_box), active_glwin -> search_widg[i]);
            }
          }
        }
      }
    }
  }
  if (active_glwin -> atom_win)
  {
    for (i=0; i<2; i++) active_glwin -> atom_win -> adv_bonding[i] = active_glwin -> adv_bonding[i];
  }
  clean_volumes_data (active_glwin);
#ifdef GTK4
  update_menu_bar (active_glwin);
#endif
  update (active_glwin);
}

/*!
  \fn G_MODULE_EXPORT void on_calc_bonds_released (GtkWidget * widg, gpointer data)

  \brief compute bonding properties

  \param widg the GtkWidget sending the signal, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_bonds_released (GtkWidget * widg, gpointer data)
{
  int j, k, l, m;
  int statusb = 0;
  int bonding = 0;
  int * colm = NULL;
  gboolean vis_bd = active_project -> visok[BD];

  if (widg) bonds_update = 1;
  bonding = (active_project -> runc[0]) ? 1 : 0;
  if (! bonds_update && active_project -> runc[0]) bonding = 0;
  if (active_glwin)
  {
    if (active_glwin -> init) colm = save_color_map (active_glwin);
    prep_ogl_bonds ();
  }
  cutoffsend ();
  if (! active_project -> dmtx) active_project -> dmtx = run_distance_matrix (widg, 0, 1);

  if (active_project -> dmtx)
  {
    //if (bonding && active_project -> steps > 1) statusb = 1;
    if (bonds_update || active_project -> runc[0] || active_project -> runc[2])
    {
      if (! active_project -> initok[BD] && bonding) initbd ();
      if (active_project -> runc[0]) clean_curves_data (BD, 0, active_project -> numc[BD]);
      prepostcalc (widg, FALSE, BD, statusb, opac);
      l = 0;
      m = 1;
      if (active_project -> bondfile != NULL)
      {
        l = 1;
        m = strlen (active_project -> bondfile);
      }
      // debug_chemical_information (active_project);
      active_project -> delta[BD] = (active_project -> max[BD]-active_project -> min[BD]) / active_project -> num_delta[BD];
      clock_gettime (CLOCK_MONOTONIC, & start_time);
      j = bonding_ (& m, & l, & bonding, & active_project -> num_delta[BD], & active_project -> min[BD], & active_project -> delta[BD], active_project -> bondfile);
      clock_gettime (CLOCK_MONOTONIC, & stop_time);
      active_project -> calc_time[BD] = get_calc_time (start_time, stop_time);
      active_project -> runok[SP] = j;
      prepostcalc (widg, bonding, BD, (bonding) ? j : vis_bd, 1.0);
      if (! j)
      {
        show_error ("Unexpected error when calculating bond properties", 0, (widg) ? widg : MainWindow);
      }
      else
      {
        if (active_glwin -> init) print_info (calculation_time(TRUE, active_project -> calc_time[BD]), NULL, active_project -> text_buffer[BD+OT]);
        bonding = 1;
        if (frag_update)
        {
          prepostcalc (widg, FALSE, -1, statusb, opac);
          k = active_glwin -> allbonds[0] + active_glwin -> allbonds[1];
          clock_gettime (CLOCK_MONOTONIC, & start_time);
          if (! molecules_ (& mol_update, & k))
          {
            show_error ("Unexpected error when looking for isolated fragment(s) and molecule(s)", 0, (widg) ? widg : MainWindow);
            if (active_glwin)
            {
              for (k=0; k<2; k++)
              {
                active_glwin -> adv_bonding[k] = FALSE;
                if (k)
                {
                  for (l=0; l<2; l++)
                  {
                    if (active_project -> force_field[l])
                    {
                      g_free (active_project -> force_field[l]);
                      active_project -> force_field[l] = NULL;
                    }
                  }
                }
                active_coord -> totcoord[k+2] = 0;
              }
            }
          }
          else
          {
            if (active_glwin)
            {
              active_glwin -> adv_bonding[0] = frag_update;
              active_glwin -> adv_bonding[1] = mol_update;
            }
          }
          clock_gettime (CLOCK_MONOTONIC, & stop_time);
          // Using the RI slot to store Frag-mol calc time.
          active_project -> calc_time[RI] = get_calc_time (start_time, stop_time);
          active_project_changed (activep);
          prepostcalc (widg, TRUE, -1, statusb, 1.0);
          if (widg != NULL) show_the_widgets (curvetoolbox);
        }
        else
        {
          active_glwin -> adv_bonding[0] = frag_update;
          active_glwin -> adv_bonding[1] = mol_update;
        }
      }
    }
    if (active_project -> runc[1])
    {
      if (! active_project -> initok[AN]) initang ();
      clean_curves_data (AN, 0, active_project -> numc[AN]);
      prepostcalc (widg, FALSE, AN, statusb, opac);
      active_project -> delta[AN] = 180.0 / active_project -> num_delta[AN];
      clock_gettime (CLOCK_MONOTONIC, & start_time);
      j = bond_angles_ (& active_project -> num_delta[AN]);
      if (! j)
      {
        show_error ("Unexpected error when calculating the bond angles distribution", 0, (widg) ? widg : MainWindow);
      }
      else
      {
        j = bond_diedrals_ (& active_project -> num_delta[AN]);
        clock_gettime (CLOCK_MONOTONIC, & stop_time);
        active_project -> calc_time[AN] = get_calc_time (start_time, stop_time);
        if (! j)
        {
          show_error ("Unexpected error when calculating the dihedral angles distribution", 0, (widg) ? widg : MainWindow);
        }
        else
        {
          if (widg != NULL) show_the_widgets (curvetoolbox);
          if (! active_project -> runc[0]) update_ang_view (active_project);
        }
      }
      prepostcalc (widg, TRUE, AN, j, 1.0);
    }
  }
  else
  {
    show_error ("The nearest neighbors table calculation has failed", 0, (widg) ? widg : MainWindow);
    bonding = 0;
    active_glwin -> adv_bonding[0] = 0;
    active_glwin -> adv_bonding[1] = 0;
  }
  if (active_glwin && bonds_update) update_glwin_after_bonds (bonding, colm);
  fill_tool_model ();
  if (widg)
  {
    show_the_widgets (curvetoolbox);
  }
  else
  {
    for (j=0; j<3; j++) active_project -> runc[j] = FALSE;
  }
  bonds_update = frag_update = mol_update = 0;
}

double bdtc;

/*!
  \fn void coordination_info (int sp, double sac, double ssac[active_project->nspec])

  \brief print out coordination information

  \param sp the target chemical species
  \param sac total coordination number for the target species
  \param ssac partial coordination number(s) for the target species
*/
void coordination_info (int sp, double sac, double ssac[active_project -> nspec])
{
  int j;
  gchar * str;
  gchar * spr;
  if (active_project -> text_buffer[BD+OT] == NULL) active_project -> text_buffer[BD+OT] = add_buffer (NULL, NULL, NULL);
  view_buffer (active_project -> text_buffer[BD+OT]);
  if (sp == 0)
  {
    print_info ("\n\nBond properties\n\n", "heading", active_project -> text_buffer[BD+OT]);
    print_info ("Existence of a bond between two atoms i (α) and j (β)\n"
                "if the two following conditions are verified:\n\n"
                "\t1) D", "italic", active_project -> text_buffer[BD+OT]);
    print_info ("ij", "sub_italic", active_project -> text_buffer[BD+OT]);
    str = g_strdup_printf (" < first minimum of the total RDF (%9.5f Å )\n\t2) D", active_chem -> grtotcutoff);
    print_info (str, "italic", active_project -> text_buffer[BD+OT]);
    g_free (str);
    print_info ("ij", "sub_italic", active_project -> text_buffer[BD+OT]);
    print_info (" < r", "italic", active_project -> text_buffer[BD+OT]);
    print_info ("cut", "sub_italic", active_project -> text_buffer[BD+OT]);
    print_info ("(α,β)\n", "italic", active_project -> text_buffer[BD+OT]);
    bdtc = sac * active_chem -> nsps[sp];
  }
  else
  {
    bdtc += sac * active_chem -> nsps[sp];
  }
  print_info ("\nCoordination numbers: ", "italic", active_project -> text_buffer[BD+OT]);
  spr = g_strdup_printf("%s", textcolor(sp));
  print_info (active_chem -> element[sp], spr, active_project -> text_buffer[BD+OT]);
  str = g_strdup_printf ("\n\n\t%s", exact_name(active_chem -> label[sp]));
  print_info (str, spr, active_project -> text_buffer[BD+OT]);
  g_free (str);
  print_info (" (total)=\t", "italic", active_project -> text_buffer[BD+OT]);
  str = g_strdup_printf("%9.5lf\n", sac);
  print_info (str, "bold", active_project -> text_buffer[BD+OT]);
  g_free (str);
  for ( j=0 ; j < active_project -> nspec ; j++ )
  {
    str = g_strdup_printf ("\t%s(", active_chem -> label[sp]);
    print_info (str, spr, active_project -> text_buffer[BD+OT]);
    g_free (str);
    str = g_strdup_printf("%s", textcolor(j));
    print_info (active_chem -> label[j], str, active_project -> text_buffer[BD+OT]);
    print_info (")", spr, active_project -> text_buffer[BD+OT]);
    print_info ("[r", NULL, active_project -> text_buffer[BD+OT]);
    print_info ("cut", "sub", active_project -> text_buffer[BD+OT]);
    print_info ("(", NULL, active_project -> text_buffer[BD+OT]);
    print_info (active_chem -> label[sp], spr, active_project -> text_buffer[BD+OT]);
    print_info (",", NULL, active_project -> text_buffer[BD+OT]);
    print_info (active_chem -> label[j], str, active_project -> text_buffer[BD+OT]);
    g_free (str);
    print_info (")= ", NULL, active_project -> text_buffer[BD+OT]);
    str = g_strdup_printf ("%9.5lf Å", active_chem -> cutoffs[sp][j]);
    print_info (str, NULL, active_project -> text_buffer[BD+OT]);
    g_free (str);
    print_info ("] = ", NULL, active_project -> text_buffer[BD+OT]);
    str = g_strdup_printf ("%9.5lf", ssac[j]);
    print_info (str, "bold", active_project -> text_buffer[BD+OT]);
    g_free (str);
    print_info ("\t or \t", NULL, active_project -> text_buffer[BD+OT]);
    if (sac != 0.0)
    {
      str = g_strdup_printf ("%7.3lf", ssac[j]*100.0/(sac));
    }
    else
    {
      str = g_strdup_printf ("%7.3lf", 0.0);
    }
    print_info (str, "bold", active_project -> text_buffer[BD+OT]);
    g_free (str);
    print_info (" %\n", NULL, active_project -> text_buffer[BD+OT]);
  }
  g_free (spr);
  if (sp == active_project -> nspec-1)
  {
    print_info ("\nAverage coordination number: ", "italic", active_project -> text_buffer[BD+OT]);
    str = g_strdup_printf ("%9.5lf\n", bdtc / active_project -> natomes);
    print_info (str, "bold", active_project -> text_buffer[BD+OT]);
    g_free (str);
  }
}

/*!
  \fn void coordout_ (int * sid, double * sac, double ssac[active_project->nspec], int * totgsa)

  \brief retrieve partial geometry information from Fortran90

  \param sid the target chemical species
  \param sac total coordination number for the target species
  \param ssac partial coordination number(s) for the target species
  \param totgsa the total number of partial coordination for the target chemical species
*/
void coordout_ (int * sid, double * sac, double ssac[active_project -> nspec], int * totgsa)
{
  active_coord -> ntg[1][* sid] = * totgsa;
  if (bonds_update) coordination_info (* sid, * sac, ssac);
}

/*void wccp_out_ (double cp[5])
{
  double x, y, z;
  gchar * str;
  print_info ("Warren-Cowley chemical order parameters:\n\n", "italic", active_project -> text_buffer[BD+OT]);
  print_info ("\tα", NULL, active_project -> text_buffer[BD+OT]);
  print_info ("w", "sub", active_project -> text_buffer[BD+OT]);
  print_info ("=\t", NULL, active_project -> text_buffer[BD+OT]);
  x = 0.0;
  x += (cp[1] * active_chem -> nsps[0])/active_project -> natomes;
  x += (cp[0] * active_chem -> nsps[1])/active_project -> natomes;
  y = 1.0 - cp[2] / (cp[3] * x);
  str = g_strdup_printf ("%f\n", y);
  print_info (str, "bold", active_project -> text_buffer[BD+OT]);
  if ((cp[0] * active_chem -> nsps[0])/active_project -> natomes > (cp[1] * active_chem -> nsps[1])/active_project -> natomes)
  {
    z = 1.0 - cp[0] / (active_chem -> nsps[1] /(active_project -> natomes * y));
  }
  else
  {
    z = 1.0 - cp[1] / (active_chem -> nsps[0] /(active_project -> natomes * y));
  }
  print_info ("\tα", NULL, active_project -> text_buffer[BD+OT]);
  print_info ("w", "sub", active_project -> text_buffer[BD+OT]);
  print_info ("0", "sup", active_project -> text_buffer[BD+OT]);
  print_info ("=\t", NULL, active_project -> text_buffer[BD+OT]);
  str = g_strdup_printf ("%f\n", y/z);
  print_info (str, "bold", active_project -> text_buffer[BD+OT]);
  print_info ("Cargill-Spaepen chemical order parameters:\n\n", "italic", active_project -> text_buffer[BD+OT]);
  // ρ
  print_info ("\tη=\t", NULL, active_project -> text_buffer[BD+OT]);
  x = cp[2] * cp[4] / (active_chem -> nsps[0]/active_project -> natomes * cp[0] * cp[1]) - 1.0;
  str = g_strdup_printf ("%f\n", x);
  print_info (str, "bold", active_project -> text_buffer[BD+OT]);
  if ((cp[0] * active_chem -> nsps[0])/active_project -> natomes > (cp[1] * active_chem -> nsps[1])/active_project -> natomes)
  {
    z = ;
  }
  else
  {
    z = ;
  }
  print_info ("\tη", NULL, active_project -> text_buffer[BD+OT]);
  print_info ("0", "sup", active_project -> text_buffer[BD+OT]);
  print_info ("=\t", NULL, active_project -> text_buffer[BD+OT]);
  str = g_strdup_printf ("%f\n", y/z);
  print_info (str, "bold", active_project -> text_buffer[BD+OT]);
}*/

/*!
  \fn void env_info (int sp, int totgsa, int numgsa[totgsa])

  \brief output environment information for target chemical species in text buffer

  \param sp the target chemcial species
  \param totgsa the total number of partial coordination(s)
  \param numgsa the number of coordination(s) by coordination type
*/
void env_info (int sp, int totgsa, int numgsa[totgsa])
{
  int i, j, k;
  int natpg[totgsa];
  gchar * str, * spr, * snr;
  int tgsa;
  print_info ("\nEnvironments for ", "italic", active_project -> text_buffer[BD+OT]);
  spr = g_strdup_printf ("%s", textcolor(sp));
  str = g_strdup_printf ("%s", exact_name(active_chem -> label[sp]));
  print_info (str, spr, active_project -> text_buffer[BD+OT]);
  g_free (spr);
  g_free (str);
  print_info (" atoms:\n\n", "italic", active_project -> text_buffer[BD+OT]);
  print_info ("\t            \tN(tot)", NULL, active_project -> text_buffer[BD+OT]);
  for ( j=0 ; j < active_project -> nspec ; j++ )
  {
    snr = g_strdup_printf ("%s", exact_name(active_chem -> label[j]));
    i = 6 - strlen (snr);
    for (k=0; k<i; k++) print_info (" ", NULL, active_project -> text_buffer[BD+OT]);
    str = g_strdup_printf ("N(%s)", snr);
    spr = g_strdup_printf ("%s", textcolor(j));
    print_info (str, spr, active_project -> text_buffer[BD+OT]);
    g_free (spr);
    g_free (str);
    g_free (snr);
  }
  print_info ("\tNumber\t\t or \tPercent\n\n", NULL, active_project -> text_buffer[BD+OT]);

  tgsa = 0;
  for ( i=0 ; i < totgsa; i++ )
  {
    tgsa += numgsa[i];
  }
  for ( i=0 ; i < totgsa; i++ )
  {
    natpg[i] = 0;
    for ( j=0 ; j < active_project -> nspec ; j++ )
    {
      k = active_coord -> partial_geo[sp][i][j];
      natpg[i] += k;
    }
    print_info ("\t", NULL, active_project -> text_buffer[BD+OT]);
    spr = g_strdup_printf ("%s", env_name (active_project, i, sp, 1, active_project -> text_buffer[BD+OT]));
    g_free (spr);
    spr = g_strdup_printf ("%s", exact_name(env_name (active_project, i, sp, 0, NULL)));
    k = 12 - strlen (spr);
    g_free (spr);
    for (j=0; j<k; j++) print_info (" ", NULL, active_project -> text_buffer[BD+OT]);
    print_info ("\t", NULL, active_project -> text_buffer[BD+OT]);
    str = g_strdup_printf ("%3d ", natpg[i]);
    print_info (str, NULL, active_project -> text_buffer[BD+OT]);
    for ( j=0 ; j < active_project -> nspec ; j++ )
    {
      k = active_coord -> partial_geo[sp][i][j];
      str = g_strdup_printf("  %7d", k);
      spr = g_strdup_printf ("%s", textcolor(j));
      print_info (str, spr, active_project -> text_buffer[BD+OT]);
      g_free (str);
      g_free (spr);
    }
    str = g_strdup_printf("  %16.5lf", (1.0*numgsa[i])/active_project -> steps);
    print_info (str, "bold", active_project -> text_buffer[BD+OT]);
    g_free (str);
    print_info ("\t or \t", NULL, active_project -> text_buffer[BD+OT]);
    str = g_strdup_printf ("%7.3lf ", 100.0*numgsa[i]/tgsa);
    print_info (str, "bold", active_project -> text_buffer[BD+OT]);
    g_free (str);
    print_info ("%\n", "bold", active_project -> text_buffer[BD+OT]);
  }
}


/*!
  \fn void update_angle_view (project * this_proj)

  \brief update angle calculation information text buffer

  \param this_proj the target project
*/
void update_angle_view (project * this_proj)
{
  gchar * str;
  if (this_proj -> text_buffer[AN+OT] == NULL) this_proj -> text_buffer[AN+OT] = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> text_buffer[AN+OT]);
  print_info ("\n\nAngle distribution function(s)\n\n", "heading", this_proj -> text_buffer[AN+OT]);


  print_info ("\tAngle space discretization:\n\n", NULL, this_proj -> text_buffer[AN+OT]);
  print_info ("\t - Number of δ° steps: ", "bold", this_proj -> text_buffer[AN+OT]);
  str = g_strdup_printf ("%d", this_proj -> num_delta[AN]);
  print_info (str, "bold_blue", this_proj -> text_buffer[AN+OT]);
  g_free (str);
  print_info ("\n\n\t between 0.0 and 180.0°\n", NULL, this_proj -> text_buffer[AN+OT]);
}

/*!
  \fn void envout_ (int * sid, int * totgsa, int numgsa[*totgsa])

  \brief retrieve environment information for target chemical species from Fortran

  \param sid the target chemical speceis
  \param totgsa the total number of partial coordination(s)
  \param numgsa the number of coordination(s) by coordination type
*/
void envout_ (int * sid, int * totgsa, int numgsa[* totgsa])
{
  /* Send info for OpenGL */
  if (bonds_update) env_info (* sid, * totgsa, numgsa);
}

void tetraout_ (int * sid, double eda[active_project -> nspec],
                double cda[active_project -> nspec],
                double dda[active_project -> nspec],
                double tepa[active_project -> nspec],
                double tcpa[active_project -> nspec],
                double tdda[active_project -> nspec],
                double atd[active_project -> nspec],
                double etd[active_project -> nspec])
{
  int i;
  gboolean print;
  gchar * str;
  print=FALSE;
  for ( i=0 ; i < active_project -> nspec ; i++)
  {
    if (eda[i] != 0.0 || cda[i] != 0.0) print=TRUE;
  }

  if (print && bonds_update)
  {
    print_info ("\nNumber and proportion of tetrahedra links for ", "italic", active_project -> text_buffer[BD+OT]);
    print_info (exact_name(active_chem -> label[* sid]), textcolor(* sid), active_project -> text_buffer[BD+OT]);
    print_info (" atoms:\n\n", "italic", active_project -> text_buffer[BD+OT]);
    for ( i=0 ; i < active_project -> nspec ; i++ )
    {
      if (eda[i] != 0.0 || cda[i] != 0.0)
      {
        print_info ("\t- ", NULL, active_project -> text_buffer[BD+OT]);
        print_info (exact_name(active_chem -> label[* sid]), textcolor(* sid), active_project -> text_buffer[BD+OT]);
        print_info ("(", NULL, active_project -> text_buffer[BD+OT]);
        print_info (exact_name(active_chem -> label[i]), textcolor(i), active_project -> text_buffer[BD+OT]);
        print_info (")", NULL, active_project -> text_buffer[BD+OT]);
        print_info ("4", "sub", active_project -> text_buffer[BD+OT]);
        print_info (" tetrahedra:\n", NULL, active_project -> text_buffer[BD+OT]);
        if (eda[i] != 0.0)
        {
          print_info ("\t\t Edge-sharing:   ", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%11.5lf", eda[i]/active_project -> steps);
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %8.5lf", tepa[i]);
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info ("\t or \t", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%7.3lf", 100*eda[i]/(eda[i]+cda[i]));
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %7.3lf", 100*tepa[i]*active_project -> steps/(eda[i]+cda[i]));
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info (" %\n", "bold", active_project -> text_buffer[BD+OT]);
        }
        if (cda[i] != 0.0)
        {
          print_info ("\t\t Corner-sharing: ", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%11.5lf", cda[i]/active_project -> steps);
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %8.5lf", tcpa[i]);
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info ("\t or \t", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%7.3lf", 100*cda[i]/(eda[i]+cda[i]));
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %7.3lf", 100*tcpa[i]*active_project -> steps/(eda[i]+cda[i]));
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info (" %\n", "bold", active_project -> text_buffer[BD+OT]);
        }
        if (dda[i] != 0.0)
        {
          print_info ("\t\t with the following bond defects: ", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%11.5lf", dda[i]/active_project -> steps);
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %8.5lf", tdda[i]);
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info ("\t or \t", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%7.3lf", 100*dda[i]/(eda[i]+cda[i]));
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %7.3lf", 100*tdda[i]*active_project -> steps/(eda[i]+cda[i]));
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info (" %\n", "bold", active_project -> text_buffer[BD+OT]);
        }
      }
    }
    print_info ("\nNumber and proportion of tetrahedra units for ", "italic", active_project -> text_buffer[BD+OT]);
    print_info (exact_name(active_chem -> label[* sid]), textcolor(* sid), active_project -> text_buffer[BD+OT]);
    print_info (" atoms:\n\n", "italic", active_project -> text_buffer[BD+OT]);
    for ( i=0 ; i < active_project -> nspec ; i++ )
    {
      if (atd[i] != 0.0)
      {
        print_info ("\t- ", NULL, active_project -> text_buffer[BD+OT]);
        print_info (exact_name(active_chem -> label[* sid]), textcolor(* sid), active_project -> text_buffer[BD+OT]);
        print_info ("(", NULL, active_project -> text_buffer[BD+OT]);
        print_info (exact_name(active_chem -> label[i]), textcolor(i), active_project -> text_buffer[BD+OT]);
        print_info (")", NULL, active_project -> text_buffer[BD+OT]);
        print_info ("4", "sub", active_project -> text_buffer[BD+OT]);
        print_info (" tetrahedra:\n", NULL, active_project -> text_buffer[BD+OT]);
        print_info ("\t\t Total number of tetrahedra:", NULL, active_project -> text_buffer[BD+OT]);
        str = g_strdup_printf("%11.5lf", atd[i]/active_project -> steps);
        print_info (str, "bold", active_project -> text_buffer[BD+OT]);
        g_free (str);
        if (active_project -> steps > 1)
        {
          str = g_strdup_printf(" +/- %8.5lf", etd[i]);
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
        }
        print_info ("\n", "bold", active_project -> text_buffer[BD+OT]);
        if (eda[i] != 0.0 )
        {
          print_info ("\t\t Edge-sharing:   ", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%11.5lf", 2.0*eda[i]/active_project -> steps);
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %8.5lf", 2.0*tepa[i]);
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info ("\t or \t", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%7.3lf", 2.0*100*eda[i]/(eda[i]+cda[i]));
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %7.3lf", 2.0*100*tepa[i]*active_project -> steps/(eda[i]+cda[i]));
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info (" %\n", "bold", active_project -> text_buffer[BD+OT]);
        }
        if (cda[i] != 0.0 )
        {

          print_info ("\t\t Corner-sharing: ", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%11.5lf", (atd[i]-2*eda[i])/active_project -> steps);
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %8.5lf", 2.0*tepa[i]);
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info ("\t or \t", NULL, active_project -> text_buffer[BD+OT]);
          str = g_strdup_printf("%7.3lf", 100.0*(atd[i]-2.0*eda[i])/atd[i]);
          print_info (str, "bold", active_project -> text_buffer[BD+OT]);
          g_free (str);
          if (active_project -> steps > 1)
          {
            str = g_strdup_printf(" +/- %7.3lf", 2.0*100.0*tepa[i]*active_project -> steps/atd[i]);
            print_info (str, "bold", active_project -> text_buffer[BD+OT]);
            g_free (str);
          }
          print_info (" %\n", "bold", active_project -> text_buffer[BD+OT]);
        }
      }
    }
  }
}
