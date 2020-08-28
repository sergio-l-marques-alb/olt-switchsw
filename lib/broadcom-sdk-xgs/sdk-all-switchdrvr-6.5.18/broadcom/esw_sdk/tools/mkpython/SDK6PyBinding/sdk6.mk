#
# SDK 6 makefile variable extractor
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#

ifndef SDK
$(error 'The $$SDK environment variable is not set')
endif

include ${SDK}/make/Make.config
include ${SDK}/make/Make.targets

BCM_LIBS = ${APPL_LIBS} ${DIAG_LIBS} ${DRV_LIBS}

OTH_OBJ = ${BLDDIR}/systems/sim/socdiag.o
OTH_OBJ += ${BLDDIR}/systems/sim/version.o

CFLAGS := $(filter-out -Werror,$(CFLAGS))

.PHONY: all libdir libs objs cflags cc

all: libdir libs objs cflags cc

libdir:
	@echo $(LIBDIR)

libs:
	@echo $(BCM_LIBS)

objs:
	@echo $(OTH_OBJ)

cflags:
	@echo $(CFGFLAGS) $(CFLAGS)

cc:
	@echo $(CC)

