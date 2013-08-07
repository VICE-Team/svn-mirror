#ifndef VICE_CONFIG_H_
#define VICE_CONFIG_H_

#define UNSTABLE

#pragma warning(disable:4996)

#define HAVE_ALLOCA            1
#define RETSIGTYPE             void
#define HAVE_RESID             1
#define HAVE_RESID_DTV         1
#define HAS_JOYSTICK           1
#define HAVE_MOUSE             1
#define HAVE_NETWORK           1

#define USE_SDLUI              1
#define USE_SDL_PREFIX         1
#define USE_SDL_AUDIO          1
#define WIN32_COMPILE          1
#define HAVE_SDLMAIN           1
#define HAVE_SDL_NUMJOYSTICKS  1
#define HAVE_HWSCALE           1
#define HAVE_CATWEASELMKIII    1
#define HAVE_HARDSID           1
#define HAVE_OPENCBM           1
#define HAVE_DYNLIB_SUPPORT    1
#define HAVE_RS232             1

#define HAS_LONGLONG_INTEGER   1
#define HAS_UNLOCKRESOURCE     1
#define SIZEOF_UNSIGNED_INT    4
#define SIZEOF_UNSIGNED_LONG   4
#define SIZEOF_UNSIGNED_SHORT  2
#define HAVE_ATEXIT            1
#define HAVE_MEMMOVE           1
#define HAVE_STRERROR          1
#define HAVE_FCNTL_H           1
#define HAVE_LIMITS_H          1
#define HAVE_COMMCTRL_H        1
#define HAVE_SHLOBJ_H          1
#define HAVE_DIRECT_H          1
#define HAVE_DIRENT_H          1
#define HAVE_ERRNO_H           1
#define HAVE_IO_H              1
#define HAVE_PROCESS_H         1
#define HAVE_SYS_TYPES_H       1
#define HAVE_SYS_STAT_H        1
#define HAVE_SIGNAL_H          1
#define HAVE_WINIOCTL_H        1

#define DWORD_IS_LONG          1
#define HAVE_CRTDBG            1
#define HAVE_HTONL             1
#define HAVE_HTONS             1
#define HAVE_GETCWD            1

#define HAVE_TFE                1
#define HAVE_FFMPEG             1
#define HAVE_FFMPEG_SWSCALE     1
#define HAVE_FFMPEG_HEADER_SUBDIRS 1

#define HAVE_STRDUP            1
#define HAVE_WORKING_VSNPRINTF 1

#define __i386__               1

#define inline                 _inline

#define _ANONYMOUS_UNION

#define S_ISDIR(m)             ((m) & _S_IFDIR)

#define MSVC_RC                1

#define strcasecmp(s1, s2)     _stricmp(s1, s2)
#define HAVE_STRCASECMP        1

#define snprintf               _snprintf

#define int64_t_C(c)           (c ## i64)
#define uint64_t_C(c)          (c ## u64)

#if defined _MSC_VER && _MSC_VER < 1400 
#define vsnprintf              _vsnprintf
#endif

#define CONFIG_WIN32

/* Set compiler version */
#if (_MSC_VER == 1100)
#define PLATFORM_COMPILER "msvc5/vs97"
#endif

#if (_MSC_VER == 1200)
#define PLATFORM_COMPILER "msvc6/vs98"
#endif

#if (_MSC_VER == 1300)
#define PLATFORM_COMPILER "msvc7.0/vs2002"
#endif

#if (_MSC_VER == 1310)
#define PLATFORM_COMPILER "msvc7.1/vs2003"
#endif

#if (_MSC_VER == 1400)
#define PLATFORM_COMPILER "msvc8/vs2005"
#endif

#if (_MSC_VER == 1500)
#define PLATFORM_COMPILER "msvc9/vs2008"
#endif

#if (_MSC_VER == 1600)
#define PLATFORM_COMPILER "msvc10/vs2010"
#endif

#if (_MSC_VER == 1700)
#define PLATFORM_COMPILER "msvc11/vs2012"
#endif

#ifndef PLATFORM_COMPILER
#define PLATFORM_COMPILER "msvc"
#endif

#ifdef _M_IA64
#define PLATFORM "win64 ia64 msvc"
#define PLATFORM_OS "win64"
#define PLATFORM_CPU "ia64"
#else
#  ifdef _WIN64
#    define PLATFORM "win64 x64 msvc"
#    define PLATFORM_OS "win64"
#    define PLATFORM_CPU "x64"
#  else
#    define PLATFORM "win32 x86 msvc"
#    define PLATFORM_OS "win32"
#    define PLATFORM_CPU "x86"
#  endif
#endif

#endif
