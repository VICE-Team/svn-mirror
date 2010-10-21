/* Platform CPU discovery
 *
 * CPU        | compiletime-support | runtime-support
 * -------------------------------------------------------
 * alpha      | yes, +sub           | not yet
 * amd64      | yes                 | not yet
 * arc        | yes, +endian        | not yet
 * arm        | yes, +endian +sub   | not yet
 * avr32      | yes, -sub           | not yet
 * blackfin   | yes, +sub           | not yet
 * cris       | yes, ~sub           | not yet
 * crx        | no                  | not yet
 * fr30       | no                  | not yet
 * frv        | no                  | not yet
 * h8300      | no                  | not yet
 * hppa       | yes, -sub           | not yet
 * ia64       | yes, -sub           | not yet
 * lm32       | no                  | not yet
 * m32c       | no                  | not yet
 * m32r       | no                  | not yet
 * m68k       | yes, incomplete sub | not yet
 * m68hc1x    | no                  | not yet
 * mcore      | no                  | not yet
 * mep        | no                  | not yet
 * microblaze | no                  | not yet
 * mips       | yes, +endian -sub   | not yet
 * mips64     | yes, +endian -sub   | not yet
 * mmix       | no                  | not yet
 * mn10300    | no                  | not yet
 * ns32k      | yes                 | not yet
 * pdp-11     | no                  | not yet
 * picochip   | no                  | not yet
 * powerpc    | yes, -sub           | not yet
 * powerpc64  | yes, -sub           | not yet
 * rx         | no                  | not yet
 * s390       | yes                 | not yet
 * s390x      | yes                 | not yet
 * score      | no                  | not yet
 * sh         | yes, -sub -endian   | not yet
 * sparc      | yes, -sub           | not yet
 * sparc64    | yes, -sub           | not yet
 * tile       | no                  | not yet
 * vax        | yes                 | not yet
 * x86        | yes, +sub           | in progress
 * xtensa     | no                  | not yet
 */

/*
 * FIXME!
 * #if out a bunch of code to avoid compiler breakage with mingw32 and some
 * other platforms.
 */
#define TEMPORARY_DISABLE 1


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


