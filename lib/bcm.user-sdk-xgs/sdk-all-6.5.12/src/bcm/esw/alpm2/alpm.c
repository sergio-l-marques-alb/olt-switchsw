/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $Id: alpm.c$
 * File:    alpm.c
 * Purpose: ALPM device independent implementation.
 * Requires:
 */

/* Implementation notes:
 */
#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <shared/bsl.h>

#include <shared/util.h>
#include <shared/l3.h>

#if defined(ALPM_ENABLE)

#include <soc/esw/trie.h>
#include <soc/esw/alpm_trie_v6.h>

#include <bcm/l3.h>

#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/alpm.h>
#include <bcm_int/esw/alpm_util.h>

/* Internal use only */

#define ALPM_FMT_DEF            (1)
#define ALPM_FMT_PKM_32B        (1)
#define ALPM_FMT_PKM_64B        (2)
#define ALPM_FMT_PKM_128        (3)
#define ALPM_FMT_PKM_FLEX_32B   (4)
#define ALPM_FMT_PKM_FLEX_64B   (5)
#define ALPM_FMT_PKM_FLEX_128   (6)

typedef struct alpm_pvt_trav_cb_data_s {
    int         unit;
    _alpm_cb_t  *acb;
    int         bkt_idx;

    SHR_BITDCL  bkt[256];                   /* 1K|8K, Max: 13 bits */
    SHR_BITDCL  bnk[ALPM_BPB_MAX * 256];    /* bnk|bkt 1K|5 or 8K|8 */
    SHR_BITDCL  ent[ALPM_BPB_MAX * 4096];   /* 10b/13b(bkt) |3b(bnk)| 4b(ent) */
    uint8       bnkfmt[ALPM_BPB_MAX * 8192];/* Bank format type */
} alpm_pvt_trav_cb_data_t;

char *alpm_tbl_str[] = {
    "alpmTblInvalid",
    "alpmTblPvtCmodeL1",
    "alpmTblPvtPmodeL1",
    "alpmTblPvtCmodeL1P128",
    "alpmTblPvtPmodeL1P128",
    "alpmTblBktCmodeL2",
    "alpmTblBktPmodeL2",
    "alpmTblBktPmodeGblL2",
    "alpmTblBktPmodePrtL2",
    "alpmTblBktCmodeL3",
    "alpmTblBktPmodeGblL3",
    "alpmTblBktPmodePrtL3",
    "alpmTblCount"
};

_alpm_ctrl_t *alpm_control[SOC_MAX_NUM_DEVICES];

/* Internal functions */
static int alpm_pvt_update_by_pvt_node(int, _alpm_cb_t *, _alpm_pvt_node_t *);

/* External functions */

/*
 *           key[4] = {key[0], key[1], key[2], key[3]}
 * ipv4:     key[4] =
 *               {IP_ADDR0f, 0, 0, 0}
 * ipv6-64:  key[4] =
 *               {IP_ADDR0f, IP_ADDR1f, 0, 0}
 * ipv6-128: key[4] =
 *               {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
 */
void
alpm_trie_key_to_pfx(int u, int pkm, uint32 *key, uint32 len, uint32 *pfx)
{
    int i;
    uint32 pfx_shift, tmp;

    pfx[0] = pfx[1] = pfx[2] = pfx[3] = pfx[4] = 0;
    /* Obtain prefix length */
    if (pkm == ALPM_PKM_32B) {
        pfx[1] = _SHIFT_RIGHT(key[0], 32 - len);
        pfx[0] = 0;
    } else if (pkm == ALPM_PKM_64B) {
        pfx[4] = key[0];
        pfx[3] = key[1];

        pfx_shift = 64 - len;
        if (pfx_shift < 32) {
            /* get lower half shifted */
            pfx[4] >>= pfx_shift;
            /* isolate shifted stuff of upper half */
            tmp = _SHIFT_LEFT(pfx[3], 32 - pfx_shift);
            pfx[3] >>= pfx_shift;
            pfx[4] |= tmp;
        } else {
            pfx[4] = _SHIFT_RIGHT(pfx[3], pfx_shift - 32);
            pfx[3] = 0;
        }
    } else {
        /* PAIR_128 style, for both mode==2 and mode==3  */
        int start;
        uint32 tmp_pfx[5] = {0};
        tmp_pfx[0] = key[0];
        tmp_pfx[1] = key[1];
        tmp_pfx[2] = key[2];
        tmp_pfx[3] = key[3];

        /* shift entire prefix right to remove trailing 0s in prefix */
        /* to simplify, words which are all 0s are ignored to begin with, so
         * a right shift of say 46 is a right shift of 46-32 = 14, but starting
         * from word 1. Once all shifting is done, shift all words to right by 1
         */
        pfx_shift = 128 - len;
        start = pfx_shift / 32;
        pfx_shift = pfx_shift % 32;
        for (i = start; i < 4; i++) {
            tmp_pfx[i] >>= pfx_shift;
            tmp = tmp_pfx[i+1] & ((1 << pfx_shift) - 1);
            tmp = _SHIFT_LEFT(tmp, 32 - pfx_shift);
            tmp_pfx[i] |= tmp;
        }
        /* make shift right justified and also reverse order for trie.
         * Also, note trie sees key only in words 1 to 4
         */
        for (i = start; i < 4; i++) {
            pfx[4 - (i - start)] = tmp_pfx[i];
        }
    }
    return ;
}

/*
 *           key[4] = {key[0], key[1], key[2], key[3]}
 * ipv4:     key[4] =
 *               {IP_ADDR0f, 0, 0, 0}
 * ipv6-64:  key[4] =
 *               {IP_ADDR0f, IP_ADDR1f, 0, 0}
 * ipv6-128: key[4] =
 *               {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
 */
void
alpm_trie_pfx_to_key(int u, int pkm, uint32 *pfx, uint32 len, uint32 *key)
{
    int key_bytes = sizeof(uint32) * BITS2WORDS(_MAX_KEY_LEN_144_);

    sal_memcpy(key, pfx, key_bytes);
    if (pkm == ALPM_PKM_128) {
        uint32 pfx_shift, tmp, start, pfx_tmp[6];
        int i;

        sal_memcpy(pfx_tmp, key, key_bytes);
        sal_memset(key, 0, key_bytes);

        /* shift entire prefix left to add trailing 0s in prefix */
        pfx_shift = 128 - len;
        start = (pfx_shift + 31) / 32;
        /* fix corner case: */
        if ((pfx_shift % 32) == 0) {
            start++;
        }
        pfx_shift = pfx_shift % 32;
        for (i = start; i <= 4; i++) {
            pfx_tmp[i] <<= pfx_shift;
            tmp = pfx_tmp[i+1] & ~(0xffffffff >> pfx_shift);
            tmp = _SHIFT_RIGHT(tmp, 32 - pfx_shift);
            if (i < 4) {
                pfx_tmp[i] |= tmp;
            }
        }
        /* make shift left justified and reversed,
         * i.e, lsw goes to index 0 */
        for (i = start; i <= 4; i++) {
            key[3 - (i - start)] = pfx_tmp[i];
        }
    } else {
        if (pkm == ALPM_PKM_32B) {
            key[0] = _SHIFT_LEFT(key[1], 32 - len);
            key[1] = 0;
        } else {
            int macro_pfx_shift = 64 - len;
            int macro_tmp;
            if (macro_pfx_shift < 32) {
                /* get upper half shifted */
                macro_tmp = key[3] << macro_pfx_shift;
                /* isolate shifted stuff off lower half */
                macro_tmp |= _SHIFT_RIGHT(key[4], 32 - macro_pfx_shift);
                /* swap the order of words */
                key[0] = key[4] << macro_pfx_shift;
                key[1] = macro_tmp;
                key[2] = key[3] = key[4] = 0;
            } else {
                key[1] = _SHIFT_LEFT(key[4], macro_pfx_shift - 32);
                key[0] = key[2] = key[3] = key[4] = 0;
            }
        }
    }
}

/* ip_addr => lpm_cfg->user_data
 * OUT: pfx in tire format
 *
 */
void
alpm_trie_cfg_to_pfx(int u, _bcm_defip_cfg_t *lpm_cfg, uint32 *pfx)
{
    /* Create Prefix */
    uint32 key[4] = {0};
    uint32 pkm = ALPM_LPM_PKM(u, lpm_cfg);

    alpm_util_cfg_to_key(u, lpm_cfg, key);
    alpm_trie_key_to_pfx(u, pkm, key, lpm_cfg->defip_sub_len, pfx);

    return ;
}

/* pfx in trie format => ip_addr
 * OUT: lpm_cfg->defip_ip6_addr/defip_ip_addr from trie format
 *
 */
void
alpm_trie_pfx_to_cfg(int u, uint32 *pfx, int pfx_len, _bcm_defip_cfg_t *lpm_cfg)
{
    /* Create Prefix */
    uint32 key[5] = {0};
    uint32 pkm = ALPM_LPM_PKM(u, lpm_cfg);

    alpm_trie_pfx_to_key(u, pkm, pfx, pfx_len, key);
    alpm_util_key_to_cfg(u, pkm, key, lpm_cfg);
    lpm_cfg->defip_sub_len = pfx_len;
    return ;
}

/* Convert TCAM entry to trie pfx format */
int
alpm_trie_ent_to_pfx(int u, int pkm, void *e, int sub_idx, int pfx_len,
                     uint32 *pfx)
{
    int rv = BCM_E_UNAVAIL;
    uint32 key[4] = {0};

    rv = tcam_entry_to_key(u, pkm, e, sub_idx, key);

    if (BCM_SUCCESS(rv)) {
        alpm_trie_key_to_pfx(u, pkm, key, pfx_len, pfx);
    }

    return rv;
}

static int
alpm_trie_pfx_cb(trie_node_t *node, void *user_data)
{
    _alpm_bkt_pfx_arr_t *pfx_arr = (_alpm_bkt_pfx_arr_t *)user_data;
    if (node->type == PAYLOAD) {
        pfx_arr->pfx[pfx_arr->pfx_cnt] = (_alpm_bkt_node_t *)node;
        pfx_arr->pfx_cnt++;
    }
    return BCM_E_NONE;
}

static int
alpm_trie_max_split_len_cb(trie_node_t *node, void *user_data)
{
    int *pfx_len = (int *)user_data;
    if (node->type == PAYLOAD) {
        uint32 key_len = ((_alpm_bkt_node_t *)node)->key_len;
        if (*pfx_len < key_len) {
            *pfx_len = key_len;
        }
    }
    return BCM_E_NONE;
}

static int
alpm_trie_max_split_len(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node)
{
    int split_len = 0;
    int rv, pvt_len = 0, key_len = 0;
    trie_t  *bkt_trie = NULL;

    if (ACB_BKT_FIXED_FMT(acb, PVT_BKT_VRF(pvt_node))) {
        int max_split_len[] = {32, 64, 128};
        split_len = max_split_len[PVT_BKT_PKM(pvt_node)] - 1;
    } else {
        pvt_len = PVT_KEY_LEN(pvt_node);
        bkt_trie = PVT_BKT_TRIE(pvt_node);

        rv = trie_traverse(bkt_trie, alpm_trie_max_split_len_cb, &key_len,
                           _TRIE_INORDER_TRAVERSE);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("trie_traverse(bkt_trie) failed (%d)", rv));
        }

        split_len = (pvt_len + key_len + 1) / 2;
    }

    return split_len;
}

static int
alpm_trie_node_delete_cb(trie_node_t *node, void *info)
{
    if (node != NULL) {
        alpm_util_free(node);
    }
    return BCM_E_NONE;
}

static int
alpm_trie_tree_destroy(int u, trie_t *trie)
{
    int rv = BCM_E_NONE;
    if (trie != NULL) {
        /* traverse bucket trie and clear it */
        rv = trie_traverse(trie, alpm_trie_node_delete_cb,
                           NULL, _TRIE_INORDER_TRAVERSE);
        if (BCM_SUCCESS(rv)) {
            trie_destroy(trie);
        }
    }

    return rv;
}

static int
alpm_trie_pvt_bkt_clear_cb(int u, _alpm_cb_t *acb,
                           _alpm_pvt_node_t *pvt_node, void *datum)
{
    int rv = BCM_E_NONE;
    trie_t *bkt_trie;

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    ALPM_IER(alpm_trie_tree_destroy(u, bkt_trie));
    PVT_BKT_TRIE(pvt_node) = NULL;

    return rv;
}

static int
alpm_trie_clear_all(int u)
{
    int i, rv = BCM_E_NONE;
    int pkm, vrf_id;
    _alpm_cb_t *acb;
    trie_t *pvt_trie = NULL, *vrf_trie = NULL;

    for (pkm = 0; pkm < ALPM_PKM_CNT; pkm ++) {
        for (vrf_id = 0; vrf_id <= ALPM_VRF_ID_MAX(u); vrf_id++) {
            for (i = 0; i < ACB_CNT(u); i++) {
                acb = ACB(u, i);
                if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
                    continue;
                }
                /* Clear Bucket Trie-tree */
                rv = bcm_esw_alpm_pvt_traverse(u, i, vrf_id, pkm,
                        alpm_trie_pvt_bkt_clear_cb, NULL);
                ALPM_IER(rv);

                /* Clear PVT Trie-tree */
                pvt_trie = ACB_PVT_TRIE(acb, vrf_id, pkm);
                ALPM_IER(alpm_trie_tree_destroy(u, pvt_trie));
                ACB_PVT_TRIE(acb, vrf_id, pkm) = NULL;
            }

            /* Clear Prefix Trie-tree */
            vrf_trie = ALPM_VRF_TRIE(u, vrf_id, pkm);
            ALPM_IER(alpm_trie_tree_destroy(u, vrf_trie));
            ALPM_VRF_TRIE(u, vrf_id, pkm) = NULL;
        }
    }

    return rv;
}

/* Fill Aux entry for operation */
static void
alpm_ppg_prepare(int u, _bcm_defip_cfg_t *lpm_cfg, int replace_len,
                 void *def_bkt_node, _alpm_ppg_data_t *ppt_data)
{
    sal_memset(ppt_data, 0, sizeof(_alpm_ppg_data_t));

    ppt_data->vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    ppt_data->pkm    = ALPM_LPM_PKM(u, lpm_cfg);
    ppt_data->key_len = lpm_cfg->defip_sub_len;
    ppt_data->bpm_len = replace_len;
    sal_memcpy(ppt_data->key, lpm_cfg->user_data, sizeof(ppt_data->key));
    /* Get assoc_data */
    ppt_data->bkt_def = def_bkt_node;

    return ;
}

static int
alpm_ppg(int u, trie_t *pivot, void *prefix, uint32 len,
         int insert, void *cb, void *user_data)
{
    int rv = BCM_E_NONE;
    uint32 pvt_len = 0;
    trie_node_t *lpm_pvt = NULL;
    trie_bpm_cb_info_t cb_info;

    cb_info.pfx = (void *)prefix;
    cb_info.len = len;
    cb_info.user_data = user_data;

    /* Find longest prefix match(pivot) of route */
    ALPM_IER(trie_find_lpm(pivot, prefix, len, &lpm_pvt));
    pvt_len = (TRIE_ELEMENT_GET(_alpm_pvt_node_t *, lpm_pvt, node))->key_len;

    if (pivot->v6_key) {
        rv = pvt_trie_v6_propagate_prefix(lpm_pvt, pvt_len,
                (void *)prefix, len, cb, &cb_info);
    } else {
        rv = pvt_trie_propagate_prefix(lpm_pvt, pvt_len,
                (void *)prefix, len, cb, &cb_info);
    }

    return rv;
}

static int
alpm_ppg_assoc_data_cb(trie_node_t *trie, trie_bpm_cb_info_t *info)
{
    int rv = BCM_E_NONE, unit;
    int update_pvt = 0;
    uint32 pfx_len, rpl_bpm_len, pvt_bpm_len;

    _alpm_cb_t *acb;
    _alpm_ppg_cb_user_data_t *user_data;
    _alpm_ppg_data_t *ppg_data;
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_node_t *pvt_def, *ppg_def;

    user_data = info->user_data;
    unit     = user_data->unit;
    ppg_data = user_data->ppg_data;
    acb      = user_data->ppg_acb;

    pvt_node    = (_alpm_pvt_node_t *)trie;
    pvt_bpm_len = PVT_BPM_LEN(pvt_node);
    pfx_len     = ppg_data->key_len;
    rpl_bpm_len = ppg_data->bpm_len;

    if (user_data->ppg_op == ALPM_PPG_INSERT) {
        rpl_bpm_len = pfx_len;
    }

    /* Addr & Mask has been screened by propagate function,
     * no need to do it again */
    if (user_data->ppg_op == ALPM_PPG_INSERT) {
        if (pvt_bpm_len > pfx_len) {
            /* Not qualified and stop from traversing its child nodes */
            return BCM_E_NONE;
        }
    }

    if (user_data->ppg_op == ALPM_PPG_DELETE) {
        if (pvt_bpm_len != pfx_len) {
            /* Not qualified and stop from traversing its child nodes */
            return BCM_E_NONE;
        }
    }

    /* Update bpm length accordingly */
    PVT_BPM_LEN(pvt_node) = rpl_bpm_len;

    /* Update PVT_BKT_DEF for pvt_node and do PPG again */
    pvt_def = PVT_BKT_DEF(pvt_node);
    ppg_def = (_alpm_bkt_node_t *)ppg_data->bkt_def;
    if (pvt_def == NULL || ppg_def == NULL || pvt_def == ppg_def) {
        update_pvt = 1;
    } else {
        if (sal_memcmp(&pvt_def->adata, &ppg_def->adata,
                       sizeof(_alpm_bkt_adata_t)) != 0) {
            update_pvt = 1;
        }
    }

    PVT_BKT_DEF(pvt_node) = ppg_def;
    if (update_pvt) {
        rv = alpm_pvt_update_by_pvt_node(unit, acb, pvt_node);
    }

    /* Update callback count */
    user_data->ppg_cnt++;

    return rv;
}

/* Software propagation */
int
alpm_ppg_assoc_data(int u, int ppg_op, _alpm_ppg_data_t *ppg_data)
{
    int i, rv = BCM_E_NONE;
    int insert = 0;
    uint32 vrf_id = 0;
    uint32 pkm = 0;

    trie_t *pvt_trie;
    _alpm_cb_t *acb;
    _alpm_ppg_cb_user_data_t user_data;

    switch (ppg_op) {
        case ALPM_PPG_INSERT:
            insert = 1;
            break;
        case ALPM_PPG_DELETE:
            insert = 0;
            break;
        case ALPM_PPG_HBP:
        default:
            return BCM_E_NONE;
    }

    vrf_id   = ppg_data->vrf_id;
    pkm      = ppg_data->pkm;

    sal_memset(&user_data, 0, sizeof(user_data));
    user_data.unit     = u;
    user_data.ppg_data = ppg_data;
    user_data.ppg_op   = ppg_op;

    for (i = 0; i < ACB_CNT(u); i++) {
        acb = ACB(u, i);
        user_data.ppg_acb  = acb;

        pvt_trie = ACB_PVT_TRIE(acb, vrf_id, pkm);
        if (pvt_trie) {
            rv = alpm_ppg(u, pvt_trie, ppg_data->key, ppg_data->key_len,
                          insert, alpm_ppg_assoc_data_cb, &user_data);
        }

        ALPM_VERB(("Propagation cb count %d\n", user_data.ppg_cnt));
    }

    return rv;

}

/* Delete ALPM pvt trie only for Direct Routes */
int
alpm_pvt_trie_delete(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int                 pkm = ALPM_LPM_PKM(u, lpm_cfg);
    trie_t              *pvt_trie = NULL;
    _alpm_pvt_node_t    *pvt_node = NULL;

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u), vrf_id, pkm);
    rv = trie_delete(pvt_trie, lpm_cfg->user_data,
                     lpm_cfg->defip_sub_len, (trie_node_t **)&pvt_node);
    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("Trie delete node failure\n"));
    }

    alpm_util_free(pvt_node);
    return rv;
}

/* Insert ALPM pvt trie only for Direct Routes which is needed in _tcam_match */
int
alpm_pvt_trie_insert(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int                 pkm = ALPM_LPM_PKM(u, lpm_cfg);
    trie_t              *pvt_trie = NULL;
    _alpm_pvt_node_t    *pvt_node = NULL;

    ALPM_ALLOC_EG(pvt_node, sizeof(_alpm_pvt_node_t), "pvt_node");
    PVT_BKT_VRF(pvt_node) = vrf_id;
    PVT_BKT_PKM(pvt_node) = pkm;
    PVT_IDX(pvt_node)     = lpm_cfg->defip_index;
    /* Direct route pvt_node doesn't use bkt_trie, def_pfx & bkt_info */
    PVT_KEY_LEN(pvt_node) = lpm_cfg->defip_sub_len;
    PVT_KEY_CPY(pvt_node, lpm_cfg->user_data);

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u), vrf_id, pkm);
    rv = trie_insert(pvt_trie, lpm_cfg->user_data, NULL,
                     lpm_cfg->defip_sub_len, (trie_node_t *)pvt_node);
    return rv;

bad:
    if (pvt_node != NULL) {
        alpm_util_free(pvt_node);
    }
    return rv;
}

/* ALPM pvt trie init only for Direct Routes */
int
alpm_pvt_trie_init(int u, int vrf_id, int pkm)
{
    int         rv = BCM_E_NONE;
    uint32      max_key_len;
    _alpm_cb_t  *acb = ACB_TOP(u);

    max_key_len = alpm_util_trie_max_key_len(u, pkm);
    ALPM_IER(trie_init(max_key_len, &ACB_PVT_TRIE(acb, vrf_id, pkm)));
    ACB_VRF_INIT_SET(u, acb, vrf_id, pkm);

    return rv;
}

/*
 * OUT: pvt_node
 */
static int
alpm_pvt_find(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg,
              _alpm_pvt_node_t **pvt_node)
{
    int             rv = BCM_E_NONE;
    trie_t          *pvt_trie;
    int             pkm = ALPM_LPM_PKM(u, lpm_cfg);
    int             vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, pkm);
    rv = trie_find_lpm(pvt_trie, lpm_cfg->user_data, lpm_cfg->defip_sub_len,
                       (trie_node_t **)pvt_node);
    if (BCM_FAILURE(rv)) {
        ALPM_INFO(("**ACB(%d).PVT.FIND failed %d\n", ACB_IDX(acb), rv));
    }

    return rv;
}

static int
alpm_pvt_insert(int u, _alpm_cb_t *acb,
                _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = 0;

    if (ACB_HAS_TCAM(acb)) {
        rv = bcm_esw_alpm_tcam_insert(u, lpm_cfg);
    } else {
        rv = alpm_cb_insert(u, ACB_UPR(u, acb), lpm_cfg);
    }

    ALPM_INFO(("**ACB(%d).PVT.ADD rv %d\n", ACB_IDX(acb), rv));

    return rv;
}

