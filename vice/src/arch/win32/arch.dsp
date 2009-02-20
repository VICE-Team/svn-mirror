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
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\vdrive" /I "..\..\c64" /I "..\..\c64dtv" /I "..\..\c128" /I "..\..\vic20" /I "..\..\pet" /I "..\..\cbm2" /I "..\..\raster" /I "..\..\monitor" /I "..\..\lib\libffmpeg" /I "..\..\plus4" /I "..\..\rs232drv" /I "..\..\sid" /I "..\..\imagecontents" /I ".\msvc\wpcap" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
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
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\vdrive" /I "..\..\c64" /I "..\..\c64dtv" /I "..\..\c128" /I "..\..\vic20" /I "..\..\pet" /I "..\..\cbm2" /I "..\..\raster" /I "..\..\monitor" /I "..\..\lib\libffmpeg" /I "..\..\plus4" /I "..\..\rs232drv" /I "..\..\sid" /I "..\..\imagecontents" /I ".\msvc\wpcap" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

SOURCE=.\archdep.c
# End Source File
# Begin Source File

SOURCE=.\blockdev.c
# End Source File
# Begin Source File

SOURCE=.\c128ui.c
# End Source File
# Begin Source File

SOURCE=.\c64dtvui.c
# End Source File
# Begin Source File

SOURCE=.\c64ui.c
# End Source File
# Begin Source File

SOURCE=.\catweaselmkiii.c
# End Source File
# Begin Source File

SOURCE=.\cbm2ui.c
# End Source File
# Begin Source File

SOURCE=.\console.c
# End Source File
# Begin Source File

SOURCE=.\dirent.c
# End Source File
# Begin Source File

SOURCE=.\ffmpeglib.c
# End Source File
# Begin Source File

SOURCE=.\fullscrn.c
# End Source File
# Begin Source File

SOURCE=.\fullscrn-ddraw.c
# End Source File
# Begin Source File

SOURCE=.\fullscrn-dx9.c
# End Source File
# Begin Source File

SOURCE=.\gifdrv.c
# End Source File
# Begin Source File

SOURCE=.\hardsid.c
# End Source File
# Begin Source File

SOURCE=.\intl.c
# End Source File
# Begin Source File

SOURCE=.\joy.c
# End Source File
# Begin Source File

SOURCE=.\kbd.c
# End Source File
# Begin Source File

SOURCE=.\mididrv.c
# End Source File
# Begin Source File

SOURCE=.\mousedrv.c
# End Source File
# Begin Source File

SOURCE=.\opencbmlib.c
# End Source File
# Begin Source File

SOURCE=.\parsid.c
# End Source File
# Begin Source File

SOURCE=.\petui.c
# End Source File
# Begin Source File

SOURCE=.\plus4ui.c
# End Source File
# Begin Source File

SOURCE=.\res.txt

!IF  "$(CFG)" == "arch - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__RES_T="res.txt"	"resdialogs.txt"	"resmenu.txt"	"resstrings.txt"	
# Begin Custom Build - Generating res.h
InputDir=.
InputPath=.\res.txt

"$(InputDir)\res.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\..\data\genwinres res.h menuid.h res.txt resdialogs.txt resmenu.txt resstrings.txt

# End Custom Build

!ELSEIF  "$(CFG)" == "arch - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__RES_T="res.txt"	"resdialogs.txt"	"resmenu.txt"	"resstrings.txt"	
# Begin Custom Build - Generating res.h
InputDir=.
InputPath=.\res.txt

"$(InputDir)\res.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\..\data\genwinres res.h menuid.h res.txt resdialogs.txt resmenu.txt resstrings.txt

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rs232.c
# End Source File
# Begin Source File

SOURCE=.\rs232dev.c
# End Source File
# Begin Source File

SOURCE=.\rs232net.c
# End Source File
# Begin Source File

SOURCE=.\signals.c
# End Source File
# Begin Source File

SOURCE=.\statusbar.c
# End Source File
# Begin Source File

SOURCE=.\system.c
# End Source File
# Begin Source File

SOURCE=.\tfearch.c
# End Source File
# Begin Source File

SOURCE=".\ui-resources.c"
# End Source File
# Begin Source File

SOURCE=.\ui.c
# End Source File
# Begin Source File

SOURCE=.\uiacia.c
# End Source File
# Begin Source File

SOURCE=.\uiattach.c
# End Source File
# Begin Source File

SOURCE=.\uic128.c
# End Source File
# Begin Source File

SOURCE=.\uic64_256k.c
# End Source File
# Begin Source File

