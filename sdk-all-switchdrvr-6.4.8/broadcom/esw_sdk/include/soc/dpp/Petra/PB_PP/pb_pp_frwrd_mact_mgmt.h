/* $Id: pb_pp_frwrd_mact_mgmt.h,v 1.9 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_frwrd_mact_mgmt.h
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

#ifndef __SOC_PB_PP_FRWRD_MACT_MGMT_INCLUDED__
/* { */
#define __SOC_PB_PP_FRWRD_MACT_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_mact_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES                                  (8)
#define SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED                              (2)

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
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_FRWRD_MACT_MGMT_FIRST,
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET,
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET,
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_TRAP_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_TRAP_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET,
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET,
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_EVENT_GET,
  SOC_PB_PP_FRWRD_MACT_EVENT_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_EVENT_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_EVENT_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_EVENT_PARSE,
  SOC_PB_PP_FRWRD_MACT_EVENT_PARSE_PRINT,
  SOC_PB_PP_FRWRD_MACT_EVENT_PARSE_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_EVENT_PARSE_VERIFY,
  SOC_PB_PP_FRWRD_MACT_MGMT_GET_PROCS_PTR,
  SOC_PB_PP_FRWRD_MACT_MGMT_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

   SOC_PB_PP_FRWRD_MACT_MAX_AGE_GET,
   SOC_PB_PP_FRWRD_MACT_AGE_CONF_DEFAULT_GET,
   SOC_PB_PP_FRWRD_MACT_AGE_CONF_WRITE,
   SOC_PB_PP_FRWRD_MACT_AGE_MODIFY_GET,
   SOC_PB_PP_FRWRD_MACT_AGE_CONF_READ,
   SOC_PB_PP_FRWRD_MACT_EVENT_KEY_INDEX_GET,
   SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_OLP_MSG_SET,
   SOC_PB_PP_FRWRD_MACT_LEARN_MSGS_OLP_MSG_GET,
   SOC_PB_PP_FRWRD_MACT_EVENT_WRITE,
   SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_VERIFY,
   SOC_PB_PP_FRWRD_MACT_INIT,
   SOC_PB_PP_FRWRD_MACT_MGMT_INIT,
   SOC_PB_PP_FRWRD_MACT_REGS_INIT,
   SOC_PB_PP_FRWRD_MACT_IS_INGRESS_LEARNING_GET,
   SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF_GET,
   SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE,
   SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_UNSAFE,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_MACT_MGMT_PROCEDURE_DESC_LAST
} SOC_PB_PP_FRWRD_MACT_MGMT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FRWRD_MACT_MGMT_FIRST,
  SOC_PB_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_MGMT_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_FID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_IS_LAG_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR,
  SOC_PB_PP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_MSG_LEN_ERR,
  SOC_PB_PP_FRWRD_MACT_MSG_IS_NOT_LEARN_MSG_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_MACT_MGMT_ERR_LAST
} SOC_PB_PP_FRWRD_MACT_MGMT_ERR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE then the aging process internally deletes aged
   *  out entries.
   */
  uint8 deleted;
  /*
   *  If TRUE then the aging process generates events for aged
   *  out entries. Should be set to TRUE in a centralized
   *  mode.
   */
  uint8 aged_out;
  /*
   *  If TRUE then the aging process generates events for
   *  entries refreshed in the last traverse of the MACT.
   */
  uint8 refreshed;

} SOC_PB_PP_FRWRD_MACT_AGING_EVENT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE then the aging process internally deletes aged
   *  out entries.
   */
   SOC_PB_PP_FRWRD_MACT_AGING_EVENT age_action[SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_AGES][SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

} SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Modification age to insert delete action.
   */
  uint32 age_delete[SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

  /*
   *  Modification age to insert delete aged-out.
   */
  uint32 age_aged_out[SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

  /*
   *  Modification age to insert refresh action.
   */
  uint32 age_refresh[SOC_PB_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

} SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION;

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

uint32
  soc_pb_pp_frwrd_mact_mgmt_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint8
  soc_pb_pp_frwrd_mact_mgmt_is_b0_high_resolution(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                  *is_high_res
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_oper_mode_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the mode of the MACT, including - ingress vs.
 *   egress learning- how each device responds internally to
 *   events (learn/aged-out/refresh) - which events to inform
 *   other devices.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info -
 *     MACT learning setting
 * REMARKS:
 *   - T20E supports only Egress independent and centralized
 *   learning modes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_oper_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

uint32
  soc_pb_pp_frwrd_mact_oper_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_oper_mode_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_mact_oper_mode_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_oper_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_aging_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the aging info including enable aging and aging
 *   time.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info -
 *     Whether to perform aging over the MAC entries and time
 *     of aging.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info
  );

uint32
  soc_pb_pp_frwrd_mact_aging_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_aging_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_mact_aging_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_aging_events_handle_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the device action upon events invoked by the aging
 *   process: - Whether the device deletes aged-out entries
 *   internally - Whether the device generates an event for
 *   aged-out entries - Whether the device generates an event
 *   for refreshed entries
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE     *aging_info -
 *     Device actions upon aging-related events.
 * REMARKS:
 *   - use soc_ppd_frwrd_mact_event_handle_info_set() to set how
 *   the OLP should distribute age-out and refresh events.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_events_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

uint32
  soc_pb_pp_frwrd_mact_aging_events_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_aging_events_handle_info_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_aging_events_handle_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_events_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Maps FID-Profile to FID, for shared learning.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  fid_profile_ndx -
 *     FID Profile. Range: 1 - 7. The 0 value is used when FID
 *     = VSI.
 *   SOC_SAND_IN  SOC_PB_PP_FID                                 fid -
 *     Filtering ID. Range: 0 - 16K-1.
 * REMARKS:
 *   - The FID-profile is an attribute of the VSI.- For VSIs
 *   with an FID-profile = 0, FID = VSI.- T20E: This API can
 *   be omitted since for all VSIs, FID = VSI.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FID                                 fid
  );

uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FID                                 fid
  );

uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_FID                                 *fid
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable the MAC limit feature, which limits per fid the
 *   maximum number of entries allowed to be in the MAC
 *   Table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info -
 *     Limitation settings, including if this feature is
 *     enabled and how to act when static entry tries to exceed
 *     the limit.
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

uint32
  soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the limit profile information including the
 *   MAC-limit (i.e., the maximum number of entries an FID
 *   can hold in the MAC Table), and the notification action
 *   if the configured limit is exceeded.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mac_learn_profile_ndx -
 *     MAC-learn-profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info -
 *     Limit profile information including the maximum number
 *     of entries that can be learned/inserted; the action to
 *     perform when an entry is tryied to be inserted/learned,
 *     exceeding the limitation.
 * REMARKS:
 *   - No limitation for a specific profile can be configured
 *   by setting is_limited to FALSE at limit_info.-
 *   mac_learn_profile_ndx is set according to
 *   soc_ppd_vsi_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );

uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );

uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_learn_profile_limit_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_event_handle_profile_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map the mac-learn-profile to the event-handle profile.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mac_learn_profile_ndx -
 *     MAC-learn-profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  uint32                                  event_handle_profile -
 *     Profile used to set how MACT events are handled. Range:
 *     0 - 1.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   mac_learn_profile_ndx is set according to
 *   soc_ppd_vsi_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  );

uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  );

uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_event_handle_profile_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_event_handle_profile_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                  *event_handle_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_event_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the last event parameters for the direct access mode
 *   (i.e., if no OLP messages are sent in case of event)
 *   from the Event FIFO.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_INFO         *info -
 *     Event information parameters (Key, Payload, Command
 *     type).
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   This API must be called only if the Learn / Shadow
 *   distribution type is through direct access to the Event
 *   FIFO
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to handle an event according to the event key
 *   parameters (event-type,vsi-handle-profile,is-lag)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key -
 *     The key that identifies the event
 *     (event-type,vsi-handle-profile,is-lag).
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info -
 *     How to handle the given events, including where (i.e.,
 *     which FIFO) to send these events.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. - To
 *   set the FIFO configuration, use for the System learn
 *   FIFO soc_ppd_frwrd_mact_sys_learn_msgs_distribution_info_set
 *   and for the Shadow FIFO
 *   soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set. - For
 *   the get API, a single event type must be set in the
 *   event-key parameter.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

uint32
  soc_pb_pp_frwrd_mact_event_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

uint32
  soc_pb_pp_frwrd_mact_event_handle_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_mact_event_handle_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute the learn messages to other
 *   devices/CPU.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info -
 *     Distribution information, in particular the header to
 *     associate with the learn messages.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   Soc_petra-B: both learn messages and shadow message must
 *   have the same external header, i.e. either both have an
 *   ITMH Header or none of them. - The get API returns also
 *   the EtherType if not inserted by the user.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

uint32
  soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute the shadow messages to the other
 *   devices/CPU.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info -
 *     Distribution information, in particular the header to
 *     associate with the shadow messages.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   Soc_petra-B: both learn messages and shadow message must
 *   have the same external header, i.e. either both have an
 *   ITMH Header or none of them. - The get API returns also
 *   the EtherType if not inserted by the user.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

uint32
  soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_mac_limit_exceeded_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the information if the MAC limitation is exceeded,
 *   i.e. when a MAC Table entry is tryied to be inserted and
 *   exceeds the limitation set per FID. This insertion can
 *   be triggered by CPU or after a packet learning.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info -
 *     Information if the MAC limit was exceed, including the
 *     last FID which caused this limitation violation.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_exceeded_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_port_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set per port MACT management information including which
 *   profile to activate when SA is known in this port.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info -
 *     Port information.
 * REMARKS:
 *   - Use soc_ppd_frwrd_mact_trap_info_set() to set the drop
 *   action for an SA MAC.- Soc_petra-B only, error when called
 *   over T20E device.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info
  );

uint32
  soc_pb_pp_frwrd_mact_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info
  );

uint32
  soc_pb_pp_frwrd_mact_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_port_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_mact_port_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_trap_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   For each trap type, set the action profile. Different
 *   actions may be assigned to the same trap type according
 *   to the port-profile (4 possibilities).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx -
 *     Type of the Trap.
 *   SOC_SAND_IN  uint32                                  port_profile_ndx -
 *     Per port profile, to enable the setting of different
 *     actions for the same trap type. To set this profile for
 *     a port, use soc_ppd_frwrd_mact_port_info_set().
 *   SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile -
 *     Trap information including snoop/forwarding action. The
 *     trap_code is necessarily 0.
 * REMARKS:
 *   - For the Trap is
 *   SOC_PPD_FRWRD_MACT_TRAP_TYPE_SAME_INTERFACE port_profile_ndx
 *   has to be 0, since there is port profile is not relevant
 *   for this Trap.- Use soc_ppd_frwrd_mact_port_info_set() to
 *   set the mapping from the port profile to the profile for
 *   action.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_trap_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile
  );

uint32
  soc_pb_pp_frwrd_mact_trap_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile
  );

uint32
  soc_pb_pp_frwrd_mact_trap_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_trap_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_mact_trap_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_trap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                      *action_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the information for bridging compatible Multicast
 *   MAC addresses.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info -
 *     Compatible Multicast MAC information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  );

uint32
  soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_ip_compatible_mc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_event_parse_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The MACT may report different events using the event
 *   FIFO (e.g., learn, age, transplant, and retrieve). This
 *   API Parses the event buffer into a meaningful structure.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf -
 *     Buffer includes MACT event
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_INFO              *mact_event -
 *     MACT Event parsed into structure
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_parse_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_INFO              *mact_event
  );

uint32
  soc_pb_pp_frwrd_mact_event_parse_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );
void
  SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_MODIFICATION  *info
  );

void
  SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENT_TABLE  *info
  );

uint32
  soc_pb_pp_frwrd_mact_learn_msg_parse_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG                  *learn_msg,
    SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF             *learn_msg_conf,
    SOC_SAND_OUT  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO       *learn_events
  );

uint32
soc_pb_pp_frwrd_mact_learn_msg_conf_get_unsafe(
  SOC_SAND_IN   int                             unit,
  SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG            *learn_msg,
  SOC_SAND_OUT  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF       *learn_msg_conf
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_mgmt_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_frwrd_mact_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_mact_mgmt_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_mgmt_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_frwrd_mact_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_mact_mgmt_get_errs_ptr(void);

uint32
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FRWRD_MACT_INCLUDED__*/
#endif

