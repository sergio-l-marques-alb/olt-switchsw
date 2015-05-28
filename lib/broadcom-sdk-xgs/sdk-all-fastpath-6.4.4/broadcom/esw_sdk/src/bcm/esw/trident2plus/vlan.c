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
#include <soc/field.h>
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
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/virtual.h>

#define BCMI_MAX_COE_MODULE_ID         255
#define BCMI_NUM_PORTS_PER_COE_MODULE  128

#define BCMI_VP_GROUP_VP_OFFSET    ((BCMI_MAX_COE_MODULE_ID * \
                                      BCMI_NUM_PORTS_PER_COE_MODULE) - 1)

#define BCMI_VP_GROUP_GLP_COUNT    ((BCMI_MAX_COE_MODULE_ID * \
                                      BCMI_NUM_PORTS_PER_COE_MODULE))

#define BCMI_VP_GROUP_GLP_MODID_SHIFT    7
#define BCMI_VP_GROUP_GLP_MODID_MASK     0xff
#define BCMI_VP_GROUP_GLP_PORT_SHIFT     0
#define BCMI_VP_GROUP_GLP_PORT_MASK      0x7f

#define BCMI_VP_GROUP_SUBPORT_MODID_GET(_gport)    \
        (((_gport) >> BCMI_VP_GROUP_GLP_MODID_SHIFT) & \
                    BCMI_VP_GROUP_GLP_MODID_MASK)

#define BCMI_VP_GROUP_SUBPORT_PORT_GET(_gport)     \
        (((_gport) >> BCMI_VP_GROUP_GLP_PORT_SHIFT) & \
                    BCMI_VP_GROUP_GLP_PORT_MASK)

#define VP_FILTER_DISABLE      0
#define VP_FILTER_VP_GRP_BMAP  1
#define VP_FILTER_HASH         2
#define VP_DO_NOT_CHECK        3

#define GPP_FILTER_DISABLE      0
#define GPP_FILTER_ING_PORT     1
#define GPP_FILTER_HASH         2
#define GPP_FILTER_VP_GRP_BMAP  3

#define _BCM_DEFAULT_STP_STATE        3 /* Forward */
#define _BCM_DEFAULT_UNTAGGED_STATE   0

#define _BCM_INVALID_STG        -1
#define _BCM_INVALID_VLAN_VFI   -1
#define _BCM_INVALID_VP_GROUP   -1

STATIC _bcm_vp_group_stp_vp_grp_map_t _bcm_vp_group_stp_vp_grp_map[64] = {
	{ SP_TREE_VP_GRP0f },
	{ SP_TREE_VP_GRP1f },
	{ SP_TREE_VP_GRP2f },
	{ SP_TREE_VP_GRP3f },
	{ SP_TREE_VP_GRP4f },
	{ SP_TREE_VP_GRP5f },
	{ SP_TREE_VP_GRP6f },
	{ SP_TREE_VP_GRP7f },
	{ SP_TREE_VP_GRP8f },
	{ SP_TREE_VP_GRP9f },
	{ SP_TREE_VP_GRP10f },
	{ SP_TREE_VP_GRP11f },
	{ SP_TREE_VP_GRP12f },
	{ SP_TREE_VP_GRP13f },
	{ SP_TREE_VP_GRP14f },
	{ SP_TREE_VP_GRP15f },
	{ SP_TREE_VP_GRP16f },
	{ SP_TREE_VP_GRP17f },
	{ SP_TREE_VP_GRP18f },
	{ SP_TREE_VP_GRP19f },
	{ SP_TREE_VP_GRP20f },
	{ SP_TREE_VP_GRP21f },
	{ SP_TREE_VP_GRP22f },
	{ SP_TREE_VP_GRP23f },
	{ SP_TREE_VP_GRP24f },
	{ SP_TREE_VP_GRP25f },
	{ SP_TREE_VP_GRP26f },
	{ SP_TREE_VP_GRP27f },
	{ SP_TREE_VP_GRP28f },
	{ SP_TREE_VP_GRP29f },
	{ SP_TREE_VP_GRP30f },
	{ SP_TREE_VP_GRP31f },
	{ SP_TREE_VP_GRP32f },
	{ SP_TREE_VP_GRP33f },
	{ SP_TREE_VP_GRP34f },
	{ SP_TREE_VP_GRP35f },
	{ SP_TREE_VP_GRP36f },
	{ SP_TREE_VP_GRP37f },
	{ SP_TREE_VP_GRP38f },
	{ SP_TREE_VP_GRP39f },
	{ SP_TREE_VP_GRP40f },
	{ SP_TREE_VP_GRP41f },
	{ SP_TREE_VP_GRP42f },
	{ SP_TREE_VP_GRP43f },
	{ SP_TREE_VP_GRP44f },
	{ SP_TREE_VP_GRP45f },
	{ SP_TREE_VP_GRP46f },
	{ SP_TREE_VP_GRP47f },
	{ SP_TREE_VP_GRP48f },
	{ SP_TREE_VP_GRP49f },
	{ SP_TREE_VP_GRP50f },
	{ SP_TREE_VP_GRP51f },
	{ SP_TREE_VP_GRP52f },
	{ SP_TREE_VP_GRP53f },
	{ SP_TREE_VP_GRP54f },
	{ SP_TREE_VP_GRP55f },
	{ SP_TREE_VP_GRP56f },
	{ SP_TREE_VP_GRP57f },
	{ SP_TREE_VP_GRP58f },
	{ SP_TREE_VP_GRP59f },
	{ SP_TREE_VP_GRP60f },
	{ SP_TREE_VP_GRP61f },
	{ SP_TREE_VP_GRP62f },
	{ SP_TREE_VP_GRP63f }
};

/* Internal functions */

STATIC int _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(int unit, 
                                              bcm_vlan_t vlan_vfi, 
                                              int egress,
                                              void *entry_data);
STATIC int _bcm_td2p_vp_group_vlan_vfi_profile_idx_get(int unit, 
                                            bcm_vlan_t vlan_vfi, 
                                            int egress, 
                                            uint32 *prof_index);
STATIC int _bcm_td2p_vp_group_vlan_vfi_profile_idx_set(int unit, 
                                            bcm_vlan_t vlan_vfi, 
                                            int egress, 
                                            int prof_index);
STATIC int _bcm_td2p_vp_group_vlan_vfi_size_get(int unit, int egress,
                                                int *num_vlan, int *num_vfi);
STATIC int _bcm_td2p_vp_group_get_free_group(int unit, int egress, int *free_vp_group);

STATIC int bcm_td2p_vp_group_group_num_ifilter_get(int unit,
                                                   _bcm_vp_group_access_type_t access_type,
                                                   int port, int egress, int *vp_group,
                                                   int *ifilter);

STATIC int _bcm_td2p_vp_group_port_vp_group_id_update(int unit, int port, int vp_group,
                                           int egress);
int _bcm_td2p_vp_group_join_with_bitmap(int unit, int port,
                                    SHR_BITDCL *vlan_vfi_bmap,
                                    int egress,
                                    int *vp_group);

STATIC int _bcm_td2p_vp_group_gpp_hw_index_get(int unit, int port,
                                    int *hw_index);
STATIC int _bcm_td2p_vp_group_resolve_port_num(int unit, int port,
                                        int *port_abs_value,
                                        int *port_offset);
STATIC int _bcm_td2p_vp_group_stp_state_set(int unit, int stg, int vp_group, 
                                 int egress, int stp_state);
STATIC int _bcm_td2p_vp_group_leave_all_stg(int unit, int vp_group, int egress);
STATIC int _bcm_td2p_vp_group_leave_all_untagged_bitmap(int unit, int vp_group);
STATIC int _bcm_td2p_vp_group_stp_state_get(int unit, int stg, int vp_group, 
                                 int egress, int *stp_state);
STATIC int _bcm_td2p_vp_group_ut_state_get(int unit, int egr_vlan, int vp_group, 
                                int *ut_state);
STATIC int _bcm_td2p_vp_group_ut_state_set(int unit, int egr_vlan, int vp_group, 
                                int ut_state);
STATIC int _bcm_td2p_vp_group_stg_for_vlan_vfi_get(int unit, int vlan_vfi, int egress,
                                         int *stg);
STATIC int _bcm_td2p_vp_group_find_matching_group(int unit, SHR_BITDCL *vlan_vfi_bmap,
                                           int stp_state, int egress, 
                                           _bcm_vp_group_access_type_t access_type,
                                           int ut_state,
                                           int *vp_group_num);
STATIC int _bcm_td2p_vp_group_port_join(int unit, int port, int egress, int vp_group, 
                                 int ut_state);
STATIC int _bcm_td2p_vp_group_stg_join(int unit, int fp_gpp_vp, int stg, int stp_state,
                                int egress, int vp_group);
STATIC int _bcm_td2p_vp_group_join(int unit, int port, int stg, int stp_state,
                        int egress, _bcm_vp_group_access_type_t access_type,
                        int vp_group, int ut_state);
STATIC int _bcm_td2p_vp_group_stg_new(int unit, int port, int stg, int stp_state,
                           int egress, int old_vp_group, int *new_vp_group);
STATIC int _bcm_td2p_vp_group_set_sw_vlan_vfi(int unit, int vlan_vfi, int old_group_num,
                                   int new_group_num, SHR_BITDCL *vlan_vfi_bmap,
                                   int egress);
STATIC int _bcm_td2p_vp_group_port_new(int unit, int port, int vlan_vfi, int egress,
                            int old_vp_group, SHR_BITDCL *new_vlan_vfi_bmap,
                            int ut_state, int *vp_group);
STATIC int _bcm_td2p_vp_group_new(int unit, int port, int vlan_vfi, int stg, int stp_state,
                        int egress, _bcm_vp_group_access_type_t access_type,
                        int old_vp_group, int *new_vp_group, int ut_state);

STATIC int _bcm_td2p_vp_group_update_group_for_vlan_vfi(int unit, int port, int vlan_vfi, 
                                             int vp_group, int egress,
                                             int leave);
STATIC int _bcm_td2p_vp_group_leave(int unit, int port, int vp_group, 
                             _bcm_vp_group_access_type_t access_type, 
                             int egress);


/* --------------------------------------------------------
 * Software book keeping for virtual port group information
 * --------------------------------------------------------
 */

