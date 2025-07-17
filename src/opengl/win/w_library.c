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
* @file w_library.c
* @short Functions to create the molecular library \n
         Functions to read the 'Simple chemical XML' files
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_library.c'
*
* Contains:
*

 - The functions to create the molecular library
 - The functions to read the 'Simple chemical XML' files

*
* List of functions:

  int clean_xml_data (xmlDoc * doc, xmlTextReaderPtr reader);
  int sml_preview (const char * filetoread);
  int get_family (gchar * str);
  int get_sml_files ();
  int select_from_library (gboolean visible, project * this_proj, atom_search * asearch);
  int insert_this_project_from_lib (int id, gboolean visible, project * this_proj, atom_search * asearch);

  double get_z_from_periodic_table (gchar * lab);

  gchar * replace_markup (char * init, char * key, char * rep);
  gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif);
  gchar * check_xml_string (gchar * init);
  gchar * open_sml_file (const char * filetoread, int fam);

  void sort_files (int num_f);
  void fill_molecule_tree (GtkListStore * store);
  void fill_family_tree (GtkListStore * store);
  void insert_preview ();
  void prepare_preview (int active, int id, gboolean visible);

  G_MODULE_EXPORT void select_library_data (GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer data);
  G_MODULE_EXPORT void set_library_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  G_MODULE_EXPORT void run_select_from_library (GtkDialog * lib, gint response_id, gpointer data);

  GtkWidget * library_tree (GtkListStore * store, int id, gchar * name);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "project.h"
#include "glview.h"
#include "glwindow.h"
#include <libxml/xmlreader.h>

element_data periodic_table_info[] = {
  {"X", "Unknown", 0, 0},
  {"H", "Hydrogen", 1, 1.008},
  {"He", "Helium", 2, 4.003},
  {"Li", "Lithium", 3, 6.94},
  {"Be", "Beryllium", 4, 9.012},
  {"B", "Boron", 5, 10.81},
  {"C", "Carbon", 6, 12.011},
  {"N", "Nitrogen", 7, 14.007},
  {"O", "Oxygen", 8, 15.999},
  {"F", "Fluorine", 9, 18.998},
  {"Ne", "Neon", 10, 20.18},
  {"Na", "Sodium", 11, 22.99},
  {"Mg", "Magnesium", 12, 24.305},
  {"Al", "Aluminium", 13, 26.982},
  {"Si", "Silicon", 14, 28.085},
  {"P", "Phosphorus", 15, 30.974},
  {"S", "Sulfur", 16, 32.06},
  {"Cl", "Chlorine", 17, 35.45},
  {"Ar", "Argon", 18, 39.948},
  {"K", "Potassium", 19, 39.098},
  {"Ca", "Calcium", 20, 40.078},
  {"Sc", "Scandium", 21, 44.956},
  {"Ti", "Titanium", 22, 47.867},
  {"V", "Vanadium", 23, 50.942},
  {"Cr", "Chromium", 24, 51.996},
  {"Mn", "Manganese", 25, 54.938},
  {"Fe", "Iron", 26, 55.845},
  {"Co", "Cobalt", 27, 58.933},
  {"Ni", "Nickel", 28, 58.693},
  {"Cu", "Copper", 29, 63.546},
  {"Zn", "Zinc", 30, 65.38},
  {"Ga", "Gallium", 31, 69.723},
  {"Ge", "Germanium", 32, 72.63},
  {"As", "Arsenic", 33, 74.922},
  {"Se", "Selenium", 34, 78.971},
  {"Br", "Bromine", 35, 79.904},
  {"Kr", "Krypton", 36, 83.798},
  {"Rb", "Rubidium", 37, 85.468},
  {"Sr", "Strontium", 38, 87.62},
  {"Y", "Yttrium", 39, 88.906},
  {"Zr", "Zirconium", 40, 91.224},
  {"Nb", "Niobium", 41, 92.906},
  {"Mo", "Molybdenum", 42, 95.95},
  {"Tc", "Technetium", 43, 98},
  {"Ru", "Ruthenium", 44, 101.07},
  {"Rh", "Rhodium", 45, 102.906},
  {"Pd", "Palladium", 46, 106.42},
  {"Ag", "Silver", 47, 107.868},
  {"Cd", "Cadmium", 48, 112.414},
  {"In", "Indium", 49, 114.818},
  {"Sn", "Tin", 50, 118.71},
  {"Sb", "Antimony", 51, 121.76},
  {"Te", "Tellurium", 52, 127.6},
  {"I", "Iodine", 53, 126.904},
  {"Xe", "Xenon", 54, 131.293},
  {"Cs", "Caesium", 55, 132.905},
  {"Ba", "Barium", 56, 137.327},
  {"La", "Lanthanum", 57, 138.905},
  {"Ce", "Cerium", 58, 140.116},
  {"Pr", "Praseodymium", 59, 140.908},
  {"Nd", "Neodymium", 60, 144.242},
  {"Pm", "Promethium", 61, 145},
  {"Sm", "Samarium", 62, 150.36},
  {"Eu", "Europium", 63, 151.964},
  {"Gd", "Gadolinium", 64, 157.25},
  {"Tb", "Terbium", 65, 158.925},
  {"Dy", "Dysprosium", 66, 162.5},
  {"Ho", "Holmium", 67, 164.93},
  {"Er", "Erbium", 68, 167.259},
  {"Tm", "Thulium", 69, 168.934},
  {"Yb", "Ytterbium", 70, 173.045},
  {"Lu", "Lutetium", 71, 174.967},
  {"Hf", "Hafnium", 72, 178.49},
  {"Ta", "Tantalum", 73, 180.948},
  {"W", "Tungsten", 74, 183.84},
  {"Re", "Rhenium", 75, 186.207},
  {"Os", "Osmium", 76, 190.23},
  {"Ir", "Iridium", 77, 192.217},
  {"Pt", "Platinum", 78, 195.084},
  {"Au", "Gold", 79, 196.967},
  {"Hg", "Mercury", 80, 200.592},
  {"Tl", "Thallium", 81, 204.38},
  {"Pb", "Lead", 82, 207.2},
  {"Bi", "Bismuth", 83, 208.98},
  {"Po", "Polonium", 84, 209},
  {"At", "Astatine", 85, 210},
  {"Rn", "Radon", 86, 222},
  {"Fr", "Francium", 87, 223},
  {"Ra", "Radium", 88, 226},
  {"Ac", "Actinium", 89, 227},
  {"Th", "Thorium", 90, 232.038},
  {"Pa", "Protactinium", 91, 231.036},
  {"U", "Uranium", 92, 238.029},
  {"Np", "Neptunium", 93, 237},
  {"Pu", "Plutonium", 94, 244},
  {"Am", "Americium", 95, 243},
  {"Cm", "Curium", 96, 247},
  {"Bk", "Berkelium", 97, 247},
  {"Cf", "Californium", 98, 251},
  {"Es", "Einsteinium", 99, 252},
  {"Fm", "Fermium", 100, 257},
  {"Md", "Mendelevium", 101, 258},
  {"No", "Nobelium", 102, 258},
  {"Lr", "Lawrencium", 103, 262},
  {"Rf", "Rutherfordium", 104, 267},
  {"Db", "Dubnium", 105, 268},
  {"Sg", "Seaborgium", 106, 269},
  {"Bh", "Bohrium", 107, 270},
  {"Hs", "Hassium", 108, 277},
  {"Mt", "Meitnerium", 109, 278},
  {"Ds", "Darmstadtium", 110, 281},
  {"Rg", "Roentgenium", 111, 282},
  {"Cn", "Copernicium", 112, 285},
  {"Nh", "Nihonium", 113, 286},
  {"Fl", "Flerovium", 114, 289},
  {"Mc", "Moscovium", 115, 289},
  {"Lv", "Livermorium", 116, 293},
  {"Ts", "Tennessine", 117, 294},
  {"Og", "Oganesson", 118, 294},
  {"D", "Deuterium", 1, 2.014000}};