/* Generic arc cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__arc__)
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "ARC (big endian)"
#  else
#    define PLATFORM_CPU "ARC (little endian)"
#  endif
#endif


/* Generic arm cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__arm__)

#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_ENDIAN " (big endian)"
#  else
#    define PLATFORM_ENDIAN " (little endian)"
#  endif

/* find specific cpu name definitions first */
#  if defined(arm2)
#    define PCPU "ARM2"
#  elif defined(arm250)
#    define PCPU "ARM250"
#  elif defined(arm3)
#    define PCPU "ARM3"
#  elif defined(arm6)
#    define PCPU "ARM6"
#  elif defined(arm60)
#    define PCPU "ARM60"
#  elif defined(arm600)
#    define PCPU "ARM600"
#  elif defined(arm610)
#    define PCPU "ARM610"
#  elif defined(arm620)
#    define PCPU "ARM620"
#  elif defined(arm7)
#    define PCPU "ARM7"
#  elif defined(arm7d)
#    define PCPU "ARM7D"
#  elif defined(arm7di)
#    define PCPU "ARM7DI"
#  elif defined(arm70)
#    define PCPU "ARM70"
#  elif defined(arm700)
#    define PCPU "ARM700"
#  elif defined(arm700i)
#    define PCPU "ARM700I"
#  elif defined(arm710)
#    define PCPU "ARM710"
#  elif defined(arm720)
#    define PCPU "ARM720"
#  elif defined(arm710c)
#    define PCPU "ARM710C"
#  elif defined(arm7100)
#    define PCPU "ARM7100"
#  elif defined(arm7500)
#    define PCPU "ARM7500"
#  elif defined(arm7500fe)
#    define PCPU "ARM7500FE"
#  elif defined(arm7m)
#    define PCPU "ARM7M"
#  elif defined(arm7dm)
#    define PCPU "ARM7DM"
#  elif defined(arm7dmi)
#    define PCPU "ARM7DMI"
#  elif defined(arm8)
#    define PCPU "ARM8"
#  elif defined(arm810)
#    define PCPU "ARM810"
#  elif defined(strongarm)
#    define PCPU "StrongARM"
#  elif defined(strongarm110)
#    define PCPU "StrongARM110"
#  elif defined(strongarm1100)
#    define PCPU "StrongARM1100"
#  elif defined(strongarm1110)
#    define PCPU "StrongARM1110"
#  elif defined(arm7tdmi)
#    define PCPU "ARM7TDMI"
#  elif defined(arm7tdmis)
#    define PCPU "ARM7TDMI-S"
#  elif defined(arm710t)
#    define PCPU "ARM710T"
#  elif defined(arm720t)
#    define PCPU "ARM720T"
#  elif defined(arm740t)
#    define PCPU "ARM740T"
#  elif defined(arm9)
#    define PCPU "ARM9"
#  elif defined(arm9tdmi)
#    define PCPU "ARM9TDMI"
#  elif defined(arm920)
#    define PCPU "ARM920"
#  elif defined(arm920t)
#    define PCPU "ARM920T"
#  elif defined(arm922t)
#    define PCPU "ARM922T"
#  elif defined(arm940t)
#    define PCPU "ARM940T"
#  elif defined(ep9312)
#    define PCPU "EP9312"
#  elif defined(arm10tdmi)
#    define PCPU "ARM10TDMI"
#  elif defined(arm1020t)
#    define PCPU "ARM1020T"
#  elif defined(arm9e)
#    define PCPU "ARM9E"
#  elif defined(arm946es)
#    define PCPU "ARM946E-S"
#  elif defined(arm966es)
#    define PCPU "ARM966E-S"
#  elif defined(arm968es)
#    define PCPU "ARM968E-S"
#  elif defined(arm10e)
#    define PCPU "ARM10E"
#  elif defined(arm1020e)
#    define PCPU "ARM1020E"
#  elif defined(arm1022e)
#    define PCPU "ARM1022E"
#  elif defined(xscale)
#    define PCPU "XSCALE"
#  elif defined(iwmmxt)
#    define PCPU "IWMMXT"
#  elif defined(iwmmxt2)
#    define PCPU "IWMMXT2"
#  elif defined(arm926ejs)
#    define PCPU "ARM926EJ-S"
#  elif defined(arm1026ejs)
#    define PCPU "ARM1026EJ-S"
#  elif defined(arm1136js)
#    define PCPU "ARM1136J-S"
#  elif defined(arm1136jfs)
#    define PCPU "ARM1136JF-S"
#  elif defined(arm1176jzs)
#    define PCPU "ARM1176JZ-S"
#  elif defined(arm1176jzfs)
#    define PCPU "ARM1176JZF-S"
#  elif defined(mpcorenovfp)
#    define PCPU "MPCORENOVFP"
#  elif defined(mpcore)
#    define PCPU "MPCORE"
#  elif defined(arm1156t2s)
#    define PCPU "ARM1156T2-S"
#  elif defined(arm1156t2fs)
#    define PCPU "ARM1156T2F-S"
#  elif defined(cortexa5)
#    define PCPU "CORTEX-A5"
#  elif defined(cortexa8)
#    define PCPU "CORTEX-A8"
#  elif defined(cortexa15)
#    define PCPU "CORTEX-A15"
#  elif defined(cortexr4)
#    define PCPU "CORTEX-R4"
#  elif defined(cortexr4f)
#    define PCPU "CORTEX-R4F"
#  elif defined(cortexm4)
#    define PCPU "CORTEX-M4"
#  elif defined(cortexm3)
#    define PCPU "CORTEX-M3"
#  elif defined(cortexm1)
#    define PCPU "CORTEX-M1"
#  elif defined(cortexm0)
#    define PCPU "CORTEX-M0"
#  else

/* find out by generic cpu defines what we are compiling for */

#    if defined(__MAVERICK__)
#      define PCPU "Maverick"
#    elif defined(__XSCALE__)
#      define PCPU "XSCALE"
#    elif defined(__IWMMXT__)
#      define PCPU "IWMMXT"
#    elif defined(__ARM_NEON__)
#      define PCPU "NEON"
#    elif defined(__thumb__)
#      define PCPU "Thumb"
#    elif defined(__thumb2__)
#      define PCPU "Thumb2"
#    else

