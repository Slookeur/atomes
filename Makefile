# This file is part of Atomes.

# Atomes is free software: you can redistribute it and/or modify it under the terms
# of the GNU Affero General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.

# Atomes is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.

# You should have received a copy of the GNU Affero General Public License along with Atomes.
# If not, see <https://www.gnu.org/licenses/>

# The targets to build are 'atomes' or 'debug'

LINUX = 1
WINDOWS = 0

# The next line defines the GTK version !
GTKV = 3
ifeq ($(GTKV),4)
  DGTK = -DGTK4 -DGTKGLAREA
  # -DGDK_DISABLE_DEPRECATION_WARNINGS -DGTK_DISABLE_DEPRECATION_WARNINGS
  # To enforce strictly newest GTK4 functions: -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
  IGTK = `pkg-config --cflags gtk4 epoxy glu libxml-2.0 pangoft2 libavutil libavcodec libavformat libswscale`
  LGTK = `pkg-config --libs gtk4 epoxy glu libxml-2.0 pangoft2 libavutil libavcodec libavformat libswscale`
else
  DGTK = -DGTK3 -DGTKGLAREA -DMENU_ICONS
  IGTK = `pkg-config --cflags gtk+-3.0 epoxy glu libxml-2.0 pangoft2 libavutil libavcodec libavformat libswscale`
  LGTK = `pkg-config --libs gtk+-3.0 epoxy glu libxml-2.0 pangoft2 libavutil libavcodec libavformat libswscale`
endif

# OpenMP
OPENMP = 1
ifeq ($(OPENMP),1)
  DOMP = -DOPENMP -fopenmp
else
  DOMP =
endif

ifeq ($(LINUX),1)

  # Use a specific compiler version
  ifeq ($(GCCVER),)
    FC = gfortran
    CC = gcc
    LD = gcc
  else
    FC = gfortran-$(GCCVER)
    CC = gcc-$(GCCVER)
    LD = gcc-$(GCCVER)
  endif
  LIBS = $(LGTK) -lm -lgfortran

  DOS = -DLINUX
  CPPFLAGS = -DCODEBLOCKS -DPACKAGE_PREFIX=\"./\" -DPACKAGE_LIBEXEC=\"./\"
  LDFLGS = $(DOMP)

  RM = rm
  RMFLAGS = -f
  MV = mv
  CP = cp
  CPFLAGS = -f
  ECHO = echo

  EXT =

  FCVER = `$(FC) -dumpfullversion`
  CCVER = `$(CC) -dumpfullversion`

endif

ifeq ($(WINDOWS),1)

  DISK = C:/
  ARCH = 64
  CPU = x86_64
  DEV = $(DISK)msys64/usr/
  COMP = $(DISK)msys64/mingw$(ARCH)/bin/
  GTK = $(DISK)msys64/mingw$(ARCH)/
  PKG = $(GTK)bin/pkg-config

  FC = $(COMP)$(CPU)-w64-mingw32-gfortran
  CC = $(COMP)$(CPU)-w64-mingw32-gcc
  LD = $(COMP)$(CPU)-w64-mingw32-gfortran $(DOMP)

  CPPFLAGS =
  LDFLG = -lz -liphlpapi

  ifeq ($(GTKV), 4)
    IGTK = -pthread -mms-bitfields -IC:/msys64/mingw64/include/gtk-4.0 -IC:/msys64/mingw64/include/cairo \
		-IC:/msys64/mingw64/include/pango-1.0 -IC:/msys64/mingw64/include/gdk-pixbuf-2.0 \
		-IC:/msys64/mingw64/include -IC:/msys64/mingw64/include/libxml2 \
		-IC:/msys64/mingw64/include/fribidi -IC:/msys64/mingw64/include \
		-IC:/msys64/mingw64/include/pixman-1 -IC:/msys64/mingw64/include/harfbuzz \
		-IC:/msys64/mingw64/include/freetype2 -IC:/msys64/mingw64/include/webp -DLIBDEFLATE_DLL \
		-IC:/msys64/mingw64/include/graphene-1.0 -IC:/msys64/mingw64/lib/graphene-1.0/include -mfpmath=sse -msse -msse2 \
		-IC:/msys64/mingw64/include/libpng16 \
		-IC:/msys64/mingw64/include/glib-2.0 -IC:/msys64/mingw64/lib/glib-2.0/include

    LGTK = -LC:/msys64/mingw64/lib -lgtk-4 -lpangowin32-1.0 -lharfbuzz -lpangocairo-1.0 -lpango-1.0 -lgdk_pixbuf-2.0 \
		-lcairo-gobject -lcairo -lgraphene-1.0 -lgio-2.0 -lglib-2.0 -lintl -lgobject-2.0 -lxml2  -lpangoft2-1.0 \
		-lepoxy -lavutil -lavcodec -lavformat -lswscale
  else
    IGTK = -pthread -mms-bitfields -IC:/msys64/mingw64/include/gtk-3.0 -IC:/msys64/mingw64/include/cairo \
		-IC:/msys64/mingw64/include/pango-1.0 -IC:/msys64/mingw64/include/atk-1.0 \
		-IC:/msys64/mingw64/include/gdk-pixbuf-2.0 -IC:/msys64/mingw64/include/libxml2 \
		-IC:/msys64/mingw64/include/fribidi -IC:/msys64/mingw64/include \
		-IC:/msys64/mingw64/include/pixman-1 -IC:/msys64/mingw64/include/harfbuzz \
		-IC:/msys64/mingw64/include/freetype2 -IC:/msys64/mingw64/include/webp -DLIBDEFLATE_DLL \
		-IC:/msys64/mingw64/include/libpng16 \
		-IC:/msys64/mingw64/include/glib-2.0 -IC:/msys64/mingw64/lib/glib-2.0/include

    LGTK = -LC:/msys64/mingw64/lib -lgtk-3 -lgdk-3 -lz -lgdi32 -limm32 -lshell32 -lole32 \
		-Wl,-luuid -lwinmm -ldwmapi -lsetupapi -lcfgmgr32 -lpangowin32-1.0 -lpangocairo-1.0 \
		-latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lepoxy -lxml2 -lpangoft2-1.0 \
		-lpango-1.0 -lgobject-2.0 -lglib-2.0 -lintl -lfontconfig -lfreetype -lavutil -lavcodec -lavformat -lswscale
  endif
  LIB = $(LGTK)

  ifeq ($(MAKECMDGOALS), atomes)
    LIBS = $(LIB)
    LDFLGS = $(LDFLG)
  endif
  ifeq ($(MAKECMDGOALS), debug)
    LIBS = $(LIB) -lssp
    LDFLGS = $(LDFLG) -fno-stack-protector
  endif

  DOS =

  RM = $(DEV)bin/rm
  RMFLAGS = -f
  CP = $(DEV)bin/cp
  MV = $(DEV)bin/mv
  CAT = $(DEV)bin/cat
  CPFLAGS = -f
  ECHO = $(DEV)bin/echo

  WINDRES = $(COMP)windres
  WIN_ATOMES = $(OBJ)win_atomes.o
  WIN_STARTUP = $(OBJ)win_startup.o

  EXT=".exe"
  FCVER = 14.2.0
  CCVER = 14.2.0