insertion_menu mol[] = {
    { "Atom", NULL, -1, 0 },
    { NULL, "H", 1, 1 },
    { NULL, "B", 5, 1 },
    { NULL, "C", 6, 1 },
    { NULL, "N", 7, 1 },
    { NULL, "O", 8, 1 },
    { NULL, "F", 9, 1 },
    { NULL, "S", 16, 1 },
    { NULL, "Cl", 17, 1 },
    { NULL, "Other ...", -1, 1 },
    { "Library", NULL, -1, 0 },
    { NULL, "H<sub>2</sub>O", -1, 3 },
    { NULL, "CH<sub>4</sub>", -1, 5 },
    { NULL, "Toluene", -1, 15 },
    { NULL, "Cp", -1, 10 },
    { NULL, "C<sub>60</sub>", -1,  60},
    { NULL, "Ni-Phthalocyanine", -1, 57 },
    { NULL, "More ...", -1, -1 },
    { NULL, NULL, -1, 0 }};

#define FAMILY 26
gchar * family_list[FAMILY]={"Misc",
                             "Alcohols",
                             "Aldehydes",
                             "Alkanes",
                             "Alkenes",
                             "Alkynes",
                             "Amides",
                             "Amines",
                             "Amino acids",
                             "Aromatics",
                             "Carboxylic acids",
                             "Cyclic alkanes",
                             "Cyclic alkenes",
                             "Ethers",
                             "Fatty acids",
                             "Fullerenes",
                             "Heterocyclics",
                             "Macrocycles",
                             "Ketones",
                             "Nitriles",
                             "Nucleobases",
                             "Steroids",
                             "Sugars (Linears)",
                             "Sugars (Cyclics)",
                             "Sulfoxides",
                             "Thiols"};

gchar * family_dir[FAMILY]={"Misc",
                            "Alcohols",
                            "Aldehydes",
                            "Alkanes",
                            "Alkenes",
                            "Alkynes",
                            "Amides",
                            "Amines",
                            "Amino_acids",
                            "Aromatics",
                            "Carboxylic_acids",
                            "Cyclic_alkanes",
                            "Cyclic_alkenes",
                            "Ethers",
                            "Fatty_acids",
                            "Fullerenes",
                            "Heterocyclics",
                            "Macrocycles",
                            "Ketones",
                            "Nitriles",
                            "Nucleobases",
                            "Steroids",
                            "Linear_sugars",
                            "Cyclic_sugars",
                            "Sulfoxides",
                            "Thiols"};