/* Unknown cpu, so handle as plain ARM */
#      define PCPU "ARM"
#    endif
#  endif
#  define PLATFORM_CPU PCPU PLATFORM_ENDIAN
#endif


/* Generic avr32 cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__avr32__)
#define PLATFORM_CPU "AVR32"
#endif


/* Generic bfin cpu discovery */
#if !defined(PLATFORM_CPU) && defined(BFIN)
#if defined(__ADSPBF512__)
#define PLATFORM_CPU "BFIN512"
#elif defined(__ADSPBF514__)
#define PLATFORM_CPU "BFIN514"
#elif defined(__ADSPBF516__)
#define PLATFORM_CPU "BFIN516"
#elif defined(__ADSPBF518__)
#define PLATFORM_CPU "BFIN518"
#elif defined(__ADSPBF522__)
#define PLATFORM_CPU "BFIN522"
#elif defined(__ADSPBF523__)
#define PLATFORM_CPU "BFIN523"
#elif defined(__ADSPBF524__)
#define PLATFORM_CPU "BFIN524"
#elif defined(__ADSPBF525__)
#define PLATFORM_CPU "BFIN525"
#elif defined(__ADSPBF526__)
#define PLATFORM_CPU "BFIN526"
#elif defined(__ADSPBF527__)
#define PLATFORM_CPU "BFIN527"
#elif defined(__ADSPBF531__)
#define PLATFORM_CPU "BFIN531"
#elif defined(__ADSPBF532__)
#define PLATFORM_CPU "BFIN532"
#elif defined(__ADSPBF533__)
#define PLATFORM_CPU "BFIN533"
#elif defined(__ADSPBF534__)
#define PLATFORM_CPU "BFIN534"
#elif defined(__ADSPBF536__)
#define PLATFORM_CPU "BFIN536"
#elif defined(__ADSPBF537__)
#define PLATFORM_CPU "BFIN537"
#elif defined(__ADSPBF538__)
#define PLATFORM_CPU "BFIN538"
#elif defined(__ADSPBF539__)
#define PLATFORM_CPU "BFIN539"
#elif defined(__ADSPBF542M__)
#define PLATFORM_CPU "BFIN542M"
#elif defined(__ADSPBF542__)
#define PLATFORM_CPU "BFIN542"
#elif defined(__ADSPBF544M__)
#define PLATFORM_CPU "BFIN544M"
#elif defined(__ADSPBF544__)
#define PLATFORM_CPU "BFIN544"
#elif defined(__ADSPBF547M__)
#define PLATFORM_CPU "BFIN547M"
#elif defined(__ADSPBF547__)
#define PLATFORM_CPU "BFIN547"
#elif defined(__ADSPBF548M__)
#define PLATFORM_CPU "BFIN548M"
#elif defined(__ADSPBF548__)
#define PLATFORM_CPU "BFIN548"
#elif defined(__ADSPBF549M__)
#define PLATFORM_CPU "BFIN549M"
#elif defined(__ADSPBF549__)
#define PLATFORM_CPU "BFIN549"
#elif defined(__ADSPBF561__)
#define PLATFORM_CPU "BFIN561"
#else
#define PLATFORM_CPU "BFIN"
#endif
#endif


/* Generic cris cpu discovery */
#if !defined(PLATFORM_CPU) && defined(CRIS)

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


/* Generic sh cpu descovery */
#if !defined(PLATFORM_CPU) && defined(__sh3__)
#  ifdef WORDS_BIGENDIAN
#    define PLATFORM_CPU "SH3 (big endian)"
#  else
#    define PLATFORM_CPU "SH3 (little endian)"
#  endif
#endif

#if !defined(PLATFORM_CPU) && defined(__SH4__)
#define PLATFORM_CPU "SH4"
#endif


/* Generic sparc64 cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__sparc64__)
#define PLATFORM_CPU "SPARC64"
#endif


/* Generic sparc cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__sparc__)
#define PLATFORM_CPU "SPARC"
#endif


/* Generic vax cpu discovery */
#if !defined(PLATFORM_CPU) && defined(__vax__)
#define PLATFORM_CPU "VAX"
#endif

