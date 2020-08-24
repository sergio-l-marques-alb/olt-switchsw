#
# $Id:$
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# 
# Makefile for SOC swstate subdirectory
#
LOCALDIR = src/bcm/dnx/algo/swstate/auto_generated

include ${SDK}/make/Make.config
#
# If this is compilation for SLK then optimize this library for code
# size.
#
ifeq ($(findstring SLK,$(TOOLCHAIN_BASE_DIR)),SLK)
CFGFLAGS += -Os
endif

lib = libbcm_dnxalgoswstate_auto_generated

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
