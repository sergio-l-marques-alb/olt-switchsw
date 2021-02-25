################################################
#                                              #
#   FastPath Makefile for the OLT1T0 card      #
#                                              #
#       Milton Ruas, 2017                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = OLT1T0
export CPU   = helixarm

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))
KERNEL_BASE  = /home/olt_shared/switchdrvr/netband-libs/trunk/linux-kernel-legacy

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/broadcom
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE             = arm-linux-

# Kernel and SDK paths
KERNEL_PATH ?= $(KERNEL_BASE)/linux-3.6.5-arm
SDK_PATH    ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom

# DESTDIR will point to build_dir location (external libs and includes)
ifeq ($(DESTDIR),)
 export BUILDIR = $(OLT_DIR)/../build_dir/CXOLT1T0
endif
export BUILDIR_LOCAL = $(OLT_DIR)/../build_dir_local/CXOLT1T0

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-olt1t0.install

include swdrvr-common.make

