/* $Id: pb_pmf_low_level_fem_tag.c,v 1.8 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_sw_db_tcam_mgmt.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_4  (4)
#define SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_14 (14)
#define SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_17 (17)

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


uint32
  soc_pb_pmf_low_level_fem_tag_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PMF_FEM_NDX
    fem_ndx;
  uint32
    lookup_profile_ndx,
    action_fomat_ndx,
    selected_bits_ndx,
    fem_pgm_ndx,
    cycle_ndx,
    fem_id_ndx;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    action_format_map;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    action_format;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *	Enable all the FEM operations
   */
  fld_val = 0x1;
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.destination_action_enable, fld_val, 10, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.dp_action_enable, fld_val, 11, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.dp_meter_cmd_action_enable, fld_val, 12, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.tc_action_enable, fld_val, 13, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.forward_action_enable, fld_val, 14, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.snoop_action_enable, fld_val, 15, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.mirror_action_enable, fld_val, 16, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.outbound_mirror_disable_action_enable, fld_val, 17, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.exclude_source_action_enable, fld_val, 18, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.ingress_shaping_action_enable, fld_val, 19, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.meter_action_enable, fld_val, 20, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.counter_action_enable, fld_val, 21, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.statistics_action_enable, fld_val, 22, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.outlif_action_enable, fld_val, 23, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.second_pass_data_update_action_enable, fld_val, 24, exit);
  SOC_PB_FLD_SET(regs->ihb.pmfgeneral_configuration0_reg.second_pass_profile_update_action_enable, fld_val, 25, exit);

  /* 
   * Set the LAG-LB to be disabled
   */
  fld_val = 0x1;
  SOC_PB_FLD_SET(regs->ihb.dbg_pmf_force_action1_reg.force_action, fld_val, 26, exit);
  fld_val = 20;
  SOC_PB_FLD_SET(regs->ihb.dbg_pmf_force_action1_reg.force_action_type, fld_val, 27, exit);
  fld_val = 0;
  SOC_PB_FLD_SET(regs->ihb.dbg_pmf_force_action2_reg.force_action_value, fld_val, 28, exit);


  /*
   *	Define the default (Raw) FEM program to do nothing
   */
  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  for (lookup_profile_ndx = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_ETH; lookup_profile_ndx <= SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW; ++lookup_profile_ndx)
  {
    lkp_profile_ndx.id = lookup_profile_ndx;
    SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input);
    fem_input.db_id = SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES; /* No TCAM use */
    fem_input.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_RAW;
    fem_input.src = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
    SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map);
    for (cycle_ndx = 0; cycle_ndx <= SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX; ++cycle_ndx)
    {
      lkp_profile_ndx.cycle_ndx = cycle_ndx;
      for (fem_id_ndx = 0; fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_id_ndx)
      {
        res = soc_pb_pmf_db_fem_input_set_unsafe(
                unit,
                &lkp_profile_ndx,
                fem_id_ndx,
                &fem_input
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
      }
    }
  }
  /*
   *	Init the FEMs
   */
  SOC_PB_PMF_FEM_NDX_clear(&fem_ndx);
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(&action_format);
  action_format.type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
  action_format.size = 0;
  for (cycle_ndx = 0; cycle_ndx <= SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX; ++cycle_ndx)
  {
    for (fem_id_ndx = 0; fem_id_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_id_ndx)
    {
      fem_ndx.cycle_ndx = cycle_ndx;
      fem_ndx.id = fem_id_ndx;

      /*
       *	Init the Action format map table - to prevent TM collision using the Action 0
       *  Here we use the action 1 or 3
       */
      for (fem_pgm_ndx = 0; fem_pgm_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_MAX; ++fem_pgm_ndx)
      {
        SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&action_format_map);
        if ((fem_pgm_ndx == SOC_PB_PMF_FEM_PGM_FOR_ETH) || (fem_pgm_ndx == SOC_PB_PMF_FEM_PGM_FOR_RAW))
        {
          if ((cycle_ndx == 1) && (fem_id_ndx == 7))
          {
            action_format_map.action_fomat_id = SOC_PB_PMF_FEM_ACTION_DEFAULT_DEST_1;
          }
          else
          {
            action_format_map.action_fomat_id = SOC_PB_PMF_FEM_ACTION_DEFAULT_NOP_3;
          }
        }
        for (selected_bits_ndx = 0; selected_bits_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
        {
          res = soc_pb_pmf_fem_action_format_map_set_unsafe(
                  unit,
                  &fem_ndx,
                  fem_pgm_ndx,
                  selected_bits_ndx,
                  &action_format_map
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
      }

      /*
       *	Init the Action format table
       */
      for (action_fomat_ndx = 0; action_fomat_ndx <= SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_MAX; ++action_fomat_ndx)
      {
        res = soc_pb_pmf_fem_action_format_set_unsafe(
                unit,
                &fem_ndx,
                action_fomat_ndx,
                &action_format
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
    }
  }

  /* 
   * Init the Route-History-Bitmap to be 0 
   * by taking a TCAM-result. Possible issues if multiple ACLs 
   * are defined in Ethernet PMF-Programs. 
   */ 
  SOC_PB_REG_SET(regs->ihb.stacking_route_history_select_reg, 0x5555555, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_fem_tag_init_unsafe()", 0, 0);
}

/* $Id: pb_pmf_low_level_fem_tag.c,v 1.8 Broadcom SDK $
 *	Return the FEM output size (4, 14, 17 bits)
 */
uint32
  soc_pb_pmf_fem_output_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX      *fem_ndx,
    SOC_SAND_OUT uint32            *output_size_in_bits
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_OUTPUT_SIZE_GET);

  switch(fem_ndx->id)
  {
  case 0:
  case 1:
    *output_size_in_bits = 4;
    break;
  
  case 2:
  case 3:
  case 4:
    *output_size_in_bits = 14;
    break;

  case 5:
  case 6:
  case 7:
    *output_size_in_bits = 17;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_output_size_get()", 0, 0);
}

/*********************************************************************
*     Select the input for each Field Extraction MACRO. The FEM
 *     processes the PCL results, and extracts possible actions
 *     to be applied on the packet. INPUT SOC_SAND_IN
 *     SOC_PPD_PMF_LKP_PROFILE *lkp_profile_ndx - Lookup-Profile
 *     information (id and cycle). SOC_SAND_IN uint32 *fem_ndx -
 *     FEM (Field Extraction Macro) Index. Range: 0 - 7.
 *     (Soc_petra-B) SOC_SAND_IN SOC_PPD_PMF_FEM_INPUT_INFO *info - FEM
 *     input parameters: the FEM-Program-Id and the FEM-Input
 *     source. RETURNS OK or Error indicationREMARKS None.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_db_fem_input_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            fem_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO             *info
  )
{
  uint32
    fld_val,
    cycle_ndx,
    res = SOC_SAND_OK;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_FEM_INPUT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_profile_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pmf_db_fem_input_set_verify(
          unit,
          lkp_profile_ndx,
          fem_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Set the table info
   */
  tbl_data.program = info->pgm_id;

  fld_val = 0;
  switch(info->src)
  {
  case SOC_PB_PMF_FEM_INPUT_SRC_A_31_0:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_31_0_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_A_47_16:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_47_16_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_A_63_32:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_63_32_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_A_79_48:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_79_48_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_A_95_64:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_95_64_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_31_0:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_31_0_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_47_16:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_47_16_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_63_32:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_63_32_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_79_48:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_79_48_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_95_64:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_95_64_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM:
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM_0:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_0_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM_1:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_1_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM_2:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_2_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM_3:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_3_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_NOP:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_NOP_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_SRC_OUT_OF_RANGE_ERR, 10, exit);
  }
  tbl_data.key_select = fld_val;


  /*
   *	Set the table data
   */
  cycle_ndx = lkp_profile_ndx->cycle_ndx;
  res = soc_pb_ihb_fem_key_profile_resolved_data_tbl_set_unsafe(
          unit,
          fem_ndx,
          cycle_ndx,
          lkp_profile_ndx->id,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (info->db_id != SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES)
  {
    res = soc_pb_sw_db_tcam_use_fem_is_present_set(
            unit,
            SOC_PB_TCAM_USER_FP,
            info->db_id,
            fem_ndx,
            cycle_ndx,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_fem_input_set_unsafe()", 0, fem_ndx);
}

uint32
  soc_pb_pmf_db_fem_input_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            fem_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_FEM_INPUT_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_LKP_PROFILE, lkp_profile_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fem_ndx, SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX, SOC_PB_PMF_LOW_LEVEL_FEM_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_INPUT_INFO, info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_fem_input_set_verify()", 0, fem_ndx);
}

uint32
  soc_pb_pmf_db_fem_input_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            fem_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_FEM_INPUT_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_LKP_PROFILE, lkp_profile_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fem_ndx, SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX, SOC_PB_PMF_LOW_LEVEL_FEM_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_fem_input_get_verify()", 0, fem_ndx);
}

/*********************************************************************
*     Select the input for each Field Extraction MACRO. The FEM
 *     processes the PCL results, and extracts possible actions
 *     to be applied on the packet. INPUT SOC_SAND_IN
 *     SOC_PPD_PMF_LKP_PROFILE *lkp_profile_ndx - Lookup-Profile
 *     information (id and cycle). SOC_SAND_IN uint32 *fem_ndx -
 *     FEM (Field Extraction Macro) Index. Range: 0 - 7.
 *     (Soc_petra-B) SOC_SAND_IN SOC_PPD_PMF_FEM_INPUT_INFO *info - FEM
 *     input parameters: the FEM-Program-Id and the FEM-Input
 *     source. RETURNS OK or Error indicationREMARKS None.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_db_fem_input_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_LKP_PROFILE                *lkp_profile_ndx,
    SOC_SAND_IN  uint32                            fem_ndx,
    SOC_SAND_OUT SOC_PB_PMF_FEM_INPUT_INFO             *info
  )
{
  uint32
    fld_val,
    cycle_ndx,
    res = SOC_SAND_OK;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_FEM_INPUT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_profile_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PMF_FEM_INPUT_INFO_clear(info);

  res = soc_pb_pmf_db_fem_input_get_verify(
          unit,
          lkp_profile_ndx,
          fem_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get the table data
   */
  cycle_ndx = lkp_profile_ndx->cycle_ndx;
  res = soc_pb_ihb_fem_key_profile_resolved_data_tbl_get_unsafe(
          unit,
          fem_ndx,
          cycle_ndx,
          lkp_profile_ndx->id,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  /*
   *	Get the table info
   */
  info->pgm_id = tbl_data.program;

  switch(tbl_data.key_select)
  {
  case SOC_PB_PMF_FEM_INPUT_SRC_A_31_0_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_31_0;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_A_47_16_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_47_16;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_A_63_32_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_63_32;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_A_79_48_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_79_48;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_A_95_64_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_A_95_64;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_31_0_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_31_0;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_47_16_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_47_16;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_63_32_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_63_32;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_79_48_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_79_48;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_B_95_64_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_B_95_64;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM_0_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_0;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM_1_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_1;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM_2_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_2;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_TCAM_3_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_TCAM_3;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL;
    break;
  case SOC_PB_PMF_FEM_INPUT_SRC_NOP_FLD_VAL:
    fld_val = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_SRC_OUT_OF_RANGE_ERR, 20, exit);
  }
  info->src = fld_val;



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_fem_input_get_unsafe()", 0, fem_ndx);
}


STATIC
  uint32
    soc_pb_pmf_db_tag_fld_select(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  uint32                            tag_profile_ndx,
      SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx,
      SOC_SAND_OUT SOC_PETRA_REG_FIELD                    **key_sel_fld
    )
{
  SOC_PB_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_TAG_FLD_SELECT);

  regs = soc_pb_regs();
  pp_regs = soc_pb_pp_regs();

  /*
   *	Select the right register according to the Tag type
   */
  switch(tag_type_ndx)
  {
  case SOC_PB_PMF_TAG_TYPE_STAT_TAG:
    *key_sel_fld = &(regs->ihb.statistics_key_select_reg.statistics_key_select[tag_profile_ndx]);
    break;

  case SOC_PB_PMF_TAG_TYPE_LAG_LB_KEY:
    *key_sel_fld = &(regs->ihb.lag_lb_key_select_reg.lag_lb_key_select[tag_profile_ndx]);
    break;

  case SOC_PB_PMF_TAG_TYPE_ECMP_LB_KEY:
    *key_sel_fld = &(pp_regs->ihb.ecmp_lb_key_select_reg.ecmp_lb_key_select[tag_profile_ndx]);
    break;

  case SOC_PB_PMF_TAG_TYPE_STACK_RT_HIST:
    *key_sel_fld = &(regs->ihb.stacking_route_history_select_reg.stacking_route_history_select[tag_profile_ndx]);
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TAG_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_tag_fld_select()", tag_profile_ndx, tag_type_ndx);
}

/*********************************************************************
*     Set the Tag value source per Tag profile. The TAG source
 *     designates the data location to take the Tag value from,
 *     and where only the lsb bits are used (the number of bits
 *     used depends on the Tag-Type).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_db_tag_select_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            tag_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO               *tag_src
  )
{
  uint32
    fld_ndx,
    reg_ndx,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REG_FIELD
    *key_sel_fld;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_TAG_SELECT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tag_src);

  regs = soc_petra_regs();

  res = soc_pb_pmf_db_tag_select_set_verify(
          unit,
          tag_profile_ndx,
          tag_type_ndx,
          tag_src
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Select the right register according to the Tag type
   */
  res = soc_pb_pmf_db_tag_fld_select(
          unit,
          tag_profile_ndx,
          tag_type_ndx,
          &key_sel_fld
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  fld_val = 0;
  switch(tag_src->val_src)
  {
  case SOC_PB_PMF_TAG_VAL_SRC_A_29_0:
    fld_val = SOC_PB_PMF_TAG_VAL_SRC_A_29_0_FLD_VAL;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_A_61_32:
    fld_val = SOC_PB_PMF_TAG_VAL_SRC_A_61_32_FLD_VAL;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_B_29_0:
    fld_val = SOC_PB_PMF_TAG_VAL_SRC_B_29_0_FLD_VAL;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_B_61_32:
    fld_val = SOC_PB_PMF_TAG_VAL_SRC_B_61_32_FLD_VAL;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_TCAM_0:
    fld_val = SOC_PB_PMF_TAG_VAL_SRC_TCAM_0_FLD_VAL;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_TCAM_1:
    fld_val = SOC_PB_PMF_TAG_VAL_SRC_TCAM_1_FLD_VAL;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_TCAM_2:
    fld_val = SOC_PB_PMF_TAG_VAL_SRC_TCAM_2_FLD_VAL;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_DIR_TBL:
    fld_val = SOC_PB_PMF_TAG_VAL_SRC_DIR_TBL_FLD_VAL;
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_VAL_SRC_OUT_OF_RANGE_ERR, 20, exit);
  }

  /*
   *	Set the field value
   */
  SOC_PB_IMPLICIT_FLD_SET(*key_sel_fld, fld_val, 30, exit);

  /*
   *	Set the Statistic-Tag shift in case of a S-Tag type
   */
  if (tag_type_ndx == SOC_PB_PMF_TAG_TYPE_STAT_TAG)
  {
    reg_ndx = tag_profile_ndx / SOC_PB_PMF_NOF_STAT_SHIFT_FLDS_PER_REG;
    fld_ndx = tag_profile_ndx % SOC_PB_PMF_NOF_STAT_SHIFT_FLDS_PER_REG;
    SOC_PB_FLD_SET(regs->ihb.statistics_key_shift_reg_reg[reg_ndx].statistics_key_shift[fld_ndx], tag_src->stat_tag_lsb_position, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_tag_select_set_unsafe()", tag_profile_ndx, 0);
}

uint32
  soc_pb_pmf_db_tag_select_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            tag_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO               *tag_src
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_TAG_SELECT_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tag_profile_ndx, SOC_PB_PMF_LOW_LEVEL_TAG_PROFILE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_TAG_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_type_ndx, SOC_PB_PMF_LOW_LEVEL_TAG_TYPE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_TAG_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_TAG_SRC_INFO, tag_src, 30, exit);

  if ((tag_src->stat_tag_lsb_position != 0) && (tag_type_ndx != SOC_PB_PMF_TAG_TYPE_STAT_TAG))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_STAT_TAG_LSB_POSITION_OUT_OF_RANGE_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_tag_select_set_verify()", tag_profile_ndx, 0);
}