#ifdef GTK3
#ifndef G_OS_WIN32
extern void gtk_window_change_gdk_visual (GtkWidget * win);
#endif // G_OS_WIN32
#endif // GTK3
extern gboolean create_3d_model (int p, gboolean load);
extern G_MODULE_EXPORT void on_realize (GtkGLArea * area, gpointer data);
extern xmlNodePtr findnode (xmlNodePtr startnode, char * nname);
extern int action_atoms_from_project (project * this_proj, atom_search * asearch, gboolean visible);
extern void to_insert_in_project (int stat, int orig, project * this_proj, atom_search * asearch, gboolean visible);
extern void create_object_from_library (int p);
extern atom_search * remove_search;
GtkListStore * family_store;
GtkTreeIter first_family_iter;
GtkListStore * molecule_store;
GtkTreeIter first_mol_iter;
GtkTreeSelection * libselect[2];
int the_family;
gchar * the_molecule;
gchar ** sml_file_name;
gchar ** mol_name;
GtkWidget * lib_preview_box = NULL;
GtkWidget * lib_preview_plot = NULL;
project * lib_proj = NULL;
gchar * other_name[5];
int o_names;
int inserted_from_lib;

/*!
  \fn double get_z_from_periodic_table (gchar * lab)

  \brief get Z from atom label

  \param lab the atomic label
*/
double get_z_from_periodic_table (gchar * lab)
{
  int i;
  for (i=0; i<120; i++)
  {
    if (g_strcmp0 (periodic_table_info[i].lab, lab) == 0) return periodic_table_info[i].Z;
  }
  return 0.0;
}

/*!
  \fn int clean_xml_data (xmlDoc * doc, xmlTextReaderPtr reader)

  \brief free XML data

  \param doc the XML doc pointer to free
  \param reader the XML reader to free
*/
int clean_xml_data (xmlDoc * doc, xmlTextReaderPtr reader)
{
  xmlFreeDoc(doc);
  xmlFreeTextReader(reader);
  xmlCleanupParser();
  return 0;
}

/*!
  \fn gchar * replace_markup (char * init, char * key, char * rep)

  \brief replace pattern in string

  \param init the string
  \param key the pattern to replace
  \param rep the new pattern
*/
gchar * replace_markup (char * init, char * key, char * rep)
{
  char * buffer = NULL;
  char * p = NULL;
  int inilen = strlen(init);
  int oldlen = strlen(key);
  int newlen = 0;
  if (rep) newlen = strlen(rep);

  // Is key in init
  if (!(p = strstr(init, key))) return init;
  buffer = g_malloc0((inilen+newlen-oldlen+1)*sizeof*buffer);

  strncpy (buffer + strlen(buffer), init, p - init);
  if (rep)
  {
    sprintf (buffer + strlen(buffer), "%s", rep);
    //strcpy (buffer + strlen(buffer), rep);
  }
  sprintf (buffer + strlen(buffer), "%s", p + oldlen);
  //strcpy (buffer + strlen(buffer), p + oldlen);
  return g_strdup_printf ("%s", buffer);
}

/*!
  \fn gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif)

  \brief substitute all patterns in string

  \param init the initial string
  \param o_motif the pattern to replace
  \param n_motif the new pattern
*/
gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif)
{
  gchar * str_a, * str_b;
  str_a = g_strdup_printf ("%s", replace_markup (init, o_motif, n_motif));
  str_b = g_strdup_printf ("%s", init);
  while (g_strcmp0 (str_a, str_b))
  {
    g_free (str_b);
    str_b = g_strdup_printf ("%s", str_a);
    g_free (str_a);
    str_a = g_strdup_printf ("%s", replace_markup (str_b, o_motif, n_motif));
  }
  g_free (str_b);
  return str_a;
}

/*!
  \fn gchar * check_xml_string (gchar * init)

  \brief check for, and correct tags in XML string

  \param init the XML string to check / correct
*/
gchar * check_xml_string (gchar * init)
{
  gchar * str = g_strdup_printf ("%s", substitute_string (init, "--i--", "<i>"));
  str = g_strdup_printf ("%s", substitute_string (str, "--I--", "</i>"));
  str = g_strdup_printf ("%s", substitute_string (str, "--b--", "<sup>"));
  str = g_strdup_printf ("%s", substitute_string (str, "--e--", "</sup>"));
  str = g_strdup_printf ("%s", substitute_string (str, "--c--", "<sub>"));
  str = g_strdup_printf ("%s", substitute_string (str, "--f--", "</sub>"));

  return str;
}

