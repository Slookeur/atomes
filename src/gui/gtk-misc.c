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
* @file gtk-misc.c
* @short GTK3/GTK4 utility subroutines, instead of having pre-processor flags everywhere in the code. \n
         Whenever I need to use a GTK function that changed between v3 and v4, \n
         I created an home made function declared in this file, and deal then here with the potential GTK versions issues.
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'gtk-misc.c'
*
* Contains:
*

 - GTK3/GTK4 utility subroutines, instead of having pre-processor flags everywhere in the code
   Whenever I need to use a GTK function that changed between v3 and v4,
   I created an home made function declared in this file, and deal then here with the potential GTK versions issues.

*
* List of functions:

  gboolean is_the_widget_visible (GtkWidget * widg);
  gboolean file_chooser_set_file_name (GtkFileChooser * chooser, gchar * filename);

  G_MODULE_EXPORT gboolean to_activate_entry (GtkWidget * widg, GdkEventFocus * event, gpointer data);
  G_MODULE_EXPORT gboolean destroy_this_window (GtkWindow * win, gpointer data);
  G_MODULE_EXPORT gboolean destroy_this_window (GtkWidget * win, GdkEvent * event, gpointer data);
  G_MODULE_EXPORT gboolean hide_this_window (GtkWindow * win, gpointer data);
  G_MODULE_EXPORT gboolean hide_this_window (GtkWidget * win, GdkEvent * event, gpointer data);

  gchar * file_chooser_get_file_name (GtkFileChooser * chooser);
  gchar * file_chooser_get_current_folder (GtkFileChooser * chooser);

  const gchar * entry_get_text (GtkEntry * entry);

  void show_the_widgets (GtkWidget * widg);
  void hide_the_widgets (GtkWidget * widg);
  void widget_set_sensitive (GtkWidget * widg, gboolean sensitive);
  void add_container_child (int type, GtkWidget * widg, GtkWidget * child);
  void add_box_child_end (GtkWidget * widg, GtkWidget * child, gboolean expand, gboolean fill, int padding);
  void add_box_child_start (int orientation, GtkWidget * widg, GtkWidget * child, gboolean expand, gboolean fill, int padding);
  void add_widget_gesture_and_key_action (GtkWidget * widget,
                                          gchar * cp_name, GCallback cp_handler, gpointer cp_data,
                                          gchar * cr_name, GCallback cr_handler, gpointer cr_data,
                                          gchar * kp_name, GCallback kp_handler, gpointer kp_data,
                                          gchar * mo_name, GCallback mo_handler, gpointer mo_data,
                                          gchar * sc_name, GCallback sc_handler, gpointer sc_data);
  void run_this_gtk_native_dialog (GtkNativeDialog * dial, GCallback handler, gpointer data);
  void run_this_gtk_dialog (GtkWidget * dial, GCallback handler, gpointer data);
  void resize_this_window (GtkWidget * window, int x, int y);
  void button_set_status (GtkWidget * button, int status);
  void update_entry_int (GtkEntry * entry, int intval);
  void update_entry_double (GtkEntry * entry, double doubleval);
  void update_entry_long_double (GtkEntry * entry, double doubleval);
  void update_entry_text (GtkEntry * entry, gchar * text);
  void text_view_set_monospace (GtkWidget * view);
  void gtk_label_align (GtkWidget * lab, float ax, float ay);
  void layout_add_widget (GtkWidget * layout, GtkWidget * child, int x_pos, int y_pos);
  void combo_set_markup (GtkWidget * combo);
  void combo_set_active (GtkWidget * combo, int pos);
  void combo_text_append (GtkWidget * combo, gchar * text);
  void combo_text_prepend (GtkWidget * combo, gchar * text);
  void setup_text_tags (GtkTextBuffer * buffer);
  void add_menu_separator (GtkWidget * menu);
  void set_renderer_color (int tocol, GtkCellRenderer * renderer, ColRGBA col);
  void button_set_image (GtkButton * but, gchar * text, int format, gpointer image);
  void adjust_label (GtkWidget * lab, int dimx, int dimy, float ax, float ay);
  void set_image_from_icon_name (GtkWidget * widg, gchar * icon);
  void append_comments (GtkWidget * vbox, gchar * symbol, gchar * legend);
  void provide_gtk_css (gchar * css);
  void destroy_this_dialog (GtkDialog * dialog);
  void destroy_this_native_dialog (GtkNativeDialog * dialog);
  void file_chooser_set_current_folder (GtkFileChooser * chooser);
  void set_color_chooser_color (GtkWidget * color_win, ColRGBA col);
  void pop_menu_at_pointer (GtkWidget * pop, double x, double y);
  void pop_menu_at_pointer (GtkWidget * widg, GdkEvent * event);
  void add_gtk_close_event (GtkWidget * widg, GCallback handler, gpointer data);
  static void convert_alpha (cairo_surface_t * surf, GdkPixbuf * pix, int src_x, int src_y, int width, int height);

  G_MODULE_EXPORT void to_activate_entry (GtkEventControllerFocus * focus, gpointer data);
  G_MODULE_EXPORT void run_destroy_dialog (GtkDialog * dialog, gint response_id, gpointer data);

  GtkWidget * menu_item_new_with_submenu (gchar * name, gboolean active, GtkWidget * sub_menu);
  GtkWidget * new_gtk_window ();
  GtkWidget * create_win (gchar * str, GtkWidget * parent, gboolean modal, gboolean resiz);
  GtkWidget * dialogmodal (gchar * str, GtkWindow * parent);
  GtkWidget * message_dialogmodal (gchar * message, gchar * title, GtkMessageType mtype, GtkButtonsType buttons, GtkWidget * parent);
  GtkWidget * dialog_cancel_apply (gchar * title, GtkWidget * parent, gboolean resiz);
  GtkWidget * create_hscale (float min, float max, float delta,
                             float val, int pos, int round, int size,
                             GCallback handler, GCallback scroll_handler, gpointer data);
  GtkWidget * create_vscale (float min, float max, float delta,
                             float val, int pos, int round, int size,
                             GCallback handler, GCallback scroll_handler, gpointer data);
  GtkWidget * create_vbox (int spacing);
  GtkWidget * create_hbox (int spacing);
  GtkWidget * dialog_get_content_area (GtkWidget * widg);
  GtkWidget * add_vbox_to_layout (GtkWidget * layout, int size_x, int size_y);
  GtkWidget * create_layout (int x, int y);
  GtkWidget * create_combo ();
  GtkWidget * create_text_view (int dimx, int dimy, int edit, int mono, GCallback handler, gpointer data, gchar * text);
  GtkWidget * create_entry (GCallback handler, int dim, int cdim, gboolean key_release, gpointer data);
  GtkWidget * stock_image (const gchar * stock_id);
  GtkWidget * create_menu_item (gboolean add_mnemo, gchar * name);
  GtkWidget * create_menu_item_from_widget (GtkWidget * widg, gboolean check, gboolean radio, gboolean status);
  GtkWidget * create_image_from_data (int format, gpointer item_image);
  GtkWidget * gtk3_menu_item (GtkWidget * menu, gchar * name,
                              int icon_format, gpointer item_icon,
                              GCallback handler, gpointer data,
                              gboolean accel, guint key, GdkModifierType mod,
                              gboolean check, gboolean radio, gboolean status);
  GtkWidget * add_advanced_item (GtkWidget * menu, GCallback handler, gpointer data, gboolean accel, guint key, GdkModifierType mod);
  GtkWidget * markup_label (gchar * text, int dimx, int dimy, float ax, float ay);
  GtkWidget * color_button (ColRGBA col, gboolean alpha, int dimx, int dimy, GCallback handler, gpointer data);
  GtkWidget * font_button (gchar * font, int dimx, int dimy, GCallback handler, gpointer data);
  GtkWidget * spin_button (GCallback handler, double value, double start, double end, double step, int digits, int dim,  gpointer data);
  GtkWidget * check_button (gchar * text, int dimx, int dimy, gboolean state, GCallback handler, gpointer data);
  GtkWidget * radio_button (gchar * text, int dimx, int dimy, gboolean state, GCallback handler, gpointer data);
  GtkWidget * create_button (gchar * text, int image_format, gchar * image, int dimx, int dimy, int relief, GCallback handler, gpointer data);
  GtkWidget * abox (GtkWidget * box, char * lab, int vspace);
  GtkWidget * bbox (GtkWidget * box, char * lab);
  GtkWidget * cbox (GtkWidget * box, char * lab);
  GtkWidget * fbox (GtkWidget * box, char * lab);
  GtkWidget * create_scroll (GtkWidget * box, int dimx, int dimy, int shadow);
  GtkWidget * create_expander (gchar * name, gchar * file_img);
  GtkWidget * destroy_this_widget (GtkWidget * widg);
  GtkWidget * create_file_chooser (const gchar * title, GtkWindow * parent, GtkFileChooserAction act, const gchar * act_name);
  GtkWidget * get_top_level (GtkWidget * widg);

  GtkTextBuffer * add_buffer (GCallback handler, gpointer data, gchar * text);

  GtkFileChooserNative * create_file_chooser (const gchar * title, GtkWindow * parent, GtkFileChooserAction act, const gchar * act_name);

  GdkPixbuf * convert_to_pixbuf (cairo_surface_t * surf);

  GMenuItem * create_menu_item (gboolean add_mnemo, gchar * name, gchar * action);

  GListModel * file_chooser_get_file_names (GtkFileChooser * chooser);

  ColRGBA * duplicate_color (int num, ColRGBA * col);
  ColRGBA gdkrgba_to_rgba (GdkRGBA colgdk);
  ColRGBA get_button_color (GtkColorChooser * colob);
  ColRGBA get_window_color (GtkWidget * color_win);

  GdkRGBA colrgba_togtkrgba (ColRGBA col);

  int get_widget_width (GtkWidget * widg);
  int get_widget_height (GtkWidget * widg);
  int button_get_status (GtkWidget * button);

*/

#include "global.h"
#include "interface.h"
#include "preferences.h"
#include "glview.h"

extern void adjust_preferences_window ();

/*!
  \fn void show_the_widgets (GtkWidget * widg)

  \brief show GtkWidget

  \param widg the GtkWidget to show
*/
void show_the_widgets (GtkWidget * widg)
{
#ifdef GTK4
  gtk_widget_set_visible (widg, TRUE);
#else
  gtk_widget_show_all (widg);
#endif
}

