#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Require 2.65 or higher, for AC_TYPE_UINT64_T
AC_PREREQ([2.65])

# Intialize project specific info
AC_INIT([FanoutPlusPlus], [1.00], [null@dahler.org])
AC_CONFIG_SRCDIR([src/FanoutPlusPlus.cpp])

AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([src/config.h])

AC_DEFINE([PACKAGE_SOURCE_URL], ["https://github.com/brandondahler/FanoutPlusPlus"], [Define where to find source code])

# Checks for C++ compiler
AC_PROG_CXX


# Checks for general header files.
AC_CHECK_HEADERS([errno.h getopt.h netinet/in.h netinet/tcp.h sys/socket.h sys/stat.h sys/un.h unistd.h])


# Checks for libevent library.
AC_CHECK_LIB([event_core], [event_dispatch], 
	[LIBS="-levent_core $LIBS"], [AC_MSG_ERROR([libevent not found.])])

AC_CHECK_HEADERS([event2/event.h event2/util.h],
       [], [AC_MSG_ERROR([libevent headers not found.])])


# Checks for needed sizes.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T


# Output makefiles
AC_OUTPUT(Makefile src/Makefile)

