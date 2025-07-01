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
* @file preferences.c
* @short Functions to create the 'User preferences' window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'configuration.c'
*
* Contains:
*

 - The GUI for the general configuration window
 - The associated controllers

*
* List of functions:



*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"
#include "glview.h"
#include "glwin.h"
#include "preferences.h"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

extern xmlNodePtr findnode (xmlNodePtr startnode, char * nname);
extern int search_type;
extern void calc_rings (GtkWidget * vbox);

extern G_MODULE_EXPORT gboolean scroll_scale_quality (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
extern GtkWidget * materials_tab (glwin * view, opengl_edition * ogl_edit, Material * the_mat);
extern GtkWidget * lights_tab (glwin * view, opengl_edition * ogl_edit, Lightning * the_light);
extern GtkWidget * fog_tab (glwin * view, opengl_edition * ogl_edit, Fog * the_fog);
extern G_MODULE_EXPORT void scale_quality (GtkRange * range, gpointer data);
extern void duplicate_fog (Fog * new_fog, Fog * old_fog);
extern void duplicate_material (Material * new_mat, Material * old_mat);
extern Light init_light_source (int type, float val, float vbl);
extern Light * copy_light_sources (int dima, int dimb, Light * old_sp);
extern GtkWidget * lightning_fix (glwin * view, Material * this_material);
extern GtkWidget * adv_box (GtkWidget * box, char * lab, int size, float xalign);
extern float mat_min_max[5][2];
extern gchar * ogl_settings[3][10];

GtkWidget * preference_notebook = NULL;

gchar * default_delta_num_leg[7] = {"<b>g(r)</b>: number of &#x3b4;r", "<b>s(q)</b>: number of &#x3b4;q", "<b>s(k)</b>: number of &#x3b4;k", "<b>g(r) FFT</b>: number of &#x3b4;r",
                                    "<b>D<sub>ij</sub></b>: number of &#x3b4;r [D<sub>ij</sub>min-D<sub>ij</sub>max]", "<b>Angles distribution</b>: number of &#x3b4;&#x3b8; [0-180°]",  "<b>Spherical harmonics</b>: l<sub>max</sub> in [2-40]"};
int * default_num_delta = NULL;   /*!< Number of x points: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = sp */
int * tmp_num_delta = NULL;

gchar * default_ring_param[7] = {"Default search",
                                 "Atom(s) to initiate the search from",
                                 " <i><b>n</b><sub>max</sub></i> = maximum size for a ring <sup>*</sup>",
                                 "Maximum number of rings of size <i><b>n</b></i> per MD step<sup>**</sup>",
                                 "Only search for ABAB rings",
                                 "No homopolar bonds in the rings (A-A, B-B ...) <sup>***</sup>",
                                 "No homopolar bonds in the connectivity matrix"};
int * default_rsparam = NULL;     /*!< Ring statistics parameters: \n
                                       0 = Default search, \n
                                       1 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                       2 = Maximum size of ring for the search Rmax, \n
                                       3 = Maximum number of ring(s) per MD step NUMA, \n
                                       4 = Search only for ABAB rings or not, \n
                                       5 = Include Homopolar bond(s) in the analysis or not, \n
                                       6 = Include homopolar bond(s) when calculating the distance matrix */
int * tmp_rsparam = NULL;

// 0 = Initnode, 1 = RMAX, 2 = CNUMAn 3 = AAAA, 4 = ABAB, 5 = Homo, 6 = 1221
gchar * default_chain_param[7] = {"Atom(s) to initiate the search from",
                                  "<i><b>n</b><sub>max</sub></i> = maximum size for a chain <sup>*</sup>",
                                  "Maximum number of chains of size <i><b>n</b></i> per MD step <sup>**</sup>",
                                  "Only search for AAAA chains",
                                  "Only search for ABAB chains",
                                  "No homopolar bonds in the chains (A-A, B-B ...) <sup>***</sup>",
                                  "Only search for 1-(2)<sub>n</sub>-1 chains, ie. isolated chains"};
int * default_csparam = NULL;     /*!< Chain statistics parameters: \n
                                       0 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                       1 = Maximum size for a chain Cmax, \n
                                       2 = Maximum number of chain(s) per MD step CNUMA, \n
                                       3 = Search only for AAAA chains or not, \n
                                       4 = Search only for ABAB chains or not, \n
                                       5 = Include Homopolar bond(s) in the analysis or not, \n
                                       6 = Search only for 1-(2)n-1 chains */
int * tmp_csparam = NULL;

gchar * default_ogl_leg[5] = {"Default style", "Atom(s) color map", "Polyhedra color map", "Quality", "Number of light sources"};
int * default_opengl = NULL;
int * tmp_opengl = NULL;

Material default_material;
Material tmp_material;
Lightning default_lightning;
Lightning tmp_lightning;
Fog default_fog;
Fog tmp_fog;

gboolean preferences = FALSE;
opengl_edition * pref_ogl_edit = NULL;

/*!
  \fn int xml_save_xyz_to_file (xmlTextWriterPtr writer, int did, gchar * legend, gchar * key, vec3_t data)

  \brief save vector data (x,y,z) to XML file

  \param writer the XML writer to update
  \param did id, if any
  \param legend the corresponding legend
  \param data the data to save
*/
int xml_save_xyz_to_file (xmlTextWriterPtr writer, int did, gchar * legend, gchar * key, vec3_t data)
{
  int rc;
  gchar * str;
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute(writer, BAD_CAST (const xmlChar *)"info", BAD_CAST legend);
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST key);
  if (rc < 0) return 0;
  if (did > -1)
  {
    str = g_strdup_printf ("%d", did);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST str);
    g_free (str);
    if (rc < 0) return 0;
  }
  str = g_strdup_printf ("%f", data.x);
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"x", BAD_CAST str);
  g_free (str);
  if (rc < 0) return 0;
  str = g_strdup_printf ("%f", data.y);
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"y", BAD_CAST str);
  g_free (str);
  if (rc < 0) return 0;
  str = g_strdup_printf ("%f", data.z);
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"z", BAD_CAST str);
  g_free (str);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;
  return 1;
}

