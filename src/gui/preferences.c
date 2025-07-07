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
#include "bind.h"
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
extern gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif);
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
extern GtkWidget * adv_box (GtkWidget * box, char * lab, int vspace, int size, float xalign);
extern float mat_min_max[5][2];
extern gchar * ogl_settings[3][10];

GtkWidget * atom_entry_over[8];
GtkWidget * bond_entry_over[6];
GtkWidget * preference_notebook = NULL;

int * default_num_delta = NULL;   /*!< Number of x points: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = sp \n 7 = msd */
int * tmp_num_delta = NULL;
double * default_delta_t = NULL;  /*!< 0 = time step, \n 1 = time unit , in: fs, ps, ns, µs, ms */
double * tmp_delta_t = NULL;

int * default_rsparam = NULL;     /*!< Ring statistics parameters: \n
                                       0 = Default search, \n
                                       1 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                       2 = Maximum size of ring for the search Rmax, \n
                                       3 = Maximum number of ring(s) per MD step NUMA, \n
                                       4 = Search only for ABAB rings or not, \n
                                       5 = Include Homopolar bond(s) in the analysis or not, \n
                                       6 = Include homopolar bond(s) when calculating the distance matrix */
int * tmp_rsparam = NULL;
int * default_csparam = NULL;     /*!< Chain statistics parameters: \n
                                       0 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                       1 = Maximum size for a chain Cmax, \n
                                       2 = Maximum number of chain(s) per MD step CNUMA, \n
                                       3 = Search only for AAAA chains or not, \n
                                       4 = Search only for ABAB chains or not, \n
                                       5 = Include Homopolar bond(s) in the analysis or not, \n
                                       6 = Search only for 1-(2)n-1 chains */
int * tmp_csparam = NULL;

/*! \typedef element_radius

  \brief element radius data structure
*/
typedef struct element_radius element_radius;
struct element_radius
{
  int Z;            /*!< Atomic number */
  double rad;       /*!< Assiociated radius */
  element_radius * next;
  element_radius * prev;
};

// 5+3 styles + 5+3 cloned styles
element_radius * default_atomic_rad[16];
element_radius * tmp_atomic_rad[16];
// 3 styles + 3 cloned styles
element_radius * default_bond_rad[6];
element_radius * tmp_bond_rad[6];
int radius_id;

gchar * default_ogl_leg[5] = {"Default style", "Atom(s) color map", "Polyhedra color map", "Quality", "Number of light sources"};
int * default_opengl = NULL;
int * tmp_opengl = NULL;
Material default_material;
Material tmp_material;
Lightning default_lightning;
Lightning tmp_lightning;
Fog default_fog;
Fog tmp_fog;

// Model
gboolean default_clones;
gboolean tmp_clones;
gboolean default_cell;
gboolean tmp_cell;
gboolean * default_o_at_rs;
gboolean * tmp_o_at_rs;
double * default_at_rs;
double * tmp_at_rs;
gboolean * default_o_bd_rw;
gboolean * tmp_o_bd_rw;
double * default_bd_rw;
double * tmp_bd_rw;

gboolean preferences = FALSE;
opengl_edition * pref_ogl_edit = NULL;

gchar * xml_style_leg[6] = {"ball_and_stick", "wireframes", "spacefilled", "spheres", "cylinders", "dots"};
gchar * xml_filled_leg[4] = {"covalent", "ionic", "van-der-waals", "crystal"};
gchar * xml_atom_leg[3] = {"atoms_radius", "dot_size", "sphere_radius"};
gchar * xml_bond_leg[3] = {"bond_radius", "wireframe_width", "cylinders_radius"};

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
  \fn int xml_save_parameter_to_file (xmlTextWriterPtr writer, gchar * xml_leg, gchar * xml_key, gboolean doid, int xml_id, gchar * value)

  \brief save single parameter to XML file

  \param writer the XML writer to update
  \param xml_leg information
  \param xml_key key
  \param doid add id attribute to parameter (1/0)
  \param xml_id value for id
  \param value parameter to write
