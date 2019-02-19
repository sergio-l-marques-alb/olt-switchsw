/*
 * $Id: triumph2_qos.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * All Rights Reserved.$
 *
 * QoS module
 */
#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH2_SUPPORT)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <bcm/qos.h>
#include <bcm/error.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/xgs3.h>

#define _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP    1
#define _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS      2
#define _BCM_QOS_MAP_TYPE_DSCP_TABLE         3
#define _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE     4
#define _BCM_QOS_MAP_TYPE_MASK           0x3ff
#define _BCM_QOS_MAP_SHIFT                  10

#define _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS 0x100

typedef struct _bcm_tr2_qos_bookkeeping_s {
    SHR_BITDCL *ing_pri_cng_bitmap; /* ING_PRI_CNG_MAP chunks used */
    uint32 *pri_cng_hw_idx; /* Actual profile number used */
    SHR_BITDCL *egr_mpls_bitmap; /* EGR_MPLS_EXP_MAPPING_1 / 
                                  EGR_MPLS_PRI_MAPPING chunks used */
    uint32 *egr_mpls_hw_idx; /* Actual profile number used */
    SHR_BITDCL *dscp_table_bitmap; /* DSCP_TABLE chunks used */
    uint32 *dscp_hw_idx; /* Actual profile number used */
    SHR_BITDCL *egr_dscp_table_bitmap; /* EGR_DSCP_TABLE chunks used */
    uint32 *egr_dscp_hw_idx; /* Actual profile number used */
} _bcm_tr2_qos_bookkeeping_t;

STATIC _bcm_tr2_qos_bookkeeping_t  _bcm_tr2_qos_bk_info[BCM_MAX_NUM_UNITS];
STATIC sal_mutex_t _qos_mutex[BCM_MAX_NUM_UNITS] = {NULL};

/*
 * QoS resource lock
 */
#define QOS_LOCK(unit) \
        sal_mutex_take(_qos_mutex[unit], sal_mutex_FOREVER);

#define QOS_UNLOCK(unit) \
        sal_mutex_give(_qos_mutex[unit]);

#define QOS_INFO(_unit_) (&_bcm_tr2_qos_bk_info[_unit_])
#define DSCP_CODE_POINT_CNT 64
#define DSCP_CODE_POINT_MAX (DSCP_CODE_POINT_CNT - 1)

/*
 * ING_PRI_CNG_MAP usage bitmap operations
 */
#define _BCM_QOS_ING_PRI_CNG_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->ing_pri_cng_bitmap, (_identifier_))
#define _BCM_QOS_ING_PRI_CNG_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->ing_pri_cng_bitmap, (_identifier_))
#define _BCM_QOS_ING_PRI_CNG_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->ing_pri_cng_bitmap, (_identifier_))

/*
 * EGR_MPLS_EXP_MAPPING_1 / EGR_MPLS_PRI_MAPPING usage bitmap operations
 */
#define _BCM_QOS_EGR_MPLS_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->egr_mpls_bitmap, (_identifier_))
#define _BCM_QOS_EGR_MPLS_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->egr_mpls_bitmap, (_identifier_))
#define _BCM_QOS_EGR_MPLS_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->egr_mpls_bitmap, (_identifier_))

/*
 * DSCP_TABLE usage bitmap operations
 */
#define _BCM_QOS_DSCP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->dscp_table_bitmap, (_identifier_))
#define _BCM_QOS_DSCP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->dscp_table_bitmap, (_identifier_))
#define _BCM_QOS_DSCP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->dscp_table_bitmap, (_identifier_))

/*
 * EGR_DSCP_TABLE usage bitmap operations
 */
#define _BCM_QOS_EGR_DSCP_TABLE_USED_GET(_u_, _identifier_) \
        SHR_BITGET(QOS_INFO(_u_)->egr_dscp_table_bitmap, (_identifier_))
#define _BCM_QOS_EGR_DSCP_TABLE_USED_SET(_u_, _identifier_) \
        SHR_BITSET(QOS_INFO((_u_))->egr_dscp_table_bitmap, (_identifier_))
#define _BCM_QOS_EGR_DSCP_TABLE_USED_CLR(_u_, _identifier_) \
        SHR_BITCLR(QOS_INFO((_u_))->egr_dscp_table_bitmap, (_identifier_))

STATIC int
_bcm_qos_id_alloc(int unit, SHR_BITDCL *bitmap)
{
    int i;
    for (i = 0; i < sizeof(bitmap); i++) {
        if (!SHR_BITGET(bitmap, i)) {
            return i;
        }
    }
    return -1;
}

STATIC void
_bcm_tr2_qos_free_resources(int unit)
{
    _bcm_tr2_qos_bookkeeping_t *qos_info = QOS_INFO(unit);

    if (!qos_info) {
        return;
    }

    /* Destroy ingress profile bitmap */
    if (qos_info->ing_pri_cng_bitmap) {
        sal_free(qos_info->ing_pri_cng_bitmap);
        qos_info->ing_pri_cng_bitmap = NULL;
    }
    if (qos_info->pri_cng_hw_idx) {
        sal_free(qos_info->pri_cng_hw_idx);
        qos_info->pri_cng_hw_idx = NULL;
    }

    /* Destroy egress profile usage bitmap */
    if (qos_info->egr_mpls_bitmap) {
        sal_free(qos_info->egr_mpls_bitmap);
        qos_info->egr_mpls_bitmap = NULL;
    }
    if (qos_info->egr_mpls_hw_idx) {
        sal_free(qos_info->egr_mpls_hw_idx);
        qos_info->egr_mpls_hw_idx = NULL;
    }

    /* Destroy DSCP table profile usage bitmap */
    if (qos_info->dscp_table_bitmap) {
        sal_free(qos_info->dscp_table_bitmap);
        qos_info->dscp_table_bitmap = NULL;
    }
    if (qos_info->dscp_hw_idx) {
        sal_free(qos_info->dscp_hw_idx);
        qos_info->dscp_hw_idx = NULL;
    }

    /* Destroy egress DSCP table profile usage bitmap */
    if (qos_info->egr_dscp_table_bitmap) {
        sal_free(qos_info->egr_dscp_table_bitmap);
        qos_info->egr_dscp_table_bitmap = NULL;
    }
    if (qos_info->egr_dscp_hw_idx) {
        sal_free(qos_info->egr_dscp_hw_idx);
        qos_info->egr_dscp_hw_idx = NULL;
    }

    /* Destroy the mutex */
    if (_qos_mutex[unit]) {
        sal_mutex_destroy(_qos_mutex[unit]);
        _qos_mutex[unit] = NULL;
    }
}

