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
!! @file dvtb.F90
!! @short Model expansion for algorithmic purposes
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

LOGICAL FUNCTION dvtbox(ST, NAS, NAT, NPOS)

!
! Create a superlattice
! Size of the superlattice will be NBX*NBX*NBX with NBX >= 3
! NBX is set in dmtx.f90
!

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: ST, NAS, NAT
DOUBLE PRECISION, DIMENSION(NAT,3), INTENT(INOUT) :: NPOS
DOUBLE PRECISION, DIMENSION(3) :: DVT
INTEGER :: iid, jid, kid, lid, mid, nid

iid=0
do jid=-NBX+(NBX/2)+1, NBX-(NBX/2)-1, 1
  do kid=-NBX+(NBX/2)+1, NBX-(NBX/2)-1, 1
    do lid=-NBX+(NBX/2)+1, NBX-(NBX/2)-1, 1
      if (NCELLS .gt. 1) then
        mid = ST
      else
        mid = 1
      endif
      DVT(1)=jid*THE_BOX(mid)%lvect(1,1)+kid*THE_BOX(mid)%lvect(2,1)+lid*THE_BOX(mid)%lvect(3,1)
      DVT(2)=jid*THE_BOX(mid)%lvect(1,2)+kid*THE_BOX(mid)%lvect(2,2)+lid*THE_BOX(mid)%lvect(3,2)
      DVT(3)=jid*THE_BOX(mid)%lvect(1,3)+kid*THE_BOX(mid)%lvect(2,3)+lid*THE_BOX(mid)%lvect(3,3)
      do mid=1, NAS
        iid = iid + 1
        do nid=1, 3
          NPOS(iid,nid)=FULLPOS(mid,nid,ST)+DVT(nid)
        enddo
      enddo
    enddo
  enddo
enddo

dvtbox=.true.

END FUNCTION
