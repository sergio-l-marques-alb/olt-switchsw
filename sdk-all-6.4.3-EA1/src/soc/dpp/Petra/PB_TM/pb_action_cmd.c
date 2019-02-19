/* $Id: pb_action_cmd.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_action_cmd.c
*
* MODULE PREFIX:  soc_pb_action
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
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_action_cmd.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>


#include <soc/dpp/Petra/petra_ports.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_ACTION_CMD_SNOOP_INFO_SNOOP_SIZE_MAX                (SOC_PB_ACTION_NOF_CMD_SIZE_BYTESS-1)
#define SOC_PB_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MAX                (1023)
#define SOC_PB_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN                (0)
#define SOC_PB_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MAX              (1023)
#define SOC_PB_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN              (0)

#define SOC_PB_ACTION_CMD_SNOOP_SIZE_NOF_BITS                      (2)
#define SOC_PB_ACTION_CMD_MIRROR_TABLE_OFFSET                      (16)

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
*     Set a snoop action profile in the snoop action profile
 *     table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_action_cmd_snoop_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  SOC_PB_ACTION_CMD_SNOOP_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK,
    snoop_size_enc,
    fld_val,
    reg_val;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA
    snp_act_profile_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACTION_CMD_SNOOP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_CLEAR(&snp_mrr_tbl0_data,SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  SOC_PETRA_CLEAR(&snp_mrr_tbl1_data,SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  SOC_PETRA_CLEAR(&snp_act_profile_tbl_data,SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA,1);

  regs = soc_petra_regs();

  /* Encode snoop size */
  SOC_PB_REG_GET(regs->irr.snoop_size_reg.snoop_size, reg_val, 10, exit);
  switch (info->size)
  {
    case SOC_TMC_ACTION_CMD_SIZE_BYTES_64:
      snoop_size_enc = 0x0;
      break;
    case SOC_TMC_ACTION_CMD_SIZE_BYTES_128:
      snoop_size_enc = 0x1;
      break;
    case SOC_TMC_ACTION_CMD_SIZE_BYTES_192:
      snoop_size_enc = 0x2;
      break;
    case SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT:
      snoop_size_enc = 0x3;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR, 10, exit);
      break;
  }

  res = soc_sand_bitstream_set_field(&reg_val,(2 * action_ndx) - 2,SOC_PB_ACTION_CMD_SNOOP_SIZE_NOF_BITS,snoop_size_enc);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  SOC_PB_REG_SET(regs->irr.snoop_size_reg.snoop_size, reg_val, 20, exit);

  /* Encode destination type and id */
  switch(info->cmd.dest_id.type)
  {
  case SOC_PETRA_DEST_TYPE_QUEUE:
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | ( 1 << 15);
    break;
  case SOC_PETRA_DEST_TYPE_MULTICAST:
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | ( 1 << 14);
    break;
  case SOC_PETRA_DEST_TYPE_SYS_PHY_PORT:
    soc_petra_ports_logical_sys_id_build(FALSE, 0, 0, info->cmd.dest_id.id, &snp_mrr_tbl0_data.destination);
    break;
  case SOC_PETRA_DEST_TYPE_LAG:
    soc_petra_ports_logical_sys_id_build(TRUE, info->cmd.dest_id.id, 0, 0, &snp_mrr_tbl0_data.destination);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_ACTION_CMD_DEST_ID_OUT_OF_RANGE_ERR, 30, exit);
    break;
  }

  snp_mrr_tbl0_data.dp          = info->cmd.dp.value;
  snp_mrr_tbl0_data.dp_ow       = SOC_SAND_BOOL2NUM(info->cmd.dp.enable);
  snp_mrr_tbl0_data.tc          = info->cmd.tc.value;
  snp_mrr_tbl0_data.tc_ow       = SOC_SAND_BOOL2NUM(info->cmd.tc.enable);

  /* After setting all parameters in struct, write it to HW*/
  res = soc_pb_irr_snoop_mirror_table0_tbl_set_unsafe(
          unit,
          action_ndx,
          &snp_mrr_tbl0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  snp_mrr_tbl1_data.counter_ptr0    = info->cmd.counter_ptr_1.value;
  snp_mrr_tbl1_data.counter_ptr0_ow = SOC_SAND_BOOL2NUM(info->cmd.counter_ptr_1.enable);
  snp_mrr_tbl1_data.counter_ptr1    = info->cmd.counter_ptr_2.value;
  snp_mrr_tbl1_data.counter_ptr1_ow = SOC_SAND_BOOL2NUM(info->cmd.counter_ptr_2.enable);
  snp_mrr_tbl1_data.dp_cmd          = info->cmd.meter_dp.value;
  snp_mrr_tbl1_data.dp_cmd_ow       = SOC_SAND_BOOL2NUM(info->cmd.meter_dp.enable);
  snp_mrr_tbl1_data.meter_ptr0      = info->cmd.meter_ptr_low.value;
  snp_mrr_tbl1_data.meter_ptr0_ow   = SOC_SAND_BOOL2NUM(info->cmd.meter_ptr_low.enable);
  snp_mrr_tbl1_data.meter_ptr1      = info->cmd.meter_ptr_up.value;
  snp_mrr_tbl1_data.meter_ptr1_ow   = SOC_SAND_BOOL2NUM(info->cmd.meter_ptr_up.enable);
  
  /* After setting all parameters in struct, write it to HW*/
  res = soc_pb_irr_snoop_mirror_table1_tbl_set_unsafe(
          unit,
          SOC_PB_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,
          &snp_mrr_tbl1_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PB_FLD_GET(regs->idr.ingress_multicast_indication_reg.snoop_is_ing_mc , fld_val, 60, exit);
  SOC_SAND_SET_BIT(fld_val,SOC_SAND_NUM2BOOL(info->cmd.is_ing_mc),action_ndx);
  SOC_PB_FLD_SET(regs->idr.ingress_multicast_indication_reg.snoop_is_ing_mc , fld_val, 70, exit);

  /* Set snoop probability */
  snp_act_profile_tbl_data.snp_act_snp_sampling_probability = info->prob;

  res = soc_pb_pp_ihb_snp_act_profile_tbl_set_unsafe(
          unit,
          action_ndx,
          &snp_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_action_cmd_snoop_set_unsafe()", action_ndx, 0);
}

uint32
  soc_pb_action_cmd_snoop_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  SOC_PB_ACTION_CMD_SNOOP_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACTION_CMD_SNOOP_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, SOC_PB_ACTION_NDX_MIN, SOC_PB_ACTION_NDX_MAX, SOC_PB_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_SNOOP_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_action_cmd_snoop_set_verify()", action_ndx, 0);
}

uint32
  soc_pb_action_cmd_snoop_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACTION_CMD_SNOOP_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, SOC_PB_ACTION_NDX_MIN, SOC_PB_ACTION_NDX_MAX, SOC_PB_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /* Implanted */
  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_action_cmd_snoop_get_verify()", action_ndx, 0);
}