/* Initialize the QoS module. */
int 
bcm_tr2_qos_init(int unit)
{
    _bcm_tr2_qos_bookkeeping_t *qos_info = QOS_INFO(unit);
    int ing_profiles, egr_profiles, rv = BCM_E_NONE;
    int dscp_profiles, egr_dscp_profiles;
    ing_profiles = soc_mem_index_count(unit, ING_PRI_CNG_MAPm) / 16;
    egr_profiles = soc_mem_index_count(unit, EGR_MPLS_PRI_MAPPINGm) / 64;
    dscp_profiles = soc_mem_index_count(unit, DSCP_TABLEm) / 64;
    egr_dscp_profiles = soc_mem_index_count(unit, EGR_DSCP_TABLEm) / 64;

    /* Create mutex */
    if (NULL == _qos_mutex[unit]) {
        _qos_mutex[unit] = sal_mutex_create("qos mutex");
        if (_qos_mutex[unit] == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }

    /* Allocate ingress profile usage bitmap */
    if (NULL == qos_info->ing_pri_cng_bitmap) {
        qos_info->ing_pri_cng_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(ing_profiles), "ing_pri_cng_bitmap");
        if (qos_info->ing_pri_cng_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->ing_pri_cng_bitmap, 0, SHR_BITALLOCSIZE(ing_profiles));
    if (NULL == qos_info->pri_cng_hw_idx) {
        qos_info->pri_cng_hw_idx = 
            sal_alloc(sizeof(uint32) * ing_profiles, "pri_cng_hw_idx");
        if (qos_info->pri_cng_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }    
    }
    sal_memset(qos_info->pri_cng_hw_idx, 0, SHR_BITALLOCSIZE(ing_profiles));

    /* Allocate egress profile usage bitmap */
    if (NULL == qos_info->egr_mpls_bitmap) {
        qos_info->egr_mpls_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(egr_profiles), "egr_mpls_bitmap");
        if (qos_info->egr_mpls_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_mpls_bitmap, 0, SHR_BITALLOCSIZE(egr_profiles));
    if (NULL == qos_info->egr_mpls_hw_idx) {
        qos_info->egr_mpls_hw_idx = 
            sal_alloc(sizeof(uint32) * egr_profiles, "egr_mpls_hw_idx");
        if (qos_info->egr_mpls_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }    
    }
    sal_memset(qos_info->egr_mpls_hw_idx, 0, SHR_BITALLOCSIZE(egr_profiles));

    /* Allocate DSCP_TABLE profile usage bitmap */
    if (NULL == qos_info->dscp_table_bitmap) {
        qos_info->dscp_table_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(dscp_profiles), "dscp_table_bitmap");
        if (qos_info->dscp_table_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->dscp_table_bitmap, 0, SHR_BITALLOCSIZE(dscp_profiles));
    if (NULL == qos_info->dscp_hw_idx) {
        qos_info->dscp_hw_idx = 
            sal_alloc(sizeof(uint32) * dscp_profiles, "dscp_hw_idx");
        if (qos_info->dscp_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }    
    }
    sal_memset(qos_info->dscp_hw_idx, 0, SHR_BITALLOCSIZE(dscp_profiles));

    /* Allocate EGR_DSCP_TABLE profile usage bitmap */
    if (NULL == qos_info->egr_dscp_table_bitmap) {
        qos_info->egr_dscp_table_bitmap =
            sal_alloc(SHR_BITALLOCSIZE(egr_dscp_profiles), "egr_dscp_table_bitmap");
        if (qos_info->egr_dscp_table_bitmap == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(qos_info->egr_dscp_table_bitmap, 0, 
               SHR_BITALLOCSIZE(egr_dscp_profiles));
    if (NULL == qos_info->egr_dscp_hw_idx) {
        qos_info->egr_dscp_hw_idx = 
            sal_alloc(sizeof(uint32) * egr_dscp_profiles, "egr_dscp_hw_idx");
        if (qos_info->egr_dscp_hw_idx == NULL) {
            _bcm_tr2_qos_free_resources(unit);
            return BCM_E_MEMORY;
        }    
    }
    sal_memset(qos_info->egr_dscp_hw_idx, 0, SHR_BITALLOCSIZE(egr_dscp_profiles));

    return rv;
}

/* Detach the QoS module. */
int 
bcm_tr2_qos_detach(int unit)
{
    int rv = BCM_E_NONE;
    _bcm_tr2_qos_free_resources(unit);
    return rv;
}

STATIC int
_bcm_tr2_qos_l2_map_create(int unit, uint32 flags, int *map_id) 
{
    ing_pri_cng_map_entry_t ing_pri_map[16];
    egr_mpls_pri_mapping_entry_t egr_mpls_map[64];
    egr_mpls_exp_mapping_1_entry_t egr_mpls_exp_map[64];
    int id, index = -1, index2 = -1, rv = BCM_E_NONE;
    void *entries[1];
    if (flags & BCM_QOS_MAP_INGRESS) {
        /* Reserve a chunk in the ING_PRI_CNG_MAP table */
        memset(ing_pri_map, 0, sizeof(ing_pri_cng_map_entry_t) * 16);
        entries[0] = &ing_pri_map;
        BCM_IF_ERROR_RETURN(_bcm_ing_pri_cng_map_entry_add(unit, entries, 16,
                                                           (uint32 *)&index));
        id = _bcm_qos_id_alloc(unit, QOS_INFO(unit)->ing_pri_cng_bitmap);
        if (id == -1) {
            return BCM_E_INTERNAL;
        }
        QOS_INFO(unit)->pri_cng_hw_idx[id] = index / 16;
        _BCM_QOS_ING_PRI_CNG_USED_SET(unit, id);
        *map_id = id | (_BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP 
                  << _BCM_QOS_MAP_SHIFT);
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        /* Reserve a chunk in the EGR_MPLS_EXP_MAPPING_1/EGR_MPLS_PRI_MAPPING 
           tables - these two tables always done together */
        memset(egr_mpls_map, 0, sizeof(egr_mpls_pri_mapping_entry_t) * 64);
        entries[0] = &egr_mpls_map;
        BCM_IF_ERROR_RETURN(_bcm_egr_mpls_pri_map_entry_add(unit, entries, 64,
                                                            (uint32 *)&index));
        memset(egr_mpls_exp_map, 0, sizeof(egr_mpls_exp_mapping_1_entry_t) * 64);
        entries[0] = &egr_mpls_exp_map;
        BCM_IF_ERROR_RETURN(_bcm_egr_mpls_exp_map_entry_add(unit, entries, 64,
                                                            (uint32 *)&index2));
        if (index != index2) {
            return BCM_E_INTERNAL;
        }
        id = _bcm_qos_id_alloc(unit, QOS_INFO(unit)->egr_mpls_bitmap);
        if (id == -1) {
            return BCM_E_INTERNAL;
        }
        QOS_INFO(unit)->egr_mpls_hw_idx[id] = index / 64;
        _BCM_QOS_EGR_MPLS_USED_SET(unit, id);
        *map_id = id | (_BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS 
                  << _BCM_QOS_MAP_SHIFT);
    }
    return rv;
} 

STATIC int
_bcm_tr2_qos_l3_map_create(int unit, uint32 flags, int *map_id) 
{
    dscp_table_entry_t dscp_table[64];
    egr_dscp_table_entry_t egr_dscp_table[64];
    int id, index = -1, rv = BCM_E_NONE;
    void *entries[1];
    if (flags & BCM_QOS_MAP_INGRESS) {
        /* Reserve a chunk in the DSCP_TABLE */
        memset(dscp_table, 0, sizeof(dscp_table_entry_t) * 64);
        entries[0] = &dscp_table;
        BCM_IF_ERROR_RETURN(_bcm_dscp_table_entry_add(unit, entries, 64,
                                                      (uint32 *)&index));
        id = _bcm_qos_id_alloc(unit, QOS_INFO(unit)->dscp_table_bitmap);
        if (id == -1) {
            return BCM_E_INTERNAL;
        }
        QOS_INFO(unit)->dscp_hw_idx[id] = index / 64;
        _BCM_QOS_DSCP_TABLE_USED_SET(unit, id);
        *map_id = id | (_BCM_QOS_MAP_TYPE_DSCP_TABLE 
                  << _BCM_QOS_MAP_SHIFT);
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        /* Reserve a chunk in the EGR_DSCP_TABLE */
        memset(egr_dscp_table, 0, sizeof(egr_dscp_table_entry_t) * 64);
        entries[0] = &egr_dscp_table;
        BCM_IF_ERROR_RETURN(_bcm_egr_dscp_table_entry_add(unit, entries, 64,
                                                          (uint32 *)&index));
        id = _bcm_qos_id_alloc(unit, QOS_INFO(unit)->egr_dscp_table_bitmap);
        if (id == -1) {
            return BCM_E_INTERNAL;
        }
        QOS_INFO(unit)->egr_dscp_hw_idx[id] = index / 64;
        _BCM_QOS_EGR_DSCP_TABLE_USED_SET(unit, id);
        *map_id = id | (_BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE 
                  << _BCM_QOS_MAP_SHIFT);
    }
    return rv;
}

STATIC int
_bcm_tr2_qos_mpls_map_create(int unit, uint32 flags, int *map_id) 
{
    if (flags & BCM_QOS_MAP_INGRESS) {
        /* The ING_MPLS_EXP_MAPPING table is used */
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
        BCM_IF_ERROR_RETURN
            (bcm_tr_mpls_exp_map_create(unit, BCM_MPLS_EXP_MAP_INGRESS, 
                                        map_id));
#endif
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        /* The EGR_MPLS_EXP_MAPPING_1 table is used */
        /* Shared with L2 egress map */
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_qos_l2_map_create(unit, BCM_QOS_MAP_EGRESS, map_id));
    }
    return BCM_E_PARAM;
}

/* Create a QoS map profile */
/* This will allocate an ID and a profile index with all-zero mapping */
int 
bcm_tr2_qos_map_create(int unit, uint32 flags, int *map_id)
{
    int rv = BCM_E_UNAVAIL;
    if (flags == 0) {
        return BCM_E_PARAM;
    }
    QOS_LOCK(unit);
    if (flags & BCM_QOS_MAP_L2) {
        rv = _bcm_tr2_qos_l2_map_create(unit, flags, map_id);
    } else if (flags & BCM_QOS_MAP_L3) {
        rv = _bcm_tr2_qos_l3_map_create(unit, flags, map_id);
    } else if (flags & BCM_QOS_MAP_MPLS) {
        rv = _bcm_tr2_qos_mpls_map_create(unit, flags, map_id);
    }
    QOS_UNLOCK(unit);
    return rv;
}

/* Destroy a QoS map profile */
/* This will free the profile index and de-allocate the ID */
int 
bcm_tr2_qos_map_destroy(int unit, int map_id)
{
    int id, rv = BCM_E_UNAVAIL;
    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_ing_pri_cng_map_entry_delete
                     (unit, QOS_INFO(unit)->pri_cng_hw_idx[id] * 16);
            QOS_INFO(unit)->pri_cng_hw_idx[id] = 0;
            _BCM_QOS_ING_PRI_CNG_USED_CLR(unit, id);
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_egr_mpls_pri_map_entry_delete
                     (unit, QOS_INFO(unit)->egr_mpls_hw_idx[id] * 64);
            rv = _bcm_egr_mpls_exp_map_entry_delete
                     (unit, QOS_INFO(unit)->egr_mpls_hw_idx[id] * 64);
            QOS_INFO(unit)->egr_mpls_hw_idx[id] = 0;
            _BCM_QOS_EGR_MPLS_USED_CLR(unit, id);
        }
        break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_dscp_table_entry_delete
                     (unit, QOS_INFO(unit)->dscp_hw_idx[id] * 64);
            QOS_INFO(unit)->dscp_hw_idx[id] = 0;
            _BCM_QOS_DSCP_TABLE_USED_CLR(unit, id);
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
        if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        } else {
            rv = _bcm_egr_dscp_table_entry_delete
                    (unit, QOS_INFO(unit)->egr_dscp_hw_idx[id] * 64);
            QOS_INFO(unit)->egr_dscp_hw_idx[id] = 0;
            _BCM_QOS_EGR_DSCP_TABLE_USED_CLR(unit, id);
        }
        break;
    default:
        if (map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            rv = bcm_tr_mpls_exp_map_destroy(unit, map_id);
#endif
        } else {
            rv = BCM_E_PARAM;
        }
        break;
    }
    QOS_UNLOCK(unit);
    return rv;
}

