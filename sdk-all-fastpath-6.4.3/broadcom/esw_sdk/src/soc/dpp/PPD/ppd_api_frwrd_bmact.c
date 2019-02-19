/* $Id: ppd_api_frwrd_bmact.c,v 1.17 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_frwrd_bmact.c
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
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_bmact.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_frwrd_bmact.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_bmact.h>
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
    Ppd_procedure_desc_element_frwrd_bmact[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_BVID_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_BVID_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_BVID_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_BVID_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_BMACT_GET_PROCS_PTR),
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
*     Init device to support Mac in mac.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_bmact_init(
    SOC_SAND_IN  int                               unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_INIT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_init,(unit));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_init_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_init()", 0, 0);
}

/*********************************************************************
*     Set the B-VID bridging attributes. Backbone Mac
 *     addresses that do not serve as MyMAC for I-components
 *     that are processed according to their B-VID
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_bmact_bvid_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                  bvid_ndx,
    SOC_SAND_IN  SOC_PPD_BMACT_BVID_INFO                  *bvid_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_BVID_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bvid_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_bvid_info_set,(unit, bvid_ndx, bvid_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_bvid_info_set_print,(unit,bvid_ndx,bvid_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_bvid_info_set()", bvid_ndx, 0);
}

/*********************************************************************
*     Set the B-VID bridging attributes. Backbone Mac
 *     addresses that do not serve as MyMAC for I-components
 *     that are processed according to their B-VID
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_bmact_bvid_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                  bvid_ndx,
    SOC_SAND_OUT SOC_PPD_BMACT_BVID_INFO                  *bvid_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_BVID_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bvid_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_bvid_info_get,(unit, bvid_ndx, bvid_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_bvid_info_get_print,(unit,bvid_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_bvid_info_get()", bvid_ndx, 0);
}

/*********************************************************************
*     Set the BVID range for Traffic Engineered Provider
 *     Backbone Bridging
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_bmact_pbb_te_bvid_range_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_BMACT_PBB_TE_VID_RANGE           *pbb_te_bvids
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pbb_te_bvids);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_pbb_te_bvid_range_set,(unit, pbb_te_bvids));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_pbb_te_bvid_range_set_print,(unit,pbb_te_bvids));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_pbb_te_bvid_range_set()", 0, 0);
}

/*********************************************************************
*     Set the BVID range for Traffic Engineered Provider
 *     Backbone Bridging
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_bmact_pbb_te_bvid_range_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_BMACT_PBB_TE_VID_RANGE           *pbb_te_bvids
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pbb_te_bvids);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_pbb_te_bvid_range_get,(unit, pbb_te_bvids));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_pbb_te_bvid_range_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_pbb_te_bvid_range_get()", 0, 0);
}

/*********************************************************************
*     Add an entry to the B-MACT DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_bmact_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_KEY                  *bmac_key,
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_INFO                 *bmact_entry_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);
  SOC_SAND_CHECK_NULL_INPUT(bmact_entry_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_entry_add,(unit, bmac_key, bmact_entry_info, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_entry_add_print,(unit,bmac_key,bmact_entry_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_entry_add()", 0, 0);
}

/*********************************************************************
*     Remove an entry from the B-MACT DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_bmact_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_KEY                  *bmac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_entry_remove,(unit, bmac_key));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_entry_remove_print,(unit,bmac_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_entry_remove()", 0, 0);
}

/*********************************************************************
*     Get an entry from the B-MACT DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_bmact_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_KEY                  *bmac_key,
    SOC_SAND_OUT SOC_PPD_BMACT_ENTRY_INFO                 *bmact_entry_info,
    SOC_SAND_OUT uint8                                *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(bmac_key);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_entry_get,(unit, bmac_key, bmact_entry_info, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_entry_get_print,(unit,bmac_key,bmact_entry_info,found));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_entry_get()", 0, 0);
}

uint32
  soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set(
  SOC_SAND_IN  int                                      unit,
  SOC_SAND_IN  uint32                                      pcp_profile_ndx,
  SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
  SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
  SOC_SAND_IN SOC_SAND_PP_PCP_UP                           out_pcp,
  SOC_SAND_IN SOC_SAND_PP_DEI_CFI                          out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_eg_vlan_pcp_map_set,(unit, pcp_profile_ndx, tc_ndx, dp_ndx, out_pcp, out_dei));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set_print,(unit,pcp_profile_ndx, tc_ndx, dp_ndx, out_pcp, out_dei));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set()", 0, 0);
}

uint32
  soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get(
  SOC_SAND_IN  int                                      unit,
  SOC_SAND_IN  uint32                                      pcp_profile_ndx,
  SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
  SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
  SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
  SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(out_pcp);
  SOC_SAND_CHECK_NULL_INPUT(out_dei);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(frwrd_bmact_eg_vlan_pcp_map_get,(unit, pcp_profile_ndx, tc_ndx, dp_ndx, out_pcp, out_dei));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get_print,(unit,pcp_profile_ndx, tc_ndx, dp_ndx, out_pcp, out_dei));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_frwrd_bmact module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_bmact_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_frwrd_bmact;
}
void
  SOC_PPD_BMACT_BVID_INFO_clear(
    SOC_SAND_OUT SOC_PPD_BMACT_BVID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_BMACT_BVID_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BMACT_PBB_TE_VID_RANGE_clear(
    SOC_SAND_OUT SOC_PPD_BMACT_PBB_TE_VID_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_BMACT_PBB_TE_VID_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BMACT_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPD_BMACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_BMACT_ENTRY_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BMACT_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_BMACT_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_BMACT_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPD_DEBUG_IS_LVL1

void
  SOC_PPD_BMACT_BVID_INFO_print(
    SOC_SAND_IN  SOC_PPD_BMACT_BVID_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_BMACT_BVID_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BMACT_PBB_TE_VID_RANGE_print(
    SOC_SAND_IN  SOC_PPD_BMACT_PBB_TE_VID_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_BMACT_PBB_TE_VID_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BMACT_ENTRY_KEY_print(
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_BMACT_ENTRY_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_BMACT_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_BMACT_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_frwrd_bmact_init_print(
    SOC_SAND_IN  int                               unit
  )
{
  return;
}
void
  soc_ppd_frwrd_bmact_bvid_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         bvid_ndx,
    SOC_SAND_IN  SOC_PPD_BMACT_BVID_INFO                     *bvid_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "bvid_ndx: %u\n\r"),bvid_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "bvid_info:")));
  SOC_PPD_BMACT_BVID_INFO_print((bvid_info));

  return;
}
void
  soc_ppd_frwrd_bmact_bvid_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         bvid_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "bvid_ndx: %u\n\r"),bvid_ndx));

  return;
}
void
  soc_ppd_frwrd_bmact_pbb_te_bvid_range_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "pbb_te_bvids:")));
  SOC_PPD_BMACT_PBB_TE_VID_RANGE_print((pbb_te_bvids));

  return;
}
void
  soc_ppd_frwrd_bmact_pbb_te_bvid_range_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_bmact_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_KEY                     *bmac_key,
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_INFO                    *bmact_entry_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "bmac_key:")));
  SOC_PPD_BMACT_ENTRY_KEY_print((bmac_key));

  LOG_CLI((BSL_META_U(unit,
                      "bmact_entry_info:")));
  SOC_PPD_BMACT_ENTRY_INFO_print((bmact_entry_info));

  return;
}
void
  soc_ppd_frwrd_bmact_entry_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_KEY                     *bmac_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "bmac_key:")));
  SOC_PPD_BMACT_ENTRY_KEY_print((bmac_key));

  return;
}
void
soc_ppd_frwrd_bmact_entry_get_print(
	SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  SOC_PPD_BMACT_ENTRY_KEY                     *bmac_key
  )
{

	LOG_CLI((BSL_META_U(unit,
                            "bmac_key:")));
	SOC_PPD_BMACT_ENTRY_KEY_print((bmac_key));

	return;
}
void
  soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set_print(
  SOC_SAND_IN  int                               unit,
  SOC_SAND_IN  uint32                                pcp_profile_ndx,
  SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
  SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
  SOC_SAND_IN SOC_SAND_PP_PCP_UP                           out_pcp,
  SOC_SAND_IN SOC_SAND_PP_DEI_CFI                          out_dei
  )
{
  return;
}
void
soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN SOC_SAND_PP_PCP_UP                           *out_pcp,
    SOC_SAND_IN SOC_SAND_PP_DEI_CFI                          *out_dei
    )
{
    return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

