#include <shared/bsl.h>
#include <soc/mcm/memregs.h> 
#if defined(BCM_88675_A0)
/* $Id: jer_ingress_traffic_mgmt.c,v 1.85 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h> 
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/JER/jer_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_general.h> 
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/JER/jer_nif.h>
#include <shared/swstate/access/sw_state_access.h>

#define JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE 4

/* (2^22 - 1) */
#define JER_ITM_VSQ_FC_BD_SIZE_MAX              SOC_TMC_ITM_VSQ_FC_BD_SIZE_MAX
#define JER_ITM_VSQ_GRNT_BD_SIZE_MAX            SOC_TMC_ITM_VSQ_GRNT_BD_SIZE_MAX

STATIC uint32
  jer_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
     SOC_SAND_IN int                                                     unit,
     SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO                            *wred_param,
     SOC_SAND_INOUT SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA *tbl_data
  );

STATIC uint32
  jer_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
     SOC_SAND_IN int                                                     unit,
     SOC_SAND_IN  SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  *tbl_data,
     SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO                            *wred_param
  );

STATIC uint32
  jer_itm_admit_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              admt_tst_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  );
/*********************************************************************
* NAME:
*     jer_itm_init
* FUNCTION:
*     Initialization of the Arad blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  jer_itm_init(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_E_NONE;
  uint32
    pdm_nof_entries,
	mem_excess_size,
    idx;
  uint32
	  data = 0;
  SOC_TMC_ITM_GLOB_RCS_DROP_TH
    glbl_drop, glbl_drop_exact;
  SOC_TMC_ITM_GLOB_RCS_FC_TH
    glbl_fc, glbl_fc_exact;
  SOC_TMC_ITM_VSQ_GROUP
    vsq_group;
  SOC_TMC_ITM_VSQ_FC_INFO
    vsq_fc_info,
    exact_vsq_fc_info;
  int 
    pool_id;
  ARAD_MGMT_INIT *init = &(SOC_DPP_CONFIG(unit)->arad->init);

  SOCDNX_INIT_FUNC_DEFS;
  
  /*res = arad_itm_regs_init(unit);*/
  SOC_DPP_CONFIG(unit)->jer->tm.nof_remote_faps_with_remote_credit_value = 0;

  soc_mem_field32_set(unit, IPST_CRVSm, &data, CR_VAL_BMPf, 0x0); 
  res = arad_fill_table_with_entry(unit, IPST_CRVSm, MEM_BLOCK_ANY, &data);
  SOCDNX_IF_ERR_EXIT(res);

  data = 0;
  res = READ_IPST_CREDIT_ARBITER_CONFIGr(unit, &data); 
  SOCDNX_IF_ERR_EXIT(res);

  soc_reg_field_set(unit, IPST_CREDIT_ARBITER_CONFIGr, &data, CR_VAL_SEL_ENABLEf, TRUE);
  res = WRITE_IPST_CREDIT_ARBITER_CONFIGr(unit, data); 
  SOCDNX_IF_ERR_EXIT(res);


  SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop);
  SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop_exact);
  SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc);
  SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc_exact);
  SOC_TMC_ITM_VSQ_FC_INFO_clear(&vsq_fc_info);
  SOC_TMC_ITM_VSQ_FC_INFO_clear(&exact_vsq_fc_info);

  /* If we are in ocb_only mode, we need to set all ITM FC registers to 0*/
  if ((init->ocb.ocb_enable != OCB_ONLY) && ((init->ocb.ocb_enable != OCB_ENABLED) || (init->dram.nof_drams != 0))){
      glbl_fc.bdbs.hp.set       = 256;
      glbl_fc.bdbs.hp.clear     = 1024;
      glbl_fc.bdbs.lp.set       = 768;
      glbl_fc.bdbs.lp.clear     = 1500;
      glbl_fc.unicast.hp.set    = 672;
      glbl_fc.unicast.hp.clear  = 1344;
      glbl_fc.unicast.lp.set    = 10752;
      glbl_fc.unicast.lp.clear  = 43008;
      glbl_fc.full_mc.hp.set    = 416;
      glbl_fc.full_mc.hp.clear  = 832;
      glbl_fc.full_mc.lp.set    = 672;
      glbl_fc.full_mc.lp.clear  = 1344;
  }
  
  /*res = arad_itm_glob_rcs_fc_set_unsafe(unit, &glbl_fc, &glbl_fc_exact);*/

  glbl_drop.bdbs[0].set       = 128;
  glbl_drop.bdbs[0].clear     = 512;
  for (idx = 1; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.bdbs[idx].set   = 256;
    glbl_drop.bdbs[idx].clear = 1024;
  }

  
  pdm_nof_entries = SOC_DPP_DEFS_GET(unit, pdm_size);
  for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
  {
    
    glbl_drop.bds[idx].set   = 1008 * (pdm_nof_entries / 1024); 
    glbl_drop.bds[idx].clear =  928 * (pdm_nof_entries / 1024); 
  }

  /* If we are in ocb_only mode, we need to set all IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS registers to 0*/
  if ((init->ocb.ocb_enable != OCB_ONLY) && ((init->ocb.ocb_enable != OCB_ENABLED) || (init->dram.nof_drams != 0))){

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.unicast[idx].set   = 496;
        glbl_drop.unicast[idx].clear = 672;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.full_mc[idx].set   = 160;
        glbl_drop.full_mc[idx].clear = 320;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.mini_mc[idx].set   = 160;
        glbl_drop.mini_mc[idx].clear = 320;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
          glbl_drop.mem_excess[idx].set   = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX;
          glbl_drop.mem_excess[idx].clear = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX - (ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX/8);
      }
  } else {
      mem_excess_size = (((SOC_DPP_DEFS_GET(unit, ocb_memory_size) * 1024 * 1024 /* 8mb*/) / 8 /* 8mB*/) / 128 /* 128 bytes units */);
      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++) {
          glbl_drop.mem_excess[idx].set   = mem_excess_size;
          glbl_drop.mem_excess[idx].clear = mem_excess_size - (mem_excess_size / 8);
      }
  }
  
  /*res = arad_itm_glob_rcs_drop_set_unsafe(unit, &glbl_drop, &glbl_drop_exact);*/
  

  /* Flow control, by default set maximal threshold */
  vsq_fc_info.bd_size_fc.set = JER_ITM_VSQ_FC_BD_SIZE_MAX;
  vsq_fc_info.bd_size_fc.clear = JER_ITM_VSQ_FC_BD_SIZE_MAX;

  for (vsq_group = 0; vsq_group < SOC_TMC_NOF_VSQ_GROUPS; vsq_group++) {
      for (pool_id = 0; pool_id < ((vsq_group == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) ? SOC_TMC_ITM_NOF_RSRC_POOLS : 1); pool_id++) {
           res = jer_itm_vsq_fc_set(unit, vsq_group, 0 /* Default VSQ rate class */ , pool_id, &vsq_fc_info, &exact_vsq_fc_info);
           SOCDNX_IF_ERR_EXIT(res);
      }
  }
 
  /* set the total of the gaurenteed VOQ resorce */
  
  SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource.total = pdm_nof_entries; 
  res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE, &(SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource)); /* update warm boot data */
  SOCDNX_IF_ERR_EXIT(res);

  res = jer_itm_setup_dp_map(unit);
  SOCDNX_IF_ERR_EXIT(res);

exit: 
  SOCDNX_FUNC_RETURN; 
}

/*********************************************************************
*     Set ECN as enabled or disabled for the device
*********************************************************************/
uint32
  jer_itm_enable_ecn_set(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled /* ECN will be enabled/disabled for non zero/zero values */
  )
{
  uint32 res, enable_bit = enabled ? 1 : 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit_semaphore, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ECN_REMARKr, SOC_CORE_ALL, 0, ECN_ENABLE_SYSTEMf,  enable_bit)); /* Enables marking congestion on ECN capable packets */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit_semaphore, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ECN_REMARKr, SOC_CORE_ALL, 0, DSCP_AND_ECN_ENf,  enable_bit)); /* Ethernet remark profile MSB are taken from FTMH.CNI and FTMH.ECN-capable */

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_itm_enable_ecn_set()", unit, enabled);
}

/*********************************************************************
*     Return if ECN is enabled for the device
*********************************************************************/
uint32
  jer_itm_enable_ecn_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled /* will return non zero if /ECN is enabled */
  )
{
  uint32 res, val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(enabled);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit_semaphore, ARAD_REG_ACCESS_ERR, READ_EPNI_ECN_REMARKr(unit, REG_PORT_ANY, &val));
  *enabled = soc_reg_field_get(unit, EPNI_ECN_REMARKr, val, ECN_ENABLE_SYSTEMf);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_itm_enable_ecn_get()", unit, 0);
}

/********************************************************************* 
*     Sets ingress shaping configuration. This includes 
*     ingress shaping queues range, and credit generation 
*     configuration. 
*     Details: in the H file. (search for prototype) 
*********************************************************************/ 
uint32 
  jer_itm_ingress_shape_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  ) 
{ 
  uint32 
    rate_internal, 
    fld_val, 
    res, 
    dsp_pp, 
    base_port_tc, 
    nof_prio,
    flags; 
  soc_port_t 
    port; 
  soc_pbmp_t 
    ports_bm; 
  int 
    tmp_core = SOC_CORE_ALL, 
    core_id = SOC_CORE_ALL;
  SOCDNX_INIT_FUNC_DEFS; 
  SOCDNX_NULL_CHECK(info); 

  if (((core < 0) || (core > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && (core != SOC_CORE_ALL)) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
  if (core == SOC_CORE_ALL) {
      core_id = 0;
  } else {
      core_id = core;
  }
  if (core_id == 0) { 
      /*If we get SOC_CORE_ALL then we give values from the core 0*/ 
      res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_LOW_0f, &info->q_range.q_num_low);  
      SOCDNX_IF_ERR_EXIT(res); 
 
      res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_0f, &info->q_range.q_num_high); 
      SOCDNX_IF_ERR_EXIT(res); 
  } else if (core_id == 1) { 
      res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_ISP_1r, REG_PORT_ANY, 0, ISP_QNUM_LOW_1f, &info->q_range.q_num_low);  
      SOCDNX_IF_ERR_EXIT(res); 
 
      res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_ISP_1r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_1f, &info->q_range.q_num_high); 
      SOCDNX_IF_ERR_EXIT(res); 
  } else { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
  res = soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, core_id, 0, INGRESS_SHAPING_PORT_IDf, &base_port_tc);  
  SOCDNX_IF_ERR_EXIT(res); 
 
  /* Look for match base q pair */ 
  res = soc_port_sw_db_valid_ports_core_get(unit, core_id, 0, &ports_bm);
  SOCDNX_IF_ERR_EXIT(res); 
 
  SOC_PBMP_ITER(ports_bm, port)  
  {
      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
      if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags))) {

          res = soc_port_sw_db_local_to_tm_port_get(unit, port, &dsp_pp, &tmp_core); 
          SOCDNX_IF_ERR_EXIT(res); 
     
          res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, dsp_pp, &nof_prio); 
          SOCDNX_IF_ERR_EXIT(res); 
          if (core_id != tmp_core) {
              SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
          }
          if (base_port_tc == nof_prio) { 
              info->sch_port = dsp_pp; 
          }
      }
  } 
   
  res = soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, core_id, 0, INGRESS_SHAPING_ENABLEf, &fld_val); 
  SOCDNX_IF_ERR_EXIT(res); 
 
  info->enable = SOC_SAND_NUM2BOOL(fld_val); 
 
  res = soc_reg_above_64_field32_read(unit, IPS_INGRESS_SHAPE_SCHEDULER_CONFIGr, core_id, 0, ISS_MAX_CR_RATEf, &rate_internal); 
  SOCDNX_IF_ERR_EXIT(res); 
 
  res = arad_intern_clock2rate( 
          unit, 
          rate_internal, 
          TRUE, /* is_for_ips */ 
          &(info->rate) 
        ); 
  SOCDNX_SAND_IF_ERR_EXIT(res); 
 
exit: 
  SOCDNX_FUNC_RETURN; 
} 
 
/********************************************************************* 
*     Sets ingress shaping configuration. This includes 
*     ingress shaping queues range, and credit generation 
*     configuration. 
*     Details: in the H file. (search for prototype) 
*********************************************************************/ 
uint32 
  jer_itm_ingress_shape_set( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  )  
{ 
    uint32 
        fld_val, 
        q_hi, 
        q_low, 
        q_base, 
        ihp_reg_low, 
        ihp_reg_hi, 
        base_port_tc, 
        rate_internal, 
        res; 
    SOCDNX_INIT_FUNC_DEFS; 
    SOCDNX_NULL_CHECK(info); 
    if ((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 
 
    res = soc_reg_above_64_field32_read(unit, IRR_FLOW_BASE_QUEUEr, core_id, 0, FLOW_BASE_QUEUE_Nf, &q_base); 
    SOCDNX_IF_ERR_EXIT(res); 
 
    if (info->enable) { 
        q_low = info->q_range.q_num_low; 
        q_hi  = info->q_range.q_num_high; 
    } else { 
        /* 
         *  Set 'hi' below 'low'. 
         *  Note: there is an assumption here that: For 'Add' mode, Base queue is smaller then ARAD_MAX_QUEUE_ID - 1 
         */ 
        q_low = q_base + 2; 
        q_hi = q_low - 1; 
    } 
    /* 
    *  ECI 
    */ 
    if (core_id == 0) { 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_LOW_0f, q_low);  
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_0f, q_hi); 
        SOCDNX_IF_ERR_EXIT(res); 
    } 
    if (core_id == 0) { 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_1r, REG_PORT_ANY, 0, ISP_QNUM_LOW_1f, q_low);  
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_1r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_1f, q_hi); 
        SOCDNX_IF_ERR_EXIT(res);        
    } 
    /* 
    *  IHP 
    */ 
    ihp_reg_low = q_low - q_base; 
    ihp_reg_hi  = q_hi  - q_base; 
 
    res = arad_parser_ingress_shape_state_set( 
            unit, 
            info->enable, 
            ihp_reg_low, 
            ihp_reg_hi 
          ); 
    SOCDNX_SAND_IF_ERR_EXIT(res); 
  fld_val = info->enable ? 0x1 : 0x0;
    res = soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, core_id, 0, INGRESS_SHAPING_ENABLEf, fld_val); 
    SOCDNX_IF_ERR_EXIT(res); 
 
    if (info->enable) 
    { 
        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, info->sch_port, &base_port_tc); 
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, core_id, 0, INGRESS_SHAPING_PORT_IDf, base_port_tc); 
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = arad_intern_rate2clock(unit, info->rate, TRUE, /* is_for_ips */&rate_internal); 
        SOCDNX_SAND_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, IPS_INGRESS_SHAPE_SCHEDULER_CONFIGr, core_id, 0, ISS_MAX_CR_RATEf, rate_internal); 
        SOCDNX_IF_ERR_EXIT(res); 
    } 
 