static int
alpm_pvt_delete(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = 0;

    if (ACB_HAS_TCAM(acb)) {
        rv = bcm_esw_alpm_tcam_delete(u, lpm_cfg);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("**ACB(%d).PVT.DEL tcam_delete 0x%x/%d failed %d\n",
                      ACB_IDX(acb), lpm_cfg->defip_ip_addr,
                      lpm_cfg->defip_sub_len, rv));
        }
    } else {
        rv = alpm_cb_delete(u, ACB_UPR(u, acb), lpm_cfg);
    }

    ALPM_INFO(("**ACB(%d).PVT.DEL rv %d\n", ACB_IDX(acb), rv));

    return rv;
}

static int
alpm_pvt_delete_by_pvt_node(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node)
{
    int rv = BCM_E_NONE;
    _bcm_defip_cfg_t lpm_cfg;

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    /* defip_flags */
    if (PVT_BKT_TRIE(pvt_node) &&
        PVT_BKT_TRIE(pvt_node)->v6_key) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    /* Key & Length */
    alpm_trie_pfx_to_cfg(u, pvt_node->key, PVT_KEY_LEN(pvt_node), &lpm_cfg);
    sal_memcpy(lpm_cfg.user_data, pvt_node->key, sizeof(pvt_node->key));
    lpm_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, PVT_BKT_VRF(pvt_node));

    /* Assoc-data */
    if (PVT_BKT_DEF(pvt_node) != NULL) {
        alpm_util_adata_trie_to_cfg(u, &PVT_BKT_DEF(pvt_node)->adata, &lpm_cfg);
    }

    if (ACB_HAS_TCAM(acb)) {
        lpm_cfg.pvt_node = (uint32 *)pvt_node;
    }

    rv = alpm_pvt_delete(u, acb, &lpm_cfg);

    return rv;
}

static int
alpm_pvt_update(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;

    _alpm_cb_t *upr_acb = NULL;
    _alpm_pvt_node_t *pvt_node = NULL;
    _alpm_bkt_node_t *bkt_node = NULL;

    if (ACB_HAS_TCAM(acb)) {
        /* Use insert routine to update */
        rv = bcm_esw_alpm_tcam_insert(u, lpm_cfg);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("**ACB(%d).PVT.UPD tcam_insert failed %d\n",
                      ACB_IDX(acb), rv));
        }
    } else {
        upr_acb = ACB_UPR(u, acb);
        ALPM_IEG(alpm_cb_find(u, upr_acb, lpm_cfg, &pvt_node, &bkt_node));
        rv = alpm_bkt_ent_write(u, upr_acb, pvt_node, lpm_cfg,
                                bkt_node->ent_idx);
    }

    ALPM_INFO(("**ACB(%d).PVT.UPD rv %d\n", ACB_IDX(acb), rv));

bad:
    return rv;
}

static int
alpm_pvt_update_by_pvt_node(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node)
{
    int rv = BCM_E_NONE;
    _bcm_defip_cfg_t lpm_cfg;

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_UPDATE_SKIP) {
        return rv;
    }

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));

    /* defip_flags */
    if (PVT_BKT_TRIE(pvt_node) &&
        PVT_BKT_TRIE(pvt_node)->v6_key) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    /* Key & Length */
    alpm_trie_pfx_to_cfg(u, pvt_node->key, PVT_KEY_LEN(pvt_node), &lpm_cfg);
    sal_memcpy(lpm_cfg.user_data, pvt_node->key, sizeof(pvt_node->key));
    lpm_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, PVT_BKT_VRF(pvt_node));

    /* Assoc-data */
    if (PVT_BKT_DEF(pvt_node) != NULL) {
        alpm_util_adata_trie_to_cfg(u, &PVT_BKT_DEF(pvt_node)->adata, &lpm_cfg);
    }

    if (ACB_HAS_TCAM(acb)) {
        lpm_cfg.pvt_node = (uint32 *)pvt_node;
    } else {
        if (ACB_BKT_FIXED_FMT(acb, 1)) {
            lpm_cfg.bkt_kshift = 0;
        } else {
            lpm_cfg.bkt_kshift = PVT_KEY_LEN(pvt_node); /* for upr acb bkt */
        }
    }
    lpm_cfg.bkt_info = &PVT_BKT_INFO(pvt_node);

    rv = alpm_pvt_update(u, acb, &lpm_cfg);

    return rv;
}

static int
alpm_bkt_find(int u, _alpm_cb_t *acb,
              _bcm_defip_cfg_t *lpm_cfg,
              _alpm_pvt_node_t *pvt_node,
              _alpm_bkt_node_t **bkt_node)
{
    int rv = BCM_E_NOT_FOUND;
    trie_t *bkt_trie = PVT_BKT_TRIE(pvt_node);
    if (bkt_trie) {
        rv = trie_search(bkt_trie, lpm_cfg->user_data, lpm_cfg->defip_sub_len,
                         (trie_node_t **)bkt_node);
    }
    return rv;
}

static int
alpm_bkt_ent_del(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                 int ent_idx)
{
    int rv = BCM_E_NONE;
    /* TD3ALPMTBD */
    return rv;
}

int
alpm_bkt_ent_get(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                 _alpm_bkt_info_t *bkt_info,
                 int pfx_len, uint32 *ent_idx, int *fmt_update)
{
    /* Start with shorter prefix fmt */
    int i, rv = BCM_E_NONE;
    uint8 min_fmt = 0xff;
    int start, close, step;
    int bnkpb = BPC_BNK_PER_BKT(bp_conf);
    int rofs  = BI_ROFS(bkt_info);

    /* Different bucket use different method for allocation, then after split,
     * bank distribution will be like this:
     bank 1:
        ----> Bank 0: FMT1[*_______________]
        ----> Bank 1: FMT3[**********______]
        ----> Bank 2: FMT3[**********______]
        ----> Bank 3: FMT3[**********______]
        ----> Bank 4: FMT3[**********______]
        ----> Bank 5: FMT3[________________]
        ----> Bank 6: FMT3[________________]
        ----> Bank 7: FMT3[________________]
     bank 2:
        ----> Bank 0: FMT1[________________]
        ----> Bank 1: FMT1[________________]
        ----> Bank 2: FMT1[________________]
        ----> Bank 3: FMT1[________________]
        ----> Bank 4: FMT3[**______________]
        ----> Bank 5: FMT3[**********______]
        ----> Bank 6: FMT3[**********______]
        ----> Bank 7: FMT3[**********______]
     */

    if (!BPC_BKT_WRA(bp_conf) || BI_BKT_IDX(bkt_info) & 1) {
        start = rofs;
        close = rofs + bnkpb;
        step  = 1;
    } else {
        start = rofs + bnkpb - 1;
        close = rofs - 1;
        step  = -1;
    }

    /* First round is to get a bank that:
     *  with valid fmt type (assigned) AND
     *  proper prefix length AND
     *  bank is not full
     *
     *  go through each bank to find the most efficient fmt
     *
     */
    for (i = start; ABS(i-close) > 0; i += step) {
        uint8 bnk_fmt = bkt_info->bnk_fmt[i % bnkpb];
        uint16 vet_bmp = bkt_info->vet_bmp[i % bnkpb];

        /* Ignore invalid fmt type */
        if (!BI_BNK_IS_USED(bkt_info, i % bnkpb) ||
            bnk_fmt > ACB_FMT_MAX(acb)) {
            continue;
        }

        /* Ignore full bank */
        if (vet_bmp == (1 << ACB_FMT_ENT_MAX(acb, bnk_fmt)) - 1) {
            continue;
        }

        /* Ignore unfit fmt 1bit Length should be considered */
        if (!BPC_BKT_FIXED_FMT(bp_conf) &&
            pfx_len > ACB_FMT_PFX_LEN(acb, bnk_fmt)) {
            continue;
        }

        if (bnk_fmt < min_fmt) {
            min_fmt = bnk_fmt;
            *ent_idx = i % bnkpb;
        }
    }

    /* ent_idx format
     * -------------------------------
     * | ent idx | bnk idx | bkt idx |
     * -------------------------------
     */
    /* Now we find the bank, also need to find the empty slot in this bank */
    if (min_fmt != 0xff) {
        for (i = 0; i < ACB_FMT_ENT_MAX(acb, min_fmt); i++) {
            if ((bkt_info->vet_bmp[*ent_idx] & (1 << i)) == 0) {
                bkt_info->bnk_fmt[*ent_idx] = min_fmt;
                bkt_info->vet_bmp[*ent_idx] |= 1 << i;
                *ent_idx = ALPM_IDX_MAKE(acb, bkt_info, *ent_idx, i);
                goto end;
            }
        }
    }

    if (BPC_BKT_FIXED_FMT(bp_conf)) {
        rv = BCM_E_FULL;
        goto end;
    }

    /* do second round if missed in the first round:
     *  bank fmt is not fit (e.g. prefix length is shorter) BUT
     *  this bank is empty, SO
     *  we change the bank fmt and make it available.
     */
    for (i = start; ABS(i-close) > 0; i += step) {
        uint8 fmt, bnk_fmt = bkt_info->bnk_fmt[i % bnkpb];
        uint16 vet_bmp = bkt_info->vet_bmp[i % bnkpb];

        /* Ignore invalid fmt type */
        if (!BI_BNK_IS_USED(bkt_info, i % bnkpb) ||
            bnk_fmt > ACB_FMT_MAX(acb) ||
            vet_bmp != 0) {
            continue;
        }

        /* vet_bmp is empty && bnk_fmt is valid
         * from most efficient fmt to least efficient fmt */
        for (fmt = 1; fmt <= ACB_FMT_MAX(acb); fmt++) {
            if (pfx_len <= ACB_FMT_PFX_LEN(acb, fmt)) {
                /* Entry 0 of bank i */
                bkt_info->vet_bmp[i % bnkpb] |= 1;
                bkt_info->bnk_fmt[i % bnkpb] = fmt;
                *ent_idx = ALPM_IDX_MAKE(acb, bkt_info, i % bnkpb, 0);

                if (bnk_fmt != fmt && fmt_update != NULL) {
                    *fmt_update = TRUE;
                }
                goto end;
            }
        }
    }

    rv = BCM_E_FULL;
end:
    if (rv != BCM_E_FULL) {
        if (ALPM_LOG_CHECK(BSL_VERBOSE)) {
            char bnk_str[ALPM_BPB_MAX*2+2];
            sal_strcpy(bnk_str, "_ _ _ _ _ _ _ _");
            bnk_str[2 * ALPM_IDX_TO_BNK(acb, *ent_idx)] =
                bkt_info->bnk_fmt[ALPM_IDX_TO_BNK(acb, *ent_idx)] + 48;

            ALPM_VERB(("**ACB(%d).ENT.get: RBBE (%d %d [%s] %d)%s\n",
                       ACB_IDX(acb), BI_ROFS(bkt_info), BI_BKT_IDX(bkt_info),
                       bnk_str, ALPM_IDX_TO_ENT(*ent_idx),
                       (fmt_update && *fmt_update ? "*" : "")));
        }
    } else {
        ALPM_VERB(("**ACB(%d).ENT.get: failed (FULL)\n", ACB_IDX(acb)));
    }
    return rv;
}

int
alpm_bkt_ent_write(int u, _alpm_cb_t *acb,
                   _alpm_pvt_node_t *pvt_node,
                   _bcm_defip_cfg_t *lpm_cfg, int ent_idx)
{
    int rv;
    rv = ALPM_DRV(u)->alpm_bkt_ent_write(u, acb, pvt_node, lpm_cfg, ent_idx);
    return rv;
}

static int
alpm_bkt_bnks_get(int u, _alpm_cb_t *acb, _alpm_bkt_info_t *bkt_info,
                  int *count)
{
    int rv = BCM_E_NONE;
    int i, bnk_cnt = 0;

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        uint8 bnk_fmt = bkt_info->bnk_fmt[i];
        if (BI_BNK_IS_USED(bkt_info, i) &&
            bnk_fmt <= ACB_FMT_MAX(acb)) {
            bnk_cnt ++;
        }
    }

    if (count) {
        *count = bnk_cnt;
    }

    return rv;
}

/* step == 0 means check if *start_bnk is occupied */
static int
alpm_bkt_bnk_alloc(int u, _alpm_bkt_pool_conf_t *bp_conf, int *start_bnk,
                   int alloc_sz, int step)
{
    int rv = BCM_E_NONE;
    int i, bnk_cnt, used = 0;
    SHR_BITDCL *bnk_bmp;

    bnk_bmp = BPC_BNK_BMP(bp_conf);
    bnk_cnt = BPC_BNK_CNT(bp_conf);

    for (i = (*start_bnk); i < bnk_cnt; i += step) {
        SHR_BITTEST_RANGE(bnk_bmp, i, alloc_sz, used);
        if (!used) {
            break;
        }
    }

    if (i == bnk_cnt) {
        rv = BCM_E_NOT_FOUND;
    } else {
        SHR_BITSET_RANGE(bnk_bmp, i, alloc_sz);
        BPC_BNK_USED(bp_conf) += alloc_sz;
        *start_bnk = i;
    }

    if (rv == BCM_E_NONE) {
        ALPM_INFO(("bnk alloc start %d sz %d\n", *start_bnk, alloc_sz));
    }

    return rv;
}

static void
alpm_bkt_sub_bkt_info(int u, _alpm_cb_t *acb, int vrf_id,
                     _alpm_bkt_pool_conf_t *bp_conf,
                     int start_bnk, int range, int *pkm, uint32 *sub_bkt_bmp)
{
    int i, bnk_pbk;
    SHR_BITDCL *bnk_bmp;
    _alpm_pvt_node_t *pvt_node;

    bnk_bmp = BPC_BNK_BMP(bp_conf);
    bnk_pbk = BPC_BNK_PER_BKT(bp_conf);
    for (i = start_bnk; i < start_bnk + range; i++) {
        if (SHR_BITGET(bnk_bmp, i)) {
            int tab_idx =
                ALPM_TAB_IDX_GET_BKT_BNK(acb, 0, i / bnk_pbk, i % bnk_pbk);
            pvt_node =
                (_alpm_pvt_node_t *)ACB_VRF_PVT_PTR(acb, vrf_id, tab_idx);
            if (pvt_node != NULL) {
                *pkm = PVT_BKT_PKM(pvt_node);
                *sub_bkt_bmp |= 1 << BI_SUB_BKT_IDX(&pvt_node->bkt_info);
            }
        }
    }

    return ;
}

static int
alpm_bkt_pkm_sbkt_chk(int u, _alpm_cb_t *acb, int vrf_id,
                      _alpm_bkt_pool_conf_t *bp_conf,
                      int start_bnk, int range,
                      uint16 *sub_bkt_idx, int *sub_bkt_pkm)
{
    uint32 sub_bkt_bmp = 0;

    uint16 first_sub_bkt[] = {
        0, /* 0b0000 */ 1, /* 0b0001 */ 0, /* 0b0010 */ 2, /* 0b0011 */
        0, /* 0b0100 */ 1, /* 0b0101 */ 0, /* 0b0110 */ 3, /* 0b0111 */
        0, /* 0b1000 */ 1, /* 0b1001 */ 0, /* 0b1010 */ 2, /* 0b1011 */
        0, /* 0b1100 */ 1, /* 0b1101 */ 0, /* 0b1110 */
    };

    *sub_bkt_pkm = -1;
    alpm_bkt_sub_bkt_info(u, acb, vrf_id, bp_conf, start_bnk, range,
                          sub_bkt_pkm, &sub_bkt_bmp);
    /* All 4 sub-pkts are used */
    if (sub_bkt_bmp >= 0xf) {
        return BCM_E_NOT_FOUND;
    }

    *sub_bkt_idx = first_sub_bkt[sub_bkt_bmp];
    return BCM_E_NONE;
}

/* step == 0xfffffff means check if *start_bnk is occupied */
static int
alpm_bkt_sharing_bnk_alloc(int u, _alpm_cb_t *acb, int pkm, int vrf_id,
                           _alpm_bkt_pool_conf_t *bp_conf,
                           _alpm_bkt_info_t *bkt_info, int *avl_sz,
                           int start_bnk, int alloc_sz, int step)
{
    int rv = BCM_E_NONE;
    int i, j, bnks, bnk_cnt, bnk_pbk, used;
    uint16 sub_bkt_idx = 0;
    int count, range = 0;
    int check_avl, max_avl_sz = 0;
    SHR_BITDCL *bnk_bmp;

    sal_memset(bkt_info, 0, sizeof(_alpm_bkt_info_t));
    bnk_bmp = BPC_BNK_BMP(bp_conf);
    bnk_cnt = BPC_BNK_CNT(bp_conf);
    bnk_pbk = BPC_BNK_PER_BKT(bp_conf);

    /* if step == 0xfffffff means check_exist */
    check_avl = (alloc_sz == 1) && (step > (bnk_cnt + bnk_pbk));
    assert(check_avl || (start_bnk == 0 && step == bnk_pbk));

    /* Case 1: Check if a bank is available */
    if (check_avl) {
        i = start_bnk;
        range = alloc_sz;
        if (!SHR_BITGET(bnk_bmp, i)) {
            goto _done;
        }

        return BCM_E_NOT_FOUND;
    }

    /* Case 2: Allocate bank from empty buckets */
    range = bnk_pbk;
    for (i = 0; i < bnk_cnt; i += step) {
        SHR_BITTEST_RANGE(bnk_bmp, i, range, used);
        if (!used) {
            break;
        }
    }
    if (i < bnk_cnt) {
        goto _done;
    }

    /* Case 3: Allocate bank from non-empty buckets
     * WARNING: V4/V6 should not be sharing same bucket */
    i = 0;
    while (i < bnk_cnt) {
        SHR_BITCOUNT_RANGE(bnk_bmp, count, i, range);
        if (range == count) {
            i += step;
            continue;
        }

        if ((range - count) > max_avl_sz) {
            int bkt_pkm;
            rv = alpm_bkt_pkm_sbkt_chk(u, acb, vrf_id, bp_conf, i, range,
                                       &sub_bkt_idx, &bkt_pkm);
            if (rv == BCM_E_NOT_FOUND || bkt_pkm != pkm) {
                i += step;
                continue;
            }

            max_avl_sz = range - count;
            if (max_avl_sz >= alloc_sz) {
                /* Check available sub_bkt_idx,
                 * total sub_bkt_idx cnt is 4,
                 * check_exist case doesn't need sub_bkt_idx
                 */
                break;
            }
        }

        i += step;
    }

    if (i >= bnk_cnt) {
        if (avl_sz != NULL) {
            *avl_sz = max_avl_sz;
        }
        return BCM_E_NOT_FOUND;
    }

_done:
    BI_ROFS(bkt_info) = 0;
    BI_BKT_IDX(bkt_info) = i / bnk_pbk;
    BI_SUB_BKT_IDX(bkt_info) = sub_bkt_idx;
    bnks = 0;
    for (j = i; j < i+range; j++) {
        if (!SHR_BITGET(bnk_bmp, j)) {
            bkt_info->bnk_fmt[j % bnk_pbk] =
                alpm_bkt_bnk_def_fmt_get(u, acb, pkm, vrf_id);
            /* bkt_info->vet_bmp default to 0 */
            SHR_BITSET(bnk_bmp, j);
            if (++bnks >= alloc_sz) {
                break;
            }
        }
    }

    BPC_BNK_USED(bp_conf) += alloc_sz;

    return rv;
}

int
alpm_bkt_bnk_free(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                  _alpm_bkt_info_t *bkt_info, int bnk)
{
    int gbl_bnk, bkt_bnk, tab_idx;
    SHR_BITDCL *bnk_bmp;

    bnk_bmp = BPC_BNK_BMP(bp_conf);
    bkt_bnk = BPC_BNK_PER_BKT(bp_conf);
    gbl_bnk = BI_BKT_IDX(bkt_info) * bkt_bnk + bnk;
    if (BI_ROFS(bkt_info) > bnk) {
        gbl_bnk += bkt_bnk;
    }

    if (!SHR_BITGET(bnk_bmp, gbl_bnk)) {
        return BCM_E_PARAM;
    }
    SHR_BITCLR(bnk_bmp, gbl_bnk);
    BPC_BNK_USED(bp_conf) -= 1;

    tab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, bnk);
    if (bp_conf->pvt_ptr != NULL) {
        bp_conf->pvt_ptr[tab_idx] = NULL;
    }

    /* Suppose to update bkt_info->rofs when bnk is freed */
    if (BPC_BKT_WRA(bp_conf) && bnk == BI_ROFS(bkt_info)) {
        int new_rofs = (bnk + 1) % bkt_bnk;
        if (new_rofs < BI_ROFS(bkt_info)) {
            BI_BKT_IDX(bkt_info)++;
        }
        BI_ROFS(bkt_info) = new_rofs;
    }

    BI_BNK_FREE(bkt_info, bnk);

    ALPM_INFO(("bnk  free bnk %d \n", gbl_bnk));

    acb->acb_cnt.c_bnkfree++;

    return BCM_E_NONE;
}

static int
alpm_bkt_bnk_shrink(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                    _alpm_bkt_info_t *bkt_info, int *fmt_update)
{
    int i, bnkpb, bnk;
    int bnks, bnke;

    bnkpb = BPC_BNK_PER_BKT(bp_conf);

    /* Store bnks and bnke because alpm_bkt_bnk_free could change rofs value */
    bnks  = BI_ROFS(bkt_info);
    bnke  = bnkpb + BI_ROFS(bkt_info);

    for (i = bnks; i < bnke; i++) {
        bnk = i % bnkpb;
        if (BI_BNK_IS_USED(bkt_info, bnk) && bkt_info->vet_bmp[bnk] == 0) {
            /* Release bank bnk */
            ALPM_INFO(("**ACB(%d).BNK.RLS: RBBE (%d %d %d *)\n",
                       ACB_IDX(acb), -1, BI_BKT_IDX(bkt_info), bnk));

            (void)alpm_bkt_bnk_free(u, acb, bp_conf, bkt_info, bnk);
            if (fmt_update != NULL) {
                *fmt_update = 1;
            }
        }
    }

    acb->acb_cnt.c_bnkshrk++;

    return BCM_E_NONE;
}

uint8
alpm_bkt_bnk_def_fmt_get(int u, _alpm_cb_t *acb, int pkm, int vrf_id)
{
    uint8 fmt;
    static uint8 def_fmt[] = {
        ALPM_FMT_PKM_32B,
        ALPM_FMT_PKM_FLEX_32B,
        ALPM_FMT_PKM_64B,
        ALPM_FMT_PKM_FLEX_64B,
        ALPM_FMT_PKM_128,
        ALPM_FMT_PKM_FLEX_128
    };
    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        fmt = def_fmt[pkm << 1 | ACB_VRF_DB_TYPE(u, acb, vrf_id, pkm)];
    } else {
        fmt = ALPM_FMT_DEF;
    }

    return fmt;
}

/* _alpm_bkt_alloc_full to make sure a entire bucket
 * is allocated or return FULL */
