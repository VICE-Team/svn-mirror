# Microsoft Developer Studio Project File - Name="residdtv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=resid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "residdtv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "residdtv.mak" CFG="residdtv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "residdtv - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "residdtv - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "residdtv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\residdtv\Release"
# PROP Intermediate_Dir "libs\residdtv\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "residdtv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\residdtv\Debug"
# PROP Intermediate_Dir "libs\residdtv\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I ".\msvc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /YX /FD /c
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

# Name "residdtv - Win32 Release"
# Name "residdtv - Win32 Debug"
# Begin Source File

SOURCE=..\..\resid-dtv\envelope.cc

!IF  "$(CFG)" == "residdtv - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-dtv\envelope.cc
InputName=envelope

"libs\residdtv\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Release/residdtv.pch" /Fo"libs\residdtv\Release/" /Fd"libs\residdtv\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residdtv - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-dtv\envelope.cc
InputName=envelope

"libs\residdtv\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Debug/residdtv.pch" /Fo"libs\residdtv\Debug/" /Fd"libs\residdtv\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\envelope.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\extfilt.cc

!IF  "$(CFG)" == "residdtv - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-dtv\extfilt.cc
InputName=extfilt

"libs\residdtv\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Release/residdtv.pch" /Fo"libs\residdtv\Release/" /Fd"libs\residdtv\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residdtv - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-dtv\extfilt.cc
InputName=extfilt

"libs\residdtv\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Debug/residdtv.pch" /Fo"libs\residdtv\Debug/" /Fd"libs\residdtv\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\extfilt.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\filter.cc

!IF  "$(CFG)" == "residdtv - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-dtv\filter.cc
InputName=filter

"libs\residdtv\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Release/residdtv.pch" /Fo"libs\residdtv\Release/" /Fd"libs\residdtv\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residdtv - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-dtv\filter.cc
InputName=filter

"libs\residdtv\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Debug/residdtv.pch" /Fo"libs\residdtv\Debug/" /Fd"libs\residdtv\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\filter.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\sid.cc

!IF  "$(CFG)" == "residdtv - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-dtv\sid.cc
InputName=sid

"libs\residdtv\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Release/residdtv.pch" /Fo"libs\residdtv\Release/" /Fd"libs\residdtv\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residdtv - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-dtv\sid.cc
InputName=sid

"libs\residdtv\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residdtv\Debug/residdtv.pch" /Fo"libs\residdtv\Debug/" /Fd"libs\residdtv\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\sid.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\siddefs.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\version.cc

!IF  "$(CFG)" == "residdtv - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-dtv\version.cc
InputName=version

"libs\residdtv\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Release/residdtv.pch" /Fo"libs\residdtv\Release/" /Fd"libs\residdtv\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residdtv - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-dtv\version.cc
InputName=version

"libs\residdtv\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residdtv\Debug/residdtv.pch" /Fo"libs\residdtv\Debug/" /Fd"libs\residdtv\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\voice.cc

!IF  "$(CFG)" == "residdtv - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-dtv\voice.cc
InputName=voice

"libs\residdtv\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Release/residdtv.pch" /Fo"libs\residdtv\Release/" /Fd"libs\residdtv\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residdtv - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-dtv\voice.cc
InputName=voice

"libs\residdtv\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residdtv\Debug/residdtv.pch" /Fo"libs\residdtv\Debug/" /Fd"libs\residdtv\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\voice.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\wave.cc

!IF  "$(CFG)" == "residdtv - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-dtv\wave.cc
InputName=wave

"libs\residdtv\Release/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MT /W3 /EHsc /I ".\msvc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"libs\residdtv\Release/residdtv.pch" /Fo"libs\residdtv\Release/" /Fd"libs\residdtv\Release/" /FD /TP /c $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "residdtv - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-dtv\wave.cc
InputName=wave

"libs\residdtv\Debug/$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /nologo /MTd /W3 /EHsc /Z7 /Od /I ".\msvc" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residdtv\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /D "_DEBUG" /Fp"libs\residdtv\Debug/residdtv.pch" /Fo"libs\residdtv\Debug/" /Fd"libs\residdtv\Debug/" /FD /TP /c $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-dtv\wave.h
# End Source File
# End Target
# End Project
