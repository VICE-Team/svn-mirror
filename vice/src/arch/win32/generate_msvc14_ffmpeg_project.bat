@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC14 project as vs14-ffmpeg
if exist vs14-ffmpeg goto removedir
:makemsvcdir
mkdir vs14-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -14 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs14-ffmpeg\make-msvc-bindist.bat
copy msvc\winid*.bat vs14-ffmpeg
copy msvc\debug.h vs14-ffmpeg\debug.h
echo MSVC14 project files generated in vs14-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs14-ffmpeg\*.* /S
rd vs14-ffmpeg /s /q
if exist vs14-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs14-ffmpeg directory, please remove vs14-ffmpeg manually and run this batch file again.
:end
pause
