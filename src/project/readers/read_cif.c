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
* @file read_cif.c
* @short Functions to read CIF files
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_cif.c'
*
* Contains:
*

 - The functions to read CIF files

*
* List of functions:

  int get_atom_wyckoff (gchar * line, int wid);
  int cif_get_value int cif_get_value (gchar * kroot, gchar * keyw, int lstart, int linec, gchar ** cif_word,
                                       gboolean rec_val, gboolean all_ligne, gboolean total_num, gboolean record_position, int * line_position)
  int cif_file_get_data_in_loop (int linec, int lid);
  int cif_file_get_number_of_atoms (int linec, int lid, int nelem);
  int get_loop_line_id (int lid);
  int get_loop_line_for_key (int linec, int conf, gchar * key_a, gchar * key_b);
  int cif_file_get_number_of_positions (int lid);
  int get_space_group_from_hm (gchar * hmk);
  int get_setting_from_hm (gchar * hmk, int end);
  int group_info_from_hm_key (int spg, gchar * key_hm);
  int cif_get_space_group (int linec);
  int open_cif_configuration (int linec, int conf);
  int open_cif_file (int linec);

  float get_atom_coord (gchar * line, int mid);

  gboolean get_missing_object_from_user ();
  gboolean cif_file_get_atoms_data (int conf, int lin, int cid[8]);
  gboolean cif_get_atomic_coordinates (int linec);
  gboolean cif_get_symmetry_positions (int linec);
  gboolean cif_get_cell_data (int linec, int conf);

  gchar * get_cif_word (gchar * mot);
  gchar * get_atom_label (gchar * line, int lid);
  gchar * get_atom_disorder (gchar * line, int lid);
  gchar * get_string_from_origin (space_group * spg);
  gchar * cif_retrieve_value (int linec, int conf, gchar * key_a, gchar * key_b, gboolean all_ligne, gboolean in_loop, gboolean warning);

  G_MODULE_EXPORT void set_cif_to_insert (GtkComboBox * box, gpointer data);
  void file_get_to_line (int line_id);
  void check_for_to_lab (int ato, gchar * stlab);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "project.h"
#include "atom_edit.h"
#include "cbuild_edit.h"
#include "readers.h"
#include <ctype.h>
#ifdef OPENMP
#  include <omp.h>
#endif

extern int get_atom_id_from_periodic_table (atom_search * asearch);
extern double get_z_from_periodic_table (gchar * lab);
extern void get_origin (space_group * spg);
extern void compute_lattice_properties (cell_info * cell, int box_id);
extern int test_lattice (builder_edition * cbuilder, cell_info * cif_cell);
extern int read_space_group (builder_edition * cbuilder, int spg);
extern gchar * wnpos[3];
extern void get_wyck_char (float val, int ax, int bx);
extern space_group * duplicate_space_group (space_group * spg);
extern distance distance_3d (cell_info * cell, int mdstep, atom * at, atom * bt);
extern void sort (int dim, int * tab);

extern gchar * tmp_pos;

FILE * cifp;
char * line_ptr;
int * keylines = NULL;
int saved_group;
int cif_atoms;
int cif_nspec;
int * cif_lot = NULL;
int * cif_nsps = NULL;

gboolean cif_multiple = FALSE;

gchar ** cif_strings = NULL;

gchar * cif_coord_opts[40][2] = {{"b1", "Monoclinic unique axis b, cell choice 1, abc"},    // 0
                                 {"b2", "Monoclinic unique axis b, cell choice 2, abc"},    // 1
                                 {"b3", "Monoclinic unique axis b, cell choice 3, abc"},    // 2
                                 {"-b1", "Monoclinic unique axis b, cell choice 1, c-ba"},  // 3
                                 {"-b2", "Monoclinic unique axis b, cell choice 2, c-ba"},  // 4
                                 {"-b3", "Monoclinic unique axis b, cell choice 3, c-ba"},  // 5
                                 {"c1", "Monoclinic unique axis c, cell choice 1, abc"},    // 6
                                 {"c2", "Monoclinic unique axis c, cell choice 2, abc"},    // 7
                                 {"c3", "Monoclinic unique axis c, cell choice 3, abc"},    // 8
                                 {"-c1", "Monoclinic unique axis c, cell choice 1, ba-c"},  // 9
                                 {"-c2", "Monoclinic unique axis c, cell choice 2, ba-c"},  // 10
                                 {"-c3", "Monoclinic unique axis c, cell choice 3, ba-c"},  // 11
                                 {"a1", "Monoclinic unique axis a, cell choice 1, abc"},    // 12
                                 {"a2", "Monoclinic unique axis a, cell choice 2, abc"},    // 13
                                 {"a3", "Monoclinic unique axis a, cell choice 3, abc"},    // 14
                                 {"-a1", "Monoclinic unique axis a, cell choice 1, -acb"},  // 15
                                 {"-a2", "Monoclinic unique axis a, cell choice 2, -acb"},  // 16
                                 {"-a3", "Monoclinic unique axis a, cell choice 3, -acb"},  // 17
                                 {"abc", "Orthorhombic"},                                   // 18
                                 {"ba-c", "Orthorhombic"},                                  // 10
                                 {"cab", "Orthorhombic"},                                   // 20
                                 {"-cba", "Orthorhombic"},                                  // 21
                                 {"bca", "Orthorhombic"},                                   // 22
                                 {"a-cb", "Orthorhombic"},                                  // 23
                                 {"1abc", "Orthorhombic origin choice 1"},                  // 24
                                 {"1ba-c", "Orthorhombic origin choice 1"},                 // 25
                                 {"1cab", "Orthorhombic origin choice 1"},                  // 26
                                 {"1-cba", "Orthorhombic origin choice 1"},                 // 27
                                 {"1bca", "Orthorhombic origin choice 1"},                  // 28
                                 {"1a-cb", "rthorhombic origin choice 1"},                  // 29
                                 {"2abc", "Orthorhombic origin choice 2"},                  // 30
                                 {"2ba-c", "Orthorhombic origin choice 2"},                 // 31
                                 {"2cab", "Orthorhombic origin choice 2"},                  // 32
                                 {"2-cba", "Orthorhombic origin choice 2"},                 // 33
                                 {"2bca", "Orthorhombic origin choice 2"},                  // 34
                                 {"2a-cb", "Orthorhombic origin choice 2"},                 // 35
                                 {"1", "Tetragonal or cubic origin choice 1"},              // 36
                                 {"2", "Tetragonal or cubic origin choice 2"},              // 37
                                 {"h", "Trigonal using hexagonal axes"},                    // 38
                                 {"r", "Trigonal using rhombohedral axes "}};               // 39

#ifdef G_OS_WIN32
  typedef intptr_t ssize_t;

  ssize_t getline(char **lineptr, size_t *n, FILE *stream)
  {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL)
    {
      errno = EINVAL;
      return -1;
    }

    c = getc(stream);
    if (c == EOF) return -1;

    if (*lineptr == NULL)
    {
      *lineptr = malloc(128);
      if (*lineptr == NULL) return -1;
      *n = 128;
    }

    pos = 0;
    while(c != EOF)
    {
      if (pos + 1 >= *n)
      {
        size_t new_size = *n + (*n >> 2);
        if (new_size < 128)
        {
          new_size = 128;
        }
        char *new_ptr = realloc(*lineptr, new_size);
        if (new_ptr == NULL) return -1;
        *n = new_size;
        *lineptr = new_ptr;
      }

      ((unsigned char *)(*lineptr))[pos ++] = c;
      if (c == '\n') break;
      c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
  }
#endif // G_OS_WIN_32

/*!
  \fn gchar * get_cif_word (gchar * mot)

  \brief get string from CIF file, EOL can be ugly

  \param mot the string that was read in the file
*/
gchar * get_cif_word (gchar * mot)
{
  gchar * word = substitute_string (mot, "\n", NULL);
  word = substitute_string (word, "\r", NULL);
  return word;
}

/*!
  \fn float get_atom_coord (gchar * line, int mid)

  \brief read atom coordinates from CIF file

  \param line the string that contains the data
  \param mid the position to reach on the line
*/
float get_atom_coord (gchar * line, int mid)
{
  gchar * co_line;
  gchar * init = g_strdup_printf ("%s", line);
  char * co_word = strtok_r (init, " ", & co_line);
  int i;
  for (i=0; i<mid-1; i++)
  {
    co_word = strtok_r (NULL, " ", & co_line);
  }
  double v = string_to_double ((gpointer)get_cif_word(co_word));
  g_free (init);
  return v;
}

/*!
  \fn gchar * get_atom_label (gchar * line, int lid)

  \brief read atom label from CIF file

  \param line the string that contains the data
  \param lid the position to reach on the line
*/
gchar * get_atom_label (gchar * line, int lid)
{
  gchar * at_line;
  gchar * init = g_strdup_printf ("%s", line);
  char * at_word = strtok_r (init, " ", & at_line);
  int i;
  for (i=0; i<lid-1; i++) at_word = strtok_r (NULL, " ", & at_line);
  gchar * str;
  for (i=0; i<10; i++)
  {
    str = g_strdup_printf ("%d", i);
    at_word = substitute_string (at_word, str, NULL);
    g_free (str);
  }
  at_word = get_cif_word (at_word);
  at_word = substitute_string (at_word, "-", NULL);
  at_word = substitute_string (at_word, "+", NULL);
  g_free (init);
  return g_strdup_printf ("%c%c", at_word[0], tolower(at_word[1]));
}

/*!
  \fn int get_atom_wyckoff (gchar * line, int wid)

  \brief read Wyckoff position from CIF file

  \param line the string that contains the data
  \param wid the position to reach on the line
*/
int get_atom_wyckoff (gchar * line, int wid)
{
  gchar * wy_line;
  gchar * init = g_strdup_printf ("%s", line);
  char * wy_word = strtok_r (init, " ", & wy_line);
  int i, j;
  j = 0;
  for (i=0; i<wid-1; i++) wy_word = strtok_r (NULL, " ", & wy_line);
  for (i=0; i<this_reader -> lattice.sp_group -> numw; i++)
  {
    if (g_strcmp0(get_cif_word(wy_word), this_reader -> lattice.sp_group -> wyckoff[i].let) == 0)
    {
      j = i;
      break;
    }
  }
  g_free (init);
  return j;
}

/*!
  \fn gchar * get_atom_disorder (gchar * line, int lid)

  \brief read atom disorder group from CIF file

  \param line the string that contains the data
  \param lid the position to reach on the line
*/
gchar * get_atom_disorder (gchar * line, int lid)
{
  gchar * at_line;
  gchar * init = g_strdup_printf ("%s", line);
  char * at_word = strtok_r (init, " ", & at_line);
  int i;
  for (i=0; i<lid-1; i++) at_word = strtok_r (NULL, " ", & at_line);
  g_free (init);
  return get_cif_word (at_word);
}

GtkWidget ** img_cif;
atom_search * cif_search = NULL;
atomic_object * cif_object = NULL;

