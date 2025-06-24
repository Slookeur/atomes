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
 - The associated controlers

*
* List of functions:



*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"
#include "preferences.h"
#include <libxml/xmlreader.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

extern xmlNodePtr findnode (xmlNodePtr startnode, char * nname);
extern int search_type;
extern void calc_rings (GtkWidget * vbox);

gchar * default_delta_num_leg[7] = {"<b>g(r)</b>: number of &#x3b4;r", "<b>s(q)</b>: number of &#x3b4;q", "<b>s(k)</b>: number of &#x3b4;k", "<b>g(r) FFT</b>: number of &#x3b4;r",
                                    "<b>D<sub>ij</sub></b>: number of &#x3b4;r [D<sub>ij</sub>min-D<sub>ij</sub>max]", "<b>Angles distribution</b>: number of &#x3b4;&#x3b8; [0-180°]",  "<b>Spherical harmonics</b>: l<sub>max</sub> in [2-40]"};
int default_num_delta[7];          /*!< Number of x points: \n 0 = gr, \n 1 = sq, \n 2 = sk, \n 3 = gftt, \n 4 = bd, \n 5 = an, \n 6 = sp */

gchar * default_ring_param[7] = {"Default search",
                                 "Atom(s) to initiate the search from",
                                 " <i><b>n</b><sub>max</sub></i> = maximum size for a ring <sup>*</sup>",
                                 "Maximum number of rings of size <i><b>n</b></i> per MD step<sup>**</sup>",
                                 "Only search for ABAB rings",
                                 "No homopolar bonds in the rings (A-A, B-B ...) <sup>***</sup>",
                                 "No homopolar bonds in the connectivity matrix"};
int default_rsparam[7];            /*!< Ring statistics parameters: \n
                                        0 = Default search, \n
                                        1 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                        2 = Maximum size of ring for the search Rmax, \n
                                        3 = Maximum number of ring(s) per MD step NUMA, \n
                                        4 = Search only for ABAB rings or not, \n
                                        5 = Include Homopolar bond(s) in the analysis or not, \n
                                        6 = Include homopolar bond(s) when calculating the distance matrix */


// 0 = Initnode, 1 = RMAX, 2 = CNUMAn 3 = AAAA, 4 = ABAB, 5 = Homo, 6 = 1221
gchar * default_chain_param[7] = {"Atom(s) to initiate the search from",
                                  "<i><b>n</b><sub>max</sub></i> = maximum size for a chain <sup>*</sup>",
                                  "Maximum number of chains of size <i><b>n</b></i> per MD step <sup>**</sup>",
                                  "Only search for AAAA chains",
                                  "Only search for ABAB chains",
                                  "No homopolar bonds in the chains (A-A, B-B ...) <sup>***</sup>",
                                  "Only search for 1-(2)<sub>n</sub>-1 chains, ie. isolated chains"};
int default_csparam[7];             /*!< Chain statistics parameters: \n
                                         0 = Initial node(s) for the search: selected chemical species or all atoms, \n
                                         1 = Maximum size for a chain Cmax, \n
                                         2 = Maximum number of chain(s) per MD step CNUMA, \n
                                         3 = Search only for AAAA chains or not, \n
                                         4 = Search only for ABAB chains or not, \n
                                         5 = Include Homopolar bond(s) in the analysis or not, \n
                                         6 = Search only for 1-(2)n-1 chains */

gboolean preferences = FALSE;

void set_parameter (double value, gchar * code_var, int id)
{
  if (g_strcmp0(code_var, "default_num_delta") == 0)
  {
    default_num_delta[id] = (int)value;
  }
  else if (g_strcmp0(code_var, "default_rsparam") == 0)
  {
    default_rsparam[id] = (int)value;
  }
  else if (g_strcmp0(code_var, "default_csparam") == 0)
  {
    default_csparam[id] = (int)value;
  }
}

