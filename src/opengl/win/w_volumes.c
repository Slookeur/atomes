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
* @file w_volumes.c
* @short Functions to create the 'Volumes' window \n
         Functions to compute system / fragment / molecular volumes
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_volumes.c'
*
* Contains:
*

 - The functions to create the 'Volumes' window
 - The functions to compute system / fragment / molecular volumes

*
* List of functions:

  double cap_volume (double ht, double dh);
  double get_sphere_caps_volume (double dab, double rad, double rbd);
  double sphere_volume (double rad);
  double get_atoms_volume (project * this_proj, int rid, int sid, int gid, int gcid);
  double molecular_volume (int nats, atom * ats_vol, double baryc[3], double * rvdws, double a_ang, double b_ang, double c_ang);
  double get_atoms_box (project * this_proj, int rid, int sid, int geo, int gid);

  void clean_volumes_data (glwin * view);
  void adjust_vol_md_step (project * this_proj, int geo);
  void add_frag_mol_vol_data (GtkWidget * vbox, project * this_proj, glwin * view, int geo);

  G_MODULE_EXPORT void molecular_volumes (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void fm_molecular_volumes (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void show_volumes (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void show_volumes (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void fm_show_volumes (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void fm_show_volumes (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_volume_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void fm_set_volume_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_md_step_vol (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void update_vol_frag_mol_search (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_angular_precision (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void window_volumes (GtkWidget * widg, gpointer data);

  GtkWidget * frag_mol_volume_search (project * this_proj, int g);
  GtkWidget * frag_mol_volume_tab (glwin * view, int geo);
  GtkWidget * vol_model_tab (glwin * view);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "glview.h"
#include "atom_edit.h"

extern void center_this_molecule (glwin * view);
extern double draw_cuboid (gboolean draw, int SHADID, int shadnum, mat4_t rot, vec3_t cpos, double paral[3][3], ColRGBA col, double slab_alpha);

/*!
  \fn void clean_volumes_data (glwin * view)

  \brief clean volume data

  \param view the target glwin
*/
void clean_volumes_data (glwin * view)
{
  if (view -> volume_win)
  {
    if (view -> volume_win -> win)
    {
      destroy_this_widget (view -> volume_win -> win);
    }
    g_free (view -> volume_win);
    view -> volume_win = NULL;
  }
  int i, j;
  for (i=0; i<FILLED_STYLES; i++)
  {
    if (view -> atoms_volume[i]) g_free (view -> atoms_volume[i]);
    view -> atoms_volume[i] = NULL;
    if (view -> atoms_ppvolume[i]) g_free (view -> atoms_ppvolume[i]);
    view -> atoms_ppvolume[i] = NULL;
    if (view -> volume_box[i]) g_free (view -> volume_box[i]);
    view -> volume_box[i] = NULL;
    view -> comp_vol[i] = FALSE;
    view -> anim -> last -> img -> show_vol[i] = FALSE;
  }
  for (i=0; i<2; i++)
  {
    for (j=0; j<FILLED_STYLES; j++)
    {
      if (view -> frag_mol_volume[i][j]) g_free (view -> frag_mol_volume[i][j]);
      view -> frag_mol_volume[i][j] = NULL;
      if (view -> frag_mol_ppvolume[i][j]) g_free (view -> frag_mol_ppvolume[i][j]);
      view -> frag_mol_ppvolume[i][j] = NULL;
      if (! i)
      {
        if (view -> frag_box[j]) g_free (view -> frag_box[j]);
        view -> frag_box[j] = NULL;
      }
      if (view -> fm_comp_vol[i][j]) g_free (view -> fm_comp_vol[i][j]);
      view -> fm_comp_vol[i][j] = NULL;
      if (view -> anim -> last -> img -> fm_show_vol[i][j]) g_free (view -> anim -> last -> img -> fm_show_vol[i][j]);
      view -> anim -> last -> img -> fm_show_vol[i][j] = NULL;
      if (view -> anim -> last -> img -> fm_vol_col[i][j]) g_free (view -> anim -> last -> img -> fm_vol_col[i][j]);
      view -> anim -> last -> img -> fm_vol_col[i][j] = NULL;
    }
  }
  int shaders[1] = {VOLMS};
  re_create_md_shaders (1, shaders, get_project_by_id(view -> proj));
}

/*!
  \fn double cap_volume (double ht, double dh)

  \brief compute cap volume

  \param ht cap height
  \param dh sphere radius
*/
double cap_volume (double ht, double dh)
{
  return (pi/3.0)*ht*ht*(3*dh-ht);
}

/*!
  \fn double get_sphere_caps_volume (double dab, double rad, double rbd)

  \brief compute sphere cap volume

  \param dab distance between the center of the 2 spheres
  \param rad sphere a radius
  \param rbd sphere b radius
*/
double get_sphere_caps_volume (double dab, double rad, double rbd)
{
  double h1, h2;
  double px;
  double rmi, rma;
  rmi = min (rad, rbd);
  rma = max (rad, rbd);
  px = (dab*dab - rmi*rmi + rma*rma) / (2.0*dab);
  h1 = rma - px;
  h2 = rmi - (dab - px);
  return cap_volume (h1, rma) + cap_volume (h2, rmi);
}

/*!
  \fn double sphere_volume (double rad)

  \brief compute sphere volume

  \param rad sphere radius
*/
double sphere_volume (double rad)
{
  return ((4.0*pi)/3.0)*rad*rad*rad;
}

/*!
  \fn double get_atoms_volume (project * this_proj, int rid, int sid, int gid, int gcid)

  \brief compute exact atomic volume for all system or fragment or molecule

  \param this_proj the target project
  \param rid the type of atomic radius(ii)
  \param sid the MD step
  \param gid -1 = all system, 2 = fragment(s), 3 = molecule(s)
  \param gcid fragment or molecule id number
*/
double get_atoms_volume (project * this_proj, int rid, int sid, int gid, int gcid)
{
  int i, j, k, l, m, n, o;
  double vol = 0.0;
  double cap_vol = 0.0;
  double * rvdws = allocdouble (this_proj -> nspec);
  distance dist;
  for (i=0; i<this_proj -> nspec; i++)
  {
    j = (int)this_proj -> chemistry -> chem_prop[CHEM_Z][i];
    rvdws[i] =  set_radius_ (& j, & rid);
  }
  for (j=0; j<2; j++)
  {
    for (k=0; k<this_proj -> modelgl -> bonds[sid][j]; k++)
    {
      l = this_proj -> modelgl -> bondid[sid][j][k][0];
      m = this_proj -> atoms[sid][l].sp;
      n = this_proj -> modelgl -> bondid[sid][j][k][1];
      o = this_proj -> atoms[sid][n].sp;
      if (gid < 0 || this_proj -> atoms[sid][l].coord[gid] == gcid)
      {
        dist = distance_3d (& this_proj -> cell, (this_proj -> cell.npt) ? sid : 0, & this_proj -> atoms[sid][l], & this_proj -> atoms[sid][n]);
        if (dist.length < rvdws[m]+rvdws[o]) cap_vol += get_sphere_caps_volume (dist.length, rvdws[m], rvdws[o]);
      }
    }
  }
  vol = 0.0;
  if (gid < 0)
  {
    for (i=0; i<this_proj -> nspec; i++)
    {
      vol += this_proj -> chemistry -> nsps[i] * sphere_volume (rvdws[i]);
    }
    vol -= cap_vol;
  }
  else
  {
    if (gid == 3)
    {
      for (i=0; i<this_proj -> nspec; i++)
      {
        vol += this_proj -> modelfc -> mols[sid][gcid].species[i] * sphere_volume (rvdws[i]);
      }
      vol *= this_proj -> modelfc -> mols[sid][gcid].multiplicity;
      vol -= cap_vol;
      vol /= this_proj -> modelfc -> mols[sid][gcid].multiplicity;
    }
    else
    {
      for (i=0; i<this_proj -> natomes; i++)
      {
        if (this_proj -> atoms[sid][i].coord[gid] == gcid)
        {
          j = this_proj -> atoms[sid][i].sp;
          vol += sphere_volume (rvdws[j]);
        }
      }
      vol -= cap_vol;
    }
  }
  g_free (rvdws);
  return vol;
}

double vamin[3], vamax[3];

/*!
  \fn double molecular_volume (int nats, atom * ats_vol, double baryc[3], double * rvdws, double a_ang, double b_ang, double c_ang)

  \brief compute volume

  \param nats number of atoms
  \param ats_vol the list of atom(s)
  \param baryc barycenter of the atomic coordinates
  \param rvdws the list of atomic radius (ii)
  \param a_ang x axis rotation angle
  \param b_ang y axis rotation angle
  \param c_ang z axis rotation angle
*/
double molecular_volume (int nats, atom * ats_vol, double baryc[3], double * rvdws, double a_ang, double b_ang, double c_ang)
{
  double paral[3][3];
  mat4_t rot;
  vec3_t c_old, c_new;
  rot = m4_rotation_xyz (a_ang, b_ang, c_ang);
  int i, j, k;
  k = 0;
  for (i=0; i<nats; i++)
  {
    j = ats_vol[i].sp;
    c_old = vec3(ats_vol[i].x+baryc[0], ats_vol[i].y+baryc[1], ats_vol[i].z+baryc[2]);
    c_new = m4_mul_pos (rot, c_old);
    if (! k)
    {
      vamin[0] = c_new.x - rvdws[j];
      vamax[0] = c_new.x + rvdws[j];
      vamin[1] = c_new.y - rvdws[j];
      vamax[1] = c_new.y + rvdws[j];
      vamin[2] = c_new.z - rvdws[j];
      vamax[2] = c_new.z + rvdws[j];
      k ++;
    }
    else
    {
      vamin[0] = min(vamin[0], c_new.x - rvdws[j]);
      vamax[0] = max(vamax[0], c_new.x + rvdws[j]);
      vamin[1] = min(vamin[1], c_new.y - rvdws[j]);
      vamax[1] = max(vamax[1], c_new.y + rvdws[j]);
      vamin[2] = min(vamin[2], c_new.z - rvdws[j]);
      vamax[2] = max(vamax[2], c_new.z + rvdws[j]);
    }
  }
  paral[0][0] = vamax[0] - vamin[0];
  paral[0][1] = 0.0;
  paral[0][2] = 0.0;
  paral[1][0] = 0.0;
  paral[1][1] = vamax[1] - vamin[1];
  paral[1][2] = 0.0;
  paral[2][0] = 0.0;
  paral[2][1] = 0.0;
  paral[2][2] = vamax[2] - vamin[2];
  ColRGBA null; // Not used
  return draw_cuboid (FALSE, VOLMS, 0, m4_identity (), vec3(0.0,0.0,0.0), paral, null, 1.0);
}

/*!
  \fn double get_atoms_box (project * this_proj, int rid, int sid, int geo, int gid)

  \brief find volume box parameters for object

  \param this_proj the target project
  \param rid type of radius
  \param sid the MD step
  \param geo -1 = all system, 2 = fragment(s), 3 = molecule(s)
  \param gid fragment or molecule id number
*/
double get_atoms_box (project * this_proj, int rid, int sid, int geo, int gid)
{
  int i, j;
  double * rvdws = allocdouble (this_proj -> nspec);
  for (i=0; i<this_proj -> nspec; i++)
  {
    j = (int)this_proj -> chemistry -> chem_prop[CHEM_Z][i];
    rvdws[i] =  set_radius_ (& j, & rid);
  }
  if (geo < 0) center_this_molecule (this_proj -> modelgl);
  int a_ang, b_ang, c_ang;
  double val, vbl;
  int finess = pow (10, this_proj -> modelgl -> volume_win -> angp);
  int nats;
  atom * ats_vol = NULL;
  atomic_object * object = NULL;
  gboolean rtmp;
  double * baryc;
  switch (geo)
  {
    case 2:
      object_motion = TRUE;
      rtmp = this_proj -> modelgl -> rebuild[0][0];
      this_proj -> modelgl -> rebuild[0][0] = TRUE;
      object = create_object_from_frag_mol (this_proj, geo, gid, NULL);
      object_motion = FALSE;
      this_proj -> modelgl -> rebuild[0][0] = rtmp;
      nats = object -> atoms;
      baryc = duplicate_double (3, object -> baryc);
      ats_vol = object -> at_list;
      break;
    default:
      nats = this_proj -> natomes;
      ats_vol = this_proj -> atoms[sid];
      baryc = allocdouble (3);
      break;
  }

  for (a_ang=0; a_ang < 90*finess; a_ang ++)
  {
    for (b_ang=0; b_ang < 90*finess; b_ang ++)
    {
      for (c_ang=0; c_ang < 90*finess; c_ang ++)
      {
        val = molecular_volume (nats, ats_vol, baryc, rvdws, a_ang/finess, b_ang/finess, c_ang/finess);
        if (a_ang == 0 && b_ang == 0 && c_ang == 0)
        {
          vbl = val;
          if (geo < 0)
          {
            for (i=0; i<3; i++)
            {
              this_proj -> modelgl -> volume_box[rid][sid][i] = vamax[i] - vamin[i];
              this_proj -> modelgl -> volume_box[rid][sid][i+6] = (vamax[i] + vamin[i]) / 2.0;
            }
            this_proj -> modelgl -> volume_box[rid][sid][3] = a_ang/finess;
            this_proj -> modelgl -> volume_box[rid][sid][4] = b_ang/finess;
            this_proj -> modelgl -> volume_box[rid][sid][5] = c_ang/finess;
          }
          else
          {
            for (i=0; i<3; i++)
            {
              this_proj -> modelgl -> frag_box[rid][sid][gid][i] = vamax[i] - vamin[i];
              this_proj -> modelgl -> frag_box[rid][sid][gid][i+6] = (vamax[i] + vamin[i]) / 2.0;
            }
            this_proj -> modelgl -> frag_box[rid][sid][gid][3] = a_ang/finess;
            this_proj -> modelgl -> frag_box[rid][sid][gid][4] = b_ang/finess;
            this_proj -> modelgl -> frag_box[rid][sid][gid][5] = c_ang/finess;
          }
        }
        else if (val < vbl)
        {
          vbl = val;
          if (geo < 0)
          {
            for (i=0; i<3; i++)
            {
              this_proj -> modelgl -> volume_box[rid][sid][i] = vamax[i] - vamin[i];
              this_proj -> modelgl -> volume_box[rid][sid][i+6] = (vamax[i] + vamin[i]) / 2.0;
            }
            this_proj -> modelgl -> volume_box[rid][sid][3] = a_ang/finess;
            this_proj -> modelgl -> volume_box[rid][sid][4] = b_ang/finess;
            this_proj -> modelgl -> volume_box[rid][sid][5] = c_ang/finess;
          }
          else
          {
            for (i=0; i<3; i++)
            {
              this_proj -> modelgl -> frag_box[rid][sid][gid][i] = vamax[i] - vamin[i];
              this_proj -> modelgl -> frag_box[rid][sid][gid][i+6] = (vamax[i] + vamin[i]) / 2.0;
            }
            this_proj -> modelgl -> frag_box[rid][sid][gid][3] = a_ang/finess;
            this_proj -> modelgl -> frag_box[rid][sid][gid][4] = b_ang/finess;
            this_proj -> modelgl -> frag_box[rid][sid][gid][5] = c_ang/finess;
          }
        }
        else
        {
          break;
        }
      }
    }
  }
  ats_vol = NULL;
  g_free (rvdws);
  g_free (baryc);
  if (geo == 2) g_free (object);
  return vbl;
}

/*!
  \fn G_MODULE_EXPORT void molecular_volumes (GtkButton * but, gpointer data)

  \brief compute volume

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void molecular_volumes (GtkButton * but, gpointer data)
{
  tint * dat = (tint *)data;
  project * this_proj = get_project_by_id (dat -> a);
  int i;
  for (i=0; i<this_proj -> steps; i++) this_proj -> modelgl -> atoms_ppvolume[dat -> b][i] = get_atoms_box (this_proj, dat -> b, i, -1, 0);
  this_proj -> modelgl -> comp_vol[dat -> b] = TRUE;
  widget_set_sensitive (this_proj -> modelgl -> volume_win -> hbvol[dat -> b], this_proj -> modelgl -> comp_vol[dat -> b]);
  double movol = 0.0;
  for (i=0; i<this_proj -> steps; i++) movol += this_proj -> modelgl -> atoms_ppvolume[dat -> b][i];
  movol /= this_proj -> steps;
  gchar * str = g_strdup_printf ("%15.3f", movol);
  gtk_label_set_text  ((GtkLabel *)this_proj -> modelgl -> volume_win -> lab_vol[dat -> b], str);
  g_free (str);
  if (this_proj -> modelgl -> comp_vol[dat -> b])
  {
    hide_the_widgets (this_proj -> modelgl -> volume_win -> compb[dat -> b]);
    show_the_widgets (this_proj -> modelgl -> volume_win -> hboxv[dat -> b]);
  }
  this_proj -> modelgl -> volumes = TRUE;
}

/*!
  \fn void adjust_vol_md_step (project * this_proj, int geo)

  \brief update volume value labels

  \param this_proj the target project
  \param geo 2 = fragment(s), 3 = molecule(s)
*/
void adjust_vol_md_step (project * this_proj, int geo)
{
  int i, j, k;
  k = this_proj -> modelgl -> volume_win -> sid[geo-2];
  gchar * str;
  if (this_proj -> coord -> totcoord[geo] <= 10000)
  {
    for (i=0; i<FILLED_STYLES; i++)
    {
      for (j=0; j<this_proj -> coord -> totcoord[geo]; j++)
      {
        if (this_proj -> modelgl -> fm_comp_vol[geo-2][i][k][j])
        {
          hide_the_widgets (this_proj -> modelgl -> volume_win -> fm_compb[geo-2][i][j]);
          str = g_strdup_printf ("%15.3f", this_proj -> modelgl -> frag_mol_ppvolume[geo-2][i][k][j]);
          gtk_label_set_text  ((GtkLabel *)this_proj -> modelgl -> volume_win -> fm_lab_vol[geo-2][i][j], str);
          g_free (str);
          show_the_widgets (this_proj -> modelgl -> volume_win -> fm_hboxv[geo-2][i][j]);
        }
        else
        {
          show_the_widgets (this_proj -> modelgl -> volume_win -> fm_compb[geo-2][i][j]);
          hide_the_widgets (this_proj -> modelgl -> volume_win -> fm_hboxv[geo-2][i][j]);
        }
      }
    }
  }
  else
  {
    for (i=0; i<FILLED_STYLES; i++)
    {
      for (j=0; j<this_proj -> modelgl -> volume_win -> ngeov[geo-2]; j++)
      {
        k =  this_proj -> modelgl -> volume_win -> geov_id[geo-2][j];
        if (this_proj -> modelgl -> fm_comp_vol[geo-2][i][0][k])
        {
          hide_the_widgets (this_proj -> modelgl -> volume_win -> fm_compb[geo-2][i][k]);
          str = g_strdup_printf ("%15.3f", this_proj -> modelgl -> frag_mol_ppvolume[geo-2][i][0][k]);
          gtk_label_set_text  ((GtkLabel *)this_proj -> modelgl -> volume_win -> fm_lab_vol[geo-2][i][k], str);
          g_free (str);
          show_the_widgets (this_proj -> modelgl -> volume_win -> fm_hboxv[geo-2][i][k]);
        }
        else
        {
          show_the_widgets (this_proj -> modelgl -> volume_win -> fm_compb[geo-2][i][k]);
          hide_the_widgets (this_proj -> modelgl -> volume_win -> fm_hboxv[geo-2][i][k]);
        }
      }
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void fm_molecular_volumes (GtkButton * but, gpointer data)

  \brief compute fragment / molecule volume

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void fm_molecular_volumes (GtkButton * but, gpointer data)
{
  int i, j;
  qint * dat = (qint *)data;
  project * this_proj = get_project_by_id (dat -> a);
  int sid = this_proj -> modelgl -> volume_win -> sid[dat -> b-2];
  if (dat -> b == 2)
  {
    this_proj -> modelgl -> frag_mol_ppvolume[dat -> b-2][dat -> d][sid][dat -> c] = get_atoms_box (this_proj, dat -> d, sid, dat -> b, dat -> c);
  }
  else
  {
    this_proj -> modelgl -> frag_mol_ppvolume[dat -> b-2][dat -> d][sid][dat -> c] = 0.0;
    for (i=0; i<this_proj -> modelfc -> mols[sid][dat -> c].multiplicity; i++)
    {
      j = this_proj -> modelfc -> mols[sid][dat -> c].fragments[i];
      if (! this_proj -> modelgl -> fm_comp_vol[0][dat -> d][sid][j])
      {
        this_proj -> modelgl -> frag_mol_ppvolume[0][dat -> d][sid][j] = get_atoms_box (this_proj, dat -> d, sid, 2, j);
        this_proj -> modelgl -> fm_comp_vol[0][dat -> d][sid][j] = TRUE;
        widget_set_sensitive (this_proj -> modelgl -> volume_win -> fm_hbvol[0][dat -> d][j], this_proj -> modelgl -> fm_comp_vol[0][dat -> d][sid][j]);
      }
      this_proj -> modelgl -> frag_mol_ppvolume[dat -> b-2][dat -> d][sid][dat -> c] += this_proj -> modelgl -> frag_mol_ppvolume[0][dat -> d][sid][j];
    }
    this_proj -> modelgl -> frag_mol_ppvolume[dat -> b-2][dat -> d][sid][dat -> c] /= this_proj -> modelfc -> mols[sid][dat -> c].multiplicity;
  }
  center_molecule (this_proj);
  for (i=0; i<2; i++) this_proj -> modelgl -> saved_coord[i] = save_coordinates (this_proj, i);
  init_default_shaders (this_proj -> modelgl);
  update (this_proj -> modelgl);
  this_proj -> modelgl -> fm_comp_vol[dat -> b-2][dat -> d][sid][dat -> c] = TRUE;
  widget_set_sensitive (this_proj -> modelgl -> volume_win -> fm_hbvol[dat -> b-2][dat -> d][dat -> c], this_proj -> modelgl -> fm_comp_vol[dat -> b-2][dat -> d][sid][dat -> c]);
  gchar * str = g_strdup_printf ("%15.3f", this_proj -> modelgl -> frag_mol_ppvolume[dat -> b-2][dat -> d][sid][dat -> c]);
  gtk_label_set_text  ((GtkLabel *)this_proj -> modelgl -> volume_win -> fm_lab_vol[dat -> b-2][dat -> d][dat -> c], str);
  g_free (str);
  if (this_proj -> modelgl -> fm_comp_vol[dat -> b-2][dat -> d][sid][dat -> c])
  {
    hide_the_widgets (this_proj -> modelgl -> volume_win -> fm_compb[dat -> b-2][dat -> d][dat -> c]);
    show_the_widgets (this_proj -> modelgl -> volume_win -> fm_hboxv[dat -> b-2][dat -> d][dat -> c]);
  }
  if (dat -> b == 3)
  {
    adjust_vol_md_step (this_proj, 2);
  }
  this_proj -> modelgl -> volumes = TRUE;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void show_volumes (GtkCheckButton * but, gpointer data)

  \brief toggle show / hide volume callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_volumes (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void show_volumes (GtkToggleButton * but, gpointer data)

  \brief toggle show / hide volume callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_volumes (GtkToggleButton * but, gpointer data)
#endif
{
  tint * dat = (tint *)data;
  project * this_proj = get_project_by_id (dat -> a);
  this_proj -> modelgl -> anim -> last -> img -> show_vol[dat -> b] = button_get_status ((GtkWidget *)but);
  int shaders[1] = {VOLMS};
  re_create_md_shaders (1, shaders, this_proj);
  update (this_proj -> modelgl);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void fm_show_volumes (GtkCheckButton * but, gpointer data)

  \brief  toggle show / hide fragment / molecule volume callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void fm_show_volumes (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void fm_show_volumes (GtkToggleButton * but, gpointer data)

  \brief toggle show / hide fragment / molecule volume callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void fm_show_volumes (GtkToggleButton * but, gpointer data)
#endif
{
  qint * dat = (qint *)data;
  project * this_proj = get_project_by_id (dat -> a);
  this_proj -> modelgl -> anim -> last -> img -> fm_show_vol[dat -> b-2][dat -> d][dat -> c] = button_get_status ((GtkWidget *)but);
  int shaders[1] = {VOLMS};
  re_create_md_shaders (1, shaders, this_proj);
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void set_volume_color (GtkColorChooser * colob, gpointer data)

  \brief change volume color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_volume_color (GtkColorChooser * colob, gpointer data)
{
  tint * id = (tint *) data;
  project * this_proj = get_project_by_id(id -> a);
  this_proj -> modelgl -> anim -> last -> img -> vol_col[id -> b] = get_button_color (colob);
  int shaders[1] = {VOLMS};
  re_create_md_shaders (1, shaders, this_proj);
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void fm_set_volume_color (GtkColorChooser * colob, gpointer data)

  \brief change fragment / molecule volume color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void fm_set_volume_color (GtkColorChooser * colob, gpointer data)
{
  qint * dat = (qint *)data;
  project * this_proj = get_project_by_id (dat -> a);
  this_proj -> modelgl -> anim -> last -> img -> fm_vol_col[dat -> b-2][dat -> d][dat -> c] = get_button_color (colob);
  int shaders[1] = {VOLMS};
  re_create_md_shaders (1, shaders, this_proj);
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void set_md_step_vol (GtkSpinButton * res, gpointer data)

  \brief change MD step spin callback

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_md_step_vol (GtkSpinButton * res, gpointer data)
{
  qint * dat = (qint *)data;
  project * this_proj = get_project_by_id (dat -> a);
  this_proj -> modelgl -> volume_win -> sid[dat -> b-2] = gtk_spin_button_get_value_as_int(res);
  adjust_vol_md_step (this_proj, dat -> b);
}

/*!
  \fn void add_frag_mol_vol_data (GtkWidget * vbox, project * this_proj, glwin * view, int geo)

  \brief add fragment / molecule volume data to the search tab

  \param vbox the GtkWidget sending the signal
  \param this_proj the target project
  \param view the target glwin
  \param geo 2 = fragment(s), 3 = molecule(s)
*/
void add_frag_mol_vol_data (GtkWidget * vbox, project * this_proj, glwin * view, int geo)
{
  gchar * name_geo[2] = {"fragment", "molecule"};
  gchar * fmo[2] = {"Fragment", "Molecule"};
  GtkWidget * hbox;
  GtkWidget * hhbox;
  GtkWidget * fragtab;
  gchar * str;
  int i, j;
  int ngeov = (this_proj -> coord -> totcoord[geo] > 10000) ? view -> volume_win -> ngeov[geo-2] : this_proj -> coord -> totcoord[geo];
  if (geo == 3)
  {
    str = g_strdup_printf ("<u>Mean volume(s) occupied by the atom(s) for each %s:</u>", name_geo[geo-2]);
  }
  else
  {
    str = g_strdup_printf ("<u>Volume(s) occupied by the atom(s) for each %s:</u>", name_geo[geo-2]);
  }
  abox (vbox, str, 5);
  g_free (str);
  fragtab = create_scroll (vbox, -1, -1, GTK_SHADOW_NONE);
  gtk_widget_set_hexpand (fragtab, TRUE);
  gtk_widget_set_vexpand (fragtab, TRUE);
  GtkWidget * vvbox = create_vbox (BSEP);
  add_container_child (CONTAINER_SCR, fragtab, vvbox);
  double vof;
  int geoid;
  for (i=0; i<ngeov; i++)
  {
    geoid = (this_proj -> coord -> totcoord[geo] > 10000) ? view -> volume_win -> geov_id[geo-2][i] : i;
    str = g_strdup_printf ("%s N°%d", fmo[geo-2], geoid+1);
    hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 200, -1, 0.0, 0.5), FALSE, FALSE, 20);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hbox, FALSE, FALSE, 5);
    g_free (str);
    for (j=0; j<FILLED_STYLES; j++)
    {
      hbox = create_hbox (BSEP);
      str = g_strdup_printf ("using <b>%s</b>: ", text_filled[j]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 200, -1, 0.0, 0.5), FALSE, FALSE, 30);
      g_free (str);
      view -> frag_mol_volume[geo-2][j][0][geoid] = get_atoms_volume (this_proj, j, 0, geo, i);
      str = g_strdup_printf ("%15.3f", view -> frag_mol_volume[geo-2][j][0][geoid]);
      hhbox = create_hbox (BSEP);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 100, -1, 1.0, 0.5), FALSE, FALSE, 0);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("<b>&#xC5;<sup>3</sup></b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 20);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hbox, FALSE, FALSE, 5);
    }
  }
  if (geo == 3)
  {
    str = g_strdup_printf ("<u>Mean smallest rectangle parallepiped volume(s) to englobe the atom(s) for each %s:</u>", name_geo[geo-2]);
  }
  else
  {
    str = g_strdup_printf ("<u>Smallest rectangle parallepiped volume(s) to englobe the atom(s) for each %s:</u>", name_geo[geo-2]);
  }
  abox (vbox, str, 5);
  g_free (str);
  fragtab = create_scroll (vbox, -1, -1, GTK_SHADOW_NONE);
  gtk_widget_set_hexpand (fragtab, TRUE);
  gtk_widget_set_vexpand (fragtab, TRUE);
  vvbox = create_vbox (BSEP);
  add_container_child (CONTAINER_SCR, fragtab, vvbox);
  for (i=0; i<ngeov; i++)
  {
    geoid = (this_proj -> coord -> totcoord[geo] > 10000) ? view -> volume_win -> geov_id[geo-2][i] : i;
    str = g_strdup_printf ("%s N°%d", fmo[geo-2], geoid+1);
    hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 200, -1, 0.0, 0.5), FALSE, FALSE, 20);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hbox, FALSE, FALSE, 5);
    g_free (str);
    for (j=0; j<FILLED_STYLES; j++)
    {
      hbox = create_hbox (BSEP);
      str = g_strdup_printf ("using <b>%s</b>: ", text_filled[j]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 200, -1, 0.0, 0.5), FALSE, FALSE, 30);
      g_free (str);
      hhbox = create_hbox (BSEP);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, FALSE, FALSE, 0);
      view -> volume_win -> fm_compb[geo-2][j][geoid] = create_button ("Compute", IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(fm_molecular_volumes), & view -> gcid[geo][geoid][j]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, view -> volume_win -> fm_compb[geo-2][j][geoid], FALSE, FALSE, 20);
      vof = 0.0;
      if (view -> frag_mol_ppvolume[geo-2][j]) vof = view -> frag_mol_ppvolume[geo-2][j][0][geoid];
      str = g_strdup_printf ("%15.3f",  vof);
      view -> volume_win -> fm_hboxv[geo-2][j][geoid] = create_hbox (BSEP);
      view -> volume_win -> fm_lab_vol[geo-2][j][geoid] = markup_label (str, 100, -1, 1.0, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> volume_win -> fm_hboxv[geo-2][j][geoid] , view -> volume_win -> fm_lab_vol[geo-2][j][geoid], FALSE, FALSE, 20);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> volume_win -> fm_hboxv[geo-2][j][geoid] , markup_label ("<b>&#xC5;<sup>3</sup></b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, view -> volume_win -> fm_hboxv[geo-2][j][geoid] , FALSE, FALSE, 0);
      view -> volume_win -> fm_hbvol[geo-2][j][geoid] = create_hbox (BSEP);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> volume_win -> fm_hbvol[geo-2][j][geoid], check_button ("Show/Hide", 100, -1, view -> anim -> last -> img -> fm_show_vol[geo-2][j][geoid], G_CALLBACK(fm_show_volumes), & view -> gcid[geo][geoid][j]), FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> volume_win -> fm_hbvol[geo-2][j][geoid], color_button (view -> anim -> last -> img -> fm_vol_col[geo-2][j][geoid], TRUE, 50, -1, G_CALLBACK(fm_set_volume_color), & view -> gcid[geo][geoid][j]), FALSE, FALSE, 5);
      widget_set_sensitive (view -> volume_win -> fm_hbvol[geo-2][j][geoid], view -> fm_comp_vol[geo-2][j][0][geoid]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, view -> volume_win -> fm_hbvol[geo-2][j][geoid], FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hbox, FALSE, FALSE, 0);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void update_vol_frag_mol_search (GtkEntry * res, gpointer data)

  \brief update fragment / molecule search entry

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_vol_frag_mol_search (GtkEntry * res, gpointer data)
{
  tint * dat = (tint * )data;
  const gchar * m = entry_get_text (res);
  int v = (int)string_to_double ((gpointer)m);
  project * this_proj = get_project_by_id(dat -> a);
  int g = dat -> b;
  if (v > 0 && v <= this_proj -> coord -> totcoord[g])
  {
    int i;
    gboolean update = TRUE;
    for (i=0; i<this_proj -> modelgl -> volume_win -> ngeov[g-2]; i++)
    {
      if (this_proj -> modelgl -> volume_win -> geov_id[g-2][i] == v-1)
      {
        update = FALSE;
        break;
      }
    }
    if (update)
    {
      int * sdata = duplicate_int (this_proj -> modelgl -> volume_win -> ngeov[g-2], this_proj -> modelgl -> volume_win -> geov_id[g-2]);
      g_free (this_proj -> modelgl -> volume_win -> geov_id[g-2]);
      this_proj -> modelgl -> volume_win -> geov_id[g-2] = allocint (this_proj -> modelgl -> volume_win -> ngeov[g-2]+1);
      for (i=0; i<this_proj -> modelgl -> volume_win -> ngeov[g-2]; i++)
      {
        this_proj -> modelgl -> volume_win -> geov_id[g-2][i] = sdata[i];
      }
      this_proj -> modelgl -> volume_win -> geov_id[g-2][i] = v-1;
      this_proj -> modelgl -> volume_win -> ngeov[g-2] ++;

      destroy_this_widget (this_proj -> modelgl -> volume_win -> fm_vvbox[g-2]);
      this_proj -> modelgl -> volume_win -> fm_vvbox[g-2] = create_vbox (BSEP);
      add_frag_mol_vol_data (this_proj -> modelgl -> volume_win -> fm_vvbox[g-2], this_proj, this_proj -> modelgl, g);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, this_proj -> modelgl -> volume_win -> fm_vbox[g-2], this_proj -> modelgl -> volume_win -> fm_vvbox[g-2], TRUE, TRUE, 0);
      show_the_widgets (this_proj -> modelgl -> volume_win -> fm_vvbox[g-2]);
      adjust_vol_md_step (this_proj, g);
    }
  }
  else
  {
    update_entry_text (res, "");
  }
}

/*!
  \fn GtkWidget * frag_mol_volume_search (project * this_proj, int g)

  \brief create the fragment(s) / molecule(s) search widget

  \param this_proj the target project
  \param g 2 = fragment(s), 3 = molecule(s)
*/
GtkWidget * frag_mol_volume_search (project * this_proj, int g)
{
  GtkWidget * frag_mol_search = create_vbox (BSEP);
  gchar * obj[2] = {"fragment", "molecule"};
  gchar * str = g_strdup_printf ("Too many <b>%ss</b> in your model !\n"
                                 "  It is impossible to display the entire list ...\n"
                                 "... instead you can look for %s(s) 'manually':\n", obj[g-2], obj[g-2]);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, frag_mol_search, markup_label(str, 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  g_free (str);
  gchar * search_item[2]={"Fragment ID:", "Molecule ID:"};
  GtkWidget * hbox;
  GtkWidget * entry;
  GtkWidget * label;
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, frag_mol_search, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(search_item[g-2], 100, -1, 0.0, 0.5), FALSE, FALSE, 20);
  entry = create_entry (G_CALLBACK(update_vol_frag_mol_search), 100, 15, FALSE, & this_proj -> modelgl -> colorp[g][0]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,entry, FALSE, FALSE, 0);
  str = g_strdup_printf ("in [%d - %d]", 1, this_proj ->  coord -> totcoord[g]);
  label = markup_label (str, 50, -1, 0.0, 0.5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,label, FALSE, FALSE, 20);
  g_free (str);
  return frag_mol_search;
}

/*!
  \fn GtkWidget * frag_mol_volume_tab (glwin * view, int geo)

  \brief create the fragment(s) / molecule(s) tab

  \param view the target glwin
  \param geo 2 = fragment(s), 3 = molecule(s)
*/
GtkWidget * frag_mol_volume_tab (glwin * view, int geo)
{
  GtkWidget * vbox = create_vbox (BSEP);
  project * this_proj = get_project_by_id (view -> proj);
  GtkWidget * hbox;
  gchar * str;
  if (this_proj -> steps > 1)
  {
    // MD step box
    GtkWidget * sbut = spin_button (G_CALLBACK(set_md_step_vol), view -> volume_win -> sid[geo-2], 1.0, this_proj -> steps, 1.0, 0, 1000, & view -> gcid[geo][0][geo]);
    gtk_widget_set_size_request (sbut, 25, -1);
    GtkWidget * fix = gtk_fixed_new ();
    gtk_fixed_put (GTK_FIXED (fix), sbut, 0, 10);
    str = g_strdup_printf ("<u>Select MD step [1-%d]:</u>", this_proj -> steps);
    hbox = abox (vbox, str, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fix, FALSE, FALSE, 20);
  }

  int i, j;
  for (i=0; i<FILLED_STYLES; i++)
  {
    if (! view -> fm_comp_vol[geo-2][i])
    {
      view -> fm_comp_vol[geo-2][i] = allocdbool (this_proj -> steps, this_proj -> coord -> totcoord[geo]);
    }
    if (! view -> anim -> last -> img -> fm_show_vol[geo-2][i])
    {
      view -> anim -> last -> img -> fm_show_vol[geo-2][i] = allocbool (this_proj -> coord -> totcoord[geo]);
    }
    if (! view -> anim -> last -> img -> fm_vol_col[geo-2][i])
    {
      view -> anim -> last -> img -> fm_vol_col[geo-2][i] = g_malloc0(this_proj -> coord -> totcoord[geo]*sizeof*view -> anim -> last -> img -> fm_vol_col[geo-2][i]);
      for (j=0; j<this_proj -> coord -> totcoord[geo]; j++)
      {
        view -> anim -> last -> img -> fm_vol_col[geo-2][i][j] = init_color (j+i*this_proj -> coord -> totcoord[geo], FILLED_STYLES*this_proj -> coord -> totcoord[geo]);
        view -> anim -> last -> img -> fm_vol_col[geo-2][i][j].alpha = 0.75;
      }
    }
    if (! view -> frag_box[i])
    {
      view -> frag_box[i] = alloctdouble (this_proj -> steps, this_proj -> coord -> totcoord[geo], 9);
    }
    if (! view -> frag_mol_volume[geo-2][i])
    {
      view -> frag_mol_volume[geo-2][i] = allocddouble (this_proj -> steps, this_proj -> coord -> totcoord[geo]);
    }
    if (! view -> frag_mol_ppvolume[geo-2][i])
    {
      view -> frag_mol_ppvolume[geo-2][i] = allocddouble (this_proj -> steps, this_proj -> coord -> totcoord[geo]);
    }
    if (! view -> volume_win -> fm_compb[geo-2][i])
    {
      view -> volume_win -> fm_compb[geo-2][i] = g_malloc0 (this_proj -> coord -> totcoord[geo]*sizeof*view -> volume_win ->fm_compb[geo-2][i]);
    }
    if (! view -> volume_win -> fm_hbvol[geo-2][i])
    {
      view -> volume_win -> fm_hbvol[geo-2][i] = g_malloc0 (this_proj -> coord -> totcoord[geo]*sizeof*view -> volume_win -> fm_hbvol[geo-2][i]);
    }
    if (! view -> volume_win -> fm_hboxv[geo-2][i])
    {
      view -> volume_win -> fm_hboxv[geo-2][i] = g_malloc0 (this_proj -> coord -> totcoord[geo]*sizeof*view -> volume_win -> fm_hboxv[geo-2][i]);
    }
    if (! view -> volume_win -> fm_lab_vol[geo-2][i])
    {
      view -> volume_win -> fm_lab_vol[geo-2][i] = g_malloc0 (this_proj -> coord -> totcoord[geo]*sizeof*view -> volume_win -> fm_lab_vol[geo-2][i]);
    }
  }
  GtkWidget * fragtab;
  if (this_proj -> coord -> totcoord[geo] >  10000)
  {
    fragtab = create_scroll (vbox, -1, -1, GTK_SHADOW_NONE);
    gtk_widget_set_hexpand (fragtab, TRUE);
    gtk_widget_set_vexpand (fragtab, TRUE);
    add_container_child (CONTAINER_SCR, fragtab, frag_mol_volume_search(this_proj, geo));
  }
  view -> volume_win -> fm_vbox[geo-2] = create_vbox (BSEP);
  view -> volume_win -> fm_vvbox[geo-2] = create_vbox (BSEP);
  add_frag_mol_vol_data (view -> volume_win -> fm_vvbox[geo-2], this_proj, view, geo);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, view -> volume_win -> fm_vbox[geo-2], view -> volume_win -> fm_vvbox[geo-2], TRUE, TRUE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, view -> volume_win -> fm_vbox[geo-2], TRUE, TRUE, 0);
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void set_angular_precision (GtkComboBox * box, gpointer data)

  \brief change angular precision

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_angular_precision (GtkComboBox * box, gpointer data)
{
  glwin * view = (glwin *)data;
  int i = combo_get_active ((GtkWidget *)box);
  if (i != view -> volume_win -> angp)
  {
    project * this_proj = get_project_by_id (view -> proj);
    view -> volume_win -> angp = i;
    int j, k;
    for (i=0; i<FILLED_STYLES; i++)
    {
      show_the_widgets (view -> volume_win -> compb[i]);
      hide_the_widgets (view -> volume_win -> hboxv[i]);
      view -> comp_vol[i] = FALSE;
      widget_set_sensitive (view -> volume_win -> hbvol[i], view -> comp_vol[i]);
      view -> anim -> last -> img -> show_vol[i] = FALSE;
      for (j=0; j<2; j++)
      {
        for (k=0; k<this_proj -> coord -> totcoord[j+2]; k++)
        {
          show_the_widgets (view -> volume_win -> fm_compb[j][i][k]);
          hide_the_widgets (view -> volume_win -> fm_hboxv[j][i][k]);
          view -> fm_comp_vol[j][i][0][k] = FALSE;
          widget_set_sensitive (view -> volume_win -> fm_hbvol[j][i][k], view -> fm_comp_vol[j][i][0][k]);
          view -> anim -> last -> img -> fm_show_vol[j][i][k] = FALSE;
        }
      }
    }
    this_proj -> modelgl -> volumes = FALSE;
    int shaders[1] = {VOLMS};
    re_create_md_shaders (1, shaders, this_proj);
    update (view);
  }
}

/*!
  \fn GtkWidget * vol_model_tab (glwin * view)

  \brief create the 'Model' volume tab

  \param view the target glwin
*/
GtkWidget * vol_model_tab (glwin * view)
{
  GtkWidget * vbox = create_vbox (BSEP);
  abox (vbox, "<u>Volume(s) occupied by all the atom(s):</u>", 5);
  GtkWidget * hbox;
  int i, j;
  project * this_proj = get_project_by_id (view -> proj);
  GtkWidget * hhbox;
  gchar * str;
  double vof;
  for (i=0; i<FILLED_STYLES; i++)
  {
    hbox = create_hbox (BSEP);
    str = g_strdup_printf ("using <b>%s</b>: ", text_filled[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 200, -1, 0.0, 0.5), FALSE, FALSE, 30);
    g_free (str);
    if (! view -> atoms_volume[i]) view -> atoms_volume[i] = allocdouble (this_proj -> steps);
    for (j=0; j<this_proj -> steps; j++) view -> atoms_volume[i][j] = get_atoms_volume (this_proj, i, j, -1, 0);
    vof = 0.0;
    for (j=0; j<this_proj -> steps; j++) vof += view -> atoms_volume[i][j];
    vof /= this_proj -> steps;
    str = g_strdup_printf ("%15.3f", vof);
    hhbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 100, -1, 1.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("<b>&#xC5;<sup>3</sup></b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 20);
    if (this_proj -> steps > 1)
    {
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("average by MD step", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    }
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  }
  abox (vbox, "<u>Smallest rectangle parallepiped volume(s) to englobe all atom(s):</u>", 5);
  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new("Angular precision: "), FALSE, FALSE, 10);
  GtkWidget * ang_combo = create_combo ();
  gchar * angpr[4] = {"°", "°/10", "°/100", "°/1000"};
  for (i=0; i<4; i++) combo_text_append (ang_combo, angpr[i]);
  combo_set_active (ang_combo, view -> volume_win -> angp);
  g_signal_connect (G_OBJECT (ang_combo), "changed", G_CALLBACK(set_angular_precision), view);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ang_combo, FALSE, FALSE, 0);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  for (i=0; i<FILLED_STYLES; i++)
  {
    hbox = create_hbox (BSEP);
    str = g_strdup_printf ("using <b>%s</b>: ", text_filled[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 200, -1, 0.0, 0.5), FALSE, FALSE, 30);
    g_free (str);
    hhbox = create_hbox (BSEP);
    if (! view -> comp_vol[i])
    {
      view -> anim -> last -> img -> vol_col[i].red = (i+1.0)/FILLED_STYLES;
      view -> anim -> last -> img -> vol_col[i].green = 1.0;
      view -> anim -> last -> img -> vol_col[i].blue = 0.0;
      view -> anim -> last -> img -> vol_col[i].alpha = 0.75;
    }
    if (! view -> atoms_ppvolume[i]) view -> atoms_ppvolume[i] = allocdouble (this_proj -> steps);
    if (! view -> volume_box[i]) view -> volume_box[i] = allocddouble (this_proj -> steps, 9);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, FALSE, FALSE, 0);
    view -> volume_win -> compb[i] = create_button ("Compute", IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(molecular_volumes), & view -> colorp[i][0]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, view -> volume_win -> compb[i], FALSE, FALSE, 20);
    vof = 0.0;
    for (j=0; j<this_proj -> steps; j++) vof += view -> atoms_ppvolume[i][j];
    vof /= this_proj -> steps;
    str = g_strdup_printf ("%15.3f",  vof);
    view -> volume_win -> hboxv[i] = create_hbox (BSEP);
    view -> volume_win -> lab_vol[i] = markup_label (str, 100, -1, 1.0, 0.5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> volume_win -> hboxv[i], view -> volume_win -> lab_vol[i], FALSE, FALSE, 20);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> volume_win -> hboxv[i], markup_label ("<b>&#xC5;<sup>3</sup></b>", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, view -> volume_win -> hboxv[i], FALSE, FALSE, 0);
    view -> volume_win -> hbvol[i] = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> volume_win -> hbvol[i], check_button ("Show/Hide", 100, -1, view -> anim -> last -> img -> show_vol[i], G_CALLBACK(show_volumes), & view -> colorp[i][0]), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> volume_win -> hbvol[i], color_button (view -> anim -> last -> img -> vol_col[i], TRUE, 50, -1, G_CALLBACK(set_volume_color), & view -> colorp[i][0]), FALSE, FALSE, 5);
    widget_set_sensitive (view -> volume_win -> hbvol[i], view -> comp_vol[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, view -> volume_win -> hbvol[i], FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  }
  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void window_volumes (GtkWidget * widg, gpointer data)

  \brief create the 'Volumes' window callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void window_volumes (GtkWidget * widg, gpointer data)
{
  glwin * view = (glwin *) data;
  if (view -> volume_win == NULL)
  {
    view -> volume_win = g_malloc0 (sizeof*view -> volume_win);
    project * this_proj = get_project_by_id (view -> proj);
    gchar * str = g_strdup_printf ("%s - volumes", this_proj -> name);
    view -> volume_win -> win = create_win (str, view -> win, FALSE, FALSE);
    gtk_widget_set_size_request (view -> volume_win -> win, 450, 420);
    g_free (str);
    GtkWidget * vbox = create_vbox (BSEP);
    add_container_child (CONTAINER_WIN, view -> volume_win -> win, vbox);
    GtkWidget * notebook = gtk_notebook_new ();
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, notebook, TRUE, TRUE, 0);
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vol_model_tab (view), gtk_label_new ("Model"));
    if (view -> adv_bonding[0]) gtk_notebook_append_page (GTK_NOTEBOOK(notebook), frag_mol_volume_tab (view, 2), gtk_label_new ("Fragment(s)"));
    if (view -> adv_bonding[1]) gtk_notebook_append_page (GTK_NOTEBOOK(notebook), frag_mol_volume_tab (view, 3), gtk_label_new ("Molecule(s)"));
    add_gtk_close_event (view -> volume_win -> win, G_CALLBACK(hide_this_window), NULL);
    show_the_widgets (view -> volume_win -> win);
    int i;
    for (i=0; i<FILLED_STYLES; i++)
    {
      if (view -> comp_vol[i])
      {
        hide_the_widgets (view -> volume_win -> compb[i]);
      }
      else
      {
        hide_the_widgets (view -> volume_win -> hboxv[i]);
      }
    }
    if (view -> adv_bonding[0]) adjust_vol_md_step (this_proj, 2);
    if (view -> adv_bonding[1]) adjust_vol_md_step (this_proj, 3);
  }
  else
  {
    show_the_widgets (view -> volume_win -> win);
  }
}
