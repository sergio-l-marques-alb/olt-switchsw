/* 
 * $Id:$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:        port.c
 * Purpose:     Port driver.
 *
 */

#include <soc/defs.h>

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/td2_td2p.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm/error.h>
#include <bcm_int/esw/stat.h>

/*
 * Function:
 *  bcmi_td2p_stat_port_attach
 * Description:
 *  Initializes the BCM stat module for the new port that has been flexed
 * Parameters:
 *  unit - StrataSwitch PCI device unit number (driver internal).
 *  port - port number
 * Returns:
 *  BCM_E_NONE - Success.
 *  BCM_E_INTERNAL - Chip access failure.
 */

int
bcmi_td2p_stat_port_attach(int unit, bcm_port_t port)
{
    pbmp_t      pbmp;
    int         config_threshold;

    config_threshold = soc_property_get(unit, spn_BCM_STAT_JUMBO, 1518);
    if ((config_threshold < 1518) ||
        (config_threshold > 0x3fff) ) {
        config_threshold = 1518;
    }

    BCM_IF_ERROR_RETURN    
        (_bcm_esw_stat_ovr_threshold_set(unit, port, config_threshold));

    /* Adding the new port to bitmap and passing it to 
       counter_set32_by_port as it takes only bitmap as argument */
       SOC_PBMP_CLEAR(pbmp);
       SOC_PBMP_PORT_ADD(pbmp, port);
       SOC_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbmp, 0));
       
    /* Note: Assuming soc_counter_start would be called later after
             all the modules for the new port are initialised */
    

   return BCM_E_NONE;
}
/*
 * Function:
 *  bcmi_td2p_stat_port_detach
 * Description:
 *  Clear the port based statistics from the StrataSwitch port.
 * Parameters:
 *  unit - StrataSwitch PCI device unit number (driver internal).
 *  port - port number
 * Returns:
 *  BCM_E_NONE - Success.
 */

int
bcmi_td2p_stat_port_detach(int unit, bcm_port_t port)
{
    pbmp_t      pbm;
    
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    BCM_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbm, 0));
    
    return(BCM_E_NONE);
    
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
