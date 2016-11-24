/*
 * $Id: triumph_l3.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:        l3.c
 * Purpose:     Triumph L3 function implementations
 */


#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH_SUPPORT)  && defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>

#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>


/*
 * Function:
 *      _bcm_tr_defip_init
 * Purpose:
 *      Initialize L3 DEFIP table for triumph devices.
 * Parameters:
 *      unit    - (IN)  SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_defip_init(int unit)
{
    soc_mem_t mem_v4;      /* IPv4 Route table memory.             */
    soc_mem_t mem_v6;      /* IPv6 Route table memory.             */
    soc_mem_t mem_v6_128;  /* IPv6 full prefix route table memory. */

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_tr_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Initialize IPv4 entries lookup engine. */
    if (L3_DEFIPm == mem_v4) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_tr_ext_lpm_init(unit, mem_v4));
    }

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Initialize IPv6 entries lookup engine. */
    if (L3_DEFIPm == mem_v6) {
        if (mem_v4 != mem_v6) {
            BCM_IF_ERROR_RETURN(soc_fb_lpm_init(unit));
        }
    } else {
        BCM_IF_ERROR_RETURN(_bcm_tr_ext_lpm_init(unit, mem_v6));
    }

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
        BCM_IF_ERROR_RETURN(_bcm_trx_defip_128_init(unit));
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _bcm_tr_defip_deinit
 * Purpose:
 *      De-initialize L3 DEFIP table for triumph devices.
 * Parameters:
 *      unit    - (IN)  SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_defip_deinit(int unit)
{
    soc_mem_t mem_v4;      /* IPv4 Route table memory.             */
    soc_mem_t mem_v6;      /* IPv6 Route table memory.             */
    soc_mem_t mem_v6_128;  /* IPv6 full prefix route table memory. */

    /* Get memory for IPv4 entries. */
    BCM_IF_ERROR_RETURN(_bcm_tr_l3_defip_mem_get(unit, 0, 0, &mem_v4));

    /* Initialize IPv4 entries lookup engine. */
    if (L3_DEFIPm == mem_v4) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_tr_ext_lpm_deinit(unit, mem_v4));
    }

    /* Get memory for IPv6 entries. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr_l3_defip_mem_get(unit, BCM_L3_IP6, 0, &mem_v6));

    /* Initialize IPv6 entries lookup engine. */
    if ((L3_DEFIPm == mem_v6) && (mem_v4 != mem_v6)) {
        BCM_IF_ERROR_RETURN(soc_fb_lpm_deinit(unit));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_tr_ext_lpm_deinit(unit, mem_v6));
    }

    /* Get memory for IPv6 entries with prefix length > 64. */
    BCM_IF_ERROR_RETURN
        (_bcm_tr_l3_defip_mem_get(unit, BCM_L3_IP6,
                                  BCM_XGS3_L3_IPV6_PREFIX_LEN,
                                  &mem_v6_128));

    /* Initialize IPv6 entries lookup engine. */
    if (mem_v6 != mem_v6_128) {
        BCM_IF_ERROR_RETURN(_bcm_trx_defip_128_deinit(unit));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr_l3_ingress_interface_get
 * Purpose:
 *      Get ingress interface config.
 * Parameters:
 *      unit     - (IN) BCM unit number.
 *      iif      - (IN) Interface configuration.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr_l3_ingress_interface_get(int unit, _bcm_l3_ingress_intf_t *iif)
{
    iif_entry_t entry;       /* HW entry buffer.        */
    int intf_id;             /* Interface id.           */
    int rv;                  /* Operation return status.*/

    
    /* Input parameters sanity check. */
    if (NULL == iif) {
        return (BCM_E_PARAM);
    }
    if ((iif->intf_id > soc_mem_index_max(unit, L3_IIFm)) || 
        (iif->intf_id < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    intf_id = iif->intf_id;
    sal_memset(iif, 0, sizeof(_bcm_l3_ingress_intf_t));
    sal_memcpy(&entry,  soc_mem_entry_null(unit, L3_IIFm),
               sizeof(iif_entry_t));

    /* Read interface config from hw. */
    rv = soc_mem_read(unit, L3_IIFm, MEM_BLOCK_ANY, intf_id, (uint32 *)&entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Get class id value. */
    iif->if_class = soc_mem_field32_get(unit, L3_IIFm, (uint32 *)&entry, CLASS_IDf);

    /* Get vrf id value. */
    iif->vrf = soc_mem_field32_get(unit, L3_IIFm, (uint32 *)&entry, VRFf);

    /* Get vrf id value. */
    if (0 == soc_mem_field32_get(unit, L3_IIFm, (uint32 *)&entry,
                                 ALLOW_GLOBAL_ROUTEf)) {
        iif->flags |= BCM_VLAN_L3_VRF_GLOBAL_DISABLE; 
    }

    /* Set interface id. */
    iif->intf_id = intf_id;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr_l3_ingress_interface_set
 * Purpose:
 *      Set ingress interface config.
 * Parameters:
 *      unit     - (IN) BCM unit number.
 *      iif      - (IN) Interface configuration.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr_l3_ingress_interface_set(int unit, _bcm_l3_ingress_intf_t *iif)
{
    iif_entry_t entry;          /* HW entry buffer.        */
    uint8 value;                /* Enable/disable boolean. */

    /* Input parameters sanity check. */
    if (NULL == iif) {
        return (BCM_E_PARAM);
    }
    if ((iif->intf_id > soc_mem_index_max(unit, L3_IIFm)) || 
        (iif->intf_id < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    sal_memcpy(&entry,  soc_mem_entry_null(unit, L3_IIFm),
               sizeof(iif_entry_t));

    /* Set class id value. */
    soc_mem_field32_set(unit, L3_IIFm, (uint32 *)&entry, CLASS_IDf,
                        iif->if_class);

    /* Set vrf id value. */
    soc_mem_field32_set(unit, L3_IIFm, (uint32 *)&entry, VRFf, iif->vrf);

    /* Set identity mapping for IPMC L3 IIF */
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_mem_field_valid(unit, L3_IIFm, IPMC_L3_IIFf)) {
        soc_mem_field32_set(unit, L3_IIFm, (uint32 *)&entry, IPMC_L3_IIFf, 
                            iif->intf_id);
    }
#endif    
    /* Set enable global route value. */
    value  = (iif->flags & BCM_VLAN_L3_VRF_GLOBAL_DISABLE) ?  0 : 1;
    soc_mem_field32_set(unit, L3_IIFm, (uint32 *)&entry,
                        ALLOW_GLOBAL_ROUTEf, value); 

    /* Write  updated entry back to hw.*/
    return  soc_mem_write(unit, L3_IIFm, MEM_BLOCK_ALL, iif->intf_id, 
                          (uint32 *)&entry);
}

/*
 * Function:
 *      _bcm_tr_l3_ingress_interface_clr
 * Purpose:
 *      Set ingress interface config.
 * Parameters:
 *      unit     - (IN) BCM unit number.
 *      iif      - (IN) Interface configuration.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr_l3_ingress_interface_clr(int unit, int intf_id)
{
    iif_entry_t entry;          /* HW entry buffer.        */

    /* Input parameters sanity check. */
    if ((intf_id > soc_mem_index_max(unit, L3_IIFm)) || 
        (intf_id < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    sal_memcpy(&entry,  soc_mem_entry_null(unit, L3_IIFm),
               sizeof(iif_entry_t));

    /* Write  updated entry back to hw.*/
    return  soc_mem_write(unit, L3_IIFm, MEM_BLOCK_ALL, intf_id, 
                          (uint32 *)&entry);
}

/*
 * Function:
 *      _bcm_tr_l3_intf_class_set
 * Purpose:
 *      Set classigication class to the interface. 
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      vid        - (IN) vlan id.
 *      intf_class - (IN) Interface class id.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr_l3_intf_class_set(int unit, bcm_vlan_t vid, uint32 intf_class)
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/
    int ret_val;                /* Operation return value.  */

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    if (intf_class> SOC_INTF_CLASS_MAX(unit)) {
        return (BCM_E_PARAM);
    }
    
    iif.intf_id = vid;
    soc_mem_lock(unit, L3_IIFm);

    ret_val = _bcm_tr_l3_ingress_interface_get(unit, &iif);
    if (BCM_FAILURE(ret_val)) {
        soc_mem_unlock(unit, L3_IIFm);
        return (ret_val);
    }

    iif.if_class = intf_class;

    ret_val = _bcm_tr_l3_ingress_interface_set(unit, &iif);

    soc_mem_unlock(unit, L3_IIFm);

    return (ret_val);
}

/*
 * Function:
 *      _bcm_tr_l3_intf_global_route_enable_get
 * Purpose:
 *      Get the global route enable flag for the specified L3 interface
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vid       - (IN)Vlan id. 
 *      enable    - (OUT)enable. 
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr_l3_intf_global_route_enable_get(int unit, bcm_vlan_t vid, int *enable)
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    iif.intf_id = vid;

    BCM_IF_ERROR_RETURN(_bcm_tr_l3_ingress_interface_get(unit, &iif));

    *enable = (iif.flags & BCM_VLAN_L3_VRF_GLOBAL_DISABLE) ? 0 : 1;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr_l3_intf_global_route_enable_set
 * Purpose:
 *      Set global route enable flag for the L3 interface. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vid       - (IN)Vlan id. 
 *      enable    - (IN)Enable. 
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr_l3_intf_global_route_enable_set(int unit, bcm_vlan_t vid, int enable)  
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/
    int ret_val;                /* Operation return value.  */

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    iif.intf_id = vid;
    soc_mem_lock(unit, L3_IIFm);

    ret_val = _bcm_tr_l3_ingress_interface_get(unit, &iif);
    if (BCM_FAILURE(ret_val)) {
        soc_mem_unlock(unit, L3_IIFm);
        return (ret_val);
    }

    if (enable) {
       iif.flags &= ~BCM_VLAN_L3_VRF_GLOBAL_DISABLE;
    } else {
       iif.flags |= BCM_VLAN_L3_VRF_GLOBAL_DISABLE;
    }

    ret_val = _bcm_tr_l3_ingress_interface_set(unit, &iif);

    soc_mem_unlock(unit, L3_IIFm);

    return (ret_val);
}

/*
 * Function:
 *      _bcm_tr_l3_intf_vrf_get
 * Purpose:
 *      Get the VRF,flags info for the specified L3 interface
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vid       - (IN)Vlan id. 
 *      vrf       - (OUT)Vrf. 
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr_l3_intf_vrf_get(int unit, bcm_vlan_t vid, bcm_vrf_t *vrf)
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    iif.intf_id = vid;

    BCM_IF_ERROR_RETURN(_bcm_tr_l3_ingress_interface_get(unit, &iif));

    *vrf = iif.vrf;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr_l3_intf_vrf_bind
 * Purpose:
 *      Bind interface to the VRF & update if info.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vid       - (IN)Vlan id. 
 *      vrf       - (OUT)Vrf. 
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr_l3_intf_vrf_bind(int unit, bcm_vlan_t vid, bcm_vrf_t vrf)  
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/
    int ret_val;                /* Operation return value.  */

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    if (vrf > SOC_VRF_MAX(unit)) {
        return (BCM_E_PARAM);
    }
    
    iif.intf_id = vid;
    soc_mem_lock(unit, L3_IIFm);

    ret_val = _bcm_tr_l3_ingress_interface_get(unit, &iif);
    if (BCM_FAILURE(ret_val)) {
        soc_mem_unlock(unit, L3_IIFm);
        return (ret_val);
    }

    iif.vrf = vrf;

    ret_val = _bcm_tr_l3_ingress_interface_set(unit, &iif);

    soc_mem_unlock(unit, L3_IIFm);

    return (ret_val);
}

/*
 * Function:
 *      _bcm_tr_l3_defip_mem_get
 * Purpose:
 *      Resolve route table memory for a given route entry.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      flags      - (IN)IPv6/IPv4 route.
 *      plen       - (IN)Prefix length.
 *      mem        - (OUT)Route table memory.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_l3_defip_mem_get(int unit, uint32 flags, int plen, soc_mem_t *mem)
{
#if defined(BCM_VALKYRIE_SUPPORT) || defined(BCM_ENDURO_SUPPORT) || \
    defined(BCM_APOLLO_SUPPORT) || defined(BCM_VALKYRIE2_SUPPORT)
    if (!soc_feature(unit, soc_feature_esm_support)) {
        if (flags & BCM_L3_IP6) {
            if (plen > 64) {
                *mem = L3_DEFIP_128m;
            } else {  /* IPv6 prefix length < 64 bit. */
                *mem = L3_DEFIPm;
            }
        } else { /* IPv4 route. */
            *mem = L3_DEFIPm;
        }
        return (BCM_E_NONE);
    }
#endif
    if (flags & BCM_L3_IP6) {
        if (plen > 64) {
            if (soc_mem_index_count(unit, EXT_IPV6_128_DEFIPm)) {
                *mem = EXT_IPV6_128_DEFIPm;
            } else {  /* No external 128 bit tcam. */
                *mem = L3_DEFIP_128m;
            }
        } else {  /* IPv6 prefix length < 64 bit. */
            if (soc_mem_index_count(unit, EXT_IPV6_128_DEFIPm)) {
                *mem = EXT_IPV6_128_DEFIPm;
            } else if (soc_mem_index_count(unit, EXT_IPV6_64_DEFIPm)) {
                *mem = EXT_IPV6_64_DEFIPm;
            } else {  /* No external tcam. */
                *mem = L3_DEFIPm;
            }
        }
    } else { /* IPv4 route. */
        if (soc_mem_index_count(unit, EXT_IPV4_DEFIPm)) {
            *mem = EXT_IPV4_DEFIPm;
        } else {  /* No external tcam. */
            *mem = L3_DEFIPm;
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr_l3_enable
 * Purpose:
 *      Resolve route table memory for a given route entry.
 * Parameters:
 *      unit       - (IN)BCM device number.
 *      port       - (IN)Port number.
 *      flags      - (IN)IPv6/IPv4 flag.
 *      enable     - (IN)Enable /Disable 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_l3_enable(int unit, bcm_port_t port, uint32 flags, int enable)
{
    int rv;
    soc_field_t fields[2];
    uint32 values[2] = {1, 0};
    uint32 rval, keygen_type;

    if (!soc_feature(unit, soc_feature_esm_support)) {
        /* No esm on this devices. */
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(READ_ESM_KEYGEN_CTLr(unit, &rval));
    keygen_type = soc_reg_field_get(unit, ESM_KEYGEN_CTLr, rval, TCAM_TYPEf);

    if (flags & BCM_L3_IP6) {
        fields[0] = IPV6_FWD_MODEf;
        fields[1] = IPV6_128_ENf;

        if (enable) {
            
            values[0] = 2; /* L3 forwarding mode */
            /* Enable 128 bit forwarding if 128 bit table is present.*/
            if (soc_mem_index_count(unit, EXT_IPV6_128_DEFIPm)) {
                values[1] = 1;
            }
        } else {
            values[0] = 1; /* use L2 to forward IPv6 packet */
        }
        rv = soc_reg_fields32_modify(unit, ESM_MODE_PER_PORTr, port,
                                     2, fields, values);
    } else {
        fields[0] = IPV4_FWD_MODEf;
        if (enable) {
            
            values[0] = 2; /* L3 forwarding mode */
        } else {
            values[0] = 1; /* use L2 to forward IPv4 packet */
        }
        rv = soc_reg_fields32_modify(unit, ESM_MODE_PER_PORTr, port, 
                                     1, fields, values);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr_l3_clear_hit
 * Purpose:
 *      Clear hit bit on l3 entry
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      mem       - (IN)L3 table memory.
 *      l3cfg     - (IN)l3 entry info. 
 *      l3x_entry - (IN)l3 entry filled hw buffer.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l3_clear_hit(int unit, soc_mem_t mem,
                       _bcm_l3_cfg_t *l3cfg, void *l3x_entry)
{
    uint32 *buf_p;                /* HW buffer address.       */ 
    int mcast;                    /* Entry is multicast flag. */ 
    int ipv6;                     /* Entry is IPv6 flag.      */
    int idx;                      /* Iterator index.          */
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f, HIT_3f };

    /* Input parameters check */
    if ((NULL == l3cfg) || (NULL == l3x_entry)) {
        return (BCM_E_PARAM);
    }

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);
    mcast = (l3cfg->l3c_flags & BCM_L3_IPMC);

    /* Init memory pointers. */ 
    buf_p = (uint32 *)l3x_entry;

    /* If entry was not hit  there is nothing to clear */
    if (!(l3cfg->l3c_flags & BCM_L3_HIT)) {
        return (BCM_E_NONE);
    }

    /* Reset entry hit bit in hw. */
    if (ipv6 && mcast) {
        /* IPV6 multicast entry hit reset. */
        for (idx = 1; idx < 4; idx++) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 0);
        }
    } else if (ipv6 || mcast) {
        /* Reset IPV6 unicast  hit bit. */
        for (idx = 1; idx < 2; idx++) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 0);
        }
    }

    /* Reset hit bit. */
    soc_mem_field32_set(unit, mem, buf_p, hitf[0], 0);

    /* Write entry back to hw. */
    return BCM_XGS3_MEM_WRITE(unit, mem, l3cfg->l3c_hw_index, buf_p);
}

