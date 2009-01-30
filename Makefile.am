# Process this file with automake to create Makefile.in
# 
# Copyright 1997, 1998, 2005, 2006, 2007, 2008, 2009 Free Software
# Foundation, Inc.
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
AUTOMAKE_OPTIONS = no-dependencies

SUBDIRS = intl po lib doc src tests m4 vms bootstrap

EXTRA_DIST = TODO README README-alpha

# We should be able to just define `ACLOCAL_AMFLAGS = --acdir=m4',
# but this runs afoul of a bug in automake 1.
# The following hack works around this bug by creating acinclude.m4 manually.
ACLOCAL_AMFLAGS =
M4DIR = $(srcdir)/m4
ACINCLUDE_INPUTS = $(M4DIR)/decl.m4 $(M4DIR)/djgpp.m4 $(M4DIR)/dosfile.m4 \
  $(M4DIR)/envsep.m4 $(M4DIR)/error.m4 $(M4DIR)/gettext.m4 $(M4DIR)/glibc.m4 \
  $(M4DIR)/header.m4 $(M4DIR)/init.m4 $(M4DIR)/install.m4 \
  $(M4DIR)/inttypes_h.m4 $(M4DIR)/isc-posix.m4 $(M4DIR)/largefile.m4 \
  $(M4DIR)/lcmessage.m4 $(M4DIR)/malloc.m4 $(M4DIR)/mbstate_t.m4 \
  $(M4DIR)/missing.m4 $(M4DIR)/progtest.m4 $(M4DIR)/realloc.m4 \
  $(M4DIR)/regex.m4 $(M4DIR)/sanity.m4 $(M4DIR)/strerror_r.m4 \
  $(M4DIR)/uintmax_t.m4 $(M4DIR)/ulonglong.m4 $(M4DIR)/xstrtoumax.m4
$(srcdir)/acinclude.m4 : $(ACINCLUDE_INPUTS)
	cat $(ACINCLUDE_INPUTS) >$(srcdir)/acinclude.m4

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
