#
# $Id:$
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# 
# Makefile for SOC swstate subdirectory
#
LOCALDIR = src/shared/swstate

include ${SDK}/make/Make.config
#
# Take the subdirs 'access layout diagnostic' if either DPP or DNX are defined
#
subdirs = 
ifdef DPP_CHIPS  
  subdirs += access layout diagnostic infrastructure utils
endif

ifdef DNX_CHIPS  
  subdirs += utils
endif

include ${SDK}/make/Make.subdirs

lib = libshared_swstate

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
