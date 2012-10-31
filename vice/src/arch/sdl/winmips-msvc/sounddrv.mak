# Microsoft Developer Studio Generated NMAKE File, Based on sounddrv.dsp
!IF "$(CFG)" == ""
CFG=sounddrv - Win32 Release
!MESSAGE No configuration specified. Defaulting to sounddrv - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "sounddrv - Win32 Release" && "$(CFG)" != "sounddrv - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sounddrv.mak" CFG="sounddrv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sounddrv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sounddrv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "sounddrv - Win32 Release"

OUTDIR=.\libs\sounddrv\Release
INTDIR=.\libs\sounddrv\Release
# Begin Custom Macros
OutDir=.\libs\sounddrv\Release
# End Custom Macros

ALL : "$(OUTDIR)\sounddrv.lib"


CLEAN :
	-@erase "$(INTDIR)\soundaiff.obj"
	-@erase "$(INTDIR)\sounddummy.obj"
	-@erase "$(INTDIR)\sounddump.obj"
	-@erase "$(INTDIR)\soundfs.obj"
	-@erase "$(INTDIR)\soundiff.obj"
	-@erase "$(INTDIR)\soundmovie.obj"
	-@erase "$(INTDIR)\soundvoc.obj"
	-@erase "$(INTDIR)\soundwav.obj"
	-@erase "$(INTDIR)\soundwmm.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\sounddrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\sounddrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sounddrv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\sounddrv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\soundaiff.obj" \
	"$(INTDIR)\sounddummy.obj" \
	"$(INTDIR)\sounddump.obj" \
	"$(INTDIR)\soundfs.obj" \
	"$(INTDIR)\soundiff.obj" \
	"$(INTDIR)\soundmovie.obj" \
	"$(INTDIR)\soundvoc.obj" \
	"$(INTDIR)\soundwav.obj" \
	"$(INTDIR)\soundwmm.obj"

"$(OUTDIR)\sounddrv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sounddrv - Win32 Debug"

OUTDIR=.\libs\sounddrv\Debug
INTDIR=.\libs\sounddrv\Debug
# Begin Custom Macros
OutDir=.\libs\sounddrv\Debug
# End Custom Macros

ALL : "$(OUTDIR)\sounddrv.lib"


CLEAN :
	-@erase "$(INTDIR)\soundaiff.obj"
	-@erase "$(INTDIR)\sounddummy.obj"
	-@erase "$(INTDIR)\sounddump.obj"
	-@erase "$(INTDIR)\soundfs.obj"
	-@erase "$(INTDIR)\soundiff.obj"
	-@erase "$(INTDIR)\soundmovie.obj"
	-@erase "$(INTDIR)\soundvoc.obj"
	-@erase "$(INTDIR)\soundwav.obj"
	-@erase "$(INTDIR)\soundwmm.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\sounddrv.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\sounddrv.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sounddrv.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\sounddrv.lib" 
LIB32_OBJS= \
	"$(INTDIR)\soundaiff.obj" \
	"$(INTDIR)\sounddummy.obj" \
	"$(INTDIR)\sounddump.obj" \
	"$(INTDIR)\soundfs.obj" \
	"$(INTDIR)\soundiff.obj" \
	"$(INTDIR)\soundmovie.obj" \
	"$(INTDIR)\soundvoc.obj" \
	"$(INTDIR)\soundwav.obj" \
	"$(INTDIR)\soundwmm.obj"

"$(OUTDIR)\sounddrv.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "sounddrv - Win32 Release" || "$(CFG)" == "sounddrv - Win32 Debug"
SOURCE=..\..\..\sounddrv\soundaiff.c

"$(INTDIR)\soundaiff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\sounddrv\sounddummy.c

"$(INTDIR)\sounddummy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\sounddrv\sounddump.c

"$(INTDIR)\sounddump.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\sounddrv\soundfs.c

"$(INTDIR)\soundfs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\sounddrv\soundiff.c

"$(INTDIR)\soundiff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\sounddrv\soundmovie.c

"$(INTDIR)\soundmovie.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\sounddrv\soundvoc.c

"$(INTDIR)\soundvoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\sounddrv\soundwav.c

"$(INTDIR)\soundwav.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\sounddrv\soundwmm.c

"$(INTDIR)\soundwmm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

