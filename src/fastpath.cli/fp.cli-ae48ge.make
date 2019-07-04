BOARD=AE48GE
CPU=armv7

TOOLCHAIN_BASE_DIR ?= /opt/broadcom_gh2/usr
TOOLCHAIN_BIN_DIR  ?= $(TOOLCHAIN_BASE_DIR)/bin
LD_LIBRARY_PATH    ?= $(TOOLCHAIN_BASE_DIR)/lib
CROSS_COMPILE      ?= $(TOOLCHAIN_BIN_DIR)/arm-linux-

export CCACHE_TEMPDIR ?= /home/disk1/milton-r-silva/ccache
export CCACHE_DIR     ?= /home/disk1/milton-r-silva/ccache

include fp.cli-common.make
