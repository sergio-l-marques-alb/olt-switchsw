/*
 * $Id: cmicsim.c,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * The part of PCID that simulates the cmic
 *
 * Requires:
 *
 * Provides:
 *     soc_internal_pcic_init
 *     soc_internal_read
 *     soc_internal_write
 *     soc_internal_reset
 *     soc_internal_vlan_tab_init
 *     soc_internal_memory_fetch
 *     soc_internal_memory_store
 *     soc_internal_manual_hash
 *     soc_internal_vlan_tab_init
 *     soc_internal_manual_hash
 *     soc_internal_send_int
 *     soc_internal_table_dma
 *     soc_internal_schan_ctrl_write
 */

#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>
#include <soc/mem.h>
#include <soc/hash.h>

#include <soc/cmic.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <bde/pli/verinet.h>

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"
#include "dma.h"


/*
 * PCI Config Space Simulation
 */

void
soc_internal_pcic_init(pcid_info_t *pcid_info, uint16 pci_dev_id,
                       uint16 pci_ven_id, uint8 pci_rev_id, uint32 pci_mbar0)
{
    /* PCI ID's translated at higher level */

    /* Device ID (16b), Vendor ID (16b) */
    PCIC(pcid_info, 0x00) = (pci_dev_id << 16) | pci_ven_id;

    /* Status (16b), Command (16b) */
    PCIC(pcid_info, 0x04) = 0x04800046;

    /* Class Code (24b), Rev ID (8b) */
    PCIC(pcid_info, 0x08) = 0x02800000 | pci_rev_id;

    /* BIST, Hdr Type, Lat Timer, Cache Line Size */
    PCIC(pcid_info, 0x0c) = 0x00002008;

    /* BAR0 */
    PCIC(pcid_info, 0x10) = pci_mbar0;

    /* BAR1 */
    PCIC(pcid_info, 0x14) = 0x00000000;

    /* BAR2 */
    PCIC(pcid_info, 0x18) = 0x00000000;

    /* BAR3 */
    PCIC(pcid_info, 0x1c) = 0x00000000;

    /* BAR4 */
    PCIC(pcid_info, 0x20) = 0x00000000;

    /* BAR5 */
    PCIC(pcid_info, 0x24) = 0x00000000;

    /* Cardbus CIS Pointer */
    PCIC(pcid_info, 0x28) = 0x00000000;

    /* Subsystem ID (16b), Subsystem Vendor ID (16b) */
    PCIC(pcid_info, 0x2c) = 0x00000000;

    /* Expansion ROM base addr */
    PCIC(pcid_info, 0x30) = 0x00000000;

    /* Reserved (24b), Capabilities Ptr (8b) */
    PCIC(pcid_info, 0x34) = 0x00000000;

    /* Reserved */
    PCIC(pcid_info, 0x38) = 0x00000000;

    /* Max_Lat, Min_Gnt, Int Pin, Int Line */
    PCIC(pcid_info, 0x3c) = 0x0000010b;

    /* Retry_Timeout, TRDY_Timeout */
    PCIC(pcid_info, 0x40) = 0x00008080;

    /* Retry_Timeout, TRDY_Timeout */
    PCIC(pcid_info, 0x44) = 0x00000000;
}

/*
 * Given a GBP address, adjust it so it wraps like the hardware would
 * behave, according to simulated GBP size and number of banks.
 */

static uint32
gbp_address_adjust(pcid_info_t *pcid_info, uint32 address)
{
    uint32          sfappoolsize[SOC_MAX_MEM_WORDS];
    int             size_shift;

    
    if (SOC_IS_TUCANA(pcid_info->unit)) {
        size_shift = 16;
    } else {

        soc_internal_read_reg(pcid_info,
                              soc_reg_addr(pcid_info->unit,
                                           SFAPPOOLSIZEr, REG_PORT_ANY, 0),
                              sfappoolsize);

        if (soc_reg_field_get(pcid_info->unit, SFAPPOOLSIZEr, sfappoolsize[0],
                              SDRAMSIZEf) & 1) {
            /* If simulating 4 banks, memory looks normal */
            if (pcid_info->opt_gbp_banks == 4)
                size_shift = 16;	       /* megabytes/16 */
            else {
                /* If simulating 2 banks, memory aliases in 128-entry blocks */
                /* and looks twice as bit */
                address &= ~0x80;	       /* Bit 7 goes away */
                size_shift = 17;	       /* megabytes*2/16 */
            }
        } else {
            /* If simulating 2 banks, memory looks normal */
            if (pcid_info->opt_gbp_banks == 2)
                size_shift = 16;	       /* megabytes/16 */
            else {
                /* If simulating 4 banks, memory looks half size */
                size_shift = 15;	       /* megabytes/2/16 */
            }
        }
    }

    return (0x12000000 |
            (address & ((pcid_info->opt_gbp_mb << size_shift) - 1)));
}

/* This is for STRATA chips (5600, 05, 15, 80) only */
static void
bist_sim_write(pcid_info_t *pcid_info, uint32 address, void *data)
{
    uint32	*data32 = (uint32 *) data;

    if (!SOC_IS_STRATA(pcid_info->unit)) {
        return;
    }
#ifdef BCM_5615
    if (SOC_CHIP_GROUP(pcid_info->unit) == SOC_CHIP_BCM5615 &&
        ((address & 0xfffffff4) == 0x380024 ||
         (address & 0xfffffff4) == 0x480024)) {		/* UL_BIST_REGx */
        if (*data32 & 1) {				/* INIT_BIST */
            *data32 = 3;				/* BIST_DONE */
            *data32 |= (random() % 3) << 2;		/* FAIL_CNT */
            *data32 |= (random() % 2048) << 6;		/* FAIL_ADDR0 */
            *data32 |= (random() % 2048) << 17;		/* FAIL_ADDR1 */
        } else {
            /* All bits read only except INIT_BIST */
            soc_internal_read_reg(pcid_info, address, data);
            data32[0] &= ~1;
        }
        /* Modified UL_BIST_REGx will be written upon return */
        return;
    }

    if (SOC_CHIP_GROUP(pcid_info->unit) == SOC_CHIP_BCM5615 &&
        (address == 0x080028 ||
         address == 0x180028 ||
         address == 0x280028)) {			/* FFP_BIST */
        if (*data32 & 1) {				/* INIT_BIST */
            *data32 = 3;				/* BIST_DONE */
            *data32 |= (random() % 3) << 2;		/* FAIL_CNT */
            *data32 |= (random() % 256) << 6;		/* FAIL_ADDR0 */
            *data32 |= (random() % 256) << 14;		/* FAIL_ADDR1 */
        } else {
            /* All bits read only except INIT_BIST */
            soc_internal_read_reg(pcid_info, address, data);
            data32[0] &= ~1;
        }
        /* Modified FFP_BIST will be written upon return */
        return;
    }
#endif	/* BCM_5615 */

    if ((address & 0xff0fffff) == 0x80000) {		/* CONFIG */
        uint32		val[SOC_MAX_MEM_WORDS];
        uint32		new_config;

        /* Desired new value */
        new_config = *data32;

        /* Current old value */
        soc_internal_read_reg(pcid_info, address, data);

        if (new_config & 0x200) {			/* IN_BIST<0> */
            data32[0] |= 0x800;				/* BIST_DONE<0> */
            val[0] = ((random() % 0x4000) |		/* ADDR1 */
                      (random() % 0x4000) << 14 |	/* ADDR2 */
                      (random() % 3) << 28);		/* FCNT */
            soc_internal_write_reg(pcid_info, (address & 0xf80000) | 0x12,
                                   val);		/* EPC_BIST1 */
        }

        if (new_config & 0x400) {			/* IN_BIST<1> */
            data32[0] |= 0x1000;			/* BIST_DONE<1> */
            val[0] = ((random() % 0x4000) |		/* ADDR1 */
                      (random() % 0x4000) << 14 |	/* ADDR2 */
                      (random() % 3) << 28);		/* FCNT */
            soc_internal_write_reg(pcid_info, (address & 0xf80000) | 0x11,
                                   val);		/* EPC_BIST2 */
        }

        /* The BIST_DONE part of the register must be maintained */

        data32[0] = (new_config & ~0x1800) | (data32[0] & 0x1800);

        /* Modified CONFIG will be written upon return */
        return;
    }

    if ((SOC_CHIP_GROUP(pcid_info->unit) == SOC_CHIP_BCM5680 &&
	 address == 0x880110) ||
        (SOC_CHIP_GROUP(pcid_info->unit) != SOC_CHIP_BCM5680 &&
	 address == 0x680110)) {
        /*
         * CBP BIST completes automatically because the INITIATEBISTx
         * register indicates the DONE status when read.  User will
         * write base memory address; change this write data to
         * simulated response data.
         */

        *data32 = ((random() % 0x4000) |	/* DEADADDRESS1 */
                   (random() % 0x4000) << 14 |	/* DEADADDRESS2 */
                   (random() % 3) << 28);	/* DEADADDRESSCOUNT */
    }
}

