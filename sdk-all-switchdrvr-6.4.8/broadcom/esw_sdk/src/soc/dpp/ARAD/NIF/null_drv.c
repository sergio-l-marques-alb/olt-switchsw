/*
 * $Id: null_drv.c,v 1.4 Broadcom SDK $
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
 * SOC NULL DRV
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/cm.h>
#include <soc/ll.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/common_drv.h>


/*
 * Function:
 *      soc_null_drv_init
 * Purpose:
 *      init NULL port
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Port number.
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
STATIC soc_error_t 
soc_null_drv_init(int unit, soc_port_t port)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      soc_null_drv_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - unit number.
 *      port - Port number.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_null_drv_enable_set(int unit, soc_port_t port, int enable)
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_FUNC_RETURN
}

/*
 * Function:
 *      soc_null_drv_speed_set
 * Purpose:
 *      Set NULL in the specified speed.
 * Parameters:
 *      unit - unit number.
 *      port - port number.
 *      speed - speed to set
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_null_drv_speed_set(int unit, soc_port_t port, int speed)
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_FUNC_RETURN
}

/*
 * Function:
 *      soc_null_drv_loopback_set
 * Purpose:
 *      Set a NULL into/out-of loopback mode
 * Parameters:
 *      unit - unit number.
 *      port - port number.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_null_drv_loopback_set(int unit, soc_port_t port, int lb)
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_FUNC_RETURN
}

/*
 * Function:
 *      soc_null_drv_loopback_get
 * Purpose:
 *      Get current NULL loopback mode setting.
 * Parameters:
 *      unit - unit number.
 *      port - port number.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_null_drv_loopback_get(int unit, soc_port_t port, int *lb)
{
    SOCDNX_INIT_FUNC_DEFS

    *lb = 0;

    SOCDNX_FUNC_RETURN
}

CONST mac_driver_t soc_null_driver = {
    "NULL Driver",                  /* drv_name */
    soc_null_drv_init,              /* md_init  */
    soc_null_drv_enable_set,        /* md_enable_set */
    NULL,                           /* md_enable_get */
    NULL,                           /* md_duplex_set */
    NULL,                           /* md_duplex_get */
    soc_null_drv_speed_set,         /* md_speed_set */
    NULL,                           /* md_speed_get */
    NULL,                           /* md_pause_set */
    NULL,                           /* md_pause_get */
    NULL,                           /* md_pause_addr_set */
    NULL,                           /* md_pause_addr_get */
    soc_null_drv_loopback_set,      /* md_lb_set */
    soc_null_drv_loopback_get,      /* md_lb_get */
    NULL,                           /* md_interface_set */
    NULL,                           /* md_interface_get */
    NULL,                           /* md_ability_get - Deprecated */
    NULL,                           /* md_frame_max_set */
    NULL,                           /* md_frame_max_get */
    NULL,                           /* md_ifg_set */
    NULL,                           /* md_ifg_get */
    NULL,                           /* md_encap_set */
    NULL,                           /* md_encap_get */
    NULL,                           /* md_control_set */
    NULL,                           /* md_control_get */
    NULL                            /* md_ability_local_get */
 };

#undef _ERR_MSG_MODULE_NAME


