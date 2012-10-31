# Microsoft Developer Studio Generated NMAKE File, Based on vicii.dsp
!IF "$(CFG)" == ""
CFG=vicii - Win32 Release
!MESSAGE No configuration specified. Defaulting to vicii - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "vicii - Win32 Release" && "$(CFG)" != "vicii - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vicii.mak" CFG="vicii - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vicii - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vicii - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "vicii - Win32 Release"

OUTDIR=.\libs\vicii\Release
INTDIR=.\libs\vicii\Release
# Begin Custom Macros
OutDir=.\libs\vicii\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vicii.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\vicii.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vicii-badline.obj"
	-@erase "$(INTDIR)\vicii-clock-stretch.obj"
	-@erase "$(INTDIR)\vicii-cmdline-options.obj"
	-@erase "$(INTDIR)\vicii-color.obj"
	-@erase "$(INTDIR)\vicii-draw.obj"
	-@erase "$(INTDIR)\vicii-fetch.obj"
	-@erase "$(INTDIR)\vicii-irq.obj"
	-@erase "$(INTDIR)\vicii-mem.obj"
	-@erase "$(INTDIR)\vicii-phi1.obj"
	-@erase "$(INTDIR)\vicii-resources.obj"
	-@erase "$(INTDIR)\vicii-snapshot.obj"
	-@erase "$(INTDIR)\vicii-sprites.obj"
	-@erase "$(INTDIR)\vicii-stubs.obj"
	-@erase "$(INTDIR)\vicii-timing.obj"
	-@erase "$(INTDIR)\vicii.obj"
	-@erase "$(OUTDIR)\vicii.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\c64dtv" /I "..\..\..\raster" /I "..\..\..\video" /I "..\..\..\monitor" /I "..\..\..\c64\cart" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vicii.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vicii.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vicii.lib" 
LIB32_OBJS= \
	"$(INTDIR)\vicii-badline.obj" \
	"$(INTDIR)\vicii-clock-stretch.obj" \
	"$(INTDIR)\vicii-cmdline-options.obj" \
	"$(INTDIR)\vicii-color.obj" \
	"$(INTDIR)\vicii-draw.obj" \
	"$(INTDIR)\vicii-fetch.obj" \
	"$(INTDIR)\vicii-irq.obj" \
	"$(INTDIR)\vicii-mem.obj" \
	"$(INTDIR)\vicii-phi1.obj" \
	"$(INTDIR)\vicii-resources.obj" \
	"$(INTDIR)\vicii-snapshot.obj" \
	"$(INTDIR)\vicii-sprites.obj" \
	"$(INTDIR)\vicii-stubs.obj" \
	"$(INTDIR)\vicii-timing.obj" \
	"$(INTDIR)\vicii.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\vicii.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vicii - Win32 Debug"

OUTDIR=.\libs\vicii\Debug
INTDIR=.\libs\vicii\Debug
# Begin Custom Macros
OutDir=.\libs\vicii\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vicii.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\vicii.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vicii-badline.obj"
	-@erase "$(INTDIR)\vicii-clock-stretch.obj"
	-@erase "$(INTDIR)\vicii-cmdline-options.obj"
	-@erase "$(INTDIR)\vicii-color.obj"
	-@erase "$(INTDIR)\vicii-draw.obj"
	-@erase "$(INTDIR)\vicii-fetch.obj"
	-@erase "$(INTDIR)\vicii-irq.obj"
	-@erase "$(INTDIR)\vicii-mem.obj"
	-@erase "$(INTDIR)\vicii-phi1.obj"
	-@erase "$(INTDIR)\vicii-resources.obj"
	-@erase "$(INTDIR)\vicii-snapshot.obj"
	-@erase "$(INTDIR)\vicii-sprites.obj"
	-@erase "$(INTDIR)\vicii-stubs.obj"
	-@erase "$(INTDIR)\vicii-timing.obj"
	-@erase "$(INTDIR)\vicii.obj"
	-@erase "$(OUTDIR)\vicii.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\c64dtv" /I "..\..\..\raster" /I "..\..\..\video" /I "..\..\..\monitor" /I "..\..\..\c64\cart" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vicii.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vicii.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vicii.lib" 
LIB32_OBJS= \
	"$(INTDIR)\vicii-badline.obj" \
	"$(INTDIR)\vicii-clock-stretch.obj" \
	"$(INTDIR)\vicii-cmdline-options.obj" \
	"$(INTDIR)\vicii-color.obj" \
	"$(INTDIR)\vicii-draw.obj" \
	"$(INTDIR)\vicii-fetch.obj" \
	"$(INTDIR)\vicii-irq.obj" \
	"$(INTDIR)\vicii-mem.obj" \
	"$(INTDIR)\vicii-phi1.obj" \
	"$(INTDIR)\vicii-resources.obj" \
	"$(INTDIR)\vicii-snapshot.obj" \
	"$(INTDIR)\vicii-sprites.obj" \
	"$(INTDIR)\vicii-stubs.obj" \
	"$(INTDIR)\vicii-timing.obj" \
	"$(INTDIR)\vicii.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\vicii.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "vicii - Win32 Release" || "$(CFG)" == "vicii - Win32 Debug"

!IF  "$(CFG)" == "vicii - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vicii - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\vicii\vicii-badline.c"

"$(INTDIR)\vicii-badline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-clock-stretch.c"

"$(INTDIR)\vicii-clock-stretch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-cmdline-options.c"

"$(INTDIR)\vicii-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-color.c"

"$(INTDIR)\vicii-color.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-draw.c"

"$(INTDIR)\vicii-draw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-fetch.c"

"$(INTDIR)\vicii-fetch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-irq.c"

"$(INTDIR)\vicii-irq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-mem.c"

"$(INTDIR)\vicii-mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-phi1.c"

"$(INTDIR)\vicii-phi1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-resources.c"

"$(INTDIR)\vicii-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-snapshot.c"

"$(INTDIR)\vicii-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-sprites.c"

"$(INTDIR)\vicii-sprites.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-stubs.c"

"$(INTDIR)\vicii-stubs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-timing.c"

"$(INTDIR)\vicii-timing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vicii\vicii.c

"$(INTDIR)\vicii.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

