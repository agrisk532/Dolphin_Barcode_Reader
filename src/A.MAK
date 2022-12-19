# Microsoft Visual C++ generated build script - Do not modify

PROJ = A
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\DOLPHIN\PROJECTS\AV_1\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = SCAN.C      
FIRSTCPP = MAIN.CPP    
RC = rc
CFLAGS_D_DEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /D "_DOS" /Fd"A.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G3 /W3 /AM /D "NDEBUG" /D "_DOS" 
LFLAGS_D_DEXE = /NOLOGO /NOI /STACK:10000 /ONERROR:NOEXE /CO 
LFLAGS_R_DEXE = /NOLOGO /NOI /STACK:10240 /ONERROR:NOEXE 
LIBS_D_DEXE = oldnames mlibce dolphinm 
LIBS_R_DEXE = oldnames mlibce dolphinm 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = MAIN.SBR \
		SCAN.SBR \
		SCREEN.SBR \
		UTIL.SBR


MAIN_DEP = c:\dolphin\devtools\include\dolphin.h \
	c:\dolphin\projects\av_1\screen.h \
	c:\dolphin\projects\av_1\util.h


SCAN_DEP = c:\dolphin\devtools\include\dolphin.h


SCREEN_DEP = c:\dolphin\projects\av_1\screen.h \
	c:\dolphin\devtools\include\dolphin.h


UTIL_DEP = c:\dolphin\devtools\include\dolphin.h \
	c:\dolphin\projects\av_1\util.h


all:	$(PROJ).EXE

MAIN.OBJ:	MAIN.CPP $(MAIN_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c MAIN.CPP

SCAN.OBJ:	SCAN.C $(SCAN_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c SCAN.C

SCREEN.OBJ:	SCREEN.CPP $(SCREEN_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SCREEN.CPP

UTIL.OBJ:	UTIL.CPP $(UTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTIL.CPP

$(PROJ).EXE::	MAIN.OBJ SCAN.OBJ SCREEN.OBJ UTIL.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
MAIN.OBJ +
SCAN.OBJ +
SCREEN.OBJ +
UTIL.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
C:\Dolphin\DevTools\Lib\Msvc\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
