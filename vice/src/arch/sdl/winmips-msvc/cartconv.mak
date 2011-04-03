# Microsoft Developer Studio Generated NMAKE File, Based on cartconv.dsp
!IF "$(CFG)" == ""
CFG=cartconv - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cartconv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cartconv - Win32 Release" && "$(CFG)" != "cartconv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cartconv.mak" CFG="cartconv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cartconv - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "cartconv - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cartconv - Win32 Release"

OUTDIR=.\..\..\..\..\data
INTDIR=.\libs\cartconv\Release
# Begin Custom Macros
OutDir=.\..\..\..\..\data
# End Custom Macros

ALL : "$(OUTDIR)\cartconv.exe"


CLEAN :
	-@erase "$(INTDIR)\cartconv.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cartconv.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\cartconv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cartconv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib wsock32.lib version.lib SDLmain.lib SDL.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\cartconv.pdb" /machine:MIPS /out:"$(OUTDIR)\cartconv.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cartconv.obj"

"$(OUTDIR)\cartconv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cartconv - Win32 Debug"

OUTDIR=.\..\..\..\..\data
INTDIR=.\libs\cartconv\Debug
# Begin Custom Macros
OutDir=.\..\..\..\..\data
# End Custom Macros

ALL : "$(OUTDIR)\cartconv.exe"


CLEAN :
	-@erase "$(INTDIR)\cartconv.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cartconv.exe"
	-@erase "$(OUTDIR)\cartconv.ilk"
	-@erase "$(OUTDIR)\cartconv.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\cartconv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cartconv.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib wsock32.lib version.lib SDLmain.lib SDL.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\cartconv.pdb" /debug /machine:MIPS /nodefaultlib:"msvcrt.lib" /out:"$(OUTDIR)\cartconv.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cartconv.obj"

"$(OUTDIR)\cartconv.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "cartconv - Win32 Release" || "$(CFG)" == "cartconv - Win32 Debug"
SOURCE=..\..\..\cartconv.c

"$(INTDIR)\cartconv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

