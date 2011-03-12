# Microsoft Developer Studio Project File - Name="c64sc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=c64 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "c64sc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "c64sc.mak" CFG="c64sc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "c64sc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "c64sc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "c64sc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\c64sc\Release"
# PROP Intermediate_Dir "libs\c64sc\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\tape" /I "..\..\sid" /I "..\..\viciisc" /I "..\..\c64\cart" /I "..\..\raster" /I "..\..\monitor" /I "..\..\rs232drv" /D "WIN32" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "c64sc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\c64sc\Debug"
# PROP Intermediate_Dir "libs\c64sc\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\tape" /I "..\..\sid" /I "..\..\c64\cart" /I "..\..\viciisc" /I "..\..\raster" /I "..\..\monitor" /I "..\..\rs232drv" /D "WIN32" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

# Name "c64sc - Win32 Release"
# Name "c64sc - Win32 Debug"
# Begin Source File

SOURCE="..\..\c64\c64-cmdline-options.c"
# End Source File
# Begin Source File

SOURCE="..\..\c64\c64-resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\c64\c64-snapshot.c"
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64_256k.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64bus.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64cia1.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64cia2.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64cpu.c

!IF  "$(CFG)" == "c64sc - Win32 Release"

# ADD CPP /Ot /Oa /Ow /Oi /Op /Oy
# SUBTRACT CPP /Os

!ELSEIF  "$(CFG)" == "c64sc - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\c64\c64datasette.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64drive.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64export.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64fastiec.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64gluelogic.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64iec.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64io.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64keyboard.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64memsc.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64meminit.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64memlimit.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64memrom.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64memsnapshot.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64parallel.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64pla.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64printer.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64rom.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64romset.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64rsuser.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64scmodel.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64sound.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\c64video.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\patchrom.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\plus256k.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\plus60k.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\psid.c
# End Source File
# Begin Source File

SOURCE=..\..\c64\reloc65.c
# End Source File
# End Target
# End Project
