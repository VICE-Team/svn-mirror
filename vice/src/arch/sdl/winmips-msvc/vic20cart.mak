# Microsoft Developer Studio Generated NMAKE File, Based on vic20cart.dsp
!IF "$(CFG)" == ""
CFG=vic20cart - Win32 Release
!MESSAGE No configuration specified. Defaulting to vic20cart - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "vic20cart - Win32 Release" && "$(CFG)" != "vic20cart - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vic20cart.mak" CFG="vic20cart - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vic20cart - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vic20cart - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "vic20cart - Win32 Release"

OUTDIR=.\libs\vic20cart\Release
INTDIR=.\libs\vic20cart\Release
# Begin Custom Macros
OutDir=.\libs\vic20cart\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vic20cart.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\vic20cart.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\finalexpansion.obj"
	-@erase "$(INTDIR)\mascuerade-stubs.obj"
	-@erase "$(INTDIR)\megacart.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vic-fp.obj"
	-@erase "$(INTDIR)\vic20-generic.obj"
	-@erase "$(INTDIR)\vic20-ieee488.obj"
	-@erase "$(INTDIR)\vic20-midi.obj"
	-@erase "$(INTDIR)\vic20-sidcart.obj"
	-@erase "$(INTDIR)\vic20cart.obj"
	-@erase "$(INTDIR)\vic20cartmem.obj"
	-@erase "$(OUTDIR)\vic20cart.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vic20" /I "..\..\..\c64\cart" /I "..\..\..\drive" /I "..\..\..\tape" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vic20cart.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vic20cart.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vic20cart.lib" 
LIB32_OBJS= \
	"$(INTDIR)\finalexpansion.obj" \
	"$(INTDIR)\mascuerade-stubs.obj" \
	"$(INTDIR)\megacart.obj" \
	"$(INTDIR)\vic20-generic.obj" \
	"$(INTDIR)\vic20-ieee488.obj" \
	"$(INTDIR)\vic20-midi.obj" \
	"$(INTDIR)\vic20-sidcart.obj" \
	"$(INTDIR)\vic20cart.obj" \
	"$(INTDIR)\vic20cartmem.obj" \
	"$(INTDIR)\vic-fp.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\vic20cart.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vic20cart - Win32 Debug"

OUTDIR=.\libs\vic20cart\Debug
INTDIR=.\libs\vic20cart\Debug
# Begin Custom Macros
OutDir=.\libs\vic20cart\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vic20cart.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\vic20cart.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\finalexpansion.obj"
	-@erase "$(INTDIR)\mascuerade-stubs.obj"
	-@erase "$(INTDIR)\megacart.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vic-fp.obj"
	-@erase "$(INTDIR)\vic20-generic.obj"
	-@erase "$(INTDIR)\vic20-ieee488.obj"
	-@erase "$(INTDIR)\vic20-midi.obj"
	-@erase "$(INTDIR)\vic20-sidcart.obj"
	-@erase "$(INTDIR)\vic20cart.obj"
	-@erase "$(INTDIR)\vic20cartmem.obj"
	-@erase "$(OUTDIR)\vic20cart.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vic20" /I "..\..\..\c64\cart" /I "..\..\..\drive" /I "..\..\..\tape" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vic20cart.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vic20cart.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\vic20cart.lib" 
LIB32_OBJS= \
	"$(INTDIR)\finalexpansion.obj" \
	"$(INTDIR)\mascuerade-stubs.obj" \
	"$(INTDIR)\megacart.obj" \
	"$(INTDIR)\vic20-generic.obj" \
	"$(INTDIR)\vic20-ieee488.obj" \
	"$(INTDIR)\vic20-midi.obj" \
	"$(INTDIR)\vic20-sidcart.obj" \
	"$(INTDIR)\vic20cart.obj" \
	"$(INTDIR)\vic20cartmem.obj" \
	"$(INTDIR)\vic-fp.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\vic20cart.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "vic20cart - Win32 Release" || "$(CFG)" == "vic20cart - Win32 Debug"

!IF  "$(CFG)" == "vic20cart - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vic20cart - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\vic20\cart\finalexpansion.c"

"$(INTDIR)\finalexpansion.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\mascuerade-stubs.c"

"$(INTDIR)\mascuerade-stubs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\megacart.c"

"$(INTDIR)\megacart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\vic-fp.c"

"$(INTDIR)\vic-fp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\vic20-generic.c"

"$(INTDIR)\vic20-generic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\vic20-ieee488.c"

"$(INTDIR)\vic20-ieee488.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\vic20-midi.c"

"$(INTDIR)\vic20-midi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\vic20-sidcart.c"

"$(INTDIR)\vic20-sidcart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\vic20cart.c"

"$(INTDIR)\vic20cart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\vic20\cart\vic20cartmem.c"

"$(INTDIR)\vic20cartmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