/* Generic x86 cpu discovery */
#if !defined(PLATFORM_CPU) && !defined(FIND_X86_CPU) && (defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && !defined(__amd64__) && !defined(__x86_64__)
#define FIND_X86_CPU
#endif

#if !defined(PLATFORM_CPU) && defined(FIND_X86_CPU)

static char *unknown = "Unknown x86-compatible";

/* cpuid function */
#ifdef __GNUC__
#define cpuid(func, ax, bx, cx, dx) \
    __asm__ __volatile__ ("cpuid":  \
    "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func))
#else
#define cpuid(func, a, b, c, d) \
    asm { \
        mov eax, func \
        cpuid \
        mov a, eax \
        mov b, ebx \
        mov c, ecx \
        mov d, edx \
    }
#endif

/* Detect 8086/8088 CPU */
inline static int is_8086(void)
{
    int is86;

#ifdef __GNUC__
    __asm__ __volatile__ ("pushf;"
                          "pop %ax;"
                          "mov %ax, %cx;"
                          "and 0x0fff, %ax;"
                          "push %ax;"
                          "popf;"
                          "pushf;"
                          "pop %ax;"
                          "and 0x0f000, %ax;"
                          "cmp 0x0f000, %ax;"
                          "jne check86done;"
                          "movl $1, is86;"
                          "check86done:");
#else
    asm {
        pushfd
        pop ax
        mov cx, ax
        and ax, 0fffh
        push ax
        popfd
        pushfd
        pop ax
        and ax, 0f000h
        cmp ax, 0f000h
        jne check86done
        mov is86, 1
        check86done:
    }
#endif

    return is86;
}

/* Detect 80386 CPU */
static int is_80386(void)
{
#if !TEMPORARY_DISABLE
    int is386;

#ifdef __GNUC__
    __asm__ __volatile__ ("pushf;"
                          "pop %eax;"
                          "movl %eax, %ecx;"
                          "xorl 0x40000, %eax;"
                          "push %eax;"
                          "popf;"
                          "pushf;"
                          "pop %eax;"
                          "xorl %ecx, %eax;"
                          "movl $1, is386;"
                          "jz donecheck386;"
                          "movl $0, is386;"
                          "donecheck386:");
#else
    asm {
        pushfd
        pop eax
        mov ecx, eax
        xor eax, 0x40000
        push eax
        popf
        pushf
        pop eax
        xor eax, ecx
        mov is386, 1
        jz donecheck386
        mov is386, 0
        donecheck386:
    }
#endif
    return is386;
#else
    return 0;
#endif
}

static int is_not_80386(void)
{
    return !is_80386();
}

/* check if cpuid is present */
inline static int has_cpuid(void)
{
    int hasCPUID;

#ifdef __GNUC__
    __asm__ __volatile__ ("pushf;"
                          "pop %eax;"
                          "movl %eax, %ecx;"
                          "andl 0x00200000, %ecx;"
                          "xorl 0x00200000, %eax;"
                          "push %eax;"
                          "popf;"
                          "pushf;"
                          "pop %eax;"
                          "andl 0x00200000, %eax;"
                          "xorl %ecx, %eax;"
                          "movl %eax, hasCPUID");
#else
    asm {
        pushfd
        pop eax
        mov ecx, eax
        and ecx, 0x00200000
        xor eax, 0x00200000
        push eax
        popfd
        pushfd
        pop eax
        and eax, 0x00200000
        xor eax, ecx
        mov hasCPUID, eax
    }
#endif

    return hasCPUID;
}

/* check for old i80486 */
inline static int is_i80486(void)
{
    int is486;

#ifdef __GNUC__
    __asm__ __volatile__ ("movl $0x5555, %eax;"
                          "xorl %edx, %edx;"
                          "movl $2, %ecx;"
                          "clc;"
                          "divl %ecx;"
                          "jnc donecheck486;"
                          "movl $1, is0486;"
                          "donecheck486:");
#else
    asm {
        mov eax, 0x5555
        xor edx, edx
        mov ecx, 2
        clc
        div ecx
        jnc donecheck486
        mov is486, 1
        donecheck486:
    }
#endif

    return is486;
}

/* check for cyrix cpu */
inline static int is_cyrix(void)
{
#if !TEMPORARY_DISABLE
    int cyrix;

#ifdef __GNUC__
    __asm__ __volatile__ ("xor %ax, %ax;"
                          "sahf;"
                          "mov $5, %ax;"
                          "mov $2, %bx;"
                          "div %bl;"
                          "lahf;"
                          "cmp $2, %ah;"
                          "jne donecheckcyrix;"
                          "movl $1, cyrix;"
                          "donecheckcyrix:");
#else
    asm {
        xor ax, ax
        sahf
        mov ax, 5
        mov bx, 2
        div bl
        lahf
        cmp ah, 2
        jne donecheckcyrix
        mov cyrix, 1
        donecheckcyrix:
    }
#endif
    return cyrix;
#else
    return 0;
#endif
}

inline static int is_am386dxllv(void)
{
    unsigned int result, temp;
    int x;
    unsigned int temp2 = (0x55555555 - (unsigned)&x);
    unsigned int temp3 = (unsigned)&x;

#ifdef __GNUC__
    __asm__ __volatile__ ("pusha;"
                          "addl $4, %0;"
                          "popa;"
                          "movl 0x55555555(%0, %3, 2), %1"
                          : "=a" (result), "=r" (temp)
                          : "a" (temp2), "r" (temp));
#else
    /* TODO */
#endif

    temp3 = (unsigned)&x;
    if ((result + temp3) * 3 != -1) {
        return 1;
    }
    return 0;
}

#define is_vendor_string(string) \
    DWORD regax, regbx, regcx, regdx; \
    char type_buf[13]; \
    cpuid(0, regax, regbx, regcx, regdx); \
    memcpy(type_buf + 0, &regbx, 4); \
    memcpy(type_buf + 4, &regdx, 4); \
    memcpy(type_buf + 8, &regcx, 4); \
    if (!strcmp(type_buf, string)) { \
        return 1; \
    } \
    return 0

inline static int is_intel_vendor(void)
{
    is_vendor_string("GenuineIntel");
}

inline static int is_amd_vendor(void)
{
    is_vendor_string("AuthenticAMD");
}

inline static int is_cyrix_vendor(void)
{
    is_vendor_string("CyrixInstead");
}

static inline int is_umc_vendor(void)
{
    is_vendor_string("UMC UMC UMC ");
}

static inline int is_rise_vendor(void)
{
    is_vendor_string("RiseRiseRise");
}

static inline int is_nexgen_vendor(void)
{
    is_vendor_string("NexGenDriven");
}

static inline int is_centaur_vendor(void)
{
    is_vendor_string("CentaurHauls");
}

static inline int is_sis_vendor(void)
{
    is_vendor_string("SiS SiS SiS ");
}

static inline int is_transmeta_vendor(void)
{
    is_vendor_string("GenuineTMx86");
}

static inline int is_nsc_vendor(void)
{
    is_vendor_string("Geode by NSC");
}

typedef struct cpuid_model_s {
    DWORD type;
    DWORD mask;
    char *name;
    int (*detect)(void);
} cpuid_model_t;

/* 0000 0000 XXFF XXMM TTTT FFFF MMMM SSSS */

cpuid_model_t cpu_models[] = {
    { 0x0000, 0xff00, "i386DX", is_80386 },
    { 0x0300, 0xfff0, "Am386DX(L/LV)", is_am386dxllv },
    { 0x0300, 0xfff0, "i386DX", NULL },
    { 0x0340, 0xfff0, "RapidCAD (tm)", NULL },
    { 0x03D5, 0xffff, "NexGen Nx5-100", NULL },
    { 0x2300, 0xfff0, "Am386DX(L/LV)", is_am386dxllv },
    { 0x2300, 0xfff0, "i386SX", NULL },
    { 0x3300, 0xff00, "i376", NULL },
    { 0x4300, 0xfffe, "i376", NULL },
    { 0x4302, 0xffff, "i376", NULL },
    { 0x4300, 0xff00, "i386SL", NULL },
    { 0xA300, 0xff00, "IBM 386SLC", NULL },

    { 0x0000, 0xffff, "486DX", is_not_80386 },
    { 0x0005, 0xffff, "Cx486S/D", NULL },
    { 0x0006, 0xffff, "Cx486DX", NULL },
    { 0x0007, 0xffff, "Cx486DX2", NULL },
    { 0x0008, 0xffff, "Cx486DX4", NULL },

    { 0x000400, 0xffffff, "Intel i80486DX-25/33", is_intel_vendor }, /* CI */
    { 0x000401, 0xffffff, "Intel i80486DX-50", is_intel_vendor },    /* CI */
    { 0x000402, 0xffffff, "Intel i80486SX", is_intel_vendor },       /* CI */
    { 0x000403, 0xffffff, "Intel i80486DX/2", is_intel_vendor },     /* CI */
    { 0x000404, 0xffffff, "Intel i80486SL", is_intel_vendor },       /* CI */
    { 0x000405, 0xffffff, "Intel i80486SX/2", is_intel_vendor },     /* CI */
    { 0x000407, 0xffffff, "Intel i80486DX/2-WB", is_intel_vendor },  /* CI */
    { 0x000408, 0xffffff, "Intel i80486DX/4", is_intel_vendor },     /* CI */
    { 0x000409, 0xffffff, "Intel i80486DX/4-WB", is_intel_vendor },  /* CI */
    { 0x000400, 0xfffff0, "Intel i80486", is_intel_vendor },         /* CI */

    { 0x0400, 0xfff0, "Cx486SLC", is_cyrix },
    { 0x0400, 0xfff0, "Am486DX", is_amd_vendor },
    { 0x0400, 0xfff0, "i486DX", NULL },
    { 0x0410, 0xffff, "Cx486SLC", is_cyrix_vendor },
    { 0x0410, 0xffff, "TI486SLC/DLC/e / TI486SXL(C)/2", is_cyrix },
    { 0x0410, 0xffff, "Am486DX", is_amd_vendor },
    { 0x0410, 0xffff, "i486DX50", NULL },
    { 0x0411, 0xffff, "TI486SLC/DLC/e / TI486SXL(C)/2", is_cyrix },
    { 0x0411, 0xffff, "Am486DX", is_amd_vendor },
    { 0x0411, 0xffff, "i486DX50", NULL },
    { 0x0412, 0xffff, "Am486DX/DXL/DXLV/DE", NULL },
    { 0x0413, 0xffff, "i486DX50", NULL },
    { 0x0414, 0xffff, "i486DX50", NULL },
    { 0x0415, 0xffff, "i486DX50", NULL },
    { 0x0410, 0xfff0, "Cx486SLC", is_cyrix },
    { 0x0410, 0xfff0, "UMC U5(S)D", is_umc_vendor },
    { 0x0410, 0xfff0, "Am486DX", is_amd_vendor },
    { 0x0410, 0xfff0, "i486DX50", NULL },
    { 0x0420, 0xffff, "Cx486SLC", is_cyrix },
    { 0x0420, 0xffff, "i486SX / i487SX", NULL },
    { 0x0421, 0xffff, "i487SX", NULL },
    { 0x0422, 0xffff, "i486SX / i487SX", NULL },
    { 0x0423, 0xffff, "UMC U5S", is_umc_vendor },
    { 0x0423, 0xffff, "i486SX", NULL },
    { 0x0424, 0xffff, "i486SX", NULL },
    { 0x0427, 0xffff, "i486SX", NULL },
    { 0x0428, 0xffff, "Cx5x86", is_cyrix },
    { 0x0428, 0xffff, "i486SX", NULL },
    { 0x0429, 0xffff, "Cx5x86", NULL },
    { 0x042A, 0xffff, "Cx5x86", is_cyrix },
    { 0x042A, 0xffff, "i486SX", NULL },
    { 0x042B, 0xffff, "Cx5x86", is_cyrix },
    { 0x042B, 0xffff, "i486SX", NULL },
    { 0x042C, 0xffff, "Cx5x86", NULL },
    { 0x042D, 0xffff, "Cx5x86", NULL },
    { 0x042E, 0xffff, "Cx5x86", is_cyrix },
    { 0x042E, 0xffff, "i486SX", NULL },
    { 0x042F, 0xffff, "Cx5x86", NULL },
    { 0x0420, 0xfff0, "UMC U5S", is_umc_vendor },
    { 0x0420, 0xfff0, "Cx486SLC / Cx5x86", is_cyrix },
    { 0x0420, 0xfff0, "i486SX / i486GX / i487SX", NULL },
    { 0x0432, 0xffff, "Am486DX2 / Am486DXL2 / Am486DX4", is_amd_vendor },
    { 0x0432, 0xffff, "i486DX2", NULL },
    { 0x1432, 0xffff, "i486DX2 (overdrive)", NULL },
    { 0x0433, 0xffff, "i486DX2", NULL },
    { 0x1433, 0xffff, "i486DX2 (overdrive)", NULL },
    { 0x0434, 0xffff, "Am486DX2 / Am486DX4", is_amd_vendor },
    { 0x0434, 0xffff, "i486DX2", NULL },
    { 0x1434, 0xffff, "i486DX2 (overdrive)", NULL },
    { 0x0435, 0xffff, "i486DX2", NULL },
    { 0x0436, 0xffff, "i486DX2WT", NULL },
    { 0x0430, 0xfff0, "UMC U486DX2", is_umc_vendor },
    { 0x0430, 0xfff0, "Am486DX2 / Am486DE2 / Am486DX4", is_amd_vendor },
    { 0x0430, 0xfff0, "i486DX2 / i487SX", NULL },
    { 0x1430, 0xfff0, "i486DX2 (overdrive)", NULL },
    { 0x0440, 0xfffe, "i486SL", NULL },
    { 0x0440, 0xfff0, "MediaGX", is_cyrix_vendor },
    { 0x0440, 0xfff0, "i486SL / i486DXL", NULL },
    { 0x045B, 0xffff, "i486SX2", NULL },
    { 0x0450, 0xfff0, "UMC U486SX2", is_umc_vendor },
    { 0x0450, 0xfff0, "i486SX2", NULL },
    { 0x0470, 0xffff, "i486DX2WB", NULL },
    { 0x0474, 0xffff, "Am486DX2 / Am486DX4", NULL },
    { 0x0470, 0xfff0, "Am486DX2", is_amd_vendor },
    { 0x0470, 0xfff0, "i486DX2", NULL },
    { 0x0480, 0xffff, "IBM BL486DX2 / Cx486DX2-V / TI486DX2", is_cyrix },
    { 0x0480, 0xffff, "i486DX4", NULL },
    { 0x0481, 0xffff, "TI486DX4", NULL },
    { 0x0483, 0xffff, "i486DX4", NULL },
    { 0x0484, 0xffff, "Am486DX4 / Am5x86", is_amd_vendor },
    { 0x0484, 0xffff, "i486DX4", NULL },
    { 0x0480, 0xfff0, "Am486DX4", is_amd_vendor },
    { 0x0480, 0xfff0, "i486DX4", NULL },
    { 0x1480, 0xfff0, "i486DX4 (overdrive)", NULL },
    { 0x0490, 0xffff, "Cx5x86", is_cyrix },
    { 0x0490, 0xffff, "i486DX4", NULL },
    { 0x0494, 0xffff, "Am486DX4 / Am5x86", NULL },
    { 0x0490, 0xfff0, "Cx5x86", is_cyrix },
    { 0x0490, 0xfff0, "i486DX4WB", NULL },
    { 0x04E4, 0xffff, "Am486DX5-133 / Am5x86", NULL },
    { 0x04E0, 0xfff0, "Am5x86", NULL },
    { 0x04F4, 0xffff, "Am486DX5-133 / Am5x86", NULL },
    { 0x04F0, 0xfff0, "Am5x86", NULL },
    { 0x1480, 0xffff, "i486DX4 (overdrive)", NULL },
    { 0x8400, 0xff00, "IBM 486BLX/BLX2/BLX3", NULL },
    { 0xA400, 0xfff0, "IBM 486SLC", NULL },
    { 0xA412, 0xffff, "IBM 486SLC2", NULL },
    { 0xA410, 0xfff0, "IBM 486SLC(2)", NULL },
    { 0xA420, 0xfff0, "IBM 486SLC2", NULL },
    { 0xA439, 0xffff, "IBM 486SLC3", NULL },
    { 0xA430, 0xfff0, "IBM 486SLC2/SLC3", NULL },
    { 0xA480, 0xffff, "Cx486Dx2-V / IBM BL486DX2", NULL },
    { 0xA400, 0xff00, "IBM 486SLC", NULL },

    { 0x000500, 0xffffff, "Intel Pentium 60/66", is_intel_vendor }, /* CI */
    { 0x001501, 0xffffff, "Intel Pentium 60/66 (overdrive for P5)", is_intel_vendor }, /* CI */

    { 0x0500, 0xfffe, "Am5k86 (SSA5)", NULL },
    
    { 0x000501, 0xffffff, "Intel Pentium 60/66", is_intel_vendor },                /* CI */
    { 0x001502, 0xffffff, "Intel Pentium 75 - 200 (overdrive for P54C)", is_intel_vendor }, /* CI */
    { 0x000502, 0xffffff, "Intel Pentium P54C 75 - 200", is_intel_vendor }, /* CI */
    { 0x001503, 0xffffff, "Intel Pentium (overdrive for i486 P24T)", is_intel_vendor }, /* CI */
    { 0x001504, 0xffffff, "Intel Pentium (overdrive for P54C)", is_intel_vendor }, /* CI */
    { 0x000504, 0xffffff, "Intel Pentium MMX P55C", is_intel_vendor }, /* CI */

    { 0x0504, 0xffff, "Nx586", NULL },
    { 0x0506, 0xffff, "Nx586", NULL },

    { 0x000507, 0xffffff, "Intel Pentium MMX P54C 75 - 200", is_intel_vendor }, /* CI */
    { 0x000508, 0xffffff, "Intel Pentium MMX P55C", is_intel_vendor }, /* CI */
    { 0x000500, 0xfffff0, "Intel Pentium", is_intel_vendor }, /* CI */

    { 0x0500, 0xfff0, "Rise mP6 iDragon", is_rise_vendor },
    { 0x0500, 0xfff0, "Nx586", is_nexgen_vendor },
    { 0x0500, 0xfff0, "Am5k86", is_amd_vendor },


    { 0x0511, 0xffff, "K5", NULL },
    { 0x0512, 0xffff, "K5", is_amd_vendor },
    { 0x0512, 0xffff, "Pentium", NULL },
    { 0x0513, 0xffff, "Pentium", NULL },
    { 0x0514, 0xffff, "K5", is_amd_vendor },
    { 0x0514, 0xffff, "Pentium", NULL },

    { 0x000600, 0xfffffe, "Intel Pentium Pro", is_intel_vendor }, /* CI */
    { 0x001603, 0xffffff, "Intel Pentium II (overdrive)", is_intel_vendor }, /* CI */
    { 0x000603, 0xffffff, "Intel Pentium II (Klamath)", is_intel_vendor }, /* CI */
    { 0x000604, 0xffffff, "Intel Pentium P55CT overdrive (Deschutes)", is_intel_vendor }, /* CI */
#if 0
    { 0x000605, 0xffffff, "Intel Pentium II Xeon (Deschutes)", is_intel_vendor_and_Xeon_Deschutes }, /* CI */
#endif

    { 0, 0, NULL, NULL }
};

/* runtime cpu detection */
inline static char* platform_get_runtime_cpu(void)
{
    DWORD regax, regbx, regcx, regdx;
    int hasCPUID;
    int i;

#if !TEMPORARY_DISABLE
    hasCPUID = detect_cpuid();
    if (hasCPUID) {
        cpuid(1, regax, regbx, regcx, regdx);
        for (i = 0; cpu_models[i].name != NULL; i++) {
            if ((regax & cpu_models[i].mask) == cpu_models[i].type) {
                if (cpu_models[i].detect == NULL) {
                    return cpu_models[i].name;
                } else {
                    if (cpu_models[i].detect() == 1) {
                        return cpu_models[i].name;
                    }
                }
            }
        }
    } else {
    }
#else
    return "";
#endif
 
}

#define PLATFORM_GET_RUNTIME_CPU_DECLARED

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

