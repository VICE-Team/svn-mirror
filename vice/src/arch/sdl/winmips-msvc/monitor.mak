# Microsoft Developer Studio Generated NMAKE File, Based on monitor.dsp
!IF "$(CFG)" == ""
CFG=monitor - Win32 Debug
!MESSAGE No configuration specified. Defaulting to monitor - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "monitor - Win32 Release" && "$(CFG)" != "monitor - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "monitor.mak" CFG="monitor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "monitor - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "monitor - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "monitor - Win32 Release"

OUTDIR=.\libs\monitor\Release
INTDIR=.\libs\monitor\Release
# Begin Custom Macros
OutDir=.\libs\monitor\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\monitor.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\monitor.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\asm6502.obj"
	-@erase "$(INTDIR)\asm6502dtv.obj"
	-@erase "$(INTDIR)\asm6809.obj"
	-@erase "$(INTDIR)\asmz80.obj"
	-@erase "$(INTDIR)\mon_assemble6502.obj"
	-@erase "$(INTDIR)\mon_assemble6809.obj"
	-@erase "$(INTDIR)\mon_assemblez80.obj"
	-@erase "$(INTDIR)\mon_breakpoint.obj"
	-@erase "$(INTDIR)\mon_command.obj"
	-@erase "$(INTDIR)\mon_disassemble.obj"
	-@erase "$(INTDIR)\mon_drive.obj"
	-@erase "$(INTDIR)\mon_file.obj"
	-@erase "$(INTDIR)\mon_lex.obj"
	-@erase "$(INTDIR)\mon_memory.obj"
	-@erase "$(INTDIR)\mon_parse.obj"
	-@erase "$(INTDIR)\mon_register6502.obj"
	-@erase "$(INTDIR)\mon_register6502dtv.obj"
	-@erase "$(INTDIR)\mon_register6809.obj"
	-@erase "$(INTDIR)\mon_registerz80.obj"
	-@erase "$(INTDIR)\mon_ui.obj"
	-@erase "$(INTDIR)\mon_util.obj"
	-@erase "$(INTDIR)\monitor.obj"
	-@erase "$(INTDIR)\monitor_network.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\monitor.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vdrive" /I "..\..\..\monitor" /I "..\..\..\lib\p64" /I "..\..\..\drive" /I "..\..\..\imagecontents" /D "NDEBUG" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_WINDOWS" /Fp"$(INTDIR)\monitor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\monitor.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\monitor.lib" 
LIB32_OBJS= \
	"$(INTDIR)\asm6502.obj" \
	"$(INTDIR)\asm6502dtv.obj" \
	"$(INTDIR)\asm6809.obj" \
	"$(INTDIR)\asmz80.obj" \
	"$(INTDIR)\mon_assemble6502.obj" \
	"$(INTDIR)\mon_assemble6809.obj" \
	"$(INTDIR)\mon_assemblez80.obj" \
	"$(INTDIR)\mon_breakpoint.obj" \
	"$(INTDIR)\mon_command.obj" \
	"$(INTDIR)\mon_disassemble.obj" \
	"$(INTDIR)\mon_drive.obj" \
	"$(INTDIR)\mon_file.obj" \
	"$(INTDIR)\mon_lex.obj" \
	"$(INTDIR)\mon_memory.obj" \
	"$(INTDIR)\mon_parse.obj" \
	"$(INTDIR)\mon_register6502.obj" \
	"$(INTDIR)\mon_register6502dtv.obj" \
	"$(INTDIR)\mon_register6809.obj" \
	"$(INTDIR)\mon_registerz80.obj" \
	"$(INTDIR)\mon_ui.obj" \
	"$(INTDIR)\mon_util.obj" \
	"$(INTDIR)\monitor.obj" \
	"$(INTDIR)\monitor_network.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\monitor.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "monitor - Win32 Debug"

