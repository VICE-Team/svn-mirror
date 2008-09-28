# Microsoft Developer Studio Project File - Name="monitor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=monitor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "monitor.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "monitor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\monitor\Release"
# PROP Intermediate_Dir "libs\monitor\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\vdrive" /I "..\..\monitor" /I "..\..\drive" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "monitor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "monitor___Win32_Debug"
# PROP BASE Intermediate_Dir "monitor___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\monitor\Debug"
# PROP Intermediate_Dir "libs\monitor\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\vdrive" /I "..\..\monitor" /I "..\..\drive" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "monitor - Win32 Release"
# Name "monitor - Win32 Debug"
# Begin Source File

SOURCE=..\..\monitor\asm6502.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\asm6502dtv.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\asmz80.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_assemble6502.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_assemblez80.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_breakpoint.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_command.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_disassemble.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_drive.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_file.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_lex.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_lex.l
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_memory.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_parse.y
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_register6502.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_register6502dtv.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_registerz80.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_ui.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\mon_util.c
# End Source File
# Begin Source File

SOURCE=..\..\monitor\monitor.c
# End Source File
# End Target
# End Project
