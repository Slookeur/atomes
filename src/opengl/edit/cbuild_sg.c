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
* @file cbuild_sg.c
* @short Functions to read space group data from XML files
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cbuild_sg.c'
*
* Contains:
*

 - The functions to read space group data from XML files

*
* List of functions:

  int get_this_group_data (space_group * spg,  xmlNodePtr racine);

  space_group * clean_sgl_data (xmlDoc * doc, xmlTextReaderPtr reader);
  space_group * read_sg_xml_file (const char * filetoread);

*/

#include "global.h"
#include "cbuild_edit.h"

gchar * groups[230] = {"P1",
                       "P-1",
                       "P2",
                       "P2<sub>1</sub>",
                       "C2",
                       "Pm",
                       "Pc",
                       "Cm",
                       "Cc",
                       "P2/m",
                       "P2<sub>1</sub>/m",
                       "C2/m",
                       "P2/c",
                       "P2<sub>1</sub>/c",
                       "C2/c",
                       "P222",
                       "P222<sub>1</sub>",
                       "P2<sub>1</sub>2<sub>1</sub>2",
                       "P2<sub>1</sub>2<sub>1</sub>2<sub>1</sub>",
                       "C222<sub>1</sub>",
                       "C222",
                       "F222",
                       "I222",
                       "I2<sub>1</sub>2<sub>1</sub>2<sub>1</sub>",
                       "Pmm2",
                       "Pmc2<sub>1</sub>",
                       "Pcc2",
                       "Pma2",
                       "Pca2<sub>1</sub>",
                       "Pnc2",
                       "Pmn2<sub>1</sub>",
                       "Pba2",
                       "Pna2<sub>1</sub>",
                       "Pnn2",
                       "Cmm2",
                       "Cmc2<sub>1</sub>",
                       "Ccc2",
                       "Amm2",
                       "Aem2",
                       "Ama2",
                       "Aea2",
                       "Fmm2",
                       "Fdd2",
                       "Imm2",
                       "Iba2",
                       "Ima2",
                       "Pmmm",
                       "Pnnn",
                       "Pccm",
                       "Pban",
                       "Pmma",
                       "Pnna",
                       "Pmna",
                       "Pcca",
                       "Pbam",
                       "Pccn",
                       "Pbcm",
                       "Pnnm",
                       "Pmmn",
                       "Pbcn",
                       "Pbca",
                       "Pnma",
                       "Cmcm",
                       "Cmce",
                       "Cmmm",
                       "Cccm",
                       "Cmme",
                       "Ccce",
                       "Fmmm",
                       "Fddd",
                       "Immm",
                       "Ibam",
                       "Ibca",
                       "Imma",
                       "P4",
                       "P4<sub>1</sub>",
                       "P4<sub>2</sub>",
                       "P4<sub>3</sub>",
                       "I4",
                       "I4<sub>1</sub>",
                       "P-4",
                       "I-4",
                       "P4/m",
                       "P4<sub>2</sub>/m",
                       "P4/n",
                       "P4<sub>2</sub>/n",
                       "I4/m",
                       "I4<sub>1</sub>/a",
                       "P422",
                       "P42<sub>1</sub>2",
                       "P4<sub>1</sub>22",
                       "P4<sub>1</sub>2<sub>1</sub>2",
                       "P4<sub>2</sub>22",
                       "P4<sub>2</sub>2<sub>1</sub>2",
                       "P4<sub>3</sub>22",
                       "P4<sub>3</sub>2<sub>1</sub>2",
                       "I422",
                       "I4<sub>1</sub>22",
                       "P4mm",
                       "P4bm",
                       "P4<sub>2</sub>cm",
                       "P4<sub>2</sub>nm",
                       "P4cc",
                       "P4nc",
                       "P4<sub>2</sub>mc",
                       "P4<sub>2</sub>bc",
                       "I4mm",
                       "I4cm",
                       "I4<sub>1</sub>md",
                       "I4<sub>1</sub>cd",
                       "P-42m",
                       "P-42c",
                       "P-42<sub>1</sub>m",
                       "P-42<sub>1</sub>c",
                       "P-4m2",
                       "P-4c2",
                       "P-4b2",
                       "P-4n2",
                       "I-4m2",
                       "I-4c2",
                       "I-42m",
                       "I-42d",
                       "P4/mmm",
                       "P4/mcc",
                       "P4/nbm",
                       "P4/nnc",
                       "P4/mbm",
                       "P4/mnc",
                       "P4/nmm",
                       "P4/ncc",
                       "P4<sub>2</sub>/mmc",
                       "P4<sub>2</sub>/mcm",
                       "P4<sub>2</sub>/nbc",
                       "P4<sub>2</sub>/nnm",
                       "P4<sub>2</sub>/mbc",
                       "P4<sub>2</sub>/mnm",
                       "P4<sub>2</sub>/nmc",
                       "P4<sub>2</sub>/ncm",
                       "I4/mmm",
                       "I4/mcm",
                       "I4<sub>1</sub>/amd",
                       "I4<sub>1</sub>/acd",
                       "P3",
                       "P3<sub>1</sub>",
                       "P3<sub>2</sub>",
                       "R3",
                       "P-3",
                       "R-3",
                       "P312",
                       "P321",
                       "P3<sub>1</sub>12",
                       "P3<sub>1</sub>21",
                       "P3<sub>2</sub>12",
                       "P3<sub>2</sub>21",
                       "R32",
                       "P3m1",
                       "P31m",
                       "P3c1",
                       "P31c",
                       "R3m",
                       "R3c",
                       "P-31m",
                       "P-31c",
                       "P-3m1",
                       "P-3c1",
                       "R-3m",
                       "R-3c",
                       "P6",
                       "P6<sub>1</sub>",
                       "P6<sub>5</sub>",
                       "P6<sub>2</sub>",
                       "P6<sub>4</sub>",
                       "P6<sub>3</sub>",
                       "P-6",
                       "P6/m",
                       "P6<sub>3</sub>/m",
                       "P622",
                       "P6<sub>1</sub>22",
                       "P6<sub>5</sub>22",
                       "P6<sub>2</sub>22",
                       "P6<sub>4</sub>22",
                       "P6<sub>3</sub>22",
                       "P6mm",
                       "P6cc",
                       "P6<sub>3</sub>cm",
                       "P6<sub>3</sub>mc",
                       "P-6m2",
                       "P-6c2",
                       "P-62m",
                       "P-62c",
                       "P6/mmm",
                       "P6/mcc",
                       "P6<sub>3</sub>/mcm",
                       "P6<sub>3</sub>/mmc",
                       "P23",
                       "F23",
                       "I23",
                       "P2<sub>1</sub>3",
                       "I2<sub>1</sub>3",
                       "Pm-3",
                       "Pn-3",
                       "Fm-3",
                       "Fd-3",
                       "Im-3",
                       "Pa-3",
                       "Ia-3",
                       "P432",
                       "P4<sub>2</sub>32",
                       "F432",
                       "F4<sub>1</sub>32",
                       "I432",
                       "P4<sub>3</sub>32",
                       "P4<sub>1</sub>32",
                       "I4<sub>1</sub>32",
                       "P-43m",
                       "F-43m",
                       "I-43m",
                       "P-43n",
                       "F-43c",
                       "I-43d",
                       "Pm-3m",
                       "Pn-3n",
                       "Pm-3n",
                       "Pn-3m",
                       "Fm-3m",
                       "Fm-3c",
                       "Fd-3m",
                       "Fd-3c",
                       "Im-3m",
                       "Ia-3d"};

