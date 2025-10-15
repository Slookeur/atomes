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
* @file workspace.h
* @short Function declarations for workspace managment
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'workspace.h'
*
* Contains:

 - Function declarations for workspace managment

*/

#ifndef WORKSPACE_H_
#define WORKSPACE_H_

void correct_this_window_title (GtkWidget * win, gchar * str);
G_MODULE_EXPORT void activate_project (GtkWidget * widg, gpointer data);
G_MODULE_EXPORT void change_project_name (GtkWidget * wid, gpointer edata);
void add_project_to_workspace ();
void remove_project_from_workspace (int id);
#endif
