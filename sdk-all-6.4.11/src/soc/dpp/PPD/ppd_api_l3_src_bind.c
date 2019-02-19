/* $Id: ppd_api_l3_src_bind.c,v 1.6 Broadcom SDK $
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
*/
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_l3_src_bind.c
*
* MODULE PREFIX:  ppd
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
#include <soc/dpp/PPD/ppd_api_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_l3_src_bind.h>


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
    Ppd_procedure_desc_element_src_bind[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV4_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV4_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV4_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV4_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV4_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV4_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV4_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV4_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV6_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV6_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV6_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV6_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV6_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV6_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV6_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_IPV6_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_SRC_BIND_GET_PROCS_PTR),
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
*    Add an L3 source binding ipv4 entry. 
 *    Given a key, e.g. (lif_ndx).
 *    Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
    uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SRC_BIND_IPV4_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv4_add,(unit, src_bind_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_src_bind_ipv4_add_print,(unit,src_bind_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_add()", 0, 0);
}

/*********************************************************************
*     Get an ipv4 entry according to its key. 
 *     Given a key, e.g. (lif_ndx)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SRC_BIND_IPV4_GET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
    SOC_SAND_CHECK_NULL_INPUT(found);
    
    SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv4_get,(unit, src_bind_info, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_src_bind_ipv4_get_print,(unit,src_bind_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_get()", 0, 0);
}

/*********************************************************************
*     Remove an ipv4 entry from the l3 source bind table according
 *     to the given Key. Given a key, e.g. (lif_ndx).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SRC_BIND_IPV4_REMOVE);
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
    SOC_SAND_CHECK_NULL_INPUT(success);
    
    SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv4_remove,(unit, src_bind_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_src_bind_ipv4_remove_print,(unit,src_bind_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_remove()", 0, 0);
}

/*********************************************************************
*     Clear all ipv4 entry inthe l3 source bind table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_table_clear(
    SOC_SAND_IN  int                           unit
  )
{
    uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SRC_BIND_IPV4_TABLE_CLEAR);
  
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv4_table_clear,(unit));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_src_bind_ipv4_table_clear_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_table_clear()", 0, 0);
}

/*********************************************************************
*     Gets the ipv4 source bind  host table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv4_host_get_block(
    SOC_SAND_IN    int                              unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE      *block_range,
    SOC_SAND_OUT   SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT   SOC_PPD_FRWRD_IP_ROUTE_STATUS   *routes_status,
    SOC_SAND_OUT   uint32                          *nof_entries
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(routes_status);
    SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv4_host_get_block,(unit, block_range, src_bind_info, routes_status, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_get_block_print,(unit, block_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv4_host_get_block()", 0, 0);
}

/*********************************************************************
*    Add an L3 source binding ipv6 entry. 
 *    Given a key, e.g. (lif_ndx).
 *    Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
    uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SRC_BIND_IPV6_ADD);
  
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv6_add,(unit, src_bind_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_src_bind_ipv6_add_print,(unit,src_bind_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_add()", 0, 0);
}

/*********************************************************************
*     Get an ipv6 entry according to its key. 
 *     Given a key, e.g. (lif_ndx)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  )
{
    uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SRC_BIND_IPV6_GET);
  
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv6_get,(unit, src_bind_info, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_src_bind_ipv6_get_print,(unit,src_bind_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_get()", 0, 0);
}

/*********************************************************************
*     Remove an ipv6 entry from the l3 source bind table according
 *     to the given Key. Given a key, e.g. (lif_ndx).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
    uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SRC_BIND_IPV6_REMOVE);
  
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv6_remove,(unit, src_bind_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_src_bind_ipv6_remove_print,(unit,src_bind_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_remove()", 0, 0);
}

/*********************************************************************
*     Clear all ipv6 entry inthe l3 source bind table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_table_clear(
    SOC_SAND_IN  int                           unit
  )
{
    uint32
      res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_SRC_BIND_IPV6_TABLE_CLEAR);
  
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv6_table_clear,(unit));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_src_bind_ipv6_table_clear_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_table_clear()", 0, 0);

}

/*********************************************************************
*     Gets the ipv6 source bind  host and subnet table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_src_bind_ipv6_get_block(
    SOC_SAND_IN    int                              unit,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE  *block_range,
    SOC_SAND_OUT   SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT   SOC_PPD_FRWRD_IP_ROUTE_STATUS   *routes_status,
    SOC_SAND_OUT   uint32                          *nof_entries
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
    SOC_SAND_CHECK_NULL_INPUT(block_range);
    SOC_SAND_CHECK_NULL_INPUT(routes_status);
    SOC_SAND_CHECK_NULL_INPUT(nof_entries);

    SOC_PPD_ARAD_ONLY_DEVICE_CALL(src_bind_ipv6_get_block,(unit, block_range, src_bind_info, routes_status, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_get_block_print,(unit, block_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_src_bind_ipv6_get_block()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_lif module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_src_bind_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_src_bind;
}

void
  SOC_PPD_SRC_BIND_IPV4_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);

  SOC_PPC_SRC_BIND_IPV4_ENTRY_clear(src_bind_info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_SRC_BIND_IPV6_ENTRY_clear(
    SOC_SAND_OUT SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);

  SOC_PPC_SRC_BIND_IPV6_ENTRY_clear(src_bind_info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PPD_DEBUG_IS_LVL3
void
  SOC_PPD_SRC_BIND_IPV4_INFO_print(
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY *src_bind_info
  )
{
    SOC_SAND_PP_IPV4_ADDRESS ipv4_addr;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(src_bind_info);

    soc_sand_SAND_PP_IPV4_ADDRESS_clear(&ipv4_addr);
    ipv4_addr.address[0] = src_bind_info->sip;

    LOG_CLI((BSL_META_U(unit,
                        "lif_ndx: %u"), src_bind_info->lif_ndx));
    if (src_bind_info->smac_valid) {
        LOG_CLI((BSL_META_U(unit,
                            " smac: ")));
        soc_sand_SAND_PP_MAC_ADDRESS_print(&(src_bind_info->smac));
    }
    LOG_CLI((BSL_META_U(unit,
                        " sip: ")));
    soc_sand_SAND_PP_IPV4_ADDRESS_print(&ipv4_addr);
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_SRC_BIND_IPV6_INFO_print(
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY *src_bind_info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(src_bind_info);

    LOG_CLI((BSL_META_U(unit,
                        "lif_ndx: %u"), src_bind_info->lif_ndx));
    if (src_bind_info->smac_valid) {
        LOG_CLI((BSL_META_U(unit,
                            " smac: ")));
        soc_sand_SAND_PP_MAC_ADDRESS_print(&(src_bind_info->smac));
    }
    LOG_CLI((BSL_META_U(unit,
                        " sip: ")));
    soc_sand_SAND_PP_IPV6_ADDRESS_print(&(src_bind_info->sip6));
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_ppd_src_bind_ipv4_add_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info
  )
{    
    LOG_CLI((BSL_META_U(unit,
                        "IPv4 src_bind_info:")));
    SOC_PPD_SRC_BIND_IPV4_INFO_print((src_bind_info));

    return;
}

void
  soc_ppd_src_bind_ipv4_get_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info
  )
{
    LOG_CLI((BSL_META_U(unit,
                        "IPv4 src_bind_info:")));
    SOC_PPD_SRC_BIND_IPV4_INFO_print((src_bind_info));

    return;
}

void
  soc_ppd_src_bind_ipv4_remove_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV4_ENTRY     *src_bind_info
  )
{
    LOG_CLI((BSL_META_U(unit,
                        "IPv4 src_bind_info:")));
    SOC_PPD_SRC_BIND_IPV4_INFO_print((src_bind_info));

    return;
}

void
  soc_ppd_src_bind_ipv4_table_clear_print(
    SOC_SAND_IN  int                           unit
  )
{
    return;
}

void
  soc_ppd_src_bind_ipv6_add_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info
  )
{
    LOG_CLI((BSL_META_U(unit,
                        "IPv6 src_bind_info:")));
    SOC_PPD_SRC_BIND_IPV6_INFO_print((src_bind_info));

    return;
}

void
  soc_ppd_src_bind_ipv6_get_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info
  )
{
    LOG_CLI((BSL_META_U(unit,
                        "IPv6 src_bind_info:")));
    SOC_PPD_SRC_BIND_IPV6_INFO_print((src_bind_info));

    return;
}

void
  soc_ppd_src_bind_ipv6_remove_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_SRC_BIND_IPV6_ENTRY     *src_bind_info
  )
{
    LOG_CLI((BSL_META_U(unit,
                        "IPv6 src_bind_info:")));
    SOC_PPD_SRC_BIND_IPV6_INFO_print((src_bind_info));

    return;
}

void
  soc_ppd_frwrd_get_block_print(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE   *block_range_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "block_range_key:")));
  soc_sand_SAND_TABLE_BLOCK_RANGE_print((block_range_key));

  return;
}

void
  soc_ppd_src_bind_ipv6_table_clear_print(
    SOC_SAND_IN  int                           unit
  )
{
    return;
}

#endif


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

