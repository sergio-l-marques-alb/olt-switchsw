#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
LOCALDIR = src/appl/ctest/dnx

include ${SDK}/make/Make.config

lib = libctest_dnx

subdirs+= algo dbal fabric field l3 lif switch tm trap vlan mpls oamp srv6 infra crps

ifneq (,$(findstring -DINCLUDE_KBP,$(CFGFLAGS)))
subdirs += kbp
endif

include ${SDK}/make/Make.subdirs
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
