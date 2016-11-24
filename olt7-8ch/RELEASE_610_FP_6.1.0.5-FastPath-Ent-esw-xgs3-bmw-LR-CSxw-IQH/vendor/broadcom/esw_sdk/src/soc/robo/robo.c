/*
 * $Id: robo.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
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
 * File:        robo.c
 * Purpose:
 * Requires:
 */
#include <soc/robo.h>
#include <soc/drv.h>
#include <soc/mcm/robo/driver.h>
#include <soc/error.h>

#include <soc/debug.h>



#ifdef BCM_ROBO_SUPPORT

/* Will be added and changed later */
/*
 * Robo chip driver functions.  Common across Robo devices for now.
 * These may get broken out by chip in the future, but not needed yet.
 */
soc_functions_t soc_robo_drv_funs = {
    soc_robo_misc_init,
    soc_robo_mmu_init,
    soc_robo_age_timer_get,
    soc_robo_age_timer_max_get,
    soc_robo_age_timer_set
};

int bcm53222_attached = 0;

/*
 * soc_robo_5324_mmu_default_set():
 *   - Do 5324M MMU default Configuration.
 */
int
soc_robo_5324_mmu_default_set(int unit) 
{
    uint32          reg_addr, reg_len, temp;
    uint32          reg_value = 0;
    int             rv = SOC_E_NONE;

    /* FCON_FLOWMIXr: Page Offset = 0x0A 0x30 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_FLOWMIXr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_FLOWMIXr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_MCAST_DROPf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_UCAST_DROPf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_TXQ_PAUSEf, &temp));
    temp = 0;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_RX_DROPf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_RX_PAUSEf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_RX_FCON_CTRLr: Page Offset = 0x0A 0x92 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_RX_FCON_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_RX_FCON_CTRLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RX_FCON_CTRLr, &reg_value, EN_UNPAUSE_HDLf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_GLOB_TH_CTRL_2r: Page Offset = 0x0A 0x10 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_GLOB_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_GLOB_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xe7;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_2r, &reg_value, FCON_GLOB_DROP_THf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_2r, &reg_value, FCON_GLOB_MCDROP_THf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_GLOB_TH_CTRL_1r: Page Offset = 0x0A 0x0E */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_GLOB_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_GLOB_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xa3;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_1r, &reg_value, FCON_GLOB_PAUSE_THf, &temp));
    temp = 0x99;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_1r, &reg_value, FCON_GLOB_HYST_THf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q0_100_TH_CTRL_2r: Page Offset = 0x0A 0x08 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q0_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q0_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x2e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q1_100_TH_CTRL_2r: Page Offset = 0x0A 0x6C */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q1_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q1_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x2e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q2_100_TH_CTRL_2r: Page Offset = 0x0A 0x7A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q2_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q2_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x2e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q3_100_TH_CTRL_2r: Page Offset = 0x0A 0x88 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q3_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q3_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x2e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q0_100_TH_CTRL_1r: Page Offset = 0x0A 0x06 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q0_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q0_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x0b;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));
    temp = 0x7;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q1_100_TH_CTRL_1r: Page Offset = 0x0A 0x6A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q1_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q1_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x0b;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));
    temp = 0x7;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q2_100_TH_CTRL_1r: Page Offset = 0x0A 0x78 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q2_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q2_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x0b;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));
    temp = 0x7;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q3_100_TH_CTRL_1r: Page Offset = 0x0A 0x86 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q3_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q3_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x0b;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));
    temp = 0x7;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_MISC_TXFLOW_CTRLr: Page Offset = 0x0A 0x4A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_MISC_TXFLOW_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_MISC_TXFLOW_CTRLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_MISC_TXFLOW_CTRLr, &reg_value, RESERVE_BLANCEf, &temp));
    temp = 0xe7;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_MISC_TXFLOW_CTRLr, &reg_value, TOTAL_1QTH_DROPf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_RSRV_BUFNUMr: Page Offset = 0x0A 0x90 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_RSRV_BUFNUMr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_RSRV_BUFNUMr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x8;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RSRV_BUFNUMr, &reg_value, RXBASE_RESV_BUFNUMf, &temp));
    temp = 0x4;
    if (SOC_IS_ROBO5324_A1(unit)) {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, FCON_RSRV_BUFNUMr, &reg_value, RUNOFFf, &temp));
    } else {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, FCON_RSRV_BUFNUMr, &reg_value, RESERVED_Rf, &temp));
    }

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    return SOC_E_NONE;
}

