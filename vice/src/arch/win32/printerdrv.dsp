# Microsoft Developer Studio Project File - Name="printerdrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=printerdrv - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "printerdrv.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "printerdrv.mak" CFG="printerdrv - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "printerdrv - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "printerdrv - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "printerdrv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\printerdrv\Release"
# PROP Intermediate_Dir "libs\printerdrv\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "printerdrv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "printerdrv___Win32_Debug"
# PROP BASE Intermediate_Dir "printerdrv___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\printerdrv\Debug"
# PROP Intermediate_Dir "libs\printerdrv\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "printerdrv - Win32 Release"
# Name "printerdrv - Win32 Debug"
# Begin Source File

SOURCE="..\..\printerdrv\driver-select.c"
# End Source File
# Begin Source File

SOURCE="..\..\printerdrv\drv-ascii.c"
# End Source File
# Begin Source File

SOURCE="..\..\printerdrv\drv-mps803.c"
# End Source File
# Begin Source File

SOURCE="..\..\printerdrv\drv-nl10.c"
# End Source File
# Begin Source File

SOURCE="..\..\printerdrv\interface-serial.c"
# End Source File
# Begin Source File

SOURCE="..\..\printerdrv\interface-userport.c"
# End Source File
# Begin Source File

SOURCE="..\..\printerdrv\output-graphics.c"
# End Source File
# Begin Source File

SOURCE="..\..\printerdrv\output-select.c"
# End Source File
# Begin Source File

SOURCE="..\..\printerdrv\output-text.c"
# End Source File
# Begin Source File

SOURCE=..\..\printerdrv\printer-serial.c
# End Source File
# Begin Source File

SOURCE=..\..\printerdrv\printer-userport.c
# End Source File
# Begin Source File

SOURCE=..\..\printerdrv\printer.c
# End Source File
# End Target
# End Project
