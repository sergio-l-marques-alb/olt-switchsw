################################################
#                                              #
#   FastPath Makefile for the TG16G card       #
#                                              #
#       Milton Ruas, 2017                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = TG16G
export CPU   = pq2pro

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))
KERNEL_BASE  = /home/olt_shared/switchdrvr/netband-libs/trunk/linux-kernel-legacy

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE = powerpc-e300c3-linux-

# Kernel and SDK paths
KERNEL_PATH ?= $(KERNEL_BASE)/linux-denx-3.18.25
SDK_PATH    ?= $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-switchdrvr-6.5.7/broadcom

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-tg16g.install

include swdrvr-common.make

