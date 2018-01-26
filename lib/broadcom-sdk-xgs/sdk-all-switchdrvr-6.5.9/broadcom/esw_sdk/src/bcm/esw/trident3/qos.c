/*
 * $Id: qos.c,v 1.88 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * All Rights Reserved.$
 *
 * QoS module
 */
#include <soc/defs.h>
#include <shared/bsl.h>
#include <assert.h>

#include <sal/core/libc.h>
#include <shared/util.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/qos.h>
#include <bcm/error.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/qos.h>

#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */

#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/switch.h>
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
#include <bcm_int/esw/mpls.h>
#endif /* INCLUDE_L3 && BCM_MPLS_SUPPORT */

#define TD3_EGR_MPLS_COMBO_MAP_ENTRY_SIZE    12
#define EGR_MPLS_COMBO_MAP_ENTRY_TOTAL       TD3_EGR_MPLS_COMBO_MAP_ENTRY_SIZE
#define EGR_OUTER_DOT1P_TABLE                4
#define EGR_MPLS_EXP_MEM_TABLE               9
#ifdef BCM_TRIDENT3_SUPPORT
#define _BCM_QOS_PROFILE_ENTRIES_PER_SET 64

soc_mem_t  egr_mpls_combo_map_info[BCM_MAX_NUM_UNITS][EGR_MPLS_COMBO_MAP_ENTRY_TOTAL];
int egr_mpls_combo_map_entry_total[BCM_MAX_NUM_UNITS];

void bcm_td3_egr_mpls_combo_map_info_init(uint32 unit)
{
    if (SOC_IS_TRIDENT3X(unit)) {
        egr_mpls_combo_map_entry_total[unit] = TD3_EGR_MPLS_COMBO_MAP_ENTRY_SIZE;
        egr_mpls_combo_map_info[unit][0] = EGR_MPLS_PRI_MAPPINGm;
        egr_mpls_combo_map_info[unit][1] = EGR_ZONE_3_DOT1P_MAPPING_TABLE_1m;
        egr_mpls_combo_map_info[unit][2] = EGR_ZONE_3_DOT1P_MAPPING_TABLE_2m;
        egr_mpls_combo_map_info[unit][3] = EGR_ZONE_3_DOT1P_MAPPING_TABLE_3m;
        egr_mpls_combo_map_info[unit][4] = EGR_ZONE_3_DOT1P_MAPPING_TABLE_4m;
        egr_mpls_combo_map_info[unit][5] = EGR_ZONE_1_DOT1P_MAPPING_TABLE_1m;
        egr_mpls_combo_map_info[unit][6] = EGR_ZONE_1_DOT1P_MAPPING_TABLE_2m;
        egr_mpls_combo_map_info[unit][7] = EGR_ZONE_1_DOT1P_MAPPING_TABLE_3m;
        egr_mpls_combo_map_info[unit][8] = EGR_ZONE_1_DOT1P_MAPPING_TABLE_4m;
        egr_mpls_combo_map_info[unit][9] = EGR_MPLS_EXP_MAPPING_1m;
        egr_mpls_combo_map_info[unit][10] = EGR_MPLS_EXP_MAPPING_2m;
        egr_mpls_combo_map_info[unit][11] = EGR_MPLS_EXP_MAPPING_3m;
    }
}

void bcm_td3_egr_mpls_combo_create_zone_entry(int unit,
                                              soc_mem_t mem, 
                                              int mems_cnt, 
                                              int *mem_words, 
                                              soc_mem_t *mems)
{
    if (SOC_MEM_IS_VALID(unit, mem)) {
        *(mems + mems_cnt) = mem;
        /* Since Zone memory are the same, we use first one as template */
        if (EGR_MPLS_PRI_MAPPINGm == mem) {
            *(mem_words + mems_cnt) = sizeof(egr_mpls_pri_mapping_entry_t) /
                                    sizeof(uint32);
	} else if ((EGR_MPLS_EXP_MAPPING_1m == mem) || 
                   (EGR_MPLS_EXP_MAPPING_2m == mem) ||
                   (EGR_MPLS_EXP_MAPPING_3m == mem)) {
            *(mem_words + mems_cnt) = sizeof(egr_mpls_exp_mapping_1_entry_t) /
                                    sizeof(uint32);
        } else {
            *(mem_words + mems_cnt) = sizeof(egr_zone_1_dot1p_mapping_table_1_entry_t) /
                                    sizeof(uint32);
	}
    }
}

