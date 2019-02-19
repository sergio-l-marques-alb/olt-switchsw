/*
 * $Id: niv.c 1.26 Broadcom SDK $
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
 * File:    niv.c
 * Purpose: Implements NIV APIs for Trident.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/l2x.h>

#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/triumph3.h>

/*
 * Software book keeping for NIV related information
 */
typedef struct _bcm_trident_niv_port_info_s {
    uint32 flags;
    bcm_gport_t port;
    uint16 virtual_interface_id;
    bcm_vlan_t match_vlan;
} _bcm_trident_niv_port_info_t;

typedef struct _bcm_trident_niv_bookkeeping_s {
    _bcm_trident_niv_port_info_t *port_info; /* VP state */
} _bcm_trident_niv_bookkeeping_t;

STATIC _bcm_trident_niv_bookkeeping_t _bcm_trident_niv_bk_info[BCM_MAX_NUM_UNITS];

#define NIV_INFO(unit) (&_bcm_trident_niv_bk_info[unit])
#define NIV_PORT_INFO(unit, vp) (&NIV_INFO(unit)->port_info[vp])

/*
 * Function:
 *      _bcm_trident_niv_port_cnt_update
 * Purpose:
 *      Update port's VP count.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      gport - (IN) GPORT ID.
 *      vp    - (IN) Virtual port number.
 *      incr  - (IN) If TRUE, increment VP count, else decrease VP count.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trident_niv_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr)
{
    int mod_out, port_out, tgid_out, id_out;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count;
    int idx;
    int mod_local;
    _bcm_port_info_t *port_info;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &mod_out, 
                                &port_out, &tgid_out, &id_out));
    if (-1 != id_out) {
        return BCM_E_PARAM;
    }

    /* Update the physical port's SW state. If associated with a trunk,
     * update each local physical port's SW state.
     */

    if (BCM_TRUNK_INVALID != tgid_out) {

        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid_out, 
                    SOC_MAX_NUM_PORTS, local_member_array, &local_member_count));

        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &port_info);
            if (incr) {
                port_info->vp_count++;
            } else {
                port_info->vp_count--;
            }
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        if (mod_local) {
            if (soc_feature(unit, soc_feature_sysport_remap)) { 
                BCM_XLATE_SYSPORT_S2P(unit, &port_out); 
            }
            _bcm_port_info_access(unit, port_out, &port_info);
            if (incr) {
                port_info->vp_count++;
            } else {
                port_info->vp_count--;
            }
        }
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_trident_niv_reinit
 * Purpose:
 *      Warm boot recovery for the NIV software module
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_trident_niv_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int stable_size;
    uint8 *ing_nh_buf = NULL;
    ing_l3_next_hop_entry_t *ing_nh_entry;
    uint8 *egr_nh_buf = NULL;
    egr_l3_next_hop_entry_t *egr_nh_entry;
    int i, index_min, index_max;
    uint32 entry_type, vp, trunk_bit;
    bcm_trunk_t tgid;
    bcm_module_t modid, mod_out;
    bcm_port_t port_num, port_out;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Recover NIV virtual ports from Ingress and
     * Egress Next Hop tables
     */
    ing_nh_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, ING_L3_NEXT_HOPm),
            "Ing Next Hop buffer");
    if (NULL == ing_nh_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, ING_L3_NEXT_HOPm);
    index_max = soc_mem_index_max(unit, ING_L3_NEXT_HOPm);
    rv = soc_mem_read_range(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
            index_min, index_max, ing_nh_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    egr_nh_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, EGR_L3_NEXT_HOPm),
            "Egr Next Hop buffer");
    if (NULL == egr_nh_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, EGR_L3_NEXT_HOPm);
    index_max = soc_mem_index_max(unit, EGR_L3_NEXT_HOPm);
    rv = soc_mem_read_range(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
            index_min, index_max, egr_nh_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    for (i = index_min; i <= index_max; i++) {
        egr_nh_entry = soc_mem_table_idx_to_pointer
            (unit, EGR_L3_NEXT_HOPm, egr_l3_next_hop_entry_t *, 
             egr_nh_buf, i);

        /* Check entry type */
        entry_type = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                ENTRY_TYPEf);
        if (entry_type != 2) {
            /* Not NIV virtual port entry type */
            continue;
        }

        /* Check that VN-tag action is ADD */
        if (VNTAG_ADD != soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    SD_TAG__VNTAG_ACTIONSf)) {
            continue;
        }

        /* Recover VP */
        vp = soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                SD_TAG__DVPf);
        if ((stable_size == 0) || SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            rv = _bcm_vp_used_set(unit, vp, _bcmVpTypeNiv);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        } else {
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                /* VP bitmap is recovered by virtual_init */
                continue;
            }
        }

        /* Recover virtual interface ID */
        NIV_PORT_INFO(unit, vp)->virtual_interface_id =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                SD_TAG__VNTAG_DST_VIFf);

        /* Recover physical trunk or port */
        ing_nh_entry = soc_mem_table_idx_to_pointer
            (unit, ING_L3_NEXT_HOPm, ing_l3_next_hop_entry_t *, 
             ing_nh_buf, i);
        trunk_bit = soc_ING_L3_NEXT_HOPm_field32_get(unit, ing_nh_entry,
                Tf);
        if (trunk_bit) {
            tgid = soc_ING_L3_NEXT_HOPm_field32_get(unit, ing_nh_entry,
                    TGIDf);
            BCM_GPORT_TRUNK_SET(NIV_PORT_INFO(unit, vp)->port, tgid);
        } else {
            modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, ing_nh_entry,
                    MODULE_IDf);
            port_num = soc_ING_L3_NEXT_HOPm_field32_get(unit, ing_nh_entry,
                    PORT_NUMf);
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    modid, port_num, &mod_out, &port_out);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            BCM_GPORT_MODPORT_SET(NIV_PORT_INFO(unit, vp)->port,
                    mod_out, port_out);
        }

        /* Recover multicast flag */
        if (soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                    SD_TAG__VNTAG_Pf)) {
            NIV_PORT_INFO(unit, vp)->flags |= BCM_NIV_PORT_MULTICAST;
        }

        /* Recover match_vlan */
        NIV_PORT_INFO(unit, vp)->match_vlan =
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, egr_nh_entry,
                SD_TAG__SD_TAG_VIDf);

        if (stable_size == 0) {
            /* In the Port module, a port's VP count is not recovered in 
             * level 1 Warm Boot.
             */
            rv = _bcm_trident_niv_port_cnt_update(unit,
                    NIV_PORT_INFO(unit, vp)->port, vp, TRUE);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

cleanup:
    if (ing_nh_buf) {
        soc_cm_sfree(unit, ing_nh_buf);
    }
    if (egr_nh_buf) {
        soc_cm_sfree(unit, egr_nh_buf);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_trident_niv_free_resources
 * Purpose:
 *      Free all allocated tables and memory
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_trident_niv_free_resources(int unit)
{
    if (NIV_INFO(unit)->port_info) {
        sal_free(NIV_INFO(unit)->port_info);
        NIV_INFO(unit)->port_info = NULL;
    }
}

/*
 * Function:
 *      bcm_trident_niv_init
 * Purpose:
 *      Initialize the NIV module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_init(int unit)
{
    int num_vp;
    int rv = BCM_E_NONE;

    sal_memset(NIV_INFO(unit), 0,
            sizeof(_bcm_trident_niv_bookkeeping_t));

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    if (NULL == NIV_INFO(unit)->port_info) {
        NIV_INFO(unit)->port_info =
            sal_alloc(sizeof(_bcm_trident_niv_port_info_t) * num_vp, "niv_port_info");
        if (NULL == NIV_INFO(unit)->port_info) {
            _bcm_trident_niv_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(NIV_INFO(unit)->port_info, 0,
            sizeof(_bcm_trident_niv_port_info_t) * num_vp);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* Warm Boot recovery */
        rv = _bcm_trident_niv_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_trident_niv_free_resources(unit);
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_detach
 * Purpose:
 *      Detach the NIV module.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_cleanup(int unit)
{
    _bcm_trident_niv_free_resources(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_niv_nh_info_set
 * Purpose:
 *      Get a next hop index and configure next hop tables.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      niv_port   - (IN) Pointer to NIV port structure. 
 *      vp         - (IN) Virtual port number. 
 *      drop       - (IN) Drop indication. 
 *      nh_index   - (IN/OUT) Next hop index. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trident_niv_nh_info_set(int unit, bcm_niv_port_t *niv_port, int vp,
        int drop, int *nh_index)
{
    int rv;
    uint32 nh_flags;
    bcm_l3_egress_t nh_info;
    egr_l3_next_hop_entry_t egr_nh;
    uint8 egr_nh_entry_type;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int id;
    int ing_nh_port;
    int ing_nh_module;
    int ing_nh_trunk;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;

    /* Get a next hop index */

    if (niv_port->flags & BCM_NIV_PORT_REPLACE) {
        if ((*nh_index > soc_mem_index_max(unit, EGR_L3_NEXT_HOPm)) ||
                (*nh_index < soc_mem_index_min(unit, EGR_L3_NEXT_HOPm)))  {
            return BCM_E_PARAM;
        }
    } else {
        /*
         * Allocate a next-hop entry. By calling bcm_xgs3_nh_add()
         * with _BCM_L3_SHR_WRITE_DISABLE flag, a next-hop index is
         * allocated but nothing is written to hardware. The "nh_info"
         * in this case is not used, so just set to all zeros.
         */
        bcm_l3_egress_t_init(&nh_info);

        nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
        rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info, nh_index);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Write EGR_L3_NEXT_HOP entry */

    if (niv_port->flags & BCM_NIV_PORT_REPLACE) {
        /* Read the existing egress next_hop entry */
        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                *nh_index, &egr_nh);
        BCM_IF_ERROR_RETURN(rv);

        /* Be sure that the existing entry is programmed to SD-tag */
        egr_nh_entry_type = 
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, ENTRY_TYPEf);
        if (egr_nh_entry_type != 0x2) { /* != SD-tag */
            return BCM_E_PARAM;
        }
    } else {
        egr_nh_entry_type = 0x2; /* SD-tag */
    }

    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            ENTRY_TYPEf, egr_nh_entry_type);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
            SD_TAG__DVPf, vp);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__HG_HDR_SELf, 1);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__VNTAG_DST_VIFf, niv_port->virtual_interface_id);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__VNTAG_FORCE_Lf, 0);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__VNTAG_Pf,
            (niv_port->flags & BCM_NIV_PORT_MULTICAST) ? 1 : 0);
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__VNTAG_ACTIONSf, 1);
    /* The field SD_TAG_VID is not used for NIV.
     * Store match_vlan here for warm boot recovery.
     */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
            SD_TAG__SD_TAG_VIDf, niv_port->match_vlan);
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &egr_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Resolve gport */

    rv = _bcm_esw_gport_resolve(unit, niv_port->port, &mod_out, 
            &port_out, &trunk_id, &id);
    if (rv < 0) {
        goto cleanup;
    }

    ing_nh_port = -1;
    ing_nh_module = -1;
    ing_nh_trunk = -1;

    if (BCM_GPORT_IS_TRUNK(niv_port->port)) {
        ing_nh_module = -1;
        ing_nh_port = -1;
        ing_nh_trunk = trunk_id;
    } else {
        ing_nh_module = mod_out;
        ing_nh_port = port_out;
        ing_nh_trunk = -1;
    }

    /* Write ING_L3_NEXT_HOP entry */

    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));

    if (ing_nh_trunk == -1) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, PORT_NUMf, ing_nh_port);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, MODULE_IDf, ing_nh_module);
    } else {    
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, Tf, 1);
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, TGIDf, ing_nh_trunk);
    }

    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh, DROPf, drop);
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
            &ing_nh, ENTRY_TYPEf, 0x2); /* L2 DVP */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_mem_field_valid(unit, ING_L3_NEXT_HOPm, DVP_ATTRIBUTE_1_INDEXf)) {
        uint32 mtu_profile_index;

        rv = _bcm_tr3_mtu_profile_index_get(unit, 0x3fff, &mtu_profile_index);
        if (rv < 0) {
            goto cleanup;
        }
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                DVP_ATTRIBUTE_1_INDEXf, mtu_profile_index);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                MTU_SIZEf, 0x3fff);
    }

    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    /* Write INITIAL_ING_L3_NEXT_HOP entry */

    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    if (ing_nh_trunk == -1) {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, PORT_NUMf, ing_nh_port);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, MODULE_IDf, ing_nh_module);
    } else {
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, Tf, 1);
        soc_mem_field32_set(unit, INITIAL_ING_L3_NEXT_HOPm,
                &initial_ing_nh, TGIDf, ing_nh_trunk);
    }
    rv = soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
            MEM_BLOCK_ALL, *nh_index, &initial_ing_nh);
    if (rv < 0) {
        goto cleanup;
    }

    return rv;

