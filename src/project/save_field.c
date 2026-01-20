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
* @file save_field.c
* @short Functions to save force field information in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'save_field.c'
*
* Contains:
*

 - The functions to save force field information in the atomes project file format

*
* List of functions:

  int save_field_atom (FILE * fp);
  int save_field_shell (FILE * fp);
  int save_field_constraint (FILE * fp);
  int save_field_pmf (FILE * fp);
  int save_field_rigid (FILE * fp);
  int save_field_tethered (FILE * fp, int fid);
  int save_field_prop (FILE * fp, int fid, int pid);
  int save_field_struct (FILE * fp, int fid);
  int save_field_molecule (FILE * fp, int fid);
  int save_field_body (FILE * fp, int fid);
  int save_field_external (FILE * fp, int fid);
  int save_dlp_field_data (FILE * fp, project * this_proj);
  int save_lmp_field_data (FILE * fp, project * this_proj);

*/

#include "global.h"
#include "project.h"
#include "dlp_field.h"

/*
typedef struct field field
struct field
{
  gboolean prepare_file[2];
  // Field and Config files
  gboolean afp[MAXDATC+MAXDATA];
  int type;
  int energy_unit;
  int atom_init;
  int molecules;
  field_molecule * first_molecule;
  int nbody[5];
  field_nth_body * first_body[5];
  // Tersoff potential cross terms
  double ** cross;
  int extern_fields;
  field_external * first_external;

  // Control file
  double sys_opts[17];
  double io_opts[23];
  double ana_opts[17];
  double elec_opts[11];
  double vdw_opts[6];
  double met_opts[2];
  double equi_opts[17];
  int ensemble;
  int thermostat;
  double thermo_opts[10];
  double md_opts[20];
  double out_opts[31];
};

*/

