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
!>
!! @file writedata.F90
!! @short Export curve data using data received from C
!! @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>

CHARACTER (LEN=35) FUNCTION ylegend (job, nleg, idl)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: job, nleg, idl
INTEGER :: DAL, DBL, DCL, DDL, DEL
LOGICAL :: done

ylegend = ''

if (job.eq.IDGR .or. job.eq.IDGRFFT) then

  if (nleg < GRNUM) then
    DEL=nleg
  else
    DEL=nleg-(GRNUM+SQNUM+SKNUM)
  endif
  if (DEL .eq. 0) then
    if (idl .eq. 0) then
      ylegend = """g(r)[tot] Neutrons"""
    else
      ylegend = "g(r)[tot] Neutrons"
    endif
  elseif (DEL .eq. 1) then
    if (idl .eq. 0) then
      ylegend = """g(r)[tot] Neutrons - smoothed"""
    else
      ylegend = "g(r)[tot] Neutrons - smoothed"
    endif
  elseif (DEL .eq. 2) then
    if (idl .eq. 0) then
      ylegend = """G(r)[tot] Neutrons"""
    else
      ylegend = "G(r)[tot] Neutrons"
    endif
  elseif (DEL .eq. 3) then
    if (idl .eq. 0) then
      ylegend = """G(r)[tot] Neutrons - smoothed"""
    else
      ylegend = "G(r)[tot] Neutrons - smoothed"
    endif
  elseif (DEL .eq. 4) then
    if (idl .eq. 0) then
      ylegend = """g(r)[tot] X-rays"""
    else
      ylegend = "g(r)[tot] X-rays"
    endif
  elseif (DEL .eq. 5) then
    if (idl .eq. 0) then
      ylegend = """g(r)[tot] X-rays - smoothed"""
    else
      ylegend = "g(r)[tot] X-rays - smoothed"
    endif
  elseif (DEL .eq. 6) then
    if (idl .eq. 0) then
      ylegend = """G(r)[tot] X-rays"""
    else
      ylegend = "G(r)[tot] X-rays"
    endif
  elseif (DEL .eq. 7) then
    if (idl .eq. 0) then
      ylegend = """G(r)[tot] X-rays - smoothed"""
    else
      ylegend = "G(r)[tot] X-rays - smoothed"
    endif
  elseif (DEL .ge. 8) then
    DCL=7
    done=.false.
    do DAL=1, NSP
      do DBL=1, NSP
        DCL=DCL+1
        if (DCL.eq.DEL) then
          if (idl .eq. 0) then
            ylegend="""g\sij\N(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"""
          else
            ylegend="g(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"
          endif
          done=.true.
          exit
        endif
        DCL=DCL+1
        if (DCL.eq.DEL) then
          if (idl .eq. 0) then
            ylegend="""g\sij\N(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"] - smoothed"""
          else
            ylegend="g(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"] - smoothed"
          endif
          done=.true.
          exit
        endif
        DCL=DCL+1
        if (DCL.eq.DEL) then
          if (idl .eq. 0) then
            ylegend="""G\sij\N(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"""
          else
            ylegend="G(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"
          endif
          done=.true.
          exit
        endif
        DCL=DCL+1
        if (DCL.eq.DEL) then
          if (idl .eq. 0) then
            ylegend="""G\sij\N(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"] - smoothed"""
          else
            ylegend="G(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"] - smoothed"
          endif
          done=.true.
          exit
        endif
        DCL=DCL+1
        if (DCL.eq.DEL) then
          if (idl .eq. 0) then
            ylegend="""dn\sij\N(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"""
          else
            ylegend="dn(r)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"
          endif
          done=.true.
          exit
        endif
      enddo
      if(done) exit
    enddo
    if (NSP .eq. 2) then
      DCL=DCL+1
      if (DCL.eq.DEL) then
        if (idl .eq. 0) then
          ylegend="""BT\sNN\N(r)"""
        else
          ylegend="BT[NN](r)"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DEL) then
        if (idl .eq. 0) then
          ylegend="""BT\sNN\N(r) - smoothed"""
        else
          ylegend="BT[NN](r) - smoothed"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DEL) then
        if (idl .eq. 0) then
          ylegend="""BT\sNC\N(r)"""
        else
          ylegend="BT[NC](r)"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DEL) then
        if (idl .eq. 0) then
          ylegend="""BT\sNC\N(r) - smoothed"""
        else
          ylegend="BT[NC](r) - smoothed"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DEL) then
        if (idl .eq. 0) then
          ylegend="""BT\sCC\N(r)"""
        else
          ylegend="BT[CC](r)"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DEL) then
        if (idl .eq. 0) then
          ylegend="""BT\sCC\N(r) - smoothed"""
        else
          ylegend="BT[CC](r) - smoothed"
        endif
      endif
    endif
  endif

