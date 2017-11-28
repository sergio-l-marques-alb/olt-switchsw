/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $Id: alpm.c$
 * File:    alpm_tcam.c
 * Purpose: ALPM TCAM management. device independent implementation.
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

#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>

#include <bcm/l3.h>

#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/alpm.h>
#include <bcm_int/esw/alpm_util.h>

/* Internal function declaration */

typedef struct _tcam_state_s {
    int start;  /* start index for this prefix length */
    int end;    /* End index for this prefix length */
    int prev;   /* Prev (Lo to Hi) prefix length with non zero entry count */
    int next;   /* Next (Hi to Lo) prefix length with non zero entry count */
    int vent;   /* valid entries */
    int fent;   /* free entries */
} _tcam_state_t;

typedef struct _tcam_tab_ctrl_s {
    _tcam_state_t  *state;

    /* PFX management */
    int             pfx_min_v6;
    int             pfx_max_vrf_ents;
    int             pfx_max_ents;
    int             pfx_max_idx;
    int             pfx_max_vrf_idx;
} _tcam_tab_ctrl_t;

typedef struct _tcam_ctrl_s {
    _tcam_tab_ctrl_t *tcam[ALPM_PKM_CNT];
} _tcam_ctrl_t;

_tcam_ctrl_t *_tcam_control[SOC_MAX_NUM_DEVICES];

#define _PFX_MIN_V6                     (33)
#define _PFX_MAX_VRF_ENTS               (64 + 32 + 2 + 1)
#define _PFX_MAX_VRF_IDX                (_PFX_MAX_VRF_ENTS - 1)
#define _PFX_MAX_ENTS                   (3 * _PFX_MAX_VRF_ENTS)
#define _PFX_MAX_IDX                    (_PFX_MAX_ENTS - 1)

#define _PFX_MIN_V6_128                 (0)
#define _PFX_MAX_VRF_ENTS_128           (128 + 2 + 1)
#define _PFX_MAX_VRF_IDX_128            (_PFX_MAX_VRF_ENTS_128 - 1)
#define _PFX_MAX_ENTS_128               (3 * _PFX_MAX_VRF_ENTS_128)
#define _PFX_MAX_IDX_128                (_PFX_MAX_ENTS_128 - 1)

#define _PFX_IS_PKM_64B(u, pfx)   ((pfx) % _PFX_MAX_VRF_ENTS >= _PFX_MIN_V6 && \
                                   (pfx) != _PFX_MAX_VRF_IDX)

#define TCAMC(u)                         (_tcam_control[u])
#define _TCAM_INIT_CHECK(u)              (TCAMC(u) != NULL)
#define _TCAM_PKM_ATTACHED(u, pkm)       (TCAMC(u)->tcam[pkm] != NULL)
#define _TCAM_STATE(u, pkm)              (TCAMC(u)->tcam[pkm]->state)
#define _TCAM_STATE_START(u, pkm, pfx)   (TCAMC(u)->tcam[pkm]->state[pfx].start)
#define _TCAM_STATE_END(u, pkm, pfx)     (TCAMC(u)->tcam[pkm]->state[pfx].end)
#define _TCAM_STATE_PREV(u, pkm, pfx)    (TCAMC(u)->tcam[pkm]->state[pfx].prev)
#define _TCAM_STATE_NEXT(u, pkm, pfx)    (TCAMC(u)->tcam[pkm]->state[pfx].next)
#define _TCAM_STATE_VENT(u, pkm, pfx)    (TCAMC(u)->tcam[pkm]->state[pfx].vent)
#define _TCAM_STATE_FENT(u, pkm, pfx)    (TCAMC(u)->tcam[pkm]->state[pfx].fent)

#define _TCAM_PFX_MIN_V6(u, pkm)         (TCAMC(u)->tcam[pkm]->pfx_min_v6)
#define _TCAM_PFX_MAX_VRF_ENTS(u, pkm)   (TCAMC(u)->tcam[pkm]->pfx_max_vrf_ents)
#define _TCAM_PFX_MAX_ENTS(u, pkm)       (TCAMC(u)->tcam[pkm]->pfx_max_ents)
#define _TCAM_PFX_MAX_IDX(u, pkm)        (TCAMC(u)->tcam[pkm]->pfx_max_idx)
#define _TCAM_PFX_MAX_VRF_IDX(u, pkm)    (TCAMC(u)->tcam[pkm]->pfx_max_vrf_idx)

_tcam_tab_ctrl_t _tcam_defip_ctrl = {
    NULL,
    _PFX_MIN_V6,
    _PFX_MAX_VRF_ENTS,
    _PFX_MAX_ENTS,
    _PFX_MAX_IDX,
    _PFX_MAX_VRF_IDX
};

_tcam_tab_ctrl_t _tcam_defip128_ctrl = {
    NULL,
    _PFX_MIN_V6_128,
    _PFX_MAX_VRF_ENTS_128,
    _PFX_MAX_ENTS_128,
    _PFX_MAX_IDX_128,
    _PFX_MAX_VRF_IDX_128
};

STATIC int
_tcam_trie_lookup(int u, _bcm_defip_cfg_t *lpm_cfg, int *key_index)
{
    int             rv = BCM_E_NOT_FOUND;
    trie_t          *pvt_trie;
    _alpm_pvt_node_t *pvt_node;

    int             pkm = ALPM_LPM_PKM(u, lpm_cfg);
    int             vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u), vrf_id, pkm);
    if (pvt_trie != NULL) {
        rv = trie_search(pvt_trie, lpm_cfg->user_data, lpm_cfg->defip_sub_len,
                         (trie_node_t **)&pvt_node);
        if (BCM_SUCCESS(rv)) {
            *key_index = PVT_IDX(pvt_node);
        }
    }
    return rv;
}

STATIC int
_tcam_trie_update(int u, uint32 *key, int len, int vrf_id, int pkm, int key_idx)
{
    int                 rv = BCM_E_NOT_FOUND;
    trie_t              *pvt_trie;
    _alpm_pvt_node_t    *pvt_node;

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u), vrf_id, pkm);
    if (pvt_trie != NULL) {
        rv = trie_search(pvt_trie, key, len, (trie_node_t **)&pvt_node);
        if (BCM_SUCCESS(rv)) {
            PVT_IDX(pvt_node) = key_idx;
        }
    }

    return rv;
}

