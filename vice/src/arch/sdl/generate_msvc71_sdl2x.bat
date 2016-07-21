@echo off
if not exist ..\win32\vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo SDL2x generating MSVC7.1 project as win32-sdl2x-msvc71
if exist win32-sdl2x-msvc71 goto removedir
:makemsvcdir
mkdir win32-sdl2x-msvc71
cd ..\win32\vs_tmpl
mkmsvc.exe -sdl2 -71 vice
cd ..\..\sdl
copy msvc-files\make-bindist.bat.proto win32-sdl2x-msvc71\make-msvc-bindist.bat
copy msvc-files\debug.h win32-sdl2x-msvc71
copy msvc-files\dirent.h win32-sdl2x-msvc71
copy msvc-files\ide-config-sdl2.h win32-sdl2x-msvc71\ide-config.h
copy msvc-files\ide-siddefs.h win32-sdl2x-msvc71
copy msvc-files\ide-siddtvdefs.h win32-sdl2x-msvc71
copy msvc-files\inttypes.h win32-sdl2x-msvc71
copy msvc-files\stdint.h win32-sdl2x-msvc71
copy msvc-files\ffmpeg-config.h win32-sdl2x-msvc71
copy msvc-files\vice.manifest win32-sdl2x-msvc71
echo SDL2x MSVC7.1 project files generated in win32-sdl2x-msvc71
goto end
:missingmkmsvc
echo ..\win32\vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del win32-sdl2x-msvc71\*.* /S
rd win32-sdl2x-msvc71 /s /q
if exist win32-sdl2x-msvc71 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the win32-sdl2x-msvc71 directory, please remove win32-sdl2x-msvc71 manually and run this batch file again.
:end
pause
