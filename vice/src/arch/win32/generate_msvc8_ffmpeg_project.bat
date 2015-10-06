@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC8 project as vs8-ffmpeg
if exist vs8-ffmpeg goto removedir
:makemsvcdir
mkdir vs8-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -8 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs8-ffmpeg\make-msvc-bindist.bat
copy msvc\winid*.bat vs8-ffmpeg
copy msvc\debug.h vs8-ffmpeg\debug.h
echo MSVC8 project files generated in vs8-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs8-ffmpeg\*.* /S
rd vs8-ffmpeg /s /q
if exist vs8-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs8-ffmpeg directory, please remove vs8-ffmpeg manually and run this batch file again.
:end
pause
