/*
 * $Id: triumph_ipmc.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:        ipmc.c
 * Purpose:     Tracks and manages IPMC tables.
 */

#ifdef INCLUDE_L3

#include <soc/l3x.h>

#include <bcm/error.h>
#include <bcm/ipmc.h>

#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/multicast.h>

#include <bcm_int/esw_dispatch.h>

typedef struct _bcm_tr_ipmc_s
{
    int ipmc_initialized;
    int ipmc_size;
    SHR_BITDCL *ipmc_allocated;
    int *ipmc_l3_index;
    int *ipmc_ip6;
} _bcm_tr_ipmc_t;

#define TR_IPMC_USED_SET(unit, n)                               \
        SHR_BITSET(tr_ipmc_info[unit].ipmc_allocated, n);
#define TR_IPMC_USED_CLR(unit, n)                               \
        SHR_BITCLR(tr_ipmc_info[unit].ipmc_allocated, n);
#define TR_IPMC_USED_ISSET(unit, n)                             \
        SHR_BITGET(tr_ipmc_info[unit].ipmc_allocated, n)
#define TR_IPMC_ID(unit, id)                                    \
        if ((id < 0) || (id >= tr_ipmc_info[unit].ipmc_size))   \
            { return BCM_E_PARAM; }

#define TR_IPMC_INIT(unit)                                      \
        if (!tr_ipmc_info[unit].ipmc_initialized)               \
            { return BCM_E_INIT; }
#define TR_IPMC_INFO(unit)   (&tr_ipmc_info[unit])
#define TR_IPMC_GROUP_NUM(unit)   (tr_ipmc_info[unit].ipmc_size)

#define TR_IPMC_LOCK(unit) soc_mem_lock(unit, L3_IPMCm)
#define TR_IPMC_UNLOCK(unit) soc_mem_unlock(unit, L3_IPMCm)

static _bcm_tr_ipmc_t tr_ipmc_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/*
 * Function:
 *      _bcm_tr_ipmc_create
 * Purpose:
 *      Create an ipmc entry.
 */

int
_bcm_tr_ipmc_create(int unit, int *ipmc_id)
{
    int i;

    TR_IPMC_LOCK(unit);
    for (i = 0; i < TR_IPMC_GROUP_NUM(unit); i++) {
        if (!TR_IPMC_USED_ISSET(unit, i)) {
            TR_IPMC_USED_SET(unit, i);
            *ipmc_id = i;
            TR_IPMC_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }
    TR_IPMC_UNLOCK(unit);

    return (BCM_E_FULL);
}

/*
 * Function:
 *      _bcm_tr_ipmc_id_alloc
 * Purpose:
 *      allocate the given ipmc entry.
 */

int
_bcm_tr_ipmc_id_alloc(int unit, int ipmc_id)
{
    TR_IPMC_LOCK(unit);
    if (!TR_IPMC_USED_ISSET(unit, ipmc_id)) {
        TR_IPMC_USED_SET(unit, ipmc_id);
        TR_IPMC_UNLOCK(unit);
        return BCM_E_NONE;
    }
    TR_IPMC_UNLOCK(unit);

    return BCM_E_FULL;
}

/*
 * Function:
 *      _bcm_tr_ipmc_id_free
 * Purpose:
 *      Delete an ipmc entry.
 */

int
_bcm_tr_ipmc_id_free(int unit, int ipmc_id)
{
    TR_IPMC_INIT(unit);
    TR_IPMC_ID(unit, ipmc_id);

    TR_IPMC_LOCK(unit);
    if (TR_IPMC_USED_ISSET(unit, ipmc_id)) {
        TR_IPMC_USED_CLR(unit, ipmc_id);
    }
    TR_IPMC_UNLOCK(unit);

    return BCM_E_NONE;
}

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *      _tr2_ipmc_glp_get
 * Purpose:
 *      Fill source information to bcm_ipmc_addr_t struct.
 */
STATIC int
_tr2_ipmc_glp_get(int unit, bcm_ipmc_addr_t *ipmc, ipmc_1_entry_t *entry)
                    
{
    int                 mod, port_tgid, is_trunk, rv = BCM_E_NONE;
    int                 no_src_check = FALSE;

    is_trunk = soc_L3_IPMC_1m_field32_get(unit, entry, Tf);
    mod = soc_L3_IPMC_1m_field32_get(unit, entry, MODULE_IDf);
    port_tgid = soc_L3_IPMC_1m_field32_get(unit, entry, PORT_NUMf);
    if (is_trunk) {
        if ((port_tgid == 0x3f) && (mod == SOC_MODID_MAX(unit))) {
            no_src_check = TRUE;
        } else {
            mod = 0;
            port_tgid = soc_L3_IPMC_1m_field32_get(unit, entry, TGIDf);
        }
    }
    if (no_src_check) {
        ipmc->ts = 0;
        ipmc->mod_id = -1;
        ipmc->port_tgid = -1;
        ipmc->flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    } else if (is_trunk) {
        ipmc->ts = 1;
        ipmc->mod_id = 0;
        ipmc->port_tgid = port_tgid;
    } else {
        bcm_module_t    mod_in, mod_out;
        bcm_port_t      port_in, port_out;

        mod_in = mod;
        port_in = port_tgid;
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in,
                                    &mod_out, &port_out));
        ipmc->ts = 0;
        ipmc->mod_id = mod_out;
        ipmc->port_tgid = port_out;
    }
    return rv;
}
#endif

/*
 * Function:
 *      _tr_ipmc_info_get
 * Purpose:
 *      Fill information to bcm_ipmc_addr_t struct.
 */

