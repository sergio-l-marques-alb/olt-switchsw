/*
 * $Id: esw_vlan.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * Module: VLAN management
 *
 * Purpose:
 *     These routines manage the VTABLE entries as well as VLAN-related
 *     fields in the PTABLE.
 *
 *     The bcm layer keeps the VTABLE and PTABLE coherent in terms of
 *     what ports belong to what VLANs and which ports are untagged.
 *
 */

#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <sal/core/boot.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/debug.h>

#include <bcm_int/common/lock.h>
#include <bcm_int/control.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stg.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/draco.h>
#if defined(BCM_EASYRIDER_SUPPORT)
#include <bcm_int/esw/easyrider.h>
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/multicast.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/mim.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>

#define VLAN_OUT(flags, stuff)  BCM_DEBUG(flags | BCM_DBG_VLAN, stuff)

#define VLAN_WARN(stuff)        VLAN_OUT(BCM_DBG_WARN, stuff)
#define VLAN_ERR(stuff)         VLAN_OUT(BCM_DBG_ERR, stuff)
#define VLAN_VERB(stuff)        VLAN_OUT(BCM_DBG_VERBOSE, stuff)


bcm_vlan_info_t vlan_info[BCM_MAX_NUM_UNITS];

#ifndef BCM_VLAN_VID_MAX
#define BCM_VLAN_VID_MAX        4095
#endif

#define CHECK_INIT(unit)                                        \
        if (!vlan_info[unit].init)                              \
            return BCM_E_INIT

#define CHECK_VID(unit, vid)                                    \
        if ((vid) > BCM_VLAN_VID_MAX)                           \
            return BCM_E_PARAM

STATIC int _bcm_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp,
                              pbmp_t ubmp);

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_vlan_reinit(int unit);
#else
#define    _bcm_vlan_reinit(unit)     (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * For BCM internal use to set vlan create default flood mode.
 * Follow the same locking convention as other members in the structure
 */
/*
 * Set the default PFM to be used by vlan_create
 * Called from bcm_switch_control_set
 */
int
_bcm_esw_vlan_flood_default_set(int unit, bcm_vlan_mcast_flood_t mode)
{
    bcm_vlan_info_t             *vi = &vlan_info[unit];

    vi->flood_mode = mode;

    return BCM_E_NONE;
}

/*
 * Get the default PFM used by vlan_create
 * Called from bcm_switch_control_get, bcm_vlan_create
 */
int
_bcm_esw_vlan_flood_default_get(int unit, bcm_vlan_mcast_flood_t *mode)
{
    bcm_vlan_info_t             *vi = &vlan_info[unit];

    *mode = vi->flood_mode;

    return BCM_E_NONE;
}

/*
 * Set the PFM to be used in the Module Header for L2 packets when 
 * EGR_CONFIG.STATIC_MH_PFM == 1. When STATIC_MH_PFM==0, the PFM in 
 * the MH comes from the Vlan table.
 */
int
_bcm_esw_higig_flood_l2_set(int unit, bcm_vlan_mcast_flood_t mode)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32              rval;

    if (soc_feature(unit, soc_feature_static_pfm)) {
        if (mode >= BCM_VLAN_MCAST_FLOOD_COUNT) {
            return BCM_E_PARAM;
        }

        if (SOC_IS_EASYRIDER(unit)) {
            SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
            soc_reg_field_set(unit, EGR_CONFIGr, &rval, MH_PFMf, mode);
            SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIGr(unit, rval));
        } else {
            SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
            soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, MH_PFMf, mode);
            SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));
        }
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Get the PFM to be used in the Module Header for L2 packets when 
 * EGR_CONFIG.STATIC_MH_PFM == 1.
 */
int
_bcm_esw_higig_flood_l2_get(int unit, bcm_vlan_mcast_flood_t *mode)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32              rval;

    if (soc_feature(unit, soc_feature_static_pfm)) {
        if (SOC_IS_EASYRIDER(unit)) {
            SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
            *mode = soc_reg_field_get(unit, EGR_CONFIGr, rval, MH_PFMf);
        } else {
            SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
            *mode = soc_reg_field_get(unit, EGR_CONFIG_1r, rval, MH_PFMf);
        }
        return BCM_E_NONE;
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Set the PFM to be used in the Module Header for IPMC packets.
 * On ERC0, this PFM is used when EGR_CONFIG.STATIC_L3MC_PFM == 1
 * Otherwise, the PFM in the MH comes from the Vlan table.
 */
int
_bcm_esw_higig_flood_l3_set(int unit, bcm_vlan_mcast_flood_t mode)
{
#ifdef BCM_EASYRIDER_SUPPORT
    uint32              rval;

    if (SOC_IS_EASYRIDER(unit)) {
        if (soc_feature(unit, soc_feature_static_pfm)) {
            if (mode >= BCM_VLAN_MCAST_FLOOD_COUNT) {
                return BCM_E_PARAM;
            }
            SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
            soc_reg_field_set(unit, EGR_CONFIGr, &rval, L3MC_MH_PFMf, mode);
            SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIGr(unit, rval));
            return BCM_E_NONE;
        }
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Get the PFM to be used in the Module Header for IPMC packets.
 * On ERC0, this PFM is used when EGR_CONFIG.STATIC_L3MC_PFM == 1
 */
int
_bcm_esw_higig_flood_l3_get(int unit, bcm_vlan_mcast_flood_t *mode)
{
#ifdef BCM_EASYRIDER_SUPPORT
    uint32              rval;

    if (SOC_IS_EASYRIDER(unit)) {
        if (soc_feature(unit, soc_feature_static_pfm)) {
            SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
            *mode = soc_reg_field_get(unit, EGR_CONFIGr, rval, L3MC_MH_PFMf);
            return BCM_E_NONE;
        }
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * MACRO:
 *      _BCM_VBMP_INSERT
 * Purpose:
 *      Add a VLAN ID to a vbmap
 */
#define _BCM_VBMP_INSERT(_bmp_, _vid_)    SHR_BITSET(((_bmp_).w), (_vid_))

/*
 * Macro:
 *      _BCM_VBMP_REMOVE
 * Purpose:
 *      Delete a VLAN ID from a vbmp
 */
#define _BCM_VBMP_REMOVE(_bmp_, _vid_)    SHR_BITCLR(((_bmp_).w), (_vid_))

/*
 * Macro :
 *      _BCM_VBMP_LOOKUP
 * Purpose:
 *      Return TRUE if a VLAN ID exists in a vbmp, FALSE otherwise
 */
#define _BCM_VBMP_LOOKUP(_bmp_, _vid_)    SHR_BITGET(((_bmp_).w), (_vid_))

/*
 * Function:
 *      _bcm_vbmp_destroy
 * Purpose:
 *      Free all memory used by a active VLANs bitmap
 */

STATIC int
_bcm_vbmp_destroy(vbmp_t *bmp)
{
    if (bmp->w != NULL) {
        sal_free(bmp->w);
        bmp->w = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vbmp_init
 * Purpose:
 *      Create new valid vlans bitmap.
 */

STATIC int
_bcm_vbmp_init(vbmp_t *bmp)
{
    int alloc_size;

    /* Deinit previously allocated bitmap. */
    _bcm_vbmp_destroy(bmp);

    alloc_size = SHR_BITALLOCSIZE(BCM_VLAN_COUNT);
    bmp->w = sal_alloc(alloc_size, "Active vlans bitmap");
    if (NULL == bmp->w) {
        return (BCM_E_MEMORY);
    }
    sal_memset(bmp->w, 0, alloc_size);

    return BCM_E_NONE;
}

/*
 * Check for valid VLAN. If easy reload, check software state;
 * otherwise, check hardware entry
 */
int _bcm_vlan_valid_check(int unit, int table, vlan_tab_entry_t *vt, 
                     bcm_vlan_t vid) 
{
#if defined(BCM_EASY_RELOAD_SUPPORT)                          
    if (SOC_IS_RELOADING(unit)) {
        vbmp_t vbmp;
        
        if (! ((table == EGR_VLANm) || (table == VLAN_TABm))) {
            return BCM_E_PARAM;
        }
        vbmp = (table == EGR_VLANm) ? vlan_info[unit].egr_vlan_bmp :
               vlan_info[unit].vlan_tab_bmp;
        if (! _BCM_VBMP_LOOKUP(vbmp, vid)) {       
            return BCM_E_NOT_FOUND;                             
        }
    } else                                                    
#endif                                                        
    {                                                         
        if (!soc_mem_field32_get(unit, table, vt, VALIDf)) {  
            return BCM_E_NOT_FOUND;                             
        }
    }
    return BCM_E_NONE;
}
    
#if defined(BCM_EASY_RELOAD_SUPPORT)                          
/*
 * Set valid VLAN as software state for easy reload
 */
int _bcm_vlan_valid_set(int unit, int table, bcm_vlan_t vid, int val)
{
    vbmp_t vbmp;
    if (! ((table == EGR_VLANm) || (table == VLAN_TABm))) {
        return BCM_E_PARAM;
    }
    vbmp = (table == EGR_VLANm) ? vlan_info[unit].egr_vlan_bmp :
           vlan_info[unit].vlan_tab_bmp;
    
    if (val) {
        _BCM_VBMP_INSERT(vbmp, vid);
    } else {
        _BCM_VBMP_REMOVE(vbmp, vid);
    }
    return BCM_E_NONE;
}
#endif



/*
 * Function:
 *      bcm_vlan_init
 * Purpose:
 *      Initialize the VLAN module.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 */

int
bcm_esw_vlan_init(int unit)
{
    bcm_vlan_info_t             *vi = &vlan_info[unit];

    if (SOC_WARM_BOOT(unit) || SOC_IS_RCPU_ONLY(unit)) {
        return(_bcm_vlan_reinit(unit));
    } else {
       
        bcm_vlan_data_t         vd;

        /*
         * Initialize hardware tables
         */

        vd.vlan_tag = BCM_VLAN_DEFAULT;
        BCM_PBMP_ASSIGN(vd.port_bitmap, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_LB(unit));
        BCM_PBMP_ASSIGN(vd.ut_port_bitmap, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_LB(unit));

        /*
         * A compile-time application policy may prefer to not add
         * Ethernet or CPU ports to the default VLAN.
         */

#ifdef  BCM_VLAN_NO_DEFAULT_ETHER
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_E_ALL(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_E_ALL(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_ETHER */
#ifdef  BCM_VLAN_NO_DEFAULT_CPU
        BCM_PBMP_REMOVE(vd.port_bitmap, PBMP_CMIC(unit));
#endif  /* BCM_VLAN_NO_DEFAULT_CPU */
#if defined(BCM_VLAN_NO_AUTO_STACK)
        BCM_PBMP_REMOVE(vd.port_bitmap, SOC_PBMP_STACK_CURRENT(unit));
        BCM_PBMP_REMOVE(vd.ut_port_bitmap, SOC_PBMP_STACK_CURRENT(unit));
#endif  /* BCM_VLAN_NO_AUTO_STACK */

#ifdef BCM_MCAST_FLOOD_DEFAULT
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_flood_default_set(unit, BCM_MCAST_FLOOD_DEFAULT));
#else
        BCM_IF_ERROR_RETURN
            (_bcm_esw_vlan_flood_default_set(unit,
                                             BCM_VLAN_MCAST_FLOOD_UNKNOWN));
#endif

#if defined (BCM_EASY_RELOAD_SUPPORT)
        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->egr_vlan_bmp));
        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->vlan_tab_bmp));
#endif

        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_vlan_init(unit, &vd));

#ifdef BCM_EASYRIDER_SUPPORT
        if (soc_feature(unit, soc_feature_vlan_translation_range)) {
            if (SOC_IS_EASYRIDER(unit)) {
                BCM_IF_ERROR_RETURN
                    (bcm_er_vlan_range_profile_init(unit));
            }
        }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_gport_service_counters)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_flex_stat_init(unit, _bcmFlexStatTypeService));
        }
        if (soc_feature(unit, soc_feature_vlan_queue_map)) {
            int size;

            if (vi->qm_bmp != NULL) {
                sal_free(vi->qm_bmp);
            }
            if (vi->qm_it_bmp != NULL) {
                sal_free(vi->qm_it_bmp);
            }

            size = SHR_BITALLOCSIZE(soc_mem_index_count(unit,
                                                        PHB2_COS_MAPm) >> 4);
            vi->qm_bmp = sal_alloc(size, "VLAN queue mapping bitmap");
            if (vi->qm_bmp == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(vi->qm_bmp, 0, size); 
            vi->qm_it_bmp = sal_alloc(size, "VLAN queue mapping bitmap");
            if (vi->qm_it_bmp == NULL) {
                return BCM_E_MEMORY;
            }
        }
#endif

        /*
         * Initialize software structures
         */

        vi->defl = BCM_VLAN_DEFAULT;

        /* In case bcm_vlan_init is called more than once */
        BCM_IF_ERROR_RETURN(_bcm_vbmp_init(&vi->bmp));
        _BCM_VBMP_INSERT(vi->bmp, vd.vlan_tag);

#if defined (BCM_EASY_RELOAD_SUPPORT)
        _BCM_VBMP_INSERT(vi->egr_vlan_bmp, vd.vlan_tag);
        _BCM_VBMP_INSERT(vi->vlan_tab_bmp, vd.vlan_tag);
#endif

        /* Free vlan translation arrays if any. */
        if (NULL != vi->egr_trans) {
            sal_free(vi->egr_trans);
            vi->egr_trans = NULL;
        }
        if (NULL != vi->ing_trans) {
            sal_free(vi->ing_trans);
            vi->ing_trans = NULL;
        }

        vi->count = 1;
    }

    vi->init = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_vlan_detach
 * Purpose:
 *      De-initialize the VLAN module.
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */

int 
bcm_esw_vlan_detach(int unit)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];
    int rv = BCM_E_NONE;

    BCM_LOCK(unit);

    _bcm_vbmp_destroy(&vi->bmp);

    if (vi->egr_trans != NULL) {
        sal_free(vi->egr_trans);
        vi->egr_trans = NULL;
    }
    if (vi->ing_trans != NULL) {
        sal_free(vi->ing_trans);
        vi->ing_trans = NULL;
    }

    vi->init = 0;

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        rv = _bcm_esw_flex_stat_detach(unit, _bcmFlexStatTypeService);
    }
    if (soc_feature(unit, soc_feature_vlan_queue_map)) {
        sal_free(vi->qm_bmp);
        vi->qm_bmp = NULL;
        sal_free(vi->qm_it_bmp);
        vi->qm_it_bmp  = NULL;
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
    || defined(BCM_RAVEN_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        rv = _bcm_xgs3_vlan_profile_detach(unit);
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || \
          BCM_RAVEN_SUPPORT */

    BCM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_vlan_create
 * Purpose:
 *      Main body of bcm_vlan_create; assumes locking already done;
 */

STATIC int
_bcm_vlan_create(int unit, bcm_vlan_t vid)
{
    bcm_stg_t   stg_defl;
    int         rv;

    rv = bcm_esw_stg_default_get(unit, &stg_defl);
    if (rv == BCM_E_UNAVAIL) {
        stg_defl = -1;
    } else if (rv < 0) {
        return rv;
    }

    if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_EXISTS;
    }

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_create(unit, vid));

    _BCM_VBMP_INSERT(vlan_info[unit].bmp, vid);

    vlan_info[unit].count++;

    if (stg_defl >= 0) {
        /* Must be after v bitmap insert */
        BCM_IF_ERROR_RETURN
            (bcm_esw_stg_vlan_add(unit, stg_defl, vid));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_create
 * Purpose:
 *      Allocate and configure a VLAN on StrataSwitch.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to create.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_EXISTS - VLAN ID already in use.
 * Notes:
 *      VLAN is placed in the default STG and can be reassigned later.
 */

int
bcm_esw_vlan_create(int unit, bcm_vlan_t vid)
{
    int                 rv;

    VLAN_VERB(("VLAN %d: create vid %d\n", unit, vid));

    CHECK_INIT(unit);
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_vlan_create(unit, vid);

#if !defined(BCM_VLAN_NO_AUTO_STACK) /* Add in stack ports if so configured */
    if (rv == BCM_E_NONE) {
        soc_pbmp_t  stacked, empty_pbmp;

        SOC_PBMP_ASSIGN(stacked, SOC_PBMP_STACK_CURRENT(unit));
        SOC_PBMP_CLEAR(empty_pbmp);

        rv = _bcm_vlan_port_add(unit, vid, stacked, empty_pbmp);
    }
#endif  /* BCM_VLAN_NO_AUTO_STACK */
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_destroy
 * Purpose:
 *      Main body of bcm_vlan_destroy; assumes locking done.
 */

STATIC int
_bcm_vlan_destroy(int unit, bcm_vlan_t vid)
{
    bcm_stg_t           stg;
    pbmp_t              pbmp, ubmp;
    int                 rv;
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    bcm_multicast_t     group;
    vlan_tab_entry_t    vtab;
    int                 mc_index = 0;
#endif

    /* Cannot destroy default VLAN */
    if (vid == vlan_info[unit].defl) {
        return BCM_E_BADID;
    }

    if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, vid, &pbmp, &ubmp));

    /* Remove VLAN from its spanning tree group */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_stg_get(unit, vid, &stg));

    rv = _bcm_stg_vlan_destroy(unit, stg, vid);
    if (rv < 0 && rv != BCM_E_UNAVAIL) {
        return rv;
    }

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    /* If virtual port switching is enabled, VLAN members are picked from
     * the L2_BITMAP in the corresponding MC group. Need to destroy the
     * multicast group as well. */
    if (soc_feature(unit, soc_feature_wlan)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vtab));
        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            mc_index = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_index);
            BCM_IF_ERROR_RETURN(bcm_esw_multicast_destroy(unit, group));
        }
    }
#endif

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_destroy(unit, vid));

    _BCM_VBMP_REMOVE(vlan_info[unit].bmp, vid);

    vlan_info[unit].count--;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_destroy
 * Purpose:
 *      Deallocate VLAN from StrataSwitch.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to affect.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 *      BCM_E_BADID           Cannot remove default VLAN
 *      BCM_E_NOT_FOUND VLAN ID not in use.
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_destroy(int unit, bcm_vlan_t vid)
{
    int         rv;

    VLAN_VERB(("VLAN %d: destroy vid %d\n", unit, vid));

    CHECK_INIT(unit);
    CHECK_VID(unit, vid);

    BCM_LOCK(unit);
    rv = _bcm_vlan_destroy(unit, vid);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_1st (internal)
 * Purpose:
 *      Return the first defined VLAN ID that is not the default VLAN.
 */

STATIC INLINE bcm_vlan_t
_bcm_vlan_1st(int unit)
{
    int idx;

    for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
        if ((idx !=  vlan_info[unit].defl) &&
            (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, idx))) {
            return (idx);
        }
    }
    return BCM_VLAN_INVALID;
}

/*
 * Function:
 *      bcm_vlan_destroy_all
 * Purpose:
 *      Destroy all VLANs except the default VLAN
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_destroy_all(int unit)
{
    int                 rv = BCM_E_NONE;
    bcm_vlan_t          vid;

    VLAN_VERB(("VLAN %d: destroy all VLANs\n", unit));

    CHECK_INIT(unit);

    BCM_LOCK(unit);

    while ((vid = _bcm_vlan_1st(unit)) != BCM_VLAN_INVALID) {
        if ((rv = bcm_esw_vlan_destroy(unit, vid)) < 0) {
            break;
        }
    }

    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_port_add
 * Purpose:
 *      Main part of bcm_vlan_port_add; assumes locking already done.
 */

STATIC int
_bcm_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp)
{
    pbmp_t              vlan_pbmp, vlan_ubmp;
    pbmp_t              stacked;
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    pbmp_t              l3_pbmp;
    vlan_tab_entry_t    vtab;
    int                 mc_index = 0;
#endif

    /* No such thing as untagged CPU */
    SOC_PBMP_REMOVE(ubmp, PBMP_CMIC(unit));

    /*
     * Illegal to have untagged stack ports; remove inactive ports from bitmap
     */
    SOC_PBMP_STACK_ACTIVE_GET(unit, stacked);
    SOC_PBMP_REMOVE(ubmp, stacked);
    SOC_PBMP_REMOVE(pbmp, SOC_PBMP_STACK_INACTIVE(unit));
    SOC_PBMP_REMOVE(ubmp, SOC_PBMP_STACK_INACTIVE(unit));

    /* Don't add ports that are already there */
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_port_get(unit, vid,
                                               &vlan_pbmp, &vlan_ubmp));

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    /* If virtual port switching is enabled, VLAN members are picked from
     * the L2_BITMAP in the corresponding MC group */
    if (soc_feature(unit, soc_feature_wlan)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vtab));
        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            mc_index = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
            BCM_IF_ERROR_RETURN(_bcm_tr2_multicast_ipmc_read
                (unit, mc_index, &vlan_pbmp, &l3_pbmp));
        }
    }
#endif

    SOC_PBMP_REMOVE(pbmp, vlan_pbmp);
    SOC_PBMP_REMOVE(ubmp, vlan_pbmp);

    if (SOC_PBMP_NOT_NULL(pbmp) || SOC_PBMP_NOT_NULL(ubmp)) {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_vlan_port_add(unit, vid, pbmp, ubmp));

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_wlan)) {
            if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
                SOC_PBMP_OR(vlan_pbmp, pbmp);
                BCM_IF_ERROR_RETURN(_bcm_tr2_multicast_ipmc_write
                    (unit, mc_index, vlan_pbmp, l3_pbmp, TRUE));
            }
        }
#endif
        /* do a ptable update on all ports in the vlan */
        SOC_PBMP_OR(vlan_pbmp, pbmp);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_port_add
 * Purpose:
 *      Add ports to the specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to add port to as a member.
 *      pbmp - port bitmap for members of VLAN
 *      ubmp - untagged members of VLAN
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */

int
bcm_esw_vlan_port_add(int unit, bcm_vlan_t vid, pbmp_t pbmp, pbmp_t ubmp)
{
    int                 rv = BCM_E_NONE;

    CHECK_INIT(unit);
    CHECK_VID(unit, vid);

    VLAN_VERB(("VLAN %d: port add: vid %d. pbm 0x%x. utpbm 0x%x.\n",
               unit, vid, SOC_PBMP_WORD_GET(pbmp, 0),
               SOC_PBMP_WORD_GET(ubmp, 0)));

    BCM_LOCK(unit);
    rv = _bcm_vlan_port_add(unit, vid, pbmp, ubmp);
    BCM_UNLOCK(unit);

#ifdef BCM_XGS12_SWITCH_SUPPORT
#ifdef INCLUDE_L3
        /* reconcile L3 programming with VLAN tables' state */
    if (SOC_IS_XGS12_SWITCH(unit) && soc_feature(unit, soc_feature_l3)) {
        if (SOC_PBMP_NOT_NULL(ubmp)) {
            _bcm_xgs_l3_untagged_update(unit, vid, BCM_TRUNK_INVALID, -1);
        }
    }
#endif  /* INCLUDE_L3 */
#endif  /* BCM_XGS12_SWITCH_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_port_remove
 * Purpose:
 *      Main part of bcm_vlan_port_remove; assumes locking already done.
 */

STATIC int
_bcm_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    pbmp_t              vlan_pbmp, vlan_ubmp;
    int                 rv = BCM_E_NONE;
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    pbmp_t              l3_pbmp;
    vlan_tab_entry_t    vtab;
    int                 mc_index = 0;
#endif

    /* Don't remove ports that are not there */

    BCM_LOCK(unit);

    rv = (mbcm_driver[unit]->mbcm_vlan_port_get(unit, vid, &vlan_pbmp, &vlan_ubmp));
    if (rv < 0) {
        BCM_UNLOCK(unit);
        return rv;
    }
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    /* If virtual port switching is enabled, VLAN members are picked from
     * the L2_BITMAP in the corresponding MC group */
    if (soc_feature(unit, soc_feature_wlan)) {
        rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vtab);
        if (rv < 0) {
            BCM_UNLOCK(unit);
            return rv;
        }
        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            mc_index = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
            rv = _bcm_tr2_multicast_ipmc_read(unit, mc_index, &vlan_pbmp, &l3_pbmp);
            if (rv < 0) {
                BCM_UNLOCK(unit);
                return rv;
            }
        }
    }
#endif

    SOC_PBMP_AND(pbmp, vlan_pbmp);

    rv = (mbcm_driver[unit]->mbcm_vlan_port_remove(unit, vid, pbmp));
    if (rv < 0) {
        BCM_UNLOCK(unit);
        return rv;
    }
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan)) {
        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            BCM_PBMP_REMOVE(vlan_pbmp, pbmp);
            rv = _bcm_tr2_multicast_ipmc_write(unit, mc_index, vlan_pbmp, l3_pbmp, TRUE);
            if (rv < 0) {
                BCM_UNLOCK(unit);
                return rv;
            }
        }
    }
#endif

    BCM_UNLOCK(unit);

#ifdef BCM_XGS12_SWITCH_SUPPORT
#ifdef INCLUDE_L3
        /* reconcile L3 programming with VLAN tables' state */
    if (SOC_IS_XGS12_SWITCH(unit) && soc_feature(unit, soc_feature_l3)) {
        SOC_PBMP_AND(pbmp, vlan_ubmp);
        if (SOC_PBMP_NOT_NULL(pbmp)) {
            _bcm_xgs_l3_untagged_update(unit, vid, BCM_TRUNK_INVALID, -1);
        }
    }
