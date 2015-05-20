/*
 * $Id: $ 
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
 * File:    vlan.c
 * Purpose: Handle trident2plus specific vlan features:
 *             Manages VFI VLAN membership tables.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#if defined(BCM_TRIDENT2PLUS_SUPPORT)

#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/ptable.h>
#include <soc/drv.h>
#include <soc/mcm/memacc.h>
#include <bcm/error.h>
#include <bcm/port.h>

#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/ipmc.h>

#if defined(INCLUDE_L3)

/* --------------------------------------------------------
 * Software book keeping for virtual port group information
 * --------------------------------------------------------
 */

typedef struct _bcm_td2p_vp_group_s {
    int vp_count;            /* Number of VPs that belong to this VP group */
    SHR_BITDCL *vp_bitmap;   /* Bitmap of VPs that belong to this VP group */
    SHR_BITDCL *vlan_bitmap; /* VLANs this VP group belongs to */
} _bcm_td2p_vp_group_t;

typedef struct _bcm_td2p_vp_group_bk_s {
    int vp_group_initialized; /* Flag to check initialized status */
    int num_ing_vp_group;     /* Number of ingress VP groups */
    _bcm_td2p_vp_group_t *ing_vp_group_array; /* Ingress VP group array */
    int num_eg_vp_group;      /* Number of egress VP groups */
    _bcm_td2p_vp_group_t *eg_vp_group_array; /* Egress VP group array */
} _bcm_td2p_vp_group_bk_t;

typedef struct {
    int ingress;
    int egress;
} _bcm_td2p_vp_group_unmanaged_t;

STATIC _bcm_td2p_vp_group_bk_t _bcm_td2p_vp_group_bk[BCM_MAX_NUM_UNITS];

STATIC _bcm_td2p_vp_group_unmanaged_t 
        _bcm_td2p_vp_group_unmanaged[BCM_MAX_NUM_UNITS];

#define VP_GROUP_BK(unit) (&_bcm_td2p_vp_group_bk[unit])

#define VP_GROUP_INIT(unit)                              \
    do {                                                 \
        if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) { \
            return BCM_E_UNIT;                           \
        }                                                \
        if (!VP_GROUP_BK(unit)->vp_group_initialized) {  \
            return BCM_E_INIT;                           \
        }                                                \
    } while (0)

#define ING_VP_GROUP(unit, vp_group) \
    (&VP_GROUP_BK(unit)->ing_vp_group_array[vp_group])

#define EG_VP_GROUP(unit, vp_group) \
    (&VP_GROUP_BK(unit)->eg_vp_group_array[vp_group])

#define TD2P_EGR_DVP_ATTRIBUTE_FIELD(_type,_sf) \
        (_type) == 1 ? TRILL__##_sf: \
        (_type) == 2 ? VXLAN__##_sf: \
        (_type) == 3 ? L2GRE__##_sf: \
        COMMON__##_sf;

/*
 * Function:
 *      _bcm_td2p_vp_group_free_resources
 * Purpose:
 *      Free VP group data structures. 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_td2p_vp_group_free_resources(int unit)
{
    int num_vp_groups, i;

    if (VP_GROUP_BK(unit)->ing_vp_group_array) {
        num_vp_groups = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);
        for (i = 0; i < num_vp_groups; i++) {
            if (ING_VP_GROUP(unit, i)->vp_bitmap) {
                sal_free(ING_VP_GROUP(unit, i)->vp_bitmap);
                ING_VP_GROUP(unit, i)->vp_bitmap = NULL;
            }
            if (ING_VP_GROUP(unit, i)->vlan_bitmap) {
                sal_free(ING_VP_GROUP(unit, i)->vlan_bitmap);
                ING_VP_GROUP(unit, i)->vlan_bitmap = NULL;
            }
        }
        sal_free(VP_GROUP_BK(unit)->ing_vp_group_array);
        VP_GROUP_BK(unit)->ing_vp_group_array = NULL;
    }

    if (VP_GROUP_BK(unit)->eg_vp_group_array) {
        num_vp_groups = soc_mem_field_length(unit, EGR_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);
        for (i = 0; i < num_vp_groups; i++) {
            if (EG_VP_GROUP(unit, i)->vp_bitmap) {
                sal_free(EG_VP_GROUP(unit, i)->vp_bitmap);
                EG_VP_GROUP(unit, i)->vp_bitmap = NULL;
            }
            if (EG_VP_GROUP(unit, i)->vlan_bitmap) {
                sal_free(EG_VP_GROUP(unit, i)->vlan_bitmap);
                EG_VP_GROUP(unit, i)->vlan_bitmap = NULL;
            }
        }
        sal_free(VP_GROUP_BK(unit)->eg_vp_group_array);
        VP_GROUP_BK(unit)->eg_vp_group_array = NULL;
    }

    return;
}

/*
 * Function:
 *      _td2p_egr_dvp_attribute_field_name_get
 * Purpose:
 *      Retrieve the proper field name based the device and vp type.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_td2p_egr_dvp_attribute_field_name_get(int unit, 
                         egr_dvp_attribute_entry_t *dvp_entry, 
                         soc_field_t legacy_name,
                         soc_field_t *result_name)
{
    int vp_type;

    vp_type = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, dvp_entry, VP_TYPEf);
    switch (legacy_name) {
        case EN_EFILTERf:
            *result_name = TD2P_EGR_DVP_ATTRIBUTE_FIELD(vp_type, EN_EFILTERf);
            break;
        case VLAN_MEMBERSHIP_PROFILEf:
            *result_name = TD2P_EGR_DVP_ATTRIBUTE_FIELD(vp_type,
                    VLAN_MEMBERSHIP_PROFILEf);
            break;
        default:
            return BCM_E_NOT_FOUND;
            break;
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_td2p_vp_group_reinit
 * Purpose:
 *      Recover the virtual port group data structures.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_reinit(int unit)
{
    int rv = BCM_E_NONE;
    uint8 *source_vp_buf = NULL;
    uint8 *vlan_buf = NULL;
    uint8 *egr_dvp_buf = NULL;
    uint8 *vlan_vfi_buf = NULL;
    uint8 *vlan_mpls_buf = NULL;
    int profile_ptr;
    uint8 *egr_vlan_buf = NULL;
    int index_min, index_max;
    int i, k;
    source_vp_entry_t *svp_entry;
    int vp_group;
    vlan_tab_entry_t *vlan_entry = NULL;
    vlan_mpls_entry_t *vlan_mpls_entry;
    uint32 fldbuf[2];
    egr_dvp_attribute_entry_t *egr_dvp_entry = NULL;
    egr_vlan_entry_t *egr_vlan_entry = NULL;
    ing_vlan_vfi_membership_entry_t *ing_vlan_vfi = NULL;
    egr_vlan_vfi_membership_entry_t *egr_vlan_vfi = NULL;
    soc_field_t en_efilter_f;
    soc_field_t vm_prof_f;

    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership) &&
            !bcm_td2p_ing_vp_group_unmanaged_get(unit)) {

        /* Recover ingress VP group's virtual ports from SOURCE_VP table */

        source_vp_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, SOURCE_VPm), "SOURCE_VP buffer");
        if (NULL == source_vp_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, SOURCE_VPm);
        index_max = soc_mem_index_max(unit, SOURCE_VPm);
        rv = soc_mem_read_range(unit, SOURCE_VPm, MEM_BLOCK_ANY,
                index_min, index_max, source_vp_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            svp_entry = soc_mem_table_idx_to_pointer
                (unit, SOURCE_VPm, source_vp_entry_t *, source_vp_buf, i);

            if (soc_SOURCE_VPm_field32_get(unit, svp_entry,
                        ENABLE_IFILTERf) == 0) {
                continue;
            }

            vp_group = soc_SOURCE_VPm_field32_get(unit, svp_entry,
                    VLAN_MEMBERSHIP_PROFILEf);
            SHR_BITSET(ING_VP_GROUP(unit, vp_group)->vp_bitmap, i);
            ING_VP_GROUP(unit, vp_group)->vp_count++;
        }

        soc_cm_sfree(unit, source_vp_buf);
        source_vp_buf = NULL;

        /* Recover ingress VP group's vlans from VLAN table */

        vlan_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, VLAN_TABm), "VLAN_TAB buffer");
        if (NULL == vlan_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, VLAN_TABm);
        index_max = soc_mem_index_max(unit, VLAN_TABm);
        rv = soc_mem_read_range(unit, VLAN_TABm, MEM_BLOCK_ANY,
                index_min, index_max, vlan_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        vlan_mpls_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, VLAN_MPLSm), "VLAN_MPLS buffer");
        if (NULL == vlan_mpls_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, VLAN_MPLSm);
        index_max = soc_mem_index_max(unit, VLAN_MPLSm);
        rv = soc_mem_read_range(unit, VLAN_MPLSm, MEM_BLOCK_ANY,
                index_min, index_max, vlan_mpls_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        vlan_vfi_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, ING_VLAN_VFI_MEMBERSHIPm), 
                "ING_VLAN_VFI buffer");
        if (NULL == vlan_vfi_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, ING_VLAN_VFI_MEMBERSHIPm);
        index_max = soc_mem_index_max(unit, ING_VLAN_VFI_MEMBERSHIPm);
        rv = soc_mem_read_range(unit, ING_VLAN_VFI_MEMBERSHIPm, 
                MEM_BLOCK_ANY, index_min, index_max, vlan_vfi_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            vlan_entry = soc_mem_table_idx_to_pointer
                (unit, VLAN_TABm, vlan_tab_entry_t *, vlan_buf, i);

            if (soc_VLAN_TABm_field32_get(unit, vlan_entry, VALIDf) == 0) {
                continue;
            }

            if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
               if (soc_VLAN_TABm_field32_get(unit, vlan_entry,
                           VIRTUAL_PORT_ENf) == 0) {
                   continue;
               }
            }

            vlan_mpls_entry = soc_mem_table_idx_to_pointer
                (unit, VLAN_MPLSm, vlan_mpls_entry_t *, vlan_mpls_buf, i);
            profile_ptr = soc_VLAN_MPLSm_field32_get(unit, vlan_mpls_entry,
                    MEMBERSHIP_PROFILE_PTRf);

            ing_vlan_vfi = soc_mem_table_idx_to_pointer(unit, 
                    ING_VLAN_VFI_MEMBERSHIPm, 
                    ing_vlan_vfi_membership_entry_t *,
                    vlan_vfi_buf, profile_ptr);
            soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, ing_vlan_vfi,
                    VP_GROUP_BITMAPf, fldbuf);

            for (k = 0; k < VP_GROUP_BK(unit)->num_ing_vp_group; k++) {
                if (fldbuf[k / 32] & (1 << (k % 32))) {
                    /* The bit in VP_GROUP_BITMAP that corresponds to
                     * VP group k is set.
                     */
                    SHR_BITSET(ING_VP_GROUP(unit, k)->vlan_bitmap, i);
                }
            }
        }

        soc_cm_sfree(unit, vlan_buf);
        soc_cm_sfree(unit, vlan_mpls_buf);
        soc_cm_sfree(unit, vlan_vfi_buf);
        vlan_buf = NULL;
        vlan_mpls_buf = NULL;
        vlan_vfi_buf = NULL;

    }

    if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership) &&
            !bcm_td2p_egr_vp_group_unmanaged_get(unit)) {

        /* Recover egress VP group's virtual ports from
         * EGR_DVP_ATTRIBUTE table
         */

        egr_dvp_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, EGR_DVP_ATTRIBUTEm),
                "EGR_DVP buffer");
        if (NULL == egr_dvp_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, EGR_DVP_ATTRIBUTEm);
        index_max = soc_mem_index_max(unit, EGR_DVP_ATTRIBUTEm);
        rv = soc_mem_read_range(unit, EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ANY,
                index_min, index_max, egr_dvp_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            egr_dvp_entry = soc_mem_table_idx_to_pointer
                (unit, EGR_DVP_ATTRIBUTEm, egr_dvp_attribute_entry_t *,
                 egr_dvp_buf, i);

            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                         egr_dvp_entry, EN_EFILTERf, &en_efilter_f));

            if (soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, egr_dvp_entry,
                        en_efilter_f) == 0) {
                continue;
            }

            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                         egr_dvp_entry, VLAN_MEMBERSHIP_PROFILEf, 
                         &vm_prof_f));

            vp_group = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, egr_dvp_entry,
                    vm_prof_f);
            SHR_BITSET(EG_VP_GROUP(unit, vp_group)->vp_bitmap, i);
            EG_VP_GROUP(unit, vp_group)->vp_count++;
        }

        soc_cm_sfree(unit, egr_dvp_buf);
        egr_dvp_buf = NULL;

        /* Recover egress VP group's vlans from EGR_VLAN table */

        egr_vlan_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, EGR_VLANm), "EGR_VLAN buffer");
        if (NULL == egr_vlan_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, EGR_VLANm);
        index_max = soc_mem_index_max(unit, EGR_VLANm);
        rv = soc_mem_read_range(unit, EGR_VLANm, MEM_BLOCK_ANY,
                index_min, index_max, egr_vlan_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        vlan_vfi_buf = soc_cm_salloc(unit,
                SOC_MEM_TABLE_BYTES(unit, EGR_VLAN_VFI_MEMBERSHIPm), 
                "EGR_VLAN_VFI buffer");
        if (NULL == vlan_vfi_buf) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        index_min = soc_mem_index_min(unit, EGR_VLAN_VFI_MEMBERSHIPm);
        index_max = soc_mem_index_max(unit, EGR_VLAN_VFI_MEMBERSHIPm);
        rv = soc_mem_read_range(unit, EGR_VLAN_VFI_MEMBERSHIPm, 
                MEM_BLOCK_ANY, index_min, index_max, vlan_vfi_buf);
        if (SOC_FAILURE(rv)) {
            goto cleanup;
        }

        for (i = index_min; i <= index_max; i++) {
            egr_vlan_entry = soc_mem_table_idx_to_pointer
                (unit, EGR_VLANm, egr_vlan_entry_t *, egr_vlan_buf, i);

            if (soc_EGR_VLANm_field32_get(unit, egr_vlan_entry, VALIDf) == 0) {
                continue;
            }

            profile_ptr = soc_VLAN_TABm_field32_get(unit, vlan_entry,
                    MEMBERSHIP_PROFILE_PTRf);

            egr_vlan_vfi = soc_mem_table_idx_to_pointer(unit, 
                    EGR_VLAN_VFI_MEMBERSHIPm, 
                    egr_vlan_vfi_membership_entry_t *,
                    vlan_vfi_buf, profile_ptr);
            soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, egr_vlan_vfi,
                        VP_GROUP_BITMAPf, fldbuf);

            for (k = 0; k < VP_GROUP_BK(unit)->num_eg_vp_group; k++) {
                if (fldbuf[k / 32] & (1 << (k % 32))) {
                    /* The bit in VP_GROUP_BITMAP that corresponds to
                     * VP group k is set.
                     */
                    SHR_BITSET(EG_VP_GROUP(unit, k)->vlan_bitmap, i);
                }
            }
        }
        
        soc_cm_sfree(unit, egr_vlan_buf);
        soc_cm_sfree(unit, vlan_vfi_buf);
        egr_vlan_buf = NULL;
        vlan_vfi_buf = NULL;

    }

