FP_BASE=../../../../..

FPLIB=fp_base.a

# BROAD Makefile.
#
#
# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.cfg
# Specify additional header directories needed by your component
COMPONENT_INCLDIR += $(BROADCOM_INCL) \
		-I$(BROADCOM_SRC) \
		-I$(FP_BASE)/andl/hapi/$(L7_CHIP)/base \
		-I$(FP_BASE)/andl/hapi/$(L7_CHIP)/policy \
		-I$(FP_BASE)/andl/hapi/$(L7_CHIP)/switching \
		-I$(FP_BASE)/andl/dapi \
		-I../base/bcmsys \
# Specify compile flags.
#
#CFLAGSLOCAL= $(GFLAG) -DVXWORKS -DARROW -DMOUSSE -DBROADCOM_DEBUG -DDEBUG_STARTUP="DK_WARN+DK_ERR+DK_VERBOSE"

CFLAGSLOCAL= $(GFLAG)  $(CRC_WARN1) $(L7_OS_VERSION) -DBROADCOM_DEBUG
# Specify modules that need to be compiled.
#
LIBOBJS = l7_usl_bcm.o l7_usl_bcm_l2.o l7_usl_bcm_l3.o \
	  l7_usl_bcm_ipmcast.o l7_usl_bcm_debug.o l7_usl_bcm_policy.o \
	  broad_hpc_binds.o l7_usl_bcm_vlan_ipsubnet.o l7_usl_bcm_vlan_mac.o l7_bcmx_ipmcast.o \

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.footer

