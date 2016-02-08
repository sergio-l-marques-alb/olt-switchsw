/*
 * $Id: triumph2_mim.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:    mim.c
 * Purpose: Manages MiM functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/triumph.h>

#include <bcm/error.h>
#include <bcm/mim.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>

/*
 * MIM VPN state - need to remember the ISID
 */
typedef struct _bcm_tr2_vpn_info_s {
    int isid;                /* The ISID value */
} _bcm_tr2_vpn_info_t;

/*
 * MIM port state - need to remember where to find the port.
 * Flags indicate the type of the port and hence the tables where the data is.
 * The index is used for the SOURCE_TRUNK_MAP table or for the
 * MPLS_STATION_TCAM.
 * The match attributes are the keys for the various hash tables.
 */
#define _BCM_MIM_PORT_TYPE_NETWORK                    (1 << 0)
#define _BCM_MIM_PORT_TYPE_ACCESS_PORT                (1 << 1)
#define _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN           (1 << 2)
#define _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED   (1 << 3)
#define _BCM_MIM_PORT_TYPE_ACCESS_LABEL               (1 << 4)
#define _BCM_MIM_PORT_TYPE_PEER                       (1 << 5)
typedef struct _bcm_tr2_mim_port_info_s {
    uint32 flags;
    uint32 index; /* Index of memory where port info is programmed */
    bcm_trunk_t tgid; /* Trunk group ID */
    bcm_module_t modid; /* Module id */
    bcm_port_t port; /* Port */
    bcm_vlan_t match_vlan; /* S-Tag */
    bcm_vlan_t match_inner_vlan; /* C-Tag */
    bcm_mpls_label_t match_label; /* MPLS label */
    bcm_mac_t match_tunnel_srcmac; /* NVP src BMAC */
    bcm_vlan_t match_tunnel_vlan; /* NVP BVID */
} _bcm_tr2_mim_port_info_t;

/*
 * Software book keeping for MIM related information
 */
typedef struct _bcm_tr2_mim_bookkeeping_s {
    _bcm_tr2_vpn_info_t *vpn_info;         /* VPN state */
    _bcm_tr2_mim_port_info_t *port_info;   /* VP state */
    SHR_BITDCL  *intf_bitmap;             /* L3 interfaces used */
} _bcm_tr2_mim_bookkeeping_t;


STATIC _bcm_tr2_mim_bookkeeping_t  _bcm_tr2_mim_bk_info[BCM_MAX_NUM_UNITS];

STATIC int mim_initialized; /* Flag to check initialized status */

STATIC sal_mutex_t _mim_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define L3_INFO(unit)    (&_bcm_l3_bk_info[unit])
#define MIM_INFO(_unit_) (&_bcm_tr2_mim_bk_info[_unit_])
#define VPN_ISID(_unit_, _vfi_)  \
        (_bcm_tr2_mim_bk_info[_unit_].vpn_info[_vfi_].isid)

#define MIM_INIT(unit)                                    \
    do {                                                  \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {  \
            return BCM_E_UNIT;                            \
        }                                                 \
        if (!mim_initialized) {                           \
            return BCM_E_INIT;                            \
        }                                                 \
    } while (0)

/* 
 * MIM module lock
 */
#define MIM_LOCK(unit) \
        sal_mutex_take(_mim_mutex[unit], sal_mutex_FOREVER);

#define MIM_UNLOCK(unit) \
        sal_mutex_give(_mim_mutex[unit]); 

/*
 * L3 interface usage bitmap operations
 */
#define _BCM_MIM_INTF_USED_GET(_u_, _intf_) \
        SHR_BITGET(MIM_INFO(_u_)->intf_bitmap, (_intf_))
#define _BCM_MIM_INTF_USED_SET(_u_, _intf_) \
        SHR_BITSET(MIM_INFO((_u_))->intf_bitmap, (_intf_))
#define _BCM_MIM_INTF_USED_CLR(_u_, _intf_) \
        SHR_BITCLR(MIM_INFO((_u_))->intf_bitmap, (_intf_))

/*
 * Function:
 *      bcm_mim_enable
 * Purpose:
 *      Enable/disable MiM function.
 * Parameters:
 *      unit - SOC unit number.
 *      enable - TRUE: enable MiM support; FALSE: disable MiM support.
 * Returns:
 *      BCM_E_XXX.
 */
STATIC int
bcm_tr2_mim_enable(int unit, int enable)
{
    int port;
    int idx, cnt;
    vlan_mpls_entry_t vlan_mpls;
    port_tab_entry_t ptab;

    PBMP_PORT_ITER(unit, port) {
        /* No MiM lookup on stacking ports. */
        if (IS_ST_PORT(unit, port)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlMacInMac,
                                      (enable) ? 1 : 0));
    }
    if (SOC_MEM_IS_VALID(unit, VLAN_MPLSm)) {
        cnt = soc_mem_index_count(unit, VLAN_MPLSm);
        for (idx = 0; idx < cnt; idx++) {
            sal_memset(&vlan_mpls, 0, sizeof(vlan_mpls_entry_t));
            BCM_IF_ERROR_RETURN
                (READ_VLAN_MPLSm(unit, SOC_BLOCK_ANY, idx, &vlan_mpls));
            soc_VLAN_MPLSm_field32_set(unit, &vlan_mpls, MIM_TERM_ENABLEf, 1);
            BCM_IF_ERROR_RETURN
                (WRITE_VLAN_MPLSm(unit, SOC_BLOCK_ALL, idx, &vlan_mpls));
        }
    }
    /* Enable MC_TERM_ENABLE on loopback port */
#if defined(BCM_ENDURO_SUPPORT)
    if (SOC_IS_ENDURO(unit)) {
        idx = 1;
    } else 
#endif /* BCM_ENDURO_SUPPORT */
    {
        idx = 54;
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, idx, &ptab));
    soc_PORT_TABm_field32_set(unit, &ptab, MIM_MC_TERM_ENABLEf, 1);    
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL, idx, &ptab));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_mim_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_tr2_mim_free_resources(int unit)
{
    _bcm_tr2_mim_bookkeeping_t *mim_info = MIM_INFO(unit);

    if (_mim_mutex[unit]) {
        sal_mutex_destroy(_mim_mutex[unit]);
        _mim_mutex[unit] = NULL;
    } 
    if (mim_info->vpn_info) {
        sal_free(mim_info->vpn_info);
        mim_info->vpn_info = NULL;
    }
    if (mim_info->port_info) {
        sal_free(mim_info->port_info);
        mim_info->port_info = NULL;
    }
    if (mim_info->intf_bitmap) {
        sal_free(mim_info->intf_bitmap);
        mim_info->intf_bitmap = NULL;
    }
}

/*
 * Function:
 *      bcm_mim_detach
 * Purpose:
 *      Detach the MIM software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_detach(int unit)
{
    int rv = BCM_E_NONE;
    rv = bcm_tr2_mim_enable(unit, FALSE);

    if (rv == BCM_E_INIT)
    {
        rv = BCM_E_NONE;
    }

    _bcm_tr2_mim_free_resources(unit);
    mim_initialized = FALSE;
    return rv;
}
 
/*
 * Function:
 *      bcm_mim_init
 * Purpose:
 *      Initialize the MIM software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_init(int unit)
{
    int num_vfi, num_vp, num_intf, rv = BCM_E_NONE;
    _bcm_tr2_mim_bookkeeping_t *mim_info = MIM_INFO(unit);

    if (!L3_INFO(unit)->l3_initialized) {
        soc_cm_debug(DK_L3, "L3 module must be initialized first\n");
        return BCM_E_NONE;
    }

    

    if (mim_initialized) {
        BCM_IF_ERROR_RETURN(bcm_tr2_mim_detach(unit));
    }
    num_vfi = soc_mem_index_count(unit, VFIm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);

    sal_memset(mim_info, 0, sizeof(_bcm_tr2_mim_bookkeeping_t));

    if (mim_info->vpn_info == NULL) {
        mim_info->vpn_info = sal_alloc(sizeof(_bcm_tr2_vpn_info_t) 
                                       * num_vfi, "mim_vpn_info");
        if (mim_info->vpn_info == NULL) {
            _bcm_tr2_mim_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(mim_info->vpn_info, 0, sizeof(_bcm_tr2_vpn_info_t) * num_vfi);

    if (mim_info->port_info == NULL) {
        mim_info->port_info = sal_alloc(sizeof(_bcm_tr2_mim_port_info_t) 
                                       * num_vp, "mim_port_info");
        if (mim_info->port_info == NULL) {
            _bcm_tr2_mim_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(mim_info->port_info, 0, sizeof(_bcm_tr2_mim_port_info_t) * num_vp);

    if (_mim_mutex[unit] == NULL) {
        _mim_mutex[unit] = sal_mutex_create("mim mutex");
        if (_mim_mutex[unit] == NULL) {
            _bcm_tr2_mim_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    if (NULL == mim_info->intf_bitmap) {
        mim_info->intf_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(num_intf), "intf_bitmap");
        if (mim_info->intf_bitmap == NULL) {
            _bcm_tr2_mim_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(mim_info->intf_bitmap, 0, SHR_BITALLOCSIZE(num_intf));    

    /* Enable MiM */
    rv = bcm_tr2_mim_enable(unit, TRUE);
    if (rv < 0) {
        _bcm_tr2_mim_free_resources(unit);
        return rv;
    }

    mim_initialized = TRUE;
    return rv;
}