cleanup:
    if (source_vp_buf) {
        soc_cm_sfree(unit, source_vp_buf);
    }
    if (vlan_buf) {
        soc_cm_sfree(unit, vlan_buf);
    }
    if (vlan_mpls_buf) {
        soc_cm_sfree(unit, vlan_mpls_buf);
    }
    if (egr_dvp_buf) {
        soc_cm_sfree(unit, egr_dvp_buf);
    }
    if (egr_vlan_buf) {
        soc_cm_sfree(unit, egr_vlan_buf);
    }
    if (vlan_vfi_buf) {
        soc_cm_sfree(unit, vlan_vfi_buf);
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     bcm_td2p_vp_group_sw_dump
 * Purpose:
 *     Displays VP group information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_td2p_vp_group_sw_dump(int unit)
{
    int i, k;
    int vp_bitmap_bit_size;
    int num_vlan;

    cli_out("\nSW Information Ingress VP Group - Unit %d\n", unit);
    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        cli_out("\n  Ingress VP Group = %d\n", i);
        cli_out("    VP Count = %d\n",
                ING_VP_GROUP(unit, i)->vp_count);

        cli_out("    VP List =");
        vp_bitmap_bit_size = soc_mem_index_count(unit, SOURCE_VPm);
        for (k = 0; k < vp_bitmap_bit_size; k++) {
            if (SHR_BITGET(ING_VP_GROUP(unit, i)->vp_bitmap, k)) {
                cli_out(" %d", k);
            }
        }
        cli_out("\n");

        cli_out("    VLAN List =");
        num_vlan = soc_mem_index_count(unit, VLAN_TABm);
        for (k = 0; k < num_vlan; k++) {
            if (SHR_BITGET(ING_VP_GROUP(unit, i)->vlan_bitmap, k)) {
                cli_out(" %d", k);
            }
        }
        cli_out("\n");
    }

    cli_out("\nSW Information Egress VP Group - Unit %d\n", unit);
    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        cli_out("\n  Egress VP Group = %d\n", i);
        cli_out("    VP Count = %d\n",
                EG_VP_GROUP(unit, i)->vp_count);

        cli_out("    VP List =");
        vp_bitmap_bit_size = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
        for (k = 0; k < vp_bitmap_bit_size; k++) {
            if (SHR_BITGET(EG_VP_GROUP(unit, i)->vp_bitmap, k)) {
                cli_out(" %d", k);
            }
        }
        cli_out("\n");

        cli_out("    VLAN List =");
        num_vlan = soc_mem_index_count(unit, EGR_VLANm);
        for (k = 0; k < num_vlan; k++) {
            if (SHR_BITGET(EG_VP_GROUP(unit, i)->vlan_bitmap, k)) {
                cli_out(" %d", k);
            }
        }
        cli_out("\n");
    }

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/*
 * Function:
 *      bcm_td2p_vp_group_init
 * Purpose:
 *      Initialize the virtual port group data structures.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_init(int unit)
{
    int num_vp_groups, num_vp, num_vlan, i;
    int rv = BCM_E_NONE;

    _bcm_td2p_vp_group_free_resources(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
    } else
#endif
    {
        _bcm_td2p_vp_group_unmanaged[unit].ingress = FALSE;
        _bcm_td2p_vp_group_unmanaged[unit].egress  = FALSE;
    }
    sal_memset(VP_GROUP_BK(unit), 0, sizeof(_bcm_td2p_vp_group_bk_t));

    num_vp_groups = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm,
            VP_GROUP_BITMAPf);
    VP_GROUP_BK(unit)->num_ing_vp_group = num_vp_groups;

    if (NULL == VP_GROUP_BK(unit)->ing_vp_group_array) {
        VP_GROUP_BK(unit)->ing_vp_group_array = 
            sal_alloc(sizeof(_bcm_td2p_vp_group_t) * num_vp_groups,
                    "ingress vp group array");
        if (NULL == VP_GROUP_BK(unit)->ing_vp_group_array) {
            _bcm_td2p_vp_group_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(VP_GROUP_BK(unit)->ing_vp_group_array, 0,
            sizeof(_bcm_td2p_vp_group_t) * num_vp_groups);

    for (i = 0; i < num_vp_groups; i++) {
        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        if (NULL == ING_VP_GROUP(unit, i)->vp_bitmap) {
            ING_VP_GROUP(unit, i)->vp_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_vp), "ingress vp group vp bitmap");
            if (NULL == ING_VP_GROUP(unit, i)->vp_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(ING_VP_GROUP(unit, i)->vp_bitmap, 0,
                SHR_BITALLOCSIZE(num_vp));

        num_vlan = soc_mem_index_count(unit, VLAN_TABm);
        if (NULL == ING_VP_GROUP(unit, i)->vlan_bitmap) {
            ING_VP_GROUP(unit, i)->vlan_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_vlan), "ingress vp group vlan bitmap");
            if (NULL == ING_VP_GROUP(unit, i)->vlan_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(ING_VP_GROUP(unit, i)->vlan_bitmap, 0,
                SHR_BITALLOCSIZE(num_vlan));
    }

    num_vp_groups = soc_mem_field_length(unit, EGR_VLAN_VFI_MEMBERSHIPm,
            VP_GROUP_BITMAPf);
    VP_GROUP_BK(unit)->num_eg_vp_group = num_vp_groups;

    if (NULL == VP_GROUP_BK(unit)->eg_vp_group_array) {
        VP_GROUP_BK(unit)->eg_vp_group_array = 
            sal_alloc(sizeof(_bcm_td2p_vp_group_t) * num_vp_groups,
                    "egress vp group array");
        if (NULL == VP_GROUP_BK(unit)->eg_vp_group_array) {
            _bcm_td2p_vp_group_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(VP_GROUP_BK(unit)->eg_vp_group_array, 0,
            sizeof(_bcm_td2p_vp_group_t) * num_vp_groups);

    for (i = 0; i < num_vp_groups; i++) {
        num_vp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
        if (NULL == EG_VP_GROUP(unit, i)->vp_bitmap) {
            EG_VP_GROUP(unit, i)->vp_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_vp), "egress vp group vp bitmap");
            if (NULL == EG_VP_GROUP(unit, i)->vp_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(EG_VP_GROUP(unit, i)->vp_bitmap, 0,
                SHR_BITALLOCSIZE(num_vp));

        num_vlan = soc_mem_index_count(unit, EGR_VLANm);
        if (NULL == EG_VP_GROUP(unit, i)->vlan_bitmap) {
            EG_VP_GROUP(unit, i)->vlan_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_vlan), "egress vp group vlan bitmap");
            if (NULL == EG_VP_GROUP(unit, i)->vlan_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(EG_VP_GROUP(unit, i)->vlan_bitmap, 0,
                SHR_BITALLOCSIZE(num_vlan));
    }

    VP_GROUP_BK(unit)->vp_group_initialized = 1;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_td2p_vp_group_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_td2p_vp_group_free_resources(unit);
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_group_detach
 * Purpose:
 *      De-initialize the virtual port group data structures.
 * Parameters:
 *      unit - SOC unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_detach(int unit)
{
    _bcm_td2p_vp_group_free_resources(unit);

    VP_GROUP_BK(unit)->vp_group_initialized = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_phy_port_trunk_is_local
 * Purpose:
 *      Determine if the given physical port or trunk is local.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      gport - (IN) Physical port or trunk GPORT ID.
 *      is_local - (OUT) Indicates if gport is local.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_td2p_phy_port_trunk_is_local(int unit, bcm_gport_t gport, int *is_local)
{
    bcm_trunk_t trunk_id;
    int rv = BCM_E_NONE;
    int local_member_count;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    int id;
    int modid_local;

    *is_local = 0;

    if (BCM_GPORT_IS_TRUNK(gport)) {
        trunk_id = BCM_GPORT_TRUNK_GET(gport);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }
        rv = _bcm_esw_trunk_local_members_get(unit, trunk_id,
                0, NULL, &local_member_count);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }   
        if (local_member_count > 0) {
            *is_local = 1;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, gport, &mod_out, &port_out,
                                    &trunk_id, &id)); 
        if ((trunk_id != -1) || (id != -1)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &modid_local));

        *is_local = modid_local;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td2p_vlan_vp_group_set
 * Purpose:
 *      set/clear the specified VP group in the vlan's vp group bitmap.
 * Parameters:
 *      unit       - (IN) Device Number
 *      vlan_mem         - (IN) VLAN_TABm or EGR_VLANm 
 *      vlan             - (IN) vlan mem entry index 
 *      vp_group         - (IN) the specified VP group 
 *      enable           - (IN) TRUE to set, FALSE to clear  
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If vp_group == -1 and enable == false, clear the vp_group bitmap
 */

