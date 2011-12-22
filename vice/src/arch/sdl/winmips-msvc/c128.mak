# Microsoft Developer Studio Generated NMAKE File, Based on c128.dsp
!IF "$(CFG)" == ""
CFG=c128 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to c128 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "c128 - Win32 Release" && "$(CFG)" != "c128 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "c128.mak" CFG="c128 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "c128 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "c128 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "c128 - Win32 Release"

OUTDIR=.\libs\c128\Release
INTDIR=.\libs\c128\Release
# Begin Custom Macros
OutDir=.\libs\c128\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c128.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\c128.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c128-cmdline-options.obj"
	-@erase "$(INTDIR)\c128-resources.obj"
	-@erase "$(INTDIR)\c128-snapshot.obj"
	-@erase "$(INTDIR)\c128.obj"
	-@erase "$(INTDIR)\c128cia1.obj"
	-@erase "$(INTDIR)\c128cpu.obj"
	-@erase "$(INTDIR)\c128drive.obj"
	-@erase "$(INTDIR)\c128fastiec.obj"
	-@erase "$(INTDIR)\c128mem.obj"
	-@erase "$(INTDIR)\c128meminit.obj"
	-@erase "$(INTDIR)\c128memlimit.obj"
	-@erase "$(INTDIR)\c128memrom.obj"
	-@erase "$(INTDIR)\c128memsnapshot.obj"
	-@erase "$(INTDIR)\c128model.obj"
	-@erase "$(INTDIR)\c128mmu.obj"
	-@erase "$(INTDIR)\c128rom.obj"
	-@erase "$(INTDIR)\c128romset.obj"
	-@erase "$(INTDIR)\c128video.obj"
	-@erase "$(INTDIR)\daa.obj"
	-@erase "$(INTDIR)\functionrom.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\z80.obj"
	-@erase "$(INTDIR)\z80mem.obj"
	-@erase "$(OUTDIR)\c128.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64\cart" /I "..\..\..\drive" /I "..\..\..\userport" /I "..\..\..\tape" /I "..\..\..\c64" /I "..\..\..\sid" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\vdc" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c128.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c128.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c128.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c128-cmdline-options.obj" \
	"$(INTDIR)\c128-resources.obj" \
	"$(INTDIR)\c128-snapshot.obj" \
	"$(INTDIR)\c128.obj" \
	"$(INTDIR)\c128cia1.obj" \
	"$(INTDIR)\c128cpu.obj" \
	"$(INTDIR)\c128drive.obj" \
	"$(INTDIR)\c128fastiec.obj" \
	"$(INTDIR)\c128mem.obj" \
	"$(INTDIR)\c128meminit.obj" \
	"$(INTDIR)\c128memlimit.obj" \
	"$(INTDIR)\c128memrom.obj" \
	"$(INTDIR)\c128memsnapshot.obj" \
	"$(INTDIR)\c128model.obj" \
	"$(INTDIR)\c128mmu.obj" \
	"$(INTDIR)\c128rom.obj" \
	"$(INTDIR)\c128romset.obj" \
	"$(INTDIR)\c128video.obj" \
	"$(INTDIR)\daa.obj" \
	"$(INTDIR)\functionrom.obj" \
	"$(INTDIR)\z80.obj" \
	"$(INTDIR)\z80mem.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\c128.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "c128 - Win32 Debug"

