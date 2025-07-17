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
* @file glwindow.c
* @short Functions to create a project OpenGL window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'glwindow.c'
*
* Contains:
*

 - The functions to create a project OpenGL window

*
* List of functions:

  gboolean create_3d_model (int p, gboolean load);

  G_MODULE_EXPORT gboolean on_key_pressed (GtkWidget * widg, GdkEventKey * event, gpointer data);
  G_MODULE_EXPORT gboolean on_glwin_key_pressed (GtkEventControllerKey * self, guint keyval, guint keycode, GdkModifierType state, gpointer data);

  void update_all_menus (glwin * view, int nats);
  void menu_items_opengl (GtkWidget * menu, glwin * view, int pop);
  void menu_items_model (GtkWidget * menu, glwin * view, int pop);
  void menu_items_view (GtkWidget * menu, glwin * view, int popm);
  void prepare_opengl_menu_bar (glwin * view);
  void change_color_map (glwin * view, int col);
  void set_motion (glwin * view, int axis, int da, int db, gboolean UpDown, GdkModifierType state);
  void activate_glwin_action (gchar * action_string, gchar * action_name, glwin * view);
  void glwin_key_pressed (guint keyval, GdkModifierType state, gpointer data);
  void prep_model (int p);

  G_MODULE_EXPORT void render_gl_image (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void on_win_realize (GtkWidget * widg, gpointer data);

  GtkWidget * prep_rings_menu (glwin * view, int id);
  GtkWidget * coord_menu (glwin * view);
  GtkWidget * menu_opengl (glwin * view, int pop);
  GtkWidget * menu_model (glwin * view, int pop);
  GtkWidget * menu_view (glwin * view, int popm);

  mat4_t insert_projection (glwin * view);

  vec3_t get_insertion_coordinates (glwin * view);

*/

#include "global.h"
#include "project.h"
#include "calc.h"
#include "glview.h"
#include "initcoord.h"
#include "submenus.h"
#include "color_box.h"

extern G_MODULE_EXPORT void opengl_advanced (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void coord_properties (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void set_style (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void set_render (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void set_mode (GtkWidget * widg, gpointer data);
extern void set_sensitive_coord_menu (glwin * view, gboolean status);
extern void set_color_map_sensitive (glwin * view);
extern G_MODULE_EXPORT void set_selection_mode (GtkWidget * widg, gpointer data);

extern gboolean spin (gpointer data);
extern G_MODULE_EXPORT void spin_stop (GtkButton * but, gpointer data);
extern G_MODULE_EXPORT void spin_go (GtkWidget * widg, gpointer data);
extern void update_menus (glwin * view);
extern G_MODULE_EXPORT void set_box_axis_style (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void window_measures (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void window_recorder (GtkWidget * widg, gpointer data);
extern void window_encode (glwin * view, gboolean video);
extern GtkWidget * menupoly (glwin * view, int jd, int id, gchar * poln);
extern G_MODULE_EXPORT void set_color_map (GtkWidget * widg, gpointer data);
#ifdef GTK4
extern void update_menu_bar (glwin * view);
extern G_MODULE_EXPORT void set_full_screen (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void add_object (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void label_unlabel_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void select_unselect_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void edit_in_new_project (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void remove_the_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void copy_the_atoms (GSimpleAction * action, GVariant * parameter, gpointer data);
#else
extern void prep_all_coord_menus (glwin * view);
extern G_MODULE_EXPORT void set_full_screen (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void to_reset_view (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void add_object (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void label_unlabel_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void select_unselect_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void remove_the_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void copy_the_atoms (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void edit_in_new_project (GtkWidget * widg, gpointer data);
#endif
extern void save_rotation_quaternion (glwin * view);
extern void rotate_x_y (glwin * view, double angle_x, double angle_y);
extern void translate (project * this_proj, int status, int axis, vec3_t trans);
extern int selected_aspec;
extern int is_selected;
extern int is_labelled;
extern G_MODULE_EXPORT void on_create_new_project (GtkWidget * widg, gpointer data);
extern gchar * action_atoms[3];
extern int get_selection_type (glwin * view);
extern GtkWidget * shortcuts_window (int sections, int group_by_section[sections], int groups, int shortcut_by_group[groups],
                                     gchar * section_names[sections], gchar * group_names[groups], shortcuts shortcs[]);

atomic_object * copied_object = NULL;

gchar * opengl_section_names[]={ "General", "Analysis mode", "Edition mode" };
int opengl_group_by_section[] = { 5, 4, 3 };
gchar * opengl_group_names[]={"Styles", "Measures", "Selection mode", "Window management", "Mouse mode",
                              "Camera rotation", "Camera motion", "Camera spin", "Model interaction",
                              "Coordinates rotation", "Coordinates translation", "Model interaction"};
int opengl_shortcut_by_group[] = { 10, 1, 4, 6, 2, // Global
                                    4, 6, 6, 3,    // Analysis
                                    4, 6, 6 };     // Edition

shortcuts opengl_shortcuts[] = {
//
// Global
//
  { "Atom(s) color map", "change atom(s) color map", GDK_KEY_a, "a" },
  { "Polyhedra color map", "change polyhedra color map", GDK_KEY_p, "p" },
  { "Ball and stick", "change global style to ball and stick", GDK_KEY_b, "b" },
  { "Cylinders", "change global style to cylinders", GDK_KEY_w, "w" },
  { "Spheres", "change global style to spheres", GDK_KEY_s, "s" },
  { "Covalent radius", "change global style to cylinders", GDK_KEY_o, "o" },
  { "Ionic radius", "change global style to cylinders", GDK_KEY_i, "i" },
  { "Van der Waals radius", "change global style to van der Waals radius", GDK_KEY_v, "v" },
  { "In crystal radius", "change global style to in crystal radius", GDK_KEY_r, "r" },
  { "Wireframe", "change global style to wireframe", GDK_KEY_w, "w" },
//
  { "All measures for the selection, pressed:\n\n"
    "\t- 1st time: show inter-atomic distance(s)\n"
    "\t- 2nd time: show inter-atomic angles\n"
    "\t- 3rd time: hide measures", "Show measures", GDK_KEY_m, "m" },
//
  { "Atom", "atom selection mode", GDK_KEY_a, "<Shift>a" },
  { "Coordination", "coordination sphere selection mode", GDK_KEY_c, "<Shift>c" },
  { "Fragment", "fragment selection mode", GDK_KEY_f, "<Shift>f" },
  { "Molecule", "molecule selection mode", GDK_KEY_m, "<Shift>m" },
//
  { "Environments", "Environments window",  GDK_KEY_e, "<Ctrl>e" },
  { "Measures", "Measures window",  GDK_KEY_m, "<Ctrl>m" },
  { "Recorder", "Recorder window",  GDK_KEY_r, "<Ctrl>r" },
  { "Create new project", "Create new project",  GDK_KEY_n, "<Ctrl>n" },
  { "Enter fullscreen", "Enter fullscreen", GDK_KEY_f, "<Ctrl>f" },
  { "Exit fullscreen", "Exit fullscreen", GDK_KEY_Escape, "Escape" },
//
#ifdef OSX
  { "Analysis mode", "Analysis mode", GDK_KEY_a, "<Meta>a" },
  { "Edition mode", "Edition mode", GDK_KEY_e, "<Meta>e" },
#else
  { "Analysis mode", "Analysis mode", GDK_KEY_a, "<Alt>a" },
  { "Edition mode", "Edition mode", GDK_KEY_e, "<Alt>e" },
#endif
//
// Analysis
//
  { "Rotate right", "rotate right", GDK_KEY_Right, "Right" },
  { "Rotate left", "rotate left", GDK_KEY_Left, "Left" },
  { "Rotate up", "rotate up", GDK_KEY_Up, "Up" },
  { "Rotate down", "rotate down", GDK_KEY_Down, "Down" },
//
  { "Move right", "move camera right", GDK_KEY_Right, "<Ctrl>Right" },
  { "Move left", "move camera left", GDK_KEY_Left, "<Ctrl>Left" },
  { "Move up", "move camera up", GDK_KEY_Up, "<Ctrl>Up" },
  { "Move down", "move camera down", GDK_KEY_Down, "<Ctrl>Down" },
  { "Zoom in", "zoom camera in", GDK_KEY_Down, "<Shift>Up" },
  { "Zoom out", "zoom camera out", GDK_KEY_Down, "<Shift>Down" },
//
  { "Spin right", "rotate right", GDK_KEY_Right, "<Ctrl><Shift>Right" },
  { "Spin left", "rotate left", GDK_KEY_Left, "<Ctrl><Shift>Left" },
  { "Spin up", "rotate up", GDK_KEY_Up, "<Ctrl><Shift>Up" },
  { "Spin down", "rotate down", GDK_KEY_Down, "<Ctrl><Shift>Down" },
  { "Pause / restart", "pause / restart spinning", GDK_KEY_space, "space" },
  { "Stop", "stop spinning",  GDK_KEY_s, "<Ctrl>s" },
//
  { "Label / unlabel all atom(s)", "Label / unlabel all atom(s)",  GDK_KEY_l, "<Ctrl>l" },
  { "Select / unselect all atom(s)", "Select / unselect all atom(s)",  GDK_KEY_a, "<Ctrl>a" },
  { "Copy selection", "Copy selection",  GDK_KEY_c, "<Ctrl>c" },
//
// Edition
//
  { "Rotate right", "rotate right", GDK_KEY_Right, "Right" },
  { "Rotate left", "rotate left", GDK_KEY_Left, "Left" },
  { "Rotate up", "rotate up", GDK_KEY_Up, "Up" },
  { "Rotate down", "rotate down", GDK_KEY_Down, "Down" },
//
  { "Translate right", "move camera right", GDK_KEY_Right, "<Ctrl>Right" },
  { "Translate left", "move camera left", GDK_KEY_Left, "<Ctrl>Left" },
  { "Translate up", "move camera up", GDK_KEY_Up, "<Ctrl>Up" },
  { "Translate down", "move camera down", GDK_KEY_Down, "<Ctrl>Down" },
  { "Zoom in", "zoom camera in", GDK_KEY_Up, "<Shift>Up" },
  { "Zoom out", "zoom camera out", GDK_KEY_Down, "<Shift>Down" },
//
  { "Label / unlabel all atom(s)", "Label / unlabel all atom(s)",  GDK_KEY_l, "<Ctrl>l" },
  { "Select / unselect all atom(s)", "Select / unselect all atom(s)",  GDK_KEY_a, "<Ctrl>a" },
  { "Copy all selection", "Copy selection",  GDK_KEY_c, "<Ctrl>c" },
  { "Cut selection", "Cut selection",  GDK_KEY_x, "<Ctrl>x" },
  { "Paste selection", "Paste selection",  GDK_KEY_v, "<Ctrl>v" },
  { "Delete selection", "Delete selection", GDK_KEY_Delete, "Delete" }
};

#ifdef GTK3
/*!
  \fn GtkWidget * prep_rings_menu (glwin * view, int id)

  \brief create the 'Rings' submenu GTK3

  \param view the target glwin
  \param id atoms in ring(s) (0) or polyhedra from rings (1)
*/
GtkWidget * prep_rings_menu (glwin * view, int id)
{
  if (id == 0)
  {
    return menu_rings (view, 0);
  }
  else
  {
    return menupoly (view, 0, 2, NULL);
  }
}

/*!
  \fn GtkWidget * coord_menu (glwin * view)

  \brief create the 'Coordination' submenu GTK3

  \param view the target glwin
*/
GtkWidget * coord_menu (glwin * view)
{
  int i, j, k;
  GtkWidget * widg;
  for (j=4; j<10; j++)
  {
    if (j<9)
    {
      if (view -> ogl_poly[0][j] != NULL)
      {
        for (k=0; k<get_project_by_id(view -> proj) -> coord -> totcoord[j]; k++)
        {
          if (view -> ogl_poly[0][j][k] != NULL)
          {
            if (GTK_IS_WIDGET(view -> ogl_poly[0][j][k]))
            {
              widg = gtk_widget_get_parent (view -> ogl_poly[0][j][k]);
              if (GTK_IS_WIDGET(widg))
              {
                g_object_ref (view -> ogl_poly[0][j][k]);
                gtk_container_remove (GTK_CONTAINER(widg), view -> ogl_poly[0][j][k]);
              }
            }
          }
        }
      }
    }
    if (view -> ogl_geom[0][j] != NULL)
    {
      for (k=0; k<get_project_by_id(view -> proj) -> coord -> totcoord[j]; k++)
      {
        if (view -> ogl_geom[0][j][k] != NULL)
        {
          if (GTK_IS_WIDGET(view -> ogl_geom[0][j][k]))
          {
            widg =  gtk_widget_get_parent (view -> ogl_geom[0][j][k]);
            if (GTK_IS_WIDGET(widg))
            {
              g_object_ref (view -> ogl_geom[0][j][k]);
              gtk_container_remove (GTK_CONTAINER(widg), view -> ogl_geom[0][j][k]);
            }
          }
        }
      }
    }
  }
  if (view -> ogl_chains[0]) view -> ogl_chains[0] = destroy_this_widget (view -> ogl_chains[0]);
  for (i=0; i<2; i++)
  {
    for (j=2; j<4; j++)
    {
      detach_frag_mol_menu (view, i, j);
    }
  }
  for (i=1; i<OGL_COORDS; i++)
  {
    view -> ogl_coord[i] = destroy_this_widget (view -> ogl_coord[i]);
  }
  for (i=0; i<2; i++)
  {
    view -> ogl_rings[i*6] = menu_item_new_with_submenu ("Ring(s)", view -> rings, prep_rings_menu (view, i*6));
  }
  view -> ogl_chains[0] = menu_item_new_with_submenu ("Chain(s)", view -> chains, add_menu_coord (view, 0, 9));
  view -> ogl_coord[1] = menu_coord (view, 0);
  view -> ogl_coord[2] = menu_item_new_with_submenu ("Polyhedra", TRUE, menu_poly (view, 0));
  view -> ogl_coord[3] = menu_item_new_with_submenu ("Fragment(s)", get_project_by_id(view -> proj) -> coord -> totcoord[2], add_menu_coord (view, 0, 2));
  view -> ogl_coord[4] = menu_item_new_with_submenu ("Molecule(s)", get_project_by_id(view -> proj) -> coord -> totcoord[3], add_menu_coord (view, 0, 3));
  GtkWidget * menu = gtk_menu_new ();
  gtk_menu_shell_append ((GtkMenuShell *)menu, view -> ogl_coord[1]);
  gtk_menu_shell_append ((GtkMenuShell *)menu, view -> ogl_coord[2]);
  gtk_menu_shell_append ((GtkMenuShell *)menu, view -> ogl_rings[0]);
  gtk_menu_shell_append ((GtkMenuShell *)menu, view -> ogl_chains[0]);
  gtk_menu_shell_append ((GtkMenuShell *)menu, view -> ogl_coord[3]);
  gtk_menu_shell_append ((GtkMenuShell *)menu, view -> ogl_coord[4]);
  gtk3_menu_item (menu, "Advanced", IMG_NONE, NULL, G_CALLBACK(coord_properties), (gpointer)& view -> colorp[30][0], TRUE, GDK_KEY_e, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
  // add_advanced_item (menu, G_CALLBACK(coord_properties), (gpointer)& view -> colorp[30][0], TRUE, GDK_KEY_e, GDK_CONTROL_MASK);
  return menu;
}
#endif

/*!
  \fn void update_all_menus (glwin * view, int nats)

  \brief update all menus of the OpenGL window

  \param view the target glwin
  \param nats the total number of atoms
*/
void update_all_menus (glwin * view, int nats)
{
#ifdef GTK3
  int i, j, k, l;
  i = view -> anim -> last -> img -> style;
  j = (nats <= 1000) ? BALL_AND_STICK : DEFAULT_STYLE;
  if (i != j)
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_styles[j], FALSE);
    if (i != SPACEFILL)
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_styles[i], TRUE);
      set_style (view -> ogl_styles[i], & view -> colorp[i][0]);
    }
    else
    {
      i = view -> anim -> last -> img -> filled_type;
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> filled_styles[i], TRUE);
      set_style (view -> filled_styles[i], & view -> colorp[OGL_STYLES+i][0]);
    }
  }
  i = view -> anim -> last -> img -> render;
  j = FILL;
  if (i != j)
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_render[j], FALSE);
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_render[i], TRUE);
    set_render (view -> ogl_render[i], & view -> colorp[i][0]);
  }

  update_menus (view);
  widget_set_sensitive (view -> ogl_box[0], active_cell -> ltype);
  if (view -> anim -> last -> img -> abc -> box == NONE)
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[0][0], FALSE);
    set_box_axis_style (view -> ogl_box_axis[0][0], & view -> colorp[0][0]);
  }
  else
  {
    j = (view -> anim -> last -> img -> abc -> box == WIREFRAME) ? 1 : 2;
    k = j*j;
    l = (view -> anim -> last -> img -> abc -> box == WIREFRAME) ? CYLINDERS : WIREFRAME;
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[0][0], TRUE);
    set_box_axis_style (view -> ogl_box_axis[0][0], & view -> colorp[0][0]);
    view -> anim -> last -> img -> abc -> box = l;
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[i][j], TRUE);
    set_box_axis_style (view -> ogl_box_axis[0][j], & view -> colorp[k][0]);
  }

  if (view -> anim -> last -> img -> xyz -> axis == NONE)
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][0], FALSE);
    set_box_axis_style (view -> ogl_box_axis[i][0], & view -> colorp[0][1]);
  }
  else
  {
    j = (view -> anim -> last -> img -> xyz -> axis == WIREFRAME) ? 1 : 2;
    k = j*j;
    l = (view -> anim -> last -> img -> xyz -> axis == WIREFRAME) ? CYLINDERS : WIREFRAME;
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][0], TRUE);
    set_box_axis_style (view -> ogl_box_axis[i][0], & view -> colorp[0][1]);
    view -> anim -> last -> img -> xyz -> axis = l;
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][j], TRUE);
    set_box_axis_style (view -> ogl_box_axis[1][j], & view -> colorp[k][1]);
  }

  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_rep[view -> anim -> last -> img -> rep], TRUE);
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_rep[! view -> anim -> last -> img -> rep], FALSE);
  for (i=0; i<5; i++) gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][8+i], FALSE);
  if (view -> anim -> last -> img -> xyz -> t_pos != CUSTOM)
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][8+view -> anim -> last -> img -> xyz -> t_pos], TRUE);
  }
  set_advanced_bonding_menus (view);
  widget_set_sensitive (view -> ogl_clones[0], view -> allbonds[1]);
  update_rings_menus (view);
  update_chains_menus (view);
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_clones[0], view -> anim -> last -> img -> draw_clones);
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_clones[5], view -> anim -> last -> img -> cloned_poly);
  int * cmap = save_color_map (view);
  set_color_map_sensitive (view);
  if (view -> color_styles[0])
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> color_styles[0], TRUE);
    set_color_map (view -> color_styles[0], & view  -> colorp[0][0]);
  }
  if (view -> color_styles[ATOM_MAPS])
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> color_styles[ATOM_MAPS], TRUE);
    set_color_map (view -> color_styles[ATOM_MAPS], & view  -> colorp[ATOM_MAPS][0]);
  }
  restore_color_map (view, cmap);
  g_free (cmap);
  gtk_range_set_value (GTK_RANGE(view -> ogl_quality), view -> anim -> last -> img -> quality);
