/* $Id: ppd_api_lif_ing_vlan_edit.c,v 1.12 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_lif_ing_vlan_edit.c
*
* MODULE PREFIX:  soc_ppd_lif
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
#include <soc/dpp/PPD/ppd_api_lif_ing_vlan_edit.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_lif_ing_vlan_edit.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_lif_ing_vlan_edit.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_lif_ing_vlan_edit.h>
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
    Ppd_procedure_desc_element_lif_ing_vlan_edit[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_ING_VLAN_EDIT_GET_PROCS_PTR),
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
*     Set the Ingress VLAN Edit command to perform over
 *     packets according to incoming VLAN tags format and
 *     required VLAN edit profile (set according to AC).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_command_id_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key,
    SOC_SAND_IN  uint32                                command_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(command_key);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_command_id_set,(unit, command_key, command_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_command_id_set_print,(unit,command_key,command_id));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_command_id_set()", 0, 0);
}

/*********************************************************************
*     Set the Ingress VLAN Edit command to perform over
 *     packets according to incoming VLAN tags format and
 *     required VLAN edit profile (set according to AC).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_command_id_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key,
    SOC_SAND_OUT uint32                                *command_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_ID_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(command_key);
  SOC_SAND_CHECK_NULL_INPUT(command_id);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_command_id_get,(unit, command_key, command_id));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_command_id_get_print,(unit,command_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_command_id_get()", 0, 0);
}

/*********************************************************************
*     Set the Edit command to perform over packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_command_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                command_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO      *command_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(command_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_command_info_set,(unit, command_ndx, command_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_command_info_set_print,(unit,command_ndx,command_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_command_info_set()", command_ndx, 0);
}

/*********************************************************************
*     Set the Edit command to perform over packets.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_command_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                command_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO      *command_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(command_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_command_info_get,(unit, command_ndx, command_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_command_info_get_print,(unit,command_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_command_info_get()", command_ndx, 0);
}

/*********************************************************************
*     Set mapping from outer S-tag PCP and DEI to the PCP and
 *     DEI values to be set in the transmitted packet's tag.
 *     This is the mapping to be used when the incoming packet
 *     is S-tagged (outer Tag).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_pcp_map_stag_set,(unit, pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx, out_pcp, out_dei));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set_print,(unit,pcp_profile_ndx,tag_pcp_ndx,tag_dei_ndx,out_pcp,out_dei));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set()", pcp_profile_ndx, tag_pcp_ndx);
}

/*********************************************************************
*     Set mapping from outer S-tag PCP and DEI to the PCP and
 *     DEI values to be set in the transmitted packet's tag.
 *     This is the mapping to be used when the incoming packet
 *     is S-tagged (outer Tag).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_STAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_pcp_map_stag_get,(unit, pcp_profile_ndx, tag_pcp_ndx, tag_dei_ndx, out_pcp, out_dei));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get_print,(unit,pcp_profile_ndx,tag_pcp_ndx,tag_dei_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get()", pcp_profile_ndx, tag_pcp_ndx);
}

/*********************************************************************
*     Set mapping from outer C-tag UP to the PCP and DEI
 *     values to be set in the transmitted packet's tag. This
 *     is the mapping to be used when the incoming packet is
 *     C-tagged (outer Tag.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_pcp_map_ctag_set,(unit, pcp_profile_ndx, tag_up_ndx, out_pcp, out_dei));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set_print,(unit,pcp_profile_ndx,tag_up_ndx,out_pcp,out_dei));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set()", pcp_profile_ndx, tag_up_ndx);
}

/*********************************************************************
*     Set mapping from outer C-tag UP to the PCP and DEI
 *     values to be set in the transmitted packet's tag. This
 *     is the mapping to be used when the incoming packet is
 *     C-tagged (outer Tag.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_CTAG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_pcp_map_ctag_get,(unit, pcp_profile_ndx, tag_up_ndx, out_pcp, out_dei));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get_print,(unit,pcp_profile_ndx,tag_up_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get()", pcp_profile_ndx, tag_up_ndx);
}

/*********************************************************************
*     Set mapping from COS parameters (DP and TC) to the PCP
 *     and DEI values to be set in the transmitted packet's
 *     tag. This is the mapping to be used when the incoming
 *     packet has no tags.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_pcp_map_untagged_set,(unit, pcp_profile_ndx, tc_ndx, dp_ndx, out_pcp, out_dei));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set_print,(unit,pcp_profile_ndx,tc_ndx,dp_ndx,out_pcp,out_dei));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set()", pcp_profile_ndx, tc_ndx);
}

/*********************************************************************
*     Set mapping from COS parameters (DP and TC) to the PCP
 *     and DEI values to be set in the transmitted packet's
 *     tag. This is the mapping to be used when the incoming
 *     packet has no tags.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_ING_VLAN_EDIT_PCP_MAP_UNTAGGED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_ing_vlan_edit_pcp_map_untagged_get,(unit, pcp_profile_ndx, tc_ndx, dp_ndx, out_pcp, out_dei));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get_print,(unit,pcp_profile_ndx,tc_ndx,dp_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get()", pcp_profile_ndx, tc_ndx);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_lif_ing_vlan_edit module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_lif_ing_vlan_edit_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_lif_ing_vlan_edit;
}
void
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(
    SOC_SAND_OUT SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_TAG_VID_SRC enum_val
  )
{
  return SOC_PPC_LIF_ING_VLAN_EDIT_TAG_VID_SRC_to_string(enum_val);
}

const char*
  SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC enum_val
  )
{
  return SOC_PPC_LIF_ING_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(enum_val);
}

void
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY_print(
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_lif_ing_vlan_edit_command_id_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key,
    SOC_SAND_IN  uint32                                command_id
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "command_key:")));
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY_print((command_key));

  LOG_CLI((BSL_META_U(unit,
                      "command_id: %lu\n\r"),command_id));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_command_id_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY       *command_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "command_key:")));
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_KEY_print((command_key));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_command_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                command_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO      *command_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "command_ndx: %lu\n\r"),command_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "command_info:")));
  SOC_PPD_LIF_ING_VLAN_EDIT_COMMAND_INFO_print((command_info));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_command_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                command_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "command_ndx: %lu\n\r"),command_ndx));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pcp_profile_ndx: %lu\n\r"),pcp_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tag_pcp_ndx: %u\n\r"), tag_pcp_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tag_dei_ndx: %u\n\r"),tag_dei_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "out_pcp: %u\n\r"), out_pcp));

  LOG_CLI((BSL_META_U(unit,
                      "out_dei: %u\n\r"),out_dei));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         tag_dei_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pcp_profile_ndx: %lu\n\r"),pcp_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tag_pcp_ndx: %u\n\r"), tag_pcp_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tag_dei_ndx: %u\n\r"),tag_dei_ndx));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pcp_profile_ndx: %lu\n\r"),pcp_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tag_up_ndx: %u\n\r"), tag_up_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "out_pcp: %u\n\r"), out_pcp));

  LOG_CLI((BSL_META_U(unit,
                      "out_dei: %u\n\r"),out_dei));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          tag_up_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pcp_profile_ndx: %lu\n\r"),pcp_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tag_up_ndx: %u\n\r"), tag_up_ndx));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pcp_profile_ndx: %lu\n\r"),pcp_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tc_ndx: %u\n\r"), tc_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "dp_ndx: %u\n\r"), dp_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "out_pcp: %u\n\r"), out_pcp));

  LOG_CLI((BSL_META_U(unit,
                      "out_dei: %u\n\r"),out_dei));

  return;
}
void
  soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pcp_profile_ndx: %lu\n\r"),pcp_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "tc_ndx: %u\n\r"), tc_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "dp_ndx: %u\n\r"), dp_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

