## Process this file with automake to create Makefile.in
# Copyright 1997-1998, 2005-2010 Free Software Foundation, Inc.
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

TESTS =						\
  backref.sh					\
  backref-word					\
  bre.sh					\
  case-fold-backref				\
  case-fold-backslash-w				\
  case-fold-char-class				\
  case-fold-char-range				\
  case-fold-char-type				\
  char-class-multibyte				\
  dfaexec-multibyte				\
  empty						\
  ere.sh					\
  euc-mb					\
  fedora					\
  fgrep-infloop					\
  file.sh					\
  fmbtest					\
  foad1						\
  help-version					\
  include-exclude				\
  khadafy.sh					\
  max-count-vs-context				\
  options.sh					\
  pcre.sh					\
  pcre-z					\
  reversed-range-endpoints			\
  sjis-mb					\
  spencer1.sh					\
  spencer1-locale				\
  status.sh					\
  prefix-of-multibyte				\
  warning.sh					\
  word-multi-file				\
  yesno.sh

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

CLEANFILES = \
  spencer1.script bre.script ere.script khadafy.out patfile \
  cspatfile csinput yesno.txt 112869.out 116909.in 116909.list \
  116909.out 123362.out 161700.out 171379.out 179698.out \
  204255.first 204255.second

TESTS_ENVIRONMENT =				\
  tmp__=$$TMPDIR; test -d "$$tmp__" || tmp__=.;	\
  TMPDIR=$$tmp__; export TMPDIR;		\
  exec 9>&2;					\
  shell_or_perl_() {				\
    if grep '^\#!/usr/bin/perl' "$$1" > /dev/null; then			\
      if $(PERL) -e 'use warnings' > /dev/null 2>&1; then		\
	grep '^\#!/usr/bin/perl -T' "$$1" > /dev/null && T_=T || T_=;	\
        $(PERL) -w$$T_ -I$(srcdir) -MCoreutils				\
	      -M"CuTmpdir qw($$f)" -- "$$1";	\
      else					\
	echo 1>&2 "$$tst: configure did not find a usable version of Perl," \
	  "so skipping this test";		\
	(exit 77);				\
      fi;					\
    else					\
      $(SHELL) "$$1";				\
    fi;						\
  };						\
  export					\
  LOCALE_FR='$(LOCALE_FR)'                      \
  LOCALE_FR_UTF8='$(LOCALE_FR_UTF8)'            \
  AWK=$(AWK)					\
  GREP=$(top_builddir)/src/grep			\
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
  PACKAGE_BUGREPORT='$(PACKAGE_BUGREPORT)'	\
  PACKAGE_VERSION=$(PACKAGE_VERSION)		\
  PERL='$(PERL)'				\
  SHELL='$(SHELL)'				\
  PATH='$(abs_top_builddir)/src$(PATH_SEPARATOR)'"$$PATH" \
  ; shell_or_perl_

VERBOSE = yes
