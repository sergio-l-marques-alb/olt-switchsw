/* $Id: arad_pp_api_lif_table.h,v 1.4 Broadcom SDK $
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

#ifndef __ARAD_PP_API_LIF_TABLE_INCLUDED__
/* { */
#define __ARAD_PP_API_LIF_TABLE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_lif_table.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

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

#define ARAD_PP_LIF_ENTRY_TYPE_EMPTY                         SOC_PPC_LIF_ENTRY_TYPE_EMPTY
#define ARAD_PP_LIF_ENTRY_TYPE_AC                            SOC_PPC_LIF_ENTRY_TYPE_AC
#define ARAD_PP_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP             SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP
#define ARAD_PP_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP            SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP
#define ARAD_PP_LIF_ENTRY_TYPE_PWE                           SOC_PPC_LIF_ENTRY_TYPE_PWE
#define ARAD_PP_LIF_ENTRY_TYPE_ISID                          SOC_PPC_LIF_ENTRY_TYPE_ISID
#define ARAD_PP_LIF_ENTRY_TYPE_IP_TUNNEL_RIF                 SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF
#define ARAD_PP_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF               SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF
#define ARAD_PP_LIF_ENTRY_TYPE_TRILL_NICK                    SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK
#define ARAD_PP_LIF_ENTRY_TYPE_EXTENDER                      SOC_PPC_LIF_ENTRY_TYPE_EXTENDER
#define ARAD_PP_LIF_ENTRY_TYPE_ALL                           SOC_PPC_LIF_ENTRY_TYPE_ALL
#define ARAD_PP_NOF_LIF_ENTRY_TYPES                          SOC_PPC_NOF_LIF_ENTRY_TYPES
typedef SOC_PPC_LIF_ENTRY_TYPE                                 ARAD_PP_LIF_ENTRY_TYPE;


typedef SOC_PPC_LIF_ENTRY_PER_TYPE_INFO                        ARAD_PP_LIF_ENTRY_PER_TYPE_INFO;
typedef SOC_PPC_LIF_ENTRY_INFO                                 ARAD_PP_LIF_ENTRY_INFO;
typedef SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE                    ARAD_PP_LIF_TBL_TRAVERSE_MATCH_RULE;

typedef SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO                  ARAD_PP_LIF_TABLE_ENTRY_ACCESSED_INFO;

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
 *   arad_pp_lif_table_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Traverse the LIF Table entries (in specified range), and
 *   retrieve entries that match the given rule.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_LIF_TBL_TRAVERSE_MATCH_RULE             *rule -
 *     Rule to compare the LIF table entries against.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                        *block_range -
 *     Block range in the MACT.
 *   SOC_SAND_OUT ARAD_PP_LIF_ENTRY_INFO                          *entries_array -
 *     Logical interfaces info
 *   SOC_SAND_OUT uint32                                      *nof_entries -
 *     Number of valid entries in 'entries_array'.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_table_get_block(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_LIF_TBL_TRAVERSE_MATCH_RULE             *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                        *block_range,
    SOC_SAND_OUT ARAD_PP_LIF_ENTRY_INFO                          *entries_array,
    SOC_SAND_OUT uint32                                      *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_table_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable updating LIF table entry attribute, without
 *   accessing the LIF KEY.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_ndx -
 *     Lif entry index
 *   SOC_SAND_OUT ARAD_PP_LIF_ENTRY_INFO                      *lif_entry_info -
 *     LIF entry attributes, according to LIF type.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_table_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_ENTRY_INFO                      *lif_entry_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_table_entry_update
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable updating LIF table entry attribute, without
 *   accessing the LIF KEY.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_ndx -
 *     Lif entry index
 *   SOC_SAND_IN  ARAD_PP_LIF_ENTRY_INFO                      *lif_entry_info -
 *     LIF entry attributes, according to LIF type.
 * REMARKS:
 *   Advanced function. Straitforward usage of the LIF table
 *   is adding or removing the interface according to its
 *   key.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_table_entry_update(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_ENTRY_INFO                      *lif_entry_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_table_entry_accessed_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   check if entry was accessed upon packet lookup
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_ndx -
 *     Lif entry index
 *   SOC_SAND_IN  uint8                               clear_access_stat -
 *     clear on read
 *   SOC_SAND_OUT  ARAD_PP_LIF_TABLE_ENTRY_ACCESSED_INFO       *accessed_info -
 *     was accessed
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_table_entry_accessed_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_LIF_ID                            lif_ndx,
    SOC_SAND_IN  uint8                               clear_access_stat,
    SOC_SAND_OUT  ARAD_PP_LIF_TABLE_ENTRY_ACCESSED_INFO     *accessed_info
  );

void
  ARAD_PP_LIF_ENTRY_PER_TYPE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LIF_ENTRY_PER_TYPE_INFO *info
  );

void
  ARAD_PP_LIF_ENTRY_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LIF_ENTRY_INFO *info
  );

void
  ARAD_PP_LIF_TBL_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT ARAD_PP_LIF_TBL_TRAVERSE_MATCH_RULE *info
  );

void
  ARAD_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_LIF_ENTRY_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_LIF_ENTRY_TYPE enum_val
  );

void
  ARAD_PP_LIF_ENTRY_PER_TYPE_INFO_print(
    SOC_SAND_IN  ARAD_PP_LIF_ENTRY_PER_TYPE_INFO *info,
    SOC_SAND_IN  ARAD_PP_LIF_ENTRY_TYPE  type
  );

void
  ARAD_PP_LIF_ENTRY_INFO_print(
    SOC_SAND_IN  ARAD_PP_LIF_ENTRY_INFO *info
  );

void
  ARAD_PP_LIF_TBL_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  ARAD_PP_LIF_TBL_TRAVERSE_MATCH_RULE *info
  );

void
  ARAD_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  ARAD_PP_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_LIF_TABLE_INCLUDED__*/

#endif
