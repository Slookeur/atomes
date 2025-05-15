/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2025 by CNRS and University of Strasbourg */

/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Atomes major version */
#define ATOMES_MAJOR_VERSION 1

/* Atomes minor version */
#define ATOMES_MINOR_VERSION 1

/* Atomes patch version */
#define ATOMES_PATCH_VERSION 18

/* Define to dummy `main' function (if any) required to link to the Fortran
   libraries. */
/* #undef FC_DUMMY_MAIN */

/* Define if F77 and FC dummy `main' functions are identical. */
/* #undef FC_DUMMY_MAIN_EQ_F77 */

/* Define to a macro mangling the given C identifier (in lower and upper
   case), which must not contain underscores, for linking with Fortran. */
#define FC_FUNC(name,NAME) name ## _

/* As FC_FUNC, but for C identifiers containing underscores. */
#define FC_FUNC_(name,NAME) name ## _

/* GETTEXT package name */
#define GETTEXT_PACKAGE "atomes"

/* Name of package */
#define PACKAGE "atomes"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "atomes@ipcms.unistra.fr"

/* Define to the full name of this package. */
#define PACKAGE_NAME "atomes"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "atomes 1.1.18"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "atomes"

/* Define to the home page for this package. */
#define PACKAGE_URL "https://atomes.ipcms.fr"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.1.18"

/* Version number of package */
#define VERSION "1.1.18"
