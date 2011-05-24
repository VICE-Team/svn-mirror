# Microsoft Developer Studio Generated NMAKE File, Based on c64cart.dsp
!IF "$(CFG)" == ""
CFG=c64cart - Win32 Debug
!MESSAGE No configuration specified. Defaulting to c64cart - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "c64cart - Win32 Release" && "$(CFG)" != "c64cart - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "c64cart - Win32 Release"

OUTDIR=.\libs\c64cart\Release
INTDIR=.\libs\c64cart\Release
# Begin Custom Macros
OutDir=.\libs\c64cart\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c64cart.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\c64cart.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\actionreplay.obj"
	-@erase "$(INTDIR)\actionreplay2.obj"
	-@erase "$(INTDIR)\actionreplay3.obj"
	-@erase "$(INTDIR)\actionreplay4.obj"
	-@erase "$(INTDIR)\atomicpower.obj"
	-@erase "$(INTDIR)\c64-generic.obj"
	-@erase "$(INTDIR)\c64-midi.obj"
	-@erase "$(INTDIR)\c64acia1.obj"
	-@erase "$(INTDIR)\c64tpi.obj"
	-@erase "$(INTDIR)\capture.obj"
	-@erase "$(INTDIR)\comal80.obj"
	-@erase "$(INTDIR)\delaep256.obj"
	-@erase "$(INTDIR)\delaep64.obj"
	-@erase "$(INTDIR)\delaep7x8.obj"
	-@erase "$(INTDIR)\diashowmaker.obj"
	-@erase "$(INTDIR)\digimax.obj"
	-@erase "$(INTDIR)\dinamic.obj"
	-@erase "$(INTDIR)\dqbb.obj"
	-@erase "$(INTDIR)\easyflash.obj"
	-@erase "$(INTDIR)\epyxfastload.obj"
	-@erase "$(INTDIR)\exos.obj"
	-@erase "$(INTDIR)\expert.obj"
	-@erase "$(INTDIR)\final.obj"
	-@erase "$(INTDIR)\final3.obj"
	-@erase "$(INTDIR)\finalplus.obj"
	-@erase "$(INTDIR)\freezeframe.obj"
	-@erase "$(INTDIR)\freezemachine.obj"
	-@erase "$(INTDIR)\funplay.obj"
	-@erase "$(INTDIR)\gamekiller.obj"
	-@erase "$(INTDIR)\gs.obj"
	-@erase "$(INTDIR)\ide64.obj"
	-@erase "$(INTDIR)\isepic.obj"
	-@erase "$(INTDIR)\kcs.obj"
	-@erase "$(INTDIR)\mach5.obj"
	-@erase "$(INTDIR)\magicdesk.obj"
	-@erase "$(INTDIR)\magicformel.obj"
	-@erase "$(INTDIR)\magicvoice.obj"
	-@erase "$(INTDIR)\mikroass.obj"
	-@erase "$(INTDIR)\mmc64.obj"
	-@erase "$(INTDIR)\mmcreplay.obj"
	-@erase "$(INTDIR)\ocean.obj"
	-@erase "$(INTDIR)\prophet64.obj"
	-@erase "$(INTDIR)\ramcart.obj"
	-@erase "$(INTDIR)\retroreplay.obj"
	-@erase "$(INTDIR)\reu.obj"
	-@erase "$(INTDIR)\rexep256.obj"
	-@erase "$(INTDIR)\rexutility.obj"
	-@erase "$(INTDIR)\ross.obj"
	-@erase "$(INTDIR)\sfx_soundexpander.obj"
	-@erase "$(INTDIR)\sfx_soundsampler.obj"
	-@erase "$(INTDIR)\simonsbasic.obj"
	-@erase "$(INTDIR)\snapshot64.obj"
	-@erase "$(INTDIR)\stardos.obj"
	-@erase "$(INTDIR)\stb.obj"
	-@erase "$(INTDIR)\superexplode5.obj"
	-@erase "$(INTDIR)\supergames.obj"
	-@erase "$(INTDIR)\supersnapshot.obj"
	-@erase "$(INTDIR)\supersnapshot4.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\warpspeed.obj"
	-@erase "$(INTDIR)\westermann.obj"
	-@erase "$(INTDIR)\zaxxon.obj"
	-@erase "$(OUTDIR)\c64cart.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\rtc" /I "..\..\..\vicii" /I "..\..\..\drive" /I "..\..\..\sid" /I "..\..\..\monitor" /I "..\..\..\core" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c64cart.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c64cart.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c64cart.lib" 
