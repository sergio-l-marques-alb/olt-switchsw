FP_BASE=../../../../../../..

FPLIB=fp_shared.a

# -*- Makefile -*-
# $Id: Makefile,v 1.1 2011/04/18 17:11:09 mruas Exp $
# $Copyright: (c) 2004 Broadcom Corp.
# All Rights Reserved.$
#
# Makefile for Linux user BDE
#

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.cfg

# Specify additional header directories needed by your component
COMPONENT_INCLDIR += $(BROADCOM_INCL) \
	-I../include \
	-I$(KERNEL_SRC)/arch/ppc
	
	

# Specify compile flags.
#
CFLAGSLOCAL= $(GFLAG) $(SWCFLAG)  \
 	-DBROADCOM_DEBUG

# Specify modules that need to be compiled.
#
ifeq ($(findstring LINUX,$(L7_OS_VERSION)),LINUX) 
LIBOBJS = mpool.o
else
LIBOBJS =
endif

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.footer

