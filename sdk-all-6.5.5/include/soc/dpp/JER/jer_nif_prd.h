/*
 * $Id: $
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
 * File: jer_nif_prd.h
 * 
 */

#ifndef __JER_NIF_PRD_INCLUDED__

#define __JER_NIF_PRD_INCLUDED__

#include <soc/portmod/portmod.h>
#include <soc/dpp/JER/jer_stat.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dcmn/dcmn_defs.h>

#define SOC_JER_NIF_PRD_MAX_KEY_BUILD_OFFSETS (4)

typedef struct soc_jer_nif_prd_config_s {
    int never_drop_control_frames; /* should be set only in VLAN mode */
    int seperate_tdm_and_non_tdm;
    int seperate_tdm_and_non_tdm_two_ports;
    int never_drop_tdm_packets;
} soc_jer_nif_prd_config_t;

typedef struct soc_jer_nif_prd_hard_stage_control_s {
    int itmh_type;                      /*0 = Arad based ITMH, 1 = Jericho based ITMH */
    int small_chunk_priority_override;  /* if set, chunks < 48B will get priority 0 (lowest)*/
} soc_jer_nif_prd_hard_stage_control_t;

typedef struct soc_jer_nif_prd_hard_stage_properties_s {
    int trust_ip_sdcp;
    int trust_mpls_exp;
    int trust_outer_eth_tag;
    int trust_inner_eth_tag;
    int outer_tag_size;
    int default_priority;
} soc_jer_nif_prd_hard_stage_properties_t;

typedef struct soc_jer_nif_prd_control_plane_s {
    uint64 mac_da_val;
    uint64 mac_da_mask;
    uint32 ether_type_code;
    uint32 ether_type_code_mask;
} soc_jer_nif_prd_control_plane_t;

typedef struct soc_jer_nif_prd_tcam_entry_s {
    uint64 key;
    uint64 mask;
    int priority;
    int valid;
} soc_jer_nif_prd_tcam_entry_t;

int soc_jer_plus_prd_hard_stage_enable_set(int unit, int port, uint32 en_mask);
int soc_jer_plus_prd_hard_stage_enable_get(int unit, int port, uint32 *en_mask);

int soc_jer_plus_prd_config_set(int unit, int port, soc_jer_nif_prd_config_t *prd_config);
int soc_jer_plus_prd_config_get(int unit, int port, soc_jer_nif_prd_config_t *prd_config);

int soc_jer_plus_prd_hard_stage_control_set(int unit, int port, soc_jer_nif_prd_hard_stage_control_t* hard_stage_ctrl);
int soc_jer_plus_prd_hard_stage_control_get(int unit, int port, soc_jer_nif_prd_hard_stage_control_t* hard_stage_ctrl);

int soc_jer_plus_prd_tpid_set(int unit, int port, int tpid_index, uint16 tpid_val); /**/
int soc_jer_plus_prd_tpid_get(int unit, int port, int tpid_index, uint16 *tpid_val); /**/

int soc_jer_plus_prd_hard_stage_port_type_set(int unit, int port, int port_type);
int soc_jer_plus_prd_hard_stage_port_type_get(int unit, int port, int *port_type);

int soc_jer_plus_prd_hard_stage_properties_set(int unit, int port, soc_jer_nif_prd_hard_stage_properties_t *properties);/**/
int soc_jer_plus_prd_hard_stage_properties_get(int unit, int port, soc_jer_nif_prd_hard_stage_properties_t *properties);/**/

int soc_jer_plus_prd_hard_ether_type_set(int unit, int port, uint32 ether_type_field, uint32 ether_type_val);
int soc_jer_plus_prd_hard_ether_type_get(int unit, int port, uint32 ether_type_field, uint32 *ether_type_val);

int soc_jer_plus_prd_control_plane_set(int unit, int port, soc_jer_nif_prd_control_plane_t *control_frame_conf);
int soc_jer_plus_prd_control_plane_get(int unit, int port, soc_jer_nif_prd_control_plane_t *control_frame_conf);

int soc_jer_plus_prd_map_tm_tc_dp_set(int unit, int port, soc_reg_above_64_val_t map_value);
int soc_jer_plus_prd_map_tm_tc_dp_get(int unit, int port, soc_reg_above_64_val_t map_value);

int soc_jer_plus_prd_map_ip_dscp_set(int unit, int port, soc_reg_above_64_val_t map_value);
int soc_jer_plus_prd_map_ip_dscp_get(int unit, int port, soc_reg_above_64_val_t map_value);

int soc_jer_plus_prd_map_eth_pcp_dei_set(int unit, int port, uint32 map_value);
int soc_jer_plus_prd_map_eth_pcp_dei_get(int unit, int port, uint32 *map_value);

int soc_jer_plus_prd_map_mpls_exp_set(int unit, int port, uint32 map_value);
int soc_jer_plus_prd_map_mpls_exp_get(int unit, int port, uint32 *map_value);

int soc_jer_plus_soft_stage_enable_set(int unit, int port, int en_eth, int en_tm); 
int soc_jer_plus_soft_stage_enable_get(int unit, int port, int *en_eth, int *en_tm); 

int soc_jer_plus_soft_stage_key_construct_set(int unit, int port, int *key_build_offset_array); /*can assume array size is 4*/
int soc_jer_plus_soft_stage_key_construct_get(int unit, int port, int *key_build_offset_array); 

int soc_jer_plus_soft_stage_key_entry_set(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry); 
int soc_jer_plus_soft_stage_key_entry_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry);

#endif /*__JER_NIF_PRD_INCLUDED__*/