endif

SRC = src/
GUI = src/gui/
WORK = src/workspace/
PROJ = src/project/
CALC = src/calc/
DLPOLY = src/calc/dl_poly/
LAMMPS = src/calc/lammps/
FIELDS = src/calc/force_fields/
CPMD = src/calc/cpmd/
CP2K = src/calc/cp2k/
CURVE = src/curve/
OGL = src/opengl/
GLWIN = src/opengl/win/
GLDRAW = src/opengl/draw/
GLEDIT = src/opengl/edit/
FOR = src/fortran/
OBJ = obj/
BIN = bin/

INC = -I$(SRC) -I$(GUI) -I$(WORK) -I$(PROJ) -I$(PROJ)readers/ -I$(CALC) -I$(DLPOLY) -I$(LAMMPS) -I$(FIELDS) -I$(CPMD) -I$(CP2K) -I$(CURVE) -I$(GLWIN) -I$(GLEDIT) -I$(GLDRAW) -I$(OGL) -I.
INCLUDES = $(INC) $(IGTK)
# -DGDK_DISABLE_DEPRECATION_WARNINGS -DGTK_DISABLE_DEPRECATION_WARNINGS
# To enforce strictly newest GTK4 functions: -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED

ifeq ($(MAKECMDGOALS),atomes)
  FCFLAGS = -O2 -cpp
  CFLAGS = -O2
  LDFLAGS = $(LIBS) $(LDFLGS)
  DEFS = -DHAVE_CONFIG_H $(DGTK) $(DOS)
endif

ifeq ($(MAKECMDGOALS),debug)
  FCFLAGS = -fno-second-underscore -O0 -Wall -g3 -pg -ggdb3 -cpp -dA -dD -dH -dp -dP -fvar-tracking -fbounds-check -fstack-protector-all
  CFLAGS = -O0 -Wall -g3 -pg -ggdb3 -cpp -dA -dD -dH -dp -dP -fvar-tracking -fbounds-check -fstack-protector-all -Wduplicated-cond
  LDFLAGS = $(LIBS) $(LDFLGS) -pg
  DEFS = -DHAVE_CONFIG_H -DDEBUG $(DGTK) $(DOS)
endif

PROGRAM = atomes

# The objects which make the executable

SOURCES_F90 := $(wildcard $(FOR)*.F90)
OBJECTS_F90 := $(patsubst $(FOR)%.F90, $(OBJ)%.o, $(SOURCES_F90))
MODOBJECTS_F90 := $(OBJ)mendeleiev.o $(OBJ)parameters.o

OBJECTS_c = $(OBJ)global.o $(OBJ_GUI) $(OBJ_WORK) $(OBJ_PROJ) $(OBJ_CURVE) \
			$(OBJ_CALC) $(OBJ_POLY) $(OBJ_LAMMPS) $(OBJ_FIELD) $(OBJ_CPMD) $(OBJ_CP2K) $(OBJ_OGL)

OBJ_GUI = \
	$(OBJ)gtk-misc.o \
	$(OBJ)work_menu.o \
	$(OBJ)edit_menu.o \
	$(OBJ)calc_menu.o \
	$(OBJ)tools.o \
	$(OBJ)gui.o \
	$(OBJ)preferences.o \
	$(OBJ)initc.o \
	$(OBJ)callbacks.o \
	$(OBJ)interface.o \
	$(OBJ)bdcall.o \
	$(OBJ)grcall.o \
	$(OBJ)sqcall.o \
	$(OBJ)ringscall.o \
	$(OBJ)chainscall.o \
	$(OBJ)msdcall.o \
	$(OBJ)spcall.o \
	$(OBJ)main.o

OBJ_WORK = \
	$(OBJ)modelinfo.o \
	$(OBJ)workinfo.o \
	$(OBJ)workspace.o

OBJ_PROJ = \
	$(OBJ)read_isaacs.o \
	$(OBJ)read_cif.o \
	$(OBJ)read_coord.o \
	$(OBJ)read_xyz.o \
	$(OBJ)read_c3d.o \
	$(OBJ)read_trj.o \
	$(OBJ)read_vas.o \
	$(OBJ)read_pdb.o \
	$(OBJ)read_hist.o \
	$(OBJ)read_npt.o \
	$(OBJ)update_p.o \
	$(OBJ)init_p.o \
	$(OBJ)debugio.o \
	$(OBJ)read_field.o \
	$(OBJ)read_qm.o \
	$(OBJ)read_opengl.o \
	$(OBJ)read_curve.o \
	$(OBJ)read_mol.o \
	$(OBJ)read_bond.o \
	$(OBJ)open_p.o \
	$(OBJ)close_p.o \
	$(OBJ)save_field.o \
	$(OBJ)save_qm.o \
	$(OBJ)save_opengl.o \
	$(OBJ)save_curve.o \
	$(OBJ)save_mol.o \
	$(OBJ)save_bond.o \
	$(OBJ)save_p.o \
	$(OBJ)project.o

