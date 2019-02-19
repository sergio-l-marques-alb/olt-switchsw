# $Id: Makefile,v 1.5 Broadcom SDK $
# $Copyright: (c) 2017 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for SOC SAL support (Linux Kernel)
#
LOCALDIR = src/sal/core/linux

include ${SDK}/make/Make.config

# Compiling with -std=gnu99 may lead to bogus 
# "initializer element is not constant" errors for certain kernel macros.
CFLAGS := $(filter-out -std=gnu99,$(CFLAGS))

lib = libsal_core_plat
include ${SDK}/make/Make.kernlib

include ${SDK}/make/Make.depend
