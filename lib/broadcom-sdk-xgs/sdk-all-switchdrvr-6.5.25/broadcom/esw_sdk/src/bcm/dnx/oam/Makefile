#
# $Id: Makefile,v 1.4 Broadcom SDK $
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#


LOCALDIR = src/bcm/dnx/oam

include ${SDK}/make/Make.config

#
# Take the subdir 
#

ifdef DNX2_CHIPS
# {
subdirs_oamp_v1 =	\
    oamp_v1
# }
endif


ifdef DNX2_CHIPS
# {
subdirs +=  ${subdirs_oamp_v1}
# }
endif

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxoam

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend

