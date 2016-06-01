/*
* $Id: cosq.h,v 1.7 Broadcom SDK $
* $Copyright: Copyright 2016 Broadcom Corporation.
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
*
* This file contains structures and functions declarations for 
* In-band cell configuration and Source Routed Cell.
* 
*/
#ifndef _SOC_DPP_COSQ_H
#define _SOC_DPP_COSQ_H

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/TMC/tmc_api_flow_control.h>

typedef struct soc_ips_queue_info_s {
    uint32 queue_id;
    uint32 queue_byte_size;
    uint32 target_flow_id;
    uint32 got_flow_info;
    uint32 target_fap_id;
    uint32 target_data_port_id;
} soc_ips_queue_info_t;

typedef struct soc_hr2ps_info_s {
    uint32              mode;
    uint32              weight;
    uint32              kbits_sec_max;
    uint32              max_burst;
    SOC_TMC_SCH_SE_INFO se_info;
    SOC_TMC_SCH_FLOW    flow_info;
} soc_hr2ps_info_t;

/* flow control / addmission settings */
typedef struct soc_cosq_threshold_s {
    int dp;                         /* drop precedence. relevant for
                                       BCM_COSQ_THRESHOLD_PER_DP flag value */
    int value;                      /* threshold value for drop/flow control */
    int tc;                         /* traffic class for which the thresholds
                                       are to be configured */
} soc_cosq_threshold_t;

typedef enum soc_dpp_cosq_gport_fabric_pipe_e {
    soc_dpp_cosq_gport_fabric_pipe_all = 0,
    soc_dpp_cosq_gport_fabric_pipe_ingress = 1,
    soc_dpp_cosq_gport_fabric_pipe_egress = 2
} soc_dpp_cosq_gport_fabric_pipe_t;

typedef enum soc_dpp_cosq_gport_egress_core_fifo_e {
    soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast = 0,
    soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast = 1,
    soc_dpp_cosq_gport_egress_core_fifo_fabric_tdm = 2,
    soc_dpp_cosq_gport_egress_core_fifo_local_ucast = 3,
    soc_dpp_cosq_gport_egress_core_fifo_local_mcast = 4,
    soc_dpp_cosq_gport_egress_core_fifo_local_tdm = 5,
    soc_dpp_cosq_gport_egress_core_fifo_fabric_generic_pipe = 6
} soc_dpp_cosq_gport_egress_core_fifo_t;

typedef enum soc_dpp_cosq_threshold_type_e {
    soc_dpp_cosq_threshold_bytes = _SHR_COSQ_THRESHOLD_BYTES,
    soc_dpp_cosq_threshold_packet_descriptors = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS,
    soc_dpp_cosq_threshold_packets = _SHR_COSQ_THRESHOLD_PACKETS,
    soc_dpp_cosq_threshold_data_buffers = _SHR_COSQ_THRESHOLD_DATA_BUFFERS,
    soc_dpp_cosq_threshold_available_packet_descriptors = _SHR_COSQ_THRESHOLD_AVAILABLE_PACKET_DESCRIPTORS,
    soc_dpp_cosq_threshold_available_data_buffers = _SHR_COSQ_THRESHOLD_AVAILABLE_DATA_BUFFERS,
    soc_dpp_cosq_threshold_buffer_descriptor_buffers = _SHR_COSQ_THRESHOLD_BUFFER_DESCRIPTOR_BUFFERS,
    soc_dpp_cosq_threshold_buffer_descriptors = _SHR_COSQ_THRESHOLD_BUFFER_DESCRIPTORS,
    soc_dpp_cosq_threshold_dbuffs = _SHR_COSQ_THRESHOLD_DBUFFS,
    soc_dpp_cosq_threshold_full_dbuffs = _SHR_COSQ_THRESHOLD_FULL_DBUFFS,
    soc_dpp_cosq_threshold_mini_dbuffs = _SHR_COSQ_THRESHOLD_MINI_DBUFFS,
    soc_dpp_cosq_threshold_dynamic_weight = _SHR_COSQ_THRESHOLD_DYNAMIC_WEIGHT,
    soc_dpp_cosq_threshold_packet_descriptors_min = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_MIN,
    soc_dpp_cosq_threshold_packet_descriptors_max = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_MAX,
    soc_dpp_cosq_threshold_packet_descriptors_alpha = _SHR_COSQ_THRESHOLD_PACKET_DESCRIPTORS_ALPHA,
    soc_dpp_cosq_threshold_data_buffers_min = _SHR_COSQ_THRESHOLD_DATA_BUFFERS_MIN,
    soc_dpp_cosq_threshold_data_buffers_max = _SHR_COSQ_THRESHOLD_DATA_BUFFERS_MAX,
    soc_dpp_cosq_thereshold_data_buffers_alpha = _SHR_COSQ_THERESHOLD_DATA_BUFFERS_ALPHA,
    soc_dpp_cosq_threshold_num = _SHR_COSQ_THRESHOLD_NUM
} soc_dpp_cosq_threshold_type_t;

