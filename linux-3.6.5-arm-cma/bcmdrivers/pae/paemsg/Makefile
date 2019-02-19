EXTRA_CFLAGS += -I$(BCMDRIVERS_DIR)/pae/paemsg  
KBUILD_CFLAGS += -DBCMDRIVER -Dlinux

iproc_pae-objs := paemsg.o pae_regs.o pae_fw.o pae_log.o pae_cmds.o pae_stats.o pae_addr.o pae_arp.o pae_xor.o

# arp callback test
#EXTRA_CFLAGS += -DTEST_ARP_CALLBACK
#iproc_pae-objs += pae_test_callback.o

#EXTRA_CFLAGS += -DPAE_FAKE_SW_NAT64
#iproc_pae-objs += pae_sw_fake_nat64.o

obj-$(CONFIG_IPROC_PAE) := iproc_pae.o
