
/* $Id: arad_pp_api_frwrd_mact.h,v 1.11 Broadcom SDK $
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

#ifndef __ARAD_PP_API_FRWRD_MACT_INCLUDED__
/* { */
#define __ARAD_PP_API_FRWRD_MACT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_frwrd_mact.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Indicates iterator over MAC table reached end of the
 *     table.                                                  */
#define  ARAD_PP_FRWRD_MACT_ITER_END (SOC_PPC_FRWRD_MACT_ITER_END)

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
/* Define application ID of LEM DB prefix which will be used in arad_pp_xxx files  */
#define ARAD_PP_FRWRD_MACT_PREFIX_APP_ID_OMAC                  SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_OMAC
#define ARAD_PP_FRWRD_MACT_PREFIX_APP_ID_OMAC_2_VMAC           SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_OMAC_2_VMAC
#define ARAD_PP_FRWRD_MACT_PREFIX_APP_ID_VMAC                  SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_VMAC
#define ARAD_PP_FRWRD_MACT_PREFIX_APP_ID_VMAC_2_OMAC           SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_VMAC_2_OMAC
#define ARAD_PP_NOF_FRWRD_MACT_PREFIX_APP_IDS                  SOC_PPC_NOF_FRWRD_MACT_PREFIX_APP_IDS
typedef SOC_PPC_FRWRD_MACT_APP_ID                              ARAD_PP_FRWRD_MACT_APP_ID;

#define ARAD_PP_FRWRD_MACT_KEY_TYPE_MAC_ADDR                 SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR
#define ARAD_PP_FRWRD_MACT_KEY_TYPE_IPV4_MC                  SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC
#define ARAD_PP_FRWRD_MACT_KEY_TYPE_SLB                      SOC_PPC_FRWRD_MACT_KEY_TYPE_SLB
#define ARAD_PP_NOF_FRWRD_MACT_KEY_TYPES                     SOC_PPC_NOF_FRWRD_MACT_KEY_TYPES
typedef SOC_PPC_FRWRD_MACT_KEY_TYPE                            ARAD_PP_FRWRD_MACT_KEY_TYPE;

#define ARAD_PP_FRWRD_MACT_ADD_TYPE_INSERT                   SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT
#define ARAD_PP_FRWRD_MACT_ADD_TYPE_LEARN                    SOC_PPC_FRWRD_MACT_ADD_TYPE_LEARN
#define ARAD_PP_FRWRD_MACT_ADD_TYPE_REFRESH                  SOC_PPC_FRWRD_MACT_ADD_TYPE_REFRESH
#define ARAD_PP_FRWRD_MACT_ADD_TYPE_TRANSPLANT               SOC_PPC_FRWRD_MACT_ADD_TYPE_TRANSPLANT
#define ARAD_PP_NOF_FRWRD_MACT_ADD_TYPES                     SOC_PPC_NOF_FRWRD_MACT_ADD_TYPES
typedef SOC_PPC_FRWRD_MACT_ADD_TYPE                            ARAD_PP_FRWRD_MACT_ADD_TYPE;

#define ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_NONE         SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_NONE
#define ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE       SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE
#define ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE       SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE
#define ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT        SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT
#define ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE     SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_RETRIEVE
#define ARAD_PP_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES         SOC_PPC_NOF_FRWRD_MACT_TRAVERSE_ACTION_TYPES
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE                ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE;

#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_NONE        SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_NONE
#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE   SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE
#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL
#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO
#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC
#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA     SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA
#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ALL         SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ALL
#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED
#define ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_GROUP    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_GROUP
#define ARAD_PP_NOF_FRWRD_MACT_TRAVERSE_MATCH_SELECTS        SOC_PPC_NOF_FRWRD_MACT_TRAVERSE_MATCH_SELECTS_ARAD
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT               ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT;

#define ARAD_PP_FRWRD_MACT_TRAVERSE_STATE_NORMAL               SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_NORMAL
#define ARAD_PP_FRWRD_MACT_TRAVERSE_STATE_AGGREGATE            SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_AGGREGATE
#define ARAD_PP_FRWRD_MACT_TRAVERSE_STATE_RUN                  SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_RUN
#define ARAD_PP_FRWRD_MACT_TRAVERSE_STATE_RESET                SOC_PPC_FRWRD_MACT_TRAVERSE_STATE_RESET
#define ARAD_PP_NOF_FRWRD_MACT_TRAVERSE_STATES                 SOC_PPC_NOF_FRWRD_MACT_TRAVERSE_STATES
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_STATE                      ARAD_PP_FRWRD_MACT_TRAVERSE_STATE;