/*!
  \fn int sml_preview (const char * filetoread)

  \brief retrieve preview information from 'Simple chemical library XML' file

  \param filetoread the name of the file to read
*/
int sml_preview (const char * filetoread)
{
  int i, j, k;
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  xmlNodePtr racine, name_node, chem_node;
  xmlNodePtr n_node, at_node, sp_node, coord_node;
  xmlNodePtr spec_node, lab_node, lot_node, pbc_node;
  xmlAttrPtr xspec;

  if (lib_proj != NULL) close_project (lib_proj);
  init_project (TRUE);
  lib_proj = active_project;

  reader = xmlReaderForFile(filetoread, NULL, 0);
  doc = xmlParseFile(filetoread);
  racine = xmlDocGetRootElement(doc);

  name_node = findnode (racine -> children, "names");
  if (name_node == NULL) return clean_xml_data (doc, reader);
  n_node = findnode (name_node -> children, "library-name");
  if (n_node == NULL) return clean_xml_data (doc, reader);
  lib_proj -> name = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(n_node));
  lib_proj -> name = check_xml_string (lib_proj -> name);
  n_node = findnode (name_node -> children, "iupac-name");
  if (n_node == NULL) return clean_xml_data (doc, reader);
  o_names = 0;
  other_name[0] = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(n_node));
  o_names ++;
  n_node = findnode (name_node -> children, "other-names");
  if (n_node == NULL) return clean_xml_data (doc, reader);
  n_node = n_node -> children;
  for (name_node = n_node; name_node && o_names < 5; name_node = name_node->next)
  {
    if (name_node -> type == XML_ELEMENT_NODE)
    {
      other_name[o_names] = g_strdup_printf ("%s", (gchar *)xmlNodeGetContent(name_node));
      o_names ++;
    }
  }
  chem_node = findnode(racine -> children, "chemistry");
  if (chem_node == NULL) return clean_xml_data (doc, reader);
  at_node = findnode (chem_node -> children, "atoms");
  if (at_node == NULL) return clean_xml_data (doc, reader);
  lib_proj -> natomes = (int)string_to_double ((gpointer)xmlNodeGetContent(at_node));

  sp_node = findnode (chem_node -> children, "species");
  if (sp_node == NULL) return clean_xml_data (doc, reader);
  spec_node = sp_node -> properties -> children;
  if (spec_node == NULL) return clean_xml_data (doc, reader);
  lib_proj -> nspec = (int)string_to_double ((gpointer)xmlNodeGetContent(spec_node));
  if (lib_proj -> natomes < 1 || lib_proj -> nspec < 1) return clean_xml_data (doc, reader);
  alloc_proj_data (lib_proj, 1);
  lab_node = sp_node -> children;
  if (lab_node == NULL) return clean_xml_data (doc, reader);
  j = 0;
  for (i=0; i<lib_proj -> nspec; i++)
  {
    lab_node = findnode (lab_node, "label");
    if (lab_node == NULL) return clean_xml_data (doc, reader);
    lib_proj -> chemistry -> label[i] = g_strdup_printf ("%s", (char *)xmlNodeGetContent(lab_node));
    xspec = lab_node -> properties;
    if (xspec == NULL) return clean_xml_data (doc, reader);
    while (xspec)
    {
      lot_node = xspec -> children;
      if (lot_node == NULL) return clean_xml_data (doc, reader);
      if (g_strcmp0 ("num",(char *)xspec -> name) == 0)
      {
        lib_proj -> chemistry -> nsps[i] = (int)string_to_double ((gpointer)xmlNodeGetContent(lot_node));
        lib_proj -> chemistry -> chem_prop[CHEM_Z][i] = get_z_from_periodic_table (lib_proj -> chemistry -> label[i]);
        k = (int)lib_proj -> chemistry -> chem_prop[CHEM_Z][i];
        lib_proj -> chemistry -> chem_prop[CHEM_M][i] = periodic_table_info[k].M;
        lib_proj -> chemistry -> chem_prop[CHEM_R][i] = set_radius_ (& k, & j);
      }
      xspec = xspec -> next;
    }
    lab_node = lab_node -> next;
  }
  active_chem = lib_proj -> chemistry;
  initcutoffs (active_chem, lib_proj -> nspec);
  at_node = findnode (racine -> children, "coordinates");
  if (at_node == NULL) return clean_xml_data (doc, reader);
  coord_node = at_node -> children;
  if (coord_node == NULL) return clean_xml_data (doc, reader);
  for (i=0; i<lib_proj -> natomes; i++)
  {
    coord_node = findnode (coord_node, "atom");
    if (coord_node == NULL) return clean_xml_data (doc, reader);
    xspec = coord_node -> properties;
    if (xspec == NULL) return clean_xml_data (doc, reader);
    while (xspec)
    {
      lot_node = xspec -> children;
      if (lot_node == NULL) return clean_xml_data (doc, reader);
      if (g_strcmp0 ("x",(char *)xspec -> name) == 0)
      {
        lib_proj -> atoms[0][i].x = string_to_double ((gpointer)xmlNodeGetContent(lot_node));
      }
      else if (g_strcmp0 ("y",(char *)xspec -> name) == 0)
      {
        lib_proj -> atoms[0][i].y = string_to_double ((gpointer)xmlNodeGetContent(lot_node));
      }
      else if (g_strcmp0 ("z",(char *)xspec -> name) == 0)
      {
        lib_proj -> atoms[0][i].z = string_to_double ((gpointer)xmlNodeGetContent(lot_node));
      }
      else if (g_strcmp0 ("sp",(char *)xspec -> name) == 0)
      {
        lib_proj -> atoms[0][i].sp = (int)string_to_double ((gpointer)xmlNodeGetContent(lot_node));
        lib_proj -> atoms[0][i].show[0] = TRUE;
      }
      xspec = xspec -> next;
    }
    coord_node = coord_node -> next;
  }
  pbc_node = findnode (racine -> children, "lattice");
  if (pbc_node != NULL)
  {

  }
  xmlFreeDoc(doc);
  xmlFreeTextReader(reader);
  xmlCleanupParser();
  return 1;
}

