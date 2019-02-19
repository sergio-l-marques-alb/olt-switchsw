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
 * File:    bfd_sdk_msg.h
 */

#ifndef BFD_SDK_MSG_H_
#define BFD_SDK_MSG_H_

#include <soc/shared/bfd_msg.h>

/*
 * BFD Session Get control message
 */
typedef struct bfd_sdk_msg_ctrl_sess_get_s {
    uint32   sess_id;
    uint8    enable;
    uint8    passive;
    uint8    poll;
    uint8    local_demand;
    uint8    remote_demand;
    uint8    local_diag;
    uint8    remote_diag;
    uint8    local_sess_state;
    uint8    remote_sess_state;
    uint8    local_detect_mult;
    uint8    remote_detect_mult;
    uint32   local_discriminator;
    uint32   remote_discriminator;
    uint32   local_min_tx;
    uint32   remote_min_tx;
    uint32   local_min_rx;
    uint32   remote_min_rx;
    uint32   local_min_echo_rx;
    uint32   remote_min_echo_rx;
    uint8    auth_type;
    uint32   auth_key;
    uint32   xmt_auth_seq;
    uint32   rcv_auth_seq;
    uint8    encap_type;    /* Raw, UDP-IPv4/IPv6, used for UDP checksum */
    uint32   encap_length;  /* BFD encapsulation length */
    uint8    encap_data[SHR_BFD_MAX_ENCAP_LENGTH];  /* Encapsulation data */
    uint16   lkey_etype;    /* Lookup key Ether Type */
    uint16   lkey_offset;   /* Lookup key offset */
    uint16   lkey_length;   /* Lookup key length */
    uint32   mep_id_length; /* MPLS-TP CV Source MEP-ID TLV length */
    uint8    mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                             Source MEP-ID */
    uint8    mpls_label[SHR_BFD_MPLS_LABEL_LENGTH]; /* Incoming inner MPLS
                                                       label packet format */
    uint32   tx_port;
    uint32   tx_cos;
    uint32   tx_pri;
    uint32   tx_qnum;
#ifdef SDK_45223_DISABLED
    uint32   remote_mep_id_length;
    uint8    remote_mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                                 Remote MEP-ID */
    uint32   mis_conn_mep_id_length;
    uint8    mis_conn_mep_id[_SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CV
                                                       Mis connectivity MEP-ID */
#endif
} bfd_sdk_msg_ctrl_sess_get_t;

#endif /* BFD_SDK_MSG_H_ */