else if (job.eq.IDSQ .or. job.eq.IDSK) then

  if (nleg - (GRNUM+SQNUM) < 0) then
    DDL = nleg - GRNUM
  else
    DDL = nleg - GRNUM-SQNUM
  endif
  if (DDL .eq. 0) then
    if (idl .eq. 0) then
      ylegend = """S(q)[total] Neutrons"""
    else
      ylegend = "S(q)[total] Neutrons"
    endif
  elseif (DDL .eq. 1) then
    if (idl .eq. 0) then
      ylegend = """S(q) Neutrons - smoothed"""
    else
      ylegend = "S(q) Neutrons - smoothed"
    endif
  elseif (DDL .eq. 2) then
    if (idl .eq. 0) then
      ylegend = """Q(q)[total] Neutrons"""
    else
      ylegend = "Q(q)[total] Neutrons"
    endif
  elseif (DDL .eq. 3) then
    if (idl .eq. 0) then
      ylegend = """Q(q)[total] Neutrons - smoothed"""
    else
      ylegend = "Q(q)[total] Neutrons - smoothed"
    endif
  elseif (DDL .eq. 4) then
    if (idl .eq. 0) then
      ylegend = """S(q)[total] X-rays"""
    else
      ylegend = "S(q)[total] X-rays"
    endif
  elseif (DDL .eq. 5) then
    if (idl .eq. 0) then
      ylegend = """S(q) X-rays - smoothed"""
    else
      ylegend = "S(q) X-rays - smoothed"
    endif
  elseif (DDL .eq. 6) then
    if (idl .eq. 0) then
      ylegend = """Q(q)[total] X-rays"""
    else
      ylegend = "Q(q)[total] X-rays"
    endif
  elseif (DDL .eq. 7) then
    if (idl .eq. 0) then
      ylegend = """Q(q)[total] X-rays - smoothed"""
    else
      ylegend = "Q(q)[total] X-rays - smoothed"
    endif
  elseif (DDL .ge. 8) then
    DCL=7
    done=.false.
    do DAL=1, NSP
      do DBL=1, NSP
        DCL=DCL+1
        if (DCL.eq.DDL) then
          if (idl .eq. 0) then
            ylegend="""AL\sij\N(q)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"""
          else
            ylegend="AL (q)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"
          endif
          done=.true.
          exit
        endif
        DCL=DCL+1
        if (DCL.eq.DDL) then
          if (idl .eq. 0) then
            ylegend="""AL\sij\N(q)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"] - smoothed"""
          else
            ylegend="AL (q)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"] - smoothed"
          endif
          done=.true.
          exit
        endif
      enddo
      if(done) exit
    enddo
    do DAL=1, NSP
      do DBL=1, NSP
        DCL=DCL+1
        if (DCL.eq.DDL) then
          if (idl .eq. 0) then
            ylegend="""FZ\sij\N(q)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"""
          else
            ylegend="FZ N(q)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"
          endif
          done=.true.
          exit
        endif
        DCL=DCL+1
        if (DCL.eq.DDL) then
          if (idl .eq. 0) then
            ylegend="""FZ\sij\N(q)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"] - smoothed"""
          else
            ylegend="FZ N(q)["// &
            TL(DAL)(1:LEN_TRIM(TL(DAL)))//","//TL(DBL)(1:LEN_TRIM(TL(DBL)))//"] - smoothed"
          endif
          done=.true.
          exit
        endif
      enddo
      if(done) exit
    enddo
    if (NSP .eq. 2) then
      DCL=DCL+1
      if (DCL.eq.DDL) then
        if (idl .eq. 0) then
          ylegend="""BT\sNN\N(q)"""
        else
          ylegend="BT[NN](q)"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DDL) then
        if (idl .eq. 0) then
          ylegend="""BT\sNN\N(q) - smoothed"""
        else
          ylegend="BT[NN](q) - smoothed"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DDL) then
        if (idl .eq. 0) then
          ylegend="""BT\sNC\N(q)"""
        else
          ylegend="BT[NC](q)"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DDL) then
        if (idl .eq. 0) then
          ylegend="""BT\sNC\N(q) - smoothed"""
        else
          ylegend="BT[NC](q) - smoothed"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DDL) then
        if (idl .eq. 0) then
          ylegend="""BT\sCC\N(q)"""
        else
          ylegend="BT[CC](q)"
        endif
      endif
     DCL=DCL+1
      if (DCL.eq.DDL) then
        if (idl .eq. 0) then
          ylegend="""BT\sCC\N(q) - smoothed"""
        else
          ylegend="BT[CC](q) - smoothed"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DDL) then
        if (idl .eq. 0) then
          ylegend="""BT\sZZ\N(q)"""
        else
          ylegend="BT[ZZ](q)"
        endif
      endif
      DCL=DCL+1
      if (DCL.eq.DDL) then
        if (idl .eq. 0) then
          ylegend="""BT\sZZ\N(q) - smoothed"""
        else
          ylegend="BT[ZZ](q) - smoothed"
        endif
      endif
    endif
  endif