exit: 
  SOCDNX_FUNC_RETURN; 
} 
/********************************************************************* 
*     Defines packet queues categories - in contiguous blocks. 
*     IQM queues are divided to 4 categories in contiguous 
*     blocks. Category-4 from 'category-end-3' till the last 
*     queue (32K). 
*     Details: in the H file. (search for prototype) 
*********************************************************************/ 
STATIC uint32 
  _jer_itm_category_rngs_verify( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
    SOCDNX_INIT_FUNC_DEFS; 
    SOCDNX_NULL_CHECK(info);    
 
  if ((info->vsq_ctgry0_end > info->vsq_ctgry1_end) || (info->vsq_ctgry1_end > info->vsq_ctgry2_end) || (info->vsq_ctgry0_end > info->vsq_ctgry2_end)) { 
    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
  /* coverity[unsigned_compare : FALSE] */ 
  if ((info->vsq_ctgry0_end < 0) || (info->vsq_ctgry0_end > SOC_DPP_DEFS_GET(unit, nof_queues)-1)) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
  /* coverity[unsigned_compare : FALSE] */ 
  if ((info->vsq_ctgry1_end < 0) || (info->vsq_ctgry1_end > (SOC_DPP_DEFS_GET(unit, nof_queues)-1))) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
  /* coverity[unsigned_compare : FALSE] */ 
  if ((info->vsq_ctgry2_end < 0) || (info->vsq_ctgry2_end > (SOC_DPP_DEFS_GET(unit, nof_queues)-1))) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
exit: 
  SOCDNX_FUNC_RETURN; 
} 
 
 
/********************************************************************* 
*     Defines packet queues categories - in contiguous blocks. 
*     IQM queues are divided to 4 categories in contiguous 
*     blocks. Category-4 from 'category-end-3' till the last 
*     queue (32K). 
*     Details: in the H file. (search for prototype) 
*********************************************************************/ 
uint32 
  jer_itm_category_rngs_set( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
  uint32  
      res; 
  uint64  
      reg_val; 
  SOCDNX_INIT_FUNC_DEFS; 
  SOCDNX_NULL_CHECK(info); 
  if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
  res = _jer_itm_category_rngs_verify(unit, core_id, info); 
  SOCDNX_IF_ERR_EXIT(res); 
 
  res = READ_IQM_PACKET_QUEUES_CATEGORIESr(unit, core_id, &reg_val); 
  SOCDNX_IF_ERR_EXIT(res); 
 
  soc_reg64_field32_set(unit, IQM_PACKET_QUEUES_CATEGORIESr, &reg_val, TOP_PKT_Q_CAT_0f, info->vsq_ctgry0_end); 
  soc_reg64_field32_set(unit, IQM_PACKET_QUEUES_CATEGORIESr, &reg_val, TOP_PKT_Q_CAT_1f, info->vsq_ctgry1_end); 
  soc_reg64_field32_set(unit, IQM_PACKET_QUEUES_CATEGORIESr, &reg_val, TOP_PKT_Q_CAT_2f, info->vsq_ctgry2_end); 
 
  res = WRITE_IQM_PACKET_QUEUES_CATEGORIESr(unit, core_id, reg_val); 
  SOCDNX_IF_ERR_EXIT(res); 
exit: 
  SOCDNX_FUNC_RETURN; 
} 
 
 
/********************************************************************* 
*     Defines packet queues categories - in contiguous blocks. 
*     IQM queues are divided to 4 categories in contiguous 
*     blocks. Category-4 from 'category-end-3' till the last 
*     queue (32K). 
*     Details: in the H file. (search for prototype) 
*********************************************************************/ 
uint32 
  jer_itm_category_rngs_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
    uint32  
        res; 
    uint64  
        reg_val; 
    SOCDNX_INIT_FUNC_DEFS; 
    SOCDNX_NULL_CHECK(info); 
    if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 
 
    res = READ_IQM_PACKET_QUEUES_CATEGORIESr(unit, core_id, &reg_val); 
    SOCDNX_IF_ERR_EXIT(res); 
 
    info->vsq_ctgry0_end = soc_reg64_field32_get(unit, IQM_PACKET_QUEUES_CATEGORIESr, reg_val, TOP_PKT_Q_CAT_0f); 
    info->vsq_ctgry1_end = soc_reg64_field32_get(unit, IQM_PACKET_QUEUES_CATEGORIESr, reg_val, TOP_PKT_Q_CAT_1f); 
    info->vsq_ctgry2_end = soc_reg64_field32_get(unit, IQM_PACKET_QUEUES_CATEGORIESr, reg_val, TOP_PKT_Q_CAT_2f); 
 
exit: 
  SOCDNX_FUNC_RETURN; 
} 
/*********************************************************************
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_itm_admit_test_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = jer_itm_admit_test_tmplt_verify(unit, core_id, admt_tst_ndx, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = arad_itm_convert_admit_one_test_tmplt_to_u32(info->test_a, &test_a_in_sand_u32);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  test_a_index = admt_tst_ndx;
  res = soc_reg_above_64_field32_modify(unit, IQM_REJECT_ADMISSIONr, core_id, 0, RJCT_TMPLTA_SET_0f + test_a_index,  test_a_in_sand_u32);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_itm_convert_admit_one_test_tmplt_to_u32(info->test_b, &test_b_in_sand_u32);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  test_b_index = admt_tst_ndx;
  res = soc_reg_above_64_field32_modify(unit, IQM_REJECT_ADMISSIONr, core_id, 0, RJCT_TMPLTB_SET_0f + test_b_index,  test_b_in_sand_u32);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_set_unsafe()",0,0);
}

/*********************************************************************
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  jer_itm_admit_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              admt_tst_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (admt_tst_ndx > (ARAD_ITM_ADMIT_TSTS_LAST-1))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_ADMT_TEST_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 1, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_verify()",0,0);
}

/*********************************************************************
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_itm_admit_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              admt_tst_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_GET_UNSAFE);
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 1, exit);
  }
  SOC_SAND_CHECK_NULL_INPUT(info);
  test_a_index = admt_tst_ndx;
  res = soc_reg_above_64_field32_read(unit, IQM_REJECT_ADMISSIONr, core_id, 0, RJCT_TMPLTA_SET_0f + test_a_index, &test_a_in_sand_u32);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_itm_convert_u32_to_admit_one_test_tmplt(test_a_in_sand_u32, &(info->test_a));
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  test_b_index = admt_tst_ndx;
  res = soc_reg_above_64_field32_read(unit, IQM_REJECT_ADMISSIONr, core_id, 0, RJCT_TMPLTB_SET_0f + test_b_index, &test_b_in_sand_u32);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_convert_u32_to_admit_one_test_tmplt(test_b_in_sand_u32, &(info->test_b));
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_get_unsafe()",0,0);
}


uint32
  jer_itm_vsq_wred_gen_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  )
{
    uint32 
        data;
    const soc_mem_t mem_arr_IQM_VSQ_PQWQ[SOC_TMC_NOF_VSQ_GROUPS] = 
        {IQM_VSQA_PQWQm, IQM_VSQB_PQWQm, IQM_VSQC_PQWQm, IQM_VSQD_PQWQm, IQM_VSQE_PQWQm, IQM_VSQF_PQWQm};
    uint32  
        res = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }   
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (info->exp_wq > (0x1 << soc_mem_field_length(unit, IQM_VSQA_PQWQm, VQ_WEIGHTf))) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        if (pool_id == 0) {
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHT_POOL_0f, info->exp_wq);
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_POOL_0_ENf, info->wred_en);
        } else if (pool_id == 1) {
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHT_POOL_1f, info->exp_wq);
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_POOL_1_ENf, info->wred_en);
        }
    } else {
        soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHTf, info->exp_wq);
        soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_ENf, info->wred_en);
    }
    res = soc_mem_write(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
exit: 
    SOCDNX_FUNC_RETURN; 
}

uint32
  jer_itm_vsq_wred_gen_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  )
{
    uint32 
        data;
    const soc_mem_t mem_arr_IQM_VSQ_PQWQ[SOC_TMC_NOF_VSQ_GROUPS] = 
        {IQM_VSQA_PQWQm, IQM_VSQB_PQWQm, IQM_VSQC_PQWQm, IQM_VSQD_PQWQm, IQM_VSQE_PQWQm, IQM_VSQF_PQWQm};
    uint32  
        res = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);

    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        if (pool_id == 0) {
            info->exp_wq = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHT_POOL_0f);
            info->wred_en = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_POOL_0_ENf);
        } else if (pool_id == 1) {
            info->exp_wq = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHT_POOL_1f);
            info->wred_en = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_POOL_1_ENf);
        }
    } else {
        info->exp_wq = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHTf);
        info->wred_en = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_ENf);
    }
exit: 
    SOCDNX_FUNC_RETURN; 
}

uint32
    jer_itm_vsq_wred_get(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  int                    pool_id,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info) 
{
    uint32  
        res = SOC_E_NONE;
    uint32
      entry_offset,
      array_index,
      data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        array_index = pool_id;
    } else {
        array_index = 0;
    }
  /* 
   * since IQM_VQPR_M[A-F] have the same fields in hardware, but currently in the description
   * IQM_VQPR_M[E-F] have different fields, we use mem_arr_IQM_VQPR_M[group_id] for memory
   * access and IQM_VQPR_MAm for filed access
   */ 
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 

    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    tbl_data.c3 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_3f);
    tbl_data.c1 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_1f);
    tbl_data.vq_wred_pckt_sz_ignr = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_PCKT_SZ_IGNRf);   
    tbl_data.max_avrg_th = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MAX_THf);
    tbl_data.min_avrg_th = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MIN_THf);
    info->wred_en = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_ENf));

    res = jer_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(unit, &tbl_data, info);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}

uint32
    jer_itm_vsq_wred_set(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  int                    pool_id,
        SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO *info,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *exact_info)
{
    uint32  
        res = SOC_E_NONE;
    uint32
      entry_offset,
      array_index,
      data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        array_index = pool_id;
    } else {
        array_index = 0;
    }
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 

    tbl_data.c3 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_3f);
    tbl_data.c1 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_1f);
    tbl_data.vq_wred_pckt_sz_ignr = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_PCKT_SZ_IGNRf);   
    tbl_data.max_avrg_th = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MAX_THf);
    tbl_data.min_avrg_th = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MIN_THf);

    res = jer_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(unit, info, &tbl_data);
    SOCDNX_IF_ERR_EXIT(res);

    res = jer_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(unit, &tbl_data, exact_info);
    SOCDNX_IF_ERR_EXIT(res);

    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_3f, tbl_data.c3);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_1f, tbl_data.c1);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_PCKT_SZ_IGNRf, tbl_data.vq_wred_pckt_sz_ignr);   
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MAX_THf, tbl_data.max_avrg_th);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MIN_THf, tbl_data.min_avrg_th);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_ENf, info->wred_en);

    res = soc_mem_array_write(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 
exit: 
    SOCDNX_FUNC_RETURN; 
}

