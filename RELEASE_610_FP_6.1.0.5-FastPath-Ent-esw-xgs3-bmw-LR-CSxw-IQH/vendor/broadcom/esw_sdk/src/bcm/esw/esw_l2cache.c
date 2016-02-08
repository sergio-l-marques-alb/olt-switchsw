/*
 * $Id: esw_l2cache.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * L2 Cache - Layer 2 BPDU and overflow address cache
 */

#include <sal/core/libc.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/l2u.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/vlan.h>

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_XGS3_SWITCH_SUPPORT
/*
 * Function:
 *      _bcm_l2_from_l2u
 * Purpose:
 *      Convert L2 User table entry to a hardware-independent L2 cache entry.
 * Parameters:
 *      unit - Unit number
 *      l2caddr - (OUT) Hardware-independent L2 entry
 *      l2u_entry - Firebolt L2 User entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_cache_from_l2u(int unit, 
                       bcm_l2_cache_addr_t *l2caddr, l2u_entry_t *l2u_entry)
{
    bcm_module_t    mod_in = 0, mod_out;
    bcm_port_t      port_in = 0, port_out;
    uint32          mask[2];
    int             skip_l2u;

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        sal_memset(l2caddr, 0, sizeof (*l2caddr));

        if (!_l2u_field32_get(unit, l2u_entry, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }

        _l2u_mac_addr_get(unit, l2u_entry, MAC_ADDRf, l2caddr->mac);

        l2caddr->vlan = _l2u_field32_get(unit, l2u_entry, VLAN_IDf);
        l2caddr->prio = _l2u_field32_get(unit, l2u_entry, PRIf);

        if (_l2u_field32_get(unit, l2u_entry, RPEf)) {
            l2caddr->flags |= BCM_L2_CACHE_SETPRI;
        }

#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            if (_l2u_field32_get(unit, l2u_entry, MH_OPCODE_5f)) {
                l2caddr->flags |= BCM_L2_CACHE_REMOTE_LOOKUP;
            }
            if (_l2u_field32_get(unit, l2u_entry, MY_STATIONf)) {
                l2caddr->flags |= BCM_L2_CACHE_TUNNEL;
            }
        }
#endif

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if ((SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit)
             || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) &&
            _l2u_field32_get(unit, l2u_entry, DO_NOT_LEARN_MACSAf)) {
            l2caddr->flags |= BCM_L2_CACHE_LEARN_DISABLE;
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

        if (_l2u_field32_get(unit, l2u_entry, CPUf)) {
            l2caddr->flags |= BCM_L2_CACHE_CPU;
        }

        if (_l2u_field32_get(unit, l2u_entry, BPDUf)) {
            l2caddr->flags |= BCM_L2_CACHE_BPDU;
        }

        if (_l2u_field32_get(unit, l2u_entry, DST_DISCARDf)) {
            l2caddr->flags |= BCM_L2_CACHE_DISCARD;
        }

        if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            if (_l2u_field32_get(unit, l2u_entry, Tf)) {
                l2caddr->flags |= BCM_L2_CACHE_TRUNK;
                l2caddr->dest_trunk = _l2u_field32_get(unit, l2u_entry, TGIDf);
            } else {
                mod_in = _l2u_field32_get(unit, l2u_entry, MODULE_IDf);
                port_in = _l2u_field32_get(unit, l2u_entry, PORT_NUMf);
            }
        } else {
            mod_in = _l2u_field32_get(unit, l2u_entry, MODULE_IDf);
            port_in = _l2u_field32_get(unit, l2u_entry, PORT_TGIDf);
            if (port_in & BCM_TGID_TRUNK_INDICATOR(unit)) {
                l2caddr->flags |= BCM_L2_CACHE_TRUNK;
                l2caddr->dest_trunk = BCM_MODIDf_TGIDf_TO_TRUNK(unit, mod_in,
                                                                port_in);
            }
        }
        if (!(l2caddr->flags & BCM_L2_CACHE_TRUNK)) {
            if (!SOC_MODID_ADDRESSABLE(unit, mod_in)) {
                return BCM_E_BADID;
            }
            BCM_IF_ERROR_RETURN(bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                       mod_in, port_in, &mod_out,
                                                       &port_out));
            l2caddr->dest_modid = mod_out;
            l2caddr->dest_port = port_out;
        }

        if (_l2u_field32_get(unit, l2u_entry, L3f)) {
            l2caddr->flags |= BCM_L2_CACHE_L3;
        }

        if (_l2u_field32_get(unit, l2u_entry, MIRRORf)) {
            l2caddr->flags |= BCM_L2_CACHE_MIRROR;
        }

        _l2u_mac_addr_get(unit, l2u_entry, MASKf, l2caddr->mac_mask);
        _l2u_field_get(unit, l2u_entry, MASKf, mask);
        l2caddr->vlan_mask = (mask[1] >> 16) & 0xfff;

#if defined(BCM_EASYRIDER_SUPPORT) 
        if (SOC_IS_EASYRIDER(unit)) {
            l2caddr->src_port = _l2u_field32_get(unit, l2u_entry, SRC_PORTf);
            l2caddr->src_port_mask = mask[1] >> 28;
            if (l2caddr->src_port_mask == 0xf) {
                /* Normalize */
                l2caddr->src_port_mask = BCM_L2_SRCPORT_MASK_ALL;
            }
        }
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_TRX_SUPPORT) 
        if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, CLASS_IDf)) {
            l2caddr->lookup_class = _l2u_field32_get(unit, l2u_entry, CLASS_IDf);
        }
