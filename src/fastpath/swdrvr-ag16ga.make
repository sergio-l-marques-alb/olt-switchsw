################################################
#                                              #
#   FastPath Makefile for the TG16GF card      #
#                                              #
#       Milton Ruas, 2017                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = AG16GA
export CPU   = katanaarm


# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))
KERNEL_BASE  = /home/olt_shared/switchdrvr/netband-libs/trunk/linux-kernel-legacy

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/xldk/5.0.3/kt2/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
export CCACHE_TEMPDIR     = $(CURRENT_PATH)/.ccache
export CCACHE_DIR         = $(CURRENT_PATH)/.ccache
CROSS_COMPILE             = arm-linux-

# Kernel and SDK paths
KERNEL_PATH = $(KERNEL_BASE)/linux-3.6.5-kt2arm_LDK-3.4.7-RC4
SDK_PATH    = $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.15/broadcom

ifeq ($(DESTDIR),)
 export BUILDIR = $(OLT_DIR)/../build_dir/ag16ga
endif
export BUILDIR_LOCAL = $(OLT_DIR)/../build_dir_local/ag16ga

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-ag16ga.install

include swdrvr-common.make

