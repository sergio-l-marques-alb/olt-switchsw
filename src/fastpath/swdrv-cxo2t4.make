################################################
#                                              #
#   FastPath Makefile for the CXO2T4 card      #
#                                              #
#       Milton Ruas, 2018                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = cxo2t4
export CPU   = e500mc

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))

# Cross-Compiler
TOOLCHAIN_BASE_DIR = /opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr
TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin/ppce500mc-fsl-linux
LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
COMPILER = $(TOOLCHAIN_BIN_DIR)/powerpc-fsl-linux-

#export SYSROOT=--sysroot=/opt/fsl/1.2/sysroots/ppce500mc-fsl-linux

# DESTDIR will point to build_dir location (external libs and includes)
ifeq ($(DESTDIR),)
 DESTDIR = $(OLT_DIR)/../build_dir/$BOARD
endif
# KERNEL_SOURCE_DIR will point to the kernel location
ifeq ($(KERNEL_SOURCE_DIR),)
 KERNEL_SOURCE_DIR = $(OLT_DIR)/../lib/kernel/linux-4.1.8-QorIQ-SDK2.0-CXO2T4
endif

# Kernel and SDK paths
KERNEL_PATH = $(KERNEL_SOURCE_DIR)
SDK_PATH    = $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.12/broadcom

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

#NUM_CPUS=1

INSTALL_SCRIPT=swdrv-cxo2t4.install

include swdrv-common.make

