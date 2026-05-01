#! /bin/bash

SCRIPTDIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo SCRIPTDIR: $SCRIPTDIR

#SOURCE=$1
SOURCE=$SCRIPTDIR/../src

LOGFILE=snapshots.log
TEXIFILE=snapshots.texi

#echo source: $SOURCE
#echo log: $LOGFILE
#echo texi: $TEXIFILE

rm -f $LOGFILE
rm -f $TEXIFILE

echo "extracting documentation on snapshots (see $LOGFILE)"

########################################################################################

declare -a extratexi=(  "MAINC64CPU" "MAINVIC20CPU" "CPU6809" "MAIN65816CPU"
                          "C64ROM" "C64MEM"
                          "C64SCPUROM" "C64SCPUMEM"
                          "C64DTVROM" "C64DTVMEM"
                          "C128ROM" "C128MEM"
                          "VIC20ROM" "VIC20MEM"
                          "PLUS4ROM" "PLUS4MEM"
                          "PETROM" "PETMEM"
                          "DWWMEM"
#                          "PETDWWPIA"
                          "CBM2ROM" "CBM2MEM"
                          "C500DATA"
                          "SID" "SIDRESID"
                          "CRTC"
                          "MYPIA_NAME"
                          "MYACIA"
                          "VIA"
                          "RIOT"
                          "CIA"
                          "TPI"
                          "VIC-I"
                          "VIC-II" "VIC-IISC" "DTVVIC"
                          "TED"
                        )

# returns "snapshots.texi" or "MODULE.snapshots.texi", depending on whether
# $1 is contained in the above array

# $1 __MODULE__
function texi_check_extra_texi
{
    if [[ ${extratexi[@]} =~ $1 ]]
    then
        echo "@c generated, do not edit" > "$1.$TEXIFILE"
        echo "$1.$TEXIFILE"
    else
        echo "$TEXIFILE"
    fi
}

# $1 __INFILE__
function texi_table
{

    if  [ -n "$__MODULE__" ]; then
        TABLEFILE=`texi_check_extra_texi $__MODULE__`
        echo "TEXI TABLEFILE:"$TABLEFILE >> $LOGFILE
        echo -en "\n@b{The "$__MODULE__" module}\n\n" >> $TABLEFILE
    else
        return
    fi

    if  [ -n "$__SNAP_MAJOR__" ] && [ -n "$__SNAP_MINOR__" ]; then
        echo -en "\nVersion numbers: Major "$__SNAP_MAJOR__", Minor "$__SNAP_MINOR__".\n\n" >> $TABLEFILE
    fi

    if  [ -n "$TABLE3" ]; then
        echo "@multitable @columnfractions .2 .3 .5" >> $TABLEFILE
        echo -en "@item @b{Type}\n@tab @b{Name}\n@tab @b{Description}\n" >> $TABLEFILE
        echo "$TABLE3" | sed 's:^:@item :g' | sed 's:|:\n@tab :g' >> $TABLEFILE
        echo -en "@end multitable\n\n" >> $TABLEFILE
    fi

    if  [ -n "$TABLE4" ]; then
        echo "@multitable @columnfractions .2 .3 .1 .4" >> $TABLEFILE
        echo -en "@item @b{Type}\n@tab @b{Name}\n@tab @b{Version}\n@tab @b{Description}\n" >> $TABLEFILE
        echo "$TABLE4" | sed 's:^:@item :g' | sed 's:|:\n@tab :g' >> $TABLEFILE
        echo -en "@end multitable\n\n" >> $TABLEFILE
    fi

    if  [ -n "$__MODULE__" ] || [ -n "$__SNAP_MAJOR__" ] || [ -n "$__SNAP_MINOR__" ] || [ -n "$TABLE3" ] || [ -n "$TABLE4" ]; then
        echo "@i{ src${F##$SOURCE} }"  >> $TABLEFILE
    fi

    echo "@sp 1"  >> $TABLEFILE

}

