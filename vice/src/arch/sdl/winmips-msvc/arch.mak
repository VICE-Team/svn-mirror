# Microsoft Developer Studio Generated NMAKE File, Based on arch.dsp
!IF "$(CFG)" == ""
CFG=arch - Win32 Debug
!MESSAGE No configuration specified. Defaulting to arch - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "arch - Win32 Release" && "$(CFG)" != "arch - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "arch - Win32 Release"

OUTDIR=.\libs\arch\Release
INTDIR=.\libs\arch\Release
# Begin Custom Macros
OutDir=.\libs\arch\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\arch.lib"

!ELSE 

ALL : "base - Win32 Release" "gentranslate - Win32 Release" "geninfocontrib - Win32 Release" "$(OUTDIR)\arch.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"geninfocontrib - Win32 ReleaseCLEAN" "gentranslate - Win32 ReleaseCLEAN" "base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\archdep.obj"
	-@erase "$(INTDIR)\blockdev.obj"
	-@erase "$(INTDIR)\catweaselmkiii.obj"
	-@erase "$(INTDIR)\console.obj"
	-@erase "$(INTDIR)\dynlib.obj"
	-@erase "$(INTDIR)\fullscreen.obj"
	-@erase "$(INTDIR)\hardsid.obj"
	-@erase "$(INTDIR)\joy.obj"
	-@erase "$(INTDIR)\kbd.obj"
	-@erase "$(INTDIR)\lightpendrv.obj"
	-@erase "$(INTDIR)\menu_c128hw.obj"
	-@erase "$(INTDIR)\menu_c64_common_expansions.obj"
	-@erase "$(INTDIR)\menu_c64_expansions.obj"
	-@erase "$(INTDIR)\menu_c64cart.obj"
	-@erase "$(INTDIR)\menu_c64dtvhw.obj"
	-@erase "$(INTDIR)\menu_c64hw.obj"
	-@erase "$(INTDIR)\menu_c64model.obj"
	-@erase "$(INTDIR)\menu_cbm2cart.obj"
	-@erase "$(INTDIR)\menu_cbm2hw.obj"
	-@erase "$(INTDIR)\menu_common.obj"
	-@erase "$(INTDIR)\menu_debug.obj"
	-@erase "$(INTDIR)\menu_drive.obj"
	-@erase "$(INTDIR)\menu_drive_rom.obj"
	-@erase "$(INTDIR)\menu_ffmpeg.obj"
	-@erase "$(INTDIR)\menu_help.obj"
	-@erase "$(INTDIR)\menu_joystick.obj"
	-@erase "$(INTDIR)\menu_lightpen.obj"
	-@erase "$(INTDIR)\menu_midi.obj"
	-@erase "$(INTDIR)\menu_mouse.obj"
	-@erase "$(INTDIR)\menu_network.obj"
	-@erase "$(INTDIR)\menu_petcart.obj"
	-@erase "$(INTDIR)\menu_pethw.obj"
	-@erase "$(INTDIR)\menu_plus4cart.obj"
	-@erase "$(INTDIR)\menu_plus4hw.obj"
	-@erase "$(INTDIR)\menu_printer.obj"
	-@erase "$(INTDIR)\menu_ram.obj"
	-@erase "$(INTDIR)\menu_reset.obj"
	-@erase "$(INTDIR)\menu_rom.obj"
	-@erase "$(INTDIR)\menu_rs232.obj"
	-@erase "$(INTDIR)\menu_screenshot.obj"
	-@erase "$(INTDIR)\menu_settings.obj"
	-@erase "$(INTDIR)\menu_sid.obj"
	-@erase "$(INTDIR)\menu_snapshot.obj"
	-@erase "$(INTDIR)\menu_sound.obj"
	-@erase "$(INTDIR)\menu_speed.obj"
	-@erase "$(INTDIR)\menu_tape.obj"
	-@erase "$(INTDIR)\menu_vic20cart.obj"
	-@erase "$(INTDIR)\menu_vic20hw.obj"
	-@erase "$(INTDIR)\menu_video.obj"
	-@erase "$(INTDIR)\mousedrv.obj"
	-@erase "$(INTDIR)\rs232.obj"
	-@erase "$(INTDIR)\rs232dev.obj"
	-@erase "$(INTDIR)\rs232net.obj"
	-@erase "$(INTDIR)\sdlmain.obj"
	-@erase "$(INTDIR)\signals.obj"
	-@erase "$(INTDIR)\ui.obj"
	-@erase "$(INTDIR)\uicmdline.obj"
	-@erase "$(INTDIR)\uifilereq.obj"
	-@erase "$(INTDIR)\uihotkey.obj"
	-@erase "$(INTDIR)\uimenu.obj"
	-@erase "$(INTDIR)\uimon.obj"
	-@erase "$(INTDIR)\uimsgbox.obj"
	-@erase "$(INTDIR)\uipause.obj"
	-@erase "$(INTDIR)\uipoll.obj"
	-@erase "$(INTDIR)\uistatusbar.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\video.obj"
	-@erase "$(INTDIR)\vkbd.obj"
	-@erase "$(INTDIR)\vsidui.obj"
	-@erase "$(INTDIR)\vsyncarch.obj"
	-@erase "$(INTDIR)\x128_ui.obj"
	-@erase "$(INTDIR)\x64_ui.obj"
	-@erase "$(INTDIR)\x64dtv_ui.obj"
	-@erase "$(INTDIR)\x64sc_ui.obj"
	-@erase "$(INTDIR)\xcbm2_ui.obj"
	-@erase "$(INTDIR)\xpet_ui.obj"
	-@erase "$(INTDIR)\xplus4_ui.obj"
	-@erase "$(INTDIR)\xvic_ui.obj"
	-@erase "$(OUTDIR)\arch.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\platform" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\userport" /I "..\..\..\c64" /I "..\..\..\c64\cart" /I "..\..\..\c64dtv" /I "..\..\..\c128" /I "..\..\..\vic20" /I "..\..\..\pet" /I "..\..\..\cbm2" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\lib\p64" /I "..\..\..\lib\libffmpeg" /I "..\..\..\plus4" /I "..\..\..\rs232drv" /I "..\..\..\sid" /I "..\..\..\imagecontents" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\arch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