#endif  /* INCLUDE_L3 */
#endif  /* BCM_XGS12_SWITCH_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_port_remove
 * Purpose:
 *      Remove ports from a specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to remove port(s) from.
 *      pbmp - port bitmap for ports to remove.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_port_remove(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    int                 rv = BCM_E_NONE;

    VLAN_VERB(("VLAN %d: port remove: vid %d. pbm 0x%x.\n",
               unit, vid, SOC_PBMP_WORD_GET(pbmp, 0)));

    CHECK_INIT(unit);
    CHECK_VID(unit, vid);

    rv = _bcm_vlan_port_remove(unit, vid, pbmp);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_get
 * Purpose:
 *      Retrieves a list of the member ports of an existing VLAN.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - VLAN ID to lookup.
 *      tag_pbmp - (output) member port bitmap (ignored if NULL)
 *      untag_pbmp - (output) untagged port bitmap (ignored if NULL)
 * Returns:
 *      BCM_E_NONE - Success (port bitmaps filled in).
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - No such VLAN defined.
 */

int
bcm_esw_vlan_port_get(int unit, bcm_vlan_t vid, pbmp_t *pbmp, pbmp_t *ubmp)
{
    int                 rv;
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    pbmp_t              l3_pbmp;
    vlan_tab_entry_t    vtab;
    int                 mc_index;
#endif

    CHECK_INIT(unit);
    CHECK_VID(unit, vid);

    if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_vlan_port_get(unit, vid, pbmp, ubmp);
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    /* If virtual port switching is enabled, VLAN members are picked from
     * the L2_BITMAP in the corresponding MC group */
    if (soc_feature(unit, soc_feature_wlan)) {
        rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vid, &vtab);
        if (rv < 0) {
            BCM_UNLOCK(unit);
            return rv;
        }
        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            mc_index = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
            rv = _bcm_tr2_multicast_ipmc_read(unit, mc_index, pbmp, &l3_pbmp);
            if (rv < 0) {
                BCM_UNLOCK(unit);
                return rv;
            }
        }
    }
#endif
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_list
 * Purpose:
 *      Main body of bcm_vlan_list() and bcm_vlan_list_by_pbmp().
 *      Assumes locking already done.
 * Parameters:
 *      list_all - if TRUE, lists all ports and ignores list_pbmp.
 *      list_pbmp - if list_all is FALSE, lists only VLANs containing
 *              any of the ports in list_pbmp.
 */

STATIC int
_bcm_vlan_list(int unit, bcm_vlan_data_t **listp, int *countp,
               int list_all, pbmp_t list_pbmp)
{
    bcm_vlan_data_t     *list;
    int                 count, i, rv;
    int                 idx;
    int                 valid_count;
    pbmp_t              pbmp, ubmp, tbmp;

    *countp = 0;
    *listp = NULL;

    if (!list_all && SOC_PBMP_IS_NULL(list_pbmp)) {     /* Empty list */
        return BCM_E_NONE;
    }

    count = vlan_info[unit].count;

    if (count == 0) {
        return BCM_E_NONE;                      /* Empty list */
    }

    if ((list = sal_alloc(count * sizeof (list[0]), "vlan_list")) == NULL) {
        return BCM_E_MEMORY;
    }

    i = 0;
    valid_count = 0;

    for (idx = BCM_VLAN_MIN; idx < BCM_VLAN_COUNT; idx++) {
        if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, idx)) {
            continue;
        }

        rv = bcm_esw_vlan_port_get(unit, idx, &pbmp, &ubmp);
        if (BCM_FAILURE(rv)) {
            sal_free(list);
            return rv;
        }
        valid_count++;

        SOC_PBMP_ASSIGN(tbmp, list_pbmp);
        SOC_PBMP_AND(tbmp, pbmp);
        if (list_all || SOC_PBMP_NOT_NULL(tbmp)) {
            list[i].vlan_tag = idx;
            BCM_PBMP_ASSIGN(list[i].port_bitmap, pbmp);
            BCM_PBMP_ASSIGN(list[i].ut_port_bitmap, ubmp);
            i++;

        }
        if (valid_count == count)   {
            break;
        }
    } 
    assert(!list_all || i == count);  /* If list_all, make sure all listed */

    *countp = i;
    *listp = list;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_list
 * Purpose:
 *      Returns an array of all defined VLANs and their port bitmaps.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      listp - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero VLANs defined.
 *      countp - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero VLANs defined.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_MEMORY - Out of system memory.
 * Notes:
 *      The caller is responsible for freeing the memory that is
 *      returned, using bcm_vlan_list_destroy.
 */

int
bcm_esw_vlan_list(int unit, bcm_vlan_data_t **listp, int *countp)
{
    int         rv;
    pbmp_t      empty_pbm;

    CHECK_INIT(unit);

    SOC_PBMP_CLEAR(empty_pbm);
    BCM_LOCK(unit);
    rv = _bcm_vlan_list(unit, listp, countp, TRUE, empty_pbm);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_list_by_pbmp
 * Purpose:
 *      Returns an array of defined VLANs and port bitmaps.
 *      Only VLANs that containing any of the specified ports are listed.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      pbmp - Bitmap of ports
 *      listp - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero VLANs defined.
 *      countp - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero VLANs defined.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 *      BCM_E_MEMORY            Out of system memory.
 * Notes:
 *      The caller is responsible for freeing the memory that is
 *      returned, using bcm_vlan_list_destroy.
 */

int
bcm_esw_vlan_list_by_pbmp(int unit, pbmp_t pbmp,
                          bcm_vlan_data_t **listp, int *countp)
{
    int         rv;

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    rv = _bcm_vlan_list(unit, listp, countp, FALSE, pbmp);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_list_destroy
 * Purpose:
 *      Destroy a list returned by bcm_vlan_list.
 *      Also works for the zero-VLAN case (NULL list).
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      list - List returned by bcm_vlan_list
 *      count - Count returned by bcm_vlan_list
 * Returns:
 *      BCM_E_NONE              Success.
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_list_destroy(int unit, bcm_vlan_data_t *list, int count)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    if (list != NULL) {
        sal_free(list);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_default_get
 * Purpose:
 *      Get the default VLAN ID
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid_ptr - (OUT) Target to receive the VLAN ID.
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_esw_vlan_default_get(int unit, bcm_vlan_t *vid_ptr)
{
    *vid_ptr = vlan_info[unit].defl;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vlan_default_set
 * Purpose:
 *      Main part of bcm_vlan_default_set; assumes locking already done.
 */

STATIC int
_bcm_vlan_default_set(int unit, bcm_vlan_t vid)
{
    if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
        return BCM_E_NOT_FOUND;
    }

    vlan_info[unit].defl = vid;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_default_set
 * Purpose:
 *      Set the default VLAN ID
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid - The new default VLAN
 * Returns:
 *      BCM_E_NONE - Success.
 * Notes:
 *      The new default VLAN must already exist.
 */

int
bcm_esw_vlan_default_set(int unit, bcm_vlan_t vid)
{
    int                 rv;

    VLAN_VERB(("VLAN %d: dflt vid set: %d\n", unit, vid));

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    rv = _bcm_vlan_default_set(unit, vid);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_stg_get
 * Purpose:
 *      Retrieve the VTABLE STG for the specified vlan
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *      vid - VLAN ID.
 *      stg_ptr - (OUT) Pointer to store stgid for return.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_stg_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg_ptr)
{
    int         rv;

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_vlan_stg_get(unit, vid, stg_ptr);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_vlan_stg_set
 * Purpose:
 *      Update the VTABLE STG for the specified vlan
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number
 *      vid - VLAN ID
 *      stg - New spanning tree group number for VLAN
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg)
{
    int         rv;

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    rv = bcm_esw_stg_vlan_add(unit, stg, vid);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_stp_set
 * Purpose:
 *      Main part of bcm_vlan_stp_set; assumes locking already done.
 */

STATIC int
_bcm_vlan_stp_set(int unit, bcm_vlan_t vid, bcm_port_t port, int stp_state)
{
    bcm_stg_t   stgid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_stg_get(unit, vid, &stgid));
    BCM_IF_ERROR_RETURN(bcm_esw_stg_stp_set(unit, stgid, port, stp_state));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_stp_set
 * Purpose:
 *      Set the spanning tree state for a port in the whole spanning
 *      tree group that contains the specified VLAN.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number
 *      vid - VLAN ID
 *      port - Port
 *      stp_state - State to set
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_stp_set(int unit, bcm_vlan_t vid, bcm_port_t port, int stp_state)
{
    int         rv;
    if (BCM_GPORT_IS_SET(port)) {
        bcm_trunk_t     tgid;
        bcm_module_t    modid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid) ) {
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }

    BCM_LOCK(unit);
    rv = _bcm_vlan_stp_set(unit, vid, port, stp_state);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_vlan_stp_get
 * Purpose:
 *      Main part of bcm_vlan_stp_get; assumes locking already done.
 */

STATIC int
_bcm_vlan_stp_get(int unit, bcm_vlan_t vid, bcm_port_t port, int *stp_state)
{
    bcm_stg_t   stgid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_stg_get(unit, vid, &stgid));
    BCM_IF_ERROR_RETURN(bcm_esw_stg_stp_get(unit, stgid, port, stp_state));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_stp_get
 * Purpose:
 *      Get the spanning tree state for a port in the whole spanning
 *      tree group that contains the specified VLAN.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *      vid - VLAN ID.
 *      port - Port
 *      stp_state - (OUT) State to return.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_stp_get(int unit, bcm_vlan_t vid, bcm_port_t port, int *stp_state)
{
    int         rv;

    if (BCM_GPORT_IS_SET(port)) {
        bcm_trunk_t     tgid;
        bcm_module_t    modid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid) ) {
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }

    BCM_LOCK(unit);
    rv = _bcm_vlan_stp_get(unit, vid, port, stp_state);
    BCM_UNLOCK(unit);

    return rv;
}

/*
 * Port-based VLAN actions
 */
/* 
 * Function:
 *      bcm_vlan_port_default_action_set
 * Purpose: 
 *      Set the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_port_default_action_set(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{   
    int rv = BCM_E_UNAVAIL;
    
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid) ) {
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
        soc_mem_lock(unit, VLAN_PROTOCOL_DATAm);
        rv = _bcm_trx_vlan_port_default_action_set(unit, port, action);
        soc_mem_unlock(unit, VLAN_PROTOCOL_DATAm);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*  
 * Function:
 *      bcm_vlan_port_default_action_get
 * Purpose:
 *      Get the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_port_default_action_get(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid) ) {
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
        soc_mem_lock(unit, VLAN_PROTOCOL_DATAm);
        rv = _bcm_trx_vlan_port_default_action_get(unit, port, action);
        soc_mem_unlock(unit, VLAN_PROTOCOL_DATAm);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    return rv;
}


/*  
 * Function:
 *      bcm_vlan_port_default_action_delete
 * Purpose:
 *      Delete the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_port_default_action_delete(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid) ) {
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
        soc_mem_lock(unit, VLAN_PROTOCOL_DATAm);
        rv = _bcm_trx_vlan_port_default_action_delete(unit, port);
        soc_mem_unlock(unit, VLAN_PROTOCOL_DATAm);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_set
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_port_egress_default_action_set(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid) ) {
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
        rv = _bcm_trx_vlan_port_egress_default_action_set(unit, port, action);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_get
 * Purpose:
 *      Get the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */     
int
bcm_esw_vlan_port_egress_default_action_get(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;
 
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid) ) {
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
        rv = _bcm_trx_vlan_port_egress_default_action_get(unit, port, action);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_vlan_port_egress_default_action_delete
 * Purpose:
 *      delete the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */     
int
bcm_esw_vlan_port_egress_default_action_delete(int unit, bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL;
 
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid) ) {
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
        rv = _bcm_trx_vlan_port_egress_default_action_delete(unit, port);
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

/*
 * Protocol-based VLAN actions
 */

/*
 * Function   :
 *      bcm_vlan_port_protocol_action_add
 * Description   :
 *      Add protocol based VLAN with specified action.
 *      If the entry already exists, update the action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 *      action    (IN) Action for outer tag and inner tag
 * Note:
 *    Program VLAN_PROTOCOL_DATAm and VLAN_PROTOCOLm.
 */
int
bcm_esw_vlan_port_protocol_action_add(int unit,
                                      bcm_port_t port,
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_PROTOCOL_DATAm);
        rv =  _bcm_trx_vlan_port_protocol_action_add(unit, port, frame,
                                                     ether, action);
        soc_mem_unlock(unit, VLAN_PROTOCOL_DATAm);
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}
/*
 * Function   :
 *      bcm_vlan_port_protocol_action_get
 * Description   :
 *      Get protocol based VLAN with specified action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 *      action    (OUT) Action for outer and inner tag
 * Note:
 *    
 */
int
bcm_esw_vlan_port_protocol_action_get(int unit,
                                      bcm_port_t port,
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_PROTOCOL_DATAm);
        rv = _bcm_trx_vlan_port_protocol_action_get(unit, port, frame,
                                                     ether, action);
        soc_mem_unlock(unit, VLAN_PROTOCOL_DATAm);
        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return (rv);
}


/*
 * Function   :
 *      bcm_vlan_port_protocol_action_delete
 * Description   :
 *      Delete protocol based VLAN action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 * Note:
 *    Program VLAN_PROTOCOL_DATAm and VLAN_PROTOCOLm.
 */
int
bcm_esw_vlan_port_protocol_action_delete(int unit,bcm_port_t port,
                                         bcm_port_frametype_t frame,
                                         bcm_port_ethertype_t ether)
{
    int rv = BCM_E_UNAVAIL; 
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_PROTOCOL_DATAm);
        rv = _bcm_trx_vlan_port_protocol_delete(unit, port, frame, ether);
        soc_mem_unlock(unit, VLAN_PROTOCOL_DATAm);
        return rv;
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}


/*
 * Function   :
 *      bcm_vlan_port_protocol_action_delete_all
 * Description   :
 *      Delete all protocol based VLAN actiona.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 * Note:
 *    Program VLAN_PROTOCOL_DATAm and VLAN_PROTOCOLm.
 */
int
bcm_esw_vlan_port_protocol_action_delete_all(int unit,bcm_port_t port)
{
    int rv = BCM_E_UNAVAIL; 

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_PROTOCOL_DATAm);
        rv = _bcm_trx_vlan_port_protocol_delete_all(unit, port);
        soc_mem_unlock(unit, VLAN_PROTOCOL_DATAm);
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}

/*
 * Function   :
 *      bcm_esw_vlan_port_protocol_action_traverse
 * Description   :
 *      Traverse over vlan port protocol actions. 
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      cb        (IN) User provided call back function
 *      user_data (IN) User provided data
 * Note:
 *    Program VLAN_PROTOCOL_DATAm and VLAN_PROTOCOLm.
 */
int
bcm_esw_vlan_port_protocol_action_traverse(int unit, 
                                bcm_vlan_port_protocol_action_traverse_cb cb,
                                           void *user_data)
{
    int rv = BCM_E_UNAVAIL; 
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_PROTOCOL_DATAm);
        rv = _bcm_trx_vlan_port_protocol_action_traverse(unit, cb, user_data);
        soc_mem_unlock(unit, VLAN_PROTOCOL_DATAm);
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}


/*
 * Mac based vlans
 */
int
bcm_esw_vlan_mac_add(int unit, bcm_mac_t mac, bcm_vlan_t vid, int prio)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        prio &= ~BCM_PRIO_DROP_FIRST;

        if ((prio & ~BCM_VLAN_XLATE_PRIO_MASK) != 0) {
            return BCM_E_PARAM;
        }

        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = vid;
        action.new_inner_vlan = 0;
        action.priority = prio;
        action.dt_outer_prio = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.it_outer      = bcmVlanActionAdd;
        action.it_inner_prio = bcmVlanActionDelete;
        action.ut_outer      = bcmVlanActionAdd;

        return _bcm_trx_vlan_mac_action_add(unit, mac, &action);
    }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        vlan_mac_entry_entry_t  vment;
        int                     cng, vt_mode;

        bcm_esw_vlan_control_get(unit, bcmVlanTranslate, &vt_mode);
        if (vt_mode) {
            return BCM_E_CONFIG; /* Not in MAC based VLAN mode */
        }

        cng = 0;
        if (prio & BCM_PRIO_DROP_FIRST) {
            cng = 1;
            prio &= ~BCM_PRIO_DROP_FIRST;
        }
        if ((prio & ~BCM_PRIO_MASK) != 0) {
            return BCM_E_PARAM;
        }

        sal_memset(&vment, 0, sizeof(vment));

        soc_VLAN_MAC_ENTRYm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
        soc_VLAN_MAC_ENTRYm_field32_set(unit, &vment, VLAN_IDf, vid);
        soc_VLAN_MAC_ENTRYm_field32_set(unit, &vment, PRIf, prio);
        soc_VLAN_MAC_ENTRYm_field32_set(unit, &vment, CNGf, cng);
        soc_VLAN_MAC_ENTRYm_field32_set(unit, &vment, VALID_BITf, 1);

        return soc_mem_insert(unit, VLAN_MAC_ENTRYm, MEM_BLOCK_ALL, &vment);
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        vlan_mac_entry_entry_t  vment;
        prio &= ~BCM_PRIO_DROP_FIRST;

        if ((prio & ~BCM_VLAN_XLATE_PRIO_MASK) != 0) {
            return BCM_E_PARAM;
        }

        sal_memset(&vment, 0, sizeof(vment));
        soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
        soc_VLAN_MACm_field32_set(unit, &vment, VLAN_IDf, vid);
        soc_VLAN_MACm_field32_set(unit, &vment, PRIf, prio);
        soc_VLAN_MACm_field32_set(unit, &vment, VALIDf, 1);

        return soc_mem_insert(unit, VLAN_MACm, MEM_BLOCK_ALL, &vment);
    }
#endif
    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_mac_delete(int unit, bcm_mac_t mac)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_mac_delete(unit, mac);
    }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        int     vt_mode;

        bcm_esw_vlan_control_get(unit, bcmVlanTranslate, &vt_mode);
        if (vt_mode) {
            return BCM_E_CONFIG; /* Not in MAC based VLAN mode */
        }

        return soc_mem_delete(unit, VLAN_MAC_ENTRYm, MEM_BLOCK_ALL,
                              (void *)mac);
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        vlan_mac_entry_entry_t  vment;
        int     rv;

        sal_memset(&vment, 0, sizeof(vment));
        soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
        rv = soc_mem_delete(unit, VLAN_MACm, MEM_BLOCK_ALL, &vment);
        if (rv == SOC_E_NOT_FOUND) {
            rv = SOC_E_NONE;
        }
        return rv;
    }
#endif
    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_mac_delete_all(int unit)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_mac_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        int     vt_mode;

        bcm_esw_vlan_control_get(unit, bcmVlanTranslate, &vt_mode);
        if (vt_mode) {
            return BCM_E_CONFIG; /* Not in MAC based VLAN mode */
        }

        return soc_mem_clear(unit, VLAN_MAC_VALIDm, MEM_BLOCK_ALL, 0);
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return soc_mem_clear(unit, VLAN_MACm, MEM_BLOCK_ALL, 0);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_mac_action_add
 * Purpose:
 *     Add an association from MAC address to VLAN actions, which are
 *             used for VLAN tag/s assignment to untagged packets.
 *      
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      mac     - (IN) Mac address. 
 *      action    (IN) Action for outer and inner tag
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_mac_action_add(int unit, bcm_mac_t mac, bcm_vlan_action_set_t *action)
{
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_mac_action_add(unit, mac, action);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_mac_action_get
 * Purpose:
 *    Retrive an association from MAC address to VLAN actions, which
 *    are used for VLAN tag assignment to untagged packets.
 *      
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      mac     - (IN) Mac address. 
 *      action    (OUT) Action for outer and inner tag
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_mac_action_get(int unit, bcm_mac_t mac, bcm_vlan_action_set_t *action)
{
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_mac_action_get(unit, mac, action);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_mac_action_delete
 * Purpose:
 *    Remove an association from MAC address to VLAN actions, which
 *    are used for VLAN tag assignment to untagged packets.
 *      
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      mac     - (IN) Mac address. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_mac_action_delete(int unit, bcm_mac_t mac)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_mac_delete(unit, mac);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_mac_action_delete_all
 * Purpose:
 *    Remove all MAC addresses to VLAN actions associations.
 *    Mac to VLAN actions are used for VLAN tag assignment to untagged packets.
 *      
 * Parameters:
 *      unit    - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_mac_action_delete_all(int unit)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_mac_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_mac_action_traverse
 * Description   :
 *      Traverse over vlan mac actions, which are used for VLAN
 *      tag/s assignment to untagged packets.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
bcm_esw_vlan_mac_action_traverse(int unit, 
                                 bcm_vlan_mac_action_traverse_cb cb, 
                                 void *user_data)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_mac_action_traverse(unit, cb, user_data);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

#ifdef BCM_FIREBOLT_SUPPORT
#ifdef BCM_WARM_BOOT_SUPPORT 
STATIC int
_bcm_fb_vlan_translate_reload(int unit, int xtable)
{
    soc_mem_t           mem;
    int                 addvid, index_min, index_max, index_count, i, rv;
    uint32              *vtcachep, vtcache;
    vlan_xlate_entry_t  *vtentries, *vtent;
    uint32              ve_valid, ve_port, ve_vid, ve_add;

    if (!soc_feature(unit, soc_feature_vlan_translation)) {
        return BCM_E_NONE;
    }

    switch (xtable) {
    case BCM_VLAN_XLATE_ING:
        mem = VLAN_XLATEm;
        addvid = 0;
        break;
    case BCM_VLAN_XLATE_EGR:
        mem = EGR_VLAN_XLATEm;
        addvid = -1;
        break;
    case BCM_VLAN_XLATE_DTAG:
        mem = VLAN_XLATEm;
        addvid = 1;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);
    index_count = soc_mem_index_count(unit, mem);

    soc_mem_lock(unit, mem);

    /* get vt cache, allocate if necessary */
    if (mem == EGR_VLAN_XLATEm) {
        vtcachep = vlan_info[unit].egr_trans;
    } else {
        vtcachep = vlan_info[unit].ing_trans;
    }
    if (vtcachep == NULL) {
        vtcachep = sal_alloc(sizeof(*vtcachep) * (index_max+1),
                             "vlan trans cache");
        if (vtcachep == NULL) {
            soc_mem_unlock(unit, mem);
            return BCM_E_MEMORY;
        }
        sal_memset(vtcachep, 0, sizeof(*vtcachep) * (index_max+1));
        if (mem == EGR_VLAN_XLATEm) {
            vlan_info[unit].egr_trans = vtcachep;
        } else {
            vlan_info[unit].ing_trans = vtcachep;
        }
    }

    vtentries = soc_cm_salloc(unit, index_count * sizeof(*vtentries),
                              "vlan trans dma");
    if (vtentries == NULL) {
        soc_mem_unlock(unit, mem);
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max,
                            vtentries);
    if (rv < 0) {
        soc_cm_sfree(unit, vtentries);
        sal_free(vtcachep);
        if (mem == EGR_VLAN_XLATEm) {
            vlan_info[unit].egr_trans = NULL;
        } else {
            vlan_info[unit].ing_trans = NULL;
        }
        soc_mem_unlock(unit, mem);
        return rv;
    }
    for (i = 0; i < index_count; i++) {
        vtent = soc_mem_table_idx_to_pointer(unit, mem, vlan_xlate_entry_t *,
                                             vtentries, i);
        vtcache = 0;
        ve_valid = soc_mem_field32_get(unit, mem, vtent, VALIDf);
        if (ve_valid) {
            BCM_VTCACHE_VALID_SET(vtcache, 1);
            ve_port = soc_mem_field32_get(unit, mem, vtent, PORTf);
            BCM_VTCACHE_PORT_SET(vtcache, ve_port);
            ve_vid = soc_mem_field32_get(unit, mem, vtent, OLD_VLAN_IDf);
            BCM_VTCACHE_VID_SET(vtcache, ve_vid);
            if (addvid >= 0) {
                ve_add = soc_mem_field32_get(unit, mem, vtent, ADD_VIDf);
                BCM_VTCACHE_ADD_SET(vtcache, ve_add);
            }
        }
        vtcachep[index_min + i] = vtcache;
    }
    soc_cm_sfree(unit, vtentries);
    soc_mem_unlock(unit, mem);
    return BCM_E_NONE;
}
#else
#define _bcm_fb_vlan_translate_reload(unit, xtable)    (BCM_E_NONE)
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *      bcm_esw_vlan_translate_get
 * Purpose:
 *      Get vlan translation
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */

int 
bcm_esw_vlan_translate_get (int unit, bcm_port_t port, bcm_vlan_t old_vid,
                            bcm_vlan_t *new_vid, int *prio)
{

    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);
    if (NULL == new_vid) {
        return BCM_E_PARAM;
    }
    if (NULL == prio) {
        return BCM_E_PARAM;
    }
    
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_gport_t gport;
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }
        bcm_vlan_action_set_t_init(&action);

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_get(unit, gport,
                                               bcmVlanTranslateKeyPortOuter,
                                               old_vid, 0, &action));

        if (bcmVlanActionReplace == action.ot_outer){
            *new_vid = action.new_outer_vlan;
            *prio = action.priority;
            return BCM_E_NONE;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_get(unit, gport, 
                                               bcmVlanTranslateKeyPortInner,
                                               0, old_vid, &action));
        if (bcmVlanActionAdd == action.it_outer) {
            *new_vid = action.new_inner_vlan;
            *prio = action.priority;
            return BCM_E_NONE;
        }

        return BCM_E_NOT_FOUND;
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_d15_vlan_translate_get(unit, port, old_vid, new_vid, prio);
    }