STATIC uint32
  jer_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
    SOC_SAND_IN int                                                     unit,
    SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO                            *wred_param,
    SOC_SAND_INOUT SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA *tbl_data
  )
{
  uint32
    res,
    max_prob,
    calc,
    max_val_c1,
    max_avrg_th_16_byte;
  int32
    avrg_th_diff_wred_granular = 0;
  int32
    min_avrg_th_exact_wred_granular,
    max_avrg_th_exact_wred_granular;
  uint32
    trunced;
  SOC_SAND_U64
    u64_1,
    u64_2,
    u64_c2 = {{0}};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(wred_param); 
    SOCDNX_NULL_CHECK(tbl_data); 
    trunced = FALSE;

    /*
    * min_avrg_th
    */
    tbl_data->min_avrg_th = 0;
    max_avrg_th_16_byte = SOC_SAND_DIV_ROUND_UP(wred_param->min_avrg_th,SOC_TMC_ITM_WRED_GRANULARITY);
    res = arad_itm_man_exp_buffer_set(
          max_avrg_th_16_byte,
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit),
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          &(tbl_data->min_avrg_th),
          &min_avrg_th_exact_wred_granular
        );
    SOCDNX_SAND_IF_ERR_EXIT(res);
    /* min_avrg_th_exact *= ARAD_ITM_WRED_GRANULARITY; */
    /*
    * max_avrg_th
    */
    tbl_data->max_avrg_th = 0;
    res = arad_itm_man_exp_buffer_set(
            SOC_SAND_DIV_ROUND_UP(wred_param->max_avrg_th,SOC_TMC_ITM_WRED_GRANULARITY),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit),
            FALSE,
            &(tbl_data->max_avrg_th),
            &max_avrg_th_exact_wred_granular
            );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    /* max_avrg_th_exact *= SOC_TMC_ITM_WRED_GRANULARITY; */
    /*
    * max_packet_size
    */
    calc = wred_param->max_packet_size;
    if (calc > SOC_TMC_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC) {
        calc = SOC_TMC_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
    } 
    calc = SOC_SAND_DIV_ROUND_UP(calc, SOC_TMC_ITM_WRED_GRANULARITY);
    tbl_data->c3 = soc_sand_log2_round_up(calc);

    /*
    *  Packet size ignore
    */
    tbl_data->vq_wred_pckt_sz_ignr = wred_param->ignore_packet_size;
    /*
     * max_probability
     */
    max_prob = (wred_param->max_probability);
    if(max_prob>=100) {
        max_prob = 99;
    }
    /*
    * max_probability
    * C1 = ((2^32)/100)*max-prob / (max-th - min-th) in powers of 2
    */
    calc = SOC_TMC_WRED_NORMALIZE_FACTOR * max_prob;
    /*
     * We do not use 'SOC_SAND_DIV_ROUND' or 'SOC_SAND_DIV_ROUND_UP'
     * because at this point we might have in calc '((2^32)/100)*max-prob'
     * which can be very large number and the other dividers do ADD before
     * the division.
     */
    max_val_c1 = 31; /* soc_sand_log2_round_down(0xFFFFFFFF) */
    avrg_th_diff_wred_granular = (max_avrg_th_exact_wred_granular - min_avrg_th_exact_wred_granular);
    if(avrg_th_diff_wred_granular == 0) {
        tbl_data->c1 = max_val_c1;
    } else {
        calc = SOC_SAND_DIV_ROUND_DOWN(calc, avrg_th_diff_wred_granular);
        tbl_data->c1 = soc_sand_log2_round_down(calc);
    } 
    if(tbl_data->c1 < max_val_c1) {
        /*
         * Check if a bigger C1 gives closer result of the value we add.
         */
        uint32
            now     = 1 <<(tbl_data->c1),
            changed = 1 <<(tbl_data->c1+1),
            diff_with_now,
            diff_with_change;
        diff_with_change = changed-calc;

        diff_with_now    = calc-now;
        if( diff_with_change < diff_with_now) {
            tbl_data->c1 += 1;
        }
    }
    SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->c1, max_val_c1);
    if (max_avrg_th_16_byte > 0) {
        max_val_c1 = SOC_SAND_DIV_ROUND_DOWN(0xFFFFFFFF, max_avrg_th_16_byte);
        max_val_c1 = soc_sand_log2_round_down(max_val_c1);
        SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->c1, max_val_c1);
    }
    /*
    * max_probability
    * C2 = FACTOR * max-prob * min-th / (max-th - min-th)
    */
    soc_sand_u64_multiply_longs(SOC_TMC_WRED_NORMALIZE_FACTOR, max_prob * min_avrg_th_exact_wred_granular, &u64_2);
    soc_sand_u64_devide_u64_long(&u64_2, avrg_th_diff_wred_granular, &u64_c2);
    /*
    * P =
    */
    soc_sand_u64_multiply_longs(min_avrg_th_exact_wred_granular, (1 << tbl_data->c1),&u64_1);
    if(soc_sand_u64_is_bigger(&u64_c2, &u64_1)) {
        sal_memcpy(&u64_c2, &u64_1, sizeof(SOC_SAND_U64));
    }

    trunced = soc_sand_u64_to_long(&u64_c2, &tbl_data->c2);
    if (trunced) {
        tbl_data->c2 = 0xFFFFFFFF;
    }
exit: 
    SOCDNX_FUNC_RETURN; 
}
STATIC uint32
  jer_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
     SOC_SAND_IN int                                                     unit,
     SOC_SAND_IN  SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  *tbl_data,
     SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO                            *wred_param
  )
{
    uint32
      res;
    uint32
      avrg_th_diff_wred_granular,
      two_power_c1,
      remainder;
    SOC_SAND_U64
      u64_1,
      u64_2;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(wred_param); 
    SOCDNX_NULL_CHECK(tbl_data); 

    res = arad_itm_man_exp_buffer_get(
            tbl_data->min_avrg_th,
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit),
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit),
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit),
            FALSE,
            (int32*)&(wred_param->min_avrg_th)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);
    wred_param->min_avrg_th *= SOC_TMC_ITM_WRED_GRANULARITY;
    /*
     * max_avrg_th
     */
    res = arad_itm_man_exp_buffer_get(
            tbl_data->max_avrg_th,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit),
            FALSE,
            (int32*)&(wred_param->max_avrg_th)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);
    wred_param->max_avrg_th *= SOC_TMC_ITM_WRED_GRANULARITY;
    /*
     * max_packet_size
     */
    wred_param->max_packet_size = (0x1<<(tbl_data->c3))*SOC_TMC_ITM_WRED_GRANULARITY;
    /*
     *  Packet size ignore
     */
    wred_param->ignore_packet_size = SOC_SAND_NUM2BOOL(tbl_data->vq_wred_pckt_sz_ignr);
    avrg_th_diff_wred_granular = (wred_param->max_avrg_th - wred_param->min_avrg_th) / SOC_TMC_ITM_WRED_GRANULARITY;
    two_power_c1 = 1<<tbl_data->c1;
    /*
     * C1 = ((2^32)/100)*max-prob / (max-th - min-th) in powers of 2
     * ==>
     * max-prob =  ( 2^C1 * (max-th - min-th) ) / ((2^32)/100)
     */
    soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff_wred_granular, &u64_1);
    remainder = soc_sand_u64_devide_u64_long(&u64_1, SOC_TMC_WRED_NORMALIZE_FACTOR, &u64_2);
    soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);

    if(remainder > (SOC_TMC_WRED_NORMALIZE_FACTOR/2)) {
      wred_param->max_probability++;
    }

    if(wred_param->max_probability > 100) {
      wred_param->max_probability = 100;
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


uint32
  jer_itm_vsq_tail_drop_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  )
{
    uint32  
        res = SOC_E_NONE;
    uint32
        exp_man,
        entry_offset,
        array_index,
        data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    soc_field_t field; 
    uint32
        max_size_mnt_nof_bits,
        max_size_exp_nof_bits,
        max_size_mnt,
        max_size_exp;
    /* 
    int 
        ocb_offset = 0;
    */
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    res = jer_itm_vsq_tail_drop_default_get(unit, exact_info);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        array_index = pool_id;
    } else {
        array_index = 0;
    }
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    if (vsq_group_ndx >= SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS) {
        if (is_headroom) {
            field = VQ_MAX_BUFF_SHARED_SIZEf;
        } else {
            field = VQ_MAX_BUFF_HEADROOM_SIZEf;
        }
    } else {
        field = VQ_MAX_QUE_BUFF_SIZEf;
    }
    max_size_mnt_nof_bits = 8;
    max_size_exp_nof_bits = soc_mem_field_length(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], field) - max_size_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(info->max_inst_q_size_bds, max_size_mnt_nof_bits, max_size_exp_nof_bits, 0, &max_size_mnt, &max_size_exp);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    exact_info->max_inst_q_size_bds = (max_size_mnt * (1 << max_size_exp));

    arad_iqm_mantissa_exponent_set(unit, max_size_mnt, max_size_exp, max_size_mnt_nof_bits, &exp_man);   
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, field, exp_man); 

   /*
    *  If FadtDropExp3 is set FADT-Max-TH = Free-Shared-Resource << FadtDropExp2:-0
    *  Else, FADT-Max-TH = Free-Shared-Resource >> FadtDropExp2:-0
    */
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_PG) {
        if (info->alpha >= 0) {
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, FADT_DROP_EXPf, info->alpha);    
        } else {
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, FADT_DROP_EXPf,  (1 << 4) | (0x7 % (0 - info->alpha)));
        }
    }
    res = soc_mem_array_write(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}
/*********************************************************************
*     Get tail drop default parameters on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class.
*     The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  jer_itm_vsq_tail_drop_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    info->max_inst_q_size = (0x1 << soc_mem_field_length(unit, IQM_VSQA_PRMm, VQ_MAX_QUE_BUFF_SIZEf)) - 1;
    info->max_inst_q_size_bds = (0x1 << soc_mem_field_length(unit, IQM_VSQA_PRMm, VQ_MAX_QUE_BUFF_SIZEf)) - 1;
    info->alpha = (0x1 << soc_mem_field_length(unit, IQM_VSQE_PRMm, FADT_DROP_EXPf)) - 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_itm_vsq_tail_drop_get_unsafe()",0,0);
}

uint32
  jer_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    uint32  
        res = SOC_E_NONE;
    uint32
        exp_man;
    uint32
      entry_offset,
      array_index,
      data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    soc_field_t field;
    uint32 max_size_exp = 0;
    uint32 max_size_mnt = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        array_index = pool_id;
    } else {
        array_index = 0;
    }
    res = jer_itm_vsq_tail_drop_default_get(unit, info);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    /* since IQM_VQPR_M[A-F] have the same fields in hardware, but currently in the description
     IQM_VQPR_M[E-F] have different fields, we use mem_arr_IQM_VQPR_M[group_id] for memory
     access and IQM_VQPR_MAm for filed access */
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    if (vsq_group_ndx >= SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS) {
        if (is_headroom) {
            field = VQ_MAX_BUFF_SHARED_SIZEf;
        } else {
            field = VQ_MAX_BUFF_HEADROOM_SIZEf;
        }
    } else {
        field = VQ_MAX_QUE_BUFF_SIZEf;
    }
    exp_man = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, field);
    arad_iqm_mantissa_exponent_get(unit, exp_man, 8, &max_size_mnt, &max_size_exp);
    info->max_inst_q_size_bds = (max_size_mnt * (1 << max_size_exp));

    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_PG) {
        info->alpha = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, FADT_DROP_EXPf);
    }

    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}
/*********************************************************************
*    Set the VSQ flow-control info according to vsq-group-id
*********************************************************************/
uint32
  jer_itm_vsq_fc_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_FC_INFO    *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *exact_info
  )
{
  uint32
    res,
    vsq_bds_th_clear_and_set_mnt_nof_bits,
    vsq_bds_th_clear_and_set_exp_nof_bits,
    set_threshold_bd_mnt,
    set_threshold_bd_exp,
    clear_threshold_bd_mnt,
    clear_threshold_bd_exp,
    mnt_exp,
    data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE],
    array_index = 0;
  const soc_mem_t mem_arr_IQM_VSQ_FC_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_FC_PRMm, IQM_VSQB_FC_PRMm, IQM_VSQC_FC_PRMm, IQM_VSQD_FC_PRMm, IQM_VSQE_FC_PRMm, IQM_VSQF_FC_PRMm}; 
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);
  
  if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
      array_index = pool_id;
  } else {
      array_index = 0;
  }
  if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (info->bd_size_fc.clear > JER_ITM_VSQ_FC_BD_SIZE_MAX) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (info->bd_size_fc.set > JER_ITM_VSQ_FC_BD_SIZE_MAX) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  sal_memset(data, 0x0, sizeof(data));
  res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res); 

  vsq_bds_th_clear_and_set_mnt_nof_bits = 8;
  vsq_bds_th_clear_and_set_exp_nof_bits = soc_mem_field_length(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], SET_THRESHOLD_BDSf) - vsq_bds_th_clear_and_set_mnt_nof_bits;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.clear,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          0,
          &(clear_threshold_bd_mnt),
          &(clear_threshold_bd_exp)
        );
  SOCDNX_SAND_IF_ERR_EXIT(res); 

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.set,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          0,
          &(set_threshold_bd_mnt),
          &(set_threshold_bd_exp)
        );
  SOCDNX_SAND_IF_ERR_EXIT(res); 

  exact_info->bd_size_fc.clear = ((clear_threshold_bd_mnt) * (1 << (clear_threshold_bd_exp)));
  exact_info->bd_size_fc.set = ((set_threshold_bd_mnt) * (1 << (set_threshold_bd_exp)));

  arad_iqm_mantissa_exponent_set(unit, set_threshold_bd_mnt, set_threshold_bd_exp, 4, &mnt_exp);
  soc_mem_field32_set(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, SET_THRESHOLD_BDSf, mnt_exp);

  arad_iqm_mantissa_exponent_set(unit, clear_threshold_bd_mnt, clear_threshold_bd_exp, 4, &mnt_exp);
  soc_mem_field32_set(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, CLR_THRESHOLD_BDSf, mnt_exp);

  res = soc_mem_array_write(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res); 

exit: 
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*    Get the VSQ flow-control info according to vsq-group-id
*********************************************************************/
uint32
  jer_itm_vsq_fc_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *info
  )
{
  uint32
    res,
    set_threshold_bd_mnt,
    set_threshold_bd_exp,
    clear_threshold_bd_mnt,
    clear_threshold_bd_exp,
    mnt_exp,
    data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE],
    array_index = 0;
  const soc_mem_t mem_arr_IQM_VSQ_FC_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_FC_PRMm, IQM_VSQB_FC_PRMm, IQM_VSQC_FC_PRMm, IQM_VSQD_FC_PRMm, IQM_VSQE_FC_PRMm, IQM_VSQF_FC_PRMm};
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  }
  if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
      array_index = pool_id;
  } else {
      array_index = 0;
  }
  if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  sal_memset(data, 0x0, sizeof(data));
  res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  res = soc_mem_read(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res);

  mnt_exp = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, SET_THRESHOLD_BDSf);
  arad_iqm_mantissa_exponent_get(unit, mnt_exp, 8, &(set_threshold_bd_mnt), &(set_threshold_bd_exp));

  mnt_exp = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, CLR_THRESHOLD_BDSf);
  arad_iqm_mantissa_exponent_get(unit, mnt_exp, 8, &(clear_threshold_bd_mnt), &(clear_threshold_bd_exp));

  info->bd_size_fc.clear = ((clear_threshold_bd_mnt) * (1 << (clear_threshold_bd_exp)));
  info->bd_size_fc.set = ((set_threshold_bd_mnt) * (1 << (set_threshold_bd_exp)));
  info->q_size_fc.clear = 0;
  info->q_size_fc.set = 0;

