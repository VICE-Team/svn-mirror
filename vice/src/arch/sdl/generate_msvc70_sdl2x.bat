@echo off

rem generate_msvc70_sdl2x.bat - generate MSVC7.0/VS2002 SDL2.x project directory
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
echo generating SDL2x MSVC7.0 project as win32-sdl2x-msvc70
if exist win32-sdl2x-msvc70 goto removedir
:makemsvcdir
mkdir win32-sdl2x-msvc70
cd ..\win32\vs_tmpl
mkmsvc.exe -sdl2 -70 vice
cd ..\..\sdl
copy msvc-files\make-bindist-sdl2x.bat.proto win32-sdl2x-msvc70\make-msvc-bindist.bat
copy msvc-files\debug.h win32-sdl2x-msvc70
copy msvc-files\dirent.h win32-sdl2x-msvc70
copy msvc-files\ide-config-sdl2.h win32-sdl2x-msvc70\ide-config.h
copy msvc-files\ide-siddefs.h win32-sdl2x-msvc70
copy msvc-files\ide-siddtvdefs.h win32-sdl2x-msvc70
copy msvc-files\inttypes.h win32-sdl2x-msvc70
copy msvc-files\stdint.h win32-sdl2x-msvc70
copy msvc-files\ffmpeg-config.h win32-sdl2x-msvc70
copy msvc-files\vice.manifest win32-sdl2x-msvc70
echo SDL2x MSVC7.0 project files generated in win32-sdl2x-msvc70
goto end
:missingmkmsvc
echo ..\win32\vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del win32-sdl2x-msvc70\*.* /S
rd win32-sdl2x-msvc70 /s /q
if exist win32-sdl2x-msvc70 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the win32-sdl2x-msvc70 directory, please remove win32-sdl2x-msvc70 manually and run this batch file again.
:end
pause
