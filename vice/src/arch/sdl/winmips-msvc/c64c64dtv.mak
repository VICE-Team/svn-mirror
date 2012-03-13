# Microsoft Developer Studio Generated NMAKE File, Based on c64c64dtv.dsp
!IF "$(CFG)" == ""
CFG=c64c64dtv - Win32 Debug
!MESSAGE No configuration specified. Defaulting to c64c64dtv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "c64c64dtv - Win32 Release" && "$(CFG)" != "c64c64dtv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "c64c64dtv.mak" CFG="c64c64dtv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "c64c64dtv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "c64c64dtv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "c64c64dtv - Win32 Release"

OUTDIR=.\libs\c64c64dtv\Release
INTDIR=.\libs\c64c64dtv\Release
# Begin Custom Macros
OutDir=.\libs\c64c64dtv\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c64c64dtv.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\c64c64dtv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c64bus.obj"
	-@erase "$(INTDIR)\c64drive.obj"
	-@erase "$(INTDIR)\c64fastiec.obj"
	-@erase "$(INTDIR)\c64keyboard.obj"
	-@erase "$(INTDIR)\c64parallel.obj"
	-@erase "$(INTDIR)\c64printer.obj"
	-@erase "$(INTDIR)\c64rom.obj"
	-@erase "$(INTDIR)\c64romset.obj"
	-@erase "$(INTDIR)\c64rsuser.obj"
	-@erase "$(INTDIR)\c64video.obj"
	-@erase "$(INTDIR)\patchrom.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\c64c64dtv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\drive\iec\c64exp" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\rs232drv" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c64c64dtv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c64c64dtv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c64c64dtv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c64bus.obj" \
	"$(INTDIR)\c64drive.obj" \
	"$(INTDIR)\c64fastiec.obj" \
	"$(INTDIR)\c64keyboard.obj" \
	"$(INTDIR)\c64parallel.obj" \
	"$(INTDIR)\c64printer.obj" \
	"$(INTDIR)\c64rom.obj" \
	"$(INTDIR)\c64romset.obj" \
	"$(INTDIR)\c64rsuser.obj" \
	"$(INTDIR)\c64video.obj" \
	"$(INTDIR)\patchrom.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\c64c64dtv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "c64c64dtv - Win32 Debug"

OUTDIR=.\libs\c64c64dtv\Debug
INTDIR=.\libs\c64c64dtv\Debug
# Begin Custom Macros
OutDir=.\libs\c64c64dtv\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c64c64dtv.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\c64c64dtv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c64bus.obj"
	-@erase "$(INTDIR)\c64drive.obj"
	-@erase "$(INTDIR)\c64fastiec.obj"
	-@erase "$(INTDIR)\c64keyboard.obj"
	-@erase "$(INTDIR)\c64parallel.obj"
	-@erase "$(INTDIR)\c64printer.obj"
	-@erase "$(INTDIR)\c64rom.obj"
	-@erase "$(INTDIR)\c64romset.obj"
	-@erase "$(INTDIR)\c64rsuser.obj"
	-@erase "$(INTDIR)\c64video.obj"
	-@erase "$(INTDIR)\patchrom.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\c64c64dtv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\drive\iec\c64exp" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\rs232drv" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c64c64dtv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c64c64dtv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c64c64dtv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c64bus.obj" \
	"$(INTDIR)\c64drive.obj" \
	"$(INTDIR)\c64fastiec.obj" \
	"$(INTDIR)\c64keyboard.obj" \
	"$(INTDIR)\c64parallel.obj" \
	"$(INTDIR)\c64printer.obj" \
	"$(INTDIR)\c64rom.obj" \
	"$(INTDIR)\c64romset.obj" \
	"$(INTDIR)\c64rsuser.obj" \
	"$(INTDIR)\c64video.obj" \
	"$(INTDIR)\patchrom.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\c64c64dtv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "c64c64dtv - Win32 Release" || "$(CFG)" == "c64c64dtv - Win32 Debug"

!IF  "$(CFG)" == "c64c64dtv - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "c64c64dtv - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\c64\c64bus.c

"$(INTDIR)\c64bus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64drive.c

"$(INTDIR)\c64drive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64fastiec.c

"$(INTDIR)\c64fastiec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64keyboard.c

"$(INTDIR)\c64keyboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64parallel.c

"$(INTDIR)\c64parallel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64printer.c

"$(INTDIR)\c64printer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64rom.c

"$(INTDIR)\c64rom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64romset.c

"$(INTDIR)\c64romset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64rsuser.c

"$(INTDIR)\c64rsuser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64video.c

"$(INTDIR)\c64video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\patchrom.c

"$(INTDIR)\patchrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