else if (job .eq. IDBD) then

  DDL = GRNUM+GQNUM+SQNUM+SKNUM
  do DAL=1, NSP
    do DBL=1, NSP
      if (nleg .eq. DDL) then
        if (idl .eq. 0) then
          ylegend = """% Dij ["//TL(DAL)(1:LEN_TRIM(TL(DAL)))//"-" &
          //TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"""
          done=.true.
          exit
        else
          ylegend = "% Dij ["//TL(DAL)(1:LEN_TRIM(TL(DAL)))//"-" &
          //TL(DBL)(1:LEN_TRIM(TL(DBL)))//"]"
          done=.true.
          exit
        endif
      endif
      DDL=DDL+1
    enddo
  enddo

else if (job .eq. IDAN) then

  DDL = GRNUM+GQNUM+SQNUM+SKNUM+BDNUM
  do DAL=1, NSP
    do DBL=1, NSP
      do DCL=1, NSP
        if (nleg .eq. DDL) then
          if (idl .eq. 0) then
            ylegend = """% Angle ["//TL(DAL)(1:LEN_TRIM(TL(DAL)))//"-" &
            //TL(DBL)(1:LEN_TRIM(TL(DBL)))//"-"//TL(DCL)(1:LEN_TRIM(TL(DCL)))//"]"""
          else
            ylegend = "% Angle ["//TL(DAL)(1:LEN_TRIM(TL(DAL)))//"-" &
            //TL(DBL)(1:LEN_TRIM(TL(DBL)))//"-"//TL(DCL)(1:LEN_TRIM(TL(DCL)))//"]"
          endif
        endif
        DDL=DDL+1
      enddo
    enddo
  enddo
  do DAL=1, NSP
    do DBL=1, NSP
      do DCL=1, NSP
        do DEL=1, NSP
          if (nleg .eq. DDL) then
            if (idl .eq. 0) then
              ylegend = """% Diedral ["// &
              TL(DAL)(1:LEN_TRIM(TL(DAL)))//"-"//TL(DBL)(1:LEN_TRIM(TL(DBL)))// &
              "-"//TL(DCL)(1:LEN_TRIM(TL(DCL)))//"-"//TL(DEL)(1:LEN_TRIM(TL(DEL)))//"]"""
            else
              ylegend = "% Diedral ["// &
              TL(DAL)(1:LEN_TRIM(TL(DAL)))//"-"//TL(DBL)(1:LEN_TRIM(TL(DBL)))// &
              "-"//TL(DCL)(1:LEN_TRIM(TL(DCL)))//"-"//TL(DEL)(1:LEN_TRIM(TL(DEL)))//"]"
            endif
          endif
          DDL=DDL+1
        enddo
      enddo
    enddo
  enddo

