# Microsoft Developer Studio Generated NMAKE File, Based on serial.dsp
!IF "$(CFG)" == ""
CFG=serial - Win32 Debug
!MESSAGE No configuration specified. Defaulting to serial - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "serial - Win32 Release" && "$(CFG)" != "serial - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "serial.mak" CFG="serial - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "serial - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "serial - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "serial - Win32 Release"

OUTDIR=.\libs\serial\Release
INTDIR=.\libs\serial\Release
# Begin Custom Macros
OutDir=.\libs\serial\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\serial.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\serial.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\fsdrive.obj"
	-@erase "$(INTDIR)\realdevice.obj"
	-@erase "$(INTDIR)\serial-device.obj"
	-@erase "$(INTDIR)\serial-iec-bus.obj"
	-@erase "$(INTDIR)\serial-iec-device.obj"
	-@erase "$(INTDIR)\serial-iec-lib.obj"
	-@erase "$(INTDIR)\serial-iec.obj"
	-@erase "$(INTDIR)\serial-realdevice.obj"
	-@erase "$(INTDIR)\serial-trap.obj"
	-@erase "$(INTDIR)\serial.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\serial.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vdrive" /I "..\..\..\drive" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\serial.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\serial.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\serial.lib" 
LIB32_OBJS= \
	"$(INTDIR)\fsdrive.obj" \
	"$(INTDIR)\realdevice.obj" \
	"$(INTDIR)\serial-device.obj" \
	"$(INTDIR)\serial-iec-bus.obj" \
	"$(INTDIR)\serial-iec-device.obj" \
	"$(INTDIR)\serial-iec-lib.obj" \
	"$(INTDIR)\serial-iec.obj" \
	"$(INTDIR)\serial-realdevice.obj" \
	"$(INTDIR)\serial-trap.obj" \
	"$(INTDIR)\serial.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\serial.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "serial - Win32 Debug"

OUTDIR=.\libs\serial\Debug
INTDIR=.\libs\serial\Debug
# Begin Custom Macros
OutDir=.\libs\serial\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\serial.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\serial.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\fsdrive.obj"
	-@erase "$(INTDIR)\realdevice.obj"
	-@erase "$(INTDIR)\serial-device.obj"
	-@erase "$(INTDIR)\serial-iec-bus.obj"
	-@erase "$(INTDIR)\serial-iec-device.obj"
	-@erase "$(INTDIR)\serial-iec-lib.obj"
	-@erase "$(INTDIR)\serial-iec.obj"
	-@erase "$(INTDIR)\serial-realdevice.obj"
	-@erase "$(INTDIR)\serial-trap.obj"
	-@erase "$(INTDIR)\serial.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\serial.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vdrive" /I "..\..\..\drive" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\serial.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\serial.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\serial.lib" 
LIB32_OBJS= \
	"$(INTDIR)\fsdrive.obj" \
	"$(INTDIR)\realdevice.obj" \
	"$(INTDIR)\serial-device.obj" \
	"$(INTDIR)\serial-iec-bus.obj" \
	"$(INTDIR)\serial-iec-device.obj" \
	"$(INTDIR)\serial-iec-lib.obj" \
	"$(INTDIR)\serial-iec.obj" \
	"$(INTDIR)\serial-realdevice.obj" \
	"$(INTDIR)\serial-trap.obj" \
	"$(INTDIR)\serial.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\serial.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "serial - Win32 Release" || "$(CFG)" == "serial - Win32 Debug"

!IF  "$(CFG)" == "serial - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "serial - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\serial\fsdrive.c"

"$(INTDIR)\fsdrive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\realdevice.c"

"$(INTDIR)\realdevice.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\serial-device.c"

"$(INTDIR)\serial-device.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\serial-iec-bus.c"

"$(INTDIR)\serial-iec-bus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\serial-iec-device.c"

"$(INTDIR)\serial-iec-device.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\serial-iec-lib.c"

"$(INTDIR)\serial-iec-lib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\serial-iec.c"

"$(INTDIR)\serial-iec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\serial-realdevice.c"

"$(INTDIR)\serial-realdevice.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\serial-trap.c"

"$(INTDIR)\serial-trap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\serial\serial.c"

"$(INTDIR)\serial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

