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

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/broadcom
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE             = arm-linux-

# Kernel and SDK paths
KERNEL_PATH ?= $(OLT_DIR)/../lib/kernel/linux-3.6.5-arm
SDK_PATH    ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-fastpath-6.4.3/broadcom
# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-olt1t0.install

include swdrvr-common.make

