/* 
 * $Id: olp_pack.c,v 1.0 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * File:        olp_pack.c
 * Purpose:     OLP pack and unpack routines for:
 *              - OLP tx/rx header 
 *
 *
 * OLP control messages
 *
 * OLP messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The OLP control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <soc/defs.h>
#include <shared/pack.h>
#include <soc/shared/olp_pkt.h>
#include <soc/shared/olp_pack.h>

/*********************************************************
 * OLP Tx/Rx Network Packet Header Pack/Unpack
 *
 * Functions:
 *      shr_olp_tx/rx_<header>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      network packet header to/from given buffer.
 * Parameters:
 *   _pack()
 *      buffer   - (OUT) Buffer where to pack header.
 *      <header> - (IN)  Header to pack.
 *   _unpack()
 *      buffer   - (IN)  Buffer with data to unpack.
 *      <header> - (OUT) Returns unpack header.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_olp_tx_header_unpack(uint8 *bufp, soc_olp_tx_hdr_t *opt)
{

    _SHR_UNPACK_U32(bufp,opt->u1.h1);
    _SHR_UNPACK_U32(bufp,opt->u2.h2);
    _SHR_UNPACK_U32(bufp,opt->u3.h3);
    _SHR_UNPACK_U32(bufp,opt->u4.h4);
    return bufp;
}

uint8 *
shr_olp_tx_header_pack(uint8 *bufp, soc_olp_tx_hdr_t *opt)
{

    _SHR_PACK_U32(bufp,opt->u1.h1);
    _SHR_PACK_U32(bufp,opt->u2.h2);
    _SHR_PACK_U32(bufp,opt->u3.h3);
    _SHR_PACK_U32(bufp,opt->u4.h4);
    return bufp;
}

uint8 *
shr_olp_rx_header_unpack(uint8 *bufp, soc_olp_rx_hdr_t *opr)
{
    _SHR_UNPACK_U32(bufp,opr->u1.h1);
    _SHR_UNPACK_U32(bufp,opr->u2.h2);
    _SHR_UNPACK_U32(bufp,opr->u3.h3);
    _SHR_UNPACK_U32(bufp,opr->u4.h4);
    _SHR_UNPACK_U32(bufp,opr->u5.h5);
    return bufp;
}

uint8 *
shr_olp_rx_header_pack(uint8 *bufp, soc_olp_rx_hdr_t *opr)
{
    _SHR_PACK_U32(bufp,opr->u1.h1);
    _SHR_PACK_U32(bufp,opr->u2.h2);
    _SHR_PACK_U32(bufp,opr->u3.h3);
    _SHR_PACK_U32(bufp,opr->u4.h4);
    _SHR_PACK_U32(bufp,opr->u5.h5);
    return bufp;
}
