#
# $Id$
# $Copyright: (c) 2017 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for dpp subdirectory
#

LOCALDIR = src/soc/dnxc

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

subdirs = legacy dnxc_data swstate

include ${SDK}/make/Make.subdirs

lib = libsoc_dnxc

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
