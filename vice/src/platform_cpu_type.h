#if !defined(PLATFORM_CPU) && !defined(FIND_X86_CPU) && !defined(FIND_M68K_CPU) && !defined(FIND_ALPHA_CPU)

#ifdef __ia64__
#define PLATFORM_CPU "IA64"
#endif

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

#ifdef __alpha__
#define FIND_ALPHA_CPU
#endif

#ifdef __sparc64__
#define PLATFORM_CPU "SPARC64"
#endif

#if defined(__sparc__) && !defined(__sparc64__)
#define PLATFORM_CPU "SPARC"
#endif

#ifdef __arm__
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "ARM (big endian)"
#  else
#    define PLATFORM_CPU "ARM (little endian)"
#  endif
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

#endif
