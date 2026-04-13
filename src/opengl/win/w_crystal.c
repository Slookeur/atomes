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
* @file w_crystal.c
* @short Functions to create a crystal database window \n
         This is a project and not in use for the time being
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_crystal.c'
*
* Contains:
*

 - The functions to create a crystal database window
 - This is a project and not in use for the time being

*
* List of functions:

  int cif_preview (const char * filetoread);
  int get_cgroup (gchar * str);
  int get_cif_files ();
  int get_num_cif ();
  int get_num_group ();
  int get_cif_info_from_cif_node (xmlNodePtr * cnode);
  int open_cif_database (gchar * filetoread);
  int prepare_data_base (int db);
  int build_crystal_from_cif_database (project * this_proj);

  void sort_crystal_files (int num_c);
  void fill_cif_tree (GtkListStore * store);
  void fill_group_tree (GtkListStore * store);
  void fill_symmetry_tree (GtkListStore * store);
  void fill_database_tree (GtkListStore * store);
  void insert_cif_preview ();
  void prepare_cif_preview (int id);
  void prepare_cif_list (int dba, int sym, int spg);

  GtkWidget * cif_tree (GtkListStore * store, int id, gchar * name);

*/

/* Crystal database window, project not in use for the time being */

#include "global.h"
#include "interface.h"
#include "project.h"
#include "glview.h"
#include "glwindow.h"
#include "bind.h"

#define DATABASE 1
gchar * database_list[DATABASE] = {"American Mineralogist Crystal Structure Database"};
                                    /*"Crystallography Open Database",
                                    "Database of Zeolite Structures"};*/
gchar * database_dir[DATABASE] = {"AMCSD"};//, "COD", "DZC"};


#define SYMMETRIES 8
gchar * sym_list[SYMMETRIES]= {"Triclinic", "Monoclinic", "Orthorhombic", "Tetragonal", "Trigonal", "Hexagonal"," Cubic", "Other"};


extern G_MODULE_EXPORT void on_calc_bonds_released (GtkWidget * widg, gpointer data);
#ifdef GTK3
#ifndef G_OS_WIN32
extern void gtk_window_change_gdk_visual (GtkWidget * win);
#endif // G_OS_WIN32
#endif // GTK3
extern gboolean create_3d_model (int p, gboolean load);
extern G_MODULE_EXPORT void on_realize (GtkGLArea * area, gpointer data);
extern void init_camera (project * this_proj);
extern void alloc_proj_data (project * this_proj, int cid);
extern int action_atoms_from_project (project * this_proj, atom_search * asearch, int status, gboolean visible);
extern void to_insert_in_project (int stat, int orig, project * this_proj, atom_search * asearch, gboolean visible);
extern void create_object_from_library (int p);
GtkListStore * database_store;
GtkTreeIter first_database_iter;
GtkListStore * symmetry_store;
GtkTreeIter first_symmetry_iter;
GtkListStore * group_store;
GtkTreeIter first_group_iter;
GtkListStore * cif_store;
GtkTreeIter first_cif_iter;
GtkTreeSelection * cifselect[3];
int the_database;
int the_symmetry;
int the_group;
gchar * the_crystal;
gchar ** cif_file_name;
gchar ** cif_name;
GtkWidget * cif_preview_box = NULL;
GtkWidget * cif_preview_plot = NULL;
project * cif_proj = NULL;
gchar * other_name[5];
int o_names;

/*!
  \fn int cif_preview (const char * filetoread)

  \brief NOT USED !

  \param filetoread
*/
int cif_preview (const char * filetoread)
{
  int i, j, k;
  if (cif_proj != NULL) close_project (cif_proj);
  init_project (TRUE);
  cif_proj = active_project;
  cif_proj -> coordfile = g_strdup_print ("%s", filetoread);
  if (open_coordinate_file (6))
  {
    // Cif error
    return 0;
  }
  return 1;
}