typedef struct _bcm_td2p_vp_group_s {
    int gpp_vp_count;          /* Number of VPs and GPPs that belong to this VP group */
    int stg_count;             /* Number of STGs referencing this group */
    int untagged_count;        /*  Number of ports that are set to be Untagged, 
                                   applies to Egress only */
    SHR_BITDCL *gpp_vp_bitmap; /* Bitmap of VPs and GPPs that belong to this VP group */
    SHR_BITDCL *vlan_vfi_bitmap;  /* VLANs and VFIs this VP group belongs to */
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

STATIC sal_mutex_t _bcm_vp_group_ing_mutex[BCM_MAX_NUM_UNITS] = {NULL};
STATIC sal_mutex_t _bcm_vp_group_egr_mutex[BCM_MAX_NUM_UNITS] = {NULL};


/* 
 * VP Group resource lock
 */
#define VP_GROUP_ING_LOCK(unit) \
        sal_mutex_take(_bcm_vp_group_ing_mutex[unit], sal_mutex_FOREVER); 

#define VP_GROUP_EGR_LOCK(unit) \
        sal_mutex_take(_bcm_vp_group_egr_mutex[unit], sal_mutex_FOREVER); 

#define VP_GROUP_ING_UNLOCK(unit) \
        sal_mutex_give(_bcm_vp_group_ing_mutex[unit]); 

#define VP_GROUP_EGR_UNLOCK(unit) \
        sal_mutex_give(_bcm_vp_group_egr_mutex[unit]); 

#define VP_GROUP_LOCK_MEMS(unit, egress) \
        if(!egress) { \
            VP_GROUP_ING_LOCK(unit); \
        } else { \
            VP_GROUP_EGR_LOCK(unit); \
        }

#define VP_GROUP_UNLOCK_MEMS(unit, egress) \
        if(!egress) { \
            VP_GROUP_ING_UNLOCK(unit); \
        } else { \
            VP_GROUP_EGR_UNLOCK(unit); \
        }

const char * _bcm_vp_group_access_type_strs[] = {
    "GLP",
    "VP",
    "STG"
};

const char * _bcm_vp_group_port_type_strs[] = {
    "Front-Panel",
    "GLP",
    "VP"
};

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

#define ING_STG_GROUP_DATA(unit, stg) \
    (&VP_GROUP_BK(unit)->ing_vp_group_array[vp_group])

#define TD2P_EGR_DVP_ATTRIBUTE_FIELD(_type,_sf) \
        (_type) == 1 ? TRILL__##_sf: \
        (_type) == 2 ? VXLAN__##_sf: \
        (_type) == 3 ? L2GRE__##_sf: \
        COMMON__##_sf;

#define VP_GROUP_SP_TREE_FIELD(_vp_group) \
        SP_TREE_VP_GRP##_vp_group##f

#define VP_GROUP_CLEANUP(op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) { goto cleanup; } } while(0)

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
            if (ING_VP_GROUP(unit, i)->gpp_vp_bitmap) {
                sal_free(ING_VP_GROUP(unit, i)->gpp_vp_bitmap);
                ING_VP_GROUP(unit, i)->gpp_vp_bitmap = NULL;
            }
            if (ING_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
                sal_free(ING_VP_GROUP(unit, i)->vlan_vfi_bitmap);
                ING_VP_GROUP(unit, i)->vlan_vfi_bitmap = NULL;
            }
        }
        sal_free(VP_GROUP_BK(unit)->ing_vp_group_array);
        VP_GROUP_BK(unit)->ing_vp_group_array = NULL;
    }

    if (_bcm_vp_group_ing_mutex[unit]) {
        sal_mutex_destroy(_bcm_vp_group_ing_mutex[unit]);
        _bcm_vp_group_ing_mutex[unit] = NULL;
    }

    if (VP_GROUP_BK(unit)->eg_vp_group_array) {
        num_vp_groups = soc_mem_field_length(unit, EGR_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);
        for (i = 0; i < num_vp_groups; i++) {
            if (EG_VP_GROUP(unit, i)->gpp_vp_bitmap) {
                sal_free(EG_VP_GROUP(unit, i)->gpp_vp_bitmap);
                EG_VP_GROUP(unit, i)->gpp_vp_bitmap = NULL;
            }
            if (EG_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
                sal_free(EG_VP_GROUP(unit, i)->vlan_vfi_bitmap);
                EG_VP_GROUP(unit, i)->vlan_vfi_bitmap = NULL;
            }
        }
        sal_free(VP_GROUP_BK(unit)->eg_vp_group_array);
        VP_GROUP_BK(unit)->eg_vp_group_array = NULL;
    }

    if (_bcm_vp_group_egr_mutex[unit]) {
        sal_mutex_destroy(_bcm_vp_group_egr_mutex[unit]);
        _bcm_vp_group_egr_mutex[unit] = NULL;
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
            SHR_BITSET(ING_VP_GROUP(unit, vp_group)->gpp_vp_bitmap, i);
            ING_VP_GROUP(unit, vp_group)->gpp_vp_count++;
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
                    SHR_BITSET(ING_VP_GROUP(unit, k)->vlan_vfi_bitmap, i);
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
            SHR_BITSET(EG_VP_GROUP(unit, vp_group)->gpp_vp_bitmap, i);
            EG_VP_GROUP(unit, vp_group)->gpp_vp_count++;
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

            profile_ptr = soc_EGR_VLANm_field32_get(unit, egr_vlan_entry,
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
                    SHR_BITSET(EG_VP_GROUP(unit, k)->vlan_vfi_bitmap, i);
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
    int gpp_gpp_vp_bitmap_bit_size;
    int num_vlan, num_vfi, num_gpp;

    cli_out("\nSW Information Ingress VP Group - Unit %d\n", unit);

    _bcm_td2p_vp_group_vlan_vfi_size_get(unit, 0 /* Ingress */, 
                                      &num_vlan, &num_vfi);

    num_gpp = BCMI_MAX_COE_MODULE_ID * BCMI_NUM_PORTS_PER_COE_MODULE;
    gpp_gpp_vp_bitmap_bit_size = soc_mem_index_count(unit, SOURCE_VPm) +
            num_gpp;

    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        cli_out("\n  Ingress GPP/VP Group = %d\n", i);
        cli_out("    GPP/VP Count = %d\n",
                ING_VP_GROUP(unit, i)->gpp_vp_count);
        cli_out("    STG Count = %d\n",
                ING_VP_GROUP(unit, i)->stg_count);
        cli_out("    Untagged Count = %d\n",
                ING_VP_GROUP(unit, i)->untagged_count);

        cli_out("    GPP/VP List =");

        for (k = 0; k < gpp_gpp_vp_bitmap_bit_size; k++) {
            if (SHR_BITGET(ING_VP_GROUP(unit, i)->gpp_vp_bitmap, k)) {

                if(k < num_gpp) {
                    cli_out(" GPP (mod,port) (%d,%d)", (k % BCMI_MAX_COE_MODULE_ID),
                            (k - (k % BCMI_MAX_COE_MODULE_ID)));
                } else {
                    cli_out(" VP %d", k);
                }
            }
        }
        cli_out("\n");

        cli_out("    VLAN/VFI List =");

        for (k = 0; k < (num_vlan + num_vfi); k++) {
            if (SHR_BITGET(ING_VP_GROUP(unit, i)->vlan_vfi_bitmap, k)) {
                if(k <= BCM_VLAN_MAX) {
                    cli_out(" vlan %d", k);
                } else {
                    cli_out(" vfi %d", k);
                }
            }
        }
        cli_out("\n");
    }

    cli_out("\nSW Information Egress VP Group - Unit %d\n", unit);

    _bcm_td2p_vp_group_vlan_vfi_size_get(unit, 1 /* Egress */, 
                                      &num_vlan, &num_vfi);

    gpp_gpp_vp_bitmap_bit_size = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm) +
        num_gpp;

    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        cli_out("\n  Egress VP Group = %d\n", i);
        cli_out("    GPP/VP Count = %d\n",
                EG_VP_GROUP(unit, i)->gpp_vp_count);
        cli_out("    STG Count = %d\n",
                EG_VP_GROUP(unit, i)->stg_count);
        cli_out("    Untagged Count = %d\n",
                EG_VP_GROUP(unit, i)->untagged_count);

        cli_out("    GPP/VP List =");

        for (k = 0; k < gpp_gpp_vp_bitmap_bit_size; k++) {
            if (SHR_BITGET(EG_VP_GROUP(unit, i)->gpp_vp_bitmap, k)) {

                if(k < num_gpp) {
                    cli_out(" GPP (mod,port) (%d,%d)", (k % BCMI_MAX_COE_MODULE_ID),
                            (k - (k % BCMI_MAX_COE_MODULE_ID)));
                } else {
                    cli_out(" VP %d", k);
                }
            }
        }
        cli_out("\n");

        cli_out("    VLAN List =");
        num_vlan = soc_mem_index_count(unit, EGR_VLANm);
        for (k = 0; k < num_vlan; k++) {
            if (SHR_BITGET(EG_VP_GROUP(unit, i)->vlan_vfi_bitmap, k)) {
                if(k <= BCM_VLAN_MAX) {
                    cli_out(" vlan %d", k);
                } else {
                    cli_out(" vfi %d", k);
                }
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
    int num_vp_groups = 0, num_vp = 0; 
    int num_gpp = 0, num_vlan = 0, num_vfi = 0, i = 0;
    int rv = BCM_E_NONE;
    uint32 reg_value = 0;

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

    SOC_IF_ERROR_RETURN
        (_bcm_td2p_vp_group_vlan_vfi_size_get(unit, 0 /* egress */, 
                                              &num_vlan, &num_vfi));

    num_gpp =  BCMI_MAX_COE_MODULE_ID * BCMI_NUM_PORTS_PER_COE_MODULE;
    num_vp   =  soc_mem_index_count(unit, SOURCE_VPm);

    for (i = 0; i < num_vp_groups; i++) {
        if (NULL == ING_VP_GROUP(unit, i)->gpp_vp_bitmap) {
            ING_VP_GROUP(unit, i)->gpp_vp_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_gpp + num_vp),
           "ingress vp group vp gpp bitmap");
            if (NULL == ING_VP_GROUP(unit, i)->gpp_vp_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }
        sal_memset(ING_VP_GROUP(unit, i)->gpp_vp_bitmap, 0,
                   (SHR_BITALLOCSIZE(num_gpp + num_vp)));

        if (NULL == ING_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
            ING_VP_GROUP(unit, i)->vlan_vfi_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_vlan + num_vfi), 
                  "ingress vp group vlan vfi bitmap");
            if (NULL == ING_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }

        sal_memset(ING_VP_GROUP(unit, i)->vlan_vfi_bitmap, 0,
                         SHR_BITALLOCSIZE(num_vlan + num_vfi));
    }

    /* Enable the STG checks on Ingress VP Groups */
    soc_reg_field_set(unit, VP_GROUP_CHECK_ENABLEr, &reg_value,
                      STG_ENABLEf, 1);
    BCM_IF_ERROR_RETURN(WRITE_VP_GROUP_CHECK_ENABLEr(unit, reg_value));
    
    if (NULL == _bcm_vp_group_ing_mutex[unit]) {
        _bcm_vp_group_ing_mutex[unit] = sal_mutex_create("vp group ing mutex");
        if (_bcm_vp_group_ing_mutex[unit] == NULL) {
            _bcm_td2p_vp_group_free_resources(unit);
            return BCM_E_MEMORY;
        }
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

    SOC_IF_ERROR_RETURN
        (_bcm_td2p_vp_group_vlan_vfi_size_get(unit, 1 /* egress */, 
                                              &num_vlan, &num_vfi));

    num_vp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);

    for (i = 0; i < num_vp_groups; i++) {
        if (NULL == EG_VP_GROUP(unit, i)->gpp_vp_bitmap) {
            EG_VP_GROUP(unit, i)->gpp_vp_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_gpp + num_vp),
                 "egress vp group vp gpp bitmap");
            if (NULL == EG_VP_GROUP(unit, i)->gpp_vp_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }

        sal_memset(EG_VP_GROUP(unit, i)->gpp_vp_bitmap, 0,
                   (SHR_BITALLOCSIZE(num_gpp + num_vp)));

        if (NULL == EG_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
            EG_VP_GROUP(unit, i)->vlan_vfi_bitmap = sal_alloc
                (SHR_BITALLOCSIZE(num_vlan + num_vfi),
                 "egress vp group vlan vfi bitmap");
            if (NULL == EG_VP_GROUP(unit, i)->vlan_vfi_bitmap) {
                _bcm_td2p_vp_group_free_resources(unit);
                return BCM_E_MEMORY;
            }
        }

        sal_memset(EG_VP_GROUP(unit, i)->vlan_vfi_bitmap, 0,
                SHR_BITALLOCSIZE(num_vlan + num_vfi));
    }

    /* Enables the STG checks and UT deletion on Egress VP Groups */
    reg_value = 0;
    soc_reg_field_set(unit, EGR_VP_GROUP_CHECK_ENABLEr, &reg_value,
            STG_ENABLEf, 1);
    soc_reg_field_set(unit, EGR_VP_GROUP_CHECK_ENABLEr, &reg_value,
            UNTAG_ENABLEf, 1);
    BCM_IF_ERROR_RETURN(WRITE_EGR_VP_GROUP_CHECK_ENABLEr(unit, reg_value)); 

    if (NULL == _bcm_vp_group_egr_mutex[unit]) {
        _bcm_vp_group_egr_mutex[unit] = sal_mutex_create("vp group egr mutex");
        if (_bcm_vp_group_egr_mutex[unit] == NULL) {
            _bcm_td2p_vp_group_free_resources(unit);
            return BCM_E_MEMORY;
        }
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
    int local_member_count;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    int id;
    int modid_local;
    int rv = BCM_E_NONE;

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

    entry = &vlan_vfi_entry;
    if(vlan_mem == VLAN_MPLSm) {
        rv = _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vlan, 
                                                      BCMI_VP_GROUP_INGRESS,
                                                      &entry);
    } else {
        rv = _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vlan, 
                                                      BCMI_VP_GROUP_EGRESS,
                                                      &entry); 
    }

    soc_mem_unlock(unit, vlan_vfi_mem);

    return rv;
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
 *      _bcm_td2p_vp_vlan_vfi_bitmap_get
 * Purpose:
 *      Get a bitmap of all the VLANs the given VP belongs to.
 * Parameters:
 *      unit        - (IN) BCM device number
 *      vp_gport    - (IN) VP gport ID 
 *      vlan_vfi_bitmap - (OUT) Bitmap of VLANs
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_vlan_vfi_bitmap_get(int unit, bcm_gport_t vp_gport,
        SHR_BITDCL *vlan_vfi_bitmap)
{
    int rv;
    int vp;
    int mc_type;
    source_vp_entry_t svp_entry;
    int vp_group;
    int num_vlan, num_vfi;
    egr_dvp_attribute_entry_t egr_dvp_entry;
    bcm_port_t local_port;
    int local_port_count;
    uint8 *vlan_tab_buf = NULL;
    int index_min, index_max;
    int if_max, if_count = 0;
    bcm_if_t *if_array = NULL;
    int i, j, k;
    vlan_tab_entry_t *vlan_tab_entry = NULL;
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

    if(vlan_vfi_bitmap == NULL) {
        return BCM_E_PARAM;
    }

    /* Get the size of VLAN and VFI tables and allocate memory */
    _bcm_td2p_vp_group_vlan_vfi_size_get(unit, 0, &num_vlan, &num_vfi);

    /* Check if VP already belongs to a VP group.
     * If so, just return the VP group's VLAN bitmap.
     */
    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {
        SOC_IF_ERROR_RETURN
            (READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp_entry));
        if (soc_SOURCE_VPm_field32_get(unit, &svp_entry, ENABLE_IFILTERf)) {
            vp_group = soc_SOURCE_VPm_field32_get(unit, &svp_entry,
                    VLAN_MEMBERSHIP_PROFILEf);
            sal_memcpy(vlan_vfi_bitmap, 
                       ING_VP_GROUP(unit, vp_group)->vlan_vfi_bitmap,
                       SHR_BITALLOCSIZE(num_vlan + num_vfi));
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
            sal_memcpy(vlan_vfi_bitmap, EG_VP_GROUP(unit, vp_group)->vlan_vfi_bitmap,
                    SHR_BITALLOCSIZE(num_vlan + num_vfi));
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

    SHR_BITCLR_RANGE(vlan_vfi_bitmap, 0, (num_vlan + num_vfi));

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

#if defined(INCLUDE_L3)

            /* Get VP replication list for (mc_index, local_port) */
            rv = bcm_esw_ipmc_egress_intf_get(unit, mc_index_array[j],
                    local_port, if_max, if_array, &if_count);
            if (SOC_FAILURE(rv)) {
                goto cleanup;
            }

#endif
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
            SHR_BITSET(vlan_vfi_bitmap, i);
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
 *      _bcm_td2p_vp_group_vlan_vfi_profile_entry_set
 * Purpose:
 *      Given a VLAN/VFI value and a direction (Igr/Egr), set the
 *      data associated with a profile entry and update the VLAN/VFI
 *      with a new profile index
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi - (IN) VLAN/VFI value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      entry_data - (IN) Entry to write
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_vlan_vfi_profile_entry_set(int unit, 
                                              bcm_vlan_t vlan_vfi, 
                                              int egress,
                                              void *entry_data)
{
    uint32 cur_prof_index = 0, new_prof_index = 0;
    int32 cur_idx_ref_count = 0, new_idx_ref_count = 0;

    /* Get the profile_idx associated with the VLAN/VFI */
    _bcm_td2p_vp_group_vlan_vfi_profile_idx_get(unit, vlan_vfi, egress, 
                                                &cur_prof_index);

    /* Gather the ref counts */
    if(!egress) {
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_vfi_mbrship_profile_ref_count(unit, cur_prof_index,
                                                     BCMI_VP_GROUP_INGRESS,
                                                     &cur_idx_ref_count));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_vfi_mbrship_profile_ref_count(unit, cur_prof_index,
                                                         BCMI_VP_GROUP_EGRESS,
                                                         &cur_idx_ref_count));
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"\n\n!:Enter!Before profile: vlan_vfi: %d egress %d Old index %d "
                         "old_ref_cnt %d \n"), 
                         vlan_vfi, egress, cur_prof_index, cur_idx_ref_count));

    /* Depending on the direction, update the  relevant profile index,
       if the old and new are different, delete the old one */
    if (!egress) {
        _bcm_vlan_vfi_memebershipship_profile_entry_op(unit, &entry_data, 1, 
                                               BCMI_VP_GROUP_ADD, 
                                               BCMI_VP_GROUP_INGRESS,
                                               (uint32 *)&new_prof_index);

            /*
             * If the old-profile-index entry is non-zero delete it, note that if the 
             * same index is reallocated, the profile-mgmt would have incremented the 
             * ref-count anyway.
             */
            if(cur_prof_index != 0) {
                _bcm_vlan_vfi_memebershipship_profile_entry_op(unit, NULL, 1, 
                                                      BCMI_VP_GROUP_DELETE, 
                                                      BCMI_VP_GROUP_INGRESS,
                                                      &cur_prof_index);
            }
    } else {
        _bcm_vlan_vfi_memebershipship_profile_entry_op(unit, &entry_data, 1, 
                                               BCMI_VP_GROUP_ADD, 
                                               BCMI_VP_GROUP_EGRESS,
                                               (uint32 *)&new_prof_index);

        /*
         * If the old-profile-index entry is non-zero delete it, note that if the 
         * same index is reallocated, the profile-mgmt would have incremented the 
         * ref-count anyway.
         */
        if(cur_prof_index != 0) {
            _bcm_vlan_vfi_memebershipship_profile_entry_op(unit, NULL, 1, 
                                                   BCMI_VP_GROUP_DELETE, 
                                                   BCMI_VP_GROUP_EGRESS,
                                                   &cur_prof_index);

        }
   }

    /* Write the new index into the VALN/VFI tables */
    _bcm_td2p_vp_group_vlan_vfi_profile_idx_set(unit, vlan_vfi, 
                                                egress, new_prof_index);

    /* Gather the ref counts */
    if(!egress) {
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_vfi_mbrship_profile_ref_count(unit, cur_prof_index,
                                                     BCMI_VP_GROUP_INGRESS, 
                                                     &cur_idx_ref_count));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_vfi_mbrship_profile_ref_count(unit, new_prof_index,
                                                     BCMI_VP_GROUP_INGRESS,
                                                     &new_idx_ref_count));

    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_vfi_mbrship_profile_ref_count(unit, cur_prof_index,
                                                     BCMI_VP_GROUP_EGRESS,
                                                     &cur_idx_ref_count));
        BCM_IF_ERROR_RETURN
            (_bcm_vlan_vfi_mbrship_profile_ref_count(unit,new_prof_index,
                                                     BCMI_VP_GROUP_EGRESS, 
                                                     &new_idx_ref_count));
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: After profile: egress %d old_index %d "
                         "old_ref_cnt %d new_index %d "
                         "new_ref_count: %d \n"),
                        egress, cur_prof_index, cur_idx_ref_count, 
                        new_prof_index, new_idx_ref_count));

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: Old index %d New index %d egress %d "
                         "vlan-vfi %d Data0 0x%x Data1 0x%x "
                         "Data2 0x%x Data3 0x%x Data4 0x%x Data5 0x%x Exit!\n\n\n"), 
                         cur_prof_index, new_prof_index, egress, vlan_vfi,
                         ((egr_vlan_vfi_membership_entry_t*)entry_data)->entry_data[5],
                         ((egr_vlan_vfi_membership_entry_t*)entry_data)->entry_data[4],
                         ((egr_vlan_vfi_membership_entry_t*)entry_data)->entry_data[3],
                         ((egr_vlan_vfi_membership_entry_t*)entry_data)->entry_data[2],
                         ((egr_vlan_vfi_membership_entry_t*)entry_data)->entry_data[1],
                         ((egr_vlan_vfi_membership_entry_t*)entry_data)->entry_data[0]));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_get_vlan_vfi_profile_idx
 * Purpose:
 *      Given a VLAN/VFI value and a direction (Igr/Egr), get the
 *      VLAN/VFI membership profile.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi   - (IN) VLAN/VFI in question
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      prof_index - (OUT) The associated profile index if all goes well
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_vlan_vfi_profile_idx_get(int unit, 
                                            bcm_vlan_t vlan_vfi, 
                                            int egress, 
                                            uint32 *prof_index)
{
    int                 rv = BCM_E_NONE;
    uint32              *entry = NULL, index;
    soc_mem_t           mem;

    if(prof_index == NULL) {
        return BCM_E_PARAM;
    }

    /* Depending on VLAN/VFI, set relevant memory and index */
    if (!_BCM_VPN_VFI_IS_SET(vlan_vfi)) {
        
        if (!egress) {
            mem = VLAN_MPLSm;
        } else {
            mem = EGR_VLANm;
        }
        /* Index in case of a VLAN is same as what is passed in */
        index = vlan_vfi;
    } else {
        
        if (!egress) {
            mem = VFIm;
        } else {
            mem = EGR_VFIm;
        }
        /* Index in case of a VFI is the encoded value */
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vfi);
    }

    /* Allocated the desired size */
    entry = sal_alloc(SOC_MEM_BYTES(unit, mem), "Get VLAN/VFI profile index");

    /* Read the memory and extract the field in question */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry);

    /* On error cleanup and return */
    VP_GROUP_CLEANUP(rv);

    /* Assign the profile index */
    *prof_index = soc_mem_field32_get(unit, mem, entry, MEMBERSHIP_PROFILE_PTRf);

cleanup:
    if (entry) {
        sal_free(entry);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_set_vlan_vfi_profile_idx
 * Purpose:
 *      Given a VLAN/VFI value, direction (Igr/Egr) and prof_index set
 *      the associated VLAN_VFI profile.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi   - (IN) VLAN/VFI value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      prof_index - (OUT) The associated profile index if all goes well
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_vlan_vfi_profile_idx_set(int unit, 
                                            bcm_vlan_t vlan_vfi, 
                                            int egress, 
                                            int prof_index)
{
    int                 index = -1;
    soc_mem_t           mem;

    /* Depending on VLAN/VFI, write to relevant memories */
    if (!_BCM_VPN_VFI_IS_SET(vlan_vfi)) {
        if (!egress) {
            mem = VLAN_MPLSm;
        } else {
            mem = EGR_VLANm;
        }

        index = vlan_vfi;
    } else {
        if (!egress) {
            mem = VFIm;
        } else {
            mem = EGR_VFIm;
        }

        /* Index in case of a VFI is the encoded value */
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vfi);
    }

#if !defined(SOC_NO_NAMES)
    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: mem: %s index: %d prof_index: %d\n"),
                        soc_mem_name[mem], index, prof_index));
#endif

    SOC_IF_ERROR_RETURN(
        soc_mem_field32_modify(unit, mem, index,
                   MEMBERSHIP_PROFILE_PTRf, prof_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_vlan_vfi_size_get
 * Purpose:
 *      Given a direction (Igr/Egr), get the VLAN/VFI table sizes.
 * Parameters:
 *      unit - (IN) BCM device number
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      num_vlan - (OUT) The depth of the ING/EGR VLAN Table
 *      num_vfi -  (OUT) The depth of the ING/EGR VFI Table
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_vlan_vfi_size_get(int unit, int egress, int *num_vlan, int *num_vfi)
{
    if (!egress) {
        *num_vlan = soc_mem_index_count(unit, VLAN_TABm);
        *num_vfi = soc_mem_index_count(unit, VFIm);
    } else {
        *num_vlan = soc_mem_index_count(unit, EGR_VLANm);
        *num_vfi = soc_mem_index_count(unit, EGR_VFIm);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_get_free_group
 * Purpose:
 *      Find a free unused VP_Group, given the direction (Igr/Egr).
 * Parameters:
 *      unit - (IN) BCM device number
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      free_vp_group - (OUT) A free VP-Group, if there is one available 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_get_free_group(int unit, int egress, int *free_vp_group)
{
    int  i, num_grps;
    _bcm_td2p_vp_group_t   *vp_grp_array = NULL;

    *free_vp_group = -1;

    /* Get the data pertaining to Ingress/Egress */
    if(!egress) {
        num_grps = VP_GROUP_BK(unit)->num_ing_vp_group;
        vp_grp_array = ING_VP_GROUP(unit, 0);
    } else {
        num_grps = VP_GROUP_BK(unit)->num_eg_vp_group;
        vp_grp_array = EG_VP_GROUP(unit, 0);
    }

    for (i = 0; i < num_grps; i++) {
        if ((0 == vp_grp_array->gpp_vp_count) && 
            (0 == vp_grp_array->stg_count) &&
            (0 == vp_grp_array->untagged_count)) {
            *free_vp_group = i;
            return BCM_E_NONE;
        }

        /* Move to the next group */ 
        vp_grp_array++;
    }

    /* If we end up here, no free VP groups are found */
    return BCM_E_FULL;
}

/*
 * Function:
 *      bcm_td2p_vp_group_port_move
 * Purpose:
 *      Move GPP/VP from one VP group to another due to add/remove
 *      of the GPP/VP to/from VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      gport   - (IN) Gpp/VP port value
 *      vlan_vfi - (IN) VLAN_VFI to/from which VP is added or removed
 *      add  - (IN) If TRUE, VP is added to VLAN, else removed from VLAN.
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      flags - (IN) Flags value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_port_move(int unit, int gport, bcm_vlan_t vlan_vfi, 
                            int add, int egress, int flags)
{
    int rv = BCM_E_NONE, is_vfi = 0, index = 0;
    int ifilter_en = 0;
    int old_vp_group = 0, vp_grp_num = 0, new_vp_group = 0;
    int num_vlan, num_vfi;
    SHR_BITDCL *new_vlan_vfi_bitmap = NULL;
    _bcm_vp_group_access_type_t access_type;

    VP_GROUP_LOCK_MEMS(unit, egress);

    /* Determine the correct access type based on params */
    if(BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        access_type = _bcmVpGrpGlp;
    } else {
        access_type = _bcmVpGrpVp;
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"Enter !: port 0x%x vlan_vfi: %d add: %d "
                         "egress: %d access_type: %s flags: 0x%x \n\n"), 
                          gport, vlan_vfi, add, egress, 
                          _bcm_vp_group_access_type_strs[access_type],
                           flags));

    /* Get the current vp group associated with this port */
    rv = bcm_td2p_vp_group_group_num_ifilter_get(unit, access_type, gport,
                                            egress, &old_vp_group,
                                            &ifilter_en);

    VP_GROUP_CLEANUP(rv);

    /* Get the current vp_group/ifilter associated with this port, if the
       FILTER value is not vp_group, reject config */
    if(access_type == _bcmVpGrpVp) {

        if (VP_FILTER_VP_GRP_BMAP != ifilter_en) {
            rv = BCM_E_DISABLED;

            LOG_INFO(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,"!: VP_GRP filter method not in "
                                 "use ... leave %d \n"), 
                                  ifilter_en));
            goto cleanup;
        }
    } else if(access_type == _bcmVpGrpGlp) {

        if (GPP_FILTER_VP_GRP_BMAP != ifilter_en) {
            rv = BCM_E_DISABLED;

            LOG_INFO(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,"!: VP_GRP filter method not in "
                                 "use ... leave %d \n"), 
                                  ifilter_en));
            goto cleanup;
        }
    } 

    /* Derive VP's new VLAN bitmap by adding/removing VLAN
     * to/from VP group's VLAN bitmap
     */

    /* Get the VLAN/VFI table sizes */
    rv = _bcm_td2p_vp_group_vlan_vfi_size_get(unit, egress, 
                                              &num_vlan, &num_vfi);

    VP_GROUP_CLEANUP(rv);

    new_vlan_vfi_bitmap = sal_alloc(SHR_BITALLOCSIZE(num_vlan + num_vfi), 
                                        "vlan bitmap");
    if (NULL == new_vlan_vfi_bitmap) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    sal_memset(new_vlan_vfi_bitmap, 0, SHR_BITALLOCSIZE(num_vlan + num_vfi));

    /* Get the current vlan_vfi bitmap associated with this old_group */
    if (!egress) {
        sal_memcpy(new_vlan_vfi_bitmap, 
                ING_VP_GROUP(unit, old_vp_group)->vlan_vfi_bitmap,
                SHR_BITALLOCSIZE(num_vlan + num_vfi));
    } else {
        sal_memcpy(new_vlan_vfi_bitmap, 
                EG_VP_GROUP(unit, old_vp_group)->vlan_vfi_bitmap,
                SHR_BITALLOCSIZE(num_vlan + num_vfi));
    }

    /* Based on if it is a 'ADD' or not, update the bitmap */
    /* Check if this is a VFI and get the encoded index */
    if(_BCM_VPN_VFI_IS_SET(vlan_vfi)) {
         is_vfi = 1;
        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vfi);
    } else {
        index = vlan_vfi;
    }

    /* Set or clear the VLAN/VFI */
    if (add) {
        SHR_BITSET(new_vlan_vfi_bitmap,
                    (!is_vfi ? index : index + (BCM_VLAN_MAX + 1)));
    } else {
        SHR_BITCLR(new_vlan_vfi_bitmap, 
                    (!is_vfi ? index : index + (BCM_VLAN_MAX + 1)));
    }

    /* Based on Ingress/Egress, compare the relevant bitmap */
    if (!egress) {
        if (SHR_BITEQ_RANGE(new_vlan_vfi_bitmap,
                    ING_VP_GROUP(unit, old_vp_group)->vlan_vfi_bitmap,
                    0, (num_vlan + num_vfi))) {
            /* The VLAN bitmaps are the same, no vp-grp movement needed */
            goto cleanup;
        }
    } else {

        /* Get the UNTAG bit for this EGR_VLAN */
        int ut_state = 0;
        _bcm_td2p_vp_group_ut_state_get(unit, vlan_vfi, old_vp_group, &ut_state);

        /* Compare the existing vlan/vfi bitmaps and the ut_state
           on this group */
        if (SHR_BITEQ_RANGE(new_vlan_vfi_bitmap,
                    EG_VP_GROUP(unit, old_vp_group)->vlan_vfi_bitmap,
                    0, (num_vlan + num_vfi))  &&
            ((flags & BCM_VLAN_GPORT_ADD_UNTAGGED) & ut_state)) {
            /* The VLAN bitmaps and the ut_flags are the same,
               no vp-grp movement needed */
            goto cleanup;
        }
    }

    /* Try and find a group we can re-use */
    rv = _bcm_td2p_vp_group_find_matching_group(unit, new_vlan_vfi_bitmap, 
                                      _BCM_DEFAULT_STP_STATE, egress,
                                      access_type, 
                                     (flags & BCM_VLAN_GPORT_ADD_UNTAGGED),
                                       &vp_grp_num);

    VP_GROUP_CLEANUP(rv);

    if(vp_grp_num != -1) {

        LOG_INFO(BSL_LS_BCM_VLAN,
            (BSL_META_U(unit,"!: Matching group: %d found\n\n"), 
                              vp_grp_num));
        /* Join the group */
        rv = _bcm_td2p_vp_group_join(unit, gport, _BCM_INVALID_STG, 
                                     _BCM_DEFAULT_STP_STATE, 
                                     egress, access_type, vp_grp_num,
                                     (flags & BCM_VLAN_GPORT_ADD_UNTAGGED));

        VP_GROUP_CLEANUP(rv);

        /* Now leave the previous group */
        rv = _bcm_td2p_vp_group_leave(unit, gport, old_vp_group, 
                                      access_type, egress);

        goto cleanup;
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: NO matching group, adding new\n")));

    /* If we end up here, it means we have NOT found an existing vp_group
       to join, we need to create a new one */
    rv = _bcm_td2p_vp_group_new(unit, gport, vlan_vfi, _BCM_INVALID_STG, 
                                _BCM_DEFAULT_STP_STATE, 
                                egress, access_type, old_vp_group,
                                &new_vp_group,
                                (flags & BCM_VLAN_GPORT_ADD_UNTAGGED));

    VP_GROUP_CLEANUP(rv);

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: New group: %d Old_group: %d \n\n"), 
                          new_vp_group, old_vp_group));

    /* Now leave the previous group, if different from the old one */
    if (old_vp_group != new_vp_group) {
        rv = _bcm_td2p_vp_group_leave(unit, gport, old_vp_group, 
                                      access_type, egress);
    }

