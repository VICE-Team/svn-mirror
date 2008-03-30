# Microsoft Developer Studio Project File - Name="png" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=png - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "png.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "png.mak" CFG="png - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "png - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "png - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "png - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\png\Release"
# PROP Intermediate_Dir "libs\png\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\lib\lpng" /I "..\..\lib\zlib" /D "NDEBUG" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "_WINDOWS" /D "_CRT_SECURE_NO_DEPRECATE" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "png - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\png\Debug"
# PROP Intermediate_Dir "libs\png\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\lib\lpng" /I "..\..\lib\zlib" /D "_DEBUG" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "_WINDOWS" /D "_CRT_SECURE_NO_DEPRECATE" /YX /FD /c
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

# Name "png - Win32 Release"
# Name "png - Win32 Debug"
# Begin Source File

SOURCE="..\..\lib\lpng\png.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngerror.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngget.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngmem.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngpread.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngread.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngrio.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngrtran.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngrutil.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngset.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngtrans.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngwio.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngwrite.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngwtran.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib\lpng\pngwutil.c"
# End Source File
# End Target
# End Project