int
_tcam_trie_update_by_cfg(int u, _bcm_defip_cfg_t *lpm_cfg, int key_idx)
{
    int rv;
    int pkm = ALPM_LPM_PKM(u, lpm_cfg);
    int vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    rv = _tcam_trie_update(u, lpm_cfg->user_data, lpm_cfg->defip_sub_len,
                           vrf_id, pkm, key_idx);

    return rv;
}

int
tcam_entry_valid(int u, int pkm, void *e, int sub_idx)
{
    int vld = 0;

    vld = ALPM_DRV(u)->tcam_entry_valid(u, pkm, e, sub_idx);

    return vld;
}

STATIC int
_tcam_entry_valid_set(int u, int pkm, void *e, int sub_idx, int val)
{
    int rv = BCM_E_NONE;

    rv = ALPM_DRV(u)->tcam_entry_valid_set(u, pkm, e, sub_idx, val);

    return rv;
}

STATIC int
_tcam_trie_update_by_ent(int u, int pkm, void *e, int hw_idx)
{
    int         i, rv = BCM_E_NONE;
    uint32      pfx[5];
    int         pfx_len, vrf_id;

    if (pkm == ALPM_PKM_32B) {
        /* For IPv4(32B packing mode), check and update both 2 entries */
        for (i = 0; i < 2; i++) {
            if (tcam_entry_valid(u, pkm, e, i)) {
                ALPM_IER(tcam_entry_vrf_id_get(u, pkm, e, i, &vrf_id));
                ALPM_IER(tcam_entry_pfx_len_get(u, pkm, e, i, &pfx_len));
                ALPM_IER(alpm_trie_ent_to_pfx(u, pkm, e, i, pfx_len, pfx));
                rv = _tcam_trie_update(u, pfx, pfx_len, vrf_id, pkm,
                                       hw_idx << 1 | i);
            }
        }
    } else {
        if (tcam_entry_valid(u, pkm, e, 0)) {
            ALPM_IER(tcam_entry_vrf_id_get(u, pkm, e, 0, &vrf_id));
            ALPM_IER(tcam_entry_pfx_len_get(u, pkm, e, 0, &pfx_len));
            ALPM_IER(alpm_trie_ent_to_pfx(u, pkm, e, 0, pfx_len, pfx));
            rv = _tcam_trie_update(u, pfx, pfx_len, vrf_id, pkm, hw_idx);
        }
    }

    return rv;
}

/**********************************************
* TCAM Management functions *
***********************************************/

/*
 * TCAM based PIVOT implementation. Each table entry can hold two IPV4 PIVOTs or
 * one IPV6 PIVOT entry. VRF independent routes placed at the beginning or
 * at the end of table based on application provided entry vrf id
 * (BCM_L3_VRF_OVERRIDE/BCM_L3_VRF_GLOBAL).
 *
 *              _PFX_MAX_INDEX
 * lpm_prefix_index[98].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    0                      ==
 * lpm_prefix_index[98].end   ---> ===============================
 *
 * lpm_prefix_index[97].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV6  Prefix Len = 64  ==
 * lpm_prefix_index[97].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[x].begin --->  ===============================
 *                                 ==                           ==
 *                                 ==                           ==
 * lpm_prefix_index[x].end   --->  ===============================
 *
 *
 *              _PFX_MIN_V6
 * lpm_prefix_index[33].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV6  Prefix Len = 0   ==
 * lpm_prefix_index[33].end   ---> ===============================
 *
 *
 * lpm_prefix_index[32].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 32  ==
 * lpm_prefix_index[32].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[0].begin --->  ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 0   ==
 * lpm_prefix_index[0].end   --->  ===============================
 *
 *
 */

int
tcam_entry_vrf_id_get(int u, int pkm, void *e, int sub_idx, int *vrf_id)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_vrf_id_get(u, pkm, e, sub_idx, vrf_id);

    return rv;
}

int
tcam_entry_pfx_len_get(int u, int pkm, void *e, int sub_idx, int *pfx_len)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_pfx_len_get(u, pkm, e, sub_idx, pfx_len);

    return rv;
}

/*
 * x | y
 * --------------------
 * 0 | 0 : entry0_to_0
 * 0 | 1 : entry0_to_1
 * 1 | 0 : entry1_to_0
 * 1 | 1 : entry1_to_1
 * src and dst can be same
 */
STATIC int
_tcam_entry_x_to_y(int u, int pkm, void *src, void *dst,
                   int copy_hit, int x, int y)
{
    int         rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_entry_x_to_y(u, pkm, src, dst, copy_hit, x, y);

    return rv;
}

STATIC int
_tcam_entry_from_cfg(int u, _bcm_defip_cfg_t *lpm_cfg, void *e, int x)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_from_cfg(u, lpm_cfg, e, x);
    return rv;
}

int
tcam_entry_to_cfg(int u, int pkm, void *e, int x, _bcm_defip_cfg_t *lpm_cfg)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_to_cfg(u, pkm, e, x, lpm_cfg);
    return rv;
}

int
tcam_entry_read(int u, int pkm, void *e, int index, int s_index)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u);
    rv = ALPM_DRV(u)->mem_entry_read(u, acb, acb->pvt_tbl[pkm], index, e, FALSE);

    return rv;
}

int
tcam_entry_read_no_cache(int u, int pkm, void *e, int index, int s_index)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u);
    rv = ALPM_DRV(u)->mem_entry_read(u, acb, acb->pvt_tbl[pkm], index, e, TRUE);

    return rv;
}

STATIC int
_tcam_entry_write(int u, int pkm, void *e, int index, int s_index)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u);
    rv = ALPM_DRV(u)->mem_entry_write(u, acb, NULL, acb->pvt_tbl[pkm], index, e);

    return rv;
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
int
tcam_entry_to_key(int u, int pkm, void *e, int sub_idx, uint32 *key)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_to_key(u, pkm, e, sub_idx, key);

    return rv;
}