#else
  update_menu_bar (view);
#endif
}

/*!
  \fn G_MODULE_EXPORT void render_gl_image (GtkWidget * widg, gpointer data)

  \brief render image from the OpenGL window

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void render_gl_image (GtkWidget * widg, gpointer data)
{
  glwin * view = (glwin *) data;
  window_encode (view, FALSE);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void view_shortcuts (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief view OpenGL window shortcuts callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void view_shortcuts (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void view_shortcuts (GtkWidget * widg, gpointer data)

  \brief OpenGL window shortcuts callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void view_shortcuts (GtkWidget * widg, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  view -> shortcuts = destroy_this_widget (view -> shortcuts);
  view -> shortcuts = shortcuts_window (G_N_ELEMENTS(opengl_group_by_section), opengl_group_by_section, G_N_ELEMENTS(opengl_shortcut_by_group),
                                        opengl_shortcut_by_group, opengl_section_names, opengl_group_names, opengl_shortcuts);
}

#ifdef GTK3

/*!
  \fn void menu_items_opengl (GtkWidget * menu, glwin * view, int pop)

  \brief create the 'OpenGL' submenu items GTK3

  \param menu the GtkWidget sending the signal
  \param view the target glwin
  \param pop main app (0) or popup (1)
*/
void menu_items_opengl (GtkWidget * menu, glwin * view, int pop)
{
  GtkWidget * style = gtk3_menu_item (menu, "Style", IMG_FILE, (gpointer)PACKAGE_MOL, NULL, NULL, FALSE, 0, 0, FALSE, FALSE, get_project_by_id(view -> proj) -> nspec);
  gtk_menu_item_set_submenu ((GtkMenuItem *)style, menu_style(view, pop));
  gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Color Scheme(s)", get_project_by_id(view -> proj) -> nspec, menu_map(view, pop)));
  gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Render", get_project_by_id(view -> proj) -> nspec, menu_render(view, pop)));
  gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Quality", get_project_by_id(view -> proj) -> nspec, menu_quality(view, pop)));
  gtk3_menu_item (menu, "Material And Light(s)", IMG_NONE, NULL, G_CALLBACK(opengl_advanced), (gpointer)view, FALSE, 0, 0, FALSE, FALSE, FALSE);
  gtk3_menu_item (menu, "Render Image", IMG_FILE, (gpointer)PACKAGE_IMG, G_CALLBACK(render_gl_image), (gpointer)view, FALSE, 0, 0, FALSE, FALSE, FALSE);
}

