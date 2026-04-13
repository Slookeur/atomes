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
* @file cp2k_mol.c
* @short Functions to fix fragment(s) when creating the CP2K input file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cp2k_mol.c'
*
* Contains:
*

 - The functions to fix fragment(s) when creating the CP2K input file

 Note: this is not used for the time being !

*
* List of functions:

  void frag_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void frag_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void cp2k_fix_molecule ();

  G_MODULE_EXPORT void select_frag (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void select_fixed_atom_confirm (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_cp2k_fix_molecule (GtkDialog * dial, gint response_id, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "cp2k.h"
#include "calc.h"

extern void proj_unselect_all_atoms ();
extern ColRGBA init_color (int id, int numid);
extern G_MODULE_EXPORT void cp2k_select_coord_id (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
extern int at_col;
extern int ** old_fixed;
int a_frag;
int * fix_frag;
extern GtkTreeStore * add_model;

/*!
  \fn G_MODULE_EXPORT void select_frag (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select molecule toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*
*/
G_MODULE_EXPORT void select_frag (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(add_model), & iter, path);
  int i;
  gtk_tree_model_get (GTK_TREE_MODEL(add_model), & iter, 0, & i, -1);
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    fix_frag[i-1] = 0;
    a_frag --;
  }
  else
  {
    a_frag ++;
    fix_frag[i-1] = 1;
  }
  gtk_tree_store_set (add_model, & iter, 1, fix_frag[i-1], -1);
}

/*!
  \fn void frag_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer in the CP2K molecule tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void frag_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i;
  gtk_tree_model_get (mod, iter, 1, & i, -1);
  gtk_cell_renderer_set_visible (renderer, i);
}

/*!
  \fn void frag_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color in the CP2K molecule tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void frag_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  gtk_tree_model_get (mod, iter, 1, & j, -1);
  set_renderer_color (j, renderer, init_color (i, qm_coord -> totcoord[2]));
}

gboolean sel_and_conf;

/*!
  \fn G_MODULE_EXPORT void select_fixed_atom_confirm (GtkDialog * dialog, gint response_id, gpointer data)

  \brief confirm fix selection

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_fixed_atom_confirm (GtkDialog * dialog, gint response_id, gpointer data)
{
  int i;
  if (response_id == GTK_RESPONSE_YES)
  {
    sel_and_conf = TRUE;
    for (i=0; i<qm_coord -> totcoord[2]; i++)
    {
      if (fix_frag[i] && ! old_fixed[i][0] && ! old_fixed[i][1] && ! old_fixed[i][2])
      {
        gchar * str = g_strdup_printf ("Fragment %d has been selected but no coordinates appear to be frozen !\n"
                                       "Unselect fragment %d or select coordinate(s) to freeze !", i+1, i+1);
        show_warning (str, qm_assistant);
        g_free (str);
        sel_and_conf = FALSE;
      }
    }
  }
  destroy_this_dialog (dialog);
}

/*!
  \fn G_MODULE_EXPORT void run_cp2k_fix_molecule (GtkDialog * dial, gint response_id, gpointer data)

  \brief CP2K assistant fixing fragment(s) - running the dialog

  \param dial the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_cp2k_fix_molecule (GtkDialog * dial, gint response_id, gpointer data)
{
  gchar * str;
  int i, j;
  gboolean done = FALSE;
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (a_frag > 0)
      {
        if (a_frag > 1)
        {
          str = g_strdup_printf ("Fragments N°%d", fix_frag[0]+1);
          if (a_frag > 2)
          {
            for (i=1; i<a_frag-1; i++)
            {
              str = g_strdup_printf ("%s, %d", str, fix_frag[i]+1);
            }
          }
          str = g_strdup_printf ("%s and %d have been selected !", str, fix_frag[a_frag-1]+1);
          str = g_strdup_printf ("%s\nConfirm this choice and fix these fragments ?", str);
        }
        else
        {
          str = g_strdup_printf ("Fragment N°%d has been selected !", fix_frag[0]+1);
          str = g_strdup_printf ("%s\nConfirm this choice and fix this fragment ?", str);
        }
        field_question (str, G_CALLBACK(select_fixed_atom_confirm), NULL);
        g_free (str);
        if (sel_and_conf)
        {
          done = TRUE;
          if (tmp_cp2k -> fixlist[1] != NULL)
          {
            g_free (tmp_cp2k -> fixlist[1]);
            tmp_cp2k -> fixlist[1] = NULL;
            if (tmp_cp2k -> fixcoord[1] != NULL)
            {
              g_free (tmp_cp2k -> fixcoord[1]);
              tmp_cp2k -> fixcoord[1] = NULL;
            }
          }
          if (a_frag > 0)
          {
            tmp_cp2k -> fixlist[1] = allocint (a_frag);
            tmp_cp2k -> fixcoord[1] = allocdint (a_frag, 3);
            tmp_cp2k -> fixat[1] = a_frag;
            j = -1;
            for (i=0; i<qm_coord -> totcoord[2]; i++)
            {
              if (fix_frag[i])
              {
                j ++;
                tmp_cp2k -> fixlist[1][j] = i;
                tmp_cp2k -> fixcoord[1][j][0] = old_fixed[i][0];
                tmp_cp2k -> fixcoord[1][j][1] = old_fixed[i][1];
                tmp_cp2k -> fixcoord[1][j][2] = old_fixed[i][2];
              }
            }
          }
        }
      }
      break;
    case GTK_RESPONSE_CLOSE:
      done = TRUE;
      break;
  }
  if (done) destroy_this_dialog (dial);
}

/*!
  \fn void cp2k_fix_molecule ()

  \brief CP2K assistant fixing fragment(s) - creating the dialog
*/
void cp2k_fix_molecule ()
{
  int i, j, k;
  at_col = 0;
  gchar * str = g_strdup_printf ("Please select the fragment(s) to fix");
  GtkWidget * fmol = dialogmodal (str, GTK_WINDOW(qm_assistant));
  g_free (str);
  gtk_dialog_add_button (GTK_DIALOG(fmol), "Apply", GTK_RESPONSE_APPLY);
  GtkWidget * frag_tree =  NULL;
  GtkTreeIter iter;
  GtkTreeViewColumn * frag_col[5];
  GtkCellRenderer * frag_cell[5];
  gchar * frag_title[5] = {"Fragment", "Viz.3D & Select", "x", "y", "z"};
  gchar * ctype[5]={"text", "active", "active", "active", "active"};
  GType col_type[5] = {G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN};
  add_model = gtk_tree_store_newv (5, col_type);
  frag_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(add_model));
  for (i=0; i<5; i++)
  {
    if (i == 0)
    {
      frag_cell[i] = gtk_cell_renderer_text_new ();
    }
    else
    {
      frag_cell[i] = gtk_cell_renderer_toggle_new ();
      if (i == 1)
      {
        g_signal_connect (G_OBJECT(frag_cell[i]), "toggled", G_CALLBACK(select_frag), NULL);
      }
      else
      {
        j = i + 2;
        g_signal_connect (G_OBJECT(frag_cell[i]), "toggled", G_CALLBACK(cp2k_select_coord_id), GINT_TO_POINTER(j));
      }
    }
    frag_col[i] = gtk_tree_view_column_new_with_attributes (frag_title[i], frag_cell[i], ctype[i], i, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(frag_tree), frag_col[i]);
    gtk_tree_view_column_set_alignment (frag_col[i], 0.5);
    if (i == 0)
    {
      gtk_tree_view_column_set_cell_data_func (frag_col[i], frag_cell[i], frag_set_color, NULL, NULL);
    }
    else if (i > 1)
    {
      gtk_tree_view_column_set_cell_data_func (frag_col[i], frag_cell[i], frag_set_visible, NULL, NULL);
    }
  }
  // fill model
  fix_frag = NULL;
  fix_frag = allocint (qm_coord -> totcoord[2]);
  old_fixed = allocdint (qm_coord -> totcoord[2], 3);
  a_frag = 0;
  for (i=0; i<qm_coord -> totcoord[2]; i++)
  {
    gtk_tree_store_append (add_model, & iter, NULL);
    for (j=0; j<tmp_cp2k -> fixat[1]; j++)
    {
      if (tmp_cp2k -> fixlist[1][j] == i)
      {
        fix_frag[i] = 1;
        a_frag ++;
        for (k=0; k<3; k++) old_fixed[i][k] = tmp_cp2k -> fixcoord[1][j][k];
        break;
      }
    }
    gtk_tree_store_set (add_model, & iter, 0, i + 1,
                                           1, fix_frag[i],
                                           2, old_fixed[i][0],
                                           3, old_fixed[i][1],
                                           4, old_fixed[i][2], -1);
  }
  g_object_unref ( add_model);
  gtk_tree_view_expand_all (GTK_TREE_VIEW(frag_tree));

  i = ((qm_coord -> totcoord[2]+1)*35 < 500) ? (qm_coord -> totcoord[2]+1)*35 : 500;
  GtkWidget * scrollsets = create_scroll (dialog_get_content_area (fmol), 220, i, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrollsets, frag_tree);
  run_this_gtk_dialog (fmol, G_CALLBACK(run_cp2k_fix_molecule), NULL);
  proj_unselect_all_atoms ();
  g_free (fix_frag);
}
