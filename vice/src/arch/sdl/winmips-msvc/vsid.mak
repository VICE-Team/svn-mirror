# Microsoft Developer Studio Generated NMAKE File, Based on vsid.dsp
!IF "$(CFG)" == ""
CFG=vsid - Win32 Debug
!MESSAGE No configuration specified. Defaulting to vsid - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "vsid - Win32 Release" && "$(CFG)" != "vsid - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsid.mak" CFG="vsid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsid - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vsid - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

OUTDIR=.\..\..\..\..\data
INTDIR=.\libs\vsid\Release
# Begin Custom Macros
OutDir=.\..\..\..\..\data
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vsid.exe"

!ELSE 

ALL : "iecbus - Win32 Release" "core - Win32 Release" "diskimage - Win32 Release" "monitor - Win32 Release" "video - Win32 Release" "vdrive - Win32 Release" "raster - Win32 Release" "vicii - Win32 Release" "sounddrv - Win32 Release" "sid - Win32 Release" "serial - Win32 Release" "rs232drv - Win32 Release" "parallel - Win32 Release" "ieee - Win32 Release" "iecieee - Win32 Release" "c64exp - Win32 Release" "iec - Win32 Release" "drive - Win32 Release" "vsidlib - Win32 Release" "base - Win32 Release" "arch - Win32 Release" "$(OUTDIR)\vsid.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"arch - Win32 ReleaseCLEAN" "base - Win32 ReleaseCLEAN" "vsidlib - Win32 ReleaseCLEAN" "drive - Win32 ReleaseCLEAN" "iec - Win32 ReleaseCLEAN" "c64exp - Win32 ReleaseCLEAN" "iecieee - Win32 ReleaseCLEAN" "ieee - Win32 ReleaseCLEAN" "parallel - Win32 ReleaseCLEAN" "rs232drv - Win32 ReleaseCLEAN" "serial - Win32 ReleaseCLEAN" "sid - Win32 ReleaseCLEAN" "sounddrv - Win32 ReleaseCLEAN" "vicii - Win32 ReleaseCLEAN" "raster - Win32 ReleaseCLEAN" "vdrive - Win32 ReleaseCLEAN" "video - Win32 ReleaseCLEAN" "monitor - Win32 ReleaseCLEAN" "diskimage - Win32 ReleaseCLEAN" "core - Win32 ReleaseCLEAN" "iecbus - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\maincpu.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\vsid.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vsid.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vsid.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ole32.lib wsock32.lib version.lib SDLmain.lib SDL.lib opengl32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\vsid.pdb" /machine:MIPS /out:"$(OUTDIR)\vsid.exe" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\maincpu.obj" \
	".\libs\arch\Release\arch.lib" \
	".\libs\base\Release\base.lib" \
	".\libs\vsidlib\Release\vsidlib.lib" \
	".\libs\drive\Release\drive.lib" \
	".\libs\iec\Release\iec.lib" \
	".\libs\c64exp\Release\c64exp.lib" \
	".\libs\iecieee\Release\iecieee.lib" \
	".\libs\ieee\Release\ieee.lib" \
	".\libs\parallel\Release\parallel.lib" \
	".\libs\rs232drv\Release\rs232drv.lib" \
	".\libs\serial\Release\serial.lib" \
	".\libs\sid\Release\sid.lib" \
	".\libs\sounddrv\Release\sounddrv.lib" \
	".\libs\vicii\Release\vicii.lib" \
	".\libs\raster\Release\raster.lib" \
	".\libs\vdrive\Release\vdrive.lib" \
	".\libs\video\Release\video.lib" \
	".\libs\monitor\Release\monitor.lib" \
	".\libs\diskimage\Release\diskimage.lib" \
	".\libs\core\Release\core.lib" \
	".\libs\iecbus\Release\iecbus.lib"

"$(OUTDIR)\vsid.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

OUTDIR=.\..\..\..\..\data
INTDIR=.\libs\vsid\Debug
# Begin Custom Macros
OutDir=.\..\..\..\..\data
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\vsid.exe"

!ELSE 

