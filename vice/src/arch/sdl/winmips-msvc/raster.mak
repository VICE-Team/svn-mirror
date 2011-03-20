# Microsoft Developer Studio Generated NMAKE File, Based on raster.dsp
!IF "$(CFG)" == ""
CFG=raster - Win32 Debug
!MESSAGE No configuration specified. Defaulting to raster - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "raster - Win32 Release" && "$(CFG)" != "raster - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "raster.mak" CFG="raster - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "raster - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "raster - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "raster - Win32 Release"

OUTDIR=.\libs\raster\Release
INTDIR=.\libs\raster\Release
# Begin Custom Macros
OutDir=.\libs\raster\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\raster.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\raster.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\raster-cache.obj"
	-@erase "$(INTDIR)\raster-canvas.obj"
	-@erase "$(INTDIR)\raster-changes.obj"
	-@erase "$(INTDIR)\raster-cmdline-options.obj"
	-@erase "$(INTDIR)\raster-line-changes-sprite.obj"
	-@erase "$(INTDIR)\raster-line-changes.obj"
	-@erase "$(INTDIR)\raster-line.obj"
	-@erase "$(INTDIR)\raster-modes.obj"
	-@erase "$(INTDIR)\raster-resources.obj"
	-@erase "$(INTDIR)\raster-sprite-cache.obj"
	-@erase "$(INTDIR)\raster-sprite-status.obj"
	-@erase "$(INTDIR)\raster-sprite.obj"
	-@erase "$(INTDIR)\raster.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\raster.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\raster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\raster.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\raster.lib" 
LIB32_OBJS= \
	"$(INTDIR)\raster-cache.obj" \
	"$(INTDIR)\raster-canvas.obj" \
	"$(INTDIR)\raster-changes.obj" \
	"$(INTDIR)\raster-cmdline-options.obj" \
	"$(INTDIR)\raster-line-changes-sprite.obj" \
	"$(INTDIR)\raster-line-changes.obj" \
	"$(INTDIR)\raster-line.obj" \
	"$(INTDIR)\raster-modes.obj" \
	"$(INTDIR)\raster-resources.obj" \
	"$(INTDIR)\raster-sprite-cache.obj" \
	"$(INTDIR)\raster-sprite-status.obj" \
	"$(INTDIR)\raster-sprite.obj" \
	"$(INTDIR)\raster.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\raster.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "raster - Win32 Debug"

OUTDIR=.\libs\raster\Debug
INTDIR=.\libs\raster\Debug
# Begin Custom Macros
OutDir=.\libs\raster\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\raster.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\raster.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\raster-cache.obj"
	-@erase "$(INTDIR)\raster-canvas.obj"
	-@erase "$(INTDIR)\raster-changes.obj"
	-@erase "$(INTDIR)\raster-cmdline-options.obj"
	-@erase "$(INTDIR)\raster-line-changes-sprite.obj"
	-@erase "$(INTDIR)\raster-line-changes.obj"
	-@erase "$(INTDIR)\raster-line.obj"
	-@erase "$(INTDIR)\raster-modes.obj"
	-@erase "$(INTDIR)\raster-resources.obj"
	-@erase "$(INTDIR)\raster-sprite-cache.obj"
	-@erase "$(INTDIR)\raster-sprite-status.obj"
	-@erase "$(INTDIR)\raster-sprite.obj"
	-@erase "$(INTDIR)\raster.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\raster.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\raster.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\raster.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\raster.lib" 
LIB32_OBJS= \
	"$(INTDIR)\raster-cache.obj" \
	"$(INTDIR)\raster-canvas.obj" \
	"$(INTDIR)\raster-changes.obj" \
	"$(INTDIR)\raster-cmdline-options.obj" \
	"$(INTDIR)\raster-line-changes-sprite.obj" \
	"$(INTDIR)\raster-line-changes.obj" \
	"$(INTDIR)\raster-line.obj" \
	"$(INTDIR)\raster-modes.obj" \
	"$(INTDIR)\raster-resources.obj" \
	"$(INTDIR)\raster-sprite-cache.obj" \
	"$(INTDIR)\raster-sprite-status.obj" \
	"$(INTDIR)\raster-sprite.obj" \
	"$(INTDIR)\raster.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\raster.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "raster - Win32 Release" || "$(CFG)" == "raster - Win32 Debug"

!IF  "$(CFG)" == "raster - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "raster - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\raster\raster-cache.c"

"$(INTDIR)\raster-cache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-canvas.c"

"$(INTDIR)\raster-canvas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-changes.c"

"$(INTDIR)\raster-changes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-cmdline-options.c"

"$(INTDIR)\raster-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-line-changes-sprite.c"

"$(INTDIR)\raster-line-changes-sprite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-line-changes.c"

"$(INTDIR)\raster-line-changes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-line.c"

"$(INTDIR)\raster-line.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-modes.c"

"$(INTDIR)\raster-modes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-resources.c"

"$(INTDIR)\raster-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-sprite-cache.c"

"$(INTDIR)\raster-sprite-cache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-sprite-status.c"

"$(INTDIR)\raster-sprite-status.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\raster\raster-sprite.c"

"$(INTDIR)\raster-sprite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\raster\raster.c

"$(INTDIR)\raster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

