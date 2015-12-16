/* $Id: pb_pp_fp_fem.c,v 1.11 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_fem.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp_key.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FP_FEM_DB_ID_NDX_MAX                             (SOC_PB_PP_FP_NOF_DBS-1)
#define SOC_PB_PP_FP_FEM_ACTION_TYPE_MAX                           (SOC_PB_PP_NOF_FP_ACTION_TYPES - 1)
#define SOC_PB_PP_FP_FEM_DB_STRENGTH_MAX                           (SOC_PB_PP_FP_NOF_DBS-1)
#define SOC_PB_PP_FP_FEM_DB_ID_MAX                                 (SOC_PB_PP_FP_FEM_DB_ID_NDX_MAX)
#define SOC_PB_PP_FP_FEM_ENTRY_STRENGTH_MAX                        (SOC_SAND_U16_MAX)
#define SOC_PB_PP_FP_FEM_ENTRY_ID_MAX                              (SOC_PB_PP_FP_NOF_ENTRY_IDS-1)

#define SOC_PB_PP_FP_FEM_SAND_U64_NOF_BITS                         (64)
#define SOC_PB_PP_FP_FEM_MASK_LENGTH_IN_BITS                       (4)

#define SOC_PB_PP_FP_BIT_LOC_LSB_CHANGE_KEY                        (5)
#define SOC_PB_PP_FP_BIT_LOC_CHANGE_KEY_OPERATION_FLD_SIZE         (2)

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

static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_fp_fem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_INSERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_INSERT_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_INSERT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_INSERT_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_IS_PLACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_IS_PLACE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_IS_PLACE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_TAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_TAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_TAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_TAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_TAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_TAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_TAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_TAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_TAG_ACTION_TYPE_CONVERT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_IS_FEM_BLOCKING_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_DUPLICATE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_CONFIGURE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_CONFIGURATION_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FP_FEM_REORGANIZE),



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_fp_fem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pfg_ndx' is out of range. \n\r "
    "The range is: 0 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'db_id_ndx' is out of range. \n\r "
    "The range is: 0 - 127.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'db_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR",
    "The parameter 'db_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'entry_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR",
    "The parameter 'entry_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
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
  soc_pb_pp_fp_tag_action_type_convert(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE         action_type,
    SOC_SAND_OUT uint8                    *is_tag_action,
    SOC_SAND_OUT uint32                    *action_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_TAG_ACTION_TYPE_CONVERT);

  *is_tag_action = FALSE;
  *action_ndx = SOC_PB_PP_FP_NOF_TAG_ACTION_TYPES;

  switch (action_type)
  {
  case SOC_PB_PP_FP_ACTION_TYPE_STAT:
    *is_tag_action = TRUE;
    *action_ndx = SOC_PB_PMF_TAG_TYPE_STAT_TAG;
  	break;
  case SOC_PB_PP_FP_ACTION_TYPE_LAG_LB:
    *is_tag_action = TRUE;
    *action_ndx = SOC_PB_PMF_TAG_TYPE_LAG_LB_KEY;
    break;
  case SOC_PB_PP_FP_ACTION_TYPE_ECMP_LB:
    *is_tag_action = TRUE;
    *action_ndx = SOC_PB_PMF_TAG_TYPE_ECMP_LB_KEY;
    break;
  case SOC_PB_PP_FP_ACTION_TYPE_STACK_RT_HIST:
    *is_tag_action = TRUE;
    *action_ndx = SOC_PB_PMF_TAG_TYPE_STACK_RT_HIST;
    break;
  default:
    break;
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_tag_action_type_convert()", action_type, 0);
}




/*
 *	Return the Action output size (number of
 *  significant bits)
 */
uint32
  soc_pb_pp_fp_action_type_max_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE fp_action_type,
    SOC_SAND_OUT uint32            *action_size_in_bits,
    SOC_SAND_OUT uint32            *action_size_in_bits_in_fem
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ACTION_TYPE_MAX_SIZE_GET);

  switch(fp_action_type)
  {
  case SOC_PB_PP_FP_ACTION_TYPE_DEST:
    *action_size_in_bits = 17;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_DP:
    *action_size_in_bits = 4;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_TC:
    *action_size_in_bits = 4;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_TRAP:
    *action_size_in_bits = 11;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_SNP:
    *action_size_in_bits = 10;
    break;
  
  case SOC_PB_PP_FP_ACTION_TYPE_MIRROR:
    *action_size_in_bits = 4;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_MIR_DIS:
    *action_size_in_bits = 1;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_EXC_SRC:
    *action_size_in_bits = 1;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_IS:
    *action_size_in_bits = 16;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_METER:
    *action_size_in_bits = 14;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_COUNTER:
    *action_size_in_bits = 13;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_OUTLIF:
    *action_size_in_bits = 16;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_VSQ_PTR:
    *action_size_in_bits = 14;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY:
    *action_size_in_bits = 12;
    break;

  default:
    *action_size_in_bits = 0;
  }

  *action_size_in_bits_in_fem = *action_size_in_bits;
  /* Special case for Change Key where the LSB are not encoded by the user */
  if (fp_action_type == SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY)
  {
    *action_size_in_bits_in_fem = *action_size_in_bits + SOC_PB_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_action_type_max_size_get()", 0, 0);
}

uint32
  soc_pb_pp_fp_qual_lsb_and_length_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE         qual_type,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT uint32                   *qual_lsb,
    SOC_SAND_OUT uint32                   *qual_length_no_padding
  )
{
  uint32
    qual_lsb_lcl = 0,
    res;
  uint32
    qual_size_in_bits_no_padding = 0,
    qual_type_ndx;
  SOC_PB_PP_FP_KEY_DESC
    key_desc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_QUAL_LSB_AND_LENGTH_GET);

  res = soc_pb_pp_fp_key_desc_get_unsafe(
          unit,
          db_id_ndx,
          &key_desc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
  {
    if (fp_database_info->qual_types[qual_type_ndx] == BCM_FIELD_ENTRY_INVALID)
    {
      continue;
    }

    if (fp_database_info->qual_types[qual_type_ndx] == qual_type)
    {
      /*
       * Get the qualifier size without padding
       */
      qual_lsb_lcl = key_desc.fields[qual_type_ndx].lsb;
      qual_size_in_bits_no_padding = key_desc.fields[qual_type_ndx].length;
      break;
    }
  }


  *qual_lsb = qual_lsb_lcl;
  *qual_length_no_padding = qual_size_in_bits_no_padding;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_lsb_and_length_get()", 0, 0);
}




