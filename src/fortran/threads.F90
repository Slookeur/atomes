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
! Copyright (C) 2022-2026 by CNRS and University of Strasbourg
!
!>
!! @file threads.F90
!! @short OpenMP utility functions
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

INTEGER FUNCTION GET_THREAD_START (NOBJ, NTHREADS, THREAD_ID)

  IMPLICIT NONE

  INTEGER, INTENT(IN) :: NOBJ, NTHREADS, THREAD_ID
  INTEGER :: TMP
  REAL :: VA, VB

  VA = NOBJ
  VB = NTHREADS
  TMP = INT (VA/VB)
  GET_THREAD_START = TMP * THREAD_ID + 1

END FUNCTION

INTEGER FUNCTION GET_THREAD_END (NOBJ, NTHREADS, THREAD_ID)

  IMPLICIT NONE

  INTEGER, INTENT(IN) :: NOBJ, NTHREADS, THREAD_ID
  INTEGER :: TMP
  REAL :: VA, VB

  VA = NOBJ
  VB = NTHREADS
  TMP = INT (VA/VB)
  if (THREAD_ID .eq. NTHREADS-1) then
    GET_THREAD_END = NOBJ
  else
    GET_THREAD_END = TMP * (THREAD_ID+1)
  endif

END FUNCTION