/*!
  \fn int save_preferences_to_xml_file ()

  \brief save software preferences to XML file
*/
int save_preferences_to_xml_file ()
{
  int rc;
#ifdef G_OS_WIN32
  ATOMES_CONFIG = g_build_filename (PACKAGE_PREFIX, "atomes.pml", NULL);
#else
  struct passwd * pw = getpwuid(getuid());
  ATOMES_CONFIG = g_strdup_printf ("%s/.config/atomes/atomes.pml", pw -> pw_dir);
#endif

  xmlTextWriterPtr writer;

  gchar * xml_delta_num_leg[7] = {"g(r): number of δr", "s(q): number of δq", "s(k): number of δk", "g(r) FFT: number of δr",
                                  "Dij: number of δr [min(Dij)-max(Dij)]", "Angles distribution: number of δθ [0-180°]",  "Spherical harmonics: l(max) in [2-40]"};
  gchar * xml_rings_leg[7] = {"Default search",
                              "Atom(s) to initiate the search from",
                              "Maximum size for a ring",
                              "Maximum number of rings of size n per MD step",
                              "Only search for ABAB rings",
                              "No homopolar bonds in the rings (A-A, B-B ...)",
                              "No homopolar bonds in the connectivity matrix"};
  gchar * xml_chain_leg[7] = {"Atom(s) to initiate the search from",
                              "Maximum size for a ring",
                              "Maximum number of rings of size n per MD step",
                              "Only search for AAAA chains",
                              "Only search for ABAB chains",
                              "No homopolar bonds in the chains (A-A, B-B ...)",
                              "Only search for 1-(2)n-1 chains"};
  gchar * xml_opengl_leg[7] = {"Default style",
                               "Atom(s) color map",
                               "Polyhedra color map",
                               "Quality",
                               "Lightning model",
                               "Material",
                               "Fog"};
  gchar * xml_material_leg[8] = {"Predefine material",
                                 "Lightning model",
                                 "Metallic",
                                 "Roughness",
                                 "Back lightning",
                                 "Gamma",
                                 "Opacity",
                                 "Albedo"};
  gchar * xml_lightning_leg[7] = {"Type",
                                  "Fix",
                                  "Position",
                                  "Direction",
                                  "Intensity",
                                  "Attenuation",
                                  "Spot specifics"};
  gchar* xml_fog_leg[5] = {"Mode",
                           "Type",
                           "Density",
                           "Depth",
                           "Color"};

  /* Create a new XmlWriter for ATOMES_CONFIG, with no compression. */
  writer = xmlNewTextWriterFilename (ATOMES_CONFIG, 0);
  if (writer == NULL) return 0;
  rc = xmlTextWriterSetIndent(writer, 1);
  if (rc < 0) return 0;
  /* Start the document with the xml default for the version,
   * encoding MY_ENCODING and the default for the standalone
   * declaration. */
  rc = xmlTextWriterStartDocument (writer, NULL, MY_ENCODING, NULL);
  if (rc < 0) return 0;

  rc = xmlTextWriterWriteComment (writer, (const xmlChar *)"atomes preferences XML file");
  if (rc < 0) return 0;

  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"atomes_preferences-xml");
  if (rc < 0) return 0;

  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"analysis");
  if (rc < 0) return 0;
  int i;
  gchar * str;

  for (i=0; i<7; i++)
  {
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_delta_num_leg[i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "default_num_delta");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", i);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST (const xmlChar *)str);
    g_free (str);
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", default_num_delta[i]);
    rc = xmlTextWriterWriteFormatString (writer, "%s", str);
    g_free (str);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement (writer);
    if (rc < 0) return 0;
  }

  for (i=0; i<7; i++)
  {
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_rings_leg[i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "default_rsparam");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", i);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST (const xmlChar *)str);
    g_free (str);
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", default_rsparam[i]);
    rc = xmlTextWriterWriteFormatString (writer, "%s", str);
    g_free (str);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement (writer);
    if (rc < 0) return 0;
  }

  for (i=0; i<7; i++)
  {
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_chain_leg[i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "default_csparam");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", i);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST (const xmlChar *)str);
    g_free (str);
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", default_csparam[i]);
    rc = xmlTextWriterWriteFormatString (writer, "%s", str);
    g_free (str);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement (writer);
    if (rc < 0) return 0;
  }

  // End analysis
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"opengl");
  if (rc < 0) return 0;
  for (i=0; i<4; i++)
  {
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_opengl_leg[i]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "default_opengl");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", i);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST (const xmlChar *)str);
    g_free (str);
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", default_opengl[i]);
    rc = xmlTextWriterWriteFormatString (writer, "%s", str);
    g_free (str);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement (writer);
    if (rc < 0) return 0;
  }
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"material");
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_material_leg[0]);
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "default_material");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST "-1");
  if (rc < 0) return 0;
  str = g_strdup_printf ("%d", default_material.predefine);
  rc = xmlTextWriterWriteFormatString (writer, "%s", str);
  g_free (str);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;
  for (i=0; i<6; i++)
  {
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_material_leg[i+1]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "default_material");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", i);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST (const xmlChar *)str);
    g_free (str);
    if (rc < 0) return 0;
    str = g_strdup_printf ("%f", default_material.param[i]);
    rc = xmlTextWriterWriteFormatString (writer, "%s", str);
    g_free (str);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement (writer);
    if (rc < 0) return 0;
  }

  rc = xml_save_xyz_to_file (writer, 6, xml_material_leg[7], "default_material", default_material.albedo);
  if (! rc) return rc;

  // End matertial
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"lightning");
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST "Number of lights");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "default_lightning");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST "0");
  if (rc < 0) return 0;
  str = g_strdup_printf ("%d", default_lightning.lights);
  rc = xmlTextWriterWriteFormatString (writer, "%s", str);
  g_free (str);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;
  for (i=0; i<default_lightning.lights; i++)
  {
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"light");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", i);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST (const xmlChar *)str);
    g_free (str);
    if (rc < 0) return 0;
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_lightning_leg[0]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "light.type");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", default_lightning.spot[i].type);
    rc = xmlTextWriterWriteFormatString (writer, "%s", str);
    g_free (str);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement (writer);
    if (rc < 0) return 0;
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_lightning_leg[1]);
    if (rc < 0) return 0;
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "light.fix");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", default_lightning.spot[i].fix);
    rc = xmlTextWriterWriteFormatString (writer, "%s", str);
    g_free (str);
    if (rc < 0) return 0;
    rc = xmlTextWriterEndElement (writer);
    if (rc < 0) return 0;

    if (default_lightning.spot[i].type)
    {
      rc = xml_save_xyz_to_file (writer, -1, xml_lightning_leg[2], "light.position", default_lightning.spot[i].position);
      if (! rc) return rc;
    }
    if (default_lightning.spot[i].type != 1)
    {
      rc = xml_save_xyz_to_file (writer, -1, xml_lightning_leg[3], "light.direction", default_lightning.spot[i].direction);
      if (! rc) return rc;
    }
    rc = xml_save_xyz_to_file (writer, -1, xml_lightning_leg[4], "light.intensity", default_lightning.spot[i].intensity);
    if (! rc) return rc;
    if (default_lightning.spot[i].type)
    {
      rc = xml_save_xyz_to_file (writer, -1, xml_lightning_leg[5], "light.attenuation", default_lightning.spot[i].attenuation);
      if (! rc) return rc;
      rc = xml_save_xyz_to_file (writer, -1, xml_lightning_leg[6], "light.spot", default_lightning.spot[i].spot_data);
      if (! rc) return rc;
    }
     rc = xmlTextWriterEndElement (writer);
    if (rc < 0) return 0;
  }

  // End lightning
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"fog");
  if (rc < 0) return 0;
  // Mode
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_fog_leg[0]);
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "fog.mode");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST "0");
  if (rc < 0) return 0;
  str = g_strdup_printf ("%d", default_fog.mode);
  rc = xmlTextWriterWriteFormatString (writer, "%s", str);
  g_free (str);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement (writer);
  // Type
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_fog_leg[1]);
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "fog.type");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST "1");
  if (rc < 0) return 0;
  str = g_strdup_printf ("%d", default_fog.based);
  rc = xmlTextWriterWriteFormatString (writer, "%s", str);
  g_free (str);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;
  // Density
  if (rc < 0) return 0;
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_fog_leg[2]);
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "fog.density");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST "2");
  if (rc < 0) return 0;
  str = g_strdup_printf ("%f", default_fog.density);
  rc = xmlTextWriterWriteFormatString (writer, "%s", str);
  g_free (str);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;
  // Depth
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_fog_leg[3]);
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST "fog.depth");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST "3");
  if (rc < 0) return 0;
  str = g_strdup_printf ("%f", default_fog.depth[0]);
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"start", BAD_CAST str);
  g_free (str);
  if (rc < 0) return 0;
  str = g_strdup_printf ("%f", default_fog.depth[1]);
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"end", BAD_CAST str);
  g_free (str);
  if (rc < 0) return 0;
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;
  // Color
  rc = xml_save_xyz_to_file (writer, 4, xml_fog_leg[4], "fog.color", default_fog.color);
  if (! rc) return rc;
  // End fog
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  // End opengl
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  rc = xmlTextWriterEndDocument (writer);
  if (rc < 0) return 0;

  xmlFreeTextWriter (writer);
  return 1;
}

