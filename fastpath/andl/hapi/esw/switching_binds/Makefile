FP_BASE=../../../../..

FPLIB=fp_switching_binds.a

# HAPI L3 Makefile.
#
#

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.cfg

# Specify additional header directories needed by your component
COMPONENT_INCLDIR +=  $(BROADCOM_INCL) \
                    -I$(FP_BASE)/andl/dapi \
                    -I$(FP_BASE)/andl/hapi/$(L7_CHIP)/base \
                    -I$(FP_BASE)/andl/hapi/$(L7_CHIP)/switching 

# Specify compile flags.
#
CFLAGSLOCAL= $(GFLAG) $(L7_OS_VERSION)

ifeq (0,$(BLD_L7_PRODUCT_SMARTPATH))
CFLAGSLOCAL += -DBROADCOM_DEBUG
endif


# Specify modules that need to be compiled.
#

LIBOBJS = broad_l3.o

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.footer