/*!
  \fn void hide_the_widgets (GtkWidget * widg)

  \brief hide GtkWidget

  \param widg the GtkWidget to show
*/
void hide_the_widgets (GtkWidget * widg)
{
#ifdef GTK4
  gtk_widget_set_visible (widg, FALSE);
#else
  gtk_widget_hide (widg);
#endif
}

/*!
  \fn void widget_set_sensitive (GtkWidget * widg, gboolean sensitive)

  \brief Set sensitivity for a GtkWidget, ensuring it is a GtkWidget

  \param widg the GtkWidget
  \param sensitive Sensitivity
*/
void widget_set_sensitive (GtkWidget * widg, gboolean sensitive)
{
  if (widg != NULL)
  {
    if (GTK_IS_WIDGET(widg))
    {
      gtk_widget_set_sensitive (widg, sensitive);
    }
  }
}

/*!
  \fn void add_container_child (int type, GtkWidget * widg, GtkWidget * child)

  \brief Add a GtkWidget into another GtkWidget

  \param type GTK4 only: the type of container
  \param widg the container Gtkwidget
  \param child the child GtkWidget
*/
void add_container_child (int type, GtkWidget * widg, GtkWidget * child)
{
#ifdef GTK3
  gtk_container_add (GTK_CONTAINER(widg), child);
#else
  switch (type)
  {
    case CONTAINER_WIN:
      // gtk_window_set_child ((GtkWindow *)widg, NULL);
      gtk_window_set_child ((GtkWindow *)widg, child);
      break;
    case CONTAINER_SCR:
      // Bug in the management of scrolled window child
/* Frequently getting warning messages at this point with GTK4
(atomes:?????): GLib-GObject-CRITICAL **: ??:??:??.???: g_object_set: assertion 'G_IS_OBJECT (object)' failed
(atomes:?????): Gtk-CRITICAL **: ??:??:??.???: gtk_widget_unparent: assertion 'GTK_IS_WIDGET (widget)' failed
*/
      gtk_scrolled_window_set_child ((GtkScrolledWindow *)widg, NULL);
      gtk_scrolled_window_set_child ((GtkScrolledWindow *)widg, child);
      break;
    case CONTAINER_VIE:
      // gtk_viewport_set_child ((GtkViewport *)widg, NULL);
      gtk_viewport_set_child ((GtkViewport *)widg, child);
      break;
    case CONTAINER_BUT:
      // gtk_button_set_child ((GtkButton *)widg, NULL);
      gtk_button_set_child ((GtkButton *)widg, child);
      break;
    case CONTAINER_FRA:
      // gtk_frame_set_child ((GtkFrame *)widg, NULL);
      gtk_frame_set_child ((GtkFrame *)widg, child);
      break;
    case CONTAINER_EXP:
      // gtk_expander_set_child ((GtkExpander *)widg, NULL);
      gtk_expander_set_child ((GtkExpander *)widg, child);
      break;
  }
#endif
}

/*!
  \fn void add_box_child_end (GtkWidget * widg, GtkWidget * child, gboolean expand, gboolean fill, int padding)

  \brief Add a GtkWidget in a GtkBox at the end position

  \param widg the GtkBox
  \param child the GtkWidget to add
  \param expand GTK3 only: expandable
  \param fill GTK3 only: fill
  \param padding GTK3 only: box padding
*/
void add_box_child_end (GtkWidget * widg, GtkWidget * child, gboolean expand, gboolean fill, int padding)
{
#ifdef GTK4
  // gtk_widget_set_hexpand (child, TRUE);
  gtk_box_append (GTK_BOX(widg), child);
#else
  gtk_box_pack_end (GTK_BOX(widg), child, expand, fill, padding);
#endif
}

/*!
  \fn void add_box_child_start (int orientation, GtkWidget * widg, GtkWidget * child, gboolean expand, gboolean fill, int padding)

  \brief Add a GtkWidget in a GtkBox at the initial position

  \param orientation horizontal or vertical
  \param widg the GtkBox
  \param child the GtkWidget to add
  \param expand GTK3 only: expandable
  \param fill GTK3 only: fill
  \param padding GTK3 only: box padding
*/
void add_box_child_start (int orientation, GtkWidget * widg, GtkWidget * child, gboolean expand, gboolean fill, int padding)
{
#ifdef GTK4
  if (orientation == GTK_ORIENTATION_HORIZONTAL && padding)
  {
    gtk_box_append (GTK_BOX(widg), markup_label (" ", padding, -1, 0.0, 0.0));
  }
  else if (orientation == GTK_ORIENTATION_VERTICAL && padding > 10)
  {
    gtk_box_append (GTK_BOX(widg), markup_label (" ", -1, padding/2, 0.0, 0.0));
  }
  gtk_box_append (GTK_BOX(widg), child);
  if (orientation == GTK_ORIENTATION_HORIZONTAL && padding)
  {
    gtk_box_append (GTK_BOX(widg), markup_label (" ", padding, -1, 0.0, 0.0));
  }
  else if (orientation == GTK_ORIENTATION_VERTICAL && padding > 10)
  {
    gtk_box_append (GTK_BOX(widg), markup_label (" ", -1, padding/2, 0.0, 0.0));
  }
#else
  gtk_box_pack_start (GTK_BOX(widg), child, expand, fill, padding);
#endif
}

#ifdef GTK3
/*!
  \fn GtkWidget * menu_item_new_with_submenu (gchar * name, gboolean active, GtkWidget * sub_menu)

  \brief Adding a GtkMenuItem with a submenu and setting the sensitivity

  \param name Name of the menu item
  \param active Sensitivity
  \param sub_menu the GtkWidget submenu
*/
GtkWidget * menu_item_new_with_submenu (gchar * name, gboolean active, GtkWidget * sub_menu)
{
   GtkWidget * item =  create_menu_item (FALSE, name);
   gtk_menu_item_set_submenu ((GtkMenuItem *)item, sub_menu);
   widget_set_sensitive (item, active);
   return item;
}
#endif

/*!
  \fn GtkWidget * new_gtk_window ()

  \brief create a new GtkWindow
*/
GtkWidget * new_gtk_window ()
{
#ifdef GTK4
  return gtk_window_new ();
#else
  return gtk_window_new (GTK_WINDOW_TOPLEVEL);
#endif
}

#ifdef GTK4
/*!
  \fn void add_widget_gesture_and_key_action (GtkWidget * widget,
                                              gchar * cp_name, GCallback cp_handler, gpointer cp_data,
                                              gchar * cr_name, GCallback cr_handler, gpointer cr_data,
                                              gchar * kp_name, GCallback kp_handler, gpointer kp_data,
                                              gchar * mo_name, GCallback mo_handler, gpointer mo_data,
                                              gchar * sc_name, GCallback sc_handler, gpointer sc_data)

  \brief Adding GTK4 mouse, and keyboard events to a GtkWidget

  \param widget the GtkWidget
  \param cp_name Name of the click pressed event
  \param cp_handler Click pressed callback
  \param cp_data Click pressed data pointer
  \param cr_name Name of the click released event
  \param cr_handler Click released callback
  \param cr_data Click released data pointer
  \param kp_name Name of the key pressed event
  \param kp_handler Key pressed callback
  \param kp_data Key pressed data pointer
  \param mo_name Name of the mouse motion event
  \param mo_handler Mouse motion callback
  \param mo_data Mouse motion data pointer
  \param sc_name Name of the mouse scroll event
  \param sc_handler Mouse scroll callback
  \param sc_data Mouse scroll data pointer
*/
void add_widget_gesture_and_key_action (GtkWidget * widget,
                                        gchar * cp_name, GCallback cp_handler, gpointer cp_data,
                                        gchar * cr_name, GCallback cr_handler, gpointer cr_data,
                                        gchar * kp_name, GCallback kp_handler, gpointer kp_data,
                                        gchar * mo_name, GCallback mo_handler, gpointer mo_data,
                                        gchar * sc_name, GCallback sc_handler, gpointer sc_data)
{
  GtkGesture * gesture;
  GtkEventController * key;
  GtkEventController * motion;
  if (cp_name)
  {
    gesture = gtk_gesture_click_new ();
    gtk_event_controller_set_name (GTK_EVENT_CONTROLLER (gesture), cp_name);
    gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture), 0);
    if (cp_handler) g_signal_connect (gesture, "pressed", cp_handler, cp_data);
    gtk_widget_add_controller (widget, GTK_EVENT_CONTROLLER (gesture));
  }
  if (cr_name)
  {
    gesture = gtk_gesture_click_new ();
    gtk_event_controller_set_name (GTK_EVENT_CONTROLLER (gesture), cr_name);
    gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture), 0);
    if (cr_handler) g_signal_connect (gesture, "released", cr_handler, cr_data);
    gtk_widget_add_controller (widget, GTK_EVENT_CONTROLLER (gesture));
  }
  if (kp_name)
  {
    key = gtk_event_controller_key_new ();
    gtk_event_controller_set_name (key, kp_name);
    if (kp_handler) g_signal_connect (key, "key-pressed", kp_handler, kp_data);
    gtk_widget_add_controller (widget, key);
  }
  if (mo_name)
  {
    motion = gtk_event_controller_motion_new ();
    gtk_event_controller_set_name (motion, mo_name);
    if (mo_handler) g_signal_connect (motion, "motion", mo_handler, mo_data);
    gtk_widget_add_controller (widget, motion);
  }
  if (sc_name)
  {
    motion = gtk_event_controller_scroll_new (GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    gtk_event_controller_set_name (motion, sc_name);
    if (sc_handler) g_signal_connect (motion, "scroll", sc_handler, sc_data);
    gtk_widget_add_controller (widget, motion);
  }
}
#endif

/*!
  \fn GtkWidget * create_win (gchar * str, GtkWidget * parent, gboolean modal, gboolean resiz)

  \brief create a new GtkWindow

  \param str Title
  \param parent Parent GtkWidget (if any)
  \param modal Modal (TRUE/FALSE)
  \param resiz Can be re-sized (TRUE/FALSE)
*/
GtkWidget * create_win (gchar * str, GtkWidget * parent, gboolean modal, gboolean resiz)
{
  GtkWidget * win;
  win = new_gtk_window ();
  if (str) gtk_window_set_title (GTK_WINDOW(win), prepare_for_title(str));
  gtk_window_set_resizable (GTK_WINDOW (win), TRUE);
#ifdef GTK3
  gtk_window_set_attached_to (GTK_WINDOW (win), parent);
  gtk_window_set_icon (GTK_WINDOW (win), THETD);
#endif
  // gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW (parent));
  gtk_window_set_modal (GTK_WINDOW(win), modal);
  gtk_window_set_resizable (GTK_WINDOW(win), resiz);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (win), TRUE);
  return win;
}

