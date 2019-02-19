/*
 * $Id$
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
 * common defines between v4 and v6-128 code 
 */

#ifndef _ESW_TRIDENT2_ALPM_INT_H
#define _ESW_TRIDENT2_ALPM_INT_H

#include <soc/esw/trie.h>
#include <soc/tomahawk.h>

#define SOC_ALPM_MODE_PARALLEL  1
#define SOC_ALPM_MODE_TCAM_ALPM  2

#define _MAX_KEY_LEN_   144
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#define VRF_ID_LEN      (12)
#elif defined(BCM_TOMAHAWK_SUPPORT)
#define VRF_ID_LEN      (11)
#else
#define VRF_ID_LEN      (10)
#endif

#define MAX_VRF_ID   ((1 << VRF_ID_LEN) + 1)
#define _MAX_KEY_WORDS_ (BITS2WORDS(_MAX_KEY_LEN_))

#define SOC_ALPM_GET_GLOBAL_BANK_DISABLE(u, bank_disable)\
do {\
    (bank_disable) = soc_alpm_mode_get((u));\
    if ((bank_disable)) {\
        /* parallel mode. Nothing for combined mode */\
        if (SOC_URPF_STATUS_GET((u))) {\
            (bank_disable) = 0x3; /* Search in banks, B2 and B3 */\
        } else {\
            (bank_disable) = 0;\
        }\
    }\
} while (0)

#define SOC_ALPM_GET_VRF_BANK_DISABLE(u, bank_disable)\
do {\
    (bank_disable) = soc_alpm_mode_get((u));\
    if ((bank_disable)) {\
        /* parallel mode. Nothing for combined mode */\
        if (SOC_URPF_STATUS_GET((u))) {\
            (bank_disable) = 0xC; /* Search in banks, B1 and B0 */\
        } else {\
            (bank_disable) = 0;\
        }\
    }\
} while (0)

#define SOC_ALPM_TH_GET_GLOBAL_BANK_DISABLE(u, bank_disable)\
do {\
    (bank_disable) = soc_th_alpm_mode_get((u));\
    if ((bank_disable)) {\
        /* parallel mode. Nothing for combined mode */\
        if (SOC_URPF_STATUS_GET((u))) {\
            (bank_disable) = 0x3; /* Search in banks, B2 and B3 */\
        } else {\
            (bank_disable) = 0;\
        }\
    }\
} while (0)

#define SOC_ALPM_TH_GET_VRF_BANK_DISABLE(u, bank_disable)\
do {\
    (bank_disable) = soc_th_alpm_mode_get((u));\
    if ((bank_disable)) {\
        /* parallel mode. Nothing for combined mode */\
        if (SOC_URPF_STATUS_GET((u))) {\
            (bank_disable) = 0xC; /* Search in banks, B1 and B0 */\
        } else {\
            (bank_disable) = 0;\
        }\
    }\
} while (0)

#define SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(u, bank_disable)\
do {\
    (bank_disable) = soc_th_alpm_mode_get((u));\
    if (((bank_disable) == 1) && (SOC_URPF_STATUS_GET((u)))) {\
        /* parallel mode. */ \
        (bank_disable) = 0xD; /* Search in banks, B1 */\
    } else {\
        (bank_disable) = 0xC;\
    }\
} while (0)
 
#define SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(u, bank_disable)\
do {\
    (bank_disable) = soc_th_alpm_mode_get((u));\
    if (((bank_disable) == 1) && (SOC_URPF_STATUS_GET((u)))) {\
        /* parallel mode. */ \
        (bank_disable) = 0xE; /* Search in banks, B0 */\
    } else {\
        (bank_disable) = 0xC;\
    }\
} while (0)

#define ALPM_PREFIX_IN_TCAM(u, vrf_id) ((vrf_id == SOC_L3_VRF_OVERRIDE) || \
                  ((soc_th_alpm_mode_get(u) == SOC_ALPM_MODE_TCAM_ALPM) && \
                                        (vrf_id == SOC_L3_VRF_GLOBAL))) 

