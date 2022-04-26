# $Id: Makefile,v 1.113 Broadcom SDK $
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for BCM driver Library
#

LOCALDIR = src/bcm/tomahawk3

export BCM_HIDE_DISPATCHABLE=1

include ${SDK}/make/Make.config

subdirs := $(SUBDIR_NONE)

include ${SDK}/make/Make.subdirs

lib = libbcm_th3new
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
