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
KERNEL_BASE  = /home/olt_shared/switchdrvr/netband-libs/trunk

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin/ppce500mc-fsl-linux
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE = powerpc-fsl-linux-

# Kernel and SDK paths

KERNEL_PATH ?= $(KERNEL_BASE)/linux-4.1.35-QorIQ-SDK2.0
SDK_PATH    ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.20/broadcom

# DESTDIR will point to build_dir location (external libs and includes)
ifeq ($(DESTDIR),)
 export BUILDIR = $(OLT_DIR)/../build_dir/$(BOARD)
endif
export BUILDIR_LOCAL = $(OLT_DIR)/../build_dir_local/$(BOARD)

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-cxo160g.install

include swdrvr-common.make

