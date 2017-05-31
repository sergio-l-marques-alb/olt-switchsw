BOARD=CXO160G
CPU=e500mc

TOOLCHAIN_BASE_DIR=/opt/fsl/1.2/sysroots/i686-fslsdk-linux/usr
TOOLCHAIN_BIN_DIR=$(TOOLCHAIN_BASE_DIR)/bin
LD_LIBRARY_PATH=$(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE=$(TOOLCHAIN_BIN_DIR)/ppce500mc-fsl-linux/powerpc-fsl-linux-

include fp.cli-common.make