static soc_mem_t
addr_to_mem(int unit, uint32 address)
{
    soc_mem_t       mem;
    uint32          offset, min_addr, max_addr; 

    offset = address & ~0x00f00000; /* Strip block ID */
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (soc_mem_is_valid(unit, mem) &&
            SOC_MEM_INFO(unit, mem).blocks != 0) {
            min_addr = max_addr = SOC_MEM_INFO(unit, mem).base;
            min_addr += SOC_MEM_INFO(unit, mem).index_min;
            max_addr += SOC_MEM_INFO(unit, mem).index_max;
            if (offset >= min_addr && offset <= max_addr) {
                if (SOC_IS_XGS3_SWITCH(unit)) {
                    /* Match block */
                    if (((address >> SOC_BLOCK_BP) & 0xf) !=
                        SOC_BLOCK2OFFSET(unit, SOC_MEM_BLOCK_ANY(unit, mem))) {
                        continue;
                    }
                }
                return mem;
            }
        }
    }

    return INVALIDm;
}

int
soc_internal_write_reg(pcid_info_t *pcid_info, uint32 address, uint32 *data)
{
    int             index;
    soc_datum_t    *d;
    soc_regaddrinfo_t ainfo;
    uint64 mask;

    soc_regaddrinfo_get(pcid_info->unit, &ainfo, address);

    if (!ainfo.valid || ainfo.reg < 0) {
        /* One hidden register recognized for Draco */
        if (!SOC_IS_DRACO(pcid_info->unit) || address != 0xe00010) {
            debugk(DK_WARN,
                   "Attempt to write to unknown S-channel address 0x%x\n",
                   address);
        }
    } else if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
        SOC_REG_INFO(pcid_info->unit, ainfo.reg).flags &
               SOC_REG_FLAG_RO) {
        if (ainfo.reg != HASH_OUTPUTr) {
            debugk(DK_WARN,
                   "Attempt to write to read-only S-channel address 0x%x\n",
                   address);
        }
    }

    bist_sim_write(pcid_info, address, data);

#if defined(BCM_FIREBOLT_SUPPORT)
    /*
    ** Map EPIPE_HI/IPIPE_HI port registers to EPIPE/IPIPE block
    */
    if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
        int blk;
        int unit = pcid_info->unit;
        blk = SOC_BLOCK2OFFSET(unit, IPIPE_HI_BLOCK(unit)) << 20;
        if (((address & 0x00f80000) == blk)
            && ((address & 0x0001f000) != (CMIC_PORT(unit) << 12)))    {
            if ((ainfo.reg == ICOS_SELr) ||
                (ainfo.reg == IUNKNOWN_UCAST_BLOCK_MASKr) ||
                (ainfo.reg == IUNKNOWN_MCAST_BLOCK_MASKr) ||
                (ainfo.reg == IBCAST_BLOCK_MASKr) ||
                (ainfo.reg == IEMIRROR_CONTROLr) ||
                (ainfo.reg == IMIRROR_CONTROLr) ||
#if defined(BCM_RAPTOR_SUPPORT)
                (ainfo.reg == IUNKNOWN_UCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IUNKNOWN_MCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IBCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IEMIRROR_CONTROL_HIr) ||
#endif /* BCM_RAPTOR_SUPPORT */
                0) {
                blk = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit)) << 20;
                address = (address & ~(0x00f00000)) | blk;
            }
        }
        blk = SOC_BLOCK2OFFSET(unit, EPIPE_HI_BLOCK(unit)) << 20;
        if (((address & 0x00f80000) == blk)
            && ((address & 0x0001f000) != (CMIC_PORT(unit) << 12)))    {
            blk = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit)) << 20;
            address = (address & ~(0x00f00000)) | blk;
        }
    }
#endif	/* BCM_FIREBOLT_SUPPORT */
    index = SOC_HASH_DATUM(address);
    for (d = pcid_info->reg_ht[index]; d != NULL; d = d->next) {
        if (d->addr == address) {
            memcpy(d->data, data, 8);
            return 0;
        }
    }

    if ((!SOC_COUNTER_INVALID(pcid_info->unit, ainfo.reg)) &&
        (SOC_REG_IS_COUNTER(pcid_info->unit, ainfo.reg))) {
        COMPILER_64_SET(mask, 0, 1);
        COMPILER_64_SHL(mask,SOC_REG_INFO(pcid_info->unit,
					  ainfo.reg).fields[0].len);
        COMPILER_64_SUB_32(mask, 1);
        data[1] &= COMPILER_64_HI(mask);
        data[0] &= COMPILER_64_LO(mask);
    }

    d = sal_alloc(sizeof(soc_datum_t), "rdatum_t");
    d->addr = address;
    memcpy(d->data, data, 8);
    d->next = pcid_info->reg_ht[index];
    pcid_info->reg_ht[index] = d;

    return 0;
}

int
soc_internal_read_reg(pcid_info_t *pcid_info, uint32 address, uint32 *data)
{
    int             index;
    soc_datum_t    *d;
    soc_regaddrinfo_t ainfo;

    soc_regaddrinfo_get(pcid_info->unit, &ainfo, address);

    if (!ainfo.valid || ainfo.reg < 0) {
        debugk(DK_ERR,
               "Attempt to read from unknown S-channel address 0x%x\n",
               address);
    } else if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
        SOC_REG_INFO(pcid_info->unit, ainfo.reg).flags & SOC_REG_FLAG_WO) {
        debugk(DK_WARN,
               "Attempt to read from write-only S-channel address 0x%x\n",
               address);
    }

#ifdef BCM_EASYRIDER_SUPPORT
    /*
    ** 
    */
    if (SOC_IS_EASYRIDER(pcid_info->unit)) {
        int blk;
        int unit = pcid_info->unit;
        uint32 mdata[SOC_MAX_MEM_WORDS];
        blk = SOC_BLOCK2OFFSET(unit, HSE_BLOCK(unit)) << 20;
        if ((address & 0x00f00000) == blk) {
            if (ainfo.reg == CMDWORD_SHADOW_HSEr) {
                soc_internal_read_mem(pcid_info,
                    soc_mem_addr(pcid_info->unit, COMMAND_MEMORY_HSEm,
                        SOC_MEM_BLOCK_ANY(pcid_info->unit,
                                          COMMAND_MEMORY_HSEm), 0),
                                      mdata);

                data[0] = soc_mem_field32_get(pcid_info->unit,
                                              COMMAND_MEMORY_HSEm, mdata,
                                              COMMAND_WORDf);
                return 0;
            }
        }
    }
#endif	/* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
    /*
    ** Map EPIPE_HI/IPIPE_HI port registers to EPIPE/IPIPE block
    ** Well not really. Some of the registers are Higig only.
    */
    if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
        int blk;
        int unit = pcid_info->unit;
        blk = SOC_BLOCK2OFFSET(unit, IPIPE_HI_BLOCK(unit)) << 20;
        if (((address & 0x00f80000) == blk)
            && ((address & 0x0001f000) != (CMIC_PORT(unit) << 12)))    {
            if ((ainfo.reg == ICOS_SELr) ||
                (ainfo.reg == IUNKNOWN_UCAST_BLOCK_MASKr) ||
                (ainfo.reg == IUNKNOWN_MCAST_BLOCK_MASKr) ||
                (ainfo.reg == IBCAST_BLOCK_MASKr) ||
                (ainfo.reg == IEMIRROR_CONTROLr) ||
                (ainfo.reg == IMIRROR_CONTROLr) ||
#if defined(BCM_RAPTOR_SUPPORT)
                (ainfo.reg == IUNKNOWN_UCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IUNKNOWN_MCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IBCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IEMIRROR_CONTROL_HIr) ||
#endif /* BCM_RAPTOR_SUPPORT */
                0) {
                blk = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit)) << 20;
                address = (address & ~(0x00f00000)) | blk;
                soc_regaddrinfo_get(pcid_info->unit, &ainfo, address);
            }
        }
        blk = SOC_BLOCK2OFFSET(unit, EPIPE_HI_BLOCK(unit)) << 20;
        if (((address & 0x00f80000) == blk)
            && ((address & 0x0001f000) != (CMIC_PORT(unit) << 12)))    {
            blk = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit)) << 20;
            address = (address & ~(0x00f00000)) | blk;
            soc_regaddrinfo_get(pcid_info->unit, &ainfo, address);
        }
    }
