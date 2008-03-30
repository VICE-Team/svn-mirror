# Microsoft Developer Studio Project File - Name="drive" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=drive - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "drive.mak".
!MESSAGE 
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

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "drive - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\drive\Release"
# PROP Intermediate_Dir "libs\drive\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "drive - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\drive\Debug"
# PROP Intermediate_Dir "libs\drive\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "drive - Win32 Release"
# Name "drive - Win32 Debug"
# Begin Source File

SOURCE=..\..\drive\cia1571drive0.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\cia1571drive1.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\cia1581drive0.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\cia1581drive1.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\drive.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\drivecpu0.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\drivecpu1.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\fdc.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\riot1drive0.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\riot2drive0.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\via1drive0.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\via1drive1.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\via2drive0.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\via2drive1.c
# End Source File
# Begin Source File

SOURCE=..\..\drive\wd1770.c
# End Source File
# End Target
# End Project