LIB32_OBJS= \
	"$(INTDIR)\actionreplay.obj" \
	"$(INTDIR)\actionreplay2.obj" \
	"$(INTDIR)\actionreplay3.obj" \
	"$(INTDIR)\actionreplay4.obj" \
	"$(INTDIR)\atomicpower.obj" \
	"$(INTDIR)\c64-generic.obj" \
	"$(INTDIR)\c64-midi.obj" \
	"$(INTDIR)\c64acia1.obj" \
	"$(INTDIR)\c64tpi.obj" \
	"$(INTDIR)\capture.obj" \
	"$(INTDIR)\comal80.obj" \
	"$(INTDIR)\delaep256.obj" \
	"$(INTDIR)\delaep64.obj" \
	"$(INTDIR)\delaep7x8.obj" \
	"$(INTDIR)\diashowmaker.obj" \
	"$(INTDIR)\digimax.obj" \
	"$(INTDIR)\dinamic.obj" \
	"$(INTDIR)\dqbb.obj" \
	"$(INTDIR)\easyflash.obj" \
	"$(INTDIR)\epyxfastload.obj" \
	"$(INTDIR)\exos.obj" \
	"$(INTDIR)\expert.obj" \
	"$(INTDIR)\final.obj" \
	"$(INTDIR)\final3.obj" \
	"$(INTDIR)\finalplus.obj" \
	"$(INTDIR)\freezeframe.obj" \
	"$(INTDIR)\freezemachine.obj" \
	"$(INTDIR)\funplay.obj" \
	"$(INTDIR)\gamekiller.obj" \
	"$(INTDIR)\gs.obj" \
	"$(INTDIR)\ide64.obj" \
	"$(INTDIR)\isepic.obj" \
	"$(INTDIR)\kcs.obj" \
	"$(INTDIR)\mach5.obj" \
	"$(INTDIR)\magicdesk.obj" \
	"$(INTDIR)\magicformel.obj" \
	"$(INTDIR)\magicvoice.obj" \
	"$(INTDIR)\mmc64.obj" \
	"$(INTDIR)\mmcreplay.obj" \
	"$(INTDIR)\mikroass.obj" \
	"$(INTDIR)\ocean.obj" \
	"$(INTDIR)\prophet64.obj" \
	"$(INTDIR)\ramcart.obj" \
	"$(INTDIR)\retroreplay.obj" \
	"$(INTDIR)\reu.obj" \
	"$(INTDIR)\rexep256.obj" \
	"$(INTDIR)\rexutility.obj" \
	"$(INTDIR)\ross.obj" \
	"$(INTDIR)\sfx_soundexpander.obj" \
	"$(INTDIR)\sfx_soundsampler.obj" \
	"$(INTDIR)\simonsbasic.obj" \
	"$(INTDIR)\snapshot64.obj" \
	"$(INTDIR)\stardos.obj" \
	"$(INTDIR)\stb.obj" \
	"$(INTDIR)\superexplode5.obj" \
	"$(INTDIR)\supergames.obj" \
	"$(INTDIR)\supersnapshot.obj" \
	"$(INTDIR)\supersnapshot4.obj" \
	"$(INTDIR)\warpspeed.obj" \
	"$(INTDIR)\westermann.obj" \
	"$(INTDIR)\zaxxon.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\c64cart.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "c64cart - Win32 Debug"

