@echo off

rem generate_msvc71_ffmpeg_project.bat - generate MSVC7.1/VS2003 (& FFMPEG) project directory
rem
rem Written by
rem  Marco van den Heuvel <blackystardust68@yahoo.com>
rem
rem This file is part of VICE, the Versatile Commodore Emulator.
rem See README for copyright notice.
rem
rem  This program is free software; you can redistribute it and/or modify
rem  it under the terms of the GNU General Public License as published by
rem  the Free Software Foundation; either version 2 of the License, or
rem  (at your option) any later version.
rem
rem  This program is distributed in the hope that it will be useful,
rem  but WITHOUT ANY WARRANTY; without even the implied warranty of
rem  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem  GNU General Public License for more details.
rem
rem  You should have received a copy of the GNU General Public License
rem  along with this program; if not, write to the Free Software
rem  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
rem  02111-1307  USA.

if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC7.1 project as vs71-ffmpeg
if exist vs71-ffmpeg goto removedir
:makemsvcdir
mkdir vs71-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -71 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs71-ffmpeg\make-msvc-bindist.bat
copy msvc\debug.h vs71-ffmpeg\debug.h
copy msvc\mon_parse.c vs71-ffmpeg\mon_lex.c
copy msvc\mon_parse.c vs71-ffmpeg\mon_parse.c
copy msvc\mon_parse.h vs71-ffmpeg\mon_parse.h
echo MSVC7.1 project files generated in vs71-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs71-ffmpeg\*.* /S
rd vs71-ffmpeg /s /q
if exist vs71-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs71-ffmpeg directory, please remove vs71-ffmpeg manually and run this batch file again.
:end
pause