#endif /* BCM_DRACO15_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_get(unit, port, old_vid, new_vid, prio,
                                          BCM_VLAN_XLATE_ING);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_get(unit, port, modid, tgid, old_vid,
                                         new_vid, prio, BCM_VLAN_XLATE_ING);
    }
#endif /* BCM_EASYRIDER_SUPPORT */

    return BCM_E_UNAVAIL;
}



/*
 * Function:
 *      bcm_esw_vlan_translate_add
 * Purpose:
 *      Add vlan translation
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int
bcm_esw_vlan_translate_add(int unit, int port, bcm_vlan_t old_vid,
                           bcm_vlan_t new_vid, int prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1; 

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);
    CHECK_VID(unit, new_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;
        bcm_gport_t gport;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) { 
                return BCM_E_PORT; 
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }

        /* add an entry for inner-tagged packets */
        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = new_vid;
        action.priority = prio;
        action.it_outer = bcmVlanActionAdd;
        action.it_inner = bcmVlanActionDelete;
        action.it_inner_prio = bcmVlanActionDelete;

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_add(unit, gport, 
                                               bcmVlanTranslateKeyPortInner,
                                               0, old_vid, &action));

        /* add an entry for outer-tagged packets */
        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = new_vid;
        action.priority = prio;
        action.ot_outer      = bcmVlanActionReplace;
        action.ot_outer_prio = bcmVlanActionReplace;
        action.dt_outer      = bcmVlanActionReplace;
        action.dt_outer_prio = bcmVlanActionReplace;

        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_add(unit, gport,
                                               bcmVlanTranslateKeyPortOuter,
                                               old_vid, 0, &action));
        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_d15_vlan_translate_add(unit, port, old_vid, new_vid, prio);
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_add(unit, port, old_vid, new_vid, prio,
                                          BCM_VLAN_XLATE_ING);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_add(unit, port, modid, tgid, old_vid,
                                         new_vid, prio, BCM_VLAN_XLATE_ING);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_translate_delete
 * Purpose:
 *      Delete vlan translation
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to delete translation for
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */

int
bcm_esw_vlan_translate_delete(int unit, int port, bcm_vlan_t old_vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_gport_t gport;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) { 
                return BCM_E_PORT; 
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_delete(unit, gport, 
                                                  bcmVlanTranslateKeyPortInner,
                                                  0, old_vid));
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_translate_action_delete(unit, gport, 
                                                  bcmVlanTranslateKeyPortOuter,
                                                  old_vid, 0));
        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_d15_vlan_translate_delete(unit, port, old_vid);
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, port, old_vid,
                                             BCM_VLAN_XLATE_ING);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_delete(unit, port, modid, tgid, old_vid,
                                            BCM_VLAN_XLATE_ING);
    }
#endif

    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_translate_delete_all(int unit)
{
    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_translate_action_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_d15_vlan_translate_delete(unit, -1, BCM_VLAN_NONE);
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, -1, BCM_VLAN_NONE,
                                             BCM_VLAN_XLATE_ING);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_delete_all(unit, BCM_VLAN_XLATE_ING);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_action_add
 * Description   :
 *      Add an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int
bcm_esw_vlan_translate_action_add(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_add(unit, port, 
                                                 key_type, outer_vlan,
                                                 inner_vlan, action);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_action_get
 * Description   :
 *      Get an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (OUT) Action for outer and inner tag
 */
int 
bcm_esw_vlan_translate_action_get (int unit, bcm_gport_t port,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_get(unit, port, key_type, 
                                                 outer_vlan, inner_vlan, 
                                                 action);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}


/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 *      entries in Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_action_traverse_int_cb(int unit, void *trv_info, 
                                               int *stop)
{
    _bcm_vlan_translate_traverse_t          *trvs_str;
    _translate_action_traverse_cb_t   *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_action_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, trvs_str->key_type,
                             trvs_str->outer_vlan, trvs_str->inner_vlan,
                             trvs_str->action, trvs_str->user_data);  
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_egress_action_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 *      entries in Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_egress_action_traverse_int_cb(int unit, void *trv_info,
                                                      int *stop)
{
    _bcm_vlan_translate_traverse_t              *trvs_str;
    _translate_egress_action_traverse_cb_t      *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_egress_action_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;

    return usr_cb_st->usr_cb(unit, trvs_str->port_class,
                             trvs_str->outer_vlan, 
                             trvs_str->inner_vlan, 
                             trvs_str->action,
                             trvs_str->user_data);
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 *      entries in Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_action_range_traverse_int_cb(int unit, void *trv_info,
                                                     int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _translate_action_range_traverse_cb_t *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_action_range_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, trvs_str->outer_vlan, 
                             trvs_str->outer_vlan_high, trvs_str->inner_vlan, 
                             trvs_str->inner_vlan_high, trvs_str->action, 
                             trvs_str->user_data);  
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 *      entries and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_range_traverse_int_cb(int unit, void *trv_info,
                                                     int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _translate_range_traverse_cb_t *usr_cb_st;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_range_traverse_cb_t *)trvs_str->user_cb_st;

    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, trvs_str->outer_vlan, 
                             trvs_str->outer_vlan_high, 
                             trvs_str->action->new_outer_vlan,
                             trvs_str->action->priority, trvs_str->user_data);  
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_traverse_int_cb(int unit, void *trv_info, int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _translate_traverse_cb_t        *usr_cb_st;
    bcm_vlan_t                      old_vlan, new_vlan;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_traverse_cb_t *)trvs_str->user_cb_st;
    if (BCM_VLAN_INVALID != trvs_str->outer_vlan) {
        old_vlan = trvs_str->outer_vlan;
    } else {
        old_vlan = trvs_str->inner_vlan;
    }

    if (BCM_VLAN_INVALID != trvs_str->action->new_outer_vlan) {
        new_vlan = trvs_str->action->new_outer_vlan;
    } else {
        new_vlan = trvs_str->action->new_inner_vlan;
    }

    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, old_vlan, new_vlan, 
                             trvs_str->action->priority, trvs_str->user_data); 
}


/*
 * Function   :
 *      _bcm_esw_vlan_translate_egress_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_translate_egress_traverse_int_cb(int unit, void *trv_info, 
                                               int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _translate_egress_traverse_cb_t *usr_cb_st;
    bcm_vlan_t                      old_vlan, new_vlan;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_translate_egress_traverse_cb_t *)trvs_str->user_cb_st;
    if (BCM_VLAN_INVALID != trvs_str->outer_vlan) {
        old_vlan = trvs_str->outer_vlan;
    } else {
        old_vlan = trvs_str->inner_vlan;
    }

    if (BCM_VLAN_INVALID != trvs_str->action->new_outer_vlan) {
        new_vlan = trvs_str->action->new_outer_vlan;
    } else {
        new_vlan = trvs_str->action->new_inner_vlan;
    }

    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, old_vlan, new_vlan, 
                             trvs_str->action->priority, trvs_str->user_data); 
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_egress_traverse_int_cb
 * Description   :
 *      Internal call back function to call user provided call back 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN) Traverse structure that contain all relevant info
 */
int 
_bcm_esw_vlan_dtag_traverse_int_cb(int unit, void *trv_info, int *stop)
{
    _bcm_vlan_translate_traverse_t  *trvs_str;
    _dtag_traverse_cb_t             *usr_cb_st;
    bcm_vlan_t                      old_vlan, new_vlan;

    if ((NULL == trv_info) || (NULL == stop)){
        return BCM_E_PARAM;
    }
    trvs_str = (_bcm_vlan_translate_traverse_t *)trv_info;
    usr_cb_st = (_dtag_traverse_cb_t *)trvs_str->user_cb_st;
    if (BCM_VLAN_INVALID != trvs_str->outer_vlan) {
        old_vlan = trvs_str->outer_vlan;
    } else {
        old_vlan = trvs_str->inner_vlan;
    }

    if (BCM_VLAN_INVALID != trvs_str->action->new_outer_vlan) {
        new_vlan = trvs_str->action->new_outer_vlan;
    } else {
        new_vlan = trvs_str->action->new_inner_vlan;
    }
    
    *stop = FALSE;
    return usr_cb_st->usr_cb(unit, trvs_str->gport, old_vlan, new_vlan, 
                             trvs_str->action->priority, trvs_str->user_data); 
}

/*
 * Function:
 *     _bcm_esw_vlan_translate_entry_valid
 * Description:
 *      Check if given Vlan Translate entry is Valid
 * Parameters:
 *      unit         device number
 *      mem          memory to operate on
 *      vent         entry read from HW
 *      valid       (OUT) Entry valid indicator
 * Return:
 *     BCM_E_XXX
 */

STATIC int
_bcm_esw_vlan_translate_entry_valid(int unit, soc_mem_t mem, uint32 *vent, int *valid)
{
    uint32      fval = 0;

    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }

    if ((NULL == vent) || (NULL == valid)) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TRX_SUPPORT) || defined (BCM_FIREBOLT_SUPPORT)
            if (SOC_IS_FBX(unit)) {
                fval = soc_mem_field32_get(unit, mem, vent, VALIDf);
            } 
#endif /* BCM_TRX_SUPPORT || BCM_FIREBOLT_SUPPORT */
#if defined(BCM_EASYRIDER_SUPPORT)
            if (SOC_IS_EASYRIDER(unit)) {
                fval = BCM_VLAN_VALID(soc_mem_field32_get(unit, mem, vent, 
                                                    OLD_VLAN_IDf)) ? 1 : 0;
            }
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_DRACO15_SUPPORT)
            if (SOC_IS_DRACO15(unit)) {
                fval = soc_mem_field32_get(unit, mem, vent, VALID_BITf);
            }
#endif /* BCM_EASYRIDER_SUPPORT */


    *valid = fval ? TRUE : FALSE;

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_esw_vlan_translate_entry_parse
 * Description:
 *      Parser function to parse vlan translate entry into truverse structure
 *      will call per chip parser
 * Parameters:
 *      unit         device number
 *      mem          memory to operate on
 *      vent        entry read from HW
 *      trvs_info    Structure to fill 
 * Return:
 *     BCM_E_XXX
 */

STATIC int
_bcm_esw_vlan_translate_entry_parse(int unit, soc_mem_t mem, uint32 *vent, 
                                    _bcm_vlan_translate_traverse_t *trvs_info)
{
#ifdef BCM_TRX_SUPPORT

    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_vlan_translate_parse(unit, mem, vent, trvs_info);
    }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_parse(unit, mem, vent, trvs_info);
    } 
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return _bcm_er_vlan_translate_parse(unit, mem, vent, trvs_info);
    }
#endif /* BCM_EASYRIDER_SUPPORT */
#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        return _bcm_d15_vlan_translate_parse(unit, mem, vent, trvs_info);
    }
#endif /* BCM_EASYRIDER_SUPPORT */

    return (BCM_E_UNAVAIL);
}

/*
 * Function   :
 *      _bcm_esw_vlan_translate_traverse_mem
 * Description   :
 *      Traverse over all translate entries and call parse function 
 *      based on the unit
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
_bcm_esw_vlan_translate_traverse_mem(int unit, soc_mem_t mem, 
                                   _bcm_vlan_translate_traverse_t *trvs_info)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32          *vt_tbl_chnk;
    uint32          *vent;
    int             valid, stop, rv = BCM_E_NONE;
    
    if (INVALIDm == mem) {
        return BCM_E_MEMORY;
    }
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_VLANDELETE_CHUNKS,
                                 VLAN_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    vt_tbl_chnk = soc_cm_salloc(unit, buf_size, "vlan translate traverse");
    if (NULL == vt_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)vt_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) <= mem_idx_max) ? 
            chnk_idx + chunksize : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, vt_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx < chnk_end; ent_idx ++) {
            vent = 
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                                             vt_tbl_chnk, ent_idx);
            rv = _bcm_esw_vlan_translate_entry_valid(unit, mem, vent, &valid);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (!valid) {
                continue;
            }
            rv = _bcm_esw_vlan_translate_entry_parse(unit, mem, vent, trvs_info);
            if (BCM_FAILURE(rv)) {
                break;
            }
            rv = trvs_info->int_cb(unit, trvs_info, &stop);
            if (BCM_FAILURE(rv)) {
                break;
            }
            /* 
               Only get cb function that will use internal traverse should mark
               stop condition as true upon a match 
             */
            if (stop) {     
                break;
            }
        }
    }
    soc_cm_sfree(unit, vt_tbl_chnk);
    return rv;        
}



/*
 * Function   :
 *      bcm_vlan_translate_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_esw_vlan_translate_traverse(int unit, bcm_vlan_translate_traverse_cb cb, 
                            void *user_data)
{

        
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    _translate_traverse_cb_t            usr_cb_st;
    soc_mem_t                           mem = INVALIDm;

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    trvs_st.int_cb = _bcm_esw_vlan_translate_traverse_int_cb;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        mem = VLAN_XLATEm;
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        mem = VLAN_XLATEm;
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        mem = IGR_VLAN_XLATEm;
    }
#endif /* BCM_EASYRIDER_SUPPORT */

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        mem = VLAN_MAC_ENTRYm;
    }
#endif /* BCM_DRACO15_SUPPORT */

    if (INVALIDm == mem) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
}


/*
 * Function   :
 *      bcm_vlan_translate_egress_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_esw_vlan_translate_egress_traverse(int unit, 
                                   bcm_vlan_translate_egress_traverse_cb cb, 
                                   void *user_data)
{

        
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    _translate_egress_traverse_cb_t     usr_cb_st;
    soc_mem_t                           mem = INVALIDm;

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_egress_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    trvs_st.int_cb = _bcm_esw_vlan_translate_egress_traverse_int_cb;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        mem = EGR_VLAN_XLATEm;
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        mem = EGR_VLAN_XLATEm;
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        mem = EGR_VLAN_XLATEm;
    }
#endif /* BCM_EASYRIDER_SUPPORT */

    if (INVALIDm == mem) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_traverse
 * Description   :
 *      Traverse over all translate entries and call given call back with 
 *      new vid and prio.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int
bcm_esw_vlan_dtag_traverse(int unit, 
                       bcm_vlan_dtag_traverse_cb cb, 
                       void *user_data)
{
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    _dtag_traverse_cb_t                 usr_cb_st;
    soc_mem_t                           mem = INVALIDm;

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_dtag_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    trvs_st.int_cb = _bcm_esw_vlan_dtag_traverse_int_cb;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        mem = VLAN_XLATEm;
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        mem = VLAN_XLATEm;
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        mem = IGR_VLAN_XLATEm;
    }
#endif /* BCM_EASYRIDER_SUPPORT */

    if (INVALIDm == mem) {
        return BCM_E_UNAVAIL;
    }
    return _bcm_esw_vlan_translate_traverse_mem(unit, mem, &trvs_st);
}

/*
 * Function   :
 *      bcm_vlan_translate_action_traverse
 * Description   :
 *      Traverse over all translate entries and call given callback with 
 *      action structure
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
bcm_esw_vlan_translate_action_traverse(int unit, 
                                   bcm_vlan_translate_action_traverse_cb cb, 
                                   void *user_data)
{
    _bcm_vlan_translate_traverse_t      trvs_st;
    bcm_vlan_action_set_t               action;
    _translate_action_traverse_cb_t     usr_cb_st;

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_action_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        trvs_st.int_cb = _bcm_esw_vlan_translate_action_traverse_int_cb;
        return _bcm_esw_vlan_translate_traverse_mem(unit, VLAN_XLATEm, &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_action_traverse
 * Description   :
 *      Traverse over all translate entries and call given callback with 
 *      action structure
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 */
int 
bcm_esw_vlan_translate_egress_action_traverse(int unit, 
                            bcm_vlan_translate_egress_action_traverse_cb cb, 
                            void *user_data)
{
    _bcm_vlan_translate_traverse_t          trvs_st;
    bcm_vlan_action_set_t                   action;
    _translate_egress_action_traverse_cb_t  usr_cb_st;

    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_egress_action_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        trvs_st.int_cb = _bcm_esw_vlan_translate_egress_action_traverse_int_cb;
        return _bcm_esw_vlan_translate_traverse_mem(unit, EGR_VLAN_XLATEm, 
                                                    &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_vlan_translate_action_delete
 * Purpose:
 *      Delete a vlan translate lookup entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port            (IN) Generic port 
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int
bcm_esw_vlan_translate_action_delete(int unit,
                                     bcm_gport_t port,
                                     bcm_vlan_translate_key_t key_type,
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan)
{
    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_delete(unit, port, 
                                                    key_type, outer_vlan,
                                                    inner_vlan);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_translate_range_add(int unit, int port, bcm_vlan_t old_vid_low,
                                 bcm_vlan_t old_vid_high, bcm_vlan_t new_vid,
                                 int int_prio)
{
    bcm_gport_t     gport;
    
    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid_low);
    CHECK_VID(unit, old_vid_high);
    CHECK_VID(unit, new_vid);
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            bcm_vlan_action_set_t action;
            
            /* add vlan range and translate entry for inner-tagged packets */
            bcm_vlan_action_set_t_init(&action);
            action.new_outer_vlan = new_vid;
            action.priority = int_prio;
            action.it_outer = bcmVlanActionAdd;
            action.it_inner = bcmVlanActionDelete;
            action.it_inner_prio = bcmVlanActionDelete;

            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_add(unit, gport,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID, 
                                        old_vid_low, old_vid_high, 
                                        &action));

            /* add vlan translate entry for outer-tagged packets */
            bcm_vlan_action_set_t_init(&action);
            action.new_outer_vlan = new_vid;
            action.priority      = int_prio;
            action.ot_outer      = bcmVlanActionReplace;
            action.ot_outer_prio = bcmVlanActionReplace;
            action.dt_outer      = bcmVlanActionReplace;
            action.dt_outer_prio = bcmVlanActionReplace;
 
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_add(unit, gport,
                                        bcmVlanTranslateKeyPortOuter,
                                        old_vid_low, BCM_VLAN_INVALID,
                                        &action));
            return BCM_E_NONE;
        }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            return bcm_er_vlan_translate_range_add(unit, gport, old_vid_low,
                                                 old_vid_high, new_vid, int_prio,
                                                 BCM_VLAN_XLATE_ING);
        }
#endif
    }

    return BCM_E_UNAVAIL;
}

int bcm_esw_vlan_translate_range_get (int unit, bcm_port_t port,
                                      bcm_vlan_t old_vlan_low,
                                      bcm_vlan_t old_vlan_high,
                                      bcm_vlan_t *new_vid, int *prio)
{
    bcm_gport_t     gport;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vlan_low);
    CHECK_VID(unit, old_vlan_high);

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if ((NULL == new_vid) || NULL == prio ){
        return BCM_E_PARAM;
    }
    if (old_vlan_high < old_vlan_low) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            return bcm_er_vlan_translate_range_get(unit, gport, old_vlan_low,
                                                 old_vlan_high, new_vid, prio,
                                                 BCM_VLAN_XLATE_ING);
        }
#endif
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            bcm_vlan_action_set_t action;
            
            bcm_vlan_action_set_t_init(&action);
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_get(unit, gport,
                                        old_vlan_low, old_vlan_high, 
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                        &action));
            if (BCM_VLAN_INVALID != action.new_outer_vlan) {
                *new_vid = action.new_outer_vlan;
                *prio = action.priority;
                return BCM_E_NONE;
            } 

            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_get(unit, gport,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                        old_vlan_low, old_vlan_high, 
                                        &action));

            if (BCM_VLAN_INVALID != action.new_inner_vlan) {
                *new_vid = action.new_inner_vlan;
                *prio = action.priority;
                return BCM_E_NONE;
            }
        
            return BCM_E_NOT_FOUND;
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_translate_range_delete(int unit, int port,
                                    bcm_vlan_t old_vid_low,
                                    bcm_vlan_t old_vid_high)
{
    bcm_gport_t gport;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid_low);
    CHECK_VID(unit, old_vid_high);
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            return bcm_er_vlan_translate_range_delete(unit, gport, old_vid_low,
                                                    old_vid_high,
                                                    BCM_VLAN_XLATE_ING);
        }
#endif
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            /* Delete outer vlan translate entry */
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_delete(unit, gport,
                                               bcmVlanTranslateKeyPortOuter,
                                               old_vid_low, BCM_VLAN_INVALID));
            /* Delete inner vlan translate entry and range */
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_delete(unit, gport,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID, 
                                        old_vid_low, old_vid_high));
            return BCM_E_NONE;
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_translate_range_delete_all(int unit)
{
    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            return bcm_er_vlan_translate_range_delete_all(unit, 
                                                        BCM_VLAN_XLATE_ING);
        }
#endif
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            return _bcm_trx_vlan_translate_action_range_delete_all(unit);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

int 
bcm_esw_vlan_dtag_range_add(int unit, int port,
                            bcm_vlan_t old_vid_low, 
                            bcm_vlan_t old_vid_high,
                            bcm_vlan_t new_vid, int int_prio)
{
    bcm_gport_t gport;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid_low);
    CHECK_VID(unit, old_vid_high);
    CHECK_VID(unit, new_vid);
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            return bcm_er_vlan_translate_range_add(unit, gport, old_vid_low,
                                                 old_vid_high, new_vid, int_prio,
                                                 BCM_VLAN_XLATE_DTAG);
        }
#endif
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            bcm_vlan_action_set_t action;

            bcm_vlan_action_set_t_init(&action);
            action.new_outer_vlan = new_vid;
            action.priority = int_prio;
            /* For inner tagged packets, set the outer tag action to ADD.
             * For all other packet types, the action is initialized to NONE.
             */
            action.it_outer = bcmVlanActionAdd;

            return _bcm_trx_vlan_translate_action_range_add(unit, gport,
                                          BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                          old_vid_low, old_vid_high,
                                          &action);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}


int 
bcm_esw_vlan_dtag_range_get (int unit, bcm_port_t port,
                             bcm_vlan_t old_vid_low,
                             bcm_vlan_t old_vid_high,
                             bcm_vlan_t *new_vid,
                             int *prio)
{
    bcm_gport_t     gport;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid_low);
    CHECK_VID(unit, old_vid_high);
    
    if ((NULL == new_vid) || NULL == prio ){
        return BCM_E_PARAM;
    }
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }
    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            return bcm_er_vlan_translate_range_get(unit, gport, old_vid_low,
                                                 old_vid_high, new_vid, prio,
                                                 BCM_VLAN_XLATE_DTAG);
        }
#endif
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            bcm_vlan_action_set_t action;

            bcm_vlan_action_set_t_init(&action);
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_get(unit, gport,
                                        old_vid_low, old_vid_high, 
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                        &action));
            if (BCM_VLAN_INVALID != action.new_outer_vlan) {
                *new_vid = action.new_outer_vlan;
                *prio = action.priority;
                return BCM_E_NONE;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_translate_action_range_get(unit, gport,
                                        BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                        old_vid_low, old_vid_high, 
                                        &action));
            if (BCM_VLAN_INVALID != action.new_inner_vlan) {
                *new_vid = action.new_inner_vlan;
                *prio = action.priority;
                return BCM_E_NONE;
            }

            return BCM_E_NOT_FOUND;
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}


int 
bcm_esw_vlan_dtag_range_delete(int unit, int port, 
                               bcm_vlan_t old_vid_low,
                               bcm_vlan_t old_vid_high)
{
    bcm_gport_t     gport;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid_low);
    CHECK_VID(unit, old_vid_high);
    if (old_vid_high < old_vid_low) {
        return BCM_E_PARAM;
    }
    if (BCM_GPORT_IS_SET(port)) {
        gport = port;
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            return bcm_er_vlan_translate_range_delete(unit, gport, old_vid_low,
                                                    old_vid_high,
                                                    BCM_VLAN_XLATE_DTAG);
        }
#endif
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            return _bcm_trx_vlan_translate_action_range_delete(unit, gport,
                                          BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                          old_vid_low, old_vid_high);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

int 
bcm_esw_vlan_dtag_range_delete_all(int unit)
{
    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            return bcm_er_vlan_translate_range_delete_all(unit, 
                                                        BCM_VLAN_XLATE_DTAG);
        }
#endif
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            return _bcm_trx_vlan_translate_action_range_delete_all(unit);
        }
