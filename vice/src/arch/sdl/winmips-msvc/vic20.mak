# Microsoft Developer Studio Generated NMAKE File, Based on vic20.dsp
!IF "$(CFG)" == ""
CFG=vic20 - Win32 Release
!MESSAGE No configuration specified. Defaulting to vic20 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "vic20 - Win32 Release" && "$(CFG)" != "vic20 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vic20.mak" CFG="vic20 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vic20 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vic20 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "vic20 - Win32 Release"

OUTDIR=.\libs\vic20\Release
INTDIR=.\libs\vic20\Release
# Begin Custom Macros
OutDir=.\libs\vic20\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vic20.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\vic20.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vic-cmdline-options.obj"
	-@erase "$(INTDIR)\vic-color.obj"
	-@erase "$(INTDIR)\vic-cycle.obj"
	-@erase "$(INTDIR)\vic-draw.obj"
	-@erase "$(INTDIR)\vic-mem.obj"
	-@erase "$(INTDIR)\vic-resources.obj"
	-@erase "$(INTDIR)\vic-snapshot.obj"
	-@erase "$(INTDIR)\vic-timing.obj"
	-@erase "$(INTDIR)\vic.obj"
	-@erase "$(INTDIR)\vic20-cmdline-options.obj"
	-@erase "$(INTDIR)\vic20-resources.obj"
	-@erase "$(INTDIR)\vic20-snapshot.obj"
	-@erase "$(INTDIR)\vic20.obj"
	-@erase "$(INTDIR)\vic20bus.obj"
	-@erase "$(INTDIR)\vic20cpu.obj"
	-@erase "$(INTDIR)\vic20datasette.obj"
	-@erase "$(INTDIR)\vic20drive.obj"
	-@erase "$(INTDIR)\vic20iec.obj"
	-@erase "$(INTDIR)\vic20ieeevia1.obj"
	-@erase "$(INTDIR)\vic20ieeevia2.obj"
	-@erase "$(INTDIR)\vic20io.obj"
	-@erase "$(INTDIR)\vic20mem.obj"
	-@erase "$(INTDIR)\vic20memrom.obj"
	-@erase "$(INTDIR)\vic20memsnapshot.obj"
	-@erase "$(INTDIR)\vic20printer.obj"
	-@erase "$(INTDIR)\vic20rom.obj"
	-@erase "$(INTDIR)\vic20romset.obj"
	-@erase "$(INTDIR)\vic20rsuser.obj"
	-@erase "$(INTDIR)\vic20sound.obj"
	-@erase "$(INTDIR)\vic20via1.obj"
	-@erase "$(INTDIR)\vic20via2.obj"
	-@erase "$(INTDIR)\vic20video.obj"
	-@erase "$(OUTDIR)\vic20.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /I "..\..\..\vic20\cart" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vic20.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vic20.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vic20.lib" 
LIB32_OBJS= \
	"$(INTDIR)\vic-cmdline-options.obj" \
	"$(INTDIR)\vic-color.obj" \
	"$(INTDIR)\vic-cycle.obj" \
	"$(INTDIR)\vic-draw.obj" \
	"$(INTDIR)\vic-mem.obj" \
	"$(INTDIR)\vic-resources.obj" \
	"$(INTDIR)\vic-snapshot.obj" \
	"$(INTDIR)\vic-timing.obj" \
	"$(INTDIR)\vic.obj" \
	"$(INTDIR)\vic20-cmdline-options.obj" \
	"$(INTDIR)\vic20-resources.obj" \
	"$(INTDIR)\vic20-snapshot.obj" \
	"$(INTDIR)\vic20.obj" \
	"$(INTDIR)\vic20bus.obj" \
	"$(INTDIR)\vic20cpu.obj" \
	"$(INTDIR)\vic20datasette.obj" \
	"$(INTDIR)\vic20drive.obj" \
	"$(INTDIR)\vic20iec.obj" \
	"$(INTDIR)\vic20ieeevia1.obj" \
	"$(INTDIR)\vic20ieeevia2.obj" \
	"$(INTDIR)\vic20io.obj" \
	"$(INTDIR)\vic20mem.obj" \
	"$(INTDIR)\vic20memrom.obj" \
	"$(INTDIR)\vic20memsnapshot.obj" \
	"$(INTDIR)\vic20printer.obj" \
	"$(INTDIR)\vic20rom.obj" \
	"$(INTDIR)\vic20romset.obj" \
	"$(INTDIR)\vic20rsuser.obj" \
	"$(INTDIR)\vic20sound.obj" \
	"$(INTDIR)\vic20via1.obj" \
	"$(INTDIR)\vic20via2.obj" \
	"$(INTDIR)\vic20video.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\vic20.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vic20 - Win32 Debug"

