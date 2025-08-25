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
* @file w_advance.c
* @short Functions to create the OpenGL parameters edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_advance.c'
*
* Contains:
*

 - The functions to create the OpenGL parameters edition window

*
* List of functions:

  int * light_source_to_be_removed (int val, Lightning * ogl_lightning, opengl_edition * ogl_edit);

  G_MODULE_EXPORT gboolean scroll_scale_param (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
  G_MODULE_EXPORT gboolean scroll_scale_quality (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
  G_MODULE_EXPORT gboolean scroll_set_fog_param (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
  G_MODULE_EXPORT gboolean close_advanced (GtkWidget * window, gpointer data);
  G_MODULE_EXPORT gboolean close_advanced (GtkWidget * widg, GdkEvent * event, gpointer data);

  void print_light_source (Light source, int i);
  void show_active_light_data (opengl_edition * ogl_win, int lid, int tid);
  void update_light_data (int li, opengl_edition * ogl_win);
  void create_lights_combo (int num_lights, opengl_edition * ogl_win);
  void add_remove_lights (int val, gpointer data);
  void set_data_pos (vec3_t * vect, int pos, double v);
  void param_has_changed (gpointer data, double val);
  void fog_param_changed (gpointer data, GLfloat u, GtkRange * range);
  void setup_fog_dialogs (opengl_edition * ogl_edit, int fid);
  void close_advanced_opengl (gpointer data);

  G_MODULE_EXPORT void toggled_delete_ligth (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void toggled_delete_ligth (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void run_light_source_to_be_removed (GtkDialog * win, gint response_id, gpointer data);
  G_MODULE_EXPORT void show_light_param (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_nlights_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_nlights (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void update_light_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_object_pos (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_light_type (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_light_fix (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void show_this_light (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void show_this_light (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_use_template_toggle (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_use_template_toggle (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_template (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_l_model (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void update_mat_param (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void scale_param (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void scale_quality (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void set_fog_param (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void set_fog_type (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void set_fog_mode (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void opengl_advanced (GtkWidget * widg, gpointer data);

  GtkWidget * adv_box (GtkWidget * box, char * lab, int vspace, int size, float xalign);
  GtkWidget * bdv_box (GtkWidget * box, char * lab, int size, float xalign);
  GtkWidget * GtkWidget * create_setting_pos (gchar * lab, int size, float xalign, int pid, int lid, float * values, opengl_edition * ogl_win);
  GtkWidget * lights_tab (glwin * view, opengl_edition * ogl_edit, Lightning * the_light);
  GtkWidget * materials_tab (glwin * view, opengl_edition * ogl_edit, Material * the_mat);
  GtkWidget * fog_tab (glwin * view, opengl_edition * ogl_edit, Fog * the_fog);

  Light init_light_source (int type, float val, float vbl);
  Light copy_light_source (Light old_sp);
  Light * copy_light_sources (int dima, int dimb, Light * old_sp);

*/

#include "global.h"
#include "interface.h"
#include "preferences.h"
#include "glview.h"
#include "glwindow.h"

extern void set_quality (int q, glwin * view);

gchar * material_template[TEMPLATES] = {"Opaque",
                                        "Brushed metal",
                                        "Shiny metal",
                                        "Plastic",
                                        "Transparent",
                                        "Translucent",
                                        "Diffuse"};

GLfloat template_parameters[TEMPLATES][5] ={{0.50, 0.50, 0.90, 1.00, 1.00},  // Ok
                                            {0.90, 0.60, 1.00, 1.50, 1.00},  // Ok
                                            {0.80, 0.40, 1.00, 1.00, 1.00},  // Ok
                                            {0.35, 0.15, 1.00, 1.50, 1.00},  // Ok
                                            {0.50, 0.50, 0.50, 1.00, 0.50},  //
                                            {0.50, 0.50, 0.50, 1.00, 0.75},  //
                                            {0.35, 0.80, 1.00, 1.50, 1.00}}; // Ok

float mat_min_max[5][2] = {{0.0, 1.0},
                           {0.0, 1.0},
                           {0.0, 10.0},
                           {0.0, 10.0},
                           {0.0, 1.0}};

gchar * ogl_settings[3][10] = {{"<u>Albedo:</u>",
                                "<u>Metallic:</u>",
                                "<u>Roughness:</u>",
                                "<u>Ambient occlusion:</u>",
                                "<u>Gamma correction:</u>",
                                "<u>Opacity:</u>"},
                               {"<u>Position:</u>",
                                "<u>Direction:</u>",
                                "<u>Intensity:</u>",
                                "<u>Constant attenuation:</u>",
                                "<u>Linear attenuation:</u>",
                                "<u>Quadratic attenuation:</u>",
                                "<u>Cone angle</u>",
                                "<u>Inner cutoff:</u>",
                                "<u>Outer cutoff:</u>",
                                "<u>Type:</u>"},
                               {"<b>Fog color</b>"}};

gchar * lpos[3] = {"x", "y", "z"};
gchar * cpos[3] = {"r", "g", "b"};

/*!
  \fn GtkWidget * adv_box (GtkWidget * box, char * lab, int vspace, int size, float xalign)

  \brief create a box with markup label

  \param box the box to insert the widget in
  \param lab label
  \param vspace vertical space
  \param size size
  \param xalign x alignement
*/
GtkWidget * adv_box (GtkWidget * box, char * lab, int vspace, int size, float xalign)
{
  GtkWidget * hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(lab, size, -1, xalign, 0.5), FALSE, FALSE, 25);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, vspace);
  return hbox;
}

/*!
  \fn GtkWidget * bdv_box (GtkWidget * box, char * lab, int size, float xalign)

  \brief create a box with markup label

  \param box the GtkWidget sending the signal
  \param lab label
  \param size size
  \param xalign x alignement
*/
GtkWidget * bdv_box (GtkWidget * box, char * lab, int size, float xalign)
{
  GtkWidget * hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (lab, size, -1, xalign, 0.5), FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 5);
  return hbox;
}

GtkWidget * d_close;
int status;

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void toggled_delete_ligth (GtkCheckButton * but, gpointer data)

  \brief toggle delete light callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_delete_ligth (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void toggled_delete_ligth (GtkToggleButton * but, gpointer data)

  \brief toggle delete light callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggled_delete_ligth (GtkToggleButton * but, gpointer data)
#endif
{
  if (button_get_status ((GtkWidget *)but))
  {
    status --;
  }
  else
  {
    status ++;
  }
  if (status == 0)
  {
    widget_set_sensitive (d_close, 1);
  }
  else
  {
    widget_set_sensitive (d_close, 0);
  }
}

int * light_list;
GtkWidget ** light_but;

/*!
  \fn G_MODULE_EXPORT void run_light_source_to_be_removed (GtkDialog * win, gint response_id, gpointer data)

  \brief remove light source(s) - running the dialog

  \param win the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_light_source_to_be_removed (GtkDialog * win, gint response_id, gpointer data)
{
  Lightning * ogl_lightning = (Lightning *)data;
  int i, j;
  j = 0;
  for (i=0; i<ogl_lightning -> lights; i++)
  {
    if (button_get_status ((GtkWidget *)light_but[i]))
    {
      light_list[j] = i;
      j ++;
    }
  }
  destroy_this_dialog (win);
}

/*!
  \fn int * light_source_to_be_removed (int val, Lightning * ogl_lightning, opengl_edition * ogl_edit)

  \brief remove light source(s) - creating the dialog

  \param val number of light(s) to remove
  \param ogl_lightning the target lightning data structure
  \param ogl_edit the target OpenGL edition window
*/
int * light_source_to_be_removed (int val, Lightning * ogl_lightning, opengl_edition * ogl_edit)
{
  int i;
  gchar * str;
  status = val;
  GtkWidget * win = dialogmodal ("Remove light source(s)", GTK_WINDOW(ogl_edit -> win));
  GtkWidget * vbox = dialog_get_content_area (win);
  d_close =  gtk_dialog_get_widget_for_response (GTK_DIALOG (win), GTK_RESPONSE_CLOSE);
  widget_set_sensitive (d_close, 0);
  if (val > 1)
  {
    str = g_strdup_printf ("Please select the %d light sources to be removed: ", val);
  }
  else
  {
    str = g_strdup_printf ("Please select the %d light source to be removed: ", val);
  }
  bbox (vbox, str);
  g_free (str);
  light_but = g_malloc (ogl_lightning -> lights * sizeof*light_but);
  for (i=0; i<ogl_lightning -> lights; i++)
  {
    str = g_strdup_printf ("Light N°%d", i+1);
    light_but[i] = check_button (str, -1, 40, FALSE, G_CALLBACK(toggled_delete_ligth), (gpointer)GINT_TO_POINTER(i));
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, light_but[i], TRUE, TRUE, 0);
    g_free (str);
  }
  light_list = allocint(val);
  run_this_gtk_dialog (win, G_CALLBACK(run_light_source_to_be_removed), ogl_lightning);
  return light_list;
}

/*!
  \fn void print_light_source (Light source, int i)

  \brief print light source data

  \param source the light source
  \param i the light source id
*/
void print_light_source (Light source, int i)
{
  g_debug ("\n");
  g_debug ("Light N°%d", i);
  g_debug ("Type= %d", source.type);
  g_debug ("Pos_x= %f, Pos_y= %f, Pos_z= %f", source.position.x, source.position.y, source.position.z);
  g_debug ("Dir_x= %f, Dir_y= %f, Dir_z= %f", source.direction.x, source.direction.y, source.direction.z);
  g_debug ("Int_r= %f, Int_g= %f, Int_b= %f", source.intensity.x, source.intensity.y, source.intensity.z);
  g_debug ("Att_c= %f, Att_l= %f, Att_q= %f", source.attenuation.x, source.attenuation.y, source.attenuation.z);
  g_debug ("Spo_a= %f, Spo_i= %f, Spo_o= %f", source.spot_data.x, source.spot_data.y, source.spot_data.z);
  g_debug ("\n");
}

/*
  Light attenuation table (from Ogre3D):
  Distance(to object) 	Constant 	Linear 	Quadratic
        7                 1.0        0.7       1.8
       13                 1.0        0.35      0.44
       20                 1.0        0.22      0.20
       32                 1.0        0.14      0.07
       50                 1.0        0.09      0.032
       65                 1.0        0.07      0.017
      100                 1.0        0.045     0.0075
      160                 1.0        0.027     0.0028
      200                 1.0        0.022     0.0019
      325                 1.0        0.014     0.0007
      600                 1.0        0.007     0.0002
     3250                 1.0        0.0014    0.000007
*/

/*!
  \fn Light init_light_source (int type, float val, float vbl)

  \brief initialize a light source

  \param type the type of light
  \param size depth or max (a,b,c)
  \param depth field depth
  \param intens
*/
Light init_light_source (int type, float size, float depth)
{
  Light new_light;
  new_light.type = type;
  new_light.fix = (type != 1) ? 0 : 1;
  new_light.show = 0;
  new_light.direction = vec3(0.0, 0.0, 0.0);
  double intensity = (type == 1) ? 100.0*DEFAULT_INTENSITY : DEFAULT_INTENSITY;
  if (size != depth)
  {
    intensity *= exp (size/depth);
  }
  if (! preferences && depth <= 50.0) intensity *= depth / 100.0;
  new_light.intensity = vec3 (intensity, intensity, intensity);
  new_light.attenuation = vec3 (1.0, 0.14, 0.07);
  new_light.spot_data = vec3 (20.0, 20.0, 20.0);
  if (type == 0)
  {
    new_light.position = vec3 (0.0, 0.0, 0.0);
    new_light.direction = vec3 (0.0, 0.0, -1.0);
  }
  else
  {
    new_light.position  = vec3 (depth*1.5, 0.0, 0.0);
    if (type == 2)
    {
      new_light.intensity = v3_muls (new_light.intensity, 100.0);
      float tan = (size * sqrt(2.0) / 2.0) / (depth - size);
      float tetha = fabs(atanf (tan)) * 90.0 / pi;
      new_light.spot_data = vec3 (tetha, tetha, tetha);
    }
  }
  return new_light;
}

/*!
  \fn Light copy_light_source (Light old_sp)

  \brief create a copy of a light source

  \param old_sp the light source to copy
*/
Light copy_light_source (Light old_sp)
{
  Light new_sp;
  new_sp.type = old_sp.type;
  new_sp.fix = old_sp.fix;
  new_sp.show = old_sp.show;
  new_sp.position = old_sp.position;
  new_sp.direction = old_sp.direction;
  new_sp.intensity = old_sp.intensity;
  new_sp.attenuation = old_sp.attenuation;
  new_sp.spot_data = old_sp.spot_data;
  return new_sp;
}

/*!
  \fn Light * copy_light_sources (int dima, int dimb, Light * old_sp)

  \brief create a copy of a list of light sources

  \param dima new list size
  \param dimb old list size to duplicate
  \param old_sp old light sources
*/
Light * copy_light_sources (int dima, int dimb, Light * old_sp)
{
  int j;
  Light * new_sp = g_malloc (dima*sizeof * new_sp);
  for (j=0; j<dimb; j++)
  {
    //print_light_source (old_sp[j], j);
    new_sp[j] = copy_light_source (old_sp[j]);
    /*new_sp[j].type = old_sp[j].type;
    new_sp[j].fix = old_sp[j].fix;
    new_sp[j].show = old_sp[j].show;
    new_sp[j].position = old_sp[j].position;
    new_sp[j].direction = old_sp[j].direction;
    new_sp[j].intensity = old_sp[j].intensity;
    new_sp[j].attenuation = old_sp[j].attenuation;
    new_sp[j].spot_data = old_sp[j].spot_data;*/
    //print_light_source (new_sp[j], j);
  }
  return new_sp;
}

/*!
  \fn void show_active_light_data (opengl_edition * ogl_win, int lid, int tid)

  \brief show active light data

  \param ogl_win the target OpenGL edition window
  \param lid the light id
  \param tid the light type
*/
void show_active_light_data (opengl_edition * ogl_win, int lid, int tid)
{
  Light * this_light = (preferences) ? & tmp_lightning.spot[lid] : & get_project_by_id(ogl_win -> proj) -> modelgl -> anim -> last -> img -> l_ghtning.spot[lid];
  this_light -> type = tid;

  if (is_the_widget_visible(ogl_win -> advanced_light_box)) hide_the_widgets (ogl_win -> advanced_light_box);
  widget_set_sensitive (ogl_win -> light_type, lid);
  if (this_light -> type) show_the_widgets (ogl_win -> advanced_light_box);
  int i;
  for (i=0; i<2; i++)
  {
    if (is_the_widget_visible(ogl_win -> light_b_coord[i])) hide_the_widgets (ogl_win -> light_b_coord[i]);
    if (is_the_widget_visible(ogl_win -> light_b_entry[i])) hide_the_widgets (ogl_win -> light_b_entry[i]);
  }
  if (this_light -> type)
  {
    show_the_widgets (ogl_win -> light_b_coord[0]);
    show_the_widgets (ogl_win -> light_b_entry[0]);
  }
  if (this_light -> type == 0 || this_light -> type == 2)
  {
    show_the_widgets (ogl_win -> light_b_coord[1]);
  }
  if (this_light -> type == 2)
  {
    show_the_widgets (ogl_win -> light_b_entry[1]);
  }
  if (! preferences)
  {
    if (is_the_widget_visible(ogl_win -> light_show)) hide_the_widgets (ogl_win -> light_show);
    if (this_light -> type) show_the_widgets (ogl_win -> light_show);
  }
}

/*!
  \fn void update_light_data (int li, opengl_edition * ogl_win)

  \brief update light data

  \param li the light id
  \param ogl_win the target OpenGL edition window
*/
void update_light_data (int li, opengl_edition * ogl_win)
{
  Light * this_light;
  if (! preferences)
  {
    this_light = & get_project_by_id(ogl_win -> proj) -> modelgl -> anim -> last -> img -> l_ghtning.spot[li];
  }
  else
  {
    this_light = & tmp_lightning.spot[li];
  }
  combo_set_active (ogl_win -> light_type, this_light -> type);
  combo_set_active (ogl_win -> light_fix, this_light -> fix);
  show_active_light_data (ogl_win, li, this_light -> type);
  update_entry_double (GTK_ENTRY(ogl_win -> light_entry[0]), this_light -> attenuation.x);
  update_entry_double (GTK_ENTRY(ogl_win -> light_entry[1]), this_light -> attenuation.y);
  update_entry_double (GTK_ENTRY(ogl_win -> light_entry[2]), this_light -> attenuation.z);
  update_entry_double (GTK_ENTRY(ogl_win -> light_entry[3]), this_light -> spot_data.x);
  update_entry_double (GTK_ENTRY(ogl_win -> light_entry[4]), this_light -> spot_data.y);
  update_entry_double (GTK_ENTRY(ogl_win -> light_entry[5]), this_light -> spot_data.z);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[1][0]), this_light -> position.x);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[1][1]), this_light -> position.y);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[1][2]), this_light -> position.z);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[2][0]), this_light -> direction.x);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[2][1]), this_light -> direction.y);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[2][2]), this_light -> direction.z);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[3][0]), this_light -> intensity.x);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[3][1]), this_light -> intensity.y);
  update_entry_double (GTK_ENTRY(ogl_win -> entogl[3][2]), this_light -> intensity.z);
  if (! preferences)
  {
    button_set_status (ogl_win -> light_show, this_light -> show);
  }
}

/*!
  \fn G_MODULE_EXPORT void show_light_param (GtkComboBox * box, gpointer data)

  \brief update light parameters based on light id in combo box

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_light_param (GtkComboBox * box, gpointer data)
{
  int li = combo_get_active ((GtkWidget *)box);
  update_light_data (li, (opengl_edition *)data);
}

/*!
  \fn void create_lights_combo (int num_lights, opengl_edition * ogl_win)

  \brief create light combo box

  \param num_lights the target number of lights
  \param ogl_win the target OpenGL edition window
*/
void create_lights_combo (int num_lights, opengl_edition * ogl_win)
{
  ogl_win -> lights = create_combo ();
  int i;
  gchar * str;
  for (i=0; i<num_lights; i++)
  {
    str = g_strdup_printf ("Light N°%d", i+1);
    combo_text_append (ogl_win -> lights, str);
    g_free (str);
  }
  gtk_widget_set_size_request (ogl_win -> lights, 100, -1);
  g_signal_connect (G_OBJECT (ogl_win -> lights), "changed", G_CALLBACK(show_light_param), ogl_win);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ogl_win -> lights_box, ogl_win -> lights, FALSE, FALSE, 10);
}

/*!
  \fn void add_remove_lights (int val, gpointer data)

  \brief add or remove lights

  \param val total number of light(s)
  \param data the associated data pointer
*/
void add_remove_lights (int val, gpointer data)
{
  int i, j, k, m;
  project * this_proj;
  glwin * view;
  Lightning * this_lightning;
  opengl_edition * ogl_edit;
  if (! preferences)
  {
    view = (glwin *)data;
    this_proj = get_project_by_id (view -> proj);
    this_lightning = & view -> anim -> last -> img -> l_ghtning;
    ogl_edit = view -> opengl_win;
  }
  else
  {
    this_lightning = & tmp_lightning;
    ogl_edit = pref_ogl_edit;
  }

  gboolean delete_ligth;
  i = this_lightning -> lights;
  Light * old_spots;
  if (val > i)
  {
#ifdef DEBUG
    g_debug ("ADDING_LIGHT_SOURCE:: val= %d, i= %d", val, i);
#endif
    // Adding light source(s)
    old_spots = copy_light_sources (i, i, this_lightning -> spot);
    g_free (this_lightning -> spot);
    this_lightning -> spot = copy_light_sources (val, i, old_spots);
    this_lightning -> lights = val;
    float pos = (preferences) ? 20.0 : (this_proj -> cell.box[0].param[0][0] == 0.0) ? 20.0 : this_proj -> cell.box[0].param[0][0];
    for (j=i; j<val; j++)
    {
      this_lightning -> spot[j] = init_light_source (0, pos, (! preferences) ? this_proj -> modelgl -> p_moy : 1.0); // Init directional by default
    }
    //free (old_spots);
  }
  else if (val < i)
  {
    // We need to remove a light
#ifdef DEBUG
    g_debug ("REMOVING_LIGHT_SOURCE:: val= %d, i= %d", val, i);
#endif
    int * ltr = light_source_to_be_removed (i-val, this_lightning, ogl_edit);
    if (ltr != NULL)
    {
      old_spots = copy_light_sources (i, i, this_lightning -> spot);
      for (k=0; k < i-val; k++)
      {
#ifdef DEBUG
        g_debug ("REMOVING_LIGHT_SOURCES:: k= %d, ltr[%d]= %d", k, k, ltr[k]);
#endif
      }
      g_free (this_lightning -> spot);
      this_lightning -> spot = g_malloc (val*sizeof*this_lightning -> spot);
      m = -1;
      for (j=0; j<i; j++)
      {
        delete_ligth = FALSE;
        for (k=0; k< i-val; k++)
        {
          if (j == ltr[k]) delete_ligth = TRUE;
        }
        if (! delete_ligth)
        {
          m ++;
          this_lightning -> spot[m] = copy_light_source (old_spots[j]);
        }
      }
      g_free (old_spots);
      this_lightning -> lights = val;
#ifdef DEBUG
      g_debug ("LIGHT(s) HAVE BEEN REMOVED:: NEW_LIGHTS_NUM= %d", val);
#endif
    }
  }
  create_lights_combo (this_lightning -> lights, ogl_edit);
  ogl_edit -> lights = destroy_this_widget (ogl_edit -> lights);
  show_the_widgets (ogl_edit -> lights);
  combo_set_active (ogl_edit -> lights, 0);
  update_light_data (0, ogl_edit);
  if (! preferences)
  {
    view -> create_shaders[LIGHT] = TRUE;
    update (view);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_nlights_spin (GtkSpinButton * res, gpointer data)

  \brief  change the number of light(s) - spin button

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_nlights_spin (GtkSpinButton * res, gpointer data)
{
  add_remove_lights (gtk_spin_button_get_value_as_int(res), data);
}

/*!
  \fn G_MODULE_EXPORT void set_nlights (GtkEntry * res, gpointer data)

  \brief change the number of light(s) - entry

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_nlights (GtkEntry * res, gpointer data)
{
  int i;
  const gchar * m;
  m = entry_get_text (res);
  i = (int) string_to_double ((gpointer)m);
  add_remove_lights (i, data);
}

/*!
  \fn G_MODULE_EXPORT void update_light_param (GtkEntry * res, gpointer data)

  \brief update light parameter

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_light_param (GtkEntry * res, gpointer data)
{
  dint  * lid = (dint *)data;
  glwin * view = (preferences) ? NULL : get_project_by_id(lid -> a) -> modelgl;
  opengl_edition * ogl_edit = (preferences) ? pref_ogl_edit : (view) ? view -> opengl_win : NULL;
  if (ogl_edit)
  {
    int li = combo_get_active (ogl_edit-> lights);
    Light * this_light = (preferences) ? & tmp_lightning.spot[li] : & view -> anim -> last -> img -> l_ghtning.spot[li];
    const gchar * m = entry_get_text (res);
    double v = string_to_double ((gpointer)m);
    switch (lid -> b)
    {
      case 0:
        this_light -> attenuation.x = v;
        break;
      case 1:
        this_light -> attenuation.y = v;
        break;
      case 2:
        this_light -> attenuation.z = v;
        break;
      case 3:
        this_light -> spot_data.x = v;
        break;
      case 4:
        this_light -> spot_data.y = v;
        break;
      case 5:
        this_light -> spot_data.z = v;
        break;
    }
    update_entry_double (res, v);
    if (this_light -> show) view -> create_shaders[LIGHT] = TRUE;
  }
  if (! preferences) update (view);
}

/*!
  \fn void set_data_pos (vec3_t * vect, int pos, double v)

  \brief modify a vector component

  \param vect vector to adjust
  \param pos position to adjust
  \param v new value
*/
void set_data_pos (vec3_t * vect, int pos, double v)
{
  switch (pos)
  {
    case 0:
      vect -> x = v;
      break;
    case 1:
      vect -> y = v;
      break;
    case 2:
      vect -> z = v;
      break;
  }
}

/*!
  \fn G_MODULE_EXPORT void set_object_pos (GtkEntry * res, gpointer data)

  \brief set object position

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_object_pos (GtkEntry * res, gpointer data)
{
  tint * id = (tint *)data;
  glwin * view;
  opengl_edition * edit_ogl;
  Material * the_mat;
  Lightning * the_lightning;
  Fog * the_fog;
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  if (! preferences)
  {
    view = get_project_by_id(id -> a) -> modelgl;
    the_mat = & view -> anim -> last -> img -> m_terial;
    the_lightning = & view -> anim -> last -> img -> l_ghtning;
    the_fog = & view -> anim -> last -> img -> f_g;
    edit_ogl = view -> opengl_win;
  }
  else
  {
    the_mat = & tmp_material;
    the_lightning = & tmp_lightning;
    the_fog = & tmp_fog;
    edit_ogl = pref_ogl_edit;
  }
  if (id -> b == 0)
  {
    set_data_pos (& the_mat -> albedo, id -> c, v);
  }
  else if (id -> b == 4)
  {
    set_data_pos (& the_fog -> color, id -> c, v);
  }
  else if (id -> b > 0 && edit_ogl)
  {
    int li = combo_get_active (edit_ogl -> lights);
    switch (id -> b)
    {
      case 1:
        set_data_pos (& the_lightning -> spot[li].position, id -> c, v);
        break;
      case 2:
        set_data_pos (& the_lightning -> spot[li].direction, id -> c, v);
        break;
      case 3:
        set_data_pos (& the_lightning -> spot[li].intensity, id -> c, v);
        break;
    }
    if (the_lightning -> spot[li].show && ! preferences) view -> create_shaders[LIGHT] = TRUE;
  }
  if (edit_ogl) update_entry_double (res, v);
  if (! preferences) update (view);
}

/*!
  \fn G_MODULE_EXPORT void set_light_type (GtkComboBox * box, gpointer data)

  \brief set light type callback

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_light_type (GtkComboBox * box, gpointer data)
{
  opengl_edition * ogl_win = (opengl_edition *)data;
  int li = combo_get_active (ogl_win -> lights);
  int ti = combo_get_active ((GtkWidget *)box);
  show_active_light_data (ogl_win, li, ti);
}

/*!
  \fn G_MODULE_EXPORT void set_light_fix (GtkComboBox * box, gpointer data)

  \brief set light fix callback

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_light_fix (GtkComboBox * box, gpointer data)
{
  opengl_edition * ogl_win = (opengl_edition *)data;
  glwin * view;
  Lightning * ogl_ligthning;
  if (! preferences)
  {
    view = get_project_by_id(ogl_win -> proj) -> modelgl;
    ogl_ligthning = & view -> anim -> last -> img -> l_ghtning;
  }
  else
  {
    ogl_ligthning = & tmp_lightning;
  }
  int li = combo_get_active (ogl_win -> lights);
  ogl_ligthning -> spot[li].fix = combo_get_active ((GtkWidget *)box);
  if (! preferences)
  {
    view -> create_shaders[LIGHT] = TRUE;
    update (view);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void show_this_light (GtkCheckButton * but, gpointer data)

  \brief show / hide this light callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_this_light (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void show_this_light (GtkToggleButton * but, gpointer data)

  \brief show / hide this light callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_this_light (GtkToggleButton * but, gpointer data)
#endif
{
  opengl_edition * ogl_win = (opengl_edition *)data;
  glwin * view = get_project_by_id(ogl_win -> proj) -> modelgl;
  int li = combo_get_active (ogl_win -> lights);
  view -> anim -> last -> img -> l_ghtning.spot[li].show = button_get_status ((GtkWidget *)but);
  view -> create_shaders[LIGHT] = TRUE;
  update (view);
}

/*!
  \fn GtkWidget * create_setting_pos (gchar * lab, int size, float xalign, int pid, int lid, float * values, opengl_edition * ogl_win)

  \brief create OpenGL setting entries table

  \param lab label to display
  \param size size of the label
  \param xalign x alignement
  \param pid parameter id (0 = material, 1 = light direction, 2 = light position, 3 = light intensity, 4 = fog)
  \param lid parameter label id
  \param values target parameter values
  \param ogl_win the target OpenGL edition window
*/
GtkWidget * create_setting_pos (gchar * lab, int size, float xalign, int pid, int lid, float * values, opengl_edition * ogl_win)
{
  int i;
  GtkWidget * setting_pos = create_vbox (BSEP);
  adv_box (setting_pos, lab, 5, size, xalign);
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, setting_pos, hbox, FALSE, FALSE, 0);
  GtkWidget * sbox = create_hbox (0);
  gtk_widget_set_size_request (sbox, 500, -1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, sbox, FALSE, FALSE, 70);
  GtkWidget * label;
  for (i=0; i<3; i++)
  {
    ogl_win -> pos_pointer[pid][i].a = ogl_win -> proj;
    ogl_win -> pos_pointer[pid][i].b = pid;
    ogl_win -> pos_pointer[pid][i].c = i;
    label = markup_label ((pid > 0 && pid < 3) ? lpos[i] : cpos[i], 20, -1, 1.0, 0.5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, sbox, label, FALSE, FALSE, 5);
    ogl_win -> entogl[pid][i] = create_entry (G_CALLBACK(set_object_pos), 80, 10, FALSE, & ogl_win -> pos_pointer[pid][i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, sbox, ogl_win -> entogl[pid][i], FALSE, FALSE, 0);
    update_entry_double (GTK_ENTRY(ogl_win -> entogl[pid][i]), values[i]);
  }
  if (pid == 1 && ! preferences)
  {
    ogl_win -> light_show = check_button ("Show light", -1, -1, FALSE, G_CALLBACK(show_this_light), ogl_win);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, sbox, ogl_win -> light_show, FALSE, FALSE, 10);
  }
  return setting_pos;
}

/*!
  \fn GtkWidget * lights_tab (glwin * view, opengl_edition * ogl_edit, Lightning * the_lightning)

  \brief OpenGL light(s) parameters tab

  \param view the target glwin
  \param ogl_edit the target OpenGL edition window
  \param ogl_lightning the target lightning data structure
*/
GtkWidget * lights_tab (glwin * view, opengl_edition * ogl_edit, Lightning * ogl_lightning)
{
  int i, j, k;
  GtkWidget * vbox;
  GtkWidget * hbox, * lhbox;

  GtkWidget * layout = create_layout (-1, 600);
  vbox = add_vbox_to_layout (layout, 480, -1);
  hbox = bdv_box (vbox, "<b>Number of light sources</b>\n(add or remove lights - up to 10 sources)", 250, 0.0);
  gtk_widget_set_size_request (hbox, -1, 65);

  GtkWidget * nlights = spin_button (G_CALLBACK(set_nlights_spin), ogl_lightning -> lights, 1.0, 10.0, 1.0, 0, 100, view);
  gtk_widget_set_size_request (nlights, 25, -1);
  GtkWidget * fix = gtk_fixed_new ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fix, FALSE, FALSE, 20);
  gtk_fixed_put (GTK_FIXED (fix), nlights, 0, 20);

  hbox = bdv_box (vbox, "<b>Configure light source <sup>*</sup></b>", 250, 0.0);
  ogl_edit -> lights_box = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ogl_edit -> lights_box, FALSE, FALSE, 10);
  create_lights_combo (ogl_lightning -> lights, ogl_edit);

  bdv_box (vbox, "<b>Light configuration</b>", 250, 0.0);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  ogl_edit -> light_type = create_combo ();
  gchar * ltype[3] = {"Directional", "Point", "Spot"};
  for (i=0; i<3; i++)
  {
    combo_text_append (ogl_edit -> light_type, ltype[i]);
  }
  g_signal_connect (G_OBJECT (ogl_edit -> light_type), "changed", G_CALLBACK(set_light_type), ogl_edit);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ogl_edit -> light_type, FALSE, FALSE, 60);

  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, gtk_label_new("Fixed by respect to: "), FALSE, FALSE, 10);
  ogl_edit -> light_fix = create_combo();
  gchar * lfix[2] = {"The viewer", "The model"};
  for (i=0; i<2; i++)
  {
    combo_text_append (ogl_edit -> light_fix, lfix[i]);
  }
  g_signal_connect (G_OBJECT (ogl_edit -> light_fix), "changed", G_CALLBACK(set_light_fix), ogl_edit);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ogl_edit -> light_fix, FALSE, FALSE, 10);

  float values[3] = {0.0, 0.0, 0.0};
  // Position
  gchar * str = (preferences) ? g_strdup_printf ("%s <sup>**</sup>", ogl_settings[1][0]) : g_strdup_printf ("%s", ogl_settings[1][0]);
  ogl_edit -> light_b_coord[0] = create_setting_pos (str, 130, 0.0, 1, 0, values, ogl_edit);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, ogl_edit -> light_b_coord[0], FALSE, FALSE, 0);
  // Direction
  ogl_edit -> light_b_coord[1] = create_setting_pos (ogl_settings[1][1], 130, 0.0, 2, 1, values, ogl_edit);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, ogl_edit -> light_b_coord[1], FALSE, FALSE, 0);
  // Intensity
  str = (preferences) ? g_strdup_printf ("%s <sup>**</sup>", ogl_settings[1][2]) : g_strdup_printf ("%s", ogl_settings[1][2]);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, create_setting_pos (str, 130, 0.0, 3, 2, values, ogl_edit), FALSE, FALSE, 0);
  g_free (str);
  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  ogl_edit -> advanced_light_box = create_vbox (BSEP);
  add_box_child_start ( GTK_ORIENTATION_HORIZONTAL, hbox, ogl_edit -> advanced_light_box, FALSE, FALSE, 40);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ogl_edit -> advanced_light_box, markup_label("<b>Advanced parameters</b>", -1, -1, 0.1, 0.5), FALSE, FALSE, 10);
  GtkWidget * lbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ogl_edit -> advanced_light_box, lbox, FALSE, FALSE, 0);
  GtkWidget * lvbox = create_vbox (BSEP);
  add_box_child_start ( GTK_ORIENTATION_HORIZONTAL, lbox, lvbox, FALSE, FALSE, 20);
  k = 0;
  for (i=0; i<2; i++)
  {
    ogl_edit -> light_b_entry[i] = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, lvbox, ogl_edit -> light_b_entry[i], FALSE, FALSE, 0);
    for (j=0; j<3; j++)
    {
      ogl_edit -> light_entry[k] = create_entry (G_CALLBACK(update_light_param), 100, 15, FALSE, & ogl_edit -> pointer[k]);
      lhbox = adv_box (ogl_edit -> light_b_entry[i], ogl_settings[1][k+3], 0, 170, 0.0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, lhbox, ogl_edit -> light_entry[k], FALSE, FALSE, 10);
      if (i == 1) add_box_child_start (GTK_ORIENTATION_HORIZONTAL, lhbox, gtk_label_new("°"), FALSE, FALSE, 0);
      k ++;
    }
  }
  show_the_widgets (layout);
  combo_set_active (ogl_edit -> lights, 0);
  update_light_data (0, ogl_edit);

  append_comments (vbox, "<sup>*</sup>", "Note that light N°1 must be a directional light");
  if (preferences) append_comments (vbox, "<sup>**</sup>", "Intensity and position will be corrected based on model depth");

  return layout;
}

