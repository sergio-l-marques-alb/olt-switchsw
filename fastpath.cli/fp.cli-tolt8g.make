BOARD=TOLT8G
CPU=pq2pro

TOOLCHAIN_BASE_DIR=/opt/freescale/usr/local/gcc-4.0.2-glibc-2.3.6-nptl-2/powerpc-e300c3-linux
TOOLCHAIN_BIN_DIR=$(TOOLCHAIN_BASE_DIR)/bin
LD_LIBRARY_PATH=$(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE=$(TOOLCHAIN_BIN_DIR)/powerpc-e300c3-linux-

include fp.cli-common.make