/*********************************************************************
*     Set a snoop action profile in the snoop action profile
 *     table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_action_cmd_snoop_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_OUT SOC_PB_ACTION_CMD_SNOOP_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK,
    snoop_size_enc,
    fld_val,
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA
    snp_act_profile_tbl_data;
  uint8
    is_lag_not_phys;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACTION_CMD_SNOOP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_ACTION_CMD_SNOOP_INFO_clear(info);

  SOC_PETRA_CLEAR(&snp_mrr_tbl0_data,SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  SOC_PETRA_CLEAR(&snp_mrr_tbl1_data,SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  SOC_PETRA_CLEAR(&snp_act_profile_tbl_data,SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA,1);

  regs = soc_petra_regs();
  
  /* Encode snoop size */
  SOC_PB_FLD_GET(regs->irr.snoop_size_reg.snoop_size, fld_val, 10, exit);
  snoop_size_enc = SOC_SAND_GET_BITS_RANGE(fld_val, (2 * action_ndx) - 1 , (2 * action_ndx) - 2);
  switch (snoop_size_enc)
  {
    case 0x0:
      info->size = SOC_TMC_ACTION_CMD_SIZE_BYTES_64;
      break;
    case 0x1:
      info->size = SOC_TMC_ACTION_CMD_SIZE_BYTES_128;
      break;
    case 0x2:
      info->size = SOC_TMC_ACTION_CMD_SIZE_BYTES_192;
        break;
    case 0x3:
      info->size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR, 10, exit);
      break;
  }

  res = soc_pb_irr_snoop_mirror_table0_tbl_get_unsafe(
          unit,
          action_ndx,
          &snp_mrr_tbl0_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Encode destination type and id */
  if (snp_mrr_tbl0_data.destination & ( 1 << 15))
  {
    info->cmd.dest_id.type = SOC_PETRA_DEST_TYPE_QUEUE;
    info->cmd.dest_id.id = SOC_SAND_GET_BITS_RANGE(snp_mrr_tbl0_data.destination, 14 , 0);
  }
  else if (snp_mrr_tbl0_data.destination & ( 1 << 14))
  {
    info->cmd.dest_id.type = SOC_PETRA_DEST_TYPE_MULTICAST;
    info->cmd.dest_id.id = SOC_SAND_GET_BITS_RANGE(snp_mrr_tbl0_data.destination, 13 , 0);
  }
  else
  {
    soc_petra_ports_logical_sys_id_parse(
      snp_mrr_tbl0_data.destination,
      &is_lag_not_phys,
      &lag_id,
      &lag_member_id,
      &sys_phys_port_id
    );
  
    if (is_lag_not_phys)
    {
      info->cmd.dest_id.type = SOC_PETRA_DEST_TYPE_LAG;
      info->cmd.dest_id.id = lag_id;
    }
    else
    {
      info->cmd.dest_id.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
      info->cmd.dest_id.id = sys_phys_port_id;
    }
  }
  
  info->cmd.dp.value  = snp_mrr_tbl0_data.dp;
  info->cmd.dp.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl0_data.dp_ow);
  info->cmd.tc.value  = snp_mrr_tbl0_data.tc;
  info->cmd.tc.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl0_data.tc_ow);

  res = soc_pb_irr_snoop_mirror_table1_tbl_get_unsafe(
          unit,
          SOC_PB_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,
          &snp_mrr_tbl1_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  info->cmd.counter_ptr_1.value  = snp_mrr_tbl1_data.counter_ptr0;
  info->cmd.counter_ptr_1.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.counter_ptr0_ow);
  info->cmd.counter_ptr_2.value  = snp_mrr_tbl1_data.counter_ptr1;
  info->cmd.counter_ptr_2.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.counter_ptr1_ow);
  info->cmd.meter_dp.value       = snp_mrr_tbl1_data.dp_cmd;
  info->cmd.meter_dp.enable      = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.dp_cmd_ow);
  info->cmd.meter_ptr_low.value  = snp_mrr_tbl1_data.meter_ptr0;
  info->cmd.meter_ptr_low.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.meter_ptr0_ow);
  info->cmd.meter_ptr_up.value   = snp_mrr_tbl1_data.meter_ptr1;
  info->cmd.meter_ptr_up.enable  = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.meter_ptr1_ow);

  SOC_PB_FLD_GET(regs->idr.ingress_multicast_indication_reg.snoop_is_ing_mc, fld_val, 40, exit);
  info->cmd.is_ing_mc = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val,action_ndx));
  
  /* get snoop probability */
  res = soc_pb_pp_ihb_snp_act_profile_tbl_get_unsafe(
          unit,
          action_ndx,
          &snp_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  info->prob = snp_act_profile_tbl_data.snp_act_snp_sampling_probability;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_action_cmd_snoop_get_unsafe()", action_ndx, 0);
}