uint32
  soc_pb_pmf_db_tag_select_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            tag_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_TAG_SELECT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(tag_profile_ndx, SOC_PB_PMF_LOW_LEVEL_TAG_PROFILE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_TAG_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(tag_type_ndx, SOC_PB_PMF_LOW_LEVEL_TAG_TYPE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_TAG_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_tag_select_get_verify()", tag_profile_ndx, 0);
}

/*********************************************************************
*     Set the Tag value source per Tag profile. The TAG source
 *     designates the data location to take the Tag value from,
 *     and where only the lsb bits are used (the number of bits
 *     used depends on the Tag-Type).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_db_tag_select_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            tag_profile_ndx,
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE                   tag_type_ndx,
    SOC_SAND_OUT SOC_PB_PMF_TAG_SRC_INFO               *tag_src
  )
{
  uint32
    reg_ndx,
    fld_ndx,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REG_FIELD
    *key_sel_fld;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DB_TAG_SELECT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tag_src);

  regs = soc_petra_regs();

  SOC_PB_PMF_TAG_SRC_INFO_clear(tag_src);

  res = soc_pb_pmf_db_tag_select_get_verify(
          unit,
          tag_profile_ndx,
          tag_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Select the right register according to the Tag type
   */
  res = soc_pb_pmf_db_tag_fld_select(
          unit,
          tag_profile_ndx,
          tag_type_ndx,
          &key_sel_fld
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the field value
   */
  SOC_PB_IMPLICIT_FLD_GET(*key_sel_fld, fld_val, 20, exit);

  switch(fld_val)
  {
  case SOC_PB_PMF_TAG_VAL_SRC_A_29_0_FLD_VAL:
    tag_src->val_src = SOC_PB_PMF_TAG_VAL_SRC_A_29_0;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_A_61_32_FLD_VAL:
    tag_src->val_src = SOC_PB_PMF_TAG_VAL_SRC_A_61_32;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_B_29_0_FLD_VAL:
    tag_src->val_src = SOC_PB_PMF_TAG_VAL_SRC_B_29_0;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_B_61_32_FLD_VAL:
    tag_src->val_src = SOC_PB_PMF_TAG_VAL_SRC_B_61_32;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_TCAM_0_FLD_VAL:
    tag_src->val_src = SOC_PB_PMF_TAG_VAL_SRC_TCAM_0;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_TCAM_1_FLD_VAL:
    tag_src->val_src = SOC_PB_PMF_TAG_VAL_SRC_TCAM_1;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_TCAM_2_FLD_VAL:
    tag_src->val_src = SOC_PB_PMF_TAG_VAL_SRC_TCAM_2;
    break;

  case SOC_PB_PMF_TAG_VAL_SRC_DIR_TBL_FLD_VAL:
    tag_src->val_src = SOC_PB_PMF_TAG_VAL_SRC_DIR_TBL;
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_VAL_SRC_OUT_OF_RANGE_ERR, 20, exit);
  }

  /*
   *	Get the Statistic-Tag shift in case of a S-Tag type
   */
  if (tag_type_ndx == SOC_PB_PMF_TAG_TYPE_STAT_TAG)
  {
    reg_ndx = tag_profile_ndx / SOC_PB_PMF_NOF_STAT_SHIFT_FLDS_PER_REG;
    fld_ndx = tag_profile_ndx % SOC_PB_PMF_NOF_STAT_SHIFT_FLDS_PER_REG;
    SOC_PB_FLD_GET(regs->ihb.statistics_key_shift_reg_reg[reg_ndx].statistics_key_shift[fld_ndx], tag_src->stat_tag_lsb_position, 40, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_db_tag_select_get_unsafe()", tag_profile_ndx, 0);
}

/*********************************************************************
*     Set the location of 4 bits from the FEM-key that select
 *     the performed action format for this key (configure the
 *     Select-4-bits table).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_fem_select_bits_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_SELECT_BITS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fem_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pmf_fem_select_bits_set_verify(
          unit,
          fem_ndx,
          fem_pgm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Select the MSB of the four bits
   */
  tbl_data.bit_select = info->sel_bit_msb;
  res = soc_pb_ihb_fem_program_resolved_data_tbl_set_unsafe(
          unit,
          fem_ndx->id,
          fem_ndx->cycle_ndx,
          fem_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_select_bits_set_unsafe()", 0, fem_pgm_ndx);
}

uint32
  soc_pb_pmf_fem_select_bits_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_SELECT_BITS_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_NDX, fem_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fem_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_SELECTED_BITS_INFO, info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_select_bits_set_verify()", 0, fem_pgm_ndx);
}

uint32
  soc_pb_pmf_fem_select_bits_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_SELECT_BITS_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_NDX, fem_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fem_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_select_bits_get_verify()", 0, fem_pgm_ndx);
}