#ifdef GTK4
/*!
  \fn void run_this_gtk_native_dialog (GtkNativeDialog * dial, GCallback handler, gpointer data)

  \brief run a GTK4 GtkNativeDialog (used to read files)

  \param dial the GtkNativeDialog
  \param handler the callback
  \param data the associated data pointer
*/
void run_this_gtk_native_dialog (GtkNativeDialog * dial, GCallback handler, gpointer data)
{
  gtk_native_dialog_set_modal (dial, TRUE);
  if (handler) g_signal_connect (G_OBJECT(dial), "response", handler, data);
  gtk_native_dialog_show (dial);
  dialog_id ++;
  Event_loop[dialog_id] = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (Event_loop[dialog_id]);
}
#endif

/*!
  \fn void run_this_gtk_dialog (GtkWidget * dial, GCallback handler, gpointer data)

  \brief run a GTK (3 and 4) basic GtkDialog

  \param dial the GtkDialog
  \param handler the callback
  \param data the associated data pointer
*/
void run_this_gtk_dialog (GtkWidget * dial, GCallback handler, gpointer data)
{
  gtk_window_set_modal (GTK_WINDOW(dial), TRUE);
  if (handler) g_signal_connect (G_OBJECT(dial), "response", handler, data);
  show_the_widgets (dial);
  if (preferences) adjust_preferences_window ();
  dialog_id ++;
  Event_loop[dialog_id] = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (Event_loop[dialog_id]);
}

/*!
  \fn GtkWidget * dialogmodal (gchar * str, GtkWindow * parent)

  \brief Create a new dialog modal window

  \param str Title
  \param parent Parent GtkWindow, if any
*/
GtkWidget * dialogmodal (gchar * str, GtkWindow * parent)
{
  GtkWidget * win = gtk_dialog_new_with_buttons (str, parent, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, _("_Close"), GTK_RESPONSE_CLOSE, NULL);
  gtk_window_set_resizable (GTK_WINDOW (win), FALSE);
#ifdef GTK3
  gtk_window_set_icon (GTK_WINDOW (win), THETD);
#endif
  return win;
}

/*!
  \fn GtkWidget * message_dialogmodal (gchar * message, gchar * title, GtkMessageType mtype, GtkButtonsType buttons, GtkWidget * parent)

  \brief create a modal (cannot be ignored) message window

  \param message the message
  \param title the title
  \param mtype the message type
  \param buttons the type of closing button to add
  \param parent the parent window, if any
*/
GtkWidget * message_dialogmodal (gchar * message, gchar * title, GtkMessageType mtype, GtkButtonsType buttons, GtkWidget * parent)
{
  GtkWidget * win;
  if (parent)
  {
    win = gtk_message_dialog_new (GTK_WINDOW(parent), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, mtype, buttons, NULL);
  }
  else
  {
    win = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, mtype, buttons, NULL);
  }
  gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (win), message);
  gtk_window_set_title (GTK_WINDOW(win), title);
  gtk_window_set_resizable (GTK_WINDOW (win), FALSE);
#ifdef GTK3
  gtk_window_set_icon (GTK_WINDOW (win), THETD);
#endif
  show_the_widgets (win);
  return win;
}

/*!
  \fn GtkWidget * dialog_cancel_apply (gchar * title, GtkWidget * parent, gboolean resiz)

  \brief create a dialog modal offering a choice to apply something or not

  \param title the message
  \param parent the parent window, if any
  \param resiz Is the new window re-sizable ?
*/
GtkWidget * dialog_cancel_apply (gchar * title, GtkWidget * parent, gboolean resiz)
{
  GtkWidget * dca = gtk_dialog_new_with_buttons (title, GTK_WINDOW(parent), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 "Cancel", GTK_RESPONSE_CANCEL, "Apply", GTK_RESPONSE_APPLY, NULL);
  gtk_window_set_resizable (GTK_WINDOW(dca), resiz);
#ifdef GTK3
  gtk_window_set_icon (GTK_WINDOW (dca), THETD);
#endif
  return dca;
}

/*!
  \fn void resize_this_window (GtkWidget * window, int x, int y)

  \brief resize this GtkWindow

  \param window the GtkWidget to resize
  \param x the x size
  \param y the y size
*/
void resize_this_window (GtkWidget * window, int x, int y)
{
#ifdef GTK3
  gtk_window_resize (GTK_WINDOW(window), x, y);
#else
  gtk_window_set_default_size (GTK_WINDOW(window), x, y);
#endif
}

/*!
  \fn const gchar * entry_get_text (GtkEntry * entry)

  \brief get the text in a GtkEntry

  \param entry the GtkEntry
*/
const gchar * entry_get_text (GtkEntry * entry)
{
#ifdef GTK4
  return gtk_editable_get_text (GTK_EDITABLE(entry));
#else
  return gtk_entry_get_text (entry);
#endif
}

/*!
  \fn void update_entry_int (GtkEntry * entry, int intval)

  \brief update the content of a GtkEntry as int

  \param entry the GtkEntry
  \param intval the new int value to display
*/
void update_entry_int (GtkEntry * entry, int intval)
{
  gchar * value = g_strdup_printf("%d", intval);
#ifdef GTK4
  gtk_editable_set_text (GTK_EDITABLE(entry), (const gchar *)value);
#else
  gtk_entry_set_text (entry, (const gchar *)value);
#endif
  g_free (value);
}

/*!
  \fn void update_entry_double (GtkEntry * entry, double doubleval)

  \brief update the content of a GtkEntry as double

  \param entry the GtkEntry
  \param doubleval the new double value to display
*/
void update_entry_double (GtkEntry * entry, double doubleval)
{
  gchar * value = g_strdup_printf("%f", doubleval);
#ifdef GTK4
  gtk_editable_set_text (GTK_EDITABLE(entry), (const gchar *)value);
#else
  gtk_entry_set_text (entry, (const gchar *)value);
#endif
  g_free (value);
}

/*!
  \fn void update_entry_long_double (GtkEntry * entry, double doubleval)

  \brief update the content of a GtkEntry as long double

  \param entry the GtkEntry
  \param doubleval the new long double value to display
*/
void update_entry_long_double (GtkEntry * entry, double doubleval)
{
  gchar * value = g_strdup_printf("%15.10lf", doubleval);
#ifdef GTK4
  gtk_editable_set_text (GTK_EDITABLE(entry), (const gchar *)value);
#else
  gtk_entry_set_text (entry, (const gchar *)value);
  g_free (value);
#endif
}

/*!
  \fn void update_entry_text (GtkEntry * entry, gchar * text)

  \brief update the content of a GtkEntry as string

  \param entry the GtkEntry
  \param text the new string to display
*/
void update_entry_text (GtkEntry * entry, gchar * text)
{
#ifdef GTK4
  gtk_editable_set_text (GTK_EDITABLE(entry), (const gchar *)text);
#else
  gtk_entry_set_text (entry, (const gchar *)text);
#endif
}

/*!
  \fn void text_view_set_monospace (GtkWidget * view)

  \brief set the font of a GtkTextView as monospace

  \param view the GtkTextView
*/
void text_view_set_monospace (GtkWidget * view)
{
  gtk_text_view_set_monospace (GTK_TEXT_VIEW(view), TRUE);
}

/*!
  \fn void gtk_label_align (GtkWidget * lab, float ax, float ay)

  \brief set text alignment in a GtkLabel

  \param lab the GtkLabel
  \param ax the x alignment
  \param ay the Y alignment
*/
void gtk_label_align (GtkWidget * lab, float ax, float ay)
{
  gtk_label_set_xalign (GTK_LABEL (lab), ax);
  gtk_label_set_yalign (GTK_LABEL (lab), ay);
}

/*!
  \fn gboolean is_the_widget_visible (GtkWidget * widg)

  \brief test if a GtkWidget exist, then return if it is visible or not

  \param widg the GtkWidget
*/
gboolean is_the_widget_visible (GtkWidget * widg)
{
  if (GTK_IS_WIDGET(widg))
  {
    return gtk_widget_is_visible (widg);
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn GtkWidget * create_hscale (float min, float max, float delta,
                                 float val, int pos, int round, int size,
                                 GCallback handler, GCallback scroll_handler, gpointer data)

  \brief create an horizontal scale GtkWidget

  \param min Min value for the h scale
  \param max Max value for the h scale
  \param delta the step on the h scale
  \param val the value to position to set on the h scale
  \param pos the location where the value is on display
  \param round the rounding digit for the value on display
  \param size the x size of the GtkWdiget
  \param handler the callback to move the h scale
  \param scroll_handler the callback on mouse scroll
  \param data the associated data pointer
*/
GtkWidget * create_hscale (float min, float max, float delta,
                           float val, int pos, int round, int size,
                           GCallback handler, GCallback scroll_handler, gpointer data)
{
  GtkWidget * hscale;
  hscale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, min, max, delta);
  gtk_scale_set_draw_value (GTK_SCALE(hscale), TRUE);
  gtk_widget_set_size_request (hscale, size, -1);
  gtk_scale_set_value_pos (GTK_SCALE(hscale), pos);
  gtk_range_set_value (GTK_RANGE(hscale), val);
  gtk_range_set_round_digits (GTK_RANGE(hscale), round);
  if (handler != NULL) g_signal_connect (G_OBJECT(hscale), "value-changed", handler, data);
  if (scroll_handler != NULL) g_signal_connect (G_OBJECT(hscale), "change-value", scroll_handler, data);
  return hscale;
}

/*!
  \fn GtkWidget * create_vscale (float min, float max, float delta,
                              float val, int pos, int round, int size,
                              GCallback handler, GCallback scroll_handler, gpointer data)

  \brief create an vertical scale GtkWidget

  \param min Min value for the v scale
  \param max Max value for the v scale
  \param delta the step on the v scale
  \param val the value to position to set on the h scale
  \param pos the location where the value is on display
  \param round the rounding digit for the value on display
  \param size the x size of the GtkWdiget
  \param handler the callback to move the v scale
  \param scroll_handler the callback on mouse scroll
  \param data the associated data pointer
*/
GtkWidget * create_vscale (float min, float max, float delta,
                           float val, int pos, int round, int size,
                           GCallback handler, GCallback scroll_handler, gpointer data)
{
  GtkWidget * vscale;
  vscale = gtk_scale_new_with_range (GTK_ORIENTATION_VERTICAL, min, max, delta);
  gtk_scale_set_draw_value (GTK_SCALE(vscale), TRUE);
  gtk_scale_set_value_pos (GTK_SCALE(vscale), pos);
  gtk_widget_set_size_request (vscale, size, size);
  gtk_range_set_value (GTK_RANGE(vscale), val);
  gtk_range_set_round_digits (GTK_RANGE(vscale), round);
  if (handler != NULL) g_signal_connect (G_OBJECT(vscale), "value-changed", handler, data);
  if (scroll_handler != NULL) g_signal_connect (G_OBJECT(vscale), "change-value", scroll_handler, data);
  return vscale;
}

/*!
  \fn GtkWidget * create_vbox (int spacing)

  \brief create a GtkBox with vertical orientation

  \param spacing spacing between elements
*/
GtkWidget * create_vbox (int spacing)
{
  return gtk_box_new (GTK_ORIENTATION_VERTICAL, spacing);
}

/*!
  \fn GtkWidget * create_hbox (int spacing)

  \brief create a GtkBox with horizontal orientation

  \param spacing spacing between elements
*/
GtkWidget * create_hbox (int spacing)
{
  return gtk_box_new (GTK_ORIENTATION_HORIZONTAL, spacing);
}

/*!
  \fn GtkWidget * dialog_get_content_area (GtkWidget * widg)

  \brief prepare GtkWidget to insert content in a GtkDialog window

  \param widg the GtkDialog window
*/
GtkWidget * dialog_get_content_area (GtkWidget * widg)
{
#ifdef GTK3
  return gtk_dialog_get_content_area (GTK_DIALOG(widg));
#else
  GtkWidget * vbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, gtk_dialog_get_content_area (GTK_DIALOG(widg)), vbox, TRUE, TRUE, 0);
  return vbox;
#endif
}