STATIC int
_tr_ipmc_info_get(int unit, int hw_index, int index,
                    bcm_ipmc_addr_t *ipmc, ipmc_entry_t *entry)
{
    int                 mod = -1, port_tgid = -1, is_trunk = 0, rv=BCM_E_NONE;
    uint32              cfg2;
    int                 untag, port, no_src_check = FALSE;
    _bcm_l3_cfg_t       l3cfg;

    SOC_PBMP_CLEAR(ipmc->l3_pbmp);
    SOC_PBMP_CLEAR(ipmc->l2_pbmp);

    SOC_PBMP_WORD_SET(ipmc->l3_pbmp, 0,
                      soc_L3_IPMCm_field32_get(unit, entry,
                                               L3_BITMAP_LOf));
    SOC_PBMP_WORD_SET(ipmc->l2_pbmp, 0,
                      soc_L3_IPMCm_field32_get(unit, entry,
                                               L2_BITMAP_LOf));

    if(!SOC_IS_ENDURO(unit)) {
        SOC_PBMP_WORD_SET(ipmc->l3_pbmp, 1,
                          soc_L3_IPMCm_field32_get(unit, entry,
                                                   L3_BITMAP_HIf));
        SOC_PBMP_WORD_SET(ipmc->l2_pbmp, 1,
                          soc_L3_IPMCm_field32_get(unit, entry,
                                                   L2_BITMAP_HIf));
    }

    ipmc->v = soc_L3_IPMCm_field32_get(unit, entry, VALIDf);


    if (soc_mem_field_valid(unit, L3_IPMCm, PORT_NUMf)) {
        is_trunk = soc_L3_IPMCm_field32_get(unit, entry, Tf);
        mod = soc_L3_IPMCm_field32_get(unit, entry, MODULE_IDf);
        port_tgid = soc_L3_IPMCm_field32_get(unit, entry, PORT_NUMf);
        if (is_trunk) {
            if ((port_tgid == 0x3f) && (mod == SOC_MODID_MAX(unit))) {
                no_src_check = TRUE;
            } else {
                mod = 0;
                port_tgid = soc_L3_IPMCm_field32_get(unit, entry, TGIDf);
            }
        }
        if (no_src_check) {
            ipmc->ts = 0;
            ipmc->mod_id = -1;
            ipmc->port_tgid = -1;
            ipmc->flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
        } else if (is_trunk) {
            ipmc->ts = 1;
            ipmc->mod_id = 0;
            ipmc->port_tgid = port_tgid;
        } else {
            bcm_module_t    mod_in, mod_out;
            bcm_port_t      port_in, port_out;

            mod_in = mod;
            port_in = port_tgid;
            BCM_IF_ERROR_RETURN
                (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        mod_in, port_in,
                                        &mod_out, &port_out));
            ipmc->ts = 0;
            ipmc->mod_id = mod_out;
            ipmc->port_tgid = port_out;
        }
    }

    if (ipmc->v) {
        sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
        l3cfg.l3c_flags = BCM_L3_IPMC;
        if (index >= 0) {
            if (tr_ipmc_info[unit].ipmc_ip6[index]) {
                l3cfg.l3c_flags |= BCM_L3_IP6;
                ipmc->flags |= BCM_IPMC_IP6;
            } else {
                ipmc->flags &= ~BCM_IPMC_IP6;
            }
        } else {
            if (ipmc->flags & BCM_IPMC_IP6) {
                l3cfg.l3c_flags |= BCM_L3_IP6;
            }
        }
        if (ipmc->flags & BCM_IPMC_HIT_CLEAR) {
            l3cfg.l3c_flags |= BCM_L3_HIT_CLEAR;
        }
        rv = bcm_xgs3_l3_get_by_index(unit, hw_index, &l3cfg);
        if (rv < 0) {
            return (rv);
        }

        if (((index >= 0) && tr_ipmc_info[unit].ipmc_ip6[index]) ||
                       ((index < 0) && (ipmc->flags & BCM_IPMC_IP6))) {
            sal_memcpy(ipmc->s_ip6_addr, &l3cfg.l3c_sip6, BCM_IP6_ADDRLEN);
            sal_memcpy(ipmc->mc_ip6_addr, &l3cfg.l3c_ip6, BCM_IP6_ADDRLEN);
        } else {
            ipmc->s_ip_addr = l3cfg.l3c_src_ip_addr;
            ipmc->mc_ip_addr = l3cfg.l3c_ipmc_group;
        }

        if (l3cfg.l3c_flags & BCM_L3_HIT) {
            ipmc->flags |= BCM_IPMC_HIT;
        }

        if (l3cfg.l3c_flags & BCM_L3_RPE) {
            ipmc->cos = l3cfg.l3c_prio;
            ipmc->flags |= BCM_IPMC_SETPRI;
        } else {
            ipmc->cos = -1;
            ipmc->flags &= ~BCM_IPMC_SETPRI;
        }
        ipmc->ipmc_index = l3cfg.l3c_ipmc_ptr;
        ipmc->lookup_class = l3cfg.l3c_lookup_class;
        ipmc->vrf = l3cfg.l3c_vrf;
        ipmc->vid = l3cfg.l3c_vid;
    }

    /* Calculate untagged bitmap */
    BCM_PBMP_CLEAR(ipmc->l2_ubmp);
    SOC_PBMP_ITER(ipmc->l2_pbmp, port) {
        if (!IS_E_PORT(unit, port)) { /* CMIC, HG never untagged */
            continue;
        }

        cfg2 = 0;
        SOC_IF_ERROR_RETURN(READ_EGR_IPMC_CFG2r(unit, port, &cfg2));
        untag = soc_reg_field_get(unit, EGR_IPMC_CFG2r, cfg2, UNTAGf);

        if (untag) {
            SOC_PBMP_PORT_ADD(ipmc->l2_ubmp, port);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _tr_ipmc_write
 * Purpose:
 *      Write an ipmc entry from bcm_ipmc_addr_t struct.
 */

STATIC int
_tr_ipmc_write(int unit, int ipmc_id, bcm_ipmc_addr_t *ipmc)
{
    int                 rv;
    ipmc_entry_t        entry;
    int                 mod, port_tgid, is_trunk, no_src_check = FALSE;
#if defined(BCM_TRIUMPH2_SUPPORT)
    ipmc_1_entry_t      entry_1;
    sal_memset(&entry_1, 0, sizeof(ipmc_1_entry_t));
#endif
    sal_memset(&entry, 0, sizeof(ipmc_entry_t));

    soc_L3_IPMCm_field32_set(unit, &entry, VALIDf, ipmc->v);
    soc_L3_IPMCm_field32_set(unit, &entry, L3_BITMAP_LOf,
                             SOC_PBMP_WORD_GET(ipmc->l3_pbmp, 0));
    soc_L3_IPMCm_field32_set(unit, &entry, L2_BITMAP_LOf,
                             SOC_PBMP_WORD_GET(ipmc->l2_pbmp, 0));

    if(!SOC_IS_ENDURO(unit)) {
        soc_L3_IPMCm_field32_set(unit, &entry, L3_BITMAP_HIf,
                                 SOC_PBMP_WORD_GET(ipmc->l3_pbmp, 1));
        soc_L3_IPMCm_field32_set(unit, &entry, L2_BITMAP_HIf,
                                 SOC_PBMP_WORD_GET(ipmc->l2_pbmp, 1));
    }

    if ((ipmc->flags & BCM_IPMC_SOURCE_PORT_NOCHECK) ||
        (ipmc->port_tgid < 0)) {                        /* no source port */
        no_src_check = TRUE;
        is_trunk = 0;
        mod = SOC_MODID_MAX(unit);
        port_tgid = 0x3f;
    } else if (ipmc->ts) {                              /* trunk source port */
        is_trunk = 1;
        mod = 0;
        port_tgid = ipmc->port_tgid;
    } else {                                            /* source port */
        bcm_module_t    mod_in, mod_out;
        bcm_port_t      port_in, port_out;

        mod_in = ipmc->mod_id;
        port_in = ipmc->port_tgid;
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    mod_in, port_in,
                                    &mod_out, &port_out));
        /* Check parameters, since above is an application callback */
        if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
            return BCM_E_PORT;
        }
        is_trunk = 0;
        mod = mod_out;
        port_tgid = port_out;
    }

    if (is_trunk) {
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            soc_L3_IPMC_1m_field32_set(unit, &entry_1, Tf, 1);
            soc_L3_IPMC_1m_field32_set(unit, &entry_1, TGIDf, port_tgid);
        } else