/*********************************************************************
*     Set the location of 4 bits from the FEM-key that select
 *     the performed action format for this key (configure the
 *     Select-4-bits table).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_fem_select_bits_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_OUT SOC_PB_PMF_FEM_SELECTED_BITS_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_SELECT_BITS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fem_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(info);

  res = soc_pb_pmf_fem_select_bits_get_verify(
          unit,
          fem_ndx,
          fem_pgm_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_ihb_fem_program_resolved_data_tbl_get_unsafe(
          unit,
          fem_ndx->id,
          fem_ndx->cycle_ndx,
          fem_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  info->sel_bit_msb = tbl_data.bit_select;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_select_bits_get_unsafe()", 0, fem_pgm_ndx);
}

/*********************************************************************
*     Set the location of 4 bits from the FEM-key that select
 *     the performed action format for this key (configure the
 *     Select-4-bits table).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_fem_action_format_map_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fem_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pmf_fem_action_format_map_set_verify(
          unit,
          fem_ndx,
          fem_pgm_ndx,
          selected_bits_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Set the entry offset and write to the table
   */
  entry_offset = selected_bits_ndx + (fem_pgm_ndx << SOC_PB_PMF_FEM_SEL_BITS_SIZE_IN_BITS);

  tbl_data.map_index = info->action_fomat_id;
  tbl_data.map_data = info->map_data;
  res = soc_pb_ihb_fem_map_index_table_tbl_set_unsafe(
          unit,
          fem_ndx->id,
          fem_ndx->cycle_ndx,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_action_format_map_set_unsafe()", 0, fem_pgm_ndx);
}

