#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ([2.68])
AC_INIT([Multicast Group Membership Discovery], [1.1.0], [hdcare@ptinovacao.pt], [mgmd])
AC_CONFIG_HEADERS([config.h])
AC_COPYRIGHT([Copyright 2012 PT Inovacao, SA.  All rights reserved.  Use is subject to license terms])
AM_INIT_AUTOMAKE([-Wall])
AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])

m4_pattern_allow([AM_PROG_AR], [AM_PROG_AR])  #AM_PROG_AR
# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_MAKE_SET
AM_PROG_CC_C_O
# Checks for libraries.
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero gettimeofday localtime_r memset socket strerror strtoul])

MGMD_LIBS="-lpthread -lrt"
AC_SUBST(MGMD_LIBS)
#LIBS="${LIBS} ${MGMD_LIBS}"

#MGMD_CFLAGS="-Wall -Wextra -Werror -Wno-missing-field-initializers -Wno-strict-aliasing -Wstack-protector -fstack-check -fstack-protector-all -g -ggdb"
MGMD_CFLAGS="-Wall -Wextra -Werror -Wno-missing-field-initializers -Wno-strict-aliasing -g -Os"
AC_SUBST(MGMD_CFLAGS)
#CFLAGS="${CFLAGS} ${MGMD_CFLAGS}"

RFS_PATH=$DESTDIR
AC_SUBST(RFS_PATH)

AC_ARG_ENABLE([binary], AS_HELP_STRING([--enable-binary], [Creates a MGMD standalone binary]))
AS_IF([test "x$enable_binary" = "xyes"], [binary=yes], [binary=no])
AS_IF([test "x$binary" = "xyes"], [AC_DEFINE(_COMPILE_AS_BINARY_, [], ["Creates a MGMD standalone binary"])])
AS_IF([test "x$binary" = "xyes"], [MGMD_CFLAGS+=" -D_COMPILE_AS_BINARY_"])
AM_CONDITIONAL([ENABLE_PTIN_BINARY], [test "x$binary" = "xyes"])

AC_ARG_VAR([PTIN_MGMD_PLATFORM_CTRL_TIMEOUT],[Number of seconds to wait for MGMD to answer a CTRL event])
AC_ARG_VAR([PTIN_MGMD_PLATFORM_STACK_SIZE],[Stack size to be used in mgmd and timers threads])
AC_ARG_VAR([PTIN_MGMD_PLATFORM_MAX_CHANNELS],[Maximum number of channels])
AC_ARG_VAR([PTIN_MGMD_PLATFORM_MAX_WHITELIST],[Maximum number of white-list entries])
AC_ARG_VAR([PTIN_MGMD_PLATFORM_MAX_CLIENTS],[Maximum number of clients])
AC_ARG_VAR([PTIN_MGMD_PLATFORM_MAX_PORTS],[Maximum number of ports])
AC_ARG_VAR([PTIN_MGMD_PLATFORM_MAX_SERVICES], [Maximum number of services])
AC_ARG_VAR([PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE], [Maximum frame size])

if test -z $PTIN_MGMD_PLATFORM_CTRL_TIMEOUT;then PTIN_MGMD_PLATFORM_CTRL_TIMEOUT=5;fi
if test -z $PTIN_MGMD_PLATFORM_STACK_SIZE;then PTIN_MGMD_PLATFORM_STACK_SIZE=10*32*1024;fi
if test -z $PTIN_MGMD_PLATFORM_MAX_CHANNELS;then PTIN_MGMD_PLATFORM_MAX_CHANNELS=2048;fi
if test -z $PTIN_MGMD_PLATFORM_MAX_WHITELIST;then PTIN_MGMD_PLATFORM_MAX_WHITELIST=$PTIN_MGMD_PLATFORM_MAX_CHANNELS;fi
if test -z $PTIN_MGMD_PLATFORM_MAX_CLIENTS;then PTIN_MGMD_PLATFORM_MAX_CLIENTS=1024;fi
if test -z $PTIN_MGMD_PLATFORM_MAX_PORTS;then PTIN_MGMD_PLATFORM_MAX_PORTS=20;fi
if test -z $PTIN_MGMD_PLATFORM_MAX_SERVICES;then PTIN_MGMD_PLATFORM_MAX_SERVICES=8;fi
if test -z $PTIN_MGMD_PLATFORM_MAX_SERVICE_ID;then PTIN_MGMD_PLATFORM_MAX_SERVICE_ID=$PTIN_MGMD_PLATFORM_MAX_SERVICES;fi
if test -z $PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE; then PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE=1500;fi

