################################################
#                                              #
#   FastPath Makefile for the CXO640G card     #
#                                              #
#       Milton Ruas, 2017                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = CXO640G
export CPU   = pq3

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))
KERNEL_BASE  = /home/olt_shared/switchdrvr/netband-libs/trunk/linux-kernel-legacy

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/eldk/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE = ppc_85xxDP-

# Kernel and SDK paths
KERNEL_PATH ?= $(KERNEL_BASE)/linux-denx-3.18.25_cxo640g-mx
SDK_PATH    ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.20/broadcom

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-cxo640g.install

include swdrvr-common.make

