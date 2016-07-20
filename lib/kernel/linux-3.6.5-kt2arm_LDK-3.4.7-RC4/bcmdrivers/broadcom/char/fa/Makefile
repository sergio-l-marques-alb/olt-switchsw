# File: bcmdrivers/broadcom/char/fa/Makefile
#
# Makefile for the BCM9530x Flow Acceleration module
#

ctf-objs   := ctf_drv.o ctf_config.o fcache.o
 
obj-$(CONFIG_IPROC_FA) += ctf.o

EXTRA_CFLAGS += -I. -I$(BCMDRIVERS_DIR)/broadcom/include

ifeq  ($(strip $(BRCM_DRIVER_CTF_DEBUG)),y)
EXTRA_CFLAGS += -DCTFDBG
endif
