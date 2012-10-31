# Microsoft Developer Studio Generated NMAKE File, Based on printerdrv.dsp
!IF "$(CFG)" == ""
CFG=printerdrv - Win32 Release
!MESSAGE No configuration specified. Defaulting to printerdrv - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "printerdrv - Win32 Release" && "$(CFG)" != "printerdrv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "printerdrv.mak" CFG="printerdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "printerdrv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "printerdrv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "printerdrv - Win32 Release"

OUTDIR=.\libs\printerdrv\Release
INTDIR=.\libs\printerdrv\Release
# Begin Custom Macros
OutDir=.\libs\printerdrv\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\printerdrv.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\printerdrv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\driver-select.obj"
	-@erase "$(INTDIR)\drv-ascii.obj"
	-@erase "$(INTDIR)\drv-mps803.obj"
	-@erase "$(INTDIR)\drv-nl10.obj"
	-@erase "$(INTDIR)\drv-raw.obj"
	-@erase "$(INTDIR)\interface-serial.obj"
	-@erase "$(INTDIR)\interface-userport.obj"
	-@erase "$(INTDIR)\output-graphics.obj"
	-@erase "$(INTDIR)\output-select.obj"
	-@erase "$(INTDIR)\output-text.obj"
	-@erase "$(INTDIR)\printer-serial.obj"
	-@erase "$(INTDIR)\printer-userport.obj"
	-@erase "$(INTDIR)\printer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\printerdrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /D "NDEBUG" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_WINDOWS" /Fp"$(INTDIR)\printerdrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\printerdrv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\printerdrv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\driver-select.obj" \
	"$(INTDIR)\drv-ascii.obj" \
	"$(INTDIR)\drv-mps803.obj" \
	"$(INTDIR)\drv-nl10.obj" \
	"$(INTDIR)\drv-raw.obj" \
	"$(INTDIR)\interface-serial.obj" \
	"$(INTDIR)\interface-userport.obj" \
	"$(INTDIR)\output-graphics.obj" \
	"$(INTDIR)\output-select.obj" \
	"$(INTDIR)\output-text.obj" \
	"$(INTDIR)\printer-serial.obj" \
	"$(INTDIR)\printer-userport.obj" \
	"$(INTDIR)\printer.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\printerdrv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "printerdrv - Win32 Debug"

OUTDIR=.\libs\printerdrv\Debug
INTDIR=.\libs\printerdrv\Debug
# Begin Custom Macros
OutDir=.\libs\printerdrv\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\printerdrv.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\printerdrv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\driver-select.obj"
	-@erase "$(INTDIR)\drv-ascii.obj"
	-@erase "$(INTDIR)\drv-mps803.obj"
	-@erase "$(INTDIR)\drv-nl10.obj"
	-@erase "$(INTDIR)\drv-raw.obj"
	-@erase "$(INTDIR)\interface-serial.obj"
	-@erase "$(INTDIR)\interface-userport.obj"
	-@erase "$(INTDIR)\output-graphics.obj"
	-@erase "$(INTDIR)\output-select.obj"
	-@erase "$(INTDIR)\output-text.obj"
	-@erase "$(INTDIR)\printer-serial.obj"
	-@erase "$(INTDIR)\printer-userport.obj"
	-@erase "$(INTDIR)\printer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\printerdrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /D "_DEBUG" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_WINDOWS" /Fp"$(INTDIR)\printerdrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\printerdrv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\printerdrv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\driver-select.obj" \
	"$(INTDIR)\drv-ascii.obj" \
	"$(INTDIR)\drv-mps803.obj" \
	"$(INTDIR)\drv-nl10.obj" \
	"$(INTDIR)\drv-raw.obj" \
	"$(INTDIR)\interface-serial.obj" \
	"$(INTDIR)\interface-userport.obj" \
	"$(INTDIR)\output-graphics.obj" \
	"$(INTDIR)\output-select.obj" \
	"$(INTDIR)\output-text.obj" \
	"$(INTDIR)\printer-serial.obj" \
	"$(INTDIR)\printer-userport.obj" \
	"$(INTDIR)\printer.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\printerdrv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "printerdrv - Win32 Release" || "$(CFG)" == "printerdrv - Win32 Debug"

!IF  "$(CFG)" == "printerdrv - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "printerdrv - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\printerdrv\driver-select.c"

"$(INTDIR)\driver-select.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\drv-ascii.c"

"$(INTDIR)\drv-ascii.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\drv-mps803.c"

"$(INTDIR)\drv-mps803.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\drv-nl10.c"

"$(INTDIR)\drv-nl10.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\drv-raw.c"

"$(INTDIR)\drv-raw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\interface-serial.c"

"$(INTDIR)\interface-serial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\interface-userport.c"

"$(INTDIR)\interface-userport.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\output-graphics.c"

"$(INTDIR)\output-graphics.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\output-select.c"

"$(INTDIR)\output-select.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\output-text.c"

"$(INTDIR)\output-text.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\printer-serial.c"

"$(INTDIR)\printer-serial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\printerdrv\printer-userport.c"

"$(INTDIR)\printer-userport.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\printerdrv\printer.c

"$(INTDIR)\printer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

