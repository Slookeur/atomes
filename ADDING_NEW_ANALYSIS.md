# Adding a new analysis to the **atomes** software

this document describes the steps required to add a new analysis in **atomes** 
and to make use of the [graph visualization system](https://atomes.ipcms.fr/analyze/) of the **atomes** program. 

## Before starting 

  - If not done yet please give a look to the [`CONTRIBUTING.md`](https://github.com/Slookeur/atomes/blob/devel/CONTRIBUTING.md) document
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
  - **3** : Adding the new analysis using the **atomes** software internal data structures

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

> [!CAUTION]
> In the following I will use the `IDC`, sometimes `idc` keywords as examples, remember to adjust it ! 

### 1. Edit the file [`src/global.h`](https://slookeur.github.io/atomes-doxygen/d2/d49/global_8h.html) to make the information available in other parts of the code:

  - Define `IDC` a new, unique, 3 characters variable, associated to the new calculation ID number: 
```C
#define GDR 0
#define SQD 1
#define SKD 2
#define GDK 3
#define BND 4
#define ANG 5
#define RIN 6
#define CHA 7
#define SPH 8
#define MSD 9
#define IDC 10
```
The associated number should be the latest calculation ID number + 1 

At the time I wrote this tutorial MSD was the last one set to 9. 

  - Increment the total number of calculations available : `NCALCS`
  - Increment the total number calculation using graphs : `NGRAPHS`

### 2. Edit the file [`/src/gui/gui.c`](https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html)
  - At the top modify the following variables to describe the new calculation, and to create the corresponding menu elements:

    - [`atomes_action analyze_acts[]`](https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html#a63faa9f0b3e4a03314fadd5c0e0072ee) append a line similar to `{"analyze.idc",    GINT_TO_POINTER(IDC-1)}`

```C
atomes_action analyze_acts[] = {{"analyze.gr",     GINT_TO_POINTER(GDR)},
                                {"analyze.sq",     GINT_TO_POINTER(SQD)},
                                {"analyze.sk",     GINT_TO_POINTER(SKD)},
                                {"analyze.gk",     GINT_TO_POINTER(GDK)},
                                {"analyze.bonds",  GINT_TO_POINTER(BND)},
                                {"analyze.rings",  GINT_TO_POINTER(RIN-1)},
                                {"analyze.chains", GINT_TO_POINTER(CHA-1)},
                                {"analyze.sp",     GINT_TO_POINTER(SPH-1)},
                                {"analyze.msd",    GINT_TO_POINTER(MSD-1)},
                                {"analyze.idc",    GINT_TO_POINTER(IDC-1)}};
```

    - [`char * calc_name[]`](https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html#af7398ae8daba1bd18190e2cea0ff7735) : add the new calculation name for the menu items

```C
char * calc_name[] = {"g(r)/G(r)",
                      "S(q) from FFT[g(r)]",
                      "S(q) from Debye equation",
                      "g(r)/G(r) from FFT[S(q)]",
                      "Bonds and angles",
                      "Ring statistics",
                      "Chain statistics",
                      "Spherical harmonics",
                      "Mean Squared Displacement",
                      "The new analysis"};
```

    - [`char * graph_name[]`](https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html#ac889711808825fe192212c8a19e2d2b3) : add the new calculation name for the graph windows

```C
char * graph_name[] = {"g(r)/G(r)",
                       "S(q) from FFT[g(r)]",
                       "S(q) from Debye equation",
                       "g(r)/G(r) from FFT[S(q)]",
                       "Bonds properties",
                       "Angle distributions",
                       "Ring statistics",
                       "Chain statistics",
                       "Spherical harmonics",
                       "Mean Squared Displacement",
                       "The new analysis"};
```
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

  graph_img[IDC] = g_build_filename (PACKAGE_PREFIX, "pixmaps/idc.png", NULL);

  ...
}
```
### 3. Edit the file [`src/gui/initc.c`](https://slookeur.github.io/atomes-doxygen/d9/d35/initc_8c.html) to declare the new analysis

Search for the `atomes_analysis` function to declare the new analysis
```C
void init_atomes_analysis ()
{
  ...

  active_project -> analysis[IDC] = setup_analysis (IDC, TRUE, num_graphs, num_compat, comp_list);

  ...
}
```

### 4. Update the default availability for the new calculation:

  - Edit [`src/project/update_p.c`](https://slookeur.github.io/atomes-doxygen/db/d3e/update__p_8c.html) search for the `update_analysis_availability` function to add the proper flags
```C
void update_analysis_availability (project * this_proj)
{
  ...
  if (this_proj -> cell.has_a_box) // Or any other prerequisite
  {
    ...
       
    active_project -> analysis[IDC] -> avail_ok = TRUE;

    ...
  }
  else
  {
    ...
      
    active_project -> analysis[IDC] -> avail_ok = FALSE;

    ...
  }
  ...

  // Otherwise default value to TRUE (or FALSE)
  active_project -> analysis[IDC] -> avail_ok = TRUE;

  ...
}
```

### 5. Optional graph setup, if any:

  - Edit the file [`src/curve/yaxis.c`](https://slookeur.github.io/atomes-doxygen/df/dfb/yaxis_8c.html) to adjust specific axis autoscale information

```C
void autoscale_axis (project * this_proj, Curve * this_curve, int rid, int cid, int aid)
{
  ...

  // Not that aid is the axis: 0 = x, 1 = y
  if (rid = IDC)
  {
    // Min value for the new calculation to be specified here
    this_curve -> axmin[aid] = min_value[aid];

    // Max value for the new calculation to be specified here
    this_curve -> axmax[aid] = max_value[aid];    
  }

  ...
}
```

Note that by default **atomes** will just take the min and max values of the available calculation results but you might want to adjust this.

The autoscale is performed immediately after in this function. 

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

  - In the function `run_on_calc_activate` add a test case for the new analysis:
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
Note that `test_idc()` is an optional testing routine you might want to write to ensure that conditions are met to perform the analysis.

You now need to write the `on_calc_idc_released` function to perform the calculation (see bellow).
 

## Adding the new analysis using the **atomes** software internal data structures

Create a new file

## **atomes** release candidate requirements

  - Finally `*.apf` and `*.awf` files version should evolve to save and read the new calculation data
  - Ultimately: modify the `preferences.c` file to offer the options to save user preferences for this calculation
  
