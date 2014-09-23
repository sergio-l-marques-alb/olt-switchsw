export TOOLCHAIN_BASE_DIR=/opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr
export TOOLCHAIN_BIN_DIR=$(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH=$(TOOLCHAIN_BASE_DIR)/lib

# Makefile for a multifile kernel module
CROSS=$(TOOLCHAIN_BIN_DIR)/ppce500mc-fsl-linux/powerpc-fsl-linux-
#LIB = 
CC=$(CROSS)gcc
LD=$(CROSS)ld

CFLAGS=-DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wno-strict-aliasing -g
OPTIMIZACAO = -O6

LDFLAGS = -Wl,-Bdynamic

TARGET_PATH=../fastpath/output/FastPath-Ent-esw-xgs4-e500mc-LR-CSxw-IQH_CXO640G/ipl

include fp.cli-common.make