#endif /* BCM_TRX_SUPPORT */
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_add
 * Description :
 *   Add an entry to the VLAN Translation table, which assigns
 *   VLAN actions for packets matching within the VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (generic port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *      action          (IN) Action for outer and inner tag
 *
 * Notes :
 *   For translation of double-tagged packets, specify a valid
 *   VLAN ID value for outer_vlan_low/high and inner_vlan_low/high.
 *   For translation of single outer-tagged packets, specify a
 *   valid VLAN ID for outer_vlan_low/high and BCM_VLAN_INVALID
 *   for inner_vlan_low/high. For translation of single inner-tagged
 *   packets, specify a valid VLAN ID for inner_vlan_low/high and
 *   BCM_VLAN_INVALID for outer_vlan_low/high.
 */

int
bcm_esw_vlan_translate_action_range_add(int unit, bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low,
                                        bcm_vlan_t outer_vlan_high,
                                        bcm_vlan_t inner_vlan_low,
                                        bcm_vlan_t inner_vlan_high,
                                        bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);
    if (BCM_VLAN_INVALID != outer_vlan_low) {
        CHECK_VID(unit, outer_vlan_low);
    }
    if (BCM_VLAN_INVALID != outer_vlan_high) {
        CHECK_VID(unit, outer_vlan_high);
    }
    if (BCM_VLAN_INVALID != inner_vlan_low) {
        CHECK_VID(unit, inner_vlan_low);
    }
    if (BCM_VLAN_INVALID != inner_vlan_high) {
        CHECK_VID(unit, inner_vlan_high);
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_range_add(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high, 
                                             action);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function :
 *      bcm_esw_vlan_translate_action_range_get
 * Description :
 *   Get an entry to the VLAN Translation table, which assigns
 *   VLAN actions for packets matching within the VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (generic port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *      action          (OUT) Action for outer and inner tag
 *
 * Notes :
 *   For translation of double-tagged packets, specify a valid
 *   VLAN ID value for outer_vlan_low/high and inner_vlan_low/high.
 *   For translation of single outer-tagged packets, specify a
 *   valid VLAN ID for outer_vlan_low/high and BCM_VLAN_INVALID
 *   for inner_vlan_low/high. For translation of single inner-tagged
 *   packets, specify a valid VLAN ID for inner_vlan_low/high and
 *   BCM_VLAN_INVALID for outer_vlan_low/high.
 */
int 
bcm_esw_vlan_translate_action_range_get (int unit, bcm_port_t port,
                                         bcm_vlan_t outer_vlan_low,
                                         bcm_vlan_t outer_vlan_high,
                                         bcm_vlan_t inner_vlan_low,
                                         bcm_vlan_t inner_vlan_high,
                                         bcm_vlan_action_set_t *action)
{
    CHECK_INIT(unit);
    if (BCM_VLAN_INVALID != outer_vlan_low) {
        CHECK_VID(unit, outer_vlan_low);
    }
    if (BCM_VLAN_INVALID != outer_vlan_high) {
        CHECK_VID(unit, outer_vlan_high);
    }
    if (BCM_VLAN_INVALID != inner_vlan_low) {
        CHECK_VID(unit, inner_vlan_low);
    }
    if (BCM_VLAN_INVALID != inner_vlan_high) {
        CHECK_VID(unit, inner_vlan_high);
    }
    if (NULL == action) {
        return BCM_E_PARAM;
    }
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_range_get(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high, 
                                             action);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_delete
 * Description :
 *   Delete an entry from the VLAN Translation table for the 
 *   specified VLAN range(s).
 * Parameters :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress gport (generic port)
 *      outer_vlan_low  (IN) Outer VLAN ID Low
 *      outer_vlan_high (IN) Outer VLAN ID High
 *      inner_vlan_low  (IN) Inner VLAN ID Low
 *      inner_vlan_high (IN) Inner VLAN ID High
 *
 * Notes :
 *   For translation of double-tagged packets, specify a valid
 *   VLAN ID value for outer_vlan_low/high and inner_vlan_low/high.
 *   For translation of single outer-tagged packets, specify a
 *   valid VLAN ID for outer_vlan_low/high and BCM_VLAN_INVALID
 *   for inner_vlan_low/high. For translation of single inner-tagged
 *   packets, specify a valid VLAN ID for inner_vlan_low/high and
 *   BCM_VLAN_INVALID for outer_vlan_low/high.
 */

int
bcm_esw_vlan_translate_action_range_delete(int unit, bcm_gport_t port,
                                           bcm_vlan_t outer_vlan_low,
                                           bcm_vlan_t outer_vlan_high,
                                           bcm_vlan_t inner_vlan_low,
                                           bcm_vlan_t inner_vlan_high)
{
    CHECK_INIT(unit);
    if (BCM_VLAN_INVALID != outer_vlan_low) {
        CHECK_VID(unit, outer_vlan_low);
    }
    if (BCM_VLAN_INVALID != outer_vlan_high) {
        CHECK_VID(unit, outer_vlan_high);
    }
    if (BCM_VLAN_INVALID != inner_vlan_low) {
        CHECK_VID(unit, inner_vlan_low);
    }
    if (BCM_VLAN_INVALID != inner_vlan_high) {
        CHECK_VID(unit, inner_vlan_high);
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_range_delete(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_translate_action_range_delete_all
 * Description :
 *     Delete all VLAN range entries from the VLAN Translation table.
 * Parameters :
 *      unit            (IN) BCM unit number
 */

int
bcm_esw_vlan_translate_action_range_delete_all(int unit)
{
    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_action_range_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function :
 *      bcm_vlan_translate_action_range_traverse
 * Description :
 *   Traverses over VLAN Translation table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_esw_vlan_translate_action_range_traverse(int unit,
    bcm_vlan_translate_action_range_traverse_cb cb, void *user_data)
{
    _bcm_vlan_translate_traverse_t          trvs_st;
    bcm_vlan_action_set_t                   action;
    _translate_action_range_traverse_cb_t   usr_cb_st;
    
    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }
    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_action_range_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    trvs_st.int_cb = _bcm_esw_vlan_translate_action_range_traverse_int_cb;

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_vlan_translate_action_range_traverse(unit, &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}


/*
 * Function :
 *      bcm_vlan_translate_range_traverse
 * Description :
 *   Traverses over VLAN translate table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_esw_vlan_translate_range_traverse(int unit, 
                                  bcm_vlan_translate_range_traverse_cb cb,
                                  void *user_data)
{

    _bcm_vlan_translate_traverse_t          trvs_st;
    bcm_vlan_action_set_t                   action;
    _translate_range_traverse_cb_t          usr_cb_st;
    
    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }
    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_translate_range_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        trvs_st.int_cb = _bcm_esw_vlan_translate_action_range_traverse_int_cb;  
        return _bcm_trx_vlan_translate_action_range_traverse(unit, &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        trvs_st.int_cb = _bcm_esw_vlan_translate_range_traverse_int_cb;  
        return bcm_er_vlan_translate_range_traverse(unit, &trvs_st, BCM_VLAN_XLATE_ING);
    }
#endif /* BCM_TRX_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function :
 *      bcm_vlan_dtag_range_traverse
 * Description :
 *   Traverses over VLAN double tagging table and call provided callback 
 *   with valid entries.
 * Parameters :
 *      unit            (IN) BCM unit number
 *      cb              (IN) User callback function
 *      user_data       (IN) Pointer to user specific data
 * Return:
 *
 *      BCM_E_XXX
 */
int 
bcm_esw_vlan_dtag_range_traverse(int unit, bcm_vlan_dtag_range_traverse_cb cb,
                             void *user_data)
{

    _bcm_vlan_translate_traverse_t          trvs_st;
    bcm_vlan_action_set_t                   action;
    _dtag_range_traverse_cb_t               usr_cb_st;
    
    if (!soc_feature(unit, soc_feature_vlan_translation_range)) {
        return BCM_E_UNAVAIL;
    }
    if (!cb) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trvs_st, 0, sizeof(_bcm_vlan_translate_traverse_t));
    sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
    sal_memset(&usr_cb_st, 0, sizeof(_dtag_range_traverse_cb_t));

    usr_cb_st.usr_cb = cb;

    trvs_st.user_data = user_data;
    trvs_st.action = &action;
    trvs_st.user_cb_st = (void *)&usr_cb_st;
    
#if defined(BCM_TRX_SUPPORT)
    trvs_st.int_cb = _bcm_esw_vlan_translate_action_range_traverse_int_cb;  
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_vlan_translate_action_range_traverse(unit, &trvs_st);
    }
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        trvs_st.int_cb = _bcm_esw_vlan_translate_range_traverse_int_cb;  
        return bcm_er_vlan_translate_range_traverse(unit, &trvs_st, BCM_VLAN_XLATE_DTAG);
    }
#endif /* BCM_TRX_SUPPORT */
    
    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_translate_egress_add(int unit, int port, bcm_vlan_t old_vid,
                                  bcm_vlan_t new_vid, int prio)
{

    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);
    CHECK_VID(unit, new_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;
        
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }

        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = new_vid;
        action.new_inner_vlan = 0;
        action.priority = prio;
        action.ot_outer = bcmVlanActionReplace;
        action.dt_outer = bcmVlanActionReplace;

        return _bcm_trx_vlan_translate_egress_action_add(unit, port, 
                                                        old_vid, 0, 
                                                        &action);
    }
#endif /* BCM_TRX_SUPPORT */


#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_add(unit, port, old_vid, new_vid, prio,
                                          BCM_VLAN_XLATE_EGR);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_add(unit, port, modid, tgid, old_vid,
                                         new_vid, prio, BCM_VLAN_XLATE_EGR);
    }
#endif

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_vlan_translate_egress_get
 * Purpose:
 *      Get vlan egress translation
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid nad prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int bcm_esw_vlan_translate_egress_get (int unit, bcm_port_t port,
                                       bcm_vlan_t old_vid,
                                       bcm_vlan_t *new_vid,
                                       int *prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

    if ((NULL == new_vid) ||(NULL == prio)){
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;
        
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }

        bcm_vlan_action_set_t_init(&action);
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_translate_egress_action_get(unit, port, old_vid,
                                                      0, &action));
        *new_vid = action.new_outer_vlan;
        *prio = action.priority;

        return BCM_E_NONE;
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_get(unit, port, old_vid, new_vid,
                                          prio, BCM_VLAN_XLATE_EGR);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_get(unit, port, modid, tgid, old_vid,
                                         new_vid, prio, BCM_VLAN_XLATE_EGR);
    }
#endif

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_translate_egress_delete(int unit, int port, bcm_vlan_t old_vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

    /* XGS does not support translation of VLAN ID 0 */
    if (old_vid == 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_translate_egress_action_delete(unit, port, 
                                                           old_vid, 0);
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, port, old_vid,
                                             BCM_VLAN_XLATE_EGR);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_delete(unit, port, modid, tgid, old_vid,
                                            BCM_VLAN_XLATE_EGR);
    }
#endif

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_translate_egress_delete_all(int unit)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_translate_egress_action_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, -1, BCM_VLAN_NONE,
                                             BCM_VLAN_XLATE_EGR);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_delete_all(unit, BCM_VLAN_XLATE_EGR);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_action_add
 * Description   :
 *      Add an entry to egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int
bcm_esw_vlan_translate_egress_action_add(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_action_set_t *action)
{   
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id; 

    CHECK_INIT(unit);
    CHECK_VID(unit, outer_vlan);
    CHECK_VID(unit, inner_vlan);

    if (NULL == action) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port_class)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port_class, &modid, &port_class, 
                                   &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port_class)) { 
            return BCM_E_PORT; 
        }
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_egress_action_add(unit, port_class, 
                                             outer_vlan, inner_vlan, 
                                             action);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_action_get
 * Description   :
 *      Get an entry to egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (OUT) Action for outer and inner tag
 */
int 
bcm_esw_vlan_translate_egress_action_get (int unit, int port_class,
                                          bcm_vlan_t outer_vlan,
                                          bcm_vlan_t inner_vlan,
                                          bcm_vlan_action_set_t *action)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id; 

    CHECK_INIT(unit);
    CHECK_VID(unit, outer_vlan);
    CHECK_VID(unit, inner_vlan);

    if (NULL == action) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port_class)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port_class, &modid, &port_class, 
                                   &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port_class)) { 
            return BCM_E_PORT; 
        }
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_egress_action_get(unit, port_class, 
                                                        outer_vlan, inner_vlan, 
                                                        action);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_translate_egress_action_delete
 * Purpose:
 *      Delete an egress vlan translate lookup entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int
bcm_esw_vlan_translate_egress_action_delete(int unit, int port_class,
                                            bcm_vlan_t outer_vlan,
                                            bcm_vlan_t inner_vlan)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid;
    int                 id; 

    CHECK_INIT(unit);
    CHECK_VID(unit, outer_vlan);
    CHECK_VID(unit, inner_vlan);

    if (BCM_GPORT_IS_SET(port_class)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port_class, &modid, &port_class, 
                                   &tgid, &id));
        if ((-1 != tgid) || (-1 != id)){
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port_class)) { 
            return BCM_E_PORT; 
        }
    }

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        return _bcm_trx_vlan_translate_egress_action_delete(unit, port_class, 
                                                           outer_vlan, 
                                                           inner_vlan);
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_dtag_add(int unit, int port, bcm_vlan_t inner_vid,
                      bcm_vlan_t outer_vid, int prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

    CHECK_INIT(unit);
    CHECK_VID(unit, inner_vid);
    CHECK_VID(unit, outer_vid);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_vlan_action_set_t action;
        bcm_gport_t gport;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) { 
                return BCM_E_PORT; 
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }

        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = outer_vid;
        action.priority = prio;
        /* For inner tagged packets, set the outer tag action to ADD.
         * For all other packet types, the action is initialized to NONE.
         */
        action.it_outer = bcmVlanActionAdd;

        return _bcm_trx_vlan_translate_action_add(unit, gport,
                                                 bcmVlanTranslateKeyPortInner,
                                                 0, inner_vid, &action);
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_add(unit, port, inner_vid, outer_vid,
                                          prio, BCM_VLAN_XLATE_DTAG);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_add(unit, port, modid, tgid, inner_vid,
                                         outer_vid, prio, BCM_VLAN_XLATE_DTAG);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_dtag_get
 * Purpose:
 *      Get vlan translation for double tagging
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port numebr
 *      old_vid - Old VLAN ID to has translation for
 *      new_vid - New VLAN ID that packet will get
 *      prio    - Priority
 * Returns:
 *      BCM_E_NONE - Translation found, new_vid and prio will have the values.
 *      BCM_E_NOT_FOUND - Translation does not exist
 *      BCM_E_XXX  - Other error
 * Notes:
 *      None.
 */
int 
bcm_esw_vlan_dtag_get (int unit, bcm_port_t port,
                       bcm_vlan_t old_vid,
                       bcm_vlan_t *new_vid,
                       int *prio)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

    CHECK_INIT(unit);
    CHECK_VID(unit, old_vid);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_gport_t gport;
        bcm_vlan_action_set_t action;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }
        bcm_vlan_action_set_t_init(&action);
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_translate_action_get(unit, gport,
                                                 bcmVlanTranslateKeyPortInner,
                                               0, old_vid, &action));
        if (bcmVlanActionAdd == action.it_outer) {
            *new_vid = action.new_outer_vlan;
            *prio = action.priority;

            return BCM_E_NONE;
        } 

        return BCM_E_NOT_FOUND;
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_get(unit, port, old_vid, new_vid,
                                          prio, BCM_VLAN_XLATE_DTAG);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_get(unit, port, modid, tgid, old_vid,
                                         new_vid, prio, BCM_VLAN_XLATE_DTAG);
    }
#endif

    return BCM_E_UNAVAIL;
}


int
bcm_esw_vlan_dtag_delete(int unit, int port, bcm_vlan_t vid)
{
    bcm_module_t        modid;
    bcm_trunk_t         tgid = -1;
    int                 id = -1;

    CHECK_INIT(unit);
    CHECK_VID(unit, vid);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_gport_t gport;

        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(port)) {
            gport = port;
        } else {
            if (!SOC_PORT_VALID(unit, port)) { 
                return BCM_E_PORT; 
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &gport));
        }

        return _bcm_trx_vlan_translate_action_delete(unit, gport,
                                                    bcmVlanTranslateKeyPortInner,
                                                    0, vid);
    }
#endif /* BCM_TRX_SUPPORT */

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if (!SOC_IS_EASYRIDER(unit) && (-1 != tgid || -1 != id)) {
            return BCM_E_PORT;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) { 
            return BCM_E_PORT; 
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_my_modid_get(unit, &modid));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, port, vid,
                                             BCM_VLAN_XLATE_DTAG);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_delete(unit, port, modid, tgid, vid,
                                            BCM_VLAN_XLATE_DTAG);
    }
#endif

    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_dtag_delete_all(int unit)
{
    CHECK_INIT(unit);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (!soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
        return _bcm_trx_vlan_translate_action_delete_all(unit);
    }
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_vlan_translate_delete(unit, -1, BCM_VLAN_NONE,
                                             BCM_VLAN_XLATE_DTAG);
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return bcm_er_vlan_translate_delete_all(unit, BCM_VLAN_XLATE_DTAG);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * IP based vlans
 */

#ifdef BCM_XGS3_SWITCH_SUPPORT
static void
_ip6_prefix_to_mask(int prefix, bcm_ip6_t mask)
{
    int i;

    sal_memset(mask, 0, sizeof(bcm_ip6_t));
    /* Construct the mask from the prefix */
    for (i = 0; prefix > 8; prefix -= 8, i++) {
        mask[i] = 0xff;
    }
    mask[i] = 0xff << (8 - prefix);
}

static int
_ip6_mask_to_prefix(bcm_ip6_t mask)
{
    int i;
    int prefix;
    for (i = 0, prefix = 0; mask[i] == 0xff; i++) {
        prefix += 8;
    }
    switch(mask[i]) {
        case 0x80:
            prefix += 1;
            break;
        case 0xc0:
            prefix += 2;
            break;
        case 0xe0:
            prefix += 3;
            break;
        case 0xf0:
            prefix += 4;
            break;
        case 0xf8:
            prefix += 5;
            break;
        case 0xfc:
            prefix += 6;
            break;
        case 0xfe:
            prefix += 7;
            break;
        case 0:
            break;
        default:
            return(-1);
    }
    return prefix;
}

static void
_ip4cat(uint8 * buf, bcm_ip_t ip)
{
    int i;
    for (i = 3; i > -1; i--) {
        buf[i] = ip & 0x00ff;
        ip >>= 8;
    }
}

/*
 * On XGS3, VLAN_SUBNET.IP_ADDR stores only a top half (64 bits) of
 * the IPV6 address. Comparing it against a full length of a parameter
 * will have certainly result in a mismatch. There should be a
 * special type for a stored part of the address; meanwhile, a
 * symbolic constant is used.
 */

#define VLAN_SUBNET_IP_ADDR_LENGTH 8

/* Macro to copy bcm_ip6_t addr into uint32[] for soc_mem_field_set call */
#define VLAN_IP6_TO_HW_FORMAT(_ip, _reg)        \
    do { \
        _reg[0] = (_ip[4] << 24) | (_ip[5] << 16) | (_ip[6] << 8) | _ip[7]; \
        _reg[1] = (_ip[0] << 24) | (_ip[1] << 16) | (_ip[2] << 8) | _ip[3]; \
    } while (0)

/* Macro to copy uint32[] addr values from soc_mem_field_get into bcm_ip6_t */
#define VLAN_HW_FORMAT_TO_IP6(_reg, _ip)        \
    do { \
       _ip[3] = (_reg[1]) & 0xff;               \
       _ip[2] = (_reg[1] >> 8) & 0xff;          \
       _ip[1] = (_reg[1] >> 16) & 0xff;         \
       _ip[0] = (_reg[1] >> 24) & 0xff;         \
       _ip[7] = (_reg[0]) & 0xff;               \
       _ip[6] = (_reg[0] >> 8) & 0xff;          \
       _ip[5] = (_reg[0] >> 16) & 0xff;         \
       _ip[4] = (_reg[0] >> 24) & 0xff;         \
    } while (0)

static int
_xgs3_vlan_ipv6_add(int unit,
                    bcm_ip6_t ip, bcm_ip6_t mask, bcm_vlan_t vid, int prio)
{
    int i, imin, imax, nent, vsbytes;
    int match = -1, empty = -1, insert = -1;
    int rv;
    vlan_subnet_entry_t * vstab, * vsnull, *vstabp;
    int prefix;
    uint32 hw_ip[2], hw_nm[2];

    VLAN_CHK_ID(unit, vid);
    VLAN_CHK_PRIO(unit, prio);

    VLAN_IP6_TO_HW_FORMAT(ip, hw_ip);
    VLAN_IP6_TO_HW_FORMAT(mask, hw_nm);

    imin = soc_mem_index_min(unit, VLAN_SUBNETm);
    imax = soc_mem_index_max(unit, VLAN_SUBNETm);
    nent = soc_mem_index_count(unit, VLAN_SUBNETm);
    vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
    vsbytes = WORDS2BYTES(vsbytes);

    prefix = _ip6_mask_to_prefix(mask);
    if (prefix < 0) {
        return BCM_E_PARAM;
    }

    vstab = soc_cm_salloc(unit, nent * sizeof(*vstab), "vlan_subnet");
    if (vstab == NULL) {
        return BCM_E_MEMORY;
    }

    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    soc_mem_lock(unit, VLAN_SUBNETm);
    rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                            imin, imax, vstab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_SUBNETm);
        soc_cm_sfree(unit, vstab);
        return rv;
    }

    for(i = 0; i < nent; i++) {
        uint32 enm[2], eip[2];
        bcm_ip6_t temp_nm;
        int epx;
        vstabp = soc_mem_table_idx_to_pointer(unit,
                        VLAN_SUBNETm, vlan_subnet_entry_t *,
                        vstab, i);
        if (sal_memcmp(vstabp, vsnull, vsbytes) == 0) {
            empty = i;
            break;
        }
        soc_mem_field_get(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, MASKf, (uint32 *) enm);

        /* copy mask into bcm_ip6_t */
        sal_memset(temp_nm, 0, sizeof(bcm_ip6_t));
        VLAN_HW_FORMAT_TO_IP6(enm, temp_nm);

        epx = _ip6_mask_to_prefix(temp_nm);
        if (epx < 0) {
            return BCM_E_PARAM;
        }
        if ((epx < prefix) && (insert < 0)) {
            insert = i;
        }
        if (sal_memcmp(hw_nm, enm, VLAN_SUBNET_IP_ADDR_LENGTH) != 0) {
            continue;
        }
        soc_mem_field_get(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, IP_ADDRf, (uint32 *) eip);

        if (sal_memcmp(hw_ip, eip, VLAN_SUBNET_IP_ADDR_LENGTH) == 0) {
            match = i;
        }
    }

    if (match >= 0) {   /* found an exact match */
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                            vlan_subnet_entry_t *, vstab, match);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, VLAN_IDf, vid);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, PRIf, prio);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, VALIDf, 1);
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
        if (soc_mem_field_valid(unit, VLAN_SUBNETm, RESERVED_MASKf)) {
            soc_mem_field32_set(unit, VLAN_SUBNETm, vstabp, RESERVED_MASKf, 0);
        }
        if (soc_mem_field_valid(unit, VLAN_SUBNETm, RESERVED_KEYf)) {
            soc_mem_field32_set(unit, VLAN_SUBNETm, vstabp, RESERVED_KEYf, 0);
        }
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, match, vstabp);
    } else if (empty < 0) {
        rv = BCM_E_FULL;
    } else {
        if (insert < 0) {
            insert = empty;
        }

        for (i = empty; i > insert; i--) {
            vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                            vlan_subnet_entry_t *, vstab, (i - 1));
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vstabp);
            if (rv < 0) {
                break;
            }
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i - 1, &vsnull);
            if (rv < 0) {
                break;
            }
        }
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, insert);
        sal_memset(vstabp, 0, vsbytes);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, VLAN_IDf, vid);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, PRIf, prio);
        soc_VLAN_SUBNETm_field32_set(unit, vstabp, VALIDf, 1);
        soc_mem_field_set(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, IP_ADDRf, (uint32 *) hw_ip);
        soc_mem_field_set(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, MASKf, (uint32 *) hw_nm);
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
        if (soc_mem_field_valid(unit, VLAN_SUBNETm, RESERVED_MASKf)) {
            soc_mem_field32_set(unit, VLAN_SUBNETm, vstabp, RESERVED_MASKf, 0);
        }
        if (soc_mem_field_valid(unit, VLAN_SUBNETm, RESERVED_KEYf)) {
            soc_mem_field32_set(unit, VLAN_SUBNETm, vstabp, RESERVED_KEYf, 0);
        }
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */

        if (rv >= 0) {
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL,
                    insert, vstabp);
        }
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}