cleanup:
    if (!(niv_port->flags & BCM_NIV_PORT_REPLACE)) {
        (void) bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, *nh_index);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_trident_niv_nh_info_delete
 * Purpose:
 *      Free next hop index and clear next hop tables.
 * Parameters:
 *      unit     - (IN) SOC unit number. 
 *      nh_index - (IN) Next hop index. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trident_niv_nh_info_delete(int unit, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
    ing_l3_next_hop_entry_t ing_nh;
    initial_ing_l3_next_hop_entry_t initial_ing_nh;

    /* Clear EGR_L3_NEXT_HOP entry */
    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &egr_nh));

    /* Clear ING_L3_NEXT_HOP entry */
    sal_memset(&ing_nh, 0, sizeof(ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN(soc_mem_write (unit, ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &ing_nh));

    /* Clear INITIAL_ING_L3_NEXT_HOP entry */
    sal_memset(&initial_ing_nh, 0, sizeof(initial_ing_l3_next_hop_entry_t));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_ING_L3_NEXT_HOPm,
                                   MEM_BLOCK_ALL, nh_index, &initial_ing_nh));

    /* Free the next-hop index. */
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_nh_del(unit, _BCM_L3_SHR_WRITE_DISABLE, nh_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_niv_match_add
 * Purpose:
 *      Add match criteria for NIV VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      niv_port - (IN) Pointer to VLAN virtual port structure. 
 *      vp - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trident_niv_match_add(int unit, bcm_niv_port_t *niv_port, int vp)
{
    vlan_xlate_entry_t    vent, old_vent;
    int                   key_type;
    bcm_module_t          mod_out;
    bcm_port_t            port_out;
    bcm_trunk_t           trunk_out;
    int                   tmp_id;
    bcm_vlan_action_set_t action;
    uint32                profile_idx;
    int                   rv;
    bcm_trunk_t           trunk_id;
    int                   idx;
    int                   mod_local;
    int                   num_local_ports;
    bcm_port_t            local_ports[SOC_MAX_NUM_PORTS];
    int                   local_member_count;
    bcm_port_t            local_member_array[SOC_MAX_NUM_PORTS];
    int                   port_key_type_vif_vlan, port_key_type_vif;
    int                   port_key_type_a, port_key_type_b;
    int                   use_port_a, use_port_b;
    int                   vt_enable;

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));

    soc_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);

    if (BCM_VLAN_VALID(niv_port->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__VLANf,
                niv_port->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, key_type);

    if (niv_port->virtual_interface_id >=
            (1 << soc_mem_field_length(unit, VLAN_XLATEm, VIF__SRC_VIFf))) {
        return BCM_E_PARAM;
    }
    soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__SRC_VIFf,
            niv_port->virtual_interface_id);

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_mem_field_valid(unit, VLAN_XLATEm, SOURCE_TYPEf)) {
        soc_VLAN_XLATEm_field32_set(unit, &vent, SOURCE_TYPEf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, niv_port->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(niv_port->port)) {
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__Tf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__TGIDf, trunk_out);
    } else {
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__MODULE_IDf, mod_out);
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__PORT_NUMf, port_out);
    }

    soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__MPLS_ACTIONf, 0x1); /* SVP */
    soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__SOURCE_VPf, vp);

    bcm_vlan_action_set_t_init(&action);
    if (BCM_VLAN_VALID(niv_port->match_vlan)) {
        action.dt_outer = bcmVlanActionCopy;
        action.dt_inner = bcmVlanActionDelete;
        action.ot_outer = bcmVlanActionReplace;
    } else {
        action.ot_outer_prio = bcmVlanActionReplace;
        action.ut_outer = bcmVlanActionAdd;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, &action, &profile_idx));
    soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__TAG_ACTION_PROFILE_PTRf,
            profile_idx);
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_mem_field_valid(unit, VLAN_XLATEm, VIF__VLAN_ACTION_VALIDf)) {
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__VLAN_ACTION_VALIDf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    rv = soc_mem_insert_return_old(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
            &vent, &old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, &old_vent,
                VIF__TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    BCM_IF_ERROR_RETURN(rv);

    num_local_ports = 0;
    if (BCM_GPORT_IS_TRUNK(niv_port->port)) {
        trunk_id = BCM_GPORT_TRUNK_GET(niv_port->port);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                SOC_MAX_NUM_PORTS, local_member_array, &local_member_count);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
        for (idx = 0; idx < local_member_count; idx++) {
            local_ports[num_local_ports++] = local_member_array[idx];
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, niv_port->port, &mod_out, &port_out,
                                    &trunk_id, &tmp_id)); 
        if ((trunk_id != -1) || (tmp_id != -1)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        if (mod_local) {
            local_ports[num_local_ports++] = port_out;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_pt_vtkey_type_value_get(unit,
                VLXLT_HASH_KEY_TYPE_VIF_VLAN, &port_key_type_vif_vlan));
    BCM_IF_ERROR_RETURN(_bcm_esw_pt_vtkey_type_value_get(unit,
                VLXLT_HASH_KEY_TYPE_VIF, &port_key_type_vif));
    for (idx = 0; idx < num_local_ports; idx++) {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_config_get(unit, local_ports[idx],
                    _bcmPortVTKeyTypeFirst, &port_key_type_a));
        BCM_IF_ERROR_RETURN(_bcm_esw_port_config_get(unit, local_ports[idx],
                    _bcmPortVTKeyPortFirst, &use_port_a));
        BCM_IF_ERROR_RETURN(_bcm_esw_port_config_get(unit, local_ports[idx],
                    _bcmPortVTKeyTypeSecond, &port_key_type_b));
        BCM_IF_ERROR_RETURN(_bcm_esw_port_config_get(unit, local_ports[idx],
                    _bcmPortVTKeyPortSecond, &use_port_b));

        if (BCM_VLAN_VALID(niv_port->match_vlan)) {
            if ((port_key_type_a != port_key_type_vif_vlan) &&
                    (port_key_type_b != port_key_type_vif_vlan)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                            local_ports[idx], _bcmPortVTKeyTypeFirst,
                            port_key_type_vif_vlan));
                BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                            local_ports[idx], _bcmPortVTKeyPortFirst, 1));
                if (port_key_type_a == port_key_type_vif) {
                    /* The first slot contained VIF key type, which
                     * was just overwritten by VIF_VLAN key type. Hence,
                     * the VIF key type needs to be moved to second slot.
                     */
                    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                                local_ports[idx], _bcmPortVTKeyTypeSecond,
                                port_key_type_a));
                    BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                                local_ports[idx], _bcmPortVTKeyPortSecond,
                                use_port_a));
                }
            }
        } else {
            if (port_key_type_b != port_key_type_vif) {
                BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                            local_ports[idx], _bcmPortVTKeyTypeSecond,
                            port_key_type_vif));
                BCM_IF_ERROR_RETURN(_bcm_esw_port_config_set(unit,
                            local_ports[idx], _bcmPortVTKeyPortSecond, 1));
            }
        }

        /* Enable ingress VLAN translation */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_get(unit, local_ports[idx],
                                      _bcmPortVlanTranslate, &vt_enable));
        if (!vt_enable) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_port_config_set(unit, local_ports[idx],
                                          _bcmPortVlanTranslate, 1));
        }

        if (BCM_VLAN_VALID(niv_port->match_vlan)) {
            /* Enable egress VLAN translation */
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                        EGR_VLAN_CONTROL_1r, local_ports[idx], VT_ENABLEf, 1));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trident_niv_match_delete
 * Purpose:
 *      Delete match criteria for NIV VP.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trident_niv_match_delete(int unit, int vp)
{
    vlan_xlate_entry_t vent, old_vent;
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    uint32             profile_idx;
    int                rv;

    sal_memset(&vent, 0, sizeof(vlan_xlate_entry_t));

    if (BCM_VLAN_VALID(NIV_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__VLANf,
                NIV_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, key_type);

    soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__SRC_VIFf,
            NIV_PORT_INFO(unit, vp)->virtual_interface_id);

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_mem_field_valid(unit, VLAN_XLATEm, SOURCE_TYPEf)) {
        soc_VLAN_XLATEm_field32_set(unit, &vent, SOURCE_TYPEf, 1);
    }
