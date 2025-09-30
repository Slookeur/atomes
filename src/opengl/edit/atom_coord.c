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
* @file atom_coord.c
* @short Functions to correct the coordination menus and the corresponding data pointers after edition
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'atom_coord.c'
*
* Contains:
*

 - The functions to correct the coordination menus and the corresponding data pointers after edition

*
* List of functions:

  int test_this_coord (project * this_proj, int spec, int gid, int cid, int minc, int maxc);

  void print_coord_info (project * this_proj, coord_info * coord);
  void clean_coords_and_geoms (project * this_proj, atom_edition * edit,
                               int new_spec, int rem, int adds, int * spid, int *** tmpgeo,
                               gboolean * showcoord[2], gboolean * showpoly[2]);
  void new_coord_menus (project * this_proj, coord_info * coord, int new_spec, int nmols,
                        gboolean * showcoord[2], gboolean * showpoly[2], gboolean * showfrag,
                        gboolean update_it, gboolean update_mol);
  void recover_opengl_data (project * this_proj, int nmols, int add, int rem, int * num, int * rec, int *** tmpgeo, gboolean * showfrag);

  coord_info * duplicate_coord_info (coord_info * old_coord);

*/

#include "atom_edit.h"

/*!
  \fn int test_this_coord (project * this_proj, int spec, int gid, int cid, int minc, int maxc)

  \brief Look for a type of coordination sphere in a model

  \param this_proj the target project
  \param spec the number of chemical species
  \param gid the coordination type
  \param cid the coordination id
  \param minc min value for coordination id
  \param maxc max value for coordination id
*/
int test_this_coord (project * this_proj, int spec, int gid, int cid, int minc, int maxc)
{
  int i, j, k, l;
  l = 0;
  for (i=0; i<this_proj -> natomes; i++)
  {
    j = this_proj -> atoms[0][i].sp;
    k = this_proj -> atoms[0][i].coord[gid];
    if (j == spec && (k > maxc && k < minc)) return -1;
    if (j == spec && k == cid)
    {
      l ++;
    }
  }
  return l;
}