/*!
  \fn int get_cgroup (gchar * str)

  \brief NOT USED !

  \param str
*/
int get_cgroup (gchar * str)
{
  int i;
  for (i=0; i<CFAMILY; i++)
  {
    if (strcmp(str,cgroup_list[i]) == 0) return i;
  }
  return -1;
}

/*!
  \fn void sort_crystal_files (int num_c)

  \brief NOT USED !

  \param num_c number of file(s) to sort
*/
void sort_crystal_files (int num_c)
{
  int i, j;
  gchar * str = NULL;
  for(i=0;i<num_c;i++)
  {
    for(j=i+1;j<num_c;j++)
    {
       if(strcmp(cif_name[i],cif_name[j])>0)
       {
         str = g_strdup_printf ("%s", cif_name[i]);
         cif_name[i] = g_strdup_printf ("%s", cif_name[j]);
         cif_name[j] = g_strdup_printf ("%s", str);
         str = g_strdup_printf ("%s", cif_file_name[i]);
         cif_file_name[i] = g_strdup_printf ("%s", cif_file_name[j]);
         cif_file_name[j] = g_strdup_printf ("%s", str);
       }
    }
  }
  for(i=0;i<num_c;i++)
  {
    cif_name[i] = check_xml_string(cif_name[i]);
  }
}

/*!
  \fn int get_cif_files ()

  \brief NOT USED !
*/
int get_cif_files ()
{
  int val = 0;
  gchar * str, * fcif;
  gchar * libdir;
  gchar * search_dir;
#ifdef G_OS_WIN32
   search_dir =  = g_strdup_printf ("%s\\%s\\%s", database_dir[the_database], sym_dir[the_symmetry], group_dir[the_group]);
   libdir  = g_strdup_printf ("%s\\crystals\\%s", PACKAGE_LIB_DIR, search_dir);
   gchar * libwin32 = g_strdup_printf ("%s\\crystals\\%s\\*.cif", PACKAGE_LIB_DIR, search_dir);
   WIN32_FIND_DATA ffd;
   HANDLE hFind = FindFirstFile (libwin32, & ffd);
   if (hFind != INVALID_HANDLE_VALUE)
   {
     if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
     {
       str = g_strdup_printf ("%s\\%s", libdir, (char *)ffd.cFileName);
       if (open_cif_file(str) != NULL) val ++;
       g_free (str);
     }
     while (FindNextFile(hFind, &ffd) != 0)
     {
       if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
       {
         str = g_strdup_printf ("%s\\%s", libdir, (char *)ffd.cFileName);
         if (open_cif_file(str) != NULL) val ++;
         g_free (str);
       }
     }
     FindClose(hFind);
   }
#else
   search_dir =  = g_strdup_printf ("%s/%s/%s", database_dir[the_database], sym_dir[the_symmetry], group_dir[the_group]);
   libdir =g_strdup_printf ("%s/crystals/%s", PACKAGE_LIB_DIR, search_dir);
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
         if (open_cif_file(str) != NULL) val ++;
         g_free (str);
       }
     }
     closedir(d);
   }
#endif
  if (val > 0)
  {
    cif_file_name = calloc(val, sizeof*cif_file_name);
    cif_name = calloc(val, sizeof*cif_name);
    val = 0;
#ifdef G_OS_WIN32
    hFind = FindFirstFile (libwin32, & ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
      if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
      {
        str = g_strdup_printf ("%s\\%s", libdir, (char *)ffd.cFileName);
        fcif = open_cif_file(str);
        if (fcif != NULL)
        {
          cif_file_name[val] = g_strdup_printf ("%s", str);
          cif_name[val] = g_strdup_printf ("%s", fcif);
          val ++;
          g_free (fcif);
        }
        g_free (str);
      }
      while (FindNextFile(hFind, &ffd) != 0)
      {
        if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
        {
          str = g_strdup_printf ("%s\\%s", libdir, (char *)ffd.cFileName);
          fcif = open_cif_file(str);
          if (fcif != NULL)
          {
            cif_file_name[val] = g_strdup_printf ("%s", str);
            cif_name[val] = g_strdup_printf ("%s", fcif);
            val ++;
            g_free (fcif);
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
          fcif = open_cif_file(str);
          if (fcif != NULL)
          {
            cif_file_name[val] = g_strdup_printf ("%s", str);
            cif_name[val] = g_strdup_printf ("%s", fcif);
            val ++;
            g_free (fcif);
          }
          g_free (str);
        }
      }
      closedir(d);
    }

#endif
    sort_crystal_files (val);
  }
  return val;
}

