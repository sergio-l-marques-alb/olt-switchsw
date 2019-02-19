#include <shared/bsl.h>
#include <soc/mcm/memregs.h> 
#if defined(BCM_88675_A0)
/* $Id: jer_ingress_traffic_mgmt.c,v 1.85 Broadcom SDK $
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

#define JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE 4
/* (2^22 - 1) */
#define JER_ITM_VSQ_FC_BD_SIZE_MAX              0X3FFFFF


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

  for (vsq_group = 0; vsq_group < ARAD_ITM_VSQ_GROUP_LAST; vsq_group++) 
  {
       res = jer_itm_vsq_fc_set(unit, vsq_group, 0, /* Default VSQ rate class */ &vsq_fc_info, &exact_vsq_fc_info);
       SOCDNX_IF_ERR_EXIT(res);
  }
 
  /* set the total of the gaurenteed VOQ resorce */
  
  SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource.total = pdm_nof_entries; 
  res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE, &(SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource)); /* update warm boot data */
  SOCDNX_IF_ERR_EXIT(res);


  
  /*res = arad_itm_setup_dp_map(unit);*/
  /*SOCDNX_IF_ERR_EXIT(res);*/


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
    SOC_SAND_IN  int                 core_id, 
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
    tmp_core;  
  SOCDNX_INIT_FUNC_DEFS; 
  SOCDNX_NULL_CHECK(info); 
  if (core_id == 0 || core_id == SOC_CORE_ALL) { 
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
  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm); 
  SOCDNX_IF_ERR_EXIT(res); 
 
  SOC_PBMP_ITER(ports_bm, port)  
  {
      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
      if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags))) {

          res = soc_port_sw_db_local_to_tm_port_get(unit, port, &dsp_pp, &tmp_core); 
          SOCDNX_IF_ERR_EXIT(res); 
     
          res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, tmp_core, dsp_pp, &nof_prio); 
          SOCDNX_IF_ERR_EXIT(res); 
     
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
    if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
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
    if (core_id == 0 || core_id == SOC_CORE_ALL) { 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_LOW_0f, q_low);  
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_0f, q_hi); 
        SOCDNX_IF_ERR_EXIT(res); 
    } 
    if (core_id == 0 || core_id == SOC_CORE_ALL) { 
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
    jer_itm_vsq_wred_get(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info) 
{
    uint32  
        res = SOC_E_NONE;
    uint32
      entry_offset,
      data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 

  /* since IQM_VQPR_M[A-F] have the same fields in hardware, but currently in the description
     IQM_VQPR_M[E-F] have different fields, we use mem_arr_IQM_VQPR_M[group_id] for memory
     access and IQM_VQPR_MAm for filed access */
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], MEM_BLOCK_ANY, entry_offset, data);
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
        SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO *info,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *exact_info)
{
    uint32  
        res = SOC_E_NONE;
    uint32
      entry_offset,
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
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], MEM_BLOCK_ANY, entry_offset, data);
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

    res = soc_mem_write(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], MEM_BLOCK_ANY, entry_offset, data);
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
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB,
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
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
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
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
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB,
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
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
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
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
    SOC_SAND_IN  int                 unit,
    /*SOC_SAND_IN  uint8               is_ocb_only,*/
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  )
{
    uint32  
        res = SOC_E_NONE;
    uint32
        exp_man,
        entry_offset,
        data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    const soc_field_t field_arr_BUFF_SIZE[SOC_TMC_NOF_VSQ_GROUPS] = {VQ_MAX_QUE_BUFF_SIZEf, VQ_MAX_QUE_BUFF_SIZEf, VQ_MAX_QUE_BUFF_SIZEf, VQ_MAX_QUE_BUFF_SIZEf, VQ_MAX_BUFF_SHARED_SIZEf, VQ_MAX_BUFF_SHARED_SIZEf};
    uint32
        max_inst_q_siz_bds_mnt_nof_bits,
        max_inst_q_siz_bds_exp_nof_bits,
        vq_max_szie_bds_mnt,
        vq_max_szie_bds_exp;
    /* 
    int 
        ocb_offset = 0;
    */
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);
    exact_info->max_inst_q_size = info->max_inst_q_size_bds;

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    /*
    if (is_ocb_only) {
        ocb_offset = SOC_TMC_NOF_DROP_PRECEDENCE + SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX;
    } 
    */ 
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);

    max_inst_q_siz_bds_mnt_nof_bits = 7;
    max_inst_q_siz_bds_exp_nof_bits = soc_mem_field_length(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], field_arr_BUFF_SIZE[vsq_group_ndx]) - max_inst_q_siz_bds_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(
            info->max_inst_q_size_bds,
            max_inst_q_siz_bds_mnt_nof_bits,
            max_inst_q_siz_bds_exp_nof_bits,
            0,
            &(vq_max_szie_bds_mnt),
            &(vq_max_szie_bds_exp)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if(vq_max_szie_bds_mnt == 0)
    {
      exact_info->max_inst_q_size_bds = 0;
    } 
    else {
      exact_info->max_inst_q_size_bds = ((vq_max_szie_bds_mnt) * (1<<(vq_max_szie_bds_exp)));
    }

    arad_iqm_mantissa_exponent_set(unit, vq_max_szie_bds_mnt, vq_max_szie_bds_exp, max_inst_q_siz_bds_mnt_nof_bits, &exp_man);   
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, field_arr_BUFF_SIZE[vsq_group_ndx], exp_man); 

    res = soc_mem_write(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], MEM_BLOCK_ANY, entry_offset, data);
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

    
    info->max_inst_q_size = (0x1 << soc_mem_field_length(unit, IQM_VSQA_PRMm, SET_THRESHOLD_BDSf)) - 1;
    info->max_inst_q_size_bds = (0x1 << soc_mem_field_length(unit, IQM_VSQA_PRMm, CLR_THRESHOLD_BDSf)) - 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_itm_vsq_tail_drop_get_unsafe()",0,0);
}