/*
 * soc_robo_5348_mmu_default_set():
 *   - Do 5348 MMU default Configuration.
 */
int
soc_robo_5348_mmu_default_set(int unit) 
{
    uint32          reg_addr, reg_len, temp;
    uint32          reg_value = 0;
    int             rv = SOC_E_NONE;

    /* FCON_RX_FCON_CTRLr: Page Offset = 0x0A 0x92 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_RX_FCON_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_RX_FCON_CTRLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x63e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RX_FCON_CTRLr, &reg_value, RXFCON_PAUSE_THf, &temp));

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RX_FCON_CTRLr, &reg_value, EN_RXCON_PAUSE_REMAPf, &temp));

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RX_FCON_CTRLr, &reg_value, EN_UNPAUSE_HDLf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_FLOWMIXr: Page Offset = 0x0A 0x30 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_FLOWMIXr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_FLOWMIXr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_MCAST_DROPf, &temp));

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_UCAST_DROPf, &temp));

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_TXQ_PAUSEf, &temp));

    temp = 0x0;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_RX_DROPf, &temp));

    temp = 0x0;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_RX_PAUSEf, &temp));

    temp = 0x3;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, QOS_RSRV_QUOTA_OPTf, &temp));

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_PARKING_PREVENTIONf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_GLOB_TH_CTRL_2r: Page Offset = 0x0A 0x10 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_GLOB_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_GLOB_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xed;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_2r, &reg_value, FCON_GLOB_DROP_THf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_2r, &reg_value, FCON_GLOB_MCDROP_THf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_GLOB_TH_CTRL_1r: Page Offset = 0x0A 0x0E */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_GLOB_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_GLOB_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xaf;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_1r, &reg_value, FCON_GLOB_PAUSE_THf, &temp));
    temp = 0x88;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_1r, &reg_value, FCON_GLOB_HYST_THf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q0_100_TH_CTRL_2r: Page Offset = 0x0A 0x08 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q0_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q0_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xde;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q1_100_TH_CTRL_2r: Page Offset = 0x0A 0x6C */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q1_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q1_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xde;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q2_100_TH_CTRL_2r: Page Offset = 0x0A 0x7A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q2_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q2_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xde;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q3_100_TH_CTRL_2r: Page Offset = 0x0A 0x88 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q3_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q3_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xde;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q0_100_TH_CTRL_1r: Page Offset = 0x0A 0x06 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q0_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q0_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x27;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));

    temp = 0x1e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q1_100_TH_CTRL_1r: Page Offset = 0x0A 0x6A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q1_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q1_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x27;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));

    temp = 0x1e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q2_100_TH_CTRL_1r: Page Offset = 0x0A 0x78 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q2_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q2_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x27;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));

    temp = 0x1e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q3_100_TH_CTRL_1r: Page Offset = 0x0A 0x86 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q3_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q3_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x27;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));

    temp = 0x1e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_MISC_TXFLOW_CTRLr: Page Offset = 0x0A 0x4A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_MISC_TXFLOW_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_MISC_TXFLOW_CTRLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_MISC_TXFLOW_CTRLr, &reg_value, RESERVE_BLANCEf, &temp));
    temp = 0xed;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_MISC_TXFLOW_CTRLr, &reg_value, TOTAL_LQTH_DROPf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_RXBASE_BUFNUMr: Page Offset = 0x0A 0x8A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_RXBASE_BUFNUMr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_RXBASE_BUFNUMr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x8;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RXBASE_BUFNUMr, &reg_value, RXBASE_RESV_BUFNUMf, &temp));

    temp = 0x8;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RXBASE_BUFNUMr, &reg_value, RXBASE_HYST_BUFNUMf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_RSRV_BUFNUMr: Page Offset = 0x0A 0x90 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_RSRV_BUFNUMr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_RSRV_BUFNUMr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x8;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RSRV_BUFNUMr, &reg_value, RUNOFFf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    return SOC_E_NONE;
}

/*
 * soc_robo_53242_mmu_default_set():
 *   - Do 53242 MMU default Configuration.
 */
