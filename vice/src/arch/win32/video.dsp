# Microsoft Developer Studio Project File - Name="video" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=video - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "video.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "video.mak" CFG="video - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "video - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "video - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "video - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\video\Release"
# PROP Intermediate_Dir "libs\video\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\c64" /I "..\..\raster" /I "..\..\video" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "video - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\video\Debug"
# PROP Intermediate_Dir "libs\video\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\c64" /I "..\..\raster" /I "..\..\video" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

# Name "video - Win32 Release"
# Name "video - Win32 Debug"
# Begin Source File

SOURCE="..\..\video\render1x1.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\render1x1pal.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\render1x2.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\render2x2.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\render2x2pal.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\video-canvas.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\video-cmdline-options.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\video-color.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\video-render.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\video-resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\video\video-viewport.c"
# End Source File
# End Target
# End Project
