# Microsoft Developer Studio Generated NMAKE File, Based on base.dsp
!IF "$(CFG)" == ""
CFG=base - Win32 Debug
!MESSAGE No configuration specified. Defaulting to base - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "base - Win32 Release" && "$(CFG)" != "base - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "base.mak" CFG="base - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "base - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "base - Win32 Release"

OUTDIR=.\libs\base\Release
INTDIR=.\libs\base\Release
# Begin Custom Macros
OutDir=.\libs\base\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\translate_table.h" "$(OUTDIR)\base.lib"

!ELSE 

ALL : "gentranslate - Win32 Release" "geninfocontrib - Win32 Release" "..\..\..\translate_table.h" "$(OUTDIR)\base.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"geninfocontrib - Win32 ReleaseCLEAN" "gentranslate - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\alarm.obj"
	-@erase "$(INTDIR)\attach.obj"
	-@erase "$(INTDIR)\autostart-prg.obj"
	-@erase "$(INTDIR)\autostart.obj"
	-@erase "$(INTDIR)\cbmdos.obj"
	-@erase "$(INTDIR)\cbmimage.obj"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\clipboard.obj"
	-@erase "$(INTDIR)\clkguard.obj"
	-@erase "$(INTDIR)\cmdline.obj"
	-@erase "$(INTDIR)\color.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\datasette.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dma.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\findpath.obj"
	-@erase "$(INTDIR)\fliplist.obj"
	-@erase "$(INTDIR)\gcr.obj"
	-@erase "$(INTDIR)\info.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\initcmdline.obj"
	-@erase "$(INTDIR)\interrupt.obj"
	-@erase "$(INTDIR)\ioutil.obj"
	-@erase "$(INTDIR)\joystick.obj"
	-@erase "$(INTDIR)\kbdbuf.obj"
	-@erase "$(INTDIR)\keyboard.obj"
	-@erase "$(INTDIR)\lib.obj"
	-@erase "$(INTDIR)\lightpen.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\machine-bus.obj"
	-@erase "$(INTDIR)\machine.obj"
	-@erase "$(INTDIR)\midi.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\network.obj"
	-@erase "$(INTDIR)\opencbmlib.obj"
	-@erase "$(INTDIR)\palette.obj"
	-@erase "$(INTDIR)\ram.obj"
	-@erase "$(INTDIR)\rawfile.obj"
	-@erase "$(INTDIR)\resources.obj"
	-@erase "$(INTDIR)\romset.obj"
	-@erase "$(INTDIR)\screenshot.obj"
	-@erase "$(INTDIR)\snapshot.obj"
	-@erase "$(INTDIR)\socket.obj"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sysfile.obj"
	-@erase "$(INTDIR)\translate.obj"
	-@erase "$(INTDIR)\traps.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vsync.obj"
	-@erase "$(INTDIR)\zfile.obj"
	-@erase "$(INTDIR)\zipcode.obj"
	-@erase "$(OUTDIR)\base.lib"
	-@erase "..\..\..\translate_table.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\userport" /I "..\..\..\rs232drv" /I "..\..\..\imagecontents" /I "..\..\..\monitor" /I "..\..\..\lib\p64" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\base.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\base.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\base.lib" 
LIB32_OBJS= \
	"$(INTDIR)\alarm.obj" \
	"$(INTDIR)\attach.obj" \
	"$(INTDIR)\autostart-prg.obj" \
	"$(INTDIR)\autostart.obj" \
	"$(INTDIR)\cbmdos.obj" \
	"$(INTDIR)\cbmimage.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\clipboard.obj" \
	"$(INTDIR)\clkguard.obj" \
	"$(INTDIR)\cmdline.obj" \
	"$(INTDIR)\color.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\datasette.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dma.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\findpath.obj" \
	"$(INTDIR)\fliplist.obj" \
	"$(INTDIR)\gcr.obj" \
	"$(INTDIR)\info.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\initcmdline.obj" \
	"$(INTDIR)\interrupt.obj" \
	"$(INTDIR)\ioutil.obj" \
	"$(INTDIR)\joystick.obj" \
	"$(INTDIR)\kbdbuf.obj" \
	"$(INTDIR)\keyboard.obj" \
	"$(INTDIR)\lib.obj" \
	"$(INTDIR)\lightpen.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\machine-bus.obj" \
	"$(INTDIR)\machine.obj" \
	"$(INTDIR)\midi.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\network.obj" \
	"$(INTDIR)\opencbmlib.obj" \
	"$(INTDIR)\palette.obj" \
	"$(INTDIR)\ram.obj" \
	"$(INTDIR)\rawfile.obj" \
	"$(INTDIR)\resources.obj" \
	"$(INTDIR)\romset.obj" \
	"$(INTDIR)\screenshot.obj" \
	"$(INTDIR)\snapshot.obj" \
	"$(INTDIR)\socket.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\sysfile.obj" \
	"$(INTDIR)\translate.obj" \
	"$(INTDIR)\traps.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\vsync.obj" \
	"$(INTDIR)\zfile.obj" \
	"$(INTDIR)\zipcode.obj"

