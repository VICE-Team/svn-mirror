# Microsoft Developer Studio Project File - Name="residfp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=resid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "residfp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "resid.mak" CFG="residfp - Win32 Debug"
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

!IF  "$(CFG)" == "resid - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /YX /FD /c
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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"residfp\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /YX /FD /c
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

SOURCE=..\..\resid-fp\convolve.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\concolve.cc
InputName=version

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\convolve.cc
InputName=version

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\convolve.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\envelope.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\envelope.cc
InputName=envelope

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\envelope.cc
InputName=envelope

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\envelope.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\envelope.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\extfilt.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\extfilt.cc
InputName=extfilt

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\extfilt.cc
InputName=extfilt

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\extfilt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\extfilt.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\filter.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\filter.cc
InputName=filter

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\filter.cc
InputName=filter

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\filter.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\pot.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\pot.cc
InputName=pot

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\pot.cc
InputName=pot

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\pot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\pot.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\sid.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\sid.cc
InputName=sid

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\sid.cc
InputName=sid

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\sid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\sid.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\siddefs.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\version.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\version.cc
InputName=version

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\version.cc
InputName=version

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\version.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\voice.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\voice.cc
InputName=voice

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\voice.cc
InputName=voice

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\voice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\voice.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave.cc
InputName=wave

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave.cc
InputName=wave

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave.h
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave6581__ST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave6581__ST.cc
InputName=wave6581__ST

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave6581__ST.cc
InputName=wave6581__ST

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave6581__ST.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave6581_P_T.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave6581_P_T.cc
InputName=wave6581_P_T

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave6581_P_T.cc
InputName=wave6581_P_T

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave6581_P_T.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave6581_PS_.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave6581_PS_.cc
InputName=wave6581_PS_

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave6581_PS_.cc
InputName=wave6581_PS_

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave6581_PS_.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave6581_PST.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave6581_PST.cc
InputName=wave6581_PST

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave6581_PST.cc
InputName=wave6581_PST

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave6581_PST.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave8580__ST.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave8580__ST.cc
InputName=wave8580__ST

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave8580__ST.cc
InputName=wave8580__ST

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave8580__ST.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave8580_P_T.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave8580_P_T.cc
InputName=wave8580_P_T

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave8580_P_T.cc
InputName=wave8580_P_T

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave8580_P_T.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave8580_PS_.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave8580_PS_.cc
InputName=wave8580_PS_

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave8580_PS_.cc
InputName=wave8580_PS_

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave8580_PS_.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave8580_PST.cc

!IF  "$(CFG)" == "residfp - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid-fp\wave8580_PST.cc
InputName=wave8580_PST

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "residfp - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid-fp\wave8580_PST.cc
InputName=wave8580_PST

"..\\..\\resid-fp\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid-fp\\$(InputName).cc ..\\..\\resid-fp\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid-fp\wave8580_PST.cpp
# End Source File
# End Target
# End Project