#endif
        {
            soc_L3_IPMCm_field32_set(unit, &entry, Tf, 1);
            soc_L3_IPMCm_field32_set(unit, &entry, TGIDf, port_tgid);
        }
    } else {
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            soc_L3_IPMC_1m_field32_set(unit, &entry_1, MODULE_IDf, mod);
            soc_L3_IPMC_1m_field32_set(unit, &entry_1, PORT_NUMf, port_tgid);
            if (no_src_check) {
                soc_L3_IPMC_1m_field32_set(unit, &entry_1, Tf, 1);
            }
        } else
#endif
        {
            soc_L3_IPMCm_field32_set(unit, &entry, MODULE_IDf, mod);
            soc_L3_IPMCm_field32_set(unit, &entry, PORT_NUMf, port_tgid);
            if (no_src_check) {
                soc_L3_IPMCm_field32_set(unit, &entry, Tf, 1);
            }
        }
    }
    rv = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, ipmc_id, &entry);
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        rv = soc_mem_write(unit, L3_IPMC_1m, MEM_BLOCK_ALL, ipmc_id, &entry_1);
    }
#endif
    return (rv);
}

/*
 * Function:
 *      _tr_ipmc_enable
 * Purpose:
 *      Enable/disable IPMC support.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      enable - TRUE: enable IPMC support.
 *               FALSE: disable IPMC support.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_tr_ipmc_enable(int unit, int enable)
{
    int                 port, do_vlan;

    enable = enable ? 1 : 0;
    do_vlan = soc_property_get(unit, spn_IPMC_DO_VLAN, 1);

    PBMP_PORT_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port,
                                      _bcmPortIpmcV4Enable, enable));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port,
                                      _bcmPortIpmcV6Enable, enable));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port,
                                      _bcmPortIpmcVlanKey,
                                      (enable && do_vlan) ? 1 : 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_ipmc_init
 * Purpose:
 *      Initialize the IPMC module and enable IPMC support.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This function has to be called before any other IPMC functions.
 */

