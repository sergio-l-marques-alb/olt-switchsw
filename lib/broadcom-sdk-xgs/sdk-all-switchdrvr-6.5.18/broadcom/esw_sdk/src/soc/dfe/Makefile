#
# $Id$
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for dfe subdirectory
#

LOCALDIR = src/soc/dfe

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

subdirs := cmn

ifdef BCM_88750_A0
subdirs += fe1600
endif

ifdef BCM_88950_A0
subdirs += fe1600
subdirs += fe3200
endif

include ${SDK}/make/Make.subdirs

lib = libsoc_dfe

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

