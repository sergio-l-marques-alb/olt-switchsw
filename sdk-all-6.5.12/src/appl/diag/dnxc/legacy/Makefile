#
# $Copyright: (c) 2017 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# $Id: Makefile,v 1.10 Broadcom SDK $
LOCALDIR = src/appl/diag/dnxc/legacy

include ${SDK}/make/Make.config

lib = libdiag_dnxclegacy

CFGFLAGS += -std=gnu99 -D_GNU_SOURCE

OBJS  = \
	port.o \

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
