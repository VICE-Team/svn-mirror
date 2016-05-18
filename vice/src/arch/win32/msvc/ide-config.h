#ifndef VICE_CONFIG_H_
#define VICE_CONFIG_H_

#define UNSTABLE

/* FIXME: Doesn't work with the rewrite. #define FEATURE_CPUMEMHISTORY 1*/
#define HAVE_WORKING_VSNPRINTF

#if defined _MSC_VER && _MSC_VER < 1400 
# define vsnprintf _vsnprintf
#endif

#pragma warning(disable:4996)

#define WIN32_COMPILE           1

#define HAVE_ALLOCA             1
#define RETSIGTYPE              void
#define HAVE_RESID              1
#define HAVE_RESID_DTV          1
#define HAS_JOYSTICK            1
#define HAVE_MOUSE              1
#define HAVE_LIGHTPEN           1
#define HAVE_CATWEASELMKIII     1
#define HAVE_HARDSID            1
#define HAVE_RS232DEV           1
#define HAVE_RS232NET           1
#define HAVE_DYNLIB_SUPPORT     1

#ifndef _M_IA64
#define HAVE_PARSID             1
#define HAVE_SSI2001            1
#endif

#define HAS_LONGLONG_INTEGER    1
#define HAS_UNLOCKRESOURCE      1
#define SIZEOF_UNSIGNED_INT     4
#define SIZEOF_UNSIGNED_LONG    4
#define SIZEOF_UNSIGNED_SHORT   2
#define HAVE_ATEXIT             1
#define HAVE_MEMMOVE            1
#define HAVE_STRERROR           1
#define HAVE_FCNTL_H            1
#define HAVE_LIMITS_H           1
#define HAVE_COMMCTRL_H         1
#define HAVE_SHLOBJ_H           1
#define HAVE_DIRECT_H           1
#define HAVE_DIRENT_H           1
#define HAVE_ERRNO_H            1
#define HAVE_IO_H               1
#define HAVE_PROCESS_H          1
#define HAVE_SYS_TYPES_H        1
#define HAVE_SYS_STAT_H         1
#define HAVE_SIGNAL_H           1
#define HAVE_WINIOCTL_H         1
#define HAVE_TIME_T_IN_TIME_H   1

#define HAVE_STRTOUL            1
#define HAVE_STRTOK_R			1
#define HAVE_STRREV			1
#define HAVE_STRLWR			1
#define HAVE_STRLCPY			1
#define HAVE_LTOA			1
#define HAVE_ULTOA			1
#define HAVE_VSNPRINTF		1
#define HAVE_SNPRINTF			1

#define DWORD_IS_LONG           1
#define HAVE_TFE                1

#ifndef _M_IA64
#define HAVE_FFMPEG             1
#define HAVE_FFMPEG_SWSCALE     1
#define HAVE_FFMPEG_HEADER_SUBDIRS 1
#endif

#ifndef _WIN64
#define FFMPEG_ALIGNMENT_HACK   1
#endif

#define HAVE_OPENCBM            1
#define HAVE_MIDI               1
#define HAVE_CRTDBG             1
#define HAS_TRANSLATION         1
#define HAVE_HTONL              1
#define HAVE_HTONS              1
#define HAVE_NETWORK            1
#define HAVE_GETCWD             1

#ifndef NODIRECTX
#define HAVE_DINPUT             1
#define USE_DXSOUND             1
#define HAVE_DSOUND_LIB         1

#if !defined(_M_IA64) && !defined(_M_ARM)
#define HAVE_GUIDLIB            1
#endif

#endif

#if !defined(_M_AMD64) && !defined(_M_IA64) && !defined(_M_ARM)
#define __i386__                1
#endif

#ifdef _M_AMD64
#define __amd64__               1
#endif

#define inline                  _inline

#define _ANONYMOUS_UNION

#define S_ISDIR(m)              ((m) & _S_IFDIR)

#define MSVC_RC                 1

#define strcasecmp(s1, s2)      _stricmp(s1, s2)
#define HAVE_STRCASECMP         1

#if defined _MSC_VER && _MSC_VER < 1900 
#define snprintf _snprintf
#endif

/* begin: for FFMPEG: common.h */
#define CONFIG_WIN32

#define int64_t_C(c)     (c ## i64)
#define uint64_t_C(c)    (c ## ui64)
/* end: for FFMPEG: common.h */

#endif