#define ARAD_PP_FRWRD_MACT_TABLE_SW_ONLY                       SOC_PPC_FRWRD_MACT_TABLE_SW_ONLY
#define ARAD_PP_FRWRD_MACT_TABLE_SW_HW                         SOC_PPC_FRWRD_MACT_TABLE_SW_HW
#define ARAD_PP_FRWRD_MACT_TABLE_TYPE_LAST                     SOC_PPC_FRWRD_MACT_TABLE_TYPE_LAST
typedef SOC_PPC_FRWRD_MACT_TABLE_TYPE                          ARAD_PP_FRWRD_MACT_TABLE_TYPE;

typedef SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR                  ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR;
typedef SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC                   ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC;
typedef SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL                       ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL;
typedef SOC_PPC_FRWRD_MACT_ENTRY_KEY                           ARAD_PP_FRWRD_MACT_ENTRY_KEY;
typedef SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO                    ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO;
typedef SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO                    ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO;
typedef SOC_PPC_FRWRD_MACT_ENTRY_VALUE_SLB                     ARAD_PP_FRWRD_MACT_ENTRY_VALUE_SLB;
typedef SOC_PPC_FRWRD_MACT_ENTRY_VALUE                         ARAD_PP_FRWRD_MACT_ENTRY_VALUE;
typedef SOC_PPC_FRWRD_MACT_TIME                                ARAD_PP_FRWRD_MACT_TIME;
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC         ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC;
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC     ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC;
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE             ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE;
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE           ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE;
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE                 ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE;
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION                     ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION;
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO                ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO;
typedef SOC_PPC_FRWRD_MACT_TRAVERSE_MODE_INFO                  ARAD_PP_FRWRD_MACT_TRAVERSE_MODE_INFO;

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
 *   arad_pp_frwrd_mact_app_to_prefix_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get prefix value base application id.
 *
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                                  app_id -
 *     application ID
 *   SOC_SAND_OUT uint32                                     *prefix -
 *     prefix value base application ID
 *     
 * REMARKS:
 *   - 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_app_to_prefix_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        app_id,
    SOC_SAND_OUT uint32                        *prefix);

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_mact_entry_pack
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Pack a L2 entry from PPC MAC entry to a buffer(CPU request request register format) .
 *
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                                  insert -
 *     Describe whether it's add/delete a entry to/from MACT table.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ADD_TYPE                 add_type -
 *     Describe how this entry is added (insert/transplant).
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key -
 *     The MACT key, e.g. (FID, MAC address).
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value -
 *     The value of the MACT entry including forwarding and
 *     aging information.
 *   SOC_SAND_OUT uint32                                     *data -
 *     buffer store packed register value
 *   SOC_SAND_OUT uint32                                     *data_len -
 *     the length for register value
 *     
 * REMARKS:
 *   - The key can be (FID, DIP) for IPv4 compatible MC
 *   packets.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_entry_pack(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint8                                       insert,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *data,
    SOC_SAND_OUT uint8                                       *data_len
  );


