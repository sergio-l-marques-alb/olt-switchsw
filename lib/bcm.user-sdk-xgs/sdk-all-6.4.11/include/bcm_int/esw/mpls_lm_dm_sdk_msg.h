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
 * File:    mpls_lm_dm_sdk_msg.h
 */

#ifndef MPLS_LM_DM_SDK_MSG_H_
#define MPLS_LM_DM_SDK_MSG_H_

#include <soc/shared/mpls_lm_dm_msg.h>

/*
 * MPLS_LM_DM Initialization control message
 */
typedef struct mpls_lm_dm_sdk_msg_ctrl_init_s {
    uint32  num_sessions;        /* Max number of MPLS_LM_DM sessions */
    uint32  rx_channel;          /* Local RX DMA channel (0..3) */
    uint32  flags;               /* init flags */
} mpls_lm_dm_sdk_msg_ctrl_init_t;


/*
 * MPLS_LM_DM control messages
 */
typedef union mpls_lm_dm_sdk_msg_ctrl_s {
    mpls_lm_dm_sdk_msg_ctrl_init_t          init;
    shr_mpls_lm_dm_msg_ctrl_loss_add_t      loss_add;
    shr_mpls_lm_dm_msg_ctrl_loss_delete_t   loss_del;
    shr_mpls_lm_dm_msg_ctrl_loss_get_t      loss_get;
    shr_mpls_lm_dm_msg_ctrl_loss_data_t     loss_data;
    shr_mpls_lm_dm_msg_ctrl_delay_add_t     delay_add;
    shr_mpls_lm_dm_msg_ctrl_delay_delete_t  delay_del;
    shr_mpls_lm_dm_msg_ctrl_delay_get_t     delay_get;
    shr_mpls_lm_dm_msg_ctrl_delay_data_t    delay_data;
} mpls_lm_dm_sdk_msg_ctrl_t;

#endif /* MPLS_LM_DM_SDK_MSG_H_ */
