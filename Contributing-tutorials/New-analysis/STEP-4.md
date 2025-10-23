# **atomes** release candidate requirements

  - Internal `*.apf` and `*.awf` files version should evolve to save and read the new calculation data

  - Modify the `preferences.c` file to offer the options to save user preferences for this calculation

The dialog preferences offers the user the possibility to configure preferences that will be applied for any new project
to be opened in **atomes** workspace, that include analysis. 

It is required to update this preference dialog to include the new analysis, 
doing so will offer the possibility to save and read user preferences for the new analysis from the `**atomes.pml**` file. 
However this is likely to be more complicated. 

  - Modify the GNU archive for the official software distribution

## 1. Modifying the `*.apf` and `*.awf` files

You will need to modified simultaneoulsy:

  - How to save the data for the IDC calculation
  - How to read the data saved about an IDC calculation
  - How to ensure the version compatibility that older files are still readable

### 1. Saving the IDC analysis calculation data

This is only required if: 
  - You modified the `atomes_anlysis` data structure to match your requirements
  - You need to save more information than available in the current data structure

#### 1. Modifying the file `src/project/open_p.c`



### 2. Reading the IDC analysis calculation data 

#### 1. Modifying the file `src/project/save_p.c` 

  - Incrementing the project file version number


  - 

## 2. Modifying the `preferences.c` file to save and read user preferences


## 3. Modifyinf the GNU archive for the official software distribution


[atomes_doxygen]:https://slookeur.github.io/atomes-doxygen/index.html
