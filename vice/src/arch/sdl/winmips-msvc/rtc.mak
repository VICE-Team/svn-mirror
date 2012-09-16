# Microsoft Developer Studio Generated NMAKE File, Based on rtc.dsp
!IF "$(CFG)" == ""
CFG=rtc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to rtc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "rtc - Win32 Release" && "$(CFG)" != "rtc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rtc.mak" CFG="rtc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rtc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "rtc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "rtc - Win32 Release"

OUTDIR=.\libs\rtc\Release
INTDIR=.\libs\rtc\Release
# Begin Custom Macros
OutDir=.\libs\rtc\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\rtc.lib"

!ELSE 

ALL : "base - Win32 Release" "$(OUTDIR)\rtc.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\bq4830y.obj"
	-@erase "$(INTDIR)\ds12c887.obj"
	-@erase "$(INTDIR)\ds1202_1302.obj"
	-@erase "$(INTDIR)\ds1216e.obj"
	-@erase "$(INTDIR)\rtc.obj"
	-@erase "$(INTDIR)\rtc-58321a.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\rtc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\raster" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\rtc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rtc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\rtc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bq4830y.obj" \
	"$(INTDIR)\ds12c887.obj" \
	"$(INTDIR)\ds1202_1302.obj" \
	"$(INTDIR)\ds1216e.obj" \
	"$(INTDIR)\rtc.obj" \
	"$(INTDIR)\rtc-58321a.obj" \
	".\libs\base\Release\base.lib"

"$(OUTDIR)\rtc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "rtc - Win32 Debug"

OUTDIR=.\libs\rtc\Debug
INTDIR=.\libs\rtc\Debug
# Begin Custom Macros
OutDir=.\libs\rtc\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\rtc.lib"

!ELSE 

ALL : "base - Win32 Debug" "$(OUTDIR)\rtc.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"base - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\bq4830y.obj"
	-@erase "$(INTDIR)\ds12c887.obj"
	-@erase "$(INTDIR)\ds1202_1302.obj"
	-@erase "$(INTDIR)\ds1216e.obj"
	-@erase "$(INTDIR)\rtc.obj"
	-@erase "$(INTDIR)\rtc-58321a.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\rtc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I ".\\" /I "..\\" /I "..\..\..\\" /I "..\..\..\raster" /D "WIN32" /D "WINMIPS" /D "IDE_COMPILE" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\rtc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"  /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rtc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\rtc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bq4830y.obj" \
	"$(INTDIR)\ds12c887.obj" \
	"$(INTDIR)\ds1202_1302.obj" \
	"$(INTDIR)\ds1216e.obj" \
	"$(INTDIR)\rtc.obj" \
	"$(INTDIR)\rtc-58321a.obj" \
	".\libs\base\Debug\base.lib"

"$(OUTDIR)\rtc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "rtc - Win32 Release" || "$(CFG)" == "rtc - Win32 Debug"

!IF  "$(CFG)" == "rtc - Win32 Release"

"base - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" 
   cd "."

"base - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "rtc - Win32 Debug"

"base - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" 
   cd "."

"base - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\base.mak" CFG="base - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

SOURCE="..\..\..\rtc\bq4830y.c"

"$(INTDIR)\bq4830y.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\rtc\ds12c887.c"

"$(INTDIR)\ds12c887.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\rtc\ds1202_1302.c"

"$(INTDIR)\ds1202_1302.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\rtc\ds1216e.c"

"$(INTDIR)\ds1216e.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\rtc\rtc.c"

"$(INTDIR)\rtc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\rtc\rtc-58321a.c"

"$(INTDIR)\rtc-58321a.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