STATIC int
_tcam_cfg_write(int u, _bcm_defip_cfg_t *lpm_cfg, int index, int s_index, int update)
{
    int         rv = BCM_E_NONE;
    int         hw_idx;
    int         pkm = ALPM_LPM_PKM(u, lpm_cfg);
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];

    hw_idx = (pkm == ALPM_PKM_32B) ? index >> 1 : index;

    /* Entry already present. Update the entry */
    if (pkm == ALPM_PKM_32B) {
        if (update) {
            ALPM_IER(tcam_entry_read(u, pkm, e, hw_idx, s_index));
        } else {
            sal_memset(e, 0, sizeof(e));
        }
        /* Need to clear entry if both entry0 & 1 are invalid? */
        rv = _tcam_entry_from_cfg(u, lpm_cfg, e, index & 1);
    } else {
        sal_memset(e, 0, sizeof(e));
        rv = _tcam_entry_from_cfg(u, lpm_cfg, e, 0);
    }

    if (BCM_SUCCESS(rv)) {
        rv = _tcam_entry_write(u, pkm, e, hw_idx, s_index);
    }

    return rv;
}

STATIC void
_tcam_state_dump(int u, int pkm)
{
    int i;
    int max_pfx_len;

    if (!_TCAM_PKM_ATTACHED(u, pkm)) {
        return;
    }

    max_pfx_len = _TCAM_PFX_MAX_IDX(u, pkm);

#ifndef BCM_WARM_BOOT_SUPPORT_SW_DUMP
    if (!bsl_check(bslLayerSoc, bslSourceAlpm, bslSeverityVerbose, u)) {
        return;
    }
#endif

    for (i = max_pfx_len; i >= 0 ; i--) {
        if ((i != _TCAM_PFX_MAX_IDX(u, pkm)) &&
            (_TCAM_STATE_START(u, pkm, i) == -1)) {
            continue;
        }

#ifdef ALPM_WARM_BOOT_DEBUG
        ALPM_ERR(("**PFX = %d P = %d N = %d START = %d END = %d "
                  "VENT = %d FENT = %d\n", i,
                  _TCAM_STATE_PREV(u, pkm, i),
                  _TCAM_STATE_NEXT(u, pkm, i),
                  _TCAM_STATE_START(u, pkm, i),
                  _TCAM_STATE_END(u, pkm, i),
                  _TCAM_STATE_VENT(u, pkm, i),
                  _TCAM_STATE_FENT(u, pkm, i)));
#else
        ALPM_VERB(("**PFX = %d P = %d N = %d START = %d END = %d "
                   "VENT = %d FENT = %d\n", i,
                    _TCAM_STATE_PREV(u, pkm, i),
                    _TCAM_STATE_NEXT(u, pkm, i),
                    _TCAM_STATE_START(u, pkm, i),
                    _TCAM_STATE_END(u, pkm, i),
                    _TCAM_STATE_VENT(u, pkm, i),
                    _TCAM_STATE_FENT(u, pkm, i)));
#endif
    }
}

/*
 *      Create a slot for the new entry rippling the entries if required
 */
STATIC int
_tcam_entry_shift(int u, int pkm, int from_ent, int to_ent)
{
    int         rv;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];

    ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
    rv = _tcam_trie_update_by_ent(u, pkm, e, to_ent);
    if (BCM_SUCCESS(rv)) {
        rv = _tcam_entry_write(u, pkm, e, to_ent, from_ent);
    }

    return rv;
}

/*
 *      Shift prefix entries 1 entry UP, while preserving
 *      last half empty IPv4 entry if any.
 */
STATIC int
_tcam_shift_pfx_up(int u, int pfx, int pkm)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         from_ent;
    int         to_ent;
    uint32      v0, v1;
    int         rv = BCM_E_NONE;

    to_ent = _TCAM_STATE_END(u, pkm, pfx) + 1;

    if (pkm != ALPM_PKM_128) {
        pkm = _PFX_IS_PKM_64B(u, pfx);
    }

    if (pkm == ALPM_PKM_32B) {
        from_ent = _TCAM_STATE_END(u, pkm, pfx);
        ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, pkm, e, 0);
        v1 = tcam_entry_valid(u, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            _tcam_trie_update_by_ent(u, pkm, e, to_ent);
            ALPM_IER(_tcam_entry_write(u, pkm, e, to_ent, from_ent));
            to_ent--;
        }
    }

    from_ent = _TCAM_STATE_START(u, pkm, pfx);
    if (from_ent != to_ent) {
        ALPM_IER(_tcam_entry_shift(u, pkm, from_ent, to_ent));
    }
    _TCAM_STATE_START(u, pkm, pfx) += 1;
    _TCAM_STATE_END(u, pkm, pfx) += 1;

    return rv;
}

/*
 *      Shift prefix entries 1 entry DOWN, while preserving
 *      last half empty IPv4 entry if any.
 */
STATIC int
_tcam_shift_pfx_down(int u, int pfx, int pkm)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         from_ent;
    int         to_ent;
    int         prev_ent;
    uint32      v0, v1;
    int         rv = BCM_E_NONE;

    to_ent = _TCAM_STATE_START(u, pkm, pfx) - 1;

    /* Don't move empty prefix . */
    if (_TCAM_STATE_VENT(u, pkm, pfx) == 0) {
        _TCAM_STATE_START(u, pkm, pfx) = to_ent;
        _TCAM_STATE_END(u, pkm, pfx) = to_ent - 1;
        return BCM_E_NONE;
    }

    if (pkm != ALPM_PKM_128) {
        pkm = _PFX_IS_PKM_64B(u, pfx);
    }

    if (pkm == ALPM_PKM_32B &&
        _TCAM_STATE_END(u, pkm, pfx) != _TCAM_STATE_START(u, pkm, pfx)) {

        from_ent = _TCAM_STATE_END(u, pkm, pfx);

        ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, pkm, e, 0);
        v1 = tcam_entry_valid(u, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            /* Shift entry before last to start - 1 position. */
            prev_ent = from_ent - 1;
            ALPM_IER(_tcam_entry_shift(u, pkm, prev_ent, to_ent));

            _tcam_trie_update_by_ent(u, pkm, e, prev_ent);
            ALPM_IER(_tcam_entry_write(u, pkm, e, prev_ent, from_ent));
        } else {
            /* Last entry is full -> just shift it to start - 1  position. */
            _tcam_trie_update_by_ent(u, pkm, e, to_ent);
            ALPM_IER(_tcam_entry_write(u, pkm, e, to_ent, from_ent));
        }

    } else  {
        from_ent = _TCAM_STATE_END(u, pkm, pfx);
        ALPM_IER(_tcam_entry_shift(u, pkm, from_ent, to_ent));
    }
    _TCAM_STATE_START(u, pkm, pfx) -= 1;
    _TCAM_STATE_END(u, pkm, pfx) -= 1;

    return rv;
}

