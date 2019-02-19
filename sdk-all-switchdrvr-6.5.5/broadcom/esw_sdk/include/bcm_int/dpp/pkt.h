/*
 * $Id: pkt.h,v 1.3 Broadcom SDK $
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
 * File:        pkt.h
 * Purpose:     Common Internal pkt utilities
 *            
 *
 */

#ifndef _BCM_PKT_H
#define _BCM_PKT_H

#include <shared/bsl.h>

#include <soc/defs.h>	
#include <bcm/debug.h>

#include <bcm/pkt.h>


/* Dump a DNX packet. */
extern void bcm_pkt_dnx_dump(
    bcm_pkt_t *pkt);
/* Dump itmh destination */
extern void bcm_pkt_dnx_itmh_dest_dump(
    bcm_pkt_dnx_itmh_dest_t *itmh_dest);

/* Dump itmh header. */
extern void bcm_pkt_dnx_itmh_dump(
    bcm_pkt_dnx_itmh_t *ext);

/* Dump ftmh lb extension. */
extern void bcm_pkt_dnx_ftmh_lb_extension_dump(
    bcm_pkt_dnx_ftmh_lb_extension_t *ext);

/* Dump ftmh dest extension. */
extern void bcm_pkt_dnx_ftmh_dest_extension_dump(
    bcm_pkt_dnx_ftmh_dest_extension_t *ext);

/* Dump ftmh stack extension. */
extern void bcm_pkt_dnx_ftmh_stack_extension_dump(
    bcm_pkt_dnx_ftmh_stack_extension_t *ext);

/* Dump ftmh header. */
extern void bcm_bcm_pkt_dnx_ftmh_dump(
    bcm_pkt_dnx_ftmh_t *ext);
#endif /* _BCM_PKT_H */
