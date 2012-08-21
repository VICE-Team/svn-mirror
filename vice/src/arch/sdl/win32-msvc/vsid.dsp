# Microsoft Developer Studio Project File - Name="vsid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=vsid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsid.mak" CFG="vsid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsid - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vsid - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsid - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\data"
# PROP Intermediate_Dir "libs\vsid\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\monitor" /I "..\..\..\lib\p64"  /I "..\..\..\platform" /D "WIN32" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ole32.lib wsock32.lib version.lib SDLmain.lib SDL.lib opengl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\data"
# PROP Intermediate_Dir "libs\vsid\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\monitor" /I "..\..\..\lib\p64"  /I "..\..\..\platform" /D "WIN32" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ole32.lib wsock32.lib version.lib SDLmain.lib SDL.lib opengl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /NODEFAULTLIB:msvcrt.lib

!ENDIF 

# Begin Target

# Name "vsid - Win32 Release"
# Name "vsid - Win32 Debug"
# Begin Source File

SOURCE=..\..\..\main.c
# End Source File
# Begin Source File

SOURCE=..\..\..\maincpu.c

!IF  "$(CFG)" == "vsid - Win32 Release"

# ADD CPP /Ot /Oa /Ow /Oi /Op /Oy /Ob2
# SUBTRACT CPP /Ox /Og /Os

!ELSEIF  "$(CFG)" == "vsid - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vice.manifest
# End Source File
# End Target
# End Project
