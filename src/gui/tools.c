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
* @file tools.c
* @short Callbacks for the toolbox dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'tools.c'
*
* Contains:
*

 - The callbacks for the toolbox dialog

*
* List of functions:

  gchar * prepare_for_title (gchar * init);

  void fill_tool_model ();
  void tool_set_visible (GtkTreeViewColumn * col,
                         GtkCellRenderer * renderer,
                         GtkTreeModel * mod,
                         GtkTreeIter * iter,
                         gpointer data);
  void adjust_tool_model (int calc, int curve, gchar * string_path);

  G_MODULE_EXPORT void toggle_show_hide_curve (GtkCellRendererToggle * cell_renderer,
                                                gchar * string_path, gpointer data);

  GtkWidget * tooltree ();
  GtkWidget * curvetbox ();

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"

extern GtkWidget * create_curve (tint * data);
extern gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif);

GtkTreeStore * tool_model = NULL;
GtkWidget * tool_tree = NULL;
GtkWidget * toolscroll = NULL;

/*!
  \fn gchar * prepare_for_title (gchar * init)

  \brief prepare a string for a window title, getting rid of all markup

  \param init the initial string
*/
gchar * prepare_for_title (gchar * init)
{
  gchar * str = g_strdup_printf ("%s", substitute_string (init, "<sub>", NULL));
  str = g_strdup_printf ("%s", substitute_string (str, "</sub>", NULL));
  str = g_strdup_printf ("%s", substitute_string (str, "<sup>", NULL));
  str = g_strdup_printf ("%s", substitute_string (str, "</sup>", NULL));
  str = g_strdup_printf ("%s", substitute_string (str, "<i>", NULL));
  str = g_strdup_printf ("%s", substitute_string (str, "</i>", NULL));
  return str;
}

/*!
  \fn void fill_tool_model ()

  \brief fill the tool window tree model
*/
void fill_tool_model ()
{
  GtkTreeIter calc_level, curve_level;
  int i, j;
  gboolean status, append;
  gchar * str;
  GtkImage * img;

  if (active_project)
  {
    i = (active_project -> steps > 1) ? 25 : 0;
    gtk_window_set_resizable (GTK_WINDOW (curvetoolbox), TRUE);
#ifdef GTK4
    gtk_widget_set_size_request (curvetoolbox, 300, 210+i);
#else
    gtk_widget_set_size_request (curvetoolbox, 300, 240+i);
#endif
    gtk_window_set_resizable (GTK_WINDOW (curvetoolbox), FALSE);
  }
  gtk_tree_store_clear (tool_model);
  for (i=0; i<NGRAPHS; i++)
  {
    if (i != MS)
    {
      append = TRUE;
    }
    else
    {
      if (active_project)
      {
        append = (active_project -> steps > 1) ? TRUE : FALSE;
      }
      else
      {
        append = FALSE;
      }
    }
    if (append)
    {
      gtk_tree_store_append (tool_model, & calc_level, NULL);
      img = GTK_IMAGE(gtk_image_new_from_file(graph_img[i]));
#ifdef GTK4
      gtk_tree_store_set (tool_model, & calc_level, 0, -1, 1, -1, 2, img, 3, graph_name[i], -1);
#else
      GdkPixbuf * pix =  gtk_image_get_pixbuf(img);
      gtk_tree_store_set (tool_model, & calc_level, 0, -1, 1, -1, 2, pix, 3, graph_name[i], -1);
#endif

      gtk_image_clear (img);
      if (active_project)
      {
        if (active_project -> numc[i] > 0 && active_project -> visok[i])
        {
          for (j=0; j<active_project -> numc[i]; j++)
          {
            if (active_project -> curves[i][j] -> name && active_project -> curves[i][j] -> ndata)
            {
              gtk_tree_store_append (tool_model, & curve_level, & calc_level);
              status = FALSE;
              str = g_strdup_printf ("%s", active_project -> curves[i][j] -> name);
              if (active_project -> curves[i][j] -> window != NULL)
              {
                if (GTK_IS_WIDGET(active_project -> curves[i][j] -> window))
                {
                  if (gtk_widget_get_visible(active_project -> curves[i][j] -> window)) status = TRUE;
                }
              }
              gtk_tree_store_set (tool_model, & curve_level, 0, i, 1, j, 3, str, 4, status, -1);
              g_free (str);
            }
          }
        }
      }
    }
  }
}

