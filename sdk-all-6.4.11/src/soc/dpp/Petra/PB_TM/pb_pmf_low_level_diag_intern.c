/* $Id: pb_pmf_low_level_diag_intern.c,v 1.8 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>


#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_diag_intern.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PMF_LOW_LEVEL_DIAG_NOF_FORCE_ACTIONS                (4)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_PROG_ID_MAX                      (31)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_ENABLE_MAX                       (31)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_BUFF_MAX                         (SOC_SAND_U32_MAX)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_BUFF_LEN_MAX                     (SOC_SAND_U32_MAX)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_TYPE_MAX                         (SOC_PB_PMF_NOF_DIAG_FORCES-1)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_VALUE_MAX                        (SOC_SAND_U32_MAX)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_VAL_MAX                          (SOC_SAND_U32_MAX)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_STRENGTH_MAX                     (7)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_SNP_STRENGTH_MAX                 (3)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_OTH_STRENGTH_MAX                 (0)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_PASS_NUM_MAX                     (1)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_PRG_MAX                          (32)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_IN_KEY_MAX                       (SOC_SAND_U32_MAX)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_IN_PRG_MAX                       (7)

#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_A_SIZE       (176)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_B_SIZE       (176)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_ETH_SIZE     (144)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV4_SIZE    (144)
#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV6_SIZE    (288)

#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_A_SIZE_U32       (SOC_SAND_DIV_ROUND_UP(SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_A_SIZE,32))
#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_B_SIZE_U32       (SOC_SAND_DIV_ROUND_UP(SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_B_SIZE,32))
#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_ETH_SIZE_U32       (SOC_SAND_DIV_ROUND_UP(SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_ETH_SIZE,32))
#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV4_SIZE_U32       (SOC_SAND_DIV_ROUND_UP(SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV4_SIZE,32))
#define SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV6_SIZE_U32       (SOC_SAND_DIV_ROUND_UP(SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV6_SIZE,32))

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

/*********************************************************************
*     Force PMF to execute the given program.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_prog_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    prog_id,
    SOC_SAND_IN  uint8                   enable
  )
{
  uint32
    tmp,
    reg_val;
  SOC_PB_PP_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_PROG_SET_UNSAFE);

  SOC_SAND_TODO_IMPLEMENT_WARNING;

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->ihb.pmfgeneral_configuration1_reg, reg_val, 10, exit);

  tmp = SOC_SAND_NUM2BOOL(enable);
  SOC_PB_PP_FLD_TO_REG(regs->ihb.pmfgeneral_configuration1_reg.force_program,tmp,reg_val,20,exit);

  tmp = prog_id;
  SOC_PB_PP_FLD_TO_REG(regs->ihb.pmfgeneral_configuration1_reg.force_program_num,tmp,reg_val,30,exit);

  SOC_PB_PP_REG_SET(regs->ihb.pmfgeneral_configuration1_reg, reg_val, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_prog_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_diag_force_prog_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    prog_id,
    SOC_SAND_IN  uint8                   enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_PROG_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(prog_id, SOC_PB_PMF_LOW_LEVEL_DIAG_PROG_ID_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_PROG_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(enable, SOC_PB_PMF_LOW_LEVEL_DIAG_ENABLE_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_ENABLE_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_prog_set_verify()", 0, 0);
}

uint32
  soc_pb_pmf_diag_force_prog_get_verify(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_PROG_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_prog_get_verify()", 0, 0);
}

/*********************************************************************
*     Force PMF to execute the given program.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_prog_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *prog_id,
    SOC_SAND_OUT uint8                   *enable
  )
{
  uint32
    tmp,
    reg_val;
  SOC_PB_PP_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_PROG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prog_id);
  SOC_SAND_CHECK_NULL_INPUT(enable);


  SOC_SAND_TODO_IMPLEMENT_WARNING;

  regs = soc_pb_pp_regs();

  SOC_PB_PP_REG_GET(regs->ihb.pmfgeneral_configuration1_reg, reg_val, 10, exit);

  
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.pmfgeneral_configuration1_reg.force_program,tmp,reg_val,20,exit);
  *enable = SOC_SAND_BOOL2NUM(tmp);
  
  SOC_PB_PP_FLD_FROM_REG(regs->ihb.pmfgeneral_configuration1_reg.force_program_num,tmp,reg_val,30,exit);
  *prog_id = tmp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_prog_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Returns used programs for all packets since last call.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_selected_progs_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *progs_bmp
  )
{
  uint32
    reg_val;
  SOC_PETRA_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_SELECTED_PROGS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(progs_bmp);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->ihb.dbg_selected_program_reg, reg_val, 10, exit);
  *progs_bmp = reg_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_selected_progs_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_diag_selected_progs_get_verify(
    SOC_SAND_IN  int                   unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_SELECTED_PROGS_GET_VERIFY);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_selected_progs_get_verify()", 0, 0);
}


STATIC uint32
  soc_pb_pmf_diag_force_action_internal_info_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO     *action_info,
    SOC_SAND_OUT  uint32                   *action_code,
    SOC_SAND_OUT  uint32                   *valid1,
    SOC_SAND_OUT  uint32                   *value1,
    SOC_SAND_OUT  uint32                   *tc_valid,
    SOC_SAND_OUT  uint32                   *tc_value,
    SOC_SAND_OUT  uint32                   *dp_valid,
    SOC_SAND_OUT  uint32                   *dp_value

  )
{
  uint32
    num_of_valid_act=0,
    single_act=FALSE,
    dest_act=FALSE,
    tc_act=FALSE,
    dp_act=FALSE,
    ind=0,
    single_valid_index=0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_ACTION_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  for (ind = 0; ind < SOC_PB_PMF_LOW_LEVEL_DIAG_NOF_FORCE_ACTIONS; ++ind)
  {
    if (action_info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_NOP)
    {
      continue;
    }
    ++num_of_valid_act;
    if (num_of_valid_act == 1)
    {
      single_valid_index = ind;
    }
    /* if one action be supplied */
    if (action_info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_DEST)
    {
      dest_act = TRUE;
      *value1 = action_info->actions[ind].value;
      *valid1 = action_info->actions[ind].is_valid;
    }
    else if (action_info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_TC)
    {
      tc_act = TRUE;
      *tc_value = action_info->actions[ind].value;
      *tc_valid = action_info->actions[ind].is_valid;
    }
    else if (action_info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_DP)
    {
      dp_act = TRUE;
      *dp_value = action_info->actions[ind].value;
      *dp_valid = action_info->actions[ind].is_valid;
    }
    else{
      single_act = TRUE;
      *value1 = action_info->actions[ind].value;
      *valid1 = action_info->actions[ind].is_valid;
    }
  }
  if ((num_of_valid_act == 2) ||
      (num_of_valid_act > 3) ||
      ((num_of_valid_act == 3) && (!dest_act || !tc_act || !dp_act)) ||
      (num_of_valid_act > 1 && single_act)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_DIAG_INVALID_ACTS_COMBINATION_ERR, 50, exit);
  }
  if (num_of_valid_act == 0)
  {
    *action_code = SOC_PB_PMF_NOF_DIAG_FORCES;
  }
  else if (num_of_valid_act == 3)
  {
    /* num_of_valid_act may be 3 if the condition (!dest_act || !tc_act || !dp_act)) above is false 
      and single_act is false also */
    /* coverity[dead_error_line : FALSE] */
    *action_code = SOC_PB_PMF_DIAG_FORCE_INVALID;
  }
  else
  {
    *action_code = action_info->actions[single_valid_index].type;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_action_set_unsafe()", 0, 0);
}
/*********************************************************************
*     Force PMF to perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_action_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO     *action_info
  )
{
  uint32
    reg_val=0,
    strength=0,
    action_code=0,
    tc_value=0,
    tc_valid=0,
    dp_value=0,
    dp_valid=0,
    value1=0,
    valid1=0;
  SOC_PETRA_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_ACTION_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);


  regs = soc_petra_regs();

  res = soc_pb_pmf_diag_force_action_internal_info_get_unsafe(
          unit,
          action_info,
          &action_code,
          &valid1,
          &value1,
          &tc_valid,
          &tc_value,
          &dp_valid,
          &dp_value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

 /*
  * if no action is valid
  */
  if (action_code == SOC_PB_PMF_NOF_DIAG_FORCES)
  {
    /* set force invalid and exit*/
    reg_val = 0;
    SOC_PB_FLD_SET(regs->ihb.dbg_pmf_force_action1_reg.force_action, reg_val, 50, exit);
    goto exit;
  }

  strength = action_info->strength;
 /*
  *
  */
  reg_val = 0;
  switch(action_code)
  {
    case SOC_PB_PMF_DIAG_FORCE_DEST:
        /* FwdDestValid     */
        res = soc_sand_set_field(&reg_val,0,0,valid1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        /* FwdDest          */
        res = soc_sand_set_field(&reg_val,20,4,value1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        /* FwdStrength      */
        res = soc_sand_set_field(&reg_val,26,24,strength);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
    case SOC_PB_PMF_DIAG_FORCE_TC:
        /* TcValid          */
        res = soc_sand_set_field(&reg_val,0,0,tc_valid);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        /* TC               */
        res = soc_sand_set_field(&reg_val,6,4,tc_value);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        /* FwdStrength      */
        res = soc_sand_set_field(&reg_val,10,8,strength);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_DP:
          /* DPValid          */
          res = soc_sand_set_field(&reg_val,0,0,dp_valid);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* DP               */
          res = soc_sand_set_field(&reg_val,5,4,dp_value);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* FwdStrength      */
          res = soc_sand_set_field(&reg_val,10,8,strength);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_INVALID:
          /* FwdDestValid     */
          res = soc_sand_set_field(&reg_val,0,0,valid1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* FwdDest          */
          res = soc_sand_set_field(&reg_val,17,1,value1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* TcValid          */
          res = soc_sand_set_field(&reg_val,18,18,tc_valid);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* TC               */
          res = soc_sand_set_field(&reg_val,21,19,dp_value);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* DPValid          */
          res = soc_sand_set_field(&reg_val,22,22,dp_valid);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* DP               */
          res = soc_sand_set_field(&reg_val,24,23,dp_value);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* FwdStrength      */
          res = soc_sand_set_field(&reg_val,27,25,strength);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_DP_METER:
        /* DP_MeterCmdValid      */
          res = soc_sand_set_field(&reg_val,0,0,valid1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* DP_MeterCmd      */
          res = soc_sand_set_field(&reg_val,5,4,value1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_FWD_STRENGTH:
        /* FwdStrength        */
        res = soc_sand_set_field(&reg_val,2,0,value1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_METER_A:
        /* MeterPtrAValid     */
        res = soc_sand_set_field(&reg_val,0,0,valid1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* MeterPtrA        */
          res = soc_sand_set_field(&reg_val,16,4,value1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_METER_B:
        /* MeterPtrBValid     */
        res = soc_sand_set_field(&reg_val,0,0,valid1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* MeterPtrB        */
          res = soc_sand_set_field(&reg_val,16,4,value1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_COUNTER_A:
        /* CounterPtrAValid   */
        res = soc_sand_set_field(&reg_val,0,0,valid1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* CounterPtrA      */
          res = soc_sand_set_field(&reg_val,15,4,value1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_COUNTER_B:
        /* CounterPtrBValid   */
        res = soc_sand_set_field(&reg_val,0,0,valid1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
          /* CounterPtrB      */
          res = soc_sand_set_field(&reg_val,15,4,value1);
          SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_CPU_TRAP_CODE:
       /* CpuTrapCode         */
       res = soc_sand_set_field(&reg_val,7,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
       /* CpuTrapQual      */
       res = soc_sand_set_field(&reg_val,21,8,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
       /* FwdStrength      */
       res = soc_sand_set_field(&reg_val,26,24,strength);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_SNOOP_CODE:
       /* SnoopCode           */
       res = soc_sand_set_field(&reg_val,7,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        /* SnoopStrength    */
        res = soc_sand_set_field(&reg_val,9,8,value1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_OUTBOUND_MIRROR_DIS:
       /* OutBoundMirrorDis   */
       res = soc_sand_set_field(&reg_val,0,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_EXCLUDE_SRC:
       /* ExcludeSrc          */
       res = soc_sand_set_field(&reg_val,0,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_INGRESS_SHAPING:
       /* IngressShaping      */
       res = soc_sand_set_field(&reg_val,15,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_MIRROR_ACTION:
       /* MirrorAction        */
       res = soc_sand_set_field(&reg_val,3,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_BYTES_TO_REMOVE:
       /* BytestoRemove       */
       res = soc_sand_set_field(&reg_val,6,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_HEADER_PROFILE_NDX:
       /* HeaderProfileNdx    */
       res = soc_sand_set_field(&reg_val,2,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_SEQ_NUM_TAG:
       /* SeqNumTag           */
       res = soc_sand_set_field(&reg_val,15,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
        break;
      case SOC_PB_PMF_DIAG_FORCE_STATISTICS_TAG:
       /* StatisticsTag       */
       res = soc_sand_set_field(&reg_val,31,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_LB_KEY:
       /* LbKey               */
       res = soc_sand_set_field(&reg_val,7,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_ECMP_KEY:
       /* ECMP_Key            */
       res = soc_sand_set_field(&reg_val,19,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_STACKING_ROUTE:
       /* StackingRoute       */
       res = soc_sand_set_field(&reg_val,15,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
      case SOC_PB_PMF_DIAG_FORCE_OUTLIF:
       /* OutLif              */
       res = soc_sand_set_field(&reg_val,15,0,value1);
       SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
      break;
    }
  SOC_PB_REG_SET(regs->ihb.dbg_pmf_force_action2_reg.force_action_value, reg_val, 30, exit);
  SOC_PB_FLD_SET(regs->ihb.dbg_pmf_force_action1_reg.force_action_type, action_code, 40, exit);
  reg_val = 1;
  SOC_PB_FLD_SET(regs->ihb.dbg_pmf_force_action1_reg.force_action, reg_val, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_action_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_diag_force_action_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO     *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_ACTION_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_ACTION_INFO, action_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_action_set_verify()", 0, 0);
}

uint32
  soc_pb_pmf_diag_force_action_get_verify(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_ACTION_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_action_get_verify()", 0, 0);
}

/*********************************************************************
*     Force PMF to perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_action_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_ACTION_INFO     *action_info
  )
{
  uint32
    reg_val,
    strength=0,
    action_code,
    tc_value=0,
    tc_valid=0,
    dp_value=0,
    dp_valid=0,
    value1,
    valid1=1;
  SOC_PETRA_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_ACTION_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  SOC_PB_PMF_DIAG_ACTION_INFO_clear(action_info);

  reg_val = 0;
  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->ihb.dbg_pmf_force_action1_reg.force_action, reg_val, 50, exit);
 /*
  * if no action is forced, done
  */
  if (!reg_val)
  {
    goto exit;
  }

  SOC_PB_FLD_GET(regs->ihb.dbg_pmf_force_action1_reg.force_action_type, action_code, 40, exit);
  SOC_PB_REG_GET(regs->ihb.dbg_pmf_force_action2_reg.force_action_value, reg_val, 45, exit);
 
  switch(action_code)
  {
  case SOC_PB_PMF_DIAG_FORCE_DEST:
    /* FwdDestValid     */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    /* FwdDest          */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,20,4);
    /* FwdStrength      */
    strength = SOC_SAND_GET_BITS_RANGE(reg_val,26,24);
    break;
  case SOC_PB_PMF_DIAG_FORCE_TC:
    /* TcValid          */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    /* TC               */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,6,4);
    /* FwdStrength      */
    strength = SOC_SAND_GET_BITS_RANGE(reg_val,10,8);
    break;
  case SOC_PB_PMF_DIAG_FORCE_DP:
    /* DPValid          */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    /* DP               */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,5,4);
    /* FwdStrength      */
    strength = SOC_SAND_GET_BITS_RANGE(reg_val,10,8);
    break;
  case SOC_PB_PMF_DIAG_FORCE_INVALID:
    /* FwdDestValid     */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    /* FwdDest          */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,17,1);
    /* TcValid          */
    tc_valid = SOC_SAND_GET_BITS_RANGE(reg_val,18,18);
    /* TC               */
    tc_value = SOC_SAND_GET_BITS_RANGE(reg_val,21,19);
    /* DPValid          */
    dp_valid = SOC_SAND_GET_BITS_RANGE(reg_val,22,22);
    /* DP               */
    dp_value = SOC_SAND_GET_BITS_RANGE(reg_val,24,23);
    /* FwdStrength      */
    strength = SOC_SAND_GET_BITS_RANGE(reg_val,27,25);
    break;
  case SOC_PB_PMF_DIAG_FORCE_DP_METER:
    /* DP_MeterCmdValid      */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    /* DP_MeterCmd      */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,5,4);
    break;
  case SOC_PB_PMF_DIAG_FORCE_FWD_STRENGTH:
    /* FwdStrength        */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,2,0);
    break;
  case SOC_PB_PMF_DIAG_FORCE_METER_A:
    /* MeterPtrAValid     */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    /* MeterPtrA        */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,16,4);
    break;
  case SOC_PB_PMF_DIAG_FORCE_METER_B:
    /* MeterPtrBValid     */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    /* MeterPtrB        */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,16,4);
    break;
  case SOC_PB_PMF_DIAG_FORCE_COUNTER_A:
    /* CounterPtrAValid   */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    /* CounterPtrA      */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,15,4);
    break;
  case SOC_PB_PMF_DIAG_FORCE_COUNTER_B:
    /* CounterPtrBValid   */
    valid1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    /* CounterPtrB      */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,15,4);
    break;
  case SOC_PB_PMF_DIAG_FORCE_CPU_TRAP_CODE:
    /* CpuTrapCode         */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,7,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    /* CpuTrapQual      */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,21,8);
    /* FwdStrength      */
    strength = SOC_SAND_GET_BITS_RANGE(reg_val,26,24);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  case SOC_PB_PMF_DIAG_FORCE_SNOOP_CODE:
    /* SnoopCode           */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,7,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    /* SnoopStrength    */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,9,8);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  case SOC_PB_PMF_DIAG_FORCE_OUTBOUND_MIRROR_DIS:
    /* OutBoundMirrorDis   */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  case SOC_PB_PMF_DIAG_FORCE_EXCLUDE_SRC:
    /* ExcludeSrc          */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,0,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  case SOC_PB_PMF_DIAG_FORCE_INGRESS_SHAPING:
    /* IngressShaping      */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,15,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  case SOC_PB_PMF_DIAG_FORCE_MIRROR_ACTION:
    /* MirrorAction        */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,3,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  case SOC_PB_PMF_DIAG_FORCE_BYTES_TO_REMOVE:
    /* BytestoRemove       */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,6,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  case SOC_PB_PMF_DIAG_FORCE_HEADER_PROFILE_NDX:
    /* HeaderProfileNdx    */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,2,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  case SOC_PB_PMF_DIAG_FORCE_SEQ_NUM_TAG:
    /* SeqNumTag           */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,15,0);
    break;
  case SOC_PB_PMF_DIAG_FORCE_STATISTICS_TAG:
    /* StatisticsTag       */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,31,0);
    break;
  case SOC_PB_PMF_DIAG_FORCE_LB_KEY:
    /* LbKey               */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,7,0);
    break;
  case SOC_PB_PMF_DIAG_FORCE_ECMP_KEY:
    /* ECMP_Key            */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,19,0);
    break;
  case SOC_PB_PMF_DIAG_FORCE_STACKING_ROUTE:
    /* StackingRoute       */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,15,0);
    break;
  case SOC_PB_PMF_DIAG_FORCE_OUTLIF:
    /* OutLif              */
    value1 = SOC_SAND_GET_BITS_RANGE(reg_val,15,0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    break;
  default:
    goto exit;
  }

  action_info->actions[0].is_valid = SOC_SAND_NUM2BOOL(valid1);
  action_info->actions[0].value = value1;
  action_info->actions[0].type = (action_code != SOC_PB_PMF_DIAG_FORCE_INVALID)?action_code:SOC_PB_PMF_DIAG_FORCE_DEST;
  action_info->strength = strength;
  
  if (action_code == SOC_PB_PMF_DIAG_FORCE_INVALID)
  {
    action_info->actions[1].is_valid = SOC_SAND_NUM2BOOL(tc_valid);
    action_info->actions[1].value = tc_value;
    action_info->actions[1].type = SOC_PB_PMF_DIAG_FORCE_TC;
  }
  if (action_code == SOC_PB_PMF_DIAG_FORCE_INVALID)
  {
    action_info->actions[1].is_valid = SOC_SAND_NUM2BOOL(dp_valid);
    action_info->actions[1].value = dp_value;
    action_info->actions[1].type = SOC_PB_PMF_DIAG_FORCE_DP;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_action_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Returns the keys built by the PMF for the last packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_built_keys_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_KEYS_INFO       *built_keys
  )
{
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_BUILT_KEYS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(built_keys);

  SOC_PB_PMF_DIAG_KEYS_INFO_clear(built_keys);

  regs = soc_petra_regs();
  pp_regs  = soc_pb_pp_regs();


  res = soc_petra_read_reg_buffer_unsafe(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->ihb.dbg_key_a_reg[0]).addr,
          SOC_TMC_DEFAULT_INSTANCE,
          SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_A_SIZE_U32,
          built_keys->custom_a.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  built_keys->custom_a.buff_len = SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_A_SIZE;

    res = soc_petra_read_reg_buffer_unsafe(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->ihb.dbg_key_b_reg[0]).addr,
          SOC_TMC_DEFAULT_INSTANCE,
          SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_B_SIZE_U32,
          built_keys->custom_b.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  built_keys->custom_b.buff_len = SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_B_SIZE;

    res = soc_petra_read_reg_buffer_unsafe(
          unit,
          (SOC_PETRA_REG_ADDR*)&(pp_regs->ihb.dbg_eth_key_reg_0),
          SOC_TMC_DEFAULT_INSTANCE,
          SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_ETH_SIZE_U32,
          built_keys->eth.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  built_keys->eth.buff_len = SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_ETH_SIZE;

    res = soc_petra_read_reg_buffer_unsafe(
          unit,
          (SOC_PETRA_REG_ADDR*)&(pp_regs->ihb.dbg_ipv4_key_reg_0),
          SOC_TMC_DEFAULT_INSTANCE,
          SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV4_SIZE_U32,
          built_keys->ipv4.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  built_keys->ipv4.buff_len = SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV4_SIZE;

  res = soc_petra_read_reg_buffer_unsafe(
          unit,
          (SOC_PETRA_REG_ADDR*)&(pp_regs->ihb.dbg_ipv6_key_reg_0),
          SOC_TMC_DEFAULT_INSTANCE,
          SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV6_SIZE_U32,
          built_keys->ipv6.buff
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  built_keys->ipv6.buff_len = SOC_PB_PMF_LOW_LEVEL_DIAG_KEY_IPV6_SIZE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_built_keys_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_diag_built_keys_get_verify(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_BUILT_KEYS_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_built_keys_get_verify()", 0, 0);
}

/*********************************************************************
*     Set pre-configuration so freeze inputs/outputs will be
 *     snapshoot according to this information
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_fem_freeze_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  )
{
  uint32
    reg_val,
    tmp;
  SOC_PETRA_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEM_FREEZE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(freeze_info);
  regs = soc_petra_regs();
  
  if (freeze_info->pass_num == 0)
  {
    /* if first pass set all to one*/
    reg_val = 0xff;
  }
  else
  {
    /* if second pass set all to zero */
    reg_val = 0;
  }
  SOC_PB_REG_SET(regs->ihb.dbg_fem_reg, reg_val, 10, exit);

  reg_val = 0;
  if (freeze_info->is_prg_valid)
  {
    tmp = 1;
    SOC_PB_FLD_TO_REG(regs->ihb.dbg_freeze_fem_reg.dbg_freeze_fem_at_pmf_program,tmp,reg_val,20,exit);
  }
  SOC_PB_FLD_TO_REG(regs->ihb.dbg_freeze_fem_reg.dbg_freeze_pmf_program,freeze_info->prg,reg_val,30,exit);

  tmp = freeze_info->freeze;
  SOC_PB_FLD_TO_REG(regs->ihb.dbg_freeze_fem_reg.dbg_freeze_fem,tmp,reg_val,50,exit);
  SOC_PB_REG_SET(regs->ihb.dbg_freeze_fem_reg, reg_val, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fem_freeze_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_diag_fem_freeze_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEM_FREEZE_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_FEM_FREEZE_INFO, freeze_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fem_freeze_set_verify()", 0, 0);
}

uint32
  soc_pb_pmf_diag_fem_freeze_get_verify(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEM_FREEZE_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fem_freeze_get_verify()", 0, 0);
}

/*********************************************************************
*     Set pre-configuration so freeze inputs/outputs will be
 *     snapshoot according to this information
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_fem_freeze_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  )
{
  uint32
    reg_val,
    tmp;
  SOC_PETRA_REGS
    *regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEM_FREEZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(freeze_info);

  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_clear(freeze_info);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->ihb.dbg_fem_reg, reg_val, 10, exit);
  if (reg_val == 0xff)
  {
    /* if first pass get all to one*/
    freeze_info->pass_num = 0;
  }
  else
  {
    /* if second pass get all to zero */
    freeze_info->pass_num = 1;
  }
  
  SOC_PB_REG_GET(regs->ihb.dbg_freeze_fem_reg, reg_val, 60, exit);

  SOC_PB_FLD_FROM_REG(regs->ihb.dbg_freeze_fem_reg.dbg_freeze_fem_at_pmf_program,tmp,reg_val,20,exit);

  if (tmp == 1)
  {
    freeze_info->is_prg_valid = TRUE;
  }
  SOC_PB_FLD_FROM_REG(regs->ihb.dbg_freeze_fem_reg.dbg_freeze_pmf_program,freeze_info->prg,reg_val,30,exit);

  SOC_PB_FLD_FROM_REG(regs->ihb.dbg_freeze_fem_reg.dbg_freeze_fem,tmp,reg_val,50,exit);
  freeze_info->freeze = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fem_freeze_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the FEM information including inputs and outputs to
 *     each FEM,
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_fems_info_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint8                   release,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_INFO        fems_info[8]
  )
{
  uint32
    indx,
    tmp;
  uint32
    reg_vals[16];
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEMS_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fems_info);

  for (indx = 0; indx < 8; ++indx)
  {
    SOC_PB_PMF_DIAG_FEM_INFO_clear(&(fems_info[indx]));
  }

  SOC_PETRA_CLEAR(reg_vals,uint32,16);

  pp_regs = soc_pb_pp_regs();
  regs = soc_petra_regs();

  res = soc_petra_read_reg_buffer_unsafe(
          unit,
          (SOC_PETRA_REG_ADDR*)&(pp_regs->ihb.dbg_femkey_reg).addr,
          SOC_TMC_DEFAULT_INSTANCE,
          16,
          reg_vals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (indx = 0; indx < 8; ++indx)
  {
    fems_info[indx].in_key = reg_vals[indx*2];
  }

  for (indx = 0; indx < 8; ++indx)
  {
    tmp = 0;
    SOC_PB_PP_FLD_FROM_REG(pp_regs->ihb.dbg_fem_reg.dbg_femaction_valid,tmp,reg_vals[indx*2+1],40,exit);
    fems_info[indx].out_action.is_valid = SOC_SAND_NUM2BOOL(tmp);

    tmp = 0;
    SOC_PB_PP_FLD_FROM_REG(pp_regs->ihb.dbg_fem_reg.dbg_femaction_type,tmp,reg_vals[indx*2+1],50,exit);
    fems_info[indx].out_action.type = tmp;

    tmp = 0;
    SOC_PB_PP_FLD_FROM_REG(pp_regs->ihb.dbg_fem_reg.dbg_femaction,tmp,reg_vals[indx*2+1],60,exit);
    fems_info[indx].out_action.value = tmp;

    tmp = 0;
    SOC_PB_PP_FLD_FROM_REG(pp_regs->ihb.dbg_fem_reg.dbg_femprogram,tmp,reg_vals[indx*2+1],70,exit);
    fems_info[indx].in_prg = tmp;
  }

  if (release)
  {
    tmp = 0;
    SOC_PB_FLD_SET(regs->ihb.dbg_freeze_fem_reg.dbg_freeze_fem,tmp,80,exit);
  }

    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fems_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_diag_fems_info_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint8                release
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEMS_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fems_info_get_verify()", 0, 0);
}


uint32
  SOC_PB_PMF_DIAG_BUFFER_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PMF_DIAG_BUFF_MAX_SIZE; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->buff[ind], SOC_PB_PMF_LOW_LEVEL_DIAG_BUFF_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_BUFF_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->buff_len, SOC_PB_PMF_LOW_LEVEL_DIAG_BUFF_LEN_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_DIAG_BUFFER_verify()",0,0);
}

uint32
  SOC_PB_PMF_DIAG_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PMF_LOW_LEVEL_DIAG_TYPE_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->value, SOC_PB_PMF_LOW_LEVEL_DIAG_VALUE_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_VALUE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_DIAG_ACTION_VAL_verify()",0,0);
}

uint32
  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL *info
  )

{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PMF_LOW_LEVEL_DIAG_TYPE_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->value, SOC_PB_PMF_LOW_LEVEL_DIAG_VAL_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_VAL_OUT_OF_RANGE_ERR, 11, exit);
  if (info->type == SOC_PB_PMF_DIAG_FORCE_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_DIAG_TYPE_OUT_OF_RANGE_ERR, 50, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_verify()",0,0);
}

uint32
  SOC_PB_PMF_DIAG_KEYS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_KEYS_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_BUFFER, &(info->eth), 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_BUFFER, &(info->ipv4), 11, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_BUFFER, &(info->ipv6), 12, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_BUFFER, &(info->custom_a), 13, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_BUFFER, &(info->custom_b), 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_DIAG_KEYS_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_DIAG_ACTION_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    num_of_valid_act=0,
    single_act=FALSE,
    dest_act=FALSE,
    is_snoop=FALSE,
    is_trap=FALSE,
    tc_act=FALSE,
    dp_act=FALSE;
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < 4; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_FORCE_ACTION_VAL, &(info->actions[ind]), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->strength, SOC_PB_PMF_LOW_LEVEL_DIAG_STRENGTH_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);

  for (ind = 0; ind < SOC_PB_PMF_LOW_LEVEL_DIAG_NOF_FORCE_ACTIONS; ++ind)
  {
    if (info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_NOP)
    {
      continue;
    }
    ++num_of_valid_act;
    /* if one action be supplied */
    if (info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_DEST)
    {
      dest_act = TRUE;
    }
    else if (info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_TC)
    {
      tc_act = TRUE;
    }
    else if (info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_DP)
    {
      dp_act = TRUE;
    }
    else{
      single_act = TRUE;
    }

    if (info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_SNOOP_CODE)
    {
      is_snoop = TRUE;
    }
    if (info->actions[ind].type == SOC_PB_PMF_DIAG_FORCE_CPU_TRAP_CODE)
    {
      is_trap = TRUE;
    }
    
  }

  if ((num_of_valid_act == 2) ||
      (num_of_valid_act > 3) ||
      ((num_of_valid_act == 3) && (!dest_act || !tc_act || !dp_act)) ||
      (num_of_valid_act > 1 && single_act)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_DIAG_INVALID_ACTS_COMBINATION_ERR, 50, exit);
  }

  if (is_snoop)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->strength, SOC_PB_PMF_LOW_LEVEL_DIAG_SNP_STRENGTH_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  }
  else if (!dest_act && !tc_act && !dp_act && !is_trap)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->strength, SOC_PB_PMF_LOW_LEVEL_DIAG_OTH_STRENGTH_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  }


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_DIAG_ACTION_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pass_num, SOC_PB_PMF_LOW_LEVEL_DIAG_PASS_NUM_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_PASS_NUM_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->prg, SOC_PB_PMF_LOW_LEVEL_DIAG_PRG_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_PRG_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_DIAG_FEM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_key, SOC_PB_PMF_LOW_LEVEL_DIAG_IN_KEY_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_IN_KEY_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_prg, SOC_PB_PMF_LOW_LEVEL_DIAG_IN_PRG_MAX, SOC_PB_PMF_LOW_LEVEL_DIAG_IN_PRG_OUT_OF_RANGE_ERR, 12, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_DIAG_ACTION_VAL, &(info->out_action), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_DIAG_FEM_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

