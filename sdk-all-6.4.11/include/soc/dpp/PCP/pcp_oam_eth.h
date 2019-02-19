/* $Id: pcp_oam_eth.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PCP_OAM_ETH_INCLUDED__
/* { */
#define __SOC_PCP_OAM_ETH_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PCP/pcp_oam_api_eth.h>
#include <soc/dpp/PCP/pcp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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
  PCP_OAM_ETH_GENERAL_INFO_SET = PCP_PROC_DESC_BASE_OAM_ETH_FIRST,
  PCP_OAM_ETH_GENERAL_INFO_SET_PRINT,
  PCP_OAM_ETH_GENERAL_INFO_SET_UNSAFE,
  PCP_OAM_ETH_GENERAL_INFO_SET_VERIFY,
  PCP_OAM_ETH_GENERAL_INFO_GET,
  PCP_OAM_ETH_GENERAL_INFO_GET_PRINT,
  PCP_OAM_ETH_GENERAL_INFO_GET_VERIFY,
  PCP_OAM_ETH_GENERAL_INFO_GET_UNSAFE,
  PCP_OAM_ETH_COS_MAPPING_INFO_SET,
  PCP_OAM_ETH_COS_MAPPING_INFO_SET_PRINT,
  PCP_OAM_ETH_COS_MAPPING_INFO_SET_UNSAFE,
  PCP_OAM_ETH_COS_MAPPING_INFO_SET_VERIFY,
  PCP_OAM_ETH_COS_MAPPING_INFO_GET,
  PCP_OAM_ETH_COS_MAPPING_INFO_GET_PRINT,
  PCP_OAM_ETH_COS_MAPPING_INFO_GET_VERIFY,
  PCP_OAM_ETH_COS_MAPPING_INFO_GET_UNSAFE,
  PCP_OAM_MP_INFO_SET,
  PCP_OAM_MP_INFO_SET_PRINT,
  PCP_OAM_MP_INFO_SET_UNSAFE,
  PCP_OAM_MP_INFO_SET_VERIFY,
  PCP_OAM_MP_INFO_GET,
  PCP_OAM_MP_INFO_GET_PRINT,
  PCP_OAM_MP_INFO_GET_VERIFY,
  PCP_OAM_MP_INFO_GET_UNSAFE,
  PCP_OAM_ETH_ACC_MEP_INFO_SET,
  PCP_OAM_ETH_ACC_MEP_INFO_SET_PRINT,
  PCP_OAM_ETH_ACC_MEP_INFO_SET_UNSAFE,
  PCP_OAM_ETH_ACC_MEP_INFO_SET_VERIFY,
  PCP_OAM_ETH_ACC_MEP_INFO_GET,
  PCP_OAM_ETH_ACC_MEP_INFO_GET_PRINT,
  PCP_OAM_ETH_ACC_MEP_INFO_GET_VERIFY,
  PCP_OAM_ETH_ACC_MEP_INFO_GET_UNSAFE,
  PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET,
  PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_PRINT,
  PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_UNSAFE,
  PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_VERIFY,
  PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET,
  PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_PRINT,
  PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_VERIFY,
  PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_UNSAFE,
  PCP_OAM_ETH_ACC_LMM_SET,
  PCP_OAM_ETH_ACC_LMM_SET_PRINT,
  PCP_OAM_ETH_ACC_LMM_SET_UNSAFE,
  PCP_OAM_ETH_ACC_LMM_SET_VERIFY,
  PCP_OAM_ETH_ACC_LMM_GET,
  PCP_OAM_ETH_ACC_LMM_GET_PRINT,
  PCP_OAM_ETH_ACC_LMM_GET_VERIFY,
  PCP_OAM_ETH_ACC_LMM_GET_UNSAFE,
  PCP_OAM_ETH_ACC_DLM_SET,
  PCP_OAM_ETH_ACC_DLM_SET_PRINT,
  PCP_OAM_ETH_ACC_DLM_SET_UNSAFE,
  PCP_OAM_ETH_ACC_DLM_SET_VERIFY,
  PCP_OAM_ETH_ACC_DLM_GET,
  PCP_OAM_ETH_ACC_DLM_GET_PRINT,
  PCP_OAM_ETH_ACC_DLM_GET_VERIFY,
  PCP_OAM_ETH_ACC_DLM_GET_UNSAFE,
  PCP_OAM_ETH_RMEP_ADD,
  PCP_OAM_ETH_RMEP_ADD_PRINT,
  PCP_OAM_ETH_RMEP_ADD_UNSAFE,
  PCP_OAM_ETH_RMEP_ADD_VERIFY,
  PCP_OAM_ETH_RMEP_REMOVE,
  PCP_OAM_ETH_RMEP_REMOVE_PRINT,
  PCP_OAM_ETH_RMEP_REMOVE_UNSAFE,
  PCP_OAM_ETH_RMEP_REMOVE_VERIFY,
  PCP_OAM_ETH_RMEP_INFO_SET,
  PCP_OAM_ETH_RMEP_INFO_SET_PRINT,
  PCP_OAM_ETH_RMEP_INFO_SET_UNSAFE,
  PCP_OAM_ETH_RMEP_INFO_SET_VERIFY,
  PCP_OAM_ETH_RMEP_INFO_GET,
  PCP_OAM_ETH_RMEP_INFO_GET_PRINT,
  PCP_OAM_ETH_RMEP_INFO_GET_VERIFY,
  PCP_OAM_ETH_RMEP_INFO_GET_UNSAFE,
  PCP_OAM_ETH_ACC_MEP_TX_RDI_SET,
  PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_PRINT,
  PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_UNSAFE,
  PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_VERIFY,
  PCP_OAM_ETH_ACC_MEP_TX_RDI_GET,
  PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_PRINT,
  PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_VERIFY,
  PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_UNSAFE,
  PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET,
  PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_PRINT,
  PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_UNSAFE,
  PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_VERIFY,
  PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET,
  PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_PRINT,
  PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_UNSAFE,
  PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_VERIFY,
  PCP_OAM_ETH_GET_PROCS_PTR,
  PCP_OAM_ETH_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  PCP_OAM_ETH_PROCEDURE_DESC_LAST
} PCP_OAM_ETH_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_OAM_ETH_NO_VID_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_OAM_ETH_FIRST,
  PCP_OAM_ETH_NO_OUT_AC_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_COS_PROFILE_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_MP_LEVEL_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_ACC_MEP_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_ACC_LMM_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_ACC_DLM_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_PRIO_NDX_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_HEADER_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_RMEP_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_RMEP_NDX_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_RDI_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_DM_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_CUSTOM_HEADER_SIZE_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_TC_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_DP_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_DIRECTION_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_DN_VID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_AC_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_UP_VID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_CCM_INTERVAL_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_CCM_PRIORITY_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_MA_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_MEP_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_RMEP_ID_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_INTERVAL_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_PRIORITY_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_TX_FCF_CURR_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_TX_FCF_PREV_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_RX_FCB_CURR_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_RX_FCB_PREV_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  PCP_OAM_ETH_DESTINATION_TYPE_UNKNOWN_ERR,
  PCP_OAM_ETH_RMEP_ADD_FAILURE_ERR,
  PCP_OAM_ETH_RMEP_REMOVE_FOUND_FAILURE_ERR,
  PCP_OAM_ETH_ACC_RX_COUNTER_OUT_OF_RANGE_ERR,
  PCP_OAM_ETH_ACC_TX_COUNTER_OUT_OF_RANGE_ERR,

  /*
   * Last element. Do no touch.
   */
  PCP_OAM_ETH_ERR_LAST
} PCP_OAM_ETH_ERR;

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
 *   pcp_oam_eth_general_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets general configuration of Ethernet
 *   OAM.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO        *info -
 *     General configuration information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_general_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO        *info
  );