/*
 * Function:
 *      bcm_mim_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_vpn_create(int unit, bcm_mim_vpn_config_t *info)
{
    int index, rv = BCM_E_PARAM;
    vfi_entry_t vfi_entry;
    vfi_1_entry_t vfi_1_entry;
    mpls_entry_entry_t mpls_entry;
    egr_vlan_xlate_entry_t egr_vlan_xlate_entry;
    int vfi, bc_group, umc_group, uuc_group;
    int bc_group_type, umc_group_type, uuc_group_type;

    MIM_INIT(unit);

    /* Check that the groups are valid. */
    bc_group_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
    bc_group = _BCM_MULTICAST_ID_GET(info->broadcast_group);
    umc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_multicast_group);
    umc_group = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
    uuc_group_type = _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
    uuc_group = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

    if ((bc_group_type != _BCM_MULTICAST_TYPE_MIM) ||
        (umc_group_type != _BCM_MULTICAST_TYPE_MIM) ||
        (uuc_group_type != _BCM_MULTICAST_TYPE_MIM) ||
        (bc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
        (umc_group >= soc_mem_index_count(unit, L3_IPMCm)) ||
        (uuc_group >= soc_mem_index_count(unit, L3_IPMCm))) {
        return BCM_E_PARAM;
    }

    MIM_LOCK(unit);
    if (info->flags & BCM_MIM_VPN_WITH_ID) {
        /* Check if VFI exists and handle the replace case */
        vfi = _BCM_MIM_VPN_ID_GET(info->vpn);
        if (_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            if (!(info->flags & BCM_MIM_VPN_REPLACE)) {
                MIM_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
        } else {
           rv = _bcm_vfi_alloc_with_id(unit, VFIm, _bcmVfiTypeMim, vfi);
           if (rv < 0) {
               MIM_UNLOCK(unit);
               return rv;
           }
        }
    } else {
        /* Allocate a free VFI entry */
        rv = _bcm_vfi_alloc(unit, VFIm, _bcmVfiTypeMim, &vfi);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
    }
    VPN_ISID(unit, vfi) = info->lookup_id;

    /* Commit the VFI entry to HW */
    sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
    soc_VFIm_field32_set(unit, &vfi_entry, BC_INDEXf, bc_group);
    soc_VFIm_field32_set(unit, &vfi_entry, UMC_INDEXf, umc_group);
    soc_VFIm_field32_set(unit, &vfi_entry, UUC_INDEXf, uuc_group);
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);

    if (rv < 0) {
        MIM_UNLOCK(unit);
        return rv;
    }

    /* Check for incoming SD-tag match */
    if (info->flags & BCM_MIM_VPN_MATCH_SERVICE_VLAN_TPID) {
        sal_memset(&vfi_1_entry, 0, sizeof(vfi_1_entry_t));
        rv = _bcm_fb2_outer_tpid_entry_add(unit, info->match_service_tpid, &index);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
        soc_VFI_1m_field32_set(unit, &vfi_1_entry, SD_TAG_MODEf, 1);
        soc_VFI_1m_field32_set(unit, &vfi_1_entry, TPID_ENABLEf, (1 << index));
        rv = WRITE_VFI_1m(unit, MEM_BLOCK_ALL, vfi, &vfi_1_entry);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
    }

    /* Program the ISID to VFI mapping */
    sal_memset(&mpls_entry, 0, sizeof(mpls_entry_entry_t));
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, KEY_TYPEf, 0x2);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, VALIDf, 0x1);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, MIM_ISID__ISIDf, info->lookup_id);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, MIM_ISID__VFIf, vfi);
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &mpls_entry, &mpls_entry, 0);
    if (rv == SOC_E_NONE) {
        sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
        rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);
        _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
        MIM_UNLOCK(unit);
        return BCM_E_EXISTS;
    } else if (rv != SOC_E_NOT_FOUND) {
        sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
        rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);
        _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
        MIM_UNLOCK(unit);
        return rv;
    }
    rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &mpls_entry);
    if (rv < 0) {
        MIM_UNLOCK(unit);
        return rv;
    }

    /* Program the VFI to ISID mapping */
    sal_memset(&egr_vlan_xlate_entry, 0, sizeof(egr_vlan_xlate_entry_t));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, ENTRY_TYPEf, 0x3);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, MIM_ISID__VFIf, vfi);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, VALIDf, 0x1);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, MIM_ISID__ISIDf, 
                                    info->lookup_id);
    rv = soc_mem_search(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ANY, &index,
                        &egr_vlan_xlate_entry, &egr_vlan_xlate_entry, 0);
    if (rv == SOC_E_NONE) {
        _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
        MIM_UNLOCK(unit);
        return BCM_E_EXISTS;
    } else if (rv != SOC_E_NOT_FOUND) {
        _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);
        MIM_UNLOCK(unit);
        return rv;
    }
    rv = soc_mem_insert(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &egr_vlan_xlate_entry);
    if (rv < 0) {
        MIM_UNLOCK(unit);
        return rv;
    }

    MIM_UNLOCK(unit);

    _BCM_MIM_VPN_SET(info->vpn, _BCM_MIM_VPN_TYPE_MIM, vfi);
    return rv;
}

/*
 * Function:
 *      bcm_mim_vpn_destroy
 * Purpose:
 *      Delete a VPN instance
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_vpn_destroy(int unit, bcm_mim_vpn_t vpn)
{
    int vfi, rv, tpid_en, i;
    vfi_entry_t vfi_entry;
    vfi_1_entry_t vfi_1_entry;
    mpls_entry_entry_t mpls_entry;
    egr_vlan_xlate_entry_t egr_vlan_xlate_entry;

    MIM_INIT(unit);
    MIM_LOCK(unit);

    vfi = _BCM_MIM_VPN_ID_GET(vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        MIM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    /* Delete all the MIM ports on this VPN */
    rv = bcm_tr2_mim_port_delete_all(unit, vpn);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 

    sal_memset(&vfi_entry, 0, sizeof(vfi_entry_t));
    rv = WRITE_VFIm(unit, MEM_BLOCK_ALL, vfi, &vfi_entry);

    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        /* Release Service counter, if any */
        _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeService,
                                       vpn);
    }

    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 

    sal_memset(&mpls_entry, 0, sizeof(mpls_entry_entry_t));
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, KEY_TYPEf, 0x2);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, VALIDf, 0x1);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_entry, MIM_ISID__ISIDf, VPN_ISID(unit, vfi));
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &mpls_entry);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 

    sal_memset(&egr_vlan_xlate_entry, 0, sizeof(egr_vlan_xlate_entry_t));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, ENTRY_TYPEf, 0x3);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, MIM_ISID__VFIf, vfi);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, VALIDf, 0x1);
    rv = soc_mem_delete(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ANY, &egr_vlan_xlate_entry);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 

    rv = soc_mem_read(unit, VFI_1m, MEM_BLOCK_ANY, vfi, &vfi_1_entry);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 
                                     
    /* Delete TPID if applicable */
    if (soc_VFI_1m_field32_get(unit, &vfi_1_entry, SD_TAG_MODEf)) {
        tpid_en = soc_VFI_1m_field32_get(unit, &vfi_1_entry, TPID_ENABLEf);
        for (i = 0; i < 4; i++) {
            if (tpid_en & (1 << i)) {
               (void)_bcm_fb2_outer_tpid_entry_delete(unit, i);
                break;
            }
        }
    }
    sal_memset(&vfi_1_entry, 0, sizeof(vfi_1_entry_t));
    rv = WRITE_VFI_1m(unit, MEM_BLOCK_ALL, vfi, &vfi_1_entry);
    if (BCM_FAILURE(rv)) {
        MIM_UNLOCK(unit);
        return rv;
    } 

    VPN_ISID(unit, vfi) = 0;
    (void) _bcm_vfi_free(unit, _bcmVfiTypeMim, vfi);

    MIM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_mim_vpn_destroy_all
 * Purpose:
 *      Delete al VPN instances
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_vpn_destroy_all(int unit)
{
    int num_vfi, i, rv, rv_error = BCM_E_NONE;
    bcm_vpn_t vpn;

    MIM_INIT(unit);
    num_vfi = soc_mem_index_count(unit, VFIm);
    for (i = 0; i < num_vfi; i++) {
        if (_bcm_vfi_used_get(unit, i, _bcmVfiTypeMim)) {
            _BCM_MIM_VPN_SET(vpn, _BCM_MIM_VPN_TYPE_MIM, i);
            rv = bcm_tr2_mim_vpn_destroy(unit, vpn);
            if (rv < 0) {
                rv_error = rv;
            }
        }
    }
    return rv_error;
}

