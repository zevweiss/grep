## Process this file with automake to create Makefile.in
AUTOMAKE_OPTIONS = ansi2knr no-dependencies

LN = ln

bin_PROGRAMS = grep
bin_SCRIPTS = egrep fgrep
CLEANFILES = egrep fgrep
grep_SOURCES = grep.c dfa.c kwset.c search.c \
               grepmat.c
noinst_HEADERS = grep.h dfa.h kwset.h getpagesize.h system.h mbsupport.h

LDADD = @INTLLIBS@ ../lib/libgreputils.a
datadir = $(prefix)/@DATADIRNAME@
localedir = $(datadir)/locale
INCLUDES = -I../intl -I$(top_srcdir)/lib -DLOCALEDIR=\"$(localedir)\"

EXTRA_DIST = \
             dosbuf.c \
             vms_fab.c vms_fab.h

OPTION_for_egrep = -E
OPTION_for_fgrep = -F

egrep fgrep: Makefile
	echo '#!/bin/sh' >$@
	echo 'exec grep $(OPTION_for_$@) $${1+"$$@"}' >>$@
	chmod a+x $@
