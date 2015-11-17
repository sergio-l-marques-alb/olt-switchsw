/* 
 * $Id: mpls_lm_dm_pack.c,v 1.13 Broadcom SDK $
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
 *
 * File:        mpls_lm_dm_pack.c
 * Purpose:     MPLS_LM_DM pack and unpack routines for:
 *              - MPLS_LM_DM Control messages
 *              - Network Packet headers (PDUs)
 *
 *
 * MPLS_LM_DM control messages
 *
 * MPLS_LM_DM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The MPLS_LM_DM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/mpls_lm_dm_msg.h>
#include <soc/shared/mpls_lm_dm_pkt.h>
#include <soc/shared/mpls_lm_dm_pack.h>


/***********************************************************
 * MPLS_LM_DM Control Message Pack/Unpack
 *
 * Functions:
 *      shr_mpls_lm_dm_msg_ctrl_<type>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      MPLS_LM_DM control message into/from a given buffer
 * Parameters:
 *   _pack()
 *      buffer  - (OUT) Buffer where to pack message.
 *      msg     - (IN)  MPLS_LM_DM control message to pack.
 *   _unpack()
 *      buffer  - (IN)  Buffer with message to unpack.
 *      msg     - (OUT) Returns MPLS_LM_DM control message.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_mpls_lm_dm_msg_ctrl_init_pack(uint8 *buf, shr_mpls_lm_dm_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_init_unpack(uint8 *buf, shr_mpls_lm_dm_msg_ctrl_init_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->num_sessions);
    _SHR_UNPACK_U32(buf, msg->rx_channel);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_stat_req_pack(uint8 *buf,
                               shr_mpls_lm_dm_msg_ctrl_stat_req_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->clear);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_stat_req_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_stat_req_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->clear);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_stat_reply_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_stat_reply_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->packets_in);
    _SHR_PACK_U32(buf, msg->packets_out);
    _SHR_PACK_U32(buf, msg->packets_drop);
    _SHR_PACK_U32(buf, msg->packets_auth_drop);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_stat_reply_unpack(uint8 *buf,
                                   shr_mpls_lm_dm_msg_ctrl_stat_reply_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->packets_in);
    _SHR_UNPACK_U32(buf, msg->packets_out);
    _SHR_UNPACK_U32(buf, msg->packets_drop);
    _SHR_UNPACK_U32(buf, msg->packets_auth_drop);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_add_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_add_t *msg)
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
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_add_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_add_t *msg)
{
    int i;
    
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);

    _SHR_UNPACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_UNPACK_U8 (buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<(MPLS_LM_DM_OLP_HDR_LEN); i++) {
        _SHR_UNPACK_U8 (buf, msg->olp_encap_data[i]);
    }

    _SHR_UNPACK_U8 (buf, msg->ctr_size);
    for (i=0; i<msg->ctr_size; i++) {
        _SHR_UNPACK_U32(buf, msg->ctr_base_id[i]);
        _SHR_UNPACK_U8 (buf, msg->ctr_offset[i]);
        _SHR_UNPACK_U8 (buf, msg->ctr_action[i]);
    }

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_delete_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_delete_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_delete_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_delete_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_get_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_get_t *msg)
{
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_get_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_get_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_data_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_data_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->loss_threshold);
    _SHR_PACK_U32(buf, msg->loss_nearend);
    _SHR_PACK_U32(buf, msg->loss_farend);
    _SHR_PACK_U32(buf, msg->tx_nearend);
    _SHR_PACK_U32(buf, msg->rx_nearend);
    _SHR_PACK_U32(buf, msg->tx_farend);
    _SHR_PACK_U32(buf, msg->rx_farend);
    _SHR_PACK_U32(buf, msg->rx_oam_packets);
    _SHR_PACK_U32(buf, msg->tx_oam_packets);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);

    _SHR_PACK_U8 (buf, msg->ctr_size);
    for (i=0; i<msg->ctr_size; i++) {
        _SHR_PACK_U32(buf, msg->ctr_base_id[i]);
        _SHR_PACK_U8 (buf, msg->ctr_offset[i]);
        _SHR_PACK_U8 (buf, msg->ctr_action[i]);
    }

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_loss_data_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_loss_data_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->loss_threshold);
    _SHR_UNPACK_U32(buf, msg->loss_nearend);
    _SHR_UNPACK_U32(buf, msg->loss_farend);
    _SHR_UNPACK_U32(buf, msg->tx_nearend);
    _SHR_UNPACK_U32(buf, msg->rx_nearend);
    _SHR_UNPACK_U32(buf, msg->tx_farend);
    _SHR_UNPACK_U32(buf, msg->rx_farend);
    _SHR_UNPACK_U32(buf, msg->rx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->tx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);

    _SHR_UNPACK_U8 (buf, msg->ctr_size);
    for (i=0; i<msg->ctr_size; i++) {
        _SHR_UNPACK_U32(buf, msg->ctr_base_id[i]);
        _SHR_UNPACK_U8 (buf, msg->ctr_offset[i]);
        _SHR_UNPACK_U8 (buf, msg->ctr_action[i]);
    }

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_add_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);
    _SHR_PACK_U8 (buf, msg->dm_format);

    _SHR_PACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_PACK_U8 (buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<MPLS_LM_DM_OLP_HDR_LEN; i++) {
        _SHR_PACK_U8 (buf, msg->olp_encap_data[i]);
    }
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_add_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_add_t *msg)
{
    int i;
    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);
    _SHR_UNPACK_U8 (buf, msg->dm_format);

    _SHR_UNPACK_U16(buf, msg->l2_encap_length);
    for (i=0; i<msg->l2_encap_length; i++) {
        _SHR_UNPACK_U8 (buf, msg->l2_encap_data[i]);
    }
    for (i=0; i<(MPLS_LM_DM_OLP_HDR_LEN); i++) {
        _SHR_UNPACK_U8 (buf, msg->olp_encap_data[i]);
    }
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_delete_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_delete_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_delete_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_delete_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->sess_id);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_get_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_get_t *msg)
{

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_get_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_get_t *msg)
{

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_data_pack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_data_t *msg)
{

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->delay_seconds);
    _SHR_PACK_U32(buf, msg->delay_nanoseconds);
    _SHR_PACK_U32(buf, msg->txf_seconds);
    _SHR_PACK_U32(buf, msg->txf_nanoseconds);
    _SHR_PACK_U32(buf, msg->rxf_seconds);
    _SHR_PACK_U32(buf, msg->rxf_nanoseconds);
    _SHR_PACK_U32(buf, msg->txb_seconds);
    _SHR_PACK_U32(buf, msg->txb_nanoseconds);
    _SHR_PACK_U32(buf, msg->rxb_seconds);
    _SHR_PACK_U32(buf, msg->rxb_nanoseconds);
    _SHR_PACK_U32(buf, msg->rx_oam_packets);
    _SHR_PACK_U32(buf, msg->tx_oam_packets);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);
    _SHR_PACK_U8 (buf, msg->dm_format);

    return buf;
}

uint8 *
shr_mpls_lm_dm_msg_ctrl_delay_data_unpack(uint8 *buf,
                                 shr_mpls_lm_dm_msg_ctrl_delay_data_t *msg)
{

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

    return buf;
}