/*!
  \fn void layout_add_widget (GtkWidget * layout, GtkWidget * child, int x_pos, int y_pos)

  \brief Add a GtkWidget in a GtkLayout

  \param layout the GtkLayout
  \param child the GtkWidget to insert
  \param x_pos the x position to insert at
  \param y_pos the y position to insert at
*/
void layout_add_widget (GtkWidget * layout, GtkWidget * child, int x_pos, int y_pos)
{
#ifdef GTK3
  gtk_layout_put (GTK_LAYOUT(layout), child, x_pos, y_pos);
#else
  gtk_fixed_put (GTK_FIXED(layout), child, x_pos, y_pos);
#endif
}

/*!
  \fn GtkWidget * add_vbox_to_layout (GtkWidget * layout, int size_x, int size_y)

  \brief Insert a vertical GtkBox in a GtkLatout then send back the GtkBox

  \param layout the GtkLayout
  \param size_x the x size of the box
  \param size_y the y size of the box
*/
GtkWidget * add_vbox_to_layout (GtkWidget * layout, int size_x, int size_y)
{
  GtkWidget * vbox = create_vbox (BSEP);
  gtk_widget_set_size_request (vbox, size_x, size_y);
  layout_add_widget (layout, vbox, 0, 5);
  return vbox;
}

/*!
  \fn GtkWidget * create_layout (int x, int y)

  \brief create a GtkLayout / GtkFixed widget

  \param x the x size of the widget
  \param y the y size of the widget
*/
GtkWidget * create_layout (int x, int y)
{
  GtkWidget * layout;
#ifdef GTK3
  layout = gtk_layout_new (NULL, NULL);
#else
  layout = gtk_fixed_new ();
#endif
  gtk_widget_set_size_request (layout, x, y);
  return layout;
}

/*!
  \fn int combo_get_active (GtkWidget * combo)

  \brief retrieve the active item's position

  \param combo the target GtkWidget
*/
int combo_get_active (GtkWidget * combo)
{
  return gtk_combo_box_get_active ((GtkComboBox *)combo);
}
/* #ifdef GTK4
#if GTK_MINOR_VERSION > 9
  return gtk_drop_down_get_selected (GtkDropDown *)combo);
#else
  return gtk_combo_box_get_active ((GtkComboBox *)combo);
#endif
#else
  return gtk_combo_box_get_active ((GtkComboBox *)combo);
#endif // GTK4
} */

/*!
  \fn void combo_set_active (GtkWidget * combo, int pos)

  \brief set the active item's position

  \param combo the target GtkWidget
  \param pos the position to set
*/
void combo_set_active (GtkWidget * combo, int pos)
{
  gtk_combo_box_set_active ((GtkComboBox *)combo, pos);
}

/*!
  \fn void combo_set_markup (GtkWidget * combo)

  \brief use pango markup in combo widget

  \param combo to the target widget
*/
void combo_set_markup (GtkWidget * combo)
{
  GList * cell_list = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(combo));
  if (cell_list && cell_list -> data)
  {
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), cell_list -> data, "markup", 0, NULL);
  }
}

/*!
  \fn void combo_text_append (GtkWidget * combo, gchar * text)

  \brief append text in GtkComboBox widget

  \param combo the GtkWidget
  \param text the text to append
*/
void combo_text_append (GtkWidget * combo, gchar * text)
{
  gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo), text);
}

/*!
  \fn void combo_text_prepend (GtkWidget * combo, gchar * text)

  \brief prepend text in GtkComboBox widget

  \param combo the GtkWidget
  \param text the text to prepend
*/
void combo_text_prepend (GtkWidget * combo, gchar * text)
{
  gtk_combo_box_text_prepend_text (GTK_COMBO_BOX_TEXT(combo), text);
}

/*!
  \fn GtkWidget * create_combo ()

  \brief create a GtkCombox widget, note deprecated in GTK4
*/
GtkWidget * create_combo ()
{
  return gtk_combo_box_text_new ();
}

