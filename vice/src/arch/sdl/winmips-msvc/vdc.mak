# Microsoft Developer Studio Generated NMAKE File, Based on vdc.dsp
!IF "$(CFG)" == ""
CFG=vdc - Win32 Release
!MESSAGE No configuration specified. Defaulting to vdc - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "vdc - Win32 Release" && "$(CFG)" != "vdc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vdc.mak" CFG="vdc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vdc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vdc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "vdc - Win32 Release"

OUTDIR=.\libs\vdc\Release
INTDIR=.\libs\vdc\Release
# Begin Custom Macros
OutDir=.\libs\vdc\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vdc.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\vdc.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vdc-cmdline-options.obj"
	-@erase "$(INTDIR)\vdc-color.obj"
	-@erase "$(INTDIR)\vdc-draw.obj"
	-@erase "$(INTDIR)\vdc-mem.obj"
	-@erase "$(INTDIR)\vdc-resources.obj"
	-@erase "$(INTDIR)\vdc-snapshot.obj"
	-@erase "$(INTDIR)\vdc.obj"
	-@erase "$(OUTDIR)\vdc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\raster" /I "..\..\..\vicii" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vdc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vdc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vdc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\vdc-cmdline-options.obj" \
	"$(INTDIR)\vdc-color.obj" \
	"$(INTDIR)\vdc-draw.obj" \
	"$(INTDIR)\vdc-mem.obj" \
	"$(INTDIR)\vdc-resources.obj" \
	"$(INTDIR)\vdc-snapshot.obj" \
	"$(INTDIR)\vdc.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\vdc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vdc - Win32 Debug"

OUTDIR=.\libs\vdc\Debug
INTDIR=.\libs\vdc\Debug
# Begin Custom Macros
OutDir=.\libs\vdc\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vdc.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\vdc.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vdc-cmdline-options.obj"
	-@erase "$(INTDIR)\vdc-color.obj"
	-@erase "$(INTDIR)\vdc-draw.obj"
	-@erase "$(INTDIR)\vdc-mem.obj"
	-@erase "$(INTDIR)\vdc-resources.obj"
	-@erase "$(INTDIR)\vdc-snapshot.obj"
	-@erase "$(INTDIR)\vdc.obj"
	-@erase "$(OUTDIR)\vdc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\raster" /I "..\..\..\vicii" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vdc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vdc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vdc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\vdc-cmdline-options.obj" \
	"$(INTDIR)\vdc-color.obj" \
	"$(INTDIR)\vdc-draw.obj" \
	"$(INTDIR)\vdc-mem.obj" \
	"$(INTDIR)\vdc-resources.obj" \
	"$(INTDIR)\vdc-snapshot.obj" \
	"$(INTDIR)\vdc.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\vdc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "vdc - Win32 Release" || "$(CFG)" == "vdc - Win32 Debug"

!IF  "$(CFG)" == "vdc - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vdc - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\vdc\vdc-cmdline-options.c"

"$(INTDIR)\vdc-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vdc\vdc-color.c"

"$(INTDIR)\vdc-color.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vdc\vdc-draw.c"

"$(INTDIR)\vdc-draw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vdc\vdc-mem.c"

"$(INTDIR)\vdc-mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vdc\vdc-resources.c"

"$(INTDIR)\vdc-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vdc\vdc-snapshot.c"

"$(INTDIR)\vdc-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vdc\vdc.c

"$(INTDIR)\vdc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