#endif /* BCM_TRIDENT2_SUPPORT */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, NIV_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(NIV_PORT_INFO(unit, vp)->port)) {
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__Tf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__TGIDf, trunk_out);
    } else {
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__MODULE_IDf, mod_out);
        soc_VLAN_XLATEm_field32_set(unit, &vent, VIF__PORT_NUMf, port_out);
    }

    rv = soc_mem_delete_return_old(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
            &vent, &old_vent);
    if ((rv == SOC_E_NONE) &&
            soc_VLAN_XLATEm_field32_get(unit, &old_vent, VALIDf)) {
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, &old_vent,
                                                  VIF__TAG_ACTION_PROFILE_PTRf);       
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _trident_niv_vxlate_traverse
 * Purpose:
 * traverse the vlan_xlate table to find out entries created
 * based on the niv port. Based on the delete_action flag to either 
 * modify or delete the found entries 
 *
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      vp - (IN) Virtual port number.
 *      niv_port - (IN) new niv port info
 *      delete_action - (IN) delete the found entry if TRUE or modify 
 *                           if FALSE
 * Returns:
 *      BCM_X_XXX
 */

STATIC int 
_trident_niv_vxlate_traverse (int unit, int vp, 
               bcm_niv_port_t *niv_port, int delete_action)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    uint32          *vt_tbl_chnk;
    uint32          *vent;
    int             valid,  rv = BCM_E_NONE;
    int             tmp_id;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    bcm_trunk_t     trunk_out;
    soc_mem_t mem;
    int key_val;
    int vt_key;
    int vent_vp;
    int profile_idx;
    int vif_hit;
    int vif;
    int vent_gport;
    vlan_xlate_entry_t old_vent;

    if ((!delete_action) && (niv_port == NULL)) {
        return BCM_E_INTERNAL;
    } 
    mem = VLAN_XLATEm;

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
            chnk_idx + chunksize - 1: mem_idx_max;

        soc_mem_lock(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, vt_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, mem);
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            vent =
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                             vt_tbl_chnk, ent_idx);
            valid = soc_mem_field32_get(unit, mem, vent, VALIDf);
            if (!valid) {
                continue;
            }
            key_val = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf);
            rv = _bcm_esw_vlan_xlate_key_type_get(unit,
                  key_val,&vt_key);

            if (BCM_FAILURE(rv)) {
                break;
            }
            switch (vt_key) {
                case VLXLT_HASH_KEY_TYPE_VIF_OTAG:
                case VLXLT_HASH_KEY_TYPE_VIF_ITAG:
                case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                case VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
                    vif_hit = TRUE;
                    break;
                default:
                    vif_hit = FALSE;
                    break;
            }
            if (vif_hit == FALSE) {
                continue;
            }
            vent_vp = soc_mem_field32_get(unit, mem, vent, VIF__SOURCE_VPf);
            vif = soc_mem_field32_get(unit, mem, vent, VIF__SRC_VIFf);
            tmp_id = soc_mem_field32_get(unit, mem, vent, VIF__Tf);
            if (tmp_id) {
                trunk_out = soc_mem_field32_get(unit, mem, vent, VIF__TGIDf);
                BCM_GPORT_TRUNK_SET(vent_gport,trunk_out);
            } else {
                mod_out = soc_mem_field32_get(unit, mem, vent, VIF__MODULE_IDf);
                port_out = soc_mem_field32_get(unit, mem, vent, VIF__PORT_NUMf);
                BCM_GPORT_MODPORT_SET(vent_gport,mod_out,port_out);
            }
               
            if ((vent_vp != vp) || 
                (vent_gport != NIV_PORT_INFO(unit, vp)->port) ||
                (vif != NIV_PORT_INFO(unit, vp)->virtual_interface_id) ) {
                continue;
            }

            /* found the match, delete it first */
            rv = soc_mem_delete_return_old(unit, mem,MEM_BLOCK_ALL,
                            vent,&old_vent);

            if (BCM_FAILURE(rv)) {
                break;
            }
            if (delete_action) {
                profile_idx = soc_mem_field32_get(unit, mem, &old_vent,
                                    VIF__TAG_ACTION_PROFILE_PTRf);
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit, 
                             profile_idx);
                if (BCM_FAILURE(rv)) {
                    break;
                } else {
                    continue;
                }
            }

            /* modification action */
            vif = niv_port->virtual_interface_id;

            /* replace with the new niv port info */
            soc_mem_field32_set(unit, mem, &old_vent, VIF__SRC_VIFf, vif);
            rv = _bcm_esw_gport_resolve(unit, niv_port->port,
                      &mod_out, &port_out, &trunk_out, &tmp_id);
            if (BCM_FAILURE(rv)) {
                break;
            }
            if (BCM_GPORT_IS_TRUNK(niv_port->port)) {
                soc_mem_field32_set(unit, mem, &old_vent, VIF__Tf, 1);
                soc_mem_field32_set(unit, mem, &old_vent, VIF__TGIDf, 
                              trunk_out);
            } else {
                soc_mem_field32_set(unit, mem, &old_vent, VIF__MODULE_IDf, 
                                            mod_out);
                soc_mem_field32_set(unit, mem, &old_vent, VIF__PORT_NUMf, 
                             port_out);
            }
            rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, &old_vent);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        soc_mem_unlock(unit, mem);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, vt_tbl_chnk);
    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_port_add
 * Purpose:
 *      Create a NIV port on VIS.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      niv_port - (IN/OUT) NIV port information (OUT : niv_port_id)
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_port_add(int unit,
                         bcm_niv_port_t *niv_port)
{
    int mode;
    int vp;
    int rv = BCM_E_NONE;
    int num_vp;
    int nh_index = 0;
    ing_dvp_table_entry_t dvp_entry;
    source_vp_entry_t svp_entry;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;

    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_mode_get(unit, &mode));
    if (!mode) {
        soc_cm_debug(DK_L3, "L3 egress mode must be set first\n");
        return BCM_E_DISABLED;
    }

    if (!(niv_port->flags & BCM_NIV_PORT_REPLACE)) { /* Create new NIV VP */

        if (niv_port->flags & BCM_NIV_PORT_WITH_ID) {
            if (!BCM_GPORT_IS_NIV_PORT(niv_port->niv_port_id)) {
                return BCM_E_PARAM;
            }
            vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port->niv_port_id);

            if (_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
                return BCM_E_EXISTS;
            } else {
                rv = _bcm_vp_used_set(unit, vp, _bcmVpTypeNiv);
                if (rv < 0) {
                    return rv;
                }
            }
        } else {
            /* allocate a new VP index */
            num_vp = soc_mem_index_count(unit, SOURCE_VPm);
            rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm,
                    _bcmVpTypeNiv, &vp);
            if (rv < 0) {
                return rv;
            }
        }

        /* Configure next hop tables */
        rv = _bcm_trident_niv_nh_info_set(unit, niv_port, vp, 0,
                                              &nh_index);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure DVP table */
        sal_memset(&dvp_entry, 0, sizeof(ing_dvp_table_entry_t));
        soc_ING_DVP_TABLEm_field32_set(unit, &dvp_entry, NEXT_HOP_INDEXf,
                                       nh_index);
        rv = WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure SVP table */
        sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
        soc_SOURCE_VPm_field32_set(unit, &svp_entry, ENTRY_TYPEf, 3);

        /* Set the CML */
        rv = _bcm_vp_default_cml_mode_get(unit, &cml_default_enable,
                &cml_default_new, &cml_default_move);
        if (rv < 0) {
            goto cleanup;
        }

        if (cml_default_enable) {
            /* Set the CML to default values */
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_NEWf, cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp_entry, CML_FLAGS_MOVEf, 0x8);
        }

        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);
        if (rv < 0) {
            goto cleanup;
        }

        /* Configure SOURCE_VP_2 table */
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_is_valid(unit, SOURCE_VP_2m)) {
            source_vp_2_entry_t svp_2_entry;

            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            soc_SOURCE_VP_2m_field32_set(unit, &svp_2_entry, PARSE_USING_SGLP_TPIDf, 1);
            rv = WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry);
            if (rv < 0) {
                goto cleanup;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        /* Configure ingress VLAN translation table for unicast VPs */
        if (!(niv_port->flags & BCM_NIV_PORT_MULTICAST)) {
            rv = _bcm_trident_niv_match_add(unit, niv_port, vp);
            if (rv < 0) {
                goto cleanup;
            }
        }

        /* Increment port's VP count */
        rv = _bcm_trident_niv_port_cnt_update(unit, niv_port->port, vp, TRUE);
        if (rv < 0) {
            goto cleanup;
        }

    } else { /* Replace properties of existing NIV VP */

        if (!(niv_port->flags & BCM_NIV_PORT_WITH_ID)) {
            return BCM_E_PARAM;
        }
        if (!BCM_GPORT_IS_NIV_PORT(niv_port->niv_port_id)) {
            return BCM_E_PARAM;
        }
        vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port->niv_port_id);
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            return BCM_E_PARAM;
        }

        /* For existing niv vp, NH entry already exists */
        BCM_IF_ERROR_RETURN
            (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

        nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                NEXT_HOP_INDEXf);

        /* Update existing next hop entries */
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_nh_info_set(unit, niv_port, vp, 0,
                                              &nh_index));

        /* Delete old ingress VLAN translation entry,
         * install new ingress VLAN translation entry
         */
        if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MULTICAST)) {
            BCM_IF_ERROR_RETURN
                (_bcm_trident_niv_match_delete(unit, vp));
        }

        if (!(niv_port->flags & BCM_NIV_PORT_MULTICAST)) {
            BCM_IF_ERROR_RETURN
                (_bcm_trident_niv_match_add(unit, niv_port, vp));
        }

        if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MULTICAST)) {
            /* traverse the vlan_xlate table to find out entries created
             * based on the niv port. Check the new niv port's multicast flag 
             * to modify or delete existing entries accordingly 
             */
            rv = _trident_niv_vxlate_traverse(unit,vp,niv_port,
                    (niv_port->flags & BCM_NIV_PORT_MULTICAST)? TRUE:FALSE);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Decrement old port's VP count, increment new port's VP count */
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_port_cnt_update(unit,
                NIV_PORT_INFO(unit, vp)->port, vp, FALSE));

        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_port_cnt_update(unit,
                NIV_PORT_INFO(unit, vp)->port, vp, TRUE));
    }

    /* Set NIV VP software state */
    NIV_PORT_INFO(unit, vp)->flags = niv_port->flags;
    NIV_PORT_INFO(unit, vp)->port = niv_port->port;
    NIV_PORT_INFO(unit, vp)->virtual_interface_id = niv_port->virtual_interface_id;
    NIV_PORT_INFO(unit, vp)->match_vlan = niv_port->match_vlan;

    BCM_GPORT_NIV_PORT_ID_SET(niv_port->niv_port_id, vp);

    return rv;

