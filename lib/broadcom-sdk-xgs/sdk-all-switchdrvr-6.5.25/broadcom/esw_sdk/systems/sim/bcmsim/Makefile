#
# $Copyright: (c) 2021 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
# $Id: Makefile,v 1.11 Broadcom SDK $
ifdef SDK
include ${SDK}/make/Make.config
endif

sdk_env_check:
ifndef SDK
	@echo "SDK environment variable not set. "
	exit 1
endif

lcm_env_check:
ifndef BCMSIM_ROOT
	@echo "BCMSIM_ROOT environment variable not set "
	exit 1
endif

BCMSIM_DIAG_LIBS = \
	${COND_LIB_EDITLINE} \
	libdiag.$(libext) \
	${ESW_DIAG_LIBS} \
	${COND_LIB_TEST} \
	${COND_LIB_TCL}  \
	$(COND_LIB_SAL_APPL) \
	$(COND_LIB_SAL_APPL_PLATFORM)
    
BCM_LIBS = ${APPL_LIBS} ${BCMSIM_DIAG_LIBS} ${DRV_LIBS}
BCM_LIBS_BLD = $(addprefix ${LIBDIR}/,${BCM_LIBS})

pcid: sdk_env_check
	$(MAKE) -C ${SDK}/systems/sim

all: pcid
