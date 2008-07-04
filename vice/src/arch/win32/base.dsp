# Microsoft Developer Studio Project File - Name="base" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=base - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "base.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "base.mak" CFG="base - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "base - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "base - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "base - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\base\Release"
# PROP Intermediate_Dir "libs\base\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\lib\zlib" /I "..\..\drive" /I "..\..\vdrive" /I "..\..\rs232drv" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\base\Debug"
# PROP Intermediate_Dir "libs\base\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\lib\zlib" /I "..\..\drive" /I "..\..\vdrive" /I "..\..\rs232drv" /D "DONT_USE_UNISTD_H" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

# Name "base - Win32 Release"
# Name "base - Win32 Debug"
# Begin Source File

SOURCE=..\..\translate.txt

!IF  "$(CFG)" == "base - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__RES_T="..\..\translate.txt"	
# Begin Custom Build - Generating translate.h and translate_table.h
InputDir=..\..
InputPath=..\..\translate.txt

"$(InputDir)\translate.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\..\data\gentranslate ..\..\translate.txt ..\..\translate.h ..\..\translate_table.h

# End Custom Build

!ELSEIF  "$(CFG)" == "base - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RES_T="..\..\translate.txt"

# Begin Custom Build - Generating translate.h and translate_table.h
InputDir=..\..
InputPath=..\..\translate.txt

"$(InputDir)\translate.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\..\data\gentranslate ..\..\translate.txt ..\..\translate.h ..\..\translate_table.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\alarm.c"
# End Source File
# Begin Source File

SOURCE="..\..\attach.c"
# End Source File
# Begin Source File

SOURCE="..\..\autostart.c"
# End Source File
# Begin Source File

SOURCE="..\..\charset.c"
# End Source File
# Begin Source File

SOURCE="..\..\cbmdos.c"
# End Source File
# Begin Source File

SOURCE="..\..\cbmimage.c"
# End Source File
# Begin Source File

SOURCE="..\..\clipboard.c"
# End Source File
# Begin Source File

SOURCE="..\..\clkguard.c"
# End Source File
# Begin Source File

SOURCE="..\..\cmdline.c"
# End Source File
# Begin Source File

SOURCE="..\..\color.c"
# End Source File
# Begin Source File

SOURCE="..\..\crc32.c"
# End Source File
# Begin Source File

SOURCE="..\..\datasette.c"
# End Source File
# Begin Source File

SOURCE="..\..\debug.c"
# End Source File
# Begin Source File

SOURCE="..\..\dma.c"
# End Source File
# Begin Source File

SOURCE="..\..\emuid.c"
# End Source File
# Begin Source File

SOURCE="..\..\event.c"
# End Source File
# Begin Source File

SOURCE="..\..\findpath.c"
# End Source File
# Begin Source File

SOURCE="..\..\fliplist.c"
# End Source File
# Begin Source File

SOURCE="..\..\gcr.c"
# End Source File
# Begin Source File

SOURCE="..\..\info.c"
# End Source File
# Begin Source File

SOURCE="..\..\init.c"
# End Source File
# Begin Source File

SOURCE="..\..\initcmdline.c"
# End Source File
# Begin Source File

SOURCE="..\..\interrupt.c"
# End Source File
# Begin Source File

SOURCE="..\..\ioutil.c"
# End Source File
# Begin Source File

SOURCE="..\..\joystick.c"
# End Source File
# Begin Source File

SOURCE="..\..\kbdbuf.c"
# End Source File
# Begin Source File

SOURCE="..\..\keyboard.c"
# End Source File
# Begin Source File

SOURCE="..\..\lib.c"
# End Source File
# Begin Source File

SOURCE="..\..\log.c"
# End Source File
# Begin Source File

SOURCE="..\..\machine-bus.c"
# End Source File
# Begin Source File

SOURCE="..\..\machine.c"
# End Source File
# Begin Source File

SOURCE="..\..\mouse.c"
# End Source File
# Begin Source File

SOURCE="..\..\network.c"
# End Source File
# Begin Source File

SOURCE="..\..\palette.c"
# End Source File
# Begin Source File

SOURCE="..\..\ram.c"
# End Source File
# Begin Source File

SOURCE="..\..\rawfile.c"
# End Source File
# Begin Source File

SOURCE="..\..\resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\romset.c"
# End Source File
# Begin Source File

SOURCE="..\..\screenshot.c"
# End Source File
# Begin Source File

SOURCE="..\..\snapshot.c"
# End Source File
# Begin Source File

SOURCE="..\..\sound.c"
# End Source File
# Begin Source File

SOURCE="..\..\sysfile.c"
# End Source File
# Begin Source File

SOURCE="..\..\translate.c"
# End Source File
# Begin Source File

SOURCE="..\..\traps.c"
# End Source File
# Begin Source File

SOURCE="..\..\util.c"
# End Source File
# Begin Source File

SOURCE="..\..\vsync.c"
# End Source File
# Begin Source File

SOURCE="..\..\zfile.c"
# End Source File
# Begin Source File

SOURCE="..\..\zipcode.c"
# End Source File
# End Target
# End Project
