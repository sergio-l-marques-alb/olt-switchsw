/*
 * $Id$
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
 * *
 * File:    bhh_sdk_msg.h
 */

#ifndef BHH_SDK_MSG_H_
#define BHH_SDK_MSG_H_

/*
 * BHH Statistics control messages (Request/Reply)
 */

typedef struct bhh_sdk_version_exchange_msg_s {
    uint32  version; /* BHH SDK or appl version */
}bhh_sdk_version_exchange_msg_t;


typedef struct bhh_sdk_msg_ctrl_sess_get_s {
    uint32   sess_id;
    uint8    enable;
    uint8    passive;
    uint8    local_demand;
    uint8    remote_demand;
    uint8    local_sess_state;
    uint8    remote_sess_state;
    uint8    mel;
    uint16   mep_id;
    uint8    meg_id[SHR_BHH_MEG_ID_LENGTH];
    uint32   local_period;
    uint8    encap_type;
    uint32   encap_length;  /* BHH encapsulation length */
    uint8    encap_data[SHR_BHH_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint32   tx_port;
    uint32   tx_cos;
    uint32   tx_pri;
    uint32   tx_qnum;
    uint32   mpls_label;
    uint32   if_num;
    uint32   flags;
    uint16   remote_mep_id;
    uint32   fault_flags;
    uint32   remote_period;
} bhh_sdk_msg_ctrl_sess_get_t;

/*
 *  * BHH Loss Measurement get
 *   */
typedef struct bhh_sdk_msg_ctrl_loss_get_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  loss_threshold;
    uint32  loss_nearend;
    uint32  loss_farend;
    uint32  tx_nearend;
    uint32  rx_nearend;
    uint32  tx_farend;
    uint32  rx_farend;
    uint32  rx_oam_packets;
    uint32  tx_oam_packets;
    uint32  int_pri;
    uint8   pkt_pri;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_loss_get_t;

/*
 * BHH Initialization control message
 */
typedef struct bhh_sdk_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of BHH sessions */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
    uint32  node_id;             /* Node ident */
    uint8   carrier_code[SHR_BHH_CARRIER_CODE_LEN];
    uint32  data_collection_mode;/* LM/DM data collection mode */
    uint16  max_encap_length;      /* Max length of encap */
} bhh_sdk_msg_ctrl_init_t;


/*
 * BHH Session enable message
 */
typedef struct bhh_sdk_msg_ctrl_rmep_create_s {
    uint32  sess_id;
    uint32  flags;
    uint16  remote_mep_id;
    uint8   enable;
    uint32  period;
} bhh_sdk_msg_ctrl_rmep_create_t;

/*
 * BHH Loss Measurement add
 */
typedef struct bhh_sdk_msg_ctrl_loss_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_loss_add_t;

/*
 * BHH Delay Measurement add
 */
typedef struct bhh_sdk_msg_ctrl_delay_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  int_pri;
    uint8   pkt_pri;
    uint8   dm_format;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_delay_add_t;

/*
 * BHH Loopback add
 */
typedef struct bhh_sdk_msg_ctrl_loopback_add_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  ttl;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_loopback_add_t;

typedef struct bhh_sdk_msg_ctrl_sess_set_s {
    uint32  sess_id;
    uint32  flags;
    uint8   passive;
    uint8   mel;    /* MEG level */
    uint16  mep_id;
    uint8   meg_id[SHR_BHH_MEG_ID_LENGTH];
    uint32  period;
    uint8   encap_type;    /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    uint32  encap_length;  /* BHH encapsulation length */
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint32  tx_port;
    uint32  tx_cos;
    uint32  tx_pri;
    uint32  tx_qnum;
    uint32  mpls_label;
    uint32  if_num;
    uint32  lm_counter_index;
    uint16  inner_vlan;
    uint16  outer_vlan;
    uint8   endpoint_type;
    uint8   num_session_entries;
} bhh_sdk_msg_ctrl_sess_set_t;

/*
 * BHH Session Update
 */
typedef struct bhh_sdk_msg_session_update_s {
    uint32  flags;
    uint32  sess_id;
    uint8   msg_type;
    uint8   rdi;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_session_update_t;

/*
 * BHH Delay Measurement get
 */
typedef struct bhh_sdk_msg_ctrl_delay_get_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  delay_seconds;
    uint32  delay_nanoseconds;
    uint32  txf_seconds;
    uint32  txf_nanoseconds;
    uint32  rxf_seconds;
    uint32  rxf_nanoseconds;
    uint32  txb_seconds;
    uint32  txb_nanoseconds;
    uint32  rxb_seconds;
    uint32  rxb_nanoseconds;
    uint32  rx_oam_packets;
    uint32  tx_oam_packets;
    uint32  int_pri;
    uint8   pkt_pri;
    uint8   dm_format;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_delay_get_t;

/*
 * BHH Loopback get
 */
typedef struct bhh_sdk_msg_ctrl_loopback_get_s {
    uint32  flags;
    uint32  sess_id;    /* BHH session (endpoint) id */
    uint32  period;
    uint32  ttl;
    uint32  discovery_flags;
    uint32  discovery_id;
    uint32  discovery_ttl;
    uint32  rx_count;
    uint32  tx_count;
    uint32  drop_count;
    uint32  unexpected_response;
    uint32  out_of_sequence;
    uint32  local_mipid_missmatch;
    uint32  remote_mipid_missmatch;
    uint32  invalid_target_mep_tlv;
    uint32  invalid_mep_tlv_subtype;
    uint32  invalid_tlv_offset;
    uint16  encap_length;
    uint8   encap_data[SHR_BHH_MAX_ENCAP_LENGTH];
} bhh_sdk_msg_ctrl_loopback_get_t;

/*
 *  BHH control messages
 */
typedef union bhh_sdk_msg_ctrl_s {
    bhh_sdk_msg_ctrl_init_t         init;
    bhh_sdk_msg_ctrl_sess_set_t     sess_set;
    bhh_sdk_msg_ctrl_sess_get_t     sess_get;
    shr_bhh_msg_ctrl_sess_delete_t  sess_delete;
    shr_bhh_msg_ctrl_stat_req_t     stat_req;
    shr_bhh_msg_ctrl_stat_reply_t   stat_reply;
    bhh_sdk_msg_ctrl_loopback_add_t loopback_add;
    bhh_sdk_msg_ctrl_loopback_get_t loopback_get;
    shr_bhh_msg_ctrl_pm_stats_get_t pm_stats_get;
    bhh_sdk_version_exchange_msg_t  bhh_ver_msg;
    bhh_sdk_msg_ctrl_loss_add_t     loss_add;
    bhh_sdk_msg_ctrl_loss_get_t     loss_get;
    bhh_sdk_msg_ctrl_delay_add_t    delay_add;
    bhh_sdk_msg_ctrl_delay_get_t    delay_get;
} bhh_sdk_msg_ctrl_t;

#endif /* BHH_SDK_MSG_H_ */