/*!
  \fn void set_parameter (double value, gchar * key, int vid, vec3_t * vect, int start, int end)

  \brief set default parameter

  \param value the value to set
  \param key the name of variable to set
  \param vid the id number to set
  \param vect vector to set, if any
  \param start initial value, if any, -1.0 otherwise
  \param end final value, if any, -1.0 otherwise

*/
void set_parameter (double value, gchar * key, int vid, vec3_t * vect)
{
  if (g_strcmp0(key, "default_num_delta") == 0)
  {
    default_num_delta[vid] = (int)value;
  }
  else if (g_strcmp0(key, "default_rsparam") == 0)
  {
    default_rsparam[vid] = (int)value;
  }
  else if (g_strcmp0(key, "default_csparam") == 0)
  {
    default_csparam[vid] = (int)value;
  }
  else if (g_strcmp0(key, "default_opengl") == 0)
  {
    default_opengl[vid] = (int)value;
  }
  else if (g_strcmp0(key, "default_material") == 0)
  {
    if (vid < 0)
    {
      default_material.predefine = (int)value;
    }
    else if (vid == 6)
    {
      if (vect)
      {
        default_material.albedo = * vect;
      }
    }
    else
    {
      default_material.param[vid] = value;
    }
  }
  else if (g_strcmp0(key, "default_lightning") == 0)
  {

  }
  else if (g_strcmp0(key, "fog.mode") == 0)
  {
    default_fog.mode = (int)value;
  }
  else if (g_strcmp0(key, "fog.type") == 0)
  {
    default_fog.type = (int)value;
  }
  else if (g_strcmp0(key, "fog.density") == 0)
  {
    default_fog.density = value;
  }
  else if (g_strcmp0(key, "fog.depth") == 0)
  {
    if (start != -1.0) default_fog.depth[0] = start;
    if (end != -1.0) default_fog.depth[1] = end;
  }
  else if (g_strcmp0(key, "fog.color") == 0 && vect)
  {
    default_fog.color = * vect;
  }
}