/* Bucket Management Functions */
/* The Buckets from the shared RAM are assigned to TCAM PIVOTS to store
 * prefixes. The bucket once assigned to a TCAM PIVOT will be in use till the
 * PIVOT is active (at least one Prefix is used in the bucket). If no entries
 * are in the bucket, the bucket can be treated as free. The bucket pointers
 * follow TCAM entries when the TCAM entires are moved up or down to make
 * space.
 */

/* Shared bitmap routines are used to track bucket usage 
 * For TH, in Parallel mode with uRPF enable, Global low & Private routes can 
 * share same physical bucket, separate bmap are used for tracking each bucket 
 * usage, for other mode or in TD2, they just point to same memory address */
typedef struct soc_alpm_bucket_s {
    SHR_BITDCL *alpm_vrf_bucket_bmap;
    SHR_BITDCL *alpm_glb_bucket_bmap;
    int alpm_bucket_bmap_size;
    int bucket_count;
    int next_free;
} soc_alpm_bucket_t;

extern soc_alpm_bucket_t soc_alpm_bucket[];
/* For TD2, we just use one bmap, glb bmap is not used */
#define SOC_ALPM_BUCKET_BMAP(u)         (soc_alpm_bucket[u].alpm_vrf_bucket_bmap)
#define SOC_ALPM_BUCKET_BMAP_BYTE(u,i)  (soc_alpm_bucket[u].alpm_bucket_bmap[i])
#define SOC_ALPM_BUCKET_BMAP_SIZE(u)    (soc_alpm_bucket[u].alpm_bucket_bmap_size)
#define SOC_ALPM_BUCKET_NEXT_FREE(u)    (soc_alpm_bucket[u].next_free)
#define SOC_ALPM_BUCKET_COUNT(u)        (soc_alpm_bucket[u].bucket_count)
#define SOC_ALPM_BUCKET_MAX_INDEX(u)    (soc_alpm_bucket[u].bucket_count - 1)

#define SOC_ALPM_RPF_BKT_IDX(u, bkt)    \
    (bkt + SOC_ALPM_BUCKET_COUNT(u))

/* For TH, in Parallel mode, we combine the buckets */
extern soc_alpm_bucket_t soc_th_alpm_bucket[];

#define SOC_TH_ALPM_VRF_BUCKET_BMAP(u)      \
            (soc_th_alpm_bucket[u].alpm_vrf_bucket_bmap)
#define SOC_TH_ALPM_GLB_BUCKET_BMAP(u)      \
            (soc_th_alpm_bucket[u].alpm_glb_bucket_bmap)
#define SOC_TH_ALPM_BUCKET_BMAP_BYTE(u,i)   \
            (soc_th_alpm_bucket[u].alpm_bucket_bmap[i])
#define SOC_TH_ALPM_BUCKET_BMAP_SIZE(u)     \
            (soc_th_alpm_bucket[u].alpm_bucket_bmap_size)
#define SOC_TH_ALPM_BUCKET_NEXT_FREE(u)     \
            (soc_th_alpm_bucket[u].next_free)
#define SOC_TH_ALPM_BUCKET_COUNT(u)         \
            (soc_th_alpm_bucket[u].bucket_count)
#define SOC_TH_ALPM_BUCKET_MAX_INDEX(u)     \
            (soc_th_alpm_bucket[u].bucket_count - 1)
#define SOC_TH_ALPM_RPF_BKT_IDX(u, bkt)     \
            (bkt + SOC_TH_ALPM_BUCKET_COUNT(u))

#define PRESERVE_HIT                    TRUE

typedef struct _payload_s payload_t;
struct _payload_s {
    trie_node_t node; /*trie node */
    payload_t *next; /* list node */
    unsigned int key[BITS2WORDS(_MAX_KEY_LEN_)];
    unsigned int len;
    int index;   /* Memory location */
    payload_t *bkt_ptr;
};