int
bcm_tr_ipmc_init(int unit)
{
    egr_ipmc_entry_t egr_entry;
#if defined(BCM_TRIUMPH2_SUPPORT)
    ipmc_remap_entry_t remap_entry;
#endif
    _bcm_tr_ipmc_t  *info = TR_IPMC_INFO(unit);
    int i, rv = BCM_E_NONE;

    bcm_tr_ipmc_delete_all(unit);
    BCM_IF_ERROR_RETURN(bcm_tr_ipmc_detach(unit));
    BCM_IF_ERROR_RETURN(_tr_ipmc_enable(unit, TRUE));

    TR_IPMC_GROUP_NUM(unit) = soc_mem_index_count(unit, L3_IPMCm);
    info->ipmc_allocated =
        sal_alloc(SHR_BITALLOCSIZE(TR_IPMC_GROUP_NUM(unit)), "IPMC-alloc");
    if (info->ipmc_allocated == NULL) {
        return BCM_E_MEMORY;
    }

    info->ipmc_l3_index =
        sal_alloc(TR_IPMC_GROUP_NUM(unit) * sizeof(int), "IPMC-l3");
    if (info->ipmc_l3_index == NULL) {
        sal_free(info->ipmc_allocated);
        info->ipmc_allocated = NULL;
        return (BCM_E_MEMORY);
    }

    info->ipmc_ip6 =
        sal_alloc(TR_IPMC_GROUP_NUM(unit) * sizeof(int), "IPMC-v6");
    if (info->ipmc_ip6 == NULL) {
        sal_free(info->ipmc_allocated);
        info->ipmc_allocated = NULL;
        sal_free(info->ipmc_l3_index);
        info->ipmc_l3_index = NULL;
        return (BCM_E_MEMORY);
    }

    sal_memset((char *)(tr_ipmc_info[unit].ipmc_allocated), 0,
               SHR_BITALLOCSIZE(tr_ipmc_info[unit].ipmc_size));

    /* Initialize EGR_IPMC entries to have L3_PAYLOAD == 1 */
    sal_memset(&egr_entry, 0, sizeof(egr_entry));
    if (soc_mem_field_valid(unit, EGR_IPMCm, L3_PAYLOADf)) {
        soc_EGR_IPMCm_field32_set(unit, &egr_entry, L3_PAYLOADf, 0);
    }
    if (soc_mem_field_valid(unit, EGR_IPMCm, REPLICATION_TYPEf)) {
        soc_EGR_IPMCm_field32_set(unit, &egr_entry, REPLICATION_TYPEf, 0);
    }
    soc_EGR_IPMCm_field32_set(unit, &egr_entry, DONT_PRUNE_VLANf, 0);

    for (i = 0; i < TR_IPMC_GROUP_NUM(unit); i++) {
        info->ipmc_l3_index[i] = -1;
        info->ipmc_ip6[i] = 0;
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            /* Initialize identity mapping */
            sal_memset(&remap_entry, 0, sizeof(remap_entry));
            soc_L3_IPMC_REMAPm_field32_set(unit, &remap_entry, L3MC_INDEXf, i);
            rv = WRITE_L3_IPMC_REMAPm(unit, MEM_BLOCK_ALL, i, &remap_entry);
        }
#endif
        rv = WRITE_EGR_IPMCm(unit, MEM_BLOCK_ALL, i, &egr_entry);
        if (rv < 0) {
             sal_free(info->ipmc_allocated);
             info->ipmc_allocated = NULL;
             sal_free(info->ipmc_l3_index);
             info->ipmc_l3_index = NULL;
             sal_free(info->ipmc_ip6);
             info->ipmc_ip6 = NULL;
             return rv;
        }
    }
    info->ipmc_initialized = TRUE;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_ipmc_detach
 * Purpose:
 *      Detach the IPMC module.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_detach(int unit)
{
    _bcm_tr_ipmc_t    *info = TR_IPMC_INFO(unit);

    if (info->ipmc_initialized) {
        BCM_IF_ERROR_RETURN(_tr_ipmc_enable(unit, FALSE));

        if (info->ipmc_allocated != NULL) {
            sal_free(info->ipmc_allocated);
            info->ipmc_allocated = NULL;
        }

        if (info->ipmc_l3_index != NULL) {
            sal_free(info->ipmc_l3_index);
            info->ipmc_l3_index = NULL;
        }

        if (info->ipmc_ip6 != NULL) {
            sal_free(info->ipmc_ip6);
            info->ipmc_ip6 = NULL;
        }

        info->ipmc_initialized = FALSE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_ipmc_get
 * Purpose:
 *      Get an IPMC entry by index.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      index - The index number.
 *      ipmc - (OUT) IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_get(int unit, int index, bcm_ipmc_addr_t *ipmc)
{
    ipmc_entry_t        ipmc_entry;
#ifdef BCM_TRIUMPH2_SUPPORT
    ipmc_1_entry_t      ipmc_1_entry;
#endif
    int                 rv = BCM_E_NONE;

    TR_IPMC_INIT(unit);
    TR_IPMC_ID(unit, index);

    if (TR_IPMC_USED_ISSET(unit, index) &&
        (tr_ipmc_info[unit].ipmc_l3_index[index] != -1)) {

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, index, &ipmc_entry));
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, L3_IPMC_1m, MEM_BLOCK_ANY, index, 
                              &ipmc_1_entry));
            BCM_IF_ERROR_RETURN
                (_tr2_ipmc_glp_get(unit, ipmc, &ipmc_1_entry));
        }