*/
int xml_save_parameter_to_file (xmlTextWriterPtr writer, gchar * xml_leg, gchar * xml_key, gboolean doid, int xml_id, gchar * value)
{
  int rc;
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"parameter");
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"info", BAD_CAST xml_leg);
  if (rc < 0) return 0;
  rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"key", BAD_CAST xml_key);
  if (rc < 0) return 0;
  if (doid)
  {
    gchar * str;
    str = g_strdup_printf ("%d", xml_id);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST (const xmlChar *)str);
    g_free (str);
    if (rc < 0) return 0;
  }
  rc = xmlTextWriterWriteFormatString (writer, "%s", value);
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

  gchar * xml_delta_num_leg[8] = {"g(r): number of δr", "s(q): number of δq", "s(k): number of δk", "g(r) FFT: number of δr",
                                  "Dij: number of δr [min(Dij)-max(Dij)]", "Angles distribution: number of δθ [0-180°]",
                                  "Spherical harmonics: l(max) in [2-40]", "MSD: steps between configurations"};
  gchar * xml_delta_t_leg[2] = {"MSD: time steps δt", "MSD: time unit"};
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
  gchar * xml_fog_leg[5] = {"Mode",
                            "Type",
                            "Density",
                            "Depth",
                            "Color"};
  gchar * xml_model_leg[2] = {"Show clones",
                              "Show box"};

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
  int i, j, k, l, m, n, o;
  gchar * str;

  for (i=0; i<8; i++)
  {
    str = g_strdup_printf ("%d",  default_num_delta[i]);
    rc = xml_save_parameter_to_file (writer, xml_delta_num_leg[i], "default_num_delta", TRUE, i, str);
    g_free (str);
    if (! rc) return 0;
  }
  // Delta t
  for (i=0; i<2; i++)
  {
    str = g_strdup_printf ("%f", default_delta_t[i]);
    rc = xml_save_parameter_to_file (writer, xml_delta_t_leg[i], "default_delta_t", TRUE, i, str);
    g_free (str);
    if (! rc) return 0;
  }
  // Rings
  for (i=0; i<7; i++)
  {
    str = g_strdup_printf ("%d", default_rsparam[i]);
    rc = xml_save_parameter_to_file (writer,  xml_rings_leg[i], "default_rsparam", TRUE, i, str);
    g_free (str);
    if (! rc) return 0;
  }
  // Chains
  for (i=0; i<7; i++)
  {
    str = g_strdup_printf ("%d", default_csparam[i]);
    rc = xml_save_parameter_to_file (writer,  xml_chain_leg[i], "default_csparam", TRUE, i, str);
    g_free (str);
    if (! rc) return 0;
  }

  // End analysis
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"opengl");
  if (rc < 0) return 0;
  for (i=0; i<4; i++)
  {

    str = g_strdup_printf ("%d", default_opengl[i]);
    rc = xml_save_parameter_to_file (writer,  xml_opengl_leg[i], "default_opengl", TRUE, i, str);
    g_free (str);
    if (! rc) return 0;
  }

  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"material");
  if (rc < 0) return 0;

  str = g_strdup_printf ("%d", default_material.predefine);
  rc = xml_save_parameter_to_file (writer,  xml_material_leg[0], "default_material", TRUE, -1, str);
  g_free (str);
  if (! rc) return 0;

  for (i=0; i<6; i++)
  {
    str = g_strdup_printf ("%f", default_material.param[i]);
    rc = xml_save_parameter_to_file (writer, xml_material_leg[i+1], "default_material", TRUE, i, str);
    g_free (str);
    if (! rc) return 0;
  }

  rc = xml_save_xyz_to_file (writer, 6, xml_material_leg[7], "default_material", default_material.albedo);
  if (! rc) return rc;

  // End material
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"lightning");
  if (rc < 0) return 0;

  str = g_strdup_printf ("%d", default_lightning.lights);
  rc = xml_save_parameter_to_file (writer, "Number of lights", "default_lightning", TRUE, 0, str);
  g_free (str);
  if (! rc) return 0;

  for (i=0; i<default_lightning.lights; i++)
  {
    rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"light");
    if (rc < 0) return 0;
    str = g_strdup_printf ("%d", i);
    rc = xmlTextWriterWriteAttribute (writer, BAD_CAST (const xmlChar *)"id", BAD_CAST (const xmlChar *)str);
    g_free (str);
    if (rc < 0) return 0;

    str = g_strdup_printf ("%d", default_lightning.spot[i].type);
    rc = xml_save_parameter_to_file (writer, xml_lightning_leg[0], "light.type", FALSE, 0, str);
    g_free (str);
    if (! rc) return 0;
    str = g_strdup_printf ("%d", default_lightning.spot[i].fix);
    rc = xml_save_parameter_to_file (writer, xml_lightning_leg[1], "light.fix", FALSE, 0, str);
    g_free (str);
    if (! rc) return 0;

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
  str = g_strdup_printf ("%d", default_fog.mode);
  rc = xml_save_parameter_to_file (writer, xml_fog_leg[0], "fog.mode", TRUE, 0, str);
  g_free (str);
  if (! rc) return 0;
  // Type
  str = g_strdup_printf ("%d", default_fog.based);
  rc = xml_save_parameter_to_file (writer, xml_fog_leg[1], "fog.type", TRUE, 1, str);
  g_free (str);
  if (! rc) return 0;
  // Density
  str = g_strdup_printf ("%f", default_fog.density);
  rc = xml_save_parameter_to_file (writer, xml_fog_leg[2], "fog.density", TRUE, 2, str);
  g_free (str);
  if (! rc) return 0;
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


  // Model
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"model");
  if (rc < 0) return 0;

  if (default_clones)
  {
    str = g_strdup_printf ("%d", default_clones);
    rc = xml_save_parameter_to_file (writer, xml_model_leg[0], "default_clones", FALSE, 0, str);
    g_free (str);
    if (! rc) return 0;
  }
  if (! default_cell)
  {
    str = g_strdup_printf ("%d", default_cell);
    rc = xml_save_parameter_to_file (writer, xml_model_leg[1], "default_cell", FALSE, 0, str);
    g_free (str);
    if (! rc) return 0;
  }
  rc = xmlTextWriterStartElement (writer, BAD_CAST (const xmlChar *)"atoms_and_bonds");
  if (rc < 0) return 0;

  element_radius * tmp_rad;
  gboolean do_atoms;
  gboolean do_bonds;
  gchar * str_a, * str_b;
  for (i=0; i<OGL_STYLES; i++)
  {
    do_atoms = do_bonds = FALSE;
    if (i != 4)
    {
      j = (i < 4) ? i : 4;
      if (default_o_at_rs[j] || default_o_at_rs[j+5]) do_atoms = TRUE;
      if (default_atomic_rad[j] || default_atomic_rad[j+5]) do_atoms = TRUE;
      if (! do_atoms)
      {
        if (j == 2)
        {
          for (k=0; k<3; k++)
          {
            if (default_atomic_rad[10+k] || default_atomic_rad[12+k]) do_atoms = TRUE;
          }
        }
      }
    }
    if (i != 2 && i != 3 && i != 5)
    {
      j = (j < 2) ? j : 2;
      if (default_o_bd_rw[j] || default_o_bd_rw[j+3]) do_bonds = TRUE;
      if (default_bond_rad[j] || default_bond_rad[j+3]) do_bonds = TRUE;
    }
    if (do_atoms || do_bonds)
    {
      rc = xmlTextWriterStartElement (writer, BAD_CAST xml_style_leg[i]);
      if (rc < 0) return 0;
      if (do_atoms)
      {
        j = (i < 4) ? i : 4;
        k = (j == 0 || j == 2) ? 0 : (j == 1 || j == 4) ? 1 : 2;
        for (l=0; l<2; l++)
        {
          if (default_o_at_rs[j+l*5])
          {
            str_a = (l) ? g_strdup_printf ("clone-%s", xml_atom_leg[k]) : g_strdup_printf ("%s", xml_atom_leg[k]);
            str_b = g_strdup_printf ("%f", default_at_rs[j+l*5]);
            rc = xml_save_parameter_to_file (writer, str_a, "default_at_rs", TRUE, j+l*5, str_b);
            g_free (str_a);
            g_free (str_b);
            if (! rc) return 0;
          }
        }
        l = (j == 2) ? 4 : 1;
        for (m=0; m < l;  m++)
        {
          n = (m) ? 7 : 0;
          for (o= 0; o<2; o++)
          {
            n = (o && m) ? 5 : n;
            if (default_atomic_rad[j+n+5*o+m])
            {
              str_a = (o) ? g_strdup_printf ("clone-%s", (j == 2) ? xml_filled_leg[l] : xml_atom_leg[k]) : g_strdup_printf ("%s", (j == 2) ? xml_filled_leg[l] : xml_atom_leg[k]);
              rc = xmlTextWriterStartElement (writer, BAD_CAST str_a);
              g_free (str_a);
              if (rc < 0) return 0;
              tmp_rad = default_atomic_rad[j+n+5*o+m];
              while (tmp_rad)
              {
                str = g_strdup_printf ("%f", tmp_rad -> rad);
                rc = xml_save_parameter_to_file (writer, periodic_table_info[tmp_rad -> Z].lab, "default_atomic_rad", TRUE, tmp_rad -> Z, str);
                g_free (str);
                if (! rc) return 0;
                tmp_rad = tmp_rad -> next;
              }
              rc = xmlTextWriterEndElement (writer);
              if (rc < 0) return 0;
            }
          }
        }
      }
      if (do_bonds)
      {
        j = (j < 2) ? j : 2;
        for (k=0; k<2; k++)
        {
          if (default_o_bd_rw[j+k*3])
          {
            str_a = (k) ? g_strdup_printf ("clone-%s", xml_atom_leg[k]) : g_strdup_printf ("%s", xml_bond_leg[k]);
            str_b = g_strdup_printf ("%f", default_bd_rw[j+k*3]);
            rc = xml_save_parameter_to_file (writer, str_a, "default_bd_rw", TRUE, j+k*3, str_b);
            g_free (str_a);
            g_free (str_b);
            if (! rc) return 0;
          }
          if (default_bond_rad[j+k*3])
          {
            str_a = (k) ? g_strdup_printf ("clone-%s", (j == 2) ? xml_filled_leg[k] : xml_bond_leg[j]) : g_strdup_printf ("%s", (j == 2) ? xml_filled_leg[k] : xml_bond_leg[j]);
            rc = xmlTextWriterStartElement (writer, BAD_CAST str_a);
            g_free (str_a);
            if (rc < 0) return 0;
            tmp_rad = default_bond_rad[j+k*3];
            while (tmp_rad)
            {
              str = g_strdup_printf ("%f", tmp_rad -> rad);
              rc = xml_save_parameter_to_file (writer, periodic_table_info[tmp_rad -> Z].lab, "default_bond_rad", TRUE, tmp_rad -> Z, str);
              g_free (str);
              if (! rc) return 0;
              tmp_rad = tmp_rad -> next;
            }
            rc = xmlTextWriterEndElement (writer);
            if (rc < 0) return 0;
          }
        }
      }
      rc = xmlTextWriterEndElement (writer);
      if (rc < 0) return 0;
    }
  }

  // End atoms_and_bonds
  rc = xmlTextWriterEndElement (writer);
  if (rc < 0) return 0;

  // End model
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
  \fn void set_parameter (double value, gchar * key, int vid, vec3_t * vect, float start, float end)

  \brief set default parameter

  \param value the value to set
  \param key the name of variable to set
  \param vid the id number to set
  \param vect vector to set, if any
  \param start initial value, if any, -1.0 otherwise
  \param end final value, if any, -1.0 otherwise

