/* $Id: pb_pp_lif_table.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_lif_table.h
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

#ifndef __SOC_PB_PP_LIF_TABLE_INCLUDED__
/* { */
#define __SOC_PB_PP_LIF_TABLE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_lif_table.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

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
  SOC_PB_PP_LIF_TABLE_GET_BLOCK = SOC_PB_PP_PROC_DESC_BASE_LIF_TABLE_FIRST,
  SOC_PB_PP_LIF_TABLE_GET_BLOCK_PRINT,
  SOC_PB_PP_LIF_TABLE_GET_BLOCK_UNSAFE,
  SOC_PB_PP_LIF_TABLE_GET_BLOCK_VERIFY,
  SOC_PB_PP_LIF_TABLE_ENTRY_GET,
  SOC_PB_PP_LIF_TABLE_ENTRY_GET_PRINT,
  SOC_PB_PP_LIF_TABLE_ENTRY_GET_UNSAFE,
  SOC_PB_PP_LIF_TABLE_ENTRY_GET_VERIFY,
  SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE,
  SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_PRINT,
  SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_UNSAFE,
  SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_VERIFY,
  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_GET,
  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_GET_UNSAFE,
  SOC_PB_PP_LIF_TABLE_GET_PROCS_PTR,
  SOC_PB_PP_LIF_TABLE_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LIF_TABLE_PROCEDURE_DESC_LAST
} SOC_PB_PP_LIF_TABLE_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_LIF_TABLE_TYPE_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_LIF_TABLE_FIRST,
  SOC_PB_PP_LIF_TABLE_ENTRIES_TYPE_BM_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_TYPE_MISMATCH_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LIF_TABLE_ERR_LAST
} SOC_PB_PP_LIF_TABLE_ERR;

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
  soc_pb_pp_lif_table_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );
 
/*********************************************************************
* NAME:
 *   soc_pb_pp_lif_table_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the LIF Table entries (in specified range), and
 *   retrieve entries that match the given rule.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE         *rule -
 *     Rule to compare the LIF table entries against.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Block range in the MACT.
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO                      *entries_array -
 *     Logical interfaces info
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries in 'entries_array'.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_lif_table_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO                      *entries_array,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_lif_table_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_lif_table_entry_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable updating LIF table entry attribute, without
 *   accessing the LIF KEY.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx -
 *     Lif entry index
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO                      *lif_entry_info -
 *     LIF entry attributes, according to LIF type.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO                      *lif_entry_info
  );

uint32
  soc_pb_pp_lif_table_entry_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_lif_table_entry_update_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable updating LIF table entry attribute, without
 *   accessing the LIF KEY.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx -
 *     Lif entry index
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO                      *lif_entry_info -
 *     LIF entry attributes, according to LIF type.
 * REMARKS:
 *   Advanced function. Straitforward usage of the LIF table
 *   is adding or removing the interface according to its
 *   key.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_update_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO                      *lif_entry_info
  );

uint32
  soc_pb_pp_lif_table_entry_update_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO                      *lif_entry_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_lif_table_entry_accessed_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   check if entry was accessed upon packet lookup
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx -
 *     Lif entry index
 *   SOC_SAND_IN  uint8                               clear_access_stat -
 *     clear on read
 *   SOC_SAND_OUT  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO       *accessed_info -
 *     was accessed
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_accessed_info_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx,
    SOC_SAND_IN  uint8                               clear_access_stat,
    SOC_SAND_OUT  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO       *accessed_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_lif_table_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_lif_table module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lif_table_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_lif_table_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_lif_table module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lif_table_get_errs_ptr(void);

uint32
  SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO *info,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_TYPE             type
  );

uint32
  SOC_PB_PP_LIF_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO *info
  );

uint32
  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_LIF_TABLE_INCLUDED__*/
#endif

