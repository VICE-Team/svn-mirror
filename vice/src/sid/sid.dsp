# Microsoft Developer Studio Project File - Name="sid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sid.mak" CFG="sid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sid - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sid - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "sid - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\arch\win32\msvc" /I "..\arch\win32" /I "..\\" /I "..\resid" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "sid - Win32 Debug"

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
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\arch\win32\msvc" /I "..\arch\win32" /I "..\\" /I "..\resid" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "sid - Win32 Release"
# Name "sid - Win32 Debug"
# Begin Source File

SOURCE=.\resid.cc

!IF  "$(CFG)" == "sid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\resid.cc
InputName=resid

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /I "..\arch\win32\msvc" /I "..\arch\win32"\
 /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\resid.pch"   /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "sid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\resid.cc
InputName=resid

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /I "..\arch\win32\msvc" /I "..\arch\win32"\
 /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\resid.pch"   /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP  "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sid.c
# End Source File
# End Target
# End Project
