# Microsoft Developer Studio Generated NMAKE File, Based on drive.dsp
!IF "$(CFG)" == ""
CFG=drive - Win32 Debug
!MESSAGE No configuration specified. Defaulting to drive - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "drive - Win32 Release" && "$(CFG)" != "drive - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "drive.mak" CFG="drive - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "drive - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "drive - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "drive - Win32 Release"

OUTDIR=.\libs\drive\Release
INTDIR=.\libs\drive\Release
# Begin Custom Macros
OutDir=.\libs\drive\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\drive.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\drive.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\drive-check.obj"
	-@erase "$(INTDIR)\drive-cmdline-options.obj"
	-@erase "$(INTDIR)\drive-overflow.obj"
	-@erase "$(INTDIR)\drive-resources.obj"
	-@erase "$(INTDIR)\drive-snapshot.obj"
	-@erase "$(INTDIR)\drive-sound.obj"
	-@erase "$(INTDIR)\drive-writeprotect.obj"
	-@erase "$(INTDIR)\drive.obj"
	-@erase "$(INTDIR)\drivecpu.obj"
	-@erase "$(INTDIR)\driveimage.obj"
	-@erase "$(INTDIR)\drivemem.obj"
	-@erase "$(INTDIR)\driverom.obj"
	-@erase "$(INTDIR)\drivesync.obj"
	-@erase "$(INTDIR)\rotation.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\drive.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vdrive" /I "..\..\..\rtc" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\drive.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\drive.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\drive.lib" 
LIB32_OBJS= \
	"$(INTDIR)\drive-check.obj" \
	"$(INTDIR)\drive-cmdline-options.obj" \
	"$(INTDIR)\drive-overflow.obj" \
	"$(INTDIR)\drive-resources.obj" \
	"$(INTDIR)\drive-snapshot.obj" \
	"$(INTDIR)\drive-sound.obj" \
	"$(INTDIR)\drive-writeprotect.obj" \
	"$(INTDIR)\drive.obj" \
	"$(INTDIR)\drivecpu.obj" \
	"$(INTDIR)\driveimage.obj" \
	"$(INTDIR)\drivemem.obj" \
	"$(INTDIR)\driverom.obj" \
	"$(INTDIR)\drivesync.obj" \
	"$(INTDIR)\rotation.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\drive.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "drive - Win32 Debug"

OUTDIR=.\libs\drive\Debug
INTDIR=.\libs\drive\Debug
# Begin Custom Macros
OutDir=.\libs\drive\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\drive.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\drive.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\drive-check.obj"
	-@erase "$(INTDIR)\drive-cmdline-options.obj"
	-@erase "$(INTDIR)\drive-overflow.obj"
	-@erase "$(INTDIR)\drive-resources.obj"
	-@erase "$(INTDIR)\drive-snapshot.obj"
	-@erase "$(INTDIR)\drive-sound.obj"
	-@erase "$(INTDIR)\drive-writeprotect.obj"
	-@erase "$(INTDIR)\drive.obj"
	-@erase "$(INTDIR)\drivecpu.obj"
	-@erase "$(INTDIR)\driveimage.obj"
	-@erase "$(INTDIR)\drivemem.obj"
	-@erase "$(INTDIR)\driverom.obj"
	-@erase "$(INTDIR)\drivesync.obj"
	-@erase "$(INTDIR)\rotation.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\drive.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vdrive" /I "..\..\..\rtc" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\drive.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\drive.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\drive.lib" 
LIB32_OBJS= \
	"$(INTDIR)\drive-check.obj" \
	"$(INTDIR)\drive-cmdline-options.obj" \
	"$(INTDIR)\drive-overflow.obj" \
	"$(INTDIR)\drive-resources.obj" \
	"$(INTDIR)\drive-snapshot.obj" \
	"$(INTDIR)\drive-sound.obj" \
	"$(INTDIR)\drive-writeprotect.obj" \
	"$(INTDIR)\drive.obj" \
	"$(INTDIR)\drivecpu.obj" \
	"$(INTDIR)\driveimage.obj" \
	"$(INTDIR)\drivemem.obj" \
	"$(INTDIR)\driverom.obj" \
	"$(INTDIR)\drivesync.obj" \
	"$(INTDIR)\rotation.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\drive.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "drive - Win32 Release" || "$(CFG)" == "drive - Win32 Debug"

!IF  "$(CFG)" == "drive - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "drive - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\drive\drive-check.c"

"$(INTDIR)\drive-check.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\drive-cmdline-options.c"

"$(INTDIR)\drive-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\drive-overflow.c"

"$(INTDIR)\drive-overflow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\drive-resources.c"

"$(INTDIR)\drive-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\drive-snapshot.c"

"$(INTDIR)\drive-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\drive-sound.c"

"$(INTDIR)\drive-sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\drive\drive-writeprotect.c"

"$(INTDIR)\drive-writeprotect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\drive.c

"$(INTDIR)\drive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\drivecpu.c

"$(INTDIR)\drivecpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\driveimage.c

"$(INTDIR)\driveimage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\drivemem.c

"$(INTDIR)\drivemem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\driverom.c

"$(INTDIR)\driverom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\drivesync.c

"$(INTDIR)\drivesync.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\drive\rotation.c

"$(INTDIR)\rotation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

