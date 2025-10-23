# **atomes** release candidate requirements

  - Version compatibitilty **MUST** be ensured to read older poject `*.apf` and workspace `*.awf` files

  - Project `*.apf` and workspace `*.awf` files version should evolve to save and read the new calculation data

  - Modify the `preferences.c` file to offer the options to save user preferences for this calculation

The dialog preferences offers the user the possibility to configure preferences that will be applied for any new project
to be opened in **atomes** workspace, that include analysis. 

It is required to update this preference dialog to include the new analysis, 
doing so will offer the possibility to save and read user preferences for the new analysis from the `**atomes.pml**` file. 
However this is likely to be more complicated. 

  - Modify the GNU archive for the official software distribution

## 1. Ensuring the compatibility with older files

During the [first step of this process][step-1] you modified the number of available calculations `NCACLS` that is used extensively in the code,
including when reading and saving project files, therefore you need to consider that reading older file versions will required to read 
`NCALCS - 1` data sets. 

For that edit the file [`src/project/open_pc.c`][open_p.c] 

  - Search for the [open_project][open_project] function and at the beginning locate the [`calcs_to_read`][calcs_to_read] variable definition:

  ```C
  int open_project (FILE * fp, int npi)
  {

    ...

    int calcs_to_read = NCALCS;
    // Start version related tests

    ...

    // End version related tests

    ...

  }
  ```
  - **DO NOT MODIFY** this line, instead modify it for other versions tested afterwards in the section related to version testing:

  ```C

  if (g_strcmp0(ver, "%\n% project file v-2.6\n%\n") == 0)  
  {

    ...

    calcs_to_read --;
  }
  else if (g_strcmp0(ver, "%\n% project file v-2.7\n%\n") == 0)
  {

    ...

    calcs_to_read --;
  }
  ```
Repeat the process for any version listed in that testing section.

> [!WARNING]
> You might need to decrement the value of `calcs_to_read` even more, 
> for instance change
> ```C
>   calcs_to_read --;
> ```
> Into
> ```C
>   calcs_to_read -= 2;
> ```

## 2. Modifying the `*.apf` and `*.awf` files for the IDC analysis

You will need to consider and modify:

  - How to save the data for the IDC calculation
  - How to read the data saved about an IDC calculation

### 1. Saving the IDC analysis calculation data

This is only required if: 

  - You modified the `atomes_anlysis` data structure to match your requirements
  - You need to save more information than available in the current data structure

#### 1. Modifying the file `src/project/open_p.c`

This is actually mandatory

### 2. Reading the IDC analysis calculation data 

#### 1. Modifying the file `src/project/save_p.c` 

  - Incrementing the project file version number

### 

## 3. Modifying the `preferences.c` file to save and read user preferences


## 4. Modifyinf the GNU archive for the official software distribution


[atomes_doxygen]:https://slookeur.github.io/atomes-doxygen/index.html
