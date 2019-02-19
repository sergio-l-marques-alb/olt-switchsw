/* $Id: pb_pp_lif_table.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_lif_table.c
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

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_table.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_rif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LIF_TABLE_ENTRIES_TYPE_BM_MAX                    (SOC_SAND_U32_MAX)

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

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_lif_table[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_LIF_TABLE_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_lif_table[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_LIF_TABLE_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_TABLE_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_LIF_ENTRY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_TABLE_ENTRIES_TYPE_BM_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_LIF_TABLE_ENTRIES_TYPE_BM_OUT_OF_RANGE_ERR",
    "The parameter 'entries_type_bm' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_TYPE_MISMATCH_ERR,
    "SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_TYPE_MISMATCH_ERR",
    "when update entry using soc_pb_pp_lif_table_entry_update_unsafe. \n\r "
    "information type has to be same as entry type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_lif_table_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_init_unsafe()", 0, 0);
}



/*********************************************************************
*     Traverse the LIF Table entries (in specified range), and
 *     retrieve entries that match the given rule.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_table_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE             *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                      *block_range,
    SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO                          *entries_array,
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    entry_ndx,
    entries_to_scan,
    nof_valid_entries = 0;
  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO
    accessed_info;
  uint8
    match=FALSE;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(entries_array);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  *nof_entries = 0;

  /*
   *	First verification on the block iteration
   */
  if (block_range->iter > SOC_PB_PP_LIF_ID_MAX)
  {
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  entries_to_scan = block_range->entries_to_scan;

  if (entries_to_scan == SOC_SAND_TBL_ITER_SCAN_ALL)
  {
    entries_to_scan = SOC_PB_PP_LIF_ID_MAX+1;
  }
  

  for (entry_ndx = block_range->iter; entry_ndx < block_range->iter + entries_to_scan; ++entry_ndx)
  {
    if (nof_valid_entries >= block_range->entries_to_act || entry_ndx > SOC_PB_PP_LIF_ID_MAX)
    {
      /*
       *	No need to go further, maximal number of entries have been found
       */
      break;
    }

    res = soc_pb_pp_lif_table_entry_get_unsafe(
            unit,
            entry_ndx,
            &entries_array[nof_valid_entries]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (entries_array[nof_valid_entries].type & rule->entries_type_bm)
    {
      if (rule->accessed_only)
      {
        res = soc_pb_pp_lif_table_entry_accessed_info_get_unsafe(unit,entry_ndx,FALSE,&accessed_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
        if (accessed_info.accessed)
        {
          match = TRUE;
        }
        else
        {
          match = FALSE;
        }
      }
      else
      {
        match = TRUE;
      }
    }
    else
    {
      match = FALSE;
    }
    if (match)
    {
      ++nof_valid_entries;
    }
  }
  *nof_entries = nof_valid_entries;
  block_range->iter = entry_ndx;
  if (block_range->iter > SOC_PB_PP_LIF_ID_MAX)
  {
    SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_get_block_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_lif_table_get_block_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE             *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                        *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_GET_BLOCK_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE, rule, 10, exit);
  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_TABLE_BLOCK_RANGE, block_range, 20, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Enable updating LIF table entry attribute, without
 *     accessing the LIF KEY.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_ndx,
    SOC_SAND_OUT SOC_PB_PP_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  SOC_PB_PP_LIF_ENTRY_TYPE
    entry_type;
  uint8
    found;
  SOC_PB_PP_LIF_ID
    lif_ndex = lif_ndx;
  SOC_PB_PP_RIF_IP_TERM_INFO
    term_info;
  SOC_PB_PP_MPLS_TERM_INFO
    mpls_term_info;
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_SYS_VSI_ID
      vsi_index;
  SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_ADDITIONAL_INFO  pwe_additional_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);

  SOC_PB_PP_LIF_ENTRY_INFO_clear(lif_entry_info);

  SOC_PPD_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);

  res = soc_pb_pp_sw_db_lif_table_entry_use_get(
          unit,
          lif_ndx,
          &entry_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  lif_entry_info->type = entry_type;
  lif_entry_info->index = lif_ndx;
  switch (entry_type)
  {
    case SOC_PB_PP_LIF_ENTRY_TYPE_AC:
    case SOC_PB_PP_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PB_PP_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
      res = soc_pb_pp_l2_lif_ac_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_ndex,
              &(lif_entry_info->value.ac),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_LIF_ENTRY_TYPE_PWE:
      res = soc_pb_pp_l2_lif_pwe_get_internal_unsafe(
              unit,
              0,
              TRUE,
              &lif_ndex,
			        &pwe_additional_info,
              &(lif_entry_info->value.pwe),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
      res = soc_pb_pp_rif_ip_tunnel_map_get_internal_unsafe(
              unit,
              0,
              TRUE,
              &lif_ndex,
              &term_info,
              &(lif_entry_info->value.rif),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
      res = soc_pb_pp_rif_mpls_label_map_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_ndex,
              &mpls_term_info,
              &(lif_entry_info->value.rif),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
  case SOC_PB_PP_LIF_ENTRY_TYPE_ISID:
      res = soc_pb_pp_l2_lif_isid_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_ndex,
              &vsi_index,
              &(lif_entry_info->value.isid),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
    case SOC_PB_PP_LIF_ENTRY_TYPE_TRILL_NICK:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FEATURE_NOT_SUPPORTED_ERR, 10, exit);
    break;
    default:
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_entry_get_unsafe()", lif_ndx, 0);
}

uint32
  soc_pb_pp_lif_table_entry_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lif_ndx, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_entry_get_verify()", lif_ndx, 0);
}

/*********************************************************************
*     Enable updating LIF table entry attribute, without
 *     accessing the LIF KEY.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_update_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  SOC_PB_PP_LIF_ENTRY_TYPE
    entry_type;
  SOC_PB_PP_RIF_IP_TERM_INFO
    ip_term_info;
  SOC_PB_PP_MPLS_TERM_INFO
    mpls_term_info;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_PP_RIF_INFO
    tmp_rif;
  SOC_PB_PP_LIF_ID
    lif_indx=lif_ndx;
  uint8
    found;
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_L2_LIF_ISID_INFO
      tmp_isid;
  SOC_PB_PP_SYS_VSI_ID
      vsi_index;
  SOC_PB_PP_L2_LIF_PWE_ADDITIONAL_INFO          pwe_additional_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_UNSAFE);

  SOC_PPD_L2_LIF_PWE_ADDITIONAL_INFO_clear(&pwe_additional_info);
  SOC_SAND_CHECK_NULL_INPUT(lif_entry_info);
  
  res = soc_pb_pp_sw_db_lif_table_entry_use_get(
          unit,
          lif_ndx,
          &entry_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  /* Verify: entry type consistent. In case of empty configure as user wants */
  if (entry_type != lif_entry_info->type && entry_type != SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_TYPE_MISMATCH_ERR, 10, exit);
  }

  switch (lif_entry_info->type)
  {
    case SOC_PB_PP_LIF_ENTRY_TYPE_AC:
    case SOC_PB_PP_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    case SOC_PB_PP_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
      res = soc_pb_pp_l2_lif_ac_add_internal_unsafe(
              unit,
              NULL,
              TRUE,
              lif_ndx,
              &(lif_entry_info->value.ac),
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_LIF_ENTRY_TYPE_PWE:
      res = soc_pb_pp_l2_lif_pwe_add_internal_unsafe(
              unit,
              0,
              TRUE,
              lif_ndx,
			  &pwe_additional_info,
              &(lif_entry_info->value.pwe),
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:

      /* get before set, as term is not updated */
      res = soc_pb_pp_rif_ip_tunnel_map_get_internal_unsafe(
              unit,
              0,
              TRUE,
              &lif_indx,
              &ip_term_info,
              &(tmp_rif),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_pb_pp_rif_ip_tunnel_map_add_internal_unsafe(
              unit,
              0,
              TRUE,
              lif_ndx,
              &ip_term_info,
              &(lif_entry_info->value.rif),
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
    case SOC_PB_PP_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
      /* get before set, as term is not updated */
      res = soc_pb_pp_rif_mpls_label_map_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_indx,
              &mpls_term_info,
              &(tmp_rif),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_pb_pp_rif_mpls_label_map_add_internal_unsafe(
              unit,
              NULL,
              TRUE,
              lif_ndx,
              &mpls_term_info,
              &(lif_entry_info->value.rif),
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
  case SOC_PB_PP_LIF_ENTRY_TYPE_ISID:
      /* get before set, as isid->vsi mapping is not updated */
      res = soc_pb_pp_l2_lif_isid_get_internal_unsafe(
              unit,
              NULL,
              TRUE,
              &lif_indx,
              &vsi_index,
              &(tmp_isid),
              &found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_pb_pp_l2_lif_isid_add_internal_unsafe(
              unit,
              vsi_index,
              NULL,
              TRUE,
              lif_ndx,
              &(lif_entry_info->value.isid),
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
  case SOC_PB_PP_LIF_ENTRY_TYPE_TRILL_NICK:
    break;
    default:
      break;
  }


  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_entry_update_unsafe()", lif_ndx, 0);
}

uint32
  soc_pb_pp_lif_table_entry_update_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO                          *lif_entry_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_ENTRY_UPDATE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lif_ndx, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_LIF_ENTRY_INFO, lif_entry_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_entry_update_verify()", lif_ndx, 0);
}

/*********************************************************************
*     GET access status
 *     the LIF entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_lif_table_entry_accessed_info_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                           lif_ndx,
    SOC_SAND_IN  uint8                              clear_access_stat,
    SOC_SAND_OUT SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO    *accessed_info
  )
{
  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA
    ihb_sem_entry_accessed_tbl_data;
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(accessed_info);

  SOC_SAND_ERR_IF_ABOVE_MAX(lif_ndx, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 5, exit);

  SOC_PB_PP_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(accessed_info);

  /* was entry accessed */
  res = soc_pb_pp_ihp_sem_result_accessed_tbl_get_unsafe(
          unit,
          lif_ndx/16,
          &ihb_sem_entry_accessed_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tmp = SOC_SAND_GET_BIT(ihb_sem_entry_accessed_tbl_data.sem_result_accessed,(lif_ndx) % 16);
  accessed_info->accessed = SOC_SAND_NUM2BOOL(tmp);

  /* clear accessed bit if needed*/
  if (clear_access_stat)
  {
    tmp = 0;
    res = soc_sand_set_field(
            &(ihb_sem_entry_accessed_tbl_data.sem_result_accessed),
            lif_ndx % 16,
            lif_ndx % 16,
            tmp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_pb_pp_ihp_sem_result_accessed_tbl_set_unsafe(
            unit,
            lif_ndx/16,
            &ihb_sem_entry_accessed_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_lif_table_entry_accessed_info_get_unsafe()", lif_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_lif_table module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lif_table_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_lif_table;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_lif_table module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lif_table_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_lif_table;
}

uint32
  SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO *info,
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_TYPE             type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch (type)
  {
  case SOC_PB_PP_LIF_ENTRY_TYPE_AC:
  case SOC_PB_PP_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
  case SOC_PB_PP_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_AC_INFO, &(info->ac), 10, exit);
  break;
  case SOC_PB_PP_LIF_ENTRY_TYPE_PWE:
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_PWE_INFO, &(info->pwe), 11, exit);
  break;
  case SOC_PB_PP_LIF_ENTRY_TYPE_ISID:
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_ISID_INFO, &(info->isid), 12, exit);
  break;
  case SOC_PB_PP_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
  case SOC_PB_PP_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_RIF_INFO, &(info->rif), 13, exit);
  break;
  case SOC_PB_PP_LIF_ENTRY_TYPE_TRILL_NICK:
    SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_L2_LIF_TRILL_INFO, &(info->trill), 14, exit);
  break;
  default:
    break;
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_ENTRY_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = SOC_PB_PP_LIF_ENTRY_PER_TYPE_INFO_verify (&(info->value),info->type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
  SOC_SAND_ERR_IF_ABOVE_MAX(info->index, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_ENTRY_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE_verify(
    SOC_SAND_IN  SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->entries_type_bm, SOC_PB_PP_LIF_TABLE_ENTRIES_TYPE_BM_MAX, SOC_PB_PP_LIF_TABLE_ENTRIES_TYPE_BM_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_LIF_TBL_TRAVERSE_MATCH_RULE_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

