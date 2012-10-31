# Microsoft Developer Studio Generated NMAKE File, Based on iec.dsp
!IF "$(CFG)" == ""
CFG=iec - Win32 Release
!MESSAGE No configuration specified. Defaulting to iec - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "iec - Win32 Release" && "$(CFG)" != "iec - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iec.mak" CFG="iec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iec - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "iec - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "iec - Win32 Release"

OUTDIR=.\libs\iec\Release
INTDIR=.\libs\iec\Release
# Begin Custom Macros
OutDir=.\libs\iec\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\iec.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\iec.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cia1571d.obj"
	-@erase "$(INTDIR)\cia1581d.obj"
	-@erase "$(INTDIR)\fdd.obj"
	-@erase "$(INTDIR)\glue1571.obj"
	-@erase "$(INTDIR)\iec-cmdline-options.obj"
	-@erase "$(INTDIR)\iec-resources.obj"
	-@erase "$(INTDIR)\iec.obj"
	-@erase "$(INTDIR)\iecrom.obj"
	-@erase "$(INTDIR)\memiec.obj"
	-@erase "$(INTDIR)\pc8477.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\via1d1541.obj"
	-@erase "$(INTDIR)\via4000.obj"
	-@erase "$(INTDIR)\wd1770.obj"
	-@erase "$(OUTDIR)\iec.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\iec.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iec.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\iec.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cia1571d.obj" \
	"$(INTDIR)\cia1581d.obj" \
	"$(INTDIR)\fdd.obj" \
	"$(INTDIR)\glue1571.obj" \
	"$(INTDIR)\iec-cmdline-options.obj" \
	"$(INTDIR)\iec-resources.obj" \
	"$(INTDIR)\iec.obj" \
	"$(INTDIR)\iecrom.obj" \
	"$(INTDIR)\memiec.obj" \
	"$(INTDIR)\pc8477.obj" \
	"$(INTDIR)\via1d1541.obj" \
	"$(INTDIR)\via4000.obj" \
	"$(INTDIR)\wd1770.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\iec.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "iec - Win32 Debug"

OUTDIR=.\libs\iec\Debug
INTDIR=.\libs\iec\Debug
# Begin Custom Macros
OutDir=.\libs\iec\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\iec.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\iec.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cia1571d.obj"
	-@erase "$(INTDIR)\cia1581d.obj"
	-@erase "$(INTDIR)\fdd.obj"
	-@erase "$(INTDIR)\glue1571.obj"
	-@erase "$(INTDIR)\iec-cmdline-options.obj"
	-@erase "$(INTDIR)\iec-resources.obj"
	-@erase "$(INTDIR)\iec.obj"
	-@erase "$(INTDIR)\iecrom.obj"
	-@erase "$(INTDIR)\memiec.obj"
	-@erase "$(INTDIR)\pc8477.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\via1d1541.obj"
	-@erase "$(INTDIR)\via4000.obj"
	-@erase "$(INTDIR)\wd1770.obj"
	-@erase "$(OUTDIR)\iec.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\iec.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iec.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\iec.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cia1571d.obj" \
	"$(INTDIR)\cia1581d.obj" \
	"$(INTDIR)\fdd.obj" \
	"$(INTDIR)\glue1571.obj" \
	"$(INTDIR)\iec-cmdline-options.obj" \
	"$(INTDIR)\iec-resources.obj" \
	"$(INTDIR)\iec.obj" \
	"$(INTDIR)\iecrom.obj" \
	"$(INTDIR)\memiec.obj" \
	"$(INTDIR)\pc8477.obj" \
	"$(INTDIR)\via1d1541.obj" \
	"$(INTDIR)\via4000.obj" \
	"$(INTDIR)\wd1770.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\iec.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "iec - Win32 Release" || "$(CFG)" == "iec - Win32 Debug"

!IF  "$(CFG)" == "iec - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "iec - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\drive\iec\cia1571d.c

"$(INTDIR)\cia1571d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\cia1581d.c

"$(INTDIR)\cia1581d.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\fdd.c

"$(INTDIR)\fdd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\glue1571.c

"$(INTDIR)\glue1571.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\iec\iec-cmdline-options.c"

"$(INTDIR)\iec-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\iec\iec-resources.c"

"$(INTDIR)\iec-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\iec.c

"$(INTDIR)\iec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\iecrom.c

"$(INTDIR)\iecrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\memiec.c

"$(INTDIR)\memiec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\pc8477.c

"$(INTDIR)\pc8477.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\via1d1541.c

"$(INTDIR)\via1d1541.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\via4000.c

"$(INTDIR)\via4000.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\iec\wd1770.c

"$(INTDIR)\wd1770.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

