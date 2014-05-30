## Process this file with automake to create Makefile.in
# Copyright 1997-1998, 2005-2014 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

check_PROGRAMS = get-mb-cur-max
AM_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib
AM_CFLAGS = $(WARN_CFLAGS) $(WERROR_CFLAGS)

# Tell the linker to omit references to unused shared libraries.
AM_LDFLAGS = $(IGNORE_UNUSED_LIBRARIES_CFLAGS)
LDADD = ../lib/libgreputils.a $(LIBINTL) ../lib/libgreputils.a

XFAIL_TESTS =

# Equivalence classes are only supported when using the system
# matcher (which means only with glibc).
if USE_INCLUDED_REGEX
XFAIL_TESTS += equiv-classes
endif

TESTS =						\
  backref					\
  backref-multibyte-slow			\
  backref-word					\
  backslash-s-and-repetition-operators		\
  backslash-s-vs-invalid-multitype		\
  big-hole					\
  big-match					\
  bogus-wctob					\
  bre						\
  case-fold-backref				\
  case-fold-backslash-w				\
  case-fold-char-class				\
  case-fold-char-range				\
  case-fold-char-type				\
  case-fold-titlecase				\
  char-class-multibyte				\
  char-class-multibyte2				\
  context-0					\
  count-newline					\
  dfa-coverage					\
  dfa-heap-overrun				\
  dfaexec-multibyte				\
  empty						\
  empty-line					\
  epipe						\
  equiv-classes					\
  ere						\
  euc-mb					\
  fedora					\
  fgrep-infloop					\
  file						\
  fmbtest					\
  foad1						\
  grep-dev-null					\
  grep-dir					\
  help-version					\
  in-eq-out-infloop				\
  include-exclude				\
  inconsistent-range				\
  invalid-multibyte-infloop			\
  khadafy					\
  long-line-vs-2GiB-read			\
  max-count-overread				\
  max-count-vs-context				\
  mb-non-UTF8-performance			\
  multibyte-white-space				\
  multiple-begin-or-end-line			\
  null-byte					\
  empty-line-mb					\
  unibyte-bracket-expr				\
  unibyte-negated-circumflex			\
  high-bit-range				\
  options					\
  pcre						\
  pcre-abort					\
  pcre-infloop					\
  pcre-invalid-utf8-input			\
  pcre-utf8					\
  pcre-w					\
  pcre-wx-backref				\
  pcre-z					\
  posix-bracket					\
  prefix-of-multibyte				\
  r-dot						\
  repetition-overflow				\
  reversed-range-endpoints			\
  skip-device					\
  sjis-mb					\
  spencer1					\
  spencer1-locale				\
  status					\
  surrogate-pair				\
  symlink					\
  turkish-eyes					\
  turkish-I					\
  turkish-I-without-dot				\
  warn-char-classes				\
  word-delim-multibyte				\
  word-multi-file				\
  yesno

EXTRA_DIST =					\
  $(TESTS)					\
  bre.awk					\
  bre.tests					\
  ere.awk					\
  ere.tests					\
  init.cfg					\
  init.sh					\
  khadafy.lines					\
  khadafy.regexp				\
  spencer1.awk					\
  spencer1.tests				\
  spencer1-locale.awk

# Default to a nonzero value.  Environment overrides.
# http://udrepper.livejournal.com/11429.html
MALLOC_PERTURB_ = 1

TESTS_ENVIRONMENT =				\
  tmp__=$$TMPDIR; test -d "$$tmp__" || tmp__=.;	\
  TMPDIR=$$tmp__; export TMPDIR;		\
						\
  if test -n "$$BASH_VERSION" || (eval "export v=x") 2>/dev/null; then \
    export_with_values () { export "$$@"; };		\
  else							\
    export_with_values ()				\
    {							\
      sed_extract_var='s/=.*//';			\
      sed_quote_value="s/'/'\\\\''/g;s/=\\(.*\\)/='\\1'/";\
      for arg in "$$@"; do				\
        var=`echo "$$arg" | sed "$$sed_extract_var"`;	\
        arg=`echo "$$arg" | sed "$$sed_quote_value"`;	\
        eval "$$arg";					\
        export "$$var";					\
      done;						\
    };							\
  fi;							\
						\
  export_with_values				\
  VERSION='$(VERSION)'				\
  LOCALE_FR='$(LOCALE_FR)'			\
  LOCALE_FR_UTF8='$(LOCALE_FR_UTF8)'		\
  AWK=$(AWK)					\
  GREP_OPTIONS=''				\
  LC_ALL=C					\
  abs_top_builddir='$(abs_top_builddir)'	\
  abs_top_srcdir='$(abs_top_srcdir)'		\
  abs_srcdir='$(abs_srcdir)'			\
  built_programs='grep egrep fgrep'		\
  srcdir='$(srcdir)'				\
  top_srcdir='$(top_srcdir)'			\
  CC='$(CC)'					\
  GREP_TEST_NAME=`echo $$tst|sed 's,^\./,,;s,/,-,g'` \
  MAKE=$(MAKE)					\
  MALLOC_PERTURB_=$(MALLOC_PERTURB_)		\
  PACKAGE_BUGREPORT='$(PACKAGE_BUGREPORT)'	\
  PACKAGE_VERSION=$(PACKAGE_VERSION)		\
  PERL='$(PERL)'				\
  SHELL='$(SHELL)'				\
  PATH='$(abs_top_builddir)/src$(PATH_SEPARATOR)'"$$PATH" \
  ; 9>&2

LOG_COMPILER = $(SHELL)

VERBOSE = yes

check: check_executable_TESTS
.PHONY: check_executable_TESTS
check_executable_TESTS: Makefile
	$(AM_V_GEN)fail=0;						\
        cd $(srcdir) && for i in $(TESTS); do				\
	  test -x $$i || { fail=1; echo $$i >&2; };			\
	done;								\
	test $$fail = 1							\
          && { echo the above test scripts are not executable >&2; exit 1; } \
          || :