/*!
  \fn void print_coord_info (project * this_proj, coord_info * coord)

  \brief print coordination info

  \param this_proj the target project
  \param coord the target coordination info
*/
void print_coord_info (project * this_proj, coord_info * coord)
{
  int * min_bs, * max_bs;
  int i, j, k, l, m;
  gchar * str;
  int test;
  if (coord -> species)
  {
    min_bs = allocint(coord -> species);
    max_bs = allocint(coord -> species);
    g_debug (" ");
    g_debug ("coord -> species = %d", coord -> species);
    g_debug (" ");
    for (i=0; i<4; i++)
    {
      g_debug (" --- start geo[%d] ---", i);
      g_debug ("geo i = %d, totcoord[%d] = %d", i, i, coord -> totcoord[i]);
      if ((i < 2 || i > 3) && coord -> totcoord[i])
      {
        j = (i < 2) ? coord -> species : 1;
        g_debug ("  spec i = %d, jmax= %d", i, j);
        g_debug (" ");
        for (k=0; k<j; k++)
        {
          min_bs[k] = max_bs[k] = coord -> partial_geo[k][0][0];
          for (m=0; m<coord -> species; m++)
          {
            min_bs[k] = min (min_bs[k], coord -> partial_geo[k][0][m]);
            max_bs[k] = max (max_bs[k], coord -> partial_geo[k][0][m]);
          }
          for (l=1; l<coord -> ntg[i][k]; l++)
          {
            for (m=0; m<coord -> species; m++)
            {
              min_bs[k] = min (min_bs[k], coord -> partial_geo[k][l][m]);
              max_bs[k] = max (max_bs[k], coord -> partial_geo[k][l][m]);
            }
          }
        }
        for (k=0; k<j; k++)
        {
          g_debug ("    coord i = %d, k = %d, coord -> ntg[%d][%d] = %d", i, k, i, k, coord -> ntg[i][k]);
          test = (this_proj) ? test_this_coord (this_proj, k, i, 0, min_bs[k], max_bs[k]) : 0;
          str = g_strdup_printf ("          l = %d, coord -> geolist[%d][%d][%d]= %d, num[at,%d]= %d", 0, i, k, 0, coord -> geolist[i][k][0], 0, test);
          if (coord -> ntg[i][k] > 1)
          {
            for (l=1; l<coord -> ntg[i][k]; l++)
            {
              test = (this_proj) ? test_this_coord (this_proj, k, i, l, min_bs[k], max_bs[k]) : 0;
              str = g_strdup_printf ("%s\n                                                  l = %d, coord -> geolist[%d][%d][%d]= %d, num[at,%d]= %d", str, l, i, k, l, coord -> geolist[i][k][l], l, test);
            }
          }
          g_debug ("%s", str);
          g_free (str);
        }
        if (i == 1)
        {
          g_debug (" ");
          for (k=0; k<j; k++)
          {
             str = g_strdup_printf ("    Partials geo\n                                                 k = %d, coord -> partial_geo[%d][%d][%d]= %d", k, k, 0, 0, coord -> partial_geo[k][0][0]);
             for (l=1; l<j; l++)
             {
                str = g_strdup_printf ("%s\n                                                 k = %d, coord -> partial_geo[%d][%d][%d]= %d", str, k, k, 0, l, coord -> partial_geo[k][0][l]);
             }
             if (coord -> ntg[i][k] > 1)
             {
               for (l=1; l<coord -> ntg[i][k]; l++)
               {
                 str = g_strdup_printf ("%s\n\n                                                 k = %d, coord -> partial_geo[%d][%d][%d]= %d", str, k, k, l, 0, coord -> partial_geo[k][l][0]);
                 for (m=1; m<j; m++)
                 {
                    str = g_strdup_printf ("%s\n                                                 k = %d, coord -> partial_geo[%d][%d][%d]= %d", str, k, k, l, m, coord -> partial_geo[k][l][m]);
                 }
               }
             }
             g_debug ("%s", str);
             g_free (str);
          }
        }
        g_debug (" ");
      }
      g_debug (" --- end geo[%d] ---", i);
      g_debug (" ");
    }
    g_free (min_bs);
    g_free (max_bs);
  }
  else
  {
    g_debug (" ");
    g_debug ("Coord is empty no species !");
    g_debug (" ");
  }
}

/*!
  \fn coord_info * duplicate_coord_info (coord_info * old_coord)

  \brief duplicate coordination information data structure

  \param old_coord the coordination info to duplicate
*/
coord_info * duplicate_coord_info (coord_info * old_coord)
{
  coord_info * new_coord = g_malloc0 (sizeof*new_coord);
  int i, j, k, l;
  if (! old_coord -> species) return new_coord;
  new_coord -> species = old_coord -> species;
  new_coord -> cmax = old_coord -> cmax;
  new_coord -> cmin = old_coord -> cmin;
  for (i=0; i<10; i++)
  {
    new_coord -> totcoord[i] = old_coord -> totcoord[i];
    if ((i < 2 || i > 3) && old_coord -> totcoord[i])
    {
      j = (i < 2) ? new_coord -> species : 1;
      new_coord -> ntg[i] = duplicate_int (j, old_coord -> ntg[i]);
      new_coord -> geolist[i] = g_malloc0 (j*sizeof*new_coord -> geolist[i]);
      if (i == 1) new_coord -> partial_geo = g_malloc0 (j*sizeof*new_coord -> partial_geo);
      for (k=0; k<j; k++)
      {
        new_coord -> geolist[i][k] = duplicate_int (new_coord -> ntg[i][k], old_coord -> geolist[i][k]);
        if (i == 1)
        {
          new_coord -> partial_geo[k] = g_malloc0 (new_coord -> ntg[i][k]*sizeof*new_coord -> partial_geo[k]);
          for (l=0; l<new_coord -> ntg[i][k]; l++)
          {
            new_coord -> partial_geo[k][l] = duplicate_int (new_coord -> species, old_coord -> partial_geo[k][l]);
          }
        }
      }
    }
  }
  return new_coord;
}

