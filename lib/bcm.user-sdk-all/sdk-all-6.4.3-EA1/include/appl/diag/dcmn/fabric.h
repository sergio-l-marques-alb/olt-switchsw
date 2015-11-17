/*
 * $Id: fabric.h,v 1.11 Broadcom SDK $
 *
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
 * FE1600 DEFS H
 */
#ifndef _APPL_DIAG_DCMN_FABRIC_H_INCLUDED_
#define _APPL_DIAG_DCMN_FABRIC_H_INCLUDED_

#include <appl/diag/shell.h>

#include <appl/diag/dcmn/diag.h>

#include <bcm/fabric.h>

#define DIAG_DNX_FABRIC_THRESHOLDS_MAX 4

typedef struct diag_dnx_fabric_link_th_info_s {
    /*RX*/
    int th_types_rx_nof;
    bcm_fabric_link_threshold_type_t th_types_rx[DIAG_DNX_FABRIC_THRESHOLDS_MAX];
    uint32 values_rx[DIAG_DNX_FABRIC_THRESHOLDS_MAX];

    /*TX*/
    int th_types_tx_nof;
    bcm_fabric_link_threshold_type_t th_types_tx[DIAG_DNX_FABRIC_THRESHOLDS_MAX];
    uint32 values_tx[DIAG_DNX_FABRIC_THRESHOLDS_MAX];

    /*Middle stage*/
    int th_types_middle_nof;
    bcm_fabric_link_threshold_type_t th_types_middle[DIAG_DNX_FABRIC_THRESHOLDS_MAX];
    uint32 values_middle[DIAG_DNX_FABRIC_THRESHOLDS_MAX];
} diag_dnx_fabric_link_th_info_t;

typedef struct diag_dnx_fabric_link_config_s
{
    int enable;
    int speed;
    bcm_port_pcs_t pcs;
    int pcs_llfc_extract_cig;
    int pcs_error_detect;
    int pcs_llfc_low_latency;
    uint32 cl72;
    int pcp;
    bcm_fabric_link_remote_pipe_mapping_t pipe_mapping;
    uint32 rx_polarity;
    uint32 tx_polarity;
    uint32 lane_swap;
    int clk_freq;
} diag_dnx_fabric_link_config_t;

cmd_result_t diag_dnx_fabric_pack_usage(int unit, args_t *args);

cmd_result_t diag_dnx_fabric_reachability(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_connectivity(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_link(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_queues(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_thresholds(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_properties(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_link_config(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_mesh(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_traffic_profile(int unit, args_t *a);
cmd_result_t diag_dnx_fabric_mesh_topology(int unit, args_t *a);

void diag_dnx_fabric_reachability_usage(int unit);
void diag_dnx_fabric_connectivity_usage(int unit);
void diag_dnx_fabric_link_usage(int unit);
void diag_dnx_fabric_queues_usage(int unit);
void diag_dnx_fabric_thresholds_usage(int unit);
void diag_dnx_fabric_properties_usage(int unit);
void diag_dnx_fabric_link_config_usage(int unit);
void diag_dnx_fabric_mesh_usage(int unit);
void diag_dnx_fabric_traffic_profile_usage(int unit);
void diag_dnx_fabric_mesh_topology_usage(int unit);

#endif /*!_APPL_DIAG_DCMN_FABRIC_H_INCLUDED_*/