#endif	/* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT)
    /*
    ** Patch over array registers with different reset values
    */
    if (SOC_IS_FIREBOLT2(pcid_info->unit)) {
        if (ainfo.reg == EGR_OUTER_TPIDr) {
            switch (address & 0x3) {
            case 0:
                ainfo.reg = EGR_OUTER_TPID_0r;
                break;
            case 1:
                ainfo.reg = EGR_OUTER_TPID_1r;
                break;
            case 2:
                ainfo.reg = EGR_OUTER_TPID_2r;
                break;
            case 3:
                ainfo.reg = EGR_OUTER_TPID_3r;
                break;
            default:
                break;
            }
            ainfo.idx = -1;
        }
        if (ainfo.reg == ING_OUTER_TPIDr) {
            switch (address & 0x3) {
            case 0:
                ainfo.reg = ING_OUTER_TPID_0r;
                break;
            case 1:
                ainfo.reg = ING_OUTER_TPID_1r;
                break;
            case 2:
                ainfo.reg = ING_OUTER_TPID_2r;
                break;
            case 3:
                ainfo.reg = ING_OUTER_TPID_3r;
                break;
            default:
                break;
            }
            ainfo.idx = -1;
        }
    }
#endif	/* BCM_FIREBOLT2_SUPPORT */
    index = SOC_HASH_DATUM(address);
    for (d = pcid_info->reg_ht[index]; d != NULL; d = d->next) {
        if (d->addr == address) {
            memcpy(data, d->data, 8);
#if defined(BCM_FIREBOLT_SUPPORT)
            if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
                /*
                 * Reading ING_HW_RESET_CONTROL_2/EGR_HW_RESET_CONTROL_1
                 * returns DONE=1
                 */
                if (address == 0x780002) {	/* ING_HW_RESET_CONTROL_2 */
                        data[0] |= (1 << 18);
                }
                if (address == 0x980001) {	/* EGR_HW_RESET_CONTROL_1 */
                        data[0] |= (1 << 18);
                }
            }
            else if (SOC_IS_HB_GW(pcid_info->unit)) {
                /*
                 * Reading ING_HW_RESET_CONTROL_2/EGR_HW_RESET_CONTROL_1
                 * returns DONE=1
                 */
                if (address == 0x180203) {	/* ING_HW_RESET_CONTROL_2_X */
                        data[0] |= (1 << 18);
                }
                if (address == 0x180403) {	/* ING_HW_RESET_CONTROL_2_Y */
                        data[0] |= (1 << 18);
                }
                if (address == 0x180603) {	/* ING_HW_RESET_CONTROL_2 */
                        data[0] |= (1 << 18);
                }
                if (address == 0x480601) {	/* EGR_HW_RESET_CONTROL_1 */
                        data[0] |= (1 << 18);
                }
            }
#endif	/* BCM_FIREBOLT_SUPPORT */
            return 0;
        }
    }

    /*
     * The register is not in the hash table, so it has not been written
     * before.  If this is a recognizable register address, use its
     * reset default value, otherwise use all F's.
     */

    if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
            (ainfo.valid && ainfo.reg >= 0)) {
        data[0] = SOC_REG_INFO(pcid_info->unit, ainfo.reg).rst_val_lo;
        data[1] = SOC_REG_INFO(pcid_info->unit, ainfo.reg).rst_val_hi;
    } else {
        data[0] = 0xffffffff;
        data[1] = 0xffffffff;
    }
#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT)
    if (SOC_IS_FX_HX(pcid_info->unit)) {
        if (address == 0xb80210) {	/* BSAFE_GLB_DEV_STATUS */
            data[0] |= (1 << 7);    /* PROD_CFG_VLD */
        }
        if (address == 0xb8020c) {	/* BSAFE_GLB_PROD_CFG */
            data[0] |= (pcid_info->opt_gbp_wid << 8); /* LMD_ENABLE */
        }
    }
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT */
#ifdef	BCM_XGS_FABRIC_SUPPORT
    if (SOC_IS_HERCULES1(pcid_info->unit) &&
        (address & 0xff0fffff) == 0x00000002) {
        /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
        data[0] |= 0x00000008;
    }
    if (SOC_IS_HERCULES15(pcid_info->unit) &&
        (address & 0xff0fffff) == 0x00000202) {
        /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
        data[0] |= 0x00000008;
    }
#endif	/* BCM_XGS_FABRIC_SUPPORT */

#ifdef	BCM_DRACO_SUPPORT
    if ((SOC_IS_DRACO(pcid_info->unit) || SOC_IS_TUCANA(pcid_info->unit)) &&
        (address == 0xc00022)) {
        /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
        data[0] |= 0x00000008;
    }
#endif	/* BCM_DRACO_SUPPORT */

#ifdef	BCM_LYNX_SUPPORT
    if (SOC_IS_LYNX(pcid_info->unit) &&
        (address == 0x00000202 ||	/* MAC_XGXS_STAT.XE0 */
         address == 0x00100202)) {	/* MAX_XGXS_STAT.HG0 */
        /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
        data[0] |= 0x00000008;
    }
#endif	/* BCM_LYNX_SUPPORT */

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(pcid_info->unit) &&
        (address == 0x00100002 ||	/* MAC_XGXS_STAT.XE0 */
         address == 0x00200002)) {	/* MAX_XGXS_STAT_XE3 */
        /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
        data[0] |= 0x00000008;
    }
#endif	/* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FB(pcid_info->unit) ||
        SOC_IS_HELIX(pcid_info->unit) ||
        SOC_IS_FELIX(pcid_info->unit)) {
        if (address == 0x00200002 ||	/* MAC_XGXS_STAT.XE0 */
             address == 0x00300002 ||	/* MAC_XGXS_STAT.XE1 */
             address == 0x00400002 ||	/* MAC_XGXS_STAT.XE2 */
             address == 0x00500002) {	/* MAX_XGXS_STAT_XE3 */
            /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
            data[0] |= 0x00000008;
        }
    }
#endif	/* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(pcid_info->unit)) {
        if ((address & 0xfff00fff) == 0x000002) { /* MAX_XGXS_STAT */
            /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
            data[0] |= 0x00000020;
        }
    }
#endif	/* BCM_BRADLEY_SUPPORT */

    return 0;
}

int
soc_internal_write_mem(pcid_info_t *pcid_info, uint32 address, uint32 *data)
{
    int             index;
    soc_datum_t    *d;
    soc_mem_t       mem;

    if ((mem = addr_to_mem(pcid_info->unit, address)) != INVALIDm) {
#ifdef	BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
                /*
                 * Map EPIPE_HI/IPIPE_HI port registers to EPIPE/IPIPE block
                 */
                int blk;
                int unit = pcid_info->unit;
                int cpu_port = CMIC_PORT(unit);
                blk = SOC_BLOCK2OFFSET(unit, IPIPE_HI_BLOCK(unit)) << 20;
                if (((address & 0x00f00000) == blk)
                    && ((address & 0x0000001f) != cpu_port )
                    && (mem != IFP_PORT_FIELD_SELm))    {
                    blk = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit)) << 20;
                    address = (address & ~(0x00f00000)) | blk;
                }
                blk = SOC_BLOCK2OFFSET(unit, EPIPE_HI_BLOCK(unit)) << 20;
                if (((address & 0x00f00000) == blk)
                    && ((address & 0x0000001f) != cpu_port))    {
                    blk = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit)) << 20;
                    address = (address & ~(0x00f00000)) | blk;
                }
            }

            switch(mem) {
                case L3_ENTRY_IPV4_UNICASTm:
                case L3_ENTRY_IPV4_MULTICASTm:
                case L3_ENTRY_IPV6_UNICASTm:
                case L3_ENTRY_IPV6_MULTICASTm:
                    return (soc_internal_l3x2_write(pcid_info, mem,
                                                    address, data));
                default: break;
            }
        }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef	BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(pcid_info->unit)) {
            switch(mem) {
            case COMMAND_MEMORY_BSEm:
            case COMMAND_MEMORY_CSEm:
            case COMMAND_MEMORY_HSEm:
                return (soc_internal_mcmd_write(pcid_info, mem, data, TRUE));
                default: break;
            }
        }