// ***************** MATERIAL ******************* //

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_use_template_toggle (GtkCheckButton * but, gpointer data)

  \brief use or not OpenGL material template callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_use_template_toggle (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_use_template_toggle (GtkToggleButton * but, gpointer data)

  \brief use or not OpenGL material template callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_use_template_toggle (GtkToggleButton * but, gpointer data)
#endif
{
  int i, j, k;
  Material * the_mat;
  opengl_edition * ogl_edit;
  glwin * view;
  if (! preferences)
  {
    view = (glwin *)data;
    ogl_edit = view -> opengl_win;
    the_mat = & view -> anim -> last -> img -> m_terial;
    i = combo_get_active (view -> opengl_win -> templates);
  }
  else
  {
    ogl_edit = pref_ogl_edit;
    the_mat = & tmp_material;
    i = combo_get_active (pref_ogl_edit -> templates);
  }
  j = button_get_status ((GtkWidget *)but);
  if (j)
  {
    if (i == -1) i = 3;
    the_mat -> albedo = vec3(0.5, 0.5, 0.5);
    for (k=0; k<5; k++)
    {
      the_mat -> param[k+1] = template_parameters[i][k];
      if (ogl_edit)
      {
        update_entry_double (GTK_ENTRY(ogl_edit -> m_entry[k]), the_mat -> param[k+1]);
        gtk_range_set_value (GTK_RANGE(ogl_edit -> m_scale[k]), the_mat -> param[k+1]);
      }
    }
    combo_set_active (ogl_edit -> templates, i);
    if (ogl_edit)
    {
      update_entry_double (GTK_ENTRY(ogl_edit -> entogl[0][0]), the_mat -> albedo.x);
      update_entry_double (GTK_ENTRY(ogl_edit -> entogl[0][1]), the_mat -> albedo.y);
      update_entry_double (GTK_ENTRY(ogl_edit -> entogl[0][2]), the_mat -> albedo.z);
    }
    k = i+1;
  }
  else
  {
    k = 0;
  }
  the_mat -> predefine = k;
  widget_set_sensitive (ogl_edit -> templates, the_mat -> predefine);
  widget_set_sensitive (ogl_edit -> param_mat, ! the_mat -> predefine);
  if (! preferences)
  {
    update (view);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_template (GtkComboBox * box, gpointer data)

  \brief change the OpenGL material template

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_template (GtkComboBox * box, gpointer data)
{
  int i, j;
  Material * the_mat;
  glwin * view;
  opengl_edition * ogl_edit;
  if (preferences)
  {
    the_mat = & tmp_material;
    ogl_edit = pref_ogl_edit;
  }
  else
  {
    view = (glwin *)data;
    the_mat = & view -> anim -> last -> img -> m_terial;
    ogl_edit = view -> opengl_win;
  }
  i = combo_get_active ((GtkWidget *)box);
  the_mat -> predefine = i + 1;
  for (j=0; j<5; j++)
  {
    the_mat -> param[j+1] = template_parameters[i][j];
#ifdef DEBUG
    g_debug ("SET_TEMPLATES:: j= %d, val= %f", j, template_parameters[i][j]);
#endif
   if (ogl_edit)
   {
     update_entry_double (GTK_ENTRY(ogl_edit -> m_entry[j]), the_mat -> param[j+1]);
     gtk_range_set_value (GTK_RANGE(ogl_edit -> m_scale[j]), the_mat -> param[j+1]);
   }
  }
  if (ogl_edit)
  {
    update_entry_double (GTK_ENTRY(ogl_edit -> entogl[0][0]), the_mat -> albedo.x);
    update_entry_double (GTK_ENTRY(ogl_edit -> entogl[0][1]), the_mat -> albedo.y);
    update_entry_double (GTK_ENTRY(ogl_edit -> entogl[0][2]), the_mat -> albedo.z);
  }
  if (! preferences) update (view);
}

/*!
  \fn G_MODULE_EXPORT void set_l_model (GtkComboBox * box, gpointer data)

  \brief change OpenGL lightning model

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_l_model (GtkComboBox * box, gpointer data)
{
  if (preferences)
  {
    tmp_material.param[0] = combo_get_active ((GtkWidget *)box);
  }
  else
  {
    glwin * view = (glwin *)data;
    view -> anim -> last -> img -> m_terial.param[0] = combo_get_active ((GtkWidget *)box);
    update (view);
  }
}

/*!
  \fn void param_has_changed (gpointer data, double val)

  \brief update OpenGL material parameter

  \param data the associated data pointer
  \param val the new value
*/
void param_has_changed (gpointer data, double val)
{
  dint * mid = (dint *)data;
  opengl_edition * edit_ogl;
  Material * the_mat;
  glwin * view;
  if (preferences)
  {
    edit_ogl = pref_ogl_edit;
    the_mat = & tmp_material;
  }
  else
  {
    view = get_project_by_id(mid -> a) -> modelgl;
    edit_ogl = view -> opengl_win;
    the_mat = & view -> anim -> last -> img -> m_terial;
  }
  if (mat_min_max[mid -> b][0] >= 0.0 && val <= mat_min_max[mid -> b][1]) the_mat -> param[mid -> b + 1] = val;
  if (edit_ogl)
  {
    update_entry_double (GTK_ENTRY(edit_ogl -> m_entry[mid -> b]), the_mat -> param[mid -> b + 1]);
    gtk_range_set_value (GTK_RANGE(edit_ogl -> m_scale[mid -> b]), the_mat -> param[mid -> b + 1]);
  }
  if (! preferences) update (view);
}

/*!
  \fn G_MODULE_EXPORT void update_mat_param (GtkEntry * res, gpointer data)

  \brief update OpenGL material parameter - entry

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_mat_param (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  param_has_changed (data, v);
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_scale_param (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief update OpenGL material parameter - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_scale_param (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  param_has_changed (data, value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void scale_param (GtkRange * range, gpointer data)

  \brief update OpenGL material parameter - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void scale_param (GtkRange * range, gpointer data)
{
  param_has_changed (data, gtk_range_get_value (range));
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_scale_quality (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief update OpenGL quality - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_scale_quality (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  set_quality ((int)value, data);
#ifdef GTK4
  if (! preferences) update_menu_bar ((glwin *)data);
#endif
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void scale_quality (GtkRange * range, gpointer data)

  \brief update OpenGL quality - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void scale_quality (GtkRange * range, gpointer data)
{
  set_quality ((int)gtk_range_get_value (range), data);
#ifdef GTK4
  if (! preferences) update_menu_bar ((glwin *)data);
#endif
}

/*!
  \fn GtkWidget * lightning_box (glwin * view, Material this_material)

  \brief create ligthning model combo box

  \param view the target glwin, if any
  \param this_material the target material, if any
*/
GtkWidget * lightning_fix (glwin * view, Material * this_material)
{
  GtkWidget * fix = gtk_fixed_new ();
  GtkWidget * lmodel = create_combo ();
  gtk_fixed_put (GTK_FIXED (fix), lmodel, 0, 10);

  char * l_model[6] = {"None", "Phong", "Blinn", "Cook-Torrance-Blinn", "Cook-Torrance-Beckmann", "Cook-Torrance-GCX"};
  int i;
  for (i=0; i<6; i++)
  {
    combo_text_append (lmodel, l_model[i]);
  }
  g_signal_connect (G_OBJECT (lmodel), "changed", G_CALLBACK(set_l_model), view);
  gtk_widget_set_size_request (lmodel, 200, -1);
  combo_set_active (lmodel, this_material -> param[0]);
  return fix;
}

/*!
  \fn GtkWidget * materials_tab (glwin * view, opengl_edition * ogl_edit, Material * the_mat)

  \brief OpenGL material parameters tab

  \param view the target glwin
  \param ogl_edit the target OpenGL edition window
  \param the_mat the target material data structure
*/
GtkWidget * materials_tab (glwin * view, opengl_edition * ogl_edit, Material * the_mat)
{
  GtkWidget * layout = create_layout (-1, 300);
  GtkWidget * vbox = add_vbox_to_layout (layout, 650, -1);
  int i;
  GtkWidget * box, * hbox;

  box = adv_box (vbox, "<b>Quality</b> ", 5, 150, 0.0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, create_hscale (2, 500, 1, (view) ? view -> anim -> last -> img -> quality : tmp_opengl[3], GTK_POS_TOP, 1, 200,
                       G_CALLBACK(scale_quality), G_CALLBACK(scroll_scale_quality), view), FALSE, FALSE, 0);
  box = adv_box (vbox, "<b>Lightning model</b> ", 5, 150, 0.0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, lightning_fix (view, the_mat), FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 20);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox,
                       check_button ("<b>Use template</b>", 100, 40, the_mat -> predefine, G_CALLBACK(set_use_template_toggle), view),
                       FALSE, FALSE, 0);

  hbox = create_hbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, hbox, markup_label ("<b>Templates</b>", 150, -1, 0.0, 0.5), FALSE, FALSE, 50);
  ogl_edit -> templates  = create_combo ();
  for (i=0; i<TEMPLATES; i++)
  {
    combo_text_append (ogl_edit -> templates, material_template[i]);
  }
  combo_set_active (ogl_edit -> templates, the_mat -> predefine-1);
  g_signal_connect (G_OBJECT (ogl_edit -> templates), "changed", G_CALLBACK(set_template), view);
  gtk_widget_set_size_request (ogl_edit -> templates, 100, -1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ogl_edit -> templates, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 20);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  ogl_edit -> param_mat = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, ogl_edit -> param_mat, FALSE, FALSE, 50);

  ogl_edit -> param_mat = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, ogl_edit -> param_mat, FALSE, FALSE, 0);

  bdv_box (ogl_edit -> param_mat, "<b>Material properties</b>", 250, -1);

  GtkWidget * m_fixed;
  for (i=0; i<5; i++)
  {
    box = adv_box (ogl_edit -> param_mat, ogl_settings[0][i+1], 0, 130, 0.0);
    ogl_edit -> m_scale[i] =  create_hscale (mat_min_max[i][0], mat_min_max[i][1], 0.001, the_mat -> param[i+1],
                                             GTK_POS_TOP, 3, 200, G_CALLBACK(scale_param), G_CALLBACK(scroll_scale_param), & ogl_edit -> pointer[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, ogl_edit -> m_scale[i], FALSE, FALSE, 10);
    ogl_edit -> m_entry[i] = create_entry (G_CALLBACK(update_mat_param), 100, 15, FALSE, & ogl_edit -> pointer[i]);
    update_entry_double(GTK_ENTRY(ogl_edit -> m_entry[i]), the_mat -> param[i+1]);
    m_fixed = gtk_fixed_new ();
    gtk_fixed_put (GTK_FIXED(m_fixed), ogl_edit -> m_entry[i], 0, 15);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, m_fixed, FALSE, FALSE, 15);
  }
  float values[] = {the_mat -> albedo.x, the_mat -> albedo.y, the_mat -> albedo.z};

  add_box_child_start (GTK_ORIENTATION_VERTICAL, ogl_edit -> param_mat, create_setting_pos (ogl_settings[0][0], 130, -1, 0, 0, values, ogl_edit), FALSE, FALSE, 20);
  show_the_widgets (layout);
  widget_set_sensitive (ogl_edit -> templates, the_mat -> predefine);
  widget_set_sensitive (ogl_edit -> param_mat, ! the_mat -> predefine);
  return layout;
}

