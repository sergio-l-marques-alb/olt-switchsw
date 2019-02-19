/* $Id: petra_auto_queue_flow_mgmt_queue.c,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_auto_queue_flow_mgmt_queue.c
*
* MODULE PREFIX:  soc_petra_aqfm_queue
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
#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_api_ingress_packet_queuing.h>
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
  soc_petra_aqfm_queue_info_default_get(
    SOC_SAND_OUT SOC_PETRA_AQFM_QUEUE_INFO     *p_queue_info
  )
{
  uint32
    class_i;
  SOC_PETRA_ITM_QUEUE_INFO
    *p_queue = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUE_INFO_DEFAULT_GET);

  SOC_SAND_CHECK_NULL_INPUT(p_queue_info);

  for(class_i = 0; class_i < SOC_PETRA_NOF_TRAFFIC_CLASSES; ++class_i)
  {
    p_queue = &(p_queue_info->queues[class_i]);
    soc_petra_PETRA_ITM_QUEUE_INFO_clear(p_queue);
    p_queue->cr_req_type_ndx = SOC_PETRA_ITM_QT_NDX_00;
    p_queue->credit_cls = SOC_PETRA_ITM_CR_DISCNT_CLS_NDX_00;
    p_queue->rate_cls = 0;
    p_queue->vsq_connection_cls = 0;
    p_queue->vsq_traffic_cls = 0;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_queue_info_default_get()",0,0);
}

uint32
  soc_petra_aqfm_queues_open_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO *auto_queue_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    queue_id,
    dest_id,
    system_physical_port,
    flow_id,
    flow_qrtt;
  uint32
    fap_id,
    class_i;
  SOC_PETRA_IPQ_QUARTET_MAP_INFO
    quartet_map_info;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;
  SOC_PETRA_ITM_CR_REQUEST_INFO
    itm_cr_request_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUES_OPEN);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);
  soc_petra_PETRA_IPQ_QUARTET_MAP_INFO_clear(&quartet_map_info);
  soc_petra_PETRA_ITM_CR_REQUEST_INFO_clear(&itm_cr_request_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  for(class_i = 0; class_i < auto_sys_info.nof_traffic_classes; ++class_i)
  {
    res = soc_petra_aqfm_destination_id_get(
            unit,
            destination_fap_id,
            destination_port_id,
            &dest_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_aqfm_queue_id_get(
            unit,
            dest_id,
            class_i,
            &queue_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_mgmt_system_fap_id_get(
            unit,
            &fap_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    res = soc_petra_aqfm_flow_id_get(
            unit,
            fap_id,
            destination_port_id,
            class_i,
            &flow_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_aqfm_system_physical_port_id_get(
            unit,
            dest_id,
            &system_physical_port
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = soc_petra_ipq_quartet_reset_unsafe(
            unit,
            SOC_SAND_DIV_ROUND_DOWN(queue_id, 4)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    flow_qrtt = SOC_SAND_DIV_ROUND_DOWN(flow_id, 8) * 2;

    quartet_map_info.flow_quartet_index = flow_qrtt;
    quartet_map_info.is_composite = TRUE;
    quartet_map_info.system_physical_port = system_physical_port;
    res = soc_petra_ipq_queue_to_flow_mapping_set_unsafe(
            unit,
            SOC_SAND_DIV_ROUND_DOWN(queue_id, 4),
            &quartet_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    res = soc_petra_itm_queue_info_set_unsafe(
            unit,
            queue_id,
            &auto_queue_info->queues[class_i]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    /*
     * Only the class 0 affects packet to queue mapping.
     * Other classes queue number is base_class + class_i
     */
    if(class_i == 0)
    {
      res = soc_petra_ipq_destination_id_packets_base_queue_id_set_unsafe(
              unit,
              system_physical_port,
              TRUE,
              queue_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_queues_open_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_queues_update_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO *queues_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    queue_id,
    dest_id;
  uint32
    class_i;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUES_UPDATE);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  for(class_i = 0; class_i < auto_sys_info.nof_traffic_classes; ++class_i)
  {
    res = soc_petra_aqfm_destination_id_get(
            unit,
            destination_fap_id,
            destination_port_id,
            &dest_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    res = soc_petra_aqfm_queue_id_get(
            unit,
            dest_id,
            class_i,
            &queue_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_petra_itm_queue_info_set_unsafe(
            unit,
            queue_id,
            &queues_info->queues[class_i]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_queues_update_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_queues_close_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    dest_id,
    queue_id;
  uint32
    class_i;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;
  SOC_PETRA_IPQ_QUARTET_MAP_INFO
    quartet_map_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUES_CLOSE);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);
  soc_petra_PETRA_IPQ_QUARTET_MAP_INFO_clear(&quartet_map_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  for(class_i = 0; class_i < auto_sys_info.nof_traffic_classes; ++class_i)
  {
    res = soc_petra_aqfm_destination_id_get(
            unit,
            destination_fap_id,
            destination_port_id,
            &dest_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_petra_aqfm_queue_id_get(
            unit,
            dest_id,
            class_i,
            &queue_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_petra_ipq_queue_to_flow_mapping_get_unsafe(
            unit,
            SOC_SAND_DIV_ROUND_DOWN(queue_id, 4),
            &quartet_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    quartet_map_info.flow_quartet_index = SOC_PETRA_IPQ_INVALID_FLOW_QUARTET;
    quartet_map_info.is_composite = TRUE;

    res = soc_petra_ipq_queue_to_flow_mapping_set_unsafe(
            unit,
            SOC_SAND_DIV_ROUND_DOWN(queue_id, 4),
            &quartet_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_queues_close_unsafe()",0,0);
}

uint32
  soc_petra_aqfm_all_queues_open_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO    *p_queues_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    dest_fap_i,
    port_id,
    first_relative_id,
    port_i;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_ALL_QUEUES_OPEN_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(p_queues_info);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 50, exit);

  for(dest_fap_i = 0; dest_fap_i < auto_sys_info.max_nof_faps_in_system; ++dest_fap_i)
  {
    res = soc_petra_aqfm_port_first_relative_id_get(
            unit,
            &first_relative_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    for(port_i = first_relative_id; port_i < auto_sys_info.max_nof_ports_per_fap + first_relative_id; ++port_i)
    {
      res = soc_petra_aqfm_port_relative2actual_get(
              unit,
              port_i,
              &port_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      res = soc_petra_aqfm_queues_open_unsafe(
              unit,
              dest_fap_i,
              port_id,
              p_queues_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_aqfm_all_queues_open_unsafe()",0,0);
}

#if SOC_PETRA_DEBUG

void
  soc_petra_aqfm_queue_port_scheme_queues_print(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  port_id,
    SOC_SAND_IN  uint8  minimal_printing
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    dest_id,
    queue_id,
    system_physical_port;
  uint32
    fap_id,
    nof_faps_to_print,
    dest_fap_i,
    class_i;
  SOC_PETRA_AQFM_SYSTEM_INFO
    auto_sys_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_AQFM_QUEUE_PORT_SCHEME_QUEUES_PRINT);

  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(&auto_sys_info);

  ret = soc_petra_sw_db_auto_scheme_info_get(
          unit,
          &auto_sys_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  nof_faps_to_print = (minimal_printing == TRUE ? 1 : auto_sys_info.max_nof_faps_in_system);

  for(dest_fap_i = 0; dest_fap_i < nof_faps_to_print; ++dest_fap_i)
  {
    for(class_i = 0;class_i < auto_sys_info.nof_traffic_classes; ++class_i)
    {
      res = soc_petra_aqfm_destination_id_get(
              unit,
              dest_fap_i,
              port_id,
              &dest_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      res = soc_petra_aqfm_queue_id_get(
              unit,
              dest_id,
              class_i,
              &queue_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_petra_aqfm_system_physical_port_id_get(
              unit,
              dest_id,
              &system_physical_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_petra_mgmt_system_fap_id_get(
              unit,
              &fap_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

      soc_sand_os_printf(
        "  Local FAP %d Dest Fap %d, Port %d, Class %d Queue ID %u:\n\r",
        fap_id,
        dest_fap_i,
        port_id,
        class_i,
        queue_id
      );
      if(class_i == 0)
      {
        soc_sand_os_printf(
          "    Destination ID = %u, \n\r",
          system_physical_port
        );
      }
    }
  }

exit:

  if (ex != no_err)
  {
    soc_sand_os_printf("Error occurred in %s.\n\r", "soc_petra_aqfm_queue_port_scheme_queues_print");
  }
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