OUTDIR=.\libs\c64cart\Debug
INTDIR=.\libs\c64cart\Debug
# Begin Custom Macros
OutDir=.\libs\c64cart\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c64cart.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\c64cart.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\actionreplay.obj"
	-@erase "$(INTDIR)\actionreplay2.obj"
	-@erase "$(INTDIR)\actionreplay3.obj"
	-@erase "$(INTDIR)\actionreplay4.obj"
	-@erase "$(INTDIR)\atomicpower.obj"
	-@erase "$(INTDIR)\c64-generic.obj"
	-@erase "$(INTDIR)\c64-midi.obj"
	-@erase "$(INTDIR)\c64acia1.obj"
	-@erase "$(INTDIR)\c64tpi.obj"
	-@erase "$(INTDIR)\capture.obj"
	-@erase "$(INTDIR)\comal80.obj"
	-@erase "$(INTDIR)\delaep256.obj"
	-@erase "$(INTDIR)\delaep64.obj"
	-@erase "$(INTDIR)\delaep7x8.obj"
	-@erase "$(INTDIR)\diashowmaker.obj"
	-@erase "$(INTDIR)\digimax.obj"
	-@erase "$(INTDIR)\dinamic.obj"
	-@erase "$(INTDIR)\dqbb.obj"
	-@erase "$(INTDIR)\easyflash.obj"
	-@erase "$(INTDIR)\epyxfastload.obj"
	-@erase "$(INTDIR)\exos.obj"
	-@erase "$(INTDIR)\expert.obj"
	-@erase "$(INTDIR)\final.obj"
	-@erase "$(INTDIR)\final3.obj"
	-@erase "$(INTDIR)\finalplus.obj"
	-@erase "$(INTDIR)\freezeframe.obj"
	-@erase "$(INTDIR)\freezemachine.obj"
	-@erase "$(INTDIR)\funplay.obj"
	-@erase "$(INTDIR)\gamekiller.obj"
	-@erase "$(INTDIR)\gs.obj"
	-@erase "$(INTDIR)\ide64.obj"
	-@erase "$(INTDIR)\isepic.obj"
	-@erase "$(INTDIR)\kcs.obj"
	-@erase "$(INTDIR)\mach5.obj"
	-@erase "$(INTDIR)\magicdesk.obj"
	-@erase "$(INTDIR)\magicformel.obj"
	-@erase "$(INTDIR)\magicvoice.obj"
	-@erase "$(INTDIR)\mikroass.obj"
	-@erase "$(INTDIR)\mmc64.obj"
	-@erase "$(INTDIR)\mmcreplay.obj"
	-@erase "$(INTDIR)\ocean.obj"
	-@erase "$(INTDIR)\prophet64.obj"
	-@erase "$(INTDIR)\ramcart.obj"
	-@erase "$(INTDIR)\retroreplay.obj"
	-@erase "$(INTDIR)\reu.obj"
	-@erase "$(INTDIR)\rexep256.obj"
	-@erase "$(INTDIR)\rexutility.obj"
	-@erase "$(INTDIR)\ross.obj"
	-@erase "$(INTDIR)\sfx_soundexpander.obj"
	-@erase "$(INTDIR)\sfx_soundsampler.obj"
	-@erase "$(INTDIR)\simonsbasic.obj"
	-@erase "$(INTDIR)\snapshot64.obj"
	-@erase "$(INTDIR)\stardos.obj"
	-@erase "$(INTDIR)\stb.obj"
	-@erase "$(INTDIR)\superexplode5.obj"
	-@erase "$(INTDIR)\supergames.obj"
	-@erase "$(INTDIR)\supersnapshot.obj"
	-@erase "$(INTDIR)\supersnapshot4.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\warpspeed.obj"
	-@erase "$(INTDIR)\westermann.obj"
	-@erase "$(INTDIR)\zaxxon.obj"
	-@erase "$(OUTDIR)\c64cart.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\c64" /I "..\..\..\rtc" /I "..\..\..\vicii" /I "..\..\..\drive" /I "..\..\..\sid" /I "..\..\..\monitor" /I "..\..\..\core" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c64cart.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c64cart.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c64cart.lib" 
