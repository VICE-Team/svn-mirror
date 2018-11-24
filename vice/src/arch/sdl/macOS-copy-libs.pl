#!/usr/bin/perl
#
# macOS-copy-libs.pl - Bindist creation tool for macOS SDL VICE
# Copyright (c) 2018 Michael C. Martin <mcmartin@gmail.com>
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

# NOTICE: This file is currently incomplete. Right now it only prints
#         what it intends to do instead of actually doing it. Once I'm
#         confident that the command text has been debugged I will
#         swap it out to do the work for real and then remove this
#         notice.

use strict;
use warnings;

# Check our preconditions: that we're running just outside of the VICE
# bundle and that the FW_DIR environment variable has been set, which
# identifies where supplemental frameworks live.

my $bindir = 'VICE.app/Contents/Resources/bin';
die "Run this script in the same directory as VICE.app!" unless -d $bindir;
die "FW_DIR environment variable not specified!" unless exists($ENV{"FW_DIR"});

my $fwdir = $ENV{"FW_DIR"};
my @binaries = map("$bindir/$_", split(' ', `ls $bindir`));

# Collect every framework and library linked by any binary. We use hashes as a
# cheap way to dedup each of these.
my %frameworks = ();
my %libraries = ();
foreach my $binary (@binaries) {
  open (LINKEDLIBS, "otool -L \"$binary\" |");
  # Skip the per-file header line.
  <LINKEDLIBS>;
  # If this isn't an executable, the first line we process is the name
  # of the library, but processing that will be a no-op.
  while (<LINKEDLIBS>) {
    $_ = (split)[0];
    next if /^\/usr\//;
    next if /^\/System\//;
    if (/\.framework\//) {
      # TODO: Confirm that this framework has a copy in FW_DIR? The
      #       make would have failed at an earlier phase if it weren't
      #       there, but the user might have messed with their file
      #       system between making 'all' and 'bindist', I suppose
      $frameworks{$_} = 1.0;
    } else {
      if (not exists $libraries{$_}) {
        $libraries{$_} = 1.0;      
        push @binaries, $_
      }
    }
  }
  close LINKEDLIBS;
}
# Copy libraries and frameworks into place, using the Apple-specific
# "ditto" command to make sure that all attributes etc are preserved
foreach (keys %libraries) {
  print "ditto \"$_\" \"$bindir\"\n";
}
print "mkdir \"VICE.app/Contents/Frameworks\"\n";
foreach (keys %frameworks) {
  /.*\/(.*\.framework).*/;
  my $destdir = "VICE.app/Contents/Frameworks/$1";
  print "mkdir \"$destdir\"\n";
  print "ditto \"$fwdir/$1\" \"$destdir\"\n";
}
# Create new id tokens for the dylibs and assemble the mapping to
# change them to the new relative locations.
my %links;
foreach (keys %libraries) {
  /.*\/(.*)/;
  $links{$_} = "\@executable_path/$1";
  print "install_name_tool -id \@executable_path/$1 \"$bindir/$1\"\n";
}
# Create new id tokens for the frameworks. Note that there's an extra
# ".." in the path for this than is usual for macOS applications;
# that's because x64/x128/xplus4/etc all live in
# Contents/Resources/bin instead of Contents/MacOS, so it's got to
# step out one more to get to Contents/Frameworks. And yes, this means
# that the actual VICE launcher binary doesn't get to link any of
# these frameworks itself. If we need that in the future we'll need to
# migrate all the binaries and libraries into Contents/MacOS.
foreach (keys %frameworks) {
  /.*\/(.*\.framework.*)/;
  $links{$_} = "\@executable_path/../../Frameworks/$1";
  print "install_name_tool -id \@executable_path/../../Frameworks/$1 VICE.app/Contents/Frameworks/$1\n";
}
# Update the linkage information for all libraries and frameworks in
# all binaries. This includes the libraries in %libraries but not in
# %frameworks, as frameworks are assumed to be self-contained.
foreach my $binary (@binaries) {
  foreach (keys %links) {
    print "install_name_tool -old \"$_\" -new \"$links{$_}\" \"$binary\"\n";
  };
}