AC_SUBST(PTIN_MGMD_PLATFORM_CTRL_TIMEOUT)
AC_SUBST(PTIN_MGMD_PLATFORM_STACK_SIZE)
AC_SUBST(PTIN_MGMD_PLATFORM_MAX_CHANNELS)
AC_SUBST(PTIN_MGMD_PLATFORM_MAX_WHITELIST)
AC_SUBST(PTIN_MGMD_PLATFORM_MAX_CLIENTS)
AC_SUBST(PTIN_MGMD_PLATFORM_MAX_PORTS)
AC_SUBST(PTIN_MGMD_PLATFORM_MAX_SERVICES)
AC_SUBST(PTIN_MGMD_PLATFORM_MAX_SERVICE_ID)
AC_SUBST(PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE)

AS_IF([test $PTIN_MGMD_PLATFORM_MAX_WHITELIST -lt $PTIN_MGMD_PLATFORM_MAX_CHANNELS], AC_MSG_ERROR([The white-list must support at least max_channels entries]))


#
# unless explicitly set, drop previx from sysconfdir
# and localstatedir
#
if test "$sysconfdir" = '${prefix}/etc' ; then
        if test "$prefix" = 'NONE' ; then
                sysconfdir="/etc"
        else
                sysconfdir="$prefix"/etc
fi      
fi
AC_SUBST([sysconfdir])

if test "$localstatedir" = '${prefix}/var' ; then
        if test "$prefix" = 'NONE' ; then
                localstatedir="/var"
        else
                localstatedir="$prefix"/var
        fi
fi
AC_SUBST([localstatedir])


AC_CONFIG_FILES([Makefile 
                 src/Makefile
                 src/ptin_mgmd_defs.h
                 mgmd.pc])
AC_OUTPUT
echo "
mgmd configuration
--------------------
version					: ${version}
host operationg system			: ${host_os}
target					: ${target}
compiler				: ${CC}
compiler flags				: ${CFLAGS}
linker flags				: ${LDFLAGS} ${LIBS}
prefix					: ${prefix}
exec_prefix				: ${exec_prefix}
libdir					: ${libdir}
bindir					: ${bindir}
sbindir					: ${sbindir}
PTIN_MGMD_PLATFORM_CTRL_TIMEOUT		: ${PTIN_MGMD_PLATFORM_CTRL_TIMEOUT}
PTIN_MGMD_PLATFORM_STACK_SIZE		: ${PTIN_MGMD_PLATFORM_STACK_SIZE}
PTIN_MGMD_PLATFORM_MAX_CHANNELS		: ${PTIN_MGMD_PLATFORM_MAX_CHANNELS}
PTIN_MGMD_PLATFORM_MAX_WHITELIST	: ${PTIN_MGMD_PLATFORM_MAX_WHITELIST}
PTIN_MGMD_PLATFORM_MAX_CLIENTS		: ${PTIN_MGMD_PLATFORM_MAX_CLIENTS}
PTIN_MGMD_PLATFORM_MAX_PORTS		: ${PTIN_MGMD_PLATFORM_MAX_PORTS}
PTIN_MGMD_PLATFORM_MAX_SERVICES		: ${PTIN_MGMD_PLATFORM_MAX_SERVICES}
PTIN_MGMD_PLATFORM_MAX_SERVICE_ID	: ${PTIN_MGMD_PLATFORM_MAX_SERVICE_ID}
PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE	: ${PTIN_MGMD_PLATFORM_MAX_FRAME_SIZE}
" > /dev/stderr

