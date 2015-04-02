@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC7.0 project as vs70-ffmpeg
if exist vs70-ffmpeg goto removedir
:makemsvcdir
mkdir vs70-ffmpeg
cd vs_tmpl
mkmsvc.exe -ffmpeg -native -70 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs70-ffmpeg\make-msvc-bindist.bat
echo MSVC7.0 project files generated in vs70-ffmpeg
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs70-ffmpeg\*.* /S
rd vs70-ffmpeg /s /q
if exist vs70-ffmpeg goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs70-ffmpeg directory, please remove vs70-ffmpeg manually and run this batch file again.
:end
pause
