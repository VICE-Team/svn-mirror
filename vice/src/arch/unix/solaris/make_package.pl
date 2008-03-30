#!/usr/local/bin/perl -w
#
# Automated processes to create SUN packages
# You can run this script after you did a 'make install' in the chrooted
# environment. Run it from the <whatever>/packagename-1.0/usr/local/ directory
#
# JA: 06-01-2000        Initial release
# JA: 25-01-2000        Beautified a little
# AH: 20-07-2000        Beautified a bit more

$find = "/usr/bin/find";
$pkgproto = "/usr/bin/pkgproto";
$pkgmk = "/usr/bin/pkgmk";
$pkgtrans = "/usr/bin/pkgtrans";
$temp = "/tmp/prototype$$";
$prototype = "prototype";
$pkginfo = "pkginfo";
($gid ,$pkg ,$uid ,$userInfo ,$email ,$quota ,$group ,$passwd
,$category ,$userHome ,$vendor ,$loginShell ,$pstamp ,$basedir)=();

# Sanitycheck

$pwd = `pwd`;
if ($pwd =~ '\/usr\/local') {
        $pwd = $`;
}
die  "Wrong location, please cd to <PKGBASE>/usr/local/ and run again.\n"
    	if ($pwd eq "");

system  ("$find . -print | $pkgproto > $temp");
open (PREPROTO,"<$temp") || die "Unable to read prototype information ($!)\n";
open (PROTO,">$prototype") || die "Unable to write file prototype ($!)\n";
print PROTO "i pkginfo=./$pkginfo\n";
while (<PREPROTO>) {
        # Read the prototype information from /tmp/prototype$$
        chomp;
        $thisline = $_;
        if ($thisline =~ " prototype "
         or $thisline =~ " pkginfo ") {
          # We don't need that line
        } elsif ($thisline =~ "^[fd] ") {
          # Change the ownership for files and directories
          ($dir, $none, $file, $mode, $user, $group) = split / /,$thisline;
          print PROTO "$dir $none $file $mode bin bin\n";
        } else {
          # Symlinks and other stuff should be printed as well ofcourse
          print PROTO "$thisline\n";
        }
}
close PROTO;
close PREPROTO;

# Clean up
unlink $temp || warn "Unable to remove tempfile ($!)\n";

# Now we can start building the package
#
# First get some info

$thispackage = `basename $pwd`;
if ($thispackage =~ '-') {
        $default{"name"} = $`;
        $default{"version"} = $';
        chomp $default{"version"};
} else {
        $default{"name"} = $thispackage;
        chomp $default{"name"};
        $default{"version"} = "1.0";
}
$default{"pkg"} = "UMC" . substr($default{"name"},0,4);
$default{"arch"} = `uname -m`;
chomp $default{"arch"};
$default{"category"} = "application";
$default{"vendor"} = "The VICE Team";
$default{"email"} = "vice-devel@\firenze.linux.it";
$login = getlogin();
($user, $passwd, $uid, $gid, $quota, $default{"pstamp"}, $userInfo,
 $userHome, $loginShell) = getpwnam ($login);
$default{"pstamp"} = "Marco van den Heuvel"
    	if ($default{"pstamp"} eq "");
$os = `uname -r`;
$os =~ '\.';
$os = "sol$'";
chomp $os;
$default{"basedir"} = "/usr/local";

# Check for correctness of guessed values by userinput

%questions = (
  pkg => "Please give the name for this package",
  name => "Now enter the real name for this package",
  arch => "What architecture did you build the package on?",
  version => "Enter the version number of the package",
  category => "What category does this package belong to?",
  vendor => "Who is the vendor of this package?",
  email => "Enter the email adress for contact",
  pstamp => "Enter your own name",
  basedir => "What is the basedir this package will install into?",
  packagename => "How should i call the packagefile?",
);

@vars = qw(pkg name arch version category vendor email pstamp basedir
    	   packagename);
foreach $varname (@vars) {
        $default{"$varname"} = "$name-$version-$os-$arch-local"
	    	if ($varname eq "packagename");
        getvar($varname);
}
$classes = "none";

# Create the pkginfo file

print "\nNow creating $pkginfo file\n";
open (PKGINFO,">$pkginfo") || die "Unable to open $pkginfo for writing ($!)\n";
print PKGINFO "PKG=\"$pkg\"\n";
print PKGINFO "NAME=\"$name\"\n";
print PKGINFO "ARCH=\"$arch\"\n";
print PKGINFO "VERSION=\"$version\"\n";
print PKGINFO "CATEGORY=\"$category\"\n";
print PKGINFO "VENDOR=\"$vendor\"\n";
print PKGINFO "EMAIL=\"$email\"\n";
print PKGINFO "PSTAMP=\"$pstamp\"\n";
print PKGINFO "BASEDIR=\"$basedir\"\n";
print PKGINFO "CLASSES=\"$classes\"\n";
close PKGINFO;
print "Done.\n";

# Build and zip the package

print "Building package\n";
system ("$pkgmk -r `pwd`");
system ("(cd /var/spool/pkg;$pkgtrans -s `pwd` /tmp/$packagename)");
system ("gzip /tmp/$packagename");
print "Done. (/tmp/$packagename.gz)\n";

# The subroutines

sub getvar {
        my $questionname = "@_";
        print "$questions{$questionname} [$default{\"$questionname\"}]: ";
        my $answer = <STDIN>;
        chomp $answer;
        $$questionname = $answer;
        $$questionname = $default{$questionname} if ($$questionname eq "");
}
