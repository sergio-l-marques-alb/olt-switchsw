#
# $Copyright: (c) 2018 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# $Id: Makefile,v 1.11 Broadcom SDK $
LOCALDIR = src/appl/diag/dnx

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

OBJS  = \
	diag_dnx_cmdlist.o \
	diag_dnx_visibility.o \
	diag_dnx_legacy.o

ifneq (,$(findstring -DADAPTER_SERVER_MODE,$(CFGFLAGS)))
subdirs+= adapter
endif
subdirs+= arr
subdirs+= crps
subdirs+= dbal
subdirs+= diag
subdirs+= dram
subdirs+= error_recovery
subdirs+= export
subdirs+= field
subdirs+= switch
subdirs+= gpm
subdirs+= reset
subdirs+= kleap
subdirs+= l2
subdirs+= vlan
subdirs+= l3
subdirs+= mdb
subdirs+= meter
subdirs+= oam
subdirs+= port
subdirs+= stg
subdirs+= stif
subdirs+= tm
subdirs+= trap
subdirs+= tx
subdirs+= nif
subdirs+= swstate
subdirs+= fabric
subdirs+= kbp
subdirs+= pp
subdirs+= stat_pp
subdirs+= time_analyzer
subdirs+= knet
subdirs+= srv6
# do no remove line below, it is used by auto VENDOR code removal script (remdef.sh)
#endif

lib = libdiag_dnx

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