# $1 __INFILE__
function log_table
{
    echo __INFILE__=$1 >> $LOGFILE

    if  [ -n "$__MODULE__" ]; then
        echo __MODULE__=$__MODULE__ >> $LOGFILE
    fi

    if  [ -n "$__SNAP_MAJOR__" ]; then
        echo __SNAP_MAJOR__=$__SNAP_MAJOR__ >> $LOGFILE
    fi
    if  [ -n "$__SNAP_MINOR__" ]; then
        echo __SNAP_MINOR__=$__SNAP_MINOR__ >> $LOGFILE
    fi

    if  [ -n "$TABLE3" ]; then
        echo "   Type | Name | Description" >> $LOGFILE
        echo "$TABLE3" >> $LOGFILE
        echo -en "\n" >> $LOGFILE
    fi

    if  [ -n "$TABLE4" ]; then
        echo "   Type | Name | Version | Description" >> $LOGFILE
        echo "$TABLE4" >> $LOGFILE
        echo -en "\n" >> $LOGFILE
    fi
}

########################################################################################

# $1 filename
function grep_module
{
    __MODULE__=`grep "[[:space:]]snap_module_name.*=.*\".*" $1 | sed -e 's:.*"\(.*\)".*;:\1:g' | sed -e 's: /.*::g'`
    if  [ -z "$__MODULE__" ]; then
        __MODULE__=`grep "snap_ram_module_name.*=.*\".*" $1 | sed -e 's:.*"\(.*\)".*;:\1:g' | sed -e 's: /.*::g'`
        if  [ -z "$__MODULE__" ]; then
            __MODULE__=`grep "snap_rom_module_name.*=.*\".*" $1 | sed -e 's:.*"\(.*\)".*;:\1:g' | sed -e 's: /.*::g'`
            if  [ -z "$__MODULE__" ]; then
                __MODULE__=`grep "snap_mem_module_name.*=.*\".*" $1 | sed -e 's:.*"\(.*\)".*;:\1:g' | sed -e 's: /.*::g'`
                if  [ -z "$__MODULE__" ]; then
                    __MODULE__=`grep "flash_snap_module_name.*=.*\".*" $1 | sed -e 's:.*"\(.*\)".*;:\1:g' | sed -e 's: /.*::g'`
                    if  [ -z "$__MODULE__" ]; then
                        __MODULE__=`grep "define.*[[:space:]]SNAP_MODULE_NAME.*\".*" $1 | sed -e 's:#define.*SNAP_MODULE_NAME *"\(.*\)":\1:g' | sed -e 's: /.*::g'`
                        if  [ -z "$__MODULE__" ]; then
                            __MODULE__=`grep "define.*FLASH_SNAP_MODULE_NAME*\".*" $1 | sed -e 's:#define.*FLASH_SNAP_MODULE_NAME *"\(.*\)":\1:g' | sed -e 's: /.*::g'`
                            if  [ -z "$__MODULE__" ]; then
                                __MODULE__=`grep "snapshot_module_create.*(.*\".*" $1 | sed -e 's:.*"\(.*\)".*:\1:g'`
                                if  [ -z "$__MODULE__" ]; then
                                    __MODULE__=`grep "snapshot_create.*(.*\".*" $1 | sed -e 's:.*"\(.*\)".*:\1:g'`
                                fi
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi
}