LIB32_OBJS= \
	"$(INTDIR)\actionreplay.obj" \
	"$(INTDIR)\actionreplay2.obj" \
	"$(INTDIR)\actionreplay3.obj" \
	"$(INTDIR)\actionreplay4.obj" \
	"$(INTDIR)\atomicpower.obj" \
	"$(INTDIR)\c64-generic.obj" \
	"$(INTDIR)\c64-midi.obj" \
	"$(INTDIR)\c64acia1.obj" \
	"$(INTDIR)\c64tpi.obj" \
	"$(INTDIR)\capture.obj" \
	"$(INTDIR)\comal80.obj" \
	"$(INTDIR)\delaep256.obj" \
	"$(INTDIR)\delaep64.obj" \
	"$(INTDIR)\delaep7x8.obj" \
	"$(INTDIR)\diashowmaker.obj" \
	"$(INTDIR)\digimax.obj" \
	"$(INTDIR)\dinamic.obj" \
	"$(INTDIR)\dqbb.obj" \
	"$(INTDIR)\easyflash.obj" \
	"$(INTDIR)\epyxfastload.obj" \
	"$(INTDIR)\exos.obj" \
	"$(INTDIR)\expert.obj" \
	"$(INTDIR)\final.obj" \
	"$(INTDIR)\final3.obj" \
	"$(INTDIR)\finalplus.obj" \
	"$(INTDIR)\freezeframe.obj" \
	"$(INTDIR)\freezemachine.obj" \
	"$(INTDIR)\funplay.obj" \
	"$(INTDIR)\gamekiller.obj" \
	"$(INTDIR)\gs.obj" \
	"$(INTDIR)\ide64.obj" \
	"$(INTDIR)\isepic.obj" \
	"$(INTDIR)\kcs.obj" \
	"$(INTDIR)\mach5.obj" \
	"$(INTDIR)\magicdesk.obj" \
	"$(INTDIR)\magicformel.obj" \
	"$(INTDIR)\magicvoice.obj" \
	"$(INTDIR)\mmc64.obj" \
	"$(INTDIR)\mmcreplay.obj" \
	"$(INTDIR)\mikroass.obj" \
	"$(INTDIR)\ocean.obj" \
	"$(INTDIR)\prophet64.obj" \
	"$(INTDIR)\ramcart.obj" \
	"$(INTDIR)\retroreplay.obj" \
	"$(INTDIR)\reu.obj" \
	"$(INTDIR)\rexep256.obj" \
	"$(INTDIR)\rexutility.obj" \
	"$(INTDIR)\ross.obj" \
	"$(INTDIR)\sfx_soundexpander.obj" \
	"$(INTDIR)\sfx_soundsampler.obj" \
	"$(INTDIR)\simonsbasic.obj" \
	"$(INTDIR)\snapshot64.obj" \
	"$(INTDIR)\stardos.obj" \
	"$(INTDIR)\stb.obj" \
	"$(INTDIR)\superexplode5.obj" \
	"$(INTDIR)\supergames.obj" \
	"$(INTDIR)\supersnapshot.obj" \
	"$(INTDIR)\supersnapshot4.obj" \
	"$(INTDIR)\warpspeed.obj" \
	"$(INTDIR)\westermann.obj" \
	"$(INTDIR)\zaxxon.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\c64cart.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "c64cart - Win32 Release" || "$(CFG)" == "c64cart - Win32 Debug"

