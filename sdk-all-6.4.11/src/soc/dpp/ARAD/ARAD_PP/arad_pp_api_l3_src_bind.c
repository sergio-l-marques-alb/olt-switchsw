/* $Id: arad_pp_api_l3_src_bind.c,v 1.5 Broadcom SDK $
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

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
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

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*    Add an L3 source binding ipv4 entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv4_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV4_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_src_bind_ipv4_add_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_add_unsafe(
          unit,
          src_bind_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv4_add()", 0, 0);
}

/*********************************************************************
*     Get an ipv4 entry according to its key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv4_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV4_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_src_bind_ipv4_get_verify(
        unit,
        src_bind_info
      );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_get_unsafe(
          unit,
          src_bind_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv4_get()", 0, 0);
}

/*********************************************************************
*     Remove an ipv4 entry from the l3 source bind table
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv4_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV4_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV4_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_src_bind_ipv4_remove_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_remove_unsafe(
          unit,
          src_bind_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv4_remove()", 0, 0);
}

/*********************************************************************
*     Clear all ipv6 entry inthe l3 source bind table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv4_table_clear(
    SOC_SAND_IN  int                           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV4_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv4_table_clear()", 0, 0);
}

/*********************************************************************
*     Gets the ipv4 source bind host table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv4_host_get_block(
  SOC_SAND_IN    int                              unit,
  SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE      *block_range,
  SOC_SAND_OUT   ARAD_PP_SRC_BIND_IPV4_ENTRY     *src_bind_info,
  SOC_SAND_OUT   ARAD_PP_FRWRD_IP_ROUTE_STATUS   *routes_status,
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

  res = arad_pp_src_bind_ipv4_host_get_block_verify(
          unit,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv4_host_get_block_unsafe(
          unit,
          block_range,
          src_bind_info,
          routes_status,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv4_host_get_block()", 0, 0);
}

uint32
  arad_pp_src_bind_ipv4_host_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE       *block_range
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(block_range->entries_to_act, DPP_PP_LEM_NOF_ENTRIES(unit), ARAD_PP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(block_range->iter, DPP_PP_LEM_NOF_ENTRIES(unit), ARAD_PP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_get_block_verify()", 0, 0);
}

/*********************************************************************
*    Add an L3 source binding ipv6 entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv6_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV6_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_src_bind_ipv6_add_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_add_unsafe(
          unit,
          src_bind_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv6_add()", 0, 0);
}

/*********************************************************************
*     Get an ipv6 entry according to its key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv6_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT uint8                           *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV6_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_src_bind_ipv6_get_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_get_unsafe(
          unit,
          src_bind_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv4_get()", 0, 0);
}

/*********************************************************************
*     Remove an ipv6 entry from the l3 source bind table
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv6_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_SRC_BIND_IPV6_ENTRY     *src_bind_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV6_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_bind_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_src_bind_ipv6_remove_verify(
          unit,
          src_bind_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_remove_unsafe(
          unit,
          src_bind_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv6_remove()", 0, 0);
}

/*********************************************************************
*     Clear all ipv6 entry inthe l3 source bind table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv6_table_clear(
    SOC_SAND_IN  int                           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_SRC_BIND_IPV6_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /* No need to verify input parameter: unit */

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv6_table_clear()", 0, 0);
}

/*********************************************************************
*     Gets the ipv6 source bind  host and subnet table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_src_bind_ipv6_get_block(
  SOC_SAND_IN    int                              unit,
  SOC_SAND_INOUT ARAD_PP_IP_ROUTING_TABLE_RANGE  *block_range,
  SOC_SAND_OUT   ARAD_PP_SRC_BIND_IPV6_ENTRY     *src_bind_info,
  SOC_SAND_OUT   ARAD_PP_FRWRD_IP_ROUTE_STATUS   *routes_status,
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

  res = arad_pp_src_bind_ipv6_get_block_verify(
          unit,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_src_bind_ipv6_get_block_unsafe(
          unit,
          block_range,
          src_bind_info,
          routes_status,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv6_get_block()", 0, 0);
}

uint32
  arad_pp_src_bind_ipv6_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT ARAD_PP_IP_ROUTING_TABLE_RANGE   *block_range_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_IP_ROUTING_TABLE_RANGE, block_range_key, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_src_bind_ipv6_get_block_unsafe_verify()", 0, 0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