/*!
  \fn void read_analysis_preferences (xmlNodePtr analysis_node)

  \brief read analysis preferences from XML configuration

  \param analysis_node the XML node that point to analysis preferences
*/
void read_analysis_preferences (xmlNodePtr analysis_node)
{
  xmlNodePtr node, p_node;
  xmlAttrPtr p_details;
  gboolean set_codevar, set_id;
  gchar * code_var;
  int id;
  double value;
  node = findnode (analysis_node  -> children, "parameter");
  while (node)
  {
    value = string_to_double ((gpointer)xmlNodeGetContent(node));
    p_details = node -> properties;
    set_codevar = set_id = FALSE;
    while (p_details)
    {
      p_node = p_details -> children;
      if (p_node)
      {
        if (g_strcmp0("code_var",(char *)p_details -> name) == 0)
        {
          code_var = g_strdup_printf ("%s", xmlNodeGetContent(p_node));
          set_codevar = TRUE;
        }
        if (g_strcmp0("id",(char *)p_details -> name) == 0)
        {
          id = (int) string_to_double ((gpointer)xmlNodeGetContent(p_node));
          set_id = TRUE;
        }
      }
      p_details = p_details -> next;
    }
    if (set_codevar && set_id)
    {
      set_parameter (value, code_var, id);
    }
    node = node -> next;
    node = findnode (node, "parameter");
  }
}

/*!
  \fn void read_preferences_from_xml_configuration ()

  \brief read software preferences from XML configuration
*/
void read_preferences_from_xml_configuration ()
{
#ifdef G_OS_WIN32
  ATOMES_CONFIG = g_build_filename (PACKAGE_PREFIX, "atomes.cfg", NULL);
#else
  struct passwd * pw = getpwuid(getuid());
  ATOMES_CONFIG = g_strdup_printf ("%s/.config/atomes/atomes.cfg", pw -> pw_dir);
#endif
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
          read_analysis_preferences (node);
        }
        node = findnode(racine -> children, "opengl");
        if (node)
        {

        }
        node = findnode(racine -> children, "model");
        if (node)
        {

        }
      }
      xmlFreeDoc(doc);
      xmlCleanupParser();
    }
    xmlFreeTextReader(reader);
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
}

/*!
  \fn void set_atomes_preferences ()

  \brief set software default parameters
*/
void set_atomes_preferences ()
{
  restore_defaults_parameters (NULL, NULL);
  read_preferences_from_xml_configuration ();
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
  \fn GtkWidget * opengl_preferences ()

  \brief OpenGL preferences
*/
GtkWidget * opengl_preferences ()
{
  GtkWidget * vbox = create_vbox (BSEP);

  return vbox;
}

/*!
  \fn G_MODULE_EXPORT void set_default_num_delta (GtkEntry * res, gpointer data)

  \brief update default num delta preferences

  \param res the GtkEntry the signal is coming from
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_default_num_delta (GtkEntry * res, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  default_num_delta[i] = (int) string_to_double ((gpointer)m);
  update_entry_int (res, default_num_delta[i]);
}

/*!
  \fn G_MODULE_EXPORT void set_default_rs (GtkEntry * res, gpointer data)

  \brief update default ring statistics preferences

  \param res the GtkEntry the signal is coming from
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_default_rs (GtkEntry * res, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  default_rsparam[i] = (int) string_to_double ((gpointer)m);
  update_entry_int (res, default_rsparam[i]);
}

/*!
  \fn G_MODULE_EXPORT void set_default_cs (GtkEntry * res, gpointer data)

  \brief update default chain statistics preferences

  \param res the GtkEntry the signal is coming from
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_default_cs (GtkEntry * res, gpointer data)
{
  int i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  default_csparam[i] = (int) string_to_double ((gpointer)m);
  update_entry_int (res, default_csparam[i]);
}

/*!
  \fn G_MODULE_EXPORT void ring_combox_changed (GtkComboBox * box, gpointer data)

  \brief change ring statistics calculation preference

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void ring_combox_changed (GtkComboBox * box, gpointer data)
{
  int i = gtk_combo_box_get_active(box);
  int j = GPOINTER_TO_INT(data);
  default_rsparam[j] = i;
}

/*!
  \fn G_MODULE_EXPORT void chain_combox_changed (GtkComboBox * box, gpointer data)

  \brief change chain statistics calculation preference

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void chain_combox_changed (GtkComboBox * box, gpointer data)
{
  int i = gtk_combo_box_get_active(box);
  default_csparam[0] = i;
}

/*!
  \fn GtkWidget * combox_ring_chain_pref (gchar * str, int num, gchar * list_item[num], int val, int cid)

  \brief create a combo box for the ring / chain statistics parameters

  \param str label of the combo box
  \param num number of values to insert in the combo box
  \param list_item text data to insert in the combo boc
  \param val active value for the combo box
  \param cid 0 = rings, 1 = chains
*/
GtkWidget * combox_ring_chain_pref (gchar * str, int num, gchar * list_item[num], int val, int cid)
{
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (str, 350, 40, 0.0, 0.5), FALSE, FALSE, 15);
  GtkWidget * fixed = gtk_fixed_new ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, FALSE, FALSE, 0);
  GtkWidget * combo = create_combo ();
  int i;
  for (i=0; i<num; i++) combo_text_append (combo, list_item[i]);
  gtk_fixed_put (GTK_FIXED(fixed), combo, -1, 5);
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo), val);
  switch (cid)
  {
    case 2:
      g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(chain_combox_changed), NULL);
      break;
    default:
      g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ring_combox_changed), GINT_TO_POINTER(cid));
      break;
  }
  return hbox;
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
  //gchar * str;
  int i;
  for (i=0; i<7; i++)
  {
    hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (default_delta_num_leg[i], 350, -1, 0.0, 0.5), FALSE, FALSE, 15);
    entry = create_entry (G_CALLBACK(set_default_num_delta), 150, 15, TRUE, GINT_TO_POINTER(i));
    update_entry_int ((GtkEntry *)entry, default_num_delta[i]);
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
  \fn void create_configuration_dialog ()

  \brief create the configuration window