/*!
  \fn void setup_text_tags (GtkTextBuffer * buffer)

  \brief prepare the available text tags for the GtkTextBuffer

  \param buffer the GtkTextBuffer
*/
void setup_text_tags (GtkTextBuffer * buffer)
{
  GtkTextTagTable * textags = gtk_text_buffer_get_tag_table(buffer);

  /* Sans font */
  tag = gtk_text_tag_new ("sans");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "family", "sans", NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* Monospace font */
  tag = gtk_text_tag_new ("monospace");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "family", "monospace", NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* Default Font size */
  tag = gtk_text_tag_new ("default-size");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* <u> */
  tag = gtk_text_tag_new ("underline");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "underline", PANGO_UNDERLINE_SINGLE, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* <uu> */
  tag = gtk_text_tag_new ("underline_double");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "underline", PANGO_UNDERLINE_DOUBLE, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* <i> italic */
  tag = gtk_text_tag_new ("italic");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "style", PANGO_STYLE_ITALIC, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* <b> bold */
  tag = gtk_text_tag_new ("bold");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

   /* <b> bold italic */
  tag = gtk_text_tag_new ("bold_italic");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "style", PANGO_STYLE_ITALIC,
                     "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* <sup> superscript */
  tag = gtk_text_tag_new ("sup");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "scale", .6, "rise", 6000, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* <sub> subscript */
  tag = gtk_text_tag_new ("sub");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "scale", .6, "rise", -6000, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* justify-left */
  tag = gtk_text_tag_new("justify-left");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "justification", GTK_JUSTIFY_LEFT, NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref(tag);

  /* justify-center */
  tag = gtk_text_tag_new("justify-center");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "justification", GTK_JUSTIFY_CENTER, NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref(tag);

  /* justify-right */
  tag = gtk_text_tag_new("justify-right");
  g_object_set(tag, "justification", GTK_JUSTIFY_RIGHT, NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* heading */
  tag = gtk_text_tag_new("heading");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                    "weight", PANGO_WEIGHT_BOLD,
                    "scale", 1.5,
                    "justification", GTK_JUSTIFY_CENTER,
                    "underline", PANGO_UNDERLINE_DOUBLE, NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* red font */
  tag = gtk_text_tag_new("red");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "red", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* orange font */
  tag = gtk_text_tag_new("orange");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "orange", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

   /* yellow font */
  tag = gtk_text_tag_new("yellow");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "yellow2", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

   /* green font */
  tag = gtk_text_tag_new("green");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "green", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* light green font */
  tag = gtk_text_tag_new("light_green");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "light green", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* cyan font */
  tag = gtk_text_tag_new("cyan");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "cyan", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* blue font */
  tag = gtk_text_tag_new("blue");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "blue", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* pink font */
  tag = gtk_text_tag_new("pink");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "pink", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* violet font */
  tag = gtk_text_tag_new("violet");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "foreground", "dark violet", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* grey background */
  tag = gtk_text_tag_new("grey_back");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE, "background", "lightgrey", NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* bold grey background */
  tag = gtk_text_tag_new("bold_grey_back");
  g_object_set(tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                    "background", "lightgrey",
                    "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add(textags, tag);
  g_object_unref (tag);

  /* bold red font */
  tag = gtk_text_tag_new ("bold_red");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "foreground", "red",
                     "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* bold blue font */
  tag = gtk_text_tag_new ("bold_blue");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "foreground", "blue",
                     "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* bold green font */
  tag = gtk_text_tag_new ("bold_green");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "foreground", "green",
                     "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* bold orange font */
  tag = gtk_text_tag_new ("bold_orange");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "foreground", "orange",
                     "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* bold pink font */
  tag = gtk_text_tag_new ("bold_pink");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "foreground", "pink",
                     "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

  /* subscript italic*/
  tag = gtk_text_tag_new ("sub_italic");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "style", PANGO_STYLE_ITALIC,
                     "scale", .6, "rise", -6000, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

   /* subscript bold */
  tag = gtk_text_tag_new ("sub_bold");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "weight", PANGO_WEIGHT_BOLD,
                     "scale", .6, "rise", -6000, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);

   /* superscript bold */
  tag = gtk_text_tag_new ("sup_bold");
  g_object_set (tag, "size", DEFAULT_FONT_SIZE * PANGO_SCALE,
                     "weight", PANGO_WEIGHT_BOLD,
                     "scale", .6, "rise", 6000, NULL);
  gtk_text_tag_table_add (textags, tag);
  g_object_unref (tag);
}

/*!
  \fn GtkTextBuffer * add_buffer (GCallback handler, gpointer data, gchar * text)

  \brief create a GtkTextBuffer

  \param handler the callback on buffer changed
  \param data the associated data pointer
  \param text the text to display
*/
GtkTextBuffer * add_buffer (GCallback handler, gpointer data, gchar * text)
{
  GtkTextBuffer * buffer;
  GtkTextIter bStart;
  GtkTextIter bEnd;
  buffer = gtk_text_buffer_new (NULL);
  setup_text_tags (buffer);
  gtk_text_buffer_get_start_iter (buffer, &bStart);
  gtk_text_buffer_get_end_iter (buffer, &bEnd);
  gtk_text_buffer_delete (buffer, &bStart, &bEnd);
  if (text != NULL) gtk_text_buffer_set_text (buffer, text, -1);
  if (handler != NULL) g_signal_connect (G_OBJECT (buffer), "changed", handler, data);
  return buffer;
}

/*!
  \fn GtkWidget * create_text_view (int dimx, int dimy, int edit, int mono, GCallback handler, gpointer data, gchar * text)

  \brief create a GtkTextView and display some text

  \param dimx the x size of the GtkTextView
  \param dimy the y size of the GtkTextView
  \param edit Is the text view editable (1/0)
  \param mono Use monospace font (1/0)
  \param handler the callback on buffer changed
  \param data the associated data pointer
  \param text the text to display
*/
GtkWidget * create_text_view (int dimx, int dimy, int edit, int mono, GCallback handler, gpointer data, gchar * text)
{
  GtkWidget * tview = gtk_text_view_new ();
  gtk_text_view_set_buffer (GTK_TEXT_VIEW(tview), add_buffer(handler, data, text));
  gtk_widget_set_size_request (tview, dimx, dimy);
  gtk_text_view_set_editable (GTK_TEXT_VIEW(tview), edit);
  if (mono) text_view_set_monospace (tview);
  return tview;
}

/*!
  \fn static void convert_alpha (cairo_surface_t * surf, GdkPixbuf * pix, int src_x, int src_y, int width, int height)

  \brief convert a cairo surface to pixbuf

  \param surf the cairo surface to convert
  \param pix the resulting GdkPixBuf
  \param src_x Shift on x, if any
  \param src_y Shift on y, if any
  \param width Image width
  \param height Image height
*/
static void convert_alpha (cairo_surface_t * surf, GdkPixbuf * pix, int src_x, int src_y, int width, int height)
{
  int x, y;
  guchar * src_data = cairo_image_surface_get_data (surf);
  int src_stride = cairo_image_surface_get_stride (surf);
  guchar * dest_data = gdk_pixbuf_get_pixels (pix);
  int dest_stride = gdk_pixbuf_get_rowstride (pix);

  src_data += src_stride * src_y + src_x * 4;

  for (y = 0; y < height; y++) {
    guint32 *src = (guint32 *) src_data;

    for (x = 0; x < width; x++) {
      guint alpha = src[x] >> 24;

      if (alpha == 0)
        {
          dest_data[x * 4 + 0] = 0;
          dest_data[x * 4 + 1] = 0;
          dest_data[x * 4 + 2] = 0;
        }
      else
        {
          dest_data[x * 4 + 0] = (((src[x] & 0xff0000) >> 16) * 255 + alpha / 2) / alpha;
          dest_data[x * 4 + 1] = (((src[x] & 0x00ff00) >>  8) * 255 + alpha / 2) / alpha;
          dest_data[x * 4 + 2] = (((src[x] & 0x0000ff) >>  0) * 255 + alpha / 2) / alpha;
        }
      dest_data[x * 4 + 3] = alpha;
    }

    src_data += src_stride;
    dest_data += dest_stride;
  }
}

/*!
  \fn GdkPixbuf * convert_to_pixbuf (cairo_surface_t * surf)

  \brief convert cairo surface to GdkPixbuf

  \param surf the cairo sufrace to convert
*/
GdkPixbuf * convert_to_pixbuf (cairo_surface_t * surf)
{
  int width  = cairo_image_surface_get_width (surf);
  int height = cairo_image_surface_get_height (surf);
  GdkPixbuf * pix = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, width , height);
  convert_alpha (surf, pix, 0, 0, width, height);
  return pix;
}

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT gboolean to_activate_entry (GtkWidget * widg, GdkEventFocus * event, gpointer data)

  \brief GtkEntry activate signal managment

  \param widg the GtkWidget sending the signal
  \param event the associated focus event
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean to_activate_entry (GtkWidget * widg, GdkEventFocus * event, gpointer data)
{
  g_signal_emit_by_name (G_OBJECT(widg), "activate", data);
  return FALSE;
}
#else
/*!
  \fn G_MODULE_EXPORT void to_activate_entry (GtkEventControllerFocus * focus, gpointer data)

  \brief GtkEntry activate signal managment

  \param focus Focus event controller
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_activate_entry (GtkEventControllerFocus * focus, gpointer data)
{
  g_signal_emit_by_name (G_OBJECT(gtk_event_controller_get_widget((GtkEventController *)focus)), "activate", data);
}
#endif

/*!
  \fn GtkWidget * create_entry (GCallback handler, int dim, int cdim, gboolean key_release, gpointer data)

  \brief Create a GtkEntry

  \param handler the callback
  \param dim the x size
  \param cdim the maximum number of character to display
  \param key_release Use release event (1/0)
  \param data the data pointer for the callback
*/
GtkWidget * create_entry (GCallback handler, int dim, int cdim, gboolean key_release, gpointer data)
{
  GtkWidget * entry = gtk_entry_new ();
  gtk_widget_set_size_request (entry, dim, -1);
#ifdef GTK4
  gtk_editable_set_width_chars (GTK_EDITABLE(entry), 0);
  gtk_editable_set_max_width_chars (GTK_EDITABLE(entry), cdim);
#else
  gtk_entry_set_width_chars (GTK_ENTRY(entry), 0);
  gtk_entry_set_max_width_chars (GTK_ENTRY(entry), cdim);
#endif
  gtk_entry_set_alignment (GTK_ENTRY(entry), 1.0);

  if (handler != NULL)
  {
    g_signal_connect (G_OBJECT (entry), "activate", handler, data);
#ifdef GTK3
    g_signal_connect (G_OBJECT (entry), "focus-out-event", G_CALLBACK(to_activate_entry), data);
#else
 /* Pour ajouter une icone dans l'entry:
    gtk_entry_set_icon_from_icon_name (GTK_ENTRY (entry), GTK_ENTRY_ICON_PRIMARY, "edit-find-symbolic");
    // Set up the search icon
    gtk_entry_set_icon_activatable (GTK_ENTRY (entry), GTK_ENTRY_ICON_PRIMARY, TRUE);
    gtk_entry_set_icon_sensitive (GTK_ENTRY (entry), GTK_ENTRY_ICON_PRIMARY, TRUE);
    // Then the press can have action see gtk4 demo
    g_signal_connect (entry, "icon-press", G_CALLBACK(icon_press_cb), NULL); */
    GtkEventController * focus = gtk_event_controller_focus_new ();
    gtk_event_controller_set_name (focus, "focus-out");
    g_signal_connect (G_OBJECT(focus), "leave", G_CALLBACK(to_activate_entry), data);
    gtk_widget_add_controller (entry, focus);
#endif
  }
  if (key_release)
  {
#ifdef GTK3
     g_signal_connect (G_OBJECT (entry), "key-release-event", G_CALLBACK(to_activate_entry), data);
#else
     // Key press controler
#endif
  }
  return entry;
}

/*!
  \fn GtkWidget * stock_image (const gchar * stock_id)

  \brief create a GtkImage for the Gtk database

  \param stock_id
*/
GtkWidget * stock_image (const gchar * stock_id)
{
#ifdef GTK4
  return gtk_image_new_from_icon_name (stock_id);
#else
  return gtk_image_new_from_icon_name (stock_id, GTK_ICON_SIZE_MENU);
#endif
}

#ifdef GTK3
/*!
  \fn GtkWidget * create_menu_item (gboolean add_mnemo, gchar * name)

  \brief create a GtkMenuItem and set mnemonic

  \param add_mnemo Use mnemonic (1/0)
  \param name the name of the menu item
*/
GtkWidget * create_menu_item (gboolean add_mnemo, gchar * name)
{
  return (add_mnemo) ? gtk_menu_item_new_with_mnemonic (name) : gtk_menu_item_new_with_label (name);
}

/*!
  \fn GtkWidget * create_menu_item_from_widget (GtkWidget * widg, gboolean check, gboolean radio, gboolean status)

  \brief create a GtkMenuItem using a GtkWidget as label

  \param widg the GtkWidget to use
  \param check Create a check menu item (1/0)
  \param radio Create a radio menu item (1/0)
  \param status If check or radio status (1/0)
*/
GtkWidget * create_menu_item_from_widget (GtkWidget * widg, gboolean check, gboolean radio, gboolean status)
{
  GtkWidget * item;
  if (check)
  {
    item = gtk_check_menu_item_new ();
    if (radio) gtk_check_menu_item_set_draw_as_radio (GTK_CHECK_MENU_ITEM(item), TRUE);
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(item), status);
  }
  else
  {
    item = gtk_menu_item_new ();
  }
  gtk_container_add (GTK_CONTAINER(item), widg);
  return item;
}
#endif

/* GIcon*
g_icon_new_for_string (
  const gchar* str,
  GError** error
)

void
g_menu_item_set_icon (
  GMenuItem* menu_item,
  GIcon* icon
)

void
g_menu_item_set_attribute (
  GMenuItem* menu_item,
  const gchar* attribute,
  const gchar* format_string,
  ...
) */

/*!
  \fn GtkWidget * create_image_from_data (int format, gpointer item_image)

  \brief create Gtk image for data

  \param format Image format
  \param item_image the data to create the image
*/
GtkWidget * create_image_from_data (int format, gpointer item_image)
{
  GtkWidget * icon = NULL;
  switch (format)
  {
    case IMG_PIXBUF:
      icon = gtk_image_new_from_pixbuf ((GdkPixbuf*)item_image);
      break;
    case IMG_SURFACE:
#ifdef GTK4
      icon = gtk_image_new_from_pixbuf (convert_to_pixbuf ((cairo_surface_t *)item_image));
#else
      icon = gtk_image_new_from_surface ((cairo_surface_t *)item_image);
#endif
      break;
    case IMG_FILE:
      icon = gtk_image_new_from_file ((const gchar *)item_image);
      break;
    case IMG_STOCK:
      icon = stock_image ((const gchar *)item_image);
      break;
  }
  return icon;
}

