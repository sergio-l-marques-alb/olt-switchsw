# File: bcmdrivers/fa2/Makefile
#
# Makefile for Northstar+ flow accelerator
#
obj-$(CONFIG_IPROC_FA2) := iproc_fa2.o
iproc_fa2-objs   :=  fa2.o fa2_tables.o fa2_if.o

EXTRA_CFLAGS += -I. -I$(BCMDRIVERS_DIR)/gmac/src/include -DBCMDRIVER
