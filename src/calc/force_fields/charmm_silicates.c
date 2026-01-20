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
* @file charmm_silicates.c
* @short CHARMM-charmm_silicates force field, from 'charmm_silicates'
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'charmm_silicates.c'
*
* Contains:
*

  CHARMM-charmm_silicates force field, data from taken from:

       - the file: 'charmm_silicates'
       - distributed in: 'toppar_c36_jul16_mass_orig.tgz'

  This file contains several tables:

    Atoms      : charmm_silicates_atoms[13][5]
    Bonds      : charmm_silicates_bonds[14][5]
    Angles     : charmm_silicates_angles[27][8]
    Dihedrals  : charmm_silicates_dihedrals[34][8]
    Non bonded : charmm_silicates_vdw[13][6]

*/

#include "global.h"

int charmm_silicates_objects[14] = {13, 0, 0, 14, 0, 0, 27, 0, 34, 0 , 0, 0, 13, 0};
int charmm_silicates_dim[14] = {5, 0, 0, 5, 0, 0, 8, 0, 8, 0, 0, 0, 6, 0};

/*
 Atoms
  0= Element, 1= Mass, 2= Key, 3= Description, 4= charmm_silicates info
*/
char * charmm_silicates_atoms[13][5]= {{"H", "1.008000", "HT", "TIPS3P WATER HYDROGEN", "1"},
                                 {"O", "15.99940", "OT", "TIPS3P WATER OXYGEN", "2"},
                                 {"D", "1.008000", "DUM", "dummy atom", "3"},
                                 {"H", "1.008000", "HSiO", "Si hydroxyl hydrogen;", "100"},
                                 {"H", "1.008000", "HSiA", "Si aliphatic hydrogen;", "101"},
                                 {"H", "1.008000", "HAl", "Si aliphatic hydrogen;", "102"},
                                 {"O", "15.999000", "OSiH", "Si Hydroxyl Oxygen;", "103"},
                                 {"O", "15.999000", "OSiE", "Si Ester Oxygen;", "104"},
                                 {"O", "15.999000", "OSiA", "Si Ester Oxygen;", "105"},
                                 {"O", "15.999000", "OAl", "Si Hydroxyl Oxygen;", "106"},
                                 {"Si", "28.085500", "Si", "Si Atom;", "107"},
                                 {"Si", "28.085500", "SiH3", "Si Atom;", "108"},
                                 {"Al", "26.981540", "Al", "Aluminum Atom;", "110"}};

/*
 Quadratic bonds
  0= Key_a, 1= Key_b, 2= R0 (A), 3= Kb (kcal mol^-1 A^2), 4= charmm_silicates info

  V(R) = Kb x (R - R0)^2
*/
char * charmm_silicates_bonds[14][5]= {{"Si", "OSiE", "302.000", "1.698", " "},
                                  {"Al", "OSiE", "302.000", "1.698", " "},
                                  {"Si", "OSiA", "302.000", "1.698", " "},
                                  {"Si", "OSiH", "325.000", "1.68", " "},
                                  {"Al", "OAl", "302.000", "2.00", " "},
                                  {"SiH3", "OSiE", "304.000", "1.682", " "},
                                  {"HSiO", "OSiH", "566.000", "0.975", " "},
                                  {"HAl", "OAl", "566.000", "0.9600", " "},
                                  {"Al", "HAl", "450.000", "0.9572", " "},
                                  {"Si", "HSiA", "204.0", "1.489", " "},
                                  {"HSiA", "SiH3", "204.0", "1.489", " "},
                                  {"Si", "OAl", "304.000", "1.682", " "},
                                  {"HT", "HT", "0.0", "1.5139", "from TIPS3P geometry (for SHAKE w/PARAM)"},
                                  {"HT", "OT", "450.0", "0.9572", "from TIPS3P geometry"}};