uint32
  soc_pb_pmf_fem_action_format_map_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_NDX, fem_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fem_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(selected_bits_ndx, SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_OUT_OF_RANGE_ERR, 30, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO, info, 40, exit);

exit:  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_action_format_map_set_verify()", 0, fem_pgm_ndx);
}

uint32
  soc_pb_pmf_fem_action_format_map_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_NDX, fem_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fem_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_FEM_PGM_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(selected_bits_ndx, SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_action_format_map_get_verify()", 0, fem_pgm_ndx);
}

/*********************************************************************
*     Set the location of 4 bits from the FEM-key that select
 *     the performed action format for this key (configure the
 *     Select-4-bits table).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_fem_action_format_map_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            fem_pgm_ndx,
    SOC_SAND_IN  uint32                            selected_bits_ndx,
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fem_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(info);

  res = soc_pb_pmf_fem_action_format_map_get_verify(
          unit,
          fem_ndx,
          fem_pgm_ndx,
          selected_bits_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Get the entry offset and read from the table
   */
  entry_offset = selected_bits_ndx + (fem_pgm_ndx << SOC_PB_PMF_FEM_SEL_BITS_SIZE_IN_BITS);

  res = soc_pb_ihb_fem_map_index_table_tbl_get_unsafe(
          unit,
          fem_ndx->id,
          fem_ndx->cycle_ndx,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  info->action_fomat_id = tbl_data.map_index;
  info->map_data = tbl_data.map_data;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_action_format_map_get_unsafe()", 0, fem_pgm_ndx);
}

