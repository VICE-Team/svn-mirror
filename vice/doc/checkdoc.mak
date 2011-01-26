#! /usr/bin/make -f

.SILENT:

PROGS= \
	../src/petcat \
	../src/cartconv \
	../src/c1541 \
	../src/x64 \
	../src/x64sc \
	../src/x64dtv \
	../src/x128 \
	../src/xcbm2 \
	../src/xvic \
	../src/xplus4 \
	../src/xpet

all: help

help:
	echo -ne "check vice.texi\n\n"
	echo -ne "usage: ./checkdoc.mak [full | opt | res | clean]\n\n"
	echo -ne "options:\n"
	echo -ne "full\tdo all checks\n"
	echo -ne "opt\tcheck command-line options\n"
	echo -ne "res\tcheck resources\n"
	echo -ne "clean\tremove temp files\n"

vice.opts.tmp: $(PROGS)
	echo "creating vice.opts.tmp"
	echo "[C64]" > vice.opts.tmp
	../src/x64 -help >> vice.opts.tmp
	echo "[C64SC]" >> vice.opts.tmp
	../src/x64sc -help >> vice.opts.tmp
	echo "[C64DTV]" >> vice.opts.tmp
	../src/x64dtv -help >> vice.opts.tmp
	echo "[C128]" >> vice.opts.tmp
	../src/x128 -help >> vice.opts.tmp
	echo "[VSID]" >> vice.opts.tmp
	../src/x64 -vsid -help >> vice.opts.tmp
	echo "[CBM-II]" >> vice.opts.tmp
	../src/xcbm2 -help >> vice.opts.tmp
	echo "[CBM-II-5x0]" >> vice.opts.tmp
	../src/xcbm2 -help >> vice.opts.tmp
	echo "[VIC20]" >> vice.opts.tmp
	../src/xvic -help >> vice.opts.tmp
	echo "[PLUS4]" >> vice.opts.tmp
	../src/xplus4 -help >> vice.opts.tmp
	echo "[PET]" >> vice.opts.tmp
	../src/xpet -help >> vice.opts.tmp
	echo "[petcat]" >> vice.opts.tmp
	-../src/petcat -help | tr '[]' '()' >> vice.opts.tmp
	echo "[cartconv]" >> vice.opts.tmp
	-../src/cartconv -help >> vice.opts.tmp

checkdoc: checkdoc.c
	echo "creating checkdoc"
	gcc -o checkdoc checkdoc.c

full: checkdoc vice.opts.tmp
	./checkdoc -all vice.texi ~/.vice/vicerc vice.opts.tmp

res: checkdoc vice.opts.tmp
	./checkdoc -res vice.texi ~/.vice/vicerc vice.opts.tmp

opt: checkdoc vice.opts.tmp
	./checkdoc -opt vice.texi ~/.vice/vicerc vice.opts.tmp

clean:
	rm -f ./checkdoc
	rm -f vice.opts.tmp
