/*
 * $Id: mirror.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
 *
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
 */
#include <soc/drv.h>
#include <soc/debug.h>

/*
 *  Function : drv_mirror_set
 *
 *  Purpose :
 *      Set ingress and egress ports of mirroring
 *
 *  Parameters :
 *      unit        :   unit id
 *      enable   :   enable/disable.
 *      monitor_port     :   monitor port id.
 *      ingress_bmp     :   ingress port bitmap.
 *      egress_bmp      :   egress port bitmap
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_mirror_set(int unit, uint32 enable, uint32 monitor_port, 
    soc_pbmp_t ingress_bmp, soc_pbmp_t egress_bmp)
{
    uint32	reg_addr, reg_value, reg_len, temp;
    int		rv= SOC_E_NONE;

    soc_cm_debug(DK_PORT, 
        "drv_mirror_set : unit %d, %sable, m_port = %d, \
        ingress_bmp = 0x%x, egress_bmp = 0x%x\n",
         unit, enable ? "en" : "dis", monitor_port, \
         SOC_PBMP_WORD_GET(ingress_bmp, 0), SOC_PBMP_WORD_GET(egress_bmp, 0));

        /* check ingress mirror */
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, IGMIRCTLr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, IGMIRCTLr, 0, 0);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
        /* write ingress mirror mask */
        temp = SOC_PBMP_WORD_GET(ingress_bmp, 0);
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_set)
            (unit, IGMIRCTLr, &reg_value, IN_MIR_MSKf, &temp));
        if ((rv = (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
        /* check egress mirror */
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, EGMIRCTLr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, EGMIRCTLr, 0, 0);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
        /* write egress mirror mask */
        temp = SOC_PBMP_WORD_GET(egress_bmp, 0);
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_set)
            (unit, EGMIRCTLr, &reg_value, OUT_MIR_MSKf, &temp));
        if ((rv = (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }

    if (enable) { /* enable mirror */
        /* check mirror control */
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MIRCAPCTLr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MIRCAPCTLr, 0, 0);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
        temp = 1;
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_set)
            (unit, MIRCAPCTLr, &reg_value, MIR_ENf, &temp));

        if (SOC_IS_ROBO5324(unit)) {
            temp = (0x1 << monitor_port);
        } else if (SOC_IS_ROBODINO(unit)||SOC_IS_ROBO5395(unit) ||
                   SOC_IS_ROBO5398(unit)||SOC_IS_ROBO5397(unit) ||
                   SOC_IS_ROBO53115(unit)||SOC_IS_ROBO53118(unit)) {
            temp = monitor_port;
        }
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_set)
            (unit, MIRCAPCTLr, &reg_value, SMIR_CAP_PORTf, &temp));

        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value,reg_len));


    } else { /* disable mirror */
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MIRCAPCTLr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MIRCAPCTLr, 0, 0);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
        temp = 0;
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_set)
            (unit, MIRCAPCTLr, &reg_value, MIR_ENf, &temp));
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value,reg_len));
    }
    return rv;
}

/*
 *  Function : drv_mirror_get
 *
 *  Purpose :
 *      Get ingress and egress ports of mirroring
 *
 *  Parameters :
 *      unit        :   unit id
 *      enable   :   enable/disable.
 *      monitor_port     :   monitor port id.
 *      ingress_bmp     :   ingress port bitmap.
 *      egress_bmp      :   egress port bitmap
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_mirror_get(int unit, uint32 *enable, uint32 *monitor_port, 
    soc_pbmp_t *ingress_bmp, soc_pbmp_t *egress_bmp)
{
    uint32	reg_addr, reg_value, reg_len, temp;
    int		rv= SOC_E_NONE, port;
    soc_pbmp_t	pbmp;

    /* ingress mask */
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, IGMIRCTLr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, IGMIRCTLr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    SOC_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->reg_field_get)
        (unit, IGMIRCTLr, &reg_value, IN_MIR_MSKf, &temp));
    SOC_PBMP_WORD_SET(*ingress_bmp, 0, temp);

    /* egress mask */
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, EGMIRCTLr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, EGMIRCTLr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    SOC_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->reg_field_get)
        (unit, EGMIRCTLr, &reg_value, OUT_MIR_MSKf, &temp));
    SOC_PBMP_WORD_SET(*egress_bmp, 0, temp);

    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MIRCAPCTLr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MIRCAPCTLr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    /* enable value */
    temp = 0;
    SOC_IF_ERROR_RETURN(
        (DRV_SERVICES(unit)->reg_field_get)
        (unit, MIRCAPCTLr, &reg_value, MIR_ENf, &temp));
    if (temp) {
        /* enable value */
        *enable = TRUE;
        /* monitor port */
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
            (unit, MIRCAPCTLr, &reg_value, SMIR_CAP_PORTf, &temp));
        if (SOC_IS_ROBO5324(unit)) {
            SOC_PBMP_WORD_SET(pbmp, 0, temp);
            SOC_PBMP_ITER(pbmp, port) { 
                *monitor_port = port;
                break;
            }
        } else if (SOC_IS_ROBODINO(unit)||SOC_IS_ROBO5395(unit)||
                   SOC_IS_ROBO5398(unit)||SOC_IS_ROBO5397(unit) ||
                   SOC_IS_ROBO53115(unit)||SOC_IS_ROBO53118(unit)) {
            *monitor_port = temp;
        }

    } else {
        SOC_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->reg_field_get)
            (unit, MIRCAPCTLr, &reg_value, SMIR_CAP_PORTf, &temp));
        *monitor_port = temp;
        *enable = FALSE;
    }
    soc_cm_debug(DK_PORT, 
        "drv_mirror_get : unit %d, %sable, m_port = %d, \
        ingress_bmp = 0x%x, egress_bmp = 0x%x\n",
         unit, *enable ? "en" : "dis", *monitor_port, 
         SOC_PBMP_WORD_GET(*ingress_bmp, 0), SOC_PBMP_WORD_GET(*egress_bmp, 0));
    return rv;
}