/*
 *      Create a slot for the new entry rippling the entries if required
 *      returned update=1 if half-free entry, update=0 if full-free entry.
 */
STATIC int
_tcam_free_slot_create(int u, int pfx, int pkm, int *free_slot, int *update)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         prev_pfx;
    int         next_pfx;
    int         free_pfx;
    int         curr_pfx;
    int         from_ent;
    uint32      v0, v1;

    if (_TCAM_STATE_VENT(u, pkm, pfx) == 0) {
        /*
         * Find the  prefix position. Only prefix with valid
         * entries are in the list.
         * next -> high to low prefix. low to high index
         * prev -> low to high prefix. high to low index
         * Unused prefix length _PFX_MAX_INDEX is the head of the
         * list and is node corresponding to this is always
         * present.
         */
        curr_pfx = _TCAM_PFX_MAX_IDX(u, pkm);
        if (ALPM_TCAM_ZONED(u)) {
            if (pfx <= _TCAM_PFX_MAX_VRF_IDX(u, pkm)) {
                curr_pfx = _TCAM_PFX_MAX_VRF_IDX(u, pkm);
            }
        }
        while (_TCAM_STATE_NEXT(u, pkm, curr_pfx) > pfx) {
            curr_pfx = _TCAM_STATE_NEXT(u, pkm, curr_pfx);
        }
        /* Insert the new prefix */
        next_pfx = _TCAM_STATE_NEXT(u, pkm, curr_pfx);
        if (next_pfx != -1) {
            _TCAM_STATE_PREV(u, pkm, next_pfx) = pfx;
        }
        _TCAM_STATE_NEXT(u, pkm, pfx) = _TCAM_STATE_NEXT(u, pkm, curr_pfx);
        _TCAM_STATE_PREV(u, pkm, pfx) = curr_pfx;
        _TCAM_STATE_NEXT(u, pkm, curr_pfx) = pfx;

        _TCAM_STATE_FENT(u, pkm, pfx) =
            (_TCAM_STATE_FENT(u, pkm, curr_pfx) + 1) / 2;
        _TCAM_STATE_FENT(u, pkm, curr_pfx) -= _TCAM_STATE_FENT(u, pkm, pfx);
        _TCAM_STATE_START(u, pkm, pfx) = _TCAM_STATE_END(u, pkm, curr_pfx) +
                                    _TCAM_STATE_FENT(u, pkm, curr_pfx) + 1;
        _TCAM_STATE_END(u, pkm, pfx) = _TCAM_STATE_START(u, pkm, pfx) - 1;
        _TCAM_STATE_VENT(u, pkm, pfx) = 0;
    } else if (pkm == ALPM_PKM_32B) {
        /* For IPv4 Check if alternate entry is free */
        from_ent = _TCAM_STATE_START(u, pkm, pfx);
        ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, pkm, e, 0);
        v1 = tcam_entry_valid(u, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
            *update = 1; /* half-free entry */
            return BCM_E_NONE;
        }

        from_ent = _TCAM_STATE_END(u, pkm, pfx);
        ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, pkm, e, 0);
        v1 = tcam_entry_valid(u, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
            *update = 1; /* half-free entry */
            return BCM_E_NONE;
        }
    }

    free_pfx = pfx;
    while (_TCAM_STATE_FENT(u, pkm, free_pfx) == 0) {
        free_pfx = _TCAM_STATE_NEXT(u, pkm, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on this side try the other side */
            free_pfx = pfx;
            break;
        }
    }

    while (_TCAM_STATE_FENT(u, pkm, free_pfx) == 0) {
        free_pfx = _TCAM_STATE_PREV(u, pkm, free_pfx);
        if (free_pfx == -1) {
            if (_TCAM_STATE_VENT(u, pkm, pfx) == 0) {
                /* We failed to allocate entries for a newly allocated prefix.*/
                prev_pfx = _TCAM_STATE_PREV(u, pkm, pfx);
                next_pfx = _TCAM_STATE_NEXT(u, pkm, pfx);
                if (-1 != prev_pfx) {
                    _TCAM_STATE_NEXT(u, pkm, prev_pfx) = next_pfx;
                }
                if (-1 != next_pfx) {
                    _TCAM_STATE_PREV(u, pkm, next_pfx) = prev_pfx;
                }
            }
            return BCM_E_FULL;
        }
    }

    /*
     * Ripple entries to create free space
     */
    while (free_pfx > pfx) {
        next_pfx = _TCAM_STATE_NEXT(u, pkm, free_pfx);
        ALPM_IER(_tcam_shift_pfx_down(u, next_pfx, pkm));
        _TCAM_STATE_FENT(u, pkm, free_pfx) -= 1;
        _TCAM_STATE_FENT(u, pkm, next_pfx) += 1;
        free_pfx = next_pfx;
    }

    while (free_pfx < pfx) {
        ALPM_IER(_tcam_shift_pfx_up(u, free_pfx, pkm));
        _TCAM_STATE_FENT(u, pkm, free_pfx) -= 1;
        prev_pfx = _TCAM_STATE_PREV(u, pkm, free_pfx);
        _TCAM_STATE_FENT(u, pkm, prev_pfx) += 1;
        free_pfx = prev_pfx;
    }

    _TCAM_STATE_VENT(u, pkm, pfx) += 1;
    _TCAM_STATE_FENT(u, pkm, pfx) -= 1;
    _TCAM_STATE_END(u, pkm, pfx) += 1;

    *free_slot = _TCAM_STATE_END(u, pkm, pfx) << ((pkm) ? 0 : 1);
    *update = 0; /* full-free entry */

    return BCM_E_NONE;
}