cleanup:
    if (!(niv_port->flags & BCM_NIV_PORT_REPLACE)) {
        (void) _bcm_vp_free(unit, _bcmVpTypeNiv, 1, vp);
        _bcm_trident_niv_nh_info_delete(unit, nh_index);

        sal_memset(&dvp_entry, 0, sizeof(ing_dvp_table_entry_t));
        (void)WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry);

        sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
        (void)WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_is_valid(unit, SOURCE_VP_2m)) {
            source_vp_2_entry_t svp_2_entry;

            sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
            (void)WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry);
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        if (!(niv_port->flags & BCM_NIV_PORT_MULTICAST)) {
            (void) _bcm_trident_niv_match_delete(unit, vp);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_port_delete
 * Purpose:
 *      Destroy a NIV virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) NIV GPORT ID.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_port_delete(int unit, bcm_gport_t gport)
{
    int vp;
    source_vp_entry_t svp_entry;
    int nh_index;
    ing_dvp_table_entry_t dvp_entry;

    if (!BCM_GPORT_IS_NIV_PORT(gport)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }

    /* Delete ingress VLAN translation entry */
    if (!(NIV_PORT_INFO(unit, vp)->flags & BCM_NIV_PORT_MULTICAST)) {
        BCM_IF_ERROR_RETURN
            (_bcm_trident_niv_match_delete(unit, vp));
        BCM_IF_ERROR_RETURN(_trident_niv_vxlate_traverse(unit,vp,
                            NULL, TRUE));
    }

    /* Clear SVP entry */
    sal_memset(&svp_entry, 0, sizeof(source_vp_entry_t));
    BCM_IF_ERROR_RETURN
        (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));

    /* Clear SOURCE_VP_2 entry */
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_mem_is_valid(unit, SOURCE_VP_2m)) {
        source_vp_2_entry_t svp_2_entry;

        sal_memset(&svp_2_entry, 0, sizeof(source_vp_2_entry_t));
        BCM_IF_ERROR_RETURN
            (WRITE_SOURCE_VP_2m(unit, MEM_BLOCK_ALL, vp, &svp_2_entry));
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    /* Clear DVP entry */
    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry, NEXT_HOP_INDEXf);
    sal_memset(&dvp_entry, 0, sizeof(ing_dvp_table_entry_t));
    BCM_IF_ERROR_RETURN
        (WRITE_ING_DVP_TABLEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry));

    /* Clear next hop entries and free next hop index */
    BCM_IF_ERROR_RETURN
        (_bcm_trident_niv_nh_info_delete(unit, nh_index));

    /* Decrement port's VP count */
    BCM_IF_ERROR_RETURN
        (_bcm_trident_niv_port_cnt_update(unit,
                                          NIV_PORT_INFO(unit, vp)->port,
                                          vp, FALSE));
    /* Free VP */
    BCM_IF_ERROR_RETURN
        (_bcm_vp_free(unit, _bcmVpTypeNiv, 1, vp));

    /* Clear NIV VP software state */
    sal_memset(NIV_PORT_INFO(unit, vp), 0, sizeof(_bcm_trident_niv_port_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_port_delete_all
 * Purpose:
 *      Destroy all NIV virtual ports.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_port_delete_all(int unit)
{
    int i;
    bcm_gport_t niv_port_id;

    for (i = soc_mem_index_min(unit, SOURCE_VPm);
         i <= soc_mem_index_max(unit, SOURCE_VPm);
         i++) {
        if (_bcm_vp_used_get(unit, i, _bcmVpTypeNiv)) {
            BCM_GPORT_NIV_PORT_ID_SET(niv_port_id, i);
            BCM_IF_ERROR_RETURN(bcm_trident_niv_port_delete(unit, niv_port_id));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_port_get
 * Purpose:
 *      Get NIV virtual port info.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      niv_port - (IN/OUT) Pointer to NIV virtual port structure. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_port_get(int unit, bcm_niv_port_t *niv_port)
{
    int vp;

    if (!BCM_GPORT_IS_NIV_PORT(niv_port->niv_port_id)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port->niv_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_niv_port_t_init(niv_port);
    niv_port->flags = NIV_PORT_INFO(unit, vp)->flags;
    BCM_GPORT_NIV_PORT_ID_SET(niv_port->niv_port_id, vp);
    niv_port->port = NIV_PORT_INFO(unit, vp)->port;
    niv_port->virtual_interface_id = NIV_PORT_INFO(unit, vp)->virtual_interface_id;
    niv_port->match_vlan = NIV_PORT_INFO(unit, vp)->match_vlan;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_port_traverse
 * Purpose:
 *      Traverse all NIV ports and call supplied callback routine.
 * Parameters:
 *      unit      - (IN) Device Number
 *      cb        - (IN) User-provided callback
 *      user_data - (IN/OUT) Cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_port_traverse(int unit, bcm_niv_port_traverse_cb cb, 
                                  void *user_data)
{
    int i;
    bcm_niv_port_t niv_port;

    for (i = soc_mem_index_min(unit, SOURCE_VPm);
         i <= soc_mem_index_max(unit, SOURCE_VPm);
         i++) {
        if (_bcm_vp_used_get(unit, i, _bcmVpTypeNiv)) {
            bcm_niv_port_t_init(&niv_port);
            BCM_GPORT_NIV_PORT_ID_SET(niv_port.niv_port_id, i);
            BCM_IF_ERROR_RETURN(bcm_trident_niv_port_get(unit, &niv_port));
            BCM_IF_ERROR_RETURN(cb(unit, &niv_port, user_data));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_trident_niv_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for a NIV virtual port
 * Parameters:
 *      unit     - (IN) BCM device number
 *      gport    - (IN) Global port identifier
 *      modid    - (OUT) Module ID
 *      port     - (OUT) Port number
 *      trunk_id - (OUT) Trunk ID
 *      id       - (OUT) Virtual port ID
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trident_niv_port_resolve(int unit, bcm_gport_t niv_port_id,
                          bcm_module_t *modid, bcm_port_t *port,
                          bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE, nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;

    if (!BCM_GPORT_IS_NIV_PORT(niv_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_NIV_PORT_ID_GET(niv_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN(READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                              NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x2) {
        /* Entry type is not L2 DVP */
        return BCM_E_NOT_FOUND;
    }
    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
        *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
    } else {
        *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
        *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
    }
    *id = vp;
    return rv;
}

/*
 * Purpose:
 *	Create NIV Forwarding table entry
 * Parameters:
 *	unit - (IN) Device Number
 *	iv_fwd_entry - (IN) NIV Forwarding table entry
 */
int
bcm_trident_niv_forward_add(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t tgid_out;
    int id_out;
    l2x_entry_t l2x_entry;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0xfff) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                    iv_fwd_entry->dest_port,
                    &mod_out, &port_out, &tgid_out, &id_out));
        if (-1 != id_out) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);
        if (-1 != tgid_out) {
            BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, tgid_out));
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DEST_TYPEf, 1);
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__TGIDf, tgid_out);
        } else {
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__MODULE_IDf, mod_out);
            soc_L2Xm_field32_set(unit, &l2x_entry, VIF__PORT_NUMf, port_out);
        }
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__STATIC_BITf, 1);

    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }
        if (!_BCM_MULTICAST_IS_L2(iv_fwd_entry->dest_multicast)) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__L2MC_PTRf,
                _BCM_MULTICAST_ID_GET(iv_fwd_entry->dest_multicast));
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__STATIC_BITf, 1);
    }

    soc_mem_lock(unit, L2Xm);

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_REPLACE)) {
        rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    } else { /* Replace existing entry */
        rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
        rv = soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    }

    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Purpose:
 *	Delete NIV Forwarding table entry
 * Parameters:
 *      unit - (IN) Device Number
 *      iv_fwd_entry - (IN) NIV Forwarding table entry
 */
