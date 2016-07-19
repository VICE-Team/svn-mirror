@echo off
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