/*
 * Delete a slot and adjust entry pointers if required.
 */
STATIC int
_tcam_free_slot_delete(int u, int pfx, int pkm, int slot)
{
    int         prev_pfx, next_pfx;
    int         fidx, tidx;
    uint32      ef[SOC_MAX_MEM_FIELD_WORDS];
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    void        *et;
    int         rv = BCM_E_NONE;

    fidx    = _TCAM_STATE_END(u, pkm, pfx);
    tidx    = slot;

    if (pkm == ALPM_PKM_32B) {
        tidx >>= 1;
        ALPM_IER(tcam_entry_read(u, pkm, ef, fidx, fidx));
        if (fidx != tidx) {
            ALPM_IER(tcam_entry_read(u, pkm, e, tidx, tidx));
            et = e;
        } else {
            et = ef;
        }

        if (tcam_entry_valid(u, pkm, ef, 1)) {
            rv = _tcam_entry_x_to_y(u, pkm, ef, et, TRUE, 1, slot & 1);
            (void)_tcam_entry_valid_set(u, pkm, ef, 1, 0);
        } else {
            rv = _tcam_entry_x_to_y(u, pkm, ef, et, TRUE, 0, slot & 1);
            (void)_tcam_entry_valid_set(u, pkm, ef, 0, 0);
            _TCAM_STATE_VENT(u, pkm, pfx) -= 1;
            _TCAM_STATE_FENT(u, pkm, pfx) += 1;
            _TCAM_STATE_END(u, pkm, pfx) -= 1;
        }

        /* not true for OVERRIDE routes */
        if (tidx != fidx) {
            ALPM_IER(_tcam_trie_update_by_ent(u, pkm, et, tidx));
            ALPM_IER(_tcam_entry_write(u, pkm, et, tidx, tidx));
        }
        ALPM_IER(_tcam_trie_update_by_ent(u, pkm, ef, fidx));
        ALPM_IER(_tcam_entry_write(u, pkm, ef, fidx, fidx));
    } else { /* IPV6 */
        _TCAM_STATE_VENT(u, pkm, pfx) -= 1;
        _TCAM_STATE_FENT(u, pkm, pfx) += 1;
        _TCAM_STATE_END(u, pkm, pfx)  -= 1;
        if (tidx != fidx) {
            ALPM_IER(tcam_entry_read(u, pkm, ef, fidx, fidx));
            ALPM_IER(_tcam_trie_update_by_ent(u, pkm, ef, tidx));
            ALPM_IER(_tcam_entry_write(u, pkm, ef, tidx, fidx));
        }
        /* v6-64 indices are still raw in pivot tcam */
        sal_memset(ef, 0, sizeof(ef));
        ALPM_IER(_tcam_entry_write(u, pkm, ef, fidx, fidx));
    }

    if (_TCAM_STATE_VENT(u, pkm, pfx) == 0) {
        /* remove from the list */
        prev_pfx = _TCAM_STATE_PREV(u, pkm, pfx); /* Always present */
        assert(prev_pfx != -1);
        next_pfx = _TCAM_STATE_NEXT(u, pkm, pfx);
        _TCAM_STATE_NEXT(u, pkm, prev_pfx) = next_pfx;
        _TCAM_STATE_FENT(u, pkm, prev_pfx) += _TCAM_STATE_FENT(u, pkm, pfx);
        _TCAM_STATE_FENT(u, pkm, pfx) = 0;
        if (next_pfx != -1) {
            _TCAM_STATE_PREV(u, pkm, next_pfx) = prev_pfx;
        }
        _TCAM_STATE_NEXT(u, pkm, pfx) = -1;
        _TCAM_STATE_PREV(u, pkm, pfx) = -1;
        _TCAM_STATE_START(u, pkm, pfx) = -1;
        _TCAM_STATE_END(u, pkm, pfx) = -1;
    }

    return rv;
}

/*
 * _tcam_prefix_length_get (Extract vrf weighted
 * prefix lenght from the hw entry based on ip, mask & vrf)
 */
STATIC void
_tcam_prefix_length_get(int u, _bcm_defip_cfg_t *lpm_cfg, int *pfx_len)
{
    int     key_len;
    int     pkm = ALPM_LPM_PKM(u, lpm_cfg);
    int     ipv6_min, max_entries;

    ipv6_min = _TCAM_PFX_MIN_V6(u, pkm);
    max_entries = _TCAM_PFX_MAX_ENTS(u, pkm);

    key_len = lpm_cfg->defip_sub_len;
    if (pkm != ALPM_PKM_32B) {
        key_len += ipv6_min;
    }

    /* In ALPM the arragnement of VRF is at he begining followed by VRF
     * override and global */
    switch (lpm_cfg->defip_vrf) {
    case BCM_L3_VRF_GLOBAL:
        if (ALPM_TCAM_ZONED(u)) {
            *pfx_len = key_len + (max_entries / 3);
        } else {
            *pfx_len = key_len;
        }
        break;
    case BCM_L3_VRF_OVERRIDE:
        *pfx_len = key_len + 2 * (max_entries / 3);
        break;
    default:
        if (ALPM_TCAM_ZONED(u)) {
            *pfx_len = key_len;
        } else {
            *pfx_len = key_len + (max_entries / 3);
        }
        break;
    }
    return ;
}

/*
 * _tcam_match (Exact match for the key. Will match both IP address
 * and mask)
 *
 * OUT index_ptr: return key location
 * OUT pfx_len: Key prefix length. vrf+32+pfx len for single wide(64b)
 */
