/*
 * $Id: dma.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:    dma.c
 * Purpose:     SOC DMA LLC (Link Layer) driver; used for sending
 *              and receiving packets over MII (and later, the uplink).
 *      Interface via ethernet et module of bcm47xx.
 */
#include <shared/et/osl.h>
#include <shared/et/proto/ethernet.h>
#include <shared/et/bcmenetmib.h>
#include <soc/etc.h>

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <soc/mcm/robo/driver.h>
#include <soc/debug.h>
#include <soc/dma.h>

#include <soc/cm.h>

static int soc_dma_initialized = 0;

#define DV_MAGIC_NUMBER 0xba5eba11

#define DP_FMT  "%sdata[%04x]: "    /* dump line start format */
#define DP_BPL  16          /* dumped bytes per line */

void
soc_robo_dma_ether_dump(int unit, char *pfx, uint8 *addr, int len, int offset)
{
    int     i = 0, j;

    if (len >= DP_BPL && (DP_BPL & 1) == 0) {
        char    linebuf[128], *s;
        /* Show first line with MAC addresses in curly braces */
        s = linebuf;
        sal_sprintf(s, DP_FMT "{", pfx, i);
        while (*s != 0) s++;
        for (i = offset; i < offset + 6; i++) {
            sal_sprintf(s, "%02x", addr[i]);
            while (*s != 0) s++;
        }
        sal_sprintf(s, "} {");
        while (*s != 0) s++;
        for (; i < offset + 12; i++) {
            sal_sprintf(s, "%02x", addr[i]);
            while (*s != 0) s++;
        }
        sal_sprintf(s, "}");
        while (*s != 0) s++;
        for (; i < offset + DP_BPL; i += 2) {
            sal_sprintf(s, " %02x%02x", addr[i], addr[i + 1]);
            while (*s != 0) s++;
        }
        soc_cm_print("%s\n", linebuf);
        }

        for (; i < len; i += DP_BPL) {
        char    linebuf[128], *s;
        s = linebuf;
        sal_sprintf(s, DP_FMT, pfx, i);
        while (*s != 0) s++;
        for (j = i; j < i + DP_BPL && j < len; j++) {
            sal_sprintf(s, "%02x%s", addr[j], j & 1 ? " " : "");
            while (*s != 0) s++;
        }
        soc_cm_print("%s\n", linebuf);
    }

}


/*
 * Function:
 *  soc_robo_dma_dump_pkt
 * Purpose:
 *  Dump packet data in human readable form
 * Parameters:
 *  pfx - prefix for all output strings
 *  addr - pointer to data bytes of packet
 *  len - length of data to dump
 * Returns:
 *  Nothing
 */

void
soc_robo_dma_dump_pkt(int unit, char *pfx, uint8 *addr, int len)
{
    int ether_offset;

    COMPILER_REFERENCE(unit);

    if (len == 0 || !addr) {
        soc_cm_print(DP_FMT "<NONE>\n", pfx, 0);
        return;
    }

    ether_offset = 0;

    soc_robo_dma_ether_dump(unit, pfx, addr, len, ether_offset);
}

#undef DP_FMT
#undef DP_BPL

/*
 * Function:
 *      soc_robo_dma_dv_valid
 * Purpose:
 *      Check if a DV is (probably) valid
 * Parameters:
 *      dv - The dv to examine
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Does not guarantee a DV is valid, but will detect
 *      most _invalid_ DVs.
 */

int
soc_robo_dma_dv_valid(robo_dv_t *dv)
{
    if (dv->dv_magic != DV_MAGIC_NUMBER) {
        return FALSE;
    }

    return TRUE;
}


/*
 * Function:
 *  soc_robo_dma_dump_dv
 * Purpose:
 *  Dump a "dv" structure and all the DCB fields.
 * Parameters:
 *  dv_chain - pointer to dv list to dump.
 * Returns:
 *  Nothing.
 */

