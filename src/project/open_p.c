/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2026 by CNRS and University of Strasbourg */

/*!
* @file open_p.c
* @short Functions to start the reading of an atomes project file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'open_p.c'
*
* Contains:
*

 - The functions to start the reading of an atomes project file

*
* List of functions:

  int read_analysis (FILE * fp, project * this_proj, atomes_analysis * this_analysis, int wid);
  int open_project (FILE * fp, int wid);

  char * read_string (int i, FILE * fp);

  gchar * read_this_string (FILE * fp);

  void initcnames (project * this_proj, int rid);
  void allocatoms (project * this_proj);
  void alloc_proj_data (project * this_proj, int cid);

  chemical_data * alloc_chem_data (int spec);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"
#include "glview.h"
#include "movie.h"
#include "preferences.h"

extern void init_box_calc ();
extern void set_color_map_sensitive (glwin * view);
extern void init_gr (project * this_proj, int rdf);
extern void init_sq (project * this_proj, int sqk);
extern void init_bond (project * this_proj);
extern void init_ang (project * this_proj);
extern void init_ring (project * this_proj);
extern void init_chain (project * this_proj);
extern void init_msd (project * this_proj);
extern void init_sph (project * this_proj, int opening);
extern void init_skt (project * this_proj, int opening);
extern void alloc_analysis_curves (int pid, atomes_analysis * this_analysis);
extern void add_curve_widgets (project * this_proj, int rid);

extern G_MODULE_EXPORT void run_render_image (GtkDialog * info, gint response_id, gpointer data);

gboolean version_2_5_and_bellow;
gboolean version_2_6_and_above;
gboolean version_2_7_and_above;
gboolean version_2_8_and_above;
gboolean version_2_9_and_above;

/*!
  \fn char * read_string (int i, FILE * fp)

  \brief read a string from a file

  \param i the size of the string to read
  \param fp the file pointer
*/
char * read_string (int i, FILE * fp)
{
  char * tmp = NULL;
  tmp = g_malloc0(i*sizeof*tmp);
  int j;
  for (j=0; j<i; j++)
  {
    tmp[j] = fgetc(fp);
  }
  //return fgets (tmp, i+1, fp);
  return tmp;
}

/*!
  \fn gchar * read_this_string (FILE * fp)

  \brief is there a string to read in this file ? yes do it

  \param fp the file pointer
*/
gchar * read_this_string (FILE * fp)
{
  int i;
  if (fread (& i, sizeof(int), 1, fp) != 1) return NULL;
  if (i > 0)
  {
    gchar * str = g_strdup_printf ("%s", read_string (i, fp));
    return str;
  }
  return NULL;
}


/*!
  \fn void initcnames (project * this_proj, int rid)

  \brief initialize curve names

  \param project the target project
  \param rid calculation id
*/
void initcnames (project * this_proj, int rid)
{
  switch (rid)
  {
    case GDR:
      init_gr (this_proj, rid);
      break;
    case SQD:
      init_sq (this_proj, rid);
      break;
    case SKD:
      init_sq (this_proj, rid);
      break;
    case GDK:
      init_gr (this_proj, rid);
      break;
    case BND:
      init_bond (this_proj);
      break;
    case ANG:
      init_ang (this_proj);
      break;
    case RIN:
      init_ring (this_proj);
      break;
    case CHA:
      init_chain (this_proj);
      break;
    case SPH:
      init_sph (this_proj, 1);
      break;
    case MSD:
      init_msd (this_proj);
      break;
    case SKT:
      init_skt (this_proj, 1);
      break;
  }
}

/*!
  \fn void allocatoms (project * this_proj)

  \brief allocate project data

  \param this_proj the target project
*/
void allocatoms (project * this_proj)
{
  int i, j;
  if (this_proj -> atoms != NULL)
  {
    g_free (this_proj -> atoms);
    this_proj -> atoms = NULL;
  }
  this_proj -> atoms = g_malloc0(this_proj -> steps*sizeof*this_proj -> atoms);
  for (i=0; i < this_proj -> steps; i++)
  {
    this_proj -> atoms[i] = g_malloc0(this_proj -> natomes*sizeof*this_proj -> atoms[i]);
    for (j=0; j<this_proj -> natomes; j++)
    {
      this_proj -> atoms[i][j].style = NONE;
    }
  }
}

