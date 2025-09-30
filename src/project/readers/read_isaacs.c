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
* @file read_isaacs.c
* @short Functions to read an ISAACS XML file \n
         Functions to write an ISAACS XML file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_isaacs.c'
*
* Contains:
*

 - The functions to read an ISAACS XML file
 - The functions to write an ISAACS XML file

*
* List of functions:

  int XmlwriterFilename (const char *uri);
  int write_xml (const char * filetosave);
  int get_spec_from_data (xmlChar * data);
  int setprop (xmlNodePtr pnode);
  int testopening (char * tdata, char * tfichier);
  int setchemistry (xmlNodePtr xsnode);
  int setbox (xmlNodePtr boxnode);
  int setpbc (xmlNodePtr pbcnode);
  int setcutoffs (xmlNodePtr cutnode);
  int settime(xmlNodePtr timenode);
  int check_xml (const char * filetocheck);

  size_t strfind (char * ida);

  gboolean file_exists(const char * filename);

  gchar * open_xml (const char * filetoread);

  xmlNodePtr findnode (xmlNodePtr startnode, char * nname);

*/

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>

#include "global.h"
#include "interface.h"
#include "callbacks.h"

extern int open_coordinate_file (int id);

#define NFORMATS 7

char * reg_types[NFORMATS] = {"XYZ file",
                              "Chem3D file",
                              "CPMD trajectory",
                              "VASP trajectory",
                              "multiple XYZ file",
                              "multiple Chem3D file",
                              "PDB file"};

/*!
  \fn size_t strfind (char * ida)

  \brief size of a string without spaces

  \param ida
*/
size_t strfind (char * ida)
{
  size_t a, b, c;

  a = strlen(ida);
  b = 0;
  for ( c=0 ; c < a ; c++)
  {
    if (ida[c] != ' ')
    {
      b ++;
    }
  }
  return b;
}