/*!
  \fn int get_num_cif ()

  \brief NOT USED !
*/
int get_num_cif ()
{
  return num_cif[the_symmetry][the_group];
}

/*!
  \fn int get_num_group ()

  \brief NOT USED !
*/
int get_num_group ()
{
  return num_group[the_symmetry];
}

/*!
  \fn void fill_cif_tree (GtkListStore * store)

  \brief NOT USED !

  \param store
*/
void fill_cif_tree (GtkListStore * store)
{
  GtkTreeIter cif_level;
  int i;
  for (i=0; i<get_num_cif(); i++)
  {
    if (i == 0)
    {
      gtk_list_store_append (store, & first_cif_iter);
      gtk_list_store_set (store, & first_cif_iter, 0, i, 1, 2, 3, cif_name[i], -1);
    }
    else
    {
      gtk_list_store_append (store, & cif_level);
      gtk_list_store_set (store, & cif_level, 0, i, 1, 2, 3, cif_name[i], -1);
    }
  }
}

/*!
  \fn void fill_group_tree (GtkListStore * store)

  \brief NOT USED !

  \param store
*/
void fill_group_tree (GtkListStore * store)
{
  GtkTreeIter group_level;
  int i;
  for (i=0; i<get_num_group(); i++)
  {
    if (i == 0)
    {
      gtk_list_store_append (store, & first_group_iter);
      gtk_list_store_set (store, & first_group_iter, 0, i, 1, 2, 2, group_list[i], -1);
    }
    else
    {
      gtk_list_store_append (store, & group_level);
      gtk_list_store_set (store, & group_level, 0, i, 1, 2, 2, group_list[i], -1);
    }
  }
}

/*!
  \fn void fill_symmetry_tree (GtkListStore * store)

  \brief NOT USED !

  \param store
*/
void fill_symmetry_tree (GtkListStore * store)
{
  GtkTreeIter symmetry_level;
  int i;
  for (i=0; i<SYMMETRIES; i++)
  {
    if (i == 0)
    {
      gtk_list_store_append (store, & first_symmetry_iter);
      gtk_list_store_set (store, & first_symmetry_iter, 0, i, 1, 1, 2, symmetry_list[i], -1);
    }
    else
    {
      gtk_list_store_append (store, & symmetry_level);
      gtk_list_store_set (store, & symmetry_level, 0, i, 1, 1, 2, symmetry_list[i], -1);
    }
  }
}

/*!
  \fn void fill_database_tree (GtkListStore * store)

  \brief NOT USED !

  \param store
*/
void fill_database_tree (GtkListStore * store)
{
  GtkTreeIter database_level;
  int i;
  for (i=0; i<CDATABASE; i++)
  {
    if (i == 0)
    {
      gtk_list_store_append (store, & first_database_iter);
      gtk_list_store_set (store, & first_database_iter, 0, i, 1, 0, 2, database_list[i], -1);
    }
    else
    {
      gtk_list_store_append (store, & database_level);
      gtk_list_store_set (store, & database_level, 0, i, 1, 0, 2, database_list[i], -1);
    }
  }
}