int
bcm_trident_niv_forward_delete(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    l2x_entry_t l2x_entry;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0xfff) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);

    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);
    }

    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_delete(unit, L2Xm, MEM_BLOCK_ALL, &l2x_entry);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Purpose:
 *	Delete all NIV Forwarding table entries
 * Parameters:
 *	unit - Device Number
 */
int
bcm_trident_niv_forward_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    l2_bulk_match_mask_entry_t match_mask;
    l2_bulk_match_data_entry_t match_data;
    int field_len;

    sal_memset(&match_mask, 0, sizeof(match_mask));
    sal_memset(&match_data, 0, sizeof(match_data));

    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, VALIDf, 1);
    soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, VALIDf, 1);

    field_len = soc_mem_field_length(unit, L2_BULK_MATCH_MASKm, KEY_TYPEf);
    soc_mem_field32_set(unit, L2_BULK_MATCH_MASKm, &match_mask, KEY_TYPEf,
            (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2_BULK_MATCH_DATAm, &match_data, KEY_TYPEf,
            TR_L2_HASH_KEY_TYPE_VIF);

    soc_mem_lock(unit, L2Xm);
    rv = WRITE_L2_BULK_MATCH_MASKm(unit, MEM_BLOCK_ALL, 0,
            &match_mask);
    if (BCM_SUCCESS(rv)) {
        rv = WRITE_L2_BULK_MATCH_DATAm(unit, MEM_BLOCK_ALL, 0,
                &match_data);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_reg_field32_modify(unit, L2_BULK_CONTROLr, REG_PORT_ANY,
                ACTIONf, 1);
    }
    if (BCM_SUCCESS(rv)) {
        rv = soc_l2x_port_age(unit, L2_BULK_CONTROLr, INVALIDr);
    }
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Purpose:
 *      Get NIV Forwarding table entry
 * Parameters:
 *      unit - (IN) Device Number
 *      iv_fwd_entry - (IN/OUT) NIV forwarding table info
 */
int
bcm_trident_niv_forward_get(int unit, bcm_niv_forward_t *iv_fwd_entry)
{
    int rv = BCM_E_NONE;
    l2x_entry_t l2x_entry, l2x_entry_out;
    int idx;
    _bcm_gport_dest_t dest;
    int l2mc_index;

    if (iv_fwd_entry->name_space > 0xfff) {
        return BCM_E_PARAM;
    }

    if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
        if (iv_fwd_entry->virtual_interface_id > 0xfff) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 0);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);

    } else { /* Multicast forward entry */
        if (iv_fwd_entry->virtual_interface_id > 0x3fff) {
            return BCM_E_PARAM;
        }

        sal_memset(&l2x_entry, 0, sizeof(l2x_entry));
        soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                TR_L2_HASH_KEY_TYPE_VIF);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__NAMESPACEf,
                iv_fwd_entry->name_space);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__Pf, 1);
        soc_L2Xm_field32_set(unit, &l2x_entry, VIF__DST_VIFf,
                iv_fwd_entry->virtual_interface_id);
    }

    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ALL, &idx, &l2x_entry,
            &l2x_entry_out, 0);
    soc_mem_unlock(unit, L2Xm);

    if (SOC_SUCCESS(rv)) {
        if (!(iv_fwd_entry->flags & BCM_NIV_FORWARD_MULTICAST)) {
            if (soc_L2Xm_field32_get(unit, &l2x_entry_out, VIF__DEST_TYPEf)) {
                dest.tgid = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                    VIF__TGIDf);
                dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            } else {
                dest.modid = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                        VIF__MODULE_IDf);
                dest.port = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                        VIF__PORT_NUMf);
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest,
                        &(iv_fwd_entry->dest_port)));
        } else {
            l2mc_index = soc_L2Xm_field32_get(unit, &l2x_entry_out,
                    VIF__L2MC_PTRf);
            _BCM_MULTICAST_GROUP_SET(iv_fwd_entry->dest_multicast,
                    _BCM_MULTICAST_TYPE_L2, l2mc_index);
        }
    }

    return rv;
}

