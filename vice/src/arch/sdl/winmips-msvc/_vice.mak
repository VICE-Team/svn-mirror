# Microsoft Developer Studio Generated NMAKE File, Based on _vice.dsp
!IF "$(CFG)" == ""
CFG=_vice - Win32 Release
!MESSAGE No configuration specified. Defaulting to _vice - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "_vice - Win32 Release" && "$(CFG)" != "_vice - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "_vice.mak" CFG="_vice - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "_vice - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "_vice - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : 

!ELSE 

ALL : "cartconv - Win32 Release" "c1541 - Win32 Release" "petcat - Win32 Release" "xvic - Win32 Release" "xplus4 - Win32 Release" "xpet - Win32 Release" "xcbm2 - Win32 Release" "xcbm5x0 - Win32 Release" "x128 - Win32 Release" "vsid - Win32 Release" "x64sc - Win32 Release" "x64dtv - Win32 Release" "x64 - Win32 Release" 

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"x64 - Win32 ReleaseCLEAN" "x64dtv - Win32 ReleaseCLEAN" "x64sc - Win32 ReleaseCLEAN" "x128 - Win32 ReleaseCLEAN" "vsid - Win32 ReleaseCLEAN" "xcbm2 - Win32 ReleaseCLEAN" "xcbm5x0 - Win32 ReleaseCLEAN" "xpet - Win32 ReleaseCLEAN" "xplus4 - Win32 ReleaseCLEAN" "xvic - Win32 ReleaseCLEAN" "petcat - Win32 ReleaseCLEAN" "c1541 - Win32 ReleaseCLEAN" "cartconv - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "WINMIPS" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\_vice.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\_vice.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib   /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\_vice.pdb" /machine:MIPS /out:"$(OUTDIR)\_vice.exe" 
LINK32_OBJS= \
	

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : 

!ELSE 

ALL : "cartconv - Win32 Debug" "c1541 - Win32 Debug" "petcat - Win32 Debug" "xvic - Win32 Debug" "xplus4 - Win32 Debug" "xpet - Win32 Debug" "xcbm5x0 - Win32 Debug" "xcbm2 - Win32 Debug" "x128 - Win32 Debug" "vsid - Win32 Debug" "x64sc - Win32 Debug" "x64dtv - Win32 Debug" "x64 - Win32 Debug" 

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"x64 - Win32 DebugCLEAN" "x64dtv - Win32 DebugCLEAN" "x64sc - Win32 DebugCLEAN" "x128 - Win32 DebugCLEAN" "vsid - Win32 DebugCLEAN" "xcbm2 - Win32 DebugCLEAN" "xcbm5x0 - Win32 DebugCLEAN" "xpet - Win32 DebugCLEAN" "xplus4 - Win32 DebugCLEAN" "xvic - Win32 DebugCLEAN" "petcat - Win32 DebugCLEAN" "c1541 - Win32 DebugCLEAN" "cartconv - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "WINMIPS" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\_vice.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\_vice.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib   /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\_vice.pdb" /debug /machine:MIPS /out:"$(OUTDIR)\_vice.exe" /pdbtype:sept 
LINK32_OBJS= \
	

!ENDIF 


!IF "$(CFG)" == "_vice - Win32 Release" || "$(CFG)" == "_vice - Win32 Debug"

!IF  "$(CFG)" == "_vice - Win32 Release"

"x64 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64.mak" CFG="x64 - Win32 Release" 
   cd "."

"x64 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64.mak" CFG="x64 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"x64 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64.mak" CFG="x64 - Win32 Debug" 
   cd "."

"x64 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64.mak" CFG="x64 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"x64dtv - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64dtv.mak" CFG="x64dtv - Win32 Release" 
   cd "."

"x64dtv - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64dtv.mak" CFG="x64dtv - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"x64dtv - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64dtv.mak" CFG="x64dtv - Win32 Debug" 
   cd "."

"x64dtv - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64dtv.mak" CFG="x64dtv - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"x64sc - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64sc.mak" CFG="x64sc - Win32 Release" 
   cd "."

"x64sc - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64sc.mak" CFG="x64sc - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"x64sc - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64sc.mak" CFG="x64sc - Win32 Debug" 
   cd "."

"x64sc - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x64sc.mak" CFG="x64sc - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"x128 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x128.mak" CFG="x128 - Win32 Release" 
   cd "."

"vsid - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vsid.mak" CFG="vsid - Win32 Release" 
   cd "."

"x128 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x128.mak" CFG="x128 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

"vsid - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vsid.mak" CFG="vsid - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"x128 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x128.mak" CFG="x128 - Win32 Debug" 
   cd "."

"vsid - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vsid.mak" CFG="vsid - Win32 Debug" 
   cd "."

"x128 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\x128.mak" CFG="x128 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

"vsid - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\vsid.mak" CFG="vsid - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"xcbm2 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xcbm2.mak" CFG="xcbm2 - Win32 Release" 
   cd "."

"xcbm5x0 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xcbm5x0.mak" CFG="xcbm5x0 - Win32 Release" 
   cd "."

"xcbm2 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xcbm2.mak" CFG="xcbm2 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

"xcbm5x0 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xcbm5x0.mak" CFG="xcbm5x0 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"xcbm2 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xcbm2.mak" CFG="xcbm2 - Win32 Debug" 
   cd "."

"xcbm5x0 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xcbm5x0.mak" CFG="xcbm5x0 - Win32 Debug" 
   cd "."

"xcbm2 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xcbm2.mak" CFG="xcbm2 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

"xcbm5x0 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xcbm5x0.mak" CFG="xcbm5x0 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"xpet - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xpet.mak" CFG="xpet - Win32 Release" 
   cd "."

"xpet - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xpet.mak" CFG="xpet - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"xpet - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xpet.mak" CFG="xpet - Win32 Debug" 
   cd "."

"xpet - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xpet.mak" CFG="xpet - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"xplus4 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xplus4.mak" CFG="xplus4 - Win32 Release" 
   cd "."

"xplus4 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xplus4.mak" CFG="xplus4 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"xplus4 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xplus4.mak" CFG="xplus4 - Win32 Debug" 
   cd "."

"xplus4 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xplus4.mak" CFG="xplus4 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"xvic - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xvic.mak" CFG="xvic - Win32 Release" 
   cd "."

"xvic - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xvic.mak" CFG="xvic - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"xvic - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xvic.mak" CFG="xvic - Win32 Debug" 
   cd "."

"xvic - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\xvic.mak" CFG="xvic - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"petcat - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\petcat.mak" CFG="petcat - Win32 Release" 
   cd "."

"petcat - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\petcat.mak" CFG="petcat - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"petcat - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\petcat.mak" CFG="petcat - Win32 Debug" 
   cd "."

"petcat - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\petcat.mak" CFG="petcat - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"c1541 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c1541.mak" CFG="c1541 - Win32 Release" 
   cd "."

"c1541 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c1541.mak" CFG="c1541 - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"c1541 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c1541.mak" CFG="c1541 - Win32 Debug" 
   cd "."

"c1541 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\c1541.mak" CFG="c1541 - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "_vice - Win32 Release"

"cartconv - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\cartconv.mak" CFG="cartconv - Win32 Release" 
   cd "."

"cartconv - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\cartconv.mak" CFG="cartconv - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "_vice - Win32 Debug"

"cartconv - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\cartconv.mak" CFG="cartconv - Win32 Debug" 
   cd "."

"cartconv - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\cartconv.mak" CFG="cartconv - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

