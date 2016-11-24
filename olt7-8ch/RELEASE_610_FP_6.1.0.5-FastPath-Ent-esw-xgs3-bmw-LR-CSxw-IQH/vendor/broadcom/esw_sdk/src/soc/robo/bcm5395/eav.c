/*
 * $Id: eav.c,v 1.1 2011/04/18 17:11:09 mruas Exp $
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
 * Field Processor related CLI commands
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv_if.h>
#include <soc/drv.h>
#include <soc/debug.h>


#define EAV_BCM5395_MAX_TICK_INC    63
#define EAV_BCM5395_MAX_TICK_ADJUST_PERIOD    15
#define EAV_BCM5395_MAX_SLOT_ADJUST_PERIOD    15
#define EAV_BCM5395_MAX_TICK_ONE_SLOT    3126
#define EAV_BCM5395_MAX_SLOT_NUMBER    31

#define EAV_BCM53118_MAX_PCP_VALUE    0x7

/* Bytes count allowed for EAV Class4/Class5 bandwidth within a slot time */
#define EAV_5395_MAX_BANDWIDTH_VALUE 16383
#define EAV_5395_MIN_BANDWIDTH_VALUE 0

int 
drv_bcm5395_eav_control_set(int unit, uint32 type, uint32 param)
{
    int rv = SOC_E_NONE;
    uint32 reg_addr, reg_value, temp;
    int reg_len;
    
    switch (type) {
        case DRV_EAV_CONTROL_TIME_STAMP_TO_IMP:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, TM_STAMP_RPT_CTRLr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, TM_STAMP_RPT_CTRLr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            if (param){
                temp = 1;
            } else {
                temp = 0;
            }
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, TM_STAMP_RPT_CTRLr, 
                &reg_value, TSRPT_PKT_ENf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_CONTROL_MAX_AV_SIZE:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_MAX_AV_PKT_SZr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_MAX_AV_PKT_SZr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            temp = param;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_MAX_AV_PKT_SZr, 
                &reg_value, MAX_AV_PKT_SZf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_CONTROL_STREAM_CLASSA_PCP:
            if (SOC_IS_ROBO53118(unit)) {
                reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, CLASS_PCPr, 0, 0);
                reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, CLASS_PCPr);

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value,reg_len));

                if (param > EAV_BCM53118_MAX_PCP_VALUE) {
                    return SOC_E_PARAM;
                }

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                    (unit, CLASS_PCPr, &reg_value, CLASSA_PCPf, &param));

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                    (unit, reg_addr, &reg_value,reg_len));
            } else {
                rv = SOC_E_UNAVAIL;
            }
            break;
        case DRV_EAV_CONTROL_STREAM_CLASSB_PCP:
            if (SOC_IS_ROBO53118(unit)) {
                reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, CLASS_PCPr, 0, 0);
                reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, CLASS_PCPr);

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value,reg_len));

                if (param > EAV_BCM53118_MAX_PCP_VALUE) {
                    return SOC_E_PARAM;
                }

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                    (unit, CLASS_PCPr, &reg_value, CLASSB_PCPf, &param));

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                    (unit, reg_addr, &reg_value,reg_len));
            } else {
                rv = SOC_E_UNAVAIL;
            }
            break;
        default:
            rv = SOC_E_PARAM;
    }

    return rv;
}


int 
drv_bcm5395_eav_control_get(int unit, uint32 type, uint32 *param)
{
    int rv = SOC_E_NONE;
    uint32 reg_addr, reg_value, temp;
    int reg_len;
    
    switch (type) {
        case DRV_EAV_CONTROL_TIME_STAMP_TO_IMP:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, TM_STAMP_RPT_CTRLr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, TM_STAMP_RPT_CTRLr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, TM_STAMP_RPT_CTRLr, 
                &reg_value, TSRPT_PKT_ENf, &temp));
            if (temp) {
                *param = TRUE;
            } else {
                *param = FALSE;
            }
            break;
        case DRV_EAV_CONTROL_MAX_AV_SIZE:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_MAX_AV_PKT_SZr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_MAX_AV_PKT_SZr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_MAX_AV_PKT_SZr, 
                &reg_value, MAX_AV_PKT_SZf, &temp));
            *param = temp;
            break;
        case DRV_EAV_CONTROL_STREAM_CLASSA_PCP:
            if (SOC_IS_ROBO53118(unit)) {
                reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, CLASS_PCPr, 0, 0);
                reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, CLASS_PCPr);

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value,reg_len));

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                    (unit, CLASS_PCPr, &reg_value, CLASSA_PCPf, &temp));
                *param = temp;
            } else {
                rv = SOC_E_UNAVAIL;
            }
            break;
        case DRV_EAV_CONTROL_STREAM_CLASSB_PCP:
            if (SOC_IS_ROBO53118(unit)) {
                reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, CLASS_PCPr, 0, 0);
                reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, CLASS_PCPr);

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value,reg_len));

                SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                    (unit, CLASS_PCPr, &reg_value, CLASSB_PCPf, &temp));
                *param = temp;
            } else {
                rv = SOC_E_UNAVAIL;
            }
            break;
        default:
            rv = SOC_E_PARAM;
    }

    return rv;
}

