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
KERNEL_BASE  = /home/olt_shared/switchdrvr/netband-libs/trunk

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/xldk/6.0.1/helix5/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE = aarch64-linux-

export CCACHE_TEMPDIR = /home/olt_shared/switchdrvr/ccache
export CCACHE_DIR     = /home/olt_shared/switchdrvr/ccache

# Kernel and SDK paths
KERNEL_PATH ?= $(KERNEL_BASE)/linux-tc16sxg/linux-5.4.2
SDK_PATH    ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.20/broadcom

# DESTDIR will point to build_dir location (external libs and includes)
ifeq ($(DESTDIR),)
 export BUILDIR = $(OLT_DIR)/../build_dir/$(BOARD)
endif
export BUILDIR_LOCAL = $(OLT_DIR)/../build_dir_local/$(BOARD)

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

#NUM_CPUS=1

INSTALL_SCRIPT=swdrvr-tc16sxg.install

include swdrvr-common.make

