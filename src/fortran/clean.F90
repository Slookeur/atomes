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
! Copyright (C) 2022-2024 by CNRS and University of Strasbourg
!
!>
!! @file clean.F90
!! @short Clean up Fortran90 data
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif
#include "version.h"

SUBROUTINE profree () BIND (C,NAME='profree_')

USE PARAMETERS

IMPLICIT NONE

! deallocation of possibly remaining data

if (allocated(FULLPOS)) deallocate(FULLPOS)
if (allocated(TAB_OF_TYPE)) deallocate(TAB_OF_TYPE)
if (allocated(LOT)) deallocate(LOT)
if (allocated(NBSPBS)) deallocate(NBSPBS)
if (allocated(TL)) deallocate(TL)
if (allocated(ATOMID)) deallocate(ATOMID)
if (allocated(MASS)) deallocate(MASS)
if (allocated(RVDW)) deallocate(RVDW)
if (allocated(Xi)) deallocate(Xi)
if (allocated(Gr_CUT)) deallocate(Gr_CUT)
if (allocated(NSCATTL)) deallocate(NSCATTL)
if (allocated(XSCATTL)) deallocate(XSCATTL)


END SUBROUTINE
