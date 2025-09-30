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
* @file initcoord.c
* @short Coordinations and polyhedra menus initialization \n
         Coordinations and polyhedra GTK3 menus and menu items creation \n
         Initialization of coordinations and polyhedra related data from Fortran90 information
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'initcoord.c'
*
* Contains:
*

 - Coordinations and polyhedra menu initialization
 - Coordinations and polyhedra GTK3 menus and menu items creation
 - Initialization of coordinations and polyhedra related data from Fortran90 information

*
* List of functions:

  void gcid_spcolor_setup (int sp, int id);
  void set_color_map_sensitive (glwin * view);
  void set_advanced_bonding_menus (glwin * view);
  void prep_all_coord_menus (glwin * view);
  void partial_geo_out_ (int * sp, int * id, int * ngsp, int coord[* ngsp]);
  void allocate_partial_geo_ (int * sp, int * ngsp);
  void init_menu_coordinations_ (int * id, int * sp, int * ngsp, int coordt[* ngsp]);
  void init_menu_fragmol_ (int * id);
  void init_menurings_ (int * coo, int * ids, int * ngsp, int coordt[* ngsp], int * init);
  void init_opengl_coords (int id, int nt, int init);
  void send_coord_opengl_ (int * id, int * num, int * cmin, int * cmax, int * nt, int coord[* num]);

  GtkWidget * coord_view_setup (int * sp, int id, int jd);
  GtkWidget * coord_color_setup (int * sp, int id, int jd);
  GtkWidget * poly_show_setup (int * sp, int id, int jd);
  GtkWidget * create_coord_menu (int p, char * name, gboolean va, GtkWidget * menu, qint * data);

  ColRGBA init_color (int id, int numid);

*/

#include "global.h"
#include "interface.h"
#include "bind.h"
#include "color_box.h"
#include "glwindow.h"
#include "glview.h"

extern GtkWidget * coord_menu (glwin * view);
extern cairo_surface_t * col_surface (double r, double g, double b, int x, int y);
extern G_MODULE_EXPORT void coord_properties (GtkWidget * widg, gpointer data);
extern void setup_molecules ();
extern int * duplicate_int (int num, int * old_val);

int ** idgeo = NULL;

/*!
  \fn ColRGBA init_color (int id, int numid)

  \brief initialize color based id number over total number of elements

  \param id the id number
  \param numid the total number of elements
*/
ColRGBA init_color (int id, int numid)
{
  float a, b;
  ColRGBA col;
  if (numid > 5)
  {
    a = (float)id;
    b = (float)numid / 4.0;
    if (a < b)
    {
      col.red = 1.0;
      col.green = a / b;
      col.blue = 0.0;
    }
    else if (a < 2*b)
    {
      col.red = 1.0 - (1.0 * (a-b)/b);
      col.green = 1.0;
      col.blue = 0.0;
    }
    else if (a < 3*b)
    {
      col.red = 0.0;
      col.green = 1.0;
      col.blue = (1.0 * (a-2.0*b)/b);
    }
    else
    {
      col.red = 0.0;
      col.green = 1.0 - (1.0 * (a-3.0*b)/b);
      col.blue = 1.0;
    }
    col.alpha = 1.0;
  }
  else
  {
    col = std[id];
    col.alpha = 1.0;
  }
  return col;
}

/*!
  \fn void gcid_spcolor_setup (int sp, int id)

  \brief prepare color pointers

  \param sp the chemical species
  \param id the coordination id
*/
void gcid_spcolor_setup (int sp, int id)
{
  int i;
  if (active_glwin -> gcid[id] == NULL)
  {
    active_glwin -> gcid[id] = g_malloc0 (active_coord -> totcoord[id]*sizeof*active_glwin -> gcid[id]);
    for (i=0; i<active_coord -> totcoord[id]; i++)
    {
      active_glwin -> gcid[id][i] = g_malloc0 (64*sizeof*active_glwin -> gcid[id][i]);
    }
  }
  active_image -> spcolor[id][sp] = g_malloc0 (active_coord -> totcoord[id]*sizeof*active_image -> spcolor[id][sp]);
}

