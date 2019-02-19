/*
 * $Id:$
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
 */
/*
 *
 * File:        bfd_sdk_pack.c
 * Purpose:     BFD unpack routines for:
 *              - BFD Control messages
 *
 */
#if defined(INCLUDE_BFD)

#include <bcm_int/esw/bfd_sdk_pack.h>

uint8 *
bfd_sdk_msg_ctrl_init_pack(uint8 *buf, bfd_sdk_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->encap_size);
    _SHR_PACK_U32(buf, msg->num_auth_sha1_keys);
    _SHR_PACK_U32(buf, msg->num_auth_sp_keys);
    _SHR_PACK_U32(buf, msg->rx_channel);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_INIT_CONFIG_FLAGS)) {
        _SHR_PACK_U32(buf, msg->config_flags);
    }

    return buf;
}

uint8 *
bfd_sdk_msg_ctrl_stat_reply_unpack(uint8 *buf, bfd_sdk_msg_ctrl_stat_reply_t *msg,
                                   uint8 stat64bit)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->packets_in);
    _SHR_UNPACK_U32(buf, msg->packets_out);
    _SHR_UNPACK_U32(buf, msg->packets_drop);
    _SHR_UNPACK_U32(buf, msg->packets_auth_drop);
    if (stat64bit && BFD_UC_FEATURE_CHECK(BFD_FEATURE_64BIT_STAT)) {
        _SHR_UNPACK_U32(buf, msg->packets_in_hi);
        _SHR_UNPACK_U32(buf, msg->packets_out_hi);
        _SHR_UNPACK_U32(buf, msg->packets_drop_hi);
        _SHR_UNPACK_U32(buf, msg->packets_auth_drop_hi);
    }
    return buf;
}


uint8 *
bfd_sdk_msg_ctrl_stat_req_pack(uint8 *buf,
                               bfd_sdk_msg_ctrl_stat_req_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->options);

    return buf;
}

uint8 *
bfd_sdk_version_exchange_msg_unpack(uint8 *buf, bfd_sdk_version_exchange_msg_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bfd_sdk_version_exchange_msg_pack(uint8 *buf, bfd_sdk_version_exchange_msg_t *msg)
{
    _SHR_PACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bfd_sdk_msg_ctrl_sess_set_pack(uint8 *buf,
                               bfd_sdk_msg_ctrl_sess_set_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U8(buf, msg->passive);
    _SHR_PACK_U8(buf, msg->local_demand);
    _SHR_PACK_U8(buf, msg->local_diag);
    _SHR_PACK_U8(buf, msg->local_detect_mult);
    _SHR_PACK_U32(buf, msg->local_discriminator);
    _SHR_PACK_U32(buf, msg->remote_discriminator);
    _SHR_PACK_U32(buf, msg->local_min_tx);
    _SHR_PACK_U32(buf, msg->local_min_rx);
    _SHR_PACK_U32(buf, msg->local_min_echo_rx);
    _SHR_PACK_U8(buf, msg->auth_type);
    _SHR_PACK_U32(buf, msg->auth_key);
    _SHR_PACK_U32(buf, msg->xmt_auth_seq);
    _SHR_PACK_U8(buf, msg->encap_type);
    _SHR_PACK_U32(buf, msg->encap_length);
    for (i = 0; i < SHR_BFD_MAX_ENCAP_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->encap_data[i]);
    }
    _SHR_PACK_U16(buf, msg->lkey_etype);
    _SHR_PACK_U16(buf, msg->lkey_offset);
    _SHR_PACK_U16(buf, msg->lkey_length);
    _SHR_PACK_U32(buf, msg->mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->mep_id[i]);
    }
    for (i = 0; i < SHR_BFD_MPLS_LABEL_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->mpls_label[i]);
    }
    _SHR_PACK_U32(buf, msg->tx_port);
    _SHR_PACK_U32(buf, msg->tx_cos);
    _SHR_PACK_U32(buf, msg->tx_pri);
    _SHR_PACK_U32(buf, msg->tx_qnum);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_ECHO_MODE)) {
        _SHR_PACK_U8(buf, msg->local_echo);
    }
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_REMOTE_MEP_ID)) {
        _SHR_PACK_U32(buf, msg->remote_mep_id_length);
        for (i = 0; i < msg->remote_mep_id_length; i++) {
             _SHR_PACK_U8(buf, msg->remote_mep_id[i]);
        }
    }
    if(BFD_UC_FEATURE_CHECK(BFD_FEATURE_RX_PKT_VLAN_ID)) {
        _SHR_PACK_U16(buf, msg->rx_pkt_vlan_id);
    }
    return buf;
}

