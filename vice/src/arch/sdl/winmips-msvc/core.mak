# Microsoft Developer Studio Generated NMAKE File, Based on core.dsp
!IF "$(CFG)" == ""
CFG=core - Win32 Release
!MESSAGE No configuration specified. Defaulting to core - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "core - Win32 Release" && "$(CFG)" != "core - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "core.mak" CFG="core - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "core - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "core - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "core - Win32 Release"

OUTDIR=.\libs\core\Release
INTDIR=.\libs\core\Release
# Begin Custom Macros
OutDir=.\libs\core\Release
# End Custom Macros

ALL : "$(OUTDIR)\core.lib"


CLEAN :
	-@erase "$(INTDIR)\ata.obj"
	-@erase "$(INTDIR)\ciacore.obj"
	-@erase "$(INTDIR)\ciatimer.obj"
	-@erase "$(INTDIR)\flash040core.obj"
	-@erase "$(INTDIR)\fmopl.obj"
	-@erase "$(INTDIR)\mc6821core.obj"
	-@erase "$(INTDIR)\riotcore.obj"
	-@erase "$(INTDIR)\ser-eeprom.obj"
	-@erase "$(INTDIR)\spi-sdcard.obj"
	-@erase "$(INTDIR)\t6721.obj"
	-@erase "$(INTDIR)\tpicore.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\viacore.obj"
	-@erase "$(OUTDIR)\core.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\core" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\core.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\core.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\core.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ata.obj" \
	"$(INTDIR)\ciacore.obj" \
	"$(INTDIR)\ciatimer.obj" \
	"$(INTDIR)\flash040core.obj" \
	"$(INTDIR)\fmopl.obj" \
	"$(INTDIR)\mc6821core.obj" \
	"$(INTDIR)\riotcore.obj" \
	"$(INTDIR)\ser-eeprom.obj" \
	"$(INTDIR)\spi-sdcard.obj" \
	"$(INTDIR)\t6721.obj" \
	"$(INTDIR)\tpicore.obj" \
	"$(INTDIR)\viacore.obj"

"$(OUTDIR)\core.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "core - Win32 Debug"

OUTDIR=.\libs\core\Debug
INTDIR=.\libs\core\Debug
# Begin Custom Macros
OutDir=.\libs\core\Debug
# End Custom Macros

ALL : "$(OUTDIR)\core.lib"


CLEAN :
	-@erase "$(INTDIR)\ata.obj"
	-@erase "$(INTDIR)\ciacore.obj"
	-@erase "$(INTDIR)\ciatimer.obj"
	-@erase "$(INTDIR)\flash040core.obj"
	-@erase "$(INTDIR)\fmopl.obj"
	-@erase "$(INTDIR)\mc6821core.obj"
	-@erase "$(INTDIR)\riotcore.obj"
	-@erase "$(INTDIR)\ser-eeprom.obj"
	-@erase "$(INTDIR)\spi-sdcard.obj"
	-@erase "$(INTDIR)\t6721.obj"
	-@erase "$(INTDIR)\tpicore.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\viacore.obj"
	-@erase "$(OUTDIR)\core.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\core" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\core.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\core.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\core.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ata.obj" \
	"$(INTDIR)\ciacore.obj" \
	"$(INTDIR)\ciatimer.obj" \
	"$(INTDIR)\flash040core.obj" \
	"$(INTDIR)\fmopl.obj" \
	"$(INTDIR)\mc6821core.obj" \
	"$(INTDIR)\riotcore.obj" \
	"$(INTDIR)\ser-eeprom.obj" \
	"$(INTDIR)\spi-sdcard.obj" \
	"$(INTDIR)\t6721.obj" \
	"$(INTDIR)\tpicore.obj" \
	"$(INTDIR)\viacore.obj"

"$(OUTDIR)\core.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "core - Win32 Release" || "$(CFG)" == "core - Win32 Debug"
SOURCE="..\..\..\core\ata.c"

"$(INTDIR)\ata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\ciacore.c"

"$(INTDIR)\ciacore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\ciatimer.c"

"$(INTDIR)\ciatimer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\flash040core.c"

"$(INTDIR)\flash040core.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\core\fmopl.c

"$(INTDIR)\fmopl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\mc6821core.c"

"$(INTDIR)\mc6821core.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\riotcore.c"

"$(INTDIR)\riotcore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\ser-eeprom.c"

"$(INTDIR)\ser-eeprom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\spi-sdcard.c"

"$(INTDIR)\spi-sdcard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\t6721.c"

"$(INTDIR)\t6721.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\tpicore.c"

"$(INTDIR)\tpicore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\core\viacore.c"

"$(INTDIR)\viacore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

