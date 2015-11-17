/* $Id: ppd_api_vsi.c,v 1.16 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_vsi.c
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
#include <soc/dpp/PPD/ppd_api_vsi.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_vsi.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_vsi.h>
#endif
#ifdef LINK_PCP_LIBRARIES
  #include <soc/dpp/PCP/pcp_api_frwrd_mact_mgmt.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_vsi.h>
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
    Ppd_procedure_desc_element_vsi[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_MAP_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_MAP_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_MAP_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_MAP_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_DEFAULT_FRWRD_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_DEFAULT_FRWRD_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_DEFAULT_FRWRD_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_DEFAULT_FRWRD_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_VSI_GET_PROCS_PTR),
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
*     Map VSI to sys-VSI and system VSI to egress VSI
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_map_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              eg_local_vsid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_VSI_MAP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
    case SOC_SAND_DEV_T20E:
    case SOC_SAND_DEV_PB:
      /*
       *	Not supported for Soc_petra-B/T20E
       */
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 12, exit);
      break;
  }

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_map_add_print,(unit,local_vsi_ndx,sys_vsid,eg_local_vsid));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_map_add()", local_vsi_ndx, 0);
}

/*********************************************************************
*     Remove mapping of local VSI to system VSI, and system
 *     VSI to local VSI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_map_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_VSI_MAP_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  switch (SOC_SAND_DEVICE_TYPE_GET(unit))
  {
    case SOC_SAND_DEV_T20E:
    case SOC_SAND_DEV_PB:
      /*
       *	Not supported for Soc_petra-B/T20E
       */
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_FUNC_UNSUPPORTED_ERR, 10, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_INVALID_DEVICE_TYPE_ERR, 12, exit);
      break;
  }

exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_map_remove_print,(unit,local_vsi_ndx,sys_vsid));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_map_remove()", local_vsi_ndx, 0);
}

/*********************************************************************
*     Set the action profile (forwarding/snooping) to assign
 *     for packets upon failure lookup in the MACT (MACT
 *     default forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_default_frwrd_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_VSI_DEFAULT_FRWRD_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dflt_frwrd_key);
  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_PPD_DEVICE_CALL(vsi_default_frwrd_info_set,(unit, dflt_frwrd_key, action_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_default_frwrd_info_set_print,(unit,dflt_frwrd_key,action_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_default_frwrd_info_set()", 0, 0);
}

/*********************************************************************
*     Set the action profile (forwarding/snooping) to assign
 *     for packets upon failure lookup in the MACT (MACT
 *     default forwarding).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_default_frwrd_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_OUT SOC_PPD_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_VSI_DEFAULT_FRWRD_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(dflt_frwrd_key);
  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_PPD_DEVICE_CALL(vsi_default_frwrd_info_get,(unit, dflt_frwrd_key, action_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_default_frwrd_info_get_print,(unit,dflt_frwrd_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_default_frwrd_info_get()", 0, 0);
}

/*********************************************************************
*     Set the Virtual Switch Instance information. After
 *     setting the VSI, the user may attach L2 Logical
 *     Interfaces to it: ACs; PWEs
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PPD_VSI_INFO                            *vsi_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_VSI_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(vsi_info_set,(unit, vsi_ndx, vsi_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_info_set_print,(unit,vsi_ndx,vsi_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_info_set()", vsi_ndx, 0);
}

/*********************************************************************
*     Set the Virtual Switch Instance information. After
 *     setting the VSI, the user may attach L2 Logical
 *     Interfaces to it: ACs; PWEs
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_PPD_VSI_INFO                            *vsi_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_VSI_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(vsi_info_get,(unit, vsi_ndx, vsi_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_info_get_print,(unit,vsi_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_info_get()", vsi_ndx, 0);
}

/*********************************************************************
*     Set egress vsi profile info to profile index.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 soc_ppd_vsi_egress_mtu_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_VSI_EGRESS_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(vsi_egress_mtu_set,(unit, FALSE /*LinkLayer*/, vsi_profile_ndx, mtu_val));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_egress_profile_set_print,(unit,vsi_profile_ndx,mtu_val));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_egress_profile_set()", vsi_profile_ndx, 0);
}

/*********************************************************************
 *     Get egress vsi profile info from profile index.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_egress_mtu_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_OUT  uint32                              *mtu_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_VSI_EGRESS_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mtu_val);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(vsi_egress_mtu_get,(unit, FALSE /*LinkLayer*/, vsi_profile_ndx, mtu_val));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_egress_profile_get_print,(unit,vsi_profile_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_egress_profile_get()", vsi_profile_ndx, 0);
}

/*********************************************************************
 *     Set egress vsi l2cp trap.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_l2cp_trap_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE               handle_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(vsi_l2cp_trap_set,(unit, l2cp_key, handle_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_l2cp_trap_set_print,(unit,l2cp_key,handle_type));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_l2cp_trap_set()", 0, 0);
}


/*********************************************************************
 *     Get egress vsi l2cp trap.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_vsi_l2cp_trap_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_OUT  SOC_PPD_VSI_L2CP_HANDLE_TYPE               *handle_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(vsi_l2cp_trap_get,(unit, l2cp_key, handle_type));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_vsi_l2cp_trap_get_print,(unit,l2cp_key,handle_type));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_vsi_l2cp_trap_get()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_vsi module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_vsi_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_vsi;
}

void
  SOC_PPD_VSI_L2CP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_VSI_L2CP_KEY *key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(key);

  SOC_PPC_VSI_L2CP_KEY_clear(key);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_VSI_INFO_clear(
    SOC_SAND_OUT SOC_PPD_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_VSI_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY_clear(
    SOC_SAND_OUT SOC_PPD_VSI_DEFAULT_FRWRD_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_VSI_DEFAULT_FRWRD_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

void
  SOC_PPD_VSI_INFO_print(
    SOC_SAND_IN  SOC_PPD_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_VSI_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY_print(
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_VSI_DEFAULT_FRWRD_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_vsi_map_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              eg_local_vsid
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_vsi_ndx: %lu\n\r"),local_vsi_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "sys_vsid: %lu\n\r"),sys_vsid));

  LOG_CLI((BSL_META_U(unit,
                      "eg_local_vsid: %lu\n\r"),eg_local_vsid));

  return;
}
void
  soc_ppd_vsi_map_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              local_vsi_ndx,
    SOC_SAND_IN  SOC_PPD_SYS_VSI_ID                          sys_vsid
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_vsi_ndx: %lu\n\r"),local_vsi_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "sys_vsid: %lu\n\r"),sys_vsid));

  return;
}
void
  soc_ppd_vsi_default_frwrd_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "dflt_frwrd_key:")));
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY_print((dflt_frwrd_key));

  LOG_CLI((BSL_META_U(unit,
                      "action_profile:")));
  SOC_PPD_ACTION_PROFILE_print((action_profile));

  return;
}
void
  soc_ppd_vsi_default_frwrd_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_DEFAULT_FRWRD_KEY               *dflt_frwrd_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "dflt_frwrd_key:")));
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY_print((dflt_frwrd_key));

  return;
}
void
  soc_ppd_vsi_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PPD_VSI_INFO                            *vsi_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsi_ndx: %lu\n\r"),vsi_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "vsi_info:")));
  SOC_PPD_VSI_INFO_print((vsi_info));

  return;
}
void
  soc_ppd_vsi_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VSI_ID                              vsi_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vsi_ndx: %lu\n\r"),vsi_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

