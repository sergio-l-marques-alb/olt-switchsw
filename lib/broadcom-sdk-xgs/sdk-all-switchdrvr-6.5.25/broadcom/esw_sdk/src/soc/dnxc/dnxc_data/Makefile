#
# $Id: Makefile,v 1.4 Broadcom SDK $
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for dnx/dnx_data subdirectory
#

LOCALDIR = src/soc/dnxc/dnxc_data

include ${SDK}/make/Make.config

ifdef BCM_DNX_INTERNAL_INIT
ifeq ($(shell test -d init_internal; echo $$?),0)
subdirs = init_internal
include ${SDK}/make/Make.subdirs
endif
endif

lib = libsoc_dnxcdnxcdata

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

