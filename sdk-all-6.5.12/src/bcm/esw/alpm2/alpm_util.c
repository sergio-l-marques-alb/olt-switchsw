/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $Id: alpm_util.c$
 * File:    alpm_util.c
 * Purpose: ALPM util (device independent implementation).
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

char *alpm_util_pkm_str[] = {
    "32B",
    "64B",
    "128"
};

char *alpm_util_acb_str[] = {
    "ACB.0",
    "ACB.1",
};

static uint32 alpm_mem_alloc_cnt;
static uint32 alpm_mem_free_cnt;
static uint32 alpm_mem_alloc_sz;

void *
alpm_util_alloc(unsigned int sz, char *s)
{
    void *rv = NULL;
    rv = sal_alloc(sz, s);
    if (rv != NULL) {
        alpm_mem_alloc_cnt++;
        alpm_mem_alloc_sz += sz;
    }

    return rv;
}

void
alpm_util_free(void *addr)
{
    if (addr != NULL) {
        alpm_mem_free_cnt++;
        return sal_free(addr);
    }
}

void
alpm_util_mem_stat_get(uint32 *alloc_cnt, uint32 *free_cnt)
{
    if (alloc_cnt) {
        *alloc_cnt = alpm_mem_alloc_cnt;
    }

    if (free_cnt) {
        *free_cnt = alpm_mem_free_cnt;
    }
}

void
alpm_util_mem_stat_clear()
{
    alpm_mem_alloc_cnt = 0;
    alpm_mem_free_cnt = 0;
    alpm_mem_alloc_sz = 0;
}

int
alpm_util_trie_max_key_len(int u, int pkm)
{
    int max_key_len;

    max_key_len = (pkm == ALPM_PKM_32B) ?
                       _MAX_KEY_LEN_48_ :
                       _MAX_KEY_LEN_144_;
    return max_key_len;
}

int
alpm_util_trie_max_split_len(int u, int pkm)
{
    int max_split_len = 24;

    max_split_len = ALPMC(u)->_alpm_spl[pkm];

    return max_split_len;
}

void
alpm_util_adata_cfg_to_trie(int unit, _bcm_defip_cfg_t *lpm_cfg,
                            _alpm_bkt_adata_t *bkt_adata)
{
    /* lpm_cfg->defip_ecmp_index is used to store nh_ecmp_idx */
    bkt_adata->defip_flags               = lpm_cfg->defip_flags;
    if (bkt_adata->defip_flags & BCM_L3_IPMC) {
        bkt_adata->defip_ecmp_index          = lpm_cfg->defip_mc_group;
    } else {
        bkt_adata->defip_ecmp_index          = lpm_cfg->defip_ecmp_index;
    }
    bkt_adata->defip_prio                = lpm_cfg->defip_prio;
    bkt_adata->defip_lookup_class        = lpm_cfg->defip_lookup_class;
    bkt_adata->defip_flex_ctr_pool       = lpm_cfg->defip_flex_ctr_pool;
    bkt_adata->defip_flex_ctr_mode       = lpm_cfg->defip_flex_ctr_mode;
    bkt_adata->defip_flex_ctr_base_id    = lpm_cfg->defip_flex_ctr_base_id;
    return ;
}

void
alpm_util_adata_trie_to_cfg(int unit, _alpm_bkt_adata_t *bkt_adata,
                            _bcm_defip_cfg_t *lpm_cfg)
{
    /* lpm_cfg->defip_ecmp_index is used to store nh_ecmp_idx */
    lpm_cfg->defip_flags               = bkt_adata->defip_flags;
    if (bkt_adata->defip_flags & BCM_L3_IPMC) {
        lpm_cfg->defip_mc_group            = bkt_adata->defip_ecmp_index;
    } else {
        lpm_cfg->defip_ecmp_index          = bkt_adata->defip_ecmp_index;
    }
    lpm_cfg->defip_prio                = bkt_adata->defip_prio;
    lpm_cfg->defip_lookup_class        = bkt_adata->defip_lookup_class;
    lpm_cfg->defip_flex_ctr_pool       = bkt_adata->defip_flex_ctr_pool;
    lpm_cfg->defip_flex_ctr_mode       = bkt_adata->defip_flex_ctr_mode;
    lpm_cfg->defip_flex_ctr_base_id    = bkt_adata->defip_flex_ctr_base_id;
    return ;
}

