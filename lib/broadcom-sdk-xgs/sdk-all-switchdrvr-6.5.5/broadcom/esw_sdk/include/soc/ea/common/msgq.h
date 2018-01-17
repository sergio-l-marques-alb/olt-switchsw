/*
 * $Id: msgq.h,v 1.1 Broadcom SDK $
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
 * File:     msgq.h
 * Purpose:
 *
 */

#ifndef _SOC_EA_MSGQ_H
#define _SOC_EA_MSGQ_H

#include <sal/types.h>
#include <sal/core/sync.h>

#define SOC_EA_MSGQ_OK        0x00000000
#define SOC_EA_MSGQ_BAD_PARAM 0x80000001
#define SOC_EA_MSGQ_NO_MEM    0x80000002
#define SOC_EA_MSGQ_FULL      0x80000003
#define SOC_EA_MSGQ_EMPTY     0x80000004
#define SOC_EA_MSGQ_TIMEOUT   0x80000005
#define SOC_EA_MSGQ_FAULT     0x80000006

typedef struct soc_ea_msgq_node_s {
    uint8        *buf;
    int          len;
} soc_ea_msgq_node_t;

typedef struct soc_ea_msgq_s {
    uint8                   *base_ptr;
    struct soc_ea_msgq_s    *self;
    sal_sem_t               sem;
    int                     max_msgs;
    int                     max_msg_len;
    int                     head;
    int                     tail;
    int                     count;
    int                     ecode;
    soc_ea_msgq_node_t      *nodes;
} soc_ea_msgq_t;

extern soc_ea_msgq_t * soc_ea_msgq_create (char *name, int max_msg_len, int msg_num);
extern int soc_ea_msgq_destroy (soc_ea_msgq_t *msgq);
extern int soc_ea_msgq_send (soc_ea_msgq_t *msgq, char *msg, int len);
extern int soc_ea_msgq_recv (soc_ea_msgq_t *msgq, uint8 *buf, int max_bytes, int usec);
extern int soc_ea_msgq_count_get (soc_ea_msgq_t *msgq);
extern int soc_ea_msgq_error_get (soc_ea_msgq_t *msgq);
extern int soc_ea_msgq_clear (soc_ea_msgq_t *msgq);
extern void soc_ea_msgq_show (soc_ea_msgq_t *msgq);

#endif /* _SOC_EA_MSGQ_H */
