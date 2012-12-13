#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])

AC_INIT([FanoutPlusPlus], [1.00], [null@dahler.org])
AC_CONFIG_SRCDIR([src/FanoutPlusPlus.cpp])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_LIB([event_core], [event_dispatch], 
	[LIBS="-levent_core $LIBS"], [AC_MSG_ERROR([libevent not found.])])

# Checks for header files.
AC_CHECK_HEADERS([errno.h netinet/in.h netinet/tcp.h sys/socket.h sys/stat.h sys/un.h unistd.h])

AC_CHECK_HEADERS([event2/event.h event2/util.h],
       [], [AC_MSG_ERROR([libevent headers not found.])])


# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

AC_OUTPUT(Makefile src/Makefile)