// ***************** FOG ******************* //

/*!
  \fn void fog_param_changed (gpointer data, GLfloat u, GtkRange * range)

  \brief update OpenGL fog parameter

  \param data the associated data pointer
  \param u the new value
  \param range the GtkRange to udapte if needed
*/
void fog_param_changed (gpointer data, GLfloat u, GtkRange * range)
{
  dint * fid = (dint *)data;
  Fog * this_fog;
  opengl_edition * ogl_edit;
  glwin * view;
  if (! preferences)
  {
    view = get_project_by_id (fid -> a) -> modelgl;
    this_fog = & view -> anim -> last -> img -> f_g;
    ogl_edit = view -> opengl_win;
  }
  else
  {
    this_fog = & tmp_fog;
    ogl_edit = pref_ogl_edit;
  }
  GLfloat v, w;
  if (fid -> b > 0)
  {
    v = this_fog -> depth[0];
    w = this_fog -> depth[1];
    if (fid -> b == 1 && u < w)
    {
      this_fog -> depth[0] = u;
    }
    else if (fid -> b == 2 && u > v)
    {
      this_fog -> depth[1] = u;
    }
    else if (ogl_edit)
    {
      if (fid -> b == 1)
      {
        gtk_range_set_value (range, (gdouble) (w-0.1));
      }
      else
      {
        gtk_range_set_value (range, (gdouble) (v+0.1));
      }
    }
  }
  else
  {
    this_fog -> density = u;
  }
  if (! preferences) update (view);
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_fog_param (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief update OpenGL fog parameter - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_fog_param (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  fog_param_changed (data, (GLfloat) value, range);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_fog_param (GtkRange * range, gpointer data)

  \brief update OpenGL fog parameter - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_fog_param (GtkRange * range, gpointer data)
{
  fog_param_changed (data, (GLfloat) gtk_range_get_value (range), range);
}

/*!
  \fn G_MODULE_EXPORT void set_fog_type (GtkWidget * widg, gpointer data)

  \brief set OpenGL fog type

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_fog_type (GtkWidget * widg, gpointer data)
{
  opengl_edition * ogl_edit = (opengl_edition *)data;
  Fog * this_fog;
  glwin * view;
  if (! preferences)
  {
    view = get_project_by_id (ogl_edit -> proj) -> modelgl;
    this_fog = & view -> anim -> last -> img -> f_g;
  }
  else
  {
    this_fog = & tmp_fog;
  }
  this_fog -> based = combo_get_active (widg);
  if (! preferences) update (view);
}

/*!
  \fn void setup_fog_dialogs (opengl_edition * ogl_edit, int fid)

  \brief update OpenGL fog tab based of fog type

  \param ogl_edit the target opengl_edition
  \param fid the fog mode
*/
void setup_fog_dialogs (opengl_edition * ogl_edit, int fid)
{
  Fog * this_fog;
  glwin * view;
  if (! preferences)
  {
    view = get_project_by_id (ogl_edit -> proj) -> modelgl;
    this_fog = & view -> anim -> last -> img -> f_g;
  }
  else
  {
    this_fog = & tmp_fog;
  }
  this_fog -> mode = fid;
  if (this_fog -> mode)
  {
    show_the_widgets (ogl_edit -> param_fog);
    if (this_fog -> mode == 1)
    {
      show_the_widgets (ogl_edit -> depth_box);
      hide_the_widgets (ogl_edit -> dens_box);
    }
    else
    {
      hide_the_widgets (ogl_edit -> depth_box);
      show_the_widgets (ogl_edit -> dens_box);
    }
  }
  else
  {
    hide_the_widgets (ogl_edit -> param_fog);
  }
  if (! preferences) update (view);
}

/*!
  \fn G_MODULE_EXPORT void set_fog_mode (GtkWidget * widg, gpointer data)

  \brief set fog mode callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_fog_mode (GtkWidget * widg, gpointer data)
{
  setup_fog_dialogs ((opengl_edition *)data, combo_get_active (widg));
}

/*!
  \fn GtkWidget * fog_tab (glwin * view, opengl_edition * ogl_edit, Fog * the_fog)

  \brief OpenGL fog parameters tab

  \param view the target glwin
  \param ogl_edit the target OpenGL edition window
  \param the_fog the target fog data structure
*/
GtkWidget * fog_tab (glwin * view, opengl_edition * ogl_edit, Fog * the_fog)
{
  GtkWidget * layout = create_layout (480, -1);
  GtkWidget * vbox = add_vbox_to_layout (layout, 480, -1);

  GtkWidget * box = adv_box (vbox, "<b>Fog mode</b> ", 10, 150, 0.0);
  GtkWidget * fogmod = create_combo ();
  combo_text_append (fogmod, "None");
  combo_text_append (fogmod, "Linear");
  combo_text_append (fogmod, "Exponential");
  combo_text_append (fogmod, "Exponential squared");
  gtk_widget_set_size_request (fogmod, 200, -1);
  combo_set_active (fogmod, the_fog -> mode);
  g_signal_connect (G_OBJECT (fogmod), "changed", G_CALLBACK(set_fog_mode), ogl_edit);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, fogmod, FALSE, FALSE, 0);

  ogl_edit -> param_fog = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, ogl_edit -> param_fog, FALSE, FALSE, 0);

  box = adv_box (ogl_edit -> param_fog, "<b>Fog type</b> ", 5, 150, 0.0);
  GtkWidget * fogtype = create_combo ();
  combo_text_append (fogtype, "Plane based");
  combo_text_append (fogtype, "Range based");
  gtk_widget_set_size_request (fogtype, 200, -1);
  combo_set_active (fogtype, the_fog -> based);
  g_signal_connect (G_OBJECT (fogtype), "changed", G_CALLBACK(set_fog_type), ogl_edit);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, fogtype, FALSE, FALSE, 0);

  ogl_edit -> dens_box =  adv_box (ogl_edit -> param_fog, "<b>Fog density</b>", 10, 150.0, 0.0);
  ogl_edit -> fog_range[0] = create_hscale (0.0, 1.0, 0.01, the_fog -> density, GTK_POS_TOP, 3,
                                            250, G_CALLBACK(set_fog_param), G_CALLBACK(scroll_set_fog_param), & ogl_edit -> pointer[0]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ogl_edit -> dens_box, ogl_edit -> fog_range[0], FALSE, FALSE, 0);

  char * depthfog[2] = {"\t depth<sup>*</sup> start: ", "\t depth<sup>*</sup> end: "};

  ogl_edit -> depth_box = create_vbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ogl_edit -> param_fog, ogl_edit -> depth_box, FALSE, FALSE, 0);
  box = adv_box (ogl_edit -> depth_box, "<b>Fog depth</b>", 10, 150.0, 0.0);
  int i;
  for (i=0; i<2; i++)
  {
    box = adv_box (ogl_edit -> depth_box, depthfog[i], 5, 170, 0.0);
    ogl_edit -> fog_range[i+1] = create_hscale (0.0, 100.0, 0.1, the_fog -> depth[i], GTK_POS_TOP, 2,
                                                250, G_CALLBACK(set_fog_param), G_CALLBACK(scroll_set_fog_param), & ogl_edit -> pointer[i+1]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, ogl_edit -> fog_range[i+1], FALSE, FALSE, 0);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ogl_edit -> depth_box, markup_label("* % of the OpenGL model depth.", -1, -1, 0.5, 0.5) , FALSE, FALSE, 5);

  float values[] = {the_fog -> color.x, the_fog -> color.y, the_fog -> color.z};
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ogl_edit -> param_fog, create_setting_pos (ogl_settings[2][0], 130, 0.0, 4, 0, values, ogl_edit), FALSE, FALSE, 5);
  show_the_widgets (layout);
  return layout;
}

