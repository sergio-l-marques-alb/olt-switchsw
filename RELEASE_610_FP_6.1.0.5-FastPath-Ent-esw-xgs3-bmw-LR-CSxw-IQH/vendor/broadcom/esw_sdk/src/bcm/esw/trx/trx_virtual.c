/*
 * $Id: trx_virtual.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:    virtual.c
 * Purpose: Manages VP / VFI resources
 */
#if defined(INCLUDE_L3)
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/mcm/allenum.h>

#include <sal/compiler.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw/virtual.h>

typedef struct _bcm_virtual_bookkeeping_s {
    SHR_BITDCL  *vfi_bitmap;        /* Global VFI bitmap */
    SHR_BITDCL  *vp_bitmap;         /* Global Virtual Port bitmap */
    SHR_BITDCL  *mpls_vfi_bitmap;   /* MPLS VFI bitmap */
    SHR_BITDCL  *mpls_vp_bitmap;    /* MPLS Virtual Port bitmap */
    SHR_BITDCL  *mim_vfi_bitmap;    /* MIM VFI bitmap */
    SHR_BITDCL  *mim_vp_bitmap;     /* MIM Virtual Port bitmap */
    SHR_BITDCL  *subport_vp_bitmap; /* Subport Virtual Port bitmap */
    SHR_BITDCL  *wlan_vp_bitmap;    /* WLAN Virtual Port bitmap */
} _bcm_virtual_bookkeeping_t;

STATIC _bcm_virtual_bookkeeping_t  _bcm_virtual_bk_info[BCM_MAX_NUM_UNITS] = {{ 0 }};
STATIC sal_mutex_t _virtual_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define VIRTUAL_INFO(_unit_)   (&_bcm_virtual_bk_info[_unit_])

/*
 * VFI table usage bitmap operations
 */
#define _BCM_VIRTUAL_VFI_USED_GET(_u_, _vfi_) \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vfi_bitmap, (_vfi_))
#define _BCM_VIRTUAL_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vfi_bitmap, (_vfi_))
#define _BCM_VIRTUAL_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vfi_bitmap, (_vfi_))

/*
 * Virtual Port usage bitmap operations
 */
#define _BCM_VIRTUAL_VP_USED_GET(_u_, _vp_) \
        SHR_BITGET(VIRTUAL_INFO(_u_)->vp_bitmap, (_vp_))
#define _BCM_VIRTUAL_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->vp_bitmap, (_vp_))
#define _BCM_VIRTUAL_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->vp_bitmap, (_vp_))

/*
 * MPLS VFI table usage bitmap operations
 */
#define _BCM_MPLS_VFI_USED_GET(_u_, _vfi_) \
        SHR_BITGET(VIRTUAL_INFO(_u_)->mpls_vfi_bitmap, (_vfi_))
#define _BCM_MPLS_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->mpls_vfi_bitmap, (_vfi_))
#define _BCM_MPLS_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->mpls_vfi_bitmap, (_vfi_))

/*
 * MPLS Virtual Port usage bitmap operations
 */
#define _BCM_MPLS_VP_USED_GET(_u_, _vp_) \
        SHR_BITGET(VIRTUAL_INFO(_u_)->mpls_vp_bitmap, (_vp_))
#define _BCM_MPLS_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->mpls_vp_bitmap, (_vp_))
#define _BCM_MPLS_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->mpls_vp_bitmap, (_vp_))

/*
 * MIM VFI table usage bitmap operations
 */
#define _BCM_MIM_VFI_USED_GET(_u_, _vfi_) \
        SHR_BITGET(VIRTUAL_INFO(_u_)->mim_vfi_bitmap, (_vfi_))
#define _BCM_MIM_VFI_USED_SET(_u_, _vfi_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->mim_vfi_bitmap, (_vfi_))
#define _BCM_MIM_VFI_USED_CLR(_u_, _vfi_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->mim_vfi_bitmap, (_vfi_))

/*
 * MIM Virtual Port usage bitmap operations
 */
#define _BCM_MIM_VP_USED_GET(_u_, _vp_) \
        SHR_BITGET(VIRTUAL_INFO(_u_)->mim_vp_bitmap, (_vp_))
#define _BCM_MIM_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->mim_vp_bitmap, (_vp_))
#define _BCM_MIM_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->mim_vp_bitmap, (_vp_))

/* 
 * Virtual resource lock
 */