int
bcm_td2p_vlan_vp_group_set(
      int unit,           /* unit number */
      soc_mem_t vlan_mem, /* either VLAN_TABm or EGR_VLANm */
      int vlan,           /* index to the above memory table */
      int vp_group,       /* the VP group number */
      int enable          /* TRUE to set, FALSE to clear the VP group */
)
{
    int field_len;
    vlan_tab_entry_t vtab;
    vlan_mpls_entry_t vlan_mpls_entry;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 vp_group_bitmap[2];
    soc_mem_t vlan_vfi_mem;
    uint32 profile_idx;
    void *entry;
    int i;
    int rv;

    sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));
    sal_memset(&vlan_mpls_entry, 0, sizeof(vlan_mpls_entry_t));
    sal_memset(&vlan_vfi_entry, 0, sizeof(ing_vlan_vfi_membership_entry_t));

    if (vlan_mem == VLAN_TABm) {
        vlan_mem = VLAN_MPLSm;
        vlan_vfi_mem = ING_VLAN_VFI_MEMBERSHIPm;
        entry = &vlan_mpls_entry;
    } else {
        vlan_vfi_mem = EGR_VLAN_VFI_MEMBERSHIPm;
        entry = &vtab;
    }

    soc_mem_lock(unit, vlan_mem);
    if ((rv = soc_mem_read(unit, vlan_mem, MEM_BLOCK_ANY, vlan,
                    entry)) != BCM_E_NONE) {
        soc_mem_unlock(unit, vlan_mem);
        return rv;
    }
    soc_mem_field_get(unit, vlan_mem, (uint32 *)entry,
            MEMBERSHIP_PROFILE_PTRf, &profile_idx);
    soc_mem_unlock(unit, vlan_mem);

    /* safety check */
    field_len = soc_mem_field_length(unit, vlan_vfi_mem,
               VP_GROUP_BITMAPf);
    if (vp_group >= 0) {
        if (vp_group >= field_len) {
            return BCM_E_PARAM;
        }
    } else if (enable) {
        return BCM_E_PARAM;
    }

    if (field_len > sizeof(vp_group_bitmap) * 8) {
        return BCM_E_INTERNAL;
    }

    soc_mem_lock(unit, vlan_vfi_mem);
    if ((rv = soc_mem_read(unit, vlan_vfi_mem, MEM_BLOCK_ANY, profile_idx,
                    &vlan_vfi_entry)) != BCM_E_NONE) {
        soc_mem_unlock(unit, vlan_vfi_mem);
        return rv;
    }

    if (vp_group >= 0) {
        soc_mem_field_get(unit, vlan_vfi_mem, (uint32 *)&vlan_vfi_entry,
                        VP_GROUP_BITMAPf, vp_group_bitmap);
        if (enable) {
            vp_group_bitmap[vp_group/32] |= 1 << (vp_group%32);
        } else {
            /* clear the vp_group */
            vp_group_bitmap[vp_group/32] &= ~(1 << (vp_group%32));
        }
    } else {
        /* clear the VP group bitmap for this vlan */
        for (i = 0; i < COUNTOF(vp_group_bitmap); i++) {
            vp_group_bitmap[i] = 0;
        }
    }
    soc_mem_field_set(unit, vlan_vfi_mem, (uint32 *)&vlan_vfi_entry,
                        VP_GROUP_BITMAPf, vp_group_bitmap);
    if ((rv = soc_mem_write(unit, vlan_vfi_mem,
                 MEM_BLOCK_ALL, profile_idx, &vlan_vfi_entry)) != BCM_E_NONE) {
        soc_mem_unlock(unit, vlan_vfi_mem);
        return rv;
    }
    soc_mem_lock(unit, vlan_vfi_mem);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vlan_vp_group_get
 * Purpose:
 *      get the flags indicating the given vp group is set for
 *      ingress or/and egress.
 * Parameters:
 *      unit       - (IN) Device Number
 *      vlan_mem         - (IN) VLAN_TABm or EGR_VLANm 
 *      vlan             - (IN) vlan mem entry index 
 *      vp_group         - (IN) the specified VP group 
 *      enable           - (OUT) usage flag  
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2p_vlan_vp_group_get(
      int unit,           /* unit number */
      soc_mem_t vlan_mem, /* either VLAN_TABm or EGR_VLANm */
      int vlan,           /* index to the above memory table */
      int vp_group,       /* the VP group number */
      int *enable         /* TRUE: the given group is set */
)
{
    int field_len;
    vlan_tab_entry_t vtab;
    vlan_mpls_entry_t vlan_mpls_entry;
    soc_mem_t vlan_vfi_mem;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 vp_group_bitmap[2];
    uint32 profile_idx;
    void *entry;
    int rv;

    sal_memset(&vtab, 0, sizeof(vlan_tab_entry_t));
    sal_memset(&vlan_mpls_entry, 0, sizeof(vlan_mpls_entry_t));
    sal_memset(&vlan_vfi_entry, 0, sizeof(ing_vlan_vfi_membership_entry_t));

    if (vlan_mem == VLAN_TABm) {
        vlan_mem = VLAN_MPLSm;
        vlan_vfi_mem = ING_VLAN_VFI_MEMBERSHIPm;
        entry = &vlan_mpls_entry;
    } else {
        vlan_vfi_mem = EGR_VLAN_VFI_MEMBERSHIPm;
        entry = &vtab;
    }

    if ((rv = soc_mem_read(unit, vlan_mem, MEM_BLOCK_ANY, vlan,
                    entry)) != BCM_E_NONE) {
        return rv;
    }
    soc_mem_field_get(unit, vlan_mem, (uint32 *)entry,
            MEMBERSHIP_PROFILE_PTRf, &profile_idx);

    /* safety check */
    field_len = soc_mem_field_length(unit, vlan_vfi_mem,
               VP_GROUP_BITMAPf);
    if (vp_group >= field_len) {
        return BCM_E_PARAM;
    }

    if (field_len > sizeof(vp_group_bitmap) * 8) {
        return BCM_E_INTERNAL;
    }
    
    *enable = 0;
    if ((rv = soc_mem_read(unit, vlan_vfi_mem, MEM_BLOCK_ANY, profile_idx,
                    &vlan_vfi_entry)) != BCM_E_NONE) {
        return rv;
    }

    soc_mem_field_get(unit, vlan_vfi_mem, (uint32 *)&vlan_vfi_entry,
                        VP_GROUP_BITMAPf, vp_group_bitmap);
    if (vp_group_bitmap[vp_group/32] & (1 << (vp_group % 32))) {
        *enable = TRUE;
    }
        
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_local_ports_get
 * Purpose:
 *      Get the local ports on which the given VP resides.
 * Parameters:
 *      unit       - (IN) Device Number
 *      vp         - (IN) Virtual port number
 *      local_port_max   - (OUT) Size of local_port_array
 *      local_port_array - (OUT) Array of local ports
 *      local_port_count - (OUT) Number of local ports 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If local_port_max = 0 and local_port_array == NULL,
 *      the number of local ports will still be returned in
 *      local_port_count. 
 */

STATIC int
_bcm_td2p_vp_local_ports_get(int unit, int vp, int local_port_max,
        bcm_port_t *local_port_array, int *local_port_count)
{
    ing_dvp_table_entry_t dvp_entry;
    ing_l3_next_hop_entry_t ing_nh;
    uint32 nh_index;
    bcm_trunk_t trunk_id;
    bcm_module_t modid;
    bcm_port_t port;
    int modid_local;

    if (local_port_max < 0) {
        return BCM_E_PARAM;
    }

    if ((local_port_max == 0) && (NULL != local_port_array)) {
        return BCM_E_PARAM;
    }

    if ((local_port_max > 0) && (NULL == local_port_array)) {
        return BCM_E_PARAM;
    }

    if (NULL == local_port_count) {
        return BCM_E_PARAM;
    }

    *local_port_count = 0;

    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
            NEXT_HOP_INDEXf);
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                nh_index, &ing_nh));

    if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
        trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);

        BCM_IF_ERROR_RETURN
            (_bcm_esw_trunk_local_members_get(unit,
                                             trunk_id, 
                                             local_port_max,
                                             local_port_array, 
                                             local_port_count));
    } else {
        modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
        port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);

        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, modid, &modid_local));
        if (TRUE != modid_local) {
            *local_port_count = 0;
            return BCM_E_NONE;
        }

        *local_port_count = 1;
        if (NULL != local_port_array) {
            local_port_array[0] = port;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vlan_vp_group_get_all
 * Purpose:
 *      get the array of gports and flags related to the gport.
 * Parameters:
 *      unit       - (IN) Device Number
 *      vlan       - (IN) vlan mem entry index
 *      array_max  - (IN) max size of the array 
 *      bcm_gport_t *gport_array  - (IN) gport array
 *      flags_array -(IN) flags array related to gports
 *      port_cnt   - (IN/OUT) point to the first available array entry
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td2p_vlan_vp_group_get_all(
      int unit,           /* unit number */
      int vlan,           /* index to the vlan  memory table */
      int array_max,      /* max size of the array */
      bcm_gport_t *gport_array,    /* gport array */
      int *flags_array,            /* flags array related to gports */
      int *port_cnt)      /* point to the first available array entry */
{
    int num_vp_groups;
    int vpg;
    int ing_enable;
    int egr_enable;
    int rv;

    num_vp_groups = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm,
            VP_GROUP_BITMAPf);

    for (vpg = 0; vpg < num_vp_groups; vpg++) {
        ing_enable = 0;
        egr_enable = 0;
        if ((*port_cnt == array_max) && (gport_array || flags_array)) {
            break;
        }
        if (flags_array) {
            flags_array[*port_cnt] = 0;
        }
        if (soc_feature(unit,
                  soc_feature_vp_group_ingress_vlan_membership) &&
                  bcm_td2p_ing_vp_group_unmanaged_get(unit)) {
            rv = bcm_td2p_vlan_vp_group_get(unit,VLAN_TABm,vlan,
                             vpg,&ing_enable);
            BCM_IF_ERROR_RETURN(rv);
            if (ing_enable) {
                if (gport_array) {
                    BCM_GPORT_VP_GROUP_SET(gport_array[*port_cnt],vpg);
                }
                if (flags_array) {
                    flags_array[*port_cnt] = BCM_VLAN_PORT_INGRESS_ONLY;
                }
            }
        }
        if (soc_feature(unit,
                 soc_feature_vp_group_egress_vlan_membership) &&
                 bcm_td2p_egr_vp_group_unmanaged_get(unit)) {
            rv = bcm_td2p_vlan_vp_group_get(unit,EGR_VLANm,vlan,
                             vpg,&egr_enable);
            BCM_IF_ERROR_RETURN(rv);
            if (egr_enable) {
                if (gport_array) {
                    BCM_GPORT_VP_GROUP_SET(gport_array[*port_cnt],vpg);
                }
                if (flags_array) {
                    flags_array[*port_cnt] |= BCM_VLAN_PORT_EGRESS_ONLY;
                }
            }
        }
        if (ing_enable || egr_enable) {
            (*port_cnt)++;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_vlan_bitmap_get
 * Purpose:
 *      Get a bitmap of all the VLANs the given VP belongs to.
 * Parameters:
 *      unit        - (IN) BCM device number
 *      vp_gport    - (IN) VP gport ID 
 *      vlan_bitmap - (OUT) Bitmap of VLANs
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_vlan_bitmap_get(int unit, bcm_gport_t vp_gport,
        SHR_BITDCL *vlan_bitmap)
{
    int rv;
    int vp;
    int mc_type;
    source_vp_entry_t svp_entry;
    int vp_group;
    int num_vlan;
    egr_dvp_attribute_entry_t egr_dvp_entry;
    bcm_port_t local_port;
    int local_port_count;
    uint8 *vlan_tab_buf = NULL;
    int index_min, index_max;
    int if_max, if_count;
    bcm_if_t *if_array = NULL;
    int i, j, k;
    vlan_tab_entry_t *vlan_tab_entry;
    int mc_index_array[3];
    int match_prev_mc_index = FALSE;
    bcm_multicast_t group;
    bcm_gport_t local_gport;
    bcm_if_t encap_id;
    int match = FALSE;

    if (BCM_GPORT_IS_VLAN_PORT(vp_gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_VLAN;
    } else if (BCM_GPORT_IS_NIV_PORT(vp_gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_NIV;
    } else if (BCM_GPORT_IS_EXTENDER_PORT(vp_gport)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(vp_gport);
        mc_type = _BCM_MULTICAST_TYPE_EXTENDER;
    } else {
        return BCM_E_PARAM;
    }

    /* Check if VP already belongs to a VP group.
     * If so, just return the VP group's VLAN bitmap.
     */
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
        SOC_IF_ERROR_RETURN
            (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
        if (soc_SOURCE_VPm_field32_get(unit, &svp_entry, ENABLE_IFILTERf)) {
            vp_group = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                    VLAN_MEMBERSHIP_PROFILEf);
            num_vlan = soc_mem_index_count(unit, VLAN_TABm);
            sal_memcpy(vlan_bitmap, ING_VP_GROUP(unit, vp_group)->vlan_bitmap,
                    SHR_BITALLOCSIZE(num_vlan));
            return BCM_E_NONE;
        }
    }

    if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        soc_field_t en_efilter_f;
        soc_field_t vm_prof_f;
        SOC_IF_ERROR_RETURN
            (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &egr_dvp_entry));

        BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                    &egr_dvp_entry, EN_EFILTERf, &en_efilter_f));

        if (soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &egr_dvp_entry,
                    en_efilter_f)) {
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                        &egr_dvp_entry, VLAN_MEMBERSHIP_PROFILEf, &vm_prof_f));
            vp_group = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &egr_dvp_entry,
                    vm_prof_f);
            num_vlan = soc_mem_index_count(unit, EGR_VLANm);
            sal_memcpy(vlan_bitmap, EG_VP_GROUP(unit, vp_group)->vlan_bitmap,
                    SHR_BITALLOCSIZE(num_vlan));
            return BCM_E_NONE;
        }
    }

    /* VP does not belong to any VP group. Need to derive VLAN bitmap by
     * searching through each VLAN table entry's BC_IDX/UMC_IDX/UUC_IDX
     * multicast groups to see if VP belongs to their VP replication lists.
     */ 

    /* Get one local port on which the VP resides.
     * Even if the VP resides on a trunk group, only one trunk
     * member is needed since all members of a trunk group
     * have the same VP replication list.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td2p_vp_local_ports_get(unit,
                                    vp, 1, &local_port, &local_port_count));
    if (local_port_count == 0) {
        return BCM_E_PORT; 
    }

    vlan_tab_buf = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, VLAN_TABm), "VLAN_TAB buffer");
    if (NULL == vlan_tab_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    index_min = soc_mem_index_min(unit, VLAN_TABm);
    index_max = soc_mem_index_max(unit, VLAN_TABm);
    rv = soc_mem_read_range(unit, VLAN_TABm, MEM_BLOCK_ANY,
            index_min, index_max, vlan_tab_buf);
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    if_max = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    if_array = sal_alloc(sizeof(bcm_if_t) * if_max, "if_array");
    if (NULL == if_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(if_array, 0, sizeof(bcm_if_t) * if_max);

    SHR_BITCLR_RANGE(vlan_bitmap, 0, soc_mem_index_count(unit, VLAN_TABm));

    for (i = index_min; i <= index_max; i++) {
        vlan_tab_entry = soc_mem_table_idx_to_pointer
            (unit, VLAN_TABm, vlan_tab_entry_t *, vlan_tab_buf, i);

        if (0 == soc_VLAN_TABm_field32_get(unit, vlan_tab_entry,
                    VALIDf)) {
            continue;
        }
        
        if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
           if (0 == soc_VLAN_TABm_field32_get(unit, vlan_tab_entry,
                       VIRTUAL_PORT_ENf)) {
               continue;
           }
        }

        mc_index_array[0] = soc_VLAN_TABm_field32_get(unit, vlan_tab_entry,
                BC_IDXf);
        mc_index_array[1] = soc_VLAN_TABm_field32_get(unit, vlan_tab_entry,
                UMC_IDXf);
        mc_index_array[2] = soc_VLAN_TABm_field32_get(unit, vlan_tab_entry,
                UUC_IDXf);

        for (j = 0; j < 3; j++) {

            /* Check if the same mc_index was already searched */
            match_prev_mc_index = FALSE;
            for (k = j - 1; k >= 0; k--) {
                if (mc_index_array[j] == mc_index_array[k]) {
                    match_prev_mc_index = TRUE;
                    break;
                }
            }
            if (match_prev_mc_index) {
                /* continue to next mc_index */
                continue;
            }

            /* Get VP replication list for (mc_index, local_port) */
            rv = bcm_esw_ipmc_egress_intf_get(unit, mc_index_array[j],
                    local_port, if_max, if_array, &if_count);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            /* Get VP's encap_id */
            _BCM_MULTICAST_GROUP_SET(group, mc_type, mc_index_array[j]);
            rv = bcm_esw_port_gport_get(unit, local_port, &local_gport);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }
            if (BCM_GPORT_IS_VLAN_PORT(vp_gport))  {
                rv = bcm_esw_multicast_vlan_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            } else if (BCM_GPORT_IS_NIV_PORT(vp_gport)) {
                rv = bcm_esw_multicast_niv_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            } else {
                rv = bcm_esw_multicast_extender_encap_get(unit, group,
                        local_gport, vp_gport, &encap_id);
            } 


            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

            /* Search for VP's encap_id in if_array */
            match = FALSE;
            for (k = 0; k < if_count; k++) {
                if (encap_id == if_array[k]) {
                    match = TRUE;
                    break;
                }
            }
            if (match) {
                break;
            }
        }

        if (match) {
            /* VP belongs to this VLAN */
            SHR_BITSET(vlan_bitmap, i);
        }
    }

