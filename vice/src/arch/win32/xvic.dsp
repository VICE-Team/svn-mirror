# Microsoft Developer Studio Project File - Name="xvic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
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
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i ".\MSVC" /i ".\\" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib comctl32.lib winmm.lib ddraw.lib dsound.lib dinput.lib /nologo /subsystem:windows /machine:I386

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i ".\MSVC" /i ".\\" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib comctl32.lib winmm.lib ddraw.lib dsound.lib dinput.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "xvic - Win32 Release"
# Name "xvic - Win32 Debug"
# Begin Source File

SOURCE=..\..\alarm.c
# End Source File
# Begin Source File

SOURCE=..\..\asm.c
# End Source File
# Begin Source File

SOURCE=..\..\attach.c
# End Source File
# Begin Source File

SOURCE=..\..\autostart.c
# End Source File
# Begin Source File

SOURCE=..\..\charsets.c
# End Source File
# Begin Source File

SOURCE=..\..\ciatimer.c
# End Source File
# Begin Source File

SOURCE=..\..\clkguard.c
# End Source File
# Begin Source File

SOURCE=..\..\cmdline.c
# End Source File
# Begin Source File

SOURCE=..\..\datasette.c
# End Source File
# Begin Source File

SOURCE=..\..\emuid.c
# End Source File
# Begin Source File

SOURCE=..\..\findpath.c
# End Source File
# Begin Source File

SOURCE=..\..\fliplist.c
# End Source File
# Begin Source File

SOURCE=..\..\fsdevice.c
# End Source File
# Begin Source File

SOURCE=..\..\gcr.c
# End Source File
# Begin Source File

SOURCE=..\..\imagecontents.c
# End Source File
# Begin Source File

SOURCE=..\..\info.c
# End Source File
# Begin Source File

SOURCE=..\..\interrupt.c
# End Source File
# Begin Source File

SOURCE=..\..\kbdbuf.c
# End Source File
# Begin Source File

SOURCE=..\..\log.c
# End Source File
# Begin Source File

SOURCE=..\..\main.c
# End Source File
# Begin Source File

SOURCE=..\..\maincpu.c
# End Source File
# Begin Source File

SOURCE=..\..\memutils.c
# End Source File
# Begin Source File

SOURCE=..\..\mon.c
# End Source File
# Begin Source File

SOURCE=..\..\mon_lex.c
# End Source File
# Begin Source File

SOURCE=..\..\mon_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\p00.c
# End Source File
# Begin Source File

SOURCE=..\..\palette.c
# End Source File
# Begin Source File

SOURCE=..\..\parallel.c
# End Source File
# Begin Source File

SOURCE=..\..\resources.c
# End Source File
# Begin Source File

SOURCE=.\resvic.rc

!IF  "$(CFG)" == "xvic - Win32 Release"

!ELSEIF  "$(CFG)" == "xvic - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\romset.c
# End Source File
# Begin Source File

SOURCE=..\..\serial.c
# End Source File
# Begin Source File

SOURCE=..\..\snapshot.c
# End Source File
# Begin Source File

SOURCE=..\..\sound.c
# End Source File
# Begin Source File

SOURCE=..\..\sysfile.c
# End Source File
# Begin Source File

SOURCE=..\..\t64.c
# End Source File
# Begin Source File

SOURCE=..\..\tap.c
# End Source File
# Begin Source File

SOURCE=..\..\tape.c
# End Source File
# Begin Source File

SOURCE=..\..\traps.c
# End Source File
# Begin Source File

SOURCE=..\..\utils.c
# End Source File
# Begin Source File

SOURCE="..\..\vdrive-iec.c"
# End Source File
# Begin Source File

SOURCE=..\..\vdrive.c
# End Source File
# Begin Source File

SOURCE=..\..\warn.c
# End Source File
# Begin Source File

SOURCE=..\..\zfile.c
# End Source File
# Begin Source File

SOURCE=..\..\zipcode.c
# End Source File
# End Target
# End Project
