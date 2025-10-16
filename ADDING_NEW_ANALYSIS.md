# Adding a new analysis to the **atomes** software

this document describes the steps required to add a new analysis in **atomes** 
and to make use of the [graph visualization system](https://atomes.ipcms.fr/analyze/) of the **atomes** program. 

## Before starting 

  - List all information required by this new analysis: 
    - Required parameter(s) for the user to input
    - Any requirement(s) for the calculation to be performed, for example: 
      - Should periodic boundary condition be applied ?
      - Should a prior calculation be performed ?

  - **atomes** Graphical User Interface uses the GTK+ library, in a coded version and not a XML file version, 
    meaning that it is required to code the dialog handling the calculation, I tried to simplify this as much as I could, 
    but in the end it is impossible to simplify everything.  
  
  - Prepare a 16x16 pixels PNG image to be used as an icon to illustrate the calculation for the GUI and place it in the `data\pixmaps` folder.

## The general TODO list

  1. Adding the new analysis description in the code
  2. Coding the new analysis calculation dialog
  3. Coding the new calculation and its connections to the **atomes** software internal data structures
  4. **atomes** release candidate requirements:

   - Modifying the **atomes** project (`.apf`) and workspace (`.awf`) files format

     - to save / read the new analysis parameters and results
     - to ensure the reading of older `.apf` and `.awf` file format(s)

   - Modifying the user preferences dialog to consider the new analysis default parameter(s)

     - to save / read the new analysis parameter(s)
     - to ensure the reading of older user preferences XML file (should be automatic)

Overall step **1.** is easy, step **2.** and **3.** are slightly more complicated and might require my help, and step **.4** is the most complicated part. 

## Adding the new analysis description in the code

Here is the step by step procedure: 

  0. Pick a 3 letter keyword to describe your new calculation, ex: **IDC**

  1. Edit the file `src/global.c` to create a `PACKAGE_IDC` variable:

  ```
  gchar * PACKAGE_IDC = NULL;
  ```

  2. Edit the file `src.global.h` to make the information available in other parts of the code:

    - Define 'IDC' a new, and unique, 3 characters variable, associated to the new calculation ID number: 

  ```
  #define IDC 10
  ```
  The associated number should be the latest calculation ID number + 1

    - Insert the following: 

  ```
  extern gchar * PACKAGE_IDC;
  ```
    - Increment the total number of calculations available : `NCALCS`
    - Increment increment the total number calculation using graphs : `NGRAPHS` (if needed)

  3. Edit the file `src/gui/main.c`:
 
    - to read the icon file for the new analysis (after line ): 

    ```
    PACKAGE_IDC = g_build_filename (PACKAGE_PREFIX, "pixmaps/idc.png", NULL);
    ```

  4. Edit the file `gui.c` :

    a. At the top modify the following variables to describe the new calculation, and to create the corresponding menu elements:
      - `atomes_action analyze_acts[]` : add a line similar to `{"analyze.idc",    GINT_TO_POINTER(IDC-1)}`
      - `char * calc_name[]` : add the new calculation name for the menu items
      - `char * graph_name[]` : add the new calculation name for the graph windows

    b. In the function 'G_MODULE_EXPORT void atomes_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data)' add the calculation menu callback:

             else if (g_strcmp0 (name, "analyze.idc") == 0)  // Update this line using the value in analyze_acts[]
             {
               on_calc_activate (NULL, data); // This does not change
             }

           - in the function 'GtkWidget * create_main_window (GApplication * atomes).' declare the icon for the new calculation:

             graph_img[IDC] = g_strdup_printf ("%s", PACKAGE_IDC);

       - edit the file 'calc_menu.c'

         - in the function 'G_MODULE_EXPORT void on_calc_activate (GtkWidget * widg, gpointer data)' add a case for the new analysis

           case IDC:
             calc_idc (box);
             break;

         - write the 'calc_idc' function that describes the calculation dialog for the new analysis:

         /*!
             \fn void calc_idc (GtkWidget * vbox)

             \brief creation of the idc calculation widgets

             \param vbox GtkWidget that will receive the data
          */
          void calc_bonds (GtkWidget * vbox)
          {
            GtkWidget * idc_box;

            add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, idc_box, FALSE, FALSE, 0);
          }

          Contact me for help !

         - In the function 'G_MODULE_EXPORT void run_on_calc_activate (GtkDialog * dial, gint response_id, gpointer data)' 

           - Add a case for the new analysis:

             case IDC:
               if (test_idc()) on_calc_idc_released (calc_win, NULL);
               break;

            - Note that 'test_idc()' might be a testing routine you want to write to ensure that conditions are met to perform the analysis

            - You now need to write the 'on_calc_idc_released' function to perform the calculation
 

       - edit the file 'initc.c'

         - declare the new analysis:

           active_project -> analysis[IDC] = setup_analysis (IDC, TRUE, num_graphs, num_compat, list_of_compat_calc);

       - if periodicity is required for this calculation, edit 'edit_menuc.c' edit the 'init_box_calc()' function
         to add the proper flags for :  'active_project -> analysis[IDC] -> avail_ok'

       - edit the file 'cbuild_action.c' line 1680 to add the default availability for this calculation
       - edit the file 'popup.c' line 2155 to add the default availability for this calculation

       - Curves setup if any:
         - adjust autoscale information, edit the file 'yaxis.c' line 107

       - Finally '*.apf' and '*.awf' files version should evolve to save and read the new calculation data

       - Ultimately: modify the 'preferences.c' file to offer the options to save user preferences for this calculation

  */
  
