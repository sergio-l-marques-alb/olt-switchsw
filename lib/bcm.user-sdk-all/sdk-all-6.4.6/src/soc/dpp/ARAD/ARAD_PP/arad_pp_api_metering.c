#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_metering.c,v 1.15 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_METERING
#include <shared/bsl.h>

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

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>

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
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_meters_group_info_set(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int									 core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  res = arad_pp_mtr_meters_group_info_set_verify(
          unit,
		  core_id,
          mtr_group_ndx,
          mtr_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_meters_group_info_set_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          mtr_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_set()", mtr_group_ndx, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_meters_group_info_get(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  res = arad_pp_mtr_meters_group_info_get_verify(
	      core_id,
          unit,
          mtr_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_meters_group_info_get_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          mtr_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_get()", mtr_group_ndx, 0);
}

/*********************************************************************
*     Add Bandwidth Profile and set it attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_add(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO             *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_mtr_bw_profile_add_verify(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx,
          bw_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_bw_profile_add_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx,
          bw_profile_info,
          exact_bw_profile_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_add()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Get Bandwidth Profile attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_get(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);

  res = arad_pp_mtr_bw_profile_get_verify(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_bw_profile_get_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx,
          bw_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_get()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Remove Bandwidth Profile
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_remove(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_mtr_bw_profile_remove_verify(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_bw_profile_remove_unsafe(
          unit,
		  core_id,
          mtr_group_ndx,
          bw_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_remove()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Set meter attributes by mapping meter instance to
 *     bandwidth profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);

  res = arad_pp_mtr_meter_ins_to_bw_profile_map_set_verify(
          unit,
		  core_id,
          meter_ins_ndx,
          bw_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe(
          unit,
		  core_id,
          meter_ins_ndx,
          bw_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_set()", 0, 0);
}

/*********************************************************************
*     Set meter attributes by mapping meter instance to
 *     bandwidth profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get(
    SOC_SAND_IN  int                                  unit,
	SOC_SAND_IN  int									 core_id,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_OUT uint32                                  *bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);
  SOC_SAND_CHECK_NULL_INPUT(bw_profile_id);

  res = arad_pp_mtr_meter_ins_to_bw_profile_map_get_verify(
          unit,
		  core_id,
          meter_ins_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe(
          unit,
		  core_id,
          meter_ins_ndx,
          bw_profile_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_get()", 0, 0);
}

/*********************************************************************
*     Enable / Disable Ethernet policing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_enable_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_mtr_eth_policer_enable_set_verify(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_enable_set_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_set()", 0, 0);
}

/*********************************************************************
*     Enable / Disable Ethernet policing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_enable_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = arad_pp_mtr_eth_policer_enable_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_enable_get_unsafe(
          unit,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_get()", 0, 0);
}

/*********************************************************************
*     Set policer attributes of the Ethernet policer. Enable
 *     policing per ingress port and Ethernet type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_params_set(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  res = arad_pp_mtr_eth_policer_params_set_verify(
          unit,
          port_ndx,
          eth_type_ndx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_params_set_unsafe(
          unit,
          port_ndx,
          eth_type_ndx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_set()", port_ndx, 0);
}

/*********************************************************************
*     Set policer attributes of the Ethernet policer. Enable
 *     policing per ingress port and Ethernet type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_params_get(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  res = arad_pp_mtr_eth_policer_params_get_verify(
          unit,
          port_ndx,
          eth_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_params_get_unsafe(
          unit,
          port_ndx,
          eth_type_ndx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_get()", port_ndx, 0);
}

/*********************************************************************
*     Set Ethernet policer Global Profile attributes.         
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  res = arad_pp_mtr_eth_policer_glbl_profile_set_verify(
          unit,
          glbl_profile_idx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_glbl_profile_set_unsafe(
          unit,
          glbl_profile_idx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_set()", 0, 0);
}

/*********************************************************************
*     Set Ethernet policer Global Profile attributes.         
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  res = arad_pp_mtr_eth_policer_glbl_profile_get_verify(
          unit,
          glbl_profile_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_glbl_profile_get_unsafe(
          unit,
          glbl_profile_idx,
          policer_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_get()", 0, 0);
}

/*********************************************************************
*     Map Ethernet policer per ingress port and Ethernet type 
 *     to Ethernet policer Global Profile.                     
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_mtr_eth_policer_glbl_profile_map_set_verify(
          unit,
          port_ndx,
          eth_type_ndx,
          glbl_profile_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe(
          unit,
          port_ndx,
          eth_type_ndx,
          glbl_profile_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_set()", 0, 0);
}

/*********************************************************************
*     Map Ethernet policer per ingress port and Ethernet type 
 *     to Ethernet policer Global Profile.                     
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_OUT uint32                  *glbl_profile_idx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_profile_idx);

  res = arad_pp_mtr_eth_policer_glbl_profile_map_get_verify(
          unit,
          port_ndx,
          eth_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mtr_eth_policer_glbl_profile_map_get_unsafe(
          unit,
          port_ndx,
          eth_type_ndx,
          glbl_profile_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_get()", 0, 0);
}
void
  ARAD_PP_MTR_GROUP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_GROUP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MTR_BW_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_BW_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_MTR_ETH_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE enum_val
  )
{
  return SOC_PPC_MTR_ETH_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_MTR_COLOR_MODE_to_string(
    SOC_SAND_IN  ARAD_PP_MTR_COLOR_MODE enum_val
  )
{
  return SOC_PPC_MTR_COLOR_MODE_to_string(enum_val);
}

const char*
  ARAD_PP_MTR_RES_USE_to_string(
    SOC_SAND_IN  ARAD_PP_MTR_RES_USE enum_val
  )
{
  return SOC_PPC_MTR_RES_USE_to_string(enum_val);
}
void
  ARAD_PP_MTR_METER_ID_print(
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_METER_ID_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MTR_GLBL_INFO_print(
    SOC_SAND_IN  ARAD_PP_MTR_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MTR_GROUP_INFO_print(
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_GROUP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MTR_BW_PROFILE_INFO_print(
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_BW_PROFILE_INFO_print(info);
  LOG_CLI((BSL_META_U(unit,
                      "is_sharing_enabled: %u\n\r"),info->is_sharing_enabled));
  if (info->is_sharing_enabled) {
      LOG_CLI((BSL_META_U(unit,
                          "max_cir: %u\n\r"),info->max_cir));
  }
  LOG_CLI((BSL_META_U(unit,
                      "max_eir: %u\n\r"),info->max_eir));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */

