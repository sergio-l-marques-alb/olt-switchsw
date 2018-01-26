#
# $Copyright: (c) 2017 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
LOCALDIR = src/appl/ctest/

include ${SDK}/make/Make.config

CFGFLAGS += -std=gnu99
export CFGFLAGS

subdirs = dnxc

ifdef DNX_CHIPS
subdirs+=dnx
endif
ifdef DNXF_CHIPS
subdirs+=dnxf
endif

include ${SDK}/make/Make.subdirs