cleanup:
    if (new_vlan_vfi_bitmap) {
        sal_free(new_vlan_vfi_bitmap);
        new_vlan_vfi_bitmap = NULL;
    }

    VP_GROUP_UNLOCK_MEMS(unit, egress);

    return rv;
}

#if 0 

<<---- Relevant ???? NOPE, I dont think!!!

/*
 * Function:
 *      _bcm_td2p_vp_group_find_group_matching_stp_state
 * Purpose:
 *      For a given vlan_vfi bmap, all relevant STG's and find a matching group
 *      for the 'stp_state' passed in
 * Parameters:
 *      unit - (IN) BCM device number
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress.
 *      num_vlan - (OUT) The depth of the ING/EGR VLAN Table
 *      num_vfi -  (OUT) The depth of the ING/EGR VFI Table
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2p_vp_group_find_group_matching_stp_state(int unit, int egress, 
                                                 int stp_state, 
                                                 SHR_BITDCL *vlan_vfi_bitmap)
{
    _SHR_PBMP_ITER

}

#endif

/*
 * Function:
 *      _bcm_td2p_vp_group_port_vp_group_id_update
 * Purpose:
 *      For a given port, update its vp_group id into relevant memory
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port in question
 *      vp_group - (IN) vp_group to update with
 *      egress - (IN) True if we are dealing with egress 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_port_vp_group_id_update(int unit, int port, int vp_group,
                                           int egress)
{
    soc_mem_t                          mem = SOURCE_VPm; 
    soc_field_t                        mbrship_field;
    _bcm_vp_group_port_type_t          port_type;
    bcm_trunk_t                        trunk_id;
    bcm_module_t                       mod_out;
    bcm_port_t                         port_out;
    int                                index = 0, id = 0;
    egr_dvp_attribute_entry_t          egr_dvp_entry;

    /* Get the port type for the 'port' supplied */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    /* Resolve the port */
    SOC_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &mod_out, 
                                               &port_out, &trunk_id, &id));

    /* Set the default membership field */
    mbrship_field = VLAN_MEMBERSHIP_PROFILEf;

    /* Depending on the 'port_type' get the offset we need to add */
    if(port_type == _bcmVpGrpPortGlp) {

        if (!egress) {
            mem = SOURCE_TRUNK_MAP_TABLEm;
        } else {
            mem = EGR_GPP_ATTRIBUTESm;
        }

        SOC_IF_ERROR_RETURN(
            _bcm_td2p_vp_group_gpp_hw_index_get(unit, port, &index));

    } else if (port_type == _bcmVpGrpPortVp){

        if (!egress) {
            mem = SOURCE_VPm;
        } else {
            mem = EGR_DVP_ATTRIBUTEm;

            /* Over ride the mbrship_field appropriately for egr vp */
            SOC_IF_ERROR_RETURN
                (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, id, &egr_dvp_entry));
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                         &egr_dvp_entry, mbrship_field, &mbrship_field));
        }

        /* Set up the index */
        index = id;
    }

#if !defined(SOC_NO_NAMES)
    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!:vp_group %d port 0x%x egress %d"
                         "mem %s field %s \n\n"), 
                         vp_group, port, egress, soc_mem_name[mem], 
                         SOC_FIELD_NAME(unit, mbrship_field)));
#endif

    /* Modify the group into relevant memory */
    SOC_IF_ERROR_RETURN(
        soc_mem_field32_modify(unit, mem, index,
                               mbrship_field, vp_group));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_port_type_get
 * Purpose:
 *      For a given port, return its type, whether is a FP/GPP/VP
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port in question
 *      port_type - (OUT) port_type
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2p_vp_group_port_type_get(int unit, int port, 
                                 _bcm_vp_group_port_type_t *port_type)
{
    /* Determine the port type (FP, GPP, VP) */
    if (!BCM_GPORT_IS_SET(port) ||
        ((BCM_GPORT_IS_SET(port)) && (BCM_GPORT_IS_LOCAL(port) ||
                                      BCM_GPORT_IS_MODPORT(port)))) {
        *port_type = _bcmVpGrpPortLocal;
    } else if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
        *port_type = _bcmVpGrpPortGlp;
    } else {
        *port_type = _bcmVpGrpPortVp;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_join_with_bitmap
 * Purpose:
 *      Given a port and a vlan_vfi bmap, either join and existing
 *      vp group or create a new group.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port number passed in
 *      vlan_vfi_bmap - (IN) The VLAN/VFI bmap
 *      egress -  (IN) Direction, TRUE is egress
 *      vp_group -  (OUT) The alloted vp_group num, if all goes well
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_td2p_vp_group_join_with_bitmap(int unit, int port,
                                    SHR_BITDCL *vlan_vfi_bmap,
                                    int egress,
                                    int *vp_group)
{
    int   vp_group_num, rv;

    *vp_group = _BCM_INVALID_VP_GROUP;
    VP_GROUP_LOCK_MEMS(unit, egress);

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: port 0x%x egress %d\n\n"), 
                         port, egress));

    /* Try and find a group we can re-use */
    rv = _bcm_td2p_vp_group_find_matching_group(unit, vlan_vfi_bmap, 
                                                _BCM_DEFAULT_STP_STATE, 
                                                egress,
                                                _bcmVpGrpVp,
                                                _BCM_DEFAULT_UNTAGGED_STATE, 
                                                &vp_group_num);

    if(vp_group_num != -1) {
        /* Join the group */
        rv = _bcm_td2p_vp_group_join(unit, port, _BCM_INVALID_STG, 
                  _BCM_DEFAULT_STP_STATE, 
                   egress, 
                  _bcmVpGrpVp, /* This could be a VP/GPP, using VP here */
                   vp_group_num,
                  _BCM_DEFAULT_UNTAGGED_STATE); 

        *vp_group = vp_group_num;
    } else {

        /* If we end up here, it means we have NOT found an existing vp_group
           to join, we need to look for a new one */
        rv = _bcm_td2p_vp_group_port_new(unit, port, _BCM_INVALID_VLAN_VFI, 
                                         egress, _BCM_INVALID_VP_GROUP, 
                                         vlan_vfi_bmap, 
                                         _BCM_DEFAULT_UNTAGGED_STATE,
                                         vp_group);

        if(rv != BCM_E_NONE) {
            LOG_INFO(BSL_LS_BCM_VLAN,
                (BSL_META_U(unit,"!: Port add failed: %d\n\n"), rv));

            VP_GROUP_UNLOCK_MEMS(unit, egress);
            return rv;
        }

        /* Update the new vp_group_field into the port that we are processing */
        SOC_IF_ERROR_RETURN(
            _bcm_td2p_vp_group_port_vp_group_id_update(unit, port, 
                                                       *vp_group, egress));
    }

    VP_GROUP_UNLOCK_MEMS(unit, egress);

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: Group joined: %d\n\n"), *vp_group));

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_gpp_hw_index_get
 * Purpose:
 *      Given a port and port_type, pass back the absolute value to
 *      be used within a vp_group in addition to the offset.
 *      bmap from the group this port is a member of.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port number
 *      egress - (IN) True for egress
 *      hw_index - (OUT) The hardware index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_gpp_hw_index_get(int unit, int port,
                                    int *hw_index)
{
    bcm_trunk_t trunk_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    int id;

    /* Resolve the port */
    SOC_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &mod_out, 
                                               &port_out, &trunk_id, &id));

    SOC_IF_ERROR_RETURN(
        _bcm_esw_src_mod_port_table_index_get(unit, mod_out, 
                                              port_out, hw_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_resolve_port_num
 * Purpose:
 *      Given a port and port_type, pass back the absolute value to
 *      be used for vp_group bmap sw updations, in addition to the offset.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Port number passed in .
 *      port_abs_value - (OUT) The absolue port number
 *      port_offset -  (OUT) The offset to be used for VP group bmap
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td2p_vp_group_resolve_port_num(int unit, int port,
                                    int *port_abs_value,
                                    int *port_offset)
{
    bcm_module_t    modid;
    bcm_port_t      port_num;
    bcm_trunk_t     trunk_id;
    int             id;
    _bcm_vp_group_port_type_t port_type;

    /* Get the port type for the 'port' supplied */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    /* Resolve the port */
    SOC_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &modid, 
                                               &port_num, &trunk_id, &id));

    /* Depending on the 'port_type' get the offset we need to add */
    if(port_type == _bcmVpGrpPortGlp) {
        /* GPP's reside at the start of the array, no offset needed */
        *port_offset = 0;
        /* For GPP ports, resolve index based on (mod,port) */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_src_mod_port_table_index_get(unit, modid, port_num, port_abs_value));
    } else {
        /* VP's follow the GPP's, add the offset */
        *port_offset = BCMI_VP_GROUP_VP_OFFSET;
        /* For VP ports, the vp value comes from the 'id' */
        *port_abs_value = id;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_group_group_num_ifilter_get
 * Purpose:
 *      Given a access_type and the port value,
 *      return the current vp_group the entity belongs to.
 * Parameters:
 *      unit - (IN) BCM device number
 *      access_type - (IN) The data_type which can be FP, GPP/VP or STG
 *      port   - (IN) Gpp/VP port value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      vp_group - (OUT) The vp_group associated with the entity
 *      ifilter - (OUT) The ifilter value 
 *      
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_td2p_vp_group_group_num_ifilter_get(int unit, 
                                        _bcm_vp_group_access_type_t access_type,
                                        int port,
                                        int egress,
                                        int *vp_group,
                                        int *ifilter)
{
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    bcm_trunk_t     trunk_id;
    int             id, rv = BCM_E_NONE, index, lport_index;
    soc_mem_t       vp_mem = SOURCE_VPm, gpp_stm_mem = SOURCE_TRUNK_MAP_TABLEm;
    soc_mem_t       lport_mem = LPORT_TABm;
    soc_field_t     mbrship_field = VLAN_MEMBERSHIP_PROFILEf;
    soc_field_t     vp_enfilter_field =  ENABLE_IFILTERf;
    soc_field_t     gpp_enfilter_field = EN_EFILTERf;
    soc_field_t     lport_field = LPORT_PROFILE_IDXf;
    uint32          *vp_entry = NULL, *gpp_stm_entry = NULL, *lport_entry = NULL;
    egr_dvp_attribute_entry_t dvp_entry;
    _bcm_vp_group_port_type_t port_type;

    /* Get the port type for the 'port' supplied */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    /* Resolve the port */
    SOC_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &mod_out, 
                                               &port_out, &trunk_id, &id));

    /* Depending on the port type and the direction,
       point to the correct mems/fields */
    if (!egress) {
        if(port_type == _bcmVpGrpPortVp) {

            vp_mem = SOURCE_VPm;
            vp_enfilter_field = ENABLE_IFILTERf;

        } else if (port_type == _bcmVpGrpPortGlp) {

            gpp_stm_mem = SOURCE_TRUNK_MAP_TABLEm;
            lport_mem = LPORT_TABm;
            lport_field = LPORT_PROFILE_IDXf;
            gpp_enfilter_field = EN_IFILTERf;
        }
        
        mbrship_field = VLAN_MEMBERSHIP_PROFILEf;
         
    } else {
        if(port_type == _bcmVpGrpPortVp) {

            vp_mem = EGR_DVP_ATTRIBUTEm;

            SOC_IF_ERROR_RETURN
                (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, id, &dvp_entry));
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                         &dvp_entry, EN_EFILTERf, &vp_enfilter_field));
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                         &dvp_entry, VLAN_MEMBERSHIP_PROFILEf, &mbrship_field));

        } else if (port_type == _bcmVpGrpPortGlp) {

            gpp_stm_mem = EGR_GPP_ATTRIBUTESm;
            lport_mem = EGR_LPORT_PROFILEm;
            lport_field = EGR_LPORT_PROFILE_IDXf;
            gpp_enfilter_field = EN_EFILTERf;
            mbrship_field = VLAN_MEMBERSHIP_PROFILEf;
        }
    }

    if(port_type == _bcmVpGrpPortVp) {

        /* Enable below log when needed, not much use every run
#if !defined(SOC_NO_NAMES)         
        LOG_INFO(BSL_LS_BCM_VLAN,
            (BSL_META_U(unit,"!:vp_mem %s mbr_field %s filter_field %s"), 
                             soc_mem_name[vp_mem], 
                             SOC_FIELD_NAME(unit, mbrship_field), 
                             SOC_FIELD_NAME(unit, vp_enfilter_field)));
#endif         
        */

        /* Read the VP entry and get the associated mbrship_ptr and
           en_ifilter value */
        vp_entry = sal_alloc(SOC_MEM_BYTES(unit, vp_mem), 
                             "Get VP Group and Enfilter");

        if (vp_entry == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        rv = soc_mem_read(unit, vp_mem, MEM_BLOCK_ANY, id, vp_entry);

        VP_GROUP_CLEANUP(rv);

        *vp_group = soc_mem_field32_get(unit, vp_mem, 
                                        vp_entry, mbrship_field);
        *ifilter = soc_mem_field32_get(unit, vp_mem, 
                                       vp_entry, vp_enfilter_field);
    } else if (port_type == _bcmVpGrpPortGlp) {

        _bcm_td2p_vp_group_gpp_hw_index_get(unit, port, &index);

        /* Enable below log when needed, not much use every run
#if !defined(SOC_NO_NAMES)         
        LOG_INFO(BSL_LS_BCM_VLAN,
            (BSL_META_U(unit,"!:gpp_stm_mem %s mbr_field %s "
                             "lport_field: %s filter_field %s\n"), 
                             soc_mem_name[gpp_stm_mem], 
                             SOC_FIELD_NAME(unit,mbrship_field), 
                             SOC_FIELD_NAME(unit, lport_field), 
                             SOC_FIELD_NAME(unit, gpp_enfilter_field)));
#endif         
        */

        gpp_stm_entry = sal_alloc(SOC_MEM_BYTES(unit, gpp_stm_mem),
                                  "VP Group STM");
        if (gpp_stm_entry == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        /* With the base and the port_num get to the mem location */
        rv = soc_mem_read(unit, gpp_stm_mem, MEM_BLOCK_ANY, 
                          index, gpp_stm_entry);

        VP_GROUP_CLEANUP(rv);

        *vp_group = soc_mem_field32_get(unit, gpp_stm_mem, 
                                        gpp_stm_entry, mbrship_field);

        /* Get the LPORT profile index */
        lport_index = soc_mem_field32_get(unit, gpp_stm_mem, 
                                        gpp_stm_entry, lport_field);

        lport_entry = sal_alloc(SOC_MEM_BYTES(unit, lport_mem),
                                  "LPORT Entry");

        if (lport_entry == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        /* Read the lport entry */
        rv = soc_mem_read(unit, lport_mem, MEM_BLOCK_ANY, 
                          lport_index, lport_entry);

        VP_GROUP_CLEANUP(rv);

        /* From the LPORT memory, get the EN_FILTER value */
        *ifilter = soc_mem_field32_get(unit, lport_mem, 
                                        lport_entry, gpp_enfilter_field);
    }


    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,
          "!: Port 0x%x egress: %d "
          "ifilter: %1d vp_group: %d port_type %s \n"),
         port, egress, *ifilter, *vp_group,
        _bcm_vp_group_port_type_strs[port_type]));


