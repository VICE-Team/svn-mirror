# Microsoft Developer Studio Generated NMAKE File, Based on tcbm.dsp
!IF "$(CFG)" == ""
CFG=tcbm - Win32 Release
!MESSAGE No configuration specified. Defaulting to tcbm - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "tcbm - Win32 Release" && "$(CFG)" != "tcbm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tcbm.mak" CFG="tcbm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tcbm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tcbm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "tcbm - Win32 Release"

OUTDIR=.\libs\tcbm\Release
INTDIR=.\libs\tcbm\Release
# Begin Custom Macros
OutDir=.\libs\tcbm\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\tcbm.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\tcbm.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\glue1551.obj"
	-@erase "$(INTDIR)\mem1551.obj"
	-@erase "$(INTDIR)\tcbm-cmdline-options.obj"
	-@erase "$(INTDIR)\tcbm-resources.obj"
	-@erase "$(INTDIR)\tcbm.obj"
	-@erase "$(INTDIR)\tcbmrom.obj"
	-@erase "$(INTDIR)\tpid.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\tcbm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\tcbm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tcbm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tcbm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\glue1551.obj" \
	"$(INTDIR)\mem1551.obj" \
	"$(INTDIR)\tcbm-cmdline-options.obj" \
	"$(INTDIR)\tcbm-resources.obj" \
	"$(INTDIR)\tcbm.obj" \
	"$(INTDIR)\tcbmrom.obj" \
	"$(INTDIR)\tpid.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\tcbm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tcbm - Win32 Debug"

OUTDIR=.\libs\tcbm\Debug
INTDIR=.\libs\tcbm\Debug
# Begin Custom Macros
OutDir=.\libs\tcbm\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\tcbm.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\tcbm.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\glue1551.obj"
	-@erase "$(INTDIR)\mem1551.obj"
	-@erase "$(INTDIR)\tcbm-cmdline-options.obj"
	-@erase "$(INTDIR)\tcbm-resources.obj"
	-@erase "$(INTDIR)\tcbm.obj"
	-@erase "$(INTDIR)\tcbmrom.obj"
	-@erase "$(INTDIR)\tpid.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\tcbm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\tcbm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tcbm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tcbm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\glue1551.obj" \
	"$(INTDIR)\mem1551.obj" \
	"$(INTDIR)\tcbm-cmdline-options.obj" \
	"$(INTDIR)\tcbm-resources.obj" \
	"$(INTDIR)\tcbm.obj" \
	"$(INTDIR)\tcbmrom.obj" \
	"$(INTDIR)\tpid.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\tcbm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "tcbm - Win32 Release" || "$(CFG)" == "tcbm - Win32 Debug"

!IF  "$(CFG)" == "tcbm - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "tcbm - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\drive\tcbm\glue1551.c

"$(INTDIR)\glue1551.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\tcbm\mem1551.c

"$(INTDIR)\mem1551.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\tcbm\tcbm-cmdline-options.c"

"$(INTDIR)\tcbm-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\tcbm\tcbm-resources.c"

"$(INTDIR)\tcbm-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\tcbm\tcbm.c

"$(INTDIR)\tcbm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\tcbm\tcbmrom.c

"$(INTDIR)\tcbmrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\tcbm\tpid.c

"$(INTDIR)\tpid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