#endif
        BCM_IF_ERROR_RETURN
            (_tr_ipmc_info_get(unit, tr_ipmc_info[unit].ipmc_l3_index[index],
                               index, ipmc, &ipmc_entry));
        ipmc->ipmc_index = index;
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr_ipmc_lookup
 * Purpose:
 *      Look up an IPMC entry by sip, mcip and vid
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      index - (OUT) The index number.
 *      ipmc - (IN, OUT) IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_lookup(int unit, int *index, bcm_ipmc_addr_t *ipmc)
{
    ipmc_entry_t  ipmc_entry;
#ifdef BCM_TRIUMPH2_SUPPORT
    ipmc_1_entry_t ipmc_1_entry;
#endif
    _bcm_l3_cfg_t l3cfg;
    int           ipmc_id;

    TR_IPMC_INIT(unit);

    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
    l3cfg.l3c_vid = ipmc->vid;
    l3cfg.l3c_flags = BCM_L3_IPMC;
    l3cfg.l3c_vrf = ipmc->vrf;
    if (ipmc->flags & BCM_IPMC_IP6) {
        sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
        l3cfg.l3c_flags |= BCM_L3_IP6;
        BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg));
    } else {
        l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
        l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
        BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg));
    }

    ipmc_id = l3cfg.l3c_ipmc_ptr;
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, ipmc_id, &ipmc_entry));

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, L3_IPMC_1m, MEM_BLOCK_ANY, ipmc_id, 
                          &ipmc_1_entry));
        BCM_IF_ERROR_RETURN
            (_tr2_ipmc_glp_get(unit, ipmc, &ipmc_1_entry));
    }
#endif
    ipmc_id = l3cfg.l3c_hw_index;

    BCM_IF_ERROR_RETURN
        (_tr_ipmc_info_get(unit, ipmc_id, -1, ipmc, &ipmc_entry));

    if (index != NULL) {
        *index = l3cfg.l3c_ipmc_ptr;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_ipmc_add
 * Purpose:
 *      Add a new entry to the L3 table.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ipmc - (IN) IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc)
{
    _bcm_l3_cfg_t       l3cfg; /* L3 ipmc entry.           */
    int                 rv;    /* Operation return status. */

    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));

    l3cfg.l3c_vid = ipmc->vid;
    l3cfg.l3c_flags = BCM_L3_IPMC;
    l3cfg.l3c_vrf = ipmc->vrf;
    l3cfg.l3c_lookup_class = ipmc->lookup_class;
    if (ipmc->flags & BCM_IPMC_SETPRI) {
        l3cfg.l3c_flags |= BCM_L3_RPE;
        l3cfg.l3c_prio = ipmc->cos;
    }
    if (ipmc->flags & BCM_IPMC_IP6) {
        if (!BCM_IP6_MULTICAST(ipmc->mc_ip6_addr)) {
            return BCM_E_PARAM;
        }
        sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
        l3cfg.l3c_flags |= BCM_L3_IP6;
    } else {
        if (!BCM_IP4_MULTICAST(ipmc->mc_ip_addr)) {
            return BCM_E_PARAM;
        }
        l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
        l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
    }

    l3cfg.l3c_ipmc_ptr = ipmc->ipmc_index;
    l3cfg.l3c_flags |=  BCM_L3_HIT;
    l3cfg.l3c_vid = ipmc->vid;

    if (ipmc->flags & BCM_IPMC_REPLACE) {
        l3cfg.l3c_flags |= BCM_L3_REPLACE;
    }

    if (ipmc->flags & BCM_IPMC_IP6) {
        rv = mbcm_driver[unit]->mbcm_l3_ip6_add(unit, &l3cfg);
        /* Get L3 hw entry index. */
        if (BCM_SUCCESS(rv)) {
            rv = mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg);
        }
    } else {
        rv = mbcm_driver[unit]->mbcm_l3_ip4_add(unit, &l3cfg);
        /* Get L3 hw entry index. */
        if (BCM_SUCCESS(rv)) {
            rv = mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg);
        }
    }

    if (BCM_SUCCESS(rv)) {
        tr_ipmc_info[unit].ipmc_ip6[ipmc->ipmc_index] =
            (ipmc->flags & BCM_IPMC_IP6) ? 1: 0;

        tr_ipmc_info[unit].ipmc_l3_index[ipmc->ipmc_index] =
            l3cfg.l3c_hw_index;
    }
    return (rv);
}

/*
 * Function:
 *      _bcm_tr_ipmc_del
 * Purpose:
 *      Remove an  entry from the L3 table.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ipmc - (IN)IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_tr_ipmc_del(int unit, bcm_ipmc_addr_t *ipmc)
{
    _bcm_l3_cfg_t       l3cfg; /* L3 ipmc entry.           */
    int                 rv;    /* Operation return status. */

    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));

    l3cfg.l3c_vid = ipmc->vid;
    l3cfg.l3c_flags = BCM_L3_IPMC;
    l3cfg.l3c_vrf = ipmc->vrf;
    l3cfg.l3c_vid = ipmc->vid;

    if (ipmc->flags & BCM_IPMC_IP6) {
        if (!BCM_IP6_MULTICAST(ipmc->mc_ip6_addr)) {
            return BCM_E_PARAM;
        }
        sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
        l3cfg.l3c_flags |= BCM_L3_IP6;
    } else {
        if (!BCM_IP4_MULTICAST(ipmc->mc_ip_addr)) {
            return BCM_E_PARAM;
        }
        l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
        l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
    }

    if (ipmc->flags & BCM_IPMC_IP6) {
        rv = mbcm_driver[unit]->mbcm_l3_ip6_delete(unit, &l3cfg);
    } else {
        rv = mbcm_driver[unit]->mbcm_l3_ip4_delete(unit, &l3cfg);
    }
    return (rv);
}