/*!
  \fn void insert_cif_preview ()

  \brief NOT USED !
*/
void insert_cif_preview ()
{
  gchar * str;
  cif_preview_plot = gtk_fixed_new ();
  gtk_box_pack_start (GTK_BOX(cif_preview_box), cif_preview_plot, FALSE, FALSE, 10);
  GtkWidget * grid = gtk_grid_new ();
  gtk_fixed_put (GTK_FIXED(cif_preview_plot), grid, 0, 10);
  gtk_widget_set_size_request (grid, -1, 200);
  gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
  gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
  gtk_grid_set_row_spacing (GTK_GRID (grid), 1);
  gtk_grid_set_column_spacing (GTK_GRID (grid), 1);

  gtk_grid_attach (GTK_GRID (grid), cif_proj -> modelgl -> plot, 0, 0, 4, 4);
  gtk_grid_attach (GTK_GRID (grid), markup_label("<i>Formula:</i>", 100, -1, 0.0, 0.5), 5, 1, 3, 1);
  int i;
  for (i=0; i<cif_proj -> nspec; i++)
  {
    if (i > 0)
    {
      str = g_strdup_printf ("%s <b>%s", str, cif_proj -> chemistry -> label[i]);
    }
    else
    {
      str = g_strdup_printf ("<b>%s", cif_proj -> chemistry -> label[i]);
    }
    if (cif_proj -> chemistry -> nsps[i] > 1)
    {
      str = g_strdup_printf ("%s<sub>%d</sub>", str, cif_proj -> chemistry -> nsps[i]);
    }
    str = g_strdup_printf ("%s</b>", str);
  }
  gtk_grid_attach (GTK_GRID (grid), markup_label(str, 100, -1, 0.0, 0.5), 8, 1, 3, 1);
  g_free (str);
  gtk_grid_attach (GTK_GRID (grid),markup_label("<i>Density:</i>", 100, -1, 0.0, 0.5), 5, 2, 3, 1);
  str = g_strdup_printf ("<b>%.3f g/cm<sup>3</sup></b>", cif_proj -> density);
  gtk_grid_attach (GTK_GRID (grid), markup_label(str, 100, -1, 0.0, 0.5), 8, 2, 3, 1);
  g_free (str);

  gtk_grid_attach (GTK_GRID (grid), markup_label("<i>Name:</i>", 100, -1, 0.0, 0.5), 0, 5, 3, 1);
  str = g_strdup_printf ("<b>%s</b>", );
  gtk_grid_attach (GTK_GRID (grid), markup_label(str, 100, 30, 0.0, 0.5), 3, 5, 11, 1);

  show_the_widgets_all (cif_preview_box);
}

/*!
  \fn void prepare_cif_preview (int id)

  \brief NOT USED !

  \param id
*/
void prepare_cif_preview (int id)
{
  if (cif_file_name != NULL)
  {
    if (cif_preview (cif_file_name[id]))
    {
      if (cif_preview_plot != NULL) gtk_widget_destroy (cif_preview_plot);
      cif_proj -> run = TRUE;
      active_project_changed (cif_proj -> id);
      create_3d_model (cif_proj -> id, FALSE);
      insert_cif_preview ();
      bonds_update = 0;
      active_glwin = cif_proj -> modelgl;
      active_glwin -> init = FALSE;
      active_image = & active_glwin -> anim -> last -> img;
      on_calc_bonds_released (NULL, NULL);
      cif_proj -> modelgl -> anim -> last -> img.quality = 30;
      cif_proj -> modelgl -> anim -> last -> img.rep = ORTHOGRAPHIC;
      gtk_widget_set_size_request (cif_proj -> modelgl -> plot, 150, 150);
      update (cif_proj -> modelgl);
    }
  }
}

/*!
  \fn int get_cif_info_from_cif_node (xmlNodePtr * cnode)

  \brief NOT USED !

  \param cnode
*/
int get_cif_info_from_cif_node (xmlNodePtr * cnode)
{

  return 1;
}

