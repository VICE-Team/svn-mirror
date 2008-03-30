# Microsoft Developer Studio Project File - Name="plus4" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=plus4 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plus4.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plus4.mak" CFG="plus4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plus4 - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "plus4 - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plus4 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\plus4\Release"
# PROP Intermediate_Dir "libs\plus4\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\raster" /I "..\..\monitor" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "plus4 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\plus4\Debug"
# PROP Intermediate_Dir "libs\plus4\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\raster" /I "..\..\monitor" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

# Name "plus4 - Win32 Release"
# Name "plus4 - Win32 Debug"
# Begin Source File

SOURCE="..\..\plus4\plus4-cmdline-options.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\plus4-resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\plus4-snapshot.c"
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4cpu.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4datasette.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4iec.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4mem.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4memlimit.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4pio1.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4pio2.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\plus4rom.c
# End Source File
# Begin Source File

SOURCE=..\..\plus4\tcbm.c
# End Source File
# Begin Source File

SOURCE="..\..\plus4\ted-cmdline-options.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\ted-color.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\ted-draw.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\ted-mem.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\ted-resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\ted-snapshot.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\ted-sound.c"
# End Source File
# Begin Source File

SOURCE="..\..\plus4\ted-timer.c"
# End Source File
# Begin Source File

SOURCE=..\..\plus4\ted.c
# End Source File
# End Target
# End Project
