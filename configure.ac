dnl
dnl autoconf input file for GNU grep
dnl
dnl Copyright (C) 1997-2006, 2009-2020 Free Software Foundation, Inc.
dnl
dnl This file is part of GNU grep.
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3, or (at your option)
dnl any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <https://www.gnu.org/licenses/>.

AC_INIT([GNU grep],
        m4_esyscmd([build-aux/git-version-gen .tarball-version]),
        [bug-grep@gnu.org])

if test -n "$GREP" || test -n "$EGREP"; then
  AC_MSG_ERROR(
    [no working 'grep' found
  A working 'grep' command is needed to build GNU Grep.
  This 'grep' should support -e and long lines.
  On Solaris 10, install the package SUNWggrp or SUNWxcu4.
  On Solaris 11, install the package text/gnu-grep or system/xopen/xcu4.])
fi

AC_CONFIG_AUX_DIR(build-aux)
AC_CONFIG_SRCDIR(src/grep.c)
AC_DEFINE([GREP], 1, [We are building grep])
AC_PREREQ([2.63])
AC_CONFIG_MACRO_DIRS([m4])

dnl Automake stuff.
AM_INIT_AUTOMAKE([1.11 no-dist-gzip dist-xz color-tests parallel-tests
                  subdir-objects])
AM_SILENT_RULES([yes]) # make --enable-silent-rules the default.

AC_CONFIG_HEADERS([config.h:config.hin])

dnl Checks for programs.
AC_CANONICAL_HOST
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_CC
gl_EARLY
AC_PROG_RANLIB
PKG_PROG_PKG_CONFIG([0.9.0])

# grep never invokes mbrtowc or mbrlen on empty input,
# so don't worry about this common bug,
# as working around it would merely slow grep down.
gl_cv_func_mbrtowc_empty_input='assume yes'

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_CONST
gl_INIT

# Ensure VLAs are not used.
# Note -Wvla is implicitly added by gl_MANYWARN_ALL_GCC
AC_DEFINE([GNULIB_NO_VLA], [1], [Define to 1 to disable use of VLAs])

# The test suite needs to know if we have a working perl.
# FIXME: this is suboptimal.  Ideally, we would be able to call gl_PERL
# with an ACTION-IF-NOT-FOUND argument ...
cu_have_perl=yes
case $PERL in *"/missing "*) cu_have_perl=no;; esac
AM_CONDITIONAL([HAVE_PERL], [test $cu_have_perl = yes])

AC_ARG_ENABLE([gcc-warnings],
  [AS_HELP_STRING([--enable-gcc-warnings],
                  [turn on lots of GCC warnings (for developers)])],
  [case $enableval in
     yes|no) ;;
     *)      AC_MSG_ERROR([bad value $enableval for gcc-warnings option]) ;;
   esac
   gl_gcc_warnings=$enableval],
  [gl_gcc_warnings=no
   if test "$GCC" = yes && test -d "$srcdir"/.git; then
     AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([[
          #if ! (6 < __GNUC__ + (2 <= __GNUC_MINOR__))
            #error "--enable-gcc-warnings defaults to 'no' on older GCC"
          #endif
          ]])],
       [gl_gcc_warnings=yes])
   fi]
)

if test "$gl_gcc_warnings" = yes; then
  gl_WARN_ADD([-Werror], [WERROR_CFLAGS])
  AC_SUBST([WERROR_CFLAGS])

  nw=
  # This, $nw, is the list of warnings we disable.
  nw="$nw -Wdeclaration-after-statement" # too useful to forbid
  nw="$nw -Waggregate-return"       # anachronistic
  nw="$nw -Wlong-long"              # C90 is anachronistic (lib/gethrxtime.h)
  nw="$nw -Wc++-compat"             # We don't care about C++ compilers
  nw="$nw -Wundef"                  # Warns on '#if GNULIB_FOO' etc in gnulib
  nw="$nw -Wsystem-headers"         # Don't let system headers trigger warnings
  nw="$nw -Wpadded"                 # Our structs are not padded
  nw="$nw -Wstack-protector"        # generates false alarms for useful code
  nw="$nw -Wswitch-default"         # Too many warnings for now
  nw="$nw -Wunsafe-loop-optimizations" # OK to suppress unsafe optimizations
  nw="$nw -Winline"                 # streq.h's streq4, streq6 and strcaseeq6
  nw="$nw -Wstrict-overflow"        # regexec.c

  gl_MANYWARN_ALL_GCC([ws])
  gl_MANYWARN_COMPLEMENT([ws], [$ws], [$nw])
  for w in $ws; do
    gl_WARN_ADD([$w])
  done
  gl_WARN_ADD([-Wno-missing-field-initializers]) # We need this one
  gl_WARN_ADD([-Wno-sign-compare])     # Too many warnings for now
  gl_WARN_ADD([-Wno-unused-parameter]) # Too many warnings for now
  gl_WARN_ADD([-Wno-cast-function-type]) # sig-handler.h's sa_handler_t cast

  # In spite of excluding -Wlogical-op above, it is enabled, as of
  # gcc 4.5.0 20090517, and it provokes warnings in cat.c, dd.c, truncate.c
  gl_WARN_ADD([-Wno-logical-op])

  AC_SUBST([WARN_CFLAGS])

  AC_DEFINE([lint], [1], [Define to 1 if the compiler is checking for lint.])
  AC_DEFINE([GNULIB_PORTCHECK], [1], [enable some gnulib portability checks])
  AH_VERBATIM([GNULIB_PORTCHECK_FORTIFY_SOURCE],
  [/* Enable compile-time and run-time bounds-checking, and some warnings,
      without upsetting glibc 2.15+. */
   #if (defined GNULIB_PORTCHECK && !defined _FORTIFY_SOURCE \
        && defined __OPTIMIZE__ && __OPTIMIZE__)
   # define _FORTIFY_SOURCE 2
   #endif
  ])

  # We use a slightly smaller set of warning options for lib/.
  # Remove the following and save the result in GNULIB_WARN_CFLAGS.
  nw=
  nw="$nw -Wunused-macros"
  gl_WARN_ADD([-Wno-format-nonliteral])
  gl_MANYWARN_COMPLEMENT([GNULIB_WARN_CFLAGS], [$WARN_CFLAGS], [$nw])
  AC_SUBST([GNULIB_WARN_CFLAGS])
fi

# By default, argmatch should fail calling usage (EXIT_FAILURE).
AC_DEFINE([ARGMATCH_DIE], [usage (EXIT_FAILURE)],
          [Define to the function xargmatch calls on failures.])
AC_DEFINE([ARGMATCH_DIE_DECL], [void usage (int _e)],
          [Define to the declaration of the xargmatch failure function.])

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT

dnl Checks for functions.
AC_FUNC_CLOSEDIR_VOID

AC_CHECK_FUNCS_ONCE(isascii setlocale)

dnl I18N feature
AM_GNU_GETTEXT_VERSION([0.18.2])
AM_GNU_GETTEXT([external])

dnl Some installers want to be informed if we do not use our regex.
dnl For example, if the host platform uses dynamic linking and the installer
dnl knows that the grep may be invoked on other hosts with buggy libraries,
dnl then the installer should configure --with-included-regex.
AM_CONDITIONAL([USE_INCLUDED_REGEX], [test "$ac_use_included_regex" = yes])
if test "$ac_use_included_regex" = no; then
  AC_MSG_WARN([Included lib/regex.c not used])
fi

gl_FUNC_PCRE
AM_CONDITIONAL([USE_PCRE], [test $use_pcre = yes])

case $host_os in
  mingw*) suffix=w32 ;;
  *) suffix=posix ;;
esac
COLORIZE_SOURCE=colorize-$suffix.c
AC_SUBST([COLORIZE_SOURCE])

AC_CONFIG_FILES([
  Makefile
  lib/Makefile
  src/Makefile
  tests/Makefile
  po/Makefile.in
  doc/Makefile
  gnulib-tests/Makefile
])
AC_OUTPUT