static int
_xgs3_vlan_ipv6_delete(int unit, bcm_ip6_t ip, bcm_ip6_t mask)
{
    int i, imin, imax, nent, vsbytes;
    int match = -1, empty = -1;
    int rv;
    vlan_subnet_entry_t * vstab, * vsnull,  *vstabp;
    uint32 hw_ip[2], hw_nm[2];

    VLAN_IP6_TO_HW_FORMAT(ip, hw_ip);
    VLAN_IP6_TO_HW_FORMAT(mask, hw_nm);

    imin = soc_mem_index_min(unit, VLAN_SUBNETm);
    imax = soc_mem_index_max(unit, VLAN_SUBNETm);
    nent = soc_mem_index_count(unit, VLAN_SUBNETm);
    vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
    vsbytes = WORDS2BYTES(vsbytes);
    vstab = soc_cm_salloc(unit, nent * sizeof(*vstab), "vlan_subnet");

    if (vstab == NULL) {
        return BCM_E_MEMORY;
    }

    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    soc_mem_lock(unit, VLAN_SUBNETm);
    rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                            imin, imax, vstab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_SUBNETm);
        soc_cm_sfree(unit, vstab);
        return rv;
    }

    for(i = 0; i < nent; i++) {
        uint32 enm[2], eip[2];
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, i);
        if (sal_memcmp(vstabp, vsnull, vsbytes) == 0) {
            empty = i;
            break;
        }
        soc_mem_field_get(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, MASKf, (uint32 *) enm);
        if (sal_memcmp(hw_nm, enm, VLAN_SUBNET_IP_ADDR_LENGTH) != 0) {
            continue;
        }
        soc_mem_field_get(unit, VLAN_SUBNETm,
                (uint32 *) vstabp, IP_ADDRf, (uint32 *) eip);
        if (sal_memcmp(hw_ip, eip, VLAN_SUBNET_IP_ADDR_LENGTH) == 0) {
            match = i;
        }
    }

    if (match < 0) {
        soc_mem_unlock(unit, VLAN_SUBNETm);
        soc_cm_sfree(unit, vstab);
        return BCM_E_NOT_FOUND;
    }

    if (empty == -1) {
        empty = nent;
    }

    for (i = match; (i < empty - 1) && (rv == BCM_E_NONE); i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, (i + 1));
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vstabp);
    }

    if (rv >= 0) {
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, empty - 1, vsnull);
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}

static int
_xgs3_vlan_ipv6_delete_all(int unit)
{
    int i, imax;
    int rv = BCM_E_NONE;
    vlan_subnet_entry_t * vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    soc_mem_lock(unit, VLAN_SUBNETm);
    imax = soc_mem_index_max(unit, VLAN_SUBNETm);

    for (i = soc_mem_index_min(unit, VLAN_SUBNETm); i <= imax; i++) {
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, i, vsnull);
        if (rv < 0) {
            break;
        }
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    return rv;
}
#endif

/*
 * Vlan selection based on IPv4 addresses
 */

/*
 * Function:
 *      bcm_vlan_ip4_add
 * Purpose:
 *      Add an IPv4 subnet lookup to select vlan and priority for
 *      untagged packets
 * Parameters:
 *      unit -          device number
 *      ipaddr -        IPv4 address
 *      netmask -       network mask of ipaddr to match
 *      vid -           VLAN number to give matching packets
 *      prio -          priority to give matching packets
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This API is superseded by bcm_vlan_ip_add.
 */

int
bcm_esw_vlan_ip4_add(int unit, bcm_ip_t ipaddr, bcm_ip_t netmask,
                     bcm_vlan_t vid, int prio)
{
#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        int                     cng, i, nent, rv;
        int                     match, empty, insert, vsbytes;
        vlan_subnet_entry_t     *vsent, *vsnull, *vsentp;
        vlan_data_entry_t       vdent;
        bcm_ip_t                eip, enm;
        uint32                     imin, imax;

        VLAN_CHK_ID(unit, vid);

        cng = 0;
        if (prio & BCM_PRIO_DROP_FIRST) {
            cng = 1;
            prio &= ~BCM_PRIO_DROP_FIRST;
        }
        VLAN_CHK_PRIO(unit, prio);
        vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
        vsbytes = WORDS2BYTES(vsbytes);

        imin = soc_mem_index_min(unit, VLAN_SUBNETm);
        imax = soc_mem_index_max(unit, VLAN_SUBNETm);
        nent = soc_mem_index_count(unit, VLAN_SUBNETm);
        vsent = soc_cm_salloc(unit, nent * sizeof(*vsent),
                              "vlan_subnet_entry");
        if (vsent == NULL) {
            return BCM_E_MEMORY;
        }

        vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                                soc_mem_index_min(unit, VLAN_SUBNETm),
                                soc_mem_index_max(unit, VLAN_SUBNETm),
                                vsent);
        if (rv < 0) {
            soc_mem_unlock(unit, VLAN_SUBNETm);
            soc_cm_sfree(unit, vsent);
            return rv;
        }

        match = empty = insert = -1;
        for (i = 0; i < nent; i++) {
            vsentp = soc_mem_table_idx_to_pointer(unit,
                            VLAN_SUBNETm, vlan_subnet_entry_t *,
                            vsent, i);
            if (sal_memcmp(vsentp, vsnull, vsbytes) == 0) {
                empty = i;
                break;
            }
            enm = soc_VLAN_SUBNETm_field32_get(unit, vsentp, MASKf);
            if (~enm > ~netmask && insert < 0) {
                insert = i;
            }
            if (enm != netmask) {
                continue;
            }
            eip = soc_VLAN_SUBNETm_field32_get(unit, vsentp, IP_ADDRf);
            if (eip == ipaddr) {
                match = i;
            }
        }

        if (match >= 0) {       /* exact match found, we'll update it */
            sal_memset(&vdent, 0, sizeof(vdent));
            soc_VLAN_DATAm_field32_set(unit, &vdent, VLAN_IDf, vid);
            soc_VLAN_DATAm_field32_set(unit, &vdent, PRIf, prio);
            soc_VLAN_DATAm_field32_set(unit, &vdent, CNGf, cng);
            rv = WRITE_VLAN_DATAm(unit, MEM_BLOCK_ALL, match, &vdent);
            soc_mem_unlock(unit, VLAN_SUBNETm);
            soc_cm_sfree(unit, vsent);
            return rv;
        }
        if (empty < 0) {        /* table is full */
            soc_mem_unlock(unit, VLAN_SUBNETm);
            soc_cm_sfree(unit, vsent);
            return BCM_E_FULL;
        }
        if (insert < 0) {       /* append at the end */
            insert = empty;
        }

        /*
         * We must be careful here never to leave the vlan_subnet and
         * vlan_data tables inconsistent.  The algorithm is, for each entry
         * from empty back to insert:
         *      copy vlan_data[i-1] -> vlan_data[i]
         *      copy vlan_subnet[i-1] -> vlan_subnet[i]
         *      clear vlan_subnet[i-1]
         * insert vlan_data[i] at insert
         * insert vlan_subnet[i] at insert
         */

        for (i = empty; i > insert; i--) {
            rv = READ_VLAN_DATAm(unit, MEM_BLOCK_ANY, i-1, &vdent);
            if (rv < 0) {
                break;
            }
            rv = WRITE_VLAN_DATAm(unit, MEM_BLOCK_ANY, i, &vdent);
            if (rv < 0) {
                break;
            }
            vsentp = soc_mem_table_idx_to_pointer(unit,
                            VLAN_SUBNETm, vlan_subnet_entry_t *,
                            vsent, (i - 1));
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vsentp);
            if (rv < 0) {
                break;
            }
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, i-1, vsnull);
            if (rv < 0) {
                break;
            }
        }

        sal_memset(&vdent, 0, sizeof(vdent));
        soc_VLAN_DATAm_field32_set(unit, &vdent, VLAN_IDf, vid);
        soc_VLAN_DATAm_field32_set(unit, &vdent, PRIf, prio);
        soc_VLAN_DATAm_field32_set(unit, &vdent, CNGf, cng);
        vsentp = soc_mem_table_idx_to_pointer(unit,
                        VLAN_SUBNETm, vlan_subnet_entry_t *,
                        vsent, insert);
        soc_VLAN_SUBNETm_field32_set(unit, vsentp, IP_ADDRf, ipaddr);
        soc_VLAN_SUBNETm_field32_set(unit, vsentp, MASKf, netmask);
        rv = WRITE_VLAN_DATAm(unit, MEM_BLOCK_ALL, insert, &vdent);
        if (rv >= 0) {
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, insert, vsentp);
        }

        soc_mem_unlock(unit, VLAN_SUBNETm);
        soc_cm_sfree(unit, vsent);
        return rv;
    }
#endif /* BCM_DRACO15_SUPPORT */
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_ip6_t ip6addr;
        bcm_ip6_t ip6mask;
        uint8 * ptr;

#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SC_CQ(unit) &&
            !soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
#endif
        /* Construct ipv6 mask/address from ipv4 parameters.
         * Only use the upper 64-bits of ipv6 mask/address.
         */
        ptr = ip6addr;
        sal_memset(ptr, 0, sizeof(ip6addr));
        sal_memcpy(ptr, "\xff\xff\x00\x00", 4);
        _ip4cat(ptr + 4, ipaddr);

        ptr = ip6mask;
        sal_memset(ptr, 0, sizeof(ip6mask));
        sal_memcpy(ptr, "\xff\xff\xff\xff", 4);
        _ip4cat(ptr + 4, netmask);

        return _xgs3_vlan_ipv6_add(unit, ip6addr, ip6mask, vid, prio);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_ip4_delete
 * Purpose:
 *      Delete an IPv4 subnet lookup entry.
 * Parameters:
 *      unit -          device number
 *      ipaddr -        IPv4 address
 *      netmask -       network mask of ipaddr to match
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This API is superseded by bcm_vlan_ip_delete.
 */

int
bcm_esw_vlan_ip4_delete(int unit, bcm_ip_t ipaddr, bcm_ip_t netmask)
{
#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        int                     i, nent, rv, match, empty, vsbytes;
        vlan_subnet_entry_t     *vsent, *vsnull, *vsentp;
        vlan_data_entry_t       vdent;
        bcm_ip_t                eip, enm;

        nent = soc_mem_index_count(unit, VLAN_SUBNETm);
        vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
        vsbytes = WORDS2BYTES(vsbytes);
        vsent = soc_cm_salloc(unit, nent * sizeof(*vsent),
                              "vlan_subnet_entry");
        if (vsent == NULL) {
            return BCM_E_MEMORY;
        }

        vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                                soc_mem_index_min(unit, VLAN_SUBNETm),
                                soc_mem_index_max(unit, VLAN_SUBNETm),
                                vsent);
        if (rv < 0) {
            soc_mem_unlock(unit, VLAN_SUBNETm);
            soc_cm_sfree(unit, vsent);
            return rv;
        }

        match = empty = -1;
        for (i = 0; i < nent; i++) {
            vsentp = soc_mem_table_idx_to_pointer(unit,
                            VLAN_SUBNETm, vlan_subnet_entry_t *,
                            vsent, i);
            if (sal_memcmp(vsentp, vsnull, vsbytes) == 0) {
                empty = i;
                break;
            }
            enm = soc_VLAN_SUBNETm_field32_get(unit, vsentp, MASKf);
            if (enm != netmask) {
                continue;
            }
            eip = soc_VLAN_SUBNETm_field32_get(unit, vsentp, IP_ADDRf);
            if (eip == ipaddr) {
                match = i;
            }
        }
        if (match < 0) {
            soc_mem_unlock(unit, VLAN_SUBNETm);
            soc_cm_sfree(unit, vsent);
            return BCM_E_NOT_FOUND;
        }
        if (empty < 0) {        /* table is full */
            empty = nent;
        }

        /*
         * We must be careful here never to leave the vlan_subnet and
         * vlan_data tables inconsistent.  The algorithm is, for each entry
         * from match to empty:
         *      clear vlan_subnet[i]
         *      copy vlan_data[i+1] -> vlan_data[i]
         *      copy vlan_subnet[i+1] -> vlan_subnet[i]
         *      clear the last duplicated vlan_subnet/vlan_data entry
         */

        rv = BCM_E_NONE;
        for (i = match; i < empty-1; i++) {
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, i, vsnull);
            if (rv < 0) {
                break;
            }
            rv = READ_VLAN_DATAm(unit, MEM_BLOCK_ANY, i+1, &vdent);
            if (rv < 0) {
                break;
            }
            rv = WRITE_VLAN_DATAm(unit, MEM_BLOCK_ANY, i, &vdent);
            if (rv < 0) {
                break;
            }
            vsentp = soc_mem_table_idx_to_pointer(unit,
                            VLAN_SUBNETm, vlan_subnet_entry_t *,
                            vsent, (i + 1));
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vsentp);
            if (rv < 0) {
                break;
            }
        }
        if (rv >= 0 && empty <= nent) {
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, empty-1, vsnull);
            if (rv >= 0) {
                rv = WRITE_VLAN_DATAm(unit, MEM_BLOCK_ALL, empty-1,
                                      soc_mem_entry_null(unit, VLAN_DATAm));
            }
        }

        soc_mem_unlock(unit, VLAN_SUBNETm);
        soc_cm_sfree(unit, vsent);
        return rv;
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_ip6_t ip6addr;
        bcm_ip6_t ip6mask;
        uint8 * ptr;

#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SC_CQ(unit) &&
            !soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
#endif
        /* Construct ipv6 mask/address from ipv4 parameters */
        ptr = ip6addr;
        sal_memset(ptr, 0, sizeof(ip6addr));
        sal_memcpy(ptr, "\xff\xff\x00\x00", 4);
        _ip4cat(ptr + 4, ipaddr);

        ptr = ip6mask;
        sal_memset(ptr, 0, sizeof(ip6mask));
        sal_memcpy(ptr, "\xff\xff\xff\xff", 4);
        _ip4cat(ptr + 4, netmask);

        return _xgs3_vlan_ipv6_delete(unit, ip6addr, ip6mask);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_ip4_delete_all
 * Purpose:
 *      Delete all IPv4 subnet lookup entries.
 * Parameters:
 *      unit -          device number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This API is superseded by bcm_vlan_ip_delete_all.
 */

int
bcm_esw_vlan_ip4_delete_all(int unit)
{
#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        int                     i, imax, rv;
        vlan_subnet_entry_t     *vsnull;
        vlan_data_entry_t       *vdnull;

        vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);
        vdnull = soc_mem_entry_null(unit, VLAN_DATAm);
        rv = BCM_E_NONE;
        soc_mem_lock(unit, VLAN_SUBNETm);
        /* Clear the VLAN_SUBNET memory */
        i = soc_mem_index_min(unit, VLAN_SUBNETm);
        imax = soc_mem_index_max(unit, VLAN_SUBNETm);
        for (; i <= imax; i++) {
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, i, vsnull);
            if (rv < 0) {
                break;
            }
        }
        /* Clear the VLAN_DATA memory */
        i = soc_mem_index_min(unit, VLAN_DATAm);
        imax = soc_mem_index_max(unit, VLAN_DATAm);
        for (; i <= imax; i++) {
            rv = WRITE_VLAN_DATAm(unit, MEM_BLOCK_ALL, i, vdnull);
            if (rv < 0) {
                break;
            }
        }
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return rv;
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SC_CQ(unit) &&
            !soc_feature(unit, soc_feature_vlan_action)) {
            return BCM_E_UNAVAIL;
        }
#endif
        return _xgs3_vlan_ipv6_delete_all(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Vlan selection based on IPv6 addresses
 */

/*
 * Function:
 *      _bcm_vlan_ip6_add (internal)
 * Purpose:
 *      Add an IPv6 subnet lookup to select vlan and priority for
 *      untagged packets
 * Parameters:
 *      unit -          device number
 *      ipaddr -        IPv6 address
 *      prefix -        network prefix of ipaddr to match
 *      vlan -          VLAN number to give matching packets
 *      prio -          priority to give matching packets
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Valid prefix lengths may be limited to no more than 64
 */

STATIC int
_bcm_vlan_ip6_add(int unit,
                  bcm_ip6_t ipaddr,
                  int prefix,
                  bcm_vlan_t vlan,
                  int prio)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_ip6_t mask;

        if (prefix > 64) {
            return BCM_E_PARAM;
        }

        _ip6_prefix_to_mask(prefix, mask);
        return _xgs3_vlan_ipv6_add(unit, ipaddr, mask, vlan, prio);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_vlan_ip6_delete (internal)
 * Purpose:
 *      Delete an IPv6 subnet lookup entry
 * Parameters:
 *      unit -          device number
 *      ipaddr -        IPv6 address
 *      prefix -        network prefix of ipaddr to match
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_vlan_ip6_delete(int unit,
                     bcm_ip6_t ipaddr,
                     int prefix)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_ip6_t mask;

        if (prefix > 64) {
            return BCM_E_PARAM;
        }

        _ip6_prefix_to_mask(prefix, mask);
        return _xgs3_vlan_ipv6_delete(unit, ipaddr, mask);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_vlan_ip6_delete_all (internal)
 * Purpose:
 *      Delete all IPv6 subnet lookup entries
 * Parameters:
 *      unit -          device number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_vlan_ip6_delete_all(int unit)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        return _xgs3_vlan_ipv6_delete_all(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Vlan selection based on unified IPv4/IPv6 information structure.
 */

/*
 * Function:
 *      bcm_vlan_ip_add
 * Purpose:
 *      Add a subnet lookup to select vlan and priority for
 *      untagged packets
 * Parameters:
 *      unit -          device number
 *      vlan_ip -       structure specifying IP address and other info
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_add(int unit, bcm_vlan_ip_t *vlan_ip)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        int rv; 

        if (soc_feature(unit, soc_feature_vlan_action)) {
            bcm_vlan_action_set_t action;

            bcm_vlan_action_set_t_init(&action);
            action.new_outer_vlan = vlan_ip->vid;
            action.new_inner_vlan = 0;
            action.priority = vlan_ip->prio;
            action.dt_outer_prio = bcmVlanActionReplace;
            action.ot_outer_prio = bcmVlanActionReplace;
            action.it_outer      = bcmVlanActionAdd;
            action.it_inner_prio = bcmVlanActionDelete;
            action.ut_outer      = bcmVlanActionAdd;

            soc_mem_lock(unit, VLAN_SUBNETm);
            rv =  _bcm_trx_vlan_ip_action_add(unit, vlan_ip, &action);
            soc_mem_unlock(unit, VLAN_SUBNETm);

            return rv;
        } else {
            return BCM_E_UNAVAIL;  /* Latency bypass */
        }
    }
#endif /* BCM_TRX_SUPPORT */

    return (vlan_ip->flags & BCM_VLAN_SUBNET_IP6)
        ? _bcm_vlan_ip6_add(unit, vlan_ip->ip6, vlan_ip->prefix,
                vlan_ip->vid, vlan_ip->prio)
        : bcm_esw_vlan_ip4_add(unit, vlan_ip->ip4, vlan_ip->mask,
                vlan_ip->vid, vlan_ip->prio);
}

/*
 * Function:
 *      bcm_vlan_ip_delete
 * Purpose:
 *      Delete a subnet lookup entry.
 * Parameters:
 *      unit -          device number
 *      vlan_ip -       structure specifying IP address and other info
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_delete(int unit, bcm_vlan_ip_t *vlan_ip)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        int rv;

        if (soc_feature(unit, soc_feature_vlan_action)) {
            soc_mem_lock(unit, VLAN_SUBNETm);
            rv = _bcm_trx_vlan_ip_delete(unit, vlan_ip);
            soc_mem_unlock(unit, VLAN_SUBNETm);

            return rv;
        } else {
            return BCM_E_UNAVAIL;  /* Latency bypass */
        }
    }
#endif /* BCM_TRX_SUPPORT */

    return (vlan_ip->flags & BCM_VLAN_SUBNET_IP6)
        ? _bcm_vlan_ip6_delete(unit, vlan_ip->ip6, vlan_ip->prefix)
        : bcm_esw_vlan_ip4_delete(unit, vlan_ip->ip4, vlan_ip->mask);
}

/*
 * Function:
 *      bcm_vlan_ip_delete_all
 * Purpose:
 *      Delete all subnet lookup entries.
 * Parameters:
 *      unit -          device number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_delete_all(int unit)
{
    int rv;

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        if (soc_feature(unit, soc_feature_vlan_action)) {
            soc_mem_lock(unit, VLAN_SUBNETm);
            rv = _bcm_trx_vlan_ip_delete_all(unit);
            soc_mem_unlock(unit, VLAN_SUBNETm);

            return rv;
        } else {
            return BCM_E_UNAVAIL;  /* Latency bypass */
        }
    }
#endif /* BCM_TRX_SUPPORT */

    if ((rv = _bcm_vlan_ip6_delete_all(unit)) == BCM_E_UNAVAIL) {
        rv = BCM_E_NONE;
    }
    if (rv == BCM_E_NONE) {
        rv = bcm_esw_vlan_ip4_delete_all(unit);
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_ip_action_add
 * Purpose:
 *      Add a subnet lookup to select vlan action for untagged packets
 * Parameters:
 *      unit    -   device number
 *      vlan_ip -   structure specifying IP address and other info
 *      action  -   structure VLAN tag actions
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_action_add(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
#ifdef BCM_TRX_SUPPORT
    int rv;
        
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_action_add(unit, vlan_ip, action);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_ip_action_get
 * Purpose:
 *      Get an action for subnet 
 * Parameters:
 *      unit    -   (IN) device number
 *      vlan_ip -   (IN) structure specifying IP address and other info
 *      action  -   (OUT) structure VLAN tag actions
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_action_get(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
#ifdef BCM_TRX_SUPPORT
    int rv; 

    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_action_get(unit, vlan_ip, action);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_ip_action_delete
 * Purpose:
 *      Delete an action for subnet 
 * Parameters:
 *      unit    -   (IN) device number
 *      vlan_ip -   (IN) structure specifying IP address and other info
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_action_delete(int unit, bcm_vlan_ip_t *vlan_ip)
{
#ifdef BCM_TRX_SUPPORT
    int rv; 

    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_delete(unit, vlan_ip);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_vlan_ip_action_delete_all
 * Purpose:
 *      Delete all actions for all subnets
 * Parameters:
 *      unit    -   (IN) device number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_vlan_ip_action_delete_all(int unit)
{
#ifdef BCM_TRX_SUPPORT
    int rv; 

    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_delete_all(unit);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_esw_vlan_ip_action_traverse
 * Purpose:
 *      Delete all actions for all subnets
 * Parameters:
 *      unit    -   (IN) device number
 *      cb      -   (IN) call back function 
 *      user_data - (IN) a pointer to user data
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_ip_action_traverse(int unit, bcm_vlan_ip_action_traverse_cb cb, 
                                void *user_data)
{
#ifdef BCM_TRX_SUPPORT
    int rv;

    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_vlan_action)) {
        soc_mem_lock(unit, VLAN_SUBNETm);
        rv = _bcm_trx_vlan_ip_action_traverse(unit, cb, user_data);
        soc_mem_unlock(unit, VLAN_SUBNETm);

        return rv;
    }
#endif /* BCM_TRX_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * IMPLEMENTATION NOTES
 *
 * bcm_vlan_ip4_* should be implemented in FB/ER as adding
 * entries to the VLAN_SUBNET tables with:
 *      VLAN_SUBNET.IP_ADDR = 0xFFFF0000 <ip4_addr>
 *      VLAN_SUBNET.MASK = 0xFFFFFFFF <ip4_netmask>
 *      VLAN_SUBNET.VALID = 1
 * The VLAN_SUBNET entries must be sorted in order from
 * longest prefix match to shortest prefix match.
 */

/*
 * Function:
 *     bcm_vlan_control_set
 *
 * Purpose:
 *     Set miscellaneous VLAN-specific chip options
 *
 * Parameters:
 *     unit - StrataSwitch PCI device unit number (driver internal).
 *     type - A value from bcm_vlan_control_t enumeration list
 *     arg  - state whose meaning is dependent on 'type'
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_INTERNAL - Chip access failure.
 *     BCM_E_UNAVAIL  - type not supported on unit
 */

int
bcm_esw_vlan_control_set(int unit, bcm_vlan_control_t type, int arg)
{

    VLAN_VERB(("VLAN %d: control set: type %d, arg %d\n",
               unit, type, arg));

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        int                 port;
        int                 val   = 0;
        uint32              rval  = 0;
        uint32              orval = 0;
        switch (type) {
        case bcmVlanPreferIP4:
            SOC_IF_ERROR_RETURN(READ_ARL_CONTROLr(unit, &rval));
            orval = rval;
            soc_reg_field_set(unit, ARL_CONTROLr, &rval, VLAN_PRECEDENCEf,
                              arg ? 1 : 0);
            if (rval != orval) {
                SOC_IF_ERROR_RETURN(WRITE_ARL_CONTROLr(unit, rval));
            }
            return BCM_E_NONE;
        case bcmVlanPreferMAC:
            SOC_IF_ERROR_RETURN(READ_ARL_CONTROLr(unit, &rval));
            orval = rval;
            soc_reg_field_set(unit, ARL_CONTROLr, &rval, VLAN_PRECEDENCEf,
                              arg ? 0 : 1);
            if (rval != orval) {
                SOC_IF_ERROR_RETURN(WRITE_ARL_CONTROLr(unit, rval));
            }
            return BCM_E_NONE;
        case bcmVlanTranslate:
            arg = (arg ? 1 : 0);
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN
                  (_bcm_esw_port_config_get(unit, port, _bcmPortVlanTranslate,
                                            &val));
                break;
            }
            if ((val != 0) != (arg != 0)) {
                /* Changing to new mode, empty the VLAN MAC table */
                if (val) {
                    bcm_esw_vlan_translate_delete_all(unit);
                } else {
                    bcm_esw_vlan_mac_delete_all(unit);
                }
                /* Now change all ports to new mode */
                PBMP_E_ITER(unit, port) {
                    SOC_IF_ERROR_RETURN
                        (_bcm_esw_port_config_set(unit, port,
                                                  _bcmPortVlanTranslate, arg));
                }
            }
            return BCM_E_NONE;
        default:        /* fall through */
            break;
        }
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        bcm_port_t  port;
        uint32      vlan_ctrl;
        uint32      data;
        int         oval;

        PBMP_E_ITER(unit, port) {
            break;
        }

        switch (type) {
        case bcmVlanDropUnknown:
            return bcm_esw_switch_control_set(unit,
                                              bcmSwitchUnknownVlanToCpu, arg);

        case bcmVlanShared:
            arg = (arg ? 1 : 0);
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            data = vlan_ctrl;
            soc_reg_field_set(unit, VLAN_CTRLr, &vlan_ctrl,
                              USE_LEARN_VIDf, arg);
            if (data != vlan_ctrl) {
                SOC_IF_ERROR_RETURN(WRITE_VLAN_CTRLr(unit, vlan_ctrl));
                if (SOC_IS_FBX(unit)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, EGR_CONFIGr, port,
                                                USE_LEARN_VIDf, arg));
                }
            }
            return BCM_E_NONE;
        case bcmVlanSharedID:
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            data = vlan_ctrl;
            soc_reg_field_set(unit, VLAN_CTRLr, &vlan_ctrl,
                              LEARN_VIDf, arg);
            if (data != vlan_ctrl) {
                SOC_IF_ERROR_RETURN(WRITE_VLAN_CTRLr(unit, vlan_ctrl));
                if (SOC_IS_FBX(unit)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, EGR_CONFIGr, port,
                                                LEARN_VIDf, arg));
                }
            }
            return BCM_E_NONE;
        case bcmVlanPreferIP4:
            arg = (arg ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (_bcm_esw_port_config_get(unit, port,
                                          _bcmPortVlanPrecedence, &oval));
            if (arg != oval) {
                PBMP_E_ITER(unit, port) {
                    SOC_IF_ERROR_RETURN
                        (_bcm_esw_port_config_set(unit, port,
                                              _bcmPortVlanPrecedence, arg));
                }
            }
            return BCM_E_NONE;

        case bcmVlanPreferMAC:
            arg = (arg ? 0 : 1);
            SOC_IF_ERROR_RETURN
                (_bcm_esw_port_config_get(unit, port,
                                          _bcmPortVlanPrecedence, &oval));
            if (arg != oval) {
                PBMP_E_ITER(unit, port) {
                    SOC_IF_ERROR_RETURN
                        (_bcm_esw_port_config_set(unit, port,
                                              _bcmPortVlanPrecedence, arg));
                }
            }
            return BCM_E_NONE;

        case bcmVlanTranslate:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              _bcmPortVlanTranslate, &oval));
                if (arg != oval) {
                    if (SOC_IS_EASYRIDER(unit)) {
                        PBMP_E_ITER(unit, port) {
                            SOC_IF_ERROR_RETURN
                                (soc_reg_field32_modify(unit, EGR_PORTr,
                                                   port, VT_ENABLEf, arg));
                            SOC_IF_ERROR_RETURN
                                (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortVlanTranslate, arg));
                        }
                    } else {
                        PBMP_E_ITER(unit, port) {
                            SOC_IF_ERROR_RETURN
                            (soc_reg_field32_modify(unit, EGR_VLAN_CONTROL_1r,
                                                    port, VT_ENABLEf, arg));
                            SOC_IF_ERROR_RETURN
                                (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortVlanTranslate, arg));
                        }
                    }
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanIgnorePktTag:
#if defined(BCM_EASYRIDER_SUPPORT)
            if (soc_feature(unit, soc_feature_ignore_pkt_tag)) {
                arg = (arg ? 1 : 0);
                SOC_IF_ERROR_RETURN(READ_DEF_VLAN_CONTROLr(unit, &vlan_ctrl));
                data = vlan_ctrl;
                soc_reg_field_set(unit, DEF_VLAN_CONTROLr, &vlan_ctrl,
                                  IGNORE_PKT_TAGf, arg);
                if (data != vlan_ctrl) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_DEF_VLAN_CONTROLr(unit, vlan_ctrl));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_EASYRIDER_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPreferEgressTranslate:
            arg = (arg ? 1 : 0);
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, EGR_CONFIG_1r, port,
                                            DISABLE_VT_IF_IFP_CHANGE_VLANf, 
                                            arg));
                return BCM_E_NONE;
            }