*/
void set_parameter (double value, gchar * key, int vid, vec3_t * vect, float start, float end)
{
  element_radius * tmp_rad;
  if (g_strcmp0(key, "default_num_delta") == 0)
  {
    default_num_delta[vid] = (int)value;
  }
  else if (g_strcmp0(key, "default_delta_t") == 0)
  {
    default_delta_t[vid] = value;
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
    else if (vid == 6 && vect)
    {
      default_material.albedo = * vect;
    }
    else
    {
      default_material.param[vid] = value;
    }
  }
  else if (g_strcmp0(key, "default_lightning") == 0)
  {
    default_lightning.lights = (int)value;
  }
  else if (g_strcmp0(key, "light.type") == 0)
  {
    default_lightning.spot[vid].type = (int)value;
  }
  else if (g_strcmp0(key, "light.fix") == 0)
  {
    default_lightning.spot[vid].fix = (int)value;
  }
  else if (g_strcmp0(key, "light.direction") == 0 && vect)
  {
    default_lightning.spot[vid].direction = * vect;
  }
  else if (g_strcmp0(key, "light.position") == 0 && vect)
  {
    default_lightning.spot[vid].position = * vect;
  }
  else if (g_strcmp0(key, "light.intensity") == 0 && vect)
  {
    default_lightning.spot[vid].intensity = * vect;
  }
  else if (g_strcmp0(key, "light.attenuation") == 0 && vect)
  {
    default_lightning.spot[vid].attenuation = * vect;
  }
  else if (g_strcmp0(key, "light.spot") == 0 && vect)
  {
    default_lightning.spot[vid].spot_data = * vect;
  }
  else if (g_strcmp0(key, "fog.mode") == 0)
  {
    default_fog.mode = (int)value;
  }
  else if (g_strcmp0(key, "fog.type") == 0)
  {
    default_fog.based = (int)value;
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
  else if (g_strcmp0(key, "default_cell") == 0)
  {
    default_cell = (int)value;
  }
  else if (g_strcmp0(key, "default_clones") == 0)
  {
    default_clones = (int)value;
  }
  else if (g_strcmp0(key, "default_at_rs") == 0)
  {
    default_o_at_rs[vid] = TRUE;
    default_at_rs[vid] = value;
  }
  else if (g_strcmp0(key, "default_bd_rw") == 0)
  {
    default_o_bd_rw[vid] = TRUE;
    default_bd_rw[vid] = value;
  }
  else if (g_strcmp0(key, "default_atomic_rad") == 0)
  {
    if (default_atomic_rad[radius_id])
    {
      tmp_rad = default_atomic_rad[radius_id];
      while (tmp_rad -> next)
      {
        tmp_rad = tmp_rad -> next;
      }
      tmp_rad -> next = g_malloc0(sizeof*tmp_rad);
      tmp_rad = tmp_rad -> next;
    }
    else
    {
      default_atomic_rad[radius_id] = g_malloc0(sizeof*default_atomic_rad[radius_id]);
      tmp_rad = default_atomic_rad[radius_id];
    }
    tmp_rad -> Z = vid;
    tmp_rad -> rad = value;
  }
  else if (g_strcmp0(key, "default_bond_rad") == 0)
  {
    if (default_bond_rad[radius_id])
    {
      tmp_rad = default_bond_rad[radius_id];
      while (tmp_rad -> next)
      {
        tmp_rad = tmp_rad -> next;
      }
      tmp_rad -> next = g_malloc0(sizeof*tmp_rad);
      tmp_rad = tmp_rad -> next;
    }
    else
    {
      default_bond_rad[radius_id] = g_malloc0(sizeof*default_bond_rad[radius_id]);
      tmp_rad = default_bond_rad[radius_id];
    }
    tmp_rad -> Z = vid;
    tmp_rad -> rad = value;
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
  gchar * content;
  int id;
  float start, end;
  double value;
  vec3_t vec;
  while (parameter_node)
  {
    content = g_strdup_printf ("%s", xmlNodeGetContent(parameter_node));
    value = (g_strcmp0(content, "") == 0) ? 0.0 : string_to_double ((gpointer)content);
    p_details = parameter_node -> properties;
    set_codevar = set_id = FALSE;
    set_x = set_y = set_z = FALSE;
    start = end = -1.0;
    while (p_details)
    {
      p_node = p_details -> children;
      if (p_node)
      {
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
  \fn void read_light (xmlNodePtr light_node)

  \brief read light preferences from XML configuration

  \param light_node
*/
void read_light (xmlNodePtr light_node)
{
  xmlNodePtr l_node, p_node;
  xmlNodePtr parameter_node;
  xmlAttrPtr l_details, p_details;
  gchar * key;
  int lid;
  gboolean set_codevar;
  gboolean set_lid = FALSE;
  gboolean set_x, set_y, set_z;
  gchar * content;
  double value;
  vec3_t vec;
  l_details = light_node -> properties;
  while (l_details)
  {
    l_node = l_details -> children;
    if (l_node)
    {
      if (g_strcmp0("id",(char *)l_details -> name) == 0)
      {
        lid = (int) string_to_double ((gpointer)xmlNodeGetContent(l_node));
        set_lid = TRUE;
      }
    }
    l_details = l_details -> next;
  }
  if (set_lid)
  {
    parameter_node = findnode (light_node -> children, "parameter");
    set_codevar = FALSE;
    set_x = set_y = set_z = FALSE;
    while (parameter_node)
    {
      content = g_strdup_printf ("%s", xmlNodeGetContent(parameter_node));
      value = (g_strcmp0(content, "") == 0) ? 0.0 : string_to_double ((gpointer)content);
      p_details = parameter_node -> properties;
      while (p_details)
      {
        p_node = p_details -> children;
        if (p_node)
        {
          if (g_strcmp0("key",(char *)p_details -> name) == 0)
          {
            key = g_strdup_printf ("%s", xmlNodeGetContent(p_node));
            set_codevar = TRUE;
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
        }
        p_details = p_details -> next;
      }
      if (set_codevar)
      {
        set_parameter (value, key, lid, (set_x && set_y && set_z) ? & vec : NULL, -1.0, -1.0);
      }
      parameter_node = parameter_node -> next;
      parameter_node = findnode (parameter_node, "parameter");
    }
  }
}

/*!
  \fn void read_preferences (xmlNodePtr preference_node)

  \brief read preferences from XML configuration

  \param preference_node node the XML node that point to preferences
*/
void read_preferences (xmlNodePtr preference_node)
{
  xmlNodePtr node;
  node = findnode (preference_node  -> children, "parameter");
  read_parameter (node);
}

/*!
  \fn void read_style_from_xml_file ()

  \brief read style preferences from XML file
*/
void read_style_from_xml_file (xmlNodePtr style_node, int style)
{
  xmlNodePtr node;
  int i;
  // node = findnode (style_node -> children, xml_atom_leg);
  if (node)
  {

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
  xmlNodePtr node, p_node, l_node;
  const xmlChar aml[22]="atomes_preferences-xml";
  // int i;
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
          p_node = findnode (node  -> children, "material");
          if (p_node)
          {
            read_preferences (p_node);
          }
          p_node = findnode (node  -> children, "lightning");
          if (p_node)
          {
            read_preferences (p_node);
            l_node = findnode (p_node -> children, "light");
            while (l_node)
            {
              read_light (l_node);
              l_node = l_node -> next;
              l_node = findnode (l_node, "light");
            }
          }
          p_node = findnode (node  -> children, "fog");
          if (p_node)
          {
            read_preferences(p_node);
          }
          read_preferences (node);
        }
        node = findnode(racine -> children, "model");
        if (node)
        {
          read_preferences (node);
          p_node = findnode(node -> children, "atoms_and_bonds");
          if (p_node)
          {
            for (i=0; i<OGL_STYLES; i++)
            {
              l_node = findnode (p_node -> children, xml_style_leg[i]);
              if (l_node)
              {
                read_style_from_xml_file (l_node, i);
              }
            }
          }
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
  default_num_delta = allocint (8);
  default_delta_t = allocdouble (2);
  default_rsparam = allocint (7);
  default_csparam = allocint (7);
  default_opengl = allocint (5);
  default_at_rs = allocdouble (10);
  default_o_at_rs = allocbool (10);
  default_bd_rw = allocdouble (6);
  default_o_bd_rw = allocbool (6);
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
  \fn GtkWidget * pref_list (gchar * mess[2], int nelem, gchar * mlist[nelem][2], gchar * end)

  \brief print information message with item list of elements

  \param mess main information message
  \param nelem number of elements in the list
  \param mlsit item list
  \param end end message, if any
*/
GtkWidget * pref_list (gchar * mess[2], int nelem, gchar * mlist[nelem][2], gchar * end)
{
  gchar * str;
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * vvbox = create_vbox (BSEP);
  GtkWidget * hbox;
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, markup_label(mess[0], -1, -1, 0.5, 0.5), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, markup_label(mess[1], -1, -1, 0.5, 0.5), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, vvbox, FALSE, FALSE, 20);
  int i;
  for (i=0; i<nelem; i++)
  {
    hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(" ", 60, -1, 0.0, 0.5), FALSE, FALSE, 0);
    str = g_strdup_printf ("<b>%s</b>", mlist[i][0]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 120, -1, 0.0, 0.5), FALSE, FALSE, 5);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(":", -1, -1, 1.0, 0.5), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(mlist[i][1], -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  }
  if (end)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(end, -1, -1, 0.5, 0.5), FALSE, FALSE, 20);
  }
  return vbox;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggled_default_stuff (GtkCheckButton * but, gpointer data)

  \brief toggle set / unset default callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_default_stuff (GtkCheckButton * but, gpointer data)
{
  int status = gtk_check_button_get_active (but);
#else
/*!
  \fn G_MODULE_EXPORT void toggled_default_stuff (GtkToggleButton * but, gpointer data)

  \brief toggle set / unset default callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_default_stuff (GtkToggleButton * but, gpointer data)
{
  int status = gtk_toggle_button_get_active (but);
#endif
  int object = GPOINTER_TO_INT(data);
  switch (object)
  {
    case 0:
      tmp_clones = status;
      break;
    case 1:
      tmp_cell = status;
      break;
    default:
      if (object < 0)
      {
        // Bonds
        tmp_o_bd_rw[-object-2] = status;
        widget_set_sensitive(bond_entry_over[-object-2], status);
      }
      else
      {
        tmp_o_at_rs[object-2] = status;
        widget_set_sensitive(atom_entry_over[object-2], status);
      }
      break;
  }
}

/*!
  \fn G_MODULE_EXPORT void set_default_stuff (GtkEntry * res, gpointer data)

  \brief update default number of delta preferences

  \param res the GtkEntry the signal is coming from
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_default_stuff (GtkEntry * res, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double value = string_to_double ((gpointer)m);
  if (i < 0)
  {
    // Bonds
    tmp_bd_rw[-i-2] = value;
  }
  else
  {
    tmp_at_rs[i-2] = value;
  }
  update_entry_double (res, value);
}

int the_object;
element_radius ** edit_list;
GtkWidget * pref_tree;
gboolean user_defined;

/*!
  \fn element_radius * duplicate_element_radius (element_radius * old_list)

  \brief duplicate an element radius data structure

  \param old_list the data structure to duplicate
*/
element_radius * duplicate_element_radius (element_radius * old_list)
{
  if (! old_list) return NULL;
  element_radius * new_list = g_malloc0(sizeof*new_list);
  element_radius * tmp_rad, * tmp_rbd;
  tmp_rad = old_list;
  tmp_rbd = new_list;
  while (tmp_rad)
  {
    tmp_rbd -> Z = tmp_rad -> Z;
    tmp_rbd -> rad = tmp_rad -> rad;
    if (tmp_rad -> next)
    {
      tmp_rbd -> next = g_malloc0(sizeof*tmp_rbd -> next);
      tmp_rbd -> next -> prev = tmp_rbd;
      tmp_rbd = tmp_rbd -> next;
    }
    tmp_rad = tmp_rad -> next;
  }
  return new_list;
}

/*!
  \fn float get_radius (int object, int col, int z, element_radius * rad_list)

  \brief retrieve the radius/width of a species depending on style

  \param object the object to look at
  \param modifier for tabulated radii
  \param z atomic number
  \param rad_list pre allocated data, if any

*/
float get_radius (int object, int col, int z, element_radius * rad_list)
{
  element_radius * tmp_rad = rad_list;
  int ft;
  while (tmp_rad)
  {
    if (tmp_rad -> Z == z)
    {
      user_defined = TRUE;
      return tmp_rad -> rad;
    }
    tmp_rad = tmp_rad -> next;
  }
  if (object < 0)
  {
    object = - object - 2;
    // Bonds
    if (object == 0 || object == 3)
    {
      if (z < 119)
      {
        ft = 0;
        return set_radius_ (& z, & ft) / 2.0;
      }
    }
    else
    {
      return DEFAULT_SIZE;
    }
  }
  else
  {
    object -= 2;
    if (object == 2 || object == 7 || col)
    {
      ft = col;
      if (z < 119)
      {
        return set_radius_ (& z, & ft);
      }
    }
    else if (object == 1 || object == 4 || object == 6 || object == 9)
    {
      // Dots
      return DEFAULT_SIZE;
    }
    else if (object == 0 || object == 3 || object == 5 ||  object == 8)
    {
      ft = 0;
      return set_radius_ (& z, & ft);
    }
  }
  return 0.0;
}

/*!
  \fn G_MODULE_EXPORT void edit_pref (GtkCellRendererText * cell, gchar * path_string, gchar * new_text, gpointer user_data)

  \brief edit cell in the preferences tree model

  \param cell the GtkCellRendererText sending the signal
  \param path_string the path in the tree model
  \param new_text the string describing the new value
  \param user_data the associated data pointer
*/
G_MODULE_EXPORT void edit_pref (GtkCellRendererText * cell, gchar * path_string, gchar * new_text, gpointer user_data)
{
  int col = GPOINTER_TO_INT(user_data);
  GtkTreeModel * pref_model = gtk_tree_view_get_model(GTK_TREE_VIEW(pref_tree));
  GtkTreeIter row;
  gtk_tree_model_get_iter_from_string (pref_model, & row, path_string);
  int z;
  float val = string_to_double ((gpointer)new_text);
  gtk_tree_model_get (pref_model, & row, 3, & z, -1);
  gtk_list_store_set (GTK_LIST_STORE(pref_model), & row, col+4, val, -1);
  gboolean add_elem = FALSE;
  gboolean remove_elem = FALSE;
  element_radius * tmp_list;
  int col_val[4] = {1, 3, 5, 10};
  float v = get_radius (the_object, col, z, NULL);
  if (edit_list[col])
  {
    tmp_list = edit_list[col];
    add_elem = TRUE;
    while (tmp_list)
    {
      if (tmp_list -> Z == z)
      {
        tmp_list -> rad = val;
        if (val == v)
        {
          remove_elem = TRUE;
          if (tmp_list -> next)
          {
            if (tmp_list -> prev)
            {
              tmp_list -> prev -> next = tmp_list -> next;
              tmp_list -> next -> prev = tmp_list -> prev;
            }
            else
            {
              edit_list[col] = tmp_list -> next;
              edit_list[col] -> prev = NULL;
            }
          }
          else if (tmp_list -> prev)
          {
            edit_list[col] -> rad = tmp_list -> rad;
            edit_list[col] -> Z = tmp_list -> Z;
            edit_list[col] -> prev = NULL;
          }
          else
          {
            g_free (edit_list[col]);
            edit_list[col] = NULL;
          }
          add_elem = FALSE;
        }
        else
        {
          tmp_list -> rad = v;
          add_elem = FALSE;
        }
      }
      tmp_list = tmp_list -> next;
    }
  }
  else if (val != v)
  {
    add_elem = TRUE;
  }

  if (add_elem)
  {
    if (edit_list[col])
    {
      tmp_list = edit_list[col];
      while (tmp_list)
      {
        if (! tmp_list -> next) break;
        tmp_list = tmp_list -> next;
      }
      tmp_list -> next = g_malloc0(sizeof*tmp_list -> next);
      tmp_list -> next -> prev = tmp_list;
      tmp_list = tmp_list -> next;
    }
    else
    {
      edit_list[col] = g_malloc0(sizeof*edit_list[col]);
      tmp_list = edit_list[col];
    }
    tmp_list -> Z = z;
    tmp_list -> rad = val;
    gtk_tree_model_get (pref_model, & row, 0, & z, -1);
    if (! z)
    {
      z = col_val[col];
    }
    else
    {
      int a, b, c, d;
      a = z / 10;
      b = (z - a * 10) / 5;
      c = (z - a * 10 - b * 5) / 3;
      d = z - a * 10 - b * 5 - c * 3;
      z = 0;
      if (a || col == 3) z += 10;
      if (b || col == 2) z += 5;
      if (c || col == 1) z += 3;
      if (d || col == 0) z += 1;
    }
    gtk_list_store_set (GTK_LIST_STORE(pref_model), & row, 0, z, -1);
  }
  else if (remove_elem)
  {
    gtk_tree_model_get (pref_model, & row, 0, & z, -1);
    z -= col_val[col];
    gtk_list_store_set (GTK_LIST_STORE(pref_model), & row, 0, z, -1);
  }
}

/*!
  \fn G_MODULE_EXPORT void edit_chem_preferences (GtkDialog * edit_chem, gint response_id, gpointer data)

  \brief edit chem preferences - running the dialog

  \param edit_prefs the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void edit_chem_preferences (GtkDialog * edit_chem, gint response_id, gpointer data)
{
  int i, j, k, l, m;
  int object = object = GPOINTER_TO_INT (data);
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (object < 0)
      {
        object = - object - 2;
        if (tmp_bond_rad[object]) g_free (tmp_bond_rad[object]);
        tmp_bond_rad[object] = duplicate_element_radius (edit_list[0]);
      }
      else
      {
        j = (object == 4 || object == 9) ? 4 : 1;
        k = (object < 5) ? 0 : 1;
        object = object - 2;
        l = (k) ? 5 : 7;
        for (i=0; i<j; i++)
        {
          m = (i) ? 1 : 0;
          if (tmp_atomic_rad[j-2+m*l+i]) g_free (tmp_atomic_rad[object+m*l+i]);
          tmp_atomic_rad[object+m*l+i] = duplicate_element_radius (edit_list[i]);
        }
      }
      break;
    default:
      j = (object < 0) ? 1 : (object == 4 || object == 9) ? 4 : 1;
      for (i=0; i<j; i++)
      {
        if (edit_list[i])
        {
          g_free (edit_list[i]);
          edit_list[i] = NULL;
        }
      }
      g_free (edit_list);
      edit_list = NULL;
      break;
  }
  destroy_this_dialog (edit_chem);
}

/*!
  \fn void radius_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
void radius_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int cid = GPOINTER_TO_INT (data);
  int vid;
  gtk_tree_model_get (mod, iter, 0, & vid, -1);
  gboolean docol = FALSE;
  switch (vid)
  {
    case 1:
      docol = (cid == 0) ? TRUE : FALSE;
      break;
    case 3:
      docol = (cid == 1) ? TRUE : FALSE;
      break;
    case 5:
      docol = (cid == 2) ? TRUE : FALSE;
      break;
    case 10:
      docol = (cid == 3) ? TRUE : FALSE;
      break;
    case 4:
      docol = (cid == 0 || cid == 1) ? TRUE : FALSE;
      break;
    case 6:
      docol = (cid == 0 || cid == 2) ? TRUE : FALSE;
      break;
    case 8:
      docol = (cid == 1 || cid == 2) ? TRUE : FALSE;
      break;
    case 11:
      docol = (cid == 0 || cid == 3) ? TRUE : FALSE;
      break;
    case 13:
      docol = (cid == 1 || cid == 3) ? TRUE : FALSE;
      break;
    case 15:
      docol = (cid == 2 || cid == 3) ? TRUE : FALSE;
      break;
    case 9:
      docol = (cid != 3) ? TRUE : FALSE;
      break;
    case 14:
      docol = (cid != 2) ? TRUE : FALSE;
      break;
    case 16:
      docol = (cid != 1) ? TRUE : FALSE;
      break;
    case 18:
      docol = (cid != 0) ? TRUE : FALSE;
      break;
    case 19:
      docol = TRUE;
      break;
  }
  set_renderer_color (docol, renderer, init_color (cid, 4));
}

/*!
  \fn G_MODULE_EXPORT void edit_species_parameters (GtkButton * but, gpointer data)

  \brief edit atoms and bonds species related parameters

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void edit_species_parameters (GtkButton * but, gpointer data)
{
  gchar * ats[3]={"atom(s)", "dot(s)", "sphere(s)"};
  gchar * dim[3]={"radius", "size", "width"};
  gchar * bts[3]={"bond(s)", "wireframe", "cylinders"};
  the_object = GPOINTER_TO_INT(data);
  int i, j, k, l, n, m;
  int aid, bid;
  int num_col;
  gchar * str;
  if (the_object < 0)
  {
    // Going for bonds
    aid = - the_object - 2;
    aid = (aid) > 2 ? aid - 3 : aid;
    bid = (the_object == -3 || the_object == -6) ? 2 : 0;
    str = (the_object < -4) ? g_strdup_printf ("Edit cloned %s %s", bts[aid], dim[bid]) : g_strdup_printf ("Edit %s %s", bts[aid], dim[bid]);
    num_col = 5;
  }
  else
  {
    // Going for atoms
    aid = the_object - 2;
    aid = (aid == 0 || aid == 2 || aid == 5 || aid == 7) ? 0 : (aid == 1 || aid == 4 || aid == 6 || aid == 9) ? 1 : 2;
    bid = (the_object == 1 || the_object == 6) ? 1 : 0;
    str = (the_object - 2 > 4) ? g_strdup_printf ("Edit cloned %s %s", ats[aid], dim[bid]) : g_strdup_printf ("Edit %s %s", ats[aid], dim[bid]);
    num_col = (the_object == 4 || the_object == 9) ? 8 : 5;
  }

  edit_list = NULL;
  GtkWidget * win = dialog_cancel_apply (str, MainWindow, TRUE);
  g_free (str);
  gtk_window_set_default_size (GTK_WINDOW(win), (num_col == 8) ? 600 : 300, 600);
  GtkWidget * vbox = dialog_get_content_area (win);
  GType type[8] = {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE};
  GtkTreeViewColumn * pref_col[num_col];
  GtkCellRenderer * pref_cel[num_col];
  GtkTreeSelection * pref_select;
  GtkListStore * pref_model = gtk_list_store_newv (num_col, type);
  GtkTreeIter elem;
  i = (the_object < 0) ? - the_object - 2 : the_object - 2;
  j = (num_col == 8) ? 4 : 1;
  k = ((the_object < 0 && i > 2) || (the_object > 0 && i > 4)) ? 1 : 0;
  edit_list = g_malloc0(j*sizeof*edit_list);
  for (l=0; l<j; l++)
  {
    if (the_object < 0)
    {
      edit_list[l] = duplicate_element_radius (tmp_bond_rad[i]);
    }
    else
    {
      m = (k) ? 5 : 7;
      n = (l) ? 1 : 0;
      edit_list[l] = duplicate_element_radius (tmp_atomic_rad[i+n*m+l]);
    }
  }

  for (i=1; i<120; i++)
  {
    user_defined = FALSE;
    gtk_list_store_append (pref_model, & elem);
    gtk_list_store_set (pref_model, & elem, 0, user_defined,
                        1, periodic_table_info[i].name,
                        2, periodic_table_info[i].lab,
                        3, periodic_table_info[i].Z,
                        4, get_radius (the_object, 0, i, edit_list[0]), -1);
    if (num_col == 8)
    {
      j = user_defined;
      user_defined = FALSE;
      gtk_list_store_set (pref_model, & elem, 5,  get_radius (the_object, 1, i, edit_list[1]), -1);
      j += (user_defined) ? 3 : 0;
      user_defined = FALSE;
      gtk_list_store_set (pref_model, & elem, 6, get_radius (the_object, 2, i, edit_list[2]), -1);
      j += (user_defined) ? 5 : 0;
      user_defined = FALSE;
      gtk_list_store_set (pref_model, & elem, 7, get_radius (the_object, 3, i, edit_list[3]), -1);
      j += (user_defined) ? 10 : 0;
      user_defined = FALSE;
      gtk_list_store_set (pref_model, & elem, 0, j, -1);
    }
  }

  pref_tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL(pref_model));
  gchar * name[3] = {"Element", "Symbol", "Z"};
  gchar * g_name[3] = {"Radius", "Size", "Width"};
  gchar * f_name[4] = {"Covalent [1]","Ionic [2]","van Der Waals [3]", "Crystal [4,5]"};

  for (i=0; i<num_col; i++)
  {
    pref_cel[i] = gtk_cell_renderer_text_new();
    if (i > 3)
    {
      g_object_set (pref_cel[i], "editable", TRUE, NULL);
      gtk_cell_renderer_set_alignment (pref_cel[i], 0.5, 0.5);
      g_signal_connect (G_OBJECT(pref_cel[i]), "edited", G_CALLBACK(edit_pref), GINT_TO_POINTER(i-4));
      pref_col[i] = gtk_tree_view_column_new_with_attributes((num_col) == 8 ? f_name[i-4] : g_name[bid], pref_cel[i], "text", i, NULL);
      gtk_tree_view_column_set_cell_data_func (pref_col[i], pref_cel[i], radius_set_color_and_markup, GINT_TO_POINTER(i-4), NULL);
    }
    else if (i)
    {
      pref_col[i] = gtk_tree_view_column_new_with_attributes(name[i-1], pref_cel[i], "text", i, NULL);
      gtk_tree_view_column_set_alignment (pref_col[i], 0.5);
      gtk_tree_view_column_set_resizable (pref_col[i], TRUE);
      gtk_tree_view_column_set_min_width (pref_col[i], 50);
    }
    else
    {
      pref_col[i] = gtk_tree_view_column_new_with_attributes("", pref_cel[i], "text", i, NULL);
      gtk_tree_view_column_set_visible (pref_col[i], FALSE);
    }
    gtk_tree_view_append_column(GTK_TREE_VIEW(pref_tree), pref_col[i]);
  }
  g_object_unref (pref_model);
  pref_select = gtk_tree_view_get_selection (GTK_TREE_VIEW(pref_tree));
  gtk_tree_selection_set_mode (pref_select, GTK_SELECTION_SINGLE);
  gtk_tree_view_expand_all (GTK_TREE_VIEW(pref_tree));

  GtkWidget * scrol = create_scroll (vbox, -1, 570, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrol, pref_tree);
  if (num_col == 8)
  {
    gchar * legend={"\n<sub>[1] B. Cordero and al. <i>Dalton Trans</i>, <b>213</b>:1112 (2008).</sub>\n"
                    "<sub>[2] Slater. <i>J. Chem. Phys.</i>, <b>41</b>:3199 (1964).</sub>\n"
                    "<sub>[3] Bondi A. <i>J. Phys. Chem.</i>, <b>68</b>:441 (1964).</sub>\n"
                    "<sub>[4] R.D. Shannon and C.T. Prewitt <i>Acta Cryst. B</i>, <b>25</b>:925-946 (1969).</sub>\n"
                    "<sub>[5] R.D. Shannon <i>Acta Cryst. A</i>, <b>23</b>:751-767 (1976).</sub>"};
     add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(legend, -1, 25, 0.0, 0.5), FALSE, FALSE, 0);
  }
  run_this_gtk_dialog (win, G_CALLBACK(edit_chem_preferences), data);
}

/*!
  \fn GtkWidget * over_param (int object, int style)

  \brief create override check button and entry

  \param object 0 = atoms, 1 = bonds
  \param style style id number
*/
GtkWidget * over_param (int object, int style)
{
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox = create_hbox (BSEP);
  int clone = ((object && style > 2) || (! object && style > 4)) ? 20 : 0;
  int mod = (object) ? -1 : 1;
  gboolean over = (! object) ? TRUE : (object && (style != 2 && style != 5)) ? TRUE : FALSE;
  gchar * leg;
  if (over)
  {
    hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(" ", 60+clone, -1, 0.0, 0.0), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, create_button ("Edit species related parameters", IMG_NONE, NULL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(edit_species_parameters), GINT_TO_POINTER(mod*(style+2))), FALSE, FALSE, 60);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
    hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(" ", 60+clone, -1, 0.0, 0.0), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button ("Override species based parameter", -1, -1, (object) ? tmp_o_bd_rw[style] : tmp_o_at_rs[style], G_CALLBACK(toggled_default_stuff), GINT_TO_POINTER(mod*(style+2))), FALSE, FALSE, 10);
  }
  else
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(" ", 60+clone, -1, 0.0, 0.0), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Set default value", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
  }
  if (object)
  {
    bond_entry_over[style] = create_entry(G_CALLBACK(set_default_stuff), 100, 10, TRUE, GINT_TO_POINTER(mod*(style+2)));
    update_entry_double (GTK_ENTRY(bond_entry_over[style]), tmp_bd_rw[style]);
    if (over) widget_set_sensitive (bond_entry_over[style], tmp_o_bd_rw[style]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, bond_entry_over[style], FALSE, FALSE, 0);
    leg = g_strdup_printf ("%s", (style == 1 || style == 4) ? "pts" : "&#xC5;");
  }
  else
  {
    atom_entry_over[style] = create_entry(G_CALLBACK(set_default_stuff), 100, 10, TRUE, GINT_TO_POINTER(mod*(style+2)));
    update_entry_double (GTK_ENTRY(atom_entry_over[style]), tmp_at_rs[style]);
    if (over) widget_set_sensitive (atom_entry_over[style], tmp_o_at_rs[style]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, atom_entry_over[style], FALSE, FALSE, 0);
    leg = g_strdup_printf ("%s", (style == 1 || style == 6 || style == 4 || style == 9) ? "pts" : "&#xC5;");
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(leg, -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
  g_free (leg);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  return vbox;
}

/*!
  \fn GtkWidget * style_tab (int style)

  \brief create preferences tab for a style

  \param style the style for this tab
*/
GtkWidget * style_tab (int style)
{
  GtkWidget * vbox = create_vbox (BSEP);
  gchar * object[3]={"<b>Atom(s)</b>", "<b>Bond(s)</b>", "\t<u>Clone(s)</u>"};
  gchar * ats[3]={"tom(s) ", "ot(s)", "phere(s)"};
  gchar * ha_init[3]={"A", "D", "S"};
  gchar * la_init[3]={"a", "d", "s"};
  gchar * dim[3]={"radius", "size", "width"};
  gchar * bts[3]={"ond(s)", "ireframe", "ylinders"};
  gchar * hb_init[3]={"B", "W", "C"};
  gchar * lb_init[3]={"b", "w", "c"};
  int i;
  int bsid;
  int lid;
  gchar * str;
  gboolean do_atoms = FALSE;
  gboolean do_bonds = FALSE;
  if (style == 0 || style == 1 || style == 2 || style == 3 || style == 5)
  {
    do_atoms = TRUE;
  }
  if (style == 0 || style == 1 || style == 4)
  {
    do_bonds = TRUE;
    bsid = (style == 4) ? 2 : style;
  }
  if (do_atoms)
  {
    lid = (style == 3) ? 2 : (style == 1 || style == 5) ? 1 : 0;
    for (i=0; i<2; i++)
    {
      adv_box (vbox, object[i*2], 10-5*i, 120, 0.0);
      if (! i)
      {
        str = g_strdup_printf ("\t%s%s %s", ha_init[lid], ats[lid], dim[(lid != 1) ? 0 : 1]);
      }
      else
      {
        str = g_strdup_printf ("\t\tClone %s%s %s", la_init[lid], ats[lid], dim[(lid != 1) ? 0 : 1]);
      }
      adv_box (vbox, str, 10, 120, 0.0);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, over_param (0, (style < 5) ? style+5*i : 4 + 5*i), FALSE, FALSE, 0);
    }
  }
  if (do_bonds)
  {
    // Bond(s) parameters
    for (i=0; i<2; i++)
    {
      adv_box (vbox, object[i+1], 10-5*i, 120, 0.0);
      if (! i)
      {
        str = g_strdup_printf ("\t%s%s %s", hb_init[bsid], bts[bsid], dim[(bsid != 1) ? 0 : 2]);
      }
      else
      {
        str = g_strdup_printf ("\t\tClone %s%s %s", lb_init[bsid], bts[bsid], dim[(bsid != 1) ? 0 : 2]);
      }
      adv_box (vbox, str, 10, 120, 0.0);
      g_free (str);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, over_param (1, bsid+3*i), FALSE, FALSE, 0);
    }
  }

  return vbox;
}

/*!
  \fn GtkWidget * model_preferences ()

  \brief model preferences
*/
GtkWidget * model_preferences ()
{
  GtkWidget * notebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  show_the_widgets (notebook);
  GtkWidget * vbox = create_vbox (BSEP);
  //GtkWidget * hbox;
  //GtkWidget * combo;
  gchar * info[2] = {"The <b>Model</b> tab regroups atom(s), bond(s) and clone(s) options",
                     "which effect apply when the corresponding <b>OpenGL</b> style is used:"};
  gchar * m_list[6][2] = {{"Ball and stick", "atoms and bonds radii"},
                          {"Wireframe", "dots size and wireframes width"},
                          {"Spacefill", "tabulated parameters"},
                          {"Spheres", "atoms radii"},
                          {"Cylinders", "bonds radii"},
                          {"Dots", "dots size"}};
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(" ", -1, 30, 0.0, 0.0), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, pref_list (info, 6, m_list, NULL), FALSE, FALSE, 0);
  gchar * other_info[2] = {"It also provides options to customize atomic label(s),", "and, the model box, if any:"};
  gchar * o_list[2][2] = {{"Labels", "atomic labels"},
                          {"Box", "model box details"}};
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, pref_list (other_info, 2, o_list, NULL), FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(" ", -1, 40, 0.0, 0.0), FALSE, FALSE, 0);
  GtkWidget * hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button ("Always show clone(s), if any.", 250, -1, tmp_clones, G_CALLBACK(toggled_default_stuff), GINT_TO_POINTER(0)), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button ("Always show box, if any.", 250, -1, tmp_clones, G_CALLBACK(toggled_default_stuff), GINT_TO_POINTER(1)), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vbox, gtk_label_new ("General"));

  int i;
  for (i=0; i<OGL_STYLES; i++)
  {
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), style_tab (i), gtk_label_new (text_styles[i]));
  }

  // Show / hide atom(s) ?
  /* Atom radius
     Bond radius
      -> fonction du style
  */
  // Label atom(s)
  //    - Label tab
  // Chemical species color

  // Repeat for clones


  // gtk_notebook_append_page (GTK_NOTEBOOK(notebook), atom_tab (0), gtk_label_new ("Atom(s)"));
  // gtk_notebook_append_page (GTK_NOTEBOOK(notebook), atom_tab (1), gtk_label_new ("Clone(s)"));
  // gtk_notebook_append_page (GTK_NOTEBOOK(notebook), label_tab (), gtk_label_new ("Label(s)"));
  // gtk_notebook_append_page (GTK_NOTEBOOK(notebook), box_tab (), gtk_label_new ("Box"));

  return notebook;
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
  GtkWidget * vbox = create_vbox (BSEP);
  GtkWidget * hbox;
  GtkWidget * combo;

  // Creating an OpenGL edition data structure
  pref_ogl_edit = g_malloc0(sizeof*pref_ogl_edit);
  int i;
  for (i=0; i<6; i++)
  {
    pref_ogl_edit -> pointer[i].a = -1;
    pref_ogl_edit -> pointer[i].b = i;
  }

  gchar * info[2] = {"The <b>OpenGL</b> tab regroups rendering options",
                     "use it to configure the OpenGL 3D scene:"};
  gchar * m_list[3][2] = {{"Material", "aspect for atom(s) and bond(s)"},
                          {"Lights", "lightning of the scene"},
                          {"Fog", "atmosphere effects"}};
  gchar * end = {"It also offers to adjust the main visualization style and the color maps"};

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, pref_list (info, 3, m_list, end), FALSE, FALSE, 30);

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

  show_the_widgets (vbox);

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vbox, gtk_label_new ("General"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), materials_tab (NULL, pref_ogl_edit, & tmp_material), gtk_label_new ("Material"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), lights_tab (NULL, pref_ogl_edit, & tmp_lightning), gtk_label_new ("Lights"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), fog_tab (NULL, pref_ogl_edit, & tmp_fog), gtk_label_new ("Fog"));

  gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook), 0);
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
  double value = string_to_double ((gpointer)m);
  if (i < 8)
  {
    tmp_num_delta[i] = (int) value;
    update_entry_int (res, tmp_num_delta[i]);
  }
  else
  {
    tmp_delta_t[0] = value;
    update_entry_double (res, tmp_delta_t[0]);
  }
}

