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
* @file ringscall.c
* @short Callbacks for the ring statistics calculation dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'ringscall.c'
*
* Contains:
*

 - The callbacks for the ring statistics calculation dialog

*
* List of functions:

  void init_ring (project * this_proj);
  void update_rings_menus (glwin * view);
  void update_rings_view (project * this_proj, int c);
  void clean_rings_data (int rid, glwin * view);
  void save_rings_data_ (int * taille,
                         double ectrc[* taille],
                         double ectpna[* taille],
                         double ectmax[* taille],
                         double ectmin[* taille],
                         double * rpstep, double * ectrpst,
                         double * nampat, double * ectampat);

  G_MODULE_EXPORT void on_calc_rings_released (GtkWidget * widg, gpointer data);

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "glview.h"
#include "curve.h"
#include "initcoord.h"

extern GtkWidget * prep_rings_menu (glwin * view, int id);
extern gboolean run_distance_matrix (GtkWidget * widg, int calc, int up_ngb);
extern void clean_coord_window (project * this_proj);
#ifdef GTK3
extern G_MODULE_EXPORT void show_hide_poly (GtkWidget * widg, gpointer data);
#else
extern G_MODULE_EXPORT void show_hide_poly (GSimpleAction * action, GVariant * parameter, gpointer data);
#endif
gboolean toggled_rings;

/*!
  \fn void init_ring (project * this_proj)

  \brief initialize the curve widgets for the ring statistics

  \param this_proj the target project
*/
void init_ring (project * this_proj)
{
  int i, j, k, l;
  char * cp[4] = {"Rc(n)[", "Pn(n)[", "Pmax(n)[", "Pmin(n)["};
  l = 0;
  for ( i = 0 ; i < 5 ; i++ )
  {
    for ( j = 0 ; j < 4 ; j++ )
    {

      this_proj -> analysis[RIN] -> curves[l] -> name = g_strdup_printf ("%s - %sAll]", rings_type[i], cp[j]);
      l=l+1;
    }
    for ( j = 0 ; j < this_proj -> nspec ; j++ )
    {
      for ( k = 0 ; k < 4 ; k++ )
      {
        this_proj -> analysis[RIN] -> curves[l] -> name = g_strdup_printf ("%s - %s%s]", rings_type[i], cp[k], active_chem -> label[j]);
        l=l+1;
      }
    }
  }
  add_curve_widgets (this_proj, RIN);
  this_proj -> analysis[RIN] -> init_ok = TRUE;
}

#ifdef GTK3
/*!
  \fn void update_rings_menus (glwin * view)

  \brief update the ring(s) menu for the glview

  \param view the glview
*/
void update_rings_menus (glwin * view)
{
  int i, j;
  GtkWidget * menu;
  for (i=0; i<2; i++)
  {
    menu = gtk_menu_item_get_submenu (GTK_MENU_ITEM(view -> ogl_rings[i*6]));
    if (GTK_IS_WIDGET(menu))
    {
      for (j=1; j<OGL_RINGS/2; j++)
      {
        if (GTK_IS_WIDGET(view -> ogl_rings[i*6+j]))
        {
          g_object_ref (view -> ogl_rings[i*6+j]);
          gtk_container_remove (GTK_CONTAINER(menu), view -> ogl_rings[i*6+j]);
        }
      }
    }
    gtk_menu_item_set_submenu ((GtkMenuItem *)view -> ogl_rings[i*6], prep_rings_menu (view, i*6));
    show_the_widgets (view -> ogl_rings[i*6]);
    widget_set_sensitive (view -> ogl_rings[i*6], view -> rings);
  }
}
#endif

