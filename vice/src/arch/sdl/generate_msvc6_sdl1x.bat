@echo off
if not exist ..\win32\vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating SDL1x MSVC6 project as win32-sdl1x-msvc6
if exist win32-sdl1x-msvc6 goto removedir
:makemsvcdir
mkdir win32-sdl1x-msvc6
cd ..\win32\vs_tmpl
mkmsvc.exe -sdl -6 vice
cd ..\..\sdl
copy msvc-files\make-bindist-sdl1x.bat.proto win32-sdl1x-msvc6\make-msvc-bindist.bat
copy msvc-files\debug.h win32-sdl1x-msvc6
copy msvc-files\dirent.h win32-sdl1x-msvc6
copy msvc-files\ide-config.h win32-sdl1x-msvc6
copy msvc-files\ide-siddefs.h win32-sdl1x-msvc6
copy msvc-files\ide-siddtvdefs.h win32-sdl1x-msvc6
copy msvc-files\inttypes.h win32-sdl1x-msvc6
copy msvc-files\stdint.h win32-sdl1x-msvc6
copy msvc-files\ffmpeg-config.h win32-sdl1x-msvc6
copy msvc-files\vice.manifest win32-sdl1x-msvc6
echo SDL1x MSVC6 project files generated in win32-sdl1x-msvc6
goto end
:missingmkmsvc
echo ..\win32\vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del win32-sdl1x-msvc6\*.* /S
rd win32-sdl1x-msvc6 /s /q
if exist win32-sdl1x-msvc6 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the win32-sdl1x-msvc6 directory, please remove win32-sdl1x-msvc6 manually and run this batch file again.
:end
pause
