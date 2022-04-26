#
# $Id: Makefile Exp $
#
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#

LOCALDIR = src/soc/dpp/QAX/

include ${SDK}/make/Make.config

subdirs := QAX_PP

ifdef BCM_88270_A0  

ifndef LINUX_MAKE_KERNEL
subdirs += PEM
endif

endif

include ${SDK}/make/Make.subdirs

lib = libsoc_dpp_qax

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
