/*
 * $Id: trx_common.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * File:    common.c
 * Purpose: Manages resources shared across multiple modules
 */
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/mcm/allenum.h>
#include <soc/profile_mem.h>

#include <sal/compiler.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trx.h>

typedef struct _bcm_common_bookkeeping_s {
    int initialized;            /* Flag to check initialized status */
    int mac_da_profile_created; /* Flag to check MAC_DA profile creation */
    int lport_profile_created;  /* Flag to check LPORT profile creation */
    int ing_pri_cng_map_created; /* Flag to check ING_PRI_CNG_MAP creation */
    int egr_mpls_pri_map_created; /* Flag to check EGR_MPLS_PRI_MAPPING creation */    
    int egr_mpls_exp_map_created; /* Flag to check EGR_MPLS_EXP_MAPPING_1 creation */
    int dscp_table_created; /* Flag to check DSCP_TABLE creation */
    int egr_dscp_table_created; /* Flag to check EGR_DSCP_TABLE creation */
    soc_profile_mem_t *mac_da_profile; /* cache of EGR_MAC_DA_PROFILE */
    soc_profile_mem_t *lport_profile;  /* cache of LPORT_TABLE (profile) */
    soc_profile_mem_t *ing_pri_cng_map;  /* cache of ING_PRI_CNG_MAP (profile) */
    soc_profile_mem_t *egr_mpls_pri_map;  /* cache of EGR_MPLS_PRI_MAPPING */
    soc_profile_mem_t *egr_mpls_exp_map;  /* cache of EGR_MPLS_EXP_MAPPING_1 */
    soc_profile_mem_t *dscp_table;  /* cache of DSCP_TABLE (profile) */
    soc_profile_mem_t *egr_dscp_table;  /* cache of EGR_DSCP_TABLE (profile) */
    uint16      *mpls_station_hash; /* Hash values for MPLS STATION entries */
} _bcm_common_bookkeeping_t;

STATIC _bcm_common_bookkeeping_t  _bcm_common_bk_info[BCM_MAX_NUM_UNITS] = {{ 0 }};
STATIC sal_mutex_t _common_mutex[BCM_MAX_NUM_UNITS] = {NULL};

#define COMMON_INFO(_unit_)   (&_bcm_common_bk_info[_unit_])
#define MAC_DA_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].mac_da_profile)
#define LPORT_PROFILE(_unit_) (_bcm_common_bk_info[_unit_].lport_profile)
#define ING_PRI_CNG_MAP(_unit_) (_bcm_common_bk_info[_unit_].ing_pri_cng_map)
#define EGR_MPLS_PRI_MAPPING(_unit_) (_bcm_common_bk_info[_unit_].egr_mpls_pri_map)
#define EGR_MPLS_EXP_MAPPING_1(_unit_) (_bcm_common_bk_info[_unit_].egr_mpls_exp_map)
#define DSCP_TABLE(_unit_) (_bcm_common_bk_info[_unit_].dscp_table)
#define EGR_DSCP_TABLE(_unit_) (_bcm_common_bk_info[_unit_].egr_dscp_table)
#define MPLS_STATION_HASH(_unit_, _index_) \
        (_bcm_common_bk_info[_unit_].mpls_station_hash[_index_])

/* 
 * Common resource lock
 */
#define COMMON_LOCK(unit) \
        sal_mutex_take(_common_mutex[unit], sal_mutex_FOREVER); 

#define COMMON_UNLOCK(unit) \
        sal_mutex_give(_common_mutex[unit]); 

/*
 * Function:
 *      _bcm_common_free_resource
 * Purpose:
 *      Free all allocated tables and memory
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_common_free_resource(int unit, _bcm_common_bookkeeping_t *info)
{
    if (!info) {
        return;
    }
    if (_common_mutex[unit]) {
        sal_mutex_destroy(_common_mutex[unit]);
        _common_mutex[unit] = NULL;
    } 
    if (info->mpls_station_hash) {
        sal_free(info->mpls_station_hash);
        info->mpls_station_hash = NULL;
    }
    if (info->mac_da_profile) {
        if (info->mac_da_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->mac_da_profile);
        }
        sal_free(info->mac_da_profile);
        info->mac_da_profile = NULL;
    }
    if (info->lport_profile) {
        if (info->lport_profile_created) {
            (void) soc_profile_mem_destroy(unit, info->lport_profile);
        }
        sal_free(info->lport_profile);
        info->lport_profile = NULL;
    }
    if (info->ing_pri_cng_map) {
        if (info->ing_pri_cng_map_created) {
            (void) soc_profile_mem_destroy(unit, info->ing_pri_cng_map);
        }
        sal_free(info->ing_pri_cng_map);
        info->ing_pri_cng_map = NULL;
    }
    if (info->egr_mpls_pri_map) {
        if (info->egr_mpls_pri_map_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_mpls_pri_map);
        }
        sal_free(info->egr_mpls_pri_map);
        info->egr_mpls_pri_map = NULL;
    }
    if (info->egr_mpls_exp_map) {
        if (info->egr_mpls_exp_map_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_mpls_exp_map);
        }
        sal_free(info->egr_mpls_exp_map);
        info->egr_mpls_exp_map = NULL;
    }
    if (info->dscp_table) {
        if (info->dscp_table_created) {
            (void) soc_profile_mem_destroy(unit, info->dscp_table);
        }
        sal_free(info->dscp_table);
        info->dscp_table = NULL;
    }
    if (info->egr_dscp_table) {
        if (info->egr_dscp_table_created) {
            (void) soc_profile_mem_destroy(unit, info->egr_dscp_table);
        }
        sal_free(info->egr_dscp_table);
        info->egr_dscp_table = NULL;
    }
    return;
}

/*
 * Function:
 *      _bcm_common_cleanup
 * Purpose:
 *      Internal function for initializing common resource management
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
int _bcm_common_cleanup(int unit)
{
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    _bcm_common_free_resource(unit, info);
    info->mac_da_profile_created = FALSE;
    info->lport_profile_created = FALSE;
    info->ing_pri_cng_map_created = FALSE;
    info->egr_mpls_pri_map_created = FALSE;
    info->egr_mpls_exp_map_created = FALSE;
    info->dscp_table_created = FALSE;
    info->egr_dscp_table_created = FALSE;
    info->initialized = FALSE;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_common_init
 * Purpose:
 *      Internal function for initializing common resource management
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      BCM_X_XXX
 */
