@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC10 project as vs10-ffmpeg
if exist vs10-ffmpeg goto removedir
:makemsvcdir
mkdir vs10-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -10 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs10-ffmpeg\make-msvc-bindist.bat
copy msvc\winid*.bat vs10-ffmpeg
echo MSVC10 project files generated in vs10-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs10-ffmpeg\*.* /S
rd vs10-ffmeg /s /q
if exist vs10-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs10-ffmpeg directory, please remove vs10-ffmpeg manually and run this batch file again.
:end
pause
