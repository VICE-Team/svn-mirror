@Echo Off

echo ------- Make clean -------

if "%1"=="" goto ALL

echo  Cleaning vac++\%1\*.obj
xdel vac++\%1\*.obj  /s /l /n /d
echo  Cleaning vac++\%1\*.ics
xdel vac++\%1\*.ics  /s /l /n /d
goto END

:ALL

if "%1"=="all" goto CLEAN
goto END

:CLEAN
echo Cleaning vac++\*.obj
xdel vac++\*.obj  /s /l /n /d

:END
echo -------    Done    -------
