# Microsoft Developer Studio Generated NMAKE File, Based on gfxoutputdrv.dsp
!IF "$(CFG)" == ""
CFG=gfxoutputdrv - Win32 Debug
!MESSAGE No configuration specified. Defaulting to gfxoutputdrv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "gfxoutputdrv - Win32 Release" && "$(CFG)" != "gfxoutputdrv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gfxoutputdrv.mak" CFG="gfxoutputdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gfxoutputdrv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gfxoutputdrv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "gfxoutputdrv - Win32 Release"

OUTDIR=.\libs\gfxoutputdrv\Release
INTDIR=.\libs\gfxoutputdrv\Release
# Begin Custom Macros
OutDir=.\libs\gfxoutputdrv\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\gfxoutputdrv.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\gfxoutputdrv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\bmpdrv.obj"
	-@erase "$(INTDIR)\doodledrv.obj"
	-@erase "$(INTDIR)\gfxoutput.obj"
	-@erase "$(INTDIR)\iffdrv.obj"
	-@erase "$(INTDIR)\pcxdrv.obj"
	-@erase "$(INTDIR)\ppmdrv.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\gfxoutputdrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\lib\libffmpeg" /I "..\..\win32\msvc" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\gfxoutputdrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gfxoutputdrv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\gfxoutputdrv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bmpdrv.obj" \
	"$(INTDIR)\doodledrv.obj" \
	"$(INTDIR)\gfxoutput.obj" \
	"$(INTDIR)\iffdrv.obj" \
	"$(INTDIR)\pcxdrv.obj" \
	"$(INTDIR)\ppmdrv.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\gfxoutputdrv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gfxoutputdrv - Win32 Debug"

OUTDIR=.\libs\gfxoutputdrv\Debug
INTDIR=.\libs\gfxoutputdrv\Debug
# Begin Custom Macros
OutDir=.\libs\gfxoutputdrv\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\gfxoutputdrv.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\gfxoutputdrv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\bmpdrv.obj"
	-@erase "$(INTDIR)\doodledrv.obj"
	-@erase "$(INTDIR)\gfxoutput.obj"
	-@erase "$(INTDIR)\iffdrv.obj"
	-@erase "$(INTDIR)\pcxdrv.obj"
	-@erase "$(INTDIR)\ppmdrv.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\gfxoutputdrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\lib\libffmpeg" /I "..\..\win32\msvc" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\gfxoutputdrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gfxoutputdrv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\gfxoutputdrv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bmpdrv.obj" \
	"$(INTDIR)\doodledrv.obj" \
	"$(INTDIR)\gfxoutput.obj" \
	"$(INTDIR)\iffdrv.obj" \
	"$(INTDIR)\pcxdrv.obj" \
	"$(INTDIR)\ppmdrv.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\gfxoutputdrv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "gfxoutputdrv - Win32 Release" || "$(CFG)" == "gfxoutputdrv - Win32 Debug"

!IF  "$(CFG)" == "gfxoutputdrv - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "gfxoutputdrv - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\gfxoutputdrv\bmpdrv.c

"$(INTDIR)\bmpdrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\gfxoutputdrv\doodledrv.c

"$(INTDIR)\doodledrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\gfxoutputdrv\gfxoutput.c

"$(INTDIR)\gfxoutput.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\gfxoutputdrv\iffdrv.c

"$(INTDIR)\iffdrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\gfxoutputdrv\pcxdrv.c

"$(INTDIR)\pcxdrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\gfxoutputdrv\ppmdrv.c

"$(INTDIR)\ppmdrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