#ifdef GTK3
/*!
  \fn GtkWidget * gtk3_menu_item (GtkWidget * menu, gchar * name,
                                  int icon_format, gpointer item_icon,
                                  GCallback handler, gpointer data,
                                  gboolean accel, guint key, GdkModifierType mod,
                                  gboolean check, gboolean radio, gboolean status)

  \brief create a GT3 menu item, and insert it in a menu, if any

  \param menu the menu to insert the menu item in
  \param name the name of the menu item
  \param icon_format the image format of the icon for the menu item
  \param item_icon the data for the image of the icon
  \param handler the callback for the menu item
  \param data the associated data pointer
  \param accel Keyboard short cut -1/0)
  \param key the key to press
  \param mod the modifier in any (Ctrl, Alt ...)
  \param check Create a check menu item (1/0)
  \param radio Create a radio menu item (1/0)
  \param status If check or radio status (1/0)
*/
GtkWidget * gtk3_menu_item (GtkWidget * menu, gchar * name,
                            int icon_format, gpointer item_icon,
                            GCallback handler, gpointer data,
                            gboolean accel, guint key, GdkModifierType mod,
                            gboolean check, gboolean radio, gboolean status)
{
  GtkWidget * item = NULL;
  GtkWidget * icon = NULL;
  GtkWidget * lab = NULL;

  if (name)
  {
    if (accel)
    {
      lab = gtk_accel_label_new (name);
      gtk_label_align (lab, 0.0, 0.5);
      gtk_widget_set_size_request (lab, -1, -1);
      gtk_label_set_use_markup (GTK_LABEL(lab), TRUE);
      gtk_accel_label_set_accel ((GtkAccelLabel *)lab, key, mod);
    }
    else
    {
      lab = markup_label(name, -1, -1, 0.0, 0.5);
    }
  }
  if (icon_format != IMG_NONE) icon = create_image_from_data (icon_format, item_icon);
  if (name)
  {
#ifdef MENU_ICONS
    if (icon)
    {
      GtkWidget * box = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, icon, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, lab, FALSE, FALSE, (icon_format == IMG_SURFACE) ? 0 : 6);
      item = create_menu_item_from_widget (box, check, radio, status);
    }
    else
    {
      item = create_menu_item_from_widget (lab, check, radio, status);
    }
#else
    item = create_menu_item_from_widget (lab, check, radio, status);
#endif
  }
  else if (icon)
  {
    item = create_menu_item_from_widget (icon, check, radio, status);
  }
  if (handler != NULL) g_signal_connect (G_OBJECT(item), "activate", handler, data);
  if (menu != NULL) gtk_menu_shell_append ((GtkMenuShell *)menu, item);
  return item;
}

/*!
  \fn GtkWidget * add_advanced_item (GtkWidget * menu, GCallback handler, gpointer data, gboolean accel, guint key, GdkModifierType mod)

  \brief short cut to create and insert a menu item with the "Advanced" text and a stock icon

  \param menu the menu to insert the menu item
  \param handler the callback for the menu item
  \param data the associated data pointer
  \param accel Keyboard short cut -1/0)
  \param key the key to press
  \param mod the modifier in any (Ctrl, Alt ...)
*/
GtkWidget * add_advanced_item (GtkWidget * menu, GCallback handler, gpointer data, gboolean accel, guint key, GdkModifierType mod)
{
  return gtk3_menu_item (menu, "Advanced", IMG_STOCK, (gpointer)DPROPERTIES, handler, data, accel, key, mod, FALSE, FALSE, FALSE);
}

/*!
  \fn void add_menu_separator (GtkWidget * menu)

  \brief add sepator in a menu

  \param menu the GtkMenu
*/
void add_menu_separator (GtkWidget * menu)
{
  gtk_menu_shell_append ((GtkMenuShell *)menu, gtk_separator_menu_item_new ());
}
#endif

/*!
  \fn GtkWidget * markup_label (gchar * text, int dimx, int dimy, float ax, float ay)

  \brief create a GtkLabel with pango markup

  \param text Message to display
  \param dimx y size for the widget
  \param dimy y size for the widget
  \param ax x alignment
  \param ay y alignment
*/
GtkWidget * markup_label (gchar * text, int dimx, int dimy, float ax, float ay)
{
  GtkWidget * lab = gtk_label_new (text);
  gtk_label_align (lab, ax, ay);
  gtk_widget_set_size_request (lab, dimx, dimy);
  gtk_label_set_use_markup (GTK_LABEL(lab), TRUE);
  return lab;
}

/*!
  \fn ColRGBA * duplicate_color (int num, ColRGBA * col)

  \brief duplicate a ColRGBA pointer

  \param num Size of the pointer
  \param col ColRGBA pointer to duplicate
*/
ColRGBA * duplicate_color (int num, ColRGBA * col)
{
  ColRGBA * new_col = g_malloc0 (num*sizeof*new_col);
  int i;
  for (i=0; i<num; i++) new_col[i] = col[i];
  return new_col;
}

/*!
  \fn ColRGBA gdkrgba_to_rgba (GdkRGBA colgdk)

  \brief convert GdkRGBA color to ColRGBA color

  \param colgdk the GdkRGBA to convert
*/
ColRGBA gdkrgba_to_rgba (GdkRGBA colgdk)
{
  ColRGBA col;
  col.red = colgdk.red;
  col.green = colgdk.green;
  col.blue = colgdk.blue;
  col.alpha = colgdk.alpha;
  return col;
}

/*!
  \fn GdkRGBA colrgba_togtkrgba (ColRGBA col)

  \brief convert ColRGBA color to GdkRGBA color

  \param col the ColRGBA to convert
*/
GdkRGBA colrgba_togtkrgba (ColRGBA col)
{
  GdkRGBA colo;
  colo.red = col.red;
  colo.green = col.green;
  colo.blue = col.blue;
  colo.alpha = col.alpha;
  return colo;
}

/*!
  \fn void set_renderer_color (int tocol, GtkCellRenderer * renderer, ColRGBA col)

  \brief set the color of a GtkCellRenderer

  \param tocol Apply color (1/0)
  \param renderer the GtkCellRenderer
  \param col the color to apply
*/
void set_renderer_color (int tocol, GtkCellRenderer * renderer, ColRGBA col)
{
  if (tocol)
  {
    GdkRGBA colo = colrgba_togtkrgba (col);
    g_object_set (renderer, "foreground-rgba", & colo, "foreground-set", TRUE, NULL);
    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD, "weight-set", TRUE, NULL);
  }
  else
  {
    g_object_set(renderer, "foreground-set", FALSE, "weight", FALSE, NULL);
  }
}

/*!
  \fn void button_set_image (GtkButton * but, gchar * text, int format, gpointer image)

  \brief Add an image to a GtkButton

  \param but the GtkButton
  \param text the message to display
  \param format the image format
  \param image the data of the image
*/
void button_set_image (GtkButton * but, gchar * text, int format, gpointer image)
{
#ifdef GTK4
  GtkWidget * img = create_image_from_data (format, image);
  if (text)
  {
    GtkWidget * hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label (text, -1, -1, 0.0, 0.5), FALSE, FALSE, 0);
    gtk_button_set_child (but, hbox);
  }
  else
  {
    gtk_button_set_child (but, img);
  }
#else
  gtk_button_set_image (but, create_image_from_data (format, image));
#endif
}

/*!
  \fn void adjust_label (GtkWidget * lab, int dimx, int dimy, float ax, float ay)

  \brief adjust the text position, and widget size of a GtkLabel

  \param lab the GtkLabel to adjust
  \param dimx the x size
  \param dimy the y size
  \param ax the x alignment
  \param ay the y alignment
*/
void adjust_label (GtkWidget * lab, int dimx, int dimy, float ax, float ay)
{
  gtk_label_set_xalign (GTK_LABEL(lab), ax);
  gtk_label_set_yalign (GTK_LABEL(lab), ay);
  gtk_widget_set_size_request (lab, dimx, dimy);
  gtk_label_set_use_markup (GTK_LABEL(lab), TRUE);
}

/*!
  \fn GtkWidget * color_button (ColRGBA col, gboolean alpha, int dimx, int dimy, GCallback handler, gpointer data)

  \brief create a color selection button

  \param col the color to use
  \param alpha Use alpha channel (1/0)
  \param dimx Size x of the button
  \param dimy Size y of the button
  \param handler the callback for the button
  \param data the associated data pointer
*/
GtkWidget * color_button (ColRGBA col, gboolean alpha, int dimx, int dimy, GCallback handler, gpointer data)
{
  GtkWidget * colorb;
  GdkRGBA colo = colrgba_togtkrgba (col);
  colorb = gtk_color_button_new_with_rgba (& colo);
  gtk_color_chooser_set_use_alpha (GTK_COLOR_CHOOSER(colorb), alpha);
  if (handler != NULL) g_signal_connect (G_OBJECT(colorb), "color-set", handler, data);
  gtk_widget_set_size_request (colorb, dimx, dimy);
  return colorb;
}

/*!
  \fn GtkWidget * font_button (gchar * font, int dimx, int dimy, GCallback handler, gpointer data)

  \brief create a font selection button

  \param font the font to use
  \param dimx Size x of the button
  \param dimy Size y of the button
  \param handler the callback for the button
  \param data the associated data pointer
*/
GtkWidget * font_button (gchar * font, int dimx, int dimy, GCallback handler, gpointer data)
{
  GtkWidget * fontb = gtk_font_button_new_with_font (font);
  g_signal_connect (G_OBJECT(fontb), "font-set", handler, data);
  gtk_widget_set_size_request (fontb, dimx, dimy);
  return fontb;
}

/*!
  \fn GtkWidget * spin_button (GCallback handler, double value, double start, double end, double step, int digits, int dim,  gpointer data)

  \brief create a spin button

  \param handler the callback for the button
  \param value Initial value for the spin button
  \param start Minimum value
  \param end Maximum value
  \param step Step between values
  \param digits Number of digits displayed
  \param dim Size x of the button
  \param data the associated data pointer
*/
GtkWidget * spin_button (GCallback handler, double value, double start, double end, double step, int digits, int dim,  gpointer data)
{
  GtkWidget * spin = gtk_spin_button_new_with_range (start, end, step);
  gtk_spin_button_set_digits (GTK_SPIN_BUTTON(spin), digits);
#ifdef GTK4
  gtk_editable_set_alignment (GTK_EDITABLE(spin), 1.0);
#else
  gtk_entry_set_alignment (GTK_ENTRY(spin), 1.0);
#endif
  gtk_spin_button_set_value (GTK_SPIN_BUTTON(spin), value);
  gtk_widget_set_size_request (spin, dim, -1);
  if (handler != NULL) g_signal_connect (G_OBJECT (spin), "value-changed", handler, data);
  return spin;
}

