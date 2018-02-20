################################################
#                                              #
#   FastPath Makefile for the TA12XG card      #
#                                              #
#       Milton Ruas, 2018                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = ta12xg
export CPU   = e500

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/eldk/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
COMPILER = $(TOOLCHAIN_BIN_DIR)/ppc_85xxDP-

# Kernel and SDK paths
KERNEL_PATH = $(OLT_DIR)/../lib/kernel/linux-3.8.13
SDK_PATH    = $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.12/broadcom

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrv-ta12xg.install

include swdrv-common.make

