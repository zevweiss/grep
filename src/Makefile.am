## Process this file with automake to create Makefile.in
AUTOMAKE_OPTIONS=no-dependencies

LN = ln

bin_PROGRAMS = grep egrep fgrep
grep_SOURCES = grep.h \
               dfa.c dfa.h \
               kwset.c  kwset.h \
               obstack.c obstack.h \
	       savedir.c savedir.h \
               getopt.c getopt1.c getopt.h \
               search.c getpagesize.h system.h

fgrep_SOURCES = $(grep_SOURCES)
egrep_SOURCES = $(grep_SOURCES)

grep_LDADD  = grep.o  $(LDADD)
egrep_LDADD = fgrep.o $(LDADD)
fgrep_LDADD = fgrep.o $(LDADD)

LDADD = @ALLOCA@ @LIBOBJS@ @INTLLIBS@
localedir = $(prefix)/@DATADIRNAME@/locale
INCLUDES = -I../intl -DLOCALEDIR=\"$(localedir)\"

man_MANS = grep.1 fgrep.1 egrep.1

EXTRA_DIST = grep.1 egrep.man fgrep.man \
             regex.c regex.h \
             dosbuf.c

CLEANFILES = egrep.1 fgrep.1

fgrep.o:
	$(COMPILE) -DMATCHER=\"fgrep\" $(srcdir)/egrep.c -c -o $@
egrep.o:
	$(COMPILE) -DMATCHER=\"egrep\" $(srcdir)/egrep.c -c -o $@

fgrep.1: fgrep.man
	inst=`echo "grep" | sed '$(transform)'`.1; \
sed -e "s%@grep@%$$inst%g" $(srcdir)/fgrep.man > $@

egrep.1: egrep.man
	inst=`echo "grep" | sed '$(transform)'`.1; \
sed -e "s%@grep@%$$inst%g" $(srcdir)/egrep.man > $@
