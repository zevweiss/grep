#!/bin/sh
aclocal -I `dirname $0`/m4
autoheader
automake -a
autoconf
