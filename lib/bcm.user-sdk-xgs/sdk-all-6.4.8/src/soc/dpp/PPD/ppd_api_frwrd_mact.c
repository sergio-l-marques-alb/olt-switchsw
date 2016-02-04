/* $Id: ppd_api_frwrd_mact.c,v 1.18 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_frwrd_mact.c
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
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_mact.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_frwrd_mact.h>
#endif
#ifdef LINK_PCP_LIBRARIES
  #include <soc/dpp/PCP/pcp_api_frwrd_mact.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_mact.h>
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
    Ppd_procedure_desc_element_frwrd_mact[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_ENTRY_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_ENTRY_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_ENTRY_PACK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_ENTRY_PACK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAVERSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAVERSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_PRINT_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_PRINT_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRWRD_MACT_GET_PROCS_PTR),
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
*     Get prefix value base application id.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_app_to_prefix_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        app_id,
    SOC_SAND_OUT uint32                        *prefix)

{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prefix);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_app_to_prefix_get,(unit, app_id, prefix));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_app_to_prefix_get()", 0, 0);
}


/*********************************************************************
*     Add an entry to the MAC table. Given a key, e.g. (FID,
 *     MAC), the packets associated with this FID and having
 *     this MAC address as DA will be processed and forwarded
 *     according to the given value.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_entry_pack(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint8                                       insert,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint32                                      *data,
    SOC_SAND_OUT uint8                                       *data_len
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_ENTRY_PACK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(data);
  SOC_SAND_CHECK_NULL_INPUT(data_len);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_entry_pack,(unit, insert, add_type, mac_entry_key, mac_entry_value, data, data_len));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_entry_pack_print,(unit,insert,add_type, mac_entry_key,mac_entry_value));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_pack()", 0, 0);
}


/*********************************************************************
*     Add an entry to the MAC table. Given a key, e.g. (FID,
 *     MAC), the packets associated with this FID and having
 *     this MAC address as DA will be processed and forwarded
 *     according to the given value.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_ENTRY_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_entry_add,(unit, add_type, mac_entry_key, mac_entry_value, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_entry_add_print,(unit,add_type,mac_entry_key,mac_entry_value));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_add()", 0, 0);
}

/*********************************************************************
*     Remove an entry from the MAC table according to the
 *     given Key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  int                                sw_only
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_ENTRY_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_entry_remove,(unit, mac_entry_key, sw_only));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_entry_remove_print,(unit, mac_entry_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_remove()", 0, 0);
}

/*********************************************************************
*     Get an entry according to its key, e.g. (FID, MAC
 *     address).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value,
    SOC_SAND_OUT uint8                               *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_entry_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_entry_value);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_entry_get,(unit, mac_entry_key, mac_entry_value, found));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_entry_get_print,(unit,mac_entry_key));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_entry_get()", 0, 0);
}

/*********************************************************************
*     Traverse the MACT entries when MAC limit per tunnel is enabled. 
 *     Compare each entry to a given rule, and for matching entries
 *     perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_traverse_by_mac_limit_per_tunnel(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_OUT uint32                                *nof_matched_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_TRAVERSE_BY_MAC_LIMIT_PER_TUNNEL);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_traverse_by_mac_limit_per_tunnel,(unit, rule, action, nof_matched_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_traverse_print,(unit,rule,action, 1));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse_by_mac_limit_per_tunnel()", 0, 0);
}


/*********************************************************************
*     Traverse the MACT entries. Compare each entry to a given
 *     rule, and for matching entries perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_traverse(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                               wait_till_finish,
    SOC_SAND_OUT uint32                                *nof_matched_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_TRAVERSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(action);
  SOC_SAND_CHECK_NULL_INPUT(nof_matched_entries);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_traverse,(unit, rule, action, wait_till_finish, nof_matched_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_traverse_print,(unit,rule,action,wait_till_finish));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse()", 0, 0);
}

/*********************************************************************
*     Returns the status of the traverse, including which
 *     action is performed and according to what rule, besides
 *     the expected time to finish the traverse and the number
 *     of matched entries if the traverse was finished.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_traverse_status_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_INFO     *status
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_PPD_ARAD_TMP_FRWRD_DEVICE_CALL(frwrd_mact_traverse_status_get,(unit, status));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_traverse_status_get_print,(unit));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse_status_get()", 0, 0);
}

uint32
  soc_ppd_frwrd_mact_traverse_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN SOC_PPD_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mode);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_traverse_mode_info_set,(unit, mode));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse_mode_info_set()", 0, 0);
}


uint32
  soc_ppd_frwrd_mact_traverse_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_MODE_INFO  *mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mode);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_traverse_mode_info_get,(unit, mode));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_traverse_mode_info_get()", 0, 0);
}

/*********************************************************************
*     Traverse the MAC Table entries (in specified range), and
 *     retrieve entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN SOC_PPD_FRWRD_MACT_TABLE_TYPE               mact_type,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_OUT uint32                                *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_FRWRD_MACT_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(mact_keys);
  SOC_SAND_CHECK_NULL_INPUT(mact_vals);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPD_ARAD_ONLY_DEVICE_CALL(frwrd_mact_get_block, (unit, rule, mact_type, block_range, mact_keys, mact_vals, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_get_block_print,(unit,rule,block_range));
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_frwrd_mact_get_block()",0,0);
}
#if SOC_PPD_DEBUG_IS_LVL1
/*********************************************************************
*     Print MAC Table entries in the table format.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_print_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_IN  uint32                                nof_entries,
    SOC_SAND_IN  uint32                                flavors
  )
{
  uint32
    indx;
  SOC_SAND_PP_IPV4_ADDRESS
    ipv4_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(mact_keys);
  SOC_SAND_CHECK_NULL_INPUT(mact_vals);
  soc_sand_SAND_PP_IPV4_ADDRESS_clear(&ipv4_addr);

  LOG_CLI((BSL_META_U(unit,
                      " ----------------------------------------------------------------------------------\n\r"
                      "|                                      MAC Table                                   |\n\r"
                      "|----------------------------------------------------------------------------------|\n\r")));

  LOG_CLI((BSL_META_U(unit,
                      "|      |                   |   |SA |grp| Destination  |     Additional Info        |\n\r"
                      "| FID  |      MAC/IP       |Dyn|Drp|   | Type  | Val  |Type |                      |\n\r"
                      " ---------------------------------------------------------------------------------- \n\r")));

  for (indx = 0; indx < nof_entries; ++indx)
  {
    if (mact_keys[indx].key_type == SOC_PPD_FRWRD_MACT_KEY_TYPE_MAC_ADDR)
    {
      LOG_CLI((BSL_META_U(unit,
                          "| %-5d| "), mact_keys[indx].key_val.mac.fid));
      soc_sand_SAND_PP_MAC_ADDRESS_print((&mact_keys[indx].key_val.mac.mac));
    }
    else if (mact_keys[indx].key_type == SOC_PPD_FRWRD_MACT_KEY_TYPE_IPV4_MC)
    {
      LOG_CLI((BSL_META_U(unit,
                          "| %-5d| "), mact_keys[indx].key_val.ipv4_mc.fid));
      /* LOG_CLI((BSL_META_U(unit,
                             "  %-15d"), mact_keys[indx].key_val.ipv4_mc.dip)); */
      ipv4_addr.address[0] = mact_keys[indx].key_val.ipv4_mc.dip;
      soc_sand_SAND_PP_IPV4_ADDRESS_print(&ipv4_addr);
      LOG_CLI((BSL_META_U(unit,
                          "         ")));
    }
    LOG_CLI((BSL_META_U(unit,
                        " |")));

    LOG_CLI((BSL_META_U(unit,
                        " %s |"), (mact_vals[indx].aging_info.is_dynamic)?"V":"X"));
    LOG_CLI((BSL_META_U(unit,
                        " %s |"), (mact_vals[indx].frwrd_info.drop_when_sa_is_known)?"V":"X"));
    if (mact_vals[indx].group == 0) {
        LOG_CLI((BSL_META_U(unit,
                            " - |")));
    }
    else {
        LOG_CLI((BSL_META_U(unit,
                            " %d |"), mact_vals[indx].group));
    }

    SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
      "",
      &(mact_vals[indx].frwrd_info.forward_decision)
    );

    LOG_CLI((BSL_META_U(unit,
                        "|\n\r")));
  }

  LOG_CLI((BSL_META_U(unit,
                      " ----------------------------------------------------------------------------- \n\r"
               )));
  
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_frwrd_mact_print_block_print,(unit,mact_keys,mact_vals,nof_entries,flavors));
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_frwrd_mact_print_block()",0,0);
}
#endif
/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_frwrd_mact module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_mact_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_frwrd_mact;
}
void
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_IPV4_MC_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_VAL_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_KEY_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(info);
  info->key_type = SOC_PPD_FRWRD_MACT_KEY_TYPE_MAC_ADDR;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_AGING_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_FRWRD_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_VALUE_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TIME_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TIME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TIME_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_TRAVERSE_MODE_INFO *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPD_FRWRD_MACT_TRAVERSE_MODE_INFO));
    SOC_SAND_MAGIC_NUM_SET;
  exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_FRWRD_MACT_KEY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_KEY_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_KEY_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_ADD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ADD_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_ADD_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_to_string(enum_val);
}

