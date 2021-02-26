################################################
#                                              #
#   FastPath Makefile for the OLT1T0F card     #
#                                              #
#       Milton Ruas, 2017                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = OLT1T0F
export CPU   = katanaarm

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))
KERNEL_BASE  = /home/olt_shared/switchdrvr/netband-libs/trunk/linux-kernel-legacy

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/broadcom_kt2_hx4/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE  = arm-linux-

# Kernel and SDK paths
KERNEL_PATH = $(KERNEL_BASE)/linux-3.6.5-kt2arm_LDK-3.4.7-RC4
SDK_PATH    = $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-all-switchdrvr-6.5.15/broadcom

# DESTDIR will point to build_dir location (external libs and includes)
ifeq ($(DESTDIR),)
 export BUILDIR = $(OLT_DIR)/../build_dir/CXOLT1T0-F
endif
export BUILDIR_LOCAL = $(OLT_DIR)/../build_dir_local/CXOLT1T0-F

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

INSTALL_SCRIPT=swdrvr-olt1t0f.install

#NUM_CPUS=1

include swdrvr-common.make