/*
 * Function:
 *      bcm_tr_ipmc_add
 * Purpose:
 *      Add a new entry to the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      ipmc - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc)
{

    bcm_ipmc_addr_t orig_data;
    bcm_ipmc_addr_t orig_idx_data;
    ipmc_entry_t ipmc_entry;
    int new_entry, rv;

    TR_IPMC_INIT(unit);

    orig_data = *ipmc; 

    /* Make sure identical entry doesn't exist. */
    rv = bcm_esw_ipmc_find(unit, &orig_data);
    if (BCM_SUCCESS(rv)) { 
        if (!(ipmc->flags & BCM_IPMC_REPLACE)) {
           return (BCM_E_EXISTS);
        } else {
            ipmc->ipmc_index = orig_data.ipmc_index;
            new_entry = FALSE;
        }
    } else {
        /* Return if error occured. */
        if (rv != BCM_E_NOT_FOUND) {
            return (rv);
        }
        new_entry = TRUE;
    }

    /* Make sure requested index, if any, is not used by some other entry. */
    if(ipmc->flags & BCM_IPMC_USE_IPMC_INDEX) {
         if (_BCM_MULTICAST_IS_SET(orig_data.ipmc_index)) {
              ipmc->ipmc_index = _BCM_MULTICAST_ID_GET(orig_data.ipmc_index); 
              BCM_IF_ERROR_RETURN
                  (_bcm_trx_multicast_ipmc_read(unit,  ipmc->ipmc_index, 
                                        &ipmc->l2_pbmp, &ipmc->l3_pbmp));

         } else {
              /* If replacement required make sure proper index provided. */
              if (!new_entry) {
                   if (orig_data.ipmc_index != ipmc->ipmc_index) {
                        return (BCM_E_PARAM);
                   }
              }  else { /* Make sure index provided for new entry  */
                   /* is not used by some other entry.        */
                   /* Preserve entry ipv4/ipv6 flags. */
                   orig_idx_data.flags = ipmc->flags;
                   rv = bcm_tr_ipmc_get(unit, ipmc->ipmc_index, &orig_idx_data);
                   if (BCM_E_NOT_FOUND == rv) {
                        /* Mark unused index as taken. */
                       BCM_IF_ERROR_RETURN(_bcm_tr_ipmc_id_alloc(unit, ipmc->ipmc_index));
                   } else {
                       BCM_IF_ERROR_RETURN(rv);
                       return (BCM_E_EXISTS);
                   }
              }
    	 }
    } else if (new_entry){
        /* Allocate new unused entry index. */
        BCM_IF_ERROR_RETURN(_bcm_tr_ipmc_create(unit, &ipmc->ipmc_index));
    }

    /* Write L3_IPMC table entry. */
    ipmc->v = (ipmc->flags & BCM_IPMC_ADD_DISABLED) ? 0 : 1;
    rv = _tr_ipmc_write(unit, ipmc->ipmc_index, ipmc);
    if (BCM_FAILURE(rv)) {
        if (!new_entry) {
            _bcm_tr_ipmc_del(unit, ipmc);
        }
        TR_IPMC_USED_CLR(unit, ipmc->ipmc_index);
        return (rv);
    }

    if (!new_entry) {
        return (BCM_E_NONE);
    }

    rv = _bcm_tr_ipmc_add(unit, ipmc);
    if (BCM_FAILURE(rv)) {
         sal_memset(&ipmc_entry, 0, sizeof(ipmc_entry));
         SOC_IF_ERROR_RETURN
             (soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, ipmc->ipmc_index, &ipmc_entry));
        TR_IPMC_USED_CLR(unit, ipmc->ipmc_index);
    }
    return (rv);
}

/*
 * Function:
 *      bcm_tr_ipmc_put
 * Purpose:
 *      Overwrite an entry in the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      index - Table index to overwrite.
 *      data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_put(int unit, int index, bcm_ipmc_addr_t *ipmc)
{
    _bcm_l3_cfg_t   l3cfg;

    TR_IPMC_INIT(unit);
    TR_IPMC_ID(unit, index); 

    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));

    l3cfg.l3c_vid = ipmc->vid;
    l3cfg.l3c_flags = BCM_L3_IPMC;
    l3cfg.l3c_vrf = ipmc->vrf;
    l3cfg.l3c_lookup_class = ipmc->lookup_class;
    if (ipmc->flags & BCM_IPMC_IP6) {
        sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
        l3cfg.l3c_flags |= BCM_L3_IP6;
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg));
    } else {
        l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
        l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg));
    }
    if (!(ipmc->flags & BCM_IPMC_SETPRI)) {
        l3cfg.l3c_flags &= ~BCM_L3_RPE;
        l3cfg.l3c_prio = 0;
    } else {
        l3cfg.l3c_flags |= BCM_L3_RPE;
        l3cfg.l3c_prio = ipmc->cos;
    }
    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_replace(unit, &l3cfg));

    return _tr_ipmc_write(unit, index, ipmc);
}

/*
 * Function:
 *      bcm_tr_ipmc_delete
 * Purpose:
 *      Delete an entry from the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If BCM_IPMC_KEEP_ENTRY is true, the entry valid bit is cleared
 *      but the entry is not deleted from the table.
 */

