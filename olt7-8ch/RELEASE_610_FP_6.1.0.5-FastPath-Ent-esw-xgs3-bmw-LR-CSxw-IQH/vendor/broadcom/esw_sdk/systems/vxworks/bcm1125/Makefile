FP_BASE=../../../../../..

FPLIB=fp_bcm1125.a

# -*- Makefile -*-
# $Id: Makefile,v 1.1 2011/04/18 17:11:11 mruas Exp $
# BroadCom Driver Makefile.
#
#

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.cfg

# Specify additional header directories needed by your component
COMPONENT_INCLDIR += $(BROADCOM_INCL) \
	-I$(BSP_DIR) \
	-I$(BROADCOM_ROOT)/systems

# Specify compile flags.
#
CFLAGSLOCAL= $(GFLAG) $(SWCFLAG) -DBROADCOM_DEBUG

# Specify modules that need to be compiled.
#
ifeq ($(findstring VXWORKS,$(L7_OS_VERSION)),VXWORKS) 
LIBOBJS = socdiag.o
else
LIBOBJS =
endif

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.footer
