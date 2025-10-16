# Adding a new analysis to the **atomes** software

this document describes the steps required to add a new analysis in **atomes** 
and to make use of the [graph visualization system](https://atomes.ipcms.fr/analyze/) of the **atomes** program. 

## Before starting 

  - List all information required by this new analysis: 
    - Required parameter(s) for the user to input
    - Any requirement(s) for the calculation to be performed, for example:

      - Is a box description required ? 
      - Should periodic boundary condition be applied ?
      - Should a prior calculation be performed ?
      - Is a trajectory (multiple configurations) required ?

  - **atomes** Graphical User Interface uses the GTK+ library, in a coded version and not a XML file version, 
    meaning that it is required to code the dialog handling the calculation, I tried to simplify this as much as I could, 
    but in the end it is impossible to simplify everything.  
  
  - Prepare an image to illustrate the calculation for the GUI
    - PNG format 
    - 16x16 pixels 
    - Place it in the `data/pixmaps` folder.

## Overview of the TODO list

  - **1** : Adding the new analysis description in the code
  - **2** : Coding the new analysis user dialog and its callbacks
  - **3** : Coding the new calculation and its connections to the **atomes** software internal data structures

    - Create a new source file to implement the calculation
    - Add this new file to the `Makefile`

  - **4** : **atomes** release candidate requirements:

    - Modifying the **atomes** project (`.apf`) and workspace (`.awf`) files format

      - To save / read the new analysis parameters and results
      - To ensure the reading of older `.apf` and `.awf` file format(s)

    - Modifying the user preferences dialog to consider the new analysis default parameter(s)

      - To save / read the new analysis parameter(s)
      - To ensure the reading of older user preferences XML file (should be automatic)

Overall step **1.** is easy, step **2.** and **3.** are slightly more complicated and might require my help.
 
Step **.4** is the most complicated part. 

## Adding the new analysis description in the code

Here is the step by step procedure: 

### 0. Pick a 3 letter keyword to describe your new calculation, ex: ***IDC***

### 1. Edit the file [`src/global.c`](https://slookeur.github.io/atomes-doxygen/dc/d57/global_8c.html) to create a `PACKAGE_IDC` variable
  ```C
  gchar * PACKAGE_IDC = NULL;
  ```
This is to be done close to line **97**

### 2. Edit the file [`src/global.h`](https://slookeur.github.io/atomes-doxygen/d2/d49/global_8h.html) to make the information available in other parts of the code:

  - Define `IDC` a new, unique, 3 characters variable, associated to the new calculation ID number: 
  ```C
  #define IDC 10
  ```
  The associated number should be the latest calculation ID number + 1
  This is to be done close to line **336**

  - Insert the following: 
  ```C
  extern gchar * PACKAGE_IDC;
  ```
  This is to be done close to line **366**
  - Increment the total number of calculations available : `NCALCS`
  - Increment increment the total number calculation using graphs : `NGRAPHS` (if needed)

### 3. Edit the file [`src/gui/main.c`](https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html) to read the icon file for the new analysis (after line ): 
  ```C
  PACKAGE_IDC = g_build_filename (PACKAGE_PREFIX, "pixmaps/idc.png", NULL);
  ```
### 4. Edit the file [`/src/gui/gui.c`](https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html)
  - At the top modify the following variables to describe the new calculation, and to create the corresponding menu elements:

    - `atomes_action analyze_acts[]` : add a line similar to `{"analyze.idc",    GINT_TO_POINTER(IDC-1)}`
    - `char * calc_name[]` : add the new calculation name for the menu items
    - `char * graph_name[]` : add the new calculation name for the graph windows

  - In the function `atomes_menu_bar_action` add the calculation menu callback:
  ```C
  G_MODULE_EXPORT void atomes_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data)
  {
    ...

  else if (g_strcmp0 (name, "analyze.idc") == 0)  // Update this line using the value in analyze_acts[]
  {
    on_calc_activate (NULL, data); // This does not change
  }

    ...
  }
  ```
  - In the function `create_main_window` declare the icon for the new calculation:
  ```C
  GtkWidget * create_main_window (GApplication * atomes)
 {
    ...

   graph_img[IDC] = g_strdup_printf ("%s", PACKAGE_IDC);

    ...
  }
  ```
### 5. Edit the file [`src/gui/initc.c`](https://slookeur.github.io/atomes-doxygen/d9/d35/initc_8c.html)

  - declare the new analysis, after line :
  ```C
  active_project -> analysis[IDC] = setup_analysis (IDC, TRUE, num_graphs, num_compat, list_of_compat_calc);
  ```

### 6. If periodicity is required for this calculation:

  - Edit [`src/gui/edit_menuc.c`](https://slookeur.github.io/atomes-doxygen/d8/da6/edit__menu_8c.html) search for the `init_box_calc()` function to add the proper flags for
```C
  active_project -> analysis[IDC] -> avail_ok
```
  - Edit the file [`src/opengl/edit/cbuild_action.c`](https://slookeur.github.io/atomes-doxygen/d0/dd3/cbuild__action_8c.html) close to line **1680** to add the default availability for this calculation
  - Edit the file [`src/opengl/win/popup.c`](https://slookeur.github.io/atomes-doxygen/d5/da4/popup_8c.html) close line **2155** to add the default availability for this calculation

### 7. Optional graph setup, if any:

  - Edit the file [`src/curve/yaxis.c`](https://slookeur.github.io/atomes-doxygen/df/dfb/yaxis_8c.html) to adjust the Y axis autoscale information, after line 107

## Coding the new analysis user dialog and its callbacks


### 1. Edit the file [`src/gui/calc_menu.c`](https://slookeur.github.io/atomes-doxygen/d8/d5e/calc__menu_8c.html)

  - In the function `on_calc_activate` add a case for the new analysis
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

Contact me for help !

  - In the function `G_MODULE_EXPORT void run_on_calc_activate (GtkDialog * dial, gint response_id, gpointer data)` 
    - Add a test case for the new analysis:
    ```C
      case IDC:
        if (test_idc()) on_calc_idc_released (calc_win, NULL);
        break;
     ```
    Note that `test_idc()` might be a testing routine you want to write to ensure that conditions are met to perform the analysis

    - You now need to write the `on_calc_idc_released` function to perform the calculation (see bellow).
 

## Coding the new calculation and its connections to the **atomes** software internal data structures

Create a new file

## **atomes** release candidate requirements

  - Finally `*.apf` and `*.awf` files version should evolve to save and read the new calculation data
  - Ultimately: modify the `preferences.c` file to offer the options to save user preferences for this calculation
  