#endif
            return BCM_E_UNAVAIL;
        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        bcm_port_t          port; 
        uint64              r64, o64;
        switch (type) {
        case bcmVlanDropUnknown:
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_CPU_CONTROLr(unit, port, &r64));
                o64 = r64;
                soc_reg64_field32_set(unit, CPU_CONTROLr, &r64,
                                      UVLAN_TOCPUf, arg ? 0 : 1);
                if (COMPILER_64_NE(r64, o64)) {
                    SOC_IF_ERROR_RETURN(WRITE_CPU_CONTROLr(unit, port, r64));
                }
            }
            return BCM_E_NONE;
        case bcmVlanShared:
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_VLAN_CONTROLr(unit, port, &r64));
                o64 = r64;
                soc_reg64_field32_set(unit, VLAN_CONTROLr, &r64,
                                      USE_LEARN_VIDf, arg ? 1 : 0);
                if (COMPILER_64_NE(r64, o64)) {
                    SOC_IF_ERROR_RETURN(WRITE_VLAN_CONTROLr(unit, port, r64));
                }
            }
            return BCM_E_NONE;
        case bcmVlanSharedID:
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_VLAN_CONTROLr(unit, port, &r64));
                o64 = r64;
                soc_reg64_field32_set(unit, VLAN_CONTROLr, &r64,
                                      LEARN_VIDf, arg & 0xfff);
                if (COMPILER_64_NE(r64, o64)) {
                    SOC_IF_ERROR_RETURN(WRITE_VLAN_CONTROLr(unit, port, r64));
                }
            }
            return BCM_E_NONE;
        default:
            break;
        }
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_vlan_control_get
 *
 * Purpose:
 *     Get miscellaneous VLAN-specific chip options
 *
 * Parameters:
 *     unit - StrataSwitch PCI device unit number (driver internal).
 *     type - A value from bcm_vlan_control_t enumeration list
 *     arg  - (OUT) state whose meaning is dependent on 'type'
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_PARAM    - arg points to NULL
 *     BCM_E_INTERNAL - Chip access failure.
 *     BCM_E_UNAVAIL  - type not supported on unit
 */

int
bcm_esw_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg)
{

    if (arg == 0) {
        return BCM_E_PARAM;
    }

#ifdef BCM_DRACO15_SUPPORT
    if (SOC_IS_DRACO15(unit)) {
        int                 port;
        uint32              rval;
        switch (type) {
        case bcmVlanPreferIP4:
            SOC_IF_ERROR_RETURN(READ_ARL_CONTROLr(unit, &rval));
            *arg = soc_reg_field_get(unit, ARL_CONTROLr, rval,
                                     VLAN_PRECEDENCEf);
            return BCM_E_NONE;
        case bcmVlanPreferMAC:
            SOC_IF_ERROR_RETURN(READ_ARL_CONTROLr(unit, &rval));
            *arg = !soc_reg_field_get(unit, ARL_CONTROLr, rval,
                                      VLAN_PRECEDENCEf);
            return BCM_E_NONE;
        case bcmVlanTranslate:
            PBMP_E_ITER(unit, port) {
                break;
            }
            return (_bcm_esw_port_config_get(unit, port,
                                             _bcmPortVlanTranslate, arg));
        default:        /* fall through */
            break;
        }
    }
#endif /* BCM_DRACO15_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_XGS3_FABRIC(unit)) {
        int                 port;
        uint32              vlan_ctrl;

        PBMP_E_ITER(unit, port) {
            break;
        }
        switch (type) {

        case bcmVlanDropUnknown:
            return bcm_esw_switch_control_get(unit,
                                              bcmSwitchUnknownVlanToCpu, arg);

        case bcmVlanShared:
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            *arg = soc_reg_field_get(unit, VLAN_CTRLr,
                                     vlan_ctrl, USE_LEARN_VIDf);
            return BCM_E_NONE;

        case bcmVlanSharedID:
            SOC_IF_ERROR_RETURN(READ_VLAN_CTRLr(unit, &vlan_ctrl));
            *arg = soc_reg_field_get(unit, VLAN_CTRLr,
                                     vlan_ctrl, LEARN_VIDf);
            return BCM_E_NONE;

        case bcmVlanPreferIP4:
           return (_bcm_esw_port_config_get(unit, port,
                                            _bcmPortVlanPrecedence, arg));
        case bcmVlanPreferMAC:
            SOC_IF_ERROR_RETURN
                (_bcm_esw_port_config_get(unit, port,
                                          _bcmPortVlanPrecedence, arg));
            *arg = !*arg;
            return BCM_E_NONE;
        case bcmVlanTranslate:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                 _bcmPortVlanTranslate, arg));
            } else {
                return BCM_E_UNAVAIL;
            }
        case bcmVlanIgnorePktTag:
#if defined(BCM_EASYRIDER_SUPPORT)
            if (soc_feature(unit, soc_feature_ignore_pkt_tag)) {
                SOC_IF_ERROR_RETURN(READ_DEF_VLAN_CONTROLr(unit, &vlan_ctrl));
                *arg = soc_reg_field_get(unit, DEF_VLAN_CONTROLr,
                                         vlan_ctrl, IGNORE_PKT_TAGf);
                return BCM_E_NONE;
            }
#endif /* BCM_EASYRIDER_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPreferEgressTranslate:
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                uint32 egr_cfg;
                SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &egr_cfg));
                *arg = soc_reg_field_get(unit, EGR_CONFIG_1r, egr_cfg, 
                                         DISABLE_VT_IF_IFP_CHANGE_VLANf);
                return BCM_E_NONE;
            }
#endif
            return BCM_E_UNAVAIL;
        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        int                 port;
        uint64              r64;
        switch (type) {
        case bcmVlanDropUnknown:
            COMPILER_64_ZERO(r64);
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_CPU_CONTROLr(unit, port, &r64));
                break;
            }
            *arg = !soc_reg64_field32_get(unit, CPU_CONTROLr, r64,
                                          UVLAN_TOCPUf);
            return BCM_E_NONE;
        case bcmVlanShared:
            COMPILER_64_ZERO(r64);
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_VLAN_CONTROLr(unit, port, &r64));
                break;
            }
            *arg = soc_reg64_field32_get(unit, VLAN_CONTROLr, r64,
                                         USE_LEARN_VIDf);
            return BCM_E_NONE;
        case bcmVlanSharedID:
            COMPILER_64_ZERO(r64);
            PBMP_E_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_VLAN_CONTROLr(unit, port, &r64));
                break;
            }
            *arg = soc_reg64_field32_get(unit, VLAN_CONTROLr, r64,
                                         LEARN_VIDf);
            return BCM_E_NONE;
        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */

    return BCM_E_UNAVAIL;
}

static int
_bcm_vlan_control_port_set(int unit, int port,
                           bcm_vlan_control_port_t type, int arg)
{
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_FBX(unit) || SOC_IS_EASYRIDER(unit)) {
        uint32 data;
        uint32 evc;
        soc_reg_t egr_register;

        if (SOC_IS_EASYRIDER(unit)) {
            egr_register = EGR_PORTr;
        } else {
            egr_register = EGR_VLAN_CONTROL_1r;
        }

        switch (type) {
        case bcmVlanPortPreferIP4:
            arg = (arg ? 1 : 0);
            return (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortVlanPrecedence, arg));

        case bcmVlanPortPreferMAC:
            arg = (arg ? 0 : 1);
            return (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortVlanPrecedence, arg));

        case bcmVlanTranslateIngressEnable:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortVlanTranslate, arg));
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateIngressMissDrop:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
                return (_bcm_esw_port_config_set(unit, port,
                                                _bcmPortVTMissDrop, arg));
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressEnable:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_read(unit, soc_reg_addr(unit,
                                                egr_register, port, 0), &evc));
                data = evc;
                soc_reg_field_set(unit, egr_register, &evc, VT_ENABLEf, arg);
                if (evc != data) {
                    return (soc_reg32_write(unit, soc_reg_addr(unit,
                                               egr_register, port, 0), evc));
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissDrop:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                arg = (arg ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_read(unit, soc_reg_addr(unit,
                                               egr_register, port, 0), &evc));
                data = evc;
                soc_reg_field_set(unit, egr_register, &evc, VT_MISS_DROPf, arg);
                if (evc != data) {
                    return (soc_reg32_write(unit, soc_reg_addr(unit,
                                                egr_register, port, 0), evc));
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissUntaggedDrop:
            if ((soc_feature(unit, soc_feature_untagged_vt_miss)) &&
                (soc_feature(unit,soc_feature_vlan_translation))) {
                arg = (arg ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_read(unit, soc_reg_addr(unit,
                                              egr_register, port, 0), &evc));
                data = evc;
                soc_reg_field_set(unit, egr_register, &evc, VT_MISS_UT_DROPf,
                                  arg);
                if (evc != data) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_write(unit, soc_reg_addr(unit,
                                               egr_register, port, 0), evc));
                }
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissUntag:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) || \
            defined(BCM_RAVEN_SUPPORT)
            if (SOC_REG_FIELD_VALID(unit, egr_register, VT_MISS_UNTAGf)) {
                arg = (arg ? 1 : 0);
                return soc_reg_field32_modify(unit, egr_register, port,
                                              VT_MISS_UNTAGf, arg);
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanLookupMACEnable:
            arg = (arg ? 1 : 0);
            return (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortLookupMACEnable, arg));

        case bcmVlanLookupIPEnable:
            arg = (arg ? 1 : 0);
            return (_bcm_esw_port_config_set(unit, port,
                                             _bcmPortLookupIPEnable, arg));

        case bcmVlanPortUseInnerPri:
            arg = (arg ? 1 : 0);
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_MIM_PORT(port)) {
                return (_bcm_tr2_svp_field_set(unit, port, 
                                               USE_INNER_PRIf, arg));
            }
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit) ||
                SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortUseInnerPri, arg));
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortUseOuterPri:
            arg = (arg ? 1 : 0);
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_MIM_PORT(port) || BCM_GPORT_IS_MIM_PORT(port)) {
                return (_bcm_tr2_svp_field_set(unit, port, 
                                               TRUST_OUTER_DOT1Pf, arg));
            }
#endif
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortUseOuterPri, arg));
            }
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortVerifyOuterTpid:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) \
 || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit)) {
                arg = (arg ? 1 : 0);
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortVerifyOuterTpid, arg));
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortOuterTpidSelect:

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
 || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || 
                SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit)) {
                if (arg != BCM_PORT_OUTER_TPID &&
                    arg != BCM_VLAN_OUTER_TPID) {
                    return BCM_E_PARAM;
                }
                SOC_IF_ERROR_RETURN
                    (READ_EGR_VLAN_CONTROL_1r(unit, port, &evc));
                data = evc;
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &evc,
                                      OUTER_TPID_SELf, arg);
                if (evc != data) {
                    SOC_IF_ERROR_RETURN
                        (WRITE_EGR_VLAN_CONTROL_1r(unit, port, evc));
                }
                return BCM_E_NONE;
            }

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortTranslateKeyFirst:
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                int rv, key_type = 0, use_port = 0;
                switch (arg) {
                    case bcmVlanTranslateKeyPortDouble:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyDouble:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                        break;
                    case bcmVlanTranslateKeyPortOuterTag:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyOuterTag:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_OTAG;
                        break;
                    case bcmVlanTranslateKeyPortInnerTag:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyInnerTag:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_ITAG;
                        break;
                    case bcmVlanTranslateKeyPortOuter:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyOuter:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_OVID;
                        break;
                    case bcmVlanTranslateKeyPortInner:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyInner:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_IVID;
                        break;
                    default:
                        return BCM_E_PARAM;
                }
                rv = _bcm_esw_port_config_set(unit, port,
                                              _bcmPortVTKeyTypeFirst, 
                                              key_type);
                if (rv < 0) {
                    return rv;
                }
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortVTKeyPortFirst, 
                                                 use_port));
            }
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanPortTranslateKeySecond:
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                int rv, key_type = 0, use_port = 0;
                switch (arg) {
                    case bcmVlanTranslateKeyPortDouble:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyDouble:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_IVID_OVID;
                        break;
                    case bcmVlanTranslateKeyPortOuterTag:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyOuterTag:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_OTAG;
                        break;
                    case bcmVlanTranslateKeyPortInnerTag:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyInnerTag:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_ITAG;
                        break;
                    case bcmVlanTranslateKeyPortOuter:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyOuter:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_OVID;
                        break;
                    case bcmVlanTranslateKeyPortInner:
                        use_port = 1;
                        /* fall through */
                    case bcmVlanTranslateKeyInner:
                        key_type = TR_VLXLT_HASH_KEY_TYPE_IVID;
                        break;
                    default:
                        return BCM_E_PARAM;
                }
                rv = _bcm_esw_port_config_set(unit, port,
                                              _bcmPortVTKeyTypeSecond, 
                                              key_type);
                if (rv < 0) {
                    return rv;
                }
                return (_bcm_esw_port_config_set(unit, port,
                                                 _bcmPortVTKeyPortSecond, 
                                                 use_port));
            }
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;

        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT & BCM_EASYRIDER_SUPPORT */
    return BCM_E_UNAVAIL;
}

int
bcm_esw_vlan_control_port_set(int unit, bcm_port_t port,
                              bcm_vlan_control_port_t type, int arg)
{
    if (port == -1) {
        PBMP_E_ITER(unit, port) {
            BCM_IF_ERROR_RETURN
                (_bcm_vlan_control_port_set(unit, port, type, arg));
        }
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid) ) {
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
        return _bcm_vlan_control_port_set(unit, port, type, arg);
    }

    return BCM_E_NONE;
}

int
bcm_esw_vlan_control_port_get(int unit, bcm_port_t port,
                              bcm_vlan_control_port_t type, int * arg)
{
    if (arg == 0) {
        return BCM_E_PARAM;
    }
    if (BCM_GPORT_IS_SET(port)) {
        bcm_trunk_t     tgid;
        bcm_module_t    modid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid) ) {
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_FBX(unit) || SOC_IS_EASYRIDER(unit)) {
        uint32 evc;
        soc_reg_t egr_register;

        if (SOC_IS_EASYRIDER(unit)) {
            egr_register = EGR_PORTr;
        } else {
            egr_register = EGR_VLAN_CONTROL_1r;
        }

        switch (type) {
        case bcmVlanPortPreferIP4:
            return _bcm_esw_port_config_get(unit, port, _bcmPortVlanPrecedence,
                                            arg);

        case bcmVlanPortPreferMAC:
            SOC_IF_ERROR_RETURN
                (_bcm_esw_port_config_get(unit, port, _bcmPortVlanPrecedence,
                                            arg));
            *arg = !*arg;
            return BCM_E_NONE;

        case bcmVlanTranslateIngressEnable:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                 _bcmPortVlanTranslate, arg));
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateIngressMissDrop:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                 _bcmPortVTMissDrop, arg));
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressEnable:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                SOC_IF_ERROR_RETURN
                        (soc_reg32_read(unit, soc_reg_addr(unit,
                                        egr_register, port, 0), &evc));
                *arg = soc_reg_field_get(unit, egr_register, evc, VT_ENABLEf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissDrop:
            if (soc_feature(unit, soc_feature_vlan_translation)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_read(unit, soc_reg_addr(unit,
                                    egr_register, port, 0), &evc));
                *arg = soc_reg_field_get(unit, egr_register, evc,
                                         VT_MISS_DROPf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissUntaggedDrop:
            if ((soc_feature(unit, soc_feature_untagged_vt_miss)) &&
                (soc_feature(unit,soc_feature_vlan_translation))) {
                SOC_IF_ERROR_RETURN (soc_reg32_read(unit,
                                     soc_reg_addr(unit, egr_register, port, 0),
                                     &evc));
                *arg = soc_reg_field_get(unit, egr_register, evc,
                                         VT_MISS_UT_DROPf);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;

        case bcmVlanTranslateEgressMissUntag:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) || \
            defined(BCM_RAVEN_SUPPORT)
            if (SOC_REG_FIELD_VALID(unit, egr_register, VT_MISS_UNTAGf)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_read(unit, 
                                    soc_reg_addr(unit, egr_register, port, 0),
                                    &evc));
                *arg = soc_reg_field_get(unit, egr_register, evc,
                                         VT_MISS_UNTAGf);
                return BCM_E_NONE; 
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
            return BCM_E_UNAVAIL;

        case bcmVlanLookupMACEnable:
            return _bcm_esw_port_config_get(unit, port,
                                            _bcmPortLookupMACEnable, arg);
        case bcmVlanLookupIPEnable:
            return _bcm_esw_port_config_get(unit, port,
                                            _bcmPortLookupIPEnable, arg);
        case bcmVlanPortUseInnerPri:
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_MIM_PORT(port)) {
                return (_bcm_tr2_svp_field_get(unit, port, 
                                               USE_INNER_PRIf, arg));
            }
#endif
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit) ||
                SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                _bcmPortUseInnerPri, arg));
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortUseOuterPri:
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
            if (BCM_GPORT_IS_MIM_PORT(port) || BCM_GPORT_IS_MIM_PORT(port)) {
                return (_bcm_tr2_svp_field_get(unit, port, 
                                               TRUST_OUTER_DOT1Pf, arg));
            }
#endif
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                _bcmPortUseOuterPri, arg));
            }
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortVerifyOuterTpid:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) \
 || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit)) {
                return (_bcm_esw_port_config_get(unit, port,
                                                _bcmPortVerifyOuterTpid, arg));
            }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortOuterTpidSelect:

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) \
 || defined(BCM_TRX_SUPPORT)
            if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) ||
                SOC_IS_TRX(unit) || SOC_IS_HAWKEYE(unit)) {
                SOC_IF_ERROR_RETURN
                    (READ_EGR_VLAN_CONTROL_1r(unit, port, &evc));
                *arg = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r,
                                         evc, OUTER_TPID_SELf);
                return BCM_E_NONE;
            }

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortTranslateKeyFirst:
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                int key_type = 0, use_port = 0;

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              _bcmPortVTKeyTypeFirst, 
                                              &key_type));

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              _bcmPortVTKeyPortFirst, 
                                              &use_port));
                switch (*arg) {
                    case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortDouble :
                                            bcmVlanTranslateKeyDouble;
                        break;
                    case TR_VLXLT_HASH_KEY_TYPE_OTAG:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortOuterTag :
                                            bcmVlanTranslateKeyOuterTag;
                        break;
                    case TR_VLXLT_HASH_KEY_TYPE_ITAG:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortInnerTag :
                                            bcmVlanTranslateKeyInnerTag;
                        break;
                    case TR_VLXLT_HASH_KEY_TYPE_OVID:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortOuter :
                                            bcmVlanTranslateKeyOuter;
                        break;
                    case TR_VLXLT_HASH_KEY_TYPE_IVID:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortInner :
                                            bcmVlanTranslateKeyInner;
                        break;
                    default:
                        *arg = bcmVlanTranslateKeyInvalid;
                        break;
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL;
        case bcmVlanPortTranslateKeySecond:
#if defined(BCM_TRX_SUPPORT)
            if (SOC_IS_TRX(unit)) {
                int key_type = 0, use_port = 0;

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              _bcmPortVTKeyTypeSecond,
                                              &key_type));

                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_config_get(unit, port,
                                              _bcmPortVTKeyPortSecond,
                                              &use_port));
                switch (*arg) {
                    case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortDouble :
                                            bcmVlanTranslateKeyDouble;
                        break;
                    case TR_VLXLT_HASH_KEY_TYPE_OTAG:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortOuterTag :
                                            bcmVlanTranslateKeyOuterTag;
                        break;
                    case TR_VLXLT_HASH_KEY_TYPE_ITAG:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortInnerTag :
                                            bcmVlanTranslateKeyInnerTag;
                        break;
                    case TR_VLXLT_HASH_KEY_TYPE_OVID:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortOuter :
                                            bcmVlanTranslateKeyOuter;
                        break;
                    case TR_VLXLT_HASH_KEY_TYPE_IVID:
                        *arg = (use_port) ? bcmVlanTranslateKeyPortInner :
                                            bcmVlanTranslateKeyInner;
                        break;
                    default:
                        *arg = bcmVlanTranslateKeyInvalid;
                        break;
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRX_SUPPORT */
            return BCM_E_UNAVAIL; 
        default:
            return BCM_E_UNAVAIL;
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    return BCM_E_UNAVAIL;
}

#if defined(BCM_XGS_SWITCH_SUPPORT)
/****************************************************************
 *
 * Stack port updating of VLAN tables
 *
 ****************************************************************/

/* Boolean: returns TRUE if bitmap changed. */
STATIC int
_stk_vlan_bitmap_update(int unit, bcm_pbmp_t remove_ports,
                        bcm_pbmp_t old_ports, bcm_pbmp_t *new_ports)
{
    SOC_PBMP_ASSIGN(*new_ports, old_ports);
    SOC_PBMP_OR(*new_ports, SOC_PBMP_STACK_CURRENT(unit));
    SOC_PBMP_REMOVE(*new_ports, remove_ports);

    return SOC_PBMP_NEQ(*new_ports, old_ports);
}