/*
 * Function:
 *      bcm_mim_vpn_get
 * Purpose:
 *      Get information about a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vpn   - (IN)  VPN instance ID
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_vpn_get(int unit, bcm_mim_vpn_t vpn, 
                    bcm_mim_vpn_config_t *info)
{
    int vfi, tpid_en, i;
    vfi_entry_t vfi_entry;
    vfi_1_entry_t vfi_1_entry;

    MIM_INIT(unit);
    vfi = _BCM_MIM_VPN_ID_GET(vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        MIM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }
    
    info->flags |= BCM_MIM_VPN_MIM;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, VFIm, MEM_BLOCK_ANY, vfi, 
                                     &vfi_entry));
    info->broadcast_group = soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
    info->unknown_unicast_group = soc_VFIm_field32_get(unit, &vfi_entry, 
                                                       UUC_INDEXf);
    info->unknown_multicast_group = soc_VFIm_field32_get(unit, &vfi_entry, 
                                                         UMC_INDEXf);
    info->lookup_id = VPN_ISID(unit, vfi);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, VFI_1m, MEM_BLOCK_ANY, vfi, 
                                     &vfi_1_entry));

    if (soc_VFI_1m_field32_get(unit, &vfi_1_entry, SD_TAG_MODEf)) {
        info->flags |= BCM_MIM_VPN_MATCH_SERVICE_VLAN_TPID;
        tpid_en = soc_VFI_1m_field32_get(unit, &vfi_1_entry, TPID_ENABLEf);
        for (i = 0; i < 4; i++) {
            if (tpid_en & (1 << i)) {
                _bcm_fb2_outer_tpid_entry_get(unit, 
                                      &info->match_service_tpid, i);
                break;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_mim_vpn_traverse
 * Purpose:
 *      Get information about a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      cb    - (IN)  User-provided callback
 *      info  - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */

int bcm_tr2_mim_vpn_traverse(int unit, bcm_mim_vpn_traverse_cb cb, 
                             void *user_data)
{
    int i, index_min, index_max, rv = BCM_E_NONE;
    int vpn;
    bcm_mim_vpn_config_t info;

    MIM_INIT(unit);

    index_min = soc_mem_index_min(unit, VFIm);
    index_max = soc_mem_index_max(unit, VFIm);

    MIM_LOCK(unit);
    for (i = index_min; i <= index_max; i++) {
        if (!_bcm_vfi_used_get(unit, i, _bcmVfiTypeMim)) {
            continue;
        }
        bcm_mim_vpn_config_t_init(&info);
        _BCM_MIM_VPN_SET(vpn, _BCM_MIM_VPN_TYPE_MIM, i);
        rv = bcm_tr2_mim_vpn_get(unit, vpn, &info);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit);
            return rv;
        }
        rv = cb(unit, &info, user_data);
        if (BCM_FAILURE(rv)) {
            MIM_UNLOCK(unit); 
            return rv;
        }
    }
    MIM_UNLOCK(unit);
    return rv;
}

STATIC int
_bcm_tr2_mim_l3_intf_add(int unit, _bcm_l3_intf_cfg_t *if_info)
{
    int i, num_intf;
    egr_l3_intf_entry_t egr_intf;
    bcm_mac_t hw_mac;
    num_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    for (i = 0; i < num_intf; i++) {
        if (_BCM_MIM_INTF_USED_GET(unit, i)) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm,  
                                             MEM_BLOCK_ANY, i, &egr_intf));
            soc_mem_mac_addr_get(unit, EGR_L3_INTFm, &egr_intf, 
                                 MAC_ADDRESSf, hw_mac);
            if (SAL_MAC_ADDR_EQUAL(hw_mac, if_info->l3i_mac_addr)) {
                if_info->l3i_index = i;
                return BCM_E_NONE;
            }
        }
    }
    /* Create an interface */
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_intf_create(unit, if_info));
    _BCM_MIM_INTF_USED_SET(unit, if_info->l3i_index);
    return BCM_E_NONE;
}

typedef struct _bcm_tr2_ing_nh_info_s {
    int      port;
    int      module;
    int      trunk;
    uint32   mtu;
} _bcm_tr2_ing_nh_info_t;

typedef struct _bcm_tr2_egr_nh_info_s {
    uint8    entry_type;
    uint8    dvp_is_network;
    uint8    sd_tag_action_present;
    uint8    sd_tag_action_not_present;
    int      dvp;
    int      intf_num;
    int      sd_tag_vlan;
    int      macda_index;
    int      tpid_index;
} _bcm_tr2_egr_nh_info_t;

STATIC int
_bcm_tr2_mim_l2_nh_info_add(int unit, bcm_mim_port_t *mim_port, int vp, int drop,
                           int *nh_index, bcm_port_t *local_port, int *is_local) 
{
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_mac_da_profile_entry_t macda;
    _bcm_tr2_ing_nh_info_t ing_nh_info;
    _bcm_tr2_egr_nh_info_t egr_nh_info;
    bcm_l3_egress_t nh_info;
    _bcm_l3_intf_cfg_t if_info;
    uint32 nh_flags;
    int gport_id, rv;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int action_present, action_not_present, tpid_index = -1;
    int old_tpid_idx = -1, old_macda_idx = -1, old_intf_num = -1;
    void *entries[1];
    uint64 temp_mac;

    
    /* Initialize values */
    *local_port = 0;
    *is_local = 0;
    ing_nh_info.mtu = 0x3fff; 
    ing_nh_info.port = -1;
    ing_nh_info.module = -1;
    ing_nh_info.trunk = -1;

    egr_nh_info.dvp = vp;
    egr_nh_info.dvp_is_network = (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) ? 1 : 0;
    egr_nh_info.sd_tag_action_present = 0;
    egr_nh_info.sd_tag_action_not_present = 0;
    egr_nh_info.intf_num = -1;
    egr_nh_info.sd_tag_vlan = -1;
    egr_nh_info.macda_index = 0;
    egr_nh_info.tpid_index = -1;

    if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
        if ((*nh_index > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) ||
            (*nh_index < soc_mem_index_min(unit, EGR_L3_NEXT_HOPm)))  {
            return BCM_E_PARAM;
        }
        /* Read the existing egress next_hop entry */
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                          *nh_index, &egr_nh);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /*
         * Allocate a next-hop entry. By calling bcm_xgs3_nh_add()
         * with _BCM_L3_SHR_WRITE_DISABLE flag, a next-hop index is
         * allocated but nothing is written to hardware. The "nh_info"
         * in this case is not used, so just set to all zeros.
         */
        sal_memset(&nh_info, 0, sizeof(bcm_l3_egress_t));
        nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
        rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, nh_index);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Resolve the gport */
    rv = _bcm_esw_gport_resolve(unit, mim_port->port, &mod_out, 
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
        ing_nh_info.module = -1;
        ing_nh_info.port = -1;
        ing_nh_info.trunk = trunk_id;
        MIM_INFO(unit)->port_info[vp].modid = -1;
        MIM_INFO(unit)->port_info[vp].port = -1;
        MIM_INFO(unit)->port_info[vp].tgid = trunk_id;
    } else {
        ing_nh_info.module = mod_out;
        ing_nh_info.port = port_out;
        ing_nh_info.trunk = -1;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, mod_out, is_local));

        if (TRUE == *is_local) {
            /* Indicated to calling function that this is a local port */
            *is_local = 1;
            *local_port = ing_nh_info.port;
        }

        MIM_INFO(unit)->port_info[vp].modid = mod_out;
        MIM_INFO(unit)->port_info[vp].port = port_out;
        MIM_INFO(unit)->port_info[vp].tgid = -1;
    }

    if ((mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) || 
        (mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
        /* Deal with backbone/peer ports (ingress and egress) */
        /* Use the MIM view of EGR_L3_NEXT_HOP */
        if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
            /* Be sure that the existing entry is programmed to MIM */
            egr_nh_info.entry_type = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
            if (egr_nh_info.entry_type != 0x3) { /* != MIM */
                return BCM_E_PARAM;
            }
            /* Remember old MAC DA profile index */
            old_macda_idx = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 MIM__MAC_DA_PROFILE_INDEXf);
            /* Remember old interface number (for MAC SA) */
            old_intf_num = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 MIM__INTF_NUMf);
        }
        egr_nh_info.entry_type = 0x3;

        /* Add MAC DA profile */
        sal_memset(&macda, 0, sizeof(egr_mac_da_profile_entry_t));
        soc_mem_mac_addr_set(unit, EGR_MAC_DA_PROFILEm, &macda, 
                             MAC_ADDRESSf, mim_port->egress_tunnel_dstmac);
        entries[0] = &macda;
        rv = _bcm_mac_da_profile_entry_add(unit, entries, 1,
                                           (uint32 *) &egr_nh_info.macda_index);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Add MACSA to an L3 interface entry - ref count if exists */
        if (!BCM_VLAN_VALID(mim_port->egress_tunnel_vlan)) {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        sal_memset(&if_info, 0, sizeof(_bcm_l3_intf_cfg_t));
        SAL_MAC_ADDR_TO_UINT64(mim_port->egress_tunnel_srcmac, temp_mac);
        SAL_MAC_ADDR_FROM_UINT64(if_info.l3i_mac_addr, temp_mac);
        if_info.l3i_vid = mim_port->egress_tunnel_vlan;
        rv = _bcm_tr2_mim_l3_intf_add(unit, &if_info);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Populate the fields of MIM::EGR_l3_NEXT_HOP */
        sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            ENTRY_TYPEf, egr_nh_info.entry_type);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__DVPf, egr_nh_info.dvp);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, MIM__DVP_IS_NETWORK_PORTf,
                            egr_nh_info.dvp_is_network);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__BVID_VALIDf, 1);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__INTF_NUMf, if_info.l3i_index);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__BVIDf, mim_port->egress_tunnel_vlan);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            MIM__HG_HDR_SELf, 1); /* HG 2 */
        if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                MIM__ISID_LOOKUP_TYPEf, 1); /* Peer */
        }
        if (mim_port->flags & BCM_MIM_PORT_EGRESS_TUNNEL_DEST_MAC_USE_SERVICE) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                MIM__ADD_ISID_TO_MACDAf, 1); 
        }

    } else if (mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS){
        /* Deal with access ports */
        /* Use the SD_TAG view of EGR_L3_NEXT_HOP */
        if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
            /* Be sure that the existing entry is programmed to SD-tag */
            egr_nh_info.entry_type = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
            if (egr_nh_info.entry_type != 0x2) { /* != SD-tag */
                return BCM_E_PARAM;
            }
            action_present = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
            action_not_present = 
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
            if ((action_not_present == 0x1) || (action_present == 0x1)) {
                /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
                 * index of the entry getting replaced is valid. Save
                 * the old tpid index to be deleted later.
                 */
                old_tpid_idx = 
                    soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                     SD_TAG__SD_TAG_TPID_INDEXf);
            }
        }
        egr_nh_info.entry_type = 0x2; /* SD-tag */
    
        /* Populate SD_TAG::EGR_L3_NEXT_HOP entry */
        sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            ENTRY_TYPEf, egr_nh_info.entry_type);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            SD_TAG__DVPf, egr_nh_info.dvp);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, SD_TAG__DVP_IS_NETWORK_PORTf,
                            egr_nh_info.dvp_is_network);

        if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED) {
            if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD) {
                if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                    return BCM_E_PARAM;
                }
                egr_nh_info.sd_tag_vlan = mim_port->egress_service_vlan; 
                egr_nh_info.sd_tag_action_not_present = 0x1; /* ADD */
            }
            if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE) {
                if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                    return BCM_E_PARAM;
                }
                /* REPLACE_VID_TPID */
                egr_nh_info.sd_tag_vlan = mim_port->egress_service_vlan; 
                egr_nh_info.sd_tag_action_present = 0x1;
            } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE) {
                if (!BCM_VLAN_VALID(mim_port->egress_service_vlan)) {
                    return BCM_E_PARAM;
                }
                /* REPLACE_VID_ONLY */
                egr_nh_info.sd_tag_vlan = mim_port->egress_service_vlan; 
                egr_nh_info.sd_tag_action_present = 0x2;
            } else if (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE) {
                egr_nh_info.sd_tag_action_present = 0x3; /* DELETE */
            } else {
                
            }

            if ((mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD) ||
                (mim_port->flags & BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE)) {
                /* TPID value is used */
                rv = _bcm_fb2_outer_tpid_entry_add(unit, mim_port->egress_service_tpid, 
                                                   &tpid_index);
                BCM_IF_ERROR_RETURN(rv);
                egr_nh_info.tpid_index = tpid_index;
            }

            /* Populate SD-TAG attributes */
            if (egr_nh_info.sd_tag_vlan != -1) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, SD_TAG__SD_TAG_VIDf, egr_nh_info.sd_tag_vlan);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                            SD_TAG__SD_TAG_ACTION_IF_PRESENTf,
                            egr_nh_info.sd_tag_action_present);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf,
                                egr_nh_info.sd_tag_action_not_present);
            if (egr_nh_info.tpid_index != -1) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, SD_TAG__SD_TAG_TPID_INDEXf,
                                    egr_nh_info.tpid_index);
            }
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                            SD_TAG__HG_MODIFY_ENABLEf, (*is_local) ? 1 : 0);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, SD_TAG__HG_HDR_SELf, 1);
        }
    }

    /* Write INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, MODULE_IDf, ing_nh_info.module);
    } else {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, Tf, 1);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                            &initial_ing_nh, TGIDf, ing_nh_info.trunk);
        BCM_GPORT_TRUNK_SET(*local_port, ing_nh_info.trunk);
    }
    rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &initial_ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write EGR_L3_NEXT_HOP entry */
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &egr_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    if (ing_nh_info.trunk == -1) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, PORT_NUMf, ing_nh_info.port);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, MODULE_IDf, ing_nh_info.module);
    } else {    
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, Tf, 1);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, TGIDf, ing_nh_info.trunk);
    }
    if (drop) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    }
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, ENTRY_TYPEf, 0x2); /* L2 DVP */
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                       &ing_nh, MTU_SIZEf, ing_nh_info.mtu);
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, *nh_index, &ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Delete old TPID, MAC indexes */
    if (old_macda_idx != -1) {
        rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
        BCM_IF_ERROR_RETURN(rv);
    }
    if (old_tpid_idx != -1) {
        (void)_bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx);
    }
    return rv;