STATIC int
_tcam_match(int u, _bcm_defip_cfg_t *lpm_cfg,
            int *index_ptr, int *pfx_len)
{
    int         key_index;
    int         pfx = 0;

    /* Calculate vrf weighted prefix lengh. */
    _tcam_prefix_length_get(u, lpm_cfg, &pfx);
    if (pfx_len != NULL) {
        *pfx_len = pfx;
    }

    if (_tcam_trie_lookup(u, lpm_cfg, &key_index) == BCM_E_NONE) {
        *index_ptr = key_index;
        return BCM_E_NONE;
    } else {
        return BCM_E_NOT_FOUND;
    }
}

int
bcm_esw_alpm_tcam_avail(int u, int pkm, int vrf_id)
{
    int i;
    int max_pfx_len;

    if (!TCAMC(u) || !_TCAM_PKM_ATTACHED(u, pkm)) {
        return 0;
    }

    if (ALPM_TCAM_ZONED(u) && !ALPM_VRF_ID_IS_GBL(u, vrf_id)) {
        max_pfx_len = _TCAM_PFX_MAX_VRF_IDX(u, pkm);
    } else {
        max_pfx_len = _TCAM_PFX_MAX_IDX(u, pkm);
    }

    i = max_pfx_len;
    while (i >= 0) {
        if (_TCAM_STATE_FENT(u, pkm, i) > 0) {
            return 1;
        }

        i = _TCAM_STATE_NEXT(u, pkm, i);
    }

    return 0;
}

int
bcm_esw_alpm_tcam_state_free_get(int u, int pkm, int *free_cnt, int *used_cnt)
{
    int i;
    int max_pfx_len;
    int fcnt = 0, vcnt = 0;

    if (!TCAMC(u) || !_TCAM_PKM_ATTACHED(u, pkm)) {
        return BCM_E_NONE;
    }

    max_pfx_len = _TCAM_PFX_MAX_IDX(u, pkm);
    for (i = max_pfx_len; i >= 0 ; i--) {
        if ((i != _TCAM_PFX_MAX_IDX(u, pkm)) &&
            (_TCAM_STATE_START(u, pkm, i) == -1)) {
            continue;
        }

        fcnt += _TCAM_STATE_FENT(u, pkm, i);
        vcnt += _TCAM_STATE_VENT(u, pkm, i);
    }

    if (free_cnt) {
        *free_cnt = fcnt;
    }

    if (used_cnt) {
        *used_cnt = vcnt;
    }

    return BCM_E_NONE;
}

/*
 * De-initialize the start/end tracking pointers for each prefix length
 */
int
bcm_esw_alpm_tcam_deinit(int u)
{
    int i;

    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return BCM_E_UNAVAIL;
    }

    for (i = 0; i < ALPM_PKM_CNT; i++) {
        if (_TCAM_PKM_ATTACHED(u, i) &&
            _TCAM_STATE(u, i) != NULL) {
            alpm_util_free(_TCAM_STATE(u, i));
            _TCAM_STATE(u, i) = NULL;
        }
    }

    if (_TCAM_INIT_CHECK(u)) {
        alpm_util_free(TCAMC(u));
        TCAMC(u) = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Initialize the start/end tracking pointers for each prefix length
 */
int
bcm_esw_alpm_tcam_init(int u)
{
    int rv = BCM_E_NONE;
    int i, j, max_pfx_len;
    int tbl_sz;
    int pfx_st_sz;

    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return BCM_E_UNAVAIL;
    }

    if (_TCAM_INIT_CHECK(u)) {
        /* this is a reinit. clean up old state */
        if (bcm_esw_alpm_tcam_deinit(u) < 0) {
            return BCM_E_UNAVAIL;
        }
    }

    ALPM_ALLOC_EG(TCAMC(u), sizeof(_tcam_ctrl_t), "TCAMC");

    TCAMC(u)->tcam[ALPM_PKM_32B] = &_tcam_defip_ctrl;
    TCAMC(u)->tcam[ALPM_PKM_64B] = &_tcam_defip_ctrl;
    if (ALPMC(u)->_alpm_128b) {
        TCAMC(u)->tcam[ALPM_PKM_64B] = &_tcam_defip128_ctrl;
        TCAMC(u)->tcam[ALPM_PKM_128] = &_tcam_defip128_ctrl;
    }

    for (i = 0; i < ALPM_PKM_CNT; i++) {
        if (TCAMC(u)->tcam[i] == NULL || _TCAM_STATE(u, i) != NULL) {
            continue;
        }

        max_pfx_len = _TCAM_PFX_MAX_ENTS(u, i);
        pfx_st_sz = sizeof(_tcam_state_t) * max_pfx_len;
        ALPM_ALLOC_EG(_TCAM_STATE(u, i), pfx_st_sz, "LPM_pfx_info");
        for (j = 0; j < max_pfx_len; j++) {
            _TCAM_STATE_START(u, i, j)  = -1;
            _TCAM_STATE_END(u, i, j)    = -1;
            _TCAM_STATE_PREV(u, i, j)   = -1;
            _TCAM_STATE_NEXT(u, i, j)   = -1;
            _TCAM_STATE_VENT(u, i, j)   = 0;
            _TCAM_STATE_FENT(u, i, j)   = 0;
        }

        tbl_sz = tcam_table_size(u, i);

        ALPM_INFO(("tcam_init: pkm %d table sz: %d urpf %d zoned %d\n", i,
                   tbl_sz, SOC_URPF_STATUS_GET(u), ALPM_TCAM_ZONED(u)));

        if (ALPM_TCAM_ZONED(u)) {
            /* For Parallel search, the TCAM is hard paritioned.
             * First half are for VRF specific, the bottom half for override and
             * global. Start of Global entries start at the bottom half of TCAM
             */
            _TCAM_STATE_END(u, i, _TCAM_PFX_MAX_IDX(u, i)) = (tbl_sz >> 1) - 1;
            /* Free entries for VRF */
            _TCAM_STATE_FENT(u, i, _TCAM_PFX_MAX_VRF_IDX(u, i)) = tbl_sz >> 1;
            /* Remaining entries are assigned to Global entries */
            _TCAM_STATE_FENT(u, i, (_TCAM_PFX_MAX_IDX(u, i))) =
                (tbl_sz - _TCAM_STATE_FENT(u, i, _TCAM_PFX_MAX_VRF_IDX(u, i)));
        } else {
            /* Remaining entries are assigned to Global entries */
            _TCAM_STATE_FENT(u, i, (_TCAM_PFX_MAX_IDX(u, i))) = tbl_sz;
        }
    }
    return rv;

bad:
    (void)bcm_esw_alpm_tcam_deinit(u);
    return rv;
}

