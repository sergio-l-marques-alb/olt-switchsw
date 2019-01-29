FP_BASE=../../../..

FPLIB=fp_dhcp_snooping.a

#
# DHCP Snooping Public Makefile.
#

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.cfg

# Specify the correct relative path for libsw.a file.
LIBDIR=../lib

# Specify compile flags.
#
CFLAGSLOCAL= -I.  $(GFLAG) -I$(FP_BASE)/src/application/switching/dhcp_snooping/include $(CRC_WARN1)

# Specify modules that need to be compiled.
#
LIBOBJS=ds_api.o ds_cfg.o ds_util.o ds_cnfgr.o \
	ds_outcalls.o ds_main.o ds_ipsg.o ds_ipsg_api.o \
	ds_binding.o ds_debug.o

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.footer