int
soc_robo_53242_mmu_default_set(int unit) 
{
    uint32          reg_addr, reg_len, temp;
    uint32          reg_value = 0;
    int             rv = SOC_E_NONE;

    /* FCON_Q0_100_TH_CTRL_1r: Page Offset = 0x0A 0x06 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q0_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q0_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x13;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));
    temp = 0x1c;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q0_100_TH_CTRL_2r: Page Offset = 0x0A 0x08 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q0_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q0_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x98;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));
    temp = 0x73;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q0_100_TH_CTRL_2r, &reg_value, BT100_MCDROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_GLOB_TH_CTRL_1r: Page Offset = 0x0A 0x0E */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_GLOB_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_GLOB_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x44;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_1r, &reg_value, FCON_GLOB_HYST_THf, &temp));
    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_1r, &reg_value, FCON_GLOB_PAUSE_THf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_GLOB_TH_CTRL_2r: Page Offset = 0x0A 0x10 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_GLOB_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_GLOB_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x9b;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_2r, &reg_value, FCON_GLOB_DROP_THf, &temp));
    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_GLOB_TH_CTRL_2r, &reg_value, FCON_GLOB_MCDROP_THf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_FLOWMIXr: Page Offset = 0x0A 0x30 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_FLOWMIXr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_FLOWMIXr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x3;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, QOS_RSRV_QUOTA_OPTf, &temp));
    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_PARKING_PREVENTIONf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_MCAST_BLANCEf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_MCAST_DROPf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_UCAST_DROPf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_TXQ_PAUSEf, &temp));
    temp = 0x0;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_RX_DROPf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_FLOWMIXr, &reg_value, EN_RX_PAUSEf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_MISC_TXFLOW_CTRLr: Page Offset = 0x0A 0x4A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_MISC_TXFLOW_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_MISC_TXFLOW_CTRLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_MISC_TXFLOW_CTRLr, &reg_value, RESERVE_BLANCEf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q1_100_TH_CTRL_1r: Page Offset = 0x0A 0x6A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q1_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q1_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x13;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));
    temp = 0x1c;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q1_100_TH_CTRL_2r: Page Offset = 0x0A 0x6C */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q1_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q1_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x98;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));
    temp = 0x73;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q1_100_TH_CTRL_2r, &reg_value, BT100_MCDROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q2_100_TH_CTRL_1r: Page Offset = 0x0A 0x78 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q2_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q2_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x13;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));
    temp = 0x1c;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q2_100_TH_CTRL_2r: Page Offset = 0x0A 0x7A */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q2_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q2_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x98;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));
    temp = 0x73;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q2_100_TH_CTRL_2r, &reg_value, BT100_MCDROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q3_100_TH_CTRL_1r: Page Offset = 0x0A 0x86 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q3_100_TH_CTRL_1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q3_100_TH_CTRL_1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x13;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_1r, &reg_value, BT100_HYST_THRSf, &temp));
    temp = 0x1c;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_1r, &reg_value, BT100_PAUS_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_Q3_100_TH_CTRL_2r: Page Offset = 0x0A 0x88 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_Q3_100_TH_CTRL_2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_Q3_100_TH_CTRL_2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x98;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_2r, &reg_value, BT100_DROP_THRSf, &temp));
    temp = 0x73;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_Q3_100_TH_CTRL_2r, &reg_value, BT100_MCDROP_THRSf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_RX_FCON_CTRLr: Page Offset = 0x0A 0x92 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_RX_FCON_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_RX_FCON_CTRLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_RX_FCON_CTRLr, &reg_value, EN_UNPAUSE_HDLf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_DLF_TH_CTRLr: Page Offset = 0x0A 0x94 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_DLF_TH_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_DLF_TH_CTRLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_DLF_TH_CTRLr, &reg_value, TOTAL_INDV_DLFTH_DROPf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* FCON_BCST_TH_CTRLr: Page Offset = 0x0A 0x96 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, FCON_BCST_TH_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, FCON_BCST_TH_CTRLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, FCON_BCST_TH_CTRLr, &reg_value, TOTAL_INDV_BCSTTH_DROPf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_HYST_THRESH_Q1r: Page Offset = 0x0A 0xC0 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_HYST_THRESH_Q1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_HYST_THRESH_Q1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x46;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_HYST_THRESH_Q1r, &reg_value, TL_HYST_TH_Q1f, &temp));
    temp = 0x80;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_HYST_THRESH_Q1r, &reg_value, TL_PAUSE_TH_Q1f, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_DROP_THRESH_Q1r: Page Offset = 0x0A 0xC2 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_DROP_THRESH_Q1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_DROP_THRESH_Q1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x9d;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DROP_THRESH_Q1r, &reg_value, TL_DROP_TH_Q1f, &temp));
    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DROP_THRESH_Q1r, &reg_value, RESERVED_Rf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_HYST_THRESH_Q2r: Page Offset = 0x0A 0xC4 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_HYST_THRESH_Q2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_HYST_THRESH_Q2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x48;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_HYST_THRESH_Q2r, &reg_value, TL_HYST_TH_Q2f, &temp));
    temp = 0x82;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_HYST_THRESH_Q2r, &reg_value, TL_PAUSE_TH_Q2f, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_DROP_THRESH_Q2r: Page Offset = 0x0A 0xC6 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_DROP_THRESH_Q2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_DROP_THRESH_Q2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x9f;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DROP_THRESH_Q2r, &reg_value, TL_DROP_TH_Q2f, &temp));
    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DROP_THRESH_Q2r, &reg_value, RESERVED_Rf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_HYST_THRESH_Q3r: Page Offset = 0x0A 0xC8 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_HYST_THRESH_Q3r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_HYST_THRESH_Q3r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x4a;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_HYST_THRESH_Q3r, &reg_value, TL_HYST_TH_Q3f, &temp));
    temp = 0x84;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_HYST_THRESH_Q3r, &reg_value, TL_PAUSE_TH_Q3f, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_DROP_THRESH_Q3r: Page Offset = 0x0A 0xCA */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_DROP_THRESH_Q3r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_DROP_THRESH_Q3r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0xa1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DROP_THRESH_Q3r, &reg_value, TL_DROP_TH_Q3f, &temp));
    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DROP_THRESH_Q3r, &reg_value, RESERVED_Rf, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_DLF_DROP_THRESH_Q1r: Page Offset = 0x0A 0xD0 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_DLF_DROP_THRESH_Q1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_DLF_DROP_THRESH_Q1r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DLF_DROP_THRESH_Q1r, &reg_value, TOTAL_DLF_DROP_THRESH_Q1f, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DLF_DROP_THRESH_Q1r, &reg_value, TOTAL_BC_DROP_THRESH_Q1f, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_DLF_DROP_THRESH_Q2r: Page Offset = 0x0A 0xD2 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_DLF_DROP_THRESH_Q2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_DLF_DROP_THRESH_Q2r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DLF_DROP_THRESH_Q2r, &reg_value, TOTAL_DLF_DROP_THRESH_Q2f, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DLF_DROP_THRESH_Q2r, &reg_value, TOTAL_BC_DROP_THRESH_Q2f, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    /* TOTAL_DLF_DROP_THRESH_Q3r: Page Offset = 0x0A 0xD4 */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, TOTAL_DLF_DROP_THRESH_Q3r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, TOTAL_DLF_DROP_THRESH_Q3r);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    temp = 0x7e;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DLF_DROP_THRESH_Q3r, &reg_value, TOTAL_DLF_DROP_THRESH_Q3f, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, TOTAL_DLF_DROP_THRESH_Q3r, &reg_value, TOTAL_BC_DROP_THRESH_Q3f, &temp));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    return SOC_E_NONE;
}

