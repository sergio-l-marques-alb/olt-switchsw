# $Id: Makefile,v 1.6 Broadcom SDK $
# $Copyright: (c) 2020 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# 

include ${SDK}/make/Make.config

export TARGET
export BLDDIR
export LIBDIR

ifdef INCLUDE_DNX_LIBS
subdirs += dnxc
subdirs += dnx
endif

ifdef DNXF_CHIPS
subdirs += dnxc
subdirs += dnxf
endif

ifdef SAND_CHIPS
subdirs += sand
endif

include ${SDK}/make/Make.subdirs