/*!
  \fn gchar * open_sml_file (const char * filetoread, int fam)

  \brief open 'Simple chemical library XML' file

  \param filetoread the name of the file to open
  \param fam the molecular family
*/
gchar * open_sml_file (const char * filetoread, int fam)
{
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  const xmlChar sml[8]="scl-xml";
  xmlChar * cdata;
  xmlNodePtr racine, node;
  /*
   * build an xmlReader for that file
   */
  reader = xmlReaderForFile(filetoread, NULL, 0);
  if (reader == NULL)
  {
    return NULL;
  }
  else
  {
    doc = xmlParseFile(filetoread);
    if (doc == NULL) return NULL;
    racine = xmlDocGetRootElement(doc);
    if (g_strcmp0 ((char *)(racine -> name), (char *)sml) != 0)
    {
      clean_xml_data (doc, reader);
      return NULL;
    }
    node = racine -> children;
    node = findnode(node, "class");
    cdata = xmlNodeGetContent(node);
    if (g_strcmp0 ((gchar *)cdata, family_list[fam]) != 0)
    {
      clean_xml_data (doc, reader);
      return NULL;
    }
    node = findnode (racine -> children, "names");
    if (node == NULL)
    {
      clean_xml_data (doc, reader);
      return NULL;
    }
    node = findnode (node -> children, "library-name");
    if (node == NULL)
    {
      clean_xml_data (doc, reader);
      return NULL;
    }
    cdata = xmlNodeGetContent(node);
    xmlFreeDoc(doc);
    xmlFreeTextReader(reader);
    xmlCleanupParser();
    return (gchar *)cdata;
  }
}

/*!
  \fn int get_family (gchar * str)

  \brief get molecular family id

  \param str the moecular family
*/
int get_family (gchar * str)
{
  int i;
  for (i=0; i<FAMILY; i++)
  {
    if (g_strcmp0 (str,family_list[i]) == 0) return i;
  }
  return -1;
}

/*!
  \fn void sort_files (int num_f)

  \brief sort file(s) by molecular family and name

  \param num_f number of file(s) to sort
*/
void sort_files (int num_f)
{
  int i, j;
  gchar * str = NULL;
  for(i=0;i<num_f;i++)
  {
    for(j=i+1;j<num_f;j++)
    {
       if(g_strcmp0 (mol_name[i],mol_name[j])>0)
       {
         str = g_strdup_printf ("%s", mol_name[i]);
         mol_name[i] = g_strdup_printf ("%s", mol_name[j]);
         mol_name[j] = g_strdup_printf ("%s", str);
         str = g_strdup_printf ("%s", sml_file_name[i]);
         sml_file_name[i] = g_strdup_printf ("%s", sml_file_name[j]);
         sml_file_name[j] = g_strdup_printf ("%s", str);
       }
    }
  }
  for(i=0;i<num_f;i++)
  {
    mol_name[i] = check_xml_string(mol_name[i]);
  }
}

/*!
  \fn int get_sml_files ()

  \brief get the library 'Simple chemical library XML' files
*/
int get_sml_files ()
{
  int val = 0;
  gchar * str, * fsml;
  gchar * libdir;
#ifdef G_OS_WIN32
   libdir  = g_strdup_printf ("%s\\molecules\\%s", PACKAGE_LIB_DIR, family_dir[the_family]);
   gchar * libwin32 = g_strdup_printf ("%s\\molecules\\%s\\*.sml", PACKAGE_LIB_DIR, family_dir[the_family]);
   WIN32_FIND_DATA ffd;
   HANDLE hFind = FindFirstFile (libwin32, & ffd);
   if (hFind != INVALID_HANDLE_VALUE)
   {
     if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
     {
       str = g_strdup_printf ("%s\\%s", libdir, (char *)ffd.cFileName);
       if (open_sml_file(str, the_family) != NULL) val ++;
       g_free (str);
     }
     while (FindNextFile(hFind, &ffd) != 0)
     {
       if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
       {
         str = g_strdup_printf ("%s\\%s", libdir, (char *)ffd.cFileName);
         if (open_sml_file(str, the_family) != NULL) val ++;
         g_free (str);
       }
     }
     FindClose(hFind);
   }
#else
   libdir = g_strdup_printf ("%s/molecules/%s", PACKAGE_LIB_DIR, family_dir[the_family]);
   DIR * d;
   struct dirent * dir;
   d = opendir(libdir);
   if (d)
   {
     while ((dir = readdir(d)) != NULL)
     {
       if (dir -> d_type == DT_REG)
       {
         str = g_strdup_printf ("%s/%s", libdir, dir -> d_name);
         if (open_sml_file(str, the_family) != NULL) val ++;
         g_free (str);
       }
     }
     closedir(d);
   }
#endif
  if (val > 0)
  {
    sml_file_name = g_malloc0 (val*sizeof*sml_file_name);
    mol_name = g_malloc0 (val*sizeof*mol_name);
    val = 0;
#ifdef G_OS_WIN32
    hFind = FindFirstFile (libwin32, & ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
      if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
      {
        str = g_strdup_printf ("%s\\%s", libdir, (char *)ffd.cFileName);
        fsml = open_sml_file(str, the_family);
        if (fsml != NULL)
        {
          sml_file_name[val] = g_strdup_printf ("%s", str);
          mol_name[val] = g_strdup_printf ("%s", fsml);
          val ++;
          g_free (fsml);
        }
        g_free (str);
      }
      while (FindNextFile(hFind, &ffd) != 0)
      {
        if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
        {
          str = g_strdup_printf ("%s\\%s", libdir, (char *)ffd.cFileName);
          fsml = open_sml_file(str, the_family);
          if (fsml != NULL)
          {
            sml_file_name[val] = g_strdup_printf ("%s", str);
            mol_name[val] = g_strdup_printf ("%s", fsml);
            val ++;
            g_free (fsml);
          }
          g_free (str);
        }
      }
      FindClose(hFind);
    }
#else
    d = opendir (libdir);
    if (d)
    {
      while ((dir = readdir(d)) != NULL)
      {
        if (dir -> d_type == DT_REG)
        {
          str = g_strdup_printf ("%s/%s", libdir, dir -> d_name);
          fsml = open_sml_file(str, the_family);
          if (fsml != NULL)
          {
            sml_file_name[val] = g_strdup_printf ("%s", str);
            mol_name[val] = g_strdup_printf ("%s", fsml);
            val ++;
            g_free (fsml);
          }
          g_free (str);
        }
      }
      closedir(d);
    }
#endif
    sort_files (val);
  }
  return val;
}