/*!
  \fn chemical_data * alloc_chem_data (int spec)

  \brief allocate chemistry data

  \param spec the number of chemical species
*/
chemical_data * alloc_chem_data (int spec)
{
  chemical_data * chem = g_malloc0(sizeof*chem);
  chem -> label = g_malloc0(spec*sizeof*chem -> label);
  chem -> element = g_malloc0(spec*sizeof*chem -> element);
  chem -> nsps = allocint (spec);
  chem -> formula = allocint (spec);
  chem -> grtotcutoff = default_totcut;
  chem -> cutoffs = allocddouble (spec, spec);
  chem -> chem_prop = allocddouble (CHEM_PARAMS, spec);
  return chem;
}

/*!
  \fn void alloc_proj_data (project * this_proj, int cid)

  \brief allocate data

  \param this_proj the target project
  \param cid Allocate chemistry data (1/0)
*/
void alloc_proj_data (project * this_proj, int cid)
{
  if (cid) this_proj -> chemistry = alloc_chem_data (this_proj -> nspec);
  allocatoms (this_proj);
}

/*!
  \fn int read_analysis (FILE * fp, project * this_proj, atomes_analysis * this_analysis, int wid)

  \brief saving analysis parameter(s) and result(s) to project file

  \param fp the file pointer
  \param this_proj the target project
  \param this_analysis the target analysis
  \param wid reading workspace (1/0)
*/
int read_analysis (FILE * fp, project * this_proj, atomes_analysis * this_analysis, int wid)
{
  int i, j;
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (i != this_analysis -> aid)
  {
    // This is not supposed to happen
    return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  }
  this_analysis -> name = read_this_string (fp);
  if (! this_analysis -> name) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> avail_ok, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> init_ok, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> calc_ok, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> requires_md, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> num_delta, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> delta, sizeof(double), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> min, sizeof(double), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> max, sizeof(double), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> fact, sizeof(double), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (fread (& this_analysis -> graph_res, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
  if (this_analysis -> graph_res)
  {
    if (fread (& this_analysis -> numc, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
    if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
    if (i != this_analysis -> c_sets)
    {
      // This is not supposed to happen
      return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
    }
    if (fread (this_analysis -> compat_id, sizeof(int), this_analysis -> c_sets, fp) != this_analysis -> c_sets) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
    if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
    if (i)
    {
      this_analysis -> x_title = read_this_string (fp);
      if (! this_analysis -> x_title) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
    }
    if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_ANA);
    if (i)
    {
      initcnames (this_proj, this_analysis -> aid);
      for (j=0; j<i; j++)
      {
        // g_debug ("Reading :: analysis= %s, aid= %d, j= %d", this_analysis -> name, this_analysis -> aid, j);
        if (read_project_curve (fp, wid, this_proj -> id) != OK)
        {
          update_error_trace (__FILE__, __func__, __LINE__-2);
          // g_debug ("Error :: analysis= %s, aid= %d, j= %d", this_analysis -> name, this_analysis -> aid, j);
          return ERROR_CURVE;
        }
        if (this_analysis -> aid == SPH)
        {
          init_curve_title (this_proj, SPH, j);
        }
      }
    }
  }
  return OK;
}

/*!
  \fn int open_project (FILE * fp, int wid)

  \brief open atomes project file

  \param fp the file pointer
  \param wid reading workspace (1/0)
*/
int open_project (FILE * fp, int wid)
{
  int i, j, k;
  gchar * version;

  version = read_this_string (fp);
  if (! version) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);

  version_2_6_and_above = FALSE;
  version_2_7_and_above = FALSE;
  version_2_8_and_above = FALSE;
  version_2_9_and_above = FALSE;

  int calcs_to_read;
  // Start version related tests
  if (g_strcmp0(version, "%\n% project file v-2.6\n%\n") == 0)
  {
    version_2_6_and_above = TRUE;
  }
  else if (g_strcmp0(version, "%\n% project file v-2.7\n%\n") == 0)
  {
    version_2_6_and_above = TRUE;
    version_2_7_and_above = TRUE;
  }
  else if (g_strcmp0(version, "%\n% project file v-2.8\n%\n") == 0)
  {
    version_2_6_and_above = TRUE;
    version_2_7_and_above = TRUE;
    version_2_8_and_above = TRUE;
  }
  else if (g_strcmp0(version, "%\n% project file v-2.9\n%\n") == 0)
  {
    version_2_6_and_above = TRUE;
    version_2_7_and_above = TRUE;
    version_2_8_and_above = TRUE;
    version_2_9_and_above = TRUE;
  }
  // End version related tests

  // Ensure file compatibility with STEP_LIMIT for atomes version <1.3.0
  reading_step_limit = (version_2_9_and_above) ? STEP_LIMIT : STEP_LIMIT*10;
  // Temporary buffers for version compatibility < 2.9
  gboolean * tmp_avail, * tmp_init, * tmp_calc;
  int * tmp_num_delta;
  double * tmp_delta;
  double * tmp_min;
  double * tmp_max;

 #ifdef DEBUG
  g_debug ("%s", version);
 #endif // DEBUG
  g_free (version);

  // After that we read the data
  active_project -> name = read_this_string (fp);
  if (active_project -> name == NULL) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
#ifdef DEBUG
  g_debug ("OPEN_PROJECT: Project name= %s",active_project -> name);
#endif
  if (fread (& active_project -> tfile, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (active_project -> tfile > -1)
  {
    active_project -> coordfile = read_this_string (fp);
    if (active_project -> coordfile == NULL) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (i > -1)
  {
    active_project -> bondfile = read_this_string (fp);
    if (active_project -> bondfile == NULL) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  if (version_2_9_and_above)
  {
    if (fread (& calcs_to_read, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  else
  {
    // NCALCS was first set to 10
    calcs_to_read = 10;
    // We need temporary buffers to read this data
    tmp_avail = allocbool (calcs_to_read);
    tmp_init = allocbool (calcs_to_read);
    tmp_calc = allocbool (calcs_to_read);
    if (fread (tmp_avail, sizeof(gboolean), calcs_to_read, fp) != calcs_to_read) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    if (fread (tmp_init, sizeof(gboolean), calcs_to_read, fp) != calcs_to_read) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    if (fread (tmp_calc, sizeof(gboolean), calcs_to_read, fp) != calcs_to_read) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  if (fread (& active_project -> nspec, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (& active_project -> natomes, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (& active_project -> steps, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (& active_cell -> pbc, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (& active_cell -> frac, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (& active_cell -> ltype,  sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (& i,  sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (i > 1 && i != active_project -> steps) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (i > 1) active_cell -> npt = TRUE;
  active_cell -> box = g_malloc0(i*sizeof*active_cell -> box);
  active_box = & active_cell -> box[0];
  for (j=0; j<i; j++)
  {
    for (k=0; k<3; k++)
    {
      if (fread (active_cell -> box[j].vect[k],  sizeof(double), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    }
    if (fread (active_cell -> box[j].param[0],  sizeof(double), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    if (fread (active_cell -> box[j].param[1],  sizeof(double), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  if (fread (& active_cell -> crystal, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (active_cell -> crystal && i)
  {
    active_cell -> sp_group = g_malloc0(sizeof*active_cell -> sp_group);
    active_cell -> sp_group -> id = i;
    active_cell -> sp_group -> bravais = read_this_string (fp);
    if (! active_cell -> sp_group -> bravais) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    active_cell -> sp_group -> hms = read_this_string (fp);
    if (! active_cell -> sp_group -> hms) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    active_cell -> sp_group -> setting = read_this_string (fp);
    if (! active_cell -> sp_group -> setting) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  if (fread (& active_project -> run, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (& active_project -> initgl, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (active_project -> tmp_pixels, sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (atomes_render_image  && ! render_image_pixels) render_image_pixels = duplicate_int (2, active_project -> tmp_pixels);
  if (! version_2_9_and_above)
  {
    // Temporary buffers again
    tmp_num_delta = allocint (calcs_to_read);
    tmp_delta = allocdouble (calcs_to_read);
    if (fread (tmp_num_delta, sizeof(int), calcs_to_read, fp) != calcs_to_read) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    if (fread (tmp_delta, sizeof(double), calcs_to_read, fp) != calcs_to_read) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  else
  {
    // Analysis data for k-points sampling
    for (i=0; i<2; i++)
    {
      if (fread (active_project -> sk_advanced[i], sizeof(double), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    }
  }
  if (fread (active_project -> rsearch, sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  for (i=0; i<5; i++)
  {
    if (fread (active_project -> rsparam[i], sizeof(int), 6, fp) != 6) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    if (fread (active_project -> rsdata[i], sizeof(double), 5, fp) != 5) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  if (fread (& active_project -> csearch, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (active_project -> csparam, sizeof(int), 7, fp) != 7) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (fread (active_project -> csdata, sizeof(double), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (! version_2_9_and_above)
  {
    // Temporary buffers again
    tmp_min = allocdouble (calcs_to_read);
    tmp_max = allocdouble (calcs_to_read);
    if (fread (tmp_min, sizeof(double), calcs_to_read, fp) != calcs_to_read) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    if (fread (tmp_max, sizeof(double), calcs_to_read, fp) != calcs_to_read) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  }
  if (fread (& active_project -> tunit, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
  if (version_2_9_and_above)
  {
    if (active_project -> steps)
    {
      if (fread (& active_project -> skt_corr_threshold, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      if (fread (& active_project -> skt_all_sets, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      if (! active_project -> skt_all_sets)
      {
        if (fread (& active_project -> skt_n_data_sets, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
        if (active_project -> skt_n_data_sets)
        {
          active_project -> skt_step_id = allocint (active_project -> skt_n_data_sets);
          if (fread (active_project -> skt_step_id, sizeof(int), active_project -> skt_n_data_sets, fp) != active_project -> skt_n_data_sets) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
        }
      }
      if (fread (& active_project -> sqw_n_data_sets, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      if (active_project -> sqw_n_data_sets)
      {
        active_project -> sqw_q_id = allocdouble (active_project -> sqw_n_data_sets);
        if (fread (active_project -> sqw_q_id, sizeof(double), active_project -> sqw_n_data_sets, fp) != active_project -> sqw_n_data_sets) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      }
      if (fread (& active_project -> sqw_freq, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    }
  }
  if (active_project -> natomes != 0 && active_project -> nspec != 0)
  {
    alloc_proj_data (active_project, 1);
    active_chem = active_project -> chemistry;
    if (version_2_6_and_above)
    {
      for (i=0; i<active_project -> nspec; i++)
      {
        active_chem -> label[i] = read_this_string (fp);
        if (! active_chem -> label[i]) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
        active_chem -> element[i] = read_this_string (fp);
        if (! active_chem -> element[i]) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      }
    }
    if (fread (active_chem -> nsps, sizeof(int), active_project -> nspec, fp) != active_project -> nspec) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    if (fread (active_chem -> formula, sizeof(int), active_project -> nspec, fp) != active_project -> nspec) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    j = 0;
    for (i=0; i<active_project -> nspec; i++) j+= active_chem -> nsps[i];
    if (j != active_project -> natomes) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    for (i=0; i<CHEM_PARAMS; i++)
    {
      if (fread (active_chem -> chem_prop[i], sizeof(double), active_project -> nspec, fp) != active_project -> nspec) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    }
    if (! version_2_7_and_above)
    {
      for (i=0; i<active_project -> nspec; i++)
      {
        active_chem -> chem_prop[CHEM_X][i] = active_chem -> chem_prop[CHEM_Z][i];
      }
    }
    if (fread (& active_chem -> grtotcutoff, sizeof(double), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    for ( i = 0 ; i < active_project -> nspec ; i ++ )
    {
      if (fread (active_chem -> cutoffs[i], sizeof(double), active_project -> nspec, fp) != active_project -> nspec) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
    }
    for (i=0; i<active_project -> steps; i++)
    {
      for (j=0; j< active_project -> natomes; j++)
      {
        if (read_atom_a (fp, active_project, i, j) != OK)
        {
          update_error_trace (__FILE__, __func__, __LINE__-2);
          return ERROR_ATOM_A;
        }
      }
    }
    init_box_calc ();
    if (active_project -> run)
    {
#ifdef DEBUG
      g_debug ("OPEN_PROJECT:: So far so good ... still");
      g_debug ("OPEN_PROJECT:: RUN PROJECT\n");
#endif
      i = alloc_data_ (& active_project -> natomes,
                       & active_project -> nspec,
                       & active_project -> steps);
      if (i == 1)
      {
        if (! version_2_6_and_above)
        {
          j = 1;
          prep_spec_ (active_chem -> chem_prop[CHEM_Z], active_chem -> nsps, & j);
        }
        // Read curves
        init_atomes_analysis (active_project, FALSE);
        if (version_2_9_and_above)
        {
          for (i=0; i<calcs_to_read; i++)
          {
            if (active_project -> analysis[i])
            {
              if (read_analysis (fp, active_project, active_project -> analysis[i], wid) != OK)
              {
                update_error_trace (__FILE__, __func__, __LINE__-2);
                return ERROR_ANA;
              }
            }
          }
        }
        else
        {
          for (i=0; i<calcs_to_read; i++)
          {
            if (active_project -> analysis[i])
            {
              active_project -> analysis[i] -> avail_ok = tmp_avail[i];
              active_project -> analysis[i] -> init_ok = tmp_init[i];
              active_project -> analysis[i] -> calc_ok = tmp_calc[i];
              active_project -> analysis[i] -> delta = tmp_delta[i];
              active_project -> analysis[i] -> num_delta = tmp_num_delta[i];
              active_project -> analysis[i] -> min = tmp_min[i];
              active_project -> analysis[i] -> max = tmp_max[i];
              if (active_project -> analysis[i] -> init_ok)
              {
                initcnames (active_project, i);
              }
            }
          }
          g_free (tmp_avail);
          g_free (tmp_init);
          g_free (tmp_calc);
          g_free (tmp_delta);
          g_free (tmp_num_delta);
          g_free (tmp_min);
          g_free (tmp_max);
          if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
#ifdef DEBUG
          g_debug ("\n**********************************************\n curves to read= %d\n**********************************************\n", i);
#endif
          if (i != 0)
          {
            j = 0;
            if (fread (& j, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
            if (j)
            {
              active_project -> analysis[SPH] -> numc = j;
              alloc_analysis_curves (active_project -> id, active_project -> analysis[SPH]);
              add_curve_widgets (active_project, SPH);
              active_project -> analysis[SPH] -> avail_ok = TRUE;
              for (k=0; k<j; k++)
              {
                active_project -> analysis[SPH] -> curves[k] -> name = read_this_string (fp);
                if (active_project -> analysis[SPH] -> curves[k] -> name == NULL) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
              }
            }
            for (j=0; j<i; j++)
            {
              if (read_project_curve (fp, wid, activep) != OK)
              {
                update_error_trace (__FILE__, __func__, __LINE__-2);
                return ERROR_CURVE;
              }
            }
          }
        }
        if (! atomes_render_image) fill_tool_model();
      }
      else
      {
        return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      }
    }
  }
  else
  {
    // error
    return ERROR_NO_WAY;
  }
#ifdef DEBUG
  // debugioproj (active_project, "READ INIT");
#endif
  if (update_project() != 1)
  {
    update_error_trace (__FILE__, __func__, __LINE__-3);
    return ERROR_UPDATE;
  }
  else
  {
    if (active_project -> initgl)
    {
      gboolean tmp_bonding;
      if (fread (& tmp_bonding, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      if (fread (tmp_adv_bonding, sizeof(gboolean), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      apply_project (TRUE);
      if (! atomes_render_image) fill_tool_model ();
      int tmpcoord[10];
      if (fread (tmpcoord, sizeof(int), 10, fp) != 10) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
      if (active_glwin -> bonding)
      {
        for (i=0; i<2; i++) if (tmpcoord[i] != active_project -> coord -> totcoord[i]) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
        for (i=2; i<4; i++)
        {
          if (active_glwin -> adv_bonding[i-2])
          {
            if (tmpcoord[i] != active_project -> coord -> totcoord[i]) return signal_error (__FILE__, __func__, __LINE__, ERROR_PROJECT);
          }
        }
      }
      for (i=0; i<10; i++) active_project -> coord -> totcoord[i] = tmpcoord[i];
      // Read molecule info
      if ((active_project -> natomes > ATOM_LIMIT || active_project -> steps > reading_step_limit) && tmp_adv_bonding[1])
      {
        if (read_mol (fp) != OK)
        {
          update_error_trace (__FILE__, __func__, __LINE__-2);
          return ERROR_MOL;
        }
      }
      for (i=0; i<2; i++) active_glwin -> adv_bonding[i] = tmp_adv_bonding[i];
      active_glwin -> bonding = tmp_bonding;

      if (read_bonding (fp) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_COORD;
      }

      if (read_opengl_image (fp, active_project, active_glwin -> anim -> last -> img, active_project -> nspec) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_IMAGE;
      }

      if (read_dlp_field_data (fp, active_project) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }
      if (read_lmp_field_data (fp, active_project) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }

      for (i=0; i<2; i++)
      {
        if (read_cpmd_data (fp, i, active_project) != OK)
        {
          update_error_trace (__FILE__, __func__, __LINE__-2);
          return ERROR_QM;
        }
      }
      for (i=0; i<2; i++)
      {
        if (read_cp2k_data (fp, i, active_project) != OK)
        {
          update_error_trace (__FILE__, __func__, __LINE__-2);
          return ERROR_QM;
        }
      }
#ifdef GTK3
      // GTK3 Menu Action To Check
      set_color_map_sensitive (active_glwin);
#endif
      return OK;
    }
    return OK;
  }
}
