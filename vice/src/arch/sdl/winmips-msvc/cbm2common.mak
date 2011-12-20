# Microsoft Developer Studio Generated NMAKE File, Based on cbm2common.dsp
!IF "$(CFG)" == ""
CFG=cbm2common - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cbm2common - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cbm2common - Win32 Release" && "$(CFG)" != "cbm2common - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cbm2common.mak" CFG="cbm2common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cbm2common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cbm2common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cbm2common - Win32 Release"

OUTDIR=.\libs\cbm2common\Release
INTDIR=.\libs\cbm2common\Release
# Begin Custom Macros
OutDir=.\libs\cbm2common\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cbm2common.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\cbm2common.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cbm2-cmdline-options.obj"
	-@erase "$(INTDIR)\cbm2acia1.obj"
	-@erase "$(INTDIR)\cbm2bus.obj"
	-@erase "$(INTDIR)\cbm2cart.obj"
	-@erase "$(INTDIR)\cbm2cia1.obj"
	-@erase "$(INTDIR)\cbm2cpu.obj"
	-@erase "$(INTDIR)\cbm2datasette.obj"
	-@erase "$(INTDIR)\cbm2drive.obj"
	-@erase "$(INTDIR)\cbm2iec.obj"
	-@erase "$(INTDIR)\cbm2memsnapshot.obj"
	-@erase "$(INTDIR)\cbm2model.obj"
	-@erase "$(INTDIR)\cbm2printer.obj"
	-@erase "$(INTDIR)\cbm2romset.obj"
	-@erase "$(INTDIR)\cbm2sound.obj"
	-@erase "$(INTDIR)\cbm2tpi1.obj"
	-@erase "$(INTDIR)\cbm2tpi2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cbm2common.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\userport" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\crtc" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\cbm2common.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cbm2common.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\cbm2common.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cbm2-cmdline-options.obj" \
	"$(INTDIR)\cbm2acia1.obj" \
	"$(INTDIR)\cbm2bus.obj" \
	"$(INTDIR)\cbm2cart.obj" \
	"$(INTDIR)\cbm2cia1.obj" \
	"$(INTDIR)\cbm2cpu.obj" \
	"$(INTDIR)\cbm2datasette.obj" \
	"$(INTDIR)\cbm2drive.obj" \
	"$(INTDIR)\cbm2iec.obj" \
	"$(INTDIR)\cbm2memsnapshot.obj" \
	"$(INTDIR)\cbm2model.obj" \
	"$(INTDIR)\cbm2printer.obj" \
	"$(INTDIR)\cbm2romset.obj" \
	"$(INTDIR)\cbm2sound.obj" \
	"$(INTDIR)\cbm2tpi1.obj" \
	"$(INTDIR)\cbm2tpi2.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\cbm2common.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cbm2common - Win32 Debug"

OUTDIR=.\libs\cbm2common\Debug
INTDIR=.\libs\cbm2common\Debug
# Begin Custom Macros
OutDir=.\libs\cbm2common\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cbm2common.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\cbm2common.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cbm2-cmdline-options.obj"
	-@erase "$(INTDIR)\cbm2acia1.obj"
	-@erase "$(INTDIR)\cbm2bus.obj"
	-@erase "$(INTDIR)\cbm2cart.obj"
	-@erase "$(INTDIR)\cbm2cia1.obj"
	-@erase "$(INTDIR)\cbm2cpu.obj"
	-@erase "$(INTDIR)\cbm2datasette.obj"
	-@erase "$(INTDIR)\cbm2drive.obj"
	-@erase "$(INTDIR)\cbm2iec.obj"
	-@erase "$(INTDIR)\cbm2memsnapshot.obj"
	-@erase "$(INTDIR)\cbm2model.obj"
	-@erase "$(INTDIR)\cbm2printer.obj"
	-@erase "$(INTDIR)\cbm2romset.obj"
	-@erase "$(INTDIR)\cbm2sound.obj"
	-@erase "$(INTDIR)\cbm2tpi1.obj"
	-@erase "$(INTDIR)\cbm2tpi2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cbm2common.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\userport" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\crtc" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\cbm2common.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cbm2common.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\cbm2common.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cbm2-cmdline-options.obj" \
	"$(INTDIR)\cbm2acia1.obj" \
	"$(INTDIR)\cbm2bus.obj" \
	"$(INTDIR)\cbm2cart.obj" \
	"$(INTDIR)\cbm2cia1.obj" \
	"$(INTDIR)\cbm2cpu.obj" \
	"$(INTDIR)\cbm2datasette.obj" \
	"$(INTDIR)\cbm2drive.obj" \
	"$(INTDIR)\cbm2iec.obj" \
	"$(INTDIR)\cbm2memsnapshot.obj" \
	"$(INTDIR)\cbm2model.obj" \
	"$(INTDIR)\cbm2printer.obj" \
	"$(INTDIR)\cbm2romset.obj" \
	"$(INTDIR)\cbm2sound.obj" \
	"$(INTDIR)\cbm2tpi1.obj" \
	"$(INTDIR)\cbm2tpi2.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\cbm2common.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "cbm2common - Win32 Release" || "$(CFG)" == "cbm2common - Win32 Debug"

!IF  "$(CFG)" == "cbm2common - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "cbm2common - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\cbm2\cbm2-cmdline-options.c"

"$(INTDIR)\cbm2-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2acia1.c

"$(INTDIR)\cbm2acia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2bus.c

"$(INTDIR)\cbm2bus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2cart.c

"$(INTDIR)\cbm2cart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2cia1.c

"$(INTDIR)\cbm2cia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2cpu.c

"$(INTDIR)\cbm2cpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2datasette.c

"$(INTDIR)\cbm2datasette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2drive.c

"$(INTDIR)\cbm2drive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2iec.c

"$(INTDIR)\cbm2iec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2memsnapshot.c

"$(INTDIR)\cbm2memsnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2model.c

"$(INTDIR)\cbm2model.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2printer.c

"$(INTDIR)\cbm2printer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2romset.c

"$(INTDIR)\cbm2romset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2sound.c

"$(INTDIR)\cbm2sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2tpi1.c

"$(INTDIR)\cbm2tpi1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cbm2\cbm2tpi2.c

"$(INTDIR)\cbm2tpi2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

