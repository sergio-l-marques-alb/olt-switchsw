FP_BASE=../../../../..

FPLIB=fp_ip_mcast.a

# HAPI L3 Makefile.
#
#

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.cfg

# Specify additional header directories needed by your component
COMPONENT_INCLDIR +=  $(BROADCOM_INCL) \
			-I$(BROADCOM_SRC) \
			-I$(FP_BASE)/andl/dapi \
			-I$(FP_BASE)/andl/hapi/$(L7_CHIP)/base \
			-I$(FP_BASE)/andl/hapi/(L7_CHIP)/switching \
			-I$(FP_BASE)/andl/hapi/(L7_CHIP)/stacking \
			-I$(FP_BASE)/andl/hapi/(L7_CHIP)/stacking_binds

# Specify compile flags.
#
CFLAGSLOCAL= $(GFLAG) $(L7_OS_VERSION) $(CRC_WARN1) -DBROADCOM_DEBUG

# Specify modules that need to be compiled.
#

LIBOBJS = broad_l3_mcast.o broad_l3_mcast_debug.o

clean::
	-$(RM) *.o
	-$(RM) *.makefile.log
	

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.footer

