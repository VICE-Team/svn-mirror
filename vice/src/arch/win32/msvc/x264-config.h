#define HAVE_MMX 0
#define ARCH_X86 1
#define SYS_WINDOWS 1
#define STACK_ALIGNMENT 32
#define HAVE_WIN32THREAD 1
#define HAVE_THREAD 1
#define HAVE_LOG2F 0
#define HAVE_AVS 1
#define USE_AVXSYNTH 0
#define HAVE_VECTOREXT 1
#define HAVE_GPL 1
#define HAVE_INTERLACED 1
#define HAVE_OPENCL 1
#define HAVE_MALLOC_H 0
#define HAVE_ALTIVEC 0
#define HAVE_ALTIVEC_H 0
#define HAVE_ARMV6 0
#define HAVE_ARMV6T2 0
#define HAVE_NEON 0
#define HAVE_BEOSTHREAD 0
#define HAVE_POSIXTHREAD 0
#define HAVE_SWSCALE 0
#define HAVE_LAVF 0
#define HAVE_FFMS 0
#define HAVE_GPAC 0
#define HAVE_CPU_COUNT 0
#define HAVE_THP 0
#define HAVE_LSMASH 0
#define HAVE_AS_FUNC 0

#if !defined(IDE_COMPILE) || (defined(IDE_COMPILE) && (_MSC_VER >= 1310))
#define LLN(x) (x ## LL)
#define ULLN(x) (x ## ULL)
#else
#define LLN(x) ((__int64)(x))
#define ULLN(x) ((unsigned __int64)(x))
#endif