uint32
  pcp_oam_eth_general_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO        *info
  );

uint32
  pcp_oam_eth_general_info_get_verify(
    SOC_SAND_IN  int                       unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_general_info_set_unsafe" API.
 *     Refer to "pcp_oam_eth_general_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_general_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_ETH_GENERAL_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_cos_mapping_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function maps the cos profile and the user priority
 *   (both are MEP attributes) to TC and DP for the ITMH
 *   (relevant for down MEP injection)
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx -
 *     Class of service. Range: 0-3
 *   SOC_SAND_IN  uint32                        prio_ndx -
 *     VLAN tag's user priority. Range: 0 - 7
 *   SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO    *info -
 *     COS mapping configuration information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_cos_mapping_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO    *info
  );

uint32
  pcp_oam_eth_cos_mapping_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO    *info
  );

uint32
  pcp_oam_eth_cos_mapping_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_cos_mapping_info_set_unsafe" API.
 *     Refer to "pcp_oam_eth_cos_mapping_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  pcp_oam_eth_cos_mapping_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_COS_PROFILE_ID      cos_ndx,
    SOC_SAND_IN  uint32                        prio_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_COS_MAPPING_INFO    *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_mp_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function is used to define a Maintenance Point (MP)
 *   on an Attachment-Circuit (incoming/outgoing), MD-level,
 *   and MP direction (up/down). If the MP is one of the 4K
 *   accelerated MEPs, the function configures the related
 *   OAMP databases and associates the AC, MD-Level and the
 *   MP type with a user-provided handle. This handle is
 *   later used by user to access OAMP database for this MEP.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key -
 *     Identifier of the key (direction, attachment-circuit,
 *     level) on which the MP is set.
 *   SOC_SAND_IN  PCP_OAM_ETH_MP_INFO             *info -
 *     MP configuration information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_mp_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO             *info
  );