/*
 Quadratic angles:
  0= Key_a, 1= Key_b, 2= Key_c, 3= Ktheta (kcal mol^-1 rad^2), 4= Theta0 (deg)

  V(Theta) = Ktheta * (Theta - Theta0)^2

 Urey-Bradley angles:
  5= Kub (kcal mol^-1 A^2), 6= S0 (A), 7= charmm_silicates info

  V(S) = Kub x (S - S0)^2
*/
char * charmm_silicates_angles[27][8]= {{"OSiE", "Si", "OSiE", "30.000", "117.00", " ", " ", "from sim1"},
                                   {"OSiE", "SiH3", "OSiE", "30.000", "121.50", " ", " ", "from sim1"},
                                   {"Si", "OSiE", "SiH3", "34.000", "150.50", " ", " ", "from sim1"},
                                   {"Si", "OSiE", "Si", "34.000", "150.00", " ", " ", "from sim1"},
                                   {"Si", "OSiA", "Si", "34.000", "150.00", " ", " ", "from sim1"},
                                   {"OSiA", "Si", "OSiH", "32.000", "126.00", " ", " ", "from sim1"},
                                   {"OSiA", "Si", "OSiA", "30.000", "121.50", " ", " ", "from sim1"},
                                   {"OSiA", "Si", "OSiE", "30.000", "121.50", " ", " ", "from sim1"},
                                   {"SiH3", "OSiE", "SiH3", "34.000", "159.00", " ", " ", "from sim1"},
                                   {"HSiA", "SiH3", "HSiA", "30.00", "119.00", " ", " ", "from sim1"},
                                   {"HSiA", "SiH3", "OSiE", "44.000", "118.00", " ", " ", "from sim1"},
                                   {"Si", "OSiH", "HSiO", "34.00", "122.50", " ", " ", "quartz, from sim2"},
                                   {"OSiE", "Si", "OSiH", "32.000", "126.00", " ", " ", "from sim2"},
                                   {"OSiH", "Si", "OSiH", "30.000", "117.00", " ", " ", "from sim2"},
                                   {"Al", "OAl", "Al", "30.000", "98.00", " ", " ", " "},
                                   {"Al", "OAl", "Si", "30.000", "117.00", " ", " ", " "},
                                   {"OAl", "Si", "OSiE", "30.000", "117.00", " ", " ", " "},
                                   {"HAl", "OAl", "Al", "35.000", "93.40", " ", " ", " "},
                                   {"OAl", "Al", "HAl", "35.000", "93.40", " ", " ", " "},
                                   {"OAl", "Al", "OAl", "30.000", "90.00", " ", " ", " "},
                                   {"OSiE", "Al", "OSiE", "30.000", "117.00", " ", " ", " "},
                                   {"OSiE", "Al", "OAl", "30.000", "117.00", " ", " ", " "},
                                   {"OSiE", "Al", "HAl", "30.00", "119.00", " ", " ", " "},
                                   {"Al", "OSiE", "Si", "30.000", "117.00", " ", " ", " "},
                                   {"Al", "OSiE", "Al", "30.000", "117.00", " ", " ", " "},
                                   {"OSiE", "Si", "HSiA", "44.000", "118.00", " ", " ", " "},
                                   {"HT", "OT", "HT", "55.0", "104.52", " ", " ", "FROM TIPS3P GEOMETRY"}};