OUTDIR=.\libs\monitor\Debug
INTDIR=.\libs\monitor\Debug
# Begin Custom Macros
OutDir=.\libs\monitor\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\monitor.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\monitor.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\asm6502.obj"
	-@erase "$(INTDIR)\asm6502dtv.obj"
	-@erase "$(INTDIR)\asm6809.obj"
	-@erase "$(INTDIR)\asmz80.obj"
	-@erase "$(INTDIR)\mon_assemble6502.obj"
	-@erase "$(INTDIR)\mon_assemble6809.obj"
	-@erase "$(INTDIR)\mon_assemblez80.obj"
	-@erase "$(INTDIR)\mon_breakpoint.obj"
	-@erase "$(INTDIR)\mon_command.obj"
	-@erase "$(INTDIR)\mon_disassemble.obj"
	-@erase "$(INTDIR)\mon_drive.obj"
	-@erase "$(INTDIR)\mon_file.obj"
	-@erase "$(INTDIR)\mon_lex.obj"
	-@erase "$(INTDIR)\mon_memory.obj"
	-@erase "$(INTDIR)\mon_parse.obj"
	-@erase "$(INTDIR)\mon_register6502.obj"
	-@erase "$(INTDIR)\mon_register6502dtv.obj"
	-@erase "$(INTDIR)\mon_register6809.obj"
	-@erase "$(INTDIR)\mon_registerz80.obj"
	-@erase "$(INTDIR)\mon_ui.obj"
	-@erase "$(INTDIR)\mon_util.obj"
	-@erase "$(INTDIR)\monitor.obj"
	-@erase "$(INTDIR)\monitor_network.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\monitor.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vdrive" /I "..\..\..\monitor" /I "..\..\..\lib\p64" /I "..\..\..\drive" /I "..\..\..\imagecontents" /D "_DEBUG" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_WINDOWS" /Fp"$(INTDIR)\monitor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\monitor.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\monitor.lib" 
LIB32_OBJS= \
	"$(INTDIR)\asm6502.obj" \
	"$(INTDIR)\asm6502dtv.obj" \
	"$(INTDIR)\asm6809.obj" \
	"$(INTDIR)\asmz80.obj" \
	"$(INTDIR)\mon_assemble6502.obj" \
	"$(INTDIR)\mon_assemble6809.obj" \
	"$(INTDIR)\mon_assemblez80.obj" \
	"$(INTDIR)\mon_breakpoint.obj" \
	"$(INTDIR)\mon_command.obj" \
	"$(INTDIR)\mon_disassemble.obj" \
	"$(INTDIR)\mon_drive.obj" \
	"$(INTDIR)\mon_file.obj" \
	"$(INTDIR)\mon_lex.obj" \
	"$(INTDIR)\mon_memory.obj" \
	"$(INTDIR)\mon_parse.obj" \
	"$(INTDIR)\mon_register6502.obj" \
	"$(INTDIR)\mon_register6502dtv.obj" \
	"$(INTDIR)\mon_register6809.obj" \
	"$(INTDIR)\mon_registerz80.obj" \
	"$(INTDIR)\mon_ui.obj" \
	"$(INTDIR)\mon_util.obj" \
	"$(INTDIR)\monitor.obj" \
	"$(INTDIR)\monitor_network.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\monitor.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "monitor - Win32 Release" || "$(CFG)" == "monitor - Win32 Debug"

!IF  "$(CFG)" == "monitor - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "monitor - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\..\..\monitor\asm6502.c

"$(INTDIR)\asm6502.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\asm6502dtv.c

"$(INTDIR)\asm6502dtv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\asm6809.c

"$(INTDIR)\asm6809.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\asmz80.c

"$(INTDIR)\asmz80.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_assemble6502.c

"$(INTDIR)\mon_assemble6502.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_assemble6809.c

"$(INTDIR)\mon_assemble6809.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_assemblez80.c

"$(INTDIR)\mon_assemblez80.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_breakpoint.c

"$(INTDIR)\mon_breakpoint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_command.c

"$(INTDIR)\mon_command.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_disassemble.c

"$(INTDIR)\mon_disassemble.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_drive.c

"$(INTDIR)\mon_drive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_file.c

"$(INTDIR)\mon_file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_lex.c

"$(INTDIR)\mon_lex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_memory.c

"$(INTDIR)\mon_memory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_parse.c

!IF  "$(CFG)" == "monitor - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vdrive" /I "..\..\..\monitor" /I "..\..\..\drive" /I "..\..\..\imagecontents" /D "NDEBUG" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_WINDOWS" /D YYMALLOC=malloc /D YYFREE=free /Fp"$(INTDIR)\monitor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

"$(INTDIR)\mon_parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "monitor - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\vdrive" /I "..\..\..\monitor" /I "..\..\..\drive" /I "..\..\..\imagecontents" /D "_DEBUG" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_WINDOWS" /D YYMALLOC=malloc /D YYFREE=free /Fp"$(INTDIR)\monitor.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

"$(INTDIR)\mon_parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\monitor\mon_register6502.c

"$(INTDIR)\mon_register6502.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_register6502dtv.c

"$(INTDIR)\mon_register6502dtv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_register6809.c

"$(INTDIR)\mon_register6809.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_registerz80.c

"$(INTDIR)\mon_registerz80.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_ui.c

"$(INTDIR)\mon_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\mon_util.c

"$(INTDIR)\mon_util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\monitor.c

"$(INTDIR)\monitor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\monitor\monitor_network.c

"$(INTDIR)\monitor_network.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