/*!
  \fn void update_rings_view (project * this_proj, int c)

  \brief update the text view for ring statistics

  \param this_proj the target project
  \param c the ring type
*/
void update_rings_view (project * this_proj, int c)
{
  int i, j, k;
  gchar * nelt;
  gchar * col;
  gchar * tab;
  gchar * cid;
  gchar * str;
  if (this_proj -> analysis[RIN] -> calc_buffer == NULL) this_proj -> analysis[RIN] -> calc_buffer = add_buffer (NULL, NULL, NULL);
  view_buffer (this_proj -> analysis[RIN] -> calc_buffer);
  j = this_proj -> rsparam[c][0];
  if (j == 0)
  {
    nelt = g_strdup_printf ("All");
    col = NULL;
  }
  else
  {
    nelt = g_strdup_printf ("%s", this_proj -> chemistry -> label[j-1]);
    col = textcolor(j-1);
  }
  print_info ("\n\nRing statistics\n\n", "heading", this_proj -> analysis[RIN] -> calc_buffer);
  str = g_strdup_printf ("\n%s rings analysis details:\n", rings_type[c]);
  print_info (str, "italic", this_proj -> analysis[RIN] -> calc_buffer);
  g_free (str);

  if (this_proj -> rsparam[c][2])
  {
    print_info (" * only ABAB rings have been considered\n", "italic", this_proj -> analysis[RIN] -> calc_buffer);
  }
  if (this_proj -> rsparam[c][3])
  {
    print_info (" * no homopolar bonds in the rings (A-A, B-B ...)\n", "italic", this_proj -> analysis[RIN] -> calc_buffer);
  }
  if (this_proj -> rsparam[c][4])
  {
    print_info (" * no homopolar bonds in the connectivity matrix (A-A, B-B ...)\n", "italic", this_proj -> analysis[RIN] -> calc_buffer);
  }

  print_info ("\n Atom(s) used to start the search: ", NULL, this_proj -> analysis[RIN] -> calc_buffer);
  print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
  if (j != 0) print_info (" atom(s) only", NULL, this_proj -> analysis[RIN] -> calc_buffer);

  if (this_proj -> steps > 1)
  {
    print_info ("\n Average number of rings per configuration: ", NULL, this_proj -> analysis[RIN] -> calc_buffer);
    str = g_strdup_printf ("%f", this_proj -> rsdata[c][0]);
    print_info (str, "bold", this_proj -> analysis[RIN] -> calc_buffer);
    g_free (str);
    str = g_strdup_printf (" +/- %f\n", this_proj -> rsdata[c][1]);
    print_info (str, "bold", this_proj -> analysis[RIN] -> calc_buffer);
    g_free (str);
  }
  else
  {
    print_info ("\n Total number of rings: ", NULL, this_proj -> analysis[RIN] -> calc_buffer);
    str = g_strdup_printf ("%f\n", this_proj -> rsdata[c][0]);
    print_info (str, "bold", this_proj -> analysis[RIN] -> calc_buffer);
    g_free (str);
  }
  if (c  == 1 || c == 2)
  {
    if (this_proj -> steps > 1)
    {
      print_info (" Average number of ring(s) with n > ", NULL, this_proj -> analysis[RIN] -> calc_buffer);
    }
    else
    {
      print_info (" Number of ring(s) with n > ", NULL, this_proj -> analysis[RIN] -> calc_buffer);
    }
    str = g_strdup_printf ("%d", this_proj -> rsparam[c][1]);
    print_info (str, "bold", this_proj -> analysis[RIN] -> calc_buffer);
    g_free (str);
    print_info ("  nodes that potentially exist: ", NULL, this_proj -> analysis[RIN] -> calc_buffer);
    str = g_strdup_printf ("%f", this_proj -> rsdata[c][2]);
    print_info (str, "bold", this_proj -> analysis[RIN] -> calc_buffer);
    g_free (str);
    if (this_proj -> steps > 1)
    {
      str = g_strdup_printf (" +/- %f", this_proj -> rsdata[c][3]);
      print_info (str, "bold", this_proj -> analysis[RIN] -> calc_buffer);
      g_free (str);
    }
    print_info ("\n", NULL, this_proj -> analysis[RIN] -> calc_buffer);
  }
  print_info ("\n\t n\tRc(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
  print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
  if (this_proj -> steps > 1)
  {
    if (j == this_proj -> nspec)
    {
      print_info ("]\t  +/-   \tPn(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\t  +/-   \tPmax(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\t  +/-   \tPmin(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\t  +/-\n", "bold", this_proj -> analysis[RIN] -> calc_buffer);
    }
    else
    {
      print_info ("]\t   +/-  \tPn(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\t   +/-  \tPmax(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\t   +/-  \tPmin(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\t   +/-  \n", "bold", this_proj -> analysis[RIN] -> calc_buffer);
    }
  }
  else
  {
    if (j == this_proj -> nspec)
    {
      print_info ("]\tPn(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\tPmax(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\tPmin(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\n", "bold", this_proj -> analysis[RIN] -> calc_buffer);
    }
    else
    {
      print_info ("]\tPn(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\tPmax(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\tPmin(n)[", "bold", this_proj -> analysis[RIN] -> calc_buffer);
      print_info (nelt, col, this_proj -> analysis[RIN] -> calc_buffer);
      print_info ("]\n", "bold", this_proj -> analysis[RIN] -> calc_buffer);
    }
  }
  tab = NULL;
  cid = NULL;
  k = 4*(c*(this_proj -> nspec+1) + this_proj -> rsparam[c][0]);
  j = 1;
  for ( i=2 ; i < this_proj -> rsparam[c][1] ; i++ )
  {
    if (this_proj -> analysis[RIN] -> curves[k] -> data[1][i] != 0.0)
    {
      j ++;
      if (j - 2*(j/2) == 0)
      {
        tab = g_strdup_printf ("grey_back");
        cid = g_strdup_printf ("bold_grey_back");
      }
      else
      {
        tab = NULL;
        cid = g_strdup_printf ("bold");
      }
      print_info ("\t", NULL, this_proj -> analysis[RIN] -> calc_buffer);
      if (i < 9)
      {
        print_info (" ",cid, this_proj -> analysis[RIN] -> calc_buffer);
      }
      str = g_strdup_printf("%d", i+1);
      print_info (str, cid, this_proj -> analysis[RIN] -> calc_buffer);
      g_free (str);
      str = g_strdup_printf("\t%f\t", this_proj -> analysis[RIN] -> curves[k] -> data[1][i]);
      print_info (str, tab, this_proj -> analysis[RIN] -> calc_buffer);
      g_free (str);
      if (this_proj -> steps > 1)
      {
        str = g_strdup_printf("%f\t", this_proj -> analysis[RIN] -> curves[k] -> err[i]);
        print_info (str, tab, this_proj -> analysis[RIN] -> calc_buffer);
        g_free (str);
      }
      str = g_strdup_printf("%f\t", this_proj -> analysis[RIN] -> curves[k+1] -> data[1][i]);
      print_info (str, tab, this_proj -> analysis[RIN] -> calc_buffer);
      g_free (str);
      if (this_proj -> steps > 1)
      {
        str = g_strdup_printf("%f\t", this_proj -> analysis[RIN] -> curves[k+1] -> err[i]);
        print_info (str, tab, this_proj -> analysis[RIN] -> calc_buffer);
        g_free (str);
      }
      str = g_strdup_printf("%f\t", this_proj -> analysis[RIN] -> curves[k+2] -> data[1][i]);
      print_info (str, tab, this_proj -> analysis[RIN] -> calc_buffer);
      g_free (str);
      if (this_proj -> steps > 1)
      {
        str = g_strdup_printf("%f\t", this_proj -> analysis[RIN] -> curves[k+2] -> err[i]);
        print_info (str, tab, this_proj -> analysis[RIN] -> calc_buffer);
        g_free (str);
      }
      str = g_strdup_printf("%f", this_proj -> analysis[RIN] -> curves[k+3] -> data[1][i]);
      print_info (str, tab, this_proj -> analysis[RIN] -> calc_buffer);
      g_free (str);
      if (this_proj -> steps > 1)
      {
        str = g_strdup_printf("\t%f", this_proj -> analysis[RIN] -> curves[k+3] -> err[i]);
        print_info (str, tab, this_proj -> analysis[RIN] -> calc_buffer);
        g_free (str);
      }
      print_info ("\n", NULL, this_proj -> analysis[RIN] -> calc_buffer);
      if (tab != NULL)
      {
        g_free (tab);
      }
      if (cid != NULL)
      {
        g_free (cid);
      }
    }
  }
  print_info (calculation_time(TRUE, this_proj -> rsdata[c][4]), NULL, this_proj -> analysis[RIN] -> calc_buffer);
  g_free (nelt);
  if (col != NULL)
  {
    g_free (col);
  }
}

/*!
  \fn void clean_rings_data (int rid, glwin * view)

  \brief clean a ring type data for a glview

  \param rid Rings type
  \param view the glview
*/
void clean_rings_data (int rid, glwin * view)
{
  project * this_proj = get_project_by_id(view -> proj);
  view -> ring_max[rid] = 0;
  int i, j;
#ifdef GTK3
  // GTK3 Menu Action To Check
  for (i=0; i<this_proj -> coord -> totcoord[4+rid]; i++)
  {
    if (view -> ogl_poly[0][4+rid][i] != NULL)
    {
      if (GTK_IS_WIDGET(view -> ogl_poly[0][4+rid][i]))
      {
        if (view -> anim -> last -> img -> show_poly[4+rid][i])
        {
          gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_poly[0][4+rid][i], FALSE);
          show_hide_poly (view -> ogl_poly[0][4+rid][i], & view -> gcid[4+rid][i][4+rid]);
        }
      }
    }
  }
#endif
  if (this_proj -> coord -> totcoord[4+rid])
  {
    if (this_proj -> coord -> ntg[4+rid]) g_free (this_proj -> coord -> ntg[4+rid]);
    this_proj -> coord -> ntg[4+rid] = NULL;
    if (this_proj -> coord -> geolist[4+rid][0]) g_free (this_proj -> coord -> geolist[4+rid][0]);
    this_proj -> coord -> geolist[4+rid][0] = NULL;
    this_proj -> coord -> totcoord[4+rid] = 0;
  }
  if (view -> all_rings[rid]) g_free (view -> all_rings[rid]);
  view -> all_rings[rid] = NULL;
  if (view -> num_rings[rid]) g_free (view -> num_rings[rid]);
  view -> num_rings[rid] = NULL;
  if (view -> show_rpoly[rid]) g_free (view -> show_rpoly[rid]);
  view -> show_rpoly[rid] = NULL;
  if (view -> gcid[4+rid]) g_free (view -> gcid[4+rid]);
  view -> gcid[4+rid] = NULL;
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      if (this_proj -> atoms[i][j].rings[rid])
      {
        g_free (this_proj -> atoms[i][j].rings[rid]);
        this_proj -> atoms[i][j].rings[rid] = NULL;
      }
    }
  }
  int shaders[1] = {RINGS};
  re_create_md_shaders (1, shaders, this_proj);
  update (view);
