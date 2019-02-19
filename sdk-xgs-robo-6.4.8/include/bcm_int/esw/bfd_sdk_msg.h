/*
 * $Id$
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
 * *
 * File:    bfd_sdk_msg.h
 */

#ifndef BFD_SDK_MSG_H_
#define BFD_SDK_MSG_H_

/*
 * BFD Statistics control messages (Request/Reply)
 *
 */

typedef struct bfd_sdk_msg_ctrl_stat_reply_s {
    uint32  sess_id;           /* BFD session (endpoint) id */
    uint32  packets_in;        /* Total packets in */
    uint32  packets_out;       /* Total packets out */
    uint32  packets_drop;      /* Total packets drop */
    uint32  packets_auth_drop; /* Packets drop due to authentication failure */

    /* 64bit statistics */
    uint32  packets_in_hi;
    uint32  packets_out_hi;
    uint32  packets_drop_hi;
    uint32  packets_auth_drop_hi;
} bfd_sdk_msg_ctrl_stat_reply_t;

typedef struct bfd_sdk_msg_ctrl_stat_req_s {
    uint32  sess_id;    /* BFD session (endpoint) id */
    uint32  options;    /* clear / 64bit get options */
} bfd_sdk_msg_ctrl_stat_req_t;

typedef struct bfd_sdk_version_exchange_msg_s {
    uint32  version; /* BFD SDK or appl version */
}bfd_sdk_version_exchange_msg_t;

/*
 *  BFD control messages
 */
typedef union bfd_sdk_msg_ctrl_s {
    shr_bfd_msg_ctrl_init_t         init;
    shr_bfd_msg_ctrl_sess_set_t     sess_set;
    shr_bfd_msg_ctrl_sess_get_t     sess_get;
    shr_bfd_msg_ctrl_auth_sp_t      auth_sp;
    shr_bfd_msg_ctrl_auth_sha1_t    auth_sha1;
    bfd_sdk_msg_ctrl_stat_req_t     stat_req;
    bfd_sdk_msg_ctrl_stat_reply_t   stat_reply;
    bfd_sdk_version_exchange_msg_t  msg;
} bfd_sdk_msg_ctrl_t;

#endif /* BFD_SDK_MSG_H_ */