/*!
  \fn void read_parameter (xmlNodePtr parameter_node)

  \brief read preferences from XML configuration

  \param parameter_node node the XML node that point to parameter
*/
void read_parameter (xmlNodePtr parameter_node)
{
  xmlNodePtr p_node;
  xmlAttrPtr p_details;
  gboolean set_codevar, set_id;
  gboolean set_x, set_y, set_z;
  gchar * key;
  int id;
  double start, end;
  double value;
  vec3_t vec;
  while (parameter_node)
  {
    value = string_to_double ((gpointer)xmlNodeGetContent(parameter_node));
    p_details = parameter_node -> properties;
    set_codevar = set_id = FALSE;
    set_x = set_y = set_z = FALSE;
    while (p_details)
    {
      p_node = p_details -> children;
      if (p_node)
      {
        start = end = -1.0;
        if (g_strcmp0("key",(char *)p_details -> name) == 0)
        {
          key = g_strdup_printf ("%s", xmlNodeGetContent(p_node));
          set_codevar = TRUE;
        }
        else if (g_strcmp0("id",(char *)p_details -> name) == 0)
        {
          id = (int) string_to_double ((gpointer)xmlNodeGetContent(p_node));
          set_id = TRUE;
        }
        else if (g_strcmp0("x",(char *)p_details -> name) == 0)
        {
          vec.x = string_to_double ((gpointer)xmlNodeGetContent(p_node));
          set_x = TRUE;
        }
        else if (g_strcmp0("y",(char *)p_details -> name) == 0)
        {
          vec.y = string_to_double ((gpointer)xmlNodeGetContent(p_node));
          set_y = TRUE;
        }
        else if (g_strcmp0("z",(char *)p_details -> name) == 0)
        {
          vec.z = string_to_double ((gpointer)xmlNodeGetContent(p_node));
          set_z = TRUE;
        }
        else if (g_strcmp0("start",(char *)p_details -> name) == 0)
        {
          start = string_to_double ((gpointer)xmlNodeGetContent(p_node));
        }
        else if (g_strcmp0("end",(char *)p_details -> name) == 0)
        {
          end = string_to_double ((gpointer)xmlNodeGetContent(p_node));
        }
      }
      p_details = p_details -> next;
    }
    if (set_codevar && set_id)
    {
      set_parameter (value, key, id, (set_x && set_y && set_z) ? & vec : NULL, start, end);
    }
    parameter_node = parameter_node -> next;
    parameter_node = findnode (parameter_node, "parameter");
  }
}