#ifdef GTK3
// GTK3 Menu Action To Check
/*!
  \fn void set_color_map_sensitive (glwin * view)

  \brief set color map menu items sensitivity GTK3

  \param view the target glwin
*/
void set_color_map_sensitive (glwin * view)
{
  int i, j;
  for (i=0; i<ATOM_MAPS+POLY_MAPS; i++)
  {
    j = i - ATOM_MAPS*(i/ATOM_MAPS);
    if (j < 3 ) widget_set_sensitive (view -> color_styles[i], 1);
    if (j == 3) widget_set_sensitive (view -> color_styles[i], view -> adv_bonding[0]);
    if (j == 4) widget_set_sensitive (view -> color_styles[i], view -> adv_bonding[1]);
    if (j == 5) widget_set_sensitive (view -> color_styles[i], (get_project_by_id(view -> proj) -> force_field[0]) ? 1 : 0);
    if (j == 6) widget_set_sensitive (view -> color_styles[i], (! (i/ATOM_MAPS) || view -> custom_map) ? 1 : 0);
  }
}

/*!
  \fn void set_advanced_bonding_menus (glwin * view)

  \brief set sensitivity of advanced bonding menus GTK3

  \param view the target glwin
*/
void set_advanced_bonding_menus (glwin * view)
{
  int i;
  for (i=0; i<2; i++)
  {
    widget_set_sensitive (view -> ogl_smode[2*i+2], view -> adv_bonding[0]);
    widget_set_sensitive (view -> ogl_smode[2*i+3], view -> adv_bonding[1]);
    widget_set_sensitive (view -> ogl_mode[2+i+NINPUTS], view -> adv_bonding[1]);
  }
}

/*!
  \fn void prep_all_coord_menus (glwin * view)

  \brief prepare coordination menus GTK3

  \param view the target glwin
*/
void prep_all_coord_menus (glwin * view)
{
  // GTK3 Menu Action To Check
  set_color_map_sensitive (view);
  gtk_menu_item_set_submenu ((GtkMenuItem *)view -> ogl_coord[0], coord_menu (view));
  widget_set_sensitive (view -> ogl_coord[3], view -> adv_bonding[0]);
  widget_set_sensitive (view -> ogl_coord[4], view -> adv_bonding[1]);
  show_the_widgets (view -> ogl_coord[0]);
  set_advanced_bonding_menus (view);
}

/*!
  \fn GtkWidget * coord_view_setup (int * sp, int id, int jd)

  \brief create coordination menu elements GTK3

  \param sp the chemical species
  \param id the coordination (0=total, 1=partial, >1 rings)
  \param jd pop menu or main app menu
*/
GtkWidget * coord_view_setup (int * sp, int id, int jd)
{
  GtkWidget * menuv = gtk_menu_new ();
  if (id < 2)
  {
    active_glwin -> oglmv[jd][id][* sp] = NULL;
    active_glwin -> oglmv[jd][id][* sp] = create_menu_item (TRUE, active_chem -> label[* sp]);
    gtk_menu_item_set_submenu ((GtkMenuItem *)active_glwin -> oglmv[jd][id][* sp], menuv);
  }
  return menuv;
}

/*!
  \fn GtkWidget * coord_color_setup (int * sp, int id, int jd)

  \brief create coordination color menu elements GTK3

  \param sp the chemical species
  \param id the coordination (0=total, 1=partial, >1 rings)
  \param jd pop menu or main app menu
*/
GtkWidget * coord_color_setup (int * sp, int id, int jd)
{
  if (! jd) gcid_spcolor_setup (* sp, id);
  GtkWidget * menuc = NULL;
  menuc = gtk_menu_new ();
  if (id < 2)
  {
    active_glwin -> oglmc[jd][id][* sp] = NULL;
    active_glwin -> oglmc[jd][id][* sp] = create_menu_item (TRUE, active_chem -> label[* sp]);
    gtk_menu_item_set_submenu ((GtkMenuItem *)active_glwin -> oglmc[jd][id][* sp], menuc);
  }
  return menuc;
}

