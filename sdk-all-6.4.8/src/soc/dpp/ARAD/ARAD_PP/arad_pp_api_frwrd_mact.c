
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_frwrd_mact.c,v 1.13 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>

#include <soc/hwstate/hw_log.h>
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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */
/*********************************************************************
*     Get prefix value base application id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_mact_app_to_prefix_get(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                      app_id,
    SOC_SAND_OUT uint32                                      *prefix
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prefix);

  res = arad_pp_frwrd_mact_app_to_prefix_get_unsafe(
          unit,
          app_id,
          prefix);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_app_to_prefix_get()", 0, 0);
}

/*********************************************************************
*     Pack a L2 entry from PPC MAC entry to a 
 *     buffer(CPU request request register format).
 *     Details: in the H file. (search for prototype)
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
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_PACK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(data_len);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  res = arad_pp_frwrd_mact_entry_pack_verify(
            unit,
            mac_entry_key,
            mac_entry_value
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_entry_pack_unsafe(
          unit,
          insert,
          add_type,
          mac_entry_key,
          mac_entry_value,
          data,
          data_len);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_add()", 0, 0);
}


/*********************************************************************
*     Add an entry to the MAC table. Given a key, e.g. (FID,
 *     MAC), the packets associated with this FID and having
 *     this MAC address as DA will be processed and forwarded
 *     according to the given value.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_mact_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_frwrd_mact_entry_add_verify(
          unit,
          add_type,
          mac_entry_key,
          mac_entry_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_entry_add_unsafe(
          unit,
          add_type,
          mac_entry_key,
          mac_entry_value,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_add()", 0, 0);
}

/*********************************************************************
*     Remove an entry from the MAC table according to the
 *     given Key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_mact_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  int                                sw_only
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);

  res = arad_pp_frwrd_mact_entry_remove_verify(
          unit,
          mac_entry_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_entry_remove_unsafe(
          unit,
          mac_entry_key,
          sw_only
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_remove()", 0, 0);
}

/*********************************************************************
*     Get an entry according to its key, e.g. (FID, MAC
 *     address).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_mact_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_frwrd_mact_entry_get_verify(
          unit,
          mac_entry_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_entry_get_unsafe(
          unit,
          mac_entry_key,
          mac_entry_value,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_entry_get()", 0, 0);
}

/*********************************************************************
*     Traverse the MACT entries when MAC limit per tunnel is enabled. 
 *     Compare each entry to a given rule, and for matching entries
 *     perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_mact_traverse_by_mac_limit_per_tunnel(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_TABLE_BLOCK_RANGE              
      block_range;
  ARAD_PP_LEM_ACCESS_KEY                  
      rule_key;
  ARAD_PP_LEM_ACCESS_KEY                  
      rule_key_mask;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

  res = arad_pp_frwrd_mact_traverse_verify(
          unit,
          rule,
          action,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /* default scan from beginning and scan all */
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
  block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
  /*
   *	Get the rule parameters in the internal form
   */
  res = arad_pp_frwrd_mact_rule_to_key_convert(
          unit,
          rule,
          &rule_key,
          &rule_key_mask,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit_semaphore);

  res = arad_pp_frwrd_mact_traverse_by_mac_limit_per_tunnel_internal_unsafe(
          unit,
          &rule_key,
          &rule_key_mask,
          &rule->value_rule,
          &block_range,
          action,
          nof_matched_entries
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_by_mac_limit_per_tunnel()", 0, 0);
}


/*********************************************************************
*     Traverse the MACT entries. Compare each entry to a given
 *     rule, and for matching entries perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_mact_traverse(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                                 wait_till_finish,
    SOC_SAND_OUT uint32                                  *nof_matched_entries
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_TABLE_BLOCK_RANGE              
      block_range;
  ARAD_PP_LEM_ACCESS_KEY                  
      rule_key;
  ARAD_PP_LEM_ACCESS_KEY                  
      rule_key_mask;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

  res = arad_pp_frwrd_mact_traverse_verify(
          unit,
          rule,
          action,
          wait_till_finish
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /* default scan from beginning and scan all */
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
  block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
  /*
   *	Get the rule parameters in the internal form
   */
  res = arad_pp_frwrd_mact_rule_to_key_convert(
          unit,
          rule,
          &rule_key,
          &rule_key_mask,
          0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit_semaphore);

  res = arad_pp_frwrd_mact_traverse_internal_unsafe(
          unit,
          &rule_key,
          &rule_key_mask,
          &rule->value_rule,
          &block_range,
          action,
          wait_till_finish,
          nof_matched_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse()", 0, 0);
}

