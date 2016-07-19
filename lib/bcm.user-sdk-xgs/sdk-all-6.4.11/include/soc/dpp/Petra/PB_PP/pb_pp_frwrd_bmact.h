/* $Id: pb_pp_frwrd_bmact.h,v 1.10 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_frwrd_bmact.h
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

#ifndef __SOC_PB_PP_FRWRD_BMACT_INCLUDED__
/* { */
#define __SOC_PB_PP_FRWRD_BMACT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_bmact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>

/* } */
/*************
 * DEFINES   *
 *************/ 
/* { */

#define SOC_PB_PP_LEM_ACCESS_ASD_IS_DYNAMIC					  (24)
#define SOC_PB_PP_LEM_ACCESS_ASD_IS_DYNAMIC_LEN				  (1)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_DROP					  (23)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_DROP_LEN				  (1)
#define SOC_PB_PP_LEM_ACCESS_ASD_LEARN_TYPE					  (22)
#define SOC_PB_PP_LEM_ACCESS_ASD_LEARN_TYPE_LEN				  (1)
#define SOC_PB_PP_LEM_ACCESS_ASD_I_SID_DOMAIN				  (16)
#define SOC_PB_PP_LEM_ACCESS_ASD_I_SID_DOMAIN_LEN			  (6)
#define SOC_PB_PP_LEM_ACCESS_ASD_LEARN_DESTINATION		      (0)
#define SOC_PB_PP_LEM_ACCESS_ASD_LEARN_DESTINATION_LEN		  (16)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_FRWRD_BMACT_FIRST,
  SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_BVID_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET,
  SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_BVID_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET,
  SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_PRINT,
  SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_SET_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET,
  SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_PRINT,
  SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_PBB_TE_BVID_RANGE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD_PRINT,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_ADD_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_GET,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_GET_PRINT,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_GET_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_ENTRY_GET_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET_VERIFY,
  SOC_PB_PP_FRWRD_BMACT_GET_PROCS_PTR,
  SOC_PB_PP_FRWRD_BMACT_GET_ERRS_PTR,
  SOC_PB_PP_FRWRD_BMACT_INIT,
  SOC_PB_PP_FRWRD_BMACT_INIT_UNSAFE,
  SOC_PB_PP_FRWRD_BMACT_INIT_PRINT,
  SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_SET,
  SOC_PB_PP_FRWRD_BMACT_EG_VLAN_PCP_MAP_GET,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_BMACT_PROCEDURE_DESC_LAST
} SOC_PB_PP_FRWRD_BMACT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_BMACT_SUCCESS_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FRWRD_BMACT_FIRST,
  SOC_PB_PP_FRWRD_BMACT_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_BMACT_B_FID_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_BMACT_SYS_PORT_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_BMACT_I_SID_DOMAIN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_BMACT_DA_NOT_FOUND_ACTION_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_BMACT_MAC_IN_MAC_CHECK_IF_ENABLED_ERR,
  SOC_PB_PP_FRWRD_BMACT_SA_AUTH_ENABLED_ERR,
  SOC_PB_PP_FRWRD_BMACT_PCP_PROFILE_NDX_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_BMACT_ERR_LAST
} SOC_PB_PP_FRWRD_BMACT_ERR;

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
* NAME:
*   soc_pb_pp_frwrd_bmact_init_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Init device to support Mac in mac. 
*   User cannot disable it once configured. 
*   Only reboot can reset the configurations.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
*   soc_pb_pp_is_mac_in_mac_enabled
* TYPE:
*   PROC
* FUNCTION:
*   Shows whether Mac in mac is enabled on the device. 
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_OUT uint8                                 *enabled -
*     Shows if Mac in mac is enabled or not.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_is_mac_in_mac_enabled(
  SOC_SAND_IN  int      unit,
  SOC_SAND_OUT  uint8     *enabled
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_bmact_mac_in_mac_enable
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Mac-in-Mac TPID profile. This also shows Mac-in_mac is
 *   enbled on the device.
 * INPUT:
 *   SOC_SAND_IN  int           unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   none.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_mac_in_mac_enable(
    SOC_SAND_IN  int           unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_bmact_bvid_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the B-VID bridging attributes. Backbone Mac
 *   addresses that do not serve as MyMAC for I-components
 *   that are processed according to their B-VID
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx -
 *     Backbone VID
 *   SOC_SAND_IN  SOC_PB_PP_BMACT_BVID_INFO                     *bvid_info -
 *     B-VID attributes
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_bvid_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_BMACT_BVID_INFO                     *bvid_info
  );

uint32
  soc_pb_pp_frwrd_bmact_bvid_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_BMACT_BVID_INFO                     *bvid_info
  );

