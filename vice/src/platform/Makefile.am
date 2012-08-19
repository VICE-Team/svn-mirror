INCLUDES = \
	@ARCH_INCLUDES@ \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src

noinst_LIBRARIES = libplatform.a

libplatform_a_SOURCES = \
	platform.c \
	platform.h \
	platform_aix_version.h \
	platform_amigaos3_runtime_os.c \
	platform_amigaos4_runtime_os.c \
	platform_compiler.h \
	platform_cpu_type.h \
	platform_discovery.h \
	platform_freebsd_version.h \
	platform_linux_libc_version.h \
	platform_netbsd_version.h \
	platform_openbsd_version.h \
	platform_qnx6_version.h \
	platform_windows_runtime_os.c \
	platform_x86_runtime_cpu.c