OBJ_CALC = \
	$(OBJ)calc.o

OBJ_CURVE = \
	$(OBJ)tab-1.o \
	$(OBJ)tab-2.o \
	$(OBJ)tab-3.o \
	$(OBJ)tab-4.o \
	$(OBJ)cedit.o \
	$(OBJ)datab.o \
	$(OBJ)cwidget.o \
	$(OBJ)glyph.o \
	$(OBJ)labels.o \
	$(OBJ)title.o \
	$(OBJ)legend.o \
	$(OBJ)xaxis.o \
	$(OBJ)yaxis.o \
	$(OBJ)frame.o \
	$(OBJ)draw.o \
	$(OBJ)show.o \
	$(OBJ)w_data.o \
	$(OBJ)w_img.o \
	$(OBJ)m_curve.o \
	$(OBJ)w_curve.o \
	$(OBJ)curve.o

OBJ_POLY = \
	$(OBJ)dlp_control.o \
	$(OBJ)dlp_init.o \
	$(OBJ)dlp_active.o \
	$(OBJ)dlp_copy.o \
	$(OBJ)dlp_comp.o \
	$(OBJ)dlp_viz.o \
	$(OBJ)dlp_mol.o \
	$(OBJ)dlp_atom.o \
	$(OBJ)dlp_ff_match.o \
	$(OBJ)dlp_edit.o \
	$(OBJ)dlp_print.o \
	$(OBJ)dlp_field.o

OBJ_LAMMPS = \
	$(OBJ)la_print.o \

OBJ_FIELD = \
	$(OBJ)force_fields.o

#	$(OBJ)amber94.o \
#	$(OBJ)amber96.o \
#	$(OBJ)amber98.o \
#	$(OBJ)amber99.o \
#	$(OBJ)cff91.o \
#	$(OBJ)charmm22_prot.o \
#	$(OBJ)charmm22_prot_metals.o \
#	$(OBJ)charmm35_ethers.o \
#	$(OBJ)charmm36_carb.o \
#	$(OBJ)charmm36_cgenff.o \
#	$(OBJ)charmm36_lipid.o \
#	$(OBJ)charmm36_na.o \
#	$(OBJ)charmm36_prot.o \
#	$(OBJ)charmm36_prot_metals.o \
#	$(OBJ)charmm_silicates.o \
#	$(OBJ)compass.o \
#	$(OBJ)cvff.o \
#	$(OBJ)cvff_aug.o \
#	$(OBJ)oplsaap.o \
#	$(OBJ)oplsaar.o \
#	$(OBJ)pcff.o


OBJ_CPMD = \
	$(OBJ)cpmd_print.o \
	$(OBJ)cpmd_nose.o \
	$(OBJ)cpmd_restart.o \
	$(OBJ)cpmd_atoms.o \
	$(OBJ)cpmd_init.o

OBJ_CP2K = \
	$(OBJ)cp2k_print.o \
	$(OBJ)cp2k_molopt-basis.o \
	$(OBJ)cp2k_gth-basis.o \
	$(OBJ)cp2k_basis.o \
	$(OBJ)cp2k_pot.o \
	$(OBJ)cp2k_files.o \
	$(OBJ)cp2k_mol.o \
	$(OBJ)cp2k_init.o

OBJ_WIN = \
	$(OBJ)color_box.o \
	$(OBJ)m_style.o \
	$(OBJ)m_map.o \
	$(OBJ)m_render.o \
	$(OBJ)m_quality.o \
	$(OBJ)m_atoms.o \
	$(OBJ)m_bonds.o \
	$(OBJ)m_clones.o \
	$(OBJ)m_box.o \
	$(OBJ)m_coord.o \
	$(OBJ)m_poly.o \
	$(OBJ)m_tools.o \
	$(OBJ)m_edit.o \
	$(OBJ)m_rep.o \
	$(OBJ)m_proj.o \
	$(OBJ)m_back.o \
	$(OBJ)m_axis.o \
	$(OBJ)m_anim.o \
	$(OBJ)menu_bar.o \
	$(OBJ)w_measures.o \
	$(OBJ)w_volumes.o \
	$(OBJ)w_colors.o \
	$(OBJ)w_atoms.o \
	$(OBJ)w_bonds.o \
	$(OBJ)w_labels.o \
	$(OBJ)w_search.o \
	$(OBJ)w_periodic.o \
	$(OBJ)w_library.o \
	$(OBJ)w_cutoffs.o \
	$(OBJ)w_rings.o \
	$(OBJ)w_chains.o \
	$(OBJ)w_coord.o \
	$(OBJ)w_box.o \
	$(OBJ)w_axis.o \
	$(OBJ)w_advance.o \
	$(OBJ)w_sequencer.o \
	$(OBJ)w_spiner.o \
	$(OBJ)w_encode.o \
	$(OBJ)w_record.o \
	$(OBJ)initcoord.o \
	$(OBJ)initmol.o \
	$(OBJ)initring.o \
	$(OBJ)initchain.o \
	$(OBJ)popup.o \
	$(OBJ)glwindow.o

OBJ_CEDIT = \
	$(OBJ)cell_shift.o \
	$(OBJ)cell_extra.o \
	$(OBJ)cell_super.o \
	$(OBJ)cell_density.o \
	$(OBJ)cell_cut.o \
	$(OBJ)cell_pixel.o \
	$(OBJ)cell_edit.o

OBJ_AEDIT = \
	$(OBJ)atom_action.o \
	$(OBJ)atom_coord.o \
	$(OBJ)atom_geo.o \
	$(OBJ)atom_insert.o \
	$(OBJ)atom_move.o \
	$(OBJ)atom_object.o \
	$(OBJ)atom_remove.o \
	$(OBJ)atom_search.o \
	$(OBJ)atom_species.o \
	$(OBJ)atom_edit.o