#endif /* BCM_EASYRIDER_SUPPORT */

        if  (soc_mem_is_aggr(pcid_info->unit, mem)) {
            return soc_internal_write_aggrmem(pcid_info, address, data);
        }
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS_SWITCH(pcid_info->unit) &&
        !SOC_IS_XGS3_SWITCH(pcid_info->unit)) {
        uint32	arl_block, l3xaddr;

        arl_block = 0x00e00000;
        l3xaddr = 0x05090000;
        if (SOC_IS_LYNX(pcid_info->unit)) {
            arl_block = 0x00300000;
        }
        if (SOC_IS_DRACO15(pcid_info->unit)) {
            l3xaddr = 0x05080000;
        }
        if ((address & 0xffff0000) == (0x01050000 | arl_block)) {
            debugk(DK_WARN, "Attempt to write R/O memory L2X\n");
            return -1;
        }
        if ((address & 0xffff0000) == (l3xaddr | arl_block)) {
            debugk(DK_WARN, "Attempt to write R/O memory L3X\n");
            return -1;
        }
        if (SOC_IS_DRACO15(pcid_info->unit)) {
            if ((address & 0xffff0000) == (0x05090000 | arl_block)) {
                debugk(DK_WARN, "Attempt to write R/O memory DEFIP_ALL\n");
                return -1;
            }
        } else {
            if ((address & 0xffff0000) == (0x050a0000 | arl_block)) {
                debugk(DK_WARN, "Attempt to write R/O memory DEFIP_HI_ALL\n");
                return -1;
            }
            if ((address & 0xffff0000) == (0x050b0000 | arl_block)) {
                debugk(DK_WARN, "Attempt to write R/O memory DEFIP_LO_ALL\n");
                return -1;
            }
        }
        if (address == (0x06000000 | arl_block)) {
            /* This is the hash input memory */
            soc_internal_manual_hash(pcid_info, data);
        }
    }
#endif

    if ((!SOC_IS_XGS3_SWITCH(pcid_info->unit)) &&
        ((address & 0xff000000) == 0x12000000)) {
        address = gbp_address_adjust(pcid_info, address);
        /* Simulate 64-bit wide GBP by putting junk in missing bus bits */
        if (pcid_info->opt_gbp_wid == 64) {
            data[2] = data[3] = 0xdeadbeef;
        }
    }

    index = SOC_HASH_DATUM(address);

    for (d = pcid_info->mem_ht[index]; d != NULL; d = d->next) {
        if (d->addr == address) {
            memcpy(d->data, data, SOC_MAX_MEM_WORDS * 4);
            return 0;
        }
    }

    d = sal_alloc(sizeof(soc_datum_t), "mdatum_t");
    d->addr = address;
    memcpy(d->data, data, SOC_MAX_MEM_WORDS * 4);
    d->next = pcid_info->mem_ht[index];
    pcid_info->mem_ht[index] = d;

    return 0;
}

int
soc_internal_read_mem(pcid_info_t *pcid_info, uint32 address, uint32 *data)
{
    int             i, index;
    soc_datum_t    *d;
    soc_mem_t       mem;

    if ((mem = addr_to_mem(pcid_info->unit, address)) != INVALIDm) {
#ifdef	BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
                /*
                 * Map EPIPE_HI/IPIPE_HI port registers to EPIPE/IPIPE block
                 */
                int blk;
                int unit = pcid_info->unit;
                int cpu_port = CMIC_PORT(unit);
                blk = SOC_BLOCK2OFFSET(unit, IPIPE_HI_BLOCK(unit)) << 20;
                if (((address & 0x00f00000) == blk)
                    && ((address & 0x0000001f) != cpu_port )
                    && (mem != IFP_PORT_FIELD_SELm))    {
                    blk = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit)) << 20;
                    address = (address & ~(0x00f00000)) | blk;
                }
                blk = SOC_BLOCK2OFFSET(unit, EPIPE_HI_BLOCK(unit)) << 20;
                if (((address & 0x00f00000) == blk)
                    && ((address & 0x0000001f) != cpu_port))    {
                    blk = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit)) << 20;
                    address = (address & ~(0x00f00000)) | blk;
                }
            }

            switch(mem) {
                case L3_ENTRY_IPV4_UNICASTm:
                case L3_ENTRY_IPV4_MULTICASTm:
                case L3_ENTRY_IPV6_UNICASTm:
                case L3_ENTRY_IPV6_MULTICASTm:
                    return (soc_internal_l3x2_read(pcid_info, mem,
                                                   address, data));
                default: break;
            }
        }
#endif /* BCM_FIREBOLT_SUPPORT */

        if  (soc_mem_is_aggr(pcid_info->unit, mem)) {
            return soc_internal_read_aggrmem(pcid_info, address, data);
        }
    }

#ifdef	BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS_SWITCH(pcid_info->unit) &&
        !SOC_IS_XGS3_SWITCH(pcid_info->unit)) {
        uint32	arl_block, l3xaddr;

        arl_block = 0x00e00000;
        l3xaddr = 0x05090000;
        if (SOC_IS_LYNX(pcid_info->unit)) {
            arl_block = 0x00300000;
        }
        if (SOC_IS_DRACO15(pcid_info->unit)) {
            l3xaddr = 0x05080000;
        }
        if ((address & 0xffff0000) == (0x01050000 | arl_block)) {
            return soc_internal_l2x_read(pcid_info, address,
                                         (l2x_entry_t *)data);
        }
        if ((address & 0xffff0000) == (l3xaddr | arl_block)) {
            return soc_internal_l3x_read(pcid_info, address,
                                         (l3x_entry_t *)data);
        }
    }
#endif	/* BCM_XGS_SWITCH_SUPPORT */

    if (!(SOC_IS_HERCULES15(pcid_info->unit)) &&
        (!SOC_IS_XGS3_SWITCH(pcid_info->unit)) &&
        ((address & 0xff000000) == 0x12000000)) {
        address = gbp_address_adjust(pcid_info, address);
    }

    index = SOC_HASH_DATUM(address);

    for (d = pcid_info->mem_ht[index]; d != NULL; d = d->next) {
        if (d->addr == address) {
            memcpy(data, d->data,
		   (SOC_MAX_MEM_WORDS * 4));
	    goto found;
        }
    }

    /*
     * The register/memory has not been written before so it is not in
     * the hash table.  If this is a recognizable register address, use
     * its reset default value; if it's a memory, use the null entry;
     * otherwise, use 0xf's.
     */

    if ((mem = addr_to_mem(pcid_info->unit, address)) != INVALIDm) {
        memcpy(data,		       /* Memory */
               soc_mem_entry_null(pcid_info->unit, mem),
               4 * soc_mem_entry_words(pcid_info->unit, mem));
    } else {			       /* Other */
        for (i = 0; i < SOC_MAX_MEM_WORDS; i++) {
            data[i] = 0xffffffff;
        }
    }

 found:

#ifdef	BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(pcid_info->unit)) {
	/* Simulate Tucana missing the FSEL field */
	if ((address & 0xff0ff000) == 0x07020000) {
	    /*
	     * IRULE.x or GIRULE.x (the FSEL field happens to be in the
	     * same bit positions for each).
	     */
	    soc_mem_field32_set(pcid_info->unit, mem, data, FSELf, 0);
	}
    }
#endif /* BCM_TUCANA_SUPPORT */

    return 0;
}


/*
** This is a generic function to handle aggregate memories. Memories
** made up of unequal depth and field mismatch between unified view
** and individual component memories require special handling.
** For example L2 table has HIT bits which are packet into 1/8 size table
*/
int
soc_internal_read_aggrmem(pcid_info_t *pcid_info, uint32 addr, uint32 *data)
{
    uint32      ent[SOC_MAX_MEM_WORDS];
    uint32      fv[SOC_MAX_MEM_WORDS];
    soc_mem_t   *m = NULL;
    int         index, i;
    int         offset;
    int         adj_offset = 0;
    int         f;
    int         nfield;
    int         pck_factor;
    soc_mem_t   mem;
    soc_mem_info_t *amemp;
    soc_mem_info_t *memp;
    soc_field_info_t *fieldp;
    int         unit = pcid_info->unit;


    mem = addr_to_mem(pcid_info->unit, addr);
    offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0xfffff;
    index = 0;
    memset(data, 0, 4 * soc_mem_entry_words(unit, mem));
    do {
        m = SOC_MEM_AGGR(unit, index);
        if (m[0] == mem) {
            /* Fetch the individual components */
            nfield = 1;
            for(i = 1; m[i] != INVALIDm; i++) {
                /*
                ** Multiple entries compacted into one entry like HIT
                ** requires special handling.
                */
                amemp = &SOC_MEM_INFO(unit, mem);
                pck_factor = (soc_mem_index_max(unit, mem) /
                              soc_mem_index_max(unit, m[i]));
                adj_offset = offset / pck_factor;
                soc_internal_read_mem(pcid_info,
                        soc_mem_addr(unit, m[i],
                                SOC_MEM_BLOCK_ANY(unit, m[i]),
                                adj_offset), ent);

                /* Iterate thru' the fields in this View */
                memp = &SOC_MEM_INFO(unit, m[i]);
                if (pck_factor == 1) {
                    for (f = memp->nFields - 1; f >= 0; f--) {
                        fieldp = &memp->fields[f];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        soc_mem_field_set(unit, mem, data, fieldp->field, fv);
                        nfield++;
                    }
                } else if (memp->nFields == pck_factor) {
                        fieldp = &memp->fields[memp->nFields -
                                    (offset % pck_factor) - 1];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        fieldp = &amemp->fields[(amemp->nFields - nfield)];
                        soc_mem_field_set(unit, mem, data, fieldp->field, fv);
                        nfield++;
                } else {
                    debugk(DK_ERR,
                           "soc_internal_read_aggrmem:mem=%d"
                           "Unable to handle Aggregate Mem component %d\n",
                           mem, m[i]);
                }
            }
            return(0);
        }
        index++;
    } while (m[0] != INVALIDm);

    /* Not found in the table of aggregate memories */
    debugk(DK_ERR,
           "soc_internal_read_aggrmem:mem=%d missing in Aggregate Mem List\n",
           mem);
    memcpy(data,		       /* Memory */
           soc_mem_entry_null(unit, mem),
           4 * soc_mem_entry_words(unit, mem));
    return(0);
}