static int
alpm_bkt_alloc_attempt(int u, _alpm_cb_t *acb, int pkm, int vrf_id,
                       int req_bnk_cnt, int step, _alpm_bkt_info_t *bkt_info,
                       int *avl_bnk_cnt)
{
    int rv;
    int i, bnk = 0, bnkpb;

    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        rv = alpm_bkt_sharing_bnk_alloc(u, acb, pkm, vrf_id,
                    ACB_BKT_VRF_POOL(acb, vrf_id), bkt_info, avl_bnk_cnt,
                    0, req_bnk_cnt, step);
    } else {
        rv = alpm_bkt_bnk_alloc(u, ACB_BKT_VRF_POOL(acb, vrf_id), &bnk,
                                req_bnk_cnt, step);
        if (BCM_SUCCESS(rv)) {
            bnkpb = ACB_BNK_PER_BKT(acb, vrf_id);
            BI_ROFS(bkt_info) = bnk % bnkpb;
            BI_BKT_IDX(bkt_info) = bnk / bnkpb;
            sal_memset(bkt_info->vet_bmp, 0, sizeof(bkt_info->vet_bmp));
            sal_memset(bkt_info->bnk_fmt, 0, sizeof(bkt_info->bnk_fmt));
            for (i = 0; i < req_bnk_cnt; i++) {
                bkt_info->bnk_fmt[(BI_ROFS(bkt_info) + i) % bnkpb]
                    = alpm_bkt_bnk_def_fmt_get(u, acb, pkm, vrf_id);
            }
        }
    }

    if (rv == BCM_E_NOT_FOUND) {
        ALPM_VERB(("**ACB(%d).BNK resource unavailable (Full)\n",
                   ACB_IDX(acb)));
        rv = BCM_E_FULL;
    } else if (rv == BCM_E_NONE) {
        ALPM_INFO(("**ACB(%d).BNK get: bkt %4d r %d bnk[%d %d %d %d %d %d %d %d]\n",
                   ACB_IDX(acb), BI_BKT_IDX(bkt_info), BI_ROFS(bkt_info),
                   bkt_info->bnk_fmt[0], bkt_info->bnk_fmt[1],
                   bkt_info->bnk_fmt[2], bkt_info->bnk_fmt[3],
                   bkt_info->bnk_fmt[4], bkt_info->bnk_fmt[5],
                   bkt_info->bnk_fmt[6], bkt_info->bnk_fmt[7]));
    }

    return rv;
}

int
alpm_bkt_bnk_nearest_get(int u, SHR_BITDCL *bnk_bmp, int bnk_sz,
                         int left_bnk, int right_bnk, int *nst_bnk)
{
    int i, rv = BCM_E_NONE;
    int nst_left, nst_right;

    nst_left  = -1;
    nst_right = bnk_sz;

    if (left_bnk >= 0) {
        for (i = left_bnk; i >= 0; i--) {
            if (!SHR_BITGET(bnk_bmp, i)) {  /* TBD performance */
                break;
            }
        }
        nst_left = i;
    }

    if (right_bnk < bnk_sz) {
        for (i = right_bnk; i < bnk_sz; i++) {
            if (!SHR_BITGET(bnk_bmp, i)) {  /* TBD performance */
                break;
            }
        }
        nst_right = i;
    }

    if (nst_left >= 0 && nst_right < bnk_sz) {
        if ((left_bnk - nst_left) > (nst_right - right_bnk)) {
            *nst_bnk = nst_right;
        } else {
            *nst_bnk = nst_left;
        }
    } else if (nst_left >= 0) {
        *nst_bnk = nst_left;
    } else if (nst_right < bnk_sz) {
        *nst_bnk = nst_right;
    } else {
        rv = BCM_E_FULL;
    }

    return rv;
}

static int
alpm_bkt_pfx_ent_idx_update(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                            _alpm_bkt_info_t *src_bkt, int src_bnk,
                            _alpm_bkt_info_t *dst_bkt, int dst_bnk)
{
    int i, rv = BCM_E_UNAVAIL;
    int sb = src_bnk;
    int db = dst_bnk;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_bkt_node_t *bkt_node = NULL;
    trie_t      *bkt_trie = NULL;

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = trie_traverse(bkt_trie, alpm_trie_pfx_cb, pfx_arr,
                       _TRIE_INORDER_TRAVERSE);
    ALPM_IEG(rv);
    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        int ent, bnk, bkt;

        bkt_node = pfx_arr->pfx[i];
        ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
        bnk = ALPM_IDX_TO_BNK(acb, bkt_node->ent_idx);
        bkt = ALPM_IDX_TO_BKT(acb, bkt_node->ent_idx);
        if (bnk < PVT_ROFS(pvt_node)) {
            bkt -= 1;
        }

        if (src_bnk == -1) {
            sb = bnk;
            db = bnk;
        }

        if (bnk == sb && bkt == BI_BKT_IDX(src_bkt)) {
            bkt_node->ent_idx = ALPM_IDX_MAKE(acb, dst_bkt, db, ent);
        }
    }

bad:
    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }
    return rv;

}

static int
alpm_bkt_bnk_copy(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                  _alpm_bkt_info_t *src_bkt, int src_bnk,
                  _alpm_bkt_info_t *dst_bkt, int dst_bnk)
{
    int rv = BCM_E_UNAVAIL;
    int src_idx, dst_idx;

    src_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, src_bkt, src_bnk);
    dst_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, dst_bkt, dst_bnk);
    rv = ALPM_DRV(u)->alpm_bkt_bnk_copy(u, acb, pvt_node, src_bkt, dst_bkt,
                                        src_idx, dst_idx);
    if (BCM_SUCCESS(rv)) {
        /* Performance improvement ALPMTBD in pfx_ent_idx_update */
        rv = alpm_bkt_pfx_ent_idx_update(u, acb, pvt_node, src_bkt, src_bnk,
                                         dst_bkt, dst_bnk);
    }

    return rv;
}

static int
alpm_bkt_bnk_clear(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
                   int tab_idx)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_bkt_bnk_clear(u, acb, pvt_node, tab_idx);
    return rv;
}

/* Receive a new bank 'to_bnk' on one side and release old bank on
 * another side
 *
 * When we reach here, to_bnk should be neighbour of pvt_node's bkt_info.
 * After moving is done, free bank to free_bnk.
 */
static int
alpm_bkt_bnk_ripple(int u, _alpm_cb_t *acb,
                   _alpm_pvt_node_t *pvt_node, int to_bnk, int *free_bnk)
{
    int i, rv = BCM_E_NONE;
    int bnkpb, start_bnk, end_bnk, mv_bnk;
    int mb, tb, tab_idx = 0;
    uint8  bnk_fmt;
    uint16 vet_bmp;
    _alpm_bkt_info_t *bkt_info, dst_bkt;

    bkt_info = &PVT_BKT_INFO(pvt_node);
    bnkpb = ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node));
    start_bnk = PVT_ROFS(pvt_node);
    /* Find end bank */
    for (i = 1; i < bnkpb; i++) {
        if (!BI_BNK_IS_USED(bkt_info, start_bnk % bnkpb)) {
            start_bnk++;
            i--;
            continue;
        }
        if (!BI_BNK_IS_USED(bkt_info, (start_bnk+i) % bnkpb)) {
            break;
        }
    }
    start_bnk = PVT_BKT_IDX(pvt_node) * bnkpb + start_bnk;
    end_bnk = start_bnk + i - 1;

    /* 1. copy bank on the other side to to_bnk */
    /* S: start, E: end, T: to_bnk
     -----------------     -----------------
     | |#|#|#|#|#| | |  -> |#|#|#|#|#| | | |
     -----------------     -----------------
      ^ ^       ^                     ^
      T S       E                     T
     -----------------     -----------------
     | |#|#|#|#|#| | |  -> | | |#|#|#|#|#| |
     -----------------     -----------------
        ^       ^ ^           ^
        S       E T           T
     */
    if (to_bnk < start_bnk) {
        mv_bnk = end_bnk;
        /* Adjust start_bnk */
        start_bnk = to_bnk;
    } else {
        mv_bnk = start_bnk;

        /* Adjust start_bnk */
        start_bnk++;
        while (start_bnk < end_bnk) {
            if (!BI_BNK_IS_USED(bkt_info, start_bnk % bnkpb)) {
                start_bnk++;
            } else {
                break;
            }
        }
    }
    mb = mv_bnk % bnkpb;
    tb = to_bnk % bnkpb;

    /* Save src_idx before rofs, bkt-idx changes */
    tab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bkt_info, mb);

    sal_memset(&dst_bkt, 0, sizeof(dst_bkt));
    BI_ROFS(&dst_bkt)    = start_bnk % bnkpb;
    BI_BKT_IDX(&dst_bkt) = start_bnk / bnkpb;

    ALPM_IER(alpm_bkt_bnk_copy(u, acb, pvt_node, bkt_info, mb,
                                &dst_bkt, tb));
    /* 2. Update bkt_info info */
    bnk_fmt = bkt_info->bnk_fmt[mb];
    vet_bmp = bkt_info->vet_bmp[mb];
    (void)alpm_bkt_bnk_alloc(u, ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                             &to_bnk, 1, 0xfffffff);
    /* bkt_info could be changed here includes:
     * rofs, bkt-idx, bnk_fmt, so we need to save those values.
     */
    (void)alpm_bkt_bnk_free(u, acb, ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                             bkt_info, mb);

    PVT_ROFS(pvt_node)    = BI_ROFS(&dst_bkt);
    PVT_BKT_IDX(pvt_node) = BI_BKT_IDX(&dst_bkt);
    bkt_info->bnk_fmt[tb] = bnk_fmt;
    bkt_info->vet_bmp[tb] = vet_bmp;

    /* 3. Update PVT */
    if (pvt_node != NULL) {
        rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node);
        ALPM_IER(rv);
    }

    /* 4. invalidate old bnk */
    ALPM_IER(alpm_bkt_bnk_clear(u, acb, pvt_node, tab_idx));
    if (tb != mb) {
        bkt_info->bnk_fmt[mb] = 0;
        bkt_info->vet_bmp[mb] = 0;
    }

    if (free_bnk != NULL) {
        *free_bnk = mv_bnk;
    }

    acb->acb_cnt.c_ripple++;

    return rv;
}

static int
alpm_bkt_move(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node,
              _alpm_bkt_info_t *src_bkt, _alpm_bkt_info_t *dst_bkt)
{
    int rv = BCM_E_NONE;
    int bnkpb, sbnk, dbnk, tab_idx;
    int lsbnk, ldbnk; /* Local src/dst bnk */
    _alpm_bkt_info_t tmp_bkt;

    bnkpb = ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node));
    dbnk  = BI_ROFS(dst_bkt);
    ldbnk = dbnk % bnkpb;
    while (!BI_BNK_IS_USED(dst_bkt, ldbnk)) {
        dbnk ++;
        ldbnk = dbnk % bnkpb;
    }

    /* Copy */
    for (sbnk = src_bkt->rofs; sbnk < src_bkt->rofs + bnkpb; sbnk++) {
        lsbnk = sbnk % bnkpb;
        ldbnk = dbnk % bnkpb;
        if (!BI_BNK_IS_USED(src_bkt, lsbnk)) {
            continue;
        }

        dst_bkt->bnk_fmt[ldbnk] = src_bkt->bnk_fmt[lsbnk];
        dst_bkt->vet_bmp[ldbnk] = src_bkt->vet_bmp[lsbnk];

        ALPM_IER(alpm_bkt_bnk_copy(u, acb, pvt_node, src_bkt, lsbnk,
                                   dst_bkt, ldbnk));

        dbnk++;
        ldbnk = dbnk % bnkpb;
        while (!BI_BNK_IS_USED(dst_bkt, ldbnk)) {
            dbnk ++;
            ldbnk = dbnk % bnkpb;
        }
    }

    /* swap src_bkt <-> dst_bkt info */
    sal_memcpy(&tmp_bkt, src_bkt, sizeof(_alpm_bkt_info_t));
    sal_memcpy(src_bkt, dst_bkt, sizeof(_alpm_bkt_info_t));
    sal_memcpy(dst_bkt, &tmp_bkt, sizeof(_alpm_bkt_info_t));

    if (pvt_node != NULL) {
        rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node);
        ALPM_IER(rv);
    }

    /* Invalidate src bucket */
    for (sbnk = 0; sbnk < bnkpb; sbnk++) {
        if (!BI_BNK_IS_USED(dst_bkt, sbnk)) {
            continue;
        }

        tab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, dst_bkt, sbnk);
        ALPM_IER(alpm_bkt_bnk_clear(u, acb, pvt_node, tab_idx));
        dst_bkt->vet_bmp[sbnk] = 0;
    }

    /* Release bnk to global pool */
    (void)alpm_bkt_bnk_shrink(u, acb, ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                               dst_bkt, NULL);

    return rv;
}

/*
 *
  0 1 2 3 4 5 6 7
 -----------------
 | |#|#|#|#|#|*|*|
 -----------------
  ^   --->  ^
 from       to

 -----------------
 | |*|*|*|#|#|#| |
 -----------------
          ^ <-- ^
          to    from

 * PLEASE NOTICE THAT from_bnk AND to_bnk INDICATE FREE BANK LOCATION,
 * NOT USED BANK LOCATION.
 */
static int
alpm_bkt_free_bnk_move(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                       int from_bnk, int to_bnk)
{
    int mv_bnk, step;
    _alpm_pvt_node_t *pvt_node = NULL;
    int tab_idx, free_bnk = -1, bnkpb;
    int rv = BCM_E_NONE;

    bnkpb = BPC_BNK_PER_BKT(bp_conf);
    step = from_bnk < to_bnk ? 1 : -1;
    mv_bnk = from_bnk + step;
    while (ABS(mv_bnk - to_bnk) >= 0) {
        tab_idx = ALPM_TAB_IDX_GET_BKT_BNK(acb, 0,
                    mv_bnk/bnkpb, mv_bnk%bnkpb);
        if (bp_conf->pvt_ptr &&
            bp_conf->pvt_ptr[tab_idx] == NULL) {
            from_bnk = mv_bnk;
            mv_bnk += step;
            continue;
        }

        if (bp_conf->pvt_ptr != NULL) {
            pvt_node = (_alpm_pvt_node_t *)bp_conf->pvt_ptr[tab_idx];
            rv = alpm_bkt_bnk_ripple(u, acb, pvt_node, from_bnk, &free_bnk);
            ALPM_IEG(rv);
            if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
                rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "FreeBnkMove");
                ALPM_IEG(rv);
            }
        }

        if (free_bnk == to_bnk) {
            break;
        } else {
            from_bnk = free_bnk;
            mv_bnk = from_bnk + step;
        }
    }

bad:
    return rv;
}

/*
 * To find a minimum range which contains req_cnt free banks
 *
 * min_left and min_right are return value indicate the range.
 *
 */
int
alpm_bkt_free_bnk_range_get(int u, SHR_BITDCL *bnk_bmp, int bnk_sz,
                            int req_cnt, int *rng_left, int *rng_right,
                            int *rng_mid)
{
    int i, rv = BCM_E_NONE;
    int alloc_sz, idx, free_cnt = 0;
    int *free_bnk_arr = NULL;

    int min_idx = -1, min_range;

    alloc_sz = bnk_sz * sizeof(int);
    ALPM_ALLOC_EG(free_bnk_arr, alloc_sz, "free_bnk_arr");
    /*
     -------------------------------------------------------------
     | |#|#| | | |#|#|#|#| | |#|#| | |#|#| |#| |#| |#|#|#|#| | |#|
     -------------------------------------------------------------
      ^                                                         ^
      0                                                        bnk_sz-1
     */
    min_range = bnk_sz;
    for (i = 0; i < bnk_sz; i++) {
        if (!SHR_BITGET(bnk_bmp, i)) {
            free_bnk_arr[free_cnt] = i;
            idx = free_cnt - (req_cnt - 1);
            if (idx >= 0) {
                if (min_range > (free_bnk_arr[free_cnt] - free_bnk_arr[idx])) {
                    min_range = (free_bnk_arr[free_cnt] - free_bnk_arr[idx]);
                    min_idx = free_cnt;
                }
            }
            free_cnt ++;
        }
    }

    if (min_idx != -1) {
        *rng_left = free_bnk_arr[min_idx - (req_cnt - 1)];
        *rng_right = free_bnk_arr[min_idx];
        *rng_mid = free_bnk_arr[min_idx - (req_cnt - 1) / 2];
    } else {
        rv = BCM_E_FULL;
    }

bad:
    if (free_bnk_arr != NULL) {
        alpm_util_free(free_bnk_arr);
    }
    return rv;
}

static int
alpm_bkt_free(int u, _alpm_cb_t *acb, int vrf_id,
              _alpm_bkt_info_t *bkt_info)
{
    int rv = BCM_E_NONE;
    int bnk;
    _alpm_bkt_pool_conf_t *bp_conf;

    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
    for (bnk = 0; bnk < ALPM_BPB_MAX; bnk++) {
        if (BI_BNK_IS_USED(bkt_info, bnk)) {
            (void)alpm_bkt_bnk_free(u, acb, bp_conf, bkt_info, bnk);
        }
    }

    return rv;
}

/* OUT: bkt_info */
static int
alpm_bkt_alloc(int u, _alpm_cb_t *acb, int pkm, int vrf_id, int req_bnk_cnt,
               _alpm_bkt_info_t *bkt_info, int *avl_bnk_cnt)
{
    int rv = BCM_E_NONE;

    sal_memset(bkt_info, 0, sizeof(_alpm_bkt_info_t));
    rv = alpm_bkt_alloc_attempt(u, acb, pkm, vrf_id, req_bnk_cnt,
                                ACB_BNK_PER_BKT(acb, vrf_id), bkt_info,
                                avl_bnk_cnt);
    if (rv != BCM_E_FULL) {
        goto end;
    }

    /* rv == BCM_E_FULL
     *
     * No free bkt available, start defragment process to create a
     * new available bucket.
     * It's also bucket sharing process for TH/TD3
     */
    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        rv = alpm_cb_merge(u, acb, pkm, vrf_id);
        return BCM_SUCCESS(rv) ? BCM_E_BUSY : rv;
    }

    rv = alpm_cb_defrag(u, acb, vrf_id);
    if (BCM_SUCCESS(rv)) {
        rv = alpm_bkt_alloc_attempt(u, acb, pkm, vrf_id, req_bnk_cnt,
                                    1, bkt_info, avl_bnk_cnt);
    }

end:
    return rv;
}

static int
alpm_bkt_insert(int u, _alpm_cb_t *acb,
                _bcm_defip_cfg_t *lpm_cfg, _alpm_pvt_node_t *pvt_node,
                uint32 *ent_idx)
{
    int rv = BCM_E_NONE;
    int fmt_update = 0;
    int vrf_id = 0;
    int pfx_len;
    _alpm_bkt_info_t *bkt_info = &PVT_BKT_INFO(pvt_node);

    vrf_id = PVT_BKT_VRF(pvt_node);
    if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
        pfx_len = lpm_cfg->defip_sub_len;
    } else {
        pfx_len = lpm_cfg->defip_sub_len - PVT_KEY_LEN(pvt_node);
    }
    rv = alpm_bkt_ent_get(u, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                          bkt_info, pfx_len, ent_idx, &fmt_update);
    if (rv == BCM_E_FULL) {
        int bnk_cnt = 0;
        /* If all banks are full, check if bucket expansion is required */
        alpm_bkt_bnks_get(u, acb, bkt_info, &bnk_cnt);
        if (ACB_BNK_PER_BKT(acb, vrf_id) > bnk_cnt) {
            /* To do bucket expansion here */
            if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
                rv = alpm_cb_expand_in_bkt(u, acb, pvt_node, bnk_cnt);
            } else {
                rv = alpm_cb_expand(u, acb, pvt_node, bnk_cnt);
            }
            if (BCM_SUCCESS(rv)) {
                rv = alpm_bkt_ent_get(u, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                                      bkt_info, pfx_len, ent_idx, &fmt_update);
            }
        }
    }

    if (BCM_SUCCESS(rv)) {
        rv = alpm_bkt_ent_write(u, acb, pvt_node, lpm_cfg, *ent_idx);
        if (BCM_SUCCESS(rv) && fmt_update && !ACB_BKT_FIXED_FMT(acb, vrf_id)) {
            rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node);
        }
        if (BCM_FAILURE(rv)) {
            rv = alpm_bkt_ent_del(u, acb, pvt_node, *ent_idx);
        }
    }

    if (BCM_SUCCESS(rv)) {
        if (ALPM_LOG_CHECK(BSL_INFO)) {
            char bnk_str[ALPM_BPB_MAX*2+2];
            sal_strcpy(bnk_str, "_ _ _ _ _ _ _ _");
            bnk_str[2 * ALPM_IDX_TO_BNK(acb, *ent_idx)] =
                bkt_info->bnk_fmt[ALPM_IDX_TO_BNK(acb, *ent_idx)] + 48;

            ALPM_INFO(("**ACB(%d).BKT.INS: RBBE (%d %d [%s] %d)%s\n",
                       ACB_IDX(acb), PVT_ROFS(pvt_node),
                       PVT_BKT_IDX(pvt_node), bnk_str, ALPM_IDX_TO_ENT(*ent_idx),
                       (fmt_update ? "*" : "")));
        }
    } else {
        ALPM_VERB(("**ACB(%d).BKT.INS: failed %d\n", ACB_IDX(acb), rv));
    }

    return rv;
}

int
alpm_bkt_delete(int u, _alpm_cb_t *acb,
                _bcm_defip_cfg_t *lpm_cfg, _alpm_pvt_node_t *pvt_node,
                uint32 *key_idx)
{
    int rv = BCM_E_NONE;

    /* TD3ALPMTBD */

    return rv;
}

static int
alpm_bkt_pfx_idx_backup(int u, _alpm_bkt_pfx_arr_t *pfx_arr, uint32 **bak_idx)
{
    int i, rv = BCM_E_NONE;
    int count = pfx_arr->pfx_cnt;
    uint32 *pfx_idx;

    ALPM_ALLOC_EG(pfx_idx, count * sizeof(uint32), "bak_idx");

    for (i = 0; i < count; i++) {
        pfx_idx[i] = pfx_arr->pfx[i]->ent_idx;
    }

    if (bak_idx != NULL) {
        *bak_idx = pfx_idx;
    }

bad:
    return rv;
}

static void
alpm_bkt_pfx_idx_restore(int u, _alpm_bkt_pfx_arr_t *pfx_arr, uint32 *bak_idx)
{
    int i;
    uint32 tmp_idx;
    int count = pfx_arr->pfx_cnt;

    for (i = 0; i < count; i++) {
        tmp_idx = pfx_arr->pfx[i]->ent_idx;
        pfx_arr->pfx[i]->ent_idx = bak_idx[i];
        bak_idx[i] = tmp_idx;
    }

    return ;
}

static int
alpm_bkt_pfx_copy(int u, _alpm_cb_t *acb,
                  _alpm_bkt_pfx_arr_t *pfx_arr,
                  _alpm_pvt_node_t *opvt_node,
                  _alpm_pvt_node_t *npvt_node)
{
    int rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->alpm_bkt_pfx_copy(u, acb, pfx_arr, opvt_node,
                                        npvt_node);
    return rv;
}

static int
alpm_bkt_pfx_clean(int u, _alpm_cb_t *acb,
                   _alpm_pvt_node_t *pvt_node,
                   int count, uint32 *ent_idx)
{
    int rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->alpm_bkt_pfx_clean(u, acb, pvt_node, count, ent_idx);

    return rv;
}