void bcm_td3_egr_mpls_map_entry_default(int unit, soc_mem_t mem, uint32 *map_buf, uint32 entry_size)
{
    int int_pri;
    int cng;
    uint32 *buf;

    if ((EGR_MPLS_EXP_MAPPING_1m == mem) || 
        (EGR_MPLS_EXP_MAPPING_2m == mem) ||
        (EGR_MPLS_EXP_MAPPING_3m == mem)) {
       /* Those 3 memory don't need default */
       return;
    }

    if (SOC_MEM_IS_VALID(unit, mem)) {
        for (int_pri = 0; int_pri < 16; int_pri++) {
            for (cng = 0; cng < 4; cng++) {
	        /* This loops altogher 6 bits - yet we only support up to 4 bits */
                buf = (uint32 *)((char *)map_buf + (((int_pri << 2) | cng) * entry_size));
	        if (EGR_MPLS_PRI_MAPPINGm == mem) {
                    soc_mem_field32_set(unit, mem, buf, NEW_PRIf, int_pri > 7? 7: int_pri);
                    soc_mem_field32_set(unit, mem, buf, NEW_CFIf, cng > 1?1:cng);
		} else {
                    soc_mem_field32_set(unit, mem, buf, PRIf, int_pri > 7? 7: int_pri);
                    soc_mem_field32_set(unit, mem, buf, CFIf, cng > 1?1:cng);
                }
            }
        }
    }

    return;
}

