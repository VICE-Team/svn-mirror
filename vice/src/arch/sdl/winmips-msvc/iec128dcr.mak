# Microsoft Developer Studio Generated NMAKE File, Based on iec128dcr.dsp
!IF "$(CFG)" == ""
CFG=iec128dcr - Win32 Debug
!MESSAGE No configuration specified. Defaulting to iec128dcr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "iec128dcr - Win32 Release" && "$(CFG)" != "iec128dcr - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iec128dcr.mak" CFG="iec128dcr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iec128dcr - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "iec128dcr - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "iec128dcr - Win32 Release"

OUTDIR=.\libs\iec128dcr\Release
INTDIR=.\libs\iec128dcr\Release
# Begin Custom Macros
OutDir=.\libs\iec128dcr\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\iec128dcr.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\iec128dcr.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\iec128dcr-cmdline-options.obj"
	-@erase "$(INTDIR)\iec128dcr-resources.obj"
	-@erase "$(INTDIR)\iec128dcr.obj"
	-@erase "$(INTDIR)\iec128dcrrom.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\iec128dcr.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\iec128dcr.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iec128dcr.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\iec128dcr.lib" 
LIB32_OBJS= \
	"$(INTDIR)\iec128dcr-cmdline-options.obj" \
	"$(INTDIR)\iec128dcr-resources.obj" \
	"$(INTDIR)\iec128dcr.obj" \
	"$(INTDIR)\iec128dcrrom.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\iec128dcr.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "iec128dcr - Win32 Debug"

OUTDIR=.\libs\iec128dcr\Debug
INTDIR=.\libs\iec128dcr\Debug
# Begin Custom Macros
OutDir=.\libs\iec128dcr\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\iec128dcr.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\iec128dcr.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\iec128dcr-cmdline-options.obj"
	-@erase "$(INTDIR)\iec128dcr-resources.obj"
	-@erase "$(INTDIR)\iec128dcr.obj"
	-@erase "$(INTDIR)\iec128dcrrom.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\iec128dcr.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\iec128dcr.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iec128dcr.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\iec128dcr.lib" 
LIB32_OBJS= \
	"$(INTDIR)\iec128dcr-cmdline-options.obj" \
	"$(INTDIR)\iec128dcr-resources.obj" \
	"$(INTDIR)\iec128dcr.obj" \
	"$(INTDIR)\iec128dcrrom.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\iec128dcr.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "iec128dcr - Win32 Release" || "$(CFG)" == "iec128dcr - Win32 Debug"

!IF  "$(CFG)" == "iec128dcr - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "iec128dcr - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\drive\iec128dcr\iec128dcr-cmdline-options.c"

"$(INTDIR)\iec128dcr-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\iec128dcr\iec128dcr-resources.c"

"$(INTDIR)\iec128dcr-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec128dcr\iec128dcr.c

"$(INTDIR)\iec128dcr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec128dcr\iec128dcrrom.c

"$(INTDIR)\iec128dcrrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