STATIC void
_xgs_vlan_bitmap_get(int unit, vlan_tab_entry_t *vt, bcm_pbmp_t *ports)
{
    SOC_PBMP_CLEAR(*ports);

    if (SOC_IS_DRACO(unit) || SOC_IS_LYNX(unit) || SOC_IS_XGS3_SWITCH(unit)) {
#if defined(BCM_DRACO_SUPPORT) || defined(BCM_LYNX_SUPPORT) || \
    defined(BCM_XGS3_SWITCH_SUPPORT)
        soc_mem_pbmp_field_get(unit, VLAN_TABm, vt, PORT_BITMAPf, ports);
#endif
    } else if (SOC_IS_TUCANA(unit)) {
#if defined(BCM_TUCANA_SUPPORT)
        uint32 bitmap32;

        bitmap32 = soc_VLAN_TABm_field32_get(unit, vt, PORT_BITMAP_M0f);
        SOC_PBMP_WORD_SET(*ports, 0, bitmap32);
        bitmap32 = soc_VLAN_TABm_field32_get(unit, vt, PORT_BITMAP_M1f);
        SOC_PBMP_WORD_SET(*ports, 1, bitmap32);

        
#endif
    }
}

STATIC void
_xgs_vlan_bitmap_set(int unit, vlan_tab_entry_t *vt, bcm_pbmp_t ports)
{
    if (SOC_IS_TUCANA(unit)) {
#if defined(BCM_TUCANA_SUPPORT)
        uint32 bitmap32;

        bitmap32 = SOC_PBMP_WORD_GET(ports, 0);
        soc_VLAN_TABm_field32_set(unit, vt, PORT_BITMAP_M0f, bitmap32);
        bitmap32 = SOC_PBMP_WORD_GET(ports, 1);
        soc_VLAN_TABm_field32_set(unit, vt, PORT_BITMAP_M1f, bitmap32);

        
#endif
    } else {
#if defined(BCM_DRACO_SUPPORT) || defined(BCM_LYNX_SUPPORT) || \
    defined(BCM_XGS3_SWITCH_SUPPORT)
        soc_mem_pbmp_field_set(unit, VLAN_TABm, vt, PORT_BITMAPf, &ports);
#endif
    }
}
#endif /* BCM_XGS_SWITCH_SUPPORT */

STATIC int
_xgs_vlan_stk_update(int unit, int vid, bcm_pbmp_t add_ports,
                     bcm_pbmp_t remove_ports)
{
#if defined(BCM_XGS_SWITCH_SUPPORT)

    vlan_tab_entry_t vt;
    bcm_pbmp_t old_ports, new_ports;

    /* Read in table entry.  Assume valid as given VID */
    SOC_IF_ERROR_RETURN(READ_VLAN_TABm(unit, MEM_BLOCK_ANY, vid, &vt));

    _xgs_vlan_bitmap_get(unit, &vt, &old_ports);

    /* See if changed */
    if (_stk_vlan_bitmap_update(unit, remove_ports, old_ports, &new_ports)) {
        _xgs_vlan_bitmap_set(unit, &vt, new_ports);
        SOC_IF_ERROR_RETURN(WRITE_VLAN_TABm(unit, MEM_BLOCK_ANY, vid, &vt));
    }

    /* FIREBOLT has an EGR VLAN table;
       in general, this should match VLAN_TAB */
    if (SOC_IS_FBX(unit) && soc_feature(unit, soc_feature_egr_vlan_check) ) {
#if defined(BCM_FIREBOLT_SUPPORT)
        egr_vlan_entry_t evt;
        bcm_pbmp_t old_ports, new_ports;

        /* Read in EGR_VLAN table entry.  */
        SOC_IF_ERROR_RETURN(READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vid, &evt));

        if (_bcm_vlan_valid_check(unit, EGR_VLANm, (void *) &evt, 
                                  vid) == BCM_E_NONE) {
            soc_mem_pbmp_field_get(unit, EGR_VLANm, &evt, PORT_BITMAPf,
                                   &old_ports);

            /* See if changed */
            if (_stk_vlan_bitmap_update(unit, remove_ports,
                                        old_ports, &new_ports)) {
                soc_mem_pbmp_field_set(unit, EGR_VLANm, &evt, PORT_BITMAPf,
                                       &new_ports);
                SOC_IF_ERROR_RETURN(WRITE_EGR_VLANm(unit, MEM_BLOCK_ANY,
                                                    vid, &evt));
            }
        }
#endif /* BCM_FIREBOLT_SUPPORT */
    }

#endif /* BCM_XGS_SWITCH_SUPPORT */

    return BCM_E_NONE;
}

STATIC int
_esw_vlan_stk_update(int unit, int vid, bcm_pbmp_t add_ports,
                     bcm_pbmp_t remove_ports)
{
    VLAN_VERB(("VLAN %d: stk update internal: vid %d, add 0x%x, "
               "rmv 0x%x\n", unit, vid,
               SOC_PBMP_WORD_GET(add_ports, 0),
               SOC_PBMP_WORD_GET(remove_ports, 0)));

    if (SOC_IS_XGS_SWITCH(unit)) {
        return _xgs_vlan_stk_update(unit, vid, add_ports, remove_ports);
    }

    return BCM_E_NONE;  /* Ignore where not supported. */
}

STATIC void
_vlan_mem_lock(int unit)
{
#ifdef  BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        soc_mem_lock(unit, VLAN_TABm);
    }
#endif

#ifdef  BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        soc_mem_lock(unit, VLAN_TABm);
        if (SOC_IS_EASYRIDER(unit)) {
            soc_mem_lock(unit, EGR_VLAN_XLATEm);
        } else {
            soc_mem_lock(unit, EGR_VLANm);
        }
        
    }
#endif
}

STATIC void
_vlan_mem_unlock(int unit)
{
#ifdef  BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        soc_mem_unlock(unit, VLAN_TABm);
    }
#endif

#ifdef  BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (SOC_IS_EASYRIDER(unit)) {
            soc_mem_unlock(unit, EGR_VLAN_XLATEm);
        } else {
            soc_mem_unlock(unit, EGR_VLANm);
        }

        soc_mem_unlock(unit, VLAN_TABm);
    }
#endif
}



int
_bcm_esw_vlan_untag_update(int unit, bcm_port_t port, int untag)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t port_pbmp, port_ubmp, vlan_pbmp, vlan_ubmp;
    int vid;

    if (!soc_feature(unit, soc_feature_xport_convertible) && !SOC_IS_RAVEN(unit)) {
        return BCM_E_NONE;
    }

    SOC_PBMP_CLEAR(port_pbmp);
    SOC_PBMP_CLEAR(port_ubmp);

    SOC_PBMP_PORT_SET(port_pbmp, port);
    if (!untag) {
        SOC_PBMP_PORT_SET(port_ubmp, port);
    }

    _vlan_mem_lock(unit);
    vid = vlan_info[unit].defl;
    rv = mbcm_driver[unit]->mbcm_vlan_port_get(unit, vid,
                                               &vlan_pbmp, &vlan_ubmp);

    if (rv >= 0) {
        if (untag || SOC_PBMP_MEMBER(vlan_pbmp, port)) {
            rv = mbcm_driver[unit]->
                mbcm_vlan_port_add(unit, vid, port_pbmp, port_ubmp);
        }
    }
    _vlan_mem_unlock(unit);

    return rv;
}



int
_bcm_esw_vlan_stk_update(int unit, uint32 flags)
{
    int rv = BCM_E_NONE;

#if !defined(BCM_VLAN_NO_AUTO_STACK)
    bcm_pbmp_t add_ports, remove_ports;
    int vid;
    int vlan_count = 0;

    VLAN_VERB(("VLAN %d: stk_update: flags 0x%x\n",
               unit, flags));

    /*
     * Force all stack ports in all VLANs and assume BLOCK registers
     * take care of DLFs and BCast; remove ports no longer stacking.
     */
    SOC_PBMP_ASSIGN(add_ports, SOC_PBMP_STACK_CURRENT(unit));

    SOC_PBMP_ASSIGN(remove_ports, SOC_PBMP_STACK_PREVIOUS(unit));
    SOC_PBMP_REMOVE(remove_ports, SOC_PBMP_STACK_CURRENT(unit));

    _vlan_mem_lock(unit);

    for (vid = BCM_VLAN_MIN;vid < BCM_VLAN_COUNT; vid++) {
        if (0 == _BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
            continue;
        }
        vlan_count++;
        rv = _esw_vlan_stk_update(unit, vid, add_ports, remove_ports);
        if (BCM_FAILURE(rv)) {
            break;
        }
        if (vlan_info[unit].count == vlan_count) {
            break;
        }
    }
    _vlan_mem_unlock(unit);
#endif
    return rv;
}


/*
 * Function:
 *      bcm_vlan_mcast_flood_set
 * Purpose:
 *      Set the VLAN Multicast flood mode (aka PFM - port filter mode)
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid  - VLAN to change the flood setting on.
 *      mode - Multicast flood mode
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int
bcm_esw_vlan_mcast_flood_set(int unit,
                             bcm_vlan_t vlan,
                             bcm_vlan_mcast_flood_t mode)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_mc_flood_ctrl)) {
        CHECK_INIT(unit);
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (_BCM_MPLS_VPN_IS_VPLS(vlan)) {
        if ((SOC_IS_TRIUMPH (unit) ) || (SOC_IS_VALKYRIE(unit))) {
            return bcm_tr_mpls_mcast_flood_set(unit, vlan, mode);
        } else if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH2(unit) ||
                   SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) {
            return rv;
        }
    } 
#endif
        CHECK_VID(unit, vlan);

        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
            rv = _bcm_xgs3_vlan_mcast_flood_set(unit, vlan, mode);
        } else {
            rv = BCM_E_PARAM;
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_vlan_mcast_flood_get
 * Purpose:
 *      Get the VLAN Multicast flood mode (aka PFM - port filter mode)
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vid  - VLAN to get the flood setting for.
 *      mode - Multicast flood mode
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int
bcm_esw_vlan_mcast_flood_get(int unit,
                             bcm_vlan_t vlan,
                             bcm_vlan_mcast_flood_t *mode)
{
   int rv = BCM_E_UNAVAIL;
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_mc_flood_ctrl)) {
        CHECK_INIT(unit);
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (_BCM_MPLS_VPN_IS_VPLS(vlan)) {
        if ((SOC_IS_TRIUMPH (unit) ) || (SOC_IS_VALKYRIE(unit))) {
            return bcm_tr_mpls_mcast_flood_get(unit, vlan, mode);
        } else if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH2(unit) ||
                   SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) {
            return rv;
        }
    } 
#endif
        CHECK_VID(unit, vlan);
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
            rv = _bcm_xgs3_vlan_mcast_flood_get(unit, vlan, mode);
        } else {
            rv = BCM_E_PARAM;
        }
    }
#endif
    return rv;
}


/*
 * Function:
 *      bcm_esw_vlan_block_set
 * Purpose:
 *      Set per VLAN configuration.
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      vid     - (IN) VLAN to get the flood setting for.
 *      block   - (IN) VLAN block structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_block_set(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_TRX_SUPPORT
    CHECK_INIT(unit);
    CHECK_VID(unit, vid);
    if (SOC_MEM_IS_VALID(unit, VLAN_PROFILE_2m))  {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
            BCM_LOCK(unit);
            rv = _bcm_trx_vlan_block_set(unit, vid, block);
            BCM_UNLOCK(unit);
        } else {
            rv = BCM_E_PARAM;
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_block_get
 * Purpose:
 *      Get per VLAN block configuration.
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      vid     - (IN) VLAN to get the flood setting for.
 *      control - (OUT) VLAN control structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_block_get(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    int rv = BCM_E_UNAVAIL;

    if (NULL == block) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_TRX_SUPPORT
    CHECK_INIT(unit);
    CHECK_VID(unit, vid);
    if (SOC_MEM_IS_VALID(unit, VLAN_PROFILE_2m)) {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
            BCM_LOCK(unit);
            rv = _bcm_trx_vlan_block_get(unit, vid, block);
            BCM_UNLOCK(unit);
        } else {
            rv = BCM_E_PARAM;
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_control_vlan_set
 * Purpose:
 *      Set per VLAN configuration.
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid     - (IN) VLAN to get the flood setting for.
 *      control - (IN) VLAN control structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
                              bcm_vlan_control_vlan_t control)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_XGS3_SWITCH_SUPPORT
    CHECK_INIT(unit);
    CHECK_VID(unit, vid);
    if ((control.vrf > SOC_VRF_MAX(unit))) {
        return (BCM_E_PARAM);
    }
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {

        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
            BCM_LOCK(unit);
            rv = _bcm_xgs3_vlan_control_vlan_set(unit, vid, &control);
            BCM_UNLOCK(unit);
        } else {
            rv = BCM_E_PARAM;
        }
    }
#endif
#if defined(BCM_EASYRIDER_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_EASYRIDER(unit)) {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
            BCM_LOCK(unit);
            rv = _bcm_er_vlan_control_vlan_set(unit, vid, &control);
            BCM_UNLOCK(unit);
        } 
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_vlan_control_vlan_get
 * Purpose:
 *      Get per VLAN configuration.
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vid     - (IN) VLAN to get the flood setting for.
 *      control - (OUT) VLAN control structure
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_vlan_control_vlan_get(int unit, bcm_vlan_t vid,
                              bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_UNAVAIL;

    if (NULL == control) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    CHECK_INIT(unit);
    CHECK_VID(unit, vid);
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vid)) {
            BCM_LOCK(unit);
            rv = _bcm_xgs3_vlan_control_vlan_get(unit, vid, control);
            BCM_UNLOCK(unit);
        } else {
            rv = BCM_E_PARAM;
        }
    }
#endif
#if defined(BCM_EASYRIDER_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_EASYRIDER(unit)) {
        if (soc_feature(unit, soc_feature_mpls_per_vlan)) {
            int ena;
            bcm_vlan_control_vlan_t_init(control);
            rv = _bcm_mpls_vlan_enable_get(unit, vid, &ena);
            if (rv == BCM_E_NONE) {
                control->flags |= ((ena == TRUE) ? 0 : BCM_VLAN_MPLS_DISABLE);
            }
        }
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_add
 * Purpose:
 *      Add a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 *      port_1     - First port in the cross-connect
 *      port_2     - Second port in the cross-connect
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_cross_connect_add(int unit, 
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                               bcm_gport_t port_1, bcm_gport_t port_2)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_l2_cross_connect_add(unit, outer_vlan, inner_vlan,
                                           port_1, port_2);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete
 * Purpose:
 *      Delete a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      inner_vlan - Inner vlan ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_cross_connect_delete(int unit,
                                  bcm_vlan_t outer_vlan, 
                                  bcm_vlan_t inner_vlan)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_l2_cross_connect_delete(unit, outer_vlan, inner_vlan);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_delete_all
 * Purpose:
 *      Delete all VLAN cross connect entries
 * Parameters:
 *      unit       - Device unit number
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_cross_connect_delete_all(int unit)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_l2_cross_connect_delete_all(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_vlan_cross_connect_traverse
 * Purpose:
 *      Walks through the valid cross connect entries and calls 
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
bcm_esw_vlan_cross_connect_traverse(int unit,
                                    bcm_vlan_cross_connect_traverse_cb cb,
                                    void *user_data)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return bcm_tr_l2_cross_connect_traverse(unit, cb, user_data);
    }
#endif
    return BCM_E_UNAVAIL;
}

#if defined(BCM_TRIUMPH2_SUPPORT)
STATIC int
_bcm_xgs3_vlan_table_hw_fs_set(int unit, bcm_vlan_t vid,
                               int fs_idx, int table)
{
    vlan_tab_entry_t    vt;
    int                 rv;

    /* Upper layer already checks that vid is valid */
    soc_mem_lock(unit, table);

    rv = soc_mem_read(unit, table, MEM_BLOCK_ANY, (int) vid, &vt);

    if (BCM_SUCCESS(rv) && !soc_mem_field32_get(unit, table, &vt, VALIDf)) {
        rv = BCM_E_NOT_FOUND;
    }

    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, table, &vt, SERVICE_CTR_IDXf, fs_idx);

        rv = soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid, &vt);
    }

    soc_mem_unlock(unit, table);

    return rv;
}

int
_bcm_esw_vlan_flex_stat_index_set(int unit, bcm_vlan_t vlan, int fs_idx)
{
    int rv;

    BCM_LOCK(unit); /* To access the VLAN existence bitmap */
    if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
        rv = _bcm_xgs3_vlan_table_hw_fs_set(unit, vlan, fs_idx, VLAN_TABm);
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_xgs3_vlan_table_hw_fs_set(unit, vlan,
                                                fs_idx, EGR_VLANm);
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    BCM_UNLOCK(unit);
    return rv;
}

STATIC int
_bcm_esw_vlan_stat_param_valid(int unit, bcm_vlan_t vlan, bcm_cos_t cos)
{
#if defined(INCLUDE_L3)
    int vfi;
#endif /* INCLUDE_L3 */

    CHECK_INIT(unit);

    if (!soc_feature(unit, soc_feature_gport_service_counters)) {
        return BCM_E_UNAVAIL;
    }

#if defined(INCLUDE_L3)
    /* Is it a VPN? */
    if (_BCM_MPLS_VPN_IS_SET(vlan)) {
        vfi = _BCM_MPLS_VPN_ID_GET(vlan);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMpls)) {
            return BCM_E_NOT_FOUND;
        }
    } else if (_BCM_MIM_VPN_IS_SET(vlan)) {
        vfi = _BCM_MIM_VPN_ID_GET(vlan);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            return BCM_E_NOT_FOUND;
        }
    } else 
#endif /* INCLUDE_L3 */
    {
        CHECK_VID(unit, vlan);
    }

    if (cos != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC _bcm_flex_stat_t
_bcm_esw_vlan_stat_to_flex_stat(bcm_vlan_stat_t stat)
{
    _bcm_flex_stat_t flex_stat;

    switch (stat) {
    case bcmVlanStatIngressPackets:
        flex_stat = _bcmFlexStatIngressPackets;
        break;
    case bcmVlanStatIngressBytes:
        flex_stat = _bcmFlexStatIngressBytes;
        break;
    case bcmVlanStatEgressPackets:
        flex_stat = _bcmFlexStatEgressPackets;
        break;
    case bcmVlanStatEgressBytes:
        flex_stat = _bcmFlexStatEgressBytes;
        break;
    default:
        flex_stat = _bcmFlexStatNum;
    }

    return flex_stat;
}

/* Requires "idx" variable */
#define BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr) \
    for (idx = 0; idx < nstat; idx++) { \
        if (NULL == value_arr + idx) { \
            return (BCM_E_PARAM); \
        } \
    }

STATIC int
_bcm_vlan_stat_array_convert(int unit, int nstat, bcm_vlan_stat_t *stat_arr, 
                             _bcm_flex_stat_t *fs_arr)
{
    int idx;

    if ((nstat <= 0) || (nstat > _bcmFlexStatNum)) {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < nstat; idx++) {
        if (NULL == stat_arr + idx) {
            return (BCM_E_PARAM);
        }
        fs_arr[idx] = _bcm_esw_vlan_stat_to_flex_stat(stat_arr[idx]);
    }
    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      bcm_esw_vlan_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated VLAN.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_enable_set(int unit, bcm_vlan_t vlan, int enable)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, BCM_COS_INVALID));
    return _bcm_esw_flex_stat_enable_set(unit, _bcmFlexStatTypeService,
                             _bcm_esw_vlan_flex_stat_hw_index_set,
                                         NULL, vlan, enable);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_get
 * Purpose:
 *      Extract per-VLAN statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_get(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                      bcm_vlan_stat_t stat, uint64 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    return _bcm_esw_flex_stat_get(unit, _bcmFlexStatTypeService, vlan,
                           _bcm_esw_vlan_stat_to_flex_stat(stat), val);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_get32
 * Purpose:
 *      Extract per-VLAN statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_get32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                        bcm_vlan_stat_t stat, uint32 *val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    return _bcm_esw_flex_stat_get32(unit, _bcmFlexStatTypeService, vlan,
                           _bcm_esw_vlan_stat_to_flex_stat(stat), val);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_set
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified VLAN.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_set(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                      bcm_vlan_stat_t stat, uint64 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    return _bcm_esw_flex_stat_set(unit, _bcmFlexStatTypeService, vlan,
                           _bcm_esw_vlan_stat_to_flex_stat(stat), val);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_set32
 * Purpose:
 *      Set the specified statistic to the indicated value for the
 *      specified VLAN.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_set32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                        bcm_vlan_stat_t stat, uint32 val)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    return _bcm_esw_flex_stat_set32(unit, _bcmFlexStatTypeService, vlan,
                           _bcm_esw_vlan_stat_to_flex_stat(stat), val);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_multi_get(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                            int nstat, bcm_vlan_stat_t *stat_arr, 
                            uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr));
    BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_get(unit, _bcmFlexStatTypeService, vlan,
                                        nstat, fs_arr, value_arr);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple VLAN statistic
 *      types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_multi_get32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                              int nstat,  bcm_vlan_stat_t *stat_arr, 
                              uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr));
    BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_get32(unit, _bcmFlexStatTypeService, vlan,
                                          nstat, fs_arr, value_arr);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_multi_set(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                            int nstat, bcm_vlan_stat_t *stat_arr, 
                            uint64 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr));
    BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_set(unit, _bcmFlexStatTypeService, vlan,
                                        nstat, fs_arr, value_arr);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_esw_vlan_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple VLAN statistic
 *      types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN Id.
 *      cos - (IN) COS or priority
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_vlan_stat_multi_set32(int unit, bcm_vlan_t vlan, bcm_cos_t cos, 
                              int nstat, bcm_vlan_stat_t *stat_arr, 
                              uint32 *value_arr)
{
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_t fs_arr[_bcmFlexStatNum]; /* Normalize stats */
    int idx;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_stat_param_valid(unit, vlan, cos));
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_stat_array_convert(unit, nstat, stat_arr, fs_arr));
    BCM_VLAN_VALUE_ARRAY_VALID(unit, nstat, value_arr);

    return _bcm_esw_flex_stat_multi_set32(unit, _bcmFlexStatTypeService,
                                          vlan, nstat, fs_arr, value_arr);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *     _bcm_vlan_queue_map_id_check
 * Description:
 *     To check the status of the specified qmid
 * Parameters:
 *     unit            device number
 *     qmid            queue mapping identifier
 * Return:
 *     BCM_E_UNAVAIL   queue mapping feature is not supported for this device
 *     BCM_E_INIT      vlan component is not initialized on this device
 *     BCM_E_BADID     specified qmid is not in valid range
 *     BCM_E_NOT_FOUND spefified qmid is in valid range but not in used
 *     BCM_E_NONE      specified qmid is in used
 */
STATIC int
_bcm_vlan_queue_map_id_check(int unit,
                             int qmid)
{
    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);

    if (qmid < 0 || qmid >= (soc_mem_index_count(unit, PHB2_COS_MAPm >> 4))) {
        return BCM_E_BADID;
    }

    if (!SHR_BITGET(vlan_info[unit].qm_bmp, qmid)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/* Create a VLAN queue map entry. */
int
bcm_esw_vlan_queue_map_create(int unit,
                              uint32 flags,
                              int *qmid)
{
    bcm_vlan_info_t *vi;
    int size, bmp_size, free_qmid, i;
    uint32 bits, tag_flags;

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);

    vi = &vlan_info[unit];
    size = soc_mem_index_count(unit, PHB2_COS_MAPm) >> 4;

    if (flags & BCM_VLAN_QUEUE_MAP_REPLACE) {
        if (!(flags & BCM_VLAN_QUEUE_MAP_WITH_ID)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, *qmid));
    }
    tag_flags = flags & (BCM_VLAN_QUEUE_MAP_INNER_PKT_PRI |
                         BCM_VLAN_QUEUE_MAP_OUTER_PKT_PRI);
    if (tag_flags != BCM_VLAN_QUEUE_MAP_OUTER_PKT_PRI &&
        tag_flags != BCM_VLAN_QUEUE_MAP_INNER_PKT_PRI) {
        return BCM_E_PARAM;
    }

    if (!(flags & BCM_VLAN_QUEUE_MAP_REPLACE)) {
        free_qmid = size;
        bmp_size = _SHR_BITDCLSIZE(size);
        /* find the index of first zero from qm_bmp */
        for (i = 0; i < bmp_size; i++) {
            if (vi->qm_bmp[i] == 0xffffffff) {
                continue;
            }
            bits = vi->qm_bmp[i];
            bits &= (bits << 1) | 0x00000001;
            bits &= (bits << 2) | 0x00000003;
            bits &= (bits << 4) | 0x0000000f;
            bits &= (bits << 8) | 0x000000ff;
            bits &= (bits << 16) | 0x0000ffff;
            free_qmid = (i << 5) + _shr_popcount(bits);
            break;
        }
        if (free_qmid >= size) {
            return BCM_E_RESOURCE;
        }
        SHR_BITSET(vi->qm_bmp, free_qmid);
        *qmid = free_qmid;
    }

    if (flags & BCM_VLAN_QUEUE_MAP_INNER_PKT_PRI) {
        SHR_BITSET(vi->qm_it_bmp, *qmid);
    } else {
        SHR_BITCLR(vi->qm_it_bmp, *qmid);
    }

    return BCM_E_NONE;
}

/* Delete a VLAN queue map entry. */
int
bcm_esw_vlan_queue_map_destroy(int unit,
                               int qmid)
{
    BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, qmid));

    SHR_BITCLR(vlan_info[unit].qm_bmp, qmid);

    return BCM_E_NONE;
}