cleanup:
    if (vlan_tab_buf) {
        soc_cm_sfree(unit, vlan_tab_buf);
    }

    if (if_array) {
        sal_free(if_array);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_ing_vp_group_join
 * Purpose:
 *      Assign VP to an ingress VP group.
 * Parameters:
 *      unit        - (IN) BCM device number
 *      vp          - (IN) VP number
 *      vlan_bitmap - (IN) Bitmap of VLANs the VP belongs to.
 *      vp_group    - (OUT) Assigned VP group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_ing_vp_group_join(int unit, int vp,
        SHR_BITDCL *vlan_bitmap, int *vp_group)
{
    int i;
    int num_vlan, vid;
    uint32 profile_idx;
    vlan_mpls_entry_t vlan_mpls_entry;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];

    num_vlan = soc_mem_index_count(unit, VLAN_TABm);

    /* First, check if VP's vlan_bitmap matches any existing ingress
     * VP group's vlan_bitmap. If so, assign the VP to the VP group
     * with the matching vlan_bitmap.
     */
    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        if ((ING_VP_GROUP(unit, i)->vp_count > 0) &&
            SHR_BITEQ_RANGE(vlan_bitmap, ING_VP_GROUP(unit, i)->vlan_bitmap,
                    0, num_vlan)) {
            *vp_group = i;
            SHR_BITSET(ING_VP_GROUP(unit, i)->vp_bitmap, vp);
            ING_VP_GROUP(unit, i)->vp_count++;
            return BCM_E_NONE;
        }
    }

    /* VP's vlan_bitmap does not match any existing VP group's
     * vlan_bitmap. Allocate a new ingress VP group for the VP.
     */
    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        if (0 == ING_VP_GROUP(unit, i)->vp_count) {
            *vp_group = i;
            SHR_BITSET(ING_VP_GROUP(unit, i)->vp_bitmap, vp);
            ING_VP_GROUP(unit, i)->vp_count++;
            sal_memcpy(ING_VP_GROUP(unit, i)->vlan_bitmap, vlan_bitmap,
                    SHR_BITALLOCSIZE(num_vlan));

            /* Update VLAN table's VP_GROUP_BITMAP field */
            for (vid = 0; vid < num_vlan; vid++) {
                if (SHR_BITGET(ING_VP_GROUP(unit, i)->vlan_bitmap, vid)) {
                    SOC_IF_ERROR_RETURN
                        (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vid,
                                         &vlan_mpls_entry));
                    soc_VLAN_MPLSm_field_get(unit, &vlan_mpls_entry,
                            MEMBERSHIP_PROFILE_PTRf, &profile_idx);

                    SOC_IF_ERROR_RETURN
                        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                                                       profile_idx,
                                                       &vlan_vfi_entry));
                    soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit,
                            &vlan_vfi_entry, VP_GROUP_BITMAPf, fldbuf);
                    fldbuf[i / 32] |= (1 << (i % 32));
                    soc_ING_VLAN_VFI_MEMBERSHIPm_field_set(unit,
                            &vlan_vfi_entry, VP_GROUP_BITMAPf, fldbuf);
                    SOC_IF_ERROR_RETURN
                        (WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL,
                                                        profile_idx,
                                                        &vlan_vfi_entry));
                }
            }

            return BCM_E_NONE;
        }
    }

    /* An empty VP group is not available. */
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_td2p_eg_vp_group_join
 * Purpose:
 *      Assign VP to an egress VP group.
 * Parameters:
 *      unit        - (IN) BCM device number
 *      vp          - (IN) VP number
 *      vlan_bitmap - (IN) Bitmap of VLANs the VP belongs to.
 *      vp_group    - (OUT) Assigned VP group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_eg_vp_group_join(int unit, int vp,
        SHR_BITDCL *vlan_bitmap, int *vp_group)
{
    int i;
    int num_vlan, vid;
    uint32 profile_idx;
    egr_vlan_entry_t egr_vlan_entry;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];

    num_vlan = soc_mem_index_count(unit, EGR_VLANm);

    /* First, check if VP's vlan_bitmap matches any existing egress
     * VP group's vlan_bitmap. If so, assign the VP to the VP group
     * with the matching vlan_bitmap.
     */
    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        if ((EG_VP_GROUP(unit, i)->vp_count > 0) &&
            SHR_BITEQ_RANGE(vlan_bitmap, EG_VP_GROUP(unit, i)->vlan_bitmap,
                    0, num_vlan)) {
            *vp_group = i;
            SHR_BITSET(EG_VP_GROUP(unit, i)->vp_bitmap, vp);
            EG_VP_GROUP(unit, i)->vp_count++;
            return BCM_E_NONE;
        }
    }

    /* VP's vlan_bitmap does not match any existing VP group's
     * vlan_bitmap. Allocate a new egress VP group for the VP.
     */
    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        if (0 == EG_VP_GROUP(unit, i)->vp_count) {
            *vp_group = i;
            SHR_BITSET(EG_VP_GROUP(unit, i)->vp_bitmap, vp);
            EG_VP_GROUP(unit, i)->vp_count++;
            sal_memcpy(EG_VP_GROUP(unit, i)->vlan_bitmap, vlan_bitmap,
                    SHR_BITALLOCSIZE(num_vlan));

            /* Update EGR_VLAN table's VP_GROUP_BITMAP field */
            for (vid = 0; vid < num_vlan; vid++) {
                if (SHR_BITGET(EG_VP_GROUP(unit, i)->vlan_bitmap, vid)) {
                    SOC_IF_ERROR_RETURN
                        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vid,
                                        &egr_vlan_entry));
                    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry,
                            MEMBERSHIP_PROFILE_PTRf, &profile_idx);

                    SOC_IF_ERROR_RETURN
                        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                                                       profile_idx,
                                                       &vlan_vfi_entry));
                    soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit,
                            &vlan_vfi_entry, VP_GROUP_BITMAPf, fldbuf);
                    fldbuf[i / 32] |= (1 << (i % 32));
                    soc_EGR_VLAN_VFI_MEMBERSHIPm_field_set(unit,
                            &vlan_vfi_entry, VP_GROUP_BITMAPf, fldbuf);
                    SOC_IF_ERROR_RETURN
                        (WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL,
                                                        profile_idx,
                                                        &vlan_vfi_entry));
                }
            }

            return BCM_E_NONE;
        }
    }

    /* An empty VP group is not available. */
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_td2p_ing_vp_group_leave
 * Purpose:
 *      Remove VP from an ingress VP group.
 * Parameters:
 *      unit     - (IN) BCM device number
 *      vp       - (IN) VP number
 *      vp_group - (IN) VP group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_ing_vp_group_leave(int unit, int vp, int vp_group)
{
    int num_vlan, vid;
    uint32 profile_idx;
    vlan_mpls_entry_t vlan_mpls_entry;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];

    SHR_BITCLR(ING_VP_GROUP(unit, vp_group)->vp_bitmap, vp);
    ING_VP_GROUP(unit, vp_group)->vp_count--;

    if (0 == ING_VP_GROUP(unit, vp_group)->vp_count) {

        /* Clear the bit corresponding to the given VP group in
         * VLAN table's VP_GROUP_BITMAP field
         */
        num_vlan = soc_mem_index_count(unit, VLAN_MPLSm);
        for (vid = 0; vid < num_vlan; vid++) {
            if (SHR_BITGET(ING_VP_GROUP(unit, vp_group)->vlan_bitmap, vid)) {
                SOC_IF_ERROR_RETURN
                    (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vid,
                                     &vlan_mpls_entry));
                soc_VLAN_MPLSm_field_get(unit, &vlan_mpls_entry,
                        MEMBERSHIP_PROFILE_PTRf, &profile_idx);

                SOC_IF_ERROR_RETURN
                    (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                                                   profile_idx,
                                                   &vlan_vfi_entry));
                soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry,
                        VP_GROUP_BITMAPf, fldbuf);
                fldbuf[vp_group / 32] &= ~(1 << (vp_group % 32));
                soc_ING_VLAN_VFI_MEMBERSHIPm_field_set(unit, &vlan_vfi_entry,
                        VP_GROUP_BITMAPf, fldbuf);
                SOC_IF_ERROR_RETURN
                    (WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL,
                                                    profile_idx,
                                                    &vlan_vfi_entry));
            }
        }

        SHR_BITCLR_RANGE(ING_VP_GROUP(unit, vp_group)->vlan_bitmap, 0,
                num_vlan);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_eg_vp_group_leave
 * Purpose:
 *      Remove VP from an egress VP group.
 * Parameters:
 *      unit     - (IN) BCM device number
 *      vp       - (IN) VP number
 *      vp_group - (IN) VP group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_eg_vp_group_leave(int unit, int vp, int vp_group)
{
    int num_vlan, vid;
    uint32 profile_idx;
    egr_vlan_entry_t egr_vlan_entry;
    egr_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];

    SHR_BITCLR(EG_VP_GROUP(unit, vp_group)->vp_bitmap, vp);
    EG_VP_GROUP(unit, vp_group)->vp_count--;

    if (0 == EG_VP_GROUP(unit, vp_group)->vp_count) {

        /* Clear the bit corresponding to the given VP group in
         * EGR_VLAN table's VP_GROUP_BITMAP field
         */
        num_vlan = soc_mem_index_count(unit, EGR_VLANm);
        for (vid = 0; vid < num_vlan; vid++) {
            if (SHR_BITGET(EG_VP_GROUP(unit, vp_group)->vlan_bitmap, vid)) {
                SOC_IF_ERROR_RETURN
                    (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vid,
                                    &egr_vlan_entry));
                soc_EGR_VLANm_field_get(unit, &egr_vlan_entry,
                        MEMBERSHIP_PROFILE_PTRf, &profile_idx);

                SOC_IF_ERROR_RETURN
                    (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY,
                                                   profile_idx,
                                                   &vlan_vfi_entry));
                soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry,
                        VP_GROUP_BITMAPf, fldbuf);
                fldbuf[vp_group / 32] &= ~(1 << (vp_group % 32));
                soc_EGR_VLAN_VFI_MEMBERSHIPm_field_set(unit, &vlan_vfi_entry,
                        VP_GROUP_BITMAPf, fldbuf);
                SOC_IF_ERROR_RETURN
                    (WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL,
                                                    profile_idx,
                                                    &vlan_vfi_entry));
            }
        }

        SHR_BITCLR_RANGE(EG_VP_GROUP(unit, vp_group)->vlan_bitmap, 0,
                num_vlan);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_move
 * Purpose:
 *      Move VP from one ingress VP group to another due to add/remove VP
 *      to/from VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN to/from which VP is added or removed
 *      add  - (IN) If TRUE, VP is added to VLAN, else removed from VLAN.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ing_vp_group_move(int unit, int vp, bcm_vlan_t vlan, int add)
{
    int rv = BCM_E_NONE;
    source_vp_entry_t svp_entry;
    int ifilter_en;
    int old_vp_group, new_vp_group;
    int num_vlan;
    SHR_BITDCL *new_vlan_bitmap = NULL;

    SOC_IF_ERROR_RETURN
        (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
    ifilter_en = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
            ENABLE_IFILTERf);
    if (1 != ifilter_en) {
        return BCM_E_DISABLED;
    }

    old_vp_group = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
            VLAN_MEMBERSHIP_PROFILEf);

    /* Derive VP's new VLAN bitmap by adding/removing VLAN
     * to/from VP group's VLAN bitmap
     */
    num_vlan = soc_mem_index_count(unit, VLAN_TABm);
    new_vlan_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vlan),
            "vlan bitmap");
    if (NULL == new_vlan_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(new_vlan_bitmap, 0, SHR_BITALLOCSIZE(num_vlan));

    sal_memcpy(new_vlan_bitmap,
            ING_VP_GROUP(unit, old_vp_group)->vlan_bitmap, 
            SHR_BITALLOCSIZE(num_vlan));
    if (add) {
        SHR_BITSET(new_vlan_bitmap, vlan);
    } else {
        SHR_BITCLR(new_vlan_bitmap, vlan);
    }

    /* If VP's new VLAN bitmap is not the same as the VP group's
     * VLAN bitmap, move the VP to another VP group
     */
    if (!SHR_BITEQ_RANGE(new_vlan_bitmap,
                ING_VP_GROUP(unit, old_vp_group)->vlan_bitmap,
                0, num_vlan)) {

        /* Join a new VP group */
        rv = _bcm_td2p_ing_vp_group_join(unit, vp, new_vlan_bitmap,
                &new_vp_group);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Leave the old VP group */
        rv = _bcm_td2p_ing_vp_group_leave(unit, vp, old_vp_group);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Update vp group field */
        soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                VLAN_MEMBERSHIP_PROFILEf, new_vp_group);
        rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

cleanup:
    if (new_vlan_bitmap) {
        sal_free(new_vlan_bitmap);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_eg_vp_group_move
 * Purpose:
 *      Move VP from one egress VP group to another due to add/remove VP
 *      to/from VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vp   - (IN) VP number
 *      vlan - (IN) VLAN to/from which VP is added or removed
 *      add  - (IN) If TRUE, VP is added to VLAN, else removed from VLAN.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_eg_vp_group_move(int unit, int vp, bcm_vlan_t vlan, int add)
{
    int rv = BCM_E_NONE;
    egr_dvp_attribute_entry_t dvp_entry;
    int efilter_en;
    int old_vp_group, new_vp_group;
    int num_vlan;
    SHR_BITDCL *new_vlan_bitmap = NULL;
    soc_field_t en_efilter_f;
    soc_field_t vm_prof_f;

    SOC_IF_ERROR_RETURN
        (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                &dvp_entry, EN_EFILTERf, &en_efilter_f));
    efilter_en = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
            en_efilter_f);
    if (1 != efilter_en) {
        return BCM_E_DISABLED;
    }

    BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                &dvp_entry, VLAN_MEMBERSHIP_PROFILEf, &vm_prof_f));
    old_vp_group = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
            vm_prof_f);

    /* Derive VP's new VLAN bitmap by adding/removing VLAN
     * to/from VP group's VLAN bitmap
     */
    num_vlan = soc_mem_index_count(unit, EGR_VLANm);
    new_vlan_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vlan),
            "vlan bitmap");
    if (NULL == new_vlan_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(new_vlan_bitmap, 0, SHR_BITALLOCSIZE(num_vlan));

    sal_memcpy(new_vlan_bitmap,
            EG_VP_GROUP(unit, old_vp_group)->vlan_bitmap, 
            SHR_BITALLOCSIZE(num_vlan));
    if (add) {
        SHR_BITSET(new_vlan_bitmap, vlan);
    } else {
        SHR_BITCLR(new_vlan_bitmap, vlan);
    }

    /* If VP's new VLAN bitmap is not the same as the VP group's
     * VLAN bitmap, move the VP to another VP group
     */
    if (!SHR_BITEQ_RANGE(new_vlan_bitmap,
                EG_VP_GROUP(unit, old_vp_group)->vlan_bitmap,
                0, num_vlan)) {

        /* Join a new VP group */
        rv = _bcm_td2p_eg_vp_group_join(unit, vp, new_vlan_bitmap,
                &new_vp_group);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Leave the old VP group */
        rv = _bcm_td2p_eg_vp_group_leave(unit, vp, old_vp_group);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Update vp group field */
        soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                vm_prof_f, new_vp_group);
        rv = WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp, &dvp_entry);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

