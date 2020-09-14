#
# $Copyright: (c) 2020 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# $Id: Makefile,v 1.11 Broadcom SDK $
LOCALDIR = src/appl/diag/dnxf

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

lib = libdiag_dnxf

subdirs+= swstate

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
