/*
 * $Id: dfe_fabric.c,v 1.6 Broadcom SDK $
 *
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * SOC DFE FABRIC
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

#ifdef BCM_DFE_SUPPORT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <bcm/fabric.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <bcm/error.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/mbcm.h>

#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_fabric_flow_control.h>
#include <soc/dfe/fe1600/fe1600_fabric_links.h>
#include <soc/dfe/fe1600/fe1600_fabric_multicast.h>
#include <soc/dfe/fe1600/fe1600_fabric_status.h>
#include <soc/dfe/fe1600/fe1600_fabric_topology.h>
#include <bcm_int/control.h>

/*
 * Function:
 *      soc_dfe_fabric_link_status_all_get
 * Purpose:
 *      Get all links status
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      links_array_max_size - (IN)  max szie of link_status array
 *      link_status          - (OUT) array of link status per link
 *      errored_token_count  - (OUT) array error token count per link
 *      links_array_count    - (OUT) array actual size
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t
soc_dfe_fabric_link_status_all_get(int unit, int links_array_max_size, uint32* link_status, uint32* errored_token_count, int* links_array_count)
{
    int rc, nof_links = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    
    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);
    if(links_array_max_size < nof_links) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Array is too small")));
    }

    SOCDNX_NULL_CHECK(link_status);
    SOCDNX_NULL_CHECK(errored_token_count);
    SOCDNX_NULL_CHECK(links_array_count);

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_status_all_get,(unit, links_array_max_size, link_status, errored_token_count, links_array_count));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit); 
    SOCDNX_FUNC_RETURN;
}

#endif /* BCM_DFE_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

