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
* @file save_qm.c
* @short Functions to save ab-initio (CPMD/CP2K) calculation parameters in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'save_qm.c'
*
* Contains:
*

 - The functions to save ab-initio (CPMD/CP2K) calculation parameters in the atomes project file format

*
* List of functions:

  int save_thermo (FILE * fp, thermostat * thermo);
  int save_fixed_atoms (FILE * fp, int fixatoms, int * fixlist, int ** fixcoord);
  int save_cpmd_data (FILE * fp, int cid, project * this_proj);
  int save_cp2k_data (FILE * fp, int cid, project * this_proj);

*/

#include "global.h"
#include "project.h"

/*!
  \fn int save_thermo (FILE * fp, thermostat * thermo)

  \brief save thermostat to file

  \param fp the file pointer
  \param thermo the thermostat to save
*/
int save_thermo (FILE * fp, thermostat * thermo)
{
  if (fwrite (& thermo -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (& thermo -> type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (& thermo -> sys, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (& thermo -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (thermo -> params, sizeof(double), 4, fp) != 4) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (& thermo -> natoms, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  return OK;
}

/*!
  \fn int save_fixed_atoms (FILE * fp, int fixatoms, int * fixlist, int ** fixcoord)

  \brief save fixed atom(s) to file

  \param fp the file pointer
  \param fixatoms the number of fixed atom(s)
  \param fixlist the list of fixed atom(s)
  \param fixcoord the list of fixed coordinate(s) for the fix atom(s)
*/
int save_fixed_atoms (FILE * fp, int fixatoms, int * fixlist, int ** fixcoord)
{
  int i;
  if (fwrite (& fixatoms, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fixatoms)
  {
    if (fixlist)
    {
      if (fwrite (fixlist, sizeof(int), fixatoms, fp) != fixatoms) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
    else
    {
      i = 0;
      if (fwrite (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
    if (fixcoord)
    {
      for (i=0; i<fixatoms; i++) if (fwrite (fixcoord[i], sizeof(int), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
    else
    {
      i = 0;
      if (fwrite (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
  }
  return OK;
}

/*!
  \fn int save_cpmd_data (FILE * fp, int cid, project * this_proj)

  \brief save CPMD data to file

  \param fp the file pointer
  \param cid the CPMD id (0 = ab-initio, 1 = QM-MM)
  \param this_proj the target project
*/
int save_cpmd_data (FILE * fp, int cid, project * this_proj)
{
  int i;
  if (this_proj -> cpmd_input[cid] == NULL)
  {
    i = 0;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    return OK;
  }
  i = 1;
  if (fwrite (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (& this_proj -> cpmd_input[cid] -> calc_type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (this_proj -> cpmd_input[cid] -> default_opts, sizeof(double), 17, fp) != 17) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (this_proj -> cpmd_input[cid] -> calc_opts, sizeof(double), 24, fp) != 24) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (& this_proj -> cpmd_input[cid] -> thermostats, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (this_proj -> cpmd_input[cid] -> thermostats)
  {
    thermostat * thermo = this_proj -> cpmd_input[cid] -> ions_thermostat;
    i = 0;
    while (thermo)
    {
      if (save_thermo (fp, thermo) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_QM;
      }
      i ++;
      thermo = thermo -> next;
    }
    i = (this_proj -> cpmd_input[cid] -> elec_thermostat) ? 1 : 0;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    if (this_proj -> cpmd_input[cid] -> elec_thermostat)
    {
      if (save_thermo (fp, this_proj -> cpmd_input[cid] -> elec_thermostat) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_QM;
      }
    }
  }
  if (save_fixed_atoms (fp, this_proj -> cpmd_input[cid] -> fixat, this_proj -> cpmd_input[cid] -> fixlist, this_proj -> cpmd_input[cid] -> fixcoord) != OK)
  {
    update_error_trace (__FILE__, __func__, __LINE__-2);
    return ERROR_QM;
  }
  if (fwrite (& this_proj -> cpmd_input[cid] -> dummies, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (this_proj -> cpmd_input[cid] -> dummies)
  {
    dummy_atom * dummy = this_proj -> cpmd_input[cid] -> dummy;
    while (dummy)
    {
      if (fwrite (& dummy -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fwrite (& dummy -> type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fwrite (& dummy -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fwrite (dummy -> xyz, sizeof(double), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fwrite (dummy -> coord, sizeof(int), 4, fp) != 4) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (fwrite (& dummy -> natoms, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      if (dummy -> natoms)
      {
        if (fwrite (dummy -> list, sizeof(int), dummy -> natoms, fp) != dummy -> natoms) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
      }
      dummy = dummy -> next;
    }
  }
  for (i=0; i<this_proj -> nspec; i++)
  {
    if (fwrite (this_proj -> cpmd_input[cid] -> pp[i], sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  }
  return save_this_string (fp, this_proj -> cpmd_input[cid] -> info);
}

/*!
  \fn int save_cp2k_data (FILE * fp, int cid, project * this_proj)

  \brief save CP2K data to file

  \param fp the file pointer
  \param cid the CP2K id (0 = ab-initio, 1 = QM-MM)
  \param this_proj the target project
*/
int save_cp2k_data (FILE * fp, int cid, project * this_proj)
{
  int i, j;
  if (this_proj -> cp2k_input[cid] == NULL)
  {
    i = 0;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    return OK;
  }
  i = 1;
  if (fwrite (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (& this_proj -> cp2k_input[cid] -> input_type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (fwrite (this_proj -> cp2k_input[cid] -> opts, sizeof(double), 42, fp) != 42) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  for (i=0; i<3; i++)
  {
    if (fwrite (this_proj -> cp2k_input[cid] -> extra_opts[i], sizeof(double), 4, fp) != 4) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  }
  if (fwrite (& this_proj -> cp2k_input[cid] -> thermostats, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  if (this_proj -> cp2k_input[cid] -> thermostats)
  {
    thermostat * thermo = this_proj -> cp2k_input[cid] -> ions_thermostat;
    while (thermo)
    {
      if (save_thermo (fp, thermo) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_QM;
      }
      thermo = thermo -> next;
    }
  }
  for (i=0; i<2; i++)
  {
    if (save_fixed_atoms (fp, this_proj -> cp2k_input[cid] -> fixat[i], this_proj -> cp2k_input[cid] -> fixlist[i], this_proj -> cp2k_input[cid] -> fixcoord[i]) != OK)
    {
      update_error_trace (__FILE__, __func__, __LINE__-2);
      return ERROR_QM;
    }
  }
  for (i=0; i<this_proj -> nspec; i++)
  {
    if (fwrite (this_proj -> cp2k_input[cid] -> spec_data[i], sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    for (j=0; j<2; j++)
    {
      if (save_this_string (fp, this_proj -> cp2k_input[cid] -> spec_files[i][j]) != OK) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
    }
  }
  for (i=0; i<5; i++)
  {
    if (save_this_string (fp, this_proj -> cp2k_input[cid] -> files[i]) != OK) return signal_error (__FILE__, __func__, __LINE__, ERROR_QM);
  }
  return save_this_string (fp, this_proj -> cp2k_input[cid] -> info);
}