/*!
  \fn GtkWidget * menu_opengl (glwin * view, int pop)

  \brief create the 'OpenGL' submenu GTK3

  \param view the target glwin
  \param pop main app (0) or popup (1)
*/
GtkWidget * menu_opengl (glwin * view, int pop)
{
  GtkWidget * menu = gtk_menu_new ();
  menu_items_opengl (menu, view, pop);
  return menu;
}

/*!
  \fn void menu_items_model (GtkWidget * menu, glwin * view, int pop)

  \brief create the 'Model' submenu items GTK3

  \param menu the GtkWidget sending the signal
  \param view the target glwin
  \param pop main app (0) or popup (1)
*/
void menu_items_model (GtkWidget * menu, glwin * view, int pop)
{
  if (get_project_by_id(view -> proj) -> nspec)
  {
    gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Atom(s)", TRUE, menu_atoms (view, pop, 0)));
    gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Bond(s)", TRUE, menu_bonds (view, pop, 0)));
    gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Clone(s)", TRUE, menu_clones (view, pop)));
    gtk_menu_shell_append ((GtkMenuShell *)menu, menu_box_axis (view, 0, 0));
  }
}

/*!
  \fn GtkWidget * menu_model (glwin * view, int pop)

  \brief create the 'Model' submenu GTK3

  \param view the target glwin
  \param pop main app (0) or popup (1)
*/
GtkWidget * menu_model (glwin * view, int pop)
{
  GtkWidget * menu = gtk_menu_new ();
  menu_items_model (menu, view, pop);
  return menu;
}

