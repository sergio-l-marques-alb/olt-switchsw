/*
 * $Id: link.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
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
 * Hardware Linkscan module
 *
 * Hardware linkscan is available, but its use is not recommended
 * because a software linkscan task is very low overhead and much more
 * flexible.
 *
 * If hardware linkscan is used, each MII operation must temporarily
 * disable it and wait for the current scan to complete, increasing the
 * latency.  PHY status register 1 may contain clear-on-read bits that
 * will be cleared by hardware linkscan and not seen later.  Special
 * support is provided for the Serdes MAC.
 */

#include <sal/core/libc.h>
#include <sal/core/boot.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>

#include <soc/mcm/robo/driver.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/register.h>

/*
 * Function:    
 *      soc_robo_wan_port_link_get
 * Purpose:
 *      Get current linking state of WAN port.
 * Parameters:  
 *      unit - RoboSwitch unit #.
 *      *link - current link status of WAN port.
 * Returns:
 *      SOC_E_XXX
 * Note : 
 *      1. Get link status of WAN port.
 */
int
soc_robo_wan_port_link_get(int unit, int *link)
{
#ifdef WAN_PORT_SUPPORT
    uint32 reg_addr, reg_len, reg_value, link_sts;

    if (SOC_IS_ROBO5397(unit)) {
        reg_addr = (DRV_SERVICES(unit)->reg_addr)
                        (unit, E_MIISTSr, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
                        (unit, E_MIISTSr);
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                            (unit, reg_addr, &reg_value, reg_len));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, E_MIISTSr, &reg_value, LINK_STAf, &link_sts));
        *link = (link_sts) ? TRUE: FALSE;
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:    
 *      soc_robo_wan_port_link_sw_update
 * Purpose:
 *      Update the linking state of WAN port.
 * Parameters:  
 *      unit - RoboSwitch unit #.
 *      link - link status to be override.
 * Returns:
 *      SOC_E_XXX
 * Note : 
 *      1. Do software override to Mac.
 */
int
soc_robo_wan_port_link_sw_update(int unit, int link)
{
#ifdef WAN_PORT_SUPPORT
    uint32 reg_addr, reg_len, reg_value;

    if (SOC_IS_ROBO5397(unit)) {
        reg_addr = (DRV_SERVICES(unit)->reg_addr)
                        (unit, STS_OVERRIDE_WAN_Pr, 0, 0);
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
                        (unit, STS_OVERRIDE_WAN_Pr);
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                            (unit, reg_addr, &reg_value, reg_len));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, STS_OVERRIDE_WAN_Pr, &reg_value, LINK_STSf, &link));
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len));
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:    
 *      soc_robo_link_sw_update
 * Purpose:
 *      Update the linking state in the chip on the assigned port
 * Parameters:  
 *      unit - RoboSwitch unit #.
 *      port - Port to process.
 * Returns:
 *      SOC_E_XXX
 * Note : 
 *      1. return SOC_E_PARAM is the port is CPU port.
 */

int
soc_robo_link_sw_update(int unit, int link, int port)
{
    uint32 	reg_index, overrid_findex, field_index, reg_addr, reg_len, temp;
    uint32  reg_value;
    
    uint32  physcan_findex;
    
    if (IS_FE_PORT(unit, port)){
        reg_index = STS_OVERRIDE_Pr;
    } else if (IS_GE_PORT(unit, port)){
        if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)){
            reg_index = STS_OVERRIDE_GMIIPr;
        } else {
            reg_index = STS_OVERRIDE_GPr;
        }
    } else {
        return SOC_E_PARAM;
    }

    OVERRIDE_LOCK(unit);
    /* Read Port Override Register */ 
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                    (unit, reg_index, port, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                    (unit, reg_index);
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_read)
                        (unit, reg_addr, &reg_value, reg_len));

    /* all robo chips with the same filed name */
    field_index = LINK_STSf;
    if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)) {
        overrid_findex = SW_OVERRIDE_Rf;
    } else {
        overrid_findex = SW_ORDf;
    }

    if (IS_FE_PORT(unit, port)){
        /* bcm5324_a1 defined filed name on appending "_1" */
        if (SOC_IS_ROBO5324_A1(unit)){
            overrid_findex = SW_ORDf;
        }
        
        /* bcm5324 FE SW override register definition at link bit is inverted 
         *  - 1 is link-down ; and 0 is link-up
         */
        if (SOC_IS_ROBO5324(unit)){
            link = !link;
        }
        
        /* phy auto-scan field :
         *  - bcm5348 have per FE port phy scan control register.
         */
        if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)){
            physcan_findex = EN_PHY_SCANf;
            
            /* disable phy scan */
            temp = 0;
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, reg_index, &reg_value, physcan_findex, &temp);
        }
    } else {        /* is GE port */
        /* phy auto-scan field :
         *  1. bcm5324 - only GE port had such field.
         *  2. bcm5396/5398 - no such filed in SW override reg.
         */
        if (soc_feature(unit, soc_feature_robo_ge_serdes_mac_autosync)){
            /* For robo chips 5348/5347/53262 , do not use override register 
            *  to overrid mac. Or we'll get runt packet after plug/unplug ge link.
            */
            physcan_findex = EN_PHY_SCANf;
            temp = 0;
            (DRV_SERVICES(unit)->reg_field_set)
                    (unit, reg_index, &reg_value, overrid_findex, &temp);
            temp = 0;
            (DRV_SERVICES(unit)->reg_field_set)
                    (unit, reg_index, &reg_value, physcan_findex, &temp);
            /* Write Port Override Register */
            (DRV_SERVICES(unit)->reg_write)
                    (unit, reg_addr, &reg_value, reg_len);
            OVERRIDE_UNLOCK(unit);
            return SOC_E_NONE;
        }         
        if (SOC_IS_ROBO5324(unit) || SOC_IS_ROBO5389(unit) || 
                    SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit) || 
                    SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit)){
            if (SOC_IS_ROBO5324(unit)){
                physcan_findex = EN_STS_OVERRIDEf;
            } else if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit) ||
                SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)){
                physcan_findex = EN_PHY_SCANf;
            } else {        /* bcm5389 only */
                physcan_findex = PHY_SCAN_ENf;
            }

            /* disable phy scan */
            temp = 0;
            (DRV_SERVICES(unit)->reg_field_set)
                (unit, reg_index, &reg_value, physcan_findex, &temp);
        }

    }
                        
    temp = 1;
    (DRV_SERVICES(unit)->reg_field_set)
        (unit, reg_index, &reg_value, overrid_findex, &temp);
    
    temp = link ? 1 : 0;
    (DRV_SERVICES(unit)->reg_field_set)
        (unit, reg_index, &reg_value, field_index, &temp);
    
    /* Write Port Override Register */
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_write)
                        (unit, reg_addr, &reg_value, reg_len));

    SOC_DEBUG_PRINT((DK_LINK | DK_VERBOSE,
		     "soc_robo_link_sw_update: link=%d port=%d\n", link, port));

    OVERRIDE_UNLOCK(unit);
    return SOC_E_NONE;
    
}

