# Coding the new analysis user dialog and its callbacks

## Edit the file [`src/gui/calc_menu.c`][calc_menu.c]

  1. In the function [`run_on_calc_activate`][run_on_calc_activate] add a test case for the new analysis:

  ```C
  G_MODULE_EXPORT void run_on_calc_activate (GtkDialog * dial, gint response_id, gpointer data)
  {
    ...

    case IDC:
      if (test_idc()) on_calc_idc_released (calc_win, NULL);
      break;

    ...
  }
  ```

> [!TIP]
> Note that `test_idc()` is an optional testing routine you might want to write to ensure that conditions are met to perform the analysis.

> [!IMPORTANT]
> Note that `on_calc_idc_released` is a function you **MUST** write to perform the calculation ([see next step][step-3]).
 
  2. In the function [`on_calc_activate`][on_calc_activate] add a test case for the new analysis

  ```C
  G_MODULE_EXPORT void on_calc_activate (GtkWidget * widg, gpointer data)
  {
    ...

    case IDC:
      calc_idc (box);
      break;
      
    ...
  }
  ```
> [!IMPORTANT]
> Note that `calc_idc` is a function you **MUST** write to describe the user dialog for the analysis (see below).

  3. Write the `calc_idc` function that describes the calculation dialog for the new analysis:

  ```C
  /*!
    \fn void calc_idc (GtkWidget * vbox)

    \brief creation of the idc calculation widgets

    \param vbox GtkWidget that will receive the data
  */
  void calc_idc (GtkWidget * vbox)
  {
    GtkWidget * idc_box;

   // This part requires to be a litte bit familiar with GTK+

    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, idc_box, FALSE, FALSE, 0);
  }
  ```
> [!TIP]
> Some examples are provided afterwards but many more are available in **atomes** source code, including in the file [`src/gui/calc_menu.c`][calc_menu.c]