/*!
  \fn void menu_items_view (GtkWidget * menu, glwin * view, int popm)

  \brief create the 'View' menu items GTK3

  \param menu the GtkWidget sending the signal
  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
void menu_items_view (GtkWidget * menu, glwin * view, int popm)
{
  gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Representation", TRUE, menu_rep (view, popm)));
  gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Projection", TRUE, menu_proj (view)));
  gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Background", TRUE, menu_back (view)));
  if (get_project_by_id(view -> proj) -> nspec) gtk_menu_shell_append ((GtkMenuShell *)menu, menu_box_axis (view, popm, 1));
  if (! popm)
  {
    add_menu_separator (menu);
    gtk3_menu_item (menu, "Reset view", IMG_STOCK, (gpointer)FITBEST, G_CALLBACK(to_reset_view), (gpointer)view, FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk3_menu_item (menu, "Center molecule", IMG_STOCK, (gpointer)FITBEST, G_CALLBACK(to_center_this_molecule), (gpointer)view, FALSE, 0, 0, FALSE, FALSE, FALSE);
    add_menu_separator (menu);
    gtk3_menu_item (menu, "Fullscreen", IMG_STOCK, (gpointer)FULLSCREEN, G_CALLBACK(set_full_screen), (gpointer)view, TRUE, GDK_KEY_f, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
  }
}

/*!
  \fn GtkWidget * menu_view (glwin * view, int popm)

  \brief create the 'View' submenu GTK3

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GtkWidget * menu_view (glwin * view, int popm)
{
  GtkWidget * menu = gtk_menu_new ();
  menu_items_view (menu, view, popm);
  return menu;
}

/*!
  \fn GtkWidget * menu_help (glwin * view, int popm)

  \brief create the 'Help' submenu GTK3

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GtkWidget * menu_help (glwin * view, int popm)
{
  GtkWidget * menu;
  menu = gtk_menu_new ();
  gtk3_menu_item (menu, "Shortcuts", IMG_NONE, NULL, G_CALLBACK(view_shortcuts), (gpointer)view, FALSE, 0, 0, FALSE, FALSE, FALSE);
  return menu;
}
#endif

/*!
  \fn void prepare_opengl_menu_bar (glwin * view)

  \brief update the OpenGL window menu bar

  \param view the target glwin
*/
void prepare_opengl_menu_bar (glwin * view)
{
#ifdef GTK3
  view -> ogl_coord[0] = destroy_this_widget (view -> ogl_coord[0]);
#endif
  view -> menu_bar = destroy_this_widget (view -> menu_bar);
#ifdef GTK3
  view -> menu_bar = gtk_menu_bar_new ();
  gtk_menu_shell_append ((GtkMenuShell *)view -> menu_bar, menu_item_new_with_submenu ("OpenGL", TRUE, menu_opengl(view, 0)));
  project * this_proj = get_project_by_id (view -> proj);
  gtk_menu_shell_append ((GtkMenuShell *)view -> menu_bar, menu_item_new_with_submenu ("Model", this_proj -> nspec, menu_model(view, 0)));
  view -> ogl_coord[0] = create_menu_item (FALSE, "Chemistry");
  gtk_menu_shell_append ((GtkMenuShell *)view -> menu_bar, view -> ogl_coord[0]);
  widget_set_sensitive (view -> ogl_coord[0], this_proj -> nspec);
  if (this_proj -> nspec)
  {
    gtk_menu_item_set_submenu ((GtkMenuItem *)view -> ogl_coord[0], coord_menu (view));
  }
  gtk_menu_shell_append ((GtkMenuShell *)view -> menu_bar, menu_item_new_with_submenu ("Tools", TRUE, menu_tools(view, 0)));
  gtk_menu_shell_append ((GtkMenuShell *)view -> menu_bar, menu_item_new_with_submenu ("View", TRUE, menu_view(view, 0)));
  gtk_menu_shell_append ((GtkMenuShell *)view -> menu_bar, menu_anim (view, 0));

  show_the_widgets (view -> menu_bar);

  if (this_proj -> nspec) update_all_menus (view, this_proj -> natomes);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, view -> menu_box, view -> menu_bar, TRUE, TRUE, 0);
  /* GtkWidget * menu = gtk_menu_bar_new ();
  gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Help", TRUE, menu_help(view, 0)));
  add_box_child_end (view -> menu_box, menu, FALSE, FALSE, 0);
  show_the_widgets (menu);*/
  show_the_widgets (view -> menu_bar);
#else
  update_menu_bar (view);
#endif
}

/*!
  \fn void change_color_map (glwin * view, int col)

  \brief change atom / polyhedra color map

  \param view the target glwin
  \param col the color id
*/
void change_color_map (glwin * view, int col)
{
  int i, j;
  i = ATOM_MAPS-1;
  if (view -> custom_map) i++;
  if (view -> anim -> last -> img -> color_map[col] < i-1)
  {
    if (view -> anim -> last -> img -> color_map[col] < 2)
    {
      j = col*ATOM_MAPS + view -> anim -> last -> img -> color_map[col] + 1;
    }
    else if (view -> anim -> last -> img -> color_map[col] == 2 && view -> adv_bonding[0])
    {
      j = col*ATOM_MAPS + view -> anim -> last -> img -> color_map[col] + 1;
    }
    else if (view -> anim -> last -> img -> color_map[col] == 3 && view -> adv_bonding[1])
    {
      j = col*ATOM_MAPS + view -> anim -> last -> img -> color_map[col] + 1;
    }
    else if (view -> anim -> last -> img -> color_map[col] == 4 && get_project_by_id(view -> proj) -> force_field[0])
    {
      j = col*ATOM_MAPS + view -> anim -> last -> img -> color_map[col] + 1;
    }
    else
    {
      j = col*ATOM_MAPS + ((view -> custom_map) ? 6 : 0);
    }
  }
  else
  {
    j = col*ATOM_MAPS;
  }
  gboolean was_input = reading_input;
  reading_input = TRUE;
#ifdef GTK3
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> color_styles[j], TRUE);
  set_color_map (view -> color_styles[j], & view -> colorp[j][0]);
#else
  gchar * variant = g_strdup_printf ("set-%s.%d.0", (col) ? "pmap" : "amap", j);
  g_action_group_activate_action ((GActionGroup *)view -> action_group, (col) ? "set-pmap" : "set-amap", g_variant_new_string((const gchar *)variant));
  g_free (variant);
#endif
  reading_input = was_input;
}