uint32
  soc_pb_pp_fp_qual_type_and_local_lsb_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                   bit_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_TYPE         *qual_type,
    SOC_SAND_OUT uint32                   *qual_lsb
  )
{
  uint32
    qual_lsb_lcl = 0,
    qual_lsb_lcl_with_padding = 0,
    res;
  uint32
    qual_size_in_bits,
    qual_size_in_bits_no_padding = 0,
    qual_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_QUAL_TYPE_AND_LOCAL_LSB_GET);

  *qual_type = BCM_FIELD_ENTRY_INVALID;
  for (qual_type_ndx = 0; qual_type_ndx < SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX; ++qual_type_ndx)
  {
    if (fp_database_info->qual_types[qual_type_ndx] == BCM_FIELD_ENTRY_INVALID)
    {
      continue;
    }

    /*
     * Get the qualifier size with padding
     */
    res = soc_pb_pp_fp_key_length_get_unsafe(
            unit,
            fp_database_info->qual_types[qual_type_ndx],
            TRUE,
            &qual_size_in_bits
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_pb_pp_fp_key_length_get_unsafe(
            unit,
            fp_database_info->qual_types[qual_type_ndx],
            FALSE,
            &qual_size_in_bits_no_padding
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    qual_lsb_lcl_with_padding = qual_lsb_lcl + qual_size_in_bits - qual_size_in_bits_no_padding;
    if ((bit_ndx >= qual_lsb_lcl_with_padding) && (bit_ndx < qual_lsb_lcl + qual_size_in_bits))
    {
      *qual_type = fp_database_info->qual_types[qual_type_ndx];
      break;
    }
    qual_lsb_lcl += qual_size_in_bits;
  }

  *qual_lsb = bit_ndx - qual_lsb_lcl_with_padding;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_qual_type_and_local_lsb_get()", 0, 0);
}

uint32
  soc_pb_pp_fp_action_lsb_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE       action_type,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT uint32                   *action_lsb
  )
{
  uint32
    action_size_in_bits,
    action_size_in_bits_in_fem,
    action_lsb_lcl = 0,
    res;
  uint32
    action_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ACTION_LSB_GET);

  for (action_type_ndx = 0; action_type_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    if (fp_database_info->action_types[action_type_ndx] != action_type)
    {
      /*
       * Get the action size
       */
      res = soc_pb_pp_fp_action_type_max_size_get(
              unit,
              fp_database_info->action_types[action_type_ndx],
              &action_size_in_bits,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      action_lsb_lcl += action_size_in_bits;
    }
    else
    {
      break;
    }
  }

  *action_lsb = action_lsb_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_action_lsb_get()", 0, 0);
}


