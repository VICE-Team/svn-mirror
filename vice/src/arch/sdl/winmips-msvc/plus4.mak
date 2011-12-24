# Microsoft Developer Studio Generated NMAKE File, Based on plus4.dsp
!IF "$(CFG)" == ""
CFG=plus4 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to plus4 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "plus4 - Win32 Release" && "$(CFG)" != "plus4 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plus4.mak" CFG="plus4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plus4 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "plus4 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "plus4 - Win32 Release"

OUTDIR=.\libs\plus4\Release
INTDIR=.\libs\plus4\Release
# Begin Custom Macros
OutDir=.\libs\plus4\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\plus4.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\plus4.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\digiblaster.obj"
	-@erase "$(INTDIR)\plus4-cmdline-options.obj"
	-@erase "$(INTDIR)\plus4-resources.obj"
	-@erase "$(INTDIR)\plus4-sidcart.obj"
	-@erase "$(INTDIR)\plus4-snapshot.obj"
	-@erase "$(INTDIR)\plus4.obj"
	-@erase "$(INTDIR)\plus4acia.obj"
	-@erase "$(INTDIR)\plus4bus.obj"
	-@erase "$(INTDIR)\plus4cart.obj"
	-@erase "$(INTDIR)\plus4cpu.obj"
	-@erase "$(INTDIR)\plus4datasette.obj"
	-@erase "$(INTDIR)\plus4drive.obj"
	-@erase "$(INTDIR)\plus4iec.obj"
	-@erase "$(INTDIR)\plus4mem.obj"
	-@erase "$(INTDIR)\plus4memcsory256k.obj"
	-@erase "$(INTDIR)\plus4memhannes256k.obj"
	-@erase "$(INTDIR)\plus4memlimit.obj"
	-@erase "$(INTDIR)\plus4memrom.obj"
	-@erase "$(INTDIR)\plus4memsnapshot.obj"
	-@erase "$(INTDIR)\plus4model.obj"
	-@erase "$(INTDIR)\plus4parallel.obj"
	-@erase "$(INTDIR)\plus4pio1.obj"
	-@erase "$(INTDIR)\plus4pio2.obj"
	-@erase "$(INTDIR)\plus4printer.obj"
	-@erase "$(INTDIR)\plus4rom.obj"
	-@erase "$(INTDIR)\plus4romset.obj"
	-@erase "$(INTDIR)\plus4speech.obj"
	-@erase "$(INTDIR)\plus4tcbm.obj"
	-@erase "$(INTDIR)\plus4video.obj"
	-@erase "$(INTDIR)\ted-badline.obj"
	-@erase "$(INTDIR)\ted-cmdline-options.obj"
	-@erase "$(INTDIR)\ted-color.obj"
	-@erase "$(INTDIR)\ted-draw.obj"
	-@erase "$(INTDIR)\ted-fetch.obj"
	-@erase "$(INTDIR)\ted-irq.obj"
	-@erase "$(INTDIR)\ted-mem.obj"
	-@erase "$(INTDIR)\ted-resources.obj"
	-@erase "$(INTDIR)\ted-snapshot.obj"
	-@erase "$(INTDIR)\ted-sound.obj"
	-@erase "$(INTDIR)\ted-timer.obj"
	-@erase "$(INTDIR)\ted-timing.obj"
	-@erase "$(INTDIR)\ted.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\plus4.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /I "..\..\..\core" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\plus4.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\plus4.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\plus4.lib" 
LIB32_OBJS= \
	"$(INTDIR)\digiblaster.obj" \
	"$(INTDIR)\plus4-cmdline-options.obj" \
	"$(INTDIR)\plus4-resources.obj" \
	"$(INTDIR)\plus4-sidcart.obj" \
	"$(INTDIR)\plus4-snapshot.obj" \
	"$(INTDIR)\plus4.obj" \
	"$(INTDIR)\plus4acia.obj" \
	"$(INTDIR)\plus4bus.obj" \
	"$(INTDIR)\plus4cart.obj" \
	"$(INTDIR)\plus4cpu.obj" \
	"$(INTDIR)\plus4datasette.obj" \
	"$(INTDIR)\plus4drive.obj" \
	"$(INTDIR)\plus4iec.obj" \
	"$(INTDIR)\plus4mem.obj" \
	"$(INTDIR)\plus4memcsory256k.obj" \
	"$(INTDIR)\plus4memhannes256k.obj" \
	"$(INTDIR)\plus4memlimit.obj" \
	"$(INTDIR)\plus4memrom.obj" \
	"$(INTDIR)\plus4memsnapshot.obj" \
	"$(INTDIR)\plus4model.obj" \
	"$(INTDIR)\plus4parallel.obj" \
	"$(INTDIR)\plus4pio1.obj" \
	"$(INTDIR)\plus4pio2.obj" \
	"$(INTDIR)\plus4printer.obj" \
	"$(INTDIR)\plus4rom.obj" \
	"$(INTDIR)\plus4romset.obj" \
	"$(INTDIR)\plus4speech.obj" \
	"$(INTDIR)\plus4tcbm.obj" \
	"$(INTDIR)\plus4video.obj" \
	"$(INTDIR)\ted-badline.obj" \
	"$(INTDIR)\ted-cmdline-options.obj" \
	"$(INTDIR)\ted-color.obj" \
	"$(INTDIR)\ted-draw.obj" \
	"$(INTDIR)\ted-fetch.obj" \
	"$(INTDIR)\ted-irq.obj" \
	"$(INTDIR)\ted-mem.obj" \
	"$(INTDIR)\ted-resources.obj" \
	"$(INTDIR)\ted-snapshot.obj" \
	"$(INTDIR)\ted-sound.obj" \
	"$(INTDIR)\ted-timer.obj" \
	"$(INTDIR)\ted-timing.obj" \
	"$(INTDIR)\ted.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\plus4.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "plus4 - Win32 Debug"

