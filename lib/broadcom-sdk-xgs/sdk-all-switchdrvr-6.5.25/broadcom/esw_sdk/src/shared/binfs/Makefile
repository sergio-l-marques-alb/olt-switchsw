# $Id: Makefile,v 1.1 Broadcom SDK $
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
# Makefile for Shared Library
#

LOCALDIR = src/shared/binfs

include ${SDK}/make/Make.config

ifeq ($(BINFS_UPDATE),TRUE)
$(warning Update binfs data .... $(shell python $(SDK)/tools/genbinfs.py > /dev/null))
endif

OBJS = binfs.o binfs_file_list.o

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

lib = libshared_binfs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
