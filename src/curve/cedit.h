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
* @file cedit.h
* @short Variable declarations for the curve layout edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'cedit.h'
*
* Contains:

 - Variable declarations for the curve layout edition window

*/

#ifndef CEDIT_H_
#define CEDIT_H_

#include "global.h"

extern GtkWidget * thesetbox;
extern GtkWidget * setcolorbox;
extern char * lapos[2];

void prepbox (gpointer data);
void edit_curve (gpointer cdata);
#endif