/* Add an entry to a QoS map */
/* Read the existing profile chunk, modify what's needed and add the 
 * new profile. This can result in the HW profile index changing for a 
 * given ID */
int 
bcm_tr2_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int alloc_size, offset, id, index, index2, rv = BCM_E_NONE;
    ing_pri_cng_map_entry_t ing_pri_map[16];
    egr_mpls_pri_mapping_entry_t egr_mpls_map[64];
    egr_mpls_exp_mapping_1_entry_t egr_mpls_exp_map[64];
    dscp_table_entry_t dscp_table[64];
    egr_dscp_table_entry_t egr_dscp_table[64];
    char *buf, *buf2;
    void *entries[1];

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L2) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) || 
                (map->pkt_pri > 7) || (map->pkt_cfi > 1) || 
                ((map->color != bcmColorGreen) && 
                (map->color != bcmColorYellow) && 
                (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Allocate memory for DMA */
            alloc_size = 16 * sizeof (ing_pri_cng_map_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 pri cng map");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(ing_pri_map, 0, alloc_size);
            sal_memset(buf, 0, alloc_size);

            /* Read the old profile chunk */
            index = QOS_INFO(unit)->pri_cng_hw_idx[id] * 16;
            rv = soc_mem_read_range(unit, ING_PRI_CNG_MAPm, MEM_BLOCK_ANY, 
                                    index, index + 15, buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 16; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, ING_PRI_CNG_MAPm,
                                                          void *, buf, index);
                sal_memcpy(ing_pri_map + index, entries[0],
                           sizeof(ing_pri_cng_map_entry_t));
            }

            /* Modify what's needed */
            offset = (map->pkt_pri << 1) | map->pkt_cfi;
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &ing_pri_map[offset], 
                                PRIf, map->int_pri);
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &ing_pri_map[offset],
                                 CNGf, _BCM_COLOR_ENCODING(unit, map->color));

            /* Delete the old profile chunk */
            index = QOS_INFO(unit)->pri_cng_hw_idx[id] * 16;
            rv = _bcm_ing_pri_cng_map_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Add new chunk and store new HW index */
            entries[0] = ing_pri_map;
            rv = _bcm_ing_pri_cng_map_entry_add(unit, entries,
                                                16, (uint32 *)&index);
            QOS_INFO(unit)->pri_cng_hw_idx[id] = index / 16;

            /* Free the DMA memory */
            soc_cm_sfree(unit, buf);
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!((flags & BCM_QOS_MAP_L2) || (flags & BCM_QOS_MAP_MPLS))
                 || !(flags & BCM_QOS_MAP_EGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) || 
                (map->pkt_pri > 7) || (map->pkt_cfi > 1) || 
                ((map->color != bcmColorGreen) && 
                (map->color != bcmColorYellow) && 
                (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if (flags & BCM_QOS_MAP_MPLS) {
                if ((map->exp > 7) || (map->exp < 0)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            }

            /* Allocate memory for DMA */
            alloc_size = 64 * sizeof (egr_mpls_pri_mapping_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 egr mpls pri map");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_map, 0, alloc_size);
            sal_memset(buf, 0, alloc_size);
            alloc_size = 64 * sizeof (egr_mpls_exp_mapping_1_entry_t);
            buf2 = soc_cm_salloc(unit, alloc_size, "TR2 egr mpls exp map");
            if (NULL == buf2) {
                sal_free(buf);
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_exp_map, 0, alloc_size);
            sal_memset(buf2, 0, alloc_size);

            /* Read the old profile chunk */
            index = QOS_INFO(unit)->egr_mpls_hw_idx[id] * 64;
            rv = soc_mem_read_range(unit, EGR_MPLS_PRI_MAPPINGm, MEM_BLOCK_ANY, 
                                    index, index + 63, buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                soc_cm_sfree(unit, buf2);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 64; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, 
                                                          EGR_MPLS_PRI_MAPPINGm, 
                                                          void *, buf, index);
                sal_memcpy(egr_mpls_map + index, entries[0],
                           sizeof(egr_mpls_pri_mapping_entry_t));
            }
            rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_1m, MEM_BLOCK_ANY, 
                                    index, index + 63, buf2);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                soc_cm_sfree(unit, buf2);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 64; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, 
                                                          EGR_MPLS_EXP_MAPPING_1m, 
                                                          void *, buf2, index);
                sal_memcpy(egr_mpls_exp_map + index, entries[0],
                           sizeof(egr_mpls_exp_mapping_1_entry_t));
            }

            /* Modify what's needed */
            offset = (map->int_pri << 2) | _BCM_COLOR_ENCODING
                                           (unit, map->color);
            soc_mem_field32_set(unit, EGR_MPLS_PRI_MAPPINGm,  
                                (uint32 *)&egr_mpls_map[offset], 
                                NEW_PRIf, map->pkt_pri);
            soc_mem_field32_set(unit, EGR_MPLS_PRI_MAPPINGm,  
                                (uint32 *)&egr_mpls_map[offset], 
                                NEW_CFIf, map->pkt_cfi);
            soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m,  
                                (uint32 *)&egr_mpls_exp_map[offset], 
                                PRIf, map->pkt_pri);
            soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m,  
                                (uint32 *)&egr_mpls_exp_map[offset], 
                                CFIf, map->pkt_cfi);
            if (flags & BCM_QOS_MAP_MPLS) {
                soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m,  
                                    (uint32 *)&egr_mpls_exp_map[offset], 
                                    EXPf, map->pkt_cfi);
            }

            /* Delete the old profile chunk */
            index = QOS_INFO(unit)->egr_mpls_hw_idx[id] * 64;
            rv = _bcm_egr_mpls_pri_map_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                soc_cm_sfree(unit, buf2);
                QOS_UNLOCK(unit);
                return (rv);
            }
            rv = _bcm_egr_mpls_exp_map_entry_delete(unit, index);

            /* Add new chunk and store new HW index */
            entries[0] = egr_mpls_map;
            rv = _bcm_egr_mpls_pri_map_entry_add(unit, entries,
                                                 64, (uint32 *)&index);
            entries[0] = egr_mpls_exp_map;
            rv = _bcm_egr_mpls_exp_map_entry_add(unit, entries,
                                                 64, (uint32 *)&index2);
            if (index != index2) {
                QOS_UNLOCK(unit);
                return BCM_E_INTERNAL;
            }
            QOS_INFO(unit)->egr_mpls_hw_idx[id] = index / 16;

            /* Free the DMA memory */
            soc_cm_sfree(unit, buf);
            soc_cm_sfree(unit, buf2);
        }
        break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L3) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                (map->dscp > DSCP_CODE_POINT_MAX) || (map->dscp < 0) ||
                ((map->color != bcmColorGreen) && 
                 (map->color != bcmColorYellow) && 
                 (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Allocate memory for DMA */
            alloc_size = 64 * sizeof (dscp_table_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 dscp table");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(dscp_table, 0, alloc_size);
            sal_memset(buf, 0, alloc_size);

            /* Read the old profile chunk */
            index = QOS_INFO(unit)->dscp_hw_idx[id] * 64;
            rv = soc_mem_read_range(unit, DSCP_TABLEm, MEM_BLOCK_ANY, 
                                    index, index + 63, buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 64; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, DSCP_TABLEm,
                                                          void *, buf, index);
                sal_memcpy(dscp_table + index, entries[0],
                           sizeof(dscp_table_entry_t));
            }

            /* Modify what's needed */
            offset = map->dscp;
            soc_DSCP_TABLEm_field32_set(unit, (uint32 *)&dscp_table[offset],
                                        DSCPf, map->dscp);
            soc_DSCP_TABLEm_field32_set(unit, (uint32 *)&dscp_table[offset], 
                                        PRIf, map->int_pri);
            soc_DSCP_TABLEm_field32_set(unit, (uint32 *)&dscp_table[offset], CNGf, 
                                        _BCM_COLOR_ENCODING(unit, map->color));

            /* Delete the old profile chunk */
            index = QOS_INFO(unit)->dscp_hw_idx[id] * 64;
            rv = _bcm_dscp_table_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Add new chunk and store new HW index */
            entries[0] = dscp_table;
            rv = _bcm_dscp_table_entry_add(unit, entries, 64, (uint32 *)&index);
            QOS_INFO(unit)->dscp_hw_idx[id] = index / 64;

            /* Free the DMA memory */
            soc_cm_sfree(unit, buf);
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
        if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L3) || !(flags & BCM_QOS_MAP_EGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                (map->dscp > DSCP_CODE_POINT_MAX) || (map->dscp < 0) ||
                ((map->color != bcmColorGreen) && 
                 (map->color != bcmColorYellow) && 
                 (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Allocate memory for DMA */
            alloc_size = 64 * sizeof (egr_dscp_table_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 egr_dscp table");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_dscp_table, 0, alloc_size);
            sal_memset(buf, 0, alloc_size);

            /* Read the old profile chunk */
            index = QOS_INFO(unit)->egr_dscp_hw_idx[id] * 64;
            rv = soc_mem_read_range(unit, EGR_DSCP_TABLEm, MEM_BLOCK_ANY, 
                                    index, index + 63, buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 64; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, EGR_DSCP_TABLEm,
                                                          void *, buf, index);
                sal_memcpy(egr_dscp_table + index, entries[0],
                           sizeof(egr_dscp_table_entry_t));
            }

            /* Modify what's needed */
            offset = (map->int_pri << 2) | _BCM_COLOR_ENCODING
                                           (unit, map->color);
            soc_EGR_DSCP_TABLEm_field32_set(unit, (uint32 *)
                                            &egr_dscp_table[offset], 
                                            DSCPf, map->dscp);

            /* Delete the old profile chunk */
            index = QOS_INFO(unit)->egr_dscp_hw_idx[id] * 64;
            rv = _bcm_egr_dscp_table_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Add new chunk and store new HW index */
            entries[0] = egr_dscp_table;
            rv = _bcm_egr_dscp_table_entry_add(unit, entries, 64, 
                                               (uint32 *)&index);
            QOS_INFO(unit)->egr_dscp_hw_idx[id] = index / 64;

            /* Free the DMA memory */
            soc_cm_sfree(unit, buf);
        }
        break;
    default:
        if (map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            bcm_mpls_exp_map_t ing_exp_map;
            bcm_mpls_exp_map_t_init(&ing_exp_map);
            if (!(flags & BCM_QOS_MAP_MPLS) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) || 
                (map->exp > 7) || (map->exp < 0) || 
                ((map->color != bcmColorGreen) && 
                (map->color != bcmColorYellow) && 
                (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Call the triumph/mpls.c implementation */
            ing_exp_map.exp = map->exp;
            ing_exp_map.priority = map->int_pri;
            ing_exp_map.color = map->color;
            rv = bcm_tr_mpls_exp_map_set(unit, map_id, &ing_exp_map);
#endif
        } else {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        break;
    }
    QOS_UNLOCK(unit);
    return rv;
}

/* Clear an entry from a QoS map */
/* Read the existing profile chunk, modify what's needed and add the 
 * new profile. This can result in the HW profile index changing for a 
 * given ID */
int 
bcm_tr2_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int alloc_size, offset, id, index, index2, rv = BCM_E_NONE;
    ing_pri_cng_map_entry_t ing_pri_map[16];
    egr_mpls_pri_mapping_entry_t egr_mpls_map[64];
    egr_mpls_exp_mapping_1_entry_t egr_mpls_exp_map[64];
    dscp_table_entry_t dscp_table[64];
    egr_dscp_table_entry_t egr_dscp_table[64];
    char *buf, *buf2;
    void *entries[1];

    id = map_id & _BCM_QOS_MAP_TYPE_MASK;
    QOS_LOCK(unit);
    switch (map_id >> _BCM_QOS_MAP_SHIFT) {
    case _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP:
        if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L2) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->pkt_pri > 7) || (map->pkt_cfi > 1)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Allocate memory for DMA */
            alloc_size = 16 * sizeof (ing_pri_cng_map_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 pri cng map");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(ing_pri_map, 0, alloc_size);
            sal_memset(buf, 0, alloc_size);

            /* Read the old profile chunk */
            index = QOS_INFO(unit)->pri_cng_hw_idx[id] * 16;
            rv = soc_mem_read_range(unit, ING_PRI_CNG_MAPm, MEM_BLOCK_ANY, 
                                    index, index + 15, buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 16; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, ING_PRI_CNG_MAPm,
                                                          void *, buf, index);
                sal_memcpy(ing_pri_map + index, entries[0],
                           sizeof(ing_pri_cng_map_entry_t));
            }

            /* Modify what's needed - set to zero */
            offset = (map->pkt_pri << 1) | map->pkt_cfi;
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &ing_pri_map[offset], 
                                PRIf, 0);
            soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &ing_pri_map[offset],
                                CNGf, 0);

            /* Delete the old profile chunk */
            index = QOS_INFO(unit)->pri_cng_hw_idx[id] * 16;
            rv = _bcm_ing_pri_cng_map_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Add new chunk and store new HW index */
            entries[0] = ing_pri_map;
            rv = _bcm_ing_pri_cng_map_entry_add(unit, entries,
                                                16, (uint32 *)&index);
            QOS_INFO(unit)->pri_cng_hw_idx[id] = index / 16;

            /* Free the DMA memory */
            soc_cm_sfree(unit, buf);

        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS:
        if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!((flags & BCM_QOS_MAP_L2) || (flags & BCM_QOS_MAP_MPLS))
                 || !(flags & BCM_QOS_MAP_EGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) || 
                ((map->color != bcmColorGreen) && 
                (map->color != bcmColorYellow) && 
                (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Allocate memory for DMA */
            alloc_size = 64 * sizeof (egr_mpls_pri_mapping_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 egr mpls pri map");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_map, 0, alloc_size);
            sal_memset(buf, 0, alloc_size);
            alloc_size = 64 * sizeof (egr_mpls_exp_mapping_1_entry_t);
            buf2 = soc_cm_salloc(unit, alloc_size, "TR2 egr mpls exp map");
            if (NULL == buf2) {
                sal_free(buf);
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_mpls_exp_map, 0, alloc_size);
            sal_memset(buf2, 0, alloc_size);

            /* Read the old profile chunk */
            index = QOS_INFO(unit)->egr_mpls_hw_idx[id] * 64;
            rv = soc_mem_read_range(unit, EGR_MPLS_PRI_MAPPINGm, MEM_BLOCK_ANY, 
                                    index, index + 63, buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                soc_cm_sfree(unit, buf2);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 64; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, 
                                                          EGR_MPLS_PRI_MAPPINGm, 
                                                          void *, buf, index);
                sal_memcpy(egr_mpls_map + index, entries[0],
                           sizeof(egr_mpls_pri_mapping_entry_t));
            }
            rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_1m, MEM_BLOCK_ANY, 
                                    index, index + 63, buf2);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                soc_cm_sfree(unit, buf2);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 64; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, 
                                                          EGR_MPLS_EXP_MAPPING_1m, 
                                                          void *, buf2, index);
                sal_memcpy(egr_mpls_exp_map + index, entries[0],
                           sizeof(egr_mpls_exp_mapping_1_entry_t));
            }

            /* Modify what's needed - set to zero */
            offset = (map->int_pri << 2) | _BCM_COLOR_ENCODING
                                           (unit, map->color);
            soc_mem_field32_set(unit, EGR_MPLS_PRI_MAPPINGm,  
                                (uint32 *)&egr_mpls_map[offset], NEW_PRIf, 0);
            soc_mem_field32_set(unit, EGR_MPLS_PRI_MAPPINGm,  
                                (uint32 *)&egr_mpls_map[offset], NEW_CFIf, 0);
            soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m,  
                                (uint32 *)&egr_mpls_exp_map[offset], PRIf, 0);
            soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m,  
                                (uint32 *)&egr_mpls_exp_map[offset], CFIf, 0);
            if (flags & BCM_QOS_MAP_MPLS) {
                soc_mem_field32_set(unit, EGR_MPLS_EXP_MAPPING_1m,  
                                    (uint32 *)&egr_mpls_exp_map[offset], 
                                    EXPf, 0);
            }

            /* Delete the old profile chunk */
            index = QOS_INFO(unit)->egr_mpls_hw_idx[id] * 64;
            rv = _bcm_egr_mpls_pri_map_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                soc_cm_sfree(unit, buf2);
                QOS_UNLOCK(unit);
                return (rv);
            }
            rv = _bcm_egr_mpls_exp_map_entry_delete(unit, index);

            /* Add new chunk and store new HW index */
            entries[0] = egr_mpls_map;
            rv = _bcm_egr_mpls_pri_map_entry_add(unit, entries,
                                                 64, (uint32 *)&index);
            entries[0] = egr_mpls_exp_map;
            rv = _bcm_egr_mpls_exp_map_entry_add(unit, entries,
                                                 64, (uint32 *)&index2);
            if (index != index2) {
                QOS_UNLOCK(unit);
                return BCM_E_INTERNAL;
            }
            QOS_INFO(unit)->egr_mpls_hw_idx[id] = index / 16;

            /* Free the DMA memory */
            soc_cm_sfree(unit, buf);
            soc_cm_sfree(unit, buf2);
        }
        break;
    case _BCM_QOS_MAP_TYPE_DSCP_TABLE:
        if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L3) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->dscp > DSCP_CODE_POINT_MAX) || (map->dscp < 0)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Allocate memory for DMA */
            alloc_size = 64 * sizeof (dscp_table_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 dscp table");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(dscp_table, 0, alloc_size);
            sal_memset(buf, 0, alloc_size);

            /* Read the old profile chunk */
            index = QOS_INFO(unit)->dscp_hw_idx[id] * 64;
            rv = soc_mem_read_range(unit, DSCP_TABLEm, MEM_BLOCK_ANY, 
                                    index, index + 63, buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 64; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, DSCP_TABLEm,
                                                          void *, buf, index);
                sal_memcpy(dscp_table + index, entries[0],
                           sizeof(dscp_table_entry_t));
            }

            /* Modify what's needed - set to zero */
            offset = map->dscp;
            soc_DSCP_TABLEm_field32_set(unit, (uint32 *)&dscp_table[offset],
                                        DSCPf, map->dscp);
            soc_DSCP_TABLEm_field32_set(unit, (uint32 *)&dscp_table[offset], 
                                        PRIf, 0);
            soc_DSCP_TABLEm_field32_set(unit, (uint32 *)&dscp_table[offset],  
                                        CNGf, 0);

            /* Delete the old profile chunk */
            index = QOS_INFO(unit)->dscp_hw_idx[id] * 64;
            rv = _bcm_dscp_table_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Add new chunk and store new HW index */
            entries[0] = dscp_table;
            rv = _bcm_dscp_table_entry_add(unit, entries, 64, (uint32 *)&index);
            QOS_INFO(unit)->dscp_hw_idx[id] = index / 64;

            /* Free the DMA memory */
            soc_cm_sfree(unit, buf);
        }
        break;
    case _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE:
        if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        } else {
            if (!(flags & BCM_QOS_MAP_L3) || !(flags & BCM_QOS_MAP_EGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->int_pri > 15) || (map->int_pri < 0) ||
                ((map->color != bcmColorGreen) && 
                 (map->color != bcmColorYellow) && 
                 (map->color != bcmColorRed))) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Allocate memory for DMA */
            alloc_size = 64 * sizeof (egr_dscp_table_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 egr_dscp table");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(egr_dscp_table, 0, alloc_size);
            sal_memset(buf, 0, alloc_size);

            /* Read the old profile chunk */
            index = QOS_INFO(unit)->egr_dscp_hw_idx[id] * 64;
            rv = soc_mem_read_range(unit, EGR_DSCP_TABLEm, MEM_BLOCK_ANY, 
                                    index, index + 63, buf);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }
            for (index = 0; index < 64; index++) {
                entries[0] = soc_mem_table_idx_to_pointer(unit, EGR_DSCP_TABLEm,
                                                          void *, buf, index);
                sal_memcpy(egr_dscp_table + index, entries[0],
                           sizeof(egr_dscp_table_entry_t));
            }

            /* Modify what's needed - set to zero */
            offset = (map->int_pri << 2) | _BCM_COLOR_ENCODING
                                           (unit, map->color);
            soc_EGR_DSCP_TABLEm_field32_set(unit, (uint32 *)
                                            &egr_dscp_table[offset], 
                                            DSCPf, 0);

            /* Delete the old profile chunk */
            index = QOS_INFO(unit)->egr_dscp_hw_idx[id] * 64;
            rv = _bcm_egr_dscp_table_entry_delete(unit, index);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, buf);
                QOS_UNLOCK(unit);
                return (rv);
            }

            /* Add new chunk and store new HW index */
            entries[0] = egr_dscp_table;
            rv = _bcm_egr_dscp_table_entry_add(unit, entries, 64, 
                                               (uint32 *)&index);
            QOS_INFO(unit)->egr_dscp_hw_idx[id] = index / 64;

            /* Free the DMA memory */
            soc_cm_sfree(unit, buf);
        }
        break;
    default:
        if (map_id & _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS) {
#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
            bcm_mpls_exp_map_t ing_exp_map;
            bcm_mpls_exp_map_t_init(&ing_exp_map);
            if (!(flags & BCM_QOS_MAP_MPLS) || !(flags & BCM_QOS_MAP_INGRESS)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            if ((map->exp > 7) || (map->exp < 0)) {
                QOS_UNLOCK(unit);
                return BCM_E_PARAM;
            }
            /* Call the triumph/mpls.c implementation */
            ing_exp_map.exp = map->exp;
            ing_exp_map.priority = 0;
            ing_exp_map.color = 0;
            rv = bcm_tr_mpls_exp_map_set(unit, map_id, &ing_exp_map);   
#endif
        } else {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        break;
    }
    QOS_UNLOCK(unit);
    return rv;
}