#endif /* BCM_TRX_SUPPORT */

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_fb_l2_to_l2u
 * Purpose:
 *      Convert a hardware-independent L2 cache entry to a L2 User table entry.
 * Parameters:
 *      unit - Unit number
 *      l2u_entry - (OUT) Firebolt L2 User entry
 *      l2caddr - Hardware-independent L2 entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_cache_to_l2u(int unit, 
                     l2u_entry_t *l2u_entry, bcm_l2_cache_addr_t *l2caddr)
{
    bcm_module_t    mod_in, mod_out;
    bcm_port_t      port_in, port_out;
    uint32          mask[2];
    soc_field_t     port_field = 0;
    int             skip_l2u;

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        VLAN_CHK_ID(unit, l2caddr->vlan);
        VLAN_CHK_PRIO(unit, l2caddr->prio);

        sal_memset(l2u_entry, 0, sizeof (*l2u_entry));

        _l2u_field32_set(unit, l2u_entry, VALIDf, 1);
        _l2u_mac_addr_set(unit, l2u_entry, MAC_ADDRf, l2caddr->mac);
        _l2u_field32_set(unit, l2u_entry, VLAN_IDf, l2caddr->vlan);
        _l2u_field32_set(unit, l2u_entry, PRIf, l2caddr->prio);

        if (l2caddr->flags & BCM_L2_CACHE_SETPRI) {
            _l2u_field32_set(unit, l2u_entry, RPEf, 1);
        }

#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            if (l2caddr->flags & BCM_L2_CACHE_REMOTE_LOOKUP) {
                _l2u_field32_set(unit, l2u_entry, MH_OPCODE_5f, 1);
            }
            if (l2caddr->flags & BCM_L2_CACHE_TUNNEL) {
                _l2u_field32_set(unit, l2u_entry, MY_STATIONf, 1);
            }
        }
#endif /* BCM_EASYRIDER_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if ((SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit)
             || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) &&
            (l2caddr->flags & BCM_L2_CACHE_LEARN_DISABLE)) {
            _l2u_field32_set(unit, l2u_entry, DO_NOT_LEARN_MACSAf, 1);
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

        if (l2caddr->flags & BCM_L2_CACHE_CPU) {
            _l2u_field32_set(unit, l2u_entry, CPUf, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_BPDU) {
            _l2u_field32_set(unit, l2u_entry, BPDUf, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_DISCARD) {
            _l2u_field32_set(unit, l2u_entry, DST_DISCARDf, 1);
        }

        if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
                _l2u_field32_set(unit, l2u_entry, Tf, 1);
                _l2u_field32_set(unit, l2u_entry, TGIDf, l2caddr->dest_trunk);
            } else {
                port_field = PORT_NUMf;
            }
        } else {
            if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
                _l2u_field32_set(unit, l2u_entry, MODULE_IDf,
                                 BCM_TRUNK_TO_MODIDf(unit,
                                                     l2caddr->dest_trunk));
                _l2u_field32_set(unit, l2u_entry, PORT_TGIDf,
                                 BCM_TRUNK_TO_TGIDf(unit,
                                                    l2caddr->dest_trunk));
            } else {
                port_field = PORT_TGIDf;
            }
        }

        if (!(l2caddr->flags & BCM_L2_CACHE_TRUNK)) {
            mod_in = l2caddr->dest_modid;
            port_in = l2caddr->dest_port;
            PORT_DUALMODID_VALID(unit, port_in);
            BCM_IF_ERROR_RETURN
                (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                        mod_in, port_in, &mod_out, &port_out));
            if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return BCM_E_PORT;
            }
            _l2u_field32_set(unit, l2u_entry, MODULE_IDf, mod_out);
            _l2u_field32_set(unit, l2u_entry, port_field, port_out);
        }

        if (l2caddr->flags & BCM_L2_CACHE_L3) {
            _l2u_field32_set(unit, l2u_entry, L3f, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_MIRROR) {
            _l2u_field32_set(unit, l2u_entry, MIRRORf, 1);
        }

        _l2u_mac_addr_set(unit, l2u_entry, MASKf, l2caddr->mac_mask);
        _l2u_field_get(unit, l2u_entry, MASKf, mask);
        mask[1] |= (l2caddr->vlan_mask & 0xfff) << 16;
#if defined(BCM_EASYRIDER_SUPPORT)
        if (SOC_IS_EASYRIDER(unit)) {
            _l2u_field32_set(unit, l2u_entry, SRC_PORTf, l2caddr->src_port);
            mask[1] |= (l2caddr->src_port_mask & 0xf) << 28;
        }
#endif /* BCM_EASYRIDER_SUPPORT */

        _l2u_field_set(unit, l2u_entry, MASKf, mask);

#if defined(BCM_TRX_SUPPORT) 
        if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, CLASS_IDf)) {
            _l2u_field32_set(unit, l2u_entry, CLASS_IDf, l2caddr->lookup_class);
        }
