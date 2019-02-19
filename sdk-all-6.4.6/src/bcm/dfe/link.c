/*
 * $Id: link.c,v 1.6 Broadcom SDK $
 *
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
 * DFE LINK
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_LINK
#include <shared/bsl.h>
#include <sal/types.h>

#include <bcm_int/common/debug.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm_int/common/link.h>

#include <bcm_int/dfe_dispatch.h>
#include <bcm_int/dfe/link.h>

#include <soc/drv.h>
#include <soc/linkctrl.h>

/*
 * Function:
 *     _bcm_dfe_link_port_link_get
 * Purpose:
 *     Return current PHY up/down status.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *     hw   - If TRUE, assume hardware linkscan is active and use it
 *              to reduce PHY reads.
 *            If FALSE, do not use information from hardware linkscan.
 *     up   - (OUT) TRUE for link up, FALSE for link down.
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_XXX
 */
STATIC int
_bcm_dfe_link_port_link_get(int unit, bcm_port_t port, int hw, int *up)
{
    int  rc;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dfe_port_link_status_get(unit, port, up);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *     _bcm_dfe_link_trunk_sw_ftrigger
 * Purpose:
 *     Remove specified ports with link down from trunks.
 * Parameters:
 *     unit        - Device unit number
 *     pbmp_active - Bitmap of ports
 *     pbmp_status - Bitmap of port status
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_dfe_link_trunk_sw_ftrigger(int unit, bcm_pbmp_t pbmp_active,
                                   bcm_pbmp_t pbmp_status)
{
    BCMDNX_INIT_FUNC_DEFS;
    /* No action for DFE device */
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

static _bcm_ls_driver_t  _bcm_ls_driver_dfe = {
    NULL,                           /* ld_hw_interrupt */
    _bcm_dfe_link_port_link_get,    /* ld_port_link_get */
    NULL,                           /* ld_internal_select */
    NULL,                           /* ld_update_asf */
    _bcm_dfe_link_trunk_sw_ftrigger /* ld_trunk_sw_failover_trigger */
};

/*
 * Function:
 *     bcm_linkscan_init
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
bcm_dfe_linkscan_init( int unit)
{
   int  rv;
   bcm_pbmp_t pbmp_hw, pbmp_sw;
   char pbmp_buf[SOC_PBMP_FMT_LEN];
   bcm_port_t port;
   BCMDNX_INIT_FUNC_DEFS;
                 
   rv = _bcm_linkscan_init(unit, &_bcm_ls_driver_dfe);
   BCMDNX_IF_ERR_EXIT(rv);

   /*Read Soc Properties*/
   BCM_PBMP_CLEAR(pbmp_sw);
   pbmp_sw = soc_property_suffix_num_pbmp_get(unit, -1, spn_BCM_LINKSCAN_PBMP, "sw", pbmp_sw);
   BCM_PBMP_CLEAR(pbmp_hw);
   pbmp_hw = soc_property_suffix_num_pbmp_get(unit, -1, spn_BCM_LINKSCAN_PBMP, "hw", pbmp_hw);

   /*HW linkscan is the default*/
   BCM_PBMP_REMOVE(pbmp_sw, pbmp_hw);

   /*Print values*/
   SOC_PBMP_FMT(pbmp_sw, pbmp_buf);
   LOG_VERBOSE(BSL_LS_BCM_LINK,
               (BSL_META_U(unit,
                           "linkscan pbmp sw: %s\n"), pbmp_buf));
   SOC_PBMP_FMT(pbmp_hw, pbmp_buf);
   LOG_VERBOSE(BSL_LS_BCM_LINK,
               (BSL_META_U(unit,
                           "linkscan pbmp hw: %s\n"), pbmp_buf));

   /*Set values*/
   BCM_PBMP_ITER(pbmp_sw, port)
   {
       rv = bcm_dfe_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_SW);
       BCMDNX_IF_ERR_EXIT(rv);
   }

   BCM_PBMP_ITER(pbmp_hw, port)
   {
       rv = bcm_dfe_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_HW);
       BCMDNX_IF_ERR_EXIT(rv);
   }

exit:
    BCMDNX_FUNC_RETURN; 

}
 


#undef _ERR_MSG_MODULE_NAME