int 
drv_bcm5395_eav_enable_set(int unit, uint32 port, uint32 enable)
{
    uint32 reg_addr, reg_value, temp;
    int reg_len;

    /* Set EAV enable register */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
        (unit, RESE_AV_EN_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
        (unit, RESE_AV_EN_CTRLr);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value,reg_len));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, RESE_AV_EN_CTRLr, &reg_value, AV_ENf, &temp));
    if (enable){
        temp |= 0x1 << port;
    } else {
        temp &= ~(0x1 << port);
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, RESE_AV_EN_CTRLr, &reg_value, AV_ENf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value,reg_len));
    
    return SOC_E_NONE;
}

int 
drv_bcm5395_eav_enable_get(int unit, uint32 port, uint32 *enable)
{
    uint32 reg_addr, reg_value, temp;
    int reg_len;
    
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
        (unit, RESE_AV_EN_CTRLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
        (unit, RESE_AV_EN_CTRLr);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value,reg_len));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, RESE_AV_EN_CTRLr, &reg_value, AV_ENf, &temp));
    if (temp & (0x1 << port)){
        *enable = TRUE;
    } else {
        *enable = FALSE;
    }
    return SOC_E_NONE;
}

int 
drv_bcm5395_eav_link_status_set(int unit, uint32 port, uint32 link)
{
    uint32 reg_addr, reg_value, temp;
    int reg_len;

    /* Set EAV Link register */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
        (unit, EAV_LNK_STATUSr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
        (unit, EAV_LNK_STATUSr);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value,reg_len));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, EAV_LNK_STATUSr, &reg_value, 
        PT_EAV_LNK_STATUSf, &temp));
    if (link){
        temp |= 0x1 << port;
    } else {
        temp &= ~(0x1 << port);
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, EAV_LNK_STATUSr, &reg_value, 
        PT_EAV_LNK_STATUSf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value,reg_len));
    
    return SOC_E_NONE;
}

int 
drv_bcm5395_eav_link_status_get(int unit, uint32 port, uint32 *link)
{
    uint32 reg_addr, reg_value, temp;
    int reg_len;
    
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
        (unit, EAV_LNK_STATUSr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
        (unit, EAV_LNK_STATUSr);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value,reg_len));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, EAV_LNK_STATUSr, &reg_value, 
        PT_EAV_LNK_STATUSf, &temp));
    if (temp & (0x1 << port)){
        *link = TRUE;
    } else {
        *link = FALSE;
    }
    return SOC_E_NONE;
}

int
drv_bcm5395_eav_egress_timestamp_get(int unit, uint32 port,
    uint32 *timestamp)
{
    uint32 reg_addr, reg_value, temp;
    int reg_len;

    /* Check Valid Status */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
        (unit, TM_STAMP_STATUSr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
        (unit, TM_STAMP_STATUSr);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value,reg_len));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, TM_STAMP_STATUSr, &reg_value, VALID_STATUSf, &temp));
    
    if ((temp & (0x1 << port)) == 0) {
        return SOC_E_EMPTY;
    }

    /* Get Egress Time STamp Value */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
        (unit, RESE_EGRESS_TM_STAMPr, port, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
        (unit, RESE_EGRESS_TM_STAMPr);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value,reg_len));
    *timestamp = reg_value;
    return SOC_E_NONE;
}


