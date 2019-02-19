IPROC_PAE_IPSEC_CFLAGS = -I$(BCMDRIVERS_DIR)/pae/ipsec -I$(BCMDRIVERS_DIR)/pae/paemsg 

obj-$(CONFIG_IPROC_PAE_IPSEC) := iproc_pae_ipsec.o

CFLAGS_paeipsecmsg.o += $(IPROC_PAE_IPSEC_CFLAGS)
CFLAGS_paehostif.o   += $(IPROC_PAE_IPSEC_CFLAGS)
CFLAGS_notifier.o    += $(IPROC_PAE_IPSEC_CFLAGS)

iproc_pae_ipsec-objs := paeipsecmsg.o  paehostif.o notifier.o
