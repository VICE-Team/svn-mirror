#! /usr/bin/perl -w
#   ---------------------------------------------------------------------------
#   This file is part of reSID, a MOS6581 SID emulator engine.
#   Copyright (C) 1999  Dag Lem <resid@nimrod.no>
# 
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
# 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#   ---------------------------------------------------------------------------

use strict;

die("Usage: samp2src name data-in src-out\n") unless $#ARGV == 2;
my ($name, $in, $out) = @ARGV[0..2];

open(F, "<$in") or die($!);
local $/ = undef;
my $data = <F>;
close(F) or die($!);

my @sample = map(sprintf("0x%02x", ord($_)), split(//, $data));

$data = '';
my $i;
for ($i = 0; $i <= $#sample; $i += 8) {
  $data .= sprintf("/* 0x%03x: */  ", $i) . join(', ', @sample[$i..$i+7]) . ",\n";
}

open(F, ">$out") or die($!);
print F <<\EOF;
//  ---------------------------------------------------------------------------
//  This file is part of reSID, a MOS6581 SID emulator engine.
//  Copyright (C) 1999  Dag Lem <resid@nimrod.no>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  ---------------------------------------------------------------------------

EOF
print F "#include \"wave.h\"\n\nreg8 WaveformGenerator::$name\[\] =\n{\n";
print F $data;
print F "};\n";
close(F) or die($!);

exit(0);