/*!
  \fn GtkWidget * poly_show_setup (int * sp, int id, int jd)

  \brief create polyhedra menu item elements GTK3

  \param sp the chemical species
  \param id the coordination (0=total, 1=partial, >1 rings)
  \param jd pop menu or main app menu
*/
GtkWidget * poly_show_setup (int * sp, int id, int jd)
{
  GtkWidget * menup = gtk_menu_new ();
  active_glwin -> oglmpv[jd][id][* sp] = NULL;
  if (id < 2)
  {
    active_glwin -> oglmpv[jd][id][* sp] = create_menu_item (TRUE, active_chem -> label[* sp]);
  }
  else
  {
    active_glwin -> oglmpv[jd][id][* sp] = create_menu_item (TRUE, "Show/Hide");
  }
  gtk_menu_item_set_submenu ((GtkMenuItem *)active_glwin -> oglmpv[jd][id][* sp], menup);
  return menup;
}

/*!
  \fn GtkWidget * create_coord_menu (int p, char * name, gboolean va, GtkWidget * menu, qint * data)

  \brief create coordination/polyhedra menu widget GTK3

  \param p coordination (0), or polyhedra (1)
  \param name text of the menu item
  \param va status, coordination or polyhedra visible or not (1/0)
  \param menu the GtkWidget menu to attach the menu item to
  \param data the associated data pointer
*/
GtkWidget * create_coord_menu (int p, char * name, gboolean va, GtkWidget * menu, qint * data)
{
  GtkWidget * coord_widget = gtk3_menu_item (menu, name, IMG_NONE, NULL, NULL, NULL, FALSE, 0,  0, TRUE, FALSE, va);
  if (p == 0)
  {
    g_signal_connect (G_OBJECT (coord_widget), "activate", G_CALLBACK(show_hide_coord), data);
  }
  else
  {
    g_signal_connect (G_OBJECT (coord_widget), "activate", G_CALLBACK(show_hide_poly), data);
  }
  return coord_widget;
}
#endif

/*!
  \fn void partial_geo_out_ (int * sp, int * id, int * ngsp, int coord[* ngsp])

  \brief partial coordination data from Fortran90

  \param sp the chemical species
  \param id the partial coordination id
  \param ngsp the number of chemical species
  \param coord the list of partial coordination(s) for that chemical species and coordination id
*/
void partial_geo_out_ (int * sp, int * id, int * ngsp, int coord[* ngsp])
{
  active_coord -> partial_geo[* sp][* id] = duplicate_int (* ngsp, coord);
}

/*!
  \fn void allocate_partial_geo_ (int * sp, int * ngsp)

  \brief allocate partial coordination(s) data

  \param sp the chemical species
  \param ngsp the number of distinct coordination(s) for that chemical species
*/
void allocate_partial_geo_ (int * sp, int * ngsp)
{
  if (active_coord -> partial_geo[* sp] != NULL)
  {
    g_free (active_coord -> partial_geo[* sp]);
    active_coord -> partial_geo[* sp] = NULL;
  }
  active_coord -> partial_geo[* sp] = g_malloc (* ngsp * sizeof*active_coord -> partial_geo[* sp]);
}

