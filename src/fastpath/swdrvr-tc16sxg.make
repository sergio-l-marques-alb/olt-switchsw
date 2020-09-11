################################################
#                                              #
#   FastPath Makefile for the TC16SXG card     #
#                                              #
#                    2020                      #
#                                              #
################################################

# Board and CPU identification
export BOARD = TC16SXG
export CPU   = td3x3arm

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/eldk/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE = ppc_85xxDP-

# Kernel and SDK paths
KERNEL_PATH ?= $(OLT_DIR)/../lib/kernel/linux-denx-3.18.25_cxo640g-mx
SDK_PATH    ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.18/broadcom

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-tc16sxg.install

include swdrvr-common.make

# (Trident3-X3) FIXME: using CXO640G's toolchain, kernel and uP arch. It MUST be corrected