/*!
  \fn void set_motion (glwin * view, int axis, int da, int db, gboolean UpDown, GdkModifierType state)

  \brief handle keyboard motion event on the OpenGL window

  \param view the target glwin
  \param axis axis
  \param da direction (-1/1)
  \param db zoom out (1) or zoom in (3)
  \param UpDown up or down key motion (TRUE), or else (FALSE)
  \param state keyboard modifier
*/
void set_motion (glwin * view, int axis, int da, int db, gboolean UpDown, GdkModifierType state)
{
  if (state & GDK_CONTROL_MASK)
  {
    if (view -> mode == EDITION)
    {
      vec3_t trans;
      if (axis)
      {
        trans = vec3(view -> anim -> last -> img -> p_depth*(double) da / view -> pixels[axis], 0.0, 0.0);
      }
      else
      {
        trans = vec3(0.0, view -> anim -> last -> img -> p_depth*(double) da / view -> pixels[axis], 0.0);
      }
      translate (get_project_by_id(view -> proj), 1, 1, trans);
      init_default_shaders (view);
      update (view);
    }
    else if (state & GDK_SHIFT_MASK)
    {
       spin_go (NULL, & view -> colorp[db][axis]);
    }
    else
    {
      view -> anim -> last -> img -> c_shift[! axis] += (double) da / view -> pixels[axis];
      if (view -> rep_win)
      {
        if (view -> rep_win -> camera_widg[! axis + 5] && GTK_IS_WIDGET(view -> rep_win -> camera_widg[! axis + 5]))
        {
          gtk_spin_button_set_value ((GtkSpinButton *)view -> rep_win -> camera_widg[! axis + 5], - view -> anim -> last -> img -> c_shift[! axis]);
        }
      }
      update (view);
    }
  }
  else if ((state & GDK_SHIFT_MASK) && UpDown)
  {
    if (db == 1 && view -> anim -> last -> img -> zoom > ZOOM_MAX) zoom (view, -1);
    if (db == 3) zoom (view, 1);
    update (view);
  }
  else if (state != GDK_SHIFT_MASK)
  {
    save_rotation_quaternion (view);
    double cameraAngle[2] = {0.0, 0.0};
    cameraAngle[axis] = 0.5*da;
    rotate_x_y (view, cameraAngle[0], cameraAngle[1]);
    update (view);
  }
}

/*!
  \fn mat4_t insert_projection (glwin * view)

  \brief calculate the insertion projection matrix to insert object in the 3D window

  \param view the target glwin
*/
mat4_t insert_projection (glwin * view)
{
  GLdouble w, h;
  GLdouble dw, dh;
  double gleft, gright, gbottom, gtop;
  double zoom = view -> anim -> last -> img -> zoom;
  zoom *= (view -> anim -> last -> img -> p_depth /  view -> anim -> last -> img -> gnear);
  dw = view -> anim -> last -> img -> c_shift[0]*2.0*zoom;
  dh = view -> anim -> last -> img -> c_shift[1]*2.0*zoom;
  double aspect = (double)view -> pixels[0] / (double)view -> pixels[1];
  if (aspect > 1.0)
  {
    w = zoom * aspect;
    h = zoom;
  }
  else
  {
    w = zoom;
    h = zoom / aspect;
  }
  gleft = -w + dw;
  gright = w + dw;
  gbottom = -h + dh;
  gtop = h + dh;
  return m4_ortho (gleft, gright, gbottom, gtop, -view -> anim -> last -> img -> gfar, view -> anim -> last -> img -> gfar);
}

/*!
  \fn vec3_t get_insertion_coordinates (glwin * view)

  \brief get the insertion coordinates to insert object in the 3D window

  \param view the target glwin
*/
vec3_t get_insertion_coordinates (glwin * view)
{
  vec3_t pos;
  mat4_t insert_pmv_matrix;
  insert_pmv_matrix = m4_mul (insert_projection (view), view -> model_view_matrix);
  pos = vec3 (view -> mouseX, view -> mouseY, 0.75);
  return v3_un_project (pos, view -> view_port, insert_pmv_matrix);
}

/*!
  \fn void activate_glwin_action (gchar * action_string, gchar * action_name, glwin * view)

  \brief the keyboard shortcut actions for the OpenGL window

  \param action_string the variant string
  \param action_name the action name
  \param view the target glwin
*/
void activate_glwin_action (gchar * action_string, gchar * action_name, glwin * view)
{
  GVariant * action_state = g_variant_new_string (action_string);
  g_action_group_activate_action ((GActionGroup *)view -> action_group, action_name, action_state);
  // g_variant_unref (action_state);
}

/*!
  \fn void glwin_key_pressed (guint keyval, GdkModifierType state, gpointer data)

  \brief the keyboard shortcut actions for the OpenGL window

  \param keyval the key pressed
  \param state the keyboard modifier
  \param data the associated data pointer
*/
void glwin_key_pressed (guint keyval, GdkModifierType state, gpointer data)
{
  glwin * view = (glwin *)data;
  int i;
  switch (keyval)
  {
    case GDK_KEY_Escape:
      if (view -> fullscreen)
      {
#ifdef GTK4
        set_full_screen (NULL, NULL, view);
#else
        set_full_screen (NULL, view);
#endif
      }
      break;
    case GDK_KEY_Delete:
      if (get_project_by_id(view -> proj) -> natomes && ! is_atom_win_active(view) && get_project_by_id(view -> proj) -> steps == 1 && view -> mode == EDITION)
      {
        opengl_project_changed (view -> proj);
        is_selected = 1;
        selected_aspec = -1;
#ifdef GTK4
        remove_the_atoms (NULL, NULL, & cut_sel);
#else
        remove_the_atoms (NULL, & cut_sel);
#endif
      }
      break;
    case GDK_KEY_space:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        if (view -> spin[0] || view -> spin[1])
        {
          for (i=0; i<2; i++)
          {
            view -> spin[i+2] = view -> spin[i];
            view -> spin_speed[i+2] = view -> spin_speed[i];
            view -> spin[i] = FALSE;
          }
        }
        else if (view -> spin[2] || view -> spin[3])
        {
          for (i=0; i<2; i++)
          {
            view -> spin[i] = view -> spin[i+2];
            view -> spin_speed[i] = view -> spin_speed[i+2];
            g_timeout_add (REFRESH, (GSourceFunc) spin, & view -> colorp[0][i]);
            view -> spin[i+2] = FALSE;
            view -> spin_speed[i+2] = 0;
          }
        }
      }
      break;
    case GDK_KEY_a:
      if ((state & GDK_CONTROL_MASK) && get_project_by_id(view -> proj) -> natomes)
      {
        opengl_project_changed (view -> proj);
        selected_aspec = -1;
        i = get_selection_type (view);
        if (view -> anim -> last -> img -> selected[i] -> selected == opengl_project -> natomes)
        {
#ifdef GTK4
          select_unselect_atoms (NULL, NULL, & view -> colorp[0][0]);
#else
          select_unselect_atoms (NULL, & view -> colorp[0][0]);
#endif
        }
        else
        {
#ifdef GTK4
          select_unselect_atoms (NULL, NULL, & view -> colorp[0][1]);
#else
          select_unselect_atoms (NULL, & view -> colorp[0][1]);
#endif
        }
      }
#ifdef GTK4
#ifdef OSX
      else if (state & GDK_META_MASK)
#else
      else if (state & GDK_ALT_MASK)
#endif
#else
      else if (state & GDK_MOD1_MASK)
#endif
      {
        if (view -> mode == EDITION)
        {
#ifdef GTK4
          activate_glwin_action ("set-mouse-mode.0.0", "set-mouse-mode", view);
#else
          // GTK3 Menu Action To Check
          gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_mode[0], TRUE);
          set_mode (view -> ogl_mode[0], & view -> colorp[0][0]);
#endif
        }
      }
      else if (get_project_by_id(view -> proj) -> natomes)
      {
        change_color_map (view, 0);
        update (view);
      }
      break;
    case GDK_KEY_A:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        // selection mode to coordination sphere
#ifdef GTK4
        activate_glwin_action ("set-sel-mode.0.0", "set-sel-mode", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_smode[0], TRUE);
        set_selection_mode (view -> ogl_smode[0], & view -> colorp[0][0]);
