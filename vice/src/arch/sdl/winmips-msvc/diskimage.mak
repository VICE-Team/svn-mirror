# Microsoft Developer Studio Generated NMAKE File, Based on diskimage.dsp
!IF "$(CFG)" == ""
CFG=diskimage - Win32 Debug
!MESSAGE No configuration specified. Defaulting to diskimage - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "diskimage - Win32 Release" && "$(CFG)" != "diskimage - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "diskimage.mak" CFG="diskimage - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "diskimage - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "diskimage - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "diskimage - Win32 Release"

OUTDIR=.\libs\diskimage\Release
INTDIR=.\libs\diskimage\Release
# Begin Custom Macros
OutDir=.\libs\diskimage\Release
# End Custom Macros

ALL : "$(OUTDIR)\diskimage.lib"


CLEAN :
	-@erase "$(INTDIR)\diskimage.obj"
	-@erase "$(INTDIR)\fsimage-check.obj"
	-@erase "$(INTDIR)\fsimage-create.obj"
	-@erase "$(INTDIR)\fsimage-gcr.obj"
	-@erase "$(INTDIR)\fsimage-p64.obj"
	-@erase "$(INTDIR)\fsimage-probe.obj"
	-@erase "$(INTDIR)\fsimage.obj"
	-@erase "$(INTDIR)\rawimage.obj"
	-@erase "$(INTDIR)\realimage.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\diskimage.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\diskimage" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\diskimage.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\diskimage.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\diskimage.lib" 
LIB32_OBJS= \
	"$(INTDIR)\diskimage.obj" \
	"$(INTDIR)\fsimage-check.obj" \
	"$(INTDIR)\fsimage-create.obj" \
	"$(INTDIR)\fsimage-gcr.obj" \
	"$(INTDIR)\fsimage-p64.obj" \
	"$(INTDIR)\fsimage-probe.obj" \
	"$(INTDIR)\fsimage.obj" \
	"$(INTDIR)\rawimage.obj" \
	"$(INTDIR)\realimage.obj"

"$(OUTDIR)\diskimage.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "diskimage - Win32 Debug"

OUTDIR=.\libs\diskimage\Debug
INTDIR=.\libs\diskimage\Debug
# Begin Custom Macros
OutDir=.\libs\diskimage\Debug
# End Custom Macros

ALL : "$(OUTDIR)\diskimage.lib"


CLEAN :
	-@erase "$(INTDIR)\diskimage.obj"
	-@erase "$(INTDIR)\fsimage-check.obj"
	-@erase "$(INTDIR)\fsimage-create.obj"
	-@erase "$(INTDIR)\fsimage-gcr.obj"
	-@erase "$(INTDIR)\fsimage-p64.obj"
	-@erase "$(INTDIR)\fsimage-probe.obj"
	-@erase "$(INTDIR)\fsimage.obj"
	-@erase "$(INTDIR)\rawimage.obj"
	-@erase "$(INTDIR)\realimage.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\diskimage.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\diskimage" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\diskimage.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\diskimage.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\diskimage.lib" 
LIB32_OBJS= \
	"$(INTDIR)\diskimage.obj" \
	"$(INTDIR)\fsimage-check.obj" \
	"$(INTDIR)\fsimage-create.obj" \
	"$(INTDIR)\fsimage-gcr.obj" \
	"$(INTDIR)\fsimage-p64.obj" \
	"$(INTDIR)\fsimage-probe.obj" \
	"$(INTDIR)\fsimage.obj" \
	"$(INTDIR)\rawimage.obj" \
	"$(INTDIR)\realimage.obj"

"$(OUTDIR)\diskimage.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "diskimage - Win32 Release" || "$(CFG)" == "diskimage - Win32 Debug"
SOURCE="..\..\..\diskimage\diskimage.c"

"$(INTDIR)\diskimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\diskimage\fsimage-check.c"

"$(INTDIR)\fsimage-check.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\diskimage\fsimage-create.c"

"$(INTDIR)\fsimage-create.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\diskimage\fsimage-gcr.c"

"$(INTDIR)\fsimage-gcr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\diskimage\fsimage-p64.c"

"$(INTDIR)\fsimage-p64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\diskimage\fsimage-probe.c"

"$(INTDIR)\fsimage-probe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\diskimage\fsimage.c"

"$(INTDIR)\fsimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\diskimage\rawimage.c"

"$(INTDIR)\rawimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\diskimage\realimage.c"

"$(INTDIR)\realimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