int _bcm_common_init(int unit)
{
    int num_station, rv = BCM_E_NONE;
    soc_mem_t mem;
    _bcm_common_bookkeeping_t *info = COMMON_INFO(unit);
    
    num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);

    if (info->initialized) {
        BCM_IF_ERROR_RETURN(_bcm_common_cleanup(unit));
    }

    /* Create mutex */
    if (NULL == _common_mutex[unit]) {
        _common_mutex[unit] = sal_mutex_create("common mutex");
        if (_common_mutex[unit] == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }
    }

    /* MPLS station hash */
    if (NULL == info->mpls_station_hash) {
        info->mpls_station_hash =
                sal_alloc(sizeof(uint16) * num_station, "mpls station hash");
        if (info->mpls_station_hash == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(info->mpls_station_hash, 0, sizeof(uint16) * num_station);

    /* Create profile table cache (or re-init if it already exists) */
    mem = EGR_MAC_DA_PROFILEm;
    if (NULL == info->mac_da_profile) {
        info->mac_da_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                         "MAC DA Profile Mem");
        if (info->mac_da_profile == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(info->mac_da_profile);

        rv = soc_profile_mem_create(unit, &mem, 1, info->mac_da_profile);
        info->mac_da_profile_created = TRUE;
        if (rv < 0) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }
    }

    /* Create profile table cache (or re-init if it already exists) */
    mem = LPORT_TABm;
    if (NULL == info->lport_profile && 
        soc_feature(unit, soc_feature_lport_tab_profile)) {
        info->lport_profile = sal_alloc(sizeof(soc_profile_mem_t),
                                         "LPORT_PROFILE_TABLE Mem");
        if (info->lport_profile == NULL) {
            _bcm_common_free_resource(unit, info);
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(info->lport_profile);

        rv = soc_profile_mem_create(unit, &mem, 1, info->lport_profile);
        info->lport_profile_created = TRUE;
        if (rv < 0) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }

        rv = _bcm_trx_lport_tab_default_entry_add(unit, info->lport_profile);
        if (BCM_FAILURE(rv)) {
            _bcm_common_free_resource(unit, info);
            return rv;
        }
    }
	
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
         /* Create profile table cache (or re-init if it already exists) */
         mem = ING_PRI_CNG_MAPm;
         if (NULL == info->ing_pri_cng_map) {
              info->ing_pri_cng_map = sal_alloc(sizeof(soc_profile_mem_t),
                                         "ING_PRI_CNG_MAP Profile Mem");
              if (info->ing_pri_cng_map == NULL) {
                   _bcm_common_free_resource(unit, info);
                   return BCM_E_MEMORY;
              }
              soc_profile_mem_t_init(info->ing_pri_cng_map);

              rv = soc_profile_mem_create(unit, &mem, 1, info->ing_pri_cng_map);
              info->ing_pri_cng_map_created = TRUE;
              if (rv < 0) {
                   _bcm_common_free_resource(unit, info);
                   return rv;
              }
         }
         mem = EGR_MPLS_PRI_MAPPINGm;
         if (NULL == info->egr_mpls_pri_map) {
              info->egr_mpls_pri_map = sal_alloc(sizeof(soc_profile_mem_t),
                                         "EGR_MPLS_PRI_MAPPING Profile Mem");
              if (info->egr_mpls_pri_map == NULL) {
                   _bcm_common_free_resource(unit, info);
                   return BCM_E_MEMORY;
              }
              soc_profile_mem_t_init(info->egr_mpls_pri_map);

              rv = soc_profile_mem_create(unit, &mem, 1, info->egr_mpls_pri_map);
              info->egr_mpls_pri_map_created = TRUE;
              if (rv < 0) {
                   _bcm_common_free_resource(unit, info);
                   return rv;
              }
         }
         mem = EGR_MPLS_EXP_MAPPING_1m;
         if (NULL == info->egr_mpls_exp_map) {
              info->egr_mpls_exp_map = sal_alloc(sizeof(soc_profile_mem_t),
                                         "EGR_MPLS_EXP_MAPPING_1 Profile Mem");
              if (info->egr_mpls_exp_map == NULL) {
                   _bcm_common_free_resource(unit, info);
                   return BCM_E_MEMORY;
              }
              soc_profile_mem_t_init(info->egr_mpls_exp_map);

              rv = soc_profile_mem_create(unit, &mem, 1, info->egr_mpls_exp_map);
              info->egr_mpls_exp_map_created = TRUE;
              if (rv < 0) {
                   _bcm_common_free_resource(unit, info);
                   return rv;
              }
         }
         mem = DSCP_TABLEm;
         if (NULL == info->dscp_table) {
              info->dscp_table = sal_alloc(sizeof(soc_profile_mem_t),
                                         "DSCP_TABLE Profile Mem");
              if (info->dscp_table == NULL) {
                   _bcm_common_free_resource(unit, info);
                   return BCM_E_MEMORY;
              }
              soc_profile_mem_t_init(info->dscp_table);

              rv = soc_profile_mem_create(unit, &mem, 1, info->dscp_table);
              info->dscp_table_created = TRUE;
              if (rv < 0) {
                   _bcm_common_free_resource(unit, info);
                   return rv;
              }
         }
         mem = EGR_DSCP_TABLEm;
         if (NULL == info->egr_dscp_table) {
              info->egr_dscp_table = sal_alloc(sizeof(soc_profile_mem_t),
                                         "EGR_DSCP_TABLE Profile Mem");
              if (info->egr_dscp_table == NULL) {
                   _bcm_common_free_resource(unit, info);
                   return BCM_E_MEMORY;
              }
              soc_profile_mem_t_init(info->egr_dscp_table);

              rv = soc_profile_mem_create(unit, &mem, 1, info->egr_dscp_table);
              info->egr_dscp_table_created = TRUE;
              if (rv < 0) {
                   _bcm_common_free_resource(unit, info);
                   return rv;
              }
         }
         rv = _bcm_tr2_ing_pri_cng_map_default_entry_add
                     (unit, info->ing_pri_cng_map);
         if (BCM_FAILURE(rv)) {
                _bcm_common_free_resource(unit, info);
                return rv;
         }
    }	
#endif
    info->initialized = TRUE;
    return rv;
}