/*
 * Function:
 *      _bcm_tr_l3_ipmc_ent_init
 * Purpose:
 *      Set GROUP/SOURCE/VID/IMPC flag in the entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      l3x_entry - (IN/OUT) IPMC entry to fill. 
 *      l3cfg     - (IN) Api IPMC data. 
 * Returns:
 *    BCM_E_XXX
 */
void
_bcm_tr_l3_ipmc_ent_init(int unit, uint32 *buf_p,
                             _bcm_l3_cfg_t *l3cfg)
{
    soc_mem_t mem;                     /* IPMC table memory.    */
    int ipv6;                          /* IPv6 entry indicator. */
    int idx;                           /* Iteration index.      */
    soc_field_t typef[] = { KEY_TYPE_0f, KEY_TYPE_1f, 
                            KEY_TYPE_2f, KEY_TYPE_3f };
    soc_field_t validf[] = { VALID_0f, VALID_1f, VALID_2f, VALID_3f };

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;

    if (ipv6) {
        /* Set group address. */
        soc_mem_ip6_addr_set(unit, mem, buf_p, GROUP_IP_ADDR_LWR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);
        l3cfg->l3c_ip6[0] = 0x0;    /* Don't write ff entry already mcast. */
        soc_mem_ip6_addr_set(unit, mem, buf_p, GROUP_IP_ADDR_UPR_56f, 
                             l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);
        l3cfg->l3c_ip6[0] = 0xff;    /* Restore The entry  */

        /* Set source  address. */
        soc_mem_ip6_addr_set(unit, mem, buf_p, SOURCE_IP_ADDR_LWR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_LOWER_ONLY);
        soc_mem_ip6_addr_set(unit, mem, buf_p, SOURCE_IP_ADDR_UPR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_UPPER_ONLY);

        for (idx = 0; idx < 4; idx++) {
            /* Set key type */
            soc_mem_field32_set(unit, mem, buf_p, typef[idx], 
                                TR_L3_HASH_KEY_TYPE_V6MC);

            /* Set entry valid bit. */
            soc_mem_field32_set(unit, mem, buf_p, validf[idx], 1);

        }
    } else {
        /* Set group id. */
        soc_mem_field32_set(unit, mem, buf_p, GROUP_IP_ADDRf,
                            l3cfg->l3c_ip_addr);

        /* Set source address. */
        soc_mem_field32_set(unit, mem, buf_p, SOURCE_IP_ADDRf,
                            l3cfg->l3c_src_ip_addr);

        for (idx = 0; idx < 2; idx++) {
            /* Set key type */
            soc_mem_field32_set(unit, mem, buf_p, typef[idx], 
                                TR_L3_HASH_KEY_TYPE_V4MC);

            /* Set entry valid bit. */
            soc_mem_field32_set(unit, mem, buf_p, validf[idx], 1);
        }
    }

    /* Set vlan id. */
    soc_mem_field32_set(unit, mem, buf_p, VLAN_IDf, l3cfg->l3c_vid);

    /* Set virtual router id. */
    if (SOC_MEM_FIELD_VALID(unit, mem, VRF_IDf)) {
        soc_mem_field32_set(unit, mem, buf_p, VRF_IDf, l3cfg->l3c_vrf);
    }
    return;
}

/*
 * Function:
 *      _bcm_tr_l3_ipmc_ent_parse
 * Purpose:
 *      Service routine used to parse hw l3 ipmc entry to api format.
 * Parameters:
 *      unit      - (IN)SOC unit number. 
 *      l3cfg     - (IN/OUT)l3 entry parsing destination buf.
 *      l3x_entry - (IN/OUT)hw buffer.
 * Returns:
 *      void
 */
STATIC INLINE void
_bcm_tr_l3_ipmc_ent_parse(int unit, _bcm_l3_cfg_t *l3cfg,
                          l3_entry_ipv6_multicast_entry_t *l3x_entry)
{
    soc_mem_t mem;                     /* IPMC table memory.    */
    uint32 *buf_p;                     /* HW buffer address.    */
    int ipv6;                          /* IPv6 entry indicator. */
    int idx;                           /* Iteration index.      */
    soc_field_t hitf[] = {HIT_1f, HIT_2f, HIT_3f };

    buf_p = (uint32 *)l3x_entry;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;

    /* Mark entry as multicast & clear rest of the flags. */
    l3cfg->l3c_flags = BCM_L3_IPMC;
    if (ipv6) {
       l3cfg->l3c_flags |= BCM_L3_IP6;
    }

    /* Read hit value. */
    if(soc_mem_field32_get(unit, mem, buf_p, HIT_0f)) { 
        l3cfg->l3c_flags |= BCM_L3_HIT;
    } else if (ipv6) {
        /* Get hit bit. */
        for (idx = 0; idx < 3; idx++) {
            if(soc_mem_field32_get(unit, mem, buf_p, hitf[idx])) { 
                l3cfg->l3c_flags |= BCM_L3_HIT;
                break;
            }
        }
    }

    /* Set ipv6 group address to multicast. */
    if (ipv6) {
        l3cfg->l3c_ip6[0] = 0xff;   /* Set entry ip to mcast address. */
    }

    /* Read priority override */
    if(soc_mem_field32_get(unit, mem, buf_p, RPEf)) { 
        l3cfg->l3c_flags |= BCM_L3_RPE;
    }

    /* Read destination discard bit. */
    if(soc_mem_field32_get(unit, mem, buf_p, DST_DISCARDf)) { 
        l3cfg->l3c_flags |= BCM_L3_DST_DISCARD;
    }

    /* Read Virtual Router Id. */
    l3cfg->l3c_vrf = soc_mem_field32_get(unit, mem, buf_p, VRF_IDf);

    /* Pointer to ipmc replication table. */
    l3cfg->l3c_ipmc_ptr = soc_mem_field32_get(unit, mem, buf_p, L3MC_INDEXf);

    /* Classification lookup class id. */
    l3cfg->l3c_lookup_class = soc_mem_field32_get(unit, mem, buf_p, CLASS_IDf);

    /* Read priority value. */
    l3cfg->l3c_prio = soc_mem_field32_get(unit, mem, buf_p, PRIf);
    return;
}