/*********************************************************************
*     Set a mirror action profile in the mirror action profile
 *     table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_action_cmd_mirror_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  SOC_PB_ACTION_CMD_MIRROR_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA
    mrr_act_profile_tbl_data;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACTION_CMD_MIRROR_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_CLEAR(&snp_mrr_tbl0_data,SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  SOC_PETRA_CLEAR(&snp_mrr_tbl1_data,SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  SOC_PETRA_CLEAR(&mrr_act_profile_tbl_data,SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA,1);

  regs = soc_petra_regs();

  /* Encode destination type and id */
  switch(info->cmd.dest_id.type)
  {
  case SOC_PETRA_DEST_TYPE_QUEUE:
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | ( 1 << 15);
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id  == (uint32)(-1) ?
      (1 << 16) - 1 :                    /* mark to drop */
      info->cmd.dest_id.id | (1 << 15);  /* regular queue / flow id */
    break;
  case SOC_PETRA_DEST_TYPE_MULTICAST:
    snp_mrr_tbl0_data.destination = info->cmd.dest_id.id | ( 1 << 14);
    break;
  case SOC_PETRA_DEST_TYPE_SYS_PHY_PORT:
    soc_petra_ports_logical_sys_id_build(FALSE, 0, 0, info->cmd.dest_id.id, &snp_mrr_tbl0_data.destination);
    break;
  case SOC_PETRA_DEST_TYPE_LAG:
    soc_petra_ports_logical_sys_id_build(TRUE, info->cmd.dest_id.id, 0, 0, &snp_mrr_tbl0_data.destination);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_ACTION_CMD_DEST_ID_OUT_OF_RANGE_ERR, 30, exit);
    break;
  }

  snp_mrr_tbl0_data.dp          = info->cmd.dp.value;
  snp_mrr_tbl0_data.dp_ow       = SOC_SAND_BOOL2NUM(info->cmd.dp.enable);
  snp_mrr_tbl0_data.tc          = info->cmd.tc.value;
  snp_mrr_tbl0_data.tc_ow       = SOC_SAND_BOOL2NUM(info->cmd.tc.enable);

  /* After setting all parameters in struct, write it to HW*/
  res = soc_pb_irr_snoop_mirror_table0_tbl_set_unsafe(
    unit,
    SOC_PB_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,
    &snp_mrr_tbl0_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  snp_mrr_tbl1_data.counter_ptr0    = info->cmd.counter_ptr_1.value;
  snp_mrr_tbl1_data.counter_ptr0_ow = SOC_SAND_BOOL2NUM(info->cmd.counter_ptr_1.enable);
  snp_mrr_tbl1_data.counter_ptr1    = info->cmd.counter_ptr_2.value;
  snp_mrr_tbl1_data.counter_ptr1_ow = SOC_SAND_BOOL2NUM(info->cmd.counter_ptr_2.enable);
  snp_mrr_tbl1_data.dp_cmd          = info->cmd.meter_dp.value;
  snp_mrr_tbl1_data.dp_cmd_ow       = SOC_SAND_BOOL2NUM(info->cmd.meter_dp.enable);
  snp_mrr_tbl1_data.meter_ptr0      = info->cmd.meter_ptr_low.value;
  snp_mrr_tbl1_data.meter_ptr0_ow   = SOC_SAND_BOOL2NUM(info->cmd.meter_ptr_low.enable);
  snp_mrr_tbl1_data.meter_ptr1      = info->cmd.meter_ptr_up.value;
  snp_mrr_tbl1_data.meter_ptr1_ow   = SOC_SAND_BOOL2NUM(info->cmd.meter_ptr_up.enable);

  /* After setting all parameters in struct, write it to HW*/
  res = soc_pb_irr_snoop_mirror_table1_tbl_set_unsafe(
    unit,
    action_ndx, /* The index of Table1 are inverted with table0 */
    &snp_mrr_tbl1_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PB_FLD_GET(regs->idr.ingress_multicast_indication_reg.mirror_is_ing_mc , fld_val, 60, exit);
  SOC_SAND_SET_BIT(fld_val,SOC_SAND_NUM2BOOL(info->cmd.is_ing_mc), action_ndx);
  SOC_PB_FLD_SET(regs->idr.ingress_multicast_indication_reg.mirror_is_ing_mc , fld_val, 70, exit);

  /* Set mirror probability */
  mrr_act_profile_tbl_data.mrr_act_mrr_sampling_probability = info->prob;

  res = soc_pb_pp_ihb_mrr_act_profile_tbl_set_unsafe(
          unit,
          action_ndx,
          &mrr_act_profile_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_action_cmd_mirror_set_unsafe()", action_ndx, 0);
}

uint32
  soc_pb_action_cmd_mirror_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  SOC_PB_ACTION_CMD_MIRROR_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACTION_CMD_MIRROR_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, SOC_PB_ACTION_NDX_MIN, SOC_PB_ACTION_NDX_MAX, SOC_PB_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_MIRROR_INFO, info, 20, exit);

  /* IMPLEMENTED */

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_action_cmd_mirror_set_verify()", action_ndx, 0);
}