/*!
  \fn void clean_coords_and_geoms (project * this_proj, atom_edition * edit,
                                   int new_spec, int rem, int adds, int * spid, int *** tmpgeo,
                                   gboolean * showcoord[2], gboolean * showpoly[2])

  \brief clean coordination data and atomic coordination id data

  \param this_proj the target project
  \param edit
  \param new_spec the number of chemical species
  \param rem the number of chemical species to remove
  \param adds the number of new chemical species
  \param spid the number of atom(s) by chemical species
  \param tmpgeo the new number of coordination [c] by chemical species [s] 'coorrd -> ntg[c][s]'
  \param showcoord the coordination show status
  \param showpoly the polyhedra show status
*/
void clean_coords_and_geoms (project * this_proj, atom_edition * edit,
                             int new_spec, int rem, int adds, int * spid, int *** tmpgeo,
                             gboolean * showcoord[2], gboolean * showpoly[2])
{
  int i, j, k, l, m, n, o, p, q, r, s;
  int * tntg;
  coord_info * old_coord = duplicate_coord_info (edit -> coord);
  g_free (edit -> coord);
  edit -> coord = g_malloc0 (sizeof*edit -> coord);
  coord_info * tmp_coord = edit -> coord;
  this_proj -> coord -> cmin = 20;
  this_proj -> coord -> cmax = 0;
  for (i=0; i<2; i++)
  {
    tmp_coord -> totcoord[i] = 0;
    tmp_coord -> geolist[i] = g_malloc (new_spec*sizeof*tmp_coord -> geolist[i]);
    tmp_coord -> ntg[i] = allocint (new_spec);
    if (i) tmp_coord -> partial_geo = g_malloc (new_spec*sizeof*tmp_coord -> partial_geo);
    showcoord[i] = allocbool(old_coord -> totcoord[i]);
    showpoly[i] = allocbool(old_coord -> totcoord[i]);
    // g_debug ("i= %d, allocating showcoord/poly of %d", i, old_coord -> totcoord[i]);
    tmp_coord -> totcoord[i] = 0;
    j = s = 0;
    // g_debug ("i= %d, new_spec= %d, rem= %d", i, new_spec, rem);
    for (k=0; k<new_spec+rem; k++)
    {
      if (spid[k])
      {
        l = 0;
        // g_debug ("k= %d, alloc tntg of: %d", k, old_coord -> ntg[i][k]);
        tntg = allocint (old_coord -> ntg[i][k]);
        // if (i == 0) g_debug ("         checking geom:: i= %d, SP= %d, ntg[%d][%d]= %d", i, k, i, k, old_coord -> ntg[i][k]);
        for (m=0; m<old_coord -> ntg[i][k]; m++)
        {
          // if (i == 0) g_debug ("                      :: m = %d, tmpgeo[%d][%d][%d]= %d", m, i, k, m, tmpgeo[i][k][m]);
          if (tmpgeo[i][k][m])
          {
            p = 1;
            if (i)
            {
              //g_debug ("checking partial, m= %d, l= %d", m, l);
              if (l)
              {
                p = 0;
                for (o=0; o<m; o++)
                {
                  for (q=0; q<new_spec+rem; q++)
                  {
                    if (spid[q] && old_coord -> partial_geo[k][m][q] != old_coord -> partial_geo[k][o][q])
                    {
                      p = 1;
                      break;
                    }
                  }
                  if (! p) break;
                }
              }
            }
            else
            {
              for (o=0; o<m; o++)
              {
                if (old_coord -> geolist[i][k][m] == old_coord -> geolist[i][k][o])
                {
                  // g_debug ("  WAS HERE:: k= %d, m= %d, o= %d, geolist = %d", k, m, o, old_coord -> geolist[i][k][m]);
                  p = 0;
                  break;
                }
              }
            }
            if (p)
            {
              // g_debug ("      i= %d, tntg[m]= 1, m= %d", i, m);
              tntg[m] = 1;
              o = l;
              l ++;
            }
            // if (i == 0) g_debug ("      geo= %d, old_sp= %d, new_sp= %d, old_geo= %d, new_geo= %d", i, k, s, m, o);
            for (p=0; p<this_proj -> natomes; p++)
            {
              if (this_proj -> atoms[0][p].sp == s)
              {
                if (this_proj -> atoms[0][p].coord[i] == m)
                {
                  this_proj -> atoms[0][p].coord[i] = o;
                }
              }
            }
          }
        }
        if (l)
        {
          tmp_coord -> geolist[i][j] = allocint(l);
          if (i)
          {
            // g_debug ("allocating partial[%d] of %d and %d", j, l, new_spec);
            tmp_coord -> partial_geo[j] = allocdint (l, new_spec);
          }
          // if (! i) g_debug ("   ntg[0][%d]= %d", j, l);
          tmp_coord -> ntg[i][j] = l;
          tmp_coord -> totcoord[i] += l;

          p = (k < new_spec+rem-adds) ? k : new_spec+rem-adds;
          q = 0;
          for (r=0; r<p; r++) q += this_proj -> coord -> ntg[i][r];
          p = 0;
          for (r=0; r<j; r++) p += tmp_coord -> ntg[i][r];
          m = 0;
          for (n=0; n<old_coord -> ntg[i][k]; n++)
          {
            // g_debug ("n= %d, tntg[%d]= %d", n, n, tntg[n]);
            if (tntg[n])
            {
              tmp_coord -> geolist[i][j][m] = old_coord -> geolist[i][k][n];
              this_proj -> coord -> cmax = max (tmp_coord -> geolist[i][j][m], this_proj -> coord -> cmax);
              this_proj -> coord -> cmin = min (tmp_coord -> geolist[i][j][m], this_proj -> coord -> cmin);
              if (k < new_spec-adds+rem)
              {
                if (n < this_proj -> coord -> ntg[i][k])
                {
                  //g_debug ("p= %d, m= %d, p+m= %d, q= %d, n= %d, q+n= %d", p, m, p+m, q, n, q+n);
                  showcoord[i][p+m] = this_proj -> modelgl -> anim -> last -> img -> show_coord[i][q+n];
                  showpoly[i][p+m] = this_proj -> modelgl -> anim -> last -> img -> show_poly[i][q+n];
                }
                else
                {
                  showcoord[i][p+m] = TRUE;
                  showpoly[i][p+m] = FALSE;
                }
              }
              else
              {
                //g_debug ("p= %d, m= %d, p+m= %d", p, m, p+m);
                showcoord[i][p+m] = TRUE;
                showpoly[i][p+m] = FALSE;
              }
              if (i)
              {
                o = 0;
                for (r=0; r<new_spec+rem; r++)
                {
                  if (spid[r])
                  {
                    tmp_coord -> partial_geo[j][m][o] = old_coord -> partial_geo[k][n][r];
                    //g_debug ("j= %d, m= %d, o= %d, partial= %d, %d", j, m, o, tmp_coord -> partial_geo[j][m][o], old_coord -> partial_geo[k][n][r]);
                    o ++;
                  }
                }
              }
              m ++;
            }
          }
        }
        g_free (tntg);
        j ++;
      }
      s += (spid[k]) ? 1 : 0;
    }
  }
}

