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
  *      bcm_td2p_share_vp_get
  * Purpose:
  *      Get shared VPs info per VPN
  * Parameters:
  *      unit       - (IN) Device Number
  *      vpn        - (IN) VPN that shared vp belong to
  *      vp_bitmap  - (OUT) shared vp bitmap
  * Returns:
  *      BCM_E_XXX
  */
 int
 bcm_td2p_share_vp_get(int unit, bcm_vpn_t vpn, SHR_BITDCL *vp_bitmap)
 {
     vlan_xlate_entry_t *vent;
     soc_mem_t mem;
     int index_min = 0;
     int index_max = 0;
     uint8 *xlate_buf = NULL;
     int buf_size = 0;
     int i = 0;
     int rv = BCM_E_NONE;
     int vfi = -1;
     int vp = -1;

     if (vpn != BCM_VXLAN_VPN_INVALID) {
         BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
         _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
     }

     mem = VLAN_XLATEm;
     index_min = soc_mem_index_min(unit, mem);
     index_max = soc_mem_index_max(unit, mem);

     buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
     xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
     if (NULL == xlate_buf) {
         return BCM_E_MEMORY;
     }
     rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
     if (rv < 0) {
         soc_cm_sfree(unit, xlate_buf);
         return rv;
     }

     for (i = index_min; i <= index_max; i++) {
         vent = soc_mem_table_idx_to_pointer(unit,
             mem, vlan_xlate_entry_t *, xlate_buf, i);
         if (soc_mem_field32_get(unit, mem, vent, VALIDf) == 0) {
             continue;
         }

         if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
             continue;
         }

         if (vfi != -1 && soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != vfi) {
             continue;
         }

         vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
         SHR_BITSET(vp_bitmap, vp);
     }

     (void)soc_cm_sfree(unit, xlate_buf);

     return BCM_E_NONE;
 }

/*
 * Function:
 *      bcm_td2p_share_vp_delete
 * Purpose:
 *      Delete match criteria of shared VPs
 * Parameters:
 *      unit    - (IN) Device Number
 *      vpn     - (IN) VPN that shared vp belong to
 *      vp      - (IN) Source Virtual Port
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_share_vp_delete(int unit, bcm_vpn_t vpn, int vp)
{
    vlan_xlate_entry_t *vent;
    soc_mem_t mem;
    int index_min = 0;
    int index_max = 0;
    uint8 *xlate_buf = NULL;
    int buf_size = 0;
    int i = 0;
    int rv = BCM_E_NONE;
    int vfi = -1;
    int source_vp = -1;

    if (vpn != BCM_VXLAN_VPN_INVALID) {
        BCM_IF_ERROR_RETURN(_bcm_td2_vxlan_vpn_is_valid(unit, vpn));
        _BCM_VXLAN_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vpn);
    }

    mem = VLAN_XLATEm;
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    buf_size = SOC_MEM_TABLE_BYTES(unit, mem);
    xlate_buf = soc_cm_salloc(unit, buf_size, "VLAN_XLATE buffer");
    if (NULL == xlate_buf) {
        return BCM_E_MEMORY;
    }
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, xlate_buf);
    if (rv < 0) {
        soc_cm_sfree(unit, xlate_buf);
        return rv;
    }

    for (i = index_min; i <= index_max; i++) {
        vent = soc_mem_table_idx_to_pointer(unit,
            mem, vlan_xlate_entry_t *, xlate_buf, i);
        if (soc_mem_field32_get(unit, mem, vent, VALIDf) == 0) {
            continue;
        }

        if (soc_mem_field32_get(unit, mem, vent, XLATE__MPLS_ACTIONf) != 0x1) {
            continue;
        }

        if (vfi != -1 && soc_mem_field32_get(unit, mem, vent, XLATE__VFIf) != vfi) {
            continue;
        }

        source_vp = soc_mem_field32_get(unit, mem, vent, XLATE__SOURCE_VPf);
        if (vp != source_vp) {
            continue;
        }

        bcm_td2_vxlan_port_match_count_adjust(unit, source_vp, -1);
        BCM_IF_ERROR_RETURN(
                soc_mem_delete_index(unit, VLAN_XLATEm, MEM_BLOCK_ALL, i));
    }

    (void)soc_cm_sfree(unit, xlate_buf);

    return BCM_E_NONE;
}
#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3) */
