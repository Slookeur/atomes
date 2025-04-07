# atomes

![License][license]
![Development Status][dev_status]

[atomes][atomes] is a Free (Open Source) cross-platform software licensed under the terms 
of the Affero GPL v3+ license. 
Atoms is a toolbox developed to analyze, to visualize and to create/edit three-dimensional atomistic models.
It offers a workspace that allows to have many projects opened simultaneously.

The different projects in the workspace can exchange data: analysis results, atomic coordinates ...
atomes also provides an advanced input preparation system for further calculations using well known molecular dynamics codes:

- Classical MD : [DLPOLY][dlpoly] and [LAMMPS][lammps]
- ab-initio MD : [CPMD][cpmd] and [CP2K][cp2k]
- QM-MM MD : [CPMD][cpmd] and [CP2K][cp2k]

To prepare the input ﬁlles for these calculations is likely to be the key, and most complicated step towards MD simulations. 
atomes offers a user-friendly assistant to help and guide the user step by step to achieve this crucial step.

## Features

  - Analysis of 3D atomistic model: neutron and x-rays diffraction, rings statistics, chain statistics, bond order, MSD ...
  - Visualization: measures, coordination polyhedras, advanced coloring, advanced design
  - Edition: molecular library, crystal builder, cell edition, surface creation and passivation ...
  - MD input preparation: 
	- Classical MD: [DLPOLY][dlpoly] and [LAMMPS][lammps]
	- ab-initio MD: [CPMD][cpmd] and [CP2K][cp2k]
	- QM-MM MD: [CPMD][cpmd] and [CP2K][cp2k]

## Build instructions

To build ***atomes*** standard version: 

```
make atomes
```

To build ***atomes*** debug version:

```
make debug
```

The latter is for debugging purposes only, also prints extra information at runtime to help with the process.

### Build options

#### Building the GTK4 version of ***atomes***

***atomes*** uses the [GTK][gtk] lib for the GUI, by default GTK3 is used, however it is possible to build the GTK4 version (beta), to do that edit the `Makefile` and change:

> GTKV = 3 

to 

> GTKV = 4

Here are some issues with GTK4 that cannot be sovled for the time being:
  1. No way to attach widget to menu items not in the top level of the menu (https://gitlab.gnome.org/GNOME/gtk/-/issues/5955)

#### Building the serial version of ***atomes***

By default ***atomes*** uses [OpenMP][openmp] to parallelize several calculations over the CPU cores. 
It is possible to turn this off, and to build a serial version of ***atomes***, to do that edit the `Makefile` and change:

> OPENMP = 1

to

> OPENMP =


## Who's behind ***atomes***


***atomes*** is developed by [Dr. Sébastien Le Roux][slr], research engineer for the [CNRS][cnrs]

<p align="center">
  <a href="https://www.cnrs.fr/"><img width="100" src="https://www.cnrs.fr/themes/custom/cnrs/logo.svg" alt="CNRS logo" align="center"></a>
</p>

[Dr. Sébastien Le Roux][slr] works at the Institut de Physique et Chimie des Matériaux de Strasbourg [IPCMS][ipcms]

<p align="center">
  <a href="https://www.ipcms.fr/"><img width="100" src="https://www.ipcms.fr/uploads/2020/09/cropped-dessin_logo_IPCMS_couleur_vectoriel_r%C3%A9%C3%A9quilibr%C3%A9-2.png" alt="IPCMS logo" align="center"></a>
</p>

## Documentation

The documenation is hosted on [GitHub][github] here: [atomes documentation][atomes-doc]

## Tutorials

Tutorial are regrouped and hosted on [GitHub][github] here: [atomes tutorials][atomes-tuto]

[license]:https://img.shields.io/badge/License-AGPL_v3%2B-blue
[dev_status]:https://www.repostatus.org/badges/latest/active.svg
[slr]:https://www.ipcms.fr/sebastien-le-roux/
[cnrs]:https://www.cnrs.fr/
[ipcms]:https://www.ipcms.fr/
[github]:https://github.com/
[jekyll]:https://jekyllrb.com/
[atomes]:https://atomes.ipcms.fr/
[atomes-doc]:https://slookeur.github.io/atomes-doc/
[atomes-tuto]:https://slookeur.github.io/atomes-tuto/
[dlpoly]:https://www.scd.stfc.ac.uk/Pages/DL_POLY.aspx
[lammps]:https://lammps.sandia.gov/
[cpmd]:http://www.cpmd.org
[cp2k]:http://cp2k.berlios.de
[gtk]:https://www.gtk.org/
[openmp]:https://www.openmp.org/
