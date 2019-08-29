dnl vim: set et ts=4 sw=4 sts=4:
dnl
dnl \brief  Compiler check macros for VICE
dnl
dnl A few macros to simplify checking compiler and linker features.
dnl Incomplete and unused at the moment for 'code review'. The idea is to add
dnl VICE_CXXFLAGS_[REQUEST|REQUIRE]() as well, and perhaps linker checks,
dnl should we require those.
dnl
dnl \author Bas Wassink <b.wassink@ziggo.nl>


dnl VICE_CFLAG_REQUEST(flag)
dnl
dnl Check if the compiler supports `flag` and add it when suppported
dnl
AC_DEFUN([VICE_CFLAG_REQUEST],
[
    AC_LANG_PUSH([C])
    AC_MSG_CHECKING([if the C compiler supports $1])
    old_CFLAGS="$CFLAGS"
    dnl We need -Werror to make AC_TRY_COMPILE error out with Clang
    dnl We need -Wno-strict-prototypes to avoid barfing on 'main()'
    CFLAGS="$VICE_CFLAGS $1 -Wno-strict-prototypes -Werror"
    AC_TRY_COMPILE(
        [],
        [int poop = 42; return poop;],
        [AC_MSG_RESULT([yes])
         VICE_CFLAGS="$VICE_CFLAGS $1"],
        [AC_MSG_RESULT([no])]
    )
    CFLAGS="$old_CFLAGS"
    AC_LANG_POP([C])
])


dnl VICE_CFLAG_REQUIRE(flag)
dnl
dnl Check if the compiler supports `flag` and error out when not suppported
dnl
AC_DEFUN([VICE_CFLAG_REQUIRE],
[
    AC_LANG_PUSH([C])
    AC_MSG_CHECKING([if the C compiler supports $1])
    old_CFLAGS="$CFLAGS"
    dnl We need -Werror to make AC_TRY_COMPILE error out with Clang
    CFLAGS="$VICE_CFLAGS $1 -Wno-strict-prototypes -Werror"
    AC_TRY_COMPILE(
        [],
        [int poop = 42; return poop;],
        [AC_MSG_RESULT([yes])
         VICE_CFLAGS="$VICE_CFLAGS $1"],
        [AC_MSG_ERROR([no, $1 is required])]
    )
    CFLAGS="$old_CFLAGS"
    AC_LANG_POP([C])
])