/*!
  \fn int XmlwriterFilename (const char * uri)

  \brief write ISAACS XML file

  \param uri the file name
*/
int XmlwriterFilename (const char * uri)
{
  int rc;
  int i, j;
  xmlTextWriterPtr writer;

  char isaacinfo[16] = " I.S.A.A.C.S. v";
  char xmlinfo[11]=" XML file ";
  const xmlChar intro[50]="";
  char * val;
  char * ncut;
  size_t lgt;

  /* Create a new XmlWriter for uri, with no compression. */
  writer = xmlNewTextWriterFilename(uri, 0);
  if (writer == NULL) return 0;
  rc = xmlTextWriterSetIndent(writer, 1);
  if (rc < 0) return 0;
  /* Start the document with the xml default for the version,
   * encoding MY_ENCODING and the default for the standalone
   * declaration. */
  rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
  if (rc < 0) return 0;


  strcpy((char *)intro, isaacinfo);
  strcat((char *)intro, "1.0");
  strcat((char *)intro, xmlinfo);
  rc = xmlTextWriterWriteComment(writer, intro);
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement(writer, BAD_CAST "isaacs-xml");
  if (rc < 0) return 0;

  /* Start the "data" element. Since this is the first
   * element, this will be the root element of the document.
   <data>
	  <type> </type>
	  <file> <file>
   </data>*/

  rc = xmlTextWriterWriteComment(writer, (const xmlChar *)" Data format and file containing the configuration(s) ");
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"data");
  if (rc < 0) return 0;
  int reg = 0;
  if (active_project -> tfile < 2)
  {
    reg = (active_project -> steps > 1) ? 4 : 0;
  }
  else if (active_project -> tfile == 2)
  {
    reg = (active_project -> steps > 1) ? 5 : 1;
  }
  else if (active_project -> tfile < 5)
  {
    reg = 2;
  }
  else if (active_project -> tfile  < 6)
  {
    reg = 3;
  }
  else if (active_project -> tfile  < 9)
  {
    reg = 6;
  }
  else
  {
    return 0;
  }

  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"type", "%s", reg_types[reg]);
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"file", "%s", active_project -> coordfile);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;

  /* Start the "chemistry" element.
  <chemistry>
    <atoms> </atoms>
    <species number=" ">
      <active_project -> label id="0"> </active_project -> label>
      ...
    </species>
    <element symbol="">
	  <name> </name>
	  <z> </z>
	  <mass> </mass>
	  <rad> </rad>
	  <radius> </radius>
	  <nscatt> </nscatt>
	  <xscatt> </xscatt>
    </element>
  </chemistry> */

  rc = xmlTextWriterWriteComment(writer, (const xmlChar *)" Chemistry information ");
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"chemistry");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"atoms", "%d", active_project -> natomes);
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"species");
  if (rc < 0) return 0;
  val = g_strdup_printf("%d",active_project -> nspec);
  rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"number", BAD_CAST val);
  g_free (val);
  if (rc < 0) return 0;
  for ( i=0 ; i<active_project -> nspec ; i++)
  {
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"label");
    if (rc < 0) return 0;
    val = g_strdup_printf("%d",i);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"id", BAD_CAST val);
    g_free (val);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatString (writer, "%s", active_chem -> label[i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 0;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;
  for ( i=0 ; i<active_project -> nspec ; i++)
  {
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"element");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"symbol", BAD_CAST active_chem -> label[i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"name", "%s ", active_chem -> element[i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"z", "%d", (int)active_chem -> chem_prop[CHEM_Z][i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"mass", "%f", active_chem -> chem_prop[CHEM_M][i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"rad", "%d", 0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"radius", "%f", active_chem -> chem_prop[CHEM_R][i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"nscatt", "%f", active_chem -> chem_prop[CHEM_N][i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"xscatt", "%f", active_chem -> chem_prop[CHEM_Z][i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 0;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;

  /* Start the "box" element.
  <box>
    <edges>
      <a></a>
      <b></b>
      <c></c>
    </edges>
    <angles> </angles>
    <vectors>
      <a.x></a.x>
      <a.y></a.y>
      <a.z></a.z>
      <b.x></b.x>
      <b.y></b.y>
      <b.z></b.z>
      <c.x></c.x>
      <c.y></c.y>
      <c.z></c.z>
    </vectors>
  </box> */

  rc = xmlTextWriterWriteComment(writer, (const xmlChar *)" Box information ");
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"box");
  if (rc < 0) return 0;
  if (active_cell -> ltype == 1)
  {
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"edges");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"a", "%f", active_box -> param[0][0]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"b", "%f", active_box -> param[0][1]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"c", "%f", active_box -> param[0][2]);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 0;
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"angles");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"alpha", "%f", active_box -> param[1][0]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"beta", "%f", active_box -> param[1][1]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"gamma", "%f", active_box -> param[1][2]);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 0;
  }
  else
  {
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"edges");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"a", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"b", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"c", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 0;
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"angles");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"alpha", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"beta", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"gamma", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 0;
  }
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"vectors");
  if (rc < 0) return 0;
  if (active_cell -> ltype == 2)
  {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"a.x", "%f", active_box -> vect[0][0]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"a.y", "%f", active_box -> vect[0][1]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"a.z", "%f", active_box -> vect[0][2]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"b.x", "%f", active_box -> vect[1][0]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"b.y", "%f", active_box -> vect[1][1]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"b.z", "%f", active_box -> vect[1][2]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"c.x", "%f", active_box -> vect[2][0]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"c.y", "%f", active_box -> vect[2][1]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"c.z", "%f", active_box -> vect[2][2]);
    if (rc < 0) return 0;
  }
  else
  {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"a.x", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"a.y", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"a.z", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"b.x", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"b.y", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"b.z", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"c.x", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"c.y", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"c.z", "%f", 0.0);
    if (rc < 0) return 0;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;
 /* Start the "pbc" element.
  <pbc>
    <apply></apply>
    <fractional></fractional>
    <fractype></fractype>
  </pbc>
  */

  rc = xmlTextWriterWriteComment(writer, (const xmlChar *)" PBC information ");
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"pbc");
  if (rc < 0) return 0;
  if (active_cell -> pbc)
  {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"apply", "%s", "TRUE");
  }
  else
  {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"apply", "%s", "FALSE");
  }
  if (rc < 0) return 0;
  if (active_cell -> frac)
  {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"fractional", "%s", "TRUE");
  }
  else
  {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"fractional", "%s", "FALSE");
  }
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"fractype", "%d", active_cell -> frac);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;

 /* Start the "cutoffs" element.
  <cutoffs>
    <total></total>
    <partials>
      <active_chem -> label[spi]-active_chem -> label[spj]></active_chem -> label[spi]-active_chem -> label[spj]>
    </partials>
  </cutoffs>
  */

  rc = xmlTextWriterWriteComment(writer, (const xmlChar *)" Cutoffs information ");
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"cutoffs");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"total", "%f", active_chem -> grtotcutoff);
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"partials");
  if (rc < 0) return 0;
  for ( i=0 ; i<active_project -> nspec ; i++ )
  {
    for ( j=0 ; j<active_project -> nspec ; j++ )
    {
      lgt = 10;
      lgt += strfind (active_chem -> label[i]) + strlen("-") + strfind(active_chem -> label[j]);
      ncut = g_malloc0 (lgt*sizeof*ncut);
      strncpy (ncut, active_chem -> label[i], strfind(active_chem -> label[i]));
      strcat (ncut, "-");
      strncat (ncut, active_chem -> label[j], strfind(active_chem -> label[j]));
      rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST ncut, "%f", active_chem -> cutoffs[i][j]);
      if (rc < 0) return 0;
      g_free (ncut);
      ncut=NULL;
    }
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;

  /* Start the "time series" element.
  <time-series>
    <dt></dt>
    <unit></unit>
    <ndt></ndt>
  </time-series>
  */

  if (active_project -> steps > 1)
  {
    rc = xmlTextWriterWriteComment(writer, (const xmlChar *)" Time series ");
    if (rc < 0) return 0;
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"time-series");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"dt", "%f", 0.0);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"unit", "t [%s]", untime[active_project -> tunit]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"ndt", "%d", 1);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 0;
  }

  /* Start the "project" element.
  <project></project>
  */

  rc = xmlTextWriterWriteComment(writer, (const xmlChar *)" Apply project ");
  if (rc < 0) return 0;
  if (active_project -> run)
  {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"project", "%s", "TRUE");
  }
  else
  {
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"project", "%s", "FALSE");
  }
  if (rc < 0) return 0;