static int
alpm_bkt_pfx_shrink(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node)
{
    int         rv = BCM_E_NONE;
    trie_t      *bkt_trie = NULL;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = trie_traverse(bkt_trie, alpm_trie_pfx_cb, pfx_arr,
                       _TRIE_INORDER_TRAVERSE);
    ALPM_IEG(rv);

    rv = ALPM_DRV(u)->alpm_bkt_pfx_shrink(u, acb, pvt_node, pfx_arr);

    if (BCM_SUCCESS(rv)) {
        int fmt_update = 0;
        rv = alpm_bkt_bnk_shrink(u, acb, ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                                 &PVT_BKT_INFO(pvt_node), &fmt_update);
        if (fmt_update && !ACB_BKT_FIXED_FMT(acb, PVT_BKT_VRF(pvt_node))) {
            (void) alpm_pvt_update_by_pvt_node(u, acb, pvt_node);
        }
    }

bad:
    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }

    return rv;
}

static int
alpm_bkt_trie_split(int u, _alpm_cb_t *acb,
                    _bcm_defip_cfg_t *lpm_cfg,
                    trie_t *split_bkt_trie,
                    _alpm_pvt_node_t *split_pvt_node,
                    int max_split_count)
{
    int                 rv;
    int                 pkm, max_split_len, bkt_cnt;
    uint32              pvt_len, pvt_key[5] = {0};

    _alpm_pvt_node_t    *pvt_node = NULL;
    trie_t              *bkt_trie = NULL;

    pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;
    max_split_len = alpm_trie_max_split_len(u, acb, pvt_node);

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    bkt_cnt = bkt_trie->trie->count;
    rv = trie_split(bkt_trie, max_split_len, FALSE, pvt_key, &pvt_len,
                    &split_bkt_trie->trie, NULL, FALSE, max_split_count);
    ALPM_IEG(rv);

    pkm = ALPM_LPM_PKM(u, lpm_cfg);
    PVT_BKT_TRIE(split_pvt_node)  = split_bkt_trie;
    PVT_BKT_VRF(split_pvt_node)   = ALPM_LPM_VRF_ID(u, lpm_cfg);
    PVT_BKT_PKM(split_pvt_node)   = pkm;
    PVT_KEY_CPY(split_pvt_node, pvt_key);
    PVT_KEY_LEN(split_pvt_node)   = pvt_len;

    ALPM_INFO(("**ACB(%d).BKT.SPLIT %d entries from bkt %d(cnt %d)\n",
               ACB_IDX(acb), split_bkt_trie->trie->count,
               PVT_BKT_IDX(pvt_node), bkt_cnt));

    return rv;

bad:

    if (split_bkt_trie->trie != NULL) {
        (void)trie_merge(bkt_trie, split_bkt_trie->trie, pvt_key, pvt_len);
    }

    return rv;
}

static int
alpm_bkt_split_pvt_add(int u, _alpm_cb_t *acb,
                       _bcm_defip_cfg_t *pvt_lpm_cfg,
                       _alpm_pvt_node_t *pvt_node)
{
    int                 rv = BCM_E_NONE;
    int                 pkm, vrf_id;
    int                 pvt_len = 0;
    trie_t              *pfx_trie = NULL;
    trie_t              *pvt_trie = NULL;
    _alpm_pfx_node_t    *pfx_node = NULL;

    pvt_len = PVT_KEY_LEN(pvt_node);
    pkm     = ALPM_LPM_PKM(u, pvt_lpm_cfg);
    vrf_id  = ALPM_LPM_VRF_ID(u, pvt_lpm_cfg);
    pfx_trie = ALPM_VRF_TRIE(u, vrf_id, pkm);

    /* Assoc-data and bpm_len info of pvt_lpm_cfg comes from trie_find_lpm */
    ALPM_IER(trie_find_lpm(pfx_trie, pvt_node->key, pvt_len,
                              (trie_node_t **)&pfx_node));
    PVT_BPM_LEN(pvt_node) = pfx_node->key_len;
    PVT_BKT_DEF(pvt_node) = pfx_node->bkt_ptr;

    /* Construct KEY(IP address), should come from pvt_key */
    alpm_trie_pfx_to_cfg(u, pvt_node->key, pvt_len, pvt_lpm_cfg);
    sal_memcpy(pvt_lpm_cfg->user_data, pvt_node->key, sizeof(uint32) * 5);
    /* Construct Assoc-data */
    if (pfx_node->bkt_ptr != NULL) {
        alpm_util_adata_trie_to_cfg(u, &pfx_node->bkt_ptr->adata, pvt_lpm_cfg);
    } else {
        pvt_lpm_cfg->defip_flags &=
            ~(BCM_L3_RPE | BCM_L3_DST_DISCARD | BCM_L3_MULTIPATH);
        pvt_lpm_cfg->defip_ecmp_index = 0;
        pvt_lpm_cfg->defip_mc_group = 0;
        pvt_lpm_cfg->defip_prio = 0;
        pvt_lpm_cfg->defip_lookup_class = 0;
        pvt_lpm_cfg->defip_flex_ctr_pool = 0;
        pvt_lpm_cfg->defip_flex_ctr_mode = 0;
        pvt_lpm_cfg->defip_flex_ctr_base_id = 0;
    }
    rv = alpm_pvt_insert(u, acb, pvt_lpm_cfg);
    if (BCM_SUCCESS(rv)) {
        PVT_IDX(pvt_node) = pvt_lpm_cfg->defip_index;
        pvt_trie = ACB_PVT_TRIE(acb, vrf_id, pkm);
        rv = trie_insert(pvt_trie, pvt_node->key, NULL, pvt_len,
                         (trie_node_t *)pvt_node);
    }
    /* rv = *_E_FULL is allowed here */

    return rv;
}

/*
 * OUT pvt_node,
 * OUT bkt_node
 */

int
alpm_cb_find(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg,
             _alpm_pvt_node_t **pvt_node, _alpm_bkt_node_t **bkt_node)
{
    int rv = BCM_E_NONE;

    rv = alpm_pvt_find(u, acb, lpm_cfg, pvt_node);
    if (BCM_SUCCESS(rv)) {
        rv = alpm_bkt_find(u, acb, lpm_cfg, *pvt_node, bkt_node);
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Find and update the match in the database.
 * Returns BCM_E_NONE, if found and updated.
 * BCM_E_NOT_FOUND if not found
 * BCM_E_FAIL, if update failed.
 */
static int
alpm_cb_update(int u, _alpm_cb_t *acb,
               _bcm_defip_cfg_t *lpm_cfg)
{
    _alpm_ppg_data_t pdata;
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_node_t *bkt_node;
    int         rv = BCM_E_NONE;

    /* Find and update the entry */
    ALPM_INFO(("**ACB(%d).FIND.UPDATE ...\n", ACB_IDX(acb)));
    pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;
    bkt_node = (_alpm_bkt_node_t *)lpm_cfg->bkt_node;
    if (bkt_node == NULL) {
        ALPM_ERR(("**ACB(%d).FIND.UPDATE bkt_node == NULL...\n",
                  ACB_IDX(acb)));
        return BCM_E_FAIL;
    }
    alpm_util_adata_cfg_to_trie(u, lpm_cfg, &bkt_node->adata);
    ALPM_IER(alpm_bkt_ent_write(u, acb, pvt_node, lpm_cfg, bkt_node->ent_idx));

    if (ACB_HAS_RTE(acb)) {
        /* Perform INSERT propagation to update assoc_data */
        alpm_ppg_prepare(u, lpm_cfg, lpm_cfg->defip_sub_len, bkt_node, &pdata);
        ALPM_IER(alpm_ppg_assoc_data(u, ALPM_PPG_INSERT, &pdata));
    }
    return rv;
}

static int
alpm_vrf_pfx_trie_add(int u, _alpm_cb_t *acb,
                      _bcm_defip_cfg_t *lpm_cfg, void *udata)
{
    int         rv = BCM_E_NONE, pkm, vrf_id;
    uint32      *prefix;
    uint32      length;

    trie_t      *pfx_trie = NULL;
    trie_node_t *lpmp;
    _alpm_pfx_node_t *pfx_node = NULL, *tmp_node;

    if (!ACB_HAS_RTE(acb)) {
        return rv;
    }

    vrf_id = lpm_cfg->defip_vrf;
    if (vrf_id == BCM_L3_VRF_GLOBAL) {
        /* GLobal low */
        vrf_id = ALPM_VRF_ID_GLO(u);
    } else if (vrf_id < 0) {
        assert(0);
    }
    pkm = ALPM_LPM_PKM(u, lpm_cfg);
    pfx_trie = ALPM_VRF_TRIE(u, vrf_id, pkm);

    if (lpm_cfg->defip_sub_len == 0) {
        /* Add a route already in trie */
        lpmp = NULL;
        rv = trie_find_lpm(pfx_trie, 0, 0, &lpmp);
        if (BCM_SUCCESS(rv)) {
            tmp_node = (_alpm_bkt_node_t *)lpmp;
            tmp_node->bkt_ptr = udata;
        }

        return rv;
    }

    /* Non default route start from here */
    prefix = lpm_cfg->user_data;
    length = lpm_cfg->defip_sub_len;
    pfx_node = alpm_util_alloc(sizeof(_alpm_pfx_node_t), "VRF_trie_node");
    if (pfx_node == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(pfx_node, 0, sizeof(_alpm_bkt_node_t));
    sal_memcpy(pfx_node->key, prefix, sizeof(pfx_node->key));
    pfx_node->key_len = length;
    pfx_node->ent_idx = lpm_cfg->defip_index;
    pfx_node->bkt_ptr = udata;

    rv = trie_insert(pfx_trie, prefix, NULL, length, (trie_node_t *)pfx_node);
    if (BCM_FAILURE(rv)) {
        if (pfx_node != NULL) {
            alpm_util_free(pfx_node);
        }
    }

    return rv;
}

static int
alpm_vrf_pfx_trie_delete(int u, _alpm_cb_t *acb,
                         _bcm_defip_cfg_t *lpm_cfg, void *udata,
                         _alpm_bkt_node_t **alt_def_pfx,
                         uint32 *alt_bpm_len)
{
    int         rv = BCM_E_NONE, rv2, pkm, vrf_id;
    uint32      *prefix;
    uint32      pfx_len, bpm_len;

    trie_t      *pfx_trie = NULL;
    trie_node_t *lpmp = NULL;
    _alpm_pfx_node_t *pfx_node = NULL, *tmp_node;

    if (!ACB_HAS_RTE(acb)) {
        return rv;
    }

    vrf_id = lpm_cfg->defip_vrf;
    if (vrf_id == BCM_L3_VRF_GLOBAL) {
        /* Global low */
        vrf_id = ALPM_VRF_ID_GLO(u);
    } else if (vrf_id < 0) {
        assert(0);
    }
    pkm = ALPM_LPM_PKM(u, lpm_cfg);
    pfx_trie = ALPM_VRF_TRIE(u, vrf_id, pkm);
    prefix      = lpm_cfg->user_data;
    pfx_len     = lpm_cfg->defip_sub_len;

    if (pfx_len != 0) {
        rv = trie_delete(pfx_trie, prefix, pfx_len, &lpmp);
        pfx_node = (_alpm_bkt_node_t *)lpmp;
        ALPM_IEG(rv);

        /* Find the alternative bpm node */
        lpmp = NULL;
        rv = trie_find_lpm(pfx_trie, prefix, pfx_len, &lpmp);
        tmp_node = (_alpm_pfx_node_t *)lpmp;
        if (BCM_SUCCESS(rv)) {
            _alpm_bkt_node_t *t = (_alpm_bkt_node_t *)(tmp_node->bkt_ptr);
            if (t != NULL) {
                /* Means default route exists */
                bpm_len = t->key_len;
            } else {
                /* This should happen only if the lpm is a default route */
                bpm_len = 0;
            }

            *alt_def_pfx = t;
            *alt_bpm_len = bpm_len;
        } else {
            /* Not expected */
            ALPM_IEG(rv);
        }
    } else {
        /* Delete a default route */
        lpmp = NULL;
        rv = trie_find_lpm(pfx_trie, prefix, pfx_len, &lpmp);
        tmp_node = (_alpm_pfx_node_t *)lpmp;
        tmp_node->bkt_ptr = NULL;
    }

    if (pfx_node != NULL) {
        alpm_util_free(pfx_node);
    }

    return rv;

bad:
    /* Recover if necessary */
    if (pfx_node != NULL) {
        rv2 = trie_insert(pfx_trie, prefix, NULL,
                          pfx_len, (trie_node_t *)pfx_node);
        if (BCM_FAILURE(rv2)) {
            ALPM_ERR(("Recover failed\n"));
        }
    }
    return rv;
}

static int
alpm_vrf_pfx_trie_init(int u, int vrf_id, int pkm)
{
    int rv = BCM_E_NONE;
    trie_t  *root = NULL;
    uint32  max_key_len;
    uint32  key[5] = {0};
    _alpm_pfx_node_t    *pfx_node = NULL;

    max_key_len = alpm_util_trie_max_key_len(u, pkm);

    pfx_node = alpm_util_alloc(sizeof(*pfx_node), "Payload for pfx trie key");
    if (pfx_node != NULL) {
        sal_memset(pfx_node, 0, sizeof(*pfx_node));
        rv = trie_init(max_key_len, &ALPM_VRF_TRIE(u, vrf_id, pkm));
        if (BCM_SUCCESS(rv)) {
            root = ALPM_VRF_TRIE(u, vrf_id, pkm);
            rv = trie_insert(root, key, NULL, 0, &(pfx_node->node));
        }

        if (BCM_FAILURE(rv)) {
            alpm_util_free(pfx_node);
            if (root != NULL) {
                trie_destroy(root);
                ALPM_VRF_TRIE(u, vrf_id, pkm) = NULL;
            }
        }
    } else {
        rv = BCM_E_MEMORY;
    }

    return rv;
}

int
alpm_vrf_pfx_trie_deinit(int u, int vrf_id, int pkm)
{
    int rv = BCM_E_NONE;
    trie_t  *root = NULL;
    uint32  key[5] = {0};
    _alpm_pfx_node_t    *pfx_node = NULL;

    root = ALPM_VRF_TRIE(u, vrf_id, pkm);
    if (root == NULL) {
        return rv;
    }
    rv = trie_delete(root, key, 0, (trie_node_t **)&pfx_node);
    if (BCM_SUCCESS(rv)) {
        alpm_util_free(pfx_node);
        (void) trie_destroy(root);
        ALPM_VRF_TRIE(u, vrf_id, pkm) = NULL;
    }

    return rv;
}

int
alpm_vrf_pvt_trie_init(int u, _alpm_cb_t *acb,
                       _bcm_defip_cfg_t *pvt_cfg,
                       _alpm_pvt_node_t **pvt_node,
                       _alpm_bkt_info_t *bkt_info)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id;
    int                 pkm;
    uint32              max_key_len, length = 0;
    _alpm_pvt_node_t    *tmp_node = NULL;
    trie_t              *pvt_root = NULL;

    pkm = ALPM_LPM_PKM(u, pvt_cfg);
    vrf_id = ALPM_LPM_VRF_ID(u, pvt_cfg);

    max_key_len = alpm_util_trie_max_key_len(u, pkm);

    ALPM_IEG(trie_init(max_key_len, &ACB_PVT_TRIE(acb, vrf_id, pkm)));

    pvt_root = ACB_PVT_TRIE(acb, vrf_id, pkm);
    ALPM_ALLOC_EG(tmp_node, sizeof(_alpm_pvt_node_t), "pvt_node");

    /* kshift = 0 */
    ALPM_IEG(trie_init(max_key_len, &PVT_BKT_TRIE(tmp_node)));
    sal_memcpy(&PVT_BKT_INFO(tmp_node), bkt_info, sizeof(_alpm_bkt_info_t));
    PVT_BKT_VRF(tmp_node)   = vrf_id;
    PVT_BKT_PKM(tmp_node)   = pkm;
    PVT_BKT_DEF(tmp_node)   = NULL;
    PVT_BPM_LEN(tmp_node)   = length;
    PVT_IDX(tmp_node)       = pvt_cfg->defip_index;

    *pvt_node = tmp_node;

    return rv;

bad:
    if (tmp_node != NULL) {
        alpm_util_free(tmp_node);
    }

    if (PVT_BKT_TRIE(tmp_node) != NULL) {
        trie_destroy(PVT_BKT_TRIE(tmp_node));
        PVT_BKT_TRIE(tmp_node) = NULL;
    }

    if (pvt_root != NULL) {
        trie_destroy(pvt_root);
        ACB_PVT_TRIE(acb, vrf_id, pkm) = NULL;
    }
    return rv;
}

int
alpm_vrf_pvt_trie_deinit(int u, _alpm_cb_t *acb, int vrf_id, int pkm)
{
    int                 rv = BCM_E_NONE;
    uint32              key[5] = {0};
    _alpm_pvt_node_t    *pvt_node = NULL;
    trie_t              *pvt_root = NULL;

    pvt_root = ACB_PVT_TRIE(acb, vrf_id, pkm);
    rv = trie_delete(pvt_root, key, 0, (trie_node_t **)&pvt_node);
    if (BCM_SUCCESS(rv)) {
        (void) trie_destroy(pvt_root);
        (void) trie_destroy(PVT_BKT_TRIE(pvt_node));
        alpm_util_free(pvt_node);
    }

    return rv;
}

/*
 * alpm_vrf_init
 * Add a VRF default route when a L3 VRF is added
 * Adds a 0.0 entry into VRF
 */
static int
alpm_vrf_init(int u, _alpm_cb_t *acb, int vrf_id, int pkm, uint8 db_type)
{
    int                     rv = BCM_E_NONE;
    int                     bkt_allocated = 0, pvt_trie_inited = 0;
    _alpm_bkt_info_t        bkt_info;
    _bcm_defip_cfg_t        pvt_cfg;
    _alpm_pvt_node_t        *pvt_node = NULL;

    ACB_VRF_DB_TYPE_SET(u, acb, vrf_id, pkm, db_type);
    rv = alpm_bkt_alloc(u, acb, pkm, vrf_id, 1, &bkt_info, NULL);
    if (rv == BCM_E_BUSY) {
        ALPM_IEG(alpm_bkt_alloc(u, acb, pkm, vrf_id, 1, &bkt_info, NULL));
    }
    bkt_allocated = 1;

    sal_memset(&pvt_cfg, 0, sizeof(pvt_cfg));
    pvt_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);
    pvt_cfg.defip_flags |= (pkm != ALPM_PKM_32B) ? BCM_L3_IP6 : 0;

    ALPM_IEG(alpm_vrf_pvt_trie_init(u, acb, &pvt_cfg, &pvt_node, &bkt_info));
    pvt_trie_inited = 1;

    pvt_cfg.bkt_info = (uint32 *)&bkt_info;
    if (ACB_HAS_TCAM(acb)) { /* need for TCAM write */
        pvt_cfg.pvt_node = (uint32 *)pvt_node;
    }
    ALPM_IEG(alpm_pvt_insert(u, acb, &pvt_cfg));
    PVT_IDX(pvt_node) = pvt_cfg.defip_index;
    pvt_cfg.pvt_node = (uint32 *)pvt_node;
    ALPM_IEG(trie_insert(ACB_PVT_TRIE(acb, vrf_id, pkm), pvt_cfg.user_data,
                         NULL, 0, (void *)pvt_node));

    ACB_VRF_INIT_SET(u, acb, vrf_id, pkm);

    if (BCM_SUCCESS(rv) && ACB_HAS_RTE(acb)) {
        rv = alpm_vrf_pfx_trie_init(u, vrf_id, pkm);
    }

bad:
    if (BCM_FAILURE(rv)) {
        /* ALPMTBD rollback */
        if (bkt_allocated) {
            (void)alpm_bkt_free(u, acb, vrf_id, &bkt_info);
        }
        if (pvt_trie_inited) {
            (void)alpm_vrf_pvt_trie_deinit(u, acb, vrf_id, pkm);
        }
    }
    return rv;

}

/*
 * alpm_vrf_deinit
 * Delete the VRF default route when a L3 VRF is destroyed
 */
int
alpm_vrf_deinit(int u, int vrf_id, int pkm)
{
    int                 i, rv = BCM_E_NONE;
    _bcm_defip_cfg_t    lpm_cfg;
    _alpm_pvt_node_t    *pvt_node = NULL;
    _alpm_cb_t          *acb;

    sal_memset(&lpm_cfg, 0, sizeof(lpm_cfg));
    lpm_cfg.defip_vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);
    if (pkm != ALPM_PKM_32B) {
        lpm_cfg.defip_flags |= BCM_L3_IP6;
    }

    for (i = ACB_CNT(u) - 1; i >= 0; i--) {
        acb = ACB(u, i);
        if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
            continue;
        }

        ALPM_IER(alpm_pvt_find(u, acb, &lpm_cfg, &pvt_node));
        /* Banks are already freed via alpm_bkt_pfx_shrink */
        /* ALPM_IER(alpm_bkt_free(u, acb, vrf_id, &PVT_BKT_INFO(pvt_node))); */

        /* add Entry into tcam as default routes for the VRF */
        if (ACB_HAS_TCAM(acb)) {
            lpm_cfg.pvt_node = pvt_node;
            (void)alpm_bkt_bnk_shrink(u, acb,
                    ACB_BKT_VRF_POOL(acb, vrf_id),
                    &PVT_BKT_INFO(pvt_node), NULL);
        } else {
            lpm_cfg.pvt_node = NULL;
        }
        lpm_cfg.bkt_node = NULL;
        ALPM_IER(alpm_pvt_delete(u, acb, &lpm_cfg));
        ALPM_IER(alpm_vrf_pvt_trie_deinit(u, acb, vrf_id, pkm));
        ACB_VRF_INIT_CLEAR(u, acb, vrf_id, pkm);
    }

    rv = alpm_vrf_pfx_trie_deinit(u, vrf_id, pkm);

    return rv;
}

/* What is expected here when entering this function:
 * all occupied banks are continuous.
 *    all i in {i | bkt_info->bnk_fmt[i] is used} are continuous AND
 *    foreach i belongs to {i | bkt_info->bnk_fmt[i] is used}:
 *        bkt_info->vet_bmp[i]!=0
 */