/*
 * Function:
 *      _bcm_tr_l3_ipmc_get
 * Purpose:
 *      Get l3 multicast entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      l3cfg     - (IN/OUT)Group/Source key & Get result buffer.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_tr_l3_ipmc_get(int unit, _bcm_l3_cfg_t *l3cfg)
{
    l3_entry_ipv6_multicast_entry_t l3x_key;    /* Lookup key buffer.    */
    l3_entry_ipv6_multicast_entry_t l3x_entry;  /* Search result buffer. */
    int clear_hit;                              /* Clear hit flag.       */
    soc_mem_t mem;                              /* IPMC table memory.    */
    int ipv6;                                   /* IPv6 entry indicator. */
    int rv;                                     /* Return value.         */

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;

    /*  Zero buffers. */
    sal_memcpy(&l3x_key, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);
    sal_memcpy(&l3x_entry, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);

    /* Check if clear hit bit is required. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    /* Lookup Key preparation. */
    _bcm_tr_l3_ipmc_ent_init(unit, (uint32 *)&l3x_key, l3cfg);

    /* Perform hw lookup. */
    MEM_LOCK(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l3cfg->l3c_hw_index,
                        &l3x_key, &l3x_entry, 0);
    MEM_UNLOCK(unit, mem);
    BCM_XGS3_LKUP_IF_ERROR_RETURN(rv, BCM_E_NOT_FOUND);

    /* Extract buffer information. */
    _bcm_tr_l3_ipmc_ent_parse(unit, l3cfg, &l3x_entry);

    /* Clear the HIT bit */
    if (clear_hit) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l3_clear_hit(unit, mem, l3cfg, &l3x_entry));
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr_l3_ipmc_add
 * Purpose:
 *      Add l3 multicast entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      l3cfg     - (IN/OUT)Group/Source key.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_tr_l3_ipmc_add(int unit, _bcm_l3_cfg_t *l3cfg)
{
    l3_entry_ipv6_multicast_entry_t l3x_entry;  /* Write entry buffer.   */
    soc_mem_t mem;                              /* IPMC table memory.    */
    uint32 *buf_p;                              /* HW buffer address.    */
    int ipv6;                                   /* IPv6 entry indicator. */
    int idx;                                    /* Iteration index.      */
    int idx_max;                                /* Iteration index max.  */
    int rv;                                     /* Return value.         */
    soc_field_t hitf[] = { HIT_0f, HIT_1f, HIT_2f, HIT_3f };

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;

    /*  Zero buffers. */
    buf_p = (uint32 *)&l3x_entry; 
    sal_memcpy(buf_p, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);

   /* Prepare entry to write. */
    _bcm_tr_l3_ipmc_ent_init(unit, (uint32 *)&l3x_entry, l3cfg);

    /* Set priority override bit. */
    if (l3cfg->l3c_flags & BCM_L3_RPE) {
        soc_mem_field32_set(unit, mem, buf_p, RPEf, 1);
    }

    /* Set destination discard. */
    if (l3cfg->l3c_flags & BCM_L3_DST_DISCARD) {
        soc_mem_field32_set(unit, mem, buf_p, DST_DISCARDf, 1);
    }

    /* Virtual router id. */
    soc_mem_field32_set(unit, mem, buf_p, VRF_IDf, l3cfg->l3c_vrf);

    /* Set priority. */
    soc_mem_field32_set(unit, mem, buf_p, PRIf, l3cfg->l3c_prio);

    /* Pointer to ipmc table. */
    soc_mem_field32_set(unit, mem, buf_p, L3MC_INDEXf, l3cfg->l3c_ipmc_ptr);

    /* Classification lookup class id. */
    soc_mem_field32_set(unit, mem, buf_p, CLASS_IDf, l3cfg->l3c_lookup_class);

    idx_max = (ipv6) ? 4 : 2;
    for (idx = 0; idx < idx_max; idx++) {
        /* Set hit bit. */
        if (l3cfg->l3c_flags & BCM_L3_HIT) {
            soc_mem_field32_set(unit, mem, buf_p, hitf[idx], 1);
        }
    }

    /* Write entry to the hw. */
    MEM_LOCK(unit, mem);
    /* Handle replacement. */
    if (BCM_XGS3_L3_INVALID_INDEX != l3cfg->l3c_hw_index) {
        rv = BCM_XGS3_MEM_WRITE(unit, mem, l3cfg->l3c_hw_index, buf_p);
    } else {
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, (void *)buf_p);
    }

    /* Increment number of ipmc routes. */
    if (BCM_SUCCESS(rv) && (BCM_XGS3_L3_INVALID_INDEX == l3cfg->l3c_hw_index)) {
        (ipv6) ? BCM_XGS3_L3_IP6_IPMC_CNT(unit)++ : \
                 BCM_XGS3_L3_IP4_IPMC_CNT(unit)++;
    }
    MEM_UNLOCK(unit, mem);
    return rv;
}