#endif
      }
      break;
    case GDK_KEY_b:
      if (get_project_by_id(view -> proj) -> natomes)
      {
#ifdef GTK4
        activate_glwin_action ("set-style.0.0", "set-style", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_styles[BALL_AND_STICK], TRUE);
        set_style (view -> ogl_styles[BALL_AND_STICK], & view -> colorp[BALL_AND_STICK][0]);
#endif

      }
      break;
    case GDK_KEY_c:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        if (state & GDK_CONTROL_MASK)
        {
          if (view -> anim -> last -> img -> selected[0] -> selected)
          {
            opengl_project_changed (view -> proj);
            selected_aspec = -1;
            is_selected = 1;
            view -> nth_copy = 1;
            view -> insert_coords = get_insertion_coordinates (view);
#ifdef GTK4
            copy_the_atoms (NULL, NULL, & cut_sel);
#else
            copy_the_atoms (NULL, & cut_sel);
#endif
            update_insert_combos ();
          }
          else
          {
            if (copied_object)
            {
              g_free (copied_object);
              copied_object = NULL;
            }
          }
        }
        else
        {
#ifdef GTK4
          activate_glwin_action ("set-style.8.0", "set-style", view);
#else
          // GTK3 Menu Action To Check
          gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_styles[CYLINDERS], TRUE);
          set_style (view -> ogl_styles[CYLINDERS], & view -> colorp[CYLINDERS][0]);
#endif
        }
      }
      break;
    case GDK_KEY_C:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        // selection mode to coordination sphere
#ifdef GTK4
        activate_glwin_action ("set-sel-mode.1.0", "set-sel-mode", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_smode[1], TRUE);
        set_selection_mode (view -> ogl_smode[1], & view -> colorp[1][0]);
#endif
      }
      break;
    case GDK_KEY_d:
      if (get_project_by_id(view -> proj) -> natomes)
      {
#ifdef GTK4
        activate_glwin_action ("set-style.9.0", "set-style", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_styles[PUNT], TRUE);
        set_style (view -> ogl_styles[PUNT], & view -> colorp[PUNT][0]);
#endif
      }
      break;
    case GDK_KEY_e:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        if (state & GDK_CONTROL_MASK)
        {
          coord_properties (NULL, & view -> colorp[30][0]);
        }
#ifdef GTK4
#ifdef OSX
        else if (state & GDK_META_MASK)
#else
        else if (state & GDK_ALT_MASK)
#endif
#else
        else if (state & GDK_MOD1_MASK)
#endif
        {
          if (view -> mode == ANALYZE && get_project_by_id(view -> proj) -> steps == 1)
          {
#ifdef GTK4
            activate_glwin_action ("set-mouse-mode.1.0", "set-mouse-mode", view);
#else
            // GTK3 Menu Action To Check
            gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_mode[1], TRUE);
            set_mode (view -> ogl_mode[1], & view -> colorp[1][0]);
#endif
          }
        }
      }
      break;
    case GDK_KEY_f:
#ifdef GTK4
      if (state & GDK_CONTROL_MASK) set_full_screen (NULL, NULL, view);
#else
      if (state & GDK_CONTROL_MASK) set_full_screen (NULL, view);
#endif
      break;
    case GDK_KEY_F:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        // selection mode to coordination sphere
#ifdef GTK4
        activate_glwin_action ("set-sel-mode.2.0", "set-sel-mode", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_smode[2], TRUE);
        set_selection_mode (view -> ogl_smode[2], & view -> colorp[2][0]);
#endif
      }
      break;
    case GDK_KEY_i:
      if (get_project_by_id(view -> proj) -> natomes)
      {
#ifdef GTK4
        activate_glwin_action ("set-style.3.0", "set-style", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> filled_styles[1], TRUE);
        set_style (view -> filled_styles[1], & view -> colorp[OGL_STYLES+1][0]);
#endif
      }
      break;
    case GDK_KEY_l:
      if ((state & GDK_CONTROL_MASK) && get_project_by_id(view -> proj) -> natomes)
      {
        opengl_project_changed (view -> proj);
        selected_aspec = is_selected = -1;
        if (view -> labelled == opengl_project -> natomes*opengl_project -> steps)
        {
#ifdef GTK4
           label_unlabel_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][0]);
#else
           label_unlabel_atoms (NULL, & opengl_project -> modelgl -> colorp[0][0]);
#endif
        }
        else
        {
#ifdef GTK4
           label_unlabel_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][1]);
#else
           label_unlabel_atoms (NULL, & opengl_project -> modelgl -> colorp[0][1]);
#endif
        }
      }
      break;
    case GDK_KEY_m:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        if (state & GDK_CONTROL_MASK)
        {
          view -> anim -> last -> img -> m_is_pressed = 0;
          window_measures (NULL, view);
        }
        else if (view -> anim -> last -> img -> m_is_pressed < 2)
        {
          view -> anim -> last -> img -> m_is_pressed ++;
        }
        else
        {
          view -> anim -> last -> img -> m_is_pressed = 0;
        }
        view -> create_shaders[MEASU] = TRUE;
        update (view);
      }
      break;
    case GDK_KEY_M:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        // selection mode to coordination sphere
#ifdef GTK4
        activate_glwin_action ("set-sel-mode.3.0", "set-sel-mode", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_smode[3], TRUE);
        set_selection_mode (view -> ogl_smode[3], & view -> colorp[3][0]);
#endif
      }
      break;
    case GDK_KEY_n:
      if (state & GDK_CONTROL_MASK) on_create_new_project (NULL, NULL);
      break;
    case GDK_KEY_o:
      if (get_project_by_id(view -> proj) -> natomes)
      {
#ifdef GTK4
          activate_glwin_action ("set-style.2.0", "set-style", view);
#else
          // GTK3 Menu Action To Check
          gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> filled_styles[0], TRUE);
          set_style (view -> filled_styles[0], & view -> colorp[OGL_STYLES][0]);
#endif
      }
      break;
    case GDK_KEY_p:
      if (get_project_by_id(view -> proj) -> natomes) change_color_map (view, 1);
      break;
    case GDK_KEY_r:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        if (state & GDK_CONTROL_MASK)
        {
          window_recorder (NULL, (gpointer)view);
        }
        else
        {
#ifdef GTK4
          activate_glwin_action ("set-style.5.0", "set-style", view);
#else
        // GTK3 Menu Action To Check
          gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> filled_styles[3], TRUE);
          set_style (view -> filled_styles[3], & view -> colorp[OGL_STYLES+3][0]);
#endif
        }
      }
      break;
      if ((state & GDK_CONTROL_MASK) && get_project_by_id(view -> proj) -> natomes) window_recorder (NULL, (gpointer)view);
      break;
    case GDK_KEY_s:
      if (get_project_by_id(view -> proj) -> natomes)
      {
        if (state & GDK_CONTROL_MASK)
        {
          spin_stop (NULL, data);
        }
        else
        {
#ifdef GTK4
          activate_glwin_action ("set-style.7.0", "set-style", view);
#else
          // GTK3 Menu Action To Check
          gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_styles[SPHERES], TRUE);
          set_style (view -> ogl_styles[SPHERES], & view -> colorp[SPHERES][0]);
#endif
        }
      }
      break;
    case GDK_KEY_v:
      if (state & GDK_CONTROL_MASK)
      {
        if (copied_object && ! is_atom_win_active(view) && get_project_by_id(view -> proj) -> steps == 1 && view -> mode == EDITION)
        {
          //i = (! get_project_by_id(view -> proj) -> natomes) ? 1 : 0;
          opengl_project_changed (view -> proj);
#ifdef GTK4
          add_object (NULL, NULL, & view -> colorp[0][0]);
#else
          add_object (NULL, & view -> colorp[0][0]);
#endif
          view -> nth_copy ++;
          /*if (i)
          {
            i = activep;
            active_project_changed (view -> proj);
            frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
            mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
            bonds_update = 1;
            on_calc_bonds_released (NULL, NULL);
            active_project_changed (i);
          }*/
        }
      }
      else if (get_project_by_id(view -> proj) -> natomes)
      {
#ifdef GTK4
        activate_glwin_action ("set-style.4.0", "set-style", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> filled_styles[2], TRUE);
        set_style (view -> filled_styles[2], & view -> colorp[OGL_STYLES+2][0]);
#endif
      }
      break;
    case GDK_KEY_w:
      if (get_project_by_id(view -> proj) -> natomes)
      {
#ifdef GTK4
        activate_glwin_action ("set-style.1.0", "set-style", view);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_styles[WIREFRAME], TRUE);
        set_style (view -> ogl_styles[WIREFRAME], & view -> colorp[WIREFRAME][0]);
