#!/bin/bash

#
# cmake-bootstrap.sh
#
#  Take a configured source tree, and generate a cmake build system
#  from the makefiles generated by autotools. Useful for generating
#  IDE project files.
#
# Written by
#  David Hogan <david.q.hogan@gmail.com>
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

cd "$(dirname "$0")"
set -o errexit
set -o nounset

# Remove any previous run
find . -type f -name 'CMakeLists.txt' -exec rm {} \;
find . -type f -name 'CMakeCache.txt' -exec rm {} \;
find . -type f -name 'cmake_install.cmake' -exec rm {} \;
find . -type d -name 'CMakeFiles' | xargs -IQQQ rm -rf "QQQ"

# Initialise user variables if not set
if [ -z ${CFLAGS+x} ]
then
	CFLAGS=""
fi

if [ -z ${CXXFLAGS+x} ]
then
	CXXFLAGS=""
fi

if [ -z ${OBJCFLAGS+x} ]
then
	OBJCFLAGS=""
fi

if [ -z ${LDFLAGS+x} ]
then
	LDFLAGS=""
fi

# Quiet versions of pushd and popd
function pushdq {
	pushd $1 > /dev/null
}

function popdq {
	popd > /dev/null
}

function unique_preserve_order {
	awk '!x[$0]++'
}

function space {
	echo -n ' '
}

function extract_make_var {
	local varname=$1

	#
	# Sadly make --eval doesn't work in macOS, so we simulate a
	# make --eval='extract_make_var: ; @echo TESTS $(TESTS)' extract_make_var
	# by modifying a copy of the Makefile.
	#

	cp Makefile Makefile.bak

	# Try to make this non-destuctive on ctrl-c
	trap "[ -f Makefile.bak ] && mv Makefile.bak Makefile" EXIT

	echo -e "\nextract_make_var:\n\t@echo \$($varname)" >> Makefile
	
	local result=$(make extract_make_var)
	echo -n $result

	mv Makefile.bak Makefile
	trap - EXIT
}

function extract_include_dirs {
	(extract_make_var AM_CPPFLAGS; space; extract_make_var VICE_CFLAGS; space; extract_make_var VICE_CXXFLAGS) \
		| sed $'s/ -/\\\n-/g' | grep '^-I' | sed 's/^-I//' | unique_preserve_order | tr "\n" " "
}

function extract_link_dirs {
	(extract_make_var AM_LDFLAGS; space; extract_make_var VICE_LDFLAGS; ) \
		| sed $'s/ -/\\\n-/g' | grep '^-L' | sed 's/^-L//' | unique_preserve_order | tr "\n" " "
}

function extract_c_compile_definitions {
	extract_make_var COMPILE \
		| sed $'s/ -/\\\n-/g' | grep '^-D' | sed -e 's/^-D//g' | tr "\n" " "
}

function extract_cxx_compile_definitions {
	extract_make_var CXXCOMPILE \
		| sed $'s/ -/\\\n-/g' | grep '^-D' | sed -e 's/^-D//g' | tr "\n" " "
}

function extract_objc_compile_definitions {
	extract_make_var OBJCCOMPILE \
		| sed $'s/ -/\\\n-/g' | grep '^-D' | sed -e 's/^-D//g' | tr "\n" " "
}

function extract_c_cxx_objc_flags {
	local flags=""

	#
	# We handling include dirs and definitiions directly, so
	# we exclude these from the extracted cflags/cxxflags.
	# Also don't include macos sdk target flags to avoid
	# lots of warnings when linking to newer deps.
	#

	while (( "$#" )); do
		case "$1" in
			-I*)
				shift 
				;;
			-framework)
				shift 2
				;;
			-Wl,-framework,*)
				shift
				;;
			-Wl,-framework)
				shift 2
				;;
			-D*)
				shift
				;;
			-mmacosx-version*)
				shift 
				;;
			*)
				if [ -z "$flags" ]
				then
					flags=$1
				else
					flags="$flags\n$1"
				fi
				shift
				;;
		esac
	done

	#
	# Echo the extracted flags.
	#
	# But also don't warn about using deprecated macOS stuff. We are supporting
	# old versions of macOS so we need to use them.
	#

	echo -n -e "-Wno-deprecated-declarations $flags" | tr "\n" " "
}

function extract_cflags {
	extract_c_cxx_objc_flags $(extract_make_var AM_CFLAGS; space; extract_make_var CFLAGS; echo -n " $CFLAGS")
}

function extract_cxxflags {
	extract_c_cxx_objc_flags $(extract_make_var AM_CXXFLAGS; space; extract_make_var CXXFLAGS; echo -n " $CXXFLAGS")
}

function extract_objcflags {
	extract_c_cxx_objc_flags $(extract_make_var AM_OBJCFLAGS; space; extract_make_var OBJCFLAGS; echo -n " $OBJCFLAGS")
}

function extract_ldflags {
	local executable=$1
	extract_c_cxx_objc_flags \
		$(extract_make_var \
			${executable}_LDFLAGS; space; \
			extract_make_var AM_LDFLAGS; space; \
			extract_make_var LDFLAGS; \
			echo -n " $LDFLAGS")
}