/*!
  \fn void close_advanced_opengl (gpointer data)

  \brief close OpenGL rendering window free data

  \param data the associated data pointer
*/
void close_advanced_opengl (gpointer data)
{
  glwin * view = (glwin *)data;
  view -> opengl_win -> win = destroy_this_widget (view -> opengl_win -> win);
  g_free (view -> opengl_win);
  view -> opengl_win = NULL;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean close_advanced (GtkWidget * window, gpointer data)

  \brief close OpenGL rendering advanced window callback GTK4

  \param window the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean close_advanced (GtkWidget * window, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean close_advanced (GtkWidget * widg, GdkEvent * event, gpointer data)

  \brief close OpenGL rendering advanced window callback GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean close_advanced (GtkWidget * widg, GdkEvent * event, gpointer data)
#endif
{
  close_advanced_opengl (data);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void opengl_advanced (GtkWidget * widg, gpointer data)

  \brief create OpenGL rendering advanced window

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void opengl_advanced (GtkWidget * widg, gpointer data)
{
  glwin * view = (glwin *)data;
  if (view -> opengl_win == NULL)
  {
    view -> opengl_win = g_malloc0 (sizeof*view -> opengl_win);
    view -> opengl_win -> proj = view -> proj;
    int i;
    for (i=0; i<6; i++)
    {
      view -> opengl_win -> pointer[i].a = view -> proj;
      view -> opengl_win -> pointer[i].b = i;
    }
    gchar * str = g_strdup_printf ("OpenGL material aspect and light settings - %s", get_project_by_id(view -> proj) -> name);
    view -> opengl_win -> win = create_win (str, view -> win, FALSE, FALSE);
    g_free (str);
#ifdef DEBUG
    gtk_window_set_resizable (GTK_WINDOW (view -> opengl_win -> win), TRUE);
#endif
    GtkWidget * vbox = create_vbox (5);
    add_container_child (CONTAINER_WIN, view -> opengl_win -> win, vbox);
#ifdef GTK4
    gtk_widget_set_size_request (vbox, 580, 670);
#else
    gtk_widget_set_size_request (vbox, 580, 650);
#endif
    GtkWidget * notebook = gtk_notebook_new ();
    show_the_widgets (notebook);
#ifdef GTK4
   gtk_widget_set_vexpand (notebook, TRUE);
#endif
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, notebook, TRUE, TRUE, 0);
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), materials_tab (view, view -> opengl_win, & view -> anim -> last -> img -> m_terial), markup_label("<b>Material aspect</b>", -1, -1, 0.0, 0.5));
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), lights_tab (view, view -> opengl_win, & view -> anim -> last -> img -> l_ghtning),
                                                      markup_label("<b>Configure light sources</b>", -1, -1, 0.0, 0.5));
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), fog_tab (view, view -> opengl_win, & view -> anim -> last -> img -> f_g), markup_label("<b>Configure fog</b>", -1, -1, 0.0, 0.5));
    add_global_option (vbox, & view -> colorp[0][0]);
    add_gtk_close_event (view -> opengl_win -> win, G_CALLBACK(close_advanced), view);
  }
  show_the_widgets (view -> opengl_win -> win);
  update_light_data (0, view -> opengl_win);
  setup_fog_dialogs (view -> opengl_win, view -> anim -> last -> img -> f_g.mode);
}
