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

# NOTICE: This file is currently incomplete. Right now it takes an
#         already built bindist and recursively scans the VICE
#         executables and its dependencies to determine what libraries
#         need to be copied into the bundle. To be able to replace the
#         current bindsit script, it needs to:
#
#         - Actually execute those copies into the correct parts of
#           the bundle
#         - Correctly run install_name_tool to patch up all loading
#           paths
#
#         This is enough work to do that I'm going to be committing it
#         in parts.

use strict;
use warnings;

my $bindir = 'VICE.app/Contents/Resources/bin';

die "Run this script in the same directory as VICE.app!" unless -d $bindir;
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
print "Frameworks\n----------\n";
print join("\n", keys %frameworks);
print "\n\nLibraries\n---------\n";
print join("\n", keys %libraries);
print "\n";
