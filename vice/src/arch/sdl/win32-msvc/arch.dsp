# Microsoft Developer Studio Project File - Name="arch" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=arch - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "arch.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "arch.mak" CFG="arch - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "arch - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "arch - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "arch - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\arch\Release"
# PROP Intermediate_Dir "libs\arch\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\c64" /I "..\..\..\c64\cart" /I "..\..\..\c64dtv" /I "..\..\..\c128" /I "..\..\..\vic20" /I "..\..\..\pet" /I "..\..\..\cbm2" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\lib\libffmpeg" /I "..\..\..\plus4" /I "..\..\..\rs232drv" /I "..\..\..\sid" /I "..\..\..\imagecontents" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "arch - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\arch\Debug"
# PROP Intermediate_Dir "libs\arch\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\c64" /I "..\..\..\c64\cart" /I "..\..\..\c64dtv" /I "..\..\..\c128" /I "..\..\..\vic20" /I "..\..\..\pet" /I "..\..\..\cbm2" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\lib\libffmpeg" /I "..\..\..\plus4" /I "..\..\..\rs232drv" /I "..\..\..\sid" /I "..\..\..\imagecontents" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

# Name "arch - Win32 Release"
# Name "arch - Win32 Debug"
# Begin Source File

SOURCE=..\archdep.c
# End Source File
# Begin Source File

SOURCE=..\blockdev.c
# End Source File
# Begin Source File

SOURCE=..\catweaselmkiii.c
# End Source File
# Begin Source File

SOURCE=..\console.c
# End Source File
# Begin Source File

SOURCE=..\dynlib.c
# End Source File
# Begin Source File

SOURCE=..\fullscreen.c
# End Source File
# Begin Source File

SOURCE=..\hardsid.c
# End Source File
# Begin Source File

SOURCE=..\joy.c
# End Source File
# Begin Source File

SOURCE=..\kbd.c
# End Source File
# Begin Source File

SOURCE=..\lightpendrv.c
# End Source File
# Begin Source File

SOURCE=..\menu_c128hw.c
# End Source File
# Begin Source File

SOURCE=..\menu_c64_common_expansions.c
# End Source File
# Begin Source File

SOURCE=..\menu_c64_expansions.c
# End Source File
# Begin Source File

SOURCE=..\menu_c64cart.c
# End Source File
# Begin Source File

SOURCE=..\menu_c64dtvhw.c
# End Source File
# Begin Source File

SOURCE=..\menu_c64hw.c
# End Source File
# Begin Source File

SOURCE=..\menu_cbm2cart.c
# End Source File
# Begin Source File

SOURCE=..\menu_cbm2hw.c
# End Source File
# Begin Source File

SOURCE=..\menu_common.c
# End Source File
# Begin Source File

SOURCE=..\menu_debug.c
# End Source File
# Begin Source File

SOURCE=..\menu_drive.c
# End Source File
# Begin Source File

SOURCE=..\menu_drive_rom.c
# End Source File
# Begin Source File

SOURCE=..\menu_help.c
# End Source File
# Begin Source File

SOURCE=..\menu_joystick.c
# End Source File
# Begin Source File

SOURCE=..\menu_lightpen.c
# End Source File
# Begin Source File

SOURCE=..\menu_midi.c
# End Source File
# Begin Source File

SOURCE=..\menu_mouse.c
# End Source File
# Begin Source File

SOURCE=..\menu_network.c
# End Source File
# Begin Source File

SOURCE=..\menu_petcart.c
# End Source File
# Begin Source File

SOURCE=..\menu_pethw.c
# End Source File
# Begin Source File

SOURCE=..\menu_plus4cart.c
# End Source File
# Begin Source File

SOURCE=..\menu_plus4hw.c
# End Source File
# Begin Source File

SOURCE=..\menu_ram.c
# End Source File
# Begin Source File

SOURCE=..\menu_reset.c
# End Source File
# Begin Source File

SOURCE=..\menu_rom.c
# End Source File
# Begin Source File

SOURCE=..\menu_screenshot.c
# End Source File
# Begin Source File

SOURCE=..\menu_settings.c
# End Source File
# Begin Source File

SOURCE=..\menu_sid.c
# End Source File
# Begin Source File

SOURCE=..\menu_snapshot.c
# End Source File
# Begin Source File

SOURCE=..\menu_sound.c
# End Source File
# Begin Source File

SOURCE=..\menu_speed.c
# End Source File
# Begin Source File

SOURCE=..\menu_tape.c
# End Source File
# Begin Source File

SOURCE=..\menu_vic20cart.c
# End Source File
# Begin Source File

SOURCE=..\menu_vic20hw.c
# End Source File
# Begin Source File

SOURCE=..\menu_video.c
# End Source File
# Begin Source File

SOURCE=..\mousedrv.c
# End Source File
# Begin Source File

SOURCE=..\sdlmain.c
# End Source File
# Begin Source File

SOURCE=..\signals.c
# End Source File
# Begin Source File

SOURCE=..\ui.c
# End Source File
# Begin Source File

SOURCE=..\uicmdline.c
# End Source File
# Begin Source File

SOURCE=..\uifilereq.c
# End Source File
# Begin Source File

SOURCE=..\uihotkey.c
# End Source File
# Begin Source File

SOURCE=..\uimenu.c
# End Source File
# Begin Source File

SOURCE=..\uimon.c
# End Source File
# Begin Source File

SOURCE=..\uimsgbox.c
# End Source File
# Begin Source File

SOURCE=..\uipause.c
# End Source File
# Begin Source File

SOURCE=..\uipoll.c
# End Source File
# Begin Source File

SOURCE=..\uistatusbar.c
# End Source File
# Begin Source File

SOURCE=..\video.c
# End Source File
# Begin Source File

SOURCE=..\vkbd.c
# End Source File
# Begin Source File

SOURCE=..\vsidui.c
# End Source File
# Begin Source File

SOURCE=..\vsyncarch.c
# End Source File
# Begin Source File

SOURCE=..\x128_ui.c
# End Source File
# Begin Source File

SOURCE=..\x64_ui.c
# End Source File
# Begin Source File

SOURCE=..\x64dtv_ui.c
# End Source File
# Begin Source File

SOURCE=..\xcbm2_ui.c
# End Source File
# Begin Source File

SOURCE=..\xpet_ui.c
# End Source File
# Begin Source File

SOURCE=..\xplus4_ui.c
# End Source File
# Begin Source File

SOURCE=..\xvic_ui.c
# End Source File
# End Target
# End Project
