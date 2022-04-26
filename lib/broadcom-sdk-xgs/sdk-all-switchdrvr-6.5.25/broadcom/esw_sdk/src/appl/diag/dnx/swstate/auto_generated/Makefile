#
# $Id$
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for dpp subdirectory
#

LOCALDIR = src/appl/diag/dnx/swstate/auto_generated

include ${SDK}/make/Make.config
#
# If this is compilation for SLK then optimize this library for code
# size. Use PARTIAL_OPT_CODE for doing only partial code size optimization
#
ifeq ($(findstring PARTIAL_OPT_CODE,$(MAKE_CUSTOM_CODE_SIZE)),PARTIAL_OPT_CODE)
CFGFLAGS += -Os
endif
ifeq ($(findstring OPTIMIZE_CODE,$(MAKE_CUSTOM_CODE_SIZE)),OPTIMIZE_CODE)
CFGFLAGS += -Os
endif

lib = libdiag_dnxswstate_auto_generated

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

