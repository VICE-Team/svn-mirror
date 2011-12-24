# Microsoft Developer Studio Generated NMAKE File, Based on pet.dsp
!IF "$(CFG)" == ""
CFG=pet - Win32 Debug
!MESSAGE No configuration specified. Defaulting to pet - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "pet - Win32 Release" && "$(CFG)" != "pet - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pet.mak" CFG="pet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pet - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pet - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "pet - Win32 Release"

OUTDIR=.\libs\pet\Release
INTDIR=.\libs\pet\Release
# Begin Custom Macros
OutDir=.\libs\pet\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\pet.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\pet.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\pet-cmdline-options.obj"
	-@erase "$(INTDIR)\pet-resources.obj"
	-@erase "$(INTDIR)\pet-sidcart.obj"
	-@erase "$(INTDIR)\pet-snapshot.obj"
	-@erase "$(INTDIR)\pet.obj"
	-@erase "$(INTDIR)\petacia1.obj"
	-@erase "$(INTDIR)\petbus.obj"
	-@erase "$(INTDIR)\petdatasette.obj"
	-@erase "$(INTDIR)\petdrive.obj"
	-@erase "$(INTDIR)\petdww.obj"
	-@erase "$(INTDIR)\petiec.obj"
	-@erase "$(INTDIR)\petmem.obj"
	-@erase "$(INTDIR)\petmemsnapshot.obj"
	-@erase "$(INTDIR)\petpia1.obj"
	-@erase "$(INTDIR)\petpia2.obj"
	-@erase "$(INTDIR)\petprinter.obj"
	-@erase "$(INTDIR)\petreu.obj"
	-@erase "$(INTDIR)\petrom.obj"
	-@erase "$(INTDIR)\petromset.obj"
	-@erase "$(INTDIR)\petsound.obj"
	-@erase "$(INTDIR)\petvia.obj"
	-@erase "$(INTDIR)\petvideo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\pet.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\crtc" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\pet.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pet.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\pet.lib" 
LIB32_OBJS= \
	"$(INTDIR)\pet-cmdline-options.obj" \
	"$(INTDIR)\pet-resources.obj" \
	"$(INTDIR)\pet-sidcart.obj" \
	"$(INTDIR)\pet-snapshot.obj" \
	"$(INTDIR)\pet.obj" \
	"$(INTDIR)\petacia1.obj" \
	"$(INTDIR)\petbus.obj" \
	"$(INTDIR)\petdatasette.obj" \
	"$(INTDIR)\petdrive.obj" \
	"$(INTDIR)\petdww.obj" \
	"$(INTDIR)\petiec.obj" \
	"$(INTDIR)\petmem.obj" \
	"$(INTDIR)\petmemsnapshot.obj" \
	"$(INTDIR)\petpia1.obj" \
	"$(INTDIR)\petpia2.obj" \
	"$(INTDIR)\petprinter.obj" \
	"$(INTDIR)\petreu.obj" \
	"$(INTDIR)\petrom.obj" \
	"$(INTDIR)\petromset.obj" \
	"$(INTDIR)\petsound.obj" \
	"$(INTDIR)\petvia.obj" \
	"$(INTDIR)\petvideo.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\pet.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pet - Win32 Debug"

OUTDIR=.\libs\pet\Debug
INTDIR=.\libs\pet\Debug
# Begin Custom Macros
OutDir=.\libs\pet\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\pet.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\pet.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\pet-cmdline-options.obj"
	-@erase "$(INTDIR)\pet-resources.obj"
	-@erase "$(INTDIR)\pet-sidcart.obj"
	-@erase "$(INTDIR)\pet-snapshot.obj"
	-@erase "$(INTDIR)\pet.obj"
	-@erase "$(INTDIR)\petacia1.obj"
	-@erase "$(INTDIR)\petbus.obj"
	-@erase "$(INTDIR)\petdatasette.obj"
	-@erase "$(INTDIR)\petdrive.obj"
	-@erase "$(INTDIR)\petdww.obj"
	-@erase "$(INTDIR)\petiec.obj"
	-@erase "$(INTDIR)\petmem.obj"
	-@erase "$(INTDIR)\petmemsnapshot.obj"
	-@erase "$(INTDIR)\petpia1.obj"
	-@erase "$(INTDIR)\petpia2.obj"
	-@erase "$(INTDIR)\petprinter.obj"
	-@erase "$(INTDIR)\petreu.obj"
	-@erase "$(INTDIR)\petrom.obj"
	-@erase "$(INTDIR)\petromset.obj"
	-@erase "$(INTDIR)\petsound.obj"
	-@erase "$(INTDIR)\petvia.obj"
	-@erase "$(INTDIR)\petvideo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\pet.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\crtc" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\sid" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\pet.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pet.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\pet.lib" 
LIB32_OBJS= \
	"$(INTDIR)\pet-cmdline-options.obj" \
	"$(INTDIR)\pet-resources.obj" \
	"$(INTDIR)\pet-sidcart.obj" \
	"$(INTDIR)\pet-snapshot.obj" \
	"$(INTDIR)\pet.obj" \
	"$(INTDIR)\petacia1.obj" \
	"$(INTDIR)\petbus.obj" \
	"$(INTDIR)\petdatasette.obj" \
	"$(INTDIR)\petdrive.obj" \
	"$(INTDIR)\petdww.obj" \
	"$(INTDIR)\petiec.obj" \
	"$(INTDIR)\petmem.obj" \
	"$(INTDIR)\petmemsnapshot.obj" \
	"$(INTDIR)\petpia1.obj" \
	"$(INTDIR)\petpia2.obj" \
	"$(INTDIR)\petprinter.obj" \
	"$(INTDIR)\petreu.obj" \
	"$(INTDIR)\petrom.obj" \
	"$(INTDIR)\petromset.obj" \
	"$(INTDIR)\petsound.obj" \
	"$(INTDIR)\petvia.obj" \
	"$(INTDIR)\petvideo.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\pet.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "pet - Win32 Release" || "$(CFG)" == "pet - Win32 Debug"

!IF  "$(CFG)" == "pet - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "pet - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\pet\pet-cmdline-options.c"

"$(INTDIR)\pet-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\pet\pet-resources.c"

"$(INTDIR)\pet-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\pet\pet-sidcart.c"

"$(INTDIR)\pet-sidcart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\pet\pet-snapshot.c"

"$(INTDIR)\pet-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\pet.c

"$(INTDIR)\pet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petacia1.c

"$(INTDIR)\petacia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petbus.c

"$(INTDIR)\petbus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petdatasette.c

"$(INTDIR)\petdatasette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petdrive.c

"$(INTDIR)\petdrive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petdww.c

"$(INTDIR)\petdww.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petiec.c

"$(INTDIR)\petiec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petmem.c

"$(INTDIR)\petmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petmemsnapshot.c

"$(INTDIR)\petmemsnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petpia1.c

"$(INTDIR)\petpia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petpia2.c

"$(INTDIR)\petpia2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petprinter.c

"$(INTDIR)\petprinter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petreu.c

"$(INTDIR)\petreu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petrom.c

"$(INTDIR)\petrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petromset.c

"$(INTDIR)\petromset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petsound.c

"$(INTDIR)\petsound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petvia.c

"$(INTDIR)\petvia.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\pet\petvideo.c

"$(INTDIR)\petvideo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

