# Microsoft Developer Studio Generated NMAKE File, Based on cbm5x0.dsp
!IF "$(CFG)" == ""
CFG=cbm5x0 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cbm5x0 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cbm5x0 - Win32 Release" && "$(CFG)" != "cbm5x0 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cbm5x0.mak" CFG="cbm5x0 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cbm5x0 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cbm5x0 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cbm5x0 - Win32 Release"

OUTDIR=.\libs\cbm5x0\Release
INTDIR=.\libs\cbm5x0\Release
# Begin Custom Macros
OutDir=.\libs\cbm5x0\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cbm5x0.lib"

!ELSE 

ALL : "base - Win32 Release" "cbm2common - Win32 Release" "$(OUTDIR)\cbm5x0.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" "cbm2common - Win32 ReleaseCLEAN"
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cbm5x0-resources.obj"
	-@erase "$(INTDIR)\cbm5x0-snapshot.obj"
	-@erase "$(INTDIR)\cbm5x0.obj"
	-@erase "$(INTDIR)\cbm5x0mem.obj"
	-@erase "$(INTDIR)\cbm5x0rom.obj"
	-@erase "$(INTDIR)\cbm5x0video.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cbm5x0.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\crtc" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\cbm5x0.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cbm5x0.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\cbm5x0.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cbm5x0-resources.obj" \
	"$(INTDIR)\cbm5x0-snapshot.obj" \
	"$(INTDIR)\cbm5x0.obj" \
	"$(INTDIR)\cbm5x0mem.obj" \
	"$(INTDIR)\cbm5x0rom.obj" \
	"$(INTDIR)\cbm5x0video.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\cbm5x0.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cbm5x0 - Win32 Debug"

OUTDIR=.\libs\cbm5x0\Debug
INTDIR=.\libs\cbm5x0\Debug
# Begin Custom Macros
OutDir=.\libs\cbm5x0\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cbm5x0.lib"

!ELSE 

ALL : "base - Win32 Debug" "cbm2common - Win32 Debug" "$(OUTDIR)\cbm5x0.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" "cbm2common - Win32 DebugCLEAN"
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cbm5x0-resources.obj"
	-@erase "$(INTDIR)\cbm5x0-snapshot.obj"
	-@erase "$(INTDIR)\cbm5x0.obj"
	-@erase "$(INTDIR)\cbm5x0mem.obj"
	-@erase "$(INTDIR)\cbm5x0rom.obj"
	-@erase "$(INTDIR)\cbm5x0video.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cbm5x0.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\crtc" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\cbm5x0.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cbm5x0.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\cbm5x0.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cbm5x0-resources.obj" \
	"$(INTDIR)\cbm5x0-snapshot.obj" \
	"$(INTDIR)\cbm5x0.obj" \
	"$(INTDIR)\cbm5x0mem.obj" \
	"$(INTDIR)\cbm5x0rom.obj" \
	"$(INTDIR)\cbm5x0video.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\cbm5x0.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "cbm5x0 - Win32 Release" || "$(CFG)" == "cbm5x0 - Win32 Debug"

!IF  "$(CFG)" == "cbm5x0 - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"cbm2common - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\cbm2common.mak" CFG="cbm2common - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

"cbm2common - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\cbm2common.mak" CFG="cbm2common - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "cbm5x0 - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"cbm2common - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\cbm2common.mak" CFG="cbm2common - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

"cbm2common - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\cbm2common.mak" CFG="cbm2common - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\cbm2\cbm5x0-resources.c"

"$(INTDIR)\cbm5x0-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\cbm2\cbm5x0-snapshot.c"

"$(INTDIR)\cbm5x0-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm5x0.c

"$(INTDIR)\cbm5x0.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm5x0mem.c

"$(INTDIR)\cbm5x0mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm5x0rom.c

"$(INTDIR)\cbm5x0rom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm5x0video.c

"$(INTDIR)\cbm5x0video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