cleanup:
    if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    if (tpid_index != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
    }
    if (egr_nh_info.macda_index != -1) {
        (void) _bcm_mac_da_profile_entry_delete(unit, egr_nh_info.macda_index);
    }
    return rv;
}

STATIC int
_bcm_tr2_mim_l2_nh_info_delete(int unit, int nh_index)
{
    int rv, old_macda_idx = -1;
    int action_present, action_not_present, old_tpid_idx = -1;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &initial_ing_nh));

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                ENTRY_TYPEf) == 0x2) {
        /* Access port */
        action_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                     SD_TAG__SD_TAG_ACTION_IF_PRESENTf);
        action_not_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                     SD_TAG__SD_TAG_ACTION_IF_NOT_PRESENTf);
        if ((action_not_present == 0x1) || (action_present == 0x1)) {
            /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
             * index of the entry is valid. Save
             * the old tpid index to be deleted later.
             */
            old_tpid_idx =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG__SD_TAG_TPID_INDEXf);
        }
    } else if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                ENTRY_TYPEf) == 0x3){
        /* Network or peer port */
        old_macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                     MIM__MAC_DA_PROFILE_INDEXf);
    } else {
        return BCM_E_NOT_FOUND;
    }

    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write (unit, ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &ing_nh));

    /* Clear INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN (soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    /* Delete old TPID */
    if (old_tpid_idx != -1) {
        (void) _bcm_fb2_outer_tpid_entry_delete(unit, old_tpid_idx);
    }

    /* Delete old MAC profile reference */
    if (old_macda_idx != -1) {
        rv = _bcm_mac_da_profile_entry_delete(unit, old_macda_idx);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Free the next-hop entry. */
    rv = bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index);
    return rv;
}

STATIC int
_bcm_tr2_mim_match_add(int unit, bcm_mim_port_t *mim_port, int vp)
{
    int rv = BCM_E_NONE, num_bits_for_port, i, gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;

    rv = _bcm_esw_gport_resolve(unit, mim_port->port, &mod_out,
                                &port_out, &trunk_id, &gport_id);
    BCM_IF_ERROR_RETURN(rv);

    if ((mim_port->criteria == BCM_MIM_PORT_MATCH_PORT_VLAN) ||
        (mim_port->criteria == BCM_MIM_PORT_MATCH_PORT_VLAN_STACKED)) {

        vlan_xlate_entry_t vent;

        sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));
        soc_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &vent, MPLS_ACTIONf, 0x1); /* SVP */
        soc_VLAN_XLATEm_field32_set(unit, &vent, DISABLE_VLAN_CHECKSf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &vent, SOURCE_VPf, vp);
        if (mim_port->criteria == BCM_MIM_PORT_MATCH_PORT_VLAN) {
            if (!BCM_VLAN_VALID(mim_port->match_vlan)) {
                return BCM_E_PARAM;
            }
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_OVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf,
                                        mim_port->match_vlan);
            MIM_INFO(unit)->port_info[vp].flags |= 
                                     _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN;
            MIM_INFO(unit)->port_info[vp].match_vlan = mim_port->match_vlan;

        } else {
            if (!BCM_VLAN_VALID(mim_port->match_vlan) || 
                !BCM_VLAN_VALID(mim_port->match_inner_vlan)) {
                return BCM_E_PARAM;
            }
            soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf,
                                        TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
            soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf,
                                        mim_port->match_vlan);
            soc_VLAN_XLATEm_field32_set(unit, &vent, IVIDf,
                                        mim_port->match_inner_vlan);
            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED;
            MIM_INFO(unit)->port_info[vp].match_vlan = mim_port->match_vlan;
            MIM_INFO(unit)->port_info[vp].match_inner_vlan = 
                             mim_port->match_inner_vlan;
        }

        if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
            soc_VLAN_XLATEm_field32_set(unit, &vent, Tf, 1);
            soc_VLAN_XLATEm_field32_set(unit, &vent, TGIDf, trunk_id);
        } else {
            soc_VLAN_XLATEm_field32_set(unit, &vent, MODULE_IDf, mod_out);
            soc_VLAN_XLATEm_field32_set(unit, &vent, PORT_NUMf, port_out);
        }
        rv = soc_mem_insert(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &vent);
        BCM_IF_ERROR_RETURN(rv);

    } else if (mim_port->criteria == BCM_MIM_PORT_MATCH_PORT) {
        if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
            return BCM_E_PARAM;
        } else {
            num_bits_for_port =
                _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));
            i = (mod_out << num_bits_for_port) | port_out;
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        i, SOURCE_VPf, vp);
            BCM_IF_ERROR_RETURN(rv);

            
            rv = soc_mem_field32_modify(unit, PORT_TABm, port_out,
                                        PORT_OPERATIONf, 0x1); /* L2_SVP */
            BCM_IF_ERROR_RETURN(rv);

            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_ACCESS_PORT;
            MIM_INFO(unit)->port_info[vp].index = i;
        }

    } else if ((mim_port->criteria == BCM_MIM_PORT_MATCH_LABEL)) {
        mpls_entry_entry_t ment;

        sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
        if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, Tf, 1);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, TGIDf, trunk_id);
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MODULE_IDf, mod_out);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, PORT_NUMf, port_out);
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_LABELf,
                                    mim_port->match_label);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, VALIDf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, SOURCE_VPf, vp);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_BOSf,
                                    0x1); /* L2 SVP */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_ACTION_IF_NOT_BOSf,
                                    0x0); /* INVALID */

        rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);
        BCM_IF_ERROR_RETURN(rv);
        MIM_INFO(unit)->port_info[vp].flags |= 
                         _BCM_MIM_PORT_TYPE_ACCESS_LABEL;
        MIM_INFO(unit)->port_info[vp].match_label = mim_port->match_label;

    } else if ((mim_port->criteria == BCM_MIM_PORT_MATCH_TUNNEL_VLAN_SRCMAC)) {
        mpls_entry_entry_t ment;
        uint64 temp_mac;

        if (!BCM_VLAN_VALID(mim_port->match_tunnel_vlan)) {
            return BCM_E_PARAM;
        }

        sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
        soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, 1); /* MIM NVP */

        if (BCM_GPORT_IS_TRUNK(mim_port->port)) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_NVP__Tf, 1);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_NVP__TGIDf, trunk_id);
        } else {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_NVP__MODULE_IDf, mod_out);
            soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_NVP__PORT_NUMf, port_out);
        }
        if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
            soc_MPLS_ENTRYm_field32_set(unit, &ment, 
                                        MIM_NVP__ISID_LOOKUP_TYPEf, 1);
            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_PEER;
        } else {
            MIM_INFO(unit)->port_info[vp].flags |= 
                             _BCM_MIM_PORT_TYPE_NETWORK;
        }
        soc_MPLS_ENTRYm_field32_set(unit, &ment, VALIDf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_NVP__SVPf, vp);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_NVP__BVIDf,
                                    mim_port->match_tunnel_vlan);
        soc_mem_mac_addr_set(unit, MPLS_ENTRYm, &ment, MIM_NVP__BMACSAf, 
                             mim_port->match_tunnel_srcmac);
        rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);
        BCM_IF_ERROR_RETURN(rv);
        SAL_MAC_ADDR_TO_UINT64(mim_port->match_tunnel_srcmac, temp_mac);
        SAL_MAC_ADDR_FROM_UINT64
           (MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac, temp_mac); 
        MIM_INFO(unit)->port_info[vp].match_tunnel_vlan = 
                         mim_port->match_tunnel_vlan;
    }
    return rv;
}