/*!
  \fn G_MODULE_EXPORT void tunit_changed (GtkComboBox * box, gpointer data)

  \brief change default time units

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void tunit_changed (GtkComboBox * box, gpointer data)
{
  tmp_delta_t[1] = (double) gtk_combo_box_get_active(box);
}

/*!
  \fn GtkWidget * calc_preferences ()

  \brief analysis preferences
*/
GtkWidget * calc_preferences ()
{
  GtkWidget * notebook = gtk_notebook_new ();
  GtkWidget * vbox, * vvbox;
  GtkWidget * hbox, * hhbox;
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
  show_the_widgets (notebook);
  vbox = create_vbox (BSEP);
  gchar * default_delta_num_leg[8] = {"<b>g(r)</b>: number of &#x3b4;r", "<b>s(q)</b>: number of &#x3b4;q", "<b>s(k)</b>: number of &#x3b4;k", "<b>g(r) FFT</b>: number of &#x3b4;r",
                                      "<b>D<sub>ij</sub></b>: number of &#x3b4;r [D<sub>ij</sub>min-D<sub>ij</sub>max]", "<b>Angles distribution</b>: number of &#x3b4;&#x3b8; [0-180°]",
                                      "<b>Spherical harmonics</b>: l<sub>max</sub> in [2-40]", "step(s) between configurations"};

  gchar * info[2] = {"The <b>Analysis</b> tab regroups calculation options",
                     "use it to setup your own default parameters:"};
  gchar * m_list[2][2] = {{"Rings", "ring statistics options"},
                          {"Chains", "chain statistics options"}};
  gchar * end = {"Other options are listed below"};

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, pref_list (info, 2, m_list, end), FALSE, FALSE, 20);
  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  vvbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vvbox, FALSE, FALSE, 10);
  GtkWidget * entry;
  int i;
  for (i=0; i<8; i++)
  {
    if (i == 7)
    {
      hhbox = create_hbox (BSEP);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label ("<b>Mean Squared Displacement</b>:", 310, -1, 0.0, 0.5), FALSE, FALSE, 15);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hhbox, FALSE, FALSE, 5);
    }
    hhbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label (default_delta_num_leg[i], (i ==7) ? 285 : 310, -1, 0.0, 0.5), FALSE, FALSE, (i == 7) ? 30 : 15);
    entry = create_entry (G_CALLBACK(set_default_num_delta), 110, 10, TRUE, GINT_TO_POINTER(i));
    update_entry_int ((GtkEntry *)entry, tmp_num_delta[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, entry, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hhbox, FALSE, FALSE, 5);
  }
  hhbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label ("time step(s) &#x3b4;t", 285, -1, 0.0, 0.5), FALSE, FALSE, 30);
  entry = create_entry (G_CALLBACK(set_default_num_delta), 110, 10, TRUE, GINT_TO_POINTER(i));
  update_entry_double ((GtkEntry *)entry, tmp_delta_t[0]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, entry, FALSE, FALSE, 0);
  GtkWidget * tcombo = create_combo ();
  for (i=0; i<5 ; i++) combo_text_append (tcombo, untime[i]);

  gtk_combo_box_set_active (GTK_COMBO_BOX(tcombo), (int)tmp_delta_t[1]);
  g_signal_connect(G_OBJECT(tcombo), "changed", G_CALLBACK(tunit_changed), NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, tcombo, FALSE, FALSE, 0);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, hhbox, FALSE, FALSE, 5);

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vbox, gtk_label_new ("General"));
  for (i=0; i<2; i++)
  {
    vbox = create_vbox (BSEP);
    search_type = i;
    calc_rings (vbox);
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), vbox, gtk_label_new ((i) ? "Chains" : "Rings"));
  }
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
  if (tmp_delta_t)
  {
    g_free (tmp_delta_t);
    tmp_delta_t = NULL;
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
  if (tmp_o_at_rs)
  {
    g_free (tmp_o_at_rs);
    tmp_o_at_rs = NULL;
  }
  if (tmp_at_rs)
  {
    g_free (tmp_at_rs);
    tmp_at_rs = NULL;
  }
  if (tmp_o_bd_rw)
  {
    g_free (tmp_o_bd_rw);
    tmp_o_bd_rw = NULL;
  }
  if (tmp_bd_rw)
  {
    g_free (tmp_bd_rw);
    tmp_bd_rw = NULL;
  }
  int i;
  for (i=0; i<16; i++)
  {
    if (tmp_atomic_rad[i])
    {
      g_free (tmp_atomic_rad[i]);
      tmp_atomic_rad[i] = NULL;
    }
  }
  for (i=0; i<6; i++)
  {
    if (tmp_bond_rad[i])
    {
      g_free (tmp_bond_rad[i]);
      tmp_bond_rad[i] = NULL;
    }
  }
}

