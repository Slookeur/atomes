# **atomes** release candidate requirements

  - Version compatibitilty **MUST** be ensured to read older poject `*.apf` and workspace `*.awf` files

  - Project `*.apf` and workspace `*.awf` files version should evolve to save and read the new calculation data

  - Modify the [`preferences.c`][preferences.c] file to offer the options to save user preferences for this calculation

The preferences dialog offers the user the possibility to configure preferences that will be applied for any new project
to be opened in **atomes** workspace, that include analysis. 

It is required to update this preference dialog to include the new analysis, 
doing so will offer the possibility to save and read user preferences for the new analysis from the `**atomes.pml**` file. 
However this is likely to be more complicated. 

  - Modify the GNU archive for the official software distribution

## 1. Modifying the `*.apf` and `*.awf` files to save the IDC analysis

You will need to consider and modify:

  - How to save the data used, or needed, to perform the IDC calculation
  - How to read the data used, or needed, to perform the IDC calculation

If the fields available in the [`atomes_analysis`][atomes_analysis] data structure are enough to store the information and parameters, 
then you will have nothing to do. 

Otherwise 2 options are available to you, either modify [`atomes_project`] data structure to store the extra parameters, or modify
 [`atomes_analysis`][atomes_analysis] data structure to do it. 


#### 1. Modifying the file  [`src/project/open_p.c`][open_p.c]

This is actually mandatory

### 2. Reading the IDC analysis calculation data 

#### 1. Modifying the file [`src/project/save_p.c`][save_p.c]

  - Incrementing the project file version number

### 

## 3. Modifying the [`src/gui/preferences.c`][preferences.c] file to save and read user preferences


## 4. Modifyinf the GNU archive for the official software distribution


[atomes_doxygen]:https://slookeur.github.io/atomes-doxygen/index.html
[preferences.c]:https://slookeur.github.io/atomes-doxygen/de/dee/preferences_8c.html
[open_p.c]:https://slookeur.github.io/atomes-doxygen/da/d5e/open__p_8c.html
[open_project]:https://slookeur.github.io/atomes-doxygen/da/d5e/open__p_8c.html#a0b222c223270264f9754d008a37317aa
[calcs_to_read]:to_be_done
[save_p.c]:https://slookeur.github.io/atomes-doxygen/d7/d70/save__p_8c.html
