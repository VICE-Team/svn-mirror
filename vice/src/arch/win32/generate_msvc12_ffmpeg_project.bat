@echo off

rem generate_msvc12_ffmpeg_project.bat - generate MSVC12/VS2013 (& FFMPEG) project directory
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
echo generating MSVC12 project as vs12-ffmpeg
if exist vs12-ffmpeg goto removedir
:makemsvcdir
mkdir vs12-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -12 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs12-ffmpeg\make-msvc-bindist.bat
copy msvc\winid*.bat vs12-ffmpeg
copy msvc\debug.h vs12-ffmpeg\debug.h
copy msvc\mon_parse.c vs12-ffmpeg\mon_lex.c
copy msvc\mon_parse.c vs12-ffmpeg\mon_parse.c
copy msvc\mon_parse.h vs12-ffmpeg\mon_parse.h
echo MSVC12 project files generated in vs12-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs12-ffmpeg\*.* /S
rd vs12-ffmpeg /s /q
if exist vs12-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs12-ffmpeg directory, please remove vs12-ffmpeg manually and run this batch file again.
:end
pause