cleanup:
    if (new_vlan_bitmap) {
        sal_free(new_vlan_bitmap);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_vlan_get_all
 * Purpose:
 *      Get all VPs from the VP groups belonging to the given VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan - (IN) VLAN ID 
 *      vp_bitmap - (OUT) Bitmap of VPs.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ing_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap)
{
    vlan_mpls_entry_t vlan_mpls_entry;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];
    int i;
    uint32 profile_idx;

    SOC_IF_ERROR_RETURN
        (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vlan, &vlan_mpls_entry));
    soc_VLAN_MPLSm_field_get(unit, &vlan_mpls_entry, MEMBERSHIP_PROFILE_PTRf,
            &profile_idx);

    SOC_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_idx,
                                       &vlan_vfi_entry));
    soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry, 
            VP_GROUP_BITMAPf, fldbuf);
    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        if (fldbuf[i / 32] & (1 << (i % 32))) {
            /* The bit in VP_GROUP_BITMAP that corresponds to
             * VP group i is set. Get all VPs in VP group i.
             */
            SHR_BITOR_RANGE(vp_bitmap, ING_VP_GROUP(unit, i)->vp_bitmap,
                    0, soc_mem_index_count(unit, SOURCE_VPm), vp_bitmap);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_eg_vp_group_vlan_get_all
 * Purpose:
 *      Get all VPs from the VP groups belonging to the given VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan - (IN) VLAN ID 
 *      vp_bitmap - (OUT) Bitmap of VPs.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_eg_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *vp_bitmap)
{
    egr_vlan_entry_t egr_vlan_entry;
    egr_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];
    int i;
    uint32 profile_idx;

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vlan, &egr_vlan_entry));
    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry, MEMBERSHIP_PROFILE_PTRf,
            &profile_idx);

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_idx, 
                                       &vlan_vfi_entry));
    soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry,
            VP_GROUP_BITMAPf, fldbuf);

    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        if (fldbuf[i / 32] & (1 << (i % 32))) {
            /* The bit in VP_GROUP_BITMAP that corresponds to
             * VP group i is set. Get all VPs in VP group i.
             */
            SHR_BITOR_RANGE(vp_bitmap, EG_VP_GROUP(unit, i)->vp_bitmap,
                    0, soc_mem_index_count(unit, SOURCE_VPm), vp_bitmap);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_vlan_delete_all
 * Purpose:
 *      Delete all VP groups from the given VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan - (IN) VLAN ID 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ing_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan)
{
    vlan_mpls_entry_t vlan_mpls_entry;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];
    int i, k;
    uint32 profile_idx;
    int vp_bitmap_bit_size;

    SOC_IF_ERROR_RETURN
        (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vlan, &vlan_mpls_entry));
    soc_VLAN_MPLSm_field_get(unit, &vlan_mpls_entry, MEMBERSHIP_PROFILE_PTRf,
            &profile_idx);

    SOC_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_idx,
                                       &vlan_vfi_entry));
    soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry,
            VP_GROUP_BITMAPf, fldbuf);
    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        if (fldbuf[i / 32] & (1 << (i % 32))) {
            /* The bit in VP_GROUP_BITMAP that corresponds to
             * VP group i is set. Then remove all VPs in VP group i
             * from VLAN.
             */
            vp_bitmap_bit_size = soc_mem_index_count(unit, SOURCE_VPm);
            for (k = 0; k < vp_bitmap_bit_size; k++) {
                if (SHR_BITGET(ING_VP_GROUP(unit, i)->vp_bitmap, k)) {
                    /* VP k belongs to VP group i */
                    BCM_IF_ERROR_RETURN
                        (bcm_td2p_ing_vp_group_move(unit, k, vlan, FALSE));  
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_eg_vp_group_vlan_delete_all
 * Purpose:
 *      Delete all VP groups from the given VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan - (IN) VLAN ID 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_eg_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan)
{
    egr_vlan_entry_t egr_vlan_entry;
    egr_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];
    int i, k;
    uint32 profile_idx;
    int vp_bitmap_bit_size;

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vlan, &egr_vlan_entry));
    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry, MEMBERSHIP_PROFILE_PTRf,
            &profile_idx);

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_idx,
                                       &vlan_vfi_entry));
    soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry,
            VP_GROUP_BITMAPf, fldbuf);
    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        if (fldbuf[i / 32] & (1 << (i % 32))) {
            /* The bit in VP_GROUP_BITMAP that corresponds to
             * VP group i is set. Then remove all VPs in VP group i
             * from VLAN.
             */
            vp_bitmap_bit_size = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
            for (k = 0; k < vp_bitmap_bit_size; k++) {
                if (SHR_BITGET(EG_VP_GROUP(unit, i)->vp_bitmap, k)) {
                    /* VP k belongs to VP group i */
                    BCM_IF_ERROR_RETURN
                        (bcm_td2p_eg_vp_group_move(unit, k, vlan, FALSE));  
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_vlan_member_set
 * Purpose:
 *      Set virtual port ingress and egress filter modes.
 * Parameters:
 *      unit  - (IN) BCM device number
 *      gport - (IN) VP gport ID 
 *      flags - (IN) Ingress and egress filter modes.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_vlan_member_set(int unit, bcm_gport_t gport, uint32 flags)
{
    int rv = BCM_E_NONE;
    int vp;
    bcm_vlan_port_t vlan_vp;
    bcm_gport_t phy_port_trunk;
    int is_local;
    source_vp_entry_t svp_entry;
    egr_dvp_attribute_entry_t dvp_entry;
    int old_ifilter_en, old_efilter_en;
    int old_vp_group, vp_group;
    int num_vlan;
    SHR_BITDCL *vlan_bitmap = NULL;
    soc_field_t   en_efilter_f;
    soc_field_t   vm_prof_f;

    if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) {
        if (!soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
            return BCM_E_UNAVAIL;
        }
    }

    if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
        if (!soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
            return BCM_E_UNAVAIL;
        }
    }

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_vlan_port_t_init(&vlan_vp);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_vp.vlan_port_id, vp);
        BCM_IF_ERROR_RETURN(bcm_tr2_vlan_vp_find(unit, &vlan_vp));
        phy_port_trunk = vlan_vp.port;

    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        bcm_niv_port_t niv_port;

        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_niv_port_t_init(&niv_port);
        BCM_GPORT_NIV_PORT_ID_SET(niv_port.niv_port_id, vp);
        BCM_IF_ERROR_RETURN(bcm_trident_niv_port_get(unit, &niv_port));
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            phy_port_trunk = BCM_GPORT_INVALID;
        } else {
            phy_port_trunk = niv_port.port;
        }
    } else 
    if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        bcm_extender_port_t extender_port;

        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_extender_port_t_init(&extender_port);
        BCM_GPORT_EXTENDER_PORT_ID_SET(extender_port.extender_port_id, vp);
        BCM_IF_ERROR_RETURN(bcm_tr3_extender_port_get(unit, &extender_port));
        phy_port_trunk = extender_port.port;
    } else
    { 
        return BCM_E_PARAM;
    }

    if (phy_port_trunk != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2p_phy_port_trunk_is_local(unit, phy_port_trunk,
                                               &is_local));
        if (!is_local) {
            /* Ingress and egress filter modes can be set only on local VPs */
            return BCM_E_PORT;
        }
    }

    /* Set ingress filter mode */

    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership) &&
       (flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {
        SOC_IF_ERROR_RETURN
            (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
        old_ifilter_en = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                ENABLE_IFILTERf);
        if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) { 
            /* td2 hash table */
            if (0 == old_ifilter_en) {
                /* enable hash table */
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                    ENABLE_IFILTERf, 2);
                SOC_IF_ERROR_RETURN(WRITE_SOURCE_VPm(unit, 
                                MEM_BLOCK_ALL, vp, &svp_entry));
            }
        } else {
            if (2 == old_ifilter_en) {
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        ENABLE_IFILTERf, 0);
                BCM_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));
            }
        }
    } else if (soc_feature(unit,
                soc_feature_vp_group_ingress_vlan_membership)) {

        SOC_IF_ERROR_RETURN
            (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
        old_ifilter_en = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                ENABLE_IFILTERf);
        old_vp_group = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                VLAN_MEMBERSHIP_PROFILEf);

        if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) {
            if (0 == old_ifilter_en) {
                /* Enable ingress filtering */

                /* Get all the VLANs this VP belongs to */
                num_vlan = soc_mem_index_count(unit, VLAN_TABm);
                vlan_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vlan),
                        "vlan bitmap");
                if (NULL == vlan_bitmap) {
                    return BCM_E_MEMORY;
                }
                sal_memset(vlan_bitmap, 0, SHR_BITALLOCSIZE(num_vlan));
                rv = _bcm_td2p_vp_vlan_bitmap_get(unit, gport, vlan_bitmap);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                /* Assign VP to an ingress VP group */ 
                rv = _bcm_td2p_ing_vp_group_join(unit, vp, vlan_bitmap,
                        &vp_group);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                /* Update ingress filter enable and vp group fields */
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        ENABLE_IFILTERf, 1);
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        VLAN_MEMBERSHIP_PROFILEf, vp_group);
                rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                sal_free(vlan_bitmap);
                vlan_bitmap = NULL;
            }
        } else {
            if (1 == old_ifilter_en) {
                /* Disable ingress filtering */

                /* Update ingress filter enable and vp group fields */
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        ENABLE_IFILTERf, 0);
                soc_SOURCE_VPm_field32_set(unit, &svp_entry,
                        VLAN_MEMBERSHIP_PROFILEf, 0);
                BCM_IF_ERROR_RETURN
                    (WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp_entry));

                /* Remove VP from VP group */
                BCM_IF_ERROR_RETURN
                    (_bcm_td2p_ing_vp_group_leave(unit, vp, old_vp_group));
            }
        }
    }

    SOC_IF_ERROR_RETURN
        (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                 &dvp_entry, EN_EFILTERf, &en_efilter_f));
    BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                 &dvp_entry, VLAN_MEMBERSHIP_PROFILEf, &vm_prof_f));

    /* Set egress filter mode */
    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership) &&
       (flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) { 
        old_efilter_en = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, 
                           &dvp_entry, en_efilter_f);
        /* td2 hash table */
        if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
            if (0 == old_efilter_en) {
                /* enable hash table */
                soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                        en_efilter_f, 2);
                BCM_IF_ERROR_RETURN(WRITE_EGR_DVP_ATTRIBUTEm(unit, 
                                  MEM_BLOCK_ALL, vp, &dvp_entry));
            }
        } else {
           if (2 == old_efilter_en) {
                soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                                    en_efilter_f, 0);
                BCM_IF_ERROR_RETURN(WRITE_EGR_DVP_ATTRIBUTEm(unit, 
                                  MEM_BLOCK_ALL, vp, &dvp_entry));
            }
        }
    } else if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        old_efilter_en = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
                                 en_efilter_f);
        old_vp_group = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
                                     vm_prof_f);
        if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
            if (0 == old_efilter_en) {
                /* Enable egress filtering */

                /* Get all the VLANs this VP belongs to */
                num_vlan = soc_mem_index_count(unit, EGR_VLANm);
                vlan_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vlan),
                        "vlan bitmap");
                if (NULL == vlan_bitmap) {
                    return BCM_E_MEMORY;
                }
                sal_memset(vlan_bitmap, 0, SHR_BITALLOCSIZE(num_vlan));
                rv = _bcm_td2p_vp_vlan_bitmap_get(unit, gport, vlan_bitmap);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                /* Assign VP to an egress VP group */ 
                rv = _bcm_td2p_eg_vp_group_join(unit, vp, vlan_bitmap,
                        &vp_group);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                /* Update egress filter enable and vp group fields */
                soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                        en_efilter_f, 1);
                soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                        vm_prof_f, vp_group);
                rv = WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp,
                        &dvp_entry);
                if (BCM_FAILURE(rv)) {
                    goto cleanup;
                }

                sal_free(vlan_bitmap);
                vlan_bitmap = NULL;
            }
        } else {
            if (1 == old_efilter_en) {
                /* Disable egress filtering */

                /* Update egress filter enable and vp group fields */
                soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                        en_efilter_f, 0);
                soc_EGR_DVP_ATTRIBUTEm_field32_set(unit, &dvp_entry,
                        vm_prof_f, 0);
                BCM_IF_ERROR_RETURN
                    (WRITE_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ALL, vp,
                                              &dvp_entry));

                /* Remove VP from VP group */
                BCM_IF_ERROR_RETURN
                    (_bcm_td2p_eg_vp_group_leave(unit, vp, old_vp_group));
            }
        }
    }