int
soc_internal_write_aggrmem(pcid_info_t *pcid_info, uint32 addr, uint32 *data)
{
    uint32      ent[SOC_MAX_MEM_WORDS];
    uint32      fv[SOC_MAX_MEM_WORDS];
    soc_mem_t   *m = NULL;
    int         index, i;
    int         offset;
    int         adj_offset = 0;
    int         f;
    int         nfield;
    int         pck_factor;
    soc_mem_t   mem;
    soc_mem_info_t *amemp;
    soc_mem_info_t *memp;
    soc_field_info_t *fieldp;
    int         unit = pcid_info->unit;

    mem = addr_to_mem(pcid_info->unit, addr);
    memset(ent, 0, 4 * soc_mem_entry_words(unit, mem));
    offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0xfffff;
    index = 0;
    do {
        m = SOC_MEM_AGGR(unit, index);
        if (m[0] == mem) {
            /* Fetch the individual components */
            nfield = 1;
            for(i = 1; m[i] != INVALIDm; i++) {
                /*
                ** Multiple entries compacted into one entry like HIT
                ** requires special handling.
                */
                amemp = &SOC_MEM_INFO(unit, mem);
                pck_factor = (soc_mem_index_max(unit, mem) /
                              soc_mem_index_max(unit, m[i]));
                adj_offset = offset / pck_factor;
                memp = &SOC_MEM_INFO(unit, m[i]);

                /* Iterate thru' the fields in this View */
                if (pck_factor == 1) {
                    for (f = memp->nFields - 1; f >= 0; f--) {
                        fieldp = &memp->fields[f];
                        soc_mem_field_get(unit, mem, data, fieldp->field, fv);
                        soc_mem_field_set(unit, m[i], ent, fieldp->field, fv);
                        nfield++;
                    }
                } else if (memp->nFields == pck_factor) {
                        fieldp = &amemp->fields[(amemp->nFields - nfield)];
                        soc_mem_field_get(unit, mem, data, fieldp->field, fv);
                        fieldp = &memp->fields[memp->nFields -
                                    (offset % pck_factor) - 1];
                        soc_internal_read_mem(pcid_info,
                                soc_mem_addr(unit, m[i],
                                        SOC_MEM_BLOCK_ANY(unit, m[i]),
                                        adj_offset), ent);
                        soc_mem_field_set(unit, m[i], ent, fieldp->field, fv);
                        nfield++;
                } else {
                    debugk(DK_ERR,
                           "soc_internal_read_aggrmem:mem=%d"
                           "Unable to handle Aggregate Mem component %d\n",
                           mem, m[i]);
                }
                soc_internal_write_mem(pcid_info,
                                soc_mem_addr(unit, m[i],
                                    SOC_MEM_BLOCK_ANY(unit, m[i]),
                                    adj_offset), ent);
            }
            return(0);
        }
        index++;
    } while (m[0] != INVALIDm);

    /* Not found in the table of aggregate memories */
    debugk(DK_ERR,
           "soc_internal_write_aggrmem:mem=%d missing in Aggregate Mem List\n",
           mem);
    return(0);
}

/*
 * Draco VLAN hardware table initialization (clear all valid bits)
 */

int
soc_internal_vlan_tab_init(pcid_info_t *pcid_info)
{
    int index, index_min, index_max;

    debugk(DK_VERBOSE, "VLAN_TAB Init\n");

    index_min = soc_mem_index_min(pcid_info->unit, VLAN_TABm);
    index_max = soc_mem_index_max(pcid_info->unit, VLAN_TABm);

    for (index = index_min; index <= index_max; index++) {
        soc_internal_write_mem(pcid_info,
                               soc_mem_addr(pcid_info->unit, VLAN_TABm, 0,
					    index),
                               soc_mem_entry_null(pcid_info->unit, VLAN_TABm));
    }

    return 0;
}

#ifdef	BCM_FIREBOLT_SUPPORT
void
_soc_l2x2_nak_check(int unit, schan_msg_t    *msg)
{
    int             op_fail_pos;
    /*
     * ==================================================
     * | MFIFO_FULL | OP_FAIL | Index  | L2x entry data |
     * ==================================================
     */
    op_fail_pos = (_shr_popcount(soc_mem_index_max(unit, L2Xm)) +
                   soc_mem_entry_bits(unit, L2Xm)) % 32;

    if (msg->readresp.data[3] & (0x3 << op_fail_pos)) {
        msg->readresp.header.cpu = 1;
    }
}
#endif	/* BCM_FIREBOLT_SUPPORT */

/*
 * schan_op
 *
 * Look at schan header, perform action(s), and make response.
 */

static int
schan_op(pcid_info_t *pcid_info, int unit)
{
    schan_msg_t    *msg;
    int             rv = -1;
#ifdef	BCM_FIREBOLT_SUPPORT
    uint8           banks = 0;
#endif

    msg = (schan_msg_t *)&PCIM(pcid_info, CMIC_SCHAN_MESSAGE(unit, 0));

    /* Clear nak bit in schan control */
    PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST;

    switch (msg->header.opcode) {
    case WRITE_REGISTER_CMD_MSG:
        debugk(DK_VERBOSE,
               "S-Channel operation: WRITE_REGISTER_CMD 0x%x\n",
               msg->writecmd.address);
        rv = soc_internal_write_reg(pcid_info, msg->writecmd.address,
                                    msg->writecmd.data);
        break;
    case READ_REGISTER_CMD_MSG:
        debugk(DK_VERBOSE,
               "S-Channel operation: READ_REGISTER_CMD 0x%x\n",
               msg->readcmd.address);
        msg->readresp.header.opcode = READ_REGISTER_ACK_MSG;
        rv = soc_internal_read_reg(pcid_info, msg->readcmd.address,
                                   msg->readresp.data);
        break;
    case WRITE_MEMORY_CMD_MSG:
        debugk(DK_VERBOSE,
               "S-Channel operation: WRITE_MEMORY_CMD 0x%x\n",
               msg->writecmd.address);
        rv = soc_internal_write_mem(pcid_info, msg->writecmd.address,
                                    msg->writecmd.data);
        break;
    case READ_MEMORY_CMD_MSG:
        debugk(DK_VERBOSE,
               "S-Channel operation: READ_MEMORY_CMD 0x%x\n",
               msg->readcmd.address);
        msg->readresp.header.opcode = READ_MEMORY_ACK_MSG;
        rv = soc_internal_read_mem(pcid_info, msg->readcmd.address,
                                   msg->readresp.data);
        break;
    case ARL_INSERT_CMD_MSG:
        debugk(DK_VERBOSE, "S-Channel operation: ARL_INSERT_CMD_MSG\n");
#ifdef	BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            msg->readresp.header.opcode = ARL_INSERT_DONE_MSG;
            banks =  msg->l2x2.header.cos & 0x3;
            rv = soc_internal_l2x2_entry_ins(pcid_info, banks,
                                      (l2x_entry_t *)msg->l2x2.data,
                                      msg->readresp.data);
            _soc_l2x2_nak_check(pcid_info->unit, msg);
            break;
        }
#endif	/* BCM_FIREBOLT_SUPPORT */
#ifdef	BCM_XGS_SWITCH_SUPPORT
        if (SOC_IS_XGS_SWITCH(pcid_info->unit)) {
            rv = soc_internal_l2x_ins(pcid_info,
                                      (l2x_entry_t *)msg->arlins.data);
            break;
        }
#endif	/* BCM_XGS_SWITCH_SUPPORT */
        if (SOC_IS_XGS_FABRIC(pcid_info->unit)) {
            debugk(DK_WARN, "Bad call for Hercules: ARL_INSERT_CMD_MSG\n");
            break;
        }
        rv = soc_internal_arl_ins(pcid_info,
                                  (arl_entry_t *)msg->arlins.data);
        break;
    case ARL_DELETE_CMD_MSG:
        debugk(DK_VERBOSE, "S-Channel operation: ARL_DELETE_CMD_MSG\n");
#ifdef	BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            msg->readresp.header.opcode = ARL_DELETE_DONE_MSG;
            banks = msg->l2x2.header.cos & 0x3;
            rv = soc_internal_l2x2_entry_del(pcid_info, banks,
                                      (l2x_entry_t *)msg->l2x2.data,
                                      msg->readresp.data);
            _soc_l2x2_nak_check(pcid_info->unit, msg);
            break;
        }
