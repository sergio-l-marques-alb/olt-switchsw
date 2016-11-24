# -*- Makefile -*-
# $Id: Makefile,v 1.1 2011/04/18 17:11:10 mruas Exp $
# $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
# OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
# BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$

override-target=ecos-raptor
LOCALDIR = systems/ecos/raptor/
LOCAL_CLEAN = bsplib_clean

ifndef ECOSKERNEL_INSTALL_DIR
toast:; @$(ECHO) 'The $$ECOSKERNEL_INSTALL_DIR environment variable is not set'; exit 1
endif

include $(ECOSKERNEL_INSTALL_DIR)/include/pkgconf/ecos.mak

MOZART=1
export ECOSKERNEL_INSTALL_DIR ECOS_GLOBAL_CFLAGS

ifndef LVL7_FIXUP
OPT_CFLAGS = -Wno-error
else
OPT_CFLAGS = 
endif
OPT_CPPFLAGS = $(OPT_CFLAGS)
OPT_CXXFLAGS = $(OPT_CFLAGS)
export OPT_CFLAGS OPT_CPPFLAGS OPT_CXXFLAGS 

all : build

ifdef SDK
include ${SDK}/make/Make.config
include $(SDK)/make/Make.depend
else
toast:; @$(ECHO) 'The $$SDK environment variable is not set'; exit 1
endif

build : kernel_modules sdk_src_modules bde_impl

bde_impl:
	$(MAKE) -f lib.mak

sdk_src_modules:
	$(MAKE) -C $(SDK)/src

kernel_modules:
	$(MAKE) -C $(SDK)/systems/bde/ecos

clean::
	$(MAKE) -C $(SDK)/src clean
	$(MAKE) -C $(SDK)/systems/bde/ecos clean
	$(MAKE) -f lib.mak clean

ifdef LVL7_FIXUP
.PHONY: get_sdk_flags

get_sdk_flags:
	@echo "CFLAGS_SDK := $(filter -D%,$(CFLAGS))" >$(L7_OUT_SDK_FLAGS)
endif

