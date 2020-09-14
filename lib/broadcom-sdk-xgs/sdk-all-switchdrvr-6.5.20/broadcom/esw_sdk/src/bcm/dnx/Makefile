#
# $Id: Makefile,v 1.4 Broadcom SDK $
# $Copyright: (c) 2020 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#

include ${SDK}/make/Make.config

ifndef ESW_CHIPS
CFGFLAGS += -std=gnu99 -D_SOC_MEMACC_H -D_SOC_MEMREGS_H -D_SOC_ALLFIELDS_H -D_SOC_INTENUM_H -D_SOC_DRV_H
else
CFGFLAGS += -std=gnu99
endif

#
# If this is compilation for SLK-ALL then optimize for code
# size. This is added to temporarily overcome the code size
# issue on SLK compiler.
#
ifeq ($(findstring OPTIMIZE_CODE,$(MAKE_CUSTOM_SLK_CODE_SIZE)),OPTIMIZE_CODE)
CFGFLAGS += -Os
endif

export CFGFLAGS

subdirs = \
        aacl    \
		algo    \
        bier    \
        bfd \
        cmn \
        cosq    \
        dnx_unavail \
        dram \
        extender \
        fabric  \
        failover \
        flow \
        field   \
        gtimer  \
        init    \
        instru  \
        ipmc    \
        kbp \
        l2  \
        l3  \
        time  \
        lif \
        link \
        mirror  \
        mdb     \
        mpls    \
        mpls_port   \
        mpls_l2vpn  \
        multicast   \
        oam \
        policer \
        ppp \
        ptp \
        port    \
        qos \
        rx  \
        sat \
        spb \
        stat \
        stg \
        stk \
        srv6    \
        switch  \
        tdm     \
        trunk   \
        tune    \
        tunnel  \
        tx  \
        vlan    \
        vswitch \
        pvt \
        vxlan \
	interrupt \
        synce \
        knet \
        sec

include ${SDK}/make/Make.subdirs