typedef enum soc_dpp_cosq_threshold_global_type_e {
    soc_dpp_cosq_threshold_global_type_unicast,
    soc_dpp_cosq_threshold_global_type_multicast,
    soc_dpp_cosq_threshold_global_type_total,
    soc_dpp_cosq_threshold_global_type_service_pool_0,
    soc_dpp_cosq_threshold_global_type_service_pool_1 /* must be after pool_0 */
} soc_dpp_cosq_threshold_global_type_t ;

/* soc_color_t */
typedef enum soc_color_e {
        socColorGreen = _SHR_COLOR_GREEN,
        socColorYellow = _SHR_COLOR_YELLOW,
        socColorRed = _SHR_COLOR_RED,
        socColorDropFirst = bcmColorRed,
        socColorBlack = _SHR_COLOR_BLACK,
        socColorPreserve = _SHR_COLOR_PRESERVE,
        socColorCount = _SHR_COLOR_COUNT
} soc_color_t;

/* cosq ingress port drop priority maps */
typedef enum soc_dpp_prd_map_e {
    socDppPrdTmTcDpPriorityTable = 0,
    socDppPrdIpDscpToPriorityTable = 1,
    socDppPrdEthPcpDeiToPriorityTable = 2,
    socDppPrdMplsExpToPriorityTable = 3
} soc_dpp_prd_map_t;


soc_error_t
  soc_dpp_cosq_flow_and_up_info_get(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     int                          core,
    SOC_SAND_IN     uint8                           is_flow,
    SOC_SAND_IN     uint32                          dest_id, /* Destination port or flow ndx */
    SOC_SAND_IN     uint32                          reterive_status,
    SOC_SAND_INOUT  SOC_TMC_SCH_FLOW_AND_UP_INFO    *flow_and_up_info
  );

soc_error_t
  soc_dpp_cosq_hr2ps_info_get(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   int                                core,
    SOC_SAND_IN   uint32                             se_id,
    SOC_SAND_OUT  soc_hr2ps_info_t                   *hr2ps_info
  );

soc_error_t
  soc_dpp_cosq_non_empty_queues_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                queue_to_read_from,
    SOC_SAND_IN  uint32                max_array_size,
    SOC_SAND_OUT soc_ips_queue_info_t* queues,
    SOC_SAND_OUT uint32*               nof_queues_filled,
    SOC_SAND_OUT uint32*               next_queue,
    SOC_SAND_OUT uint32*               reached_end
  );

soc_error_t
  soc_dpp_cosq_ingress_queue_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_INFO      *info    
  );

soc_error_t
  soc_dpp_cosq_ingress_queue_category_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT int                  *voq_category
  );

soc_error_t
  soc_dpp_cosq_ingress_queue_to_flow_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_IPQ_QUARTET_MAP_INFO          *queue_map_info
  );

soc_error_t
  soc_dpp_cosq_ingress_alpha_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT int32                  *alpha 
  );

soc_error_t
  soc_dpp_cosq_ingress_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT  SOC_TMC_ITM_ADMIT_TSTS     *test_tmplt
  );

soc_error_t
  soc_dpp_cosq_ingress_queue_dyn_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_DYN_INFO      *info    
  );
  
soc_error_t
  soc_dpp_cosq_vsq_index_global_to_group_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsq_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_GROUP      *soc_vsq_group_type,
    SOC_SAND_OUT uint32                        *vsq_index,
    SOC_SAND_OUT uint8                         *is_ocb_only
  );

int soc_dpp_voq_max_size_drop(int unit, uint32 *is_max_size);

/*
 * This function receives a FAP ID and returnes wethrer the FAP ID is local
 * (of a core of the local device) and if so, returns the core ID.
 * For single core devices or devices in single core mode, the core ID is always 0.
 */
soc_error_t soc_dpp_is_fap_id_local_and_get_core_id(
    SOC_SAND_IN   int      unit,
    SOC_SAND_IN   uint32   fap_id,    /* input FAP ID */
    SOC_SAND_OUT  uint8    *is_local, /* returns TRUE/FASLE based on if fap_id is of a local core */
    SOC_SAND_OUT  int      *core_id   /* if is_local returns TRUE, will contain the core ID of the FAP ID */
);

soc_error_t
  soc_dpp_fc_status_info_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN SOC_TMC_FC_STATUS_KEY   *fc_status_key,
    SOC_SAND_OUT SOC_TMC_FC_STATUS_INFO *fc_status_info 
  );

/*
 * Convert gport describing a destination to to TM dest information in SOC_TMC_DEST_INFO.
 * Identifies destination type and ID.
 */
soc_error_t 
dpp_gport_to_tm_dest_info( /* This code was originally in _bcm_dpp_gport_to_tm_dest_info() */
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  soc_gport_t       dest,      /* input destination gport */
    SOC_SAND_OUT SOC_TMC_DEST_INFO *dest_info /* output destination structure */
);
#endif
