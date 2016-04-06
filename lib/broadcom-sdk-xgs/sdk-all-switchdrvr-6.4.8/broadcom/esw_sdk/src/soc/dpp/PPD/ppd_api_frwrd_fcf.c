/* $Id: ppd_api_frwrd_fcf.c,v 1.5 Broadcom SDK $
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
*/
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_frwrd_fcf.c
*
* MODULE PREFIX:  soc_ppd_frwrd
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fcf.h>
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_fcf.h>
#endif



/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 *  MACROS   *
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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_frwrd_fcf[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ROUTING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_ZONING_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_FCF_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Setting global information of the FCF routing (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_glbl_info_set,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_glbl_info_set_print,(unit,glbl_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Setting global information of the FCF routing (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_glbl_info_get,(unit, glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_glbl_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Add fcf route entry to the routing table. Binds between
 *     fcf Unicast route key (fcf-address/prefix) and a FEC
 *     entry identified by route_info for a given router. As a
 *     result of this operation, Unicast fcf packets
 *     designated to the FCF address matching the given key (as
 *     long there is no more-specific route key) will be routed
 *     according to the information in the FEC entry identified
 *     by route_info.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_route_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_INFO*            route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_route_add,(unit, route_key, route_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_route_add_print,(unit,route_key,route_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_route_add()", 0, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_route_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                               exact_match,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_STATUS      *route_status,
    SOC_SAND_OUT uint8                               *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_route_get,(unit, route_key, exact_match, route_info, route_status, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_route_get_print,(unit,route_key,exact_match));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_route_get()", 0, 0);
}

/*********************************************************************
*     Gets the fcf UC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_route_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE     *block_range,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_KEY          *route_keys,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_INFO         *routes_info,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_STATUS       *routes_status,
    SOC_SAND_OUT uint32                                *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_route_get_block,(unit, block_range, route_keys, routes_info, routes_status, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_route_get_block_print,(unit,block_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_route_get_block()", 0, 0);
}

/*********************************************************************
*     Remove entry from the routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_route_remove,(unit, route_key, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_route_remove_print,(unit,route_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_route_remove()", 0, 0);
}

/*********************************************************************
*     Clear the fcf UC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_routing_table_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags

  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ROUTING_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_routing_table_clear,(unit,vfi_ndx,flags));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_routing_table_clear_print,(unit,vfi_ndx,flags));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_routing_table_clear()", 0, 0);
}

/*********************************************************************
*     Add fcf entry to the zoning table. Binds between Host and
 *     next hop information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_zoning_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ZONING_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(zoning_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*SOC_PPD_DEVICE_CALL(frwrd_fcf_zoning_add,(unit, zoning_key, routing_info, success));*/
  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_zoning_add,(unit, zoning_key, routing_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_zoning_add_print,(unit,zoning_key,routing_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_zoning_add()", 0, 0);
}

/*********************************************************************
*     Gets the routing information associated with the given
 *     route key on VFI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_zoning_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT uint8                           *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ZONING_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(zoning_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(found);


  /* for arad call with flags */
  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_zoning_get,(unit, zoning_key, routing_info, route_status, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_zoning_get_print,(unit,zoning_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_zoning_get()", 0, 0);
}

/*********************************************************************
*     Gets the zoning table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_zoning_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ZONING_KEY             *zoning_keys,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ZONING_INFO      *zoning_info,
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                            *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ZONING_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(zoning_keys);
  SOC_SAND_CHECK_NULL_INPUT(zoning_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);


  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_zoning_get_block,(unit, block_range_key, flags, zoning_keys, zoning_info, routes_status, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_zoning_get_block_print,(unit,block_range_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_zoning_get_block()", 0, 0);
}

/*********************************************************************
*     Remove fcf route entry from the routing table of a
 *     virtual fabric identifier (VFI).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_zoning_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY             *zoning_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ZONING_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(zoning_key);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_zoning_remove,(unit, zoning_key));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_zoning_remove_print,(unit,zoning_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_zoning_remove()", 0, 0);
}



/*********************************************************************
*     Clear fcf routing table of VFI
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_fcf_zoning_table_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               flags,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY         *key,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_FCF_ZONING_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_fcf_zoning_table_clear,(unit, flags,key,info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_fcf_zoning_table_clear_print,(unit,flags,key,info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fcf_zoning_table_clear()", flags, 0);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_frwrd_fcf module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_fcf_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_frwrd_fcf;
}

void
  SOC_PPD_FRWRD_FCF_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FCF_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_ROUTE_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FCF_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_ROUTE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPD_FRWRD_FCF_ZONING_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ZONING_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_ZONING_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FCF_ZONING_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_FCF_ZONING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_ZONING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PPD_DEBUG_IS_LVL1


void
  SOC_PPD_FRWRD_FCF_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FCF_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_ROUTE_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPD_FRWRD_FCF_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_ROUTE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FCF_ZONING_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_ZONING_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_FCF_ZONING_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_FCF_ZONING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3


void
  soc_ppd_frwrd_fcf_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_GLBL_INFO                *glbl_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "glbl_info:")));
  SOC_PPD_FRWRD_FCF_GLBL_INFO_print((glbl_info));

  return;
}
void
  soc_ppd_frwrd_fcf_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_fcf_route_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_INFO*            route_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "route_key:")));
  SOC_PPD_FRWRD_FCF_ROUTE_KEY_print((route_key));
  LOG_CLI((BSL_META_U(unit,
                      "route_info:")));
  SOC_PPD_FRWRD_FCF_ROUTE_INFO_print(route_info);

  return;
}
void
  soc_ppd_frwrd_fcf_route_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                               exact_match
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "route_key:")));
  SOC_PPD_FRWRD_FCF_ROUTE_KEY_print((route_key));

  LOG_CLI((BSL_META_U(unit,
                      "exact_match: %u\n\r"),exact_match));

  return;
}
void
  soc_ppd_frwrd_fcf_route_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE              *block_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "block_range:")));
  SOC_PPD_IP_ROUTING_TABLE_RANGE_print((block_range));

  return;
}
void
  soc_ppd_frwrd_fcf_route_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ROUTE_KEY             *route_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "route_key:")));
  SOC_PPD_FRWRD_FCF_ROUTE_KEY_print((route_key));

  return;
}
void
  soc_ppd_frwrd_fcf_routing_table_clear_print(
    SOC_SAND_IN  int                               unit
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
  )
{

  return;
}
void
  soc_ppd_frwrd_fcf_zoning_add_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_INFO      *routing_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "zoning_key:")));
  SOC_PPD_FRWRD_FCF_ZONING_KEY_print((zoning_key));

  LOG_CLI((BSL_META_U(unit,
                      "routing_info:")));
  SOC_PPD_FRWRD_FCF_ZONING_INFO_print((routing_info));

  return;
}
void
  soc_ppd_frwrd_fcf_zoning_get_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY             *zoning_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "zoning_key:")));
  SOC_PPD_FRWRD_FCF_ZONING_KEY_print((zoning_key));

  return;
}
void
  soc_ppd_frwrd_fcf_zoning_get_block_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "block_range_key:")));
  soc_sand_SAND_TABLE_BLOCK_RANGE_print((block_range_key));

  return;
}
void
  soc_ppd_frwrd_fcf_zoning_remove_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY             *zoning_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "zoning_key:")));
  SOC_PPD_FRWRD_FCF_ZONING_KEY_print((zoning_key));

  return;
}

void
  soc_ppd_frwrd_fcf_zoning_table_clear_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               flags,
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_ZONING_KEY         *key,
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ZONING_INFO        *info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vfi_ndx: %lu\n\r"),vfi_ndx));
  LOG_CLI((BSL_META_U(unit,
                      "zoning_key:")));
  SOC_PPD_FRWRD_FCF_ZONING_KEY_print((key));
  LOG_CLI((BSL_META_U(unit,
                      "zoning_info:")));
  SOC_PPD_FRWRD_FCF_ZONING_INFO_print((info));

  return;
}


#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

