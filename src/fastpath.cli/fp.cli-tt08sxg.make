export TOOLCHAIN_BASE_DIR=/opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr
export TOOLCHAIN_BIN_DIR=$(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH=$(TOOLCHAIN_BASE_DIR)/lib
#export LD_LIBRARY_PATH=/opt/eldk/ppc_85xxDP/lib

# Makefile for a multifile kernel module
CROSS=$(TOOLCHAIN_BIN_DIR)/ppce500mc-fsl-linux/powerpc-fsl-linux-
#LIB = 
CC=$(CROSS)gcc
LD=$(CROSS)ld

CFLAGS=-DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -Wno-strict-aliasing -g
OPTIMIZACAO = -O6

LDFLAGS = -Wl,-Bdynamic

TARGET_PATH=../fastpath/output/FastPath-Ent-dpp-dnx-e500mc-LR-CSxw-IQH_TT08SXG/ipl

include fp.cli-common.make
