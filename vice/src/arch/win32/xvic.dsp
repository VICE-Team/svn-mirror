# Microsoft Developer Studio Project File - Name="xvic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xvic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xvic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xvic.mak" CFG="xvic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xvic - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xvic - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xvic - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\data"
# PROP Intermediate_Dir "libs\xvic\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\vdrive" /I "..\..\monitor" /I "..\..\sid" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i ".\MSVC" /i ".\\" /i "..\..\\" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ddraw.lib dsound.lib ole32.lib dxguid.lib wsock32.lib version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "xvic - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\data"
# PROP Intermediate_Dir "libs\xvic\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\vdrive" /I "..\..\monitor" /I "..\..\sid" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i ".\MSVC" /i ".\\" /i "..\..\\" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dxguid.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib winmm.lib ddraw.lib dsound.lib ole32.lib wsock32.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xvic - Win32 Release"
# Name "xvic - Win32 Debug"
# Begin Source File

SOURCE=..\..\main.c
# End Source File
# Begin Source File

SOURCE=..\..\maincpu.c

!IF  "$(CFG)" == "xvic - Win32 Release"

# ADD CPP /Ot /Oa /Ow /Oi /Op /Oy
# SUBTRACT CPP /Os

!ELSEIF  "$(CFG)" == "xvic - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resvic20.rc

!IF  "$(CFG)" == "xvic - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__RESVI="..\..\debug.h"	"resvic20.rc"	"res.rc"	"resdrivevic20.rc"	"resrs232user.rc"	"ressidcart.rc"	"resmidi.rc"

# Begin Custom Build
InputPath=.\resvic20.rc

"resvic20cat.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /b resvic20.rc + res.rc + resdrivevic20.rc + resrs232user.rc + resmidi.rc + ressidcart.rc resvic20cat.rc /b

# End Custom Build

!ELSEIF  "$(CFG)" == "xvic - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RESVI="..\..\debug.h"	"resvic20.rc"	"res.rc"	"resdrivevic20.rc"	"resrs232user.rc"	"ressidcart.rc"	"resmidi.rc"

# Begin Custom Build
InputPath=.\resvic20.rc

"resvic20cat.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy /b resvic20.rc + res.rc + resdrivevic20.rc + resrs232user.rc + resmidi.rc + ressidcart.rc resvic20cat.rc /b

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resvic20cat.rc
# End Source File
# Begin Source File

SOURCE=.\vice.manifest
# End Source File
# End Target
# End Project