int
alpm_cb_expand(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, int bnk_cnt)
{
    int i, rv = BCM_E_FULL;
    _alpm_bkt_info_t *bkt_info;
    _alpm_bkt_pool_conf_t *bp_conf;
    int bnkpb, start, end;
    int leftnb, rightnb;
    int fmt_update = 0;

    ALPM_VERB(("Expanding ALPM CB %d bkt %d......\n",
               acb->acb_idx, PVT_BKT_IDX(pvt_node)));

    /* There 4 cases when expanding a bucket */
    /* Case 1: neighbour bank available
     *         -> occupy neighbour bank */
    /*
      0 1 2 3 4 5 6 7
     -----------------
     |#|#|#|#|#| | | |
     -----------------
      ^
     rofs=0
     -----------------
     | | |#|#|#|#| | |
     -----------------
          ^
         rofs=2
     -----------------
     | | | |#|#|#|#|#|
     -----------------
            ^
           rofs=3          bkt+1
     -----------------     -----------------
     | | | | | |#|#|#|     |#|#| | | | | | |
     -----------------     -----------------
                ^
               rofs=5
     */

    /* There is a neighbour */
    bkt_info = &PVT_BKT_INFO(pvt_node);
    bp_conf  = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));
    bnkpb = BPC_BNK_PER_BKT(bp_conf);
    start = PVT_ROFS(pvt_node);
    end   = PVT_ROFS(pvt_node) + bnkpb;

    /* Get neighbour bank offset based on bkt-idx */
    for (i = start; i < end; i++) {
        if (!BI_BNK_IS_USED(bkt_info, i % bnkpb)) {
            break;
        }
    }

    leftnb  = PVT_BKT_IDX(pvt_node) * bnkpb + start - 1;
    rightnb = PVT_BKT_IDX(pvt_node) * bnkpb + i;
    if (leftnb >= 0) {
        rv = alpm_bkt_bnk_alloc(u, bp_conf, &leftnb, 1, 0xfffffff);
        if (BCM_SUCCESS(rv)) {
            if (PVT_BKT_IDX(pvt_node) != (leftnb / bnkpb)) {
                /* Update */
                PVT_BKT_IDX(pvt_node) = leftnb / bnkpb;
            }

            PVT_ROFS(pvt_node) = leftnb % bnkpb;
            bkt_info->bnk_fmt[PVT_ROFS(pvt_node)] = ALPM_FMT_DEF;
            fmt_update = TRUE;
            goto end;
        }
    }

    if (rightnb < BPC_BNK_CNT(bp_conf)) {
        rv = alpm_bkt_bnk_alloc(u, bp_conf, &rightnb, 1, 0xfffffff);
        if (BCM_SUCCESS(rv)) {
            bkt_info->bnk_fmt[rightnb % bnkpb] = ALPM_FMT_DEF;
            fmt_update = TRUE;
            goto end;
        }
    }

    /* Case 2: No neighbour bank, but there are other
     * continuous banks available:
     *         -> Move banks to other area */
    start = 0;
    rv = alpm_bkt_bnk_alloc(u, bp_conf, &start, bnk_cnt+1, 1);
    if (BCM_SUCCESS(rv)) {
        _alpm_bkt_info_t dst_bkt;

        sal_memset(&dst_bkt, 0, sizeof(dst_bkt));
        BI_ROFS(&dst_bkt) = start % bnkpb;
        BI_BKT_IDX(&dst_bkt) = start / bnkpb;
        for (i = 0; i <= bnk_cnt; i++) {
            dst_bkt.bnk_fmt[(BI_ROFS(&dst_bkt) + i) % bnkpb] = ALPM_FMT_DEF;
        }

        /* Move banks */
        rv = alpm_bkt_move(u, acb, pvt_node, bkt_info, &dst_bkt);
        goto end;
    }

    /* Case 3: No neighbour bank, no continuous banks available
     *         -> move empty bank to neighbour bank */
    /* TD3ALPMTBD, can be optimized by using partial defragmentation to
     * create a free bucket then do Case 2.
     */

    /* Find nearest unused bit position */
    if (leftnb >= 0 || rightnb < BPC_BNK_CNT(bp_conf)) {
        int nst_bnk = -1;
        rv = alpm_bkt_bnk_nearest_get(u,
                    BPC_BNK_BMP(bp_conf), BPC_BNK_CNT(bp_conf),
                    leftnb, rightnb, &nst_bnk);
        if (BCM_SUCCESS(rv)) {
            /* Move unused nst_bnk */
            if (nst_bnk > rightnb) {
                /* Move nst_bnk to rightnb */
                rv = alpm_bkt_free_bnk_move(u, acb, bp_conf, nst_bnk, rightnb);
                ALPM_IER(rv);
                /* Now rightnb is free */
                rv = alpm_bkt_bnk_alloc(u, bp_conf, &rightnb, 1, 0xfffffff);
                if (BCM_SUCCESS(rv)) {
                    bkt_info->bnk_fmt[rightnb % bnkpb] = ALPM_FMT_DEF;
                    fmt_update = TRUE;
                    goto end;
                }
            } else if (nst_bnk < leftnb) {
                /* Move nst_bnk to leftnb */
                rv = alpm_bkt_free_bnk_move(u, acb, bp_conf, nst_bnk, leftnb);
                ALPM_IER(rv);
                /* Now leftnb is free */
                rv = alpm_bkt_bnk_alloc(u, bp_conf, &leftnb, 1, 0xfffffff);
                if (BCM_SUCCESS(rv)) {
                    if (PVT_BKT_IDX(pvt_node) != (leftnb / bnkpb)) {
                        /* Update */
                        PVT_BKT_IDX(pvt_node) = leftnb / bnkpb;
                    }
                    PVT_ROFS(pvt_node) = leftnb % bnkpb;
                    bkt_info->bnk_fmt[PVT_ROFS(pvt_node)] = ALPM_FMT_DEF;

                    fmt_update = TRUE;
                    goto end;
                }
            } else {
                /* Shouldn't be here */
                rv = BCM_E_FULL;
            }
        }
    }

    /* Case 4: table full */

end:
    if (fmt_update && BCM_SUCCESS(rv)) {
        rv = alpm_pvt_update_by_pvt_node(u, acb, pvt_node);
    }
    if (BCM_SUCCESS(rv)) {
        if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
            rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "Expand");
        }
        acb->acb_cnt.c_expand++;
    }
    return rv;
}

/* What is expected here when entering this function:
 * all occupied banks are continuous.
 *    all i in {i | bkt_info->bnk_fmt[i] is used} are continuous AND
 *    foreach i belongs to {i | bkt_info->bnk_fmt[i] is used}:
 *        bkt_info->vet_bmp[i]!=0
 */
int
alpm_cb_expand_in_bkt(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node, int bnk_cnt)
{
    int i, rv = BCM_E_FULL;
    int bnkpb, end;

    _alpm_bkt_info_t *bkt_info;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_bkt_info_t dst_bkt;

    ALPM_VERB(("Expanding ALPM CB %d bkt %d......\n",
               acb->acb_idx, PVT_BKT_IDX(pvt_node)));

    /* Case 1: bank available in the same bucket
     *         -> occupy the free bank */

    bkt_info = &PVT_BKT_INFO(pvt_node);
    bp_conf  = ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node));
    bnkpb = BPC_BNK_PER_BKT(bp_conf);
    end   = bnkpb;

    /* Occupy the free bank in the same bucket based on bkt-idx */
    for (i = 0; i < end; i++) {
        int glb_bnk;
        if (BI_BNK_IS_USED(bkt_info, i)) {
            continue;
        }

        glb_bnk = PVT_BKT_IDX(pvt_node) * bnkpb + i;
        rv = alpm_bkt_sharing_bnk_alloc(u, acb, PVT_BKT_PKM(pvt_node),
                    PVT_BKT_VRF(pvt_node), bp_conf, &dst_bkt, NULL,
                    glb_bnk, 1, 0xfffffff);
        if (BCM_SUCCESS(rv)) {
            BI_ROFS(bkt_info) = 0;
            bkt_info->bnk_fmt[glb_bnk % bnkpb] =
                dst_bkt.bnk_fmt[glb_bnk % bnkpb];
            goto end;
        }
    }

    /* Case 2: No free bank in the same bucket, but there are other
     * continuous banks available:
     *         -> Move banks to other area */
    rv = alpm_bkt_sharing_bnk_alloc(u, acb, PVT_BKT_PKM(pvt_node),
                PVT_BKT_VRF(pvt_node), bp_conf, &dst_bkt, NULL,
                0, bnk_cnt+1, bnkpb);
    if (BCM_SUCCESS(rv)) {
        /* Move banks */
        rv = alpm_bkt_move(u, acb, pvt_node, bkt_info, &dst_bkt);
        goto end;
    }

    /* Other case: table full */

end:
    if (BCM_SUCCESS(rv)) {
        if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
            rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "Expand");
        }
        acb->acb_cnt.c_expand++;
    } else if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_FULL;
    }
    return rv;
}

int
alpm_cb_defrag(int u, _alpm_cb_t *acb, int vrf_id)
{
    int rv = BCM_E_NONE;
    int i, j, bnkpb;
    int rngl, rngr, rngm;
    _alpm_bkt_pool_conf_t *bp_conf;
    SHR_BITDCL *bnk_bmp;

    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
    bnkpb   = BPC_BNK_PER_BKT(bp_conf);

    if (BPC_BNK_CNT(bp_conf) - BPC_BNK_USED(bp_conf) < bnkpb) {
        return BCM_E_FULL;
    }

    /* Find best zone for defragmentation */
    bnk_bmp = BPC_BNK_BMP(bp_conf);
    rv = alpm_bkt_free_bnk_range_get(u,
            bnk_bmp, BPC_BNK_CNT(bp_conf),
            bnkpb, &rngl, &rngr, &rngm);

    ALPM_IEG(rv);

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_DEFRAG) {
        cli_out("free range get: bpb %d, range left %d, right %d, mid %d\n",
                bnkpb, rngl, rngr, rngm);
        alpm_util_bnk_range_print(u, bnk_bmp, 0xfffffff, 32, rngl, rngr);
    }

    /*
     ---------------------------
     | |#|#| | | | |#|#|#| | | |
     ---------------------------
      ^         ^             ^
      rngl      rngm          rngr

     ---------------------------
     | |#| |#| |#| |#| |#| |#| |
     ---------------------------
      ^           ^           ^
      rngl        rngm        rngr
     */
    for (i = rngm - 1; i > rngl; i--) {
        if (!SHR_BITGET(bnk_bmp, i)) {
            continue;
        }
        for (j = i - 1; j >= rngl; j--) {
            if (SHR_BITGET(bnk_bmp, j)) {
                continue;
            }

            /* bnk_bmp could be changed here */
            rv = alpm_bkt_free_bnk_move(u, acb, bp_conf, j, i);
            ALPM_IEG(rv);
            break;
        }
    }

    for (i = rngm + 1; i < rngr; i++) {
        if (!SHR_BITGET(bnk_bmp, i)) {
            continue;
        }
        for (j = i + 1; j <= rngr; j++) {
            if (SHR_BITGET(bnk_bmp, j)) {
                continue;
            }

            /* bnk_bmp could be changed here */
            rv = alpm_bkt_free_bnk_move(u, acb, bp_conf, j, i);
            ALPM_IEG(rv);
            break;
        }
    }

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_DEFRAG) {
        cli_out("After Defrag: \n");
        alpm_util_bnk_range_print(u, bnk_bmp, 0xfffffff, 32,
                                  rngl, rngr);
    }

    acb->acb_cnt.c_defrag++;

bad:
    return rv;
}

static int
alpm_cb_split(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg,
              _alpm_bkt_info_t *bkt_info, int max_spl_cnt,
              _alpm_pvt_node_t **split_pvt_node)
{
    int     rv = BCM_E_NONE;
    int     max_key_len, pkm;
    int     bkt_allocated = 0;
    int     vrf_id = 0;

    uint32  *bak_idx = NULL;

    trie_t      *nbkt_trie = NULL;

    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_pvt_node_t    *opvt_node = NULL;
    _alpm_pvt_node_t    *npvt_node = NULL;
    _bcm_defip_cfg_t    pvt_lpm_cfg;

    pkm    = ALPM_LPM_PKM(u, lpm_cfg);
    vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    ALPM_ALLOC_EG(npvt_node, sizeof(*npvt_node), "pvt_node");

    /* 2. Allocate a new bucket based on the split prefixes */
    /* min_sz, in TH-style-ALPM devices, it's entry count,
     * in FMT-enabled deivces, it's bank count */
    opvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;

    /* 1. trie_split to get split-prefix set */
    max_key_len = alpm_util_trie_max_key_len(u, pkm);
    ALPM_IEG(trie_init(max_key_len, &nbkt_trie));
    rv = alpm_bkt_trie_split(u, acb, lpm_cfg, nbkt_trie, npvt_node, max_spl_cnt);
    ALPM_IEG(rv);

    rv = trie_traverse(nbkt_trie, alpm_trie_pfx_cb, pfx_arr,
                       _TRIE_INORDER_TRAVERSE);
    ALPM_IEG(rv);

    /* for !FIXED_FMT:
     * Need to sort pfx_arr here, node with longer prefix length comes first,
     * Should let longer bkt node to choose bkt_ent_get first
     */
    if (!ACB_BKT_FIXED_FMT(acb, 1)) {
        _shr_sort(pfx_arr->pfx, pfx_arr->pfx_cnt, sizeof(_alpm_bkt_node_t *),
                  alpm_util_pfx_len_cmp);
    }

    /* 3. Save old ent_idx of prefixes for recovery */
    rv = alpm_bkt_pfx_idx_backup(u, pfx_arr, &bak_idx);
    ALPM_IEG(rv);

    /* 3. HW: Copy split prefixes from old bucket to new bucket */
    sal_memcpy(&PVT_BKT_INFO(npvt_node), bkt_info, sizeof(_alpm_bkt_info_t));
    bkt_allocated = TRUE;

    /* bkt_pfx_copy updates ent_idx of each prefix in the new trie */
    rv = alpm_bkt_pfx_copy(u, acb, pfx_arr, opvt_node, npvt_node);
    ALPM_IEG(rv);

    /* Free unused banks of new pivot to Global bank pool */
    (void)alpm_bkt_bnk_shrink(u, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                              &PVT_BKT_INFO(npvt_node), NULL);
    /* 4. HW: insert new pivot
     *    recursion happens here: BCM_E_FULL is expected
     */
    sal_memcpy(&pvt_lpm_cfg, lpm_cfg, sizeof(pvt_lpm_cfg));
    /* never insert new pvt with HIT=1 (from original lpm_cfg->defip_flags) */
    pvt_lpm_cfg.defip_flags &= ~BCM_L3_HIT;
    if (ACB_HAS_TCAM(acb)) {
        pvt_lpm_cfg.pvt_node = (uint32 *)npvt_node;
        pvt_lpm_cfg.bkt_info = NULL;
    } else {
        pvt_lpm_cfg.pvt_node = NULL;
        pvt_lpm_cfg.bkt_info = (uint32 *)&PVT_BKT_INFO(npvt_node);
        if (ACB_BKT_FIXED_FMT(acb, 1)) {
            pvt_lpm_cfg.bkt_kshift = 0;
        } else {
            /* for upr acb bkt */
            pvt_lpm_cfg.bkt_kshift = PVT_KEY_LEN(npvt_node);
        }
    }
    pvt_lpm_cfg.bkt_node = NULL;

    rv = alpm_bkt_split_pvt_add(u, acb, &pvt_lpm_cfg, npvt_node);
    /* rv = *_E_FULL is allowed here */
    if (rv == BCM_E_FULL) {
        ALPM_INFO(("**ACB(%d).BKT.SPLIT return Full\n", ACB_IDX(acb)));
    }
    ALPM_IEG(rv);

    /*
     * In case of pivot insert failure, we need to :
     *     a). recover split_trie
     *     b). Invalidate new bucket.
     */

    /* 5. HW: Invalidate split prefixes in the old bucket */
    (void)alpm_bkt_pfx_clean(u, acb, opvt_node, pfx_arr->pfx_cnt, bak_idx);

    /* 6. SW: Shrink old bucket,
     * A important step to make sure banks are continuously occupied by pivot
     */
    (void)alpm_bkt_pfx_shrink(u, acb, opvt_node);

    *split_pvt_node = npvt_node;

    /* 7. SW: release temporary resources */
    alpm_util_free(bak_idx);
    alpm_util_free(pfx_arr);

    if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
        rv = alpm_cb_pvt_sanity_cb(u, acb, opvt_node, "SplitOpvt");
        if (BCM_SUCCESS(rv)) {
            rv = alpm_cb_pvt_sanity_cb(u, acb, npvt_node, "SplitNpvt");
        }
    }

    acb->acb_cnt.c_split++;
    if (BCM_SUCCESS(rv)) {
        ALPM_MERGE_STATE_CHANGED(u, vrf_id);
    }

    return rv;

bad:
    /* Clean new bucket */
    if (bak_idx != NULL) {
        alpm_bkt_pfx_idx_restore(u, pfx_arr, bak_idx);
        (void)alpm_bkt_pfx_clean(u, acb, npvt_node, pfx_arr->pfx_cnt, bak_idx);
        alpm_util_free(bak_idx);
    }

    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }

    /* free allocated bucket */
    if (bkt_allocated) {
        (void)alpm_bkt_free(u, acb, vrf_id, &PVT_BKT_INFO(npvt_node));
    }

    if (nbkt_trie != NULL) {
        if (nbkt_trie->trie != NULL) {
            (void)trie_merge(PVT_BKT_TRIE(opvt_node), nbkt_trie->trie,
                             npvt_node->key, PVT_KEY_LEN(npvt_node));
        }

        trie_destroy(nbkt_trie);
    }

    if (npvt_node != NULL) {
        alpm_util_free(npvt_node);
    }

    return rv;
}

static int
alpm_cb_merge_pvt(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *src_pvt,
                  _alpm_pvt_node_t *dst_pvt, int merge_dir)
{
    int rv = BCM_E_NONE;
    int i, bnk_pbk;
    int pkm, vrf_id;
    uint32  *bak_idx = NULL;

    SHR_BITDCL *bnk_bmp;
    trie_node_t *tn = NULL;
    _alpm_pvt_node_t *prt_pvt, *chd_pvt, *tmp_pvt;
    _alpm_bkt_info_t tmp_bi;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;

    pkm    = PVT_BKT_PKM(src_pvt);
    vrf_id = PVT_BKT_VRF(src_pvt);
    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
    bnk_pbk = BPC_BNK_PER_BKT(bp_conf);
    bnk_bmp = BPC_BNK_BMP(bp_conf);

    /* 1. Expand the bkt of dst_pvt */
    for (i = PVT_BKT_IDX(dst_pvt) * bnk_pbk;
         i < (PVT_BKT_IDX(dst_pvt) + 1) * bnk_pbk; i++) {
        if (SHR_BITGET(bnk_bmp, i)) {
            continue;
        }

        PVT_BKT_INFO(dst_pvt).bnk_fmt[i % bnk_pbk] =
            alpm_bkt_bnk_def_fmt_get(u, acb, pkm, vrf_id);
        SHR_BITSET(bnk_bmp, i);
        BPC_BNK_USED(bp_conf) += 1;
    }

    /* 2. Copy pfx from src_pvt to dst_pvt */
    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = trie_traverse(PVT_BKT_TRIE(src_pvt), alpm_trie_pfx_cb, pfx_arr,
                       _TRIE_INORDER_TRAVERSE);
    ALPM_IEG(rv);
    rv = alpm_bkt_pfx_idx_backup(u, pfx_arr, &bak_idx);
    ALPM_IEG(rv);
    rv = alpm_bkt_pfx_copy(u, acb, pfx_arr, src_pvt, dst_pvt);
    ALPM_IEG(rv);

    if (merge_dir == ACB_MERGE_PARENT_TO_CHILD) {
        prt_pvt = src_pvt;
        chd_pvt = dst_pvt;
    } else {
        prt_pvt = dst_pvt;
        chd_pvt = src_pvt;
    }
    if (merge_dir == ACB_MERGE_PARENT_TO_CHILD) {
        /* 3. Update pvt bkt_ptr if copy from parent to child */
        sal_memcpy(&tmp_bi, &PVT_BKT_INFO(prt_pvt), sizeof(_alpm_bkt_info_t));
        sal_memcpy(&PVT_BKT_INFO(prt_pvt), &PVT_BKT_INFO(chd_pvt),
                   sizeof(_alpm_bkt_info_t));
        rv = alpm_pvt_update_by_pvt_node(u, acb, prt_pvt);
        ALPM_IEG(rv);
        /* Recovery bkt info for invalidation */
        sal_memcpy(&PVT_BKT_INFO(prt_pvt), &tmp_bi, sizeof(_alpm_bkt_info_t));
    }

    /* 4. Invalid old entries */
    rv = alpm_bkt_pfx_clean(u, acb, src_pvt, pfx_arr->pfx_cnt, bak_idx);
    ALPM_IEG(rv);
    rv = alpm_bkt_bnk_shrink(u, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                             &PVT_BKT_INFO(src_pvt), NULL);
    ALPM_IEG(rv);
    rv = alpm_bkt_bnk_shrink(u, acb, ACB_BKT_VRF_POOL(acb, vrf_id),
                             &PVT_BKT_INFO(dst_pvt), NULL);
    ALPM_IEG(rv);

    /* 5. Merge trie */
    rv = trie_merge(PVT_BKT_TRIE(prt_pvt), PVT_BKT_TRIE(chd_pvt)->trie,
                    chd_pvt->key, chd_pvt->key_len);
    ALPM_IEG(rv);

    /* 6. Delete child pvt entry from pvt table */
    rv = alpm_pvt_delete_by_pvt_node(u, acb, chd_pvt);
    ALPM_IEG(rv);

    /* 7. Destroy child pvt from trie, tn is chd_pvt */
    rv = trie_delete(ACB_PVT_TRIE(acb, vrf_id, pkm), chd_pvt->key,
                     PVT_KEY_LEN(chd_pvt), &tn);
    ALPM_IEG(rv);

    /* Should copy bi before trie_destroy tn */
    if (merge_dir == ACB_MERGE_PARENT_TO_CHILD) {
        int tab_idx, bnk;
        _alpm_bkt_info_t *bi;
        bi = &PVT_BKT_INFO(prt_pvt);
        sal_memcpy(bi, &PVT_BKT_INFO(chd_pvt), sizeof(_alpm_bkt_info_t));

        /* Update pvt_ptr for child bucket */
        for (bnk = 0; bnk < bnk_pbk; bnk++) {
            if (BI_BNK_IS_USED(bi, bnk)) {
                tab_idx = ALPM_TAB_IDX_GET_BI_BNK(acb, bi, bnk);
                bp_conf->pvt_ptr[tab_idx] = prt_pvt;
            }
        }
    }

    tmp_pvt = (_alpm_pvt_node_t *)tn;
    trie_destroy(PVT_BKT_TRIE(tmp_pvt));
    alpm_util_free(tmp_pvt);
    alpm_util_free(bak_idx);
    alpm_util_free(pfx_arr);

    return rv;
    /* 8. Update hit bits */

bad:
    if (bak_idx) {
        alpm_util_free(bak_idx);
    }
    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }
    return rv;
}

