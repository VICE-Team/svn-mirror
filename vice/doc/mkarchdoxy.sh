#!/bin/bash

# Script to generate Doxygen output for the new archdep code (src/arch/shared)
#
# Generating the default Doxygen docs takes a long time, and it doesn't even
# split stdout from stderr, so getting a list of warnings/errors isn't easy.

# Log file for any Doxygen warnings/errors. Since doxygen outputs *a lot* of
# information, collection warnings/errors into a separate file makes sense
LOG=archdep-doxy-errors.log

cd ..
mkdir -p doc/doxy/archdep
doxygen doc/Doxyfile.archdep 2> "$LOG"