uint32
  jer_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    uint32  
        res = SOC_E_NONE;
    uint32
        exp_man;
    uint32
      entry_offset,
      data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    const soc_field_t field_arr_BUFF_SIZE[SOC_TMC_NOF_VSQ_GROUPS] = {VQ_MAX_QUE_BUFF_SIZEf, VQ_MAX_QUE_BUFF_SIZEf, VQ_MAX_QUE_BUFF_SIZEf, VQ_MAX_QUE_BUFF_SIZEf, VQ_MAX_BUFF_SHARED_SIZEf, VQ_MAX_BUFF_SHARED_SIZEf};
    SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  tbl_data;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    /* since IQM_VQPR_M[A-F] have the same fields in hardware, but currently in the description
     IQM_VQPR_M[E-F] have different fields, we use mem_arr_IQM_VQPR_M[group_id] for memory
     access and IQM_VQPR_MAm for filed access */
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);

    exp_man = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, field_arr_BUFF_SIZE[vsq_group_ndx]);
    arad_iqm_mantissa_exponent_get(unit, exp_man, 7, &(tbl_data.vq_max_szie_bds_mnt), &(tbl_data.vq_max_szie_bds_exp));

    if(tbl_data.vq_max_szie_bds_mnt == 0) {
      info->max_inst_q_size_bds = 0;
    } else {
      info->max_inst_q_size_bds = ((tbl_data.vq_max_szie_bds_mnt) * (1<<(tbl_data.vq_max_szie_bds_exp)));
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
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO     *exact_info
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
    data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE];
  const soc_mem_t mem_arr_IQM_VSQ_FC_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_FC_PRMm, IQM_VSQB_FC_PRMm, IQM_VSQC_FC_PRMm, IQM_VSQD_FC_PRMm, IQM_VSQE_FC_PRMm, IQM_VSQF_FC_PRMm}; 
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);
  
  if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  }
  sal_memset(data, 0x0, sizeof(data));

  res = soc_mem_read(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res); 

  vsq_bds_th_clear_and_set_mnt_nof_bits = 4;
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

  res = soc_mem_write(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res); 

exit: 
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*    Get the VSQ flow-control info according to vsq-group-id
*********************************************************************/
uint32
  jer_itm_vsq_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    res,
    set_threshold_bd_mnt,
    set_threshold_bd_exp,
    clear_threshold_bd_mnt,
    clear_threshold_bd_exp,
    mnt_exp,
    data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE];
  const soc_mem_t mem_arr_IQM_VSQ_FC_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_FC_PRMm, IQM_VSQB_FC_PRMm, IQM_VSQC_FC_PRMm, IQM_VSQD_FC_PRMm, IQM_VSQE_FC_PRMm, IQM_VSQF_FC_PRMm};
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  }

  sal_memset(data, 0x0, sizeof(data));
  res = soc_mem_read(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res);

  mnt_exp = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, SET_THRESHOLD_BDSf);
  arad_iqm_mantissa_exponent_get(unit, mnt_exp, 4, &(set_threshold_bd_mnt), &(set_threshold_bd_exp));

  mnt_exp = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, CLR_THRESHOLD_BDSf);
  arad_iqm_mantissa_exponent_get(unit, mnt_exp, 4, &(clear_threshold_bd_mnt), &(clear_threshold_bd_exp));

  info->bd_size_fc.clear = ((clear_threshold_bd_mnt) * (1 << (clear_threshold_bd_exp)));
  info->bd_size_fc.set = ((set_threshold_bd_mnt) * (1 << (set_threshold_bd_exp)));
  info->q_size_fc.clear = 0;
  info->q_size_fc.set = 0;

exit: 
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME
#endif /* of #if defined(BCM_88675_A0) */
#include <soc/dpp/SAND/Utils/sand_footer.h> 

