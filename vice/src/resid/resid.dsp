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
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
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
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
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

SOURCE=.\envelope.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\envelope.cc
InputName=envelope

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\envelope.cc
InputName=envelope

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\extfilt.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\extfilt.cc
InputName=extfilt

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\extfilt.cc
InputName=extfilt

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\filter.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\filter.cc
InputName=filter

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\filter.cc
InputName=filter

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pot.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\pot.cc
InputName=pot

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\pot.cc
InputName=pot

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sid.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\sid.cc
InputName=sid

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\sid.cc
InputName=sid

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\version.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\version.cc
InputName=version

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\version.cc
InputName=version

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\voice.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\voice.cc
InputName=voice

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\voice.cc
InputName=voice

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave.cc
InputName=wave

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave.cc
InputName=wave

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave6581__ST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave6581__ST.cc
InputName=wave6581__ST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave6581__ST.cc
InputName=wave6581__ST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave6581_P_T.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave6581_P_T.cc
InputName=wave6581_P_T

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave6581_P_T.cc
InputName=wave6581_P_T

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave6581_PS_.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave6581_PS_.cc
InputName=wave6581_PS_

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave6581_PS_.cc
InputName=wave6581_PS_

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave6581_PST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave6581_PST.cc
InputName=wave6581_PST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave6581_PST.cc
InputName=wave6581_PST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave8580__ST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave8580__ST.cc
InputName=wave8580__ST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave8580__ST.cc
InputName=wave8580__ST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave8580_P_T.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave8580_P_T.cc
InputName=wave8580_P_T

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave8580_P_T.cc
InputName=wave8580_P_T

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave8580_PS_.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave8580_PS_.cc
InputName=wave8580_PS_

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave8580_PS_.cc
InputName=wave8580_PS_

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\wave8580_PST.cc

!IF  "$(CFG)" == "resid - Win32 Release"

# Begin Custom Build
InputDir=.
IntDir=.\Release
InputPath=.\wave8580_PST.cc
InputName=wave8580_PST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /ML /W3 /GX /Z7 /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ELSEIF  "$(CFG)" == "resid - Win32 Debug"

# Begin Custom Build
InputDir=.
IntDir=.\Debug
InputPath=.\wave8580_PST.cc
InputName=wave8580_PST

"$(IntDir)\\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl.exe /nologo /MLd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D\
  PACKAGE=\"resid\" /D VERSION=\"0.7\" /D SIZEOF_INT=4 /Fp"$(INTDIR)\resid.pch"\
  /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c /TP "$(InputDir)\\$(InputName).cc"

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
