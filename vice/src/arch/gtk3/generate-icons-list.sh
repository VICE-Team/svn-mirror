#!/usr/bin/env bash
#
# Get list of icons used in the Gtk3 UI by parsing *.c files in the current
# directory.
#
# @author:  Bas Wassink <b.wassink@ziggo.nl>
#
#
# To mark an icon file for copying, a Gtk3 UI source file (.c) should contain
# a specific line in a comment:
#
#   "$VICEICON"<whitespace><icon-size>/<category>/<stock-icon-name>
#
# So for example:
#   /* $VICEICON    24x24/actions/document-open */
#
# Currently the Gtk3 code uses only the <stock-icon-name> and adds "-symbolic"
# to it to load an icon. Gtk3 handles the <category> prefix transparently
# (ie it probably uses some index file), and the <icon-size> is set when
# calling gtk_button_new_from_icon_name(name, size), where size is currently
# GTK_ICON_SIZE_LARGE_TOOLBAR, which responds to 24x24 pixels.
#
# The filenames for 'symbolic' icons seem to use '*-symbolic.symbolic.png', so
# so either a script using this script needs to add that, or this script needs
# to add that.


# Use a subshell to parse out icons, and then use sort -u to condense the list:
(
    for f in `find . -name '*.c'`; do
        sed -n "s/^.*\$VICEICON\s*\([/a-zA-Z0-9_-]\+\).*$/\1/p" "$f"
    done
) | sort -u

