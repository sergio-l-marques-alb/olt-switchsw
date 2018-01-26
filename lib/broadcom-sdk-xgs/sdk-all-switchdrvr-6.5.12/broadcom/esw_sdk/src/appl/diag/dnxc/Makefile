#
# $Copyright: (c) 2017 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# $Id: Makefile,v 1.11 Broadcom SDK $
LOCALDIR = src/appl/diag/dnxc

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

lib = libdiag_dnxc

OBJS  = \
        diag_dnxc_data.o \
        diag_dnxc_fabric.o \
        diag_dnxc_show.o \
        diag_dnxc_diag.o \
        diag_dnxc_intr.o \
		diag_dnxc_linkscan.o

subdirs+= applref
subdirs+= legacy

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