OUTDIR=.\libs\c128\Debug
INTDIR=.\libs\c128\Debug
# Begin Custom Macros
OutDir=.\libs\c128\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c128.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\c128.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c128-cmdline-options.obj"
	-@erase "$(INTDIR)\c128-resources.obj"
	-@erase "$(INTDIR)\c128-snapshot.obj"
	-@erase "$(INTDIR)\c128.obj"
	-@erase "$(INTDIR)\c128cia1.obj"
	-@erase "$(INTDIR)\c128cpu.obj"
	-@erase "$(INTDIR)\c128drive.obj"
	-@erase "$(INTDIR)\c128fastiec.obj"
	-@erase "$(INTDIR)\c128mem.obj"
	-@erase "$(INTDIR)\c128meminit.obj"
	-@erase "$(INTDIR)\c128memlimit.obj"
	-@erase "$(INTDIR)\c128memrom.obj"
	-@erase "$(INTDIR)\c128memsnapshot.obj"
	-@erase "$(INTDIR)\c128model.obj"
	-@erase "$(INTDIR)\c128mmu.obj"
	-@erase "$(INTDIR)\c128rom.obj"
	-@erase "$(INTDIR)\c128romset.obj"
	-@erase "$(INTDIR)\c128video.obj"
	-@erase "$(INTDIR)\daa.obj"
	-@erase "$(INTDIR)\functionrom.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\z80.obj"
	-@erase "$(INTDIR)\z80mem.obj"
	-@erase "$(OUTDIR)\c128.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64\cart" /I "..\..\..\drive" /I "..\..\..\userport" /I "..\..\..\tape" /I "..\..\..\c64" /I "..\..\..\sid" /I "..\..\..\vicii" /I "..\..\..\raster" /I "..\..\..\vdc" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c128.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c128.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c128.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c128-cmdline-options.obj" \
	"$(INTDIR)\c128-resources.obj" \
	"$(INTDIR)\c128-snapshot.obj" \
	"$(INTDIR)\c128.obj" \
	"$(INTDIR)\c128cia1.obj" \
	"$(INTDIR)\c128cpu.obj" \
	"$(INTDIR)\c128drive.obj" \
	"$(INTDIR)\c128fastiec.obj" \
	"$(INTDIR)\c128mem.obj" \
	"$(INTDIR)\c128meminit.obj" \
	"$(INTDIR)\c128memlimit.obj" \
	"$(INTDIR)\c128memrom.obj" \
	"$(INTDIR)\c128memsnapshot.obj" \
	"$(INTDIR)\c128model.obj" \
	"$(INTDIR)\c128mmu.obj" \
	"$(INTDIR)\c128rom.obj" \
	"$(INTDIR)\c128romset.obj" \
	"$(INTDIR)\c128video.obj" \
	"$(INTDIR)\daa.obj" \
	"$(INTDIR)\functionrom.obj" \
	"$(INTDIR)\z80.obj" \
	"$(INTDIR)\z80mem.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\c128.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "c128 - Win32 Release" || "$(CFG)" == "c128 - Win32 Debug"

!IF  "$(CFG)" == "c128 - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "c128 - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\c128\c128-cmdline-options.c"

"$(INTDIR)\c128-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c128\c128-resources.c"

"$(INTDIR)\c128-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c128\c128-snapshot.c"

"$(INTDIR)\c128-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128.c

"$(INTDIR)\c128.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128cia1.c

"$(INTDIR)\c128cia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128cpu.c

"$(INTDIR)\c128cpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128drive.c

"$(INTDIR)\c128drive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128fastiec.c

"$(INTDIR)\c128fastiec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128mem.c

"$(INTDIR)\c128mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128meminit.c

"$(INTDIR)\c128meminit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128memlimit.c

"$(INTDIR)\c128memlimit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128memrom.c

"$(INTDIR)\c128memrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128memsnapshot.c

"$(INTDIR)\c128memsnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128model.c

"$(INTDIR)\c128mmu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128mmu.c

"$(INTDIR)\c128mmu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128rom.c

"$(INTDIR)\c128rom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128romset.c

"$(INTDIR)\c128romset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\c128video.c

"$(INTDIR)\c128video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\daa.c

"$(INTDIR)\daa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\functionrom.c

"$(INTDIR)\functionrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\z80.c

"$(INTDIR)\z80.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c128\z80mem.c

"$(INTDIR)\z80mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