cleanup:
    if (vlan_bitmap) {
        sal_free(vlan_bitmap);
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_vlan_member_get
 * Purpose:
 *      Get virtual port ingress and egress filter modes.
 * Parameters:
 *      unit  - (IN) BCM device number
 *      gport - (IN) VP gport id
 *      flags - (OUT) Ingress and egress filter modes.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_vlan_member_get(int unit, bcm_gport_t gport, uint32 *flags)
{
    int vp;
    source_vp_entry_t svp_entry;
    int ifilter_en;
    egr_dvp_attribute_entry_t dvp_entry;
    int efilter_en;
    soc_field_t en_efilter_f;

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
    } else {
        return BCM_E_PARAM;
    }

    *flags = 0;

    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
        SOC_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp,
                    &svp_entry));
        ifilter_en = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                ENABLE_IFILTERf);
        if (ifilter_en == 1) {
            *flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
        }
    }

    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) {
        SOC_IF_ERROR_RETURN(READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp,
                    &svp_entry));
        ifilter_en = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                ENABLE_IFILTERf);
        if (ifilter_en == 2) {
            *flags |= BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP |
                      BCM_PORT_VLAN_MEMBER_INGRESS;
        }
    } 

    if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {
        SOC_IF_ERROR_RETURN
            (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
        BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                 &dvp_entry, EN_EFILTERf, &en_efilter_f));
        efilter_en = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
                en_efilter_f);
        if (efilter_en == 1) {
            *flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
        }
    }
    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {
        SOC_IF_ERROR_RETURN
            (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
        BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                 &dvp_entry, EN_EFILTERf, &en_efilter_f));
        efilter_en = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit, &dvp_entry,
                en_efilter_f);
        if (efilter_en == 2) {
            *flags |= BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP |
                      BCM_PORT_VLAN_MEMBER_EGRESS;
        }
    } 

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_unmanaged_get
 * Purpose:
 *      Check if VP group resource is managed by user. If so, user will be
 *      resposible to assign a VP group to a VP and add the VP group to the
 *      vlan's VP group bitmap in order to establish the vlan membership.
 * Parameters:
 *      unit  - (IN) BCM device number
 * Returns:
 *      TRUE/FALSE
 */
