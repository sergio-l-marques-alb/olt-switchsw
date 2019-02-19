/* $Id: petra_auto_queue_flow_mgmt_aggregate.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_auto_queue_flow_mgmt_aggregate.c
*
* MODULE PREFIX:  soc_petra_aqfm_agg
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

#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>

#include <soc/dpp/Petra/petra_api_auto_queue_flow_mgmt.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_port.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_aggregate.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_flow.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_queue.h>

#include <soc/dpp/Petra/petra_general.h>

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

uint32
  soc_petra_aqfm_agg_actual_id_get(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint32      port_id,
    SOC_SAND_IN  uint32       agg_auto_id,
    SOC_SAND_OUT uint32      *entity_id
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    agg_level,
    agg_index;
  SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO
    agg_item;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_ACTUAL_ID_GET);

  SOC_SAND_CHECK_NULL_INPUT(entity_id);

  agg_level = SOC_SAND_GET_BYTE_1(agg_auto_id);
  agg_index = SOC_SAND_GET_BYTE_2(agg_auto_id);

  ret = soc_petra_sw_db_auto_agg_info_item_get(
          unit,
          port_id,
          agg_level,
          agg_index,
          &agg_item
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  *entity_id = agg_item.entity_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_actual_id_get()",0,0);
}

uint32
  soc_petra_aqfm_agg_default_get(
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO      *p_aggs_info
  )
{
  uint32
    agg_level_i,
    agg_index_i;
  SOC_PETRA_SCH_FLOW
    *p_flow = NULL;
  SOC_PETRA_SCH_SE_INFO
    *p_sch_port = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGGS_DEFAULT_GET);

  SOC_SAND_CHECK_NULL_INPUT(p_aggs_info);

  for(agg_level_i = 0; agg_level_i < SOC_PETRA_AQFM_NOF_HIERARCY; ++agg_level_i)
  {
    for(agg_index_i = 0; agg_index_i < SOC_PETRA_AQFM_NOF_INDEX; ++agg_index_i)
    {
      p_aggs_info->item[agg_level_i][agg_index_i].valid = FALSE;

      p_flow     = &(p_aggs_info->item[agg_level_i][agg_index_i].flow);
      p_sch_port = &(p_aggs_info->item[agg_level_i][agg_index_i].se);

      soc_petra_PETRA_SCH_FLOW_clear(p_flow);

      p_flow->flow_type       = SOC_PETRA_FLOW_AGGREGATE;
      p_flow->is_slow_enabled = FALSE;

      p_sch_port->state = SOC_PETRA_SCH_SE_STATE_ENABLE;
      p_sch_port->type = SOC_PETRA_SCH_SE_TYPE_NONE;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_default_get()",0,0);
}

uint32
  soc_petra_aqfm_agg_info_item_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  port_id,
    SOC_SAND_IN  uint32                  agg_level,
    SOC_SAND_IN  uint32                  agg_index,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_ITEM_INFO  *agg_item,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW            *exact_flow
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO
    table_item;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_INFO_ITEM_SET);

  table_item.entity_id = agg_item->se.id;
  table_item.credit_source_subflow_0 = agg_item->flow.sub_flow[0].credit_source.id;
  table_item.credit_source_subflow_1 = agg_item->flow.sub_flow[1].credit_source.id;
  table_item.valid = agg_item->valid;

  if (table_item.valid)
  {
    res = soc_petra_sch_aggregate_set_unsafe(
            unit,
            agg_item->se.id,
            &(agg_item->se),
            &(agg_item->flow),
            exact_flow
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  ret = soc_petra_sw_db_auto_agg_info_item_set(
          unit,
          port_id,
          agg_level,
          agg_index,
          &table_item
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_info_item_set()",0,0);
}

uint32
  soc_petra_aqfm_agg_info_item_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_id,
    SOC_SAND_IN  uint32                 agg_level,
    SOC_SAND_IN  uint32                 agg_index,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_ITEM_INFO *val
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO
    table_item;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_INFO_ITEM_GET);

  res = soc_petra_sw_db_auto_agg_info_item_get(
          unit,
          port_id,
          agg_level,
          agg_index,
          &table_item
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  val->se.id= table_item.entity_id;
  val->flow.sub_flow[0].credit_source.id = table_item.credit_source_subflow_0;
  val->flow.sub_flow[1].credit_source.id = table_item.credit_source_subflow_1;
  val->valid = table_item.valid;

  if (table_item.valid)
  {
    res = soc_petra_sch_aggregate_get_unsafe(
            unit,
            val->se.id,
            &(val->se),
            &(val->flow)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_info_item_get()",0,0);
}

uint32
  soc_petra_aqfm_agg_actual_id_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO *agg_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    agg_level_i,
    agg_index_i,
    current_hr,
    current_cl,
    current_fq,
    nof_sub_flows = 0,
    agg_id,
    agg_flow_id,
    max_agg_id;
  SOC_PETRA_AQFM_AGG_ITEM_INFO
    *agg_item = NULL;
  SOC_PETRA_AQFM_AGG_BASE_IDS
    agg_base_id;
  SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO
    table_item;
  SOC_PETRA_SCH_GLOBAL_PER1K_INFO
    global_per1k_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGGS_ACTUAL_IDS_SET);

  soc_petra_PETRA_AQFM_AGG_BASE_IDS_clear(&agg_base_id);
  soc_petra_PETRA_SCH_GLOBAL_PER1K_INFO_clear(&global_per1k_info);

  res = soc_petra_aqfm_agg_base_ids_get(
          unit,
          port_id,
          &agg_base_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  current_hr = agg_base_id.hr.min;
  current_cl = agg_base_id.cl.min;
  current_fq = agg_base_id.fq.min;

  for(agg_level_i = 0;agg_level_i < SOC_PETRA_AQFM_NOF_HIERARCY; ++agg_level_i)
  {
    for(agg_index_i = 0; agg_index_i < SOC_PETRA_AQFM_NOF_INDEX; ++agg_index_i)
    {
      agg_item = &(agg_info->item[agg_level_i][agg_index_i]);

      if(!(agg_item->valid))
      {
        continue;
      }

      nof_sub_flows = 0;
      nof_sub_flows = agg_item->flow.sub_flow[0].is_valid ? nof_sub_flows + 1 : nof_sub_flows;
      nof_sub_flows = agg_item->flow.sub_flow[1].is_valid ? nof_sub_flows + 1 : nof_sub_flows;

      switch(agg_item->se.type)
      {
      case SOC_PETRA_SCH_SE_TYPE_HR:
        agg_id = current_hr;
        max_agg_id = agg_base_id.hr.max;
        break;
      case SOC_PETRA_SCH_SE_TYPE_CL:
        agg_id = current_cl;
        max_agg_id = agg_base_id.cl.max;
        break;
      case SOC_PETRA_SCH_SE_TYPE_FQ:
        agg_id = current_fq;
        max_agg_id = agg_base_id.fq.max;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_AQFM_SCH_SUB_FLOW_TYPE_OUT_OF_RANGE_ERR, 4, exit);
      }

      if(agg_id > max_agg_id)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_AQFM_CANT_ALLOC_AGG_ID_ERR, 8, exit);
      }

      agg_item->se.id = agg_id;
      agg_flow_id = soc_petra_sch_se2flow_id(agg_id);

      res = soc_petra_sch_per1k_info_get_unsafe(
              unit,0,
              SOC_PETRA_SCH_FLOW_TO_1K_ID(agg_flow_id),
              &global_per1k_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

      agg_item->flow.sub_flow[0].id = agg_flow_id;
      agg_item->flow.sub_flow[1].id = global_per1k_info.is_odd_even ? agg_flow_id + 1 : agg_flow_id + 2;

      switch(agg_item->se.type)
      {
      case SOC_PETRA_SCH_SE_TYPE_HR:
        current_hr = agg_id + 1;
        break;
      case SOC_PETRA_SCH_SE_TYPE_CL:
        current_cl = agg_id + 1;
        break;
      case SOC_PETRA_SCH_SE_TYPE_FQ:
        current_fq = agg_id + 1;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_AQFM_SCH_SUB_FLOW_TYPE_OUT_OF_RANGE_ERR, 12, exit);
      }

      res = soc_petra_aqfm_credit_sources_get(
              unit,
              port_id,
              &(agg_item->flow),
              &(agg_item->flow)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      table_item.entity_id = agg_item->se.id;
      table_item.credit_source_subflow_0 = agg_item->flow.sub_flow[0].credit_source.id;
      table_item.credit_source_subflow_1 = agg_item->flow.sub_flow[1].credit_source.id;
      table_item.valid = agg_item->valid;

      res = soc_petra_sw_db_auto_agg_info_item_set(
              unit,
              port_id,
              agg_level_i,
              agg_index_i,
              &table_item
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_actual_id_set()",0,0);
}

uint32
  soc_petra_aqfm_agg_port_aggs_open_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO  *agg_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO  *exact_aggs_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    agg_level_i,
    agg_index_i;
  SOC_PETRA_AQFM_AGG_ITEM_INFO
    *agg_item,
    *exact_aggs_item;
  SOC_PETRA_AQFM_AGG_INFO
    auto_agg_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_OPEN_PORT_AGGS);

  soc_petra_PETRA_AQFM_AGG_INFO_clear(&auto_agg_info);

  ret = SOC_SAND_OK; sal_memcpy(
          &auto_agg_info,
          agg_info,
          sizeof(SOC_PETRA_AQFM_AGG_INFO)
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 5, exit);

  res = soc_petra_aqfm_agg_actual_id_set(
          unit,
          port_id,
          &auto_agg_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for(agg_level_i = 0; agg_level_i < SOC_PETRA_AQFM_NOF_HIERARCY; ++agg_level_i)
  {
    for(agg_index_i = 0; agg_index_i < SOC_PETRA_AQFM_NOF_INDEX; ++agg_index_i)
    {
      agg_item = &(auto_agg_info.item[agg_level_i][agg_index_i]);
      exact_aggs_item = &(exact_aggs_info->item[agg_level_i][agg_index_i]);

      if(!(agg_item->valid))
      {
        continue;
      }

      res = soc_petra_aqfm_agg_info_item_set(
              unit,
              port_id,
              agg_level_i,
              agg_index_i,
              agg_item,
              &(exact_aggs_item->flow)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_port_aggs_open_unsafe()",port_id,0);
}

uint32
  soc_petra_aqfm_all_aggregates_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN SOC_PETRA_AQFM_AGG_INFO       *p_agg_info
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    res = SOC_SAND_OK;
  uint32
    port_id,
    first_relative_id,
    port_i;
  SOC_PETRA_AQFM_AGG_INFO
    *agg_info = NULL,
    *exact_agg_info = NULL;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_ALL_AGGREGATES_OPEN_UNSAFE);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_info);

  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(agg_info, PETRA_AQFM_AGG_INFO);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(exact_agg_info, PETRA_AQFM_AGG_INFO);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  res = soc_petra_aqfm_port_first_relative_id_get(
          unit,
          &first_relative_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for(port_i = first_relative_id; port_i < auto_info.max_nof_ports_per_fap + first_relative_id; ++port_i)
  {
    res = soc_petra_aqfm_port_relative2actual_get(
            unit,
            port_i,
            &port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    ret = SOC_SAND_OK; sal_memcpy(
            agg_info,
            p_agg_info,
            sizeof(SOC_PETRA_AQFM_AGG_INFO)
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 40, exit);

    res = soc_petra_aqfm_agg_port_aggs_open_unsafe(
            unit,
            port_id,
            agg_info,
            exact_agg_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:

  SOC_PETRA_FREE(agg_info);
  SOC_PETRA_FREE(exact_agg_info);

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_all_aggregates_open_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_agg_port_aggs_close_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  )
{
  uint32
    res = SOC_SAND_OK;
  int32
    agg_level_i;
  uint32
    agg_index_i;
  SOC_PETRA_AQFM_AGG_ITEM_INFO
    agg_item;
  SOC_PETRA_SCH_FLOW
    exact_flow;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_CLOSE_PORT_AGGS);

  soc_petra_PETRA_SCH_FLOW_clear(&exact_flow);

  for(agg_level_i = SOC_PETRA_AQFM_NOF_HIERARCY - 1; agg_level_i >= 0; --agg_level_i)
  {
    for(agg_index_i = 0; agg_index_i < SOC_PETRA_AQFM_NOF_INDEX; ++agg_index_i)
    {
      res = soc_petra_aqfm_agg_info_item_get(
              unit,
              port_id,
              agg_level_i,
              agg_index_i,
              &agg_item
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if(!agg_item.valid)
      {
        continue;
      }

      agg_item.valid = FALSE;
      agg_item.se.state = SOC_PETRA_SCH_SE_STATE_DISABLE;

      res = soc_petra_aqfm_agg_info_item_set(
              unit,
              port_id,
              agg_level_i,
              agg_index_i,
              &agg_item,
              &exact_flow
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_petra_sch_flow_delete_unsafe(
              unit,
              agg_item.flow.sub_flow[0].id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_port_aggs_close_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_agg_port_aggs_update_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO       *aggs_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO       *exact_aggs_info,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO      *flows_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO      *exact_flows_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    agg_level_i,
    agg_index_i;
  const SOC_PETRA_AQFM_AGG_ITEM_INFO
    *aggs_info_item;
  SOC_PETRA_AQFM_AGG_ITEM_INFO
    *exact_aggs_info_item;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_UPDATE_PORT_AGGS);

  SOC_SAND_CHECK_NULL_INPUT(aggs_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_aggs_info);
  SOC_SAND_CHECK_NULL_INPUT(flows_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_flows_info);

  res = soc_petra_aqfm_agg_port_aggs_open_unsafe(
          unit,
          port_id,
          aggs_info,
          exact_aggs_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  for(agg_level_i = 0; agg_level_i < SOC_PETRA_AQFM_NOF_HIERARCY; ++agg_level_i)
  {
    for(agg_index_i = 0; agg_index_i < SOC_PETRA_AQFM_NOF_INDEX; ++agg_index_i)
    {
      aggs_info_item = &(aggs_info->item[agg_level_i][agg_index_i]);
      exact_aggs_info_item = &(exact_aggs_info->item[agg_level_i][agg_index_i]);

      if(!(aggs_info_item->valid))
      {
        continue;
      }

      res = soc_petra_aqfm_agg_port_single_aggs_update_unsafe(
              unit,
              port_id,
              agg_level_i,
              agg_index_i,
              aggs_info_item,
              exact_aggs_info_item
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_port_aggs_update_unsafe()",unit,port_id);
}

uint32
  soc_petra_aqfm_agg_port_single_aggs_update_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  port_id,
    SOC_SAND_IN  uint32                  agg_level,
    SOC_SAND_IN  uint32                  agg_index,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_ITEM_INFO  *aggs_info_item,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_ITEM_INFO  *exact_aggs_info_item
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    agg_flow_id;
  SOC_PETRA_AQFM_AGG_ITEM_INFO
    agg_item_info_to_set;
  SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO
    agg_table_item_info;
  SOC_PETRA_SCH_GLOBAL_PER1K_INFO
    global_per1k_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_UPDATE_PORT_AGGS);

  SOC_SAND_CHECK_NULL_INPUT(aggs_info_item);
  SOC_SAND_CHECK_NULL_INPUT(exact_aggs_info_item);

  soc_petra_PETRA_SCH_GLOBAL_PER1K_INFO_clear(&global_per1k_info);
  soc_petra_PETRA_AQFM_AGG_ITEM_INFO_clear(&agg_item_info_to_set);

  ret = soc_petra_sw_db_auto_agg_info_item_get(
          unit,
          port_id,
          agg_level,
          agg_index,
          &agg_table_item_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memcpy(
          &agg_item_info_to_set,
          aggs_info_item,
          sizeof(SOC_PETRA_AQFM_AGG_ITEM_INFO)
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  res = soc_petra_aqfm_credit_sources_get(
          unit,
          port_id,
          &(aggs_info_item->flow),
          &(agg_item_info_to_set.flow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  agg_item_info_to_set.se.id = agg_table_item_info.entity_id;

  agg_flow_id = soc_petra_sch_se2flow_id(agg_table_item_info.entity_id);

  res = soc_petra_sch_per1k_info_get_unsafe(
          unit,0,
          SOC_PETRA_SCH_FLOW_TO_1K_ID(agg_flow_id),
          &global_per1k_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  agg_item_info_to_set.flow.sub_flow[0].id = agg_flow_id;
  agg_item_info_to_set.flow.sub_flow[1].id = global_per1k_info.is_odd_even ? agg_flow_id + 1 : agg_flow_id + 2;

  res = soc_petra_aqfm_agg_info_item_set(
          unit,
          port_id,
          agg_level,
          agg_index,
          &agg_item_info_to_set,
          &(exact_aggs_info_item->flow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_agg_port_single_aggs_update_unsafe()",unit,port_id);
}

#if SOC_PETRA_DEBUG

void
  soc_petra_aqfm_agg_port_scheme_aggregates_print(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 port_id
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    agg_level_i,
    agg_index_i;
  SOC_PETRA_AQFM_AGG_ITEM_INFO
    agg_item;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_AGG_PORT_SCHEME_AGGREGATES_PRINT);

  soc_petra_PETRA_AQFM_AGG_ITEM_INFO_clear(&agg_item);

  for(agg_level_i = 0; agg_level_i < SOC_PETRA_AQFM_NOF_HIERARCY; ++agg_level_i)
  {
    for(agg_index_i = 0; agg_index_i < SOC_PETRA_AQFM_NOF_INDEX; ++agg_index_i)
    {
      res = soc_petra_aqfm_agg_info_item_get(
              unit,
              port_id,
              agg_level_i,
              agg_index_i,
              &agg_item
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      if(!(agg_item.valid))
      {
        continue;
      }

      soc_petra_PETRA_SCH_SE_INFO_print(&(agg_item.se));
      soc_petra_PETRA_SCH_FLOW_print(&(agg_item.flow), TRUE);
    }
  }

exit:

  if (ex != no_err)
  {
    soc_sand_os_printf("Error occurred in %s.\n\r", "soc_petra_api_auto_port_scheme_aggregates_print");
  }
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
