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
* @file cp2k_print.c
* @short Functions to print the CP2K input file(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cp2k_print.c'
*
* Contains:
*

 - The functions to print the CP2K input file(s)

*
* List of functions:

  gchar * cp2kbool (double opt);

  void print_cp2k_var (gchar * var, GtkTextBuffer * buffer);
  void print_var_section (int num, gchar ** section, GtkTextBuffer * buffer);
  void print_thermostat_cp2k (int n_thermo, GtkTextBuffer * buffer);
  void print_motion_cp2k (int m, GtkTextBuffer * buffer);
  void print_coord_cp2k (GtkTextBuffer * buffer);
  void print_subsys_cp2k (GtkTextBuffer * buffer);
  void print_variables_cp2k (GtkTextBuffer * buffer);
  void print_global_cp2k (GtkTextBuffer * buffer);
  void print_cp2k_print (gchar * spaces, gchar * info, int i, int j, GtkTextBuffer * buffer);
  void print_cp2k (int f, GtkTextBuffer * buffer);

*/

#include "global.h"
#include "interface.h"
#include "calc.h"
#include "cp2k.h"

gchar * globopts[49] = {"@SET SYSTEM                         ", //  0 |    - char
                        "@SET RUN                            ", //  1 |  0 - char(int)
                        "@SET CPU_TIME                       ", //  2 |  1 - int
                        "@SET PRINT                          ", //  3 |  2 - char(int)
                        "@SET USE_RESTART                    ", //  4 |  3 - bool(int)
                        "@SET RESTART_FILE                   ", //  5 |  4 - char
                        "@SET BASIS_FILE                     ", //  6 |  5 - char
                        "@SET PSEUDO_FILE                    ", //  7 |  6 - char
                        "@SET WAVE_FILE                      ", //  8 |  7 - char
                        "@SET CHARGE                         ", //  9 |  8 - int
                        "@SET CUTOFF                         ", // 10 |  9 - double
                        "@SET GRIDS                          ", // 11 | 10 - int
                        "@SET QS_METHOD                      ", // 12 | 11 - char(int)
                        "@SET SCF_GUESS                      ", // 13 | 12 - char(int)
                        "@SET SCF_NCYCLES                    ", // 13 | 13 - int
                        "@SET SCF_NCONV                      ", // 14 | 14 - double
                        "@SET SCF_OCYCLES                    ", // 16 | 15 - int
                        "@SET SCF_OCONV                      ", // 17 | 16 - double
                        "@SET OT_MINI                        ", // 18 | 17 - char(int)
                        "@SET FUNCTIONAL                     ", // 19 | 18 - char(int)
                        "@SET PBC                            ", // 20 | 29 - char(int)
                        "@SET SYM                            ", // 21 | 31 - int
                        "@SET A                              ", // 22 |    - double
                        "@SET B                              ", // 23 |    - double
                        "@SET C                              ", // 24 |    - double
                        "@SET ALPHA                          ", // 25 |    - double
                        "@SET BETA                           ", // 26 |    - double
                        "@SET GAMMA                          ", // 27 |    - double
                        "@SET AX                             ", // 28 |    - double
                        "@SET AY                             ", // 29 |    - double
                        "@SET AZ                             ", // 30 |    - double
                        "@SET BX                             ", // 31 |    - double
                        "@SET BY                             ", // 32 |    - double
                        "@SET BZ                             ", // 33 |    - double
                        "@SET CX                             ", // 34 |    - double
                        "@SET CY                             ", // 35 |    - double
                        "@SET CZ                             ", // 36 |    - double
                        "@SET COORD_FILE                     ", // 37 |    - char
                        "@SET MD_ENSEMBLE                    ", // 38 | 32 - char(int)
                        "@SET MD_STEPS                       ", // 39 | 34 - int
                        "@SET MD_DELTA_T                     ", // 40 | 35 - double
                        "@SET MD_TEMP                        ", // 41 | 36 - double
                        "@SET GEO_MINI                       ", // 42 | 37 - char(int)
                        "@SET GEO_STEPS                      ", // 43 | 38 - int
                        "@SET GEO_CONV                       ", // 44 | 39 - double
                        "@SET OUT_STEPS                      ", // 45 | 40 - int
                        "@SET OUT_UNIT                       ", // 46 | 42 - char(int)
                        "@SET OUT_FORM                       ", // 47 | XYZ
                        "@SET OUT_UNIT                       "};// 48 | Angstrom

gchar * cp2kglobal[9] = {"&GLOBAL\n"
                         "  PROJECT_NAME  ","SYSTEM",
                         "\n  RUN_TYPE      ","RUN",
                         "\n  PRINT_LEVEL   ","PRINT",
                         "\n  WALLTIME      ","CPU_TIME",
                         "\n&END GLOBAL\n"};

gchar * cp2krestart[7] = {"@IF ( ","USE_RESTART"," == TRUE )\n"
                          "  &EXT_RESTART ON\n"
                          "    RESTART_DEFAULT F\n"
                          "    RESTART_FILE_NAME ","RESTART_FILE",
                          "\n    RESTART_POS T\n"
                          "    RESTART_COUNTERS T\n"
                          "    @IF ( ","RUN"," == MOLECULAR_DYNAMICS )\n"
                          "      RESTART_VEL T\n"
                          "      RESTART_THERMOSTAT T\n"
                          "    @ENDIF\n"
                          "  &END EXT_RESTART\n"
                          "@ENDIF\n"};

gchar * cp2kfev[8][17] = {{"!\n"
                           "! Always a FORCE_EVAL section that describes\n"
                           "! the method to compute energy and forces\n"
                           "!\n"
                           "&FORCE_EVAL\n"
                           "  METHOD QUICKSTEP\n"
                           "  &DFT\n"
                           "! First specify files that will be used thereafter \n"
                           "    BASIS_SET_FILE_NAME ",
                           "BASIS_FILE",
                           "\n    POTENTIAL_FILE_NAME ", "PSEUDO_FILE",
                           "\n@IF ( ","USE_RESTART"," == TRUE )\n"
                           "    WFN_RESTART_FILE_NAME ","WAVE_FILE",
                           "\n@ENDIF\n"
                           "    CHARGE ","CHARGE",
                           "\n! Going to the Multi-grids section\n"
                           "    &MGRID\n"
                           "      CUTOFF ",
                           "CUTOFF",
                           "                      ! => Cutoff of the finest grid level\n"
                           "      NGRIDS ",
                           "GRIDS",
                           "                       ! => Number of multigrids to use, default = 4\n"
                           "    &END MGRID\n"
                           "! Going to the Quickstep Setup section\n"
                           "    &QS\n"
                           "      METHOD ",
                           "QS_METHOD",
                           "\n      EPS_DEFAULT 1.0E-12                   ! => Default value is 1.0E-10\n"
                           "      MAP_CONSISTENT TRUE                   ! => This is the default value\n"},
                          {"      EXTRAPOLATION_ORDER 3                 ! => This is the default value\n"
                           "    &END QS\n"},
                          {"! Going to the Self Consistent Field section\n"
                           "    &SCF\n"
                           "! Maximum number of cycle\n"
                           "      SCF_GUESS ",
                           "SCF_GUESS",
                           "                ! => Initial guess for the wave-function\n"
                           "      MAX_SCF ","SCF_NCYCLES",
                           "                ! => Maximum number of SCF cycles\n"
                           "      EPS_SCF ","SCF_NCONV",
                           "                  ! => Threshold for the SCF convergence\n"
                           "! If after the ${SCF_NCYCLES} first SCF steps no convergence has been reached\n"
                           "! more SCF cycles can be done updating the preconditioner. \n"
                           "! Detail information is then presented in the 'Outer' SCF section\n"
                           "      &OUTER_SCF\n"
                           "        MAX_SCF ",
                           "SCF_OCYCLES",
                           "              ! => We update the preconditioner and start a new SCF cycle\n"
                           "                                            !    up to ${SCF_NCYCLES} x ${SCF_OCYCLES} can be computed\n"
                           "        EPS_SCF ",
                           "SCF_OCONV",
                           "                ! => Convergence threshold for the extra cycles\n"
                           "      &END OUTER_SCF\n\n"
                           "! Going to the Orbital Transformation section\n"
                           "      &OT ON\n"
                           "        MINIMIZER ",
                           "OT_MINI",
                           "\n        PRECONDITIONER FULL_ALL             ! => Preconditioner for the minimization scheme,\n"
                           "                                            !    FULL_ALL is the most effective state selective\n"
                           "                                            !    preconditioner and is based on diagonalization\n"
                           "        ENERGY_GAP 0.001                    ! => Underestimated value of the Gap (HOMO-LUMO) in a.u.\n"
                           "                                            !    to be used with the FULL_ALL preconditioner\n"
                           "      &END OT\n\n"
                           "      &PRINT\n"
                           "! To tune the printing of the restart file\n"
                           "        &RESTART\n"
                           "          LOG_PRINT_KEY T                   ! => Printing on screen when restart file is written\n"
                           "          &EACH\n"
                           "            QS_SCF 0                        ! => Never write restart file(s) during the SCF cycle\n"},
                          {"          &END EACH\n"
                           "          ADD_LAST NUMERIC\n"
                           "        &END RESTART\n"},
                          {"      &END PRINT\n\n"
                           "    &END SCF\n"
                           "! Going to the exchange-correlation section\n"
                           "    &XC\n"
                           "      &XC_FUNCTIONAL ",
                           "FUNCTIONAL",
                           "\n      &END XC_FUNCTIONAL\n"},
                          {"    &END XC\n"},
                          {"! Spin polarized calculation\n"
                           "    UKS ",
                           "SPIN_POLARIZED",
                           "\n    MULTIPLICITY ", "SPIN_MULTIPLICITY"},
                          {"  &END DFT\n"}};

gchar * qs_extrapo[2]={"      EXTRAPOLATION ASPC                    ! => ASPC recommended for MD, PS otherwise\n",
                       "      EXTRAPOLATION PS                      ! => ASPC recommended for MD, PS otherwise\n"};

gchar * scf_wrestart[3][3]={{"JUST_ENERGY ","OUT_STEPS","        ! => Write restart file every ${OUT_STEPS} SCF steps\n"},
                            {"GEO_OPT ","OUT_STEPS","            ! => Write restart file every ${OUT_STEPS} GEO_OPT steps\n"},
                            {"MD ","OUT_STEPS","                 ! => Write restart file every ${OUT_STEPS} MD steps\n"}};

gchar * cp2kincludes[3] = {"forces.inc", "system.inc", "motion.inc"};

gchar * thermostatr[4] = {"\n    &THERMOSTAT\n",
                          "      TYPE ",
                          "\n      REGION ",
                          "    &END THERMOSTAT"};

gchar * cp2k_thermo[4] = {"AD_LANGEVIN", "CSVR", "GLE", "NOSE"};

gchar * thermo_region[3] = {"GLOBAL", "DEFINED", "MOLECULE"};

gchar * define_region[4] = {"\n      &DEFINE_REGION",
                            "\n        LIST",
                            "\n        MOLECULE",
                            "\n      &END DEFINE_REGION"};

gchar * thermo_type[4][5] = {{"\n      &AD_LANGEVIN"
                              "\n        TIMECON_LANGEVIN ",
                              "\n        TIMECON_NH       ",
                              "\n        &CHI ",
                              "\n        &END CHI"
                              "\n        &MASS ",
                              "\n        &END MASS"
                              "\n      &END AD_LANGEVIN\n"},
                             {"\n      &CSVR"
                              "\n        TIMECON ",
                              "\n      &END CSVR\n"},
                             {"\n"},
                             {"\n      &NOSE"
                              "\n        LENGTH  ",
                              "\n        MTS     ",
                              "\n        TIMECON ",
                              "\n        YOSHIDA ",
                              "\n      &END NOSE\n"}};

gchar * cp2kmotion[4][15] = {{"&MOTION\n\n"
                              "  &GEO_OPT\n"
                              "    MINIMIZER ","GEO_MINI",
                              "\n    MAX_ITER  ","GEO_STEPS",
                              "\n    MAX_FORCE ","GEO_CONV",
                              "\n  &END GEO_OPT\n"
                              "  &PRINT\n"
                              "    &RESTART\n"
                              "      LOG_PRINT_KEY T\n"
                              "      &EACH\n"
                              "        GEO_OPT ","OUT_STEPS",
                              "\n      &END EACH\n"
                              "      ADD_LAST NUMERIC\n"
                              "    &END RESTART\n"
                              "    &TRAJECTORY\n"
                              "      LOG_PRINT_KEY T\n"
                              "      FORMAT XYZ\n"
                              "      &EACH\n"
                              "        GEO_OPT ","OUT_STEPS",
                              "\n      &END EACH\n"
                              "      ADD_LAST NUMERIC\n"
                              "    &END TRAJECTORY\n"
                              "  &END PRINT\n"},
                           {"&MOTION\n\n"
                            "  &MD\n"
                            "    ENSEMBLE  ","MD_ENSEMBLE",
                            "\n    STEPS       ","MD_STEPS",
                            "\n    TIMESTEP    ","MD_DELTA_T",
                            "\n    TEMPERATURE   ","MD_TEMP"},
                           {"\n  &END MD\n\n"
                            "  &PRINT\n"
                            "    &RESTART\n"
                            "      LOG_PRINT_KEY T\n"
                            "      &EACH\n"
                            "        MD ","OUT_STEPS",
                            "\n      &END EACH\n"
                            "      ADD_LAST NUMERIC\n"
                            "    &END RESTART\n\n"
                            "    &TRAJECTORY\n"
                            "      LOG_PRINT_KEY T\n"
                            "      FORMAT ","OUT_FORM",
                            "\n      UNIT   ","OUT_UNIT",
                            "\n      &EACH\n"
                            "        MD ","OUT_STEPS",
                            "\n      &END EACH\n"
                            "      ADD_LAST NUMERIC\n"
                            "    &END TRAJECTORY\n\n"
                            "    &VELOCITIES\n"
                            "      LOG_PRINT_KEY T\n"
                            "      FORMAT ","OUT_FORM",
                            "\n      UNIT   ","OUT_UNIT",
                            "\n      &EACH\n"
                            "        MD ","OUT_STEPS",
                            "\n      &END EACH\n"
                            "      ADD_LAST NUMERIC\n"
                            "    &END VELOCITIES\n"
                            "  &END PRINT\n"},
                           {"\n&END MOTION"}};

gchar * cp2ksyst[3][3] = {{"!\n"
                           "! Always a SUBSYS section that describes\n"
                           "! the chemistry and the periodicity of the system\n"
                           "!\n"
                           "&SUBSYS\n"
                           "  &CELL\n"
                           "    PERIODIC ",
                           "PBC","\n"},
                          {"\n  &END CELL\n"
                           "  &COORD","\n  &END COORD\n"
                          },
                          {"\n  &END CELL\n"
                           "  &TOPOLOGY\n"
                           "! Using an XYZ file, coordinates are always Cartesian and in angstrom\n"
                           "    COORDINATE XYZ\n"
                           "    COORD_FILE_NAME ",
                           "COORD_FILE",
                           "\n  &END TOPOLOGY\n"}};

gchar * cp2klat[2][19] = {{"    ABC ","A"," ","B"," ","C",
                          "\n    ALPHA_BETA_GAMMA ","ALPHA"," ","BETA"," ","GAMMA",
                          "\n    SYMMETRY ","SYM"," "," "," "," "," "},
                         {"    A ","AX"," ","AY"," ","AZ",
                          "\n    B ","BX"," ","BY"," ","BZ",
                          "\n    C ","CX"," ","CY"," ","CZ","\n"}};

gchar * cp2kspin[2] = {"@SET SPIN_POLARIZED                 ",
                       "@SET SPIN_MULTIPLICITY              "};

gchar * cp2ksroks[3] = {"@SET ROKS                           ",
                        "@SET ROKS_SCALING                   ",
                        "@SET ROKS_SPIN_CONFIG               "};

gchar * cp2kroks[7] = {"    ROKS ","ROKS",
                       "                            ! => Restricted Open Kohn-Sham calculation\n"
                       "    &LOW_SPIN_ROKS\n"
                       "      ENERGY_SCALING ",
                       "ROKS_SCALING",
                       "\n      SPIN_CONFIGURATION ", "ROKS_SPIN_CONFIG",
                       "\n    &END LOW_SPIN_ROKS\n"};

gchar * cp2kmocu[3] = {"@SET NUM_HOMO                       ",
                       "@SET NUM_LUMO                       ",
                       "@SET CUBES                          "};

gchar * cp2kmocubes[7] = {"        NHOMO ","NUM_HOMO",
                          "\n        NLUMO ","NUM_LUMO",
                          "\n        WRITE_CUBES ","CUBES",
                          "\n"};

gchar * cp2ksvdw[4] = {"@SET VDW_FCT                        ",
                       "@SET VDW_TYPE                       ",
                       "@SET VDW_CUTOFF                     ",
                       "@SET VDW_LONG                       "};

gchar * cp2kvdw[4][7] = {{"      &VDW_POTENTIAL\n"
                          "        POTENTIAL_TYPE ", "VDW_FCT",
                          "\n        "},
                         {"&NON_LOCAL\n"
                          "          TYPE ", "VDW_TYPE",
                          "\n          CUTOFF ", "VDW_CUTOFF",
                          "\n        &END NON_LOCAL\n"},
                         {"&PAIR_POTENTIAL\n"
                          "          TYPE ", "VDW_TYPE",
                          "\n          R_CUTOFF ", "VDW_CUTOFF",
                          "\n          LONG_RANGE_CORRECTION ","VDW_LONG",
                          "\n        &END PAIR_POTENTIAL\n"},
                         {"      &END VDW_POTENTIAL\n"}};

gchar * vdw_fct[2] = {"NON_LOCAL", "PAIR_POTENTIAL"};

gchar * cp2k_cons[2] = {"\n  &CONSTRAINT\n", "  &END CONSTRAINT\n"};

gchar * cp2k_fix[4] = {"    &FIXED_ATOMS\n      COMPONENTS_TO_FIX ",
                       "      LIST", "      MOLNAME",
                       "\n    &END FIXED_ATOMS\n"};

/*!
  \fn void print_cp2k_var (gchar * var, GtkTextBuffer * buffer)

  \brief print a CP2K variable name

  \param var the variable name
  \param buffer the GtkTextBuffer to print into
*/
void print_cp2k_var (gchar * var, GtkTextBuffer * buffer)
{
  print_info ("${", "bold", buffer);
  print_info (var, "bold_blue", buffer);
  print_info ("}", "bold", buffer);
}

/*!
  \fn void print_var_section (int num, gchar ** section, GtkTextBuffer * buffer)

  \brief print CP2K input file variable section

  \param num the number of element(s) to print
  \param section the section text
  \param buffer the GtkTextBuffer to print into
*/
void print_var_section (int num, gchar ** section, GtkTextBuffer * buffer)
{
  int i;
  for (i=0; i<num; i++)
  {
    if (i%2)
    {
      print_cp2k_var (section[i], buffer);
    }
    else
    {
      print_info (section[i], NULL, buffer);
    }
  }
}

/*!
  \fn gchar * cp2kbool (double opt)

  \brief print CP2K boolean

  \param opt the value to print
*/
gchar * cp2kbool (double opt)
{
  if (opt == 1.0)
  {
    return g_strdup_printf ("TRUE");
  }
  else
  {
    return g_strdup_printf ("FALSE");
  }
}

extern int v_thermo[2][CP2NTHERM];

/*!
  \fn void print_thermostat_cp2k (int n_thermo, GtkTextBuffer * buffer)

  \brief print CP2K thermostat(s) information

  \param n_thermo the number ot thermostat(s)
  \param buffer the GtkTextBuffer to print into
*/
void print_thermostat_cp2k (int n_thermo, GtkTextBuffer * buffer)
{
  int i, j, k, l, m;
  gchar * str;
  thermostat * thermo = tmp_cp2k -> ions_thermostat;
  for (i=0; i<n_thermo; i++)
  {
    if (n_thermo > 1)
    {
      str = g_strdup_printf ("\n! Thermostat N°%d", i+1);
      print_info (str, NULL, buffer);
      g_free (str);
    }
    print_info (thermostatr[0], NULL, buffer);
    print_info (thermostatr[1], NULL, buffer);
    print_info (cp2k_thermo[thermo -> type-1], "red", buffer);
    print_info (thermostatr[2], NULL, buffer);
    print_info (thermo_region[thermo -> sys], "red", buffer);
    if (thermo -> sys > 0)
    {
      print_info (define_region[0], NULL,  buffer);
      print_info (define_region[thermo -> sys], NULL,  buffer);
      if (thermo -> sys == 1)
      {
        l = 0;
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
                  print_info (str, "red", buffer);
                  g_free (str);
                }
              }
            }
          }
        }
      }
      else
      {
        str = g_strdup_printf (" MOL-%d", thermo -> natoms+1);
        print_info (str, "red", buffer);
        g_free (str);
      }
      print_info (define_region[3], NULL,  buffer);
    }

    for (j=0; j<v_thermo[1][thermo -> type]; j++)
    {
      print_info (thermo_type[thermo -> type-1][j], NULL, buffer);
      if (thermo -> type == 4 && j != 2)
      {
        str = g_strdup_printf ("%d", (int)thermo -> params[j]);
      }
      else
      {
         str = g_strdup_printf ("%f", thermo -> params[j]);
      }
      print_info (str, "red", buffer);
      g_free (str);
    }
    print_info (thermo_type[thermo -> type-1][j], NULL, buffer);

    if (thermo -> next != NULL) thermo = thermo -> next;
  }
  print_info (thermostatr[3], NULL, buffer);
}

