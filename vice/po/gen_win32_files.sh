#!/bin/sh
#
# gen_win32_files.sh - win32 res*.rc regeneration helper script
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>

while test x"$1" != "x"
do
    file=$1
    checkfile=$(echo $file | awk '{ string=substr($1, 1, 18); print string; }' )
    realfile=${file%%.po.c}
    if test x"$checkfile" = "x../src/arch/win32/"; then
        echo regenerating $realfile
        ./gen_win32_rc $realfile
        cat >$realfile ../src/arch/win32/temp_en.rc ../src/arch/win32/temp_da.rc \
                       ../src/arch/win32/temp_de.rc ../src/arch/win32/temp_es.rc \
                       ../src/arch/win32/temp_fr.rc ../src/arch/win32/temp_hu.rc \
                       ../src/arch/win32/temp_it.rc ../src/arch/win32/temp_ko.rc \
                       ../src/arch/win32/temp_nl.rc ../src/arch/win32/temp_pl.rc \
                       ../src/arch/win32/temp_ru.rc ../src/arch/win32/temp_sv.rc \
                       ../src/arch/win32/temp_tr.rc
        rm ../src/arch/win32/temp_en.rc
        rm ../src/arch/win32/temp_da.rc
        rm ../src/arch/win32/temp_de.rc
        rm ../src/arch/win32/temp_es.rc
        rm ../src/arch/win32/temp_fr.rc
        rm ../src/arch/win32/temp_hu.rc
        rm ../src/arch/win32/temp_it.rc
        rm ../src/arch/win32/temp_ko.rc
        rm ../src/arch/win32/temp_nl.rc
        rm ../src/arch/win32/temp_pl.rc
        rm ../src/arch/win32/temp_ru.rc
        rm ../src/arch/win32/temp_sv.rc
        rm ../src/arch/win32/temp_tr.rc
    fi
    shift
done