int bcm_td3_egr_mpls_combo_map_create(uint32 unit, 
                                      int *egr_mpls_combo_map_created,
                                      soc_profile_mem_t **egr_mpls_combo_map)
{
    soc_mem_t mem;
    soc_mem_t mems[12];
    int mem_words[12];
    int ix;
    int rv = BCM_E_NONE;

    if (NULL == *egr_mpls_combo_map) {
        *egr_mpls_combo_map = sal_alloc(sizeof(soc_profile_mem_t),
                                             "MPLS Combo Profile Mem");
        if (*egr_mpls_combo_map == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(*egr_mpls_combo_map);

        /* Prevent entries from moving around in the tables */
        (*egr_mpls_combo_map)->flags = SOC_PROFILE_MEM_F_NO_SHARE;

        for (ix=0; ix < egr_mpls_combo_map_entry_total[unit]; ix++) {
            /* First memory entry of the combo */
            mem = egr_mpls_combo_map_info[unit][ix]; /* Read out mem ID from book keeping info */
            bcm_td3_egr_mpls_combo_create_zone_entry(unit, mem, ix, mem_words, mems);
        }

        rv = soc_profile_mem_create(unit, mems, mem_words, egr_mpls_combo_map_entry_total[unit],
                                        *egr_mpls_combo_map);
        if (BCM_FAILURE(rv)) {
            sal_free(*egr_mpls_combo_map);
            return rv;
        }
        *egr_mpls_combo_map_created = TRUE;
    }

    return rv;
}

int bcm_td3_egr_mpls_combo_map_default(uint32 unit)
{
    soc_mem_t mem;
    int ix;
    int rv = BCM_E_NONE;
    void *entries[12];
    uint32 index;
    uint32 *map_buf[TD3_EGR_MPLS_COMBO_MAP_ENTRY_SIZE] = {NULL}; 
    uint32 entry_size;
    uint32 alloc_size;

    /* Pick up the largest size among the 3 memory we are programming here */
    entry_size = sizeof(egr_mpls_pri_mapping_entry_t);
    if (entry_size < sizeof(egr_zone_3_dot1p_mapping_table_1_entry_t)) {
        entry_size = sizeof(egr_zone_3_dot1p_mapping_table_1_entry_t);
    } 

    if (entry_size < sizeof(egr_mpls_exp_mapping_1_entry_t)) {
       entry_size = sizeof(egr_mpls_exp_mapping_1_entry_t);
    }

    /* We default 64 entries */
    alloc_size = entry_size * 64;

    for (ix=0; ix < egr_mpls_combo_map_entry_total[unit]; ix++) {
        /* First memory entry of the combo */
        mem = egr_mpls_combo_map_info[unit][ix]; /* Read out mem ID from book keeping info */

        map_buf[ix] = sal_alloc(alloc_size, "TD3 EGR MPLS map");
        if (map_buf[ix] == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(map_buf[ix], 0, alloc_size);
        bcm_td3_egr_mpls_map_entry_default(unit, mem, map_buf[ix], entry_size);
        entries[ix] = (void *)map_buf[ix];
    }

    rv = _bcm_egr_mpls_combo_map_entry_add(unit, entries, 64, &index);

cleanup:
    for (ix=0; ix < egr_mpls_combo_map_entry_total[unit]; ix++) {
       if (map_buf[ix] != NULL) {
           sal_free(map_buf[ix]);
       }
    }
   
    return rv;
}


int bcm_td3_egr_mpls_combo_dump(int unit, soc_profile_mem_t *mpls_profile)
{
    egr_mpls_pri_mapping_entry_t egr_mpls_pri_map[_BCM_QOS_PROFILE_ENTRIES_PER_SET];
    egr_zone_1_dot1p_mapping_table_1_entry_t  egr_mpls_zone_map[8][_BCM_QOS_PROFILE_ENTRIES_PER_SET];
    egr_mpls_exp_mapping_1_entry_t  egr_mpls_exp_map[3][_BCM_QOS_PROFILE_ENTRIES_PER_SET];
    int ix, jx;
    void *entries[12];
    int pri, cfi, exp, new_pri, new_cfi;
    soc_mem_t mem;
    int num_entries;
    int rv = BCM_E_NONE;
    int entries_per_set;
    int ref_count;

    mem = EGR_MPLS_PRI_MAPPINGm;
    num_entries = soc_mem_index_count(unit, mem);

    LOG_CLI((BSL_META_U(unit,
                  "  EGR_MPLS_PRI_MAPPING & EGR_MPLS_EXP_MAP & EGR ZONE_1/3 PRI Combo Mem\n")));
    LOG_CLI((BSL_META_U(unit,
                 "    Number of entries: %d\n"), num_entries));
    LOG_CLI((BSL_META_U(unit,
                 "    Index RefCount EntriesPerSet - PRI CFI MPLS_EXP\n")));

    for (jx = 0; jx < num_entries;
        jx += _BCM_QOS_PROFILE_ENTRIES_PER_SET) {
        rv = soc_profile_mem_ref_count_get(unit,
                                           mpls_profile,
                                           jx, &ref_count);
        if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META_U(unit,
                        " *** Error retrieving profile reference: %d ***\n"), rv));
            break;
        }

        if (ref_count <= 0) {
            continue;
        }

        /* Add new chunk and store new HW index */
        entries[0] = &egr_mpls_pri_map;
        for (ix=0; ix < 8; ix++) {
            entries[ix+1] = &egr_mpls_zone_map[ix];
        }

        for (ix=0; ix < 3; ix++) {
            entries[ix+EGR_MPLS_EXP_MEM_TABLE] = &egr_mpls_exp_map[ix];
        }

        rv = soc_profile_mem_get(unit, mpls_profile,
                                 jx, _BCM_QOS_PROFILE_ENTRIES_PER_SET, entries);
        if (SOC_FAILURE(rv)) {
             LOG_CLI((BSL_META_U(unit,
                                " *** Error retrieving profile data: %d ***\n"), rv));
             break;
        }

        for (ix = 0; ix < _BCM_QOS_PROFILE_ENTRIES_PER_SET; ix++) {
            entries_per_set = mpls_profile->tables[0].entries[jx+ix].entries_per_set;

            new_pri = soc_mem_field32_get(unit, EGR_MPLS_PRI_MAPPINGm,
                                         &(egr_mpls_pri_map[ix]),
                                         NEW_PRIf);
            new_cfi = soc_mem_field32_get(unit, EGR_MPLS_PRI_MAPPINGm,
                                          &(egr_mpls_pri_map[ix]),
                                          NEW_CFIf);
            cfi = -1;
            pri = -1;
            exp = -1;

            cfi = soc_mem_field32_get(unit, EGR_ZONE_3_DOT1P_MAPPING_TABLE_3m,
                                      &(egr_mpls_zone_map[0][ix]),
                                      CFIf);
            pri = soc_mem_field32_get(unit, EGR_ZONE_3_DOT1P_MAPPING_TABLE_3m,
                                      &(egr_mpls_zone_map[0][ix]),
                                      PRIf);

            exp = soc_mem_field32_get(unit, EGR_MPLS_EXP_MAPPING_1m,
                                      &(egr_mpls_exp_map[0][ix]),
                                      MPLS_EXPf);
            LOG_CLI((BSL_META_U(unit,
                                "  %5d %8d %13d     %2d  %2d  %2d %2d %2d"),
                                ix + jx, ref_count, entries_per_set,
                                pri, cfi, new_pri, new_cfi, exp));
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
    }

    return rv;
}


