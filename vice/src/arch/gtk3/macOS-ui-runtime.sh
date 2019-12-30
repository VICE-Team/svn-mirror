#
# macOS-runtime.sh - GTK specific runtime init
#
# Written by
#  David Hogan <david.q.hogan@gmail.com>
#
# Intended to be combined with common runtime init and embedded
# in launcher script via 'source'
#
# This file is part of VICE, the Versatile Commodore Emulator.
# See README for copyright notice.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
#  02111-1307  USA.
#

# setup gtk environment
# See https://gitlab.gnome.org/GNOME/gtk-mac-bundler

# Fontconfig
export FONTCONFIG_PATH="$BUNDLE_LIB/vice/fonts"

export XDG_CONFIG_DIRS="$BUNDLE_ETC"/xdg
export XDG_DATA_DIRS="$BUNDLE_SHARE"
export GTK_DATA_PREFIX="$BUNDLE_RESOURCES"
export GTK_EXE_PREFIX="$BUNDLE_RESOURCES"
export GTK_PATH="$BUNDLE_RESOURCES"

export GDK_PIXBUF_MODULE_FILE="$BUNDLE_LIB/gdk-pixbuf-2.0/2.10.0/loaders.cache"
if [ `uname -r | cut -d . -f 1` -ge 10 ]; then
    export GTK_IM_MODULE_FILE="$BUNDLE_ETC/gtk-3.0/gtk.immodules"
fi

I18NDIR="$BUNDLE_SHARE/locale"

# Set the locale-related variables appropriately:
unset LANG LC_MESSAGES LC_MONETARY LC_COLLATE

# Has a language ordering been set?
# If so, set LC_MESSAGES and LANG accordingly; otherwise skip it.
# First step uses sed to clean off the quotes and commas, to change - to _, and change the names for the chinese scripts from "Hans" to CN and "Hant" to TW.
APPLELANGUAGES=`defaults read .GlobalPreferences AppleLanguages | sed -En   -e 's/\-/_/' -e 's/Hant/TW/' -e 's/Hans/CN/' -e 's/[[:space:]]*\"?([[:alnum:]_]+)\"?,?/\1/p' `
if test "$APPLELANGUAGES"; then
  # A language ordering exists.
  # Test, item per item, to see whether there is an corresponding locale.
  for L in $APPLELANGUAGES; do
    # test for exact matches:
    if test -f "$I18NDIR/${L}/LC_MESSAGES/$BUNDLE_NAME.mo"; then
      export LANG=$L
      break
    fi

    # This is a special case, because often the original strings are in US
    # English and there is no translation file.
    if test "x$L" == "xen_US"; then
      export LANG=$L
      break
    fi

    # OK, now test for just the first two letters:
    if test -f "$I18NDIR/${L:0:2}/LC_MESSAGES/$BUNDLE_NAME.mo"; then
      export LANG=${L:0:2}
      break
    fi
  
    # Same thing, but checking for any english variant.
    if test "x${L:0:2}" == "xen"; then
      export LANG=$L
      break
    fi;
  done  
fi

# If we didn't get a language from the language list, try the Collation preference, in case it's the only setting that exists.
APPLECOLLATION=`defaults read .GlobalPreferences AppleCollationOrder`
if test -z "${LANG}" -a -n "$APPLECOLLATION"; then
  if test -f "$I18NDIR/${APPLECOLLATION:0:2}/LC_MESSAGES/$BUNDLE_NAME.mo"; then
    export LANG=${APPLECOLLATION:0:2}
  fi
fi
if test ! -z $APPLECOLLATION; then
  export LC_COLLATE=$APPLECOLLATION
fi

# Continue by attempting to find the Locale preference.
APPLELOCALE=`defaults read .GlobalPreferences AppleLocale`

if test -f "$I18NDIR/${APPLELOCALE:0:5}/LC_MESSAGES/$BUNDLE_NAME.mo"; then
  if test -z $LANG; then 
    export LANG="${APPLELOCALE:0:5}"
  fi
elif test -z $LANG -a -f "$I18NDIR/${APPLELOCALE:0:2}/LC_MESSAGES/$BUNDLE_NAME.mo"; then
  export LANG="${APPLELOCALE:0:2}"
fi

# Next we need to set LC_MESSAGES. If at all possilbe, we want a full
# 5-character locale to avoid the "Locale not supported by C library"
# warning from Gtk -- even though Gtk will translate with a
# two-character code.
if test -n $LANG; then 
  # If the language code matches the applelocale, then that's the message
  # locale; otherwise, if it's longer than two characters, then it's
  # probably a good message locale and we'll go with it.
  if test $LANG == ${APPLELOCALE:0:5} -o $LANG != ${LANG:0:2}; then
    export LC_MESSAGES=$LANG
    # Next try if the Applelocale is longer than 2 chars and the language
    # bit matches $LANG
  elif test $LANG == ${APPLELOCALE:0:2} -a $APPLELOCALE > ${APPLELOCALE:0:2}; then
    export LC_MESSAGES=${APPLELOCALE:0:5}
    # Fail. Get a list of the locales in $PREFIX/share/locale that match
    # our two letter language code and pick the first one, special casing
    # english to set en_US
  elif test $LANG == "en"; then
    export LC_MESSAGES="en_US"
  else
    LOC=`find $PREFIX/share/locale -name $LANG???`
    for L in $LOC; do 
      export LC_MESSAGES=$L
    done
  fi
else
  # All efforts have failed, so default to US english
  export LANG="en_US"
  export LC_MESSAGES="en_US"
fi

CURRENCY=`echo $APPLELOCALE |  sed -En 's/.*currency=([[:alpha:]]+).*/\1/p'`
if test "x$CURRENCY" != "x"; then 
  #  The user has set a special currency. Gtk doesn't install LC_MONETARY files, but Apple does in /usr/share/locale, so we're going to look there for a locale to set LC_CURRENCY to.
  if test -f /usr/local/share/$LC_MESSAGES/LC_MONETARY; then
    if test -a `cat /usr/local/share/$LC_MESSAGES/LC_MONETARY` == $CURRENCY; then
      export LC_MONETARY=$LC_MESSAGES
    fi
  fi
  
  # Avoid running into set -o nounset
  LC_MONETARY=$(echo $LC_MONETARY)
  if test -z "$LC_MONETARY"; then 
    FILES=`find /usr/share/locale -name LC_MONETARY -exec grep -H $CURRENCY {} \;`
    if test -n "$FILES"; then 
      export LC_MONETARY=`echo $FILES | sed -En 's%/usr/share/locale/([[:alpha:]_]+)/LC_MONETARY.*%\1%p'`
    fi
  fi
fi

# No currency value means that the AppleLocale governs:
if test -z "$LC_MONETARY"; then
  LC_MONETARY=${APPLELOCALE:0:5}
fi

# For Gtk, which only looks at LC_ALL:
export LC_ALL=$LC_MESSAGES

if test -f "$BUNDLE_LIB/charset.alias"; then
    export CHARSETALIASDIR="$BUNDLE_LIB"
fi

