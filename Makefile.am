# Process this file with automake to create Makefile.in
#
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
#
AUTOMAKE_OPTIONS = gnu 1.10
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po lib doc src tests vms gnulib-tests

EXTRA_DIST = build-aux/config.rpath TODO README README-alpha \
	     Makefile.boot README-boot


# make cvs-clean blatantly stolen from KDE CVS ;)
cvs-clean:
	@if test ! -d CVS; then \
	  echo "You don't have a toplevel CVS directory."; \
	  echo "You most certainly didn't use cvs to get these sources."; \
	  echo "But this function depends on cvs's information."; \
	  exit 1 ;\
	fi;\
	pwd=`pwd` ;\
	dirs=`find . -type d | grep -v CVS | sed -e "s#^./##"` ;\
	for i in $$dirs; do \
	  if test ! -d "$$pwd/$$i/CVS"; then \
	    echo "D $$i" ;\
	    rm -rf "$$pwd/$$i"; \
	    continue; \
	  fi ;\
	  cd $$pwd/$$i ;\
	  for f in * .*; do \
	    if test ! -d "$$f"; then \
	      if grep "^/$$f/" CVS/Entries > /dev/null; then \
		a="b"; \
	      else \
	        echo "F $$i/$$f"; \
		rm -f "$$pwd/$$i/$$f"; \
	      fi; \
	    fi ; \
	  done; \
	done

# Arrange so that .tarball-version appears only in the distribution
# tarball, and never in a checked-out repository.
dist-hook: gen-ChangeLog
	$(AM_V_GEN)echo $(VERSION) > $(distdir)/.tarball-version

gen_start_date = 2009-11-27
.PHONY: gen-ChangeLog
gen-ChangeLog:
	$(AM_V_GEN)if test -d .git; then				\
	  $(top_srcdir)/build-aux/gitlog-to-changelog			\
	    --since=$(gen_start_date) > $(distdir)/cl-t;		\
	  rm -f $(distdir)/ChangeLog;					\
	  mv $(distdir)/cl-t $(distdir)/ChangeLog;			\
	fi
