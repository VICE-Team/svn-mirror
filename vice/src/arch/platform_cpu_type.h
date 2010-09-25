/* Platform CPU discovery
 *
 * CPU        | support
 * -------------------------------------------------------
 * alpha      | yes, including sub-type
 * amd64      | yes
 * arc        | no
 * arm        | yes, including endian, but no sub-type yet
 * avr32      | yes, no sub-type
 * blackfin   | no
 * cris       | no
 * crx        | no
 * fr30       | no
 * frv        | no
 * h8300      | no
 * hppa       | yes, no sub-type yet
 * ia64       | yes, no sub-type
 * lm32       | no
 * m32c       | no
 * m32r       | no
 * m68k       | yes, including incomplete sub-type
 * m68hc1x    | no
 * mcore      | no
 * mep        | no
 * microblaze | no
 * mips       | yes, including endian, no sub-type yet
 * mips64     | yes, including endian, no sub-type yet
 * mmix       | no
 * mn10300    | no
 * ns32k      | yes
 * pdp-11     | no
 * picochip   | no
 * powerpc    | yes, no sub-type yet
 * powerpc64  | yes, no sub-type yet
 * rx         | no
 * s390       | yes
 * s390x      | yes
 * score      | no
 * sh         | yes, but incomplete sub-type and endian (*)
 * x86        | yes, including sub-type
 */

/* Generic alpha cpu discovery */
#if !defined(FIND_ALPHA_CPU) && (defined(__alpha__) || defined(__alpha_ev6__) || defined(__alpha_ev5__) || defined(__alpha_ev4__))
#define FIND_ALPHA_CPU
#endif

#ifdef FIND_ALPHA_CPU

#ifdef __alpha_ev6__
#define PLATFORM_CPU "Alpha EV6"
#endif

#if !defined(PLATFORM_CPU) && defined(__alpha_ev5__)
#define PLATFORM_CPU "Alpha EV5"
#endif

#if !defined(PLATFORM_CPU) && defined(__alpha_ev4__)
#define PLATFORM_CPU "Alpha EV4"
#endif

#ifndef PLATFORM_CPU
#define PLATFORM_CPU "Alpha"
#endif

#endif


/* Generic amd64/x86_64 cpu discovery */
#if !defined(PLATFORM_CPU) && (defined(__amd64__) || defined(__x86_64__))
#define PLATFORM_CPU "AMD64/x86_64"
#endif


/* Generic arm cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__arm__)
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "ARM (big endian)"
#  else
#    define PLATFORM_CPU "ARM (little endian)"
#  endif
#endif


/* Generic avr32 cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__avr32__)
#define PLATFORM_CPU "AVR32"
#endif


/* Generic hppa cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__hppa__)
#defined PLATFORM_CPU "HPPA"
#endif

/* Generic ia64 cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__ia64__)
#define PLATFORM_CPU "IA64"
#endif


/* Generic m68k cpu discovery */
#if !defined(PLATFORM_CPU) && !defined(FIND_M68K_CPU) && defined(__m68k__)
#define FIND_M68K_CPU
#endif

#if !defined(PLATFORM_CPU) && defined(FIND_M68K_CPU)

#ifdef __mc68060__
#define PLATFORM_CPU "68060"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68040__)
#define PLATFORM_CPU "68040"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68030__)
#define PLATFORM_CPU "68030"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68020__)
#define PLATFORM_CPU "68020"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68010__)
#define PLATFORM_CPU "68010"
#endif

#if !defined(PLATFORM_CPU) && defined(__mc68000__)
#define PLATFORM_CPU "68000"
#endif

#ifndef PLATFORM_CPU
#define PLATFORM_CPU "M68K"
#endif

#endif


/* Generic mips cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__mips__) && !defined(__mips64__)
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "MIPS (big endian)"
#  else
#    define PLATFORM_CPU "MIPS (little endian)"
#  endif
#endif


/* Generic mips64 cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__mips64__)
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "MIPS64 (big endian)"
#  else
#    define PLATFORM_CPU "MIPS64 (little endian)"
#  endif
#endif


/* Generic ns32k cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__ns32000__)
#define PLATFORM_CPU_"NS32K"
#endif


/* Generic powerpc cpu discovery */
#if !defined(PLATFORM_CPU) && (defined(__powerpc__) || defined(__ppc__)) && !defined(__powerpc64__)
#define PLATFORM_CPU "PPC"
#endif


/* Generic powerpc64 cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__powerpc64__)
#define PLATFORM_CPU "PPC64"
#endif


/* Generic s390 cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__s390__) && !defined(__s390x__)
#define PLATFORM_CPU "S390"
#endif

#if !defined(PLATFORM_CPU) && defined(__s390x__)
#define PLATFORM_CPU "S390x"
#endif


/* Generic x86 cpu discovery */
#if !defined(PLATFORM_CPU) && !defined(FIND_X86_CPU) && (defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && !defined(__amd64__) && !defined(__x86_64__)
#define FIND_X86_CPU
#endif

#if !defined(PLATFORM_CPU) && defined(FIND_X86_CPU)

#ifdef __i686__
#define PLATFORM_CPU "Pentium Pro"
#endif

#if !defined(PLATFORM_CPU) && defined(__i586__)
#define PLATFORM_CPU "Pentium"
#endif

#if !defined(PLATFORM_CPU) && defined(__i486__)
#define PLATFORM_CPU "80486"
#endif

#if !defined(PLATFORM_CPU) && defined(__i386__)
#define PLATFORM_CPU "80386"
#endif

#ifndef PLATFORM_CPU
#define PLATFORM_CPU "Unknown intel x86 compatible"
#endif

#endif


#ifndef PLATFORM_CPU

#ifdef __sparc64__
#define PLATFORM_CPU "SPARC64"
#endif

#if defined(__sparc__) && !defined(__sparc64__)
#define PLATFORM_CPU "SPARC"
#endif

#ifdef __sh3__
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "SH3 (big endian)"
#  else
#    define PLATFORM_CPU "SH3 (little endian)"
#  endif
#endif

#ifdef __SH4__
#define PLATFORM_CPU "SH4"
#endif

#ifdef __vax__
#define PLATFORM_CPU "VAX"
#endif

#endif