/*
 * Function:
 *      _bcm_tr_l3_ipmc_del
 * Purpose:
 *      Delete l3 multicast entry.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      l3cfg     - (IN)Group/Source deletion key.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_tr_l3_ipmc_del(int unit, _bcm_l3_cfg_t *l3cfg)
{
    l3_entry_ipv6_multicast_entry_t l3x_entry;  /* Delete buffer.          */
    soc_mem_t mem;                              /* IPMC table memory.      */
    int ipv6;                                   /* IPv6 entry indicator.   */
    int rv;                                     /* Operation return value. */

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;

    /*  Zero entry buffer. */
    sal_memcpy(&l3x_entry, soc_mem_entry_null(unit, mem), 
               soc_mem_entry_words(unit,mem) * 4);


    /* Key preparation. */
    _bcm_tr_l3_ipmc_ent_init(unit, (uint32 *)&l3x_entry, l3cfg);

    /* Delete the entry from hw. */
    MEM_LOCK(unit, mem);

    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void *)&l3x_entry);

    /* Decrement number of ipmc routes. */
    if (BCM_SUCCESS(rv)) {
        (ipv6) ? BCM_XGS3_L3_IP6_IPMC_CNT(unit)-- : \
            BCM_XGS3_L3_IP4_IPMC_CNT(unit)--;
    }
    MEM_UNLOCK(unit, mem);
    return rv;
}

