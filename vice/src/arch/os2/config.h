//#ifndef _CONFIG_H
//#define _CONFIG_H

/* src/config.h.  Generated automatically by configure.  */
/* src/config.h.in.  Generated automatically from configure.in by autoheader.  */

#define OS2 /* Operating System is OS/2 */

//#define _POSIX_SOURCE // vac++
// INCLUDE <os2.h> ???????  with all INCLS befor?

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

#ifdef __EMX__
  #define HAVE_ALLOCA      1  /* Define if you have alloca, as a function or macro.  */
  #define HAVE_ALLOCA_H    1  /* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
  #define HAVE_DIRENT_H    1  /* Define if you have the <dirent.h> header file.  */
  #define HAVE_STRINGS_H   1  /* Define if you have the <strings.h> header file.  */
  #define HAVE_SYS_FILE_H  1  /* Define if you have the <sys/file.h> header file.  */
  #define HAVE_SYS_IOCTL_H 1  /* Define if you have the <sys/ioctl.h> header file.  */
#endif

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
//#define size_t unsigned

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if lex declares yytext as a char * by default, not a char[].  */
#define YYTEXT_POINTER 1

/* Define to your X11 display depth.  */
#define X_DISPLAY_DEPTH 0

/* Define if you want to use Rober W. McMullen's TextField widget.  */
//#define ENABLE_TEXTFIELD 1

/* Do we use MITSHM extensions?  */
#define USE_MITSHM 1

/* Are we using the `readline' library or one replacement?  */
//#define HAVE_READLINE 1

/* Do we want to enable RS232 support via ACIA emulation?  */
//#define HAVE_RS232 1

/* Do we want to enable Printer support?  */
//#define HAVE_PRINTER 1

/* Do we want to enable the ReSID code support?  */
#define HAVE_RESID            1

/* Do we support a 1351 mouse?  */
//#define HAVE_MOUSE 1

/* Do we have to emulate a joystick?  */
#define HAS_JOYSTICK          1

/* Is digital joystick support avaiable?  */
/* #undef HAS_DIGITAL_JOYSTICK */

/* see zfile.c */
#define NAME_NEED_QM          1

/* This seems to be needed to prevent the emulator from FPEs */
#define FIXPOINT_ARITHMETIC   1  

#define SIZEOF_UNSIGNED_INT   4   /* The number of bytes in a unsigned int.           */
#define SIZEOF_UNSIGNED_LONG  4   /* The number of bytes in a unsigned long.          */
#define SIZEOF_UNSIGNED_SHORT 2   /* The number of bytes in a unsigned short.         */

#define HAVE_ATEXIT           1   /* Define if you have the atexit function.          */
#define HAVE_GETTIMEOFDAY     1   /* Define if you have the gettimeofday function.    */
#define HAVE_MEMMOVE          1   /* Define if you have the memmove function.         */
#define HAVE_STRERROR         1   /* Define if you have the strerror function.        */

#define strcasecmp stricmp        /* Define if you have the strcasecmp function.      */
#define HAVE_STRCASECMP

#define strncasecmp strnicmp      /* Define if you have the strncasecmp function.     */
#define HAVE_STRNCASECMP

#define HAVE_IO_H             1   /* Define if you have the <io.h> header file        */
#define HAVE_SIGNAL_H         1   /* Define if you have the <signal.h> header file    */
#define HAVE_ERRNO_H          1   /* Define if you have the <errno.h> header file     */
#define HAVE_FCNTL_H          1   /* Define if you have the <fcntl.h> header file.    */
#define HAVE_LIMITS_H         1   /* Define if you have the <limits.h> header file.   */
#define HAVE_REGEXP_H         1   /* Define if you have the <regexp.h> header file.   */
#define HAVE_SGTTY_H          1   /* Define if you have the <sgtty.h> header file.    */
#define HAVE_SYS_PARAM_H      1   /* Define if you have the <sys/param.h> header file.*/
#define HAVE_SYS_TIME_H       1   /* Define if you have the <sys/time.h> header file. */

/* #undef HAVE_USLEEP *//* Define if you have the usleep function.  */
/* #undef HAVE_REGEX_H *//* Define if you have the <regex.h> header file.  */
/* #undef HAVE_SYS_DIR_H *//* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIRENT_H *//* Define if you have the <sys/dirent.h> header file.  */
