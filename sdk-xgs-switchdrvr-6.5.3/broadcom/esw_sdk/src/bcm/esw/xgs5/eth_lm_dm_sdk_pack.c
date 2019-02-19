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
 * File:    eth_lm_dm_sdk_pack.c
 */

#if defined(INCLUDE_ETH_LM_DM)
#include <bcm_int/esw/eth_lm_dm_sdk_pack.h>

/* eth_lm_dm_sdk_ctrl_version_msg_pack
 * @brief
 * Pack the eth_lm_dm sdk version
 */
uint8 *
eth_lm_dm_sdk_version_exchange_msg_pack(uint8 *buf, eth_lm_dm_sdk_version_exchange_msg_t *msg)
{
    _SHR_PACK_U32(buf, msg->version);
    return buf;
}


/* eth_lm_dm_sdk_ctrl_version_msg_unpack
 * @brief
 * UnPack the eth_lm_dm SDK version
 */
uint8 *
eth_lm_dm_sdk_version_exchange_msg_unpack(uint8 *buf, eth_lm_dm_sdk_version_exchange_msg_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->version);
    return buf;
}

uint8 *
eth_lm_dm_sdk_msg_ctrl_init_pack(uint8 *buf, eth_lm_dm_sdk_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    if(ETH_LM_DM_UC_FEATURE_CHECK(ETH_LM_DM_PM)) {
        _SHR_PACK_U32(buf, msg->data_collection_mode);
    }
    return buf;
}
#else /* INCLUDE_ETH_LM_DM */
int bcm_xgs5_eth_lm_dm_not_empty;
#endif  /* INCLUDE_ETH_LM_DM */
