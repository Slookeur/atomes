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
* @file read_qm.c
* @short Functions to read ab-intio (CPMD/CP2K) calculation parameters in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_qm.c'
*
* Contains:
*

 - Functions to read ab-intio (CPMD/CP2K) calculation parameters in the atomes project file format

*
* List of functions:

  int read_thermo (FILE * fp, thermostat * thermo);
  int read_fixed_atoms_cpmd (FILE * fp, cpmd * cpmd_input);
  int read_fixed_atoms_cp2k (FILE * fp, cp2k * cp2k_input, int idf);
  int read_cpmd_data (FILE * fp, int cid, project * this_proj);
  int read_cp2k_data (FILE * fp, int cid, project * this_proj);

*/

#include "global.h"
#include "project.h"

/*!
  \fn int read_thermo (FILE * fp, thermostat * thermo)

  \brief read thermostat information from file

  \param fp the file pointer
  \param thermo the thermostat to store the data
*/
int read_thermo (FILE * fp, thermostat * thermo)
{
  if (fread (& thermo -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (& thermo -> type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (& thermo -> sys, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (& thermo -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (thermo -> params, sizeof(double), 4, fp) != 4) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (& thermo -> natoms, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  return OK;
}

/*!
  \fn int read_fixed_atoms_cpmd (FILE * fp, cpmd * cpmd_input)

  \brief read fixed CPMD atom(s) from file

  \param fp the file pointer
  \param cpmd_input the CPMD input structure to store the data
*/
int read_fixed_atoms_cpmd (FILE * fp, cpmd * cpmd_input)
{
  int i;
  if (fread (& cpmd_input -> fixat, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (cpmd_input -> fixat)
  {
    if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    if (i)
    {
      cpmd_input -> fixlist = allocint (cpmd_input -> fixat);
      if (fread (cpmd_input -> fixlist, sizeof(int), cpmd_input -> fixat, fp) != cpmd_input -> fixat) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
    if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    if (i)
    {
      cpmd_input -> fixcoord = allocdint (cpmd_input -> fixat, 3);
      for (i=0; i<cpmd_input -> fixat; i++) if (fread (cpmd_input -> fixcoord[i], sizeof(int), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
  }
  return OK;
}

/*!
  \fn int read_fixed_atoms_cp2k (FILE * fp, cp2k * cp2k_input, int idf)

  \brief read fixed CP2K from file

  \param fp the file pointer
  \param cp2k_input the CP2K input structure to store the data
  \param idf the fixed atom(s) format
*/
int read_fixed_atoms_cp2k (FILE * fp, cp2k * cp2k_input, int idf)
{
  int i;
  if (fread (& cp2k_input -> fixat[idf], sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (cp2k_input -> fixat[idf])
  {
    if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    if (i)
    {
      cp2k_input -> fixlist[idf] = allocint (cp2k_input -> fixat[idf]);
      if (fread (cp2k_input -> fixlist[idf], sizeof(int), cp2k_input -> fixat[idf], fp) != cp2k_input -> fixat[idf]) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
    if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    if (i)
    {
      cp2k_input -> fixcoord[idf] = allocdint (cp2k_input -> fixat[idf], 3);
      for (i=0; i<cp2k_input -> fixat[idf]; i++) if (fread (cp2k_input -> fixcoord[idf][i], sizeof(int), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
  }
  return OK;
}

/*!
  \fn int read_cpmd_data (FILE * fp, int cid, project * this_proj)

  \brief read CPMD data from file

  \param fp the file pointer
  \param cid CPMD id (0 = ab-initio, 1 = QM-MM)
  \param this_proj the target project
*/
int read_cpmd_data (FILE * fp, int cid, project * this_proj)
{
  int i;
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (! i) return OK;
  this_proj -> cpmd_input[cid] = g_malloc0(sizeof*this_proj -> cpmd_input[cid]);
  if (fread (& this_proj -> cpmd_input[cid] -> calc_type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (this_proj -> cpmd_input[cid] -> default_opts, sizeof(double), 17, fp) != 17) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (this_proj -> cpmd_input[cid] -> calc_opts, sizeof(double), 24, fp) != 24) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (& this_proj -> cpmd_input[cid] -> thermostats, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (this_proj -> cpmd_input[cid] -> thermostats)
  {
    this_proj -> cpmd_input[cid] -> ions_thermostat = g_malloc0(sizeof*this_proj -> cpmd_input[cid] -> ions_thermostat);
    thermostat * thermo = this_proj -> cpmd_input[cid] -> ions_thermostat;
    for (i=0; i<this_proj -> cpmd_input[cid] -> thermostats; i++)
    {
      if (read_thermo (fp, thermo) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_QM;
      }
      if (i < this_proj -> cpmd_input[cid] -> thermostats - 1)
      {
        thermo -> next = g_malloc0(sizeof*thermo -> next);
        thermo -> next -> prev = thermo;
        thermo = thermo -> next;
      }
    }
    if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    if (i)
    {
      this_proj -> cpmd_input[cid] -> elec_thermostat = g_malloc0(sizeof*this_proj -> cpmd_input[cid] -> elec_thermostat);
      if (read_thermo (fp, this_proj -> cpmd_input[cid] -> elec_thermostat) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_QM;
      }
    }
  }
  if (read_fixed_atoms_cpmd (fp, this_proj -> cpmd_input[cid]) != OK)
  {
    update_error_trace (__FILE__, __func__, __LINE__-2);
    return ERROR_QM;
  }
  if (fread (& this_proj -> cpmd_input[cid] -> dummies, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (this_proj -> cpmd_input[cid] -> dummies)
  {
    this_proj -> cpmd_input[cid] -> dummy = g_malloc0(sizeof*this_proj -> cpmd_input[cid] -> dummy);
    dummy_atom * dummy = this_proj -> cpmd_input[cid] -> dummy;
    for (i=0; i < this_proj -> cpmd_input[cid] -> dummies; i ++)
    {
      if (fread (& dummy -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fread (& dummy -> type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fread (& dummy -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fread (dummy -> xyz, sizeof(double), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fread (dummy -> coord, sizeof(int), 4, fp) != 4) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fread (& dummy -> natoms, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (dummy -> natoms)
      {
        dummy -> list = allocint (dummy -> natoms);
        if (fread (dummy -> list, sizeof(int), dummy -> natoms, fp) != dummy -> natoms) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      }
      if (i < this_proj -> cpmd_input[cid] -> dummies - 1)
      {
        dummy -> next = g_malloc0(sizeof*dummy -> next);
        dummy -> next -> prev = dummy;
        dummy = dummy -> next;
      }
    }
  }
  this_proj -> cpmd_input[cid] -> pp = allocdint (this_proj -> nspec, 2);
  for (i=0; i<this_proj -> nspec; i++)
  {
    if (fread (this_proj -> cpmd_input[cid] -> pp[i], sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  }
  this_proj -> cpmd_input[cid] -> info = read_this_string (fp);
  /*g_debug (" ********************* CPMD INFO *********************");
  g_debug ("\n%s\n", this_proj -> cpmd_input[cid] -> info);
  g_debug (" *****************************************************");*/
  if (this_proj -> cpmd_input[cid] -> info == NULL) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  return OK;
}

/*!
  \fn int read_cp2k_data (FILE * fp, int cid, project * this_proj)

  \brief read CP2K data from file

  \param fp the file pointer
  \param cid CP2K id (0 = ab-initio, 1 = QM-MM)
  \param this_proj the target project
*/
int read_cp2k_data (FILE * fp, int cid, project * this_proj)
{
  int i, j;
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (! i) return OK;
  this_proj -> cp2k_input[cid] = g_malloc0(sizeof*this_proj -> cp2k_input[cid]);
  if (fread (& this_proj -> cp2k_input[cid] -> input_type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fread (this_proj -> cp2k_input[cid] -> opts, sizeof(double), 42, fp) != 42) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  for (i=0; i<3; i++)
  {
    if (fread (this_proj -> cp2k_input[cid] -> extra_opts[i], sizeof(double), 4, fp) != 4) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  }
  if (fread (& this_proj -> cp2k_input[cid] -> thermostats, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (this_proj -> cp2k_input[cid] -> thermostats)
  {
    this_proj -> cp2k_input[cid] -> ions_thermostat = g_malloc0(sizeof*this_proj -> cp2k_input[cid] -> ions_thermostat);
    thermostat * thermo = this_proj -> cp2k_input[cid] -> ions_thermostat;
    for (i=0; i<this_proj -> cp2k_input[cid] -> thermostats; i++)
    {
      if (read_thermo (fp, thermo) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_QM;
      }
      if (i < this_proj -> cp2k_input[cid] -> thermostats - 1)
      {
        thermo -> next = g_malloc0(sizeof*thermo -> next);
        thermo -> next -> prev = thermo;
        thermo = thermo -> next;
      }
    }
  }
  for (i=0; i<2; i++)
  {
    if (read_fixed_atoms_cp2k (fp, this_proj -> cp2k_input[cid], i) != OK)
    {
      update_error_trace (__FILE__, __func__, __LINE__-2);
      return ERROR_QM;
    }
  }
  this_proj -> cp2k_input[cid] -> spec_data = allocdint (this_proj -> nspec, 2);
  this_proj -> cp2k_input[cid] -> spec_files = g_malloc0(this_proj -> nspec*sizeof*this_proj -> cp2k_input[cid] -> spec_files);
  for (i=0; i<this_proj -> nspec; i++)
  {
    if (fread (this_proj -> cp2k_input[cid] -> spec_data[i], sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    this_proj -> cp2k_input[cid] -> spec_files[i] = g_malloc0(2*sizeof*this_proj -> cp2k_input[cid] -> spec_files[i]);
    for (j=0; j<2; j++)
    {
      this_proj -> cp2k_input[cid] -> spec_files[i][j] = read_this_string (fp);
    }
  }
  for (i=0; i<5; i++)
  {
    this_proj -> cp2k_input[cid] -> files[i] = read_this_string (fp);
  }
  this_proj -> cp2k_input[cid] -> info = read_this_string (fp);
  if (this_proj -> cp2k_input[cid] -> info == NULL) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  return OK;
}