void
soc_robo_dma_dump_dv(int unit, char *pfx, robo_dv_t *dv_chain)
{
    char    tmps[128];
    char    *op_name;
    int     i;

    if (!soc_robo_dma_dv_valid(dv_chain)) {
        soc_cm_print("%sdv@%p appears invalid\n", pfx, (void *)dv_chain);
        return;
    }

    switch(dv_chain->dv_op) {
    case DV_NONE:   op_name = "None";   break;
    case DV_TX:     op_name = "TX";         break;
    case DV_RX:     op_name = "RX";         break;
    default:        op_name = "*ERR*";  break;
    }
    tmps[0] = '\0';

    soc_cm_print("%sdv@%p op=%s vcnt=%d cnt=%d\n",
         pfx, (void *)dv_chain, op_name, dv_chain->dv_vcnt,
         dv_chain->dv_length);
    soc_cm_print("%s    user1 %p. user2 %p. user3 %p. user4 %p\n",
         pfx, dv_chain->dv_public1.ptr, dv_chain->dv_public2.ptr,
                 dv_chain->dv_public3.ptr, dv_chain->dv_public4.ptr);
    sal_strncpy(tmps, pfx, sizeof(tmps) - 1);
    sal_strncpy(tmps + sal_strlen(tmps), "  ", 
            sizeof(tmps) - sal_strlen(tmps) - 1);
    
    for (i = 0; i < dv_chain->dv_vcnt; i++) {
        robo_dcb_t    *dcb;
        sal_vaddr_t   addr;

        dcb = (robo_dcb_t *)(&dv_chain->dv_dcb[i]);
        addr = (sal_vaddr_t)(dcb->dcb_vaddr);
        soc_cm_print("%sdcb[%d] @%p: addr=%p, len=%d\n",
                     pfx, i, (void *)dcb, (void *)addr, dcb->len);
        if (soc_cm_debug_check(DK_PACKET)) {
            if (dv_chain->dv_op == DV_TX) {
                soc_robo_dma_dump_pkt(unit, pfx, (uint8 *) addr, dcb->len);
            } else if (dv_chain->dv_op == DV_RX) {
                soc_robo_dma_dump_pkt(unit, pfx, (uint8 *) addr, dcb->len);
            }
        }
    }
}

/*
 * Function:
 *  soc_robo_dma_start
 * Purpose:
 *  Launch a SOC_DMA DMA operation.
 * Parameters:
 *  unit - unit number.
 *  dv_chain - dma request description.
 * Returns:
 *  SOC_E_NONE - operation started.
 *  SOC_E_TIMEOUT - operation failed to be queued.
 */

int
soc_robo_dma_start(int unit, robo_dv_t *dv_chain)
{
    /* If DK_DMA set, dump out info on request, before queued on channel */

    if (soc_cm_debug_check(DK_DMA) && (dv_chain->dv_op == DV_TX)) {
        soc_robo_dma_dump_dv(unit, "dma (before): ", dv_chain);
    }

    switch (dv_chain->dv_op) {
    case DV_TX:
        et_soc_start(unit, dv_chain);
        break;
    case DV_RX:
        et_soc_rx_chain(unit, dv_chain);
        break;
    default:
        soc_cm_print("ERROR: unit %d unknown dma op %d\n", 
            unit, dv_chain->dv_op);
        assert(0);
        return (SOC_E_PARAM);
    }

    return (SOC_E_NONE);
}


int 
soc_robo_dma_rxenable(int unit)
{
    et_soc_rxmon_on(unit);
    return SOC_E_NONE;
}


int 
soc_robo_dma_rxstop(int unit)
{
    et_soc_rxmon_off(unit);
    return SOC_E_NONE;
}




/*
 * Function:
 *  soc_robo_dma_dv_alloc
 * Purpose:
 *  Allocate and initialize a dv struct.
 * Parameters:
 *  op - operations iov requested for.
 *  cnt - number of DCBs required.
 * Notes:
 *  If a DV on the free list will accomodate the request,
 *  satisfy it from there to avoid extra alloc/free calls.
 */

robo_dv_t *
soc_robo_dma_dv_alloc(int unit, dvt_t op, int cnt)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    robo_dv_t   *dv;

    assert(cnt > 0);

    /* Check if we can use one off the free list */

    soc->stat.dv_alloc++;

    dv = sal_alloc(sizeof(robo_dv_t), "soc_dv_alloc");
    if (dv == NULL) {
        soc_cm_print("ERROR: unit %d dv alloc failed.\n", unit);
        return(dv);
    }

    dv->dv_dcb = sal_alloc(sizeof(robo_dcb_t) * cnt, "soc_dcb_alloc");
    if (dv->dv_dcb == NULL) {
        soc_cm_print("ERROR: unit %d dcb alloc failed.\n", unit);
        sal_free(dv);
        return(NULL);
    }
    sal_memset(dv->dv_dcb, 0, sizeof(robo_dcb_t) * cnt);

    switch (op) {
    case DV_TX:
        /* alloc tx specific header and tail crc */
        /* No BRCM type (2 bytes) for BCM53115, BCM53118 */
        if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
            dv->dv_dmabufhdr = (sal_vaddr_t)
                       soc_cm_salloc(unit, SOC_DMA_TX_HDR_53115, "dma_tx_hdr");
            dv->dv_dmabufcrc = (sal_vaddr_t)
                       soc_cm_salloc(unit, 4, "dma_tx_crc");
        } else {
            dv->dv_dmabufhdr = (sal_vaddr_t)
                       soc_cm_salloc(unit, SOC_DMA_TX_HDR, "dma_tx_hdr");
            dv->dv_dmabufcrc = (sal_vaddr_t)
                       soc_cm_salloc(unit, SOC_DMA_TX_CRC, "dma_tx_crc");
        }

        /* fail on any of above */
        if (!dv->dv_dmabufhdr || !dv->dv_dmabufcrc) {
                soc_cm_print("ERROR: unit %d tx dmabuf hdr/crc alloc failed.\n",
                 unit);
            if (dv->dv_dmabufhdr) {
                    soc_cm_sfree(unit, (void *)dv->dv_dmabufhdr);
                }

            if (dv->dv_dmabufcrc) {
                    soc_cm_sfree(unit, (void *)dv->dv_dmabufcrc);
                }

            sal_free(dv->dv_dcb);
            sal_free(dv);
            return (NULL);
        }

        /* No BRCM type (2 bytes) for BCM53115, BCM53118 */
        if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
            sal_memset((void *)dv->dv_dmabufhdr, 0, SOC_DMA_TX_HDR_53115);
            sal_memset((void *)dv->dv_dmabufcrc, 0, 4);
        } else {
            sal_memset((void *)dv->dv_dmabufhdr, 0, SOC_DMA_TX_HDR);
            sal_memset((void *)dv->dv_dmabufcrc, 0, SOC_DMA_TX_CRC);
        }
        break;

    case DV_RX:
        dv->dv_dmabufhdr = (sal_vaddr_t)NULL;
        dv->dv_dmabufcrc = (sal_vaddr_t)NULL;
        break;

    default:
        soc_cm_print("ERROR: unit %d unknown dma op %d\n", unit, op);
        assert(0);
        return (NULL);
    }

    dv->dv_unit        = unit;
    dv->dv_magic       = DV_MAGIC_NUMBER;
    dv->dv_op          = op;
    dv->dv_length      = 0;
    dv->dv_cnt         = cnt;
    dv->dv_vcnt        = 0;
    dv->dv_dcnt        = 0;
    dv->dv_brcm_tag    = 0;
    dv->dv_done_packet = NULL;
    dv->dv_public1.ptr = NULL;
    dv->dv_public2.ptr = NULL;
    dv->dv_public3.ptr = NULL;
    dv->dv_public4.ptr = NULL;
    return(dv);
}

/*
 * Function:
 *  soc_robo_dma_dv_free
 * Purpose:
 *  Free a dv struct.
 * Parameters:
 *  dv - pointer to dv to free (NOT a dv chain).
 * Returns:
 *  Nothing.
 */

void
soc_robo_dma_dv_free(int unit, robo_dv_t *dv)
{
    soc_control_t   *soc = SOC_CONTROL(unit);

    soc->stat.dv_free++;
    assert(dv->dv_magic == DV_MAGIC_NUMBER);
    dv->dv_magic = 0;

    if (dv->dv_op == DV_TX) {
        /* free tx extra dma buffer */
        soc_cm_sfree(unit, (void *)dv->dv_dmabufhdr);
        soc_cm_sfree(unit, (void *)dv->dv_dmabufcrc);
    }

    if (dv->dv_dcb) {
        sal_free((void *)dv->dv_dcb);
    }

    sal_free((void *)dv);
}