/*!
  \fn void print_motion_cp2k (int m, GtkTextBuffer * buffer)

  \brief print the CP2K input MOTION section

  \param m 0 = Geometry optimisation, 1 = MD
  \param buffer the GtkTextBuffer to print into
*/
void print_motion_cp2k (int m, GtkTextBuffer * buffer)
{
  gchar * intro = "!\n"
                  "! Always a MOTION section that describes\n"
                  "! how to move the atoms\n"
                  "!\n";
  print_info (intro, NULL, buffer);
  print_var_section ((m==0) ? 11 : 8, cp2kmotion[m], buffer);
  if (m)
  {
    if ((int)tmp_cp2k -> opts[CP2ENS] != 2 && (int)tmp_cp2k -> opts[CP2ENS] != 3)
    {
      if (tmp_cp2k -> ions_thermostat -> type) print_thermostat_cp2k (tmp_cp2k -> thermostats, buffer);
    }
    print_var_section (15, cp2kmotion[m+1], buffer);
  }

  if ((int)tmp_cp2k -> opts[CP2CON])
  {
    gchar * fixec[7] = {"X\n", "Y\n", "Z\n", "XY\n", "XZ\n", "YZ\n", "XYZ\n"};
    int fixed[7][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
    int i, j, k;
    gchar * str;
    gboolean doit;
    for (i=0; i<2; i++)
    {
      if (tmp_cp2k -> fixat[i] > 0)
      {
        print_info (cp2k_cons[0], NULL, buffer);
        for (j=0; j<7; j++)
        {
          doit = FALSE;
          for (k=0; k<tmp_cp2k -> fixat[i]; k++)
          {
            if (tmp_cp2k -> fixcoord[i][k][0] == fixed[j][0] &&
                tmp_cp2k -> fixcoord[i][k][1] == fixed[j][1] &&
                tmp_cp2k -> fixcoord[i][k][2] == fixed[j][2])
            {
              print_info (cp2k_fix[0], NULL, buffer);
              print_info (fixec[j], NULL, buffer);
              if (i==0)
              {
                print_info (cp2k_fix[1], NULL, buffer);
              }
              else
              {
                print_info (cp2k_fix[2], NULL, buffer);
              }
              doit= TRUE;
              break;
            }
          }
          if (doit)
          {
            for (k=0; k<tmp_cp2k -> fixat[i]; k++)
            {
              if (tmp_cp2k -> fixcoord[i][k][0] == fixed[j][0] &&
                  tmp_cp2k -> fixcoord[i][k][1] == fixed[j][1] &&
                  tmp_cp2k -> fixcoord[i][k][2] == fixed[j][2])
              {
                if (i ==0)
                {
                  str = g_strdup_printf (" %d", tmp_cp2k -> fixlist[i][k]+1);
                }
                else
                {
                  str = g_strdup_printf (" MOL-%d", tmp_cp2k -> fixlist[i][k]+1);
                }
                print_info (str, NULL, buffer);
                g_free (str);
              }
            }
            print_info (cp2k_fix[3], NULL, buffer);
          }
        }
        print_info (cp2k_cons[1], NULL, buffer);
      }
    }
  }
  print_info (cp2kmotion[3][0], NULL, buffer);
}

/*!
  \fn void print_coord_cp2k (GtkTextBuffer * buffer)

  \brief print atomic coordinates in CP2K input format

  \param buffer the GtkTextBuffer to print into
*/
void print_coord_cp2k (GtkTextBuffer * buffer)
{
  gchar * str;
  if (tmp_cp2k -> input_type)
  {
    gchar * str = g_strdup_printf ("\t%d\n# This file was prepared using %s", qm_proj -> natomes, PACKAGE);
    print_info (str, NULL, buffer);
    g_free (str);
  }
  int i, j;
  for (i=0; i<qm_proj -> nspec; i++)
  {
    for (j=0; j<qm_proj -> natomes; j++)
    {
      if (qm_proj -> atoms[0][j].sp == i)
      {
        print_info ("\n", NULL, buffer);
        print_info (qm_proj -> chemistry -> label[i], NULL, buffer);
        str = g_strdup_printf ("         %15.10lf     %15.10lf     %15.10lf",
                               qm_proj -> atoms[0][j].x, qm_proj -> atoms[0][j].y, qm_proj -> atoms[0][j].z);
        print_info (str, NULL, buffer);
        g_free (str);
        /*if (qm_view -> bonding)
        {
          if (tmp_cp2k -> fixat[1] > 0 || tmp_cp2k -> ions_thermostat -> sys > 1)
          {
            str = g_strdup_printf ("         MOL-%d", qm_proj -> atoms[0][j].coord[3]+1);
            print_info (str, NULL, buffer);
            g_free (str);
          }
        }*/
      }
    }
  }
}

/*!
  \fn void print_subsys_cp2k (GtkTextBuffer * buffer)

  \brief print the CP2K input SUBSYS section

  \param buffer the GtkTextBuffer to print into
*/
void print_subsys_cp2k (GtkTextBuffer * buffer)
{
  print_var_section (3, cp2ksyst[0], buffer);
  int i = ((int)tmp_cp2k -> opts[CP2LAT]) ? 19 : 14;
  print_var_section (i, cp2klat[(int)tmp_cp2k -> opts[CP2LAT]], buffer);
  if (! tmp_cp2k -> input_type)
  {
    print_info (cp2ksyst[1][0], NULL, buffer);
    print_coord_cp2k (buffer);
    print_info (cp2ksyst[1][1], NULL, buffer);
  }
  else
  {
    print_var_section (3, cp2ksyst[2], buffer);
  }
  gchar * str;
  print_info ("! What follow is tricky and tests are recommended\n"
              "! Atomic basis set and pseudo-potential must be provided for all chemical species\n"
              "! the exact sequences '${BASIS_FOR_*}' '${POTENTIAL_FOR_*}' appear\n"
              "! inside the files that contain respectively the basis sets and the pseudo-potentials\n"
              "! in front of the name of the '*' element\n" ,
              NULL, buffer);
  for (i=0; i<qm_proj -> nspec; i++)
  {
    str = g_strdup_printf ("  &KIND %s\n    BASIS_SET ", exact_name(qm_proj -> chemistry -> label[i]));
    print_info (str, NULL, buffer);
    g_free (str);
    str = g_strdup_printf ("BASIS_FOR_%s", exact_name(qm_proj -> chemistry -> label[i]));
    print_cp2k_var (str, buffer);
    g_free (str);
    if (strlen(exact_name(qm_proj -> chemistry -> label[i])) == 1) print_info (" ",NULL, buffer);
    print_info ("\n    POTENTIAL ", NULL, buffer);

    str = g_strdup_printf ("POTENTIAL_FOR_%s", exact_name(qm_proj -> chemistry -> label[i]));
    print_cp2k_var (str, buffer);
    g_free (str);
    print_info ("\n  &END KIND\n", NULL, buffer);
  }
  print_info ("&END SUBSYS", NULL, buffer);
}

/*!
  \fn void print_variables_cp2k (GtkTextBuffer * buffer)

  \brief print the list of the CP2K input variables

  \param buffer the GtkTextBuffer to print into
*/
void print_variables_cp2k (GtkTextBuffer * buffer)
{
  int i, j, k, l;
  gchar * str;
  gboolean append;
  gchar * fileinfo[5] = {"! This file contains the restart information:",
                         "! This file contains the basis set(s):",
                         "! This file contains the pseudo-potential(s):",
                         "! This file contains the wave-function:",
                         "! This file contains the atomic coordinates in XYZ format:"};
  l = 0;
  for (i=0; i<41; i++)
  {
    if (i==0)
    {
      print_info (globopts[0], NULL, buffer);
      print_info (exact_name(qm_proj -> name), "bold", buffer);
      print_info ("\n", NULL, buffer);
      print_info (globopts[i+1], NULL, buffer);
      print_info (cp2k_default_keywords[l][(int)tmp_cp2k -> opts[i]], "blue", buffer);
      print_info ("\n", NULL, buffer);
      l++;
    }
    else if (i == CP2SPI)
    {
      if ((int)tmp_cp2k -> opts[CP2SPI])
      {
        print_info (cp2kspin[0], NULL, buffer);
        print_info (cp2kbool (tmp_cp2k -> opts[CP2SPI]), "blue", buffer);
        print_info ("\n", NULL, buffer);
        print_info (cp2kspin[1], NULL, buffer);
        str = g_strdup_printf ("%d", (int)tmp_cp2k -> opts[CP2SPM]);
        print_info (str, "blue", buffer);
        g_free (str);
        print_info ("\n", NULL, buffer);
      }
    }
    else if (i==CP2PLE || i==CP2QSM || i==CP2SCG || i==CP2SMI || i==CP2FCT)
    {
      print_info (globopts[i+1], NULL, buffer);
      print_info (cp2k_default_keywords[l][(int)tmp_cp2k -> opts[i]], "blue", buffer);
      print_info ("\n", NULL, buffer);
      l++;
      if (i==CP2SMI)
      {
        if ((int)tmp_cp2k -> opts[CP2ROK])
        {
          print_info (cp2ksroks[0], NULL, buffer);
          print_info (cp2kbool (tmp_cp2k -> opts[CP2ROK]), "blue", buffer);
          print_info ("\n", NULL, buffer);
          print_info (cp2ksroks[1], NULL, buffer);
          str = g_strdup_printf ("%f", tmp_cp2k -> extra_opts[1][0]);
          print_info (str, "blue", buffer);
          g_free (str);
          print_info ("\n", NULL, buffer);
          print_info (cp2ksroks[2], NULL, buffer);
          str = g_strdup_printf ("%d", (int)tmp_cp2k -> extra_opts[1][1] + 1);
          print_info (str, "blue", buffer);
          g_free (str);
          print_info ("\n", NULL, buffer);
        }
        if ((int)tmp_cp2k -> opts[CP2POR])
        {
          for (j=0; j<2; j++)
          {
            print_info (cp2kmocu[j], NULL, buffer);
            str = g_strdup_printf ("%d", (int)tmp_cp2k -> extra_opts[2][j]);
            print_info (str, "blue", buffer);
            g_free (str);
            print_info ("\n", NULL, buffer);
          }
          print_info (cp2kmocu[j], NULL, buffer);
          print_info (cp2kbool (tmp_cp2k -> extra_opts[2][2]), "blue", buffer);
          print_info ("\n", NULL, buffer);
        }
      }
      if (i==CP2FCT)
      {
        if ((int)tmp_cp2k -> opts[CP2VDW])
        {
          print_info (cp2ksvdw[0], NULL, buffer);
          print_info (vdw_fct[(int)tmp_cp2k -> extra_opts[0][0]], "blue", buffer);
          print_info ("\n", NULL, buffer);
          print_info (cp2ksvdw[1], NULL, buffer);
          print_info (cp2k_vdw_keywords[(int)tmp_cp2k -> extra_opts[0][0]][(int)tmp_cp2k -> extra_opts[0][1]], "blue", buffer);
          print_info ("\n", NULL, buffer);
          print_info (cp2ksvdw[2], NULL, buffer);
          str = g_strdup_printf ("%f", tmp_cp2k -> extra_opts[0][2]);
          print_info (str, "blue", buffer);
          g_free (str);
          print_info ("\n", NULL, buffer);
          if (tmp_cp2k -> extra_opts[0][0] == 1.0)
          {
            print_info (cp2ksvdw[3], NULL, buffer);
            print_info (cp2kbool (tmp_cp2k -> extra_opts[0][3]), "blue", buffer);
            print_info ("\n", NULL, buffer);
          }
        }
      }
    }
    else if (i == CP2RES)
    {
      print_info (globopts[4], NULL, buffer);
      print_info (cp2kbool (tmp_cp2k -> opts[i]), "blue", buffer);
      print_info ("\n", NULL, buffer);
      for (j=0; j<4; j++)
      {
        print_info (fileinfo[j], "bold_red", buffer);
        print_info ("\n", NULL, buffer);
        print_info (globopts[5+j], NULL, buffer);
        if (tmp_cp2k -> files[j] != NULL)
        {
          print_info (tmp_cp2k -> files[j], "blue", buffer);
        }
        else
        {
          print_info ("None", "blue", buffer);
          if ((tmp_cp2k -> opts[i] == 1.0 && (j==0 || j==3)) || (j==1 || j==2)) print_info ("\t\t! A file is required !", "bold_red", buffer);
        }
        print_info ("\n", NULL, buffer);
      }
    }
    else if (i==CP2KTI || i==CP2CHA || i==CP2GRI || i==CP2SNN ||i==CP2SNO || i==CP2NST || i==CP2MAG ||i==CP2OUF)
    {
      if (i==CP2NST && tmp_cp2k -> opts[CP2RUN] != 3.0)
      {
        append = FALSE;
      }
      else if (i==CP2MAG && tmp_cp2k -> opts[CP2RUN] != 2.0)
      {
        append = FALSE;
      }
      else
      {
        append = TRUE;
      }
      if (append)
      {
        j = (i > CP2SNO) ? i+7 : i+1;
        print_info (globopts[j], NULL, buffer);
        str = g_strdup_printf ("%d", (int)tmp_cp2k -> opts[i]);
        print_info (str, "blue", buffer);
        g_free (str);
        print_info ("\n", NULL, buffer);
        if (i==CP2OUF)
        {
          print_info (globopts[47], NULL, buffer);
          print_info ("XYZ", "blue", buffer);
          print_info ("\n", NULL, buffer);
          print_info (globopts[48], NULL, buffer);
          print_info ("ANGSTROM", "blue", buffer);
          print_info ("\n", NULL, buffer);
        }
      }
    }
    else if (i==CP2CUT || i==CP2SCN || i==CP2SCO || i==CP2DLT || i==CP2TMP || i==CP2GEF)
    {
      if ((i == CP2DLT || i == CP2TMP) && tmp_cp2k -> opts[CP2RUN] != 3.0)
      {
        append = FALSE;
      }
      else if (i == CP2GEF && tmp_cp2k -> opts[CP2RUN] != 2.0)
      {
        append = FALSE;
      }
      else
      {
        append = TRUE;
      }
      if (append)
      {
        j = (i > CP2SCO) ? i+7 : i+1;
        print_info (globopts[j], NULL, buffer);
        str = g_strdup_printf ("%f", tmp_cp2k -> opts[i]);
        print_info (str, "blue", buffer);
        g_free (str);
        print_info ("\n", NULL, buffer);
      }
    }
    else if (i == CP2PBC)
    {
      print_info (globopts[i-8], NULL, buffer);
      print_info (cp2k_default_keywords[6][(int)tmp_cp2k -> opts[i]], "blue", buffer);
      print_info ("\n", NULL, buffer);
    }
    else if (i == CP2ENS && tmp_cp2k -> opts[CP2RUN] == 3.0)
    {
      print_info (globopts[38], NULL, buffer);
      print_info (cp2k_default_keywords[9][(int)tmp_cp2k -> opts[i]], "blue", buffer);
      print_info ("\n", NULL, buffer);
    }
    else if (i == CP2GMI && tmp_cp2k -> opts[CP2RUN] == 2.0)
    {
      print_info (globopts[42], NULL, buffer);
      print_info (cp2k_default_keywords[10][(int)tmp_cp2k -> opts[i]], "blue", buffer);
      print_info ("\n", NULL, buffer);
    }
    else if (i == CP2LAT)
    {
      if (tmp_cp2k -> opts[i] == 0.0)
      {
        for (j=0; j<6; j++)
        {
          print_info (globopts[22+j], NULL, buffer);
          k = j/3;
          str = g_strdup_printf ("%f", qm_proj -> cell.box[0].param[k][j-3*k]);
          print_info (str, "blue", buffer);
          g_free (str);
          print_info ("\n", NULL, buffer);
        }
        print_info (globopts[21], NULL, buffer);
        print_info (cp2k_default_keywords[8][(int)tmp_cp2k -> opts[CP2SYM]], "blue", buffer);
        print_info ("\n", NULL, buffer);
      }
      else
      {
        for (j=0; j<3; j++)
        {
          print_info (globopts[28+j], NULL, buffer);
          k = j/3;
          str = g_strdup_printf ("%f", qm_proj -> cell.box[0].vect[k][j-3*k]);
          print_info (str, "blue", buffer);
          g_free (str);
          print_info ("\n", NULL, buffer);
        }
      }
      if (tmp_cp2k -> input_type)
      {
        print_info (fileinfo[4], "bold_red", buffer);
        print_info ("\n", NULL, buffer);
        print_info (globopts[37], NULL, buffer);
        print_info ("coord.inc", "blue", buffer);
        print_info ("\n", NULL, buffer);
      }
      for (j=0; j<qm_proj -> nspec;j++)
      {
        str = g_strdup_printf ("@SET BASIS_FOR_%s                   ", exact_name(qm_proj -> chemistry -> label[j]));
        print_info (str, NULL, buffer);
        g_free (str);
        if (strlen(exact_name(qm_proj -> chemistry -> label[j])) == 1) print_info (" ",NULL, buffer);
        if (tmp_cp2k -> spec_data[j][0] > -1)
        {
          print_info (get_nth_key (j, tmp_cp2k -> spec_data[j][0], 0), "blue", buffer);
        }
        else
        {
          print_info ("None", "blue", buffer);
          print_info ("\t\t! A keyword is required !", "bold_red", buffer);
        }
        print_info ("\n", NULL, buffer);
        str = g_strdup_printf ("@SET POTENTIAL_FOR_%s               ", exact_name(qm_proj -> chemistry -> label[j]));
        print_info (str, NULL, buffer);
        g_free (str);
        if (strlen(exact_name(qm_proj -> chemistry -> label[j])) == 1) print_info (" ",NULL, buffer);
        if (tmp_cp2k -> spec_data[j][1] > -1)
        {
          print_info (get_nth_key (j, tmp_cp2k -> spec_data[j][1], 1), "blue", buffer);
        }
        else
        {
          print_info ("None", "blue", buffer);
          print_info ("\t\t! A keyword is required !", "bold_red", buffer);
        }
        print_info ("\n", NULL, buffer);
      }
    }
  }
  print_info ("\n", NULL, buffer);
}

/*!
  \fn void print_global_cp2k (GtkTextBuffer * buffer)

  \brief print the CP2K input file GLOBAL section

  \param buffer the GtkTextBuffer to print into
*/
void print_global_cp2k (GtkTextBuffer * buffer)
{
  print_variables_cp2k (buffer);
  print_var_section (9, cp2kglobal, buffer);

  if (tmp_cp2k -> input_type)
  {
    print_info ("\n", NULL, buffer);
    print_info ("!\n", NULL, buffer);
    print_info ("! The mandatory FORCE_EVAL section include file\n", NULL, buffer);
    print_info ("!\n", NULL, buffer);
    print_info ("@INCLUDE '", "bold", buffer);
    print_info (cp2kincludes[0], "red", buffer);
    print_info ("'", "bold", buffer);
    print_info ("\n\n", NULL, buffer);
    if (tmp_cp2k -> opts[CP2RUN] > 1.0 && tmp_cp2k -> opts[CP2RUN] < 4.0)
    {
      print_info ("!\n", NULL, buffer);
      print_info ("! The mandatory MOTION section include file\n", NULL, buffer);
      print_info ("!\n", NULL, buffer);
      print_info ("@INCLUDE '", "bold", buffer);
      print_info (cp2kincludes[2], "red", buffer);
      print_info ("'", "bold", buffer);
      print_info ("\n\n", NULL, buffer);
    }
  }
  else
  {
    print_info ("\n", NULL, buffer);
  }
}

/*!
  \fn void print_cp2k_print (gchar * spaces, gchar * info, int i, int j, GtkTextBuffer * buffer)

  \brief print CP2K input file PRINT section content

  \param spaces spaces string
  \param info variable string to print
  \param i SCF restart option
  \param j 1 = MO cubes, 0 = else
  \param buffer the GtkTextBuffer to print into
*/
void print_cp2k_print (gchar * spaces, gchar * info, int i, int j, GtkTextBuffer * buffer)
{
  print_info (spaces, NULL, buffer);
  print_info ("&", NULL, buffer);
  print_info (info, NULL, buffer);
  print_info ("\n", NULL, buffer);
  print_info (spaces, NULL, buffer);
  print_info ("  LOG_PRINT_KEY T                   ! => Printing on screen when restart file is written\n", NULL, buffer);
  print_info (spaces, NULL, buffer);
  print_info ("  &EACH\n", NULL, buffer);
  print_info (spaces, NULL, buffer);
  print_info ("    ", NULL, buffer);
  print_var_section (3, scf_wrestart[i], buffer);
  print_info (spaces, NULL, buffer);
  print_info ("  &END EACH\n", NULL, buffer);
  print_info (spaces, NULL, buffer);
  print_info ("  ADD_LAST NUMERIC\n", NULL, buffer);
  if (j)
  {
    print_var_section (7, cp2kmocubes, buffer);
  }
  print_info (spaces, NULL, buffer);
  print_info ("&END ", NULL, buffer);
  print_info (info, NULL, buffer);
  print_info ("\n", NULL, buffer);
}

/*!
  \fn void print_cp2k (int f, GtkTextBuffer * buffer)

  \brief print the CP2K input file section

  \param f the section id
  \param buffer the GtkTextBuffer to print into
*/
void print_cp2k (int f, GtkTextBuffer * buffer)
{
  int i;
  switch (f)
  {
    case 0:
      print_global_cp2k (buffer);
      if (tmp_cp2k -> input_type && (int)tmp_cp2k -> opts[CP2RES]) print_var_section (7, cp2krestart, buffer);
      break;
    case 1:
      print_var_section (17, cp2kfev[0], buffer);
      i = ((int)tmp_cp2k -> opts[CP2RUN] == 3) ? 0 : 1;
      print_info (qs_extrapo[i], NULL, buffer);
      print_var_section (1, cp2kfev[1], buffer);

      i = ((int)tmp_cp2k -> opts[CP2RUN] == 2 || (int)tmp_cp2k -> opts[CP2RUN] == 3) ? (int)tmp_cp2k -> opts[CP2RUN] - 1 : 0;
      if ((int)tmp_cp2k -> opts[CP2ROK]) print_var_section (7, cp2kroks, buffer);
      if ((int)tmp_cp2k -> opts[CP2POR])
      {
        print_info ("    &PRINT\n", NULL, buffer);
        print_cp2k_print ("      ", "MO_CUBES", i, 1, buffer);
        print_info ("    &END PRINT\n", NULL, buffer);
      }

      print_var_section (13, cp2kfev[2], buffer);
      print_info ("            ", NULL, buffer);
      print_var_section (3, scf_wrestart[i], buffer);
      print_var_section (1, cp2kfev[3], buffer);
      if ((int)tmp_cp2k -> opts[CP2PMU]) print_cp2k_print ("        ", "MULLIKEN", i, 0, buffer);
      if ((int)tmp_cp2k -> opts[CP2PLO]) print_cp2k_print ("        ", "LOWDIN", i, 0, buffer);
      print_var_section (3, cp2kfev[4], buffer);
      if ((int)tmp_cp2k -> opts[CP2VDW])
      {
        print_var_section (3, cp2kvdw[0], buffer);
        print_var_section ((int)tmp_cp2k -> extra_opts[0][0]*2+5, cp2kvdw[(int)tmp_cp2k -> extra_opts[0][0]+1], buffer);
        print_var_section (1, cp2kvdw[3], buffer);
      }
      print_info (cp2kfev[5][0], NULL, buffer);
      if ((int)tmp_cp2k -> opts[CP2SPI])
      {
        print_var_section (4, cp2kfev[6], buffer);
        print_info ("\n", NULL, buffer);
      }
      print_info (cp2kfev[7][0], NULL, buffer);

      if ((int)tmp_cp2k -> opts[CP2PFO] || (int)tmp_cp2k -> opts[CP2PST])
      {
        print_info ("\n", NULL, buffer);
        print_info ("  &PRINT\n", NULL, buffer);
        if ((int)tmp_cp2k -> opts[CP2PFO]) print_cp2k_print ("    ", "FORCES", i, 0, buffer);
        if ((int)tmp_cp2k -> opts[CP2PST]) print_cp2k_print ("    ", "STRESS_TENSOR", i, 0, buffer);
        print_info ("  &END PRINT\n", NULL, buffer);
      }

      if (tmp_cp2k -> input_type)
      {
        print_info ("\n", NULL, buffer);
        print_info ("!\n", NULL, buffer);
        print_info ("! The mandatory SUBSYS section include file\n", NULL, buffer);
        print_info ("!\n", NULL, buffer);
        print_info ("@INCLUDE '", "bold", buffer);
        print_info (cp2kincludes[1], "red", buffer);
        print_info ("'", "bold", buffer);
        print_info ("\n\n", NULL, buffer);
        print_info ("&END FORCE_EVAL\n", NULL, buffer);
      }
      break;
    case 2:
      print_subsys_cp2k (buffer);
      if (! tmp_cp2k -> input_type)
      {
        print_info ("\n&END FORCE_EVAL\n", NULL, buffer);
        if ((int)tmp_cp2k -> opts[CP2RUN] != 2 && (int)tmp_cp2k -> opts[CP2RUN] != 3 && (int)tmp_cp2k -> opts[CP2RES])
        {
          print_info ("\n", NULL, buffer);
          print_var_section (7, cp2krestart, buffer);
        }
      }
      break;
    case 4:
      print_coord_cp2k (buffer);
      break;
    default:
      if (f == 5 || f == 6)
      {
        for (i=0; i<qm_proj -> nspec; i++)
        {
          if (tmp_cp2k -> spec_data[i][f-5] > -1)
          {
            print_info (get_nth_elem (i, tmp_cp2k -> spec_data[i][f-5], f-5), NULL, buffer);
          }
        }
      }
      else
      {
        if (tmp_cp2k -> opts[CP2RUN] > 1.0 && tmp_cp2k -> opts[CP2RUN] < 4.0)
        {
          print_motion_cp2k ((int)tmp_cp2k -> opts[CP2RUN]- 2, buffer);
        }
        if (! tmp_cp2k -> input_type && (int)tmp_cp2k -> opts[CP2RES]) print_var_section (7, cp2krestart, buffer);
      }
      break;
  }
}
