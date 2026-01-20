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
* @file cpmd_print.c
* @short Functions to print the CPMD input file(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cpmd_print.c'
*
* Contains:
*

 - The functions to print the CPMD input file(s)

*
* List of functions:

  void print_info_section (GtkTextBuffer * buf);
  void print_this_thermostat (thermostat * thermo, int id, GtkTextBuffer * buf);
  void print_thermostat (GtkTextBuffer * buf);
  void print_restart (GtkTextBuffer * buf);
  void print_cpmd_section (GtkTextBuffer * buf);
  void print_dft_section (GtkTextBuffer * buf);
  void print_vdw_section (GtkTextBuffer * buf);
  void print_prop_section (GtkTextBuffer * buf);
  void print_system_section (GtkTextBuffer * buf);
  void print_atoms_section (GtkTextBuffer * buf);
  void print_the_section (int s, int p, GtkTextBuffer * buffer);

*/

#include "global.h"
#include "interface.h"
#include "calc.h"
#include "cpmd.h"

extern dummy_atom * get_active_dummy (int id);

int cpmd_sym[NSYM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 14};

/*!
  \fn void print_info_section (GtkTextBuffer * buf)

  \brief print the CPMD input file INFO section

  \param buf the GtkTextBuffer to print into
*/
void print_info_section (GtkTextBuffer * buf)
{
  gchar * str = g_strdup_printf ("  This input was prepared using %s\n\n", PACKAGE);
  print_info (str, NULL, buf);
  g_free (str);
  if (tmp_cpmd -> info != NULL) print_info (tmp_cpmd -> info, NULL, buf);
  print_info ("\n\n", NULL, buf);
}

/*!
  \fn void print_this_thermostat (thermostat * thermo, int id, GtkTextBuffer * buf)

  \brief print CPMD thermostat parameters

  \param thermo the target thermostat
  \param id 0 = ionic, 1 = fictitious electronic
  \param buf the GtkTextBuffer to print into
*/
void print_this_thermostat (thermostat * thermo, int id, GtkTextBuffer * buf)
{
  int i, j, k, l, m;
  gchar * temp[2]={"TEMPERATURE", "TEMPERATURE ELECTRONS"};
  gchar * tobj[2]={"IONS", "ELECTRONS"};
  gchar * str = NULL;

  if (thermo -> type == 0)
  {
    str = g_strdup_printf ("\n\n  %s\n", temp[id]);
    print_info (str, NULL, buf);
    g_free (str);
    str = g_strdup_printf ("    %f", thermo -> params[0]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  else
  {
    if (thermo -> sys == GLOBAL)
    {
      str = g_strdup_printf ("\n\n  %s %s\n", termoke[thermo -> type-1], tobj[id]);
    }
    else if (thermo -> sys == LOCAL)
    {
      str = g_strdup_printf ("\n\n  %s %s %s\n", termoke[thermo -> type-1], tobj[id], nosekey[1]);
    }
    print_info (str, NULL, buf);
    g_free (str);
    if (thermo -> sys == LOCAL)
    {
      str = g_strdup_printf ("    %d\n", tmp_cpmd -> thermostats);
      print_info (str, NULL, buf);
      g_free (str);
      for (i=0; i<tmp_cpmd -> thermostats; i++)
      {
        for (j=0; j<2; j++)
        {
          str = g_strdup_printf ("     %11.5lf", thermo -> params[j]);
          print_info (str, NULL, buf);
          g_free (str);
        }
        print_info ("\n", NULL, buf);
        if (thermo-> next != NULL) thermo = thermo-> next;
      }
      str = g_strdup_printf ("    %d\n", tmp_cpmd -> thermostats);
      print_info (str, NULL, buf);
      g_free (str);
      thermo = tmp_cpmd -> ions_thermostat;
      for (i=0; i<tmp_cpmd -> thermostats; i++)
      {
        l = 0;
        str = g_strdup_printf ("    %d", i+1);
        print_info (str, NULL, buf);
        g_free (str);
        for (j=0; j < qm_proj-> nspec; j++)
        {
          for (k=0; k < qm_proj-> natomes; k++)
          {
            if (qm_proj -> atoms[0][k].sp == j)
            {
              l++;
              for (m=0; m < thermo -> natoms; m++)
              {
                if (thermo -> list[m] == k)
                {
                  str = g_strdup_printf (" %d", l);
                  print_info (str, NULL, buf);
                  g_free (str);
                }
              }
            }
          }
        }
        if (i < tmp_cpmd -> thermostats-1) print_info ("\n", NULL, buf);
        if (thermo-> next != NULL) thermo = thermo-> next;
      }
    }
    else
    {
      for (j=0; j<2; j++)
      {
        str = g_strdup_printf ("     %11.5lf", thermo -> params[j]);
        print_info (str, NULL, buf);
        g_free (str);
      }
    }
  }
}

/*!
  \fn void print_thermostat (GtkTextBuffer * buf)

  \brief print the CPMD input file THERMOSTAT section

  \param buf the GtkTextBuffer to print into
*/
void print_thermostat (GtkTextBuffer * buf)
{
  gchar * str;
  int id = (tmp_cpmd -> calc_type == 2) ? BAROC : BAROB;
  if ((int)tmp_cpmd -> calc_opts[id])
  {
    str = g_strdup_printf ("\n\n  %s", calc_box_keys[tmp_cpmd -> calc_type][(int)tmp_cpmd -> calc_opts[id]]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  id = (tmp_cpmd -> calc_type == 2) ? ANNIC : ANNIB;
  if ((int)tmp_cpmd -> calc_opts[id])
  {
    str = g_strdup_printf ("\n\n  ANNEALING IONS\n    %f", tmp_cpmd -> calc_opts[id+1]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  if (tmp_cpmd -> calc_type == 2)
  {
    if ((int)tmp_cpmd -> calc_opts[ANNEC])
    {
      str = g_strdup_printf ("\n\n  ANNEALING ELECTRONS\n    %f", tmp_cpmd -> calc_opts[AFAEC]);
      print_info (str, NULL, buf);
      g_free (str);
    }
  }
  print_this_thermostat (tmp_cpmd -> ions_thermostat, 0, buf);
  if (tmp_cpmd -> calc_type == 2)
  {
    print_this_thermostat (tmp_cpmd -> elec_thermostat, 1, buf);
  }
}

/*!
  \fn void print_restart (GtkTextBuffer * buf)

  \brief print the CPMD input file RESTART section

  \param buf the GtkTextBuffer to print into
*/
void print_restart (GtkTextBuffer * buf)
{
  gchar * str;
  if (tmp_cpmd -> restart[0] < 2)
  {
    str = g_strdup_printf ("\n\n  INITIALIZE WAVEFUNCTION %s", rest_kw[tmp_cpmd -> restart[0]]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  else
  {
    print_info ("\n\n  RESTART WAVEFUNCTION LATEST", NULL, buf);
    print_info ("\n  RESTART COORDINATES LATEST", NULL, buf);
    if (tmp_cpmd -> calc_type > 1 && tmp_cpmd -> calc_type < 4)
    {
      print_info ("\n  RESTART ACCUMULATORS LATEST", NULL, buf);
      print_info ("\n  RESTART VELOCITIES LATEST", NULL, buf);
      if (tmp_cpmd -> ions_thermostat -> type > 0) print_info ("\n  RESTART NOSEP LATEST", NULL, buf);
      if (tmp_cpmd -> calc_type == 2 && tmp_cpmd -> elec_thermostat -> type == 1) print_info ("\n  RESTART NOSEE LATEST", NULL, buf);
    }
  }
}

/*!
  \fn void print_cpmd_section (GtkTextBuffer * buf)

  \brief print CPMD input file CPMD section

  \param buf the GtkTextBuffer to print into
*/
void print_cpmd_section (GtkTextBuffer * buf)
{
  gchar * str = g_strdup_printf ("  %s", calc_kw[tmp_cpmd -> calc_type]);
  print_info (str, NULL, buf);
  g_free (str);
  int i;

  switch (tmp_cpmd -> calc_type)
  {
    case 0:
      str = g_strdup_printf ("\n\n  CONVERGENCE ORBITAL\n    %E\n\n", tmp_cpmd -> calc_opts[CONVO]);
      print_info (str, NULL, buf);
      g_free (str);
      str = g_strdup_printf ("  %s\n\n", calc_box_keys[0][(int)tmp_cpmd -> calc_opts[OPTIO]]);
      print_info (str, NULL, buf);
      g_free (str);
      str = g_strdup_printf ("  MAXITER\n    %d", (int)tmp_cpmd -> calc_opts[STEPO]);
      print_info (str, NULL, buf);
      g_free (str);
      break;
    case 1:
      str = g_strdup_printf ("\n\n  CONVERGENCE GEOMETRY\n    %E\n\n", tmp_cpmd -> calc_opts[CONVG]);
      print_info (str, NULL, buf);
      g_free (str);
      str = g_strdup_printf ("  %s\n\n", calc_box_keys[1][(int)tmp_cpmd -> calc_opts[OPTIG]]);
      print_info (str, NULL, buf);
      g_free (str);
      str = g_strdup_printf ("  MAXSTEP\n    %d", (int)tmp_cpmd -> calc_opts[STEPG]);
      print_info (str, NULL, buf);
      g_free (str);
      break;
    case 2:
      str = g_strdup_printf ("\n\n  MAXSTEP\n    %d", (int)tmp_cpmd -> calc_opts[STEPC]);
      print_info (str, NULL, buf);
      g_free (str);
      break;
    case 3:
      str = g_strdup_printf ("\n\n  MAXSTEP\n    %d", (int)tmp_cpmd -> calc_opts[STEPB]);
      print_info (str, NULL, buf);
      g_free (str);
      break;
    case 4:
      str = g_strdup_printf ("\n    %d", (int)tmp_cpmd -> calc_opts[KSUNO]);
      print_info (str, NULL, buf);
      g_free (str);
      if ((int)tmp_cpmd -> calc_opts[RHOUT])
      {
        str = g_strdup_printf ("\n  RHOOUT BANDS\n    %d\n    ", (int)tmp_cpmd -> calc_opts[NBAND]);
        print_info (str, NULL, buf);
        g_free (str);
      }
      break;
    case 5:
      str = g_strdup_printf (" %s", calc_box_keys[4][(int)tmp_cpmd -> calc_opts[VIBRA]]);
      print_info (str, NULL, buf);
      g_free (str);
      break;
    case 6:

      break;
  }

  print_restart (buf);

  if (tmp_cpmd -> calc_type < 4)
  {
    if (tmp_cpmd -> calc_type == 0)
    {
      i = TSTPO;
    }
    else if (tmp_cpmd -> calc_type == 1)
    {
      i = TSTPG;
    }
    else if (tmp_cpmd -> calc_type == 2)
    {
      i = TSTPC;
    }
    else
    {
      i = TSTPB;
    }
    str = g_strdup_printf ("\n\n  TIMESTEP\n    %f", tmp_cpmd -> calc_opts[i]);
    print_info (str, NULL, buf);
    g_free (str);
  }

  if (tmp_cpmd -> calc_type != 3)
  {
    str = g_strdup_printf ("\n\n  EMASS\n    %f", tmp_cpmd -> default_opts[DEFEM]);
    print_info (str, NULL, buf);
    g_free (str);
  }

  if (tmp_cpmd -> calc_type == 2 || tmp_cpmd -> calc_type == 3) print_thermostat (buf);

  if (tmp_cpmd -> default_opts[DEFLS] == 1.0)
  {
    print_info ("\n\n  LOCAL SPIN DENSITY", NULL, buf);
  }

  if (tmp_cpmd -> default_opts[DEFVD] == 1.0)
  {
    print_info ("\n\n  VDW CORRECTION", NULL, buf);
  }

  gchar * sv_key[2] = {"STORE", "RESTFILE"};
  for (i=0; i<2; i++)
  {
    str = g_strdup_printf ("\n\n  %s\n    %d", sv_key[i], tmp_cpmd -> restart[i+1]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  if (tmp_cpmd ->  calc_type != 0 && tmp_cpmd -> restart[3])
  {
    print_info ("\n\n  TRAJECTORY SAMPLE", NULL, buf);
    if (tmp_cpmd -> restart[4]) print_info (" XYZ", NULL, buf);
    if (tmp_cpmd -> restart[5]) print_info (" FORCES", NULL, buf);
    str = g_strdup_printf ("\n    %d", tmp_cpmd -> restart[3]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  if (tmp_cpmd -> restart[6])
  {
    print_info ("\n\n  PRINT", NULL, buf);
    if (tmp_cpmd -> restart[7]) print_info (" INFO", NULL, buf);
    //if (tmp_cpmd -> restart[8]) print_info (" EIGENVALUES", NULL, buf);
    if (tmp_cpmd -> restart[8]) print_info (" COORDINATES", NULL, buf);
    if (tmp_cpmd -> restart[9]) print_info (" FORCES", NULL, buf);
    str = g_strdup_printf ("\n    %d", tmp_cpmd -> restart[6]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  print_info ("\n\n", NULL, buf);
}

/*!
  \fn void print_dft_section (GtkTextBuffer * buf)

  \brief print the CPMD input file DFT section

  \param buf the GtkTextBuffer to print into
*/
void print_dft_section (GtkTextBuffer * buf)
{
  gchar * str = g_strdup_printf ("  FUNCTIONAL %s", default_keywords[0][(int)tmp_cpmd -> default_opts[DEFDF]]);
  print_info (str, NULL, buf);
  g_free (str);
  str = g_strdup_printf ("\n\n  GC-CUTOFF\n    %E\n\n", tmp_cpmd -> default_opts[DEFGC]);
  print_info (str, NULL, buf);
  g_free (str);
}

/*!
  \fn void print_vdw_section (GtkTextBuffer * buf)

  \brief print the CPMD input file VDW section

  \param buf the GtkTextBuffer to print into
*/
void print_vdw_section (GtkTextBuffer * buf)
{
  gchar * str = g_strdup_printf ("  EMPIRICAL CORRECTION\n"
                                 "    VDW PARAMETERS\n"
                                 "    ALL DFT-D2\n"
                                 "    S6GRIMME\n      %s\n"
                                 "  END EMPIRICAL CORRECTION\n\n",
                                 default_keywords[0][(int)tmp_cpmd -> default_opts[DEFDF]]);
  print_info (str, NULL, buf);
  g_free (str);
}

/*!
  \fn void print_prop_section (GtkTextBuffer * buf)

  \brief print the CPMD input file PROPERTIES section

  \param buf the GtkTextBuffer to print into
*/
void print_prop_section (GtkTextBuffer * buf)
{

}

/*!
  \fn void print_system_section (GtkTextBuffer * buf)

  \brief print the CPMD input file SYSTEM section

  \param buf the GtkTextBuffer to print into
*/
void print_system_section (GtkTextBuffer * buf)
{
  double u, v;
  gchar * str;
  u = 0.52917721;
  if ((int)tmp_cpmd -> default_opts[DEFAN])
  {
    u = 1.0;
    print_info ("  ANGSTROM\n\n", NULL, buf);
  }

  if (! (int)tmp_cpmd -> default_opts[DEFVE])
  {
    str = g_strdup_printf ("  SYMMETRY\n    %d\n\n", cpmd_sym[(int)tmp_cpmd -> default_opts[DEFSY]]);
    print_info (str, NULL, buf);
    g_free (str);
  }

  print_info ("  CELL", NULL, buf);
  if ((int)tmp_cpmd -> default_opts[DEFVE])
  {
    print_info (" VECTORS", NULL, buf);
    if (((int)tmp_cpmd -> default_opts[DEFAB]))
    {
      print_info (" ABSOLUTE", NULL, buf);
    }
  }
  else
  {
    if (((int)tmp_cpmd -> default_opts[DEFAB]))
    {
      print_info (" ABSOLUTE", NULL, buf);
    }
    if (((int)tmp_cpmd -> default_opts[DEFDG]))
    {
      print_info (" DEGREE", NULL, buf);
    }
  }

  int i, j;
  if ((int)tmp_cpmd -> default_opts[DEFVE])
  {
    for (i=0; i<3; i++)
    {
      print_info ("\n   ", NULL, buf);
      for (j=0; j<3; j++)
      {
        v = qm_proj -> cell.box[0].vect[i][j] / u;
        str = g_strdup_printf (" %12.6lf", v);
        print_info (str, NULL, buf);
        g_free (str);
      }
    }
  }
  else
  {
    print_info ("\n   ", NULL, buf);
    for (i=0; i<2; i++)
    {
      for (j=0; j<3; j++)
      {
        if (i == 0)
        {
          if ((int)tmp_cpmd -> default_opts[DEFAB])
          {
            v = qm_proj -> cell.box[0].param[i][j] / u;
          }
          else
          {
            if (j==0)
            {
              v = qm_proj -> cell.box[0].param[i][j] / u;
            }
            else if (qm_proj -> cell.box[0].param[i][0] != 0.0)
            {
              v = qm_proj -> cell.box[0].param[i][j] / qm_proj -> cell.box[0].param[i][0];
            }
            else
            {
              v = -1.0;
            }
          }
        }
        else
        {
          if ((int)tmp_cpmd -> default_opts[DEFDG])
          {
            v = qm_proj -> cell.box[0].param[i][j];
          }
          else
          {
            v = cos (qm_proj -> cell.box[0].param[i][j]*pi/180.0);
          }
        }
        str = g_strdup_printf (" %12.6lf", v);
        print_info (str, NULL, buf);
        g_free (str);
      }
    }
  }

  print_info ("\n\n", NULL, buf);
  str = g_strdup_printf ("  CUTOFF\n    %15.10lf\n\n", tmp_cpmd -> default_opts[DEFCU]);
  print_info (str, NULL, buf);
  g_free (str);
}

/*!
  \fn void print_atoms_section (GtkTextBuffer * buf)

  \brief print the CPMD input file ATOMS section

  \param buf the GtkTextBuffer to print into
*/
void print_atoms_section (GtkTextBuffer * buf)
{
  int i, j, k, l, m;
  gchar * str;
  double u = ((int)tmp_cpmd -> default_opts[DEFAN]) ? 1.0 : 0.52917721;

  if ((int)tmp_cpmd -> default_opts[DEFCO] &&
      ((int)tmp_cpmd -> default_opts[DEFFI] == 0 || ((int)tmp_cpmd -> default_opts[DEFFI] > 0 && tmp_cpmd -> fixat > 0)))
  {
    str = g_strdup_printf ("  CONSTRAINTS\n    FIX %s\n", default_keywords[5][(int)tmp_cpmd -> default_opts[DEFFI]]);
    print_info (str, NULL, buf);
    g_free (str);
    if ((int)tmp_cpmd -> default_opts[DEFFI] > 0)
    {
      str = g_strdup_printf ("      %d\n     ", tmp_cpmd -> fixat);
      print_info (str, NULL, buf);
      g_free (str);
      k = m = 0;
      for (i=0; i<qm_proj -> nspec; i++)
      {
        for (j=0; j<qm_proj -> natomes; j++)
        {
          if (qm_proj -> atoms[0][j].sp == i)
          {
            k ++;
            for (l=0; l<tmp_cpmd->fixat; l++)
            {
              if (qm_proj -> atoms[0][j].id == tmp_cpmd -> fixlist[l])
              {
                m ++;
                str = g_strdup_printf (" %d", k);
                print_info (str, NULL, buf);
                g_free (str);
                if ((int)tmp_cpmd -> default_opts[DEFFI] == 2)
                {
                  str = g_strdup_printf ("   %d   %d   %d\n",
                                         tmp_cpmd -> fixcoord[l][0],
                                         tmp_cpmd -> fixcoord[l][1],
                                         tmp_cpmd -> fixcoord[l][2]);
                  print_info (str, NULL, buf);
                  g_free (str);
                  if (m < tmp_cpmd->fixat)
                  {
                    print_info ("     ", NULL, buf);
                  }
                }
              }
            }
          }
        }
      }
      if ((int)tmp_cpmd -> default_opts[DEFFI] != 2) print_info ("\n", NULL, buf);
    }
    print_info ("  END CONSTRAINTS\n\n", NULL, buf);
  }

  if ((int)tmp_cpmd -> default_opts[DEFDU] && tmp_cpmd -> dummies > 0)
  {
    gchar * dtype[3]={"    TYPE1", "    TYPE2", "    TYPE3"};
    str = g_strdup_printf ("  DUMMY ATOMS\n    %d\n", tmp_cpmd -> dummies);
    print_info (str, NULL, buf);
    g_free (str);
    dummy_atom * dumm;
    for (i=0; i<tmp_cpmd -> dummies; i++)
    {
      dumm = get_active_dummy (i);
      print_info (dtype[dumm -> type], NULL, buf);
      if (dumm -> type == 0)
      {
        str = g_strdup_printf (" %12.6lf  %12.6lf  %12.6lf\n", dumm -> xyz[0], dumm -> xyz[1], dumm -> xyz[2]);
      }
      else if (dumm -> natoms > 0)
      {
        if ( dumm -> natoms < qm_proj -> natomes)
        {
          l = 0;
          str = g_strdup_printf (" %d", dumm -> natoms);
          for (j=0; j<qm_proj -> nspec; j++)
          {
            for (k=0; k<qm_proj -> natomes; k++)
            {
              if (qm_proj -> atoms[0][k].sp == j)
              {
                l ++;
                for (m=0; m<dumm -> natoms; m++)
                {
                  if (qm_proj -> atoms[0][k].id == dumm -> list[m])
                  {
                    str = g_strdup_printf ("%s %d", str, l);
                  }
                }
              }
            }
          }
        }
        else if (dumm -> natoms == qm_proj -> natomes)
        {
          str = g_strdup_printf (" %d", -1);
        }
        str = g_strdup_printf ("%s\n", str);
      }
      else
      {
        str = g_strdup_printf ("\n");
      }
      print_info (str, NULL, buf);
      g_free (str);
    }
    print_info ("\n", NULL, buf);
  }

  for (i=0; i<qm_proj -> nspec; i++)
  {
    str = g_strdup_printf ("*%s-%s.psp\n", exact_name(qm_proj -> chemistry -> label[i]), default_keywords[0][(int)tmp_cpmd -> default_opts[DEFDF]]);
    print_info (str, NULL, buf);
    g_free (str);
    str = g_strdup_printf ("    LMAX=%s LOC=%s\n    %d\n", default_keywords[7][(int)tmp_cpmd -> pp[i][0]],
                                                           default_keywords[8][(int)tmp_cpmd -> pp[i][1]], qm_proj -> chemistry -> nsps[i]);
    print_info (str, NULL, buf);
    g_free (str);
    for (j=0; j<qm_proj -> natomes; j++)
    {
      if (qm_proj -> atoms[0][j].sp == i)
      {
        str = g_strdup_printf ("      %12.6lf  %12.6lf  %12.6lf\n",
                               qm_proj -> atoms[0][j].x/u, qm_proj -> atoms[0][j].y/u, qm_proj -> atoms[0][j].z/u);
        print_info (str, NULL, buf);
        g_free (str);
      }
    }
    print_info ("\n", NULL, buf);
  }
}

/*!
  \fn void print_the_section (int s, int p, GtkTextBuffer * buffer)

  \brief print CPMD input section

  \param s the section id
  \param p 0 = section preview, 1 = complete input file
  \param buffer the GtkTextBuffer to print into
*/
void print_the_section (int s, int p, GtkTextBuffer * buffer)
{
  int i;
  gchar * str;
  if (buffer != NULL)
  {
    if (s > 0 && s < 4)
    {
      i = 1;
    }
    else if (s == 0)
    {
      i = 0;
    }
    else
    {
      i = s - 2;
    }
    if (! p)
    {
      GtkTextIter bStart;
      GtkTextIter bEnd;
      gtk_text_buffer_get_start_iter (buffer, & bStart);
      gtk_text_buffer_get_end_iter (buffer, & bEnd);
      gtk_text_buffer_delete (buffer, & bStart, & bEnd);
    }
    if (p && s > 0) print_info ("\n\n", NULL, buffer);
    str  = g_strdup_printf ("&%s\n\n", cpmd_elements[i]);
    print_info (str, "bold", buffer);
    g_free (str);
    if (s == 0)
    {
      print_info_section (buffer);
    }
    else if (s < 4)
    {
      print_cpmd_section (buffer);
    }
    else
    {
      switch (s)
      {
        case 4:
          print_dft_section (buffer);
          break;
        case 5:
          print_vdw_section (buffer);
          break;
        case 6:
          print_prop_section (buffer);
          break;
        case 7:
          print_system_section (buffer);
          break;
        case 8:
          print_atoms_section (buffer);
          break;
      }
    }
    print_info ("&END", "bold", buffer);
  }
}