STATIC int
_bcm_tr2_mim_match_delete(int unit, int vp)
{
    int port, rv;

    if (MIM_INFO(unit)->port_info[vp].flags & 
        _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN) {
        vlan_xlate_entry_t vent;
        sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));

        soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, 
                                    TR_VLXLT_HASH_KEY_TYPE_OVID);
        soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf,
                                    MIM_INFO(unit)->port_info[vp].match_vlan);
        soc_VLAN_XLATEm_field32_set(unit, &vent, MODULE_IDf, 
                                    MIM_INFO(unit)->port_info[vp].modid);
        soc_VLAN_XLATEm_field32_set(unit, &vent, PORT_NUMf, 
                                    MIM_INFO(unit)->port_info[vp].port);
        rv = soc_mem_delete(unit, VLAN_XLATEm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);   

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED) {
        vlan_xlate_entry_t vent;
        sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));

        soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, 
                                    TR_VLXLT_HASH_KEY_TYPE_IVID_OVID);
        soc_VLAN_XLATEm_field32_set(unit, &vent, OVIDf,
                                    MIM_INFO(unit)->port_info[vp].match_vlan);
        soc_VLAN_XLATEm_field32_set(unit, &vent, IVIDf,
                              MIM_INFO(unit)->port_info[vp].match_inner_vlan);
        soc_VLAN_XLATEm_field32_set(unit, &vent, MODULE_IDf, 
                                    MIM_INFO(unit)->port_info[vp].modid);
        soc_VLAN_XLATEm_field32_set(unit, &vent, PORT_NUMf, 
                                    MIM_INFO(unit)->port_info[vp].port);
        rv = soc_mem_delete(unit, VLAN_XLATEm, MEM_BLOCK_ANY, &vent);
        BCM_IF_ERROR_RETURN(rv);   

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_PORT) {
        rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, 
                                    MIM_INFO(unit)->port_info[vp].index, 
                                    SOURCE_VPf, 0);        
        BCM_IF_ERROR_RETURN(rv);
        port = MIM_INFO(unit)->port_info[vp].index & SOC_PORT_ADDR_MAX(unit);
        rv = soc_mem_field32_modify(unit, PORT_TABm, port,
                                    PORT_OPERATIONf, 0x0); /* NORMAL */
        BCM_IF_ERROR_RETURN(rv);

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_LABEL) {
        mpls_entry_entry_t ment; 
        sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MODULE_IDf, 
                                    MIM_INFO(unit)->port_info[vp].modid);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, PORT_NUMf, 
                                    MIM_INFO(unit)->port_info[vp].port);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MPLS_LABELf,
                                    MIM_INFO(unit)->port_info[vp].match_label);
        rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &ment);
        BCM_IF_ERROR_RETURN(rv);

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               (_BCM_MIM_PORT_TYPE_NETWORK | _BCM_MIM_PORT_TYPE_PEER)) {
        mpls_entry_entry_t ment;
        sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
        soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, 1); /* MIM NVP */
        soc_MPLS_ENTRYm_field32_set(unit, &ment, VALIDf, 1);
        soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_NVP__BVIDf,
                               MIM_INFO(unit)->port_info[vp].match_tunnel_vlan);
        soc_mem_mac_addr_set(unit, MPLS_ENTRYm, &ment, MIM_NVP__BMACSAf, 
                             MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac);
        rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment);
        BCM_IF_ERROR_RETURN(rv);
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_mim_peer_port_config_add(int unit, bcm_mim_port_t *mim_port, 
                                  int vp, bcm_mim_vpn_t vpn) {
    mpls_entry_entry_t ment;
    egr_vlan_xlate_entry_t egr_vlan_xlate_entry;
    int index, vfi, rv = BCM_E_NONE;

    /* Need an ISID-SVP entry in MPLS_ENTRY */
    vfi = _BCM_MIM_VPN_ID_GET(vpn);
    sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
    soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, 3); /* MIM_ISID_SVP */
    soc_MPLS_ENTRYm_field32_set(unit, &ment, VALIDf, 0x1);
    soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_ISID__ISIDf, 
                                VPN_ISID(unit, vfi));
    soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_ISID__SVPf, vp);
    soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_ISID__VFIf, vfi);
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &ment, &ment, 0);
    if (rv == SOC_E_NONE) {
        return BCM_E_EXISTS;
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }
    BCM_IF_ERROR_RETURN(soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &ment));

    /* Also need the reverse entry in the egress */
    sal_memset(&egr_vlan_xlate_entry, 0, sizeof(egr_vlan_xlate_entry_t));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, ENTRY_TYPEf, 
                                    0x4);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, MIM_ISID__VFIf, 
                                    vfi);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, VALIDf, 0x1);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry,  
                                    MIM_ISID__ISIDf, VPN_ISID(unit, vfi));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, 
                                    MIM_ISID__DVPf, vp);
    rv = soc_mem_search(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ANY, &index,
                        &egr_vlan_xlate_entry, &egr_vlan_xlate_entry, 0);
    if (rv == SOC_E_NONE) {
        return BCM_E_EXISTS;
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    }
    rv = soc_mem_insert(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &egr_vlan_xlate_entry);

    return rv;
}

STATIC int
_bcm_tr2_mim_peer_port_config_delete(int unit, int vp, bcm_mim_vpn_t vpn) {
    mpls_entry_entry_t ment;
    egr_vlan_xlate_entry_t egr_vlan_xlate_entry;
    int vfi, rv = BCM_E_NONE;

    /* Delete the ISID-SVP entry in MPLS_ENTRY */
    vfi = _BCM_MIM_VPN_ID_GET(vpn);
    sal_memset(&ment, 0, sizeof(mpls_entry_entry_t));
    soc_MPLS_ENTRYm_field32_set(unit, &ment, KEY_TYPEf, 3); /* MIM_ISID_SVP */
    soc_MPLS_ENTRYm_field32_set(unit, &ment, VALIDf, 0x1);
    soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_ISID__ISIDf, 
                                VPN_ISID(unit, vfi));
    soc_MPLS_ENTRYm_field32_set(unit, &ment, MIM_ISID__SVPf, vp);
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &ment);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete the reverse entry in the egress */
    sal_memset(&egr_vlan_xlate_entry, 0, sizeof(egr_vlan_xlate_entry_t));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, ENTRY_TYPEf, 
                                    0x4);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, MIM_ISID__VFIf, 
                                    vfi);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, VALIDf, 0x1);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &egr_vlan_xlate_entry, 
                                    MIM_ISID__DVPf, vp);
    rv = soc_mem_delete(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ANY, 
                        &egr_vlan_xlate_entry);
    return rv;
}

