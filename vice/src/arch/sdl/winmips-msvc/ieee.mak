# Microsoft Developer Studio Generated NMAKE File, Based on ieee.dsp
!IF "$(CFG)" == ""
CFG=ieee - Win32 Release
!MESSAGE No configuration specified. Defaulting to ieee - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "ieee - Win32 Release" && "$(CFG)" != "ieee - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ieee.mak" CFG="ieee - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ieee - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ieee - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ieee - Win32 Release"

OUTDIR=.\libs\ieee\Release
INTDIR=.\libs\ieee\Release
# Begin Custom Macros
OutDir=.\libs\ieee\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ieee.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\ieee.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\fdc.obj"
	-@erase "$(INTDIR)\ieee-cmdline-options.obj"
	-@erase "$(INTDIR)\ieee-resources.obj"
	-@erase "$(INTDIR)\ieee.obj"
	-@erase "$(INTDIR)\ieeerom.obj"
	-@erase "$(INTDIR)\memieee.obj"
	-@erase "$(INTDIR)\riot1d.obj"
	-@erase "$(INTDIR)\riot2d.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\via1d2031.obj"
	-@erase "$(OUTDIR)\ieee.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ieee.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ieee.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\ieee.lib" 
LIB32_OBJS= \
	"$(INTDIR)\fdc.obj" \
	"$(INTDIR)\ieee-cmdline-options.obj" \
	"$(INTDIR)\ieee-resources.obj" \
	"$(INTDIR)\ieee.obj" \
	"$(INTDIR)\ieeerom.obj" \
	"$(INTDIR)\memieee.obj" \
	"$(INTDIR)\riot1d.obj" \
	"$(INTDIR)\riot2d.obj" \
	"$(INTDIR)\via1d2031.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\ieee.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ieee - Win32 Debug"

OUTDIR=.\libs\ieee\Debug
INTDIR=.\libs\ieee\Debug
# Begin Custom Macros
OutDir=.\libs\ieee\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ieee.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\ieee.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\fdc.obj"
	-@erase "$(INTDIR)\ieee-cmdline-options.obj"
	-@erase "$(INTDIR)\ieee-resources.obj"
	-@erase "$(INTDIR)\ieee.obj"
	-@erase "$(INTDIR)\ieeerom.obj"
	-@erase "$(INTDIR)\memieee.obj"
	-@erase "$(INTDIR)\riot1d.obj"
	-@erase "$(INTDIR)\riot2d.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\via1d2031.obj"
	-@erase "$(OUTDIR)\ieee.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ieee.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ieee.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\ieee.lib" 
LIB32_OBJS= \
	"$(INTDIR)\fdc.obj" \
	"$(INTDIR)\ieee-cmdline-options.obj" \
	"$(INTDIR)\ieee-resources.obj" \
	"$(INTDIR)\ieee.obj" \
	"$(INTDIR)\ieeerom.obj" \
	"$(INTDIR)\memieee.obj" \
	"$(INTDIR)\riot1d.obj" \
	"$(INTDIR)\riot2d.obj" \
	"$(INTDIR)\via1d2031.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\ieee.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "ieee - Win32 Release" || "$(CFG)" == "ieee - Win32 Debug"

!IF  "$(CFG)" == "ieee - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "ieee - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\drive\ieee\fdc.c

"$(INTDIR)\fdc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\ieee\ieee-cmdline-options.c"

"$(INTDIR)\ieee-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\ieee\ieee-resources.c"

"$(INTDIR)\ieee-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\ieee\ieee.c

"$(INTDIR)\ieee.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\ieee\ieeerom.c

"$(INTDIR)\ieeerom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\ieee\memieee.c

"$(INTDIR)\memieee.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\ieee\riot1d.c

"$(INTDIR)\riot1d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\ieee\riot2d.c

"$(INTDIR)\riot2d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\ieee\via1d2031.c

"$(INTDIR)\via1d2031.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