#endif	/* BCM_FIREBOLT_SUPPORT */
#ifdef	BCM_XGS_SWITCH_SUPPORT
        if (SOC_IS_XGS_SWITCH(pcid_info->unit)) {
            rv = soc_internal_l2x_del(pcid_info,
                                      (l2x_entry_t *)msg->arlins.data);
            break;
        }
#endif	/* BCM_XGS_SWITCH_SUPPORT */
        if (SOC_IS_XGS_FABRIC(pcid_info->unit)) {
            debugk(DK_WARN, "Bad call for Hercules: ARL_DELETE_CMD_MSG\n");
            break;
        }
        rv = soc_internal_arl_del(pcid_info,
                                  (arl_entry_t *)msg->arlins.data);
        break;
    case L2_LOOKUP_CMD_MSG:
#ifdef	BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            msg->readresp.header.opcode = L2_LOOKUP_ACK_MSG;
            banks = msg->l2x2.header.cos & 0x3;
            rv = soc_internal_l2x2_entry_lkup(pcid_info, banks,
                                      (l2x_entry_t *)msg->l2x2.data,
                                      msg->readresp.data);
            _soc_l2x2_nak_check(pcid_info->unit, msg);
            break;
        }
        break;
#endif	/* BCM_FIREBOLT_SUPPORT */
    case ARL_LOOKUP_CMD_MSG:
#ifdef	BCM_XGS_SWITCH_SUPPORT
        if (SOC_IS_XGS_SWITCH(pcid_info->unit) &&
            !SOC_IS_XGS3_SWITCH(pcid_info->unit)) {
            rv = soc_internal_l2x_lkup(pcid_info,
                                       (l2x_entry_t *)msg->arllkup.data,
                                       msg->readresp.data);
            msg->readresp.header.opcode = READ_MEMORY_ACK_MSG;
            break;
        }
#endif	/* BCM_XGS_SWITCH_SUPPORT */
        if (SOC_IS_XGS_FABRIC(pcid_info->unit)) {
            debugk(DK_WARN, "Bad call for Hercules: ARL_LOOKUP_CMD_MSG\n");
            break;
        }

        if (soc_feature(unit, soc_feature_arl_lookup_cmd)) {
            int             copyno;
            copyno = (msg->arllkup.address >> 20) & 0xf;
            rv = soc_internal_arl_lkup(pcid_info, copyno,
                                       (arl_entry_t *)msg->arllkup.data,
                                       (arl_entry_t *)msg->readresp.data);
            msg->readresp.header.opcode = READ_MEMORY_ACK_MSG;
        } else {
            debugk(DK_VERBOSE,
                   "S-Channel operation: message not handled: %s (%d)\n",
                   soc_schan_op_name(msg->header.opcode), msg->header.opcode);
        }
        break;
    case L3_INSERT_CMD_MSG:
        debugk(DK_VERBOSE, "S-Channel operation: L3_INSERT_CMD_MSG\n");
#ifdef	BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            msg->readresp.header.opcode = L3_INSERT_DONE_MSG;
            banks = msg->l3x2.header.cos & 0x3;
            rv = soc_internal_l3x2_entry_ins(pcid_info, banks,
                          (l3_entry_ipv6_multicast_entry_t *)msg->l3x2.data);
            break;
        }
#endif	/* BCM_FIREBOLT_SUPPORT */
#ifdef	BCM_XGS_SWITCH_SUPPORT
        if (SOC_IS_XGS_SWITCH(pcid_info->unit)) {
            rv = soc_internal_l3x_ins(pcid_info,
                                      (l3x_entry_t *)msg->l3ins.data);
            break;
        }
#endif	/* BCM_XGS_SWITCH_SUPPORT */
        debugk(DK_WARN, "Bad call for Non-Draco: L3_INSERT_CMD_MSG\n");
        rv = -1;
        break;
    case L3_DELETE_CMD_MSG:
        debugk(DK_VERBOSE, "S-Channel operation: L3_DELETE_CMD_MSG\n");
#ifdef	BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            msg->readresp.header.opcode = L3_DELETE_DONE_MSG;
            banks = msg->l3x2.header.cos & 0x3;
            rv = soc_internal_l3x2_entry_del(pcid_info, banks,
                          (l3_entry_ipv6_multicast_entry_t *)msg->l3x2.data);
            break;
        }
#endif	/* BCM_FIREBOLT_SUPPORT */
#ifdef	BCM_XGS_SWITCH_SUPPORT
        if (SOC_IS_XGS_SWITCH(pcid_info->unit)) {
            rv = soc_internal_l3x_del(pcid_info,
                                      (l3x_entry_t *)msg->l3ins.data);
            break;
        }
#endif	/* BCM_XGS_SWITCH_SUPPORT */
        debugk(DK_WARN, "Bad call for Non-Draco: L3_DELETE_CMD_MSG\n");
        rv = -1;
        break;
    case L3X2_LOOKUP_CMD_MSG:
#ifdef	BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            msg->readresp.header.opcode = L3X2_LOOKUP_ACK_MSG;
            banks = msg->l3x2.header.cos & 0x3;
            rv = soc_internal_l3x2_entry_lkup(pcid_info, banks,
                          (l3_entry_ipv6_multicast_entry_t *)msg->l3x2.data,
                          msg->readresp.data);
            if (msg->readresp.data[0] == -1) {
                msg->readresp.header.cpu = 1;
            }
            break;
        }
        break;
#endif	/* BCM_FIREBOLT_SUPPORT */
    case INIT_SFAP_MSG:
        rv = 0;
        break;
    default:
        debugk(DK_VERBOSE,
               "S-Channel operation: message not handled: %s (%d)\n",
               soc_schan_op_name(msg->header.opcode), msg->header.opcode);
        break;
    }

    if (rv == 0) {
        PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_DONE_TST;
    }

    if (soc_feature(unit, soc_feature_schmsg_alias)) {
        /* Synchronize message buffer (at 0) with aliased memory */
        memcpy(&(PCIM(pcid_info, 0)),
               &(PCIM(pcid_info, PCIM_ALIAS_OFFSET)), PCIM_ALIASED_BYTES);
    }

    return rv;
}

/*
 * soc_internal_table_dma
 *
 * Do a table DMA transfer
 */

void
soc_internal_table_dma(pcid_info_t *pcid_info)
{
    uint32		entry[SOC_MAX_MEM_WORDS];
    uint32		table_addr;
    uint32		dma_addr;
    uint32		dma_count;
    uint32		dma_beats;

    table_addr = PCIM(pcid_info, CMIC_TABLE_DMA_START);
    dma_addr = PCIM(pcid_info, CMIC_ARL_DMA_ADDR);
    dma_count = PCIM(pcid_info, CMIC_ARL_DMA_CNT) & 0x1fffffff;
    dma_beats = PCIM(pcid_info, CMIC_ARL_DMA_CNT) >> 29;

    debugk(DK_VERBOSE,
           "Table DMA: schan_addr=0x%x count=0x%x beats=%d dest=0x%x\n",
           table_addr, dma_count, dma_beats, dma_addr);

    while (dma_count-- > 0) {
        soc_internal_read_mem(pcid_info, table_addr, entry);
        table_addr++;

        soc_internal_memory_store(pcid_info, dma_addr,
                                  (uint8 *)entry, dma_beats * 4,
                                  MF_ES_DMA_OTHER);
        dma_addr += dma_beats * 4;
    }

    

    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_ARL_DMA_DONE_TST;
    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_ARL_DMA_CNT0;
    soc_internal_send_int(pcid_info);
}