uint32
  soc_pb_pp_frwrd_bmact_bvid_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_bmact_bvid_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_bmact_bvid_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_bvid_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx,
    SOC_SAND_OUT SOC_PB_PP_BMACT_BVID_INFO                     *bvid_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the BVID range for Traffic Engineered Provider
 *   Backbone Bridging
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids -
 *     Range of B-VIDs, to be used as PBB-TE services
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  );

uint32
  soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  );

uint32
  soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_pbb_te_bvid_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_bmact_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the B-MACT DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key -
 *     B-VID and B-MAC
 *   SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_INFO                    *bmact_entry_info -
 *     B-MACT entry attributes
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   Unless PBB-TE is expected to be called by the B-MAC
 *   learning process
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_INFO                    *bmact_entry_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_bmact_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_INFO                    *bmact_entry_info
  );

uint32
	soc_pb_pp_frwrd_bmact_key_parse(
	SOC_SAND_IN   int                                unit,
	SOC_SAND_IN   SOC_PB_PP_LEM_ACCESS_KEY                     *key,
	SOC_SAND_OUT  SOC_PB_PP_BMACT_ENTRY_KEY                    *bmac_key
  );

uint32
  soc_pb_pp_frwrd_bmact_payload_convert(
      SOC_SAND_IN  int                             unit,
      SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD              *payload,
      SOC_SAND_OUT SOC_PB_PP_BMACT_ENTRY_INFO                *bmact_entry_info
  );

/*********************************************************************
* NAME:   
 *   soc_pb_pp_frwrd_bmact_entry_remove_unsafe
 * TYPE:  
 *   PROC
 * FUNCTION:
 *   Remove an entry from the B-MACT DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key -
 *     B-VID and B-MAC
 * REMARKS:
 *   Unless PBB-TE is expected to be called by the B-MAC
 *   learning process
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key
  );

uint32
  soc_pb_pp_frwrd_bmact_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_bmact_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the B-MACT DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key -
 *     B-VID and B-MAC
 *   SOC_SAND_OUT  SOC_PB_PP_BMACT_ENTRY_INFO                   *bmact_entry_info -
 *     B-MACT entry attributes
 *   SOC_SAND_OUT  uint8                                *found -
 *     FALSE: the entry was not found
 * REMARKS:
 *   Unless PBB-TE is expected to be called by the B-MAC
 *   learning process
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_bmact_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key,
	SOC_SAND_OUT SOC_PB_PP_BMACT_ENTRY_INFO                    *bmact_entry_info,
	SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_frwrd_bmact_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY                     *bmac_key
  );

uint32
  soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx, /* ?? */
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );

uint32
  soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
    );

uint32
  soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );

uint32
  soc_pb_pp_frwrd_bmact_eg_vlan_pcp_map_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
    );

uint32
  soc_pb_pp_frwrd_bmact_default_sem_index_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ac_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_bmact_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_frwrd_bmact module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_bmact_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_bmact_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_frwrd_bmact module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_bmact_get_errs_ptr(void);

uint32
  SOC_PB_PP_BMACT_BVID_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_BMACT_BVID_INFO *info
  );

uint32
  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_BMACT_PBB_TE_VID_RANGE *info
  );

uint32
  SOC_PB_PP_BMACT_ENTRY_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_KEY *info
  );

uint32
  SOC_PB_PP_BMACT_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_BMACT_ENTRY_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FRWRD_BMACT_INCLUDED__*/
#endif

