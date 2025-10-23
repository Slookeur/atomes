# **atomes** release candidate requirements

  - Internal `*.apf` and `*.awf` files version should evolve to save and read the new calculation data

    1. Saving the new IDC analysis data

    2. Reading the new IDC analysis data


  - Modify the `preferences.c` file to offer the options to save user preferences for this calculation

The dialog preferences offers the user the possibility to configure preferences that will be applied for any new project
to be opened in **atomes** workspace, that include analysis. 

It is required to update this preference dialog to include the new analysis, 
doing so will offer the possibility to save and read user preferences for the new analysis from the `**atomes.pml**` file. 
However this is likely to be more complicated. 


  - Modify the GNU archive for the official software distribution
    - Edit the file `Makefile.am` to insert the new files


[atomes_doxygen]:https://slookeur.github.io/atomes-doxygen/index.html
