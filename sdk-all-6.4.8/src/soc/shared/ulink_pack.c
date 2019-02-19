/* 
 * $Id: ulink_pack.c,v 1.0 Broadcom SDK $
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
 * File:        ulink_pack.c
 * Purpose:     ULINK pack and unpack routines for:
 *              - ukernel link module 
 *
 *
 * ULINK control messages
 *
 * ULINK messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The ULINK control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <soc/defs.h>
#include <shared/pack.h>
#include <soc/shared/ulink_pack.h>

/*********************************************************
 * ULINK port bitmap Pack/Unpack
 *
 * Functions:
 *      shr_ulink_pbm_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack link  
 *      bitmap to/from given buffer.
 * Parameters:
 *   _pack()
 *      buffer   - (OUT) Buffer where to pack bitmap.
 *      <header> - (IN)  bitmap to pack.
 *   _unpack()
 *      buffer   - (IN)  Buffer with data to unpack.
 *      <header> - (OUT) Returns unpack header.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_ulink_pbm_unpack(uint8 *bufp, soc_ulink_pbm_msg_t *msg)
{
    int i, word_max;

    _SHR_UNPACK_U8(bufp, msg->flags);
    _SHR_UNPACK_U8(bufp, msg->words);
    word_max = (msg->words <= UL_PBMP_WORD_MAX) ? msg->words : UL_PBMP_WORD_MAX;
    
    for(i = 0; i < word_max; i++) {
        _SHR_UNPACK_U32(bufp,msg->pbits[i]);
    }
    return bufp;
}

uint8 *
shr_ulink_pbm_pack(uint8 *bufp, soc_ulink_pbm_msg_t *msg)
{
    int i;
    _SHR_PACK_U8(bufp, msg->flags);
    _SHR_PACK_U8(bufp, msg->words);
    for(i = 0; i < msg->words; i++) {
        _SHR_PACK_U32(bufp,msg->pbits[i]);
    }
    return bufp;
}

