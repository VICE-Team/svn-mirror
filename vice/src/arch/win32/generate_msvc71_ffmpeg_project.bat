@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC7.1 project as vs71-ffmpeg
if exist vs71-ffmpeg goto removedir
:makemsvcdir
mkdir vs71-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -71 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs71-ffmpeg\make-msvc-bindist.bat
echo MSVC7.1 project files generated in vs71-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs71-ffmpeg\*.* /S
rd vs71-ffmpeg /s /q
if exist vs71-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs71-ffmpeg directory, please remove vs71-ffmpeg manually and run this batch file again.
:end
pause