else if (job .eq. IDRI) then

  DDL = nleg-(GRNUM+GQNUM+SQNUM+SKNUM+BDNUM+ANNUM)
  if (DDL.eq.0 .or. DDL.eq.4 .or. DDL.eq.8 .or. DDL.eq.12 .or. DDL.eq.16) then
    if (idl .eq. 0) then
      ylegend= """R\sc\N(\f{Times-Italic}n\f{})"""
    else
      ylegend= "Rc(n)"
    endif
  elseif (DDL.eq.1 .or. DDL.eq.5 .or. DDL.eq.9 .or. DDL.eq.13 .or. DDL.eq.17) then
    if (idl .eq. 0) then
      ylegend= """P\sn\N(\f{Times-Italic}n\f{})"""
    else
      ylegend= "Pn(n)"
    endif
  elseif (DDL.eq.2 .or. DDL.eq.6 .or. DDL.eq.10 .or. DDL.eq.14 .or. DDL.eq.18) then
    if (idl .eq. 0) then
      ylegend= """P\smax\N(\f{Times-Italic}n\f{})"""
    else
      ylegend= "Pmax(n)"
    endif
  elseif (DDL.eq.3 .or. DDL.eq.7 .or. DDL.eq.11 .or. DDL.eq.15 .or. DDL.eq.19) then
    if (idl .eq. 0) then
      ylegend= """P\smin\N(\f{Times-Italic}n\f{})"""
    else
      ylegend= "Pmin(n)"
    endif
  endif

elseif (job .eq. IDCH) then

elseif (job .eq. IDSP) then

  DDL = nleg-(GRNUM+GQNUM+SQNUM+SKNUM+BDNUM+ANNUM+RINUM+CHNUM)
!  if (DDL .eq. 0) then
!    if (idl .eq. 0) then
!      ylegend= """Q\sl\N [average]"""
!    else
!      ylegend= "Ql [average]"
!    endif
  if (DDL.le.NSP-1) then
    do DEL=1, NSP
      if (idl .eq. 0) then
        ylegend= """Q\sl\N ["//TL(DEL)(1:LEN_TRIM(TL(DEL)))//" atoms]"""
      else
        ylegend= "Ql ["//TL(DEL)(1:LEN_TRIM(TL(DEL)))//" atoms]"
      endif
    enddo
  else
    if (idl .eq. 0) then
      ylegend= """Q\sl\N specific environment"""
    else
      ylegend= "Ql specific environment"
    endif
  endif