#endif
      }
      break;
    case GDK_KEY_x:
      if ((state & GDK_CONTROL_MASK) && get_project_by_id(view -> proj) -> natomes)
      {
        if (! is_atom_win_active(view) && get_project_by_id(view -> proj) -> steps == 1 && view -> mode == EDITION)
        {
          if (view -> anim -> last -> img -> selected[0] -> selected)
          {
            opengl_project_changed (view -> proj);
            selected_aspec = -1;
            is_selected = 1;
#ifdef GTK4
            copy_the_atoms (NULL, NULL, & cut_sel);
            remove_the_atoms (NULL, NULL, & cut_sel);
#else
            copy_the_atoms (NULL, & cut_sel);
            remove_the_atoms (NULL, & cut_sel);
#endif
          }
        }
      }
      break;
    case GDK_KEY_Right:
      if (get_project_by_id(view -> proj) -> natomes) set_motion (view, 1, 1, 1, FALSE, state);
      break;
    case GDK_KEY_Left:
      if (get_project_by_id(view -> proj) -> natomes) set_motion (view, 1, -1, 3, FALSE, state);
      break;
    case GDK_KEY_Up:
      if (get_project_by_id(view -> proj) -> natomes) set_motion (view, 0, 1, 3, TRUE, state);
      break;
    case GDK_KEY_Down:
      if (get_project_by_id(view -> proj) -> natomes) set_motion (view, 0, -1, 1, TRUE, state);
      break;
  }
}

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT gboolean on_key_pressed (GtkWidget * widg, GdkEventKey * event, gpointer data)

  \brief keyboard key press event for the OpenGL window GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEventKey triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_key_pressed (GtkWidget * widg, GdkEventKey * event, gpointer data)
{
  if (event -> type == GDK_KEY_PRESS)
  {
    glwin_key_pressed (event -> keyval, event -> state, data);
  }
  return FALSE;
}
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_glwin_key_pressed (GtkEventControllerKey * self, guint keyval, guint keycode, GdkModifierType state, gpointer data)

  \brief keyboard key press event for the OpenGL window GTK4

  \param self the GtkEventControllerKey sending the signal
  \param keyval number of times it was pressed
  \param keycode the key pressed
  \param state the keyboard modifier
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_glwin_key_pressed (GtkEventControllerKey * self, guint keyval, guint keycode, GdkModifierType state, gpointer data)
{
  glwin_key_pressed (keyval, state, data);
  return TRUE;
}
#endif

/*!
  \fn G_MODULE_EXPORT void on_win_realize (GtkWidget * widg, gpointer data)

  \brief realize event for the OpenGL window

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_win_realize (GtkWidget * widg, gpointer data)
{
  glwin * view = (glwin *)data;
  int * pix = get_project_by_id(view -> proj) -> tmp_pixels;
  int shift = 0;
  if (GTK_IS_WIDGET(view -> menu_bar)) shift = get_widget_height (view -> menu_bar);
  if (pix[0] > 0 && pix[1] > 0)
  {
    resize_this_window (view -> win, pix[0], pix[1]+shift);
    pix[0] = pix[1] = -1;
  }
  else
  {
    resize_this_window (view -> win, 500, 500);
  }
}

#ifdef GTK3
#ifdef LINUX
/*!
  \fn void gtk_window_change_gdk_visual (GtkWidget * win)

  \brief change the Gdk visual

  \param win the GtkWidget sending the signal
*/
void gtk_window_change_gdk_visual (GtkWidget * win)
{
  // GTK+ > 3.15.1 uses an X11 visual optimized for GTK+'s OpenGL stuff
  // since revid dae447728d: https://github.com/GNOME/gtk/commit/dae447728d
  // However, in some cases it simply cannot start an OpenGL context.
  // This changes to the default X11 visual instead the GTK's default.
  GdkScreen * screen = gdk_screen_get_default ();
  GList * visuals = gdk_screen_list_visuals (screen);
  // printf("n visuals: %u\n", g_list_length(visuals));
  GdkX11Screen* x11_screen = GDK_X11_SCREEN (screen);
  g_assert (x11_screen != NULL);
  Visual * default_xvisual = DefaultVisual (GDK_SCREEN_XDISPLAY(x11_screen), GDK_SCREEN_XNUMBER(x11_screen));
  GdkVisual * default_visual = NULL;
  // int i = 0;
  while (visuals != NULL)
  {
    GdkVisual * visual = GDK_X11_VISUAL (visuals -> data);
    if (default_xvisual -> visualid == gdk_x11_visual_get_xvisual(GDK_X11_VISUAL (visuals -> data)) -> visualid)
    {
      // printf("Default visual %d\n", i);
      default_visual = visual;
    }
    // i++;
    visuals = visuals -> next;
  }
  gtk_widget_set_visual (win, default_visual);
}
#endif
#endif

gboolean create_3d_model (int p, gboolean load)
{
  project * this_proj = get_project_by_id (p);
#ifndef GTKGLAREA
  if (! glXQueryExtension (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), NULL, NULL))
  {
    g_warning ("Sorry OpenGL is not supported !");
    return FALSE;
  }
  else
#endif
  {
    if (this_proj -> modelgl)
    {
      g_free (this_proj -> modelgl);
      this_proj -> modelgl = NULL;
    }
    this_proj -> modelgl = g_malloc0 (sizeof*this_proj -> modelgl);
    this_proj -> modelgl -> init = FALSE;
    this_proj -> modelgl -> proj = this_proj -> id;
    GtkWidget * gl_vbox;
    if (load)
    {
      gchar * str = g_strdup_printf ("%s - 3D view - [%s mode]", this_proj -> name, mode_name[0]);
      this_proj -> modelgl -> win = create_win (str, MainWindow, FALSE, TRUE);
#ifdef GTK3
#ifdef GTKGLAREA
#ifdef LINUX
      if (! atomes_visual) gtk_window_change_gdk_visual (this_proj -> modelgl -> win);
#endif
#endif
#endif
      // this_proj -> modelgl -> accel_group = gtk_accel_group_new ();
      // gtk_window_add_accel_group (GTK_WINDOW (this_proj -> modelgl -> win), this_proj -> modelgl -> accel_group);
      g_free (str);
      gl_vbox = create_vbox (BSEP);
      add_container_child (CONTAINER_WIN, this_proj -> modelgl -> win, gl_vbox);
      this_proj -> modelgl -> menu_box = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, gl_vbox, this_proj -> modelgl -> menu_box, FALSE, FALSE, 0);
#ifdef GTK3
      this_proj -> modelgl -> menu_bar = gtk_menu_bar_new ();
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, this_proj -> modelgl -> menu_box, this_proj -> modelgl -> menu_bar, TRUE, TRUE, 0);
#endif
    }
