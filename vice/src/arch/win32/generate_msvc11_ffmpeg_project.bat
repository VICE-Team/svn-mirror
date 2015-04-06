@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC11 project as vs11-ffmpeg
if exist vs11-ffmpeg goto removedir
:makemsvcdir
mkdir vs11-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -11 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs11-ffmpeg\make-msvc-bindist.bat
copy msvc\winid*.bat vs11-ffmpeg
echo MSVC11 project files generated in vs11-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs11-ffmpeg\*.* /S
rd vs11-ffmpeg /s /q
if exist vs11-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs11-ffmpeg directory, please remove vs11-ffmpeg manually and run this batch file again.
:end
pause