cleanup:

    if (vp_entry != NULL) {
        sal_free(vp_entry);
    }

    if (gpp_stm_entry != NULL) {
        sal_free(gpp_stm_entry);
    }

    if (lport_entry != NULL) {
        sal_free(lport_entry);
    }

    return rv;
}


/*
 * Function:
 *      bcm_td2p_vp_group_ifilter_set
 * Purpose:
 *      Given a access_type and the port value,
 *      return the current vp_group the entity belongs to.
 * Parameters:
 *      unit - (IN) BCM device number
 *      access_type - (IN) The data_type which can be FP, GPP/VP or STG
 *      port   - (IN) Gpp/VP port value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      ifilter - (IN) The ifilter value to set
 *      
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_td2p_vp_group_ifilter_set(int unit, 
                              _bcm_vp_group_access_type_t access_type,
                              int port,
                              int egress,
                              int ifilter)
{
    bcm_module_t    modid;
    bcm_port_t      port_num;
    bcm_trunk_t     trunk_id;
    int             id, rv = BCM_E_NONE;
    soc_mem_t       vp_mem = SOURCE_VPm;
    soc_field_t     vp_enfilter_field = ENABLE_IFILTERf;
    soc_field_t     gpp_enfilter_field = EN_EFILTERf;
    _bcm_vp_group_port_type_t port_type;
    egr_dvp_attribute_entry_t dvp_entry;

    /* Get the port type for the 'port' supplied */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    /* Resolve the port */
    SOC_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &modid, 
                                               &port_num, &trunk_id, &id));

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,
          "!: Port 0x%x egress: %d "
          "ifilter: %1d port_type %s \n"),
         port, egress, ifilter, 
        _bcm_vp_group_port_type_strs[port_type]));

    /* Depending on the port type and the direction,
       point to the correct mems/fields */
    if (!egress) {
        if(port_type == _bcmVpGrpPortVp) {

            vp_mem = SOURCE_VPm;
            vp_enfilter_field = ENABLE_IFILTERf;

        } else if (port_type == _bcmVpGrpPortGlp) {
            gpp_enfilter_field = EN_IFILTERf;
        } 
    } else {
        if(port_type == _bcmVpGrpPortVp) {

            vp_mem = EGR_DVP_ATTRIBUTEm;

            SOC_IF_ERROR_RETURN
                (READ_EGR_DVP_ATTRIBUTEm(unit, MEM_BLOCK_ANY, id, &dvp_entry));
            BCM_IF_ERROR_RETURN(_td2p_egr_dvp_attribute_field_name_get(unit,
                         &dvp_entry, EN_EFILTERf, &vp_enfilter_field));
        } else if (port_type == _bcmVpGrpPortGlp) {
            gpp_enfilter_field = EN_EFILTERf;
        }
    }

    if(port_type == _bcmVpGrpPortVp) {

        rv = soc_mem_field32_modify(unit, vp_mem, id,
                       vp_enfilter_field, ifilter);

    } else if (port_type == _bcmVpGrpPortGlp) {

        rv = _bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_NONE,
                                   gpp_enfilter_field, ifilter);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_stp_state_set
 * Purpose:
 *      Given a "STG" and a "Group-Num", set the STP-State.
 *      and 
 * Parameters:
 *      unit - (IN) BCM device number
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      vp_group - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *       stp_state  - (OUT) The stp-state of the stg-group combo 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_stp_state_set(int unit, int stg, int vp_group, 
                                 int egress, int stp_state)
{
    soc_mem_t             mem;

    /* Param check */
    if(vp_group < 0) {
        return BCM_E_PARAM;
    }

    if (!egress) {
        mem = STG_TABm;
    } else {
        mem = EGR_VLAN_STGm;
    }

    SOC_IF_ERROR_RETURN(
        soc_mem_field32_modify(unit, mem, stg,
                   _bcm_vp_group_stp_vp_grp_map[vp_group].stp_vp_group_field,
                   stp_state));

    /* 
#if !defined(SOC_NO_NAMES)     
    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,
          ": vp_group: %d for Stg 0x%x (%s) was set to stp_state: %d egress: %d \n\n"),
         vp_group, stg, soc_mem_name[mem], stp_state, egress));
#endif     
    */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_leave_all_stg
 * Purpose:
 *      Given a vp_group, clear all STGs of this vp_group, we do this by
 *      setting the 'stp-state' to zero.
 * * Parameters:
 *      unit - (IN) BCM device number
 *      vp_group - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_leave_all_stg(int unit, int vp_group, int egress)
{
    int stg_max = 0, stg;

    bcm_esw_stg_count_get(unit, &stg_max);

    for (stg = 1; stg <= stg_max; stg++) {
        _bcm_td2p_vp_group_stp_state_set(unit, stg, vp_group, 
                                         egress, _BCM_DEFAULT_STP_STATE);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_leave_all_untagged_bitmap
 * Purpose:
 *      Given a vp_group, clear all untag_bits of this vp_group, we do this by
 *      setting the 'ut-state' to zero.
 * * Parameters:
 *      unit - (IN) BCM device number
 *      vp_group - (IN) - STP state that the value needs to be set to
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_leave_all_untagged_bitmap(int unit, int vp_group)
{
    int egr_vlan = 0, num_vlan;

    num_vlan = soc_mem_index_count(unit, EGR_VLANm);
    for (egr_vlan = 0; egr_vlan < num_vlan; egr_vlan++) {
        _bcm_td2p_vp_group_ut_state_set(unit, egr_vlan, 
                                        vp_group, _BCM_DEFAULT_UNTAGGED_STATE);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_stp_state_get
 * Purpose:
 *      Given a "STG" and a "Group-Num", get the STP-State.
 *
 * Parameters:
 *      unit - (IN) BCM device number
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      vp_group - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      stp_state  - (OUT) The stp-state of the stg-group combo 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_stp_state_get(int unit, int stg, int vp_group, 
                                 int egress, int *stp_state)
{
    soc_mem_t   mem;
    uint32      *stg_entry = NULL;
    int          rv = BCM_E_NONE;

    if (!egress) {
        mem = STG_TABm;
    } else {
        mem = EGR_VLAN_STGm;
    }

    /* Allocated the needed space for the entry */
    stg_entry = sal_alloc(SOC_MEM_BYTES(unit, mem),
                          "VP Group STM");

    if (stg_entry == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    /* Read the relevant entry */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 
                      stg, stg_entry);

    VP_GROUP_CLEANUP(rv);

    /* Get the STP state */
    *stp_state = soc_mem_field32_get(unit, mem, stg_entry, 
                 _bcm_vp_group_stp_vp_grp_map[vp_group].stp_vp_group_field);

cleanup:

    if (stg_entry != NULL) {
        sal_free(stg_entry);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_ut_state_get
 * Purpose:
 *      Given an "EGR-VLAN" and a group, get the UT state.
 *
 * Parameters:
 *      unit - (IN) BCM device number
 *      egr_vlan - (IN) Egress VLAN
 *      vp_group - (IN) - STP state that the value needs to be set to
 *      ut_state  - (OUT) The UT state for the given group on this egr_vlan
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_ut_state_get(int unit, int egr_vlan, int vp_group, 
                                int *ut_state)
{
    uint32              ut_grp_bmap[2];
    int                 rv = BCM_E_NONE;
    egr_vlan_entry_t    egr_vlan_entry;

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, egr_vlan, &egr_vlan_entry));
    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry,
            UT_VP_GRP_BITMAPf, ut_grp_bmap);
    *ut_state = (ut_grp_bmap[vp_group / 32] & (1 << (vp_group % 32)));

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_ut_state_set
 * Purpose:
 *      Given an "EGR-VLAN" and a group, set the UT state.
 *
 * Parameters:
 *      unit - (IN) BCM device number
 *      egr_vlan - (IN) Egress VLAN
 *      vp_group - (IN) - STP state that the value needs to be set to
 *      ut_state - (IN) The UT state for the given group on this egr_vlan
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_ut_state_set(int unit, int egr_vlan, int vp_group, 
                                int ut_state)
{
    uint32              ut_grp_bmap[2];
    egr_vlan_entry_t    egr_vlan_entry;
    int                 rv = BCM_E_NONE;

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, egr_vlan, &egr_vlan_entry));
    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry,
            UT_VP_GRP_BITMAPf, ut_grp_bmap);

    /* Set or clear the relevant group */
    if(ut_state) {
        ut_grp_bmap[vp_group / 32] |= (1 << (vp_group % 32));
    } else {
        ut_grp_bmap[vp_group / 32] |= ~(1 << (vp_group % 32));
    }
    soc_EGR_VLANm_field_set(unit, &egr_vlan_entry,
            UT_VP_GRP_BITMAPf, ut_grp_bmap);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLANm(unit, MEM_BLOCK_ALL, egr_vlan, &egr_vlan_entry));

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_stg_for_vlan_vfi_get
 * Purpose:
 *      Given a "vlan/vfi" and a egress flag, get the associated STG.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi - (IN) VLAN to get the STG for
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      stg - (OUT) The STG for the VLAN
 *  
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_stg_for_vlan_vfi_get(int unit, int vlan_vfi, int egress,
                                         int *stg)
{
    soc_mem_t          mem;
    uint32             *entry = NULL, index = 0;
    int                 rv = BCM_E_NONE;

    /* Depending on VLAN/VFI and direction, note the memory to use */
    if (!_BCM_VPN_VFI_IS_SET(vlan_vfi)) {
        if (!egress) {
            mem = VLAN_TABm;
        } else {
            mem = EGR_VLANm;
        }
        index = vlan_vfi;
    } else {
        if (!egress) {
            mem = VFIm;
        } else {
            mem = EGR_VFIm;
        }

        _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vfi);
    }

    entry = sal_alloc(SOC_MEM_BYTES(unit, mem),
                              "VP Group STG");
    if (entry == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }

    /* Read the memory location */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry);

    VP_GROUP_CLEANUP(rv);
        
    /* Assign the stp_state */
    *stg = soc_mem_field32_get(unit, mem, entry, STGf);

#if !defined(SOC_NO_NAMES)     
    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!:vlan_vfi %d egress %d "
                         "mem %s stg: %d \n\n"), 
                         vlan_vfi, egress, soc_mem_name[mem], *stg));
#endif