uint32
  pcp_oam_mp_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO             *info
  );

uint32
  pcp_oam_mp_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_mp_info_set_unsafe" API.
 *     Refer to "pcp_oam_mp_info_set_unsafe" API for details.
*********************************************************************/
uint32
  pcp_oam_mp_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY              *mp_key,
    SOC_SAND_OUT PCP_OAM_ETH_MP_INFO             *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_mep_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function configures an accelerated MEP.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mp_info_set()). Range:
 *     0-4K
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO        *info -
 *     Accelerated MEP configuration information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO        *info
  );

uint32
  pcp_oam_eth_acc_mep_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO        *info
  );

uint32
  pcp_oam_eth_acc_mep_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_mep_info_set_unsafe" API.
 *     Refer to "pcp_oam_eth_acc_mep_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_mep_custom_header_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets user custom header.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mp_info_set()). Range:
 *     0-4K
 *   SOC_SAND_IN  uint32                        header -
 *     Placed at the beginning of the packet
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_custom_header_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint32                        header
  );

uint32
  pcp_oam_eth_acc_mep_custom_header_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint32                        header
  );

uint32
  pcp_oam_eth_acc_mep_custom_header_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_mep_custom_header_set_unsafe" API.
 *     Refer to "pcp_oam_eth_acc_mep_custom_header_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_custom_header_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT uint32                        *header
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_lmm_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets LMM info for the specified LMM
 *   identifier.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx -
 *     LMM user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_acc_mep_info_set()).
 *     Range: 0-127
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO        *info -
 *     LMM configuration setting
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_lmm_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO        *info
  );

uint32
  pcp_oam_eth_acc_lmm_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO        *info
  );

uint32
  pcp_oam_eth_acc_lmm_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_lmm_set_unsafe" API.
 *     Refer to "pcp_oam_eth_acc_lmm_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_lmm_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_LMM_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_dlm_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets DLM info for the specified DLM
 *   identifier.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx -
 *     DLM user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_acc_mep_info_set()).
 *     Range: 0-127
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO        *info -
 *     DLM configuration setting
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_dlm_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO        *info
  );

uint32
  pcp_oam_eth_acc_dlm_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO        *info
  );

uint32
  pcp_oam_eth_acc_dlm_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_dlm_set_unsafe" API.
 *     Refer to "pcp_oam_eth_acc_dlm_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_dlm_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_DLM_INFO        *info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_rmep_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function adds a remote MEP to list of remote MEPs
 *   associated with the accelerated MEP identified by
 *   acc_mep_ndx.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key -
 *     Remote MEP key consisting of the remote MEP id of the
 *     arriving CFM packet and the local accelerated MEP handle
 *     as determined by Procedure
 *     pcp_oam_eth_acc_mep_info_set()
 *   SOC_SAND_IN  uint32                        rmep -
 *     Handle to the remote MEP object that is monitored by the
 *     local MP. Range: 0 - 8K
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_rmep_add_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key,
    SOC_SAND_IN  uint32                        rmep
  );

