#serial 4

dnl Initially derived from code in GNU grep.
dnl Mostly written by Jim Meyering.

AC_DEFUN(jm_INCLUDED_REGEX,
  [
    dnl Even packages that don't use regex.c can use this macro.
    dnl Of course, for them it doesn't do anything.

    # Normally we should skip the test for GLIBC but a bug
    # in the older regex will not be detected so test for everyone.
    # The failing regular expression is from `Spencer ere test
    # #75' in grep-2.2f.
    dnl AC_REQUIRE([AM_GLIBC])

    ac_use_included_regex=yes

    # Without this run-test, on older glibc2 systems we'd end up
    # using the buggy system regex.
    AC_CACHE_CHECK([for working re_compile_pattern],
                   jm_cv_func_working_re_compile_pattern,
	AC_TRY_RUN(
	  changequote(<<, >>)dnl
	  <<
#include <stdio.h>
#include <regex.h>
	    int
	    main ()
	    {
	      static struct re_pattern_buffer regex;
	      const char *s;
	      re_set_syntax (RE_SYNTAX_POSIX_EGREP);
	      /* Add this third left square bracket, XX, to balance the
		 three right ones below.  Otherwise autoconf-2.14 chokes.  */
	      s = re_compile_pattern ("a[[:XX:]]b\n", 9, &regex);
	      /* This should fail with _Invalid character class name_ error.  */
	      exit (s ? 0 : 1);
	    }
	  >>,
	  changequote([, ])dnl

	 jm_cv_func_working_re_compile_pattern=yes,
	 jm_cv_func_working_re_compile_pattern=no,
	 dnl When crosscompiling, assume it's broken.
	 jm_cv_func_working_re_compile_pattern=no))
    if test "$jm_cv_func_working_re_compile_pattern" = yes; then
      ac_use_included_regex=no
     fi

    AC_SUBST(LIBOJS)dnl
    test -n "$1" || AC_MSG_ERROR([missing argument])
    syscmd([test -f $1])
    ifelse(sysval, 0,
      [

	AC_ARG_WITH(included-regex,
	[  --without-included-regex don't compile regex; this is the default on
                          systems with version 2 of the GNU C library
                          (use with caution on other system)],
		    jm_with_regex=$withval,
		    jm_with_regex=$ac_use_included_regex)
	if test "$jm_with_regex" = yes; then
	  LIBOBJS="$LIBOBJS regex.${ac_objext}"
	fi
      ],
    )
  ]
)
