# Microsoft Developer Studio Generated NMAKE File, Based on gentranslate.dsp
!IF "$(CFG)" == ""
CFG=gentranslate - Win32 Release
!MESSAGE No configuration specified. Defaulting to gentranslate - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "gentranslate - Win32 Release" && "$(CFG)" != "gentranslate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gentranslate.mak" CFG="gentranslate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gentranslate - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gentranslate - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "gentranslate - Win32 Release"

OUTDIR=.\..\..\..\..\data
INTDIR=.\libs\gentranslate\Release
# Begin Custom Macros
OutDir=.\..\..\..\..\data
# End Custom Macros

ALL : "$(OUTDIR)\gentranslate.exe"


CLEAN :
	-@erase "$(INTDIR)\gentranslate.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\gentranslate.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_CONSOLE" /D "_MBCS" /D "_CRT_SECURE_NO_DEPRECATE" /Fp"$(INTDIR)\gentranslate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gentranslate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib   kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib   /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gentranslate.pdb" /machine:MIPS /out:"$(OUTDIR)\gentranslate.exe" 
LINK32_OBJS= \
	"$(INTDIR)\gentranslate.obj"

"$(OUTDIR)\gentranslate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gentranslate - Win32 Debug"

OUTDIR=.\..\..\..\..\data
INTDIR=.\libs\gentranslate\Debug
# Begin Custom Macros
OutDir=.\..\..\..\..\data
# End Custom Macros

ALL : "$(OUTDIR)\gentranslate.exe"


CLEAN :
	-@erase "$(INTDIR)\gentranslate.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\gentranslate.exe"
	-@erase "$(OUTDIR)\gentranslate.ilk"
	-@erase "$(OUTDIR)\gentranslate.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_CONSOLE" /D "_MBCS" /D "_CRT_SECURE_NO_DEPRECATE" /Fp"$(INTDIR)\gentranslate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gentranslate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib   kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib   /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\gentranslate.pdb" /debug /machine:MIPS /out:"$(OUTDIR)\gentranslate.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\gentranslate.obj"

"$(OUTDIR)\gentranslate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "gentranslate - Win32 Release" || "$(CFG)" == "gentranslate - Win32 Debug"
SOURCE=..\..\win32\utils\gentranslate.c

"$(INTDIR)\gentranslate.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

