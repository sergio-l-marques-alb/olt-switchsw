/* 
 * $Id: petra_link.c,v 1.10 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * File:        soc_petra_link.c
 * Purpose:     Hardware Linkscan module
 *
 * There is no Hardware linkscan in Soc_petra.
 *
 * These routines will be called by the linkscan module,
 * so they need to be defined and return SOC_E_NONE.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT


#include <shared/bsl.h>

#include <soc/linkctrl.h>
#include <soc/dcmn/error.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/cmicm.h>


/*
 * Function:
 *     _soc_petra_linkctrl_linkscan_hw_init
 * Purpose:
 *     Initialize hardware linkscan.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     SOC_E_NONE
 */
STATIC int
_soc_petra_linkctrl_linkscan_hw_init(int unit)
{
   
    /* Nothing to do, no HW linkscan */
    return SOC_E_NONE;
}


/*
 * Function:
 *     _soc_petra_linkctrl_linkscan_config
 * Purpose:
 *     Set ports to hardware linkscan.
 * Parameters:
 *     unit          - Device number
 *     hw_mii_pbm    - Port bit map of ports to scan with MIIM registers
 *     hw_direct_pbm - Port bit map of ports to scan using NON MII
 * Returns:
 *     SOC_E_NONE
 */
STATIC int
_soc_petra_linkctrl_linkscan_config(int unit, pbmp_t hw_mii_pbm,
                                    pbmp_t hw_direct_pbm)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* No HW linkscan support */
     if (SOC_PBMP_NOT_NULL(hw_mii_pbm) || SOC_PBMP_NOT_NULL(hw_direct_pbm)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("error hw linkscan is not supported")));
     }
 
exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _soc_petra_linkctrl_linkscan_pause
 * Purpose:
 *     Pause link scanning, without disabling it.
 *     This call is used to pause scanning temporarily.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     SOC_E_NONE
 */
STATIC int
_soc_petra_linkctrl_linkscan_pause(int unit)
{
    return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_petra_linkctrl_linkscan_continue
 * Purpose:
 *     Continue link scanning after it has been paused.
 * Parameters:
 *     unit  - Device number
 * Returns:
 *     SOC_E_NONE
 */
STATIC int
_soc_petra_linkctrl_linkscan_continue(int unit)
{
    return SOC_E_NONE;
}


/*
 * Function:    
 *     _soc_petra_linkctrl_update
 * Purpose:
 *     Update the forwarding state in device.
 * Parameters:  
 *      unit - Device unit number
 * Returns:
 *     SOC_E_NONE
 */
STATIC int
_soc_petra_linkctrl_update(int unit)
{
    return SOC_E_NONE;
}

STATIC int
_soc_petra_linkctrl_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    return SOC_E_NONE;
}


/*
 * Link Control Driver - Soc_petra
 */
soc_linkctrl_driver_t  soc_linkctrl_driver_petra = {
    NULL,                                     /* port mapping */
    _soc_petra_linkctrl_linkscan_hw_init,     /* ld_linkscan_hw_init */
    _soc_petra_linkctrl_linkscan_config,      /* ld_linkscan_config */
    _soc_petra_linkctrl_linkscan_pause,       /* ld_linkscan_pause */
    _soc_petra_linkctrl_linkscan_continue,    /* ld_linkscan_continue */
    _soc_petra_linkctrl_update,                /* ld_update */
    _soc_petra_linkctrl_hw_link_get           /* ld_hw_link_get */

};

