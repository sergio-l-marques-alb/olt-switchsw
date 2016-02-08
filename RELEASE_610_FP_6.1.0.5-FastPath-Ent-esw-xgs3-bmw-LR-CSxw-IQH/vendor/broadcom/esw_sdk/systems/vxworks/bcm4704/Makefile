FP_BASE=../../../../../..

FPLIB=fpi_chassis.a

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
        -I$(BROADCOM_ROOT)/systems \
        -I$(FP_BASE)/bsp/cpu/$(L7_CPU)/$(L7_TARGETOS)/$(L7_CPU)

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