int
bcm_tr_ipmc_delete(int unit, bcm_ipmc_addr_t *ipmc)
{
    int                 ipmc_id;
    ipmc_entry_t        ipmc_entry;
    _bcm_l3_cfg_t       l3cfg;
    bcm_pbmp_t          repl_pbmp;

    TR_IPMC_INIT(unit);

    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
    l3cfg.l3c_vid = ipmc->vid;
    l3cfg.l3c_flags = BCM_L3_IPMC;
    l3cfg.l3c_vrf = ipmc->vrf;
    if (ipmc->flags & BCM_IPMC_IP6) {
        sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
        l3cfg.l3c_flags |= BCM_L3_IP6;
        BCM_IF_ERROR_RETURN(
            mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg));
    } else {
        l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
        l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
        BCM_IF_ERROR_RETURN(
            mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg));
    }

    ipmc_id = l3cfg.l3c_ipmc_ptr;
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, ipmc_id, &ipmc_entry));

    SOC_PBMP_CLEAR(repl_pbmp);
    SOC_PBMP_WORD_SET(repl_pbmp, 0,
                      soc_L3_IPMCm_field32_get(unit, &ipmc_entry,
                                               L3_BITMAP_LOf));

    if(!SOC_IS_ENDURO(unit)) {
        SOC_PBMP_WORD_SET(repl_pbmp, 1,
                          soc_L3_IPMCm_field32_get(unit, &ipmc_entry,
                                                   L3_BITMAP_HIf));
    }
    SOC_PBMP_AND(repl_pbmp, PBMP_PORT_ALL(unit));

    sal_memset(&ipmc_entry, 0, sizeof(ipmc_entry));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, ipmc_id, &ipmc_entry));

    if (!(ipmc->flags & BCM_IPMC_KEEP_ENTRY)) {
        l3cfg.l3c_flags = BCM_L3_IPMC;
        if (ipmc->flags & BCM_IPMC_IP6) {
            l3cfg.l3c_flags |= BCM_L3_IP6;
            BCM_IF_ERROR_RETURN(
                mbcm_driver[unit]->mbcm_l3_ip6_delete(unit, &l3cfg));
        } else {
            BCM_IF_ERROR_RETURN(
                mbcm_driver[unit]->mbcm_l3_ip4_delete(unit, &l3cfg));
        }

        /* Delete all replications */
        BCM_IF_ERROR_RETURN
            (bcm_esw_ipmc_repl_delete_all(unit, ipmc_id, repl_pbmp));

        /* Mark as unused */
        tr_ipmc_info[unit].ipmc_l3_index[ipmc_id] = -1;
        tr_ipmc_info[unit].ipmc_ip6[ipmc_id] = 0;
        TR_IPMC_USED_CLR(unit, ipmc_id);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_ipmc_delete_all
 * Purpose:
 *      Delete all entries from the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_delete_all(int unit)
{
    _bcm_tr_ipmc_t  *info = TR_IPMC_INFO(unit);
    ipmc_entry_t ipmc_entry;
    bcm_pbmp_t repl_pbmp;
    int i, flags, rv = BCM_E_NONE;

    TR_IPMC_INIT(unit);

    TR_IPMC_LOCK(unit);
    for (i = 0; i < TR_IPMC_GROUP_NUM(unit); i++) {
        if (info->ipmc_l3_index[i] != -1) {
            flags = BCM_L3_IPMC;
            if(info->ipmc_ip6[i]) {
               flags |=  BCM_L3_IP6;
            }
            rv = bcm_xgs3_l3_del_by_index(unit, info->ipmc_l3_index[i], flags);
            if (rv < 0) {
                break;
            }

            /* Get the replication L3 port bitmap */
            rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, i, &ipmc_entry);
            if (rv < 0) {
                break;
            }
            SOC_PBMP_CLEAR(repl_pbmp);
            SOC_PBMP_WORD_SET(repl_pbmp, 0,
                              soc_L3_IPMCm_field32_get(unit, &ipmc_entry,
                                                       L3_BITMAP_LOf));
            if(!SOC_IS_ENDURO(unit)) {
                SOC_PBMP_WORD_SET(repl_pbmp, 1,
                                  soc_L3_IPMCm_field32_get(unit, &ipmc_entry,
                                                           L3_BITMAP_HIf));
            }
            SOC_PBMP_AND(repl_pbmp, PBMP_PORT_ALL(unit));

            /* Clear the L3_IPMC entry */
            rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, i,
                                soc_mem_entry_null(unit, L3_IPMCm));
            if (rv < 0) {
                break;
            }

            /* Delete all replications */
            rv = bcm_esw_ipmc_repl_delete_all(unit, i, repl_pbmp);
            if (rv < 0) {
                break;
            }

            /* Mark as unused */
            info->ipmc_l3_index[i] = -1;
            info->ipmc_ip6[i] = 0;
            TR_IPMC_USED_CLR(unit, i);
        }
    }
    TR_IPMC_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_tr_ipmc_age
 * Purpose:
 *      Age out the ipmc entry by clearing the HIT bit when appropriate,
 *      the ipmc entry itself is removed if HIT bit is not set.
 * Parameters:
 *      unit       -  (IN) BCM device number.
 *      flags      -  (IN) The criteria used to age out ipmc table.
 *                         IPv6/IPv4
 *      age_cb     -  (IN) Call back routine.
 *      user_data  -  (IN) User provided cookie for callback.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_cb,
                void *user_data)
{
    int idx;                   /* Ipmc table iteration index. */
    bcm_ipmc_addr_t entry;     /* Ipmc entry iterator.        */
    int rv = BCM_E_NONE;       /* Operation return status.    */

    TR_IPMC_INIT(unit);
    TR_IPMC_LOCK(unit);

    for (idx = 0; idx < TR_IPMC_GROUP_NUM(unit); idx++) {
        if (TR_IPMC_USED_ISSET(unit, idx)) {
            sal_memset(&entry, 0, sizeof(bcm_ipmc_addr_t));
            /* Read entry at specific index . */
            rv = bcm_tr_ipmc_get(unit, idx, &entry);
            if (BCM_FAILURE(rv)) {
                break;
            }

            if ((flags & BCM_IPMC_IP6) != (entry.flags & BCM_IPMC_IP6)) {
                continue;
            }

            /* Make sure update only ipv4 or ipv6 entries. */
            if (entry.flags & BCM_IPMC_HIT) {
                /* Clear hit bit on used entry. */
                entry.flags |= BCM_IPMC_HIT_CLEAR;
                rv = bcm_tr_ipmc_get(unit, idx, &entry);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            } else {
                /* Delete & notify caller on unused entry. */
                rv = bcm_tr_ipmc_delete(unit, &entry);
                if (BCM_FAILURE(rv)) {
                    break;
                }

                /* Invoke user callback. */
                if (NULL != age_cb) {
                    (*age_cb)(unit, &entry, user_data);
                }
            }
        }
    }
    TR_IPMC_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_tr_ipmc_traverse
 * Purpose:
 *      Go through all valid ipmc entries, and call the callback function
 *      at each entry
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      flags     - (IN) The criteria used to age out ipmc table.
 *      cb        - (IN) User supplied callback function.
 *      user_data - (IN) User supplied cookie used in parameter
 *                       in callback function.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_ipmc_traverse(int unit, uint32 flags, bcm_ipmc_traverse_cb cb,
                     void *user_data)
{
    int idx;                   /* Ipmc table iteration index. */
    bcm_ipmc_addr_t entry;     /* Ipmc entry iterator.        */
    int rv = BCM_E_NONE;       /* Operation return status.    */

    TR_IPMC_INIT(unit);
    TR_IPMC_LOCK(unit);

    for (idx = 0; idx < TR_IPMC_GROUP_NUM(unit); idx++) {
        if (TR_IPMC_USED_ISSET(unit, idx)) {
            sal_memset(&entry, 0, sizeof(bcm_ipmc_addr_t));
            /* Read entry at specific index . */
            rv = bcm_tr_ipmc_get(unit, idx, &entry);
            if (BCM_FAILURE(rv)) {
                break;
            }

            if ((flags & BCM_IPMC_IP6) != (entry.flags & BCM_IPMC_IP6)) {
                continue;
            }

            /* Invoke user callback. */
            (*cb)(unit, &entry, user_data);
        }
    }
    TR_IPMC_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_tr_ipmc_enable
 * Purpose:
 *      Enable or disable IPMC chip functions.
 * Parameters:
 *      unit - Unit number
 *      enable - TRUE to enable; FALSE to disable
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_enable(int unit, int enable)
{
    TR_IPMC_INIT(unit);

    return _tr_ipmc_enable(unit, enable);
}


/*
 * Function:
 *      bcm_tr_ipmc_src_port_check
 * Purpose:
 *      Enable or disable Source Port checking in IPMC lookups.
 * Parameters:
 *      unit - Unit number
 *      enable - TRUE to enable; FALSE to disable
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_src_port_check(int unit, int enable)
{
    TR_IPMC_INIT(unit);

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tr_ipmc_src_ip_search
 * Purpose:
 *      Enable or disable Source IP significance in IPMC lookups.
 * Parameters:
 *      unit - Unit number
 *      enable - TRUE to enable; FALSE to disable
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_src_ip_search(int unit, int enable)
{
    TR_IPMC_INIT(unit);

    if (enable) {
        return BCM_E_NONE;  /* always on */
    } else {
        return BCM_E_FAIL;  /* cannot be disabled */
    }
}