/*
 * Function:
 *      bcm_mim_port_add
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vpn   - (IN)  VPN id
 *      mim_port  - (IN/OUT) MIM port configuration info
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_tr2_mim_port_add(int unit, bcm_mim_vpn_t vpn, bcm_mim_port_t *mim_port)
{
    int drop, mode, is_local = 0, rv = BCM_E_PARAM, nh_index = 0;
    int old_tpid_enable = 0, tpid_enable = 0, tpid_index;
    bcm_port_t local_port;
    int i, vp, num_vp, ipmc_id, vfi = -1;
    source_vp_entry_t svp;
    ing_dvp_table_entry_t dvp;
    vfi_entry_t vfi_entry;
    bcm_multicast_t mc_group = 0;

    MIM_INIT(unit);

    rv = bcm_xgs3_l3_egress_mode_get(unit, &mode);
    BCM_IF_ERROR_RETURN(rv);
    if (!mode) {
        soc_cm_debug(DK_L3, "L3 egress mode must be set first\n");
        return BCM_E_DISABLED;
    }

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }

    MIM_LOCK(unit);
    /* Allocate a new VP or use provided one */
    if (mim_port->flags & BCM_MIM_PORT_WITH_ID) {
        if (!BCM_GPORT_IS_MIM_PORT((mim_port->mim_port_id))) {
            MIM_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);
        if (_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
            if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
                MIM_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
        } else {
            rv = _bcm_vp_used_set(unit, vp, _bcmVpTypeMim);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                return rv;
            }
        }
    } else {
        /* allocate a new VP index */
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, _bcmVpTypeMim, &vp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
    }

    /* Deal with the different kinds of VPs */
    if (mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) {
        /* Deal with access VPs */
        if (vpn == _BCM_MIM_VPN_INVALID) {
            MIM_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        vfi = _BCM_MIM_VPN_ID_GET(vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            MIM_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        }
    } else if (mim_port->flags & BCM_MIM_PORT_TYPE_BACKBONE) {
        /* Deal with network VPs - no associated VPN, just a tunnel */
        if (!(mim_port->criteria == BCM_MIM_PORT_MATCH_TUNNEL_VLAN_SRCMAC)) {
            MIM_UNLOCK(unit);
            return BCM_E_PARAM;
        }
    } else if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
        /* Deal with peer VPs - tunnel and associated VPN */
        if (vpn == _BCM_MIM_VPN_INVALID) {
            MIM_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        vfi = _BCM_MIM_VPN_ID_GET(vpn);
        if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
            MIM_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        }
    }

    /* Program, the next hop, matching and egress tables */
    if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
        /* For existing access ports, NH entry already exists */
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
        rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            return rv;
        }
        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                                  NEXT_HOP_INDEXf);
        if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
            /* SD-tag mode, save the old TPID enable bits */
            old_tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp,
                                                         TPID_ENABLEf);
        }
    } else {
        sal_memset(&svp, 0, sizeof(source_vp_entry_t));
        sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    }

    /* Program next hop entry using existing or new index */
    drop = (mim_port->flags & BCM_MIM_PORT_DROP) ? 1 : 0;
    rv = _bcm_tr2_mim_l2_nh_info_add(unit, mim_port, vp, drop,
                                    &nh_index, &local_port, &is_local);
    if (rv < 0) {
        MIM_UNLOCK(unit);
        goto port_cleanup;
    }

    /* Program the VP tables */
    if (mim_port->criteria == BCM_MIM_PORT_MATCH_SERVICE_VLAN_TPID) {
        rv = _bcm_fb2_outer_tpid_entry_add(unit, mim_port->match_service_tpid,
                                           &tpid_index);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            goto port_cleanup;
        }
        tpid_enable = (1 << tpid_index);
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 1);
        soc_SOURCE_VPm_field32_set(unit, &svp, TPID_ENABLEf, tpid_enable);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, SD_TAG_MODEf, 0);
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, CLASS_IDf,
                               mim_port->if_class);
    soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_OUTER_DOT1Pf, 1);
    if ((mim_port->flags & BCM_MIM_PORT_TYPE_ACCESS) || 
        (mim_port->flags & BCM_MIM_PORT_TYPE_PEER)) {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 1);
    }

    /* Program the matching criteria */
    if (mim_port->flags & BCM_MIM_PORT_REPLACE) {
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            goto port_cleanup;
        }
    } else {
        /* Link in the newly allocated next-hop entry */
        soc_ING_DVP_TABLEm_field32_set(unit, &dvp, NEXT_HOP_INDEXf,
                                       nh_index);
        rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            goto port_cleanup;
        }

        /* Initialize the SVP parameters */
        soc_SOURCE_VPm_field32_set(unit, &svp,
                                   ENTRY_TYPEf, 0x1); /* L2 VP */
        if (vfi != -1) {
            soc_SOURCE_VPm_field32_set(unit, &svp, VFIf, vfi);
        }

        /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
        soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, 0x8);
        soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, 0x8);
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
        if (rv < 0) {
            MIM_UNLOCK(unit);
            goto port_cleanup;
        }

        /* Add the port to the VFI broadcast replication list */
        if (is_local && (vfi != -1)) {
            bcm_gport_t local_gport;

            /* Get IPMC index from HW */
            rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
            if (rv < 0) {
                MIM_UNLOCK(unit);
                goto port_cleanup;
            }
            ipmc_id = soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
            _BCM_MULTICAST_GROUP_SET(mc_group, _BCM_MULTICAST_TYPE_MIM, ipmc_id);
            /* Convert system local_port to physical local_port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &local_port);
            }
            rv = bcm_esw_port_gport_get(unit, local_port, &local_gport);
            if (BCM_FAILURE(rv)) {
                goto port_cleanup;
            }
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit)) {
                rv = bcm_tr_multicast_egress_add(unit, mc_group,
                                             local_gport, nh_index);
            } else 
#endif /* BCM_ENDURO_SUPPORT */
            {
                rv = bcm_tr2_multicast_egress_add(unit, mc_group,
                                             local_gport, nh_index + 
                                             BCM_XGS3_DVP_EGRESS_IDX_MIN);
            }
            if (rv < 0) {
                MIM_UNLOCK(unit);
                goto port_cleanup;
            }
        }

        /*
         * Match entries cannot be replaced, instead, callers
         * need to delete the existing entry and re-add with the
         * new match parameters.
         */
        rv = _bcm_tr2_mim_match_add(unit, mim_port, vp);

        if (rv < 0) {
            bcm_gport_t local_gport;
            if (is_local) {
                (void) bcm_esw_port_gport_get(unit, local_port, &local_gport);
#if defined(BCM_ENDURO_SUPPORT)
                if (SOC_IS_ENDURO(unit)) {
                    (void) bcm_tr_multicast_egress_delete(unit, mc_group, 
                                                       local_gport, nh_index);
                } else 
#endif /* BCM_ENDURO_SUPPORT */
                {
                    (void) bcm_tr2_multicast_egress_delete(unit, mc_group, 
                                                       local_gport, nh_index + 
                                                       BCM_XGS3_DVP_EGRESS_IDX_MIN);
                }
            }
        } else {
            /* For peer ports, create entries in ingress and egress 
               VLAN translate tables */
            if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
                rv = _bcm_tr2_mim_peer_port_config_add(unit, mim_port, vp, vpn);
            }
            /* Set the MIM port ID */
            BCM_GPORT_MIM_PORT_ID_SET(mim_port->mim_port_id, vp);
            mim_port->encap_id = nh_index;
        }
    }
    MIM_UNLOCK(unit);
port_cleanup:
    if (rv < 0) {
        if (tpid_enable) {
            (void) _bcm_fb2_outer_tpid_entry_delete(unit, tpid_index);
        }
        if (!(mim_port->flags & BCM_MIM_PORT_REPLACE)) {
            (void) _bcm_vp_free(unit, _bcmVpTypeMim, 1, vp);
            _bcm_tr2_mim_l2_nh_info_delete(unit, nh_index);
        }
    }
    if (old_tpid_enable) {
        for (i = 0; i < 4; i++) {
            if (old_tpid_enable & (1 << i)) {
                (void) _bcm_fb2_outer_tpid_entry_delete(unit, i);
                break;
            }
        }
    }
    return rv;
}