/*
 * Function:
 *  soc_robo_dma_desc_add
 * Purpose:
 *  Add a DMA descriptor to a DMA chain independent of the
 *  descriptor type.
 * Parameters:
 *  dv - pointer to DMA I/O Vector to be filled in.
 *  addr/cnt - values to add to the DMA chain.
 * Returns:
 *  < 0 - SOC_E_XXXX error code
 *  >= 0 - # entries left that may be filled.
 * Notes:
 *  Calls the specific fastpath routine if it can, defaulting
 *  to a general routine.
 */

int
soc_robo_dma_desc_add(robo_dv_t *dv, sal_vaddr_t addr, uint16 cnt)
{
    int16    dcb = dv->dv_vcnt;
    assert((dv->dv_vcnt >= 0) && (dv->dv_vcnt < dv->dv_cnt));

    dv->dv_dcb[dcb].dcb_vaddr = addr;
    dv->dv_dcb[dcb].len = cnt;
    dv->dv_dcb[dcb].next = NULL;
    if (dcb > 0) {
        dv->dv_dcb[dcb-1].next = (robo_dcb_t *)&dv->dv_dcb[dcb];
    }
    dv->dv_vcnt++;
    dv->dv_length += cnt;
    return(dv->dv_cnt - dv->dv_vcnt);
}

/*
 * Function:
 *  soc_robo_dma_dv_join
 * Purpose:
 *  Append src_chain to the end of dv_chain
 * Parameters:
 *  dv_chain - pointer to DV chain to be appended to.
 *  src_chain - pointer to DV chain to add.
 * Returns:
 *  SOC_E_NONE - success
 *  SOC_E_XXXX - error code.
 * Note:
 *  src_chain is consumed and should not be further referenced.
 *  If the last DCB in the chained list has the S/G
 *  bit set, then the S/G bit is set in the RLD dcb.
 *  The notification routines MUST be the same in all
 *  elements of the list (this condition is asserted)
 */

int
soc_robo_dma_dv_join(robo_dv_t *dv_chain, robo_dv_t *src_chain)
{
    return(SOC_E_UNAVAIL);
}

/*
 * Function:
 *  soc_robo_dma_dv_reset
 * Purpose:
 *  Reinitialize a dv struct to avoid free/alloc to reuse it.
 * Parameters:
 *  op - operation type requested.
 * Returns:
 *  Nothing.
 */

void
soc_robo_dma_dv_reset(dvt_t op, robo_dv_t *dv)
{
    dv->dv_op      = op;
    dv->dv_vcnt    = 0;
    dv->dv_dcnt    = 0;
    /* don't clear all flags */
    dv->dv_flags   = 0;
    dv->dv_public1.ptr = NULL;
    dv->dv_public2.ptr = NULL;
    dv->dv_public3.ptr = NULL;
    dv->dv_public4.ptr = NULL;
}

/*
 * Function:
 *  soc_robo_dma_init
 * Purpose:
 *  Initialize the SOC DMA routines for a SOC unit.
 * Parameters:
 *  unit - SOC unit #
 * Returns:
 *  SOC_E_NONE - Success
 *  SOC_E_BUSY - DMA channel busy.
 *  SOC_E_XXX
 * Notes:
 *  This routine frees all DV's on the free list, and configures
 *  a default channel configuration.
 */