/*
 * Implementation using _mem_read/write using entry rippling technique
 * Advantage: A completely sorted table is not required. Lookups can be slow
 * as it will perform a linear search on the entries for a given prefix length.
 * No device access necessary for the search if the table is cached. Auxiliary
 * Small number of entries need to be moved around (97 worst case)
 * for performing insert/update/delete. However CPU needs to do all
 * the work to move the entries.
 */

/*
 * bcm_esw_alpm_tcam_insert
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 */
int
bcm_esw_alpm_tcam_insert(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int         pfx;
    int         index;
    int         rv = BCM_E_NONE;
    int         found = 0;
    int         update; /* for ALPM_PKM_32B only, update=1 needs read before write */

    rv = _tcam_match(u, lpm_cfg, &index, &pfx);
    if (rv == BCM_E_NOT_FOUND) {
        rv = _tcam_free_slot_create(u, pfx, ALPM_LPM_PKM(u, lpm_cfg),
                                    &index, &update);
    } else {
        found = 1;
        update = 1;
    }

    if (rv == BCM_E_NONE) {
        if (!found) {
            lpm_cfg->defip_index = index;
            /* pvt trie will be updated in alpm.c, no need to do
             * _tcam_trie_update_by_cfg(u, lpm_cfg, index); */
        }

        _tcam_state_dump(u, ALPM_LPM_PKM(u, lpm_cfg));
        ALPM_INFO(("**TCAM_INSERT: %d:0x%x/%d(%d) at index %d\n",
                   lpm_cfg->defip_vrf, lpm_cfg->defip_ip_addr,
                   lpm_cfg->defip_sub_len, pfx, index));

        rv = _tcam_cfg_write(u, lpm_cfg, index, index, update);
        if (!found) {
            if (BCM_SUCCESS(rv)) {
                ; /* VRF_PIVOT_REF_INC(u, vrf_id, vrf, pkm); */
            }
        }
    }

    return rv;
}

/*
 * bcm_esw_alpm_tcam_delete
 */
int
bcm_esw_alpm_tcam_delete(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int         pfx;
    int         index;
    int         rv = BCM_E_NONE;
    int         pkm = ALPM_LPM_PKM(u, lpm_cfg);

    rv = _tcam_match(u, lpm_cfg, &index, &pfx);
    if (rv == BCM_E_NONE) {
        ALPM_INFO(("**TCAM_DELETE: %d:0x%x/%d(%d) at index %d\n",
                   lpm_cfg->defip_vrf, lpm_cfg->defip_ip_addr,
                   lpm_cfg->defip_sub_len, pfx, index));
        lpm_cfg->defip_index = index;
        rv = _tcam_free_slot_delete(u, pfx, pkm, index);
    }

    if (BCM_SUCCESS(rv)) {
        ; /* VRF_PIVOT_REF_DEC(u, vrf_id, vrf, pkm); */
    }

    _tcam_state_dump(u, ALPM_LPM_PKM(u, lpm_cfg));

    return(rv);
}

/*
 * bcm_esw_alpm_tcam_match
 * (Exact match for the key. Will match both IP address and mask)
 */
int
bcm_esw_alpm_tcam_match(int u, _bcm_defip_cfg_t *lpm_cfg, int *index_ptr)
{
    int     pfx;
    int     rv;
    int     hw_idx;
    int     pkm = ALPM_LPM_PKM(u, lpm_cfg);
    uint32  e[SOC_MAX_MEM_FIELD_WORDS];

    rv = _tcam_match(u, lpm_cfg, index_ptr, &pfx);
    if (BCM_SUCCESS(rv)) {
        /*
         * If entry is ipv4 copy to the "zero" half of the,
         * buffer, "zero" half of lpm_entry if the  original entry
         * is in the "one" half.
         */
        hw_idx = *index_ptr;
        if (pkm == ALPM_PKM_32B) {
            hw_idx >>= 1;
        }

        ALPM_IER(tcam_entry_read(u, pkm, e, hw_idx, hw_idx));

        if (pkm == ALPM_PKM_32B) {
            rv = tcam_entry_to_cfg(u, pkm, e, (*index_ptr & 0x1), lpm_cfg);
        } else {
            rv = tcam_entry_to_cfg(u, pkm, e, 0, lpm_cfg);
        }
    }
    return rv;
}

/* ********************* *
 * TCAM warmboot support *
 * ********************* */

/*
 * Extract vrf weighted prefix lenght from pkm, vrf_id, key_len.
 */
STATIC void
tcam_pfx_len_get(int u, int pkm, int vrf_id, int key_len, int *pfx_len)
{
    int ipv6_min, max_entries;
    int vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);

    ipv6_min = _TCAM_PFX_MIN_V6(u, pkm);
    max_entries = _TCAM_PFX_MAX_ENTS(u, pkm);

    if (pkm != ALPM_PKM_32B) {
        key_len += ipv6_min;
    }

    /* In ALPM the arragnement of VRF is at he begining followed by VRF
       override and global */
    switch (vrf) {
    case BCM_L3_VRF_GLOBAL:
        if (ALPM_TCAM_ZONED(u)) {
            *pfx_len = key_len + (max_entries / 3);
        } else {
            *pfx_len = key_len;
        }
        break;
    case BCM_L3_VRF_OVERRIDE:
        *pfx_len = key_len + 2 * (max_entries / 3);
        break;
    default:
        if (ALPM_TCAM_ZONED(u)) {
            *pfx_len = key_len;
        } else {
            *pfx_len = key_len + (max_entries / 3);
        }
        break;
    }
    return;
}

