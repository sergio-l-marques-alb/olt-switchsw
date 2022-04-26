# $Id: Makefile,v 1.18 Broadcom SDK $
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for Switch on a Chip (SOC) driver and diagnostics
#

include ${SDK}/make/Make.config

subdirs =

ifdef BUILD_FLEXE
  subdirs += std
endif

ifdef BUILD_FLEXE_DBG
  subdirs += dbg
endif

include ${SDK}/make/Make.subdirs