/*!
  \fn void prepare_cif_list (int dba, int sym, int spg)

  \brief NOT USED !

  \param dba
  \param sym
  \param spg
*/
void prepare_cif_list (int dba, int sym, int spg)
{
#ifdef G_OS_WIN32
  data_xml = g_strdup_printf ("%s\\crystals\\%s_list.xml", PACKAGE_LIB_DIR, database_dir[dba]);
#else
  data_xml =  = g_strdup_printf ("%s/crystals/%s_list.xml", PACKAGE_LIB_DIR, database_dir[dba]);
#endif
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  const xmlChar cif_base[8]="cif-database";
  xmlChar * cdata;
  xmlNodePtr racine, node, sym_node, group_node, cif_node;
  reader = xmlReaderForFile(filetoread, NULL, 0);
  if (reader == NULL)
  {
    return 0;
  }
  else
  {
    doc = xmlParseFile(filetoread);
    if (doc == NULL) return 0;
    racine = xmlDocGetRootElement(doc);
    if (strcmp((char *)(racine -> name), (char *)cif_base) != 0) return clean_xml_data (doc, reader);
    node = findnode (racine -> children, "cif-list");
    if (node == NULL) return clean_xml_data (doc, reader);
    sym_node = findnode (sym_node, sym_list[sym]);
    if (sym_node == NULL) return clean_xml_data (doc, reader);
    group_node = findnode (sym_node, group_names_by_sym[sym][spg]);
    cif_file_name = calloc(num_cif_by_sym_group[sym][spg], sizeof*cif_file_name);
    file_node = group_node -> properties;
    while (file_node)
    {
      cif_node = file_node -> children;
      if (cif_node == NULL) return clean_xml_data (doc, reader);
      if (! get_cif_info_from_cif_node (cif_node)) return 0;
    }
    xmlFreeDoc(doc);
    xmlFreeTextReader(reader);
    xmlCleanupParser();
  }
}

/*!
  \fn int open_cif_database (gchar * filetoread)

  \brief NOT USED !

  \param filetoread
*/
int open_cif_database (gchar * filetoread)
{
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  const xmlChar cif_base[8]="cif-database";
  xmlChar * cdata;
  xmlNodePtr racine, node, sym_node, group_node, cif_node;
  gchar * str;
  /*
   * build an xmlReader for that file
   */
  reader = xmlReaderForFile(filetoread, NULL, 0);
  if (reader == NULL)
  {
    return 0;
  }
  else
  {
    doc = xmlParseFile(filetoread);
    if (doc == NULL) return 0;
    racine = xmlDocGetRootElement(doc);
    if (strcmp((char *)(racine -> name), (char *)cif_base) != 0) return clean_xml_data (doc, reader);

    node = racine -> children;
    node = findnode (racine -> children, "num-cif");
    if (node == NULL) return clean_xml_data (doc, reader);
    cdata = xmlNodeGetContent(node);
    total_num_cif = (int)string_to_double ((gpointer)cdata);
    xmlFree (cdata);
    sym_node = findnode (racine -> children, "sym-cif");
    if (sym_node == NULL) return clean_xml_data (doc, reader);
    int i, j;
    for (i=0; i<SYMMETRIES; i++)
    {
      node = findnode (sym_node, sym_list[i]);
      if (node == NULL) return clean_xml_data (doc, reader);
      cdata = xmlNodeGetContent(node);
      j = (int)string_to_double ((gpointer)cdata);
      xmlFree (cdata);
      group_names_by_sym[i] = calloc(j, sizeof*group_names_by_sym[i]);
      num_cif_by_sym_group[i] = calloc(j, sizeof*num_cif_by_sym_group[i]);
      num_group_by_sym[i] = j;
      for (j=0; <num_group_by_sym[i]; j++)
      {
        group_node = findnode (node, "group");
        if (group_node == NULL) return clean_xml_data (doc, reader);
        group_spec = group_node -> properties;
        while (group_spec)
        {
          cif_node = group_spec -> children;
          if (cif_node == NULL) return clean_xml_data (doc, reader);
          if (strcmp("num_cif", (char *)cif_node -> name) == 0)
          {
            cdata = xmlNodeGetContent(cif_node);
            num_cif_by_sym_group[i][j] = (int)string_to_double ((gpointer)cdata);
            xmlFree (cdata);
          }
        }
        cdata = xmlNodeGetContent(group);
        group_names_by_sym[i][j] = g_strdup_printf ("%s", content);
      }
    }
    sym_node = findnode (racine -> children, "cif-list");
    if (sym_node == NULL) return clean_xml_data (doc, reader);
     for (i=0; i<SYMMETRIES; i++)
    {
      node = findnode (sym_node, sym_list[i]);
      if (node == NULL) return clean_xml_data (doc, reader);
      for (j=0; j<num_group_by_sym[i]; j++)
      {
        group_node = findnode (node, group_names_by_sym[i][j]);
        if (node == NULL) return clean_xml_data (doc, reader);
      }
    }
    xmlFreeDoc(doc);
    xmlFreeTextReader(reader);
    xmlCleanupParser();
    return 1;
  }
}