function extract_internal_libs {
	local libs=""

	while (( "$#" )); do
		case "$1" in
			*.a)
				lib=$(echo $1 | sed -e 's/.*\(lib.*\)\.a/\1/')
				if [ -z "$libs" ]
				then
					libs=$lib
				else
					libs="$libs\n$lib"
				fi
				shift
				;;
			*.o)
				>&2 echo "Error: Attempting to link to .o files is not currently supported ($1)."
				exit 1
				;;
			*)
				shift
				;;
		esac
	done

	echo -n -e "$libs" | tr "\n" " "
}

function extract_external_libs {
	local libs=""

	while (( "$#" )); do
		case "$1" in
			-l*)
				libs="$libs\n$(echo "$1" | sed 's/^-l//')"
				shift 
				;;
			-framework)
				libs="$libs\n$2"
				shift 2
				;;
			-Wl,-framework,*)
				libs="$libs\n$(echo "$1" | sed 's/^-Wl,-framework,//')"
				shift
				;;
			-Wl,-framework)
				libs="$libs\n$(echo "$2" | sed 's/^-Wl,//')"
				shift 2
				;;
			*)
				shift
				;;
		esac
	done

	echo -n -e "$libs" | tr "\n" " "
}

function extract_sources {
	extract_make_var $1 \
		| tr " " "\n" \
		| grep '\.\(c\|cc\|cpp\|m\)$' \
		| tr "\n" " " \
		| sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//'
}

function extract_object_sources {
	for object_file_basename in $(extract_make_var $1 | tr " " "\n" | grep '\.o$' | sed 's/\.o$//')
	do
		ls -1 \
			| grep "^${object_file_basename}\.\(c\|cc\|cpp\|m\)$" \
			| tr "\n" " "
	done
}

function extract_headers {
	extract_make_var $1 \
		| tr " " "\n" \
		| grep '\.\(h\|hh\|hpp\)$' \
		| tr "\n" " " \
		| sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//'
}

function project_relative_folder {
	local _pwd=$(pwd)
	echo -n "${_pwd#$ROOT_FOLDER/}"
}

#
# Recursively work though the configured Makefile tree, defining all the libs
#

ROOT_FOLDER="$(pwd)"

function process_source_makefile {
	local dir=$1
	
	pushdq $dir

	echo -n "Creating $(project_relative_folder)/CMakeLists.txt ("
	touch CMakeLists.txt

	#
	# Declare each built lib in the original Makefile
	#

	local first=true
	for lib_to_build in $(extract_make_var noinst_LIBRARIES | sed 's/\.a//g')
	do
		if $first
		then
			echo -n "$lib_to_build"
			first=false
		else
			echo -n ", $lib_to_build"
		fi

		cat <<-HEREDOC >> CMakeLists.txt
			add_library($lib_to_build)
			
			target_compile_definitions(
			    $lib_to_build
			    PRIVATE
			        \$<\$<COMPILE_LANGUAGE:C>:$(extract_c_compile_definitions)>
			        \$<\$<COMPILE_LANGUAGE:CXX>:$(extract_cxx_compile_definitions)>
			        \$<\$<COMPILE_LANGUAGE:OBJC>:$(extract_objc_compile_definitions)>
			    )

			target_include_directories(
			    $lib_to_build
			    PRIVATE
			        \${CMAKE_CURRENT_SOURCE_DIR}
			        $(extract_include_dirs)
			    )

			target_compile_options(
			    $lib_to_build
			    PRIVATE
			        \$<\$<COMPILE_LANGUAGE:C>:$(extract_cflags)>
			        \$<\$<COMPILE_LANGUAGE:CXX>:$(extract_cxxflags)>
			        \$<\$<COMPILE_LANGUAGE:OBJC>:$(extract_objcflags)>
			    )

			target_sources(
			    $lib_to_build
			    PRIVATE
			        $(extract_sources ${lib_to_build}_a_SOURCES)
			        $(extract_headers ${lib_to_build}_a_SOURCES)
			        $(extract_object_sources ${lib_to_build}_a_DEPENDENCIES)
			        $(extract_object_sources ${lib_to_build}_a_LIBADD)
			        $(extract_sources BUILT_SOURCES)
			        $(extract_headers BUILT_SOURCES)
			        $(extract_headers noinst_HEADERS)
			    )

		HEREDOC
	done

	echo ")"

	#
	# Generate any necessary sources
	#

	local generated_sources=$(extract_make_var BUILT_SOURCES)
	if [ ! -z "$generated_sources" ]
	then
		echo "- generating sources: $generated_sources"
		make -s $generated_sources
	fi

	#
	# Recursively process subdirs.
	#

	for subdir in $(extract_make_var SUBDIRS)
	do
		cat <<-HEREDOC >> CMakeLists.txt
			add_subdirectory($subdir)
		HEREDOC

		process_source_makefile $subdir
	done

	popdq
}

process_source_makefile src