/* Attach a QoS map to an object (Gport) */
int 
bcm_tr2_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map)
{
    int index, id, alloc_size, rv = BCM_E_NONE;
    uint8 pri, cfi;
    char *buf, *buf2;
    void *entries[1];
#ifdef INCLUDE_L3
    source_vp_entry_t svp;
    egr_wlan_dvp_entry_t wlan_dvp;
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS];
    int tunnel, vp, encap_id, nhi, l3_iif = 0;
    egr_l3_next_hop_entry_t egr_nh;
#endif

    QOS_LOCK(unit);
#ifdef INCLUDE_L3
    if (BCM_GPORT_IS_SET(port)) {
        /* Deal with different types of gports */
        if (BCM_GPORT_IS_MIM_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port)) {
            /* Deal with MiM and MPLS ports */
            if (BCM_GPORT_IS_MIM_PORT(port)) {
                vp = BCM_GPORT_MIM_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMim)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                } 
            } else {
                vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
                if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_BADID;
                } 
            }
            if (ing_map != -1) { /* -1 means no change */
                /* TRUST_DOT1P_PTR from SOURCE_VP table */
                rv = READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
                if (ing_map == 0) {
                    /* Clear the existing map */
                    soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DOT1P_PTRf, 0);
                } else {
                    /* Use the provided map */
                    if ((ing_map >> _BCM_QOS_MAP_SHIFT) != 
                         _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    soc_SOURCE_VPm_field32_set(unit, &svp, TRUST_DOT1P_PTRf, 
                                           QOS_INFO(unit)->pri_cng_hw_idx[id]);
                }
                rv = WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
            if (egr_map != -1) { /* -1 means no change */
                /* SD_TAG::DOT1P_MAPPING_PTR from EGR_L3_NEXT_HOP */
                if (BCM_GPORT_IS_MIM_PORT(port)) {
                    rv = bcm_tr2_multicast_mim_encap_get(unit, 0, 0, port, 
                                                         &encap_id);
                } else {
                    rv = bcm_tr2_multicast_vpls_encap_get(unit, 0, 0, port, 
                                                          &encap_id);
                }
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
#if defined(BCM_ENDURO_SUPPORT)
                if (SOC_IS_ENDURO(unit)) {
                    nhi = encap_id;
                } else 
#endif /* BCM_ENDURO_SUPPORT */
                {
                    nhi = encap_id - BCM_XGS3_DVP_EGRESS_IDX_MIN;
                }
                rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                  nhi, &egr_nh);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
                if (egr_map == 0) {
                    /* Clear the existing map */
                    index = 0;
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                                        SD_TAG__SD_TAG_DOT1P_PRI_SELECTf, 0);
                } else {
                    /* Use the provided map */
                    if ((egr_map >> _BCM_QOS_MAP_SHIFT) != 
                         _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    index = QOS_INFO(unit)->egr_mpls_hw_idx[id];
                    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                                        SD_TAG__SD_TAG_DOT1P_PRI_SELECTf, 1);
                }
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,  
                                    SD_TAG__SD_TAG_DOT1P_MAPPING_PTRf, index);
                rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ALL, 
                                   nhi, &egr_nh);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
        } else if (BCM_GPORT_IS_WLAN_PORT(port)) {
            /* Deal with WLAN ports */
            vp = BCM_GPORT_WLAN_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeWlan)) {
                QOS_UNLOCK(unit);
                return BCM_E_BADID;
            }
            if (ing_map != -1) { /* -1 means no change */
                if (ing_map == 0) {
                    /* Clear the existing map */
                    index = 0;
                } else {
                    /* Use the provided map */
                    if ((ing_map >> _BCM_QOS_MAP_SHIFT) != 
                         _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    index = QOS_INFO(unit)->pri_cng_hw_idx[id];
                }
                rv = _bcm_tr2_lport_field_set(unit, port, TRUST_DOT1P_PTRf,
                                              index);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
            if (egr_map != -1) { /* -1 means no change */
                /* EGR_WLAN_DVP.TUNNEL_INDEX points to CAPWAP initiator.
                 * EGR_IP_TUNNEL.DOT1P_MAPPING_PTR contains the map */
                rv = READ_EGR_WLAN_DVPm(unit, MEM_BLOCK_ANY, vp, &wlan_dvp);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
                tunnel = soc_EGR_WLAN_DVPm_field32_get(unit, &wlan_dvp, 
                                                       TUNNEL_INDEXf);
                rv = soc_mem_read(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY, 
                                  tunnel, tnl_entry);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
                if (egr_map == 0) {
                    /* Clear the existing map */
                    index = 0;
                    soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry, 
                                        DOT1P_PRI_SELECTf, 0);
                } else {
                    /* Use the provided map */
                    if ((egr_map >> _BCM_QOS_MAP_SHIFT) != 
                         _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    index = QOS_INFO(unit)->egr_mpls_hw_idx[id];
                    soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry, 
                                        DOT1P_PRI_SELECTf, 1);
                }
                soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry,  
                                    DOT1P_MAPPING_PTRf, index);
                rv = soc_mem_write(unit, EGR_IP_TUNNELm, MEM_BLOCK_ALL, 
                                   tunnel, tnl_entry);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
        } else if (BCM_GPORT_IS_TUNNEL(port)) {
            /* Deal with tunnel initiators and terminators */
            tunnel = BCM_GPORT_TUNNEL_ID_GET(port);
            if (ing_map != -1) { /* -1 means no change */
                /* Get L3_IIF from L3_TUNNEL.
                 * Get TRUST_DSCP_PTR from L3_IIF */
                alloc_size = 512 * sizeof (tnl_entry);
                buf = soc_cm_salloc(unit, alloc_size, "TR2 L3 TUNNEL");
                if (NULL == buf) {
                    QOS_UNLOCK(unit);
                    return (BCM_E_MEMORY);
                }
                rv = soc_mem_read_range(unit, L3_TUNNELm, MEM_BLOCK_ANY,  
                                        0, 511, (void *)buf);
                if (rv < 0) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return rv;
                }
                for (index = 0; index < 511; index++) {
                    entries[0] = soc_mem_table_idx_to_pointer(unit, L3_TUNNELm, 
                                                              void *, buf, 
                                                              index);
                    if (tunnel == soc_mem_field32_get(unit, L3_TUNNELm, 
                                                      entries[0], TUNNEL_IDf)) {
                        /* Tunnel found - get L3_IIF */
                        l3_iif = soc_mem_field32_get(unit, L3_TUNNELm, 
                                                     entries[0], L3_IIFf);
                        break; 
                    }
                }
                soc_cm_sfree(unit, buf);
                if (index == 511) {
                    QOS_UNLOCK(unit);
                    return BCM_E_NOT_FOUND; /* Tunnel not found */
                }
                if (ing_map == 0) {
                    /* Clear the existing map */
                    index = 0;
                } else {
                    /* Use the provided map */
                    if ((ing_map >> _BCM_QOS_MAP_SHIFT) != 
                         _BCM_QOS_MAP_TYPE_DSCP_TABLE) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (!_BCM_QOS_DSCP_TABLE_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    index = QOS_INFO(unit)->dscp_hw_idx[id];
                }
                rv = soc_mem_field32_modify(unit, L3_IIFm, l3_iif, 
                                            TRUST_DSCP_PTRf, index);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
            if (egr_map != -1) { /* -1 means no change */
                /* DSCP_MAPPING_PTR from EGR_IP_TUNNEL */
                rv = soc_mem_read(unit, EGR_IP_TUNNELm, MEM_BLOCK_ANY, 
                                  tunnel, tnl_entry);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
                if (egr_map == 0) {
                    /* Clear existing map */
                    index = 0;
                    soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry, 
                                        DSCP_SELf, 0);
                } else {
                    /* Use the provided map */
                    if ((egr_map >> _BCM_QOS_MAP_SHIFT) != 
                         _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                    if (!_BCM_QOS_EGR_DSCP_TABLE_USED_GET(unit, id)) {
                        QOS_UNLOCK(unit);
                        return BCM_E_PARAM;
                    }
                    index = QOS_INFO(unit)->egr_dscp_hw_idx[id];
                    soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry, 
                                        DSCP_SELf, 2);
                }
                soc_mem_field32_set(unit, EGR_IP_TUNNELm, tnl_entry,  
                                    DSCP_MAPPING_PTRf, index);
                rv = soc_mem_write(unit, EGR_IP_TUNNELm, MEM_BLOCK_ALL, 
                                   tunnel, tnl_entry);
                if (rv < 0) {
                    QOS_UNLOCK(unit);
                    return rv;
                }
            }
        }
    } else
