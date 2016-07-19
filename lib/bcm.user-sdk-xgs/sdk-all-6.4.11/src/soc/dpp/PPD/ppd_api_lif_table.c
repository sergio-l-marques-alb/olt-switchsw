/* $Id: ppd_api_lif_table.c,v 1.14 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_lif_table.c
*
* MODULE PREFIX:  soc_ppd_lif
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
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_lif_table.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_lif_table.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_lif_table.h>
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
    Ppd_procedure_desc_element_lif_table[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_ENTRY_UPDATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_PRINT_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_ENTRY_UPDATE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LIF_TABLE_GET_PROCS_PTR),
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
*     Traverse the LIF Table entries (in specified range), and
 *     retrieve entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_table_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPD_LIF_ENTRY_INFO                      *entries_array,
    SOC_SAND_OUT uint32                                *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_TABLE_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_table_get_block,(unit, rule, block_range, entries_array, nof_entries));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_table_get_block_print,(unit,rule,block_range));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_table_get_block()", 0, 0);
}

/*********************************************************************
*     Enable updating LIF table entry attribute, without
 *     accessing the LIF KEY.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_table_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_ndx,
    SOC_SAND_OUT SOC_PPD_LIF_ENTRY_INFO                      *lif_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_TABLE_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_table_entry_get,(unit, lif_ndx, lif_entry_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_table_entry_get_print,(unit,lif_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_table_entry_get()", lif_ndx, 0);
}

/*********************************************************************
*     Enable updating LIF table entry attribute, without
 *     accessing the LIF KEY.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_lif_table_entry_update(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_ENTRY_INFO                      *lif_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_TABLE_ENTRY_UPDATE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_table_entry_update,(unit, lif_ndx, lif_entry_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_lif_table_entry_update_print,(unit,lif_ndx,lif_entry_info));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_table_entry_update()", lif_ndx, 0);
}


/*********************************************************************
 *     get access status
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_ppd_lif_table_entry_accessed_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_ndx,
    SOC_SAND_IN  uint8                               clear_access_stat,
    SOC_SAND_OUT  SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO       *accessed_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(lif_table_entry_accessed_info_get,(unit, lif_ndx, clear_access_stat, accessed_info));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_lif_table_entry_accessed_info_get()", lif_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_lif_table module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_lif_table_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_lif_table;
}
void
  SOC_PPD_LIF_ENTRY_PER_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_ENTRY_PER_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ENTRY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_LIF_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_LIF_ENTRY_TYPE enum_val
  )
{
  return SOC_PPC_LIF_ENTRY_TYPE_to_string(enum_val);
}

void
  SOC_PPD_LIF_ENTRY_PER_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_ENTRY_PER_TYPE_INFO *info,
    SOC_SAND_IN  SOC_PPD_LIF_ENTRY_TYPE  type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_print(info,type);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_ENTRY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  soc_ppd_lif_table_print_block(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE            *rule,
    SOC_SAND_IN SOC_SAND_TABLE_BLOCK_RANGE                        *block_range,
    SOC_SAND_IN SOC_PPD_LIF_ENTRY_INFO                          *entries_array,
    SOC_SAND_IN uint32                                      nof_entries
  )
{
  uint32
    indx;
  uint8
    inc_l2=FALSE,
    inc_ac=FALSE,
    inc_pwe=FALSE,
    inc_l3=FALSE;
  SOC_PPD_LIF_TABLE_ENTRY_ACCESSED_INFO
    accessed_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LIF_TABLE_PRINT_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries_array);

  if ((rule->entries_type_bm & (SOC_PPD_LIF_ENTRY_TYPE_AC|SOC_PPD_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPD_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)) != 0)
  {
    inc_ac = TRUE;
  }
  if ((rule->entries_type_bm & (SOC_PPD_LIF_ENTRY_TYPE_PWE)) != 0)
  {
    inc_pwe = TRUE;
  }
  if (inc_ac || inc_pwe)
  {
    inc_l2 = TRUE;;
  }
  if ((rule->entries_type_bm & (SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF|SOC_PPD_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF)) != 0)
  {
    inc_l3 = TRUE;
  }

  if (inc_l3 && inc_l2)
  {
    LOG_CLI((BSL_META_U(unit,
                        " ------------------------\n\r"
                        "|         LIF Table      |\n\r"
                        " ------------------------\n\r"
                        "|offset| type |      |ACC|\n\r"
                        " ------------------------\n\r"
                 )));
  }
  else if (inc_l2)
  {
#ifndef COMPILER_STRING_CONST_LIMIT
    LOG_CLI((BSL_META_U(unit,
                        " -------------------------------------------------------------------------------------\n\r"
                        "|                                L2  LIF Table                                        |\n\r"
                        " -------------------------------------------------------------------------------------\n\r"
                        "|      |             |A|         Default/Learn                     |     |VID Edit|C|O|\n\r"
                        "|      |             |C| Destination  |     Additional Info        |     |   |prof|O|r|\n\r"
                        "|offset| type |      |C| Type  | Val  | Type |                     |VSID |VID |E|P|S|i|\n\r"
                        " -------------------------------------------------------------------------------------\n\r"
                 )));
#else 
    LOG_CLI((BSL_META_U(unit,
                        "|                                L2  LIF Table                                        |\n\r"
                        " -------------------------------------------------------------------------------------\n\r"
                        "|      |             |A|         Default/Learn                     |     |VID Edit|C|O|\n\r"
                        "|      |             |C| Destination  |     Additional Info        |     |   |prof|O|r|\n\r"
                        "|offset| type |      |C| Type  | Val  | Type |                     |VSID |VID |E|P|S|i|\n\r"
                 )));
#endif /* !COMPILER_STRING_CONST_LIMIT */
  }
  else if (inc_l3)
  {
    LOG_CLI((BSL_META_U(unit,
                        " --------------------------------------------------\n\r"
                        "|                   L3 LIF (RIF) Table             |\n\r"
                        " --------------------------------------------------\n\r"
                        "|offset| type |      |ACC| VRF |COS|TTL|RPF|Routing|\n\r"
                        " --------------------------------------------------\n\r"
                 )));
  }

  for (indx = 0; indx < nof_entries; ++indx)
  {
    if (entries_array[indx].type == SOC_PPD_LIF_ENTRY_TYPE_EMPTY)
    {
      continue;
    }
    res = soc_ppd_lif_table_entry_accessed_info_get(
            unit,
            entries_array[indx].index,
            TRUE,
            &accessed_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*offset */
    LOG_CLI((BSL_META_U(unit,
                        "|%-6u"), entries_array[indx].index));
    /* type */
    LOG_CLI((BSL_META_U(unit,
                        "|%-6s"), SOC_PPD_LIF_ENTRY_TYPE_to_string(entries_array[indx].type)));

    /* AC */
    if (entries_array[indx].type & (SOC_PPD_LIF_ENTRY_TYPE_AC|SOC_PPD_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP|SOC_PPD_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP))
    {
      /* sub-type */
      LOG_CLI((BSL_META_U(unit,
                          "|%-6s"),SOC_PPD_L2_LIF_AC_SERVICE_TYPE_to_string(entries_array[indx].value.ac.service_type)));
      if (accessed_info.accessed)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"V"));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"X"));
      }

      if (inc_l3&inc_l2)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|\n\r")));
        continue;
      }
      /* dest for AC */
      if (entries_array[indx].value.ac.service_type != SOC_PPD_L2_LIF_AC_SERVICE_TYPE_MP)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|")));
        SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
          "",
          &(entries_array[indx].value.ac.default_frwrd.default_forwarding)
        );
      }
      else
      {
        if (entries_array[indx].value.ac.learn_record.learn_type == SOC_PPD_L2_LIF_AC_LEARN_SYS_PORT)
        {
          LOG_CLI((BSL_META_U(unit,
                              "%-7s "), "|sys-port"));
          LOG_CLI((BSL_META_U(unit,
                              "%-5s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%-6s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%10s%-10s ")," ","-"));
        }
        else if (entries_array[indx].value.ac.learn_record.learn_type == SOC_PPD_L2_LIF_AC_LEARN_DISABLE)
        {
          LOG_CLI((BSL_META_U(unit,
                              "%-7s "), "|disable"));
          LOG_CLI((BSL_META_U(unit,
                              "%-5s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%-6s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%10s%-10s ")," ","-"));

        }
        else if (entries_array[indx].value.ac.learn_record.learn_type == SOC_PPD_L2_LIF_AC_LEARN_INFO)
        {
          SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
            "",
            &(entries_array[indx].value.ac.learn_record.learn_info)
            );
        }
      }
      /* |VSID |VID|VE | PCP|COS|Ori*/
      LOG_CLI((BSL_META_U(unit,
                          "|%-5u"), entries_array[indx].value.ac.vsid));
      LOG_CLI((BSL_META_U(unit,
                          "|%-4u"), entries_array[indx].value.ac.ing_edit_info.vid));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1u"), entries_array[indx].value.ac.ing_edit_info.ing_vlan_edit_profile));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1u"), entries_array[indx].value.ac.ing_edit_info.edit_pcp_profile));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1u"), entries_array[indx].value.ac.cos_profile));
      if (entries_array[indx].value.ac.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"H"));
      }
      else if (entries_array[indx].value.ac.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"S"));
      }
    }
    /* pwe */
    else if (entries_array[indx].type & (SOC_PPD_LIF_ENTRY_TYPE_PWE))
    {
      /* sub-type */
      LOG_CLI((BSL_META_U(unit,
                          "|%-6s"),SOC_PPD_L2_LIF_PWE_SERVICE_TYPE_to_string(entries_array[indx].value.pwe.service_type)));
      if (accessed_info.accessed)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"V"));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"X"));
      }

      if (inc_l3&inc_l2)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|\n\r")));
        continue;
      }
      /* dest for AC */
      if (entries_array[indx].value.pwe.service_type != SOC_PPD_L2_LIF_PWE_SERVICE_TYPE_MP)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|")));
        SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
          "",
          &(entries_array[indx].value.pwe.default_frwrd.default_forwarding)
          );
      }
      else
      {
        if (entries_array[indx].value.pwe.learn_record.enable_learning == FALSE)
        {
          LOG_CLI((BSL_META_U(unit,
                              "%-7s "), "|disable"));
          LOG_CLI((BSL_META_U(unit,
                              "%-5s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%-6s|"),"   -" ));
          LOG_CLI((BSL_META_U(unit,
                              "%10s%-10s ")," ","-"));
        }
        else
        {
          SOC_PPC_FRWRD_DECISION_INFO_print_table_format(
            "",
            &(entries_array[indx].value.pwe.learn_record.learn_info)
            );
        }
      }
      /* |VSID |VID|VE | PCP|COS|Ori*/
      LOG_CLI((BSL_META_U(unit,
                          "|%-5u"), entries_array[indx].value.pwe.vsid));
      LOG_CLI((BSL_META_U(unit,
                          "|%-4s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1s"), ""));
      LOG_CLI((BSL_META_U(unit,
                          "|%-1s"), ""));
      if (entries_array[indx].value.pwe.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"H"));
      }
      else if (entries_array[indx].value.pwe.orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s"),"S"));
      }
    }
    else if (entries_array[indx].type & (SOC_PPD_LIF_ENTRY_TYPE_IP_TUNNEL_RIF|SOC_PPD_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF))
    {
      /*|ACC| VRF |COS|TTL|RPF|Routing|*/
      LOG_CLI((BSL_META_U(unit,
                          "|%s"),"      "));
      if (accessed_info.accessed)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s")," V "));
      }
      else
      {
        LOG_CLI((BSL_META_U(unit,
                            "|%s")," X "));
      }
      if (inc_l3&inc_l2)
      {
        LOG_CLI((BSL_META_U(unit,
                            "|\n\r")));
        continue;
      }
      LOG_CLI((BSL_META_U(unit,
                          "|%-5u"), entries_array[indx].value.rif.vrf_id));
      LOG_CLI((BSL_META_U(unit,
                          "|%-3u"), entries_array[indx].value.rif.cos_profile));
      LOG_CLI((BSL_META_U(unit,
                          "|%-3u"), entries_array[indx].value.rif.ttl_scope_index));
      LOG_CLI((BSL_META_U(unit,
                          "|%-3u"), entries_array[indx].value.rif.uc_rpf_enable));
      LOG_CLI((BSL_META_U(unit,
                          "|%-7u"), entries_array[indx].value.rif.routing_enablers_bm));
    }
                
    LOG_CLI((BSL_META_U(unit,
                        "|\n\r")));
  }

  if (inc_l3 && inc_l2)
  {
    LOG_CLI((BSL_META_U(unit,
                        " ------------------------\n\r"
                 )));
  }
  else if (inc_l2)
  {
    LOG_CLI((BSL_META_U(unit,
                        " -------------------------------------------------------------------------------------\n\r"
                 )));
  }
  else if (inc_l3)
  {
    LOG_CLI((BSL_META_U(unit,
                        " --------------------------------------------------\n\r"
                 )));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_fec_print_block()", 0, 0);
}

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_lif_table_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE         *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "rule:")));
  SOC_PPD_LIF_TBL_TRAVERSE_MATCH_RULE_print((rule));

  LOG_CLI((BSL_META_U(unit,
                      "block_range:")));
  soc_sand_SAND_TABLE_BLOCK_RANGE_print((block_range));

  return;
}
void
  soc_ppd_lif_table_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_ndx: %lu\n\r"),lif_ndx));

  return;
}
void
  soc_ppd_lif_table_entry_update_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PPD_LIF_ENTRY_INFO                      *lif_entry_info
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "lif_ndx: %lu\n\r"),lif_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "lif_entry_info:")));
  SOC_PPD_LIF_ENTRY_INFO_print((lif_entry_info));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

