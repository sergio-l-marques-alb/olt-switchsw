/* $Id: pb_cnm.c,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_cnm.c
*
* MODULE PREFIX:  pb
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
#include <soc/dpp/Petra/PB_TM/pb_api_cnm.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_cnm.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PROFILE_NDX_MAX                                     (7)
#define SOC_PB_SAMPLING_RATE_MAX                                   (7)
#define SOC_PB_CPQ_NDX_MAX                                         (4*1024-1)
#define SOC_PB_CNM_Q_MAPPING_INFO_Q_SET_MAX                        (SOC_PB_CNM_NOF_Q_SETS-1)
#define SOC_PB_CNM_CPQ_INFO_PROFILE_MAX                            (7)
#define SOC_PB_CNM_PPH_ING_VLAN_EDIT_CMD_MAX                       (63)
#define SOC_PB_CNM_PDU_VERSION_MAX                                 (15)
#define SOC_PB_CNM_PDU_RES_V_MAX                                   (63)
#define SOC_PB_CNM_PACKET_TC_MAX                                   (7)
#define SOC_PB_CNM_PACKET_CP_ID_4_MSB_MAX                          (15)
#define SOC_PB_CNM_CP_INFO_PKT_GEN_MODE_MAX                        (SOC_PB_CNM_NOF_GEN_MODES-1)

#define SOC_PB_CNM_GEN_MODE_EXT_PP                                 SOC_TMC_CNM_GEN_MODE_EXT_PP
#define SOC_PB_CNM_GEN_MODE_PETRA_B_PP                             SOC_TMC_CNM_GEN_MODE_PETRA_B_PP
#define SOC_PB_CNM_GEN_MODE_SAMPLING                               SOC_TMC_CNM_GEN_MODE_SAMPLING
#define SOC_PB_CNM_Q_SET_8_CPS                                     SOC_TMC_CNM_Q_SET_8_CPS
#define SOC_PB_CNM_Q_SET_4_CPS                                     SOC_TMC_CNM_Q_SET_4_CPS

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
 *     Set the Congestion Point functionality, in Soc_petra B mode.
 *********************************************************************/

