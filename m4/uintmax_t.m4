#serial 5

dnl From Paul Eggert.


# Define uintmax_t to `unsigned long' or `unsigned long long'
# if <inttypes.h> does not exist.

AC_DEFUN([jm_AC_TYPE_UINTMAX_T],
[AC_PREREQ(2.13)dnl
  AC_REQUIRE([jm_AC_HEADER_INTTYPES_H])dnl
  if test $jm_ac_cv_header_inttypes_h = no; then
    AC_REQUIRE([jm_AC_TYPE_UNSIGNED_LONG_LONG])
    test $ac_cv_type_unsigned_long_long = yes \
      && ac_type='unsigned long long' \
      || ac_type='unsigned long'
    AC_DEFINE_UNQUOTED(uintmax_t, $ac_type,
[  Define to unsigned long or unsigned long long
   if <inttypes.h> doesn't define.])
  fi
])
