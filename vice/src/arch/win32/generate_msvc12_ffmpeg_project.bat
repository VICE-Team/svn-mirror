@echo off
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
