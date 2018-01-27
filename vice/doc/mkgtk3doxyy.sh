#!/bin/bash

# Script to generate Doxygen output for Gtk3 only.
#
# Generating the default Doxygen docs takes a long time, and it doesn't even
# split stdout from stderr, so getting a list of warnings/errors isn't easy.
#
# The idea here is to quickly generate Gtk3 UI docs, so I can update/fix
# doxygen docblocks, in the end providing proper Gtk3 widget documentation
# which may help devs other than myself.


# Log file for any Doxygen warnings/errors. Since doxygen outputs *a lot* of
# information, collection warnings/errors into a separate file makes sense
LOG=gtk3-doxy-errors.log

cd ..
doxygen doc/Doxyfile.gtk3 2> "$LOG"
 
