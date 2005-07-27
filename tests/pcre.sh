#! /bin/sh
# Regression test for GNU grep.

: ${GREP=../src/grep}

# Test that grep was compiled with HAVE_LIBPCRE.  Otherwise, pass.
echo . | ${GREP} -P . >/dev/null 2>&1 || exit 0

fs=0
ft=

# See CVS revision 1.32 of "src/search.c".
echo | { ${GREP} -P '\s*$'; } > /dev/null 2>&1 || { ft="$ft 1"; fs=1; }

test "x$ft" != x && echo "Failed PCRE tests:$ft"
exit $fs