/*
 * Table Operations for ALPM
 */

/* Generic AUX operation function */
typedef enum _soc_aux_op_s {
    INSERT_PROPAGATE,
    DELETE_PROPAGATE,
    PREFIX_LOOKUP,
    HITBIT_REPLACE
}_soc_aux_op_t;

extern int _soc_alpm_aux_op(int u, _soc_aux_op_t aux_op, 
                 defip_aux_scratch_entry_t *aux_entry, int update_scratch, 
                 int *hit, int *tcam_index, int *bucket_index);
extern int _ipmask2pfx(uint32 ipv4m, int *mask_len);

extern int alpm_bucket_assign(int u, int *bucket_pointer, int v6);
extern int alpm_bucket_release(int u, int bucket_pointer, int v6);
extern int alpm_bucket_is_assigned(int u, int bucket_ptr, int ipv6, int *used);

extern int soc_th_alpm_bucket_assign(int u, int *bucket_pointer, int vrf, int v6);
extern int soc_th_alpm_bucket_release(int u, int bucket_pointer, int vrf, int v6);
extern int soc_th_alpm_bucket_is_assigned(int u, int bucket_ptr, int vrf, int v6, int *used);

/* Debug counter structure */
typedef struct alpm_vrf_counter_s {
    uint32 v4;
    uint32 v6_64;
    uint32 v6_128;
} alpm_vrf_counter_t;

/* Per VRF PIVOT and Prefix trie. Each VRF will host a trie based on IPv4, IPV6-64 and IPV6-128
 * This seperation reduces the complexity of trie management.
 */
typedef struct alpm_vrf_handle_s {
    trie_t *pivot_trie_ipv4;     /* IPV4 Pivot trie */
    trie_t *pivot_trie_ipv6;     /* IPV6-64 Pivot trie */
    trie_t *pivot_trie_ipv6_128; /* IPV6-128 Pivot trie */    
    trie_t *prefix_trie_ipv4;    /* IPV4 Pivot trie */
    trie_t *prefix_trie_ipv6;    /* IPV6-64 Pivot trie */
    trie_t *prefix_trie_ipv6_128;    /* IPV6-128 Prefix trie */
    defip_entry_t *lpm_entry;          /* IPv4 Default LPM entry */
    defip_entry_t *lpm_entry_v6;       /* IPv6 Default LPM entry */
    defip_pair_128_entry_t *lpm_entry_v6_128;   /* IPv6-128 Default LPM entry */
    int count_v4;                /* no. of routes for this vrf */
    int count_v6_64;
    int count_v6_128;
    
    /* Debug counter */
    alpm_vrf_counter_t add;
    alpm_vrf_counter_t del;
    alpm_vrf_counter_t bkt_split;
    alpm_vrf_counter_t pivot_used;
    alpm_vrf_counter_t lpm_full;
    alpm_vrf_counter_t lpm_shift;
    
    int init_done;               /* Init for VRF completed */
                                 /* ready to accept route additions */
} alpm_vrf_handle_t;

/* Use MAX_VRF_ID to store VRF_OVERRIDE routes debug info */
extern alpm_vrf_handle_t *alpm_vrf_handle[SOC_MAX_NUM_DEVICES];

/* 
 * Bucket Hnadle
 */
typedef struct alpm_bucket_handle_s {
    trie_t *bucket_trie;   /* trie of Prefix within this bucket */
    int bucket_index;      /* bucket Pointer */
    
    /* Debug counter */
    int vrf_id;
    uint16 ipv6;
    uint16 def;
    uint16 min;
    uint16 max;
    
} alpm_bucket_handle_t;

/*
 * Pivot Structure
 */