ALL : "iecbus - Win32 Debug" "core - Win32 Debug" "diskimage - Win32 Debug" "monitor - Win32 Debug" "video - Win32 Debug" "vdrive - Win32 Debug" "raster - Win32 Debug" "vicii - Win32 Debug" "sounddrv - Win32 Debug" "sid - Win32 Debug" "serial - Win32 Debug" "rs232drv - Win32 Debug" "parallel - Win32 Debug" "ieee - Win32 Debug" "iecieee - Win32 Debug" "c64exp - Win32 Debug" "iec - Win32 Debug" "drive - Win32 Debug" "vsidlib - Win32 Debug" "base - Win32 Debug" "arch - Win32 Debug" "$(OUTDIR)\vsid.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"arch - Win32 DebugCLEAN" "base - Win32 DebugCLEAN" "vsidlib - Win32 DebugCLEAN" "drive - Win32 DebugCLEAN" "iec - Win32 DebugCLEAN" "c64exp - Win32 DebugCLEAN" "iecieee - Win32 DebugCLEAN" "ieee - Win32 DebugCLEAN" "parallel - Win32 DebugCLEAN" rs232drv - Win32 DebugCLEAN" "serial - Win32 DebugCLEAN" "sid - Win32 DebugCLEAN" "sounddrv - Win32 DebugCLEAN" "vicii - Win32 DebugCLEAN" "raster - Win32 DebugCLEAN" "vdrive - Win32 DebugCLEAN" "video - Win32 DebugCLEAN" "monitor - Win32 DebugCLEAN" "diskimage - Win32 DebugCLEAN" "core - Win32 DebugCLEAN" "iecbus - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\maincpu.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\vsid.exe"
	-@erase "$(OUTDIR)\vsid.ilk"
	-@erase "$(OUTDIR)\vsid.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vsid.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\vsid.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ole32.lib wsock32.lib version.lib SDLmain.lib SDL.lib opengl32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\vsid.pdb" /debug /machine:MIPS /nodefaultlib:"msvcrt.lib" /out:"$(OUTDIR)\vsid.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\maincpu.obj" \
	".\libs\arch\Debug\arch.lib" \
	".\libs\base\Debug\base.lib" \
	".\libs\vsidlib\Debug\vsidlib.lib" \
	".\libs\drive\Debug\drive.lib" \
	".\libs\iec\Debug\iec.lib" \
	".\libs\c64exp\Debug\c64exp.lib" \
	".\libs\iecieee\Debug\iecieee.lib" \
	".\libs\ieee\Debug\ieee.lib" \
	".\libs\parallel\Debug\parallel.lib" \
	".\libs\rs232drv\Debug\rs232drv.lib" \
	".\libs\serial\Debug\serial.lib" \
	".\libs\sid\Debug\sid.lib" \
	".\libs\sounddrv\Debug\sounddrv.lib" \
	".\libs\vicii\Debug\vicii.lib" \
	".\libs\raster\Debug\raster.lib" \
	".\libs\vdrive\Debug\vdrive.lib" \
	".\libs\video\Debug\video.lib" \
	".\libs\monitor\Debug\monitor.lib" \
	".\libs\diskimage\Debug\diskimage.lib" \
	".\libs\core\Debug\core.lib" \
	".\libs\iecbus\Debug\iecbus.lib"

"$(OUTDIR)\vsid.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "vsid - Win32 Release" || "$(CFG)" == "vsid - Win32 Debug"

!IF  "$(CFG)" == "vsid - Win32 Release"

"arch - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\arch.mak" CFG="arch - Win32 Release" 
   cd "."

"arch - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\arch.mak" CFG="arch - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"arch - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\arch.mak" CFG="arch - Win32 Debug" 
   cd "."

"arch - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\arch.mak" CFG="arch - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"vsidlib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vsidlib.mak" CFG="vsidlib - Win32 Release" 
   cd "."

"vsidlib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vsidlib.mak" CFG="vsidlib - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"vsidlib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vsidlib.mak" CFG="vsidlib - Win32 Debug" 
   cd "."

"vsidlib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vsidlib.mak" CFG="vsidlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"drive - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\drive.mak" CFG="drive - Win32 Release" 
   cd "."

"drive - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\drive.mak" CFG="drive - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"drive - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\drive.mak" CFG="drive - Win32 Debug" 
   cd "."

"drive - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\drive.mak" CFG="drive - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"iec - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iec.mak" CFG="iec - Win32 Release" 
   cd "."

"iec - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iec.mak" CFG="iec - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"iec - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iec.mak" CFG="iec - Win32 Debug" 
   cd "."

"iec - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iec.mak" CFG="iec - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"c64exp - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64exp.mak" CFG="c64exp - Win32 Release" 
   cd "."

"c64exp - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64exp.mak" CFG="c64exp - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"c64exp - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64exp.mak" CFG="c64exp - Win32 Debug" 
   cd "."

"c64exp - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c64exp.mak" CFG="c64exp - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"iecieee - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecieee.mak" CFG="iecieee - Win32 Release" 
   cd "."

"iecieee - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecieee.mak" CFG="iecieee - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"iecieee - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecieee.mak" CFG="iecieee - Win32 Debug" 
   cd "."