/*********************************************************************
*     Configure the format of the each action done by the
 *     Field Extraction Macro. Each FEM can perform up to four
 *     different actions.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_fem_action_format_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            action_fomat_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO     *info
  )
{
  uint32
    fld_extr_encoded,
    type_fld_val,
    res = SOC_SAND_OK;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    offset_data;
  uint32
    bit_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_ACTION_FORMAT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fem_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pmf_fem_action_format_set_verify(
          unit,
          fem_ndx,
          action_fomat_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Init the table data - set all the values to '0'
   */
  for (bit_ndx = 0; bit_ndx < SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE; ++bit_ndx)
  {
    tbl_data.field_select_map[bit_ndx] = 0;
    tbl_data.field_select_map[bit_ndx] |= SOC_SAND_SET_BITS_RANGE(SOC_PB_PMF_FEM_CST_ENCODED_BIT,
      SOC_PB_PMF_FEM_CST_ENCODED_BIT_MSB, SOC_PB_PMF_FEM_CST_ENCODED_BIT_LSB);
  }

  /*
   *	Set the table entry data
   */
  switch(info->type)
  {
  case SOC_PB_PMF_FEM_ACTION_TYPE_DEST:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_DEST_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_DP:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_DP_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_TC:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_TC_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_TRAP:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_TRAP_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_SNP:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_SNP_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_IS:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_IS_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_METER:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_METER_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_COUNTER:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_COUNTER_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_STAT:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_STAT_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_OUTLIF:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_OUTLIF_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM_FLD_VAL;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_NOP:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_NOP_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }
  tbl_data.action_type = type_fld_val;

  tbl_data.offset_index = action_fomat_ndx;
  offset_data.offset = info->base_value;

  /*
   *	Build the encoded Bit location
   */
  for (bit_ndx = 0; bit_ndx < info->size; ++bit_ndx)
  {
    fld_extr_encoded = 0;
    switch(info->bit_loc[bit_ndx].type)
    {
    case SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST:
      fld_extr_encoded = info->bit_loc[bit_ndx].val;
      fld_extr_encoded |= SOC_SAND_SET_BITS_RANGE(SOC_PB_PMF_FEM_CST_ENCODED_BIT,
        SOC_PB_PMF_FEM_CST_ENCODED_BIT_MSB, SOC_PB_PMF_FEM_CST_ENCODED_BIT_LSB);
      break;

    case SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY:
      fld_extr_encoded = info->bit_loc[bit_ndx].val;
      break;

    case SOC_PB_PMF_FEM_BIT_LOC_TYPE_MAP_DATA:
      fld_extr_encoded = info->bit_loc[bit_ndx].val;
      SOC_SAND_SET_BIT(fld_extr_encoded, 0x1, SOC_PB_PMF_FEM_MAP_DATA_ENCODED_BIT);
      break;
    
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TYPE_OUT_OF_RANGE_ERR, 20, exit);
    }
    tbl_data.field_select_map[bit_ndx] = fld_extr_encoded;
  }

  /*
   *	Write into the tables
   */
  res = soc_pb_ihb_fem_map_table_tbl_set_unsafe(
          unit,
          fem_ndx->id,
          fem_ndx->cycle_ndx,
          action_fomat_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((fem_ndx->id != 0) && (fem_ndx->id != 1))
  {
    res = soc_pb_ihb_fem_offset_table_tbl_set_unsafe(
            unit,
            fem_ndx->id,
            fem_ndx->cycle_ndx,
            action_fomat_ndx,
            &offset_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_action_format_set_unsafe()", 0, action_fomat_ndx);
}


uint32
  soc_pb_pmf_fem_action_format_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            action_fomat_ndx,
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK,
    output_size_in_bits;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_ACTION_FORMAT_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_NDX, fem_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(action_fomat_ndx, SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_ACTION_FORMAT_INFO, info, 30, exit);

  /*
   *	Check if this FEM has an offset option
   */
  if ((fem_ndx->id < SOC_PB_PMF_FEM_FOR_OFFSET_MIN) && (info->base_value != 0))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_FEM_INVALID_FOR_OFFSET_ERR, 40, exit);
  }

  /*
   *	Check if the size is coherent with the FEM type
   */
  res = soc_pb_pmf_fem_output_size_get(
          unit,
          fem_ndx,
          &output_size_in_bits
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (info->size > output_size_in_bits)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_FEM_OUTPUT_SIZE_ERR, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_action_format_set_verify()", 0, action_fomat_ndx);
}

