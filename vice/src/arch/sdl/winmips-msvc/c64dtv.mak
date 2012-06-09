# Microsoft Developer Studio Generated NMAKE File, Based on c64dtv.dsp
!IF "$(CFG)" == ""
CFG=c64dtv - Win32 Debug
!MESSAGE No configuration specified. Defaulting to c64dtv - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "c64dtv - Win32 Release" && "$(CFG)" != "c64dtv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "c64dtv.mak" CFG="c64dtv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "c64dtv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "c64dtv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "c64dtv - Win32 Release"

OUTDIR=.\libs\c64dtv\Release
INTDIR=.\libs\c64dtv\Release
# Begin Custom Macros
OutDir=.\libs\c64dtv\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c64dtv.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\c64dtv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c64dtv-cmdline-options.obj"
	-@erase "$(INTDIR)\c64dtv-resources.obj"
	-@erase "$(INTDIR)\c64dtv-snapshot.obj"
	-@erase "$(INTDIR)\c64dtv.obj"
	-@erase "$(INTDIR)\c64dtvblitter.obj"
	-@erase "$(INTDIR)\c64dtvcia1.obj"
	-@erase "$(INTDIR)\c64dtvcia2.obj"
	-@erase "$(INTDIR)\c64dtvcpu.obj"
	-@erase "$(INTDIR)\c64dtvdma.obj"
	-@erase "$(INTDIR)\c64dtvflash.obj"
	-@erase "$(INTDIR)\c64dtviec.obj"
	-@erase "$(INTDIR)\c64dtvmem.obj"
	-@erase "$(INTDIR)\c64dtvmeminit.obj"
	-@erase "$(INTDIR)\c64dtvmemrom.obj"
	-@erase "$(INTDIR)\c64dtvmemsnapshot.obj"
	-@erase "$(INTDIR)\c64dtvmodel.obj"
	-@erase "$(INTDIR)\c64dtvpla.obj"
	-@erase "$(INTDIR)\c64dtvsound.obj"
	-@erase "$(INTDIR)\flash-trap.obj"
	-@erase "$(INTDIR)\hummeradc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\c64dtv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\vicii" /I "..\..\..\c64" /I "..\..\..\raster" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c64dtv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c64dtv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c64dtv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c64dtv-cmdline-options.obj" \
	"$(INTDIR)\c64dtv-resources.obj" \
	"$(INTDIR)\c64dtv-snapshot.obj" \
	"$(INTDIR)\c64dtv.obj" \
	"$(INTDIR)\c64dtvblitter.obj" \
	"$(INTDIR)\c64dtvcia1.obj" \
	"$(INTDIR)\c64dtvcia2.obj" \
	"$(INTDIR)\c64dtvcpu.obj" \
	"$(INTDIR)\c64dtvdma.obj" \
	"$(INTDIR)\c64dtvflash.obj" \
	"$(INTDIR)\c64dtviec.obj" \
	"$(INTDIR)\c64dtvmem.obj" \
	"$(INTDIR)\c64dtvmeminit.obj" \
	"$(INTDIR)\c64dtvmemrom.obj" \
	"$(INTDIR)\c64dtvmemsnapshot.obj" \
	"$(INTDIR)\c64dtvmodel.obj" \
	"$(INTDIR)\c64dtvpla.obj" \
	"$(INTDIR)\c64dtvsound.obj" \
	"$(INTDIR)\flash-trap.obj" \
	"$(INTDIR)\hummeradc.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\c64dtv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "c64dtv - Win32 Debug"