"$(OUTDIR)\base.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

OUTDIR=.\libs\base\Debug
INTDIR=.\libs\base\Debug
# Begin Custom Macros
OutDir=.\libs\base\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\translate.h" "$(OUTDIR)\base.lib"

!ELSE 

ALL : "gentranslate - Win32 Debug" "geninfocontrib - Win32 Debug" "..\..\..\translate.h" "$(OUTDIR)\base.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"geninfocontrib - Win32 DebugCLEAN" "gentranslate - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\alarm.obj"
	-@erase "$(INTDIR)\attach.obj"
	-@erase "$(INTDIR)\autostart-prg.obj"
	-@erase "$(INTDIR)\autostart.obj"
	-@erase "$(INTDIR)\cbmdos.obj"
	-@erase "$(INTDIR)\cbmimage.obj"
	-@erase "$(INTDIR)\charset.obj"
	-@erase "$(INTDIR)\clipboard.obj"
	-@erase "$(INTDIR)\clkguard.obj"
	-@erase "$(INTDIR)\cmdline.obj"
	-@erase "$(INTDIR)\color.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\datasette.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dma.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\findpath.obj"
	-@erase "$(INTDIR)\fliplist.obj"
	-@erase "$(INTDIR)\gcr.obj"
	-@erase "$(INTDIR)\info.obj"
	-@erase "$(INTDIR)\init.obj"
	-@erase "$(INTDIR)\initcmdline.obj"
	-@erase "$(INTDIR)\interrupt.obj"
	-@erase "$(INTDIR)\ioutil.obj"
	-@erase "$(INTDIR)\joystick.obj"
	-@erase "$(INTDIR)\kbdbuf.obj"
	-@erase "$(INTDIR)\keyboard.obj"
	-@erase "$(INTDIR)\lib.obj"
	-@erase "$(INTDIR)\lightpen.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\machine-bus.obj"
	-@erase "$(INTDIR)\machine.obj"
	-@erase "$(INTDIR)\midi.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\network.obj"
	-@erase "$(INTDIR)\opencbmlib.obj"
	-@erase "$(INTDIR)\palette.obj"
	-@erase "$(INTDIR)\ram.obj"
	-@erase "$(INTDIR)\rawfile.obj"
	-@erase "$(INTDIR)\resources.obj"
	-@erase "$(INTDIR)\romset.obj"
	-@erase "$(INTDIR)\screenshot.obj"
	-@erase "$(INTDIR)\snapshot.obj"
	-@erase "$(INTDIR)\socket.obj"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sysfile.obj"
	-@erase "$(INTDIR)\translate.obj"
	-@erase "$(INTDIR)\traps.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vsync.obj"
	-@erase "$(INTDIR)\zfile.obj"
	-@erase "$(INTDIR)\zipcode.obj"
	-@erase "$(OUTDIR)\base.lib"
	-@erase "..\..\..\translate.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\userport" /I "..\..\..\rs232drv" /I "..\..\..\imagecontents" /I "..\..\..\monitor" /I "..\..\..\lib\p64" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\base.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\base.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\base.lib" 
LIB32_OBJS= \
	"$(INTDIR)\alarm.obj" \
	"$(INTDIR)\attach.obj" \
	"$(INTDIR)\autostart-prg.obj" \
	"$(INTDIR)\autostart.obj" \
	"$(INTDIR)\cbmdos.obj" \
	"$(INTDIR)\cbmimage.obj" \
	"$(INTDIR)\charset.obj" \
	"$(INTDIR)\clipboard.obj" \
	"$(INTDIR)\clkguard.obj" \
	"$(INTDIR)\cmdline.obj" \
	"$(INTDIR)\color.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\datasette.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dma.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\findpath.obj" \
	"$(INTDIR)\fliplist.obj" \
	"$(INTDIR)\gcr.obj" \
	"$(INTDIR)\info.obj" \
	"$(INTDIR)\init.obj" \
	"$(INTDIR)\initcmdline.obj" \
	"$(INTDIR)\interrupt.obj" \
	"$(INTDIR)\ioutil.obj" \
	"$(INTDIR)\joystick.obj" \
	"$(INTDIR)\kbdbuf.obj" \
	"$(INTDIR)\keyboard.obj" \
	"$(INTDIR)\lib.obj" \
	"$(INTDIR)\lightpen.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\machine-bus.obj" \
	"$(INTDIR)\machine.obj" \
	"$(INTDIR)\midi.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\network.obj" \
	"$(INTDIR)\opencbmlib.obj" \
	"$(INTDIR)\palette.obj" \
	"$(INTDIR)\ram.obj" \
	"$(INTDIR)\rawfile.obj" \
	"$(INTDIR)\resources.obj" \
	"$(INTDIR)\romset.obj" \
	"$(INTDIR)\screenshot.obj" \
	"$(INTDIR)\snapshot.obj" \
	"$(INTDIR)\socket.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\sysfile.obj" \
	"$(INTDIR)\translate.obj" \
	"$(INTDIR)\traps.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\vsync.obj" \
	"$(INTDIR)\zfile.obj" \
	"$(INTDIR)\zipcode.obj"

