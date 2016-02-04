/*
 * $Id: link.c,v 1.9 Broadcom SDK $
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
 * File:        link.c
 * Purpose:     BCM Linkscan module 
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_LINK

#include <shared/bsl.h>

#include <bcm_int/common/debug.h>
#include <sal/types.h>
#include <soc/linkctrl.h>
#include <soc/cmicm.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm_int/dpp/link.h>
#include <bcm_int/common/link.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <soc/dpp/port_sw_db.h>

static _bcm_ls_driver_t  _bcm_ls_driver_dpp = {
    NULL,                        /* ld_hw_interrupt */
    _bcm_petra_port_link_get,    /* ld_port_link_get */
    NULL,                        /* ld_internal_select */
    NULL,                        /* ld_update_asf */
    NULL                         /* ld_trunk_sw_failover_trigger */
};


/*
 * Function:
 *     bcm_petra_linkscan_init
 * Purpose:
 *     Initialize the linkscan software module.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If specific HW linkscan initialization is required by device,
 *     driver should call that.
 */


int
_bcm_arad_linkscan_init(int unit)
{
   int  rv;
   BCMDNX_INIT_FUNC_DEFS;
   
   rv = _bcm_linkscan_init(unit, &_bcm_ls_driver_dpp);
   BCMDNX_IF_ERR_EXIT(rv);
   
#ifdef BCM_CMICM_SUPPORT
   if(!SOC_WARM_BOOT(unit)){
       soc_cmicm_intr0_enable(unit, IRQ_CMCx_LINK_STAT_MOD);
   }
#endif /* BCM_CMICM_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN; 

}


int
bcm_petra_linkscan_init(int unit)
{
    int  rv;

    BCMDNX_INIT_FUNC_DEFS;
    if(SOC_IS_ARAD(unit)){
        BCMDNX_IF_ERR_EXIT(_bcm_arad_linkscan_init(unit));

    }
    else{
        rv = _bcm_linkscan_init(unit, &_bcm_ls_driver_dpp);
        LOG_VERBOSE(BSL_LS_BCM_LINK,
                    (BSL_META_U(unit,
                                "BCM linkscan init unit=%d rv=%d(%s)\n"),
                                unit, rv, bcm_errmsg(rv)));

        BCMDNX_IF_ERR_EXIT(rv);

    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_linkscan_mode_set(
    int unit, 
    bcm_port_t port, 
    int mode)
{
    uint32 flags;
    _bcm_dpp_gport_info_t   gport_info;
    bcm_port_t port_ndx;
    BCMDNX_INIT_FUNC_DEFS;
  
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_ndx) {
        if (!IS_SFI_PORT(unit, port)) {
            /*Statistic ports are not supported*/
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
            if(SOC_PORT_IS_STAT_INTERFACE(flags)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Stat port are not supported by linkscan"))); 
            }

            /*ILKN is not supported in HW mode*/
            if (IS_IL_PORT(unit, port) && mode == BCM_LINKSCAN_MODE_HW) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ILKN port are not supported by linkscan HW mode"))); 
            }
        }

        BCMDNX_IF_ERR_EXIT(bcm_common_linkscan_mode_set(unit, port, mode));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