uint32
  soc_pb_pmf_fem_action_format_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            action_fomat_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_ACTION_FORMAT_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_NDX, fem_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(action_fomat_ndx, SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_action_format_get_verify()", 0, action_fomat_ndx);
}

/*********************************************************************
*     Configure the format of the each action done by the
 *     Field Extraction Macro. Each FEM can perform up to four
 *     different actions.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_fem_action_format_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX                    *fem_ndx,
    SOC_SAND_IN  uint32                            action_fomat_ndx,
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_FORMAT_INFO     *info
  )
{
  uint32
    action_size = 0,
    type_fld_val,
    res = SOC_SAND_OK,
    output_size_in_bits;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    offset_data;
  uint32
    bit_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_FEM_ACTION_FORMAT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fem_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(info);

  res = soc_pb_pmf_fem_action_format_get_verify(
          unit,
          fem_ndx,
          action_fomat_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Get the data from the tables
   */
  res = soc_pb_ihb_fem_map_table_tbl_get_unsafe(
          unit,
          fem_ndx->id,
          fem_ndx->cycle_ndx,
          action_fomat_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Take the offset only if the FEM-ID is superior to 2
   *  (i.e. the option of an offset exists)
   */
  if (fem_ndx->id >= SOC_PB_PMF_FEM_FOR_OFFSET_MIN)
  {
    res = soc_pb_ihb_fem_offset_table_tbl_get_unsafe(
            unit,
            fem_ndx->id,
            fem_ndx->cycle_ndx,
            action_fomat_ndx,
            &offset_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    info->base_value = offset_data.offset;
  }
  else
  {
    info->base_value = 0;
  }


  /*
   *	Set the action type
   */
  switch(tbl_data.action_type)
  {
  case SOC_PB_PMF_FEM_ACTION_TYPE_DEST_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_DEST;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_17;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_DP_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_DP;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_4;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_TC_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_TC;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_4;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_TRAP_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_TRAP;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_14;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_SNP_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_SNP;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_14;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_14;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_4;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_4;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_IS_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_IS;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_17;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_METER_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_METER;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_14;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_COUNTER_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_COUNTER;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_14;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_STAT_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_STAT;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_14;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_OUTLIF_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_OUTLIF;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_17;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_DATA;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_17;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_2ND_PASS_PGM;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_17;
    break;
  case SOC_PB_PMF_FEM_ACTION_TYPE_NOP_FLD_VAL:
    type_fld_val = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
    action_size = SOC_PB_PMF_LOW_LEVEL_FEM_ACTION_SIZE_4;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }
  info->type = type_fld_val;

  /*
   *	Get the FEM size according to the FEM id
   */
  res = soc_pb_pmf_fem_output_size_get(
          unit,
          fem_ndx,
          &output_size_in_bits
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  info->size = action_size;


  /*
   *	Build the encoded Bit location
   */
  for (bit_ndx = 0; bit_ndx < info->size; ++bit_ndx)
  {
    if (SOC_SAND_GET_BIT(tbl_data.field_select_map[bit_ndx], 5) == 0)
    {
      /*
       *	FEM-Key source
       */
      info->bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
      info->bit_loc[bit_ndx].val = SOC_SAND_GET_BITS_RANGE(tbl_data.field_select_map[bit_ndx], 4, 0);
    }
    else if (SOC_SAND_GET_BITS_RANGE(tbl_data.field_select_map[bit_ndx], 5, 2) == SOC_PB_PMF_FEM_MAP_DATA_ENCODING)
    {
      /*
       *	Map-Data source
       */
      info->bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_MAP_DATA;
      info->bit_loc[bit_ndx].val = SOC_SAND_GET_BITS_RANGE(tbl_data.field_select_map[bit_ndx], 1, 0);
    }
    else
    {
      /*
       *	Constant source
       */
      info->bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
      info->bit_loc[bit_ndx].val = SOC_SAND_GET_BIT(tbl_data.field_select_map[bit_ndx], 0);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_fem_action_format_get_unsafe()", 0, action_fomat_ndx);
}



uint32
  SOC_PB_PMF_FEM_INPUT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pgm_id, SOC_PB_PMF_LOW_LEVEL_PGM_ID_MAX, SOC_PB_PMF_LOW_LEVEL_PGM_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src, SOC_PB_PMF_LOW_LEVEL_SRC_MAX, SOC_PB_PMF_LOW_LEVEL_SRC_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_id, SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES, SOC_PB_PMF_LOW_LEVEL_DB_ID_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_FEM_INPUT_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_TAG_SRC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->val_src, SOC_PB_PMF_LOW_LEVEL_VAL_SRC_MAX, SOC_PB_PMF_LOW_LEVEL_VAL_SRC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->stat_tag_lsb_position, SOC_PB_PMF_LOW_LEVEL_STAT_TAG_LSB_POSITION_MAX, SOC_PB_PMF_LOW_LEVEL_STAT_TAG_LSB_POSITION_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_TAG_SRC_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_FEM_NDX_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->id, SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX, SOC_PB_PMF_LOW_LEVEL_FEM_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cycle_ndx, SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_FEM_NDX_verify()",0,0);
}

uint32
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->sel_bit_msb, SOC_PB_PMF_LOW_LEVEL_SEL_BIT_MSB_MIN, SOC_PB_PMF_LOW_LEVEL_SEL_BIT_MSB_MAX, SOC_PB_PMF_LOW_LEVEL_SEL_BIT_MSB_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_FEM_SELECTED_BITS_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_fomat_id, SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_ID_MAX, SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->map_data, SOC_PB_PMF_LOW_LEVEL_MAP_DATA_MAX, SOC_PB_PMF_LOW_LEVEL_MAP_DATA_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_FEM_BIT_LOC_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_BIT_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PMF_LOW_LEVEL_TYPE_MAX, SOC_PB_PMF_LOW_LEVEL_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  switch(info->type)
  {
  case SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val, TRUE, SOC_PB_PMF_LOW_LEVEL_BIT_LOC_VAL_OUT_OF_RANGE_ERR, 11, exit);
    break;
  
  case SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val, SOC_PB_PMF_LOW_LEVEL_SEL_BIT_MSB_MAX, SOC_PB_PMF_LOW_LEVEL_BIT_LOC_VAL_OUT_OF_RANGE_ERR, 12, exit);
    break;

  case SOC_PB_PMF_FEM_BIT_LOC_TYPE_MAP_DATA:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val, SOC_PB_PMF_FEM_SEL_BITS_SIZE_IN_BITS-1, SOC_PB_PMF_LOW_LEVEL_BIT_LOC_VAL_OUT_OF_RANGE_ERR, 13, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_FEM_BIT_LOC_verify()",0,0);
}

uint32
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PMF_LOW_LEVEL_AF_TYPE_MAX, SOC_PB_PMF_LOW_LEVEL_AF_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->size, SOC_PB_PMF_LOW_LEVEL_FEM_SIZE_MAX, SOC_PB_PMF_LOW_LEVEL_SIZE_OUT_OF_RANGE_ERR, 11, exit);
  for (ind = 0; ind < info->size; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_FEM_BIT_LOC, &(info->bit_loc[ind]), 12, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->base_value, SOC_PB_PMF_LOW_LEVEL_BASE_VALUE_MAX, SOC_PB_PMF_LOW_LEVEL_BASE_VALUE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_verify()",0,0);
}

