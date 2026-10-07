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
* @file save_sml.c
* @short Functions to save atomic coordinates in SML format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'save_sml.c'
*
* Contains:
*

 - The functions to save atomic coordinates in SML format
   SML "Simple Chemical Library - XML" is file format of atomes library

*
* List of functions:

  int save_sml_chemistry (xmlTextWriterPtr writer, project * this_proj);
  int save_sml_coordinates (xmlTextWriterPtr writer, project * this_proj);
  int sml_writer (project * this_proj);
  int write_sml (project * this_proj);
  int open_sml_file_out_of_library ();

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "bind.h"
#include "readers.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>

extern int clean_xml_data (xmlDoc * doc, xmlTextReaderPtr reader);
extern xmlNodePtr findnode (xmlNodePtr startnode, char * nname);
extern double get_z_from_periodic_table (gchar * lab);

/*!
  \fn int save_sml_chemistry (xmlTextWriterPtr writer, project * this_proj)

  \brief write content of SML file

  \param writer the target XML writer
  \param this_proj the target project
*/
int save_sml_chemistry (xmlTextWriterPtr writer, project * this_proj)
{
  int rc;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"chemistry");
  if (rc < 0) return 1;
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"atoms", "%d", this_proj -> natomes);
  if (rc < 0) return 1;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"species");
  if (rc < 0) return 1;
  gchar * val;
  val = g_strdup_printf("%d", this_proj -> nspec);
  rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"number", BAD_CAST val);
  g_free (val);
  if (rc < 0) return 1;
  int i;
  for ( i=0 ; i<this_proj -> nspec ; i++ )
  {
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"label");
    if (rc < 0) return 1;
    val = g_strdup_printf("%d", i);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"id", BAD_CAST val);
    g_free (val);
    if (rc < 0) return 1;

    val = g_strdup_printf("%d", this_proj -> chemistry -> nsps[i]);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"num", BAD_CAST val);
    g_free (val);
    if (rc < 0) return 1;

    rc = xmlTextWriterWriteFormatString (writer, "%s", exact_name(this_proj -> chemistry -> label[i]));
    if (rc < 0) return 1;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 1;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 1;
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 1;

  return 0;
}

/*!
  \fn int save_sml_coordinates (xmlTextWriterPtr writer, project * this_proj)

  \brief write content of SML file

  \param writer the target XML writer
  \param this_proj the target project
*/
int save_sml_coordinates (xmlTextWriterPtr writer, project * this_proj)
{
  int rc;
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"coordinates");
  if (rc < 0) return 1;
  int i;
  gchar * val;
  for ( i=0 ; i<this_proj -> natomes ; i++ )
  {
    rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"atom");
    if (rc < 0) return 1;
    val = g_strdup_printf("%d", i+1);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"id", BAD_CAST val);
    g_free (val);
    if (rc < 0) return 1;

    val = g_strdup_printf("%d", this_proj -> atoms[0][i].sp);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"sp", BAD_CAST val);
    g_free (val);
    if (rc < 0) return 1;

    val = g_strdup_printf("%f", this_proj -> atoms[0][i].x);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"x", BAD_CAST val);
    g_free (val);
    if (rc < 0) return 1;
    val = g_strdup_printf("%f", this_proj -> atoms[0][i].y);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"y", BAD_CAST val);
    g_free (val);
    if (rc < 0) return 1;
    val = g_strdup_printf("%f", this_proj -> atoms[0][i].z);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"z", BAD_CAST val);
    g_free (val);
    if (rc < 0) return 1;
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) return 1;
  }
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 1;

  return 0;
}

/*!
  \fn int sml_writer (project * this_proj)

  \brief write content of SML file

  \param this_proj the target project
*/
int sml_writer (project * this_proj)
{
  int rc;
  xmlTextWriterPtr writer;
   /* Create a new XmlWriter with no compression. */
  writer = xmlNewTextWriterFilename(this_proj -> coordfile, 0);
  if (writer == NULL) return 1;
  rc = xmlTextWriterSetIndent(writer, 1);
  if (rc < 0) return 1;

  /* Start the document with the xml default for the version,
   * encoding MY_ENCODING and the default for the standalone
   * declaration. */
  rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
  if (rc < 0) return 1;

  rc = xmlTextWriterWriteComment(writer, (const xmlChar *)_(" Simple chemical library XML file "));
  if (rc < 0) return 1;
  rc = xmlTextWriterStartElement(writer, BAD_CAST "scl-xml");
  if (rc < 0) return 1;

  // class : corresponding family of molecule in the atomes software library
  // Setting-up "Misc" as default
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"class", "%s", "Misc");
  if (rc < 0) return 1;

  // Starting "names" section
  rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"names");
  if (rc < 0) return 1;

  // Library name = name displayed in atomes library, ask for it ?
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"library-name", "%s", "Name in atomes library");
  if (rc < 0) return 1;

  // Ask for IUPAC name ?
  rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"iupac-name", "%s", "IUPAC name");
  if (rc < 0) return 1;

  // Ask for other name(s) ?
  // rc = xmlTextWriterStartElement(writer, BAD_CAST (const xmlChar *)"other-names");
  // if (rc < 0) return 1;
  // rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"name", "%s", "If any other name here !");
  // if (rc < 0) return 1;
  // rc = xmlTextWriterEndElement(writer);
  // if (rc < 0) return 1;

  // End "names" section
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 1;

  // Ask for information ?
  // xmlTextWriterWriteFormatElement(writer, BAD_CAST (const xmlChar *)"information", "%s", "Information");
  // if (rc < 0) return 1;

  if (save_sml_chemistry (writer, this_proj) < 0) return 1;

  if (save_sml_coordinates (writer, this_proj) < 0) return 1;

  // Closing "</scl-xml>"
  rc = xmlTextWriterEndElement(writer);
  if (rc < 0) return 1;

  rc = xmlTextWriterEndDocument(writer);
  if (rc < 0) return 1;

  xmlFreeTextWriter(writer);
  return 0;
}