/* soc_misc_init() :
 *  - allowed user to do the init by chip dependant configuration.
 *
 *  Note : 
 *   1. below routine is for all Roob chip related init routine.
 *   2. different robo chip init section may separated by 
 *      "SOC_IS_ROBO53xx(unit)"
 */
int
soc_robo_misc_init(int unit)
{
    uint32          reg_addr, reg_len, temp;
    uint32          reg_value = 0;
    int             rv = SOC_E_NONE;
    
    uint32  reg_index, field_index;
    
    /* CheckMe: bcm5324 misc init may change to this function.
     *       (cucrrently the bcm5324 misc init is located in bcm_robo_init())
     */
    if (SOC_IS_ROBO5324(unit)){
        soc_robo_5324_mmu_default_set(unit);
    } else if (SOC_IS_ROBO5396(unit)){
        /* enable Exterenal PHY auto-polling */
        reg_addr = (DRV_SERVICES(unit)->reg_addr)
                        (unit, EXTPHY_SCAN_CTLr, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
                        (unit, EXTPHY_SCAN_CTLr);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }

        temp = 1;
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, EXTPHY_SCAN_CTLr, &reg_value, PHY_SCAN_ENf, &temp));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len));
        SOC_ERROR_PRINT((DK_ERR, 
                    "misc_init: External PHY auto-polling enabled\n"));
        
    } else if (SOC_IS_ROBO5397(unit)){
#ifdef WAN_PORT_SUPPORT
        reg_addr = (DRV_SERVICES(unit)->reg_addr)
                        (unit, WAN_PORT_SELr, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
                        (unit, WAN_PORT_SELr);
        temp = 0x80; /* Select WAN port */
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, WAN_PORT_SELr, &reg_value, WAN_SELECTf, &temp));

        temp = 1; /* Set non-Egress direct to WAN port */
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, WAN_PORT_SELr, &reg_value, EN_MAN2WANf, &temp));

        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len));