/* void tool_set_visible (GtkTreeViewColumn * col,
                          GtkCellRenderer * renderer,
                          GtkTreeModel * mod,
                          GtkTreeIter * iter,
                          gpointer data)

  \brief show/hide and sensitive/not a GtkCellRenderer

  \param col the column
  \param renderer the cell renderer
  \param mod the model
  \param iter the iter
  \param data the associated data pointer
*/
void tool_set_visible (GtkTreeViewColumn * col,
                       GtkCellRenderer * renderer,
                       GtkTreeModel * mod,
                       GtkTreeIter * iter,
                       gpointer data)
{
  int i, j, k;
  i = GPOINTER_TO_INT(data);
  gtk_tree_model_get (mod, iter, 0, & j, -1);
  gboolean vis = ((j < 0 && i == 2) || (j > -1 && i == 0)) ? FALSE : TRUE;
  gtk_cell_renderer_set_visible (renderer, vis);
  if (! active_project)
  {
    gtk_cell_renderer_set_sensitive (renderer, FALSE);
  }
  else if (j > -1 && active_project -> numwid > 0)
  {
    if (active_project -> numc[j])
    {
      gtk_tree_model_get (mod, iter, 1, & k, -1);
      if (active_project) gtk_cell_renderer_set_sensitive (renderer, active_project -> curves[j][k] -> ndata);
    }
  }
  else
  {
    gtk_cell_renderer_set_sensitive (renderer, TRUE);
  }
}

/*!
  \fn void adjust_tool_model (int calc, int curve, gchar * string_path)

  \brief adjust the content of the tool box tree model

  \param calc the calculation
  \param curve the curve
  \param string_path the path in the tree view
*/
void adjust_tool_model (int calc, int curve, gchar * string_path)
{
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(tool_model), & iter, path);
  if (calc == SP || calc == MS)
  {
    int i, j;
    gtk_tree_model_get (GTK_TREE_MODEL(tool_model), & iter, 0, & i, -1);
    gtk_tree_model_get (GTK_TREE_MODEL(tool_model), & iter, 1, & j, -1);
    if (i == calc && j == curve) gtk_tree_store_set (tool_model, & iter, 4, 0, -1);
  }
  else
  {
    gtk_tree_store_set (tool_model, & iter, 4, 0, -1);
  }
}