/*!
  \fn void prepare_tmp_default ()

  \brief prepare temporary parameters
*/
void prepare_tmp_default ()
{
  clean_all_tmp ();
  tmp_num_delta = duplicate_int (8, default_num_delta);
  tmp_delta_t = duplicate_double (2, default_delta_t);
  tmp_rsparam = duplicate_int (7, default_rsparam);
  tmp_csparam = duplicate_int (7, default_csparam);
  tmp_opengl = duplicate_int (5, default_opengl);
  duplicate_material (& tmp_material, & default_material);
  tmp_lightning.lights = default_lightning.lights;
  tmp_lightning.spot = copy_light_sources (tmp_lightning.lights, tmp_lightning.lights, default_lightning.spot);
  duplicate_fog (& tmp_fog, & default_fog);
  tmp_clones = default_clones;
  tmp_cell = default_cell;
  tmp_o_at_rs = duplicate_bool (10, default_o_at_rs);
  tmp_at_rs = duplicate_double (10, default_at_rs);
  tmp_o_bd_rw = duplicate_bool (6, default_o_bd_rw);
  tmp_bd_rw = duplicate_double (6, default_bd_rw);
  int i;
  for (i=0; i<16; i++)
  {
    tmp_atomic_rad[i] = duplicate_element_radius (default_atomic_rad[i]);
  }
  for (i=0; i<6; i++)
  {
    tmp_bond_rad[i] = duplicate_element_radius (default_bond_rad[i]);
  }
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
  default_num_delta = duplicate_int (8, tmp_num_delta);
  default_delta_t = duplicate_double (2, tmp_delta_t);
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
  // Model
  default_clones = tmp_clones;
  default_cell = tmp_cell;
  if (default_o_at_rs)
  {
    g_free (default_o_at_rs);
    default_o_at_rs = NULL;
  }
  default_o_at_rs = duplicate_bool (10, tmp_o_at_rs);
  if (default_at_rs)
  {
    g_free (default_at_rs);
    default_at_rs = NULL;
  }
  default_at_rs = duplicate_double (10, tmp_at_rs);
  if (default_o_bd_rw)
  {
    g_free (default_o_bd_rw);
    default_o_bd_rw = NULL;
  }
  default_o_bd_rw = duplicate_bool (6, tmp_o_bd_rw);
  if (default_bd_rw)
  {
    g_free (default_bd_rw);
    default_bd_rw = NULL;
  }
  default_bd_rw = duplicate_double (6, tmp_bd_rw);
  int i;
  for (i=0; i<16; i++)
  {
    if (default_atomic_rad[i]) g_free (default_atomic_rad[i]);
    default_atomic_rad[i] = duplicate_element_radius (tmp_atomic_rad[i]);
  }
  for (i=0; i<6; i++)
  {
    if (default_bond_rad[i]) g_free (default_bond_rad[i]);
    default_bond_rad[i] = duplicate_element_radius (tmp_bond_rad[i]);
  }
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
  default_num_delta[MS-2] = 0;
  default_delta_t[0] = 0.0;
  default_delta_t[1] = -1.0;

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

  // Fog
  default_fog.mode = 0;
  default_fog.based = 0;
  default_fog.density = 0.05;
  default_fog.depth[0] = 1.0;
  default_fog.depth[1] = 90.0;
  default_fog.color = vec3 (0.01f, 0.01f, 0.01f);

  // Model
  default_clones = FALSE;
  default_cell = TRUE;
  for (i=0; i<5; i++)
  {
    default_o_at_rs[i] = default_o_at_rs[i+5] = FALSE;
    default_at_rs[i] = default_at_rs[i+5] = (i == 0 || i == 2) ? 0.5 : DEFAULT_SIZE;
  }
  for (i=0; i<3; i++)
  {
    default_o_bd_rw[i] = default_o_bd_rw[i+3] = (i == 2) ? TRUE : FALSE;
    default_bd_rw[i] = default_bd_rw[i+3] = (i == 0 || i == 2) ? 0.5 : DEFAULT_SIZE;
  }

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

  gchar * mess[2] = {"Browse the following to modify the default configuration of <b>atomes</b>",
                     "by replacing internal parameters by user defined preferences."};
  gchar * mlist[4][2]= {{"Analysis", "calculation preferences"},
                        {"OpenGL", "rendering preferences"},
                        {"Model", "atom(s), bond(s) and box preferences"},
                        {"View", "representation and projection preferences"}};
  gchar * end = "Default parameters are used for any new project added to the workspace\n";

  add_box_child_start (GTK_ORIENTATION_VERTICAL, gbox, pref_list(mess, 4, mlist, end), FALSE, FALSE, 20);

  GtkWidget * hbox = create_hbox (BSEP);
  GtkWidget * but = create_button (NULL, IMG_NONE, NULL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(restore_defaults_parameters), NULL);
  GtkWidget * but_lab = markup_label ("Restore <b>atomes</b> default parameters", -1, -1, 0.5, 0.5);
  add_container_child (CONTAINER_BUT, but, but_lab);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 130);
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
