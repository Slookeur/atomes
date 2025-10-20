# Coding the new analysis user dialog and its callbacks

## 1. Edit the file [`src/gui/calc_menu.c`][calc_menu.c]

  - In the function [`on_calc_activate`][on_calc_activate] add a test case for the new analysis

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

  - Write the `calc_idc` function that describes the calculation dialog for the new analysis:

  ```C
  /*!
    \fn void calc_idc (GtkWidget * vbox)

    \brief creation of the idc calculation widgets

    \param vbox GtkWidget that will receive the data
  */
  void calc_bonds (GtkWidget * vbox)
  {
    GtkWidget * idc_box;

   // This part requires to be a litte bit familiar with GTK+

    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, idc_box, FALSE, FALSE, 0);
  }
  ```
> [!TIP]
> Many example are available in **atomes** source code, in particular in the file [`src/gui/calc_menu.c`][calc_menu.c]

  - In the function [`run_on_calc_activate`][run_on_calc_activate] add a test case for the new analysis:

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
> Note that `on_calc_idc_released` is a function you **MUST** write to perform the calculation (see bellow).
 

## Adding the new analysis using the **atomes** software internal data structures

Create a new file


## **atomes** release candidate requirements

  - Finally `*.apf` and `*.awf` files version should evolve to save and read the new calculation data
  - Ultimately: modify the `preferences.c` file to offer the options to save user preferences for this calculation

[calc_menu.c]:https://slookeur.github.io/atomes-doxygen/d8/d5e/calc__menu_8c.html
[on_calc_activate]:https://slookeur.github.io/atomes-doxygen/d8/d5e/calc__menu_8c.html#a981fd6ae8aa02f6ba86bbfdfbeace7ed
[run_on_calc_activate]:https://slookeur.github.io/atomes-doxygen/d8/d5e/calc__menu_8c.html#a7605cb93faba5139a75d08568f1fb0a0