/*!
  \fn G_MODULE_EXPORT void toggle_show_hide_curve (GtkCellRendererToggle * cell_renderer,
                                                gchar * string_path, gpointer data)

  \brief To show/hide a curve by clicking in the tree view

  \param cell_renderer the renderer toggled
  \param string_path the path in the tree view
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_show_hide_curve (GtkCellRendererToggle * cell_renderer,
                                             gchar * string_path, gpointer data)
{
  int i, j, k;
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(tool_model), & iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(tool_model), & iter, 0, & i, -1);
  gtk_tree_model_get (GTK_TREE_MODEL(tool_model), & iter, 1, & j, -1);
  gtk_tree_model_get (GTK_TREE_MODEL(tool_model), & iter, 4, & k, -1);
#ifdef DEBUG
  // g_debug ("Show curve:: i= %d, j= %d, k= %d", i, j, k);
#endif // DEBUG
  if (! k)
  {
    if (active_project -> curves[i][j] -> window == NULL)
    {
      active_project -> curves[i][j] -> window = create_curve (& active_project -> idcc[i][j]);
      active_project -> curves[i][j] -> path = g_strdup_printf ("%s", string_path);
    }
    show_the_widgets (active_project -> curves[i][j] -> window);
  }
  else
  {
    if (active_project -> curves[i][j] -> window != NULL)
    {
      hide_the_widgets (active_project -> curves[i][j] -> window);
    }
  }
  gtk_tree_store_set (tool_model, & iter, 4, ! k, -1);
}

/*!
  \fn GtkWidget * tooltree ()

  \brief create the toolbox tree view
*/
GtkWidget * tooltree ()
{
  GtkTreeViewColumn * tool_col[3];
  GtkCellRenderer * tool_cell[3];
  gchar * ctitle[3]={"Logo", "Name", "Button"};
  gchar * ctype[3]={"pixbuf", "text", "active"};
  GType coltype[5]= {G_TYPE_INT, G_TYPE_INT, G_TYPE_OBJECT, G_TYPE_STRING, G_TYPE_BOOLEAN};
  tool_model = gtk_tree_store_newv (5, coltype);
  int i;
  fill_tool_model ();
  tool_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tool_model));
  for (i=0; i<3; i++)
  {
    switch (i)
    {
      case 0:
        tool_cell[i] = gtk_cell_renderer_pixbuf_new ();
        break;
      case 1:
        tool_cell[i] = gtk_cell_renderer_text_new ();
        break;
      case 2:
        tool_cell[i] = gtk_cell_renderer_toggle_new ();
        g_signal_connect (G_OBJECT(tool_cell[i]), "toggled", G_CALLBACK(toggle_show_hide_curve), NULL);
        break;
    }
    tool_col[i] = gtk_tree_view_column_new_with_attributes (ctitle[i], tool_cell[i], ctype[i], i+2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tool_tree), tool_col[i]);
    gtk_tree_view_column_set_alignment (tool_col[i], 0.5);
    gtk_tree_view_column_set_cell_data_func (tool_col[i], tool_cell[i], tool_set_visible, GINT_TO_POINTER(i), NULL);
  }
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(tool_tree), FALSE);
  return tool_tree;
}

/*!
  \fn GtkWidget * curvetbox ()

  \brief create the curve tool box window
*/
GtkWidget * curvetbox ()
{
  GtkWidget * ctbox;
  ctbox = create_win ("Toolboxes", MainWindow, FALSE, FALSE);
#ifdef GTK4
  gtk_widget_set_size_request (ctbox, 300, 210);
#else
  gtk_widget_set_size_request (ctbox, 300, 240);
#endif
  graph_img[0] = g_strdup_printf ("%s", PACKAGE_GR);
  graph_img[1] = g_strdup_printf ("%s", PACKAGE_SQ);
  graph_img[2] = g_strdup_printf ("%s", PACKAGE_SQ);
  graph_img[3] = g_strdup_printf ("%s", PACKAGE_GR);
  graph_img[4] = g_strdup_printf ("%s", PACKAGE_BD);
  graph_img[5] = g_strdup_printf ("%s", PACKAGE_AN);
  graph_img[6] = g_strdup_printf ("%s", PACKAGE_RI);
  graph_img[7] = g_strdup_printf ("%s", PACKAGE_CH);
  graph_img[8] = g_strdup_printf ("%s", PACKAGE_SP);
  graph_img[9] = g_strdup_printf ("%s", PACKAGE_MS);
  // New calculation icon to be added here

  toolscroll = create_scroll (NULL, -1, -1, GTK_SHADOW_NONE);
  add_container_child (CONTAINER_SCR, toolscroll, tooltree ());
  add_container_child (CONTAINER_WIN, ctbox, toolscroll);
  show_the_widgets (toolscroll);
  add_gtk_close_event (ctbox, G_CALLBACK(hide_this_window), NULL);
  return (ctbox);
}
