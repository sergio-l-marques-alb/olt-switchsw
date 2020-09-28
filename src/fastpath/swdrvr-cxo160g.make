################################################
#                                              #
#   FastPath Makefile for the CXO160G card     #
#                                              #
#       Milton Ruas, 2017                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = CXO160G
export CPU   = e500mc

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin/ppce500mc-fsl-linux
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE = powerpc-fsl-linux-

# Kernel and SDK paths
KERNEL_PATH ?= $(OLT_DIR)/../lib/kernel/linux-4.1.8-QorIQ-SDK2.0
SDK_PATH    ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.20/broadcom

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-cxo160g.install

include swdrvr-common.make

