#
# $Id: Makefile,v 1.4 Broadcom SDK $
# $Copyright: (c) 2018 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

subdirs =	adapter	\
			dbal	\
			dnx_data\
                        dnx_data/auto_generated/jer2_a0	\
                        dnx_data/auto_generated/jer2_b0	\
                        dnx_data/auto_generated\
			drv	\
			field/tcam_access	\
			mdb	\
			pemladrv	\
			pll \
			swstate	\
			legacy	\
			kbp  \
			linkctrl  \
			utils  \
			arr  \
			stat \
            synce \
			intr \
			intr/auto_generated

#jericho2 dnx PEMLA compilation
ifeq ($(PEMLA_BUILD),1)
PEMLA_PATH=${SDK}/tools/sand/db/jericho_2/pemla
PEMLA_B0_PATH=${SDK}/tools/sand/db/jericho_2_b0/pemla
ifneq ($(REAL_SDK),)
PEMLA_PATH=${REAL_SDK}/tools/sand/db/jericho_2/pemla
PEMLA_B0_PATH=${REAL_SDK}/tools/sand/db/jericho_2_b0/pemla
endif
#Build ucode if pemla folder exists
ifneq ($(wildcard $(PEMLA_PATH)),)
subdirs += $(PEMLA_PATH)
endif
ifneq ($(wildcard $(PEMLA_B0_PATH)),)
subdirs += $(PEMLA_B0_PATH)
endif
endif

include ${SDK}/make/Make.subdirs