#endif
    } else if (SOC_IS_ROBO5348(unit) ||  SOC_IS_ROBO5347(unit)){
        soc_robo_5348_mmu_default_set(unit);
    } else if (SOC_IS_ROBO53242(unit) ||  SOC_IS_ROBO53262(unit)){
        soc_robo_53242_mmu_default_set(unit);

        reg_addr = (DRV_SERVICES(unit)->reg_addr)
            (unit, BONDING_PADr, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
            (unit, BONDING_PADr);
        (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_value, reg_len);

        reg_value &= 0x1f;
        reg_value >>= 1;

        if (reg_value == 0xf) {
            bcm53222_attached = 1;
        }
    /* appended for next robo chip */
    } else {
    }
    
    /* disable device base PHY auto-scan behavior : 
    *   - checkMe : check if only bcm5324, bcm5348 and bcm5396 has device 
    *           base enable/disable setting for phy auto-scan.
    */
    if  (SOC_IS_ROBO5324(unit) || SOC_IS_ROBO5348(unit) || 
        SOC_IS_ROBO5396(unit) || SOC_IS_ROBO5347(unit) ||
        SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)){
        if (SOC_IS_ROBO5324(unit) || SOC_IS_ROBO5348(unit)||
            SOC_IS_ROBO5347(unit) || SOC_IS_ROBO53242(unit)||
            SOC_IS_ROBO53262(unit)){
            reg_index = PHYSCAN_CTLr;
            field_index = EN_PHY_SCANf;
        } else {    /* bcm5396 only */
            reg_index = EXTPHY_SCAN_CTLr;
            field_index = PHY_SCAN_ENf;
        }

        reg_addr = (DRV_SERVICES(unit)->reg_addr)
                        (unit, reg_index, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
                        (unit, reg_index);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }

        temp = 0;
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, reg_index, &reg_value, field_index, &temp));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len));
        
    }
    
    /* reset the Traffice remarking on Non-ResEPacket */
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)){        
        soc_port_t  port;
        uint32      pri;
        
        /* bcm5395 on this feature is not implemented, so this reset exclude 
         * bcm5395 related setting.
         */
         
        /* reset the control register */
        reg_index = TRREG_CTRLr;
        reg_addr = (DRV_SERVICES(unit)->reg_addr)
                        (unit, reg_index, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
                        (unit, reg_index);
        
        if ((rv = (DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
        temp = 0;
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, reg_index, &reg_value, PCP_RMK_ENf, &temp));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, reg_index, &reg_value, CFI_RMK_ENf, &temp));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len));
        
        /* reset the TC2PCP mapping */
        PBMP_ALL_ITER(unit, port){
            for (pri = 0; pri <=7; pri++){
                
                /* the new-pri is formed as {CFI(bit4),PRI(bit3-bit0)} 
                 *  - in the reset value, the CFI is rewrite to 0 always!
                 *      (default is 1 on the RV=1 field)
                 */
                SOC_IF_ERROR_RETURN(
                        (DRV_SERVICES(unit)->port_pri_mapop_set)
                        (unit, port, DRV_PORT_OP_NORMAL_TC2PCP,
                            pri, pri, 0));
                
                /* outband TC2PCP is supported on bcm53115 only */
                if (SOC_IS_ROBO53115(unit)){
                    SOC_IF_ERROR_RETURN(
                            (DRV_SERVICES(unit)->port_pri_mapop_set)
                            (unit, port, DRV_PORT_OP_OUTBAND_TC2PCP,
                                pri, pri, 0));
                    
                }
            }
        }
    }
    
    SOC_ERROR_PRINT((DK_VERBOSE, "soc_robo_misc_init: OK\n"));
    
    return rv;
}

int
soc_robo_mmu_init(int unit)
{
    return SOC_E_NONE;
}

int
soc_robo_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    return SOC_E_NONE;
}

int
soc_robo_age_timer_max_get(int unit, int *max_seconds)
{
    return SOC_E_NONE;
}

int
soc_robo_age_timer_set(int unit, int age_seconds, int enable)
{
    return SOC_E_NONE;
}