cleanup:

    if (entry != NULL) {
        sal_free(entry);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_find_matching_group
 * Purpose:
 *      Given a "vlan_vfi" bmap and a access_type, run through all vp_groups
 *      and check if there is a vp_group that matches the "vlan_vfi",
 *      STG-state and the untagged_state.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi_bmap   - (IN) Bmap of "vlan_vfi"
 *      stp_state - (IN) - STP state that needs to be matched against
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      ut_state - (IN) The untagged state to match against
 *      vp_group_num - (OUT) If a matching vp_grp is found, that value
 *                           is filled in, into this field
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_find_matching_group(int unit, SHR_BITDCL *vlan_vfi_bmap, 
                                       int stp_state, 
                                       int egress, 
                                       _bcm_vp_group_access_type_t access_type,
                                       int ut_state,
                                       int *vp_group_num)
{
    int                    num_grps = 0, grp_stp_state = 0;
    int                    grp_ut_state = 0, stg = 0, i, vlan_vfi_value = 0;
    _bcm_td2p_vp_group_t   *vp_grp_array = NULL;
    int                    num_vlan = 0, num_vfi = 0, vlan_vfi_ctr = 0; 
    int                    grp_compatible = 0;

    /* Set the vp_group_num to invalid to start with */
    *vp_group_num = _BCM_INVALID_VP_GROUP;

    /* Get the data pertaining to Ingress/Egress */
    if(!egress) {
        num_grps = VP_GROUP_BK(unit)->num_ing_vp_group;
        vp_grp_array = ING_VP_GROUP(unit, 0);
    } else {
        num_grps = VP_GROUP_BK(unit)->num_eg_vp_group;
        vp_grp_array = EG_VP_GROUP(unit, 0);
    }

    /* Get the size of VLAN and VFI tables and allocate memory */
    SOC_IF_ERROR_RETURN
        (_bcm_td2p_vp_group_vlan_vfi_size_get(unit, egress, 
                                              &num_vlan, &num_vfi));

    /* Set a invalid valid for the group to start with */
    *vp_group_num = -1;

    /* Run through all groups and if the port of one of them, get the
       related vlan_vfi bmap */
    for (i = 0; i < num_grps; i++) {
        if ((vp_grp_array->gpp_vp_count > 0) || (vp_grp_array->stg_count > 0)) {

            /* In case of a 'stg' access type, if the "vp_gpp_count" is zero,
               only check that the stp-state for this group is the same as the
               one we are looking to add */
            if ((vp_grp_array->gpp_vp_count == 0) && 
                (access_type ==  _bcmVpGrpStg)) {

                /* Get the stp-state for this (STG,Group)*/
                SOC_IF_ERROR_RETURN(_bcm_td2p_vp_group_stp_state_get(unit, 
                                       stg, i, egress, &grp_stp_state));

                /* If the stp_state is the same as the one passed in, mark that
                   we have a found a like-minded group and exit */
                if (stp_state == grp_stp_state) {
                    *vp_group_num = i;
                    return BCM_E_NONE;
                }
            } else {

                grp_compatible = 1;

                /* Check if the vlan_vfi bmap matches what is passed in */
                if (SHR_BITEQ_RANGE(vlan_vfi_bmap, vp_grp_array->vlan_vfi_bitmap,
                        0, (num_vlan + num_vfi))) {

                    for (vlan_vfi_ctr = 0; vlan_vfi_ctr < (num_vlan + num_vfi); vlan_vfi_ctr++) {
                    
                            if (SHR_BITGET((vp_grp_array->vlan_vfi_bitmap), vlan_vfi_ctr)) {
                    
                                /* If it is a VFI we are dealing with, encode it as one */
                                if(vlan_vfi_ctr <= BCM_VLAN_MAX) {
                                    vlan_vfi_value = vlan_vfi_ctr;
                                } else {
                                    _BCM_VPN_SET(vlan_vfi_value, _BCM_VPN_TYPE_VFI, 
                                                 (vlan_vfi_ctr - (BCM_VLAN_MAX+1)));
                                }

                                /* Found a VLAN bit set, now, get its STG group */
                                SOC_IF_ERROR_RETURN(
                                    _bcm_td2p_vp_group_stg_for_vlan_vfi_get(unit, 
                                            vlan_vfi_value, egress, &stg));
                                        
                                /* Get the stp-state for the stg on this group */
                                _bcm_td2p_vp_group_stp_state_get(unit, stg, 
                                            i, egress, &grp_stp_state);

                                /* Make sure the GRP STP-State matches, else
                                   this group is not compatible. Also check if the
                                   STP state for all STGs is zero, if so, it is a
                                   candidate for re-use */
                                if((stp_state != _BCM_DEFAULT_STP_STATE) && 
                                   (grp_stp_state != _BCM_DEFAULT_STP_STATE) &&
                                   (stp_state != grp_stp_state)) {
                                        grp_compatible = 0;
                                        break;
                                }
                    
                                /* For egress cases we also need to check if the
                                   ut_state is the same, do this only for VLANs and
                                   not VFIs */
                                if(egress && (vlan_vfi_ctr <= BCM_VLAN_MAX)) {
                    
                                    _bcm_td2p_vp_group_ut_state_get(unit, vlan_vfi_value, i,
                                                                    &grp_ut_state);
                    
                                    if (ut_state != grp_ut_state) {
                                        grp_compatible = 0;
                                        break;
                                    }
                                }
                            }
                    } /* num_vlan, num_vfi looop */
                } else {
                    /* The VLAN_VFI bmaps do not match, mark the group incompatible */
                    grp_compatible = 0;
                }

                /* If the group is compatible, mark that we have a found a
                   like-minded group and exit */
                if(grp_compatible) {
                    *vp_group_num = i;
                    return BCM_E_NONE;
                }
            }
        } /* If check for port_count and stg_count */

    /* Move to the next group */
    vp_grp_array++;

    } /* Number of groups*/

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!:matched_grp_num %d egress %d \n\n"), 
                         *vp_group_num, egress));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_port_join
 * Purpose:
 *      Join a given VP group, for a case when associating port with a VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port  - (IN) Front-panel/Gpp/VP port value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      vp_group - (IN) The VP group to join
 *      ut_state - (IN) The Untagged state we are joining with 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_port_join(int unit, int port, int egress, 
                             int vp_group, int ut_state)
{
    int port_abs_value, port_offset = 0;

    _bcm_td2p_vp_group_resolve_port_num(unit, port, &port_abs_value,
                                        &port_offset);
            
    /* Update the relevant port and increment port ref counts */
    if (!egress) {
        SHR_BITSET(ING_VP_GROUP(unit, vp_group)->gpp_vp_bitmap, 
                   port_abs_value + port_offset);
        ING_VP_GROUP(unit, vp_group)->gpp_vp_count++;
    } else {
        SHR_BITSET(EG_VP_GROUP(unit, vp_group)->gpp_vp_bitmap, 
                   port_abs_value + port_offset);
        EG_VP_GROUP(unit, vp_group)->gpp_vp_count++;

        /* If ut_state is set, also increment the ut ref count */
        if(ut_state) {
            EG_VP_GROUP(unit, vp_group)->untagged_count++;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_stg_join
 * Purpose:
 *      Join a given VP group, for a case when associating STP
 *      states to ports (STG,port)
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) Gpp/VP port value
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      stp_state - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      vp_group - (IN) The VP group to join
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_stg_join(int unit, int port, int stg, int stp_state,
                            int egress, int vp_group)
{
    int mod_val, port_val, vp;
    _bcm_vp_group_port_type_t port_type;

    SOC_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_stp_state_set(unit, stg, vp_group, 
                                         egress, stp_state));

    /* Increment the STG ref count, if it is non-default */
    if(stp_state != _BCM_DEFAULT_STP_STATE) {
        if (!egress) {
            ING_VP_GROUP(unit, vp_group)->stg_count++;
        } else {
            EG_VP_GROUP(unit, vp_group)->stg_count++;
        }
    }

    /* Get port type */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    if(!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        vp = port;
        mod_val = port_val = 0;
    } else {
        vp = port;
        /* Get the mod,port associated with this gpp */
        mod_val = BCMI_VP_GROUP_SUBPORT_MODID_GET(port);
        port_val = BCMI_VP_GROUP_SUBPORT_PORT_GET(port);
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,
          "_bcm_td2p_vp_group_stg_join: Group %d stg %3d state %2d "
          "egress: %1d port_type %s vp: %d, gpp_mod %d gpp_port %d "
          "stp_count %d port_count %d.\n"),
          vp_group, stg, stp_state, egress, 
         _bcm_vp_group_port_type_strs[port_type],
          vp, mod_val, port_val, 
         (egress ? EG_VP_GROUP(unit, vp_group)->stg_count:
          ING_VP_GROUP(unit, vp_group)->stg_count ),
          (egress ? EG_VP_GROUP(unit, vp_group)->gpp_vp_count:
          ING_VP_GROUP(unit, vp_group)->gpp_vp_count)));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_join
 * Purpose:
 *      Join a given VP group, we could come here when adding ports to
 *      VLAN/VFIs or when associating STP states to ports (STG,port)
  * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) Front-panel/Gpp/VP port value
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      stp_state - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      access_type - (IN) The data_type which can be FP, GPP/VP or STG
 *      vp_group - (IN) The VP group tp join
 *      ut_state - (IN) The untagged state we are joining with
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_join(int unit, int port, int stg, int stp_state,
                        int egress, _bcm_vp_group_access_type_t access_type,
                        int vp_group, int ut_state)
{
    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"Enter ! port: 0x%x stg: %d stp_state: %2d"
            "egress: %1d access_type: %s vp_grp: %1d ut_state: %d\n\n"), 
            port, stg, stp_state, egress, 
            _bcm_vp_group_access_type_strs[access_type],vp_group,ut_state));

    if(access_type == _bcmVpGrpStg) {
        /* Need to program the relevant h/w with this group */
        SOC_IF_ERROR_RETURN(_bcm_td2p_vp_group_stg_join(unit, port, 
                            stg, stp_state, egress, vp_group));
    } else {
        SOC_IF_ERROR_RETURN(_bcm_td2p_vp_group_port_join(unit, port, 
                             egress, vp_group, ut_state));
    }

    /* Update the new vp_group_field into the port that we are processing */
    SOC_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_port_vp_group_id_update(unit, port, 
                                                   vp_group, egress));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_td2p_vp_group_stg_new
 * Purpose:
 *      Look for a free VP group, set up the necessary internal data for STG.
  * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) Front-panel/Gpp/VP port value
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      stp_state - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      new_vp_group  - (OUT) Newly allocated VP group, if all goes well
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_stg_new(int unit, int port, int stg, int stp_state,
                           int egress, int old_vp_group, int *new_vp_group)
{
    int                         vp, mod_val, port_val;
    _bcm_td2p_vp_group_t       *vp_grp_array = NULL;
    int                         num_vlan = 0, num_vfi = 0;
    int                         vlan_vfi_ctr = 0, vlan_vfi_value = 0;
    _bcm_vp_group_port_type_t   port_type;


    /* Initialze the new vp group num to invalid */
    *new_vp_group = _BCM_INVALID_VP_GROUP;

    /* Get a new unused vp group */
    SOC_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_get_free_group(unit, egress, new_vp_group));

    /* Copy over the current/passed-in vlan_vfi bitmap */
    _bcm_td2p_vp_group_set_sw_vlan_vfi(unit, _BCM_INVALID_VLAN_VFI, old_vp_group, 
                                       *new_vp_group, NULL, egress);

    /* Get the size of VLAN and VFI tables and allocate memory */
    SOC_IF_ERROR_RETURN
        (_bcm_td2p_vp_group_vlan_vfi_size_get(unit, egress, 
                                              &num_vlan, &num_vfi));

    /* Also update the port on the port bitmap */
    if (!egress) {
        vp_grp_array = ING_VP_GROUP(unit, *new_vp_group);
    } else {
        vp_grp_array = EG_VP_GROUP(unit, *new_vp_group);
    }

    /* For each of the existing VLAN/VFI on the old group, read the
       profile_index, gather the 'ing_port_bitmap' and 'vp_group_bitmap'
       update the profile index values */
    for (vlan_vfi_ctr = 0; vlan_vfi_ctr < (num_vlan + num_vfi); vlan_vfi_ctr++) {
        if (SHR_BITGET((vp_grp_array->vlan_vfi_bitmap), vlan_vfi_ctr)) {

            /* If it is a VFI we are dealing with, encode it as one */
            if(vlan_vfi_ctr <= BCM_VLAN_MAX) {
                vlan_vfi_value = vlan_vfi_ctr;
            } else {
                _BCM_VPN_SET(vlan_vfi_value, _BCM_VPN_TYPE_VFI, 
                             (vlan_vfi_ctr - (BCM_VLAN_MAX+1)));
            }

            /* Update the vp_group for the vlan/vfi */
            _bcm_td2p_vp_group_update_group_for_vlan_vfi(unit,
                                                         port,
                                                         vlan_vfi_value,
                                                         *new_vp_group,
                                                         egress,
                                                         0);

            /* Also update the vp_group for the STG for this VLAN/VFI  */
            SOC_IF_ERROR_RETURN(
                _bcm_td2p_vp_group_stg_for_vlan_vfi_get(unit, 
                        vlan_vfi_value, egress, &stg));

        }
    }

    /* Set the STP state on this group */
    SOC_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_stp_state_set(unit, stg, *new_vp_group, 
                                         egress, stp_state));

    /* Increment the stp_state is non-default, initialize the
       untagged ref count */
    if(stp_state != _BCM_DEFAULT_STP_STATE) {
        if (!egress) {
            ING_VP_GROUP(unit, *new_vp_group)->stg_count = 1;
        } else {
            EG_VP_GROUP(unit, *new_vp_group)->stg_count = 1;
        }
    }

    /* Get port type */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    if(!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        vp = port;
        mod_val = port_val = 0;
    } else {
        vp = port;
        /* Get the mod,port associated with this gpp */
        mod_val = BCMI_VP_GROUP_SUBPORT_MODID_GET(port);
        port_val = BCMI_VP_GROUP_SUBPORT_PORT_GET(port);
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
       (BSL_META_U(unit,"_bcm_td2p_vp_group_stg_new: New group %d "
          "created stg %3d state %2d egress: %1d "
          "port_type %s vp: %d, gpp_mod %d "
          "gpp_port %d.\n"), 
          *new_vp_group, stg, stp_state, egress, 
          _bcm_vp_group_port_type_strs[port_type],
          vp, mod_val, port_val));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_set_sw_vlan_vfi
 * Purpose:
 *      Set a VLAN/VFI bit position in a new group, by copying the old
 *      bmap from old_group and updating the current vlan_vfi
  * Parameters:
 *      unit - (IN) BCM device number
 *      vlan_vfi - (IN) VLAN_VFI value
 *      old_vp_group - (IN) Old VP Group
 *      new_vp_group - (IN) New VP Group
 *      vlan_vfi_bmap - (IN) VLAN_VFI bitmap to update the new group with
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_set_sw_vlan_vfi(int unit, int vlan_vfi, int old_vp_group,
                                   int new_vp_group, SHR_BITDCL *vlan_vfi_bmap,
                                   int egress)
{
    int vfi_offset = 0, num_vlan = 0, num_vfi = 0;
    int is_vfi = 0, index = 0, rv = BCM_E_NONE;
    SHR_BITDCL *new_vlan_vfi_bmap = NULL;

    /* Param check */
    if(new_vp_group < 0) {
        return BCM_E_PARAM;
    }

    /* Get the size of VLAN and VFI tables and allocate memory */
    _bcm_td2p_vp_group_vlan_vfi_size_get(unit, egress, 
                                         &num_vlan, &num_vfi);

    /* If a valid 'vlan_vfi_bmap' is not passed in, we need to get a copy from
       the existing vp_group, append the current one and then copy it to the
       new vp_group. But, if there is a valid 'vlan_vfi_bmap' passed in, we
       just copy it over the new vp_group without any alterations */
    if(vlan_vfi_bmap == NULL) {

        /* For valid 'vlan_vfi' values, compute index we need to
           set in the bitmap */
        if (vlan_vfi != _BCM_INVALID_VLAN_VFI) {

            /* If this is VFI, get the corresponding encoded value
               and set the offset */
            if(_BCM_VPN_VFI_IS_SET(vlan_vfi)) {
                is_vfi = 1;
                vfi_offset = BCM_VLAN_MAX + 1;
                _BCM_VPN_GET(index, _BCM_VPN_TYPE_VFI, vlan_vfi);
            } else {
                index = vlan_vfi;
            }
        }

        /* Allocate memory for the new vlan_vfi bmap, copy existing bmap from
           the 'old_vp_group' and update the current vlan_vfi */
        new_vlan_vfi_bmap = sal_alloc(SHR_BITALLOCSIZE(num_vlan + num_vfi), 
                                      "Set SW VLAN/VFI");

        if (new_vlan_vfi_bmap == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        sal_memset(new_vlan_vfi_bmap, 0, SHR_BITALLOCSIZE(num_vlan + num_vfi));

        /* Now, copy the relevant bmap from old_group, update the current
           vlan_vfi and write the same into the new_group_bitmap */
        if (!egress) {
            sal_memcpy(new_vlan_vfi_bmap, 
                       ING_VP_GROUP(unit, old_vp_group)->vlan_vfi_bitmap,
                       SHR_BITALLOCSIZE(num_vlan + num_vfi));

            if (vlan_vfi != _BCM_INVALID_VLAN_VFI) {
                SHR_BITSET(new_vlan_vfi_bmap,
                           (!is_vfi ? index : (index + vfi_offset)));
            }

            sal_memcpy(ING_VP_GROUP(unit, new_vp_group)->vlan_vfi_bitmap, 
                       new_vlan_vfi_bmap,
                       SHR_BITALLOCSIZE(num_vlan + num_vfi));

        } else {
            sal_memcpy(new_vlan_vfi_bmap, 
                       EG_VP_GROUP(unit, old_vp_group)->vlan_vfi_bitmap,
                       SHR_BITALLOCSIZE(num_vlan + num_vfi));

            if (vlan_vfi != _BCM_INVALID_VLAN_VFI) {
                SHR_BITSET(new_vlan_vfi_bmap,
                           (!is_vfi ? index : (index + vfi_offset)));
            }

            sal_memcpy(EG_VP_GROUP(unit, new_vp_group)->vlan_vfi_bitmap,
                       new_vlan_vfi_bmap,
                       SHR_BITALLOCSIZE(num_vlan + num_vfi));
        }
    } else {
        if (!egress) {
            sal_memcpy(ING_VP_GROUP(unit, new_vp_group)->vlan_vfi_bitmap, 
                       vlan_vfi_bmap,
                       SHR_BITALLOCSIZE(num_vlan + num_vfi));

        } else {
            sal_memcpy(EG_VP_GROUP(unit, new_vp_group)->vlan_vfi_bitmap, 
                       vlan_vfi_bmap,
                       SHR_BITALLOCSIZE(num_vlan + num_vfi));
        }
    }

cleanup:
    if(new_vlan_vfi_bmap) {
        sal_free(new_vlan_vfi_bmap);
        new_vlan_vfi_bmap = NULL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_port_new
 * Purpose:
 *      Look for a free VP group, set up the necessary internal data for port.
  * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) Gpp/VP port value
 *      vlan_vfi - (IN) VLAN/VFI value
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      old_vp_group - (IN) The current VP Group
 *      new_vlan_vfi_bmap - (IN) If valid, will be used to update new group
 *      ut_state - (IN) The Untagged state of the new group
 *      new_vp_group - (OUT) The newly allocated vp_group number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_port_new(int unit, int port, int vlan_vfi, int egress,
                            int old_vp_group, SHR_BITDCL *new_vlan_vfi_bmap,
                            int ut_state, int *new_vp_group)
{
    int num_vlan, num_vfi, mod_val, port_val, vlan_vfi_ctr;
    int abs_port_num, port_offset = 0, vlan_vfi_value;
    _bcm_td2p_vp_group_t *vp_grp_array = NULL;
    _bcm_vp_group_port_type_t port_type;

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"Enter !:port 0x%x vlan_vfi: %d "
                         "egress: %1d old_grp: %d ut_state: %1d\n\n"), 
                         port, vlan_vfi, egress, old_vp_group, ut_state));

    /* Get a new unused vp group */
    SOC_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_get_free_group(unit, egress, new_vp_group));

    /* Copy the new_vlan_vfi bmap to the new group */

    /* Copy over the current/passed-in vlan_vfi bitmap */
    _bcm_td2p_vp_group_set_sw_vlan_vfi(unit, vlan_vfi, old_vp_group, 
                                       *new_vp_group, new_vlan_vfi_bmap, 
                                       egress);

    /* Get the size of VLAN and VFI tables and allocate memory */
    _bcm_td2p_vp_group_vlan_vfi_size_get(unit, egress, 
                                         &num_vlan, &num_vfi);

    /* Get the absolute port number and the offset for this port */
    _bcm_td2p_vp_group_resolve_port_num(unit, port, 
                                        &abs_port_num, &port_offset);

    /* Also update the port on the port bitmap */
    if (!egress) {
        SHR_BITSET(ING_VP_GROUP(unit, *new_vp_group)->gpp_vp_bitmap, 
                   (abs_port_num + port_offset));
        vp_grp_array = ING_VP_GROUP(unit, *new_vp_group);
    } else {
        SHR_BITSET(EG_VP_GROUP(unit, *new_vp_group)->gpp_vp_bitmap, 
                   (abs_port_num + port_offset));
        vp_grp_array = EG_VP_GROUP(unit, *new_vp_group);
    }

    /* For each of the existing VLAN/VFI on the new group, read the
       profile_index, gather the 'ing_port_bitmap' and 'vp_group_bitmap'
       and update the 'profile_index' values */
    for (vlan_vfi_ctr = 0; vlan_vfi_ctr < (num_vlan + num_vfi); vlan_vfi_ctr++) {
        if (SHR_BITGET((vp_grp_array->vlan_vfi_bitmap), vlan_vfi_ctr)) {

            /* If it is a VFI we are dealing with, encode it as one */
            if(vlan_vfi_ctr <= BCM_VLAN_MAX) {
                vlan_vfi_value = vlan_vfi_ctr;
            } else {
                _BCM_VPN_SET(vlan_vfi_value, _BCM_VPN_TYPE_VFI, 
                             (vlan_vfi_ctr - (BCM_VLAN_MAX+1)));
            }
                
            _bcm_td2p_vp_group_update_group_for_vlan_vfi(unit, port,
                                                     vlan_vfi_value, 
                                                     *new_vp_group, 
                                                     egress,
                                                     0);

            /* For egress cases we also need to update the ut_state(If set),
               do this only for VLANs and not VFIs */
            if((egress) && (ut_state) && (vlan_vfi_ctr <= BCM_VLAN_MAX )) {
                _bcm_td2p_vp_group_ut_state_set(unit, vlan_vfi_value, *new_vp_group, 
                                                ut_state);
            }
        }
    }

    /* Increment the port ref count */
    if (!egress) {
        ING_VP_GROUP(unit, *new_vp_group)->gpp_vp_count++;
    } else {
        EG_VP_GROUP(unit, *new_vp_group)->gpp_vp_count++;

        /* If the ut_state is non-default, initialize the untagged ref count */
        if(ut_state != _BCM_DEFAULT_UNTAGGED_STATE) {
            EG_VP_GROUP(unit, *new_vp_group)->untagged_count = 1;
        }
    }

    /* Get port type */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    if(!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        mod_val = port_val = 0;
    } else {
        /* Get the mod,port associated with this gpp */
        mod_val = BCMI_VP_GROUP_SUBPORT_MODID_GET(port);
        port_val = BCMI_VP_GROUP_SUBPORT_PORT_GET(port);
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,
          "Exit : VP_Group %d vlan_vfi: %d "
          "egress: %1d port_type %s vp: 0x%x, gpp_mod %d "
          "gpp_port %d stp_count %d port_count %d untagged_count %d \n\n"),
          *new_vp_group, vlan_vfi, egress, 
         _bcm_vp_group_port_type_strs[port_type],
          port, mod_val, port_val, 
         (egress ? EG_VP_GROUP(unit, *new_vp_group)->stg_count:
          ING_VP_GROUP(unit, *new_vp_group)->stg_count ),
          (egress ? EG_VP_GROUP(unit, *new_vp_group)->gpp_vp_count:
          ING_VP_GROUP(unit, *new_vp_group)->gpp_vp_count),
         (egress ? EG_VP_GROUP(unit, *new_vp_group)->untagged_count:
         ING_VP_GROUP(unit, *new_vp_group)->untagged_count)));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_new
 * Purpose:
 *      Look for a free VP group, set up the necessary internal data.
  * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) Front-panel/Gpp/VP port value
 *      vlan_vfi - (IN) The VLAN/VFI value, if applicable
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      stp_state - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 *      access_type - (IN) The data_type which can be FP, GPP/VP or STG
 *      old_vp_group - (IN) The current VP Group
 *      new_vp_group - (OUT) New VP Group allocated, if all goes well
 *      ut_state - (IN) The Untagged state of the new group being created
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_new(int unit, int port, int vlan_vfi, int stg, int stp_state,
                        int egress, _bcm_vp_group_access_type_t access_type,
                        int old_vp_group, int *new_vp_group, int ut_state)
{

    LOG_INFO(BSL_LS_BCM_VLAN, 
              (BSL_META_U(unit,
          "!: port 0x%x stg %3d stp_state %2d "
          "egress: %1d access_type %s old_vp_grp: %d "
          "ut_state %2d\n\n"),
          port, stg, stp_state, egress, 
         _bcm_vp_group_access_type_strs[access_type],
          old_vp_group, ut_state));

    if (access_type == _bcmVpGrpStg) {
        SOC_IF_ERROR_RETURN(
            _bcm_td2p_vp_group_stg_new(unit, port, stg, stp_state,
                           egress, old_vp_group, new_vp_group));
    } else {
        SOC_IF_ERROR_RETURN(
            _bcm_td2p_vp_group_port_new(unit, port, vlan_vfi, egress,
                           old_vp_group, NULL, ut_state, new_vp_group));
    }

    /* Update the new vp_group_field into the port that we are processing */
    SOC_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_port_vp_group_id_update(unit, port, 
                                                   *new_vp_group, egress));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_update_group_for_vlan_vfi
 * Purpose:
 *      Given a VLAN/VFI and a vp_group, update the vp_group for that
 *      VLAN/VFI.
  * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) Port number 
 *      vlan_vfi  - (IN) Front-panel/Gpp/VP port value
 *      vp_group  - (IN) VP Group to leave
 *      egress  - (IN) If TRUE, handling is for egress, else ingress
 *      leave  - (IN) If TRUE, the group/port is being 'removed', else it is
 *                    being added
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_update_group_for_vlan_vfi(int unit, int port, int vlan_vfi, 
                                             int vp_group, int egress,
                                             int leave)
{
    int field_value = 0;
    uint32 fldbuf[2];
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    egr_vlan_vfi_membership_entry_t egr_vlan_vfi;
    void  *entries = NULL;
    _bcm_vp_group_port_type_t  port_type;
    uint32  prof_index = 0;
    soc_field_t field;

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"Enter !:port 0x%x vlan_vfi: %d "
                         "egress: %1d vp_group: %d leave: %1d\n\n"), 
                         port, vlan_vfi, egress, vp_group, leave));

    /* Get the port_type */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    /* Assign the field name and value */

    field = VP_GROUP_BITMAPf;
    field_value = vp_group;

    /* Get the profile_index associated with this VLAN/VFI */
    SOC_IF_ERROR_RETURN
        (_bcm_td2p_vp_group_vlan_vfi_profile_idx_get(unit, vlan_vfi, 
                                                     egress, &prof_index));

    if (!egress) {
        /* Read the current VLAN_VFI entry */
        SOC_IF_ERROR_RETURN
            (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, 
                                           MEM_BLOCK_ANY, prof_index,
                                           &ing_vlan_vfi));

        /* Extract the current vp_group_bitmap */
        soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, &ing_vlan_vfi,
                                               field, fldbuf);

        if (leave) {
            /* Unset the group in question */
            fldbuf[field_value / 32] |= ~(1 << (field_value % 32));
        } else {
            /* Set the group in question */
            fldbuf[field_value / 32] |= (1 << (field_value % 32));
        }

        soc_ING_VLAN_VFI_MEMBERSHIPm_field_set(unit, 
                                               (uint32 *)&ing_vlan_vfi,
                                               field, fldbuf);

        /* Setup the entry and profile into the ing_vlan_vfi table */
        entries = &ing_vlan_vfi;

        SOC_IF_ERROR_RETURN(
            _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vlan_vfi, 
                   BCMI_VP_GROUP_INGRESS, entries));

    } else {
        /* Read the current VLAN_VFI entry */
        SOC_IF_ERROR_RETURN
            (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, 
                                           MEM_BLOCK_ANY, prof_index,
                                           &egr_vlan_vfi));

        /* Extract the current vp_group_bitmap */
        soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, &egr_vlan_vfi,
                                               field, fldbuf);

        if (leave) {
            /* Unset the group in question */
            fldbuf[field_value / 32] |= ~(1 << (field_value % 32));
        } else {
            /* Set the group in question */
            fldbuf[field_value / 32] |= (1 << (field_value % 32));
        }

        soc_EGR_VLAN_VFI_MEMBERSHIPm_field_set(unit, 
                                               (uint32 *)&egr_vlan_vfi,
                                               field, fldbuf);

        /* Setup the entry and profile into the ing_vlan_vfi table */

        entries = &egr_vlan_vfi;
        SOC_IF_ERROR_RETURN(
            _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vlan_vfi, 
                          BCMI_VP_GROUP_EGRESS, entries));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vp_group_leave
 * Purpose:
 *      Leave a given VP group, we could come here when adding/removing
 *      ports to VLAN/VFIs or when associating STP states to ports
 *      (STG,port)
  * Parameters:
 *      unit - (IN) BCM device number
 *      port   - (IN) Front-panel/Gpp/VP port value
 *      vp_group - (IN) VP Group in question
 *      access_type - (IN) Access type (GPP/VP/STG)
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vp_group_leave(int unit, int port, int vp_group, 
                         _bcm_vp_group_access_type_t access_type, int egress)
{
    int abs_port_num, port_offset = 0, gpp_vp_count = 0;
    int stg_count = 0, untagged_count = 0, num_vlan = 0, num_vfi = 0;
    int vlan_vfi_ctr = 0, mod_val = 0, port_val = 0, vp = 0, stg = 0;
    int stp_state = 0, vlan_vfi_value = 0;
    _bcm_td2p_vp_group_t   *vp_grp_array = NULL;
    _bcm_vp_group_port_type_t port_type;

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"Enter !: port 0x%x vp_group: %d "
                         "egress: %d access_type: %s \n\n"), 
                          port, vp_group, egress, 
                          _bcm_vp_group_access_type_strs[access_type]));

    /* Get the data pertaining to Ingress/Egress */
    if(!egress) {
        vp_grp_array = ING_VP_GROUP(unit, vp_group);
    } else {
        vp_grp_array = EG_VP_GROUP(unit, vp_group);
    }

    if(access_type != _bcmVpGrpStg) {
        _bcm_td2p_vp_group_resolve_port_num(unit, port, 
                                            &abs_port_num, &port_offset);

        if (!egress) {
            SHR_BITCLR(ING_VP_GROUP(unit, vp_group)->gpp_vp_bitmap, 
                                    (abs_port_num + port_offset));

            if(ING_VP_GROUP(unit, vp_group)->gpp_vp_count) {
                ING_VP_GROUP(unit, vp_group)->gpp_vp_count--;
            }
        } else {
            SHR_BITCLR(EG_VP_GROUP(unit, vp_group)->gpp_vp_bitmap, 
                                    (abs_port_num + port_offset)); 

            if(EG_VP_GROUP(unit, vp_group)->gpp_vp_count) {
                EG_VP_GROUP(unit, vp_group)->gpp_vp_count--;
            }

            if(EG_VP_GROUP(unit, vp_group)->untagged_count) {
                EG_VP_GROUP(unit, vp_group)->untagged_count--;
            }
        } 
    } else {

        if (!egress) {
            if (ING_VP_GROUP(unit, vp_group)->stg_count) {
                ING_VP_GROUP(unit, vp_group)->stg_count--;
            }
        } else {
            if(EG_VP_GROUP(unit, vp_group)->stg_count) {
                EG_VP_GROUP(unit, vp_group)->stg_count--;
            }
        }
    }

    /* Gather the ref counts */
    if (!egress) {
        gpp_vp_count = ING_VP_GROUP(unit, vp_group)->gpp_vp_count;
        stg_count = ING_VP_GROUP(unit, vp_group)->stg_count;

        /* Force the untagged_count to be 0 for Ingress, since it is not
           applicable */
        untagged_count = 0;
    } else {
        gpp_vp_count = EG_VP_GROUP(unit, vp_group)->gpp_vp_count;
        stg_count = EG_VP_GROUP(unit, vp_group)->stg_count;
        untagged_count = EG_VP_GROUP(unit, vp_group)->untagged_count;
    }


    /* Get port type */
    _bcm_td2p_vp_group_port_type_get(unit, port, &port_type);

    if(!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        vp = port;
        mod_val = port_val = 0;
    } else {
        vp = port;
        /* Get the mod,port associated with this gpp */
        mod_val = BCMI_VP_GROUP_SUBPORT_MODID_GET(port);
        port_val = BCMI_VP_GROUP_SUBPORT_PORT_GET(port);
    }

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,
          "_bcm_td2p_vp_group_leave: Group %d stg %3d state %2d "
          "egress: %1d port_type %s vp: 0x%x, gpp_mod %d gpp_port %d "
          "stp_count %d port_count %d untagged_count %d \n"),
          vp_group, stg, stp_state, egress, 
         _bcm_vp_group_port_type_strs[port_type],
          vp, mod_val, port_val, 
         (egress ? EG_VP_GROUP(unit, vp_group)->stg_count:
          ING_VP_GROUP(unit, vp_group)->stg_count ),
          (egress ? EG_VP_GROUP(unit, vp_group)->gpp_vp_count:
          ING_VP_GROUP(unit, vp_group)->gpp_vp_count),
         (egress ? EG_VP_GROUP(unit, vp_group)->untagged_count:
         ING_VP_GROUP(unit, vp_group)->untagged_count)));

    /* Now, if the 'gpp_vp_count' AND 'stg_count' has gone to zero on this
       VP_Group delete the same and update the hardware states */
    if((0 == gpp_vp_count) && (0 == stg_count) && (0 == untagged_count)) {

        LOG_INFO(BSL_LS_BCM_VLAN,
                  (BSL_META_U(unit,"_bcm_td2p_vp_group_leave: "
                                   "Group being cleaned up!")));

        /* Run through all VLAN/VFI's and unset this vp_group bit */
        for (vlan_vfi_ctr = 0; vlan_vfi_ctr < (num_vlan + num_vfi); vlan_vfi_ctr++) {
            if (SHR_BITGET((vp_grp_array->vlan_vfi_bitmap), vlan_vfi_ctr)) {

                /* If it is a VFI we are dealing with, encode it as one */
                if(vlan_vfi_ctr <= BCM_VLAN_MAX) {
                    vlan_vfi_value = vlan_vfi_ctr;
                } else {
                    _BCM_VPN_SET(vlan_vfi_value, _BCM_VPN_TYPE_VFI, 
                                 (vlan_vfi_ctr - (BCM_VLAN_MAX+1)));
                }

                SOC_IF_ERROR_RETURN(
                _bcm_td2p_vp_group_update_group_for_vlan_vfi(unit, port,
                                                             vlan_vfi_value,
                                                             vp_group, 
                                                             egress, 1));
            }
        }

        /* Clear all STGs of this vp_group */
        _bcm_td2p_vp_group_leave_all_stg(unit, vp_group, egress);

        if (egress) {
            _bcm_td2p_vp_group_leave_all_untagged_bitmap(unit, vp_group);
        }

        /* Clear out the vlan_vfi array */
        SHR_BITCLR_RANGE(vp_grp_array->vlan_vfi_bitmap, 0, 
                         (num_vlan + num_vfi));

        /* Reset the ref counts */
        vp_grp_array->gpp_vp_count = 0;
        vp_grp_array->stg_count = 0;
        vp_grp_array->untagged_count = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_vp_group_stg_move
 * Purpose:
 *      Move FP/GPP/VP from one VP group to another due to add/remove
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN) Gpp/VP port value
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      stp_state - (IN) - STP state that the value needs to be set to
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_stg_move(int unit, int stg, int gport, int stp_state,
                           int egress)
{
    SHR_BITDCL *port_vlan_vfi_bmap = NULL;
    int num_vlan = 0, num_vfi = 0, rv, curr_stp_state = 0;
    int old_vp_group = 0, ifilter = 0, vp_grp_num = -1, new_vp_grp = 0;

    VP_GROUP_LOCK_MEMS(unit, egress);

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"! port 0x%d stg: %d egress: %d\n\n"), 
                          gport, stg, egress));

    /* Get the size of VLAN and VFI tables and allocate memory */
    rv = _bcm_td2p_vp_group_vlan_vfi_size_get(unit, egress, 
                                              &num_vlan, &num_vfi);

    VP_GROUP_CLEANUP(rv);

    port_vlan_vfi_bmap = sal_alloc(SHR_BITALLOCSIZE(num_vlan + num_vfi), 
                                   "vlan bitmap");

    if (NULL == port_vlan_vfi_bmap) {
        rv =  BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(port_vlan_vfi_bmap, 0, SHR_BITALLOCSIZE(num_vlan + num_vfi));

    /* Get the current vp group associated with this STG group */
    rv = bcm_td2p_vp_group_group_num_ifilter_get(unit, _bcmVpGrpStg, gport, 
                                                 egress, &old_vp_group, 
                                                 &ifilter);

    VP_GROUP_CLEANUP(rv);

    /* Check if the the stp-state of the current group is the same as what
       we are trying to configure, if so, its a no-op */
    _bcm_td2p_vp_group_stp_state_get(unit, stg, old_vp_group, 
                                     egress, &curr_stp_state);

    /* If we are setting the stp_state to 0 (_BCM_DEFAULT_STP_STATE),
       we will leave the current group since '0' is the default value
       for the state */
    if(stp_state == _BCM_DEFAULT_STP_STATE) {
        rv = _bcm_td2p_vp_group_leave(unit, gport, old_vp_group, 
                                      _bcmVpGrpStg, egress);
    }

    if(stp_state == curr_stp_state) {
        rv = BCM_E_NONE;
        goto cleanup;
    }

    /* Copy over the associated 'vlan_vfi' bitmap */
    sal_memcpy(port_vlan_vfi_bmap,
            EG_VP_GROUP(unit, old_vp_group)->vlan_vfi_bitmap, 
            SHR_BITALLOCSIZE(num_vlan + num_vfi));

    /* Try and find a group we can re-use */
    _bcm_td2p_vp_group_find_matching_group(unit, port_vlan_vfi_bmap,
                                           stp_state, egress,
                                           _bcmVpGrpStg, 0, &vp_grp_num);

    if(vp_grp_num != -1) {
        /* Join the group */
        rv = _bcm_td2p_vp_group_join(unit, gport, stg, stp_state, egress, 
                                     _bcmVpGrpStg, vp_grp_num,
                                     _BCM_DEFAULT_UNTAGGED_STATE);

        VP_GROUP_CLEANUP(rv);

        /* Now leave the previous group */
        rv = _bcm_td2p_vp_group_leave(unit, gport, old_vp_group, _bcmVpGrpStg,
                                      egress);
        
        goto cleanup;
    }

    /* If we end up here, it means we have NOT found an existing vp_group
       to join, we need to create a new one */
    rv = _bcm_td2p_vp_group_new(unit, gport, _BCM_INVALID_VLAN_VFI, 
                                stg, stp_state, egress, 
                                _bcmVpGrpStg, old_vp_group, &new_vp_grp,
                                _BCM_DEFAULT_UNTAGGED_STATE);
    
    /* Now leave the previous group, if different from new group */
    if ((rv != BCM_E_NONE) &&
        (old_vp_group != new_vp_grp)){
        rv = _bcm_td2p_vp_group_leave(unit, gport, old_vp_group, 
                                      _bcmVpGrpStg, egress);
    }


cleanup:

    if(port_vlan_vfi_bmap) {
        sal_free(port_vlan_vfi_bmap);
        port_vlan_vfi_bmap = NULL;
    }

    VP_GROUP_UNLOCK_MEMS(unit, egress);

    return rv;
}

