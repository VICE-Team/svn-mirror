//#ifndef _CONFIG_H
//#define _CONFIG_H


/* src/config.h.  Generated automatically by configure.  */
/* src/config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if Operating System is OS/2 */
#define OS2

#ifdef __IBMC__
 #define inline _Inline  // This means only a possible inline. See doku.
 #define getcwd _getcwd
 #define R_OK   4
 #define W_OK   2
 typedef int pid_t;
 #ifndef __EXTENDED__
  typedef long off_t;
 #endif
#endif

//#define _POSIX_SOURCE // vac++
// INCLUDE <os2.h> ???????  with all INCLS befor?

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

#ifdef __EMX__
/* Define if you have alloca, as a function or macro.  */
# define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
# define HAVE_ALLOCA_H 1

/* Define if you have the <dirent.h> header file.  */
# define HAVE_DIRENT_H 1

/* Define if you have the <strings.h> header file.  */
# define HAVE_STRINGS_H 1

/* Define vfork as fork if vfork does not work.  */
# define vfork fork

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

#endif

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `long' if <sys/types.h> doesn't define.  */
//#define off_t long

/* Define to `int' if <sys/types.h> doesn't define.  */
//#define pid_t int

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

/* Define if `sys_siglist' is declared by <signal.h>.  */
/* #undef SYS_SIGLIST_DECLARED */

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if the X Window System is missing or not being used.  */
/* #undef X_DISPLAY_MISSING */

/* Define if lex declares yytext as a char * by default, not a char[].  */
#define YYTEXT_POINTER 1

/* Define to your X11 display depth.  */
#define X_DISPLAY_DEPTH 0

/* Define if you want to use Rober W. McMullen's TextField widget.  */
//#define ENABLE_TEXTFIELD 1

/* Do we use MITSHM extensions?  */
#define USE_MITSHM 1

/* Is libXpm available?  */
//#define HAVE_LIBXPM 1

/* Do we want to use XFree86 VIDMODE extensions?  */
//#define USE_VIDMODE_EXTENSION 1

/* Are we using the `readline' library or one replacement?  */
//#define HAVE_READLINE 1

/* Do we want to enable RS232 support via ACIA emulation?  */
//#define HAVE_RS232 1

/* Do we want to enable Printer support?  */
//#define HAVE_PRINTER 1

/* Do we want to enable the ReSID code support?  */
//#define HAVE_RESID 1

/* Do we support a 1351 mouse?  */
//#define HAVE_MOUSE 1

/* Do you want to use MIDAS Sound System instead of Allegro for sound
   playback?  */
/* #undef USE_MIDAS_SOUND */

/* Do we have to emulate a joystick?  */
/* #undef HAS_JOYSTICK */

/* Is digital joystick support avaiable?  */
/* #undef HAS_DIGITAL_JOYSTICK */

/* Is a Linux compatible joystick support avaiable?  */
/* #undef LINUX_JOYSTICK */

/* Is a BSD compatible joystick support avaiable?  */
/* #undef BSD_JOYSTICK */

/* Package name.  */
#define PACKAGE "vice"

/* Version number. and a stupid workaround  */
#define VERSION      "1.1.5"
#define VICE2VERSION "1.1.5"

/* Define if this version is unstable.  */
#define UNSTABLE

/* The number of bytes in a unsigned int.  */
#define SIZEOF_UNSIGNED_INT 4

/* The number of bytes in a unsigned long.  */
#define SIZEOF_UNSIGNED_LONG 4

/* The number of bytes in a unsigned short.  */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define if you have the atexit function.  */
#define HAVE_ATEXIT 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the strcasecmp function.  */
//#define strcasecmp stricmp
#define strcasecmp stricmp
#define HAVE_STRCASECMP

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strncasecmp function.  */
#define strncasecmp strnicmp
#define HAVE_STRNCASECMP

/* Define if you have the usleep function.  */
/* #undef HAVE_USLEEP */

/* Define if you have the <SDL_audio.h> header file.  */
/* #undef HAVE_SDL_AUDIO_H */

/* Define if you have the <SDL_sleep.h> header file.  */
/* #undef HAVE_SDL_SLEEP_H */

/* Define if you have the <UMS/UMSAudioDevice.h> header file.  */
/* #undef HAVE_UMS_UMSAUDIODEVICE_H */

/* Define if you have the <X11/Sunkeysym.h> header file.  */
//#define HAVE_X11_SUNKEYSYM_H 1

/* Define if you have the <X11/xpm.h> header file.  */
//#define HAVE_X11_XPM_H 1

/* Define if you have the <allegro.h> header file.  */
/* #undef HAVE_ALLEGRO_H */

/* Define if you have the <dmedia/audio.h> header file.  */
/* #undef HAVE_DMEDIA_AUDIO_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <linux/soundcard.h> header file.  */
/* #undef HAVE_LINUX_SOUNDCARD_H */

/* Define if you have the <machine/soundcard.h> header file.  */
/* #undef HAVE_MACHINE_SOUNDCARD_H */

/* Define if you have the <midasdll.h> header file.  */
/* #undef HAVE_MIDASDLL_H */

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <regex.h> header file.  */
/* #undef HAVE_REGEX_H */

/* Define if you have the <regexp.h> header file.  */
#define HAVE_REGEXP_H 1

/* Define if you have the <sgtty.h> header file.  */
#define HAVE_SGTTY_H 1

/* Define if you have the <sys/audio.h> header file.  */
/* #undef HAVE_SYS_AUDIO_H */

/* Define if you have the <sys/audioio.h> header file.  */
/* #undef HAVE_SYS_AUDIOIO_H */

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/dirent.h> header file.  */
/* #undef HAVE_SYS_DIRENT_H */

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the SDL library (-lSDL).  */
/* #undef HAVE_LIBSDL */

/* Define if you have the UMSobj library (-lUMSobj).  */
/* #undef HAVE_LIBUMSOBJ */

/* Define if you have the audio library (-laudio).  */
/* #undef HAVE_LIBAUDIO */

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <io.h> header file.  */
#define HAVE_IO_H 1

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <signal.h> header file.  */
#define HAVE_SIGNAL_H 1

//#endif
