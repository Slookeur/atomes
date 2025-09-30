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
* @file w_encode.c
* @short Functions to create the encoding (image/video) window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_encode.c'
*
* Contains:
*

 - The functions to create the encoding (image/video) window

*
* List of functions:

  void clean_animation (glwin * view);
  void set_encoding_widget_sensitivity (gboolean video, int sensitivity);
  void window_encode (glwin * view, gboolean video);

  G_MODULE_EXPORT void set_video_frames (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_video_extra_frames (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_video_res (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_video_codec (GtkComboBox *ComboBoxGtk);
  G_MODULE_EXPORT void set_video_opengl_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_video_bitrate (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_image_format (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void run_window_encode (GtkDialog * win ,gint response_id, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "movie.h"

int video_res[2];
int framesec;
int extraframes;
int codec;
int oglquality;
int bitrate;

extern char * codec_name[VIDEO_CODECS];
extern char * image_name[IMAGE_FORMATS];
extern gboolean spin (gpointer data);

/*!
  \fn G_MODULE_EXPORT void set_video_frames (GtkEntry * res, gpointer data)

  \brief set number of frames

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_video_frames (GtkEntry * res, gpointer data)
{
  const gchar * n;
  int frs;
  n = entry_get_text (res);
  frs = string_to_double ((gpointer)n);
  if (frs > 0)
  {
    framesec = frs;
  }
  update_entry_int (res, framesec);
}

/*!
  \fn G_MODULE_EXPORT void set_video_extra_frames (GtkEntry * res, gpointer data)

  \brief set video extra frames

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_video_extra_frames (GtkEntry * res, gpointer data)
{
  const gchar * n;
  int exf;
  n = entry_get_text (res);
  exf = string_to_double ((gpointer)n);
  if (exf >= 0)
  {
    extraframes = exf;
  }
  update_entry_int (res, extraframes);
}

/*!
  \fn G_MODULE_EXPORT void set_video_res (GtkEntry * res, gpointer data)

  \brief set video / image resolution

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_video_res (GtkEntry * res, gpointer data)
{
  const gchar * n;
  int vres;
  int id = GPOINTER_TO_INT(data);
  n = entry_get_text (res);
  vres = string_to_double ((gpointer)n);
  if (vres > 0)
  {
    video_res[id] = vres;
  }
  update_entry_int (res, video_res[id]);
}

/*!
  \fn G_MODULE_EXPORT void set_video_codec (GtkComboBox *ComboBoxGtk)

  \brief change video codec

  \param *ComboBoxGtk the GtkComboBox sending the signal
*/
G_MODULE_EXPORT void set_video_codec (GtkComboBox *ComboBoxGtk)
{
  codec = combo_get_active ((GtkWidget *)ComboBoxGtk);
}

/*!
  \fn G_MODULE_EXPORT void set_video_opengl_spin (GtkSpinButton * res, gpointer data)

  \brief set encoding OpenGL quality callback - spin

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_video_opengl_spin (GtkSpinButton * res, gpointer data)
{
  qual = gtk_spin_button_get_value_as_int(res);
  if (qual == 0 || (qual > 1 && qual <= 1000))
  {
    oglquality = qual;
  }
  update_entry_int (GTK_ENTRY(res), oglquality);
}

/*!
  \fn G_MODULE_EXPORT void set_video_bitrate (GtkEntry * res, gpointer data)

  \brief set video bitrate entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_video_bitrate (GtkEntry * res, gpointer data)
{
  const gchar * n;
  int bit;
  n = entry_get_text (res);
  bit = string_to_double ((gpointer)n);
  if (bit > 0)
  {
    bitrate = bit;
  }
  update_entry_int (res, bitrate);
}

/*!
  \fn void clean_animation (glwin * view)

  \brief clean saved animation data

  \param view the target glwin
*/
void clean_animation (glwin * view)
{
  int i;
  snapshot * shot = view -> anim -> first;
  snapshot * del;
  for (i=0; i < view -> anim -> frames-1; i++)
  {
    del = shot;
    shot = shot -> next;
    g_free (del);
  }
  view -> anim -> first = view -> anim -> last = shot;
  view -> anim -> frames = 0;
  for (i=0; i<5; i++)
  {
    if (view -> anim -> last -> img -> i_rings[i] != NULL)
    {
      g_free (view -> anim -> last -> img -> i_rings[i]);
      view -> anim -> last -> img -> i_rings[i] = NULL;
    }
  }
}

GtkWidget * resf;
GtkWidget * rese;
GtkWidget * resb;
GtkWidget * res[2];
GtkWidget * cod;

/*!
  \fn void set_encoding_widget_sensitivity (gboolean video, int sensitivity)

  \brief Adjust the sensitivity for the widgets of the encoding window

  \param video encoding video (yes / no)
  \param sensitivity sensitivity
*/
void set_encoding_widget_sensitivity (gboolean video, int sensitivity)
{
  if (video)
  {
    widget_set_sensitive (resf, sensitivity);
    widget_set_sensitive (rese, sensitivity);
    widget_set_sensitive (resb, sensitivity);
  }
  widget_set_sensitive (res[0], sensitivity);
  widget_set_sensitive (res[1], sensitivity);
  widget_set_sensitive (cod, sensitivity);
}

GtkWidget * form;

/*!
  \fn G_MODULE_EXPORT void set_image_format (GtkComboBox * box, gpointer data)

  \brief selecting the image format

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_image_format (GtkComboBox * box, gpointer data)
{
  codec = combo_get_active ((GtkWidget *)box);
  switch (codec)
  {
    case 0:
      gtk_image_set_from_file (GTK_IMAGE(form), PACKAGE_PNG);
      break;
    case 1:
      gtk_image_set_from_file (GTK_IMAGE(form), PACKAGE_JPG);
      break;
    case 2:
      gtk_image_set_from_file (GTK_IMAGE(form), PACKAGE_TIFF);
      break;
    case 3:
      gtk_image_set_from_file (GTK_IMAGE(form), PACKAGE_BMP);
      break;
  }
}

GtkWidget * encoding_pb;
gboolean encode_video;

/*!
  \fn G_MODULE_EXPORT void run_window_encode (GtkDialog * win , gint response_id, gpointer data)

  \brief encoding a movie - running the dialog

  \param win the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_window_encode (GtkDialog * win , gint response_id, gpointer data)
{
  glwin * view = (glwin *)data;
  if (response_id == GTK_RESPONSE_APPLY)
  {
    set_encoding_widget_sensitivity (encode_video, 0);
    video_options * vopts = g_malloc0(sizeof*vopts);
    vopts -> proj = view -> proj;
    vopts -> oglquality = oglquality;
    vopts -> video_res = duplicate_int (2, video_res);
    vopts -> codec = codec;
    if (encode_video)
    {
      vopts -> framesec = framesec;
      vopts -> extraframes = extraframes;
      vopts -> bitrate = bitrate;
      save_movie (view, vopts);
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(encoding_pb), 0.0);
    }
    else
    {
      render_image (view, vopts);
    }
    g_free (vopts -> video_res);
    g_free (vopts);
    set_encoding_widget_sensitivity (encode_video, 1);
  }
  else
  {
    destroy_this_dialog (win);
  }
}

/*!
  \fn void window_encode (glwin * view, gboolean video)

  \brief encoding a movie - creating the dialog

  \param view the target glwin
  \param video video (1) or image (0)
*/
void window_encode (glwin * view, gboolean video)
{
  gchar * str;
  int i;
  if (video)
  {
    str = g_strdup_printf ("%s - movie encoding", prepare_for_title(get_project_by_id(view -> proj) -> name));
  }
  else
  {
    str = g_strdup_printf ("%s - image rendering", prepare_for_title(get_project_by_id(view -> proj) -> name));
  }
  GtkWidget * win = dialog_cancel_apply (str, view -> win, FALSE);
  g_free (str);
  GtkWidget * vbox = dialog_get_content_area (win);
  GtkWidget * hbox;
  if (video)
  {
    gtk_widget_set_size_request (vbox, -1, 420);
    // Frames
    hbox = create_hbox (0);
    gtk_widget_set_size_request (hbox, 300, -1);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Frames recorded:", 350, -1, 0.0, 0.5), FALSE, FALSE, 0);
    str = g_strdup_printf ("<b>%d</b>", view -> anim -> frames);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    g_free (str);

    // Frames per second
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    gtk_widget_set_size_request (hbox, 300, -1);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Frames per seconds:", 350, -1, 0.0, 0.5), FALSE, FALSE, 0);
    resf = create_entry (G_CALLBACK(set_video_frames), 100, 10, FALSE, NULL);
    framesec = 24;
    update_entry_int (GTK_ENTRY(resf), framesec);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, resf, FALSE, FALSE, 0);

    // Extra frames
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
    gtk_widget_set_size_request (hbox, 300, -1);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Extra frames every (frames):", 350, -1, 0.0, 0.5), FALSE, FALSE, 0);
    rese = create_entry (G_CALLBACK(set_video_extra_frames), 100, 10, FALSE, NULL);
    extraframes = 10;
    update_entry_int (GTK_ENTRY(rese), extraframes);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, rese, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("Movie resolution (in pixels):", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
  else
  {
    gtk_widget_set_size_request (vbox, -1, 230);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("Image resolution (in pixels):", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
  }
  hbox = create_hbox (0);
  gtk_widget_set_size_request (hbox, 300, -1);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, (video) ? 10 : 0);
  gchar * ax[2]={"x: ", "y: "};
  for (i=0; i<2; i++)
  {
    video_res[i] = view -> pixels[i];
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(ax[i], 100, -1, 0.5, 0.5), FALSE, FALSE, 0);
    res[i] = create_entry (G_CALLBACK(set_video_res), 100, 10, FALSE, (gpointer)GINT_TO_POINTER(i));
    update_entry_int (GTK_ENTRY(res[i]), video_res[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, res[i], FALSE, FALSE, 0);
  }

  cod = create_combo ();
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, video ? 0 : 10);
  gtk_widget_set_size_request (hbox, 300, -1);
  if (video)
  {
    // Codec
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Video codec:", 300, -1, 0.0, 0.5), FALSE, FALSE, 0);
    for (i=0; i<VIDEO_CODECS; i++) combo_text_append (cod, codec_name[i]);
    g_signal_connect (G_OBJECT(cod), "changed", G_CALLBACK(set_video_codec), NULL);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, cod, FALSE, FALSE, 0);
  }
  else
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Image format:", 150, -1, 0.0, 0.5), FALSE, FALSE, 0);
    GtkWidget * fixed = gtk_fixed_new ();
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, TRUE, TRUE, 0);
    for (i=0; i<IMAGE_FORMATS; i++) combo_text_append (cod, image_name[i]);
    gtk_fixed_put (GTK_FIXED(fixed), cod, -1, 10);
    g_signal_connect (G_OBJECT(cod), "changed", G_CALLBACK(set_image_format), NULL);
    form = gtk_image_new ();
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, form, TRUE, TRUE, 0);
  }
  codec = 0;
  combo_set_active (cod, codec);

  if (! video) set_image_format (GTK_COMBO_BOX(cod), NULL);

  if (video)
  {
     // Overall video Quality
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
    gtk_widget_set_size_request (hbox, 300, -1);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Video quality (bitrate in kb/s):", 350, -1, 0.0, 0.5), FALSE, FALSE, 0);
    resb = create_entry (G_CALLBACK(set_video_bitrate), 100, 10, FALSE, NULL);
    bitrate = 5000;
    update_entry_int (GTK_ENTRY(resb), bitrate);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, resb, FALSE, FALSE, 0);
  }
  // Overall OpenGL Quality
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  gtk_widget_set_size_request (hbox, 300, -1);
  if (video)
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("OpenGL quality [0-1000] (0= recorded quality):", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
  else
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("OpenGL quality [0-1000] (0= on-screen quality):", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
  }
  oglquality = 0;
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, spin_button (G_CALLBACK(set_video_opengl_spin), 0, 0, 1000, 1, 0, 100, NULL), FALSE, FALSE, 20);
  if (video)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("Progress: ", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
    encoding_pb = gtk_progress_bar_new ();
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(encoding_pb), 0.0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, encoding_pb, FALSE, FALSE, 0);
  }
  show_the_widgets (win);

  if (view -> spin[0] || view -> spin[1])
  {
    for (i=0; i<2; i++)
    {
      view -> spin[i+2] = view -> spin[i];
      view -> spin_speed[i+2] = view -> spin_speed[i];
      view -> spin[i] = FALSE;
    }
  }
  gboolean old_play, old_stop;
  old_play = view -> play;
  old_stop = view -> stop;
  view -> play = FALSE;
  view -> stop = TRUE;
  encode_video = video;
  run_this_gtk_dialog (win, G_CALLBACK(run_window_encode), view);
  clean_animation (view);
  update (view);
  if (view -> spin[2] || view -> spin[3])
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
  view -> play = old_play;
  view -> stop = old_stop;
}
