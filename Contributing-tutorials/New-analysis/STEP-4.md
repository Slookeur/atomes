# **atomes** release candidate requirements

  - Version compatibitilty **MUST** be ensured to read older poject `*.apf` and workspace `*.awf` files.

  - Project `*.apf` and workspace `*.awf` files version should evolve to save and read the new calculation data. 
However, modifications to the actual format are not necessarily mandatory

  - User preferences **MUST** offer an interface to configure the new analysis. 
Modify the [`preferences.c`][preferences.c] file to offer the options to save user preferences for this calculation.
The preferences dialog offers the user the possibility to configure preferences that will be applied for any new project
to be opened in **atomes** workspace, that include analysis. 

It is required to update this preference dialog to include the new analysis, 
doing so will offer the possibility to save and read user preferences for the new analysis from the `atomes.pml` configuration file. 
However this is likely to be more complicated. 

  - Modify the GNU archive for the official software distribution

## 1. Modifying the `*.apf` and `*.awf` files to save the IDC analysis

If the fields available in the [`atomes_analysis`][atomes_analysis] data structure are enough to store the information and parameters, 
to describe the IDC analysis then you will have nothing to do. 

Otherwise 2 options are available to you, either modify the [`project`][project] data structure to store the extra parameters, or modify
 [`atomes_analysis`][atomes_analysis] data structure to do it. 

You will need to consider and modify:

  - How to save the data used, or needed, to perform the IDC calculation
  - How to read the data used, or needed, to perform the IDC calculation

### 1. To save the new IDC data edit the file [`src/project/save_p.c`][save_p.c]

  - Search for the [`save_project`][save_project] function
  ```C
  /*!
    \fn int save_project (FILE * fp, project * this_proj)

    \brief save project to file

    \param fp the file pointer
    \param this_proj the target project
  */
  int save_project (FILE * fp, project * this_proj)
  {
    int i, j, k;
    gchar * ver;

    // First 2 lines for compatibility issues
    i = 2;
    j = 9;
    ver = g_strdup_printf ("%%\n%% project file v-%1d.%1d\n%%\n", i, j);

  ...
  ```
  - Increment the project version numbers `i` and `j`, in this example: 
  ```C
    i = 3;
    j = 0;
  ```
> [!WARNING]
>    In the next line these integer numbers are written on a single digit, which I confess might not have been my best idea at the time. 
>    It now requires to ensure to increment `i` if `j` should reach `10`. 

### 2. To read the new IDC data edit the file [`src/project/open_p.c`][open_p.c]

  - At te beginning of the file create a new gboolean variable for the new file version 
  ```C

  ...

  gboolean version_2_6_and_above;
  gboolean version_2_7_and_above;
  gboolean version_2_8_and_above;
  gboolean version_2_9_and_above;
  gboolean version_3_0_and_above;  // This is an example

  ...

  ```
  This new variable `version_3_0_and_above` will be used, if needed, to trigger actions specific to the new analysis and to be performed when reading **atomes** project files.

  - At the beginning of the [`open_project`][open_project] function, set the value of the new variable to `FALSE`
  ```C
  /*!
    \fn int open_project (FILE * fp)

    \brief open atomes project file

    \param fp the file pointer
  */
  int open_project (FILE * fp)
  {
    
    ...

    version_2_8_and_above = FALSE; 
    version_2_9_and_above = FALSE;
    version_3_0_and_above = FALSE;  // This is an example

    ...

  ```
  - Then create a new test case for the project version with a number matching the one specified when creating the new project file. 
To do that copy the previous test case, change the version number, and for this new test case only, set the value of the variable to `TRUE`
  ```C

  ...

  else if (g_strcmp0(version, "%\n% project file v-2.9\n%\n") == 0)
  {
    version_2_6_and_above = TRUE;
    version_2_7_and_above = TRUE;
    version_2_8_and_above = TRUE;
    version_2_9_and_above = TRUE;
    version_2_9_and_above = TRUE;
  }
  else if (g_strcmp0(version, "%\n% project file v-3.0\n%\n") == 0)  // This is an example
  {
    version_2_6_and_above = TRUE;
    version_2_7_and_above = TRUE;
    version_2_8_and_above = TRUE;
    version_2_9_and_above = TRUE;
    version_2_9_and_above = TRUE;
    version_3_0_and_above = TRUE;                                    // This is an example
  }

  ...

  ```
  - The rest is up to you to decide, but if you need to adapt some part of the file for the new case scenario simply use the new `version_3_0_and_above` to avoid conflics with previous version:
  ```C

  ...

  if (version_3_0_and_above) // Do something only for the new file version to read IDC related information
  {


  }

  ...

  ```
 >[!TIP]
 > Contact me if you need any help !

## 3. Modifying the [`src/gui/preferences.c`][preferences.c] file to save and read user preferences


## 4. Modifyinf the GNU archive for the official software distribution


[atomes_doxygen]:https://slookeur.github.io/atomes-doxygen/index.html
[atomes_analysis]:
[project]:https://slookeur.github.io/atomes-doxygen/dd/dbe/structproject.html
[preferences.c]:https://slookeur.github.io/atomes-doxygen/de/dee/preferences_8c.html
[open_p.c]:https://slookeur.github.io/atomes-doxygen/da/d5e/open__p_8c.html
[open_project]:https://slookeur.github.io/atomes-doxygen/da/d5e/open__p_8c.html#a0b222c223270264f9754d008a37317aa
[calcs_to_read]:to_be_done
[save_p.c]:https://slookeur.github.io/atomes-doxygen/d7/d70/save__p_8c.html
[save_project]:https://slookeur.github.io/atomes-doxygen/d7/d70/save__p_8c.html#ada55fbf67b7240b5158cdaa9f8a62810
