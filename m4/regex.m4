dnl Even packages that don't use regex.c can use this macro.
dnl Of course, for them it doesn't do anything.
dnl Derived from code in GNU grep.
dnl Derived from code in GNU fileutils ;-).

AC_DEFUN(AM_INCLUDED_REGEX,
[AC_REQUIRE([AM_GLIBC])
AC_CACHE_CHECK([whether compiling regex.c], ac_cv_included_regex,
[
# By default, don't use the included regex.c on systems with glibc 2
if test x"$ac_cv_glibc" = xyes ; then
	default=no
else
	default=yes
fi
	AC_ARG_WITH(included-regex,
	[  --without-included-regex don't compile regex; this is the default on
                          systems with version 2 of the GNU C library
                          (use with caution on other system)],
		    ac_cv_included_regex=$withval,
		    ac_cv_included_regex=$default)
if test x"$ac_cv_included_regex" = xyes; then
  LIBOBJS="$LIBOBJS regex.${ac_objext}"
fi
])])