static int
alpm_cb_merge_cb(trie_node_t *ptrie, trie_node_t *trie,
                 trie_traverse_states_e_t *state, void *info)
{
    int u, rv;
    int bnkpb, entpb;
    int vrf_id;
    _alpm_cb_t *acb;
    _alpm_pvt_node_t *prt_pvt, *chd_pvt, *src_pvt, *dst_pvt;
    _alpm_cb_merge_ctrl_t *merge_ctrl;
    _alpm_cb_merge_type_t merge_dir = ACB_MERGE_INVALID;

    prt_pvt = (_alpm_pvt_node_t *)ptrie;
    chd_pvt = (_alpm_pvt_node_t *)trie;
    if (!prt_pvt) {
        return BCM_E_NONE;
    }

    if (PVT_BKT_TRIE(prt_pvt)->trie == NULL) {
        merge_dir = ACB_MERGE_PARENT_TO_CHILD;
    }

    if (PVT_BKT_TRIE(chd_pvt)->trie == NULL) {
        merge_dir = ACB_MERGE_CHILD_TO_PARENT;
    }

    merge_ctrl = (_alpm_cb_merge_ctrl_t *)info;
    u   = merge_ctrl->unit;
    acb = merge_ctrl->acb;
    vrf_id = merge_ctrl->vrf_id;
    bnkpb  = merge_ctrl->bnk_pbkt;
    entpb  = merge_ctrl->ent_pbnk;

    if (merge_dir == ACB_MERGE_INVALID) {
        int mrts, prt_bnks, chd_bnks;

        mrts = PVT_BKT_TRIE(prt_pvt)->trie->count +
               PVT_BKT_TRIE(chd_pvt)->trie->count;
        if (mrts > bnkpb * entpb) {
            return BCM_E_NONE;
        }

        prt_bnks = bnkpb - alpm_util_bkt_token_bnks(u, acb, vrf_id, prt_pvt);
        chd_bnks = bnkpb - alpm_util_bkt_token_bnks(u, acb, vrf_id, chd_pvt);

        if (mrts <= prt_bnks * entpb) {
            merge_dir   = ACB_MERGE_CHILD_TO_PARENT;
        } else if (mrts <= chd_bnks * entpb) {
            merge_dir   = ACB_MERGE_PARENT_TO_CHILD;
        } else {
            return BCM_E_NONE;
        }
    }

    if (merge_dir == ACB_MERGE_PARENT_TO_CHILD) {
        src_pvt = prt_pvt;
        dst_pvt = chd_pvt;
    } else {
        src_pvt = chd_pvt;
        dst_pvt = prt_pvt;
    }

    rv = alpm_cb_merge_pvt(u, acb, src_pvt, dst_pvt, merge_dir);

    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("alpm merge failed rv %d\n", rv));
        return rv;
    }

    *state = TRIE_TRAVERSE_STATE_DELETED;
    if (++merge_ctrl->merge_count >= ACB_MERGE_THRESHOLD) {
        *state = TRIE_TRAVERSE_STATE_DONE;
    }

    acb->acb_cnt.c_merge++;

    return BCM_E_NONE;

}

int
alpm_cb_merge(int u, _alpm_cb_t *acb, int pkm, int vrf_id)
{
    int rv = BCM_E_NONE;
    uint8 fmt;
    trie_t *pvt_trie;
    _alpm_cb_merge_ctrl_t merge_ctrl;

    pvt_trie = ACB_PVT_TRIE(acb, vrf_id, pkm);
    if (!ALPM_MERGE_REQD(u, vrf_id) ||
        pvt_trie == NULL ||
        pvt_trie->trie == NULL) {
        return BCM_E_FULL;
    }

    sal_memset(&merge_ctrl, 0, sizeof(merge_ctrl));
    merge_ctrl.unit     = u;
    merge_ctrl.pkm      = pkm;
    merge_ctrl.vrf_id   = vrf_id;
    merge_ctrl.acb      = acb;

    fmt = alpm_bkt_bnk_def_fmt_get(u, acb, pkm, vrf_id);
    merge_ctrl.bnk_pbkt = ACB_BNK_PER_BKT(acb, vrf_id);
    merge_ctrl.ent_pbnk = ACB_FMT_ENT_MAX(acb, fmt);

    rv = trie_traverse2(pvt_trie, alpm_cb_merge_cb, &merge_ctrl,
                        _TRIE_POSTORDER_TRAVERSE);

    if (BCM_FAILURE(rv)) {
        ALPM_ERR(("Logic bucket merge failed, pkm %d vrf_id %d rv %d.\n",
                  pkm, vrf_id, rv));
    }

    if (merge_ctrl.merge_count == 0) {
        ALPM_MERGE_STATE_CHKED(u, vrf_id);
        rv = BCM_E_FULL;
    }

    return rv;
}

int
alpm_cb_delete(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int rv = BCM_E_NONE;
    int pkm, vrf_id, pfx_len;
    int bkt_empty;
    uint32 *prefix;

    _alpm_ppg_data_t ppg_data;
    _alpm_pvt_node_t *pvt_node = NULL;
    _alpm_bkt_node_t *bkt_node = NULL;
    _alpm_bkt_node_t *alt_def_pfx = NULL;
    uint32           alt_bmp_len = 0;

    trie_t *bkt_trie = NULL;
    trie_node_t *tn = NULL;

    pkm     = ALPM_LPM_PKM(u, lpm_cfg);
    vrf_id  = ALPM_LPM_VRF_ID(u, lpm_cfg);
    prefix  = lpm_cfg->user_data;
    pfx_len = lpm_cfg->defip_sub_len;
    pvt_node = lpm_cfg->pvt_node;

    /* Check Init */
    if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
        return BCM_E_PARAM;
    }

    if (pvt_node == NULL) {
        ALPM_IEG(alpm_pvt_find(u, acb, lpm_cfg, &pvt_node));
        lpm_cfg->pvt_node = (uint32 *)pvt_node;
    }

    bkt_trie = PVT_BKT_TRIE(pvt_node);
    rv = trie_delete(bkt_trie, prefix, pfx_len, &tn);
    bkt_node = (_alpm_bkt_node_t *)tn;
    ALPM_IEG(rv);

    rv = alpm_vrf_pfx_trie_delete(u, acb, lpm_cfg, pvt_node,
                                  &alt_def_pfx, &alt_bmp_len);
    ALPM_IEG(rv);

    /* do insert propagation to update existing default datas */
    if (ACB_HAS_RTE(acb)) {
        /* Fill in AUX Scratch and perform PREFIX Operation */
        alpm_ppg_prepare(u, lpm_cfg, alt_bmp_len, alt_def_pfx, &ppg_data);
        rv = alpm_ppg_assoc_data(u, ALPM_PPG_DELETE, &ppg_data);
        ALPM_IEG(rv);
    }

    /* Delete pvt entry if bucket is empty */
    bkt_empty = (PVT_KEY_LEN(pvt_node) != 0 && bkt_trie->trie == NULL);
    if (bkt_empty) {
        rv = alpm_pvt_delete_by_pvt_node(u, acb, pvt_node);
        ALPM_IEG(rv);
    }

    /* Invalid the bucket entry */
    rv = alpm_bkt_pfx_clean(u, acb, pvt_node, 1, &bkt_node->ent_idx);
    ALPM_IEG(rv);

    if (ACB_BKT_WRA(acb, PVT_BKT_VRF(pvt_node))) {
        if (bkt_empty) {
            (void)alpm_bkt_bnk_shrink(u, acb,
                    ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                    &PVT_BKT_INFO(pvt_node), NULL);
        } else {
            (void)alpm_bkt_pfx_shrink(u, acb, pvt_node);
        }
    } else {
        if (bkt_empty || bkt_trie->trie != NULL ||
            VRF_ROUTE_CNT(acb, vrf_id, pkm) == 0) {
            (void)alpm_bkt_bnk_shrink(u, acb,
                    ACB_BKT_VRF_POOL(acb, PVT_BKT_VRF(pvt_node)),
                    &PVT_BKT_INFO(pvt_node), NULL);
        }
    }

    alpm_util_free(bkt_node);

    if (bkt_empty) {
        rv = trie_delete(ACB_PVT_TRIE(acb, vrf_id, pkm), pvt_node->key,
                         PVT_KEY_LEN(pvt_node), &tn);
        if (BCM_FAILURE(rv)) {
            ALPM_ERR(("Trie delete node failure\n"));
        }

        (void)trie_destroy(bkt_trie);
        alpm_util_free(pvt_node);
    } else {
        if (BCM_SUCCESS(rv) &&
            ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
            rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "CBDelete");
        }
    }

    if (BCM_SUCCESS(rv)) {
        ALPM_MERGE_STATE_CHANGED(u, vrf_id);
    }

    VRF_ROUTE_DEC(acb, vrf_id, pkm);

    return rv;

bad:
    /* Roll back ? */
    return rv;
}

int
alpm_cb_insert(int u, _alpm_cb_t *acb, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int                 pkm = ALPM_LPM_PKM(u, lpm_cfg);
    uint32              *prefix, length;
    uint32              ent_idx;
    int                 retry_count = -1;

    trie_t              *bkt_trie = NULL;
    trie_node_t         *tmp_node = NULL;

    _alpm_ppg_data_t    ppg_data;
    _alpm_bkt_node_t    *bkt_node = NULL;
    _alpm_pvt_node_t    *split_pvt_node = NULL;
    _alpm_pvt_node_t    *pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;

    /* Init ALPM structure for first insert */
    if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
        uint8 db_type = alpm_util_route_type_get(u, lpm_cfg);
        ALPM_IEG(alpm_vrf_init(u, acb, vrf_id, pkm, db_type));
    }

retry:
    retry_count ++;
    if (pvt_node == NULL) {
        ALPM_IEG(alpm_pvt_find(u, acb, lpm_cfg, &pvt_node));
        lpm_cfg->pvt_node = (uint32 *)pvt_node;
    }

    /* entry shouldn't exist, insert the entry into the RAM */
    rv = alpm_bkt_insert(u, acb, lpm_cfg, pvt_node, &ent_idx);
    if (rv == BCM_E_FULL) {
        int     avl_bnk_cnt = 0;
        int     max_spl_cnt = 0xffff;
        _alpm_bkt_info_t bkt_info;

        /* Perform bucket split */
        if (!bcm_esw_alpm_tcam_avail(u, pkm, vrf_id) &&
            ACB_HAS_TCAM(acb)) {
            rv = alpm_cb_merge(u, acb, pkm, vrf_id);
            ALPM_IEG(rv);
            /* pvt node could be merged and freed,
             * Need a faster version ALPMTBD */
            pvt_node = NULL;
            goto retry;
        }

        rv = alpm_bkt_alloc(u, acb, ALPM_LPM_PKM(u, lpm_cfg), vrf_id,
                            ACB_BNK_PER_BKT(acb, vrf_id),
                            &bkt_info, &avl_bnk_cnt);
        if (rv == BCM_E_BUSY) {
            /* pvt node could be merged and freed,
             * Need a faster version ALPMTBD */
            pvt_node = NULL;
            goto retry;
        } else if (rv == BCM_E_FULL) {
            if (ACB_BKT_FIXED_FMT(acb, vrf_id) && avl_bnk_cnt > 0) {
                rv = alpm_bkt_alloc(u, acb, ALPM_LPM_PKM(u, lpm_cfg), vrf_id,
                                    avl_bnk_cnt, &bkt_info, NULL);
                if (rv == BCM_E_BUSY) {
                    /* pvt node could be merged and freed,
                     * Need a faster version ALPMTBD */
                    pvt_node = NULL;
                    goto retry;
                }
            }
            ALPM_IEG(rv);
        } else {
            avl_bnk_cnt = ACB_BNK_PER_BKT(acb, vrf_id);
        }

        if (ACB_BKT_FIXED_FMT(acb, vrf_id)) {
            uint8 def_fmt = alpm_bkt_bnk_def_fmt_get(u, acb, pkm, vrf_id);
            max_spl_cnt = avl_bnk_cnt * ACB_FMT_ENT_MAX(acb, def_fmt) - 1;
        }

        rv = alpm_cb_split(u, acb, lpm_cfg, &bkt_info, max_spl_cnt,
                           &split_pvt_node);
        if (BCM_FAILURE(rv)) {
            (void)alpm_bkt_free(u, acb, vrf_id, &bkt_info);
            ALPM_IEG(rv);
        }

        /* Need a faster version ALPMTBD */
        rv = alpm_pvt_find(u, acb, lpm_cfg, &pvt_node);
        ALPM_IEG(rv);

        lpm_cfg->pvt_node = (uint32 *)pvt_node;
        rv = alpm_bkt_insert(u, acb, lpm_cfg, pvt_node, &ent_idx);
        if (rv == BCM_E_FULL) {
            /* Still FULL? */
            char *buf;
            buf = sal_alloc(512, "print_buf");
            if (buf != NULL) {
                int offset = ACB_BKT_FIXED_FMT(acb, 1) ?
                                0 : PVT_KEY_LEN(pvt_node);
                alpm_util_fmt_bkt_info(buf, acb, &PVT_BKT_INFO(pvt_node));
                ALPM_ERR(("BKT FULL after split: %s pvt_len: %d "
                          "exp_fmt_len: >=%d\n", buf, PVT_KEY_LEN(pvt_node),
                          lpm_cfg->defip_sub_len - offset));
                sal_free(buf);
            }
        }
    }

    ALPM_IEG(rv);

    lpm_cfg->defip_index = ent_idx;

    /* Allocate Payload */
    ALPM_ALLOC_EG(bkt_node, sizeof(_alpm_bkt_node_t), "bkt_node");
    prefix = lpm_cfg->user_data;
    length = lpm_cfg->defip_sub_len;
    sal_memcpy(bkt_node->key, prefix, sizeof(bkt_node->key));
    bkt_node->key_len   = length;
    bkt_node->ent_idx   = ent_idx;
    alpm_util_adata_cfg_to_trie(u, lpm_cfg, &bkt_node->adata);
    /* Insert the prefix */
    /* bucket trie insert */
    bkt_trie = PVT_BKT_TRIE(pvt_node);
    rv = trie_insert(bkt_trie, prefix, NULL, length, (trie_node_t *)bkt_node);
    ALPM_IEG(rv);

    rv = alpm_vrf_pfx_trie_add(u, acb, lpm_cfg, bkt_node);
    if (BCM_FAILURE(rv)) {
        (void)trie_delete(bkt_trie, prefix, length, &tmp_node);
        goto bad;
    }

    /* do insert propagation to update existing default datas */
    if (ACB_HAS_RTE(acb)) {
        /* Fill in AUX Scratch and perform PREFIX Operation */
        alpm_ppg_prepare(u, lpm_cfg, 0, bkt_node, &ppg_data);
        rv = alpm_ppg_assoc_data(u, ALPM_PPG_INSERT, &ppg_data);
    }
    if (BCM_SUCCESS(rv) &&
        ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_NODE) {
        rv = alpm_cb_pvt_sanity_cb(u, acb, pvt_node, "CBInsert");
    }

    return rv;

bad:
    if (bkt_node != NULL) {
        alpm_util_free(bkt_node);
    }

    return rv;
}

static int
bcm_esw_alpm_ctrl_deinit(int u)
{
    int rv = BCM_E_NONE;

    if (ALPM_DRV(u)) {
        ALPM_DRV(u)->alpm_ctrl_deinit(u);
    }

    if (ALPMC(u)->_vrf_pfx_hdl != NULL) {
        alpm_util_free(ALPMC(u)->_vrf_pfx_hdl);
    }

    if (ALPMC(u)->_alpm_merge_state != NULL) {
        alpm_util_free(ALPMC(u)->_alpm_merge_state);
    }

    if (ALPMC(u) != NULL) {
        alpm_util_free(ALPMC(u));
        ALPMC(u) = NULL;
    }

    return rv;
}

int
bcm_esw_alpm_ctrl_init(int u)
{
    int rv = BCM_E_NONE;
    int vrf_id_cnt;
    int alloc_sz;
    int pair_ent_cnt = 0;
    int max_tcams = SOC_L3_DEFIP_MAX_TCAMS_GET(u);
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(u);
    int def_ipv6_128b_entries = tcam_depth * max_tcams / 4;

    alloc_sz = sizeof(_alpm_ctrl_t);
    ALPM_ALLOC_EG(ALPMC(u), alloc_sz, "ALPMC");

    ALPMC(u)->_alpm_max_vrf_id = SOC_VRF_MAX(u);

    /* Init ALPMC variables */
    ALPM_HIT_SKIP(u) = soc_property_get(u, spn_L3_ALPM_HIT_SKIP, 0);
    ALPM_MODE(u) = soc_alpm_cmn_mode_get(u);
    if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_PARALLEL) ||
        ALPM_MODE_CHK(u, BCM_ALPM_MODE_TCAM_ALPM)) {
        ALPM_TCAM_ZONED(u) = TRUE;
    }
    ALPMC(u)->_alpm_128b = soc_property_get(u, spn_IPV6_LPM_128B_ENABLE, 1);
    /* 256 entries per TCAM, 2 TCAMs per block, 4 blocks in total.
     * by default we have 2 blocks paired. */
    if (ALPMC(u)->_alpm_128b) {
        pair_ent_cnt = soc_property_get(u, spn_NUM_IPV6_LPM_128B_ENTRIES, def_ipv6_128b_entries);
        if (pair_ent_cnt > (def_ipv6_128b_entries * 2)) {
            pair_ent_cnt = def_ipv6_128b_entries * 2;
        }
    }

    ALPMC(u)->_alpm_bulk_wr_threshold =
        soc_property_get(u, "l3_alpm2_bulk_wr_threshold", 16);

    ALPMC(u)->_alpm_spl[ALPM_PKM_32B] =
        soc_property_get(u, "l3_alpm2_spl_hw", 24);
    ALPMC(u)->_alpm_spl[ALPM_PKM_64B] =
        soc_property_get(u, "l3_alpm2_spl_sw", 56);
    ALPMC(u)->_alpm_spl[ALPM_PKM_128] =
        soc_property_get(u, "l3_alpm2_spl_dw", 120);

    ALPMC(u)->_alpm_dbg_bmp = soc_property_get(u, "l3_alpm2_dbg_bmp", 0x0);
    if (ALPMC(u)->_alpm_dbg_bmp != 0) {
        int i;
        ALPM_INFO(("[ALPM-INIT] _alpm_dbg_bmp enabled bit(s) ["));
        for (i = 0; i < 32; i++) {
            if (ALPMC(u)->_alpm_dbg_bmp & (1 << i)) {
                ALPM_INFO(("%d ", i));
            }
        }
        ALPM_INFO(("]\n"));
    }

    ALPMC(u)->_alpm_dbg_info_bmp = soc_property_get(u, "l3_alpm2_dbg_info_bmp", 0x07);

    /* Private VRFs + 2 Global VRFs */
    vrf_id_cnt = ALPM_VRF_ID_CNT(u);
    alloc_sz = sizeof(_alpm_pfx_ctrl_t) * vrf_id_cnt;
    ALPM_ALLOC_EG(ALPMC(u)->_vrf_pfx_hdl, alloc_sz, "_vrf_pfx_hdl");

    alloc_sz = sizeof(int) * vrf_id_cnt;
    ALPM_ALLOC_EG(ALPMC(u)->_alpm_merge_state, alloc_sz, "_alpm_merge_state");

    if (soc_feature(u, soc_feature_alpm2)) {
        ALPMC(u)->alpm_driver = &th_alpm_driver;
    }

    rv = ALPM_DRV(u)->alpm_ctrl_init(u);
    if (BCM_FAILURE(rv)) {
        goto bad;
    }

    return rv;

bad:
    bcm_esw_alpm_ctrl_deinit(u);
    return rv;
}

/* Initialize ALPM Subsytem */

int
bcm_esw_alpm_deinit(int u)
{
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(alpm_trie_clear_all(u));
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_trace_deinit(u));
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_hit_deinit(u));
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_ctrl_deinit(u));
    BCM_IF_ERROR_RETURN(bcm_esw_alpm_tcam_deinit(u));
    alpm_util_mem_stat_clear();

    return rv;
}

static void
bcm_esw_alpm_init_done(int u, int rv)
{
    int i;
    int banks, mode, urpf, bit128;
    char *mode_str[] = {
        "invalid",
        "combined",
        "parallel",
        "mixed"
    };

    if (BCM_FAILURE(rv)) {
        LOG_CLI(("*** unit %d: alpm init failed\n", u));
        return;
    }

    /* pass */
    /* *** unit 0: alpm level 2 loaded: 4 banks in combined-128 (urpf) */
    urpf = SOC_URPF_STATUS_GET(u);
    mode = ALPM_MODE(u);
    bit128 = ALPMC(u)->_alpm_128b;

    for (i = 0; i < ACB_CNT(u); i++) {
        banks = ACB_PHY_BNK_PER_BKT(ACB(u, i));
        LOG_CLI(("*** unit %d: alpm level %d loaded: %d banks in %s-%s %s\n",
                 u, i+2, banks, mode_str[mode+1], bit128 ? "128" : "64",
                 urpf ? "(urpf)" : ""));

    }
}

int
bcm_esw_alpm_init(int u)
{
    int rv = BCM_E_NONE;

    if (ALPMC(u) && ALPMC(u)->_alpm_inited) {
        ALPM_IEG(bcm_esw_alpm_deinit(u));
    }

    ALPM_IEG(bcm_esw_alpm_ctrl_init(u));
    ALPM_IEG(bcm_esw_alpm_tcam_init(u));
    ALPM_IEG(bcm_esw_alpm_hit_init(u));
    ALPM_IEG(bcm_esw_alpm_trace_init(u));

    ALPMC(u)->_alpm_inited = TRUE;

bad:
    bcm_esw_alpm_init_done(u, rv);
    return rv;
}

/*
 * bcm_esw_alpm_insert
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 *
 */
int
bcm_esw_alpm_insert(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE, rv2;
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int                 pkm = ALPM_LPM_PKM(u, lpm_cfg);
    int                 pvt_info = lpm_cfg->defip_alpm_cookie;
    _alpm_cb_t          *acb = NULL;

    alpm_util_ipmask_apply(u, lpm_cfg);
    alpm_trie_cfg_to_pfx(u, lpm_cfg, lpm_cfg->user_data);

    /* Routes reside in TCAM only in below cases:
     *   Global High routes
     *   Global low routes in Mixed mode
     */
    if (ALPM_LPM_DIRECT_RTE(u, lpm_cfg)) {
        /* Init pvt_trie for direct routes */
        acb = ACB_TOP(u);
        if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
            ALPM_IER(alpm_pvt_trie_init(u, vrf_id, pkm));
        }

        /* Insert into TCAM directly and return */
        rv = bcm_esw_alpm_tcam_insert(u, lpm_cfg);
        if (BCM_SUCCESS(rv)) {
            if (!(pvt_info & BCM_ALPM_LPM_LOOKUP_HIT)) {
                VRF_ROUTE_ADD(acb, vrf_id, pkm);
            }
            /* Insert only Direct route pvt trie */
            rv = alpm_pvt_trie_insert(u, lpm_cfg);
            if (rv == BCM_E_EXISTS &&
                (lpm_cfg->defip_flags & BCM_L3_REPLACE)) {
                rv = BCM_E_NONE;
            }
        }
        return(rv);
    }

    /* Route mode and default route validation */
    ALPM_IER(alpm_util_route_type_check(u, lpm_cfg));
    ALPM_IER(alpm_util_def_check(u, lpm_cfg, TRUE));
    acb = ACB_BTM(u);

    /* Find and update the entry */
    if (pvt_info & BCM_ALPM_LOOKUP_HIT) {
        rv = alpm_cb_update(u, acb, lpm_cfg);
        return rv;
    }

    /* entry is not found in database */
    /* Insert the entry into the database, if FULL, split the bucket */
    /* Insert prefix into trie */
    /* Split trie : Insertion into trie results into Split */
    /* Allocate a TCAM entry for PIVOT and bucket and move entries */
    rv = alpm_cb_insert(u, acb, lpm_cfg);
    if (BCM_SUCCESS(rv)) {
        VRF_ROUTE_ADD(acb, vrf_id, pkm);
        lpm_cfg->defip_index =
            alpm_util_ent_phy_idx_get(u, acb, pkm, vrf_id, lpm_cfg->defip_index);
    } else if (rv != BCM_E_FULL) {
        ALPM_ERR(("**ACB(%d).INSERT: Failed %d\n", ACB_IDX(acb), rv));
    }

    rv2 = alpm_cb_sanity(u, pkm, vrf_id);
    rv = (rv == BCM_E_NONE) ? rv2 : rv;

    return(rv);
}

