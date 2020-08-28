#
# $Id: Makefile,v 1.4 Broadcom SDK $
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for dnx/dnx_data/auto_generated subdirectory
#

LOCALDIR = src/soc/dnx/intr/auto_generated

include ${SDK}/make/Make.config
subdirs := 
ifdef BCM_88690_A0 
subdirs += jr2 
endif
ifdef BCM_88800_A0 
subdirs += j2c 
endif
ifdef BCM_88480_A0
subdirs += q2a
endif
ifdef BCM_88850_A0
subdirs += j2p
endif
include ${SDK}/make/Make.subdirs

lib = libsoc_dnxintrag

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