STATIC
  uint32
    soc_pb_cnm_cp_pp_set_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  SOC_PB_CNM_PETRA_B_PP           *info
      )
{
  uint32
    fld_val,
    ind,
    mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  fld_val = info->ether_type;

  SOC_PB_FLD_SET(regs->ipt.cnm_vlan_tag_reg.vlan_tag_tpid, fld_val, 101, exit);

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
         &(info->mac_sa),
          mac_add_long
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  
  for(ind = 0; ind < SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S; ind++)
  {
    fld_val = mac_add_long[ind];
    SOC_PB_FLD_SET(regs->ipt.cnm_mac_sa_reg[ind].cnm_mac_sa, fld_val, 121, exit);
  }
    
  fld_val = info->pdu.version;
  SOC_PB_FLD_SET(regs->ipt.cnm_pdu_reg.cnm_pdu_version, fld_val, 141, exit);

  fld_val = info->pdu.ether_type;
  SOC_PB_FLD_SET(regs->ipt.cnm_ether_type_reg.cnm_ether_type, fld_val, 151, exit);
        
  for(ind = 0; ind < SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S; ind++)
  {
    fld_val = info->pdu.cp_id_6_msb[ind];
    SOC_PB_FLD_SET(regs->ipt.cnm_pdu_cpid_msb_reg[ind].cnm_pdu_cpid_msb, fld_val, 171, exit);
  }
  
  fld_val = info->pph.ing_vlan_edit_cmd;
  SOC_PB_FLD_SET(regs->ipt.cnm_pph_reg.cnmpph_vlan_edit_cmd_with_cnm, fld_val, 181, exit);
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_pp_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Congestion Point functionality, in particular
 *     the packet generation mode and the fields of the CNM
 *     packet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_pb_cnm_cp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO              *info
    )
{

  uint32
    fld_cnm_dune_header_format = 0,
    fld_cnm_sampling_mode = 0,
    fld_val = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PB_FLD_SET(regs->iqm.iqm_enablers_reg.cnm_enable, info->is_cp_enabled, 10, exit);

  if (!info->is_cp_enabled)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }
  
  switch (info->pkt_gen_mode)
  {
  case SOC_PB_CNM_GEN_MODE_EXT_PP:
  	break;
  case SOC_PB_CNM_GEN_MODE_PETRA_B_PP:
    fld_cnm_dune_header_format = 1;

    res =  soc_pb_cnm_cp_pp_set_unsafe(
             unit,
             &(info->pp)
             );

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
  case SOC_PB_CNM_GEN_MODE_SAMPLING:
  
    fld_cnm_sampling_mode = 1;
    break;
  default:
    break;
  }

  SOC_PB_FLD_SET(regs->ipt.cnm_contorl_reg.cnm_dune_header_format, fld_cnm_dune_header_format, 30, exit );
  SOC_PB_FLD_SET(regs->ipt.cnm_contorl_reg.cnm_sampling_mode, fld_cnm_sampling_mode, 40, exit );

  fld_val = info->pckt.tc;
  SOC_PB_FLD_SET(regs->ipt.cnm_ftmh_reg.cnmftmh_traffic_class, fld_val, 50, exit);

  fld_val = info->pckt.cp_id_4_msb;
  SOC_PB_FLD_SET(regs->ipt.cnm_contorl_reg.cnm_ext_cpid_msb, fld_val, 60, exit);

  fld_val = info->pckt.dp;
  SOC_PB_FLD_SET(regs->ipt.cnm_ftmh_reg.cnmftmh_dp, fld_val, 65, exit);

  fld_val = info->pckt.qsig;
  SOC_PB_FLD_SET(regs->ipt.cnm_ftmh_reg.cnmftmh_qsig, fld_val, 66, exit);

  if (info->pckt.dest_tm_port == SOC_PB_CNM_DEST_TM_PORT_AS_INCOMING)
  {
    fld_val = 0x1;
    SOC_PB_FLD_SET(regs->ipt.cnm_ftmh_reg.cnmftmh_otm_is_itm, fld_val, 70, exit);
  }
  else
  {
    fld_val = 0x0;
    SOC_PB_FLD_SET(regs->ipt.cnm_ftmh_reg.cnmftmh_otm_is_itm, fld_val, 75, exit);

    fld_val = info->pckt.dest_tm_port;
    SOC_PB_FLD_SET(regs->ipt.cnm_ftmh_reg.cnmftmh_otm_port, fld_val, 80, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_set_unsafe()", 0, 0);
}

uint32
  soc_pb_cnm_cp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_CP_INFO, info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_set_verify()", 0, 0);
}

uint32
  soc_pb_cnm_cp_get_verify(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_get_verify()", 0, 0);
}


STATIC
  uint32
    soc_pb_cnm_cp_pp_get_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_OUT  SOC_PB_CNM_PETRA_B_PP           *info
      )
{
  uint32
    fld_val,
    ind,
    mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->ipt.cnm_vlan_tag_reg.vlan_tag_tpid, fld_val, 101, exit);

  info->ether_type = fld_val;

  for(ind = 0; ind < SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S; ind++)
  {
    
     SOC_PB_FLD_GET(regs->ipt.cnm_mac_sa_reg[ind].cnm_mac_sa, fld_val, 121, exit);
     mac_add_long[ind] = fld_val;
    
  }

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          mac_add_long,
          &(info->mac_sa)
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  SOC_PB_FLD_GET(regs->ipt.cnm_pdu_reg.cnm_pdu_version, fld_val, 141, exit);
  
  info->pdu.version = fld_val;

  SOC_PB_FLD_GET(regs->ipt.cnm_ether_type_reg.cnm_ether_type, fld_val, 151, exit);

  info->pdu.ether_type = fld_val;

  for(ind = 0; ind < SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S; ind++)
  {
    SOC_PB_FLD_GET(regs->ipt.cnm_pdu_cpid_msb_reg[ind].cnm_pdu_cpid_msb, fld_val, 171, exit);
    info->pdu.cp_id_6_msb[ind] = fld_val;
  }

  SOC_PB_FLD_GET(regs->ipt.cnm_pph_reg.cnmpph_vlan_edit_cmd_with_cnm, fld_val, 181, exit);

  info->pph.ing_vlan_edit_cmd = fld_val;

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_pp_set_unsafe()", 0, 0);
}