/*
 * bcm_esw_alpm_lookup
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 *
 */
int
bcm_esw_alpm_lookup(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int                 rv = BCM_E_NONE;
    _alpm_cb_t    *acb = NULL;
    _alpm_pvt_node_t    *pvt_node = NULL;
    _alpm_bkt_node_t    *bkt_node = NULL;
    int                 pkm = ALPM_LPM_PKM(u, lpm_cfg);
    int                 vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    alpm_util_ipmask_apply(u, lpm_cfg);

    /* Prepare trie key format in lpm_cfg->user_data for further use */
    alpm_trie_cfg_to_pfx(u, lpm_cfg, lpm_cfg->user_data);

    if (ALPM_LPM_DIRECT_RTE(u, lpm_cfg)) {
        if (!ACB_VRF_INITED(u, ACB_TOP(u), vrf_id, pkm)) {
            ALPM_VERB(("**LOOKUP:VRF %d is not initialized\n", vrf_id));
            return BCM_E_NOT_FOUND;
        }
        rv = bcm_esw_alpm_tcam_match(u, lpm_cfg, &lpm_cfg->defip_index);
        if (BCM_SUCCESS(rv)) {
            lpm_cfg->defip_alpm_cookie = BCM_ALPM_LPM_LOOKUP_HIT;
        }
        return rv;
    }

    acb = ACB_BTM(u);

    /* Check VRF/Key_mode based initialization */
    if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
        ALPM_VERB(("**LOOKUP:VRF %d is not initialized\n", vrf_id));
        return BCM_E_NOT_FOUND;
    }

    /* Find and update the entry */
    rv = alpm_cb_find(u, acb, lpm_cfg, &pvt_node, &bkt_node);
    lpm_cfg->pvt_node = (uint32 *)pvt_node;
    lpm_cfg->bkt_node = (uint32 *)bkt_node;

    if (BCM_SUCCESS(rv)) {
        /* Copy ALPM memory to LPM entry */
        alpm_util_adata_trie_to_cfg(u, &bkt_node->adata, lpm_cfg);
        lpm_cfg->defip_index =
            alpm_util_ent_phy_idx_get(u, acb, pkm, vrf_id, bkt_node->ent_idx);
        lpm_cfg->defip_alpm_cookie = BCM_ALPM_LOOKUP_HIT;
        if (ACB_VRF_DB_TYPE(u, acb, vrf_id, pkm)) {
            lpm_cfg->defip_alpm_cookie |= ALPM_ROUTE_FULL_MD;
        }
        /* Get HIT bit from matched entry */
        if (!ALPM_HIT_SKIP(u)) {
            int idx, ent;
            int hit_val;
            _alpm_tbl_t tbl;
            ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
            idx = ALPM_TAB_IDX_GET(bkt_node->ent_idx);
            tbl = BPC_BKT_TBL(ACB_BKT_VRF_POOL(acb, vrf_id), pkm);

            hit_val = alpm_rte_hit_get(u, vrf_id, tbl, idx, ent);
            lpm_cfg->defip_flags |= (hit_val ? BCM_L3_HIT : 0);
        }
    }

    return(rv);
}

/*
 * bcm_esw_alpm_delete
 *
 */
int
bcm_esw_alpm_delete(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int     rv = BCM_E_NONE;
    _alpm_cb_t *acb;
    _alpm_pvt_node_t *pvt_node;
    _alpm_bkt_node_t *bkt_node;

    uint32  pkm = ALPM_LPM_PKM(u, lpm_cfg);
    uint32  vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    alpm_util_ipmask_apply(u, lpm_cfg);
    /* ALPM prefix */
    alpm_trie_cfg_to_pfx(u, lpm_cfg, lpm_cfg->user_data);

    /* Prefix resides in TCAM directly */
    if (ALPM_LPM_DIRECT_RTE(u, lpm_cfg)) {
        acb = ACB_TOP(u);
        rv = bcm_esw_alpm_tcam_delete(u, lpm_cfg);

        if (BCM_SUCCESS(rv)) {
            /* Delete only pvt trie for direct routes */
            rv = alpm_pvt_trie_delete(u, lpm_cfg);
            VRF_ROUTE_DEC(acb, vrf_id, pkm);
            if (VRF_ROUTE_CNT(acb, vrf_id, pkm) == 0) {
                (void)trie_destroy(ACB_PVT_TRIE(acb, vrf_id, pkm));
                ACB_VRF_INIT_CLEAR(u, acb, vrf_id, pkm);
            }
        }
        return rv;
    }

    ALPM_IER(alpm_util_def_check(u, lpm_cfg, FALSE));

    acb = ACB_BTM(u);
    if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
        ALPM_ERR(("**ACB(%d).DELETE:VRF %d/%d is not initialized\n",
                  ACB_IDX(acb), vrf_id, pkm));
        return BCM_E_NONE;
    }

    pvt_node = (_alpm_pvt_node_t *)lpm_cfg->pvt_node;
    bkt_node = (_alpm_bkt_node_t *)lpm_cfg->bkt_node;

    /* Find and update the entry */
    if ((pvt_node == NULL) || (bkt_node == NULL)) {
        rv = alpm_cb_find(u, acb, lpm_cfg, &pvt_node, &bkt_node);
        ALPM_IER(rv);
        lpm_cfg->pvt_node = pvt_node;
        lpm_cfg->bkt_node = bkt_node;
    }

    /* internally 0 is invalid bucket index */
    rv = alpm_cb_delete(u, acb, lpm_cfg);

    if (BCM_SUCCESS(rv) && VRF_ROUTE_CNT(acb, vrf_id, pkm) == 0) {
        rv = alpm_vrf_deinit(u, vrf_id, pkm);
    }

    return(rv);
}

static int
bcm_esw_alpm_pvt_trav_int_cb(trie_node_t *trie, void *datum)
{
    int rv = BCM_E_NONE;
    _alpm_pvt_trav_t    *trav_st;
    _alpm_pvt_node_t    *pvt_node;

    trav_st = (_alpm_pvt_trav_t *)datum;
    if (trie->type == PAYLOAD) {
        pvt_node = (_alpm_pvt_node_t *)trie;
        rv = trav_st->user_cb(trav_st->acb->unit, trav_st->acb,
                              pvt_node, trav_st->user_data);
    }

    return rv;
}

/* Traverse a given pivot trie-tree
 *
 */
int
bcm_esw_alpm_pvt_traverse(int u, int acb_idx, int vrf_id, int pkm,
                          bcm_esw_alpm_pvt_trav_cb trav_fn, void *user_data)
{
    int rv = BCM_E_NONE;
    _alpm_cb_t        *acb;
    _alpm_pvt_trav_t  trav_st;

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    acb = ACB(u, acb_idx);

    sal_memset(&trav_st, 0, sizeof(trav_st));

    trav_st.acb       = acb;
    trav_st.user_cb   = trav_fn;
    trav_st.user_data = user_data;

    rv = trie_traverse(ACB_PVT_TRIE(acb, vrf_id, pkm), bcm_esw_alpm_pvt_trav_int_cb,
                       (void *)&trav_st, _TRIE_INORDER_TRAVERSE);
    return rv;
}

STATIC int
bcm_esw_alpm_pfx_trav_int_cb(trie_node_t *ptrie, trie_node_t *trie,
                             trie_traverse_states_e_t *state, void *info)
{
    int rv = BCM_E_NONE;
    _alpm_pfx_trav_t    *trav_st;
    _alpm_pfx_node_t    *pfx_node;

    /* Reset state everytime */
    *state = TRIE_TRAVERSE_STATE_NONE;

    trav_st = (_alpm_pfx_trav_t *)info;
    if (trie->type == PAYLOAD) {
        pfx_node = (_alpm_pfx_node_t *)trie;
        rv = trav_st->user_cb(pfx_node, trav_st->user_data);
    }

    return rv;
}

/* Traverse a given prefix trie-tree
 *
 */
int
bcm_esw_alpm_pfx_traverse(int u, int vrf_id, int pkm,
                          bcm_esw_alpm_pfx_trav_cb trav_fn, void *user_data)
{
    _alpm_pfx_trav_t        trav_st;

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trav_st, 0, sizeof(trav_st));

    trav_st.user_cb = trav_fn;
    trav_st.user_data = user_data;

    /* Use delete safe _trie_postorder_traverse2() for update_match */
    trie_traverse2(ALPM_VRF_TRIE(u, vrf_id, pkm), bcm_esw_alpm_pfx_trav_int_cb,
                  (void *)&trav_st, _TRIE_POSTORDER_TRAVERSE);

    return BCM_E_NONE;
}

STATIC int
bcm_esw_alpm_bkt_trav_int_cb(trie_node_t *trie, void *datum)
{
    int rv = BCM_E_NONE;
    _alpm_bkt_trav_t    *trav_st;
    _alpm_bkt_node_t    *bkt_node;

    trav_st = (_alpm_bkt_trav_t *)datum;
    if (trie->type == PAYLOAD) {
        bkt_node = (_alpm_bkt_node_t *)trie;
        rv = trav_st->user_cb(bkt_node, trav_st->user_data);
    }

    return rv;
}

/* Traverse a given bkt trie-tree
 *
 */
int
bcm_esw_alpm_bkt_traverse(trie_t *bkt_trie,
                          bcm_esw_alpm_bkt_trav_cb trav_fn, void *user_data)
{
    _alpm_bkt_trav_t        trav_st;

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trav_st, 0, sizeof(trav_st));

    trav_st.user_cb = trav_fn;
    trav_st.user_data = user_data;

    trie_traverse(bkt_trie, bcm_esw_alpm_bkt_trav_int_cb,
                  (void *)&trav_st, _TRIE_INORDER_TRAVERSE);

    return BCM_E_NONE;
}

/* Get count for given pivot trie-tree */
int
bcm_esw_alpm_pvt_count_get(int u, _alpm_pvt_node_t *pvt_node, int *count)
{
    int rv = BCM_E_NONE;
    if (count && PVT_BKT_TRIE(pvt_node) && PVT_BKT_TRIE(pvt_node)->trie) {
        *count = PVT_BKT_TRIE(pvt_node)->trie->count;
    } else {
        rv = BCM_E_PARAM;
    }

    return rv;
}

/* Trouble-shooting helper functions */

static int
alpm_cb_pvt_dump_cb(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node, void *datum)
{
    int i, j, count = 0;
    int index; /* for next level bkt index */
    int idx, sub_idx;
    _alpm_bkt_info_t *bkt_info;
    char vet_str[ALPM_BPB_MAX*2+2] = {0};
    alpm_pvt_trav_cb_data_t *cb_data;

    cb_data = (alpm_pvt_trav_cb_data_t *)datum;
    (void)bcm_esw_alpm_pvt_count_get(u, pvt_node, &count);

    bkt_info = &PVT_BKT_INFO(pvt_node);
    if (cb_data->bkt_idx == -1 ||
        cb_data->bkt_idx == PVT_BKT_IDX(pvt_node)) {
        ALPM_IER(alpm_util_pvt_idx_get(u, acb, pvt_node, &idx, &sub_idx));
        cli_out("Level%d pvt_idx:%d.%d | bkt_info rofs:%d idx:%d total %d\n",
                acb->acb_idx + 1, idx, sub_idx,
                PVT_ROFS(pvt_node), PVT_BKT_IDX(pvt_node), count);
        for (i = 0; i < ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node)); i++) {
            for (j = 0; j < 16; j++) {
                if (bkt_info->vet_bmp[i] & (1 << j)) {
                    vet_str[j] = '*';
                } else {
                    vet_str[j] = '_';
                }
            }
            index = ALPM_TAB_IDX_GET_BKT_BNK(acb, PVT_ROFS(pvt_node),
                                             PVT_BKT_IDX(pvt_node), i);
            if (bkt_info->bnk_fmt[i] != 0) {
                cli_out("----> Bank %d: FMT%d[%s] index:%5d\n", i,
                        bkt_info->bnk_fmt[i], vet_str, index);
            }
        }
    }

    return 0;
}

void
alpm_cb_pvt_dump(int u, int acb_bmp, int bkt_idx)
{
    int i, vrf_id, pkm;
    _alpm_cb_t *acb;
    alpm_pvt_trav_cb_data_t *cb_data;

    if (ALPMC(u) == NULL) {
        return ;
    }

    cb_data = sal_alloc(sizeof(alpm_pvt_trav_cb_data_t), "cb_data");
    if (cb_data == NULL) {
        return ;
    }

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        acb = ACB(u, i);
        cb_data->unit = u;
        cb_data->acb = acb;
        cb_data->bkt_idx = bkt_idx;
        for (pkm = 0; pkm < ALPM_PKM_CNT; pkm ++) {
            for (vrf_id = 0; vrf_id < ALPM_VRF_ID_CNT(u); vrf_id ++) {
                if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
                    continue;
                }
                cli_out("ALPM CB[%d] Dumping Pivot vrf_id %d, pkm %s\n", i,
                        vrf_id, alpm_util_pkm_str[pkm]);
                (void)bcm_esw_alpm_pvt_traverse(u, i, vrf_id, pkm,
                        alpm_cb_pvt_dump_cb, cb_data);
            }
        }
    }

    sal_free(cb_data);

    return ;
}

static int
alpm_cb_bkt_dump_cb(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node, void *datum)
{
    int rv = BCM_E_NONE;
    int i, j, ent_bit, ent_shift;
    uint8 bnk_fmt;

    _alpm_bkt_info_t    *bkt_info;
    alpm_pvt_trav_cb_data_t *cb_data;

    cb_data = (alpm_pvt_trav_cb_data_t *)datum;
    ent_shift = _shr_popcount(ALPM_BPB_MAX - 1) + 4;
    bkt_info = &PVT_BKT_INFO(pvt_node);
    SHR_BITSET(cb_data->bkt, PVT_BKT_IDX(pvt_node));

    for (i = 0; i < ALPM_BPB_MAX; i++) {
        bnk_fmt = bkt_info->bnk_fmt[i];
        if (BI_BNK_IS_USED(bkt_info, i)) {
            int bnk_idx;
            if (i < PVT_ROFS(pvt_node)) {
                bnk_idx =
                    ((PVT_BKT_IDX(pvt_node) + 1) <<
                    _shr_popcount(ALPM_BPB_MAX - 1) | i);
            } else {
                bnk_idx =
                    (PVT_BKT_IDX(pvt_node) <<
                    _shr_popcount(ALPM_BPB_MAX - 1) | i);
            }
            SHR_BITSET(cb_data->bnk, bnk_idx);
            cb_data->bnkfmt[bnk_idx] = bnk_fmt;
        }
        for (j = 0; j < 16; j++) {
            if (SHR_BITGET(&bkt_info->vet_bmp[i], j)) {
                ent_bit = (i << 4) | j;
                if (i < PVT_ROFS(pvt_node)) {
                    ent_bit = ((PVT_BKT_IDX(pvt_node) + 1) << ent_shift) | ent_bit;
                } else {
                    ent_bit = (PVT_BKT_IDX(pvt_node) << ent_shift) | ent_bit;
                }
                SHR_BITSET(cb_data->ent, ent_bit);
            }
        }
    }

    return rv;
}

static void
alpm_cb_bkt_pool_dump(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                      char *prefix)
{
    int j, k;
    int bnk_cnt, total_bnk_cnt, bkt_cnt;
    SHR_BITDCL *bnk_bmp = BPC_BNK_BMP(bp_conf);

#define BKT_PER_BLK 64

    bkt_cnt = ACB_BKT_CNT(acb);

    cli_out("  %s BANK POOL USAGE\n", prefix);
    cli_out("------------------------------------------------------------"
            "-------------------------------\n");
    for (j = 0; j < bkt_cnt / BKT_PER_BLK; j++) {
        int tmp_bnk = 0;
        total_bnk_cnt = 0;
        cli_out("BKT%04d-%04d [", j * BKT_PER_BLK, (j+1) * BKT_PER_BLK - 1);
        for (k = j * BKT_PER_BLK; k < (j+1) * BKT_PER_BLK; k++) {
            SHR_BITCOUNT_RANGE(bnk_bmp, bnk_cnt,
                k * BPC_BNK_PER_BKT(bp_conf), BPC_BNK_PER_BKT(bp_conf));
            total_bnk_cnt += bnk_cnt;
            if (bnk_cnt >= 16) {
                cli_out("@");
            } else {
                cli_out("%x", bnk_cnt);
            }
        }
        tmp_bnk =
            total_bnk_cnt * 1000 / (BPC_BNK_PER_BKT(bp_conf) * BKT_PER_BLK);
        cli_out("] %2d.%d%%\n", tmp_bnk / 10, tmp_bnk % 10);
    }
}

static void
alpm_cb_bkt_bnk_dump(int u, _alpm_cb_t *acb, _alpm_bkt_pool_conf_t *bp_conf,
                     char *prefix)
{
    int j, k, bnk, pkm, vrf_id;
    int vrf_min, vrf_max;
    int bnk_cnt, total_bnk_cnt;
    int ent_cnt, total_ent_cnt, max_ent_cnt;
    int bkt_cnt;

    alpm_pvt_trav_cb_data_t *cdata = NULL;

#define BKT_PER_BLK 64

    bkt_cnt = ACB_BKT_CNT(acb);

    cdata = alpm_util_alloc(sizeof(*cdata), "bktusage");
    if (cdata == NULL) {
        ALPM_ERR(("Memory out of resource\n"));
        return;
    }
    sal_memset(cdata, 0, sizeof(*cdata));

    if (ACB_BKT_VRF_POOL(acb, ALPM_VRF_ID_GLO(u)) ==
        ACB_BKT_VRF_POOL(acb, 1)) {
        vrf_min = 0;
        vrf_max = ALPM_VRF_ID_GLO(u);
    } else if (bp_conf == ACB_BKT_VRF_POOL(acb, ALPM_VRF_ID_GLO(u))) {
        vrf_min = ALPM_VRF_ID_GLO(u);
        vrf_max = ALPM_VRF_ID_GLO(u);
    } else {
        vrf_min = 0;
        vrf_max = ALPMC(u)->_alpm_max_vrf_id;
    }

    for (pkm = 0; pkm < ALPM_PKM_CNT; pkm ++) {
        for (vrf_id = vrf_min; vrf_id <= vrf_max; vrf_id++) {
            if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
                continue;
            }
            (void)bcm_esw_alpm_pvt_traverse(u, acb->acb_idx, vrf_id, pkm,
                    alpm_cb_bkt_dump_cb, cdata);
        }
    }

    cli_out("  BKT RANGE |[-------------------- banks used per bucket(%7s) -"
            "-----------] AVGBK AVGET\n", prefix);
    cli_out("------------------------------------------------------------"
            "-------------------------------\n");
    for (j = 0; j < bkt_cnt / BKT_PER_BLK; j++) {
        int tmp_bnk, tmp_ent;
        total_bnk_cnt = 0;
        total_ent_cnt = 0;
        max_ent_cnt   = 0;
        cli_out("BKT%04d-%04d [", j * BKT_PER_BLK, (j+1) * BKT_PER_BLK - 1);
        for (k = j * BKT_PER_BLK; k < (j+1) * BKT_PER_BLK; k++) {
            if (SHR_BITGET(cdata->bkt, k)) {
                SHR_BITCOUNT_RANGE(cdata->bnk, bnk_cnt,
                    k << _shr_popcount(ALPM_BPB_MAX - 1), ALPM_BPB_MAX - 1);
                total_bnk_cnt += bnk_cnt;
                if (bnk_cnt >= 16) {
                    cli_out("@");
                } else {
                    cli_out("%x", bnk_cnt);
                }
            } else {
                cli_out("%s", "_");
            }
            for (bnk = 0; bnk < BPC_BNK_PER_BKT(bp_conf); bnk++) {
                int bnk_idx =
                    (k << _shr_popcount(ALPM_BPB_MAX - 1) | bnk);
                SHR_BITCOUNT_RANGE(cdata->ent, ent_cnt, bnk_idx << 4, 16);
                total_ent_cnt += ent_cnt;
                max_ent_cnt += ACB_FMT_ENT_MAX(acb, cdata->bnkfmt[bnk_idx]);
            }
        }
        tmp_bnk =
            total_bnk_cnt * 1000 / (BPC_BNK_PER_BKT(bp_conf) * BKT_PER_BLK);
        tmp_ent = (max_ent_cnt > 0)
                    ? total_ent_cnt * 1000 / max_ent_cnt
                    : 0;
        cli_out("] %2d.%d%% %2d.%d%%\n", tmp_bnk / 10, tmp_bnk % 10,
            tmp_ent / 10, tmp_ent % 10);
    }

    alpm_util_free(cdata);
}

/*
 * 0-8/0-5 : count of occupied banks)
 * _ : free bucket
 * 30% : average bank percentage per 64 bucket
 * 40% : average entry percentage per 64 bucket
 *
 * BKT0000-0063 [32458___________] 30% 40%
 * BKT0064-0127 [_3133___________] 30% 40%
 *
 */
void
alpm_cb_bkt_dump(int u, int acb_bmp)
{
    int i;
    _alpm_cb_t *acb;
    _alpm_bkt_pool_conf_t *gbl_bp = NULL;
    _alpm_bkt_pool_conf_t *prt_bp = NULL;

#define BKT_PER_BLK 64

    if (ALPMC(u) == NULL) {
        return ;
    }

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        cli_out("ALPM CB[%d] Dumping BUCKET usage:\n", i);

        acb     = ACB(u, i);
        gbl_bp  = ACB_BKT_VRF_POOL(acb, ALPM_VRF_ID_GLO(u));
        prt_bp  = ACB_BKT_VRF_POOL(acb, 1);

        alpm_cb_bkt_pool_dump(u, acb, gbl_bp,
                              gbl_bp == prt_bp ? "GLOBAL/PRIVATE" : "GLOBAL");
        alpm_cb_bkt_bnk_dump(u, acb, gbl_bp,
                              gbl_bp == prt_bp ? "GLO/PRI" : "GLOBAL");
        if (gbl_bp != prt_bp) {
            alpm_cb_bkt_pool_dump(u, acb, prt_bp, "PRIVATE");
            alpm_cb_bkt_bnk_dump(u, acb, prt_bp, "PRIVATE");
        }
    }

    return ;
}

static int
alpm_cb_fmt_dump_cb(int u, _alpm_cb_t *acb,
                    _alpm_pvt_node_t *pvt_node, void *datum)
{
    int i;
    _alpm_bkt_info_t *bkt_info;
    uint32 *fmt = (uint32 *)datum;
    uint8 bnk_fmt;

    bkt_info = &PVT_BKT_INFO(pvt_node);
    for (i = 0; i < ALPM_BPB_MAX; i++) {
        bnk_fmt = bkt_info->bnk_fmt[i];
        if (BI_BNK_IS_USED(bkt_info, i)) {
            fmt[bnk_fmt] ++;
        }
    }

    return 0;
}