exit: 
    SOCDNX_FUNC_RETURN;
}
/*
 * Map VSQ-E to CFC LLFC index
 *                  (Value must be 0-143)
 */
uint32
    jer_itm_vsq_src_port_set(
       SOC_SAND_IN int    unit,
       SOC_SAND_IN int    core_id,
       SOC_SAND_IN int    src_port_vsq_index,
       SOC_SAND_IN uint32 src_pp_port,
       SOC_SAND_IN uint8  enable
       )
{
    uint32
        res,
        data,
        field_val;
    soc_port_t logical_port;
    uint32 
        src_link = -1, 
        phy_port = -1;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_port_vsq_index < 0 || src_port_vsq_index >= SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_pp_port > SOC_MAX_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "Invalid src_pp_port %d\n"), src_pp_port));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, src_pp_port, &logical_port);
    SOCDNX_IF_ERR_EXIT(res);

    res = soc_port_sw_db_first_phy_port_get(unit, logical_port, &phy_port);
    SOCDNX_IF_ERR_EXIT(res);
    src_link = phy_port ? phy_port - 1 : phy_port;

    /*Mapping ports to VSQE (LLFC): The CFC LLFC will be configured in VSQE_MAP automatically according to the NIF interface of the src PP port.*/
    res = READ_IQM_VSQE_MAPm(unit, IQM_BLOCK(unit, core_id), src_port_vsq_index, &data);
    SOCDNX_IF_ERR_EXIT(res);

    field_val = enable ? src_link : ((1 << soc_mem_field_length(unit, IQM_VSQE_MAPm, VSQE_MAP_DATAf)) - 1);
    soc_mem_field32_set(unit, IQM_VSQE_MAPm, &data, VSQE_MAP_DATAf, field_val);
    res = WRITE_IQM_VSQE_MAPm(unit, IQM_BLOCK(unit, core_id), src_port_vsq_index, &data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
    jer_itm_vsq_src_port_get(
       SOC_SAND_IN int    unit,
       SOC_SAND_IN int    core_id,
       SOC_SAND_IN int    src_port_vsq_index,
       SOC_SAND_OUT uint32 *src_pp_port,
       SOC_SAND_OUT uint8  *enable
       )
{
    uint32
        res,
        data,
        field_val;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_port_vsq_index < 0 || src_port_vsq_index >= SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    /*Mapping ports to VSQE (LLFC): The CFC LLFC will be configured in VSQE_MAP automatically according to the NIF interface of the src PP port.*/
    res = READ_IQM_VSQE_MAPm(unit, IQM_BLOCK(unit, core_id), src_port_vsq_index, &data);
    SOCDNX_IF_ERR_EXIT(res);

    field_val = soc_mem_field32_get(unit, IQM_VSQE_MAPm, &data, VSQE_MAP_DATAf);
    *enable = (field_val != ((1 << soc_mem_field_length(unit, IQM_VSQE_MAPm, VSQE_MAP_DATAf)) - 1)) ? TRUE : FALSE;

exit: 
    SOCDNX_FUNC_RETURN;
}

/*
 * Map VSQF to CFC PFC index
 */
uint32
jer_itm_vsq_pg_mapping_set(SOC_SAND_IN int    unit,
                           SOC_SAND_IN int    core_id,
                           SOC_SAND_IN uint32 pg_vsq_base,
                           SOC_SAND_IN int    cosq,
                           SOC_SAND_IN int    src_pp_port,
                           SOC_SAND_IN uint8  enable
                           )
{
    uint32 
        pfc_cfc_index = 0,
        res,
        data;
    uint32 
        pg_vsq_id;
    soc_port_t 
        logical_port;
    uint32 
        src_link = -1,
        phy_port = -1;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (cosq < 0 || cosq >= SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    pg_vsq_id = pg_vsq_base + cosq;
    /*When creating a VSQE mapping - we create a mapping for dram-mixed and ocb-only VSQs*/
    if (pg_vsq_id >= SOC_TMC_ITM_VSQ_GROUPF_SZE(unit)) {
        LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "Invalid PG VSQ ID %d\n"), pg_vsq_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    /*Map VSQ-F to CFC PFC index (Value must be 0-575).*/
    res = READ_IQM_VSQF_MAPm(unit, IQM_BLOCK(unit, core_id), pg_vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);

    /*calculate pfc_cfc_index*/
    res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, src_pp_port, &logical_port);
    SOCDNX_IF_ERR_EXIT(res);

    res = soc_port_sw_db_first_phy_port_get(unit, logical_port, &phy_port);
    SOCDNX_IF_ERR_EXIT(res);
    /* 
     * Remove QSGMIIs offset from phy port:
     * Input:  Physical port in the range of 1-144
     * Output: Physical lane in the range of 1-72
     */
    res = soc_jer_qsgmii_offsets_remove(unit, phy_port, &phy_port);
    SOCDNX_IF_ERR_EXIT(res);
    src_link = phy_port ? phy_port - 1 : phy_port;

    pfc_cfc_index = enable ? (src_link * SOC_TMC_NOF_TRAFFIC_CLASSES) + cosq : 
        ((1 << soc_mem_field_length(unit, IQM_VSQF_MAPm, VSQF_MAP_DATAf)) - 1);
    soc_mem_field32_set(unit, IQM_VSQF_MAPm, &data, VSQF_MAP_DATAf, pfc_cfc_index);

    res = WRITE_IQM_VSQF_MAPm(unit, IQM_BLOCK(unit, core_id), pg_vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);
    

exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
jer_itm_vsq_pg_mapping_get(SOC_SAND_IN int    unit,
                           SOC_SAND_IN int    core_id,
                           SOC_SAND_IN uint32 pg_vsq_base,
                           SOC_SAND_IN int    cosq,
                           SOC_SAND_IN uint32 src_pp_port,
                           SOC_SAND_OUT uint8  *enable
                           )
{
    uint32 
        pg_vsq_id;
    uint32 
        pfc_cfc_index,
        res,
        data;
    soc_port_t 
        logical_port;
    uint32 
        src_link = -1,
        phy_port = -1;
    int 
        core_get;
    uint32 
        src_pp_port_get;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(enable);
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (cosq < 0 || cosq >= SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    pg_vsq_id = pg_vsq_base + cosq;
    if (pg_vsq_id >= SOC_TMC_ITM_VSQ_GROUPF_SZE(unit)) {
        LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "Invalid PG VSQ ID %d\n"), pg_vsq_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    /*Map VSQ-F to CFC PFC index (Value must be 0-575).*/
    res = READ_IQM_VSQF_MAPm(unit, IQM_BLOCK(unit, core_id), pg_vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);

    pfc_cfc_index = soc_mem_field32_get(unit, IQM_VSQF_MAPm, &data, VSQF_MAP_DATAf);

    if (pfc_cfc_index == ((1 << soc_mem_field_length(unit, IQM_VSQF_MAPm, VSQF_MAP_DATAf)) - 1)) {
        *enable = FALSE;
    } else {
        src_link = pfc_cfc_index / SOC_TMC_NOF_TRAFFIC_CLASSES;
        phy_port = src_link + 1;
        res = soc_jer_qsgmii_offsets_add(unit, phy_port, &phy_port);
        SOCDNX_IF_ERR_EXIT(res);
        logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
        res = soc_port_sw_db_local_to_pp_port_get(unit, logical_port, &src_pp_port_get, &core_get);
        SOCDNX_IF_ERR_EXIT(res);
        if (core_id != core_get) {
            LOG_ERROR(BSL_LS_SOC_COSQ, 
                      (BSL_META_U
                       (unit, "pfc_cfc_index %d, is not configured in the right core. Expected %d, but got %d\n"), pfc_cfc_index, core_id, core_get));
            *enable = FALSE;
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (src_pp_port != src_pp_port_get) {
            LOG_ERROR(BSL_LS_SOC_COSQ, 
                      (BSL_META_U
                       (unit, "pfc_cfc_index %d, is not configuref for the right pp_port. Expected %d, but got %d\n"), pfc_cfc_index, src_pp_port, src_pp_port_get));
            *enable = FALSE;
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        *enable = TRUE;
    }
exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
jer_itm_vsq_pg_tc_profile_mapping_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 vsq_id,
    SOC_SAND_IN int pg_tc_profile
   ) 
{
    uint32
        res;
    uint32 
        data = 0;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = READ_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);

    soc_mem_field32_set(unit, IQM_IPPPMm, &data, PG_TC_BITMAP_INDEXf, pg_tc_profile);

    res = WRITE_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);
exit: 
    SOCDNX_FUNC_RETURN;

}
uint32
jer_itm_vsq_pg_tc_profile_mapping_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 vsq_id,
    SOC_SAND_OUT int *pg_tc_profile
   ) 
{
    uint32
        res;
    uint32 
        data = 0;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = READ_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);

    *pg_tc_profile = soc_mem_field32_get(unit, IQM_IPPPMm, &data, PG_TC_BITMAP_INDEXf);

exit: 
    SOCDNX_FUNC_RETURN;

}

uint32
jer_itm_vsq_pg_tc_profile_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN int pg_tc_profile_id,
    SOC_SAND_IN uint32 pg_tc_bitmap
   ) 
{
    uint32 pg_tc_bitamp_data = 0;
    uint32
        res;
    SOCDNX_INIT_FUNC_DEFS;

    if (pg_tc_bitmap & ~((1 << soc_mem_field_length(unit, IQM_PG_TC_BITMAPm, PG_TC_BITMAPf)) - 1)) {
        LOG_ERROR(BSL_LS_SOC_COSQ, 
                  (BSL_META_U(unit, "PG TC mapping bitmap is invalid %d, maximum bit nust be %d\n"), 
                   pg_tc_bitmap, soc_mem_field_length(unit, IQM_PG_TC_BITMAPm, PG_TC_BITMAPf)));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    soc_mem_field32_set(unit, IQM_PG_TC_BITMAPm, &pg_tc_bitamp_data, PG_TC_BITMAPf, pg_tc_bitmap);
    res = WRITE_IQM_PG_TC_BITMAPm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), pg_tc_profile_id, &pg_tc_bitamp_data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
jer_itm_vsq_pg_tc_profile_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core_id,
    SOC_SAND_IN int         pg_tc_profile_id,
    SOC_SAND_OUT uint32     *pg_tc_bitmap
   ) 
{
    uint32
        res;
    uint32 pg_tc_bitamp_data = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(pg_tc_bitmap);

    res = READ_IQM_PG_TC_BITMAPm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), pg_tc_profile_id, &pg_tc_bitamp_data);
    SOCDNX_IF_ERR_EXIT(res);
    *pg_tc_bitmap = soc_mem_field32_get(unit, IQM_PG_TC_BITMAPm, &pg_tc_bitamp_data, PG_TC_BITMAPf);

exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_itm_vsq_pg_prm_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_PRM *pg_prm
  )
{
    uint32  
        res = SOC_E_NONE;
    uint32
        data = 0;   
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pg_prm);

    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
   
    res = READ_IQM_VSQ_PG_PRMm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, IS_LOSSLESSf, pg_prm->is_lossles);
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, SHARED_POOL_NUMf, pg_prm->pool_id);
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, USE_MIN_PORTf, pg_prm->use_min_port);

    /*
     *  FADT Flow control adjust factor.
     *     If FadtDropExp[3] is set
     *      - FADT-Fc-Set-TH = Free-Shared-Resource <<FadtFcExp[2:-0]
     *     Else,
     *     - FADT-Fc-Set-TH = Free-Shared-Resource >> FadtFcExp[2:-0]
     */
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, FADT_FC_EXPf, ((pg_prm->fadt_fc.sign & 0x1) << 2) |  (pg_prm->fadt_fc.exp & 0x3));
    /* 
     * FADT Flow control clear adjust factor.
     *   If FadtDropExp[3] is set
     *     - FADT-Fc-Clr-TH = Free-Shared-Resource <<FadtFcExp[2:-0] - FadtFcOffset
     *   Else,
     *     - FADT-Fc-Clr-TH = Free-Shared-Resource >> FadtFcExp[2:-0] - FadtFcOffset 
     */
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, FADT_FC_OFFSETf, pg_prm->fadt_fc.offset);
    /* 
     * If VSQ-Size smaller than FadtFcFloor, than flow control indication is cleared.
     */
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, FADT_FC_FLOORf, pg_prm->fadt_fc.floor);

    res = WRITE_IQM_VSQ_PG_PRMm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}

uint32
  jer_itm_vsq_pg_prm_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              vsq_rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_PRM *pg_prm
  )
{
    uint32  
        res = SOC_E_NONE;
    uint32
        data = 0;
    uint32 
        fadt_fc_exp_field_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pg_prm);

    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
   
    res = READ_IQM_VSQ_PG_PRMm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    pg_prm->is_lossles = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, IS_LOSSLESSf);
    pg_prm->pool_id = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, SHARED_POOL_NUMf);
    pg_prm->use_min_port = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, USE_MIN_PORTf);

    fadt_fc_exp_field_val = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, FADT_FC_EXPf);
    pg_prm->fadt_fc.exp = (fadt_fc_exp_field_val & 0x3);
    pg_prm->fadt_fc.sign = ((fadt_fc_exp_field_val >> 2) & 0x1);
    pg_prm->fadt_fc.offset = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, FADT_FC_OFFSETf);
    pg_prm->fadt_fc.floor = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, FADT_FC_FLOORf);

exit: 
    SOCDNX_FUNC_RETURN; 
}




/*********************************************************************
*	Initialize the Mrps-In-Dp-Mapping and Drop-Precedence-Mapping.
* 	Details in the H file.
*********************************************************************/