/*!
  \fn int prepare_data_base (int db)

  \brief prepare CIF files database

  \param db
*/
int prepare_data_base (int db)
{
#ifdef G_OS_WIN32
  data_xml = g_strdup_printf ("%s\\crystals\\%s_list.xml", PACKAGE_LIB_DIR, database_dir[the_database]);
#else
  data_xml =  = g_strdup_printf ("%s/crystals/%s_list.xml", PACKAGE_LIB_DIR, database_dir[the_database]);
#endif
  return open_cif_database (data_xml);
}

void select_cif_data (GtkTreeView * tree_view,
                      GtkTreePath * path,
                      GtkTreeViewColumn * column,
                      gpointer data)
{
  GtkTreeIter row;
  GtkTreeModel * model = gtk_tree_view_get_model(tree_view);
  if (gtk_tree_model_get_iter (model, & row, path))
  {
    GValue val, vbl, vcl;
    val = vbl = vcl = {0, };
    gtk_tree_model_get_value (model, & row, 0, & val);
    gtk_tree_model_get_value (model, & row, 1, & vbl);
    gtk_tree_model_get_value (model, & row, 2, & vcl);
    int i, j, k;
    i = (int)g_value_get_int (& val);
    j = (int)g_value_get_int (& vbl);
    switch (j)
    {
      case 3:
        the_cif = g_strdup_printf ("%s", (char *)g_value_get_string (& vcl));
        prepare_cif_preview (i);
        break;
      case 2:
        the_group = i;
        break;
      case 1:
        the_symmetry = i;
        the_group = 0;
        break;
      case 0:
        the_database = i;
        the_symmetry = the_group = 0;
        k = prepare_database (i);
        break;
    }
    if (j < 3)
    {
      gtk_list_store_clear (cif_store);
      if (j == 0 && k) gtk_tree_selection_select_iter (dataselect[1], & first_symmetry_iter);
      if (j == 0 || j == 1) gtk_tree_selection_select_iter (dataselect[2], & first_group_iter);
      if (j || k)
      {
        prepare_cif_list (the_databas, the_symmetry, the_group);
        fill_cif_tree (cif_store);
        gtk_tree_selection_select_iter (dataselect[3], & first_cif_iter);
        prepare_cif_preview (0);
      }
    }
  }
}

G_MODULE_EXPORT void set_library_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);

/*!
  \fn GtkWidget * cif_tree (GtkListStore * store, int id, gchar * name)

  \brief create cif tree store widget

  \param store the GtkListStore model
  \param id
  \param name column name
*/
GtkWidget * cif_tree (GtkListStore * store, int id, gchar * name)
{
  GtkWidget * scrol = create_scroll (NULL, 150, 300, GTK_SHADOW_ETCHED_IN, 0);
  GtkTreeViewColumn * datacol;
  GtkCellRenderer * datacel;
  GtkWidget * dataview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(store));
  datacel = gtk_cell_renderer_text_new ();
  datacol = gtk_tree_view_column_new_with_attributes (name, datacel, "text", 2, NULL);
  gtk_tree_view_column_set_cell_data_func (datacol, datacel, set_library_markup, NULL, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(dataview), datacol);
  gtk_tree_view_column_set_alignment (datacol, 0.5);
  gtk_tree_view_set_activate_on_single_click (GTK_TREE_VIEW(dataview), TRUE);
  g_signal_connect (dataview, "row-activated", G_CALLBACK(select_cif_data), NULL);
  g_object_unref (store);
  dataselect[id] = gtk_tree_view_get_selection (GTK_TREE_VIEW(dataview));
  gtk_tree_selection_set_mode (dataselect[id], GTK_SELECTION_SINGLE);
  gtk_container_add (GTK_CONTAINER(scrol), dataview);
  return scrol;
}