STATIC
  uint32
    soc_pb_cnm_cp_get_mode_unsafe(
      SOC_SAND_IN  int       unit,
      SOC_SAND_OUT SOC_PB_CNM_GEN_MODE *gen_mode
      )
{
    uint32
      fld_val1 = 0,
      fld_val2 = 0,
      res = SOC_SAND_OK;
    SOC_PETRA_REGS
      *regs;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_GET_UNSAFE);
  
  regs = soc_petra_regs();

   SOC_PB_FLD_GET(regs->ipt.cnm_contorl_reg.cnm_dune_header_format, fld_val1, 110, exit );

   if(fld_val1)
   {
      *gen_mode = SOC_PB_CNM_GEN_MODE_PETRA_B_PP;
   }

  SOC_PB_FLD_GET(regs->ipt.cnm_contorl_reg.cnm_sampling_mode, fld_val2, 130, exit );

  if(fld_val2)
  {
     if(!fld_val1)
     {
        *gen_mode = SOC_PB_CNM_GEN_MODE_SAMPLING;
     }
     else
     {
       SOC_SAND_SET_ERROR_CODE(  SOC_PB_CNM_CP_MULTIPLE_GEN_MODE, 211 , exit);
     }
  }

  if ( !fld_val1 && !fld_val2)
  {
    *gen_mode = SOC_PB_CNM_GEN_MODE_EXT_PP;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_get_unsafe()", 0, 0);
    
}



/*********************************************************************
*     Set the Congestion Point functionality, in particular
 *     the packet generation mode and the fields of the CNM
 *     packet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_CP_INFO              *info
  )
{
  uint32

    fld_is_cp_enabled = 0,
    fld_val = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_CNM_CP_INFO_clear(info);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->iqm.iqm_enablers_reg.cnm_enable, fld_is_cp_enabled, 10, exit);

  info->is_cp_enabled = SOC_SAND_NUM2BOOL(fld_is_cp_enabled);

  if (! fld_is_cp_enabled )
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }
  
  res = soc_pb_cnm_cp_get_mode_unsafe(
          unit,
          &(info->pkt_gen_mode)
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  if (info->pkt_gen_mode == SOC_PB_CNM_GEN_MODE_PETRA_B_PP)
  {
    res = soc_pb_cnm_cp_pp_get_unsafe(
            unit,
            &(info->pp)
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
     
  }

  SOC_PB_FLD_GET(regs->ipt.cnm_ftmh_reg.cnmftmh_traffic_class, fld_val, 240, exit);

  info->pckt.tc = fld_val;

  SOC_PB_FLD_GET(regs->ipt.cnm_contorl_reg.cnm_ext_cpid_msb, fld_val, 340, exit);
  
  info->pckt.cp_id_4_msb = fld_val;

  
  SOC_PB_FLD_GET(regs->ipt.cnm_ftmh_reg.cnmftmh_dp, fld_val, 360, exit);
  info->pckt.dp = fld_val;
  
  SOC_PB_FLD_GET(regs->ipt.cnm_ftmh_reg.cnmftmh_qsig, fld_val, 370, exit);
  info->pckt.qsig = fld_val;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the mapping from the pair (destination, traffic
 *     class) to the CP Queue.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_q_mapping_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO       *info
  )
{
  
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
 
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_Q_MAPPING_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  fld_val = info->q_base;

  /*
   *  Initializing regs->iqm.cp_queues_range_reg.cp_qnum_low with info->q_base
   */
  
  SOC_PB_FLD_SET(regs->iqm.cp_queues_range_reg.cp_qnum_low, fld_val, 10, exit);

  fld_val = info->q_base + info->nof_queues-1;

  /*
   *  Initializing regs->iqm.cp_queues_range_reg.cp_qnum_high with (info->q_base + info->nof_queues )
   */

  SOC_PB_FLD_SET(regs->iqm.cp_queues_range_reg.cp_qnum_high, fld_val, 20, exit);

  if ( info->q_set == SOC_PB_CNM_Q_SET_8_CPS)
  {
    fld_val=1;
  }
  else
  {
    fld_val = 0;
  }
  
  SOC_PB_FLD_SET(regs->iqm.cp_configuration_reg.cp_eight_qs_set_mode, fld_val, 30, exit);

  SOC_PB_FLD_SET(regs->iqm.cp_configuration_reg.cp_qnum_odd_sel, 0, 40, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_q_mapping_set_unsafe()", 0, 0);
}

