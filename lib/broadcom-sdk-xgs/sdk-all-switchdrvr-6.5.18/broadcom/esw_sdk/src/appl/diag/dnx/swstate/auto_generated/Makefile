#
# $Id$
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for dpp subdirectory
#

LOCALDIR = src/appl/diag/dnx/swstate/auto_generated

include ${SDK}/make/Make.config
#
# If this is compilation for SLK then optimize this library for code
# size.
#
ifeq ($(findstring SLK,$(TOOLCHAIN_BASE_DIR)),SLK)
CFGFLAGS += -Os
endif

lib = libdiag_dnxswstate_auto_generated

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

