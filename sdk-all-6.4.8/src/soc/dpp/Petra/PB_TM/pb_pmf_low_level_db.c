/* $Id: pb_pmf_low_level_db.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_pmf_low_level.c
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

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_ce.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>

#include <soc/dpp/Petra/PB_TM/pb_tcam.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_sw_db_tcam_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

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
  
  SOC_PB_PMF_TCAM_OPERATION_ADD = 0,
  SOC_PB_PMF_TCAM_OPERATION_GET = 1,
  SOC_PB_PMF_TCAM_OPERATION_REMOVE = 2,
  SOC_PB_PMF_NOF_TCAM_OPERATIONS = 3

} SOC_PB_PMF_TCAM_OPERATION;


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
  soc_pb_pmf_low_level_db_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_db_init_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_low_level_tcam_bank_enable_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 bank_id,
    SOC_SAND_IN uint32 cycle,
    SOC_SAND_IN uint8 enabled
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_pb_pp_regs();
  SOC_PB_PP_FLD_SET(regs->ihb.pmfgeneral_configuration1_reg.enable_pass_bank_access[bank_id][cycle],
                 SOC_SAND_BOOL2NUM(enabled), 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_tcam_bank_enable_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_tcam_lookup_db_add_unsafe(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PB_PMF_LKP_PROFILE    *lkp_profile_ndx,
    SOC_SAND_IN uint32             tcam_db_id,
    SOC_SAND_IN SOC_PB_PMF_TCAM_KEY_SRC   key_src
  )
{
  uint32
    res = SOC_SAND_OK,
    key_src_val;
  uint32
    bank_id,
    shift;
  uint8
    is_used;
  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tbl_data;
  SOC_PB_TCAM_PREFIX
    prefix;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_get_unsafe(
          unit,
          lkp_profile_ndx->id,
          lkp_profile_ndx->cycle_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Determine the key source encoding for the database
   */
  switch(key_src)
  {
  case SOC_PB_PMF_TCAM_KEY_SRC_L2:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_L2_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_A:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_B_A_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_A_71_0_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_A_103_32_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_A_143_0_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_A_175_32_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_B_71_0_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_B_103_32_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_B_143_0_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_B_175_32_FLD_VAL;
    break;
  case SOC_PB_NOF_PMF_TCAM_KEY_SRCS:
    key_src_val = SOC_PB_PMF_TCAM_KEY_SRC_UNDEF_FLD_VAL;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR, 20, exit);
  }

  /*
   *  Set the key source and the database prefix for the relevant banks
   */
  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    res = soc_pb_tcam_db_is_bank_used_unsafe(
            unit,
            tcam_db_id,
            bank_id,
            &is_used
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (is_used)
    {
      res = soc_pb_tcam_db_bank_prefix_get_unsafe(
              unit,
              tcam_db_id,
              bank_id,
              &prefix
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      shift = 4 - prefix.length;
      tbl_data.bank_db_id_and_value[bank_id] = (prefix.length == 4) ? 0 : SOC_SAND_BITS_MASK(shift - 1, 0);
      tbl_data.bank_db_id_or_value[bank_id]  = prefix.bits << shift;
      tbl_data.bank_key_select[bank_id]      = key_src_val;
    }
  }

  res = soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_set_unsafe(
          unit,
          lkp_profile_ndx->id,
          lkp_profile_ndx->cycle_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_lookup_db_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_tcam_lookup_result_config_unsafe(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PB_PMF_LKP_PROFILE    *lkp_profile_ndx,
    SOC_SAND_IN uint32             tcam_db_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_DEVICE access_device
  )
{
  uint32
    res = SOC_SAND_OK,
    bank_bmp,
    bank_ndx1,
    bank_ndx2;
  uint32
    bank_id,
    nof_banks;
  uint8
    is_used;
  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_get_unsafe(
          unit,
          lkp_profile_ndx->id,
          lkp_profile_ndx->cycle_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  bank_bmp  = 0;
  bank_ndx1 = 0;
  bank_ndx2 = 0;
  nof_banks = 0;
  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    res = soc_pb_tcam_db_is_bank_used_unsafe(
            unit,
            tcam_db_id,
            bank_id,
            &is_used
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (is_used)
    {
      bank_bmp |= SOC_SAND_BIT(bank_id);
      bank_ndx2 = bank_ndx1;
      bank_ndx1 = bank_id;
      ++nof_banks;
    }
  }

  switch (access_device)
  {
  case SOC_PB_TCAM_ACCESS_DEVICE_PD1:
    tbl_data.tcam_pd1_members = bank_bmp;
    break;

  case SOC_PB_TCAM_ACCESS_DEVICE_PD2:
    tbl_data.tcam_pd2_members = ((nof_banks != 1) ? bank_ndx2 : bank_ndx1) << 2 | bank_ndx1;
    break;

  case SOC_PB_TCAM_ACCESS_DEVICE_SEL3:
    tbl_data.tcam_sel3_member = bank_ndx1;
    break;

  case SOC_PB_TCAM_ACCESS_DEVICE_SEL4:
    tbl_data.tcam_sel4_member = bank_ndx1;
    break;

  default:
    break;
  }

  res = soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_set_unsafe(
          unit,
          lkp_profile_ndx->id,
          lkp_profile_ndx->cycle_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_lookup_result_config_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_tcam_lookup_enable_set_unsafe(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN uint32            tcam_db_id,
    SOC_SAND_IN SOC_PB_TCAM_ACCESS_CYCLE cycle,
    SOC_SAND_IN uint8            enabled
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    bank_id;
  uint8
    is_used;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    res = soc_pb_tcam_db_is_bank_used_unsafe(
            unit,
            tcam_db_id,
            bank_id,
            &is_used
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (is_used)
    {
      res = soc_pb_pmf_low_level_tcam_bank_enable_set_unsafe(
              unit,
              bank_id,
              cycle,
              enabled
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_lookup_enable_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Select the key to be searched in the TCAM.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_tcam_lookup_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                  *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            tcam_db_id,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO      *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  )
{
  uint32
    pd1,
    pd2,
    sel3,
    sel4,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tbl_data;
  uint32
    bank_ndx,
    shift;
  uint8
    is_used;
  SOC_PB_TCAM_PREFIX
    prefix;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_LOOKUP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_profile_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pmf_tcam_lookup_set_verify(
          unit,
          lkp_profile_ndx,
          tcam_db_id,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get-Modify-Set the table entry
   */
  res = soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_get_unsafe(
          unit,
          lkp_profile_ndx->id,
          lkp_profile_ndx->cycle_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(info->key_src)
  {
  case SOC_PB_PMF_TCAM_KEY_SRC_L2:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_L2_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_A:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_A_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_A_71_0_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_A_103_32_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_A_143_0_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_A_175_32_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_71_0_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_103_32_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_143_0_FLD_VAL;
    break;
  case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_175_32_FLD_VAL;
    break;
  case SOC_PB_NOF_PMF_TCAM_KEY_SRCS:
    fld_val = SOC_PB_PMF_TCAM_KEY_SRC_UNDEF_FLD_VAL;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR, 20, exit);
  }

  /*
   *	Set the AND value for the n prefix bits, and the value in the OR
   *  for each bank the database is present in
   */
  for (bank_ndx = 0; bank_ndx < SOC_PB_TCAM_NOF_BANKS; ++bank_ndx)
  {
    res = soc_pb_tcam_db_is_bank_used_unsafe(
            unit,
            tcam_db_id,
            bank_ndx,
            &is_used
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
    if (is_used)
    {
      res = soc_pb_tcam_db_bank_prefix_get_unsafe(
              unit,
              tcam_db_id,
              bank_ndx,
              &prefix
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
      shift = 4 - prefix.length;
      tbl_data.bank_key_select[bank_ndx]      = fld_val;
      tbl_data.bank_db_id_and_value[bank_ndx] = (prefix.length == 4) ?
                                                  0 : SOC_SAND_BITS_MASK(shift - 1, 0);
      tbl_data.bank_db_id_or_value[bank_ndx]  = prefix.bits << shift;
    }
  }

  /*
   *	Set the TCAM Priority Decoder / Selector if allowed
   */
  pd1 = 0;
  pd2 = 0;
  sel3 = 0;
  sel4 = 0;
  for (bank_ndx = 0; bank_ndx < SOC_PB_PMF_TCAM_NOF_BANKS; ++bank_ndx)
  {
    if (info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_PD1].bank_id_enabled[bank_ndx] == TRUE)
    {
      SOC_SAND_SET_BIT(pd1, 0x1, bank_ndx);
    }
    if (info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_PD2].bank_id_enabled[bank_ndx] == TRUE)
    {
      /*
       *	Should be twice at max
       */
      SOC_SAND_SET_BIT(pd2, 0x1, bank_ndx);
    }
    if (info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_SEL3].bank_id_enabled[bank_ndx] == TRUE)
    {
      /*
       *	Should be once at max
       */
      sel3 = bank_ndx;
    }
    if (info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_SEL4].bank_id_enabled[bank_ndx] == TRUE)
    {
      /*
       *	Should be once at max
       */
      sel4 = bank_ndx;
    }
  }

  tbl_data.tcam_pd1_members = pd1;
  tbl_data.tcam_pd2_members = pd2;
  tbl_data.tcam_sel3_member = sel3;
  tbl_data.tcam_sel4_member = sel4;

  res = soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_set_unsafe(
          unit,
          lkp_profile_ndx->id,
          lkp_profile_ndx->cycle_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_lookup_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_tcam_lookup_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            db_id_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_LOOKUP_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_LKP_PROFILE, lkp_profile_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PMF_LOW_LEVEL_DB_ID_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_DB_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_TCAM_KEY_SELECTION_INFO, info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_lookup_set_verify()", 0, db_id_ndx);
}

uint32
  soc_pb_pmf_tcam_lookup_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            db_id_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_LOOKUP_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_LKP_PROFILE, lkp_profile_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(db_id_ndx, SOC_PB_PMF_LOW_LEVEL_DB_ID_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_DB_ID_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_lookup_get_verify()", 0, db_id_ndx);
}

/*********************************************************************
*     Select the key to be searched in the TCAM.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_tcam_lookup_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            db_id_ndx,
    SOC_SAND_OUT SOC_PB_PMF_TCAM_KEY_SELECTION_INFO    *info
  )
{
  uint32
    pd1,
    pd2,
    sel3,
    sel4,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tbl_data;
  uint32
    bit_ndx,
    bank_ndx;
  uint8
    resources[SOC_PB_TCAM_NOF_BANKS];
  uint8
    is_valid=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_LOOKUP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_profile_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO_clear(info);

  res = soc_pb_pmf_tcam_lookup_get_verify(
          unit,
          lkp_profile_ndx,
          db_id_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get the table entry
   */
  res = soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_get_unsafe(
          unit,
          lkp_profile_ndx->id,
          lkp_profile_ndx->cycle_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the Banks and the prefix of this Database
   */
  res = soc_pb_sw_db_tcam_use_type_resources_get(
          unit,
          SOC_PB_TCAM_USER_FP,
          db_id_ndx,
          resources
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (bank_ndx = 0; bank_ndx < SOC_PB_TCAM_NOF_BANKS; ++bank_ndx)
  {
    if (resources[bank_ndx] != SOC_PB_TCAM_NOF_CYCLES)
    {
      switch(tbl_data.bank_key_select[bank_ndx])
      {
      case SOC_PB_PMF_TCAM_KEY_SRC_L2_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_L2;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_B_A_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_A;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_A_71_0;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_A_103_32;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_A_143_0;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_A_175_32;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_71_0;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_103_32;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_143_0;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32_FLD_VAL:
        fld_val = SOC_PB_PMF_TCAM_KEY_SRC_B_175_32;
        break;
      case SOC_PB_PMF_TCAM_KEY_SRC_UNDEF_FLD_VAL:
        fld_val = SOC_PB_NOF_PMF_TCAM_KEY_SRCS;
        break;


      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR, 20, exit);
      }
      info->key_src = fld_val;

      /*
       *	Get the AND value for the n prefix bits, and the value in the OR
       */
      fld_val = 0;
      for (bit_ndx = 0; bit_ndx < SOC_PB_PMF_TCAM_PREFIX_SIZE_IN_BITS; ++bit_ndx)
      {
        if (SOC_SAND_GET_BIT(tbl_data.bank_db_id_and_value[bank_ndx], bit_ndx) == 0x0)
        {
          /*
           *	Indicates the prefix length
           */
          fld_val = SOC_SAND_GET_BITS_RANGE(tbl_data.bank_db_id_or_value[bank_ndx], SOC_PB_PMF_TCAM_PREFIX_MSB, bit_ndx);
          break;
        }
      }
      /*
       *	Do not look at the other banks
       */
      is_valid = TRUE;
      break;
    }
  }
  info->is_valid = is_valid;

  pd1 = tbl_data.tcam_pd1_members;
  pd2 = tbl_data.tcam_pd2_members;
  sel3 = tbl_data.tcam_sel3_member;
  sel4 = tbl_data.tcam_sel4_member;

  /*
   *	Get the PD/Sel in any case
   */
  for (bank_ndx = 0; bank_ndx < SOC_PB_PMF_TCAM_NOF_BANKS; ++bank_ndx)
  {
    if (SOC_SAND_GET_BIT(pd1, bank_ndx) == TRUE)
    {
      info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_PD1].bank_id_enabled[bank_ndx] = TRUE;
    }
    else
    {
      info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_PD1].bank_id_enabled[bank_ndx] = FALSE;
    }

    if (SOC_SAND_GET_BIT(pd2, bank_ndx) == TRUE)
    {
      info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_PD2].bank_id_enabled[bank_ndx] = TRUE;
    }
    else
    {
      info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_PD2].bank_id_enabled[bank_ndx] = FALSE;
    }

    if (bank_ndx == sel3)
    {
      info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_SEL3].bank_id_enabled[bank_ndx] = TRUE;
    }
    else
    {
      info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_SEL3].bank_id_enabled[bank_ndx] = FALSE;
    }

    if (bank_ndx == sel4)
    {
      info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_SEL4].bank_id_enabled[bank_ndx] = TRUE;
    }
    else
    {
      info->bank_prio.bank_sel[SOC_PB_PMF_BANK_SEL_SEL4].bank_id_enabled[bank_ndx] = FALSE;
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_lookup_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Add an entry (in the format of a PMF TCAM Key) in a TCAM
 *     Bank.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_tcam_entry_add_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID                 *entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA                     *tcam_data,
    SOC_SAND_IN  uint8                            allow_new_bank,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                 *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ndx;
  SOC_PB_TCAM_ENTRY
    entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(tcam_data);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_TCAM_ENTRY_clear(&entry);

  res = soc_pb_pmf_tcam_entry_add_verify(
          unit,
          entry_ndx,
          tcam_data,
          allow_new_bank
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_sw_db_tcam_use_key_src_set(
          unit,
          SOC_PB_TCAM_USER_FP,
          entry_ndx->db_id,
          tcam_data->key.format.pmf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (ndx = 0; ndx < SOC_PB_TCAM_ENTRY_MAX_LEN; ++ndx)
  {
    entry.value[ndx] = tcam_data->key.data.val[ndx];
    entry.mask[ndx]  = tcam_data->key.data.mask[ndx];
  }

  res = soc_pb_tcam_managed_db_entry_add_unsafe(
          unit,
          entry_ndx->db_id,
          entry_ndx->entry_id,
          (uint16) tcam_data->priority,
          &entry,
          tcam_data->output.val,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_entry_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_tcam_entry_add_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA                  *tcam_data,
    SOC_SAND_IN  uint8                          allow_new_bank
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_ENTRY_ADD_VERIFY);

  res = SOC_PB_PMF_TCAM_ENTRY_ID_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_TCAM_DATA, tcam_data, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_entry_add_verify()", 0, 0);
}

/*********************************************************************
*     Get an entry (in the format of a PMF TCAM Key) in a TCAM
 *     Bank.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_tcam_entry_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_OUT SOC_PB_PMF_TCAM_DATA                  *tcam_data,
    SOC_SAND_OUT uint8                         *is_found
  )
{
  uint32
    key_size_in_bits,
    res = SOC_SAND_OK;
  uint32
    tcam_val_ndx;
  uint16
    priority_get;
  SOC_PB_PMF_TCAM_KEY_SRC
    key_src;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    key_size;
  SOC_PB_TCAM_ENTRY
    entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(tcam_data);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  SOC_PB_PMF_TCAM_DATA_clear(tcam_data);

  res = soc_pb_pmf_tcam_entry_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_tcam_db_entry_get_unsafe(
          unit,
          entry_ndx->db_id,
          entry_ndx->entry_id,
          &priority_get,
          &entry,
          &tcam_data->output.val,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Parse the entry, if found
   */
  if (*is_found)
  {
    res = soc_pb_sw_db_tcam_use_key_src_get(
            unit,
            SOC_PB_TCAM_USER_FP,
            entry_ndx->db_id,
            &key_src
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    tcam_data->key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    tcam_data->key.format.pmf  = key_src;

    res = soc_pb_tcam_key_size_get(
            unit,
            &(tcam_data->key.format),
            &key_size,
            &key_size_in_bits
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    tcam_data->key.size = key_size;

    for (tcam_val_ndx = 0; tcam_val_ndx < SOC_PB_TCAM_RULE_NOF_UINT32S_MAX; ++tcam_val_ndx)
    {
      tcam_data->key.data.val[tcam_val_ndx] = entry.value[tcam_val_ndx];
      tcam_data->key.data.mask[tcam_val_ndx] = entry.mask[tcam_val_ndx];
    }

    tcam_data->priority = priority_get;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_entry_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_tcam_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_ENTRY_GET_VERIFY);

  res = SOC_PB_PMF_TCAM_ENTRY_ID_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_entry_get_verify()", 0, 0);
}

/*********************************************************************
*     Remove an entry (in the format of a PMF TCAM Key) from a
 *     TCAM.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_tcam_entry_remove_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx,
    SOC_SAND_OUT uint8                         *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);

  res = soc_pb_pmf_tcam_entry_remove_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_tcam_managed_db_entry_remove_unsafe(
          unit,
          entry_ndx->db_id,
          entry_ndx->entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  *is_found = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_entry_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_tcam_entry_remove_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID              *entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_ENTRY_REMOVE_VERIFY);

  res = SOC_PB_PMF_TCAM_ENTRY_ID_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_entry_remove_verify()", 0, 0);
}


/*********************************************************************
*     Select the key source for the direct table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_db_direct_tbl_key_src_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_KEY_SRC         key_src
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_profile_ndx);

  res = soc_pb_pmf_db_direct_tbl_key_src_set_verify(
          unit,
          lkp_profile_ndx,
          key_src
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Set the entry data
   */
  tbl_data.direct_db_and_value = SOC_PB_PMF_DIRECT_TBL_BITWISE_AND_VAL;
  tbl_data.direct_db_or_value = 0;

  fld_val = 0;
  switch(key_src)
  {
  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_9_0:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_9_0_FLD_VAL;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_19_10:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_19_10_FLD_VAL;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_29_20:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_29_20_FLD_VAL;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_39_30:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_39_30_FLD_VAL;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0_FLD_VAL;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_19_10:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_19_10_FLD_VAL;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_29_20:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_29_20_FLD_VAL;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_39_30:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_39_30_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_KEY_SRC_OUT_OF_RANGE_ERR, 10, exit);
  }
  tbl_data.direct_key_select = fld_val;

  /*
   *	Set the table according to the cycle id
   */
  if (lkp_profile_ndx->cycle_ndx == 0)
  {
    res = soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
            unit,
            lkp_profile_ndx->id,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else /* Cycle id = 1*/
  {
    res = soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
            unit,
            lkp_profile_ndx->id,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_direct_tbl_key_src_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_db_direct_tbl_key_src_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_KEY_SRC         key_src
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_LKP_PROFILE, lkp_profile_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(key_src, SOC_PB_PMF_LOW_LEVEL_KEY_SRC_MAX, SOC_PB_PMF_LOW_LEVEL_KEY_SRC_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_direct_tbl_key_src_set_verify()", 0, 0);
}

uint32
  soc_pb_pmf_db_direct_tbl_key_src_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_DIRECT_TBL_KEY_SRC_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_LKP_PROFILE, lkp_profile_ndx, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_direct_tbl_key_src_get_verify()", 0, 0);
}

/*********************************************************************
*     Select the key source for the direct table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_db_direct_tbl_key_src_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_OUT SOC_PB_PMF_DIRECT_TBL_KEY_SRC         *key_src
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tbl_data;
  SOC_PB_PMF_DIRECT_TBL_KEY_SRC
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_DIRECT_TBL_KEY_SRC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_profile_ndx);
  SOC_SAND_CHECK_NULL_INPUT(key_src);

  res = soc_pb_pmf_db_direct_tbl_key_src_get_verify(
          unit,
          lkp_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get the table according to the cycle id
   */
  if (lkp_profile_ndx->cycle_ndx == 0)
  {
    res = soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
            unit,
            lkp_profile_ndx->id,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else /* Cycle id = 1*/
  {
    res = soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
            unit,
            lkp_profile_ndx->id,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  /*
   *	Get the entry data
   */
  fld_val = 0;
  switch(tbl_data.direct_key_select)
  {
  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_9_0_FLD_VAL:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_9_0;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_19_10_FLD_VAL:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_19_10;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_29_20_FLD_VAL:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_29_20;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_39_30_FLD_VAL:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_39_30;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0_FLD_VAL:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_19_10_FLD_VAL:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_19_10;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_29_20_FLD_VAL:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_29_20;
    break;

  case SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_39_30_FLD_VAL:
    fld_val = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_39_30;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_KEY_SRC_OUT_OF_RANGE_ERR, 30, exit);
  }
  *key_src = fld_val;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_direct_tbl_key_src_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Add an entry to the Database direct table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_db_direct_tbl_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA            *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_DIRECT_TBL_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  res = soc_pb_pmf_db_direct_tbl_entry_set_verify(
          unit,
          entry_ndx,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  tbl_data.direct_action_table = data->val;
  res = soc_pb_pp_ihb_direct_action_table_tbl_set_unsafe(
          unit,
          entry_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_direct_tbl_entry_set_unsafe()", entry_ndx, 0);
}

uint32
  soc_pb_pmf_db_direct_tbl_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA            *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_DIRECT_TBL_ENTRY_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PMF_LOW_LEVEL_ENTRY_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_ENTRY_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIRECT_TBL_DATA, data, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_direct_tbl_entry_set_verify()", entry_ndx, 0);
}

uint32
  soc_pb_pmf_db_direct_tbl_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_DIRECT_TBL_ENTRY_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, SOC_PB_PMF_LOW_LEVEL_ENTRY_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_ENTRY_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_direct_tbl_entry_get_verify()", entry_ndx, 0);
}

uint32
  soc_pb_pmf_db_direct_tbl_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            entry_ndx,
    SOC_SAND_OUT SOC_PB_PMF_DIRECT_TBL_DATA            *data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_DIRECT_TBL_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  res = soc_pb_pmf_db_direct_tbl_entry_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_pp_ihb_direct_action_table_tbl_get_unsafe(
          unit,
          entry_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  data->val = tbl_data.direct_action_table;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_direct_tbl_entry_get_unsafe()", entry_ndx, 0);
}

uint32
  SOC_PB_PMF_TCAM_ENTRY_ID_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_id, SOC_PB_PMF_LOW_LEVEL_DB_ID_MAX, SOC_PB_PMF_LOW_LEVEL_DB_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_id, SOC_PB_PMF_LOW_LEVEL_ENTRY_ID_MAX, SOC_PB_PMF_LOW_LEVEL_ENTRY_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_TCAM_ENTRY_ID_verify()",0,0);
}

uint32
  SOC_PB_PMF_TCAM_DATA_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_KEY, &(info->key), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->priority, SOC_PB_PMF_LOW_LEVEL_PRIORITY_MAX, SOC_PB_PMF_LOW_LEVEL_PRIORITY_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_OUTPUT, &(info->output), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_TCAM_DATA_verify()",0,0);
}

uint32
  SOC_PB_PMF_LKP_PROFILE_verify(
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->id, SOC_PB_PMF_LOW_LEVEL_ID_MAX, SOC_PB_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cycle_ndx, SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_LKP_PROFILE_verify()",0,0);
}

uint32
  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Allow Key source NOF = NOP
   */
  SOC_SAND_ERR_IF_ABOVE_MAX(info->key_src, SOC_PB_NOF_PMF_TCAM_KEY_SRCS, SOC_PB_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_TCAM_RESULT_INFO, &(info->bank_prio), 20, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_TCAM_KEY_SELECTION_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_TCAM_BANK_SELECTION_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_BANK_SELECTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_TCAM_BANK_SELECTION_verify()",0,0);
}

uint32
  SOC_PB_PMF_TCAM_RESULT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_RESULT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PMF_TCAM_NOF_LKP_RESULTS; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_TCAM_BANK_SELECTION, &(info->bank_sel[ind]), 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_TCAM_RESULT_INFO_verify()",0,0);
}




uint32
  SOC_PB_PMF_DIRECT_TBL_DATA_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->val, SOC_PB_PMF_LOW_LEVEL_DIRECT_TBL_VAL_MAX, SOC_PB_PMF_LOW_LEVEL_DIRECT_TBL_VAL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_DIRECT_TBL_DATA_verify()",0,0);
}

void
  SOC_PB_PMF_TCAM_ENTRY_ID_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_ENTRY_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_ENTRY_ID_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_LKP_PROFILE_clear(
    SOC_SAND_OUT SOC_PB_PMF_LKP_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_LKP_PROFILE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_KEY_SELECTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_BANK_SELECTION_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_BANK_SELECTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_BANK_SELECTION_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_RESULT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_RESULT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_RESULT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}




void
  SOC_PB_PMF_TCAM_DATA_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_DATA_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIRECT_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIRECT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_DIRECT_TBL_DATA_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1


const char*
  SOC_PB_PMF_DIRECT_TBL_KEY_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_KEY_SRC enum_val
  )
{
  return SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_to_string(enum_val);
}

void
  SOC_PB_PMF_TCAM_ENTRY_ID_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_ENTRY_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_ENTRY_ID_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_LKP_PROFILE_print(
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_LKP_PROFILE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SELECTION_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_BANK_SELECTION_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_BANK_SELECTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_BANK_SELECTION_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_RESULT_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_RESULT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_RESULT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  SOC_PB_PMF_TCAM_DATA_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_DATA_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



void
  SOC_PB_PMF_DIRECT_TBL_DATA_print(
    SOC_SAND_IN  SOC_PB_PMF_DIRECT_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_DIRECT_TBL_DATA_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