#endif
    {
        /* Deal with physical ports */
        if (!SOC_PORT_VALID(unit, port)) {
            QOS_UNLOCK(unit);
            return BCM_E_PARAM;
        }
        if (ing_map != -1) { /* -1 means no change */
            /* Make the port's TRUST_DOT1_PTR point to the profile index */
            if (ing_map == 0) {
                /* Clear the existing map */
                soc_mem_lock(unit, PORT_TABm);
                rv = _bcm_tr2_port_tab_set(unit, port, TRUST_DOT1P_PTRf, 0);
                soc_mem_unlock(unit, PORT_TABm);      
            } else {
                if ((ing_map >> _BCM_QOS_MAP_SHIFT) != 
                     _BCM_QOS_MAP_TYPE_ING_PRI_CNG_MAP) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
                id = ing_map & _BCM_QOS_MAP_TYPE_MASK;
                if (!_BCM_QOS_ING_PRI_CNG_USED_GET(unit, id)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
                soc_mem_lock(unit, PORT_TABm);
                rv = _bcm_tr2_port_tab_set(unit, port, TRUST_DOT1P_PTRf, 
                                           QOS_INFO(unit)->pri_cng_hw_idx[id]);
                soc_mem_unlock(unit, PORT_TABm);
            }
        }
        if (egr_map != -1) { /* -1 means no change */
            /* Copy the corresponding chunk from EGR_MPLS profiles to the 
             * EGR_PRI_CNG_MAP table, which is directly indexed by port */
            alloc_size = 64 * sizeof (egr_pri_cng_map_entry_t);
            buf = soc_cm_salloc(unit, alloc_size, "TR2 egr pri cng map");
            if (NULL == buf) {
                QOS_UNLOCK(unit);
                return (BCM_E_MEMORY);
            }
            sal_memset(buf, 0, alloc_size);
            index = port << 6;
            if (egr_map == 0) {
                /* Clear the existing map */
                rv = soc_mem_write_range(unit, EGR_PRI_CNG_MAPm, MEM_BLOCK_ALL, 
                                         index, index + 63, (void *)buf);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return rv;
                }
            } else {
                if ((egr_map >> _BCM_QOS_MAP_SHIFT) != 
                     _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
                id = egr_map & _BCM_QOS_MAP_TYPE_MASK;
                if (!_BCM_QOS_EGR_MPLS_USED_GET(unit, id)) {
                    QOS_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
                buf2 = soc_cm_salloc(unit, alloc_size, "TR2 egr mpls exp map");
                if (NULL == buf2) {
                    soc_cm_sfree(unit, buf);
                    QOS_UNLOCK(unit);
                    return (BCM_E_MEMORY);
                }
                sal_memset(buf2, 0, alloc_size);
                index = QOS_INFO(unit)->egr_mpls_hw_idx[id] * 64;
                rv = soc_mem_read_range(unit, EGR_MPLS_EXP_MAPPING_1m,  
                                        MEM_BLOCK_ANY, index, index + 63, 
                                        (void *)buf2);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, buf);
                    soc_cm_sfree(unit, buf2);
                    QOS_UNLOCK(unit);
                    return rv;
                }
                for (index = 0; index < 64; index++) {
                    entries[0] = soc_mem_table_idx_to_pointer(unit, 
                                                              EGR_MPLS_EXP_MAPPING_1m, 
                                                              void *, buf2, index);
                    pri = soc_mem_field32_get(unit, EGR_MPLS_EXP_MAPPING_1m, 
                                              entries[0], PRIf);
                    cfi = soc_mem_field32_get(unit, EGR_MPLS_EXP_MAPPING_1m, 
                                              entries[0], CFIf);
                    soc_mem_field32_set(unit, EGR_PRI_CNG_MAPm, buf + index, 
                                        PRIf, pri);
                    soc_mem_field32_set(unit, EGR_PRI_CNG_MAPm, buf + index, 
                                        CFIf, cfi);
                } 
                index = port << 6;
                rv = soc_mem_write_range(unit, EGR_PRI_CNG_MAPm, MEM_BLOCK_ALL, 
                                         index, index + 63, (void *)buf);
                if (BCM_FAILURE(rv)) {
                    soc_cm_sfree(unit, buf);
                    soc_cm_sfree(unit, buf2);
                    QOS_UNLOCK(unit);
                    return rv;
                }
                soc_cm_sfree(unit, buf2);
            }
            soc_cm_sfree(unit, buf);
        }
    }
    QOS_UNLOCK(unit);
    return rv;
}

#endif