uint32
  soc_pb_pp_fp_fem_duplicate(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    cycle_ndx,
    SOC_SAND_IN  uint32                    fem_id_orig,
    SOC_SAND_IN  uint32                    fem_id_dest
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PMF_FEM_NDX
    fem_ndx_orig,
    fem_ndx_dest;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    action_format_info;
  uint32
    pfg_ndx,
    selected_bits_ndx;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    action_format_map_info;
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO
    selected_bits_info;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input_info;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_DUPLICATE);

  /*
   * Duplicate in the counter-packet-flow order:
   * Action (Ethernet: #2), Action map, 4b select and then input
   */
  SOC_PB_PMF_FEM_NDX_clear(&fem_ndx_orig);
  fem_ndx_orig.cycle_ndx = cycle_ndx;
  fem_ndx_orig.id = fem_id_orig;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(&action_format_info);
  res = soc_pb_pmf_fem_action_format_get_unsafe(
          unit,
          &fem_ndx_orig,
          SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX,
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_PMF_FEM_NDX_clear(&fem_ndx_dest);
  fem_ndx_dest.cycle_ndx = cycle_ndx;
  fem_ndx_dest.id = fem_id_dest;
  res = soc_pb_pmf_fem_action_format_set_unsafe(
          unit,
          &fem_ndx_dest,
          SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX,
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (selected_bits_ndx = 0; selected_bits_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&action_format_map_info);
    res = soc_pb_pmf_fem_action_format_map_get_unsafe(
            unit,
            &fem_ndx_orig,
            SOC_PB_PMF_FEM_PGM_FOR_ETH,
            selected_bits_ndx,
            &action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_pb_pmf_fem_action_format_map_set_unsafe(
            unit,
            &fem_ndx_dest,
            SOC_PB_PMF_FEM_PGM_FOR_ETH,
            selected_bits_ndx,
            &action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(&selected_bits_info);
  res = soc_pb_pmf_fem_select_bits_get_unsafe(
          unit,
          &fem_ndx_orig,
          SOC_PB_PMF_FEM_PGM_FOR_ETH,
          &selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_pb_pmf_fem_select_bits_set_unsafe(
          unit,
          &fem_ndx_dest,
          SOC_PB_PMF_FEM_PGM_FOR_ETH,
          &selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  lkp_profile_ndx.cycle_ndx = cycle_ndx;
  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
  {
    lkp_profile_ndx.id = pfg_ndx;
    SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
    res = soc_pb_pmf_db_fem_input_get_unsafe(
            unit,
            &lkp_profile_ndx,
            fem_id_orig,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = soc_pb_pmf_db_fem_input_set_unsafe(
            unit,
            &lkp_profile_ndx,
            fem_id_dest,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

  /* 
   * Replicate the SW DB and cancel the original FEM
   */
  SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
  res = soc_pb_pp_sw_db_fp_fem_entry_get(
          unit,
          cycle_ndx,
          fem_id_orig,
          &fem_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_pb_pp_sw_db_fp_fem_entry_set(
          unit,
          cycle_ndx,
          fem_id_dest,
          &fem_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
  res = soc_pb_pp_sw_db_fp_fem_entry_set(
          unit,
          cycle_ndx,
          fem_id_orig,
          &fem_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_duplicate()", 0, 0);
}




uint32
  soc_pb_pp_fp_fem_is_fem_blocking_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx_to_insert,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx_to_compare,
    SOC_SAND_OUT uint8                    *is_fem_blocking
  )
{
  uint32
    db_strength_strong,
    db_strength_weak,
    entry_strength_strong,
    entry_strength_weak,
    res = SOC_SAND_OK;
  uint8
    forbidden_fem_cycle2_found = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_IS_FEM_BLOCKING_GET);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx_to_insert);
  SOC_SAND_CHECK_NULL_INPUT(entry_ndx_to_compare);

  /*
   * Verify if the second FEM info is  stronger
   */
  if (entry_ndx_to_insert->action_type[0] == entry_ndx_to_compare->action_type[0])
  {
    /*
     * Compare the DB strengths
     */
    db_strength_strong = entry_ndx_to_insert->db_strength;
    db_strength_weak = entry_ndx_to_compare->db_strength;

    /* The strongest strength is the closest to zero */
    if (db_strength_weak < db_strength_strong)
    {
      forbidden_fem_cycle2_found = TRUE;
    }
    else if ((db_strength_strong == db_strength_weak)
              && (entry_ndx_to_insert->db_id == entry_ndx_to_compare->db_id)
              && (entry_ndx_to_insert->is_for_entry == TRUE))
    {
      /*
       * Compare the Entry strengths
       */
      entry_strength_strong = entry_ndx_to_insert->entry_strength;
      entry_strength_weak = entry_ndx_to_compare->entry_strength;

      /* The strongest strength is the closest to zero */
      if (entry_strength_weak < entry_strength_strong)
      {
        forbidden_fem_cycle2_found = TRUE;
      }
    }
  }

  *is_fem_blocking = forbidden_fem_cycle2_found;

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_is_fem_blocking_get()", 0, 0);
}

/*
 *	Conversion FP to PMF actions
 */
uint32
  soc_pb_pp_fp_action_type_to_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE    fp_action_type,
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_TYPE  *pmf_fem_action_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ACTION_TYPE_TO_PMF_CONVERT);

  switch(fp_action_type)
  {
  case SOC_PB_PP_FP_ACTION_TYPE_DEST:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_DEST;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_DP:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_DP;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_TC:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_TC;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_TRAP:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_TRAP;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_SNP:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_SNP;
    break;
  
  case SOC_PB_PP_FP_ACTION_TYPE_MIRROR:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_MIR_DIS:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_EXC_SRC:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_IS:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_IS;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_METER:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_METER;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_COUNTER:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_COUNTER;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_OUTLIF:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_OUTLIF;
    break;

  case SOC_PB_PP_NOF_FP_ACTION_TYPES:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_VSQ_PTR:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_STAT;
    break;

  case SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY:
    *pmf_fem_action_type = SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_action_type_to_pmf_convert()", 0, 0);
}

uint32
  soc_pb_pp_fp_action_type_from_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_TYPE  pmf_fem_action_type,
    SOC_SAND_OUT SOC_PB_PP_FP_ACTION_TYPE    *fp_action_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_ACTION_TYPE_FROM_PMF_CONVERT);

  switch(pmf_fem_action_type)
  {
  case SOC_PB_PMF_FEM_ACTION_TYPE_DEST:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_DEST;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_DP:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_DP;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_TC:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_TC;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_TRAP:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_TRAP;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_SNP:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_SNP;
    break;
  
  case SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_MIRROR;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_MIR_DIS;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_EXC_SRC;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_IS:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_IS;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_METER:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_METER;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_COUNTER:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_COUNTER;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_OUTLIF:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_OUTLIF;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_STAT:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_VSQ_PTR;
    break;

  case SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA:
    *fp_action_type = SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY;
    break;

  default:
  case SOC_PB_PMF_FEM_ACTION_TYPE_NOP:
    *fp_action_type = SOC_PB_PP_NOF_FP_ACTION_TYPES;
    break;
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_action_type_from_pmf_convert()", 0, 0);
}





STATIC
  uint32
    soc_pb_pp_fp_fem_set(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  SOC_PB_PMF_FEM_NDX             *fem,
      SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE        action_type,
      SOC_SAND_IN  uint32                    extraction_lsb,
      SOC_SAND_IN  uint32                    nof_bits,
      SOC_SAND_IN  uint32                    base_value,
      SOC_SAND_IN  uint32                   db_id
    )
{
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  uint32
    val,
    res;
  uint32
    pfg_ndx,
    bit_loc_lsb,
    bit_ndx,
    bit_loc_ndx,
    selected_bits_ndx;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION
    fp_key_change_location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_SET);

  /*
   *	Selected-bits: no influence
   */
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
  fem_selected_bits_info.sel_bit_msb = 3;
  res = soc_pb_pmf_fem_select_bits_set_unsafe(
          unit,
          fem,
          SOC_PB_PMF_FEM_PGM_FOR_ETH,
          &fem_selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   *	Maps to Action 2
   */
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
  fem_action_format_map_info.map_data = 0;
  fem_action_format_map_info.action_fomat_id = SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX;
  for (selected_bits_ndx = 0; selected_bits_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    res = soc_pb_pmf_fem_action_format_map_set_unsafe(
            unit,
            fem,
            SOC_PB_PMF_FEM_PGM_FOR_ETH,
            selected_bits_ndx,
            &fem_action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

  /*
   *	Set the action
   */
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
  res = soc_pb_pp_fp_action_type_to_pmf_convert(
          unit,
          action_type,
          &(fem_action_format_info.type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  fem_action_format_info.size = nof_bits;
  fem_action_format_info.base_value = base_value;

  /*
   *	1st stage: only one-to-one mapping from the lsb
   */
  bit_loc_lsb = 0;

  /*
   * Special case if the action is 'Change Key':
   *  Fill the key location in the LSB
   */
  if (action_type == SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY)
  {
    bit_loc_lsb = SOC_PB_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
    for (bit_loc_ndx = 0; bit_loc_ndx < bit_loc_lsb; ++bit_loc_ndx)
    {
      fem_action_format_info.bit_loc[bit_loc_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
      if (bit_loc_ndx < SOC_PB_PP_FP_BIT_LOC_CHANGE_KEY_OPERATION_FLD_SIZE)
      {
        /* Replace operation is encoded '10' for 'OR' */
        val = (bit_loc_ndx == 0)? 0 : 1;
      }
      else
      {
        /*
         * Get the Key location per PFG
         */
        SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
        res = soc_pb_pp_fp_database_get_unsafe(
                unit,
                db_id,
                &fp_database_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 86, exit);

        fp_key_change_location = SOC_PB_PP_FP_KEY_CHANGE_LOCATION_RESERVED;
        for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
        {
          if ((fp_database_info.supported_pfgs & (1 << pfg_ndx)) == 0)
          {
            continue;
          }
          fp_key_change_location = soc_pb_pp_sw_db_fp_loc_key_changed_get(unit, pfg_ndx);
        }

        val = SOC_SAND_GET_BIT(fp_key_change_location, bit_loc_ndx - SOC_PB_PP_FP_BIT_LOC_CHANGE_KEY_OPERATION_FLD_SIZE);
      }
      fem_action_format_info.bit_loc[bit_loc_ndx].val = val;
    }
  }

  bit_ndx = bit_loc_lsb;
  for (bit_loc_ndx = 0; bit_loc_ndx < nof_bits - bit_loc_lsb; ++bit_loc_ndx)
  {
    fem_action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
    fem_action_format_info.bit_loc[bit_ndx].val = extraction_lsb + bit_loc_ndx;

    /* Special case for Change Key: the MSBs must be 0 */
    if ((action_type == SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY)
        && (bit_loc_ndx >= soc_pb_pp_sw_db_fp_key_change_size_get(unit)))
    {
      fem_action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
      fem_action_format_info.bit_loc[bit_ndx].val = 0;
    }
    bit_ndx ++;
  }

  res = soc_pb_pmf_fem_action_format_set_unsafe(
          unit,
          fem,
          SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX,
          &fem_action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_set()", 0, 0);
}


uint32
  soc_pb_pp_fp_fem_configure(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  uint32                    cycle_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_VAL            *qual_info
  )
{
  uint32
    val,
    bit_loc_lsb,
    select_msb,
    select_lsb,
    expected_select,
    get_select,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    qual_length_no_padding,
    extraction_lsb,
    action_lsb,
    qual_lsb,
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    database_info;
  SOC_PB_PMF_FEM_NDX
    fem_ndx;
  uint32
    bit_loc_ndx,
    bit_ndx,
    fld_ndx,
    pfg_ndx,
    selected_bits_ndx;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input_info;
  SOC_PB_PMF_FEM_INPUT_SRC
    src;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  int32
    u64_ndx;
  uint8
    is_qual_bit_valid,
    is_action_applied;
  SOC_PB_PP_FP_KEY_LOCATION
    key_loc;
  SOC_PB_PP_FP_DATABASE_INFO
    fp_database_info;
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION
    fp_key_change_location;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_CONFIGURE);

  /*
   * Get the DB info
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * 2 cases: Direct Extraction entries or TCAM/Direct Table Database
   * Otherwise, return error
   * The configuration of the FEM must be anti-packet-flow
   */
  /*
   * Set all except FEM input
   */
  res = soc_pb_pp_fp_action_lsb_get(
          unit,
          entry_ndx->action_type[0],
          &database_info,
          &action_lsb
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_pb_pp_fp_action_type_max_size_get(
          unit,
          entry_ndx->action_type[0],
          &action_size_in_bits_max,
          &action_size_in_bits_in_fem
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PB_PMF_FEM_NDX_clear(&fem_ndx);
  fem_ndx.cycle_ndx = cycle_ndx;
  fem_ndx.id = fem_id_ndx;
  if (
      ((database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
        ||(database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE))
      && (entry_ndx->is_for_entry == FALSE)
      )
  {
    res = soc_pb_pp_fp_fem_set(
            unit,
            &fem_ndx,
            entry_ndx->action_type[0],
            action_lsb,
            action_size_in_bits_in_fem,
            0 /* Base value */,
            entry_ndx->db_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else if (
            (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
           && (entry_ndx->is_for_entry == TRUE)
          )
  {
    /*
     *  Set the action according to the params
     */
    SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
    res = soc_pb_pp_fp_action_type_to_pmf_convert(
            unit,
            entry_ndx->action_type[0],
            &(fem_action_format_info.type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    fem_action_format_info.size = action_size_in_bits_in_fem;
    fem_action_format_info.base_value = fem_info->base_val;

    /*
     *	Get the bit mapping
     */
    bit_loc_lsb = 0;
    bit_ndx = 0;

    /*
     * Special case if the action is 'Change Key':
     *  Fill the key location in the LSB
     */
    if (entry_ndx->action_type[0] == SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY)
    {
      bit_loc_lsb = SOC_PB_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
      for (bit_loc_ndx = 0; bit_loc_ndx < bit_loc_lsb; ++bit_loc_ndx)
      {
        fem_action_format_info.bit_loc[bit_loc_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
        if (bit_loc_ndx < SOC_PB_PP_FP_BIT_LOC_CHANGE_KEY_OPERATION_FLD_SIZE)
        {
          /* Replace operation is encoded '10' for 'OR'  */
          val = (bit_loc_ndx == 0)? 0 : 1;
        }
        else
        {
          /*
           * Get the Key location per PFG
           */
          SOC_PB_PP_FP_DATABASE_INFO_clear(&fp_database_info);
          res = soc_pb_pp_fp_database_get_unsafe(
                  unit,
                  entry_ndx->db_id,
                  &fp_database_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 86, exit);

          fp_key_change_location = SOC_PB_PP_FP_KEY_CHANGE_LOCATION_RESERVED;
          for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
          {
            if ((fp_database_info.supported_pfgs & (1 << pfg_ndx)) == 0)
            {
              continue;
            }
            fp_key_change_location = soc_pb_pp_sw_db_fp_loc_key_changed_get(unit, pfg_ndx);
          }

          val = SOC_SAND_GET_BIT(fp_key_change_location, bit_loc_ndx - SOC_PB_PP_FP_BIT_LOC_CHANGE_KEY_OPERATION_FLD_SIZE);
        }
        fem_action_format_info.bit_loc[bit_loc_ndx].val = val;
        bit_ndx ++;
      }
    }

    for (fld_ndx = 0; fld_ndx < fem_info->nof_fields; ++fld_ndx)
    {
      res = soc_pb_pp_fp_qual_lsb_and_length_get(
              unit,
              entry_ndx->db_id,
              fem_info->fld_ext[fld_ndx].type,
              &database_info,
              &extraction_lsb,
              &qual_length_no_padding
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

      for (bit_loc_ndx = 0; bit_loc_ndx < fem_info->fld_ext[fld_ndx].nof_bits; ++bit_loc_ndx)
      {
        fem_action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
        fem_action_format_info.bit_loc[bit_ndx].val = extraction_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + bit_loc_ndx;

        /* Special case for Change Key: the MSBs must be 0 */
        if ((entry_ndx->action_type[0] == SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY)
          && (bit_loc_ndx >= soc_pb_pp_sw_db_fp_key_change_size_get(unit)))
        {
          fem_action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
          fem_action_format_info.bit_loc[bit_ndx].val = 0;
        }

        /* Constant Value case */
        if (fem_info->fld_ext[fld_ndx].cst_val != 0)
        {
          fem_action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
          fem_action_format_info.bit_loc[bit_ndx].val = SOC_SAND_GET_BIT(fem_info->fld_ext[fld_ndx].cst_val, bit_loc_ndx);
        }

        bit_ndx ++;
      }
    }

    res = soc_pb_pmf_fem_action_format_set_unsafe(
            unit,
            &fem_ndx,
            SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX,
            &fem_action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /*
     * Set the 4b selection and the mapping according to the mask
     */
    /*
     *	Selected-bits: get the MSB of the four bits mask
     *  In practice, assumption the mask is only in the first uint32 of u64
     */
    res = soc_pb_pp_fp_qual_lsb_and_length_get(
            unit,
            entry_ndx->db_id,
            qual_info->type,
            &database_info,
            &qual_lsb,
            &qual_length_no_padding
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    for (u64_ndx = SOC_PB_PP_FP_FEM_SAND_U64_NOF_BITS - 1; u64_ndx > 0; u64_ndx--)
    {
      if (u64_ndx >= 32)
      {
        is_qual_bit_valid = SOC_SAND_GET_BIT(qual_info->is_valid.arr[1], (u64_ndx - 32));
      }
      else
      {
        is_qual_bit_valid = SOC_SAND_GET_BIT(qual_info->is_valid.arr[0], u64_ndx);
      }

      if (is_qual_bit_valid > 0)
      {
        break;
      }
    }
    SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
    fem_selected_bits_info.sel_bit_msb = SOC_SAND_MAX(qual_lsb + u64_ndx, 3);
    res = soc_pb_pmf_fem_select_bits_set_unsafe(
            unit,
            &fem_ndx,
            SOC_PB_PMF_FEM_PGM_FOR_ETH,
            &fem_selected_bits_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    /*
     *	Maps to Action 2 according to the mask
     */
    SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
    fem_action_format_map_info.map_data = 0;
    fem_action_format_map_info.action_fomat_id = SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX;
    select_msb = (fem_selected_bits_info.sel_bit_msb > qual_lsb)? fem_selected_bits_info.sel_bit_msb - qual_lsb : 0;
    select_lsb = (fem_selected_bits_info.sel_bit_msb > qual_lsb + 3)? fem_selected_bits_info.sel_bit_msb - qual_lsb - 3 : 0;
    expected_select = SOC_SAND_GET_BITS_RANGE(qual_info->is_valid.arr[0], select_msb, select_lsb)
                      & SOC_SAND_GET_BITS_RANGE(qual_info->val.arr[0], select_msb, select_lsb);
    for (selected_bits_ndx = 0; selected_bits_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
    {
      get_select = SOC_SAND_GET_BITS_RANGE(qual_info->is_valid.arr[0], select_msb, select_lsb) & selected_bits_ndx;
      if (expected_select == get_select)
      {
        is_action_applied = TRUE;
      }
      else
      {
        is_action_applied = FALSE;
      }

      if (is_action_applied == TRUE)
      {
        res = soc_pb_pmf_fem_action_format_map_set_unsafe(
                unit,
                &fem_ndx,
                SOC_PB_PMF_FEM_PGM_FOR_ETH,
                selected_bits_ndx,
                &fem_action_format_map_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
    }

  }
  else if (
            (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
           && (entry_ndx->is_for_entry == FALSE)
          )
  {
    /* Skip this step */
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR, 100, exit);
  }

  /*
   * Set the input according to the Database type
   */
  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
  {
    if ((database_info.supported_pfgs & (1 << pfg_ndx)) == 0)
    {
      continue;
    }
    fem_input_info.db_id = SOC_PB_PP_FP_NOF_DBS;
    fem_input_info.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_ETH;
    if (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
    {
      fem_input_info.db_id = entry_ndx->db_id; /* Importance for TCAM */
      src = SOC_TMC_PMF_FEM_INPUT_TCAM;
    }
    else if (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
    {
      src = SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL;
    }
    else if (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
    {
      res = soc_pb_pp_sw_db_fp_db_key_location_get(
              unit,
              entry_ndx->db_id,
              pfg_ndx,
              &key_loc
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);
      src = key_loc.fem_src;
    }
    else
    {
      src = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
    }
    if ((database_info.supported_pfgs & (1 << pfg_ndx)) == 0)
    {
      src = SOC_TMC_PMF_FEM_INPUT_SRC_NOP;
    }
    fem_input_info.src = src;
    lkp_profile_ndx.id = pfg_ndx;
    lkp_profile_ndx.cycle_ndx = cycle_ndx;
    res = soc_pb_pmf_db_fem_input_set_unsafe(
            unit,
            &lkp_profile_ndx,
            fem_id_ndx,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }

  /*
   * SW DB set
   */
  res = soc_pb_pp_sw_db_fp_fem_entry_set(
          unit,
          cycle_ndx,
          fem_id_ndx,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_configure()", 0, 0);
}

/*
 * Only for Direct Extraction entries
 */
uint32
  soc_pb_pp_fp_fem_configuration_de_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  uint32                    cycle_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL            *qual_info
  )
{
  uint32
    bit_ndx_lsb,
    local_lsb,
    key_bit_last,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  SOC_PB_PP_FP_DATABASE_INFO
    database_info;
  SOC_PB_PMF_FEM_NDX
    fem_ndx;
  uint32
    bit_ndx,
    selected_bits_ndx_mask_last,
    selected_bits_ndx;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info_ref,
    fem_action_format_map_info;
  int32
    fld_ndx,
    u64_ndx;
  uint8
    mask[SOC_PB_PP_FP_FEM_MASK_LENGTH_IN_BITS],
    val_mask[SOC_PB_PP_FP_FEM_MASK_LENGTH_IN_BITS];
  SOC_PB_PMF_FEM_BIT_LOC_TYPE
    type_previous;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_CONFIGURATION_GET);

  /*
   * Get the DB info
   */
  SOC_PB_PP_FP_DATABASE_INFO_clear(&database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * SW DB get
   */
  SOC_PB_PMF_FEM_NDX_clear(&fem_ndx);
  fem_ndx.cycle_ndx = cycle_ndx;
  fem_ndx.id = fem_id_ndx;
  res = soc_pb_pp_sw_db_fp_fem_entry_get(
          unit,
          cycle_ndx,
          fem_id_ndx,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_fp_action_type_max_size_get(
          unit,
          entry_ndx->action_type[0],
          &action_size_in_bits_max,
          &action_size_in_bits_in_fem
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);


  if (
       (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_EXTRACTION)
       && (entry_ndx->is_for_entry == TRUE)
      )
  {
    /*
     * Get the action
     */
    SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
    res = soc_pb_pmf_fem_action_format_get_unsafe(
            unit,
            &fem_ndx,
            SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX,
            &fem_action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_pb_pp_fp_action_type_from_pmf_convert(
            unit,
            fem_action_format_info.type,
            &(fem_info->type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    fem_info->base_val = fem_action_format_info.base_value;

    /*
     *	Get the bit mapping
     */
    type_previous = SOC_PB_NOF_PMF_FEM_BIT_LOC_TYPES;
    key_bit_last = 0;
    fld_ndx = -1; /* -1 Due to the ++ for each new field */

    /* Special case for the Change Key action */
    bit_ndx_lsb = 0;
    if (fem_info->type == SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY)
    {
      bit_ndx_lsb = SOC_PB_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
    }

    for (bit_ndx = bit_ndx_lsb; bit_ndx < action_size_in_bits_in_fem; ++bit_ndx)
    {
      if (fem_action_format_info.bit_loc[bit_ndx].type == SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY)
      {
        if (
            (type_previous == fem_action_format_info.bit_loc[bit_ndx].type)
            && (key_bit_last + 1 == fem_action_format_info.bit_loc[bit_ndx].val)
            && (fld_ndx >= 0)
           )
        {
          fem_info->fld_ext[fld_ndx].nof_bits ++;
          key_bit_last = fem_action_format_info.bit_loc[bit_ndx].val;
          continue;
        }
        else
        {
          /*
           * New Field
           */
          fld_ndx ++;
          type_previous = fem_action_format_info.bit_loc[bit_ndx].type;
          key_bit_last = fem_action_format_info.bit_loc[bit_ndx].val;
          res = soc_pb_pp_fp_qual_type_and_local_lsb_get(
                  unit,
                  key_bit_last,
                  &database_info,
                  &(fem_info->fld_ext[fld_ndx].type),
                  &(fem_info->fld_ext[fld_ndx].fld_lsb)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          fem_info->fld_ext[fld_ndx].nof_bits = 1;
        }
      }
      else if (fem_info->type != SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY) /* CST, none for Key change */
      {
        if (type_previous == fem_action_format_info.bit_loc[bit_ndx].type && fld_ndx >= 0)
        {
          SOC_SAND_SET_BIT(fem_info->fld_ext[fld_ndx].cst_val, fem_action_format_info.bit_loc[bit_ndx].val, fem_info->fld_ext[fld_ndx].nof_bits);
          fem_info->fld_ext[fld_ndx].nof_bits ++;
          continue;
        }
        else
        {
          /*
           * New field
           */
          fld_ndx ++;
          SOC_SAND_SET_BIT(fem_info->fld_ext[fld_ndx].cst_val, fem_action_format_info.bit_loc[bit_ndx].val, 0);
          fem_info->fld_ext[fld_ndx].nof_bits = 1;
          type_previous = fem_action_format_info.bit_loc[bit_ndx].type;
        }
      }
    }
    fem_info->nof_fields = fld_ndx + 1;


    /*
     * Get the mask
     */
    SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
    res = soc_pb_pmf_fem_select_bits_get_unsafe(
            unit,
            &fem_ndx,
            SOC_PB_PMF_FEM_PGM_FOR_ETH,
            &fem_selected_bits_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info_ref);
    res = soc_pb_pmf_fem_action_format_map_get_unsafe(
            unit,
            &fem_ndx,
            SOC_PB_PMF_FEM_PGM_FOR_ETH,
            0, /* selected_bits_ndx = 0 */
            &fem_action_format_map_info_ref
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    selected_bits_ndx_mask_last = 0;
    for (selected_bits_ndx = 0; selected_bits_ndx < SOC_PB_PP_FP_FEM_MASK_LENGTH_IN_BITS; ++selected_bits_ndx)
    {
      SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
      res = soc_pb_pmf_fem_action_format_map_get_unsafe(
              unit,
              &fem_ndx,
              SOC_PB_PMF_FEM_PGM_FOR_ETH,
              selected_bits_ndx,
              &fem_action_format_map_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      if (fem_action_format_map_info.action_fomat_id == fem_action_format_map_info_ref.action_fomat_id)
      {
        mask[selected_bits_ndx] = 0;
        val_mask[selected_bits_ndx] = 0;
      }
      else
      {
        selected_bits_ndx_mask_last = selected_bits_ndx;
        mask[selected_bits_ndx] = 0x1;
        val_mask[selected_bits_ndx] = (fem_action_format_map_info.action_fomat_id == SOC_PB_PMF_FEM_ETH_ACTION_FORMAT_NDX)? 1 : 0;
      }
    }

    res = soc_pb_pp_fp_qual_type_and_local_lsb_get(
            unit,
            fem_selected_bits_info.sel_bit_msb - 3 + selected_bits_ndx_mask_last,
            &database_info,
            &(qual_info->type),
            &(local_lsb)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    for (bit_ndx = SOC_PB_PP_FP_FEM_MASK_LENGTH_IN_BITS; bit_ndx > 0; bit_ndx--)
    {
      u64_ndx = local_lsb - (SOC_PB_PP_FP_FEM_MASK_LENGTH_IN_BITS - bit_ndx);
      SOC_SAND_SET_BIT(qual_info->is_valid.arr[0], mask[bit_ndx - 1], u64_ndx);
      SOC_SAND_SET_BIT(qual_info->val.arr[0], val_mask[bit_ndx - 1], u64_ndx);
      if (u64_ndx == 0)
      {
        break;
      }
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_configuration_get()", 0, 0);
}





uint32
  soc_pb_pp_fp_fem_reorganize(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    cycle_ndx
  )
{
  uint32
    output_size_in_bits,
    action_size_in_bits,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  uint32
    fem_id_ndx2,
    fem_id_ndx;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry2,
    fem_entry;
  SOC_PB_PMF_FEM_NDX
    fem_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_REORGANIZE);

  /*
   * Look at the two cycles for the relevant FEMs
   */
  for (fem_id_ndx = 0; fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_id_ndx)
  {
    SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
    res = soc_pb_pp_sw_db_fp_fem_entry_get(
            unit,
            cycle_ndx,
            fem_id_ndx,
            &fem_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (fem_entry.action_type[0] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
    {
      SOC_PB_PMF_FEM_NDX_clear(&fem_ndx);
      fem_ndx.cycle_ndx = cycle_ndx;
      fem_ndx.id = fem_id_ndx;
      res = soc_pb_pmf_fem_output_size_get(
              unit,
              &fem_ndx,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      /*
       * Look if a FEM can be inserted to it
       */
      for (fem_id_ndx2 = fem_id_ndx + 1; fem_id_ndx2 <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_id_ndx2)
      {
        SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
        res = soc_pb_pp_sw_db_fp_fem_entry_get(
                unit,
                cycle_ndx,
                fem_id_ndx2,
                &fem_entry2
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        if (fem_entry2.action_type[0] != SOC_PB_PP_NOF_FP_ACTION_TYPES)
        {
          res = soc_pb_pp_fp_action_type_max_size_get(
                  unit,
                  fem_entry2.action_type[0],
                  &action_size_in_bits,
                  &action_size_in_bits_in_fem
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          /*
           * Duplicate if possible
           */
          if (output_size_in_bits > action_size_in_bits_in_fem)
          {
            /* Avoid FEM 0 / 1 when the Base-offset is positive */
            if ((output_size_in_bits != 4) || (fem_entry2.is_base_positive[0] == 0))
            {
              res = soc_pb_pp_fp_fem_duplicate(
                      unit,
                      cycle_ndx,
                      fem_id_ndx2, /* Original */
                      fem_id_ndx   /* Destination */
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            }
          }
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_reorganize()", 0, 0);
}


uint32
  soc_pb_pp_fp_fem_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    pfg_ndx,
    cycle_ndx,
    fem_id_ndx;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input_info;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  uint32
    selected_bits_ndx;
  SOC_PB_PMF_FEM_NDX
    fem_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_REMOVE);

  /*
   * Look at the two cycles for the relevant FEMs
   */
  for (cycle_ndx = 0; cycle_ndx < SOC_PB_PMF_NOF_CYCLES; ++cycle_ndx)
  {
    for (fem_id_ndx = 0; fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_id_ndx)
    {
      SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
      res = soc_pb_pp_sw_db_fp_fem_entry_get(
              unit,
              cycle_ndx,
              fem_id_ndx,
              &fem_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      if (entry_ndx->db_id == fem_entry.db_id)
      {
        if (
            (entry_ndx->is_for_entry == FALSE)
            || (entry_ndx->entry_id == fem_entry.entry_id)
           )
        {
          /*
           * Disable this FEM: SW DB and NOP in input for all PFGs
           */
          SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
          res = soc_pb_pp_sw_db_fp_fem_entry_set(
                  unit,
                  cycle_ndx,
                  fem_id_ndx,
                  &fem_entry
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

          SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
          SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
          for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
          {
            fem_input_info.db_id = 0; /* No importance */
            fem_input_info.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_ETH;
            fem_input_info.src = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
            lkp_profile_ndx.id = pfg_ndx;
            lkp_profile_ndx.cycle_ndx = cycle_ndx;
            res = soc_pb_pmf_db_fem_input_set_unsafe(
                    unit,
                    &lkp_profile_ndx,
                    fem_id_ndx,
                    &fem_input_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          }

          /*
           * Set all the mapping to action 3
           */
          SOC_PB_PMF_FEM_NDX_clear(&fem_ndx);
          fem_ndx.cycle_ndx = cycle_ndx;
          fem_ndx.id = fem_id_ndx;

          SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
          fem_action_format_map_info.map_data = 0;
          fem_action_format_map_info.action_fomat_id = SOC_PB_PMF_FEM_ACTION_DEFAULT_NOP_3;
          for (selected_bits_ndx = 0; selected_bits_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
          {
            res = soc_pb_pmf_fem_action_format_map_set_unsafe(
                    unit,
                    &fem_ndx,
                    SOC_PB_PMF_FEM_PGM_FOR_ETH,
                    selected_bits_ndx,
                    &fem_action_format_map_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          }
        }
      }
    }

    res = soc_pb_pp_fp_fem_reorganize(
            unit,
            cycle_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_remove()", 0, 0);
}


/*********************************************************************
*     Compute the best configuration to add greedily Direct
 *     Extraction entries (preference to the new
 *     Database-ID). If set, set all the FEM (selected bits,
 *     actions) and its input. Look at the previous FEM
 *     configuration to shift the FEMs if necessary. The FEM
 *     input can be changed again upon the new TCAM DB
 *     creation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_fem_insert_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *fem_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    output_size_in_bits,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  uint8
    is_tag_action,
    is_fem_higher[SOC_PB_PMF_LOW_LEVEL_NOF_FEMS] = {FALSE},
    place_found;
  uint32
    action_id,
    action_ndx,
    fem_free_min,
    fem_free_first=0,
    fem_free_last,
    cycle_ndx,
    nof_free_fems[SOC_PB_PMF_NOF_CYCLES],
    cycle_to_use,
    fem_id_ndx;
  SOC_PB_PP_FP_FEM_ENTRY
    entry_replicated,
    fem_entry;
  SOC_PB_PP_FP_FEM_CYCLE
    fem_cycle_lcl;
  SOC_PB_PMF_FEM_NDX
    fem;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_INSERT_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   * Verify
   */
  res = soc_pb_pp_fp_fem_insert_verify(
          unit,
          entry_ndx,
          fem_cycle,
          fem_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Verify if there is a place, otherwise
   * return failure
   */
  res = soc_pb_pp_fp_fem_is_place_get_unsafe(
          unit,
          entry_ndx,
          fem_cycle,
          &place_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (place_found == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  /*
   * Set the cycle to insert it: if fixed cycle, this one
   * Otherwise, the one with least full FEMs
   */
  if (fem_cycle->is_cycle_fixed)
  {
    cycle_to_use = fem_cycle->cycle_id;
  }
  else
  {
    for (cycle_ndx = 0; cycle_ndx < SOC_PB_PMF_NOF_CYCLES; ++cycle_ndx)
    {
      SOC_PB_PP_FP_FEM_CYCLE_clear(&fem_cycle_lcl);
      fem_cycle_lcl.is_cycle_fixed = TRUE;
      fem_cycle_lcl.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);
      res = soc_pb_pp_fp_fem_is_place_get_unsafe(
              unit,
              entry_ndx,
              &fem_cycle_lcl,
              &place_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      nof_free_fems[cycle_ndx] = 0;

      if (place_found == FALSE)
      {
        continue;
      }

      for (fem_id_ndx = 0; fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_id_ndx)
      {
        SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
        res = soc_pb_pp_sw_db_fp_fem_entry_get(
                unit,
                cycle_ndx,
                fem_id_ndx,
                &fem_entry
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        if (fem_entry.action_type[0] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
        {
          nof_free_fems[cycle_ndx] ++;
        }
      }
    }

    if (nof_free_fems[0] > nof_free_fems[1])
    {
      cycle_to_use = 0;
    }
    else
    {
      cycle_to_use = 1;
    }
  }

  /*
   * If success, re-organize the FEMs:
   * Find the FEMs to transfer (i.e. duplicate) before inserting it
   * Find the first available FEM, and starts from it
   */
  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    if (entry_ndx->action_type[action_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
    {
      continue;
    }

    res = soc_pb_pp_fp_tag_action_type_convert(
            unit,
            entry_ndx->action_type[action_ndx],
            &is_tag_action,
            &action_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    if (is_tag_action == TRUE)
    {
      res = soc_pb_pp_fp_fem_tag_set_unsafe(
              unit,
              entry_ndx->db_id,
              entry_ndx->action_type[action_ndx]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

      continue;
    }
    /*
     * Get the first FEM to consider
     * Assumption that the first free FEM means there is no action afterwards
     */
    fem_free_min = 0;
    for (fem_id_ndx = 0; fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_id_ndx)
    {
      SOC_PB_PMF_FEM_NDX_clear(&fem);
      fem.cycle_ndx = cycle_to_use;
      fem.id = fem_id_ndx;
      res = soc_pb_pmf_fem_output_size_get(
              unit,
              &fem,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_pb_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[action_ndx],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      if (action_size_in_bits_in_fem <= output_size_in_bits)
      {
        /* Avoid FEM 0 / 1 when the Base-offset is positive */
        if ((output_size_in_bits != 4) || (fem_info->actions[action_ndx].base_val == 0))
        {
          fem_free_min = fem_id_ndx;
          break;
        }
      }
    }

    /* 
     * Init the higher-FEM array 
     */
    for (fem_id_ndx = 0; fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_id_ndx)
    {
      is_fem_higher[fem_id_ndx] = 0;
    }

    /*
     *  Assumption: there IS a free FEM
     */
    for (fem_free_first = fem_free_min; fem_free_first <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_free_first)
    {
      SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
      res = soc_pb_pp_sw_db_fp_fem_entry_get(
              unit,
              cycle_to_use,
              fem_free_first,
              &fem_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      SOC_PB_PP_FP_FEM_ENTRY_clear(&entry_replicated);
      SOC_PETRA_COPY(&entry_replicated, entry_ndx, SOC_PB_PP_FP_FEM_ENTRY, 1);
      entry_replicated.action_type[0] = entry_ndx->action_type[action_ndx];
      res = soc_pb_pp_fp_fem_is_fem_blocking_get(
              unit,
              &entry_replicated,
              &fem_entry,
              &(is_fem_higher[fem_free_first])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      if (fem_entry.action_type[0] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
      {
        break;
      }
    }

    /*
     * Duplication if necessary
     */
    fem_free_last = fem_free_first;
    if (fem_free_first != 0)
    {
      for (fem_id_ndx = fem_free_first; fem_id_ndx > 0; fem_id_ndx--)
      {
        if (is_fem_higher[fem_id_ndx - 1] == TRUE)
        {
          fem_free_last = fem_id_ndx - 1;

          /* Petra b code. Almost not in use. Ignore coverity defects */
          /* coverity[overrun-call] */
          res = soc_pb_pp_fp_fem_duplicate(
                  unit,
                  cycle_to_use,
                  fem_free_last,
                  fem_free_first
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

          fem_free_first = fem_id_ndx - 1;
        }
      }
    }

    /*
     * Configure the last free FEM
     */
    res = soc_pb_pp_fp_fem_configure(
            unit,
            fem_free_last,
            cycle_to_use,
            &entry_replicated,
            &(fem_info->actions[action_ndx]),
            &(fem_info->qual_vals[0])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_insert_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_fp_fem_insert_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *fem_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_INSERT_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_FEM_ENTRY, entry_ndx, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_FEM_CYCLE, fem_cycle, 15, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO, fem_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_insert_verify()", 0, 0);
}

uint32
  soc_pb_pp_fp_fem_is_place_get_for_cycle(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE            *fem_info,
    SOC_SAND_OUT uint8                    *place_found
  )
{
  uint32
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    output_size_in_bits,
    res = SOC_SAND_OK;
  uint32
    action_id,
    action_ndx,
    fem_id_ndx,
    other_cycle;
  uint8
    is_tag_action,
    forbidden_fem_cycle2_found = FALSE,
    fem_is_found = FALSE;
  SOC_PB_PP_FP_FEM_ENTRY
    fem_entry_high,
    fem_entry_low,
    fem_entry;
  SOC_PB_PMF_FEM_NDX
    fem;
  SOC_PB_PP_FP_ACTION_TYPE
    fem_action_type_bmp[SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX + 1];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(place_found);

  /*
   * Verify if there is a stronger / weaker FEM in the
   * other cycle.
   * Assumption: do not remove FEMs from the other cycle at the end (HW set)
   */
  other_cycle = SOC_PB_PMF_NOF_CYCLES - fem_info->cycle_id - 1;
  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    res = soc_pb_pp_fp_tag_action_type_convert(
            unit,
            entry_ndx->action_type[action_ndx],
            &is_tag_action,
            &action_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    if ((entry_ndx->action_type[action_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES) || (is_tag_action == TRUE))
    {
      continue;
    }

    for (fem_id_ndx = 0; (fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX) && (forbidden_fem_cycle2_found == FALSE); ++fem_id_ndx)
    {
      SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry_high);
      SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry_low);
      SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
      res = soc_pb_pp_sw_db_fp_fem_entry_get(
              unit,
              other_cycle,
              fem_id_ndx,
              &fem_entry
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      /*
       * Switch the Higher FEM info according to the cycle
       */
      if (other_cycle == 1)
      {
        SOC_PETRA_COPY(&fem_entry_high, &fem_entry, SOC_PB_PP_FP_FEM_ENTRY, 1);
        SOC_PETRA_COPY(&fem_entry_low, entry_ndx, SOC_PB_PP_FP_FEM_ENTRY, 1);
        fem_entry_low.action_type[0] = entry_ndx->action_type[action_ndx];
      }
      else
      {
        SOC_PETRA_COPY(&fem_entry_high, entry_ndx, SOC_PB_PP_FP_FEM_ENTRY, 1);
        SOC_PETRA_COPY(&fem_entry_low, &fem_entry, SOC_PB_PP_FP_FEM_ENTRY, 1);
        fem_entry_high.action_type[0] = entry_ndx->action_type[action_ndx];
      }

      res = soc_pb_pp_fp_fem_is_fem_blocking_get(
              unit,
              &fem_entry_high,
              &fem_entry_low,
              &forbidden_fem_cycle2_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }
  if (forbidden_fem_cycle2_found == TRUE)
  {
    *place_found = FALSE;
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  /*
   * No forbidden FEM found, now look if there is enough free FEMs
   * with the minimal size in the same cycle
   * Build a bitmap to fill all along
   */
  for (fem_id_ndx = 0; (fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX) && (fem_is_found == FALSE); ++fem_id_ndx)
  {
    /*
     *  Get the FEM info
     */
    SOC_PB_PP_FP_FEM_ENTRY_clear(&fem_entry);
    res = soc_pb_pp_sw_db_fp_fem_entry_get(
            unit,
            fem_info->cycle_id,
            fem_id_ndx,
            &fem_entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    fem_action_type_bmp[fem_id_ndx] = fem_entry.action_type[0];
  }

  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    res = soc_pb_pp_fp_tag_action_type_convert(
            unit,
            entry_ndx->action_type[action_ndx],
            &is_tag_action,
            &action_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    if ((entry_ndx->action_type[action_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES) || (is_tag_action == TRUE))
    {
      continue;
    }

    fem_is_found = FALSE;
    for (fem_id_ndx = 0; (fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX) && (fem_is_found == FALSE); ++fem_id_ndx)
    {
      /*
       *	Check the FEM has sufficient place to copy all the needed bits
       *  Get the FEM size and the action type size
       */
      SOC_PB_PMF_FEM_NDX_clear(&fem);
      fem.cycle_ndx = fem_info->cycle_id;
      fem.id = fem_id_ndx;
      res = soc_pb_pmf_fem_output_size_get(
              unit,
              &fem,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_pb_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[action_ndx],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /* Avoid FEM 0 / 1 when the Base-offset is positive */
      if (
          (action_size_in_bits_in_fem > output_size_in_bits)
          || ((output_size_in_bits == 4) && (entry_ndx->is_base_positive[action_ndx] == TRUE))
         )
      {
        continue;
      }

      if (fem_action_type_bmp[fem_id_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
      {
        fem_is_found = TRUE;
        fem_action_type_bmp[fem_id_ndx] = entry_ndx->action_type[action_ndx];
      }
    }
    if (fem_is_found == FALSE)
    {
      *place_found = FALSE;
      SOC_PB_PP_DO_NOTHING_AND_EXIT;
    }
  }

  *place_found = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_is_place_get_for_cycle()", 0, 0);
}


/*********************************************************************
*     Check out if there is an empty FEM for this entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_fem_is_place_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE            *fem_info,
    SOC_SAND_OUT uint8                    *place_found
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    cycle_ndx;
  uint8
    fem_is_found = FALSE;
  SOC_PB_PP_FP_FEM_CYCLE
    fem_info_cycle;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_IS_PLACE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(place_found);

  /*
   * Verify
   */
  res = soc_pb_pp_fp_fem_is_place_get_verify(
          unit,
          entry_ndx,
          fem_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  /*
   * If cycle fixed, try it.
   * If not, try both cycles and return success if at least one succeeded
   */
  for (cycle_ndx = 0; (cycle_ndx < SOC_PB_PMF_NOF_CYCLES) && (fem_is_found == FALSE); ++cycle_ndx)
  {
    if ((fem_info->is_cycle_fixed == TRUE) && (cycle_ndx != fem_info->cycle_id))
    {
      continue;
    }
    
    SOC_PB_PP_FP_FEM_CYCLE_clear(&fem_info_cycle);
    fem_info_cycle.is_cycle_fixed = TRUE; /* Look only for this cycle */
    fem_info_cycle.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);

    res = soc_pb_pp_fp_fem_is_place_get_for_cycle(
            unit,
            entry_ndx,
            &fem_info_cycle,
            &fem_is_found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  *place_found = fem_is_found;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_is_place_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_fp_fem_is_place_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE            *fem_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_IS_PLACE_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_FEM_ENTRY, entry_ndx, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FP_FEM_CYCLE, fem_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_is_place_get_verify()", 0, 0);
}

/*********************************************************************
*     Check out if there is an empty FEM for this entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_fp_fem_tag_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE           action_type
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    is_tag_action;
  uint32
    pfg_ndx,
    tag_action_ndx;
  SOC_PB_PMF_TAG_SRC_INFO
    tag_src;
  SOC_PB_PP_FP_DATABASE_INFO
    database_info;
  SOC_PB_PP_FP_KEY_LOCATION
    key_loc;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_TAG_SET_UNSAFE);

  /*
   * Verify
   */
  res = soc_pb_pp_fp_fem_tag_set_verify(
          unit,
          db_id_ndx,
          action_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Set this DB for this Tag for this PFG
   */
  res = soc_pb_pp_fp_tag_action_type_convert(
          unit,
          action_type,
          &is_tag_action,
          &tag_action_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (is_tag_action == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 30, exit);
  }

  SOC_PB_PP_FP_DATABASE_INFO_clear(&database_info);
  res = soc_pb_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (pfg_ndx = 0; pfg_ndx < SOC_PB_PP_FP_NOF_PFGS; ++pfg_ndx)
  {
    if ((database_info.supported_pfgs & (1 << pfg_ndx)) == 0)
    {
      continue;
    }

    soc_pb_pp_sw_db_fp_is_tag_used_set(
      unit,
      pfg_ndx,
      tag_action_ndx,
      db_id_ndx
    );
    
    /*
     * Set the HW  - get the Tag source
     */
    res = soc_pb_pp_sw_db_fp_db_key_location_get(
            unit,
            db_id_ndx,
            pfg_ndx,
            &key_loc
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    SOC_PB_PMF_TAG_SRC_INFO_clear(&tag_src);
    if (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_TCAM)
    {
      tag_src.val_src = SOC_PB_PMF_TAG_VAL_SRC_TCAM_0; /* PD1 by default */
    }
    else if (database_info.db_type == SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE)
    {
      tag_src.val_src = SOC_PB_PMF_TAG_VAL_SRC_DIR_TBL;
    }
    else
    {
      tag_src.val_src = key_loc.tag_src;
    }

    tag_src.stat_tag_lsb_position = 0;
    res = soc_pb_pmf_db_tag_select_set_unsafe(
            unit,
            pfg_ndx, /* Tag profile = PFG */
            tag_action_ndx,
            &tag_src
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_tag_set_unsafe()", 0, db_id_ndx);
}

uint32
  soc_pb_pp_fp_fem_tag_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE           action_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FP_FEM_TAG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PP_FP_FEM_DB_ID_NDX_MAX, SOC_PB_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(action_type, SOC_PB_PP_FP_FEM_ACTION_TYPE_MAX, SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_fp_fem_tag_set_verify()", 0, db_id_ndx);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_fp_fem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_fp_fem_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_fp_fem;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_fp_fem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_fp_fem_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_fp_fem;
}
uint32
  SOC_PB_PP_FP_FEM_ENTRY_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY *info
  )
{
  uint32
    action_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_strength, SOC_PB_PP_FP_FEM_DB_STRENGTH_MAX, SOC_PB_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_id, SOC_PB_PP_FP_FEM_DB_ID_MAX, SOC_PB_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_strength, SOC_PB_PP_FP_FEM_ENTRY_STRENGTH_MAX, SOC_PB_PP_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_id, SOC_PB_PP_FP_FEM_ENTRY_ID_MAX, SOC_PB_PP_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR, 14, exit);
  for (action_ndx = 0; action_ndx < SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    if (info->action_type[action_ndx] == SOC_PB_PP_NOF_FP_ACTION_TYPES)
    {
      continue;
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(info->action_type[action_ndx], SOC_PB_PP_FP_FEM_ACTION_TYPE_MAX, SOC_PB_PP_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 20 + action_ndx, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_FEM_ENTRY_verify()",0,0);
}

uint32
  SOC_PB_PP_FP_FEM_CYCLE_verify(
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FP_FEM_CYCLE_verify()",0,0);
}


void
  SOC_PB_PP_FP_FEM_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_FEM_ENTRY *info
  )
{
    uint32
      action_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_FEM_ENTRY_clear(info);

  /* Small fix due to the SOC_PPC_NOF split with Arad */
  for (action_ndx = 0; action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
    info->action_type[action_ndx] = SOC_PPC_NOF_FP_ACTION_TYPES_PB;
  }


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_FEM_CYCLE_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_FEM_CYCLE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

void
  SOC_PB_PP_FP_FEM_ENTRY_print(
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_FEM_ENTRY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PP_FP_FEM_CYCLE_print(
    SOC_SAND_IN  SOC_PB_PP_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FP_FEM_CYCLE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