uint32
  soc_pb_cnm_q_mapping_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO       *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_Q_MAPPING_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_Q_MAPPING_INFO, info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_q_mapping_set_verify()", 0, 0);
}

uint32
  soc_pb_cnm_q_mapping_get_verify(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_Q_MAPPING_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_q_mapping_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the mapping from the pair (destination, traffic
 *     class) to the CP Queue.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_q_mapping_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_Q_MAPPING_INFO       *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_Q_MAPPING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_CNM_Q_MAPPING_INFO_clear(info);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->iqm.cp_queues_range_reg.cp_qnum_low, fld_val, 10, exit);

  info->q_base = fld_val;

  SOC_PB_FLD_GET(regs->iqm.cp_queues_range_reg.cp_qnum_high, fld_val, 20, exit);

  info->nof_queues = fld_val - info->q_base + 1;

  SOC_PB_FLD_GET(regs->iqm.cp_configuration_reg.cp_eight_qs_set_mode, fld_val, 30, exit);

  if (fld_val)
  {
    info->q_set = SOC_PB_CNM_Q_SET_8_CPS;
  }
  else
  {
    info->q_set = SOC_PB_CNM_Q_SET_4_CPS;
  }
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_q_mapping_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set parameters of the congestion test.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_congestion_test_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CONGESTION_TEST_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();
  
  fld_val = SOC_SAND_BOOL2NUM(info->is_mc_also);

  SOC_PB_FLD_SET(regs->iqm.cp_configuration_reg.en_cnm_mc_da , fld_val, 10, exit);

  fld_val = SOC_SAND_BOOL2NUM(info->is_ingr_rep_also);

  SOC_PB_FLD_SET(regs->iqm.cp_configuration_reg.en_cnm_ing_rep, fld_val, 20, exit);

  fld_val = SOC_SAND_BOOL2NUM(info->is_snoop_also);

  SOC_PB_FLD_SET(regs->iqm.cp_configuration_reg.en_cnm_snp, fld_val, 30, exit);

  fld_val = SOC_SAND_BOOL2NUM(info->is_mirr_also);

  SOC_PB_FLD_SET(regs->iqm.cp_configuration_reg.en_cnm_mirr, fld_val, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_congestion_test_set_unsafe()", 0, 0);
}

uint32
  soc_pb_cnm_congestion_test_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CONGESTION_TEST_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_CONGESTION_TEST_INFO, info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_congestion_test_set_verify()", 0, 0);
}

uint32
  soc_pb_cnm_congestion_test_get_verify(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CONGESTION_TEST_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_congestion_test_get_verify()", 0, 0);
}

