/* $Id: petra_api_ingress_packet_queuing.c,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_ingress_packet_queuing.c
*
* MODULE PREFIX:  soc_petra_ipq
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

#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/cosq.h>


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
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_explicit_mapping_mode_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_ipq_explicit_mapping_mode_info_verify(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_explicit_mapping_mode_info_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_explicit_mapping_mode_info_set()",0,0);
}

/*********************************************************************
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_explicit_mapping_mode_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_explicit_mapping_mode_info_get()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_traffic_class_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          new_class
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_ipq_traffic_class_map_verify(
    unit,
    tr_cls_ndx,
    new_class
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_traffic_class_map_set_unsafe(
    unit,
    tr_cls_ndx,
    new_class
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_traffic_class_map_set()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_traffic_class_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_IPQ_TR_CLS          *new_class
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_TRAFFIC_CLASS_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(new_class);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_traffic_class_map_get_unsafe(
    unit,
    tr_cls_ndx,
    new_class
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_traffic_class_map_get()",0,0);
}

/*********************************************************************
*     Sets the destination-id packets mapping to queue
*     information. Doesn't affect packets that arrive with
*     explicit queue-id in the header. Each destination ID is
*     mapped to a base_queue, when the packet is stored in
*     queue: base_queue + class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_destination_id_packets_base_queue_id_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  uint32              dest_ndx,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint8               valid,
     SOC_SAND_IN  uint8               sw_only,
     SOC_SAND_IN  uint32              base_queue
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_ipq_destination_id_packets_base_queue_id_verify(
    unit,
    dest_ndx,
    valid,
    base_queue
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_destination_id_packets_base_queue_id_set_unsafe(
    unit,
    dest_ndx,
    valid,
    base_queue
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_destination_id_packets_base_queue_id_set()",0,0);
}

/*********************************************************************
*     Sets the destination-id packets mapping to queue
*     information. Doesn't affect packets that arrive with
*     explicit queue-id in the header. Each destination ID is
*     mapped to a base_queue, when the packet is stored in
*     queue: base_queue + class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_destination_id_packets_base_queue_id_get(
   SOC_SAND_IN  int                 unit,
   SOC_SAND_IN  uint32              dest_ndx,
   SOC_SAND_IN  int                 core,
   SOC_SAND_OUT uint8               *valid,
   SOC_SAND_OUT uint8               *sw_only,
   SOC_SAND_OUT uint32              *base_queue
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(valid);
  SOC_SAND_CHECK_NULL_INPUT(base_queue);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_destination_id_packets_base_queue_id_get_unsafe(
    unit,
    dest_ndx,
    valid,
    base_queue
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_destination_id_packets_base_queue_id_get()",0,0);
}

/*********************************************************************
*     Map 1K of queues (256 quartets) to be in interdigitated
*     mode or not. Queue Quartets that configured to be in
*     interdigitated mode should only be configured with
*     interdigitated flow quartets, and the other-way around.
*     When interdigitated mode is set, all queue quartets
*     range are reset using soc_petra_ipq_quartet_reset(), in
*     order to prevent illegal interdigitated/composite state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_interdigitated_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_IN  uint8                 is_interdigitated
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  res = soc_petra_ipq_queue_interdigitated_mode_verify(
    unit,
    k_queue_ndx,
    is_interdigitated
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_queue_interdigitated_mode_set_unsafe(
    unit,
    k_queue_ndx,
    is_interdigitated
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_interdigitated_mode_set()",0,0);
}

/*********************************************************************
*     Map 1K of queues (256 quartets) to be in interdigitated
*     mode or not. Queue Quartets that configured to be in
*     interdigitated mode should only be configured with
*     interdigitated flow quartets, and the other-way around.
*     When interdigitated mode is set, all queue quartets
*     range are reset using soc_petra_ipq_quartet_reset(), in
*     order to prevent illegal interdigitated/composite state.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_interdigitated_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_OUT uint8                 *is_interdigitated
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_INTERDIGITATED_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_interdigitated);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_queue_interdigitated_mode_get_unsafe(
    unit,
    k_queue_ndx,
    is_interdigitated
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_interdigitated_mode_get()",0,0);
}

/*********************************************************************
*     Map a queues-quartet to system port and flow-quartet(s).
*     This function should only be called after calling the
*     soc_petra_ipq_queue_interdigitated_mode_set()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_to_flow_mapping_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_queue_to_flow_mapping_verify(
    unit,
    queue_quartet_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_ipq_queue_to_flow_mapping_set_unsafe(
    unit,
    queue_quartet_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_to_flow_mapping_set()",0,0);
}

/*********************************************************************
*     Map a queues-quartet to system port and flow-quartet(s).
*     This function should only be called after calling the
*     soc_petra_ipq_queue_interdigitated_mode_set()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_to_flow_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_OUT SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_TO_FLOW_MAPPING_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_queue_to_flow_mapping_get_unsafe(
    unit,
    queue_quartet_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_to_flow_mapping_get()",0,0);
}

/*********************************************************************
*     Unmap a queues-quartet, by mapping it to invalid
*     destination. Also, flush all the queues in the quartet.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_queue_qrtt_unmap(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  queue_quartet_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUEUE_QRTT_UNMAP);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_queue_qrtt_unmap_unsafe(
          unit,
          queue_quartet_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_queue_qrtt_unmap()",queue_quartet_ndx,0);
}

/*********************************************************************
*     Resets a quartet to default values.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ipq_quartet_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_quartet_ndx
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_QUARTET_RESET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;


  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_quartet_reset_unsafe(
    unit,
    queue_quartet_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_quartet_reset()",0,0);
}


void
  soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_IPQ_BASEQ_MAP_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IPQ_BASEQ_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_BASEQ_MAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_IPQ_QUARTET_MAP_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_QUARTET_MAP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32
  soc_petra_ipq_attached_flow_port_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 queue_ndx,
    SOC_SAND_OUT uint32 *flow_id,
    SOC_SAND_OUT uint32 *sys_port
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_IPQ_ATTACHED_FLOW_PORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(flow_id);
  SOC_SAND_CHECK_NULL_INPUT(sys_port);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ipq_attached_flow_port_get_unsafe(
          unit,
          queue_ndx,
          flow_id,
          sys_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ipq_attached_flow_port_get()",0,0);
}

#if SOC_PETRA_DEBUG_IS_LVL1


uint32
  soc_petra_ips_non_empty_queues_info_get(
   SOC_SAND_IN  int                unit,
   SOC_SAND_IN  uint32                first_queue,
   SOC_SAND_IN  uint32                max_array_size,
   SOC_SAND_OUT soc_ips_queue_info_t* queues,
   SOC_SAND_OUT uint32*               nof_queues_filled,
   SOC_SAND_OUT uint32*               next_queue,
   SOC_SAND_OUT uint32*               reached_end
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_ips_non_empty_queues_info_get_unsafe(
          unit,
          first_queue,
          max_array_size,
          queues,
          nof_queues_filled,
          next_queue,
          reached_end
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ips_non_empty_queues_print()",0,0);
}

const char*
  soc_petra_PETRA_IPQ_TR_CLS_RNG_to_string(
    SOC_SAND_IN SOC_PETRA_IPQ_TR_CLS_RNG enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_IPQ_TR_CLS_MIN:
    str = "TR_CLS_MIN";
  break;

  case SOC_PETRA_IPQ_TR_CLS_MAX:
    str = "TR_CLS_MAX";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


void
  soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(
    SOC_SAND_IN  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_IPQ_BASEQ_MAP_INFO_print(
    SOC_SAND_IN  SOC_PETRA_IPQ_BASEQ_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_BASEQ_MAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_IPQ_QUARTET_MAP_INFO_print(
    SOC_SAND_IN  SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_IPQ_QUARTET_MAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