OBJ_CBUILD = \
	$(OBJ)cbuild_action.o \
	$(OBJ)cbuild_sg.o \
	$(OBJ)cbuild_info.o \
	$(OBJ)cbuild_edit.o

OBJ_DRAW = \
	$(OBJ)d_atoms.o \
	$(OBJ)d_bonds.o \
	$(OBJ)d_label.o \
	$(OBJ)d_selection.o \
	$(OBJ)d_poly.o \
	$(OBJ)d_rings.o \
	$(OBJ)d_box.o \
	$(OBJ)d_axis.o \
	$(OBJ)d_measures.o \
	$(OBJ)d_background.o \
	$(OBJ)ogl_text.o \
	$(OBJ)movie.o \
	$(OBJ)image.o

OBJ_GL = \
	$(OBJ)arcball.o \
	$(OBJ)selection.o \
	$(OBJ)ogl_utils.o \
	$(OBJ)ogl_shaders.o \
	$(OBJ)ogl_shading.o \
	$(OBJ)ogl_draw.o \
	$(OBJ)glview.o

OBJ_OGL = $(OBJ_WIN) $(OBJ_CBUILD) $(OBJ_CEDIT) $(OBJ_AEDIT) $(OBJ_DRAW) $(OBJ_GL)

ifeq ($(WINDOWS),1)

  OBJECTS = $(WIN_ATOMES) $(OBJECTS_F90) $(OBJECTS_c)

else

  OBJECTS = $(OBJECTS_F90) $(OBJECTS_c)

endif

SOURCES_h = \
	$(SRC)version.h \
	$(SRC)config.h \
	$(SRC)affero.h \
	$(SRC)bind.h \
	$(SRC)global.h \
	$(GUI)interface.h \
	$(GUI)callbacks.h \
	$(PROJ)project.h \
	$(PROJ)/readers/readers.h \
	$(DLPOLY)dlp_field.h \
	$(FIELDS)force_field.h \
	$(OGL)glview.h \
	$(GLWIN)glwindow.h \
	$(GLWIN)submenus.h \
	$(GLWIN)color_box.h \
	$(GLWIN)initcoord.h \
	$(GLDRAW)movie.h


OGL_TEST_PROG = atomes_startup_testing

ifeq ($(WINDOWS),1)

  OGL_TEST = $(OBJ)startup_testing.o $(WIN_STARTUP)

else

  OGL_TEST = $(OBJ)startup_testing.o

endif

# The rule to build the executable

atomes: version ogl exe

debug: version ogl exe

version:
	$(ECHO) "#define FC \""$(FC) $(FCVER)"\"" > src/version.h
	$(ECHO) "#define FCFLAGS \""$(FCFLAGS)"\""  >> src/version.h
	$(ECHO) "#define CC \""$(CC) $(CCVER)"\"" >> src/version.h
	$(ECHO) "#define CFLAGS \""$(CFLAGS)"\"" >> src/version.h

all:
	$(MAKE) debug
	$(CP) $(BIN)/atomes$(EXT) $(BIN)/atomes-debug$(EXT)
	$(MAKE) clean
	$(MAKE) atomes

ogl:$(OGL_TEST)
	$(LD) -o $(BIN)$(OGL_TEST_PROG) $(OGL_TEST) $(DEFS) $(LDFLAGS)

exe:$(OBJECTS)
	$(LD) -o $(BIN)$(PROGRAM) $(OBJECTS) $(DEFS) $(LDFLAGS)

# Special targets

ifeq ($(WINDOWS),1)
  cleangui: clean_gui clean_win
  cleanc: clean_c clean_win
  clean: clean_all clean_win
  clear: clear_all clean_win
else
  cleangui: clean_gui
  cleanc: clean_c
  clean: clean_all
  clear: clear_all
endif

clean_gui:
	$(RM) $(RMFLAGS) $(OBJ_GUI) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleanwork:
	$(RM) $(RMFLAGS) $(OBJ_WORK) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleanproj:
	$(RM) $(RMFLAGS) $(OBJ_PROJ) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleancalc:
	$(RM) $(RMFLAGS) $(OBJ_CALC) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleanpoly:
	$(RM) $(RMFLAGS) $(OBJ_POLY) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleanfield:
	$(RM) $(RMFLAGS) $(OBJ_FIELD) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleancpmd:
	$(RM) $(RMFLAGS) $(OBJ_CPMD) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleancp2k:
	$(RM) $(RMFLAGS) $(OBJ_CP2K) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleancurve:
	$(RM) $(RMFLAGS) $(OBJ_CURVE) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleanedit:
	$(RM) $(RMFLAGS) $(OBJ_CEDIT) $(OBJ_AEDIT) $(OBJ_CBUILD) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleanogl:
	$(RM) $(RMFLAGS) $(OBJ_OGL) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

clean_c:
	$(RM) $(RMFLAGS) $(OBJECTS_c) $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

cleanf:
	$(RM) $(RMFLAGS) $(OBJECTS_F90) $(BIN)$(PROGRAM)
	$(RM) $(RMFLAGS) *.mod
	$(RM) $(RMFLAGS) $(OBJ)*.mod

clean_all:
	$(RM) $(RMFLAGS) $(OBJECTS) *.mod $(OBJ)*.mod $(BIN)$(PROGRAM) $(OBJ)startup_testing.o $(BIN)$(OGL_TEST_PROG)

clean_win:
	$(RM) $(RMFLAGS) $(OBJ)win_atomes.o
	$(RM) $(RMFLAGS) $(OBJ)win_startup.o

clear_all:
	$(RM) $(RMFLAGS) $(OBJECTS) *.mod $(OBJ)*.mod

TouchSource:
	$(TOUCH) $(SOURCES)

# Fortran sources
$(OBJ)%.o: $(FOR)%.F90
	$(FC) $(FCFLAGS) $(DOMP) $(DEFS) $(INC) -c $< -o $@

# Dependency so that Fortran module files are compiled first
$(filter-out $(MODOBJECTS_F90), $(OBJECTS_F90)): $(MODOBJECTS_F90)