void
  SOC_PB_PMF_FEM_INPUT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_INPUT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_INPUT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TAG_SRC_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_TAG_SRC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TAG_SRC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_NDX_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_NDX_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_SELECTED_BITS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_BIT_LOC_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_BIT_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_BIT_LOC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_FEM_ACTION_FORMAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PMF_FEM_INPUT_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_SRC enum_val
  )
{
  return SOC_TMC_PMF_FEM_INPUT_SRC_to_string(enum_val);
}

const char*
  SOC_PB_PMF_TAG_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TAG_TYPE enum_val
  )
{
  return SOC_TMC_PMF_TAG_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PMF_TAG_VAL_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TAG_VAL_SRC enum_val
  )
{
  return SOC_TMC_PMF_TAG_VAL_SRC_to_string(enum_val);
}



const char*
  SOC_PB_PMF_FEM_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_TYPE enum_val
  )
{
  return SOC_TMC_PMF_FEM_ACTION_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PMF_FEM_BIT_LOC_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FEM_BIT_LOC_TYPE enum_val
  )
{
  return SOC_TMC_PMF_FEM_BIT_LOC_TYPE_to_string(enum_val);
}

void
  SOC_PB_PMF_FEM_INPUT_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_INPUT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_INPUT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TAG_SRC_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_TAG_SRC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TAG_SRC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_PMF_FEM_NDX_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_NDX_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_SELECTED_BITS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_BIT_LOC_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_BIT_LOC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_BIT_LOC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