typedef struct alpm_pivot_s {
    trie_node_t node; /*trie node */
    /* dq_t        listnode;*/ /* list node */
    alpm_bucket_handle_t *bucket;  /* Bucket trie */
    unsigned int key[BITS2WORDS(_MAX_KEY_LEN_)];  /* pivot */
    unsigned int len;                             /* pivot length */
    int tcam_index;   /* TCAM index where the pivot is inserted */
} alpm_pivot_t;

/* Bucket sharing Data Structure */

#define BKTID_SHIFT                 (3)
#define BKTID_MASK                  ((1U << BKTID_SHIFT) - 1)
#define ALPM_BKTID(bkt, sbkt)       (((bkt) << BKTID_SHIFT) | ((sbkt) & BKTID_MASK))
#define ALPM_BKT_IDX(bid)           (((bid) >> BKTID_SHIFT) & SOC_TH_ALPM_BKT_MASK)
#define ALPM_BKT_SIDX(bid)          ((bid) & BKTID_MASK)

#define SOC_TH_MAX_ALPM_BUCKETS     (8192)  /* Only 8K for Tomahawk         */
#define SOC_TH_MAX_ALPM_VIEWS       (3)     /* Ipv4, IPv6-64, IPv6-128      */
#define SOC_TH_MAX_BUCKET_ENTRIES   (48 + 1)/* 48 for v4 double wide mode   */
#define SOC_TH_MAX_SUB_BUCKETS      (4)     /* Max to 4 sub bucket in TH    */

typedef struct alpm_bkt_usg_s {
    uint8 count;        /* No. of entries in physical bucket                */
    uint8 sub_bkts;     /* Bitmap of logical buckets                        */
    uint8 vrf_type[4];  /* Different vrf type can't be merged (Glb or vrf)  */
    int16 pivots[4];    /* Pivot indices corresponding to logical buckets   */
} alpm_bkt_usg_t;

typedef struct alpm_bkt_bmp_s {
    uint32 bkt_count;
    SHR_BITDCLNAME(bkt_bmp, SOC_TH_MAX_ALPM_BUCKETS);
} alpm_bkt_bmp_t;

#define SOC_ALPM_BS_BKT_USAGE_COUNT(u, bidx)   \
    bkt_usage[(u)][(bidx)].count

#define SOC_ALPM_BS_BKT_USAGE_PIVOT(u, bidx) \
    bkt_usage[(u)][ALPM_BKT_IDX(bidx)].pivots[ALPM_BKT_SIDX(bidx)]

#define SOC_ALPM_BS_BKT_USAGE_VRF(u, bidx) \
    bkt_usage[(u)][ALPM_BKT_IDX(bidx)].vrf_type[ALPM_BKT_SIDX(bidx)]

#define SOC_ALPM_BS_BKT_USAGE_SB_ADD(u, bidx, sbidx, tcam_idx, vrf, diff) \
    do { \
        alpm_bkt_usg_t *_bu = &bkt_usage[(u)][(bidx)]; \
        _bu->count += diff; \
        if (vrf != -1) { \
            _bu->vrf_type[(sbidx)] = vrf; \
        } \
        _bu->sub_bkts |= 1U << (sbidx); \
        _bu->pivots[(sbidx)] = (tcam_idx); \
    } while (0)

#define SOC_ALPM_BS_BKT_USAGE_SB_DEL(u, bidx, sbidx, vrf_or_clear, diff) \
    do { \
        alpm_bkt_usg_t *_bu = &bkt_usage[(u)][(bidx)]; \
        _bu->count -= (diff); \
        if (vrf_or_clear || _bu->count == 0) { \
            _bu->sub_bkts &= ~(1U << (sbidx)); \
            _bu->pivots[(sbidx)] = 0; \
            _bu->vrf_type[(sbidx)] = 0; \
        } \
    } while (0)

