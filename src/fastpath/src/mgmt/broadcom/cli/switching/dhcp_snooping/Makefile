FP_BASE=../../../../..

FPLIB=fp_dhcp_snooping.a

# L2 Switch Make file.
#
#  

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.cfg

# Specify compile flags. 
#
CLI_DIR=../..
EMWEB_DIR=../../../emweb

include $(CLI_DIR)/base/common/cli_includepaths.make

CFLAGSLOCAL= -I. $(EXTRA_OPT) $(GFLAG) $(CRC_WARN1) $(CLI_INC_PATHS) -I$(CLI_DIR)/stacking

# Specify modules that need to be compiled.
#

LIBOBJS=cli_config_dhcp_snooping.o clicommands_dhcp_snooping.o \
	cli_show_dhcp_snooping.o cli_show_running_config_dhcp_snooping.o \
	cli_clear_dhcp_snooping.o

# Do not change this line.
#
include $(FP_BASE)/src/l7tools/build/make.footer