OUTDIR=.\libs\c64dtv\Debug
INTDIR=.\libs\c64dtv\Debug
# Begin Custom Macros
OutDir=.\libs\c64dtv\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\c64dtv.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\c64dtv.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\c64dtv-cmdline-options.obj"
	-@erase "$(INTDIR)\c64dtv-resources.obj"
	-@erase "$(INTDIR)\c64dtv-snapshot.obj"
	-@erase "$(INTDIR)\c64dtv.obj"
	-@erase "$(INTDIR)\c64dtvblitter.obj"
	-@erase "$(INTDIR)\c64dtvcia1.obj"
	-@erase "$(INTDIR)\c64dtvcia2.obj"
	-@erase "$(INTDIR)\c64dtvcpu.obj"
	-@erase "$(INTDIR)\c64dtvdma.obj"
	-@erase "$(INTDIR)\c64dtvflash.obj"
	-@erase "$(INTDIR)\c64dtviec.obj"
	-@erase "$(INTDIR)\c64dtvmem.obj"
	-@erase "$(INTDIR)\c64dtvmeminit.obj"
	-@erase "$(INTDIR)\c64dtvmemrom.obj"
	-@erase "$(INTDIR)\c64dtvmemsnapshot.obj"
	-@erase "$(INTDIR)\c64dtvmodel.obj"
	-@erase "$(INTDIR)\c64dtvpla.obj"
	-@erase "$(INTDIR)\c64dtvsound.obj"
	-@erase "$(INTDIR)\flash-trap.obj"
	-@erase "$(INTDIR)\hummeradc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\c64dtv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\drive" /I "..\..\..\lib\p64" /I "..\..\..\userport" /I "..\..\..\video" /I "..\..\..\tape" /I "..\..\..\sid" /I "..\..\..\vicii" /I "..\..\..\c64" /I "..\..\..\raster" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\c64dtv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\c64dtv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\c64dtv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\c64dtv-cmdline-options.obj" \
	"$(INTDIR)\c64dtv-resources.obj" \
	"$(INTDIR)\c64dtv-snapshot.obj" \
	"$(INTDIR)\c64dtv.obj" \
	"$(INTDIR)\c64dtvblitter.obj" \
	"$(INTDIR)\c64dtvcia1.obj" \
	"$(INTDIR)\c64dtvcia2.obj" \
	"$(INTDIR)\c64dtvcpu.obj" \
	"$(INTDIR)\c64dtvdma.obj" \
	"$(INTDIR)\c64dtvflash.obj" \
	"$(INTDIR)\c64dtviec.obj" \
	"$(INTDIR)\c64dtvmem.obj" \
	"$(INTDIR)\c64dtvmeminit.obj" \
	"$(INTDIR)\c64dtvmemrom.obj" \
	"$(INTDIR)\c64dtvmemsnapshot.obj" \
	"$(INTDIR)\c64dtvmodel.obj" \
	"$(INTDIR)\c64dtvpla.obj" \
	"$(INTDIR)\c64dtvsound.obj" \
	"$(INTDIR)\flash-trap.obj" \
	"$(INTDIR)\hummeradc.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\c64dtv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "c64dtv - Win32 Release" || "$(CFG)" == "c64dtv - Win32 Debug"

!IF  "$(CFG)" == "c64dtv - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "c64dtv - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\c64dtv\c64dtv-cmdline-options.c"

"$(INTDIR)\c64dtv-cmdline-options.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64dtv\c64dtv-resources.c"

"$(INTDIR)\c64dtv-resources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64dtv\c64dtv-snapshot.c"

"$(INTDIR)\c64dtv-snapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtv.c

"$(INTDIR)\c64dtv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtvblitter.c

"$(INTDIR)\c64dtvblitter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtvcia1.c

"$(INTDIR)\c64dtvcia1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtvcia2.c

"$(INTDIR)\c64dtvcia2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64dtv\c64dtvcpu.c"

"$(INTDIR)\c64dtvcpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtvdma.c

"$(INTDIR)\c64dtvdma.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtvflash.c

"$(INTDIR)\c64dtvflash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtviec.c

"$(INTDIR)\c64dtviec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64dtv\c64dtvmem.c"

"$(INTDIR)\c64dtvmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtvmeminit.c

"$(INTDIR)\c64dtvmeminit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64dtv\c64dtvmemrom.c"

"$(INTDIR)\c64dtvmemrom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64dtv\c64dtvmemsnapshot.c"

"$(INTDIR)\c64dtvmemsnapshot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64dtv\c64dtvmodel.c"

"$(INTDIR)\c64dtvmodel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtvpla.c

"$(INTDIR)\c64dtvpla.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\c64dtvsound.c

"$(INTDIR)\c64dtvsound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\c64dtv\flash-trap.c"

"$(INTDIR)\flash-trap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\c64dtv\hummeradc.c

"$(INTDIR)\hummeradc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