#ifdef GTK4
  update_menu_bar (view);
#endif
}

/*!
  \fn G_MODULE_EXPORT void on_calc_rings_released (GtkWidget * widg, gpointer data)

  \brief compute ring statistics

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_calc_rings_released (GtkWidget * widg, gpointer data)
{
  int search = active_project -> rsearch[0];
  int i, j, k;

  if (toggled_rings) active_project -> dmtx = FALSE;

#ifdef DEBUG
  g_debug ("Calc rings !");
  g_debug (" - rings definition: %d", search);
  if (active_project -> rsparam[search][2]) g_debug (" - only ABAB rings !");
  if (active_project -> rsparam[search][3]) g_debug (" - no homopolar bonds in rings !");
  if (active_project -> rsparam[search][4]) g_debug (" - no homopolar bonds at all !");
  g_debug (" - dmtx= %d", active_project -> dmtx);
#endif

  cutoffsend ();
  //if (active_project -> steps > 1) statusb = 1;
  if (! active_project -> analysis[RIN] -> init_ok) init_ring (active_project);
  active_project -> rsparam[search][5] = 0;
  if (! active_project -> dmtx || active_project -> rsparam[search][4] || (search > 2 && active_cell -> pbc))
  {
    active_project -> dmtx = run_distance_matrix (widg, search+1, 0);
  }
  if (active_project -> dmtx)
  {
    i = search;
    j = 4*(active_project -> nspec + 1) * i;
    clean_curves_data (RIN, j+4*active_project -> rsparam[i][0], j+4*(active_project -> rsparam[i][0]+1));
    clean_rings_data (i, active_glwin);
    active_glwin -> all_rings[i] = g_malloc0 (active_project -> steps*sizeof*active_glwin -> all_rings[i]);
    active_glwin -> num_rings[i] = g_malloc0 (active_project -> steps*sizeof*active_glwin -> num_rings[i]);
    active_glwin -> show_rpoly[i] = g_malloc0 (active_project -> steps*sizeof*active_glwin -> show_rpoly[i]);
    active_glwin -> ring_max[i] = active_project -> rsparam[i][1];
    active_glwin -> rings = TRUE;
    for (j=0; j<active_project -> steps; j++)
    {
      active_glwin -> all_rings[i][j] = g_malloc0 (active_project -> rsparam[i][1]*sizeof*active_glwin -> all_rings[i][j]);
      active_glwin -> num_rings[i][j] = allocint (active_project -> rsparam[i][1]);
      active_glwin -> show_rpoly[i][j] = g_malloc (active_project -> rsparam[i][1]*sizeof*active_glwin -> show_rpoly[i][j]);
      for (k=0; k < active_project -> natomes; k++)
      {
        active_project -> atoms[j][k].rings[i] = g_malloc0 (active_project -> rsparam[i][1]*sizeof*active_project -> atoms[j][k].rings[i]);
      }
    }
    prepostcalc (widg, FALSE, RIN, 0, opac);
    j = initrings_ (& search,
                    & active_project -> rsparam[i][1],
                    & active_project -> rsparam[i][0],
                    & active_project -> rsearch[1],
                    & active_project -> rsparam[i][2],
                    & active_project -> rsparam[i][3]);
    active_project -> rsdata[i][4] = get_calc_time (start_time, stop_time);
    if (j == 0)
    {
      show_error ("The ring statistics calculation has failed", 0, widg);
      active_glwin -> ring_max[i] = 0;
      active_project -> rsdata[i][4] = 0.0;
    }
    else if (j == 2)
    {
      gchar * str = g_strdup_printf ("\t<b>The ring statistics have failed !</b>\n\n"
                                     "The number of ring per MD step appears\n"
                                     "to be bigger than the initial value of <b>%d</b>\n"
                                     "used to allocate memory to store the results.\n\n"
                                     "Increase the value and start again !",
                                     active_project -> rsearch[1]);
      show_error (str, 0, widg);
      g_free (str);
      active_glwin -> ring_max[i] = 0;
      active_project -> rsdata[i][4] = 0.0;
      j = 0;
    }
    else
    {
      if (active_coord -> totcoord[4+search])
      {
        active_project -> rsparam[search][5] = 1;
        update_rings_view (active_project, search);
      }
      else
      {
        active_glwin -> ring_max[i] = 0;
      }
    }
    prepostcalc (widg, TRUE, RIN, j, 1.0);
  }
  else
  {
    show_error ("The nearest neighbors table calculation has failed", 0, widg);
  }
  active_glwin -> rings = FALSE;
  for (i=0; i<5; i++)
  {
    if (active_coord -> totcoord[4+i])
    {
      active_glwin -> rings = TRUE;
      break;
    }
  }
  show_the_widgets (curvetoolbox);
  clean_coord_window (active_project);
#ifdef GTK3
  update_rings_menus (active_glwin);
#else
  update_menu_bar (active_glwin);
#endif
  fill_tool_model ();
  if (search > 2 && active_cell -> pbc) active_project -> dmtx = FALSE;
}

/*!
  \fn void save_rings_data_ (int * taille,
                          double ectrc[*taille],
                          double ectpna[*taille],
                          double ectmax[*taille],
                          double ectmin[*taille],
                          double * rpstep, double * ectrpst,
                          double * nampat, double * ectampat)

  \brief get rings statistics results form Fortran90

  \param taille number of data points
  \param ectrc Rc (RINGS method)
  \param ectpna Pn (RINGS method)
  \param ectmax Pmax (RINGS method)
  \param ectmin Pmin (RINGS method)
  \param rpstep ring(s) per MD step
  \param ectrpst Standard deviation
  \param nampat rings not found
  \param ectampat standard deviation
*/
void save_rings_data_ (int * taille,
                       double ectrc[* taille],
                       double ectpna[* taille],
                       double ectmax[* taille],
                       double ectmin[* taille],
                       double * rpstep, double * ectrpst,
                       double * nampat, double * ectampat)
{
  int i, j;
  i = active_project -> rsearch[0];
  active_project -> rsdata[i][0] = * rpstep;
  active_project -> rsdata[i][1] = * ectrpst;
  active_project -> rsdata[i][2] = * nampat;
  active_project -> rsdata[i][3] = * ectampat;
  j = 4*(i*(active_project -> nspec+1) + active_project -> rsparam[i][0]);
  active_project -> analysis[RIN] -> curves[j] -> err = duplicate_double (* taille, ectrc);
  active_project -> analysis[RIN] -> curves[j+1] -> err = duplicate_double (* taille, ectpna);
  active_project -> analysis[RIN] -> curves[j+2] -> err = duplicate_double (* taille, ectmax);
  active_project -> analysis[RIN] -> curves[j+3] -> err = duplicate_double (* taille, ectmin);
}