uint8
alpm_util_route_type_get(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    return soc_feature(u, soc_feature_alpm_flex_stat) ? 1 : 0;
}

int
alpm_util_route_type_check(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int pkm = ALPM_LPM_PKM(u, lpm_cfg);
    _alpm_cb_t     *acb;
    uint8 my_route_md;

    my_route_md = alpm_util_route_type_get(u, lpm_cfg);

    acb = ACB_BTM(u);
    if (ACB_VRF_INITED(u, acb, vrf_id, pkm) &&
        VRF_ROUTE_CNT(acb, vrf_id, pkm) > 0 &&
        ACB_VRF_DB_TYPE(u, acb, vrf_id, pkm) != my_route_md) {
        ALPM_ERR(("**ROUTE.CHECK:route mode flag conflict\n"));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
alpm_util_def_check(int u, _bcm_defip_cfg_t *lpm_cfg, int is_add)
{
    _alpm_cb_t     *acb;
    int vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int pkm = ALPM_LPM_PKM(u, lpm_cfg);

    /* For routes go to SRAM */
    /* combined search mode protection */
    acb = ACB_BTM(u);
    if (is_add) {
        if (VRF_ROUTE_CNT(acb, vrf_id, pkm) == 0 &&
            ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED) &&
            lpm_cfg->defip_vrf != BCM_L3_VRF_GLOBAL &&
            lpm_cfg->defip_sub_len != 0) {
            ALPM_ERR(("**DEF.CHECK: First route in VRF %d has to "
                      "be a default route in this mode\n", vrf_id));
            return BCM_E_PARAM;
        }
    } else {
        if (VRF_ROUTE_CNT(acb, vrf_id, pkm) > 1 &&
            ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED) &&
            lpm_cfg->defip_vrf != BCM_L3_VRF_GLOBAL &&
            lpm_cfg->defip_sub_len == 0) {
            ALPM_ERR(("**DEF.CHECK: Default route in VRF %d has to "
                      "be the last route to delete in this mode\n", vrf_id));
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

int
alpm_util_is_my_pivot(int u, uint32 *trie_key, uint32 trie_len,
                      uint32 *pvt_key, uint32 pvt_len)
{
    /* TD3ALPMTBD */
    /*     ALPM_IS_MY_PIVOT() */
    return BCM_E_NONE;
}

void
alpm_util_cfg_to_key(int u, _bcm_defip_cfg_t *lpm_cfg, uint32 *key)
{
    uint8  *ip6;
    uint32 pkm = ALPM_LPM_PKM(u, lpm_cfg);

    ip6 = lpm_cfg->defip_ip6_addr;
    if (pkm == ALPM_PKM_32B) {
        key[0] = lpm_cfg->defip_ip_addr;
    } else if (pkm == ALPM_PKM_64B) {
        key[0] = ip6[4] << 24 | ip6[5] << 16 | ip6[6] << 8 | ip6[7];
        key[1] = ip6[0] << 24 | ip6[1] << 16 | ip6[2] << 8 | ip6[3];
    } else if (pkm == ALPM_PKM_128) {
        key[0] = ip6[12] << 24 | ip6[13] << 16 | ip6[14] << 8 | ip6[15];
        key[1] = ip6[8] << 24 | ip6[9] << 16 | ip6[10] << 8 | ip6[11];
        key[2] = ip6[4] << 24 | ip6[5] << 16 | ip6[6] << 8 | ip6[7];
        key[3] = ip6[0] << 24 | ip6[1] << 16 | ip6[2] << 8 | ip6[3];
    }

    return ;
}

void
alpm_util_cfg_to_msk(int u, _bcm_defip_cfg_t *lpm_cfg, uint32 *msk)
{
    bcm_ip6_t ip6;
    uint32 pkm = ALPM_LPM_PKM(u, lpm_cfg);

    if (pkm == ALPM_PKM_32B) {
        msk[0] = BCM_IP4_MASKLEN_TO_ADDR(lpm_cfg->defip_sub_len);
    } else if (pkm == ALPM_PKM_64B) {
        bcm_ip6_mask_create(ip6, lpm_cfg->defip_sub_len);
        msk[0] = ip6[4] << 24 | ip6[5] << 16 | ip6[6] << 8 | ip6[7];
        msk[1] = ip6[0] << 24 | ip6[1] << 16 | ip6[2] << 8 | ip6[3];
    } else if (pkm == ALPM_PKM_128) {
        bcm_ip6_mask_create(ip6, lpm_cfg->defip_sub_len);
        msk[0] = ip6[12] << 24 | ip6[13] << 16 | ip6[14] << 8 | ip6[15];
        msk[1] = ip6[8] << 24 | ip6[9] << 16 | ip6[10] << 8 | ip6[11];
        msk[2] = ip6[4] << 24 | ip6[5] << 16 | ip6[6] << 8 | ip6[7];
        msk[3] = ip6[0] << 24 | ip6[1] << 16 | ip6[2] << 8 | ip6[3];
    }

    return ;
}

void
alpm_util_ipmask_apply(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    uint8 *ip6;
    bcm_ip_t mask4;
    bcm_ip6_t mask6;

    if (lpm_cfg->defip_flags & BCM_L3_IP6) {
        /* Unchanged byte count.    */
        int idx = lpm_cfg->defip_sub_len / 8;

        bcm_ip6_mask_create(mask6, lpm_cfg->defip_sub_len);
        ip6 = lpm_cfg->defip_ip6_addr;

        /* Apply subnet mask */
        if (idx < BCM_IP6_ADDRLEN) {
            ip6[idx] &= mask6[idx];
            for (idx++; idx < BCM_IP6_ADDRLEN; idx++) {
                ip6[idx] = 0;   /* Reset rest of bytes.     */
            }
        }
    } else {
        mask4 = BCM_IP4_MASKLEN_TO_ADDR(lpm_cfg->defip_sub_len);
        /* Apply subnet mask. */
        lpm_cfg->defip_ip_addr &= mask4;
    }

    return ;
}

void
alpm_util_bnk_range_print(int u, SHR_BITDCL *bnk_bmp, int bnk_sz,
                          int bnk_per_row, int start, int end)
{
    int i, k;

    assert(start >= 0);
    assert(end < bnk_sz);

    cli_out("%15s", "");
    for (k = 0; k < bnk_per_row; k++) {
        int val = (start + k) % 10;
        if (val == 0) {
            cli_out("%2d", (start + k) % 100);
        } else {
            cli_out(" %d", val);
        }
    }
    cli_out("\n");

    for (i = start; i <= end; i += bnk_per_row) {
        int row_end;

        cli_out("%15s", "");
        for (k = 0; k < bnk_per_row; k++) {
            cli_out("--");
        }
        cli_out("-\n");
        cli_out("BNK%05d-%05d |", i, i + bnk_per_row - 1);
        row_end = end > (i+bnk_per_row) ? i+bnk_per_row-1 : end;
        for (k = i; k <= row_end; k++) {
            cli_out("%s|", SHR_BITGET(bnk_bmp, k) ? "#" : " ");
        }
        cli_out("\n");
    }
    cli_out("%15s", "");
    for (k = 0; k < bnk_per_row; k++) {
        cli_out("--");
    }
    cli_out("-\n");

    return ;
}

void
alpm_util_fmt_tbl_util(int unit, char *buf)
{
    int i, pkm, p;
    int vcnt_all = 0;
    int cnt_all = 0;

    _alpm_cb_t *acb;
    _alpm_bkt_pool_conf_t *bpc;

    for (pkm = ALPM_PKM_32B; pkm < ALPM_PKM_CNT; pkm++) {
        int fcnt = 0, vcnt = 0;
        bcm_esw_alpm_tcam_state_free_get(unit, pkm, &fcnt, &vcnt);
        vcnt_all += vcnt;
        cnt_all += (fcnt + vcnt);
    }

    if (cnt_all != 0) {
        sal_sprintf(buf+strlen(buf), "L1:%d.%d%% ",
                    vcnt_all * 100 / cnt_all,
                    vcnt_all * 1000 / cnt_all % 10);
    }

    for (i = 0; i < ACB_CNT(unit); i++) {
        acb = ACB(unit, i);
        for (p = 0; p < ALPM_BKT_PID_CNT; p++) {
            bpc = ACB_BKT_POOL(acb, p);
            if (p > 0 && bpc == ACB_BKT_POOL(acb, p-1)) {
                continue;
            }
            sal_sprintf(buf+strlen(buf), "L%d.P%d:%d.%d%% ",
                        acb->acb_idx + 2, p,
                        BPC_BNK_USED(bpc) * 100 / BPC_BNK_CNT(bpc),
                        BPC_BNK_USED(bpc) * 1000 / BPC_BNK_CNT(bpc) % 10);
        }
    }
}

void
alpm_util_fmt_bkt_info(char *buf, _alpm_cb_t *acb, _alpm_bkt_info_t *bkt_info)
{
    int i;
    /* ACB.0 BNK%d[%d(%d %d)_%d(%d %d)_%d(%d %d)_%d(%d %d)] CNT[%d %d %d %d] */
    sal_sprintf(buf+strlen(buf), "ACB%d->", ACB_IDX(acb));
    sal_sprintf(buf+strlen(buf), "ROFS%d[", BI_ROFS(bkt_info));
    for (i = BI_ROFS(bkt_info); i < BI_ROFS(bkt_info) + ALPM_BPB_MAX; i++) {
        int bnk = i % ALPM_BPB_MAX;
        int fmt = bkt_info->bnk_fmt[bnk];
        if (!BI_BNK_IS_USED(bkt_info, bnk)) {
            continue;
        }
        sal_sprintf(buf+strlen(buf), "B%d(%d)(%d,%d/%d)_", bnk,
                    bkt_info->bnk_fmt[bnk],
                    ACB_FMT_PFX_LEN(acb, fmt),
                    _shr_popcount(bkt_info->vet_bmp[bnk]),
                    ACB_FMT_ENT_MAX(acb, fmt));
    }
    sal_sprintf(buf+strlen(buf), "]");
}

void
alpm_util_fmt_ipaddr(char buf[SAL_IPADDR_STR_LEN], ip_addr_t ipaddr)
{
    sal_sprintf(buf, "%d.%d.%d.%d",
            (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff,
            (ipaddr >> 8) & 0xff, ipaddr & 0xff);
}

void
alpm_util_fmt_ip6addr(char buf[IP6ADDR_STR_LEN], ip6_addr_t ipaddr)
{
    sal_sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
            (((uint16)ipaddr[0] << 8) | ipaddr[1]),
            (((uint16)ipaddr[2] << 8) | ipaddr[3]),
            (((uint16)ipaddr[4] << 8) | ipaddr[5]),
            (((uint16)ipaddr[6] << 8) | ipaddr[7]),
            (((uint16)ipaddr[8] << 8) | ipaddr[9]),
            (((uint16)ipaddr[10] << 8) | ipaddr[11]),
            (((uint16)ipaddr[12] << 8) | ipaddr[13]),
            (((uint16)ipaddr[14] << 8) | ipaddr[15]));
}

/* Get pivot index: idx & sub_idx (or ent)  */
int
alpm_util_pvt_idx_get(int u, _alpm_cb_t *acb, _alpm_pvt_node_t *pvt_node, int *idx, int *sub_idx)
{
    int pvt_idx;

    if (ACB_HAS_TCAM(acb)) {
        pvt_idx = PVT_IDX(pvt_node); /* tcam_idx */
        if (pvt_node->pkm == ALPM_PKM_32B) {
            *idx = pvt_idx >> 1;
            *sub_idx = pvt_idx & 0x01;
        } else {
            *idx = pvt_idx;
            *sub_idx = 0;
        }
    } else {
        /* Get upper CB bucket ent_idx for the pivot */
        ALPM_IER(alpm_pvt_ent_idx_get(u, acb, pvt_node, &pvt_idx));
        *idx = ALPM_TAB_IDX_GET(pvt_idx);
        *sub_idx = ALPM_IDX_TO_ENT(pvt_idx);
    }

    return BCM_E_NONE;
}

void
alpm_util_key_to_cfg(int u, int pkm, uint32 *key, _bcm_defip_cfg_t *lpm_cfg)
{
    uint8  *ip6 = lpm_cfg->defip_ip6_addr;

    if (pkm == ALPM_PKM_32B) {
        lpm_cfg->defip_ip_addr = key[0];
    } else if (pkm == ALPM_PKM_64B) {
        ip6[0]  = (uint8) (key[1] >> 24);
        ip6[1]  = (uint8) (key[1] >> 16 & 0xff);
        ip6[2]  = (uint8) (key[1] >> 8 & 0xff);
        ip6[3]  = (uint8) (key[1] & 0xff);
        ip6[4]  = (uint8) (key[0] >> 24);
        ip6[5]  = (uint8) (key[0] >> 16 & 0xff);
        ip6[6]  = (uint8) (key[0] >> 8 & 0xff);
        ip6[7]  = (uint8) (key[0] & 0xff);
    } else if (pkm == ALPM_PKM_128) {
        ip6[0]  = (uint8) (key[3] >> 24);
        ip6[1]  = (uint8) (key[3] >> 16 & 0xff);
        ip6[2]  = (uint8) (key[3] >> 8 & 0xff);
        ip6[3]  = (uint8) (key[3] & 0xff);
        ip6[4]  = (uint8) (key[2] >> 24);
        ip6[5]  = (uint8) (key[2] >> 16 & 0xff);
        ip6[6]  = (uint8) (key[2] >> 8 & 0xff);
        ip6[7]  = (uint8) (key[2] & 0xff);
        ip6[8]  = (uint8) (key[1] >> 24);
        ip6[9]  = (uint8) (key[1] >> 16 & 0xff);
        ip6[10] = (uint8) (key[1] >> 8 & 0xff);
        ip6[11] = (uint8) (key[1] & 0xff);
        ip6[12] = (uint8) (key[0] >> 24);
        ip6[13] = (uint8) (key[0] >> 16 & 0xff);
        ip6[14] = (uint8) (key[0] >> 8 & 0xff);
        ip6[15] = (uint8) (key[0] & 0xff);
    }
    return ;
}

/*
 * Function:
 *      alpm_util_len_to_mask
 * Purpose:
 *      Convert IPv4 or IPv6 key length to mask.
 * Parameters:
 *      pkm      - (IN)Packing mode (32B, 64B, 128)
 *      len      - (IN)Key length
 *      mask     - (OUT)Key mask
 */
void
alpm_util_len_to_mask(int pkm, uint32 len, uint32 *mask)
{
    int i;
    int cnt = ALPM_KEY_ENT_CNT(pkm);

    for (i = 0; i < cnt; i++) { /* initialize all mask to 0 */
        mask[i] = 0;
    }

    for (i = cnt - 1; i >= 0; i--) {
        if (len <= 32) {
            break;
        }
        mask[i] = 0xffffffff;
        len -= 32;
    }
    mask[i] = ~_SHIFT_RIGHT(0xffffffff, len);
    return;
}

/* Convert ALPM ASSOC_DATA and KEY info into lpm_cfg */
int
alpm_util_cfg_construct(int u, int pkm, int vrf_id, uint32 *key, int key_len,
                        _alpm_bkt_adata_t *adata, _bcm_defip_cfg_t *lpm_cfg)
{
    sal_memset(lpm_cfg, 0, sizeof(_bcm_defip_cfg_t));
    alpm_util_adata_trie_to_cfg(u, adata, lpm_cfg);
    alpm_util_key_to_cfg(u, pkm, key, lpm_cfg);
    lpm_cfg->defip_sub_len = key_len;
    lpm_cfg->defip_vrf = ALPM_VRF_ID_TO_VRF(u, vrf_id);
    /* TD3ALPMTBD, defip_flags is not determined by pkm */
    lpm_cfg->defip_flags |= ((pkm) == ALPM_PKM_32B ? 0 : BCM_L3_IP6);
    if (lpm_cfg->defip_flags & BCM_L3_MULTIPATH) {
        lpm_cfg->defip_ecmp = 1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      alpm_util_route_capacity_get
 * Purpose:
 *      Get the min or max capacity for ALPM routes from tables:
 * L3_DEFIP_ALPM_IPV4, L3_DEFIP_ALPM_IPV6_64, L3_DEFIP_ALPM_IPV6_128
 *
 * Parameters:
 *      u           - Device unit
 *      mem         - Legacy memory type
 *      max_entries - Maximum result returned.
 *      min_entries - Minimum result returned.
 */
int
alpm_util_route_capacity_get(int u, soc_mem_t mem,
                             int *max_entries, int *min_entries)
{
    int rv = BCM_E_NONE;

    if (max_entries == NULL && min_entries == NULL) {
        return BCM_E_PARAM;
    }

    rv = ALPM_DRV(u)->alpm_cap_get(u, mem, max_entries, min_entries);

    return rv;
}

int
alpm_util_bkt_info_get(int u, int vrf_id, int pkm, _alpm_cb_t *acb, void *e,
                       int sub_idx, _alpm_bkt_info_t *bkt_info,
                       int *kshift, int *def_miss)
{
    int rv = BCM_E_UNAVAIL;
    _alpm_ent_info_t info;

    sal_memset(&info, 0, sizeof(info));
    info.action_mask = ALPM_INFO_MASK_ALPM_DATA;
    info.vrf_id = vrf_id;
    info.pkm = pkm;
    info.ent_fmt = sub_idx;

    rv = ALPM_DRV(u)->alpm_ent_selective_get(u, acb, e, &info);
    if (BCM_SUCCESS(rv)) {
        sal_memcpy(bkt_info, &info.bkt_info, sizeof(_alpm_bkt_info_t));
        *kshift = info.kshift;
        *def_miss = info.default_miss;
    }
    return rv;
}

int
alpm_util_bkt_token_bnks(int u, _alpm_cb_t *acb, int vrf_id,
                         _alpm_pvt_node_t *pvt_node)
{
    int used_cnt = 0;
    int i, bnk_pbk;
    SHR_BITDCL *bnk_bmp;
    _alpm_bkt_pool_conf_t *bp_conf;
    _alpm_pvt_node_t *tmp_node;

    bp_conf = ACB_BKT_VRF_POOL(acb, vrf_id);
    bnk_bmp = BPC_BNK_BMP(bp_conf);
    bnk_pbk = BPC_BNK_PER_BKT(bp_conf);

    for (i = PVT_BKT_IDX(pvt_node) * bnk_pbk;
         i < (PVT_BKT_IDX(pvt_node) + 1) * bnk_pbk; i++) {
        if (SHR_BITGET(bnk_bmp, i)) {
            int tab_idx =
                ALPM_TAB_IDX_GET_BKT_BNK(acb, 0, i / bnk_pbk, i % bnk_pbk);
            tmp_node =
                (_alpm_pvt_node_t *)ACB_VRF_PVT_PTR(acb, vrf_id, tab_idx);
            if (tmp_node && tmp_node != pvt_node) {
                used_cnt ++;
            }
        }
    }

    return used_cnt;
}

int
alpm_util_ent_ent_get(int u, _alpm_cb_t *acb, void *e,
                      uint32 fmt, int eid, void *entry)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->alpm_ent_ent_get(u, acb, e, fmt, eid, entry);
    return rv;
}

void
alpm_util_pfx_cat(int u, int pkm, uint32 *pfx1, int len1, uint32 *pfx2, int len2,
                  uint32 *new_pfx, int *new_len)
{
    int dst_offset;
    int max_pfx_len[] = {32, 64, 128};

    *new_len = len1 + len2;
    dst_offset = max_pfx_len[pkm] - *new_len;
    sal_memcpy(new_pfx, pfx1, sizeof(uint32) * 4);
    SHR_BITCOPY_RANGE(new_pfx, dst_offset, pfx2, 0, len2);
}

/*
 * Function:
 *      alpm_util_pfx_len_cmp
 * Purpose:
 *      Compare two prefix length.
 * Returns:
 *      a<=>b
 */
int
alpm_util_pfx_len_cmp(void *a, void *b)
{
    _alpm_bkt_node_t **first;
    _alpm_bkt_node_t **second;

    first = (_alpm_bkt_node_t **) a;
    second = (_alpm_bkt_node_t **) b;

    if ((*first)->key_len < (*second)->key_len) {
        return 1;
    } else if ((*first)->key_len > (*second)->key_len) {
        return -1;
    }
    return 0;
}

int
alpm_util_bkt_pfx_get(int u, _alpm_cb_t *acb, void *e, uint32 fmt,
                      uint32 *new_key, int *new_len, uint32 *valid)
{
    int rv = BCM_E_UNAVAIL;
    _alpm_ent_info_t info;

    sal_memset(&info, 0, sizeof(info));
    info.action_mask = ALPM_INFO_MASK_VALID | ALPM_INFO_MASK_KEYLEN;
    info.ent_fmt = fmt;

    rv = ALPM_DRV(u)->alpm_ent_selective_get(u, acb, e, &info);
    if (BCM_SUCCESS(rv)) {
        *valid = info.ent_valid;
        sal_memcpy(new_key, info.key, sizeof(uint32) * 4);
        *new_len = info.key_len;
    }
    return rv;
}

int
alpm_util_bkt_adata_get(int u, int pkm, _alpm_cb_t *acb, void *e,
                        int fmt, _alpm_bkt_adata_t *adata, int *arg1)
{
    int rv = BCM_E_UNAVAIL;
    _alpm_ent_info_t info;

    sal_memset(&info, 0, sizeof(info));
    info.action_mask = ALPM_INFO_MASK_ASSOC_DATA;
    info.ent_fmt = fmt;
    info.pkm = pkm;

    rv = ALPM_DRV(u)->alpm_ent_selective_get(u, acb, e, &info);
    if (BCM_SUCCESS(rv)) {
        sal_memcpy(adata, &info.adata, sizeof(_alpm_bkt_adata_t));
        if (arg1 != NULL) {
            *arg1 = BI_SUB_BKT_IDX(&info.bkt_info);
        }
    }
    return rv;
}

int
alpm_util_ent_data_get(int u, int pkm, _alpm_cb_t *acb, void *e,
                       int fmt, void *fent1)
{

    int rv = BCM_E_UNAVAIL;
    _alpm_ent_info_t info;

    sal_memset(&info, 0, sizeof(info));
    info.action_mask = ALPM_INFO_MASK_ALPM_DATA_RAW;
    info.alpm_data_raw = fent1;
    info.ent_fmt = fmt;
    info.pkm = pkm;

    rv = ALPM_DRV(u)->alpm_ent_selective_get(u, acb, e, &info);
    return rv;
}

int
alpm_util_ent_phy_idx_get(int u, _alpm_cb_t *acb, int pkm, int vrf_id, int index)
{
    int phy_index;
    _alpm_tbl_t tbl;

    tbl = ACB_BKT_TBL(acb, vrf_id, pkm);
    phy_index = ALPM_DRV(u)->mem_ent_phy_idx_get(u, acb, tbl, index);

    return phy_index;
}

int
tcam_table_size(int u, int pkm)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_table_sz(u, pkm);
    return rv;
}

int
tcam_valid_entry_mode_get(int u, int pk, void *e, int *step_size,
                    int *pkm, int *ipv6, int *key_mode, int sub_idx)
{
    int rv = BCM_E_UNAVAIL;
    *step_size = 2; /* default step_size */
    rv = ALPM_DRV(u)->tcam_entry_mode_get(u, pk, e, step_size, pkm,
                                          ipv6, key_mode, sub_idx);
    return rv;
}

int
tcam_entry_adata_get(int u, int pkm, void *e,
                     int sub_idx, _alpm_bkt_adata_t *adata)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_entry_adata_get(u, pkm, e, sub_idx, adata);
    return rv;
}

int
tcam_entry_bdata_get(int u, int pkm, void *e, int sub_idx,  void *fent)
{
    int rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_entry_bdata_get(u, pkm, e, sub_idx, fent);
    return rv;
}

int
alpm_bkt_entry_read(int u, _alpm_tbl_t tbl, _alpm_cb_t *acb, void *e, int index)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->mem_entry_read(u, acb, tbl, index, e, FALSE);

    return rv;
}

int
alpm_bkt_entry_read_no_cache(int u, _alpm_tbl_t tbl, _alpm_cb_t *acb, void *e, int index)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->mem_entry_read(u, acb, tbl, index, e, TRUE);

    return rv;
}

#endif /* ALPM_ENABLE */