/*!
  \fn int button_get_status (GtkWidget * button)

  \brief get status of check / toggle button

  \param button the button to check
*/
int button_get_status (GtkWidget * button)
{
#ifdef GTK4
  return gtk_check_button_get_active (GTK_CHECK_BUTTON(button));
#else
  return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));
#endif
}

/*!
  \fn void button_set_status (GtkWidget * button, int status)

  \brief set status of check / toggle button

  \param button the button to update
  \param status the new status
*/
void button_set_status (GtkWidget * button, int status)
{
#ifdef GTK4
  gtk_check_button_set_active (GTK_CHECK_BUTTON(button), status);
#else
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(button), status);
#endif
}

/*!
  \fn GtkWidget * check_button (gchar * text, int dimx, int dimy, gboolean state, GCallback handler, gpointer data)

  \brief create a check button

  \param text the message to display
  \param dimx the x size
  \param dimy the y size
  \param state Checked or not (1/0)
  \param handler the callback for the button
  \param data the associated data pointer
*/
GtkWidget * check_button (gchar * text, int dimx, int dimy, gboolean state, GCallback handler, gpointer data)
{
  GtkWidget * but = gtk_check_button_new ();
  if (text != NULL)
  {
    gchar * label = g_strdup_printf (" %s", text);
#ifdef GTK4
    gtk_check_button_set_label (GTK_CHECK_BUTTON(but), label);
    GtkWidget * lab = gtk_widget_get_last_child (but);
    adjust_label (lab, -1, -1, 0.0, 0.5);
#else
    add_container_child (CONTAINER_BUT, but, markup_label(label, -1, -1, 0.0, 0.5));
#endif
    g_free (label);
  }
  gtk_widget_set_size_request (but, dimx, dimy);
  button_set_status (but, state);
  if (handler != NULL) g_signal_connect (G_OBJECT(but), "toggled", handler, data);
  return but;
}

/*!
  \fn GtkWidget * radio_button (gchar * text, int dimx, int dimy, gboolean state, GCallback handler, gpointer data)

  \brief create a radio button

  \param text the message to display
  \param dimx the x size
  \param dimy the y size
  \param state Checked or not (1/0)
  \param handler the callback for the button
  \param data the associated data pointer
*/
GtkWidget * radio_button (gchar * text, int dimx, int dimy, gboolean state, GCallback handler, gpointer data)
{
  GtkWidget * but;
#ifdef GTK4
  but = gtk_toggle_button_new ();
#else
  but = gtk_radio_button_new (NULL);
#endif
  if (text != NULL) add_container_child (CONTAINER_BUT, but, markup_label(text, -1, -1, 0.0, 0.5));
  gtk_widget_set_size_request (but, dimx, dimy);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(but), state);
  if (handler != NULL) g_signal_connect (G_OBJECT(but), "toggled", handler, data);
  return but;
}

/*!
  \fn GtkWidget * create_button (gchar * text, int image_format, gchar * image, int dimx, int dimy, int relief, GCallback handler, gpointer data)

  \brief create a simple button

  \param text the message to display
  \param image_format the image format
  \param image the name of the stock image
  \param dimx the x size
  \param dimy the y size
  \param relief button should appear pressed or not (1/0)
  \param handler the callback for the button
  \param data the associated data pointer
*/
GtkWidget * create_button (gchar * text, int image_format, gchar * image, int dimx, int dimy, int relief, GCallback handler, gpointer data)
{
  GtkWidget * but;
#ifdef GTK3
  but = (text) ? gtk_button_new_with_label (text) : gtk_button_new ();
#else
  but = (text && image_format == IMG_NONE) ? gtk_button_new_with_label (text) : gtk_button_new ();
#endif
  if (image_format != IMG_NONE) button_set_image (GTK_BUTTON(but), text, image_format, (gpointer)image);
  gtk_widget_set_size_request (but, dimx, dimy);
#ifdef GTK3
  gtk_button_set_relief (GTK_BUTTON(but), relief);
#endif
#ifdef GTK4
  if (relief == GTK_RELIEF_NONE) gtk_button_set_has_frame (GTK_BUTTON(but), FALSE);
#endif
  if (handler != NULL)
  {
    g_signal_connect (G_OBJECT (but), "clicked", handler, data);
  }
  return but;
}

/*!
  \fn void set_image_from_icon_name (GtkWidget * widg, gchar * icon)

  \brief set a image from a stock icon name

  \param widg the GtkImage to modify
  \param icon the name of the stock icon
*/
void set_image_from_icon_name (GtkWidget * widg, gchar * icon)
{
#ifdef GTK4
  gtk_image_set_from_icon_name (GTK_IMAGE(widg), icon);
#else
  gtk_image_set_from_icon_name (GTK_IMAGE(widg), icon, GTK_ICON_SIZE_BUTTON);
#endif
}

/*!
  \fn void append_comments (GtkWidget * vbox, gchar * symbol, gchar * legend)

  \brief append comments to a vertical box

  \param vbox the target vertical box
  \param symbol the symbol to legend
  \param legend the associated legend
*/
void append_comments (GtkWidget * vbox, gchar * symbol, gchar * legend)
{
  GtkWidget * hbox = create_hbox (BSEP);
  gchar * str = g_strdup_printf ("<i><sub>%s</sub></i>", legend);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(symbol, 15, -1, 1.0, 0.5) , FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, -1, -1, 0.0, 0.5) , FALSE, FALSE, 5);
  g_free (str);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
}

/*!
  \fn GtkWidget * abox (GtkWidget * box, char * lab, int vspace)

  \brief box creating routine, to help design faster elements for the GUI

  \param box the box to insert the new box into
  \param lab string to display
  \param vspace vertical space
*/
GtkWidget * abox (GtkWidget * box, char * lab, int vspace)
{
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, vspace);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>.</b>", 5, -1, 0.0, 0.25), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(lab, 150, 30, 0.0, 0.5), FALSE, FALSE, 0);
  return hbox;
}

/*!
  \fn GtkWidget * bbox (GtkWidget * box, char * lab)

  \brief box creating routine, to help design faster elements for the GUI

  \param box the box to insert the new box into
  \param lab string to display
*/
GtkWidget * bbox (GtkWidget * box, char * lab)
{
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 2);
  GtkWidget * hhbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, FALSE, FALSE, 40);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label(lab, 150, 30, 0.0, 0.5), FALSE, FALSE, 0);
  return hhbox;
}

/*!
  \fn GtkWidget * cbox (GtkWidget * box, char * lab)

  \brief box creating routine, to help design faster elements for the GUI

  \param box the box to insert the new box into
  \param lab string to display
*/
GtkWidget * cbox (GtkWidget * box, char * lab)
{
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, TRUE, TRUE, 2);
  GtkWidget * hhbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, TRUE, TRUE, 40);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, markup_label(lab, -1, 30, 0.0, 0.5), TRUE, TRUE, 40);
  return hhbox;
}

/*!
  \fn GtkWidget * fbox (GtkWidget * box, char * lab)

  \brief box creating routine, to help design faster elements for the GUI

  \param box the box to insert the new box into
  \param lab string to display
*/
GtkWidget * fbox (GtkWidget * box, char * lab)
{
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>.</b>", 5, -1, 0.0, 0.5), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(lab, 350, 30, 0.0, 0.5), FALSE, FALSE, 0);
  return hbox;
}

/*!
  \fn GtkWidget * create_scroll (GtkWidget * box, int dimx, int dimy, int shadow)

  \brief create a scroll window

  \param box the box to insert the scroll window in, if any
  \param dimx the x size of the widget
  \param dimy the x size of the widget
  \param shadow Add shadow (1/0)
*/
GtkWidget * create_scroll (GtkWidget * box, int dimx, int dimy, int shadow)
{
  GtkWidget * scroll;
#ifdef GTK4
  scroll = gtk_scrolled_window_new ();
#else
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scroll), shadow);
#endif
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request (scroll, dimx, dimy);
  // To check all create_scroll !
  if (box != NULL) add_box_child_start (GTK_ORIENTATION_VERTICAL, box, scroll, TRUE, TRUE, 0);
  return scroll;
}

/*!
  \fn GtkWidget * create_expander (gchar * name, gchar * file_img)

  \brief create GtkExpander

  \param name Name of the expander tab
  \param file_img image file to add if required
*/
GtkWidget * create_expander (gchar * name, gchar * file_img)
{
  GtkWidget * expand = gtk_expander_new (name);
  GtkWidget * hbox = create_hbox (0);
  if (file_img != NULL)
  {
    GtkWidget * img = gtk_image_new_from_file (file_img);
    gtk_widget_set_size_request (img, 20, 20);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, img, TRUE, TRUE, 10);
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(name, 200, 20, 0.0, 0.5), FALSE, TRUE, 0);
  gtk_expander_set_label_widget (GTK_EXPANDER(expand), hbox);
  return expand;
}

