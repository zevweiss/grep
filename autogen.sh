#!/bin/sh
set -x
cat m4/*.m4 > acinclude.m4
aclocal
autoheader
automake -a
autoconf