/*********************************************************************
* NAME:
 *   arad_pp_frwrd_mact_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the MAC table. Given a key, e.g. (FID,
 *   MAC), the packets associated with this FID and having
 *   this MAC address as DA will be processed and forwarded
 *   according to the given value.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ADD_TYPE                 add_type -
 *     Describe how this entry is added (insert/learn/refresh).
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key -
 *     The MACT key, e.g. (FID, MAC address).
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value -
 *     The value of the MACT entry including forwarding and
 *     aging information.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     If True, then the entry was inserted in the MAC Table
 *     correctly. Otherwise, an hardware error happened, for
 *     example if the MAC Table is full
 *     (SOC_SAND_FAILURE_OUT_OF_RESOURCES), or the limit per FID is
 *     reached (SOC_SAND_FAILURE_OUT_OF_RESOURCES_2), or an illegal
 *     process happened (e.g., an unknown FID, the request
 *     could not be sent, SOC_SAND_FAILURE_OUT_OF_RESOURCES_3). If
 *     a modification of a static entry is tried,
 *     SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST is returned.
 * REMARKS:
 *   - The key can be (FID, DIP) for IPv4 compatible MC
 *   packets.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_mact_entry_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the MAC table according to the
 *   given Key.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key -
 *     The key (FID, MAC address) of the entry to delete.
 *   SOC_SAND_IN  int                               sw_only -
 *     if sw_only!=0 delete the entry only from the shadow DB in the SW; otherwise delete from the HW and SW DBs
 * REMARKS:
 *   - key can be (FID, DIP) for IPv4 compatible MC packets.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  int                                sw_only
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_mact_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry according to its key, e.g. (FID, MAC
 *   address).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key -
 *     The MACT key (FID, MAC address).
 *   SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value -
 *     The value of the MACT entry including forwarding and
 *     aging information.
 *   SOC_SAND_OUT uint8                                 *found -
 *     If TRUE, then the key exists in the MAC Table and the
 *     corresponding value is returned. Otherwise, the key does
 *     not exist and 'mac_entry_value' is not affected
 * REMARKS:
 *   - key can be (FID, DIP) for IPv4 compatible MC packets.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                                 *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_mact_traverse
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the MACT entries. Compare each entry to a given
 *   rule, and for matching entries perform the given action.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule -
 *     Rule against which the MACT entries are compared.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION          *action -
 *     Action to perform for the matching matched entries
 *     (remove, modify, none).
 *   SOC_SAND_IN  uint8                                 wait_till_finish -
 *     If TRUE, then the function returns only when the
 *     traverse operation is terminated; otherwise, it returns
 *     immediately.
 *   SOC_SAND_OUT uint32                                  *nof_matched_entries -
 *     Number of entries in the MACT that matched the given
 *     rule. Valid only if wait_till_finish is TRUE.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_traverse(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_mact_traverse_by_mac_limit_per_tunnel
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the MACT entries when MAC limit per tunnel is enabled.
 *   Compare each entry to a given rule, and for matching entries
 *   perform the given action.
 *
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule -
 *     Rule against which the MACT entries are compared.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION          *action -
 *     Action to perform for the matching matched entries
 *     (remove, modify, none).
 *   SOC_SAND_OUT uint32                                  *nof_matched_entries -
 *     Number of entries in the MACT that matched the given
 *     rule. Valid only if wait_till_finish is TRUE.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_traverse_by_mac_limit_per_tunnel(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,   
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_mact_traverse_status_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the status of the traverse, including which
 *   action is performed and according to what rule, besides
 *   the expected time to finish the traverse and the number
 *   of matched entries if the traverse was finished.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status -
 *     The status of the traverse operation and its possible
 *     output if the traverse was finished.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_traverse_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  );


uint32
  arad_pp_frwrd_mact_traverse_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN ARAD_PP_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  );

uint32
  arad_pp_frwrd_mact_traverse_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  );


/*********************************************************************
* NAME:
 *   arad_pp_frwrd_mact_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the MAC Table entries (in specified range), and
 *   retrieve entries that match the given rule.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule -
 *     Rule to compare the MACT entries against.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Block range in the MACT.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mact_keys -
 *     Array to include MACT keys.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mact_vals -
 *     Array to include MACT entries values.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries in mact_key and mact_val.
 * REMARKS:
 *   - mact_keys and mact_vals should point to an allocated
 *   memory of 'entries_to_act' entries. - To set the
 *   iterator to point to the start of the MAC table, use
 *   soc_sand_SAND_TABLE_BLOCK_RANGE_clear(). - To check if the
 *   returned iterator points to the end of the table, use
 *   soc_sand_SAND_TABLE_BLOCK_RANGE_is_end()- Example of use: To
 *   get the whole MACT table using this API:
 *   SOC_SAND_TABLE_BLOCK_RANGE block_range;
 *   SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE rule;
 *   SOC_PPD_FRWRD_MACT_ENTRY_KEY mac_keys[100];
 *   SOC_PPD_FRWRD_MACT_ENTRY_VALUE mac_vals[100];
 *   soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
 *   block_range.entries_to_act = 100;
 *   block_range.entries_to_scan = 16K; do{
 *   soc_ppd_frwrd_mact_get_block( &rule, &block_range,mac_keys,
 *   mac_vals,&nof_entries); // do something with the
 *   entries. }
 *   while(!soc_sand_SAND_TABLE_BLOCK_RANGE_is_end(&block_range))
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_mact_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN ARAD_PP_FRWRD_MACT_TABLE_TYPE               mact_type,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  );


/***************************************************************************
 *      Reset the flush machine's rules, current_index and pause registers
****************************************************************************/
void arad_pp_frwrd_mact_clear_flush_operation(SOC_SAND_IN  int unit);

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_VALUE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_VALUE *info
  );

void
  ARAD_PP_FRWRD_MACT_TIME_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TIME *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_FRWRD_MACT_KEY_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_KEY_TYPE enum_val
  );

const char*
  ARAD_PP_FRWRD_MACT_ADD_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ADD_TYPE enum_val
  );

const char*
  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE enum_val
  );

const char*
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT enum_val
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  );

void
  ARAD_PP_FRWRD_MACT_ENTRY_VALUE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_VALUE *info
  );

void
  ARAD_PP_FRWRD_MACT_TIME_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TIME *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION *info
  );

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_FRWRD_MACT_INCLUDED__*/
#endif


