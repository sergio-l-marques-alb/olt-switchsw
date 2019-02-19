/*
* $Id: cosq.h,v 1.7 Broadcom SDK $
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
    soc_dpp_cosq_gport_egress_core_fifo_local_tdm = 5
} soc_dpp_cosq_gport_egress_core_fifo_t;

soc_error_t
  soc_dpp_cosq_flow_and_up_info_get(
    SOC_SAND_IN     int                          unit,
    SOC_SAND_IN     uint8                           is_flow,
    SOC_SAND_IN     uint32                          dest_id, /* Destination port or flow ndx */
    SOC_SAND_IN     uint32                          reterive_status,
    SOC_SAND_INOUT  SOC_TMC_SCH_FLOW_AND_UP_INFO    *flow_and_up_info
  );

soc_error_t
  soc_dpp_cosq_hr2ps_info_get(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   uint32                             se_id,
    SOC_SAND_OUT  soc_hr2ps_info_t                   *hr2ps_info
  );

soc_error_t
  soc_dpp_cosq_non_empty_queues_info_get(
    SOC_SAND_IN  int                unit,
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
    SOC_SAND_OUT uint32                     *vsq_index
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
#endif
