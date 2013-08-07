@echo off
if not exist ..\win32\vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC7.1 project as win32-msvc71
if exist win32-msvc71 goto removedir
:makemsvcdir
mkdir win32-msvc71
cd ..\win32\vs_tmpl
mkmsvc.exe -sdl -71 vice
cd ..\..\sdl
copy msvc-files\make-bindist.bat.proto win32-msvc71\make-msvc-bindist.bat
copy msvc-files\dirent.h win32-msvc71
copy msvc-files\ide-config.h win32-msvc71
copy msvc-files\ide-siddefs.h win32-msvc71
copy msvc-files\ide-siddtvdefs.h win32-msvc71
copy msvc-files\inttypes.h win32-msvc71
copy msvc-files\stdint.h win32-msvc71
copy msvc-files\vice.manifest win32-msvc71
echo MSVC7.1 project files generated in win32-msvc71
goto end
:missingmkmsvc
echo ..\win32\vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del win32-msvc71\*.* /S
rd win32-msvc71 /s /q
if exist win32-msvc71 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the win32-msvc71 directory, please remove win32-msvc71 manually and run this batch file again.
:end
pause