int 
drv_bcm5395_eav_time_sync_set(int unit, uint32 type, uint32 p0, uint32 p1)
{
    int rv = SOC_E_NONE;
    uint32 reg_addr, reg_value, temp;
    int reg_len;
    
    switch (type) {
        case DRV_EAV_TIME_SYNC_TIME_BASE:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_TM_BASEr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_TM_BASEr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            temp = p0;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_TM_BASEr, 
                &reg_value, TM_BASEf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_TIME_SYNC_TIME_ADJUST:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_TM_ADJr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_TM_ADJr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            if ((p0 > EAV_BCM5395_MAX_TICK_INC) || 
                (p1 > EAV_BCM5395_MAX_TICK_ADJUST_PERIOD)) {
                return SOC_E_PARAM;
            }
            temp = p0;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_TM_ADJr, 
                &reg_value, TM_INCf, &temp));
            temp = p1;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_TM_ADJr, 
                &reg_value, TM_ADJ_PRDf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_TIME_SYNC_TICK_COUNTER:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_SLOT_TICK_CNTRr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_SLOT_TICK_CNTRr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            if (p0 > EAV_BCM5395_MAX_TICK_ONE_SLOT) {
                return SOC_E_PARAM;
            }
            temp = p0;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_SLOT_TICK_CNTRr, 
                &reg_value, TICK_CNTRf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_TIME_SYNC_SLOT_NUMBER:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_SLOT_TICK_CNTRr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_SLOT_TICK_CNTRr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            if (p0 > EAV_BCM5395_MAX_SLOT_NUMBER) {
                return SOC_E_PARAM;
            }
            temp = p0;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_SLOT_TICK_CNTRr, 
                &reg_value, SLOT_NUMf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_TIME_SYNC_MACRO_SLOT_PERIOD:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_SLOT_ADJr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_SLOT_ADJr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            switch (p0) {
                case 1:
                    temp = 0;
                    break;
                case 2:
                    temp = 1;
                    break;
                case 4:
                    temp = 2;
                    break;
                default:
                    rv = SOC_E_PARAM;
                    return rv;
            }
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_SLOT_ADJr, 
                &reg_value, MCRO_SLOT_PRDf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_TIME_SYNC_SLOT_ADJUST:
            
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_SLOT_ADJr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_SLOT_ADJr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            switch (p0) {
                case 3125:
                    temp = 0;
                    break;
                case 3126:
                    temp = 1;
                    break;
                case 3124:
                    temp = 2;
                    break;
                default:
                    rv = SOC_E_PARAM;
                    return rv;
            }
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_SLOT_ADJr, 
                &reg_value, SLOT_ADJf, &temp));
            if (p1 >= 16) {
                rv =  SOC_E_PARAM;
                return rv;
            }
            temp = p1;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_SLOT_ADJr, 
                &reg_value, SLOT_ADJ_PRDf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        default:
            rv = SOC_E_PARAM;
    }

    return rv;
}

int 
drv_bcm5395_eav_time_sync_get(int unit, uint32 type, uint32 *p0, uint32 *p1)
{
    int rv = SOC_E_NONE;
    uint32 reg_addr, reg_value, temp;
    int reg_len;
    
    switch (type) {
        case DRV_EAV_TIME_SYNC_TIME_BASE:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_TM_BASEr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_TM_BASEr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_TM_BASEr, 
                &reg_value, TM_BASEf, &temp));
            *p0 = temp;
            break;
        case DRV_EAV_TIME_SYNC_TIME_ADJUST:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_TM_ADJr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_TM_ADJr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_TM_ADJr, 
                &reg_value, TM_INCf, &temp));
            *p0 = temp;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_TM_ADJr, 
                &reg_value, TM_ADJ_PRDf, &temp));
            *p1 = temp;
            break;
        case DRV_EAV_TIME_SYNC_TICK_COUNTER:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_SLOT_TICK_CNTRr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_SLOT_TICK_CNTRr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_SLOT_TICK_CNTRr, 
                &reg_value, TICK_CNTRf, &temp));
            *p0 = temp;
            break;
        case DRV_EAV_TIME_SYNC_SLOT_NUMBER:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_SLOT_TICK_CNTRr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_SLOT_TICK_CNTRr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_SLOT_TICK_CNTRr, 
                &reg_value, SLOT_NUMf, &temp));
            *p0 = temp;
            break;
        case DRV_EAV_TIME_SYNC_MACRO_SLOT_PERIOD:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_SLOT_ADJr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_SLOT_ADJr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_SLOT_ADJr, 
                &reg_value, MCRO_SLOT_PRDf, &temp));
            switch(temp) {
                case 0:
                    *p0 = 1;
                    break;
                case 1:
                    *p0 = 2;
                    break;
                case 2:
                    *p0 = 4;
                    break;
                default:
                    rv = SOC_E_INTERNAL;
                    return rv;
            }
            break;
        case DRV_EAV_TIME_SYNC_SLOT_ADJUST:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_SLOT_ADJr, 0, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_SLOT_ADJr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_SLOT_ADJr, 
                &reg_value, SLOT_ADJf, &temp));
            switch (temp) {
                case 0:
                    *p0 = 3125;
                    break;
                case 1:
                    *p0 = 3126;
                    break;
                case 2:
                    *p0 = 3124;
                    break;
                default:
                    return SOC_E_INTERNAL;
            }
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_SLOT_ADJr, 
                &reg_value, SLOT_ADJ_PRDf, &temp));
            *p1 = temp;
            break;
        default:
            rv = SOC_E_PARAM;
    }

    return rv;
}

