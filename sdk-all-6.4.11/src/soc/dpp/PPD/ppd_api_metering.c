/* $Id: ppd_api_metering.c,v 1.19 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_metering.c
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
#include <soc/dpp/PPD/ppd_api_metering.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_metering.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_metering.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_metering.h>
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

/* core parameter supported only in ARAD soc functions */
#define SOC_PPD_MTR_DEVICE_CALL_WITH_CORE(func, params_with_core, params_witout_core)    \
  switch (SOC_SAND_DEVICE_TYPE_GET(unit))        \
  {                                               \
    case SOC_SAND_DEV_PB:                             \
      SOC_PB_PP_DEVICE_CALL(func, params_witout_core);            \
      break;                                      \
    case SOC_SAND_DEV_ARAD:                             \
	  ARAD_PP_DEVICE_CALL(func, params_with_core);            \
	  break;                                      \
    default:                                      \
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit); \
      break; \
  }

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
    Ppd_procedure_desc_element_metering[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_METERS_GROUP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_METERS_GROUP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_METERS_GROUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_METERS_GROUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_BW_PROFILE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_BW_PROFILE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_BW_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_BW_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_BW_PROFILE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_BW_PROFILE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_ENABLE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_ENABLE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_PARAMS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_PARAMS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_PARAMS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_PARAMS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_METERING_GET_PROCS_PTR),
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
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_glbl_info_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPD_MTR_GLBL_INFO                     *mtr_glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtr_glbl_info);

  SOC_PPD_DEVICE_CALL(mtr_glbl_info_set,(unit, mtr_glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_glbl_info_set_print,(unit,mtr_glbl_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_glbl_info_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT SOC_PPD_MTR_GLBL_INFO                     *mtr_glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtr_glbl_info);

  SOC_PPD_DEVICE_CALL(mtr_glbl_info_get,(unit, mtr_glbl_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_glbl_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_meters_group_info_set(
    SOC_SAND_IN  int                                unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_GROUP_INFO                *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_METERS_GROUP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  SOC_PPD_MTR_DEVICE_CALL_WITH_CORE(mtr_meters_group_info_set, 
									(unit, core_id, mtr_group_ndx, mtr_group_info), 
									(unit, mtr_group_ndx, mtr_group_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_meters_group_info_set_print,(unit,core_id,mtr_group_ndx,mtr_group_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_meters_group_info_set()", mtr_group_ndx, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_meters_group_info_get(
    SOC_SAND_IN  int                                unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_OUT SOC_PPD_MTR_GROUP_INFO                *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_METERS_GROUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  SOC_PPD_MTR_DEVICE_CALL_WITH_CORE(mtr_meters_group_info_get, 
									(unit, core_id, mtr_group_ndx, mtr_group_info), 
									(unit, mtr_group_ndx, mtr_group_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_meters_group_info_get_print,(unit,core_id,mtr_group_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_meters_group_info_get()", mtr_group_ndx, 0);
}

/*********************************************************************
*     Add Bandwidth Profile and set it attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_bw_profile_add(
    SOC_SAND_IN  int                                unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_BW_PROFILE_INFO           *bw_profile_info,
    SOC_SAND_OUT SOC_PPD_MTR_BW_PROFILE_INFO           *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE              *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_BW_PROFILE_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_MTR_DEVICE_CALL_WITH_CORE(mtr_bw_profile_add, 
									(unit, core_id, mtr_group_ndx, bw_profile_ndx, bw_profile_info, exact_bw_profile_info, success), 
									(unit, mtr_group_ndx, bw_profile_ndx, bw_profile_info, exact_bw_profile_info, success)); 

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_bw_profile_add_print,(unit,core_id,mtr_group_ndx,bw_profile_ndx,bw_profile_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_bw_profile_add()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Get Bandwidth Profile attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_bw_profile_get(
    SOC_SAND_IN  int                                unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx,
    SOC_SAND_OUT SOC_PPD_MTR_BW_PROFILE_INFO           *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_BW_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);

  SOC_PPD_MTR_DEVICE_CALL_WITH_CORE(mtr_bw_profile_get,
									(unit, core_id, mtr_group_ndx, bw_profile_ndx, bw_profile_info),
									(unit, mtr_group_ndx, bw_profile_ndx, bw_profile_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_bw_profile_get_print,(unit,core_id,mtr_group_ndx,bw_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_bw_profile_get()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Remove Bandwidth Profile
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_bw_profile_remove(
    SOC_SAND_IN  int                                unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_BW_PROFILE_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_MTR_DEVICE_CALL_WITH_CORE(mtr_bw_profile_remove,
									(unit, core_id, mtr_group_ndx, bw_profile_ndx),
									(unit, mtr_group_ndx, bw_profile_ndx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_bw_profile_remove_print,(unit,core_id,mtr_group_ndx,bw_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_bw_profile_remove()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Set meter attributes by mapping meter instance to
 *     bandwidth profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_meter_ins_to_bw_profile_map_set(
    SOC_SAND_IN  int                                unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  SOC_PPD_MTR_METER_ID                  *meter_ins_ndx,
    SOC_SAND_IN  uint32                                bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);

  SOC_PPD_MTR_DEVICE_CALL_WITH_CORE(mtr_meter_ins_to_bw_profile_map_set,
								    (unit, core_id, meter_ins_ndx, bw_profile_id),
									(unit, meter_ins_ndx, bw_profile_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_meter_ins_to_bw_profile_map_set_print,(unit,core_id,meter_ins_ndx,bw_profile_id));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_meter_ins_to_bw_profile_map_set()", 0, 0);
}

/*********************************************************************
*     Set meter attributes by mapping meter instance to
 *     bandwidth profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_meter_ins_to_bw_profile_map_get(
    SOC_SAND_IN  int                                unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  SOC_PPD_MTR_METER_ID                  *meter_ins_ndx,
    SOC_SAND_OUT uint32                                *bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_METER_INS_TO_BW_PROFILE_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);
  SOC_SAND_CHECK_NULL_INPUT(bw_profile_id);

  SOC_PPD_MTR_DEVICE_CALL_WITH_CORE(mtr_meter_ins_to_bw_profile_map_get,
									(unit, core_id, meter_ins_ndx, bw_profile_id),
									(unit, meter_ins_ndx, bw_profile_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_meter_ins_to_bw_profile_map_get_print,(unit,core_id,meter_ins_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_meter_ins_to_bw_profile_map_get()", 0, 0);
}

/*********************************************************************
*     Enable / Disable Ethernet policing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_eth_policer_enable_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint8                               enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_ETH_POLICER_ENABLE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mtr_eth_policer_enable_set,(unit, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_eth_policer_enable_set_print,(unit,enable));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_enable_set()", 0, 0);
}

/*********************************************************************
*     Enable / Disable Ethernet policing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_eth_policer_enable_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT uint8                               *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_ETH_POLICER_ENABLE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mtr_eth_policer_enable_get,(unit, enable));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_eth_policer_enable_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_enable_get()", 0, 0);
}

/*********************************************************************
*     Set policer attributes of the Ethernet policer. Enable
 *     policing per ingress port and Ethernet type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_eth_policer_params_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  SOC_PPD_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_ETH_POLICER_PARAMS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mtr_eth_policer_params_set,(unit, port_ndx, eth_type_ndx, policer_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_eth_policer_params_set_print,(unit,port_ndx,eth_type_ndx,policer_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_params_set()", port_ndx, 0);
}

/*********************************************************************
*     Set policer attributes of the Ethernet policer. Enable
 *     policing per ingress port and Ethernet type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_mtr_eth_policer_params_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  SOC_PPD_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT SOC_PPD_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_ETH_POLICER_PARAMS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mtr_eth_policer_params_get,(unit, port_ndx, eth_type_ndx, policer_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_eth_policer_params_get_print,(unit,port_ndx,eth_type_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_params_get()", port_ndx, 0);
}

/*********************************************************************
*     Set Ethernet policer Global Profile attributes.         
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx,
    SOC_SAND_IN  SOC_PPD_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mtr_eth_policer_glbl_profile_set,(unit, glbl_profile_idx, policer_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_eth_policer_glbl_profile_set_print,(unit,glbl_profile_idx,policer_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_glbl_profile_set()", 0, 0);
}

/*********************************************************************
*     Set Ethernet policer Global Profile attributes.         
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  glbl_profile_idx,
    SOC_SAND_OUT SOC_PPD_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mtr_eth_policer_glbl_profile_get,(unit, glbl_profile_idx, policer_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_eth_policer_glbl_profile_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_glbl_profile_get()", 0, 0);
}

/*********************************************************************
*     Map Ethernet policer per ingress port and Ethernet type 
 *     to Ethernet policer Global Profile.                     
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_map_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mtr_eth_policer_glbl_profile_map_set,(unit, port_ndx, eth_type_ndx, glbl_profile_idx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_eth_policer_glbl_profile_map_set_print,(unit,port_ndx,eth_type_ndx,glbl_profile_idx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_glbl_profile_map_set()", 0, 0);
}

/*********************************************************************
*     Map Ethernet policer per ingress port and Ethernet type 
 *     to Ethernet policer Global Profile.                     
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  soc_ppd_mtr_eth_policer_glbl_profile_map_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_OUT uint32                  *glbl_profile_idx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(glbl_profile_idx);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(mtr_eth_policer_glbl_profile_map_get,(unit, port_ndx, eth_type_ndx, glbl_profile_idx));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_mtr_eth_policer_glbl_profile_map_get_print,(unit,port_ndx,eth_type_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_mtr_eth_policer_glbl_profile_map_get()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_metering module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_metering_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_metering;
}
void
  SOC_PPD_MTR_METER_ID_clear(
    SOC_SAND_OUT SOC_PPD_MTR_METER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_METER_ID_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MTR_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_MTR_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MTR_GROUP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_GROUP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MTR_BW_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_BW_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MTR_COLOR_DECISION_INFO_clear(
    SOC_SAND_OUT SOC_PPD_MTR_COLOR_DECISION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_COLOR_DECISION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MTR_COLOR_RESOLUTION_INFO_clear(
    SOC_SAND_OUT SOC_PPD_MTR_COLOR_RESOLUTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_COLOR_RESOLUTION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_MTR_ETH_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE enum_val
  )
{
  return SOC_PPC_MTR_ETH_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_MTR_COLOR_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_MTR_COLOR_MODE enum_val
  )
{
  return SOC_PPC_MTR_COLOR_MODE_to_string(enum_val);
}

const char*
  SOC_PPD_MTR_RES_USE_to_string(
    SOC_SAND_IN  SOC_PPD_MTR_RES_USE enum_val
  )
{
  return SOC_PPC_MTR_RES_USE_to_string(enum_val);
}

const char*
  SOC_PPD_MTR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_MTR_TYPE enum_val
  )
{
  return SOC_PPC_MTR_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_MTR_MODE_to_string(
    SOC_SAND_IN  bcm_policer_mode_t enum_val
  )
{
	return SOC_PPC_MTR_MODE_to_string(enum_val);
}

void
  SOC_PPD_MTR_METER_ID_print(
    SOC_SAND_IN  SOC_PPD_MTR_METER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_METER_ID_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MTR_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_MTR_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MTR_GROUP_INFO_print(
    SOC_SAND_IN  SOC_PPD_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_GROUP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_MTR_BW_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MTR_BW_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_mtr_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_MTR_GLBL_INFO                       *mtr_glbl_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mtr_glbl_info:")));
  SOC_PPD_MTR_GLBL_INFO_print((mtr_glbl_info));

  return;
}
void
  soc_ppd_mtr_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_mtr_meters_group_info_set_print(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_GROUP_INFO                *mtr_group_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "core_id: %lu\n\r"),core_id));

  LOG_CLI((BSL_META_U(unit,
                      "mtr_group_ndx: %lu\n\r"),mtr_group_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "mtr_group_info:")));
  SOC_PPD_MTR_GROUP_INFO_print((mtr_group_info));

  return;
}
void
  soc_ppd_mtr_meters_group_info_get_print(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "core_id: %lu\n\r"),core_id));

  LOG_CLI((BSL_META_U(unit,
                      "mtr_group_ndx: %lu\n\r"),mtr_group_ndx));

  return;
}
void
  soc_ppd_mtr_bw_profile_add_print(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_BW_PROFILE_INFO                 *bw_profile_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "core_id: %lu\n\r"),core_id));

  LOG_CLI((BSL_META_U(unit,
                      "mtr_group_ndx: %lu\n\r"),mtr_group_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "bw_profile_ndx: %lu\n\r"),bw_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "bw_profile_info:")));
  SOC_PPD_MTR_BW_PROFILE_INFO_print((bw_profile_info));

  return;
}
void
  soc_ppd_mtr_bw_profile_get_print(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx
  )
{
  LOG_CLI((BSL_META_U(unit,
                      "core_id: %lu\n\r"),core_id));

  LOG_CLI((BSL_META_U(unit,
                      "mtr_group_ndx: %lu\n\r"),mtr_group_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "bw_profile_ndx: %lu\n\r"),bw_profile_ndx));

  return;
}
void
  soc_ppd_mtr_bw_profile_remove_print(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  uint32                                mtr_group_ndx,
    SOC_SAND_IN  uint32                                bw_profile_ndx
  )
{
  LOG_CLI((BSL_META_U(unit,
                      "core_id: %lu\n\r"),core_id));

  LOG_CLI((BSL_META_U(unit,
                      "mtr_group_ndx: %lu\n\r"),mtr_group_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "bw_profile_ndx: %lu\n\r"),bw_profile_ndx));

  return;
}
void
  soc_ppd_mtr_meter_ins_to_bw_profile_map_set_print(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  SOC_PPD_MTR_METER_ID                  *meter_ins_ndx,
    SOC_SAND_IN  uint32                                bw_profile_id
  )
{
  LOG_CLI((BSL_META_U(unit,
                      "core_id: %lu\n\r"),core_id));

  LOG_CLI((BSL_META_U(unit,
                      "meter_ins_ndx:")));
  SOC_PPD_MTR_METER_ID_print((meter_ins_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "bw_profile_id: %lu\n\r"),bw_profile_id));

  return;
}
void
  soc_ppd_mtr_meter_ins_to_bw_profile_map_get_print(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                                   core_id,
    SOC_SAND_IN  SOC_PPD_MTR_METER_ID                  *meter_ins_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "core_id: %lu\n\r"),core_id));

  LOG_CLI((BSL_META_U(unit,
                      "meter_ins_ndx:")));
  SOC_PPD_MTR_METER_ID_print((meter_ins_ndx));

  return;
}
void
  soc_ppd_mtr_eth_policer_enable_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                               enable
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),enable));

  return;
}
void
  soc_ppd_mtr_eth_policer_enable_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_mtr_eth_policer_params_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_BW_PROFILE_INFO                 *policer_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_ndx: %lu\n\r"),port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "eth_type_ndx %s "), SOC_PPD_MTR_ETH_TYPE_to_string(eth_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "policer_info:")));
  SOC_PPD_MTR_BW_PROFILE_INFO_print((policer_info));

  return;
}
void
  soc_ppd_mtr_eth_policer_params_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE                        eth_type_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_ndx: %lu\n\r"),port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "eth_type_ndx %s "), SOC_PPD_MTR_ETH_TYPE_to_string(eth_type_ndx)));

  return;
}

void  
soc_ppd_mtr_eth_policer_glbl_profile_set_print(
  SOC_SAND_IN  int               unit,
  SOC_SAND_IN  uint32                glbl_profile_idx,
  SOC_SAND_IN  SOC_PPD_MTR_BW_PROFILE_INFO *policer_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "glbl_profile_idx: %lu\n\r"),glbl_profile_idx));

  LOG_CLI((BSL_META_U(unit,
                      "policer_info:")));
  SOC_PPD_MTR_BW_PROFILE_INFO_print((policer_info));

  return; 
}
void  
soc_ppd_mtr_eth_policer_glbl_profile_get_print(
  SOC_SAND_IN  int               unit
  )
{

  return; 
}
void  
soc_ppd_mtr_eth_policer_glbl_profile_map_set_print(
  SOC_SAND_IN  int               unit,
  SOC_SAND_IN  SOC_PPD_PORT                port_ndx,
  SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE        eth_type_ndx,
  SOC_SAND_IN  uint32                glbl_profile_idx
  )
{


  LOG_CLI((BSL_META_U(unit,
                      "eth_type_ndx %s "), SOC_PPD_MTR_ETH_TYPE_to_string(eth_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "glbl_profile_idx: %lu\n\r"),glbl_profile_idx));

  return; 
}
void  
soc_ppd_mtr_eth_policer_glbl_profile_map_get_print(
  SOC_SAND_IN  int               unit,
  SOC_SAND_IN  SOC_PPD_PORT                port_ndx,
  SOC_SAND_IN  SOC_PPD_MTR_ETH_TYPE        eth_type_ndx
  )
{


  LOG_CLI((BSL_META_U(unit,
                      "eth_type_ndx %s "), SOC_PPD_MTR_ETH_TYPE_to_string(eth_type_ndx)));

  return; 
}

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

