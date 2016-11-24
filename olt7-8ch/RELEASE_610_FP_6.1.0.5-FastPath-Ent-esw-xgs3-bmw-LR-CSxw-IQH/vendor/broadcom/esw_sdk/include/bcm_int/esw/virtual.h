/*
 * $Id: virtual.h,v 1.1 2011/04/18 17:10:56 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        virtual.h
 * Purpose:     Function declarations for VP / VFI resource management
 */

#ifndef _BCM_INT_RESOURCE_H_
#define _BCM_INT_RESOURCE_H_
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/mbcm.h>

/****************************************************************
 *
 * Virtual resource management functions
 *
 ****************************************************************/
#if defined(INCLUDE_L3)
typedef enum _bcm_vp_type_s {
    _bcmVpTypeMpls,      /* MPLS VP */
    _bcmVpTypeMim,       /* MIM VP */
    _bcmVpTypeSubport,   /* Subport VP */
    _bcmVpTypeWlan,      /* WLAN VP */
    _bcmVpTypeAny        /* Any VP */
} _bcm_vp_type_e;

typedef enum _bcm_vfi_type_s {
    _bcmVfiTypeMpls,     /* MPLS VFI */
    _bcmVfiTypeMim,      /* MIM VFI */
    _bcmVfiTypeAny       /* Any VFI */
} _bcm_vfi_type_e;

extern int _bcm_virtual_init(int unit, soc_mem_t vp_mem, soc_mem_t vfi_mem);
extern int _bcm_virtual_cleanup(int unit);
extern int _bcm_vp_alloc(int unit, int start, int end, int count, soc_mem_t vp_mem, 
                         _bcm_vp_type_e type, int *base_vp);
extern int _bcm_vp_free(int unit, _bcm_vp_type_e type, int count, int base_vp);
extern int _bcm_vp_used_set(int unit, int vp, _bcm_vp_type_e type);
extern int _bcm_vp_used_get(int unit, int vp, _bcm_vp_type_e type);
extern int _bcm_vp_if_used(int unit, int vp);
extern int _bcm_vfi_alloc(int unit, soc_mem_t vp_mem, _bcm_vfi_type_e type, int *vfi);
extern int _bcm_vfi_alloc_with_id(int unit, soc_mem_t vfi_mem, _bcm_vfi_type_e type, int vfi);
extern int _bcm_vfi_free(int unit, _bcm_vfi_type_e type, int vfi);
extern int _bcm_vfi_used_get(int unit, int vfi, _bcm_vfi_type_e type);
extern int _bcm_vfi_flex_stat_index_set(int unit, int vfi,
                                        _bcm_vfi_type_e type, int fs_idx);

#endif /* INCLUDE_L3 */

#endif /* BCM_TRX_SUPPORT */
#endif  /* !_BCM_INT_RESOURCE_H_ */