# OpenGL testing files:
$(OBJ)startup_testing.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)startup_testing.o $(SRC)startup-testing/startup_testing.c $(INCLUDES)

# C files:
$(OBJ)global.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)global.o $(SRC)global.c $(INCLUDES)

# GUI
$(OBJ)gtk-misc.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)gtk-misc.o $(GUI)gtk-misc.c $(INCLUDES)
$(OBJ)work_menu.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)work_menu.o $(GUI)work_menu.c $(INCLUDES)
$(OBJ)edit_menu.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)edit_menu.o $(GUI)edit_menu.c $(INCLUDES)
$(OBJ)calc_menu.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)calc_menu.o $(GUI)calc_menu.c $(INCLUDES)
$(OBJ)tools.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)tools.o $(GUI)tools.c $(INCLUDES)
$(OBJ)preferences.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)preferences.o $(GUI)preferences.c $(INCLUDES)
$(OBJ)gui.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)gui.o $(GUI)gui.c $(INCLUDES)
$(OBJ)initc.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)initc.o $(GUI)initc.c $(INCLUDES)

# Workspace
$(OBJ)modelinfo.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)modelinfo.o $(WORK)modelinfo.c $(INCLUDES)
$(OBJ)expinfo.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)expinfo.o $(WORK)expinfo.c $(INCLUDES)
$(OBJ)workinfo.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)workinfo.o $(WORK)workinfo.c $(INCLUDES)
$(OBJ)workspace.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)workspace.o $(WORK)workspace.c $(INCLUDES)

# GUI
$(OBJ)callbacks.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)callbacks.o $(GUI)callbacks.c $(INCLUDES)
$(OBJ)interface.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)interface.o $(GUI)interface.c $(INCLUDES)
$(OBJ)bdcall.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)bdcall.o $(GUI)bdcall.c $(INCLUDES)
$(OBJ)grcall.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)grcall.o $(GUI)grcall.c $(INCLUDES)
$(OBJ)sqcall.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)sqcall.o $(GUI)sqcall.c $(INCLUDES)
$(OBJ)ringscall.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)ringscall.o $(GUI)ringscall.c $(INCLUDES)
$(OBJ)chainscall.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)chainscall.o $(GUI)chainscall.c $(INCLUDES)
$(OBJ)msdcall.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)msdcall.o $(GUI)msdcall.c $(INCLUDES)
$(OBJ)spcall.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)spcall.o $(GUI)spcall.c $(INCLUDES)
$(OBJ)main.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)main.o $(GUI)main.c $(INCLUDES)

# Project
$(OBJ)read_isaacs.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)read_isaacs.o $(PROJ)readers/read_isaacs.c $(INCLUDES)
$(OBJ)read_cif.o:
	$(CC) -c $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)read_cif.o $(PROJ)readers/read_cif.c $(INCLUDES)
$(OBJ)read_coord.o:
	$(CC) -c $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)read_coord.o $(PROJ)readers/read_coord.c $(INCLUDES)
$(OBJ)read_xyz.o:
	$(CC) -c $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)read_xyz.o $(PROJ)readers/read_xyz.c $(INCLUDES)
$(OBJ)read_c3d.o:
	$(CC) -c $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)read_c3d.o $(PROJ)readers/read_c3d.c $(INCLUDES)
$(OBJ)read_trj.o:
	$(CC) -c $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)read_trj.o $(PROJ)readers/read_trj.c $(INCLUDES)
$(OBJ)read_vas.o:
	$(CC) -c $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)read_vas.o $(PROJ)readers/read_vas.c $(INCLUDES)
$(OBJ)read_pdb.o:
	$(CC) -c $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)read_pdb.o $(PROJ)readers/read_pdb.c $(INCLUDES)
$(OBJ)read_hist.o:
	$(CC) -c $(CFLAGS) $(DOMP) $(DEFS) -o $(OBJ)read_hist.o $(PROJ)readers/read_hist.c $(INCLUDES)
$(OBJ)read_npt.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)read_npt.o $(PROJ)readers/read_npt.c $(INCLUDES)
$(OBJ)update_p.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)update_p.o $(PROJ)update_p.c $(INCLUDES)
$(OBJ)init_p.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)init_p.o $(PROJ)init_p.c $(INCLUDES)
$(OBJ)debugio.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)debugio.o $(PROJ)debugio.c $(INCLUDES)
$(OBJ)read_field.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)read_field.o $(PROJ)read_field.c $(INCLUDES)
$(OBJ)read_qm.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)read_qm.o $(PROJ)read_qm.c $(INCLUDES)
$(OBJ)read_opengl.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)read_opengl.o $(PROJ)read_opengl.c $(INCLUDES)
$(OBJ)read_curve.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)read_curve.o $(PROJ)read_curve.c $(INCLUDES)
$(OBJ)read_mol.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)read_mol.o $(PROJ)read_mol.c $(INCLUDES)
$(OBJ)read_bond.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)read_bond.o $(PROJ)read_bond.c $(INCLUDES)
$(OBJ)open_p.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)open_p.o $(PROJ)open_p.c $(INCLUDES)
$(OBJ)close_p.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)close_p.o $(PROJ)close_p.c $(INCLUDES)
$(OBJ)save_field.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)save_field.o $(PROJ)save_field.c $(INCLUDES)
$(OBJ)save_qm.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)save_qm.o $(PROJ)save_qm.c $(INCLUDES)
$(OBJ)save_opengl.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)save_opengl.o $(PROJ)save_opengl.c $(INCLUDES)
$(OBJ)save_curve.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)save_curve.o $(PROJ)save_curve.c $(INCLUDES)
$(OBJ)save_mol.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)save_mol.o $(PROJ)save_mol.c $(INCLUDES)
$(OBJ)save_bond.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)save_bond.o $(PROJ)save_bond.c $(INCLUDES)
$(OBJ)save_p.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)save_p.o $(PROJ)save_p.c $(INCLUDES)
$(OBJ)project.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)project.o $(PROJ)project.c $(INCLUDES)