STATIC int
_bcm_tr2_mim_port_delete(int unit, bcm_vpn_t vpn, int vp)
{
    int rv = BCM_E_NONE;
    int nh_index = 0, vfi, ipmc_id, is_local;
    source_vp_entry_t svp;
    ing_dvp_table_entry_t dvp;
    ing_l3_next_hop_entry_t ing_nh;
    vfi_entry_t vfi_entry;
    bcm_module_t modid;
    bcm_port_t port;
    bcm_multicast_t mc_group = 0;
    bcm_gport_t local_gport;

    vfi = _BCM_MIM_VPN_ID_GET(vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
    BCM_IF_ERROR_RETURN(rv);
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

    rv = _bcm_tr2_mim_match_delete(unit, vp);
    if (rv < 0) {
        return rv;
    }

    /* Check if this port is on the local unit */
    rv = soc_mem_read(unit, ING_L3_NEXT_HOPm,
                      MEM_BLOCK_ANY, nh_index, &ing_nh);
    BCM_IF_ERROR_RETURN(rv);

    if (!soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf) &&
        !(MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_NETWORK)) {
        modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
        port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, modid, &is_local));
        /* If this port is on the local unit, remove from replication list */
        if (TRUE == is_local) {
            /* Get IPMC index from HW */
            rv = READ_VFIm(unit, MEM_BLOCK_ANY, vfi, &vfi_entry);
            BCM_IF_ERROR_RETURN(rv);
            ipmc_id = soc_VFIm_field32_get(unit, &vfi_entry, BC_INDEXf);
            _BCM_MULTICAST_GROUP_SET(mc_group, _BCM_MULTICAST_TYPE_MIM, ipmc_id);
            /* Convert system port to physical port */
            if (soc_feature(unit, soc_feature_sysport_remap)) {
                BCM_XLATE_SYSPORT_S2P(unit, &port);
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_gport_get(unit, port, &local_gport));
#if defined(BCM_ENDURO_SUPPORT)            
            if (SOC_IS_ENDURO(unit)) {
                rv = bcm_tr_multicast_egress_delete
                     (unit, mc_group, local_gport, nh_index);
            } else 
#endif /* BCM_ENDURO_SUPPORT */
            {
                rv = bcm_tr2_multicast_egress_delete
                     (unit, mc_group, local_gport, nh_index 
                      + BCM_XGS3_DVP_EGRESS_IDX_MIN);
            }
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    /* Clear the SVP and DVP table entries */
    
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(&dvp, 0, sizeof(ing_dvp_table_entry_t));
    rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear the next-hop table entries */
    rv = _bcm_tr2_mim_l2_nh_info_delete(unit, nh_index);
    BCM_IF_ERROR_RETURN(rv);

    /* For peer ports, delete entries in ingress and egress 
       VLAN translate tables */
    if (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_PEER) {
        rv = _bcm_tr2_mim_peer_port_config_delete(unit, vp, vpn);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Clear the SW state */
    sal_memset(&(MIM_INFO(unit)->port_info[vp]), 0, 
               sizeof(_bcm_tr2_mim_port_info_t));

    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        bcm_gport_t gport;

        /* Release Service counter, if any */
        BCM_GPORT_MIM_PORT_ID_SET(gport, vp);
        _bcm_esw_flex_stat_handle_free(unit, _bcmFlexStatTypeGport, gport);
    }

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeMim, 1, vp);
    return rv;
}

/*
 * Function:
 *      bcm_mim_port_delete
 * Purpose:
 *      Delete an mim port from a VPN
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      mim_port_id - (IN) mim port information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_port_delete(int unit, bcm_vpn_t vpn, bcm_gport_t mim_port_id)
{
    int vp, rv;

    MIM_INIT(unit);
    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        return BCM_E_NOT_FOUND;
    }
    MIM_LOCK(unit);
    rv = _bcm_tr2_mim_port_delete(unit, vpn, vp);
    MIM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_mim_port_delete_all
 * Purpose:
 *      Delete all mpls ports from a VPN
 * Parameters:
 *      unit - Device Number
 *      vpn - VPN instance ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_port_delete_all(int unit, bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    uint32 vfi, vp, num_vp;
    source_vp_entry_t svp;

    MIM_INIT(unit);

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }

    vfi = _BCM_MIM_VPN_ID_GET(vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        rv =  BCM_E_NOT_FOUND;
        goto done;
    }
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (vp = 0; vp < num_vp; vp++) {
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            goto done;
        }
        if ((soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) != 0) &&
            (vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf))) {
            rv = _bcm_tr2_mim_port_delete(unit, vpn, vp);
            if (rv < 0) {
                goto done;
            }
        }
    }
done:
    return rv;
}

STATIC int
_bcm_tr2_mim_l2_nh_info_get(int unit, bcm_mim_port_t *mim_port, int nh_index)
{
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_l3_intf_entry_t egr_intf;
    egr_mac_da_profile_entry_t macda;
    int rv, action_present, action_not_present;
    int i, intf_num = -1, macda_idx = -1, tpid_idx = -1;
    bcm_module_t mod_out, mod_in;
    bcm_port_t port_out, port_in;
    bcm_trunk_t trunk_id;

    /* Read the HW entries */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, 
                                                ENTRY_TYPEf) == 0x2) {
        if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
            trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
            BCM_GPORT_TRUNK_SET(mim_port->port, trunk_id);
        } else {
            mod_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
            port_in = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);

            rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);
        }
        action_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG_ACTION_IF_PRESENTf);
        if (action_present) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED;
        }
        action_not_present =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             SD_TAG_ACTION_IF_NOT_PRESENTf);

        if ((action_not_present == 0x1) || (action_present == 0x1)) {
            /* If SD tag action is ADD or REPLACE_VID_TPID, the tpid
             * index of the entry is valid. Get the tpid index for later.
             */
            tpid_idx =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                                 SD_TAG_TPID_INDEXf);
            mim_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG_VIDf);
            if (action_not_present) {
                mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD;
            }
            if (action_present) {
                mim_port->flags |= 
                    BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE;
            }
            for (i = 0; i < 4; i++) {
                if (tpid_idx & (1 << i)) {
                    _bcm_fb2_outer_tpid_entry_get(unit, 
                                          &mim_port->egress_service_tpid, i);
                }
            }
        } else if (action_present == 0x2) { /* REPLACE_VID_ONLY */
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE;
            mim_port->egress_service_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, SD_TAG_VIDf);
        } else if (action_present == 0x3) { /* DELETE */
            mim_port->flags |= BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE;
        }
    } else {
        return BCM_E_NOT_FOUND;
    }

    if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                ENTRY_TYPEf) == 0x3) {
        /* Backbone / peer VP */
        /* Get the tunnel encap attributes */
        mim_port->egress_tunnel_vlan =
                soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, MIM__BVIDf);
        intf_num = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                    MIM__INTF_NUMf);
        macda_idx = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, 
                                                    MIM__MAC_DA_PROFILE_INDEXf);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm, MEM_BLOCK_ANY, 
                                          intf_num, &egr_intf));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_MAC_DA_PROFILEm,  
                                         MEM_BLOCK_ANY, macda_idx, &macda));
        soc_mem_mac_addr_get(unit, EGR_L3_INTFm, &egr_intf, MAC_ADDRESSf, 
                             mim_port->egress_tunnel_srcmac); 
        soc_mem_mac_addr_get(unit, EGR_MAC_DA_PROFILEm, &macda, MAC_ADDRESSf, 
                             mim_port->egress_tunnel_dstmac); 
        if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             MIM__ISID_LOOKUP_TYPEf) == 0) {
            mim_port->flags |= BCM_MIM_PORT_TYPE_BACKBONE;
        } else {
            mim_port->flags |= BCM_MIM_PORT_TYPE_PEER;
        }
        if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                                             MIM__ADD_ISID_TO_MACDAf) == 1) {
            mim_port->flags |= BCM_MIM_PORT_EGRESS_TUNNEL_DEST_MAC_USE_SERVICE;
        }
    }

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, DROPf)) {
        mim_port->flags |= BCM_MIM_PORT_DROP;
    }
	
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_mim_match_get(int unit, bcm_mim_port_t *mim_port, int vp)
{
    int modid_count, rv = BCM_E_NONE;
    bcm_module_t mod_in, mod_out;
    bcm_port_t port_in, port_out;
    bcm_trunk_t trunk_id;
    uint64 temp_mac;

    if ((MIM_INFO(unit)->port_info[vp].flags & 
         _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN) || 
        (MIM_INFO(unit)->port_info[vp].flags & 
         _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED)) {

        mim_port->flags |= BCM_MIM_PORT_TYPE_ACCESS;
        mim_port->match_vlan = MIM_INFO(unit)->port_info[vp].match_vlan;

        if ((MIM_INFO(unit)->port_info[vp].flags & 
             _BCM_MIM_PORT_TYPE_ACCESS_PORT_VLAN_STACKED)) {
            mim_port->match_inner_vlan = 
                MIM_INFO(unit)->port_info[vp].match_inner_vlan;
            mim_port->criteria = BCM_MIM_PORT_MATCH_PORT_VLAN_STACKED; 
        } else {
            mim_port->criteria = BCM_MIM_PORT_MATCH_PORT_VLAN;
        }

        if ((MIM_INFO(unit)->port_info[vp].tgid != -1)) {
            trunk_id = MIM_INFO(unit)->port_info[vp].tgid;
            BCM_GPORT_TRUNK_SET(mim_port->port, trunk_id);
        } else {
            mod_in = MIM_INFO(unit)->port_info[vp].modid;
            port_in = MIM_INFO(unit)->port_info[vp].port;

            rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);
        }

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_PORT) {
        mim_port->flags |= (BCM_MIM_PORT_TYPE_ACCESS);
        mim_port->criteria = BCM_MIM_PORT_MATCH_PORT;
        modid_count = SOC_MODID_MAX(unit) + 1;
        port_in = MIM_INFO(unit)->port_info[vp].index % modid_count;
        mod_in = MIM_INFO(unit)->port_info[vp].index / modid_count;

        rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                mod_in, port_in, &mod_out, &port_out);
        BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               _BCM_MIM_PORT_TYPE_ACCESS_LABEL) {
        mim_port->flags |= (BCM_MIM_PORT_TYPE_ACCESS);
        mim_port->criteria = BCM_MIM_PORT_MATCH_LABEL;
        mim_port->match_label = MIM_INFO(unit)->port_info[vp].match_label;

        if ((MIM_INFO(unit)->port_info[vp].tgid != -1)) {
            trunk_id = MIM_INFO(unit)->port_info[vp].tgid;
            BCM_GPORT_TRUNK_SET(mim_port->port, trunk_id);
        } else {
            mod_in = MIM_INFO(unit)->port_info[vp].modid;
            port_in = MIM_INFO(unit)->port_info[vp].port;

            rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);
        }

    } else if (MIM_INFO(unit)->port_info[vp].flags & 
               (_BCM_MIM_PORT_TYPE_NETWORK | _BCM_MIM_PORT_TYPE_PEER)) {
        mim_port->criteria = BCM_MIM_PORT_MATCH_TUNNEL_VLAN_SRCMAC;
        if (MIM_INFO(unit)->port_info[vp].flags & _BCM_MIM_PORT_TYPE_PEER) {
            mim_port->flags |= BCM_MIM_PORT_TYPE_BACKBONE;
        } else {
            mim_port->flags |= BCM_MIM_PORT_TYPE_PEER;
        }

        SAL_MAC_ADDR_TO_UINT64
           (MIM_INFO(unit)->port_info[vp].match_tunnel_srcmac, temp_mac);
        SAL_MAC_ADDR_FROM_UINT64(mim_port->match_tunnel_srcmac, temp_mac);
        mim_port->match_tunnel_vlan = 
            MIM_INFO(unit)->port_info[vp].match_tunnel_vlan; 

        if ((MIM_INFO(unit)->port_info[vp].tgid != -1)) {
            trunk_id = MIM_INFO(unit)->port_info[vp].tgid;
            BCM_GPORT_TRUNK_SET(mim_port->port, trunk_id);
        } else {
            mod_in = MIM_INFO(unit)->port_info[vp].modid;
            port_in = MIM_INFO(unit)->port_info[vp].port;

            rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in, &mod_out, &port_out);
            BCM_GPORT_MODPORT_SET(mim_port->port, mod_out, port_out);
        }
    }
    return rv;
}

