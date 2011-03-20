# Microsoft Developer Studio Generated NMAKE File, Based on fsdevice.dsp
!IF "$(CFG)" == ""
CFG=fsdevice - Win32 Debug
!MESSAGE No configuration specified. Defaulting to fsdevice - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "fsdevice - Win32 Release" && "$(CFG)" != "fsdevice - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fsdevice.mak" CFG="fsdevice - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fsdevice - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fsdevice - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "fsdevice - Win32 Release"

OUTDIR=.\libs\fsdevice\Release
INTDIR=.\libs\fsdevice\Release
# Begin Custom Macros
OutDir=.\libs\fsdevice\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\fsdevice.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\fsdevice.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\fsdevice-close.obj"
	-@erase "$(INTDIR)\fsdevice-cmdline-options.obj"
	-@erase "$(INTDIR)\fsdevice-flush.obj"
	-@erase "$(INTDIR)\fsdevice-open.obj"
	-@erase "$(INTDIR)\fsdevice-read.obj"
	-@erase "$(INTDIR)\fsdevice-resources.obj"
	-@erase "$(INTDIR)\fsdevice-write.obj"
	-@erase "$(INTDIR)\fsdevice.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\fsdevice.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\fsdevice" /I "..\..\..\vdrive" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\fsdevice.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fsdevice.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\fsdevice.lib" 
LIB32_OBJS= \
	"$(INTDIR)\fsdevice-close.obj" \
	"$(INTDIR)\fsdevice-cmdline-options.obj" \
	"$(INTDIR)\fsdevice-flush.obj" \
	"$(INTDIR)\fsdevice-open.obj" \
	"$(INTDIR)\fsdevice-read.obj" \
	"$(INTDIR)\fsdevice-resources.obj" \
	"$(INTDIR)\fsdevice-write.obj" \
	"$(INTDIR)\fsdevice.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\fsdevice.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "fsdevice - Win32 Debug"

OUTDIR=.\libs\fsdevice\Debug
INTDIR=.\libs\fsdevice\Debug
# Begin Custom Macros
OutDir=.\libs\fsdevice\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\fsdevice.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\fsdevice.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\fsdevice-close.obj"
	-@erase "$(INTDIR)\fsdevice-cmdline-options.obj"
	-@erase "$(INTDIR)\fsdevice-flush.obj"
	-@erase "$(INTDIR)\fsdevice-open.obj"
	-@erase "$(INTDIR)\fsdevice-read.obj"
	-@erase "$(INTDIR)\fsdevice-resources.obj"
	-@erase "$(INTDIR)\fsdevice-write.obj"
	-@erase "$(INTDIR)\fsdevice.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\fsdevice.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\fsdevice" /I "..\..\..\vdrive" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\fsdevice.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fsdevice.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\fsdevice.lib" 
LIB32_OBJS= \
	"$(INTDIR)\fsdevice-close.obj" \
	"$(INTDIR)\fsdevice-cmdline-options.obj" \
	"$(INTDIR)\fsdevice-flush.obj" \
	"$(INTDIR)\fsdevice-open.obj" \
	"$(INTDIR)\fsdevice-read.obj" \
	"$(INTDIR)\fsdevice-resources.obj" \
	"$(INTDIR)\fsdevice-write.obj" \
	"$(INTDIR)\fsdevice.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\fsdevice.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "fsdevice - Win32 Release" || "$(CFG)" == "fsdevice - Win32 Debug"

!IF  "$(CFG)" == "fsdevice - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "fsdevice - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\fsdevice\fsdevice-close.c"

"$(INTDIR)\fsdevice-close.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\fsdevice\fsdevice-cmdline-options.c"

"$(INTDIR)\fsdevice-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\fsdevice\fsdevice-flush.c"

"$(INTDIR)\fsdevice-flush.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\fsdevice\fsdevice-open.c"

"$(INTDIR)\fsdevice-open.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\fsdevice\fsdevice-read.c"

"$(INTDIR)\fsdevice-read.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\fsdevice\fsdevice-resources.c"

"$(INTDIR)\fsdevice-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\fsdevice\fsdevice-write.c"

"$(INTDIR)\fsdevice-write.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\fsdevice\fsdevice.c"

"$(INTDIR)\fsdevice.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

