# Microsoft Developer Studio Generated NMAKE File, Based on tape.dsp
!IF "$(CFG)" == ""
CFG=tape - Win32 Release
!MESSAGE No configuration specified. Defaulting to tape - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "tape - Win32 Release" && "$(CFG)" != "tape - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tape.mak" CFG="tape - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tape - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tape - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "tape - Win32 Release"

OUTDIR=.\libs\tape\Release
INTDIR=.\libs\tape\Release
# Begin Custom Macros
OutDir=.\libs\tape\Release
# End Custom Macros

ALL : "$(OUTDIR)\tape.lib"


CLEAN :
	-@erase "$(INTDIR)\t64.obj"
	-@erase "$(INTDIR)\tap.obj"
	-@erase "$(INTDIR)\tape-internal.obj"
	-@erase "$(INTDIR)\tape-snapshot.obj"
	-@erase "$(INTDIR)\tape.obj"
	-@erase "$(INTDIR)\tapeimage.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\tape.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\tape" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\tape.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

.c{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tape.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tape.lib" 
LIB32_OBJS= \
	"$(INTDIR)\t64.obj" \
	"$(INTDIR)\tap.obj" \
	"$(INTDIR)\tape-internal.obj" \
	"$(INTDIR)\tape-snapshot.obj" \
	"$(INTDIR)\tape.obj" \
	"$(INTDIR)\tapeimage.obj"

"$(OUTDIR)\tape.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tape - Win32 Debug"

OUTDIR=.\libs\tape\Debug
INTDIR=.\libs\tape\Debug
# Begin Custom Macros
OutDir=.\libs\tape\Debug
# End Custom Macros

ALL : "$(OUTDIR)\tape.lib"


CLEAN :
	-@erase "$(INTDIR)\t64.obj"
	-@erase "$(INTDIR)\tap.obj"
	-@erase "$(INTDIR)\tape-internal.obj"
	-@erase "$(INTDIR)\tape-snapshot.obj"
	-@erase "$(INTDIR)\tape.obj"
	-@erase "$(INTDIR)\tapeimage.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\tape.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\tape" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\tape.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

.c{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tape.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tape.lib" 
LIB32_OBJS= \
	"$(INTDIR)\t64.obj" \
	"$(INTDIR)\tap.obj" \
	"$(INTDIR)\tape-internal.obj" \
	"$(INTDIR)\tape-snapshot.obj" \
	"$(INTDIR)\tape.obj" \
	"$(INTDIR)\tapeimage.obj"

"$(OUTDIR)\tape.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "tape - Win32 Release" || "$(CFG)" == "tape - Win32 Debug"
SOURCE="..\..\..\tape\t64.c"

"$(INTDIR)\t64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\tape\tap.c"

"$(INTDIR)\tap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\tape\tape-internal.c"

"$(INTDIR)\tape-internal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\tape\tape-snapshot.c"

"$(INTDIR)\tape-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\tape\tape.c"

"$(INTDIR)\tape.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\tape\tapeimage.c

"$(INTDIR)\tapeimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

