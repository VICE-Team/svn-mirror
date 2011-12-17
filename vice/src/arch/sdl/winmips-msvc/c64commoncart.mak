# Microsoft Developer Studio Generated NMAKE File, Based on c64commoncart.dsp
!IF "$(CFG)" == ""
CFG=c64commoncart - Win32 Debug
!MESSAGE No configuration specified. Defaulting to c64commoncart - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "c64commoncart - Win32 Release" && "$(CFG)" != "c64commoncart - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "c64commoncart.mak" CFG="c64commoncart - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "c64commoncart - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "c64commoncart - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "c64commoncart - Win32 Release"

OUTDIR=.\libs\c64commoncart\Release
INTDIR=.\libs\c64commoncart\Release
# Begin Custom Macros
OutDir=.\libs\c64commoncart\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c64commoncart.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\c64commoncart.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c64acia1.obj"
	-@erase "$(INTDIR)\digimax.obj"
	-@erase "$(INTDIR)\georam.obj"
	-@erase "$(INTDIR)\sfx_soundexpander.obj"
	-@erase "$(INTDIR)\sfx_soundsampler.obj"
	-@erase "$(INTDIR)\tfe.obj"
	-@erase "$(OUTDIR)\c64commoncart.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\rtc" /I "..\..\..\vicii" /I "..\..\..\drive" /I "..\..\..\sid" /I "..\..\..\monitor" /I "..\..\..\core" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c64commoncart.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c64commoncart.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c64commoncart.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c64acia1.obj" \
	"$(INTDIR)\digimax.obj" \
	"$(INTDIR)\georam.obj" \
	"$(INTDIR)\sfx_soundexpander.obj" \
	"$(INTDIR)\sfx_soundsampler.obj" \
	"$(INTDIR)\tfe.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\c64commoncart.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "c64commoncart - Win32 Debug"

OUTDIR=.\libs\c64commoncart\Debug
INTDIR=.\libs\c64commoncart\Debug
# Begin Custom Macros
OutDir=.\libs\c64commoncart\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c64commoncart.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\c64commoncart.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c64acia1.obj"
	-@erase "$(INTDIR)\digimax.obj"
	-@erase "$(INTDIR)\georam.obj"
	-@erase "$(INTDIR)\sfx_soundexpander.obj"
	-@erase "$(INTDIR)\sfx_soundsampler.obj"
	-@erase "$(INTDIR)\tfe.obj"
	-@erase "$(OUTDIR)\c64commoncart.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\rtc" /I "..\..\..\vicii" /I "..\..\..\drive" /I "..\..\..\sid" /I "..\..\..\monitor" /I "..\..\..\core" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c64commoncart.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c64commoncart.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c64commoncart.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c64acia1.obj" \
	"$(INTDIR)\digimax.obj" \
	"$(INTDIR)\georam.obj" \
	"$(INTDIR)\sfx_soundexpander.obj" \
	"$(INTDIR)\sfx_soundsampler.obj" \
	"$(INTDIR)\tfe.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\c64commoncart.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "c64commoncart - Win32 Release" || "$(CFG)" == "c64commoncart - Win32 Debug"

!IF  "$(CFG)" == "c64commoncart - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "c64commoncart - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\c64\cart\c64acia1.c

"$(INTDIR)\c64acia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\digimax.c

"$(INTDIR)\digimax.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\georam.c

"$(INTDIR)\georam.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\sfx_soundexpander.c

"$(INTDIR)\sfx_soundexpander.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\sfx_soundsampler.c

"$(INTDIR)\sfx_soundsampler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\tfe.c

"$(INTDIR)\tfe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

