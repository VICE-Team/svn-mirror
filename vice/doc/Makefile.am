@ALTERNATE_DOCDIR_TRUE@realdocdir = $(prefix)/share/doc/vice
@ALTERNATE_DOCDIR_FALSE@realdocdir = $(VICEDIR)/doc

docdir = $(realdocdir)

doc_DATA = \
	Minix-Howto.txt \
	MSDOS-Ethernet-Howto.txt \
	MSDOS-Howto.txt \
	NLS-Howto.txt \
	Readme.beos \
	Readme.dos \
	ReadmeMacOSX.txt \
	Win32-Cygwin-HOWTO.txt \
	Win32-Howto.txt \
	OpenVMS-Howto.txt \
	cartconv.txt \
	iec-bus.txt \
	mon.txt \
	openGLsync-howto.txt \
	GP2X-Howto.txt \
	coding-guidelines.txt

info_TEXINFOS = vice.texi

EXTRA_DIST = $(doc_DATA)

SUBDIRS = html

AM_MAKEINFOHTMLFLAGS = 
AM_MAKEINFOFLAGS = --no-split