/*!
  \fn void provide_gtk_css (gchar * css)

  \brief create a css provider based on the css data

  \param css the css data, use the name of the variable to use the css
*/
void provide_gtk_css (gchar * css)
{
  GtkCssProvider * provider = gtk_css_provider_new ();
#ifdef GTK4
#if GTK_MINOR_VERSION < 12
  gtk_css_provider_load_from_data (provider, css, -1);
#else
  gtk_css_provider_load_from_string (provider, css);
#endif
  gtk_style_context_add_provider_for_display (gdk_display_get_default (),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
#else
  gtk_css_provider_load_from_data (provider, css, -1, NULL);
  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
                                             GTK_STYLE_PROVIDER(provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
#endif
  g_object_unref (provider);
}

/*!
  \fn GtkWidget * destroy_this_widget (GtkWidget * widg)

  \brief destroy a GtkWidget

  \param widg the GtkWidget to destroy
*/
GtkWidget * destroy_this_widget (GtkWidget * widg)
{
  if (widg != NULL)
  {
    if (GTK_IS_WIDGET(widg))
    {
      if (is_the_widget_visible(widg)) hide_the_widgets (widg);
#ifdef GTK3
      gtk_widget_destroy (widg);
#else
      /* GtkWidget * wid = gtk_widget_get_parent (widg);
      if (wid != NULL)
      {
        if (GTK_IS_WIDGET(wid))
        {
          gtk_widget_unparent (widg);
        }
      } */
#endif
    }
  }
  return NULL;
}

/*!
  \fn void destroy_this_dialog (GtkDialog * dialog)

  \brief destroy a GtkDialog

  \param dialog the GtkDialog to destroy
*/
void destroy_this_dialog (GtkDialog * dialog)
{
#ifdef GTK4
  gtk_window_destroy (GTK_WINDOW(dialog));
#else
  destroy_this_widget (GTK_WIDGET(dialog));
#endif
  g_main_loop_quit (Event_loop[dialog_id]);
  g_main_loop_unref (Event_loop[dialog_id]);
  dialog_id --;
}

/*!
  \fn void destroy_this_native_dialog (GtkNativeDialog * dialog)

  \brief destroy a GtkNativeDialog

  \param dialog the GtkNativeDialog to destroy
*/
void destroy_this_native_dialog (GtkNativeDialog * dialog)
{
  gtk_native_dialog_destroy (dialog);
  g_object_unref (dialog);
  g_main_loop_quit (Event_loop[dialog_id]);
  g_main_loop_unref (Event_loop[dialog_id]);
  dialog_id --;
}

/*!
  \fn G_MODULE_EXPORT void run_destroy_dialog (GtkDialog * dialog, gint response_id, gpointer data)

  \brief to destroy a GtkDialog when the dialog emit the closing signal

  \param dialog the GtkDialog to destroy
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_destroy_dialog (GtkDialog * dialog, gint response_id, gpointer data)
{
  destroy_this_dialog (dialog);
}

#ifdef GTK4
/*!
  \fn GListModel * file_chooser_get_file_names (GtkFileChooser * chooser)

  \brief create a file list from files selected using a GtkFileChooser
*/
GListModel * file_chooser_get_file_names (GtkFileChooser * chooser)
{
  return gtk_file_chooser_get_files (chooser);
}
#else
/*!
  \fn GList * file_chooser_get_file_names (GtkFileChooser * chooser)

  \brief create a file list from files selected using a GtkFileChooser
*/
GSList * file_chooser_get_file_names (GtkFileChooser * chooser)
{
  return gtk_file_chooser_get_filenames (chooser);
}
#endif

/*!
  \fn gchar * file_chooser_get_file_name (GtkFileChooser * chooser)

  \brief get a file name from a GtkFileChooser (single file selected)

  \param chooser the GtkFilechooser
*/
gchar * file_chooser_get_file_name (GtkFileChooser * chooser)
{
#ifdef GTK4
  return g_file_get_parse_name (gtk_file_chooser_get_file (chooser));
#else
  return gtk_file_chooser_get_filename (chooser);
#endif
}

/*!
  \fn gchar * file_chooser_get_current_folder (GtkFileChooser * chooser)

  \brief get the current folder for a GtkFileChooser

  \param chooser the GtkFilechooser
*/
gchar * file_chooser_get_current_folder (GtkFileChooser * chooser)
{
#ifdef GTK4
  return g_file_get_parse_name (gtk_file_chooser_get_current_folder (chooser));
#else
  return gtk_file_chooser_get_current_folder (chooser);
#endif
}

/*!
  \fn gboolean file_chooser_set_file_name (GtkFileChooser * chooser, gchar * filename)

  \brief set file name in a GtkFilechooser

  \param chooser the GtkFilechooser
  \param filename the file name to appear in the GtkFilechooser
*/
gboolean file_chooser_set_file_name (GtkFileChooser * chooser, gchar * filename)
{
  GFile * default_file_for_saving = g_file_new_for_path (filename);
  gboolean res = gtk_file_chooser_set_file (chooser, default_file_for_saving, NULL);
  if (! res)
  {
    gchar * str = g_strdup_printf ("Impossible to locate file: %s", filename);
    show_error (str, 0, (GtkWidget *)chooser);
  }
  return res;
}

/*!
  \fn void file_chooser_set_current_folder (GtkFileChooser * chooser)

  \brief set current folder in a GtkFilechooser

  \param chooser the GtkFilechooser
*/
void file_chooser_set_current_folder (GtkFileChooser * chooser)
{
#ifdef GTK4
  gtk_file_chooser_set_current_folder (chooser, g_file_new_for_path ("./"), NULL);
#else
  gtk_file_chooser_set_current_folder (chooser, "");
#endif
}

#ifdef GTK4
/*!
  \fn GtkFileChooserNative * create_file_chooser (const gchar * title, GtkWindow * parent, GtkFileChooserAction act, const gchar * act_name)

  \brief create a GtkFileChooser, utility to select file(s)

  \param title the title of the window
  \param parent the parent window, if any
  \param act the action to perform (read or write)
  \param act_name the action name to display
*/
GtkFileChooserNative * create_file_chooser (const gchar * title, GtkWindow * parent, GtkFileChooserAction act, const gchar * act_name)
{
  return gtk_file_chooser_native_new (title, parent, act, act_name, "Cancel");
}
#else
/*!
  \fn GtkWidget * create_file_chooser (const gchar * title, GtkWindow * parent, GtkFileChooserAction act, const gchar * act_name)

  \brief create a GtkFileChooser, utility to select file(s)

  \param title the title of the window
  \param parent the parent window, if any
  \param act the action to perform (read or write)
  \param act_name the action name to display
*/
GtkWidget * create_file_chooser (const gchar * title, GtkWindow * parent, GtkFileChooserAction act, const gchar * act_name)
{
  return gtk_file_chooser_dialog_new (title, parent, act, "Cancel", GTK_RESPONSE_CANCEL, act_name, GTK_RESPONSE_ACCEPT, NULL);
}
#endif

/*!
  \fn ColRGBA get_button_color (GtkColorChooser * colob)

  \brief get the ColRGBA color from a GtkColorChooser button

  \param colob the GtkColorChooser button
*/
ColRGBA get_button_color (GtkColorChooser * colob)
{
  GdkRGBA col;
  gtk_color_chooser_get_rgba (colob, & col);
  return gdkrgba_to_rgba (col);
}

/*!
  \fn ColRGBA get_window_color (GtkWidget * color_win)

  \brief get the ColRGBA color from a color selection window

  \param color_win the color selection window
*/
ColRGBA get_window_color (GtkWidget * color_win)
{
  GdkRGBA col;
  gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER(color_win), & col);
  return gdkrgba_to_rgba (col);
}

/*!
  \fn void set_color_chooser_color (GtkWidget * color_win, ColRGBA col)

  \brief set the color of a color selection window

  \param color_win the color selection window
  \param col the color to use
*/
void set_color_chooser_color (GtkWidget * color_win, ColRGBA col)
{
  GdkRGBA colo = colrgba_togtkrgba(col);
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER(color_win), & colo);
}

#ifdef GTK4
/*!
  \fn void pop_menu_at_pointer (GtkWidget * pop, double x, double y)

  \brief popup a popover menu at pointer location

  \param pop the menu to popup
  \param x x position
  \param y y position
*/
void pop_menu_at_pointer (GtkWidget * pop, double x, double y)
{
  GdkRectangle rect;
  rect.x = x;
  rect.y = y;
  rect.width = 1;
  rect.height = 1;
  gtk_popover_set_has_arrow (GTK_POPOVER(pop), FALSE);
  gtk_popover_set_pointing_to (GTK_POPOVER(pop), & rect);
  gtk_popover_popup (GTK_POPOVER(pop));
}
#else
/*!
  \fn void pop_menu_at_pointer (GtkWidget * widg, GdkEvent * event)

  \brief popup a menu at pointer location

  \param widg the menu to popup
  \param event the GdkEvent triggering the signal
*/
void pop_menu_at_pointer (GtkWidget * widg, GdkEvent * event)
{
  show_the_widgets (widg);
  gtk_menu_popup_at_pointer (GTK_MENU (widg), event);
}
#endif

/*!
  \fn GtkWidget * get_top_level (GtkWidget * widg)

  \brief get the top level container, window, of a widget

  \param widg the GtkWidget
*/
GtkWidget * get_top_level (GtkWidget * widg)
{
#ifdef GTK4
  return (GtkWidget *)gtk_widget_get_root (widg);
#else
  return gtk_widget_get_toplevel (widg);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean destroy_this_window (GtkWindow * win, gpointer data)

  \brief destroy a GtkWindow

  \param win the GtkWindow to destroy
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean destroy_this_window (GtkWindow * win, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean destroy_this_window (GtkWidget * win, GdkEvent * event, gpointer data)

  \brief destroy a GtkWindow

  \param win the GtkWindow to destroy
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean destroy_this_window (GtkWidget * win, GdkEvent * event, gpointer data)
#endif
{
  destroy_this_widget (GTK_WIDGET(win));
  return TRUE;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean hide_this_window (GtkWindow * win, gpointer data)

  \brief hide a GtkWindow

  \param win the GtkWindow to hide
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean hide_this_window (GtkWindow * win, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean hide_this_window (GtkWidget * win, GdkEvent * event, gpointer data)

  \brief hide a GtkWindow

  \param win the GtkWindow to hide
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean hide_this_window (GtkWidget * win, GdkEvent * event, gpointer data)
#endif
{
  hide_the_widgets (GTK_WIDGET(win));
  return TRUE;
}

/*!
  \fn void add_gtk_close_event (GtkWidget * widg, GCallback handler, gpointer data)

  \brief add a close event signal and callback to a GtkWidget

  \param widg the GtkWidget
  \param handler the callback for the close event
  \param data the associated data pointer
*/
void add_gtk_close_event (GtkWidget * widg, GCallback handler, gpointer data)
{
#ifdef GTK4
  g_signal_connect (G_OBJECT (widg), "close-request", handler, data);
#else
  g_signal_connect (G_OBJECT (widg), "delete-event", handler, data);
#endif
}

/*!
  \fn int get_widget_width (GtkWidget * widg)

  \brief retrive GtkWidget width

  \param widg the GtkWidget
*/
int get_widget_width (GtkWidget * widg)
{
#ifdef GTK3
  return gtk_widget_get_allocated_width (widg);
#else
  return gtk_widget_get_width (widg);
#endif
}

/*!
  \fn int get_widget_height (GtkWidget * widg)

  \brief retrive GtkWidget height

  \param widg the GtkWidget
*/
int get_widget_height (GtkWidget * widg)
{
#ifdef GTK3
  return gtk_widget_get_allocated_height (widg);
#else
  return gtk_widget_get_height (widg);
#endif
}
