# Microsoft Developer Studio Generated NMAKE File, Based on scpu64.dsp
!IF "$(CFG)" == ""
CFG=scpu64 - Win32 Release
!MESSAGE No configuration specified. Defaulting to scpu64 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "scpu64 - Win32 Release" && "$(CFG)" != "scpu64 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scpu64.mak" CFG="scpu64 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scpu64 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "scpu64 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "scpu64 - Win32 Release"

OUTDIR=.\libs\scpu64\Release
INTDIR=.\libs\scpu64\Release
# Begin Custom Macros
OutDir=.\libs\scpu64\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\scpu64.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\scpu64.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\scpu64.obj"
	-@erase "$(INTDIR)\scpu64-cmdline-options.obj"
	-@erase "$(INTDIR)\scpu64cpu.obj"
	-@erase "$(INTDIR)\scpu64gluelogic.obj"
	-@erase "$(INTDIR)\scpu64mem.obj"
	-@erase "$(INTDIR)\scpu64meminit.obj"
	-@erase "$(INTDIR)\scpu64memsnapshot.obj"
	-@erase "$(INTDIR)\scpu64-resources.obj"
	-@erase "$(INTDIR)\scpu64rom.obj"
	-@erase "$(INTDIR)\scpu64-snapshot.obj"
	-@erase "$(INTDIR)\scpu64stubs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\scpu64.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\c64\cart" /I "..\..\..\drive" /I "..\..\..\lib\p64" /I "..\..\..\drive\iec\c64exp" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\viciisc" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\rs232drv" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\scpu64.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scpu64.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\scpu64.lib" 
LIB32_OBJS= \
	"$(INTDIR)\scpu64.obj" \
	"$(INTDIR)\scpu64-cmdline-options.obj" \
	"$(INTDIR)\scpu64cpu.obj" \
	"$(INTDIR)\scpu64gluelogic.obj" \
	"$(INTDIR)\scpu64mem.obj" \
	"$(INTDIR)\scpu64meminit.obj" \
	"$(INTDIR)\scpu64memsnapshot.obj" \
	"$(INTDIR)\scpu64-resources.obj" \
	"$(INTDIR)\scpu64rom.obj" \
	"$(INTDIR)\scpu64-snapshot.obj" \
	"$(INTDIR)\scpu64stubs.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\scpu64.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "scpu64 - Win32 Debug"

OUTDIR=.\libs\scpu64\Debug
INTDIR=.\libs\scpu64\Debug
# Begin Custom Macros
OutDir=.\libs\scpu64\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\scpu64.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\scpu64.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\scpu64.obj"
	-@erase "$(INTDIR)\scpu64-cmdline-options.obj"
	-@erase "$(INTDIR)\scpu64cpu.obj"
	-@erase "$(INTDIR)\scpu64gluelogic.obj"
	-@erase "$(INTDIR)\scpu64mem.obj"
	-@erase "$(INTDIR)\scpu64meminit.obj"
	-@erase "$(INTDIR)\scpu64memsnapshot.obj"
	-@erase "$(INTDIR)\scpu64-resources.obj"
	-@erase "$(INTDIR)\scpu64rom.obj"
	-@erase "$(INTDIR)\scpu64-snapshot.obj"
	-@erase "$(INTDIR)\scpu64stubs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\scpu64.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\c64\cart" /I "..\..\..\drive" /I "..\..\..\lib\p64" /I "..\..\..\drive\iec\c64exp" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\viciisc" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\rs232drv" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\scpu64.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\scpu64.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\scpu64.lib" 
LIB32_OBJS= \
	"$(INTDIR)\scpu64.obj" \
	"$(INTDIR)\scpu64-cmdline-options.obj" \
	"$(INTDIR)\scpu64cpu.obj" \
	"$(INTDIR)\scpu64gluelogic.obj" \
	"$(INTDIR)\scpu64mem.obj" \
	"$(INTDIR)\scpu64meminit.obj" \
	"$(INTDIR)\scpu64memsnapshot.obj" \
	"$(INTDIR)\scpu64-resources.obj" \
	"$(INTDIR)\scpu64rom.obj" \
	"$(INTDIR)\scpu64-snapshot.obj" \
	"$(INTDIR)\scpu64stubs.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\scpu64.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "scpu64 - Win32 Release" || "$(CFG)" == "scpu64 - Win32 Debug"

!IF  "$(CFG)" == "scpu64 - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "scpu64 - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\scpu64\scpu64.c"

"$(INTDIR)\scpu64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64-cmdline-options.c"

"$(INTDIR)\scpu64-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64cpu.c"

"$(INTDIR)\scpu64cpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64gluelogic.c"

"$(INTDIR)\scpu64gluelogic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64mem.c"

"$(INTDIR)\scpu64mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64meminit.c"

"$(INTDIR)\scpu64meminit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64memsnapshot.c"

"$(INTDIR)\scpu64memsnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64-resources.c"

"$(INTDIR)\scpu64-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64rom.c"

"$(INTDIR)\scpu64rom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64-snapshot.c"

"$(INTDIR)\scpu64-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\scpu64\scpu64stubs.c"

"$(INTDIR)\scpu64stubs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