int
bcm_td2p_ing_vp_group_unmanaged_get(int unit)
{
    return _bcm_td2p_vp_group_unmanaged[unit].ingress;
}

int
bcm_td2p_egr_vp_group_unmanaged_get(int unit)
{
    return _bcm_td2p_vp_group_unmanaged[unit].egress;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_unmanaged_set
 * Purpose:
 *      Set whether the VP group resource is managed by user.
 * Parameters:
 *      unit  - (IN) BCM device number
        unmanaged - (IN) TRUE/FALSE
 * Returns: BCM_E_XXX 
 *      
 */
int
bcm_td2p_ing_vp_group_unmanaged_set(int unit, int flag)
{
    int num_vp_groups;
    int vp_count;
    int i;

    /* check if to set the same value */
    if (_bcm_td2p_vp_group_unmanaged[unit].ingress == flag) {
        return BCM_E_NONE;
    }

    /* make sure auto method hasn't kicked in yet */
    if (VP_GROUP_BK(unit)->vp_group_initialized) {
        num_vp_groups = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);

        for (vp_count = 0,i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group;
                i++) {
            if (0 == ING_VP_GROUP(unit, i)->vp_count) {
                vp_count++;
            }
        }

        if (vp_count != num_vp_groups) {
            LOG_WARN(BSL_LS_BCM_VLAN,
                     (BSL_META_U(unit,
                      "Unmanaged mode set fails: VP group auto "
                      "method already started\n")));
            return BCM_E_EXISTS;
        }
    }
    _bcm_td2p_vp_group_unmanaged[unit].ingress = flag;
    return BCM_E_NONE;
}

