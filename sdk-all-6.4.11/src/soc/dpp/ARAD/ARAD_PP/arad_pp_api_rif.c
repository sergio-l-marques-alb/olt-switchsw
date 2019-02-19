#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_rif.c,v 1.8 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_VSI

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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>

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
*     Set the MPLS labels that may be mapped to Router
*     Interfaces
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_mpls_labels_range_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABELS_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  res = arad_pp_rif_mpls_labels_range_set_verify(
          unit,
          rif_labels_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_labels_range_set_unsafe(
          unit,
          rif_labels_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_labels_range_set()", 0, 0);
}

/*********************************************************************
*     Set the MPLS labels that may be mapped to Router
*     Interfaces
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_mpls_labels_range_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABELS_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_labels_range);

  res = arad_pp_rif_mpls_labels_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_labels_range_get_unsafe(
          unit,
          rif_labels_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_labels_range_get()", 0, 0);
}

/*********************************************************************
*     Enable MPLS labels termination and setting the Router
*     interface according to the terminated MPLS label.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_mpls_label_map_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_rif_mpls_label_map_add_verify(
          unit,
          mpls_key,
          lif_index,
          term_info,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_label_map_add_unsafe(
          unit,
          mpls_key,
          lif_index,
          term_info,
          rif_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_add()", 0, 0);
}

/*********************************************************************
*     Remove MPLS label that was mapped to a RIF-Tunnel
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_mpls_label_map_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_INOUT ARAD_PP_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  res = arad_pp_rif_mpls_label_map_remove_verify(
          unit,
          mpls_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_label_map_remove_unsafe(
          unit,
          mpls_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_remove()", 0, 0);
}

/*********************************************************************
*     Get MPLS label termination and Router interface info
*     according to the terminated MPLS label.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_mpls_label_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_OUT ARAD_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_MPLS_LABEL_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mpls_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_rif_mpls_label_map_get_verify(
          unit,
          mpls_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_mpls_label_map_get_unsafe(
          unit,
          mpls_key,
          lif_index,
          term_info,
          rif_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_mpls_label_map_get()", 0, 0);
}

/*********************************************************************
*     Enable IP Tunnels termination and setting the Router
*     interface according to the terminated IP tunnel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_ip_tunnel_term_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 flags,
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_rif_ip_tunnel_term_add_verify(
          unit,
          flags, 
          term_key,
          lif_index,
          term_info,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ip_tunnel_term_add_unsafe(
          unit,
          flags, 
          term_key,
          lif_index,
          term_info,
          rif_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_add()", lif_index, 0);
}

/*********************************************************************
*     Remove the IP Tunnel
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_ip_tunnel_term_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);

  res = arad_pp_rif_ip_tunnel_term_remove_verify(
          unit,
          term_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ip_tunnel_term_remove_unsafe(
          unit,
          term_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_remove()", 0, 0);
}

/*********************************************************************
*     Get IP Tunnels termination and Router interface info
*     according to the terminated IP tunnel.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_ip_tunnel_term_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_KEY               *term_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT ARAD_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_OUT ARAD_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_IP_TUNNEL_TERM_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_key);
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(term_info);
  SOC_SAND_CHECK_NULL_INPUT(rif_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_rif_ip_tunnel_term_get_verify(
          unit,
          term_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ip_tunnel_term_get_unsafe(
          unit,
          term_key,
          lif_index,
          term_info,
          rif_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ip_tunnel_term_get()", 0, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_vsid_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_vsid_map_set_verify(
          unit,
          vsid_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_vsid_map_set_unsafe(
          unit,
          vsid_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_vsid_map_set()", vsid_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_vsid_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_OUT ARAD_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_VSID_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_vsid_map_get_verify(
          unit,
          vsid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_vsid_map_get_unsafe(
          unit,
          vsid_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_vsid_map_get()", vsid_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_ID                              rif_ndx,
    SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_info_set_verify(
          unit,
          rif_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_info_set_unsafe(
          unit,
          rif_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_info_set()", rif_ndx, 0);
}

/*********************************************************************
*     Set the Router Interface according to the VSID.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_ID                              rif_ndx,
    SOC_SAND_OUT ARAD_PP_RIF_INFO                            *rif_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rif_info);

  res = arad_pp_rif_info_get_verify(
          unit,
          rif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_info_get_unsafe(
          unit,
          rif_ndx,
          rif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_info_get()", rif_ndx, 0);
}

/*********************************************************************
*     Set TTL value for TTL-scope.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_ttl_scope_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_TTL_SCOPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_rif_ttl_scope_set_verify(
          unit,
          ttl_scope_ndx,
          ttl_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ttl_scope_set_unsafe(
          unit,
          ttl_scope_ndx,
          ttl_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ttl_scope_set()", ttl_scope_ndx, 0);
}

/*********************************************************************
*     Set TTL value for TTL-scope.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_rif_ttl_scope_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_RIF_TTL_SCOPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ttl_val);

  res = arad_pp_rif_ttl_scope_get_verify(
          unit,
          ttl_scope_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_rif_ttl_scope_get_unsafe(
          unit,
          ttl_scope_ndx,
          ttl_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_ttl_scope_get()", ttl_scope_ndx, 0);
}


/*********************************************************************
*     set native vsi compensation per rif
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
arad_pp_rif_native_routing_vlan_tags_set(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  ARAD_PP_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   ) { 
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
    
    res = arad_pp_rif_native_routing_vlan_tags_set_verify(
       unit, 
       intf_id, 
       native_routing_vlan_tags
       ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
    
    SOC_SAND_TAKE_DEVICE_SEMAPHORE; 
    
    res = arad_pp_rif_native_routing_vlan_tags_set_unsafe(
       unit, 
       intf_id, 
       native_routing_vlan_tags
       ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); 
    
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE; 
    ARAD_PP_DO_NOTHING_AND_EXIT; 
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_native_routing_vlan_tags_set()", intf_id, 0);
}



/*********************************************************************
*     get native vsi compensation per rif
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
arad_pp_rif_native_routing_vlan_tags_get(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  ARAD_PP_RIF_ID                      intf_id,
   SOC_SAND_OUT  uint8                              *native_routing_vlan_tags
   ) { 
    uint32
       res = SOC_SAND_OK; 
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
    
    SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
    
    res = arad_pp_rif_native_routing_vlan_tags_get_verify(
       unit, 
       intf_id, 
       native_routing_vlan_tags
       ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 
    
    SOC_SAND_TAKE_DEVICE_SEMAPHORE; 
    
    res = arad_pp_rif_native_routing_vlan_tags_get_unsafe(
       unit, 
       intf_id, 
       native_routing_vlan_tags
       ); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore); 
    
exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE; 
    ARAD_PP_DO_NOTHING_AND_EXIT; 
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_rif_native_routing_vlan_tags_get()", intf_id, 0);
}


void
  ARAD_PP_RIF_MPLS_LABELS_RANGE_clear(
    SOC_SAND_OUT ARAD_PP_RIF_MPLS_LABELS_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_MPLS_LABELS_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  ARAD_PP_MPLS_LABEL_RIF_KEY_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_LABEL_RIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_LABEL_RIF_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_RIF_INFO_clear(
    SOC_SAND_OUT ARAD_PP_RIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_RIF_IP_TERM_INFO_clear(
    SOC_SAND_OUT ARAD_PP_RIF_IP_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_IP_TERM_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_RIF_IP_TERM_KEY_clear(
    SOC_SAND_OUT ARAD_PP_RIF_IP_TERM_KEY *key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(key);

  SOC_PPC_RIF_IP_TERM_KEY_clear(key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if ARAD_PP_DEBUG_IS_LVL1


void
  ARAD_PP_RIF_MPLS_LABELS_RANGE_print(
    SOC_SAND_IN  ARAD_PP_RIF_MPLS_LABELS_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_MPLS_LABELS_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_LABEL_RIF_KEY_print(
    SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_LABEL_RIF_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_RIF_INFO_print(
    SOC_SAND_IN  ARAD_PP_RIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_RIF_IP_TERM_INFO_print(
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_RIF_IP_TERM_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  ARAD_PP_RIF_IP_TERM_KEY_print(
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_KEY *key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(key);

  SOC_PPC_RIF_IP_TERM_KEY_print(key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */
