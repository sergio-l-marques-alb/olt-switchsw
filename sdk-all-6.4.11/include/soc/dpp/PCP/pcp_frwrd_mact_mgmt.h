/* $Id: pcp_frwrd_mact_mgmt.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PCP_FRWRD_MACT_MGMT_INCLUDED__
/* { */
#define __SOC_PCP_FRWRD_MACT_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PCP/pcp_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PCP/pcp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_FRWRD_MACT_ENTRY_NOF_AGES                                  (8)
#define PCP_FRWRD_MACT_ENTRY_NOF_IS_OWNED                              (2)

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
  PCP_FRWRD_MACT_OPER_MODE_INFO_SET = PCP_PROC_DESC_BASE_FRWRD_MACT_MGMT_FIRST,
  PCP_FRWRD_MACT_OPER_MODE_INFO_SET_PRINT,
  PCP_FRWRD_MACT_OPER_MODE_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_OPER_MODE_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_OPER_MODE_INFO_GET,
  PCP_FRWRD_MACT_OPER_MODE_INFO_GET_PRINT,
  PCP_FRWRD_MACT_OPER_MODE_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_OPER_MODE_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_AGING_INFO_SET,
  PCP_FRWRD_MACT_AGING_INFO_SET_PRINT,
  PCP_FRWRD_MACT_AGING_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_AGING_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_AGING_INFO_GET,
  PCP_FRWRD_MACT_AGING_INFO_GET_PRINT,
  PCP_FRWRD_MACT_AGING_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_AGING_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET,
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_PRINT,
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET,
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_PRINT,
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET,
  PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_PRINT,
  PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_UNSAFE,
  PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_VERIFY,
  PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET,
  PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_PRINT,
  PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_VERIFY,
  PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_UNSAFE,
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET,
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_PRINT,
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET,
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_PRINT,
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET,
  PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_PRINT,
  PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET,
  PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_PRINT,
  PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET,
  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_PRINT,
  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_PRINT,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_UNSAFE,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_VERIFY,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_PRINT,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_VERIFY,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_UNSAFE,
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_SET,
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_PRINT,
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_GET,
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_PRINT,
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET,
  PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET,
  PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET,
  PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET,
  PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_PORT_INFO_SET,
  PCP_FRWRD_MACT_PORT_INFO_SET_PRINT,
  PCP_FRWRD_MACT_PORT_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_PORT_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_PORT_INFO_GET,
  PCP_FRWRD_MACT_PORT_INFO_GET_PRINT,
  PCP_FRWRD_MACT_PORT_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_PORT_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_TRAP_INFO_SET,
  PCP_FRWRD_MACT_TRAP_INFO_SET_PRINT,
  PCP_FRWRD_MACT_TRAP_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_TRAP_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_TRAP_INFO_GET,
  PCP_FRWRD_MACT_TRAP_INFO_GET_PRINT,
  PCP_FRWRD_MACT_TRAP_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_TRAP_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET,
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_PRINT,
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_UNSAFE,
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_VERIFY,
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET,
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_PRINT,
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_UNSAFE,
  PCP_FRWRD_MACT_EVENT_GET,
  PCP_FRWRD_MACT_EVENT_GET_PRINT,
  PCP_FRWRD_MACT_EVENT_GET_UNSAFE,
  PCP_FRWRD_MACT_EVENT_GET_VERIFY,
  PCP_FRWRD_MACT_EVENT_PARSE,
  PCP_FRWRD_MACT_EVENT_PARSE_PRINT,
  PCP_FRWRD_MACT_EVENT_PARSE_UNSAFE,
  PCP_FRWRD_MACT_EVENT_PARSE_VERIFY,
  PCP_VSI_INFO_SET,
  PCP_VSI_INFO_SET_PRINT,
  PCP_VSI_INFO_SET_UNSAFE,
  PCP_VSI_INFO_SET_VERIFY,
  PCP_VSI_INFO_GET,
  PCP_VSI_INFO_GET_PRINT,
  PCP_VSI_INFO_GET_UNSAFE,
  PCP_VSI_INFO_GET_VERIFY,
  PCP_FRWRD_MACT_MGMT_GET_PROCS_PTR,
  PCP_FRWRD_MACT_MGMT_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

   PCP_FRWRD_MACT_MAX_AGE_GET,
   PCP_FRWRD_MACT_AGE_CONF_DEFAULT_GET,
   PCP_FRWRD_MACT_AGE_CONF_WRITE,
   PCP_FRWRD_MACT_AGE_MODIFY_GET,
   PCP_FRWRD_MACT_AGE_CONF_READ,
   PCP_FRWRD_MACT_EVENT_KEY_INDEX_GET,
   PCP_FRWRD_MACT_LEARN_MSGS_OLP_MSG_SET,
   PCP_FRWRD_MACT_LEARN_MSGS_OLP_MSG_GET,
   PCP_FRWRD_MACT_EVENT_WRITE,
   PCP_FRWRD_MACT_EVENT_HANDLE_INFO_VERIFY,
   PCP_FRWRD_MACT_INIT,
   PCP_FRWRD_MACT_MGMT_INIT,
   PCP_FRWRD_MACT_REGS_INIT,
   PCP_FRWRD_MACT_IS_INGRESS_LEARNING_GET,

  /*
   * Last element. Do no touch.
   */
  PCP_FRWRD_MACT_MGMT_PROCEDURE_DESC_LAST
} PCP_FRWRD_MACT_MGMT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  PCP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR = PCP_ERR_DESC_BASE_FRWRD_MACT_MGMT_FIRST,
  PCP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_MGMT_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  PCP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR,
  PCP_VSI_MAC_PCP_VSI_INFO_VERIFY_OUT_OF_RANGE_ERR,

  PCP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_MAC_LIMIT_FID_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_EVENT_HANDLE_KEY_IS_LAG_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR,
  PCP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR,
  PCP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR,

  /*
   * Last element. Do no touch.
   */
  PCP_FRWRD_MACT_MGMT_ERR_LAST
} PCP_FRWRD_MACT_MGMT_ERR;

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

} PCP_FRWRD_MACT_AGING_EVENT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE then the aging process internally deletes aged
   *  out entries.
   */
   PCP_FRWRD_MACT_AGING_EVENT age_action[PCP_FRWRD_MACT_ENTRY_NOF_AGES][PCP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

} PCP_FRWRD_MACT_AGING_EVENT_TABLE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Modification age to insert delete action.
   */
  uint32 age_delete[PCP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

  /*
   *  Modification age to insert delete aged-out.
   */
  uint32 age_aged_out[PCP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

  /*
   *  Modification age to insert refresh action.
   */
  uint32 age_refresh[PCP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

} PCP_FRWRD_MACT_AGING_MODIFICATION;

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
  pcp_frwrd_mact_mgmt_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint8
  pcp_frwrd_mact_mgmt_is_b0_high_resolution(
    SOC_SAND_IN  int                                 unit
  );


/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_oper_mode_info_set_unsafe
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
 *   SOC_SAND_IN  PCP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info -
 *     MACT learning setting
 * REMARKS:
 *   - T20E supports only Egress independent and centralized
 *   learning modes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_mact_oper_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

uint32
  pcp_frwrd_mact_oper_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_oper_mode_info_set_unsafe" API.
 *     Refer to "pcp_frwrd_mact_oper_mode_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_oper_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_aging_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the aging info including enable aging and aging
 *   time.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_MACT_AGING_INFO               *aging_info -
 *     Whether to perform aging over the MAC entries and time
 *     of aging.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_mact_aging_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_INFO               *aging_info
  );

uint32
  pcp_frwrd_mact_aging_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_INFO               *aging_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_aging_info_set_unsafe" API.
 *     Refer to "pcp_frwrd_mact_aging_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_aging_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_AGING_INFO               *aging_info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_aging_events_handle_info_set_unsafe
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
 *   SOC_SAND_IN  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE     *aging_info -
 *     Device actions upon aging-related events.
 * REMARKS:
 *   - use soc_ppd_frwrd_mact_event_handle_info_set() to set how
 *   the OLP should distribute age-out and refresh events.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_mact_aging_events_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

uint32
  pcp_frwrd_mact_aging_events_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_aging_events_handle_info_set_unsafe"
 *     API.
 *     Refer to
 *     "pcp_frwrd_mact_aging_events_handle_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_aging_events_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_mac_limit_glbl_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable the MAC limit feature, which limits per fid the
 *   maximum number of entries allowed to be in the MAC
 *   Table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info -
 *     Limitation settings, including if this feature is
 *     enabled and how to act when static entry tries to exceed
 *     the limit.
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_mact_mac_limit_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

uint32
  pcp_frwrd_mact_mac_limit_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_mac_limit_glbl_info_set_unsafe" API.
 *     Refer to
 *     "pcp_frwrd_mact_mac_limit_glbl_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_mac_limit_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_learn_profile_limit_info_set_unsafe
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
 *   SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info -
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
  pcp_frwrd_mact_learn_profile_limit_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );

uint32
  pcp_frwrd_mact_learn_profile_limit_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );

uint32
  pcp_frwrd_mact_learn_profile_limit_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_learn_profile_limit_info_set_unsafe"
 *     API.
 *     Refer to
 *     "pcp_frwrd_mact_learn_profile_limit_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_learn_profile_limit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT PCP_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_event_handle_profile_set_unsafe
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
  pcp_frwrd_mact_event_handle_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  );

uint32
  pcp_frwrd_mact_event_handle_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  );

uint32
  pcp_frwrd_mact_event_handle_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_event_handle_profile_set_unsafe" API.
 *     Refer to
 *     "pcp_frwrd_mact_event_handle_profile_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_event_handle_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                  *event_handle_profile
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_event_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the last event parameters for the direct access mode
 *   (i.e., if no OLP messages are sent in case of event)
 *   from the Event FIFO.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_INFO         *info -
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
  pcp_frwrd_mact_event_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );


/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_event_handle_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to handle an event according to the event key
 *   parameters (event-type,vsi-handle-profile,is-lag)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key -
 *     The key that identifies the event
 *     (event-type,vsi-handle-profile,is-lag).
 *   SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info -
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
  pcp_frwrd_mact_event_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

uint32
  pcp_frwrd_mact_event_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

uint32
  pcp_frwrd_mact_event_handle_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_event_handle_info_set_unsafe" API.
 *     Refer to "pcp_frwrd_mact_event_handle_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_event_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_learn_msgs_distribution_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute the learn messages to other
 *   devices/CPU.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info -
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
  pcp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