/*
 * soc_internal_xgs3_table_dma
 *
 * Do a table DMA transfer
 */

void
soc_internal_xgs3_table_dma(pcid_info_t *pcid_info)
{
    uint32		entry[SOC_MAX_MEM_WORDS];
    uint32		table_addr;
    uint32		dma_addr;
    uint32		dma_count;
    uint32		dma_beats;

    if (!(PCIM(pcid_info, CMIC_TABLE_DMA_CFG) & CMIC_TDMA_CFG_EN)) {
        return;
    }

    table_addr = PCIM(pcid_info, CMIC_TABLE_DMA_SBUS_START_ADDR);
    dma_addr = PCIM(pcid_info, CMIC_TABLE_DMA_PCIMEM_START_ADDR);
    dma_count = PCIM(pcid_info, CMIC_TABLE_DMA_ENTRY_COUNT);
    dma_beats = (PCIM(pcid_info, CMIC_TABLE_DMA_CFG) >> 16) & (0x1f);

    debugk(DK_VERBOSE,
           "Table DMA: schan_addr=0x%x count=0x%x beats=%d dest=0x%x\n",
           table_addr, dma_count, dma_beats, dma_addr);

    while (dma_count-- > 0) {
        soc_internal_read_mem(pcid_info, table_addr, entry);
        table_addr++;

        soc_internal_memory_store(pcid_info, dma_addr,
                                  (uint8 *)entry, dma_beats * 4,
                                  MF_ES_DMA_OTHER);
        dma_addr += dma_beats * 4;
    }


    PCIM(pcid_info, CMIC_TABLE_DMA_CFG) |= CMIC_TDMA_CFG_DONE;
    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_TDMA_DONE;
    soc_internal_send_int(pcid_info);
}

/*
 * soc_internal_xgs3_tslam_dma
 *
 * Do a table SLAM DMA transfer
 */

void
soc_internal_xgs3_tslam_dma(pcid_info_t *pcid_info)
{
    uint32		entry[SOC_MAX_MEM_WORDS];
    uint32		table_addr;
    uint32		dma_addr;
    uint32		dma_count;
    uint32		dma_beats;
    int                 direction;

    if (!(PCIM(pcid_info, CMIC_SLAM_DMA_CFG) & CMIC_SLAM_CFG_EN)) {
        return;
    }

    table_addr = PCIM(pcid_info, CMIC_SLAM_DMA_SBUS_START_ADDR);
    dma_addr = PCIM(pcid_info, CMIC_SLAM_DMA_PCIMEM_START_ADDR);
    dma_count = PCIM(pcid_info, CMIC_SLAM_DMA_ENTRY_COUNT);
    dma_beats = (PCIM(pcid_info, CMIC_SLAM_DMA_CFG) >> 16) & (0x1f);
    direction = (PCIM(pcid_info, CMIC_SLAM_DMA_CFG) & CMIC_SLAM_CFG_DIR);

    if (direction) {
        /* Last entry */
        dma_addr = dma_addr + (dma_count - 1) * 4 * dma_beats;
        table_addr = table_addr + dma_count - 1;
    }

    debugk(DK_VERBOSE,
           "Table DMA: schan_addr=0x%x count=0x%x beats=%d dest=0x%x\n",
           table_addr, dma_count, dma_beats, dma_addr);

    while (dma_count-- > 0) {
        soc_internal_memory_fetch(pcid_info, dma_addr,
                                  (uint8 *)entry, dma_beats * 4,
                                  MF_ES_DMA_OTHER);
        dma_addr += (direction ? -(dma_beats * 4) : (dma_beats * 4));
        soc_internal_write_mem(pcid_info, table_addr, entry);
        table_addr += (direction ? -1 : 1);

    }


    PCIM(pcid_info, CMIC_SLAM_DMA_CFG) |= CMIC_SLAM_CFG_DONE;
    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_TSLAM_DONE;
    soc_internal_send_int(pcid_info);
}

void
soc_internal_schan_ctrl_write(pcid_info_t *pcid_info, uint32 value)
{
    if (value & 0x80) {
        debugk(DK_VERBOSE, "Set SCHAN_CTRL bit %d\n", value & 0x1f);
        switch (value & 0xff) {
        case SC_MSG_START_SET:
            if ((pcid_info->schan_cb)
                && (pcid_info->schan_cb(pcid_info, pcid_info->unit) == 0)) {
                PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_SCH_MSG_DONE;
                soc_internal_send_int(pcid_info);
                break;
            } else {
                if (schan_op(pcid_info, pcid_info->unit) == 0) {
                    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_SCH_MSG_DONE;
                    soc_internal_send_int(pcid_info);
                }
            }
            break;
        case SC_MIIM_RD_START_SET:
            soc_internal_miim_op(pcid_info, 1);
            PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_MIIM_OP_DONE;
            soc_internal_send_int(pcid_info);
            break;
        case SC_MIIM_WR_START_SET:
            soc_internal_miim_op(pcid_info, 0);
            PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_MIIM_OP_DONE;
            soc_internal_send_int(pcid_info);
            break;
        case SC_ARL_DMA_EN_SET:
            if (SOC_IS_XGS_SWITCH(pcid_info->unit)) {
                soc_internal_table_dma(pcid_info);
                PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_ARL_DMA_CNT0;
                soc_internal_send_int(pcid_info);
            } else if (SOC_IS_STRATA(pcid_info->unit)) {
#if 0	/* TBD */
                PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_ARL_DMA_XFER;
                PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_ARL_DMA_CNT0;
                soc_internal_send_int(pcid_info);
#endif
            } else {
                PCIM(pcid_info, CMIC_SCHAN_CTRL) |= 1 << (value & 0x1f);
            }
        default:
            PCIM(pcid_info, CMIC_SCHAN_CTRL) |= 1 << (value & 0x1f);
            break;
        }
    } else {
        debugk(DK_VERBOSE, "Clear SCHAN_CTRL bit %d\n", value & 0x1f);
        PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~(1 << (value & 0x1f));
        switch (value & 0xff) {
        case SC_MSG_DONE_CLR:
            PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_SCH_MSG_DONE;
            break;
        case SC_MIIM_OP_DONE_CLR:
            PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_MIIM_OP_DONE;
            break;
        case SC_ARL_DMA_EN_CLR:
            PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_ARL_DMA_CNT0;
            break;
        default:
            break;
        }
    }
}

void
soc_internal_reset(pcid_info_t *pcid_info)
{
    int index;
    soc_datum_t *d;

    memset(&PCIM(pcid_info, 0), 0, PCIM_SIZE(0));

    PCIM(pcid_info, CMIC_SCHAN_CTRL) = 0x48002;

    /*
     * Delete all register values from hash table, which effectively
     * returns them to their reset default values.
     */
    for (index = 0; index < SOC_DATUM_HASH_SIZE; index++) {
        while ((d = pcid_info->reg_ht[index]) != NULL) {
            pcid_info->reg_ht[index] = d->next;
            sal_free(d);
        }
    }

    /*
     * Delete all memory entries from hash table, which effectively
     * returns them to their null entry values.
     */
    for (index = 0; index < SOC_DATUM_HASH_SIZE; index++) {
        while ((d = pcid_info->mem_ht[index]) != NULL) {
            pcid_info->mem_ht[index] = d->next;
            sal_free(d);
        }
    }

#ifdef	BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_DRACO(pcid_info->unit) || SOC_IS_LYNX(pcid_info->unit)) {
        soc_internal_l2x_init(pcid_info);
        soc_internal_l3x_init(pcid_info);
        soc_internal_vlan_tab_init(pcid_info);
    }
    if (SOC_IS_TUCANA(pcid_info->unit)) {
        soc_internal_l2x_init(pcid_info);
        soc_internal_l3x_init(pcid_info);
    }
#endif	/* BCM_XGS_SWITCH_SUPPORT */
#ifdef	BCM_XGS_FABRIC_SUPPORT
    if (SOC_IS_XGS_FABRIC(pcid_info->unit)) {
    }
#endif	/* BCM_XGS_FABRIC_SUPPORT */
    if (NEEDS_ARL_INIT(pcid_info)) {
        soc_internal_arl_init(pcid_info);
    }

    /*
     * Invoke reset callback if installed.
     */
    if (pcid_info->reset_cb) {
        pcid_info->reset_cb(pcid_info, pcid_info->unit);
    }
}