int
soc_robo_dma_init(int unit)
{

    uint32 reg_addr, reg_value, temp;
    int reg_len;


    /* Enable Frame Forwarding, Set Managed Mode */
    reg_value = 0;
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, SWMODEr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, SWMODEr);
    (DRV_SERVICES(unit)->reg_read)(unit, reg_addr, &reg_value, reg_len);
    temp = 1;
    (DRV_SERVICES(unit)->reg_field_set)
        (unit, SWMODEr, &reg_value, SW_FWDG_ENf, &temp);
    (DRV_SERVICES(unit)->reg_field_set)
        (unit, SWMODEr, &reg_value, SW_FWDG_MODEf, &temp);
    (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len);

    if (unit == soc_mii_unit) {

        soc_dma_initialized = 1;
        if(SOC_IS_ROBO5324(unit)){
        /*
     * Force MII Software Override, set 100Full Link up
     * in MII port of mgnt chip
     */
        reg_value = 0;
        reg_addr = (DRV_SERVICES(unit)->reg_addr)
            (unit, STS_OVERRIDE_P24r, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
            (unit, STS_OVERRIDE_P24r);
        temp = 1;
        (DRV_SERVICES(unit)->reg_field_set)
            (unit, STS_OVERRIDE_P24r, &reg_value, MII_SW_ORf, &temp);
        (DRV_SERVICES(unit)->reg_field_set)
            (unit, STS_OVERRIDE_P24r, &reg_value, MII_SPDf, &temp);
        (DRV_SERVICES(unit)->reg_field_set)
            (unit, STS_OVERRIDE_P24r, &reg_value, MII_FDXf, &temp);
        (DRV_SERVICES(unit)->reg_field_set)
            (unit, STS_OVERRIDE_P24r, &reg_value, MII_LINKf, &temp);
        (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len);
        }

        if(SOC_IS_ROBODINO(unit) || SOC_IS_ROBO5398(unit) ||
            SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5397(unit) ||
            SOC_IS_ROBO5347(unit) || SOC_IS_ROBO5395(unit) ||
            SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)){

            /* force enabling the BRCM header tag :
             *  - bcm5395/53115/53118 allowed user to disable the BRCM header
             *
             */
            if (SOC_IS_ROBO5395(unit)) {
                reg_value = 0;
                reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, BRCM_TAG_CTRLr, 0, 0);
                reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, BRCM_TAG_CTRLr);
                temp = 1;
                (DRV_SERVICES(unit)->reg_field_set)
                    (unit, BRCM_TAG_CTRLr, &reg_value, BRCM_TAG_ENf, &temp);
                (DRV_SERVICES(unit)->reg_write)
                    (unit, reg_addr, &reg_value, reg_len);

            } else if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
                reg_value = 0;
                reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, BRCM_HDR_CTRLr, 0, 0);
                reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, BRCM_HDR_CTRLr);
                temp = 1;
                (DRV_SERVICES(unit)->reg_field_set)
                    (unit, BRCM_HDR_CTRLr, &reg_value, BRCM_HDR_ENf, &temp);
                (DRV_SERVICES(unit)->reg_write)
                    (unit, reg_addr, &reg_value, reg_len);
            }

            /*
             * Force MII Software Override, set 100 Full Link up
             * in MII port of mgnt chip
             */
             reg_value = 0;
             reg_addr = (DRV_SERVICES(unit)->reg_addr)
                 (unit, STS_OVERRIDE_IMPr, 0, 0);
             reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, STS_OVERRIDE_IMPr);
            temp = 1;
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, STS_OVERRIDE_IMPr, &reg_value, MII_SW_ORf, &temp);        
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, STS_OVERRIDE_IMPr, &reg_value, DUPLX_MODEf, &temp);
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, STS_OVERRIDE_IMPr, &reg_value, LINK_STSf, &temp);
            temp = 1;
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, STS_OVERRIDE_IMPr, &reg_value, SPEED_Rf, &temp);
            (DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value, reg_len);
        }

        if(SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)){
            /*
             * Force MII Software Override, set 100 Full Link up
             * in MII port of mgnt chip
             */
             reg_value = 0;
             reg_addr = (DRV_SERVICES(unit)->reg_addr)
                 (unit, STS_OVERRIDE_IMPr, 0, 0);
             reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, STS_OVERRIDE_IMPr);
            temp = 1;
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, STS_OVERRIDE_IMPr, &reg_value, SW_ORDf, &temp);        
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, STS_OVERRIDE_IMPr, &reg_value, DUPLX_MODEf, &temp);
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, STS_OVERRIDE_IMPr, &reg_value, LINK_STSf, &temp);
            temp = 1;
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, STS_OVERRIDE_IMPr, &reg_value, GIGA_SPEEDf, &temp);
            (DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value, reg_len);
        }
        
        /*
         * Enable All flow (unicast, multicast, broadcast)
         * in MII port of mgmt chip
         */
        if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
            reg_value = 0;
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, IMP_CTLr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, IMP_CTLr);
            temp = 1;
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, IMP_CTLr, &reg_value, RX_UCST_ENf, &temp);
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, IMP_CTLr, &reg_value, RX_MCST_ENf, &temp);
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, IMP_CTLr, &reg_value, RX_BCST_ENf, &temp);
            
        } else {
            reg_value = 0;
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MII_PCTLr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MII_PCTLr);
            temp = 1;
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, MII_PCTLr, &reg_value, MIRX_UC_ENf, &temp);
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, MII_PCTLr, &reg_value, MIRX_MC_ENf, &temp);
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, MII_PCTLr, &reg_value, MIRX_BC_ENf, &temp);
        }
        (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len);

    } else {

        if(SOC_IS_ROBO5324(unit)){
            /* clear MII Port State Override reg in non-MII mgnt chip */
            reg_value = 0;
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, STS_OVERRIDE_P24r, 24, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, STS_OVERRIDE_P24r);
            (DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value, reg_len);
        
            /* clear MII Port Port Control reg in non-MII mgmt chip */
            reg_value = 0;
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MII_PCTLr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MII_PCTLr);
            (DRV_SERVICES(unit)->reg_write)(unit, reg_addr, &reg_value, reg_len);
        }

        if(SOC_IS_ROBODINO(unit) || SOC_IS_ROBO5348(unit) ||
            SOC_IS_ROBO5347(unit)){
            /* clear MII Port State Override reg in non-MII mgnt chip */
            reg_value = 0;
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                 (unit, STS_OVERRIDE_IMPr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                 (unit, STS_OVERRIDE_IMPr);
            (DRV_SERVICES(unit)->reg_write)
                 (unit, reg_addr, &reg_value, reg_len);
     
            /* clear MII Port Port Control reg in non-MII mgmt chip */
            reg_value = 0;
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MII_PCTLr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MII_PCTLr);
            (DRV_SERVICES(unit)->reg_write)(unit, reg_addr, &reg_value, reg_len);
        }
    }

    /* Identify mgmt port is MII port */
    reg_value = 0;
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, GMNGCFGr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, GMNGCFGr);
    temp = 2;
    (DRV_SERVICES(unit)->reg_field_set)
        (unit, GMNGCFGr, &reg_value, FRM_MNGPf, &temp);
    temp = 1;
    (DRV_SERVICES(unit)->reg_field_set)
        (unit, GMNGCFGr, &reg_value, RXBPDU_ENf, &temp);
    (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len);
    if(SOC_IS_ROBO5324(unit)){
       /* Identify mgmt port in mgmt chip */
       reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MNGPIDr, 0, 0);
       reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MNGPIDr);
       (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_value, reg_len);
       temp = CMIC_PORT(soc_mii_unit);
       (DRV_SERVICES(unit)->reg_field_set)
            (unit, MNGPIDr, &reg_value, PORT_IDf, &temp);
       (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_robo_dma_attach
 * Purpose:
 *  Setup DMA structures whedn a device is attached.
 * Parameters:
 *  unit - StrataSwitch unit #.
 * Returns:
 *  SOC_E_NONE - Attached successful.
 *  SOC_E_xxx  - Attach failed.
 * Notes:
 *  Initializes data structure without regards to the current fields,
 *  calling this routine without detach first may result in memory
 *  leaks.
 */

int
soc_robo_dma_attach(int unit)
{
    int         rv;
    soc_control_t   *soc;

    _soc_robo_device_created(unit);
    soc = SOC_CONTROL(unit);
#ifdef ETH_MII_DEBUG
    SOC_DEBUG_PRINT((DK_PCI, "soc_robo_dma_attach: unit=%d\n", unit));
    /*
     * Attached flag must be true during initialization.
     * If initialization fails, the flag is cleared by soc_detach (below).
     */

    soc->soc_flags |= SOC_F_ATTACHED;

    if (soc_ndev_attached++ == 0) {
        int         chip;

        /* Work to be done before the first SOC device is attached. */
        for (chip = 0; chip < SOC_ROBO_NUM_SUPPORTED_CHIPS; chip++) {
            /* Call each chip driver's init function */
            if (soc_robo_base_driver_table[chip]->init) {
                (soc_robo_base_driver_table[chip]->init)();
            }
        }
    }
#endif
    soc->stat.dv_alloc      = 0;    /* Init Alloc count */
    soc->stat.dv_free       = 0;    /* Init Free count */
    soc->stat.dv_alloc_q    = 0;    /* Init Alloc from Q count */
    if ((rv = et_soc_attach(unit)) != SOC_E_NONE) {
        soc_cm_print("soc_robo_dma_attach: et_soc_attach failed %d\n", rv);
        return rv;
    }
    return et_soc_open(unit);
}

/*
 * Function:
 *  soc_robo_dma_detach
 * Purpose:
 *  Abort DMA active on the specified channel, and free
 *  internal memory associated with DMA on the specified unit.
 *  It is up to the caller to ensure NO more DMAs are started.
 * Parameters:
 *  unit - StrataSwitch Unit number
 * Returns:
 *  SOC_E_TIMEOUT - indicates attempts to abort active
 *      operations failed. Device is detached, but operation
 *      is undefined at this point.
 *  SOC_E_NONE - Operation sucessful.
 */

int
soc_robo_dma_detach(int unit)
{
    if (unit == soc_mii_unit) {
        return et_soc_close(unit);
    }
    return(SOC_E_NONE);
}

/*
 * Reuse pmux field.
 */
#define dv_sem      dv_public4.ptr
#define dv_poll     dv_public4.ptr

/*
 * Function:
 *  soc_robo_dma_dma_wait_done (Internal only)
 * Purpose:
 *  Callout for DMA chain done.
 * Parameters:
 *  unit - StrataSwitch Unit #.
 *  dv_chain - Chain completed.
 * Returns:
 *  Nothing
 */

STATIC void
soc_robo_dma_dma_wait_done(int unit, robo_dv_t *dv_chain)
{
    COMPILER_REFERENCE(unit);
    sal_sem_give((sal_sem_t)dv_chain->dv_sem);
}

/*
 * Function:
 *  soc_robo_dma_wait
 * Purpose:
 *  Start a DMA operation and wait for it's completion.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  dv_chain - pointer to dv chain to execute.
 *      usec - Time out in microseconds.  Same meanings as sal_sem_take
 * Returns:
 *  SOC_E_XXXX
 */

int
soc_robo_dma_wait_timeout(int unit, robo_dv_t *dv_chain, int usec)
{
    int     rv = SOC_E_NONE;
    sal_usecs_t start_time;
    int diff_time;

    dv_chain->dv_sem = sal_sem_create("dv_sem", sal_sem_BINARY, 0);
    if (!dv_chain->dv_sem) {
        return(SOC_E_MEMORY);
    }
    dv_chain->dv_done_chain = soc_robo_dma_dma_wait_done;

    soc_robo_dma_start(unit, dv_chain);

    start_time = sal_time_usecs();
    diff_time = 0;
    if (sal_sem_take((sal_sem_t)dv_chain->dv_sem, sal_sem_FOREVER)) {
        rv = SOC_E_TIMEOUT;
    }
    sal_sem_destroy((sal_sem_t)dv_chain->dv_sem);

    return(rv);
}

/*
 * Function:
 *  soc_robo_dma_wait
 * Purpose:
 *  Start a DMA operation and wait for it's completion.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  dv_chain - pointer to dv chain to execute.
 * Returns:
 *  SOC_E_XXXX
 */

int
soc_robo_dma_wait(int unit, robo_dv_t *dv_chain)
{
    return soc_robo_dma_wait_timeout(unit, dv_chain, sal_sem_FOREVER);
}

extern et_soc_info_t *et_soc;
extern void et_soc_init(et_soc_info_t *et, bool full);

void
soc_robo_dma_reinit(int unit)
{
    struct chops *chops;
    void *ch;

    chops = et_soc->etc->chops;
    ch = et_soc->etc->ch;
    ET_SOC_DMA_LOCK(et_soc);
    if (!(*chops->recover)(ch)) {
        /*
         * If we cannot recover the error using chip's 'recover' call,
         * then do a complete re-init.
         */
        et_soc_init(et_soc, TRUE);
    }
    ET_SOC_DMA_UNLOCK(et_soc);
}

