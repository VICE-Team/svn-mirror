#!/usr/bin/perl -w
#
# This program helps in the creation of Dela EP-64 CRT images.
#
# This file is part of VICE, the Versatile Commodore Emulator.
# See README for copyright notice.
#
# Written by
#  Michael Klein <michael.klein@puffin.lb.shuttle.de>
#
# Quick instructions:
# ~~~~~~~~~~~~~~~~~~~
#
# - create an empty Dela EP-64 .crt image, which contains only the 8K
#   control/menu software ROM (which is *not* part of VICE):
#     $ ./delaep64.pl -0 delaep64-8k.bin -o delaep64.crt
#
# - attach this .crt in x64 and create one or two modules using the
#   module maker software in the menu ROM
#
# - copy module(s) to host filesystem (c1541) and create new .crt image:
#     $ ./delaep64.pl -0 delaep64-8k.bin -1 module1.bin -2 module2.bin -o delaep64.crt
#
# The size of 32K image file must be exactly 32768, 32770 or 32772 (the module
# maker adds two extra bytes at the beginning), the size of the 8K image file
# must be either 8192 or 8194.
#

use Getopt::Std;

sub usage
{
    print STDERR << "EOF";
This is the Dela EP-64 CRT creation utility.

Usage: $0 [options]
  -h      : this help
  -o file : name of the output file
  -0 file : load data for chip 0 (8K, control software)
  -1 file : load data for chip 1 (32K)
  -2 file : load data for chip 2 (32K)
EOF
    exit(shift);
}

my %opts;

getopts("h0:1:2:o:", \%opts) or usage(1);

usage(0) if defined($opts{h});

die "No control software loaded" unless defined($opts{0});
die "No output file specified" unless defined($opts{o});

my @chipsize = (0x2000, 0x8000, 0x8000);
my %romdata;

foreach my $chip (0..2)
{
    my $fn = $opts{$chip};
    next unless defined $fn;

    my $cs = $chipsize[$chip];
    my $fs = -s "$fn" or die "$fn: can't get file size: $!";

    (($fs == $cs) || ($fs == $cs+2) ||
     ($cs == 0x8000 && $fs == $cs+4)) or die "$fn: invalid file size: $fs";

    open(ROM, $fn) or die "$fn: can't open: $!";
    binmode(ROM);

    seek(ROM, $fs-$cs, 0);
    (read(ROM, $romdata{$chip}, $cs) == $cs)
        or die "$fn: error reading ROM data: $!";
}

open(CRT, ">$opts{o}") or die "can't open output file: $!";
binmode(CRT);

my $crtheader = pack("A16NnnCCa6a32",
                     "C64 CARTRIDGE",   # signature
                     0x40,              # file header size
                     0x100,             # version
                     0x18,              # hardware type
                     0x01,              # EXROM
                     0x00,              # GAME
                     "",                # Reserved
                     "DELA EP64",       # cart name
                 );

print CRT $crtheader;

foreach my $chip (sort keys(%romdata))
{
    my $chipheader = pack("A4Nnnnn",
                          "CHIP",                       # signature
                          16+length($romdata{$chip}),   # packet size
                          0x00,                         # chip type
                          $chip,                        # bank
                          0x00,                         # chip address
                          length($romdata{$chip})       # chip size
                      );

    print CRT $chipheader.$romdata{$chip};
}