uint32
soc_internal_endian_swap(pcid_info_t *pcid_info, uint32 data, int mode)
{
    if (((mode == MF_ES_DMA_PACKET) &&
         (PCIM(pcid_info, CMIC_ENDIAN_SELECT) & ES_BIG_ENDIAN_DMA_PACKET))
        ||
        ((mode == MF_ES_DMA_OTHER) &&
         (PCIM(pcid_info, CMIC_ENDIAN_SELECT) & ES_BIG_ENDIAN_DMA_OTHER))
        ||
        ((mode == MF_ES_PIO) &&
         (PCIM(pcid_info, CMIC_ENDIAN_SELECT) & ES_BIG_ENDIAN_PIO))) {
        data = (data << 16) | (data >> 16);
        return (data & 0xff00ffff) >> 8 | (data & 0xffff00ff) << 8;
    } else {
        return(data);
    }
}

void
soc_internal_memory_fetch(pcid_info_t *pcid_info, uint32 addr, uint8 *b,
                          int length, int mode)
{
    uint32	    offset;

    debugk(DK_VERBOSE, "soc_internal_memory_fetch: addr=0x%08x length=%d\n",
           addr, length);

    /*
     * For memory fetch, any alignment is allowed, but 4-byte accesses
     * are always made. This is slow but ....
     */
    offset = addr & 3;

    addr &= ~3;			/* Only read 4-bytes */

    while (length) {
        uint32	data;
        int	tl;

        data =
            soc_internal_endian_swap(pcid_info,
                                     dma_readmem(pcid_info->client->dmasock,
                                                 addr),
                                     mode);
        tl = (sizeof(uint32) - offset);
        if (length < tl) {
            tl = length;
        }

        sal_memcpy(b, ((uint8 *)&data) + offset, tl);
        length -= tl;
        addr += 4;
        b += 4 - offset;
        offset = 0;
    }
}

/* Get a string of bytes from memory addr given length; store in buf. */
void
soc_internal_bytes_fetch(pcid_info_t *pcid_info, uint32 addr, uint8 *buf,
                          int length)
{
    int read_len;

    debugk(DK_VERBOSE, "soc_internal_bytes_fetch: addr=0x%08x length=%d\n",
           addr, length);

    read_len = dma_read_bytes(pcid_info->client->dmasock, addr, buf, length);

    if (read_len != length) {
        debugk(DK_ERR,
               "soc_internal_bytes_fetch: length mismatch.  want %d. got %d\n",
               length, read_len);
    }
}

/* Get a string of bytes from memory addr given length; store in buf. */
void
soc_internal_bytes_store(pcid_info_t *pcid_info, uint32 addr, uint8 *buf,
                         int length)
{
    debugk(DK_VERBOSE, "soc_internal_bytes_store: addr=0x%08x length=%d\n",
           addr, length);

    dma_write_bytes(pcid_info->client->dmasock, addr, buf, length);
}

void
soc_internal_memory_store(pcid_info_t *pcid_info, uint32 addr, uint8 *b,
                          int length, int mode)
{
    uint32          data;

    debugk(DK_VERBOSE, "soc_internal_memory_store: addr=0x%08x length=%d\n",
           addr, length);

    /*
     * Strata (and friends) only support 4-byte aligned writes
     */

   addr &= ~3;

    while (length > 0) {
        sal_memcpy(&data, b, length < sizeof(data) ? length : sizeof(data));
        dma_writemem(pcid_info->client->dmasock,
                     addr, soc_internal_endian_swap(pcid_info, data, mode));
        b += sizeof(uint32);
        addr += sizeof(uint32);
        length -= sizeof(uint32);
    }
}

#ifdef BCM_XGS_SWITCH_SUPPORT
int
soc_internal_manual_hash(pcid_info_t *pcid_info, uint32 *data)
{
    /* First try at endianness */
    uint32          tmp_hs[SOC_MAX_MEM_WORDS];
    int             hash_type;
    int             hashed_result = 0;
    int             hash_sel;
    uint8           key[XGS_HASH_KEY_SIZE];

    soc_internal_read_reg(pcid_info,
                          soc_reg_addr(pcid_info->unit, HASH_CONTROLr,
				       REG_PORT_ANY, 0),
                          tmp_hs);
    hash_sel = soc_reg_field_get(pcid_info->unit, HASH_CONTROLr,
                                 tmp_hs[0], HASH_SELECTf);

    hash_type = soc_mem_field32_get(pcid_info->unit, HASHINPUTm,
                                  data, KEY_TYPEf);

    if (hash_sel >= XGS_HASH_COUNT) {
        debugk(DK_VERBOSE, "invalid hash selection %d", hash_sel);
        return -1;
    }

    switch (hash_type) {
    case XGS_HASH_KEY_TYPE_L2:		       /* L2 table */
    {
        /* I can't imagine the loop arithmetic for this would suck worse */
        key[0] = (data[0] << 4) & 0xf0;
        key[1] = (data[0] >> 4) & 0xff;
        key[2] = (data[0] >> 12) & 0xff;
        key[3] = (data[0] >> 20) & 0xff;
        key[4] = ((data[1] << 4) & 0xf0) | ((data[0] >> 28) & 0x0f);
        key[5] = (data[1] >> 4) & 0xff;
        key[6] = (data[1] >> 12) & 0xff;
        key[7] = (data[1] >> 20) & 0xff;

        hashed_result = soc_draco_l2_hash(pcid_info->unit, hash_sel, key);
        break;
    }

    case XGS_HASH_KEY_TYPE_L3UC:	       /* L3 table, 32-bit */
    {
        int             ix;
        for (ix = 0; ix < 4; ix++) {
            key[ix] = (data[0] >> (8 * ix)) & 0xff;
        }

        hashed_result = soc_draco_l3_hash(pcid_info->unit, hash_sel, 0, key);
        break;
    }

    case XGS_HASH_KEY_TYPE_L3MC:	       /* L3 table, 64-bit */
    {
        int             ix, key_src_ip;

        if (SOC_IS_DRACO1(pcid_info->unit)) {
            soc_internal_read_reg(pcid_info,
                                  soc_reg_addr(pcid_info->unit, CONFIGr,
					       REG_PORT_ANY, 0),
                                  tmp_hs);
            key_src_ip = soc_reg_field_get(pcid_info->unit, CONFIGr, tmp_hs[0],
                                           SRC_IP_CFGf);
        } else if (SOC_IS_LYNX(pcid_info->unit)) {
            soc_internal_read_reg(pcid_info,
                                  soc_reg_addr(pcid_info->unit, ARL_CONTROLr,
					       REG_PORT_ANY, 0),
                                  tmp_hs);
            key_src_ip = soc_reg_field_get(pcid_info->unit, ARL_CONTROLr,
					   tmp_hs[0], SRC_IP_CFGf);
        } else { /* Draco1.5 and Tucana */
            key_src_ip = FALSE;
        }

        for (ix = 0; ix < 4; ix++) {
            key[ix] = (data[0] >> (8 * ix)) & 0xff;
            key[ix + 4] = key_src_ip ? 0 : ((data[1] >> (8 * ix)) & 0xff);
        }

        if (soc_feature(pcid_info->unit, soc_feature_l3_sgv)) {
            key[8] = data[2] & 0xff;
            key[9] = (data[2] >> 8) & 0xf;
        }

        hashed_result = soc_draco_l3_hash(pcid_info->unit, hash_sel, 1, key);
        break;
    }

    default:
        debugk(DK_VERBOSE, "invalid hash type selection");
        return -1;
    }

    tmp_hs[0] = hashed_result;
    soc_internal_write_reg(pcid_info,
                           soc_reg_addr(pcid_info->unit, HASH_OUTPUTr,
					REG_PORT_ANY, 0),
                           tmp_hs);
    return 0;
}
#endif	/* BCM_XGS_SWITCH_SUPPORT */

/*
 * soc_internal_send_int
 *
 *    Checks if any interrupt is pending, and if so, sends an interrupt
 *    over the PLI socket.
 */

void
soc_internal_send_int(pcid_info_t *pcid_info)
{
    if (PCIM(pcid_info, CMIC_IRQ_MASK) & PCIM(pcid_info, CMIC_IRQ_STAT)) {
        debugk(DK_VERBOSE, "Send interrupt: msk 0x%x stat 0x%x\n",
            PCIM(pcid_info, CMIC_IRQ_MASK), PCIM(pcid_info, CMIC_IRQ_STAT));
        send_interrupt(pcid_info->client->intsock, 0);
    } else {
        debugk(DK_VERBOSE, "Interrupt masked: msk 0x%x stat 0x%x\n",
            PCIM(pcid_info, CMIC_IRQ_MASK), PCIM(pcid_info, CMIC_IRQ_STAT));
    }
}