/*
 * Pivot CB:
 *  FMT 1: 10% (1)
 *  FMT 2:  0%
 *  FMT 3: 83%
 *  FMT 4:
 *  FMT 5:
 *  FMT 6:
 *  FMT 7:
 *  FMT 8:
 *
 * Route CB:
 *  FMT 1: 2O%
 *  FMT 2:
 *  ...
 *  FMT 12:
 *
 */
void
alpm_cb_fmt_dump(int u, int acb_bmp)
{
    int i, j, vrf_id, pkm, fmt_sum;
    _alpm_cb_t *acb;
    uint32 fmt[2][16];

    if (ALPMC(u) == NULL) {
        return ;
    }

    sal_memset(fmt, 0, sizeof(fmt));
    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }

        acb = ACB(u, i);
        cli_out("ALPM CB[%d %s] Dumping FMT distribution:\n", i,
                ACB_HAS_RTE(acb) ? "Route" : "Pivot");
        for (pkm = 0; pkm < ALPM_PKM_CNT; pkm ++) {
            for (vrf_id = 0; vrf_id < ALPM_VRF_ID_CNT(u); vrf_id ++) {
                if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
                    continue;
                }
                (void)bcm_esw_alpm_pvt_traverse(u, i, vrf_id, pkm,
                        alpm_cb_fmt_dump_cb, fmt[i]);
            }
        }

        fmt_sum = 0;
        for (j = 1; j <= ACB_FMT_MAX(acb); j++) {
            fmt_sum += fmt[i][j];
        }

        /* Avoid Floating point exception */
        if (fmt_sum == 0) {
            fmt_sum = 0xdeadbee;
        }

        if (fmt_sum == 0xdeadbee) {
            cli_out("\tAll ZERO\n\n");
            continue;
        }

        for (j = 1; j <= ACB_FMT_MAX(acb); j++) {
            if (fmt[i][j] != 0) {
                cli_out("\tFMT %2d(Len %3d): %2d.%d%% (Count %d)\n", j,
                        ACB_FMT_PFX_LEN(acb, j),
                        fmt[i][j] * 100 / fmt_sum,
                        (fmt[i][j] * 1000 / fmt_sum) % 10,
                        fmt[i][j]);
            }
        }
    }

    return ;
}

static int
alpm_cb_pvtlen_dump_cb(int u, _alpm_cb_t *acb,
                       _alpm_pvt_node_t *pvt_node, void *datum)
{
    uint32 *pvt_len = (uint32 *)datum;

    if (pvt_node && PVT_KEY_LEN(pvt_node) <= 128) {
        pvt_len[PVT_KEY_LEN(pvt_node)]++;
    }

    return 0;
}

/*
 * Pivot CB:
 *  Pivot Len 0: 10% (1)
 *  ......
 *  Pivot Len 128: 20% (2)
 *
 * Route CB:
 *  ......
 *
 */
void
alpm_cb_pvtlen_dump(int u, int acb_bmp)
{
    int i, j, vrf_id, pkm, pvt_len_sum;
    _alpm_cb_t *acb;
    uint32 pvt_len[2][129];

    if (ALPMC(u) == NULL) {
        return ;
    }

    sal_memset(pvt_len, 0, sizeof(pvt_len));
    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }

        acb = ACB(u, i);
        cli_out("ALPM CB[%d %s] Dumping Pivot Length distribution:\n", i,
                ACB_HAS_RTE(acb) ? "Route" : "Pivot");
        for (pkm = 0; pkm < ALPM_PKM_CNT; pkm ++) {
            for (vrf_id = 0; vrf_id < ALPM_VRF_ID_CNT(u); vrf_id ++) {
                if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
                    continue;
                }
                (void)bcm_esw_alpm_pvt_traverse(u, i, vrf_id, pkm,
                        alpm_cb_pvtlen_dump_cb, pvt_len[i]);
            }
        }

        pvt_len_sum = 0;
        for (j = 0; j <= 128; j++) {
            pvt_len_sum += pvt_len[i][j];
        }

        /* Avoid Floating point exception */
        if (pvt_len_sum == 0) {
            pvt_len_sum = 0xdeadbee;
        }

        if (pvt_len_sum == 0xdeadbee) {
            cli_out("\tAll ZERO\n\n");
            continue;
        }

        for (j = 0; j <= 128; j++) {
            if (pvt_len[i][j] != 0) {
                cli_out("\tPVT_LEN %3d: %2d.%d%% (Count %d)\n", j,
                        pvt_len[i][j] * 100 / pvt_len_sum,
                        (pvt_len[i][j] * 1000 / pvt_len_sum) % 10,
                        pvt_len[i][j]);
            }
        }
    }

    return ;
}

/* BCM.0>alpm stat show <1|2|3> */
void
alpm_cb_stat_dump(int u, int acb_bmp)
{
    int i, j, p, pkm;
    uint32 alloc_cnt, free_cnt, dbg_bmp, dbg_on = 0;
    int free_tcam, used_tcam, tot_tcam;
    _alpm_cb_t *acb;
    _alpm_bkt_pool_conf_t *bp_conf = NULL;

#define _DBG_BMP_STR_CNT    10
    char *dbg_bmp_str[_DBG_BMP_STR_CNT] = {
        "PvtSanity",
        "PvtSanityLen",
        "PvtDefrag",
        "PvtSanityNode",
        "PvtSanityCount",
        "WriteCacheOnly",
        "BnkConf",
        "PvtUpdateSkip",
        "HitBit",
        ""
    };

    if (ALPMC(u) == NULL) {
        return ;
    }

    dbg_bmp = ALPMC(u)->_alpm_dbg_bmp;

    cli_out("ALPM debug:\n");
    for (i = 0; i < _DBG_BMP_STR_CNT; i++) {
        if (dbg_bmp & (1 << i)) {
            cli_out("\tbit %d %s\n", i, dbg_bmp_str[i]);
            dbg_on = 1;
        }
    }
    if (!dbg_on) {
        cli_out("\toff\n");
    }

    alpm_util_mem_stat_get(&alloc_cnt, &free_cnt);
    cli_out("ALPM Host mem: alloc %8d free %8d\n", alloc_cnt, free_cnt);
    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        cli_out("ALPM CB[%d] Statistics:\n\n", i);
        acb = ACB(u, i);

        if (ACB_HAS_TCAM(acb)) {
            for (pkm = ALPM_PKM_32B; pkm <= ALPM_PKM_128; pkm++) {
                bcm_esw_alpm_tcam_state_free_get(u, pkm, &free_tcam, &used_tcam);
                tot_tcam = used_tcam + free_tcam;
                if (tot_tcam == 0) {
                    tot_tcam = 1; /* avoid divide by 0 */
                }
                cli_out("\tTCAM %4s: %4d (used), %4d (free), Usage: %d.%d%%\n",
                        alpm_util_pkm_str[pkm],
                        used_tcam, free_tcam,
                        used_tcam * 100 / tot_tcam,
                        (used_tcam * 1000 / tot_tcam) % 10);
            }
            cli_out("\n");
        }

#define _BKT_POOL_USAGE_PRINT(acb_idx, pid, bp_conf)                        \
        cli_out("\tBucket Pool %d Usage(Level%d): %6d/%6d = %d.%d%%\n",     \
                pid, acb_idx+2,                                             \
                BPC_BNK_USED(bp_conf),                                      \
                BPC_BNK_CNT(bp_conf),                                       \
                BPC_BNK_USED(bp_conf) * 100 / BPC_BNK_CNT(bp_conf),         \
                (BPC_BNK_USED(bp_conf) * 1000 / BPC_BNK_CNT(bp_conf)) % 10);
        for (p = 0; p < ALPM_BKT_PID_CNT; p++) {
            bp_conf = ACB_BKT_POOL(acb, p);
            if (p > 0 && bp_conf == ACB_BKT_POOL(acb, p-1)) {
                continue;
            }
            _BKT_POOL_USAGE_PRINT(acb->acb_idx, p, bp_conf)
        }

        cli_out("\tMemory Stats: \n");
        for (j = 0; j < alpmTblCount; j++) {
            if (acb->acb_cnt.c_mem[j][0] != 0 || acb->acb_cnt.c_mem[j][1] != 0) {
                cli_out("\t  %22s: RD %10d WR %10d\n", alpm_tbl_str[j],
                        acb->acb_cnt.c_mem[j][0],
                        acb->acb_cnt.c_mem[j][1]);
            }
        }
        cli_out("\tDefrag : %8d\n", acb->acb_cnt.c_defrag);
        cli_out("\tMerge  : %8d\n", acb->acb_cnt.c_merge);
        cli_out("\tExpand : %8d\n", acb->acb_cnt.c_expand);
        cli_out("\tSplit  : %8d\n", acb->acb_cnt.c_split);
        cli_out("\tRipple : %8d\n", acb->acb_cnt.c_ripple);
        cli_out("\tBNKshrk: %8d\n", acb->acb_cnt.c_bnkshrk);
        cli_out("\tBNKfree: %8d\n", acb->acb_cnt.c_bnkfree);

        cli_out("\n");
    }
}

/* BCM.0>alpm config show <1|2|3> */
void
alpm_cb_conf_dump(int u, int acb_bmp)
{
    int i, pid, fmt;
    _alpm_cb_t *acb;

    if (ALPMC(u) == NULL) {
        return ;
    }

    for (i = 0; i < ACB_CNT(u); i++) {
        if (!(acb_bmp & (1 << i))) {
            continue;
        }
        cli_out("ALPM CB[%d] Bank Config:\n", i);
        acb = ACB(u, i);

        cli_out("\t    bkt_bits: %d\n", ACB_BKT_BITS(acb));
        cli_out("\t     bkt_cnt: %d\n", ACB_BKT_CNT(acb));
        cli_out("\tfmt_type_max: %d\n", ACB_FMT_MAX(acb));

        for (pid = 0; pid < ALPM_BKT_PID_CNT; pid++) {
            _alpm_bkt_pool_conf_t *bp_conf;

            if (pid > 0 &&
                ACB_BKT_POOL(acb, pid-1) == ACB_BKT_POOL(acb, pid)) {
                continue;
            }

            bp_conf = ACB_BKT_POOL(acb, pid);
            cli_out("\t  bnk_pool.%d: %p\n", pid, BPC_BNK_BMP(bp_conf));
            cli_out("\t bnk_total.%d: %d\n", pid, BPC_BNK_CNT(bp_conf));
            cli_out("\t  bnk_used.%d: %d\n", pid, BPC_BNK_USED(bp_conf));
            cli_out("\t  bnk_pbkt.%d: %d\n", pid, BPC_BNK_PER_BKT(bp_conf));
            cli_out("\t wrap_arnd.%d: %d\n", pid, BPC_BKT_WRA(bp_conf));
            cli_out("\t fixed_fmt.%d: %d\n", pid, BPC_BKT_FIXED_FMT(bp_conf));
        }

        cli_out("\t----------------------------------------------------\n");
        cli_out("\tFV |");
        for (fmt = 0; fmt <= ACB_FMT_MAX(acb); fmt++) {
            cli_out("%3d|", fmt);
        }
        cli_out("\n");
        cli_out("\t----------------------------------------------------\n");
        cli_out("\tCAP|");
        for (fmt = 0; fmt <= ACB_FMT_MAX(acb); fmt++) {
            cli_out("%3d|", ACB_FMT_ENT_MAX(acb, fmt));
        }
        cli_out("\n");
        cli_out("\t----------------------------------------------------\n");
        cli_out("\tPfL|");
        for (fmt = 0; fmt <= ACB_FMT_MAX(acb); fmt++) {
            cli_out("%3d|", ACB_FMT_PFX_LEN(acb, fmt));
        }
        cli_out("\n");
        cli_out("\t----------------------------------------------------\n");
    }
}

int alpm_table_dump(int u, soc_mem_t mem, int copyno, int index,
                    int fmt, int ent_idx, int flags)
{
    return ALPM_DRV(u)->alpm_table_dump(u, mem, copyno, index, fmt, ent_idx, flags);
}


static int
alpm_cb_ent_pfx_len_get(int u, _alpm_cb_t *acb,
                        _alpm_pvt_node_t *pvt_node, int ent_idx)
{
    return ALPM_DRV(u)->alpm_ent_pfx_len_get(u, acb, pvt_node, ent_idx);
}

int
alpm_cb_pvt_sanity_cb(int u, _alpm_cb_t *acb,
                      _alpm_pvt_node_t *pvt_node, void *datum)
{
    int i, rv = BCM_E_NONE;
    int bkt_node_cnt, bkt_info_cnt;
    int err = 0;
    int end_bnk, bnkpb, bnk_cnt;

    char        *pfx_str = datum;
    trie_t      *bkt_trie = NULL;
    _alpm_bkt_info_t *bkt_info;
    _alpm_bkt_pfx_arr_t *pfx_arr = NULL;
    _alpm_bkt_node_t *bkt_node;

    bkt_info = &PVT_BKT_INFO(pvt_node);
    bkt_trie = PVT_BKT_TRIE(pvt_node);
    bnkpb   = ACB_BNK_PER_BKT(acb, PVT_BKT_VRF(pvt_node));

    /* Check Global bank usage and pvt_ptr */
    if ((sal_strcmp(pfx_str, "CBInsert") == 0 ||
         sal_strcmp(pfx_str, "CBDelete") == 0 ||
         sal_strcmp(pfx_str, "SanityAll") == 0) &&
        ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_BNK_CONF) {
        int tab_idx;
        SHR_BITDCL *bnk_bmp;

        /* Private VRFs */
        bnk_bmp = ACB_BNK_BMP(acb, 1);
        bnk_cnt = ACB_BNK_CNT(acb, 1);
bnk_bmp_check:
        for (i = 0; i < bnk_cnt; i++) {
            if (SHR_BITGET(bnk_bmp, i)) {
                tab_idx = ALPM_TAB_IDX_GET_BKT_BNK(acb, 0, i/bnkpb, i%bnkpb);
                /* If bank is occupied but pvt_ptr is not set, report error
                 * Skip examing bkt 0
                 */
                if ((i/bnkpb) != 0 &&
                    ACB_PVT_PTR(acb, pvt_node, tab_idx) == NULL) {
                    cli_out("\n[%s]PVT-SANITY(CB%d): bkt %d bnk %d was used "
                            "by NULL pvt_ptr\n", pfx_str, acb->acb_idx,
                            i/bnkpb, i%bnkpb);
                    err++;
                    goto bad;
                }
            }
        }

        if (bnk_bmp != ACB_BNK_BMP(acb, ALPM_VRF_ID_GLO(u))) {
            bnk_bmp = ACB_BNK_BMP(acb, ALPM_VRF_ID_GLO(u));
            bnk_cnt = ACB_BNK_CNT(acb, ALPM_VRF_ID_GLO(u));
            goto bnk_bmp_check;
        }
    }

    /* Check if used banks are coutinuous */
    if (!ACB_BKT_FIXED_FMT(acb, 1)) {
        end_bnk = -1;
        for (i = PVT_ROFS(pvt_node); i < PVT_ROFS(pvt_node) + bnkpb; i++) {
            if (bkt_info->bnk_fmt[i % bnkpb] == 0) {
                end_bnk = i;
            } else if (end_bnk != -1) {
                /* Once end_bnk is settled, the rest bnks FMT should be 0 */
                cli_out("[%s]PVT-SANITY(CB%d): Used banks are not cont. rofs %d: ",
                        pfx_str, acb->acb_idx, PVT_ROFS(pvt_node));
                for (i = 0; i < bnkpb; i++) {
                    if (PVT_ROFS(pvt_node) == i) {
                        cli_out("^");
                    }
                    cli_out("%d ", bkt_info->bnk_fmt[i]);
                }
                cli_out("\n");
                err++;
                goto bad;
            }
        }
    }

    /* trie-tree count & vet_bmp count,
     * SHOULD NOT be used in the middle of process, e.g. bucket split
     */
    if ((sal_strcmp(pfx_str, "CBInsert") == 0 ||
         sal_strcmp(pfx_str, "CBDelete") == 0 ||
         sal_strcmp(pfx_str, "SanityAll") == 0 ||
         sal_strcmp(pfx_str, "SplitOpvt") == 0 ||
         sal_strcmp(pfx_str, "SplitNpvt") == 0) &&
        bkt_trie->trie &&
        ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_COUNT) {
        bkt_node_cnt = bkt_trie->trie->count;
        bkt_info_cnt = 0;
        for (i = 0; i < ALPM_BPB_MAX; i++) {
            uint32 vet_bmp;
            if (bkt_info->bnk_fmt[i] == 0) {
                continue;
            }

            vet_bmp = (uint32)bkt_info->vet_bmp[i];
            bkt_info_cnt += _shr_popcount(vet_bmp);
        }

        if (bkt_node_cnt != bkt_info_cnt) {
            cli_out("\n[%s]PVT-SANITY(CB%d): entry cnt mismatch in trie %d, bkt_info %d\n",
                    pfx_str, acb->acb_idx, bkt_node_cnt, bkt_info_cnt);
            err++;
            goto bad;
        }
    }

    /* bkt_info & ent_idx, tab_idx & pvt_node pointer
     * SW prefix length & HW prefix length
     *
     * Contruct:
     * - ALPM_IDX_MAKE
     * - ALPM_TAB_IDX_GET(ent_idx)
     *
     * Resolve:
     * - ALPM_IDX_TO_ENT
     * - ALPM_IDX_TO_BNK
     * - ALPM_IDX_TO_BKT
     */
    ALPM_ALLOC_EG(pfx_arr, sizeof(*pfx_arr), "pfx_arr");
    rv = trie_traverse(bkt_trie, alpm_trie_pfx_cb, pfx_arr,
                       _TRIE_INORDER_TRAVERSE);
    ALPM_IEG(rv);

    for (i = 0; i < pfx_arr->pfx_cnt; i++) {
        uint32 ent, bnk, bkt;
        uint32 tab_idx;
        uint32 vet_bmp;
        int sw_len, hw_len;

        bkt_node = pfx_arr->pfx[i];

        /* SW Prefix Length & HW Prefix Length check */
        if (ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY_LEN) {
            sw_len = bkt_node->key_len;
            hw_len = alpm_cb_ent_pfx_len_get(u, acb, pvt_node,
                                             bkt_node->ent_idx);
            if (!ACB_BKT_FIXED_FMT(acb, 1)) {
                hw_len += PVT_KEY_LEN(pvt_node);
            }
            if (sw_len != hw_len) {
                cli_out("\n[%s]PVT-SANITY(CB%d): Prefix Length mismatch sw %d "
                        "hw %d ent_idx 0x%x\n", pfx_str, acb->acb_idx, sw_len, hw_len,
                        bkt_node->ent_idx);
                err++;
                goto bad;
            }
        }

        ent = ALPM_IDX_TO_ENT(bkt_node->ent_idx);
        bnk = ALPM_IDX_TO_BNK(acb, bkt_node->ent_idx);
        bkt = ALPM_IDX_TO_BKT(acb, bkt_node->ent_idx);
        if (bnk < PVT_ROFS(pvt_node)) {
            bkt -= 1;
        }

        /* Bucket mismatch */
        if (BI_BKT_IDX(bkt_info) != bkt) {
            cli_out("\n[%s]PVT-SANITY(CB%d): Bucket idx mismatch in bkt-node %d, "
                    "pvt_node %d\n", pfx_str, acb->acb_idx, bkt, BI_BKT_IDX(bkt_info));
            err++;
            goto bad;
        }

        /* Ent, bnk mismatch */
        vet_bmp = bkt_info->vet_bmp[bnk];
        if (bkt_info->bnk_fmt[bnk] == 0 ||
            !SHR_BITGET(&vet_bmp, ent)) {
            cli_out("\n[%s]PVT-SANITY(CB%d): (ent,bnk) mismatch in trie-node (%d,%d)"
                    ", but related valid in bkt_info is (%d, bnk%d-fmt%d)\n",
                    pfx_str, acb->acb_idx, ent, bnk,
                    SHR_BITGET(&vet_bmp, ent),
                    bnk, bkt_info->bnk_fmt[bnk]);
            err++;
            goto bad;
        }

        /* tab_idx and pvt_node pointer */
        tab_idx = ALPM_TAB_IDX_GET(bkt_node->ent_idx);
        if (tab_idx >= ACB_BNK_CNT(acb, PVT_BKT_VRF(pvt_node))) {
            cli_out("\n[%s]PVT-SANITY(CB%d): Invalid tab_idx value %d "
                    "(ent %d, bnk %d, bkt %d)\n", pfx_str, acb->acb_idx,
                    tab_idx, ent, bnk, bkt);
            err++;
            goto bad;
        } else if (ACB_PVT_PTR(acb, pvt_node, tab_idx) != pvt_node) {
            _alpm_pvt_node_t *acb_pvt_node;
            acb_pvt_node = (_alpm_pvt_node_t *)ACB_PVT_PTR(acb, pvt_node, tab_idx);

            cli_out("\n[%s]PVT-SANITY(CB%d): pvt_ptr of tab_idx %d mismatch "
                    "in acb %p, trie-tree %p\n", pfx_str, acb->acb_idx, tab_idx,
                    acb_pvt_node, pvt_node);
            if (acb_pvt_node != NULL) {
                cli_out("\t- acb %p key_len %d, bkt_info(rofs %d, bkt %d)\n",
                        acb_pvt_node, PVT_KEY_LEN(acb_pvt_node),
                        PVT_ROFS(acb_pvt_node),
                        PVT_BKT_IDX(acb_pvt_node));
                cli_out("\t-trie %p key_len %d, bkt_info(rofs %d, bkt %d)\n",
                        pvt_node, PVT_KEY_LEN(pvt_node),
                        PVT_ROFS(pvt_node),
                        PVT_BKT_IDX(pvt_node));
            }
            err++;
            goto bad;
        }
    }

bad:
    if (err > 0) {
        rv = BCM_E_FAIL;
    }

    if (pfx_arr != NULL) {
        alpm_util_free(pfx_arr);
    }

    return rv;
}

/*
 * Check software status
 *
 *  - trie-tree count & vet_bmp count
 *  - bkt_info & ent_idx
 */
int
alpm_cb_sanity(int u, int pkm, int vrf_id)
{
    int rv = BCM_E_NONE;
    int acb_idx;
    _alpm_cb_t *acb = NULL;

    if (!(ALPMC(u)->_alpm_dbg_bmp & _ALPM_DBG_PVT_SANITY)) {
        return rv;
    }

    for (acb_idx = 0; acb_idx < ACB_CNT(u); acb_idx++) {
        acb = ACB(u, acb_idx);
        if (!ACB_VRF_INITED(u, acb, vrf_id, pkm)) {
            continue;
        }

        rv = bcm_esw_alpm_pvt_traverse(u, acb->acb_idx, vrf_id, pkm,
                                       alpm_cb_pvt_sanity_cb, "SanityAll");
        BCM_IF_ERROR_RETURN(rv);
    }

    return rv;
}

#endif /* ALPM_ENABLE */

