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
* @file preferences.h
* @short Preference variable declarations
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'preferences.h'
*
* Contains:

 - Preference variable declarations

*/

#ifndef PREFERENCES_H_

#define PREFERENCES_H_

gchar * default_delta_num_leg[7] = {"<b>g(r)</b>: number of &#x3b4;r", "<b>s(q)</b>: number of &#x3b4;q", "<b>s(k)</b>: number of &#x3b4;k", "<b>g(r) FFT</b>: number of &#x3b4;r",
                                    "<b>d<sub>ij</sub></b>: number of &#x3b4;r [D<sub>ij</sub>min-D<sub>ij</sub>max]", "<b>angles</b>: number of &#x3b4;&#x3b8; [0-180°]",  "<b>Spherical harmonics</b>: l<sub>max</sub> in [2-40]"};
int default_num_delta[7];          /*!< Number of x points: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = sp, \n 7 = msd */

gchar * default_ring_search[2] = {"Default search", "Maximum number of rings of size <i><b>n</b></i> per MD step<sup>**</sup>"};
int default_rsearch[2];            /*!< Ring statistics parameters: 0 = Search type, 1 = Ring's allocation parameter NUMA */

gchar * default_ring_param[5] = {"Atom(s) to initiate the search from",
                                 " <i><b>n</b><sub>max</sub></i> = maximum size for a ring <sup>*</sup>",
                                 "Only search for ABAB rings",
                                 "No homopolar bonds in the rings (A-A, B-B ...) <sup>***</sup>",
                                 "No homopolar bonds in the connectivity matrix"};
  // 0 = Initnode, 1 = RMAX, 2 = ABAB, 3 = Homo, 4 = Homo in DMTX
int default_rsparam[5];            /*!< Ring statistics parameters: \n
                                        0 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                        1 = Maximum size of ring for the search Rmax, \n
                                        2 = Search only for ABAB rings or not, \n
                                        3 = Include Homopolar bond(s) in the analysis or not, \n
                                        4 = Include homopolar bond(s) when calculating the distance matrix */

gchar * default_chain_search = "Maximum number of chains of size <i><b>n</b></i> per MD step <sup>**</sup>";
int default_csearch;                  /*!< Chain statistics allocation parameter: CNUMA */

{"Only search for AAAA chains", "Only search for ABAB chains"
  // 0 = Initnode, 1 = AAAA, 2 = ABAB, 3 = Homo, 4 = 1221, 5 = RMAX, 6 = Done + Chains
gchar * default_chain_param[6] = {"Atom(s) to initiate the search from", ,
                                  "Only search for AAAA chains",
                                  "Only search for ABAB chains",
                                  "No homopolar bonds in the chains (A-A, B-B ...) <sup>***</sup>",
                                  "Only search for 1-(2)<sub>n</sub>-1 coordinated atom chains, ie. isolated chains.",
                                  "<i><b>n</b><sub>max</sub></i> = maximum size for a chain <sup>*</sup>"};
int default_csparam[6];             /*!< Chain statistics parameters: \n
                                         0 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                         1 = Search only for AAAA chains or not, \n
                                         2 = Search only for ABAB chains or not, \n
                                         3 = Include Homopolar bond(s) in the analysis or not, \n
                                         4 = Search only for 1-(2)n-1 chains, \n
                                         5 = Maximum size of chain for the search Rmax */


#endif
