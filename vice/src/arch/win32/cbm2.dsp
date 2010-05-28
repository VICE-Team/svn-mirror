# Microsoft Developer Studio Project File - Name="cbm2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=cbm2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cbm2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cbm2.mak" CFG="cbm2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cbm2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cbm2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cbm2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\cbm2\Release"
# PROP Intermediate_Dir "libs\cbm2\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\tape" /I "..\..\sid" /I "..\..\crtc" /I "..\..\vicii" /I "..\..\raster" /I "..\..\monitor" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cbm2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\cbm2\Debug"
# PROP Intermediate_Dir "libs\cbm2\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\tape" /I "..\..\sid" /I "..\..\crtc" /I "..\..\vicii" /I "..\..\raster" /I "..\..\monitor" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "cbm2 - Win32 Release"
# Name "cbm2 - Win32 Debug"
# Begin Source File

SOURCE="..\..\cbm2\cbm2-cmdline-options.c"
# End Source File
# Begin Source File

SOURCE="..\..\cbm2\cbm2-resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\cbm2\cbm2-snapshot.c"
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2acia1.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2bus.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2cia1.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2cpu.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2datasette.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2drive.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2iec.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2mem.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2memsnapshot.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2printer.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2rom.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2romset.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2sound.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2tpi1.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2tpi2.c
# End Source File
# Begin Source File

SOURCE=..\..\cbm2\cbm2video.c
# End Source File
# End Target
# End Project