int bcm_td3_l2_egr_combo_add(int unit,
                             uint32 flags,
                             bcm_qos_map_t *map, 
                             int map_id,
                             uint32 *hw_idx,
                             uint32 entry_per_index)
{
    egr_mpls_pri_mapping_entry_t *egr_mpls_pri_map = NULL;
    egr_zone_1_dot1p_mapping_table_1_entry_t  *egr_mpls_zone_map[8]={NULL};
    egr_mpls_exp_mapping_1_entry_t  *egr_mpls_exp_map[3] = {NULL};
    int ix;
    void *entries[12], *entry;
    int alloc_size, offset, cng, index;
    int rv = BCM_E_NONE;


    if (!((flags & BCM_QOS_MAP_L2) ||
         (flags & BCM_QOS_MAP_L2_INNER_TAG)||
         (flags & BCM_QOS_MAP_MPLS))||
         !(flags & BCM_QOS_MAP_EGRESS)) {
         return BCM_E_PARAM;
    }

    if ((map->int_pri > 15) || (map->int_pri < 0) || 
        (map->pkt_pri > 7) || (map->pkt_cfi > 1) || 
        ((map->color != bcmColorGreen) && 
        (map->color != bcmColorYellow) && 
        (map->color != bcmColorRed))) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_QOS_MAP_MPLS) {
        if ((map->exp > 7) || (map->exp < 0)) {
             return BCM_E_PARAM;
        }
    }

    /* Base index of table in hardware */
    index = (*hw_idx * entry_per_index);
    
    /* We total have 64 entries, based on the given CNG and PRI, 
     * Map to only one of them */
    /* Offset within table */
    cng = _BCM_COLOR_ENCODING(unit, map->color);
    offset = (map->int_pri << 2) | cng;

    /* Allocate memory for DMA */
    /* For table EGR_MPLS_PRI_MAP */
    alloc_size = (entry_per_index * sizeof(egr_mpls_pri_mapping_entry_t));
    egr_mpls_pri_map = soc_cm_salloc(unit, alloc_size,
                                             "TD3 egr mpls pri map");
    if (NULL == egr_mpls_pri_map) {
         return (BCM_E_MEMORY);
    }

    sal_memset(egr_mpls_pri_map, 0, alloc_size);
 
    /* Read the old profile chunk */
    rv = soc_mem_read_range(unit, EGR_MPLS_PRI_MAPPINGm, MEM_BLOCK_ANY,
                            index,
                            (index + (entry_per_index -1)),
                            egr_mpls_pri_map);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Modify what's needed for EGR_MPLS_PRI_MAPPINGm */
    entry = &egr_mpls_pri_map[offset];
    soc_mem_field32_set(unit, EGR_MPLS_PRI_MAPPINGm, entry,
                        NEW_PRIf, map->pkt_pri);
    soc_mem_field32_set(unit, EGR_MPLS_PRI_MAPPINGm, entry,
                        NEW_CFIf, map->pkt_cfi);


    /* For table
     *  EGR_ZONE_1_DOT1P_MAPPING_TABLE_1(2, 3,4)
     *  EGR_ZONE_1_DOT1P_MAPPING_TABLE_1(2, 3,4)
     */
    for (ix=0; ix < 8; ix++) {
        /* Allocate memory */
        alloc_size = (entry_per_index * 
                      sizeof(egr_zone_1_dot1p_mapping_table_1_entry_t));
        egr_mpls_zone_map[ix] = soc_cm_salloc(unit, alloc_size,
                                             "TD3 egr mpls zone map");
        if (NULL == egr_mpls_zone_map[ix]) {
            goto cleanup;
        }
        sal_memset(egr_mpls_zone_map[ix], 0, alloc_size);

        /* Read out old entry */
        if (SOC_MEM_IS_VALID(unit, egr_mpls_combo_map_info[unit][ix+1])) {
            rv = soc_mem_read_range(unit, egr_mpls_combo_map_info[unit][ix+1],
                                    MEM_BLOCK_ANY,
                                    index,
                                     (index + (entry_per_index -1)),
                                    egr_mpls_zone_map[ix]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
        
        /* Modify for OUTER VLAN remarking */
        entry = &egr_mpls_zone_map[ix][offset]; 
        if (flags & BCM_QOS_MAP_L2_OUTER_TAG ) {
            if (ix > EGR_OUTER_DOT1P_TABLE) {
               soc_mem_field32_set(unit, egr_mpls_combo_map_info[unit][ix+1], entry,
                                        PRIf, map->pkt_pri);
               soc_mem_field32_set(unit, egr_mpls_combo_map_info[unit][ix+1], entry,
                                        CFIf, map->pkt_cfi);
            }
        }
  
        if (flags & BCM_QOS_MAP_L2_INNER_TAG ) {
           if (ix < EGR_OUTER_DOT1P_TABLE) {
               soc_mem_field32_set(unit, egr_mpls_combo_map_info[unit][ix+1], entry,
                                   PRIf, map->pkt_pri);
               soc_mem_field32_set(unit, egr_mpls_combo_map_info[unit][ix+1], entry,
                                   CFIf, map->pkt_cfi);
           }
        }
    }

    for (ix=0; ix < 3; ix++) {
        /* Allocate memory */
        alloc_size = (entry_per_index *
                      sizeof(egr_mpls_exp_mapping_1_entry_t));
        egr_mpls_exp_map[ix] = soc_cm_salloc(unit, alloc_size,
                                             "TD3 egr mpls exp map");
        if (NULL == egr_mpls_exp_map[ix]) {
            goto cleanup;
        }
        sal_memset(egr_mpls_exp_map[ix], 0, alloc_size);

        /* Read out old entry */
        if (SOC_MEM_IS_VALID(unit, egr_mpls_combo_map_info[unit][ix+EGR_MPLS_EXP_MEM_TABLE])) {
            rv = soc_mem_read_range(unit, egr_mpls_combo_map_info[unit][ix+EGR_MPLS_EXP_MEM_TABLE],
                                    MEM_BLOCK_ANY,
                                    index,
                                     (index + (entry_per_index -1)),
                                    egr_mpls_exp_map[ix]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
      
        /* Modify entry */
        entry = &egr_mpls_exp_map[ix][offset];
        if (flags & BCM_QOS_MAP_MPLS) {
            soc_mem_field32_set(unit, egr_mpls_combo_map_info[unit][ix+EGR_MPLS_EXP_MEM_TABLE], 
                               entry, MPLS_EXPf, map->exp);
        }
    }

    /* Delete the old profile chunk */
    rv = _bcm_egr_mpls_combo_map_entry_delete(unit, index);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Add new chunk and store new HW index */
    entries[0] = egr_mpls_pri_map;
    for (ix=0; ix < 8; ix++) {
        entries[ix+1] = egr_mpls_zone_map[ix];
    }
    for (ix=0; ix < 3; ix++) {
        entries[ix+EGR_MPLS_EXP_MEM_TABLE] = egr_mpls_exp_map[ix];
    }
    rv = _bcm_egr_mpls_combo_map_entry_add(unit, entries,
                                            entry_per_index, 
                                             (uint32 *)&index);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    *hw_idx = (index / entry_per_index);

    cleanup:
        /* Free the DMA memory */
        if (egr_mpls_pri_map != NULL) {
            soc_cm_sfree(unit, egr_mpls_pri_map);
        }

        for (ix=0; ix<8; ix++) {
           if (egr_mpls_zone_map[ix] != NULL) {
               soc_cm_sfree(unit, egr_mpls_zone_map[ix]);
           }
        }
       for (ix=0; ix < 3; ix++) {
           if (egr_mpls_exp_map[ix] != NULL) {
               soc_cm_sfree(unit, egr_mpls_exp_map[ix]);
           }
       }

    return rv;
}

int bcm_td3_qos_map_create(uint32 unit, uint32 *index, int entry_per_index)
{
    egr_mpls_pri_mapping_entry_t *egr_mpls_pri_map = NULL;
    egr_zone_1_dot1p_mapping_table_1_entry_t  *egr_mpls_zone_map[8] = {NULL};
    egr_mpls_exp_mapping_1_entry_t  *egr_mpls_exp_map[3]={NULL};
    int ix;
    void *entries[12];
    int rv = BCM_E_NONE;
    int alloc_size;
   
    /* Entries for EGR_MPLS_PRI_MAP */
    alloc_size = (entry_per_index * sizeof(egr_mpls_pri_mapping_entry_t));
    egr_mpls_pri_map = soc_cm_salloc(unit, alloc_size,
                                             "TD3 egr mpls pri map");
    if (NULL == egr_mpls_pri_map) {
         return (BCM_E_MEMORY);
    }
 
    sal_memset(egr_mpls_pri_map, 0, alloc_size);
   
    /* Entries for Zone 1 and Zone 3 table */
    for (ix=0; ix < 8; ix++) {
        /* Allocate memory */
        alloc_size = (entry_per_index *
                      sizeof(egr_zone_1_dot1p_mapping_table_1_entry_t));
        egr_mpls_zone_map[ix] = soc_cm_salloc(unit, alloc_size,
                                             "TD3 egr mpls zone map");
        if (NULL == egr_mpls_zone_map[ix]) {
            goto cleanup;
        }
        sal_memset(egr_mpls_zone_map[ix], 0, alloc_size);
    }

    /* Entries for EGR_MPLS_EXP_MAPPING_1&2*/
    for (ix=0; ix < 3; ix++) {
        /* Allocate memory */
        alloc_size = (entry_per_index *
                      sizeof(egr_mpls_exp_mapping_1_entry_t));
        egr_mpls_exp_map[ix] = soc_cm_salloc(unit, alloc_size,
                                             "TD3 egr mpls exp map");
        if (NULL == egr_mpls_exp_map[ix]) {
            goto cleanup;
        }
        sal_memset(egr_mpls_exp_map[ix], 0, alloc_size);
    }

    /* Add new chunk and store new HW index */
    entries[0] = egr_mpls_pri_map;
    for (ix=0; ix < 8; ix++) {
        entries[ix+1] = egr_mpls_zone_map[ix];
    }
    for (ix=0; ix < 3; ix++) {
        entries[ix+EGR_MPLS_EXP_MEM_TABLE] = egr_mpls_exp_map[ix];
    }
    rv = _bcm_egr_mpls_combo_map_entry_add(unit, entries,
                                            entry_per_index, index);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    return rv;
  
cleanup:
    /* Free the DMA memory */
    if (egr_mpls_pri_map != NULL) {
        soc_cm_sfree(unit, egr_mpls_pri_map);
    } 

    for (ix=0; ix<8; ix++) {
       if (egr_mpls_zone_map[ix] != NULL) {
           soc_cm_sfree(unit, egr_mpls_zone_map[ix]);
       }
    }
    for (ix=0; ix < 3; ix++) {
       if (egr_mpls_exp_map[ix] != NULL) {
           soc_cm_sfree(unit, egr_mpls_exp_map[ix]);
       }
    }

   return rv;
}

#endif /* End of BCM_TRIDENT3_SUPPORT */

