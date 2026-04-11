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
* @file read_field.c
* @short Functions to read DLPOLY force field data in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_field.c'
*
* Contains:
*

 - The functions to read DLPOLY force field data in the atomes project file format

*
* List of functions:

  int read_field_atom (FILE * fp);
  int read_field_shell (FILE * fp);
  int read_field_constraint (FILE * fp);
  int read_field_pmf (FILE * fp);
  int read_field_rigid (FILE * fp);
  int read_field_tethered (FILE * fp, int fid);
  int read_field_prop (FILE * fp, int fid, int pid);
  int read_field_struct (FILE * fp, int fid);
  int read_field_molecule (FILE * fp, int fid);
  int read_field_body (FILE * fp, int fid);
  int read_field_external (FILE * fp, int fid);
  int read_dlp_field_data (FILE * fp, project * this_proj);
  int read_lmp_field_data (FILE * fp, project * this_proj);

*/

#include "global.h"
#include "project.h"
#include "dlp_field.h"

/*!
  \fn int read_field_atom (FILE * fp)

  \brief read field atom properties from file

  \param fp the file pointer
*/
int read_field_atom (FILE * fp)
{
  if (fread (& tmp_fat -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> fid, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> afid, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fat -> name = read_this_string (fp);
  if (tmp_fat -> name == NULL) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> num, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> sp, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> mass, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> charge, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> frozen, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fat -> frozen_id = allocbool(tmp_fat -> num);
  if (fread (tmp_fat -> frozen_id, sizeof(gboolean), tmp_fat -> num, fp) != tmp_fat -> num) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fat -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fat -> list = allocint (tmp_fat -> num);
  if (fread (tmp_fat -> list, sizeof(int), tmp_fat -> num, fp) != tmp_fat -> num) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fat -> list_id = allocint (tmp_fat -> num);
  if (fread (tmp_fat -> list_id, sizeof(int), tmp_fat -> num, fp) != tmp_fat -> num) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_field_shell (FILE * fp)

  \brief read field core shell data from file

  \param fp the file pointer
*/
int read_field_shell (FILE * fp)
{
  if (fread (& tmp_fshell -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (tmp_fshell -> ia, sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fshell -> m, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fshell -> z, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fshell -> k2, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fshell -> k4, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fshell -> vdw, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fshell -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fshell -> use, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_field_constraint (FILE * fp)

  \brief read field constraint data from file

  \param fp the file pointer
*/
int read_field_constraint (FILE * fp)
{
  if (fread (& tmp_fcons -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (tmp_fcons -> ia, sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fcons -> av, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fcons -> length, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fcons -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fcons -> use, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_field_pmf (FILE * fp)

  \brief read field mean force potential data from file

  \param fp the file pointer
*/
int read_field_pmf (FILE * fp)
{
  if (fread (& tmp_fpmf -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fpmf -> av, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fpmf -> length, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (tmp_fpmf -> num, sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  int i;
  for (i=0; i<2; i++)
  {
    tmp_fpmf -> list[i] = allocint (tmp_fpmf -> num[i]);
    if (fread (tmp_fpmf -> list[i], sizeof(int), tmp_fpmf -> num[i], fp) != tmp_fpmf -> num[i]) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
    tmp_fpmf -> weight[i] = allocfloat (tmp_fpmf -> num[i]);
    if (fread (tmp_fpmf -> weight[i], sizeof(float), tmp_fpmf -> num[i], fp) != tmp_fpmf -> num[i]) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  }
  if (fread (& tmp_fpmf -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fpmf -> use, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_field_rigid (FILE * fp)

  \brief read field rigid constraints data from file

  \param fp the file pointer
*/
int read_field_rigid (FILE * fp)
{
  if (fread (& tmp_frig -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_frig -> num, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_frig -> list = allocint (tmp_frig -> num);
  if (fread (tmp_frig -> list, sizeof(int), tmp_frig -> num, fp) != tmp_frig -> num) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_frig -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_frig -> use, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_field_tethered (FILE * fp, int fid)

  \brief read field tethered data from file

  \param fp the file pointer
  \param fid the field id
*/
int read_field_tethered (FILE * fp, int fid)
{
  if (fread (& tmp_ftet -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_ftet -> num, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_ftet -> key, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  int i = fvalues[fid][0][tmp_ftet -> key];
  tmp_ftet -> val = allocfloat (i);
  if (fread (tmp_ftet -> val, sizeof(float), i, fp) != i) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_ftet -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_ftet -> use, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_field_prop (FILE * fp, int fid, int pid)

  \brief read field property from file

  \param fp the file pointer
  \param fid the field id
  \param pid the property id
*/
int read_field_prop (FILE * fp, int fid, int pid)
{
  if (fread (& tmp_fprop -> pid, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fprop -> fpid, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fprop -> key, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  int i = struct_id(pid+7);
  tmp_fprop -> aid = allocint (i);
  if (fread (tmp_fprop -> aid, sizeof(int), i, fp) != i) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  i = fvalues[fid][pid+1][tmp_fprop -> key];
  tmp_fprop -> val = allocfloat (i);
  if (fread (tmp_fprop -> val, sizeof(float), i, fp) != i) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fprop -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fprop -> use, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_field_struct (FILE * fp, int fid)

  \brief read field structural properties from file

  \param fp the file pointer
  \param fid the field id
*/
int read_field_struct (FILE * fp, int fid)
{
  if (fread (& tmp_fstr -> st, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fstr -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fstr -> num, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  int i, j;
  i = struct_id(tmp_fstr -> st + 7);
  tmp_fstr -> aid = allocint (i);
  if (fread (tmp_fstr -> aid, sizeof(int), i, fp) != i) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fstr-> av, sizeof(float), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fstr -> def = g_malloc0(sizeof*tmp_fstr -> def);
  tmp_fprop = tmp_fstr -> def;
  if (read_field_prop(fp, fid, tmp_fstr -> st) != OK)
  {
    update_error_trace (__FILE__, __func__, __LINE__-2);
    return ERROR_FIELD;
  }
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (i)
  {
    tmp_fstr -> other = g_malloc0(sizeof*tmp_fstr -> other);
    tmp_fprop = tmp_fstr -> other;
    for (j=0; j<i; j++)
    {
      if (read_field_prop(fp, fid, tmp_fstr -> st) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }
      if (j < i-1)
      {
        tmp_fprop -> next = g_malloc0(sizeof*tmp_fprop -> next);
        tmp_fprop = tmp_fprop -> next;
      }
    }
  }
  return OK;
}

/*!
  \fn int read_field_molecule (FILE * fp, int fid)

  \brief read field molecule from file

  \param fp the file pointer
  \param fid the field id
*/
int read_field_molecule (FILE * fp, int fid)
{
  if (fread (& tmp_fmol -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  //tmp_fmol -> mol = g_malloc0(sizeof*tmp_fmol -> mol);
  int i, j;
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  // if (i == 20) i = 1;
  tmp_fmol -> mol = & active_project -> modelfc -> mols[0][i];
  tmp_fmol -> name = read_this_string (fp);
  if (tmp_fmol -> name == NULL) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fmol -> multi, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fmol -> fragments = allocint (tmp_fmol -> multi);
  if (fread (tmp_fmol -> fragments, sizeof(int), tmp_fmol -> multi, fp) != tmp_fmol -> multi) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fmol -> atoms_id = g_malloc0(tmp_fmol -> mol -> natoms*sizeof*tmp_fmol -> atoms_id);

  for (i=0; i<tmp_fmol -> mol -> natoms; i++)
  {
    tmp_fmol -> atoms_id[i] = g_malloc0(tmp_fmol -> multi*sizeof*tmp_fmol -> atoms_id[i]);
    if (fread (tmp_fmol -> atoms_id[i], sizeof(dint), tmp_fmol -> multi, fp) != tmp_fmol -> multi) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  }

  if (fread (& tmp_fmol -> atoms, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fmol -> first_atom = g_malloc0(sizeof*tmp_fmol -> first_atom);
  tmp_fat = tmp_fmol -> first_atom;
  for (i=0; i<tmp_fmol -> atoms; i++)
  {
    if (read_field_atom(fp) != OK)
    {
      update_error_trace (__FILE__, __func__, __LINE__-2);
      return ERROR_FIELD;
    }
    if (i < tmp_fmol -> atoms-1)
    {
      tmp_fat -> next = g_malloc0(sizeof*tmp_fat -> next);
      tmp_fat = tmp_fat -> next;
    }
  }

  if (fread (& tmp_fmol -> shells, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (tmp_fmol -> shells)
  {
    tmp_fmol -> first_shell = g_malloc0(sizeof*tmp_fmol -> first_shell);
    tmp_fshell = tmp_fmol -> first_shell;
    for (i=0; i<tmp_fmol -> shells; i++)
    {
      if (read_field_shell(fp) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }
      if (i < tmp_fmol -> shells - 1)
      {
        tmp_fshell -> next = g_malloc0(sizeof*tmp_fshell -> next);
        tmp_fshell = tmp_fshell -> next;
      }
    }
  }

  if (fread (& tmp_fmol -> constraints, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (tmp_fmol -> constraints)
  {
    tmp_fmol -> first_constraint = g_malloc0(sizeof*tmp_fmol -> first_constraint);
    tmp_fcons = tmp_fmol -> first_constraint;
    for (i=0; i<tmp_fmol -> constraints; i++)
    {
      if (read_field_constraint(fp) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }
      if (i < tmp_fmol -> constraints - 1)
      {
        tmp_fcons -> next = g_malloc0(sizeof*tmp_fcons -> next);
        tmp_fcons = tmp_fcons -> next;
      }
    }
  }

  if (fread (& tmp_fmol -> pmfs, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (tmp_fmol -> pmfs)
  {
    tmp_fmol -> first_pmf = g_malloc0(sizeof*tmp_fmol -> first_pmf);
    tmp_fpmf = tmp_fmol -> first_pmf;
    for (i=0; i<tmp_fmol -> pmfs; i++)
    {
      if (read_field_pmf(fp) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }
      if (i < tmp_fmol -> pmfs - 1)
      {
        tmp_fpmf -> next = g_malloc0(sizeof*tmp_fpmf -> next);
        tmp_fpmf = tmp_fpmf -> next;
      }
    }
  }

  if (fread (& tmp_fmol -> rigids, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (tmp_fmol -> rigids)
  {
    tmp_fmol -> first_rigid = g_malloc0(sizeof*tmp_fmol -> first_rigid);
    tmp_frig = tmp_fmol -> first_rigid;
    for (i=0; i<tmp_fmol -> rigids; i++)
    {
      if (read_field_rigid(fp) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }
      if (i < tmp_fmol -> rigids - 1)
      {
        tmp_frig -> next = g_malloc0(sizeof*tmp_frig -> next);
        tmp_frig = tmp_frig -> next;
      }
    }
  }

  if (fread (& tmp_fmol -> tethered, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (tmp_fmol -> tethered)
  {
    tmp_fmol -> first_tethered = g_malloc0(sizeof*tmp_fmol -> first_tethered);
    tmp_ftet = tmp_fmol -> first_tethered;
    for (i=0; i<tmp_fmol -> tethered; i++)
    {
      if (read_field_tethered(fp, fid) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }
      if (i < tmp_fmol -> tethered - 1)
      {
        tmp_ftet -> next = g_malloc0(sizeof*tmp_ftet -> next);
        tmp_ftet = tmp_ftet -> next;
      }
    }
  }

  if (fread (tmp_fmol -> nstruct, sizeof(int), 8, fp) != 8) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  for (i=0; i<8; i++)
  {
    if (tmp_fmol -> nstruct[i])
    {
      tmp_fmol -> first_struct[i] = g_malloc0(sizeof*tmp_fmol -> first_struct[i]);
      tmp_fstr = tmp_fmol -> first_struct[i];
      for (j=0; j<tmp_fmol -> nstruct[i]; j++)
      {
        if (read_field_struct (fp, fid)!= OK)
        {
          update_error_trace (__FILE__, __func__, __LINE__-2);
          return ERROR_FIELD;
        }
        if (j < tmp_fmol -> nstruct[i]-1)
        {
          tmp_fstr -> next = g_malloc0(sizeof*tmp_fstr -> next);
          tmp_fstr = tmp_fstr -> next;
        }
      }
    }
  }
  return OK;
}

/*!
  \fn int read_field_body (FILE * fp, int fid)

  \brief read field nth body data from file

  \param fp the file pointer
  \param fid the field id
*/
int read_field_body (FILE * fp, int fid)
{
  if (fread (& tmp_fbody -> bd, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fbody -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fbody -> key, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (tmp_fbody -> bd == 0)
  {
    tmp_fbody -> fpid = allocint (2);
    if (fread (tmp_fbody -> fpid, sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  }
  int i, j;
  i = body_at(tmp_fbody -> bd);
  tmp_fbody -> na = allocint (i);
  if (fread (tmp_fbody -> na, sizeof(int), i, fp) != i) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  tmp_fbody -> ma = g_malloc0(i*sizeof*tmp_fbody -> ma);
  tmp_fbody -> a = g_malloc0(i*sizeof*tmp_fbody -> a);
  for (j=0; j<i; j++)
  {
    tmp_fbody -> ma[j] = allocint (tmp_fbody -> na[j]);
    if (fread (tmp_fbody -> ma[j], sizeof(int), tmp_fbody -> na[j], fp) != tmp_fbody -> na[j]) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
    tmp_fbody -> a[j] = allocint (tmp_fbody -> na[j]);
    if (fread (tmp_fbody -> a[j], sizeof(int), tmp_fbody -> na[j], fp) != tmp_fbody -> na[j]) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  }
  i = fvalues[fid][9+tmp_fbody -> bd][tmp_fbody -> key];
  tmp_fbody -> val = allocfloat (i);
  if (fread (tmp_fbody -> val, sizeof(float), i, fp) != i) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fbody -> show, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fbody -> use, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_field_external (FILE * fp, int fid)

  \brief read field external data from file

  \param fp the file pointer
  \param fid the field id
*/
int read_field_external (FILE * fp, int fid)
{
  if (fread (& tmp_fext -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fext -> key, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  int i = fvalues[fid][14][tmp_fext -> key];
  tmp_fext -> val = allocfloat (i);
  if (fread (tmp_fext -> val, sizeof(float), i, fp) != i) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& tmp_fext -> use, sizeof(gboolean), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  return OK;
}

/*!
  \fn int read_dlp_field_data (FILE * fp, project * this_proj)

  \brief read force field data from file

  \param fp the file pointer
  \param this_proj the target project
*/
int read_dlp_field_data (FILE * fp, project * this_proj)
{
  int i, j, k, l, m, n;
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (! i) return OK;
  this_proj -> force_field[0] = g_malloc0(sizeof*this_proj -> force_field[0]);
  // Allocate force field data
  this_proj -> force_field[0] -> sys_opts = allocdouble (17);
  this_proj -> force_field[0] -> io_opts = allocdouble (23);
  this_proj -> force_field[0] -> ana_opts = allocdouble (17);
  this_proj -> force_field[0] -> elec_opts = allocdouble (11);
  this_proj -> force_field[0] -> vdw_opts = allocdouble (6);
  this_proj -> force_field[0] -> met_opts = allocdouble (2);
  this_proj -> force_field[0] -> equi_opts = allocdouble (17);
  this_proj -> force_field[0] -> thermo_opts= allocdouble (10);
  this_proj -> force_field[0] -> md_opts = allocdouble (20);
  this_proj -> force_field[0] -> out_opts = allocdouble (31);
  // CONTROL file
  if (fread (this_proj -> force_field[0] -> sys_opts, sizeof(double), 17, fp) != 17) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> io_opts, sizeof(double), 23, fp) != 23) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> ana_opts, sizeof(double), 17, fp) != 17) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> elec_opts, sizeof(double), 11, fp) != 11) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> vdw_opts, sizeof(double), 6, fp) != 6) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> met_opts, sizeof(double), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> equi_opts, sizeof(double), 17, fp) != 17) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& this_proj -> force_field[0] -> ensemble, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& this_proj -> force_field[0] -> thermostat, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> thermo_opts, sizeof(double), 10, fp) != 10) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> md_opts, sizeof(double), 20, fp) != 20) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> out_opts, sizeof(double), 31, fp) != 31) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  // FIELD file
  if (fread (this_proj -> force_field[0] -> prepare_file, sizeof(gboolean), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (this_proj -> force_field[0] -> afp, sizeof(gboolean), MAXDATC+MAXDATA, fp) != MAXDATC+MAXDATA) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& this_proj -> force_field[0] -> type, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& this_proj -> force_field[0] -> energy_unit, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& this_proj -> force_field[0] -> atom_init, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (fread (& this_proj -> force_field[0] -> molecules, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (! this_proj -> force_field[0] -> molecules) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  this_proj -> force_field[0] -> first_molecule = g_malloc0(sizeof*this_proj -> force_field[0] -> first_molecule);
  tmp_fmol = this_proj -> force_field[0] -> first_molecule;
  for (i=0; i<this_proj -> force_field[0] -> molecules; i++)
  {
    if (read_field_molecule (fp, 0) != OK)
    {
      update_error_trace (__FILE__, __func__, __LINE__-2);
      return ERROR_FIELD;
    }
    // setup_field_molecule_neighbors (i, this_proj);
    for (j=0; j<tmp_fmol -> mol -> natoms; j++)
    {
      for (k=0; k<tmp_fmol -> multi; k++)
      {
        l = tmp_fmol -> atoms_id[j][k].a;
        m = tmp_fmol -> atoms_id[j][k].b;
        tmp_fat = tmp_fmol -> first_atom;
        while (tmp_fat -> id < l) tmp_fat = tmp_fat -> next;
        n = tmp_fat -> list[m];
        this_proj -> atoms[0][n].fid = j;
        this_proj -> atoms[0][n].faid = l;
      }
    }
    if (i < this_proj -> force_field[0] -> molecules-1)
    {
      tmp_fmol -> next = g_malloc0(sizeof*tmp_fmol -> next);
      tmp_fmol = tmp_fmol -> next;
    }
  }
  if (fread (this_proj -> force_field[0] -> nbody, sizeof(int), 5, fp) != 5) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  for (i=0; i<5; i++)
  {
    if (this_proj -> force_field[0] -> nbody[i])
    {
      this_proj -> force_field[0] -> first_body[i] = g_malloc0(sizeof*this_proj -> force_field[0] -> first_body[i]);
      tmp_fbody = this_proj -> force_field[0] -> first_body[i];
      for (j=0; j<this_proj -> force_field[0] -> nbody[i]; j++)
      {
        if (read_field_body (fp, 0) != OK)
        {
          update_error_trace (__FILE__, __func__, __LINE__-2);
          return ERROR_FIELD;
        }
        if (j < this_proj -> force_field[0] -> nbody[i]-1)
        {
          tmp_fbody -> next = g_malloc0(sizeof*tmp_fbody -> next);
          tmp_fbody = tmp_fbody -> next;
        }
      }
    }
  }
  // Tersoff potential cross terms
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (i)
  {
    j = this_proj -> force_field[0] -> nbody[2] * (this_proj -> force_field[0] -> nbody[2] - 1) / 2;
    this_proj -> force_field[0] -> cross = g_malloc0(j*sizeof*this_proj -> force_field[0] -> cross);
    for (k=0; k<j; k++)
    {
      this_proj -> force_field[0] -> cross[k] = g_malloc0(3*sizeof*this_proj -> force_field[0] -> cross[k]);
      if (fread (& this_proj -> force_field[0] -> cross[k], sizeof(double), 3, fp) != 3) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
    }
  }
  if (fread (& this_proj -> force_field[0] -> extern_fields, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (this_proj -> force_field[0] -> extern_fields)
  {
    this_proj -> force_field[0] -> first_external = g_malloc0(sizeof*this_proj -> force_field[0] -> first_external);
    tmp_fext = this_proj -> force_field[0] -> first_external;
    for (i=0; i<this_proj -> force_field[0] -> extern_fields; i++)
    {
      if (read_field_external(fp, 0) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_FIELD;
      }
      if (i < this_proj -> force_field[0] -> extern_fields-1)
      {
        tmp_fext -> next = g_malloc0(sizeof*tmp_fext -> next);
        tmp_fext = tmp_fext -> next;
      }
    }
  }
  return OK;
}

/*!
  \fn int read_lmp_field_data (FILE * fp, project * this_proj)

  \brief read LAMMPS field data from file

  \param fp the file pointer
  \param this_proj the target project
*/
int read_lmp_field_data (FILE * fp, project * this_proj)
{
  int i;
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_FIELD);
  if (! i) return OK;
  return OK;
}
