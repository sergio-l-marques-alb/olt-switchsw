/* $Id: ppd_api_frwrd_mact_mgmt.c,v 1.20 Broadcom SDK $
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
 * $
*/
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_frwrd_mact_mgmt.c
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
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_mact_mgmt.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_frwrd_mact_mgmt.h>
#endif
#ifdef LINK_PCP_LIBRARIES
  #include <soc/dpp/PCP/pcp_api_frwrd_mact_mgmt.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_mact_mgmt.h>
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
    Ppd_procedure_desc_element_frwrd_mact_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_EVENT_PARSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_MGMT_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_LOOKUP_TYPE_SET),

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
 *   Configures the SA lookup type. Also configured 'opportunistic learning',
 *   which is affected by whether SA authentication is enabled.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_lookup_type_set(      
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LOOKUP_TYPE         lookup_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_LOOKUP_TYPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_lookup_type_set,(unit, lookup_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_lookup_type_set,(unit,lookup_type));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_lookup_type_set()", 0, 0);
}


/*********************************************************************
*     Sets the mode of the MACT, including - ingress vs.
 *     egress learning- how each device responds internally to
 *     events (learn/aged-out/refresh) - which events to inform
 *     other devices.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_oper_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_oper_mode_info_set,(unit, oper_mode_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_oper_mode_info_set_print,(unit,oper_mode_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_oper_mode_info_set()", 0, 0);
}

/*********************************************************************
*     Sets the mode of the MACT, including - ingress vs.
 *     egress learning- how each device responds internally to
 *     events (learn/aged-out/refresh) - which events to inform
 *     other devices.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_oper_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_oper_mode_info_get,(unit, oper_mode_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_oper_mode_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_oper_mode_info_get()", 0, 0);
}

/*********************************************************************
*     Sets the aging info including enable aging and aging
 *     time.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_AGING_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_aging_info_set,(unit, aging_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_aging_info_set_print,(unit,aging_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_info_set()", 0, 0);
}

/*********************************************************************
*     Sets the aging info including enable aging and aging
 *     time.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_AGING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_aging_info_get,(unit, aging_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_aging_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_info_get()", 0, 0);
}


/*********************************************************************
*     Sets the aging info including enable aging and aging
 *     time.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_one_pass_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO   *pass_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pass_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_aging_one_pass_set,(unit, pass_info,success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_one_pass_set()", 0, 0);
}


/*********************************************************************
*     Sets the device action upon events invoked by the aging
 *     process: - Whether the device deletes aged-out entries
 *     internally - Whether the device generates an event for
 *     aged-out entries - Whether the device generates an event
 *     for refreshed entries
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_events_handle_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_aging_events_handle_info_set,(unit, aging_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_aging_events_handle_info_set_print,(unit,aging_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_events_handle_info_set()", 0, 0);
}

/*********************************************************************
*     Sets the device action upon events invoked by the aging
 *     process: - Whether the device deletes aged-out entries
 *     internally - Whether the device generates an event for
 *     aged-out entries - Whether the device generates an event
 *     for refreshed entries
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_events_handle_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_aging_events_handle_info_get,(unit, aging_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_aging_events_handle_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_events_handle_info_get()", 0, 0);
}

/*********************************************************************
*     Maps FID-Profile to FID, for shared learning.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                fid_profile_ndx,
    SOC_SAND_IN  SOC_PPD_FID                                 fid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_fid_profile_to_fid_map_set,(unit, fid_profile_ndx, fid));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_fid_profile_to_fid_map_set_print,(unit,fid_profile_ndx,fid));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_fid_profile_to_fid_map_set()", fid_profile_ndx, 0);
}

/*********************************************************************
*     Maps FID-Profile to FID, for shared learning.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                fid_profile_ndx,
    SOC_SAND_OUT SOC_PPD_FID                                 *fid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fid);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_fid_profile_to_fid_map_get,(unit, fid_profile_ndx, fid));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_fid_profile_to_fid_map_get_print,(unit,fid_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_fid_profile_to_fid_map_get()", fid_profile_ndx, 0);
}

/*********************************************************************
*     Enable the MAC limit feature, which limits per fid the
 *     maximum number of entries allowed to be in the MAC
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mac_limit_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_mac_limit_glbl_info_set,(unit, limit_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_mac_limit_glbl_info_set_print,(unit,limit_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mac_limit_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Enable the MAC limit feature, which limits per fid the
 *     maximum number of entries allowed to be in the MAC
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mac_limit_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_mac_limit_glbl_info_get,(unit, limit_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_mac_limit_glbl_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mac_limit_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Set the limit information including the MAC-limit (i.e.,
 *     the maximum number of entries per tunnel can hold in the MAC
 *     Table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_tunnel_mac_limit_info_set(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  uint16                                            port,
    SOC_SAND_IN  uint16                                            tunnel_id,
    SOC_SAND_IN  int                                               mac_limit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_tunnel_mac_limit_info_set,(unit, port, tunnel_id, mac_limit));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_tunnel_mac_limit_info_set()", 0, 0);
}

/*********************************************************************
*     Get the limit information including the MAC-limit (i.e.,
 *     the maximum number of entries per tunnel can hold in the MAC
 *     Table)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_tunnel_mac_limit_info_get(
    SOC_SAND_IN  int                                            unit,
    SOC_SAND_IN  uint16                                            port,
    SOC_SAND_IN  uint16                                            tunnel_id,
    SOC_SAND_OUT  int                                              *mac_limit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_CHECK_NULL_INPUT(mac_limit);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_tunnel_mac_limit_info_get,(unit, port, tunnel_id, mac_limit));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_tunnel_mac_limit_info_get()", 0, 0);
}


/*********************************************************************
*     Set the limit information including the MAC-limit (i.e.,
 *     the maximum number of entries an FID can hold in the MAC
 *     Table), and the notification action if the configured
 *     limit is exceeded.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_learn_profile_limit_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_learn_profile_limit_info_set,(unit, mac_learn_profile_ndx, limit_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_learn_profile_limit_info_set_print,(unit,mac_learn_profile_ndx,limit_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_profile_limit_info_set()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Set the limit information including the MAC-limit (i.e.,
 *     the maximum number of entries an FID can hold in the MAC
 *     Table), and the notification action if the configured
 *     limit is exceeded.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_learn_profile_limit_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_learn_profile_limit_info_get,(unit, mac_learn_profile_ndx, limit_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_learn_profile_limit_info_get_print,(unit,mac_learn_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_profile_limit_info_get()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Gets the information if the MAC limitation is exceeded,
 *     i.e. when a MAC Table entry is tryied to be inserted and
 *     exceeds the limitation set per FID. This insertion can
 *     be triggered by CPU or after a packet learning.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mac_limit_exceeded_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *exceed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exceed_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_mac_limit_exceeded_info_get,(unit, exceed_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_mac_limit_exceeded_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mac_limit_exceeded_info_get()", 0, 0);
}

/*********************************************************************
*     Map the mac-learn-profile to the event-handle profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_handle_profile_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_event_handle_profile_set,(unit, mac_learn_profile_ndx, event_handle_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_event_handle_profile_set_print,(unit,mac_learn_profile_ndx,event_handle_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_handle_profile_set()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Map the mac-learn-profile to the event-handle profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_handle_profile_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                *event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_handle_profile);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_event_handle_profile_get,(unit, mac_learn_profile_ndx, event_handle_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_event_handle_profile_get_print,(unit,mac_learn_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_handle_profile_get()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Set how to handle an event according to the event key
 *     parameters (event-type,vsi-handle-profile,is-lag)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_handle_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_event_handle_info_set,(unit, event_key, handle_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_event_handle_info_set_print,(unit,event_key,handle_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_handle_info_set()", 0, 0);
}

/*********************************************************************
*     Set how to handle an event according to the event key
 *     parameters (event-type,vsi-handle-profile,is-lag)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_handle_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_event_handle_info_get,(unit, event_key, handle_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_event_handle_info_get_print,(unit,event_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_handle_info_get()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the learn messages to other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_learn_msgs_distribution_info_set,(unit, distribution_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_learn_msgs_distribution_info_set_print,(unit,distribution_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_msgs_distribution_info_set()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the learn messages to other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_learn_msgs_distribution_info_get,(unit, distribution_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_learn_msgs_distribution_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_msgs_distribution_info_get()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the shadow messages to the other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_shadow_msgs_distribution_info_set,(unit, distribution_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set_print,(unit,distribution_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the shadow messages to the other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_shadow_msgs_distribution_info_get,(unit, distribution_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get()", 0, 0);
}

/*********************************************************************
*     Map the mac-learn-profile to the aging profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_fid_aging_profile_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     mac_learn_profile_ndx,
    SOC_SAND_IN  uint32     fid_aging_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_fid_aging_profile_set,(unit, mac_learn_profile_ndx, fid_aging_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_fid_aging_profile_set_print,(unit,mac_learn_profile_ndx,fid_aging_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_fid_aging_profile_set()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Map the mac-learn-profile to the fid aging profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_fid_aging_profile_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       mac_learn_profile_ndx,
    SOC_SAND_OUT uint32      *fid_aging_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fid_aging_profile);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_fid_aging_profile_get,(unit, mac_learn_profile_ndx, fid_aging_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_fid_aging_profile_get_print,(unit,mac_learn_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_fid_aging_profile_get()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Map the mac-learn-profile to the fid aging profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_profile_config(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       fid_aging_profile,
    SOC_SAND_OUT uint32       fid_aging_cycles
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_aging_profile_config,(unit, fid_aging_profile, fid_aging_cycles));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_aging_profile_config()", fid_aging_profile, 0);
}

/*********************************************************************
*     Set per port MACT management information including which
 *     profile to activate when SA is known in this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_port_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_port_info_set,(unit, core_id, local_port_ndx, port_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_port_info_set_print,(unit,local_port_ndx,port_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_port_info_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Set per port MACT management information including which
 *     profile to activate when SA is known in this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_port_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_port_info_get,(unit, core_id, local_port_ndx, port_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_port_info_get_print,(unit,local_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_port_info_get()", local_port_ndx, 0);
}

/*********************************************************************
*     For each trap type, set the action profile. Different
 *     actions may be assigned to the same trap type according
 *     to the port-profile (4 possibilities).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_trap_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_TRAP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_trap_info_set,(unit, trap_type_ndx, port_profile_ndx, action_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_trap_info_set_print,(unit,trap_type_ndx,port_profile_ndx,action_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_trap_info_set()", 0, port_profile_ndx);
}

/*********************************************************************
*     For each trap type, set the action profile. Different
 *     actions may be assigned to the same trap type according
 *     to the port-profile (4 possibilities).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_trap_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_OUT SOC_PPD_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_TRAP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_trap_info_get,(unit, trap_type_ndx, port_profile_ndx, action_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_trap_info_get_print,(unit,trap_type_ndx,port_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_trap_info_get()", 0, port_profile_ndx);
}

/*********************************************************************
*     Sets the information for bridging compatible Multicast
 *     MAC addresses.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_ip_compatible_mc_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_ip_compatible_mc_info_set,(unit, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_ip_compatible_mc_info_set_print,(unit,info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_ip_compatible_mc_info_set()", 0, 0);
}

/*********************************************************************
*     Sets the information for bridging compatible Multicast
 *     MAC addresses.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_ip_compatible_mc_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_ip_compatible_mc_info_get,(unit, info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_ip_compatible_mc_info_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_ip_compatible_mc_info_get()", 0, 0);
}

/*********************************************************************
*     Read MACT event from the events FIFO into buffer.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_BUFFER            *event_buf
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_EVENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_buf);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_event_get,(unit, event_buf));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_event_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_get()", 0, 0);
}



/*********************************************************************
*     Sets whether mim init function has been called
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mim_init_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN   uint8                              mim_initialized
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_MIM_INIT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_mim_init_set,(unit, mim_initialized));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mim_init_set()", 0, 0);
}


/*********************************************************************
*     Gets whether mim init function has been called
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mim_init_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT uint8                              *mim_initialized
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_MIM_INIT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mim_initialized);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_mim_init_get,(unit, mim_initialized));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_mim_init_get()", 0, 0);
}


/*********************************************************************
*     Sets learning for routed packets according to appFlags
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_routed_learning_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32                appFlags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_ROUTED_LEARNING_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_routed_learning_set,(unit, appFlags));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_routed_learning_set()", 0, 0);
}

/*********************************************************************
*     Gets learning for routed packets
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_routed_learning_get(
    SOC_SAND_IN   int                unit, 
    SOC_SAND_OUT  uint32                *appFlags
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_ROUTED_LEARNING_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_routed_learning_get,(unit, appFlags));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_routed_learning_get()", 0, 0);
}

/*********************************************************************
*     The MACT may report different events using the event
 *     FIFO (e.g., learn, age, transplant, and retrieve). This
 *     API Parses the event buffer into a meaningful structure.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_parse(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_INFO              *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_EVENT_PARSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_buf);
  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_event_parse,(unit, event_buf, mact_event));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_event_parse_print,(unit,event_buf));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_event_parse()", 0, 0);
}

#if (defined(LINK_PB_LIBRARIES) || defined(LINK_ARAD_LIBRARIES))
uint32
  soc_ppd_frwrd_mact_learn_msg_parse(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   SOC_PPD_FRWRD_MACT_LEARN_MSG                 *learn_msg,
    SOC_SAND_OUT  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO      *learn_events
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF 
    learn_msg_conf;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE);

  if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_PB) {
#ifdef LINK_PB_LIBRARIES
      res = soc_pb_pp_frwrd_mact_learn_msg_conf_get(
             unit,
             learn_msg,
             &learn_msg_conf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        
      res = soc_pb_pp_frwrd_mact_learn_msg_parse(
              unit,
              learn_msg,
              &learn_msg_conf,
              learn_events
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
#endif
  }
  else if(SOC_SAND_DEVICE_TYPE_GET(unit) == SOC_SAND_DEV_ARAD) {
#ifdef LINK_ARAD_LIBRARIES
      res = arad_pp_frwrd_mact_learn_msg_conf_get(
             unit,
             learn_msg,
             &learn_msg_conf
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        
      res = arad_pp_frwrd_mact_learn_msg_parse(
              unit,
              learn_msg,
              &learn_msg_conf,
              learn_events
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
#endif
  }
  else{
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 999, exit);
  }

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_learn_msg_parse_print,(unit,learn_msg));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_msg_parse()", 0, 0);
}
#endif


#ifdef BCM_88660_A0
/********************************************************************* 
 *     Map a value to the common MACT limit table.
 *     An index to the the common limit table is returned.
 *     Applicable only for Arad+ and above.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mapped_val,
    SOC_SAND_OUT uint32                                 *limit_tbl_idx,
    SOC_SAND_OUT uint32                                 *is_reserved
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_tbl_idx);
  SOC_SAND_CHECK_NULL_INPUT(is_reserved);

  if (!(SOC_IS_ARADPLUS(unit))) {
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 10, exit);
  }

  ARAD_PP_DEVICE_CALL(frwrd_mact_limit_mapped_val_to_table_index_get, (unit, mapped_val, limit_tbl_idx, is_reserved));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get_print,(unit, mapped_val));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get()", 0, 0);
}
#endif


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_frwrd_mact_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_mact_mgmt_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_frwrd_mact_mgmt;
}
void
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_AGING_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_OPER_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_BUFFER_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_LEARN_MSG *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_LEARN_MSG_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_FRWRD_MACT_LEARNING_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARNING_MODE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_LEARNING_MODE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_SHADOW_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_SHADOW_MODE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_SHADOW_MODE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_TRAP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_TRAP_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_EVENT_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_EVENT_PATH_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_PATH_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_to_string(enum_val);
}

void
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_AGING_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_EVENT_BUFFER_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARN_MSG *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_LEARN_MSG_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_frwrd_mact_oper_mode_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "oper_mode_info:")));
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_print((oper_mode_info));

  return;
}
void
  soc_ppd_frwrd_mact_oper_mode_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_aging_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_INFO               *aging_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "aging_info:")));
  SOC_PPD_FRWRD_MACT_AGING_INFO_print((aging_info));

  return;
}
void
  soc_ppd_frwrd_mact_aging_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_aging_events_handle_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "aging_info:")));
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_print((aging_info));

  return;
}
void
  soc_ppd_frwrd_mact_aging_events_handle_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                fid_profile_ndx,
    SOC_SAND_IN  SOC_PPD_FID                                 fid
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fid_profile_ndx: %lu\n\r"),fid_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "fid: %lu\n\r"),fid));

  return;
}
void
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                fid_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "fid_profile_ndx: %lu\n\r"),fid_profile_ndx));

  return;
}
void
  soc_ppd_frwrd_mact_mac_limit_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "limit_info:")));
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print((limit_info));

  return;
}
void
  soc_ppd_frwrd_mact_mac_limit_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_learn_profile_limit_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_learn_profile_ndx: %lu\n\r"),mac_learn_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "limit_info:")));
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO_print((limit_info));

  return;
}
void
  soc_ppd_frwrd_mact_learn_profile_limit_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_learn_profile_ndx: %lu\n\r"),mac_learn_profile_ndx));

  return;
}
void
  soc_ppd_frwrd_mact_mac_limit_exceeded_info_get_print(
    SOC_SAND_IN  int                              unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_event_handle_profile_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                event_handle_profile
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_learn_profile_ndx: %lu\n\r"),mac_learn_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "event_handle_profile: %lu\n\r"),event_handle_profile));

  return;
}
void
  soc_ppd_frwrd_mact_event_handle_profile_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_learn_profile_ndx: %lu\n\r"),mac_learn_profile_ndx));

  return;
}
void
  soc_ppd_frwrd_mact_event_handle_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "event_key:")));
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY_print((event_key));

  LOG_CLI((BSL_META_U(unit,
                      "handle_info:")));
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_print((handle_info));

  return;
}
void
  soc_ppd_frwrd_mact_event_handle_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "event_key:")));
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY_print((event_key));

  return;
}
void
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "distribution_info:")));
  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO_print((distribution_info));

  return;
}
void
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "distribution_info:")));
  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO_print((distribution_info));

  return;
}
void
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_fid_aging_profile_set_print(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     mac_learn_profile_ndx,
    SOC_SAND_IN  uint32     fid_aging_profile
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_learn_profile_ndx: %lu\n\r"),mac_learn_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "fid_aging_profile: %lu\n\r"),fid_aging_profile));

  return;
}

void
  soc_ppd_frwrd_mact_fid_aging_profile_get_print(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     mac_learn_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_learn_profile_ndx: %lu\n\r"),mac_learn_profile_ndx));

  return;
}

void
  soc_ppd_frwrd_mact_port_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_PORT_INFO                *port_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "port_info:")));
  SOC_PPD_FRWRD_MACT_PORT_INFO_print((port_info));

  return;
}
void
  soc_ppd_frwrd_mact_port_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  return;
}
void
  soc_ppd_frwrd_mact_trap_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_type_ndx %s "), SOC_PPD_FRWRD_MACT_TRAP_TYPE_to_string(trap_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %lu\n\r"),port_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "action_profile:")));
  SOC_PPD_ACTION_PROFILE_print((action_profile));

  return;
}
void
  soc_ppd_frwrd_mact_trap_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                port_profile_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "trap_type_ndx %s "), SOC_PPD_FRWRD_MACT_TRAP_TYPE_to_string(trap_type_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %lu\n\r"),port_profile_ndx));

  return;
}
void
  soc_ppd_frwrd_mact_ip_compatible_mc_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "info:")));
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print((info));

  return;
}
void
  soc_ppd_frwrd_mact_ip_compatible_mc_info_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_event_get_print(
    SOC_SAND_IN  int                              unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_event_parse_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_BUFFER            *event_buf
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "event_buf:")));
  SOC_PPD_FRWRD_MACT_EVENT_BUFFER_print((event_buf));

  return;
}

void
  soc_ppd_frwrd_mact_learn_msg_parse_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARN_MSG            *learn_msg
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "event_buf:")));
  SOC_PPD_FRWRD_MACT_LEARN_MSG_print((learn_msg));

  return;
}

#ifdef BCM_88660_A0
void
  soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mapped_val
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif: %u\n\r"), lif));
  LOG_CLI((BSL_META_U(unit,
                      "limit_tbl_ix: %u\n\r"), *limit_tbl_ix));

  return;
}
#endif


#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