gchar * hmsymbols[230] = {"P 1",
                          "P -1",
                          "P 1 2 1",
                          "P 1 21 1",
                          "C 1 2 1",
                          "P 1 m 1",
                          "P 1 c 1",
                          "C 1 m 1",
                          "C 1 c 1",
                          "P 1 2/m 1",
                          "P 1 21/m 1",
                          "C 1 2/m 1",
                          "P 1 2/c 1",
                          "P 1 21/c 1",
                          "C 1 2/c 1",
                          "P 2 2 2",
                          "P 2 2 21",
                          "P 21 21 2",
                          "P 21 21 21",
                          "C 2 2 21",
                          "C 2 2 2",
                          "F 2 2 2",
                          "I 2 2 2",
                          "I 21 21 21",
                          "P m m 2",
                          "P m c 21",
                          "P c c 2",
                          "P m a 2",
                          "P c a 21",
                          "P n c 2",
                          "P m n 21",
                          "P b a 2",
                          "P n a 21",
                          "P n n 2",
                          "C m m 2",
                          "C m c 21",
                          "C c c 2",
                          "A m m 2",
                          "A e m 2",
                          "A m a 2",
                          "A e a 2",
                          "F m m 2",
                          "F d d 2",
                          "I m m 2",
                          "I b a 2",
                          "I ma 2",
                          "P 2/m 2/m 2/m",
                          "P 2/n 2/n 2/n",
                          "P 2/c 2/c 2/m",
                          "P 2/b 2/a 2/n",
                          "P 21 /m 2/m 2/a",
                          "P 2/n 21/n 2/a",
                          "P 2/m 2/n 21/a",
                          "P 21/c 2/c 2/a",
                          "P 21/b 21/a 2/m",
                          "P 21/c 21/c 2/n",
                          "P 2/b 21/c 21/m",
                          "P 21/n 21/n 2/m",
                          "P 21/m 21/m 2/n",
                          "P 21/b 2/c 21/n",
                          "P 21/b 21/c 21/a",
                          "P 21/n 21/m 21/a",
                          "C 2/m 2/c 21/m",
                          "C 2/m 2/c 21/e",
                          "C 2/m 2/m 2/m",
                          "C 2/c 2/c 2/m",
                          "C 2/m 2/m 2/e",
                          "C 2/c 2/c 2/e",
                          "F 2/m 2/m 2/m",
                          "F 2/d 2/d 2/d",
                          "I 2/m 2/m 2/m",
                          "I 2/b 2/a 2/m",
                          "I 21/b 21/c 21/a",
                          "I 21/m 21/m 21/a",
                          "P 4",
                          "P 41",
                          "P 42",
                          "P 43",
                          "I 4",
                          "I 41",
                          "P -4",
                          "I -4",
                          "P 4/m",
                          "P 42/m",
                          "P 4/n",
                          "P 42/n",
                          "I 4/m",
                          "I 41/a",
                          "P 4 2 2",
                          "P 4 21 2",
                          "P 41 2 2",
                          "P 41 21 2",
                          "P 42 2 2",
                          "P 42 21 2",
                          "P 43 2 2",
                          "P 43 21 2",
                          "I 4 2 2",
                          "I 41 2 2",
                          "P 4 m m",
                          "P 4 b m",
                          "P 42 c m",
                          "P 42 n m",
                          "P 4 c c",
                          "P 4 n c",
                          "P 42 m c",
                          "P 42 b c",
                          "I 4 m m",
                          "I 4 c m",
                          "I 41 m d",
                          "I 41 c d",
                          "P -4 2 m",
                          "P -4 2 c",
                          "P -4 21 m",
                          "P -4 21 c",
                          "P -4 m 2",
                          "P -4 c 2",
                          "P -4 b 2",
                          "P -4 n 2",
                          "I -4 m 2",
                          "I -4 c 2",
                          "I -4 2 m",
                          "I -4 2 d",
                          "P 4/m 2/m 2/m",
                          "P 4/m 2/c 2/c",
                          "P 4/n 2/b 2/m",
                          "P 4/n 2/n 2/c",
                          "P 4/m 21/b 2/m",
                          "P 4/m 21/b 2/m",
                          "P 4/n 21/m 2/m",
                          "P 4/n 21/c 2/c",
                          "P 42/m 2/m 2/c",
                          "P 42/m 2/c 2/m",
                          "P 42/n 2/b 2/c",
                          "P 42/n 2/n 2/m",
                          "P 42/m 21/b 2/c",
                          "P 42/m 21/n 2/m",
                          "P 42/n 21/m 2/c",
                          "P 42/n 21/c 2/m",
                          "I 4/m 2/m 2/m",
                          "I 4/m 2/c 2/m",
                          "I 41/a 2/m 2/d",
                          "I 41/a 2/c 2/d",
                          "P 3",
                          "P 31",
                          "P 32",
                          "R 3",
                          "P -3",
                          "R -3",
                          "P 3 1 2",
                          "P 3 2 1",
                          "P 31 1 2",
                          "P 31 2 1",
                          "P 32 1 2",
                          "P 32 2 1",
                          "R 3 2",
                          "P 3 m 1",
                          "P 3 1 m",
                          "P 3 c 1",
                          "P 3 1 c",
                          "R 3 m",
                          "R 3 c",
                          "P -3 1 2/m",
                          "P -3 1 2/c",
                          "P -3 2/m 1",
                          "P -3 2/c 1",
                          "R -3 2/m",
                          "R -3 2/c",
                          "P 6",
                          "P 61",
                          "P 65",
                          "P 62",
                          "P 64",
                          "P 63",
                          "P -6",
                          "P 6/m",
                          "P 63/m",
                          "P 6 2 2",
                          "P 61 2 2",
                          "P 65 2 2",
                          "P 62 2 2",
                          "P 64 2 2",
                          "P 63 2 2",
                          "P 6 m m",
                          "P 6 c c",
                          "P 63 c m",
                          "P 63 m c",
                          "P -6 m 2",
                          "P -6 c 2",
                          "P -6 2 m",
                          "P -6 2 c",
                          "P 6/m 2/m 2/m",
                          "P 6/m 2/c 2/c",
                          "P 63/m 2/c 2/m",
                          "P 63/m 2/m 2/c",
                          "P 2 3",
                          "F 2 3",
                          "I 2 3",
                          "P 21 3",
                          "I 21 3",
                          "P 2/m -3",
                          "P 2/n -3",
                          "F 2/m -3",
                          "F 2/d -3",
                          "I 2/m -3",
                          "P 21/a -3",
                          "I 21/a -3",
                          "P 4 3 2",
                          "P 42 3 2",
                          "F 4 3 2",
                          "F 41 3 2",
                          "I 4 3 2",
                          "P 43 3 2",
                          "P 41 3 2",
                          "I 41 3 2",
                          "P -4 3 m",
                          "F -4 3 m",
                          "I -4 3 m",
                          "P -4 3 n",
                          "F -4 3 c",
                          "I -4 3 d",
                          "P 4/m -3 2/m",
                          "P 4/n -3 2/n",
                          "P 42/m -3 2/n",
                          "P 42/n -3 2/m",
                          "F 4/m -3 2/m",
                          "F 4/m -3 2/c",
                          "F 41/d -3 2/m",
                          "F 41/d -3 2/c",
                          "I 4/m -3 2/m",
                          "I 41/a -3 2/d"};

/*!
  \fn space_group * clean_sgl_data (xmlDoc * doc, xmlTextReaderPtr reader)

  \brief clean space group and corresponding XML data

  \param doc the XML doc pointer to free
  \param reader the XML reader to free
*/
space_group * clean_sgl_data (xmlDoc * doc, xmlTextReaderPtr reader)
{
  xmlFreeDoc(doc);
  xmlFreeTextReader(reader);
  xmlCleanupParser();
  return NULL;
}

/*!
  \fn int get_this_group_data (space_group * spg,  xmlNodePtr racine)

  \brief retrieve space group data

  \param spg the space group pointer to fill
  \param racine the XML node root
*/
int get_this_group_data (space_group * spg,  xmlNodePtr racine)
{
  xmlNodePtr node, num_node;
  xmlNodePtr the_setting, the_point;
  xmlNodePtr sp_node, ps_node, pp_node;
  xmlAttrPtr s_node, p_node;
  int val;
  int j, k;
  node = findnode (racine -> children, "settings");
  if (node == NULL) return 0;
  s_node = node -> properties;
  if (s_node == NULL) return 0;
  while (s_node)
  {
    num_node = s_node -> children;
    if (num_node == NULL) return 0;
    if (g_strcmp0 ("num",(char *)s_node -> name) == 0)
    {
      val = (int) string_to_double ((gpointer)xmlNodeGetContent(num_node));
    }
    s_node = s_node -> next;
  }
  if (! val) return 0;
  spg -> nums = val;
  spg -> settings = g_malloc0(spg -> nums*sizeof*spg -> settings);
  node = node -> children;
  if (node == NULL) return 0;
  j = 0;
  for (sp_node = node; sp_node; sp_node = sp_node -> next)
  {
    if (sp_node -> type == XML_ELEMENT_NODE)
    {
      s_node = sp_node -> properties;
      if (s_node == NULL) return 0;
      while (s_node)
      {
        the_setting = s_node -> children;
        if (the_setting == NULL) return 0;
        if (g_strcmp0 ("name",(char *)s_node -> name) == 0)
        {
          spg -> settings[j].name = g_strdup_printf ("%s", (char *)xmlNodeGetContent(the_setting));
        }
        else if (g_strcmp0 ("origin",(char *)s_node -> name) == 0)
        {
          spg -> settings[j].origin = (int)string_to_double ((gpointer)xmlNodeGetContent(the_setting));
        }
        else if (g_strcmp0 ("x",(char *)s_node -> name) == 0)
        {
          spg -> settings[j].pos[0] = g_strdup_printf ("%s", (char *)xmlNodeGetContent(the_setting));
        }
        else if (g_strcmp0 ("y",(char *)s_node -> name) == 0)
        {
          spg -> settings[j].pos[1] =  g_strdup_printf ("%s", (char *)xmlNodeGetContent(the_setting));
        }
        else if (g_strcmp0 ("z",(char *)s_node -> name) == 0)
        {
          spg -> settings[j].pos[2] =  g_strdup_printf ("%s", (char *)xmlNodeGetContent(the_setting));
        }
        s_node = s_node -> next;
      }
      ps_node = findnode (sp_node -> children, "points");
      if (ps_node)
      {
        p_node = ps_node -> properties;
        if (p_node == NULL) return 0;
        while (p_node)
        {
          num_node = p_node -> children;
          if (num_node == NULL) return 0;
          if (g_strcmp0 ("num",(char *)p_node -> name) == 0)
          {
            val = (int) string_to_double ((gpointer)xmlNodeGetContent(num_node));
          }
          p_node = p_node -> next;
        }
        if (! val) return 0;
        spg -> settings[j].nump = val;
        spg -> settings[j].points = g_malloc0(val*sizeof*spg -> settings[j].points);
        ps_node = ps_node -> children;
        if (ps_node == NULL) return 0;
        k = 0;
        for (pp_node = ps_node; pp_node; pp_node = pp_node->next)
        {
          if (pp_node -> type == XML_ELEMENT_NODE)
          {
            s_node = pp_node -> properties;
            if (s_node == NULL) return 0;
            spg -> settings[j].points[k] = g_malloc0(3*sizeof*spg -> settings[j].points[k]);
            while (s_node)
            {
              the_point = s_node -> children;
              if (the_point == NULL) return 0;
              if (g_strcmp0 ("x",(char *)s_node -> name) == 0)
              {
                spg -> settings[j].points[k][0] = g_strdup_printf ("%s", (char *)xmlNodeGetContent(the_point));
              }
              else if (g_strcmp0 ("y",(char *)s_node -> name) == 0)
              {
                spg -> settings[j].points[k][1] =  g_strdup_printf ("%s", (char *)xmlNodeGetContent(the_point));
              }
              else if (g_strcmp0 ("z",(char *)s_node -> name) == 0)
              {
                spg -> settings[j].points[k][2] =  g_strdup_printf ("%s", (char *)xmlNodeGetContent(the_point));
              }
              s_node = s_node -> next;
            }
            k ++;
          }
        }
      }
      j ++;
    }
  }
  return 1;
}

/*!
  \fn space_group * read_sg_xml_file (const char * filetoread)

  \brief read space group data from XML file

  \param filetoread the file to read
*/
space_group * read_sg_xml_file (const char * filetoread)
{
  int i, j;
  const xmlChar sgl[8]="sg-xml";
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  xmlNodePtr racine, node, num_node;
  xmlNodePtr cp_node, wp_node, wc_node;
  xmlNodePtr the_wyck, the_pos;
  xmlAttrPtr c_node, w_node, p_node;

  space_group * spg = g_malloc0(sizeof*spg);
  reader = xmlReaderForFile(filetoread, NULL, 0);
  if (reader == NULL)
  {
    return NULL;
  }
  else
  {
    doc = xmlParseFile(filetoread);
    if (doc == NULL) return 0;
    racine = xmlDocGetRootElement(doc);
    if (g_strcmp0 ((char *)(racine -> name), (char *)sgl) != 0) return clean_sgl_data (doc, reader);
    spg = g_malloc0(sizeof*spg);
    node = findnode (racine -> children, "space-group");
    if (node == NULL) return clean_sgl_data (doc, reader);
    spg -> name = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(node));

    node = findnode (racine -> children, "sg-num");
    if (node == NULL) return clean_sgl_data (doc, reader);
    spg -> id = (int) string_to_double ((gpointer)xmlNodeGetContent(node));

    node = findnode (racine -> children, "hm-symbol");
    if (node == NULL) return clean_sgl_data (doc, reader);
    spg -> hms = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(node));

    node = findnode (racine -> children, "bravais");
    if (node == NULL) return clean_sgl_data (doc, reader);
    spg -> bravais = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(node));

    // Settings
    if (!get_this_group_data (spg, racine)) return clean_sgl_data (doc, reader);

    node = findnode(racine -> children, "wyckoff");
    if (node == NULL) return clean_sgl_data (doc, reader);
    c_node = node -> properties;
    if (c_node == NULL) return clean_sgl_data (doc, reader);
    while (c_node)
    {
      num_node = c_node -> children;
      if (num_node == NULL) return clean_sgl_data (doc, reader);
      if (g_strcmp0 ("num",(char *)c_node -> name) == 0)
      {
        spg -> numw = (int) string_to_double ((gpointer)xmlNodeGetContent(num_node));
      }
      c_node = c_node -> next;
    }
    if (! spg -> numw) return clean_sgl_data (doc, reader);
    spg -> wyckoff = g_malloc0(spg -> numw*sizeof*spg -> wyckoff);
    i = 0;
    node = node -> children;
    if (node == NULL) return clean_sgl_data (doc, reader);
    for (cp_node = node; cp_node; cp_node = cp_node->next)
    {
      if (cp_node -> type == XML_ELEMENT_NODE)
      {
        w_node = cp_node -> properties;
        if (w_node == NULL) return clean_sgl_data (doc, reader);
        while (w_node)
        {
          the_wyck = w_node -> children;
          if (the_wyck == NULL) return clean_sgl_data (doc, reader);
          if (g_strcmp0 ("mul",(char *)w_node -> name) == 0)
          {
            spg -> wyckoff[i].multi = (int)string_to_double ((gpointer)xmlNodeGetContent(the_wyck));
          }
          else if (g_strcmp0 ("let",(char *)w_node -> name) == 0)
          {
            spg -> wyckoff[i].let = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(the_wyck));
          }
          else if (g_strcmp0 ("site",(char *)w_node -> name) == 0)
          {
            spg -> wyckoff[i].site = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(the_wyck));
          }
          w_node = w_node -> next;
        }
        spg -> wyckoff[i].pos = g_malloc0(spg -> wyckoff[i].multi*sizeof*spg -> wyckoff[i].pos);
        wp_node = cp_node -> children;
        if (wp_node == NULL) return clean_sgl_data (doc, reader);
        j = 0;
        for (wc_node = wp_node; wc_node; wc_node = wc_node->next)
        {
          if (wc_node -> type == XML_ELEMENT_NODE)
          {
            p_node = wc_node -> properties;
            if (p_node == NULL) return clean_sgl_data (doc, reader);
            spg -> wyckoff[i].pos[j] = g_malloc0(3*sizeof*spg -> wyckoff[i].pos[j]);
            while (p_node)
            {
              the_pos = p_node -> children;
              if (the_pos == NULL) return clean_sgl_data (doc, reader);
              if (g_strcmp0 ("x",(char *)p_node -> name) == 0)
              {
                spg -> wyckoff[i].pos[j][0] = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(the_pos));
              }
              else if (g_strcmp0 ("y",(char *)p_node -> name) == 0)
              {
                spg -> wyckoff[i].pos[j][1] =  g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(the_pos));
              }
              else if (g_strcmp0 ("z",(char *)p_node -> name) == 0)
              {
                spg -> wyckoff[i].pos[j][2] =  g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(the_pos));
              }
              p_node = p_node -> next;
            }
            j ++;
          }
        }
        i ++;
      }
    }

    xmlFreeDoc(doc);
    xmlFreeTextReader(reader);
    xmlCleanupParser();
    return spg;
  }
}
