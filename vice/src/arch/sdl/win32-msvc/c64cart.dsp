# Microsoft Developer Studio Project File - Name="c64cart" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=c64cart - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "c64cart.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "c64cart.mak" CFG="c64cart - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "c64cart - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "c64cart - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "c64cart - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\c64cart\Release"
# PROP Intermediate_Dir "libs\c64cart\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\rtc" /I "..\..\..\vicii" /I "..\..\..\drive" /I "..\..\..\sid" /I "..\..\..\monitor" /I "..\..\..\core" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "c64cart - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\c64cart\Debug"
# PROP Intermediate_Dir "libs\c64cart\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\rtc" /I "..\..\..\vicii" /I "..\..\..\drive" /I "..\..\..\sid" /I "..\..\..\monitor" /I "..\..\..\core" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

# Name "c64cart - Win32 Release"
# Name "c64cart - Win32 Debug"
# Begin Source File

SOURCE="..\..\..\c64\cart\actionreplay.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\actionreplay3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\actionreplay4.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\atomicpower.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\c64-midi.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\c64acia1.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\c64cart.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\c64cartmem.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\c64tpi.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\capture.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\comal80.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\crt.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\delaep256.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\delaep64.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\delaep7x8.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\digimax.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\dinamic.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\dqbb.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\easyflash.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\epyxfastload.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\exos.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\expert.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\final.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\final3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\finalplus.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\fmopl.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\freezeframe.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\freezemachine.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\funplay.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\gamekiller.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\generic.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\georam.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\gs.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\ide64.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\isepic.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\kcs.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\magicdesk.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\magicformel.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\magicvoice.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\mmc64.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\mmcreplay.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\mikroass.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\ocean.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\prophet64.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\ramcart.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\retroreplay.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\reu.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\rexep256.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\rexutility.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\ross.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\ser-eeprom.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\sfx_soundexpander.c
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\sfx_soundsampler.c
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\simonsbasic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\snapshot64.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\spi-sdcard.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\stardos.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\stb.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\superexplode5.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\supergames.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\supersnapshot.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\supersnapshot4.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\c64\cart\tfe.c
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\warpspeed.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\westermann.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\c64\cart\zaxxon.c"
# End Source File
# End Target
# End Project
