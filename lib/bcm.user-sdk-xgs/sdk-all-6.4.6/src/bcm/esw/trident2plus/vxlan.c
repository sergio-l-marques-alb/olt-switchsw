/*
 * $Id: $ 
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:    vxlan.c
 * Purpose: Handle trident2plus specific vlan features.
 */
 
#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/td2_td2p.h>

#include <bcm/error.h>
#include <bcm/types.h>

#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident2plus.h>

/*
 * Function:
 *      bcm_td2p_vxlan_dvp_vlan_xlate_key_set
 * Description:
 *      Set egress vlan translation key per VXLAN gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_id         - (IN) vxlan gport 
 *      key_type      - (IN) Hardware egress vlan translation key type.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_td2p_vxlan_dvp_vlan_xlate_key_set(int unit, bcm_gport_t port_id, int key_type)
{
    int vp, egr_key_type = 0, rv = BCM_E_UNAVAIL;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    int network_port = 0;
    source_vp_entry_t svp;
    egr_dvp_attribute_entry_t egr_dvp_attribute;
    
    rv = bcm_td2_vxlan_lock(unit);
    if (BCM_FAILURE(rv)) {
       return rv;
    }

    vp = BCM_GPORT_VXLAN_PORT_ID_GET(port_id);

    /* Be sure the entry is used for VXLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        bcm_td2_vxlan_unlock(unit);
        return BCM_E_NOT_FOUND;
    }

    /* Check for Network-Port */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        bcm_td2_vxlan_unlock(unit);
        return rv;
    }
    network_port = soc_SOURCE_VPm_field32_get(unit, &svp, NETWORK_PORTf);
    /* bcmVlanTranslateEgressKeyVpn only for network port*/
    if ((!network_port) && (key_type == TD2PLUS_DVP_EVLXLT_HASH_KEY_TYPE_VFI)) {
        bcm_td2_vxlan_unlock(unit);
        return BCM_E_UNAVAIL;
    }  
        
    sal_memset(&egr_dvp_attribute, 0, 
               sizeof(egr_dvp_attribute_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    if (BCM_FAILURE(rv)) {
        bcm_td2_vxlan_unlock(unit);
        return rv;
    }
    if (network_port) {
        egr_key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute, 
                                           VXLAN__EVXLT_KEY_SELf);
    } else {
        egr_key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute, 
                                           COMMON__EVXLT_KEY_SELf);        
    }
    if (egr_key_type != key_type) {         
        if (network_port) {
            soc_mem_field32_set(unit, mem, &egr_dvp_attribute, 
                                VXLAN__EVXLT_KEY_SELf, key_type);
        } else {
            soc_mem_field32_set(unit, mem, &egr_dvp_attribute, 
                                COMMON__EVXLT_KEY_SELf, key_type);        
        }        
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, 
                           vp, &egr_dvp_attribute);
        if (BCM_FAILURE(rv)) {
            bcm_td2_vxlan_unlock(unit);
            return rv;
        }            
    }
    bcm_td2_vxlan_unlock(unit);
    
    return rv;
}

/*
 * Function:
 *      bcm_td2p_vxlan_dvp_vlan_xlate_key_get
 * Description:
 *      Get egress vlan translation key per VXLAN gport
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      port_id         - (IN) vxlan gport 
 *      key_type      - (IN) Hardware egress vlan translation key type.
 *
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_td2p_vxlan_dvp_vlan_xlate_key_get(int unit, bcm_gport_t port_id, int *key_type)
{
    int vp, rv = BCM_E_UNAVAIL;
    soc_mem_t mem = EGR_DVP_ATTRIBUTEm;
    int network_port = 0;
    source_vp_entry_t svp;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;

    if (key_type == NULL) {
        return BCM_E_PARAM;
    }
    
    rv = bcm_td2_vxlan_lock(unit);     
    if (BCM_FAILURE(rv)) {
       return rv;
    }
    
    vp = BCM_GPORT_VXLAN_PORT_ID_GET(port_id);

    /* Be sure the entry is used for VXLAN */
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan)) {
        bcm_td2_vxlan_unlock(unit);
        return BCM_E_NOT_FOUND;
    }

    /* Check for Network-Port */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
    if (BCM_FAILURE(rv)) {
        bcm_td2_vxlan_unlock(unit);
        return rv;
    }        
    network_port = soc_SOURCE_VPm_field32_get(unit, &svp, NETWORK_PORTf);
        
    sal_memset(&egr_dvp_attribute, 0, 
               sizeof(egr_dvp_attribute_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    if (BCM_FAILURE(rv)) {
        bcm_td2_vxlan_unlock(unit);
        return rv;
    }
    if (network_port) {
        *key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute, 
                                        VXLAN__EVXLT_KEY_SELf);
    } else {
        *key_type = soc_mem_field32_get(unit, mem, &egr_dvp_attribute, 
                                        COMMON__EVXLT_KEY_SELf);
    }
    bcm_td2_vxlan_unlock(unit);
    
    return rv;
}
#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3) */