# $1 filename
function grep_major_minor
{
    __SNAP_MAJOR__=`grep "define SNAP_MAJOR" $1 | sed -e 's:.*define SNAP_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
    if  [ -z "$__SNAP_MAJOR__" ]; then
        __SNAP_MAJOR__=`grep "define CART_DUMP_VER_MAJOR" $1 | sed -e 's:.*define CART_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
        if  [ -z "$__SNAP_MAJOR__" ]; then
            __SNAP_MAJOR__=`grep "define C64CART_DUMP_VER_MAJOR" $1 | sed -e 's:.*define C64CART_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
            if  [ -z "$__SNAP_MAJOR__" ]; then
                __SNAP_MAJOR__=`grep "define FLASH040_DUMP_VER_MAJOR" $1 | sed -e 's:.*define FLASH040_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                if  [ -z "$__SNAP_MAJOR__" ]; then
                    __SNAP_MAJOR__=`grep "define FLASH800_DUMP_VER_MAJOR" $1 | sed -e 's:.*define FLASH800_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                    if  [ -z "$__SNAP_MAJOR__" ]; then
                        __SNAP_MAJOR__=`grep "define TPI_DUMP_VER_MAJOR" $1 | sed -e 's:.*define TPI_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                        if  [ -z "$__SNAP_MAJOR__" ]; then
                            __SNAP_MAJOR__=`grep "define VIA_DUMP_VER_MAJOR" $1 | sed -e 's:.*define VIA_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                            if  [ -z "$__SNAP_MAJOR__" ]; then
                                __SNAP_MAJOR__=`grep "define CIA_DUMP_VER_MAJOR" $1 | sed -e 's:.*define CIA_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                if  [ -z "$__SNAP_MAJOR__" ]; then
                                    __SNAP_MAJOR__=`grep "define RIOT_DUMP_VER_MAJOR" $1 | sed -e 's:.*define RIOT_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                    if  [ -z "$__SNAP_MAJOR__" ]; then
                                        __SNAP_MAJOR__=`grep "define VIC20CART_DUMP_VER_MAJOR" $1 | sed -e 's:.*define VIC20CART_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                        if  [ -z "$__SNAP_MAJOR__" ]; then
                                            __SNAP_MAJOR__=`grep "define ROM_SNAP_MAJOR" $1 | sed -e 's:.*define ROM_SNAP_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                            if  [ -z "$__SNAP_MAJOR__" ]; then
                                                __SNAP_MAJOR__=`grep "define DRIVE_SNAP_MAJOR" $1 | sed -e 's:.*define DRIVE_SNAP_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                                if  [ -z "$__SNAP_MAJOR__" ]; then
                                                    __SNAP_MAJOR__=`grep "define CMDHD_SNAP_MAJOR" $1 | sed -e 's:.*define CMDHD_SNAP_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                                    if  [ -z "$__SNAP_MAJOR__" ]; then
                                                        __SNAP_MAJOR__=`grep "define FDD_SNAP_MAJOR" $1 | sed -e 's:.*define FDD_SNAP_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                                        if  [ -z "$__SNAP_MAJOR__" ]; then
                                                            __SNAP_MAJOR__=`grep "define WD1770_SNAP_MAJOR" $1 | sed -e 's:.*define WD1770_SNAP_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                                        fi
                                                    fi
                                                fi
                                            fi
                                        fi
                                    fi
                                fi
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi
    if  [ -z "$__SNAP_MAJOR__" ]; then
        __SNAP_MAJOR__=`grep "define C500DATA_DUMP_VER_MAJOR" $1 | sed -e 's:.*define C500DATA_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
    fi
    if  [ -z "$__SNAP_MAJOR__" ]; then
        __SNAP_MAJOR__=`grep "define CBM2CART_DUMP_VER_MAJOR" $1 | sed -e 's:.*define CBM2CART_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
    fi
    if  [ -z "$__SNAP_MAJOR__" ]; then
        __SNAP_MAJOR__=`grep "define PIA_DUMP_VER_MAJOR" $1 | sed -e 's:.*define PIA_DUMP_VER_MAJOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
    fi

    __SNAP_MINOR__=`grep "define SNAP_MINOR" $1 | sed -e 's:.*define SNAP_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
    if  [ -z "$__SNAP_MINOR__" ]; then
        __SNAP_MINOR__=`grep "define CART_DUMP_VER_MINOR" $1 | sed -e 's:.*define CART_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
        if  [ -z "$__SNAP_MINOR__" ]; then
            __SNAP_MINOR__=`grep "define C64CART_DUMP_VER_MINOR" $1 | sed -e 's:.*define C64CART_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
            if  [ -z "$__SNAP_MINOR__" ]; then
                __SNAP_MINOR__=`grep "define FLASH040_DUMP_VER_MINOR" $1 | sed -e 's:.*define FLASH040_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                if  [ -z "$__SNAP_MINOR__" ]; then
                    __SNAP_MINOR__=`grep "define FLASH800_DUMP_VER_MINOR" $1 | sed -e 's:.*define FLASH800_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                    if  [ -z "$__SNAP_MINOR__" ]; then
                        __SNAP_MINOR__=`grep "define TPI_DUMP_VER_MINOR" $1 | sed -e 's:.*define TPI_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                        if  [ -z "$__SNAP_MINOR__" ]; then
                            __SNAP_MINOR__=`grep "define VIA_DUMP_VER_MINOR" $1 | sed -e 's:.*define VIA_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                            if  [ -z "$__SNAP_MINOR__" ]; then
                                __SNAP_MINOR__=`grep "define CIA_DUMP_VER_MINOR" $1 | sed -e 's:.*define CIA_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                if  [ -z "$__SNAP_MINOR__" ]; then
                                    __SNAP_MINOR__=`grep "define RIOT_DUMP_VER_MINOR" $1 | sed -e 's:.*define RIOT_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                    if  [ -z "$__SNAP_MINOR__" ]; then
                                        __SNAP_MINOR__=`grep "define VIC20CART_DUMP_VER_MINOR" $1 | sed -e 's:.*define VIC20CART_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                        if  [ -z "$__SNAP_MINOR__" ]; then
                                            __SNAP_MINOR__=`grep "define ROM_SNAP_MINOR" $1 | sed -e 's:.*define ROM_SNAP_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                            if  [ -z "$__SNAP_MINOR__" ]; then
                                                __SNAP_MINOR__=`grep "define DRIVE_SNAP_MINOR" $1 | sed -e 's:.*define DRIVE_SNAP_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                                if  [ -z "$__SNAP_MINOR__" ]; then
                                                    __SNAP_MINOR__=`grep "define CMDHD_SNAP_MINOR" $1 | sed -e 's:.*define CMDHD_SNAP_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                                    if  [ -z "$__SNAP_MINOR__" ]; then
                                                        __SNAP_MINOR__=`grep "define FDD_SNAP_MINOR" $1 | sed -e 's:.*define FDD_SNAP_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                                        if  [ -z "$__SNAP_MINOR__" ]; then
                                                            __SNAP_MINOR__=`grep "define WD1770_SNAP_MINOR" $1 | sed -e 's:.*define WD1770_SNAP_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
                                                        fi
                                                    fi
                                                fi
                                            fi
                                        fi
                                    fi
                                fi
                            fi
                        fi
                    fi
                fi
            fi
        fi
    fi
    if  [ -z "$__SNAP_MINOR__" ]; then
        __SNAP_MINOR__=`grep "define C500DATA_DUMP_VER_MINOR" $1 | sed -e 's:.*define C500DATA_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
    fi
    if  [ -z "$__SNAP_MINOR__" ]; then
        __SNAP_MINOR__=`grep "define CBM2CART_DUMP_VER_MINOR" $1 | sed -e 's:.*define CBM2CART_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
    fi
    if  [ -z "$__SNAP_MINOR__" ]; then
        __SNAP_MINOR__=`grep "define PIA_DUMP_VER_MINOR" $1 | sed -e 's:.*define PIA_DUMP_VER_MINOR *\(.*\) *:\1:g' | sed -e 's: /.*::g'`
    fi
}

########################################################################################

for F in `find $SOURCE -iname "*.c" -exec grep -lin snapshot {} \;`; do
#for F in `find $SOURCE -iname "*.c" -exec grep -lin "snapshot module" {} \;`; do

#    echo scanning: $F
    echo -ne "."

#    __SVNREVISION__=`svn log -l 1 -q $F | grep -v "^-" | sed 's:r\(.*\) | .* | .*:\1:g'`
#    echo __SVNREVISION__=$__SVNREVISION__ >> $LOGFILE

#type  | name            | description

    # check how many (legit) tables we find
    TABLES="`grep -i "snapshot module" $F | grep -i "format" | wc -l`"
    echo TABLES:$TABLES >> $LOGFILE

    if [ $TABLES == "0" ]; then

        echo "no table: " $F >> $LOGFILE;

    elif [ $TABLES == "1" ]; then

# extract table (one!) directly (old method)
        grep_module $F
        grep_major_minor $F

        FULL=`grep -i -A 50 "snapshot module.*format" $F`

        # extract only the part until next closing comment
        TAB=`echo "$FULL" | awk '/*\// {exit} {print}'`

        if  [ -z "$__MODULE__" ]; then
            # FIXME: what other formats are in the source?
            II=`echo "$TAB" | grep -i "snapshot module" | grep -i "format"`
            # parse headline for the table "<modulename> <module version> * snapshot module *
            __MODULE__=`echo "$II" | sed -e 's:^[/\* ]*\([^ ]*\).*:\1:g'`
        fi

#        echo MODULE "$__MODULE__" >> $LOGFILE
#        echo TAB:"$TAB" >> $LOGFILE

        TABLE3=""
        TABLE4=""
        if  [ -n "`echo "$TAB" | grep -i '.*type.*|.*name.*|.*version.*|.*description.*'`" ]; then
            TABLE4=`echo "$TAB" | grep '[^|]*| [^|]*| [^|]*|[^|]*' | grep -iv '.*type.*|.*name.*|.*version.*|.*description.*'`
        else
            if  [ -n "`echo "$TAB" | grep -i '.*type.*|.*name.*|.*description.*'`" ]; then
                TABLE3=`echo "$TAB" | grep '[^|]*| [^|]*|[^|]*' | grep -iv '.*type.*|.*name.*|.*description.*'`
            fi
        fi

        log_table $F
        texi_table $F

    else

#if there are two or more tables in the same file, we use a different/new method

        # read grep hits into an array (each entry in the array contains multiple lines)
        readarray -t tabarray <<< `grep -i "snapshot module" $F`

        for I in "${tabarray[@]}"; do

#            echo ARRAY:"$I" >> $LOGFILE

            # FIXME: what other formats are in the source?
            II=`echo "$I" | grep -i "snapshot module" | grep -i "format"`

            # parse headline for the table "<modulename> <module version> * snapshot module *
            __MODULE__=`echo "$II" | sed -e 's:^[/\* ]*\([^ ]*\).*:\1:g'`
            __SNAP_MAJOR__=`echo "$II" | sed -e 's:^[/\* ]*\([^ ]*\) \([^ ]*\)\..*:\2:g'`
            __SNAP_MINOR__=`echo "$II" | sed -e 's:^[/\* ]*\([^ ]*\) \([^ ]*\)\.\([^ ]*\).*:\3:g'`
#            echo module:"$__MODULE__" >> $LOGFILE
#            echo major:"$__SNAP_MAJOR__" >> $LOGFILE
#            echo minor:"$__SNAP_MINOR__" >> $LOGFILE

            FULL=`grep -A50 "$I" "$F"`
#            echo FULL:"$FULL" >> $LOGFILE

            # extract only the part until next closing comment
            TAB=`echo "$FULL" | awk '/*\// {exit} {print}'`
#            echo TAB:"$TAB" >> $LOGFILE

            TABLE3=""
            TABLE4=""
            if  [ -n "`echo "$TAB" | grep -i '.*type.*|.*name.*|.*version.*|.*description.*'`" ]; then
                TABLE4=`echo "$TAB" | grep '.*| .*| [^|]*|.*' | grep -iv '.*type.*|.*name.*|.*version.*|.*description.*'`
            else
                if  [ -n "`echo "$TAB" | grep -i '.*type.*|.*name.*|.*description.*'`" ]; then
                    TABLE3=`echo "$TAB" | grep '.*| [^|]*|.*' | grep -iv '.*type.*|.*name.*|.*description.*'`
                fi
            fi

            log_table $F
            texi_table $F


        done

    fi

    echo -ne "\n\n" >> $LOGFILE

done

echo -ne "\n"

# FIXME: writes data to a (top)module (info not listed)
#src/core/mc6821core.c
#src/core/i8255a.c
#src/sid/fastsid.c
#src/sid/parsid.c
#src/sid/hardsid.c
#src/sid/usbsid.c
#src/sid/catweaselmkiii.c
#src/viciisc/vicii-draw-cycle.c
#src/raster/raster-snapshot.c
#src/core/ciatimer.c
#src/core/mc6821core.c
#src/core/i8255a.c
#src/interrupt.c
#src/scpu64/scpu64cpu.c
#src/joyport/mouse.c
#src/plus4/plus4acia.c


# FIXME: snapshot not implemented (listed without table)
#src/c64/cart/shortbus_etfe.c
#src/c64/cart/magicvoice.c
#src/c64/cart/mmcreplay.c
#src/c64/cart/rrnetmk3.c
#src/c64/cart/ethernetcart.c
#src/tapeport/tapecart.c
#src/vic20/cart/vic20-ieee488.c
#src/core/cs8900.c
#src/core/ser-eeprom.c
#src/core/t6721.c
#src/core/spi-sdcard.c
#src/core/fmopl.c
#src/vdc/vdc-snapshot.c
#src/userport/userport_ps2mouse.c
#src/drive/tcbm/tcbm.c
#src/vdrive/vdrive-snapshot.c
#src/tape/tape-snapshot.c (t64)


# FIXME: cart+flash (extra flash040 modules won't be listed yet)
#src/vic20/cart/vic-fp.c
#src/vic20/cart/finalexpansion.c
#src/vic20/cart/ultimem.c
#src/c64/cart/easyflash.c
#src/c64/cart/gmod2.c
#src/c64/cart/retroreplay.c
#src/c128/cart/gmod2c128.c
