#
# $Id$
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for dpp subdirectory
#

LOCALDIR = src/soc/dcmn

include ${SDK}/make/Make.config
#
# If this is compilation for SLK-ALL then optimize for code
# size. This is added to temporarily overcome the code size
# issue on SLK compiler.
#
ifeq ($(findstring OPTIMIZE_CODE,$(MAKE_CUSTOM_SLK_ALL)),OPTIMIZE_CODE)
CFGFLAGS += -Os
endif

export CFGFLAGS

include ${SDK}/make/Make.subdirs

lib = libsoc_dcmn

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

