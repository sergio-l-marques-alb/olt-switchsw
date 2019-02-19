/* $Id: pb_pmf_low_level.c,v 1.6 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_TM/pb_action_cmd.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_ce.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/petra_api_ports.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PMF_ITMH_SNOOP_TRAP_BASE_VAL (0xD0)

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
  soc_pb_pmf_low_level_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_INIT_UNSAFE);

  regs = soc_pb_pp_regs();

  res = soc_pb_pmf_low_level_ce_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pmf_low_level_db_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pmf_low_level_fem_tag_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pmf_low_level_pgm_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pmf_pgm_mgmt_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_pb_pmf_low_level_tm_init_set_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   *	Set the PMF strength to 6
   */
  fld_val = 6;
  SOC_PB_PP_FLD_SET(regs->ihb.pmfgeneral_configuration1_reg.pmf_strength, fld_val, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_init_unsafe()", 0, 0);
}

/* $Id: pb_pmf_low_level.c,v 1.6 Broadcom SDK $
 * TM PMF Program: one function for the static configuration,
 * the other per PMF Program
 */
uint32
  soc_pb_pmf_low_level_tm_init_set_unsafe(
    SOC_SAND_IN  int                unit
  )
{
  uint32
    action_fomat_id = 0,
    four_b_msb = 0,
    res = SOC_SAND_OK;
  uint32
    snoop_cmd_ndx,
    bit_ndx,
    selected_bits_ndx,
    action_format_ndx,
    fem_ndx,
    cycle_ndx,
    tag_profile_ndx;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_DIRECT_TBL_KEY_SRC
    dt_key_src;
  SOC_PB_PMF_TAG_SRC_INFO
    tag_src;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input;
  SOC_PB_PMF_FEM_INPUT_SRC
    fem_key_src;
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO
    selected_4_msb;
  SOC_PB_PMF_FEM_NDX
    fem;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    action_format_info;
  uint8
    is_vsq_nif_enabled,
    is_action_format_set;
  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_TM_INIT_SET_UNSAFE);

  /*
   *	1. Set the Direct table Key source
   */
  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  lkp_profile_ndx.id = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
  is_vsq_nif_enabled = soc_pb_sw_db_is_vsq_nif_enabled_get(unit);

  dt_key_src = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_B_9_0; /* No use in the first cycle */
  for (cycle_ndx = 0; cycle_ndx <= SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX; ++cycle_ndx)
  {
    lkp_profile_ndx.cycle_ndx = cycle_ndx;
    res = soc_pb_pmf_db_direct_tbl_key_src_set_unsafe(
            unit,
            &lkp_profile_ndx,
            dt_key_src
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /*
   *	2. Build the Tags
   */
  tag_profile_ndx = SOC_PB_PMF_PGM_TAG_PROFILE_NDX_FOR_TM;

  SOC_PB_PMF_TAG_SRC_INFO_clear(&tag_src);
  /* Always take the S-TAG from the same position, but fill it only if enabled */
  tag_src.val_src = SOC_PB_PMF_TAG_VAL_SRC_A_61_32;
  tag_src.stat_tag_lsb_position = 0;
  res = soc_pb_pmf_db_tag_select_set_unsafe(
          unit,
          tag_profile_ndx,
          SOC_PB_PMF_TAG_TYPE_STAT_TAG,
          &tag_src
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	3. Build the FEMs
   */
  SOC_PB_PMF_FEM_NDX_clear(&fem);
  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  lkp_profile_ndx.id = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
  SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input);
  fem_input.db_id = SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES; /* No TCAM use */
  fem_input.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_TM;
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(&selected_4_msb);
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map);
  for (cycle_ndx = 0; cycle_ndx <= SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX; ++cycle_ndx)
  {
    lkp_profile_ndx.cycle_ndx = cycle_ndx;
    for (fem_ndx = 0; fem_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_ndx)
    {
      /*
       *	Set the Key source
       */
      if (cycle_ndx == 0)
      {
        if (fem_ndx == 7)
        {
          fem_key_src = SOC_PB_PMF_FEM_INPUT_SRC_A_63_32;
        }
        else if ((fem_ndx == 1) || (fem_ndx == 5) || (fem_ndx == 6))
        {
          fem_key_src = SOC_PB_PMF_FEM_INPUT_SRC_A_31_0;
        }
        else
        {
          fem_key_src = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
        }
      }
      else
      {
        if ((fem_ndx < 3) || (fem_ndx == 7))
        {
          fem_key_src = SOC_PB_PMF_FEM_INPUT_SRC_A_31_0;
        }
        else if (fem_ndx == 4)
        {
          if (is_vsq_nif_enabled == TRUE)
          {
            fem_key_src = SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL;
          }
          else
          {
            fem_key_src = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
          }
        }
        else if (fem_ndx == 6)
        {
          fem_key_src = SOC_PB_PMF_FEM_INPUT_SRC_A_95_64;
        }
        else
        {
          fem_key_src = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
        }
      }
      fem_input.src = fem_key_src;


      res = soc_pb_pmf_db_fem_input_set_unsafe(
              unit,
              &lkp_profile_ndx,
              fem_ndx,
              &fem_input
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /*
       *	Change the default 4bits-mapping if necessary
       */
      four_b_msb = 31;
      if (cycle_ndx == 1)
      {
        if (fem_ndx == 2)
        {
          four_b_msb = 27;
        }
        else if (fem_ndx == 6)
        {
          four_b_msb = 19;
        }
        else if (fem_ndx == 7)
        {
          four_b_msb = 19;
        }
      }
      selected_4_msb.sel_bit_msb = four_b_msb;
      fem.cycle_ndx = cycle_ndx;
      fem.id = fem_ndx;

      res = soc_pb_pmf_fem_select_bits_set_unsafe(
              unit,
              &fem,
              SOC_PB_PMF_FEM_PGM_FOR_TM,
              &selected_4_msb
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      /*
       *	Change the Action format map mapping when necessary
       */
      for (selected_bits_ndx = 0; selected_bits_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
      {
        if (cycle_ndx == 0)
        {
           action_fomat_id = SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT;
        }
        else
        {
          if (fem_ndx == 7)
          {
            if (selected_bits_ndx % 4 == 1)
            {
              /* Reserved - NOP */
              action_fomat_id = SOC_PB_PMF_FEM_ACTION_DEFAULT_NOP_3;
            }
            else if (selected_bits_ndx % 4 == 3)
            {
              /* Flow */
              action_fomat_id = SOC_PB_PMF_FEM_ACTION_DEFAULT_DEST_1;
            }
            else
            {
              /* UC and MC */
              action_fomat_id = SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT;
            }
          }
          else if (fem_ndx == 0)
          {
            action_fomat_id = selected_bits_ndx % 2;
          }
          else if (fem_ndx == 2)
          {
            action_fomat_id = (selected_bits_ndx != 0) ? 1 : 0;
          }
          else if (fem_ndx == 6)
          {
            action_fomat_id = SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM_IS_ON(selected_bits_ndx) ? 1 : 0;
          }
          else
          {
            action_fomat_id = SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT;
          }
        }
        fem_action_format_map.action_fomat_id = action_fomat_id;
        res = soc_pb_pmf_fem_action_format_map_set_unsafe(
                unit,
                &fem,
                SOC_PB_PMF_FEM_PGM_FOR_TM,
                selected_bits_ndx,
                &fem_action_format_map
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }

      /*
       *	Set the Action formats
       */

      for (action_format_ndx = 0; action_format_ndx <= SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_MAX; ++action_format_ndx)
      {
        is_action_format_set = FALSE;
        SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(&action_format_info);
        action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
        action_format_info.size = 0;
        if (cycle_ndx == 0)
        {
          if ((fem_ndx == 1) && (action_format_ndx == SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT))
          {
            /*
             *	Exclude-Source with bits 26:23
             */
            is_action_format_set = TRUE;
            action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_EXC_SRC;
            action_format_info.size = 1;
            for (bit_ndx = 0; bit_ndx < action_format_info.size; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[bit_ndx].val = 23 + bit_ndx;
            }
          }
          else if ((fem_ndx == 5) && (action_format_ndx == SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT))
          {
            /*
             *	Traffic-Class with {1, bits 22:20}
             */
            is_action_format_set = TRUE;
            action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_TC;
            action_format_info.size = 4;
            for (bit_ndx = 0; bit_ndx < 3; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[bit_ndx].val = 20 + bit_ndx;
            }
            action_format_info.bit_loc[3].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
            action_format_info.bit_loc[3].val = 1;
          }
          else if ((fem_ndx == 6) && (action_format_ndx == SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT))
          {
            /*
             *	Drop-Precedence with {1, 1, bits 19:18}
             */
            is_action_format_set = TRUE;
            action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_DP;
            action_format_info.size = 4;
            for (bit_ndx = 0; bit_ndx < 2; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[bit_ndx].val = 18 + bit_ndx;
            }
            for (bit_ndx = 2; bit_ndx < action_format_info.size; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
              action_format_info.bit_loc[bit_ndx].val = 1;
            }
          }
          else if ((fem_ndx == 7) && (action_format_ndx == SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT))
          {
            /*
             *	VSQ-Pointer from the Statistic-TAG with {bits 25:18}
             */
            is_action_format_set = TRUE;
            action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_STAT;
            action_format_info.size = 14;
            for (bit_ndx = 0; bit_ndx < 8; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[bit_ndx].val = bit_ndx + 18;
            }
            for (bit_ndx = 8; bit_ndx < 13; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
              action_format_info.bit_loc[bit_ndx].val = 0;
            }
            action_format_info.bit_loc[13].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
            action_format_info.bit_loc[13].val = 1;
          }
        }
        else
        {
          if ((fem_ndx == 0) && (action_format_ndx < 2))
          {
            /*
             *	0 - NOP, 1 - Mirror {0,0,0,0}
             */
            is_action_format_set = TRUE;
            if (action_format_ndx == 1)
            {
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_MIRROR;
              action_format_info.size = 4;
              for (bit_ndx = 0; bit_ndx < action_format_info.size; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
                action_format_info.bit_loc[bit_ndx].val = 0;
              }
            }
            else if (action_format_ndx == 0)
            {
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
              action_format_info.size = 0;
            }
          }
          else if ((fem_ndx == 1) && (action_format_ndx == SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT))
          {
            /*
             *	Out-Mirror disable with {bit 29}
             */
            is_action_format_set = TRUE;
            action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_MIR_DIS;
            action_format_info.size = 1;
            action_format_info.bit_loc[0].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
            action_format_info.bit_loc[0].val = 29;
          }
          else if ((fem_ndx == 2) && (action_format_ndx < 2))
          {
            /*
             *	0 - NOP, 1 - Snoop: {1,1,SOC_PB_PMF_ITMH_SNOOP_TRAP_BASE_VAL = 0xd, Bits[27:24]}
             */
            is_action_format_set = TRUE;
            if (action_format_ndx == 1)
            {
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_SNP;
              action_format_info.size = 10;
              for (bit_ndx = 0; bit_ndx < 4; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
                action_format_info.bit_loc[bit_ndx].val = 24 + bit_ndx;
              }
              for (bit_ndx = 4; bit_ndx < 8; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
                action_format_info.bit_loc[bit_ndx].val = SOC_SAND_GET_BIT(SOC_PB_PMF_ITMH_SNOOP_TRAP_BASE_VAL, bit_ndx);
              }
              for (bit_ndx = 8; bit_ndx < action_format_info.size; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
                action_format_info.bit_loc[bit_ndx].val = 1;
              }
            }
            else if (action_format_ndx == 0)
            {
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
              action_format_info.size = 0;
            }
          }
          else if ((fem_ndx == 4) && (action_format_ndx == SOC_PB_PMF_FEM_TM_ACTION_FORMAT_NDX_DEFAULT))
          {
            /*
             *	Statistic-Tag from Direct-Table (7:0)
             */
            is_action_format_set = TRUE;
            action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_STAT;
            action_format_info.size = 14;
            for (bit_ndx = 0; bit_ndx < 8; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[bit_ndx].val = bit_ndx;
            }
            for (bit_ndx = 8; bit_ndx < 13; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
              action_format_info.bit_loc[bit_ndx].val = 0;
            }
            action_format_info.bit_loc[13].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
            action_format_info.bit_loc[13].val = 1;
          }
          else if (fem_ndx == 6)
          {
            /* Get for the Base flow */
            res = soc_pb_pmf_fem_action_format_get_unsafe(
                    unit,
                    &fem,
                    action_format_ndx,
                    &action_format_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 187, exit);

            if (action_format_ndx == 0)
            {
              is_action_format_set = TRUE;
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
              action_format_info.size = 0;
            }
            else if (action_format_ndx == 1)
            {
              /*
               *	Ingress-Shaping with {bits 15:0}
               */
              is_action_format_set = TRUE;
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_IS;
              action_format_info.size = 16;
              for (bit_ndx = 0; bit_ndx < action_format_info.size; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
                action_format_info.bit_loc[bit_ndx].val = bit_ndx;
              }
            }
          }
          else if (fem_ndx == 7)
          {
            /*
             *	Destination: 0 - {0,0,17,13:0}, 1 - {1,1,14:0}, 3 - NOP
             */

            /* Get for the Base flow */
            res = soc_pb_pmf_fem_action_format_get_unsafe(
                    unit,
                    &fem,
                    action_format_ndx,
                    &action_format_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 188, exit);

            if (action_format_ndx == 3)
            {
              is_action_format_set = TRUE;
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
              action_format_info.size = 0;
            }
            else if (action_format_ndx == 0)   /* Unicast amd MC Destination */
            {
              is_action_format_set = TRUE;
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_DEST;
              action_format_info.size = 17;
              for (bit_ndx = 0; bit_ndx < 14; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
                action_format_info.bit_loc[bit_ndx].val = bit_ndx;
              }
              action_format_info.bit_loc[14].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[14].val = 17;
              for (bit_ndx = 15; bit_ndx < action_format_info.size; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
                action_format_info.bit_loc[bit_ndx].val = 0;
              }
            }
            else if (action_format_ndx == 1)   /* Flow */
            {
              is_action_format_set = TRUE;
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_DEST;
              action_format_info.size = 17;
              for (bit_ndx = 0; bit_ndx < 15; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
                action_format_info.bit_loc[bit_ndx].val = bit_ndx;
              }
              for (bit_ndx = 15; bit_ndx < action_format_info.size; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
                action_format_info.bit_loc[bit_ndx].val = 1;
              }
            }
          }
        }
        if (is_action_format_set)
        {
          res = soc_pb_pmf_fem_action_format_set_unsafe(
                  unit,
                  &fem,
                  action_format_ndx,
                  &action_format_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
        }
      }
    }
  }

  /*
   * Set the default Snoop mapping: from D0 to DF in the trap user-defined (timna)
   * The mapping is one to one
   */
  for (snoop_cmd_ndx = 0; snoop_cmd_ndx <= SOC_PB_ACTION_NDX_MAX; snoop_cmd_ndx ++)
  {
    tbl_data.snoop_action = snoop_cmd_ndx;

    res = soc_pb_pp_ihb_snoop_action_tbl_set_unsafe(
            unit,
            SOC_PB_PMF_ITMH_SNOOP_TRAP_BASE_VAL + snoop_cmd_ndx,
            &tbl_data
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_tm_init_set_unsafe()", 0, 0);
}



uint32
  soc_pb_pmf_low_level_tm_pgm_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *pp_port_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    offset4b,
    res = SOC_SAND_OK;
  uint32
    ce_ndx,
    pmf_key;
  uint8
    is_ingress_shaped,
    is_pph_present;
  SOC_PB_PMF_CE_PACKET_HEADER_INFO
    pkt_hdr_info;
  SOC_PB_PMF_PGM_INFO
    pgm_info;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_TM_PGM_SET_UNSAFE);

  *success = SOC_SAND_SUCCESS;

  is_ingress_shaped = pp_port_info->is_tm_ing_shaping_enabled;
  is_pph_present = pp_port_info->is_tm_pph_present_enabled;

  /*
   * Call the TM init for the VSQ FEM / DT source update
   * The Base flow should not be influenced
   */
  res = soc_pb_pmf_low_level_tm_init_set_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	1. Copy Engine Key construction
   */
  SOC_PB_PMF_CE_PACKET_HEADER_INFO_clear(&pkt_hdr_info);

  /*
   *	Key A:
   *  Instruction 0: Unused
   *  Instruction 1: Unused
   *  Instruction 2: Unused
   *  Instruction 3: Unused
   *  Instruction 4: Parser-PMF-Profile
   *  Instruction 5: Ingress shaping header / 16'b0
   *  Instruction 6: Statistics tag / 32'b0
   *  Instruction 7: ITMH
   */
  pmf_key = 0;
  for (ce_ndx = 0; ce_ndx < 4; ++ce_ndx)
  {
    res = soc_pb_pmf_ce_nop_entry_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pmf_key,
            ce_ndx,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /*
   *  Copy the Parser-PMF-Profile
   */
  res = soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          4,
          0,
          SOC_PB_PMF_IRPP_INFO_FIELD_PARSER_PMF_PROFILE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  if (is_ingress_shaped)
  {
    /*
     *  Copy ingress shaping destination (16 bits)
     */
    pkt_hdr_info.sub_header = SOC_PB_PMF_CE_SUB_HEADER_FWD;
    pkt_hdr_info.nof_bits = 16;
    pkt_hdr_info.offset = -16;

    res = soc_pb_pmf_ce_packet_header_entry_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pmf_key,
            5,
            &pkt_hdr_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    /*
     *  Fill with 16'b0
     */
    res = soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pmf_key,
            5,
            0,
            SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_16
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  /*
   *  Copy statistics tag
   *  expected format {VSQ-Ptr (8b, optional), S-TAG (18b)}
   */
  if (pp_port_info->is_stag_enabled == TRUE)
  {
    pkt_hdr_info.sub_header = SOC_PB_PMF_CE_SUB_HEADER_0;
    pkt_hdr_info.nof_bits = 32;
    /*
     * Get the offset from the API
     */
    offset4b = soc_pb_sw_db_stag_offset_nibble_get(unit);
    pkt_hdr_info.offset = (4 * offset4b) - pkt_hdr_info.nof_bits;
    res = soc_pb_pmf_ce_packet_header_entry_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pmf_key,
            6,
            &pkt_hdr_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else
  {
    /*
     *  Fill with 32'b0
     */
    res = soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pmf_key,
            6,
            0,
            SOC_PB_PMF_IRPP_INFO_FIELD_ZERO_32
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  }

  /*
   *  Copy ITMH
   */
  pkt_hdr_info.sub_header = (is_ingress_shaped == FALSE)? SOC_PB_PMF_CE_SUB_HEADER_1 : SOC_PB_PMF_CE_SUB_HEADER_FWD;
  pkt_hdr_info.nof_bits = 32;
  pkt_hdr_info.offset = 0;

  res = soc_pb_pmf_ce_packet_header_entry_set_unsafe(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          7,
          &pkt_hdr_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   *	Key B:
   *  Instruction 0 - 7: Unused
   *  Instruction 3: In-TM-Port
   */
  pmf_key = 1;
  for (ce_ndx = 0; ce_ndx <= 7; ++ce_ndx)
  {
    res = soc_pb_pmf_ce_nop_entry_set_unsafe(
            unit,
            pmf_pgm_ndx,
            pmf_key,
            ce_ndx,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

  /*
   *  3: Copy In-TM-Port
   */
  res = soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
          unit,
          pmf_pgm_ndx,
          pmf_key,
          3,
          0,
          SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  /*
   *	2. Program properties
   */
  SOC_PB_PMF_PGM_INFO_clear(&pgm_info);
  res = soc_pb_pmf_pgm_get_unsafe(
          unit,
          pmf_pgm_ndx,
          &pgm_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  pgm_info.lkp_profile_id[0] = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
  pgm_info.lkp_profile_id[1] = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM;
  pgm_info.tag_profile_id = SOC_PB_PMF_PGM_TAG_PROFILE_NDX_FOR_TM;
  pgm_info.bytes_to_rmv.header_type = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START;
  pgm_info.bytes_to_rmv.nof_bytes = 4; /* The ITMH */
  if (pp_port_info->first_header_size != 0)
  {
    pgm_info.bytes_to_rmv.nof_bytes += pp_port_info->first_header_size; /* The Sequence number for Fat Pipe for example*/
  }
  if (pp_port_info->is_tm_ing_shaping_enabled)
  {
    pgm_info.bytes_to_rmv.nof_bytes += 4; /* The IS-ITMH */
  }
  if (pp_port_info->is_tm_src_syst_port_ext_present)
  {
    pgm_info.bytes_to_rmv.nof_bytes += 2; /* The Source-System-Port Extension */
  }
  if (is_pph_present == FALSE)
  {
    pgm_info.header_type = SOC_PETRA_PORT_HEADER_TYPE_TM;
  }
  else
  {
    pgm_info.header_type = SOC_PETRA_PORT_HEADER_TYPE_PROG;
  }
  pgm_info.copy_pgm_var = 0;
  pgm_info.fc_type = pp_port_info->fc_type;

  res = soc_pb_pmf_pgm_set_unsafe(
          unit,
          pmf_pgm_ndx,
          &pgm_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_tm_pgm_set_unsafe()", 0, 0);
}


/*
 * Stacking / TDM PMF Program: one function for the static configuration,
 * the other per PMF Program
 */

uint32
  soc_pb_pmf_low_level_stack_init_set_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    action_fomat_id = 0,
    four_b_msb = 0,
    res = SOC_SAND_OK;
  uint32
    bit_ndx,
    selected_bits_ndx,
    action_format_ndx,
    fem_ndx,
    cycle_ndx,
    tag_profile_ndx;
  uint8
    is_fem_modified;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_TAG_SRC_INFO
    tag_src;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input;
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO
    selected_4_msb;
  SOC_PB_PMF_FEM_NDX
    fem;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map;
  SOC_PB_PMF_FEM_ACTION_FORMAT_INFO
    action_format_info;

 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_STACK_INIT_SET_UNSAFE);

  /*
   *	1. Tags built (TM)
   */
  tag_profile_ndx = SOC_PB_PMF_PGM_TAG_PROFILE_NDX_FOR_STACKING;

  SOC_PB_PMF_TAG_SRC_INFO_clear(&tag_src);
  tag_src.val_src = SOC_PB_PMF_TAG_VAL_SRC_B_29_0;
  res = soc_pb_pmf_db_tag_select_set_unsafe(
          unit,
          tag_profile_ndx,
          SOC_PB_PMF_TAG_TYPE_STACK_RT_HIST,
          &tag_src
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);


  /*
   *	2. Build the FEMs
   */
  SOC_PB_PMF_FEM_NDX_clear(&fem);
  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  lkp_profile_ndx.id = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_STACKING;
  SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input);
  fem_input.db_id = SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES; /* No TCAM use */
  SOC_PB_PMF_FEM_SELECTED_BITS_INFO_clear(&selected_4_msb);
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map);
  for (cycle_ndx = 0; cycle_ndx <= SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX; ++cycle_ndx)
  {
    lkp_profile_ndx.cycle_ndx = cycle_ndx;
    for (fem_ndx = 0; fem_ndx <= SOC_PB_PMF_LOW_LEVEL_FEM_ID_MAX; ++fem_ndx)
    {
      /*
       *	Set the Key source
       */
      if (cycle_ndx == 0)
      {
        if ((fem_ndx == 5) || (fem_ndx == 6))
        {
          fem_input.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_STACK;
          fem_input.src = SOC_PB_PMF_FEM_INPUT_SRC_A_31_0;
        }
        else
        {
          fem_input.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_RAW;
          fem_input.src = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
        }
      }
      else
      {
        if (fem_ndx == 5)
        {
          fem_input.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_STACK;
          fem_input.src = SOC_PB_PMF_FEM_INPUT_SRC_A_31_0;
        }
        else
        {
          fem_input.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_RAW;
          fem_input.src = SOC_PB_PMF_FEM_INPUT_SRC_NOP;
        }
      }

      res = soc_pb_pmf_db_fem_input_set_unsafe(
              unit,
              &lkp_profile_ndx,
              fem_ndx,
              &fem_input
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

      /*
       *	Change the default 4bits-mapping if necessary
       */
      if (cycle_ndx == 0)
      {
        four_b_msb = 31;
      }
      else
      {
        if (fem_ndx == 5)
        {
          four_b_msb = 16;
        }
        else
        {
          four_b_msb = 31;
        }
      }
      selected_4_msb.sel_bit_msb = four_b_msb;
      fem.cycle_ndx = cycle_ndx;
      fem.id = fem_ndx;

      res = soc_pb_pmf_fem_select_bits_set_unsafe(
              unit,
              &fem,
              SOC_PB_PMF_FEM_PGM_FOR_STACK,
              &selected_4_msb
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

      /*
       *	Change the Action format map mapping when necessary
       */
      for (selected_bits_ndx = 0; selected_bits_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
      {
        /*
         *	Do as Ethernet (nothing), except the DP, TC and Destination
         */
        if ((cycle_ndx == 0) && ((fem_ndx == 5) || (fem_ndx == 6)))
        {
          action_fomat_id = SOC_PB_PMF_FEM_FTMH_ACTION_FORMAT_NDX;
        }
        else  if ((cycle_ndx == 1) && (fem_ndx == 5))
        {
          if (selected_bits_ndx >= 8)
          {
            /* Multicast */
            action_fomat_id = 0;
          }
          else
          {
            /* 0: Unicast, 1: Flow */
            action_fomat_id = selected_bits_ndx >> 2;
          }
        }
        else
        {
          action_fomat_id = SOC_PB_PMF_FEM_ACTION_DEFAULT_NOP_3;
        }

        fem_action_format_map.action_fomat_id = action_fomat_id;
        res = soc_pb_pmf_fem_action_format_map_set_unsafe(
                unit,
                &fem,
                SOC_PB_PMF_FEM_PGM_FOR_STACK,
                selected_bits_ndx,
                &fem_action_format_map
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
      }

      /*
       *	Set the Action formats
       */
      for (action_format_ndx = 0; action_format_ndx <= SOC_PB_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_MAX; ++action_format_ndx)
      {
        is_fem_modified = FALSE;
        SOC_PB_PMF_FEM_ACTION_FORMAT_INFO_clear(&action_format_info);
        action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
        action_format_info.size = 0;
        if (cycle_ndx == 0)
        {
          if ((fem_ndx == 5) && (action_format_ndx == SOC_PB_PMF_FEM_FTMH_ACTION_FORMAT_NDX))
          {
            is_fem_modified = TRUE;
            /*
             *	Traffic-Class with {1, bits 24:22}
             */
            action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_TC;
            action_format_info.size = 4;
            for (bit_ndx = 0; bit_ndx < 3; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[bit_ndx].val = 22 + bit_ndx;
            }
            action_format_info.bit_loc[3].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
            action_format_info.bit_loc[3].val = 1;
          }
          else if ((fem_ndx == 6) && (action_format_ndx == SOC_PB_PMF_FEM_FTMH_ACTION_FORMAT_NDX))
          {
            is_fem_modified = TRUE;
            /*
             *	Drop-Precedence with {1, 1, bits 20:19}
             */
            action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_DP;
            action_format_info.size = 4;
            for (bit_ndx = 0; bit_ndx < 2; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[bit_ndx].val = 19 + bit_ndx;
            }
            for (bit_ndx = 2; bit_ndx < action_format_info.size; ++bit_ndx)
            {
              action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
              action_format_info.bit_loc[bit_ndx].val = 1;
            }
          }
        }
        else
        {
          if (fem_ndx == 5)
          {
            /*
             *	Destination: 0 - {0,0,BIT(16),13:0}, 1 - {1,1,14:0}, 3 - NOP
             *  Assumption that the reserved (written set to '0') are really 0
             */
            if (action_format_ndx == 3)
            {
              is_fem_modified = TRUE;
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
              action_format_info.size = 0;
            }
            else if (action_format_ndx == 0)   /* Unicast and MC Destination */
            {
              is_fem_modified = TRUE;
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_DEST;
              action_format_info.size = 17;
              for (bit_ndx = 0; bit_ndx < 14; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
                action_format_info.bit_loc[bit_ndx].val = bit_ndx;
              }
              action_format_info.bit_loc[14].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
              action_format_info.bit_loc[14].val = 16;
              for (bit_ndx = 15; bit_ndx < action_format_info.size; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
                action_format_info.bit_loc[bit_ndx].val = 0;
              }
            }
            else if (action_format_ndx == 1)   /* Flow */
            {
              is_fem_modified = TRUE;
              action_format_info.type = SOC_PB_PMF_FEM_ACTION_TYPE_DEST;
              action_format_info.size = 17;
              for (bit_ndx = 0; bit_ndx < 15; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_KEY;
                action_format_info.bit_loc[bit_ndx].val = bit_ndx;
              }
              for (bit_ndx = 15; bit_ndx < action_format_info.size; ++bit_ndx)
              {
                action_format_info.bit_loc[bit_ndx].type = SOC_PB_PMF_FEM_BIT_LOC_TYPE_CST;
                action_format_info.bit_loc[bit_ndx].val = 1;
              }
            }
          }
        }
        if (is_fem_modified == TRUE)
        {
          res = soc_pb_pmf_fem_action_format_set_unsafe(
                  unit,
                  &fem,
                  action_format_ndx,
                  &action_format_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_stack_init_set_unsafe()", 0, 0);
}


uint32
  soc_pb_pmf_low_level_stack_pgm_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *pp_port_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ce_ndx,
    pmf_key;
  SOC_PB_PMF_CE_PACKET_HEADER_INFO
    pkt_hdr_info;
  SOC_PB_PMF_PGM_INFO
    pgm_info;

 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_STACK_PGM_SET_UNSAFE);

  *success = SOC_SAND_SUCCESS;

  /*
   *	2. Copy Engine Key construction
   */
  SOC_PB_PMF_CE_PACKET_HEADER_INFO_clear(&pkt_hdr_info);

  /*
   *	Key A: 3-7 Invalid, 0 FTMH. TC, 1 for FTMH. DP,
   *  2 FTMH. Destination
   */
  pmf_key = 0;
  for (ce_ndx = 0; ce_ndx <= SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX; ++ce_ndx)
  {
    if (ce_ndx > 2)
    {
      res = soc_pb_pmf_ce_nop_entry_set_unsafe(
              unit,
              pmf_pgm_ndx,
              pmf_key,
              ce_ndx,
              TRUE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
    else if (ce_ndx == 0)
    {
      /*
       *	Get the FTMH. TC: bits 31:28
       */
      pkt_hdr_info.sub_header = SOC_PB_PMF_CE_SUB_HEADER_0;
      pkt_hdr_info.nof_bits = 4;
      pkt_hdr_info.offset = 16;
      
      res = soc_pb_pmf_ce_packet_header_entry_set_unsafe(
              unit,
              pmf_pgm_ndx,
              pmf_key,
              ce_ndx,
              &pkt_hdr_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    else if (ce_ndx == 1)
    {
      /*
       *	Get the FTMH. DP: bits 7:4
       */
      pkt_hdr_info.sub_header = SOC_PB_PMF_CE_SUB_HEADER_0;
      pkt_hdr_info.nof_bits = 4;
      pkt_hdr_info.offset = 40;
      
      res = soc_pb_pmf_ce_packet_header_entry_set_unsafe(
              unit,
              pmf_pgm_ndx,
              pmf_key,
              ce_ndx,
              &pkt_hdr_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    else if (ce_ndx == 2)
    {
      /*
       *	Get the FTMH-Ext. Destination: bits 16:0:
       *  FTMH. Is-MC(1b) + FTMH-Destination-Extension (16b)
       */
      pkt_hdr_info.sub_header = SOC_PB_PMF_CE_SUB_HEADER_0;
      pkt_hdr_info.nof_bits = 17;
      pkt_hdr_info.offset = 47;
      
      res = soc_pb_pmf_ce_packet_header_entry_set_unsafe(
              unit,
              pmf_pgm_ndx,
              pmf_key,
              ce_ndx,
              &pkt_hdr_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }

  /*
   *	Key B: 0-6 Invalid, 7 History Map
   */
  pmf_key = 1;
  for (ce_ndx = 0; ce_ndx <= SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX; ++ce_ndx)
  {
    if (ce_ndx == SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX)
    {
      /*
       *	Get the FTMH-LB-Key-Ext. HistoryMap: bits 15:0
       */
      pkt_hdr_info.sub_header = SOC_PB_PMF_CE_SUB_HEADER_0;
      pkt_hdr_info.nof_bits = 16;
      pkt_hdr_info.offset = 64;
      
      res = soc_pb_pmf_ce_packet_header_entry_set_unsafe(
              unit,
              pmf_pgm_ndx,
              pmf_key,
              ce_ndx,
              &pkt_hdr_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    else
    {
      res = soc_pb_pmf_ce_nop_entry_set_unsafe(
              unit,
              pmf_pgm_ndx,
              pmf_key,
              ce_ndx,
              TRUE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }

  /*
   *	3. Program properties
   */
  SOC_PB_PMF_PGM_INFO_clear(&pgm_info);
  res = soc_pb_pmf_pgm_get_unsafe(
          unit,
          pmf_pgm_ndx,
          &pgm_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  pgm_info.lkp_profile_id[0] = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_STACKING;
  pgm_info.lkp_profile_id[1] = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_STACKING;
  pgm_info.tag_profile_id = SOC_PB_PMF_PGM_TAG_PROFILE_NDX_FOR_STACKING;
  pgm_info.bytes_to_rmv.header_type = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START;
  pgm_info.bytes_to_rmv.nof_bytes = 0; /* No ITMH to remove, FTMH remains */
  if (pp_port_info->first_header_size)
  {
    pgm_info.bytes_to_rmv.nof_bytes += pp_port_info->first_header_size; /* The Sequence number for Fat Pipe for example */
  }
  pgm_info.header_type = SOC_PETRA_PORT_HEADER_TYPE_STACKING;
  pgm_info.copy_pgm_var = 0;
  pgm_info.fc_type = pp_port_info->fc_type;

  res = soc_pb_pmf_pgm_set_unsafe(
          unit,
          pmf_pgm_ndx,
          &pgm_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_stack_pgm_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_low_level_raw_pgm_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *pp_port_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ce_ndx,
    cycle_ndx,
    pmf_key;
  SOC_PB_PMF_PGM_INFO
    pgm_info;
  SOC_PB_PMF_DIRECT_TBL_KEY_SRC
    dt_key_src;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input;

 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_RAW_PGM_SET_UNSAFE);

  *success = SOC_SAND_SUCCESS;

  /*
   *	2. Copy Engine Key construction
   */
  /*
   *	All Keys: invalid
   */
  for (pmf_key = 0; pmf_key <= SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX; ++pmf_key)
  {
    for (ce_ndx = 0; ce_ndx <= SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX; ++ce_ndx)
    {
      res = soc_pb_pmf_ce_nop_entry_set_unsafe(
              unit,
              pmf_pgm_ndx,
              pmf_key,
              ce_ndx,
              TRUE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

  /* 
   * Key-A: CE7: In-TM-Port (7b)
   */
  res = soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
          unit,
          pmf_pgm_ndx,
          0,
          7,
          7,
          SOC_PB_PMF_IRPP_INFO_FIELD_TM_PORT
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  /* 
   * Key-A: CE6: '1' (1b) to prevent hitting low 
   * Direct table entries
   */
  res = soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
          unit,
          pmf_pgm_ndx,
          0,
          6,
          1,
          SOC_PB_PMF_IRPP_INFO_FIELD_ONES_32
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  /* 
   * Key-A: CE5: PFC[5:4] (2b)
   */
  res = soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
          unit,
          pmf_pgm_ndx,
          0,
          5,
          2,
          SOC_PB_PMF_IRPP_INFO_FIELD_PACKET_FORMAT_CODE_2MSB
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  /* 
   * Set the direct table to use the Key-A
   */
  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  lkp_profile_ndx.id = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW;
  dt_key_src = SOC_PB_PMF_DIRECT_TBL_KEY_SRC_A_9_0; /* No use in the first cycle */
  for (cycle_ndx = 0; cycle_ndx <= SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_MAX; ++cycle_ndx)
  {
    lkp_profile_ndx.cycle_ndx = cycle_ndx;
    res = soc_pb_pmf_db_direct_tbl_key_src_set_unsafe(
            unit,
            &lkp_profile_ndx,
            dt_key_src
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  }

  /* 
   * Set the FEM 7 Cycle 1 to use action of System-Port 
   */
  lkp_profile_ndx.cycle_ndx = 1;
  SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input);
  res = soc_pb_pmf_db_fem_input_get_unsafe(
          unit,
          &lkp_profile_ndx,
          7,
          &fem_input
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  /* 
   * Do the change only if not disabled by the FP 
   */
  if (fem_input.src != SOC_PB_PMF_FEM_INPUT_SRC_B_95_64)
  {
    fem_input.db_id = SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES; /* No TCAM use */
    fem_input.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_TM;
    fem_input.src = SOC_PB_PMF_FEM_INPUT_SRC_DIR_TBL;
    res = soc_pb_pmf_db_fem_input_set_unsafe(
            unit,
            &lkp_profile_ndx,
            7,
            &fem_input
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
  }

  /*
   *	3. Program properties
   */
  SOC_PB_PMF_PGM_INFO_clear(&pgm_info);
  res = soc_pb_pmf_pgm_get_unsafe(
          unit,
          pmf_pgm_ndx,
          &pgm_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  pgm_info.lkp_profile_id[0] = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW;
  pgm_info.lkp_profile_id[1] = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW;
  pgm_info.tag_profile_id = SOC_PB_PMF_PGM_TAG_PROFILE_NDX_FOR_RAW;
  pgm_info.bytes_to_rmv.header_type = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START;
  pgm_info.bytes_to_rmv.nof_bytes = 0; /* Nothing to remove */
  if (pp_port_info->first_header_size)
  {
    pgm_info.bytes_to_rmv.nof_bytes += pp_port_info->first_header_size; /* The Sequence number */
  }
  pgm_info.header_type = SOC_PETRA_PORT_HEADER_TYPE_RAW;
  pgm_info.copy_pgm_var = 0;
  pgm_info.fc_type = pp_port_info->fc_type;

  res = soc_pb_pmf_pgm_set_unsafe(
          unit,
          pmf_pgm_ndx,
          &pgm_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_raw_pgm_set_unsafe()", 0, 0);
}



uint32
  soc_pb_pmf_low_level_raw_pgm_dt_disable(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PMF_LKP_PROFILE
    lkp_profile_ndx;
  SOC_PB_PMF_FEM_INPUT_INFO
    fem_input;
  SOC_PB_PMF_FEM_NDX
    fem;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map;
  uint32
    selected_bits_ndx;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* 
   * Set the FEM 7 Cycle 1 NOT to use action of System-Port 
   */
  SOC_PB_PMF_LKP_PROFILE_clear(&lkp_profile_ndx);
  lkp_profile_ndx.id = SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_RAW;
  lkp_profile_ndx.cycle_ndx = 1;
  SOC_PB_PMF_FEM_INPUT_INFO_clear(&fem_input);
  fem_input.db_id = SOC_PB_PMF_LOW_LEVEL_NOF_DATABASES; /* No TCAM use */
  fem_input.pgm_id = SOC_PB_PMF_FEM_PGM_FOR_RAW;
  fem_input.src = SOC_PB_PMF_FEM_INPUT_SRC_B_95_64;
  res = soc_pb_pmf_db_fem_input_set_unsafe(
          unit,
          &lkp_profile_ndx,
          7,
          &fem_input
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* 
   * Map the Raw PMF-Program to look in the NOP action 
   */
  SOC_PB_PMF_FEM_NDX_clear(&fem);
  fem.cycle_ndx = 1;
  fem.id = 7;
  SOC_PB_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map);
  fem_action_format_map.action_fomat_id = SOC_PB_PMF_FEM_ACTION_DEFAULT_NOP_3;
  for (selected_bits_ndx = 0; selected_bits_ndx <= SOC_PB_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    res = soc_pb_pmf_fem_action_format_map_set_unsafe(
            unit,
            &fem,
            SOC_PB_PMF_FEM_PGM_FOR_RAW,
            selected_bits_ndx,
            &fem_action_format_map
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_raw_pgm_dt_disable()", 0, 0);
}


uint32
  soc_pb_pmf_low_level_eth_pgm_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 pfg_ndx,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *pp_port_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ce_ndx,
    pmf_key;
  SOC_PB_PMF_PGM_INFO
    pgm_info;
  SOC_PB_SW_DB_PMF_CE
    ce_instr;
  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    tbl_data;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_ETH_PGM_SET_UNSAFE);

  *success = SOC_SAND_SUCCESS;

  /*
   * Save this PMF-Program as Ethernet in SW DB
   */
  soc_pb_sw_db_is_pgm_eth_set(
    unit,
    pmf_pgm_ndx,
    TRUE
  );

  /*
   *	1. Copy Engine Key construction
   */
  /*
   *	Ethernet CE instructions according to the SW DB
   */
  for (pmf_key = 0; pmf_key <= SOC_PB_PMF_LOW_LEVEL_PMF_KEY_MAX; ++pmf_key)
  {
    for (ce_ndx = 0; ce_ndx <= SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX; ++ce_ndx)
    {
      soc_pb_sw_db_pgm_ce_instr_get(
        unit,
        pfg_ndx,
        pmf_key,
        ce_ndx,
        &ce_instr
      );

      tbl_data.inst_valid                = ce_instr.is_valid ? 1 : 0;
      tbl_data.inst_source_select        = ce_instr.is_irpp ? 1 : 0;
      tbl_data.inst_header_offset_select = ce_instr.header_offset_select;
      tbl_data.inst_niblle_field_offset  = ce_instr.nibble_field_offset;
      tbl_data.inst_bit_count            = ce_instr.bit_count;
      res = soc_pb_pp_ihb_program_instruction_table_tbl_set_unsafe(
              unit,
              pmf_key,
              pmf_pgm_ndx,
              ce_ndx,
              &tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    }
  }

  /*
   *	2. Program properties
   */
  SOC_PB_PMF_PGM_INFO_clear(&pgm_info);
  res = soc_pb_pmf_pgm_get_unsafe(
          unit,
          pmf_pgm_ndx,
          &pgm_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  pgm_info.lkp_profile_id[0] = pfg_ndx;
  pgm_info.lkp_profile_id[1] = pfg_ndx;
  pgm_info.tag_profile_id = pfg_ndx;
  pgm_info.bytes_to_rmv.header_type = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START;
  pgm_info.bytes_to_rmv.nof_bytes = 0; /* Nothing to remove */
  if (pp_port_info->first_header_size)
  {
    /* The Sequence number for Fat Pipe for example */
    pgm_info.bytes_to_rmv.nof_bytes += pp_port_info->first_header_size;
  }
  pgm_info.header_type = SOC_PETRA_PORT_HEADER_TYPE_ETH;
  pgm_info.copy_pgm_var = 0;
  pgm_info.fc_type = pp_port_info->fc_type;

  res = soc_pb_pmf_pgm_set_unsafe(
          unit,
          pmf_pgm_ndx,
          &pgm_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_eth_pgm_set_unsafe()", 0, 0);
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