/*!
  \fn void fill_molecule_tree (GtkListStore * store)

  \brief fill molecule list store

  \param store the list store to fill
*/
void fill_molecule_tree (GtkListStore * store)
{
  GtkTreeIter mol_level;
  int i;
  for (i=0; i<get_sml_files (); i++)
  {
    if (i == 0)
    {
      gtk_list_store_append (store, & first_mol_iter);
      gtk_list_store_set (store, & first_mol_iter, 0, -(i+1), 1, mol_name[i], -1);
    }
    else
    {
      gtk_list_store_append (store, & mol_level);
      gtk_list_store_set (store, & mol_level, 0, -(i+1), 1, mol_name[i], -1);
    }
  }
}

/*!
  \fn void fill_family_tree (GtkListStore * store)

  \brief fill molecular family list store

  \param store the list store to fill
*/
void fill_family_tree (GtkListStore * store)
{
  GtkTreeIter family_level;
  int i;
  for (i=0; i<FAMILY; i++)
  {
    if (i == 0)
    {
      gtk_list_store_append (store, & first_family_iter);
      gtk_list_store_set (store, & first_family_iter, 0, i, 1, family_list[i], -1);
    }
    else
    {
      gtk_list_store_append (store, & family_level);
      gtk_list_store_set (store, & family_level, 0, i, 1, family_list[i], -1);
    }
  }
}

/*!
  \fn void insert_preview ()

  \brief insert preview in library window and visualize
*/
void insert_preview ()
{
  gchar * str;
  lib_preview_plot = gtk_fixed_new ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, lib_preview_box, lib_preview_plot, FALSE, FALSE, 10);
  GtkWidget * grid = gtk_grid_new ();
  gtk_fixed_put (GTK_FIXED(lib_preview_plot), grid, 0, 10);
  gtk_widget_set_size_request (grid, -1, 200);
  gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
  gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
  gtk_grid_set_row_spacing (GTK_GRID (grid), 1);
  gtk_grid_set_column_spacing (GTK_GRID (grid), 1);

  gtk_grid_attach (GTK_GRID (grid), lib_proj -> modelgl -> plot, 0, 0, 4, 4);
  gtk_grid_attach (GTK_GRID (grid), markup_label("<i>Formula:</i>", 100, -1, 0.0, 0.5), 5, 1, 3, 1);
  int i;
  for (i=0; i<lib_proj -> nspec; i++)
  {
    if (i > 0)
    {
      str = g_strdup_printf ("%s <b>%s", str, lib_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("<b>%s", lib_proj -> chemistry -> label[i]);
    }
    if (lib_proj -> chemistry -> nsps[i] > 1)
    {
      str = g_strdup_printf ("%s<sub>%d</sub>", str, lib_proj -> chemistry -> nsps[i]);
    }
    str = g_strdup_printf ("%s</b>", str);
  }
  gtk_grid_attach (GTK_GRID (grid), markup_label(str, 100, -1, 0.0, 0.5), 8, 1, 3, 1);
  gtk_grid_attach (GTK_GRID (grid),markup_label("<i>Molecular mass:</i>", 100, -1, 0.0, 0.5), 5, 2, 3, 1);
  g_free (str);
  double mass = 0.0;
  for (i=0; i<lib_proj -> nspec; i++)
  {
    mass += (lib_proj -> chemistry -> nsps[i]*lib_proj -> chemistry -> chem_prop[CHEM_M][i]);
  }
  str = g_strdup_printf ("<b>%.3f g/mol</b>", mass);
  gtk_grid_attach (GTK_GRID (grid), markup_label(str, 100, -1, 0.0, 0.5), 8, 2, 3, 1);
  g_free (str);

  gtk_grid_attach (GTK_GRID (grid), markup_label("<i>IUPAC name:</i>", 100, -1, 0.0, 0.5), 0, 5, 3, 1);
  str = g_strdup_printf ("<b>%s</b>", check_xml_string(other_name[0]));
  gtk_grid_attach (GTK_GRID (grid), markup_label(str, 100, 30, 0.0, 0.5), 3, 5, 11, 1);

  if (o_names > 1)
  {
    GtkWidget * hbox = create_hbox (0);
    gtk_fixed_put (GTK_FIXED(lib_preview_plot), hbox, 0, 230);
    GtkWidget * vbox = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<i>Other name(s):</i>", 100, 30, 0.0, 0.5), FALSE, FALSE, 0);
    for (i=1; i<o_names; i++)
    {
      if (i > 1)
      {
        str = g_strdup_printf ("%s\n<b>%s</b>", str, check_xml_string(other_name[i]));
      }
      else
      {
        str = g_strdup_printf ("<b>%s</b>", check_xml_string(other_name[i]));
      }
    }
    vbox = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbox, FALSE, FALSE, 15);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label(str, 100, 30+20*(o_names-2), 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);
  }
  show_the_widgets (lib_preview_box);
}