#define SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(u, v6, bidx) \
    do { \
        int _count = SOC_ALPM_BS_BKT_USAGE_COUNT(u, bidx); \
        if (_count > 0) { \
            global_bkt_usage[(u)][(v6)][_count].bkt_count++; \
            SHR_BITSET(global_bkt_usage[(u)][(v6)][_count].bkt_bmp, (bidx)); \
        } \
    } while (0)

#define SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(u, v6, bidx) \
    do { \
        int _count = SOC_ALPM_BS_BKT_USAGE_COUNT(u, bidx); \
        if (_count > 0) { \
            global_bkt_usage[(u)][(v6)][_count].bkt_count--; \
            SHR_BITCLR(global_bkt_usage[(u)][(v6)][_count].bkt_bmp, (bidx)); \
        } \
    } while (0)

#define SOC_ALPM_GLOBAL_BKT_USAGE_CNT_DEC(u, v6, count) \
    do { \
        global_bkt_usage[(u)][(v6)][(count)].bkt_count--; \
    } while (0) 
#define SOC_ALPM_GLOBAL_BKT_USAGE_CNT_INC(u, v6, count) \
    do { \
        global_bkt_usage[(u)][(v6)][(count)].bkt_count++; \
    } while (0)


#define SOC_TH_ALPM_BKT_ENTRY_TO_IDX(u, idx)       \
            (ALPM_BKTID((((idx) >> (soc_th_get_alpm_banks(u) / 2)) & SOC_TH_ALPM_BKT_MASK), 0))
/* given bucket index shift left to bucket entry idx position */
#define SOC_TH_ALPM_BKT_IDX_TO_ENTRY(u, idx)       \
            (ALPM_BKT_IDX(idx) << (soc_th_get_alpm_banks(u) / 2))

/* Used for store returned entry_index in lookup function 
 * defip_index = 
 * -----------------------------------------------------
 * | 3 bits (sub bucket index) | 29 bits (entry index) |
 * ----------------------------------------------------- 
 */
#define ALPM_ENT_INDEX_BITS     (32 - BKTID_SHIFT)
#define ALPM_ENT_INDEX_MASK     ((1 << ALPM_ENT_INDEX_BITS) - 1)
#define ALPM_ENT_INDEX(idx)     ((idx) & ALPM_ENT_INDEX_MASK)

#define SOC_TH_ALPM_BUCKET_BITS (13)

extern alpm_bkt_usg_t   *bkt_usage[SOC_MAX_NUM_DEVICES];
extern alpm_bkt_bmp_t   *global_bkt_usage[SOC_MAX_NUM_DEVICES][SOC_TH_MAX_ALPM_VIEWS];

/* TCAM Pivot management */

#define MAX_PIVOT_COUNT         (16384)

/* Array of Pivots */
extern alpm_pivot_t **tcam_pivot[SOC_MAX_NUM_DEVICES];

#define ALPM_TCAM_PIVOT(u, index)    tcam_pivot[u][index]

#define PIVOT_BUCKET_HANDLE(p)    (p)->bucket
#define PIVOT_BUCKET_TRIE(p)      ((p)->bucket)->bucket_trie
#define PIVOT_BUCKET_INDEX(p)     ((p)->bucket)->bucket_index

/* Debug counter */
#define PIVOT_BUCKET_VRF(p)       ((p)->bucket)->vrf_id
#define PIVOT_BUCKET_IPV6(p)      ((p)->bucket)->ipv6
#define PIVOT_BUCKET_DEF(p)       ((p)->bucket)->def
#define PIVOT_BUCKET_MIN(p)       ((p)->bucket)->min
#define PIVOT_BUCKET_MAX(p)       ((p)->bucket)->max
#define PIVOT_BUCKET_COUNT(p)     PIVOT_BUCKET_TRIE(p)->trie->count

