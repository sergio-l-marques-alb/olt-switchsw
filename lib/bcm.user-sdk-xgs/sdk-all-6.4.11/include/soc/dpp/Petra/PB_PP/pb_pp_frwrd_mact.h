/* $Id: pb_pp_frwrd_mact.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_frwrd_mact.h
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

#ifndef __SOC_PB_PP_FRWRD_MACT_INCLUDED__
/* { */
#define __SOC_PB_PP_FRWRD_MACT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_mact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FRWRD_MACT_TABLE_NOF_ENTRIES                     SOC_PB_PP_LEM_NOF_ENTRIES
#define SOC_PB_PP_FRWRD_MACT_TABLE_ENTRY_MAX                       (SOC_PB_PP_FRWRD_MACT_TABLE_NOF_ENTRIES - 1)
#define SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_AGE_STATUS_MAX              (6)

#define SOC_PB_PP_FRWRD_MACT_NOF_MS_IN_SEC                         (1000)
#define SOC_PB_PP_FRWRD_MACT_NOF_MICROSEC_IN_MS                    (1000)

#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LSB     (19)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_MSB     (19)
#define SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LEN     (SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_MSB - SOC_PB_PP_LEM_ACCESS_ASD_SA_AUTH_ACCPT_UNTGD_LSB + 1)


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
  SOC_PB_PP_FRWRD_MACT_ENTRY_ADD = SOC_PB_PP_PROC_DESC_BASE_FRWRD_MACT_FIRST,
  SOC_PB_PP_FRWRD_MACT_ENTRY_ADD_PRINT,
  SOC_PB_PP_FRWRD_MACT_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_ENTRY_ADD_VERIFY,
  SOC_PB_PP_FRWRD_MACT_ENTRY_REMOVE,
  SOC_PB_PP_FRWRD_MACT_ENTRY_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_MACT_ENTRY_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_ENTRY_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_MACT_ENTRY_GET,
  SOC_PB_PP_FRWRD_MACT_ENTRY_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_ENTRY_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_ENTRY_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_TRAVERSE,
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_PRINT,
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_VERIFY,
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_STATUS_GET,
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_PRINT,
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_STATUS_GET_VERIFY,
  SOC_PB_PP_FRWRD_MACT_GET_BLOCK,
  SOC_PB_PP_FRWRD_MACT_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_MACT_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_MACT_PRINT_BLOCK,
  SOC_PB_PP_FRWRD_MACT_PRINT_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_MACT_PRINT_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_MACT_PRINT_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_MACT_GET_PROCS_PTR,
  SOC_PB_PP_FRWRD_MACT_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_FRWRD_MACT_KEY_CONVERT,
  SOC_PB_PP_FRWRD_MACT_PAYLOAD_MASK_BUILD,
  SOC_PB_PP_FRWRD_MACT_KEY_PARSE,
  SOC_PB_PP_FRWRD_MACT_PAYLOAD_CONVERT,
  SOC_PB_PP_FRWRD_MACT_RULE_TO_KEY_CONVERT,
  SOC_PB_PP_FRWRD_MACT_PAYLOAD_BUILD,
  SOC_PB_PP_FRWRD_MACT_PAYLOAD_MASK_RETRIEVE,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_MACT_PROCEDURE_DESC_LAST
} SOC_PB_PP_FRWRD_MACT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_MACT_ADD_TYPE_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FRWRD_MACT_FIRST,
  SOC_PB_PP_FRWRD_MACT_SUCCESS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_FLAVORS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_DIP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_KEY_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_AGE_STATUS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_SEC_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_MILI_SEC_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_DIP_MASK_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_COMPARE_MASK_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_UPDATE_MASK_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_NOF_MATCHED_ENTRIES_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_BLOCK_ENTRIES_TO_ACT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_BLOCK_ENTRIES_TO_SCAN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_BLOCK_RANGE_ITER_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_ACTION_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MACT_RETRIEVE_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_MACT_TABLE_TRAVERSE_WAITING_ERR,
  SOC_PB_PP_MACT_FWD_DECSION_DROP_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_MACT_ERR_LAST
} SOC_PB_PP_FRWRD_MACT_ERR;

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
  soc_pb_pp_frwrd_mact_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_entry_add_unsafe
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
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ADD_TYPE                 add_type -
 *     Describe how this entry is added (insert/learn/refresh).
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key -
 *     The MACT key, e.g. (FID, MAC address).
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value -
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
  soc_pb_pp_frwrd_mact_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_mact_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_entry_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the MAC table according to the
 *   given Key.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key -
 *     The key (FID, MAC address) of the entry to delete.
 * REMARKS:
 *   - key can be (FID, DIP) for IPv4 compatible MC packets.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  );

uint32
  soc_pb_pp_frwrd_mact_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry according to its key, e.g. (FID, MAC
 *   address).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key -
 *     The MACT key (FID, MAC address).
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value -
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
  soc_pb_pp_frwrd_mact_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_frwrd_mact_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_traverse_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the MACT entries. Compare each entry to a given
 *   rule, and for matching entries perform the given action.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule -
 *     Rule against which the MACT entries are compared.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_ACTION          *action -
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
  soc_pb_pp_frwrd_mact_traverse_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  );

uint32
  soc_pb_pp_frwrd_mact_traverse_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_traverse_status_get_unsafe
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
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status -
 *     The status of the traverse operation and its possible
 *     output if the traverse was finished.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_traverse_status_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the MAC Table entries (in specified range), and
 *   retrieve entries that match the given rule.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule -
 *     Rule to compare the MACT entries against.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Block range in the MACT.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mact_keys -
 *     Array to include MACT keys.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE              *mact_vals -
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
  soc_pb_pp_frwrd_mact_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_frwrd_mact_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );

uint32
  soc_pb_pp_frwrd_mact_key_parse(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY       *key,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_ENTRY_KEY *mac_entry_key
    );

uint32
  soc_pb_pp_frwrd_mact_payload_convert(
      SOC_SAND_IN  int                     unit,
      SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_PAYLOAD     *payload,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE *mac_entry_value
  );
/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_frwrd_mact module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_mact_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_frwrd_mact module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_mact_get_errs_ptr(void);

uint32
  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_VAL *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_ENTRY_AGING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_AGING_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_ENTRY_FRWRD_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_ENTRY_VALUE *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_TIME_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TIME *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_ACTION_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_ACTION *info
  );

uint32
  SOC_PB_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FRWRD_MACT_INCLUDED__*/
#endif