uint32
  pcp_frwrd_mact_learn_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_learn_msgs_distribution_info_set_unsafe"
 *     API.
 *     Refer to
 *     "pcp_frwrd_mact_learn_msgs_distribution_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute the shadow messages to the other
 *   devices/CPU.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info -
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
  pcp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

uint32
  pcp_frwrd_mact_shadow_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe"
 *     API.
 *     Refer to
 *     "pcp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  pcp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_event_parse_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The MACT may report different events using the event
 *   FIFO (e.g., learn, age, transplant, and retrieve). This
 *   API Parses the event buffer into a meaningful structure.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_BUFFER            *event_buf -
 *     Buffer includes MACT event
 *   SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_INFO              *mact_event -
 *     MACT Event parsed into structure
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_frwrd_mact_event_parse_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_INFO              *mact_event
  );

uint32
  pcp_frwrd_mact_event_parse_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );
void
  PCP_FRWRD_MACT_AGING_MODIFICATION_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_AGING_MODIFICATION  *info
  );

void
  PCP_FRWRD_MACT_AGING_EVENT_TABLE_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_AGING_EVENT_TABLE  *info
  );

/* 
 *	Function from Soc_petra-PP vsi.
 */
/*********************************************************************
* NAME:
 *   pcp_vsi_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Virtual Switch Instance information. After
 *   setting the VSI, the user may attach L2 Logical
 *   Interfaces to it: ACs; PWEs
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  PCP_VSI_ID                              vsi_ndx -
 *     System VSID. Range: 0-64K
 *   SOC_SAND_IN  PCP_VSI_INFO                            *vsi_info -
 *     VSI attributes
 * REMARKS:
 *   - Default forwarding destination:T20E: The destination
 *   is fully configurablePetra-B: The destination must
 *   correspond to one of the action pointers configured by
 *   soc_ppd_frwrd_mact_vsi_default_info_set()- Soc_petra-B Flooding:
 *   When the flooding multicast ID mapping from the local
 *   VSI is insufficient, the user may either set the
 *   destination as FEC ID with multicast destination, or
 *   utilize the ingress multicast table to remap the MID.-
 *   Soc_petra-B: When the local VSI is > 4K: The EEI cannot be
 *   used as I-SID; Topology ID. Enable-My-MAC and Enable
 *   routing have to be negated. The FID is the VSID.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  pcp_vsi_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  PCP_VSI_INFO                            *vsi_info
  );

uint32
  pcp_vsi_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  PCP_VSI_INFO                            *vsi_info
  );

uint32
  pcp_vsi_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VSI_ID                              vsi_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "pcp_vsi_info_set_unsafe" API.
 *     Refer to "pcp_vsi_info_set_unsafe" API for details.
*********************************************************************/
uint32
  pcp_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT PCP_VSI_INFO                            *vsi_info
  );

uint32
  PCP_VSI_INFO_verify(
    SOC_SAND_IN  PCP_VSI_INFO *info
  );

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_mgmt_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   pcp_api_frwrd_mact_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_frwrd_mact_mgmt_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   pcp_frwrd_mact_mgmt_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   pcp_api_frwrd_mact_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_frwrd_mact_mgmt_get_errs_ptr(void);

uint32
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  );

uint32
  PCP_FRWRD_MACT_AGING_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_INFO *info
  );

uint32
  PCP_FRWRD_MACT_EVENT_HANDLE_KEY_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY *info
  );

uint32
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_INFO *info
  );

uint32
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  );

uint32
  PCP_FRWRD_MACT_MSG_DISTR_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO *info
  );

uint32
  PCP_FRWRD_MACT_EVENT_PROCESSING_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  );

uint32
  PCP_FRWRD_MACT_OPER_MODE_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_OPER_MODE_INFO *info
  );

uint32
  PCP_FRWRD_MACT_MAC_LIMIT_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_INFO *info
  );

uint32
  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  );

uint32
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  );

uint32
  PCP_FRWRD_MACT_PORT_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_PORT_INFO *info
  );

uint32
  PCP_FRWRD_MACT_EVENT_LAG_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_LAG_INFO *info
  );

uint32
  PCP_FRWRD_MACT_EVENT_INFO_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_INFO *info
  );

uint32
  PCP_FRWRD_MACT_EVENT_BUFFER_verify(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_BUFFER *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_FRWRD_MACT_INCLUDED__*/
#endif