/*
 * Function:
 *      tcam_wb_reinit_done
 * Purpose:
 *      Update all TCAM state once entry reinit done from warmboot.
 * Parameters:
 *      u        - (IN)Device unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
tcam_wb_reinit_done(int u)
{
    int idx;
    int prev_idx;
    int defip_table_size;
    int pkm;

    for (pkm = 0; pkm < ALPM_PKM_CNT; pkm++) {
        if (!_TCAM_PKM_ATTACHED(u, pkm)) {
            continue;
        }
        prev_idx = _TCAM_PFX_MAX_IDX(u, pkm);
        defip_table_size = tcam_table_size(u, pkm);

        if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED)) {
            /* Combined search mode */
            _TCAM_STATE_PREV(u, pkm, _TCAM_PFX_MAX_IDX(u, pkm)) = -1;

            for (idx = _TCAM_PFX_MAX_IDX(u, pkm); idx > -1; idx--) {
                if (_TCAM_STATE_START(u, pkm, idx) == -1) {
                    continue;
                }

                _TCAM_STATE_PREV(u, pkm, idx) = prev_idx;
                _TCAM_STATE_NEXT(u, pkm, prev_idx) = idx;

                _TCAM_STATE_FENT(u, pkm, prev_idx) =                    \
                                  _TCAM_STATE_START(u, pkm, idx) -      \
                                  _TCAM_STATE_END(u, pkm, prev_idx) - 1;
                prev_idx = idx;

            }

            _TCAM_STATE_NEXT(u, pkm, prev_idx) = -1;
            _TCAM_STATE_FENT(u, pkm, prev_idx) =                       \
                                  defip_table_size -                   \
                                  _TCAM_STATE_END(u, pkm, prev_idx) - 1;
        } else {
            /* Parallel search mode
             *
             *    Configured VRF Range
             *    {0 - (ALPM_MAX_PFX_INDEX/3)}
             *    Global VRF Range
             *    {((ALPM_MAX_PFX_INDEX/1) + 1) - (ALPM_MAX_PFX_INDEX/2) }
             *    Override VRF Range
             *    {((ALPM_MAX_PFX_INDEX/2) + 1) - (ALPM_MAX_PFX_INDEX) }
             */

            /*
             * Global and Overide VRF range are treated as single block and
             * both blocks will be linked
             */
            _TCAM_STATE_PREV(u, pkm, _TCAM_PFX_MAX_IDX(u, pkm)) = -1;
            for (idx = _TCAM_PFX_MAX_IDX(u, pkm);
                  idx > _TCAM_PFX_MAX_VRF_IDX(u, pkm); idx--) {
                if (-1 == _TCAM_STATE_START(u, pkm, idx)) {
                    continue;
                }

                _TCAM_STATE_PREV(u, pkm, idx) = prev_idx;
                _TCAM_STATE_NEXT(u, pkm, prev_idx) = idx;

                _TCAM_STATE_FENT(u, pkm, prev_idx) =                    \
                                  _TCAM_STATE_START(u, pkm, idx) -      \
                                  _TCAM_STATE_END(u, pkm, prev_idx) - 1;

                prev_idx = idx;
            }
            _TCAM_STATE_NEXT(u, pkm, prev_idx) = -1;
            _TCAM_STATE_FENT(u, pkm, prev_idx) =                       \
                                  defip_table_size -                   \
                                  _TCAM_STATE_END(u, pkm, prev_idx) - 1;

            /* Configured VRF range */
            prev_idx = _TCAM_PFX_MAX_VRF_IDX(u, pkm);
            _TCAM_STATE_PREV(u, pkm, _TCAM_PFX_MAX_VRF_IDX(u, pkm)) = -1;
            for (idx = _TCAM_PFX_MAX_VRF_IDX(u, pkm); idx > -1; idx--) {
                if (_TCAM_STATE_START(u, pkm, idx) == -1) {
                    continue;
                }

                _TCAM_STATE_PREV(u, pkm, idx) = prev_idx;
                _TCAM_STATE_NEXT(u, pkm, prev_idx) = idx;

                _TCAM_STATE_FENT(u, pkm, prev_idx) =                    \
                                  _TCAM_STATE_START(u, pkm, idx) -      \
                                  _TCAM_STATE_END(u, pkm, prev_idx) - 1;
                prev_idx = idx;
            }
            _TCAM_STATE_NEXT(u, pkm, prev_idx) = -1;
            _TCAM_STATE_FENT(u, pkm, prev_idx) =                         \
                                  (defip_table_size >> 1) -              \
                                  _TCAM_STATE_END(u, pkm, prev_idx) - 1;
        }

        _tcam_state_dump(u, pkm);
    } /* for pkm */

    return (BCM_E_NONE);
}

/*
 * Function:
 *      tcam_wb_reinit
 * Purpose:
 *      TCAM entry state reinit from warmboot.
 * Parameters:
 *      u        - (IN)Device unit number.
 *      pkm      - (In)Packing mode (32B, 64B, 128)
 *      vrf_id   - (In)VRF ID
 *      idx      - (In)Memory Index
 *      key_len  - (In)Key prefix length
 * Returns:
 *      BCM_E_XXX
 */
int
tcam_wb_reinit(int u, int pkm, int vrf_id, int idx, int key_len)
{
    int pfx_len;
    /* extract VRF weighted prefix length */
    tcam_pfx_len_get(u, pkm, vrf_id, key_len, &pfx_len);

    if (_TCAM_STATE_VENT(u, pkm, pfx_len) == 0) {
        _TCAM_STATE_START(u, pkm, pfx_len) = idx;
        _TCAM_STATE_END(u, pkm, pfx_len) = idx;
    } else {
        _TCAM_STATE_END(u, pkm, pfx_len) = idx;
    }

    _TCAM_STATE_VENT(u, pkm, pfx_len)++;

    return (BCM_E_NONE);
}

#endif /* ALPM_ENABLE */

