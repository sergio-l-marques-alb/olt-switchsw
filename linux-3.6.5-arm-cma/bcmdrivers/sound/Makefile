# bcm535x support

EXTRA_CFLAGS	+= -I$(BCMDRIVERS_DIR)/gmac/src/include
KBUILD_CFLAGS += -DBCMDRIVER -Dlinux

iproc_snd-objs := bcm535x.o bcm535x-i2s.o bcm535x-pcm.o

obj-$(CONFIG_IPROC_SND) += iproc_snd.o