*/
void create_user_preferences_dialog ()
{
  // General prefs
  /*
  - default radius to be used
  - default X-ray scattering method

  - calculations:
    - default dr for g(r)
    - dq for s(q)
    - dk for s(k)
    - dr for g(r)

    - default dr for distances
    - default d° for angles
    - default search for molecules and frag
    - default output in file + file name

    - default ring definition to be used
    - default atom to start the search (all or
    - default max ring size
    - default NUMA ring
    - default search ABAB, no homo in rings, no homo in matrix

    - default max chain size
    - default NUMA chain
    - default only AAAA, ABAB, no homo in chains,

    - delta t for dynamics + unit
    - Steps between configurations
  */
  // OpenGL prefs
  /*

  - Default style
  - Default color scheme: atoms, poly
  - Default quality
  - Default lightning model
  - Default material (or template) : with all parameters
  - Default number of light sources, light types
  - Default fog mode and other options
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
  GtkWidget * win = create_win ("User preferences", MainWindow, TRUE, FALSE);
  preferences = TRUE;
  GtkWidget * vbox = create_vbox (5);
  add_container_child (CONTAINER_WIN, win, vbox);
  gtk_widget_set_size_request (win, 625, 600);
  GtkWidget * notebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
  show_the_widgets (notebook);
  gtk_widget_set_size_request (notebook, 600, 550);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, notebook, FALSE, FALSE, 0);
  GtkWidget * gbox = create_vbox (BSEP);
  gchar * mess = "Browse the following to modify the default configuration of <b>atomes</b>\n"
                 "by replacing internal parameters by user defined preferences.\n\n"
                 "\t<b>Analysis</b> : calculation preferences\n"
                 "\t<b>OpenGL</b> : rendering preferences\n"
                 "\t<b>Model</b> : atom(s), bond(s) and box preferences\n"
                 "\t<b>View</b> : representation and projection preferences";
  add_box_child_start (GTK_ORIENTATION_VERTICAL, gbox, markup_label (mess, -1, -1, 0.5, 0.5), FALSE, FALSE, 20);
  GtkWidget * hbox = create_hbox (BSEP);
  GtkWidget * but = create_button ("Restore all default parameters", IMG_NONE, NULL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(restore_defaults_parameters), NULL);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 20);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, gbox, hbox, FALSE, FALSE, 0);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), gbox, gtk_label_new ("General"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), calc_preferences(), gtk_label_new ("Analysis"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), opengl_preferences(), gtk_label_new ("OpenGL"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), model_preferences(), gtk_label_new ("Model"));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), view_preferences(), gtk_label_new ("View"));
  show_the_widgets (win);
  preferences = FALSE;
}
