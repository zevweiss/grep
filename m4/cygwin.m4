dnl Check for Cygwin.  This is a way to set the right value for
dnl EXEEXT.
AC_DEFUN(AC_CYGWIN,
[AC_CACHE_CHECK(for Cygwin environment, ac_cv_cygwin,
[AC_TRY_COMPILE(,[
#ifndef __CYGWIN__
#define __CYGWIN__ __CYGWIN32__
#endif
return __CYGWIN__;],
ac_cv_cygwin=yes, ac_cv_cygwin=no)
rm -f conftest*])
CYGWIN=
test "$ac_cv_cygwin" = yes && CYGWIN=yes])
