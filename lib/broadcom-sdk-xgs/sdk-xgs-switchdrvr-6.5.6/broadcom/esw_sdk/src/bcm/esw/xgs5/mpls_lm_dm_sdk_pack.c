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
 *
 * File:    mpls_lm_dm_sdk_pack.c
 */
#if defined(INCLUDE_MPLS_LM_DM)
#include <bcm/types.h>
#include <shared/pack.h>
#include <bcm_int/esw/mpls_lm_dm_feature.h>
#include <bcm_int/esw/mpls_lm_dm_sdk_msg.h>

extern uint32 mpls_lm_dm_firmware_version;

uint8 *
mpls_lm_dm_sdk_msg_ctrl_init_pack(uint8 *buf, mpls_lm_dm_sdk_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    if (mpls_lm_dm_firmware_version > MPLS_LM_DM_UC_MIN_VERSION) {
        _SHR_PACK_U32(buf, msg->flags);
    }
    return buf;
}

uint8 *
mpls_lm_dm_sdk_msg_ctrl_loss_add_pack(uint8 *buf,
                                      mpls_lm_dm_sdk_msg_ctrl_loss_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);

    _SHR_PACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_PACK_U8 (buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<MPLS_LM_DM_OLP_HDR_LEN; i++) {
        _SHR_PACK_U8 (buf, msg->olp_encap_data[i]);
    }

    _SHR_PACK_U8 (buf, msg->ctr_size);
    for (i=0; i<msg->ctr_size; i++) {
        _SHR_PACK_U32(buf, msg->ctr_base_id[i]);
        _SHR_PACK_U8 (buf, msg->ctr_offset[i]);
        _SHR_PACK_U8 (buf, msg->ctr_action[i]);
    }
    if (msg->flags & SHR_MPLS_LM_DM_BYTE_COUNT_FLAG) {
        for (i=0; i<msg->ctr_size; i++) {
            _SHR_PACK_U8 (buf, msg->ctr_byte_offset[i]);
        }
    }

    if (MPLS_LM_DM_UC_FEATURE_CHECK(MPLS_LM_DM_FLEX_CTR_SESS_ID)) {
        _SHR_PACK_U32(buf, msg->session_id);
        _SHR_PACK_U8(buf, msg->session_num_entries);
    }

    return buf;
}

uint8 *
mpls_lm_dm_sdk_msg_ctrl_delay_add_pack(uint8 *buf,
                                       mpls_lm_dm_sdk_msg_ctrl_delay_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8(buf, msg->pkt_pri);
    _SHR_PACK_U8(buf, msg->dm_format);

    _SHR_PACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_PACK_U8(buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<MPLS_LM_DM_OLP_HDR_LEN; i++) {
        _SHR_PACK_U8(buf, msg->olp_encap_data[i]);
    }

    if (MPLS_LM_DM_UC_FEATURE_CHECK(MPLS_LM_DM_FLEX_CTR_SESS_ID)) {
        _SHR_PACK_U32(buf, msg->session_id);
        _SHR_PACK_U8(buf, msg->session_num_entries);
    }

    return buf;
}

uint8 *
mpls_lm_dm_sdk_msg_ctrl_delay_data_unpack(uint8 *buf,
        mpls_lm_dm_sdk_msg_ctrl_delay_data_t *msg)
{
    int i;
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->delay_seconds);
    _SHR_UNPACK_U32(buf, msg->delay_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->txf_seconds);
    _SHR_UNPACK_U32(buf, msg->txf_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rxf_seconds);
    _SHR_UNPACK_U32(buf, msg->rxf_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->txb_seconds);
    _SHR_UNPACK_U32(buf, msg->txb_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rxb_seconds);
    _SHR_UNPACK_U32(buf, msg->rxb_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->tx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);
    _SHR_UNPACK_U8 (buf, msg->dm_format);
    
    if (MPLS_LM_DM_UC_FEATURE_CHECK(MPLS_LM_DM_COUNTER_INFO_GET)) {
        _SHR_UNPACK_U32(buf, msg->olp_tx_hdr_length);

        for (i = 0; i < msg->olp_tx_hdr_length; i++) {
            _SHR_UNPACK_U8(buf, msg->olp_tx_hdr[i]);
        }
    }
    return buf;
}

#else /* INCLUDE_MPLS_LM_DM */
int bcm_xgs5_mpls_lm_dm_not_empty;
#endif  /* INCLUDE_MPLS_LM_DM */

