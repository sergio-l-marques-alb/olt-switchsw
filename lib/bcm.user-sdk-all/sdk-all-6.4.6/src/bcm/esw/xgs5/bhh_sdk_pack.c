/*
 * $Id:$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:        bhh_sdk_pack.c
 * Purpose:     BHH pack and unpack routines for:
 *              - BHH Control messages
 *
 */
#if defined(INCLUDE_BHH)

#include <bcm_int/esw/bhh_sdk_pack.h>
#include <shared/pack.h>

uint8 *
bhh_sdk_version_exchange_msg_unpack(uint8 *buf, bhh_sdk_version_exchange_msg_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bhh_sdk_version_exchange_msg_pack(uint8 *buf, bhh_sdk_version_exchange_msg_t *msg)
{
    _SHR_PACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_sess_get_unpack(uint8 *buf,
                                 bhh_sdk_msg_ctrl_sess_get_t *msg)
{ 
    int i;

    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U8(buf, msg->enable);
    _SHR_UNPACK_U8(buf, msg->passive);
    _SHR_UNPACK_U8(buf, msg->local_demand);
    _SHR_UNPACK_U8(buf, msg->remote_demand);
    _SHR_UNPACK_U8(buf, msg->local_sess_state);
    _SHR_UNPACK_U8(buf, msg->remote_sess_state);
    _SHR_UNPACK_U8(buf, msg->mel);
    _SHR_UNPACK_U16(buf, msg->mep_id);
    for (i = 0; i < SHR_BHH_MEG_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->meg_id[i]);
    }
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U8(buf, msg->encap_type);
    _SHR_UNPACK_U32(buf, msg->encap_length);
    for (i = 0; i < SHR_BHH_MAX_ENCAP_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->encap_data[i]);
    }
    _SHR_UNPACK_U32(buf, msg->tx_port);
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U32(buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
    if(BHH_UC_FEATURE_CHECK(BHH_WB_SESS_GET)) {
        _SHR_UNPACK_U32(buf, msg->mpls_label);
        _SHR_UNPACK_U32(buf, msg->if_num);
        _SHR_UNPACK_U32(buf, msg->flags);
    }
    if(BHH_UC_FEATURE_CHECK(BHH_REMOTE_EP_NAME)) {
        _SHR_UNPACK_U16(buf, msg->remote_mep_id);
    }
    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_loss_get_unpack(uint8 *buf,
                                 bhh_sdk_msg_ctrl_loss_get_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->loss_threshold);
    _SHR_UNPACK_U32(buf, msg->loss_nearend);
    _SHR_UNPACK_U32(buf, msg->tx_nearend);
    _SHR_UNPACK_U32(buf, msg->rx_nearend);
    _SHR_UNPACK_U32(buf, msg->tx_farend);
    _SHR_UNPACK_U32(buf, msg->rx_farend);
    _SHR_UNPACK_U32(buf, msg->rx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->tx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);

    if(BHH_UC_FEATURE_CHECK(BHH_LOSS_GET)) {
        _SHR_UNPACK_U32(buf, msg->loss_farend);
    }
    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_init_pack(uint8 *buf, bhh_sdk_msg_ctrl_init_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    _SHR_PACK_U32(buf, msg->node_id);
    for (i = 0;i < SHR_BHH_CARRIER_CODE_LEN;i++)
    _SHR_PACK_U8(buf, msg->carrier_code[i]);

    if(BHH_UC_FEATURE_CHECK(BHH_PM_MODE)) {
        _SHR_PACK_U32(buf, msg->data_collection_mode);
    }
    return buf;
}

#else /* INCLUDE_BHH */
int bcm_xgs5_bfd_not_empty;
#endif  /* INCLUDE_BHH */
