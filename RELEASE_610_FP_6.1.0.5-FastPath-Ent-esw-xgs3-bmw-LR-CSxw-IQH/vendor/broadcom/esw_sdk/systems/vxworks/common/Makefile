# -*- Makefile -*-
# $Id: Makefile,v 1.1 2011/04/18 17:11:11 mruas Exp $
# $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
# OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
# BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
#

ifeq ($(findstring build_sdk_fastpath,$(L7_FEATURE)), build_sdk_fastpath)
  SYSTEM_DIR = $(SDK)/systems/vxworks/fastpath
else
  SYSTEM_DIR = $(SDK)/systems/vxworks/$(L7_CPU)
endif

all: build

include $(SDK)/make/Make.config
include $(SDK)/make/Make.depend

all_targets = user_libs 

# Define targets to be built by default.
build_targets = $(all_targets)

build:: $(BLDDIR)/.tree $(build_targets)


user_libs:
	$(MAKE) -C $(SDK)/systems/bde/vxworks 
	$(MAKE) -C $(SYSTEM_DIR) -f Makefile.fastpath 
	$(MAKE) -C $(SDK)/src 


clean::


distclean:: clean


.PHONY: variable

#
# Echo variable values used for configuration
# usage: make VAR=CC variable
#
variable::
	@echo $($(VAR))

# Make.config defines remaining phony targets
.PHONY: build user_libs


# Output the CFLAGS used by the SDK
#
ifdef LVL7_FIXUP
.PHONY: get_sdk_flags

get_sdk_flags::
	@echo "CFLAGS_SDK := $(filter -D%,$(CFLAGS))" >$(L7_OUT_SDK_FLAGS)
endif

