# $Id: Makefile,v 1.1 2011/04/18 17:11:03 mruas Exp $
# $Copyright: (c) 1999-2002, 2003 Broadcom Corp.
# All Rights Reserved.$
#
# Makefile for SOC editline support
#
LOCALDIR = src/sal/appl/editline

include ${SDK}/make/Make.config

CFLAGS += -DANSI_ARROWS -DUNIQUE_HISTORY -DHIST_SIZE=64

ifeq (${targetbase},posix)
CFLAGS += -DSYS_UNIX -DHAVE_TCGETATTR -DUSE_DIRENT
endif

ifeq (${targetbase},unix)
CFLAGS += -DSYS_UNIX -DHAVE_TCGETATTR -DUSE_DIRENT
endif

ifneq (${targetbase},linux)
CFLAGS += -DHAVE_STDLIB
endif

lib = libsal_appl_editline
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
