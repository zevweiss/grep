## Process this file with automake to create Makefile.in
AUTOMAKE_OPTIONS = ansi2knr no-dependencies

LN = ln

# We let configure figure out whether we need
# regex.c, strtoul.c, strtoull.c, and strtoumax.c.
bin_PROGRAMS = grep egrep fgrep
base_sources = grep.c grep.h \
               dfa.c dfa.h \
               kwset.c  kwset.h \
               search.c getpagesize.h system.h
egrep_SOURCES = $(base_sources) egrepmat.c
fgrep_SOURCES = $(base_sources) fgrepmat.c
grep_SOURCES  = $(base_sources) grepmat.c

LDADD = @INTLLIBS@ ../lib/libgreputils.a
datadir = $(prefix)/@DATADIRNAME@
localedir = $(datadir)/locale
INCLUDES = -I../intl -I$(top_srcdir)/lib -DLOCALEDIR=\"$(localedir)\"

EXTRA_DIST = \
             dosbuf.c \
             vms_fab.c vms_fab.h