/*!
  \fn void read_preferences (xmlNodePtr preference_node)

  \brief read preferences from XML configuration

  \param preference_node node the XML node that point to preferences
*/
void read_preferences (xmlNodePtr preference_node)
{
  xmlNodePtr node, l_node;

  node = findnode (preference_node  -> children, "parameter");
  read_parameter (node);
  node = findnode (preference_node  -> children, "material");
  if (node)
  {
    read_parameter (findnode (node -> children, "parameter"));
  }
  node = findnode (preference_node  -> children, "lightning");
  if (node)
  {
    read_parameter (findnode (node -> children, "parameter"));
    l_node = findnode (node -> children, "light");
    while (l_node)
    {
      read_parameter (l_node);
      l_node = l_node -> next;
      l_node = findnode (l_node, "light");
    }
  }
  node = findnode (preference_node  -> children, "fog");
  if (node)
  {
    read_parameter (findnode (node -> children, "parameter"));
  }
}

/*!
  \fn void read_preferences_from_xml_file ()

  \brief read software preferences from XML file
*/
void read_preferences_from_xml_file ()
{
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  xmlNodePtr racine;
  xmlNodePtr node;
  const xmlChar aml[22]="atomes_preferences-xml";
  reader = xmlReaderForFile (ATOMES_CONFIG, NULL, 0);
  if (reader)
  {
    doc = xmlParseFile (ATOMES_CONFIG);
    if (doc)
    {
      racine = xmlDocGetRootElement (doc);
      if (! g_strcmp0 ((char *)(racine -> name), (char *)aml))
      {
        node = findnode(racine -> children, "analysis");
        if (node)
        {
          read_preferences (node);
        }
        node = findnode(racine -> children, "opengl");
        if (node)
        {
          read_preferences (node);
        }
        node = findnode(racine -> children, "model");
        if (node)
        {
          read_preferences (node);
        }
      }
      xmlFreeDoc(doc);
      xmlCleanupParser();
    }
    xmlFreeTextReader(reader);
  }
}

G_MODULE_EXPORT void restore_defaults_parameters (GtkButton * but, gpointer data);

/*!
  \fn void set_atomes_preferences ()

  \brief set software default parameters
*/
void set_atomes_preferences ()
{
  default_num_delta = allocint (7);
  default_rsparam = allocint (7);
  default_csparam = allocint (7);
  default_opengl = allocint (5);
  restore_defaults_parameters (NULL, NULL);
  read_preferences_from_xml_file ();
}

/*!
  \fn GtkWidget * view_preferences ()

  \brief view preferences
*/
GtkWidget * view_preferences ()
{
  GtkWidget * vbox = create_vbox (BSEP);

  return vbox;
}

/*!
  \fn GtkWidget * model_preferences ()

  \brief model preferences
*/
GtkWidget * model_preferences ()
{
  GtkWidget * vbox = create_vbox (BSEP);

  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void set_default_style (GtkComboBox * box, gpointer data)

  \brief change default atom(s) and bond(s) style

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_default_style (GtkComboBox * box, gpointer data)
{
  GtkTreeIter iter;
  if (gtk_combo_box_get_active_iter (box, & iter))
  {
    GtkTreeModel * model = gtk_combo_box_get_model (box);
    int i;
    gtk_tree_model_get (model, & iter, 1, & i, -1);
    tmp_opengl[0] = (! i) ? 0 : i;
  }
}

/*!
  \fn GtkTreeModel * style_combo_tree ()

  \brief create opengl style combo model
*/
GtkTreeModel * style_combo_tree ()
{
  GtkTreeIter iter, iter2;
  GtkTreeStore * store;
  int i, j;
  store = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_INT);
  gtk_tree_store_append (store, & iter, NULL);
  gtk_tree_store_set (store, & iter, 0, "f(atoms) <sup>*</sup>", 1, 0, -1);
  for (i=0; i<OGL_STYLES; i++)
  {
    gtk_tree_store_append (store, & iter, NULL);
    gtk_tree_store_set (store, & iter, 0, text_styles[i], 1, i+1, -1);
    if (i == SPACEFILL)
    {
      for (j=0; j<FILLED_STYLES; j++)
      {
        gtk_tree_store_append (store, & iter2, & iter);
        gtk_tree_store_set (store, & iter2, 0, text_filled[j], 1, -j-1, -1);
      }
    }
  }
  return  GTK_TREE_MODEL (store);
}