/*!
  \fn int save_field_atom (FILE * fp)

  \brief save field atom data to file

  \param fp the file pointer
*/
int save_field_atom (FILE * fp)
{
  if (fwrite (& tmp_fat -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fat -> fid, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fat -> afid, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fat -> type, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (save_this_string (fp, tmp_fat -> name) != OK) return ERROR_RW;
  if (fwrite (& tmp_fat -> num, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fat -> sp, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fat -> mass, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fat -> charge, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fat -> frozen, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (tmp_fat -> frozen_id, sizeof(gboolean), tmp_fat -> num, fp) != tmp_fat -> num) return ERROR_RW;
  if (fwrite (& tmp_fat -> show, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (tmp_fat -> list, sizeof(int), tmp_fat -> num, fp) != tmp_fat -> num) return ERROR_RW;
  if (fwrite (tmp_fat -> list_id, sizeof(int), tmp_fat -> num, fp) != tmp_fat -> num) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_field_shell (FILE * fp)

  \brief save field core shell data to file

  \param fp the file pointer
*/
int save_field_shell (FILE * fp)
{
  if (fwrite (& tmp_fshell -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (tmp_fshell -> ia, sizeof(int), 2, fp) != 2) return ERROR_RW;
  if (fwrite (& tmp_fshell -> m, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fshell -> z, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fshell -> k2, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fshell -> k4, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fshell -> vdw, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fshell -> show, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fshell -> use, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_field_constraint (FILE * fp)

  \brief save field constraint data to file

  \param fp the file pointer
*/
int save_field_constraint (FILE * fp)
{
  if (fwrite (& tmp_fcons -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (tmp_fcons -> ia, sizeof(int), 2, fp) != 2) return ERROR_RW;
  if (fwrite (& tmp_fcons -> av, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fcons -> length, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fcons -> show, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fcons -> use, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_field_pmf (FILE * fp)

  \brief save field mean force potential data to file

  \param fp the file pointer
*/
int save_field_pmf (FILE * fp)
{
  if (fwrite (& tmp_fpmf -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fpmf -> av, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fpmf -> length, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (tmp_fpmf -> num, sizeof(int), 2, fp) != 2) return ERROR_RW;
  int i;
  for (i=0; i<2; i++)
  {
    if (fwrite (tmp_fpmf -> list[i], sizeof(int), tmp_fpmf -> num[i], fp) != tmp_fpmf -> num[i]) return ERROR_RW;
    if (fwrite (tmp_fpmf -> weight[i], sizeof(float), tmp_fpmf -> num[i], fp) != tmp_fpmf -> num[i]) return ERROR_RW;
  }

  if (fwrite (& tmp_fpmf -> show, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fpmf -> use, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_field_rigid (FILE * fp)

  \brief save field rigid constraints data to file

  \param fp the file pointer
*/
int save_field_rigid (FILE * fp)
{
  if (fwrite (& tmp_frig -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_frig -> num, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (tmp_frig -> list, sizeof(int), tmp_frig -> num, fp) != tmp_frig -> num) return ERROR_RW;
  if (fwrite (& tmp_frig -> show, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_frig -> use, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_field_tethered (FILE * fp, int fid)

  \brief save field tethered data to file

  \param fp the file pointer
  \param fid
*/
int save_field_tethered (FILE * fp, int fid)
{
  if (fwrite (& tmp_ftet -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_ftet -> num, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_ftet -> key, sizeof(int), 1, fp) != 1) return ERROR_RW;
  int i = fvalues[fid][0][tmp_ftet -> key];
  if (fwrite (tmp_ftet -> val, sizeof(float), i, fp) != i) return ERROR_RW;
  if (fwrite (& tmp_ftet -> show, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_ftet -> use, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_field_prop (FILE * fp, int fid, int pid)

  \brief save field property data to file

  \param fp the file pointer
  \param fid the field id
  \param pid the property id
*/
int save_field_prop (FILE * fp, int fid, int pid)
{
  if (fwrite (& tmp_fprop -> pid, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fprop -> fpid, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fprop -> key, sizeof(int), 1, fp) != 1) return ERROR_RW;
  int i = struct_id(pid+7);
  if (fwrite (tmp_fprop -> aid, sizeof(int), i, fp) != i) return ERROR_RW;
  i = fvalues[fid][pid+1][tmp_fprop -> key];
  if (fwrite (tmp_fprop -> val, sizeof(float), i, fp) != i) return ERROR_RW;
  if (fwrite (& tmp_fprop -> show, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fprop -> use, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_field_struct (FILE * fp, int fid)

  \brief save field structural properties to file

  \param fp the file pointer
  \param fid the field id
*/
int save_field_struct (FILE * fp, int fid)
{
  if (fwrite (& tmp_fstr -> st, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fstr -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fstr -> num, sizeof(int), 1, fp) != 1) return ERROR_RW;
  int i = struct_id(tmp_fstr -> st + 7);
  if (fwrite (tmp_fstr -> aid, sizeof(int), i, fp) != i) return ERROR_RW;
  if (fwrite (& tmp_fstr-> av, sizeof(float), 1, fp) != 1) return ERROR_RW;
  tmp_fprop = tmp_fstr -> def;
  if (save_field_prop(fp, fid, tmp_fstr -> st) != OK) return ERROR_RW;
  tmp_fprop = tmp_fstr -> other;
  i = 0;
  while (tmp_fprop)
  {
    i ++;
    tmp_fprop = tmp_fprop -> next;
  }
  if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (tmp_fstr -> other)
  {
    tmp_fprop = tmp_fstr -> other;
    while (tmp_fprop)
    {
      if (save_field_prop(fp, fid, tmp_fstr -> st) != OK) return ERROR_RW;
      tmp_fprop = tmp_fprop -> next;
    }
  }
  return OK;
}

/*!
  \fn int save_field_molecule (FILE * fp, int fid)

  \brief save field molecule data to file

  \param fp the file pointer
  \param fid the field id
*/
int save_field_molecule (FILE * fp, int fid)
{
  if (fwrite (& tmp_fmol -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fmol -> mol -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (save_this_string (fp, tmp_fmol -> name) != OK) return ERROR_RW;
  if (fwrite (& tmp_fmol -> multi, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (tmp_fmol -> fragments, sizeof(int), tmp_fmol -> multi, fp) != tmp_fmol -> multi) return ERROR_RW;
  int i;
  for (i=0; i<tmp_fmol -> mol -> natoms; i++)
  {
    if (fwrite (tmp_fmol -> atoms_id[i], sizeof(dint), tmp_fmol -> multi, fp) != tmp_fmol -> multi) return ERROR_RW;
  }
  if (fwrite (& tmp_fmol -> atoms, sizeof(int), 1, fp) != 1) return ERROR_RW;
  tmp_fat = tmp_fmol -> first_atom;
  while (tmp_fat)
  {
    if (save_field_atom(fp) != OK) return ERROR_RW;
    tmp_fat = tmp_fat -> next;
  }

  if (fwrite (& tmp_fmol -> shells, sizeof(int), 1, fp) != 1) return ERROR_RW;
  tmp_fshell = tmp_fmol -> first_shell;
  while (tmp_fshell)
  {
    if (save_field_shell(fp) != OK) return ERROR_RW;
    tmp_fshell = tmp_fshell -> next;
  }

  if (fwrite (& tmp_fmol -> constraints, sizeof(int), 1, fp) != 1) return ERROR_RW;
  tmp_fcons = tmp_fmol -> first_constraint;
  while (tmp_fcons)
  {
    if (save_field_constraint(fp) != OK) return ERROR_RW;
    tmp_fcons = tmp_fcons -> next;
  }

  if (fwrite (& tmp_fmol -> pmfs, sizeof(int), 1, fp) != 1) return ERROR_RW;
  tmp_fpmf = tmp_fmol -> first_pmf;
  while (tmp_fpmf)
  {
    if (save_field_pmf(fp) != OK) return ERROR_RW;
    tmp_fpmf = tmp_fpmf -> next;
  }

  if (fwrite (& tmp_fmol -> rigids, sizeof(int), 1, fp) != 1) return ERROR_RW;
  tmp_frig = tmp_fmol -> first_rigid;
  while (tmp_frig)
  {
    if (save_field_rigid(fp) != OK) return ERROR_RW;
    tmp_frig = tmp_frig -> next;
  }

  if (fwrite (& tmp_fmol -> tethered, sizeof(int), 1, fp) != 1) return ERROR_RW;
  tmp_ftet = tmp_fmol -> first_tethered;
  while (tmp_ftet)
  {
    if (save_field_tethered(fp, fid) != OK) return ERROR_RW;
    tmp_ftet = tmp_ftet -> next;
  }
  if (fwrite (tmp_fmol -> nstruct, sizeof(int), 8, fp) != 8) return ERROR_RW;
  for (i=0; i<8; i++)
  {
    tmp_fstr = tmp_fmol -> first_struct[i];
    while (tmp_fstr)
    {
      if (save_field_struct (fp, fid) != OK) return ERROR_RW;
      tmp_fstr = tmp_fstr -> next;
    }
  }
  return OK;
}

/*!
  \fn int save_field_body (FILE * fp, int fid)

  \brief save field nth body data to file

  \param fp the file pointer
  \param fid the field id
*/
int save_field_body (FILE * fp, int fid)
{
  if (fwrite (& tmp_fbody -> bd, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fbody -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fbody -> key, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (tmp_fbody -> bd == 0) if (fwrite (tmp_fbody -> fpid, sizeof(int), 2, fp) != 2) return ERROR_RW;
  if (fwrite (tmp_fbody -> na, sizeof(int), body_at(tmp_fbody -> bd), fp) != body_at(tmp_fbody -> bd)) return ERROR_RW;
  int i;
  for (i=0; i<body_at(tmp_fbody -> bd); i++)
  {
    if (fwrite (tmp_fbody -> ma[i], sizeof(int), tmp_fbody -> na[i], fp) != tmp_fbody -> na[i]) return ERROR_RW;
    if (fwrite (tmp_fbody -> a[i], sizeof(int), tmp_fbody -> na[i], fp) != tmp_fbody -> na[i]) return ERROR_RW;
  }
  i = fvalues[fid][9+tmp_fbody -> bd][tmp_fbody -> key];
  if (fwrite (tmp_fbody -> val, sizeof(float), i, fp) != i) return ERROR_RW;
  if (fwrite (& tmp_fbody -> show, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fbody -> use, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_field_external (FILE * fp, int fid)

  \brief save field external data to file

  \param fp the file pointer
  \param fid the field id
*/
int save_field_external (FILE * fp, int fid)
{
  if (fwrite (& tmp_fext -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& tmp_fext -> key, sizeof(int), 1, fp) != 1) return ERROR_RW;
  int i = fvalues[fid][14][tmp_fext -> key];
  if (fwrite (tmp_fext -> val, sizeof(float), i, fp) != i) return ERROR_RW;
  if (fwrite (& tmp_fext -> use, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_dlp_field_data (FILE * fp, project * this_proj)

  \brief save force field data to file

  \param fp the file pointer
  \param this_proj the target project
*/
int save_dlp_field_data (FILE * fp, project * this_proj)
{
  int i, j;
  if (this_proj -> force_field[0] == NULL)
  {
    i = 0;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
    return OK;
  }
  i = 1;
  if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
  // CONTROL file
  if (fwrite (this_proj -> force_field[0] -> sys_opts, sizeof(double), 17, fp) != 17) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> io_opts, sizeof(double), 23, fp) != 23) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> ana_opts, sizeof(double), 17, fp) != 17) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> elec_opts, sizeof(double), 11, fp) != 11) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> vdw_opts, sizeof(double), 6, fp) != 6) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> met_opts, sizeof(double), 2, fp) != 2) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> equi_opts, sizeof(double), 17, fp) != 17) return ERROR_RW;
  if (fwrite (& this_proj -> force_field[0] -> ensemble, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> force_field[0] -> thermostat, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> thermo_opts, sizeof(double), 10, fp) != 10) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> md_opts, sizeof(double), 20, fp) != 20) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> out_opts, sizeof(double), 31, fp) != 31) return ERROR_RW;
  // FIELD file
  if (fwrite (this_proj -> force_field[0] -> prepare_file, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
  if (fwrite (this_proj -> force_field[0] -> afp, sizeof(gboolean), MAXDATC+MAXDATA, fp) != MAXDATC+MAXDATA) return ERROR_RW;
  if (fwrite (& this_proj -> force_field[0] -> type, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> force_field[0] -> energy_unit, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> force_field[0] -> atom_init, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> force_field[0] -> molecules, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (! this_proj -> force_field[0] -> molecules) return ERROR_RW;
  tmp_fmol = this_proj -> force_field[0] -> first_molecule;
  while (tmp_fmol)
  {
    if (save_field_molecule (fp, 0) != OK) return ERROR_RW;
    tmp_fmol = tmp_fmol -> next;
  }
  if (fwrite (this_proj -> force_field[0] -> nbody, sizeof(int), 5, fp) != 5) return ERROR_RW;
  for (i=0; i<5; i++)
  {
    if (this_proj -> force_field[0] -> nbody[i])
    {
      tmp_fbody =  this_proj -> force_field[0] -> first_body[i];
      while (tmp_fbody)
      {
        if (save_field_body (fp, 0) != OK) return ERROR_RW;
        tmp_fbody = tmp_fbody -> next;
      }
    }
  }
  // Tersoff potential cross terms
  i = (this_proj -> force_field[0] -> cross) ? 1 : 0;
  if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (this_proj -> force_field[0] -> cross)
  {

    i = this_proj -> force_field[0] -> nbody[2] * (this_proj -> force_field[0] -> nbody[2] - 1) / 2;
    for (j=0; j<i; j++)
    {
      if (fwrite (& this_proj -> force_field[0] -> cross[j], sizeof(double), 3, fp) != 3) return ERROR_RW;
    }
  }
  if (fwrite (& this_proj -> force_field[0] -> extern_fields, sizeof(int), 1, fp) != 1) return ERROR_RW;
  tmp_fext = this_proj -> force_field[0] -> first_external;
  while (tmp_fext)
  {
    if (save_field_external(fp, 0) != OK) return ERROR_RW;
    tmp_fext = tmp_fext -> next;
  }
  return OK;
}

/*!
  \fn int save_lmp_field_data (FILE * fp, project * this_proj)

  \brief save LAMMPS force field data to file

  \param fp the file pointer
  \param this_proj the target project
*/
int save_lmp_field_data (FILE * fp, project * this_proj)
{
  int i;
  if (this_proj -> force_field[1] == NULL)
  {
    i = 0;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
    return OK;
  }
  return OK;
}
