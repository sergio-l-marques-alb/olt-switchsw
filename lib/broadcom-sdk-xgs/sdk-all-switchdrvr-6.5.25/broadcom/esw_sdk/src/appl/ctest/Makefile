#
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
LOCALDIR = src/appl/ctest/

include ${SDK}/make/Make.config

ifndef ESW_CHIPS
CFGFLAGS += -std=gnu99 -D_SOC_MEMACC_H -D_SOC_MEMREGS_H -D_SOC_ALLFIELDS_H -D_SOC_INTENUM_H -D_SOC_DRV_H
else
CFGFLAGS += -std=gnu99
endif
export CFGFLAGS

subdirs =
ifdef DNXC_CHIPS
subdirs += dnxc
endif
ifdef DNXF_CHIPS
subdirs += dnxf
endif
ifdef DNX_CHIPS
subdirs += dnx
endif

include ${SDK}/make/Make.subdirs