/*!
  \fn int build_crystal_from_cif_database (project * this_proj)

  \brief create crystal database window

  \param this_proj the target project
*/
int build_crystal_from_cif_database (project * this_proj)
{
  int active = activep;
  int res;
  GtkWidget * clib = dialogmodal ("Crystal builder", GTK_WINDOW(this_proj -> modelgl -> win));
#ifdef GTK3
#ifdef GTKGLAREA
#ifndef G_OS_WIN32
  if (! atomes_visual) gtk_window_change_gdk_visual (clib);
#endif // G_OS_WIN32
#endif // GTKGLAREA
#endif // GTK3
  gtk_dialog_add_button (GTK_DIALOG(clib), "Build", GTK_RESPONSE_APPLY);
  GtkWidget * vbox = gtk_dialog_get_content_area(GTK_DIALOG (clib));
  GtkWidget * hbox = create_hbox (5);
  gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  cif_file_name = cif_name = NULL;

  database_store = gtk_list_store_new (3, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
  gtk_box_pack_start (GTK_BOX(hbox), cif_tree (database_store, 0, "Database"), FALSE, FALSE, 0);
  group_store = gtk_list_store_new (3,  G_TYPE_INT, G_TYPE_INT,G_TYPE_STRING);
  gtk_box_pack_start (GTK_BOX(hbox), cif_tree (database_store, 1, "Symmetry"), FALSE, FALSE, 0);
  group_store = gtk_list_store_new (3, G_TYPE_INT, G_TYPE_INT,G_TYPE_STRING);
  gtk_box_pack_start (GTK_BOX(hbox), cif_tree (group_store, 2, "Group"), FALSE, FALSE, 0);
  cif_store = gtk_list_store_new (3, G_TYPE_INT, G_TYPE_INT,G_TYPE_STRING);
  gtk_box_pack_start (GTK_BOX(hbox), cif_tree (cif_store, 3, "Crystal"), FALSE, FALSE, 0);
  fill_database_store (database_store);
  cif_preview_box = create_hbox(0);
  gtk_box_pack_start (GTK_BOX(hbox), cif_preview_box, FALSE, FALSE, 0);
  cif_preview_plot = NULL;
  the_database = the_group = 0;
  gtk_tree_selection_select_iter (cifselect[0], & first_database_iter);
  gtk_tree_selection_select_iter (cifselect[1], & first_symmetry_iter);
  gtk_tree_selection_select_iter (cifselect[2], & first_group_iter);
  gtk_list_store_clear (cif_store);
  fill_cif_tree (cif_store);
  gtk_tree_selection_select_iter (cifselect[3], & first_cif_iter);
  show_the_widgets_all (clib);
  prepare_cif_preview (0);
  this_proj -> modelgl -> nth_copy = 0;
  switch (gtk_dialog_run (GTK_DIALOG(clib)))
  {
    case GTK_RESPONSE_APPLY:
      gtk_widget_set_sensitive (vbox, 0);
      this_proj -> modelgl -> other_status = 2;
      create_object_from_library (cif_proj -> id);
      gtk_widget_set_sensitive (vbox, 1);
      break;
    default:
      res = 0;
      break;
  }
  gtk_widget_destroy (clib);
  active_project_changed (active);
  if (cif_file_name != NULL) free (cif_file_name);
  if (cif_name != NULL) free (cif_name);
  if (cif_proj != NULL) close_project (cif_proj);
  cif_proj = NULL;
  return res;
}