uint32 
  jer_itm_setup_dp_map(
    SOC_SAND_IN  int unit
  )
{
  uint32
	  detailed_color_mode,
	  color_blind_mode,
	  mtr_cmd_indx,
	  in_dp_indx,
	  eth_mtr_dp_indx,
	  eth_mtr_invalid_indx,
	  mtr_dp_indx,
	  mrps_dp,
	  mrps_drop,
	  resolved_dp,
	  ingress_dp,
	  egress_dp,
	  entry_indx,
	  tbl_data;
  uint8
	  divide_egress_dp = FALSE;

  SOCDNX_INIT_FUNC_DEFS;

  detailed_color_mode = soc_property_get(unit, spn_POLICER_COLOR_RESOLUTION_MODE, 0) ? 1 : 0;
  color_blind_mode = soc_property_get(unit, spn_RATE_COLOR_BLIND, 0) ? 1 : 0;

  /* Mrps In-DP Mapping, each MRPS processor has its own table
     key:
     dp-meter-cmd 2 bit,
     incoming-dp 2 bit,
     ethernet-meter-dp 2 bit,
     ethernet-meter-invalid 1 bit
     value:
     mrps-in-dp 2 bit
     mrps-in-drop 1 bit */

  entry_indx = 0;
  for (mtr_cmd_indx = 0; mtr_cmd_indx < ARAD_PP_NOF_MTR_RES_USES; ++mtr_cmd_indx)  {
    for (in_dp_indx = 0; in_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++in_dp_indx)  {
      for (eth_mtr_dp_indx = 0; eth_mtr_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++eth_mtr_dp_indx) {
		  for (eth_mtr_invalid_indx = 0; eth_mtr_invalid_indx < 2; ++eth_mtr_invalid_indx) {
			  /* EM sets the Mrps-In-Dp to Red(=3) in PP Mode, when its enabled (not invalid) and:
			     1. its output color is Red
			     Or
			     2. its input color is red and it's not color blind*/
			  if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable && !eth_mtr_invalid_indx 
				  && ((3 == eth_mtr_dp_indx) || (3 == in_dp_indx && !color_blind_mode))) 
			  {
				  mrps_dp = 3;
				  mrps_drop = 1; /* Mrps will refer the packet as Red, igonring color_blind and always_green bits*/
			  }
			  else /* EM has no effect */
			  {
				  mrps_dp = in_dp_indx;
				  mrps_drop = 0;
			  }

			  /* MRPS0/1 tables share configuration*/
			  SOCDNX_IF_ERR_EXIT(READ_IDR_MRPS_0_IN_DP_MAPPINGm(unit, IDR_BLOCK(unit) , entry_indx, &tbl_data));

			  soc_mem_field_set(unit, IDR_MRPS_0_IN_DP_MAPPINGm, &tbl_data, MRPS_IN_DPf, &mrps_dp);
			  soc_mem_field_set(unit, IDR_MRPS_0_IN_DP_MAPPINGm, &tbl_data, MRPS_IN_DROPf, &mrps_drop);

			  SOCDNX_IF_ERR_EXIT(WRITE_IDR_MRPS_0_IN_DP_MAPPINGm(unit, IDR_BLOCK(unit), entry_indx, &tbl_data));
			  SOCDNX_IF_ERR_EXIT(WRITE_IDR_MRPS_1_IN_DP_MAPPINGm(unit, IDR_BLOCK(unit), entry_indx, &tbl_data));

			  ++entry_indx;
		  }
	  }
	}
  }

  /* Drop Precedence Mapping, one table for both MRPS processors
	 key:
     dp-meter-cmd 2 bit,
     incoming-dp 2 bit,
     ethernet-meter-dp 2 bit,
     mrps-meter-dp 2 bit
     value:
     ingress-dp 2 bit
     egress-dp 2 bit */

  if ( !detailed_color_mode && SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
	  /* If not in detailed mode and PB, use 1 bit egress DP */
	  divide_egress_dp = TRUE;
  }

  entry_indx = 0;
  for (mtr_cmd_indx = 0; mtr_cmd_indx < ARAD_PP_NOF_MTR_RES_USES; ++mtr_cmd_indx)  {
    for (in_dp_indx = 0; in_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++in_dp_indx)  {
      for (eth_mtr_dp_indx = 0; eth_mtr_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++eth_mtr_dp_indx) {
		  for (mtr_dp_indx = 0; mtr_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++mtr_dp_indx) {
			  /* TM Mode */
			  if( !SOC_DPP_CONFIG(unit)->arad->init.pp_enable)
			  {
				  ingress_dp = egress_dp = in_dp_indx; /* metering has no effect */ 
			  }
			  else /* PP Mode */
			  {
				  /* resolved dp is meter dp*/
				  resolved_dp = mtr_dp_indx;

				  /* detailed color mode differentiate between ethernet-meter Red(=2) and meter Red(=3) */
				  if (detailed_color_mode) {
					  if (2 == mtr_dp_indx) {
						  resolved_dp = 1;/* Meter Yellow(=2) becomes 1 */
					  }else if (3 == eth_mtr_dp_indx) {
						  resolved_dp = 2;/* Ethernet-meter Red(=3) becomes 2 */
					  }
				  }

				  ingress_dp = egress_dp = resolved_dp;

				  /* custom mapping property for DP=2*/
				  if (2 == resolved_dp) {
					  if (1 == soc_property_suffix_num_get(unit, 0, "custom_feature", "always_map_result_dp_2_to_1", 0)){
						  ingress_dp = 1;
						  egress_dp = 1;
					  }
					  else if (1 == soc_property_suffix_num_get(unit, 0, "custom_feature", "always_map_ingress_result_dp_2_to_1", 0)){
						  ingress_dp = 1;
					  }
					  else if (1 == soc_property_suffix_num_get(unit, 0, "custom_feature", "always_map_egress_result_dp_2_to_1", 0)){
						  egress_dp = 1;
					  }
				  }

				  /* apply meter-cmd*/
				  if (mtr_cmd_indx == ARAD_PP_MTR_RES_USE_NONE) {
					  /* metering has no effect */
					  ingress_dp = egress_dp = in_dp_indx; 
				  } else if (mtr_cmd_indx == ARAD_PP_MTR_RES_USE_OW_DP) {
					  /* metering affects ingress only */
					  egress_dp = in_dp_indx; 
				  } else if (mtr_cmd_indx == ARAD_PP_MTR_RES_USE_OW_DE) {
					  /* metering affects egress only */
					  ingress_dp = in_dp_indx;
				  } /*else, metering affects both ingress and egress*/
			  }

			  if (divide_egress_dp) {
				  egress_dp /= 2;
			  }

			  SOCDNX_IF_ERR_EXIT(READ_IDR_DROP_PRECEDENCE_MAPPINGm(unit, IDR_BLOCK(unit), entry_indx, &tbl_data));

			  soc_mem_field_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, &tbl_data, INGRESS_DROP_PRECEDENCEf, &ingress_dp);
			  soc_mem_field_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, &tbl_data, EGRESS_DROP_PRECEDENCEf, &egress_dp);

			  SOCDNX_IF_ERR_EXIT(WRITE_IDR_DROP_PRECEDENCE_MAPPINGm(unit, IDR_BLOCK(unit), entry_indx, &tbl_data));

			  ++entry_indx;
		  }
	  }
	}
  }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
jer_itm_src_vsqs_mapping_set(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int src_port_vsq_index,
       SOC_SAND_IN int src_pp_port,
       SOC_SAND_IN int pg_base
       ) 
{
    int 
        cosq;
    uint32
        res;
    uint32 
        data;
    uint8
        numq;
    soc_port_t 
        logical_port;
    uint32 
        nif_port;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_port_vsq_index < 0 || src_port_vsq_index >= SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pg_base < 0 || pg_base >= SOC_TMC_ITM_VSQ_GROUPF_SZE(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_pp_port < 0 || src_pp_port > SOC_MAX_NUM_PORTS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, src_pp_port, &logical_port);
    SOCDNX_IF_ERR_EXIT(res);
    
    res = soc_port_sw_db_egr_if_get(unit, logical_port, &nif_port);
    SOCDNX_IF_ERR_EXIT(res);
    
    res = READ_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);

    /*VSQE mapping*/
    /*In-PP-Port => NIF-Port (Port-base)*/
    soc_mem_field32_set(unit, IQM_IPPPMm, &data, NIF_PORTf, nif_port);

    /*VSQF mapping*/
    /*In-PP-Port(8) => { PG-Base(9), PG-MAP-Profile-Bitmap-Index(4)}*/
    soc_mem_field32_set(unit, IQM_IPPPMm, &data, PG_BASEf, pg_base);
    /* 
     * XGS mode is used when the Jericho is connected to an XGS device. 
     * This mode is not supported yet.
     */
    soc_mem_field32_set(unit, IQM_IPPPMm, &data, XGS_ST_VSQf, 0);

    res = WRITE_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);

    /*Mapping ports to VSQE (LLFC): The CFC LLFC will be configured in VSQE_MAP automatically according to the NIF interface of the src PP port.*/
    res = jer_itm_vsq_src_port_set(unit, core_id, src_port_vsq_index, (uint32)src_pp_port, TRUE);
    SOCDNX_IF_ERR_EXIT(res);

    /*configure a VSQ PG*/
    /* Mapping port+IQM-TCs to PG/VSQE: The CFC PFC will be configured in VSQF_MAP automatically according to the port and the IQM-TC. */ 
    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.pg_numq.get(unit, core_id, pg_base, &numq);
    SOCDNX_IF_ERR_EXIT(res);

    for (cosq = 0; cosq < numq; cosq++) {
        res = jer_itm_vsq_pg_mapping_set(unit, core_id, pg_base, cosq, src_pp_port, TRUE);
        SOCDNX_IF_ERR_EXIT(res);
    }
exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
jer_itm_src_vsqs_mapping_get(
       SOC_SAND_IN  int unit,
       SOC_SAND_IN  int core_id,
       SOC_SAND_IN  int src_pp_port,
       SOC_SAND_OUT int *src_port_vsq_index,
       SOC_SAND_OUT int *pg_base
       ) 
{
    uint32
        res;
    uint32 
        data;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(src_port_vsq_index);
    SOCDNX_NULL_CHECK(pg_base);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_pp_port < 0 || src_pp_port >= SOC_MAX_NUM_PORTS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.src_port_vsq.get(unit, core_id, src_pp_port, src_port_vsq_index);
    SOCDNX_IF_ERR_EXIT(res);

    res = READ_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);

    /*VSQE mapping*/
    /* soc_mem_field32_get(unit, IQM_IPPPMm, &data, NIF_PORTf);*/
    /*VSQF mapping*/
    *pg_base = soc_mem_field32_get(unit, IQM_IPPPMm, &data, PG_BASEf);
    /* 
     * XGS mode is used when the Jericho is connected to an XGS device. 
     * This mode is not supported yet.
     * soc_mem_field32_get(unit, IQM_IPPPMm, &data, XGS_ST_VSQf);
     */

