AC_PREREQ([2.69])
AC_INIT([mfm], [001], [angelo70@gmail.com], [mfm], [http://www.sysam.it/])
AM_INIT_AUTOMAKE([subdir-objects])

AC_CHECK_TOOL(CC, gcc, gcc)
AC_CHECK_TOOL(CXX, g++, g++)

AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test "x$PKG_CONFIG" = "xno"; then
        AC_MSG_ERROR([You need to install pkg-config])
fi

# starting cxxflags
CXXFLAGS='-O3 --pipe -Wall -std=c++17'

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

FILESYSTEM_LIBS="-lstdc++fs"
AC_SUBST([FILESYSTEM_LIBS])

LIBS="$LIBS $FILESYSTEM_LIBS"

AC_SUBST([FILESYSTEM_LIBS])

PKG_CHECK_MODULES([XEXT], [xext >= 1.3])
PKG_CHECK_MODULES([X11], [x11 >= 1.3])
PKG_CHECK_MODULES([LIBCONFIG], [libconfig >= 1.3.0])

# FLTK
unset fltkconfig
AC_CHECK_PROGS([fltkconfig], [fltk-config13 fltk-config])

if test "x$fltkconfig" = x; then
	AC_MSG_ERROR([fltk-config not found])
fi
VER=$($fltkconfig --version)
MINOR=$(echo $VER | cut -d. -f2)
PATCH=$(echo $VER | cut -d. -f3)

if test "$MINOR" -lt 3 || test "$PATCH" -lt 3; then
	AC_MSG_ERROR([FLTK >= 1.3.3 not found, got $VER])
fi

CXXFLAGS=["$CXXFLAGS $($fltkconfig --cxxflags | sed 's@-I@-isystem @g')"]
LIBS=["$LIBS $($fltkconfig --ldflags --use-images)"]

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