OUTDIR=.\libs\plus4\Debug
INTDIR=.\libs\plus4\Debug
# Begin Custom Macros
OutDir=.\libs\plus4\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\plus4.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\plus4.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\digiblaster.obj"
	-@erase "$(INTDIR)\plus4-cmdline-options.obj"
	-@erase "$(INTDIR)\plus4-resources.obj"
	-@erase "$(INTDIR)\plus4-sidcart.obj"
	-@erase "$(INTDIR)\plus4-snapshot.obj"
	-@erase "$(INTDIR)\plus4.obj"
	-@erase "$(INTDIR)\plus4acia.obj"
	-@erase "$(INTDIR)\plus4bus.obj"
	-@erase "$(INTDIR)\plus4cart.obj"
	-@erase "$(INTDIR)\plus4cpu.obj"
	-@erase "$(INTDIR)\plus4datasette.obj"
	-@erase "$(INTDIR)\plus4drive.obj"
	-@erase "$(INTDIR)\plus4iec.obj"
	-@erase "$(INTDIR)\plus4mem.obj"
	-@erase "$(INTDIR)\plus4memcsory256k.obj"
	-@erase "$(INTDIR)\plus4memhannes256k.obj"
	-@erase "$(INTDIR)\plus4memlimit.obj"
	-@erase "$(INTDIR)\plus4memrom.obj"
	-@erase "$(INTDIR)\plus4memsnapshot.obj"
	-@erase "$(INTDIR)\plus4model.obj"
	-@erase "$(INTDIR)\plus4parallel.obj"
	-@erase "$(INTDIR)\plus4pio1.obj"
	-@erase "$(INTDIR)\plus4pio2.obj"
	-@erase "$(INTDIR)\plus4printer.obj"
	-@erase "$(INTDIR)\plus4rom.obj"
	-@erase "$(INTDIR)\plus4romset.obj"
	-@erase "$(INTDIR)\plus4speech.obj"
	-@erase "$(INTDIR)\plus4tcbm.obj"
	-@erase "$(INTDIR)\plus4video.obj"
	-@erase "$(INTDIR)\ted-badline.obj"
	-@erase "$(INTDIR)\ted-cmdline-options.obj"
	-@erase "$(INTDIR)\ted-color.obj"
	-@erase "$(INTDIR)\ted-draw.obj"
	-@erase "$(INTDIR)\ted-fetch.obj"
	-@erase "$(INTDIR)\ted-irq.obj"
	-@erase "$(INTDIR)\ted-mem.obj"
	-@erase "$(INTDIR)\ted-resources.obj"
	-@erase "$(INTDIR)\ted-snapshot.obj"
	-@erase "$(INTDIR)\ted-sound.obj"
	-@erase "$(INTDIR)\ted-timer.obj"
	-@erase "$(INTDIR)\ted-timing.obj"
	-@erase "$(INTDIR)\ted.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\plus4.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /I "..\..\..\core" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\plus4.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\plus4.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\plus4.lib" 