uint32
  soc_pb_action_cmd_mirror_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACTION_CMD_MIRROR_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, SOC_PB_ACTION_NDX_MIN, SOC_PB_ACTION_NDX_MAX, SOC_PB_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);

  /* IMPLEMENTED */

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_action_cmd_mirror_get_verify()", action_ndx, 0);
}

/*********************************************************************
*     Set a mirror action profile in the mirror action profile
 *     table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_action_cmd_mirror_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_OUT SOC_PB_ACTION_CMD_MIRROR_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    snp_mrr_tbl0_data;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    snp_mrr_tbl1_data;
  SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA
    mrr_act_profile_tbl_data;
  uint8
    is_lag_not_phys;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACTION_CMD_MIRROR_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_ACTION_CMD_MIRROR_INFO_clear(info);

  SOC_PETRA_CLEAR(&snp_mrr_tbl0_data,SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA,1);
  SOC_PETRA_CLEAR(&snp_mrr_tbl1_data,SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA,1);
  SOC_PETRA_CLEAR(&mrr_act_profile_tbl_data,SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA,1);

  regs = soc_petra_regs();

  res = soc_pb_irr_snoop_mirror_table0_tbl_get_unsafe(
    unit,
    SOC_PB_ACTION_CMD_MIRROR_TABLE_OFFSET + action_ndx,
    &snp_mrr_tbl0_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Encode destination type and id */
  if (snp_mrr_tbl0_data.destination & ( 1 << 15))
  {
    info->cmd.dest_id.type = SOC_PETRA_DEST_TYPE_QUEUE;
    info->cmd.dest_id.id = SOC_SAND_GET_BITS_RANGE(snp_mrr_tbl0_data.destination, 14 , 0);
  }
  else if (snp_mrr_tbl0_data.destination & ( 1 << 14))
  {
    info->cmd.dest_id.type = SOC_PETRA_DEST_TYPE_MULTICAST;
    info->cmd.dest_id.id = SOC_SAND_GET_BITS_RANGE(snp_mrr_tbl0_data.destination, 13 , 0);
  }
  else
  {
    soc_petra_ports_logical_sys_id_parse(
      snp_mrr_tbl0_data.destination,
      &is_lag_not_phys,
      &lag_id,
      &lag_member_id,
      &sys_phys_port_id
      );

    if (is_lag_not_phys)
    {
      info->cmd.dest_id.type = SOC_PETRA_DEST_TYPE_LAG;
      info->cmd.dest_id.id = lag_id;
    }
    else
    {
      info->cmd.dest_id.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
      info->cmd.dest_id.id = sys_phys_port_id;
    }
  }

  info->cmd.dp.value  = snp_mrr_tbl0_data.dp;
  info->cmd.dp.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl0_data.dp_ow);
  info->cmd.tc.value  = snp_mrr_tbl0_data.tc;
  info->cmd.tc.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl0_data.tc_ow);

  res = soc_pb_irr_snoop_mirror_table1_tbl_get_unsafe(
    unit,
    action_ndx,
    &snp_mrr_tbl1_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  info->cmd.counter_ptr_1.value  = snp_mrr_tbl1_data.counter_ptr0;
  info->cmd.counter_ptr_1.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.counter_ptr0_ow);
  info->cmd.counter_ptr_2.value  = snp_mrr_tbl1_data.counter_ptr1;
  info->cmd.counter_ptr_2.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.counter_ptr1_ow);
  info->cmd.meter_dp.value       = snp_mrr_tbl1_data.dp_cmd;
  info->cmd.meter_dp.enable      = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.dp_cmd_ow);
  info->cmd.meter_ptr_low.value  = snp_mrr_tbl1_data.meter_ptr0;
  info->cmd.meter_ptr_low.enable = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.meter_ptr0_ow);
  info->cmd.meter_ptr_up.value   = snp_mrr_tbl1_data.meter_ptr1;
  info->cmd.meter_ptr_up.enable  = SOC_SAND_NUM2BOOL(snp_mrr_tbl1_data.meter_ptr1_ow);

  SOC_PB_FLD_GET(regs->idr.ingress_multicast_indication_reg.mirror_is_ing_mc, fld_val, 40, exit);
  info->cmd.is_ing_mc = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val, action_ndx));

  /* Get mirror probablity */
  res = soc_pb_pp_ihb_mrr_act_profile_tbl_get_unsafe(
    unit,
    action_ndx,
    &mrr_act_profile_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  
  info->prob = mrr_act_profile_tbl_data.mrr_act_mrr_sampling_probability;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_action_cmd_mirror_get_unsafe()", action_ndx, 0);
}