uint32
  pcp_oam_eth_rmep_add_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key,
    SOC_SAND_IN  uint32                        rmep
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_rmep_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function removes a remote MEP to list of remote
 *   MEPs associated with the accelerated MEP identified by
 *   acc_mep_ndx.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key -
 *     Remote MEP key consisting of the remote MEP id of the
 *     arriving CFM packet and the local accelerated MEP handle
 *     as determined by Procedure
 *     pcp_oam_eth_acc_mep_info_set()
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_rmep_remove_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key
  );

uint32
  pcp_oam_eth_rmep_remove_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY            *rmep_key
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_rmep_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This API sets a remote MEP info.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                        rmep_ndx -
 *     Remote MP to be monitored by the local MP. Range: 0 - 8K
 *   SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO           *rmep_info -
 *     Remote MEP configuration information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_rmep_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO           *rmep_info
  );

uint32
  pcp_oam_eth_rmep_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO           *rmep_info
  );

uint32
  pcp_oam_eth_rmep_info_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_rmep_info_set_unsafe" API.
 *     Refer to "pcp_oam_eth_rmep_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_rmep_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                        rmep_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_RMEP_INFO           *rmep_info
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_acc_mep_tx_rdi_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function sets/clears remote defect indication on
 *   outgoing CCM messages
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mp_info_set()). Range:
 *     0-4K
 *   SOC_SAND_IN  uint8                       rdi -
 *     Remote defect indication T - Set RDI in outgoing CCM
 *     packetF - Do not set RDI in outgoing CCM packet
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_tx_rdi_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint8                       rdi
  );

uint32
  pcp_oam_eth_acc_mep_tx_rdi_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_IN  uint8                       rdi
  );

uint32
  pcp_oam_eth_acc_mep_tx_rdi_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_oam_eth_acc_mep_tx_rdi_set_unsafe" API.
 *     Refer to "pcp_oam_eth_acc_mep_tx_rdi_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  pcp_oam_eth_acc_mep_tx_rdi_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_ID          acc_mep_ndx,
    SOC_SAND_OUT uint8                       *rdi
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_mep_delay_measurement_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function returns the last measured delay. In case
 *   DM was not defined on the provided MEP, an error is
 *   thrown
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mep_info_set()). Range:
 *     0-127
 *   SOC_SAND_OUT uint32                        *dm -
 *     Last delay measurement
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_mep_delay_measurement_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx,
    SOC_SAND_OUT uint32                        *dm
  );

uint32
  pcp_oam_eth_mep_delay_measurement_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_ID          acc_dlm_ndx
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_mep_loss_measurement_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function returns the loss measurement counters of
 *   the device (both local and remote). In case LM was not
 *   defined on the provided MEP, an error is thrown
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx -
 *     The user-defined handle, selected when the accelerated
 *     MEP was defined (see pcp_oam_eth_mep_info_set()). Range:
 *     0-127
 *   SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info -
 *     Loss measurement counters of the device (both local and
 *     remote)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_oam_eth_mep_loss_measurement_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx,
    SOC_SAND_OUT PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  );

uint32
  pcp_oam_eth_mep_loss_measurement_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_ID          acc_lmm_ndx
  );

/*********************************************************************
* NAME:
 *   pcp_oam_eth_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_oam_api_eth module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_oam_eth_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_oam_eth_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_oam_api_eth module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_oam_eth_get_errs_ptr(void);

uint32
  PCP_OAM_ETH_GENERAL_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_GENERAL_INFO *info
  );

uint32
  PCP_OAM_ETH_COS_MAPPING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_COS_MAPPING_INFO *info
  );

uint32
  PCP_OAM_ETH_MP_KEY_verify(
    SOC_SAND_IN  PCP_OAM_ETH_MP_KEY *info
  );

uint32
  PCP_OAM_ETH_MP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_MP_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_DN_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_UP_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_FWD_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_FWD_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_CCM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_CCM_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_LMM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LMM_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_DLM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_DLM_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_PERFORMANCE_MONITORING_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_INFO *info
  );

uint32
  PCP_OAM_ETH_RMEP_KEY_verify(
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_KEY *info
  );

uint32
  PCP_OAM_ETH_RMEP_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_RMEP_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_DLM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_DLM_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_LMM_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_LMM_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LM_CNT_SET_INFO *info
  );

uint32
  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_verify(
    SOC_SAND_IN  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_OAM_ETH_INCLUDED__*/
#endif