else

  DDL = GRNUM+GQNUM+SQNUM+SKNUM+BDNUM+ANNUM+RINUM+CHNUM+SHNUM
  do DAL=1, NSP
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD ("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD ("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD nac ("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD nac ("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
  enddo
  do DAL=1, NSP
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [x]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [x]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [y]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [y]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [z]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [z]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [xy]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [xy]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [xz]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [xz]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [yz]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [yz]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
  enddo
  do DAL=1, NSP
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [x/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [x/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [y/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [y/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [z/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [z/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [xy/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [xy/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [xz/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [xz/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
    if (nleg .eq. DDL) then
      if (idl .eq. 0) then
        ylegend= """MSD [yz/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [\cE\C\S2\N]"""
      else
        ylegend= "MSD [yz/nac]("//TL(DAL)(1:LEN_TRIM(TL(DAL)))//") [Å²]"
      endif
    endif
    DDL=DDL+1
  enddo
  if (nleg .eq. DDL) then
    if (idl .eq. 0) then
      ylegend= """Correction (x) [\cE\C\S2\N]"""
    else
      ylegend= "Correction (x) [Å²]"
    endif
  endif
  DDL=DDL+1
  if (nleg .eq. DDL) then
    if (idl .eq. 0) then
      ylegend= """Correction (y) [\cE\C\S2\N]"""
    else
      ylegend= "Correction (y) [Å²]"
    endif
  endif
  DDL=DDL+1
  if (nleg .eq. DDL) then
    if (idl .eq. 0) then
      ylegend= """Correction (z) [\cE\C\S2\N]"""
    else
      ylegend= "Correction (z) [Å²]"
    endif
  endif
  DDL=DDL+1
  if (nleg .eq. DDL) then
    if (idl .eq. 0) then
      ylegend= """Drift (x) [ms\S-1\N]"""
    else
      ylegend= "Drift (x) m/s"
    endif
  endif
  DDL=DDL+1
  if (nleg .eq. DDL) then
    if (idl .eq. 0) then
      ylegend= """Drift (y) [ms\S-1\N]"""
    else
      ylegend= "Drift (y) (m/s)"
    endif
  endif
  DDL=DDL+1
  if (nleg .eq. DDL) then
    if (idl .eq. 0) then
      ylegend= """Drift (z) [ms\S-1\N]"""
    else
      ylegend= "Drift (z) (m/s)"
    endif
  endif

endif

END FUNCTION

CHARACTER (LEN=65) FUNCTION xlegend (job, nleg, idl, cdc)

USE PARAMETERS

IMPLICIT NONE

INTEGER, INTENT(IN) :: job, nleg, idl
DOUBLE PRECISION, INTENT(IN) :: cdc
INTEGER :: il
CHARACTER (LEN=6), DIMENSION(5) :: tpsunit = (/'t [fs]', 't [ps]', 't [ns]', 't [us]', 't [ms]' /)
INTERFACE
  CHARACTER (LEN=7) FUNCTION getunit()
  END FUNCTION
END INTERFACE

xlegend = ''
if (idl .eq. 0) then
  if (job.eq.IDGR .or. job.eq.IDGRFFT) then
    xlegend = "r[\cE\C]"""
  elseif (job.eq.IDSQ .or. job.eq.IDSK) then
    xlegend = "q[\cE\C\S-1\N]"""
  elseif (job .eq. IDBD) then
    xlegend = "Dij[\cE\C\]"""
  elseif (job .eq. IDAN) then
    if ( nleg .lt. GRNUM+GQNUM+SQNUM+SKNUM+BDNUM+NSP*NSP*NSP ) then
      xlegend = "Angles[\c:\C]"""
    else
      xlegend = "Dihedrals[\c:\C]"""
    endif
  elseif (job .eq. IDRI) then
    xlegend = "Size \f{Times-Italic}n\f{} of the ring [total number of nodes]"""
  elseif (job .eq. IDCH) then
    xlegend = "Size \f{Times-Italic}n\f{} of the chain [total number of nodes]"""
  elseif (job .eq. IDSP) then
    xlegend = "Q\sl\N"""
  else
    il = AnINT(cdc)
    if (il .eq. 4) then
      xlegend = "\f{12}m\f{}"""
    else
      xlegend = tpsunit(il)//""""
    endif
  endif
else
  if (job.eq.IDGR .or. job.eq.IDGRFFT) then
    xlegend = "r[Å]"
  elseif (job.eq.IDSQ .or. job.eq.IDSK) then
    xlegend = "q[Å-1]"
  elseif (job .eq. IDBD) then
    xlegend = "Dij[Å]"
  elseif (job .eq. IDAN) then
    if ( nleg .lt. GRNUM+GQNUM+SQNUM+SKNUM+BDNUM+NSP*NSP*NSP ) then
      xlegend = "Angles[°]"
    else
      xlegend = "Dihedrals[°]"
    endif
  elseif (job .eq. IDRI) then
    xlegend = "Size n of the ring [total number of nodes]"
  elseif (job .eq. IDCH) then
    xlegend = "Size n of the chain [total number of nodes]"
  elseif (job .eq. IDSP) then
    xlegend = "Ql"
  else
    il = AnINT(cdc)
    xlegend = tpsunit(il)
  endif
endif

END FUNCTION

SUBROUTINE prep_file (scf, sfi, tfile, &
                      scalex, scaley, mdc, rdc, idc) BIND (C,NAME='prep_file_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: scf, rdc, idc, tfile
INTEGER (KIND=c_int), INTENT(IN) :: scalex, scaley
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: sfi
CHARACTER (LEN=scf) :: sfile
REAL (KIND=c_double), INTENT(IN) :: mdc
CHARACTER (LEN=5) :: xaxis="xaxis", yaxis="yaxis"
CHARACTER (LEN=65) :: xlabel

INTERFACE
  CHARACTER(LEN=65) FUNCTION xlegend (job, nleg, idl, cdc)
    INTEGER, INTENT(IN) :: job, nleg, idl
    DOUBLE PRECISION, INTENT(IN) :: cdc
  END FUNCTION
END INTERFACE

xlabel = xlegend(rdc, idc, tfile, mdc)

do i=1, scf
  sfile(i:i) = sfi(i)
enddo

open(unit=200, file=sfile, action="write", status='unknown')
if (tfile .eq. 0) then
  write (200, 001)
  write (200, 002) xaxis, xlabel
  write (200, 003) xaxis
  if (scalex .eq. 1) write (200, 016)
  write (200, 004) xaxis, 1.0
  write (200, 005) xaxis, 0.5
  write (200, 007) xaxis
  write (200, 008) xaxis
  write (200, 003) yaxis
  if (scaley .eq. 1) write (200, 017)
  write (200, 004) yaxis, 1.0
  write (200, 005) yaxis, 0.5
  write (200, 007) yaxis
  write (200, 008) yaxis
  write (200, 009)
  write (200, 010)
endif

001 FORMAT ("@with g0")
002 FORMAT ("@    ",A5," label """,A65)
003 FORMAT ("@    ",A5," tick on")
004 FORMAT ("@    ",A5," tick major size ",f8.6)
005 FORMAT ("@    ",A5," tick minor size ",f8.6)
007 FORMAT ("@    ",A5," ticklabel on")
008 FORMAT ("@    ",A5," ticklabel char size 0.800000")
009 FORMAT ("@    legend on")
010 FORMAT ("@    legend box linestyle 0")
016 FORMAT ("@    xaxes scale Logarithmic")
017 FORMAT ("@    yaxes scale Logarithmic")

END SUBROUTINE

SUBROUTINE append_to_file (ndata, xdata, ydata, &
                           mdc, tdata, rdc, idc, &
                           tfile, nfile, afile, lcname, cstring) BIND (C,NAME='append_to_file_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: rdc, idc
INTEGER (KIND=c_int), INTENT(IN) :: ndata
INTEGER (KIND=c_int), INTENT(IN) :: tfile, tdata
INTEGER (KIND=c_int), INTENT(IN) :: nfile, afile, lcname
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: cstring
CHARACTER (LEN=lcname) :: cname
INTEGER :: WA
INTEGER :: start, step
REAL (KIND=c_double), INTENT(IN) :: mdc
REAL (KIND=c_double), DIMENSION(ndata), INTENT(IN) :: xdata, ydata
CHARACTER (LEN=65) :: xlabel
INTERFACE
  CHARACTER(LEN=65) FUNCTION xlegend (job, nleg, idl, cdc)
    INTEGER, INTENT(IN) :: job, nleg, idl
    DOUBLE PRECISION, INTENT(IN) :: cdc
  END FUNCTION
  CHARACTER (LEN=35) FUNCTION ylegend (job, nleg, idl)
    INTEGER, INTENT(IN) :: job, nleg, idl
  END FUNCTION
END INTERFACE

do i=1, lcname
  cname(i:i) = cstring(i)
enddo

! ylabel = ylegend(rdc, idc, tfile)
xlabel = xlegend(rdc, idc, tfile, mdc)

step=1
start=1
if (rdc .eq. IDRI) start=3
if (rdc .eq. IDCH) start=2
if (rdc .eq. IDSP) then
  start=2
  step=1
endif

if (tfile .eq. 0) then
  call CHARINT(Nom, nfile)
  write (200, *) "@target G0.S",Nom(2:LEN_TRIM(Nom))
  write (200, *) "@s",Nom(2:LEN_TRIM(Nom))," legend """, cname, """"
  if (tdata .eq. 1) then
    write (200, *) "@s",Nom(2:LEN_TRIM(Nom))," line linestyle 0"
    write (200, 019)
  else
    write (200, 013)
  endif
else
  write (200, 015) xlabel, cname
endif
do WA=start, ndata, step
  write (200, 014) xdata(WA), ydata(WA)
enddo
write (200, *)

if (nfile  .eq. afile-1) close(200)

013 FORMAT ("@type xy")
014 FORMAT (f20.10,3x,f20.10)
015 FORMAT ("# ",A65," ",A35)
019 FORMAT ("@type bar")

END SUBROUTINE

SUBROUTINE save_to_file (scf, sfi, &
                         ndata, xdata, ydata, &
                         scalex, scaley, tdata, &
                         mdc, rdc, idc, tfile, lcname, cstring) BIND (C,NAME='save_to_file_')

USE PARAMETERS

IMPLICIT NONE

INTEGER (KIND=c_int), INTENT(IN) :: scf, rdc, idc, tfile
INTEGER (KIND=c_int), INTENT(IN) :: ndata, scalex, scaley, tdata, lcname
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: cstring
CHARACTER (KIND=c_char), DIMENSION(*), INTENT(IN) :: sfi
CHARACTER (LEN=scf) :: sfile
CHARACTER (LEN=lcname) :: cname
INTEGER :: step, start
REAL (KIND=c_double), INTENT(IN) :: mdc
REAL (KIND=c_double), DIMENSION(ndata), INTENT(IN) :: xdata, ydata
INTEGER :: WA, WB
CHARACTER (LEN=5) :: xaxis="xaxis", yaxis="yaxis"
CHARACTER (LEN=65) :: xlabel
INTERFACE
  CHARACTER(LEN=65) FUNCTION xlegend (job, nleg, idl, cdc)
    INTEGER, INTENT(IN) :: job, nleg, idl
    DOUBLE PRECISION, INTENT(IN) :: cdc
  END FUNCTION
  CHARACTER (LEN=35) FUNCTION ylegend (job, nleg, idl)
    INTEGER, INTENT(IN) :: job, nleg, idl
  END FUNCTION
END INTERFACE

!ylabel = ylegend(rdc, idc, tfile)
do i=1, lcname
  cname(i:i) = cstring(i)
enddo
xlabel = xlegend(rdc, idc, tfile, mdc)

start=1
step=1
if (rdc .eq. IDRI) start=3
if (rdc .eq. IDCH) start=2
if (rdc .eq. IDSP) then
  start=2
  step=1
endif

do i=1, scf
  sfile(i:i) = sfi(i)
enddo

open(unit=100, file=sfile, action="write", status='unknown')

if (tfile .eq. 0) then
  write (100, 001)
  write (100, 002) xaxis, xlabel
  write (100, 003) xaxis
  if (scalex .eq. 1) write (100, 016)
  write (100, 004) xaxis, 1.0
  write (100, 005) xaxis, 0.5
  write (100, 007) xaxis
  write (100, 008) xaxis
  write (100, 006) yaxis, cname
  write (100, 003) yaxis
  if (scaley .eq. 1) write (100, 017)
  write (100, 004) yaxis, 1.0
  write (100, 005) yaxis, 0.5
  write (100, 007) yaxis
  write (100, 008) yaxis
  write (100, 009)
  write (100, 010)
  if (rdc.eq.IDRI .or. rdc.eq.IDCH) then
    if (tdata .eq. 1) then
      WB=0
      do WA=start, ndata
        if (ydata(WA) .ne. 0) then
          call CHARINT(Nom, WB)
          call CHARINT(Nom2, WA)
          write (100, *) "@    s",Nom(2:LEN_TRIM(Nom))," legend  ""t",Nom2(2:LEN_TRIM(Nom2)),""""
          write (100, *) "@    s",Nom(2:LEN_TRIM(Nom))," line linestyle 0"
          write (100, *) "@    s",Nom(2:LEN_TRIM(Nom))," symbol fill pattern 14"
          WB=WB+1
        endif
      enddo
      WB=0
      do WA=start, ndata
        if (ydata(WA) .ne. 0) then
          call CHARINT(Nom, WB)
          write (100, *) "@target G0.S",Nom(2:LEN_TRIM(Nom))
          write (100, 019)
          write (100, 018) WA, ydata(WA)
          WB=WB+1
        endif
      enddo
    else
      write (100, 011) cname
      write (100, 012)
      write (100, 013)
      do WA=start, ndata
        if (ydata(WA) .ne. 0.0) write (100, 018) WA, ydata(WA)
      enddo
    endif
  elseif (rdc .eq. IDSP) then
    if (tdata .eq. 1) then
      WB=0
      do WA=start, ndata, step
        if (ydata(WA) .ne. 0) then
          call CHARINT(Nom, WB)
          call CHARINT(Nom2, WA-1)
          write (100, *) "@    s",Nom(2:LEN_TRIM(Nom))," legend  ""t",Nom2(2:LEN_TRIM(Nom2)),""""
          write (100, *) "@    s",Nom(2:LEN_TRIM(Nom))," line linestyle 0"
          write (100, *) "@    s",Nom(2:LEN_TRIM(Nom))," symbol fill pattern 14"
          WB=WB+1
        endif
      enddo
      WB=0
      do WA=start, ndata, step
        if (ydata(WA) .ne. 0) then
          call CHARINT(Nom, WB)
          write (100, *) "@target G0.S",Nom(2:LEN_TRIM(Nom))
          write (100, 019)
          write (100, 018) WA, ydata(WA)
          WB=WB+1
        endif
      enddo
    else
      write (100, 011) cname
      write (100, 012)
      write (100, 013)
      ! Check this
      do WA=start, ndata, 2
        write (100, 018) WA, ydata(WA)
      enddo
    endif
  elseif (rdc.eq.IDSQ .or. rdc.eq.IDSK) then
    write (100, 011) cname
    write (100, 012)
    write (100, 013)
    do WA=start, ndata
      write (100, 014) xdata(WA), ydata(WA)
    enddo
  else
    if (tdata .eq. 0) then
      write (100, 011) cname
      write (100, 012)
      write (100, 013)
    else
      write (100, 011) cname
      write (100, 012)
      write (100, 019)
    endif
    do WA=start, ndata
      write (100, 014) xdata(WA), ydata(WA)
    enddo
  endif
else
  write (100, 015) xlabel, cname
  do WA=start, ndata, step
    write (100, 014) xdata(WA), ydata(WA)
  enddo
endif
close(100)

001 FORMAT ("@with g0")
002 FORMAT ("@    ",A5," label """,A65)
003 FORMAT ("@    ",A5," tick on")
004 FORMAT ("@    ",A5," tick major size ",f8.6)
005 FORMAT ("@    ",A5," tick minor size ",f8.6)
006 FORMAT ("@    ",A5," label ",A35)
007 FORMAT ("@    ",A5," ticklabel on")
008 FORMAT ("@    ",A5," ticklabel char size 0.800000")
009 FORMAT ("@    legend on")
010 FORMAT ("@    legend box linestyle 0")
011 FORMAT ("@    s0 legend  """,A50,"""")
012 FORMAT ("@target G0.S0")
013 FORMAT ("@type xy")
014 FORMAT (f20.10,3x,f20.10)
015 FORMAT ("# ",A65," ",A35)
016 FORMAT ("@    xaxes scale Logarithmic")
017 FORMAT ("@    yaxes scale Logarithmic")
018 FORMAT (i4,5x,f20.10)
019 FORMAT ("@type bar")

END SUBROUTINE
