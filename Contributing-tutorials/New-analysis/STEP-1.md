# Adding the new analysis description in the code

## 0. Pick a 3 letter keyword to describe your new calculation, ex: ***IDC***

> [!CAUTION]
> In the following I will use the `IDC`, sometimes `idc` keywords as examples, remember to adjust it ! 

## 1. Edit the file [`src/global.h`][global.h] to make the information available in other parts of the code:

  - Increment the total number of calculations available : `NCALCS`
  ```C
  #define NCALCS 11   // The previous value was 10
  ```

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
  #define IDC 10  // This is an example
  ```

>[!IMPORTANT]
>The associated number should be the latest calculation ID number + 1,  
>when I wrote this tutorial `MSD` was the last one set to 9. 

## 2. Edit the file [`/src/gui/gui.c`][gui.c]
  - At the top modify the following variables to describe the new analysis, and to create the corresponding menu elements:

    - [`gchar * calc_name`][calc_name] : append a line to add the new analysis name for the menu items
    ```C
    gchar * calc_name[] = {"g(r)/G(r)",
                           "S(q) from FFT[g(r)]",
                           "S(q) from Debye equation",
                           "g(r)/G(r) from FFT[S(q)]",
                           "Bonds and angles",
                           "Ring statistics",
                           "Chain statistics",
                           "Spherical harmonics",
                           "Mean Squared Displacement",
                           "The new analysis"};  // This is an example
    ```
    - [`gchar * graph_name`][graph_name] : append a line to add the new analysis name for the tool box window
    ```C
    gchar * graph_name[] = {"g(r)/G(r)",
                            "S(q) from FFT[g(r)]",
                            "S(q) from Debye equation",
                            "g(r)/G(r) from FFT[S(q)]",
                            "Bonds properties",
                            "Angle distributions",
                            "Ring statistics",
                            "Chain statistics",
                            "Spherical harmonics",
                            "Mean Squared Displacement",
                            "The new analysis"};  // This is an example
    ```
    - [`gchar * graph_icon`][graph_icon] : append a line to add the new analysis icon file name
    ```C
    gchar * graph_icon[] = {"pixmaps/gr.png",
                            "pixmaps/sq.png",
                            "pixmaps/sq.png",
                            "pixmaps/gr.png",
                            "pixmaps/bd.png",
                            "pixmaps/an.png",
                            "pixmaps/ri.png",
                            "pixmaps/ch.png",
                            "pixmaps/sp.png",
                            "pixmaps/ms.png",
                            "pixmaps/idc.png"};  // this is an example
    ```

## 3. Edit the file [`src/gui/initc.c`][init.c] to declare the new analysis

Search for the [`init_atomes_analysis`][init_atomes_analysis] function to declare the new analysis

```C
/*!
  \fn void init_atomes_analysis (project * this_proj,  gboolean apply_defaults)

  \brief initialize analysis data structures for atomes

  \param this_proj the target project
  \param apply_defaults apply default parameters (1/0)
*/
void init_atomes_analysis (project * this_proj, gboolean apply_defaults)
{
  ...

  int num_g = ;             // Total number of graph windows as result(s) of the analysis
  int num_c = ;             // Number of analysis compatible, to allow overlap of the data on the graph window
  int c_list[num_c] = {};   // List of compatible analysis, integer list in the ID list, including self
  active_project -> analysis[IDC] = setup_analysis (pid, IDC, TRUE, num_g, num_c, c_list);  // This is an example

  ...
}
```
> [!WARNING]
> Analyis compatibility list **MUST** include its own unique ID, in the example IDC, and all other compatible analysis if any. 
> This information is used to handle superposition of data sets on graph windows 

## 4. Update the default availability for the new calculation:

  - Edit [`src/project/update_p.c`][update_p.c] search for the [`update_analysis_availability`][update_analysis_availability] function to add the proper flags

Specify if the analysis is always available, or if conditions have to be met:

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

## 5. Optional graph setup, if any:

  - Edit the file [`src/curve/cwidget.c`][cwidget.c] that contains few functions to tweak few graph related options

  ```C
  DataLayout * curve_default_layout (project * pid, int rid, int cid)
  {
    ...

    if (rid == IDC)
    {
       // Specific layout option(s) here
       // Among the options available in the DataLayout data structure - see bellow
    }

    ...
  }

  ...

  void curve_default_scale (project * this_proj, int rid, int cid, Curve * this_curve)
  {
    ...

    else if (rid == IDC)
    {
       this_curve -> cmin[0] = user_define_min;  // To set the default min value
       this_curve -> cmax[0] = user_define_max;  // To set the default max value
       // Or any other option in the Curve data structure - see bellow
    }

    ...
  }
  ```
> [!TIP]
> More information about the [DataLayout][DataLayout] data structure is available in the [Doxygen documentation of the **atomes** program][atomes_doxygen]
>
> 
> More information about the [Curve][Curve] data structure is available in the [Doxygen documentation of the **atomes** program][atomes_doxygen]
>
> You can also consider to modify the corresponding data structures for your need !

  - Edit the file [`src/curve/yaxis.c`][yaxis.c] to adjust specific axis autoscale information

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

> [!TIP]
> By default **atomes** will just take the minimum and maximum values for an analysis based on the calculation result(s)
>
> This is to enforce to use other set of parameters as min and max values for x and / or y axis.

The autoscale is performed immediately after in this function. 

## Next : [Coding the new analysis user dialog and its callbacks][coding]
## Previous : [Adding a new analysis to the atomes software][adding]

[Doxygen]:https://www.doxygen.nl/
[atomes_doxygen]:https://slookeur.github.io/atomes-doxygen/index.html
[global.h]:https://slookeur.github.io/atomes-doxygen/d2/d49/global_8h.html
[gui.c]:https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html
[analyze_acts]:https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html#a63faa9f0b3e4a03314fadd5c0e0072ee
[calc_name]:https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html#af7398ae8daba1bd18190e2cea0ff7735
[graph_name]:https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html#ac889711808825fe192212c8a19e2d2b3
[graph_icon]:to_be_added
[atomes_menu_bar_action]:https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html#acd3996df411bbbd56fd07fdb40953f75
[create_main_window]:https://slookeur.github.io/atomes-doxygen/d5/d03/gui_8c.html#a92f4ba1abecbb26e2b9e934397930f37
[init.c]:https://slookeur.github.io/atomes-doxygen/d9/d35/initc_8c.html
[init_atomes_analysis]:to_be_added
[update_p.c]:https://slookeur.github.io/atomes-doxygen/db/d3e/update__p_8c.html
[update_analysis_availability]:to_be_added
[cwidget.c]:https://slookeur.github.io/atomes-doxygen/d4/d80/cwidget_8c.html
[yaxis.c]:https://slookeur.github.io/atomes-doxygen/df/dfb/yaxis_8c.html
[DataLayout]:https://slookeur.github.io/atomes-doxygen/d0/d5d/struct_data_layout.html
[Curve]:https://slookeur.github.io/atomes-doxygen/da/d6e/struct_curve.html
[coding]:STEP-2.md
[adding]:README.md
