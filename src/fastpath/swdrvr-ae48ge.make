################################################
#                                              #
#   FastPath Makefile for the TA48GE card      #
#                                              #
#       Milton Ruas, 2017                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = AE48GE
export CPU   = armv7

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))
KERNEL_BASE  = /home/olt_shared/switchdrvr/netband-libs/trunk

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/broadcom_gh2/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
export CCACHE_TEMPDIR     = $(CURRENT_PATH)/.ccache
export CCACHE_DIR         = $(CURRENT_PATH)/.ccache
export BUILDIR           ?= $(DESTDIR)
export BUILDIR_LOCAL     ?= $(DESTDIR)
CROSS_COMPILE = arm-linux-

# Kernel and SDK paths
ifeq ($(KERNEL_SOURCE_DIR),)
 KERNEL_PATH = $(KERNEL_BASE)/linux-ae48ge/linux-4.14.48
else
 KERNEL_PATH = $(KERNEL_SOURCE_DIR)
endif

SDK_PATH    = $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.15/broadcom

ifeq ($(DESTDIR),)
 export BUILDIR = $(OLT_DIR)/../build_dir/ae48ge
endif
export BUILDIR_LOCAL = $(OLT_DIR)/../build_dir_local/ae48ge

#NUM_CPUS=1

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-ae48ge.install

include swdrvr-common.make

