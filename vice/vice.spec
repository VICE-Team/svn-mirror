%define version 1.11
%define rel     1
%define prefix  /usr/local

Summary: VICE, the Versatile Commodore Emulator
Name: vice
Version: %version
Release: %rel
Copyright: GPL
Group: X11/Applications/Emulators
Source: ftp://ftp.funet.fi/pub/cbm/crossplatform/emulators/VICE/vice-%{version}.tar.gz
URL: http://viceteam.bei.t-online.de/
Packager: Andreas Boose <viceteam@t-online.de>
Prefix: %{prefix}
BuildRoot: /var/tmp/vice-build-root

%description
VICE is a set of accurate emulators for the Commodore 64, 128, VIC20,
PET and CBM-II 8-bit computers, all of which run under the X Window
System.

%prep
%setup -q

%build
CFLAGS="$RPM_OPT_FLAGS -DNO_REGPARM" ./configure --prefix=%{prefix}
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}
make prefix=$RPM_BUILD_ROOT%{prefix} install-strip
gzip -9 $RPM_BUILD_ROOT%{prefix}/man/man1/*
gzip -9 $RPM_BUILD_ROOT%{prefix}/info/*

%clean
rm -rf $RPM_BUILD_ROOT

%files

%doc AUTHORS FEEDBACK INSTALL README

%{prefix}/bin/x64
%{prefix}/bin/x128
%{prefix}/bin/xvic
%{prefix}/bin/xpet
%{prefix}/bin/xcbm2
%{prefix}/bin/c1541
%{prefix}/bin/petcat
%{prefix}/bin/vsid
%{prefix}/lib/vice/
%{prefix}/man/man1/c1541.1.gz
%{prefix}/man/man1/petcat.1.gz
%{prefix}/man/man1/vice.1.gz
%{prefix}/info/vice.info.gz
%{prefix}/info/vice.info-1.gz
%{prefix}/info/vice.info-2.gz
%{prefix}/info/vice.info-3.gz
%{prefix}/info/vice.info-4.gz
%{prefix}/info/vice.info-5.gz
%{prefix}/share/locale/*/LC_MESSAGES/vice.mo