exit: 
    SOCDNX_FUNC_RETURN;
}
uint32
jer_itm_vsq_pg_ocb_set(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN int numq,
       SOC_SAND_IN uint8* ocb_only
       )
{
    int 
        cosq;
    uint32
        res;
    soc_reg_above_64_val_t 
        vsq_pg_is_ocb_data;
    soc_reg_above_64_val_t 
        vsq_pg_is_ocb_field_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(ocb_only); 

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pg_base < 0 || pg_base >= SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (numq < 1 || numq > SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    for (cosq = 0; cosq < numq; cosq++) {
        if (ocb_only[cosq] > 1) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    /*OCB only is an attribute of the PG to be configured in IQM_VSQ_PG_IS_OCB_ONLYm and does not selects a VSQ like in the other VSQ types*/
    res = READ_IQM_VSQ_PG_IS_OCB_ONLYr(unit, core_id, vsq_pg_is_ocb_data);
    SOCDNX_IF_ERR_EXIT(res);
    soc_reg_above_64_field_get(unit, IQM_VSQ_PG_IS_OCB_ONLYr, vsq_pg_is_ocb_data, VSQ_PG_IS_OCB_ONLYf, vsq_pg_is_ocb_field_val);
    for (cosq = 0; cosq < numq; cosq++) {
        if (ocb_only[cosq]) {
            SHR_BITSET(vsq_pg_is_ocb_field_val, pg_base + cosq);
        } else {
            SHR_BITCLR(vsq_pg_is_ocb_field_val, pg_base + cosq);
        }
    }
    soc_reg_above_64_field_set(unit, IQM_VSQ_PG_IS_OCB_ONLYr, vsq_pg_is_ocb_data, VSQ_PG_IS_OCB_ONLYf, vsq_pg_is_ocb_field_val);
    res = WRITE_IQM_VSQ_PG_IS_OCB_ONLYr(unit, core_id, vsq_pg_is_ocb_data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
jer_itm_vsq_pg_ocb_get(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN int numq,
       SOC_SAND_OUT uint8* ocb_only
       )
{
    int 
        cosq;
    uint32
        res;
    soc_reg_above_64_val_t 
        vsq_pg_is_ocb_data;
    soc_reg_above_64_val_t 
        vsq_pg_is_ocb_field_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(ocb_only);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pg_base < 0 || pg_base >= SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (numq < 1 || numq > SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    /*OCB only is an attribute of the PG to be configured in IQM_VSQ_PG_IS_OCB_ONLYm and does not selects a VSQ like in the other VSQ types*/
    res = READ_IQM_VSQ_PG_IS_OCB_ONLYr(unit, core_id, vsq_pg_is_ocb_data);
    SOCDNX_IF_ERR_EXIT(res);

    for (cosq = 0; cosq < numq; cosq++) {
        soc_reg_above_64_field_get(unit, IQM_VSQ_PG_IS_OCB_ONLYr, vsq_pg_is_ocb_data, VSQ_PG_IS_OCB_ONLYf, vsq_pg_is_ocb_field_val);
        ocb_only[cosq] = SHR_BITGET(vsq_pg_is_ocb_field_val, (pg_base + cosq)) ? TRUE : FALSE;
    }
exit: 
    SOCDNX_FUNC_RETURN;
}
uint32
  jer_itm_glob_rcs_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  )
{
    uint32
        res;
    soc_reg_t 
        shared_pool_reg = 0;
    soc_field_t 
        shared_pool_field_set = 0,
        shared_pool_field_clr = 0;
    soc_reg_above_64_val_t 
        shared_pool_data;
    uint8 
        is_ocb_only = FALSE;
    uint32
        drop_precedence_ndx;
    uint8
        pool_id;
    const soc_field_t 
        rjct_set_fr_bdb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_RJCT_SET_FR_BDB_TH_0f, 
            MIXED_RJCT_SET_FR_BDB_TH_1f, 
            MIXED_RJCT_SET_FR_BDB_TH_2f, 
            MIXED_RJCT_SET_FR_BDB_TH_3f,
            OCB_RJCT_SET_FR_BDB_TH_0f, 
            OCB_RJCT_SET_FR_BDB_TH_1f, 
            OCB_RJCT_SET_FR_BDB_TH_2f, 
            OCB_RJCT_SET_FR_BDB_TH_3f};
    const soc_field_t 
        rjct_clr_fr_bdb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_RJCT_CLR_FR_BDB_TH_0f, 
            MIXED_RJCT_CLR_FR_BDB_TH_1f, 
            MIXED_RJCT_CLR_FR_BDB_TH_2f, 
            MIXED_RJCT_CLR_FR_BDB_TH_4f,
            OCB_RJCT_CLR_FR_BDB_TH_0f, 
            OCB_RJCT_CLR_FR_BDB_TH_1f, 
            OCB_RJCT_CLR_FR_BDB_TH_2f, 
            OCB_RJCT_CLR_FR_BDB_TH_4f};
    const soc_field_t 
        rjct_set_oc_bd_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_RJCT_SET_OC_BD_TH_0f,
            MIXED_RJCT_SET_OC_BD_TH_1f,
            MIXED_RJCT_SET_OC_BD_TH_2f,
            MIXED_RJCT_SET_OC_BD_TH_3f,
            OCB_RJCT_SET_OC_BD_TH_0f,
            OCB_RJCT_SET_OC_BD_TH_1f,
            OCB_RJCT_SET_OC_BD_TH_2f,
            OCB_RJCT_SET_OC_BD_TH_3f};
    const soc_field_t 
        rjct_clr_oc_bd_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_RJCT_CLR_OC_BD_TH_0f, 
            MIXED_RJCT_CLR_OC_BD_TH_1f, 
            MIXED_RJCT_CLR_OC_BD_TH_2f, 
            MIXED_RJCT_CLR_OC_BD_TH_3f,
            OCB_RJCT_CLR_OC_BD_TH_0f, 
            OCB_RJCT_CLR_OC_BD_TH_1f, 
            OCB_RJCT_CLR_OC_BD_TH_2f, 
            OCB_RJCT_CLR_OC_BD_TH_3f};
    const soc_field_t 
        rjct_clr_fr_bd_mmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_CLR_FR_DB_MNMC_TH_0f,
            RJCT_CLR_FR_DB_MNMC_TH_1f,
            RJCT_CLR_FR_DB_MNMC_TH_2f,
            RJCT_CLR_FR_DB_MNMC_TH_3f};
    const soc_field_t
        rjct_set_fr_bd_mmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_SET_FR_DB_MNMC_TH_0f,
            RJCT_SET_FR_DB_MNMC_TH_1f,
            RJCT_SET_FR_DB_MNMC_TH_2f,
            RJCT_SET_FR_DB_MNMC_TH_3f};
    const soc_field_t 
        rjct_clr_fr_bd_flmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_CLR_FR_DB_FLMC_TH_0f,
            RJCT_CLR_FR_DB_FLMC_TH_1f,
            RJCT_CLR_FR_DB_FLMC_TH_2f,
            RJCT_CLR_FR_DB_FLMC_TH_3f};
    const soc_field_t 
        rjct_set_fr_bd_flmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_SET_FR_DB_FLMC_TH_0f,
            RJCT_SET_FR_DB_FLMC_TH_1f,
            RJCT_SET_FR_DB_FLMC_TH_2f,
            RJCT_SET_FR_DB_FLMC_TH_3f};
    const soc_field_t 
        rjct_dyn_size_clr_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_0f,
            MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_1f,
            MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_2f,
            MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_3f,
            OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_0f,
            OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_1f,
            OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_2f,
            OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_3f};
    const soc_field_t 
        rjct_dyn_size_set_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_0f,
            MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_1f,
            MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_2f,
            MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_3f,
            OCB_DYN_WORDS_SIZE_RJCT_SET_TH_0f,
            OCB_DYN_WORDS_SIZE_RJCT_SET_TH_1f,
            OCB_DYN_WORDS_SIZE_RJCT_SET_TH_2f,
            OCB_DYN_WORDS_SIZE_RJCT_SET_TH_3f};
    const soc_field_t 
        rjct_set_fr_ocb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_SET_FR_OCB_TH_0f,
            RJCT_SET_FR_OCB_TH_1f,
            RJCT_SET_FR_OCB_TH_2f,
            RJCT_SET_FR_OCB_TH_3f};
    const soc_field_t 
        rjct_clr_fr_ocb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_CLR_FR_OCB_TH_0f,
            RJCT_CLR_FR_OCB_TH_1f,
            RJCT_CLR_FR_OCB_TH_2f,
            RJCT_CLR_FR_OCB_TH_3f};
    SOC_TMC_THRESH_WITH_HYST_INFO *shrd_pool_th;
    uint32
      exp_man,
      bdbs_th_mnt_nof_bits,
      bds_th_mnt_nof_bits,
      mini_mc_mnt_nof_bits,
      fmc_th_mnt_nof_bits,
      uc_ocb_mnt_nof_bits,
      dyn_mnt_nof_bits;
    uint32
      mnt_val,
      exp_val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    for (is_ocb_only = 0; is_ocb_only < 2; is_ocb_only++) {
        for (pool_id = 0; pool_id < SOC_TMC_ITM_NOF_RSRC_POOLS; pool_id++) {
            for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
                if (is_ocb_only == TRUE) {
                    shrd_pool_th = &info->ocb_shrd_pool[pool_id][drop_precedence_ndx];
                    if (pool_id == 0) {
                        shared_pool_reg = IQM_OCB_SHRD_POOL_0_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                    if (pool_id == 1) {
                        shared_pool_reg = IQM_OCB_SHRD_POOL_1_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                } else {
                    shrd_pool_th = &info->mix_shrd_pool[pool_id][drop_precedence_ndx];
                    if (pool_id == 0) {
                        shared_pool_reg = IQM_MIX_SHRD_POOL_0_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                    if (pool_id == 1) {
                        shared_pool_reg = IQM_MIX_SHRD_POOL_1_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_3f;
                        }
                    }
                }
                res = soc_reg_above_64_get(unit, shared_pool_reg, core_id, 0, shared_pool_data);
                SOCDNX_IF_ERR_EXIT(res);

                shrd_pool_th->set   = soc_reg_above_64_field32_get(unit, shared_pool_reg, shared_pool_data, shared_pool_field_set);
                shrd_pool_th->clear = soc_reg_above_64_field32_get(unit, shared_pool_reg, shared_pool_data, shared_pool_field_clr);
            }
        }
    }
  /*
   *  BDB-s (Buffer Descriptor Buffers)
   */
  bdbs_th_mnt_nof_bits = 8;
  for (is_ocb_only = 0; is_ocb_only < 2; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, 
                    core_id, 
                    0, 
                    rjct_clr_fr_bdb_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
          if (is_ocb_only) {
              info->bdbs[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
          } else {
              info->ocb_bdbs[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
          }
          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, 
                    core_id, 
                    0, 
                    rjct_set_fr_bdb_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
          if (is_ocb_only) {
              info->ocb_bdbs[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
          } else {
              info->bdbs[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
          }
      }
  }
  /*
   *  BD-s (Buffer Descriptors)
   */
  bds_th_mnt_nof_bits = 8;
  for (is_ocb_only = 0; is_ocb_only < 2; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          res = soc_reg_above_64_field32_read(
                    unit,
                    IQM_GENERAL_REJECT_CONFIGURATION_B_DSr,
                    core_id,
                    0,
                    rjct_clr_oc_bd_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, bds_th_mnt_nof_bits, &mnt_val, &exp_val);
          if (is_ocb_only) {
              info->ocb_bds[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
          } else {
              info->bds[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
          }
          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_B_DSr, 
                    core_id, 
                    0, 
                    rjct_set_oc_bd_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, bds_th_mnt_nof_bits, &mnt_val, &exp_val);
          if (is_ocb_only) {
              info->ocb_bds[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
          } else {
              info->bds[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
          }
      }
  }
  /*
    info->mini_mc[index].clear
    info->mini_mc[index].set
  */
  mini_mc_mnt_nof_bits = 8;
  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_reg_above_64_field32_read(
                unit, 
                IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, 
                core_id, 
                0, 
                rjct_clr_fr_bd_mmc_th_fields[drop_precedence_ndx],  
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mini_mc_mnt_nof_bits, &mnt_val, &exp_val);
      info->mini_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));

      res = soc_reg_above_64_field32_read(
                unit, 
                IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, 
                core_id, 
                0, 
                rjct_set_fr_bd_mmc_th_fields[drop_precedence_ndx],
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mini_mc_mnt_nof_bits, &mnt_val, &exp_val);
      info->mini_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
  }
  /*
    info->full_mc[index].clear
    info->full_mc[index].set
   */
  fmc_th_mnt_nof_bits = 8;
  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_reg_above_64_field32_read(
                unit,
                IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr,
                core_id,
                0,
                rjct_clr_fr_bd_flmc_th_fields[drop_precedence_ndx],
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mini_mc_mnt_nof_bits, &mnt_val, &exp_val);
      info->mini_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));

      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, fmc_th_mnt_nof_bits, &exp_man);
      res = soc_reg_above_64_field32_read(
                unit,
                IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr,
                core_id,
                0,
                rjct_set_fr_bd_flmc_th_fields[drop_precedence_ndx],
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mini_mc_mnt_nof_bits, &mnt_val, &exp_val);
      info->full_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
  }
  /* 
    info->ocb_uc[index].set 
    info->ocb_uc[index].clear
    setting ocb unicast thresholds registers 
   */
  uc_ocb_mnt_nof_bits = 8;
  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_reg_above_64_field32_read(
                unit, 
                IQM_REJECT_CONFIGURATION_OCBSr, 
                core_id, 
                0, 
                rjct_set_fr_ocb_th_fields[drop_precedence_ndx], 
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, uc_ocb_mnt_nof_bits, &mnt_val, &exp_val);
      info->ocb_uc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));

      res = soc_reg_above_64_field32_read(
                unit, 
                IQM_REJECT_CONFIGURATION_OCBSr, 
                core_id, 
                0, 
                rjct_clr_fr_ocb_th_fields[drop_precedence_ndx], 
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, uc_ocb_mnt_nof_bits, &mnt_val, &exp_val);
      info->ocb_uc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
  }
  /* 
   * info->ocb_mc[index].set 
   * info->ocb_mc[index].clear
   * setting ocb multicast thresholds registers
   *  Extension for Arad-B
   *  Excess memory size
   */
  dyn_mnt_nof_bits = 8;
  for (is_ocb_only = 0; is_ocb_only < 2; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                    core_id, 
                    0, 
                    rjct_dyn_size_clr_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx], 
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, dyn_mnt_nof_bits, &mnt_val, &exp_val);

          if (is_ocb_only) {
              info->ocb_mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
          } else {
              info->mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
          }

          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                    core_id, 
                    0, 
                    rjct_dyn_size_set_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx], 
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, dyn_mnt_nof_bits, &mnt_val, &exp_val);

          if (is_ocb_only) {
              info->ocb_mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
          } else {
              info->mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
          }
      }
  }

exit: 
    SOCDNX_FUNC_RETURN; 
}                       