/*!
  \fn G_MODULE_EXPORT void set_cif_to_insert (GtkComboBox * box, gpointer data)

  \brief change the object to insert at an empty cif position

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_cif_to_insert (GtkComboBox * box, gpointer data)
{
  GValue val = {0, };
  int i, j, k;
  i = GPOINTER_TO_INT(data);
  GtkTreeModel * cmodel = gtk_combo_box_get_model (box);
  GtkTreeIter iter;
  gchar * str;
  gboolean done = TRUE;
  if (gtk_combo_box_get_active_iter (box, & iter))
  {
    gtk_tree_model_get_value (cmodel, & iter, 0, & val);
    str = g_strdup_printf ("%s", (char *)g_value_get_string (& val));
    j = get_selected_object_id (FALSE, activep, str, cif_search);
    to_insert_in_project (j, i, active_project, cif_search, FALSE);
    if (j > 0)
    {
      gtk_tree_store_set (GTK_TREE_STORE(cmodel), & iter, 0, periodic_table_info[j].lab, -1);
    }
    cif_search -> todo[i]  = (! j) ? 0 : 1;
    if (! j) done = FALSE;
    cif_search -> in_selection = 0;
    for (k=0; k<this_reader -> object_to_insert; k++) cif_search -> in_selection += cif_search -> todo[k];
  }
  str = (done) ? g_strdup_printf (APPLY) : g_strdup_printf (DELETEB);
  set_image_from_icon_name (img_cif[i], str);
  g_free (str);
  if (! done) combo_set_active ((GtkWidget *)box, 0);
}

/*!
  \fn gboolean get_missing_object_from_user ()

  \brief get missing atomic number in CIF file from the user
*/
gboolean get_missing_object_from_user ()
{
  cif_search = allocate_atom_search (activep, REPLACE, 0, this_reader -> object_to_insert);
  cif_object = NULL;
  GtkWidget * info = dialogmodal ("Error while reading CIF file", GTK_WINDOW(MainWindow));
  GtkWidget * vbox, * hbox;
  gchar * str;
  vbox = dialog_get_content_area (info);
  gchar * labpick = "<b>To continue and build the crystal according to the information of the CIF file\n"
                    "it is required to provide a suitable value for each and every missing parameter(s).</b>"
                    "\n\nPlease select an atom type for the following object(s):";
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label (labpick, 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  img_cif = g_malloc0(this_reader -> object_to_insert*sizeof*img_cif);
  GtkWidget * but;
  GtkCellRenderer * renderer;
  GtkTreeModel * model;
  int i;
  for (i=0; i<this_reader -> object_to_insert; i++)
  {
    hbox = create_hbox(0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
    str = g_strdup_printf ("Type N°%d:\t<b>%s</b>", i+1, this_reader -> label[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 150, -1, 0.0, 0.5), FALSE, FALSE, 20);
    g_free (str);
    img_cif[i] = stock_image (DELETEB);
    model = replace_combo_tree (TRUE, activep);
    but = gtk_combo_box_new_with_model (model);
    g_object_unref (model);
    renderer = gtk_cell_renderer_combo_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (but), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (but), renderer, "text", 0, NULL);
    combo_set_active (but, 0);
    g_signal_connect (G_OBJECT(but), "changed", G_CALLBACK(set_cif_to_insert), GINT_TO_POINTER(i));
    combo_set_markup (but);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img_cif[i], FALSE, FALSE, 30);
  }

  gchar * endpick = "In case of a molecule: insert an extra type of atom and run a substitution afterwards.";
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label (endpick, 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  run_this_gtk_dialog (info, G_CALLBACK(run_destroy_dialog), NULL);
  g_free (img_cif);
  return (cif_search -> in_selection == this_reader -> object_to_insert) ? TRUE : FALSE;
}

#ifndef OPENMP
/*!
  \fn void file_get_to_line (int line_id)

  \brief reach line in CIF file

  \param line_id Line to reach
*/
void file_get_to_line (int line_id)
{
  int i;
  tail = head;
  for (i=0; i<line_id; i++) tail = tail -> next;
}
#endif

/*!
  \fn int cif_get_value (gchar * kroot, gchar * keyw, int lstart, int linec, gchar ** cif_word,
                         gboolean rec_val, gboolean all_ligne, gboolean total_num, gboolean record_position, int * line_position)

  \brief read pattern in CIF file

  \param kroot string key (first part)
  \param keyw string key (second part)
  \param lstart starting line to reach
  \param lend final line
  \param cif_word pointer to store the data to read
  \param rec_val record position on the line
  \param all_ligne browse all line (1/0)
  \param total_num get the total number of occurrences
  \param record_position record line position of each occurrences
  \param line_position line position of each occurrences
*/
int cif_get_value (gchar * kroot, gchar * keyw, int lstart, int lend, gchar ** cif_word,
                   gboolean rec_val, gboolean all_ligne, gboolean total_num, gboolean record_position, int * line_position)
{
  int res = 0;
  int i;
  size_t j, k, l, m;
  gchar * str;
  gchar * str_w, * str_a, * str_b;
  gchar * mot;
  gchar * saved_line;
  gchar * the_line;
  gchar * the_word;
  j = strlen(kroot);
  k = strlen(keyw);
  l = j+k+1;

#ifdef OPENMP
  int numth = omp_get_max_threads ();
  #pragma omp parallel for num_threads(numth) private(i,m,the_line,saved_line,the_word,mot,str_a,str_b,str_w) shared(j,k,l,this_reader,coord_line,cif_word,rec_val,all_ligne,kroot,keyw,total_num,record_position,line_position,res)
  for (i=lstart; i<lend; i++)
  {
    the_line = NULL;
    if (res && ! total_num) goto endi;
    the_line = g_strdup_printf ("%s", coord_line[i]);
    the_word = strtok_r (the_line, " ", & saved_line);
    while (the_word)
    {
      str_w = get_cif_word (the_word);
      str_w = g_ascii_strdown (str_w, strlen(str_w));
      if (strlen(str_w) == l)
      {
        str_a = g_strdup_printf ("%c", str_w[0]);
        for (m=1; m<j; m++) str_a = g_strdup_printf ("%s%c", str_a, str_w[m]);
        str_b = g_strdup_printf ("%c", str_w[j+1]);
        for (m=j+2; m<l; m++) str_b = g_strdup_printf ("%s%c", str_b, str_w[m]);
        if (g_strcmp0(str_a, kroot) == 0 && g_strcmp0(str_b,keyw) == 0)
        {
          the_word = strtok_r (NULL, " ", & saved_line);
          if (! the_word)
          {
            if (rec_val || all_ligne)
            {
              str = g_strdup_printf ("Wrong file format: searching for <b>%s</b> - error at line <b>%d</b> !\n", keyw, i+1);
              add_reader_info (str, 0);
              g_free (str);
              g_free (str_w);
              g_free (str_a);
              g_free (str_b);
              res = -1;
              goto endi;
            }
          }
          if (total_num)
          {
            #pragma omp critical
            {
              if (record_position)
              {
                line_position[res] = i + 1;
              }
              res ++;
              if (this_reader -> steps && res == this_reader -> steps) total_num = FALSE;
            }
          }
          else
          {
            res = i + 1;
          }
          if (all_ligne)
          {
            mot = g_strdup_printf ("%s", the_word);
            the_word = strtok_r (NULL, " ", & saved_line);
            while (the_word)
            {
              mot = g_strdup_printf ("%s%s", mot, the_word);
              the_word = strtok_r (NULL, " ", & saved_line);
            }
            the_word = g_strdup_printf ("%s", mot);
            g_free (mot);
          }
          if (the_word && rec_val)
          {
            * cif_word = get_cif_word (the_word);
          }
          g_free (str_w);
          g_free (str_a);
          g_free (str_b);
          goto endi;
        }
        g_free (str_a);
        g_free (str_b);
      }
      g_free (str_w);
      the_word = strtok_r (NULL, " ", & saved_line);
    }
    endi:;
    g_free (the_line);
  }

  if (res < 0) res = 0;
#else
  file_get_to_line (lstart);
  res = 0;
  i = lstart;
  while (tail)
  {
    the_line = g_strdup_printf ("%s", tail -> line);
    the_word = strtok_r (the_line, " ", & saved_line);
    while (the_word)
    {
      str_w = get_cif_word (the_word);
      str_w = g_ascii_strdown (str_w, strlen(str_w));
      if (strlen(str_w) == l)
      {
        str_a = g_strdup_printf ("%c", str_w[0]);
        for (m=1; m<j; m++) str_a = g_strdup_printf ("%s%c", str_a, str_w[m]);
        str_b = g_strdup_printf ("%c", str_w[j+1]);
        for (m=j+2; m<l; m++) str_b = g_strdup_printf ("%s%c", str_b, str_w[m]);
        if (g_strcmp0(str_a, kroot) == 0 && g_strcmp0(str_b,keyw) == 0)
        {
          the_word = strtok_r (NULL, " ", & saved_line);
          if (! the_word)
          {
            if (rec_val || all_ligne)
            {
              str = g_strdup_printf ("Wrong file format: searching for <b>%s</b> - error at line <b>%d</b> !\n", keyw, i+1);
              add_reader_info (str, 0);
              g_free (str);
              g_free (str_w);
              g_free (str_a);
              g_free (str_b);
              g_free (the_line);
              return 0;
            }
          }

          if (all_ligne)
          {
            mot = g_strdup_printf ("%s", the_word);
            the_word = strtok_r (NULL, " ", & saved_line);
            while (the_word)
            {
              mot = g_strdup_printf ("%s%s", mot, the_word);
              the_word = strtok_r (NULL, " ", & saved_line);
            }
            the_word = g_strdup_printf ("%s", mot);
            g_free (mot);
          }
          if (the_word && rec_val)
          {
             * cif_word = get_cif_word (the_word);
          }
          if (total_num)
          {
            if (record_position)
            {
              line_position[res] = i + 1;
            }
            res ++;
            if (this_reader -> steps && res == this_reader -> steps)
            {
              g_free (str_a);
              g_free (str_b);
              g_free (str_w);
              g_free (the_line);
              return res;
            }
          }
          else
          {
            g_free (str_a);
            g_free (str_b);
            g_free (str_w);
            g_free (the_line);
            return i + 1;
          }
        }
        g_free (str_a);
        g_free (str_b);
      }
      g_free (str_w);
      the_word = strtok_r (NULL, " ", & saved_line);
    }
    g_free (the_line);
    tail = tail -> next;
    i ++;
  }
#endif
  return res;
}

/*!
  \fn int cif_file_get_data_in_loop (int linec, int lid)

  \brief get the number of "_" motifs in a line

  \param linec total number of lines
  \param lid line to reach
*/
int cif_file_get_data_in_loop (int linec, int lid)
{
  gboolean res = FALSE;
  int i = 0;
  gchar * the_word;
  gchar * the_line;
  gchar * saved_line;
#ifdef OPENMP
  while (! res)
  {
    if (lid+i < linec)
    {
      the_line = g_strdup_printf ("%s", coord_line[lid+i]);
      the_word = strtok_r (the_line, " ", & saved_line);
      if (the_word[0] == '_')
      {
        i ++;
      }
      else
      {
        res = TRUE;
      }
      g_free (the_line);
    }
    else
    {
      res = TRUE;
    }
  }
#else
  file_get_to_line (lid);
  while (! res)
  {
    if (tail)
    {
      the_line = g_strdup_printf ("%s", tail -> line);
      the_word = strtok_r (the_line, " ", & saved_line);
      if (the_word[0] == '_')
      {
        i ++;
        tail = tail -> next;
      }
      else
      {
        res = TRUE;
      }
      g_free (the_line);
    }
    else
    {
      res = TRUE;
    }
  }
#endif
  return i;
}

/*!
  \fn int get_loop_line_id (int lid)

  \brief reach a line in the CIF file

  \param lid line to reach
*/
int get_loop_line_id (int lid)
{
  int i;
  gchar * the_word;
  gchar * the_line;
  gchar * saved_line;
  gchar * str_w;
#ifdef OPENMP
  for (i=lid-1; i>-1; i--)
  {
    the_line = g_strdup_printf ("%s", coord_line[i]);
    the_word = strtok_r (the_line, " ", & saved_line);
    if (the_word)
    {
      str_w = g_ascii_strdown (the_word, strlen(the_word));
      if (g_strcmp0 ("loop_", get_cif_word(str_w)) == 0)
      {
        g_free (str_w);
        return i+1;
      }
      g_free (str_w);
    }
  }
#else
  file_get_to_line (lid);
  i = lid;
  while (tail)
  {
    the_line = g_strdup_printf ("%s", tail -> line);
    the_word = strtok_r (the_line, " ", & saved_line);
    if (the_word)
    {
      str_w = g_ascii_strdown (the_word, strlen(the_word));
      if (g_strcmp0 ("loop_", get_cif_word(str_w)) == 0)
      {
        g_free (str_w);
        return i+1;
      }
      g_free (str_w);
    }
    i --;
    tail = tail -> prev;
  }
#endif
  return 0;
}

/*!
  \fn int get_loop_line_for_key (int linec, int conf, gchar * key_a, gchar * key_b)

  \brief search a string

  \param linec total number of lines
  \param conf configuration to read
  \param key_a string root (first part)
  \param key_b string end (second part)
*/
int get_loop_line_for_key (int linec, int conf, gchar * key_a, gchar * key_b)
{
  int lli = 0;
  int * line_numbers;
  int steps;
  if (this_reader -> steps > 1)
  {
    line_numbers = allocint (this_reader -> steps);
    steps = cif_get_value (key_a, key_b, 0, linec, NULL, FALSE, FALSE, TRUE, TRUE, line_numbers);
    if (steps)
    {
      if (steps != this_reader -> steps)
      {
        gchar * str = g_strdup_printf ("<b>CIF get loop line</b>: something is wrong for keyword: %s_%s\n"
                                       "  -> keyword found= %d times\n"
                                       "  -> configurations in CIF file= %d !\n", key_a, key_b, steps, this_reader -> steps);
        add_reader_info (str, 0);
        g_free (str);
        g_free (line_numbers);
        return 0;
      }
      sort (steps, line_numbers);
      lli = line_numbers[conf];
      g_free (line_numbers);
    }
  }
  else
  {
    lli = cif_get_value (key_a, key_b, 0, linec, NULL, FALSE, FALSE, FALSE, FALSE, NULL);
  }
  return (lli) ? get_loop_line_id (lli) : 0;
}

/*!
  \fn gchar * cif_retrieve_value (int linec, int conf, gchar * key_a, gchar * key_b, gboolean all_ligne, gboolean in_loop, gboolean warning)

  \brief retrieve a field value as string in the CIF file

  \param linec total number of lines
  \param conf configuration to read
  \param keya string key (first part)
  \param keyb string key (second part)
  \param all_ligne browse all line (1/0)
  \param in_loop is field in a loop (1/0)
  \param warning show warning (1/0)
*/
gchar * cif_retrieve_value (int linec, int conf, gchar * key_a, gchar * key_b, gboolean all_ligne, gboolean in_loop, gboolean warning)
{
  gchar * str;
  gchar * cif_value = NULL;
  int loop_pos[2];
  int * line_numbers;
  int steps;
  if (this_reader -> steps > 1)
  {
    line_numbers = allocint (this_reader -> steps);
    // g_debug ("CIF:: retrieve:: linec= %d, conf= %d, key_a= %s, key_b= %s, this_reader -> steps= %d", linec, conf, key_a, key_b, this_reader -> steps);
    steps = cif_get_value (key_a, key_b, 0, linec, NULL, FALSE, FALSE, TRUE, TRUE, line_numbers);
    // g_debug ("CIF:: retrieve:: steps= %d", steps);
    if (steps)
    {
      if (steps != this_reader -> steps)
      {
        str = g_strdup_printf ("<b>CIF retrieve value</b>: keyword: %s_%s\n"
                               "  -> keyword found= %d times\n"
                               "  -> configurations in CIF file= %d !\n", key_a, key_b, steps, this_reader -> steps);
        add_reader_info (str, 0);
        g_free (str);
        str = NULL;
        g_free (line_numbers);
        line_numbers = NULL;
        return NULL;
      }
      sort (steps, line_numbers);
      if (in_loop)
      {
        // Considering that the loop has not more than a thousand keys
        loop_pos[0] = get_loop_line_id (line_numbers[conf]);
        loop_pos[1] = (loop_pos[0] + 1000) > linec ? linec : (loop_pos[0] + 1000);
      }
      else
      {
        loop_pos[0] = (! line_numbers[conf]) ? line_numbers[conf] : line_numbers[conf] - 1;
        loop_pos[1] = (loop_pos[0] + 1000) > linec ? linec : (loop_pos[0] + 1000);
        // loop_pos[1] = (conf == this_reader -> steps - 1) ? linec : line_numbers[conf + 1];
      }
      g_free (line_numbers);
      line_numbers = NULL;
    }
  }
  else
  {
    loop_pos[0] = 0;
    loop_pos[1] = linec;
  }
  if (! cif_get_value (key_a, key_b, loop_pos[0], loop_pos[1], & cif_value, TRUE, all_ligne, FALSE, FALSE, NULL) && warning)
  {
#ifdef DEBUG
    g_debug ("CIF:: retrieve:: keyword: %s_%s not found for conf %d between loop_pos[0]= %d and loop_pos[1]= %d\n", key_a, key_b, conf, loop_pos[0], loop_pos[1]);
#endif
    str = g_strdup_printf ("<b>Key positions</b>: keyword: %s_%s\n"
                           "  -> not found for conf %d between loop_pos[0]= %d and loop_pos[1]= %d\n", key_a, key_b, conf, loop_pos[0], loop_pos[1]);
    add_reader_info (str, 0);
    g_free (str);
    return NULL;
  }
  return cif_value;
}

/*!
  \fn int cif_file_get_number_of_atoms (int linec, int lid, int nelem)

  \brief get the number of atom(s) in a CIF file

  \param linec total number of lines
  \param lid line to reach
  \param nelem number of element(s) the line
*/
int cif_file_get_number_of_atoms (int linec, int lid, int nelem)
{
  gboolean res = FALSE;
  int i, j;
  char init;
  gchar * the_word;
  gchar * the_line;
  gchar * saved_line;
  i = 0;
#ifdef OPENMP
  while (! res && (lid+i) < linec)
  {
    the_line = g_strdup_printf ("%s", coord_line[lid+i]);
    the_word = strtok_r (the_line, " ", & saved_line);
    j = 0;
    while (the_word)
	{
      if (! j) init = the_word[0];
      j ++;
      the_word = strtok_r (NULL, " ", & saved_line);
    }
    if (j == nelem && init != '_')
    {
      i ++;
    }
    else
    {
      res = TRUE;
    }
    g_free (the_line);
  }
#else
  file_get_to_line (lid);
  while (! res && tail)
  {
    the_line = g_strdup_printf ("%s", tail -> line);
    the_word = strtok_r (the_line, " ", & saved_line);
    j = 0;
    while (the_word)
	{
      if (j == 0) init = the_word[0];
      j ++;
      the_word = strtok_r (NULL, " ", & saved_line);
    }
    if (j == nelem && init != '_')
    {
      i ++;
    }
    else
    {
      res = TRUE;
    }
    g_free (the_line);
    tail = tail -> next;
  }
#endif
  return i;
}

/*!
  \fn void check_for_to_lab (int ato, gchar * stlab)

  \brief check atom label

  \param ato atom id
  \param stlab label to read in the CIF file
*/
void check_for_to_lab (int ato, gchar * stlab)
{
  int i, j;
  j = -1;
  // First is the label of 'ato' already listed
  for (i=0; i<this_reader -> object_to_insert; i++)
  {
    if (g_strcmp0(this_reader -> label[i], stlab) == 0)
    {
      j = i;
      break;
    }
  }
  if (j < 0)
  {
    if (this_reader -> label)
    {
      this_reader -> label = g_realloc (this_reader -> label, (this_reader -> object_to_insert+1)*sizeof*this_reader -> label);
    }
    else
    {
      this_reader -> label = g_malloc0 (1*sizeof*this_reader -> label);
    }
    this_reader -> label[this_reader -> object_to_insert] = g_strdup_printf ("%s", stlab);
    this_reader -> object_to_insert ++;
    j = this_reader -> object_to_insert-1;
  }
  if (this_reader -> object_list)
  {
    this_reader -> object_list = g_realloc (this_reader -> object_list, (this_reader -> atom_unlabelled+1)*sizeof*this_reader -> object_list);
    this_reader -> u_atom_list = g_realloc (this_reader -> u_atom_list, (this_reader -> atom_unlabelled+1)*sizeof*this_reader -> u_atom_list);
  }
  else
  {
    this_reader -> object_list = g_malloc0 (1*sizeof*this_reader -> object_list);
    this_reader -> u_atom_list = g_malloc0 (1*sizeof*this_reader -> u_atom_list);
  }
  this_reader -> object_list[this_reader -> atom_unlabelled] = j;
  this_reader -> u_atom_list[this_reader -> atom_unlabelled] = ato;
  this_reader -> atom_unlabelled ++;
}

/*!
  \fn gboolean cif_file_get_atoms_data (int conf, int lin, int cid[8])

  \brief get atoms data from the CIF file

  \param conf the configuration to read
  \param lin line to reach
  \param cid positions on the line for the data to read
*/
gboolean cif_file_get_atoms_data (int conf, int lin, int cid[9])
{
  int i, j;
  double v;
  gchar * str;
  gboolean done = TRUE;
  gchar * cline;
  int at_step = (active_project -> steps == 1) ? 0 : conf;
#ifdef OPENMP
  int numth = omp_get_max_threads ();
  #pragma omp parallel for num_threads(numth) private(i,j,v,cline,str) shared(this_reader,coord_line,at_step,done,lin,cid)
  for (i=0; i<this_reader -> natomes; i++)
  {
    cline = g_strdup_printf ("%s", coord_line[i+lin]);
    str = get_atom_label (cline, (cid[0]) ? cid[0] : cid[1]);
    v = get_z_from_periodic_table (str);
    #pragma omp critical
    {
      if (v)
      {
        check_for_species (v, i);
      }
      else
      {
        done = FALSE;
        check_for_to_lab (i, str);
      }
    }
    if (this_reader -> cartesian)
    {
      active_project -> atoms[at_step][i].x = get_atom_coord (cline, cid[2]);
      active_project -> atoms[at_step][i].y = get_atom_coord (cline, cid[3]);
      active_project -> atoms[at_step][i].z = get_atom_coord (cline, cid[4]);
    }
    else
    {
      for (j=0; j<3; j++) this_reader -> coord[i][j] = get_atom_coord (cline, cid[j+2]);
    }
    if (! this_reader -> cartesian)
    {
      this_reader -> wyckoff[i] = (cid[5]) ? get_atom_wyckoff (cline, cid[5]) : 0;
      this_reader -> occupancy[i] = (cid[6]) ? get_atom_coord (cline, cid[6]) : 1.0;
      this_reader -> multi[i] = (cid[7]) ? get_atom_coord (cline, cid[7]) : 0.0;
      this_reader -> disorder[i] = (cid[8]) ? (int) get_atom_coord (cline, cid[8]) : 0;
    }
    if (cline)
    {
      g_free (cline);
      cline = NULL;
    }
    if (str)
    {
      g_free (str);
      str = NULL;
    }
  }
#else
  file_get_to_line (lin);
  for (i=0; i<this_reader -> natomes; i++)
  {
    cline = g_strdup_printf ("%s", tail -> line);
    str = get_atom_label (cline, (cid[0]) ? cid[0] : cid[1]);
    v = get_z_from_periodic_table (str);
    if (v)
    {
      check_for_species (v, i);
    }
    else
    {
      done = FALSE;
      check_for_to_lab (i, str);
    }
    if (this_reader -> cartesian)
    {
      active_project -> atoms[at_step][i].x = get_atom_coord (cline, cid[2]);
      active_project -> atoms[at_step][i].y = get_atom_coord (cline, cid[3]);
      active_project -> atoms[at_step][i].z = get_atom_coord (cline, cid[4]);
    }
    else
    {
      for (j=0; j<3; j++) this_reader -> coord[i][j] = get_atom_coord (cline, cid[j+2]);
    }
    if (! this_reader -> cartesian)
    {
      this_reader -> wyckoff[i] = (cid[5]) ? get_atom_wyckoff (cline, cid[5]) : 0;
      this_reader -> occupancy[i] = (cid[6]) ? get_atom_coord (cline, cid[6]) : 1.0;
      this_reader -> multi[i] = (cid[7]) ? get_atom_coord (cline, cid[7]) : 0.0;
      this_reader -> disorder[i] = (cid[8]) ? get_atom_coord (cline, cid[8]) : 0;
    }
/* #ifdef DEBUG
    j = this_reader -> wyckoff[i];
    g_debug ("CIF:: At= %s, w_letter[%d]= %s, occ= %f, x= %f, y= %f, z= %f", this_reader -> label[i],
             j, this_reader -> lattice.sp_group -> wyckoff[j].let, this_reader -> occupancy[i],
             this_reader -> coord[i][0], this_reader -> coord[i][1], this_reader -> coord[i][2]);
#endif */
    tail = tail -> next;
  }
#endif
  if (! done)
  {
    done = (cif_search) ? TRUE : get_missing_object_from_user ();
  }
  return done;
}

/*!
  \fn gboolean cif_get_atomic_coordinates (int linec)

  \brief read the atomic coordinates from the CIF file

  \param linec total number of lines
  \param conf configuration to read
*/
gboolean cif_get_atomic_coordinates (int linec, int conf)
{
  gchar * labkeys[2] = {"type_symbol", "label"};
  gchar * frackeys[3] = {"fract_x", "fract_y", "fract_z"};
  gchar * cartkeys[3] = {"cartn_x", "cartn_y", "cartn_z"};
  gchar * symkeys[4] = {"wyckoff_symbol", "occupancy", "symmetry_multiplicity", "disorder_group"};
  gchar * str = NULL;
  int cid[9];
  int loop_line;
  int loop_max;
  int i, j, k;
  double u, v;
  int * tmp_nsps;
  double * tmp_z;
  if (cif_multiple)
  {
    loop_line = get_loop_line_for_key (linec, conf, "_atom_site", (this_reader -> cartesian) ? cartkeys[0] : frackeys[0]);
    if (! loop_line)
    {
      return FALSE;
    }
  }
  else
  {
    loop_line = get_loop_line_for_key (linec, 0, "_atom_site", cartkeys[0]);
    if (! loop_line)
    {
      loop_line = get_loop_line_for_key (linec, 0, "_atom_site", frackeys[0]);
      if (! loop_line)
      {
        return FALSE;
      }
    }
    else
    {
      this_reader -> cartesian = TRUE;
    }
  }
  loop_max = (loop_line + 1000 > linec) ? linec : loop_line + 1000;
  i = 0;
  for (j=0; j<2; j++)
  {
    cid[j] = cif_get_value ("_atom_site", labkeys[j], loop_line, loop_max, NULL, FALSE, FALSE, FALSE, FALSE, NULL);
    if (cid[j])
    {
      i ++;
      cid[j] -= loop_line;
    }
  }
  if (! i)
  {
    add_reader_info ("<b>Atomic coordinates</b>: impossible to find atomic label(s) ...", 0);
    return FALSE;
  }
  for (i=0; i<3; i++)
  {
    cid[i+2] = cif_get_value ("_atom_site", (this_reader -> cartesian) ? cartkeys[i] : frackeys[i], loop_line, loop_max, NULL, FALSE, FALSE, FALSE, FALSE, NULL);
    if (cid[i+2])
    {
      cid[i+2] -= loop_line;
    }
    else
    {
      str = g_strdup_printf ("<b>Atomic coordinates</b>: impossible to find '%s' ...", (this_reader -> cartesian) ? cartkeys[i] : frackeys[i]);
      add_reader_info (str, 1);
      g_free (str);
      this_reader -> cartesian = FALSE;
    }
  }
  if (! this_reader -> cartesian)
  {
    for (i=0; i<4; i++)
    {
      cid[i+5] = cif_get_value ("_atom_site", symkeys[i], loop_line, loop_max, NULL, FALSE, FALSE, FALSE, FALSE, NULL);
      if (cid[i+5])
      {
        cid[i+5] -= loop_line;
        if (i == 1 && this_reader -> rounding < 0)
        {
          this_reader -> rounding = iask ("Please select how to handle occupancy", "Select how to handle occupancy", 5, MainWindow);
          if (this_reader -> rounding < 0 || this_reader -> rounding > 2) this_reader -> rounding = 2;
          if (! cif_use_symmetry_positions)
          {
            str = g_strdup_printf ("Occupancy %s\n\t%s\n", cif_occ[this_reader -> rounding], cif_sites[cif_use_symmetry_positions]);
            add_reader_info (str, 1);
            g_free (str);
          }
        }
      }
    }
  }
  i = cif_file_get_data_in_loop (linec, loop_line);
  this_reader -> natomes = cif_file_get_number_of_atoms (linec, loop_line+i, i);
  if (! this_reader -> natomes) return FALSE;

  if (conf && active_project -> steps > 1 && this_reader -> natomes != cif_atoms)
  {
    // Not the same number of atoms between each configuration
    str = g_strdup_printf ("<b>Atomic coordinates</b>: the number of atom(s) changes !\n"
                           "  - configuration N°%d\t :: atoms= %d\n"
                           "  - initialization      \t\t :: atoms= %d\n", conf, this_reader -> natomes, cif_atoms);
    add_reader_info (str, 0);
    g_free (str);
    return FALSE;
  }
  if (this_reader -> cartesian)
  {
    if (! conf)
    {
      active_project -> natomes = this_reader -> natomes;
      allocatoms (active_project);
    }
  }
  else
  {
    this_reader -> coord = allocddouble (this_reader -> natomes*this_reader -> steps, 3);
  }

  if (! this_reader -> cartesian)
  {
    this_reader -> lot = allocint (this_reader -> natomes);
    this_reader -> wyckoff = allocint (this_reader -> natomes);
	this_reader -> occupancy = allocdouble (this_reader -> natomes);
    this_reader -> multi = allocint (this_reader -> natomes);
    this_reader -> disorder = allocint (this_reader -> natomes);
  }
  this_reader -> z = allocdouble (1);
  this_reader -> nsps = allocint (1);

  if (! cif_file_get_atoms_data (conf, loop_line+i, cid)) return FALSE;
  if (active_project -> steps > 1)
  {
    tmp_nsps = duplicate_int (this_reader -> nspec, this_reader -> nsps);
    tmp_z = duplicate_double (this_reader -> nspec, this_reader -> z);
    for (i=1; i<this_reader -> nspec; i++)
    {
      v = tmp_z[i];
      j = tmp_nsps[i];
      for (k=i-1; k>-1; k--)
      {
        if (tmp_z[k] <= v) break;
        tmp_z[k+1] = tmp_z[k];
        tmp_nsps[k+1] = tmp_nsps[k];
      }
      tmp_z[k+1] = v;
      tmp_nsps[k+1] = j;
    }
    g_free (tmp_z);
    if (! conf)
    {
      cif_atoms = this_reader -> natomes;
      cif_nspec = this_reader -> nspec;
      cif_nsps = duplicate_int (this_reader -> nspec, tmp_nsps);
    }
  }
  if (conf && active_project -> steps > 1)
  {
    if (this_reader -> nspec != cif_nspec)
    {
      // Not the same number of chemical species between each configuration
      str = g_strdup_printf ("<b>Atomic coordinates</b>: the number of chemical species changes !\n"
                             "  - configuration N°%d\t :: species= %d\n"
                             "  - initialization      \t\t :: species= %d\n", conf, this_reader -> nspec, cif_nspec);
      add_reader_info (str, 0);
      g_free (str);
      return FALSE;
    }
    for (i=0; i<this_reader -> nspec; i++)
    {
      if (tmp_nsps[i] != cif_nsps[i])
      {
        // Not the same number of atom(s) by chemical species between each configuration
        str = g_strdup_printf ("<b>Atomic coordinates</b>: the number of atom(s) for species %d changes !\n"
                               "  - configuration N°%d\t :: atom(s)= %d\n"
                               "  - initialization      \t\t :: atom(s)= %d\n", conf, i+1, this_reader -> nsps[i], cif_nsps[i]);
        add_reader_info (str, 0);
        g_free (str);
        return FALSE;
      }
    }
    g_free (tmp_nsps);
  }
  if (! this_reader -> cartesian && cif_use_symmetry_positions)
  {
    // Testing site multiplicity, to ensure that occupancy is not > 1.0
    for (i=0; i<this_reader -> natomes; i++)
    {
      v = this_reader -> occupancy[i];
      for (j=0; j<this_reader -> natomes; j++)
      {
        if (j != i)
        {
          if (this_reader -> coord[i][0] == this_reader -> coord[j][0]
           && this_reader -> coord[i][1] == this_reader -> coord[j][1]
           && this_reader -> coord[i][2] == this_reader -> coord[j][2])
          {
            v += this_reader -> occupancy[j];
            if (v > 1.00001)
            {
              add_reader_info ("<b>Atomic coordinates</b>: a site was found to have an occupancy > 1.0 !\n", 0);
              return FALSE;
            }
          }
        }
      }
    }
  }
  if (! this_reader -> cartesian && this_reader -> chemical)
  {
    // Testing the different number of occupancies
    double * test_occ = allocdouble (1);
    int * num_occ = allocint (1);
    int * test_order = allocint (1);
    int * num_order = allocint (1);
    gboolean new_occ, new_order;
    int occupancies = 1;
    int disorders = 1;
    u = v = test_occ[0] = this_reader -> occupancy[0];
    test_order[0] = this_reader -> disorder[0];
    num_occ[0] = num_order[0] = 1;
    for (i=1; i<this_reader -> natomes; i++)
    {
      new_occ = new_order = TRUE;
      for (j=0; j<occupancies; j++)
      {
        if (test_occ[j] == this_reader -> occupancy[i])
        {
          num_occ[j] ++;
          new_occ = FALSE;
          break;
        }
      }
      if (new_occ)
      {
        test_occ = g_realloc (test_occ, (occupancies+1)*sizeof*test_occ);
        test_occ[occupancies] = this_reader -> occupancy[i];
        u = min (u, test_occ[occupancies]);
        v = max (v, test_occ[occupancies]);
        num_occ = g_realloc (num_occ, (occupancies+1)*sizeof*num_occ);
        num_occ[occupancies] = 1;
        occupancies ++;
      }
      for (j=0; j<disorders; j++)
      {
        if (test_order[j] == this_reader -> disorder[i])
        {
          num_order[j] ++;
          new_order = FALSE;
          break;
        }
      }
      if (new_order)
      {
        test_order = g_realloc (test_order, (disorders+1)*sizeof*test_order);
        test_order[disorders] = this_reader -> disorder[i];
        num_order = g_realloc (num_order, (disorders+1)*sizeof*num_order);
        num_order[disorders] = 1;
        disorders ++;
      }
    }
    if (this_reader -> natomes%occupancies == 0 || this_reader -> natomes%disorders == 0)
    {
      // Atoms can be separated based on site occupancy or site disorder
      // We can consider this as a chemical "reaction or trajectory"
      // As many lattices near by as occupancies or disorders
      new_occ = TRUE;
      for (i=0; i<occupancies; i++)
      {
        if (num_occ[i] != this_reader -> natomes/occupancies)
        {
          new_occ = FALSE;
          break;
        }
     }
      // if (new_occ) add_reader_info ("CIF file is compatible with a chemical reaction: \n\t Reactants can be separated using occupancy\n", 1);
      // g_debug ("min= %f, max= %f, max/min= %f", u, v, v/u);
      new_order = TRUE;
      for (i=0; i<disorders; i++)
      {
        if (num_order[i] != this_reader -> natomes/disorders)
        {
          new_order = FALSE;
          break;
        }
      }
      // if (new_order) add_reader_info ("CIF file is compatible with a chemical reaction: \n\t Reactants can be separated using disorder site\n", 1);
    }
  }
  return TRUE;
}

/*!
  \fn int cif_file_get_number_of_positions (int lid)

  \brief get the number of symmetry positions

  \param lid line to reach
*/
int cif_file_get_number_of_positions (int lid)
{
  gboolean res = FALSE;
  gchar * saved_line;
  gchar * the_line;
  gchar * the_word;
  int i = 0;
  while (! res)
  {
#ifdef OPENMP
    the_line = g_strdup_printf ("%s", coord_line[lid+i]);
#else
    file_get_to_line (lid+i);
    the_line = g_strdup_printf ("%s", tail -> line);
#endif
    the_word = strtok_r (the_line, " ", & saved_line);
    if (the_word[0] == '_' || g_strcmp0(the_word, "loop_") == 0)
    {
      res = TRUE;
      break;
    }
    else
    {
      i ++;
    }
  }
  if (i)
  {
    this_reader -> sym_pos = g_malloc0(i*sizeof*this_reader -> sym_pos);
    int j, k;
    gchar * str;
    gchar * k_word;
    gchar * sym_pos_line;
    for (j=0; j<i; j++)
    {
      this_reader -> sym_pos[j] = g_malloc0(3*sizeof*this_reader -> sym_pos[j]);
#ifdef OPENMP
      sym_pos_line = g_strdup_printf ("%s", coord_line[lid+j]);
#else
      file_get_to_line (lid+j);
      sym_pos_line = g_strdup_printf ("%s", tail -> line);
#endif
      the_line = g_strdup_printf ("%s", sym_pos_line);
      the_word = strtok_r (the_line, " ", & saved_line);
      k_word = g_strdup_printf ("%s", the_word);
      str = g_strdup_printf ("%d", j+1);
      if (g_strcmp0(k_word, str) == 0)
      {
        g_free (the_line);
        the_line = NULL;
        for (k=strlen(k_word); k<strlen(sym_pos_line); k++)
        {
          if (! this_line)
          {
            the_line = g_strdup_printf ("%c", sym_pos_line[k]);
          }
          else
          {
            the_line = g_strdup_printf ("%s%c", the_line, sym_pos_line[k]);
          }
        }
      }
      else
      {
        the_line = g_strdup_printf ("%s", sym_pos_line);
      }
      g_free (k_word);
      g_free (str);
      g_free (sym_pos_line);
      the_line = substitute_string (the_line, " ", NULL);
      the_line = substitute_string (the_line, "'", NULL);
      the_line = substitute_string (the_line, ",", " ");
      the_word = strtok_r (the_line, " ", & saved_line);
      for (k=0; k<3; k++)
      {
         this_reader -> sym_pos[j][k] = g_strdup_printf ("%s", the_word);
         the_word = strtok_r (NULL, " ", & saved_line);
      }
#ifdef DEBUG
      g_debug ("SYM_POS:: pos= %d, x= %s, y= %s, z= %s", j+1, this_reader -> sym_pos[j][0], this_reader -> sym_pos[j][1], this_reader -> sym_pos[j][2]);
#endif // DEBUG
    }
  }
  return i;
}

/*!
  \fn gboolean cif_get_symmetry_positions (int linec, int conf)

  \brief read the symmetry positions from the CIF file

  \param linec total number of lines
  \param conf configuration to read
*/
gboolean cif_get_symmetry_positions (int linec, int conf)
{
  gchar * pos_key[2]={"_symmetry_equiv_pos_as", "_space_group_symop_operation"};
  int loop_line;
  int line_id;
  int i;
  for (i=0; i<2; i++)
  {
    loop_line = get_loop_line_for_key (linec, conf, pos_key[i], "xyz");
    if (loop_line)
    {
      line_id = cif_get_value (pos_key[i], "xyz", loop_line, linec, NULL, FALSE, FALSE, FALSE, FALSE, NULL);
      break;
    }
  }
  if (! loop_line) return FALSE;
  // Read lines after the instruction, as many positions as line until _ or loop
  this_reader -> num_sym_pos = cif_file_get_number_of_positions (line_id);
  return TRUE;
}

/*!
  \fn int get_space_group_from_hm (gchar * hmk)

  \brief retrieve space group using the HM Key

  \param hmk
*/
int get_space_group_from_hm (gchar * hmk)
{
  int i;
  gchar * str;
  gchar * hm = g_strdup_printf ("%s", replace_markup (hmk, "S", NULL));
  for (i=0; i<230; i++)
  {
    str = substitute_string (hmsymbols[i], " ", NULL);
    if (g_strcmp0(str, hm) == 0)
    {
      g_free (str);
      g_free (hm);
      return i+1;
    }
    g_free (str);
    str = substitute_string (groups[i], "<sub>", NULL);
    str = substitute_string (str, "</sub>", NULL);
    if (g_strcmp0(str, hm) == 0)
    {
      g_free (str);
      g_free (hm);
      return i+1;
    }
    g_free (str);
  }
  // Cross checking for erroneus writting in the CIF file
  // ie. Fm3m in place of Fm-3m
  for (i=0; i<230; i++)
  {
    str = substitute_string (groups[i], "<sub>", NULL);
    str = substitute_string (str, "</sub>", NULL);
    if (g_strrstr(str, "-"))
    {
      str = substitute_string (str, "-", NULL);
      if (g_strcmp0(str, hm) == 0)
      {
        g_free (str);
        g_free (hm);
        return i+1;
      }
    }
    g_free (str);
  }
  g_free (hm);
  return 0;
}

/*!
  \fn gchar * get_string_from_origin (space_group * spg)

  \brief get the space group origin from its name

  \param spg space group
*/
gchar * get_string_from_origin (space_group * spg)
{
  gchar * str = NULL;
  if (wnpos[1])
  {
    g_free (wnpos[1]);
    wnpos[1] = NULL;
  }

  get_wyck_char (spg -> coord_origin.m01, 1, 0);
  get_wyck_char (spg -> coord_origin.m11, 1, 1);
  get_wyck_char (spg -> coord_origin.m21, 1, 2);

  if (wnpos[1])
  {
    str = g_strdup_printf ("%s", wnpos[1]);
    g_free (wnpos[1]);
    wnpos[1] = NULL;
  }
  return str;
}

/*!
  \fn int get_setting_from_hm (gchar * hmk, int end)

  \brief Getting the space group parameters using the HM Key

  \param hmk the HM key
  \param end use origin (number of possible SP origins), or not (-1)
*/
int get_setting_from_hm (gchar * hmk, int end)
{
  int i, j;
  gchar * str;
  if (this_reader -> lattice.sp_group)
  {
    i = this_reader -> lattice.sp_group -> nums;
    for (j=0; j<i; j++)
    {
      str = replace_markup (this_reader -> lattice.sp_group -> settings[j].name, "s", "/");
      str = substitute_string (str, "_", NULL);
      if (end < 0)
      {
        if (g_strcmp0(str, hmk) == 0)
        {
          g_free (str);
          return j;
        }
      }
      else
      {
        if (g_strcmp0(str, hmk) == 0 && this_reader -> lattice.sp_group -> settings[j].origin == end+1)
        {
          g_free (str);
          return j;
        }
      }
      g_free (str);
    }
    if (this_reader -> lattice.sp_group -> id > 2 && this_reader -> lattice.sp_group -> id < 16)
    {
      // This is a way around the way this familly of SG is often written,
      // using incomplete or inexact hmk keyword, ex: P21/a instead of P121/a1
      for (j=0; j<i; j++)
      {
        str = replace_markup (this_reader -> lattice.sp_group -> settings[j].name, "s", "/");
        str = substitute_string (str, "_", NULL);
        str = substitute_string (str, "12", "2");
        str = substitute_string (str, "/a1", "/a");
        str = substitute_string (str, "/b1", "/b");
        str = substitute_string (str, "/c1", "/c");
        str = substitute_string (str, "/m1", "/m");
        str = substitute_string (str, "/n1", "/n");
        if (end < 0)
        {
          if (g_strcmp0(str, hmk) == 0)
          {
            g_free (str);
            str = g_strdup_printf ("<b>Space group</b>: CIF file information could be inaccurate !\n"
                                   " CIF file space group: <b>%s</b>, CIF file H-M symbol: <b>%s</b>\n",
                                   groups[this_reader -> lattice.sp_group -> id-1], hmk);
            add_reader_info (str, 1);
            g_free (str);
            return j;
          }
        }
        else
        {
          if (g_strcmp0(str, hmk) == 0 && this_reader -> lattice.sp_group -> settings[j].origin == end+1)
          {
            g_free (str);
            str = g_strdup_printf ("<b>Space group</b>: CIF file information could be inaccurate !\n"
                                   " CIF file space group: <b>%s</b>, CIF file H-M symbol: <b>%s</b>\n",
                                   groups[this_reader -> lattice.sp_group -> id-1], hmk);
            add_reader_info (str, 1);
            g_free (str);
            return j;
          }
        }
        g_free (str);
      }
    }
    return -1;
  }
  else
  {
    return -1;
  }
}

/*!
  \fn int group_info_from_hm_key (int spg, gchar * key_hm)

  \brief get the space group information using the HM key from the CIF file

  \param spg space group
  \param key_hm HM key
*/
int group_info_from_hm_key (int spg, gchar * key_hm)
{
  int i, j;
  gchar * str;
  gchar * exts[2] = {"h", "r"};
  gchar * orig[2] = {"1", "2"};
  gchar * key = NULL;
  gchar * hmk = NULL;
  gchar * hma, * hmb;
  gchar * hmkey = substitute_string (key_hm, "'", NULL);
  //hmkey = substitute_string (hmkey, "/", "s");
  this_reader -> setting = -1;
  if (strstr(hmkey,":"))
  {
    key = g_strdup_printf ("%s", hmkey);
    hmk = g_strdup_printf ("%s", strtok (key, ":"));
    hma = g_strdup_printf ("%s:", hmk);
    hmb = replace_markup (hmkey, hma, NULL);
    i = strlen(hmb);
    hmb = g_ascii_strdown (hmb, i);
    for (i=0; i<2; i++)
    {
      if (g_strcmp0(hmb, exts[i]) == 0)
      {
        this_reader -> setting = i;
        break;
      }
    }
    g_free (hma);
    g_free (key);
    if (this_reader -> setting < 0)
    {
      for (i=0; i<2; i++)
      {
        if (g_strcmp0(hmb, orig[i]) == 0)
        {
          j = get_space_group_from_hm (hmk);
          this_reader -> setting = (spg) ? get_setting_from_hm (hmk, i) : 0;
          if (! j && this_reader -> setting < 0)
          {
            this_reader -> setting = 0;
            str = g_strdup_printf ("<b>Space group</b>: CIF file information could be inaccurate !\n"
                                   " CIF file space group: <b>%s</b>, CIF file H-M symbol: <b>%s</b>\n",
                                   groups[this_reader -> lattice.sp_group -> id-1], key_hm);
            add_reader_info (str, 1);
            g_free (str);
          }
          if (this_reader -> setting < 0) this_reader -> setting = 0;
          g_free (hmk);
          g_free (hmb);
          return j;
        }
      }
    }
    g_free (hmb);
  }
  else
  {
    hmk = g_strdup_printf ("%s", hmkey);
  }
  j = get_space_group_from_hm (hmk);
  this_reader -> setting = (spg || j) ? get_setting_from_hm (hmk, -1) : 0;
  g_free (hmk);
  if (! j && this_reader -> setting < 0)
  {
    str = g_strdup_printf ("<b>Space group</b>: CIF file information could be inaccurate !\n"
                           " CIF file space group: <b>%s</b>, CIF file H-M symbol: <b>%s</b>\n",
                           groups[this_reader -> lattice.sp_group -> id-1], key_hm);
    add_reader_info (str, 1);
    g_free (str);
  }
  if (this_reader -> setting < 0) this_reader -> setting = 0;
  return (spg) ? (j) ? j : spg : j;
}

/*!
  \fn gboolean cif_get_cell_data (int linec, int conf)

  \brief get the cell data from the CIF file

  \param linec total number of lines
  \param conf configuration to read
*/
gboolean cif_get_cell_data (int linec, int conf)
{
  gchar * cellkeys[3] = {"length_a", "length_b", "length_c"};
  gchar * cellangs[3] = {"angle_alpha", "angle_beta", "angle_gamma",};
  int i, j;
  gchar * str = NULL;
  if (! conf || (conf && active_project -> steps == 1))
  {
    this_reader -> lattice.box = g_malloc0(active_project -> steps*sizeof*this_reader -> lattice.box);
  }
  i = (conf && active_project -> steps == 1) ? 0 : conf;
  for (j=0; j<3; j++)
  {
    str = cif_retrieve_value (linec, conf, "_cell", cellkeys[j], TRUE, TRUE, TRUE);
    if (! str)
    {
      str = g_strdup_printf ("<b>Lattice parameters</b>: impossible to retrieve the '%s' parameter !\n", box_prop[0][j]);
      add_reader_info (str, 0);
      g_free (str);
      return FALSE;
    }
    this_reader -> lattice.box[i].param[0][j] = string_to_double ((gpointer)str);
    if (i)
    {
      if (this_reader -> lattice.box[i].param[0][j] != this_reader -> lattice.box[i-1].param[0][j]) active_cell -> npt = TRUE;
    }
#ifdef DEBUG
    g_debug ("CIF:: box[%d][%d]= %f", i, j, this_reader -> lattice.box[i].param[0][j]);
#endif
    g_free (str);
    str = cif_retrieve_value (linec, conf, "_cell", cellangs[j], TRUE, TRUE, TRUE);
    if (! str)
    {
      str = g_strdup_printf ("<b>Lattice parameters</b>: impossible to retrieve the '%s' parameter !\n", box_prop[1][j]);
      add_reader_info (str, 0);
      g_free (str);
      return FALSE;
    }
    this_reader -> lattice.box[i].param[1][j] = string_to_double ((gpointer)str);
    g_free (str);
    if (i)
    {
      if (this_reader -> lattice.box[i].param[1][j] != this_reader -> lattice.box[i-1].param[1][j]) active_cell -> npt = TRUE;
    }
#ifdef DEBUG
    g_debug ("CIF:: angle[%d][%d]= %f", i, j, this_reader -> lattice.box[i].param[1][j]);
#endif
  }
  this_reader -> lattice.ltype = 0;
  compute_lattice_properties (& this_reader -> lattice, i);
  for (i=0; i<3; i++) this_reader -> lattice.cextra[i] = 1;
  return TRUE;
}

/*!
  \fn int cif_get_space_group (int linec, int conf)

  \brief get the space group from the CIF file

  \param linec total number of lines
  \param conf configuration to read
*/
int cif_get_space_group (int linec, int conf)
{
  gchar * symkey[2] = {"int_tables_number", "group_it_number"};
  gchar * str = NULL;
  int spg = 0;
  int i, j, k, l;

  for (i=0; i<2; i++)
  {
    str = cif_retrieve_value (linec, conf, "_symmetry", symkey[i], TRUE, FALSE, FALSE);
    if (str)
    {
      spg = (int)string_to_double ((gpointer)str);
      break;
    }
  }
  if (! spg)
  {
    str = cif_retrieve_value (linec, conf, "_space_group", "it_number", TRUE, FALSE, FALSE);
    if (str)
    {
      spg = (int)string_to_double ((gpointer)str);
    }
  }
  gchar * hmkey = NULL;

  hmkey = cif_retrieve_value (linec, conf, "_symmetry", "space_group_name_h-m", TRUE, FALSE, FALSE);
  if (! hmkey)
  {
    hmkey = cif_retrieve_value (linec, conf, "_symmetry", "name_h-m_alt", TRUE, FALSE, FALSE);
  }
  if (! hmkey && ! spg)
  {
    add_reader_info ("<b>Space group</b>: no space group and no H-M symbol found !\n", 1);
    return FALSE;
  }
#ifdef DEBUG
  if (spg) g_debug ("CIF:: Space group:: N°= %d, name= %s", spg, groups[spg-1]);
  if (hmkey) g_debug ("CIF:: H-M symbol:: %s", hmkey);
#endif
  if (spg)
  {
    if (! read_space_group (NULL, spg-1)) return FALSE;
  }
  if (hmkey)
  {
    i = group_info_from_hm_key (spg, hmkey);
    if (! spg && ! i)
    {
      add_reader_info ("<b>Space group</b>: no space group found, unknown H-M symbol !\n", 1);
#ifdef DEBUG
      g_debug ("CIF:: No space group found, unknown H-M symbol !");
#endif
    }
    else if (spg && ! i)
    {
      str = g_strdup_printf ("<b>Space group</b>: space group and H-M symbol do not match !\n"
                             " CIF file space group: <b>%s</b>, CIF file H-M symbol: <b>%s</b>\n", groups[spg-1], hmkey);
      add_reader_info (str, 1);
      g_free (str);
#ifdef DEBUG
      g_debug ("CIF:: Space group and H-M symbol do not match:: spg= %d, hm= %d", spg, i);
#endif
    }
    else if (i && ! spg)
    {
      spg = i;
    }
    if (! this_reader -> lattice.sp_group)
    {
      if (! read_space_group (NULL, spg-1)) return FALSE;
    }
  }
  gchar * lat;
  int res = spg;
  if (spg > 1)
  {
    str = cif_retrieve_value (linec, conf, "_space_group", "it_coordinate_system_code", TRUE, FALSE, FALSE);
    if (str)
    {
      str = substitute_string (str, "'", NULL);
      for (i=0; i<40; i++)
      {
        if (g_strcmp0(str, cif_coord_opts[i][0]) == 0)
        {
          if (i < 18)
          {
            if (spg < 3 || spg > 15)
            {
              res = 0;
              break;
            }
            if (str[0] == '-')
            {
              k = (int) string_to_double ((gpointer)g_strdup_printf ("%c", str[2]));
              str = g_strdup_printf ("%c%c", str[0], str[1]);
            }
            else
            {
              k = (int) string_to_double ((gpointer)g_strdup_printf ("%c", str[1]));
              str = g_strdup_printf ("%c", str[0]);
            }
            l = 0;
            for (j=0; j< this_reader -> lattice.sp_group -> nums; j++)
            {
              this_reader -> lattice.sp_group -> sid = j;
              get_origin (this_reader -> lattice.sp_group);
              lat = get_string_from_origin (this_reader -> lattice.sp_group);
              if (g_strcmp0(lat, str) == 0) l ++;
              if (l == k)
              {
                if (j < this_reader -> setting) add_reader_info ("<b>Space group</b>: ambiguous space group setting !\n", 1);
                this_reader -> setting = j;
                break;
              }
            }
          }
          else if (i < 36)
          {
            if (spg < 16 || spg > 74)
            {
              res = 0;
              break;
            }
            k = 0;
            if (str[0] == '1' || str[0]=='2')
            {
              k = (int) string_to_double ((gpointer)g_strdup_printf ("%c", str[0]));
              str = replace_markup (str, g_strdup_printf("%d", k), NULL);
            }
            l = 0;
            for (j=0; j< this_reader -> lattice.sp_group -> nums; j++)
           {
              lat = g_strdup_printf ("%s%s%s",
                                     this_reader -> lattice.sp_group -> settings[j].pos[0],
                                     this_reader -> lattice.sp_group -> settings[j].pos[1],
                                     this_reader -> lattice.sp_group -> settings[j].pos[2]);
              if (g_strcmp0(lat, str) == 0)
              {
                if (j < this_reader -> setting || this_reader -> lattice.sp_group -> settings[j].origin != k)
                {
                  add_reader_info ("<b>Space group</b>: ambiguous space group setting !\n", 1);
                }
                this_reader -> setting = j;
                l = 1;
                break;
              }
            }
            if (! l) add_reader_info ("<b>Space group</b>: ambiguous space group setting !\n", 1);
          }
          else if (i < 38)
          {
            if (spg < 75 || (spg > 142 && spg < 195))
            {
              res = 0;
              break;
            }
            j = i - 36;
            if (j < this_reader -> setting) add_reader_info ("<b>Space group</b>: ambiguous space group setting !\n", 1);
            this_reader -> setting = j;
          }
          else
          {
            if (spg < 143 || spg > 165)
            {
              res = 0;
              break;
            }
            j = i - 38;
            if (j < this_reader -> setting) add_reader_info ("<b>Space group</b>: ambiguous space group setting !\n", 1);
            this_reader -> setting = j;
          }
        }
      }
      g_free (str);
    }

    if (spg > 142 && spg < 168)
    {
      // Trigonal space group
      gboolean correct_this = FALSE;
      box_info * box = (conf && active_project -> steps == 1) ? & this_reader -> lattice.box[0] : & this_reader -> lattice.box[conf];
      switch (this_reader -> setting)
      {
        case 0:
          if (box -> param[0][0] == box -> param[0][1] && box -> param[0][0] == box -> param[0][2])
          {
            if (box -> param[1][0] == box -> param[1][1] && box -> param[1][0] == box -> param[1][2])
            {
              correct_this = TRUE;
            }
          }
          break;
        case 1:
          if (box -> param[0][0] == box -> param[0][1] && box -> param[0][0] != box -> param[0][2])
          {
            if (box -> param[1][0] == box -> param[1][1] && box -> param[1][0] == 90.0 && box -> param[1][2] == 120.0)
            {
              correct_this = TRUE;
            }
          }
          break;
      }
      if (correct_this)
      {
        gchar * setc[2] = {"<b>hexagonal</b>", "<b>rhombohedral</b>"};
        str = g_strdup_printf ("<b>Space group</b>: found trigonal space group N°%d-%s, %s setting\n"
                               "but the lattice parameters were found in %s format ...\n"
                               "\t ... the space group setting was modified accordingly !\n",
                               spg, groups[spg-1], setc[this_reader -> setting], setc[! this_reader -> setting]);
        add_reader_info (str, 1);
        g_free (str);
        this_reader -> setting = ! this_reader -> setting;
      }
    }

    // Test space group vs. box parameters:
    this_reader -> lattice.sp_group -> sid = this_reader -> setting;
    if (! test_lattice (NULL, & this_reader -> lattice))
    {
      str = g_strdup_printf ("<b>Space group</b> and <b>lattice parameters</b> are not compatible !\n"
                             "\nCheck a, b, c, and &#x3B1;, &#x3B2;, &#x263;, with the type of crystal system.\n");
      add_reader_info (str, 0);
      g_free (str);
      res = -1;
    }
  }
  this_reader -> lattice.sp_group -> sid = this_reader -> setting;
  get_origin (this_reader -> lattice.sp_group);
  return res;
}

/*!
  \fn int open_cif_configuration (int line_start, int line_end, int cif_step)

  \brief open CIF configuration in CIF file

  \param linec total number of lines
  \param conf configuration to read
*/
int open_cif_configuration (int linec, int conf)
{
  int res;
  int i, j, k, l, m, n;
  int cid;
  if (cif_get_cell_data (linec, conf))
  {
    i = cif_get_space_group (linec, conf);
    if (conf && active_project -> steps > 1 && i != saved_group)
    {
#ifdef DEBUG
      g_debug ("CIF:: SP group changes between configuration:: conf= %d, saved_group= %d, new_group= %d", conf, saved_group, i);
#endif
      add_reader_info ("Space group changes between configurations !\n", 0);
      return 3;
    }
    else if (i > 0)
    {
      saved_group = i;
#ifdef DEBUG
      g_debug ("CIF:: SP setting:: %d, name= %s", this_reader -> setting+1, this_reader -> lattice.sp_group -> settings[this_reader -> setting].name);
#endif
      if (this_reader -> lattice.sp_group) get_origin (this_reader -> lattice.sp_group);
    }
    else if (i == 0)
    {
      // No space group found
#ifdef DEBUG
      g_debug ("CIF:: Impossible to retrieve space group information !");
#endif
    }
    else if (cif_multiple && ! this_reader -> cartesian)
    {
      // Error in space group
      return 3;
    }
  }
  else if (! this_reader -> cartesian)
  {
    // Error no cell data using fractional coordinates
    return 3;
  }

  // Reading positions
  if (cif_get_symmetry_positions (linec, conf))
  {
    if (! cif_use_symmetry_positions && this_reader -> num_sym_pos)
    {
      add_reader_info ("Symmetry position(s) in CIF file\n", 1);
    }
  }
  if (cif_use_symmetry_positions && ! this_reader -> num_sym_pos)
  {
    add_reader_info ("No symmetry position(s) in CIF file\n", 0);
    return 3;
  }
  if (cif_get_atomic_coordinates (linec, conf))
  {
    cid = (conf && active_project -> steps == 1) ? 0 : conf;
    if (! this_reader -> cartesian)
    {
      for (i=0; i<3; i++)
      {
        for (j=0; j<3; j++)
        {
          if (i < 2)
          {
            active_cell -> box[cid].param[i][j] = this_reader -> lattice.box[cid].param[i][j];
          }
          active_cell -> box[cid].vect[i][j] = this_reader -> lattice.box[cid].vect[i][j];
        }
      }
      active_cell -> ltype = 1;
      active_cell -> pbc = TRUE;
      active_cell -> has_a_box = TRUE;
      if (this_reader -> lattice.sp_group)
      {
        active_cell -> crystal = TRUE;
        active_cell -> sp_group = duplicate_space_group (this_reader -> lattice.sp_group);
      }
    }
    res = 0;
    if (cif_use_symmetry_positions)
    {
      gchar * str = g_strdup_printf ("<b>Building crystal using symmetry positions: </b> \n"
                                     "  1) evaluate candidate atomic positions using data in CIF file: \n"
                                     "     - symmetry positions\n"
                                     "     - atomic coordinates + occupancy\n"
                                     "  2) fill each candidate position using the associated occupancy: \n"
                                     "     - occupancy %s\n"
                                     "     - %s\n", cif_occ[this_reader -> rounding], cif_sites[1]);
      add_reader_info (str, 1);
      g_free (str);
      this_reader -> cartesian = TRUE;
      compute_lattice_properties (active_cell, cid);
      double spgpos[3][4];
      int max_pos = this_reader -> num_sym_pos * this_reader -> natomes;
      gboolean dist_message = FALSE;
      gboolean low_occ = FALSE;
      gboolean save_it;
      vec3_t f_pos, c_pos;
      gboolean * save_pos = allocbool (max_pos);
      mat4_t pos_mat;
      atom at, bt;
      distance dist;
      double u;
      vec3_t * all_pos = g_malloc0(max_pos*sizeof*all_pos);
      int * all_origin = allocint (max_pos);
      int * cif_pos = allocint (this_reader -> natomes);
      double ** cryst_pos = allocddouble (this_reader -> natomes, 3);
      double ** occ_pos = g_malloc0(sizeof*occ_pos);
      int ** lot_pos = g_malloc0(sizeof*lot_pos);

      double * list_occ = allocdouble (this_reader -> natomes);
      double val;
      int vbl;
      int * list_pos = allocint (this_reader -> natomes);
      for (i=0; i<this_reader -> natomes; i++)
      {
        list_pos[i] = i;
        list_occ[i] = this_reader -> occupancy[i];
      }
      for (i=1; i<this_reader -> natomes; i++)
      {
        val = list_occ[i];
        vbl = list_pos[i];
        for (j=i-1; j>-1; j--)
        {
          if (list_occ[j] >= val) break;
          list_occ[j+1] = list_occ[j];
          list_pos[j+1] = list_pos[j];
        }
        list_occ[j+1] = val;
        list_pos[j+1] = vbl;
      }
      int num_pos = 0;
      int pos_max = 0;
      // The following might be modified for reaction CIF:
      //  - this_reader -> coord[i] and this_readaer -> coord[j] are not necessarily equal
      //  - both the disorder group and the occupancy matters in place of the coordinates
      for (i=0; i<2; i++)
      {
        for (j=0; j<this_reader -> natomes; j++)
        {
          k = list_pos[j];
          if (! j)
          {
            num_pos = 0;
            for (l=0; l<3; l++) cryst_pos[num_pos][l] = this_reader -> coord[k][l];
            cif_pos[num_pos] = 1;
            if (i)
            {
              occ_pos[num_pos][0] = this_reader -> occupancy[k];
              lot_pos[num_pos][0] = this_reader -> lot[k];
            }
            num_pos ++;
            pos_max = 1;
          }
          else
          {
            save_it = TRUE;
            for (l=0; l<num_pos; l++)
            {
              if (this_reader -> coord[k][0] == cryst_pos[l][0]
               && this_reader -> coord[k][1] == cryst_pos[l][1]
               && this_reader -> coord[k][2] == cryst_pos[l][2])
              {
                save_it = FALSE;
                break;
              }
            }
            if (save_it)
            {
              for (l=0; l<3; l++) cryst_pos[num_pos][l] = this_reader -> coord[k][l];
              cif_pos[num_pos] = 1;
              if (i)
              {
                occ_pos[num_pos][0] = this_reader -> occupancy[k];
                lot_pos[num_pos][0] = this_reader -> lot[k];
              }
              num_pos ++;
            }
            else
            {
              if (i)
              {
                occ_pos[l][cif_pos[l]] = this_reader -> occupancy[k];
                lot_pos[l][cif_pos[l]] = this_reader -> lot[k];
              }
              cif_pos[l] ++;
              pos_max = max (pos_max, cif_pos[l]);
            }
          }
        }
        if (! i)
        {
          occ_pos = allocddouble (num_pos, pos_max);
          lot_pos = allocdint (num_pos, pos_max);
        }
      }
      g_free (list_occ);
      g_free (list_pos);
      for (i=0; i<num_pos; i++)
      {
        u = 0;
        for (j=0; j<cif_pos[i]; j++)
        {
          u += occ_pos[i][j];
        }
        if (u < 1.0)
        {
          low_occ = TRUE;
          break;
        }
      }
      int * all_id = allocint (num_pos);
      l = m = 0;
      for (i=0; i<this_reader -> num_sym_pos; i++)
      {
        for (j=0; j<3; j++)
        {
          tmp_pos = g_strdup_printf ("%s", this_reader -> sym_pos[i][j]);
          for (k=0; k<3; k++)
          {
            spgpos[j][k] = get_val_from_wyckoff (vect_comp[k], this_reader -> sym_pos[i][j]);
          }
          if (tmp_pos)
          {
            spgpos[j][3] = get_value_from_pos (tmp_pos);
            g_free (tmp_pos);
            tmp_pos = NULL;
          }
          else
          {
            spgpos[j][3] = 0.0;
          }
        }
        pos_mat = mat4 (spgpos[0][0], spgpos[0][1], spgpos[0][2], spgpos[0][3],
                        spgpos[1][0], spgpos[1][1], spgpos[1][2], spgpos[1][3],
                        spgpos[2][0], spgpos[2][1], spgpos[2][2], spgpos[2][3],
                        0.0, 0.0, 0.0, 1.0);
        for (j=0; j<num_pos; j++)
        {
          f_pos = vec3 (cryst_pos[j][0], cryst_pos[j][1], cryst_pos[j][2]);
          f_pos = m4_mul_coord (pos_mat, f_pos);
          c_pos = m4_mul_coord (this_reader -> lattice.box[cid].frac_to_cart, f_pos);
          all_pos[l].x = c_pos.x;
          all_pos[l].y = c_pos.y;
          all_pos[l].z = c_pos.z;
          all_origin[l] = j;
          save_it = TRUE;
          if (l)
          {
            at.x = all_pos[l].x;
            at.y = all_pos[l].y;
            at.z = all_pos[l].z;
            for (k=0; k<l; k++)
            {
              bt.x = all_pos[k].x;
              bt.y = all_pos[k].y;
              bt.z = all_pos[k].z;
              dist = distance_3d (active_cell, 0, & at, & bt);
              if (dist.length < 0.1)
              {
                dist_message = TRUE;
                save_it = FALSE;
                break;
              }
            }
          }
          save_pos[l] = save_it;
          l ++;
          if (save_it)
          {
            all_id[j] ++;
            m ++;
          }
        }
      }
      double prob;
      gboolean pick_it;
      gboolean ** taken_pos = g_malloc0 (num_pos*sizeof*taken_pos);
      int ** site_lot = g_malloc0 (num_pos*sizeof*site_lot);
      clock_t CPU_time;
      int tot_pos = 0;
      for (i=0; i<num_pos; i++)
      {
        taken_pos[i] = allocbool(all_id[i]);
        site_lot[i] = allocint(all_id[i]);
        for (j=0; j<cif_pos[i]; j++)
        {
          u = occ_pos[i][j]*all_id[i];
          if (u < 1.0 && tot_pos < all_id[i]) u = 1.0;
          k = lot_pos[i][j];
          l = 0;
          // Warning for occupancy closest integer value to u:
          //   - (int)u ?
          //   - nearbyint (u) : closest integer value ?
          while (l < occupancy(u, this_reader -> rounding))
          {
            CPU_time = clock ();
            m = (CPU_time - (j+17)*all_id[i]);
            prob = random3_(& m);
            m = round (prob * (all_id[i]-1));
            pick_it = ! taken_pos[i][m];
            if (pick_it)
            {
              site_lot[i][m] = k;
              taken_pos[i][m] = TRUE;
              l ++;
              tot_pos ++;
            }
          }
        }
      }
      crystal_data * cryst = allocate_crystal_data (tot_pos, this_reader -> nspec + this_reader -> object_to_insert);
      i = 0;
      int * cryst_lot = allocint (cryst -> objects);
      int * from_origin = allocint (num_pos);
      for (j=0; j<max_pos; j++)
      {
        if (save_pos[j])
        {
          k = all_origin[j];
          l = from_origin[k];
          if (taken_pos[k][l])
          {
            cryst -> coord[i] = g_malloc0(sizeof*cryst -> coord[i]);
            cryst -> coord[i][0].x = all_pos[j].x;
            cryst -> coord[i][0].y = all_pos[j].y;
            cryst -> coord[i][0].z = all_pos[j].z;
            cryst -> pos_by_object[i] = 1;
            cryst_lot[i] = site_lot[k][l];
            if (cryst_lot[i] < 0)
            {
              cryst -> at_by_object[i] = get_atomic_object_by_origin (cif_object, - cryst_lot[i] - 1, 0) -> atoms;
            }
            else
            {
              cryst -> at_by_object[i] = 1;
            }
            i ++;
          }
          from_origin[k] ++;
        }
      }
      g_free (site_lot);
      g_free (all_origin);
      g_free (from_origin);
      g_free (all_pos);
      g_free (save_pos);
      g_free (taken_pos);
      g_free (all_id);
      i = 0;
      for (j=0; j<cryst -> objects; j++)
      {
        i += cryst -> at_by_object[j] * cryst -> pos_by_object[j];
      }
      if (! conf || active_project -> steps == 1)
      {
        active_project -> natomes = i;
        allocatoms (active_project);
      }
      atomic_object * c_obj;
      int * spec_num = allocint (120);
      i = 0;
      for (j=0; j<cryst -> objects; j++)
      {
        if (cryst_lot[j] < 0)
        {
          k = - cryst_lot[j] - 1;
          c_obj = get_atomic_object_by_origin (cif_object, k, 0);
          for (l=0; l<c_obj -> atoms; l++)
          {
            m = c_obj -> at_list[l].sp;
            n = c_obj -> old_z[m];
            spec_num[n] ++;
            active_project -> atoms[cid][i].sp = n;
            active_project -> atoms[cid][i].x = cryst -> coord[j][0].x + c_obj -> at_list[l].x;
            active_project -> atoms[cid][i].y = cryst -> coord[j][0].y + c_obj -> at_list[l].y;
            active_project -> atoms[cid][i].z = cryst -> coord[j][0].z + c_obj -> at_list[l].z;
            i ++;
          }
        }
        else
        {
          k = (int)this_reader -> z[cryst_lot[j]];
          spec_num[k] ++;
          active_project -> atoms[cid][i].sp = k;
          active_project -> atoms[cid][i].x = cryst -> coord[j][0].x;
          active_project -> atoms[cid][i].y = cryst -> coord[j][0].y;
          active_project -> atoms[cid][i].z = cryst -> coord[j][0].z;
          i ++;
        }
      }
      g_free (this_reader -> nsps);
      int * tmp_nsps = allocint (120);
      int * tmp_spid = allocint (120);
      i = 0;
      for (j=0; j<120; j++)
      {
        if (spec_num[j])
        {
          tmp_nsps[i] = spec_num[j];
          tmp_spid[j] = i;
          i++;
        }
      }
      this_reader -> nspec = i;
      this_reader -> nsps = allocint (i);
      for (i=0; i<this_reader -> nspec; i++) this_reader -> nsps[i] = tmp_nsps[i];
      g_free (tmp_nsps);
      g_free (this_reader -> z);
      this_reader -> z = allocdouble(i);
      i = 0;
      for (j=0; j<120; j++)
      {
        if (spec_num[j])
        {
          this_reader -> z[i] = (double)j;
          i ++;
        }
      }
      for (i=0; i<active_project -> natomes; i++)
      {
        j = active_project -> atoms[cid][i].sp;
        k = tmp_spid[j];
        active_project -> atoms[cid][i].sp = k;
      }
      g_free (tmp_spid);
      if (low_occ)
      {
        add_reader_info ("The crystal will be created however some objects might be missing,\n"
                         "Occupancy is too low compared to the number of site(s) per cell.\n\n"
                         "<b>To build a crystal matching the defined occupancy</b>:\n"
                         "\t <b>1)</b> If you are trying to read a CIF file, use the crystal builder instead.\n"
                         "\t <b>2)</b> Modify the occupancy set-up to 'Completely random'.\n"
                         "\t <b>3)</b> Increase the number of unit cells up to get rid of this message.\n\n", 1);
      }
      if (dist_message)
      {
        add_reader_info ("Object(s) at equivalent positions have been removed\n"
                         "to ensure the consistency of the model\n"
                         "when using <b>P</b>eriodic <b>B</b>oundary <b>C</b>onditions\n ", 1);
      }
    }
  }
  else
  {
    // No coordinates found
#ifdef DEBUG
    g_debug ("CIF:: Impossible to retrieve atomic coordinates !");
#endif
    res = 2;
  }
  return res;
}

/*!
  \fn int open_cif_file (int linec)

  \brief open CIF file

  \param linec Total number of lines
*/
int open_cif_file (int linec)
{
  gchar * frackeys[3] = {"fract_x", "fract_y", "fract_z"};
  gchar * cartkeys[3] = {"cartn_x", "cartn_y", "cartn_z"};
  gchar * str = NULL;
  int cif_action = 0;
  int cif_step = 1;
  int cif_site;
  int cif_occup = 0;
  int i, j;

  cif_multiple = TRUE;
  // Determine the number of configuration(s) by checking the presence
  // of the instruction used to declare atomic coordinates
  this_reader -> steps = cif_get_value ("_atom_site", frackeys[0], 0, linec, NULL, FALSE, FALSE, TRUE, FALSE, NULL);
  if (! this_reader -> steps)
  {
    this_reader -> steps = cif_get_value ("_atom_site", cartkeys[0], 0, linec, NULL, FALSE, FALSE, TRUE, FALSE, NULL);
    this_reader -> cartesian = TRUE;
  }
  else
  {
    // How to treat occupancy
    cif_occup = cif_get_value ("_atom_site", "occupancy", 0, linec, NULL, FALSE, FALSE, TRUE, FALSE, NULL);
    if (cif_occup)
    {
      this_reader -> rounding = iask ("Please select how to handle occupancy", "Select how to handle occupancy", 5, MainWindow);
      if (this_reader -> rounding < 0 || this_reader -> rounding > 2) this_reader -> rounding = 2;
      if (! cif_use_symmetry_positions)
      {
        str = g_strdup_printf ("Occupancy %s\n\t%s\n", cif_occ[this_reader -> rounding], cif_sites[cif_use_symmetry_positions]);
        add_reader_info (str, 1);
        g_free (str);
      }
    }
  }

  if (this_reader -> steps > 1)
  {
    str = g_strdup_printf ("It seems the CIF file contains <b>%d</b> distinct configurations\n", this_reader -> steps);
    add_reader_info (str, 1);
    g_free (str);
    if (cif_use_symmetry_positions)
    {
      str = g_strdup_printf ("Impossible to use symmetry positions with multiple configurations\n");
      add_reader_info (str, 1);
      g_free (str);
    }
  }

  if (this_reader -> steps > 1 && ! cif_use_symmetry_positions)
  {
    cif_site = cif_get_value ("_atom_site", "disorder_group", 0, linec, NULL, FALSE, FALSE, TRUE, FALSE, NULL);
    if (cif_occup == this_reader -> steps && cif_occup == cif_site)
    {
      add_reader_info ("This CIF file could be describing a trajectory or a chemical reaction.\n", 1);
      // This is where to ask what to do !
      // Read like a chemical reaction: read one configuration sort coordinates by occupancy
      // Read all as trajectory CIF file: forget about using occupancy to sort coordinates
      //   - possible if the number of atom(s) by configuration remains constant
      // Read only a selected configuration and:
      //   - read as normal CIF file, forget about using occupancy to sort coordinates
      // Because what follow will depend on this choice
      /* cif_action = iask ("Please select how to process the data in the CIF file", "Select how to process data", 3, MainWindow);
      this_reader -> chemical = ! cif_action;
      cif_action = (cif_action == 1) ? 0 : 1; */
      cif_action = iask ("Please select how to process the data in the CIF file", "Select how to process data", 4, MainWindow);
    }
    else
    {
      add_reader_info ("This CIF file could be describing a trajectory.\n", 1);
      cif_action = iask ("Please select how to process the data in the CIF file", "Select how to process data", 4, MainWindow);
    }
  }
  else
  {
    cif_action = 1;
  }

  if (cif_action && this_reader -> steps > 1)
  {
    // We need to select the step to work on
    str = g_strdup_printf ("Select the configuration, in [1- %d]", this_reader -> steps);
    cif_step = 0;
    while (! cif_step)
    {
      cif_step = iask ("Please select the configuration to work on", str, 0, MainWindow);
      if (cif_step < 1 || cif_step > this_reader -> steps) cif_step = 0;
    }
    g_free (str);
  }
  if (cif_action)
  {
    active_project -> steps = 1;
    if (this_reader -> steps > 1)
    {
      str = g_strdup_printf ("Working on configuration N°%d\n", cif_step);
      add_reader_info (str, 1);
      g_free (str);
    }
  }

  if (cif_action)
  {
    return open_cif_configuration (linec, cif_step - 1);
  }
  else
  {
    active_project -> steps = this_reader -> steps;
    g_free (active_cell -> box);
    active_cell -> box = g_malloc0(this_reader -> steps*sizeof*active_cell -> box);
    // For each configuration open it:
    i = 0;
    for (j=0; j<active_project -> steps; j++)
    {
      this_reader -> nspec = 0;
      if (this_reader -> nsps)
      {
        g_free (this_reader -> nsps);
        this_reader -> nsps = NULL;
      }
      i = open_cif_configuration (linec, j);
      if (i) return i;
    }
    return 0;
  }
}
