dnl Check for mingw32.  This is another way to set the right value for
dnl EXEEXT.
AC_DEFUN(AC_MINGW32,
[AC_CACHE_CHECK(for mingw32 environment, ac_cv_mingw32,
[AC_TRY_COMPILE(,[return __MINGW32__;],
ac_cv_mingw32=yes, ac_cv_mingw32=no)
rm -f conftest*])
MINGW32=
test "$ac_cv_mingw32" = yes && MINGW32=yes])
