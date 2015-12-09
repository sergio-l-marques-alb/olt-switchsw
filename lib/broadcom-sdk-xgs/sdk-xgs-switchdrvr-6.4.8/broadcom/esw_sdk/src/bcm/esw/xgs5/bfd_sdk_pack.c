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
 * File:        bfd_sdk_pack.c
 * Purpose:     BFD unpack routines for:
 *              - BFD Control messages
 *
 */
#if defined(INCLUDE_BFD)

#include <bcm_int/esw/bfd_sdk_pack.h>

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

#else /* INCLUDE_BFD */
int bcm_xgs5_bfd_not_empty;
#endif  /* INCLUDE_BFD */