STATIC int
_bcm_tr2_mim_port_get(int unit, bcm_vpn_t vpn, int vp,
                        bcm_mim_port_t *mim_port)
{
    int i, nh_index, tpid_enable = 0, rv = BCM_E_NONE;
    ing_dvp_table_entry_t dvp;
    source_vp_entry_t svp;

    /* Initialize the structure */
    bcm_mim_port_t_init(mim_port);
    BCM_GPORT_MIM_PORT_ID_SET(mim_port->mim_port_id, vp);

    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);

    /* Get the match parameters */
    rv = _bcm_tr2_mim_match_get(unit, mim_port, vp);
    BCM_IF_ERROR_RETURN(rv);

    /* Get the next-hop parameters */
    rv = _bcm_tr2_mim_l2_nh_info_get(unit, mim_port, nh_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Fill in SVP parameters */
    BCM_IF_ERROR_RETURN (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
    mim_port->if_class = soc_SOURCE_VPm_field32_get(unit, &svp, CLASS_IDf);
    if (soc_SOURCE_VPm_field32_get(unit, &svp, SD_TAG_MODEf)) {
        tpid_enable = soc_SOURCE_VPm_field32_get(unit, &svp, TPID_ENABLEf);
        if (tpid_enable) {
            mim_port->criteria = BCM_MIM_PORT_MATCH_SERVICE_VLAN_TPID;
            for (i = 0; i < 4; i++) {
                if (tpid_enable & (1 << i)) {
                    _bcm_fb2_outer_tpid_entry_get(unit, 
                        &mim_port->match_service_tpid, i);
                }
            }
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_mim_port_get
 * Purpose:
 *      Get an mim port
 * Parameters:
 *      unit       - (IN) Device Number
 *      vpn        - (IN) VPN instance ID
 *      mim_port  - (IN/OUT) mim port information
 */
int
bcm_tr2_mim_port_get(int unit, bcm_vpn_t vpn, bcm_mim_port_t *mim_port)
{
    int vp;

    MIM_INIT(unit);

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        return BCM_E_NOT_FOUND;
    }
    return _bcm_tr2_mim_port_get(unit, vpn, vp, mim_port);
}

/*
 * Function:
 *      bcm_mim_port_get_all
 * Purpose:
 *      Get an mim port from a VPN
 * Parameters:
 *      unit     - (IN) Device Number
 *      vpn      - (IN) VPN instance ID
 *      port_max   - (IN) Maximum number of interfaces in array
 *      port_array - (OUT) Array of mpls ports
 *      port_count - (OUT) Number of interfaces returned in array
 *
 */
int
bcm_tr2_mim_port_get_all(int unit, bcm_vpn_t vpn, int port_max,
                        bcm_mim_port_t *port_array, int *port_count)
{
    int vp, rv = BCM_E_NONE;
    uint32 vfi, num_vp;
    source_vp_entry_t svp;

    MIM_INIT(unit);

    if (!_BCM_MIM_VPN_IS_SET(vpn)) {
        return BCM_E_PARAM;
    }

    *port_count = 0;

    vfi = _BCM_MIM_VPN_ID_GET(vpn);
    if (!_bcm_vfi_used_get(unit, vfi, _bcmVfiTypeMim)) {
        rv = BCM_E_NOT_FOUND;
        goto done;
    }
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (vp = 0; vp < num_vp; vp++) {
        if (*port_count == port_max) {
            break;
        }
        rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
        if (rv < 0) {
            goto done;
        }
        if (vfi == soc_SOURCE_VPm_field32_get(unit, &svp, VFIf)) {
            rv = _bcm_tr2_mim_port_get(unit, vpn, vp, 
                                       &port_array[*port_count]);
            if (rv < 0) {
                goto done;
            }
            (*port_count)++;
        }
    }
done:
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_mim_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a MIM port
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_mim_port_resolve(int unit, bcm_gport_t mim_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;

    MIM_INIT(unit);

    if (!BCM_GPORT_IS_MIM_PORT(mim_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x2) {
        /* Entry type is not L2 DVP */
        return BCM_E_NOT_FOUND;
    }
    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
        *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
    } else {
        /* Only add this to replication set if destination is local */
        *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
        *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
    }
    *id = vp;
    return rv;
}

/*
 * Function:
 *      bcm_tr2_mim_port_learn_get
 * Purpose:
 *      Get the CML bits for an mim port
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_port_learn_get(int unit, bcm_gport_t mim_port_id, uint32 *flags)
{
    int rv, vp, cml = 0;
    source_vp_entry_t svp;

    MIM_INIT(unit);

    /* Get the VP index from the gport */
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id);

    MEM_LOCK(unit, SOURCE_VPm);
    /* Be sure the entry is used and is set for VPLS */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return rv;
    }
    if (soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) != 1) { /* L2 VP */
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    MEM_UNLOCK(unit, SOURCE_VPm);
    cml = soc_SOURCE_VPm_field32_get(unit, &svp, CML_FLAGS_NEWf);

    *flags = 0;
    if (!(cml & (1 << 0))) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (cml & (1 << 1)) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (cml & (1 << 2)) {
       *flags |= BCM_PORT_LEARN_PENDING;
    }
    if (cml & (1 << 3)) {
       *flags |= BCM_PORT_LEARN_ARL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_mim_port_learn_set
 * Purpose:
 *      Set the CML bits for an mim port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_mim_port_learn_set(int unit, bcm_gport_t mim_port_id, uint32 flags)
{
    int vp, cml = 0, rv = BCM_E_NONE;
    source_vp_entry_t svp;

    MIM_INIT(unit);

    cml = 0;
    if (!(flags & BCM_PORT_LEARN_FWD)) {
       cml |= (1 << 0);
    }
    if (flags & BCM_PORT_LEARN_CPU) {
       cml |= (1 << 1);
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
       cml |= (1 << 2);
    }
    if (flags & BCM_PORT_LEARN_ARL) {
       cml |= (1 << 3);
    }

    /* Get the VP index from the gport */
    vp = BCM_GPORT_MIM_PORT_ID_GET(mim_port_id);

    MEM_LOCK(unit, SOURCE_VPm);
    /* Be sure the entry is used and is set for MIM */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
        return BCM_E_NOT_FOUND;
    }
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MEM_UNLOCK(unit, SOURCE_VPm);
        return rv;
    }
    if (soc_SOURCE_VPm_field32_get(unit, &svp, ENTRY_TYPEf) != 1) { /* L2 VP */
        MEM_UNLOCK(unit, SOURCE_VPm);
        return BCM_E_NOT_FOUND;
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml);
    soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    MEM_UNLOCK(unit, SOURCE_VPm);
    return rv;
}

int
_bcm_esw_mim_flex_stat_index_set(int unit, bcm_gport_t port, int fs_idx)
{
    int rv, vp, nh_index;
    ing_dvp_table_entry_t dvp;

    vp = BCM_GPORT_MIM_PORT_ID_GET(port);

    MIM_LOCK(unit);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
        MIM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    } else {
        /* Ingress side */
        rv = soc_mem_field32_modify(unit, SOURCE_VPm, vp, VINTF_CTR_IDXf, 
                                    fs_idx);

        if (BCM_SUCCESS(rv)) {
            rv = READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp);
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
            /* Egress side */
            if (BCM_SUCCESS(rv)) {
                rv = soc_reg_field32_modify(unit, EGR_L3_NEXT_HOPm, nh_index,
                                            VINTF_CTR_IDXf, fs_idx);
            }
        }
    }

    MIM_UNLOCK(unit);
    return rv;
}

int
_bcm_tr2_mim_svp_field_set(int unit, bcm_gport_t vp, 
                           soc_field_t field, int value)
{
    int rv = BCM_E_NONE;
    source_vp_entry_t svp;

    MIM_LOCK(unit);
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (rv < 0) {
        MIM_UNLOCK(unit);
        return rv;
    }
    soc_SOURCE_VPm_field32_set(unit, &svp, field, value);
    rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
    MIM_UNLOCK(unit);
    return rv;
}

#endif /* BCM_TRIUMPH2_SUPPORT */