#define PIVOT_BUCKET_ENT_CNT_UPDATE(p)      \
    if (PIVOT_BUCKET_TRIE(p)->trie != NULL) { \
        int _count = PIVOT_BUCKET_COUNT(p); \
        if (((p)->bucket)->min == 0) {      \
            ((p)->bucket)->min = _count;    \
        } else if (_count < ((p)->bucket)->min) { \
            ((p)->bucket)->min = _count;    \
        }                                   \
        if (_count > ((p)->bucket)->max) {  \
            ((p)->bucket)->max = _count;    \
        }                                   \
    }

#define PIVOT_TCAM_INDEX(p)       ((p)->tcam_index)


#define VRF_PIVOT_TRIE_IPV4(u, vrf)         \
    alpm_vrf_handle[u][vrf].pivot_trie_ipv4
#define VRF_PIVOT_TRIE_IPV6(u, vrf)         \
    alpm_vrf_handle[u][vrf].pivot_trie_ipv6
#define VRF_PIVOT_TRIE_IPV6_128(u, vrf)     \
    alpm_vrf_handle[u][vrf].pivot_trie_ipv6_128
#define VRF_PREFIX_TRIE_IPV4(u, vrf)        \
    alpm_vrf_handle[u][vrf].prefix_trie_ipv4
#define VRF_PREFIX_TRIE_IPV6(u, vrf)        \
    alpm_vrf_handle[u][vrf].prefix_trie_ipv6
#define VRF_PREFIX_TRIE_IPV6_128(u, vrf)    \
    alpm_vrf_handle[u][vrf].prefix_trie_ipv6_128

#define L3_DEFIP_MODE_V4        (0)
#define L3_DEFIP_MODE_64        (1)
#define L3_DEFIP_MODE_128       (2)

#define VRF_TRIE_INIT_DONE(u, vrf, v6, val)      \
do {\
    alpm_vrf_handle[u][vrf].init_done &= ~(1 << (v6));\
    alpm_vrf_handle[(u)][(vrf)].init_done |= ((val) & 1) << (v6);\
} while (0) 

#define VRF_TRIE_INIT_COMPLETED(u, vrf, v6)     \
    ((alpm_vrf_handle[u][vrf].init_done & (1 << (v6))) != 0)

#define VRF_TRIE_DEFAULT_ROUTE_IPV4(u, vrf)     \
    alpm_vrf_handle[u][vrf].lpm_entry
#define VRF_TRIE_DEFAULT_ROUTE_IPV6(u, vrf)     \
    alpm_vrf_handle[u][vrf].lpm_entry_v6
#define VRF_TRIE_DEFAULT_ROUTE_IPV6_128(u, vrf) \
    alpm_vrf_handle[u][vrf].lpm_entry_v6_128

#define VRF_PIVOT_FULL_INC(u, vrf, v6)                  \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].lpm_full.v4++;      \
    } else if ((v6) == 1) {                             \
        alpm_vrf_handle[(u)][(vrf)].lpm_full.v6_64++;   \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].lpm_full.v6_128++;  \
    }                                                   \
} while (0)

#define VRF_PIVOT_SHIFT_INC(u, vrf, v6)                 \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].lpm_shift.v4++;     \
    } else if ((v6) == 1) {                             \
        alpm_vrf_handle[(u)][(vrf)].lpm_shift.v6_64++;  \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].lpm_shift.v6_128++; \
    }                                                   \
} while (0)

#define VRF_PIVOT_REF_INC(u, vrf, v6)                   \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].pivot_used.v4++;    \
    } else if ((v6) == 1) {                             \
        alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_64++; \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_128++;\
    }                                                   \
} while (0)

#define VRF_PIVOT_REF_DEC(u, vrf, v6)                   \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].pivot_used.v4--;    \
    } else if ((v6) == 1) {                             \
        alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_64--; \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].pivot_used.v6_128--;\
    }                                                   \
} while (0)

#define VRF_BUCKET_SPLIT_INC(u, vrf, v6)                \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].bkt_split.v4++;     \
    } else if ((v6) == L3_DEFIP_MODE_64) {              \
        alpm_vrf_handle[(u)][(vrf)].bkt_split.v6_64++;  \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].bkt_split.v6_128++; \
    }                                                   \
} while (0)