/*!
  \fn void prepare_preview (int active, int id, gboolean visible)

  \brief prepare library molecule preview

  \param active active project id
  \param id molecule id number
  \param visible is the 'model edition' window visible
*/
void prepare_preview (int active, int id, gboolean visible)
{
  if (sml_file_name != NULL)
  {
    if (sml_preview (sml_file_name[id]))
    {
      lib_preview_plot = destroy_this_widget (lib_preview_plot);
      lib_proj -> run = TRUE;
      active_project_changed (lib_proj -> id);
      create_3d_model (lib_proj -> id, FALSE);
      if (visible)
      {
        insert_preview ();
      }
      else
      {
        on_realize (NULL, lib_proj -> modelgl);
      }
      bonds_update = 0;
      active_glwin = lib_proj -> modelgl;
      active_glwin -> init = FALSE;
      active_image = active_glwin -> anim -> last -> img;
      lib_proj  -> runc[0] = TRUE;
      on_calc_bonds_released (NULL, NULL);
      lib_proj -> modelgl -> anim -> last -> img -> quality = 30;
      lib_proj -> modelgl -> anim -> last -> img -> rep = ORTHOGRAPHIC;
      lib_proj -> modelgl -> anim -> last -> img -> xyz -> axis = NONE;
      lib_proj -> modelgl -> anim -> last -> img -> abc -> box = NONE;
      if (visible)
      {
        gtk_widget_set_size_request (lib_proj -> modelgl -> plot, 150, 150);
        update (lib_proj -> modelgl);
      }
      active_project_changed (active);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void select_library_data (GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer data)

  \brief select library element callback

  \param tree_view the GtkTreeView sending the signal
  \param path the path in the tree view
  \param column the column in the tree view
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_library_data (GtkTreeView * tree_view, GtkTreePath * path, GtkTreeViewColumn * column, gpointer data)
{
  GtkTreeIter row;
  GtkTreeModel * model = gtk_tree_view_get_model(tree_view);
  if (gtk_tree_model_get_iter (model, & row, path))
  {
    GValue val = {0, };
    GValue vbl = {0, };
    gtk_tree_model_get_value (model, & row, 0, & val);
    gtk_tree_model_get_value (model, & row, 1, & vbl);
    int i = (int)g_value_get_int (& val);
    if (i > -1)
    {
      the_family = i;
      gtk_list_store_clear (molecule_store);
      fill_molecule_tree (molecule_store);
      gtk_tree_selection_select_iter (libselect[1], & first_mol_iter);
      prepare_preview (activep, 0, TRUE);
    }
    else
    {
      the_molecule = g_strdup_printf ("%s", (char *)g_value_get_string (& vbl));
      prepare_preview (activep, -i-1, TRUE);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_library_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set font markup in the molecular library tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_library_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  gchar * str = NULL;
  gtk_tree_model_get (mod, iter, 1, & str, -1);
  g_object_set (renderer, "markup", str, NULL, NULL);
  g_free (str);
}

/*!
  \fn GtkWidget * library_tree (GtkListStore * store, int id, gchar * name)

  \brief create library tree store widget

  \param store the list store model to use
  \param id selection id (0 = family, 1 = molecule)
  \param name column label
*/
GtkWidget * library_tree (GtkListStore * store, int id, gchar * name)
{
  GtkWidget * scrol = create_scroll (NULL, 150, 300, GTK_SHADOW_ETCHED_IN);
  GtkTreeViewColumn * datacol;
  GtkCellRenderer * datacel;
  GtkWidget * dataview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(store));
  datacel = gtk_cell_renderer_text_new ();
  datacol = gtk_tree_view_column_new_with_attributes (name, datacel, "text", 1, NULL);
  gtk_tree_view_column_set_cell_data_func (datacol, datacel, set_library_markup, NULL, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(dataview), datacol);
  gtk_tree_view_column_set_alignment (datacol, 0.5);
  gtk_tree_view_set_activate_on_single_click (GTK_TREE_VIEW(dataview), TRUE);
  g_signal_connect (G_OBJECT(dataview), "row-activated", G_CALLBACK(select_library_data), NULL);
  g_object_unref (store);
  libselect[id] = gtk_tree_view_get_selection (GTK_TREE_VIEW(dataview));
  gtk_tree_selection_set_mode (libselect[id], GTK_SELECTION_SINGLE);
  add_container_child (CONTAINER_SCR, scrol, dataview);
  return scrol;
}

gboolean lib_res;
gboolean lib_visible;

/*!
  \fn G_MODULE_EXPORT void run_select_from_library (GtkDialog * lib, gint response_id, gpointer data)

  \brief select from library - running the dialog

  \param lib the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_select_from_library (GtkDialog * lib, gint response_id, gpointer data)
{
  gboolean done = FALSE;
  atom_search * asearch = (atom_search *)data;
  project * this_proj = get_project_by_id (asearch -> proj);
  GtkWidget * vbox;
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      vbox = dialog_get_content_area ((GtkWidget *)lib);
      widget_set_sensitive (vbox, FALSE);
      if (this_proj -> modelgl) this_proj -> modelgl -> other_status = 2;
      create_object_from_library (lib_proj -> id);
      active_project_changed (activep);
      if (asearch -> action == INSERT)
      {
        to_insert_in_project (FROM_LIBRARY, -1, this_proj, asearch, lib_visible);
        if (! lib_visible)
        {
          gboolean vis;
          if (this_proj -> modelgl)
          {
            vis = (this_proj -> modelgl -> atom_win) ? this_proj -> modelgl -> atom_win -> visible : FALSE;
          }
          else
          {
            vis = FALSE;
          }
          inserted_from_lib += action_atoms_from_project (this_proj, asearch, vis);
        }
        if (this_proj -> modelgl) this_proj -> modelgl -> nth_copy ++;
      }
      lib_res = FROM_LIBRARY;
      widget_set_sensitive (vbox, TRUE);
      if (asearch -> action == REPLACE) done = TRUE;
      break;
    default:
      if (this_proj -> modelgl)
      {
        if (this_proj -> modelgl -> mode == EDITION) asearch -> todo[0] = 0;
      }
      lib_res = 0;
      done = TRUE;
      break;
  }
  if (done) destroy_this_dialog (lib);
}

/*!
  \fn int select_from_library (gboolean visible, project * this_proj, atom_search * asearch)

  \brief select object to insert from the library

  \param visible is the 'model edition' window visible
  \param this_proj the target project
  \param asearch the target atom search
*/
int select_from_library (gboolean visible, project * this_proj, atom_search * asearch)
{
  int active = activep;
  lib_visible = visible;
  GtkWidget * lib = dialogmodal ("Library", GTK_WINDOW((this_proj -> modelgl) ? this_proj -> modelgl -> win : MainWindow));
#ifdef GTK3
#ifdef GTKGLAREA
#ifndef OSX
#ifndef G_OS_WIN32
  if (! atomes_visual) gtk_window_change_gdk_visual (lib);
#endif // G_OS_WIN32
#endif // OSX
#endif // GTKGLAREA
#endif // GTK3
  gtk_dialog_add_button (GTK_DIALOG(lib), (asearch -> action == REPLACE) ? "Replace" : "Insert", GTK_RESPONSE_APPLY);
  GtkWidget * vbox = dialog_get_content_area (lib);
  GtkWidget * hbox = create_hbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  sml_file_name = mol_name = NULL;
  family_store = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);
  fill_family_tree (family_store);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, library_tree (family_store, 0, "Family"), FALSE, FALSE, 0);
  molecule_store = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, library_tree (molecule_store, 1, "Molecule"), FALSE, FALSE, 0);
  lib_preview_box = create_hbox(0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lib_preview_box, FALSE, FALSE, 0);
  lib_preview_plot = NULL;
  the_family = 0;
  gtk_tree_selection_select_iter (libselect[0], & first_family_iter);
  gtk_list_store_clear (molecule_store);
  fill_molecule_tree (molecule_store);
  gtk_tree_selection_select_iter (libselect[1], & first_mol_iter);
  show_the_widgets (lib);
  prepare_preview (active, 0, TRUE);
  if (this_proj -> modelgl) this_proj -> modelgl -> nth_copy = 0;
  inserted_from_lib = 0;
  run_this_gtk_dialog (lib, G_CALLBACK(run_select_from_library), asearch);
  active_project_changed (active);
  if (sml_file_name != NULL) g_free (sml_file_name);
  if (mol_name != NULL) g_free (mol_name);
  if (lib_proj != NULL) close_project (lib_proj);
  lib_proj = NULL;
  lib_preview_plot = NULL;
  return lib_res;
}

/*!
  \fn int insert_this_project_from_lib (int id, gboolean visible, project * this_proj, atom_search * asearch)

  \brief insert object from the library

  \param id object id to insert
  \param visible is the 'model edition window' visible
  \param this_proj the target project
  \param asearch the target atom search
*/
int insert_this_project_from_lib (int id, gboolean visible, project * this_proj, atom_search * asearch)
{
  sml_file_name = mol_name = NULL;
  int family[6] = {0, 3, 9, 9, 15, 17};
  int molec[6] = {0, 8, 36, 21, 11, 0};
  int active = activep;
  the_family = family[id];
  if (get_sml_files ())
  {
    prepare_preview (active, molec[id], FALSE);
    if (this_proj -> modelgl) this_proj -> modelgl -> other_status = 2;
    create_object_from_library (lib_proj -> id);
    if (asearch -> action == INSERT)
    {
      to_insert_in_project (FROM_LIBRARY, -1, this_proj, asearch, visible);
    }
  }
  active_project_changed (active);
  if (sml_file_name != NULL) g_free (sml_file_name);
  if (mol_name != NULL) g_free (mol_name);
  if (lib_proj != NULL) close_project (lib_proj);
  lib_proj = NULL;
  return FROM_LIBRARY;
}