/*********************************************************************
*     Set parameters of the congestion test.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_congestion_test_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_CNM_CONGESTION_TEST_INFO *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CONGESTION_TEST_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  

  SOC_PB_FLD_GET(regs->iqm.cp_configuration_reg.en_cnm_mc_da , fld_val, 10, exit);

  info->is_mc_also = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_GET(regs->iqm.cp_configuration_reg.en_cnm_ing_rep, fld_val, 20, exit);

  info->is_ingr_rep_also = SOC_SAND_NUM2BOOL(fld_val);
  
  SOC_PB_FLD_GET(regs->iqm.cp_configuration_reg.en_cnm_snp, fld_val, 30, exit);

  info->is_snoop_also = SOC_SAND_NUM2BOOL(fld_val);
  
  SOC_PB_FLD_GET(regs->iqm.cp_configuration_reg.en_cnm_mirr, fld_val, 40, exit);

  info->is_mirr_also = SOC_SAND_NUM2BOOL(fld_val);

 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_congestion_test_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Configure an entry into the Congestion Point Profile
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cp_profile_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO      *info
  )
{
  uint32
    ind,
    entry_offset = profile_ndx,
    res = SOC_SAND_OK;
  
  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_PROFILE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_iqm_cnm_parameters_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.cp_fb_max_val = info->max_neg_fb_value;

  if (info->is_sampling_th_random)
  {
    tbl_data.cp_fixed_sample_base = 0;
  }
  else
  {
    tbl_data.cp_fixed_sample_base = 1;
  }

  tbl_data.cp_qeq = info->q_eq;

  tbl_data.cp_quant_div = info->quant_div;

  tbl_data.cp_w = info->cpw_power;

  for (ind = 0; ind < SOC_PB_CNM_NOF_SAMPLING_BASES ; ind++)
  {
    tbl_data.cp_sample_base[ind] =  info->sampling_base[ind];
  }
  
 

  res = soc_pb_iqm_cnm_parameters_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_profile_set_unsafe()", profile_ndx, 0);
}

uint32
  soc_pb_cnm_cp_profile_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PB_PROFILE_NDX_MAX, SOC_PB_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_CP_PROFILE_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_profile_set_verify()", profile_ndx, 0);
}

uint32
  soc_pb_cnm_cp_profile_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PB_PROFILE_NDX_MAX, SOC_PB_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_profile_get_verify()", profile_ndx, 0);
}

/*********************************************************************
*     Configure an entry into the Congestion Point Profile
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cp_profile_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CP_PROFILE_INFO      *info
  )
{
  uint32
    ind,
    entry_offset = profile_ndx,
    res = SOC_SAND_OK;
  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA
    tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CP_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_CNM_CP_PROFILE_INFO_clear(info);


  res = soc_pb_iqm_cnm_parameters_table_tbl_get_unsafe(
    unit,
    entry_offset,
    &tbl_data
    );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->max_neg_fb_value = tbl_data.cp_fb_max_val;

  if (tbl_data.cp_fixed_sample_base)
  {
    info->is_sampling_th_random = 0;
  }
  else
  {
    info->is_sampling_th_random = 1;
  }

  info->q_eq = tbl_data.cp_qeq;

  info->quant_div = tbl_data.cp_quant_div;

  info->cpw_power = tbl_data.cp_w;

  for (ind = 0; ind < SOC_PB_CNM_NOF_SAMPLING_BASES ; ind++)
  {
    info->sampling_base[ind] = tbl_data.cp_sample_base[ind];
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cp_profile_get_unsafe()", profile_ndx, 0);
}

/*********************************************************************
*     Configure an entry into the Congestion Point Profile
 *     Table for the sampling mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_sampling_profile_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  uint32                    sampling_rate
  )
{
  

  uint32
    ind,
    entry_offset = profile_ndx,
    res = SOC_SAND_OK;

  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_SAMPLING_PROFILE_SET_UNSAFE);



  res = soc_pb_iqm_cnm_parameters_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 

  for (ind = 0; ind < SOC_PB_CNM_NOF_SAMPLING_BASES ; ind++)
  {
    tbl_data.cp_sample_base[ind] = sampling_rate;
  }

  res = soc_pb_iqm_cnm_parameters_table_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_sampling_profile_set_unsafe()", profile_ndx, 0);
}

uint32
  soc_pb_cnm_sampling_profile_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_IN  uint32                    sampling_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_SAMPLING_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PB_PROFILE_NDX_MAX, SOC_PB_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(sampling_rate, SOC_PB_SAMPLING_RATE_MAX, SOC_PB_SAMPLING_RATE_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_sampling_profile_set_verify()", profile_ndx, 0);
}

uint32
  soc_pb_cnm_sampling_profile_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_SAMPLING_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(profile_ndx, SOC_PB_PROFILE_NDX_MAX, SOC_PB_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_sampling_profile_get_verify()", profile_ndx, 0);
}

/*********************************************************************
*     Configure an entry into the Congestion Point Profile
 *     Table for the sampling mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_sampling_profile_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   profile_ndx,
    SOC_SAND_OUT uint32                    *sampling_rate
  )
{
  uint32
    ind = 0,
    entry_offset = profile_ndx,
    res = SOC_SAND_OK;

  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_SAMPLING_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sampling_rate);

  res = soc_pb_iqm_cnm_parameters_table_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *sampling_rate = tbl_data.cp_sample_base[ind];
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_sampling_profile_get_unsafe()", profile_ndx, 0);
}

STATIC
  uint32
    soc_pb_cnm_cpq_tbl_set_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   cpq_ndx,
      SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
    )
{
 
  uint32
    fld_val,
    entry_offset = 0,
    res = SOC_SAND_OK;
  
  SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA
    tbl_data;
  SOC_PETRA_REGS
    *regs;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_PP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->iqm.cp_queues_range_reg.cp_qnum_low, fld_val, 5, exit);
  entry_offset = cpq_ndx;

  res = soc_pb_iqm_cnm_descriptor_static_tbl_get_unsafe(
    unit,
    entry_offset,
    &tbl_data
    );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  tbl_data.cp_id = cpq_ndx;

  tbl_data.cp_class = info->profile;

  tbl_data.cp_enable = info->is_cp_enabled;

  res = soc_pb_iqm_cnm_descriptor_static_tbl_set_unsafe(
    unit,
    entry_offset,
    &tbl_data
    );

  
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
                                         
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_cnm_cpq_tbl_set_unsafe()", cpq_ndx, 0);


}

STATIC
  uint32
    soc_pb_cnm_cpq_tbl_get_unsafe(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                   cpq_ndx,
      SOC_SAND_OUT  SOC_PB_CNM_CPQ_INFO             *info
      )
{
  uint32
    fld_val = cpq_ndx,
    res = SOC_SAND_OK;

  SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_PP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_CNM_CPQ_INFO_clear(info);


  res = soc_pb_iqm_cnm_descriptor_static_tbl_get_unsafe(
          unit,
          fld_val,
          &tbl_data
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->profile = tbl_data.cp_class;

  info->is_cp_enabled = SOC_SAND_NUM2BOOL(tbl_data.cp_enable);

 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_cnm_cpq_tbl_set_unsafe()", cpq_ndx, 0);
}


/*********************************************************************
*     Configure the CP Queue attributes into an entry of the
*     Congestion Point Table.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cpq_pp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_PP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cpq_tbl_set_unsafe(
          unit,
          cpq_ndx,
          info
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_pp_set_unsafe()", cpq_ndx, 0);
}

uint32
  soc_pb_cnm_cpq_pp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_PP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpq_ndx, SOC_PB_CPQ_NDX_MAX, SOC_PB_CPQ_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_CPQ_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_pp_set_verify()", cpq_ndx, 0);
}

uint32
  soc_pb_cnm_cpq_pp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_PP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpq_ndx, SOC_PB_CPQ_NDX_MAX, SOC_PB_CPQ_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_pp_get_verify()", cpq_ndx, 0);
}

/*********************************************************************
*     Configure the CP Queue attributes into an entry of the
 *     Congestion Point Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cpq_pp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32

    entry_offset = cpq_ndx,
    res = SOC_SAND_OK;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_PP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_CNM_CPQ_INFO_clear(info);

  res = soc_pb_cnm_cpq_tbl_get_unsafe(
    unit,
    entry_offset,
    info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_pp_get_unsafe()", cpq_ndx, 0);
}

/*********************************************************************
*     Configure the CP Queue attributes into an entry of the
 *     Congestion Point Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cpq_sampling_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_SAMPLING_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_cnm_cpq_tbl_set_unsafe(
          unit,
          cpq_ndx,
          info
          );

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_sampling_set_unsafe()", cpq_ndx, 0);
}

uint32
  soc_pb_cnm_cpq_sampling_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_SAMPLING_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpq_ndx, SOC_PB_CPQ_NDX_MAX, SOC_PB_CPQ_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_CPQ_INFO, info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_sampling_set_verify()", cpq_ndx, 0);
}

uint32
  soc_pb_cnm_cpq_sampling_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_SAMPLING_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpq_ndx, SOC_PB_CPQ_NDX_MAX, SOC_PB_CPQ_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_sampling_get_verify()", cpq_ndx, 0);
}

/*********************************************************************
*     Configure the CP Queue attributes into an entry of the
 *     Congestion Point Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_cpq_sampling_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   cpq_ndx,
    SOC_SAND_OUT SOC_PB_CNM_CPQ_INFO             *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_CPQ_SAMPLING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_CNM_CPQ_INFO_clear(info);

  res = soc_pb_cnm_cpq_tbl_get_unsafe(
          unit,
          cpq_ndx,
          info
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_cpq_sampling_get_unsafe()", cpq_ndx, 0);
}

/*********************************************************************
*     Set the timer delay of the CNM Intercept functionality
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_intercept_timer_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                     delay
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_INTERCEPT_TIMER_SET_UNSAFE);

  regs = soc_petra_regs();

  fld_val = delay;

  SOC_PB_FLD_SET(regs->egq.cnm_timer_granularity_reg.cnm_timer_granularity, fld_val, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_intercept_timer_set_unsafe()", 0, 0);
}

uint32
  soc_pb_cnm_intercept_timer_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                     delay
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_INTERCEPT_TIMER_SET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_intercept_timer_set_verify()", 0, 0);
}

uint32
  soc_pb_cnm_intercept_timer_get_verify(
    SOC_SAND_IN  int                   unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_INTERCEPT_TIMER_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_intercept_timer_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the timer delay of the CNM Intercept functionality
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnm_intercept_timer_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *delay
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNM_INTERCEPT_TIMER_GET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->egq.cnm_timer_granularity_reg.cnm_timer_granularity, fld_val, 10, exit);

  *delay = fld_val;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnm_intercept_timer_get_unsafe()", 0, 0);
}

uint32
  SOC_PB_CNM_Q_MAPPING_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_Q_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->q_set, SOC_PB_CNM_Q_MAPPING_INFO_Q_SET_MAX, SOC_PB_CNM_Q_SET_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_Q_MAPPING_INFO_verify()",0,0);
}

uint32
  SOC_PB_CNM_CONGESTION_TEST_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_CONGESTION_TEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_CONGESTION_TEST_INFO_verify()",0,0);
}

uint32
  SOC_PB_CNM_CP_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_CP_PROFILE_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_CNM_NOF_SAMPLING_BASES; ++ind)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_CP_PROFILE_INFO_verify()",0,0);
}

uint32
  SOC_PB_CNM_CPQ_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_CPQ_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->profile, SOC_PB_CNM_CPQ_INFO_PROFILE_MAX, SOC_PB_CNM_PROFILE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_CPQ_INFO_verify()",0,0);
}

uint32
  SOC_PB_CNM_PPH_verify(
    SOC_SAND_IN  SOC_PB_CNM_PPH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ing_vlan_edit_cmd, SOC_PB_CNM_PPH_ING_VLAN_EDIT_CMD_MAX, SOC_PB_CNM_ING_VLAN_EDIT_CMD_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_PPH_verify()",0,0);
}

uint32
  SOC_PB_CNM_PDU_verify(
    SOC_SAND_IN  SOC_PB_CNM_PDU *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->version, SOC_PB_CNM_PDU_VERSION_MAX, SOC_PB_CNM_VERSION_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->res_v, SOC_PB_CNM_PDU_RES_V_MAX, SOC_PB_CNM_RES_V_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_PDU_verify()",0,0);
}

uint32
  SOC_PB_CNM_PETRA_B_PP_verify(
    SOC_SAND_IN  SOC_PB_CNM_PETRA_B_PP *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_PPH, &(info->pph), 11, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_PDU, &(info->pdu), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_PETRA_B_PP_verify()",0,0);
}

uint32
  SOC_PB_CNM_PACKET_verify(
    SOC_SAND_IN  SOC_PB_CNM_PACKET *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  if (info->dest_tm_port != SOC_PB_CNM_DEST_TM_PORT_AS_INCOMING)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->dest_tm_port, SOC_PETRA_MAX_FAP_PORT_ID,
      SOC_PETRA_FAP_PORT_ID_INVALID_ERR,5,exit
    );
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tc, SOC_PB_CNM_PACKET_TC_MAX, SOC_PB_CNM_TC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cp_id_4_msb, SOC_PB_CNM_PACKET_CP_ID_4_MSB_MAX, SOC_PB_CNM_CP_ID_4_MSB_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_PACKET_verify()",0,0);
}

uint32
  SOC_PB_CNM_CP_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNM_CP_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pkt_gen_mode, SOC_PB_CNM_CP_INFO_PKT_GEN_MODE_MAX, SOC_PB_CNM_PKT_GEN_MODE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_PACKET, &(info->pckt), 12, exit);

  if(info->pkt_gen_mode == SOC_PB_CNM_GEN_MODE_PETRA_B_PP)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_CNM_PETRA_B_PP, &(info->pp), 13, exit);
  }
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNM_CP_INFO_verify()",0,0);
}
#if SOC_PB_DEBUG_IS_LVL1
#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