/*********************************************************************
*     Returns the status of the traverse, including which
 *     action is performed and according to what rule, besides
 *     the expected time to finish the traverse and the number
 *     of matched entries if the traverse was finished.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_mact_traverse_status_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  /*
   *	No verify function is needed.
   */

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_traverse_status_get_unsafe(
          unit,
          status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_status_get()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_traverse_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN ARAD_PP_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET);

  if (SOC_IS_DONE_INIT(unit))
  {
      if (BCM_UNIT_DO_HW_READ_WRITE(unit))
      {
          SOC_SAND_EXIT_AND_SEND_ERROR("WARNING: in arad_pp_frwrd_mact_traverse_mode_info_set() - HA HW Log mode is not implemented for this function", 0, 0);
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FEATURE_NOT_SUPPORTED_ERR, 8050, exit);
      }
  }

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  /*
   *	No verify function is needed.
   */

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_traverse_mode_info_set_unsafe(
          unit,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_mode_info_set()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_traverse_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  /*
   *	No verify function is needed.
   */

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_frwrd_mact_traverse_mode_info_get_unsafe(
          unit,
          mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_traverse_mode_info_get()", 0, 0);
}


/*********************************************************************
*     Traverse the MAC Table entries (in specified range), and
 *     retrieve entries that match the given rule.
 *     Details: in the H file. (search for prototype)
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
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(mact_keys);
  SOC_SAND_CHECK_NULL_INPUT(mact_vals);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_frwrd_mact_get_block_verify(
          unit,
          rule,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  if ((mact_type == ARAD_PP_FRWRD_MACT_TABLE_SW_ONLY) || (SAL_BOOT_PLISIM)) {
      res = arad_pp_frwrd_mact_get_sw_block_unsafe(
              unit,
              rule,
              block_range,
              mact_keys,
              mact_vals,
              nof_entries
            );
  }
  else
  {
      res = arad_pp_frwrd_mact_get_block_unsafe(
              unit,
              rule,
              block_range,
              mact_keys,
              mact_vals,
              nof_entries
            );
  }

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_get_block()", 0, 0);
}


/***************************************************************************
 *      Reset the flush machine's rules, current_index and pause registers
****************************************************************************/
void arad_pp_frwrd_mact_clear_flush_operation(SOC_SAND_IN  int unit)
{
    uint32 res = SOC_SAND_OK;
    uint32 entry_offset;
    ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA tbl_data;
    soc_field_t flu_machine_pause = SOC_IS_JERICHO(unit) ? LARGE_EM_FLU_MACHINE_PAUSEf : MACT_FLU_MACHINE_PAUSEf; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* now that flush is done invalidate flush db all entries */
    for (entry_offset = 0; entry_offset < ARAD_PP_MACT_NOF_FLUSH_ENTRIES; entry_offset++) {
        ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(&tbl_data);
        tbl_data.compare_valid = 0;
        res = arad_pp_ihp_flush_db_tbl_set_unsafe(
            unit,
            entry_offset,
            &tbl_data
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
    }


    /* Set the current index to 0 */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0, LARGE_EM_FLU_MACHINE_CURRENT_INDEXf,  0));

    /* Cancel the pause flush machine */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, flu_machine_pause,  0));

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_clear_flush_operation()", 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_VALUE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TIME_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TIME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TIME_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_FRWRD_MACT_KEY_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_KEY_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_KEY_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_FRWRD_MACT_ADD_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ADD_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_ADD_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT_to_string(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_SELECT enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_to_string(enum_val);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_KEY_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_ENTRY_VALUE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TIME_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TIME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TIME_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO_print(
    SOC_SAND_IN  ARAD_PP_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