/*
 * Purpose:
 *      Traverse all valid NIV forward entries and call the
 *      supplied callback routine.
 * Parameters:
 *      unit      - Device Number
 *      cb        - User callback function, called once per NIV forward entry.
 *      user_data - cookie
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_forward_traverse(int unit,
                             bcm_niv_forward_traverse_cb cb,
                             void *user_data)
{
    int rv = BCM_E_NONE;
    int chunk_entries, chunk_bytes;
    uint8 *l2_tbl_chunk = NULL;
    int chunk_idx_min, chunk_idx_max, chunk_end;
    int ent_idx, mem_idx_min, mem_idx_max;
    l2x_entry_t *l2x_entry;
    bcm_niv_forward_t iv_fwd_entry;
    int l2mc_index;
    _bcm_gport_dest_t dest;

    chunk_entries = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);
    chunk_bytes = 4 * SOC_MEM_WORDS(unit, L2Xm) * chunk_entries;
    l2_tbl_chunk = soc_cm_salloc(unit, chunk_bytes, "niv forward traverse");
    if (NULL == l2_tbl_chunk) {
        return BCM_E_MEMORY;
    }

    mem_idx_min = soc_mem_index_min(unit, L2Xm);
    mem_idx_max = soc_mem_index_max(unit, L2Xm);
    for (chunk_idx_min = mem_idx_min; chunk_idx_min <= mem_idx_max; 
         chunk_idx_min += chunk_entries) {
        sal_memset(l2_tbl_chunk, 0, chunk_bytes);

        chunk_idx_max = chunk_idx_min + chunk_entries - 1;
        if (chunk_idx_max > mem_idx_max) {
            chunk_idx_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                                chunk_idx_min, chunk_idx_max, l2_tbl_chunk);
        if (SOC_FAILURE(rv)) {
            break;
        }

        chunk_end = (chunk_idx_max - chunk_idx_min);
        for (ent_idx = 0; ent_idx <= chunk_end; ent_idx++) {
            l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm, l2x_entry_t *, 
                                             l2_tbl_chunk, ent_idx);

            if (soc_L2Xm_field32_get(unit, l2x_entry, VALIDf) == 0) {
                continue;
            }

            if (soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf) != 
                    TR_L2_HASH_KEY_TYPE_VIF) {
                continue;
            }

            bcm_niv_forward_t_init(&iv_fwd_entry);

            iv_fwd_entry.name_space = soc_L2Xm_field32_get(unit, l2x_entry,
                    VIF__NAMESPACEf);
            iv_fwd_entry.virtual_interface_id = soc_L2Xm_field32_get(unit,
                    l2x_entry, VIF__DST_VIFf);

            if (soc_L2Xm_field32_get(unit, l2x_entry, VIF__Pf)) {
                /* Multicast NIV forward entry */
                iv_fwd_entry.flags |= BCM_NIV_FORWARD_MULTICAST;
                l2mc_index = soc_L2Xm_field32_get(unit, l2x_entry,
                        VIF__L2MC_PTRf);
                _BCM_MULTICAST_GROUP_SET(iv_fwd_entry.dest_multicast,
                        _BCM_MULTICAST_TYPE_L2, l2mc_index);
            } else {
                if (soc_L2Xm_field32_get(unit, l2x_entry, VIF__DEST_TYPEf)) {
                    dest.tgid = soc_L2Xm_field32_get(unit, l2x_entry,
                            VIF__TGIDf);
                    dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                } else {
                    dest.modid = soc_L2Xm_field32_get(unit, l2x_entry,
                            VIF__MODULE_IDf);
                    dest.port = soc_L2Xm_field32_get(unit, l2x_entry,
                            VIF__PORT_NUMf);
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                }
                rv = _bcm_esw_gport_construct(unit, &dest,
                        &(iv_fwd_entry.dest_port));
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            rv = cb(unit, &iv_fwd_entry, user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_tbl_chunk);

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_ethertype_set
 * Purpose:
 *      Set NIV Ethertype.
 * Parameters:
 *      unit      - (IN) BCM device number
 *      ethertype - (IN) NIV Ethertype
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_ethertype_set(int unit, int ethertype)
{

    if (ethertype < 0 || ethertype > 0xffff) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, NIV_ETHERTYPEr,
                                REG_PORT_ANY, ETHERTYPEf, ethertype));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, NIV_ETHERTYPEr,
                                REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPEr,
                                REG_PORT_ANY, ETHERTYPEf, ethertype));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPEr,
                                REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPE_2r,
                                REG_PORT_ANY, ETHERTYPEf, ethertype));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_NIV_ETHERTYPE_2r,
                                REG_PORT_ANY, ENABLEf, ethertype ? 1 : 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_ethertype_get
 * Purpose:
 *      Get NIV Ethertype.
 * Parameters:
 *      unit      - (IN) BCM device number
 *      ethertype - (OUT) NIV Ethertype
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_ethertype_get(int unit, int *ethertype)
{
    uint32 rval;

    if (ethertype == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_NIV_ETHERTYPEr(unit, &rval));
    if (soc_reg_field_get(unit, NIV_ETHERTYPEr, rval, ENABLEf)) {
        *ethertype = soc_reg_field_get(unit, NIV_ETHERTYPEr, rval, ETHERTYPEf);
    } else {
        *ethertype = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_trident_niv_untagged_add
 * Purpose:
 *      Set NIV VP tagging/untagging status by adding
 *      a (NIV VP, VLAN) egress VLAN translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN ID. 
 *      vp   - (IN) Virtual port number.
 *      flags - (IN) Untagging indication.
 *      egr_vt_added - (OUT) Indicates if (VP, VLAN) added to egress VLAN
 *                           translation table.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_untagged_add(int unit, bcm_vlan_t vlan, int vp, int flags,
        int *egr_vt_added)
{
    egr_vlan_xlate_entry_t vent, old_vent;
    bcm_vlan_action_set_t action;
    uint32 profile_idx;
    int rv = BCM_E_NONE;

    *egr_vt_added = FALSE;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, VALIDf, 1);
    if (soc_mem_field_valid(unit, EGR_VLAN_XLATEm, ENTRY_TYPEf)) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, EGR_VLAN_XLATEm, KEY_TYPEf)) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, 1);
    }
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, DVPf, vp);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, vlan);

    if (!BCM_VLAN_VALID(NIV_PORT_INFO(unit, vp)->match_vlan)) {
        if (!(flags & BCM_VLAN_PORT_UNTAGGED)) {
            /* No need to insert an egress vlan translation entry
             * to remove the outer tag.
             */
            return BCM_E_NONE;
        }

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_mem_field_valid(unit, EGR_VP_VLAN_MEMBERSHIPm, UNTAGf)) {
            if (flags & BCM_VLAN_GPORT_ADD_VP_VLAN_MEMBERSHIP) {
                /* The UNTAG field of EGR_VP_VLAN_MEMBERSHIP table will
                 * be configured with untagging status. It would be redundant
                 * to insert an egress VLAN translation entry.
                 */
                return BCM_E_NONE;
            }
        } 
