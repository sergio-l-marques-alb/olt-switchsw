BOARD=PTIN_BOARD_CXO640G

export TOOLCHAIN_BASE_DIR=/opt/eldk/usr
export TOOLCHAIN_BIN_DIR=$(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH=$(TOOLCHAIN_BASE_DIR)/lib

# Makefile for a multifile kernel module
CROSS=$(TOOLCHAIN_BIN_DIR)/ppc_85xxDP-
#LIB = 
CC=$(CROSS)gcc
LD=$(CROSS)ld

CFLAGS=-DPTIN_BOARD=$(BOARD) -DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wunused -Wno-sign-compare -g
OPTIMIZACAO = -O6

LDFLAGS = -Wl,-Bdynamic

TARGET_PATH=../fastpath/output/FastPath-Ent-esw-xgs4-pq3-LR-CSxw-IQH_CXO640G/ipl

include fp.cli-common.make