SOURCE=.\uic64cart.c
# End Source File
# Begin Source File

SOURCE=.\uic64dtv.c
# End Source File
# Begin Source File

SOURCE=.\uicart.c
# End Source File
# Begin Source File

SOURCE=.\uicbm2set.c
# End Source File
# Begin Source File

SOURCE=.\uicmdline.c
# End Source File
# Begin Source File

SOURCE=.\uiconsole.c
# End Source File
# Begin Source File

SOURCE=.\uidatasette.c
# End Source File
# Begin Source File

SOURCE=.\uidigimax.c
# End Source File
# Begin Source File

SOURCE=.\uidqbb.c
# End Source File
# Begin Source File

SOURCE=.\uidrivec128.c
# End Source File
# Begin Source File

SOURCE=.\uidrivec64.c
# End Source File
# Begin Source File

SOURCE=.\uidrivepetcbm2.c
# End Source File
# Begin Source File

SOURCE=.\uidriveplus4.c
# End Source File
# Begin Source File

SOURCE=.\uidrivevic20.c
# End Source File
# Begin Source File

SOURCE=.\uievent.c
# End Source File
# Begin Source File

SOURCE=.\uifliplist.c
# End Source File
# Begin Source File

SOURCE=.\uigeoram.c
# End Source File
# Begin Source File

SOURCE=.\uihelp.c
# End Source File
# Begin Source File

SOURCE=.\uiide64.c
# End Source File
# Begin Source File

SOURCE=.\uiisepic.c
# End Source File
# Begin Source File

SOURCE=.\uijoystick.c
# End Source File
# Begin Source File

SOURCE=.\uikeyboard.c
# End Source File
# Begin Source File

SOURCE=.\uilib.c
# End Source File
# Begin Source File

SOURCE=.\uimediafile.c
# End Source File
# Begin Source File

SOURCE=.\uimidi.c
# End Source File
# Begin Source File

SOURCE=.\uimmc64.c
# End Source File
# Begin Source File

SOURCE=.\uimon.c
# End Source File
# Begin Source File

SOURCE=.\uimouse.c
# End Source File
# Begin Source File

SOURCE=.\uinetwork.c
# End Source File
# Begin Source File

SOURCE=.\uiperipheral.c
# End Source File
# Begin Source File

SOURCE=.\uipetreu.c
# End Source File
# Begin Source File

SOURCE=.\uipetset.c
# End Source File
# Begin Source File

SOURCE=.\uiplus256k.c
# End Source File
# Begin Source File

SOURCE=.\uiplus4cart.c
# End Source File
# Begin Source File

SOURCE=.\uiplus4mem.c
# End Source File
# Begin Source File

SOURCE=.\uiplus60k.c
# End Source File
# Begin Source File

SOURCE=.\uiquicksnapshot.c
# End Source File
# Begin Source File

SOURCE=.\uiram.c
# End Source File
# Begin Source File

SOURCE=.\uiramcart.c
# End Source File
# Begin Source File

SOURCE=.\uireu.c
# End Source File
# Begin Source File

SOURCE=.\uirom.c
# End Source File
# Begin Source File

SOURCE=.\uirs232.c
# End Source File
# Begin Source File

SOURCE=.\uirs232user.c
# End Source File
# Begin Source File

SOURCE=.\uisid.c
# End Source File
# Begin Source File

SOURCE=.\uisidcart.c
# End Source File
# Begin Source File

SOURCE=.\uisiddtv.c
# End Source File
# Begin Source File

SOURCE=.\uisnapshot.c
# End Source File
# Begin Source File

SOURCE=.\uisound.c
# End Source File
# Begin Source File

SOURCE=.\uispeed.c
# End Source File
# Begin Source File

SOURCE=.\uitfe.c
# End Source File
# Begin Source File

SOURCE=.\uivicii.c
# End Source File
# Begin Source File

SOURCE=.\uivicset.c
# End Source File
# Begin Source File

SOURCE=.\uivideo.c
# End Source File
# Begin Source File

SOURCE=.\vic20ui.c
# End Source File
# Begin Source File

SOURCE=.\video.c
# End Source File
# Begin Source File

SOURCE=.\video-ddraw.c
# End Source File
# Begin Source File

SOURCE=.\video-dx9.c
# End Source File
# Begin Source File

SOURCE=.\vsidui.c
# End Source File
# Begin Source File

SOURCE=.\vsyncarch.c
# End Source File
# Begin Source File

SOURCE=.\winmain.c
# End Source File
# End Target
# End Project