/*
 * Function:
 *      bcm_tr_ipmc_egress_port_set
 * Purpose:
 *      Configure the IP Multicast egress properties
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port to config.
 *      mac  - MAC address.
 *      untag - 1: The IP multicast packet is transmitted as untagged packet.
 *              0: The IP multicast packet is transmitted as tagged packet
 *              with VLAN tag vid.
 *      vid  - VLAN ID.
 *      ttl  - 1 to disable the TTL decrement, 0 otherwise.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_egress_port_set(int unit, bcm_port_t port,
                            const bcm_mac_t mac, int untag,
                            bcm_vlan_t vid, int ttl)
{
    uint32      cfg2;

    if (!SOC_PBMP_PORT_VALID(port) || !IS_PORT(unit, port)) {
        return BCM_E_BADID;
    }

    SOC_IF_ERROR_RETURN(READ_EGR_IPMC_CFG2r(unit, port, &cfg2));

    soc_reg_field_set(unit, EGR_IPMC_CFG2r, &cfg2,
                      UNTAGf, untag ? 1 : 0);
    soc_reg_field_set(unit, EGR_IPMC_CFG2r, &cfg2,
                      VIDf, vid);

    
    SOC_IF_ERROR_RETURN(WRITE_EGR_IPMC_CFG2r(unit, port, cfg2));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_ipmc_egress_port_get
 * Purpose:
 *      Return the IP Multicast egress properties
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port to config.
 *      mac - (OUT) MAC address.
 *      untag - (OUT) 1: The IP multicast packet is transmitted as
 *                       untagged packet.
 *                    0: The IP multicast packet is transmitted as tagged
 *                       packet with VLAN tag vid.
 *      vid - (OUT) VLAN ID.
 *      ttl_thresh - (OUT) Drop IPMC packets if TTL <= ttl_thresh.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr_ipmc_egress_port_get(int unit, bcm_port_t port, mac_addr_t mac,
                            int *untag, bcm_vlan_t *vid, int *ttl_thresh)
{
    uint32              cfg2;

    if (!SOC_PBMP_PORT_VALID(port) || !IS_PORT(unit, port)) {
        return BCM_E_BADID;
    }

    SOC_IF_ERROR_RETURN(READ_EGR_IPMC_CFG2r(unit, port, &cfg2));

    *untag = soc_reg_field_get(unit, EGR_IPMC_CFG2r, cfg2, UNTAGf);
    *vid = soc_reg_field_get(unit, EGR_IPMC_CFG2r, cfg2, VIDf);
    *ttl_thresh = -1;

    return BCM_E_NONE;
}

#endif  /* INCLUDE_L3 */

int _bcm_tr_firebolt_ipmc_not_empty;