/*
 * Function:
 *      bcm_td2p_vp_group_stg_get
 * Purpose:
 *      Given a port and a stg, gets it state
 * Parameters:
 *      unit - (IN) BCM device number
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      port - (IN) Port number
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress 
 *      stp_state - (OUT) - STP state the stg-port belongs to
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_stg_get(int unit, int stg, int gport, int egress, 
                          int *stp_state)
{
    int vp_group = 0, ifilter = 0, access_type;
    _bcm_vp_group_port_type_t port_type;

    /* Get the port type for the 'port' supplied */
    SOC_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_port_type_get(unit, gport, &port_type));

    /* Set the access_type based on the port_type */
    if(port_type == _bcmVpGrpPortGlp) {
        access_type = _bcmVpGrpGlp;
    } else if (port_type == _bcmVpGrpPortVp) {
        access_type = _bcmVpGrpVp;
    } else {
        return BCM_E_PARAM;
    }

    /* Get the vp_group and the filter associated with this port */
    SOC_IF_ERROR_RETURN(
        bcm_td2p_vp_group_group_num_ifilter_get(unit, access_type, gport,
                                            egress, &vp_group,
                                            &ifilter));

    /* Make sure the filter values are set to vp_group */
    if(((port_type ==  _bcmVpGrpPortGlp) && (ifilter != GPP_FILTER_VP_GRP_BMAP)) ||
       ((port_type ==  _bcmVpGrpPortVp) && (ifilter != VP_FILTER_VP_GRP_BMAP))) {
        return BCM_E_PARAM;
    }

    /* Now, get the stp_state associated with the stg/port */
    return _bcm_td2p_vp_group_stp_state_get(unit, stg, vp_group, 
                                    egress, stp_state);
}

/*
 * Function:
 *      bcm_td2p_vp_group_stg_default_state_set
 * Purpose:
 *      Given a stg and direction, sets default STP state for all its groups
 * Parameters:
 *      unit - (IN) BCM device number
 *      stg - (IN) STG associated with the call, if the access_type is STG
 *      egress  - (IN) If TRUE, VP handling is for egress, else ingress 
 *      stp_state - (IN) - STP state to be set to
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_vp_group_stg_default_state_set(int unit, int stg, int egress, 
                                        int stp_state)
{
    int vp_group_num, num_grps;

    /* Get the num_grps based on Ingress/Egress */
    if(!egress) {
        num_grps = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm, 
                                        VP_GROUP_BITMAPf);
    } else {
        num_grps = soc_mem_field_length(unit, EGR_VLAN_VFI_MEMBERSHIPm, 
                                        VP_GROUP_BITMAPf);
    }

    for (vp_group_num = 0; vp_group_num < num_grps; vp_group_num++) {
        SOC_IF_ERROR_RETURN(_bcm_td2p_vp_group_stp_state_set(unit, stg,
                                                             vp_group_num,
                                                             egress,
                                                             stp_state));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ing_vp_group_vlan_get_all
 * Purpose:
 *      Get all VPs from the VP groups belonging to the given VLAN.
 * Parameters:
 *      unit - (IN) BCM device number
 *      vlan - (IN) VLAN ID 
 *      gpp_vp_bitmap - (OUT) Bitmap of VPs.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_ing_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *gpp_vp_bitmap)
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
            SHR_BITOR_RANGE(gpp_vp_bitmap, ING_VP_GROUP(unit, i)->gpp_vp_bitmap,
                    0, (BCMI_VP_GROUP_GLP_COUNT + 
                        soc_mem_index_count(unit, SOURCE_VPm)), 
                        gpp_vp_bitmap);
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
 *      gpp_vp_bitmap - (OUT) Bitmap of VPs.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_eg_vp_group_vlan_get_all(int unit, bcm_vlan_t vlan,
        SHR_BITDCL *gpp_vp_bitmap)
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
            SHR_BITOR_RANGE(gpp_vp_bitmap, EG_VP_GROUP(unit, i)->gpp_vp_bitmap,
                    0, (BCMI_VP_GROUP_GLP_COUNT + 
                        soc_mem_index_count(unit, SOURCE_VPm)), 
                        gpp_vp_bitmap);
        }
    }

    return BCM_E_NONE;
}