#endif /* BCM_TRX_SUPPORT */

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

/*
 * Function:
 *      bcm_esw_l2_cache_init
 * Purpose:
 *      Initialize the L2 cache
 * Parameters:
 *      unit - device number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Clears all entries and preloads a few BCM_L2_CACHE_BPDU
 *      entries to match previous generation of devices.
 */
int
bcm_esw_l2_cache_init(int unit)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int     skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        bcm_l2_cache_addr_t addr;
        l2u_entry_t entry;
        int index;

        if (!SAL_BOOT_QUICKTURN) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, L2_USER_ENTRYm, COPYNO_ALL, TRUE));
        }
        
        bcm_l2_cache_addr_t_init(&addr);
        addr.flags = BCM_L2_CACHE_CPU | BCM_L2_CACHE_BPDU;
        /* Set default BPDU addresses (01:80:c2:00:00:00) */
        ENET_SET_MACADDR(addr.mac, _soc_mac_spanning_tree);
        ENET_SET_MACADDR(addr.mac_mask, _soc_mac_all_ones);
        BCM_IF_ERROR_RETURN(_bcm_l2_cache_to_l2u(unit, &entry, &addr));
        for (index = 0; index < L2U_BPDU_COUNT; index++) {
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, index, &index));
        }

        if (!SAL_BOOT_QUICKTURN) {
            /* Set 01:80:c2:00:00:10 */
            addr.mac[5] = 0x10;
            BCM_IF_ERROR_RETURN(_bcm_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
            /* Set 01:80:c2:00:00:0x */
            addr.mac[5] = 0x00;
            addr.mac_mask[5] = 0xf0;
            BCM_IF_ERROR_RETURN(_bcm_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
            /* Set 01:80:c2:00:00:2x */
            addr.mac[5] = 0x20;
            BCM_IF_ERROR_RETURN(_bcm_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
        }

        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_esw_l2_cache_size_get
 * Purpose:
 *      Get number of L2 cache entries
 * Parameters:
 *      unit - device number
 *      size - (OUT) number of entries in cache
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_size_get(int unit, int *size)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        *size = soc_mem_index_count(unit, L2_USER_ENTRYm);

        return 0;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_cache_set
 * Purpose:
 *      Set an L2 cache entry
 * Parameters:
 *      unit - device number
 *      index - l2 cache entry number (or -1)
 *              If -1 is given then the entry used will be the first
 *              available entry if the l2 cache address mac_mask
 *              field is all ones, or the last available entry if
 *              the mac_mask field has some zeros.  Cache entries
 *              are matched from lowest entry to highest entry.
 *      addr - l2 cache address
 *      index_used - (OUT) l2 cache entry number actually used
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_set(int unit, int index, bcm_l2_cache_addr_t *addr,
                 int *index_used)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        l2u_entry_t l2u_entry;

        if ((addr->lookup_class > SOC_ADDR_CLASS_MAX(unit)) || 
            (addr->lookup_class < 0)) {
            return (BCM_E_PARAM);
        }

        BCM_IF_ERROR_RETURN(
            _bcm_l2_cache_to_l2u(unit, &l2u_entry, addr));
        
        SOC_IF_ERROR_RETURN(
            soc_l2u_insert(unit, &l2u_entry, index, index_used));

        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_cache_get
 * Purpose:
 *      Get an L2 cache entry
 * Parameters:
 *      unit - device number
 *      index - l2 cache entry index
 *      size - (OUT) l2 cache address
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_get(int unit, int index, bcm_l2_cache_addr_t *addr)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        l2u_entry_t l2u_entry;

        SOC_IF_ERROR_RETURN(
            soc_l2u_get(unit, &l2u_entry, index));

        BCM_IF_ERROR_RETURN(
            _bcm_l2_cache_from_l2u(unit, addr, &l2u_entry));

        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_cache_delete
 * Purpose:
 *      Clear an L2 cache entry
 * Parameters:
 *      unit - device number
 *      index - l2 cache entry index
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_delete(int unit, int index)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        return soc_l2u_delete(unit, NULL, index, &index);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_cache_delete_all
 * Purpose:
 *      Clear all L2 cache entries
 * Parameters:
 *      unit - device number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_delete_all(int unit)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        return soc_l2u_delete_all(unit);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}
