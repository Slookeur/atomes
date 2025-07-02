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

// Analysis parameters

extern gboolean preferences;

extern gboolean default_clones;
extern gchar * default_delta_num_leg[8];
extern int * default_num_delta;
extern int * tmp_num_delta;
extern double * default_delta_t;
extern gchar * default_ring_param[7] ;
extern int * default_rsparam;
extern int * tmp_rsparam;
extern gchar * default_chain_param[7];
extern int * default_csparam;
extern int * tmp_csparam;

// OpenGL
extern int * default_opengl;
extern int * tmp_opengl;
extern Material default_material;
extern Material tmp_material;
extern Lightning default_lightning;
extern Lightning tmp_lightning;
extern Fog default_fog;
extern Fog tmp_fog;

// Model

extern opengl_edition * pref_ogl_edit;

extern void set_atomes_preferences ();

#endif // PREFERENCES_H_
