#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([FanoutPlusPlus], [1.00], [null@dahler.org])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/FanoutPlusPlus.cpp])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Setup for Pthreads
ACX_PTHREAD([
	LIBS="$PTHREAD_LIBS $PTHREAD_CFLAGS $LIBS"
	CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
	CXX="$PTHREAD_CXX"
    ])

AC_OUTPUT(Makefile src/Makefile)