const char*
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_SELECT_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_SELECT enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_to_string(enum_val);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY_MAC_ADDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_MAC_ADDR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_IPV4_MC_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_IPV4_MC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_VAL_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_AGING_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_AGING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_FRWRD_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_FRWRD_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_FRWRD_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_ENTRY_VALUE_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_VALUE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TIME_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TIME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TIME_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_MAC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_IPV4_MC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_KEY_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_STATUS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_TRAVERSE_STATUS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3
void
  soc_ppd_frwrd_mact_entry_pack_print(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint8                                       insert,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "action = %s "), (insert ? "ADD" : "DELETE")));

  LOG_CLI((BSL_META_U(unit,
                      "mac_entry_key:")));
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_print((mac_entry_key));

  if (TRUE == insert)
  {
    LOG_CLI((BSL_META_U(unit,
                        "add_type %s "), SOC_PPD_FRWRD_MACT_ADD_TYPE_to_string(add_type)));
    LOG_CLI((BSL_META_U(unit,
                        "mac_entry_value:")));
    SOC_PPD_FRWRD_MACT_ENTRY_VALUE_print((mac_entry_value));
  }

  return;
}

void
  soc_ppd_frwrd_mact_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ADD_TYPE                 add_type,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_entry_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mac_entry_value
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "add_type %s "), SOC_PPD_FRWRD_MACT_ADD_TYPE_to_string(add_type)));

  LOG_CLI((BSL_META_U(unit,
                      "mac_entry_key:")));
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_print((mac_entry_key));

  LOG_CLI((BSL_META_U(unit,
                      "mac_entry_value:")));
  SOC_PPD_FRWRD_MACT_ENTRY_VALUE_print((mac_entry_value));

  return;
}
void
  soc_ppd_frwrd_mact_entry_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_entry_key:")));
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_print((mac_entry_key));

  return;
}
void
  soc_ppd_frwrd_mact_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mac_entry_key
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mac_entry_key:")));
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_print((mac_entry_key));

  return;
}
void
  soc_ppd_frwrd_mact_traverse_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION          *action,
    SOC_SAND_IN  uint8                               wait_till_finish
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "rule:")));
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE_print((rule));

  LOG_CLI((BSL_META_U(unit,
                      "action:")));
  SOC_PPD_FRWRD_MACT_TRAVERSE_ACTION_print((action));

  LOG_CLI((BSL_META_U(unit,
                      "wait_till_finish: %u\n\r"),wait_till_finish));

  return;
}
void
  soc_ppd_frwrd_mact_traverse_status_get_print(
    SOC_SAND_IN  int                               unit
  )
{

  return;
}
void
  soc_ppd_frwrd_mact_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE      *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "rule:")));
  SOC_PPD_FRWRD_MACT_TRAVERSE_MATCH_RULE_print((rule));

  LOG_CLI((BSL_META_U(unit,
                      "block_range:")));
  soc_sand_SAND_TABLE_BLOCK_RANGE_print((block_range));

  return;
}
void
  soc_ppd_frwrd_mact_print_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_KEY                *mact_keys,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_ENTRY_VALUE              *mact_vals,
    SOC_SAND_IN  uint32                                nof_entries,
    SOC_SAND_IN  uint32                                flavors
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "mact_keys:")));
  SOC_PPD_FRWRD_MACT_ENTRY_KEY_print((mact_keys));

  LOG_CLI((BSL_META_U(unit,
                      "mact_vals:")));
  SOC_PPD_FRWRD_MACT_ENTRY_VALUE_print((mact_vals));

  LOG_CLI((BSL_META_U(unit,
                      "nof_entries: %lu\n\r"),nof_entries));

  LOG_CLI((BSL_META_U(unit,
                      "flavors: %lu\n\r"),flavors));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

