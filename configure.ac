dnl
dnl autoconf input file for ``GNU grep''
dnl
dnl Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004,
dnl 2005, 2006, 2009 Free Software Foundation, Inc.
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
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT(GNU grep, 2.6-cvs, bug-grep@gnu.org, grep)
AC_CONFIG_AUX_DIR(build-aux)
AC_CONFIG_SRCDIR(src/grep.c)
AC_DEFINE(GREP, 1, [We are building grep])
AC_PREREQ(2.59)

dnl Automake stuff.
AM_INIT_AUTOMAKE(dist-bzip2)
AM_CONFIG_HEADER(config.h:config.hin)

dnl Check for arguments
AC_ARG_ENABLE(perl-regexp,
 [  --disable-perl-regexp           disable perl-regexp],
 [case "${enableval}" in
  yes) testpcre=yes ;;
  no)  testpcre=no ;;
  *)   AC_MSG_ERROR(bad value ${enableval} for --disable-perl-regexp) ;;
 esac],[testpcre=yes])

dnl Checks for programs.
AC_CANONICAL_HOST
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_CC
gl_EARLY
AC_PROG_RANLIB

dnl Checks for typedefs, structures, and compiler characteristics.
AC_SYS_LARGEFILE
AM_C_PROTOTYPES
AC_TYPE_SIZE_T
AC_CHECK_TYPE(ssize_t, int)
AC_C_CONST
gl_INIT

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(stddef.h string.h stdlib.h sys/param.h memory.h unistd.h locale.h limits.h wctype.h wchar.h)
AC_HEADER_DIRENT
AC_HEADER_STAT

dnl Checks for functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MMAP

dnl getpagesize is checked for by AC_FUNC_MMAP.
AC_CHECK_FUNCS(btowc isascii iswctype mbrlen memmove mempcpy setlocale setmode wcrtomb wcscoll wctype)
# We can use mbrtowc only if we have mbstate_t.
AC_FUNC_MBRTOWC
# Yet we need a mbstate_t stub in every case.
AC_MBSTATE_T

dnl I18N feature
AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])

dnl DOS file name convention
dnl sets HAVE_DOS_FILE_NAMES
AC_DOSFILE

dnl some folks ask for this, that's fine by me
dnl hope they know what they're doing ...
dnl if glibc2 regex is not included

dnl Many GNU/Linux people have different
dnl glibc versions with buggy regex.
jm_INCLUDED_REGEX(lib/regex.c)

dnl Some installers want to be informed if we do not use our regex.
dnl For example, if the host platform uses dynamic linking and the installer
dnl knows that the grep may be invoked on other hosts with buggy libraries,
dnl then the installer should configure --with-included-regex.
if test "$jm_with_regex" = no; then
	AC_MSG_WARN(Included lib/regex.c not used)
fi

# support for pcre
if test x"$testpcre" = x"yes"; then
	if pcre-config --cflags >/dev/null 2>&1; then
		CPPFLAGS="$CPPFLAGS `pcre-config --cflags`"
		LIBS="$LIBS `pcre-config --libs`"
	fi
	AC_CHECK_LIB(pcre, pcre_exec)
fi

AC_CONFIG_FILES([Makefile
				 lib/Makefile
				 src/Makefile
				 tests/Makefile
				 po/Makefile.in
				 doc/Makefile
				 vms/Makefile])
AC_OUTPUT
