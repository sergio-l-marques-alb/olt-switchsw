/* 
 * $Id: oam_pm_pack.c,v 1.13 Broadcom SDK $
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
 * File:        oam_pm.c
 * Purpose:     OAM_PM pack and unpack routines for:
 *              - OAM_PM Control messages
 *              - Network Packet headers (PDUs)
 *
 *
 * OAM_PM control messages
 *
 * OAM_PM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The OAM_PM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/oam_pm_pack.h>


uint8 *
shr_oam_pm_msg_ctrl_pm_stats_get_pack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_pm_stats_get_t *msg)
{

    int i;
    _SHR_PACK_U32(buf,msg->flags);
    _SHR_PACK_U32(buf,msg->sess_id);
    _SHR_PACK_U32(buf,msg->far_loss_min);
    _SHR_PACK_U32(buf,msg->far_tx_min);
    _SHR_PACK_U32(buf,msg->far_loss_max);
    _SHR_PACK_U32(buf,msg->far_tx_max);
    _SHR_PACK_U32(buf,msg->far_loss);
    _SHR_PACK_U32(buf,msg->near_loss_min);
    _SHR_PACK_U32(buf,msg->near_tx_min);
    _SHR_PACK_U32(buf,msg->near_loss_max);
    _SHR_PACK_U32(buf,msg->near_tx_max);
    _SHR_PACK_U32(buf,msg->near_loss);
    _SHR_PACK_U32(buf,msg->lm_tx_count);
    _SHR_PACK_U32(buf,msg->DM_min);
    _SHR_PACK_U32(buf,msg->DM_max);
    _SHR_PACK_U32(buf,msg->DM_avg);
    _SHR_PACK_U32(buf,msg->dm_tx_count);
    _SHR_PACK_U8(buf,msg->profile_id);
    for (i = 0; i < (SHR_OAM_PM_MAX_PM_BIN_EDGES+1); i++) {
        _SHR_PACK_U32(buf, msg->bin_counters[i]);
    }

    return buf;
}

uint8 *
shr_oam_pm_msg_ctrl_pm_stats_get_unpack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_pm_stats_get_t *msg)
{
    int i;
    _SHR_UNPACK_U32(buf,msg->flags);
    _SHR_UNPACK_U32(buf,msg->sess_id);
    _SHR_UNPACK_U32(buf,msg->far_loss_min);
    _SHR_UNPACK_U32(buf,msg->far_tx_min);
    _SHR_UNPACK_U32(buf,msg->far_loss_max);
    _SHR_UNPACK_U32(buf,msg->far_tx_max);
    _SHR_UNPACK_U32(buf,msg->far_loss);
    _SHR_UNPACK_U32(buf,msg->near_loss_min);
    _SHR_UNPACK_U32(buf,msg->near_tx_min);
    _SHR_UNPACK_U32(buf,msg->near_loss_max);
    _SHR_UNPACK_U32(buf,msg->near_tx_max);
    _SHR_UNPACK_U32(buf,msg->near_loss);
    _SHR_UNPACK_U32(buf,msg->lm_tx_count);
    _SHR_UNPACK_U32(buf,msg->DM_min);
    _SHR_UNPACK_U32(buf,msg->DM_max);
    _SHR_UNPACK_U32(buf,msg->DM_avg);
    _SHR_UNPACK_U32(buf,msg->dm_tx_count);
    _SHR_UNPACK_U8(buf,msg->profile_id);
    for (i = 0; i < (SHR_OAM_PM_MAX_PM_BIN_EDGES+1); i++) {
        _SHR_UNPACK_U32(buf, msg->bin_counters[i]);
    }
    
    return buf;
}

uint8 *
shr_oam_pm_msg_ctrl_profile_attach_pack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_profile_attach_t *msg)
{

    int i;
    _SHR_PACK_U32(buf,msg->sess_id);
    _SHR_PACK_U32(buf,msg->profile_flags);
    _SHR_PACK_U8(buf,msg->profile_id);
    for (i = 0; i < SHR_OAM_PM_MAX_PM_BIN_EDGES; i++) {
        _SHR_PACK_U32(buf, msg->profile_edges[i]);
    }

    return buf;
}

uint8 *
shr_oam_pm_msg_ctrl_profile_attach_unpack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_profile_attach_t *msg)
{
    int i;
    _SHR_UNPACK_U32(buf,msg->sess_id);
    _SHR_UNPACK_U32(buf,msg->profile_flags);
    _SHR_UNPACK_U8(buf,msg->profile_id);
    for (i = 0; i < SHR_OAM_PM_MAX_PM_BIN_EDGES; i++) {
        _SHR_UNPACK_U32(buf, msg->profile_edges[i]);
    }
    
    return buf;
}

uint8 *
shr_oam_pm_msg_ctrl_raw_buffer_unpack(uint8 *buf,
                                 shr_oam_pm_msg_ctrl_raw_buffer_t *msg)
{
    int i, j;
    _SHR_UNPACK_U32(buf, msg->seq_num);
    for (i = 0; i < SHR_OAM_MAX_PM_ENDPOINTS; i++) {

        _SHR_UNPACK_U32(buf, msg->raw_sample[i].pm_id);
        _SHR_UNPACK_U32(buf, msg->raw_sample[i].lm_count);
        _SHR_UNPACK_U32(buf, msg->raw_sample[i].dm_count);
        for (j = 0; j < SHR_OAM_PM_RAW_DATA_MAX_SAMPLES; j++) {
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_counter[j].tx_fcf);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_counter[j].rx_fcf);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_counter[j].tx_fcb);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_counter[j].rx_fcb);
        }
        for (j = 0; j < SHR_OAM_PM_RAW_DATA_MAX_SAMPLES; j++) {
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].tx_tsf_secs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].tx_tsf_nsecs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].rx_tsf_secs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].rx_tsf_nsecs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].tx_tsb_secs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].tx_tsb_nsecs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].rx_tsb_secs);
            _SHR_UNPACK_U32(buf, msg->raw_sample[i].raw_timestamp[j].rx_tsb_nsecs);
        }
    }

    return buf;
}