/*!
  \fn G_MODULE_EXPORT void set_default_map (GtkComboBox * box, gpointer data)

  \brief change default atom(s) or polyhedra color map

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_default_map (GtkComboBox * box, gpointer data)
{
  int i, j;
  i = gtk_combo_box_get_active (box);
  j = GPOINTER_TO_INT(data);
  tmp_opengl[j+1] = i;
}

/*!
  \fn GtkWidget * combo_map (int obj)

  \brief create color map combo

  \param obj 0 = atom(s), 1 = polyhedra
*/
GtkWidget * combo_map (int obj)
{
  GtkWidget * combo = create_combo ();
  combo_text_append (combo, "Atomic species");
  combo_text_append (combo, "Total coordination(s)");
  combo_text_append (combo, "Partial coordination(s)");
  gtk_combo_box_set_active (GTK_COMBO_BOX(combo), tmp_opengl[1+obj]);
  g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(set_default_map), GINT_TO_POINTER(obj));
  return combo;
}

/*!
  \fn GtkWidget * opengl_preferences ()

  \brief OpenGL preferences
*/
GtkWidget * opengl_preferences ()
{
  GtkWidget * notebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  show_the_widgets (notebook);
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * combo;

  // Crearting an OpenGL edition data structure
  pref_ogl_edit = g_malloc0(sizeof*pref_ogl_edit);
  int i;
  for (i=0; i<6; i++)
  {
    pref_ogl_edit -> pointer[i].a = -1;
    pref_ogl_edit -> pointer[i].b = i;
  }

  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("<b>Style</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 15);
  GtkTreeModel * model = style_combo_tree ();
  combo = gtk_combo_box_new_with_model (model);
  g_object_unref (model);
  GtkCellRenderer * renderer = gtk_cell_renderer_combo_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);
  gtk_combo_box_set_active (GTK_COMBO_BOX(combo), tmp_opengl[0]);
  GList * cell_list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(combo));
  if (cell_list && cell_list -> data)
  {
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), cell_list -> data, "markup", 0, NULL);
  }
  g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK(set_default_style), NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<sup>*</sup> if 10 000 atoms or more: <i>Wireframe</i>, otherwise: <i>Ball and stick</i>", -1, -1, 0.5, 0.5), FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);

  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("<b>Color maps</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("\tatom(s) and bond(s)", 250, -1, 0.0, 0.5), FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo_map(0), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);

  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("\tpolyhedra", 250, -1, 0.0, 0.5), FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo_map(1), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);

  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("<b>Quality</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, create_hscale (2, 500, 1, tmp_opengl[3], GTK_POS_TOP, 1, 175,
                                                   G_CALLBACK(scale_quality), G_CALLBACK(scroll_scale_quality), NULL), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);

  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("<b>Lightning model</b>", 250, -1, 0.0, 0.5), FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lightning_fix (NULL, & tmp_material), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vbox, gtk_label_new ("General"));

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), materials_tab (NULL, pref_ogl_edit, & tmp_material), gtk_label_new ("Material"));

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), lights_tab (NULL, pref_ogl_edit, & tmp_lightning), gtk_label_new ("Lights"));

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), fog_tab (NULL, pref_ogl_edit, & tmp_fog), gtk_label_new ("Fog"));

  // gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook), 0);
  return notebook;
}

/*!
  \fn G_MODULE_EXPORT void set_default_num_delta (GtkEntry * res, gpointer data)

  \brief update default number of delta preferences

  \param res the GtkEntry the signal is coming from
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_default_num_delta (GtkEntry * res, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  tmp_num_delta[i] = (int) string_to_double ((gpointer)m);
  update_entry_int (res, tmp_num_delta[i]);
}

/*!
  \fn GtkWidget * calc_preferences ()

  \brief analysis preferences
*/
GtkWidget * calc_preferences ()
{
  GtkWidget * notebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  show_the_widgets (notebook);
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * entry;
  int i;
  for (i=0; i<7; i++)
  {
    hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (default_delta_num_leg[i], 350, -1, 0.0, 0.5), FALSE, FALSE, 15);
    entry = create_entry (G_CALLBACK(set_default_num_delta), 150, 15, TRUE, GINT_TO_POINTER(i));
    update_entry_int ((GtkEntry *)entry, tmp_num_delta[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  }
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vbox, gtk_label_new ("General"));
  vbox = create_vbox (BSEP);
  search_type = 0;
  calc_rings (vbox);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vbox, gtk_label_new ("Ring statistics"));
  vbox = create_vbox (BSEP);
  search_type = 1;
  calc_rings (vbox);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vbox, gtk_label_new ("Chain statistics"));

  return notebook;
}