/*
 * Function:
 *      _bcm_mac_da_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the MAC_DA_PROFILE table
 *      Adds an entry to the global shared SW copy of the MAC_DA_PROFILE table
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_mac_da_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, MAC_DA_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_mac_da_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the MAC_DA_PROFILE table
 *      Deletes an entry from the global shared SW copy of the MAC_DA_PROFILE table
 *      Shared by MPLS and MIM
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_mac_da_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, MAC_DA_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_entry_add
 * Purpose:
 *      Internal function for adding an entry to the LPORT_PROFILE_TABLE
 *      Adds an entry to the global shared SW copy of the LPORT_PROFILE_TABLE
 *      Shared by WLAN and proxy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_lport_profile_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, LPORT_PROFILE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_lport_profile_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the LPORT_PROFILE_TABLE
 *      Deletes an entry from the global shared SW copy of the LPORT_PROFILE_TABLE
 *      Shared by WLAN and proxy
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_lport_profile_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, LPORT_PROFILE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_pri_cng_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the ING_PRI_CNG_MAP table
 *      Adds an entry to the global shared SW copy of the ING_PRI_CNG_MAP table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_pri_cng_map_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, ING_PRI_CNG_MAP(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_ing_pri_cng_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the ING_PRI_CNG_MAP table
 *      Deletes an entry from the global shared SW copy of the ING_PRI_CNG_MAP table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_ing_pri_cng_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, ING_PRI_CNG_MAP(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_pri_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_MPLS_PRI_MAPPING table
 *      Adds an entry to the global shared SW copy of the EGR_MPLS_PRI_MAPPING table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_pri_map_entry_add(int unit, void **entries, 
                                int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_MPLS_PRI_MAPPING(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_pri_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_MPLS_PRI_MAPPING table
 *      Deletes an entry from the global shared SW copy of the EGR_MPLS_PRI_MAPPING table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_pri_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_MPLS_PRI_MAPPING(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_exp_map_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_MPLS_EXP_MAPPING_1 table
 *      Adds an entry to the global shared SW copy of the EGR_MPLS_EXP_MAPPING_1 table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_exp_map_entry_add(int unit, void **entries, 
                                int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_MPLS_EXP_MAPPING_1(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_mpls_exp_map_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_MPLS_EXP_MAPPING_1 table
 *      Deletes an entry from the global shared SW copy of the EGR_MPLS_EXP_MAPPING_1 table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_mpls_exp_map_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_MPLS_EXP_MAPPING_1(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_dscp_table_entry_add
 * Purpose:
 *      Internal function for adding an entry to the DSCP_TABLE table
 *      Adds an entry to the global shared SW copy of the DSCP_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_dscp_table_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, DSCP_TABLE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_dscp_table_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the DSCP_TABLE table
 *      Deletes an entry from the global shared SW copy of the DSCP_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_dscp_table_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, DSCP_TABLE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_dscp_table_entry_add
 * Purpose:
 *      Internal function for adding an entry to the EGR_DSCP_TABLE table
 *      Adds an entry to the global shared SW copy of the EGR_DSCP_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_dscp_table_entry_add(int unit, void **entries, 
                              int entries_per_set, uint32 *index)
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_add(unit, EGR_DSCP_TABLE(unit), entries,
                             entries_per_set, index);
    COMMON_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_egr_dscp_table_entry_delete
 * Purpose:
 *      Internal function for deleting an entry from the EGR_DSCP_TABLE table
 *      Deletes an entry from the global shared SW copy of the EGR_DSCP_TABLE table
 *      Shared by several applications.
 * Parameters:
 *      unit    -  (IN) Device number.
 *      entries -  (IN) Array of pointer to table entries set
 *      entries_per_set - (IN)  Number of entries in the set
 *      index   -  (OUT) Base index for the entires allocated in HW
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_egr_dscp_table_entry_delete(int unit, int index) 
{
    int rv;
    COMMON_LOCK(unit);
    rv = soc_profile_mem_delete(unit, EGR_DSCP_TABLE(unit), index);
    COMMON_UNLOCK(unit);
    return rv;
}

/* Structure used for hash calculation */
typedef struct _bcm_trx_mpls_station_key_s {
    bcm_mac_t  mac;
    bcm_vlan_t vlan;
} _bcm_trx_mpls_station_key_t;

/*
 * Function:
 *      _bcm_trx_mpls_station_hash_calc
 * Purpose:
 *      Calculate MPLS_STATION_TCAM entry hash.
 * Parameters:
 *      unit -  (IN)  Device number.
 *      mac  -  (IN)  MAC address
 *      vlan -  (IN)  VLAN ID
 *      hash -  (OUT) Hash valu.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_trx_mpls_station_hash_calc(int unit, bcm_mac_t mac,
                                bcm_vlan_t vlan, uint16 *hash)
{
    _bcm_trx_mpls_station_key_t key;

    if (hash == NULL) {
        return (BCM_E_PARAM);
    }
    sal_memcpy(key.mac, mac, sizeof(bcm_mac_t));
    key.vlan = vlan;
    *hash = _shr_crc16(0, (uint8 *)&key, sizeof(_bcm_trx_mpls_station_key_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_l2_tunnel_add
 * Purpose:
 *    Add a (MAC, VLAN) for tunnel/MPLS processing.
 *    Frames destined to (MAC, VLAN) is subjected to MPLS processing.
 *    Shared by MPLS and MIM
 * Parameters:
 *      unit - Unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */
int
bcm_trx_metro_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    mpls_station_tcam_entry_t station_entry;
    uint16 hash;
    bcm_mac_t hw_mac;
    bcm_vlan_t hw_vlan;
    int rv, i, num_station, free_index = -1;

    /* Get the hash value for this key */
    BCM_IF_ERROR_RETURN(_bcm_trx_mpls_station_hash_calc(unit, mac, vlan, &hash));

    /* see if the entry already exists or find free entry */
    num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
    COMMON_LOCK(unit);
    for (i = 0; i < num_station; i++) {
        if (hash == MPLS_STATION_HASH(unit, i)) {
            /* Read HW entry to see if it's really a match */
            rv = READ_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY,
                                         i, &station_entry);
            if (rv < 0) {
                COMMON_UNLOCK(unit);
                return rv;
            }
            soc_mem_mac_addr_get(unit, MPLS_STATION_TCAMm,
                                 &station_entry, MAC_ADDRf, hw_mac);
            hw_vlan = soc_mem_field32_get(unit, MPLS_STATION_TCAMm,
                                          &station_entry, VLAN_IDf);
            if ((vlan == hw_vlan) &&
                !sal_memcmp(mac, hw_mac, sizeof(bcm_mac_t))) {
                break;
            }
        }
        if ((free_index == -1) && (MPLS_STATION_HASH(unit, i) == 0)) {
            /* Read HW entry to see if it's unused */
            rv = READ_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY,
                                         i, &station_entry);
            if (rv < 0) {
                COMMON_UNLOCK(unit);
                return rv;
            }
            if (!soc_mem_field32_get(unit, MPLS_STATION_TCAMm,
                                     &station_entry, VALIDf)) {
                free_index = i;
            }
        }
    }
    if (i < num_station) {
        /* Entry already exists */
        COMMON_UNLOCK(unit);
        return BCM_E_EXISTS;
    } else if (free_index == -1) {
        /* Table is full */
        COMMON_UNLOCK(unit);
        return BCM_E_FULL;
    }
    /* Commit entry to HW */
    sal_memset(&station_entry, 0, sizeof(mpls_station_tcam_entry_t));
    soc_mem_field32_set(unit, MPLS_STATION_TCAMm, &station_entry, VALIDf, 1);
    soc_mem_mac_addr_set(unit, MPLS_STATION_TCAMm, &station_entry,
                         MAC_ADDRf, mac);
    soc_mem_field32_set(unit, MPLS_STATION_TCAMm, &station_entry,
                        VLAN_IDf, vlan);
    /* set masks to all 1's */
    for (i = 0; i < 6; i++) {
        hw_mac[i] = 0xff;
    }
    hw_vlan = 0xfff;
    soc_mem_mac_addr_set(unit, MPLS_STATION_TCAMm, &station_entry,
                         MAC_ADDR_MASKf, hw_mac);
    soc_mem_field32_set(unit, MPLS_STATION_TCAMm, &station_entry,
                        VLAN_ID_MASKf, hw_vlan);
    rv = WRITE_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY,
                                  free_index, &station_entry);
    if (rv < 0) {
        COMMON_UNLOCK(unit);
        return rv;
    }
    MPLS_STATION_HASH(unit, free_index) = hash;
    COMMON_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_l2_tunnel_delete
 * Purpose:
 *    Delete a (MAC, VLAN) for tunnel/MPLS processing.
 *    Shared by MPLS and MIM
 * Parameters:
 *      unit - Unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */
