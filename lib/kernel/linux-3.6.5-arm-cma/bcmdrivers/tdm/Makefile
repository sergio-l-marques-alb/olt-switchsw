# iproc TDM support

EXTRA_CFLAGS	+= -I$(BCMDRIVERS_DIR)/gmac/src/include
KBUILD_CFLAGS += -DBCMDRIVER -Dlinux

iproc_tdm-objs := iproc-tdm.o

obj-$(CONFIG_IPROC_TDM) += iproc_tdm.o