#define VRF_TRIE_ROUTES_INC(u, vrf, v6)                 \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].count_v4++;         \
        alpm_vrf_handle[(u)][(vrf)].add.v4++;           \
    } else if ((v6) == L3_DEFIP_MODE_64) {              \
        alpm_vrf_handle[(u)][(vrf)].count_v6_64++;      \
        alpm_vrf_handle[(u)][(vrf)].add.v6_64++;        \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].count_v6_128++;     \
        alpm_vrf_handle[(u)][(vrf)].add.v6_128++;       \
    }                                                   \
} while (0)

#define VRF_TRIE_ROUTES_DEC(u, vrf, v6)                 \
do {                                                    \
    if (!(v6)) {                                        \
        alpm_vrf_handle[(u)][(vrf)].count_v4--;         \
        alpm_vrf_handle[(u)][(vrf)].del.v4++;           \
    } else if ((v6) == L3_DEFIP_MODE_64) {              \
        alpm_vrf_handle[(u)][(vrf)].count_v6_64--;      \
        alpm_vrf_handle[(u)][(vrf)].del.v6_64++;        \
    } else {                                            \
        alpm_vrf_handle[(u)][(vrf)].count_v6_128--;     \
        alpm_vrf_handle[(u)][(vrf)].del.v6_128++;       \
    }                                                   \
} while (0)
 
#define VRF_TRIE_ROUTES_CNT(u, vrf, v6)                 \
    (((v6) == 0) ?                                      \
    alpm_vrf_handle[(u)][(vrf)].count_v4 :              \
    (((v6) == L3_DEFIP_MODE_64) ?                       \
    alpm_vrf_handle[(u)][(vrf)].count_v6_64 :           \
    alpm_vrf_handle[(u)][(vrf)].count_v6_128))

/* Used to store the list of Prefixes that need to be moved to new bucket */
#define MAX_PREFIX_PER_BUCKET 64
#define SOC_ALPM_LPM_LOCK(u)             soc_mem_lock(u, L3_DEFIPm)
#define SOC_ALPM_LPM_UNLOCK(u)           soc_mem_unlock(u, L3_DEFIPm)

typedef struct {
    payload_t *prefix[MAX_PREFIX_PER_BUCKET];
    /* indicates success or failure on bucket split move */
    int status[MAX_PREFIX_PER_BUCKET];
    int count;
} alpm_mem_prefix_array_t;

typedef struct _alpm__pfx_info_s {
    void *key_data;
    payload_t *new_pfx_pyld;
    alpm_pivot_t *pivot_pyld; 
    /* old pivot payload is passed in, and new pivot info is returned */
    void *alpm_data;
    void *alpm_sip_data;
    int bktid;
} alpm_pfx_info_t;

extern int _soc_alpm_rpf_entry(int u, int idx);
extern int _soc_alpm_find_in_bkt(int u, soc_mem_t mem, int bucket_index, 
                                 int bank_disable, uint32 *e, void *alpm_data, 
                                 int *key_index, int v6);
extern int alpm_mem_prefix_array_cb(trie_node_t *node, void *info);
extern int alpm_delete_node_cb(trie_node_t *node, void *info);
extern int _soc_alpm_insert_in_bkt(int u, soc_mem_t mem, int bucket_index,
                                   int bank_disable, void *alpm_data, 
                                   uint32 *e, int *key_index, int v6);
extern int _soc_alpm_delete_in_bkt(int u, soc_mem_t mem, int delete_bucket, 
                                   int bank_disable, void *bufp2, uint32 *e, 
                                   int *key_index, int v6);