# calc
$(OBJ)calc.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)calc.o $(CALC)calc.c $(INCLUDES)

# Curves
$(OBJ)tab-1.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)tab-1.o $(CURVE)tab-1.c $(INCLUDES)
$(OBJ)tab-2.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)tab-2.o $(CURVE)tab-2.c $(INCLUDES)
$(OBJ)tab-3.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)tab-3.o $(CURVE)tab-3.c $(INCLUDES)
$(OBJ)tab-4.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)tab-4.o $(CURVE)tab-4.c $(INCLUDES)
$(OBJ)cedit.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)cedit.o $(CURVE)cedit.c $(INCLUDES)
$(OBJ)datab.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)datab.o $(CURVE)datab.c $(INCLUDES)
$(OBJ)cwidget.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)cwidget.o $(CURVE)cwidget.c $(INCLUDES)
$(OBJ)glyph.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)glyph.o $(CURVE)glyph.c $(INCLUDES)
$(OBJ)labels.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)labels.o $(CURVE)labels.c $(INCLUDES)
$(OBJ)title.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)title.o $(CURVE)title.c $(INCLUDES)
$(OBJ)legend.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)legend.o $(CURVE)legend.c $(INCLUDES)
$(OBJ)xaxis.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)xaxis.o $(CURVE)xaxis.c $(INCLUDES)
$(OBJ)yaxis.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)yaxis.o $(CURVE)yaxis.c $(INCLUDES)
$(OBJ)frame.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)frame.o $(CURVE)frame.c $(INCLUDES)
$(OBJ)draw.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)draw.o $(CURVE)draw.c $(INCLUDES)
$(OBJ)show.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)show.o $(CURVE)show.c $(INCLUDES)
$(OBJ)w_data.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_data.o $(CURVE)w_data.c $(INCLUDES)
$(OBJ)w_img.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)w_img.o $(CURVE)w_img.c $(INCLUDES)
$(OBJ)m_curve.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)m_curve.o $(CURVE)m_curve.c $(INCLUDES)
$(OBJ)w_curve.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)w_curve.o $(CURVE)w_curve.c $(INCLUDES)
$(OBJ)curve.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)curve.o $(CURVE)curve.c $(INCLUDES)

#DL Poly force field
$(OBJ)dlp_control.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)dlp_control.o $(DLPOLY)dlp_control.c $(INCLUDES)
$(OBJ)dlp_init.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)dlp_init.o $(DLPOLY)dlp_init.c $(INCLUDES)
$(OBJ)dlp_active.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_active.o $(DLPOLY)dlp_active.c $(INCLUDES)
$(OBJ)dlp_copy.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_copy.o $(DLPOLY)dlp_copy.c $(INCLUDES)
$(OBJ)dlp_comp.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_comp.o $(DLPOLY)dlp_comp.c $(INCLUDES)
$(OBJ)dlp_viz.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_viz.o $(DLPOLY)dlp_viz.c $(INCLUDES)
$(OBJ)dlp_mol.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_mol.o $(DLPOLY)dlp_mol.c $(INCLUDES)
$(OBJ)dlp_atom.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_atom.o $(DLPOLY)dlp_atom.c $(INCLUDES)
$(OBJ)dlp_ff_match.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_ff_match.o $(DLPOLY)dlp_ff_match.c $(INCLUDES)
$(OBJ)dlp_edit.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_edit.o $(DLPOLY)dlp_edit.c $(INCLUDES)
$(OBJ)dlp_print.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_print.o $(DLPOLY)dlp_print.c $(INCLUDES)
$(OBJ)dlp_field.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)dlp_field.o $(DLPOLY)dlp_field.c $(INCLUDES)

#LAMMPS
$(OBJ)la_print.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)la_print.o $(LAMMPS)la_print.c $(INCLUDES)

#Force fields
$(OBJ)force_fields.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)force_fields.o $(FIELDS)force_fields.c $(INCLUDES)
$(OBJ)amber94.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)amber94.o $(FIELDS)amber94.c $(INCLUDES)
$(OBJ)amber96.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)amber96.o $(FIELDS)amber96.c $(INCLUDES)
$(OBJ)amber98.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)amber98.o $(FIELDS)amber98.c $(INCLUDES)
$(OBJ)amber99.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)amber99.o $(FIELDS)amber99.c $(INCLUDES)
$(OBJ)cff91.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)cff91.o $(FIELDS)cff91.c $(INCLUDES)
$(OBJ)charmm22_prot.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm22_prot.o $(FIELDS)charmm22_prot.c $(INCLUDES)
$(OBJ)charmm22_prot_metals.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm22_prot_metals.o $(FIELDS)charmm22_prot_metals.c $(INCLUDES)
$(OBJ)charmm35_ethers.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm35_ethers.o $(FIELDS)charmm35_ethers.c $(INCLUDES)
$(OBJ)charmm36_carb.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm36_carb.o $(FIELDS)charmm36_carb.c $(INCLUDES)
$(OBJ)charmm36_cgenff.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm36_cgenff.o $(FIELDS)charmm36_cgenff.c $(INCLUDES)
$(OBJ)charmm36_lipid.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm36_lipid.o $(FIELDS)charmm36_lipid.c $(INCLUDES)
$(OBJ)charmm36_na.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm36_na.o $(FIELDS)charmm36_na.c $(INCLUDES)
$(OBJ)charmm36_prot.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm36_prot.o $(FIELDS)charmm36_prot.c $(INCLUDES)
$(OBJ)charmm36_prot_metals.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm36_prot_metals.o $(FIELDS)charmm36_prot_metals.c $(INCLUDES)
$(OBJ)charmm_silicates.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)charmm_silicates.o $(FIELDS)charmm_silicates.c $(INCLUDES)
$(OBJ)compass.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)compass.o $(FIELDS)compass.c $(INCLUDES)
$(OBJ)cvff.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)cvff.o $(FIELDS)cvff.c $(INCLUDES)
$(OBJ)cvff_aug.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)cvff_aug.o $(FIELDS)cvff_aug.c $(INCLUDES)
$(OBJ)oplsaap.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)oplsaap.o $(FIELDS)oplsaap.c $(INCLUDES)
$(OBJ)oplsaar.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)oplsaar.o $(FIELDS)oplsaar.c $(INCLUDES)
$(OBJ)pcff.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)pcff.o $(FIELDS)pcff.c $(INCLUDES)