/*!
  \fn void init_menu_coordinations_ (int * id, int * sp, int * ngsp, int coordt[*ngsp])

  \brief getting atomic coordinations data from Fortran90, and related GTK3 menu elements creation

  \param id 0 for total coordination(s), 1 for partial coordination(s)
  \param sp the chemical species
  \param ngsp the number of distinct coordination(s) for that chemical species
  \param coordt the list of coordination(s) for that chemical species
*/
void init_menu_coordinations_ (int * id, int * sp, int * ngsp, int coordt[* ngsp])
{
  int i, j, k, l, m, n, o;
#ifdef GTK3
  // GTK3 Menu Action To Check
  gchar * str;
  GtkWidget * spm;
  GtkWidget * menupv;
  GtkWidget * menuc;
  GtkWidget * menuv;
#endif
  i = 0;
  for (j=0; j < * sp; j++)
  {
    i += active_coord -> ntg[* id][j];
  }
  j = 2*active_project -> nspec;
  for (l=0; l < * id; l++)
  {
    j += active_coord -> totcoord[l];
  }
  k = i + j;
  if (active_coord -> geolist[* id][* sp] != NULL)
  {
    g_free (active_coord -> geolist[* id][* sp]);
    active_coord -> geolist[* id][* sp] = NULL;
  }
  active_coord -> geolist[* id][* sp] = duplicate_int (* ngsp, coordt);
  active_coord -> ntg[* id][* sp] = * ngsp;

  l = 0;
  for (m=0; m < * ngsp; m++)
  {
    if (* id)
    {
      n = 0;
      for (o=0; o<active_project -> nspec; o++)
      {
        n += active_coord -> partial_geo[* sp][m][o];
      }
    }
    else
    {
      n = coordt[m];
    }

    if (n > 2)
    {
      l ++;
      active_image -> show_poly[* id][m+i] = TRUE;
    }
    else
    {
      active_image -> show_poly[* id][m+i] = FALSE;
    }
  }
#ifdef GTK4
  gcid_spcolor_setup (* sp, * id);
  for (m=0; m < * ngsp; m++)
  {
    active_image -> show_coord[* id][m+i] = TRUE;
    active_coord -> geolist[* id][* sp][m] = coordt[m];
    if (* id)
    {
      active_image -> spcolor[* id][* sp][m] = init_color (m+i, active_coord -> totcoord[* id]);
    }
    else
    {
      active_image -> spcolor[* id][* sp][m] = init_color (coordt[m]-active_coord -> cmin, active_coord -> cmax-active_coord -> cmin);
    }
    active_image -> show_poly[* id][m+i] = FALSE;
    active_image -> spcolor[* id][* sp][m].alpha = DEFAULT_ALPHA;
    color_box (active_glwin, k+m, * sp, m);
  }
#else
  // GTK3 Menu Action To Check
  for (m=0; m<2; m++)
  {
    menuc = coord_color_setup (sp, * id, m);
    menuv = coord_view_setup (sp, * id, m);
    if (l > 0)
    {
      menupv =  poly_show_setup (sp, * id, m);
    }
    else
    {
      active_glwin -> oglmpv[m][* id][* sp] = NULL;
    }
    for (n=0; n < * ngsp; n++)
    {
      if (* id)
      {
        str = exact_name(env_name (active_project, n, * sp, 1, NULL));
        spm = create_menu_item_from_widget (markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, FALSE);
      }
      else
      {
        str = g_strdup_printf ("%d", coordt[n]);
        spm = create_menu_item (TRUE, str);
      }
      if (! m)
      {
        active_image -> show_coord[* id][n+i] = TRUE;
        active_coord -> geolist[* id][* sp][n] = coordt[n];
        if (* id)
        {
          active_image -> spcolor[* id][* sp][n] = init_color (n+i, active_coord -> totcoord[* id]);
        }
        else
        {
          active_image -> spcolor[* id][* sp][n] = init_color (coordt[n]-active_coord -> cmin, active_coord -> cmax-active_coord -> cmin);
        }
        active_image -> spcolor[* id][* sp][n].alpha = DEFAULT_ALPHA;
      }
      active_glwin -> ogl_geom[m][* id][n+i] = create_coord_menu (0, str, TRUE, menuv, & active_glwin -> gcid[* id][n+i][* id]);
      gtk_menu_shell_append ((GtkMenuShell *)menuc, spm);
      gtk_menu_item_set_submenu ((GtkMenuItem *)spm, color_box (active_glwin, k+n, * sp, n));
      if (active_image -> show_poly[* id][n+i])
      {
        // The polyhedra show/hide menu
        active_glwin -> ogl_poly[m][* id][n+i] = create_coord_menu (1, str, FALSE, menupv, & active_glwin -> gcid[* id][n+i][* id]);
        if (m) active_image -> show_poly[* id][n+i] = FALSE;
      }
      g_free (str);
    }
  }
#endif
}

