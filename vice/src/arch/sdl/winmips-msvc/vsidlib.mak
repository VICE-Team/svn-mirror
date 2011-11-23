# Microsoft Developer Studio Generated NMAKE File, Based on vsidlib.dsp
!IF "$(CFG)" == ""
CFG=vsidlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to vsidlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "vsidlib - Win32 Release" && "$(CFG)" != "vsidlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsidlib.mak" CFG="vsidlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsidlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vsidlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "vsidlib - Win32 Release"

OUTDIR=.\libs\vsidlib\Release
INTDIR=.\libs\vsidlib\Release
# Begin Custom Macros
OutDir=.\libs\vsidlib\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vsidlib.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\vsidlib.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c64-cmdline-options.obj"
	-@erase "$(INTDIR)\c64-resources.obj"
	-@erase "$(INTDIR)\c64-snapshot.obj"
	-@erase "$(INTDIR)\c64.obj"
	-@erase "$(INTDIR)\c64bus.obj"
	-@erase "$(INTDIR)\c64cia1.obj"
	-@erase "$(INTDIR)\c64datasette.obj"
	-@erase "$(INTDIR)\c64export.obj"
	-@erase "$(INTDIR)\c64fastiec.obj"
	-@erase "$(INTDIR)\c64gluelogic.obj"
	-@erase "$(INTDIR)\c64iec.obj"
	-@erase "$(INTDIR)\c64io.obj"
	-@erase "$(INTDIR)\c64keyboard.obj"
	-@erase "$(INTDIR)\c64meminit.obj"
	-@erase "$(INTDIR)\c64memlimit.obj"
	-@erase "$(INTDIR)\c64memrom.obj"
	-@erase "$(INTDIR)\c64memsnapshot.obj"
	-@erase "$(INTDIR)\c64model.obj"
	-@erase "$(INTDIR)\c64pla.obj"
	-@erase "$(INTDIR)\c64rom.obj"
	-@erase "$(INTDIR)\c64romset.obj"
	-@erase "$(INTDIR)\c64rsuser.obj"
	-@erase "$(INTDIR)\c64sound.obj"
	-@erase "$(INTDIR)\c64video.obj"
	-@erase "$(INTDIR)\patchrom.obj"
	-@erase "$(INTDIR)\psid.obj"
	-@erase "$(INTDIR)\reloc65.obj"
	-@erase "$(INTDIR)\vsid.obj"
	-@erase "$(INTDIR)\vsidcia2.obj"
	-@erase "$(INTDIR)\vsidmem.obj"
	-@erase "$(INTDIR)\vsidstubs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\vsidlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64\cart" /I "..\..\..\drive" /I "..\..\..\userport" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\rs232drv" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vsidlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vsidlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vsidlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c64-cmdline-options.obj" \
	"$(INTDIR)\c64-resources.obj" \
	"$(INTDIR)\c64-snapshot.obj" \
	"$(INTDIR)\c64.obj" \
	"$(INTDIR)\c64bus.obj" \
	"$(INTDIR)\c64cia1.obj" \
	"$(INTDIR)\c64datasette.obj" \
	"$(INTDIR)\c64export.obj" \
	"$(INTDIR)\c64fastiec.obj" \
	"$(INTDIR)\c64gluelogic.obj" \
	"$(INTDIR)\c64iec.obj" \
	"$(INTDIR)\c64io.obj" \
	"$(INTDIR)\c64keyboard.obj" \
	"$(INTDIR)\c64meminit.obj" \
	"$(INTDIR)\c64memlimit.obj" \
	"$(INTDIR)\c64memrom.obj" \
	"$(INTDIR)\c64memsnapshot.obj" \
	"$(INTDIR)\c64model.obj" \
	"$(INTDIR)\c64pla.obj" \
	"$(INTDIR)\c64rom.obj" \
	"$(INTDIR)\c64romset.obj" \
	"$(INTDIR)\c64rsuser.obj" \
	"$(INTDIR)\c64sound.obj" \
	"$(INTDIR)\c64video.obj" \
	"$(INTDIR)\patchrom.obj" \
	"$(INTDIR)\psid.obj" \
	"$(INTDIR)\reloc65.obj" \
	"$(INTDIR)\vsid.obj" \
	"$(INTDIR)\vsidcia2.obj" \
	"$(INTDIR)\vsidmem.obj" \
	"$(INTDIR)\vsidstubs.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\vsidlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vsidlib - Win32 Debug"