int
bcm_td2p_egr_vp_group_unmanaged_set(int unit, int flag)
{
    int num_vp_groups;
    int vp_count;
    int i;

    /* check if to set the same value */
    if (_bcm_td2p_vp_group_unmanaged[unit].egress == flag) {
        return BCM_E_NONE;
    }

    /* make sure auto method hasn't kicked in yet */
    if (VP_GROUP_BK(unit)->vp_group_initialized) {
        num_vp_groups = soc_mem_field_length(unit, EGR_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);

        for (vp_count = 0,i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
            if (0 == EG_VP_GROUP(unit, i)->vp_count) {
                vp_count++;
            }
        }

        if (vp_count != num_vp_groups) {
            LOG_WARN(BSL_LS_BCM_VLAN,
                     (BSL_META_U(unit,
                 "Unmanaged mode set fails: VP group auto "
                      "method already started\n")));

            return BCM_E_EXISTS;
        }
    }
    _bcm_td2p_vp_group_unmanaged[unit].egress = flag;
    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *     bcm_td2p_vlan_table_port_get 
 * Purpose:
 *      Read the ING_PORT_BITMAP from a vlan entry.
 */
int
bcm_td2p_vlan_table_port_get(int unit, bcm_vlan_t vid, pbmp_t *ing_pbmp,
        pbmp_t *pbmp)
{
    vlan_mpls_entry_t vlan_mpls;
    egr_vlan_entry_t egr_vlan;
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    egr_vlan_vfi_membership_entry_t egr_vlan_vfi;
    uint32 profile_ptr = 0;
    
    /* Upper layer already checks that vid is valid */
    SOC_IF_ERROR_RETURN
        (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, (int) vid, &vlan_mpls));

    profile_ptr = soc_VLAN_MPLSm_field32_get(unit, &vlan_mpls,
            MEMBERSHIP_PROFILE_PTRf);

    SOC_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_ptr, 
            &ing_vlan_vfi)); 
    soc_mem_pbmp_field_get(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, ing_pbmp);

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, (int) vid, &egr_vlan));

    profile_ptr = soc_EGR_VLANm_field32_get(unit, &egr_vlan,
            MEMBERSHIP_PROFILE_PTRf);

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_ptr, 
            &egr_vlan_vfi)); 
    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, pbmp);


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_update_vlan_pbmp
 * Purpose:
 *      Update VLAN table's port bitmap.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN ID.
 *      pbmp  - (IN) VLAN port bitmap. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_update_vlan_pbmp(int unit, bcm_vlan_t vlan,
        bcm_pbmp_t *pbmp)
{
    int rv = BCM_E_NONE;
    uint32 profile_ptr = 0;
    vlan_tab_entry_t vtab;
    vlan_mpls_entry_t vlan_mpls;
    egr_vlan_entry_t egr_vtab;
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    egr_vlan_vfi_membership_entry_t egr_vlan_vfi;

    soc_mem_lock(unit, VLAN_TABm);
    rv = READ_VLAN_TABm(unit, MEM_BLOCK_ANY, vlan, &vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }
    soc_mem_pbmp_field_set(unit, VLAN_TABm, &vtab, PORT_BITMAPf, pbmp);
    rv = WRITE_VLAN_TABm(unit, MEM_BLOCK_ALL, vlan, &vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }
    soc_mem_unlock(unit, VLAN_TABm);


    rv = READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vlan, &vlan_mpls);
    profile_ptr = soc_VLAN_MPLSm_field32_get(unit, &vlan_mpls,
            MEMBERSHIP_PROFILE_PTRf);

    soc_mem_lock(unit, ING_VLAN_VFI_MEMBERSHIPm);
    rv = READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_ptr, 
            &ing_vlan_vfi); 
    if (rv < 0) {
        soc_mem_unlock(unit, ING_VLAN_VFI_MEMBERSHIPm);
        return rv;
    }
    soc_mem_pbmp_field_set(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, pbmp);
    rv = WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, profile_ptr, 
            &ing_vlan_vfi);
    if (rv < 0) {
        soc_mem_unlock(unit, ING_VLAN_VFI_MEMBERSHIPm);
        return rv;
    }
    soc_mem_unlock(unit, ING_VLAN_VFI_MEMBERSHIPm);


    rv = READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vlan, &egr_vtab);
    profile_ptr = soc_EGR_VLANm_field32_get(unit, &egr_vtab,
            MEMBERSHIP_PROFILE_PTRf);

    soc_mem_lock(unit, EGR_VLAN_VFI_MEMBERSHIPm);
    rv = READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_ptr, 
            &egr_vlan_vfi); 
    if (rv < 0) {
        soc_mem_unlock(unit, EGR_VLAN_VFI_MEMBERSHIPm);
        return rv;
    }
    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, pbmp);
    rv = WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, profile_ptr, 
            &egr_vlan_vfi);
    if (rv < 0) {
        soc_mem_unlock(unit, EGR_VLAN_VFI_MEMBERSHIPm);
        return rv;
    }
    soc_mem_unlock(unit, EGR_VLAN_VFI_MEMBERSHIPm);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vlan_vfi_membership_init
 * Purpose:
 *      Program membership port bitmap in vlan vfi membership tables
 */
int
bcm_td2p_vlan_vfi_membership_init(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    egr_vlan_vfi_membership_entry_t egr_vlan_vfi;

    BCM_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, vid,
                                       &ing_vlan_vfi));
    soc_mem_pbmp_field_set(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &pbmp);
    BCM_IF_ERROR_RETURN
        (WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid,
                                        &ing_vlan_vfi));

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, vid,
                                       &egr_vlan_vfi));
    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &pbmp);
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid,
                                        &egr_vlan_vfi));
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td2p_ing_vlan_vfi_membership_add
 * Purpose:
 *      Program membership port bitmap in vlan vfi membership tables
 */
int
bcm_td2p_ing_vlan_vfi_membership_add(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    pbmp_t      cur_pbmp;

    BCM_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, vid, &ing_vlan_vfi));
    soc_mem_pbmp_field_get(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_OR(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &cur_pbmp);
    BCM_IF_ERROR_RETURN
        (WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid, &ing_vlan_vfi));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ing_vlan_vfi_membership_delete
 * Purpose:
 *      Program membership port bitmap in vlan vfi membership tables
 */
int
bcm_td2p_ing_vlan_vfi_membership_delete(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    pbmp_t      cur_pbmp;

    BCM_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, vid, &ing_vlan_vfi));
    soc_mem_pbmp_field_get(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_REMOVE(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &cur_pbmp);
    BCM_IF_ERROR_RETURN
        (WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid, &ing_vlan_vfi));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_egr_vlan_vfi_membership_add
 * Purpose:
 *      Program membership port bitmap in vlan vfi membership tables
 */
int
bcm_td2p_egr_vlan_vfi_membership_add(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    egr_vlan_vfi_membership_entry_t egr_vlan_vfi;
    pbmp_t      cur_pbmp;

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, vid, &egr_vlan_vfi));
    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_OR(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &cur_pbmp);
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid, &egr_vlan_vfi));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_egr_vlan_vfi_membership_delete
 * Purpose:
 *      Program membership port bitmap in vlan vfi membership tables
 */
int
bcm_td2p_egr_vlan_vfi_membership_delete(int unit, bcm_vlan_t vid, pbmp_t pbmp)
{
    egr_vlan_vfi_membership_entry_t egr_vlan_vfi;
    pbmp_t      cur_pbmp;

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, vid, &egr_vlan_vfi));
    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_REMOVE(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &cur_pbmp);
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid, &egr_vlan_vfi));

    return BCM_E_NONE;
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT*/