int
bcm_trx_metro_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
    mpls_station_tcam_entry_t station_entry;
    uint16 hash;
    bcm_mac_t hw_mac;
    bcm_vlan_t hw_vlan;
    int rv, i, num_station;

    /* Get the hash value for this key */
    BCM_IF_ERROR_RETURN(_bcm_trx_mpls_station_hash_calc(unit, mac, vlan, &hash));

    /* find the entry to be deleted */
    num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
    COMMON_LOCK(unit);
    for (i = 0; i < num_station; i++) {
        if (hash == MPLS_STATION_HASH(unit, i)) {
            /* Read HW entry to see if it's really a match */
            rv = READ_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY,
                                         i, &station_entry);
            if (rv < 0) {
                COMMON_UNLOCK(unit);
                return rv;
            }
            soc_mem_mac_addr_get(unit, MPLS_STATION_TCAMm,
                                 &station_entry, MAC_ADDRf, hw_mac);
            hw_vlan = soc_mem_field32_get(unit, MPLS_STATION_TCAMm,
                                          &station_entry, VLAN_IDf);
            if ((vlan == hw_vlan) &&
                !sal_memcmp(mac, hw_mac, sizeof(bcm_mac_t))) {
                break;
            }
        }
    }
    if (i == num_station) {
        /* Entry not found */
        COMMON_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }
    /* Clear the entry in HW */
    sal_memset(&station_entry, 0, sizeof(mpls_station_tcam_entry_t));
    rv = WRITE_MPLS_STATION_TCAMm(unit, MEM_BLOCK_ANY, i, &station_entry);
    if (rv < 0) {
        COMMON_UNLOCK(unit);
        return rv;
    }
    COMMON_UNLOCK(unit);
    MPLS_STATION_HASH(unit, i) = 0;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_l2_tunnel_delete_all
 * Purpose:
 *    Delete all (MAC, VLAN) for tunnel/MPLS processing.
 *    Shared by MPLS and MIM
 * Parameters:
 *      unit - Unit number
 */
int
bcm_trx_metro_l2_tunnel_delete_all(int unit)
{
    int rv, num_station;

    num_station = soc_mem_index_count(unit, MPLS_STATION_TCAMm);
    COMMON_LOCK(unit);
    rv = soc_mem_clear(unit, MPLS_STATION_TCAMm, COPYNO_ALL, 0);
    if (rv < 0) {
        COMMON_UNLOCK(unit);
        return rv;
    }
    sal_memset(COMMON_INFO(unit)->mpls_station_hash, 0,
               sizeof(uint16) * num_station);
    COMMON_UNLOCK(unit);
    return BCM_E_NONE;
}

#if defined(BCM_TRIUMPH2_SUPPORT)
/*
 * Flexible stats:
 * 
 * Ingress/egress packet/byte counters which may be assigned from a pool
 * to various packet categories.  There are two pools available:
 * Service interface types:  VLAN, VRF, VFI
 * Virtual interface types:  Some GPORT categories
 * The source table responsible for the packet classification holds an
 * index to the counter pool.  Otherwise, the implementation of the two
 * types is equivalent, so it is abstracted here.
 * This is implemented here because so many modules are touched by
 * this feature.
 */



static _bcm_flex_stat_info_t *fs_info[BCM_MAX_NUM_UNITS][_bcmFlexStatTypeHwNum];
STATIC sal_mutex_t _flex_stat_mutex[BCM_MAX_NUM_UNITS] = {NULL};

/* The gport and fp types are currently implemented in the same HW.
 * Here we collapse them into the same info array for tracking.
 * We need the different types to distinguish the different handles.
 */
#define FS_TYPE_TO_HW_TYPE(unit, type) \
        (((type == _bcmFlexStatTypeService) || \
          (type == _bcmFlexStatTypeVrf))? \
                _bcmFlexStatTypeService : _bcmFlexStatTypeGport)

#define FS_INFO(unit, hw_type) \
            fs_info[unit][hw_type]

#define FS_MEM_SIZE(unit, hw_type)     FS_INFO(unit, hw_type)->stat_mem_size
#define FS_USED_BITMAP(unit, hw_type)  FS_INFO(unit, hw_type)->stats
#define FS_HANDLE_LIST_SIZE(unit, hw_type)  \
                            FS_INFO(unit, hw_type)->handle_list_size
#define FS_HANDLE_LIST(unit, hw_type)  FS_INFO(unit, hw_type)->handle_list
#define FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, list_index) \
            (&(FS_INFO(unit, hw_type)->handle_list[list_index]))
#define FS_HANDLE_LIST_ELEMENT_HANDLE(unit, hw_type, list_index) \
            ((FS_INFO(unit, hw_type)->handle_list[list_index].handle))
#define FS_HANDLE_LIST_ELEMENT_TYPE(unit, hw_type, list_index) \
            ((FS_INFO(unit, hw_type)->handle_list[list_index].type))
#define FS_HANDLE_LIST_ELEMENT_INDEX(unit, hw_type, list_index) \
            ((FS_INFO(unit, hw_type)->handle_list[list_index].index))
#define FS_HANDLE_LIST_ELEMENT_SIZE \
                                  sizeof(_bcm_flex_stat_handle_to_index_t)

/* Below, the macros work with the FS type */
#define FS_ING_MEM(type)         \
        ((FS_TYPE_TO_HW_TYPE(unit, type) == _bcmFlexStatTypeService) ? \
                ING_SERVICE_COUNTER_TABLEm : ING_VINTF_COUNTER_TABLEm)
#define FS_EGR_MEM(type)         \
        ((FS_TYPE_TO_HW_TYPE(unit, type) == _bcmFlexStatTypeService) ? \
                EGR_SERVICE_COUNTER_TABLEm : EGR_VINTF_COUNTER_TABLEm)
/* 
 * Common resource lock
 */
#define FS_LOCK(unit) \
        sal_mutex_take(_flex_stat_mutex[unit], sal_mutex_FOREVER);

#define FS_UNLOCK(unit) \
        sal_mutex_give(_flex_stat_mutex[unit]); 

#define FS_INIT(unit, type) \
        if (FS_INFO(unit, FS_TYPE_TO_HW_TYPE(unit, type)) == NULL) { \
            FS_UNLOCK(unit); \
            return BCM_E_INIT; \
        }