#if defined(INCLUDE_L3) && defined(BCM_TRX_SUPPORT)

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
bcm_td2p_ing_vp_group_vlan_delete_all(int unit, bcm_vlan_t vlan_vfi)
{
    vlan_mpls_entry_t vlan_mpls_entry;
    ing_vlan_vfi_membership_entry_t vlan_vfi_entry;
    uint32 fldbuf[2];
    int i, k, gport, mod, port, num_gpp;
    uint32 profile_idx;
    int gpp_vp_bitmap_bit_size;

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"! vlan_vfi %d \n\n"), vlan_vfi));

    SOC_IF_ERROR_RETURN
            (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vlan_vfi, &vlan_mpls_entry));
    soc_VLAN_MPLSm_field_get(unit, &vlan_mpls_entry, MEMBERSHIP_PROFILE_PTRf,
            &profile_idx);

    SOC_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_idx,
                                       &vlan_vfi_entry));
    soc_ING_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry,
            VP_GROUP_BITMAPf, fldbuf);

    num_gpp =  BCMI_VP_GROUP_GLP_COUNT;
    gpp_vp_bitmap_bit_size =  num_gpp + soc_mem_index_count(unit, SOURCE_VPm);

    for (i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group; i++) {
        if (fldbuf[i / 32] & (1 << (i % 32))) {
            /* The bit in VP_GROUP_BITMAP that corresponds to
             * VP group i is set. Then remove all VPs in VP group i
             * from VLAN.
             */
            for (k = 0; k < gpp_vp_bitmap_bit_size; k++) {
                if (SHR_BITGET(ING_VP_GROUP(unit, i)->gpp_vp_bitmap, k)) {
                    /* VP k belongs to VP group i */

                    /*
                    BCM_IF_ERROR_RETURN
                        (bcm_td2p_ing_vp_group_move(unit, k, vlan, FALSE));
                    */

                    if(k < num_gpp) {
                        /* Compute the (mod, port) for this GPP,
                           based on the location */ 
                        mod = (k % 255);
                        port = k - mod;
                        BCM_GPORT_SUBPORT_PORT_SET(gport, 
                                                   (((mod & 0xFF) << 0x7) | port ));
                    } else {
                        if(_bcm_vp_encode_gport(unit, (k-BCMI_VP_GROUP_VP_OFFSET), 
                                                &gport) != BCM_E_NONE) {
                            return BCM_E_INTERNAL;
                        }
                    }

                    LOG_INFO(BSL_LS_BCM_VLAN,
                        (BSL_META_U(unit,"! call to remove"
                                         "port_idx %d gport: 0x%x \n\n"), k, gport));

                    BCM_IF_ERROR_RETURN
                    (bcm_td2p_vp_group_port_move(unit, gport, vlan_vfi, 
                                                 BCMI_VP_GROUP_DELETE,
                                                 BCMI_VP_GROUP_INGRESS, 
                                                 BCMI_VP_GROUP_DEFAULT_FLAGS));
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
    int i, k, mod, port, num_gpp;
    uint32 profile_idx;
    int gpp_vp_bitmap_bit_size, gport;

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vlan, &egr_vlan_entry));
    soc_EGR_VLANm_field_get(unit, &egr_vlan_entry, MEMBERSHIP_PROFILE_PTRf,
            &profile_idx);

    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_idx,
                                       &vlan_vfi_entry));
    soc_EGR_VLAN_VFI_MEMBERSHIPm_field_get(unit, &vlan_vfi_entry,
            VP_GROUP_BITMAPf, fldbuf);

    num_gpp =  BCMI_VP_GROUP_GLP_COUNT;
    gpp_vp_bitmap_bit_size =  num_gpp + soc_mem_index_count(unit, 
                                                            EGR_DVP_ATTRIBUTEm);

    for (i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
        if (fldbuf[i / 32] & (1 << (i % 32))) {
            /* The bit in VP_GROUP_BITMAP that corresponds to
             * VP group i is set. Then remove all VPs in VP group i
             * from VLAN.
             */
            for (k = 0; k < gpp_vp_bitmap_bit_size; k++) {
                if (SHR_BITGET(EG_VP_GROUP(unit, i)->gpp_vp_bitmap, k)) {
                    /* VP k belongs to VP group i */
                    /*
                    BCM_IF_ERROR_RETURN
                        (bcm_td2p_eg_vp_group_move(unit, k, vlan, FALSE));
                    */
                    if(k < num_gpp) {
                        /* Compute the (mod, port) for this GPP, based on the location */ 
                        mod = (k % 255);
                        port = k - mod;
                        BCM_GPORT_SUBPORT_PORT_SET(gport, 
                                                   (((mod & 0xFF) << 0x7) | port ));
                    } else {
                        if(_bcm_vp_encode_gport(unit, (k-BCMI_VP_GROUP_VP_OFFSET), 
                                                &gport) != BCM_E_NONE) {
                            return BCM_E_INTERNAL;
                        }
                    }

                    BCM_IF_ERROR_RETURN
                    (bcm_td2p_vp_group_port_move(unit, gport, vlan,
                                                 BCMI_VP_GROUP_DELETE, 
                                                 BCMI_VP_GROUP_EGRESS, 
                                                 BCMI_VP_GROUP_DEFAULT_FLAGS));
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
    int vp = -1;
    bcm_vlan_port_t vlan_vp;
    bcm_gport_t phy_port_trunk = BCM_GPORT_INVALID;
    int is_local;
    int old_ifilter_en = 0, old_efilter_en = 0;
    int old_vp_group, vp_group;
    int num_vlan, num_vfi;
    SHR_BITDCL *vlan_vfi_bitmap = NULL;
    int cur_vp_group = 0;
    int filter_disable, filter_hash_table, filter_vp_grp;
    _bcm_vp_group_access_type_t access_type;

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

    /* Setup the EN_FILTER values to use, 'vp' by default */
    filter_disable = VP_FILTER_DISABLE;
    filter_hash_table = VP_FILTER_HASH;
    filter_vp_grp = VP_FILTER_VP_GRP_BMAP;

    /* Setup the access_type, 'VP' by default */
    access_type = _bcmVpGrpVp;

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
    } else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
        bcm_vxlan_port_t vxlan_port;

        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);

        /* Get the physical port or trunk the VP resides on */
        bcm_vxlan_port_t_init(&vxlan_port);
        BCM_GPORT_VXLAN_PORT_ID_SET(vxlan_port.vxlan_port_id, vp);

        /* How to handle the trunk checks below? 'bcm_td2_vxlan_port_get'
           below needs 'vlan' which we do not have access to */
        /* BCM_IF_ERROR_RETURN(bcm_td2_vxlan_port_get(unit, vlan, &vxlan_port)); 
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            phy_port_trunk = BCM_GPORT_INVALID;
        } else {
            phy_port_trunk = niv_port.port;
        } 
        */ 
    } else if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {

        /* Override the EN_FILTER values */
        filter_disable = GPP_FILTER_DISABLE;
        filter_hash_table = GPP_FILTER_HASH;
        filter_vp_grp = GPP_FILTER_VP_GRP_BMAP;

        /* Setup the access_type */
        access_type = _bcmVpGrpGlp;

    } else { 
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

    /* Get the current Ingress filter mode */
    SOC_IF_ERROR_RETURN(
        bcm_td2p_vp_group_group_num_ifilter_get(unit, access_type, gport, 
                  !(flags & BCM_PORT_VLAN_MEMBER_INGRESS),
                  &cur_vp_group,
                  &old_ifilter_en));

    /* Set ingress filter mode */

    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership) &&
       (flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) {

        if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) { 
            /* td2 hash table */
            if (filter_disable == old_ifilter_en) {
                /* enable hash table */
                bcm_td2p_vp_group_ifilter_set(unit, access_type, gport,
                              !(flags & BCM_PORT_VLAN_MEMBER_INGRESS),
                              filter_hash_table);            }
        } else {
            if (filter_hash_table == old_ifilter_en) {
                /* Disable checks */
                bcm_td2p_vp_group_ifilter_set(unit, access_type, gport,
                              !(flags & BCM_PORT_VLAN_MEMBER_INGRESS),
                              filter_disable);
            }
        }
    } else if (soc_feature(unit,
                soc_feature_vp_group_ingress_vlan_membership)) {

        if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) {
            if (filter_disable == old_ifilter_en) {
                /* Enable ingress filtering */
                /* Get all the VLANs this VP belongs to */
                num_vlan = soc_mem_index_count(unit, VLAN_TABm);

                /* Get the size of VLAN and VFI tables and allocate memory */
                _bcm_td2p_vp_group_vlan_vfi_size_get(unit, 0, 
                                                     &num_vlan, &num_vfi);

                vlan_vfi_bitmap = sal_alloc(
                    SHR_BITALLOCSIZE(num_vlan + num_vfi), "vlan bitmap");
                if (NULL == vlan_vfi_bitmap) {
                    rv = BCM_E_MEMORY;
                    VP_GROUP_CLEANUP(rv);
                }
                sal_memset(vlan_vfi_bitmap, 0, 
                           SHR_BITALLOCSIZE(num_vlan + num_vfi));

                if((access_type == _bcmVpGrpVp) && 
                   !(BCM_GPORT_IS_VXLAN_PORT(gport))) {
                    rv = _bcm_td2p_vp_vlan_vfi_bitmap_get(unit, gport, 
                                                      vlan_vfi_bitmap);
                }

                VP_GROUP_CLEANUP(rv);

                rv = _bcm_td2p_vp_group_join_with_bitmap(unit, gport, 
                                                    vlan_vfi_bitmap, 
                                                    BCMI_VP_GROUP_INGRESS, 
                                                    &vp_group);

                VP_GROUP_CLEANUP(rv);

                if(vlan_vfi_bitmap) {
                    sal_free(vlan_vfi_bitmap);
                    vlan_vfi_bitmap = NULL;
                }

                /* Update ingress filter enable and vp group fields */
                rv = bcm_td2p_vp_group_ifilter_set(unit, access_type, gport,
                              BCMI_VP_GROUP_INGRESS,
                              filter_vp_grp);

                VP_GROUP_CLEANUP(rv);
            }
        } else {
            if (filter_vp_grp == old_ifilter_en) {
                /* Disable ingress filtering */

                /* Update ingress filter enable and vp group fields */
                rv = bcm_td2p_vp_group_ifilter_set(unit, access_type, gport,
                              !(flags & BCM_PORT_VLAN_MEMBER_INGRESS),
                              filter_disable);

                VP_GROUP_CLEANUP(rv);

                /* Remove VP from VP group */
                BCM_IF_ERROR_RETURN
                (_bcm_td2p_vp_group_leave(unit, gport, cur_vp_group, 
                                          access_type, 
                                          BCMI_VP_GROUP_INGRESS));

            }
        }
    }

    /* Get the current Egress filter mode */
    SOC_IF_ERROR_RETURN(
        bcm_td2p_vp_group_group_num_ifilter_get(unit, access_type, gport, 
                  BCMI_VP_GROUP_EGRESS,
                  &old_vp_group,
                  &old_efilter_en));

    /* Set egress filter mode */
    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership) &&
       (flags & BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP)) { 

        /* td2 hash table */
        if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
            if (filter_disable == old_efilter_en) {
                /* enable hash table */
                rv = bcm_td2p_vp_group_ifilter_set(unit, access_type, gport,
                              BCMI_VP_GROUP_EGRESS, filter_hash_table);
            }
        } else {
           if (filter_hash_table == old_efilter_en) {
                rv = bcm_td2p_vp_group_ifilter_set(unit, access_type, gport,
                              BCMI_VP_GROUP_EGRESS, filter_disable);
            }
        }
    } else if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {

        if (flags & BCM_PORT_VLAN_MEMBER_EGRESS) {
            if (filter_disable == old_efilter_en) {
                /* Enable egress filtering */

                /* Get all the VLANs and VFIs this VP belongs to */
                /* Get the size of VLAN and VFI tables and allocate memory */
                _bcm_td2p_vp_group_vlan_vfi_size_get(unit, 1, 
                                         &num_vlan, &num_vfi);

                vlan_vfi_bitmap = sal_alloc(
                    SHR_BITALLOCSIZE(num_vlan + num_vfi),"vlan bitmap");

                if (NULL == vlan_vfi_bitmap) {
                    rv = BCM_E_MEMORY;
                    VP_GROUP_CLEANUP(rv);
                }
                sal_memset(vlan_vfi_bitmap, 0, 
                           SHR_BITALLOCSIZE(num_vlan + num_vfi));

                if((access_type == _bcmVpGrpVp) && 
                   !(BCM_GPORT_IS_VXLAN_PORT(gport))) {
                    rv = _bcm_td2p_vp_vlan_vfi_bitmap_get(unit, gport, 
                                                          vlan_vfi_bitmap);
                }

                VP_GROUP_CLEANUP(rv);

                rv = _bcm_td2p_vp_group_join_with_bitmap(unit, gport, 
                                                         vlan_vfi_bitmap, 
                                                         BCMI_VP_GROUP_EGRESS, 
                                                         &vp_group);

                VP_GROUP_CLEANUP(rv);

                /* Update egress filter enable */
                rv = bcm_td2p_vp_group_ifilter_set(unit, access_type, gport,
                              BCMI_VP_GROUP_EGRESS, filter_vp_grp);

                VP_GROUP_CLEANUP(rv);

                sal_free(vlan_vfi_bitmap);
                vlan_vfi_bitmap = NULL;
            }
        } else {
            if (filter_vp_grp == old_efilter_en) {
                /* Disable egress filtering */

                /* Update egress filter enable and vp group fields */
                rv = bcm_td2p_vp_group_ifilter_set(unit, access_type, gport,
                              BCMI_VP_GROUP_EGRESS, filter_disable);

                VP_GROUP_CLEANUP(rv);

                rv = _bcm_td2p_vp_group_port_vp_group_id_update(unit, gport, 0,
                            BCMI_VP_GROUP_EGRESS);

                VP_GROUP_CLEANUP(rv);

                /* Remove VP from VP group */
                rv = _bcm_td2p_vp_group_leave(unit, gport, cur_vp_group, 
                                              access_type, BCMI_VP_GROUP_EGRESS);

                VP_GROUP_CLEANUP(rv);

            }
        }
    }

cleanup:
    if (vlan_vfi_bitmap) {
        sal_free(vlan_vfi_bitmap);
    }

    return rv;
}

#endif

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
    int vp, cur_vp_group = 0;
    int filter_en = 0, filter_hash_table, filter_vp_grp;
    _bcm_vp_group_access_type_t access_type;

    /* Set access_type to VP by default */
    access_type = _bcmVpGrpVp;

    /* Setup the EN_FILTER values to use, 'vp' by default */
    filter_hash_table = VP_FILTER_HASH;
    filter_vp_grp = VP_FILTER_VP_GRP_BMAP;

    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        vp = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
        vp = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    } else if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        /* Subports are supported, but, handled differently*/

        /* Override the EN_FILTER values */
        filter_hash_table = GPP_FILTER_HASH;
        filter_vp_grp = GPP_FILTER_VP_GRP_BMAP;

        /* Set access_type to GPP*/
        access_type = _bcmVpGrpGlp;
    } else {
        return BCM_E_PARAM;
    }

    *flags = 0;
    COMPILER_REFERENCE(vp);

    if (soc_feature(unit, soc_feature_vp_group_ingress_vlan_membership)) {

        SOC_IF_ERROR_RETURN(
            bcm_td2p_vp_group_group_num_ifilter_get(unit, access_type, gport, 
                      BCMI_VP_GROUP_INGRESS,
                      &cur_vp_group,
                      &filter_en));  

        if (filter_en == filter_vp_grp) {
            *flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
        }
    }

    if (soc_feature(unit, soc_feature_ing_vp_vlan_membership)) {

        SOC_IF_ERROR_RETURN(
            bcm_td2p_vp_group_group_num_ifilter_get(unit, access_type, gport, 
                      BCMI_VP_GROUP_INGRESS,
                      &cur_vp_group,
                      &filter_en));  

        if (filter_en == filter_hash_table) {
            *flags |= BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP |
                      BCM_PORT_VLAN_MEMBER_INGRESS;
        }
    } 

    if (soc_feature(unit, soc_feature_vp_group_egress_vlan_membership)) {

        SOC_IF_ERROR_RETURN(
            bcm_td2p_vp_group_group_num_ifilter_get(unit, access_type, gport, 
                      BCMI_VP_GROUP_EGRESS,
                      &cur_vp_group,
                      &filter_en));  

        if (filter_en == filter_vp_grp) {
            *flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
        }
    }
    if (soc_feature(unit, soc_feature_egr_vp_vlan_membership)) {

        SOC_IF_ERROR_RETURN(
            bcm_td2p_vp_group_group_num_ifilter_get(unit, access_type, gport, 
                      BCMI_VP_GROUP_EGRESS,
                      &cur_vp_group,
                      &filter_en));  

        if (filter_en == filter_hash_table) {
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
    int gpp_vp_count;
    int i;

    /* check if to set the same value */
    if (_bcm_td2p_vp_group_unmanaged[unit].ingress == flag) {
        return BCM_E_NONE;
    }

    /* make sure auto method hasn't kicked in yet */
    if (VP_GROUP_BK(unit)->vp_group_initialized) {
        num_vp_groups = soc_mem_field_length(unit, ING_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);

        for (gpp_vp_count = 0,i = 0; i < VP_GROUP_BK(unit)->num_ing_vp_group;
                i++) {
            if ((0 == ING_VP_GROUP(unit, i)->gpp_vp_count) &&
                 (0 == ING_VP_GROUP(unit, i)->stg_count)){
                    gpp_vp_count++;
            }
        }

        if (gpp_vp_count != num_vp_groups) {
            LOG_INFO(BSL_LS_BCM_VLAN,
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
    int gpp_vp_count;
    int i;

    /* check if to set the same value */
    if (_bcm_td2p_vp_group_unmanaged[unit].egress == flag) {
        return BCM_E_NONE;
    }

    /* make sure auto method hasn't kicked in yet */
    if (VP_GROUP_BK(unit)->vp_group_initialized) {
        num_vp_groups = soc_mem_field_length(unit, EGR_VLAN_VFI_MEMBERSHIPm,
                VP_GROUP_BITMAPf);

        for (gpp_vp_count = 0,i = 0; i < VP_GROUP_BK(unit)->num_eg_vp_group; i++) {
            if ((0 == EG_VP_GROUP(unit, i)->gpp_vp_count) &&
                (0 == ING_VP_GROUP(unit, i)->stg_count)) {
                    gpp_vp_count++;
            }
        }

        if (gpp_vp_count != num_vp_groups) {
            LOG_INFO(BSL_LS_BCM_VLAN,
                     (BSL_META_U(unit,
                 "Unmanaged mode set fails: VP group auto "
                      "method already started\n")));

            return BCM_E_EXISTS;
        }
    }
    _bcm_td2p_vp_group_unmanaged[unit].egress = flag;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_dvp_group_validate
 * Purpose:
 *      Check dvp group id valid or not
 */
int
bcm_td2p_dvp_group_validate(int unit, int dvp_group_id)
{
    int max_group_id;     

    max_group_id = (1 << soc_mem_field_length(unit, EGR_DVP_ATTRIBUTEm, 
                                              VT_DVP_GROUP_IDf)) - 1;
    if ((dvp_group_id < 0) || (dvp_group_id > max_group_id)) {
        return BCM_E_PARAM;
    } 
    
    return BCM_E_NONE;
}

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
    vlan_tab_entry_t vtab;
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    uint32 profile_ptr = 0;
    int rv = BCM_E_NONE;
    
    /* Upper layer already checks that vid is valid */
    soc_mem_lock(unit, VLAN_TABm);
    rv = READ_VLAN_TABm(unit, MEM_BLOCK_ANY, vid, &vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_TABm);
        return rv;
    }
    soc_mem_pbmp_field_get(unit, VLAN_TABm, &vtab,
            PORT_BITMAPf, pbmp);
    soc_mem_unlock(unit, VLAN_TABm);

    SOC_IF_ERROR_RETURN
        (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, (int) vid, &vlan_mpls));

    profile_ptr = soc_VLAN_MPLSm_field32_get(unit, &vlan_mpls,
            MEMBERSHIP_PROFILE_PTRf);

    SOC_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_ptr, 
            &ing_vlan_vfi)); 
    soc_mem_pbmp_field_get(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, ing_pbmp);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_delete_vlan_vfi_membership
 * Purpose:
 *      Delete the VLAN Vfi membership entry.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      vlan  - (IN) VLAN ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_delete_vlan_vfi_membership(int unit, bcm_vlan_t vlan) 
{
    uint32 profile_ptr = 0;
    vlan_mpls_entry_t vlan_mpls;
    egr_vlan_entry_t egr_vtab;
    ing_vlan_vfi_membership_entry_t ing_vlan_vfi;
    int cur_ref_count = 0;

    sal_memset(&ing_vlan_vfi, 0, sizeof(ing_vlan_vfi_membership_entry_t));

    /* Get the MEMBERSHIP_PROFILE_PTR */
    SOC_IF_ERROR_RETURN
        (READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vlan, &vlan_mpls));
    profile_ptr = soc_VLAN_MPLSm_field32_get(unit, &vlan_mpls,
            MEMBERSHIP_PROFILE_PTRf);

    _bcm_vlan_vfi_mbrship_profile_ref_count(unit, profile_ptr,
                                             BCMI_VP_GROUP_INGRESS,
                                             &cur_ref_count);

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: Deleting profile %d for vlan_vfi: %d cur_ref_cnt: %d \n\n"),
                    profile_ptr, vlan, cur_ref_count));

    /*  Delete the Reference Count, if the profile is non-zero */
    if(profile_ptr != 0) {
        (_bcm_vlan_vfi_memebershipship_profile_entry_op(unit, NULL, 1, 
                                                   BCMI_VP_GROUP_DELETE, 
                                                   BCMI_VP_GROUP_INGRESS, 
                                                   &profile_ptr));
    }

    /* Get the MEMBERSHIP_PROFILE_PTR */
    SOC_IF_ERROR_RETURN
        (READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vlan, &egr_vtab));
    profile_ptr = soc_EGR_VLANm_field32_get(unit, &egr_vtab,
            MEMBERSHIP_PROFILE_PTRf);

    BCM_IF_ERROR_RETURN
        (_bcm_vlan_vfi_mbrship_profile_ref_count(unit, profile_ptr,
                                                 BCMI_VP_GROUP_EGRESS,
                                                 &cur_ref_count));

    LOG_INFO(BSL_LS_BCM_VLAN,
        (BSL_META_U(unit,"!: Deleting profile %d for vlan_vfi: %d cur_ref_cnt: %d \n\n"),
                    profile_ptr, vlan, cur_ref_count));

    /*  Delete the Reference Count, if the profile is non-zero */
    if(profile_ptr != 0) {
        SOC_IF_ERROR_RETURN
            (_bcm_vlan_vfi_memebershipship_profile_entry_op(unit, NULL, 1, 
                                                       BCMI_VP_GROUP_DELETE, 
                                                       BCMI_VP_GROUP_EGRESS, 
                                                       &profile_ptr));
    }
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

/*
    rv = WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, profile_ptr, 
            &ing_vlan_vfi);
 
 */

    rv = _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vlan, BCMI_VP_GROUP_INGRESS, &ing_vlan_vfi);

    soc_mem_unlock(unit, ING_VLAN_VFI_MEMBERSHIPm);

    if (rv < 0) {
        return rv;
    }

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

/*
    rv = WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, profile_ptr, 
            &egr_vlan_vfi);
 
*/ 
    rv = _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vlan, BCMI_VP_GROUP_EGRESS, &egr_vlan_vfi);

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
    vlan_mpls_entry_t vlan_mpls;
    egr_vlan_entry_t egr_vtab;
    uint32 profile_ptr = 0;

    SOC_IF_ERROR_RETURN(
            READ_VLAN_MPLSm(unit, MEM_BLOCK_ANY, vid, &vlan_mpls));

    profile_ptr = soc_VLAN_MPLSm_field32_get(unit, &vlan_mpls,
            MEMBERSHIP_PROFILE_PTRf);

    SOC_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_ptr,
                                       &ing_vlan_vfi));
    soc_mem_pbmp_field_set(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &pbmp);

