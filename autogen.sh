#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname "$0"`

if test ! -f "$srcdir"/configure.ac; then
	echo -n "**Error**: Directory \`$srcdir' does not look like the"
	echo " top-level $PKG_NAME directory"
	exit 1
fi

autoreconf -i