#CPMD input
$(OBJ)cpmd_print.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cpmd_print.o $(CPMD)cpmd_print.c $(INCLUDES)
$(OBJ)cpmd_nose.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cpmd_nose.o $(CPMD)cpmd_nose.c $(INCLUDES)
$(OBJ)cpmd_restart.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cpmd_restart.o $(CPMD)cpmd_restart.c $(INCLUDES)
$(OBJ)cpmd_atoms.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cpmd_atoms.o $(CPMD)cpmd_atoms.c $(INCLUDES)
$(OBJ)cpmd_init.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cpmd_init.o $(CPMD)cpmd_init.c $(INCLUDES)

#CP2K input
$(OBJ)cp2k_print.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cp2k_print.o $(CP2K)cp2k_print.c $(INCLUDES)
$(OBJ)cp2k_molopt-basis.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cp2k_molopt-basis.o $(CP2K)cp2k_molopt-basis.c $(INCLUDES)
$(OBJ)cp2k_gth-basis.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cp2k_gth-basis.o $(CP2K)cp2k_gth-basis.c $(INCLUDES)
$(OBJ)cp2k_basis.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cp2k_basis.o $(CP2K)cp2k_basis.c $(INCLUDES)
$(OBJ)cp2k_pot.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cp2k_pot.o $(CP2K)cp2k_pot.c $(INCLUDES)
$(OBJ)cp2k_files.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cp2k_files.o $(CP2K)cp2k_files.c $(INCLUDES)
$(OBJ)cp2k_mol.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cp2k_mol.o $(CP2K)cp2k_mol.c $(INCLUDES)
$(OBJ)cp2k_init.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cp2k_init.o $(CP2K)cp2k_init.c $(INCLUDES)

# OpenGL :: Window and menus
$(OBJ)color_box.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)color_box.o $(GLWIN)color_box.c $(INCLUDES)
$(OBJ)m_style.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)m_style.o $(GLWIN)m_style.c $(INCLUDES)
$(OBJ)m_map.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_map.o $(GLWIN)m_map.c $(INCLUDES)
$(OBJ)m_render.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_render.o $(GLWIN)m_render.c $(INCLUDES)
$(OBJ)m_quality.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_quality.o $(GLWIN)m_quality.c $(INCLUDES)
$(OBJ)m_atoms.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_atoms.o $(GLWIN)m_atoms.c $(INCLUDES)
$(OBJ)m_bonds.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_bonds.o $(GLWIN)m_bonds.c $(INCLUDES)
$(OBJ)m_clones.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_clones.o $(GLWIN)m_clones.c $(INCLUDES)
$(OBJ)m_box.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_box.o $(GLWIN)m_box.c $(INCLUDES)
$(OBJ)m_coord.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_coord.o $(GLWIN)m_coord.c $(INCLUDES)
$(OBJ)m_poly.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_poly.o $(GLWIN)m_poly.c $(INCLUDES)
$(OBJ)m_tools.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_tools.o $(GLWIN)m_tools.c $(INCLUDES)
$(OBJ)m_edit.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_edit.o $(GLWIN)m_edit.c $(INCLUDES)
$(OBJ)m_rep.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_rep.o $(GLWIN)m_rep.c $(INCLUDES)
$(OBJ)m_proj.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_proj.o $(GLWIN)m_proj.c $(INCLUDES)
$(OBJ)m_back.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_back.o $(GLWIN)m_back.c $(INCLUDES)
$(OBJ)m_axis.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_axis.o $(GLWIN)m_axis.c $(INCLUDES)
$(OBJ)m_anim.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)m_anim.o $(GLWIN)m_anim.c $(INCLUDES)
$(OBJ)menu_bar.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)menu_bar.o $(GLWIN)menu_bar.c $(INCLUDES)
$(OBJ)w_colors.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_colors.o $(GLWIN)w_colors.c $(INCLUDES)
$(OBJ)w_atoms.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_atoms.o $(GLWIN)w_atoms.c $(INCLUDES)
$(OBJ)w_labels.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_labels.o $(GLWIN)w_labels.c $(INCLUDES)
$(OBJ)w_search.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_search.o $(GLWIN)w_search.c $(INCLUDES)
$(OBJ)w_periodic.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_periodic.o $(GLWIN)w_periodic.c $(INCLUDES)
$(OBJ)w_library.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)w_library.o $(GLWIN)w_library.c $(INCLUDES)
$(OBJ)w_cutoffs.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_cutoffs.o $(GLWIN)w_cutoffs.c $(INCLUDES)
$(OBJ)w_bonds.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_bonds.o $(GLWIN)w_bonds.c $(INCLUDES)
$(OBJ)w_rings.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_rings.o $(GLWIN)w_rings.c $(INCLUDES)
$(OBJ)w_chains.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_chains.o $(GLWIN)w_chains.c $(INCLUDES)
$(OBJ)w_coord.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_coord.o $(GLWIN)w_coord.c $(INCLUDES)
$(OBJ)w_box.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_box.o $(GLWIN)w_box.c $(INCLUDES)
$(OBJ)w_axis.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_axis.o $(GLWIN)w_axis.c $(INCLUDES)
$(OBJ)w_measures.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_measures.o $(GLWIN)w_measures.c $(INCLUDES)
$(OBJ)w_volumes.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_volumes.o $(GLWIN)w_volumes.c $(INCLUDES)
$(OBJ)w_advance.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)w_advance.o $(GLWIN)w_advance.c $(INCLUDES)
$(OBJ)w_sequencer.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)w_sequencer.o $(GLWIN)w_sequencer.c $(INCLUDES)
$(OBJ)w_spiner.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)w_spiner.o $(GLWIN)w_spiner.c $(INCLUDES)
$(OBJ)w_encode.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)w_encode.o $(GLWIN)w_encode.c $(INCLUDES)
$(OBJ)w_record.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)w_record.o $(GLWIN)w_record.c $(INCLUDES)
$(OBJ)initcoord.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)initcoord.o $(GLWIN)initcoord.c $(INCLUDES)
$(OBJ)initmol.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)initmol.o $(GLWIN)initmol.c $(INCLUDES)
$(OBJ)initring.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)initring.o $(GLWIN)initring.c $(INCLUDES)
$(OBJ)initchain.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)initchain.o $(GLWIN)initchain.c $(INCLUDES)
$(OBJ)popup.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)popup.o $(GLWIN)popup.c $(INCLUDES)
$(OBJ)glwindow.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)glwindow.o $(GLWIN)glwindow.c $(INCLUDES)

