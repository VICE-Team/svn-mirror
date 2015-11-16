#ifndef VICE_CONFIG_H_
#define VICE_CONFIG_H_

#define UNSTABLE

#pragma warning 202 10
#pragma warning 201 10
#pragma enum int

#define HAVE_ALLOCA           1
#define RETSIGTYPE            void
#define HAS_JOYSTICK          1
#define HAVE_MOUSE            1
#define HAVE_NETWORK          1
#define HAVE_RS232DEV         1
#define HAVE_RS232NET         1

#define HAVE_RESID             1
#define HAVE_RESID_FP          1
#define HAVE_RESID_DTV         1

#define USE_SDLUI             1
#define WIN32_COMPILE         1
#define HAVE_SDLMAIN          1
#define HAVE_HWSCALE          1
#define HAVE_CATWEASELMKIII   1
#define HAVE_HARDSID          1
#define HAVE_OPENCBM          1
#define HAVE_DYNLIB_SUPPORT   1
#define HAVE_SDL_NUMJOYSTICKS 1
#define USE_SDL_PREFIX        1
#define HAVE_TFE              1

#define HAS_LONGLONG_INTEGER  1
#define HAS_UNLOCKRESOURCE    1
#define SIZEOF_UNSIGNED_INT   4
#define SIZEOF_UNSIGNED_LONG  4
#define SIZEOF_UNSIGNED_SHORT 2
#define HAVE_ATEXIT           1
#define HAVE_MEMMOVE          1
#define HAVE_STRERROR         1
#define HAVE_FCNTL_H          1
#define HAVE_LIMITS_H         1
#define HAVE_COMMCTRL_H       1
#define HAVE_SHLOBJ_H         1
#define HAVE_DIRECT_H         1
#define HAVE_ERRNO_H          1
#define HAVE_IO_H             1
#define HAVE_PROCESS_H        1
#define HAVE_SYS_TYPES_H      1
#define HAVE_SYS_STAT_H       1
#define HAVE_SIGNAL_H         1
#define HAVE_WINIOCTL_H       1

#define HAVE_STRTOK_R			1
#define HAVE_STRREV			1
#define HAVE_STRLWR			1
#define HAVE_STRLCPY			1
#define HAVE_LTOA			1
#define HAVE_ULTOA			1
#define HAVE_VSNPRINTF		1
#define HAVE_SNPRINTF			1

#define DWORD_IS_LONG         1
#define HAVE_CRTDBG           1
#define HAVE_HTONL            1
#define HAVE_HTONS            1
#define HAVE_GETCWD           1

#define __i386__              1

#define WATCOM_COMPILE        1
#define HAVE_STRNCASECMP      1

#define MSVC_RC               1
#define _MSC_VER              1

#define strcasecmp(s1, s2)    _stricmp(s1, s2)
#define HAVE_STRCASECMP       1

#define snprintf              _snprintf

#define int64_t_C(c)          (c ## i64)
#define uint64_t_C(c)         (c ## u64)

#define _ANONYMOUS_UNION

#define inline

#define CONFIG_WIN32

#endif
