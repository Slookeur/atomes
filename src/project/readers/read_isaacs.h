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
* @file read_isaacs.h
* @short Function declarations to read / write ISAACS XML file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'read_isaacs.h'
*
* Contains:

 - Function declarations to read / write ISAACS XML file

*/

#ifndef ISAACSRW_H_
#define ISAACSRW_H_

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>

size_t strfind (int * ida);
int XmlwriterFilename (const char *uri);
int write_xml (const char * filetosave);
gboolean file_exists (const char * filename);
xmlNodePtr findnode (xmlNodePtr startnode, char * nname);
int setprop (xmlNodePtr pnode);
int testopening (char * tdata, char * tfichier);
int setchemistry (xmlNodePtr xsnode);
int setbox (xmlNodePtr boxnode);
int setpbc (xmlNodePtr pbcnode);
int setcutoffs (xmlNodePtr cutnode);
int settime(xmlNodePtr timenode);

int check_xml (const char * filetocheck);
gchar * open_xml (const char * filetoread);
#endif