/*!
  \fn int write_sml (project * this_proj)

  \brief write SML file

  \param this_proj the target project
*/
int write_sml (project * this_proj)
{
  /* first, the file version */
  int res = sml_writer (this_proj);
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

//
/*!
  \fn int open_sml_file_out_of_library ()

  \brief read 'Simple chemical library XML' file outside of atomes library
*/
int open_sml_file_out_of_library ()
{
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  const xmlChar sml[8]="scl-xml";
  xmlNodePtr racine, chem_node;
  xmlNodePtr at_node, sp_node, coord_node;
  xmlNodePtr spec_node, lab_node, lot_node;
  xmlAttrPtr xspec;
  xmlChar * content;
  int i;
  gchar * val;
  /*
   * build an xmlReader for that file
   */
  reader = xmlReaderForFile (active_project -> coordfile, NULL, 0);
  if (reader == NULL)
  {
    return 0;
  }
  else
  {
    doc = xmlParseFile (active_project -> coordfile);
    if (doc == NULL) return 0;
    racine = xmlDocGetRootElement(doc);
    if (g_strcmp0 ((char *)(racine -> name), (char *)sml) != 0)
    {
      return clean_xml_data (doc, reader);
    }

    chem_node = findnode(racine -> children, "chemistry");
    if (chem_node == NULL) return clean_xml_data (doc, reader);
    at_node = findnode (chem_node -> children, "atoms");
    if (at_node == NULL) return clean_xml_data (doc, reader);
    content = xmlNodeGetContent(at_node);
    this_reader -> natomes = (int)string_to_double ((gpointer)content);
    xmlFree (content);
    this_reader -> steps = 1; // Always single configuration in SML file

    sp_node = findnode (chem_node -> children, "species");
    if (sp_node == NULL) return clean_xml_data (doc, reader);
    spec_node = sp_node -> properties -> children;
    if (spec_node == NULL) return clean_xml_data (doc, reader);
    content = xmlNodeGetContent(spec_node);
    this_reader -> nspec = (int)string_to_double ((gpointer)content);
    xmlFree (content);
    if (this_reader -> natomes < 1 || this_reader -> nspec < 1) return clean_xml_data (doc, reader);
    lab_node = sp_node -> children;
    if (lab_node == NULL) return clean_xml_data (doc, reader);

    this_reader -> z = allocdouble (this_reader -> nspec);
    this_reader -> nsps = allocint (this_reader -> nspec);
    for (i=0; i<this_reader -> nspec; i++)
    {
      lab_node = findnode (lab_node, "label");
      if (lab_node == NULL) return clean_xml_data (doc, reader);
      content = xmlNodeGetContent(lab_node);
      val = g_strdup_printf ("%s", content);
      xmlFree (content);
      this_reader -> z[i] = get_z_from_periodic_table (val);
      g_free (val);

      xspec = lab_node -> properties;
      if (xspec == NULL) return clean_xml_data (doc, reader);
      while (xspec)
      {
        lot_node = xspec -> children;
        if (lot_node == NULL) return clean_xml_data (doc, reader);
        if (g_strcmp0 ("num",(char *)xspec -> name) == 0)
        {
          content = xmlNodeGetContent(lot_node);
          this_reader -> nsps[i] = (int)string_to_double ((gpointer)content);
          xmlFree (content);
        }
        xspec = xspec -> next;
      }
      lab_node = lab_node -> next;
    }

    active_project -> steps = this_reader -> steps;
    active_project -> natomes = this_reader -> natomes;
    allocatoms (active_project);

    at_node = findnode (racine -> children, "coordinates");
    if (at_node == NULL) return clean_xml_data (doc, reader);
    coord_node = at_node -> children;
    if (coord_node == NULL) return clean_xml_data (doc, reader);
    for (i=0; i<this_reader -> natomes; i++)
    {
      coord_node = findnode (coord_node, "atom");
      if (coord_node == NULL) return clean_xml_data (doc, reader);
      xspec = coord_node -> properties;
      if (xspec == NULL) return clean_xml_data (doc, reader);
      while (xspec)
      {
        lot_node = xspec -> children;
        if (lot_node == NULL) return clean_xml_data (doc, reader);
        content = xmlNodeGetContent(lot_node);
        if (g_strcmp0 ("x",(char *)xspec -> name) == 0)
        {
          active_project -> atoms[0][i].x = string_to_double ((gpointer)content);
        }
        else if (g_strcmp0 ("y",(char *)xspec -> name) == 0)
        {
          active_project -> atoms[0][i].y = string_to_double ((gpointer)content);
        }
        else if (g_strcmp0 ("z",(char *)xspec -> name) == 0)
        {
          active_project -> atoms[0][i].z = string_to_double ((gpointer)content);
        }
        else if (g_strcmp0 ("sp",(char *)xspec -> name) == 0)
        {
          active_project -> atoms[0][i].sp = (int)string_to_double ((gpointer)content);
          active_project -> atoms[0][i].show[0] = TRUE;
        }
        xmlFree (content);
        xspec = xspec -> next;
      }
      coord_node = coord_node -> next;
    }
    xmlFreeDoc(doc);
    xmlFreeTextReader(reader);
    xmlCleanupParser();
    return 1;
  }
}