int 
drv_bcm5395_eav_queue_control_set(int unit, 
    uint32 port, uint32 type, uint32 param)
{
    int rv = SOC_E_NONE;
    uint32 reg_addr, reg_value, temp;
    int reg_len;
    
    switch (type) {
        case DRV_EAV_QUEUE_Q4_BANDWIDTH:
            /* Q4 BW maxmum value = 16383(0x3fff) */
            if ((param <= EAV_5395_MAX_BANDWIDTH_VALUE) ||
                (param >= EAV_5395_MIN_BANDWIDTH_VALUE)) {
                soc_cm_debug(DK_ERR, 
                    "drv_bcm5395_eav_queue_control_set : BW unsupported. \n");
                return  SOC_E_PARAM;
            }
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_C4_BW_CNTLr, port, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_C4_BW_CNTLr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            temp = param;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_C4_BW_CNTLr, 
                &reg_value, C4_BWf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_QUEUE_Q5_BANDWIDTH:
            /* Q5 BW maxmum value = 16383(0x3fff) */
            if ((param <= EAV_5395_MAX_BANDWIDTH_VALUE) ||
                (param >= EAV_5395_MIN_BANDWIDTH_VALUE)) {
                soc_cm_debug(DK_ERR, 
                    "drv_bcm5395_eav_queue_control_set : BW unsupported. \n");
                return  SOC_E_PARAM;
            }
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_C5_BW_CNTLr, port, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_C5_BW_CNTLr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            temp = param;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_C5_BW_CNTLr, 
                &reg_value, C5_BWf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        case DRV_EAV_QUEUE_Q5_WINDOW:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_C5_BW_CNTLr, port, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_C5_BW_CNTLr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            if (param){
                temp = 1;
            } else {
                temp = 0;
            }
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, RESE_C5_BW_CNTLr, 
                &reg_value, C5_WNDWf, &temp));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value,reg_len));
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}
int 
drv_bcm5395_eav_queue_control_get(int unit, 
    uint32 port, uint32 type, uint32 *param)
{
    int rv = SOC_E_NONE;
    uint32 reg_addr, reg_value, temp, max_value = 0;
    int reg_len;
    soc_field_info_t    *finfop = 0;
    
    switch (type) {
        case DRV_EAV_QUEUE_Q4_BANDWIDTH:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_C4_BW_CNTLr, port, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_C4_BW_CNTLr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_C4_BW_CNTLr, 
                &reg_value, C4_BWf, &temp));
            *param = temp;
            break;
        case DRV_EAV_QUEUE_Q5_BANDWIDTH:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_C5_BW_CNTLr, port, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_C5_BW_CNTLr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_C5_BW_CNTLr, 
                &reg_value, C5_BWf, &temp));
            *param = temp;
            break;
        case DRV_EAV_QUEUE_Q5_WINDOW:
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, RESE_C5_BW_CNTLr, port, 0);
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, RESE_C5_BW_CNTLr);
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value,reg_len));
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, RESE_C5_BW_CNTLr, 
                &reg_value, C5_WNDWf, &temp));
            if (temp){
                *param = TRUE;
            } else {
                *param = FALSE;
            }
            break;
        case DRV_EAV_QUEUE_Q4_BANDWIDTH_MAX_VALUE:
            /*
              * Get the maximum valid bandwidth value for EAV Class 4 (macro slot time = 1)
              *
              * C4_Bandwidth(bytes/slot) = 
              *     Max_value(kbits/sec) * 1024 / (8 * macro slot time * 1000)
              *
              * C4_Bandwidth (14 bits) = 0x3fff
              * Max_value = (((1<<14) * 8 * macro slot time* 1000)/(1024)) - 1
              */
            SOC_FIND_FIELD(C4_BWf,
                SOC_REG_INFO(unit, RESE_C4_BW_CNTLr).fields,
                SOC_REG_INFO(unit, RESE_C4_BW_CNTLr).nFields,
                finfop);
            assert(finfop);

            temp = (1 << finfop->len);
            max_value = ((temp * 8 * 1 * 1000) / (1024)) - 1;
            *param = max_value;
            break;
        case DRV_EAV_QUEUE_Q5_BANDWIDTH_MAX_VALUE:
            /*
              * Get the maximum valid bandwidth value for EAV Class 5
              *
              * Class 5 slot time is 125 us.
              * C5_Bandwidth(bytes/125us) = Max_value(kbits/sec) * 1024 / (8 * 8000)
              *
              * C5_Bandwidth (14 bits) = 0x3fff
              * Max_value = (((1<<14) * 8 * 8000)/(1024)) - 1
              */
            SOC_FIND_FIELD(C5_BWf,
                SOC_REG_INFO(unit, RESE_C5_BW_CNTLr).fields,
                SOC_REG_INFO(unit, RESE_C5_BW_CNTLr).nFields,
                finfop);
            assert(finfop);

            temp = (1 << finfop->len);
            max_value = ((temp * 8 * 8000) / 1024) - 1;
            *param = max_value;
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}