In the following examples are use to illustrate both how to create the user interface, and to read back the associated data what is usually call a **callback**. 

  - Create an entry to read an integer value

  ```C
  /*!
  \fn G_MODULE_EXPORT void reading_integer (GtkEntry * entry, gpointer data)

  \brief reading an integer parameter

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
  */
  G_MODULE_EXPORT void reading_integer (GtkEntry * entry, gpointer data)
  {
    const gchar * m = entry_get_text (entry);
    int param = (int) string_to_double ((gpointer)m);

    // Do something with the parameter here
    // This should include testing for acceptable range values

    update_entry_int (entry, param);
  }

  ...

  void calc_idc (GtkWidget * vbox)
  {
    GtkWidget * idc_box;

    ...

    GtkWidget * entry = create_entry (G_CALLBACK(read_integer), 100, 15, FALSE, NULL);
    // You might want to update the entry with a parameter related to IDC
    update_entry_int (GTK_ENTRY(entry), active_project -> analysis[IDC] -> num_delta);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, idc_box, entry, FALSE, FALSE, 0);

    ...

    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, idc_box, FALSE, FALSE, 0);
  }
  ```

  - Create an entry to read a double value

  ```C
  /*!
  \fn G_MODULE_EXPORT void reading_double (GtkEntry * entry, gpointer data)

  \brief reading a double parameter

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
  */
  G_MODULE_EXPORT void reading_double (GtkEntry * entry, gpointer data)
  {
    const gchar * m = entry_get_text (entry);
    doublet param = string_to_double ((gpointer)m);

    // Do something with the parameter here
    // This should include testing for acceptable range values

    update_entry_double (entry, param);
  }

  ...

  void calc_idc (GtkWidget * vbox)
  {
    GtkWidget * idc_box;

    ...

    GtkWidget * entry = create_entry (G_CALLBACK(read_integer), 100, 15, FALSE, NULL);
    // You might want to update the entry with a parameter related to IDC
    update_entry_double (GTK_ENTRY(entry), active_project -> analysis[IDC] -> delta);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, idc_box, entry, FALSE, FALSE, 0);

    ...

    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, idc_box, FALSE, FALSE, 0);
  }
  ```

  - Create a check button to capture a yes or no option

  ```C
  #ifdef GTK4
  /*!
    \fn G_MODULE_EXPORT void toggle_parameter (GtkCheckButton * but, gpointer data)

    \brief toggle a parameter GTK4 callback

    \param but the GtkCheckButton sending the signal
    \param data the associated data pointer
  */
  G_MODULE_EXPORT void toggle_parameter (GtkCheckButton * but, gpointer data)
  #else
  /*!
    \fn G_MODULE_EXPORT void toggle_parameter (GtkToggleButton * but, gpointer data)

    \brief toggle a parameter GTK3 callback

    \param but the GtkToggleButton sending the signal
    \param data the associated data pointer
  */
  G_MODULE_EXPORT void toggle_parameter (GtkToggleButton * but, gpointer data)
  #endif
  {
    gboolean status = button_get_status ((GtkWidget *)but);
    // Do something with status here
  }

  ...

  void calc_idc (GtkWidget * vbox)
  {
    GtkWidget * idc_box;

    ...

    GtkWidget * check = check_button ("Apply this or not ?", -1, 40, status, G_CALLBACK(toggle_parameter), NULL);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, idc_box, check, FALSE, FALSE, 0);
   
    ...

    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, idc_box, FALSE, FALSE, 0);
  }
  ```

  - Create a combo box for option selection

  ```C
  /*!
  \fn G_MODULE_EXPORT void combo_changed (GtkComboBox * box, gpointer data)

  \brief changing a combo box

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
  */
  G_MODULE_EXPORT void combo_changed (GtkComboBox * box, gpointer data)
  {
    int val = combo_get_active ((GtkWidget *)box);

    // Do something with the parameter here
  }

  ...

  void calc_idc (GtkWidget * vbox)
  {
    GtkWidget * idc_box;

    ...

    GtkWidget * hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label ("Please select this", 250, -1, 0.0, 0.5), FALSE, FALSE, 0);
    GtkWidget * combo = create_combo ();
    int i;
    gchar * list_items[] = {"First choice", "Second choice", "Third choice"};
    for (i=0; i<G_N_ELEMENT(list_items); i++)
    {
      combo_text_append (combo, list_item[i]);
    }
    combo_set_active (combo, 0);   // Define the active element using an integer value
    g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(combo_changed), NULL);      
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, combo, FALSE, FALSE, 10);

    ...

    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
  }
  ```

More information on the severak home made functions be found in the ['src/gui/gtk-misck.c'][gtk-misc.c] including:

 - [create_entry][create_entry]
 - [check_button][check_button]
 - [create_combo][create_combo]


[calc_menu.c]:https://slookeur.github.io/atomes-doxygen/d8/d5e/calc__menu_8c.html
[on_calc_activate]:https://slookeur.github.io/atomes-doxygen/d8/d5e/calc__menu_8c.html#a981fd6ae8aa02f6ba86bbfdfbeace7ed
[run_on_calc_activate]:https://slookeur.github.io/atomes-doxygen/d8/d5e/calc__menu_8c.html#a7605cb93faba5139a75d08568f1fb0a0
[gtk-misc.c]:https://slookeur.github.io/atomes-doxygen/d8/d90/gtk-misc_8c.html
[create_entry]:https://slookeur.github.io/atomes-doxygen/d8/d90/gtk-misc_8c.html#a0bda34e4b6916f87a3acb0f653aa7dcb
[check_button]:https://slookeur.github.io/atomes-doxygen/d8/d90/gtk-misc_8c.html#a72f9e465d5e8934fa4de4f942549b6dd
[create_combo]:https://slookeur.github.io/atomes-doxygen/d8/d90/gtk-misc_8c.html#a92e35afc6402d74fa13ba8add3995344
[step-3]:STEP-3.md