// Closing "</isaacs-xml>"
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 0;

  rc = xmlTextWriterEndDocument(writer);
  if (rc < 0) return 0;

  xmlFreeTextWriter(writer);
  return 1;
}

/*!
  \fn int write_xml (const char * filetosave)

  \brief write XML file

  \param filetosave File to write
*/
int write_xml (const char * filetosave)
{
  /* first, the file version */
  int res=XmlwriterFilename(filetosave);
  /*
   * Cleanup function for the XML library.
   */
  xmlCleanupParser();
  /*
   * this is to debug memory for regression tests
   */
  xmlMemoryDump();
  return res;
}

/*!
  \fn gboolean file_exists(const char * filename)

  \brief file exists ?

  \param filename File name
*/
gboolean file_exists(const char * filename)
{
  FILE * file;
  if ((file = fopen(filename, "r")))
  {
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

/*!
  \fn xmlNodePtr findnode (xmlNodePtr startnode, char * nname)

  \brief find XML node

  \param startnode Starting node
  \param nname Node name to find
*/
xmlNodePtr findnode (xmlNodePtr startnode, char * nname)
{
  xmlNodePtr tmp;

  tmp = startnode;
  while (g_strcmp0 ((char *)(tmp -> name), nname) != 0)
  {
    if (tmp -> next == NULL)
    {
      return NULL;
    }
    tmp = tmp -> next;
  }
  return tmp;
}

/*!
  \fn int get_spec_from_data (xmlChar * data)

  \brief get atomic species from data

  \param data the data
*/
int get_spec_from_data (xmlChar * data)
{
  int i;
  for ( i=0 ; i<active_project -> nspec ; i++)
  {
    if (g_strcmp0 (exact_name((char *)data), active_chem -> label[i]) == 0)
    {
      return i;
    }
  }
  return -1;
}

/*!
  \fn int setprop (xmlNodePtr pnode)

  \brief read chemical properties from XML node

  \param pnode the XML node
*/
int setprop (xmlNodePtr pnode)
{
  int i, res;
  xmlAttrPtr pspec;
  xmlNodePtr idn, ptnode, ptn;
  xmlChar * data;

  ptnode=pnode;
  for ( i=0 ; i<active_project -> nspec ; i++)
  {
    ptnode = findnode(ptnode, "element");
    if (ptnode == NULL)
    {
      res=6;
      goto pend;
    }
    pspec = ptnode -> properties;
    idn = pspec -> children;
    data = xmlNodeGetContent(idn);
    res = get_spec_from_data(data);
    if (res < 0)
    {
      res=6;
      goto pend;
    }
    ptnode = ptnode -> children;
    ptn = findnode(ptnode, "name");
    if (ptn == NULL)
    {
      res=6;
      goto pend;
    }
    data = xmlNodeGetContent(ptn);
    if (g_strcmp0 (exact_name((char *)data), active_chem -> element[res]) != 0)
    {
      res=6;
      goto pend;
    }
    ptn = findnode(ptnode, "z");
    if (ptn == NULL)
    {
      res=6;
      goto pend;
    }
    data= xmlNodeGetContent(ptn);
    if ((int)active_chem -> chem_prop[CHEM_Z][res] != (int)string_to_double ((gpointer)data))
    {
      res=6;
      goto pend;
    }
    ptn = findnode(ptnode, "mass");
    if (ptn == NULL)
    {
      res=6;
      goto pend;
    }
    data= xmlNodeGetContent(ptn);
    active_chem -> chem_prop[CHEM_M][res] = string_to_double ((gpointer)data);
    ptn = findnode(ptnode, "rad");
    if (ptn == NULL)
    {
      res=6;
      goto pend;
    }
    data= xmlNodeGetContent(ptn);
    // val = string_to_double ((gpointer)data);
    ptn = findnode(ptnode, "radius");
    if (ptn == NULL)
    {
      res=6;
      goto pend;
    }
    data= xmlNodeGetContent(ptn);
    active_chem -> chem_prop[CHEM_R][res] = string_to_double ((gpointer)data);
    ptn = findnode(ptnode, "nscatt");
    if (ptn == NULL)
    {
      res=6;
      goto pend;
    }
    data= xmlNodeGetContent(ptn);
    active_chem -> chem_prop[CHEM_N][res] = string_to_double ((gpointer)data);
    ptn = findnode(ptnode, "xscatt");
    if (ptn == NULL)
    {
      res=6;
      goto pend;
    }
    data= xmlNodeGetContent(ptn);
    active_chem -> chem_prop[CHEM_X][res] = string_to_double ((gpointer)data);
    ptnode = ptnode -> parent;
    ptnode = ptnode -> next;
  }
  res = 0;
pend:
  return res;
}

/*!
  \fn int testopening (char * tdata, char * tfichier)

  \brief test atomic coordinates file opening

  \param tdata Type of coordinates
  \param tfichier File name
*/
int testopening (char * tdata, char * tfichier)
{
  int i, j;
  char * err;

  j = -1;
  for ( i=0 ; i<NFORMATS ; i ++ )
  {
    if (g_strcmp0 (reg_types[i], tdata) == 0)
    {
      j = i;
      break;
    }
  }
  if (j > -1)
  {
    if (file_exists (tfichier))
    {
      switch (j)
      {
        case 0:
          active_project -> tfile = 0;
          break;
        case 1:
          active_project -> tfile = 2;
          break;
        case 2:
          active_project -> tfile = 3;
          break;
        case 3:
          active_project -> tfile = 5;
          break;
        case 4:
          active_project -> tfile = 0;
          break;
        case 5:
          active_project -> tfile = 2;
          break;
        case 6:
          active_project -> tfile = 7;
          break;
      }
      active_project -> coordfile = tfichier;
      if (open_coordinate_file (active_project -> tfile) == 0)
      {
        return 0;
      }
      else
      {
        return 11;
      }
    }
    else
    {
      j=4;
      err=g_strdup_printf("The %s\n %s\n from the XML file does not exist\n", tdata, tfichier);
      show_error (err, 0, MainWindow);
      g_free (err);
      return j;
    }
  }
  else
  {
    show_error ("Unknown data format in XML file\n", 0, MainWindow);
    j=4;
    return j;
  }
}

/*!
  \fn int setchemistry (xmlNodePtr xsnode)

  \brief read chemistry data from node

  \param xsnode the XML node
*/
int setchemistry (xmlNodePtr xsnode)
{
  xmlNodePtr idn, cs, xnode;
  xmlAttrPtr xspec;
  xmlChar * data;
  int ats, res, i;
  double val;

  xnode = xsnode -> children;
  cs = findnode(xnode, "atoms");
  if (cs == NULL)
  {
    res=5;
    goto xend;
  }
  data = xmlNodeGetContent(cs);
  val = string_to_double ((gpointer)data);
  ats = val;
  if (ats != active_project -> natomes)
  {
    show_warning ("The number of atoms in the XML file\n"
                  "is not the same that the number of atoms\n"
                  "in the file containing the coordinates.\n"
                  "Other information from the XML file\n"
                  "will be ignored\n", MainWindow);
    res=5;
    goto xend;
  }
  else
  {
    cs = findnode(xnode, "species");
    if (cs == NULL)
    {
      res=5;
      goto xend;
    }
    xspec = cs -> properties;
    idn = xspec -> children;
    data = xmlNodeGetContent(idn);
    val = string_to_double ((gpointer)data);
    ats = val;
    if (ats != active_project -> nspec)
    {
      show_warning ("The number of chemical species in the XML file\n"
                    "is not the same that the number of chemical species\n"
                    "in the file that contains the atomic coordinates.\n", MainWindow);
      res = 5;
      goto xend;
    }
    else
    {
      cs = cs -> children;
      ats = 0;
      for ( i=0 ; i<active_project -> nspec ; i++)
      {
        cs = findnode(cs, "label");
        if (cs == NULL)
        {
          res=5;
          goto xend;
        }
        data = xmlNodeGetContent(cs);
        res = get_spec_from_data (data);
        if (res < 0)
        {
          res=5;
          goto xend;
        }
        else
        {
          ats ++;
        }
      }
      if (ats != active_project -> nspec)
      {
        res=5;
        goto xend;
      }
      xnode = xsnode -> children;
      res = setprop (xnode);
    }
  }

xend:
  return res;
}

/*!
  \fn int setbox (xmlNodePtr boxnode)

  \brief read box properties from node

  \param boxnode the XML node
*/
int setbox (xmlNodePtr boxnode)
{
  int box;
  xmlNodePtr bnode, ba, bb;
  xmlChar * data;
  double val;

  bnode = boxnode -> children;
  ba=findnode(bnode, "edges");
  if (ba == NULL)
  {
    box=7;
    goto bend;
  }
  ba = ba -> children;
  bb=findnode(ba, "a");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> param[0][0]= val;
  bb=findnode(ba, "b");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> param[0][1]= val;
  bb=findnode(ba, "c");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> param[0][2]= val;
  ba=findnode(bnode, "angles");
  if (ba == NULL)
  {
    box=7;
    goto bend;
  }
  ba = ba -> children;
  bb=findnode(ba, "alpha");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> param[1][0] = val;
  bb=findnode(ba, "beta");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> param[1][1]= val;
  bb=findnode(ba, "gamma");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  if (active_box -> param[0][0] != 0.0 && active_box -> param[0][1] != 0.0 && active_box -> param[0][2] != 0.0
      && active_box -> param[1][0] != 0.0 && active_box -> param[1][1] != 0.0 && active_box -> param[1][2] != 0.0)
  {
    active_cell -> ltype = 1;
  }
  else
  {
    active_cell -> ltype = 0;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> param[1][2]= val;
  ba=findnode(bnode, "vectors");
  if (ba == NULL)
  {
    box=7;
    goto bend;
  }
  ba = ba -> children;
  bb=findnode(ba, "a.x");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[0][0]= val;
  bb=findnode(ba, "a.y");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[0][1]= val;
  bb=findnode(ba, "a.z");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[0][2]= val;
  bb=findnode(ba, "b.x");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[1][0]= val;
  bb=findnode(ba, "b.y");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[1][1]= val;
  bb=findnode(ba, "b.z");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[1][2]= val;
  bb=findnode(ba, "c.x");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[2][0]= val;
  bb=findnode(ba, "c.y");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[2][1]= val;
  bb=findnode(ba, "c.z");
  if (bb == NULL)
  {
    box=7;
    goto bend;
  }
  data =  xmlNodeGetContent(bb);
  val = string_to_double ((gpointer)data);
  active_box -> vect[2][2]= val;

  if (active_box -> vect[0][0] != 0.0 && active_box -> vect[0][1] != 0.0 && active_box -> vect[0][2] != 0.0
      && active_box -> vect[1][0] != 0.0 && active_box -> vect[1][1] != 0.0 && active_box -> vect[1][2] != 0.0
      && active_box -> vect[2][0] != 0.0 && active_box -> vect[2][1] != 0.0 && active_box -> vect[2][2] != 0.0)
  {
    active_cell -> ltype = 2;
  }
  box = 0;

bend:
  return box;
}

/*!
  \fn int setpbc (xmlNodePtr pbcnode)

  \brief read the PBC information from node

  \param pbcnode the XML node
*/
int setpbc (xmlNodePtr pbcnode)
{
  int pbc;//, j;
  xmlNodePtr pnode, bnode;
  xmlChar * data;
  //double val;

  pnode = pbcnode -> children;
  bnode=findnode(pnode, "apply");
  if (bnode == NULL)
  {
    pbc=8;
    goto pend;
  }
  data = xmlNodeGetContent(bnode);
  active_cell -> pbc = 0;
  if (g_strcmp0 ((char *)data, (char *)"TRUE") == 0)
  {
    active_cell -> pbc = 1;
  }
  bnode=findnode(pnode, "fractional");
  if (bnode == NULL)
  {
    pbc=8;
    goto pend;
  }
  data = xmlNodeGetContent(bnode);
  active_cell -> frac = 0;
  if (g_strcmp0 ((char *)data, (char *)"TRUE") == 0)
  {
    bnode=findnode(pnode, "fractype");
    if (bnode == NULL)
    {
      pbc=8;
      goto pend;
    }
    data = xmlNodeGetContent(bnode);
    //val=string_to_double ((gpointer)data);
    //j = val;
    active_cell -> frac = 1;
  }
  pbc=0;

pend:
  return pbc;
}

/*!
  \fn int setcutoffs (xmlNodePtr cutnode)

  \brief read bond cutoffs from node

  \param cutnode the XML node
*/
int setcutoffs (xmlNodePtr cutnode)
{
  int cut;
  int i, j;
  xmlNodePtr cnode, cn;
  xmlChar * data;
  double val;
  char * ncut;
  size_t lgt;

  cnode = cutnode -> children;
  cn = findnode(cnode, "total");
  if (cn == NULL)
  {
    cut=9;
    goto cend;
  }
  data = xmlNodeGetContent(cn);
  val=string_to_double ((gpointer)data);
  active_chem -> grtotcutoff = val;
  cn = findnode(cnode, "partials");
  if (cn == NULL)
  {
    cut=9;
    goto cend;
  }
  cnode = cn -> children;
  for ( i=0 ; i<active_project -> nspec ; i++ )
  {
    for ( j=0 ; j<active_project -> nspec ; j++ )
    {
      lgt=10;
      lgt+=strfind(active_chem -> label[i]) + strlen("-") + strfind(active_chem -> label[j]);
      ncut = g_malloc0 (lgt*sizeof*ncut);
      strncpy (ncut, active_chem -> label[i], strfind(active_chem -> label[i]));
      strcat (ncut, "-");
      strncat (ncut, active_chem -> label[j], strfind(active_chem -> label[j]));
      cn = findnode(cnode, ncut);
      if (cn == NULL)
      {
        cut=9;
        goto cend;
      }
      data = xmlNodeGetContent(cn);
      val=string_to_double ((gpointer)data);
      active_chem -> cutoffs[i][j] = val;
      g_free (ncut);
      ncut=NULL;
    }
  }
  cut=0;

cend:
  return cut;
}

/*!
  \fn int settime(xmlNodePtr timenode)

  \brief read MD information from node

  \param timenode the XML node
*/
int settime(xmlNodePtr timenode)
{
  int i, j;
  //double val, delta_t;
  //int tunit, ndtbs;
  int tps;
  xmlChar * data;
  xmlNodePtr tnode, tn;

  tnode = timenode -> children;
  tn = findnode(tnode, "dt");
  if (tn == NULL)
  {
    tps=10;
    goto tend;
  }
  data = xmlNodeGetContent(tn);
  //val = string_to_double ((gpointer)data);
  //delta_t = val;
  tn = findnode(tnode, "unit");
  if (tn == NULL)
  {
    tps=10;
    goto tend;
  }
  data = xmlNodeGetContent(tn);
  j=-1;
  for ( i=0 ; i<5 ; i++)
  {
    if (g_strcmp0 ((char *)data, g_strdup_printf ("t [%s]", untime[i])) == 0) j=i;
  }
  if (j == -1)
  {
    tps=10;
    goto tend;
  }
  if (j < 5)
  {
    //tunit = j+1;
  }
  tn = findnode(tnode, "ndt");
  if (tn != NULL)
  {
    data = xmlNodeGetContent(tn);
    //val = string_to_double ((gpointer)data);
    //ndtbs = val;
  }
  tps=0;

tend:
  return tps;
}

/*!
  \fn int check_xml (const char * filetocheck)

  \brief check the opening of ISAACS XML file

  \param filetocheck File name
*/
int check_xml (const char * filetocheck)
{
  int res;
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  const xmlChar xisaacs[11]="isaacs-xml";
  xmlChar * cdata, * cfichier;
  xmlNodePtr racine, node;
  /*
   * build an xmlReader for that file
   */
  reader = xmlReaderForFile(filetocheck, NULL, 0);
  if (reader != NULL)
  {
    res = 0;
    doc = xmlParseFile(filetocheck);
    if (doc == NULL)
    {
      res=1;
      goto end;
    }
    else
    {
      racine = xmlDocGetRootElement(doc);
      if (g_strcmp0 ((char *)(racine->name), (char *)xisaacs) != 0)
      {
        res=2;
        goto end;
      }
      else
      {
        node = racine -> children;
        node = findnode(node, "data");
        if (node == NULL)
        {
          res=3;
          goto end;
        }
        node = node -> children;
        node = findnode(node, "type");
        if (node == NULL)
        {
          res=3;
          goto end;
        }
        cdata = xmlNodeGetContent(node);
        node = findnode(node, "file");
        if (node == NULL)
        {
          res=3;
          goto end;
        }
        cfichier = xmlNodeGetContent(node);
        res= testopening((char *)cdata, (char *)cfichier);
        if (res != 0)
        {
          goto end;
        }
        else
        {
          node = racine -> children;
          node = findnode(node, "chemistry");
          if (node == NULL)
          {
            res=3;
            goto end;
          }
          res= setchemistry(node);
          if (res != 0)
          {
            goto end;
          }
          node = racine -> children;
          node = findnode(node, "pbc");
          if (node == NULL)
          {
            res=3;
            goto end;
          }
          res=setpbc (node);
          if (res != 0)
          {
             goto end;
          }
          node = racine -> children;
          node = findnode(node, "box");
          if (node == NULL)
          {
            res=3;
            goto end;
          }
          active_cell -> box = g_malloc0(sizeof*active_cell -> box);
          active_box = & active_cell -> box[0];
          res=setbox (node);
          if (res != 0)
          {
             goto end;
          }
          node = racine -> children;
          node = findnode(node, "cutoffs");
          if (node == NULL)
          {
            res=3;
            goto end;
          }
          active_chem -> cutoffs = allocddouble (active_project -> nspec, active_project -> nspec);
          res=setcutoffs (node);
          if (res != 0)
          {
             goto end;
          }
          if (active_project -> steps > 1)
          {
            node = racine -> children;
            node = findnode(node, "time-series");
            if (node == NULL)
            {
              res=3;
              goto end;
            }
            res=settime (node);
            if (res != 0)
            {
               goto end;
            }
          }
          node = racine -> children;
          node = findnode(node, "project");
          if (node == NULL)
          {
            res=3;
            goto end;
          }
          cdata=xmlNodeGetContent(node);
          if (g_strcmp0 ((char *)cdata, (char *)"TRUE") == 0)
          {
            active_project -> run = 1;
          }
          else
          {
            active_project -> run = 0;
          }
        }
      }
    }
  }
  else
  {
    res=1;
  }

end:
  /*
   * Cleanup function for the XML library.
   */
  if (reader != NULL) xmlFreeTextReader(reader);
  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();

  return res;
}

/*!
  \fn gchar * open_xml (const char * filetoread)

  \brief Open ISAACS XML file

  \param filetoread File name
*/
gchar * open_xml (const char * filetoread)
{
  int oxml;
  oxml = check_xml(filetoread);
  switch(oxml)
  {
    case 1:
      return ("Impossible to open the Isaacs project file (XML) file\n");
      break;
    case 2:
      return ("The file is not a valid Isaacs project file (XML) file\n");
      break;
    case 3:
      return ("The Isaacs project file (XML) file is incomplete\n");
      break;
    case 4:
      return ("Problem(s) in the &lt;data&gt; section of the Isaacs project file (XML)\n");
      break;
    case 5:
      return ("Problem(s) in the &lt;chemistry&gt; section of the Isaacs project file (XML)\n");
      break;
    case 6:
      return ("Problem(s) in the &lt;element&gt; section of the Isaacs project file (XML)\n");
      break;
    case 7:
      return ("Problem(s) in the &lt;box&gt; section of the Isaacs project file (XML)\n");
      break;
    case 8:
      return ("Problem(s) in the &lt;pbc&gt; section of the Isaacs project file (XML)\n");
      break;
    case 9:
      return ("Problem(s) in the &lt;cutoffs&gt; section of the Isaacs project file (XML)\n");
      break;
    case 10:
      return ("Problem(s) in the &lt;time-series&gt; section of the Isaacs project file (XML)\n");
      break;
    default:
      return NULL;
      break;
  }
}
