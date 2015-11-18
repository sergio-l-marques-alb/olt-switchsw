

/*
 * $Id: dfe_port.c,v 1.13 Broadcom SDK $
 *
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
 * SOC DFE PORT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

#ifdef BCM_DFE_SUPPORT
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_port.h>

#ifdef BCM_88750_SUPPORT
#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_port.h>
#endif


/*
 * Function:
 *      soc_dfe_port_loopback_set
 * Purpose:
 *      Set port loopback
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number 
 *      loopback  - (IN)  soc_dfe_loopback_mode_t
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
soc_error_t 
soc_dfe_port_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback)
{
    int rc;
    soc_dcmn_loopback_mode_t lb_start; 
    uint32 cl72_start;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit) || !SOC_IS_DFE(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    DFE_LINK_INPUT_CHECK_SOCDNX(unit, port);

    if (loopback < soc_dcmn_loopback_mode_none || loopback > soc_dcmn_loopback_mode_phy_gloop) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid loopback")));
    }

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    SOCDNX_IF_ERR_EXIT(soc_dfe_port_loopback_get(unit, port, &lb_start));
    if (loopback != soc_dcmn_loopback_mode_none) {
        if (lb_start == soc_dcmn_loopback_mode_none) {
            SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_control_get,(unit, port, -1, -1, 0, SOC_PHY_CONTROL_CL72, &cl72_start)));
            rc = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, PORT_CL72_CONF, port, &cl72_start);
            SOCDNX_IF_ERR_EXIT(rc);
            if (cl72_start == 1) {
                SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_control_set,(unit, port, -1, -1, 0, SOC_PHY_CONTROL_CL72, 0)));
            }
        }
    }
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_loopback_set,(unit, port, loopback));
    SOCDNX_IF_ERR_EXIT(rc);

    if (loopback == soc_dcmn_loopback_mode_none) {
        if (lb_start != soc_dcmn_loopback_mode_none) {
            rc = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, PORT_CL72_CONF, port, &cl72_start);
            SOCDNX_IF_ERR_EXIT(rc);

            if (cl72_start == 1) {
                SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_phy_control_set,(unit, port, -1, -1, 0, SOC_PHY_CONTROL_CL72, 1)));
            }
        }
    }

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_burst_control_set,(unit, port, loopback));
    SOCDNX_IF_ERR_EXIT(rc);

    
exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_dfe_port_loopback_get
 * Purpose:
 *      Get port loopback
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number 
 *      loopback  - (OUT) soc_dfe_loopback_mode_t
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
soc_error_t 
soc_dfe_port_loopback_get(int unit, soc_port_t port, soc_dcmn_loopback_mode_t* loopback)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit) || !SOC_IS_DFE(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    DFE_LINK_INPUT_CHECK_SOCDNX(unit, port);
    SOCDNX_NULL_CHECK(loopback);

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_loopback_get,(unit, port, loopback));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;

}

#endif /* BCM_DFE_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

