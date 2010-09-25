/* Platform CPU discovery
 *
 * CPU      | support
 * -------------------------------------------------
 * alpha    | yes, including sub-type
 * arm      | yes, including endian, but no sub-type
 * avr32    | yes, no sub-type
 * blackfin | no
 * cris     | no
 * frv      | no
 * h8300    | no
 * ia64     | yes, no sub-type
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


/* Generic ia64 cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__ia64__)
#define PLATFORM_CPU "IA64"
#endif


#if !defined(PLATFORM_CPU) && !defined(FIND_X86_CPU) && !defined(FIND_M68K_CPU)

#if defined(__amd64__) || defined(__x86_64__)
#define PLATFORM_CPU "AMD64/x86_64"
#endif

#ifdef __powerpc64__
#define PLATFORM_CPU "PPC64"
#endif

#if (defined(__powerpc__) || defined(__ppc__)) && !defined(__powerpc64__)
#define PLATFORM_CPU "PPC"
#endif

#if (defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && !defined(__amd64__) && !defined(__x86_64__)
#define FIND_X86_CPU
#endif

#ifdef __sparc64__
#define PLATFORM_CPU "SPARC64"
#endif

#if defined(__sparc__) && !defined(__sparc64__)
#define PLATFORM_CPU "SPARC"
#endif

#ifdef __hppa__
#define PLATFORM_CPU "HPPA"
#endif

#if defined(__mips__) && !defined(__mips64__)
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "MIPS (big endian)"
#  else
#    define PLATFORM_CPU "MIPS (little endian)"
#  endif
#endif

#ifdef __mips64__
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "MIPS64 (big endian)"
#  else
#    define PLATFORM_CPU "MIPS64 (little endian)"
#  endif
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

#ifdef __m68k__
#define FIND_M68K_CPU
#endif

#if defined(__s390__) && !defined(__s390x__)
#define PLATFORM_CPU "S390"
#endif

#ifdef __s390x__
#define PLATFORM_CPU "S390x"
#endif

#ifdef __vax__
#define PLATFORM_CPU "VAX"
#endif

#ifdef __ns32000__
#define PLATFORM_CPU "NS32K"
#endif

#endif
