# Microsoft Developer Studio Project File - Name="resid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
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
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
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
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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
IntDir=.\libs\resid\Release
InputPath=..\..\resid\envelope.cc
InputName=envelope

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\envelope.cc
InputName=envelope

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\extfilt.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\extfilt.cc
InputName=extfilt

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\extfilt.cc
InputName=extfilt

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\filter.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\filter.cc
InputName=filter

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\filter.cc
InputName=filter

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\pot.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\pot.cc
InputName=pot

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\pot.cc
InputName=pot

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\sid.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\sid.cc
InputName=sid

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\sid.cc
InputName=sid

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\version.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\version.cc
InputName=version

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\version.cc
InputName=version

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\voice.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\voice.cc
InputName=voice

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\voice.cc
InputName=voice

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave.cc
InputName=wave

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave.cc
InputName=wave

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581__ST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave6581__ST.cc
InputName=wave6581__ST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave6581__ST.cc
InputName=wave6581__ST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_P_T.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave6581_P_T.cc
InputName=wave6581_P_T

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave6581_P_T.cc
InputName=wave6581_P_T

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_PS_.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave6581_PS_.cc
InputName=wave6581_PS_

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave6581_PS_.cc
InputName=wave6581_PS_

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave6581_PST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave6581_PST.cc
InputName=wave6581_PST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave6581_PST.cc
InputName=wave6581_PST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580__ST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave8580__ST.cc
InputName=wave8580__ST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave8580__ST.cc
InputName=wave8580__ST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_P_T.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave8580_P_T.cc
InputName=wave8580_P_T

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave8580_P_T.cc
InputName=wave8580_P_T

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_PS_.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave8580_PS_.cc
InputName=wave8580_PS_

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave8580_PS_.cc
InputName=wave8580_PS_

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\resid\wave8580_PST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
IntDir=.\libs\resid\Release
InputPath=..\..\resid\wave8580_PST.cc
InputName=wave8580_PST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
IntDir=.\libs\resid\Debug
InputPath=..\..\resid\wave8580_PST.cc
InputName=wave8580_PST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
