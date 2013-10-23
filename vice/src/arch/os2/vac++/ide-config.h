//#ifndef _CONFIG_H
//#define _CONFIG_H

/* src/config.h.  Generated automatically by configure.  */
/* src/config.h.in.  Generated automatically from configure.in by autoheader.  */

#ifndef __OS2__
#define __OS2__ /* Operating System is OS/2 */
#endif

//#define MAIN_PROGRAM Main

/*
 * VAC++ doesn't define __i386__, __i386__ is used in vice.h
 */
#define __i386__ 1

#ifdef __EMX__
#define HAVE_ALLOCA      1  /* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA_H    1  /* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_DIRENT_H    1  /* Define if you have the <dirent.h> header file.  */
#define HAVE_STRINGS_H   1  /* Define if you have the <strings.h> header file.  */
#define HAVE_SYS_FILE_H  1  /* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_IOCTL_H 1  /* Define if you have the <sys/ioctl.h> header file.  */
#endif

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if lex declares yytext as a char * by default, not a char[].  */
#define YYTEXT_POINTER 1

/* Do we use MITSHM extensions?  */
#define USE_MITSHM 1

/* Do we want to enable the ReSID code support?  */
#if defined(__X64___) || defined(__X128__) || defined(__XCBM__) || defined(__X64DTV__) || defined(__XSCPU64__)
#define HAVE_RESID 1
#endif

/* Do we support a 1351 mouse?  */
#if defined(__X64__) || defined(__X128__) || defined(__X64DTV__) || defined(__XVIC__) || defined (__XSCPU64__)
#define HAVE_MOUSE 1
#endif

/* Do we have to emulate a joystick?  */
#define HAS_JOYSTICK 1

/* VIC II Doublesize/-scan  */
#if defined(__X64__) || defined(__X128__) || defined(__XCBM__) || defined(__X64DTV__) || defined(__XSCPU64__)
#define HAVE_VIC_II 1
#endif

#if defined __XVIC__
#define HAVE_VIC 1
#endif

#ifdef __XPLUS4__
#define HAVE_TED 1
#endif

//y-direction
#ifdef __X128__
#define HAVE_VDC 1
#endif

#if defined(__XCBM__) || defined(__XPET__)
#define HAVE_CRTC 1
#endif

#if defined(HAVE_VIC) || defined(HAVE_VIC_II) || defined(HAVE_TED)
#define HAVE_PAL 1
#endif

/* see zfile.c */
#define NAME_NEED_QM 1

/* This seems to be needed to prevent the emulator from FPEs */
#define FIXPOINT_ARITHMETIC  1

#define SIZEOF_UNSIGNED_INT   4   /* The number of bytes in a unsigned int.           */
#define SIZEOF_UNSIGNED_LONG  4   /* The number of bytes in a unsigned long.          */
#define SIZEOF_UNSIGNED_SHORT 2   /* The number of bytes in a unsigned short.         */

#define HAVE_ATEXIT       1   /* Define if you have the atexit function.          */
#define HAVE_MEMMOVE      1   /* Define if you have the memmove function.         */
#define HAVE_STRERROR     1   /* Define if you have the strerror function.        */

#define strcasecmp stricmp        /* Define if you have the strcasecmp function.      */
#define HAVE_STRCASECMP 1

#define strncasecmp strnicmp      /* Define if you have the strncasecmp function.     */
#define HAVE_STRNCASECMP 1

#define HAVE_IO_H       1   /* Define if you have the <io.h> header file        */
#define HAVE_SIGNAL_H   1   /* Define if you have the <signal.h> header file    */
#define HAVE_DIRECT_H   1   /* Define if you have the <direct.h> header file    */
#define HAVE_ERRNO_H    1   /* Define if you have the <errno.h> header file     */
#define HAVE_FCNTL_H    1   /* Define if you have the <fcntl.h> header file.    */
#define HAVE_LIMITS_H   1   /* Define if you have the <limits.h> header file.   */
#define HAVE_REGEXP_H   1   /* Define if you have the <regexp.h> header file.   */
#define HAVE_SYS_TIME_H 1   /* Define if you have the <sys/time.h> header file. */
#define HAVE_GETCWD     1

#define HAVE_STRTOK_R			1
#define HAVE_STRREV			1
#define HAVE_STRLWR			1
#define HAVE_STRLCPY			1
#define HAVE_LTOA			1
#define HAVE_ULTOA			1
#define HAVE_VSNPRINTF		1
#define HAVE_SNPRINTF			1
