#!/bin/sh
#
# Tell them not to be alarmed.

: ${srcdir=.}

failures=0

#
cat <<\EOF

Please, do not be alarmed if some of the tests failed.
Report them to <bug-grep@gnu.org>,
with the line number, the name of the file,
and grep version number 'grep --version'.
Thank You.

EOF
