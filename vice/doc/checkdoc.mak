#! /usr/bin/make -f

.SILENT:

PROGS= \
	../src/petcat \
	../src/cartconv \
	../src/c1541 \
	../src/vsid \
	../src/x64 \
	../src/x64sc \
	../src/x64dtv \
	../src/x128 \
	../src/xcbm2 \
	../src/xvic \
	../src/xplus4 \
	../src/xpet \
	../src/xscpu64

all: help

help:
	echo -ne "check vice.texi\n\n"
	echo -ne "usage: ./checkdoc.mak [full | opt | res | clean | update]\n\n"
	echo -ne "options:\n"
	echo -ne "full\tdo all checks\n"
	echo -ne "opt\tcheck command-line options\n"
	echo -ne "res\tcheck resources\n"
	echo -ne "listopt\tlist all command-line options\n"
	echo -ne "listres\tlist all resources\n"
	echo -ne "fixme\tshow FIXMEs\n"
	echo -ne "nodes\tshow nodes marked FIXME\n"
	echo -ne "clean\tremove temp files\n"
	echo -ne "update\tgenerate documentation\n"

vice.opts.tmp: $(PROGS) checkdoc.mak checkdoc.mon
	echo "creating vice.opts.tmp"
	echo "[C64]" > vice.opts.tmp
	LANGUAGE="en" x64 -help >> vice.opts.tmp
	echo "[C64SC]" >> vice.opts.tmp
	LANGUAGE="en" x64sc -help >> vice.opts.tmp
	echo "[C64DTV]" >> vice.opts.tmp
	LANGUAGE="en" x64dtv -help >> vice.opts.tmp
	echo "[C128]" >> vice.opts.tmp
	LANGUAGE="en" x128 -help >> vice.opts.tmp
	echo "[VSID]" >> vice.opts.tmp
	LANGUAGE="en" vsid -help >> vice.opts.tmp
	echo "[CBM-II]" >> vice.opts.tmp
	LANGUAGE="en" xcbm2 -help >> vice.opts.tmp
	echo "[CBM-II-5x0]" >> vice.opts.tmp
	LANGUAGE="en" xcbm2 -help >> vice.opts.tmp
	echo "[VIC20]" >> vice.opts.tmp
	LANGUAGE="en" xvic -help >> vice.opts.tmp
	echo "[PLUS4]" >> vice.opts.tmp
	LANGUAGE="en" xplus4 -help >> vice.opts.tmp
	echo "[PET]" >> vice.opts.tmp
	LANGUAGE="en" xpet -help >> vice.opts.tmp
	echo "[SCPU64]" >> vice.opts.tmp
	LANGUAGE="en" xscpu64 -help >> vice.opts.tmp
	echo "[petcat]" >> vice.opts.tmp
	-LANGUAGE="en" petcat -help | tr '[]' '()' >> vice.opts.tmp
	echo "[cartconv]" >> vice.opts.tmp
	-LANGUAGE="en" cartconv -help >> vice.opts.tmp

vice.rc.tmp: $(PROGS) checkdoc.mak checkdoc.mon
	echo "creating vice.rc.tmp"
	rm -f vice.rc.tmp
	x64 -config vice.rc.tmp -initbreak 0xfce2 -moncommands checkdoc.mon
	x64sc -config vice.rc.tmp -initbreak 0xfce2 -moncommands checkdoc.mon
	x64dtv -config vice.rc.tmp -initbreak 0xfce2 -moncommands checkdoc.mon
	x128 -config vice.rc.tmp -initbreak 0xff3d -moncommands checkdoc.mon
	vsid -config vice.rc.tmp -initbreak 0xfce2 -moncommands checkdoc.mon
	xcbm5x0 -config vice.rc.tmp -initbreak 0xf99e -moncommands checkdoc.mon
	xcbm2 -config vice.rc.tmp -initbreak 0xf997 -moncommands checkdoc.mon
	xvic -config vice.rc.tmp -initbreak 0xfd22 -moncommands checkdoc.mon
	xplus4 -config vice.rc.tmp -initbreak 0xfff6 -moncommands checkdoc.mon
	xpet -config vice.rc.tmp -initbreak 0xfd16 -moncommands checkdoc.mon
	xscpu64 -config vice.rc.tmp -initbreak 0xfce2 -moncommands checkdoc.mon
	
checkdoc: checkdoc.c
	echo "creating checkdoc"
	gcc -Wall -o checkdoc checkdoc.c

full: checkdoc vice.opts.tmp vice.rc.tmp fixme nodes
	./checkdoc -all vice.texi vice.rc.tmp vice.opts.tmp

res: checkdoc vice.opts.tmp vice.rc.tmp
	./checkdoc -res vice.texi vice.rc.tmp vice.opts.tmp

listres: checkdoc vice.opts.tmp vice.rc.tmp
	./checkdoc -listres vice.texi vice.rc.tmp vice.opts.tmp

opt: checkdoc vice.opts.tmp vice.rc.tmp
	./checkdoc -opt vice.texi vice.rc.tmp vice.opts.tmp

listopt: checkdoc vice.opts.tmp vice.rc.tmp
	./checkdoc -listopt vice.texi vice.rc.tmp vice.opts.tmp
	
update: vice.texi
	make --silent

.PHONY: fixme

fixme:
	echo -ne "list of FIXMEs ("
	echo -ne `grep -an "@c " vice.texi | grep -v "\-\-\-" | grep -v "@node" | grep -i "fixme" | wc -l`
	echo -ne "):\n"
	grep -an "@c " vice.texi | grep -v "\-\-\-" | grep -v "@node" | grep -i "fixme"
	echo -ne "\n"

todo: fixme
all: full

nodes:
	echo -ne "nodes that need fixing ("
	echo -ne `grep -an "@c " vice.texi | grep -v "\-\-\-" | grep -i "fixme" | grep "@node" | wc -l`
	echo -ne "):\n"
	grep -an "@c " vice.texi | grep -v "\-\-\-" | grep -i "fixme" | grep "@node"
	echo -ne "\n"

clean:
	rm -f ./checkdoc
	rm -f vice.opts.tmp
	rm -f vice.rc.tmp