/*!
  \fn void init_menu_fragmol_ (int * id)

  \brief getting fragment(s)/molecule(s) data from Fortran90, and related GTK3 menu elements creation

  \param id 2 for fragments, 3 for molecules
*/
void init_menu_fragmol_ (int * id)
{
#ifdef DEBUG
  gchar * keyw[2] = {"fragment(s)", "molecule(s)"};
  if (active_project -> steps > 1)
  {
    gchar * str = g_strdup_printf ("Maximum number of %s per MD step: %d", keyw[* id -2], active_coord -> totcoord[* id]);
    g_debug ("%s", str);
  }
#endif // DEBUG
  if (* id == 3)
  {
    active_image -> show_coord[3] = allocbool (active_coord -> totcoord[3]);
#ifdef GTK3
    // GTK3 Menu Action To Check
    if (active_coord -> totcoord[3] <= COORD_MAX_MENU)
    {
      active_glwin -> ogl_geom[0][3] =  g_malloc (active_coord -> totcoord[3]*sizeof*active_glwin -> ogl_geom[0][3]);
      active_glwin -> ogl_geom[1][3] =  g_malloc (active_coord -> totcoord[3]*sizeof*active_glwin -> ogl_geom[1][3]);
    }
#endif
  }
  int i, j;
  j = 2 * active_project -> nspec;
  for (i=0; i < * id; i++)
  {
    j += active_coord -> totcoord[i];
  }
#ifdef GTK4
  gcid_spcolor_setup (0, * id);
  for (i=0; i < active_coord -> totcoord[* id]; i++)
  {
    color_box (active_glwin, j+i, * id, i);
    active_image -> spcolor[* id][0][i] = init_color (i, active_coord -> totcoord[* id]);
    active_image -> spcolor[* id][0][i].alpha = DEFAULT_ALPHA;
    active_image -> show_coord[* id][i] = TRUE;
  }
#else
  int k;
  gchar * str;
  GtkWidget * spm;
  for (k=0; k<2; k++)
  {
    i = 0;
    active_glwin -> oglmv[k][* id][0] = coord_view_setup (& i, * id, k);
    active_glwin -> oglmc[k][* id][0] = coord_color_setup (& i, * id, k);
    for (i=0; i < active_coord -> totcoord[* id]; i++)
    {
      if (! k) active_image -> show_coord[* id][i] = TRUE;
      if (active_coord -> totcoord[* id] <= COORD_MAX_MENU)
      {
        if (* id == 2)
        {
          str = g_strdup_printf ("Fragment N°%d", i+1);
        }
        else
        {
          str = g_strdup_printf ("Molecule N°%d", i+1);
        }
        active_glwin -> ogl_geom[k][* id][i] = create_coord_menu (0, str, TRUE, active_glwin -> oglmv[k][* id][0], & active_glwin -> gcid[* id][i][* id]);
        spm = create_menu_item (TRUE, str);
        g_free (str);
        gtk_menu_shell_append ((GtkMenuShell *)active_glwin -> oglmc[k][* id][0], spm);
        gtk_menu_item_set_submenu ((GtkMenuItem *)spm, color_box (active_glwin, j+i, * id, i));
      }
      else
      {
        active_glwin -> gcid[* id][i][* id].a = active_glwin -> proj;
        active_glwin -> gcid[* id][i][* id].b = * id;
        active_glwin -> gcid[* id][i][* id].c = i;
        active_glwin -> gcid[* id][i][* id].d = * id;

      }
      active_image -> spcolor[* id][0][i] = init_color (i, active_coord -> totcoord[* id]);
      active_image -> spcolor[* id][0][i].alpha = DEFAULT_ALPHA;
    }
  }
#endif
}

