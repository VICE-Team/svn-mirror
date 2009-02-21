# Microsoft Developer Studio Project File - Name="residfp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=residfp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "residfp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "residfp.mak" CFG="residfp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "residfp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "residfp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "residfp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\residfp\Release"
# PROP Intermediate_Dir "libs\residfp\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\residfp\Debug"
# PROP Intermediate_Dir "libs\residfp\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_DEBUG" /YX /FD /c
# ADD CPP /MTd /W3 /GX /Z7 /Od /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /YX /FD /c
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

# Name "residfp - Win32 Release"
# Name "residfp - Win32 Debug"
# Begin Source File

SOURCE="..\..\resid-fp\convolve.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\convolve.cc"
InputName=convolve

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\convolve.cc"
InputName=convolve

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\convolve-sse.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\convolve-sse.cc"
InputName=convolve-sse

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\convolve-sse.cc"
InputName=convolve-sse

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\envelope.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\envelope.cc"
InputName=envelope

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\envelope.cc"
InputName=envelope

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\envelope.h"
# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\extfilt.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\extfilt.cc"
InputName=extfilt

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\extfilt.cc"
InputName=extfilt

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\extfilt.h"
# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\filter.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\filter.cc"
InputName=filter

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\filter.cc"
InputName=filter

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\filter.h"
# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\pot.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\pot.cc"
InputName=pot

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\pot.cc"
InputName=pot

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\pot.h"
# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\sid.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\sid.cc"
InputName=sid

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\sid.cc"
InputName=sid

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\sid.h"
# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\siddefs.h"
# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\version.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\version.cc"
InputName=version

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\version.cc"
InputName=version

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\voice.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\voice.cc"
InputName=voice

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\voice.cc"
InputName=voice

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\voice.h"
# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\wave.cc"

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath="..\..\resid-fp\wave.cc"
InputName=wave

"libs\resid-fp\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\resid-fp\Release/resid-fp.pch"  /Fo"libs\resid-fp\Release/" /Fd"libs\resid-fp\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath="..\..\resid-fp\wave.cc"
InputName=wave

"libs\residfp\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residfp\Debug/residfp.pch"  /Fo"libs\residfp\Debug/" /Fd"libs\residfp\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\resid-fp\wave.h"
# End Source File
# End Target
# End Project
