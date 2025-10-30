# Adding new source code file(s) to the **atomes** software

The following codes are frameworks designed to help you prepare any new file to be added to **atomes** 

## C file `.c`

```C
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

// The next lines are Doxygen comments to describe the contents of the file
/*!
* @file file_name.c 
* @short File content description
* @author Your name <your@email.com>
*/

// The next lines are for potential readers in raw format
/*
* This file: 'file_name.c'
*
* Contains:
*

 - This file does this !

*
* List of functions:

  Functions are listed, sorted by type.

*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"

```

## FORTRAN file `.F90`

```F90
! This file is part of the 'atomes' software.
!
! 'atomes' is free software: you can redistribute it and/or modify it under the terms
! of the GNU Affero General Public License as published by the Free Software Foundation,
! either version 3 of the License, or (at your option) any later version.
!
! 'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
! without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
! See the GNU General Public License for more details.
!
! You should have received a copy of the GNU Affero General Public License along with 'atomes'.
! If not, see <https://www.gnu.org/licenses/>
!
! Copyright (C) 2022-2025 by CNRS and University of Strasbourg
!

! The next lines are Doxygen comments to describe the contents of the file
!>
!! @file file_name.F90
!! @short File content description
!! @author Your name <your@email.com>
```

 >[!WARNING]
 > For the compiler to process this remember to modify the `Makefile` accordingly.