/*
 * Function:
 *      _bcm_tr_l3_ipmc_get_by_idx
 * Purpose:
 *      Get l3 multicast entry by entry index.
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      dma_ptr  - (IN)Table pointer in dma. 
 *      idx       - (IN)Index to read. 
 *      l3cfg     - (IN/OUT)Entry data.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_tr_l3_ipmc_get_by_idx(int unit, void *dma_ptr,
                           int idx, _bcm_l3_cfg_t *l3cfg)
{
    l3_entry_ipv6_multicast_entry_t *l3x_entry_p; /* Read buffer address.    */
    l3_entry_ipv6_multicast_entry_t l3x_entry;    /* Read buffer.            */
    soc_mem_t mem;                                /* IPMC table memory.      */
    uint32 ipv6;                                  /* IPv6 entry indicator.   */
    int clear_hit;                                /* Clear hit bit indicator.*/
    int key_type;

    /* Get entry type. */
    ipv6 = (l3cfg->l3c_flags & BCM_L3_IP6);

    /* Get table memory. */
    mem =  (ipv6) ? L3_ENTRY_IPV6_MULTICASTm : L3_ENTRY_IPV4_MULTICASTm;

    /* Check if clear hit is required. */
    clear_hit = l3cfg->l3c_flags & BCM_L3_HIT_CLEAR;

    if (NULL == dma_ptr) {             /* Read from hardware. */
        /* Zero buffers. */
        l3x_entry_p = &l3x_entry;
        sal_memcpy(l3x_entry_p, soc_mem_entry_null(unit, mem), 
                   soc_mem_entry_words(unit,mem) * 4);

        /* Read entry from hw. */
        BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, mem, idx, l3x_entry_p));
    } else {                    /* Read from dma. */
        l3x_entry_p =
            soc_mem_table_idx_to_pointer(unit, mem,
                                         l3_entry_ipv6_multicast_entry_t *,
                                         dma_ptr, idx);
    }

    /* Ignore invalid entries. */
    if (!soc_mem_field32_get(unit, mem, l3x_entry_p, VALID_0f)) {
        return (BCM_E_NOT_FOUND);
    }


    key_type = soc_mem_field32_get(unit, L3_ENTRY_ONLYm,
                                   l3x_entry_p, KEY_TYPEf);

    switch (key_type) {
      case TR_L3_HASH_KEY_TYPE_V4MC:
          l3cfg->l3c_flags = BCM_L3_IPMC;
          break;
      case TR_L3_HASH_KEY_TYPE_V6UC:
          l3cfg->l3c_flags = BCM_L3_IP6;
          break;
      case TR_L3_HASH_KEY_TYPE_V6MC:
          l3cfg->l3c_flags = BCM_L3_IP6 | BCM_L3_IPMC;
          break;
      default:
          break;
    }

    /* Ignore protocol mismatch & multicast entries. */
    if ((ipv6  != (l3cfg->l3c_flags & BCM_L3_IP6)) ||
        (!(l3cfg->l3c_flags & BCM_L3_IPMC))) {
        return (BCM_E_NONE); 
    }

    /* Set index to l3cfg. */
    l3cfg->l3c_hw_index = idx;

    if (ipv6) {
        /* Get group address. */
        soc_mem_ip6_addr_get(unit, mem, l3x_entry_p, GROUP_IP_ADDR_LWR_64f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_LOWER_ONLY);

        soc_mem_ip6_addr_get(unit, mem, l3x_entry_p, GROUP_IP_ADDR_UPR_56f,
                             l3cfg->l3c_ip6, SOC_MEM_IP6_UPPER_ONLY);

        /* Get source  address. */
        soc_mem_ip6_addr_get(unit, mem, l3x_entry_p, SOURCE_IP_ADDR_LWR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_LOWER_ONLY);
        soc_mem_ip6_addr_get(unit, mem, l3x_entry_p, SOURCE_IP_ADDR_UPR_64f, 
                             l3cfg->l3c_sip6, SOC_MEM_IP6_UPPER_ONLY);

        l3cfg->l3c_ip6[0] = 0xff;    /* Set entry to multicast*/
    } else {
        /* Get group id. */
        l3cfg->l3c_ip_addr =
            soc_mem_field32_get(unit, mem, l3x_entry_p, GROUP_IP_ADDRf);

        /* Get source address. */
        l3cfg->l3c_src_ip_addr =
            soc_mem_field32_get(unit, mem,  l3x_entry_p, SOURCE_IP_ADDRf);
    }

    /* Get vlan id. */
    l3cfg->l3c_vid = soc_mem_field32_get(unit, mem,  l3x_entry_p, VLAN_IDf);

    /* Parse entry data. */
    _bcm_tr_l3_ipmc_ent_parse(unit, l3cfg, l3x_entry_p);

    /* Clear the HIT bit */
    if (clear_hit) {
        BCM_IF_ERROR_RETURN(_bcm_tr_l3_clear_hit(unit, mem, l3cfg, l3x_entry_p));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr_l3_intf_mtu_set
 * Purpose:
 *      Set  L3 interface MTU value. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      intf_info - (IN)Interface information.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_l3_intf_mtu_set(int unit, _bcm_l3_intf_cfg_t *intf_info)
{
    mtu_values_entry_t mtu_value_buf;  /* Buffer to write mtu. */
    uint32  *mtu_value_buf_p;          /* Write buffer address.*/
    void *null_entry = soc_mem_entry_null(unit, L3_MTU_VALUESm);

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    if(!SOC_MEM_FIELD32_VALUE_FIT(unit, L3_MTU_VALUESm, 
                                  MTU_SIZEf, intf_info->l3i_mtu)) {
        return (BCM_E_PARAM);
    }

    if ((intf_info->l3i_vid < soc_mem_index_min(unit, L3_MTU_VALUESm)) || 
        (intf_info->l3i_vid > soc_mem_index_max(unit, L3_MTU_VALUESm))) {
        return (BCM_E_PARAM);
    }

    /* Reset the buffer to default value. */
    mtu_value_buf_p = (uint32 *)&mtu_value_buf;
    sal_memcpy(mtu_value_buf_p, null_entry, sizeof(mtu_values_entry_t));

    if (intf_info->l3i_mtu) {
        /* Set mtu. */
        soc_mem_field32_set(unit, L3_MTU_VALUESm, mtu_value_buf_p, 
                            MTU_SIZEf, intf_info->l3i_mtu);
    }

    return BCM_XGS3_MEM_WRITE(unit, L3_MTU_VALUESm, 
                              intf_info->l3i_vid, mtu_value_buf_p);
}

/*
 * Function:
 *      _bcm_tr_l3_intf_mtu_get
 * Purpose:
 *      Get  L3 interface MTU value. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      intf_info - (IN/OUT)Interface information with updated mtu.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr_l3_intf_mtu_get(int unit, _bcm_l3_intf_cfg_t *intf_info)
{
    mtu_values_entry_t mtu_value_buf;  /* Buffer to write mtu. */
    uint32  *mtu_value_buf_p;          /* Write buffer address.           */

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    if ((intf_info->l3i_vid < soc_mem_index_min(unit, L3_MTU_VALUESm)) || 
        (intf_info->l3i_vid > soc_mem_index_max(unit, L3_MTU_VALUESm))) {
        return (BCM_E_PARAM);
    }
    
    /* Zero the buffer. */
    mtu_value_buf_p = (uint32 *)&mtu_value_buf;
    sal_memset(mtu_value_buf_p, 0, sizeof(mtu_values_entry_t));

    /* Read mtu table entry by index. */
    BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_READ(unit, L3_MTU_VALUESm,
                                          intf_info->l3i_vid, mtu_value_buf_p));
    intf_info->l3i_mtu = 
        soc_mem_field32_get(unit, L3_MTU_VALUESm, mtu_value_buf_p, MTU_SIZEf);

    return (BCM_E_NONE);
}

#else /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
int bcm_esw_triumph_l3_not_empty;
#endif /* BCM_TRIUMPH_SUPPORT && INCLUDE_L3 */