/*!
  \fn void init_menurings_ (int * coo, int * ids, int * ngsp, int coordt[*ngsp], int * init)

  \brief getting rings statistics data from Fortran90, and related GTK3 menu elements creation

  \param coo the coord type
  \param ids the ring(s) type in [0-4], or 0 for the chains
  \param ngsp the total number of distinct ring size(s)
  \param coordt the list of ring size(s) with rings
  \param init initialize some visual information (1/0)
*/
void init_menurings_ (int * coo, int * ids, int * ngsp, int coordt[* ngsp], int * init)
{
  int j;

  if (active_coord -> geolist[* coo][0] != NULL)
  {
    g_free (active_coord -> geolist[* coo][0]);
    active_coord -> geolist[* coo][0] = NULL;
  }
  active_coord -> geolist[* coo][0] = allocint (* ngsp);
#ifdef GTK4
  gcid_spcolor_setup (0, * coo);
  for ( j=0 ; j < * ngsp ; j++ )
  {
    active_coord -> geolist[* coo][0][j] = coordt[j];
    active_image -> show_coord[* coo][j] = TRUE;
    if ( * coo < 9)
    {
      color_box (active_glwin, -3, * ids, j);
      if (* init) active_image -> show_poly[* coo][j] = FALSE;
      active_image -> spcolor[* coo][0][j] = init_color (coordt[j]-3, active_glwin -> ring_max[* ids]);
      active_image -> spcolor[* coo][0][j].alpha = DEFAULT_ALPHA;
    }
    else
    {
      active_glwin -> gcid[9][j][9].a = active_glwin -> proj;
      active_glwin -> gcid[9][j][9].b = 0;
      active_glwin -> gcid[9][j][9].c = j;
      active_glwin -> gcid[9][j][9].d = 9;
    }
  }
#else
  gchar * str;
  GtkWidget * spm;
  GtkWidget * allt;
  int i;
  for (i=0 ; i < 2 ; i++)
  {
    j = 0;
    active_glwin -> oglmc[i][* coo][j] = NULL;
    active_glwin -> oglmc[i][* coo][j] = coord_color_setup (& j, * coo, i);
    if (! * init) active_glwin -> ogl_poly[i][* coo] = g_malloc0 (*ngsp*sizeof*active_glwin -> ogl_poly[i][* coo]);
    for ( j=0 ; j < * ngsp ; j++ )
    {
      if (i == 0)
      {
        active_image -> show_coord[* coo][j] = TRUE;
        if (* coo < 9)
        {
          if (* init) active_image -> show_poly[* coo][j] = FALSE;
          active_image -> spcolor[* coo][0][j] = init_color (coordt[j]-3, active_glwin -> ring_max[* ids]);
          active_image -> spcolor[* coo][0][j].alpha = DEFAULT_ALPHA;
        }
        active_coord -> geolist[* coo][0][j] = coordt[j];
      }
      str = g_strdup_printf ("%d", coordt[j]);
      spm = create_menu_item (TRUE, str);
      active_glwin -> ogl_geom[i][* coo][j] = create_coord_menu (0, str, TRUE, NULL, & active_glwin -> gcid[* coo][j][* coo]);
      gtk_menu_shell_append ((GtkMenuShell *)active_glwin -> oglmc[i][* coo][0], spm);
      if (* coo < 9)
      {
        gtk_menu_item_set_submenu ((GtkMenuItem *)spm, color_box (active_glwin, -3, * ids, j));
        active_glwin -> ogl_poly[i][* coo][j] = create_coord_menu (1, str, active_image -> show_poly[* coo][j], NULL, & active_glwin -> gcid[* coo][j][* coo]);
      }
      else
      {
        active_glwin -> gcid[9][j][9].a = active_glwin -> proj;
        active_glwin -> gcid[9][j][9].b = 0;
        active_glwin -> gcid[9][j][9].c = j;
        active_glwin -> gcid[9][j][9].d = 9;
      }
      g_free (str);
    }
    allt = create_menu_item (TRUE, "_All");
    g_signal_connect (G_OBJECT (allt), "activate", G_CALLBACK(coord_properties), & active_glwin -> colorp[* coo][1]);
    gtk_menu_shell_append ((GtkMenuShell *)active_glwin -> oglmc[i][* coo][0], allt);
  }
#endif
}

