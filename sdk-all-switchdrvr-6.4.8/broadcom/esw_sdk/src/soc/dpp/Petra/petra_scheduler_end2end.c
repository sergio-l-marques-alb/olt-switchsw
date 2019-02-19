/* $Id: petra_scheduler_end2end.c,v 1.14 Broadcom SDK $
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
* FILENAME:       soc_petra_end2end_scheduler.c
*
* MODULE PREFIX:  soc_petra_sch
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

#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>
#include <soc/dpp/Petra/petra_scheduler_flows.h>
#include <soc/dpp/Petra/petra_scheduler_ports.h>
#include <soc/dpp/Petra/petra_scheduler_flow_converts.h>
#include <soc/dpp/Petra/petra_scheduler_device.h>
#include <soc/dpp/Petra/petra_scheduler_elements.h>
#include <soc/dpp/Petra/petra_ofp_rates.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
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
* NAME:
*     soc_petra_scheduler_end2end_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  soc_petra_scheduler_end2end_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCHEDULER_END2END_REGS_INIT);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->sch.scheduler_configuration_reg.sub_flow_enable, 0x1, 10, exit);
  SOC_PETRA_REG_SET(regs->sch.dlm_reg, 0x807, 20, exit);
  SOC_PETRA_REG_SET(regs->sch.smp_internal_messages, 0x10ff, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_scheduler_end2end_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_scheduler_end2end_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_scheduler_end2end_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    port_i,
    weight_i,
    ind;
  SOC_PETRA_SCH_GLOBAL_PER1K_INFO
    *global_per1k_info = NULL;
  SOC_PETRA_SCH_PORT_HP_CLASS_INFO
    *hp_class_info = NULL;
  SOC_PETRA_SCH_SE_CL_CLASS_TABLE
    *cl_class_table = NULL,
    *exact_cl_class_table = NULL;
  SOC_PETRA_SCH_PORT_INFO
    *sch_port_info = NULL;
  SOC_PETRA_SCH_IF_WEIGHTS
    *weights = NULL;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCHEDULER_END2END_INIT);

  regs = soc_petra_regs();

  res = soc_petra_scheduler_end2end_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sch_flow_ipf_config_mode_set_unsafe(
          unit,
          SOC_PETRA_SCH_NOF_FLOW_IPF_CONFIG_MODES
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(global_per1k_info, PETRA_SCH_GLOBAL_PER1K_INFO);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(hp_class_info, PETRA_SCH_PORT_HP_CLASS_INFO);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(cl_class_table, PETRA_SCH_SE_CL_CLASS_TABLE);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(exact_cl_class_table, PETRA_SCH_SE_CL_CLASS_TABLE);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(sch_port_info, PETRA_SCH_PORT_INFO);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(weights, PETRA_SCH_IF_WEIGHTS);

  global_per1k_info->is_cl_cir = FALSE;
  global_per1k_info->is_interdigitated = FALSE;
  global_per1k_info->is_odd_even = FALSE;
  for (ind = SOC_PETRA_SCH_FLOW_BASE_AGGR_FLOW_ID; ind <= SOC_PETRA_SCH_MAX_FLOW_ID; ind += 1024)
  {
    res = soc_petra_sch_per1k_info_set_unsafe(
            unit,
            SOC_PETRA_SCH_FLOW_TO_1K_ID(ind),
            global_per1k_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  res = soc_petra_sch_class_type_params_table_set(
          unit,0,
          cl_class_table,
          exact_cl_class_table
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  hp_class_info->lowest_hp_class[0] = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1;
  hp_class_info->lowest_hp_class[1] = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF2;
  hp_class_info->lowest_hp_class[2] = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF3;
  hp_class_info->lowest_hp_class[3] = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_SINGLE_CLASS_AF1_WFQ;
  res = soc_petra_sch_port_hp_class_conf_set_unsafe(
          unit,
          hp_class_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  sch_port_info->enable = FALSE;
  sch_port_info->group = SOC_PETRA_SCH_GROUP_AUTO;
  sch_port_info->hr_mode = SOC_PETRA_SCH_HR_MODE_NONE;
  sch_port_info->lowest_hp_class = SOC_PETRA_SCH_PORT_LOWEST_HP_HR_CLASS_EF1;
  sch_port_info->max_expected_rate = SOC_PETRA_SCH_PORT_MAX_EXPECTED_RATE_AUTO;
  for (port_i = 0; port_i < SOC_PETRA_NOF_FAP_PORTS; ++port_i)
  {
    res = soc_petra_sch_port_sched_set_unsafe(
            unit,
            port_i,
            sch_port_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  for (weight_i = 0; weight_i < SOC_PETRA_SCH_NOF_IF_WEIGHTS; weight_i++)
  {
    weights->weight[weight_i].id = weight_i;
    weights->weight[weight_i].val = 0x1;
  }
  weights->nof_enties = SOC_PETRA_SCH_NOF_IF_WEIGHTS;

  res = soc_petra_sch_if_weight_conf_set_unsafe(
          unit,
          weights
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  SOC_PETRA_FLD_SET(regs->sch.erp_config_reg.erpport_id,SOC_PETRA_FAP_EGRESS_REPLICATION_SCH_PORT_ID,70,exit);

exit:
  SOC_PETRA_FREE(global_per1k_info);
  SOC_PETRA_FREE(hp_class_info);
  SOC_PETRA_FREE(cl_class_table);
  SOC_PETRA_FREE(exact_cl_class_table);
  SOC_PETRA_FREE(sch_port_info);
  SOC_PETRA_FREE(weights);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_scheduler_end2end_init()",0,0);
}

STATIC uint32
  soc_petra_sch_group_to_se_assign(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID      father_se_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_GROUP      *group
  )
{
  uint32
    res = 0;
  SOC_PETRA_SCH_GROUP
    grp,
    father_grp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_GROUP_TO_SE_ASSIGN);

  SOC_SAND_CHECK_NULL_INPUT(group);

  /* Assign a group to an aggregate */
  res = soc_petra_sch_se_id_verify_unsafe(
        unit,
        father_se_ndx
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sch_se_group_get(
          unit,
          father_se_ndx,
          &father_grp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  grp = (father_grp + 1) % SOC_PETRA_SCH_NOF_GROUPS;

  *group = grp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_group_to_se_assign()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_se_group_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     SOC_PETRA_SCH_SE_ID        se_ndx,
    SOC_SAND_OUT    SOC_PETRA_SCH_GROUP*  group
  )
{
  uint32
    offset,
    idx,
    res;
  SOC_PETRA_SCH_FGM_TBL_DATA
    fgm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SE_GROUP_GET);
  SOC_SAND_CHECK_NULL_INPUT(group);

  res = soc_petra_sch_se_id_verify_unsafe(
          unit,
          se_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  offset  = se_ndx/8;
  idx = se_ndx%8;

  /*
   * Read indirect from FGM table
   */
  res = soc_petra_sch_fgm_tbl_get_unsafe(
          unit,
          offset,
          &fgm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *group = fgm_tbl_data.flow_group[idx];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_se_group_get()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_se_group_set(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     SOC_PETRA_SCH_SE_ID        se_ndx,
    SOC_SAND_IN     SOC_PETRA_SCH_GROUP        group
  )
{
  uint32
    offset,
    idx,
    res;
  SOC_PETRA_SCH_PORT_ID
    port_id;
  SOC_PETRA_SCH_FGM_TBL_DATA
    fgm_tbl_data;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SE_GROUP_SET);

  regs = soc_petra_regs();

  res = soc_petra_sch_se_id_verify_unsafe(
          unit,
          se_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    group, SOC_PETRA_SCH_GROUP_A, SOC_PETRA_SCH_GROUP_C,
    SOC_PETRA_SCH_GROUP_OUT_OF_RANGE_ERR, 20, exit
  );

  offset  = se_ndx/8;
  idx     = se_ndx%8;

  /*
   * Write indirect from FGM table {
   */
  res = soc_petra_sch_fgm_tbl_get_unsafe(
          unit,
          offset,
          &fgm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (fgm_tbl_data.flow_group[idx] != (uint32)group)
  {
    fgm_tbl_data.flow_group[idx] = group;
    res = soc_petra_sch_fgm_tbl_set_unsafe(
          unit,
          offset,
          &fgm_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  /* } */

  /*
   *  If this is a port, FGM must be consistent with STL registers.
   *  If this is HR which is not a port - the STL configuration doesn't matter,
   *  but we configure it anyway for consistency.
   *  set the STL registers here {
   */
  port_id = soc_petra_sch_se2port_id(se_ndx);
  if (SOC_PETRA_SCH_INDICATED_PORT_ID_IS_VALID(port_id))
  {
    /* This can be a port - configure the STL */

    /* Register index */
    idx = port_id / SOC_PETRA_NOF_STL_GROUP_CONFIG_FLDS;

    /* Field index */
    offset = port_id % SOC_PETRA_NOF_STL_GROUP_CONFIG_FLDS;

    SOC_PETRA_FLD_SET(regs->sch.stl_group_config_reg[idx].port_group[offset], (uint32)group, 50, exit);
  }

  /* } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_se_group_set()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_slow_max_rates_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SLOW_RATE      *slow_max_rates
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SLOW_MAX_RATES_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_slow_max_rates_verify()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_slow_max_rates_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SLOW_RATE      *slow_max_rates,
    SOC_SAND_OUT SOC_PETRA_SCH_SLOW_RATE      *exact_slow_max_rates
  )
{
  uint32
    slow_fld_val,
    res;
  uint32
    slow_idx;
  SOC_PETRA_SCH_SUBFLOW
    exact_sub_flow,
    sub_flow;
  SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC
    internal_sub_flow;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *slow_fld;
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_TBL_FIELD
    *peak_rate_exp_fld,
    *peak_rate_man_fld;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SLOW_MAX_RATES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exact_slow_max_rates);

  soc_petra_PETRA_SCH_SUBFLOW_clear(unit,&sub_flow);
  soc_petra_PETRA_SCH_SUBFLOW_clear(unit,&exact_sub_flow);

  regs = soc_petra_regs();
  tables = soc_petra_tbls();

  /*
   * The rate register value is interpreted like \{PeakRateExp,
   * PeakRateMan\} in the SHDS table.
   * Get the fields database for the interpretation.
   */
  peak_rate_man_fld = SOC_PETRA_TBL_REF(tables->sch.shds_tbl.peak_rate_man_even);
  peak_rate_exp_fld = SOC_PETRA_TBL_REF(tables->sch.shds_tbl.peak_rate_exp_even);


  for (slow_idx = 0; slow_idx < SOC_PETRA_SCH_NOF_SLOW_RATES; slow_idx++)
  {
    sub_flow.shaper.max_rate = slow_max_rates->rates[slow_idx];

    if (slow_idx == 0)
    {
      slow_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.shaper_configuration_1_reg.shaper_slow_rate1);
    }
    else
    {
      slow_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.shaper_configuration_1_reg.shaper_slow_rate2);
    }

   /*
    * The slow setting is equivalent to the SHDS setting.
    */
    res =
      soc_petra_sch_to_internal_subflow_shaper_convert(
        unit,
        &sub_flow,
        &internal_sub_flow,
        TRUE
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    slow_fld_val = 0;
    slow_fld_val |= SOC_PETRA_FLD_IN_PLACE(internal_sub_flow.peak_rate_exp, *peak_rate_exp_fld);
    slow_fld_val |= SOC_PETRA_FLD_IN_PLACE(internal_sub_flow.peak_rate_man, *peak_rate_man_fld);

    SOC_PETRA_IMPLICIT_FLD_SET(*slow_fld, slow_fld_val, 50, exit);

    res = soc_petra_sch_from_internal_subflow_shaper_convert(
            unit,
            &internal_sub_flow,
            &exact_sub_flow
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


    exact_slow_max_rates->rates[slow_idx] = exact_sub_flow.shaper.max_rate;

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_slow_max_rates_set_unsafe()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_slow_max_rates_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SCH_SLOW_RATE      *slow_max_rates
  )
{
  uint32
    slow_fld_val,
    res;
  uint32
    slow_idx;
  SOC_PETRA_SCH_SUBFLOW
    sub_flow;
  SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC
    internal_sub_flow;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *slow_fld;
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_TBL_FIELD
    *peak_rate_exp_fld,
    *peak_rate_man_fld;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_SLOW_MAX_RATES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(slow_max_rates);

  /* These values are accesed but have no influence on the max rates*/
  internal_sub_flow.max_burst = 0;
  internal_sub_flow.slow_rate_index = 0;

  regs = soc_petra_regs();

  /*
   * The rate register value is interpreted like \{PeakRateExp,
   * PeakRateMan\} in the SHDS table.
   * Get the fields database for the interpretation.
   */
  res = soc_petra_tbls_get(&tables);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  peak_rate_man_fld = SOC_PETRA_TBL_REF(tables->sch.shds_tbl.peak_rate_man_even);
  peak_rate_exp_fld = SOC_PETRA_TBL_REF(tables->sch.shds_tbl.peak_rate_exp_even);


  for (slow_idx = 0; slow_idx < SOC_PETRA_SCH_NOF_SLOW_RATES; slow_idx++)
  {
    if (slow_idx == 0)
    {
      slow_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.shaper_configuration_1_reg.shaper_slow_rate1);
    }
    else
    {
      slow_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.shaper_configuration_1_reg.shaper_slow_rate2);
    }


    SOC_PETRA_IMPLICIT_FLD_GET(*slow_fld, (slow_fld_val), 20, exit);

    internal_sub_flow.peak_rate_exp = SOC_PETRA_FLD_FROM_PLACE(slow_fld_val, *peak_rate_exp_fld);
    internal_sub_flow.peak_rate_man = SOC_PETRA_FLD_FROM_PLACE(slow_fld_val, *peak_rate_man_fld);

   /*
    * The slow setting is equivalent to the SHDS setting.
    */
    res =
      soc_petra_sch_from_internal_subflow_shaper_convert(
        unit,
        &internal_sub_flow,
        &sub_flow
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


    slow_max_rates->rates[slow_idx] = sub_flow.shaper.max_rate;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_slow_max_rates_get_unsafe()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_aggregate_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO        *se,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow
  )
{
  uint32
    res;
  SOC_PETRA_SCH_FLOW_ID
    flow_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_AGGREGATE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(se);
  SOC_SAND_CHECK_NULL_INPUT(flow);

  SOC_SAND_MAGIC_NUM_VERIFY(se);
  SOC_SAND_MAGIC_NUM_VERIFY(flow);

  res = soc_petra_sch_se_id_verify_unsafe(
          unit,
          se_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sch_se_verify_unsafe(
          unit,
          se
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  flow_ndx = soc_petra_sch_se2flow_id(se_ndx);

  if (flow_ndx != flow->sub_flow[0].id)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_AGGR_SE_AND_FLOW_ID_MISMATCH_ERR, 30, exit);
  }

  res = soc_petra_sch_flow_verify_unsafe(
          unit,
          flow_ndx,
          flow
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_aggregate_verify()",se_ndx,0);
}

/*********************************************************************
*     Sets an aggregate scheduler. It configures an elementary
*     scheduler, and defines a credit flow to this scheduler
*     from a 'father' scheduler. The driver writes to the
*     following tables: Scheduler Enable Memory (SEM),
*     HR-Scheduler-Configuration (SHC), CL-Schedulers
*     Configuration (SCC), Flow Group Memory (FGM) Shaper
*     Descriptor Memory (SHD) Flow Sub-Flow (FSF) Flow
*     Descriptor Memory (FDM) Shaper Descriptor Memory
*     Static(SHDS) Flow Descriptor Memory Static (FDMS)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_aggregate_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO        *se,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *exact_flow
  )
{
  uint32
    res;
  uint32
    sub_flow_i,
    nof_subflows = 0;
  SOC_PETRA_SCH_GROUP
    group = SOC_PETRA_SCH_GROUP_LAST;
  SOC_PETRA_SCH_SE_ID
    subflow_se_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_AGGREGATE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(se);
  SOC_SAND_CHECK_NULL_INPUT(flow);
  SOC_SAND_CHECK_NULL_INPUT(exact_flow);

  /*
   * Set aggregate flow parameters
   */
  if(se->state == SOC_PETRA_SCH_SE_STATE_ENABLE)
  {
    for (sub_flow_i = 0; sub_flow_i < SOC_PETRA_SCH_NOF_SUB_FLOWS; sub_flow_i++)
    {
      if (flow->sub_flow[sub_flow_i].is_valid)
      {
        ++nof_subflows;
      }
    }

    res = soc_petra_sch_se_set_unsafe(
            unit,
            se,
            nof_subflows
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
     * Assign scheduler group.
     * For composite flows - use always the first subflow
     * to define the group {
     */
    if (se->group == SOC_PETRA_SCH_GROUP_AUTO)
    {
      res = soc_petra_sch_group_to_se_assign(
        unit,
        flow->sub_flow[0].credit_source.id,
        &group
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      group = se->group;
    }


    res = soc_petra_sch_se_group_set(
            unit,
            se->id,
            group
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    if (nof_subflows > 1)
    {
      subflow_se_id = soc_petra_sch_flow2se_id(
                        flow->sub_flow[1].id
                      );
      if (SOC_PETRA_SCH_INDICATED_SE_ID_IS_VALID(subflow_se_id))
      {
        res = soc_petra_sch_se_group_set(
            unit,
            subflow_se_id,
            group
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
    }

    /*
     *  } Assign scheduler group
     */

    /* Install the aggregate, using the first subflow index */
    res = soc_petra_sch_flow_set_unsafe(
            unit,
            flow->sub_flow[0].id,
            flow,
            exact_flow
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
  else /* se->state == ARAD_SCH_SE_STATE_DISABLE */
  {
    for (sub_flow_i = 0; sub_flow_i < SOC_PETRA_SCH_NOF_SUB_FLOWS; sub_flow_i++)
    {
      if (flow->sub_flow[sub_flow_i].is_valid)
      {
        ++nof_subflows;
      }
    }

    res = soc_petra_sch_se_set_unsafe(
            unit,
            se,
            nof_subflows
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Install the aggregate, using the first subflow index */
    res = soc_petra_sch_flow_set_unsafe(
            unit,
            flow->sub_flow[0].id,
            flow,
            exact_flow
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_aggregate_set_unsafe()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_aggregate_group_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID    se_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_INFO  *se,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW     *flow
  )
{
  uint32
    res;
  uint32
    sub_flow_i,
    nof_subflows = 0;
  SOC_PETRA_SCH_GROUP
    group = SOC_PETRA_SCH_GROUP_LAST;
  SOC_PETRA_SCH_SE_ID
    subflow_se_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_AGGREGATE_GROUP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(se);
  SOC_SAND_CHECK_NULL_INPUT(flow);

  if(se->state == SOC_PETRA_SCH_SE_STATE_ENABLE)
  {
    for (sub_flow_i = 0; sub_flow_i < SOC_PETRA_SCH_NOF_SUB_FLOWS; sub_flow_i++)
    {
      if (flow->sub_flow[sub_flow_i].is_valid)
      {
        ++nof_subflows;
      }
    }
  
    res = soc_petra_sch_se_set_unsafe(
            unit,
            se,
            nof_subflows
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
    /*
     * Assign scheduler group.
     * For composite flows - use always the first subflow
     * to define the group {
     */
    if (se->group == SOC_PETRA_SCH_GROUP_AUTO)
    {
      res = soc_petra_sch_group_to_se_assign(
        unit,
        flow->sub_flow[0].credit_source.id,
        &group
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      group = se->group;
    }
  
  
    res = soc_petra_sch_se_group_set(
            unit,
            se->id,
            group
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  
    if (nof_subflows > 1)
    {
      subflow_se_id = soc_petra_sch_flow2se_id(
                        flow->sub_flow[1].id
                      );
      if (SOC_PETRA_SCH_INDICATED_SE_ID_IS_VALID(subflow_se_id))
      {
        res = soc_petra_sch_se_group_set(
            unit,
            subflow_se_id,
            group
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
    }
  
    /*
     *  } Assign scheduler group
     */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_aggregate_group_set_unsafe",0,0);
}


/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_aggregate_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_SE_ID          se_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_SE_INFO        *se,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *flow
  )
{
  uint32
    res;
  SOC_PETRA_SCH_FLOW_ID
    flow_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_AGGREGATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(se);
  SOC_SAND_CHECK_NULL_INPUT(flow);

  res = soc_petra_sch_se_id_verify_unsafe(
          unit,
          se_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  flow_ndx = soc_petra_sch_se2flow_id(
               se_ndx
             );

  res = soc_petra_sch_flow_id_verify_unsafe(
          unit,
          flow_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_sch_se_get_unsafe(
          unit,
          se_ndx,
          se
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_sch_flow_get_unsafe(
          unit,
          flow_ndx,
          flow
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_aggregate_get_unsafe()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_per1k_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_GLOBAL_PER1K_INFO *per1k_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PER1K_INFO_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(per1k_info);

  SOC_SAND_MAGIC_NUM_VERIFY(per1k_info);

  res = soc_petra_sch_k_flow_id_verify_unsafe(
          unit,
          k_flow_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(!SOC_PETRA_SCH_1K_FLOWS_IS_IN_AGGR_RANGE(k_flow_ndx))
  {
    /*
     * Note: those id-s are allowed for get - values valid for
     * first 24K flows are returned.
     * It is not valid for set (only 24K - 56K-1 range is valid)
     */
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_K_FLOW_ID_ERR, 15, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_per1k_info_verify()",0,0);
}

/*********************************************************************
*     Sets configuration for 1K flows/aggregates (256
*     quartets). Flows interdigitated mode configuration must
*     match the interdigitated mode configurations of the
*     queues they are mapped to. Note1: the following flow
*     configuration is not allowed: interdigitated = TRUE,
*     odd_even = FALSE. The reason for this is that
*     interdigitated configuration defines flow-queue mapping,
*     but a flow with odd_even configuration = FALSE cannot be
*     mapped to a queue. Note2: this configuration is only
*     relevant to flow_id-s in the range 24K - 56K.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_per1k_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_GLOBAL_PER1K_INFO *per1k_info
  )
{
  uint32
    reg_val,
    res;
  uint8
    is_cl_cir         = FALSE,
    is_odd_even       = FALSE,
    is_interdigitated = FALSE;
  uint32
    k_idx;
  SOC_PETRA_REGS
    *regs;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PER1K_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(per1k_info);

   if(!SOC_PETRA_SCH_1K_FLOWS_IS_IN_AGGR_RANGE(k_flow_ndx))
  {
    /*
     * Note: those id-s are allowed for get - values valid for
     * first 24K flows are returned.
     * It is not valid for set (only 24K - 56K-1 range is valid)
     */
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_K_FLOW_ID_ERR, 15, exit);
  }

  k_idx = k_flow_ndx - SOC_PETRA_SCH_FLOW_TO_1K_ID(SOC_PETRA_SCH_FLOW_BASE_AGGR_FLOW_ID);

  regs = soc_petra_regs();

  /*
   * OddEven {
   */
  SOC_PETRA_FLD_GET(regs->sch.fsf_composite_configuration_reg.fsfcomp_odd_even, (reg_val), 20, exit);

  is_odd_even = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx);

  if(per1k_info->is_odd_even != is_odd_even)
  {
    SOC_SAND_SET_BIT(reg_val, per1k_info->is_odd_even, k_idx);

    SOC_PETRA_FLD_SET(regs->sch.fsf_composite_configuration_reg.fsfcomp_odd_even, reg_val, 20, exit);
  }
  /*
   * OddEven }
   */

  /*
   * Interdigitated {
   */
   SOC_PETRA_FLD_GET(regs->sch.select_flow_to_queue_mapping_reg.inter_dig, (reg_val), 20, exit);

  is_interdigitated = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx);

  if(per1k_info->is_interdigitated != is_interdigitated)
  {
    SOC_SAND_SET_BIT(reg_val, per1k_info->is_interdigitated, k_idx);

    SOC_PETRA_FLD_SET(regs->sch.select_flow_to_queue_mapping_reg.inter_dig, reg_val, 20, exit);
  }
  /*
   * Interdigitated }
   */

  /*
   * CIR/EIR {
   */

  SOC_PETRA_FLD_GET(regs->sch.switch_cir_eir_in_dual_shapers_reg.switch_cireir, (reg_val), 20, exit);

  is_cl_cir = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx);

  if(per1k_info->is_cl_cir != is_cl_cir)
  {
    SOC_SAND_SET_BIT(reg_val, per1k_info->is_cl_cir, k_idx);

    SOC_PETRA_FLD_SET(regs->sch.switch_cir_eir_in_dual_shapers_reg.switch_cireir, reg_val, 20, exit);
  }

  /*
   * CIR/EIR }
   */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_per1k_info_set_unsafe()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_per1k_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_GLOBAL_PER1K_INFO *per1k_info
  )
{
  uint32
    reg_val,
    res;
  uint8
    is_cl_cir         = FALSE,
    is_odd_even       = FALSE,
    is_interdigitated = FALSE;
  uint32
    k_idx;
  SOC_PETRA_REGS
    *regs;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_PER1K_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(per1k_info);

  res = soc_petra_sch_k_flow_id_verify_unsafe(
          unit,
          k_flow_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  if (!SOC_PETRA_SCH_1K_FLOWS_IS_IN_AGGR_RANGE(k_flow_ndx))
  {
    soc_petra_PETRA_SCH_GLOBAL_PER1K_INFO_clear(per1k_info);
  }
  else
  {
    k_idx = k_flow_ndx - SOC_PETRA_SCH_FLOW_TO_1K_ID(SOC_PETRA_SCH_FLOW_BASE_AGGR_FLOW_ID);

    /*
     * OddEven {
     */
    SOC_PETRA_FLD_GET(regs->sch.fsf_composite_configuration_reg.fsfcomp_odd_even, (reg_val), 20, exit);

    is_odd_even = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx);

    per1k_info->is_odd_even = is_odd_even;
    /*
     * OddEven }
     */

    /*
     * Interdigitated {
     */
     SOC_PETRA_FLD_GET(regs->sch.select_flow_to_queue_mapping_reg.inter_dig, (reg_val), 20, exit);

    is_interdigitated = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx);

    per1k_info->is_interdigitated = is_interdigitated;
    /*
     * Interdigitated }
     */

    /*
     * CIR/EIR {
     */

    SOC_PETRA_FLD_GET(regs->sch.switch_cir_eir_in_dual_shapers_reg.switch_cireir, (reg_val), 20, exit);

    is_cl_cir = (uint8)SOC_SAND_GET_BIT(reg_val, k_idx);

    per1k_info->is_cl_cir = is_cl_cir;

    /*
     * CIR/EIR }
     */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_per1k_info_get_unsafe()",0,0);
}

/*****************************************************
* NAME
*   soc_petra_sch_nof_quartets_to_map_get
* TYPE:
*   PROC
* DATE:
*   14/01/2008
* FUNCTION:
* Calculate number of quartets to map according to the following table:
*
*    |InterDigitated | Composite | nof_quartets_to_map |
*     -------------------------------------------------
*    |      0        |     0     |         1           |
*     -------------------------------------------------
*    |      1        |     0     |         2           |
*     -------------------------------------------------
*    |      0        |     1     |         2           |
*     -------------------------------------------------
*    |      1        |     1     |         4           |
*     -------------------------------------------------
*
* INPUT:
*   SOC_SAND_IN  uint8                 is_interdigitated -
*     Interdigitated mode per-1k configuration
*   SOC_SAND_IN  uint8                 is_composite -
*     Composite per-quartet configuration
*   SOC_SAND_OUT uint32                 *nof_quartets_to_map -
*     Number of quartets to map, according to the table above
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_nof_quartets_to_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_interdigitated,
    SOC_SAND_IN  uint8                 is_composite,
    SOC_SAND_OUT uint32                 *nof_quartets_to_map
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_NOF_QUARTETS_TO_MAP_GET);
  SOC_SAND_CHECK_NULL_INPUT(nof_quartets_to_map);

  switch(is_interdigitated)
  {
  case FALSE:
    if (is_composite == FALSE)
    {
      *nof_quartets_to_map = 1;
    }
    else
    {
      *nof_quartets_to_map = 2;
    }
    break;
  case TRUE:
    if (is_composite == FALSE)
    {
      *nof_quartets_to_map = 2;
    }
    else
    {
      *nof_quartets_to_map = 4;
    }
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_FLOW_TO_Q_INVALID_GLOBAL_CONF_ERR, 10, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_sch_nof_quartets_to_map_get()", 0, 0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_to_queue_mapping_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_IN  uint32                 nof_quartets_to_map,
    SOC_SAND_IN  SOC_PETRA_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  )
{
  uint32
    offset,
    res;
  SOC_PETRA_SCH_FLOW_ID
    flow_ndx;
  uint32
    k_flow_ndx,
    quartets_to_map_calculated = 0;
  SOC_PETRA_SCH_GLOBAL_PER1K_INFO
    per1k_info;
  SOC_PETRA_SCH_FFM_TBL_DATA
    ffm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(quartet_flow_info);

  SOC_SAND_MAGIC_NUM_VERIFY(quartet_flow_info);

  res = soc_petra_sch_quartet_id_verify_unsafe(
          unit,
          quartet_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  flow_ndx = SOC_PETRA_SCH_QRTT_TO_FLOW_ID(quartet_ndx);
  k_flow_ndx = SOC_PETRA_SCH_FLOW_TO_1K_ID(flow_ndx);

  res = soc_petra_sch_per1k_info_get_unsafe(
          unit,0,
          k_flow_ndx,
          &per1k_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_ipq_queue_id_verify(
          unit,
          SOC_PETRA_IPQ_QRTT_TO_Q_ID(quartet_flow_info->base_q_qrtt_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (per1k_info.is_odd_even == FALSE)
  {
    /*
     * Flows with Odd-Even configuration set to FALSE
     * (0-2 configuration) cannot be mapped to queues
     */
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_FLOW_TO_Q_ODD_EVEN_IS_FALSE_ERR, 40, exit);
  }

  res = soc_petra_sch_nof_quartets_to_map_get(
          unit,
          per1k_info.is_interdigitated,
          quartet_flow_info->is_composite,
          &quartets_to_map_calculated
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (quartets_to_map_calculated != nof_quartets_to_map)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_FLOW_TO_Q_NOF_QUARTETS_MISMATCH_ERR, 60, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    quartet_flow_info->fip_id, SOC_PETRA_MAX_FAP_ID,
    SOC_PETRA_FAP_PORT_ID_INVALID_ERR , 65, exit
  );

  /*
   * Validate odd quartets FIP configuration
   * with even quartets FIP configuration.
   * Since this is a per-8 flow_id-s configuration,
   * it will be configured for even quartets and
   * validated for odd ones.
   */
  if ((quartet_ndx%2) == 1)
  {
    offset = quartet_ndx / 2;
    res = soc_petra_sch_ffm_tbl_get_unsafe(
          unit,
          offset,
          &ffm_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    if (ffm_tbl_data.device_number != quartet_flow_info->fip_id)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_FLOW_TO_FIP_SECOND_QUARTET_MISMATCH_ERR, 80, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_to_queue_mapping_verify()",0,0);
}


/*********************************************************************
*     Sets the mapping from flow to queue and to source fap.
*     The configuration is per quartet (up to 4 quartets). The
*     mapping depends on the following parameters: -
*     interdigitated mode - composite mode The driver writes
*     to the following tables: Flow to Queue Mapping (FQM)
*     Flow to FIP Mapping (FFM)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_to_queue_mapping_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_IN  uint32                 nof_quartets_to_map,
    SOC_SAND_IN  SOC_PETRA_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  )
{
  uint32
    offset,
    quartet_i,
    res;
  SOC_PETRA_SCH_FQM_TBL_DATA
    fqm_tbl_data;
  SOC_PETRA_SCH_FFM_TBL_DATA
    ffm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(quartet_flow_info);

   res = soc_petra_sch_quartet_id_verify_unsafe(
          unit,
          quartet_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (quartet_i = 0; quartet_i < nof_quartets_to_map; ++quartet_i)
  {
    /*
     * RMW from FQM {
     * (don't just write because of the flow_slow_enable field)
     */
    offset  = quartet_ndx + quartet_i;

    res = soc_petra_sch_fqm_tbl_get_unsafe(
            unit,
            offset,
            &fqm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    fqm_tbl_data.base_queue_num = quartet_flow_info->base_q_qrtt_id;
    fqm_tbl_data.sub_flow_mode = quartet_flow_info->is_composite;

    res = soc_petra_sch_fqm_tbl_set_unsafe(
            unit,
            offset,
            &fqm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    /*
     * RMW from FQM }
     */


    if (((quartet_ndx + quartet_i) % 2) == 0)
    {
      /*
       * Write to FFM {
       * - only for even quartets (per 8 flow id-s configuration)
       */
      offset = (quartet_ndx + quartet_i) / 2;

      ffm_tbl_data.device_number = quartet_flow_info->fip_id;

      res = soc_petra_sch_ffm_tbl_set_unsafe(
              unit,
              offset,
              &ffm_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /*
       * Write to FFM }
       */
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_to_queue_mapping_set_unsafe()",0,0);
}

/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_sch_flow_to_queue_mapping_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  )
{
  uint32
    offset,
    res;
  SOC_PETRA_SCH_FQM_TBL_DATA
    fqm_tbl_data;
  SOC_PETRA_SCH_FFM_TBL_DATA
    ffm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_TO_QUEUE_MAPPING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(quartet_flow_info);

   res = soc_petra_sch_quartet_id_verify_unsafe(
          unit,
          quartet_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Read from FQM {
   */
  offset  = quartet_ndx;

  res = soc_petra_sch_fqm_tbl_get_unsafe(
          unit,
          offset,
          &fqm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  quartet_flow_info->base_q_qrtt_id = fqm_tbl_data.base_queue_num;
  quartet_flow_info->is_composite = (uint8)fqm_tbl_data.sub_flow_mode;
  /*
   * Read from FQM }
   */

  /*
   * Read from FFM {
   */
  offset = quartet_ndx/2;

  res = soc_petra_sch_ffm_tbl_get_unsafe(
          unit,
          offset,
          &ffm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  quartet_flow_info->fip_id = ffm_tbl_data.device_number;

  /*
   * Read from FFM }
   */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_to_queue_mapping_get_unsafe()",0,0);
}


/*********************************************************************
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint8
  soc_petra_sch_is_flow_id_se_id(
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_id
  )
{
  SOC_PETRA_SCH_SE_ID
    se_id = 0;

  se_id = soc_petra_sch_flow2se_id(flow_id);

  return (SOC_PETRA_SCH_INDICATED_SE_ID_IS_VALID(se_id))?TRUE : FALSE;
}


uint32
  soc_petra_sch_flow_delete_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID         flow_ndx
  )
{
  uint32
    offset = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_FDMS_TBL_DATA
    sch_fdms_tbl_data;
  SOC_PETRA_SCH_SHDS_TBL_DATA
    shds_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_DELETE_UNSAFE);

  /*
   *	Set max-burst token-bucket to '0'.
   *  This protects from turning on an interrupt
   *  when the FDMS is zeroed
   */
  /*
   * Get current value - we only update part of the fields (odd/even)
   */
  offset  = flow_ndx / 2;
  res = soc_petra_sch_shds_tbl_get_unsafe(
          unit,
          offset,
          &shds_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (flow_ndx % 2 == 0)
  {
    shds_tbl_data.max_burst_even = 0x0;
    shds_tbl_data.max_burst_update_even = 0x1;
  }
  else
  {
    shds_tbl_data.max_burst_odd = 0x0;
    shds_tbl_data.max_burst_update_odd = 0x1;
  }

  /*
   * Write indirect to SHDS table
   */
  res = soc_petra_sch_shds_tbl_set_unsafe(
          unit,
          offset,
          &shds_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  /*
   *	Set FDMS to 0 as invalid-flow indication
   */
  sch_fdms_tbl_data.cos = 0;
  sch_fdms_tbl_data.hrsel_dual = 0;
  sch_fdms_tbl_data.sch_number = 0;
  res = soc_petra_sch_fdms_tbl_set_unsafe(
          unit,
          flow_ndx,
          &sch_fdms_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_delete_unsafe()",flow_ndx,0);
}

uint32
  soc_petra_sch_flow_is_deleted_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID         flow_ndx,
    SOC_SAND_OUT uint8                 *flow_is_reset
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_FDMS_TBL_DATA
    sch_fdms_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SCH_FLOW_IS_RESET_GET_UNSAFE);

  res = soc_petra_sch_fdms_tbl_get_unsafe(
          unit,
          flow_ndx,
          &sch_fdms_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *flow_is_reset = (sch_fdms_tbl_data.cos == 0) ? TRUE : FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_sch_flow_is_deleted_get_unsafe()",flow_ndx,0);
}

#if SOC_PETRA_DEBUG

uint32
  soc_petra_flow_status_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               flow_id,
    SOC_SAND_OUT uint32               *credit_rate
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    credit_cnt;
  uint32
    credit_worth;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FLOW_STATUS_PRINT);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->sch.credit_counter_configuration_1_reg.filter_flow, flow_id, 10, exit);
  SOC_PETRA_FLD_SET(regs->sch.credit_counter_configuration_1_reg.filter_flow_mask, 0xffff, 15, exit);


  SOC_PETRA_FLD_SET(regs->sch.credit_counter_configuration_2_reg.filter_by_flow, 0x1, 20, exit);
  SOC_PETRA_FLD_SET(regs->sch.credit_counter_configuration_2_reg.filter_by_sub_flow, 0x0, 21, exit);
  SOC_PETRA_FLD_SET(regs->sch.credit_counter_configuration_2_reg.filter_dest_fap, 0x0, 22, exit);

  SOC_PETRA_REG_GET(regs->sch.credit_counter_reg, credit_cnt, 30, exit);

  sal_msleep(1008);

  SOC_PETRA_REG_GET(regs->sch.credit_counter_reg, credit_cnt, 40, exit);

  res = soc_petra_mgmt_credit_worth_get_unsafe(
          unit,
          &credit_worth
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  *credit_rate = credit_cnt * ((SOC_SAND_NOF_BITS_IN_CHAR * credit_worth) / SOC_PETRA_RATE_1K);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_flow_status_info_get()",flow_id,0);
}

uint32
  soc_petra_port_status_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               port_id,
    SOC_SAND_OUT uint32               *credit_rate,
    SOC_SAND_OUT uint32               *fc_cnt,
    SOC_SAND_OUT uint32               *fc_percent
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    credit_cnt,
    crd_reg_val,
    fc_reg_val,
    fld_val;
  uint32
    credit_worth;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

 /*
  * count credits for the port
  */
  crd_reg_val = 0;
  fld_val = port_id;

  SOC_PETRA_FLD_TO_REG(regs->sch.dvs_credit_counter_configuration_reg.dvsfilter_port, fld_val, crd_reg_val, 10, exit);

  fld_val = 0x1;
  SOC_PETRA_FLD_TO_REG(regs->sch.dvs_credit_counter_configuration_reg.cnt_by_port, fld_val, crd_reg_val, 20, exit);

  fld_val = 0x0;
  SOC_PETRA_FLD_TO_REG(regs->sch.dvs_credit_counter_configuration_reg.cnt_by_nif, fld_val, crd_reg_val, 21, exit);

 /*
  * count flow control for the port
  */
  fc_reg_val = 0;
  fld_val = port_id;
  SOC_PETRA_FLD_TO_REG(regs->sch.dvs_fc_and_rci_counters_configuration_reg.fccnt_port, fld_val, fc_reg_val, 30, exit);

  fld_val = 0x1;
  SOC_PETRA_FLD_TO_REG(regs->sch.dvs_fc_and_rci_counters_configuration_reg.cnt_high_fc, fld_val, fc_reg_val, 40, exit);

  fld_val = 0x1;
  SOC_PETRA_FLD_TO_REG(regs->sch.dvs_fc_and_rci_counters_configuration_reg.cnt_low_fc, fld_val, fc_reg_val, 50, exit);

 /*
  * start counting
  */
  SOC_PETRA_REG_SET(regs->sch.dvs_credit_counter_configuration_reg, crd_reg_val, 60, exit);

  SOC_PETRA_REG_SET(regs->sch.dvs_fc_and_rci_counters_configuration_reg, fc_reg_val, 70, exit);
 /*
  * clear counters
  */
  SOC_PETRA_REG_GET(regs->sch.dvs_credit_counter_reg, credit_cnt, 80, exit);

  SOC_PETRA_REG_GET(regs->sch.dvs_flow_control_counter_reg, *fc_cnt, 90, exit);
 /*
  * wait ~one second
  */
  sal_msleep(1008);

  SOC_PETRA_REG_GET(regs->sch.dvs_credit_counter_reg, credit_cnt, 100, exit);

  SOC_PETRA_REG_GET(regs->sch.dvs_flow_control_counter_reg, *fc_cnt, 110, exit);

  res = soc_petra_mgmt_credit_worth_get_unsafe(
          unit,
          &credit_worth
        );

  *credit_rate = credit_cnt * ((SOC_SAND_NOF_BITS_IN_CHAR * credit_worth) / SOC_PETRA_RATE_1K);

  if (*fc_cnt == 0)
  {
    soc_sand_os_printf("Not under flow control\n\r");
  }
  else
  {
    *fc_percent = SOC_SAND_DIV_ROUND_UP((*fc_cnt * 100), soc_petra_chip_ticks_per_sec_get(unit));
    SOC_SAND_LIMIT_FROM_ABOVE(*fc_percent, 100);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_port_status_info_get()",port_id,0);

}

uint32
  soc_petra_agg_status_info_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               se_id,
    SOC_SAND_OUT uint32               *credit_rate,
    SOC_SAND_OUT uint32               *overflow
  )
{
  uint32
    credit_cnt,
    res=0;
  uint32
    credit_worth;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AGG_STATUS_PRINT);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->sch.credit_scheduler_counter_in_the_cml_configuration_reg.filter_sch_mask, 0x3fff, 10, exit);
  SOC_PETRA_FLD_SET(regs->sch.credit_scheduler_counter_in_the_cml_configuration_reg.filter_sch, se_id, 20, exit);

  SOC_PETRA_FLD_GET(regs->sch.scheduler_counter_reg.cmlschcredit_cnt, credit_cnt, 0x30, exit);

  sal_msleep(1008);

  SOC_PETRA_FLD_GET(regs->sch.scheduler_counter_reg.cmlschcredit_cnt, credit_cnt, 0x40, exit);
  SOC_PETRA_FLD_GET(regs->sch.scheduler_counter_reg.cmlschcredit_ovf, *overflow, 0x45, exit);

  if (!SOC_SAND_NUM2BOOL(*overflow) == TRUE)
  {
 
    res = soc_petra_mgmt_credit_worth_get_unsafe(
            unit,
            &credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    *credit_rate = credit_cnt * ((SOC_SAND_NOF_BITS_IN_CHAR * credit_worth)/SOC_PETRA_RATE_1K);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_agg_status_info_get()",se_id,0);
}

uint32
  soc_petra_flow_and_up_info_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               flow_id,
    SOC_SAND_IN     uint32                          reterive_status,
    SOC_SAND_INOUT  SOC_PETRA_SCH_FLOW_AND_UP_INFO    *flow_and_up_info  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    credit_source_i;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    *dummy_shaper = NULL;
  SOC_PETRA_SCH_SE_CL_CLASS_INFO class_type;
  SOC_PETRA_SCH_FLOW_AND_UP_PORT_INFO *port_sch_info;
  SOC_PETRA_SCH_FLOW_AND_UP_SE_INFO   *se_sch_info;
#ifdef PETRA_PRINT_FLOW_AND_UP_PRINT_DRM_AND_MAL_RATES
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    *shaper = NULL;
#endif
  uint32
    mal_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(dummy_shaper, PETRA_OFP_RATES_MAL_SHPR_INFO);

  if (flow_and_up_info->credit_sources_nof == 0)
  {
      /*
       * First level print.
       */
      res = soc_petra_sch_flow_get_unsafe(
         unit,
         flow_id,
         &(flow_and_up_info->sch_consumer)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


      if(reterive_status)
      {
          res = soc_petra_flow_status_info_get(unit, flow_id, &(flow_and_up_info->credit_rate));
          SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
      }

     /*
      * In each level, the current entity, get the credit
      *  source entity, and print it's configuration.
      * Flows and aggregates have credit sources, and therefore
      *  we will get up in the hierarchy, until we arrive
      *  to the port level.
      */
      
      if((flow_and_up_info->sch_consumer).sub_flow[0].is_valid == TRUE)
      {
       flow_and_up_info->credit_sources[0] = (flow_and_up_info->sch_consumer).sub_flow[0].credit_source.id;
       flow_and_up_info->credit_sources_nof++;
      }
      if((flow_and_up_info->sch_consumer).sub_flow[1].is_valid == TRUE)
      {
        flow_and_up_info->credit_sources[1] = (flow_and_up_info->sch_consumer).sub_flow[1].credit_source.id;
        flow_and_up_info->credit_sources_nof++;
      }
  }

  for(credit_source_i = 0; credit_source_i < (flow_and_up_info->credit_sources_nof); ++credit_source_i)
  {

      flow_and_up_info->sch_port_id[credit_source_i] = soc_petra_sch_se2port_id(flow_and_up_info->credit_sources[credit_source_i]);
      if(soc_petra_sch_is_port_id_valid(unit,  flow_and_up_info->sch_port_id[credit_source_i]))
      {
          flow_and_up_info->is_port_sch[credit_source_i] = 1;
          port_sch_info = &((flow_and_up_info->sch_union_info[credit_source_i]).port_sch_info);
          soc_petra_PETRA_SCH_FLOW_AND_UP_PORT_INFO_clear(port_sch_info);
          res = soc_petra_sch_port_sched_get_unsafe(
             unit,
             flow_and_up_info->sch_port_id[credit_source_i],
             &(port_sch_info->port_info)
             );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


          /* Verify OFP is connected to scheduler, */
          /* i.e. it is connected to valid mal     */
          if (flow_and_up_info->sch_port_id[credit_source_i] != SOC_PETRA_FAP_EGRESS_REPLICATION_SCH_PORT_ID)
          {
              res = soc_petra_port_ofp_mal_get_unsafe(
                 unit,
                 flow_and_up_info->sch_port_id[credit_source_i],
                 &mal_i
                 );
              SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);
          }       

          if(flow_and_up_info->sch_port_id[credit_source_i] != SOC_PETRA_FAP_EGRESS_REPLICATION_SCH_PORT_ID
             && mal_i != SOC_PETRA_IF_ID_NONE)
          {
              /*print port*/
              res = soc_petra_ofp_rates_single_port_get(
                  unit,
                  flow_and_up_info->sch_port_id[credit_source_i],
                  dummy_shaper,
                &(port_sch_info->ofp_rate_info)
                );
              SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
          } else {
              /*Mark as unavail*/
              port_sch_info->ofp_rate_info.port_id = SAL_UINT32_MAX;
          }

          if(reterive_status)
          {
              soc_petra_port_status_info_get(unit,
                                    flow_and_up_info->sch_port_id[credit_source_i], 
                                    &(port_sch_info->credit_rate),
                                    &(port_sch_info->fc_cnt),
                                    &(port_sch_info->fc_percent)
                                    );
          }
      } else if(soc_petra_sch_is_se_id_valid(unit,flow_and_up_info->credit_sources[credit_source_i]))
      {
          /*print agg.*/
          se_sch_info = &((flow_and_up_info->sch_union_info[credit_source_i]).se_sch_info);
          soc_petra_PETRA_SCH_FLOW_AND_UP_SE_INFO_clear(unit, se_sch_info);
          (se_sch_info->se_info).id = flow_and_up_info->credit_sources[credit_source_i];

          res = soc_petra_sch_aggregate_get_unsafe(
                unit,
              (se_sch_info->se_info).id,
              &(se_sch_info->se_info),
              &(se_sch_info->sch_consumer)
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

          if (se_sch_info->se_info.type == SOC_PETRA_SCH_SE_TYPE_CL){
              res = soc_petra_sch_class_type_params_get_unsafe(unit, se_sch_info->se_info.type_info.cl.id, &class_type);
              SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
              se_sch_info->cl_mode = class_type.weight_mode;
              res = soc_petra_sch_flow_ipf_config_mode_get(unit, &(se_sch_info->ipf_mode));
              SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
          } 

          if(reterive_status)
          {
            res = soc_petra_agg_status_info_get(unit, 
                                           (se_sch_info->se_info).id,
                                           &(se_sch_info->credit_rate),
                                           &(se_sch_info->credit_rate_overflow));
            SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
		
          }

          if((se_sch_info->sch_consumer).sub_flow[0].is_valid == TRUE && (flow_and_up_info->next_level_credit_sources_nof < SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES))
          {
              flow_and_up_info->next_level_credit_sources[flow_and_up_info->next_level_credit_sources_nof] = (se_sch_info->sch_consumer).sub_flow[0].credit_source.id;
              (flow_and_up_info->next_level_credit_sources_nof)++;
          }
          if((se_sch_info->sch_consumer).sub_flow[1].is_valid == TRUE && (flow_and_up_info->next_level_credit_sources_nof < SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES))
          {
              (flow_and_up_info->next_level_credit_sources)[(flow_and_up_info->next_level_credit_sources_nof)] = (se_sch_info->sch_consumer).sub_flow[1].credit_source.id;
              (flow_and_up_info->next_level_credit_sources_nof)++;
          }
      } else
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_SE_ID_ERR,40,exit);
        /*error: flow can't be credit source*/
      }
  }
#ifdef PETRA_PRINT_FLOW_AND_UP_PRINT_DRM_AND_MAL_RATES
  if ( (flow_and_up_info->next_level_credit_sources_nof) == 0)
  {
      SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(shaper, PETRA_OFP_RATES_MAL_SHPR_INFO);
      res = soc_petra_port_ofp_mal_get_unsafe(
              unit,
              flow_and_up_info->sch_port_id[credit_source_i],
              &mal_idx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

      res = soc_petra_ofp_rates_get(
              unit,
              mal_idx,
              shaper,
              &(flow_and_up_info->ofp_rates_table)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

      flow_and_up_info->ofp_rate_valid = 1;
  }
#endif

exit:
  SOC_PETRA_FREE(dummy_shaper);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_flow_and_up_info_get_unsafe()",0,0);
}

#endif

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>