/*
 * soc_robo_64_val_to_pbmp() :
 *     -- Get port bitmap from unsigned 64-bits integer variable.
 * 
 * unit: unit
 * *pbmp: (OUT) returned port bitmap
 * value64: (IN) data value for transfering into port bitmap
 */
int
soc_robo_64_val_to_pbmp(int unit, soc_pbmp_t *pbmp, uint64 value64)
{
    uint32 value32;

    COMPILER_64_TO_32_LO(value32, value64);
    SOC_PBMP_WORD_SET(*pbmp, 0, value32);
    if (SOC_INFO(unit).port_num > 32) {
        COMPILER_64_TO_32_HI(value32, value64);
        SOC_PBMP_WORD_SET(*pbmp, 1, value32);
    } else {
        SOC_PBMP_WORD_SET(*pbmp, 1, 0);
    }
    
    return SOC_E_NONE;
}

/*
 * soc_robo_64_pbmp_to_val() :
 *     -- Transfer port bitmap into unsigned 64-bits integer variable.
 * 
 * unit: unit
 * *pbmp: (IN) returned port bitmap
 * *value64: (OUT) data value for transfering into port bitmap
 */
int
soc_robo_64_pbmp_to_val(int unit, soc_pbmp_t *pbmp, uint64 *value64)
{
    uint32 value_h, value_l;

    value_l = SOC_PBMP_WORD_GET(*pbmp, 0);
    if (SOC_INFO(unit).port_num > 32) {
        value_h = SOC_PBMP_WORD_GET(*pbmp, 1);
    } else {
        value_h = 0;
    }
    COMPILER_64_SET(*value64, value_h, value_l);

    return SOC_E_NONE;
}

int
soc_robo_loop_detect_enable_set(int unit, int enable)
{
    uint32 reg_addr, reg_len;
    uint32 reg_value = 0;
    int rv = SOC_E_NONE;

    /* enable Loop Detection */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, LPDET_CFGr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, LPDET_CFGr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    /* enable/disable loop detection */
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, LPDET_CFGr, &reg_value, EN_LPDETf, (uint32 *) &enable));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    return SOC_E_NONE;
}

int
soc_robo_loop_detect_enable_get(int unit, int *enable)
{
    uint32 reg_addr, reg_len;
    uint32 reg_value = 0;
    int rv = SOC_E_NONE;

    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, LPDET_CFGr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, LPDET_CFGr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, LPDET_CFGr, &reg_value, EN_LPDETf, (uint32 *) enable));

    return SOC_E_NONE;
}

int
soc_robo_loop_detect_address_set(int unit, sal_mac_addr_t macaddr)
{
    uint32 reg_addr, reg_len;
    uint64 reg_v64, mac_field;

    SAL_MAC_ADDR_TO_UINT64(macaddr, mac_field);

    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, LD_FRM_SAr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, LD_FRM_SAr);

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, LD_FRM_SAr, (uint32 *)&reg_v64, LD_SAf, (uint32 *)&mac_field));

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, (uint32 *)&reg_v64, reg_len));

    return SOC_E_NONE;
}

int
soc_robo_loop_detect_pbmp_get(int unit, soc_pbmp_t *pbmp)
{
    uint32 reg_addr, reg_len, reg_value32, pbmp_value32;
    uint64 pbmp_value64, reg_value64 = 0;
    int rv = SOC_E_NONE;

    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, LED_PORTMAPr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, LED_PORTMAPr);
    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)) {
        if ((rv = (DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, (uint32 *)&reg_value64, reg_len)) < 0) {
            return rv;
        }
        if (SOC_IS_ROBO5348(unit)) {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, LED_PORTMAPr, (uint32 *)&reg_value64, 
            LED_WARNING_PORTMAPf, (uint32 *)&pbmp_value64));
        soc_robo_64_val_to_pbmp(unit, pbmp, pbmp_value64);
        } else { /* 5347 */
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, LED_PORTMAPr, (uint32 *)&reg_value64, 
                LED_WARNING_PORTMAPf, (uint32 *)&pbmp_value32));
            SOC_PBMP_WORD_SET(*pbmp, 0, pbmp_value32);
        }
    } else {
        if ((rv = (DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value32, reg_len)) < 0) {
            return rv;
        }
    
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, LED_PORTMAPr, &reg_value32, LED_WARNING_PORTMAPf, &pbmp_value32));
    
        SOC_PBMP_WORD_SET(*pbmp, 0, pbmp_value32);
    }

    return SOC_E_NONE;
}

#endif  /* BCM_ROBO_SUPPORT */