/* Delete all VLAN queue map entries. */
int
bcm_esw_vlan_queue_map_destroy_all(int unit)
{
    int size;

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

    CHECK_INIT(unit);

    size = soc_mem_index_count(unit, PHB2_COS_MAPm);

    sal_memset(vlan_info[unit].qm_bmp, 0, SHR_BITALLOCSIZE(size));

    return BCM_E_NONE;
}

/* Set a VLAN queue map entry. */
int
bcm_esw_vlan_queue_map_set(int unit,
                           int qmid,
                           int pkt_pri,
                           int cfi,
                           int queue,
                           int color)
{
    phb2_cos_map_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, qmid));

    if (pkt_pri < 0 || pkt_pri > 7 || cfi < 0 || cfi > 1) {
        return BCM_E_PARAM;
    }

    if (queue < 0 ||
        queue >= (1 << soc_mem_field_length(unit, PHB2_COS_MAPm, COSf))) {
        return BCM_E_PARAM;
    }

    sal_memset(&entry, 0, sizeof(entry));
    soc_mem_field32_set(unit, PHB2_COS_MAPm, &entry, COSf, queue);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, PHB2_COS_MAPm, MEM_BLOCK_ANY,
                                      (qmid << 4) | (pkt_pri << 1) | cfi,
                                      &entry));

    return BCM_E_NONE;
}

/* Get a VLAN queue map entry. */
int
bcm_esw_vlan_queue_map_get(int unit,
                           int qmid,
                           int pkt_pri,
                           int cfi,
                           int *queue,
                           int *color)
{
    phb2_cos_map_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, qmid));

    if (pkt_pri < 0 || pkt_pri > 7 || cfi < 0 || cfi > 1) {
        return BCM_E_PARAM;
    }

    if (queue == NULL || color == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, PHB2_COS_MAPm, MEM_BLOCK_ANY,
                                     (qmid << 4) | (pkt_pri << 1) | cfi,
                                     &entry));
    *queue = soc_mem_field32_get(unit, PHB2_COS_MAPm, &entry, COSf);
    *color = 0;

    return BCM_E_NONE;
}

/* Attach a queue map object to a VLAN or VFI. */
int
bcm_esw_vlan_queue_map_attach(int unit,
                              bcm_vlan_t vlan,
                              int qmid)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    vfi_entry_t vfi_entry;
    int use_inner_tag;
#endif /* BCM_TRIUMHP2_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_vlan_queue_map_id_check(unit, qmid));

#ifdef BCM_TRIUMPH2_SUPPORT
    CHECK_VID(unit, vlan);

    use_inner_tag = SHR_BITGET(vlan_info[unit].qm_it_bmp, qmid) ? 1 : 0;
    BCM_IF_ERROR_RETURN
        (_bcm_tr2_vlan_qmid_set(unit, vlan, qmid, use_inner_tag));

    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ANY, vlan, &vfi_entry));
    soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_DOT1P_MAPPING_PTRf, qmid);
    soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_USE_INNER_DOT1Pf,
                        use_inner_tag);
    soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_ENABLEf, 1);
    BCM_IF_ERROR_RETURN(WRITE_VFIm(unit, MEM_BLOCK_ANY, vlan, &vfi_entry));
#endif /* BCM_TRIUMHP2_SUPPORT */

    return BCM_E_NONE;
}

/* Get the queue map object which is attached to a VLAN or VFI. */
int
bcm_esw_vlan_queue_map_attach_get(int unit,
                                  bcm_vlan_t vlan,
                                  int *qmid)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    int use_inner_tag;
#endif /* BCM_TRIUMHP2_SUPPORT */

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    CHECK_INIT(unit);

    CHECK_VID(unit, vlan);

    if (qmid == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_vlan_qmid_get(unit, vlan, qmid, &use_inner_tag));
#endif /* BCM_TRIUMHP2_SUPPORT */

    return BCM_E_NONE;
}

/* Detach a queue map object from a VLAN or VFI. */
int
bcm_esw_vlan_queue_map_detach(int unit,
                              bcm_vlan_t vlan)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    vfi_entry_t vfi_entry;
#endif /* BCM_TRIUMHP2_SUPPORT */

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    CHECK_INIT(unit);

    CHECK_VID(unit, vlan);

    BCM_IF_ERROR_RETURN(_bcm_tr2_vlan_qmid_set(unit, vlan, -1, 0));

    BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ANY, vlan, &vfi_entry));
    soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_DOT1P_MAPPING_PTRf, 0);
    soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_USE_INNER_DOT1Pf, 0);
    soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_ENABLEf, 0);
    BCM_IF_ERROR_RETURN(WRITE_VFIm(unit, MEM_BLOCK_ANY, vlan, &vfi_entry));
#endif /* BCM_TRIUMHP2_SUPPORT */

    return BCM_E_NONE;
}

/* Detach queue map objects from all VLAN or VFI. */
int
bcm_esw_vlan_queue_map_detach_all(int unit)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    vfi_entry_t vfi_entry;
    int vlan;
#endif /* BCM_TRIUMHP2_SUPPORT */

    if (!soc_feature(unit, soc_feature_vlan_queue_map)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    CHECK_INIT(unit);

    for (vlan = BCM_VLAN_MIN; vlan < BCM_VLAN_COUNT; vlan++) {
        if (_BCM_VBMP_LOOKUP(vlan_info[unit].bmp, vlan)) {
            BCM_IF_ERROR_RETURN(_bcm_tr2_vlan_qmid_set(unit, vlan, -1, 0));

            BCM_IF_ERROR_RETURN(READ_VFIm(unit, MEM_BLOCK_ANY, vlan,
                                          &vfi_entry));
            soc_mem_field32_set(unit, VFIm, &vfi_entry,
                                PHB2_DOT1P_MAPPING_PTRf, 0);
            soc_mem_field32_set(unit, VFIm, &vfi_entry,
                                PHB2_USE_INNER_DOT1Pf, 0);
            soc_mem_field32_set(unit, VFIm, &vfi_entry, PHB2_ENABLEf, 0);
            BCM_IF_ERROR_RETURN(WRITE_VFIm(unit, MEM_BLOCK_ANY, vlan,
                                           &vfi_entry));
        }
    }
#endif /* BCM_TRIUMHP2_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_gport_add
 * Purpose:
 *      Add a Gport to the specified vlan. Adds WLAN ports to the broadcast,
 *      multicast and unknown unicast groups.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vlan - VLAN ID to add port to as a member.
 *      port - Gport ID
 *      is_untagged - untagged or not
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */
int
bcm_esw_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t port, 
                       int is_untagged)
{
    int rv = BCM_E_NONE;

    CHECK_INIT(unit);
    CHECK_VID(unit, vlan);

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan) && BCM_GPORT_IS_WLAN_PORT(port)) {
        int i, vp, mc_idx, gport_id, mod_local, group = 0;
        vlan_tab_entry_t vtab;
        bcm_wlan_port_t wlan_port;
        bcm_module_t mod_out;
        bcm_port_t port_out;
        bcm_trunk_t trunk_id;
        bcm_gport_t local_gport;
        bcm_if_t encap_id;
        bcm_pbmp_t vlan_pbmp, vlan_ubmp, pbmp, l3_pbmp;
        soc_field_t group_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};

        vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_NOT_FOUND;
        }
        if (is_untagged) {
            return BCM_E_PARAM;
        }
        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        soc_mem_lock(unit, VLAN_TABm);

        if ((rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, 
             &vtab)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_NOT_FOUND;
        }

        /* Get the VP attributes (need physical port) and encap id */
        if ((rv = bcm_esw_wlan_port_get(unit, port, &wlan_port)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Enable VP switching on the VLAN */
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            soc_mem_field32_set(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf, 1);
            if ((rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, (int)vlan, 
                 &vtab)) < 0) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            /* Also need to copy the physical port members to the L2_BITMAP of
             * the IPMC entry for each group once we've gone virtual */
            rv = mbcm_driver[unit]->mbcm_vlan_port_get(unit, vlan,
                                                       &vlan_pbmp, &vlan_ubmp);
            if (rv < 0) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            /* Deal with each group */
            for (i = 0; i < 3; i++) {
                mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, 
                                             group_type[i]);
                rv = _bcm_tr2_multicast_ipmc_read(unit, mc_idx, &pbmp, 
                                                  &l3_pbmp);
                if (rv < 0) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
                rv = _bcm_tr2_multicast_ipmc_write(unit, mc_idx, vlan_pbmp, 
                                                   l3_pbmp, TRUE);
                if (rv < 0) {
                    soc_mem_unlock(unit, VLAN_TABm);
                    return rv;
                }
            }
        } 

        /* Check if wlan_port.port is a local port or trunk */
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            trunk_id = BCM_GPORT_TRUNK_GET(wlan_port.port);
            rv = _bcm_trunk_id_validate(unit, trunk_id);
            if (BCM_FAILURE(rv)) {
                  soc_mem_unlock(unit, VLAN_TABm);
                  return (BCM_E_PORT);
            }
            local_gport = wlan_port.port;
        } else {
            rv = _bcm_esw_gport_resolve(unit, wlan_port.port, &mod_out, &port_out,
    	  			        &trunk_id, &gport_id); 
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return (BCM_E_PORT);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            if (TRUE != mod_local) {
                /* Only add this to replication set if destination is local */
               soc_mem_unlock(unit, VLAN_TABm);
               return BCM_E_PORT;
            }
            /* Convert system local_port to physical local_port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
            rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }        
        }
        rv = bcm_esw_multicast_wlan_encap_get(unit, group, local_gport,
                                              port, &encap_id);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Add the VP to the BC group */
        mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
        rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 
        if (rv < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Add the VP to the UMC group */
        mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UMC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
        rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 
        if (rv < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Add the VP to the UUC group */
        mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, UUC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
        rv = bcm_esw_multicast_egress_add(unit, group, local_gport, encap_id); 

        soc_mem_unlock(unit, VLAN_TABm);
    } else
#endif
    {
        bcm_pbmp_t pbmp, ubmp;
        bcm_port_t local_port;

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &local_port));
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(ubmp);
        BCM_PBMP_PORT_ADD(pbmp, local_port);
        if (is_untagged) {
            BCM_PBMP_PORT_ADD(ubmp, local_port);
        }
        rv = bcm_esw_vlan_port_add(unit, vlan, pbmp, ubmp);
    }

    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_delete
 * Purpose:
 *      Delete a Gport from the specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vlan - VLAN ID to add port to as a member.
 *      port - Gport ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */
int
bcm_esw_vlan_gport_delete(int unit, bcm_vlan_t vlan, bcm_gport_t port)
{
    int rv = BCM_E_NONE;

    CHECK_INIT(unit);
    CHECK_VID(unit, vlan);

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan) && BCM_GPORT_IS_WLAN_PORT(port)) {
        int vp, mc_idx, gport_id, group = 0, mod_local;
        vlan_tab_entry_t vtab;
        bcm_wlan_port_t wlan_port;
        bcm_module_t mod_out;
        bcm_port_t port_out;
        bcm_trunk_t trunk_id;
        bcm_gport_t local_gport;
        bcm_if_t encap_id;

        vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_NOT_FOUND;
        }
        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        soc_mem_lock(unit, VLAN_TABm);

        if ((rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, 
             &vtab)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_NOT_FOUND;
        }

        /* Get the VP attributes (need physical port) and encap id */
        if ((rv = bcm_esw_wlan_port_get(unit, port, &wlan_port)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        /* Check if wlan_port.port is a local port or trunk */
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            trunk_id = BCM_GPORT_TRUNK_GET(wlan_port.port);
            rv = _bcm_trunk_id_validate(unit, trunk_id);
            if (BCM_FAILURE(rv)) {
                  soc_mem_unlock(unit, VLAN_TABm);
                  return (BCM_E_PORT);
            }
            local_gport = wlan_port.port;
        } else {
            rv = _bcm_esw_gport_resolve(unit, wlan_port.port, &mod_out, &port_out,
    	  			        &trunk_id, &gport_id); 
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return (BCM_E_PORT);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            if (TRUE != mod_local) {
                /* Only add this to replication set if destination is local */
               soc_mem_unlock(unit, VLAN_TABm);
               return BCM_E_PORT;
            }
            /* Convert system local_port to physical local_port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
            rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }        
        }
        rv = bcm_esw_multicast_wlan_encap_get(unit, group, local_gport,
                                              port, &encap_id);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }

        /* Remove from the WLAN MC group */
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            /* No group exists - this is an error*/
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_PORT;
        } 
        mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
        rv = bcm_esw_multicast_egress_delete(unit, group, local_gport, 
                                              encap_id); 

        soc_mem_unlock(unit, VLAN_TABm);
    } else
#endif
    {
        bcm_pbmp_t pbmp;
        bcm_port_t local_port;

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &local_port));
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, local_port);
        rv = bcm_esw_vlan_port_remove(unit, vlan, pbmp);
    }
    return rv;
}

/*
 * Function:
 *      bcm_vlan_gport_delete_all
 * Purpose:
 *      Delete a Gport from the specified vlan.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      vlan - VLAN ID to add port to as a member.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_INTERNAL - Chip access failure.
 *      BCM_E_NOT_FOUND - VLAN ID not in use.
 */
int
bcm_esw_vlan_gport_delete_all(int unit, bcm_vlan_t vlan)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;

    CHECK_INIT(unit);
    CHECK_VID(unit, vlan);

    soc_mem_lock(unit, VLAN_TABm);

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan)) {
        int group, mc_idx;
        vlan_tab_entry_t vtab;

        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        if ((rv = soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, 
             &vtab)) < 0) {
            soc_mem_unlock(unit, VLAN_TABm);
            return rv;
        }
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            soc_mem_unlock(unit, VLAN_TABm);
            return BCM_E_NOT_FOUND;
        }

        /* Remove from the WLAN MC group */
        if (soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            /* Remove all VP members and delete the group */
            mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
            _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);
            rv = bcm_esw_multicast_egress_delete_all(unit, group); 
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            rv = bcm_esw_multicast_destroy(unit, group);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
            soc_mem_field32_set(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf, 0);
            soc_mem_field32_set(unit, VLAN_TABm, &vtab, BC_IDXf, 0);
            soc_mem_field32_set(unit, VLAN_TABm, &vtab, UMC_IDXf, 0);
            soc_mem_field32_set(unit, VLAN_TABm, &vtab, UUC_IDXf, 0);
            if ((rv = soc_mem_write(unit, VLAN_TABm, MEM_BLOCK_ALL, (int)vlan, 
                 &vtab)) < 0) {
                soc_mem_unlock(unit, VLAN_TABm);
                return rv;
            }
        }
    } 
#endif
    /* Now remove all local physical ports */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    rv = bcm_esw_vlan_port_get(unit, vlan, &pbmp, &ubmp);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }
    rv = bcm_esw_vlan_port_remove(unit, vlan, pbmp);
    soc_mem_unlock(unit, VLAN_TABm);
    return rv;
}

int
bcm_esw_vlan_gport_get(int unit, bcm_vlan_t vlan, bcm_gport_t port, 
                       int *is_untagged)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;

    CHECK_INIT(unit);
    CHECK_VID(unit, vlan);

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_wlan) && BCM_GPORT_IS_WLAN_PORT(port)) {
        int idx, vp, mc_idx, gport_id, group = 0, mod_local;
        vlan_tab_entry_t vtab;
        bcm_wlan_port_t wlan_port;
        bcm_module_t mod_out;
        bcm_port_t port_out=0;
        bcm_trunk_t trunk_id;
        bcm_trunk_add_info_t tinfo;
        bcm_port_t trunk_member_port[BCM_SWITCH_TRUNK_MAX_PORTCNT];
        bcm_gport_t local_gport;
        bcm_if_t encap_id;
        bcm_if_t *if_array = NULL;
        int alloc_size, if_count, if_cur, if_max, match, trunk_local_ports = 0;

        vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
            return BCM_E_NOT_FOUND;
        }
        sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, VLAN_TABm, MEM_BLOCK_ANY, (int)vlan, &vtab));

        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }

        /* Get the VP attributes (need physical port) and encap id */
        BCM_IF_ERROR_RETURN(bcm_esw_wlan_port_get(unit, port, &wlan_port));
        /* Check if wlan_port.port is a local port or trunk */
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            trunk_id = BCM_GPORT_TRUNK_GET(wlan_port.port);
            rv = _bcm_trunk_id_validate(unit, trunk_id);
            if (BCM_FAILURE(rv)) {
                  return (BCM_E_PORT);
            }
            rv = bcm_esw_trunk_get(unit, trunk_id, &tinfo);
            if (BCM_FAILURE(rv)) {
                  return (BCM_E_PORT);
            }
            for (idx=0; idx<tinfo.num_ports; idx++) {
                rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                       tinfo.tm[idx], tinfo.tp[idx], &mod_out, &port_out);
                if (BCM_FAILURE(rv)) {
                    return (BCM_E_PORT);
                }
                /* Convert system port to physical port */
                if (soc_feature(unit, soc_feature_sysport_remap)) {
                    BCM_XLATE_SYSPORT_S2P(unit, &port_out);
                }
                rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
                if (mod_local) {
                    trunk_member_port[trunk_local_ports++] = port_out;
                }
            }
            local_gport = wlan_port.port;
        } else {
            rv = _bcm_esw_gport_resolve(unit, wlan_port.port, &mod_out, &port_out,
    	  			        &trunk_id, &gport_id); 
            if (BCM_FAILURE(rv)) {
                return (BCM_E_PORT);
            }
            rv = _bcm_esw_modid_is_local(unit, mod_out, &mod_local);
            BCM_IF_ERROR_RETURN(rv);
            if (TRUE != mod_local) {
                /* Only add this to replication set if destination is local */
               return BCM_E_PORT;
            }
            /* Convert system local_port to physical local_port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port_out);
            }
            rv = bcm_esw_port_gport_get(unit, port_out, &local_gport);
            if (BCM_FAILURE(rv)) {
                return rv;
            }        
        }
        BCM_IF_ERROR_RETURN
            (bcm_esw_multicast_wlan_encap_get(unit, group, local_gport,
                                              port, &encap_id));

        /* Search the WLAN MC group */
        if (!soc_mem_field32_get(unit, VLAN_TABm, &vtab, VIRTUAL_PORT_ENf)) {
            /* No group exists - so not found */
            return BCM_E_NOT_FOUND;
        } 
        mc_idx = soc_mem_field32_get(unit, VLAN_TABm, &vtab, BC_IDXf);
        _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_WLAN, mc_idx);

        /* If trunk, iterate over all local ports in the trunk and search for
         * a match on any local port */
        if_max = soc_mem_index_count(unit, EGR_L3_INTFm) + 
                 soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
        alloc_size = if_max * sizeof(int);
        if_array = sal_alloc(alloc_size, "temp IPMC repl interface array");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }
        if (BCM_GPORT_IS_TRUNK(wlan_port.port)) {
            match = FALSE;
            for (idx = 0; idx < tinfo.num_ports; idx++) {
                rv = bcm_esw_ipmc_egress_intf_get(unit, mc_idx, 
                                                  trunk_member_port[idx], 
                                                  if_max, if_array, &if_count);
                if (BCM_SUCCESS(rv)) {
                    for (if_cur = 0; if_cur < if_count; if_cur++) {
                        if (if_array[if_cur] == encap_id) {
                            match = TRUE;
                            *is_untagged = 0;
                            break;
                        }
                    }
                    if (match) {
                        break;
                    }
                }
            }
            if (!match) {
                rv = BCM_E_NOT_FOUND;
            }
        } else {
        rv = bcm_esw_ipmc_egress_intf_get(unit, mc_idx, port_out, if_max,
                                          if_array, &if_count);
        if (BCM_SUCCESS(rv)) {
            match = FALSE;
            for (if_cur = 0; if_cur < if_count; if_cur++) {
                if (if_array[if_cur] == encap_id) {
                    match = TRUE;
                    *is_untagged = 0;
                    break;
                }
            }
            if (!match) {
                rv = BCM_E_NOT_FOUND;
            }
        }
        }
        sal_free(if_array);
    } else
#endif
    {
        bcm_port_t local_port;

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port, &local_port));

        /* Deal with local physical ports */
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(ubmp);
        rv = bcm_esw_vlan_port_get(unit, vlan, &pbmp, &ubmp);
        if (!BCM_PBMP_MEMBER(pbmp, local_port)) {
            return BCM_E_NOT_FOUND;
        }
        if (BCM_PBMP_MEMBER(ubmp, local_port)) {
            *is_untagged = 1;
        } else {
            *is_untagged = 0;
        }
    }
    return rv;
}

int
bcm_esw_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max, 
                           bcm_gport_t *gport_array, int *is_untagged, 
                           int* array_size)
{
    return BCM_E_UNAVAIL;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * _bcm_vlan_reinit
 *
 * Recover VLAN state from hardware (reload)
 */
STATIC int
_bcm_vlan_reinit(int unit)
{
    bcm_vlan_info_t *vi = &vlan_info[unit];

    /* Clean up existing vlan_info */
    vi->init = TRUE;
    BCM_IF_ERROR_RETURN
        (_bcm_vlan_cleanup(unit));

#ifdef BCM_MCAST_FLOOD_DEFAULT
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_flood_default_set(unit, BCM_MCAST_FLOOD_DEFAULT));
#else
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_flood_default_set(unit,
                                         BCM_VLAN_MCAST_FLOOD_UNKNOWN));
#endif

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_flex_stat_init(unit, _bcmFlexStatTypeService));
    }
#endif

    /*
     * Initialize software structures
     */
    vi->defl = BCM_VLAN_DEFAULT;
    vi->count = 1;
    vi->init = TRUE;

    BCM_IF_ERROR_RETURN
        (_bcm_vbmp_init(&vi->bmp));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_vlan_reload(unit, &vi->bmp, &vi->count));

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        /* recreate software state for vlan translation */
        BCM_IF_ERROR_RETURN
            (_bcm_fb_vlan_translate_reload(unit, BCM_VLAN_XLATE_ING));
        BCM_IF_ERROR_RETURN
            (_bcm_fb_vlan_translate_reload(unit, BCM_VLAN_XLATE_EGR));
    }
#endif

#ifdef BCM_EASYRIDER_SUPPORT
    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
        if (SOC_IS_EASYRIDER(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_er_vlan_range_profile_init(unit));
        }
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_vlan_cleanup
 * Purpose:
 *      Free up resources without touching hardware
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
_bcm_vlan_cleanup(int unit)
{
    bcm_vlan_info_t *vi;

    CHECK_INIT(unit);

    BCM_LOCK(unit);
    vi = &vlan_info[unit];
    _bcm_vbmp_destroy(&vi->bmp);
    if (vi->qm_bmp != NULL) {
        sal_free(vi->qm_bmp);
    }
    if (vi->qm_it_bmp != NULL) {
        sal_free(vi->qm_it_bmp);
    }
    if (vi->egr_trans != NULL) {
        sal_free(vi->egr_trans);
    }
    if (vi->ing_trans != NULL) {
        sal_free(vi->ing_trans);
    }
    sal_memset(vi, 0, sizeof(bcm_vlan_info_t));
    BCM_UNLOCK(unit);

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_vlan_compar
 * Purpose:
 *     Compares two VLAN IDs.  Used by sorting utility routine
 * Parameters:
 *     a - Pointer to first VLAN ID of type bcm_vlan_t to compare
 *     b - Pointer to second VLAN ID of type bcm_vlan_t to compare
 * Returns:
 *     0 If vlan1 == vlan2
 *    -1 If vlan1 <  vlan2
 *     1 If vlan1 >  vlan2
 */
STATIC int
_bcm_vlan_compar(void *a, void *b)
{
    bcm_vlan_t vlan1, vlan2;

    vlan1 = *(bcm_vlan_t *)a;
    vlan2 = *(bcm_vlan_t *)b;

    return ( (vlan1 < vlan2) ? -1 : ((vlan1 > vlan2) ? 1 : 0));
}

/*
 * Function:
 *     _bcm_vlan_sw_dump
 * Purpose:
 *     Displays VLAN software structure information.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_vlan_sw_dump(int unit)
{
    int             i, count;
    bcm_vlan_info_t *vi = &vlan_info[unit];
    bcm_vlan_t      v_array[BCM_VLAN_COUNT];
    int             vlan_count = 0;

    soc_cm_print("\nSW Information VLAN - Unit %d\n", unit);
    soc_cm_print("  Init       : %4d\n", vi->init);
    soc_cm_print("  Default    : %4d\n", vi->defl);
    soc_cm_print("  Flood Mode : %4d\n", vi->flood_mode);
    soc_cm_print("  Count      : %4d\n", vi->count);

    /* Print VLAN list sorted */
    count = 0;
    for (i = BCM_VLAN_MIN; i < BCM_VLAN_COUNT; i++) {
        if (0 == _BCM_VBMP_LOOKUP(vi->bmp, i)){
            continue;
        }
        v_array[count] = i;
        count++;
        vlan_count++;
        if (vi->count == vlan_count) {
            break;
        }
    }

    soc_cm_print("  List       :");
    for (i = 0; i < count; i++) {
        if ((i > 0) && (!(i % 10))) {
            soc_cm_print("\n              ");
        }
        soc_cm_print(" %4d", v_array[i]);
    }
    soc_cm_print("\n");

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit) ||
        SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        _bcm_fb2_vlan_profile_sw_dump(unit);
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT */ 
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