/*!
  \fn void new_coord_menus (project * this_proj, coord_info * coord, int new_spec, int nmols,
                            gboolean * showcoord[2], gboolean * showpoly[2], gboolean * showfrag,
                            gboolean update_it, gboolean update_frag, gboolean update_mol)

  \brief update coordination(s), fragment(s) and molecule(s) OpenGL menus and associated data pointers

  \param this_proj the target project
  \param coord the target coordination info data structure
  \param new_spec the number of chemical species
  \param nmols the number of fragment(s) to add
  \param showcoord the coordination show status
  \param showpoly the polyhedra show status
  \param showfrag the fragment show status
  \param update_it update atoms data (and GTK3 menus)
  \param update_mol update molecule(s) data
*/
void new_coord_menus (project * this_proj, coord_info * coord, int new_spec, int nmols,
                      gboolean * showcoord[2], gboolean * showpoly[2], gboolean * showfrag,
                      gboolean update_it, gboolean update_mol)
{
  int i, j, k;
  for (i=0; i<4; i++)
  {
    g_free (this_proj -> modelgl -> gcid[i]);
    this_proj -> modelgl -> gcid[i] = NULL;
  }

  for (i=0; i<2; i++) init_opengl_coords (i, coord -> totcoord[i], 0);
  i = 0;
  for (j=0; j<new_spec; j++)
  {
    init_menu_coordinations_ (& i, & j, & coord -> ntg[0][j], coord -> geolist[0][j]);
  }
  i ++;
  for (j=0; j<new_spec; j++)
  {
    allocate_partial_geo_ (& j, & coord -> ntg[1][j]);
    for (k=0; k<coord -> ntg[1][j]; k++) partial_geo_out_ (& j, & k, & new_spec, coord -> partial_geo[j][k]);
    init_menu_coordinations_ (& i, & j, & coord -> ntg[1][j], coord -> geolist[1][j]);
  }

  i = 2;
  this_proj -> coord -> totcoord[i] += nmols;
  if (this_proj -> coord -> totcoord[i])
  {
    this_proj -> modelgl -> adv_bonding[i-2] = TRUE;
    init_opengl_coords (i, this_proj -> coord -> totcoord[i], 0);
    init_menu_fragmol_ (& i);
  }
  else
  {
    this_proj -> modelgl -> adv_bonding[i-2] = FALSE;
  }
  if (update_mol)
  {
    i = 3;
    if (this_proj -> coord -> totcoord[i])
    {
      this_proj -> modelgl -> adv_bonding[i-2] = TRUE;
      init_opengl_coords (i, this_proj -> coord -> totcoord[i], 0);
      init_menu_fragmol_ (& i);
    }
  }
  else
  {
    this_proj -> modelgl -> adv_bonding[1] = FALSE;
    this_proj -> coord -> totcoord[3] = 0;
  }

#ifdef GTK3
  prep_all_coord_menus (this_proj -> modelgl);
#endif
  if (update_it)
  {
    gboolean * viz[2];
    for (i=0; i<2; i++) viz[i] = allocbool (this_proj -> natomes);
    for (i=0; i<this_proj -> natomes; i++)
    {
      for (j=0; j<2; j++) viz[j][i] = this_proj -> atoms[0][i].show[j];
    }
#ifdef GTK3
    // GTK3 Menu Action To Check
    for (i=0; i<2; i++)
    {
      for (j=0; j<this_proj -> coord -> totcoord[i]; j++)
      {
        if (this_proj -> modelgl -> ogl_geom[0][i][j])
        {
          if (GTK_IS_WIDGET(this_proj -> modelgl -> ogl_geom[0][i][j]))
          {
            if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[0][i][j]) != showcoord[i][j])
            {
              gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[0][i][j], showcoord[i][j]);
            }
          }
        }
        if (this_proj -> modelgl -> ogl_poly[0][i][j])
        {
          if (GTK_IS_WIDGET(this_proj -> modelgl -> ogl_poly[0][i][j]))
          {
            if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_poly[0][i][j]) != showpoly[i][j])
            {
              gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_poly[0][i][j], showpoly[i][j]);
            }
          }
        }
      }
      g_free (showcoord[i]);
      g_free (showpoly[i]);
    }
    for (i=0; i<this_proj -> coord -> totcoord[2]; i++)
    {
      if (this_proj -> modelgl -> ogl_geom[0][2][i])
      {
        if (GTK_IS_WIDGET(this_proj -> modelgl -> ogl_geom[0][2][i]))
        {
          if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[0][2][i]) != showfrag[i])
          {
            gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[0][2][i], showfrag[i]);
          }
        }
      }
    }
