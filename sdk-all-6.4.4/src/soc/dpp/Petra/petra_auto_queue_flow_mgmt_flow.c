/* $Id: petra_auto_queue_flow_mgmt_flow.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_auto_queue_flow_mgmt_flow.c
*
* MODULE PREFIX:  soc_petra_aqfm_flow
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
#include <soc/dpp/Petra/petra_scheduler_flows.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_mgmt.h>

#include <soc/dpp/Petra/petra_api_auto_queue_flow_mgmt.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_port.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_aggregate.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_flow.h>
#include <soc/dpp/Petra/petra_auto_queue_flow_mgmt_queue.h>

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
  soc_petra_aqfm_flow_default_get(
    SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO      *p_flows_info
  )
{
  SOC_PETRA_SCH_FLOW
    *p_flow = NULL;
  uint32
    class_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_FLOWS_GET_DEFAULT);
  SOC_SAND_CHECK_NULL_INPUT(p_flows_info);

  for(class_i = 0; class_i < SOC_PETRA_NOF_TRAFFIC_CLASSES; ++class_i)
  {
    p_flow = &(p_flows_info->flows[class_i]);
    soc_petra_PETRA_SCH_FLOW_clear(p_flow);

    p_flow->flow_type       = SOC_PETRA_FLOW_SIMPLE;
    p_flow->is_slow_enabled = TRUE;
    p_flow->sub_flow[0].is_valid = TRUE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_flows_default_get()",0,0);
}

uint32
  soc_petra_aqfm_flow_physical_ids_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            source_fap_id,
    SOC_SAND_IN  uint32            port_id,
    SOC_SAND_IN  uint32            class_id,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW      *logical_flow,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW      *physical_flow
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    res = SOC_SAND_OK;
  uint32
    physical_flow_id;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_FLOW_SET_PHYSICAL_IDS);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  res = soc_petra_aqfm_flow_id_get(
          unit,
          source_fap_id,
          port_id,
          class_id,
          &physical_flow_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  physical_flow->sub_flow[0].id = physical_flow_id;
  physical_flow->sub_flow[1].id = physical_flow_id + 1;

  res = soc_petra_aqfm_credit_sources_get(
          unit,
          port_id,
          logical_flow,
          physical_flow
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_flow_physical_ids_set()",0,0);
}

uint32
  soc_petra_aqfm_flow_open_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO  *flow_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    fap_id,
    class_i;
  SOC_PETRA_SCH_FLOW
    physical_flow,
    exact_flow_given;
  uint32
    source_fap_i;
  uint32
    source_dest_id,
    source_queue_id;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;
  SOC_PETRA_SCH_QUARTET_MAPPING_INFO
    quartet_mapping_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_FLOW_PORT_FLOWS_OPEN);

  soc_petra_PETRA_SCH_FLOW_clear(&physical_flow);
  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);
  soc_petra_PETRA_SCH_QUARTET_MAPPING_INFO_clear(&quartet_mapping_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  for(source_fap_i = 0; source_fap_i < auto_sys_info.max_nof_faps_in_system; ++source_fap_i)
  {
    for(class_i = 0; class_i < auto_sys_info.nof_traffic_classes; ++class_i)
    {
      ret = SOC_SAND_OK; sal_memcpy(
              &physical_flow,
              &(flow_info->flows[class_i]),
              sizeof(physical_flow)
            );
      SOC_SAND_CHECK_FUNC_RESULT(ret, 30, exit);

      res = soc_petra_aqfm_flow_physical_ids_set(
              unit,
              source_fap_i,
              port_id,
              class_i,
              &(flow_info->flows[class_i]),
              &(physical_flow)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_petra_sch_flow_set_unsafe(
              unit,
              physical_flow.sub_flow[0].id,
              &physical_flow,
              &exact_flow_given
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      if (physical_flow.sub_flow[0].is_valid)
      {
        res = soc_petra_sch_flow_slow_enable_set(
                unit,
                physical_flow.sub_flow[0].id,
                flow_info->flows[class_i].is_slow_enabled
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }

      if (physical_flow.sub_flow[1].is_valid)
      {
        res = soc_petra_sch_flow_slow_enable_set(
                unit,
                physical_flow.sub_flow[1].id,
                flow_info->flows[class_i].is_slow_enabled
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
      }

      /*
       * Flow to queue mapping: Since we map quartets, there's no need to
       * map all the flows, but only the ones that can be divided by 4.
       */
      if ((physical_flow.sub_flow[0].id) % (4 * SOC_PETRA_SCH_NOF_SUB_FLOWS) == 0)
      {
        res = soc_petra_mgmt_system_fap_id_get_unsafe(
                unit,
                &fap_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(ret, 40, exit);

        res = soc_petra_aqfm_destination_id_get(
                unit,
                fap_id,
                port_id,
                &source_dest_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(ret, 40, exit);

        res = soc_petra_aqfm_queue_id_get(
                unit,
                source_dest_id,
                class_i,
                &source_queue_id
              );
        SOC_SAND_CHECK_FUNC_RESULT(ret, 40, exit);

        quartet_mapping_info.base_q_qrtt_id = SOC_PETRA_IPQ_Q_TO_QRTT_ID(source_queue_id);
        quartet_mapping_info.fip_id = source_fap_i;
        quartet_mapping_info.is_composite = TRUE;
        res = soc_petra_sch_flow_to_queue_mapping_set_unsafe(
                unit,
                SOC_PETRA_SCH_FLOW_TO_QRTT_ID(physical_flow.sub_flow[0].id),
                SOC_PETRA_SCH_NOF_SUB_FLOWS,
                &quartet_mapping_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_flow_open_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_flow_close_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    flow_id;
  uint32
    class_i;
  uint32
    source_fap_i;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;
  SOC_PETRA_SCH_FLOW
    flow,
    exact_flow;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_CLOSE_PORT_FLOWS);


  soc_petra_PETRA_SCH_FLOW_clear(&flow);
  soc_petra_PETRA_SCH_FLOW_clear(&exact_flow);
  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  for(source_fap_i = 0; source_fap_i < auto_sys_info.max_nof_faps_in_system; ++source_fap_i)
  {
    for(class_i = 0; class_i < auto_sys_info.nof_traffic_classes; ++class_i)
    {
      res = soc_petra_aqfm_flow_id_get(
              unit,
              source_fap_i,
              port_id,
              class_i,
              &flow_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      flow.flow_type = SOC_PETRA_FLOW_SIMPLE;
      flow.sub_flow[0].is_valid = FALSE;
      flow.sub_flow[1].is_valid = FALSE;
      res = soc_petra_sch_flow_set_unsafe(
              unit,
              flow_id,
              &flow,
              &exact_flow
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_close_port_flows()",0,0);
}

uint32
  soc_petra_aqfm_flow_update_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO   *flows_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO   *exact_flows_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_UPDATE_PORT_FLOWS);

  SOC_SAND_CHECK_NULL_INPUT(flows_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_flows_info);

  res = soc_petra_aqfm_flow_close_unsafe(
          unit,
          port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_aqfm_flow_open_unsafe(
          unit,
          port_id,
          flows_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_flow_update_unsafe()",unit,port_id);
}

uint32
  soc_petra_aqfm_all_flows_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO     *p_flow_info
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    res = SOC_SAND_OK;
  uint32
    port_id,
    ports_num,
    first_relative_id,
    port_i;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_ALL_FLOWS_OPEN_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(p_flow_info);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);
  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  ports_num = auto_sys_info.max_nof_ports_per_fap;

  res = soc_petra_aqfm_port_first_relative_id_get(
          unit,
          &first_relative_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for(port_i = first_relative_id; port_i < ports_num + first_relative_id; ++port_i)
  {
    res = soc_petra_aqfm_port_relative2actual_get(
            unit,
            port_i,
            &port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_aqfm_flow_open_unsafe(
            unit,
            port_id,
            p_flow_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_all_flows_open_unsafe()",0,0);
}

#if SOC_PETRA_DEBUG

void
  soc_petra_aqfm_flow_print_port_scheme_flows(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 port_id,
    SOC_SAND_IN  uint32 minimal_printing
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    flow_id;
  uint32
    fap_id,
    nof_faps_to_print,
    source_fap_i,
    class_i;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;
  SOC_PETRA_SCH_QUARTET_MAPPING_INFO
    quartet_mapping_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_API_AUTO_PRINT_PORT_SCHEME_FLOWS);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);
  soc_petra_PETRA_SCH_QUARTET_MAPPING_INFO_clear(&quartet_mapping_info);

  res = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  nof_faps_to_print = auto_sys_info.max_nof_faps_in_system;
  if(minimal_printing)
  {
    nof_faps_to_print = 1;
  }

  for(source_fap_i = 0; source_fap_i < nof_faps_to_print; ++source_fap_i)
  {
    for(class_i = 0;class_i < auto_sys_info.nof_traffic_classes;class_i++)
    {
      res = soc_petra_aqfm_flow_id_get(
              unit,
              source_fap_i,
              port_id,
              class_i,
              &flow_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = soc_petra_mgmt_system_fap_id_get(
              unit,
              &fap_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      soc_sand_os_printf(
        "  Local FAP %d Source Fap %d, Port %d, Class %d Flow %u:\n\r",
        fap_id,
        source_fap_i,
        port_id,
        class_i,
        flow_id
      );

      res = soc_petra_sch_flow_to_queue_mapping_get(
              unit,0,
              flow_id,
              &quartet_mapping_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      soc_petra_PETRA_SCH_QUARTET_MAPPING_INFO_print(&quartet_mapping_info);
    }
  }

exit:

  if (ex != no_err)
  {
    soc_sand_os_printf("Error occurred in %s.\n\r", "soc_petra_aqfm_flow_print_port_scheme_flows");
  }
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0)
}

#endif /* SOC_PETRA_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