# OpenGL :: Cell edit
$(OBJ)cell_shift.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cell_shift.o $(GLEDIT)cell_shift.c $(INCLUDES)
$(OBJ)cell_extra.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cell_extra.o $(GLEDIT)cell_extra.c $(INCLUDES)
$(OBJ)cell_super.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cell_super.o $(GLEDIT)cell_super.c $(INCLUDES)
$(OBJ)cell_density.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cell_density.o $(GLEDIT)cell_density.c $(INCLUDES)
$(OBJ)cell_cut.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cell_cut.o $(GLEDIT)cell_cut.c $(INCLUDES)
$(OBJ)cell_pixel.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cell_pixel.o $(GLEDIT)cell_pixel.c $(INCLUDES)
$(OBJ)cell_edit.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cell_edit.o $(GLEDIT)cell_edit.c $(INCLUDES)

# OpenGL :: Atom edit
$(OBJ)atom_action.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_action.o $(GLEDIT)atom_action.c $(INCLUDES)
$(OBJ)atom_coord.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_coord.o $(GLEDIT)atom_coord.c $(INCLUDES)
$(OBJ)atom_geo.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_geo.o $(GLEDIT)atom_geo.c $(INCLUDES)
$(OBJ)atom_insert.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_insert.o $(GLEDIT)atom_insert.c $(INCLUDES)
$(OBJ)atom_move.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_move.o $(GLEDIT)atom_move.c $(INCLUDES)
$(OBJ)atom_object.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_object.o $(GLEDIT)atom_object.c $(INCLUDES)
$(OBJ)atom_remove.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_remove.o $(GLEDIT)atom_remove.c $(INCLUDES)
$(OBJ)atom_search.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_search.o $(GLEDIT)atom_search.c $(INCLUDES)
$(OBJ)atom_species.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_species.o $(GLEDIT)atom_species.c $(INCLUDES)
$(OBJ)atom_edit.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)atom_edit.o $(GLEDIT)atom_edit.c $(INCLUDES)

# OpenGL :: Crystal builder
$(OBJ)cbuild_action.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cbuild_action.o $(GLEDIT)cbuild_action.c $(INCLUDES)
$(OBJ)cbuild_sg.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cbuild_sg.o $(GLEDIT)cbuild_sg.c $(INCLUDES)
$(OBJ)cbuild_info.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cbuild_info.o $(GLEDIT)cbuild_info.c $(INCLUDES)
$(OBJ)cbuild_edit.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $(DEFS) -o $(OBJ)cbuild_edit.o $(GLEDIT)cbuild_edit.c $(INCLUDES)

# OpenGL :: Draw
$(OBJ)d_bonds.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_bonds.o $(GLDRAW)d_bonds.c $(INCLUDES)
$(OBJ)d_atoms.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_atoms.o $(GLDRAW)d_atoms.c $(INCLUDES)
$(OBJ)d_label.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_label.o $(GLDRAW)d_label.c $(INCLUDES)
$(OBJ)d_selection.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_selection.o $(GLDRAW)d_selection.c $(INCLUDES)
$(OBJ)d_poly.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_poly.o $(GLDRAW)d_poly.c $(INCLUDES)
$(OBJ)d_rings.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_rings.o $(GLDRAW)d_rings.c $(INCLUDES)
$(OBJ)d_box.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_box.o $(GLDRAW)d_box.c $(INCLUDES)
$(OBJ)d_axis.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_axis.o $(GLDRAW)d_axis.c $(INCLUDES)
$(OBJ)d_measures.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_measures.o $(GLDRAW)d_measures.c $(INCLUDES)
$(OBJ)d_background.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)d_background.o $(GLDRAW)d_background.c $(INCLUDES)
$(OBJ)ogl_text.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)ogl_text.o $(GLDRAW)ogl_text.c $(INCLUDES)
$(OBJ)movie.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)movie.o $(GLDRAW)movie.c $(INCLUDES)
$(OBJ)image.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)image.o $(GLDRAW)image.c $(INCLUDES)

# OpengGL :: GL
$(OBJ)glview.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)glview.o $(OGL)glview.c $(INCLUDES)
$(OBJ)arcball.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)arcball.o $(OGL)arcball.c $(INCLUDES)
$(OBJ)ogl_utils.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)ogl_utils.o $(OGL)ogl_utils.c $(INCLUDES)
$(OBJ)ogl_shaders.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)ogl_shaders.o $(OGL)ogl_shaders.c $(INCLUDES)
$(OBJ)ogl_shading.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)ogl_shading.o $(OGL)ogl_shading.c $(INCLUDES)
$(OBJ)selection.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)selection.o $(OGL)selection.c $(INCLUDES)
$(OBJ)ogl_draw.o:
	$(CC) -c $(CFLAGS) $(DEFS) -o $(OBJ)ogl_draw.o $(OGL)ogl_draw.c $(INCLUDES)


# Win file:

$(OBJ)win_atomes.o:
	$(WINDRES) $(SRC)atomes.rc -o $(OBJ)win_atomes.o

$(OBJ)win_startup.o:
	$(WINDRES) $(SRC)startup.rc -o $(OBJ)win_startup.o
