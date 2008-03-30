# Microsoft Developer Studio Project File - Name="resid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=resid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "resid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "resid.mak" CFG="resid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "resid - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "resid - Win32 Debug" (based on "Win32 (x86) Static Library")
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
# PROP Output_Dir "libs\resid\Release"
# PROP Intermediate_Dir "libs\resid\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\resid\Debug"
# PROP Intermediate_Dir "libs\resid\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /YX /FD /c
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

# Name "resid - Win32 Release"
# Name "resid - Win32 Debug"
# Begin Source File

SOURCE=..\..\resid\envelope.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\envelope.cc
InputName=envelope

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\envelope.cc
InputName=envelope

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\envelope.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\envelope.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\extfilt.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\extfilt.cc
InputName=extfilt

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\extfilt.cc
InputName=extfilt

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\extfilt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\extfilt.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\filter.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\filter.cc
InputName=filter

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\filter.cc
InputName=filter

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\filter.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\pot.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\pot.cc
InputName=pot

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\pot.cc
InputName=pot

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\pot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\pot.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\sid.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\sid.cc
InputName=sid

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\sid.cc
InputName=sid

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\sid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\sid.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\siddefs.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\spline.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\version.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\version.cc
InputName=version

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\version.cc
InputName=version

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\version.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\voice.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\voice.cc
InputName=voice

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\voice.cc
InputName=voice

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\voice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\voice.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave.cc
InputName=wave

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave.cc
InputName=wave

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave.h
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581__ST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave6581__ST.cc
InputName=wave6581__ST

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave6581__ST.cc
InputName=wave6581__ST

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581__ST.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_P_T.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave6581_P_T.cc
InputName=wave6581_P_T

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave6581_P_T.cc
InputName=wave6581_P_T

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_P_T.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_PS_.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave6581_PS_.cc
InputName=wave6581_PS_

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave6581_PS_.cc
InputName=wave6581_PS_

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_PS_.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_PST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave6581_PST.cc
InputName=wave6581_PST

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave6581_PST.cc
InputName=wave6581_PST

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_PST.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580__ST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave8580__ST.cc
InputName=wave8580__ST

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave8580__ST.cc
InputName=wave8580__ST

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580__ST.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_P_T.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave8580_P_T.cc
InputName=wave8580_P_T

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave8580_P_T.cc
InputName=wave8580_P_T

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_P_T.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_PS_.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave8580_PS_.cc
InputName=wave8580_PS_

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave8580_PS_.cc
InputName=wave8580_PS_

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_PS_.cpp
# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_PST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputPath=..\..\resid\wave8580_PST.cc
InputName=wave8580_PST

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputPath=..\..\resid\wave8580_PST.cc
InputName=wave8580_PST

"..\\..\\resid\\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy ..\\..\\resid\\$(InputName).cc ..\\..\\resid\\$(InputName).cpp /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_PST.cpp
# End Source File
# End Target
# End Project