OUTDIR=.\libs\vic20\Debug
INTDIR=.\libs\vic20\Debug
# Begin Custom Macros
OutDir=.\libs\vic20\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vic20.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\vic20.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vic-cmdline-options.obj"
	-@erase "$(INTDIR)\vic-color.obj"
	-@erase "$(INTDIR)\vic-cycle.obj"
	-@erase "$(INTDIR)\vic-draw.obj"
	-@erase "$(INTDIR)\vic-mem.obj"
	-@erase "$(INTDIR)\vic-resources.obj"
	-@erase "$(INTDIR)\vic-snapshot.obj"
	-@erase "$(INTDIR)\vic-timing.obj"
	-@erase "$(INTDIR)\vic.obj"
	-@erase "$(INTDIR)\vic20-cmdline-options.obj"
	-@erase "$(INTDIR)\vic20-resources.obj"
	-@erase "$(INTDIR)\vic20-snapshot.obj"
	-@erase "$(INTDIR)\vic20.obj"
	-@erase "$(INTDIR)\vic20bus.obj"
	-@erase "$(INTDIR)\vic20cpu.obj"
	-@erase "$(INTDIR)\vic20datasette.obj"
	-@erase "$(INTDIR)\vic20drive.obj"
	-@erase "$(INTDIR)\vic20iec.obj"
	-@erase "$(INTDIR)\vic20ieeevia1.obj"
	-@erase "$(INTDIR)\vic20ieeevia2.obj"
	-@erase "$(INTDIR)\vic20io.obj"
	-@erase "$(INTDIR)\vic20mem.obj"
	-@erase "$(INTDIR)\vic20memrom.obj"
	-@erase "$(INTDIR)\vic20memsnapshot.obj"
	-@erase "$(INTDIR)\vic20printer.obj"
	-@erase "$(INTDIR)\vic20rom.obj"
	-@erase "$(INTDIR)\vic20romset.obj"
	-@erase "$(INTDIR)\vic20rsuser.obj"
	-@erase "$(INTDIR)\vic20sound.obj"
	-@erase "$(INTDIR)\vic20via1.obj"
	-@erase "$(INTDIR)\vic20via2.obj"
	-@erase "$(INTDIR)\vic20video.obj"
	-@erase "$(OUTDIR)\vic20.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /I "..\..\..\vic20\cart" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vic20.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vic20.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vic20.lib" 
LIB32_OBJS= \
	"$(INTDIR)\vic-cmdline-options.obj" \
	"$(INTDIR)\vic-color.obj" \
	"$(INTDIR)\vic-cycle.obj" \
	"$(INTDIR)\vic-draw.obj" \
	"$(INTDIR)\vic-mem.obj" \
	"$(INTDIR)\vic-resources.obj" \
	"$(INTDIR)\vic-snapshot.obj" \
	"$(INTDIR)\vic-timing.obj" \
	"$(INTDIR)\vic.obj" \
	"$(INTDIR)\vic20-cmdline-options.obj" \
	"$(INTDIR)\vic20-resources.obj" \
	"$(INTDIR)\vic20-snapshot.obj" \
	"$(INTDIR)\vic20.obj" \
	"$(INTDIR)\vic20bus.obj" \
	"$(INTDIR)\vic20cpu.obj" \
	"$(INTDIR)\vic20datasette.obj" \
	"$(INTDIR)\vic20drive.obj" \
	"$(INTDIR)\vic20iec.obj" \
	"$(INTDIR)\vic20ieeevia1.obj" \
	"$(INTDIR)\vic20ieeevia2.obj" \
	"$(INTDIR)\vic20io.obj" \
	"$(INTDIR)\vic20mem.obj" \
	"$(INTDIR)\vic20memrom.obj" \
	"$(INTDIR)\vic20memsnapshot.obj" \
	"$(INTDIR)\vic20printer.obj" \
	"$(INTDIR)\vic20rom.obj" \
	"$(INTDIR)\vic20romset.obj" \
	"$(INTDIR)\vic20rsuser.obj" \
	"$(INTDIR)\vic20sound.obj" \
	"$(INTDIR)\vic20via1.obj" \
	"$(INTDIR)\vic20via2.obj" \
	"$(INTDIR)\vic20video.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\vic20.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "vic20 - Win32 Release" || "$(CFG)" == "vic20 - Win32 Debug"

!IF  "$(CFG)" == "vic20 - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vic20 - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\vic20\vic-cmdline-options.c"

"$(INTDIR)\vic-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic-color.c"

"$(INTDIR)\vic-color.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic-cycle.c"

"$(INTDIR)\vic-cycle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic-draw.c"

"$(INTDIR)\vic-draw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic-mem.c"

"$(INTDIR)\vic-mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic-resources.c"

"$(INTDIR)\vic-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic-snapshot.c"

"$(INTDIR)\vic-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic-timing.c"

"$(INTDIR)\vic-timing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic.c

"$(INTDIR)\vic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic20-cmdline-options.c"

"$(INTDIR)\vic20-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic20-resources.c"

"$(INTDIR)\vic20-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\vic20-snapshot.c"

"$(INTDIR)\vic20-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20.c

"$(INTDIR)\vic20.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20bus.c

"$(INTDIR)\vic20bus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20cpu.c

"$(INTDIR)\vic20cpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20datasette.c

"$(INTDIR)\vic20datasette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20drive.c

"$(INTDIR)\vic20drive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20iec.c

"$(INTDIR)\vic20iec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20ieeevia1.c

"$(INTDIR)\vic20ieeevia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20ieeevia2.c

"$(INTDIR)\vic20ieeevia2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20io.c

"$(INTDIR)\vic20io.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20mem.c

"$(INTDIR)\vic20mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20memrom.c

"$(INTDIR)\vic20memrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20memsnapshot.c

"$(INTDIR)\vic20memsnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20printer.c

"$(INTDIR)\vic20printer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20rom.c

"$(INTDIR)\vic20rom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20romset.c

"$(INTDIR)\vic20romset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20rsuser.c

"$(INTDIR)\vic20rsuser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20sound.c

"$(INTDIR)\vic20sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20via1.c

"$(INTDIR)\vic20via1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20via2.c

"$(INTDIR)\vic20via2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vic20\vic20video.c

"$(INTDIR)\vic20video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

