# Check to see the separator for the environment variables
# and set SEP to ";" or default ":"

dnl AM_SEP()
dnl SEP
AC_DEFUN(AM_SEP,
[AC_REQUIRE([AC_CYGWIN])
AC_REQUIRE([AC_MINGW32])
AC_REQUIRE([AC_DJGPP])
AC_MSG_CHECKING([for environ var separator])
AC_CACHE_VAL(ac_cv_sep,
[if test "$CYGWIN" = yes || test "$MINGW32" = yes || "$DJGPP" = yes ; then
  ac_cv_sep=yes
else
  ac_cv_sep=no
fi])
SEP=":"
test "$ac_cv_sep" = yes && SEP=";"
AC_MSG_RESULT(${ac_cv_sep})
AC_SUBST(SEP)])
