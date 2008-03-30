# Microsoft Developer Studio Project File - Name="vic20" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vic20 - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "vic20.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "vic20.mak" CFG="vic20 - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "vic20 - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "vic20 - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vic20 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "libs\vic20\Release"
# PROP Intermediate_Dir "libs\vic20\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\raster" /I "..\..\monitor" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vic20 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "libs\vic20\Debug"
# PROP Intermediate_Dir "libs\vic20\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I ".\msvc" /I ".\\" /I "..\..\\" /I "..\..\drive" /I "..\..\raster" /I "..\..\monitor" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
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

# Name "vic20 - Win32 Release"
# Name "vic20 - Win32 Debug"
# Begin Source File

SOURCE="..\..\vic20\vic-cmdline-options.c"
# End Source File
# Begin Source File

SOURCE="..\..\vic20\vic-color.c"
# End Source File
# Begin Source File

SOURCE="..\..\vic20\vic-draw.c"
# End Source File
# Begin Source File

SOURCE="..\..\vic20\vic-mem.c"
# End Source File
# Begin Source File

SOURCE="..\..\vic20\vic-resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\vic20\vic-snapshot.c"
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic.c
# End Source File
# Begin Source File

SOURCE="..\..\vic20\vic20-cmdline-options.c"
# End Source File
# Begin Source File

SOURCE="..\..\vic20\vic20-resources.c"
# End Source File
# Begin Source File

SOURCE="..\..\vic20\vic20-snapshot.c"
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20cartridge.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20datasette.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20iec.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20ieeevia1.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20ieeevia2.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20mem.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20rom.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20rsuser.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20sound.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20via1.c
# End Source File
# Begin Source File

SOURCE=..\..\vic20\vic20via2.c
# End Source File
# End Target
# End Project