/*
 * Function:
 *      _bcm_esw_flex_stat_detach
 * Purpose:
 *      De-allocate memory used by flexible counters.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_detach(int unit, _bcm_flex_stat_type_t fs_type)
{
    _bcm_flex_stat_info_t  *stat_info;
    _bcm_flex_stat_type_t type_idx,
        hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);
    int mutex = TRUE;

    if (fs_type >= _bcmFlexStatTypeHwNum) {
        /* We shouldn't be deallocating this */
        return BCM_E_INTERNAL;
    }

    if (NULL != _flex_stat_mutex[unit]) {
        FS_LOCK(unit);
    }

    stat_info = FS_INFO(unit, hw_type);

    if (stat_info != NULL) {
        if (stat_info->stats != NULL) {
            sal_free(stat_info->stats);
        }
        if (stat_info->handle_list != NULL) {
            sal_free(stat_info->handle_list);
        }
        sal_free(stat_info);
        FS_INFO(unit, hw_type) = NULL;
    }

    if (NULL != _flex_stat_mutex[unit]) {
        FS_UNLOCK(unit);
        /* Check if all counter types are detached */
        for (type_idx = _bcmFlexStatTypeService;
             type_idx < _bcmFlexStatTypeHwNum; type_idx ++) {
            if (FS_INFO(unit, type_idx) != NULL) {
                mutex = FALSE;
                break;
            }
        }
        /* If so, destroy the mutex also */
        if (mutex) {
            sal_mutex_destroy(_flex_stat_mutex[unit]);
            _flex_stat_mutex[unit] = NULL;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_init
 * Purpose:
 *      Allocate memory used by flexible counters.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_init(int unit, _bcm_flex_stat_type_t fs_type)
{
    _bcm_flex_stat_info_t  *stat_info;
    int alloc_size, rv = BCM_E_NONE, rve;
    soc_mem_t mem;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    if (fs_type >= _bcmFlexStatTypeHwNum) {
        /* We shouldn't be allocating this */
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_flex_stat_detach(unit, fs_type));

    /* Without the lock, we can't do anything */
    if (_flex_stat_mutex[unit] == NULL) {
        _flex_stat_mutex[unit] = sal_mutex_create("common mutex");
        if (_flex_stat_mutex[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    FS_LOCK(unit);

    alloc_size = sizeof(_bcm_flex_stat_info_t);
    stat_info = sal_alloc(alloc_size, "Flexible counters info");
    if (NULL == stat_info) {
        rv = BCM_E_MEMORY;
    }

    if (BCM_SUCCESS(rv)) {
        sal_memset(stat_info, 0, alloc_size);

        mem = FS_ING_MEM(fs_type);
        stat_info->stat_mem_size = soc_mem_index_count(unit, mem);

        alloc_size = SHR_BITALLOCSIZE(stat_info->stat_mem_size);
        stat_info->stats =
            sal_alloc(alloc_size, "Flexible counters usage bitmap");
        if (NULL == stat_info->stats) {
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv)) {
        sal_memset(stat_info->stats, 0, alloc_size);
        SHR_BITSET(stat_info->stats, 0); /* Index 0 is invalid */

        alloc_size = (FS_HANDLE_LIST_ELEMENT_SIZE *
                      stat_info->stat_mem_size);
        stat_info->handle_list =
            sal_alloc(alloc_size, "Flexible counters handle list");
        if (NULL == stat_info->handle_list) {
            rv = BCM_E_MEMORY;
        }
    }

    if (BCM_SUCCESS(rv)) {
        sal_memset(stat_info->handle_list, 0, alloc_size);
        stat_info->handle_list_size = 0;
        FS_INFO(unit, hw_type) = stat_info;
    } else {
        /* Clean up */
        rve = _bcm_esw_flex_stat_detach(unit, fs_type);
        /* We already have an error in rv, so nothing to do with rve */
    }
    FS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_flex_stat_handle_to_index_t_compare
 * Purpose:
 *      Compare two _bcm_flex_stat_handle_to_index_t elements for 
 *      binary table sort.
 * Parameters:
 *      a - (IN) pointer to first _bcm_flex_stat_handle_to_index_t
 *      b - (IN) pointer to second _bcm_flex_stat_handle_to_index_t
 * Returns:
 *      a<=>b
 */
STATIC int
_bcm_flex_stat_handle_to_index_t_compare(void *a, void *b)
{
    _bcm_flex_stat_handle_to_index_t *first, *second;

    first = (_bcm_flex_stat_handle_to_index_t *)a;
    second = (_bcm_flex_stat_handle_to_index_t *)b;

    if (first->type < second->type) {
        return (-1);
    } else if (first->type > second->type) {
        return (1);
    }

    /* Types equal, check handle */
    if (first->handle < second->handle) {
        return (-1);
    } else if (first->handle > second->handle) {
        return (1);
    }
    return (0);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_index
 * Purpose:
 *      Retrieve the flexible stat index for the given handle and stat type.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 * Returns:
 *      Flexible stat index
 */
STATIC int
_bcm_esw_flex_stat_index(int unit, _bcm_flex_stat_type_t fs_type,
                         uint32 handle)
{
    int list_index;
    _bcm_flex_stat_handle_to_index_t target;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    target.handle = handle;
    target.type = fs_type;
    list_index = _shr_bsearch(FS_HANDLE_LIST(unit, hw_type),
                              FS_HANDLE_LIST_SIZE(unit, hw_type),
                              FS_HANDLE_LIST_ELEMENT_SIZE, &target,
                              &_bcm_flex_stat_handle_to_index_t_compare);

    if ((list_index >= 0) &&
        (list_index < FS_HANDLE_LIST_SIZE(unit, hw_type))) {
        return FS_HANDLE_LIST_ELEMENT_INDEX(unit, hw_type, list_index);
    }
    return 0;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_index_insert
 * Purpose:
 *      Add the flexible stat index for the given handle and stat type
 *      to the handle list.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      index - flexible stat index
 * Returns:
 *      Flexible stat index, or 0 on resource failure
 */
STATIC int
_bcm_esw_flex_stat_index_insert(int unit, _bcm_flex_stat_type_t fs_type,
                                uint32 handle, uint32 index)
{
    int list_index, idx, tmp;
    _bcm_flex_stat_handle_to_index_t target;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    if (FS_HANDLE_LIST_SIZE(unit, hw_type) >= FS_MEM_SIZE(unit, hw_type)) {
        
        return 0;
    }

    target.handle = handle;
    target.type = fs_type;
    list_index = _shr_bsearch(FS_HANDLE_LIST(unit, hw_type),
                              FS_HANDLE_LIST_SIZE(unit, hw_type),
                              FS_HANDLE_LIST_ELEMENT_SIZE, &target,
                              &_bcm_flex_stat_handle_to_index_t_compare);


    if (list_index >= 0) {
        
        return 0;
    }

    idx = (((-1) * list_index) - 1);
    tmp = FS_HANDLE_LIST_SIZE(unit, hw_type) - 1;
    while (tmp >= idx) {
        sal_memcpy(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp + 1),
                   FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp),
                   FS_HANDLE_LIST_ELEMENT_SIZE);
        tmp--;
    }
    FS_HANDLE_LIST_ELEMENT_HANDLE(unit, hw_type, idx) = handle;
    FS_HANDLE_LIST_ELEMENT_TYPE(unit, hw_type, idx) = fs_type;
    FS_HANDLE_LIST_ELEMENT_INDEX(unit, hw_type, idx) = index;
    FS_HANDLE_LIST_SIZE(unit, hw_type)++;
    SHR_BITSET(FS_USED_BITMAP(unit, hw_type), index);
    return index;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_index_delete
 * Purpose:
 *      Remove the flexible stat index for the given handle and stat type
 *      from the handle list.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      index - flexible stat index
 * Returns:
 *      Flexible stat index, or 0 on resource failure
 */
STATIC void
_bcm_esw_flex_stat_index_delete(int unit, _bcm_flex_stat_type_t fs_type,
                                uint32 handle, uint32 index)
{
    int list_index, max, tmp;
    _bcm_flex_stat_handle_to_index_t target;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    target.handle = handle;
    target.type = fs_type;
    list_index = _shr_bsearch(FS_HANDLE_LIST(unit, hw_type),
                              FS_HANDLE_LIST_SIZE(unit, hw_type),
                              FS_HANDLE_LIST_ELEMENT_SIZE, &target,
                              &_bcm_flex_stat_handle_to_index_t_compare);

    if (list_index < 0) {
        
        return;
    }

    tmp = list_index;
    max = FS_HANDLE_LIST_SIZE(unit, hw_type) - 1;
    while (tmp < max) {
        sal_memcpy(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp),
                   FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp + 1),
                   FS_HANDLE_LIST_ELEMENT_SIZE);
        tmp++;
    }
    FS_HANDLE_LIST_SIZE(unit, hw_type)--;
    sal_memset(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type,
                                      FS_HANDLE_LIST_SIZE(unit, hw_type)),
               0, FS_HANDLE_LIST_ELEMENT_SIZE);
    SHR_BITCLR(FS_USED_BITMAP(unit, hw_type), index);
    return;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_next_free_index
 * Purpose:
 *      Retrieve the next available flexible stat index for a stat type.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      Flexible stat index
 */
STATIC int
_bcm_esw_flex_stat_next_free_index(int unit, _bcm_flex_stat_type_t fs_type)
{
    int                 ix, mem_size;
    uint32              bit;
    SHR_BITDCL          not_ptrs, *stat_bmp;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    mem_size = FS_MEM_SIZE(unit, hw_type);
    stat_bmp = FS_USED_BITMAP(unit, hw_type);
    for (ix = 0; ix < _SHR_BITDCLSIZE(mem_size); ix++) {
        not_ptrs = ~stat_bmp[ix];
        if (not_ptrs) {
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (not_ptrs & (1 << bit)) {
                    return (ix * SHR_BITWID) + bit;
                }
            }
        }
    }

    return 0;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_alloc
 * Purpose:
 *      Attempt to allocate a flexible stat index for a stat type.
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 * Returns:
 *      Success: Flexible stat index
 *      Failure: 0 (invalid stat index)
 */
STATIC int
_bcm_esw_flex_stat_alloc(int unit, _bcm_flex_stat_type_t type,
                         uint32 handle)
{
    int index;

    /* Already checked for existence */
    index = _bcm_esw_flex_stat_next_free_index(unit, type);
    if (index) {
        /* Returns 0 on error */
        index = _bcm_esw_flex_stat_index_insert(unit, type, handle, index);
    }

    return index;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_release_handles
 * Purpose:
 *      Deallocate all handles of the given stat type.
 * Parameters:
 *      unit - SOC unit number
 *      fs_type - flexible stat type
 * Returns:
 *      Flexible stat index
 */
void
_bcm_esw_flex_stat_release_handles(int unit, _bcm_flex_stat_type_t fs_type)
{
    int list_index, idx, tmp, max;
    _bcm_flex_stat_handle_to_index_t target;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, fs_type);

    target.handle = 0; /* Find start of this type */
    target.type = fs_type;
    
    FS_LOCK(unit);

    max = FS_HANDLE_LIST_SIZE(unit, hw_type);
    list_index = _shr_bsearch(FS_HANDLE_LIST(unit, hw_type), max,
                              FS_HANDLE_LIST_ELEMENT_SIZE, &target,
                              &_bcm_flex_stat_handle_to_index_t_compare);

    /* Starting point */
    if (list_index < 0) {
        idx = (((-1) * list_index) - 1);
    } else {
        idx = list_index;
    }

    tmp = idx;
    while (tmp < max) {
        if (FS_HANDLE_LIST_ELEMENT_TYPE(unit, hw_type, tmp) != fs_type) {
            /* Copy up entry */
            sal_memcpy(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, idx),
                       FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp),
                       FS_HANDLE_LIST_ELEMENT_SIZE);
            idx++;
        }
        /* Blank out current position */
        sal_memset(FS_HANDLE_LIST_ELEMENT_PTR(unit, hw_type, tmp),
                   0, FS_HANDLE_LIST_ELEMENT_SIZE);
        tmp++;
    }
    FS_HANDLE_LIST_SIZE(unit, hw_type) = idx + 1;
    FS_UNLOCK(unit);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_reinit_add
 * Purpose:
 *      Record the flexible stat information during reinitialization.
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      fs_index - the flexible stat index
 *      handle - the encoded handle for the type of packets to count.
 * Returns:
 *      BCM_X_XXX
 */
void
_bcm_esw_flex_stat_reinit_add(int unit, _bcm_flex_stat_type_t type,
                              int fs_index, uint32 handle)
{
    int rv;

    FS_LOCK(unit);
    rv = _bcm_esw_flex_stat_index_insert(unit, type, handle, fs_index);
    FS_UNLOCK(unit);
    /* Ignoring insert return code */
}

/*
 * Function:
 *      _bcm_esw_vlan_flex_stat_hw_index_set
 * Purpose:
 *      Update the flexible stat pointers of HW tables for VLAN/VFI types.
 * Parameters:
 *      unit - SOC unit number
 *      handle - the encoded handle for the type of packets to count.
 *      fs_idx - the flexible stat index
 *      cookie - info to be used from original calling routine
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_vlan_flex_stat_hw_index_set(int unit, uint32 handle,
                                     int fs_idx, void *cookie)
{
#if defined(INCLUDE_L3)
    int vfi;

    /* Is it a VPN? */
    if (_BCM_MPLS_VPN_IS_SET(handle)) {
        vfi = _BCM_MPLS_VPN_ID_GET(handle);
        return _bcm_vfi_flex_stat_index_set(unit, vfi,
                                            _bcmVfiTypeMpls, fs_idx);
    } else if (_BCM_MIM_VPN_IS_SET(handle)) {
        vfi = _BCM_MIM_VPN_ID_GET(handle);
        return _bcm_vfi_flex_stat_index_set(unit, vfi,
                                            _bcmVfiTypeMim, fs_idx);
    } else 
#endif /* INCLUDE_L3 */
    {
        return _bcm_esw_vlan_flex_stat_index_set(unit, handle, fs_idx);
    }    
}

/*
 * Function:
 *      _bcm_esw_port_flex_stat_hw_index_set
 * Purpose:
 *      Update the flexible stat pointers of HW tables for VLAN/VFI types.
 * Parameters:
 *      unit - SOC unit number
 *      handle - the encoded handle for the type of packets to count.
 *      fs_idx - the flexible stat index
 *      cookie - info to be used from original calling routine
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_port_flex_stat_hw_index_set(int unit, uint32 handle,
                                     int fs_idx, void *cookie)
{
    COMPILER_REFERENCE(cookie);
#if defined(INCLUDE_L3)
    if (BCM_GPORT_IS_MPLS_PORT(handle)) {
#if defined(BCM_MPLS_SUPPORT)
        return _bcm_esw_mpls_flex_stat_index_set(unit, handle, fs_idx);
#else
        return BCM_E_UNAVAIL;
#endif /* BCM_MPLS_SUPPORT */
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(handle) ||
               BCM_GPORT_IS_SUBPORT_PORT(handle)) {
        return _bcm_esw_subport_flex_stat_index_set(unit, handle, fs_idx);
    } else if (BCM_GPORT_IS_MIM_PORT(handle)) {
        return _bcm_esw_mim_flex_stat_index_set(unit, handle, fs_idx);
    } else if (BCM_GPORT_IS_WLAN_PORT(handle)) {
        return _bcm_esw_wlan_flex_stat_index_set(unit, handle, fs_idx);
    } else 
#endif /* INCLUDE_L3 */
    if (BCM_GPORT_IS_LOCAL(handle)) {
        return _bcm_esw_port_flex_stat_index_set(unit, handle, fs_idx);
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_handle_free
 * Purpose:
 *      Deallocate the flexible stat for a handle that has been deleted
 *      by its controlling module.
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 * Returns:
 *      Success: Flexible stat index
 *      Failure: 0 (invalid stat index)
 * Notes:
 *      No HW update is required because the controlling module clears
 *      the entry for the associated handle. We note that this FS index
 *      is again usable, and the FS HW counters will be reset when this
 *      index is next used.
 */
void
_bcm_esw_flex_stat_handle_free(int unit, _bcm_flex_stat_type_t type,
                               uint32 handle)
{
    int fs_idx;
    _bcm_flex_stat_type_t hw_type = FS_TYPE_TO_HW_TYPE(unit, type);

    FS_LOCK(unit);
    if (FS_INFO(unit, hw_type) != NULL) {
        fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
        if (fs_idx > 0) {
            _bcm_esw_flex_stat_index_delete(unit, type, handle, fs_idx);
        }
    }
    FS_UNLOCK(unit);
}

/* Write to Flexible Counters HW */
STATIC int
_bcm_esw_flex_stat_hw_set(int unit, _bcm_flex_stat_type_t type,
                          int fs_idx, _bcm_flex_stat_t stat, uint64 val)
{
    ing_service_counter_table_entry_t flex_ctr_entry;
    soc_mem_t mem;
    soc_field_t stat_field = BYTE_COUNTERf;
    uint32 value[2], mask[2];
    int rv;

    switch (stat) {
    case _bcmFlexStatIngressPackets:
        stat_field = PACKET_COUNTERf;
        /* Fall thru */
    case _bcmFlexStatIngressBytes:
        mem = FS_ING_MEM(type);
        break;
    case _bcmFlexStatEgressPackets:
        stat_field = PACKET_COUNTERf;
        /* Fall thru */
    case _bcmFlexStatEgressBytes:
        mem = FS_EGR_MEM(type);
        break;
    default:
        return BCM_E_PARAM;
    }

    if (stat_field == PACKET_COUNTERf) {
        mask[0] = 0x1fffffff; /* 29 bits */
        mask[1] = 0;
    } else { /* Bytes */
        mask[0] = 0xffffffff;
        mask[1] = 0x7; /* 35 bits */
    }

    value[0] = COMPILER_64_LO(val);
    value[1] = COMPILER_64_HI(val);
    if (((value[0] & ~mask[0]) != 0) || ((value[1] & ~mask[1]) != 0)) {
        return BCM_E_PARAM;
    }

    MEM_LOCK(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, fs_idx, &flex_ctr_entry);
    if (rv < 0) {
        MEM_UNLOCK(unit, mem);
        return rv;
    }
    soc_mem_field_set(unit, mem, (uint32 *)&flex_ctr_entry,
                      stat_field, value);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, fs_idx,
                       &flex_ctr_entry);
    MEM_UNLOCK(unit, mem);

    return rv;
}

/* Read from Flexible Counters HW */
STATIC int
_bcm_esw_flex_stat_hw_get(int unit,  _bcm_flex_stat_type_t type,
                            int fs_idx, _bcm_flex_stat_t stat, uint64 *val)
{
    ing_service_counter_table_entry_t flex_ctr_entry;
    soc_mem_t mem;
    soc_field_t stat_field = BYTE_COUNTERf;
    uint32 value[2] = {0, 0};
    int rv;

    switch (stat) {
    case _bcmFlexStatIngressPackets:
        stat_field = PACKET_COUNTERf;
        /* Fall thru */
    case _bcmFlexStatIngressBytes:
        mem = FS_ING_MEM(type);
        break;
    case _bcmFlexStatEgressPackets:
        stat_field = PACKET_COUNTERf;
        /* Fall thru */
    case _bcmFlexStatEgressBytes:
        mem = FS_EGR_MEM(type);
        break;
    default:
        return BCM_E_PARAM;
    }

    MEM_LOCK(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, fs_idx, &flex_ctr_entry);
    MEM_UNLOCK(unit, mem);

    if (BCM_SUCCESS(rv)) {
        soc_mem_field_get(unit, mem, (uint32 *)&flex_ctr_entry,
                          stat_field, value);
        COMPILER_64_SET(*val, value[1], value[0]);
    }

    return rv;
}

/* Clear the Flexible Counters HW */
STATIC int
_bcm_esw_flex_stat_hw_clear(int unit, _bcm_flex_stat_type_t type,
                            int fs_idx)
{
    ing_service_counter_table_entry_t flex_ctr_entry;
    soc_mem_t mem;
    int rv = BCM_E_NONE;

    sal_memset(&flex_ctr_entry, 0, sizeof(flex_ctr_entry));

    mem = FS_ING_MEM(type);
    MEM_LOCK(unit, mem);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, fs_idx, &flex_ctr_entry);
    MEM_UNLOCK(unit, mem);

    if (BCM_SUCCESS(rv)) {
        mem = FS_EGR_MEM(type);
        MEM_LOCK(unit, mem);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, fs_idx,
                           &flex_ctr_entry);
        MEM_UNLOCK(unit, mem);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_enable_set
 * Purpose:
 *      Deallocate the flexible stat for a handle that has been deleted
 *      by its controlling module.
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      fs_set_f - callback function to write index into HW table
 *      cookie - cookie for callback function
 *      handle - the encoded handle for the type of packets to count.
 *      enable - TRUE: configure flexible stat collection for the handle.
 *               FALSE: stop flexible stat collection for the handle.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_enable_set(int unit, _bcm_flex_stat_type_t type,
                              _bcm_flex_stat_src_index_set_f fs_set_f,
                              void *cookie, uint32 handle, int enable)
{
    int fs_idx, rv = BCM_E_NONE;

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        if (enable) {
            rv = BCM_E_EXISTS;
        } else {
            _bcm_esw_flex_stat_index_delete(unit, type, handle, fs_idx);
            rv = (*fs_set_f)(unit, handle, 0, cookie);
        }
    } else {
        if (!enable) {
            rv = BCM_E_NOT_FOUND;
        } else {
            fs_idx = _bcm_esw_flex_stat_alloc(unit, type, handle);
            if (fs_idx) {
                rv = (*fs_set_f)(unit, handle, fs_idx, cookie);
                if (BCM_FAILURE(rv)) {
                    _bcm_esw_flex_stat_index_delete(unit, type,
                                                    handle, fs_idx);
                }
            } else {
                rv = BCM_E_RESOURCE;
            }
            /* Start with cleared stats for this index */
            if (BCM_SUCCESS(rv)) {
                rv = _bcm_esw_flex_stat_hw_clear(unit, type, fs_idx);
            }
        }
    }
    FS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_get
 * Purpose:
 *      Retrieve a flexible stat for a handle 
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      stat - (IN) Type of the flexible statistic to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_get(int unit, _bcm_flex_stat_type_t type,
                       uint32 handle, _bcm_flex_stat_t stat, uint64 *val)
{
    int fs_idx, rv = BCM_E_NONE;

    if (NULL == val) {
        return (BCM_E_PARAM);
    }

    if ((stat < _bcmFlexStatIngressPackets) ||
        (stat >= _bcmFlexStatNum)) {
        return (BCM_E_PARAM);
    }

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        rv = _bcm_esw_flex_stat_hw_get(unit, type, fs_idx, stat, val);
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_get32
 * Purpose:
 *      Retrieve a flexible stat for a handle in 32-bit form 
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      stat - (IN) Type of the flexible statistic to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_get32(int unit, _bcm_flex_stat_type_t type,
                         uint32 handle, _bcm_flex_stat_t stat, uint32 *val)
{
    uint64 val64;            /* 64 bit counter value.    */
    int rv;

    if (NULL == val) {
        return (BCM_E_PARAM);
    }

    rv = _bcm_esw_flex_stat_get(unit, type, handle, stat, &val64);

    if (BCM_SUCCESS(rv)) {
        *val = COMPILER_64_LO(val64);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_set
 * Purpose:
 *      Assign a flexible stat for a handle 
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      stat - (IN) Type of the flexible statistic to retrieve.
 *      val - (IN) Counter value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_set(int unit, _bcm_flex_stat_type_t type,
                       uint32 handle, _bcm_flex_stat_t stat, uint64 val)
{
    int fs_idx, rv = BCM_E_NONE;

    if ((stat < _bcmFlexStatIngressPackets) ||
        (stat >= _bcmFlexStatNum)) {
        return (BCM_E_PARAM);
    }

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        rv = _bcm_esw_flex_stat_hw_set(unit, type, fs_idx, stat, val);
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flex_stat_set32
 * Purpose:
 *      Assign a flexible stat for a handle in 32-bit form 
 * Parameters:
 *      unit - SOC unit number
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      stat - (IN) Type of the flexible statistic to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_flex_stat_set32(int unit, _bcm_flex_stat_type_t type,
                         uint32 handle, _bcm_flex_stat_t stat, uint32 val)
{
    uint64 val64;            /* 64 bit counter value.    */

    COMPILER_64_SET(val64, 0, val);
    return _bcm_esw_flex_stat_set(unit, type, handle, stat, val64);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple flexible statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flex_stat_multi_get(int unit, _bcm_flex_stat_type_t type,
                             uint32 handle, int nstat,
                             _bcm_flex_stat_t *stat_arr,
                             uint64 *value_arr)
{
    int                 rv;        /* Operation return status.    */
    int                 idx;       /* Statistics iteration index. */
    int                 fs_idx;    /* Flexible stat mem index.    */

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        /* Read individual statistics. */
        for (idx = 0; idx < nstat; idx++) {
            if ((stat_arr[idx] < _bcmFlexStatIngressPackets) ||
                (stat_arr[idx] >= _bcmFlexStatNum)) {
                rv = BCM_E_PARAM;
                break;
            }
            rv = _bcm_esw_flex_stat_hw_get(unit, type, fs_idx,
                                           stat_arr[idx], value_arr + idx);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_multi_get32
 * Purpose:
 *      Get 32-bit counter value for multiple flexible statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flex_stat_multi_get32(int unit, _bcm_flex_stat_type_t type,
                               uint32 handle, int nstat,
                               _bcm_flex_stat_t *stat_arr,
                               uint32 *value_arr)
{
    uint64              value;     /* 64 bit counter value.       */
    int                 rv;        /* Operation return status.    */
    int                 idx;       /* Statistics iteration index. */
    int                 fs_idx;    /* Flexible stat mem index.    */

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        /* Read individual statistics. */
        for (idx = 0; idx < nstat; idx++) {
            if ((stat_arr[idx] < _bcmFlexStatIngressPackets) ||
                (stat_arr[idx] >= _bcmFlexStatNum)) {
                rv = BCM_E_PARAM;
                break;
            }
            rv = _bcm_esw_flex_stat_hw_get(unit, type, fs_idx,
                                           stat_arr[idx], &value);
            if (BCM_FAILURE(rv)) {
                break;
            }
            value_arr[idx] = COMPILER_64_LO(value);
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_multi_set
 * Purpose:
 *      Assign 64-bit counter value for multiple flexible statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flex_stat_multi_set(int unit, _bcm_flex_stat_type_t type,
                             uint32 handle, int nstat,
                             _bcm_flex_stat_t *stat_arr, 
                             uint64 *value_arr)
{
    int                 rv;        /* Operation return status.    */
    int                 idx;       /* Statistics iteration index. */
    int                 fs_idx;    /* Flexible stat mem index.    */

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        /* Read individual statistics. */
        for (idx = 0; idx < nstat; idx++) {
            if ((stat_arr[idx] < _bcmFlexStatIngressPackets) ||
                (stat_arr[idx] >= _bcmFlexStatNum)) {
                rv = BCM_E_PARAM;
                break;
            }
            rv = _bcm_esw_flex_stat_hw_set(unit, type, fs_idx,
                                           stat_arr[idx], value_arr[idx]);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_flex_stat_multi_set32
 * Purpose:
 *      Assign 32-bit counter value for multiple flexible statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - flexible stat type
 *      handle - the encoded handle for the type of packets to count.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flex_stat_multi_set32(int unit, _bcm_flex_stat_type_t type,
                               uint32 handle, int nstat,
                               _bcm_flex_stat_t *stat_arr, 
                               uint32 *value_arr)
{
    uint64              value;     /* 64 bit counter value.       */
    int                 rv;        /* Operation return status.    */
    int                 idx;       /* Statistics iteration index. */
    int                 fs_idx;    /* Flexible stat mem index.    */

    FS_LOCK(unit);
    FS_INIT(unit, type);

    fs_idx = _bcm_esw_flex_stat_index(unit, type, handle);
    if (fs_idx > 0) {
        /* Read individual statistics. */
        for (idx = 0; idx < nstat; idx++) {
            if ((stat_arr[idx] < _bcmFlexStatIngressPackets) ||
                (stat_arr[idx] >= _bcmFlexStatNum)) {
                rv = BCM_E_PARAM;
                break;
            }
            COMPILER_64_SET(value, 0, value_arr[idx]);
            rv = _bcm_esw_flex_stat_hw_set(unit, type, fs_idx,
                                           stat_arr[idx], value);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    FS_UNLOCK(unit);

    return (BCM_E_NONE);
}
#endif /* BCM_TRIUMPH2_SUPPORT */
