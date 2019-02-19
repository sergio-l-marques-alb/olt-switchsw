/* 
 * $Id: ulink_pack.h,v 1.0 Broadcom SDK $
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
 * File:        ulink_pack.h
 * Purpose:     Interface to pack and unpack routines common to
 *              SDK and uKernel for:
 *              - ULINK control message 
 *
 *              This is to be shared between SDK host and uKernel.
 */


#ifndef   _SOC_SHARED_ULINK_PACK_H_
#define   _SOC_SHARED_ULINK_PACK_H_

#ifdef BCM_UKERNEL
/* Build for uKernel not SDK */
#include "sdk_typedefs.h"
#else
#include <sal/types.h>
#endif

#define UL_MAX_NUM_PORTS                  256
#define UL_PBMP_PORT_MAX                  UL_MAX_NUM_PORTS 
#define UL_PBMP_WORD_WIDTH                32
#define UL_PBMP_BYTE_WIDTH                8
#define UL_PBMP_WBIT(port)                (1U<<((port) % UL_PBMP_WORD_WIDTH))
#define UL_PBMP_WENT(port)                ((port)/UL_PBMP_WORD_WIDTH)
#define UL_PBMP_WORD_GET(bm, word)        (bm[(word)])

#define UL_PBMP_WIDTH     (((UL_PBMP_PORT_MAX + 32 - 1)/32)*32)

#define UL_PBMP_WORD_MAX      \
    ((UL_PBMP_WIDTH + UL_PBMP_WORD_WIDTH-1) / UL_PBMP_WORD_WIDTH)

#define UL_PBMP_ENTRY(bm, port)   \
    (UL_PBMP_WORD_GET(bm, UL_PBMP_WENT(port)))
#define UL_PBMP_MEMBER(bm, port)  \
    ((UL_PBMP_ENTRY(bm, port) & UL_PBMP_WBIT(port)) != 0)

#define UL_PBMP_ASSIGN(dst, src)  (dst) = (src)

typedef struct __attribute__ ((__packed__)) _soc_ulink_pbm_msg {
    uint8 flags;         /* Not used currently */
    uint8 words;         /* Number of pbits words valid in message */
    uint32 pbits[UL_PBMP_WORD_MAX]; /* PBMP of link up ports */
} soc_ulink_pbm_msg_t;

#define UC_LINK_DMA_BUFFER_LEN      sizeof(soc_ulink_pbm_msg_t)

extern uint8 *
shr_ulink_pbm_unpack(uint8 *bufp, soc_ulink_pbm_msg_t *msg);

extern uint8 *
shr_ulink_pbm_pack(uint8 *bufp, soc_ulink_pbm_msg_t *msg);

#endif /* _SOC_SHARED_OLP_PACK_H_ */


