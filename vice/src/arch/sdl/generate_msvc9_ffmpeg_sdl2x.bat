@echo off
if not exist ..\win32\vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating SDL2x MSVC9 project as win32-sdl2x-msvc9-ffmpeg
if exist win32-sdl2x-msvc9-ffmpeg goto removedir
:makemsvcdir
mkdir win32-sdl2x-msvc9-ffmpeg
cd ..\win32\vs_tmpl
mkmsvc.exe -ffmpeg -sdl2 -9 vice
cd ..\..\sdl
copy msvc-files\make-bindist-sdl2x.bat.proto win32-sdl2x-msvc9-ffmpeg\make-msvc-bindist.bat
copy msvc-files\debug.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\dirent.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\ide-config-sdl2.h win32-sdl2x-msvc9-ffmpeg\ide-config.h
copy msvc-files\ide-siddefs.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\ide-siddtvdefs.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\inttypes.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\stdint.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\vice.manifest win32-sdl2x-msvc9-ffmpeg
copy msvc-files\winid_ia64.bat win32-sdl2x-msvc9-ffmpeg
copy msvc-files\winid_x64.bat win32-sdl2x-msvc9-ffmpeg
copy msvc-files\winid_x86.bat win32-sdl2x-msvc9-ffmpeg
copy msvc-files\ffmpeg-config.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\lame-config.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\x264_config.h win32-sdl2x-msvc9-ffmpeg
copy msvc-files\x264-config.h win32-sdl2x-msvc9-ffmpeg
echo SDL2x MSVC9 project files generated in win32-sdl2x-msvc9-ffmpeg
goto end
:missingmkmsvc
echo ..\win32\vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del win32-sdl2x-msvc9-ffmpeg\*.* /S
rd win32-sdl2x-msvc9-ffmpeg /s /q
if exist win32-sdl2x-msvc9-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the win32-sdl2x-msvc9-ffmpeg directory, please remove win32-sdl2x-msvc9-ffmpeg manually and run this batch file again.
:end
pause