function external_lib_label {
	echo -n "LIB_$(echo "$1" | tr '[a-z]' '[A-Z]' | sed -e 's/[^A-Z0-9_]/_/g')"
}

function add_executable_target {
	local executable=$1

	#
	# Each executable has its own list of external libs to be linked with.
	#
	
	LIB_ARGS="$(extract_make_var LIBS) $(extract_make_var ${executable}_LDADD)"
	LIB_LIST="$(extract_internal_libs $LIB_ARGS)"

	for lib in $(extract_external_libs $LIB_ARGS)
	do
		label=$(external_lib_label $lib)
		
		LIB_LIST="$LIB_LIST \${$label}"
	done
	
	cat <<-HEREDOC >> CMakeLists.txt

		add_executable($executable)

		target_compile_definitions(
		    $executable
		    PRIVATE
		        \$<\$<COMPILE_LANGUAGE:C>:$(extract_c_compile_definitions)>
		        \$<\$<COMPILE_LANGUAGE:CXX>:$(extract_cxx_compile_definitions)>
		        \$<\$<COMPILE_LANGUAGE:OBJC>:$(extract_objc_compile_definitions)>
		    )

		target_include_directories(
		    $executable
		    PRIVATE
		        \${CMAKE_CURRENT_SOURCE_DIR}
		        $(extract_include_dirs)
		    )
		
		target_compile_options(
		    $executable
		    PRIVATE
		        \$<\$<COMPILE_LANGUAGE:C>:$(extract_cflags)>
		        \$<\$<COMPILE_LANGUAGE:CXX>:$(extract_cxxflags)>
		        \$<\$<COMPILE_LANGUAGE:OBJC>:$(extract_objcflags)>
		    )
		
		target_link_options(
		    $executable
		    PRIVATE
		        $(extract_ldflags $executable)
		    )

		target_sources(
		    $executable
		    PRIVATE
		        $(extract_sources ${executable}_SOURCES)
		        $(extract_headers ${executable}_SOURCES)
		        $(extract_sources EXTRA_${executable}_SOURCES)
		        $(extract_headers EXTRA_${executable}_SOURCES)
		        $(extract_sources BUILT_SOURCES)
		        $(extract_headers BUILT_SOURCES)
		        $(extract_headers noinst_HEADERS)
		    )
		
		target_link_libraries(
		    $executable
		    PRIVATE
		    	$LIB_LIST
		    )
	HEREDOC
}

#
# Filter the list of excutables to those in the Makefile (x64 is optional)
#

POSSIBLE_EXECUTABLES="x64 x64sc x128 x64dtv xscpu64 xvic xpet xplus4 xcbm2 xcbm5x0 c1541 vsid"
EXECUTABLES=""

for executable in $POSSIBLE_EXECUTABLES
do
    if ! grep -q "^$executable:" Makefile
    then
        echo "Executable $executable not present"
        continue
    fi

    EXECUTABLES="$EXECUTABLES $executable"
done

#
# The src folder Makefile also defines all the non-tool executables and what they link to.
#

pushdq src

#
# Find all the libraries first
#

echo >> CMakeLists.txt

for executable in $EXECUTABLES
do	
	LIB_ARGS="$(extract_make_var LIBS) $(extract_make_var ${executable}_LDADD)"

	for lib in $(extract_external_libs $LIB_ARGS)
	do
		label=$(external_lib_label $lib)

		if ! fgrep -q "find_library($label " CMakeLists.txt
		then
			cat <<-HEREDOC >> CMakeLists.txt
				find_library($label $lib \${CMAKE_C_IMPLICIT_LINK_DIRECTORIES} $(extract_link_dirs))
			HEREDOC
		fi
	done
done

#
# Executable build targets
#

for executable in $EXECUTABLES
do
	echo "Executable: $executable"

	add_executable_target $executable
done

popdq

#
# Tools, executable targets in src/tools/x with simpler linking
#

TOOLS="petcat cartconv"

for tool in $TOOLS
do
	echo "Tool: $tool"
	
	pushdq "src/tools/$tool"

	add_executable_target $tool

	popdq
done

#
# Test program for uiclient
#

pushdq src/arch/headless/uiclient
add_executable_target uiclienttest
popdq

#
# Finally, create the top level project CMakeLists.txt
#

echo "Creating top level CMakeLists.txt"

cat <<-HEREDOC > CMakeLists.txt
	cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

HEREDOC

if [[ "$OSTYPE" == "darwin"* ]]; then
	cat <<-HEREDOC >> CMakeLists.txt
		set(CMAKE_OSX_SYSROOT "$(xcode-select -p)/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/")
		set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS cc;cpp)
		set(CMAKE_CXX_STANDARD 11)

		project(VICE C CXX OBJC)
		set(CMAKE_STATIC_LIBRARY_PREFIX "")

		add_subdirectory(src)
	HEREDOC
else
	cat <<-HEREDOC >> CMakeLists.txt
		set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS cc;cpp)
		set(CMAKE_CXX_STANDARD 11)

		project(VICE C CXX)
		set(CMAKE_STATIC_LIBRARY_PREFIX "")

		add_subdirectory(src)
	HEREDOC
fi
