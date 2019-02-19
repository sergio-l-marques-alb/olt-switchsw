
/* $Id: arad_pp_api_frwrd_ipv4.c,v 1.13 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD

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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>

#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>

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
*     Set for which IP routes (IPv4/6 UC/MC) to enable caching
 *     by SW
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ip_routes_cache_mode_enable_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ip_routes_cache_mode_enable_set_verify(
          unit,
          vrf_ndx,
          route_types
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ip_routes_cache_mode_enable_set_unsafe(
          unit,
          vrf_ndx,
          route_types
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ip_routes_cache_mode_enable_set()", vrf_ndx, 0);
}

/*********************************************************************
*     Set for which IP routes (IPv4/6 UC/MC) to enable caching
 *     by SW
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ip_routes_cache_mode_enable_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT uint32                                  *route_types
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_types);

  res = arad_pp_frwrd_ip_routes_cache_mode_enable_get_verify(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ip_routes_cache_mode_enable_get_unsafe(
          unit,
          vrf_ndx,
          route_types
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ip_routes_cache_mode_enable_get()", vrf_ndx, 0);
}

/*********************************************************************
*     Synchronize the routing table in the Data plane (HW)
 *     with the routing table in the control plane (SW)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ip_routes_cache_commit(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IP_ROUTES_CACHE_COMMIT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ip_routes_cache_commit_verify(
          unit,
          route_types,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ip_routes_cache_commit_unsafe(
          unit,
          route_types,
          vrf_ndx,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ip_routes_cache_commit()", vrf_ndx, 0);
}

/*********************************************************************
*     Setting global information of the IP routing (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_frwrd_ipv4_glbl_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_glbl_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Setting global information of the IP routing (including
 *     resources to use)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_GLBL_INFO                *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = arad_pp_frwrd_ipv4_glbl_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_glbl_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Add IPv4 route entry to the routing table. Binds between
 *     Ipv4 Unicast route key (IPv4-address/prefix) and a FEC
 *     entry identified by fec_id for a given router. As a
 *     result of this operation, Unicast Ipv4 packets
 *     designated to the IP address matching the given key (as
 *     long there is no more-specific route key) will be routed
 *     according to the information in the FEC entry identified
 *     by fec_id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_uc_route_add_unsafe(
          unit,
          route_key,
          fec_id,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_route_add()", 0, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv4_uc_route_get_verify(
          unit,
          route_key,
          exact_match
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_uc_route_get_unsafe(
          unit,
          route_key,
          exact_match,
          fec_id,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_route_get()", 0, 0);
}

/*********************************************************************
*     Gets the Ipv4 UC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(fec_ids);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(routes_location);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_ipv4_uc_route_get_block_verify(
          unit,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_uc_route_get_block_unsafe(
          unit,
          block_range,
          route_keys,
          fec_ids,
          routes_status,
          routes_location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_route_get_block()", 0, 0);
}

/*********************************************************************
*     Free table iterator.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN SOC_DPP_DBAL_SW_TABLE_IDS         table_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_dbal_iterator_deinit(
     unit,
     table_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_route_iterator_init()", 0, 0);
}

/*********************************************************************
*     Check whether the table is initiated.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS        table_id,
    SOC_SAND_OUT int                             *is_table_initiated
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_dbal_table_is_initiated(unit, table_id, is_table_initiated);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated()", 0, 0);
}


/*********************************************************************
*     Initialize table iterator.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_iterator_init(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS         table_id,
    SOC_SAND_OUT SOC_DPP_DBAL_PHYSICAL_DB_TYPES    *physical_db_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_dbal_iterator_init(
     unit,
     table_id,
     physical_db_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_route_iterator_init()", 0, 0);
}

/*********************************************************************
*     Get the next entry from the table iterator.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_DPP_DBAL_SW_TABLE_IDS        table_id,
    SOC_SAND_IN  uint32                           flags,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL              qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_OUT void                            *payload,
    SOC_SAND_OUT uint32                          *priority,
    SOC_SAND_OUT uint8                           *hit_bit,
    SOC_SAND_OUT uint8                           *found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 soc_flags = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(qual_vals);
  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  if ((table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM) || (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM)) {
      if(flags & SOC_PPC_FRWRD_IP_HOST_GET_ACCESSED_ONLY){
          soc_flags |= ARAD_PP_FRWRD_LEM_GET_BLOCK_ACCESSED_ONLY;
      }

      if(flags & SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET){
          soc_flags |= ARAD_PP_FRWRD_LEM_GET_BLOCK_CLEAR_ACCESSED;
      }
  }

  if ((table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC) || (table_id == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP)) {
	  if(flags & SOC_PPC_FRWRD_IP_HOST_KBP_HOST_INDICATION){
          soc_flags |= ARAD_PP_FRWRD_LEM_GET_BLOCK_KBP_HOST_INDICATION;
      }
  }
  
  res = arad_pp_dbal_iterator_get_next(
     unit,
     table_id,
     soc_flags,
     qual_vals,
     payload,
     priority,
     hit_bit,
     found);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_route_iterator_get_next()", 0, 0);
}


/*********************************************************************
*     Remove entry from the routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv4_uc_route_remove_verify(
          unit,
          route_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_uc_route_remove_unsafe(
          unit,
          route_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_route_remove()", 0, 0);
}

/*********************************************************************
*     Clear the IPv4 UC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_routing_table_clear(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipv4_uc_routing_table_clear_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_uc_routing_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_uc_routing_table_clear()", 0, 0);
}

/*********************************************************************
*     Add IPv4 entry to the Host table. Binds between Host and
 *     next hop information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_host_add(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv4_host_add_verify(
          unit,
          host_key,
          routing_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_host_add_unsafe(
          unit,
          host_key,
          routing_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_host_add()", 0, 0);
}

/*********************************************************************
*     Gets the routing information associated with the given
 *     route key on VRF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_host_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(host_key);
  SOC_SAND_CHECK_NULL_INPUT(routing_info);
  SOC_SAND_CHECK_NULL_INPUT(route_status);
  SOC_SAND_CHECK_NULL_INPUT(location);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv4_host_get_verify(
          unit,
          host_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_host_get_unsafe(
          unit,
          host_key,
          flags,
          routing_info,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_host_get()", 0, 0);
}

/*********************************************************************
*     Gets the host table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_host_get_block(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key,
    SOC_SAND_IN  uint32                                      flags,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                              *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(host_keys);
  SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_ipv4_host_get_block_verify(
          unit,
          block_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_host_get_block_unsafe(
          unit,
          block_range_key,
          flags,
          host_keys,
          routes_info,
          routes_status,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_host_get_block()", 0, 0);
}

/*********************************************************************
*     Remove IPv4 route entry from the routing table of a
 *     virtual router (VRF).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_host_remove(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_HOST_KEY             *host_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_HOST_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(host_key);

  res = arad_pp_frwrd_ipv4_host_remove_verify(
          unit,
          host_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_host_remove_unsafe(
          unit,
          host_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_host_remove()", 0, 0);
}

 
/*********************************************************************
*     Add IPv4 MC route entry to the routing table. Binds
 *     between Ipv4 Unicast route key (IPv4-address/prefix) and
 *     a FEC entry identified by fec_id for a given router. As
 *     a result of this operation, Unicast Ipv4 packets
 *     designated to IP address matching the given key (as long
 *     there is no more-specific route key) will be routed
 *     according to the information in the FEC entry identified
 *     by fec_id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_route_add(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv4_mc_route_add_verify(
          unit,
          route_key,
          route_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_mc_route_add_unsafe(
          unit,
          route_key,
          route_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_mc_route_add()", 0, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_route_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                             exact_match,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(route_info);
  ;
  
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv4_mc_route_get_verify(
          unit,
          route_key,
          exact_match
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_mc_route_get_unsafe(
          unit,
          route_key,
          exact_match,
          route_info,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_mc_route_get()", 0, 0);
}

/*********************************************************************
*     Gets the Ipv4 MC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(routes_info);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(routes_location);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_ipv4_mc_route_get_block_verify(
          unit,
          block_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_mc_route_get_block_unsafe(
          unit,
          block_range_key,
          route_keys,
          routes_info,
          routes_status,
          routes_location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_mc_route_get_block()", 0, 0);
}

/*********************************************************************
*     PE uint32 soc_ppd_frwrd_ipv4_mc_route_remove( SOC_SAND_IN
 *     uint32 unit, SOC_SAND_IN SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY
 *     *route_key, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success);
 *     Remove IPv4 route entry from the routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_route_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv4_mc_route_remove_verify(
          unit,
          route_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_mc_route_remove_unsafe(
          unit,
          route_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_mc_route_remove()", 0, 0);
}

/*********************************************************************
*     Clear the IPv4 MC routing table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_routing_table_clear(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipv4_mc_routing_table_clear_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_mc_routing_table_clear_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_mc_routing_table_clear()", 0, 0);
}


uint32
  soc_ppd_frwrd_ipmc_rp_add(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipmc_rp_add_verify(
          unit,
          rp_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipmc_rp_add_unsafe(
          unit,
          rp_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipmc_rp_add()", 0, 0);
}

uint32
  soc_ppd_frwrd_ipmc_rp_get_block(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  rp_id,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range_key,
    SOC_SAND_OUT int32                                  *l3_intfs,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipmc_rp_get_block_verify(
          unit,
          rp_id,
          block_range_key,
          l3_intfs,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipmc_rp_get_block_unsafe(
          unit,
          rp_id,
          block_range_key,
          l3_intfs,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipmc_rp_get_block()", 0, 0);
}

uint32
  soc_ppd_frwrd_ipmc_rp_remove(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_IP_MC_RP_INFO      *rp_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipmc_rp_remove_verify(
          unit,
          rp_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipmc_rp_remove_unsafe(
          unit,
          rp_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipmc_rp_remove()", 0, 0);
}

/*********************************************************************
*     Setting global information of the VRF including
 *     (defaults forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_VRF_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  res = arad_pp_frwrd_ipv4_vrf_info_set_verify(
          unit,
          vrf_ndx,
          vrf_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_vrf_info_set_unsafe(
          unit,
          vrf_ndx,
          vrf_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_vrf_info_set()", vrf_ndx, 0);
}

/*********************************************************************
*     Setting global information of the VRF including
 *     (defaults forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VRF_INFO                 *vrf_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_VRF_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vrf_info);

  res = arad_pp_frwrd_ipv4_vrf_info_get_verify(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_vrf_info_get_unsafe(
          unit,
          vrf_ndx,
          vrf_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_vrf_info_get()", vrf_ndx, 0);
}

/*********************************************************************
*     Add IPv4 route entry to the virtual routing table (VRF).
 *     Binds between Ipv4 route key (UC/MC IPv4-address\prefix)
 *     and a FEC entry identified by fec_id for a given virtual
 *     router. As a result of this operation, Unicast Ipv4
 *     packets designated to IP address matching the given key
 *     (as long there is no more-specific route key) will be
 *     routed according to the information in the FEC entry
 *     identified by fec_id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_route_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPC_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv4_vrf_route_add_verify(
          unit,
          vrf_ndx,
          route_key,
          fec_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_vrf_route_add_unsafe(
          unit,
          vrf_ndx,
          route_key,
          fec_id,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_vrf_route_add()", vrf_ndx, 0);
}

/*********************************************************************
*     Gets the routing information (system-fec-id) associated
 *     with the given route key on VRF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_route_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(fec_id);
  ;
  
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_ipv4_vrf_route_get_verify(
          unit,
          vrf_ndx,
          route_key,
          exact_match
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_vrf_route_get_unsafe(
          unit,
          vrf_ndx,
          route_key,
          exact_match,
          fec_id,
          route_status,
          location,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_vrf_route_get()", vrf_ndx, 0);
}

/*********************************************************************
*     Gets the routing table of a virtual router (VRF).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPC_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PPC_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_keys);
  SOC_SAND_CHECK_NULL_INPUT(fec_ids);
  SOC_SAND_CHECK_NULL_INPUT(routes_status);
  SOC_SAND_CHECK_NULL_INPUT(routes_location);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_ipv4_vrf_route_get_block_verify(
          unit,
          vrf_ndx,
          block_range_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_vrf_route_get_block_unsafe(
          unit,
          vrf_ndx,
          block_range_key,
          route_keys,
          fec_ids,
          routes_status,
          routes_location,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_vrf_route_get_block()", vrf_ndx, 0);
}

/*********************************************************************
*     Remove IPv4 route entry from the routing table of a
 *     virtual router (VRF).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_route_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_ipv4_vrf_route_remove_verify(
          unit,
          vrf_ndx,
          route_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_vrf_route_remove_unsafe(
          unit,
          vrf_ndx,
          route_key,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_vrf_route_remove()", vrf_ndx, 0);
}

/*********************************************************************
*     Clear IPv4 routing table of VRF
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_routing_table_clear(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VRF_ID                              vrf_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipv4_vrf_routing_table_clear_verify(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_vrf_routing_table_clear_unsafe(
          unit,
          vrf_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_vrf_routing_table_clear()", vrf_ndx, 0);
}

/*********************************************************************
*     Clear IPv4 routing tables for all VRFs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_frwrd_ipv4_vrf_all_routing_tables_clear_verify(
          unit,
          flags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_vrf_all_routing_tables_clear_unsafe(
          unit,
          flags
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear()", flags, 0);
}

/*********************************************************************
*     Returns the status and usage of memory.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mem_status_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MEM_STATUS      *mem_status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_IPV4_MEM_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mem_status);

  res = arad_pp_frwrd_ipv4_mem_status_get_verify(
          unit,
          mem_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_mem_status_get_unsafe(
          unit,
          mem_ndx,
          mem_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_mem_status_get()", mem_ndx, 0);
}

/*********************************************************************
*     Returns the status and usage of memory.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mem_defrage(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_IPV4_MEM_DEFRAGE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(defrag_info);

  res = arad_pp_frwrd_ipv4_mem_defrage_verify(
          unit,
          mem_ndx,
          defrag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_ipv4_mem_defrage_unsafe(
          unit,
          mem_ndx,
          defrag_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_ipv4_mem_defrage()", mem_ndx, 0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>



