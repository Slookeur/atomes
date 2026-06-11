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
* @file initcoord.h
* @short Function declarations to handle the atomic coordination data
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'initcoord.h'
*
* Contains:

 - Function declarations to handle the atomic coordination data

*/

#ifdef GTK3
void prep_all_coord_menus (glwin * view);
void prep_all_ring_menus (glwin * view);
#endif // GTK3
void set_advanced_bonding_menus (glwin * view);
void partial_geo_out_ (int * sp, int * id, int * ngsp, int coord[* ngsp]);
void allocate_partial_geo_ (int * sp, int * ngsp);
void init_menu_coordinations_ (int * id, int * sp, int * ngsp, int coordt[* ngsp]);
void init_menu_fragmol_ (int * id);
void init_menurings_ (int * coo, int * ids, int * ngsp, int coordt[* ngsp], int * init);
void init_opengl_coords (int id, int nt, int init);
void send_coord_opengl_ (int * id, int * num, int * cmin, int * cmax, int * nt, int coord[* num]);
