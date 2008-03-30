# Microsoft Developer Studio Project File - Name="c128" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=c128 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "c128.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "c128.mak" CFG="c128 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "c128 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "c128 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "c128 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\arch\win32\msvc" /I "..\arch\win32" /I "..\\" /I "..\drive" /I "..\c64" /I "..\sid" /I "..\vicii" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "c128 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\arch\win32\msvc" /I "..\arch\win32" /I "..\\" /I "..\drive" /I "..\c64" /I "..\sid" /I "..\vicii" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "c128 - Win32 Release"
# Name "c128 - Win32 Debug"
# Begin Source File

SOURCE=.\c128.c
# End Source File
# Begin Source File

SOURCE=.\c128cia1.c
# End Source File
# Begin Source File

SOURCE=.\c128cpu.c
# End Source File
# Begin Source File

SOURCE=.\c128iec.c

!IF  "$(CFG)" == "c128 - Win32 Release"

# ADD CPP /Ob1

!ELSEIF  "$(CFG)" == "c128 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\c128mem.c

!IF  "$(CFG)" == "c128 - Win32 Release"

# ADD CPP /Oy /Ob1
# SUBTRACT CPP /O<none>

!ELSEIF  "$(CFG)" == "c128 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vdc.c
# End Source File
# End Target
# End Project
