################################################
#                                              #
#   FastPath Makefile for the CXO640G card     #
#                                              #
#       Milton Ruas, 2017                      #
#       milton-r-silva@alticelabs.com          #
#                                              #
################################################

# Board and CPU identification
export BOARD = CXO640G
export CPU   = e500mc

# Base folders (using absolute references)
CURRENT_PATH = $(shell pwd)
FP_FOLDER    = $(word $(words $(subst /, ,$(CURRENT_PATH))),$(subst /, ,$(CURRENT_PATH)))
OLT_DIR      = $(subst /$(FP_FOLDER),,$(shell pwd))

# Cross-Compiler
export TOOLCHAIN_BASE_DIR = /opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr
export TOOLCHAIN_BIN_DIR  = $(TOOLCHAIN_BASE_DIR)/bin/ppce500mc-fsl-linux
export LD_LIBRARY_PATH    = $(TOOLCHAIN_BASE_DIR)/lib
COMPILER = $(TOOLCHAIN_BIN_DIR)/powerpc-fsl-linux-

# Kernel and SDK paths
KERNEL_PATH = /home/peter/kernel_3_0_51.test
SDK_PATH    = $(OLT_DIR)/../lib/broadcom-sdk-xgs/sdk-xgs-switchdrvr-6.5.7/broadcom

# Comilation logging level
export LVL7_MAKEFILE_LOGGING := N
export LVL7_MAKEFILE_DISPLAY_MODE := S

include switchdrvr-common.make