"$(OUTDIR)\base.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "base - Win32 Release" || "$(CFG)" == "base - Win32 Debug"

!IF  "$(CFG)" == "base - Win32 Release"

"geninfocontrib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\geninfocontrib.mak" CFG="geninfocontrib - Win32 Release" 
   cd "."

"geninfocontrib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\geninfocontrib.mak" CFG="geninfocontrib - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

"geninfocontrib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\geninfocontrib.mak" CFG="geninfocontrib - Win32 Debug" 
   cd "."

"geninfocontrib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\geninfocontrib.mak" CFG="geninfocontrib - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "base - Win32 Release"

"gentranslate - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gentranslate.mak" CFG="gentranslate - Win32 Release" 
   cd "."

"gentranslate - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gentranslate.mak" CFG="gentranslate - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

"gentranslate - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gentranslate.mak" CFG="gentranslate - Win32 Debug" 
   cd "."

"gentranslate - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gentranslate.mak" CFG="gentranslate - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\alarm.c"

"$(INTDIR)\alarm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\attach.c"

"$(INTDIR)\attach.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\autostart-prg.c"

"$(INTDIR)\autostart-prg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\autostart.c"

"$(INTDIR)\autostart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\cbmdos.c"

"$(INTDIR)\cbmdos.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\cbmimage.c"

"$(INTDIR)\cbmimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\charset.c"

"$(INTDIR)\charset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\clipboard.c"

"$(INTDIR)\clipboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\clkguard.c"

"$(INTDIR)\clkguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\cmdline.c"

"$(INTDIR)\cmdline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\color.c"

"$(INTDIR)\color.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\crc32.c"

"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\datasette.c"

"$(INTDIR)\datasette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\debug.c"

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\dma.c"

"$(INTDIR)\dma.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\event.c"

"$(INTDIR)\event.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\findpath.c"

"$(INTDIR)\findpath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\fliplist.c"

"$(INTDIR)\fliplist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\gcr.c"

"$(INTDIR)\gcr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\info.c"

"$(INTDIR)\info.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\init.c"

"$(INTDIR)\init.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\initcmdline.c"

"$(INTDIR)\initcmdline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\interrupt.c"

"$(INTDIR)\interrupt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\ioutil.c"

"$(INTDIR)\ioutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\joystick.c"

"$(INTDIR)\joystick.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\kbdbuf.c"

"$(INTDIR)\kbdbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\keyboard.c"

"$(INTDIR)\keyboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\lib.c"

"$(INTDIR)\lib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\lightpen.c"

"$(INTDIR)\lightpen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\log.c"

"$(INTDIR)\log.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\machine-bus.c"

"$(INTDIR)\machine-bus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\machine.c"

"$(INTDIR)\machine.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\midi.c"

"$(INTDIR)\midi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\mouse.c"

"$(INTDIR)\mouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\network.c"

"$(INTDIR)\network.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\opencbmlib.c"

"$(INTDIR)\opencbmlib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\palette.c"

"$(INTDIR)\palette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\ram.c"

"$(INTDIR)\ram.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\rawfile.c"

"$(INTDIR)\rawfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\resources.c"

"$(INTDIR)\resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\romset.c"

"$(INTDIR)\romset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\screenshot.c"

"$(INTDIR)\screenshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\snapshot.c"

"$(INTDIR)\snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\socket.c"

"$(INTDIR)\socket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\sound.c"

"$(INTDIR)\sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\sysfile.c"

"$(INTDIR)\sysfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\translate.c"

"$(INTDIR)\translate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\translate.txt"

!IF  "$(CFG)" == "base - Win32 Release"

InputDir=..\..\..
InputPath="..\..\..\translate.txt"
USERDEP__TRANS="..\..\..\translate.txt"	

"..\..\..\translate_table.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__TRANS)
	<<tempfile.bat 
	@echo off 
	..\..\..\..\data\gentranslate ..\..\..\translate.txt ..\..\..\translate.h ..\..\..\translate_table.h
<< 
	

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

InputDir=..\..\..
InputPath="..\..\..\translate.txt"
USERDEP__TRANS="..\..\..\translate.txt"	

"..\..\..\translate.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__TRANS)
	<<tempfile.bat 
	@echo off 
	..\..\..\..\data\gentranslate ..\..\..\translate.txt ..\..\..\translate.h ..\..\..\translate_table.h
<< 
	

!ENDIF 

SOURCE="..\..\..\traps.c"

"$(INTDIR)\traps.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\util.c"

"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vsync.c"

"$(INTDIR)\vsync.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\zfile.c"

"$(INTDIR)\zfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\zipcode.c"

"$(INTDIR)\zipcode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

