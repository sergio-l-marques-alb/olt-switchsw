/* $Id: arad_pp_extender.c,v 1.00 Broadcom SDK $
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
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_L3


/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_extender.h>
#include <soc/dpp/PPC/ppc_api_extender.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define EPNI_CFG_KEEP_ING_ECID_REG_SIZE 8


/* } */
/*************
 * MACROS    *
 *************/
/* { */


/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


/*********************************************************************
 * NAME:
 *   arad_pp_extender_init
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Initializes extender port configuration on this unit.
 * INPUT:
 *  int                         unit            - (IN) Identifier of the device to access.
 * REMARKS:
 *     
 * RETURNS:
 *   SOC_E_***              If there was a problem.
 *   SOC_E_NONE             Otherwise.
*********************************************************************/
soc_error_t 
arad_pp_extender_init(int unit){
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_reg_field32_modify(unit, EPNI_CFG_ENABLE_ETAGr, REG_PORT_ANY, CFG_ENABLE_ETAGf, 1);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   arad_pp_extender_deinit
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Deinitializes extender port configuration on this unit.
 * INPUT:
 *  int                         unit            - (IN) Identifier of the device to access.
 * REMARKS:
 *     
 * RETURNS:
 *   SOC_E_***              If there was a problem.
 *   SOC_E_NONE             Otherwise.
*********************************************************************/
soc_error_t 
arad_pp_extender_deinit(int unit){
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_reg_field32_modify(unit, EPNI_CFG_ENABLE_ETAGr, REG_PORT_ANY, CFG_ENABLE_ETAGf, 0);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   arad_pp_extender_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Given a port, sets how to resolve Ing-ECID-Base. Either Set according to 
 *  incoming ECID or do not touch.
 * INPUT:
 *  int                         unit            - (IN) Identifier of the device to access.
 *  int                         port            - (IN) Required port
 *  SOC_PPC_EXTENDER_PORT_INFO  port_info
 *    ->SOC_PPC_EXTENDER_PORT_ING_ECID_MODE ing_ecid_mode - (IN) _NOP for don't touch, _KEEP for use ECID.
 * REMARKS:
 *     
 * RETURNS:
 *   SOC_E_***              If there was a problem reading or wriring the register
 *   SOC_E_NONE             Otherwise.
*********************************************************************/
soc_error_t 
arad_pp_extender_port_info_set(int unit, ARAD_PP_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info){
    int rv;
    soc_reg_above_64_val_t data;
    uint32 tbl_ndx, tbl_offset, port_mask;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(port_info);

    tbl_ndx = port / SOC_SAND_NOF_BITS_IN_UINT32;

    tbl_offset = port % SOC_SAND_NOF_BITS_IN_UINT32;

    port_mask = 1 << tbl_offset;

    /* Read the previous value */
    rv = READ_EPNI_CFG_KEEP_ING_ECIDr(unit, REG_PORT_ANY, data);
    SOCDNX_IF_ERR_EXIT(rv);

    /* Write the new value - clear or set the port from the bitmap. */
    switch (port_info->ing_ecid_mode) {
    case SOC_PPC_EXTENDER_PORT_ING_ECID_NOP:
        data[tbl_ndx] &= ~port_mask;
        break;
    case SOC_PPC_EXTENDER_PORT_ING_ECID_KEEP:
        data[tbl_ndx] |= port_mask;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Wrong ing ecid mode.")));
    }

    rv = WRITE_EPNI_CFG_KEEP_ING_ECIDr(unit, REG_PORT_ANY, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
 * NAME:
 *   arad_pp_extender_port_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the port configuration for Ing-ECID-Base treatment. Either Set according to 
 *  incoming ECID or do not touch.
 * INPUT:
 *  int                         unit            - (IN) Identifier of the device to access.
 *  int                         port            - (IN) Required port
 *  SOC_PPC_EXTENDER_PORT_INFO  port_info
 *    ->SOC_PPC_EXTENDER_PORT_ING_ECID_MODE ing_ecid_mode - (OUT) _NOP for don't touch, _KEEP for use ECID.
 * REMARKS:
 *     
 * RETURNS:
 *   SOC_E_***              If there was a problem reading the register
 *   SOC_E_NONE             Otherwise.
*********************************************************************/
soc_error_t 
arad_pp_extender_port_info_get(int unit, ARAD_PP_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info){
    int rv;
    soc_reg_above_64_val_t data;
    uint32 tbl_ndx, tbl_offset, port_mask;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(port_info);

    sal_memset(port_info, 0, sizeof(SOC_PPC_EXTENDER_PORT_INFO));

    tbl_ndx = port / SOC_SAND_NOF_BITS_IN_UINT32;

    tbl_offset = port % SOC_SAND_NOF_BITS_IN_UINT32;

    port_mask = 1 << tbl_offset;

    /* Read the value */
    rv = READ_EPNI_CFG_KEEP_ING_ECIDr(unit, REG_PORT_ANY, data);
    SOCDNX_IF_ERR_EXIT(rv);

    port_info->ing_ecid_mode = (data[tbl_ndx] & port_mask) ? SOC_PPC_EXTENDER_PORT_ING_ECID_KEEP : SOC_PPC_EXTENDER_PORT_ING_ECID_NOP;

exit:
    SOCDNX_FUNC_RETURN;
}

/* } */