#endif
    for (i=0; i<this_proj -> natomes; i++)
    {
      for (j=0; j<2; j++) this_proj -> atoms[0][i].show[j] = viz[j][i];
    }
    for (i=0; i<2; i++) g_free (viz[i]);
  }
}

/*!
  \fn void recover_opengl_data (project * this_proj, int nmols, int add, int rem, int * num, int * rec, int *** tmpgeo, gboolean * showfrag)

  \brief recover image pointer data and OpenGL window menu structure

  \param this_proj the target project
  \param nmols the number of molecules
  \param add the number of new chemical species
  \param rem the number of chemical species to remove
  \param num the number of atom(s) by chemical species
  \param rec the lsit of removed chemical species, if any
  \param tmpgeo the new number of coordination [c] by chemical species [s] 'coorrd -> ntg[c][s]'
  \param showfrag the saved fragment(s) show information
*/
void recover_opengl_data (project * this_proj, int nmols, int add, int rem, int * num, int * rec, int *** tmpgeo, gboolean * showfrag)
{
  // Now OpenGL data
  int old_spec = this_proj -> nspec;
  //g_debug ("recover opengl:: old_spec= %d, add= %d, rem= %d", old_spec, add, rem);
  int new_spec = old_spec + add - rem;
  gboolean * showcoord[2];
  gboolean * showpoly[2];
  gboolean update_it = FALSE;

  clean_coords_and_geoms (this_proj, this_proj -> modelgl -> atom_win, new_spec, rem, add, num, tmpgeo, showcoord, showpoly);

  if (add || rem)
  {
    if (old_spec) free_glwin_spec_data (this_proj, old_spec);
    glwin_init_spec_data (this_proj, new_spec);
  }

  this_proj -> nspec = new_spec;
  active_project_changed (this_proj -> id);

  if ((add && old_spec) || rem)
  {
    int h, i, j, k, l, m, n, o, p, q;
    image * new_img = g_malloc0 (sizeof*new_img);
    setup_image_spec_data (this_proj, new_img);
    image * old_img = this_proj -> modelgl -> anim -> last -> img;
    // copy info in new data, then clean old data and copy it back
    for (i=0; i<2; i++)
    {
      for (j=0; j<old_spec; j++)
      {
        if (num[j])
        {
          k = rec[j];
          new_img -> show_atom[i][j-k] = old_img -> show_atom[i][j];
          new_img -> show_label[i][j-k] = old_img -> show_label[i][j];
        }
      }
    }
    for (h=0; h<2*old_spec; h++)
    {
      i = (h < old_spec) ? h : h - old_spec;
      if (num[i])
      {
        j = rec[i];
        k = (h < old_spec) ? 0 : 1;
        l = h - k*old_spec - j + k*new_spec;
        new_img -> sphererad[l] = old_img -> sphererad[h];
        new_img -> atomicrad[l] = old_img -> atomicrad[h];
        new_img -> pointrad[l] = old_img -> pointrad[h];
        new_img -> at_color[l].red = old_img -> at_color[h].red;
        new_img -> at_color[l].green = old_img -> at_color[h].green;
        new_img -> at_color[l].blue = old_img -> at_color[h].blue;
        new_img -> at_color[l].alpha = old_img -> at_color[h].alpha;
        for (m=0; m<2*old_spec; m++)
        {
          n = (m < old_spec) ? m : m - old_spec;
          if (num[n])
          {
            o = rec[n];
            p = (m < old_spec) ? 0 : 1;
            q = m - p*old_spec - o + p*new_spec;
            new_img -> linerad[l][q] = old_img  -> linerad[h][m];
            new_img -> bondrad[l][q] = old_img  -> bondrad[h][m];
          }
        }
      }
    }
    for (i=0; i<2; i++)
    {
      if (old_img -> show_atom[i] != NULL)
      {
        g_free (old_img -> show_atom[i]);
        old_img -> show_atom[i] = NULL;
      }
      if (old_img -> show_label[i] != NULL)
      {
        g_free (old_img -> show_label[i]);
        old_img -> show_label[i] = NULL;
      }
    }
    for (i=0; i<9; i++)
    {
      g_free (old_img -> spcolor[i]);
      old_img -> spcolor[i] = NULL;
    }
    g_free (old_img -> sphererad);
    g_free (old_img -> atomicrad);
    g_free (old_img -> pointrad);
    g_free (old_img -> at_color);
    g_free (old_img -> linerad);
    g_free (old_img -> bondrad);
    for (i=0; i<2; i++)
    {
      old_img -> show_atom[i] = duplicate_bool (new_spec, new_img -> show_atom[i]);
      old_img -> show_label[i] = duplicate_bool (new_spec, new_img -> show_label[i]);
    }
    old_img -> sphererad = duplicate_double (2*new_spec, new_img -> sphererad);
    old_img -> atomicrad = duplicate_double (2*new_spec, new_img -> atomicrad);
    old_img -> pointrad = duplicate_double (2*new_spec, new_img -> pointrad);
    old_img -> at_color = duplicate_color (2*new_spec, new_img -> at_color);
    old_img -> linerad = g_malloc (2*new_spec*sizeof*old_img -> linerad);
    old_img -> bondrad = g_malloc (2*new_spec*sizeof*old_img -> bondrad);
    for (i=0; i<2*new_spec; i++)
    {
      old_img -> linerad[i] = duplicate_double (2*new_spec, new_img -> linerad[i]);
      old_img -> bondrad[i] = duplicate_double (2*new_spec, new_img -> bondrad[i]);
    }
    for (i=0; i<9; i++)
    {
      old_img -> spcolor[i] = NULL;
      if (i < 2)
      {
        old_img -> spcolor[i] = g_malloc (new_spec*sizeof*old_img -> spcolor[i]);
      }
      else
      {
        old_img -> spcolor[i] = g_malloc (1*sizeof*old_img -> spcolor[i]);
        old_img -> spcolor[i][0] = NULL;
      }
    }
    g_free (new_img);
    update_it = TRUE;
  }
  else if (add)
  {
    setup_image_spec_data (this_proj, this_proj -> modelgl -> anim -> last -> img);
  }

  if (add || rem)
  {
#ifdef GTK3
    // GTK3 Menu Action To Check
    int i;
    for (i=1; i<OGL_COORDS; i++) this_proj -> modelgl -> ogl_coord[i] = NULL;
    for (i=0; i<OGL_RINGS; i++) this_proj -> modelgl -> ogl_rings[i] = NULL;
    this_proj -> modelgl -> ogl_chains[0] = NULL;
    prepare_opengl_menu_bar (this_proj -> modelgl);
    // GTK3 Menu Action To Check
    set_advanced_bonding_menus (this_proj -> modelgl);
#endif
  }
  new_coord_menus (this_proj, this_proj -> modelgl -> atom_win -> coord, new_spec, nmols, showcoord, showpoly, showfrag, update_it, (add || rem) ? FALSE : (! nmols) ? TRUE : FALSE);
}