/*!
  \fn void clean_all_tmp ()

  \brief free all temporary buffers
*/
void clean_all_tmp ()
{
  if (tmp_num_delta)
  {
    g_free (tmp_num_delta);
    tmp_num_delta = NULL;
  }
  if (tmp_rsparam)
  {
    g_free (tmp_rsparam);
    tmp_rsparam = NULL;
  }
  if (tmp_csparam)
  {
    g_free (tmp_csparam);
    tmp_csparam = NULL;
  }
  if (tmp_opengl)
  {
    g_free (tmp_opengl);
    tmp_opengl = NULL;
  }
  if (tmp_lightning.spot)
  {
    g_free (tmp_lightning.spot);
    tmp_lightning.spot = NULL;
  }
}

/*!
  \fn void prepare_tmp_default ()

  \brief prepare temporary parameters
*/
void prepare_tmp_default ()
{
  clean_all_tmp ();
  tmp_num_delta = duplicate_int (7, default_num_delta);
  tmp_rsparam = duplicate_int (7, default_rsparam);
  tmp_csparam = duplicate_int (7, default_csparam);
  tmp_opengl = duplicate_int (5, default_opengl);
  duplicate_material (& tmp_material, & default_material);
  tmp_lightning.lights = default_lightning.lights;
  tmp_lightning.spot = copy_light_sources (tmp_lightning.lights, tmp_lightning.lights, default_lightning.spot);
  duplicate_fog (& tmp_fog, & default_fog);
}

/*!
  \fn void save_preferences ()

  \brief save user preferences
*/
void save_preferences ()
{
  if (default_num_delta)
  {
    g_free (default_num_delta);
    default_num_delta = NULL;
  }
  default_num_delta = duplicate_int (7, tmp_num_delta);
  if (default_rsparam)
  {
    g_free (default_rsparam);
    default_rsparam = NULL;
  }
  default_rsparam = duplicate_int (7, tmp_rsparam);
  if (default_csparam)
  {
    g_free (default_csparam);
    default_csparam = NULL;
  }
  default_csparam = duplicate_int (7, tmp_csparam);
  if (default_opengl)
  {
    g_free (default_opengl);
    default_opengl = NULL;
  }
  default_opengl = duplicate_int (5, tmp_opengl);
  duplicate_material (& default_material, & tmp_material);
  default_lightning.lights = tmp_lightning.lights;
  default_lightning.spot = copy_light_sources (tmp_lightning.lights, tmp_lightning.lights, tmp_lightning.spot);
  duplicate_fog (& default_fog, & tmp_fog);
}