uint8 *
bfd_sdk_msg_ctrl_sess_get_unpack(uint8 *buf,
                                 bfd_sdk_msg_ctrl_sess_get_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U8(buf, msg->enable);
    _SHR_UNPACK_U8(buf, msg->passive);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_POLL_BIT)) {
        _SHR_UNPACK_U8(buf, msg->poll);
    }
    _SHR_UNPACK_U8(buf, msg->local_demand);
    _SHR_UNPACK_U8(buf, msg->remote_demand);
    _SHR_UNPACK_U8(buf, msg->local_diag);
    _SHR_UNPACK_U8(buf, msg->remote_diag);
    _SHR_UNPACK_U8(buf, msg->local_sess_state);
    _SHR_UNPACK_U8(buf, msg->remote_sess_state);
    _SHR_UNPACK_U8(buf, msg->local_detect_mult);
    _SHR_UNPACK_U8(buf, msg->remote_detect_mult);
    _SHR_UNPACK_U32(buf, msg->local_discriminator);
    _SHR_UNPACK_U32(buf, msg->remote_discriminator);
    _SHR_UNPACK_U32(buf, msg->local_min_tx);
    _SHR_UNPACK_U32(buf, msg->remote_min_tx);
    _SHR_UNPACK_U32(buf, msg->local_min_rx);
    _SHR_UNPACK_U32(buf, msg->remote_min_rx);
    _SHR_UNPACK_U32(buf, msg->local_min_echo_rx);
    _SHR_UNPACK_U32(buf, msg->remote_min_echo_rx);
    _SHR_UNPACK_U8(buf, msg->auth_type);
    _SHR_UNPACK_U32(buf, msg->auth_key);
    _SHR_UNPACK_U32(buf, msg->xmt_auth_seq);
    _SHR_UNPACK_U32(buf, msg->rcv_auth_seq);
    _SHR_UNPACK_U8(buf, msg->encap_type);
    _SHR_UNPACK_U32(buf, msg->encap_length);
    for (i = 0; i < SHR_BFD_MAX_ENCAP_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->encap_data[i]);
    }
    _SHR_UNPACK_U16(buf, msg->lkey_etype);
    _SHR_UNPACK_U16(buf, msg->lkey_offset);
    _SHR_UNPACK_U16(buf, msg->lkey_length);
    _SHR_UNPACK_U32(buf, msg->mep_id_length);
    for (i = 0; i < _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->mep_id[i]);
    }
    for (i = 0; i < SHR_BFD_MPLS_LABEL_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->mpls_label[i]);
    }
    _SHR_UNPACK_U32(buf, msg->tx_port);
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U32(buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_ECHO_MODE)) {
        _SHR_UNPACK_U8(buf, msg->local_echo);
    }
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_REMOTE_MEP_ID)) {
        _SHR_UNPACK_U32(buf, msg->remote_mep_id_length);
        for (i = 0; i < msg->remote_mep_id_length; i++) {
             _SHR_UNPACK_U8(buf, msg->remote_mep_id[i]);
        }
        _SHR_UNPACK_U32(buf, msg->mis_conn_mep_id_length);
        for (i = 0; i < msg->mis_conn_mep_id_length; i++) {
             _SHR_UNPACK_U8(buf, msg->mis_conn_mep_id[i]);
        }
    }

    if(BFD_UC_FEATURE_CHECK(BFD_FEATURE_RX_PKT_VLAN_ID)) {
        _SHR_UNPACK_U16(buf, msg->rx_pkt_vlan_id);
    }

    return buf;
}
#else /* INCLUDE_BFD */
int bcm_xgs5_bfd_not_empty;
#endif  /* INCLUDE_BFD */
