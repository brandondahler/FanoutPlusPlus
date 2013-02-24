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

# Checks for syslog header file
AC_CHECK_HEADERS([syslog.h])

# Checks for libevent library.
AC_CHECK_LIB([event_core], [event_dispatch], 
	[LIBS="-levent_core $LIBS"], [AC_MSG_ERROR([libevent not found.])])

AC_CHECK_HEADERS([event2/event.h event2/util.h],
       [], [AC_MSG_ERROR([libevent headers not found.])])


# Checks for needed sizes.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Add IPv6 feature, enable by default.

AC_ARG_ENABLE([ipv6], [   --disable-ipv6         Disables IPv6 support], 
    [case "${enableval}" in
      yes | no ) WITH_IPV6="${enableval}" ;;
      *) AC_MSG_ERROR(bad value ${enableval} for --disable-ipv6) ;;
    esac],
    [WITH_IPV6="yes"]
)

AM_CONDITIONAL([WITH_IPV6], [test "x$WITH_IPV6" = "xyes"])

# Define USE_IPV6 in config.h if we're going to compile against it
if test "x$WITH_IPV6" = "xyes"; then
    AC_DEFINE([USE_IPV6], [1], [Build with IPv6 support])
fi

# Output makefiles
AC_OUTPUT(Makefile src/Makefile)

