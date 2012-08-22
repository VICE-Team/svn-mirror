# Microsoft Developer Studio Generated NMAKE File, Based on platform.dsp
!IF "$(CFG)" == ""
CFG=platform - Win32 Debug
!MESSAGE No configuration specified. Defaulting to platform - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "platform - Win32 Release" && "$(CFG)" != "platform - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "platform.mak" CFG="platform - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "platform - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "platform - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "platform - Win32 Release"

OUTDIR=.\libs\platform\Release
INTDIR=.\libs\platform\Release
# Begin Custom Macros
OutDir=.\libs\platform\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\platform.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\platform.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\platform_windows_runtime_os.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\platform.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "WIN32_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\rtc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\platform.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\platform.lib" 
LIB32_OBJS= \
	"$(INTDIR)\platform.obj" \
	"$(INTDIR)\platform_windows_runtime_os.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\platform.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "platform - Win32 Debug"

OUTDIR=.\libs\platform\Debug
INTDIR=.\libs\platform\Debug
# Begin Custom Macros
OutDir=.\libs\platform\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\platform.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\platform.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\platform.obj"
	-@erase "$(INTDIR)\platform_windows_runtime_os.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\platform.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "WIN32_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\rtc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\platform.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\platform.lib" 
LIB32_OBJS= \
	"$(INTDIR)\platform.obj" \
	"$(INTDIR)\platform_windows_runtime_os.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\platform.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "platform - Win32 Release" || "$(CFG)" == "platform - Win32 Debug"

!IF  "$(CFG)" == "platform - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "platform - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\platform\platform.c"

"$(INTDIR)\platform.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\platform\platform_windows_runtime_os.c"

"$(INTDIR)\platform_windows_runtime_os.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

