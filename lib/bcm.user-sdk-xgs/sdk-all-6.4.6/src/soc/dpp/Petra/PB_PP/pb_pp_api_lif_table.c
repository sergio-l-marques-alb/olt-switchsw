/* $Id: pb_pp_api_lif_table.c,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_lif_table.c
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_lif_table.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_table.h>

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
*     Traverse the LIF Table entries (in specified range), and
 *     retrieve entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_table_get_block(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE             *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                        *block_range,
    SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO                          *entries_array,
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = soc_pb_pp_lif_table_get_block_verify(
          unit,
          rule,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_lif_table_get_block_unsafe(
          unit,
          rule,
          block_range,
          entries_array,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_get_block()", 0, 0);
}

/*********************************************************************
*     Enable updating LIF table entry attribute, without
 *     accessing the LIF KEY.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  res = soc_pb_pp_lif_table_entry_get_verify(
          unit,
          lif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_lif_table_entry_get_unsafe(
          unit,
          lif_ndx,
          lif_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_entry_get()", lif_ndx, 0);
}

/*********************************************************************
*     Enable updating LIF table entry attribute, without
 *     accessing the LIF KEY.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_update(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                   lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  res = soc_pb_pp_lif_table_entry_update_verify(
          unit,
          lif_ndx,
          lif_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_lif_table_entry_update_unsafe(
          unit,
          lif_ndx,
          lif_entry_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_entry_update()", lif_ndx, 0);
}

/*********************************************************************
*     Enable updating LIF table entry attribute, without
 *     accessing the LIF KEY.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_accessed_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                   lif_ndx,
    SOC_SAND_IN  uint8                                      clear_access_stat,
    SOC_SAND_OUT  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO           *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_lif_table_entry_accessed_info_get_unsafe(
          unit,
          lif_ndx,
          clear_access_stat,
          accessed_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_entry_accessed_info_get()", lif_ndx, 0);
}

void
  SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LIF_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_LIF_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_TYPE enum_val
  )
{
  return SOC_PPC_LIF_ENTRY_TYPE_to_string(enum_val);
}

void
  SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO *info,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_TYPE  type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_print(info,type);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LIF_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

