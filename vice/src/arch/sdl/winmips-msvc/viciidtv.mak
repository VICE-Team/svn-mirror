# Microsoft Developer Studio Generated NMAKE File, Based on viciidtv.dsp
!IF "$(CFG)" == ""
CFG=viciidtv - Win32 Release
!MESSAGE No configuration specified. Defaulting to viciidtv - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "viciidtv - Win32 Release" && "$(CFG)" != "viciidtv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "viciidtv.mak" CFG="viciidtv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "viciidtv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "viciidtv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "viciidtv - Win32 Release"

OUTDIR=.\libs\viciidtv\Release
INTDIR=.\libs\viciidtv\Release
# Begin Custom Macros
OutDir=.\libs\viciidtv\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\viciidtv.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\viciidtv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vicii-badline.obj"
	-@erase "$(INTDIR)\vicii-cmdline-options.obj"
	-@erase "$(INTDIR)\vicii-fetch.obj"
	-@erase "$(INTDIR)\vicii-irq.obj"
	-@erase "$(INTDIR)\vicii-mem.obj"
	-@erase "$(INTDIR)\vicii-phi1.obj"
	-@erase "$(INTDIR)\vicii-resources.obj"
	-@erase "$(INTDIR)\vicii-sprites.obj"
	-@erase "$(INTDIR)\vicii-timing.obj"
	-@erase "$(INTDIR)\vicii.obj"
	-@erase "$(INTDIR)\viciidtv-color.obj"
	-@erase "$(INTDIR)\viciidtv-draw.obj"
	-@erase "$(INTDIR)\viciidtv-snapshot.obj"
	-@erase "$(OUTDIR)\viciidtv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\c64dtv" /I "..\..\..\raster" /I "..\..\..\video" /I "..\..\..\monitor" /I "..\..\..\c64\cart" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\viciidtv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\viciidtv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\viciidtv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\vicii-badline.obj" \
	"$(INTDIR)\vicii-cmdline-options.obj" \
	"$(INTDIR)\vicii-fetch.obj" \
	"$(INTDIR)\vicii-irq.obj" \
	"$(INTDIR)\vicii-mem.obj" \
	"$(INTDIR)\vicii-phi1.obj" \
	"$(INTDIR)\vicii-resources.obj" \
	"$(INTDIR)\vicii-sprites.obj" \
	"$(INTDIR)\vicii-timing.obj" \
	"$(INTDIR)\vicii.obj" \
	"$(INTDIR)\viciidtv-color.obj" \
	"$(INTDIR)\viciidtv-draw.obj" \
	"$(INTDIR)\viciidtv-snapshot.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\viciidtv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "viciidtv - Win32 Debug"

OUTDIR=.\libs\viciidtv\Debug
INTDIR=.\libs\viciidtv\Debug
# Begin Custom Macros
OutDir=.\libs\viciidtv\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\viciidtv.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\viciidtv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vicii-badline.obj"
	-@erase "$(INTDIR)\vicii-cmdline-options.obj"
	-@erase "$(INTDIR)\vicii-fetch.obj"
	-@erase "$(INTDIR)\vicii-irq.obj"
	-@erase "$(INTDIR)\vicii-mem.obj"
	-@erase "$(INTDIR)\vicii-phi1.obj"
	-@erase "$(INTDIR)\vicii-resources.obj"
	-@erase "$(INTDIR)\vicii-sprites.obj"
	-@erase "$(INTDIR)\vicii-timing.obj"
	-@erase "$(INTDIR)\vicii.obj"
	-@erase "$(INTDIR)\viciidtv-color.obj"
	-@erase "$(INTDIR)\viciidtv-draw.obj"
	-@erase "$(INTDIR)\viciidtv-snapshot.obj"
	-@erase "$(OUTDIR)\viciidtv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\c64dtv" /I "..\..\..\raster" /I "..\..\..\video" /I "..\..\..\monitor" /I "..\..\..\c64\cart" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\viciidtv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\viciidtv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\viciidtv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\vicii-badline.obj" \
	"$(INTDIR)\vicii-cmdline-options.obj" \
	"$(INTDIR)\vicii-fetch.obj" \
	"$(INTDIR)\vicii-irq.obj" \
	"$(INTDIR)\vicii-mem.obj" \
	"$(INTDIR)\vicii-phi1.obj" \
	"$(INTDIR)\vicii-resources.obj" \
	"$(INTDIR)\vicii-sprites.obj" \
	"$(INTDIR)\vicii-timing.obj" \
	"$(INTDIR)\vicii.obj" \
	"$(INTDIR)\viciidtv-color.obj" \
	"$(INTDIR)\viciidtv-draw.obj" \
	"$(INTDIR)\viciidtv-snapshot.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\viciidtv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "viciidtv - Win32 Release" || "$(CFG)" == "viciidtv - Win32 Debug"

!IF  "$(CFG)" == "viciidtv - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "viciidtv - Win32 Debug"

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


SOURCE="..\..\..\vicii\vicii-cmdline-options.c"

"$(INTDIR)\vicii-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
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


SOURCE="..\..\..\vicii\vicii-sprites.c"

"$(INTDIR)\vicii-sprites.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\vicii-timing.c"

"$(INTDIR)\vicii-timing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\vicii\vicii.c

"$(INTDIR)\vicii.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\viciidtv-color.c"

"$(INTDIR)\viciidtv-color.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\viciidtv-draw.c"

"$(INTDIR)\viciidtv-draw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vicii\viciidtv-snapshot.c"

"$(INTDIR)\viciidtv-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