/*!
  \fn void init_opengl_coords (int id, int nt, int init)

  \brief initialize data to store the coordination data

  \param id the geometry id, in: [0-9]
  \param nt total number of distinct coordination
  \param init initialize some visual information (1/0)
*/
void init_opengl_coords (int id, int nt, int init)
{
  int k;
  active_coord -> species = active_project -> nspec;
  active_coord -> totcoord[id] = nt;
  active_image -> show_coord[id] = allocbool (nt);
#ifdef GTK3
  int i, j;
  for (i=0; i<2; i++)
  {
    if (active_glwin -> ogl_geom[i][id]) g_free (active_glwin -> ogl_geom[i][id]);
    active_glwin -> ogl_geom[i][id] =  g_malloc0 (nt*sizeof*active_glwin -> ogl_geom[i][id]);
    for (j=0; j < nt; j++)
    {
      active_glwin -> ogl_geom[i][id][j] = NULL;
    }
  }
#endif
  if (id < 2 || id > 3)
  {
    k = (id > 3) ? 1 : active_project -> nspec;
    if (active_coord -> ntg[id]) g_free (active_coord -> ntg[id]);
    active_coord -> ntg[id] = g_malloc0 (k*sizeof*active_coord -> ntg[id]);
    if (id > 3) active_coord -> ntg[id][0] = nt;
    if (id < 9)
    {
      if (! init)
      {
        if (active_image -> show_poly[id]) g_free (active_image -> show_poly[id]);
        active_image -> show_poly[id] = allocbool (nt);
      }
#ifdef GTK3
      for (i=0; i<2; i++)
      {
        if (active_glwin -> ogl_poly[i][id]) g_free (active_glwin -> ogl_poly[i][id]);
        active_glwin -> ogl_poly[i][id] = g_malloc0 (nt*sizeof*active_glwin -> ogl_poly[i][id]);
        for (j=0; j < nt; j++)
        {
          active_glwin -> ogl_poly[i][id][j] = NULL;
        }
      }
#endif
    }
  }
}

/*!
  \fn void send_coord_opengl_ (int * id, int * num, int * cmin, int * cmax, int * nt, int coord[*num])

  \brief coordination information from Fortran90

  \param id the geometry id, in: [0-9]
  \param num number of atom(s)
  \param cmin min value for the total coordination
  \param cmax max value for the total coordination
  \param nt total number of distinct coordination
  \param coord the values for each atom
*/
void send_coord_opengl_ (int * id, int * num, int * cmin, int * cmax, int * nt, int coord[* num])
{
  int i, j, k;
  if (* nt) init_opengl_coords (* id, * nt, 0);
  if (* id < 2)
  {
    if (* id == 0)
    {
      // Total coordination
      active_coord -> cmax = * cmax;
      active_coord -> cmin = * cmin;
    }
    if (coord != NULL)
    {
      for (i=0, k=0; i < active_project -> steps; i++)
      {
        for (j=0; j < active_project -> natomes; j++, k++)
        {
          active_project -> atoms[i][j].coord[* id] = coord[k] - 1;
        }
      }
    }
  }
}