uint32
  jer_itm_glob_rcs_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *exact_info
  )
{
    uint32
        res;
    soc_reg_t 
        shared_pool_reg = 0;
    soc_field_t 
        shared_pool_field_set = 0,
        shared_pool_field_clr = 0;
    soc_reg_above_64_val_t 
        shared_pool_data;
    uint8 
        is_ocb_only = FALSE;
    uint32
        drop_precedence_ndx;
    uint8
        pool_id;
    const soc_field_t 
        rjct_set_fr_bdb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_RJCT_SET_FR_BDB_TH_0f, 
            MIXED_RJCT_SET_FR_BDB_TH_1f, 
            MIXED_RJCT_SET_FR_BDB_TH_2f, 
            MIXED_RJCT_SET_FR_BDB_TH_3f,
            OCB_RJCT_SET_FR_BDB_TH_0f, 
            OCB_RJCT_SET_FR_BDB_TH_1f, 
            OCB_RJCT_SET_FR_BDB_TH_2f, 
            OCB_RJCT_SET_FR_BDB_TH_3f};
    const soc_field_t 
        rjct_clr_fr_bdb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_RJCT_CLR_FR_BDB_TH_0f, 
            MIXED_RJCT_CLR_FR_BDB_TH_1f, 
            MIXED_RJCT_CLR_FR_BDB_TH_2f, 
            MIXED_RJCT_CLR_FR_BDB_TH_4f,
            OCB_RJCT_CLR_FR_BDB_TH_0f, 
            OCB_RJCT_CLR_FR_BDB_TH_1f, 
            OCB_RJCT_CLR_FR_BDB_TH_2f, 
            OCB_RJCT_CLR_FR_BDB_TH_4f};
    const soc_field_t 
        rjct_set_oc_bd_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_RJCT_SET_OC_BD_TH_0f,
            MIXED_RJCT_SET_OC_BD_TH_1f,
            MIXED_RJCT_SET_OC_BD_TH_2f,
            MIXED_RJCT_SET_OC_BD_TH_3f,
            OCB_RJCT_SET_OC_BD_TH_0f,
            OCB_RJCT_SET_OC_BD_TH_1f,
            OCB_RJCT_SET_OC_BD_TH_2f,
            OCB_RJCT_SET_OC_BD_TH_3f};
    const soc_field_t 
        rjct_clr_oc_bd_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_RJCT_CLR_OC_BD_TH_0f, 
            MIXED_RJCT_CLR_OC_BD_TH_1f, 
            MIXED_RJCT_CLR_OC_BD_TH_2f, 
            MIXED_RJCT_CLR_OC_BD_TH_3f,
            OCB_RJCT_CLR_OC_BD_TH_0f, 
            OCB_RJCT_CLR_OC_BD_TH_1f, 
            OCB_RJCT_CLR_OC_BD_TH_2f, 
            OCB_RJCT_CLR_OC_BD_TH_3f};
    const soc_field_t 
        rjct_clr_fr_bd_mmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_CLR_FR_DB_MNMC_TH_0f,
            RJCT_CLR_FR_DB_MNMC_TH_1f,
            RJCT_CLR_FR_DB_MNMC_TH_2f,
            RJCT_CLR_FR_DB_MNMC_TH_3f};
    const soc_field_t
        rjct_set_fr_bd_mmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_SET_FR_DB_MNMC_TH_0f,
            RJCT_SET_FR_DB_MNMC_TH_1f,
            RJCT_SET_FR_DB_MNMC_TH_2f,
            RJCT_SET_FR_DB_MNMC_TH_3f};
    const soc_field_t 
        rjct_clr_fr_bd_flmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_CLR_FR_DB_FLMC_TH_0f,
            RJCT_CLR_FR_DB_FLMC_TH_1f,
            RJCT_CLR_FR_DB_FLMC_TH_2f,
            RJCT_CLR_FR_DB_FLMC_TH_3f};
    const soc_field_t 
        rjct_set_fr_bd_flmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_SET_FR_DB_FLMC_TH_0f,
            RJCT_SET_FR_DB_FLMC_TH_1f,
            RJCT_SET_FR_DB_FLMC_TH_2f,
            RJCT_SET_FR_DB_FLMC_TH_3f};
    const soc_field_t 
        rjct_dyn_size_clr_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_0f,
            MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_1f,
            MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_2f,
            MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_3f,
            OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_0f,
            OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_1f,
            OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_2f,
            OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_3f};
    const soc_field_t 
        rjct_dyn_size_set_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {
            MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_0f,
            MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_1f,
            MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_2f,
            MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_3f,
            OCB_DYN_WORDS_SIZE_RJCT_SET_TH_0f,
            OCB_DYN_WORDS_SIZE_RJCT_SET_TH_1f,
            OCB_DYN_WORDS_SIZE_RJCT_SET_TH_2f,
            OCB_DYN_WORDS_SIZE_RJCT_SET_TH_3f};
    const soc_field_t 
        rjct_set_fr_ocb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_SET_FR_OCB_TH_0f,
            RJCT_SET_FR_OCB_TH_1f,
            RJCT_SET_FR_OCB_TH_2f,
            RJCT_SET_FR_OCB_TH_3f};
    soc_field_t 
        rjct_clr_fr_ocb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {
            RJCT_CLR_FR_OCB_TH_0f,
            RJCT_CLR_FR_OCB_TH_1f,
            RJCT_CLR_FR_OCB_TH_2f,
            RJCT_CLR_FR_OCB_TH_3f};
    SOC_TMC_THRESH_WITH_HYST_INFO 
        shrd_pool_th;
    uint32
      exp_man,
      bdbs_th_mnt_nof_bits,
      bdbs_th_exp_nof_bits,
      bds_th_mnt_nof_bits,
      bds_th_exp_nof_bits,
      mini_mc_mnt_nof_bits,
      mini_mc_exp_nof_bits,
      fmc_th_mnt_nof_bits,
      fmc_th_exp_nof_bits,
      uc_ocb_mnt_nof_bits,
      uc_ocb_exp_nof_bits,
      dyn_mnt_nof_bits,
      dyn_exp_nof_bits;
    uint32
      mnt_val,
      exp_val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    /*Verifying that the input is in range*/
    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (pool_id = 0; pool_id < SOC_TMC_ITM_NOF_RSRC_POOLS; pool_id++) {
            for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
                if (is_ocb_only == TRUE) {
                    if (info->ocb_shrd_pool[pool_id][drop_precedence_ndx].clear > SOC_TMC_ITM_SHRD_RJCT_TH_MAX) {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    } 
                    if (info->ocb_shrd_pool[pool_id][drop_precedence_ndx].set > SOC_TMC_ITM_SHRD_RJCT_TH_MAX) {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                } else {
                    if (info->mix_shrd_pool[pool_id][drop_precedence_ndx].clear > SOC_TMC_ITM_SHRD_RJCT_TH_MAX) {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                    if (info->mix_shrd_pool[pool_id][drop_precedence_ndx].set > SOC_TMC_ITM_SHRD_RJCT_TH_MAX) {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                }
            }
        }
    }
    /*
     *  BDB-s (Buffer Descriptor Buffers)
     */
    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
            if (is_ocb_only) {
                if (info->ocb_bdbs[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
                if (info->ocb_bdbs[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
            } else {
                if (info->bdbs[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
                if (info->bdbs[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
            }
        }
    }
   /*
    *  BD-s (Buffer Descriptors)
    */
    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
            if (is_ocb_only) {
                /*
                info->ocb_bds[drop_precedence_ndx].clear;
                info->ocb_bds[drop_precedence_ndx].set; 
                */
            } else {
                /*
                info->bds[drop_precedence_ndx].clear;
                info->bds[drop_precedence_ndx].set; 
                */
            }
        }
    }
    /*
      info->mini_mc[index].clear
      info->mini_mc[index].set
    */
    for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
        if (info->mini_mc[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (info->mini_mc[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    /*
    info->full_mc[index].clear
    info->full_mc[index].set
    */
    for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
        if (info->full_mc[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (info->full_mc[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    /* 
    info->ocb_uc[index].set 
    info->ocb_uc[index].clear
    setting ocb unicast thresholds registers 
   */
    for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
        if (info->ocb_uc[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (info->ocb_uc[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX) {       
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
  /* 
   * info->ocb_mc[index].set 
   * info->ocb_mc[index].clear
   * setting ocb multicast thresholds registers
   *  Extension for Arad-B
   *  Excess memory size
   */
    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
            dyn_mnt_nof_bits = 8;
            dyn_exp_nof_bits = 
                soc_reg_field_length(
                   unit, 
                   IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                   rjct_dyn_size_set_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx]) - dyn_mnt_nof_bits;
            if (is_ocb_only) {
                shrd_pool_th.clear = info->ocb_mem_excess[drop_precedence_ndx].clear;
                shrd_pool_th.set =   info->ocb_mem_excess[drop_precedence_ndx].set;
            } else {
                shrd_pool_th.clear = info->mem_excess[drop_precedence_ndx].clear;
                shrd_pool_th.set =   info->mem_excess[drop_precedence_ndx].set;
            }
        }
    }
    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (pool_id = 0; pool_id < SOC_TMC_ITM_NOF_RSRC_POOLS; pool_id++) {
            for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
                if (is_ocb_only == TRUE) {
                    shrd_pool_th.clear = info->ocb_shrd_pool[pool_id][drop_precedence_ndx].clear;
                    shrd_pool_th.set = info->ocb_shrd_pool[pool_id][drop_precedence_ndx].set;
                    if (pool_id == 0) {
                        shared_pool_reg = IQM_OCB_SHRD_POOL_0_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                    if (pool_id == 1) {
                        shared_pool_reg = IQM_OCB_SHRD_POOL_1_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                } else {
                    shrd_pool_th.clear = info->mix_shrd_pool[pool_id][drop_precedence_ndx].clear;
                    shrd_pool_th.set = info->mix_shrd_pool[pool_id][drop_precedence_ndx].set;
                    if (pool_id == 0) {
                        shared_pool_reg = IQM_MIX_SHRD_POOL_0_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                    if (pool_id == 1) {
                        shared_pool_reg = IQM_MIX_SHRD_POOL_1_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_3f;
                        }
                    }
                }
                res = soc_reg_above_64_get(unit, shared_pool_reg, core_id, 0, shared_pool_data);
                SOCDNX_IF_ERR_EXIT(res);

                soc_reg_above_64_field32_set(unit, shared_pool_reg, shared_pool_data, shared_pool_field_set, shrd_pool_th.set);
                soc_reg_above_64_field32_set(unit, shared_pool_reg, shared_pool_data, shared_pool_field_clr, shrd_pool_th.clear);

                res = soc_reg_above_64_set(unit, shared_pool_reg, core_id, 0, shared_pool_data);
                SOCDNX_IF_ERR_EXIT(res);
            }
        }
    }
  /*
   *  BDB-s (Buffer Descriptor Buffers)
   */
  bdbs_th_mnt_nof_bits = 8;
  bdbs_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, rjct_set_fr_bdb_th_fields[0]) - bdbs_th_mnt_nof_bits;
  for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_bdbs[drop_precedence_ndx].clear, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_bdbs[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bdbs_th_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->bdbs[drop_precedence_ndx].clear, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->bdbs[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bdbs_th_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, 
                    core_id, 
                    0, 
                    rjct_clr_fr_bdb_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_bdbs[drop_precedence_ndx].set, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_bdbs[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bdbs_th_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->bdbs[drop_precedence_ndx].set, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->bdbs[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bdbs_th_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, 
                    core_id, 
                    0, 
                    rjct_set_fr_bdb_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
      }
  }
  

  /*
   *  BD-s (Buffer Descriptors)
   */
  bds_th_mnt_nof_bits = 8;
  bds_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DSr, rjct_clr_oc_bd_th_fields[0]) - bds_th_mnt_nof_bits;
  for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_bds[drop_precedence_ndx].clear, bds_th_mnt_nof_bits, bds_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_bds[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bds_th_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->bds[drop_precedence_ndx].clear, bds_th_mnt_nof_bits, bds_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->bds[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bds_th_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit,
                    IQM_GENERAL_REJECT_CONFIGURATION_B_DSr,
                    core_id,
                    0,
                    rjct_clr_oc_bd_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_bds[drop_precedence_ndx].set, bds_th_mnt_nof_bits, bds_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_bds[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bds_th_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->bds[drop_precedence_ndx].set, bds_th_mnt_nof_bits, bds_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->bds[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bds_th_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_B_DSr, 
                    core_id, 
                    0, 
                    rjct_set_oc_bd_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
      }
  }
  /*
    info->mini_mc[index].clear
    info->mini_mc[index].set
  */

  mini_mc_mnt_nof_bits = 8;
  mini_mc_exp_nof_bits = soc_reg_field_length(unit, IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, rjct_clr_fr_bd_mmc_th_fields[0]) - mini_mc_mnt_nof_bits;

  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_sand_break_to_mnt_exp_round_up(info->mini_mc[drop_precedence_ndx].clear, mini_mc_mnt_nof_bits, mini_mc_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->mini_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));

      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, mini_mc_mnt_nof_bits, &exp_man);
      res = soc_reg_above_64_field32_modify(
                unit, 
                IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, 
                core_id, 
                0, 
                rjct_clr_fr_bd_mmc_th_fields[drop_precedence_ndx],  
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      
      res = soc_sand_break_to_mnt_exp_round_up(info->mini_mc[drop_precedence_ndx].set, mini_mc_mnt_nof_bits, mini_mc_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->mini_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));

      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, mini_mc_mnt_nof_bits, &exp_man);
      res = soc_reg_above_64_field32_modify(
                unit, 
                IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, 
                core_id, 
                0, 
                rjct_set_fr_bd_mmc_th_fields[drop_precedence_ndx],
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);
  }

  /*
    info->full_mc[index].clear
    info->full_mc[index].set
   */

  fmc_th_mnt_nof_bits = 8;
  fmc_th_exp_nof_bits = soc_reg_field_length(unit, IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr, rjct_clr_fr_bd_flmc_th_fields[0]) - fmc_th_mnt_nof_bits;

  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_sand_break_to_mnt_exp_round_up(info->full_mc[drop_precedence_ndx].clear, fmc_th_mnt_nof_bits, fmc_th_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->full_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, fmc_th_mnt_nof_bits, &exp_man);

      res = soc_reg_above_64_field32_modify(
                unit,
                IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr,
                core_id,
                0,
                rjct_clr_fr_bd_flmc_th_fields[drop_precedence_ndx],
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);

      res = soc_sand_break_to_mnt_exp_round_up(info->full_mc[drop_precedence_ndx].set, fmc_th_mnt_nof_bits, fmc_th_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->full_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));

      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, fmc_th_mnt_nof_bits, &exp_man);
      res = soc_reg_above_64_field32_modify(
                unit,
                IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr,
                core_id,
                0,
                rjct_set_fr_bd_flmc_th_fields[drop_precedence_ndx],
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);

  }
  /* 
    info->ocb_uc[index].set 
    info->ocb_uc[index].clear
    setting ocb unicast thresholds registers 
   */
  uc_ocb_mnt_nof_bits = 8;
  uc_ocb_exp_nof_bits = soc_reg_field_length(unit, IQM_REJECT_CONFIGURATION_OCBSr, rjct_set_fr_ocb_th_fields[0]) - uc_ocb_mnt_nof_bits;
  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_sand_break_to_mnt_exp_round_up(info->ocb_uc[drop_precedence_ndx].set, uc_ocb_mnt_nof_bits, uc_ocb_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->ocb_uc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, uc_ocb_mnt_nof_bits, &exp_man);

      res = soc_reg_above_64_field32_modify(
                unit, 
                IQM_REJECT_CONFIGURATION_OCBSr, 
                core_id, 
                0, 
                rjct_set_fr_ocb_th_fields[drop_precedence_ndx], 
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);

      res = soc_sand_break_to_mnt_exp_round_up(info->ocb_uc[drop_precedence_ndx].clear, uc_ocb_mnt_nof_bits, uc_ocb_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->ocb_uc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, uc_ocb_mnt_nof_bits, &exp_man);

      res = soc_reg_above_64_field32_modify(
                unit, 
                IQM_REJECT_CONFIGURATION_OCBSr, 
                core_id, 
                0, 
                rjct_clr_fr_ocb_th_fields[drop_precedence_ndx], 
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);
  }
  /* 
   * info->ocb_mc[index].set 
   * info->ocb_mc[index].clear
   * setting ocb multicast thresholds registers
   *  Extension for Arad-B
   *  Excess memory size
   */

  for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          dyn_mnt_nof_bits = 8;
          dyn_exp_nof_bits = 
              soc_reg_field_length(
                 unit, 
                 IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                 rjct_dyn_size_set_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx]) - dyn_mnt_nof_bits;

          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_mem_excess[drop_precedence_ndx].clear, dyn_mnt_nof_bits, dyn_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, dyn_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->mem_excess[drop_precedence_ndx].clear, dyn_mnt_nof_bits, dyn_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, dyn_mnt_nof_bits, &exp_man);
          }

          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                    core_id, 
                    0, 
                    rjct_dyn_size_clr_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx], 
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);

          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_mem_excess[drop_precedence_ndx].set, dyn_mnt_nof_bits, dyn_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_mem_excess[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, dyn_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->mem_excess[drop_precedence_ndx].set, dyn_mnt_nof_bits, dyn_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->mem_excess[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, dyn_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                    core_id, 
                    0, 
                    rjct_dyn_size_set_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx], 
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
      }
  }

exit: 
    SOCDNX_FUNC_RETURN; 
}

uint32
jer_itm_vsq_src_reserve_set(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP   vsq_type,
   SOC_SAND_IN  uint32                  vsq_rt_cls_ndx,
   SOC_SAND_IN  uint32                  drop_precedence_ndx,
   SOC_SAND_IN  int                     pool_id,
   SOC_SAND_IN  uint32                  reserved_amount,
   SOC_SAND_OUT uint32*                 exact_reserved_amount
   ) 
{
    uint32 
        entry_offset,
        res,
        data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    uint32
        exp_man,
        max_size_mnt_nof_bits,
        max_size_exp_nof_bits,
        max_size_mnt,
        max_size_exp;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(exact_reserved_amount);
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (reserved_amount > JER_ITM_VSQ_GRNT_BD_SIZE_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    max_size_mnt_nof_bits = 8;
    max_size_exp_nof_bits = soc_mem_field_length(unit, IQM_VSQE_PRMm, VQ_MIN_THf) - max_size_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(reserved_amount, max_size_mnt_nof_bits, max_size_exp_nof_bits, 0, &max_size_mnt, &max_size_exp);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    *exact_reserved_amount = (max_size_mnt * (1 << max_size_exp));
    arad_iqm_mantissa_exponent_set(unit, max_size_mnt, max_size_exp, max_size_mnt_nof_bits, &exp_man);

    /*PORT RESERVE:*/
    if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        res = READ_IQM_VSQE_PRMm(unit, pool_id, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_IQM_VSQE_PRMm_field32_set(unit, data, VQ_MIN_THf, reserved_amount);

        res = WRITE_IQM_VSQE_PRMm(unit, pool_id, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);
    }
    /*PG RESERVE:*/
    if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_PG) {
        res = READ_IQM_VSQF_PRMm(unit, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_IQM_VSQF_PRMm_field32_set(unit, data, VQ_MIN_THf, reserved_amount);

        res = WRITE_IQM_VSQF_PRMm(unit, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);
    } else {
        SOCDNX_IF_ERR_EXIT(res);
    }

    *exact_reserved_amount = reserved_amount;
exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
jer_itm_vsq_src_reserve_get(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP   vsq_type,
   SOC_SAND_IN  uint32                  vsq_rt_cls_ndx,
   SOC_SAND_IN  uint32                  drop_precedence_ndx,
   SOC_SAND_IN  int                     pool_id,
   SOC_SAND_OUT uint32                  *reserved_amount
   ) 
{
    uint32 
        entry_offset,
        res,
        data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    uint32
        exp_man,
        max_size_mnt,
        max_size_exp;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reserved_amount);
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    /*PORT RESERVE*/
    if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        res = READ_IQM_VSQE_PRMm(unit, pool_id, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);

        exp_man = soc_IQM_VSQE_PRMm_field32_get(unit, data, VQ_MIN_THf);
    } else if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_PG) {
        /*PG RESERVE*/
        res = READ_IQM_VSQF_PRMm(unit, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);

        exp_man = soc_IQM_VSQF_PRMm_field32_get(unit, data, VQ_MIN_THf);
    } else {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    arad_iqm_mantissa_exponent_get(unit, exp_man, 8, &max_size_mnt, &max_size_exp);
    *reserved_amount = (max_size_mnt * (1 << max_size_exp));

exit: 
    SOCDNX_FUNC_RETURN;
}
uint32
jer_itm_resource_allocation_set(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  uint8                   pool_id,
   SOC_SAND_IN  uint8                   is_ocb_only,
   SOC_SAND_IN  uint32                  max_shared_pool,
   SOC_SAND_IN  uint32                  max_headroom
   ) 
{
    uint32
        res;
    soc_reg_above_64_val_t 
        glbl_ocb_th_data;
    uint64 
        glbl_th_data;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id != SOC_CORE_ALL && 
       (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (max_shared_pool > SOC_TMC_ITM_RESOURCE_ALLOCATION_SHARED_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (max_headroom > SOC_TMC_ITM_RESOURCE_ALLOCATION_SHARED_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (max_shared_pool > JER_ITM_VSQ_GRNT_BD_SIZE_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (max_headroom > JER_ITM_VSQ_GRNT_BD_SIZE_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (is_ocb_only) {
        COMPILER_64_ZERO(glbl_th_data);
        res = READ_IQM_SRC_VSQ_GLBL_OCB_MAX_THr(unit, core_id, &glbl_th_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (pool_id == 0) {
            soc_reg64_field32_set(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, &glbl_th_data, GLBL_OCB_SHRD_POOL_0_MAX_THf, max_shared_pool);
        } else if (pool_id == 1) {
            soc_reg64_field32_set(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, &glbl_th_data, GLBL_OCB_SHRD_POOL_1_MAX_THf, max_shared_pool);
        }
        soc_reg64_field32_set(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, &glbl_th_data, GLBL_OCB_HDRM_MAX_THf, max_headroom);
        res = WRITE_IQM_SRC_VSQ_GLBL_OCB_MAX_THr(unit, core_id, glbl_th_data);
        SOCDNX_IF_ERR_EXIT(res);
    } else {
        res = READ_IQM_GLBL_MIX_MAX_THr(unit, core_id, glbl_ocb_th_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (pool_id == 0) {
            soc_reg_above_64_field32_set(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_SHRD_POOL_0_MAX_THf, max_shared_pool);
        } else if (pool_id == 1) {
            soc_reg_above_64_field32_set(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_SHRD_POOL_1_MAX_THf, max_shared_pool);
        }
        soc_reg_above_64_field32_set(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_HDRM_MAX_THf, max_headroom);

        res = WRITE_IQM_GLBL_MIX_MAX_THr(unit, core_id, glbl_ocb_th_data);
        SOCDNX_IF_ERR_EXIT(res);
    }

    SOCDNX_IF_ERR_EXIT(res);
exit: 
    SOCDNX_FUNC_RETURN;
}
uint32
jer_itm_resource_allocation_get(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  uint8                   pool_id,
   SOC_SAND_IN  uint8                   is_ocb_only,
   SOC_SAND_OUT uint32                  *max_shared_pool,
   SOC_SAND_OUT uint32                  *max_headroom
   ) 
{
    uint32
        res = BCM_E_NONE;
    soc_reg_above_64_val_t 
        glbl_ocb_th_data;
    uint64 
        glbl_th_data;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(max_shared_pool);
    SOCDNX_NULL_CHECK(max_headroom);
    if (core_id != SOC_CORE_ALL && 
       (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (is_ocb_only) {
        COMPILER_64_ZERO(glbl_th_data);
        res = READ_IQM_SRC_VSQ_GLBL_OCB_MAX_THr(unit, core_id, &glbl_th_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (pool_id == 0) {
            *max_shared_pool = soc_reg64_field32_get(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, glbl_th_data, GLBL_OCB_SHRD_POOL_0_MAX_THf);
        } else if (pool_id == 1) {
            *max_shared_pool = soc_reg64_field32_get(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, glbl_th_data, GLBL_OCB_SHRD_POOL_1_MAX_THf);
        }
        *max_headroom = soc_reg64_field32_get(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, glbl_th_data, GLBL_OCB_HDRM_MAX_THf);
    } else {
        res = READ_IQM_GLBL_MIX_MAX_THr(unit, core_id, glbl_ocb_th_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (pool_id == 0) {
            *max_shared_pool = soc_reg_above_64_field32_get(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_SHRD_POOL_0_MAX_THf);
        } else if (pool_id == 1) {
            *max_shared_pool = soc_reg_above_64_field32_get(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_SHRD_POOL_1_MAX_THf);
        }
        *max_headroom = soc_reg_above_64_field32_get(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_HDRM_MAX_THf);
    }

exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_itm_dyn_total_thresh_set(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int32 reservation_increase /* the (signed) amount in which the thresholds should decrease (according to 100% as will be set for DP 0) */
  )
{
    uint32
        res;
    soc_dpp_guaranteed_q_resource_t guaranteed_q_resource;
    SOCDNX_INIT_FUNC_DEFS;

    if (reservation_increase < 0 &&  ((uint32)(-reservation_increase)) > SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource.used) {
        SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE, &guaranteed_q_resource);
    SOCDNX_IF_ERR_EXIT(res);
  
    /* update the amount of the resource that is left */
    if (reservation_increase) {
        int32 resource_left_calc =  ((int32)(guaranteed_q_resource.total -
                                             (guaranteed_q_resource.used))) - reservation_increase;
        uint32 resource_left =  resource_left_calc;
        if (resource_left_calc < 0) { 
            /* check if we are out of the resource */
            SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }

        if (SOC_DPP_CONFIG(unit)->tm.guaranteed_q_mode == SOC_DPP_GUARANTEED_Q_RESOURCE_MEMORY) {
            

        } else {
            soc_reg_above_64_val_t reg_above_64_val;

            uint32 reg_dp0 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
            uint32 reg_dp1 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
            uint32 reg_dp2 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
            uint32 reg_dp3 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
            uint32 mantissa, exponent, field_val;

            SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
      
            if (resource_left) { /* configure drop thresholds according to new amount of resource left */
                
                res = soc_sand_break_to_mnt_exp_round_down(
                        resource_left,
                        ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS,
                        ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
                        0,
                        &mantissa, 
                        &exponent);
                SOCDNX_SAND_IF_ERR_EXIT(res);
                field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
            } else {
                field_val = reg_dp0;
            }
            soc_reg_above_64_field32_set(unit, IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, reg_above_64_val, DRAM_BUFF_DYN_SIZE_RJCT_SET_TH_0f, field_val);
            soc_reg_above_64_field32_set(unit, IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, reg_above_64_val, DRAM_BUFF_DYN_SIZE_RJCT_CLR_TH_0f, field_val);
            if (resource_left) {
                res = soc_sand_break_to_mnt_exp_round_down(
                        (resource_left / 20) * 17, /* 85% */
                        ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, 
                        ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
                        0,
                        &mantissa, 
                        &exponent);
                SOCDNX_SAND_IF_ERR_EXIT(res);
                field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
            } else {
                field_val = reg_dp1;
            }
            soc_reg_above_64_field32_set(unit, IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, reg_above_64_val, DRAM_BUFF_DYN_SIZE_RJCT_SET_TH_1f, field_val);
            soc_reg_above_64_field32_set(unit, IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, reg_above_64_val, DRAM_BUFF_DYN_SIZE_RJCT_CLR_TH_1f, field_val);
            if (resource_left) {
                res = soc_sand_break_to_mnt_exp_round_down(
                        (resource_left / 20) * 15, /* 75% */
                        ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, 
                        ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
                        0, 
                        &mantissa, 
                        &exponent);
                SOCDNX_SAND_IF_ERR_EXIT(res);
                field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
            } else {
                field_val = reg_dp2;
            }
            soc_reg_above_64_field32_set(unit, IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, reg_above_64_val, DRAM_BUFF_DYN_SIZE_RJCT_SET_TH_2f, field_val);
            soc_reg_above_64_field32_set(unit, IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, reg_above_64_val, DRAM_BUFF_DYN_SIZE_RJCT_CLR_TH_2f, field_val);
            if (resource_left) {
                res = soc_sand_break_to_mnt_exp_round_down(
                        0, /* 0% */
                        ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, 
                        ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
                        0, 
                        &mantissa, 
                        &exponent);
                SOCDNX_SAND_IF_ERR_EXIT(res);
                field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
            } else {
                field_val = reg_dp3;
            }
            soc_reg_above_64_field32_set(unit, IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, reg_above_64_val, DRAM_BUFF_DYN_SIZE_RJCT_SET_TH_3f, field_val);
            soc_reg_above_64_field32_set(unit, IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, reg_above_64_val, DRAM_BUFF_DYN_SIZE_RJCT_CLR_TH_3f, field_val);

            res = WRITE_IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr(unit, SOC_CORE_ALL, reg_above_64_val);
            SOCDNX_IF_ERR_EXIT(res);
        }
        guaranteed_q_resource.used += reservation_increase;
        res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE, &guaranteed_q_resource); /* update warm boot data */
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }
exit:
  SOCDNX_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME
#endif /* of #if defined(BCM_88675_A0) */
#include <soc/dpp/SAND/Utils/sand_footer.h> 