#define VIRTUAL_LOCK(unit) \
        sal_mutex_take(_virtual_mutex[unit], sal_mutex_FOREVER); 

#define VIRTUAL_UNLOCK(unit) \
        sal_mutex_give(_virtual_mutex[unit]); 

/*
 * Subport Virtual Port usage bitmap operations
 */
#define _BCM_SUBPORT_VP_USED_GET(_u_, _vp_) \
        SHR_BITGET(VIRTUAL_INFO(_u_)->subport_vp_bitmap, (_vp_))
#define _BCM_SUBPORT_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->subport_vp_bitmap, (_vp_))
#define _BCM_SUBPORT_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->subport_vp_bitmap, (_vp_))

/*
 * WLAN Virtual Port usage bitmap operations
 */
#define _BCM_WLAN_VP_USED_GET(_u_, _vp_) \
        SHR_BITGET(VIRTUAL_INFO(_u_)->wlan_vp_bitmap, (_vp_))
#define _BCM_WLAN_VP_USED_SET(_u_, _vp_) \
        SHR_BITSET(VIRTUAL_INFO((_u_))->wlan_vp_bitmap, (_vp_))
#define _BCM_WLAN_VP_USED_CLR(_u_, _vp_) \
        SHR_BITCLR(VIRTUAL_INFO((_u_))->wlan_vp_bitmap, (_vp_))

/*
 * Function:
 *      _bcm_virtual_free_resource
 * Purpose:
 *      Free all allocated resources
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_virtual_free_resource(int unit, _bcm_virtual_bookkeeping_t *virtual_info)
{
    if (!virtual_info) {
        return;
    }

    if (_virtual_mutex[unit]) {
        sal_mutex_destroy(_virtual_mutex[unit]);
        _virtual_mutex[unit] = NULL;
    } 

    if (virtual_info->vfi_bitmap) {
        sal_free(virtual_info->vfi_bitmap);
        virtual_info->vfi_bitmap = NULL;
    }

    if (virtual_info->vp_bitmap) {
        sal_free(virtual_info->vp_bitmap);
        virtual_info->vp_bitmap = NULL;
    }

    if (virtual_info->mpls_vfi_bitmap) {
        sal_free(virtual_info->mpls_vfi_bitmap);
        virtual_info->mpls_vfi_bitmap = NULL;
    }

    if (virtual_info->mpls_vp_bitmap) {
        sal_free(virtual_info->mpls_vp_bitmap);
        virtual_info->mpls_vp_bitmap = NULL;
    }

    if (virtual_info->mim_vfi_bitmap) {
        sal_free(virtual_info->mim_vfi_bitmap);
        virtual_info->mim_vfi_bitmap = NULL;
    }

    if (virtual_info->mim_vp_bitmap) {
        sal_free(virtual_info->mim_vp_bitmap);
        virtual_info->mim_vp_bitmap = NULL;
    }

    if (virtual_info->subport_vp_bitmap) {
        sal_free(virtual_info->subport_vp_bitmap);
        virtual_info->subport_vp_bitmap = NULL;
    }

    if (virtual_info->wlan_vp_bitmap) {
        sal_free(virtual_info->wlan_vp_bitmap);
        virtual_info->wlan_vp_bitmap = NULL;
    }
}

/*
 * Function:
 *      _bcm_virtual_init
 * Purpose:
 *      Internal function for initializing virtual resource management
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */

int _bcm_virtual_init(int unit, soc_mem_t vp_mem, soc_mem_t vfi_mem)
{
    int num_vfi, num_vp, num_wlan_vp=0, vp, rv = BCM_E_NONE;
    _bcm_virtual_bookkeeping_t *virtual_info = VIRTUAL_INFO(unit);

    num_vfi = soc_mem_index_count(unit, vfi_mem);
    num_vp = soc_mem_index_count(unit, vp_mem);
    if (soc_feature(unit, soc_feature_wlan)) {
        num_wlan_vp = soc_mem_index_count(unit, WLAN_SVP_TABLEm);
    }
    /* 
     * Allocate resources 
     */
    if (NULL == _virtual_mutex[unit]) {
        _virtual_mutex[unit] = sal_mutex_create("virtual mutex");
        if (_virtual_mutex[unit] == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->vfi_bitmap) {
        virtual_info->vfi_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vfi), "vfi_bitmap");
        if (virtual_info->vfi_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->vp_bitmap) {
        virtual_info->vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "vp_bitmap");
        if (virtual_info->vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->mpls_vfi_bitmap) {
        virtual_info->mpls_vfi_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vfi), "mpls_vfi_bitmap");
        if (virtual_info->mpls_vfi_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->mpls_vp_bitmap) {
        virtual_info->mpls_vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "mpls_vp_bitmap");
        if (virtual_info->mpls_vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->mim_vfi_bitmap) {
        virtual_info->mim_vfi_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vfi), "mim_vfi_bitmap");
        if (virtual_info->mim_vfi_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->mim_vp_bitmap) {
        virtual_info->mim_vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "mim_vp_bitmap");
        if (virtual_info->mim_vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == virtual_info->subport_vp_bitmap) {
        virtual_info->subport_vp_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_vp), "subport_vp_bitmap");
        if (virtual_info->subport_vp_bitmap == NULL) {
            _bcm_virtual_free_resource(unit, virtual_info);
            return BCM_E_MEMORY;
        }
    }

    if (soc_feature(unit, soc_feature_wlan)) {
        if (NULL == virtual_info->wlan_vp_bitmap) {
            virtual_info->wlan_vp_bitmap =
                sal_alloc(SHR_BITALLOCSIZE(num_wlan_vp), "wlan_vp_bitmap");
            if (virtual_info->wlan_vp_bitmap == NULL) {
                _bcm_virtual_free_resource(unit, virtual_info);
                return BCM_E_MEMORY;
            }
        }
    }
    /*
     * Initialize 
     */ 
    sal_memset(virtual_info->vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
    sal_memset(virtual_info->vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    sal_memset(virtual_info->mpls_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
    sal_memset(virtual_info->mpls_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    sal_memset(virtual_info->mim_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vfi));
    sal_memset(virtual_info->mim_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    sal_memset(virtual_info->subport_vp_bitmap, 0, SHR_BITALLOCSIZE(num_vp));
    if (soc_feature(unit, soc_feature_wlan)) {
        sal_memset(virtual_info->wlan_vp_bitmap, 0, SHR_BITALLOCSIZE(num_wlan_vp));
    }
    /* HW retriction - mark VP index zero as used */
    rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, _bcmVpTypeAny, &vp);
    if (vp != 0) {
        rv = BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_virtual_cleanup
 * Purpose:
 *      Internal function for cleaning up virtual resource management
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */

int _bcm_virtual_cleanup(int unit)
{
    _bcm_virtual_bookkeeping_t *virtual_info = VIRTUAL_INFO(unit);
    _bcm_virtual_free_resource(unit, virtual_info);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vp_alloc
 * Purpose:
 *      Internal function for allocating a group of VPs
 * Parameters:
 *      unit    -  (IN) Device number.
 *      start   -  (IN) First VP index to allocate from 
 *      end     -  (IN) Last VP index to allocate from 
 *      count   -  (IN) How many consecutive VPs to allocate
 *      vp_mem  -  (IN) HW specific VP memory
 *      type    -  (IN) VP type
 *      base_vp -  (OUT) Base VP index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vp_alloc(int unit, int start, int end, int count, soc_mem_t vp_mem, 
              _bcm_vp_type_e type, int *base_vp)
{
    int num_vp = soc_mem_index_count(unit, vp_mem);
    int i, j;

    if ((end >= num_vp) || (start >= num_vp)) {
        return BCM_E_PARAM;
    }
    
    VIRTUAL_LOCK(unit);
    for (i = start; i <= end; i += count) {
        for (j = 0; j < count; j++) {
            if (_BCM_VIRTUAL_VP_USED_GET(unit, i + j)) {
                break;
            }
        }
        if (j == count) {
            break;
        }
    }
    if (i <= end) {
        *base_vp = i;
        for (j = 0; j < count; j++) {
            _BCM_VIRTUAL_VP_USED_SET(unit, i + j);
            switch (type) {
            case _bcmVpTypeMpls:
                _BCM_MPLS_VP_USED_SET(unit, i + j);
                break;
            case _bcmVpTypeMim:
                _BCM_MIM_VP_USED_SET(unit, i + j);
                break;
            case _bcmVpTypeSubport:
                _BCM_SUBPORT_VP_USED_SET(unit, i + j);
                break;
            case _bcmVpTypeWlan:
                _BCM_WLAN_VP_USED_SET(unit, i + j);
                break;
            default:
                break;
            }
        }
        VIRTUAL_UNLOCK(unit);
        return BCM_E_NONE;
    }
    VIRTUAL_UNLOCK(unit);
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_vp_free
 * Purpose:
 *      Internal function for freeing a group of VPs
 * Parameters:
 *      unit    -  (IN) Device number
 *      type    -  (IN) VP type
 *      count   -  (IN) How many consecutive VPs to free
 *      base_vp -  (IN) Base VP index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vp_free(int unit, _bcm_vp_type_e type, int count, int base_vp)
{
    int i;
    VIRTUAL_LOCK(unit);
    for (i = 0; i < count; i++) {
        _BCM_VIRTUAL_VP_USED_CLR(unit, base_vp + i);
        switch (type) {
        case _bcmVpTypeMpls:
            _BCM_MPLS_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeMim:
            _BCM_MIM_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeSubport:
            _BCM_SUBPORT_VP_USED_CLR(unit, base_vp + i);
            break;
        case _bcmVpTypeWlan:
            _BCM_WLAN_VP_USED_CLR(unit, base_vp + i);
            break;
        default:
            break;
        }
    }
    VIRTUAL_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vp_used_set
 * Purpose:
 *      Mark the VP as used
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vp      -  (IN) VP
 *      type   -  (IN) VP type
 * Returns:
 *      Boolean
 */
int
_bcm_vp_used_set(int unit, int vp, _bcm_vp_type_e type)
{
    int rv=BCM_E_NONE;
    VIRTUAL_LOCK(unit);
	
    _BCM_VIRTUAL_VP_USED_SET(unit, vp);
    switch (type) {
         case _bcmVpTypeMpls:
              _BCM_MPLS_VP_USED_SET(unit, vp);
             break;
         case _bcmVpTypeMim:
              _BCM_MIM_VP_USED_SET(unit, vp);
             break;
         case _bcmVpTypeSubport:
              _BCM_SUBPORT_VP_USED_SET(unit, vp);
              break;
         case _bcmVpTypeWlan:
              _BCM_WLAN_VP_USED_SET(unit, vp);
              break;
         default:
              break;
    }

    VIRTUAL_UNLOCK(unit);
    return rv;
}



/*
 * Function:
 *      _bcm_vp_used_get
 * Purpose:
 *      Check whether a VP is used or not
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vp      -  (IN) VP
 * Returns:
 *      Boolean
 */
int
_bcm_vp_used_get(int unit, int vp, _bcm_vp_type_e type)
{
    int rv;
    
    switch (type) {
    case _bcmVpTypeMpls:
        rv = _BCM_MPLS_VP_USED_GET(unit, vp);
        break;
    case _bcmVpTypeMim:
        rv = _BCM_MIM_VP_USED_GET(unit, vp);
        break;
    case _bcmVpTypeSubport:
        rv = _BCM_SUBPORT_VP_USED_GET(unit, vp);
        break;
    case _bcmVpTypeWlan:
        rv = _BCM_WLAN_VP_USED_GET(unit, vp);
        break;
    default:
        rv = _BCM_VIRTUAL_VP_USED_GET(unit, vp);
        break;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_vp_if_used
 * Purpose:
 *      Check whether a specific VP is used SystemWide
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vp      -  (IN) VP
 * Returns:
 *      Boolean
 */
int
_bcm_vp_if_used(int unit, int vp)
{
    int rv = BCM_E_NONE;
    VIRTUAL_LOCK(unit);
    
    if ( _bcm_vp_used_get(unit, vp, _bcmVpTypeMpls) ||
         _bcm_vp_used_get(unit, vp, _bcmVpTypeMim) ||
         _bcm_vp_used_get(unit, vp, _bcmVpTypeSubport) ||
         _bcm_vp_used_get(unit, vp, _bcmVpTypeWlan) ) {
              VIRTUAL_UNLOCK(unit);
              return BCM_E_EXISTS;
    }
		
    VIRTUAL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vfi_alloc
 * Purpose:
 *      Internal function for allocating a VFI
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi_mem -  (IN) HW specific VFI memory
 *      type    -  (IN) VFI type
 *      vfi     -  (OUT) Base VP index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_alloc(int unit, soc_mem_t vfi_mem, _bcm_vfi_type_e type, int *vfi)
{
    int i, num_vfi;
    num_vfi = soc_mem_index_count(unit, vfi_mem);
    VIRTUAL_LOCK(unit);
    for (i = 0; i < num_vfi; i++) {
        if (!_BCM_VIRTUAL_VFI_USED_GET(unit, i)) {
            break;
        }
    }
    if (i == num_vfi) {
        VIRTUAL_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }
    *vfi = i;
    _BCM_VIRTUAL_VFI_USED_SET(unit, i);
    switch (type) {
    case _bcmVfiTypeMpls:
        _BCM_MPLS_VFI_USED_SET(unit, i);
        break;
    case _bcmVfiTypeMim:
        _BCM_MIM_VFI_USED_SET(unit, i);
        break;
    default:
        break;
    }
    VIRTUAL_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vfi_alloc_with_id
 * Purpose:
 *      Internal function for allocating a VFI with a given ID
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi_mem -  (IN) HW specific VFI memory
 *      type    -  (IN) VFI type
 *      vfi     -  (IN) VFI index
 * Returns:
 *      BCM_X_XXX
 */
 
int
_bcm_vfi_alloc_with_id(int unit, soc_mem_t vfi_mem, _bcm_vfi_type_e type, int vfi)
{
    int num_vfi;
    num_vfi = soc_mem_index_count(unit, vfi_mem);

    /* Check Valid range of VFI */
    if ( vfi < 0 && vfi >= num_vfi ) {
         return BCM_E_RESOURCE;
    }
   
    VIRTUAL_LOCK(unit);
    if (_BCM_VIRTUAL_VFI_USED_GET(unit, vfi)) {
         return BCM_E_EXISTS;
   }
		
    _BCM_VIRTUAL_VFI_USED_SET(unit, vfi);
	
    switch (type) {
    case _bcmVfiTypeMpls:
        _BCM_MPLS_VFI_USED_SET(unit, vfi);
        break;
    case _bcmVfiTypeMim:
        _BCM_MIM_VFI_USED_SET(unit, vfi);
        break;
    default:
        break;
    }
    VIRTUAL_UNLOCK(unit);
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_vfi_free
 * Purpose:
 *      Internal function for freeing a VFI
 * Parameters:
 *      unit    -  (IN) Device number.
 *      type    -  (IN) VFI type
 *      base_vfi - (IN) VFI index
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_vfi_free(int unit, _bcm_vfi_type_e type, int vfi)
{
    VIRTUAL_LOCK(unit);
    _BCM_VIRTUAL_VFI_USED_CLR(unit, vfi);
    switch (type) {
    case _bcmVfiTypeMpls:
        _BCM_MPLS_VFI_USED_CLR(unit, vfi);
        break;
    case _bcmVfiTypeMim:
        _BCM_MIM_VFI_USED_CLR(unit, vfi);
        break;
    default:
        break;
    }
    VIRTUAL_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vfi_used_get
 * Purpose:
 *      Check whether a VFI is used or not 
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi     -  (IN) VFI
 *      type    -  (IN) VFI type
 * Returns:
 *      Boolean
 */
int
_bcm_vfi_used_get(int unit, int vfi, _bcm_vfi_type_e type)
{
    int rv;
    VIRTUAL_LOCK(unit);
    switch (type) {
    case _bcmVfiTypeMpls:
        rv = _BCM_MPLS_VFI_USED_GET(unit, vfi);
        break;
    case _bcmVfiTypeMim:
        rv = _BCM_MIM_VFI_USED_GET(unit, vfi);
        break;
    default:
        rv = _BCM_VIRTUAL_VFI_USED_GET(unit, vfi);
        break;
    }
    VIRTUAL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vfi_flex_stat_index_set
 * Purpose:
 *      Associate a flexible stat with a VFI
 * Parameters:
 *      unit    -  (IN) Device number.
 *      vfi     -  (IN) VFI
 *      fs_idx  -  (IN) Flexible stat index
 * Returns:
 *      Boolean
 */
int
_bcm_vfi_flex_stat_index_set(int unit, int vfi, _bcm_vfi_type_e type,
                             int fs_idx)
{
    int rv;
    VIRTUAL_LOCK(unit);
    if (_bcm_vfi_used_get(unit, vfi, type)) {
        rv = soc_mem_field32_modify(unit, VFIm, vfi,
                                    SERVICE_CTR_IDXf, fs_idx);
        if (BCM_SUCCESS(rv)) {
            rv = soc_mem_field32_modify(unit, EGR_VFIm, vfi,
                                        SERVICE_CTR_IDXf, fs_idx);
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    VIRTUAL_UNLOCK(unit);
    return rv;
}
#else /* INCLUDE_L3 */
int _bcm_esw_trx_virtual_not_empty;
#endif  /* INCLUDE_L3 */