.c{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\arch.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\arch.lib" 
LIB32_OBJS= \
	"$(INTDIR)\archdep.obj" \
	"$(INTDIR)\blockdev.obj" \
	"$(INTDIR)\catweaselmkiii.obj" \
	"$(INTDIR)\console.obj" \
	"$(INTDIR)\dynlib.obj" \
	"$(INTDIR)\fullscreen.obj" \
	"$(INTDIR)\hardsid.obj" \
	"$(INTDIR)\joy.obj" \
	"$(INTDIR)\kbd.obj" \
	"$(INTDIR)\lightpendrv.obj" \
	"$(INTDIR)\menu_c128hw.obj" \
	"$(INTDIR)\menu_c64_common_expansions.obj" \
	"$(INTDIR)\menu_c64_expansions.obj" \
	"$(INTDIR)\menu_c64cart.obj" \
	"$(INTDIR)\menu_c64dtvhw.obj" \
	"$(INTDIR)\menu_c64hw.obj" \
	"$(INTDIR)\menu_c64model.obj" \
	"$(INTDIR)\menu_cbm2cart.obj" \
	"$(INTDIR)\menu_cbm2hw.obj" \
	"$(INTDIR)\menu_common.obj" \
	"$(INTDIR)\menu_debug.obj" \
	"$(INTDIR)\menu_drive.obj" \
	"$(INTDIR)\menu_drive_rom.obj" \
	"$(INTDIR)\menu_ffmpeg.obj" \
	"$(INTDIR)\menu_help.obj" \
	"$(INTDIR)\menu_joystick.obj" \
	"$(INTDIR)\menu_lightpen.obj" \
	"$(INTDIR)\menu_midi.obj" \
	"$(INTDIR)\menu_mouse.obj" \
	"$(INTDIR)\menu_network.obj" \
	"$(INTDIR)\menu_petcart.obj" \
	"$(INTDIR)\menu_pethw.obj" \
	"$(INTDIR)\menu_plus4cart.obj" \
	"$(INTDIR)\menu_plus4hw.obj" \
	"$(INTDIR)\menu_printer.obj" \
	"$(INTDIR)\menu_ram.obj" \
	"$(INTDIR)\menu_reset.obj" \
	"$(INTDIR)\menu_rom.obj" \
	"$(INTDIR)\menu_rs232.obj" \
	"$(INTDIR)\menu_screenshot.obj" \
	"$(INTDIR)\menu_settings.obj" \
	"$(INTDIR)\menu_sid.obj" \
	"$(INTDIR)\menu_snapshot.obj" \
	"$(INTDIR)\menu_sound.obj" \
	"$(INTDIR)\menu_speed.obj" \
	"$(INTDIR)\menu_tape.obj" \
	"$(INTDIR)\menu_vic20cart.obj" \
	"$(INTDIR)\menu_vic20hw.obj" \
	"$(INTDIR)\menu_video.obj" \
	"$(INTDIR)\mousedrv.obj" \
	"$(INTDIR)\rs232.obj" \
	"$(INTDIR)\rs232dev.obj" \
	"$(INTDIR)\rs232net.obj" \
	"$(INTDIR)\sdlmain.obj" \
	"$(INTDIR)\signals.obj" \
	"$(INTDIR)\ui.obj" \
	"$(INTDIR)\uicmdline.obj" \
	"$(INTDIR)\uifilereq.obj" \
	"$(INTDIR)\uihotkey.obj" \
	"$(INTDIR)\uimenu.obj" \
	"$(INTDIR)\uimon.obj" \
	"$(INTDIR)\uimsgbox.obj" \
	"$(INTDIR)\uipause.obj" \
	"$(INTDIR)\uipoll.obj" \
	"$(INTDIR)\uistatusbar.obj" \
	"$(INTDIR)\video.obj" \
	"$(INTDIR)\vkbd.obj" \
	"$(INTDIR)\vsidui.obj" \
	"$(INTDIR)\vsyncarch.obj" \
	"$(INTDIR)\x128_ui.obj" \
	"$(INTDIR)\x64_ui.obj" \
	"$(INTDIR)\x64dtv_ui.obj" \
	"$(INTDIR)\x64sc_ui.obj" \
	"$(INTDIR)\xcbm2_ui.obj" \
	"$(INTDIR)\xpet_ui.obj" \
	"$(INTDIR)\xplus4_ui.obj" \
	"$(INTDIR)\xvic_ui.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\arch.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "arch - Win32 Debug"

OUTDIR=.\libs\arch\Debug
INTDIR=.\libs\arch\Debug
# Begin Custom Macros
OutDir=.\libs\arch\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\arch.lib"

!ELSE 

ALL : "base - Win32 Debug" "gentranslate - Win32 Debug" "geninfocontrib - Win32 Debug" "$(OUTDIR)\arch.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"geninfocontrib - Win32 DebugCLEAN" "gentranslate - Win32 DebugCLEAN" "base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\archdep.obj"
	-@erase "$(INTDIR)\blockdev.obj"
	-@erase "$(INTDIR)\catweaselmkiii.obj"
	-@erase "$(INTDIR)\console.obj"
	-@erase "$(INTDIR)\dynlib.obj"
	-@erase "$(INTDIR)\fullscreen.obj"
	-@erase "$(INTDIR)\hardsid.obj"
	-@erase "$(INTDIR)\joy.obj"
	-@erase "$(INTDIR)\kbd.obj"
	-@erase "$(INTDIR)\lightpendrv.obj"
	-@erase "$(INTDIR)\menu_c128hw.obj"
	-@erase "$(INTDIR)\menu_c64_common_expansions.obj"
	-@erase "$(INTDIR)\menu_c64_expansions.obj"
	-@erase "$(INTDIR)\menu_c64cart.obj"
	-@erase "$(INTDIR)\menu_c64dtvhw.obj"
	-@erase "$(INTDIR)\menu_c64hw.obj"
	-@erase "$(INTDIR)\menu_c64model.obj"
	-@erase "$(INTDIR)\menu_cbm2cart.obj"
	-@erase "$(INTDIR)\menu_cbm2hw.obj"
	-@erase "$(INTDIR)\menu_common.obj"
	-@erase "$(INTDIR)\menu_debug.obj"
	-@erase "$(INTDIR)\menu_drive.obj"
	-@erase "$(INTDIR)\menu_drive_rom.obj"
	-@erase "$(INTDIR)\menu_ffmpeg.obj"
	-@erase "$(INTDIR)\menu_help.obj"
	-@erase "$(INTDIR)\menu_joystick.obj"
	-@erase "$(INTDIR)\menu_lightpen.obj"
	-@erase "$(INTDIR)\menu_midi.obj"
	-@erase "$(INTDIR)\menu_mouse.obj"
	-@erase "$(INTDIR)\menu_network.obj"
	-@erase "$(INTDIR)\menu_petcart.obj"
	-@erase "$(INTDIR)\menu_pethw.obj"
	-@erase "$(INTDIR)\menu_plus4cart.obj"
	-@erase "$(INTDIR)\menu_plus4hw.obj"
	-@erase "$(INTDIR)\menu_printer.obj"
	-@erase "$(INTDIR)\menu_ram.obj"
	-@erase "$(INTDIR)\menu_reset.obj"
	-@erase "$(INTDIR)\menu_rom.obj"
	-@erase "$(INTDIR)\menu_rs232.obj"
	-@erase "$(INTDIR)\menu_screenshot.obj"
	-@erase "$(INTDIR)\menu_settings.obj"
	-@erase "$(INTDIR)\menu_sid.obj"
	-@erase "$(INTDIR)\menu_snapshot.obj"
	-@erase "$(INTDIR)\menu_sound.obj"
	-@erase "$(INTDIR)\menu_speed.obj"
	-@erase "$(INTDIR)\menu_tape.obj"
	-@erase "$(INTDIR)\menu_vic20cart.obj"
	-@erase "$(INTDIR)\menu_vic20hw.obj"
	-@erase "$(INTDIR)\menu_video.obj"
	-@erase "$(INTDIR)\mousedrv.obj"
	-@erase "$(INTDIR)\rs232.obj"
	-@erase "$(INTDIR)\rs232dev.obj"
	-@erase "$(INTDIR)\rs232net.obj"
	-@erase "$(INTDIR)\sdlmain.obj"
	-@erase "$(INTDIR)\signals.obj"
	-@erase "$(INTDIR)\ui.obj"
	-@erase "$(INTDIR)\uicmdline.obj"
	-@erase "$(INTDIR)\uifilereq.obj"
	-@erase "$(INTDIR)\uihotkey.obj"
	-@erase "$(INTDIR)\uimenu.obj"
	-@erase "$(INTDIR)\uimon.obj"
	-@erase "$(INTDIR)\uimsgbox.obj"
	-@erase "$(INTDIR)\uipause.obj"
	-@erase "$(INTDIR)\uipoll.obj"
	-@erase "$(INTDIR)\uistatusbar.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\video.obj"
	-@erase "$(INTDIR)\vkbd.obj"
	-@erase "$(INTDIR)\vsidui.obj"
	-@erase "$(INTDIR)\vsyncarch.obj"
	-@erase "$(INTDIR)\x128_ui.obj"
	-@erase "$(INTDIR)\x64_ui.obj"
	-@erase "$(INTDIR)\x64dtv_ui.obj"
	-@erase "$(INTDIR)\x64sc_ui.obj"
	-@erase "$(INTDIR)\xcbm2_ui.obj"
	-@erase "$(INTDIR)\xpet_ui.obj"
	-@erase "$(INTDIR)\xplus4_ui.obj"
	-@erase "$(INTDIR)\xvic_ui.obj"
	-@erase "$(OUTDIR)\arch.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\platform" /I "..\..\..\drive" /I "..\..\..\vdrive" /I "..\..\..\userport" /I "..\..\..\c64" /I "..\..\..\c64\cart" /I "..\..\..\c64dtv" /I "..\..\..\c128" /I "..\..\..\vic20" /I "..\..\..\pet" /I "..\..\..\cbm2" /I "..\..\..\raster" /I "..\..\..\monitor" /I "..\..\..\lib\p64" /I "..\..\..\lib\libffmpeg" /I "..\..\..\plus4" /I "..\..\..\rs232drv" /I "..\..\..\sid" /I "..\..\..\imagecontents" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\arch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

.c{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr :
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\arch.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\arch.lib" 
LIB32_OBJS= \
	"$(INTDIR)\archdep.obj" \
	"$(INTDIR)\blockdev.obj" \
	"$(INTDIR)\catweaselmkiii.obj" \
	"$(INTDIR)\console.obj" \
	"$(INTDIR)\dynlib.obj" \
	"$(INTDIR)\fullscreen.obj" \
	"$(INTDIR)\hardsid.obj" \
	"$(INTDIR)\joy.obj" \
	"$(INTDIR)\kbd.obj" \
	"$(INTDIR)\lightpendrv.obj" \
	"$(INTDIR)\menu_c128hw.obj" \
	"$(INTDIR)\menu_c64_common_expansions.obj" \
	"$(INTDIR)\menu_c64_expansions.obj" \
	"$(INTDIR)\menu_c64cart.obj" \
	"$(INTDIR)\menu_c64dtvhw.obj" \
	"$(INTDIR)\menu_c64hw.obj" \
	"$(INTDIR)\menu_c64model.obj" \
	"$(INTDIR)\menu_cbm2cart.obj" \
	"$(INTDIR)\menu_cbm2hw.obj" \
	"$(INTDIR)\menu_common.obj" \
	"$(INTDIR)\menu_debug.obj" \
	"$(INTDIR)\menu_drive.obj" \
	"$(INTDIR)\menu_drive_rom.obj" \
	"$(INTDIR)\menu_ffmpeg.obj" \
	"$(INTDIR)\menu_help.obj" \
	"$(INTDIR)\menu_joystick.obj" \
	"$(INTDIR)\menu_lightpen.obj" \
	"$(INTDIR)\menu_midi.obj" \
	"$(INTDIR)\menu_mouse.obj" \
	"$(INTDIR)\menu_network.obj" \
	"$(INTDIR)\menu_petcart.obj" \
	"$(INTDIR)\menu_pethw.obj" \
	"$(INTDIR)\menu_plus4cart.obj" \
	"$(INTDIR)\menu_plus4hw.obj" \
	"$(INTDIR)\menu_printer.obj" \
	"$(INTDIR)\menu_ram.obj" \
	"$(INTDIR)\menu_reset.obj" \
	"$(INTDIR)\menu_rom.obj" \
	"$(INTDIR)\menu_rs232.obj" \
	"$(INTDIR)\menu_screenshot.obj" \
	"$(INTDIR)\menu_settings.obj" \
	"$(INTDIR)\menu_sid.obj" \
	"$(INTDIR)\menu_snapshot.obj" \
	"$(INTDIR)\menu_sound.obj" \
	"$(INTDIR)\menu_speed.obj" \
	"$(INTDIR)\menu_tape.obj" \
	"$(INTDIR)\menu_vic20cart.obj" \
	"$(INTDIR)\menu_vic20hw.obj" \
	"$(INTDIR)\menu_video.obj" \
	"$(INTDIR)\mousedrv.obj" \
	"$(INTDIR)\rs232.obj" \
	"$(INTDIR)\rs232dev.obj" \
	"$(INTDIR)\rs232net.obj" \
	"$(INTDIR)\sdlmain.obj" \
	"$(INTDIR)\signals.obj" \
	"$(INTDIR)\ui.obj" \
	"$(INTDIR)\uicmdline.obj" \
	"$(INTDIR)\uifilereq.obj" \
	"$(INTDIR)\uihotkey.obj" \
	"$(INTDIR)\uimenu.obj" \
	"$(INTDIR)\uimon.obj" \
	"$(INTDIR)\uimsgbox.obj" \
	"$(INTDIR)\uipause.obj" \
	"$(INTDIR)\uipoll.obj" \
	"$(INTDIR)\uistatusbar.obj" \
	"$(INTDIR)\video.obj" \
	"$(INTDIR)\vkbd.obj" \
	"$(INTDIR)\vsidui.obj" \
	"$(INTDIR)\vsyncarch.obj" \
	"$(INTDIR)\x128_ui.obj" \
	"$(INTDIR)\x64_ui.obj" \
	"$(INTDIR)\x64dtv_ui.obj" \
	"$(INTDIR)\x64sc_ui.obj" \
	"$(INTDIR)\xcbm2_ui.obj" \
	"$(INTDIR)\xpet_ui.obj" \
	"$(INTDIR)\xplus4_ui.obj" \
	"$(INTDIR)\xvic_ui.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\arch.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "arch - Win32 Release" || "$(CFG)" == "arch - Win32 Debug"

!IF  "$(CFG)" == "arch - Win32 Release"

"geninfocontrib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\geninfocontrib.mak" CFG="geninfocontrib - Win32 Release" 
   cd "."

"geninfocontrib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\geninfocontrib.mak" CFG="geninfocontrib - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "arch - Win32 Debug"

"geninfocontrib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\geninfocontrib.mak" CFG="geninfocontrib - Win32 Debug" 
   cd "."

"geninfocontrib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\geninfocontrib.mak" CFG="geninfocontrib - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "arch - Win32 Release"

"gentranslate - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gentranslate.mak" CFG="gentranslate - Win32 Release" 
   cd "."

"gentranslate - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gentranslate.mak" CFG="gentranslate - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "arch - Win32 Debug"

"gentranslate - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gentranslate.mak" CFG="gentranslate - Win32 Debug" 
   cd "."

"gentranslate - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\gentranslate.mak" CFG="gentranslate - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "arch - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "arch - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE=..\archdep.c

"$(INTDIR)\archdep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\blockdev.c

"$(INTDIR)\blockdev.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\catweaselmkiii.c

"$(INTDIR)\catweaselmkiii.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\console.c

"$(INTDIR)\console.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dynlib.c

"$(INTDIR)\dynlib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\fullscreen.c

"$(INTDIR)\fullscreen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\hardsid.c

"$(INTDIR)\hardsid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\joy.c

"$(INTDIR)\joy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\kbd.c

"$(INTDIR)\kbd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\lightpendrv.c

"$(INTDIR)\lightpendrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_c128hw.c

"$(INTDIR)\menu_c128hw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_c64_common_expansions.c

"$(INTDIR)\menu_c64_common_expansions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_c64_expansions.c

"$(INTDIR)\menu_c64_expansions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_c64cart.c

"$(INTDIR)\menu_c64cart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_c64dtvhw.c

"$(INTDIR)\menu_c64dtvhw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_c64hw.c

"$(INTDIR)\menu_c64hw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_c64model.c

"$(INTDIR)\menu_c64model.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_cbm2cart.c

"$(INTDIR)\menu_cbm2cart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_cbm2hw.c

"$(INTDIR)\menu_cbm2hw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_common.c

"$(INTDIR)\menu_common.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_debug.c

"$(INTDIR)\menu_debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_drive.c

"$(INTDIR)\menu_drive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_drive_rom.c

"$(INTDIR)\menu_drive_rom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_ffmpeg.c

"$(INTDIR)\menu_ffmpeg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_help.c

"$(INTDIR)\menu_help.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_joystick.c

"$(INTDIR)\menu_joystick.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_lightpen.c

"$(INTDIR)\menu_lightpen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_midi.c

"$(INTDIR)\menu_midi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_mouse.c

"$(INTDIR)\menu_mouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_network.c

"$(INTDIR)\menu_network.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_petcart.c

"$(INTDIR)\menu_petcart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_pethw.c

"$(INTDIR)\menu_pethw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_plus4cart.c

"$(INTDIR)\menu_plus4cart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_plus4hw.c

"$(INTDIR)\menu_plus4hw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_printer.c

"$(INTDIR)\menu_printer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_ram.c

"$(INTDIR)\menu_ram.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_reset.c

"$(INTDIR)\menu_reset.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_rom.c

"$(INTDIR)\menu_rom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_rs232.c

"$(INTDIR)\menu_rs232.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_screenshot.c

"$(INTDIR)\menu_screenshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_settings.c

"$(INTDIR)\menu_settings.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_sid.c

"$(INTDIR)\menu_sid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_snapshot.c

"$(INTDIR)\menu_snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_sound.c

"$(INTDIR)\menu_sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_speed.c

"$(INTDIR)\menu_speed.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_tape.c

"$(INTDIR)\menu_tape.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_vic20cart.c

"$(INTDIR)\menu_vic20cart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_vic20hw.c

"$(INTDIR)\menu_vic20hw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\menu_video.c

"$(INTDIR)\menu_video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\mousedrv.c

"$(INTDIR)\mousedrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\rs232.c

"$(INTDIR)\rs232.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\rs232dev.c

"$(INTDIR)\rs232dev.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\rs232net.c

"$(INTDIR)\rs232net.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\sdlmain.c

"$(INTDIR)\sdlmain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\signals.c

"$(INTDIR)\signals.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\ui.c

"$(INTDIR)\ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uicmdline.c

"$(INTDIR)\uicmdline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uifilereq.c

"$(INTDIR)\uifilereq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uihotkey.c

"$(INTDIR)\uihotkey.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uimenu.c

"$(INTDIR)\uimenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uimon.c

"$(INTDIR)\uimon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uimsgbox.c

"$(INTDIR)\uimsgbox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uipause.c

"$(INTDIR)\uipause.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uipoll.c

"$(INTDIR)\uipoll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\uistatusbar.c

"$(INTDIR)\uistatusbar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\video.c

"$(INTDIR)\video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\vkbd.c

"$(INTDIR)\vkbd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\vsidui.c

"$(INTDIR)\vsidui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\vsyncarch.c

"$(INTDIR)\vsyncarch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\x128_ui.c

"$(INTDIR)\x128_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\x64_ui.c

"$(INTDIR)\x64_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\x64dtv_ui.c

"$(INTDIR)\x64dtv_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\x64sc_ui.c

"$(INTDIR)\x64sc_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\xcbm2_ui.c

"$(INTDIR)\xcbm2_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\xpet_ui.c

"$(INTDIR)\xpet_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\xplus4_ui.c

"$(INTDIR)\xplus4_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\xvic_ui.c

"$(INTDIR)\xvic_ui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

