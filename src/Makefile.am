## Process this file with automake to create Makefile.in
AUTOMAKE_OPTIONS=no-dependencies

LN = ln

bin_PROGRAMS = grep egrep fgrep
base_sources = grep.c grep.h \
               dfa.c dfa.h \
               kwset.c  kwset.h \
               obstack.c obstack.h \
	       savedir.c savedir.h \
               getopt.c getopt1.c getopt.h \
               search.c getpagesize.h system.h
egrep_SOURCES = $(base_sources) egrepmat.c
fgrep_SOURCES = $(base_sources) fgrepmat.c
grep_SOURCES  = $(base_sources) grepmat.c

LDADD = @ALLOCA@ @LIBOBJS@ @INTLLIBS@
datadir = $(prefix)/@DATADIRNAME@
localedir = $(datadir)/locale
INCLUDES = -I../intl -DLOCALEDIR=\"$(localedir)\"

man_MANS = grep.1 fgrep.1 egrep.1

EXTRA_DIST = grep.1 egrep.man fgrep.man \
             regex.h \
             dosbuf.c \
             vms_fab.c   vms_fab.h

CLEANFILES = egrep.1 fgrep.1

fgrep.1: fgrep.man
	inst=`echo "grep" | sed '$(transform)'`.1; \
sed -e "s%@grep@%$$inst%g" $(srcdir)/fgrep.man > $@

egrep.1: egrep.man
	inst=`echo "grep" | sed '$(transform)'`.1; \
sed -e "s%@grep@%$$inst%g" $(srcdir)/egrep.man > $@