/*
    BCM_IF_ERROR_RETURN
        (WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid,
                                        &ing_vlan_vfi));
*/
  
    _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vid, BCMI_VP_GROUP_INGRESS, &ing_vlan_vfi);
      

    SOC_IF_ERROR_RETURN(READ_EGR_VLANm(unit, MEM_BLOCK_ANY, vid, &egr_vtab));
    profile_ptr = soc_EGR_VLANm_field32_get(unit, &egr_vtab,
            MEMBERSHIP_PROFILE_PTRf);
 
    SOC_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, profile_ptr,
                                       &egr_vlan_vfi));

    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &pbmp);

    /*
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid,
                                        &egr_vlan_vfi));
    */

    SOC_IF_ERROR_RETURN(
        _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vid,
                                                  BCMI_VP_GROUP_EGRESS,
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
    pbmp_t                          cur_pbmp;
    uint32                          prof_index = 0;

    _bcm_td2p_vp_group_vlan_vfi_profile_idx_get(unit, vid, BCMI_VP_GROUP_INGRESS, 
                                                &prof_index);

    BCM_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, prof_index, &ing_vlan_vfi));
    soc_mem_pbmp_field_get(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_OR(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &cur_pbmp);

    /*
    BCM_IF_ERROR_RETURN
        (WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid, &ing_vlan_vfi));
    */
     
    _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vid, BCMI_VP_GROUP_INGRESS, &ing_vlan_vfi);

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
    uint32      prof_index = 0;

    /* Get the profile_idx associated with the VLAN/VFI */
    _bcm_td2p_vp_group_vlan_vfi_profile_idx_get(unit, vid, 
                                                BCMI_VP_GROUP_INGRESS, 
                                                &prof_index);

    BCM_IF_ERROR_RETURN
        (READ_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, prof_index, &ing_vlan_vfi));
    soc_mem_pbmp_field_get(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_REMOVE(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, ING_VLAN_VFI_MEMBERSHIPm, &ing_vlan_vfi,
            ING_PORT_BITMAPf, &cur_pbmp);

    /*
    BCM_IF_ERROR_RETURN
        (WRITE_ING_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid, &ing_vlan_vfi));
    */

    _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vid, BCMI_VP_GROUP_INGRESS,
                                                  &ing_vlan_vfi);

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
    uint32      prof_index = 0;

    /* Get the profile_idx associated with the VLAN/VFI */
    _bcm_td2p_vp_group_vlan_vfi_profile_idx_get(unit, vid, 
                                                BCMI_VP_GROUP_EGRESS, 
                                                &prof_index);

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, prof_index, &egr_vlan_vfi));
    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_OR(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &cur_pbmp);

    /*
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid, &egr_vlan_vfi));
    */     

    _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vid, BCMI_VP_GROUP_EGRESS, &egr_vlan_vfi);

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
    uint32      prof_index = 0;

    /* Get the profile_idx associated with the VLAN/VFI */
    _bcm_td2p_vp_group_vlan_vfi_profile_idx_get(unit, vid, 
                                                BCMI_VP_GROUP_EGRESS, 
                                                &prof_index);

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ANY, prof_index, &egr_vlan_vfi));
    soc_mem_pbmp_field_get(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &cur_pbmp);
    BCM_PBMP_REMOVE(cur_pbmp, pbmp);
    soc_mem_pbmp_field_set(unit, EGR_VLAN_VFI_MEMBERSHIPm, &egr_vlan_vfi,
            PORT_BITMAPf, &cur_pbmp);

    /*
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_VFI_MEMBERSHIPm(unit, MEM_BLOCK_ALL, vid, &egr_vlan_vfi));
    */
    
    _bcm_td2p_vp_group_vlan_vfi_profile_entry_set(unit, vid, BCMI_VP_GROUP_EGRESS,
                                                  &egr_vlan_vfi);
         
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_td2p_vfi_control_params_check
 *
 * Purpose:
 *      Verifies if input parameters are valid for the operation on the device
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id. 
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (IN) Configuration.
 *
 * Returns:
 *      BCM_E_PARAM for errorneous inputs
 *      BCM_E_NONE  if all memebers in the structure are legal
 */
STATIC int
bcmi_td2p_vfi_control_params_check(int unit, bcm_vlan_t vid,
                                   uint32 valid_fields,
                                   bcm_vlan_control_vlan_t *control)
{
    uint32 supported_flags = 0;
    uint32 supported_fields = 0;

    /*
     * VFI_PROFILE table in TD2Plus supports only the following fields:
     *
     *  - IPMCV4_L2_ENABLE  : Enable IGMP-based L2 lookups for IPMCv4 packets
     *  - IPMCV6_L2_ENABLE  : Enable IGMP-based L2 lookups for IPMCv6 packets
     *  - L2_NON_UCAST_DROP : Drop all broadcast and multicast packets
     *  - L2_NON_UCAST_TOCPU: Copy all broadcast and multicast packets to CPU
     *  - L2_MISS_DROP      : Drop unknown unicast traffic, Do not flood.
     *  - L2_MISS_TOCPU     : Copy unknown unicast traffic to the CPU
     *  - LEARN_DISABLE     : Disable learning.
     *  - EN_IFILTER        : Enable membership checks.
     */

    /*
     * VFI table can be programmed with the following fields:
     *
     *  - VRF           : Virtual Routing and Forwarding Instance
     *  - VFI_CLASS_ID  : VFI Class ID for IFP lookup key
     *  - L3_IIF        : Layer3 Incoming Interface to be assigned to the packet
     *  - UUC_INDEX     : Default unknown unicast group for this VFI
     *  - UMC_INDEX     : Default unknown multicast group for this VFI
     *  - BC_INDEX      : Default Multicast group for broadcasting to this VFI
     */

    supported_fields = (BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK |
                        BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK |
                        BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK |
                        BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK |
                        BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK);

    supported_flags =  (BCM_VLAN_IP4_MCAST_L2_DISABLE |
                        BCM_VLAN_IP6_MCAST_L2_DISABLE |
                        BCM_VLAN_UNKNOWN_UCAST_TOCPU |
                        BCM_VLAN_UNKNOWN_UCAST_DROP |
                        BCM_VLAN_NON_UCAST_TOCPU |
                        BCM_VLAN_NON_UCAST_DROP |
                        BCM_VLAN_LEARN_DISABLE |
                        BCM_VLAN_L2_CLASS_ID_ONLY 
                        );

    if (valid_fields != BCM_VLAN_CONTROL_VLAN_ALL_MASK) {
        if ((~supported_fields) & valid_fields) {
            return BCM_E_PARAM;
        }
    }

    if ((~supported_flags) & control->flags) {
        return BCM_E_PARAM;
    }

    if (control->flags & BCM_VLAN_L2_CLASS_ID_ONLY) {
        if (valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) {
            if ((control->if_class < 0) ||
                (control->if_class > SOC_INTF_CLASS_MAX(unit))) {
                return BCM_E_PARAM;
            }
        } else {
            return BCM_E_PARAM;
        }
    }


    /*
     * There is no provision to implicitly map L3 IIFs to VFIs.
     * Hence, assigning L3 IIF class id through this API is not allowed.
     * Users should use bcm_l3_ingress_create() to create L3 ingress interfaces
     * and assign class ids through the same API.
     */

    if (!soc_feature(unit, soc_feature_vp_sharing)) {
        if (control->egress_vlan != BCM_VLAN_NONE) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_profile_add
 *
 * Purpose:
 *      Programs VFI_PROFILE table based on input bcm_vlan_control_vlan_t
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (IN) Configuration.
 *     profile_index - (OUT) Profile index allocated by soc_mem_profile_add
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_profile_add(int unit, uint32 valid_fields,
                          bcm_vlan_control_vlan_t *vfi_control,
                          uint8 *profile_index)
{
    int rv;
    int i;
    soc_field_t field;
    uint32 index;
    int value;
    vfi_profile_entry_t vfi_profile_entry;
    soc_mem_t vfi_profile_mem = VFI_PROFILEm;
    void *entries[1];
    int vfi_profile_flags_map[][3] = {
            /* field,              flag,                          bool_invert */
            {L2_NON_UCAST_DROPf,   BCM_VLAN_NON_UCAST_DROP,       0},
            {L2_NON_UCAST_TOCPUf,  BCM_VLAN_NON_UCAST_TOCPU,      0},
            {L2_MISS_DROPf,        BCM_VLAN_UNKNOWN_UCAST_DROP,   0},
            {L2_MISS_TOCPUf,       BCM_VLAN_UNKNOWN_UCAST_TOCPU,  0},
            {IPMCV4_L2_ENABLEf,    BCM_VLAN_IP4_MCAST_DISABLE,    1},
            {IPMCV6_L2_ENABLEf,    BCM_VLAN_IP6_MCAST_DISABLE,    1},
            {LEARN_DISABLEf,       BCM_VLAN_LEARN_DISABLE,        0},
            {INVALIDf,             0,                             0}
        };


    *profile_index = 0;
    entries[0] = &vfi_profile_entry,
    sal_memset(&vfi_profile_entry, 0, sizeof(vfi_profile_entry_t));

    for (i = 0; i < COUNTOF(vfi_profile_flags_map); i++) {
        field = vfi_profile_flags_map[i][0];
        if (field == INVALIDf) {
            continue;
        }
        value = ((vfi_profile_flags_map[i][2]) ?
                 (!(vfi_control->flags & vfi_profile_flags_map[i][1])):
                 ((vfi_control->flags & vfi_profile_flags_map[i][1])));

        value = !!value;
        soc_mem_field32_set(unit, vfi_profile_mem, &vfi_profile_entry, field, value);
    }

    rv = _bcm_vfi_profile_entry_add(unit, entries, 1, &index);
    BCM_IF_ERROR_RETURN(rv);

    *profile_index = index;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_profile_get
 *
 * Purpose:
 *      Reads VFI_PROFILE table and fills vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     profile_index - (IN) Profile index.
 *     vfi_control - (IN) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_profile_get(int unit, uint8 profile_index, bcm_vlan_control_vlan_t *vfi_control)
{
    int rv;
    int i;
    soc_field_t field;
    int value;
    vfi_profile_entry_t vfi_profile_entry;
    soc_mem_t vfi_profile_mem = VFI_PROFILEm;
    void *entries[1];
    int vfi_profile_flags_map[][3] = {
            /* field,              flag,                          bool_invert */
            {L2_NON_UCAST_DROPf,   BCM_VLAN_NON_UCAST_DROP,       0},
            {L2_NON_UCAST_TOCPUf,  BCM_VLAN_NON_UCAST_TOCPU,      0},
            {L2_MISS_DROPf,        BCM_VLAN_UNKNOWN_UCAST_DROP,   0},
            {L2_MISS_TOCPUf,       BCM_VLAN_UNKNOWN_UCAST_TOCPU,  0},
            {IPMCV4_L2_ENABLEf,    BCM_VLAN_IP4_MCAST_DISABLE,    1},
            {IPMCV6_L2_ENABLEf,    BCM_VLAN_IP6_MCAST_DISABLE,    1},
            {LEARN_DISABLEf,       BCM_VLAN_LEARN_DISABLE,        0},
            {INVALIDf,             0,                             0}
        };

    entries[0] = &vfi_profile_entry;

    rv = _bcm_vfi_profile_entry_get(unit, profile_index, 1, entries);
    BCM_IF_ERROR_RETURN(rv);

    for (i = 0; i < COUNTOF(vfi_profile_flags_map); i++) {
        field = vfi_profile_flags_map[i][0];
        if (field == INVALIDf) {
            continue;
        }
        value = soc_mem_field32_get(unit, vfi_profile_mem, &vfi_profile_entry, field);
        vfi_control->flags |= ((vfi_profile_flags_map[i][2]) ?
                                  ((!value)  ? vfi_profile_flags_map[i][2] : 0) :
                                  ((value) ? vfi_profile_flags_map[i][2] : 0));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_td2p_vfi_control_ingress_set
 *
 * Purpose:
 *      Programs VFI table according to vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (IN) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_control_ingress_set(int unit, int vfi, uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int rv;
    int i;
    soc_field_t field;
    int value;
    uint8 profile_index;
    uint8 old_profile_index;
    vfi_entry_t vfi_entry;
    soc_mem_t mem = VFIm;
    struct vfi_fields_s {
        soc_field_t field;
        uint32  valid_field;
        int    *vlan_control_obj;
    } vfi_field_values[] = {
            {VFI_CLASS_IDf, BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK},
            {L3_IIFf,       BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK},
            {UUC_INDEXf,    BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK},
            {UMC_INDEXf,    BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK},
            {BC_INDEXf,     BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK},
            {INVALIDf,      0, 0}
    };

    vfi_field_values[0].vlan_control_obj = &(control->if_class);
    vfi_field_values[1].vlan_control_obj = &(control->ingress_if);
    vfi_field_values[2].vlan_control_obj = &(control->unknown_unicast_group);
    vfi_field_values[3].vlan_control_obj = &(control->unknown_multicast_group);
    vfi_field_values[4].vlan_control_obj = &(control->broadcast_group);

    /*
     * VFI table can be programmed with the following fields:
     *
     *  - VRF           : Virtual Routing and Forwarding Instance
     *  - VFI_CLASS_ID  : VFI Class ID for IFP lookup key
     *  - L3_IIF        : Layer3 Incoming Interface to be assigned to the packet
     *  - UUC_INDEX     : Default unknown unicast group for this VFI
     *  - UMC_INDEX     : Default unknown multicast group for this VFI
     *  - BC_INDEX      : Default Multicast group for broadcasting to this VFI
     */

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    old_profile_index = soc_mem_field32_get(unit, mem,
                                            &vfi_entry, VFI_PROFILE_PTRf);
    for (i = 0; i < COUNTOF(vfi_field_values); i++) {
        field = vfi_field_values[i].field;
        if (!(valid_fields & vfi_field_values[i].valid_field) || (field == INVALIDf)) {
            continue;
        }
        value = *(vfi_field_values[i].vlan_control_obj);
        soc_mem_field32_set(unit, mem, &vfi_entry, field, value);
    }


    /* Add profile object */
    rv = bcmi_td2p_vfi_profile_add(unit, valid_fields, control, &profile_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Overwrite the the profile pointer in VFI with the new profile index */
    soc_mem_field32_set(unit, mem, &vfi_entry, VFI_PROFILE_PTRf, profile_index);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    /* Delete the old profile object */
    rv = _bcm_vfi_profile_entry_delete(unit, old_profile_index);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_control_egress_set
 *
 * Purpose:
 *      Programs EGR_VFI table according to vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (IN) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_control_egress_set(int unit, int vfi, uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{

    int rv;
    int i;
    soc_field_t field;
    int value;
    egr_vfi_entry_t vfi_entry;
    soc_mem_t mem = EGR_VFIm;
    struct vfi_fields_s {
        soc_field_t field;
        uint32  valid_field;
        uint16    *vlan_control_obj;
    } vfi_field_values[] = {
            {OVIDf, BCM_VLAN_CONTROL_VLAN_ALL_MASK},
            {INVALIDf, 0, NULL}
    };

    vfi_field_values[0].vlan_control_obj = (uint16 *)&(control->egress_vlan);

    /* Read the memory */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (i = 0; i < COUNTOF(vfi_field_values); i++) {
        field = vfi_field_values[i].field;
        if (!(valid_fields & vfi_field_values[i].valid_field) || (field == INVALIDf)) {
            continue;
        }
        value = *(vfi_field_values[i].vlan_control_obj);

        /* update each field */
        soc_mem_field32_set(unit, mem, &vfi_entry, field, value);
    }


    /* Write the memory back */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_control_ingress_get
 *
 * Purpose:
 *      Reads VFI table and fills vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (OUT) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_control_ingress_get(int unit, int vfi, uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{
    int rv;
    int i;
    soc_field_t field;
    int value;
    uint8 profile_index;
    vfi_entry_t vfi_entry;
    soc_mem_t mem = VFIm;
    struct vfi_fields_s {
        soc_field_t field;
        uint32  valid_field;
        int    *vlan_control_obj;
    } vfi_field_values[] = {
            {VFI_CLASS_IDf, BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK},
            {L3_IIFf,       BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK},
            {UUC_INDEXf,    BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK},
            {UMC_INDEXf,    BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK},
            {BC_INDEXf,     BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK},
            {INVALIDf,      0, 0}
    };

    vfi_field_values[0].vlan_control_obj = &(control->if_class);
    vfi_field_values[1].vlan_control_obj = &(control->ingress_if);
    vfi_field_values[2].vlan_control_obj = &(control->unknown_unicast_group);
    vfi_field_values[3].vlan_control_obj = &(control->unknown_multicast_group);
    vfi_field_values[4].vlan_control_obj = &(control->broadcast_group);

    /* Read the memory */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (i = 0; i < COUNTOF(vfi_field_values); i++) {
        field = vfi_field_values[i].field;
        if (!(valid_fields & vfi_field_values[i].valid_field) || (field == INVALIDf)) {
            continue;
        }
        /* read each field */
        value = soc_mem_field32_get(unit, mem, &vfi_entry, field);
        *(vfi_field_values[i].vlan_control_obj) = value;
    }

    control->flags |= (control->if_class) ? BCM_VLAN_L2_CLASS_ID_ONLY : 0;


    field = VFI_PROFILE_PTRf;
    value = soc_mem_field32_get(unit, mem, &vfi_entry, field);

    profile_index  = value;
    rv = bcmi_td2p_vfi_profile_get(unit, profile_index, control);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td2p_vfi_control_egress_get
 *
 * Purpose:
 *      Reads EGR_VFI table and fills vfi_control
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     vfi_control - (OUT) Configuration.
 *
 * Returns:
 *      BCM_E_XXXs
 */
STATIC int
bcmi_td2p_vfi_control_egress_get(int unit, int vfi, uint32 valid_fields, bcm_vlan_control_vlan_t *control)
{

    int rv;
    int i;
    int value;
    soc_field_t field;
    egr_vfi_entry_t vfi_entry;
    soc_mem_t mem = EGR_VFIm;

    struct vfi_fields_s {
        soc_field_t field;
        uint32  valid_field;
        uint16    *vlan_control_obj;
    } vfi_field_values[] = {
            {OVIDf, BCM_VLAN_CONTROL_VLAN_ALL_MASK},
            {INVALIDf, 0, NULL}
    };

    vfi_field_values[0].vlan_control_obj = (uint16 *)&(control->egress_vlan);

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (i = 0; i < COUNTOF(vfi_field_values); i++) {
        field = vfi_field_values[i].field;
        if (!(valid_fields & vfi_field_values[i].valid_field) || (field == INVALIDf)) {
            continue;
        }
        value = soc_mem_field32_get(unit, mem, &vfi_entry, field);
        *(vfi_field_values[i].vlan_control_obj) = value;
    }


    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vfi, &vfi_entry);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}


/*
 * Function :
 *    bcmi_td2p_vlan_control_vpn_set
 *
 * Purpose  :
 *    Configures various controls on the VFI.
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id.
 *     valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (IN) Configuration structure.
 *
 * Return :
 *    BCM_E_XXX
 */
int
bcmi_td2p_vlan_control_vpn_set(int unit, bcm_vlan_t vid, uint32 valid_fields,
                               bcm_vlan_control_vlan_t *control)
{

    int rv;
    int vfi;

    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

    rv = bcmi_td2p_vfi_control_params_check(unit, vid, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    /* Update VFI & VI_PROFILE tables per vlan_control INPUT */
    rv = bcmi_td2p_vfi_control_ingress_set(unit, vfi, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    /* Update EGR_VFI table */
    rv = bcmi_td2p_vfi_control_egress_set(unit, vfi, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function :
 *    bcmi_td2p_vlan_control_vpn_get
 *
 * Purpose  :
 *
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     vid     - (IN) Vlan id.
 *     control - (OUT) Configuration structure.
 *
 * Return :
 *    BCM_E_XXX
 */
int
bcmi_td2p_vlan_control_vpn_get(int unit, bcm_vlan_t vid, uint32 valid_fields,
                               bcm_vlan_control_vlan_t *control)
{
    int rv;
    int vfi;

    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

    rv = bcmi_td2p_vfi_control_ingress_get(unit, vfi, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_td2p_vfi_control_egress_get(unit, vfi, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

#endif /* BCM_TRIDENT2PLUS_SUPPORT*/
