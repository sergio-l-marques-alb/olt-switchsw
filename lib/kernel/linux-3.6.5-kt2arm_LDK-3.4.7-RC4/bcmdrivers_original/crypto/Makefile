# File: bcmdrivers/crypto/Makefile
#
# Makefile for Northstar+ crypto accel files
#

obj-$(CONFIG_IPROC_CRYPTO) := iproc_crypto.o
iproc_crypto-objs  :=  util.o spu.o cipher.o

EXTRA_CFLAGS += -I. -I$(BCMDRIVERS_DIR)/gmac/src/include -DBCMDRIVER
