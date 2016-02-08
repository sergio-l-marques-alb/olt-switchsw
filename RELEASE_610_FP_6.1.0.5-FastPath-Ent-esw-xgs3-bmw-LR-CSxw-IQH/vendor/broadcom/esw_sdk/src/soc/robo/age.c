/*
 * $Id: age.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
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
 *  Function : drv_age_timer_set
 *
 *  Purpose :
 *      Set the age timer status and value.
 *
 *  Parameters :
 *      unit        :   RoboSwitch unit number.
 *      enable   :   enable status.
 *      age_time  :   age timer value.
 *
 *  Return :
 *      SOC_E_XXX.
 *
 *  Note :
 *      
 *
 */
int 
drv_age_timer_set(int unit, uint32 enable, uint32 age_time)
{
    uint32  reg_addr, reg_value, temp;
    int       reg_len;

    soc_cm_debug(DK_VERBOSE, 
        "drv_age_timer_set: unit %d, %sable, age_time = %d\n",
        unit, enable ? "en" : "dis", age_time);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, SPTAGTr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, SPTAGTr);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len));
    if (enable) {
        temp = age_time;
    } else {
        temp = 0;
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, SPTAGTr, &reg_value, AGE_TIMEf, &temp));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));

    return SOC_E_NONE;
}

/*
 *  Function : drv_age_timer_get
 *
 *  Purpose :
 *      Get the age timer status and value.
 *
 *  Parameters :
 *      unit        :   RoboSwitch unit number.
 *      enable   :   enable status.
 *      age_time  :   age timer value.
 *
 *  Return :
 *      SOC_E_XXX.
 *
 *  Note :
 *      
 *
 */
int 
drv_age_timer_get(int unit, uint32 *enable, uint32 *age_time)
{
    uint32	reg_addr, reg_value, temp;
    int		reg_len;

    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, SPTAGTr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, SPTAGTr);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len));
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, SPTAGTr, &reg_value, AGE_TIMEf, &temp));
    if (temp) {
        *enable = TRUE;
        *age_time = temp;
    } else {
        *enable = FALSE;
        *age_time = 0;
    }

    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len));
    soc_cm_debug(DK_VERBOSE, 
        "drv_age_timer_get: unit %d, %sable, age_time = %d\n",
        unit, *enable ? "en" : "dis", *age_time);

    return SOC_E_NONE;
}
