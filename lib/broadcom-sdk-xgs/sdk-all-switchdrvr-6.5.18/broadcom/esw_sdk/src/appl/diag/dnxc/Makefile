#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# $Id: Makefile,v 1.11 Broadcom SDK $
LOCALDIR = src/appl/diag/dnxc

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

lib = libdiag_dnxc

OBJS  = \
        diag_dnxc_data.o \
        diag_dnxc_data_man.o \
        diag_dnxc_fabric.o \
        diag_dnxc_show.o \
        diag_dnxc_counter.o \
        diag_dnxc_diag.o \
        diag_dnxc_intr.o \
        diag_dnxc_linkscan.o \
        diag_dnxc_soc.o \
        diag_dnxc_lane_map.o \
        diag_dnxc_pbmp.o \
        diag_dnxc_port.o \
        diag_dnxc_phy_prbsstat.o \
        diag_dnxc_mta.o \
        diag_dnxc_warmboot.o \
        diag_dnxc_diag_ser.o \
        diag_dnxc_patch_database.o \
        diag_dnxc_init_dnx.o

subdirs+= applref
subdirs+= swstate

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
