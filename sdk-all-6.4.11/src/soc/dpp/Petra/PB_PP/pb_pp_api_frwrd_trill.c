/* $Id: pb_pp_api_frwrd_trill.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_frwrd_trill.c
*
* MODULE PREFIX:  soc_pb_pp
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_trill.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_trill.h>

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
*     Set the key type of TRILL multicast routes lookup. The
 *     following fields are optional: Ing-Nick-key;
 *     Adjacent-EEP-key; FID-key
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_key_mask_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  res = soc_pb_pp_frwrd_trill_multicast_key_mask_set_verify(
          unit,
          masked_fields
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_multicast_key_mask_set_unsafe(
          unit,
          masked_fields
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_key_mask_set()", 0, 0);
}

/*********************************************************************
*     Set the key type of TRILL multicast routes lookup. The
 *     following fields are optional: Ing-Nick-key;
 *     Adjacent-EEP-key; FID-key
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_key_mask_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PB_PP_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  res = soc_pb_pp_frwrd_trill_multicast_key_mask_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_multicast_key_mask_get_unsafe(
          unit,
          masked_fields
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_key_mask_get()", 0, 0);
}

/*********************************************************************
*     Map nick-name to a FEC entry ID. Used for forwarding
 *     packets with the nick name as destination to the FEC,
 *     and to associate the FEC as learning information, upon
 *     receiving packets with the Nick-Name as the source
 *     address
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_add(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID            lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO   *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_frwrd_trill_unicast_route_add_verify(
          unit,
          lif_index,
          nickname_key,
          trill_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_unicast_route_add_unsafe(
          unit,
          lif_index,
          nickname_key,
          trill_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_add()", nickname_key, 0);
}

/*********************************************************************
*     Get mapping of TRILL nickname to FEC ID and LIF index
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        nickname_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID             *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_TRILL_INFO  *trill_info,
    SOC_SAND_OUT uint8                        *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(trill_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  res = soc_pb_pp_frwrd_trill_unicast_route_get_verify(
          unit,
          nickname_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_unicast_route_get_unsafe(
          unit,
          nickname_key,
          lif_index,
          trill_info,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_get()", nickname_key, 0);
}

/*********************************************************************
*     Remove TRILL nick-name
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                  *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_frwrd_trill_unicast_route_remove_verify(
          unit,
          nickname_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_unicast_route_remove_unsafe(
          unit,
          nickname_key,
          lif_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_remove()", nickname_key, 0);
}

/*********************************************************************
*     Map a TRILL distribution tree to a FEC
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                      mc_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_frwrd_trill_multicast_route_add_verify(
          unit,
          trill_mc_key,
          mc_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_multicast_route_add_unsafe(
          unit,
          trill_mc_key,
          mc_id,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_add()", 0, 0);
}

/*********************************************************************
*     Get Mapping of TRILL distribution tree to a FEC
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_OUT uint32                      *mc_id,
    SOC_SAND_OUT uint8                     *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(mc_id);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  *mc_id = 0;

  res = soc_pb_pp_frwrd_trill_multicast_route_get_verify(
          unit,
          trill_mc_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_multicast_route_get_unsafe(
          unit,
          trill_mc_key,
          mc_id,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_get()", 0, 0);
}

/*********************************************************************
*     Remove a TRILL distribution tree mapping
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);

  res = soc_pb_pp_frwrd_trill_multicast_route_remove_verify(
          unit,
          trill_mc_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_multicast_route_remove_unsafe(
          unit,
          trill_mc_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_remove()", 0, 0);
}

/*********************************************************************
*     Map SA MAC adress to expected adjacent EEP and expected
 *     system port in SA-Based_adj db. Used for authenticating
 *     incoming trill packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_adj_info_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_frwrd_trill_adj_info_set_verify(
          unit,
          mac_address_key,
          mac_auth_info,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_adj_info_set_unsafe(
          unit,
          mac_address_key,
          mac_auth_info,
          enable,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_adj_info_set()", 0, 0);
}

/*********************************************************************
*     Map SA MAC adress to expected adjacent EEP and expected
 *     system port in SA-Based_adj db. Used for authenticating
 *     incoming trill packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_adj_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_OUT SOC_PB_PP_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_OUT uint8                     *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = soc_pb_pp_frwrd_trill_adj_info_get_verify(
          unit,
          mac_address_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_adj_info_get_unsafe(
          unit,
          mac_address_key,
          mac_auth_info,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_adj_info_get()", 0, 0);
}

/*********************************************************************
*     Set TRILL global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_global_info_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = soc_pb_pp_frwrd_trill_global_info_set_verify(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_global_info_set_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_global_info_set()", 0, 0);
}

/*********************************************************************
*     Set TRILL global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_global_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  res = soc_pb_pp_frwrd_trill_global_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_frwrd_trill_global_info_get_unsafe(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_global_info_get()", 0, 0);
}

void
  SOC_PB_PP_TRILL_MC_MASKED_FIELDS_clear(
    SOC_SAND_OUT SOC_PB_PP_TRILL_MC_MASKED_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_MC_MASKED_FIELDS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_TRILL_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_TRILL_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_MC_ROUTE_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_TRILL_ADJ_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRILL_ADJ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_ADJ_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

void
  SOC_PB_PP_TRILL_MC_MASKED_FIELDS_print(
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_MASKED_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_MC_MASKED_FIELDS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_TRILL_MC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_MC_ROUTE_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_TRILL_ADJ_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRILL_ADJ_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

