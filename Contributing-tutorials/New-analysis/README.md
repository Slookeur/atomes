# Adding a new analysis to the **atomes** software

This document describes the steps required to add a new analysis in **atomes** 
and to make use of the [graph visualization system](https://atomes.ipcms.fr/analyze/) of the **atomes** program. 

To get familiar with the code of the **atomes** program please refer to the extensive [Doxygen][Doxygen] code source [documentation][atomes_doxygen]

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
    - Place it in the `data/pixmaps` folder

## Overview of the TODO list

  - **1** : Adding the new analysis description in the code
  - **2** : Coding the new analysis user dialog and its callbacks
  - **3** : Adding the new analysis to the **atomes** software
  - **4** : **atomes** release candidate requirements:

    - Modifying the **atomes** project (`.apf`) and workspace (`.awf`) file formats

      - To save / read the new analysis parameter(s) and result(s)
      - To ensure the reading compatibility of older `.apf` and `.awf` file formats

    - Modifying the user preferences dialog to consider the new analysis default parameter(s)

      - To save / read the new analysis parameter(s)
      - To ensure the reading of older user preferences XML file (should be automatic)

Overall step **1.** is easy, step **2.** and **3.** are slightly more complicated and might require my help.
 
Step **.4** is the most complicated part that will most likely require my help.

## [Adding the new analysis description in the code][adding]

## [Coding the new analysis user dialog and its callbacks][codding]

## [Adding the new analysis to the **atomes** software][analysing]

## [**atomes** release candidate requirements][releasing]


[Doxygen]:https://www.doxygen.nl/
[atomes_doxygen]:https://slookeur.github.io/atomes-doxygen/index.html
[adding]:STEP-1.md
[codding]:STEP-2.md
[analysing]:STEP-3.md
[releasing]:STEP-4.md
