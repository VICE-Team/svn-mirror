@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC9 project as vs9-ffmpeg
if exist vs9-ffmpeg goto removedir
:makemsvcdir
mkdir vs9-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -9 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs9-ffmpeg\make-msvc-bindist.bat
copy msvc\winid*.bat vs9-ffmpeg
copy msvc\debug.h vs9-ffmpeg\debug.h
echo MSVC9 project files generated in vs9-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs9-ffmpeg\*.* /S
rd vs9-ffmpeg /s /q
if exist vs9-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs9-ffmpeg directory, please remove vs9-ffmpeg manually and run this batch file again.
:end
pause