extern int soc_alpm_128_init(int u);
extern int soc_alpm_128_state_clear(int u);
extern int soc_alpm_128_deinit(int u);
extern int soc_alpm_128_lpm_init(int u);
extern int soc_alpm_128_lpm_deinit(int u);
extern int soc_alpm_physical_idx(int u, soc_mem_t mem, int index, int full);
extern int soc_alpm_logical_idx(int u, soc_mem_t mem, int index, int full);

extern int _soc_alpm_mem_index(int u, soc_mem_t mem, int bucket_index, 
                               int offset, uint32 bank_disable, int *key_index);
extern int _soc_alpm_raw_bucket_read(int u, soc_mem_t mem, int bucket_index, 
                                     void *raw_entry, void *raw_sip_entry);
extern int _soc_alpm_raw_bucket_write(int u, soc_mem_t mem, int bucket_index, 
                                      uint32 bank_disable,
                                      void *raw_entry, void *raw_sip_entry,
                                      int entry_cnt);
extern void _soc_alpm_raw_mem_read(int unit, soc_mem_t mem, void *raw, 
                                   int index, void *entry);
extern void _soc_alpm_raw_mem_write(int unit, soc_mem_t mem, void *raw, 
                                    int index, void *entry);
extern void _soc_alpm_raw_parity_set(int u, soc_mem_t mem, void *alpm_data);

/* For TH */
extern void soc_th_alpm_dbg_urpf(int u);
extern int _soc_th_alpm_mem_prefix_array_cb(trie_node_t *node, void *info);
extern int soc_th_alpm_update_hit_bits(int u, int count, int *ori_hptr, int *new_hptr);
extern int _soc_th_alpm_aux_op(int u, _soc_aux_op_t aux_op, 
                               defip_aux_scratch_entry_t *aux_entry, int update_scratch, 
                               int *hit, int *tcam_index, int *bktid);
extern int _soc_th_alpm_rpf_entry(int u, int idx);
extern int _soc_th_alpm_find_in_bkt(int u, soc_mem_t mem, int bktid, 
                                    int bank_disable, uint32 *e, void *alpm_data, 
                                    int *key_index, int v6);
extern int _soc_th_alpm_insert_in_bkt(int u, soc_mem_t mem, int bktid,
                                      int bank_disable, void *alpm_data, 
                                      uint32 *e, int *key_index, int v6);
extern int _soc_th_alpm_delete_in_bkt(int u, soc_mem_t mem, int delete_bucket, 
                                      int bank_disable, void *bufp2, uint32 *e, 
                                      int *key_index, int v6);
extern int soc_th_alpm_128_init(int u);
extern int soc_th_alpm_128_state_clear(int u);
extern int soc_th_alpm_128_deinit(int u);
extern int soc_th_alpm_128_lpm_init(int u);
extern int soc_th_alpm_128_lpm_deinit(int u);
extern int soc_th_alpm_physical_idx(int u, soc_mem_t mem, int index, int full);
extern int soc_th_alpm_logical_idx(int u, soc_mem_t mem, int index, int full);
extern int _soc_th_alpm_mem_index(int u, soc_mem_t mem, int bucket_index, 
                                  int offset, uint32 bank_disable, int *key_index);

extern void soc_th_alpm_bank_db_type_get(int u, int vrf, uint32 *bkdis, uint32 *db_type);
extern int _soc_th_alpm_bkt_entry_cnt(int u, int v6);

/* For bucket sharing */
extern int _soc_th_alpm_128_move_trie(int u, int v6, int from_bkt, int to_bkt);
extern int soc_th_alpm_bs_merge(int u, int v6, int bkt1, int bkt2);
extern int soc_th_alpm_bs_alloc(int u, int *bktid, int vrf, int v6);
extern int soc_th_alpm_bs_free(int u, int bucket, int vrf, int v6);
extern int soc_th_alpm_bu_upd(int u, int bucket, int tcam_index, 
                              int vrf_or_clear, int v6, int diff);

#endif /* _ESW_TRIDENT2_ALPM_INT_H */