#endif /* BCM_TRIDENT2_SUPPORT */

        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionDelete;
        action.ot_outer = bcmVlanActionDelete;
    } else {
        /* NIV port's match_vlan is valid. It needs to be inserted
         * into the packet using an egress vlan translation entry.
         */
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, NEW_OVIDf,
                NIV_PORT_INFO(unit, vp)->match_vlan);
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionReplace;
        action.ot_outer = bcmVlanActionReplace;
        if (flags & BCM_VLAN_PORT_UNTAGGED) {
            action.dt_inner = bcmVlanActionNone;
            action.ot_inner = bcmVlanActionNone;
        } else {
            action.dt_inner = bcmVlanActionCopy;
            action.ot_inner = bcmVlanActionCopy;
        }
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_egr_vlan_action_profile_entry_add(unit, &action, &profile_idx));
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, TAG_ACTION_PROFILE_PTRf,
            profile_idx);

    rv = soc_mem_insert_return_old(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL,
                                   &vent, &old_vent);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &old_vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    *egr_vt_added = TRUE;

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_untagged_delete
 * Purpose:
 *      Delete (NIV VP, VLAN) egress VLAN translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN ID. 
 *      vp   - (IN) Virtual port number.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_trident_niv_untagged_delete(int unit, bcm_vlan_t vlan, int vp)
{
    egr_vlan_xlate_entry_t vent, old_vent;
    uint32 profile_idx;
    int rv;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));
    if (soc_mem_field_valid(unit, EGR_VLAN_XLATEm, ENTRY_TYPEf)) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, EGR_VLAN_XLATEm, KEY_TYPEf)) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, 1);
    }
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, DVPf, vp);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, vlan);

    rv = soc_mem_delete_return_old(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL,
                                   &vent, &old_vent);
    if ((rv == SOC_E_NONE) &&
            soc_EGR_VLAN_XLATEm_field32_get(unit, &old_vent, VALIDf)) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &old_vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_untagged_get
 * Purpose:
 *      Get tagging/untagging status of a NIV virtual port by
 *      reading the (NIV VP, VLAN) egress vlan translation entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vlan - (IN) VLAN to remove virtual port from.
 *      vp   - (IN) Virtual port number.
 *      flags - (OUT) Untagging status of the NIV virtual port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_trident_niv_untagged_get(int unit, bcm_vlan_t vlan, int vp,
                                  int *flags)
{
    egr_vlan_xlate_entry_t vent, res_vent;
    int idx;
    uint32 profile_idx;
    int rv;
    bcm_vlan_action_set_t action;

    *flags = 0;

    sal_memset(&vent, 0, sizeof(egr_vlan_xlate_entry_t));
    if (soc_mem_field_valid(unit, EGR_VLAN_XLATEm, ENTRY_TYPEf)) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, ENTRY_TYPEf, 1);
    } else if (soc_mem_field_valid(unit, EGR_VLAN_XLATEm, KEY_TYPEf)) {
        soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, KEY_TYPEf, 1);
    }
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, DVPf, vp);
    soc_EGR_VLAN_XLATEm_field32_set(unit, &vent, OVIDf, vlan);

    rv = soc_mem_search(unit, EGR_VLAN_XLATEm, MEM_BLOCK_ALL, &idx,
                                   &vent, &res_vent, 0);
    if ((rv == SOC_E_NONE) &&
            soc_EGR_VLAN_XLATEm_field32_get(unit, &res_vent, VALIDf)) {
        profile_idx = soc_EGR_VLAN_XLATEm_field32_get(unit, &res_vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        _bcm_trx_egr_vlan_action_profile_entry_get(unit, &action, profile_idx);

        if (!BCM_VLAN_VALID(NIV_PORT_INFO(unit, vp)->match_vlan)) {
            if (bcmVlanActionDelete == action.ot_outer) {
                *flags = BCM_VLAN_PORT_UNTAGGED;
            }
        } else {
            if (bcmVlanActionNone == action.ot_inner) {
                *flags = BCM_VLAN_PORT_UNTAGGED;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_port_untagged_vlan_set
 * Purpose:
 *      Set the default VLAN ID for a NIV port.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - NIV gport.
 *      vid -  VLAN ID used for packets that ingress without a VLAN tag.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_trident_niv_port_untagged_vlan_set(int unit, bcm_port_t port,
        bcm_vlan_t vid)
{
    int                rv = BCM_E_NONE;
    int                vp;
    vlan_xlate_entry_t key_data, entry_data;
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    int                entry_index;

    /* Get VP */
    if (BCM_GPORT_IS_NIV_PORT(port)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(port);
    } else {
        return BCM_E_PORT;
    }

    /* Construct lookup key */
    sal_memset(&key_data, 0, sizeof(vlan_xlate_entry_t));

    if (BCM_VLAN_VALID(NIV_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__VLANf,
                NIV_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_VLAN_XLATEm_field32_set(unit, &key_data, KEY_TYPEf, key_type);

    soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__SRC_VIFf,
            NIV_PORT_INFO(unit, vp)->virtual_interface_id);

    if (soc_mem_field_valid(unit, VLAN_XLATEm, SOURCE_TYPEf)) {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, NIV_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(NIV_PORT_INFO(unit, vp)->port)) {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__Tf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__MODULE_IDf, mod_out);
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
                &entry_index, &key_data, &entry_data, 0));

    /* Replace entry's new VLAN */
    soc_VLAN_XLATEm_field32_set(unit, &entry_data, VIF__NEW_OVIDf, vid);

    /* Insert new entry */
    rv = soc_mem_insert(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &entry_data);
    if (rv == SOC_E_EXISTS) {
        rv = BCM_E_NONE;
    } else {
        return BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_trident_niv_port_untagged_vlan_get
 * Purpose:
 *      Get the default VLAN ID for a NIV port.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - NIV gport.
 *      vid  - (OUT) VLAN ID used for packets that ingress without a VLAN tag.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_trident_niv_port_untagged_vlan_get(int unit, bcm_port_t port,
        bcm_vlan_t *vid)
{
    int                rv = BCM_E_NONE;
    int                vp;
    vlan_xlate_entry_t key_data, entry_data;
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    int                entry_index;

    /* Get VP */
    if (BCM_GPORT_IS_NIV_PORT(port)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(port);
    } else {
        return BCM_E_PORT;
    }

    /* Construct lookup key */
    sal_memset(&key_data, 0, sizeof(vlan_xlate_entry_t));

    if (BCM_VLAN_VALID(NIV_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__VLANf,
                NIV_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_VLAN_XLATEm_field32_set(unit, &key_data, KEY_TYPEf, key_type);

    soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__SRC_VIFf,
            NIV_PORT_INFO(unit, vp)->virtual_interface_id);

    if (soc_mem_field_valid(unit, VLAN_XLATEm, SOURCE_TYPEf)) {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, NIV_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(NIV_PORT_INFO(unit, vp)->port)) {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__Tf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__MODULE_IDf, mod_out);
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
                &entry_index, &key_data, &entry_data, 0));

    /* Get entry's VLAN */
    *vid = soc_VLAN_XLATEm_field32_get(unit, &entry_data, VIF__NEW_OVIDf);

    return rv;
}

#ifdef BCM_TRIDENT2_SUPPORT

/*
 * Function:
 *      _bcm_td2_niv_match_vp_replace
 * Purpose:
 *      Replace VP value in NIV VP's match entry.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      vp - (IN) NIV VP whose match entry is being replaced.
 *      new_vp - (IN) New VP value.
 *      old_vp - (OUT) Old VP value.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2_niv_match_vp_replace(int unit, int vp, int new_vp, int *old_vp)
{
    int                rv = BCM_E_NONE;
    vlan_xlate_entry_t key_data, entry_data;
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    int                entry_index;

    /* Construct lookup key */
    sal_memset(&key_data, 0, sizeof(vlan_xlate_entry_t));

    if (BCM_VLAN_VALID(NIV_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__VLANf,
                NIV_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_VLAN_XLATEm_field32_set(unit, &key_data, KEY_TYPEf, key_type);

    soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__SRC_VIFf,
            NIV_PORT_INFO(unit, vp)->virtual_interface_id);

    if (soc_mem_field_valid(unit, VLAN_XLATEm, SOURCE_TYPEf)) {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, NIV_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(NIV_PORT_INFO(unit, vp)->port)) {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__Tf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__MODULE_IDf, mod_out);
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
                &entry_index, &key_data, &entry_data, 0));

    /* Replace entry's VP value */
    *old_vp = soc_VLAN_XLATEm_field32_get(unit, &entry_data, VIF__SOURCE_VPf);
    soc_VLAN_XLATEm_field32_set(unit, &entry_data, VIF__SOURCE_VPf, new_vp);

    /* Insert new entry */
    rv = soc_mem_insert(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &entry_data);
    if (rv == SOC_E_EXISTS) {
        rv = BCM_E_NONE;
    } else {
        return BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_niv_port_source_vp_lag_set
 * Purpose:
 *      Set source VP LAG for a NIV virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) NIV virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_niv_port_source_vp_lag_set(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_NIV_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_PARAM;
    }

    /* Set source VP LAG by replacing the SVP field in NIV VP's
     * match entry with the VP value representing the VP LAG.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_niv_match_vp_replace(unit, vp, vp_lag_vp, &old_vp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_niv_port_source_vp_lag_clear
 * Purpose:
 *      Clear source VP LAG for a NIV virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) NIV virtual port GPORT ID. 
 *      vp_lag_vp - (IN) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_niv_port_source_vp_lag_clear(int unit, bcm_gport_t gport,
        int vp_lag_vp)
{
    int vp, old_vp;

    if (!BCM_GPORT_IS_NIV_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_PARAM;
    }

    /* Clear source VP LAG by replacing the SVP field in NIV VP's
     * match entry with the VP value.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_niv_match_vp_replace(unit, vp, vp, &old_vp));

    /* Check that the old VP value matches the VP value representing
     * the VP LAG.
     */
    if (old_vp != vp_lag_vp) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_niv_port_source_vp_lag_get
 * Purpose:
 *      Set source VP LAG for a NIV virtual port.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      gport - (IN) NIV virtual port GPORT ID. 
 *      vp_lag_vp - (OUT) VP representing the VP LAG. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_td2_niv_port_source_vp_lag_get(int unit, bcm_gport_t gport,
        int *vp_lag_vp)
{
    int                vp;
    vlan_xlate_entry_t key_data, entry_data;
    int                key_type;
    bcm_module_t       mod_out;
    bcm_port_t         port_out;
    bcm_trunk_t        trunk_out;
    int                tmp_id;
    int                entry_index;

    if (!BCM_GPORT_IS_NIV_PORT(gport)) {
        return BCM_E_PARAM;
    }
    vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
        return BCM_E_PARAM;
    }

    /* Construct match entry lookup key */
    sal_memset(&key_data, 0, sizeof(vlan_xlate_entry_t));

    if (BCM_VLAN_VALID(NIV_PORT_INFO(unit, vp)->match_vlan)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF_VLAN, &key_type));
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__VLANf,
                NIV_PORT_INFO(unit, vp)->match_vlan);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                    VLXLT_HASH_KEY_TYPE_VIF, &key_type));
    }
    soc_VLAN_XLATEm_field32_set(unit, &key_data, KEY_TYPEf, key_type);

    soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__SRC_VIFf,
            NIV_PORT_INFO(unit, vp)->virtual_interface_id);

    if (soc_mem_field_valid(unit, VLAN_XLATEm, SOURCE_TYPEf)) {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, SOURCE_TYPEf, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, NIV_PORT_INFO(unit, vp)->port,
                                &mod_out, &port_out, &trunk_out, &tmp_id));
    if (BCM_GPORT_IS_TRUNK(NIV_PORT_INFO(unit, vp)->port)) {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__Tf, 1);
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__TGIDf, trunk_out);
    } else {
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__MODULE_IDf, mod_out);
        soc_VLAN_XLATEm_field32_set(unit, &key_data, VIF__PORT_NUMf, port_out);
    }

    /* Lookup existing entry */
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
                &entry_index, &key_data, &entry_data, 0));

    /* Get VP value representing VP LAG */
    *vp_lag_vp = soc_VLAN_XLATEm_field32_get(unit, &entry_data, VIF__SOURCE_VPf);
    if (!_bcm_vp_used_get(unit, *vp_lag_vp, _bcmVpTypeVpLag)) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_niv_sw_dump
 * Purpose:
 *     Displays NIV information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_trident_niv_sw_dump(int unit)
{
    int i, num_vp;

    soc_cm_print("\nSW Information NIV - Unit %d\n", unit);

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    soc_cm_print("\n  Port Info    : \n");
    for (i = 0; i < num_vp; i++) {
        if (NIV_PORT_INFO(unit, i)->port == 0) {
            continue;
        }
        soc_cm_print("\n  NIV port vp = %d\n", i);
        soc_cm_print("Flags = 0x%x\n", NIV_PORT_INFO(unit, i)->flags);
        soc_cm_print("Port = 0x%x\n", NIV_PORT_INFO(unit, i)->port);
        soc_cm_print("Virtual Interface ID = 0x%x\n",
                NIV_PORT_INFO(unit, i)->virtual_interface_id);
        soc_cm_print("Match VLAN = 0x%x\n", NIV_PORT_INFO(unit, i)->match_vlan);
    }
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */
