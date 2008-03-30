# Microsoft Developer Studio Project File - Name="c64" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=c64 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "c64.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "c64.mak" CFG="c64 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "c64 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "c64 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "c64 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\c64\Release"
# PROP Intermediate_Dir "libs\c64\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\sid" /I "..\..\vicii" /I "..\..\raster" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "c64 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\c64\Debug"
# PROP Intermediate_Dir "libs\c64\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\sid" /I "..\..\vicii" /I "..\..\raster" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "c64 - Win32 Release"
# Name "c64 - Win32 Debug"
# Begin Source File

SOURCE=..\..\c64\c64.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64cartmem.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64cia1.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64cia2.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64datasette.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64iec.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64mem.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64tpi.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\cartridge.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\patchrom.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\psid.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\reu.c
# End Source File
# End Target
# End Project
