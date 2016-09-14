export TOOLCHAIN_BASE_DIR=/opt/broadcom
export TOOLCHAIN_BIN_DIR=$(TOOLCHAIN_BASE_DIR)/bin
export LD_LIBRARY_PATH=$(TOOLCHAIN_BASE_DIR)/lib

# Makefile for a multifile kernel module
CROSS=$(TOOLCHAIN_BIN_DIR)/arm-linux-
#LIB = 
CC=$(CROSS)gcc
LD=$(CROSS)ld

CFLAGS=-DMAJOR=$(MAJOR) -DMINOR=$(MINOR) -DP_DEBUG -DSTT_UNDERLYING_OS=U_LINUX \
  -DREVISION=$(REVISION) -DBUILD=$(BUILD) -Wno-strict-aliasing -g \
  -marm -march=armv7-a -mfpu=vfp -mfloat-abi=softfp -msoft-float -fomit-frame-pointer \
  -Wno-pointer-sign -Wno-unused-but-set-variable -Wno-enum-compare -Wno-switch -Wno-address -Wno-sequence-point
OPTIMIZACAO = -O6

LDFLAGS = -Wl,-Bdynamic

TARGET_PATH=../fastpath/output/FastPath-Ent-esw-xgs4-katanaarm-LR-CSxw-IQH_TG16GF/ipl

include fp.shell-common.make
