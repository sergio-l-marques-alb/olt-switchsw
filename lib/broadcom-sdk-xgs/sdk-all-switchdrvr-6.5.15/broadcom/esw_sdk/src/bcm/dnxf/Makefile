# $Id$
# $Copyright: (c) 2018 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for BCM driver Library
#

LOCALDIR = src/bcm/dnxf

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

subdirs = tune

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxf

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
