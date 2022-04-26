#
# $Id: Makefile,v 1.4 2016/10/10
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#

LOCALDIR = libs/aapl
ifndef AAPL
AAPL = $(CURDIR)
endif

include ${AAPL}/make/config.mk

VSRCS += $(wildcard *.c)

LSRCS = $(notdir $(VSRCS))
LOBJS = $(addsuffix .o, $(basename $(LSRCS)))
BOBJS = $(addprefix $(BLDDIR)/,$(LOBJS))

LIBNAME = libaapl
include ${AAPL}/make/lib.mk

include ${AAPL}/make/depend.mk
