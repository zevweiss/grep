## Process this file with automake to create Makefile.in
AUTOMAKE_OPTIONS = ansi2knr

LN = ln

bin_PROGRAMS = grep egrep fgrep
grep_SOURCES  =  grep.c  search.c kwset.c dfa.c
egrep_SOURCES = egrep.c esearch.c kwset.c dfa.c
fgrep_SOURCES = fgrep.c fsearch.c kwset.c
noinst_HEADERS = grep.h dfa.h kwset.h getpagesize.h system.h mbsupport.h

LDADD = @INTLLIBS@ ../lib/libgreputils.a
datadir = $(prefix)/@DATADIRNAME@
localedir = $(datadir)/locale
INCLUDES = -I../intl -I$(top_srcdir)/lib -DLOCALEDIR=\"$(localedir)\"

EXTRA_DIST = \
             dosbuf.c \
             vms_fab.c vms_fab.h
