#! /usr/bin/perl -w

die("Usage: getdate filename\n") unless @ARGV == 1;
my ($name) = @ARGV;

($mtime) = (stat ($name) )[9];
print $mtime;