"iecieee - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecieee.mak" CFG="iecieee - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"ieee - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ieee.mak" CFG="ieee - Win32 Release" 
   cd "."

"ieee - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ieee.mak" CFG="ieee - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"ieee - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ieee.mak" CFG="ieee - Win32 Debug" 
   cd "."

"ieee - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ieee.mak" CFG="ieee - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"parallel - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\parallel.mak" CFG="parallel - Win32 Release" 
   cd "."

"parallel - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\parallel.mak" CFG="parallel - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"parallel - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\parallel.mak" CFG="parallel - Win32 Debug" 
   cd "."

"parallel - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\parallel.mak" CFG="parallel - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"rs232drv - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rs232drv.mak" CFG="rs232drv - Win32 Release" 
   cd "."

"rs232drv - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rs232drv.mak" CFG="rs232drv - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"rs232drv - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rs232drv.mak" CFG="rs232drv - Win32 Debug" 
   cd "."

"rs232drv - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\rs232drv.mak" CFG="rs232drv - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"serial - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\serial.mak" CFG="serial - Win32 Release" 
   cd "."

"serial - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\serial.mak" CFG="serial - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"serial - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\serial.mak" CFG="serial - Win32 Debug" 
   cd "."

"serial - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\serial.mak" CFG="serial - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"sid - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sid.mak" CFG="sid - Win32 Release" 
   cd "."

"sid - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sid.mak" CFG="sid - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"sid - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sid.mak" CFG="sid - Win32 Debug" 
   cd "."

"sid - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sid.mak" CFG="sid - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"sounddrv - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sounddrv.mak" CFG="sounddrv - Win32 Release" 
   cd "."

"sounddrv - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sounddrv.mak" CFG="sounddrv - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"sounddrv - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sounddrv.mak" CFG="sounddrv - Win32 Debug" 
   cd "."

"sounddrv - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\sounddrv.mak" CFG="sounddrv - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"vicii - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vicii.mak" CFG="vicii - Win32 Release" 
   cd "."

"vicii - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vicii.mak" CFG="vicii - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"vicii - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vicii.mak" CFG="vicii - Win32 Debug" 
   cd "."

"vicii - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vicii.mak" CFG="vicii - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"raster - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\raster.mak" CFG="raster - Win32 Release" 
   cd "."

"raster - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\raster.mak" CFG="raster - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"raster - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\raster.mak" CFG="raster - Win32 Debug" 
   cd "."

"raster - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\raster.mak" CFG="raster - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"vdrive - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vdrive.mak" CFG="vdrive - Win32 Release" 
   cd "."

"vdrive - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vdrive.mak" CFG="vdrive - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"vdrive - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vdrive.mak" CFG="vdrive - Win32 Debug" 
   cd "."

"vdrive - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vdrive.mak" CFG="vdrive - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"video - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\video.mak" CFG="video - Win32 Release" 
   cd "."

"video - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\video.mak" CFG="video - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"video - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\video.mak" CFG="video - Win32 Debug" 
   cd "."

"video - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\video.mak" CFG="video - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"monitor - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\monitor.mak" CFG="monitor - Win32 Release" 
   cd "."

"monitor - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\monitor.mak" CFG="monitor - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"monitor - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\monitor.mak" CFG="monitor - Win32 Debug" 
   cd "."

"monitor - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\monitor.mak" CFG="monitor - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"diskimage - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\diskimage.mak" CFG="diskimage - Win32 Release" 
   cd "."

"diskimage - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\diskimage.mak" CFG="diskimage - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"diskimage - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\diskimage.mak" CFG="diskimage - Win32 Debug" 
   cd "."

"diskimage - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\diskimage.mak" CFG="diskimage - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"core - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\core.mak" CFG="core - Win32 Release" 
   cd "."

"core - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\core.mak" CFG="core - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"core - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\core.mak" CFG="core - Win32 Debug" 
   cd "."

"core - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\core.mak" CFG="core - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "vsid - Win32 Release"

"iecbus - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecbus.mak" CFG="iecbus - Win32 Release" 
   cd "."

"iecbus - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecbus.mak" CFG="iecbus - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

"iecbus - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecbus.mak" CFG="iecbus - Win32 Debug" 
   cd "."

"iecbus - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\iecbus.mak" CFG="iecbus - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\main.c

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\maincpu.c

!IF  "$(CFG)" == "vsid - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Ot /Oa /Ow /Oi /Op /Oy /Ob2 /I ".\\" /I "..\\" /I "..\..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vsid.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

"$(INTDIR)\maincpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\monitor" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\vsid.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

"$(INTDIR)\maincpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

