#! /bin/sh
# Regression test for GNU grep.
#
# Copyright (C) 2001, 2006, 2009-2010 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

. "${srcdir=.}/init.sh"; path_prepend_ ../src

# Test that grep was compiled with HAVE_LIBPCRE.  Otherwise, pass.
echo . | grep -P . >/dev/null 2>&1 || exit 0

fail=0
ft=

# See CVS revision 1.32 of "src/search.c".
echo | { grep -P '\s*$'; } > /dev/null 2>&1 || { ft="$ft 1"; fail=1; }

test "x$ft" != x && echo "Failed PCRE tests:$ft"

Exit $fail
