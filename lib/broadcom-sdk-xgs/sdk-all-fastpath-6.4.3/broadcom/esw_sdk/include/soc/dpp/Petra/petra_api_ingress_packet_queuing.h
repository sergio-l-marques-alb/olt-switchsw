/* $Id: petra_api_ingress_packet_queuing.h,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_api_ingress_packet_queuing.h
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


#ifndef __SOC_PETRA_API_INGRESS_PACKET_QUEUING_INCLUDED__
/* { */
#define __SOC_PETRA_API_INGRESS_PACKET_QUEUING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/cosq.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_IPQ_TR_CLS SOC_TMC_IPQ_TR_CLS


/* } */

/*************
 * MACROS    *
 *************/
/* { */
#define SOC_PETRA_IPQ_Q_TO_QRTT_ID(que_id)                 SOC_TMC_IPQ_Q_TO_QRTT_ID(que_id)
#define SOC_PETRA_IPQ_QRTT_TO_Q_ID(q_que_id)               SOC_TMC_IPQ_QRTT_TO_Q_ID(q_que_id)

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef SOC_TMC_IPQ_EXPLICIT_MAPPING_MODE_INFO                 SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO;
typedef SOC_TMC_IPQ_BASEQ_MAP_INFO                             SOC_PETRA_IPQ_BASEQ_MAP_INFO;
typedef SOC_TMC_IPQ_QUARTET_MAP_INFO                           SOC_PETRA_IPQ_QUARTET_MAP_INFO;

#define SOC_PETRA_IPQ_TR_CLS_MIN        SOC_TMC_IPQ_TR_CLS_MIN
#define SOC_PETRA_IPQ_TR_CLS_MAX        SOC_TMC_IPQ_TR_CLS_MAX
typedef SOC_TMC_IPQ_TR_CLS_RNG          SOC_PETRA_IPQ_TR_CLS_RNG;