!IF  "$(CFG)" == "c64cart - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "c64cart - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\c64\cart\actionreplay.c"

"$(INTDIR)\actionreplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\actionreplay2.c"

"$(INTDIR)\actionreplay2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\actionreplay3.c"

"$(INTDIR)\actionreplay3.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\actionreplay4.c"

"$(INTDIR)\actionreplay4.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\atomicpower.c"

"$(INTDIR)\atomicpower.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\c64-generic.c"

"$(INTDIR)\c64-generic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\c64-midi.c"

"$(INTDIR)\c64-midi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\c64acia1.c

"$(INTDIR)\c64acia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\c64tpi.c

"$(INTDIR)\c64tpi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\capture.c"

"$(INTDIR)\capture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\comal80.c"

"$(INTDIR)\comal80.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\delaep256.c"

"$(INTDIR)\delaep256.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\delaep64.c"

"$(INTDIR)\delaep64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\delaep7x8.c"

"$(INTDIR)\delaep7x8.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\diashowmaker.c"

"$(INTDIR)\diashowmaker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\digimax.c

"$(INTDIR)\digimax.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\dinamic.c"

"$(INTDIR)\dinamic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\dqbb.c

"$(INTDIR)\dqbb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\easyflash.c"

"$(INTDIR)\easyflash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\epyxfastload.c"

"$(INTDIR)\epyxfastload.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\exos.c"

"$(INTDIR)\exos.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\expert.c"

"$(INTDIR)\expert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\final.c"

"$(INTDIR)\final.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\final3.c"

"$(INTDIR)\final3.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\finalplus.c"

"$(INTDIR)\finalplus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\freezeframe.c"

"$(INTDIR)\freezeframe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\freezemachine.c"

"$(INTDIR)\freezemachine.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\funplay.c"

"$(INTDIR)\funplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\gamekiller.c"

"$(INTDIR)\gamekiller.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\gs.c"

"$(INTDIR)\gs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\ide64.c"

"$(INTDIR)\ide64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\isepic.c

"$(INTDIR)\isepic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\kcs.c"

"$(INTDIR)\kcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\mach5.c"

"$(INTDIR)\mach5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\magicdesk.c"

"$(INTDIR)\magicdesk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\magicformel.c"

"$(INTDIR)\magicformel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\magicvoice.c"

"$(INTDIR)\magicvoice.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\mikroass.c"

"$(INTDIR)\mikroass.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\mmc64.c

"$(INTDIR)\mmc64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\mmcreplay.c"

"$(INTDIR)\mmcreplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\ocean.c"

"$(INTDIR)\ocean.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\prophet64.c"

"$(INTDIR)\prophet64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\ramcart.c

"$(INTDIR)\ramcart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\retroreplay.c"

"$(INTDIR)\retroreplay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\reu.c

"$(INTDIR)\reu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\rexep256.c"

"$(INTDIR)\rexep256.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\rexutility.c"

"$(INTDIR)\rexutility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\ross.c"

"$(INTDIR)\ross.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\sfx_soundexpander.c

"$(INTDIR)\sfx_soundexpander.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\sfx_soundsampler.c

"$(INTDIR)\sfx_soundsampler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\simonsbasic.c

"$(INTDIR)\simonsbasic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64\cart\snapshot64.c

"$(INTDIR)\snapshot64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\stardos.c"

"$(INTDIR)\stardos.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\stb.c"

"$(INTDIR)\stb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\superexplode5.c"

"$(INTDIR)\superexplode5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\supergames.c"

"$(INTDIR)\supergames.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\supersnapshot.c"

"$(INTDIR)\supersnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\supersnapshot4.c"

"$(INTDIR)\supersnapshot4.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\warpspeed.c"

"$(INTDIR)\warpspeed.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\westermann.c"

"$(INTDIR)\westermann.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64\cart\zaxxon.c"

"$(INTDIR)\zaxxon.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