OUTDIR=.\libs\vsidlib\Debug
INTDIR=.\libs\vsidlib\Debug
# Begin Custom Macros
OutDir=.\libs\vsidlib\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vsidlib.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\vsidlib.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c64-cmdline-options.obj"
	-@erase "$(INTDIR)\c64-resources.obj"
	-@erase "$(INTDIR)\c64-snapshot.obj"
	-@erase "$(INTDIR)\c64.obj"
	-@erase "$(INTDIR)\c64bus.obj"
	-@erase "$(INTDIR)\c64cia1.obj"
	-@erase "$(INTDIR)\c64datasette.obj"
	-@erase "$(INTDIR)\c64export.obj"
	-@erase "$(INTDIR)\c64fastiec.obj"
	-@erase "$(INTDIR)\c64gluelogic.obj"
	-@erase "$(INTDIR)\c64iec.obj"
	-@erase "$(INTDIR)\c64io.obj"
	-@erase "$(INTDIR)\c64keyboard.obj"
	-@erase "$(INTDIR)\c64meminit.obj"
	-@erase "$(INTDIR)\c64memlimit.obj"
	-@erase "$(INTDIR)\c64memrom.obj"
	-@erase "$(INTDIR)\c64memsnapshot.obj"
	-@erase "$(INTDIR)\c64model.obj"
	-@erase "$(INTDIR)\c64pla.obj"
	-@erase "$(INTDIR)\c64rom.obj"
	-@erase "$(INTDIR)\c64romset.obj"
	-@erase "$(INTDIR)\c64rsuser.obj"
	-@erase "$(INTDIR)\c64sound.obj"
	-@erase "$(INTDIR)\c64video.obj"
	-@erase "$(INTDIR)\patchrom.obj"
	-@erase "$(INTDIR)\psid.obj"
	-@erase "$(INTDIR)\reloc65.obj"
	-@erase "$(INTDIR)\vsid.obj"
	-@erase "$(INTDIR)\vsidcia2.obj"
	-@erase "$(INTDIR)\vsidmem.obj"
	-@erase "$(INTDIR)\vsidstubs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\vsidlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64\cart" /I "..\..\..\drive" /I "..\..\..\userport" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\rs232drv" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vsidlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vsidlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vsidlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c64-cmdline-options.obj" \
	"$(INTDIR)\c64-resources.obj" \
	"$(INTDIR)\c64-snapshot.obj" \
	"$(INTDIR)\c64.obj" \
	"$(INTDIR)\c64bus.obj" \
	"$(INTDIR)\c64cia1.obj" \
	"$(INTDIR)\c64datasette.obj" \
	"$(INTDIR)\c64export.obj" \
	"$(INTDIR)\c64fastiec.obj" \
	"$(INTDIR)\c64gluelogic.obj" \
	"$(INTDIR)\c64iec.obj" \
	"$(INTDIR)\c64io.obj" \
	"$(INTDIR)\c64keyboard.obj" \
	"$(INTDIR)\c64meminit.obj" \
	"$(INTDIR)\c64memlimit.obj" \
	"$(INTDIR)\c64memrom.obj" \
	"$(INTDIR)\c64memsnapshot.obj" \
	"$(INTDIR)\c64model.obj" \
	"$(INTDIR)\c64pla.obj" \
	"$(INTDIR)\c64rom.obj" \
	"$(INTDIR)\c64romset.obj" \
	"$(INTDIR)\c64rsuser.obj" \
	"$(INTDIR)\c64sound.obj" \
	"$(INTDIR)\c64video.obj" \
	"$(INTDIR)\patchrom.obj" \
	"$(INTDIR)\psid.obj" \
	"$(INTDIR)\reloc65.obj" \
	"$(INTDIR)\vsid.obj" \
	"$(INTDIR)\vsidcia2.obj" \
	"$(INTDIR)\vsidmem.obj" \
	"$(INTDIR)\vsidstubs.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\vsidlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "vsidlib - Win32 Release" || "$(CFG)" == "vsidlib - Win32 Debug"

!IF  "$(CFG)" == "vsidlib - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsidlib - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\c64\c64-cmdline-options.c"

"$(INTDIR)\c64-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\c64-resources.c"

"$(INTDIR)\c64-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\c64-snapshot.c"

"$(INTDIR)\c64-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64.c

"$(INTDIR)\c64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64bus.c

"$(INTDIR)\c64bus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64cia1.c

"$(INTDIR)\c64cia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\vsidcia2.c

"$(INTDIR)\vsidcia2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64datasette.c

"$(INTDIR)\c64datasette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64export.c

"$(INTDIR)\c64export.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64fastiec.c

"$(INTDIR)\c64fastiec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64gluelogic.c

"$(INTDIR)\c64gluelogic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64iec.c

"$(INTDIR)\c64iec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64io.c

"$(INTDIR)\c64io.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64keyboard.c

"$(INTDIR)\c64keyboard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64meminit.c

"$(INTDIR)\c64meminit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64memlimit.c

"$(INTDIR)\c64memlimit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64memrom.c

"$(INTDIR)\c64memrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64memsnapshot.c

"$(INTDIR)\c64memsnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64model.c

"$(INTDIR)\c64model.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64pla.c

"$(INTDIR)\c64pla.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE=..\..\..\c64\c64sound.c

"$(INTDIR)\c64sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\c64video.c

"$(INTDIR)\c64video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\patchrom.c

"$(INTDIR)\patchrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\psid.c

"$(INTDIR)\psid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\reloc65.c

"$(INTDIR)\reloc65.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

SOURCE=..\..\..\c64\vsid.c

"$(INTDIR)\vsid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\vsidmem.c

"$(INTDIR)\vsidmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\vsidstubs.c

"$(INTDIR)\vsidstubs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