#ifdef GTKGLAREA
    this_proj -> modelgl -> plot = gtk_gl_area_new ();
    gtk_widget_set_can_focus (this_proj -> modelgl -> plot, TRUE);
#ifdef GTK4
    gtk_widget_set_focusable (this_proj -> modelgl -> plot, TRUE);
#endif
#else
    this_proj -> modelgl -> plot = gtk_drawing_area_new ();
#endif
    gtk_widget_set_size_request (this_proj -> modelgl -> plot, 100, 100);
#ifndef GTKGLAREA
    gtk_widget_set_double_buffered (this_proj -> modelgl -> plot, FALSE);
    this_proj -> modelgl -> glcontext = NULL;
#endif
    gtk_widget_set_hexpand (this_proj -> modelgl -> plot, TRUE);
    gtk_widget_set_vexpand (this_proj -> modelgl -> plot, TRUE);
#ifdef GTKGLAREA
#ifdef GTK3
    gtk_widget_add_events (GTK_WIDGET (this_proj -> modelgl -> plot),
                           GDK_SCROLL_MASK |
                           GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK |
                           GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
#endif
#else
    gtk_widget_add_events (GTK_WIDGET (this_proj -> modelgl -> plot),
                           GDK_SMOOTH_SCROLL_MASK |
                           GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK |
                           GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
#endif
#ifdef GTK4
    if (load)
    {
      add_widget_gesture_and_key_action (this_proj -> modelgl -> win, NULL, NULL, NULL,
                                                                      NULL, NULL, NULL,
                                                                      "glwin-key-pressed", G_CALLBACK(on_glwin_key_pressed), this_proj -> modelgl,
                                                                      NULL, NULL, NULL,
                                                                      NULL, NULL, NULL);
      add_widget_gesture_and_key_action (this_proj -> modelgl -> plot, "glwin-button-pressed", G_CALLBACK(on_glwin_button_pressed), this_proj -> modelgl,
                                                                       "glwin-button-released", G_CALLBACK(on_glwin_button_released), this_proj -> modelgl,
                                                                       NULL, NULL, NULL,
                                                                       "glwin-pointer-motion", G_CALLBACK(on_glwin_pointer_motion), this_proj -> modelgl,
                                                                       "glwin-pointer-scroll", G_CALLBACK(on_glwin_pointer_scoll), this_proj -> modelgl);
    }
    else
    {
      add_widget_gesture_and_key_action (this_proj -> modelgl -> plot, "glwin-button-pressed", G_CALLBACK(on_lib_button_pressed), this_proj -> modelgl,
                                                                       "glwin-button-released", G_CALLBACK(on_lib_button_released), this_proj -> modelgl,
                                                                       NULL, NULL, NULL,
                                                                       "glwin-pointer-motion", G_CALLBACK(on_glwin_pointer_motion), this_proj -> modelgl,
                                                                       NULL, NULL, NULL);
    }
#else
    g_signal_connect (G_OBJECT (this_proj -> modelgl -> plot), "motion-notify-event", G_CALLBACK(on_motion), this_proj -> modelgl);
    g_signal_connect (G_OBJECT (this_proj -> modelgl -> plot), "button-release-event", G_CALLBACK(on_glwin_button_event), this_proj -> modelgl);
    if (load)
    {
      g_signal_connect (G_OBJECT (this_proj -> modelgl -> plot), "button-press-event", G_CALLBACK(on_glwin_button_event), this_proj -> modelgl);
      g_signal_connect (G_OBJECT (this_proj -> modelgl -> plot), "scroll-event", G_CALLBACK(on_scrolled), this_proj -> modelgl);
    }
    else
    {
      g_signal_connect (G_OBJECT (this_proj -> modelgl -> plot), "button-press-event", G_CALLBACK(on_lib_pressed), this_proj -> modelgl);
    }
#endif
    if (load)
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, gl_vbox, this_proj -> modelgl -> plot, FALSE, TRUE, 0);
#ifdef GTK3
      g_signal_connect (G_OBJECT (this_proj -> modelgl -> win), "key-press-event", G_CALLBACK(on_key_pressed), this_proj -> modelgl);
#endif
      g_signal_connect (G_OBJECT (this_proj -> modelgl -> win), "realize", G_CALLBACK(on_win_realize), this_proj -> modelgl);
      add_gtk_close_event (this_proj -> modelgl -> win, G_CALLBACK(hide_this_window), NULL);
    }
    g_signal_connect (G_OBJECT (this_proj -> modelgl -> plot), "realize", G_CALLBACK(on_realize), this_proj -> modelgl);
#ifdef GTKGLAREA
    g_signal_connect (G_OBJECT (this_proj -> modelgl -> plot), "render", G_CALLBACK(on_expose), this_proj -> modelgl);
#else
    g_signal_connect (G_OBJECT (this_proj -> modelgl -> plot), "draw", G_CALLBACK(on_expose), this_proj -> modelgl);
#endif
    return TRUE;
  }
}

/*!
  \fn void prep_model (int p)

  \brief prepare, or display, the OpenGL model window

  \param p the project id
*/
void prep_model (int p)
{
  project * this_proj = get_project_by_id (p);
  gboolean adv_bonding[2];
  if (this_proj -> modelgl == NULL)
  {
    if (create_3d_model (p, TRUE))
    {
      /*GtkWidget * dummy = create_menu_item (FALSE, "Dummy");
      gtk_menu_shell_append ((GtkMenuShell *)this_proj -> modelgl -> menu_bar, dummy);
      show_the_widgets (this_proj -> modelgl -> win);
      destroy_this_widget (dummy);*/
      show_the_widgets (this_proj -> modelgl -> win);
    }
    if (this_proj -> initgl)
    {
      active_project_changed (p);
#ifdef GTK3
      // GTK3 Menu Action To Check
      active_glwin -> ogl_box_axis[0] = g_malloc0 (OGL_BOX*sizeof*active_glwin -> ogl_box_axis[0]);
      active_glwin -> ogl_box_axis[1] = g_malloc0 (OGL_AXIS*sizeof*active_glwin -> ogl_box_axis[1]);
#endif
      prepare_opengl_menu_bar (active_glwin);
#ifdef GTK3
      GtkWidget * menu = gtk_menu_bar_new ();
      gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Help", TRUE, menu_help(active_glwin, 0)));
      add_box_child_end (active_glwin -> menu_box, menu, FALSE, FALSE, 0);
      show_the_widgets (menu);
#endif
      if (reading_input)
      {
        adv_bonding[0] = (active_project -> natomes > ATOM_LIMIT) ? 0 : tmp_adv_bonding[0];
        adv_bonding[1] = (active_project -> steps > STEP_LIMIT) ? 0 : tmp_adv_bonding[1];
        frag_update = (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
        mol_update = (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
      }
      else
      {
        frag_update = (force_mol) ? 1 : (active_project -> natomes > ATOM_LIMIT) ? 0 : 1;
        mol_update = (force_mol) ? 1 : (frag_update) ? ((active_project -> steps > STEP_LIMIT) ? 0 : 1) : 0;
        adv_bonding[0] = adv_bonding[1] = TRUE;
      }
      if (active_project -> natomes && adv_bonding[0] && adv_bonding[1])
      {
        bonds_update = 1;
        active_project -> runc[0] = FALSE;
        on_calc_bonds_released (NULL, NULL);
      }
    }
  }
  else
  {
    if (gtk_widget_get_visible(this_proj -> modelgl -> win))
    {
      hide_the_widgets (this_proj -> modelgl -> win);
    }
    else
    {
      show_the_widgets (this_proj -> modelgl -> win);
    }
  }
}