/*
 Dihedrals
  0-3= Keys, 4= Kchi (kcal mol^-1), 5= n (multi), 6= delta (deg), 7= charmm_silicates info

  V(chi) = Kchi x (1 + cos (n x (chi) - delta))
*/
char * charmm_silicates_dihedrals[34][8]= {{"OSiE", "Si", "OSiE", "SiH3", "0.18000", "5", "0.00", "quartz, from sim1"},
                                      {"OSiE", "Si", "OSiE", "Si", "0.18000", "5", "0.00", "quartz, from sim1"},
                                      {"OSiA", "Si", "OSiA", "Si", "0.18000", "5", "0.00", "quartz, from sim1"},
                                      {"OSiE", "Si", "OSiA", "Si", "0.18000", "5", "0.00", "quartz, from sim1"},
                                      {"OSiE", "SiH3", "OSiE", "Si", "0.18000", "5", "0.00", "quartz, from sim1"},
                                      {"OSiH", "Si", "OSiE", "SiH3", "0.12000", "5", "0.00", "quartz, from sim1"},
                                      {"Si", "OSiE", "Si", "OSiA", "0.12000", "5", "0.00", "quartz, from sim1"},
                                      {"HSiA", "SiH3", "OSiE", "Si", "0.20000", "3", "0.00", "quartz, from sim1"},
                                      {"HSiA", "SiH3", "OSiE", "SiH3", "0.20000", "3", "0.00", "quartz, from sim1"},
                                      {"OSiE", "Si", "OSiH", "HSiO", "0.30000", "3", "0.00", "quartz, from sim2"},
                                      {"OSiH", "Si", "OSiH", "HSiO", "0.35000", "3", "0.00", "quartz, from sim2"},
                                      {"Si", "OSiA", "Si", "OSiH", "0.12000", "5", "0.00", "quartz, from sim1"},
                                      {"SiH3", "OSiE", "SiH3", "OSiE", "0.18000", "5", "0.00", "quartz, from sim1"},
                                      {"Si", "OSiE", "Si", "HSiA", "0.20000", "3", "0.00", " "},
                                      {"Si", "OSiE", "Si", "OSiH", "0.18000", "5", "0.00", " "},
                                      {"SiH3", "OSiE", "Si", "OSiA", "0.18000", "5", "0.00", " "},
                                      {"OSiA", "Si", "OSiH", "HSiO", "0.35000", "3", "0.00", "quartz, from sim2"},
                                      {"OAl", "Al", "OAl", "HAl", "0.10000", "3", "0.00", " "},
                                      {"Al", "OAl", "Si", "OSiE", "0.10000", "3", "0.00", " "},
                                      {"Al", "OAl", "Al", "OAl", "0.10000", "3", "0.00", " "},
                                      {"Al", "OAl", "Al", "HAl", "0.30000", "3", "0.00", " "},
                                      {"Si", "OAl", "Al", "OAl", "0.10000", "3", "0.00", " "},
                                      {"Si", "OSiE", "Si", "OAl", "0.10000", "3", "0.00", " "},
                                      {"Si", "OAl", "Al", "HAl", "0.30000", "3", "0.00", " "},
                                      {"HAl", "OAl", "Al", "HAl", "0.30000", "3", "0.00", " "},
                                      {"Al", "OSiE", "Si", "OSiE", "0.10000", "3", "0.00", " "},
                                      {"Al", "OSiE", "Al", "OSiE", "0.10000", "3", "0.00", " "},
                                      {"Al", "OSiE", "Al", "OAl", "0.10000", "3", "0.00", " "},
                                      {"Al", "OAl", "Al", "OSiE", "0.10000", "3", "0.00", " "},
                                      {"Al", "OSiE", "Al", "HAl", "0.30000", "3", "0.00", " "},
                                      {"Si", "OSiE", "Al", "OSiE", "0.30000", "3", "0.00", " "},
                                      {"Si", "OSiE", "Al", "OAl", "0.30000", "3", "0.00", " "},
                                      {"Si", "OSiE", "Al", "HAl", "0.10000", "3", "0.00", " "},
                                      {"OSiE", "Al", "OAl", "HAl", "0.10000", "3", "0.00", " "}};

/*
 Impropers
  0-3= Keys, 4= Kpsi (kcal mol^-1 rad^-2), 5= psi0 (deg), 6= charmm_silicates info

  V(psi) = Kpsi x (psi - psi0)^2
*/

/*
 Non-bonded
  0= Key, 1= epsilon (kcal mol^-1), 2= Rmin/2 (A), 3= epsilon[1-4], 4= Rmin[1-4]/2, 5= charmm_silicates info

  V(rij) =  Eps(ij) x [(Rmin(ij)/rij)^12 - 2 x (Rmin(ij)/rij)^6]
  With:
    Esp(ij) = sqrt(epsilon([i) x epsilon[j])
    Rmin(ij)= (Rmin[i] + Rmin[j])/2
*/
char * charmm_silicates_vdw[13][6]= {{"HSiO", "-0.0460", "0.2245", " ", " ", "proteins"},
                               {"HSiA", "-0.022", "1.3200", " ", " ", "alkane"},
                               {"HAl", "-0.022", "1.3200", " ", " ", "alkane"},
                               {"Al", "-0.650", "2.2000", " ", " ", "to Phos"},
                               {"Si", "-0.600", "2.2000", " ", " ", "to Phos"},
                               {"SiH3", "-0.600", "2.2000", " ", " ", "to Phos"},
                               {"OSiH", "-0.1521", "1.7700", " ", " ", "proteins"},
                               {"OSiE", "-0.1521", "1.7700", " ", " ", "nucleic acids"},
                               {"OSiA", "-0.1521", "1.7700", " ", " ", "nucleic acids"},
                               {"OAl", "-0.1521", "1.7700", " ", " ", "proteins"},
                               {"HT", "-0.046", "0.2245", " ", " ", "water"},
                               {"OT", "-0.1521", "1.7682", " ", " ", "water"},
                               {"DUM", "-0.0000", "0.0000", " ", " ", "atom"}};