int 
drv_bcm5395_eav_time_sync_mac_set(int unit, uint8* mac, uint16 ethertype)
{
    uint32 reg_addr, reg_val;
    uint64 reg_val64, mac_field;
    uint32 temp;
    int rv =  SOC_E_NONE, reg_len;

    /*
     * For time sync protocol, the mac should be set in Multi-address 0 register
     */

    /* 1. Set MAC and Ethertype value */
    SAL_MAC_ADDR_TO_UINT64(mac, mac_field);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MULTIPORT_ADDR0r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MULTIPORT_ADDR0r);

    COMPILER_64_ZERO(reg_val64);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                    (unit, MULTIPORT_ADDR0r, (uint32 *)&reg_val64, 
                        MPORT_ADDRf, (uint32 *)&mac_field));
    if (ethertype) {
        temp = ethertype;
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                        (unit, MULTIPORT_ADDR0r, (uint32 *)&reg_val64, 
                            MPORT_E_TYPEf, &temp));
    }
    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, (uint32 *)&reg_val64, reg_len)) < 0) {
        return rv;
    }

    /* 2. Set Forward map to CPU only */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MPORTVEC0r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MPORTVEC0r);
    temp  = SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0);
    reg_val = 0;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, MPORTVEC0r, &reg_val, PORT_VCTRf, &temp));
    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_val, reg_len)) < 0) {
        return rv;
    }

    /* 3. Enable Multi-address o */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MULTI_PORT_CTLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MULTI_PORT_CTLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_val, reg_len)) < 0) {
        return rv;
    }
    /* Set the match condition are MAC/Ethertype */
    if (ethertype) {
        temp = DRV_MULTIPORT_CTRL_MATCH_ETYPE_ADDR;
    } else {
        temp = DRV_MULTIPORT_CTRL_MATCH_ADDR;
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, MULTI_PORT_CTLr, &reg_val, MPORT_CTRL0f, &temp));
    /* Enable time stamped to CPU */
    temp = 1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, MULTI_PORT_CTLr, &reg_val, MPORT0_TS_ENf, &temp));    

    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_val, reg_len)) < 0) {
        return rv;
    }

    return rv;
    
}

int 
drv_bcm5395_eav_time_sync_mac_get(int unit, uint8* mac, uint16 *ethertype)
{
    uint32 reg_addr, reg_val;
    uint64 reg_val64, mac_field;
    uint32 temp;
    int rv =  SOC_E_NONE, reg_len;

    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MULTI_PORT_CTLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MULTI_PORT_CTLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_val, reg_len)) < 0) {
        return rv;
    }
    /* Get the value of time sync enable */
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, MULTI_PORT_CTLr, &reg_val, MPORT0_TS_ENf, &temp));
    if ( temp == 0) {
        rv = SOC_E_DISABLED;
        return rv;
    }
    /* Get the Multi-address control value */
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, MULTI_PORT_CTLr, &reg_val, MPORT_CTRL0f, &temp));
    if (temp == DRV_MULTIPORT_CTRL_DISABLE) {
        rv = SOC_E_DISABLED;
        return rv;
    }

    /* Get the MAC and Ethertype value */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MULTIPORT_ADDR0r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MULTIPORT_ADDR0r);

    COMPILER_64_ZERO(reg_val64);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, (uint32 *)&reg_val64, reg_len)) < 0) {
        return rv;
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                    (unit, MULTIPORT_ADDR0r, (uint32 *)&reg_val64, 
                        MPORT_ADDRf, (uint32 *)&mac_field));
    SAL_MAC_ADDR_FROM_UINT64(mac, mac_field);
   
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                    (unit, MULTIPORT_ADDR0r, (uint32 *)&reg_val64, 
                        MPORT_E_TYPEf, &temp));
    *ethertype = temp;

    return rv;
    
}


