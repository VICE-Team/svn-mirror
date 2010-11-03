# Microsoft Developer Studio Project File - Name="viciisc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=viciisc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "viciisc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "viciisc.mak" CFG="viciisc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "viciisc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "viciisc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "viciisc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\viciisc\Release"
# PROP Intermediate_Dir "libs\viciisc\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\c64dtv" /I "..\..\..\raster" /I "..\..\..\video" /I "..\..\..\monitor" /I "..\..\..\c64\cart" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "viciisc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\viciisc\Debug"
# PROP Intermediate_Dir "libs\viciisc\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\c64dtv" /I "..\..\..\raster" /I "..\..\..\video" /I "..\..\..\monitor" /I "..\..\..\c64\cart" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

# Name "viciisc - Win32 Release"
# Name "viciisc - Win32 Debug"
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-chip-model.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-cmdline-options.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-color.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-cycle.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-draw.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-draw-cycle.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-fetch.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-irq.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-lightpen.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-mem.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-phi1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-snapshot.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\viciisc\vicii-timing.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\viciisc\vicii.c
# End Source File
# End Target
# End Project