/*!
  \fn G_MODULE_EXPORT void restore_all_defaults (GtkButton * but, gpointer data)

  \brief restore all default parameters

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void restore_defaults_parameters (GtkButton * but, gpointer data)
{
  int i;
  // Analysis preferences

  default_num_delta[GR] = 1000;
  default_num_delta[SQ] = 1000;
  default_num_delta[SK] = 1000;
  default_num_delta[GK] = 1000;
  default_num_delta[BD] = 100;
  default_num_delta[AN] = 90;
  default_num_delta[CH-1] = 20;

  default_rsparam[0] = -1;
  default_rsparam[1] = 0;
  default_rsparam[2] = 10;
  default_rsparam[3] = 500;
  default_rsparam[4] = 0;
  default_rsparam[5] = 0;
  default_rsparam[6] = 0;

  default_csparam[0] = 0;
  default_csparam[1] = 10;
  default_csparam[2] = 500;
  default_csparam[3] = 0;
  default_csparam[4] = 0;
  default_csparam[5] = 0;

  for (i=0; i<3; i++) default_opengl[i] = 0;
  default_opengl[3] = QUALITY;

  // Material
  default_material.predefine = 4; // Plastic
  default_material.albedo = vec3(0.5, 0.5, 0.5);
  default_material.param[0] = DEFAULT_LIGHTNING;
  default_material.param[1] = DEFAULT_METALLIC;
  default_material.param[2] = DEFAULT_ROUGHNESS;
  default_material.param[3] = DEFAULT_AMBIANT_OCCLUSION;
  default_material.param[4] = DEFAULT_GAMMA_CORRECTION;
  default_material.param[5] = DEFAULT_OPACITY;

  // Lights
  default_lightning.lights = 3;
  default_lightning.spot = g_malloc0 (3*sizeof*default_lightning.spot);
  default_lightning.spot[0] = init_light_source (0, 1.0, 1.0);
  default_lightning.spot[1] = init_light_source (1, 1.0, 1.0);
  default_lightning.spot[2] = init_light_source (1, 1.0, 1.0);

  if (preference_notebook)
  {
    GtkWidget * tab;
    prepare_tmp_default ();
    for (i=4; i>0; i--)
    {
     tab = gtk_notebook_get_nth_page (GTK_NOTEBOOK (preference_notebook), i);
     destroy_this_widget (tab);
    }
    gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), calc_preferences(), gtk_label_new ("Analysis"));
    gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), opengl_preferences(), gtk_label_new ("OpenGL"));
    gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), model_preferences(), gtk_label_new ("Model"));
    gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), view_preferences(), gtk_label_new ("View"));
    gtk_notebook_set_current_page (GTK_NOTEBOOK(preference_notebook), 0);
    show_the_widgets (preference_notebook);
  }
}

/*!
  \fn G_MODULE_EXPORT void edit_preferences (GtkDialog * edit_prefs, gint response_id, gpointer data)

  \brief edit preferences - running the dialog

  \param edit_prefs the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void edit_preferences (GtkDialog * edit_prefs, gint response_id, gpointer data)
{
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      save_preferences ();
      gchar * str = g_strdup_printf ("Saving <b>atomes</b> preferences in:\n\n\t%s\n\nIf found this file is processed at every <b>atomes</b> startup.\n\n\t\t\t\t\t\tSave file ?", ATOMES_CONFIG);
      if (ask_yes_no("Save atomes preferences ?", str, GTK_MESSAGE_QUESTION, (GtkWidget *)edit_prefs))
      {
        save_preferences_to_xml_file ();
      }
      g_free (str);
      break;
    default:
      break;
  }
  destroy_this_dialog (edit_prefs);
  preferences = FALSE;
  clean_all_tmp ();
  preference_notebook = NULL;
}

extern void update_light_data (int li, opengl_edition * ogl_win);
/*!
  \fn void create_configuration_dialog ()

  \brief create the configuration window
*/
void create_user_preferences_dialog ()
{
  /*
    - delta t for dynamics + unit
    - Steps between configurations
  */

  // Model prefs

  /*
  - Atoms / Bonds, etc
  -
  - Default show clones
  - Default show box and box option
  */

  // Representation prefs

  /* Ortho / persp

  */
  GtkWidget * win = dialog_cancel_apply ("User preferences", MainWindow, TRUE);
  preferences = TRUE;
  prepare_tmp_default ();
  GtkWidget * vbox = dialog_get_content_area (win);
  gtk_widget_set_size_request (win, 625, 645);
  gtk_window_set_resizable (GTK_WINDOW (win), FALSE);
  preference_notebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(preference_notebook), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(preference_notebook), GTK_POS_LEFT);
  gtk_widget_set_size_request (preference_notebook, 600, 635);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, preference_notebook, FALSE, FALSE, 0);
  GtkWidget * gbox = create_vbox (BSEP);
  gchar * mess = "Browse the following to modify the default configuration of <b>atomes</b>\n"
                 "by replacing internal parameters by user defined preferences.\n\n"
                 "\t<b>Analysis</b>\t: calculation preferences\n"
                 "\t<b>OpenGL  </b>\t: rendering preferences\n"
                 "\t<b>Model   </b>\t: atom(s), bond(s) and box preferences\n"
                 "\t<b>View      </b>\t: representation and projection preferences\n\n"
                 "Default parameters are used for any new project added to the workspace\n";
  add_box_child_start (GTK_ORIENTATION_VERTICAL, gbox, markup_label (mess, -1, -1, 0.5, 0.5), FALSE, FALSE, 20);
  GtkWidget * hbox = create_hbox (BSEP);
  GtkWidget * but = create_button (NULL, IMG_NONE, NULL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(restore_defaults_parameters), NULL);
  GtkWidget * but_lab = markup_label ("Restore <b>atomes</b> default parameters", -1, -1, 0.5, 0.5);
  add_container_child (CONTAINER_BUT, but, but_lab);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 20);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, gbox, hbox, FALSE, FALSE, 0);
  gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), gbox, gtk_label_new ("General"));
  gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), calc_preferences(), gtk_label_new ("Analysis"));
  gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), opengl_preferences(), gtk_label_new ("OpenGL"));
  gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), model_preferences(), gtk_label_new ("Model"));
  gtk_notebook_append_page (GTK_NOTEBOOK(preference_notebook), view_preferences(), gtk_label_new ("View"));
  show_the_widgets (preference_notebook);
  gtk_notebook_set_current_page (GTK_NOTEBOOK(preference_notebook), 0);
  run_this_gtk_dialog (win, G_CALLBACK(edit_preferences), NULL);
}
