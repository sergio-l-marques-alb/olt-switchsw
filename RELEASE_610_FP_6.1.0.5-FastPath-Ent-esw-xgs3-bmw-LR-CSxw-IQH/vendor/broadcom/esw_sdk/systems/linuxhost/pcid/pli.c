/* 
 * $Id: pli.c,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        pli.c
 *
 * Provides:
 *      pli_reset_service
 *      pli_getreg_service
 *      pli_setreg_service
 *
 * Requires:
 *      soc_internal_reset
 *      soc_internal_<mem>
 *      soc_internal_send_int
 *      pcid_dma_stat_write
 *      pcid_dma_ctrl_write
 *      soc_internal_arl_init
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>

#include <soc/cmic.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <bde/pli/verinet.h>

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"
#include "dma.h"
#include "pli.h"

/* 
 * pli_reset_service
 *
 * Set PCI memory state to reset defaults
 */
int pli_reset_service(pcid_info_t *pcid_info)
{
    soc_internal_reset(pcid_info);
    return 0;
}

/* 
 * PLI protocol compatible "get register" routine
 */

uint32
pli_getreg_service(pcid_info_t * pcid_info, int unit, uint32 type,
                   uint32 regnum)
{
    char            buffer[35];
    uint32          value, r;

    pcid_type_name(buffer, type);

    if (regnum & 3) {
        sal_printf("pli_getreg_service: unaligned access, "
               "type=0x%x addr=0x%x\n", type, regnum);
        regnum &= ~3;
    }

    value = 0xffffffff;                /* Default value */

    switch (type) {
    case PCI_CONFIG:
        r = regnum & 0xfff;
        if (r < PCIC_SIZE) {
            value = PCIC(pcid_info, r);
        }
        break;
    case PCI_MEMORY:
        r = regnum & 0xffff;
        if (r == CMIC_LED_CTRL || r == CMIC_LED_STATUS) {
            value = 0;
        } else if (r < PCIM_SIZE(unit)) {
            value = PCIM(pcid_info, r);
        } else {
            value = 0xdeadbeef;
        }
        break;
    case I2C_CONFIG:
        break;
    case PLI_CONFIG:
        break;
    case JTAG_CONFIG:
        break;
    }

    if (pcid_info->opt_pli_verbose) {
        debugk(DK_VERBOSE, "%s READ @0x%x => 0x%x\n", buffer, regnum, value);
    }

    return(soc_internal_endian_swap(pcid_info, value, MF_ES_PIO));
}

/* 
 * PLI protocol compatible "set register" routine
 */

uint32
pli_setreg_service(pcid_info_t * pcid_info, int unit, uint32 type,
                   uint32 regnum, uint32 value)
{
    char            buffer[35];
    uint32          r;

    pcid_type_name(buffer, type);

    value = soc_internal_endian_swap(pcid_info, value, MF_ES_PIO);

    if (regnum & 3) {
        sal_printf("pli_setreg_service: unaligned access, "
               "type=0x%x addr=0x%x value=0x%x\n", type, regnum, value);
        regnum &= ~3;
    }

    debugk(DK_VERBOSE, "%s WRITE @0x%x <= 0x%x\n", buffer, regnum, value);
    if (pcid_info->opt_pli_verbose) {
        debugk(DK_VERBOSE, "%s WRITE @0x%x <= 0x%x\n", buffer, regnum, value);
    }

    switch (type) {
    case PCI_CONFIG:
        r = regnum & 0xfff;
        if (r < PCIC_SIZE) {
            PCIC(pcid_info, r) = value;
        }
        break;
    case PCI_MEMORY:
        r = regnum & 0xffff;

        if (pcid_info->i2crom_fp) {
            fputc((r >> 8) & 0xff, pcid_info->i2crom_fp);
            fputc((r >> 0) & 0xff, pcid_info->i2crom_fp);
            fputc((value >> 24) & 0xff, pcid_info->i2crom_fp);
            fputc((value >> 16) & 0xff, pcid_info->i2crom_fp);
            fputc((value >> 8) & 0xff, pcid_info->i2crom_fp);
            fputc((value >> 0) & 0xff, pcid_info->i2crom_fp);
        }
        switch (r) {
        case CMIC_SCHAN_CTRL:
            soc_internal_schan_ctrl_write(pcid_info, value);
            break;
        case CMIC_CONFIG:
            if (value & CC_RESET_CPS) {
                soc_internal_reset(pcid_info);
            } else {
                PCIM(pcid_info, r) = value;
            }
            break;
        case CMIC_IRQ_MASK:
            PCIM(pcid_info, r) = value;
            soc_internal_send_int(pcid_info);       /* Send int if pending */
            break;
        case CMIC_DMA_STAT:
            pcid_dma_stat_write(pcid_info, value);
            break;
        case CMIC_DMA_CTRL:
            pcid_dma_ctrl_write(pcid_info, value);
            break;
        case CMIC_TABLE_DMA_CFG:
            PCIM(pcid_info, r) = value;
            soc_internal_xgs3_table_dma(pcid_info);
            break;
        case CMIC_SLAM_DMA_CFG:
            PCIM(pcid_info, r) = value;
            soc_internal_xgs3_tslam_dma(pcid_info);
            break;
        case CMIC_LED_CTRL:
        case CMIC_LED_STATUS:
            break;
        default:
            if (r < PCIM_SIZE(unit)) {
                PCIM(pcid_info, r) = value;
                if (soc_feature(unit, soc_feature_schmsg_alias)) {
                    if (r < 0x50) {
                        PCIM(pcid_info, r + 0x800) = value;
                    } else if (r >= 0x800 && r < 0x850) {
                        PCIM(pcid_info, r - 0x800) = value;
                    }
                }
            }
            break;
        }
        break;
    case I2C_CONFIG:
        break;
    case PLI_CONFIG:
        break;
    case JTAG_CONFIG:
        break;
    }

    return 0;
}