uint32
  SOC_PB_ACTION_CMD_OVERRIDE_verify(
    SOC_SAND_IN  SOC_PB_ACTION_CMD_OVERRIDE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_ACTION_CMD_OVERRIDE_verify()",0,0);
}

uint32
  SOC_PB_ACTION_CMD_verify(
    SOC_SAND_IN  SOC_PB_ACTION_CMD *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_OVERRIDE, &(info->tc), 11, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_OVERRIDE, &(info->dp), 12, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_OVERRIDE, &(info->meter_ptr_low), 13, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_OVERRIDE, &(info->meter_ptr_up), 14, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_OVERRIDE, &(info->meter_dp), 15, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_OVERRIDE, &(info->counter_ptr_1), 16, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD_OVERRIDE, &(info->counter_ptr_2), 17, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_ACTION_CMD_verify()",0,0);
}

uint32
  SOC_PB_ACTION_CMD_SNOOP_INFO_verify(
    SOC_SAND_IN  SOC_PB_ACTION_CMD_SNOOP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD, &(info->cmd), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->size, SOC_PB_ACTION_CMD_SNOOP_INFO_SNOOP_SIZE_MAX, SOC_PB_ACTION_SNOOP_SIZE_OUT_OF_RANGE_ERR, 11, exit);
  /* SOC_PB_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN may be changed and be grater than 0 */
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->prob, SOC_PB_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN, SOC_PB_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MAX, SOC_PB_ACTION_SNOOP_PROB_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_ACTION_CMD_SNOOP_INFO_verify()",0,0);
}

uint32
  SOC_PB_ACTION_CMD_MIRROR_INFO_verify(
    SOC_SAND_IN  SOC_PB_ACTION_CMD_MIRROR_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_ACTION_CMD, &(info->cmd), 10, exit);
  /* SOC_PB_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN may be changed and be grater than 0 */
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->prob, SOC_PB_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN, SOC_PB_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MAX, SOC_PB_ACTION_MIRROR_PROB_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_ACTION_CMD_MIRROR_INFO_verify()",0,0);
}
#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