#define SOC_PETRA_IPQ_TR_CLS_RNG_LAST   SOC_TMC_IPQ_TR_CLS_RNG_LAST

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
*     soc_petra_ipq_explicit_mapping_mode_info_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info -
*     Mapping information of packet with explicit flow header
*     to queues
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_explicit_mapping_mode_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_explicit_mapping_mode_info_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Sets the Explicit Flow Unicast packets mapping to queue.
*     Doesn't affect packets that arrive with destination_id
*     in the header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_OUT SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info -
*     Mapping information of packet with explicit flow header
*     to queues
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_explicit_mapping_mode_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_traffic_class_map_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx -
*     tr_cls_ndx-the traffic class index, for which to map a
*     new class. SOC_SAND_IN SOC_PETRA_IPQ_TR_CLS class - The new
*     class that is mapped to the tr_cls_ndx.
*  SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          class -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_traffic_class_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          new_class
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_traffic_class_map_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Maps the embedded traffic class in the packet header to
*     a logical traffic class. This logical traffic class will
*     be further used for traffic management. Note that a class
*     that is mapped to class '0' is equivalent to disabling
*     adding the class.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx -
*     tr_cls_ndx-the traffic class index, for which to map a
*     new class. SOC_SAND_IN SOC_PETRA_IPQ_TR_CLS class - The new
*     class that is mapped to the tr_cls_ndx.
*  SOC_SAND_OUT SOC_PETRA_IPQ_TR_CLS          *class -
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_traffic_class_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IPQ_TR_CLS          tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_IPQ_TR_CLS          *new_class
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_destination_id_packets_base_queue_id_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Sets the destination-id packets mapping to queue
*     information. Doesn't affect packets that arrive with
*     explicit queue-id in the header. Each destination ID is
*     mapped to a base_queue, when the packet is stored in
*     queue: base_queue + class
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  dest_ndx -
*     dest_ndx - The destination from the packet
*     System-Port-Destination ID. SOC_SAND_IN
*  SOC_SAND_IN  uint8                 valid -
*     If set the specific queue is valid, Otherwise the queue
*     is set to be invalid and packets are not sent to it.
*     SOC_SAND_IN
*  SOC_SAND_IN  uint32                  base_queue -
*     base_queue - Packet is stored in queue: base_queue +
*     class. Valid values: 0 - 32K.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_destination_id_packets_base_queue_id_set(
     SOC_SAND_IN  int                 unit,
     SOC_SAND_IN  int                 core,
     SOC_SAND_IN  uint32              dest_ndx,
     SOC_SAND_IN  uint8               valid,
     SOC_SAND_IN  uint8               sw_only,
     SOC_SAND_IN  uint32              base_queue

  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_destination_id_packets_base_queue_id_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Sets the destination-id packets mapping to queue
*     information. Doesn't affect packets that arrive with
*     explicit queue-id in the header. Each destination ID is
*     mapped to a base_queue, when the packet is stored in
*     queue: base_queue + class
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  dest_ndx -
*     dest_ndx - The destination from the packet
*     System-Port-Destination ID. SOC_SAND_IN
*  SOC_SAND_OUT uint8                 *valid -
*     If set the specific queue is valid, Otherwise the queue
*     is set to be invalid and packets are not sent to it.
*     SOC_SAND_IN
*  SOC_SAND_OUT uint32                  *base_queue -
*     base_queue - Packet is stored in queue: base_queue +
*     class. Valid values: 0 - 32K.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_destination_id_packets_base_queue_id_get(
   SOC_SAND_IN  int                 unit,
   SOC_SAND_IN  int                 core,
   SOC_SAND_IN  uint32              dest_ndx,
   SOC_SAND_OUT uint8               *valid,
   SOC_SAND_OUT uint8               *sw_only,
   SOC_SAND_OUT uint32              *base_queue
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_queue_interdigitated_mode_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Map 1K of queues (256 quartets) to be in interdigitated
*     mode or not. Queue Quartets that configured to be in
*     interdigitated mode should only be configured with
*     interdigitated flow quartets, and the other-way around.
*     When interdigitated mode is set, all queue quartets
*     range are reset using soc_petra_ipq_quartet_reset(), in
*     order to prevent illegal interdigitated/composite state.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  k_queue_ndx -
*     The K queues to configure are (k_queue_ndx * 1024) -
*     (k_queue_ndx * 1024 + 1023) SOC_SAND_IN
*  SOC_SAND_IN  uint8                 is_interdigitated -
*     If TRUE, the K queues which k_queue_index points to will
*     be set to interdigitated mode.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_queue_interdigitated_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_IN  uint8                 is_interdigitated
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_queue_interdigitated_mode_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Map 1K of queues (256 quartets) to be in interdigitated
*     mode or not. Queue Quartets that configured to be in
*     interdigitated mode should only be configured with
*     interdigitated flow quartets, and the other-way around.
*     When interdigitated mode is set, all queue quartets
*     range are reset using soc_petra_ipq_quartet_reset(), in
*     order to prevent illegal interdigitated/composite state.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  k_queue_ndx -
*     The K queues to configure are (k_queue_ndx * 1024) -
*     (k_queue_ndx * 1024 + 1023) SOC_SAND_IN
*  SOC_SAND_OUT uint8                 *is_interdigitated -
*     If TRUE, the K queues which k_queue_index points to will
*     be set to interdigitated mode.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_queue_interdigitated_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  k_queue_ndx,
    SOC_SAND_OUT uint8                 *is_interdigitated
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_queue_to_flow_mapping_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Map a queues-quartet to system port and flow-quartet(s).
*     This function should only be called after calling the
*     soc_petra_ipq_queue_interdigitated_mode_set()
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  queue_quartet_ndx -
*     queue_quartet_ndx - The index of the quartet to be
*     configured, the range of queues in the quartet is:
*     (queue_quartet_ndx * 4) to (queue_quartet_ndx * 4 + 3)
*     SOC_SAND_IN
*  SOC_SAND_IN  SOC_PETRA_IPQ_QUARTET_MAP_INFO *info -
*     Pointer to mapping configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_queue_to_flow_mapping_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_IN  SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_queue_to_flow_mapping_get
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Map a queues-quartet to system port and flow-quartet(s).
*     This function should only be called after calling the
*     soc_petra_ipq_queue_interdigitated_mode_set()
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  queue_quartet_ndx -
*     queue_quartet_ndx - The index of the quartet to be
*     configured, the range of queues in the quartet is:
*     (queue_quartet_ndx * 4) to (queue_quartet_ndx * 4 + 3)
*     SOC_SAND_IN
*  SOC_SAND_OUT SOC_PETRA_IPQ_QUARTET_MAP_INFO *info -
*     Pointer to mapping configuration structure.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_queue_to_flow_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx,
    SOC_SAND_OUT SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  );

/*********************************************************************
* NAME:
*   soc_petra_ipq_queue_qrtt_unmap
* TYPE:
*   PROC
* FUNCTION:
*   Unmap a queues-quartet, by mapping it to invalid
*   destination. Also, flush all the queues in the quartet.
* INPUT:
*   SOC_SAND_IN  int unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32  queue_quartet_ndx -
*     The index of the queue quartet, the range of queues in
*     the quartet is: (queue_quartet_ndx * 4) to
*     (queue_quartet_ndx * 4 + 3) Range: 0 - 8K-1.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_queue_qrtt_unmap(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32  queue_quartet_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_ipq_quartet_reset
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Resets a quartet to default values.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  queue_quartet_ndx -
*     queue_quartet_ndx - The 4 queues currently configured
*     are: (queue_quartet_ndx * 4) - (queue_quartet_ndx * 4 +
*     3)
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ipq_quartet_reset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_quartet_ndx
  );


void
  soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );

void
  soc_petra_PETRA_IPQ_QUARTET_MAP_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  );

uint32
  soc_petra_ipq_attached_flow_port_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32 queue_ndx,
    SOC_SAND_OUT uint32 *flow_id,
    SOC_SAND_OUT uint32 *sys_port
  );


#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_IPQ_TR_CLS_RNG_to_string(
    SOC_SAND_IN SOC_PETRA_IPQ_TR_CLS_RNG enum_val
  );


void
  soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_print(
    SOC_SAND_IN SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO *info
  );



void
  soc_petra_PETRA_IPQ_QUARTET_MAP_INFO_print(
    SOC_SAND_IN SOC_PETRA_IPQ_QUARTET_MAP_INFO *info
  );

uint32
  soc_petra_ips_non_empty_queues_info_get(
   SOC_SAND_IN  int                unit,
   SOC_SAND_IN  int                   core,
   SOC_SAND_IN  uint32                first_queue,
   SOC_SAND_IN  uint32                max_array_size,
   SOC_SAND_OUT soc_ips_queue_info_t* queues,
   SOC_SAND_OUT uint32*               nof_queues_filled,
   SOC_SAND_OUT uint32*               next_queue,
   SOC_SAND_OUT uint32*               reached_end
  );



#endif /* SOC_PETRA_DEBUG_IS_LVL1 */


/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_INGRESS_PACKET_QUEUING_INCLUDED__*/
#endif
