@echo off

rem generate_msvcmips_sdl1x.bat - generate Mips SDL1.x MSVC project directory
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

if not exist ..\win32\vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVCMIPS project as winmips-msvc
if exist winmips-msvc goto removedir
:makemsvcdir
mkdir winmips-msvc
cd ..\win32\vs_tmpl
mkmsvc.exe -sdl -4 vice
cd ..\..\sdl
copy msvc-files\make-bindist-mips.bat.proto winmips-msvc\make-msvc-bindist.bat
copy msvc-files\debug.h winmips-msvc
copy msvc-files\dirent.h winmips-msvc
copy msvc-files\ide-config-mips.h winmips-msvc\ide-config.h
copy msvc-files\inttypes.h winmips-msvc
copy msvc-files\stdint.h winmips-msvc
copy msvc-files\ffmpeg-config.h winmips-msvc
echo MSVCMIPS project files generated in winmips-msvc
goto end
:missingmkmsvc
echo ..\win32\vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del winmips-msvc\*.* /S
rd winmips-msvc /s /q
if exist winmips-msvc goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the winmips-msvc directory, please remove winmips-msvc manually and run this batch file again.
:end
pause