LIB32_OBJS= \
	"$(INTDIR)\digiblaster.obj" \
	"$(INTDIR)\plus4-cmdline-options.obj" \
	"$(INTDIR)\plus4-resources.obj" \
	"$(INTDIR)\plus4-sidcart.obj" \
	"$(INTDIR)\plus4-snapshot.obj" \
	"$(INTDIR)\plus4.obj" \
	"$(INTDIR)\plus4acia.obj" \
	"$(INTDIR)\plus4bus.obj" \
	"$(INTDIR)\plus4cart.obj" \
	"$(INTDIR)\plus4cpu.obj" \
	"$(INTDIR)\plus4datasette.obj" \
	"$(INTDIR)\plus4drive.obj" \
	"$(INTDIR)\plus4iec.obj" \
	"$(INTDIR)\plus4mem.obj" \
	"$(INTDIR)\plus4memcsory256k.obj" \
	"$(INTDIR)\plus4memhannes256k.obj" \
	"$(INTDIR)\plus4memlimit.obj" \
	"$(INTDIR)\plus4memrom.obj" \
	"$(INTDIR)\plus4memsnapshot.obj" \
	"$(INTDIR)\plus4model.obj" \
	"$(INTDIR)\plus4parallel.obj" \
	"$(INTDIR)\plus4pio1.obj" \
	"$(INTDIR)\plus4pio2.obj" \
	"$(INTDIR)\plus4printer.obj" \
	"$(INTDIR)\plus4rom.obj" \
	"$(INTDIR)\plus4romset.obj" \
	"$(INTDIR)\plus4speech.obj" \
	"$(INTDIR)\plus4tcbm.obj" \
	"$(INTDIR)\plus4video.obj" \
	"$(INTDIR)\ted-badline.obj" \
	"$(INTDIR)\ted-cmdline-options.obj" \
	"$(INTDIR)\ted-color.obj" \
	"$(INTDIR)\ted-draw.obj" \
	"$(INTDIR)\ted-fetch.obj" \
	"$(INTDIR)\ted-irq.obj" \
	"$(INTDIR)\ted-mem.obj" \
	"$(INTDIR)\ted-resources.obj" \
	"$(INTDIR)\ted-snapshot.obj" \
	"$(INTDIR)\ted-sound.obj" \
	"$(INTDIR)\ted-timer.obj" \
	"$(INTDIR)\ted-timing.obj" \
	"$(INTDIR)\ted.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\plus4.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "plus4 - Win32 Release" || "$(CFG)" == "plus4 - Win32 Debug"

!IF  "$(CFG)" == "plus4 - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "plus4 - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\plus4\digiblaster.c"

"$(INTDIR)\digiblaster.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\plus4-cmdline-options.c"

"$(INTDIR)\plus4-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\plus4-resources.c"

"$(INTDIR)\plus4-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\plus4-sidcart.c"

"$(INTDIR)\plus4-sidcart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\plus4-snapshot.c"

"$(INTDIR)\plus4-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4.c

"$(INTDIR)\plus4.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\plus4acia.c"

"$(INTDIR)\plus4acia.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4bus.c

"$(INTDIR)\plus4bus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4cart.c

"$(INTDIR)\plus4cart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4cpu.c

!IF  "$(CFG)" == "plus4 - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Ot /Oa /Ow /Oi /Op /Oy /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /I "..\..\..\core" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\plus4.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

"$(INTDIR)\plus4cpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "plus4 - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /I "..\..\..\core" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\plus4.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

"$(INTDIR)\plus4cpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\plus4\plus4datasette.c

"$(INTDIR)\plus4datasette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4drive.c

"$(INTDIR)\plus4drive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4iec.c

"$(INTDIR)\plus4iec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4mem.c

"$(INTDIR)\plus4mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4memcsory256k.c

"$(INTDIR)\plus4memcsory256k.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4memhannes256k.c

"$(INTDIR)\plus4memhannes256k.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4memlimit.c

"$(INTDIR)\plus4memlimit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4memrom.c

"$(INTDIR)\plus4memrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4memsnapshot.c

"$(INTDIR)\plus4memsnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4model.c

"$(INTDIR)\plus4model.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4parallel.c

"$(INTDIR)\plus4parallel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4pio1.c

"$(INTDIR)\plus4pio1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4pio2.c

"$(INTDIR)\plus4pio2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4printer.c

"$(INTDIR)\plus4printer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4rom.c

"$(INTDIR)\plus4rom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4romset.c

"$(INTDIR)\plus4romset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4speech.c

"$(INTDIR)\plus4speech.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4tcbm.c

"$(INTDIR)\plus4tcbm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\plus4video.c

"$(INTDIR)\plus4video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-badline.c"

"$(INTDIR)\ted-badline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-cmdline-options.c"

"$(INTDIR)\ted-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-color.c"

"$(INTDIR)\ted-color.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-draw.c"

"$(INTDIR)\ted-draw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-fetch.c"

"$(INTDIR)\ted-fetch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-irq.c"

"$(INTDIR)\ted-irq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-mem.c"

"$(INTDIR)\ted-mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-resources.c"

"$(INTDIR)\ted-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-snapshot.c"

"$(INTDIR)\ted-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-sound.c"

"$(INTDIR)\ted-sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-timer.c"

"$(INTDIR)\ted-timer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\plus4\ted-timing.c"

"$(INTDIR)\ted-timing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\plus4\ted.c

"$(INTDIR)\ted.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

